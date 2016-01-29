#ifndef _HV_CMD_LINK_
#define _HV_CMD_LINK_

#include "HvUtils.h"
#include <assert.h>
#include "HvInterface.h"
#include "HvSysInterface.h"
#include "HvThread.h"
#include "HvSocket.h"
#include "..\Protocol\hv_opt.h"

class COldNetCmd
{			
public:
	static const int COMMAND_PORT = HV_COMMAND_PORT;
	static HRESULT RecvCmdHeader(HvSys::HV_CMD_INFO *pInfo, HvCore::IHvStream* pStream)
	{
		if ( pInfo == NULL || pStream == NULL ) return E_INVALIDARG;

		HRESULT hr = S_OK;

		struct {
			unsigned short wLen;
			unsigned short wType;
		}
		cCmdReceived;

		DWORD32 dwLen = 0;
		hr = pStream->Read(&cCmdReceived, 4, &dwLen);
		if (FAILED(hr)) return hr;
		if (dwLen != 4) return E_FAIL;
		if ( (cCmdReceived.wType & 0xFF00) != 0x0100 ) 
		{
			cCmdReceived.wType = 0;
		}

		pInfo->dwFlag = 0x01000000;
		pInfo->dwCmdID = cCmdReceived.wType;
		pInfo->dwArgLen = cCmdReceived.wLen - 4;
		return hr;
	}

