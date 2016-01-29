/**
* @file SWNetRenderFilter.cpp
* @brief Implementation of CSWNetRenderFilter
* 
* @copyright Signalway All Rights Reserved
* @author zhouy
* @date 2013-04-08
* @version 1.0
*/

#include "SWFC.h"
#include "LinkManager.h"
#include "SWNetRenderFilter.h"
#include "SWCarLeft.h"
#include "SWRecord.h"
#include "SWTransmittingOpt.h"
#include "SWNetOpt.h"


#include "SWMessage.h"

#define PRINT SW_TRACE_DEBUG


// 命令包头
typedef struct tag_CameraCmdHeader
{
    DWORD dwID;
    DWORD dwInfoSize;
}
CAMERA_CMD_HEADER;



CSWNetRenderPin::CSWNetRenderPin(CSWBaseFilter* pFilter, DWORD dwDir, DWORD dwID)
	:CSWBasePin(pFilter, dwDir)
	,m_fIdle(TRUE)
	,m_dwTransmittingType(TRANSMITTING_TYPE_COUNT)
	,m_dwID(dwID)
{
	m_fInited = TRUE;
}


CSWNetRenderPin::~CSWNetRenderPin()
{
	m_fInited = FALSE;
}


HRESULT CSWNetRenderPin::Receive(CSWObject* pObj)
{
	if (!m_fInited)
	{		
		PRINT("Err: m_fInited = %d\n", m_fInited);
		return E_NOTIMPL;
	}

	//PRINT("<Net Pin> Info: received a obj = %s\n", pObj->Name());
	
	if (IsDecendant(CSWImage, pObj))
	{
		if (IsDecendant(CSWNetRenderFilter, GetFilter()))
		{	
			//PRINT("<Net Pin> Info: received an Image obj\n");
			CSWNetRenderFilter* pNetRenderFilter = (CSWNetRenderFilter*)GetFilter();
			return pNetRenderFilter->Receive(pObj, m_dwID, m_dwTransmittingType);
		}
	}
	else if (IsDecendant(CSWRecord, pObj))
	{
		//PRINT("<Net Pin> Info: received a Record obj\n");
		if (IsDecendant(CSWNetRenderFilter, GetFilter()))
		{	
			CSWNetRenderFilter* pNetRenderFilter = (CSWNetRenderFilter*)GetFilter();
			return pNetRenderFilter->Receive(pObj, m_dwID, m_dwTransmittingType);
		}
	}
	else if (IsDecendant(CSWPosImage, pObj))
	{
		if (IsDecendant(CSWNetRenderFilter, GetFilter()))
		{	
			//PRINT("<Net Pin> Info: received a PosImage obj\n");
			CSWNetRenderFilter* pNetRenderFilter = (CSWNetRenderFilter*)GetFilter();
			return pNetRenderFilter->Receive(pObj, m_dwID, TRANSMITTING_REALTIME);
		}
	}
	
	return S_OK;
}



CSWNetRenderFilter::CSWNetRenderFilter(DWORD dwInCount, DWORD dwOutCount)
	:CSWBaseFilter(dwInCount, dwOutCount)
	,m_fInited(FALSE)
	,m_pLinkManager(NULL)
	,m_pTransmitterManagerThread(NULL)
	,m_pTransmitterListLock(NULL)
	,m_wLocalPort(0)
{

	m_pIn = new CSWNetRenderPin*[GetInCount()];
	if (NULL == m_pIn)
	{
		PRINT("Err: no memory for m_pIn\n");
	}
	
	for(DWORD i = 0; i < GetInCount(); i++)
	{
		m_pIn[i] = (CSWNetRenderPin*)CreateIn(i);
		if (NULL == m_pIn[i])
		{
			PRINT("Err: failed to create In Pin #%u\n", i);
		}
	}
	
	m_lstTransmitter.RemoveAll();
}



