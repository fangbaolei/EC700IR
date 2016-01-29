#ifndef CAMNVCBROADCASTTRANSMIT_H
#define CAMNVCBROADCASTTRANSMIT_H

#include "BroadcastTransmit.h"

class CCAMNVCBroadcastTransmit : public CBroadcastTransmit
{
public:
    CCAMNVCBroadcastTransmit();
    virtual ~CCAMNVCBroadcastTransmit();

    int Run();
protected:

private:
    static int CallBack_SendNVC(char* buf, int* pLen, void* pUserData);
    static int CallBack_GetTransmitDataNVC(char* buf, int* pLen, void* pUserData);
    unsigned long m_ulFromIP;
    unsigned short m_wFromPort;
};

#endif // CAMNVCBROADCASTTRANSMIT_H
