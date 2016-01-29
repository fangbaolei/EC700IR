/**
* @file SWResultHDDTransformFilter.cpp
* @brief Implementation of CSWResultHDDTransformFilter
* 
* @copyright Signalway All Rights Reserved
* @author zhouy
* @date 2013-03-22
* @version 1.0
*/
	
#include "SWFC.h"
#include "SWResultHDDTransformFilter.h"
#include "SWTransmittingOpt.h"


#define PRINT SW_TRACE_DEBUG


typedef enum
{
    RESULT_IMAGE_BEST_SNAPSHOT = 0,
    RESULT_IMAGE_LAST_SNAPSHOT,
    RESULT_IMAGE_BEGIN_CAPTURE,
    RESULT_IMAGE_BEST_CAPTURE,
    RESULT_IMAGE_LAST_CAPTURE,
    RESULT_IMAGE_SMALL_IMAGE,
    RESULT_IMAGE_BIN_IMAGE
}
RECORD_IMAGE_TYPE;



CSWResultHDDTransformFilter::CSWResultHDDTransformFilter()
  :CSWHDDTransformFilter(1, 3)
  ,CSWMessage(MSG_RESULTHDD_FILTER_CTRL_START, MSG_RESULTHDD_FILTER_CTRL_END)
  ,m_fInited(FALSE)
  ,m_pFileMutex(NULL)
  ,m_pSemaSaveQueueSync(NULL)
  ,m_dwSaveQueueSize(4)
  ,m_iOutputType(0)
  ,m_pcMemoryFactory(NULL)
  ,m_iFilterUnSurePeccancy(0)
  ,m_iSaveResultType(0)
{
	GetIn(0)->AddObject(CLASSID(CSWRecord));
	//GetIn(1)->AddObject(CLASSID(CSWString));
	GetOut(0)->AddObject(CLASSID(CSWRecord));
	GetOut(1)->AddObject(CLASSID(CSWRecord));
	GetOut(2)->AddObject(CLASSID(CSWRecord));
}


CSWResultHDDTransformFilter::~CSWResultHDDTransformFilter()
{
	if (!m_fInited)
	{
		return ;
	}

	if (!m_fHistoryFileSaveDisabled)
	{	
		m_pFileMutex->Lock();
		while (!m_lstFile.IsEmpty())
		{
			CSWRecord* pRecord = m_lstFile.RemoveHead();
			SAFE_RELEASE(pRecord);
		}
		m_pFileMutex->Unlock();

		SAFE_RELEASE(m_pFileMutex);
		SAFE_RELEASE(m_pSemaSaveQueueSync);
	}

	m_pcMemoryFactory = NULL;
	
	m_fInited = FALSE;	
}


HRESULT CSWResultHDDTransformFilter::Initialize(const CHAR * szDirecory, const LONGLONG llTotalSize, const INT iFileSize,  const INT iSaveType)
{
	if (m_fInited)
	{
		//return S_OK;
	}

	m_iSaveResultType = iSaveType;

	if (FAILED(CSWHDDTransformFilter::Initialize(szDirecory, llTotalSize, iFileSize, TRUE)))
	{
		PRINT("Err: failed to initialize CSWResultHDDTransformFilter!\n");
		return E_FAIL;
	}

	m_pcMemoryFactory = CSWMemoryFactory::GetInstance(SW_SHARED_MEMORY);
	if (NULL == m_pcMemoryFactory)
	{
		PRINT("Err: failed to get m_pcMemoryFactory instance\n");
		return E_FAIL;
	}

	if (m_fHistoryFileSaveDisabled)
	{
		PRINT("Info: disable history file saving!\n");
	}
	else
	{
		if (NULL == m_pFileMutex)
		{
			m_pFileMutex = new CSWMutex();
			if (NULL == m_pFileMutex)
			{
				PRINT("Err: no memory for m_pFileMutex!\n");
				return E_FAIL;
			}
		}

		if (NULL == m_pSemaSaveQueueSync)
		{
			m_pSemaSaveQueueSync = new CSWSemaphore(0, m_dwSaveQueueSize);
			if (NULL == m_pSemaSaveQueueSync)
			{
				PRINT("Err: no memory for m_pSemaSaveQueueSync!\n");
				return E_FAIL;
			}
		}

		m_lstFile.RemoveAll();

		m_lstFile.SetMaxCount(m_dwSaveQueueSize);
	}
	
	m_fInited = TRUE;	

	return S_OK;
}


