#include "CTcpTransmit.h"
#include "NetTransmitUtils.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <fcntl.h>
#include "hvutils.h"

#define DEF_SOCK_BUF_LEN            (8*1024)

CTcpTransmit::CTcpTransmit()
{
    m_fdSockListenClient = m_fdSockListen = m_fdSockTransmit = -1;
    m_funcClientCominCallBack = NULL;
    m_funcClientExitCallBack = NULL;

    m_iClientDefSockRcvBufLen = DEF_SOCK_BUF_LEN;
    m_iClientDefSockSndBufLen = DEF_SOCK_BUF_LEN;
    m_iTransmitDefSockRcvBufLen = DEF_SOCK_BUF_LEN;
    m_iTransmitDefSockSndBufLen = DEF_SOCK_BUF_LEN;
}

CTcpTransmit::~CTcpTransmit()
{
}

void CTcpTransmit::SetClientCominCallBack(CALLBACK_TRANSMIT_STARTUP_FUNC func, void* pUserData)
{
    m_funcClientCominCallBack = func;
    m_pClientCominCallBackUserData = pUserData;
}

void CTcpTransmit::SetClientExitCallBack(CALLBACK_TRANSMIT_STARTUP_FUNC func, void* pUserData)
{
    m_funcClientExitCallBack = func;
    m_pClientExitCallBackUserData = pUserData;
}

void CTcpTransmit::SetClientDefSockBufLen(int iRecvBufLen, int iSendBufLen)
{
    m_iClientDefSockRcvBufLen = iRecvBufLen;
    m_iClientDefSockSndBufLen = iSendBufLen;
}

void CTcpTransmit::SetTransmitDefSockBufLen(int iRecvBufLen, int iSendBufLen)
{
    m_iTransmitDefSockRcvBufLen = iRecvBufLen;
    m_iTransmitDefSockSndBufLen = iSendBufLen;
}

int CTcpTransmit::CallBack_Startup(void* pUserData)
{
    CTcpTransmit* pThis = (CTcpTransmit*)pUserData;

    PRINTF("CTcpTransmit CallBack_Startup [%d] %d\n", pThis->m_ulListenPort, pThis->m_fdSockTransmit);

    // close client socket
    if (pThis->m_fdSockListenClient != -1)
    {
        shutdown(pThis->m_fdSockListenClient, 0x02);
        close(pThis->m_fdSockListenClient);
        pThis->m_fdSockListenClient = -1;
    }

    if (pThis->m_fdSockTransmit != -1)
    {
        shutdown(pThis->m_fdSockTransmit, 0x02);
        close(pThis->m_fdSockTransmit);
        pThis->m_fdSockTransmit = -1;
    }

    sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    memset(&addr, 0, addr_len);

    PRINTF("CTcpTransmit wait [%d] comin\n", pThis->m_ulListenPort);
    // wait connect

    if (0 != SocketWait(pThis->m_fdSockListen, true, false, 1000*1000))
    {
        PRINTF("CTcpTransmit CallBack_Startup [%d] comin\n", pThis->m_ulListenPort);
        pThis->m_fdSockListenClient = accept(pThis->m_fdSockListen,
                                             (struct sockaddr*) &addr, &addr_len);

        if ( pThis->m_fdSockListenClient != -1 )
        {
            PRINTF("CTcpTransmit CallBack_Startup [%s:%d] comin\n", inet_ntoa(addr.sin_addr), pThis->m_ulListenPort);
            // set noblock
            int iFlags = fcntl(pThis->m_fdSockListenClient, F_GETFL, 0);
            fcntl(pThis->m_fdSockListenClient, F_SETFL, iFlags|O_NONBLOCK);

            if (pThis->m_fdSockTransmit != -1)
            {
                shutdown(pThis->m_fdSockTransmit, 0x02);
                close(pThis->m_fdSockTransmit);
                pThis->m_fdSockTransmit = -1;
            }

            if (pThis->InitTransmit() == 0)
            {
                shutdown(pThis->m_fdSockListenClient, 0x02);
                close(pThis->m_fdSockListenClient);
                pThis->m_fdSockListenClient = -1;
                return 0;
            }

            int nSetBufLenlen = sizeof(pThis->m_iClientDefSockRcvBufLen);
            setsockopt( pThis->m_fdSockListenClient, SOL_SOCKET, SO_RCVBUF,
                        (void *)&pThis->m_iClientDefSockRcvBufLen, nSetBufLenlen );
            setsockopt( pThis->m_fdSockListenClient, SOL_SOCKET, SO_SNDBUF,
                        (void *)&pThis->m_iClientDefSockSndBufLen, nSetBufLenlen );

            return 1;
        }
    }
    PRINTF("CTcpTransmit CallBack_Startup [%d] err.exit\n", pThis->m_ulListenPort);
    return 0;
}

