#ifndef CTCPTRANSMIT_H
#define CTCPTRANSMIT_H

#include "hvutils.h"
#include "CTransmit.h"

class CTcpTransmit
{
public:
    CTcpTransmit();
    virtual ~CTcpTransmit();

    int Run();
    void Stop();

    int SetListen(unsigned long ulBindIP, unsigned short wBindPort);
    int SetTransmit(unsigned long ulTransIP,
                    unsigned short wTransPort,
                    unsigned long ulBindIP,
                    unsigned short wBindPort);

    void SetClientCominCallBack(CALLBACK_TRANSMIT_STARTUP_FUNC func, void* pUserData);
    void SetClientExitCallBack(CALLBACK_TRANSMIT_STARTUP_FUNC func, void* pUserData);

    void SetClientDefSockBufLen(int iRecvBufLen, int iSendBufLen);
    void SetTransmitDefSockBufLen(int iRecvBufLen, int iSendBufLen);

protected:
    CTransmit m_cTransmit;

    unsigned long m_ulListenIP;
    unsigned long m_ulListenPort;

    unsigned long m_ulTransIP;        // the IP transmit to
    unsigned short m_wTransPort;      // the Port transmit to
    unsigned long m_ulTransBindIP;    // the WAN IP use to transmit
    unsigned short m_wTransBindPort;

    int m_fdSockListen;
    int m_fdSockListenClient;
    int m_fdSockTransmit;


    int m_iClientDefSockSndBufLen;
    int m_iClientDefSockRcvBufLen;
    int m_iTransmitDefSockSndBufLen;
    int m_iTransmitDefSockRcvBufLen;

    CALLBACK_TRANSMIT_STARTUP_FUNC m_funcClientCominCallBack;
    CALLBACK_TRANSMIT_STARTUP_FUNC m_funcClientExitCallBack;
    void* m_pClientCominCallBackUserData;
    void* m_pClientExitCallBackUserData;

    int InitListen();
    int InitTransmit();

    static int CallBack_Startup(void* pUserData);
    static int CallBack_Recv(char* pBuf, int* pLen, void* pUserData);
    static int CallBack_Send(char* pBuf, int* pLen, void* pUserData);
    static int CallBack_TransmitData(char* pBuf, int* pLen, void* pUserData);
    static int CallBack_GetTransmitData(char* pBuf, int* pLen, void* pUserData);

private:
};

#endif // CTCPTRANSMIT_H
