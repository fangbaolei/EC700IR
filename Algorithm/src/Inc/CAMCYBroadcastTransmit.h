#ifndef CAMCYBROADCASTTRANSMIT_H
#define CAMCYBROADCASTTRANSMIT_H

#include "BroadcastTransmit.h"

class CCAMCYBroadcastTransmit : public CBroadcastTransmit
{
public:
    CCAMCYBroadcastTransmit();
    virtual ~CCAMCYBroadcastTransmit();

    int Run();
    void Stop();

protected:

private:

    static int CallBack_SendCY(char* buf, int* pLen, void* pUserData);
    static int CallBack_GetTransmitDataCY(char* buf, int* pLen, void* pUserData);
    int InitListenCam();
    unsigned long m_ulFromIP;
    unsigned short m_wFromPort;
    int m_fdListenCam;
};

#endif // CAMCYBROADCASTTRANSMIT_H
