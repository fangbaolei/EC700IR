#include "CAMNVCUdpTransmit.h"
#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

CCAMNVCUdpTransmit::CCAMNVCUdpTransmit()
    : m_fIsTransmitting(FALSE)
{
}

CCAMNVCUdpTransmit::~CCAMNVCUdpTransmit()
{
}

int CCAMNVCUdpTransmit::Run()
{
    m_fIsTransmitting = FALSE;

    if (InitListen() == 0)
        return 0;

    m_cTransmit.SetStartupCallback(CallBack_StartupCAMNVC, this);
    m_cTransmit.SetRecvCallback(CallBack_RecvNVC, this);
    m_cTransmit.SetSendCallback(CallBack_Send, this);
    m_cTransmit.SetTransmitDataCallback(CallBack_TransmitDataNVC, this);
    m_cTransmit.SetGetTransmitDataCallback(CallBack_GetTransmitDataNVC, this);
    SetClientCominCallBack(CallBack_ClientComin, this);
    SetClientExitCallBack(CallBack_ClientExit, this);

    return m_cTransmit.Run();
}

int CCAMNVCUdpTransmit::CallBack_StartupCAMNVC(void* pUserData)
{
    int nResult = CallBack_Startup(pUserData);
    return nResult;
}

int CCAMNVCUdpTransmit::CallBack_RecvNVC(char* pBuf, int* pLen, void* pUserData)
{
    PRINTF("CallBack_Recv\n");
    CCAMNVCUdpTransmit* pThis = (CCAMNVCUdpTransmit*)pUserData;

    sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);

    pThis->CheckLinkTimeOut();
    pThis->m_sCurTransInfo.fd = -1;   // will get a fd, if recv succ

    int iRet = 0;

    SocketWait(pThis->m_fdSockListen, true, false, 10);

    int iRecvLen = recvfrom(pThis->m_fdSockListen, pBuf, *pLen, 0, (sockaddr*)&addr, &addrlen);
    if (iRecvLen < 0)
    {
        if (errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN)
        {
            PRINTF("udp listen no data\n");
            iRet = 1;
        }
    }
    else if (iRecvLen == 0)
    {
        PRINTF("iRecvLen = 0\n");
    }
    else
    {
        *pLen = iRecvLen;
        iRet = 2;   // continue recv after send
    }

    if ( iRet == 2 ) // get data
    {
        PRINTF("udp recv %d, from wPort %d\n", iRecvLen, ntohs(addr.sin_port));
        unsigned long ulIP = ntohl(addr.sin_addr.s_addr);
        unsigned short wPort = ntohs(addr.sin_port);

        // Check and create link
        pThis->CheckLink(ulIP, wPort);
        iRet = 1;
    }

    return iRet;
}

int CCAMNVCUdpTransmit::CallBack_GetTransmitDataNVC(char* pBuf, int* pLen, void* pUserData)
{
    PRINTF("CallBack_GetTransmitData\n");
    CCAMNVCUdpTransmit* pThis = (CCAMNVCUdpTransmit*)pUserData;

    sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);

    pThis->m_sCurTransInfo.fd = -1; // will get a fd, if get succ

    if (pThis->m_SockInfoMap.size() == 0)
        return 1;

    int iRet = pThis->SelectTransmitData(&pThis->m_sCurTransInfo.fd, 10);
    if ( iRet != 2 )  // if iRet is 2, get ready fd
    {
        PRINTF("select iRet %d\n", iRet);
        return iRet;
    }

    iRet = 0;

    int iRecvLen = recvfrom(pThis->m_sCurTransInfo.fd, pBuf, *pLen, 0, (sockaddr*)&addr, &addrlen);
    if (iRecvLen < 0)
    {
        if (errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN)
        {
            PRINTF("udp do not get transmit data\n");
            iRet = 1;
        }
        else
        {
            PRINTF("recv err %d\n", errno);

            pThis->m_SockInfoMap.erase(pThis->m_sCurTransInfo.fd);

            close(pThis->m_sCurTransInfo.fd);
            pThis->m_sCurTransInfo.fd = -1;
            iRet = 1;
        }
    }
    else if (iRecvLen == 0)
    {
        PRINTF("recvfrom 0, conn exit\n");
    }
    else
    {
        *pLen = iRecvLen;
        iRet = 2;   // continue recv after send
        pThis->m_fIsTransmitting = TRUE;
    }

    if ( iRet == 2 ) // get data
    {
        PRINTF("recvfrom %d\n", iRecvLen);
        std::map<int, STransInfo>::iterator itor = pThis->m_SockInfoMap.find(pThis->m_sCurTransInfo.fd);

        if (itor != pThis->m_SockInfoMap.end())
        {
            itor->second.tLastTransTime = time(0);

            pThis->m_sCurTransInfo.ulIP = itor->second.ulIP;
            pThis->m_sCurTransInfo.wPort = itor->second.wPort;
            pThis->m_sCurTransInfo.wTransmitPort = itor->second.wTransmitPort;
            pThis->m_wNVCDataPort = ntohs(addr.sin_port);
        }
        else
        {
            pThis->m_sCurTransInfo.fd = -1;
        }

        // NVC数据一来一回，没有更多数据，不继续接就发以优化
        iRet = 1;
    }

    return iRet;
}

int CCAMNVCUdpTransmit::CallBack_TransmitDataNVC(char* buf, int* pLen, void* pUserData)
{
    PRINTF("CallBack_TransmitData\n");
    CCAMNVCUdpTransmit* pThis = (CCAMNVCUdpTransmit*)pUserData;
    if ((pThis->m_sCurTransInfo.fd == -1) || (*pLen == 0))
        return 1;

    sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);
    addr.sin_family = AF_INET;

    // use data port
    if ((*pLen == 4) || (*pLen == 7))
        addr.sin_port = htons(pThis->m_wNVCDataPort);
    else
        addr.sin_port = htons(pThis->m_wTransPort);

    addr.sin_addr.s_addr = htonl(pThis->m_ulTransIP);

    int iSendLen = sendto(pThis->m_sCurTransInfo.fd, buf, *pLen, 0, (sockaddr*)&addr, addrlen);

    if ((iSendLen == 0) || (iSendLen < 0) && (errno != EINTR && errno != EWOULDBLOCK && errno != EAGAIN))
    {
        pThis->m_SockInfoMap.erase(pThis->m_sCurTransInfo.fd);
        close(pThis->m_sCurTransInfo.fd);
        pThis->m_sCurTransInfo.fd = -1;
    }

    PRINTF("trans to %d. %d\n", pThis->m_wTransPort, iSendLen);

    return 1;
}
