#include "CUdpTransmit.h"
#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <time.h>

#define PORT_TRANS_START 20000

CUdpTransmit::CUdpTransmit()
{
    m_fIsClientComin = FALSE;
    m_lastChkConnTime = 0;
}

CUdpTransmit::~CUdpTransmit()
{

}

void CUdpTransmit::SetClientCominCallBack(CALLBACK_TRANSMIT_STARTUP_FUNC func, void* pUserData)
{
    m_funcClientCominCallBack = func;
    m_pClientCominCallBackUserData = pUserData;
}

void CUdpTransmit::SetClientExitCallBack(CALLBACK_TRANSMIT_STARTUP_FUNC func, void* pUserData)
{
    m_funcClientExitCallBack = func;
    m_pClientExitCallBackUserData = pUserData;
}

int CUdpTransmit::SelectTransmitData(int* pFdReady, int timeoUS)
{
    fd_set rfds;
    struct timeval tv;

    FD_ZERO(&rfds);

    int maxfd = 0;
    std::map<int, STransInfo>::iterator itor;
    for (itor=m_SockInfoMap.begin();
            itor!=m_SockInfoMap.end();
            itor++)
    {
        FD_SET(itor->second.fd, &rfds);

        if (maxfd < itor->second.fd)
            maxfd = itor->second.fd;
    }

    tv.tv_sec = timeoUS / 1000000; //seconds
    tv.tv_usec = timeoUS % 1000000; //us

    switch (select(maxfd+1, &rfds, NULL, NULL, timeoUS==-1 ? NULL : &tv))
    {
    case 0: /*time out*/
        return 1;

    case (-1): /*socket error*/
                    // 1 有错但不是EINTR
                    return ( errno == EINTR ) ? 1 : 0;

    default:
{
        std::map<int, STransInfo>::iterator itor;
        for (itor=m_SockInfoMap.begin();
                itor!=m_SockInfoMap.end();
                itor++ )
        {
            if (FD_ISSET(itor->second.fd, &rfds))
            {
                *pFdReady = itor->second.fd;
                break;
            }
        }

        // if get ready fd, return 2, else return 1;
        return (itor!=m_SockInfoMap.end()) ? 2 : 1;
    }

    }
}
int CUdpTransmit::CreateUdpSock(unsigned long ulIP, unsigned short wPort)
{
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1)
    {
        PRINTF("CUdpTransmit CreateUdpSock socket error!\n");
        return -1;
    }
    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(wPort);
    addr.sin_addr.s_addr = htonl(ulIP);

    int iOpt = 1;
    int iOptLen = sizeof(iOpt);
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char*)&iOpt, iOptLen);
    setsockopt(fd, SOL_SOCKET, SO_BROADCAST, (char*)&iOpt, iOptLen);

    int iRet = -1;
    iRet = bind(fd, (struct sockaddr *) &addr, sizeof(sockaddr));
    if (iRet == -1)
    {
        PRINTF("CUdpTransmit CreateUdpSock bind error!\n");
        close(fd);
    }

    if (iRet != -1)  iRet = fd;

    return iRet;
}

int CUdpTransmit::SetListen(unsigned long ulBindIP, unsigned short wBindPort)
{
    m_ulListenIP = ulBindIP;
    m_ulListenPort = wBindPort;

    return 1;
}

int CUdpTransmit::SetTransmit(unsigned long ulTransIP,
                              unsigned short wTransPort,
                              unsigned long ulBindIP)
{
    m_ulTransIP = ulTransIP;
    m_wTransPort = wTransPort;
    m_ulTransBindIP = ulBindIP;
    return 1;
}

