/**
* @file LinkManager.cpp 
* @brief 链接管理模块的实现
*
* 管理设备的链接，包括主动链接和被动链接\n
*
* @copyright Signalway All Rights Reserved
* @author zhouy
* @date 2013-04-11
* @version 1.0
*/

#include "SWFC.h"
#include "LinkManager.h"

#include "SWNetOpt.h"
#include "SWMessage.h"



#define CLINKMANAGERPRINT SW_TRACE_DEBUG


// 命令包头
typedef struct tag_CameraCmdHeader
{
    DWORD dwID;
    DWORD dwInfoSize;
}
CAMERA_CMD_HEADER;



// 响应包头
typedef struct tag_CameraResponseHeader
{
    DWORD dwID;
    DWORD dwInfoSize;
	INT	  iReturn;
}
RESPONSE_CMD_HEADER;

/*
CLinkManager* CLinkManager::GetInstance(const WORD wPassiveLinkPort, const WORD wActiveLinkPort,  const CHAR ** pArrayDstIp, const DWORD dwDstIpCount)
{
	//单实例模式
	static CLinkManager Instance;
	static BOOL	fInited = FALSE;
	if (!fInited)
	{
		if (FAILED(Instance.Initialize(wPassiveLinkPort, wActiveLinkPort, pArrayDstIp, dwDstIpCount)))
		{
			CLINKMANAGERPRINT("Err: failed to initialize CLinkManager \n");
			return NULL;
		}
		fInited = TRUE;
	}

	return &Instance;
}
*/


CLinkManager::CLinkManager()
{
	m_fInited = FALSE;
	m_dwActiveLinkState = STATE_READY;
	m_dwPassiveLinkState = STATE_READY;
	m_fNeedVerification = FALSE;
	m_fUserLogin = FALSE;
	m_wLocalPort = 0;
	m_wDstPort = 0;
	m_pszDstIp = NULL;
	m_fSafeSaver = 0;
	m_dwMaxLinkCount = 3;
	m_pPassiveLinkThread = NULL;
	m_pActiveLinkThread = NULL;
	//m_pfCallback = NULL;
	m_dwLinkCount = 0;
	m_pCallbackLock = NULL;
	
	m_lstLinks.RemoveAll();
	m_lstCallbacks.RemoveAll();

	m_strConnectInfo = "";
}


CLinkManager::~CLinkManager()
{
	if (!m_fInited)
	{
		return ;
	}

	SAFE_RELEASE(m_pPassiveLinkThread);

	SAFE_RELEASE(m_pActiveLinkThread);

	SAFE_MEM_FREE(m_pszDstIp);

	m_cMutex.Lock();
	while (!m_lstLinks.IsEmpty())
	{
		_LINK_INFO * pLink = m_lstLinks.GetHead();
		SAFE_RELEASE(pLink->pStream);
		SAFE_RELEASE(pLink->pPrevStream);
		m_lstLinks.RemoveHead();

		SAFE_MEM_FREE(pLink);
	}
	m_cMutex.Unlock();

	if (NULL != m_pCallbackLock)
	{		
		m_pCallbackLock->Unlock();

		SAFE_DELETE(m_pCallbackLock);
	}

	//m_pCallbackLock->Lock();
	while (!m_lstCallbacks.IsEmpty())
	{
		_CALLBACK_INFO * pCallback = m_lstCallbacks.GetHead();
		if (NULL != pCallback->pfCallback)
		{
			pCallback->pfCallback = NULL;
		}

		if (NULL != pCallback->pvCallbackArg)
		{
			pCallback->pvCallbackArg = NULL;
		}
		m_lstCallbacks.RemoveHead();

		SAFE_MEM_FREE(pCallback);
	}
	//m_pCallbackLock->Unlock();	

	CLINKMANAGERPRINT("Info: LinkManager (LocalPort = %d, DstPort=%d) Released!\n", m_wLocalPort,  m_wDstPort);
	
	m_fInited = FALSE;
}


CSWString CLinkManager::GetConnectInfo(VOID)
{
	return m_strConnectInfo;
}

