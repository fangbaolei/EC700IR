#include "CAMCYBroadcastTransmit.h"
#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#define PORT_CAM_CY_BROADCAST_RECV  7777

CCAMCYBroadcastTransmit::CCAMCYBroadcastTransmit()
{
}

CCAMCYBroadcastTransmit::~CCAMCYBroadcastTransmit()
{
}

int CCAMCYBroadcastTransmit::Run()
{
    if (InitListen() == 0)
        return 0;
    if (InitListenCam() == 0)
        return 0;

    m_cTransmit.SetStartupCallback(CallBack_Startup, this);
    m_cTransmit.SetRecvCallback(CallBack_Recv, this);
    m_cTransmit.SetSendCallback(CallBack_SendCY, this);
    m_cTransmit.SetTransmitDataCallback(CallBack_SendCY, this);
    m_cTransmit.SetGetTransmitDataCallback(CallBack_GetTransmitDataCY, this);

    return m_cTransmit.Run();
}

void CCAMCYBroadcastTransmit::Stop()
{
    CBroadcastTransmit::Stop();

    if (m_fdListenCam != -1)
    {
        close(m_fdListenCam);
        m_fdListenCam = -1;
    }
}

int CCAMCYBroadcastTransmit::CallBack_SendCY(char* buf, int* pLen, void* pUserData)
{
    PRINTF("CallBack_SendCY\n");
    CCAMCYBroadcastTransmit* pThis = (CCAMCYBroadcastTransmit*)pUserData;

    if ((pThis->m_sCurTransInfo.fd == -1) || (*pLen == 0))
        return 1;

    if (*pLen == 64)
    {
        // 记录连接进来的IP
        pThis->m_ulFromIP = pThis->m_sCurTransInfo.ulIP;
        pThis->m_wFromPort = pThis->m_sCurTransInfo.wPort;
        return CBroadcastTransmit::CallBack_Send(buf, pLen, pUserData);
    }
    else if (*pLen == 164)
    {
        unsigned long ulNetIP = htonl(pThis->m_ulListenIP);
        memcpy(buf+40, &ulNetIP, 4);

        sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(pThis->m_ulFromIP);
        addr.sin_port = htons(PORT_CAM_CY_BROADCAST_RECV);
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

int CCAMCYBroadcastTransmit::CallBack_GetTransmitDataCY(char* buf, int* pLen, void* pUserData)
{
    PRINTF("CallBack_Recv\n");
    CCAMCYBroadcastTransmit* pThis = (CCAMCYBroadcastTransmit*)pUserData;

    sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);

    pThis->m_sCurTransInfo.fd = -1;

    int ret = 0;
    SocketWait(pThis->m_fdListenCam, true, false, 10);

    int iRecvLen = recvfrom(pThis->m_fdListenCam, buf, *pLen, 0, (sockaddr*)&addr, &addrlen);
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
        PRINTF("udp recv %d, from wPort %s:%d\n", iRecvLen, inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));

        if ((htonl(pThis->m_ulListenIP) == addr.sin_addr.s_addr)
                || (htonl(pThis->m_ulTransIP) == addr.sin_addr.s_addr)
                || ((htonl(pThis->m_ulTransIP) & htonl(pThis->m_ulTransMask))
                    != (addr.sin_addr.s_addr & htonl(pThis->m_ulTransMask))))
        {
            PRINTF("drop\n");
            pThis->m_sCurTransInfo.fd = -1;
        }
        else
        {
            pThis->m_sCurTransInfo.fd = pThis->m_fdListenCam;
            pThis->m_sCurTransInfo.ulIP = ntohl(addr.sin_addr.s_addr);
            pThis->m_sCurTransInfo.wPort = ntohs(addr.sin_port);
        }

        ret = 1;   // continue recv after send
    }

    return ret;
}

int CCAMCYBroadcastTransmit::InitListenCam()
{
    struct sockaddr_in addr;
    int addr_len = sizeof(struct sockaddr_in);
    int iFlags;

    m_fdListenCam = socket(AF_INET, SOCK_DGRAM, 0);

    if (m_fdListenCam == -1)
    {
        PRINTF("CBroadcastTransmit socket error!\n");
        return 0;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT_CAM_CY_BROADCAST_RECV);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int iOpt = 1;
    int iOptLen = sizeof(iOpt);
    setsockopt(m_fdListenCam, SOL_SOCKET, SO_REUSEADDR, (char*)&iOpt, iOptLen);
    setsockopt(m_fdListenCam, SOL_SOCKET, SO_BROADCAST, (char*)&iOpt, iOptLen);

    int ret = 0;
    ret = bind(m_fdListenCam, (struct sockaddr *) &addr, addr_len);
    if (ret == -1)
    {
        PRINTF("CBroadcastTransmit bind error!\n");
        goto L_ERR;
    }

    PRINTF("CBroadcastTransmit bind %d!\n", m_ulListenIP);

    // set noblock
    iFlags = fcntl(m_fdListenCam, F_GETFL, 0);
    fcntl(m_fdListenCam, F_SETFL, iFlags|O_NONBLOCK);

    Trace("<NetTransmit> Broadcast Listen Cam at [%d]...\n", PORT_CAM_CY_BROADCAST_RECV);
    return 1;

L_ERR:
    close(m_fdListenCam);
    m_fdListenCam = -1;
    ret = 0;
    return ret;
}