int CTcpTransmit::CallBack_Recv(char* pBuf, int* pLen, void* pUserData)
{
    CTcpTransmit* pThis = (CTcpTransmit*)pUserData;
    int iRecvLen = 0;
    int iRecvAll = 0;

    int iBufLen = *pLen;

    bool fEnd = false;

    SocketWait(pThis->m_fdSockListenClient, true, false, 10);

    while ( (iRecvLen = recv(pThis->m_fdSockListenClient,
                             pBuf, iBufLen-iRecvAll, MSG_NOSIGNAL)) > 0)
    {
        pBuf += iRecvLen;
        if (iBufLen-iRecvAll > iRecvLen)
        {
            fEnd = true;
            iRecvAll += iRecvLen;
            break;
        }

        iRecvAll += iRecvLen;

        if (iBufLen - iRecvAll == 0)
        {
            return 2;
        }
        // continue recv
    }

    *pLen = iRecvAll;
    if (fEnd)
    {
        PRINTF("recv data %d\n", iRecvAll);
        return 1;
    }

    if ((iRecvLen < 0) && (errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN))
    {
        return 1;
    }
    else
    {
        PRINTF("CallBack_Recv err 2 %d %s\n", iRecvAll, strerror(errno));
        return 0;
    }
}

int CTcpTransmit::CallBack_Send(char* pBuf, int* pLen, void* pUserData)
{
    if (*pLen == 0)
        return 1;

    //PRINTF("CallBack_Send\n");
    CTcpTransmit* pThis = (CTcpTransmit*)pUserData;

    int iSendLen = 0;
    int iSendAll = 0;

    int iDataLen = *pLen;

    while ( iDataLen - iSendAll )
    {
        iSendLen = send(pThis->m_fdSockListenClient,
                        pBuf, iDataLen-iSendAll, MSG_NOSIGNAL);

        if (iSendLen < 0)
        {
            if (errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN)
            {
			    continue;
            }
            else
            {
                PRINTF("CallBack_Send %s\n", strerror(errno));
                return 0;
            }
        }
        else if (iSendLen == 0)
        {
            PRINTF("send 0\n");
            return 0;
        }

        pBuf += iSendLen;
        iSendAll += iSendLen;
        PRINTF("send %d\n", iSendLen);
        // continue recv
    }

    return 1;
}

int CTcpTransmit::CallBack_TransmitData(char* pBuf, int* pLen, void* pUserData)
{
    if (*pLen == 0)
        return 1;

    CTcpTransmit* pThis = (CTcpTransmit*)pUserData;

    int iSendLen = 0;
    int iSendAll = 0;

    int iDataLen = *pLen;

    while ( iDataLen - iSendAll )
    {
        iSendLen = send(pThis->m_fdSockTransmit,
                        pBuf, iDataLen-iSendAll, MSG_NOSIGNAL);

        if (iSendLen < 0)
        {
            if (errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN)
            {
                continue;
            }
            else
            {
                PRINTF("CallBack_TransmitData err 1 %d %s\n", iSendLen, strerror(errno));
                close(pThis->m_fdSockTransmit);
                pThis->m_fdSockTransmit = -1;
                return 0;
            }
        }
        else if (iSendLen == 0)
        {
            PRINTF("send 0\n");
            return 0;
        }

        pBuf += iSendLen;
        iSendAll += iSendLen;

        PRINTF("send %d\n", iSendLen);
        // continue recv
    }

    return 1;
}

int CTcpTransmit::CallBack_GetTransmitData(char* pBuf, int* pLen, void* pUserData)
{
    CTcpTransmit* pThis = (CTcpTransmit*)pUserData;
    int iRecvLen = 0;
    int iRecvAll = 0;

    int iBufLen = *pLen;

    bool fEnd = false;

    SocketWait(pThis->m_fdSockTransmit, true, false, 10);

    while ( (iRecvLen = recv(pThis->m_fdSockTransmit,
                             pBuf, iBufLen-iRecvAll, MSG_NOSIGNAL)) > 0 )
    {
        pBuf += iRecvLen;
        if (iBufLen-iRecvAll > iRecvLen)
        {
            fEnd = true;
            iRecvAll += iRecvLen;
            break;
        }

        iRecvAll += iRecvLen;

        if (iBufLen - iRecvAll == 0)
        {
            return 2;
        }
        // continue recv
    }

    *pLen = iRecvAll;
    if (fEnd)
    {
        PRINTF("getTransmitData %d\n", iRecvAll);
        return 1;
    }

    if ((iRecvLen < 0) && (errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN))
    {
        return 1;
    }
    else
    {
        PRINTF("CallBack_GetTransmitData err 2 %d : %s\n", iRecvAll, strerror(errno));
        close(pThis->m_fdSockTransmit);
        pThis->m_fdSockTransmit = -1;
        return 0;
    }
}