CSWNetRenderFilter::~CSWNetRenderFilter()
{
	if (!m_fInited)
	{		
		PRINT("Info: m_fInited = %d\n", m_fInited);
		return ;//S_OK;
	}

	//Stop();

	for(DWORD i = 0; i < GetInCount(); i++)
	{
		//m_pIn[i]->Release();
		GetIn(i)->Release();
	}
	delete []m_pIn;

	SAFE_RELEASE(m_pTransmitterManagerThread);

	m_pTransmitterListLock->Lock();
	while (!m_lstTransmitter.IsEmpty())
	{
		CDataTransmitter* pTransmitter = m_lstTransmitter.RemoveHead();
		CSWStream * pStream = pTransmitter->GetStream();		
		m_pLinkManager->ReleaseLink(pStream);			
		SAFE_RELEASE(pTransmitter);			
	}
	m_pTransmitterListLock->Unlock();

	SAFE_RELEASE(m_pTransmitterListLock);

	if (NULL != m_pLinkManager)
	{
		SAFE_RELEASE(m_pLinkManager);
	}
	

	m_fInited = FALSE;
}



CSWBasePin* CSWNetRenderFilter::CreateIn(DWORD dwNo)
{	
	return dwNo < GetInCount() ? new CSWNetRenderPin(this, 0, dwNo) : NULL;
}



CSWBasePin* CSWNetRenderFilter::GetIn(DWORD n)
{
	return  n < GetInCount() ? m_pIn[n] : NULL;
}



HRESULT CSWNetRenderFilter::Initialize(const WORD wPassivePort, const WORD wActivePort,  const CHAR * szDstIp, const BOOL fSafeSaverEnabled)
{
	HRESULT hr = S_OK;

	if (m_fInited)
	{
		return S_OK;
	}
	

	if (NULL == m_pTransmitterListLock)
	{
		m_pTransmitterListLock = new CSWMutex();
		if (NULL == m_pTransmitterListLock)
		{
			PRINT("Err: no enough memory for m_pTransmitterListLock\n");
			return E_OUTOFMEMORY;
		}
	}
	

	if (NULL == m_pLinkManager)
	{
		m_pLinkManager = new CLinkManager;
		if (NULL == m_pLinkManager)
		{
			PRINT("Err: failed to init m_pLinkManager\n");
			return E_OUTOFMEMORY;
		}

		if (FAILED(m_pLinkManager->Initialize(wPassivePort, wActivePort, szDstIp, fSafeSaverEnabled)))//todo: use real paramters to init
		{
			PRINT("Err: failed to initialize m_pLinkManager\n");
			return E_FAIL;
		}

		m_wLocalPort = wPassivePort;

		if (FAILED(m_pLinkManager->SetLinkCallback((PF_LINK_CALLBACK)LinkCallbackProxy, (PVOID)this)))
		{
			PRINT("Err: failed to set callback to m_pLinkManager\n");
			return E_FAIL;
		}		
	}

	if (NULL == m_pTransmitterManagerThread)
	{
		m_pTransmitterManagerThread = new CSWThread();
		if (NULL == m_pTransmitterManagerThread)
		{
			PRINT("Err: no enough memory for m_pTransmitterManagerThread\n");
			return E_OUTOFMEMORY;
		}
	}

	m_fInited = TRUE;

	return S_OK;
}





HRESULT CSWNetRenderFilter::Receive(CSWObject* obj, const DWORD dwInPinID, const DWORD dwTransmittingType)
{
	if (!m_fInited)
	{
		PRINT("Err: not initialized yet\n");
		return E_NOTIMPL;
	}

	static DWORD dwDiscardedCount = 0;
	static DWORD dwPrevTick = CSWDateTime::GetSystemTick();
	
	if (IsDecendant(CSWImage, obj) || IsDecendant(CSWPosImage, obj) || IsDecendant(CSWRecord, obj))
	{

		//if (IsDecendant(CSWRecord, obj)) PRINT("Info: received an %s obj\n", obj->Name());

		if (TRANSMITTING_HISTORY == dwTransmittingType)
		{
			if (FAILED(m_pTransmitterListLock->Lock(0)))
			{
				PRINT("Err: failed to get Transmitter\n");
				return E_FAIL;
			}
		}
		else
		{
			m_pTransmitterListLock->Lock();
		}
		
		

		SW_POSITION Pos = m_lstTransmitter.GetHeadPosition();
		while (m_lstTransmitter.IsValid(Pos))
		{
			CDataTransmitter* pTransmitter = m_lstTransmitter.GetNext(Pos);

			if (
				dwTransmittingType == pTransmitter->GetTransmittingType()
				&& dwInPinID == pTransmitter->GetPinID()
				)
			{
				HRESULT hr = pTransmitter->Send(obj);
				if (FAILED(hr))
				{
					//PRINT("Err: failed to send obj %s\n", obj->Name());
					dwDiscardedCount++;
				}

				if (dwDiscardedCount > 0)
				{
					DWORD dwCurTick = CSWDateTime::GetSystemTick();
					if (dwCurTick > dwPrevTick + 30000)
					{
						SW_TRACE_NORMAL("Warning: DataTransmitter discarded %d %s Obj in %d Sec.\n",
							dwDiscardedCount, obj->Name(), (dwCurTick - dwPrevTick)/1000 );
						dwDiscardedCount = 0;
						dwPrevTick = dwCurTick;
					}
				}

				if (FAILED(hr))
				{
					m_pTransmitterListLock->Unlock();
					return E_FAIL;
				}
			}
		}
		
		m_pTransmitterListLock->Unlock();
	}

	return S_OK;
}



