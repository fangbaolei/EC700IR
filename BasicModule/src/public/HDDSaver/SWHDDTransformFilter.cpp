/**
* @file SWHDDTransformFilter.cpp
* @brief Implementation of CSWHDDTransformFilter
* 
* @copyright Signalway All Rights Reserved
* @author zhouy
* @date 2013-03-22
* @version 1.0
*/
	
#include "SWFC.h"
#include "SWHDDTransformFilter.h"
#include "SWTransmittingOpt.h"


#define PRINT SW_TRACE_DEBUG


typedef struct __HISTORY_TRANSIMITTING_THREAD_ARG
{
	CSWHDDTransformFilter * pThis;
	DWORD dwPinID;
}_HISTORY_TRANSIMITTING_THREAD_ARG;



/*HRESULT CSWHDDTransformFilter::ParseTime(SWPA_DATETIME_TM& myTime,  const CHAR * szTimeString)
{
	if (NULL == szTimeString)
	{
		PRINT("Err: NULL == szTimeString\n");
		return E_INVALIDARG;
	}

	if (0 == swpa_strcmp("", szTimeString))
	{
		PRINT("Err: szTimeString has no char\n");
		return E_INVALIDARG;
	}

	if ( 6 != swpa_sscanf(szTimeString, "%hd/%hd/%hd/%hd/%hd/%hd", 
				&myTime.year, &myTime.month, &myTime.day, &myTime.hour, &myTime.min, &myTime.sec))
	{
		PRINT("Err: Failed to parse time string %s\n", szTimeString);
		return E_FAIL;
	}

	return S_OK;
}*/


DWORD CSWHDDTransformFilter::s_dwCapacity = 0;
CSWMutex CSWHDDTransformFilter::s_cMutexCapacity;



CSWHDDTransformFilter::CSWHDDTransformFilter(const DWORD dwInCount, const DWORD dwOutCount)
  :CSWBaseFilter(dwInCount, dwOutCount)
  ,m_iSaveType(1)
  ,m_iSendInterval(0)
  ,m_fSaveHistoryFlag(TRUE)
  ,m_fInited(FALSE)
  ,m_pSaveThread(NULL)
  ,m_pBigFile(NULL)
  ,m_dwHistoryTransmittingStatus(NULL)
  ,m_fHistoryTransmitting(NULL)
  ,m_tHistoryBeginTime(NULL)
  ,m_tHistoryEndTime(NULL)
  ,m_dwCarID(NULL)
  ,m_fSendThreadStarted(NULL)
  ,m_dwOutType(NULL)
  ,m_pSendThread(NULL)
  ,m_fHistoryFileSaveDisabled(FALSE)
  ,m_dwLastTime(0)
{

	GetIn(0)->AddObject(CLASSID(CSWImage));
	GetOut(0)->AddObject(CLASSID(CSWImage));
	GetOut(1)->AddObject(CLASSID(CSWImage));
	
}


CSWHDDTransformFilter::~CSWHDDTransformFilter()
{
	//if (!m_fInited)
	{
		//return ;
	}

	if (!m_fHistoryFileSaveDisabled)
	{
		SAFE_RELEASE(m_pSaveThread);
		SAFE_DELETE(m_pBigFile);	

		SAFE_MEM_FREE(m_fSendThreadStarted);
		SAFE_MEM_FREE(m_dwOutType);
		SAFE_MEM_FREE(m_tHistoryBeginTime);
		SAFE_MEM_FREE(m_tHistoryEndTime);
		SAFE_MEM_FREE(m_dwCarID);
		SAFE_MEM_FREE(m_fHistoryTransmitting);
		SAFE_MEM_FREE(m_dwHistoryTransmittingStatus);
		
		if (NULL != m_pSendThread)
		{
			for(DWORD i = 0; i < GetOutCount(); i++)
			{
				if (NULL != m_pSendThread[i])
				{
					delete m_pSendThread[i];
					//m_pSendThread[i] = NULL;
				}
			}
			
			delete [] m_pSendThread;
			m_pSendThread = NULL;
		}
	}

	m_fInited = FALSE;
	
}


