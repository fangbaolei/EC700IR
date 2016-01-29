#ifndef BROADCASTTRANSMIT_H
#define BROADCASTTRANSMIT_H

#include "NetTransmitUtils.h"
#include "CTransmit.h"

class CBroadcastTransmit
{
public:
    CBroadcastTransmit();
    virtual ~CBroadcastTransmit();

    int SetListen(unsigned long ulListenIP,
                  unsigned long ulListenMask,
                  unsigned short wListenPort);

    int SetTransmit(unsigned long ulTransIP,
                    unsigned long ulTransMask,
                    unsigned short wTransPort);

    int Run();
    void Stop();

protected:
    int m_fdSock;
    STransInfo m_sCurTransInfo;
    CTransmit m_cTransmit;

    unsigned long m_ulListenIP;
    unsigned short m_ulListenPort;
    unsigned long m_ulListenMask;

    unsigned long m_ulTransIP;        // the IP transmit out
    unsigned short m_wTransPort;      // the Port transmit out
    unsigned long m_ulTransMask;

    int InitListen();

    static int CallBack_Startup(void* pUserData);
    static int CallBack_Recv(char* buf, int* pLen, void* pUserData);
    static int CallBack_Send(char* buf, int* pLen, void* pUserData);

private:

};

#endif // BROADCASTTRANSMIT_H