HRESULT CSWResultHDDTransformFilter::Receive(CSWObject * pObj)
{
	if (!m_fInited)
	{
		PRINT("Err: not initialized yet\n");
		return E_NOTIMPL;
	}
	
	if (IsDecendant(CSWRecord, pObj))
	{		
		PRINT("Info: ResultHdd got a CSWRecord obj\n");
		CSWRecord* pRecord = (CSWRecord*)pObj;

		if (NULL == pRecord)
		{
			PRINT("Err: NULL == pRecord\n");
			return E_INVALIDARG;
		}

		if (!pRecord->IsNormal() && GetFilterUnSurePeccancy() && pRecord->GetUnSurePeccancy())
		{
			pRecord->SetPTType(PT_NORMAL);
		}
		BOOL fIsPeccancySrc = (pRecord->GetPTType() != PT_NORMAL);

		//send real time image immediately
		BOOL fPeccancy = FALSE;
		for (INT i=0; i<GetOutCount(); i++)
		{
			if (TRANSMITTING_REALTIME == m_dwOutType[i])
			{	
				// 判断是否需要发送
				BOOL fSend = TRUE;
				if (!pRecord->GetTrafficInfoFlag())
				{
					LPCSTR szText = pRecord->GetXmlString();
					fPeccancy = (NULL != swpa_strstr(szText, "违章:是"));
					if(m_iOutputType == 1 && (pRecord->GetPTType() == PT_NORMAL || !fPeccancy))
					{
						fSend = FALSE;
					}
					else if(m_iOutputType == 2 && (pRecord->GetPTType() != PT_NORMAL || fPeccancy))
					{
						fSend = FALSE;
					}
					//PRINT("\n[]\nm_dwOutType[%d]:%d,m_iOutputType:%d,fSend:%d", i, m_dwOutType[i], m_iOutputType, fSend);
					SW_TRACE_DEBUG("\n[]\nm_dwOutType[%d]:%d,m_iOutputType:%d,fSend:%d", i, m_dwOutType[i], m_iOutputType, fSend);
				}

				if( fSend )
				{
					GetOut(i)->Deliver(pRecord);
				}

			}
		}		
		

		if (!m_fHistoryFileSaveDisabled && GetSaveHistoryFlag())
		{
			//for history file saving
			BOOL fDoSave = TRUE;
			if( m_iSaveResultType == 1 && !fIsPeccancySrc)
			{
				fDoSave = FALSE;
			}

			//违章结果触发H264录像存储
			if( m_iSaveResultType == 1 && fIsPeccancySrc )
			{
				SW_TRACE_DEBUG("Info: trigger h264 video saving.\n");
				CSWMessage::SendMessage(MSG_H264HDD_FILTER_TRIGGER_VIDEO_SAVING, (WPARAM)pRecord->GetRefTime(), 0);
			}

			if( fDoSave )
			{
				m_pFileMutex->Lock();

				if (m_lstFile.IsFull())
				{
					SW_TRACE_DEBUG("Warning: ResultHdd Save Queue is full! discards this %s obj\n", pObj->Name());
					m_pFileMutex->Unlock();
					return S_OK;
				}

				SAFE_ADDREF(pRecord);
				m_lstFile.AddTail(pRecord);
				m_pSemaSaveQueueSync->Post();
				m_pFileMutex->Unlock();
			}

		}
	}
	


	return S_OK;
}



HRESULT CSWResultHDDTransformFilter::ReportStatus(LPCSTR szInfo)
{
	if (!m_fInited)
	{
		PRINT("Err: not initialized yet\n");
		return E_NOTIMPL;
	}

	CSWString strInfo = "";
	strInfo.Append("结果存储:");	
	strInfo.Append(NULL != szInfo ? CSWString(szInfo) : GetStorageStatus());
	
	return CSWMessage::SendMessage(MSG_APP_UPDATE_STATUS, (WPARAM)(const CHAR*)strInfo, 0);
}


HRESULT CSWResultHDDTransformFilter::ClearBuffer()
{
	m_fHistoryFileSaveDisabled = TRUE;

	CSWHDDTransformFilter::ClearBuffer();
	m_dwLastTime = CSWDateTime::GetSystemTick();
	if (NULL != m_pFileMutex)
	{
		m_pFileMutex->Lock();
		while (!m_lstFile.IsEmpty())
		{
			CSWRecord* pRecord = m_lstFile.RemoveHead();
			SAFE_RELEASE(pRecord);
		}
		m_pFileMutex->Unlock();

		//SAFE_RELEASE(m_pFileMutex);
		//SAFE_RELEASE(m_pSemaSaveQueueSync);
	}
}


	

