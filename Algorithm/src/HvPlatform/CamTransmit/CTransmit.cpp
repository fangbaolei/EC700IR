#include "CTransmit.h"
#include <unistd.h>
#include <stdio.h>

CTransmit::CTransmit()
{
    m_funcRecvCallback = m_funcSendCallback
        = m_funcTransmitDataCallback = m_funcGetTransmitDataCallback
        = NULL;

    m_pUserDataRecv = m_pUserDataSend
        = m_pUserDataTransmit = m_pUserDataGetTransmitData
        = NULL;

    m_fIsStop = true;

    m_iBufLen = 10 * 1024;
    m_pTransmitBuf = NULL;
}

CTransmit::~CTransmit()
{
    if (m_pTransmitBuf != NULL)
    {
        delete m_pTransmitBuf;
    }

    if (!m_fIsStop)
    {
        Stop();
    }
}

int CTransmit::Run()
{
    if (    (m_funcStartupCallback == NULL)
         || (m_funcRecvCallback == NULL)
         || (m_funcSendCallback == NULL)
         || (m_funcTransmitDataCallback == NULL)
         || (m_funcGetTransmitDataCallback == NULL)
         || (m_pUserDataStartup == NULL)
         || (m_pUserDataRecv == NULL)
         || (m_pUserDataSend == NULL)
         || (m_pUserDataTransmit == NULL)
         || (m_pUserDataGetTransmitData == NULL)
    )   return 0;

    Start(NULL);
    return 1;
}

void CTransmit::Stop()
{
    m_fKeepRunning = false;

    while (!m_fIsStop)
        usleep(100*1000);

    CHvThreadBase::Stop();
}

int CTransmit::_run()
{
    PRINTF("run\n");

    if (m_pTransmitBuf != NULL)
    {
        delete m_pTransmitBuf;
    }

    m_pTransmitBuf = new char[m_iBufLen];
    char* pBuf = m_pTransmitBuf;
    if (pBuf == NULL)
    {
        return 0;
    }

    int iDataLen;
    int iRet = 0;
    m_fKeepRunning = true;
    m_fIsStop = false;

    BOOL fIsClentComin = TRUE;

    while(m_fKeepRunning)
    {
        if (m_funcClientExitCallBack && fIsClentComin)
        {
            fIsClentComin = FALSE;
            m_funcClientExitCallBack(m_pUserDataClientExit);
        }

        iRet = m_funcStartupCallback(m_pUserDataStartup);
        if (iRet == 0)
        {
            continue;
        }
        else if (iRet < 0)
        {
            break;
        }

        while (m_fKeepRunning)
        {
            if (m_funcClientCominCallBack && !fIsClentComin)
            {
                fIsClentComin = TRUE;
                m_funcClientCominCallBack(m_pUserDataClientComin);
            }

            int iRecvRet;
            do
            {
                iDataLen = m_iBufLen;
                iRecvRet = m_funcRecvCallback(pBuf, &iDataLen, m_pUserDataRecv);
                if (iRecvRet <= 0)
                {
                    iRet = iRecvRet;
                    goto L_GET_ERR;
                }

                iRet = m_funcTransmitDataCallback(pBuf,
                            &iDataLen,
                            m_pUserDataTransmit);
                if (iRet <= 0)
                {
                    goto L_GET_ERR;
                }

            } while (iRecvRet == 2);  // 如果返回2，则发送之后继续接收

            do
            {
                iDataLen = m_iBufLen;
                iRecvRet = m_funcGetTransmitDataCallback(pBuf,
                            &iDataLen,
                            m_pUserDataGetTransmitData);
                if (iRecvRet <= 0)
                {
                    iRet = iRecvRet;
                    goto L_GET_ERR;
                }

                iRet = m_funcSendCallback(pBuf, &iDataLen, m_pUserDataSend);
                if (iRet <= 0)
                {
                    goto L_GET_ERR;
                }

            } while (iRecvRet == 2); // 如果返回2，则发送之后继续接收
        }

L_GET_ERR:
        if (iRet < 0)
        {
            break;
        }
    }

    delete m_pTransmitBuf;
    m_pTransmitBuf = NULL;
    m_fIsStop = true;

    return 1;
}

void CTransmit::SetTransmitBufLen(int iLen)
{
    m_iBufLen = iLen;
}

void CTransmit::SetStartupCallback(CALLBACK_TRANSMIT_STARTUP_FUNC funcCallback,
                                   void* pUserCallBackData)
{
    m_funcStartupCallback = funcCallback;
    m_pUserDataStartup = pUserCallBackData;
}

void CTransmit::SetRecvCallback(CALLBACK_TRANSMIT_FUNC funcCallback,
                                void* pUserCallBackData)
{
    m_funcRecvCallback = funcCallback;
    m_pUserDataRecv = pUserCallBackData;
}

void CTransmit::SetSendCallback(CALLBACK_TRANSMIT_FUNC funcCallback,
                                void* pUserCallBackData)
{
    m_funcSendCallback = funcCallback;
    m_pUserDataSend = pUserCallBackData;
}

void CTransmit::SetTransmitDataCallback(CALLBACK_TRANSMIT_FUNC funcCallback,
                                        void* pUserCallBackData)
{
    m_funcTransmitDataCallback = funcCallback;
    m_pUserDataTransmit = pUserCallBackData;
}

void CTransmit::SetGetTransmitDataCallback(CALLBACK_TRANSMIT_FUNC funcCallback,
                                           void* pUserCallBackData)
{
    m_funcGetTransmitDataCallback = funcCallback;
    m_pUserDataGetTransmitData = pUserCallBackData;
}

void CTransmit::SetClientCominDataCallback(CALLBACK_TRANSMIT_STARTUP_FUNC funcCallback,
                                           void* pUserCallBackData)
{
    m_funcClientCominCallBack = funcCallback;
    m_pUserDataClientComin = pUserCallBackData;
}

void CTransmit::SetClientExitDataCallback(CALLBACK_TRANSMIT_STARTUP_FUNC funcCallback,
                                           void* pUserCallBackData)
{
    m_funcClientExitCallBack = funcCallback;
    m_pUserDataClientExit = pUserCallBackData;
}

HRESULT CTransmit::Run(void* pvParam)
{
    return (_run()) ? S_OK : E_FAIL ;
}