int CUdpTransmit::InitListen()
{
    struct sockaddr_in addr;
    int addr_len = sizeof(struct sockaddr_in);
    int iFlags;

    m_fdSockListen = socket(AF_INET, SOCK_DGRAM, 0);

    if (m_fdSockListen == -1)
    {
        PRINTF("CUdpTransmit socket error!\n");
        return 0;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(m_ulListenPort);
    addr.sin_addr.s_addr = htonl(m_ulListenIP);

    int iOpt = 1;
    int iOptLen = sizeof(iOpt);
    setsockopt(m_fdSockListen, SOL_SOCKET, SO_REUSEADDR, (char*)&iOpt, iOptLen);

    int ret = 0;
    ret = bind(m_fdSockListen, (struct sockaddr *) &addr, addr_len);
    if (ret == -1)
    {
        PRINTF("CUdpTransmit bind error!\n");
        goto L_ERR;
    }

    PRINTF("bind %d!\n", m_ulListenIP);

    // set noblock
    iFlags = fcntl(m_fdSockListen, F_GETFL, 0);
    fcntl(m_fdSockListen, F_SETFL, iFlags|O_NONBLOCK);

    PRINTF("<NetTransmit> udp Listen at [%d]...\n", m_ulListenPort);
    return 1;

L_ERR:
    close(m_fdSockListen);
    m_fdSockListen = -1;
    ret = 0;
    return ret;
}

int CUdpTransmit::CheckLink(unsigned long ulIPIn, unsigned short wPortIn)
{
    bool fGetInfo = false;

    std::map<int, STransInfo>::iterator itor = m_SockInfoMap.begin();
    for (; itor != m_SockInfoMap.end(); )
    {
        // check the connect have comin before
        if ( (itor->second.ulIP == ulIPIn) && (itor->second.wPort == wPortIn) )
        {
            memcpy(&m_sCurTransInfo, &(itor->second), sizeof(m_sCurTransInfo));
            fGetInfo = true;
            break;
        }

        itor++;
    }

    if (!fGetInfo)
    {
        PRINTF("** new conn\n");

        // get available wPort
        static unsigned short s_port = PORT_TRANS_START;
        unsigned short wPort;
        itor = m_SockInfoMap.begin();

        while (1)
        {
            wPort = s_port + 1;
            for (itor=m_SockInfoMap.begin();
                    itor!=m_SockInfoMap.end();
                    itor++ )
            {
                if (itor->second.wTransmitPort == wPort)
                    break;
            }

            if (itor!=m_SockInfoMap.end())
            {
                s_port++;

                if (s_port == 65535)
                    s_port = PORT_TRANS_START;
            }
            else
            {
                break;
            }
        }

        // create m_fdComin
        for (int i=0; i<3; i++)
        {
            m_sCurTransInfo.fd = CreateUdpSock(m_ulTransBindIP, wPort);
            if ( m_sCurTransInfo.fd != -1 )   break;

            if (i == 2) return 0;
        }

        //save comin conn info
        m_sCurTransInfo.ulIP = ulIPIn;
        m_sCurTransInfo.wPort = wPortIn;
        m_sCurTransInfo.wTransmitPort = wPort;
        m_sCurTransInfo.tLastTransTime = time(0);
        m_SockInfoMap.insert(std::pair<int, STransInfo>(m_sCurTransInfo.fd, m_sCurTransInfo));

    }
    return 1;
}

int CUdpTransmit::CheckLinkTimeOut()
{
    bool fChkConnTime = false;
    time_t diffSec = time(0) - m_lastChkConnTime;
    int iRet = 0;

    if (diffSec > 3)
    {
        fChkConnTime = true;
        m_lastChkConnTime = time(0);
    }
    else
    {
        return iRet;
    }

    if (fChkConnTime)
    {
        std::map<int, STransInfo>::iterator itor = m_SockInfoMap.begin();
        for (; itor != m_SockInfoMap.end(); )
        {
            bool bErase = false;
            if (m_lastChkConnTime - itor->second.tLastTransTime > 3/*15*60*/ )
            {
                // timeout erase conn info
                close(itor->first);
                m_SockInfoMap.erase(itor++);
                bErase = true;
                m_fIsClientComin = FALSE;
                if (m_funcClientExitCallBack)
                {
                    m_funcClientExitCallBack(m_pClientExitCallBackUserData);
                }
                iRet = 1;
            }
            if (!bErase) ++itor;
        }
    }
    return iRet;
}

int CUdpTransmit::CallBack_Startup(void* pUserData)
{
    CUdpTransmit* pThis = (CUdpTransmit*)pUserData;

    if (pThis->m_fdSockListen == -1)
        return 0;

    return 1;
}

int CUdpTransmit::CallBack_Recv(char* buf, int* pLen, void* pUserData)
{
    PRINTF("CallBack_Recv\n");
    CUdpTransmit* pThis = (CUdpTransmit*)pUserData;

    sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);

    pThis->CheckLinkTimeOut();

    pThis->m_sCurTransInfo.fd = -1;   // will get a fd, if recv succ

    int ret = 0;
    SocketWait(pThis->m_fdSockListen, true, false, 10);

    int iRecvLen = recvfrom(pThis->m_fdSockListen, buf, *pLen, 0, (sockaddr*)&addr, &addrlen);
    if (iRecvLen < 0)
    {
        if (errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN)
        {
            PRINTF("udp listen no data\n");
            ret = 1;
        }
    }
    else if (iRecvLen == 0)
    {
        PRINTF("iRecvLen = 0\n");
    }
    else
    {
        *pLen = iRecvLen;
        ret = 2;   // continue recv after send
    }

    if ( ret == 2 ) // get data
    {
        PRINTF("udp recv %d, from wPort %d\n", iRecvLen, ntohs(addr.sin_port));
        unsigned long ulIP = ntohl(addr.sin_addr.s_addr);
        unsigned short wPort = ntohs(addr.sin_port);

        // Check and create link
        ret = (pThis->CheckLink(ulIP, wPort)) ? 2 : 1;
    }

    return ret;
}