HRESULT CSWResultHDDTransformFilter::SaveRecord(CSWRecord * pRecord)
{
	if (!m_fInited)
	{
		PRINT("Err: not initialized yet\n");
		return E_NOTIMPL;
	}
	
	if (NULL == pRecord)
	{
		PRINT("Err: NULL == pRecord\n");
		return E_INVALIDARG;
	}

	HRESULT		hr = S_OK;	
	
	//SAFE_ADDREF(pRecord); //already added reference

	INT iInfoSize = pRecord->GetXmlSize();
	INT iBufSize  =	0;
	DWORD dwSizeCrc = 0;
	DWORD dwDataCrc = 0;
	iBufSize += sizeof(iBufSize) + sizeof(dwSizeCrc) + sizeof(dwDataCrc); //for crc
	iBufSize += sizeof(iInfoSize) + iInfoSize; // for Info
	DWORD dwImageCount = 0;
	iBufSize += sizeof(dwImageCount);//for Image count

	for (DWORD i = 0; i < pRecord->GetImageTypeCount(); i++)
	{
		if (NULL != pRecord->GetImage(i))
		{	
			dwImageCount++;
			iBufSize += sizeof(DWORD/*dwImageID*/) +sizeof(DWORD/*dwImageDataSize*/) + sizeof(_IMAGE_FILE_INFO) + pRecord->GetImage(i)->GetSize();
		}
	}

	//PRINT("iBufSize = %d KB\n", iBufSize/1024);

	CSWMemory* pcMemBuf = m_pcMemoryFactory->Alloc(ALGIN_SIZE(iBufSize, 128*1024));//alignment, reduce memory fragment
	if (NULL == pcMemBuf)
	{
		PRINT("Err: no memory for pcMemBuf\n");
		return E_OUTOFMEMORY;
	}
	
	PBYTE pbBuf = (PBYTE)pcMemBuf->GetBuffer();//(PBYTE)swpa_mem_alloc(sizeof(BYTE) * iBufSize);
	if (NULL == pbBuf)
	{
		PRINT("Err: no memory for pbBuf\n");
		return E_OUTOFMEMORY;
	}
	swpa_memset(pbBuf, 0, iBufSize);
	
	PBYTE pbAddr = pbBuf;

	/*save Size crc info*/
	dwSizeCrc = CSWUtils::CalcCrc32(0, (BYTE*)&iBufSize, sizeof(iBufSize));
	swpa_memcpy(pbAddr, &dwSizeCrc, sizeof(dwSizeCrc));
	pbAddr += sizeof(dwSizeCrc);
	swpa_memcpy(pbAddr, &iBufSize, sizeof(iBufSize));
	pbAddr += sizeof(iBufSize);

	/*save Data crc info*/
	swpa_memcpy(pbAddr, &dwDataCrc, sizeof(dwDataCrc));		//will update this later in this func
	pbAddr += sizeof(dwDataCrc);
	
	
	/*save Info size */
	swpa_memcpy(pbAddr, &iInfoSize, sizeof(iInfoSize));
	pbAddr += sizeof(iInfoSize);

	
	/*save Info */
	swpa_memcpy(pbAddr, pRecord->GetXmlString(), iInfoSize);
	pbAddr += iInfoSize;


	/*save image count */
	swpa_memcpy(pbAddr, &dwImageCount, sizeof(dwImageCount));
	pbAddr += sizeof(dwImageCount);
	
	
	//PRINT("iBufSize=%d\n", iBufSize);
	
	/*save each image */
	for (DWORD i=0; i<pRecord->GetImageTypeCount(); i++)
	{
		CSWImage * pImage = pRecord->GetImage(i);
		if (NULL != pImage)
		{	
			/*save image ID */				
			swpa_memcpy(pbAddr, &i, sizeof(i));
			pbAddr += sizeof(i);


			_IMAGE_FILE_INFO sImageInfo;
			swpa_memset(&sImageInfo, 0, sizeof(_IMAGE_FILE_INFO));
			sImageInfo.dwFrameNo = pImage->GetFrameNo();
			sImageInfo.dwRefTimeMS = pRecord->GetRefTime();//pImage->GetRefTime();??
			sImageInfo.fIsCaptureImage = pImage->IsCaptureImage();
			swpa_strncpy(sImageInfo.szFrameName, pImage->GetFrameName(), sizeof(sImageInfo.szFrameName) -1);
			//GetImageTimeStamp(pImage, sImageInfo.sTimeStamp);
			pImage->GetImage(&sImageInfo.sComponent);			

			/*save image data size */
			DWORD dwImageDataSize = sizeof(sImageInfo) + pImage->GetSize();
			swpa_memcpy(pbAddr, &dwImageDataSize, sizeof(dwImageDataSize));
			pbAddr += sizeof(dwImageDataSize);

			//PRINT("dwImageDataSize=%lu\n", dwImageDataSize);

			/*save image info */
			swpa_memcpy(pbAddr, &sImageInfo, sizeof(sImageInfo));
			pbAddr += sizeof(sImageInfo);
			

			/*save image data */
			swpa_memcpy(pbAddr, sImageInfo.sComponent.rgpbData[0], pImage->GetSize());
			pbAddr += pImage->GetSize();
		}
	}


	//update Data crc
	dwDataCrc = CSWUtils::CalcCrc32(0, 
		pbBuf+sizeof(dwSizeCrc)+sizeof(iBufSize)+sizeof(dwDataCrc), 
		iBufSize-sizeof(dwSizeCrc)-sizeof(iBufSize)-sizeof(dwDataCrc));
	swpa_memcpy(pbBuf+sizeof(dwSizeCrc)+sizeof(iBufSize), &dwDataCrc, sizeof(dwDataCrc));
	
				
	CSWDateTime FileTime(pRecord->GetRefTime());

	//save the image data
	INT iTryCount = 3;
	do 
	{
		hr = m_pBigFile->Write(FileTime.GetYear(), FileTime.GetMonth(), FileTime.GetDay(),
			FileTime.GetHour(), FileTime.GetMinute(), FileTime.GetSecond(),
			pbBuf, iBufSize, pRecord->IsNormal());
		if (S_OK != hr)
		{
			PRINT("Warning: failed to save record file (time: %d/%d/%d/%d:%d:%d)[0x%x], trying another %d time(s)\n", 
				FileTime.GetYear(), FileTime.GetMonth(), FileTime.GetDay(),
				FileTime.GetHour(), FileTime.GetMinute(), FileTime.GetSecond(), hr, iTryCount);
			swpa_thread_sleep_ms(100);
			iTryCount--;
		}

		if (0 >= iTryCount)
		{
			PRINT("Err: failed to save record file (time: %d/%d/%d/%d:%d:%d)\n", 
				FileTime.GetYear(), FileTime.GetMonth(), FileTime.GetDay(),
				FileTime.GetHour(), FileTime.GetMinute(), FileTime.GetSecond());
		}
	}
	while (FAILED(hr) && 0 < iTryCount);

	CSWHDDTransformFilter::UpdateCapacity(iBufSize);

	m_pcMemoryFactory->Free(pcMemBuf);
	pcMemBuf = NULL;
	pbBuf = NULL;//SAFE_MEM_FREE(pbBuf);

	return hr;
}