HRESULT CSWHDDTransformFilter::Initialize(const CHAR * szDirecory, const LONGLONG llTotalSize, const INT iFileSize, const BOOL fIsResult)
{
	if (m_fInited)
	{
		//return S_OK;
	}

	if (NULL == m_dwOutType)
	{
		m_dwOutType = (DWORD*)swpa_mem_alloc(sizeof(DWORD) * GetOutCount());
		if (NULL == m_dwOutType)
		{
			PRINT("Err: no enough memory for m_dwOutType \n");
			return E_OUTOFMEMORY;
		}
		swpa_memset(m_dwOutType, 0, sizeof(DWORD) * GetOutCount());
	}
	

	if (NULL == szDirecory && 0 == llTotalSize && 0 == iFileSize)
	{
		m_fHistoryFileSaveDisabled = TRUE;
		m_fInited = TRUE;
		
		return S_OK;
	}

	m_fHistoryFileSaveDisabled = FALSE;
	
	if (NULL == m_pBigFile)
	{
		m_pBigFile = new CBigFile();
		if (NULL == m_pBigFile)
		{
			PRINT("Err: no enough memory for m_pBigFile \n");
			return E_OUTOFMEMORY;
		}

		if (m_pBigFile->Initialize(szDirecory, llTotalSize, iFileSize, fIsResult))
		{
			PRINT("Err: failed to Initialize m_BigFile \n");

			SAFE_DELETE(m_pBigFile);
			return E_FAIL;
		}
	}

	if (NULL == m_fSendThreadStarted)
	{
		m_fSendThreadStarted = (BOOL*)swpa_mem_alloc(sizeof(BOOL) * GetOutCount());
		if (NULL == m_fSendThreadStarted)
		{
			PRINT("Err: no enough memory for m_fSendThreadStarted \n");
			return E_OUTOFMEMORY;
		}
		swpa_memset(m_fSendThreadStarted, 0, sizeof(BOOL) * GetOutCount());
	}

	

	if (NULL == m_tHistoryBeginTime)
	{
		m_tHistoryBeginTime = (SWPA_DATETIME_TM*)swpa_mem_alloc(sizeof(SWPA_DATETIME_TM) * GetOutCount());
		if (NULL == m_tHistoryBeginTime)
		{
			PRINT("Err: no enough memory for m_tHistoryBeginTime \n");
			return E_OUTOFMEMORY;
		}
		swpa_memset(m_tHistoryBeginTime, 0, sizeof(SWPA_DATETIME_TM) * GetOutCount());
	}

	if (NULL == m_tHistoryEndTime)
	{
		m_tHistoryEndTime = (SWPA_DATETIME_TM*)swpa_mem_alloc(sizeof(SWPA_DATETIME_TM) * GetOutCount());
		if (NULL == m_tHistoryEndTime)
		{
			PRINT("Err: no enough memory for m_tHistoryEndTime \n");
			return E_OUTOFMEMORY;
		}
		swpa_memset(m_tHistoryEndTime, 0, sizeof(SWPA_DATETIME_TM) * GetOutCount());
	}

	
	if (NULL == m_dwCarID)
	{
		m_dwCarID = (DWORD*)swpa_mem_alloc(sizeof(DWORD) * GetOutCount());
		if (NULL == m_dwCarID)
		{
			PRINT("Err: no enough memory for m_dwCarID \n");
			return E_OUTOFMEMORY;
		}
		swpa_memset(m_dwCarID, 0, sizeof(DWORD) * GetOutCount());
	}

	if (NULL == m_fHistoryTransmitting)
	{
		m_fHistoryTransmitting = (BOOL*)swpa_mem_alloc(sizeof(BOOL) * GetOutCount());
		if (NULL == m_fHistoryTransmitting)
		{
			PRINT("Err: no enough memory for m_fHistoryTransmitting \n");
			return E_OUTOFMEMORY;
		}
		swpa_memset(m_fHistoryTransmitting, 0, sizeof(BOOL) * GetOutCount());
	}

	if (NULL == m_dwHistoryTransmittingStatus)
	{
		m_dwHistoryTransmittingStatus = (DWORD*)swpa_mem_alloc(sizeof(DWORD) * GetOutCount());
		if (NULL == m_dwHistoryTransmittingStatus)
		{
			PRINT("Err: no enough memory for m_dwHistoryTransmittingStatus \n");
			return E_OUTOFMEMORY;
		}
		swpa_memset(m_dwHistoryTransmittingStatus, 0, sizeof(DWORD) * GetOutCount());
	}
	
	if (NULL == m_pSendThread)
	{
		m_pSendThread = new CSWThread*[GetOutCount()];
		if (NULL == m_pSendThread)
		{
			PRINT("Err: no enough memory for m_pSendThread \n");
			return E_OUTOFMEMORY;
		}

		for(DWORD i = 0; i < GetOutCount(); i++)
		{
			m_pSendThread[i] = new CSWThread();
			if (NULL == m_pSendThread[i])
			{
				PRINT("Err: no enough memory for m_pSendThread[i] \n");
				return E_OUTOFMEMORY;
			}
		}
	}


	/*if (NULL == m_pFileMutex)
	{
		m_pFileMutex = new CSWMutex();
		if (NULL == m_pFileMutex)
		{
			PRINT("Err: no enough memory for m_pFileMutex \n");
			return E_OUTOFMEMORY;
		}
	}*/

	if (NULL == m_pSaveThread)
	{
		m_pSaveThread = new CSWThread();
		if (NULL == m_pSaveThread)
		{
			PRINT("Err: no enough memory for m_pSaveThread \n");
			return E_OUTOFMEMORY;
		}
	}


	/*
	while (!m_lstFrameBuf.IsEmpty())
	{
		VOID * pvFrame = m_lstFrameBuf.GetHead();
		m_lstFrameBuf.RemoveHead();

		SAFE_MEM_FREE(pvFrame);
	}	
	
	m_pFileMutex->Lock(-1);
	while (!m_lstFile.IsEmpty())
	{
		VOID * pvFile = m_lstFile.GetHead();
		m_lstFile.RemoveHead();

		SAFE_MEM_FREE(pvFile);
	}	
	m_pFileMutex->Unlock();
	*/
	
	m_fInited = TRUE;

	return S_OK;
}