int CUdpTransmit::CallBack_Send(char* buf, int* pLen, void* pUserData)
{
    PRINTF("CallBack_Send\n");
    CUdpTransmit* pThis = (CUdpTransmit*)pUserData;

    if (pThis->m_fdSockListen == -1)
        return 0;
    if ((pThis->m_sCurTransInfo.fd == -1) || (*pLen == 0))
        return 1;

    sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(pThis->m_sCurTransInfo.wPort);
    addr.sin_addr.s_addr = htonl(pThis->m_sCurTransInfo.ulIP);

    int iSendLen = sendto(pThis->m_fdSockListen, buf, *pLen, 0, (sockaddr*)&addr, addrlen);

    if ((iSendLen == 0) || (iSendLen < 0) && (errno != EINTR && errno != EWOULDBLOCK && errno != EAGAIN))
    {
        PRINTF("send len %d, err:%d\n", iSendLen, errno);
        return 0;
    }

    return 1;
}

int CUdpTransmit::CallBack_TransmitData(char* buf, int* pLen, void* pUserData)
{
    PRINTF("CallBack_TransmitData\n");
    CUdpTransmit* pThis = (CUdpTransmit*)pUserData;
    if ((pThis->m_sCurTransInfo.fd == -1) || (*pLen == 0))
        return 1;

    sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);
    addr.sin_family = AF_INET;
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

int CUdpTransmit::CallBack_GetTransmitData(char* buf, int* pLen, void* pUserData)
{
    PRINTF("CallBack_GetTransmitData\n");
    CUdpTransmit* pThis = (CUdpTransmit*)pUserData;

    sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);

    pThis->m_sCurTransInfo.fd = -1; // will get a fd, if get succ

    if (pThis->m_SockInfoMap.size() == 0)
        return 1;

    int ret = pThis->SelectTransmitData(&pThis->m_sCurTransInfo.fd, 10);
    if ( ret != 2 )  // if ret is 2, get ready fd
    {
        PRINTF("select ret %d\n", ret);
        return ret;
    }

    ret = 0;
    int iRecvLen = recvfrom(pThis->m_sCurTransInfo.fd, buf, *pLen, 0, (sockaddr*)&addr, &addrlen);
    if (iRecvLen < 0)
    {
        if (errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN)
        {
            PRINTF("udp do not get transmit data\n");
            ret = 1;
        }
        else
        {
            PRINTF("recv err %d\n", errno);

            pThis->m_SockInfoMap.erase(pThis->m_sCurTransInfo.fd);

            close(pThis->m_sCurTransInfo.fd);
            pThis->m_sCurTransInfo.fd = -1;
            ret = 1;
        }
    }
    else if (iRecvLen == 0)
    {
        PRINTF("recvfrom 0, conn exit\n");
    }
    else
    {
        *pLen = iRecvLen;
        ret = 2;   // continue recv after send
        if (pThis->m_funcClientCominCallBack && !pThis->m_fIsClientComin)
        {
            pThis->m_funcClientCominCallBack(pThis->m_pClientCominCallBackUserData);
        }
        pThis->m_fIsClientComin = TRUE;
    }

    if ( ret == 2 ) // get data
    {
        printf("recvfrom %d\n", iRecvLen);
        std::map<int, STransInfo>::iterator itor = pThis->m_SockInfoMap.find(pThis->m_sCurTransInfo.fd);

        if (itor != pThis->m_SockInfoMap.end())
        {
            itor->second.tLastTransTime = time(0);

            pThis->m_sCurTransInfo.ulIP = itor->second.ulIP;
            pThis->m_sCurTransInfo.wPort = itor->second.wPort;
            pThis->m_sCurTransInfo.wTransmitPort = itor->second.wTransmitPort;
        }
        else
        {
            pThis->m_sCurTransInfo.fd = -1;
        }
    }

    return ret;
}

int CUdpTransmit::Run()
{
    if (InitListen() == 0)
        return 0;

    m_cTransmit.SetStartupCallback(CallBack_Startup, this);
    m_cTransmit.SetRecvCallback(CallBack_Recv, this);
    m_cTransmit.SetSendCallback(CallBack_Send, this);
    m_cTransmit.SetTransmitDataCallback(CallBack_TransmitData, this);
    m_cTransmit.SetGetTransmitDataCallback(CallBack_GetTransmitData, this);

    return m_cTransmit.Run();
}

void CUdpTransmit::Stop()
{
    m_cTransmit.Stop();

    if (m_fdSockListen != -1)
    {
        shutdown(m_fdSockListen, 0x02);
        close(m_fdSockListen);
        m_fdSockListen = -1;
    }

    if (m_fdSockTransmit != -1)
    {
        shutdown(m_fdSockTransmit, 0x02);
        close(m_fdSockTransmit);
        m_fdSockTransmit = -1;
    }

    std::map<int, STransInfo>::iterator itor = m_SockInfoMap.begin();
    for (; itor != m_SockInfoMap.end(); )
    {
        close(itor->first);
        m_SockInfoMap.erase(itor++);
    }
}