HRESULT CSWResultHDDTransformFilter::OnHistoryFileSaving(VOID)
{
	if (!m_fInited)
	{
		PRINT("Err: not initialized yet\n");
		return E_NOTIMPL;
	}

	DWORD dwPrevTick = CSWDateTime::GetSystemTick();
	
	while (FILTER_RUNNING == GetState())
	{
		m_dwLastTime = CSWDateTime::GetSystemTick();

		if(m_fHistoryFileSaveDisabled)
		{
			swpa_thread_sleep_ms(100);
			continue;
		}
		DWORD dwTick = CSWDateTime::GetSystemTick();
		if (10000 < dwTick - dwPrevTick)
		{
			ReportStatus();
			
			dwPrevTick = dwTick;
		}
		
		if (FAILED(m_pSemaSaveQueueSync->Pend(200)))
		{
			//PRINT("Info: no Record obj in History-file saving queue!\n");
			continue;
		}

		m_pFileMutex->Lock();
		DWORD dwFileCount = m_lstFile.GetCount();
		m_pFileMutex->Unlock();

		PRINT("Info: History Record Queue Element Count: %lu\n", dwFileCount);		

		while (dwFileCount --)
		{
			m_dwLastTime = CSWDateTime::GetSystemTick();
			
			m_pFileMutex->Lock();
			CSWRecord * pRecord = m_lstFile.RemoveHead();
			m_pFileMutex->Unlock();

			
			INT iTryCount = 3;
			while (FAILED(SaveRecord(pRecord)))
			{
				m_dwLastTime = CSWDateTime::GetSystemTick();
				iTryCount --;
				
				if (0 == iTryCount)
				{
					PRINT("Err: failed to save record %p, skip this!\n", pRecord);
					//CSWApplication::Exit(2);
					ReportStatus("异常");
					//m_fHistoryFileSaveDisabled = TRUE;
					//ClearBuffer();	
					CSWMessage::SendMessage(MSG_APP_REINIT_HDD, 0, 0);
					break;
				}
				else
				{
					PRINT("Warning: failed to save record %p, trying another %d time(s)!\n", pRecord, iTryCount);
					swpa_thread_sleep_ms(100);
				}
			}


			SAFE_RELEASE(pRecord);
		}

	}

	return S_OK;
}