HRESULT CSWNetRenderFilter::OnTransmitterManage()
{
	
	if (!m_fInited)
	{
		PRINT("Err: not inited yet\n");
		return E_NOTIMPL;
	}

	DWORD dwLastUpdateTimeMs = CSWDateTime::GetSystemTick();
	
	while (FILTER_RUNNING == GetState())
	{
		if( CSWDateTime::GetSystemTick() - dwLastUpdateTimeMs > 5 * 1000 )
		{
			CSWString strInfo = "";
			if( m_wLocalPort == CAMERA_RECORD_LINK_PORT )
			{
				strInfo.Append("结果链接:");
				strInfo.Append(GetConnectInfo());
			}
			else if( m_wLocalPort == CAMERA_IMAGE_LINK_PORT )
			{
				strInfo.Append("JPEG链接:");
				strInfo.Append(GetConnectInfo());
			}
			else if( m_wLocalPort == CAMERA_VIDEO_LINK_PORT )
			{
				strInfo.Append("H264链接:");
				strInfo.Append(GetConnectInfo());
			}
			CHAR szMsg[256] = {0};
			swpa_strcpy(szMsg, (const CHAR*)strInfo);
			CSWMessage::SendMessage(MSG_APP_UPDATE_STATUS, (WPARAM)szMsg, 0);

			dwLastUpdateTimeMs = CSWDateTime::GetSystemTick();
		}
		

		m_pTransmitterListLock->Lock();	
		SW_POSITION Pos = m_lstTransmitter.GetHeadPosition();
		BOOL	fUnlocked = FALSE;
		
		while (m_lstTransmitter.IsValid(Pos))
		{			
			SW_POSITION PrevPos = Pos;
			CDataTransmitter* pTransmitter = m_lstTransmitter.GetNext(Pos);		

			DWORD dwState = HISTORY_TRANSMITTING_ONGOING;
			if (TRANSMITTING_HISTORY == pTransmitter->GetTransmittingType())
			{
				if (FAILED(GetUpstreamHistoryFileTransmittingStatus(pTransmitter->GetPinID(), &dwState)))
				{
					//PRINT("Err: failed to call GetUpstreamHistoryFileTransmittingStatus()\n");
					dwState = HISTORY_TRANSMITTING_NOTSTARTED; //if failed to get upstream status, take it as "FINISHED"
				}

				if (HISTORY_TRANSMITTING_FINISHED == dwState)
				{
					PRINT("Info: dwState = HISTORY_TRANSMITTING_FINISHED\n");
				}
				
			}
			
			if (HISTORY_TRANSMITTING_FINISHED == dwState || pTransmitter->Done())
			{				
				m_lstTransmitter.RemoveAt(PrevPos); //remove the transmitter from the list and delete it
				m_pTransmitterListLock->Unlock();
				fUnlocked = TRUE;

				CSWNetRenderPin* pIn = (CSWNetRenderPin*)GetIn(pTransmitter->GetPinID());	

				if (TRANSMITTING_HISTORY == pIn->GetTransmittingType())
				{
					PRINT("Info: stop upstream (DataType: %d) history transmitting...\n", pTransmitter->GetDataType());
					StopUpstreamHistoryFileTransmitting(pTransmitter->GetPinID());
					
					pIn->SetTransmittingType(TRANSMITTING_TYPE_COUNT);
					pIn->Deoccupy();
					
				}
				else if (TRANSMITTING_REALTIME == pIn->GetTransmittingType())
				{
					PRINT("Info: stop upstream (DataType: %d) realtime transmitting...\n", pTransmitter->GetDataType());
					StopUpstreamRealtimeTransmitting(pTransmitter->GetPinID());
					pIn->SetTransmittingType(TRANSMITTING_TYPE_COUNT);
					pIn->Deoccupy();
				}
				
				//NOTE: we have to stop the Transmitter before the link being released!!
				pTransmitter->Stop();
				
				m_pLinkManager->ReleaseLink(pTransmitter->GetStream());

				SAFE_RELEASE(pTransmitter);

				break;
			}

			BOOL fTransmitterPending = FALSE;
			if (SUCCEEDED(pTransmitter->IsPending(&fTransmitterPending)) && fTransmitterPending)
			{
				CSWStream* pStream = NULL;
				if (SUCCEEDED(m_pLinkManager->UpdateStream(pTransmitter->GetStream(), &pStream)))
				{
					pTransmitter->UpdateStream(pStream);
					SW_TRACE_DEBUG("Info: pTransmitter stream updated\n");
				}
			}
		}

		if (!fUnlocked)
		{
			m_pTransmitterListLock->Unlock();
		}
		
		CSWApplication::Sleep(500);
	}

	PRINT("Info: %s() Stopped (Port = %d)!!\n", __FUNCTION__, m_wLocalPort);
	
	return S_OK;
	
}



