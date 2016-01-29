/**
* @file		HvCmdLink.h
* @brief	命令连接核心部分
*/

#ifndef _HV_CMD_LINK_
#define _HV_CMD_LINK_

#include "hvutils.h"
#include "hvthreadbase.h"
#include "HvSockUtils.h"
#include "SocketStream.h"
#include "hv_opt.h"
#include "hvsysinterface.h"
#include "ControlCommand.h"
using namespace HiVideo;

class CNetCmd
{
public:
    static const int COMMAND_PORT = HV_COMMAND_2_PORT;
    static HRESULT RecvCmdHeader(HvSys::HV_CMD_INFO *pInfo, HvCore::IHvStream* pStream)
    {
        if (pInfo == NULL || pStream == NULL)
        {
            return E_INVALIDARG;
        }
        HRESULT hr = S_OK;

        struct
        {
            unsigned int dwLen;
            unsigned int dwType;
        } cCmdReceived;
        DWORD32 dwLen;

        hr = pStream->Read(&cCmdReceived, 8, &dwLen);
        if (FAILED(hr))
        {
            return hr;
        }
        if (dwLen != 8)
        {
            return E_FAIL;
        }
        if (( 8 == cCmdReceived.dwLen ) && ( 0x000001FF == cCmdReceived.dwType ))
        {
            DWORD32 dwBuffer;
            hr = pStream->Read(&dwBuffer, 4, &dwLen);
            if (FAILED(hr))
            {
                return hr;
            }
            if (dwLen != 4)
            {
                return E_FAIL;
            }
        }

        pInfo->dwFlag = 0x02000000;
        pInfo->dwCmdID = cCmdReceived.dwType;
        pInfo->dwArgLen = cCmdReceived.dwLen - 4;
        return hr;
    }

    static HRESULT SendRespond( HvSys::HV_CMD_RESPOND* pInfo, HvCore::IHvStream* pStream)
    {
        if ( pInfo == NULL || pStream == NULL ) return E_INVALIDARG;

        struct
        {
            unsigned int dwLen;
            unsigned int dwType;
            int nResult;
        } cRespond;

        cRespond.dwLen = 8 + pInfo->dwArgLen;
        cRespond.dwType = pInfo->dwCmdID;
        cRespond.nResult = pInfo->nResult;

        UINT nRespondLen = 12;

        if ( (pInfo->dwFlag & NOT_SEND_RESULT) == NOT_SEND_RESULT )
        {
            cRespond.dwLen = 4 + pInfo->dwArgLen;
            nRespondLen = 8;
        }
        if ((cRespond.dwType == GET_INIFILE_COMMAND )
                || (cRespond.dwType == GET_RESETREPORT_COMMAND)
                || (cRespond.dwType == SET_PARAM_COMMAND)
                || (cRespond.dwType == RANDOM_READ_FLASH_COMMAND))
        {
            struct
            {
                unsigned int dwLen;
                unsigned int dwType;
                int nResult;
                unsigned int dwFileLen;
            }cResult;
            cResult.dwLen = 12 + pInfo->dwArgLen;
            cResult.dwType = cRespond.dwType;
            cResult.nResult = pInfo->nResult;
            cResult.dwFileLen =  pInfo->dwArgLen;
            return pStream->Write( &cResult, 16, NULL );
        }
        return pStream->Write( &cRespond, nRespondLen, NULL );
    }
};

//命令协议
template <class T>
class CCmdLink : public HvSys::ICmdDataLink
{
public:
    STDMETHOD(RecvCmdHeader)(HvSys::HV_CMD_INFO *pInfo)
    {
        return T::RecvCmdHeader(pInfo, m_pStream);
    }
    STDMETHOD(SendRespond)( HvSys::HV_CMD_RESPOND* pInfo )
    {
        return T::SendRespond(pInfo, m_pStream);
    }
    STDMETHOD(ReceiveData)(
        PVOID pbBuf,
        UINT nLen,
        UINT* pReceivedLen
    )
    {
        return m_pStream->Read(pbBuf, nLen, pReceivedLen);
    }
    STDMETHOD(SendData)(
        PVOID pbBuf,
        UINT nLen,
        UINT* pSendLen
    )
    {
        return m_pStream->Write(pbBuf, nLen, pSendLen);
    }

public:
    CCmdLink(HvCore::IHvStream* pStream): m_pStream(pStream) {};
    ~CCmdLink() {};

protected:
    HvCore::IHvStream* m_pStream;
};