HRESULT CSWResultHDDTransformFilter::OnHistoryFileTransmitting(const DWORD dwPinID)
{
	HRESULT hr = S_OK;	
	PBYTE pbBuf = NULL;

	if (!m_fInited)
	{
		PRINT("Err: not initialized yet\n");
		return E_OBJ_NO_INIT;
	}

	if (FILTER_RUNNING == GetState() && m_fHistoryTransmitting[dwPinID])
	{
		m_dwHistoryTransmittingStatus[dwPinID] = HISTORY_TRANSMITTING_ONGOING;

		CSWDateTime myBeginTime(&m_tHistoryBeginTime[dwPinID]);
		CSWDateTime myEndTime(&m_tHistoryEndTime[dwPinID]);
		CSWDateTime myTime(&m_tHistoryBeginTime[dwPinID]);
		

		DWORD dwCarID = m_dwCarID[dwPinID];


		DWORD dwCurTick = CSWDateTime::GetSystemTick();
		DWORD dwPrevTick = dwCurTick;
		
		DWORD dwFailCount = 0;
		while (FILTER_RUNNING == GetState() 
			&& m_fHistoryTransmitting[dwPinID]
			&& !(myTime > myEndTime))
		{
			INT iBufSize = 0;

			PRINT("dwPinID = %lu\n", dwPinID);
			
			m_pFileMutex->Lock();
			DWORD dwFileCount = m_lstFile.GetCount();
			m_pFileMutex->Unlock();
	
			if (1 < dwFileCount)
			{
				PRINT("Info: reading is stuck due to file queue count > 1\n");
				CSWApplication::Sleep(200);
				continue;
			}

			if (!CSWHDDTransformFilter::CheckCurrentCapacity())
			{	
				PRINT("Info: reading is stuck due to capacity > 4MB\n");
				CSWApplication::Sleep(500);
				continue;
			}
			
			if (E_RECORD_NONE == (m_pBigFile->GetNext(myTime.GetYear(), myTime.GetMonth(), myTime.GetDay(), 
				myTime.GetHour(), myTime.GetMinute(), myTime.GetSecond(), GetOutputType(), &dwCarID)))
			{
				CSWDateTime tCurTime;
				if (tCurTime.GetYear() <= myTime.GetYear()
					&& tCurTime.GetMonth() <= myTime.GetMonth()
					&& tCurTime.GetDay() <= myTime.GetDay()
					&& tCurTime.GetHour() <= myTime.GetHour())
				{
					swpa_thread_sleep_ms(1000);
				}
				else
				{				
					PRINT("Err:  no record #%lu in %04d/%02d/%02d/%02d, shift to next hour\n", 
						dwCarID, myTime.GetYear(), myTime.GetMonth(), myTime.GetDay(), myTime.GetHour());
					SWPA_TIME tIntervalTime;
					tIntervalTime.sec = 1*60*60;
					tIntervalTime.msec = 0;
					myTime += tIntervalTime;

					dwCarID = 0;
				}
				
				continue;
			}
			
			
			PRINT("Info: going to read record file (time: %04d/%02d/%02d/%02d:%02d:%02d), CarID=%lu\n", 
				myTime.GetYear(), myTime.GetMonth(), myTime.GetDay(),
				myTime.GetHour(), myTime.GetMinute(), myTime.GetSecond(), dwCarID);

			hr = m_pBigFile->GetSize(myTime.GetYear(), myTime.GetMonth(), myTime.GetDay(), 
				myTime.GetHour(), myTime.GetMinute(), myTime.GetSecond(), dwCarID, &iBufSize);
			
			if (E_RECORD_NONE == hr)
			{
				CSWDateTime tCurTime;
				if (tCurTime.GetYear() <= myTime.GetYear()
					&& tCurTime.GetMonth() <= myTime.GetMonth()
					&& tCurTime.GetDay() <= myTime.GetDay()
					&& tCurTime.GetHour() <= myTime.GetHour())
				{
					swpa_thread_sleep_ms(1000);
				}
				else
				{				
					PRINT("Err:  no record #%lu in %04d/%02d/%02d/%02d, shift to next hour\n", 
						dwCarID, myTime.GetYear(), myTime.GetMonth(), myTime.GetDay(), myTime.GetHour());
					SWPA_TIME tIntervalTime;
					tIntervalTime.sec = 1*60*60;
					tIntervalTime.msec = 0;
					myTime += tIntervalTime;

					dwCarID = 0;
				}
				
				continue;
			}
			else if (E_FAIL == hr || 0 >= iBufSize)
			{
				if (dwFailCount++<3)
				{
					PRINT("Err: failed to read record #%lu in %04d/%02d/%02d/%02d, trying again...\n", 
						dwCarID, myTime.GetYear(), myTime.GetMonth(), myTime.GetDay(), myTime.GetHour());
					swpa_thread_sleep_ms(1000);
					continue;
				}
				else
				{
					PRINT("Err: failed to read record #%lu in %04d/%02d/%02d/%02d, skip it\n", 
						dwCarID, myTime.GetYear(), myTime.GetMonth(), myTime.GetDay(), myTime.GetHour());
					dwCarID++;
					dwFailCount = 0;
					continue;
				}				
			}
			else if (E_RECORD_WRITING == hr)
			{
				swpa_thread_sleep_ms(1000);
				continue;
			}
			
			PRINT("iBufSize = %d KB\n", iBufSize/1024);

			CSWMemory* pcMemBuf = m_pcMemoryFactory->Alloc(ALGIN_SIZE(iBufSize, 128*1024));//alignment, reduce memory fragment
			if (NULL == pcMemBuf)
			{
				PRINT("Err: no memory for pcMemBuf\n");
				return E_OUTOFMEMORY;
			}
			

			pbBuf = (PBYTE)pcMemBuf->GetBuffer();
			//pbBuf = (PBYTE)swpa_mem_alloc(iBufSize);
			if (NULL == pbBuf)
			{
				PRINT("Err: no enough memory for pvBuf (needs %dB)\n", iBufSize);
				hr = E_OUTOFMEMORY;
				goto Out;
			}
			

			hr = m_pBigFile->Read(myTime.GetYear(), myTime.GetMonth(), myTime.GetDay(), 
				myTime.GetHour(), myTime.GetMinute(), myTime.GetSecond(), dwCarID,  pbBuf,  iBufSize);
			if (S_OK != hr)
			{
				PRINT("Err: failed to read file [0x%x]\n", hr);
				hr = E_FAIL;
				m_pcMemoryFactory->Free(pcMemBuf);
				pcMemBuf = NULL;
				pbBuf = NULL;//SAFE_MEM_FREE(pbBuf);
				goto Out;
			}

			CSWHDDTransformFilter::UpdateCapacity(iBufSize);

			CSWRecord * pRecord = CreateRecord(pbBuf, iBufSize);
			if (NULL == pRecord)
			{
				PRINT("Err: failed to create Record\n");
				hr = E_FAIL;
				m_pcMemoryFactory->Free(pcMemBuf);
				pcMemBuf = NULL;
				pbBuf = NULL;//SAFE_MEM_FREE(pbBuf);
				goto Out;
			}
			

			m_pcMemoryFactory->Free(pcMemBuf);
			pcMemBuf = NULL;
			pbBuf = NULL;//SAFE_MEM_FREE(pbBuf);

			if (!pRecord->GetTrafficInfoFlag())
			{
				if (FAILED(pRecord->SetTransmittingType("History")))
				{
					PRINT("Err: failed to set TransmittingType to \"History\"\n");
				}
				

				if (FAILED(pRecord->SetCarID(dwCarID)))
				{
					PRINT("Err: failed to set CarID to %lu\n", dwCarID);
				}	
			}
			

			//send the Record
			do
			{
				if (0 < GetSendInterval())
				{
					/*
					dwCurTick = CSWDateTime::GetSystemTick();
					if (dwCurTick - dwPrevTick < GetSendInterval())
					{
						CSWApplication::Sleep(GetSendInterval() - dwCurTick + dwPrevTick);
					}
					dwPrevTick = CSWDateTime::GetSystemTick();
					*/
					//PRINT("Info: GetSendInterval() ret %d\n", GetSendInterval());
					CSWApplication::Sleep(GetSendInterval());
				}
				else
				{
					CSWApplication::Sleep(1000); //avoid 
				}
				
			}while (m_fHistoryTransmitting[dwPinID]	&& (FAILED(GetOut(dwPinID)->Deliver(pRecord))));

			SAFE_RELEASE(pRecord);			

			dwCarID++;
			
		}

		if (myTime > myEndTime)
		{
			//send the End-Flag Record
			CSWRecord* pEndFlagRecord = new CSWRecord();
			if (NULL == pEndFlagRecord) /* no 'Initialize()' op here, make it a non-valid record*/
			{
				PRINT("Err: no enough memory for pEndFlagRecord, won't send Record-End-Flag\n");
			}
			else
			{
				do
				{
					if (0 < GetSendInterval())
					{
						/*
						dwCurTick = CSWDateTime::GetSystemTick();
						if (dwCurTick - dwPrevTick < GetSendInterval())
						{
							CSWApplication::Sleep(GetSendInterval() - dwCurTick + dwPrevTick);
						}
						dwPrevTick = CSWDateTime::GetSystemTick();
						*/
						CSWApplication::Sleep(GetSendInterval());
					}
					else
					{
						CSWApplication::Sleep(1000); //avoid 
					}
					
				} while (m_fHistoryTransmitting[dwPinID] && (FAILED(GetOut(dwPinID)->Deliver(pEndFlagRecord))));
			}
			SAFE_RELEASE(pEndFlagRecord);
		}
	}
	
Out:

	m_dwHistoryTransmittingStatus[dwPinID] = HISTORY_TRANSMITTING_FINISHED;
	
	PRINT("Info: %s() exited...\n", __FUNCTION__);
	
	return hr;
	
}