HRESULT CLinkManager::Initialize(const WORD wLocalPort, const WORD wDstPort,  const CHAR * szDstIp, const BOOL fSafeSaverEnabled)
{
	if (m_fInited)
	{
		return S_OK;
	}

	
	m_wLocalPort = wLocalPort;
	
	m_wDstPort = wDstPort;
	m_fSafeSaver = fSafeSaverEnabled;

	CLINKMANAGERPRINT("Info: m_wLocalPort=%d, m_wDstPort=%d, szDstIp=%s\n", wLocalPort, wDstPort, szDstIp);

	if (NULL != szDstIp)
	{
		DWORD dwIpLen = swpa_strlen("xxx.xxx.xxx.xxx")+1;
		m_pszDstIp = (CHAR*)swpa_mem_alloc(dwIpLen);
		if (NULL == m_pszDstIp)
		{
			CLINKMANAGERPRINT("Err: no enough memory for m_pszDstIp\n");
			return E_OUTOFMEMORY;
		}
		swpa_memset(m_pszDstIp, 0, dwIpLen);

		swpa_strcpy(m_pszDstIp, szDstIp);
	}

	if (NULL == m_pCallbackLock)
	{
		m_pCallbackLock = new CSWMutex();
		if (NULL == m_pCallbackLock)
		{
			CLINKMANAGERPRINT("Err: no enough memory for m_pCallbackLock\n");
			SAFE_MEM_FREE(m_pszDstIp);
			return E_OUTOFMEMORY;
		}
	}

	m_pActiveLinkThread = new CSWThread();
	if (NULL == m_pActiveLinkThread)
	{
		CLINKMANAGERPRINT("Err: no enough memory for m_pActiveLinkThread\n");

		SAFE_MEM_FREE(m_pszDstIp);
		return E_OUTOFMEMORY;
	}

	m_pPassiveLinkThread = new CSWThread();
	if (NULL == m_pPassiveLinkThread)
	{
		CLINKMANAGERPRINT("Err: no enough memory for m_pPassiveLinkThread\n");

		SAFE_MEM_FREE(m_pszDstIp);
		return E_OUTOFMEMORY;
	}


	m_fInited = TRUE;
	return S_OK;
}
 


HRESULT CLinkManager::StartActiveLink(VOID)
{
	if (!m_fInited)
	{
		CLINKMANAGERPRINT("Err: CLinkManager is not inited yet\n");
		return E_NOTIMPL;
	}

	if (NULL == m_pszDstIp)
	{
		CLINKMANAGERPRINT("Info: NULL == m_pszDstIp , won't start ActiveLink Service!\n");
		return S_OK;
	}

	if (NULL == m_pActiveLinkThread)
	{
		CLINKMANAGERPRINT("Err: NULL == m_pActiveLinkThread\n");
		return E_INVALIDARG;
	}
	

	m_dwActiveLinkState = STATE_RUNNING;

	if (FAILED(m_pActiveLinkThread->Start(OnActiveLinkProxy, (PVOID)this)))
	{
		CLINKMANAGERPRINT("Err: failed to start m_pActiveLinkThread\n");
		return E_FAIL;
	}

	return S_OK;
}


HRESULT CLinkManager::StopActiveLink(VOID)
{
	if (!m_fInited)
	{
		CLINKMANAGERPRINT("Info: CLinkManager is not inited yet\n");
		return S_OK;
	}

	if (NULL == m_pActiveLinkThread)
	{
		CLINKMANAGERPRINT("Err: NULL == m_pActiveLinkThread\n");
		return E_NOTIMPL;
	}

	m_dwActiveLinkState = STATE_STOPPED;
	
	if (FAILED(m_pActiveLinkThread->Stop()))
	{
		CLINKMANAGERPRINT("Err: failed to stop m_pActiveLinkThread\n");
		return E_FAIL;
	}

	return S_OK;
}




HRESULT CLinkManager::StartPassiveLink(VOID)
{
	if (!m_fInited)
	{
		CLINKMANAGERPRINT("Err: CLinkManager is not inited yet\n");
		return E_NOTIMPL;
	}

	if (NULL == m_pPassiveLinkThread)
	{
		CLINKMANAGERPRINT("Err: NULL == m_pPassiveLinkThread\n");
		return E_NOTIMPL;
	}

	m_dwPassiveLinkState = STATE_RUNNING;

	if (FAILED(m_pPassiveLinkThread->Start(OnPassiveLinkProxy, (PVOID)this)))
	{
		CLINKMANAGERPRINT("Err: failed to start m_pPassiveLinkThread\n");
		return E_FAIL;
	}

	return S_OK;
}


HRESULT CLinkManager::StopPassiveLink(VOID)
{
	if (!m_fInited)
	{
		CLINKMANAGERPRINT("Info: CLinkManager is not inited yet\n");
		return S_OK;
	}

	if (NULL == m_pPassiveLinkThread)
	{
		CLINKMANAGERPRINT("Err: NULL == m_pPassiveLinkThread\n");
		return E_NOTIMPL;
	}

	m_dwPassiveLinkState = STATE_STOPPED;
	
	if (FAILED(m_pPassiveLinkThread->Stop()))
	{
		CLINKMANAGERPRINT("Err: failed to stop m_pPassiveLinkThread\n");
		return E_FAIL;
	}

	return S_OK;
}