HRESULT CSWHDDTransformFilter::SetTransmittingType(const DWORD dwPinID, const DWORD dwOutType)
{
	if (!m_fInited)
	{
		PRINT("Err: not initialized yet\n");
		return E_NOTIMPL;
	}

	if (m_fHistoryFileSaveDisabled && TRANSMITTING_HISTORY == dwOutType)
	{
		PRINT("Err: History file saving is disabled\n");
		return E_FAIL;
	}

	if (GetOutCount() <= dwPinID)
	{
		PRINT("Err: invalid dwPinID value (%u)\n", dwPinID);
		return E_INVALIDARG;
	}

	if (TRANSMITTING_TYPE_COUNT <= dwOutType)
	{
		PRINT("Err: invalid iOutType value (%d)\n", dwOutType);
		return E_INVALIDARG;
	}
	
	m_dwOutType[dwPinID] = dwOutType;

	if (TRANSMITTING_HISTORY == dwOutType)
	{
		m_dwHistoryTransmittingStatus[dwPinID] = HISTORY_TRANSMITTING_NOTSTARTED;
	}

	return S_OK;

}




HRESULT CSWHDDTransformFilter::StartHistoryFileTransmitting(const DWORD dwPinID, const SWPA_DATETIME_TM * psTimeBegin, const SWPA_DATETIME_TM * psTimeEnd, const DWORD dwCarID)
{
	if (!m_fInited)
	{
		PRINT("Err: not initialized yet\n");
		return E_NOTIMPL;
	}

	if (m_fHistoryFileSaveDisabled)
	{
		PRINT("Err: History file saving is disabled\n");
		return E_FAIL;
	}

	if (GetOutCount() <= dwPinID)
	{
		PRINT("Err: invalid dwPinID value (%u)\n", dwPinID);
		return E_INVALIDARG;
	}

	if (TRANSMITTING_HISTORY != m_dwOutType[dwPinID])
	{
		PRINT("Err: Out pin (%u) wasn't configed to output history file\n", dwPinID);
		return E_INVALIDARG;
	}

	if (NULL == psTimeBegin || NULL == psTimeEnd)
	{
		PRINT("Err: NULL == psTimeBegin || NULL == psTimeEnd \n");
		return E_INVALIDARG;
	}

	m_tHistoryBeginTime[dwPinID] = *psTimeBegin;
	m_tHistoryEndTime[dwPinID] = *psTimeEnd;
	m_dwCarID[dwPinID] = dwCarID;
	

	_HISTORY_TRANSIMITTING_THREAD_ARG * pArg = (_HISTORY_TRANSIMITTING_THREAD_ARG*)swpa_mem_alloc(sizeof(_HISTORY_TRANSIMITTING_THREAD_ARG));
	if (NULL == pArg)
	{
		PRINT("Err: no enough memory for pArg \n");
		return E_OUTOFMEMORY;
	}
	
	StopHistoryFileTransmitting(dwPinID);//m_pSendThread[dwPinID]->Stop();

	m_fHistoryTransmitting[dwPinID] = TRUE;

	pArg->pThis = this;
	pArg->dwPinID = dwPinID;
	HRESULT hr = m_pSendThread[dwPinID]->Start(OnHistoryFileTransmittingProxy, pArg);
	if (S_OK != hr)
	{
		PRINT("Err: failed to start history file transmitting thread (OutPin = %u) \n", dwPinID);
		
		SAFE_MEM_FREE(pArg);
	}
	
	return hr;
	
}