BOOL CSWResultHDDTransformFilter::RecordIsTrafficInfo(CSWRecord * pRecord)
{
	if (!m_fInited)
	{
		PRINT("Err: not initialized yet\n");
		return FALSE;
	}
	
	if (NULL == pRecord || NULL == pRecord->GetXmlString())
	{
		PRINT("Err: NULL == pRecord || NULL == pRecord->GetXmlString()\n");
		return FALSE;
	}

	//PRINT("Info: pEle->GetXmlString() = %s\n", pRecord->GetXmlString());
	//todo: use stored flag to judge
	if (NULL != swpa_strstr(pRecord->GetXmlString(), "TrafficInfo")
		//&& NULL == swpa_strstr(pRecord->GetXmlString(), "ResultSet")
		)
	{
		PRINT("Info: RecordIsTrafficInfo ret TRUE\n");
		return TRUE;
	}
	/*

	TiXmlDocument xmlDoc;
	if (FAILED(xmlDoc.Parse(pRecord->GetXmlString())) 
		|| NULL == xmlDoc.RootElement())
	{
		PRINT("Err: Failed to parse pRecord XML\n");
		return E_FAIL;
	}

	CHAR szText[32] = {0};

	TiXmlElement* pEleRoot = xmlDoc.RootElement();
	for (TiXmlElement* pEle = pEleRoot->FirstChildElement(); NULL != pEle; pEle = pEle->NextSiblingElement())
	{
		if (NULL != pEle->Value()
			&& 0 == swpa_strcmp("TrafficInfo", pEle->Value()))
		{
			return TRUE;
		}
	}
*/
	return FALSE;
}