HRESULT CLinkManager::SetMaxLinkCount(const DWORD dwMaxCount)
{
	if (!m_fInited)
	{
		CLINKMANAGERPRINT("Err: CLinkManager is not inited yet\n");
		return E_NOTIMPL;
	}

	if (m_dwLinkCount > dwMaxCount)
	{
		CLINKMANAGERPRINT("Err: m_dwLinkCount(%u) > dwMaxCount(%u)\n", m_dwLinkCount, dwMaxCount);
		return E_INVALIDARG;
	}

	m_dwMaxLinkCount = dwMaxCount;

	return S_OK;	
}


DWORD CLinkManager::GetMaxLinkCount(VOID)
{
	if (!m_fInited)
	{
		CLINKMANAGERPRINT("Err: CLinkManager is not inited yet\n");
		//return E_NOTIMPL;
		return 0;
	}

	return m_dwMaxLinkCount;	
}



DWORD CLinkManager::GetLinkCount(VOID)
{
	if (!m_fInited)
	{
		CLINKMANAGERPRINT("Err: CLinkManager is not inited yet\n");
		//return E_NOTIMPL;
		return 0;
	}

	return m_dwLinkCount;	
}




HRESULT CLinkManager::EnableUserLogin(VOID)
{
	if (!m_fInited)
	{
		CLINKMANAGERPRINT("Err: CLinkManager is not inited yet\n");
		return E_NOTIMPL;
	}

	if (!m_fUserLogin)
	{
		m_fUserLogin = TRUE;
	}

	

	return S_OK;
}



HRESULT CLinkManager::DisableUserLogin(VOID)
{
	if (!m_fInited)
	{
		CLINKMANAGERPRINT("Err: CLinkManager is not inited yet\n");
		return E_NOTIMPL;
	}

	if (m_fUserLogin)
	{
		m_fUserLogin = FALSE;
	}

	return S_OK;
}



HRESULT CLinkManager::EnableIdentityVerification(VOID)
{
	if (!m_fInited)
	{
		CLINKMANAGERPRINT("Err: CLinkManager is not inited yet\n");
		return E_NOTIMPL;
	}

	if (!m_fNeedVerification)
	{
		m_fNeedVerification = TRUE;
	}
	
	return S_OK;
}



HRESULT CLinkManager::DisableIdentityVerification(VOID)
{
	if (!m_fInited)
	{
		CLINKMANAGERPRINT("Err: CLinkManager is not inited yet\n");
		return E_NOTIMPL;
	}

	if (m_fNeedVerification)
	{
		m_fNeedVerification = FALSE;
	}

	return S_OK;

}




HRESULT CLinkManager::ReleaseLink(CSWStream * pStream)
{
	if (!m_fInited)
	{
		CLINKMANAGERPRINT("Err: CLinkManager is not inited yet\n");
		return E_NOTIMPL;
	}

	if (NULL == pStream)
	{
		CLINKMANAGERPRINT("Err: NULL == pStream\n");
		return E_INVALIDARG;
	}

	CLINKMANAGERPRINT("Info: going to release stream (%p)\n", pStream);

	m_cMutex.Lock();
	SW_POSITION Pos = m_lstLinks.GetHeadPosition();
	while (m_lstLinks.IsValid(Pos))
	{
		SW_POSITION PrevPos = Pos;
		_LINK_INFO* psLink = m_lstLinks.GetNext(Pos);
		if (pStream == psLink->pStream)
		{			
			//CLINKMANAGERPRINT("Info: before pStream (%p) Released\n", pStream);			
			SAFE_RELEASE(pStream);//pStream->Release();	
			SAFE_RELEASE(psLink->pPrevStream);
			CLINKMANAGERPRINT("Info: after pStream (%p) Released\n", psLink->pStream);

			m_lstLinks.RemoveAt(PrevPos);

			SAFE_MEM_FREE(psLink);
			
			m_dwLinkCount--;
			CLINKMANAGERPRINT("Info: m_dwLinkCount = %d\n", m_dwLinkCount);

			if (CAMERA_IMAGE_LINK_PORT == m_wLocalPort
				&& 0 == m_dwLinkCount)
			{
				CSWMessage::SendMessage(MSG_RECOGNIZE_GETJPEG, (WPARAM)0);
			}
			
			break;
		}
	}
	m_cMutex.Unlock();
	
	CLINKMANAGERPRINT("Info: Released!\n");
	return S_OK;
}



HRESULT CLinkManager::SetLinkCallback(PF_LINK_CALLBACK pfCallback, PVOID pvArg)
{
	if (!m_fInited)
	{
		CLINKMANAGERPRINT("Err: CLinkManager is not inited yet\n");
		return E_NOTIMPL;
	}

	if (NULL == pfCallback)
	{
		CLINKMANAGERPRINT("Err: NULL == pfCallback\n");
		return E_INVALIDARG;
	}

	_CALLBACK_INFO * pCallbackInfo = (_CALLBACK_INFO*)swpa_mem_alloc(sizeof(_CALLBACK_INFO));
	if (NULL == pCallbackInfo)
	{
		CLINKMANAGERPRINT("Err: no memory for pCallbackInfo\n");
		return E_OUTOFMEMORY;
	}

	pCallbackInfo->pfCallback = pfCallback;
	pCallbackInfo->pvCallbackArg = pvArg;

	m_pCallbackLock->Lock();
	m_lstCallbacks.AddTail(pCallbackInfo);
	m_pCallbackLock->Unlock();

	return S_OK;
}