int CTcpTransmit::Run()
{
    if (InitListen() == 0)
    {
        return 0;
    }

    m_cTransmit.SetStartupCallback(CallBack_Startup, this);
    m_cTransmit.SetRecvCallback(CallBack_Recv, this);
    m_cTransmit.SetSendCallback(CallBack_Send, this);
    m_cTransmit.SetTransmitDataCallback(CallBack_TransmitData, this);
    m_cTransmit.SetGetTransmitDataCallback(CallBack_GetTransmitData, this);

    return m_cTransmit.Run();
}

void CTcpTransmit::Stop()
{
    m_cTransmit.Stop();
	
    if (m_fdSockListenClient != -1)
    {
        shutdown(m_fdSockListenClient, 0x02);
        close(m_fdSockListenClient);
        m_fdSockListenClient = -1;
    }
	
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
}

int CTcpTransmit::InitListen()
{
    struct sockaddr_in addr;
    int addr_len = sizeof(struct sockaddr_in);
    int iFlags;

    m_fdSockListen = socket(AF_INET, SOCK_STREAM, 0);

    if (m_fdSockListen == -1)
    {
        PRINTF("CTcpTransmit socket error!\n");
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
        PRINTF("CTcpTransmit bind error!\n");
        goto L_ERR;
    }

    ret = listen(m_fdSockListen, 1);
    if (ret == -1)
    {
        PRINTF("CTcpTransmit listen error!\n");
        goto L_ERR;
    }

    // set noblock
    iFlags = fcntl(m_fdSockListen, F_GETFL, 0);
    fcntl(m_fdSockListen, F_SETFL, iFlags|O_NONBLOCK);

    Trace("<NetTransmit> tcp Listen at [%s:%d]...\n", inet_ntoa(addr.sin_addr), m_ulListenPort);
    return 1;

L_ERR:
    shutdown(m_fdSockListen, 0x02);
    close(m_fdSockListen);
    m_fdSockListen = -1;
    ret = 0;
    return ret;
}

int CTcpTransmit::InitTransmit()
{
    PRINTF("InitTransmit!\n");
    struct sockaddr_in addr;
    int addr_len = sizeof(struct sockaddr_in);
    int iFlags;

    m_fdSockTransmit = socket(AF_INET, SOCK_STREAM, 0);

    if (m_fdSockTransmit == -1)
    {
        PRINTF("socket error!\n");
        return 0;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(m_wTransBindPort);
    addr.sin_addr.s_addr = htonl(m_ulTransBindIP);

    int iOpt = 1;
    int iOptLen = sizeof(iOpt);
    setsockopt(m_fdSockTransmit, SOL_SOCKET, SO_REUSEADDR, (char*)&iOpt, iOptLen);

    int nSetBufLenlen = sizeof(m_iTransmitDefSockRcvBufLen);
    setsockopt( m_fdSockTransmit, SOL_SOCKET, SO_RCVBUF,
                (void *)&m_iTransmitDefSockRcvBufLen, nSetBufLenlen );
    setsockopt( m_fdSockTransmit, SOL_SOCKET, SO_SNDBUF,
                (void *)&m_iTransmitDefSockSndBufLen, nSetBufLenlen );

    int iRet;
    iRet = bind(m_fdSockTransmit, (struct sockaddr *) &addr, addr_len);
    if (iRet == -1)
    {
        PRINTF("bind error!\n");
        goto L_ERR;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(m_wTransPort);
    addr.sin_addr.s_addr = htonl(m_ulTransIP);

    // set noblock
    iFlags = fcntl(m_fdSockTransmit, F_GETFL, 0);
    fcntl(m_fdSockTransmit, F_SETFL, iFlags|O_NONBLOCK);

    iRet = connect(m_fdSockTransmit, (struct sockaddr*) &addr, addr_len);
    if ( (iRet == -1) && (0 == SocketWait(m_fdSockTransmit, false, true, 1000*1000)) )
    {
        PRINTF("connect timeout!\n");
        goto L_ERR;
    }

    PRINTF("<NetTransmit> Transmit at [%d], to [%s:%d]...\n", m_wTransBindPort, inet_ntoa(addr.sin_addr), m_wTransPort);
    return 1;

L_ERR:
    shutdown(m_fdSockTransmit, 0x02);
    close(m_fdSockTransmit);
    m_fdSockTransmit = -1;
    iRet = 0;
    return iRet;
}

int CTcpTransmit::SetListen(unsigned long ulBindIP, unsigned short wBindPort)
{
    m_ulListenIP = ulBindIP;
    m_ulListenPort = wBindPort;

    return 1;
}

int CTcpTransmit::SetTransmit(unsigned long ulTransIP,
                              unsigned short wTransPort,
                              unsigned long ulBindIP,
                              unsigned short wBindPort)
{
    m_ulTransIP = ulTransIP;
    m_wTransPort = wTransPort;
    m_ulTransBindIP = ulBindIP;
    m_wTransBindPort = wBindPort;

    return 1;
}