CSWRecord* CSWResultHDDTransformFilter::CreateRecord(const PBYTE pbBuf, const INT iSize)
{
	if (!m_fInited)
	{
		PRINT("Err: not initialized yet\n");
		return NULL;
	}
	
	if (NULL == pbBuf || 0 >= iSize)
	{
		PRINT("Err: NULL == pbBuf || 0 >= iSize\n");
		return NULL;
	}
	
	PBYTE pbAddr = (PBYTE)pbBuf;
	DWORD dwSizeCrcStored = 0, dwDataCrcStored = 0;
	INT iBufSize = 0;

	swpa_memcpy(&dwSizeCrcStored, pbAddr, sizeof(dwSizeCrcStored));
	pbAddr += sizeof(dwSizeCrcStored);
	swpa_memcpy(&iBufSize, pbAddr, sizeof(iBufSize));
	pbAddr += sizeof(iBufSize);
	swpa_memcpy(&dwDataCrcStored, pbAddr, sizeof(dwDataCrcStored));
	pbAddr += sizeof(dwDataCrcStored);


	if (dwSizeCrcStored != CSWUtils::CalcCrc32(0, (BYTE*)&iBufSize, sizeof(iBufSize)))
	{
		SW_TRACE_NORMAL("Err: Record Size check failed!\n");
		return NULL;
	}

	if (dwDataCrcStored != CSWUtils::CalcCrc32(0, pbAddr, iBufSize-sizeof(dwSizeCrcStored)-sizeof(iBufSize)-sizeof(dwDataCrcStored)))
	{
		SW_TRACE_NORMAL("Err: Record Data check failed!\n");
		return NULL;
	}
		
	CSWRecord * pRecord = new CSWRecord();
	if (NULL == pRecord || FAILED(pRecord->Initialize()))
	{
		PRINT("Err: no enough memory for pRecord\n");
		SAFE_RELEASE(pRecord);
		return NULL;
	}

	DWORD dwInfoSize = 0;
	swpa_memcpy(&dwInfoSize, pbAddr, sizeof(dwInfoSize));
	pbAddr += sizeof(dwInfoSize);	

	if (FAILED(pRecord->SetXml((const CHAR*)pbAddr)))
	{
		PRINT("Err: failed to set xml\n");
		SAFE_RELEASE(pRecord);		
		return NULL;
	}

	pbAddr += dwInfoSize;

	pRecord->SetTrafficInfoFlag(RecordIsTrafficInfo(pRecord));
				
	DWORD dwImageCount = 0;
	swpa_memcpy(&dwImageCount, pbAddr, sizeof(dwImageCount));
	pbAddr += sizeof(dwImageCount);

	for (DWORD i=0; i<dwImageCount; i++)
	{
		DWORD dwImageID = 0;

		swpa_memcpy(&dwImageID, pbAddr, sizeof(dwImageID));
		pbAddr += sizeof(dwImageID);
		
		DWORD dwImageDataSize = 0;
		swpa_memcpy(&dwImageDataSize, pbAddr, sizeof(dwImageDataSize));
		pbAddr += sizeof(dwImageDataSize);
		
		CSWImage * pNewImage = CreateImageFromBuffer(pbAddr, dwImageDataSize);
		if (NULL == pNewImage)
		{
			PRINT("Err: failed to create image\n");
			SAFE_RELEASE(pRecord);
			return NULL;
		}
		pbAddr += dwImageDataSize;

		pRecord->SetImage(dwImageID, pNewImage);

		SAFE_RELEASE(pNewImage);
	}

	return pRecord;
}




