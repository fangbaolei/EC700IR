#include "hvsocket.h"

namespace HiVideo
{

HV_SOCKET_HANDLE Socket(int af, int type, int protocol)
{
    return socket(af, type, protocol);
}

int Connect(HV_SOCKET_HANDLE s, const struct hv_sockaddr* name, int namelen)
{
    return connect(s, name, namelen);
}

int Bind(HV_SOCKET_HANDLE s, const struct hv_sockaddr* name, int namelen)
{
    return bind(s, name, namelen);
}

int Listen(HV_SOCKET_HANDLE s, int backlog)
{
    return listen(s, backlog);
}

HV_SOCKET_HANDLE Accept(HV_SOCKET_HANDLE s,	struct hv_sockaddr* addr, int* addrlen)
{
    return accept(s, addr, (socklen_t*)addrlen);
}

int Send(HV_SOCKET_HANDLE s, const char* buf, int len, int flags)
{
    return send(s, buf, len, flags);
}

int Recv(HV_SOCKET_HANDLE s, char* buf, int len, int flags)
{
    return recv(s, buf, len, flags);
}

int SendTo(HV_SOCKET_HANDLE s, const char* buf, int len, int flags, const struct hv_sockaddr* to, int tolen)
{
    return sendto(s, buf, len, flags, to, tolen);
}

int RecvFrom(HV_SOCKET_HANDLE s, char* buf, int len, int flags, struct hv_sockaddr* from, int* fromlen)
{
    return recvfrom(s, buf, len, flags, from, (socklen_t*)fromlen);
}

int CloseSocket(HV_SOCKET_HANDLE s)
{
    return close(s);
}

int SetSockOpt(HV_SOCKET_HANDLE s, int level, int optname, const char* optval, int optlen)
{
    return setsockopt(s, level, optname, optval, optlen);
}

int GetPeerName(HV_SOCKET_HANDLE s, struct hv_sockaddr* name, int* namelen)
{
    return getpeername(s, name, (socklen_t*)namelen);
}

int GetSockName(HV_SOCKET_HANDLE s, struct hv_sockaddr* name, int* namelen)
{
    return getsockname(s, name, (socklen_t*)namelen);
}

}