VOID* CSWNetRenderFilter::OnTransmitterManageProxy(VOID* pvArg)
{
	if (NULL == pvArg)
	{		
		PRINT("Err: NULL == pvArg\n");
		return (VOID*)E_INVALIDARG;
	}
	
	CSWNetRenderFilter* pThis = (CSWNetRenderFilter*)pvArg;

	return (VOID*)pThis->OnTransmitterManage();
}



HRESULT CSWNetRenderFilter::Run()
{
	
	if (!m_fInited)
	{
		PRINT("Err: not inited yet\n");
		return E_NOTIMPL;
	}
	
	if (FAILED(CSWBaseFilter::Run()))
	{
		PRINT("Err: failed to run CSWBaseFilter::Run()\n");
		return E_FAIL;
	}

	m_pLinkManager->StartActiveLink();
	m_pLinkManager->StartPassiveLink();

	m_pTransmitterManagerThread->Start(OnTransmitterManageProxy, (VOID*)this);

	return S_OK;	
}



HRESULT CSWNetRenderFilter::Stop()
{
	if (!m_fInited)
	{
		PRINT("Err: not inited yet\n");
		return E_NOTIMPL;
	}
	
	if (FAILED(CSWBaseFilter::Stop()))
	{
		PRINT("Err: failed to call CSWBaseFilter::Stop()\n");
		return E_FAIL;
	}
	m_pLinkManager->StopActiveLink();
	m_pLinkManager->StopPassiveLink();
	m_pTransmitterManagerThread->Stop();

	m_pTransmitterListLock->Lock();
	while (!m_lstTransmitter.IsEmpty())
	{
		CDataTransmitter* pTransmitter = m_lstTransmitter.RemoveHead();
		CSWStream * pStream = pTransmitter->GetStream();
		m_pLinkManager->ReleaseLink(pStream);		
		SAFE_RELEASE(pTransmitter);			
	}
	m_pTransmitterListLock->Unlock();
	
	PRINT("Info: %s stopped!\n", this->Name());
	
	return S_OK;
}




