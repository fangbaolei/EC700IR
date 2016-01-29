#ifndef CUDPTRANSMIT_H
#define CUDPTRANSMIT_H
#include <map>
#include <sys/socket.h>
#include "CTransmit.h"
#include "NetTransmitUtils.h"

class CUdpTransmit
{
public:
    CUdpTransmit();
    virtual ~CUdpTransmit();

    int SetListen(unsigned long ulBindIP, unsigned short wBindPort);
    int SetTransmit(unsigned long ulTransIP,
                    unsigned short wTransPort,
                    unsigned long ulBindIP);

    void SetClientCominCallBack(CALLBACK_TRANSMIT_STARTUP_FUNC func, void* pUserData);
    void SetClientExitCallBack(CALLBACK_TRANSMIT_STARTUP_FUNC func, void* pUserData);

    int Run();
    void Stop();

protected:

    std::map<int, STransInfo> m_SockInfoMap;

    int m_fdSockListen;

    int m_fdSockTransmit;

    STransInfo m_sCurTransInfo;

    CTransmit m_cTransmit;
    time_t m_lastChkConnTime;

    unsigned long m_ulListenIP;
    unsigned long m_ulListenPort;

    unsigned long m_ulTransIP;        // the IP transmit to
    unsigned short m_wTransPort;      // the Port transmit to
    unsigned long m_ulTransBindIP;    // the WAN IP use to transmit

    BOOL m_fIsClientComin;

    int InitListen();

    int SelectTransmitData(int* pFdReady, int timeoUs);
    int CreateUdpSock(unsigned long ulIP, unsigned short wPort);
    int CheckLink(unsigned long ulIPIn, unsigned short wPortIn);
    int CheckLinkTimeOut();

    CALLBACK_TRANSMIT_STARTUP_FUNC m_funcClientCominCallBack;
    CALLBACK_TRANSMIT_STARTUP_FUNC m_funcClientExitCallBack;
    void* m_pClientCominCallBackUserData;
    void* m_pClientExitCallBackUserData;

    static int CallBack_Startup(void* pUserData);
    static int CallBack_Recv(char* buf, int* pLen, void* pUserData);
    static int CallBack_Send(char* buf, int* pLen, void* pUserData);
    static int CallBack_TransmitData(char* buf, int* pLen, void* pUserData);
    static int CallBack_GetTransmitData(char* buf, int* pLen, void* pUserData);

private:
};

#endif // CUDPTRANSMIT_H
