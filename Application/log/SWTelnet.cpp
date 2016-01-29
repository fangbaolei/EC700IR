#include "SWFC.h"
#include "SWTelnet.h"

class CSWTelnetClient : public CSWThread
{
	CLASSINFO(CSWTelnetClient, CSWThread)
public:
	CSWTelnetClient(INT iLevel, const SWPA_SOCKET_T & sock)
	{
		m_iLevel = iLevel;
		m_tcpClient.Attach(sock);
	}
	virtual ~CSWTelnetClient()
	{
	}
	INT GetLevel(VOID)
	{
		return m_iLevel;
	}
	CSWTCPSocket & GetTCP(VOID)
	{
		return m_tcpClient;
	}
	virtual HRESULT Run()
	{
		while(S_OK == IsValid() && m_tcpClient.IsConnect())
		{
			CSWApplication::Sleep(1000);
		}
		return S_OK;
	} 
private:
	CSWTCPSocket m_tcpClient;
	INT m_iLevel;
};

CSWTelnet::CSWTelnet()
{
	m_fInitialize = FALSE;
	m_iHandle = swpa_list_create();
	m_fLogging = FALSE;
}

CSWTelnet::~CSWTelnet()
{
	Clear();
}

HRESULT CSWTelnet::Create(const INT nPort)
{
	if(FAILED(m_tcpTelnet.Create())
	|| FAILED(m_tcpTelnet.Bind(NULL, nPort))
	|| FAILED(m_tcpTelnet.Listen()))
	{
		printf("create telnet server %d error.\n", nPort);
		return E_FAIL;
	}
	m_fInitialize = TRUE;
	return Start();
}

VOID CSWTelnet::Clear(VOID)
{
	m_tcpTelnet.Close();	
	Stop();
	if(-1 != m_iHandle)
	{
		for(INT pos = swpa_list_open(m_iHandle); -1 != pos; pos = swpa_list_next(m_iHandle))
		{
			((CSWTCPSocket *)swpa_list_value(m_iHandle, pos))->Release();
		}
		swpa_list_close(m_iHandle);
		swpa_list_destroy(m_iHandle);
		m_iHandle = -1;
	}
	m_fLogging = FALSE;
}

HRESULT CSWTelnet::IsValid()
{
	return m_fInitialize ? CSWThread::IsValid() : E_FAIL;
}

HRESULT CSWTelnet::Run()
{
	while(S_OK == IsValid())
	{
		SWPA_SOCKET_T sock;
		if(S_OK == m_tcpTelnet.Accept(sock))
		{
			swpa_list_add(m_iHandle, new CSWTelnetClient(5, sock));
		}
	}
	return S_OK;
}

HRESULT CSWTelnet::Log(INT iLevel, LPCSTR szLog)
{
	if(-1 != m_iHandle)
	{
		CSWString strLog = szLog;
		strLog.Replace("\n", "\r\n");
		DWORD dwLiveCount = 0;
		for(INT pos = swpa_list_open(m_iHandle); -1 != pos; pos = swpa_list_next(m_iHandle))
		{
			CSWTelnetClient *pClient = (CSWTelnetClient *)swpa_list_value(m_iHandle, pos);
			if(pClient->GetLevel() >= iLevel)
			{				
				if(FAILED(pClient->GetTCP().Send((PVOID)(LPCSTR)strLog, strLog.Length() + 1)))
				{
					printf("Telnet socket close\n");
					swpa_list_delete(m_iHandle, pos);
					pClient->Release();
				}
				else
				{
					dwLiveCount++;
				}
			}
		}
		swpa_list_close(m_iHandle);

		m_fLogging = dwLiveCount > 0;
	}
	return S_OK;
}

BOOL CSWTelnet::IsLogging()
{
	return m_fLogging ? TRUE : FALSE;
}