//命令连接线程
template <class T>
class CCmdThread : public CHvThreadBase
{
public:
    CCmdThread();
    ~CCmdThread();

    virtual const char* GetName()
    {
        static char szName[] =  "CCmdThread";
        return szName;
    }

    virtual HRESULT Run(void *pvParamter);

    HRESULT Create(
        HV_SOCKET_HANDLE hSocket,
        HvSys::ICmdProcess *pCmdProc
    );

    HRESULT Close();
    bool IsConnected();
    bool ThreadIsOk();
    HRESULT GetPeerName( DWORD32* pdwAddress, int* piPort );

protected:
    bool m_fClosed;
    DWORD32 m_dwLastThreadIsOkTime;
    HV_SOCKET_HANDLE m_hSocket;
    HvSys::ICmdDataLink *m_pCmdLink;
    HvSys::ICmdProcess *m_pCmdProc;

public:
    bool m_fIsAutoLink;
};

//命令侦听线程
template <class T>
class CCmdListenThread : public CHvThreadBase
{
public:
    CCmdListenThread();
    ~CCmdListenThread();

    virtual const char* GetName()
    {
        static char szName[] = "CCmdListenThread";
        return szName;
    }

    virtual HRESULT Run(void *pvParamter);

    HRESULT Create(HvSys::ICmdProcess *pCmdProc, int iType = 0, char* szIP = NULL, int iPort = 0);

    HRESULT Close();
    HRESULT GetConnectIp( DWORD32* rgdwAddress, int* rgiPort, int* piCount );
    bool ThreadIsOk();

protected:
    bool m_fClosed;
    HV_SOCKET_HANDLE m_hSocket;
    HvSys::ICmdProcess *m_pCmdProc;
    int m_iType;
    char m_szIp[32];
    int m_iPort;
    bool m_fAutoIsConnected;
    static const int MAX_CONNECTION = 2;
    CCmdThread<T> m_rgCmdThread[MAX_CONNECTION];
    CCmdThread<T> m_cAutoCmdThread;
    DWORD32 m_dwLastThreadIsOkTime; // 控制喂狗操作时间
};

template<class T>
CCmdThread<T>::CCmdThread()
        :m_fClosed(true)
        ,m_hSocket(INVALID_SOCKET)
        ,m_pCmdLink(NULL)
        ,m_pCmdProc(NULL)
        ,m_fIsAutoLink(false)
{
    //Comment by Shaorg: 屏蔽SIGPIPE异常，该异常的默认处理将导致程序被Killed。
    sigset_t signal_mask;
    sigemptyset(&signal_mask);
    sigaddset(&signal_mask, SIGPIPE);
    int rc = pthread_sigmask(SIG_BLOCK, &signal_mask, NULL);
    if (rc != 0)
    {
        HV_Trace(1, "pthread_sigmask is error.\n");
    }

    m_dwLastThreadIsOkTime = GetSystemTick();
}

template<class T>
CCmdThread<T>::~CCmdThread()
{
    Close();
}

template<class T>
HRESULT CCmdThread<T>::GetPeerName( DWORD32* pdwAddress, int* piPort )
{
    if ( m_hSocket != INVALID_SOCKET )
    {
        return HvGetPeerName(m_hSocket, pdwAddress, (WORD16*)piPort);
    }
    else
    {
        return S_FALSE;
    }
}

template<class T>
HRESULT CCmdThread<T>::Create(
    HV_SOCKET_HANDLE hSocket,
    HvSys::ICmdProcess *pCmdProc
)
{
    if (pCmdProc == NULL)
    {
        return E_INVALIDARG;
    }

    if ( INVALID_SOCKET == hSocket )
    {
        return E_FAIL;
    }

    HvSetRecvTimeOut(hSocket, 4*1000);

    CSocketStream* pcSockStream = new CSocketStream(hSocket);
    HvCore::IHvStream* pStream = (HvCore::IHvStream*)pcSockStream;

    Close();
    m_hSocket = hSocket;
    m_pCmdLink = new CCmdLink<T>(pStream);
    m_pCmdProc = pCmdProc;
    m_fClosed = false;

    m_dwLastThreadIsOkTime = GetSystemTick();

    //立即启动线程
    return this->Start(NULL);
}