HRESULT CLinkManager::SendHandshakeMsg()
{
	if (!m_fInited)
	{
		CLINKMANAGERPRINT("Err: CLinkManager is not inited yet\n");
		return E_NOTIMPL;
	}
	return S_OK;
}



BOOL CLinkManager::IsConnected(const CHAR* szIp, const WORD wPort)
{
	if (!m_fInited)
	{
		CLINKMANAGERPRINT("Err: CLinkManager is not inited yet\n");
		return E_NOTIMPL;
	}
	
	if (NULL == szIp || 0 == swpa_strcmp(szIp, ""))
	{
		CLINKMANAGERPRINT("Err:NULL == szIp || 0 == swpa_strcmp(szIp, "")\n");
		return E_INVALIDARG;
	}

	m_cMutex.Lock();
	SW_POSITION Pos = m_lstLinks.GetHeadPosition();
	while (m_lstLinks.IsValid(Pos))
	{
		_LINK_INFO* psLink = m_lstLinks.GetNext(Pos);
		if (wPort == psLink->wPort && 0 == swpa_strcmp(szIp, psLink->szIp))
		{
			m_cMutex.Unlock();
			return TRUE;
		}
	}
	m_cMutex.Unlock();
	
	return FALSE;
}



HRESULT CLinkManager::GetDefaultHandshakeXml(PBYTE* ppbBuf, DWORD* pdwLen)
{
	CHAR* szXml = NULL;
	if (CAMERA_IMAGE_LINK_PORT == m_wLocalPort)
	{
		szXml = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\" ?><HvCmd ver=\"3.0\"><CmdName Enable=\"0\" >DownloadImage</CmdName></HvCmd>\n";
	}
	else if (CAMERA_VIDEO_LINK_PORT == m_wLocalPort)
	{
		szXml = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\" ?><HvCmd ver=\"3.0\"><CmdName Enable=\"0\" >DownloadVideo</CmdName></HvCmd>\n";
	}
	else if (CAMERA_RECORD_LINK_PORT == m_wLocalPort)
	{
		szXml = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\" ?><HvCmd ver=\"3.0\"><CmdName Enable=\"0\" >DownloadRecord</CmdName></HvCmd>\n";
	}
	else
	{
		//default:
		szXml = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\" ?><HvCmd ver=\"3.0\"><CmdName Enable=\"0\" >DownloadRecord</CmdName></HvCmd>\n";
	}

	if (NULL == ppbBuf || 0 == pdwLen)
	{
		CLINKMANAGERPRINT("Err: NULL == pbBuf || 0 == pdwLen\n");
		return E_INVALIDARG;
	}

	
	CAMERA_CMD_HEADER sHeader;

	//*pdwLen = swpa_strlen(szXml)+1 ;
	
	sHeader.dwID = CAMERA_XML_EXT_CMD; //xml
	sHeader.dwInfoSize = swpa_strlen(szXml)+1;

	*pdwLen = sHeader.dwInfoSize + sizeof(sHeader);
	
	*ppbBuf = (PBYTE)swpa_mem_alloc(*pdwLen);
	if (NULL == *ppbBuf)
	{
		CLINKMANAGERPRINT("Err: no memory for *ppbBuf\n");
		return E_OUTOFMEMORY;
	}
	swpa_memset(*ppbBuf, 0, *pdwLen);
	
	swpa_memcpy(*ppbBuf, &sHeader, sizeof(sHeader));
	swpa_strcpy((CHAR*)*ppbBuf+sizeof(sHeader), szXml);//, *pdwLen-sizeof(sHeader));


	return S_OK;
}