HRESULT CSWHDDTransformFilter::StopHistoryFileTransmitting(const DWORD dwPinID)
{
	if (!m_fInited)
	{
		PRINT("Err: not initialized yet\n");
		return E_NOTIMPL;
	}

	if (m_fHistoryFileSaveDisabled)
	{
		PRINT("Err: History file saving is disabled\n");
		return E_FAIL;
	}

	if (GetOutCount() <= dwPinID)
	{
		PRINT("Err: invalid dwPinID value (%u)\n", dwPinID);
		return E_INVALIDARG;
	}

	if (TRANSMITTING_HISTORY != m_dwOutType[dwPinID])
	{
		PRINT("Err: Out pin (%u) wasn't configed to output history file\n", dwPinID);
		return E_INVALIDARG;
	}

	m_fHistoryTransmitting[dwPinID] = FALSE;
	PRINT("dwPinID = %lu\n", dwPinID);


	if (FAILED(m_pSendThread[dwPinID]->Stop()))
	{
		PRINT("Err: failed to stop history file transmitting thread\n");
		return E_FAIL;
	}
	else
	{
		PRINT("Info: history file transmitting thread stopped\n");
		return S_OK;
	}
}



HRESULT CSWHDDTransformFilter::StartRealtimeTransmitting(const DWORD dwPinID)
{
	if (!m_fInited)
	{
		PRINT("Err: not initialized yet\n");
		return E_NOTIMPL;
	}

	if (GetOutCount() <= dwPinID)
	{
		PRINT("Err: invalid dwPinID value (%u)\n", dwPinID);
		return E_INVALIDARG;
	}

	if (0 == GetSaveType())
	{
		SetSaveHistoryFlag(FALSE);
	}

	return S_OK;
}