	static HRESULT SendRespond( HvSys::HV_CMD_RESPOND* pInfo, HvCore::IHvStream* pStream)
	{	
		if ( pInfo == NULL || pStream == NULL ) return E_INVALIDARG;

		struct {
			unsigned short wLen;
			unsigned short wType;
			int nResult;
		} cRespond;

		cRespond.wLen = 6 + pInfo->dwArgLen;
		cRespond.wType = pInfo->dwCmdID;
		cRespond.nResult = pInfo->nResult;

		UINT nRespondLen = 8;

		if ( (pInfo->dwFlag & NOT_SEND_RESULT) == NOT_SEND_RESULT )
		{
			cRespond.wLen = 2 + pInfo->dwArgLen;
			nRespondLen = 4;
		}

		return pStream->Write( &cRespond, nRespondLen, NULL );
	}		
};

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
		struct {
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

		struct {
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
			struct {
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
	CCmdLink(HvCore::IHvStream* pStream)
		: m_pStream(pStream)
	{};

	~CCmdLink() {};

protected:
	HvCore::IHvStream* m_pStream;
};

//命令连接线程
template <class T>
class CCmdThread
	:public HiVideo::IRunable
{
public:
	CCmdThread();
	~CCmdThread();
	HRESULT Create(
		HiVideo::ISocket *pSocket,
		HvSys::ICmdProcess *pCmdProc
		);
	HRESULT Close();
	bool IsConnected();
	bool ThreadIsOk();
	virtual HRESULT Run(void *pvParamter);

	HRESULT GetPeerName( DWORD32* pdwAddress, int* piPort );

protected:
	bool m_fClosed;
	DWORD32 m_dwLastThreadIsOkTime;
	HiVideo::IThread *m_pThread;
	HiVideo::ISocket *m_pSocket;
	HvSys::ICmdDataLink *m_pCmdLink;
	HvSys::ICmdProcess *m_pCmdProc;

public:
	bool m_fIsAutoLink;
};

//命令侦听线程
template <class T>
class CCmdListenThread
	: public HiVideo::IRunable
{
public:
	CCmdListenThread();
	~CCmdListenThread();
	virtual HRESULT Run(void *pvParamter);
	HRESULT Create(HvSys::ICmdProcess *pCmdProc, int iType = 0, char* szIP = NULL, int iPort = 0);
	HRESULT Close();

	HRESULT GetConnectIp( DWORD32* rgdwAddress, int* rgiPort, int* piCount );

	bool ThreadIsOk();

protected:
	bool m_fClosed;
	HiVideo::IThread *m_pThread;
	HiVideo::ISocket *m_pSocket;
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
,m_pThread(NULL)
,m_pSocket(NULL)
,m_pCmdLink(NULL)
,m_pCmdProc(NULL)
,m_fIsAutoLink(false)
{
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
	if( m_pSocket != NULL )
	{
		return m_pSocket->GetPeerName( pdwAddress, piPort );
	}
	else
	{
		return S_FALSE;
	}
}


template<class T>
HRESULT CCmdThread<T>::Create(
							  HiVideo::ISocket *pSocket,
							  HvSys::ICmdProcess *pCmdProc
							  )
{
	HRESULT hr;
	if (pSocket == NULL ||
		pCmdProc == NULL)
	{
		return E_INVALIDARG;
	}
	pSocket->SetRevTimeOut(4000);
	HvCore::IHvStream *pStream;
	hr = pSocket->GetStream(&pStream);
	if (FAILED(hr))
	{
		return E_FAIL;
	}
	Close();
	m_pSocket = pSocket;
	m_pCmdLink = new CCmdLink<T>(pStream);
	m_pCmdProc = pCmdProc;
	m_fClosed = false;
	m_dwLastThreadIsOkTime = GetSystemTick();
	return HiVideo::IThread::CreateInstance(
		&m_pThread,
		HiVideo::IThread::PRIORITY_NORMAL,
#if (RUN_PLATFORM == PLATFORM_DSP_BIOS)
		32760,
#else
		0,
#endif
		HiVideo::IThread::MEM_NORMAL,
		true,
		this);
}

template<class T>
HRESULT CCmdThread<T>::Close()
{
	m_fClosed = true;
	if (m_pThread != NULL)
	{
		m_pThread->Join();
		delete m_pThread;
		m_pThread = NULL;
	}
	if (m_pCmdLink != NULL)
	{
		delete m_pCmdLink;
		m_pCmdLink = NULL;
	}
	if (m_pSocket != NULL)
	{
		delete m_pSocket;
		m_pSocket = NULL;
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
//	HV_Trace("\n***CCmdThread::Run***\n");
	assert(m_pSocket != NULL);
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

			//if( hr == NET_FAILED )
			//{
				m_fClosed = true;
			//}
			break;
		}
	}
	m_pSocket->Close();
	//HV_Trace("\n***CCmdThread::End***\n");
	return hr;
}

template<class T>
CCmdListenThread<T>::CCmdListenThread()
:m_fClosed(false)
,m_pThread(NULL)
,m_pSocket(NULL)
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
	if( piCount == NULL )
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
	if (m_pThread != NULL)
	{
		m_fClosed = true;
		m_pThread->Join();
		delete m_pThread;
		m_pThread = NULL;
	}
	if (m_pSocket != NULL)
	{
		delete m_pSocket;
		m_pSocket = NULL;
	}
	return S_OK;
}

template<class T>
HRESULT CCmdListenThread<T>::Create(HvSys::ICmdProcess *pCmdProc, int iType, char* szIP, int iPort)
{
	HRESULT hr = S_OK;
	if (pCmdProc == NULL) return E_INVALIDARG;
	Close();
	m_pCmdProc = pCmdProc;
	if( szIP != NULL )
	{
		strcpy(m_szIp, szIP);
	}
	m_iPort = iPort;

	if( iType == 0 )
	{
		hr = HiVideo::ISocket::CreateInstance(&m_pSocket);
		if (FAILED(hr)) return hr;
		hr = m_pSocket->Listen(T::COMMAND_PORT, MAX_CONNECT_TEAM_COUNT);
		if (FAILED(hr)) return hr;
	}

	m_iType = iType;
	m_fClosed = false;
	return HiVideo::IThread::CreateInstance(
		&m_pThread,
		HiVideo::IThread::PRIORITY_NORMAL,
#if (RUN_PLATFORM == PLATFORM_DSP_BIOS)
		8192,
#else
		0,
#endif
		HiVideo::IThread::MEM_NORMAL,
		true,
		this);
}

template<class T>
HRESULT CCmdListenThread<T>::Run(void *pvParamter)
{
	HV_Sleep(2000);
	//HV_Trace("\n***CCmdListenThread::Run***\n");
	if (NULL == m_pCmdProc)
	{
		m_fClosed = true;
		return E_POINTER;
	}
	HRESULT hr = S_OK;
	HiVideo::ISocket *pClientSocket = NULL; 
	while (!m_fClosed)
	{
		m_dwLastThreadIsOkTime = GetSystemTick();

		if( m_iType == 0 )
		{
			if( m_pSocket == NULL ) 
			{
				m_fClosed = true;
				return E_POINTER;
			}
			pClientSocket = NULL;
			hr = m_pSocket->Accept(&pClientSocket, 3000);
			if (hr != S_OK) continue;
			int i;
			for (i = 0; i < MAX_CONNECT_TEAM_COUNT; i++)
			{
				if (!m_rgCmdThread[i].IsConnected())
				{
					m_rgCmdThread[i].Create(pClientSocket, m_pCmdProc);
					m_rgCmdThread[i].m_fIsAutoLink = false;
					pClientSocket = NULL;
					break;
				}
			}
			if (i == MAX_CONNECT_TEAM_COUNT && pClientSocket != NULL)
			{
				delete pClientSocket;
				pClientSocket = NULL;
			}
		}
		else
		{
			HV_Sleep(1000);
			m_fAutoIsConnected = false;
			if( m_cAutoCmdThread.IsConnected() )
			{
				m_fAutoIsConnected = true;
			}

			if( !m_fAutoIsConnected )
			{
				if( pClientSocket == NULL )
				{
					if( S_OK != HiVideo::ISocket::CreateInstance(&pClientSocket) ) continue;
				}
				if( S_OK != pClientSocket->Connect(m_szIp, m_iPort, 3000) ) 
				{
					continue;
				}
				pClientSocket->SetRevTimeOut(4000);

				m_cAutoCmdThread.Create(pClientSocket, m_pCmdProc);

				pClientSocket = NULL;
				m_fAutoIsConnected = true;
			}
		}
	}

	if( pClientSocket != NULL )
	{
		delete pClientSocket;
		pClientSocket = NULL;
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

#endif