HRESULT CLinkManager::PushLink(CSWTCPSocket* pTCPSocket)
{
	HRESULT hr = S_OK;

	if (NULL == pTCPSocket)
	{
		CLINKMANAGERPRINT("Err: NULL == pTCPSocket\n");

		return E_INVALIDARG;
	}

	
	CAMERA_CMD_HEADER sHeader;
	DWORD dwLen = 0;
	PBYTE pbBuf = NULL;
	SW_POSITION Pos = NULL;

	CSWTcpStream * pStream = new CSWTcpStream(pTCPSocket); 
	if (NULL == pStream)
	{
		CLINKMANAGERPRINT("Err: no enough memory for pStream\n");		
		return E_OUTOFMEMORY;
	}

	BOOL fRealTimeRecord = FALSE; // receive realtime records ? 
	if (CAMERA_RECORD_LINK_PORT == m_wLocalPort)
	{
		fRealTimeRecord = TRUE; //default is TRUE
	}
	
	//todo: do verification if needed

	swpa_memset(&sHeader, 0, sizeof(sHeader));

	if (FAILED(pStream->Read(&sHeader, sizeof(sHeader), NULL)))
	{
		CHAR szIp[32] = {0};
		WORD wPort = 0;
		pTCPSocket->GetPeerName(szIp, &wPort);

		if (0 == swpa_strlen(szIp) || 0 == wPort)
		{
			SW_TRACE_DEBUG("Err: connection is down. discards it.\n");
			SAFE_RELEASE(pStream);
			return E_FAIL;
		}
		
		SW_TRACE_DEBUG("Info: failed to read pStream, use default handshake data\n");

		GetDefaultHandshakeXml(&pbBuf, &dwLen);

		SW_TRACE_DEBUG("Info: default xml is \n%s\n", (CHAR*)(pbBuf+8));
	}
	else
	{		
		dwLen = sHeader.dwInfoSize;
		if (0 >= dwLen)
		{
			CLINKMANAGERPRINT("Err: 0 == dwInfoSize\n");
			
			SAFE_RELEASE(pStream);
			return E_FAIL;
		}

		pbBuf = (PBYTE)swpa_mem_alloc(dwLen+sizeof(sHeader));
		if (NULL == pbBuf)
		{
			CLINKMANAGERPRINT("Err: no enough memory for pbBuf\n");
			
			SAFE_RELEASE(pStream);
			return E_OUTOFMEMORY;
		}
		swpa_memcpy(pbBuf, &sHeader, sizeof(sHeader));

		if (FAILED(pStream->Read(pbBuf+sizeof(sHeader), dwLen, NULL)))
		{
			CLINKMANAGERPRINT("Err: failed to read pStream\n");

			SAFE_RELEASE(pStream);			
			SAFE_MEM_FREE(pbBuf);
			
			return E_FAIL;
		}

		CLINKMANAGERPRINT("Info: received xml is \n%s\n", (CHAR*)(pbBuf+sizeof(sHeader)));

		if (NULL != swpa_strstr((CHAR*)pbBuf+sizeof(sHeader), "Enable=\"1\""))
		{
			fRealTimeRecord = FALSE; 
		}

		RESPONSE_CMD_HEADER sResponse;
		sResponse.dwID = CAMERA_XML_EXT_CMD;
		sResponse.dwInfoSize = 0;
		sResponse.iReturn = S_OK;

		CHAR szIp[32] = {0};
		WORD wPort = 0;
		pTCPSocket->GetPeerName(szIp, &wPort);
		
		INT iTryCount = 8;
		while (--iTryCount)
		{
			if (FAILED(pStream->Write((PVOID)&sResponse, sizeof(sResponse), NULL)))
			{
				CLINKMANAGERPRINT("Warning: failed to write response data to %s:%d, try another %d time(s)\n", szIp, wPort, iTryCount);
				swpa_thread_sleep_ms(200);
			}
			else
			{
				CLINKMANAGERPRINT("Info: write response data to %s:%d -- OK\n", szIp, wPort);
				break;
			}
		}

		if (0 > iTryCount)
		{
			CLINKMANAGERPRINT("Err: failed to write response data to %s:%d\n", szIp, wPort);
			SAFE_MEM_FREE(pbBuf);
			SAFE_RELEASE(pStream);

			return E_FAIL;
		}
	}

	
	BOOL fBrokenConnectionFound = FALSE;

	if (fRealTimeRecord)
	{
		CHAR szPeerIp[32] = {0};
		WORD wPeerPort = 0;
		pTCPSocket->GetPeerName(szPeerIp, &wPeerPort);

		
		m_cMutex.Lock();
		SW_POSITION Pos = m_lstLinks.GetHeadPosition();
		_LINK_INFO* psLink = NULL;
		while (m_lstLinks.IsValid(Pos))
		{
			psLink = m_lstLinks.GetNext(Pos);

			if (NULL != psLink 
				&& psLink->fIsRealTimeLink
				&& 0 == swpa_strcmp(psLink->szIp, szPeerIp)
				)
			{
				CSWTCPSocket * pSocket = NULL;
				if (SUCCEEDED(psLink->pStream->GetMedium((CSWObject**)&pSocket)))
				{
					CHAR szStreamPeerIp[32] = {0};
					WORD wStreamPeerPort = 0;

					pSocket->GetPeerName(szStreamPeerIp, &wStreamPeerPort);
					
					SW_TRACE_DEBUG("Info: szStreamPeerIp = %s, wStreamPeerPort = %d\n",
						szStreamPeerIp, wStreamPeerPort);
					
					if (!psLink->fIsActiveLink || 
						0 != swpa_strcmp(psLink->szIp, szStreamPeerIp))
					{
						fBrokenConnectionFound = TRUE;
						
						SAFE_RELEASE(psLink->pPrevStream);
						psLink->pPrevStream = psLink->pStream;

						SAFE_ADDREF(pStream);
						psLink->pStream = pStream;
						swpa_strcpy(psLink->szIp, szPeerIp);
						psLink->wPort = wPeerPort;
						psLink->fIsActiveLink = TRUE;

						SW_TRACE_DEBUG("Info: stream updated\n");
						
						break;
					}
				}
			}
		}
		
		m_cMutex.Unlock();
	}

	if (!fBrokenConnectionFound)
	{
		m_pCallbackLock->Lock();
		Pos = m_lstCallbacks.GetHeadPosition();
		while (m_lstCallbacks.IsValid(Pos))
		{
			_CALLBACK_INFO * pCallback = m_lstCallbacks.GetNext(Pos);
			if (NULL != pCallback)
			{
				hr = pCallback->pfCallback(pStream, pCallback->pvCallbackArg, (PVOID)pbBuf);
				if (FAILED(hr))
				{
					CLINKMANAGERPRINT("Err: failed to pushlink %p\n", pStream);				
					hr = E_FAIL;
					break;
				}
				else
				{				
					_LINK_INFO* psLink = (_LINK_INFO*)swpa_mem_alloc(sizeof(_LINK_INFO));
					if (NULL == psLink)
					{
						CLINKMANAGERPRINT("Err: no enough memory for psLink\n");					
						hr =  E_OUTOFMEMORY;
						break;
					}
					
					swpa_memset(psLink, 0, sizeof(*psLink));
					SAFE_ADDREF(pStream);
					psLink->pStream = pStream;
					psLink->fIsActiveLink = TRUE;			
					pTCPSocket->GetPeerName(psLink->szIp, &psLink->wPort);
					psLink->pPrevStream = NULL;
					psLink->fIsRealTimeLink = fRealTimeRecord;
						
					CLINKMANAGERPRINT("Info: get peer name: %s:%d\n", psLink->szIp, psLink->wPort);
					
					m_cMutex.Lock();
					m_lstLinks.AddTail(psLink);
					m_cMutex.Unlock();

					m_dwLinkCount++;
					
					if (CAMERA_IMAGE_LINK_PORT == m_wLocalPort
						&& 1 == m_dwLinkCount)
					{
						CSWMessage::SendMessage(MSG_RECOGNIZE_GETJPEG, (WPARAM)1);
					}
					
					CLINKMANAGERPRINT("Info: m_dwLinkCount=%d, port = %d\n", m_dwLinkCount, m_wLocalPort);

					hr = S_OK;
					break;
				}
			}
		}		
		m_pCallbackLock->Unlock();
	}
	
	SAFE_RELEASE(pStream);	

	SAFE_MEM_FREE(pbBuf);

	return hr;
}


