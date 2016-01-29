#ifndef NETTRANSMITUTILS_INCLUDED
#define NETTRANSMITUTILS_INCLUDED

#include <errno.h>
#include <sys/socket.h>
#include <string.h>

typedef struct tag_TransInfo
{
    int fd;
    unsigned long ulIP;
    unsigned short wPort;
    unsigned short wTransmitPort;
    time_t tLastTransTime;
} STransInfo;

int SocketWait(int iSock, bool rd, bool wr, int iTimeMS);

void CleanSockRecvBuf(int iSock);

int TrsmBroadcastSend(unsigned long ulIPLanOut,
                      unsigned short wPortLanOut,
                      const void* buf,
                      int Len,
                      int flags,
                      const struct sockaddr* to,
                      socklen_t toLen);

#endif // NETTRANSMITUTILS_INCLUDED