template<class T>
HRESULT CCmdThread<T>::Close()
{
    m_fClosed = true;
    this->Stop(-1);

    if (m_pCmdLink != NULL)
    {
        delete m_pCmdLink;
        m_pCmdLink = NULL;
    }

    if (m_hSocket != INVALID_SOCKET)
    {
        CloseSocket(m_hSocket);
        m_hSocket = INVALID_SOCKET;
    }

    m_fIsAutoLink = false;
    return S_OK;
}

template<class T>
bool CCmdThread<T>::IsConnected()
{
    return !m_fClosed;
}

template<class T>
bool CCmdThread<T>::ThreadIsOk()
{
    if (IsConnected())
    {
        return GetSystemTick() - m_dwLastThreadIsOkTime < 120000;
    }
    else
    {
        return true;
    }
}

template<class T>
HRESULT CCmdThread<T>::Run(void *pvParamter)
{
    HV_Trace(3, "Cmd link run...\n");
    assert(m_hSocket != INVALID_SOCKET);
    assert(m_pCmdLink != NULL);
    assert(m_pCmdProc != NULL);

    HRESULT hr = S_OK;
    while (!m_fClosed)
    {
        m_dwLastThreadIsOkTime = GetSystemTick();

        HvSys::HV_CMD_INFO cInfo;
        hr = m_pCmdLink->RecvCmdHeader(&cInfo);
        if (FAILED(hr))
        {
            m_fClosed = true;
            break;
        }
        // 如果是心跳包，则不做处理
        if (0x000001FF == cInfo.dwCmdID)
        {
            continue;
        }
        //线程同步应该在Process中处理
        hr = m_pCmdProc->Process(&cInfo, m_pCmdLink);
        if (FAILED(hr))
        {
            HvSys::HV_CMD_RESPOND cRespond;
            cRespond.dwFlag = 0;
            cRespond.dwCmdID = UNKNOW_COMMAND;
            cRespond.dwArgLen = 0;
            cRespond.nResult = S_OK;

            m_pCmdLink->SendRespond(&cRespond);

            //当处理命令出错的原因是网络问题时才退出命令处理线程
            if( hr == NET_FAILED )
            {
                m_fClosed = true;
                break;
            }
        }
    }  // end while

    CloseSocket(m_hSocket);
    m_hSocket = INVALID_SOCKET;
    HV_Trace(3, "Cmd link exit...\n");

    return hr;
}

template<class T>
CCmdListenThread<T>::CCmdListenThread()
        :m_fClosed(false)
        ,m_hSocket(INVALID_SOCKET)
        ,m_pCmdProc(NULL)
        ,m_iType(0)
        ,m_iPort(6664)
        ,m_fAutoIsConnected(false)
{
    m_dwLastThreadIsOkTime = GetSystemTick();
    strcpy(m_szIp, "100.100.100.101");
}

template<class T>
CCmdListenThread<T>::~CCmdListenThread()
{
    Close();
}

template<class T>
HRESULT CCmdListenThread<T>::GetConnectIp( DWORD32* rgdwAddress, int* rgiPort, int* piCount )
{
    if ( piCount == NULL )
    {
        return E_INVALIDARG;
    }

    *piCount = 0;
    int iPos = 0;
    for (int i = 0; i < MAX_CONNECTION; i++)
    {
        if (m_rgCmdThread[i].IsConnected())
        {
            m_rgCmdThread[i].GetPeerName( rgdwAddress + iPos, rgiPort + iPos );
            ++iPos;
        }
    }

    *piCount = iPos;
    return S_OK;
}

template<class T>
HRESULT CCmdListenThread<T>::Close()
{
    this->Stop(-1);

    if (m_hSocket != INVALID_SOCKET)
    {
        CloseSocket(m_hSocket);
        m_hSocket = INVALID_SOCKET;
    }

    return S_OK;
}

#define PASSIVE_CONNECT_MODE 0