HRESULT CLinkManager::ActiveLinkHandshake(CSWTCPSocket * pSockClient)
{
	if (NULL == pSockClient)
	{
		CLINKMANAGERPRINT("Err: NULL == pSockClient\n");
		return E_INVALIDARG;
	}
	
	CHAR szSN[256] = {0};
	DWORD dwLen = 256;
	
	INT iRet = swpa_device_read_sn(szSN, &dwLen);
	if (SWPAR_OK != iRet)
	{
		CLINKMANAGERPRINT("Err: failed to get device serial number\n");
		return E_FAIL;
	}
		
	CHAR szXml[512] = {0};
	swpa_snprintf(szXml, sizeof(szXml)-1, 
		"<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\" ?>"
		"<HvCmd ver=\"1.0\">"
			"<CmdName>ConnectionRequest</CmdName>"
			"<CmdArg>"
				"<Value id=\"SN\">%s</Value>"
				"<Value id=\"SafeSaverEnable\">%d</Value>"
				"<Value id=\"ProtocolVersion\">%d</Value>"
			"</CmdArg>"
		"</HvCmd>\n",
		szSN, m_fSafeSaver, PROTOCOL_MERCURY);

	CAMERA_CMD_HEADER sHeader;

	sHeader.dwID = CAMERA_XML_EXT_CMD;
	sHeader.dwInfoSize = swpa_strlen(szXml)+1;

	if (FAILED(pSockClient->Send((VOID*)&sHeader, sizeof(sHeader), NULL)))
	{
		CLINKMANAGERPRINT("Err: failed to send cmd header\n");
		return E_FAIL;
	}

	if (FAILED(pSockClient->Send((VOID*)szXml, sHeader.dwInfoSize, NULL)))
	{
		CLINKMANAGERPRINT("Err: failed to send cmd xml\n");
		return E_FAIL;
	}


	RESPONSE_CMD_HEADER sResponse;		
	if (FAILED(pSockClient->Read((VOID*)&sResponse, sizeof(sResponse), NULL)))
	{
		CLINKMANAGERPRINT("Err: failed to read cmd response\n");
		return E_FAIL;
	}

	if (CAMERA_XML_EXT_CMD != sResponse.dwID
		|| 0 != sResponse.dwInfoSize
		|| S_OK != sResponse.iReturn)
	{
		CLINKMANAGERPRINT("Err: failed to handshake\n");
		return E_FAIL;
	}

	return S_OK;
}