HRESULT CSWHDDTransformFilter::StopRealtimeTransmitting(const DWORD dwPinID)
{
	if (!m_fInited)
	{
		PRINT("Err: not initialized yet\n");
		return E_NOTIMPL;
	}

	if (GetOutCount() <= dwPinID)
	{
		PRINT("Err: invalid dwPinID value (%u)\n", dwPinID);
		return E_INVALIDARG;
	}

	if (0 == GetSaveType())
	{
		SetSaveHistoryFlag(TRUE);
	}

	m_dwOutType[dwPinID] = TRANSMITTING_TYPE_COUNT;

	return S_OK;
}
	



	

HRESULT CSWHDDTransformFilter::GetHistoryFileTransmittingStatus(const DWORD dwPinID, DWORD* pdwStatus)
{
	if (!m_fInited)
	{
		PRINT("Err: not initialized yet\n");
		return E_NOTIMPL;
	}

	if (m_fHistoryFileSaveDisabled)
	{
		PRINT("Err: History file saving is disabled\n");
		return E_FAIL;
	}
	
	if (NULL == pdwStatus)
	{
		PRINT("Err: NULL == pdwStatus\n");
		return E_POINTER;
	}

	if (GetOutCount() <= dwPinID)
	{
		PRINT("Err: invalid dwPinID value (%u)\n", dwPinID);
		return E_INVALIDARG;
	}

	if (TRANSMITTING_HISTORY != m_dwOutType[dwPinID])
	{
		PRINT("Err: dwPinID wasn't configed to transmit history file (%u)\n", dwPinID);
		return E_INVALIDARG;
	}

	*pdwStatus = m_dwHistoryTransmittingStatus[dwPinID];
	
	return S_OK;
}






HRESULT CSWHDDTransformFilter::Run()
{
	if (!m_fInited)
	{
		PRINT("Err: not initialized yet\n");
		return E_NOTIMPL;
	}

	if (FAILED(CSWBaseFilter::Run()))
	{
		PRINT("Err: Failed to run this filter\n");
		return E_FAIL;
	}
	
	StartSaving();

	//PRINT("Info: CSWHDDTransformFilter Running...!\n");

	return S_OK;
	
}


HRESULT CSWHDDTransformFilter::Stop()
{
	if (!m_fInited)
	{
		PRINT("Err: not initialized yet\n");
		return E_NOTIMPL;
	}

	if (FAILED(CSWBaseFilter::Stop()))
	{
		PRINT("Err: Failed to stop this filter\n");
		return E_FAIL;
	}

	if (!m_fHistoryFileSaveDisabled)
	{
		//stop the file saving thread
		if (S_OK != m_pSaveThread->Stop())
		{
			PRINT("Err: failed to start file saving thread.\n");
			return E_FAIL;
		}
	}
	PRINT("Info: CSWHDDTransformFilter stopped!\n");

	return S_OK;
}



HRESULT CSWHDDTransformFilter::ClearBuffer()
{
	if (!m_fInited)
	{
		PRINT("Err: not initialized yet\n");
		return E_NOTIMPL;
	}
/*
	//stop the file saving thread
	if (S_OK != m_pSaveThread->Stop())
	{
		PRINT("Err: failed to start file saving thread.\n");
		return E_FAIL;
	}

	PRINT("Info: SaveThread stopped!\n");
*/
	return S_OK;
}


HRESULT CSWHDDTransformFilter::StartSaving()
{
	if (!m_fInited)
	{
		PRINT("Err: not initialized yet\n");
		return E_NOTIMPL;
	}

	if (!m_fHistoryFileSaveDisabled)
	{
		//start the file saving thread
		//ClearBuffer();	
		m_dwLastTime = CSWDateTime::GetSystemTick();;
		if (S_OK != m_pSaveThread->Start(OnHistoryFileSavingProxy, this))
		{
			PRINT("Err: failed to start file saving thread.\n");
			return E_FAIL;
		}
	}

	return S_OK;
}





