#include "BroadcastTransmit.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>

CBroadcastTransmit::CBroadcastTransmit()
{
}

CBroadcastTransmit::~CBroadcastTransmit()
{
}

int CBroadcastTransmit::SetListen(unsigned long ulBindIP,
                                  unsigned long ulBindMask,
                                  unsigned short wBindPort)
{
    m_ulListenIP = ulBindIP;
    m_ulListenMask = ulBindMask;
    m_ulListenPort = wBindPort;
    return 1;
}

int CBroadcastTransmit::SetTransmit(unsigned long ulTransIP,
                                    unsigned long ulTransMask,
                                    unsigned short wTransPort)
{
    m_ulTransIP = ulTransIP;
    m_ulTransMask = ulTransMask;
    m_wTransPort = wTransPort;
    return 1;
}

int CBroadcastTransmit::InitListen()
{
    struct sockaddr_in addr;
    int addr_len = sizeof(struct sockaddr_in);
    int iFlags;

    m_fdSock = socket(AF_INET, SOCK_DGRAM, 0);

    if (m_fdSock == -1)
    {
        PRINTF("CBroadcastTransmit socket error!\n");
        return 0;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(m_ulListenPort);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int iOpt = 1;
    int iOptLen = sizeof(iOpt);
    setsockopt(m_fdSock, SOL_SOCKET, SO_REUSEADDR, (char*)&iOpt, iOptLen);
    setsockopt(m_fdSock, SOL_SOCKET, SO_BROADCAST, (char*)&iOpt, iOptLen);

    int ret = 0;
    ret = bind(m_fdSock, (struct sockaddr *) &addr, addr_len);
    if (ret == -1)
    {
        PRINTF("CBroadcastTransmit bind error!\n");
        goto L_ERR;
    }

    // set noblock
    iFlags = fcntl(m_fdSock, F_GETFL, 0);
    fcntl(m_fdSock, F_SETFL, iFlags|O_NONBLOCK);

    PRINTF("<NetTransmit> Broadcast Listen at [%d]...\n", m_ulListenPort);
    return 1;

L_ERR:
    close(m_fdSock);
    m_fdSock = -1;
    ret = 0;
    return ret;
}

int CBroadcastTransmit::CallBack_Startup(void* pUserData)
{
    CBroadcastTransmit* pThis = (CBroadcastTransmit*)pUserData;

    if (pThis->m_fdSock == -1)
        return 0;

    return 1;
}

int CBroadcastTransmit::CallBack_Recv(char* buf, int* pLen, void* pUserData)
{
    PRINTF("CallBack_Recv\n");
    CBroadcastTransmit* pThis = (CBroadcastTransmit*)pUserData;

    sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);

    pThis->m_sCurTransInfo.fd = -1;

    int ret = 0;
    SocketWait(pThis->m_fdSock, true, false, 10);

    int iRecvLen = recvfrom(pThis->m_fdSock, buf, *pLen, 0, (sockaddr*)&addr, &addrlen);

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
        PRINTF("CBroadcastTransmit udp recv %d, from wPort %s:%d\n",
            iRecvLen, inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));

        if ((htonl(pThis->m_ulListenIP) == addr.sin_addr.s_addr)
                || (htonl(pThis->m_ulTransIP) == addr.sin_addr.s_addr))
        {
            PRINTF("drop\n");
            pThis->m_sCurTransInfo.fd = -1;
        }
        else
        {
            pThis->m_sCurTransInfo.fd = pThis->m_fdSock;
            pThis->m_sCurTransInfo.ulIP = ntohl(addr.sin_addr.s_addr);
            pThis->m_sCurTransInfo.wPort = ntohs(addr.sin_port);
        }

        ret = 1;   // continue recv after send
    }

    return ret;
}

int CBroadcastTransmit::CallBack_Send(char* buf, int* pLen, void* pUserData)
{
    PRINTF("CallBack_Send\n");
    CBroadcastTransmit* pThis = (CBroadcastTransmit*)pUserData;

    if ((pThis->m_sCurTransInfo.fd == -1) || (*pLen == 0))
        return 1;

    PRINTF("CallBack_Send %d\n", *pLen);
    sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);

    unsigned long ulIPIn = pThis->m_sCurTransInfo.ulIP;
    unsigned long ulIPOut;

    if ((ulIPIn & pThis->m_ulListenMask) == (pThis->m_ulListenIP & pThis->m_ulListenMask))
    {
        addr.sin_port = htons(pThis->m_wTransPort);
        ulIPOut = pThis->m_ulTransIP;

        PRINTF("send to port %d\n", pThis->m_wTransPort);
    }
    else
    {
        addr.sin_port = htons(pThis->m_sCurTransInfo.wPort);
        ulIPOut = pThis->m_ulListenIP;
    }

    int iSendLen = TrsmBroadcastSend(ulIPOut, pThis->m_ulListenPort, buf,
                                    *pLen, 0, (sockaddr*)&addr, addrlen);

    if ((iSendLen == 0) || (iSendLen < 0) && (errno != EINTR && errno != EWOULDBLOCK && errno != EAGAIN))
    {
        PRINTF("send len %d, err:%s\n", iSendLen, strerror(errno));
        return 0;
    }

    return 1;
}

int CBroadcastTransmit::Run()
{
    if (InitListen() == 0)
        return 0;

    m_cTransmit.SetStartupCallback(CallBack_Startup, this);
    m_cTransmit.SetRecvCallback(CallBack_Recv, this);
    m_cTransmit.SetSendCallback(CallBack_Send, this);
    m_cTransmit.SetTransmitDataCallback(CallBack_Send, this);
    m_cTransmit.SetGetTransmitDataCallback(CallBack_Recv, this);

    return m_cTransmit.Run();
}

void CBroadcastTransmit::Stop()
{
    m_cTransmit.Stop();

    if (m_fdSock != -1)
    {
        shutdown(m_fdSock, 0x02);
        close(m_fdSock);
        m_fdSock = -1;
    }
}
