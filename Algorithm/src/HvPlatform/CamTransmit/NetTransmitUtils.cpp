#include "NetTransmitUtils.h"
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>

int SocketWait(int iSock, bool rd, bool wr, int iTimeUS)
{
    fd_set rfds, wfds;
    struct timeval tv;
    if (iSock == -1)
    {
        return 0;
    }

    FD_ZERO(&rfds);
    FD_ZERO(&wfds);
    if (rd)
        FD_SET(iSock, &rfds);
    if (wr)
        FD_SET(iSock, &wfds);

    tv.tv_sec = iTimeUS / 1000000; //seconds
    tv.tv_usec = iTimeUS % 1000000; //us

    for (;;)
    {
        int iSelect = 0;
        iSelect = select(iSock+1, &rfds, &wfds, NULL, ((iTimeUS==-1) ? NULL : &tv));
        switch (iSelect)
        {
        case 0: /*time out*/
            return 0;

        case (-1): /*socket error*/
            if ( errno == EINTR ) break;
            return 0;

        default:
        {
            if (FD_ISSET(iSock, &rfds))
                iSelect = 1;
            else if (FD_ISSET(iSock, &wfds))
                iSelect = 2;
            else
                return 0;

            int iError = 0;
            int iLen = 0;
            if (-1 == getsockopt(iSock, SOL_SOCKET, SO_ERROR, &iError, (socklen_t *)&iLen))
                return 0;

            // 如果没错，error值不变
            return (iError == 0) ? iSelect : 0 ;
        }
        }
    }
}

void CleanSockRecvBuf(int iSock)
{
    struct timeval tmOut;
    tmOut.tv_sec = 0;
    tmOut.tv_usec = 0;

    if (iSock == -1)
    {
        return;
    }

    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(iSock, &fds);

    int nRet;

    char rgbTmp[2];

    memset(rgbTmp, 0, sizeof(rgbTmp));

    while (1)
    {
        nRet= select(FD_SETSIZE, &fds, NULL, NULL, &tmOut);
        if (nRet== 0)
        {
            break;
        }
        else if (nRet==-1) /*socket error*/
        {
            if ( errno==EINTR ) break;
            return;
        }
        if ( recv(iSock, rgbTmp, 1, MSG_NOSIGNAL) <= 0 )
        {
            return;
        }
    }
}

int TrsmBroadcastSend(unsigned long ulIPLanOut,
                      unsigned short wPortLanOut,
                      const void* buf,
                      int Len,
                      int flags,
                      const struct sockaddr* to,
                      socklen_t toLen)
{
    int iRet = 0;
    int hSockBroadcast = -1;
    hSockBroadcast = socket(AF_INET, SOCK_DGRAM, 0);

    if ( -1 != hSockBroadcast )
    {
        int iOpt = 1;
        int iOptLen = sizeof(iOpt);
        setsockopt(hSockBroadcast, SOL_SOCKET, SO_BROADCAST, (const char*)&iOpt, iOptLen);
        setsockopt(hSockBroadcast, SOL_SOCKET, SO_REUSEADDR, (const char*)&iOpt, iOptLen);

        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));

        addr.sin_family = AF_INET;
        addr.sin_port = htons(wPortLanOut);
        addr.sin_addr.s_addr = htonl(ulIPLanOut);

        if ((ulIPLanOut == INADDR_ANY)
            || (0 == bind(hSockBroadcast, (const struct sockaddr*)&addr, sizeof(addr))))
        {
            iRet = sendto(hSockBroadcast, buf, Len, flags, to, toLen);
        }

        close(hSockBroadcast);
    }

    return iRet;
}