HRESULT CLinkManager::OnActiveLink()
{
	HRESULT hr = S_OK;
	
	if (!m_fInited)
	{
		CLINKMANAGERPRINT("Err: CLinkManager is not inited yet\n");
		return E_NOTIMPL;
	}

	if (NULL == m_pszDstIp)
	{
		CLINKMANAGERPRINT("Err: NULL == m_pszDstIp\n");
		return E_INVALIDARG;
	}

	BOOL fBinded = FALSE;
	DWORD dwLastUpdateTimeMs = CSWDateTime::GetSystemTick();

	CSWTCPSocket * pSockClient = NULL;
	
	while (STATE_RUNNING == m_dwActiveLinkState)
	{
		if( CSWDateTime::GetSystemTick() - dwLastUpdateTimeMs > 5 * 1000 )
		{
			CSWString strInfo;
			CSWString strConnect;
			strInfo.Format("%d/%d_", m_dwLinkCount, GetMaxLinkCount());
			strConnect.Append(strInfo);
			m_cMutex.Lock();
			SW_POSITION pos = m_lstLinks.GetHeadPosition();
			while(m_lstLinks.IsValid(pos))
			{
				_LINK_INFO* plink = m_lstLinks.GetNext(pos);
				if( plink != NULL )
				{
					strInfo.Format("%s,%d_", plink->szIp, (INT)plink->wPort);
					strConnect.Append(strInfo);
				}
			}
			m_cMutex.Unlock();

			m_strConnectInfo = strConnect;

			dwLastUpdateTimeMs = CSWDateTime::GetSystemTick();
		}
		
		if ( m_dwLinkCount >= GetMaxLinkCount())
		{
			CLINKMANAGERPRINT("Info: m_dwLinkCount >= m_dwMaxLinkCount\n");
			CSWApplication::Sleep(2000);
			continue;
		}

		if (IsConnected(m_pszDstIp, m_wDstPort))
		{
			CSWApplication::Sleep(2000);
			continue;
		}
		
		while (NULL == pSockClient)
		{
			pSockClient = new CSWTCPSocket();

			if (NULL != pSockClient)
			{
				while (FAILED(pSockClient->Create()))
				{
					CLINKMANAGERPRINT("Err: failed to create pSockClient, trying again...\n");
					CSWApplication::Sleep(200);
				}

				pSockClient->SetSendTimeout(4000);
				pSockClient->SetRecvTimeout(4000);
			}
			else
			{
				CSWApplication::Sleep(200);
			}
		} 
		

		CLINKMANAGERPRINT("Info: ActiveLink connecting to %s:%d...\n", m_pszDstIp, m_wDstPort);
		if (SUCCEEDED(pSockClient->Connect(m_pszDstIp, m_wDstPort)))
		{
			CLINKMANAGERPRINT("Info: ActiveLink connected to %s:%d\n", m_pszDstIp, m_wDstPort);
			if (FAILED(ActiveLinkHandshake(pSockClient)))
			{
				CLINKMANAGERPRINT("Err: failed to handshake\n");
			}
			else 
			{
				if (FAILED(PushLink(pSockClient)))
				{
					CLINKMANAGERPRINT("Err: failed to push the pSockClient\n");
				}
			}
			SAFE_RELEASE(pSockClient);
		}
		else
		{
			CLINKMANAGERPRINT("Info: FAILED to connect to %s:%d, trying again...\n", m_pszDstIp, m_wDstPort);
			CSWApplication::Sleep(6000);
		}
	}
	

	m_dwActiveLinkState = STATE_STOPPED;

	CLINKMANAGERPRINT("Info: ActiveLink Service Stopped!\n");
	
	return hr;
	
}



VOID* CLinkManager::OnActiveLinkProxy(VOID * pvArg)
{
	CLinkManager * pThis = (CLinkManager*)pvArg;

	return (VOID*)pThis->OnActiveLink();
}