HRESULT CSWResultHDDTransformFilter::OnSetTransmittingType(WPARAM wParam, LPARAM lParam)
{
	DWORD dwPinID = (DWORD)wParam;
	DWORD dwType = (DWORD)lParam;

	return CSWHDDTransformFilter::SetTransmittingType(dwPinID,dwType);
}



HRESULT CSWResultHDDTransformFilter::OnStartHistoryFileTransmitting(WPARAM wParam, LPARAM lParam)
{
	DWORD dwPinID = (DWORD)wParam;

	if (GetOutCount() <= dwPinID)
	{
		PRINT("Err: dwPinID = %u is invalid\n", dwPinID);
		return E_INVALIDARG;
	}

	HISTORY_FILE_TRANSMITTING_PARAM* pParam = (HISTORY_FILE_TRANSMITTING_PARAM*)lParam;
	if (NULL == pParam)
	{
		PRINT("Err: NULL == pParam\n");
		return E_INVALIDARG;
	}

	return CSWHDDTransformFilter::StartHistoryFileTransmitting(dwPinID, pParam->psBeginTime, pParam->psEndTime, pParam->dwCarID);
	
}






HRESULT CSWResultHDDTransformFilter::OnStopHistoryFileTransmitting(WPARAM wParam, LPARAM lParam)
{
	DWORD dwPinID = (DWORD)wParam;

	if (GetOutCount() <= dwPinID)
	{
		PRINT("Err: dwPinID = %u is invalid\n", dwPinID);
		return E_INVALIDARG;
	}

	if (TRANSMITTING_HISTORY != m_dwOutType[dwPinID])
	{
		PRINT("Err: Out pin (%u) wasn't configed to output history file\n", dwPinID);
		return E_INVALIDARG;
	}

	return CSWHDDTransformFilter::StopHistoryFileTransmitting(dwPinID);
}



HRESULT CSWResultHDDTransformFilter::OnStartRealtimeTransmitting(WPARAM wParam, LPARAM lParam)
{
	DWORD dwPinID = (DWORD)wParam;

	if (GetOutCount() <= dwPinID)
	{
		PRINT("Err: dwPinID = %u is invalid\n", dwPinID);
		return E_INVALIDARG;
	}

	return CSWHDDTransformFilter::StartRealtimeTransmitting(dwPinID);
}




HRESULT CSWResultHDDTransformFilter::OnStopRealtimeTransmitting(WPARAM wParam, LPARAM lParam)
{
	DWORD dwPinID = (DWORD)wParam;

	if (GetOutCount() <= dwPinID)
	{
		PRINT("Err: dwPinID = %u is invalid\n", dwPinID);
		return E_INVALIDARG;
	}

	if (TRANSMITTING_REALTIME != m_dwOutType[dwPinID])
	{
		PRINT("Err: Out pin (%u) wasn't configed to output realtime data\n", dwPinID);
		return E_INVALIDARG;
	}

	return CSWHDDTransformFilter::StopRealtimeTransmitting(dwPinID);
}




HRESULT CSWResultHDDTransformFilter::OnGetHistoryFileTransmittingStatus(WPARAM wParam, LPARAM lParam)
{
	DWORD dwPinID = (DWORD)wParam;
	DWORD* pdwStatus = (DWORD*)lParam;

	return CSWHDDTransformFilter::GetHistoryFileTransmittingStatus(dwPinID, pdwStatus);
}