HRESULT CSWNetRenderFilter::LinkCallback(CSWStream * pStream, PVOID pvHandshakeBuf)
{
	HRESULT hr = S_OK;
	
	if (!m_fInited)
	{
		PRINT("Err: not inited yet\n");
		return E_NOTIMPL;
	}
	
	if (NULL == pStream || NULL == pvHandshakeBuf)
	{
		PRINT("Err: NULL == pStream || NULL == pvHandshakeBuf\n");
		return E_INVALIDARG;
	}

	

	CAMERA_CMD_HEADER sHeader;
	//swpa_memset(&sHeader, 0, sizeof(sHeader));
	PBYTE pbHandshakeBuf = (PBYTE)pvHandshakeBuf;
	swpa_memcpy(&sHeader, pbHandshakeBuf, sizeof(sHeader));
	

	DWORD dwLen = sHeader.dwInfoSize;
	if (0 >= dwLen)
	{
		PRINT("Err: 0 == dwInfoSize\n");
		return E_FAIL;
	}
	

	
	PVOID pvBuf = swpa_mem_alloc(dwLen);
	if (NULL == pvBuf)
	{
		PRINT("Err: no enough memory for pvBuf\n");
		return E_OUTOFMEMORY;
	}
	
	swpa_memcpy(pvBuf, pbHandshakeBuf+sizeof(sHeader), dwLen);
	
	
	CDataTransmitter* pTransmitter = new CDataTransmitter(pStream);
	if (NULL == pTransmitter)
	{
		PRINT("Err: no enough memory for pTransmitter\n");

		SAFE_MEM_FREE(pvBuf);
		return E_OUTOFMEMORY;
	}

	//PRINT("Info: going to Handshake\n");
	if (CAMERA_XML_EXT_CMD == sHeader.dwID)
	{		
		hr = ParseHandshakeXml((const CHAR*)pvBuf, pTransmitter);
		SAFE_MEM_FREE(pvBuf);
	}
	else
	{
		PRINT("Err: Unknown CMD %d\n", sHeader.dwID);
		SAFE_MEM_FREE(pvBuf);
		SAFE_RELEASE(pTransmitter);
		return E_FAIL;
	}
	

	if (FAILED(hr))
	{
		PRINT("Err: failed to Handshake\n");

		SAFE_RELEASE(pTransmitter);
		
		return E_FAIL;
	}

	PRINT("Info: Handshake Succeeded\n");

	if (TRANSMITTING_HISTORY == pTransmitter->GetTransmittingType())
	{
		DWORD i = 0;
		for (i = 0; i < GetInCount(); i++)
		{
			CSWNetRenderPin* pIn = (CSWNetRenderPin*)GetIn(i);
			if (pIn->IsIdle())
			{
				pIn->Occupy();
				pIn->SetTransmittingType(TRANSMITTING_HISTORY);
				pTransmitter->SetPinID(i);

				SetUpstreamTransmittingType(i, TRANSMITTING_HISTORY);
				StartUpstreamHistoryFileTransmitting(i, pTransmitter->GetBeginTimeString(), pTransmitter->GetEndTimeString(), pTransmitter->GetCarID());
				
				break;
			}
		}
		
		if (i >= GetInCount())
		{
			PRINT("Err: no idle pin\n");
			
			SAFE_RELEASE(pTransmitter);
			
			return E_ACCESSDENIED; //All In-pins are busy now
		}
	}
	else if (TRANSMITTING_REALTIME == pTransmitter->GetTransmittingType())
	{
		DWORD i = 0;
		for (i = 0; i < GetInCount(); i++)
		{
			CSWNetRenderPin* pIn = (CSWNetRenderPin*)GetIn(i);
			if (pIn->IsIdle())
			{
				pIn->Occupy();
				pIn->SetTransmittingType(TRANSMITTING_REALTIME);
				pTransmitter->SetPinID(i);

				SetUpstreamTransmittingType(i, TRANSMITTING_REALTIME);
				StartUpstreamRealtimeTransmitting(i);
				break;
			}
		}
		
		if (i >= GetInCount())
		{
			PRINT("Err: no idle pin\n");
			
			SAFE_RELEASE(pTransmitter);
			
			return E_ACCESSDENIED; //All In-pins are busy now
		}		
	}
	
	
	m_pTransmitterListLock->Lock();
	m_lstTransmitter.AddTail(pTransmitter);
	m_pTransmitterListLock->Unlock();

	

	return S_OK;	
}



VOID* CSWNetRenderFilter::LinkCallbackProxy(CSWStream * pStream, PVOID* pvArg, PVOID pvHandshakeBuf)
{
	if (NULL == pStream || NULL == pvArg)
	{
		PRINT("Err: NULL == pStream || NULL == pvArg\n");
		return (VOID*)E_INVALIDARG;
	}

	CSWNetRenderFilter * pThis = (CSWNetRenderFilter*)pvArg;

	return (VOID*)pThis->LinkCallback(pStream, pvHandshakeBuf);
}