HRESULT CLinkManager::OnPassiveLink()
{
	HRESULT hr = S_OK;
	
	if (!m_fInited)
	{
		CLINKMANAGERPRINT("Err: CLinkManager is not inited yet\n");
		return E_NOTIMPL;
	}

	CSWTCPSocket * pSockServer = NULL;
	pSockServer = new CSWTCPSocket();
	if (NULL == pSockServer)
	{
		CLINKMANAGERPRINT("Err: no enough memory for pSockServer\n");
		return E_OUTOFMEMORY;
	}


	BOOL fBinded = FALSE;

	DWORD dwLastUpdateTimeMs = CSWDateTime::GetSystemTick();

	while (STATE_RUNNING == m_dwPassiveLinkState)
	{		
		if( CSWDateTime::GetSystemTick() - dwLastUpdateTimeMs > 5 * 1000 )
		{
			CSWString strInfo;
			CSWString strConnect;
			strInfo.Format("%d/%d_", m_dwLinkCount, GetMaxLinkCount());
			strConnect.Append(strInfo);
			m_cMutex.Lock();
			SW_POSITION pos = m_lstLinks.GetHeadPosition();
			while(m_lstLinks.IsValid(pos))
			{
				_LINK_INFO* plink = m_lstLinks.GetNext(pos);
				if( plink != NULL )
				{
					strInfo.Format("%s,%d_", plink->szIp, (INT)plink->wPort);
					strConnect.Append(strInfo);
				}
			}
			m_cMutex.Unlock();

			m_strConnectInfo = strConnect;

			dwLastUpdateTimeMs = CSWDateTime::GetSystemTick();
		}

		if ( m_dwLinkCount >= GetMaxLinkCount())
		{
			CLINKMANAGERPRINT("Info: m_dwLinkCount >= m_dwMaxLinkCount (Port %d)\n", m_wLocalPort);
			if( fBinded )
			{
				pSockServer->Close();
				fBinded = FALSE;
			}
			CSWApplication::Sleep(2000);
			continue;
		}

		if (!fBinded)
		{
			if (FAILED(pSockServer->Create()))
			{
				CLINKMANAGERPRINT("Err: failed to create pSockServer\n");
				CSWApplication::Sleep(1000);
				continue;
			}
			
			while (FAILED(pSockServer->Bind(NULL, m_wLocalPort)))
			{
				CLINKMANAGERPRINT("Err: failed to Bind pSockServer to Port #%d\n", m_wLocalPort);

				CSWApplication::Sleep(1000);
			}

			fBinded = TRUE;
			
			pSockServer->Listen();			
			pSockServer->SetRecvTimeout(4000);
			pSockServer->SetSendTimeout(4000);
		}

		SWPA_SOCKET_T  sInSock;		
		if (SUCCEEDED(pSockServer->Accept(sInSock)))
		{
			CSWTCPSocket * pDataSock = new CSWTCPSocket();
			if (NULL == pDataSock)
			{
				CLINKMANAGERPRINT("Err: no enough memory for pDataSock\n");
				hr = E_OUTOFMEMORY;
				break;
			}			
			pDataSock->Attach(sInSock);
			pDataSock->SetRecvTimeout(4000);
			pDataSock->SetSendTimeout(4000);

			// 把发送缓冲区设小。
			//pDataSock->SetSendBufferSize(16);

			CLINKMANAGERPRINT("Info: got a connection on Port #%d\n", m_wLocalPort);
			if (FAILED(PushLink(pDataSock)))
			{
				CLINKMANAGERPRINT("Err: failed to push the pSockClient\n");
			}
		
			pDataSock->Release();
		}
	}


	SAFE_RELEASE(pSockServer);

	m_dwPassiveLinkState = STATE_STOPPED;

	CLINKMANAGERPRINT("Info: PassiveLink (%d) Service Stopped!\n", m_wLocalPort);
		
	return hr;
	
}




VOID* CLinkManager::OnPassiveLinkProxy(VOID * pvArg)
{
	CLinkManager * pThis = (CLinkManager*)pvArg;

	return (VOID*)pThis->OnPassiveLink();
}


HRESULT CLinkManager::UpdateStream(CSWStream* pOldStream, CSWStream** ppNewStream)
{
	if (NULL == pOldStream || NULL == ppNewStream)
	{
		SW_TRACE_DEBUG("%p, %p\n", pOldStream, ppNewStream);
		return E_INVALIDARG;
	}

	m_cMutex.Lock();
	SW_POSITION Pos = m_lstLinks.GetHeadPosition();
	_LINK_INFO* psLink = NULL;
	while (m_lstLinks.IsValid(Pos))
	{
		psLink = m_lstLinks.GetNext(Pos);

		if (NULL != psLink && pOldStream == psLink->pStream)
		{
			psLink->fIsActiveLink = FALSE;
			m_cMutex.Unlock();
			return E_ACCESSDENIED;
		}
		if (NULL != psLink && pOldStream == psLink->pPrevStream)
		{
			*ppNewStream = psLink->pStream;
			m_cMutex.Unlock();
			return S_OK;
		}
	}
	
	m_cMutex.Unlock();

	return E_FAIL;
}


