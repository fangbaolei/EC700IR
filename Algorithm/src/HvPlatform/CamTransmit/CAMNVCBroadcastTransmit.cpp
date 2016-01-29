#include "CAMNVCBroadcastTransmit.h"
#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>

CCAMNVCBroadcastTransmit::CCAMNVCBroadcastTransmit()
{
}

CCAMNVCBroadcastTransmit::~CCAMNVCBroadcastTransmit()
{
}

int CCAMNVCBroadcastTransmit::Run()
{
    if (InitListen() == 0)
        return 0;

    m_cTransmit.SetStartupCallback(CallBack_Startup, this);
    m_cTransmit.SetRecvCallback(CallBack_Recv, this);
    m_cTransmit.SetSendCallback(CallBack_SendNVC, this);
    m_cTransmit.SetTransmitDataCallback(CallBack_SendNVC, this);
    m_cTransmit.SetGetTransmitDataCallback(CallBack_Recv, this);

    return m_cTransmit.Run();
}

int CCAMNVCBroadcastTransmit::CallBack_SendNVC(char* buf,
                                               int* pLen,
                                               void* pUserData)
{
    PRINTF("CallBack_SendNVC\n");
    CCAMNVCBroadcastTransmit* pThis = (CCAMNVCBroadcastTransmit*)pUserData;

    if ((pThis->m_sCurTransInfo.fd == -1) || (*pLen == 0))
        return 1;

    char* pHead = "Arecont";
    if (*pLen == 22)
    {
        // 记录连接进来的IP
        pThis->m_ulFromIP = pThis->m_sCurTransInfo.ulIP;
        pThis->m_wFromPort = pThis->m_sCurTransInfo.wPort;
        return CBroadcastTransmit::CallBack_Send(buf, pLen, pUserData);
    }
    else if ((*pLen == 69) && (memcmp(buf, pHead, strlen(pHead)) == 0))
    {
        unsigned long ulNetIP = htonl(pThis->m_ulListenIP);
        memcpy(buf+28, &ulNetIP, 4);

        sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(pThis->m_ulFromIP);
        addr.sin_port = htons(pThis->m_wFromPort);
        PRINTF(">> send to %s\n", inet_ntoa(addr.sin_addr));

        int iSendLen = sendto(pThis->m_fdSock, buf, *pLen, 0, (sockaddr*)&addr, sizeof(addr));

        if ((iSendLen == 0) || (iSendLen < 0) && (errno != EINTR && errno != EWOULDBLOCK && errno != EAGAIN))
        {
            PRINTF("send len %d, err:%s\n", iSendLen, strerror(errno));
            return 0;
        }
    }

    return 1;
}

int CCAMNVCBroadcastTransmit::CallBack_GetTransmitDataNVC(
    char* buf, int* pLen, void* pUserData)
{
    CCAMNVCBroadcastTransmit* pThis = (CCAMNVCBroadcastTransmit*)pUserData;

    int iRet = CallBack_Recv(buf, pLen, pUserData);
    if ((htonl(pThis->m_ulTransIP) & htonl(pThis->m_ulTransMask))
        != (pThis->m_sCurTransInfo.ulIP & htonl(pThis->m_ulTransMask)))
    {
        PRINTF("drop\n");
        pThis->m_sCurTransInfo.fd = -1;
    }
    return iRet;
}