template<class T>
HRESULT CCmdListenThread<T>::Create(HvSys::ICmdProcess *pCmdProc, int iConnectType, char* szIP, int iPort)
{
    HRESULT hr = S_OK;
    if (pCmdProc == NULL) return E_INVALIDARG;

    Close();
    m_pCmdProc = pCmdProc;

    if ( szIP != NULL ) strcpy(m_szIp, szIP);
    m_iPort = iPort;

    if ( iConnectType == PASSIVE_CONNECT_MODE )
    {
        m_hSocket = HvCreateSocket();
        if ( INVALID_SOCKET == m_hSocket ) return E_FAIL;

        hr = HvListen(m_hSocket, T::COMMAND_PORT, MAX_CONNECT_TEAM_COUNT);
        if (FAILED(hr)) return hr;
    }

    m_iType = iConnectType;
    m_fClosed = false;

    return this->Start(NULL);
}

template<class T>
HRESULT CCmdListenThread<T>::Run(void *pvParamter)
{
    if (NULL == m_pCmdProc)
    {
        m_fClosed = true;
        return E_POINTER;
    }

    HRESULT hr = S_OK;

    HV_SOCKET_HANDLE hClientSocket = INVALID_SOCKET;

    while (!m_fClosed)
    {
        m_dwLastThreadIsOkTime = GetSystemTick();

        if ( m_iType == PASSIVE_CONNECT_MODE )
        {
            if ( m_hSocket == INVALID_SOCKET )
            {
                m_fClosed = true;
                return E_POINTER;
            }

            int i;
            for (i = 0; i < MAX_CONNECT_TEAM_COUNT; i++)
            {
                if (!m_rgCmdThread[i].IsConnected())
                {
                    break;
                }
            }

            //如果当前连接数>=指定值，则不再Accept其它连接
            //modify by huanggr 2011-03-23
            if (i >= MAX_CONNECT_TEAM_COUNT)
            {
                HV_Sleep(4000);
                continue;
            }

            hClientSocket = INVALID_SOCKET;
            hr = HvAccept(m_hSocket, hClientSocket, 4000);
            if (hr != S_OK || INVALID_SOCKET == hClientSocket )
            {
                continue;
            }

            m_rgCmdThread[i].Create(hClientSocket, m_pCmdProc);
            m_rgCmdThread[i].m_fIsAutoLink = false;
            hClientSocket = INVALID_SOCKET;
        }
        else
        {
            //主动连接模式
//            HV_Sleep(1000);
//            m_fAutoIsConnected = false;
//            if ( m_cAutoCmdThread.IsConnected() )
//            {
//                m_fAutoIsConnected = true;
//            }
//
//            if ( !m_fAutoIsConnected )
//            {
//                if ( hClientSocket == INVALID_SOCKET )
//                {
//                    hClientSocket = HvCreateSocket();
//                    if ( INVALID_SOCKET == hClientSocket ) continue;
//                }
//
//                if ( S_OK != HvConnect(hClientSocket, m_szIp, m_iPort, 3000) ) continue;
//                HvSetRevTimeOut(hClientSocket, 4000);
//
//                m_cAutoCmdThread.Create(hClientSocket, m_pCmdProc);
//
//                hClientSocket = INVALID_SOCKET;
//                m_fAutoIsConnected = true;
//            }
        }
    }  // end while

    if ( hClientSocket != INVALID_SOCKET )
    {
        CloseSocket(hClientSocket);
        hClientSocket = INVALID_SOCKET;
    }
    return hr;
}

template<class T>
bool CCmdListenThread<T>::ThreadIsOk()
{
    if (m_fClosed) return false;
    for (int i = 0; i < MAX_CONNECT_TEAM_COUNT; i++)
    {
        if (!m_rgCmdThread[i].ThreadIsOk())
        {
            return false;
        }
    }
    return true;
}

//上位机命令处理类
class CPcCmdProc : public HvSys::ICmdProcess
{
public:
    CPcCmdProc()
    {
        CreateSemaphore(&m_hSemLock,1,1);
    };

    virtual ~CPcCmdProc()
    {
        DestroySemaphore(&m_hSemLock);
    };

    STDMETHOD(Process)(HvSys::HV_CMD_INFO* pCmdInfo, HvSys::ICmdDataLink* pCmdDataLink)
    {
        if ( pCmdInfo == NULL || pCmdDataLink == NULL )
        {
            return E_INVALIDARG;
        }
        else
        {
            HRESULT hr = E_FAIL;

            SemPend(&m_hSemLock);
            hr = ExecuteCommand(pCmdInfo, pCmdDataLink);
            SemPost(&m_hSemLock);

            return hr;
        }
    };

protected:
    HV_SEM_HANDLE m_hSemLock;
};

#endif