CSWImage * CSWHDDTransformFilter::CreateImageFromBuffer(const PBYTE pbBuf, const DWORD dwBufSize)
{	
	HRESULT hr = S_OK;
	
	if (!m_fInited)
	{
		PRINT("Err: not initialized yet\n");
		return NULL;
	}
	
	if (NULL == pbBuf)
	{
		PRINT("Err: NULL == pbBuf\n");
		return NULL;
	}

	_IMAGE_FILE_INFO sImageInfo;
	swpa_memcpy(&sImageInfo, pbBuf, sizeof(_IMAGE_FILE_INFO));

	//TODO: data checking, should be replaced by CRC32 
	if (sImageInfo.sComponent.cImageType >= SW_IMAGE_TYPE_COUNT || sImageInfo.sComponent.cImageType < SW_IMAGE_YUV_422
		|| sImageInfo.sComponent.iSize > dwBufSize || sImageInfo.sComponent.iSize <= 0
		|| sImageInfo.sComponent.iWidth <= 0 || sImageInfo.sComponent.iHeight <= 0)
	{
		PRINT("Err: data validating failed!\n");
		return NULL;
	}
	
	//create a new image for history image file transmitting
	CSWMemoryFactory * pMemoryFactory = CSWMemoryFactory::GetInstance(SW_SHARED_MEMORY);
	if (NULL == pMemoryFactory)
	{
		PRINT("Err: no enough memory for pMemoryFactory\n");
		return NULL;
	}

	PRINT("Info: dwBufSize = %lu\n", dwBufSize);	
	
	
	CSWImage * pNewImage = NULL;		
	hr = CSWImage::CreateSWImage(&pNewImage, 
		sImageInfo.sComponent.cImageType, 
		sImageInfo.sComponent.iWidth, 
		sImageInfo.sComponent.iHeight,
		pMemoryFactory, 
		sImageInfo.dwFrameNo, 
		sImageInfo.dwRefTimeMS, 
		sImageInfo.fIsCaptureImage,
		sImageInfo.szFrameName,
		sImageInfo.sComponent.iSize);
	if (S_OK != hr)
	{
		PRINT("Err: failed to CreateSWImage() [0x%x]\n", hr);
		return NULL;
	}

	SW_COMPONENT_IMAGE sNewComponentImage;
	pNewImage->GetImage(&sNewComponentImage);
	
	swpa_memcpy(sNewComponentImage.rgpbData[0], (VOID*)((INT)pbBuf + sizeof(_IMAGE_FILE_INFO)), sImageInfo.sComponent.iSize/*dwBufSize*/);

	sNewComponentImage.iSize = sImageInfo.sComponent.iSize;
	pNewImage->SetImage(sNewComponentImage);


	DWORD dwTimeHigh = 0;
	DWORD dwTimeLow = 0;
	CSWDateTime::TimeConvert(sImageInfo.sTimeStamp, &dwTimeHigh, &dwTimeLow);
	pNewImage->SetRealTime(dwTimeHigh, dwTimeLow);
	
	return pNewImage;//S_OK;

}



VOID* CSWHDDTransformFilter::OnHistoryFileSavingProxy(VOID* pvParam)
{
	if (NULL == pvParam)
	{
		PRINT("Err: NULL == pvParam\n");
		return (VOID*)E_INVALIDARG;
	}


	CSWHDDTransformFilter* pThis = (CSWHDDTransformFilter*)pvParam;

	if (pThis->m_fHistoryFileSaveDisabled)
	{
		PRINT("Err: History file saving is disabled\n");
		return (VOID*)E_FAIL;
	}
	
	return (VOID*)pThis->OnHistoryFileSaving();
}

