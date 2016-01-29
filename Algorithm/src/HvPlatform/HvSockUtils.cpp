#include "HvSockUtils.h"

HV_SOCKET_HANDLE HvCreateSocket(int af/*=AF_INET*/, int type/*=SOCK_STREAM*/, int protocol/*=0*/)
{
    return Socket(af, type, protocol);
}

HRESULT HvListen(const HV_SOCKET_HANDLE& hSocket, WORD16 wPort, int backlog)
{
    struct hv_sockaddr_in sa;
    sa.sin_family = AF_INET;
    sa.sin_port = htons(wPort);
    sa.sin_addr.s_addr = htonl(INADDR_ANY);

    int iOpt = 1;
    int iOptLen = sizeof(iOpt);
    setsockopt(hSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&iOpt, iOptLen);

    int iRet = -1;
    iRet = Bind(hSocket, (hv_sockaddr*)&sa, sizeof(struct hv_sockaddr));
    if ( 0 != iRet ) return E_FAIL;

    iRet = Listen(hSocket, backlog);
    return (0 == iRet) ? S_OK : E_FAIL;
}

HRESULT HvSetRecvTimeOut(const HV_SOCKET_HANDLE& hSocket, int iMS)
{
    struct timeval uTime;
    uTime.tv_sec = iMS / 1000;
    uTime.tv_usec = (iMS % 1000) * 1000;
    int iRet = SetSockOpt(hSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&uTime, sizeof(timeval));
    return (0 == iRet) ? S_OK : E_FAIL;
}

HRESULT HvSetSendTimeOut(const HV_SOCKET_HANDLE& hSocket, int iMS)
{
    struct timeval uTime;
    uTime.tv_sec = iMS / 1000;
    uTime.tv_usec = (iMS % 1000) * 1000;
    int iRet = SetSockOpt(hSocket, SOL_SOCKET, SO_SNDTIMEO, (char*)&uTime, sizeof(timeval));
    return (0 == iRet) ? S_OK : E_FAIL;
}

//注：Linux下异步SOCKET的连接并没有超时的说法，因此在调用该函数失败后请自行Sleep一段时间。
HRESULT HvConnect(const HV_SOCKET_HANDLE& hSocket, const char* pszIPv4, WORD16 wPort, int iTimeout/*=-1*/)
{
    if (INVALID_SOCKET == hSocket || NULL == pszIPv4 )	return E_INVALIDARG;

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(wPort);
    addr.sin_addr.s_addr = inet_addr(pszIPv4);

    int iRtn = 0;
    if ( -1 == iTimeout )
    {
        iRtn = Connect(hSocket, (sockaddr*)&addr, sizeof(addr));
        return (0 == iRtn) ? S_OK : E_FAIL;
    }

    HRESULT hr = E_FAIL;

    // 设为非阻塞模式
    int flags = fcntl(hSocket, F_GETFL, 0);
    flags |= O_NONBLOCK;
    fcntl(hSocket, F_SETFL, flags);

    iRtn = Connect(hSocket, (sockaddr*)&addr, sizeof(addr));

    if ( 0 == iRtn )
    {
        hr = S_OK;
    }
    else
    {
        fd_set w;
        FD_ZERO(&w);
        FD_SET(hSocket, &w);

        struct timeval timeout;
        timeout.tv_sec = iTimeout / 1000;
        timeout.tv_usec = (iTimeout % 1000) * 1000;

        int ret = select(hSocket + 1, 0, &w, 0, &timeout);
        if (ret <= 0)
        {
            hr = E_FAIL;
        }
        else if (!FD_ISSET(hSocket, &w))
        {
            hr = E_FAIL;
        }
        else
        {
            ret = 0;
            int errLen = sizeof(int);
            if (getsockopt(hSocket, SOL_SOCKET, SO_ERROR, &ret, (socklen_t*)&errLen) < 0
                || ret != 0)
            {
                hr = E_FAIL;
            }
            else
            {
                hr = S_OK;
            }
        }
    }

    // 设为阻塞模式
    flags = fcntl(hSocket, F_GETFL, 0);
    flags &= (~O_NONBLOCK);
    fcntl(hSocket, F_SETFL, flags);
    return hr;
}

HRESULT HvAccept(const HV_SOCKET_HANDLE& hSocket, HV_SOCKET_HANDLE& hNewSocket, int iTimeout/*=-1*/)
{
    if (INVALID_SOCKET == hSocket)	return E_INVALIDARG;

    struct hv_sockaddr addr;
    int addrlen = sizeof(struct hv_sockaddr);

    if ( -1 == iTimeout )
    {
        hNewSocket = Accept(hSocket, &addr, &addrlen);
        return (INVALID_SOCKET!=hNewSocket) ? S_OK : E_FAIL;
    }

    HRESULT hr = E_FAIL;
    fd_set r;
    FD_ZERO(&r);
    FD_SET(hSocket, &r);

    struct timeval timeout;
    timeout.tv_sec = iTimeout / 1000;
    timeout.tv_usec = (iTimeout % 1000) * 1000;

    int ret = select(hSocket+1, &r, NULL, NULL, &timeout);
    if (ret == 1)
    {
        hNewSocket = Accept(hSocket, &addr, &addrlen);
        hr = ( INVALID_SOCKET != hNewSocket ) ? S_OK : E_FAIL;
    }
    else if (ret == 0)
    {
        hr = S_FALSE;
    }
    else
    {
        hr = E_FAIL;
    }
    return hr;
}

HRESULT HvGetPeerName(const HV_SOCKET_HANDLE& hSocket, DWORD32* pdwIPv4, WORD16* pwPort)
{
    struct hv_sockaddr name;
    int namelen = sizeof(struct hv_sockaddr);

    if ( 0 != GetPeerName(hSocket, &name, &namelen) )
    {
        if (pwPort) *pwPort = 0;
        if (pdwIPv4) *pdwIPv4 = 0;

        return E_FAIL;
    }
    else
    {
        WORD16 wPort = ((struct hv_sockaddr_in*)&name)->sin_port;
        DWORD32 dwIPv4 = ((struct hv_sockaddr_in*)&name)->sin_addr.s_addr;

        if(pwPort) *pwPort = ntohs(wPort);
        if(pdwIPv4) *pdwIPv4 = dwIPv4;

        return S_OK;
    }
}

HRESULT HvUDPBind(const HV_SOCKET_HANDLE& hSocket, WORD16 wLocalPort)
{
    if (hSocket == INVALID_SOCKET)
    {
        return E_HANDLE;
    }

    struct hv_sockaddr_in sa;
    memset(&sa, 0, sizeof(struct hv_sockaddr));

    sa.sin_family = AF_INET;
    sa.sin_port = htons(wLocalPort);
    sa.sin_addr.s_addr = htonl(INADDR_ANY);

    int iRet = -1;
    iRet = Bind(hSocket, (hv_sockaddr*)&sa, sizeof(struct hv_sockaddr));

    return iRet;
}
