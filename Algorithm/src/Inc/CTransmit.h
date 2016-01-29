#ifndef CNETTRANSMITBASE_H
#define CNETTRANSMITBASE_H

//#define _DEBUG_TRANSMIT

#ifndef PRINTF
#ifdef _DEBUG_TRANSMIT
#define PRINTF printf
#else
#define PRINTF(...)
#endif // #ifdef _DEBUG_TRANSMIT
#endif // #ifndef PRINTF

#include "hvutils.h"
#include "hvthreadbase.h"

typedef int (*CALLBACK_TRANSMIT_STARTUP_FUNC) (void* pUserData);
typedef int (*CALLBACK_TRANSMIT_FUNC) (char* buf, int* pLen, void* pUserData);

class CTransmit : protected CHvThreadBase
{
public:
    CTransmit();
    virtual ~CTransmit();

    virtual int Run();
    virtual void Stop();

    void SetStartupCallback(
             CALLBACK_TRANSMIT_STARTUP_FUNC funcCallback,
             void* pUserCallBackData
             );

    // retrun < 0 , exit thread
    // retrun 0 , restartup
    // retrun 1 , continue
    // retrun 2 , rerecv when transmit until no data recv
    void SetRecvCallback(
             CALLBACK_TRANSMIT_FUNC funcCallback,
             void* pUserCallBackData
             );
    void SetSendCallback(
             CALLBACK_TRANSMIT_FUNC funcCallback,
             void* pUserCallBackData
             );
    void SetTransmitDataCallback(
             CALLBACK_TRANSMIT_FUNC funcCallback,
             void* pUserCallBackData
             );
    void SetGetTransmitDataCallback(
             CALLBACK_TRANSMIT_FUNC funcCallback,
             void* pUserCallBackData
             );

    void SetClientCominDataCallback(
             CALLBACK_TRANSMIT_STARTUP_FUNC funcCallback,
             void* pUserCallBackData
             );

    void SetClientExitDataCallback(
             CALLBACK_TRANSMIT_STARTUP_FUNC funcCallback,
             void* pUserCallBackData
             );

    void SetTransmitBufLen(int iLen);

protected:
    CALLBACK_TRANSMIT_STARTUP_FUNC m_funcStartupCallback;
    CALLBACK_TRANSMIT_STARTUP_FUNC m_funcClientCominCallBack;
    CALLBACK_TRANSMIT_STARTUP_FUNC m_funcClientExitCallBack;
    CALLBACK_TRANSMIT_FUNC m_funcRecvCallback;
    CALLBACK_TRANSMIT_FUNC m_funcSendCallback;
    CALLBACK_TRANSMIT_FUNC m_funcTransmitDataCallback;
    CALLBACK_TRANSMIT_FUNC m_funcGetTransmitDataCallback;

    void* m_pUserDataStartup;
    void* m_pUserDataClientComin;
    void* m_pUserDataClientExit;
    void* m_pUserDataRecv;
    void* m_pUserDataSend;
    void* m_pUserDataTransmit;
    void* m_pUserDataGetTransmitData;

    bool m_fKeepRunning;
    bool m_fIsStop;
    int m_iBufLen;
    char* m_pTransmitBuf;

    virtual HRESULT Run(void* pvParam);

private:
	pthread_t m_thread;
	int _run();
};

#endif // CNETTRANSMITBASE_H