FILTER_STATE CSWHDDTransformFilter::GetState()
{
	DWORD dwTick = CSWDateTime::GetSystemTick();
	if(!m_fHistoryFileSaveDisabled && m_dwLastTime > 0 && dwTick > m_dwLastTime && dwTick - m_dwLastTime > 30000)
	{
		PRINT("Err: %s time out.", Name());
		//CSWApplication::Exit(2);
		ReportStatus("Òì³£");
		m_fHistoryFileSaveDisabled = TRUE;
		PRINT("Info: m_fHistoryFileSaveDisabled = %d.\n", m_fHistoryFileSaveDisabled);
		//ClearBuffer();	
		m_dwLastTime = dwTick;
		CSWMessage::SendMessage(
			MSG_APP_REINIT_HDD
			, 0 == swpa_strcmp(Name(), "CSWH264HDDTransformFilter") ? 0 : 1
			, 0
			);
		//return FILTER_PAUSED;
	}
	return CSWBaseFilter::GetState();
}


VOID* CSWHDDTransformFilter::OnHistoryFileTransmittingProxy(VOID* pvParam)
{
	if (NULL == pvParam)
	{
		PRINT("Err: NULL == pvParam\n");
		return (VOID*)E_INVALIDARG;
	}
	
	_HISTORY_TRANSIMITTING_THREAD_ARG * pArg = (_HISTORY_TRANSIMITTING_THREAD_ARG *)pvParam;
	CSWHDDTransformFilter* pThis = pArg->pThis;

	if (pThis->m_fHistoryFileSaveDisabled)
	{
		PRINT("Err: History file saving is disabled\n");
		return (VOID*)E_FAIL;
	}

	HRESULT hr = pThis->OnHistoryFileTransmitting(pArg->dwPinID);

	SAFE_MEM_FREE(pvParam);

	return (VOID*)hr;
}


HRESULT CSWHDDTransformFilter::GetImageTimeStamp(CSWImage * pImage, SWPA_TIME* pTimeStamp)
{
	CSWDateTime objTime(pImage->GetRefTime());
	
	if (NULL == pImage || NULL == pTimeStamp)
	{
		PRINT("Err: NULL == pImage || NULL == pTimeStamp\n");
		return E_INVALIDARG;
	}

	DWORD dwTimeHigh = 0;
	DWORD dwTimeLow  = 0;

	pImage->GetRealTime(&dwTimeHigh, &dwTimeLow);

	//PRINT("Info: dwTimeHigh = %lu, dwTimeLow = %lu\n", dwTimeHigh, dwTimeLow);
	
	if (0 == dwTimeHigh && 0 == dwTimeLow)
	{
		
		return objTime.GetTime(pTimeStamp);
	}
	else
	{
		return CSWDateTime::TimeConvert(dwTimeHigh, dwTimeLow, pTimeStamp);
	}
	
}




HRESULT CSWHDDTransformFilter::UpdateCapacity(const DWORD dwSize)
{
	static SWPA_TIME sPrevTime;
	CSWDateTime cCurTime;
	SWPA_TIME sTime;

	cCurTime.GetTime(&sTime);
	DWORD dwAlignSize = ALGIN_SIZE(dwSize, 512*1024);


	s_cMutexCapacity.Lock();
	if (0 == sPrevTime.msec && 0 == sPrevTime.sec)
	{
		sPrevTime = sTime;
		s_dwCapacity = dwAlignSize;
	}
	else
	{	
		if (sPrevTime.sec != sTime.sec)
		{
			PRINT("Info: Capacity: %d\n", s_dwCapacity);
			s_dwCapacity = dwAlignSize;
			sPrevTime = sTime;
		}
		else
		{	
			s_dwCapacity += dwAlignSize;
		}
	}
	s_cMutexCapacity.Unlock();
	
	return S_OK;
}


BOOL CSWHDDTransformFilter::CheckCurrentCapacity()
{
	UpdateCapacity(0);
	
	s_cMutexCapacity.Lock();
	DWORD dwCapacity = s_dwCapacity;
	s_cMutexCapacity.Unlock();
	
	return (dwCapacity < 4*1024*1024) ? TRUE : FALSE ;
}


