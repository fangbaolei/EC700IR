// 该文件编码格式必须是WIN936
/**
* @file SWH264HDDTransformFilter.cpp
* @brief Implementation of CSWH264HDDTransformFilter
* 
* @copyright Signalway All Rights Reserved
* @author zhouy
* @date 2013-03-22
* @version 1.0
*/
	
#include "SWFC.h"
#include "SWH264HDDTransformFilter.h"
#include "SWTransmittingOpt.h"


#define PRINT SW_TRACE_DEBUG



typedef struct __HISTORY_TRANSIMITTING_THREAD_ARG
{
	CSWH264HDDTransformFilter * pThis;
	DWORD dwPinID;
}_HISTORY_TRANSIMITTING_THREAD_ARG;




CSWH264HDDTransformFilter::CSWH264HDDTransformFilter()
  :CSWHDDTransformFilter(H264HDD_TRANSFORM_IN_COUNT, H264HDD_TRANSFORM_OUT_COUNT)
  ,CSWMessage(MSG_H264HDD_FILTER_CTRL_START, MSG_H264HDD_FILTER_CTRL_END)
  ,m_fInited(FALSE)
  ,m_pFileLock(NULL)
  ,m_dwSaveQueueSize(20)
  ,m_pSemaSaveQueueSync(NULL)
  ,m_iSendInterval(0)
  ,m_pcMemoryFactory(NULL)
  ,m_iSaveVideoType(0)
  ,m_pcTickLock(NULL)
  ,m_fGBVideoSaveRecord(FALSE)
  ,m_iSendPauseTime(0)
//  ,m_VideoRecordList(NULL)
  ,m_iIsGB28181Saveing(0)
  ,m_iRecordSendInterval(-1)
  ,m_iGBVideoSkipBlock(0)
  ,m_fGB28181Backward(FALSE)
{
	GetIn(0)->AddObject(CLASSID(CSWImage));
	GetOut(0)->AddObject(CLASSID(CSWImage));
	GetOut(1)->AddObject(CLASSID(CSWImage));
	GetOut(2)->AddObject(CLASSID(CSWImage));
	GetOut(3)->AddObject(CLASSID(CSWImage));
	
//	m_VideoRecordList = new CSWFile();
	
}


CSWH264HDDTransformFilter::~CSWH264HDDTransformFilter()
{
	if (!m_fInited)
	{
		return ;
	}

	if (!m_fHistoryFileSaveDisabled)
	{
		//free the frame list
		while (!m_lstFrame.IsEmpty())
		{
			CSWImage * pImage = m_lstFrame.RemoveHead();
			SAFE_RELEASE(pImage);
		}
		
		//free the file list
		//if (NULL != m_pFileLock) m_pFileLock->Lock();
		if (NULL != m_pFileLock) m_pFileLock->Pend();
		while (!m_lstFile.IsEmpty())
		{
			CSWMemory * pcFile = m_lstFile.RemoveHead();
			
			m_pcMemoryFactory->Free(pcFile);//swpa_mem_free(pbFile);
			pcFile = NULL;
		}	
		//if (NULL != m_pFileLock) m_pFileLock->Unlock();
		if (NULL != m_pFileLock) m_pFileLock->Post();

		SAFE_RELEASE(m_pFileLock);

		SAFE_RELEASE(m_pcTickLock);

		SAFE_RELEASE(m_pSemaSaveQueueSync);
	}

	m_pcMemoryFactory = NULL;
	
	m_fInited = FALSE;
	
}


HRESULT CSWH264HDDTransformFilter::Initialize(const CHAR * szDirecory, const LONGLONG llTotalSize, const INT iFileSize, const INT iSaveVideoType)
{
	if (m_fInited)
	{
		//return S_OK;
	}

	SW_TRACE_DEBUG("Info: szDirecory = %s, llTotalSize = %lld, iFileSize = %d， iSaveVideoType = %d\n",
		szDirecory, llTotalSize, iFileSize, iSaveVideoType);

	m_iSaveVideoType = iSaveVideoType;
	
	if (FAILED(CSWHDDTransformFilter::Initialize(szDirecory, llTotalSize, iFileSize, FALSE)))
	{
		PRINT("Err: failed to initialize CSWH264HDDTransformFilter!\n");
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
		if (NULL == m_pFileLock)
		{
			//m_pFileLock = new CSWMutex();
			m_pFileLock = new CSWSemaphore(1, 1);
			if (NULL == m_pFileLock)
			{
				PRINT("Err: no enough memory for m_pFileLock \n");
				return E_OUTOFMEMORY;
			}
		}

		if (NULL == m_pcTickLock)
		{
			m_pcTickLock = new CSWSemaphore(1, 1);
			if (NULL == m_pcTickLock)
			{
				PRINT("Err: no enough memory for m_pcTickLock \n");
				return E_OUTOFMEMORY;
			}
		}


		if (NULL == m_pSemaSaveQueueSync)
		{
			m_pSemaSaveQueueSync = new CSWSemaphore(0, m_dwSaveQueueSize);
			if (NULL == m_pSemaSaveQueueSync)
			{
				PRINT("Err: no enough memory for m_pSemaSaveQueueSync \n");
				return E_OUTOFMEMORY;
			}
		}

		m_lstFile.RemoveAll();
		m_lstFile.SetMaxCount(m_dwSaveQueueSize);
		
		m_lstFrame.RemoveAll();
	}
	
	m_fInited = TRUE;

	return S_OK;
}





HRESULT CSWH264HDDTransformFilter::FrameEnqueue(CSWImage* pImage)
{
	if (!m_fInited)
	{
		PRINT("Err: not initialized yet\n");
		return E_NOTIMPL;
	}
	
	if (NULL == pImage)
	{
		PRINT("Err: (NULL == pImage!\n");
		return E_INVALIDARG;
	}

	SAFE_ADDREF(pImage);
	m_lstFrame.AddTail(pImage);


	return S_OK;
}

HRESULT CSWH264HDDTransformFilter::FileEnqueue(const SWPA_TIME& sTimeStamp)
{
	if (!m_fInited)
	{
		PRINT("Err: not initialized yet\n");
		return E_NOTIMPL;
	}

	if (m_lstFrame.IsEmpty())
	{
		PRINT("Err: Frame List is empty\n");
		return E_INVALIDARG;
	}
	
	INT iBufSize = 0;
	_IMAGE_FILE_INFO sImageInfo;
	DWORD dwSizeCrc = 0, dwDataCrc = 0;
	
	iBufSize += sizeof(iBufSize);
	iBufSize += sizeof(dwSizeCrc);
	iBufSize += sizeof(dwDataCrc);
	iBufSize += sizeof(sImageInfo);
	iBufSize += (sizeof(INT/*FrameType*/) + sizeof(INT/*FrameSize*/)) * m_lstFrame.GetCount();


	SW_POSITION pos = m_lstFrame.GetHeadPosition();
	while (m_lstFrame.IsValid(pos))
	{
		CSWImage * pImage = m_lstFrame.GetNext(pos);
		if (NULL != pImage)
		{
			iBufSize += pImage->GetSize();
		}
	}

	//SW_TRACE_DEBUG("Info: iBufSize = %d\n", iBufSize);
	CSWMemory* pcMemBuf = m_pcMemoryFactory->Alloc(ALGIN_SIZE(iBufSize, 128*1024));//alignment, reduce memory fragment
	if (NULL == pcMemBuf)
	{
		PRINT("Err: no memory for pcMemBuf\n");
		while (!m_lstFrame.IsEmpty())
		{
			CSWImage* pImage = m_lstFrame.RemoveHead();	
			SAFE_RELEASE(pImage);
		}
		return E_OUTOFMEMORY;
	}
	
	PBYTE pbBuf = (PBYTE)pcMemBuf->GetBuffer();//BYTE * pbBuf = (BYTE *)swpa_mem_alloc(iSize);	
	if (NULL == pbBuf)
	{
		PRINT("Err: No enough memory for pbBuf!\n");
		while (!m_lstFrame.IsEmpty())
		{
			CSWImage* pImage = m_lstFrame.RemoveHead();	
			SAFE_RELEASE(pImage);
		}
		return E_OUTOFMEMORY;
	}
	swpa_memset(pbBuf, 0, iBufSize);


	BYTE * pbAddr = pbBuf;

	/* save size crc */
	dwSizeCrc = CSWUtils::CalcCrc32(0, (BYTE *)&iBufSize, sizeof(iBufSize));
	swpa_memcpy(pbAddr, &dwSizeCrc, sizeof(dwSizeCrc));
	pbAddr += sizeof(dwSizeCrc);

	/* save size */
	swpa_memcpy(pbAddr, &iBufSize, sizeof(iBufSize));
	pbAddr += sizeof(iBufSize);

	/* save data crc */
	swpa_memcpy(pbAddr, &dwDataCrc, sizeof(dwDataCrc)); //will update this later in the func
	pbAddr += sizeof(dwDataCrc);

	CSWImage * pImage = m_lstFrame.RemoveHead();
	while (NULL == pImage)
	{
		if (m_lstFrame.IsEmpty())
		{
			PRINT("Err: No valid Frame data in FrameQueue!\n");
			
			m_pcMemoryFactory->Free(pcMemBuf);
			pcMemBuf = NULL;
			pbBuf = NULL;//SAFE_MEM_FREE(pbBuf);
			return E_FAIL;
		}
		pImage = m_lstFrame.RemoveHead();
	}
	

	SW_COMPONENT_IMAGE sComponent;
	pImage->GetImage(&sComponent);
	sComponent.iSize = iBufSize - sizeof(iBufSize) - sizeof(sImageInfo) - sizeof(dwSizeCrc) - sizeof(dwDataCrc);
	
	swpa_memset(&sImageInfo, 0, sizeof(_IMAGE_FILE_INFO));
	
	sImageInfo.dwFrameNo = pImage->GetFrameNo();
	sImageInfo.dwRefTimeMS = pImage->GetRefTime();
	sImageInfo.fIsCaptureImage = pImage->IsCaptureImage();
	swpa_strncpy(sImageInfo.szFrameName, pImage->GetFrameName(), sizeof(sImageInfo.szFrameName)-1);		
	sImageInfo.sComponent = sComponent;
	sImageInfo.sTimeStamp = sTimeStamp;
	
	CSWDateTime FileTime(sImageInfo.sTimeStamp);
	//PRINT("Info: enqueuing h264 video file (time %u.%u)\n", sImageInfo.sTimeStamp.sec, sImageInfo.sTimeStamp.msec);
	PRINT("Info: enqueuing h264 video file (time: %d/%d/%d/%d:%d:%d)\n", 
			FileTime.GetYear(), FileTime.GetMonth(), FileTime.GetDay(),
			FileTime.GetHour(), FileTime.GetMinute(), FileTime.GetSecond());

	swpa_memcpy(pbAddr, &sImageInfo, sizeof(sImageInfo));
	pbAddr += sizeof(sImageInfo);
	

	INT iFrameType = 4096;//1st HISTORY I-Frame
	INT iFrameSize = pImage->GetSize();

	swpa_memcpy(pbAddr, &iFrameType, sizeof(iFrameType));
	pbAddr += sizeof(iFrameType);


	swpa_memcpy(pbAddr, &iFrameSize, sizeof(iFrameSize));
	pbAddr += sizeof(iFrameSize);
	
	swpa_memcpy(pbAddr, sComponent.rgpbData[0], iFrameSize);
	pbAddr += iFrameSize;


	SAFE_RELEASE(pImage);
	

	while (!m_lstFrame.IsEmpty())
	{
		pImage = m_lstFrame.RemoveHead();

		if (NULL == pImage)
		{
			continue;
		}

		iFrameType = (0 == swpa_strcmp(pImage->GetFrameName(), "IFrame")) ? 4096 : 4097;//'4097' means "HISTORY P-FRAME"

		iFrameSize = pImage->GetSize();
		
		swpa_memcpy(pbAddr, &iFrameType, sizeof(iFrameType));
		pbAddr += sizeof(iFrameType);
		
		swpa_memcpy(pbAddr, &iFrameSize, sizeof(iFrameSize));
		pbAddr += sizeof(iFrameSize);		

		pImage->GetImage(&sComponent);

		swpa_memcpy(pbAddr, sComponent.rgpbData[0], iFrameSize);
		pbAddr += iFrameSize;
		

		SAFE_RELEASE(pImage);
	}


	/* update data crc here */
	dwDataCrc = CSWUtils::CalcCrc32(0, 
		pbBuf+sizeof(dwSizeCrc)+sizeof(iBufSize)+sizeof(dwDataCrc),
		iBufSize-sizeof(dwSizeCrc)-sizeof(iBufSize)-sizeof(dwDataCrc));
	swpa_memcpy(pbBuf+sizeof(dwSizeCrc)+sizeof(iBufSize), &dwDataCrc, sizeof(dwDataCrc));
	

	//SW_TRACE_DEBUG("semainfo: to pend\n");
	//m_pFileLock->Lock();
	m_pFileLock->Pend();
	//SW_TRACE_DEBUG("semainfo: pended\n");
	if (m_lstFile.IsFull())
	{
		if (1 == m_iSaveVideoType) //only save on pecancy
		{
			SW_TRACE_DEBUG("Warning: Save Queue is full! discards the earlist data\n");

			CSWMemory* pVideoData = m_lstFile.RemoveHead();
			m_pcMemoryFactory->Free(pVideoData);
    		pVideoData = NULL;

			m_lstFile.AddTail(pcMemBuf);
			m_pSemaSaveQueueSync->Post();
		}
		else
		{
    		SW_TRACE_DEBUG("Warning: Save Queue is full! discards this H264 data\n");
    
    		m_pcMemoryFactory->Free(pcMemBuf);
    		pcMemBuf = NULL;
		}
		//m_pFileLock->Unlock();
		//return S_OK;
	}
	else
	{
		m_lstFile.AddTail(pcMemBuf);
		m_pSemaSaveQueueSync->Post();
	}
	
	//m_pFileLock->Unlock();
	m_pFileLock->Post();
	
	return S_OK;
}


HRESULT CSWH264HDDTransformFilter::Receive(CSWObject * obj)
{
	static BOOL fFirstIFrameFound = FALSE, fSecondIFrameFound = FALSE;;
	static SWPA_TIME sFirstIFTime, sSecondIFTime ;
	static DWORD dwFrameCount = 0;
	
	if (!m_fInited)
	{
		PRINT("Err: not initialized yet\n");
		return E_NOTIMPL;
	}

	if (IsDecendant(CSWImage, obj))
	{
		//PRINT("Info: received an image\n");
		
		//send real time image immediately
		for (INT i=0; i<GetOutCount(); i++)
		{
			if (TRANSMITTING_REALTIME == m_dwOutType[i])
			{
				GetOut(i)->Deliver(obj);
			}
		}
		static INT iGBEnableTick = 0;
		if( iGBEnableTick > 500 )
		{
			SW_TRACE_DEBUG("m_fGBVideoSaveCtlEnable:%d,m_fGBVideoSaveRecord:%d\n",m_fGBVideoSaveCtlEnable,m_fGBVideoSaveRecord);
			iGBEnableTick=0;
		}
		iGBEnableTick++;

		if (!m_fHistoryFileSaveDisabled && GetSaveHistoryFlag() 
			&& (!m_fGBVideoSaveCtlEnable || (m_fGBVideoSaveCtlEnable && m_fGBVideoSaveRecord) ) 
			)
		{
			//buffer the frame for future saving
			SAFE_ADDREF(obj);

			CSWImage* pImage = (CSWImage *)obj;

			dwFrameCount++;
			
			if (fFirstIFrameFound)
			{
				if (0 == swpa_strcmp(pImage->GetFrameName(), "IFrame"))
				{
					GetImageTimeStamp(pImage, &sSecondIFTime);
					if (sSecondIFTime.sec != sFirstIFTime.sec)
					{
						FileEnqueue(sFirstIFTime);

						if (1 < sSecondIFTime.sec - sFirstIFTime.sec)
						{
							PRINT("Info: sSecondIFTime - sFirstIFTime = %d\n", sSecondIFTime.sec - sFirstIFTime.sec);
						}
						
						if (25 < dwFrameCount)
						{
							PRINT("Info: dwFrameCount = %d\n", dwFrameCount);
						}

						dwFrameCount = 0;
						fFirstIFrameFound = TRUE;
						sFirstIFTime = sSecondIFTime;
					}
				}
				
				FrameEnqueue(pImage);
			}
			else
			{
				if (0 == swpa_strcmp(pImage->GetFrameName(), "IFrame"))
				{
					fFirstIFrameFound = TRUE;
					GetImageTimeStamp(pImage, &sFirstIFTime);

					FrameEnqueue(pImage);
				}
				else
				{
					/* do nothing */
				}
			}

			SAFE_RELEASE(obj);
			
		}
		else
		{
			if( m_iIsGB28181Saveing == 1 )
			{
				m_iIsGB28181Saveing = 2;
			}
		}
	}

	return S_OK;
}




HRESULT CSWH264HDDTransformFilter::ReportStatus(LPCSTR szInfo)
{
	if (!m_fInited)
	{
		PRINT("Err: not initialized yet\n");
		return E_NOTIMPL;
	}

	CSWString strInfo = "";
	strInfo.Append("H264存储:");	
	strInfo.Append(NULL != szInfo ? CSWString(szInfo) : GetStorageStatus());;

	return CSWMessage::SendMessage(MSG_APP_UPDATE_STATUS, (WPARAM)(const CHAR*)strInfo, 0);
}


HRESULT CSWH264HDDTransformFilter::ClearBuffer()
{
	m_fHistoryFileSaveDisabled = TRUE;

	CSWHDDTransformFilter::ClearBuffer();
	m_dwLastTime = CSWDateTime::GetSystemTick();
	if (NULL != m_pFileLock)
	{
		while (!m_lstFrame.IsEmpty())
		{
			CSWImage * pImage = m_lstFrame.RemoveHead();
			SAFE_RELEASE(pImage);
		}
		
		//free the file list
		//if (NULL != m_pFileLock) m_pFileLock->Lock();
		if (NULL != m_pFileLock) m_pFileLock->Pend();
		while (!m_lstFile.IsEmpty())
		{
			CSWMemory * pcFile = m_lstFile.RemoveHead();
			
			m_pcMemoryFactory->Free(pcFile);//swpa_mem_free(pbFile);
			pcFile = NULL;
		}	
		//if (NULL != m_pFileLock) m_pFileLock->Unlock();
		if (NULL != m_pFileLock) m_pFileLock->Post();

		//SAFE_RELEASE(m_pFileLock);

		//SAFE_RELEASE(m_pcTickLock);

		//SAFE_RELEASE(m_pSemaSaveQueueSync);

	}
}




HRESULT CSWH264HDDTransformFilter::SaveVideo(BYTE* pVideoFile)
{
	if (NULL == pVideoFile)
	{
		PRINT("Err: NULL == pVideoFile\n");
		return E_INVALIDARG;
	}
	
	INT iBufSize = 0;
	_IMAGE_FILE_INFO sImageInfo;
	swpa_memset(&sImageInfo, 0, sizeof(sImageInfo));
	DWORD dwSizeCrc = 0, dwDataCrc = 0;
	
	HRESULT 	hr = S_OK;

	swpa_memcpy(&dwSizeCrc, pVideoFile, sizeof(dwSizeCrc));
	swpa_memcpy(&iBufSize, pVideoFile+sizeof(dwSizeCrc), sizeof(iBufSize));
	swpa_memcpy(&dwDataCrc, pVideoFile+sizeof(dwSizeCrc)+sizeof(iBufSize), sizeof(dwDataCrc));
	swpa_memcpy(&sImageInfo, pVideoFile+sizeof(dwSizeCrc)+sizeof(iBufSize)+sizeof(dwDataCrc), sizeof(sImageInfo));		

	CSWDateTime FileTime(sImageInfo.sTimeStamp);

	//PRINT("Info: saving h264 video file (time %u)\n", sImageInfo.sTimeStamp.sec);
	PRINT("Info: saving h264 video file (time: %d/%d/%d/%d:%d:%d)\n", 
			FileTime.GetYear(), FileTime.GetMonth(), FileTime.GetDay(),
			FileTime.GetHour(), FileTime.GetMinute(), FileTime.GetSecond());
	
	if( m_iIsGB28181Saveing == 0 )
	{
		CHAR strBeginTime[128];
		swpa_memset(strBeginTime,0,128);
		swpa_sprintf(strBeginTime,"<StartTime>%04d-%02d-%02dT%02d:%02d:%02d</StartTime>\r\n",
			FileTime.GetYear(), FileTime.GetMonth(), FileTime.GetDay(),
			FileTime.GetHour(), FileTime.GetMinute(), FileTime.GetSecond());
		
		BOOL fFlag = TRUE;
		if (SWPAR_OK != swpa_utils_dir_exist("/Result"))
		{
			SW_TRACE_DEBUG("not /Result/\n");
			if (SWPAR_OK != swpa_utils_dir_create("/Result"))
			{
				SW_TRACE_DEBUG("Err: failed to create /Result/\n");
				fFlag = FALSE;
			}
		}
		if (SWPAR_OK != swpa_utils_dir_exist("/Result/0"))
		{
			SW_TRACE_DEBUG("not /Result/0\n");
			if (SWPAR_OK != swpa_utils_dir_create("/Result/0"))
			{
				SW_TRACE_DEBUG("Err: failed to create /Result/0\n");
				fFlag = FALSE;
			}
		}
		if(fFlag)
		{
			HRESULT ret0 = E_FAIL;
			if( 0 == swpa_utils_file_exist("/Result/0/StartEndTime") )
			{
//				ret0 = m_VideoRecordList->Open("/Result/0/StartEndTime", "w");
					FILE *pDD = fopen("/Result/0/StartEndTime","w");
					if( pDD != NULL )
					{
						fwrite(strBeginTime,1,swpa_strlen(strBeginTime),pDD);
						fclose(pDD);
						pDD = NULL;
					}
					else
					{
						SW_TRACE_DEBUG("Open /Result/0/StartEndTime ERR\n");
						return E_FAIL;
					}
			}
			else
			{
//				ret0 = m_VideoRecordList->Open("/Result/0/StartEndTime", "a+");
					FILE *pDD = fopen("/Result/0/StartEndTime","a+");
					if( pDD != NULL )
					{
						fwrite(strBeginTime,1,swpa_strlen(strBeginTime),pDD);
						fclose(pDD);
						pDD = NULL;
					}
					else
					{
						SW_TRACE_DEBUG("Open /Result/0/StartEndTime ERR\n");
						return E_FAIL;
					}
			}		
//			if( ret0 != S_OK )
//			{
//				SW_TRACE_DEBUG("Open /Result/0/StartEndTime ERR\n");
//				return E_FAIL;
//			}
//			HRESULT ret = m_VideoRecordList->Write(strBeginTime, swpa_strlen(strBeginTime));
//			m_VideoRecordList->Close();
//			PRINT("m_iIsGB28181Saveing  ret:%d,strBeginTime:%s",ret,strBeginTime);
			m_iIsGB28181Saveing = 1;
		}
	}
	
	if( m_iIsGB28181Saveing == 1 )
	{
		m_TempEndTime = FileTime;
	}
	
	
	//save the image data
	hr = m_pBigFile->Write(FileTime.GetYear(), FileTime.GetMonth(), FileTime.GetDay(),
		FileTime.GetHour(), FileTime.GetMinute(), FileTime.GetSecond(),
		(VOID*)(pVideoFile), iBufSize);
	if (S_OK != hr)
	{
		PRINT("Err: failed to save h264 video file (time: %04d/%02d/%02d %02d:%02d:%02d)[%#x]\n", 
			FileTime.GetYear(), FileTime.GetMonth(), FileTime.GetDay(),
			FileTime.GetHour(), FileTime.GetMinute(), FileTime.GetSecond(), hr);
		return E_FAIL;
	}

	CSWHDDTransformFilter::UpdateCapacity(iBufSize - sizeof(iBufSize));
	
	return S_OK;
}




HRESULT CSWH264HDDTransformFilter::OnHistoryFileSaving(VOID)
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
		
		if (FAILED(m_pSemaSaveQueueSync->Pend(100)))
		{
			//PRINT("Info: no Video obj in History-file saving queue!\n");
			if( m_iIsGB28181Saveing == 2 )
			{
				CHAR strEndTime[32];
				swpa_memset(strEndTime,0,32);
				swpa_sprintf(strEndTime,"<EndTime>%04d-%02d-%02dT%02d:%02d:%02d</EndTime>\r\n",
					m_TempEndTime.GetYear(), m_TempEndTime.GetMonth(), m_TempEndTime.GetDay(),
					m_TempEndTime.GetHour(), m_TempEndTime.GetMinute(), m_TempEndTime.GetSecond());
				
				BOOL fFlag = TRUE;
				if (SWPAR_OK != swpa_utils_dir_exist("/Result"))
				{
					SW_TRACE_DEBUG("not /Result/\n");
					if (SWPAR_OK != swpa_utils_dir_create("/Result"))
					{
						SW_TRACE_DEBUG("Err: failed to create /Result/\n");
						fFlag = FALSE;
					}
				}
				if (SWPAR_OK != swpa_utils_dir_exist("/Result/0"))
				{
					SW_TRACE_DEBUG("not /Result/0\n");
					if (SWPAR_OK != swpa_utils_dir_create("/Result/0"))
					{
						SW_TRACE_DEBUG("Err: failed to create /Result/0\n");
						fFlag = FALSE;
					}
				}
				
				if( fFlag )
				{
					SW_TRACE_DEBUG("ok /Result/0\n");
					CHAR temptt[11*1024];
					FILE *pDD = NULL;
					if( 0 == swpa_utils_file_exist("/Result/0/StartEndTime") )
					{
              			pDD = fopen("/Result/0/StartEndTime","w");
					}
					else
					{
              			pDD = fopen("/Result/0/StartEndTime","a+");
					}			
					
					DWORD dwReadLen = 0;
					if( pDD != NULL )
					{
						fwrite(strEndTime,1,swpa_strlen(strEndTime),pDD);
						
						swpa_memset(temptt,0,11*1024);
						dwReadLen = fread(temptt,1,11*1024,pDD);
						if( dwReadLen > 10240 )
						{
							CHAR *pptr = swpa_strstr(temptt, "</EndTime>");
							INT ioffset = pptr - temptt + 10;
							fwrite(temptt+ioffset,1,swpa_strlen(temptt+ioffset),pDD);
						}	
						swpa_memset(temptt,0,11*1024);
						fclose(pDD);
						pDD = NULL;				
					}
					else
					{
						SW_TRACE_DEBUG("Open /Result/0/StartEndTime ERR\n");
						continue;
					}		

					m_iIsGB28181Saveing = 0;
				}				
			}
			continue;
		}
	
		//SW_TRACE_DEBUG("semainfo: to pend\n");
		//m_pFileLock->Lock();
		m_pFileLock->Pend();
		//SW_TRACE_DEBUG("semainfo: pended\n");
		DWORD dwFileCount = m_lstFile.GetCount();		
		//m_pFileLock->Unlock();
		m_pFileLock->Post();
		
		PRINT("Info: History Video Queue Element Count: %lu\n", dwFileCount);

		while (dwFileCount --)
		{
			m_dwLastTime = CSWDateTime::GetSystemTick();
			
			CSWMemory * pVideoFile = NULL;
			if (1 == m_iSaveVideoType) //only save on pecancy
			{
				m_pFileLock->Pend();
				pVideoFile = m_lstFile.GetHead();		
				
				
				_IMAGE_FILE_INFO sImageInfo;
				swpa_memcpy(&sImageInfo, pVideoFile->GetBuffer()+sizeof(DWORD)+sizeof(INT)+sizeof(DWORD), sizeof(sImageInfo));

				CSWDateTime cTime(sImageInfo.sTimeStamp);

				BOOL fStore = FALSE;

				m_pcTickLock->Pend();
				SW_POSITION Pos = m_lstTriggerTick.GetHeadPosition();
				while (m_lstTriggerTick.IsValid(Pos))
				{
					SW_POSITION PrevPos = Pos;
					DWORD dwTick = m_lstTriggerTick.GetNext(Pos);
					CSWDateTime cStartTime(dwTick - 5000);
					CSWDateTime cEndTime(dwTick + 10000);
					
					if (cTime < cStartTime)
					{
						continue;
					}
					else if (cTime > cEndTime)
					{
						m_lstTriggerTick.RemoveAt(PrevPos); //该触发对应的录像已存完，将其从队列里删除
					}
					else
					{
						fStore = TRUE;
						break;
					}
				}
				m_pcTickLock->Post();

				if (fStore)
				{
					m_lstFile.RemoveHead();
					m_pFileLock->Post();
				}
				else
				{
					m_pFileLock->Post();
					break;
				}
			}
			else
			{
				//SW_TRACE_DEBUG("semainfo: to pend\n");
				//m_pFileLock->Lock();
				m_pFileLock->Pend();
				//SW_TRACE_DEBUG("semainfo: pended\n");
				pVideoFile = m_lstFile.RemoveHead();		
				//m_pFileLock->Unlock();
				m_pFileLock->Post();
			}
			
			if (NULL == pVideoFile)
			{
				PRINT("Err: got invalid history file, skip it\n");
				break;
			}
			
			INT iTryCount = 3;
			while (FAILED(SaveVideo((PBYTE)pVideoFile->GetBuffer())))
			{
				m_dwLastTime = CSWDateTime::GetSystemTick();
				if (0 == iTryCount)
				{
					PRINT("Err: failed to save video %p, skip this!\n", pVideoFile->GetBuffer());
					//CSWApplication::Exit(2);
					ReportStatus("异常");
					//m_fHistoryFileSaveDisabled = TRUE;
					//ClearBuffer();	
					CSWMessage::SendMessage(MSG_APP_REINIT_HDD, 0, 0);
					break;
				}
				else
				{
					PRINT("Warning: failed to save video %p, trying another %d time(s)!\n", pVideoFile->GetBuffer(), iTryCount);
					iTryCount--;
					swpa_thread_sleep_ms(100);
				}
			}
			
			m_pcMemoryFactory->Free(pVideoFile);
			pVideoFile = NULL;
			
			swpa_thread_sleep_ms(100);
		}
	}

	return S_OK;
}


HRESULT CSWH264HDDTransformFilter::OnHistoryFileTransmitting(const DWORD dwPinID)
{
	HRESULT hr = S_OK;
	CSWMemory* pcMemBuf = NULL;
	VOID * pvBuf = NULL;
	
	if (!m_fInited)
	{
		SW_TRACE_DEBUG("Err: not initialized yet\n");
		goto Out;
	}


	if (FILTER_RUNNING == GetState() && m_fHistoryTransmitting[dwPinID])
	{
		CSWDateTime myBeginTime(&m_tHistoryBeginTime[dwPinID]);
		CSWDateTime myEndTime(&m_tHistoryEndTime[dwPinID]);
		CSWDateTime myTime(&m_tHistoryBeginTime[dwPinID]);

		if (dwPinID==3 && m_fGB28181Backward)
		{
			myTime.SetTime(m_tHistoryEndTime[dwPinID], FALSE);
		}
		

		SWPA_TIME sAddTime;
		sAddTime.sec = 1;
		sAddTime.msec = 0;


		//PRINT("info:  myTime: %d/%d/%d/%d:%d:%d\n", 
		//		myTime.GetYear(), myTime.GetMonth(), myTime.GetDay(),
		//		myTime.GetHour(), myTime.GetMinute(), myTime.GetSecond());

		SW_TRACE_DEBUG("info:  BeginTime: %d/%d/%d/%d:%d:%d\n", 
				myBeginTime.GetYear(), myBeginTime.GetMonth(), myBeginTime.GetDay(),
				myBeginTime.GetHour(), myBeginTime.GetMinute(), myBeginTime.GetSecond());

		SW_TRACE_DEBUG("info:  EndTime: %d/%d/%d/%d:%d:%d\n", 
				myEndTime.GetYear(), myEndTime.GetMonth(), myEndTime.GetDay(),
				myEndTime.GetHour(), myEndTime.GetMinute(), myEndTime.GetSecond());
		
		
		m_dwHistoryTransmittingStatus[dwPinID] = HISTORY_TRANSMITTING_ONGOING;

		DWORD dwCurTick = CSWDateTime::GetSystemTick();
		DWORD dwPrevTick = dwCurTick;

    	DWORD dwFailCount = 0;
		DWORD dwWaitCount = 0;

		SW_TRACE_DEBUG("info: IterateTime: %d/%d/%d/%d:%d:%d\n", 
				myTime.GetYear(), myTime.GetMonth(), myTime.GetDay(),
				myTime.GetHour(), myTime.GetMinute(), myTime.GetSecond());

		SW_TRACE_DEBUG("Info: m_fGB28181Backward = %d\n", m_fGB28181Backward);
		SW_TRACE_DEBUG("Info: m_fHistoryTransmitting[dwPinID] = %d\n", m_fHistoryTransmitting[dwPinID]);
		SW_TRACE_DEBUG("Info: (myTime < myBeginTime) = %d\n", (myTime < myBeginTime));
		SW_TRACE_DEBUG("Info: (myTime < myBeginTime) = %d\n", (myTime < myBeginTime));
		
		while (FILTER_RUNNING == GetState()
			&& m_fHistoryTransmitting[dwPinID] 
			)
		{
			if (dwPinID==3 && m_fGB28181Backward)
			{
				if ((myTime < myBeginTime))
				{
					SW_TRACE_DEBUG("Info: read data -- done.\n");
					break;
				}
			}
			else
			{
				if (myTime > myEndTime)
				{
					SW_TRACE_DEBUG("Info: read data -- done.\n");
					break;
				}
			}
			
			SW_TRACE_DEBUG("Info: in while loop\n");

			SW_TRACE_DEBUG("info: IterateTime: %d/%d/%d/%d:%d:%d\n", 
				myTime.GetYear(), myTime.GetMonth(), myTime.GetDay(),
				myTime.GetHour(), myTime.GetMinute(), myTime.GetSecond());
			
			if (m_iGBVideoSkipBlock > 0)
			{
				SW_TRACE_DEBUG("m_iGBVideoSkipBlock = %d, adjusting timeline\n", m_iGBVideoSkipBlock);
				CSWDateTime beginTime(&m_tHistoryBeginTime[dwPinID]);
				SWPA_TIME sSkipTime;
				sSkipTime.sec = m_iGBVideoSkipBlock;
				sSkipTime.msec = 0;

				if (m_fGB28181Backward)
				{
					//myTime = beginTime - sSkipTime;
				}
				else
				{
					myTime = beginTime + sSkipTime;
				}
				
				m_iGBVideoSkipBlock = 0;
				
				if (myTime > myEndTime)
				{
					SW_TRACE_DEBUG("beyond record timeline\n", m_iGBVideoSkipBlock);
					break;
				}
			}
			
			INT iBufSize = 0;

			if (0 == m_iSaveVideoType) //一直存储视频时才做此限制
			{
				//m_pFileLock->Lock();
				m_pFileLock->Pend();
				DWORD dwFileCount = m_lstFile.GetCount();		
				//m_pFileLock->Unlock();
				m_pFileLock->Post();

				if (1 < dwFileCount)
				{
					SW_TRACE_DEBUG("Info: reading is stuck due to file queue count > 1\n");
					CSWApplication::Sleep(200);
					continue;
				}
			}

			if (!CSWHDDTransformFilter::CheckCurrentCapacity())
			{	
				SW_TRACE_DEBUG("Info: reading is stuck due to capacity > 4MB\n");
				CSWApplication::Sleep(500);
				continue;
			}
			
			
			SW_TRACE_DEBUG("info: going to read h264 video file (time: %d/%d/%d/%d:%d:%d)\n", 
				myTime.GetYear(), myTime.GetMonth(), myTime.GetDay(),
				myTime.GetHour(), myTime.GetMinute(), myTime.GetSecond());
			
			hr = m_pBigFile->GetSize(myTime.GetYear(), myTime.GetMonth(), myTime.GetDay(), 
				myTime.GetHour(), myTime.GetMinute(), myTime.GetSecond(), 0, &iBufSize);
			if (E_RECORD_NONE == hr)
			{
				CSWDateTime cCurTime;
				SWPA_TIME tCurTime;
				SWPA_TIME tTime;
				cCurTime.GetTime(&tCurTime);
				myTime.GetTime(&tTime);
				
				if (tTime.sec + 10 > tCurTime.sec) // wait 10 sec if too close to now
				{
					swpa_thread_sleep_ms(1000);
				}
				else
				{
					if (0 == m_iSaveVideoType) //一直存储视频时才直接跳过，否则等待
					{
						SW_TRACE_DEBUG("Err:  no video in %04d/%02d/%02d/%02d:%02d:%02d, shift to next second\n", 
							myTime.GetYear(), myTime.GetMonth(), myTime.GetDay(), myTime.GetHour(), myTime.GetMinute(), myTime.GetSecond());

						if (m_fGB28181Backward)
							myTime -= sAddTime;
						else
							myTime += sAddTime;
						swpa_thread_sleep_ms(100);
					}
					else
					{
						swpa_thread_sleep_ms(1000);
						if (++dwWaitCount > m_dwSaveQueueSize)
						{
							SW_TRACE_DEBUG("Err: no video in %04d/%02d/%02d/%02d:%02d:%02d, already waited %dsec, shift to next second\n", 
								myTime.GetYear(), myTime.GetMonth(), myTime.GetDay(), myTime.GetHour(), myTime.GetMinute(), myTime.GetSecond(),
								m_dwSaveQueueSize);
							if (m_fGB28181Backward)
								myTime -= sAddTime;
							else
								myTime += sAddTime;
							dwWaitCount = 0;
						}
					}
				}
				
				continue;
			}
			else if (E_FAIL == hr || 0 >= iBufSize)
			{
				if (dwFailCount++<3)
				{
					SW_TRACE_DEBUG("Err: failed to read video file at %04d/%02d/%02d/%02d:%02d:%02d, trying again...\n", 
						myTime.GetYear(), myTime.GetMonth(), myTime.GetDay(), myTime.GetHour(), myTime.GetMinute(), myTime.GetSecond());
					swpa_thread_sleep_ms(1000);
				}
				else
				{
					SW_TRACE_DEBUG("Err: failed to read video file at %04d/%02d/%02d/%02d:%02d:%02d, skip it\n", 
						myTime.GetYear(), myTime.GetMonth(), myTime.GetDay(), myTime.GetHour(), myTime.GetMinute(), myTime.GetSecond());
					if (m_fGB28181Backward)
						myTime -= sAddTime;
					else
						myTime += sAddTime;
					swpa_thread_sleep_ms(100);
					dwFailCount = 0;
				}
				continue;
			}
			else if (E_RECORD_WRITING == hr)
			{
				swpa_thread_sleep_ms(1000);
				continue;
			}

			dwFailCount = 0;
			dwWaitCount = 0;

			SW_TRACE_DEBUG("iBufSize=%d\n", iBufSize);

			pcMemBuf = m_pcMemoryFactory->Alloc(ALGIN_SIZE(iBufSize, 128*1024));//alignment, reduce memory fragment
			if (NULL == pcMemBuf)
			{
				SW_TRACE_DEBUG("Err: no memory for pcMemBuf\n");
				hr = E_OUTOFMEMORY;
				goto Out;
			}
			
			pvBuf = pcMemBuf->GetBuffer();
			if (NULL == pvBuf)
			{
				SW_TRACE_DEBUG("Err: no enough memory for pvBuf (needs %d Byte)\n", iBufSize);
				hr = E_OUTOFMEMORY;
				goto Out;
			}

			
			hr = m_pBigFile->Read(myTime.GetYear(), myTime.GetMonth(), myTime.GetDay(), 
				myTime.GetHour(), myTime.GetMinute(), myTime.GetSecond(), 0,  pvBuf,  iBufSize);
			if (S_OK != hr)
			{
				SW_TRACE_DEBUG("Err: failed to read file [%u]\n", hr);
				hr = E_FAIL;
				goto Out;
			}

			CSWHDDTransformFilter::UpdateCapacity(iBufSize);


			CSWImage * pNewImage = NULL;	
			if (NULL == (pNewImage = CreateVideo((PBYTE)pvBuf)))
			{
				SW_TRACE_DEBUG("Err: failed to create pNewImage\n");
				
				m_pcMemoryFactory->Free(pcMemBuf);
				pcMemBuf = NULL;
				pvBuf = NULL;//SAFE_MEM_FREE(pvBuf);
				
				if (m_fGB28181Backward)
					myTime -= sAddTime;
				else
					myTime += sAddTime;
				continue;
			}

			if (dwPinID == 3)
			{
				SendFrameToGB28181(dwPinID, pNewImage);
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
						//PRINT("Info: GetSendInterval() ret %d\n", GetSendInterval());
						CSWApplication::Sleep(GetSendInterval());
					}
					else
					{
						CSWApplication::Sleep(1000); //avoid 
					}
				} while (m_fHistoryTransmitting[dwPinID] && (FAILED(GetOut(dwPinID)->Deliver(pNewImage))));
			}

			SAFE_RELEASE(pNewImage);
			
			m_pcMemoryFactory->Free(pcMemBuf);
			pcMemBuf = NULL;
			pvBuf = NULL;//SAFE_MEM_FREE(pvBuf);
				
			if (m_fGB28181Backward)
				myTime -= sAddTime;
			else
				myTime += sAddTime;
			swpa_thread_sleep_ms(20);
		}

		if (m_fGB28181Backward ? myTime < myEndTime :myTime > myEndTime)
		{
			//send the End-Flag image
			CSWImage* pEndFlagImage = new CSWImage();
			if (NULL == pEndFlagImage) /* no 'Create()' op here, make it a non-valid image*/
			{
				SW_TRACE_DEBUG("Err: no enough memory for pEndFlagImage, won't send Image-End-Flag\n");
			}
			else
			{
				SW_COMPONENT_IMAGE cImage;
				swpa_memset(&cImage, 0, sizeof(cImage));
				cImage.cImageType = SW_IMAGE_H264_HISTORY;
				pEndFlagImage->SetImage(cImage);
				
				do
				{
					if (0 < GetSendInterval())
					{
						/*dwCurTick = CSWDateTime::GetSystemTick();
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
					
				}
				while (m_fHistoryTransmitting[dwPinID] && (FAILED(GetOut(dwPinID)->Deliver(pEndFlagImage))));
				SW_TRACE_DEBUG("history end \n");
			}
			SAFE_RELEASE(pEndFlagImage);
		}
	}


Out:

	if (NULL != pcMemBuf)
	{	
		m_pcMemoryFactory->Free(pcMemBuf);
		pcMemBuf = NULL;
		pvBuf = NULL;//SAFE_MEM_FREE(pvBuf);
	}

	m_dwHistoryTransmittingStatus[dwPinID] = HISTORY_TRANSMITTING_FINISHED;

	SW_TRACE_DEBUG("Info: %s() exited...\n", __FUNCTION__);
	
	return hr;//
	
}


HRESULT CSWH264HDDTransformFilter::SendFrameToGB28181(const DWORD dwPinID, CSWImage * pFrame)
{
	SW_COMPONENT_IMAGE sImage;
	pFrame->GetImage(&sImage);
		
	//if( 1)//dwPinID == 3 )
	{
		static INT ppp = 0;
		if( ppp > 10)
		{
			SW_TRACE_DEBUG("SW_IMAGE_H264_HISTORY \n");
			ppp = 0;
		}
		ppp++;
		
		sImage.cImageType = SW_IMAGE_H264_HISTORY;
		pFrame->SetImage(sImage);
	}

	//send the image
	BOOL b1 = FALSE,b2 = FALSE;
	do
	{
		if (0 < GetSendInterval())
		{			
			if( m_iSendPauseTime > 0 )
			{
				static DWORD iTick = 0;
				if( iTick == 0 )
				{ 
					iTick = CSWDateTime::GetSystemTick();
				}
				SW_TRACE_DEBUG("m_iSendPauseTime ======== %d\n", m_iSendPauseTime);
				if( CSWDateTime::GetSystemTick() - iTick <= m_iSendPauseTime )
				{
					CSWApplication::Sleep(10);
					break;
				}					
				iTick = 0;
				m_iSendPauseTime = 0;
			}
			else
			{
				CSWApplication::Sleep(GetSendInterval());
			}
		}
		else
		{
			CSWApplication::Sleep(20); //avoid 
		}

		
		static INT tt = 0;
		if(tt > 10)
		{
			SW_TRACE_DEBUG("Deliver == %d \n", dwPinID);
			tt = 0;
		}
		tt++;
		
		b1 = m_fHistoryTransmitting[dwPinID];

		//SW_TRACE_DEBUG("b1 == %d, b2==%d \n", b1,b2);

		CSWImage* pArrayFrame[32] = {0};
		const DWORD dwFrameCountMax = sizeof(pArrayFrame)/sizeof(pArrayFrame[0]);
		DWORD dwCount = 0;
		DWORD dwOffset = 0;
		
		PBYTE pbAddr = sImage.rgpbData[0];
		//SW_TRACE_DEBUG("sImage.iSize = %d\n", sImage.iSize);
		while (pbAddr < sImage.rgpbData[0] + (DWORD)sImage.iSize)
		{

			//SW_TRACE_DEBUG("pbAddr =0x%x\n", pbAddr);
			INT iFrameType = 0;
			swpa_memcpy(&iFrameType, pbAddr, sizeof(iFrameType));
			pbAddr += sizeof(iFrameType);

			INT iFrameSize = 0;
			swpa_memcpy(&iFrameSize, pbAddr, sizeof(iFrameSize));
			pbAddr += sizeof(iFrameSize);		

			CSWImage * pNewImage = NULL;		
			HRESULT hr = CSWImage::CreateSWImage(&pNewImage, 
				SW_IMAGE_H264, 
				sImage.iWidth, 
				sImage.iHeight,
				CSWMemoryFactory::GetInstance(SW_SHARED_MEMORY), 
				0, //todo: //sImage.dwFrameNo, 
				pFrame->GetRefTime(), //todo: //sImageInfo.dwRefTimeMS, 
				FALSE,
				iFrameType == 4096 ? "IFrame" : "PFrame",
				iFrameSize);
			if (S_OK != hr)
			{
				PRINT("Err: failed to CreateSWImage() [0x%x]\n", hr);
				return NULL;
			}

			//SW_TRACE_DEBUG("hr =0x%x\n", hr);
			SW_COMPONENT_IMAGE sNewComponentImage;
			pNewImage->GetImage(&sNewComponentImage);
			sNewComponentImage.cImageType = SW_IMAGE_H264_HISTORY;
			
			swpa_memcpy(sNewComponentImage.rgpbData[0], (VOID*)pbAddr, iFrameSize);

			sNewComponentImage.iSize = iFrameSize;
			pNewImage->SetImage(sNewComponentImage);


			DWORD dwTimeHigh = 0;
			DWORD dwTimeLow = 0;
			//CSWDateTime::TimeConvert(sImage.sTimeStamp, &dwTimeHigh, &dwTimeLow);

			pFrame->GetRealTime(&dwTimeHigh, &dwTimeLow);
			pNewImage->SetRealTime(dwTimeHigh, dwTimeLow);

			if (0)//(m_fGB28181Backward)
			{
				if (iFrameType == 4096)//IFrame
				{
					pArrayFrame[dwFrameCountMax-1-dwCount] = pNewImage;
					++dwCount;
				}
				else
				{
					SAFE_RELEASE(pNewImage);
				}
			}
			else
			{
				pArrayFrame[dwCount] = pNewImage;
				++dwCount;
			}

			
			if (dwCount >= dwFrameCountMax)
			{
				break;
			}
			
			pbAddr += iFrameSize;
		}	

		SW_TRACE_DEBUG("begin deliver 1 sec video\n");
		for (INT i=0; i<dwCount; i++)
		{
			CSWImage* pImage = pArrayFrame[i];//[m_fGB28181Backward ? dwFrameCountMax-1-i : i];
			b2 = FAILED(GetOut(dwPinID)->Deliver(pImage));
			SAFE_RELEASE(pImage);
			//swpa_thread_sleep_ms(1000/dwCount);
		}
		SW_TRACE_DEBUG("stop deliver 1 sec video\n");
		
		//SW_TRACE_DEBUG("b1 == %d, b2==%d \n", b1,b2);
		
	} while ( b1 && b2);
	
	return S_OK;
}

CSWImage * CSWH264HDDTransformFilter::CreateVideo(PBYTE pbRawData)
{
	if (NULL == pbRawData)
	{
		SW_TRACE_DEBUG("Err: NULL == pbRawData\n");
		return NULL;
	}

	INT iBufSize = 0;
	DWORD dwSizeCrcStored = 0, dwDataCrcStored = 0;

	PBYTE pbAddr = pbRawData;

	swpa_memcpy(&dwSizeCrcStored, pbAddr, sizeof(dwSizeCrcStored));
	pbAddr += sizeof(dwSizeCrcStored);

	swpa_memcpy(&iBufSize, pbAddr, sizeof(iBufSize));
	pbAddr += sizeof(iBufSize);
	
	swpa_memcpy(&dwDataCrcStored, pbAddr, sizeof(dwDataCrcStored));
	pbAddr += sizeof(dwDataCrcStored);

	if (dwSizeCrcStored != CSWUtils::CalcCrc32(0, (PBYTE)&iBufSize, sizeof(iBufSize)))
	{
		SW_TRACE_NORMAL("Err: Video Size check failed\n");
		return NULL;
	}

	if (dwDataCrcStored != CSWUtils::CalcCrc32(0, pbAddr, iBufSize-sizeof(iBufSize)-sizeof(dwSizeCrcStored)-sizeof(dwDataCrcStored)))
	{
		SW_TRACE_NORMAL("Err: Video Data check failed\n");
		return NULL;
	}

	CSWImage * pImage = NULL;	
	if (NULL == (pImage = CreateImageFromBuffer(pbAddr, iBufSize-sizeof(iBufSize)-sizeof(dwSizeCrcStored)-sizeof(dwDataCrcStored))))
	{
		SW_TRACE_DEBUG("Err: failed to create video\n");
		return NULL;
	}
	pImage->SetFrameName("IPFrame");//only for history frame

	return pImage;	
}



HRESULT CSWH264HDDTransformFilter::OnSetTransmittingType(WPARAM wParam, LPARAM lParam)
{
	DWORD dwPinID = (DWORD)wParam;
	DWORD dwType = (DWORD)lParam;

	return CSWHDDTransformFilter::SetTransmittingType(dwPinID,dwType);
}



HRESULT CSWH264HDDTransformFilter::OnStartHistoryFileTransmitting(WPARAM wParam, LPARAM lParam)
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
	
	 SW_TRACE_DEBUG("dwPinID:%d,pParam %04hd.%02hd.%02hd_%02hd:%02hd:%02hd %04hd.%02hd.%02hd_%02hd:%02hd:%02hd\n",
    dwPinID,pParam->psBeginTime->year, pParam->psBeginTime->month, pParam->psBeginTime->day, 
		pParam->psBeginTime->hour, pParam->psBeginTime->min, pParam->psBeginTime->sec,
		pParam->psEndTime->year, pParam->psEndTime->month, pParam->psEndTime->day, 
		pParam->psEndTime->hour, pParam->psEndTime->min, pParam->psEndTime->sec);
	
	return CSWHDDTransformFilter::StartHistoryFileTransmitting(dwPinID, pParam->psBeginTime, pParam->psEndTime, pParam->dwCarID);
}






HRESULT CSWH264HDDTransformFilter::OnStopHistoryFileTransmitting(WPARAM wParam, LPARAM lParam)
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




HRESULT CSWH264HDDTransformFilter::OnStartGB28181FileTransmitting(WPARAM wParam, LPARAM lParam)
{
	DWORD dwPinID = (DWORD)wParam;

	if (GetOutCount() <= dwPinID)
	{
		PRINT("Err: dwPinID = %u is invalid\n", dwPinID);
		return E_INVALIDARG;
	}

	HISTORY_GB28181_TRANSMITTING_PARAM* pParam = (HISTORY_GB28181_TRANSMITTING_PARAM*)lParam;
	if (NULL == pParam)
	{
		PRINT("Err: NULL == pParam\n");
		return E_INVALIDARG;
	}
	
	 SW_TRACE_DEBUG("dwPinID:%d,pParam %04hd.%02hd.%02hd_%02hd:%02hd:%02hd %04hd.%02hd.%02hd_%02hd:%02hd:%02hd IsBackward=%s\n",
    dwPinID,pParam->psBeginTime->year, pParam->psBeginTime->month, pParam->psBeginTime->day, 
		pParam->psBeginTime->hour, pParam->psBeginTime->min, pParam->psBeginTime->sec,
		pParam->psEndTime->year, pParam->psEndTime->month, pParam->psEndTime->day, 
		pParam->psEndTime->hour, pParam->psEndTime->min, pParam->psEndTime->sec,
		pParam->fBackward ? "TRUE" : "FALSE");

	m_fGB28181Backward = pParam->fBackward;
	
	return CSWHDDTransformFilter::StartHistoryFileTransmitting(dwPinID, pParam->psBeginTime, pParam->psEndTime, 0);
}





HRESULT CSWH264HDDTransformFilter::OnStopGB28181FileTransmitting(WPARAM wParam, LPARAM lParam)
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


HRESULT CSWH264HDDTransformFilter::OnStartRealtimeTransmitting(WPARAM wParam, LPARAM lParam)
{
	DWORD dwPinID = (DWORD)wParam;

	if (GetOutCount() <= dwPinID)
	{
		PRINT("Err: dwPinID = %u is invalid\n", dwPinID);
		return E_INVALIDARG;
	}

	return CSWHDDTransformFilter::StartRealtimeTransmitting(dwPinID);
}




HRESULT CSWH264HDDTransformFilter::OnStopRealtimeTransmitting(WPARAM wParam, LPARAM lParam)
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





HRESULT CSWH264HDDTransformFilter::OnGetHistoryFileTransmittingStatus(WPARAM wParam, LPARAM lParam)
{
	DWORD dwPinID = (DWORD)wParam;
	DWORD* pdwStatus = (DWORD*)lParam;

	return CSWHDDTransformFilter::GetHistoryFileTransmittingStatus(dwPinID, pdwStatus);
}



HRESULT CSWH264HDDTransformFilter::OnTriggerVideoSaving(WPARAM wParam, LPARAM lParam)
{
	DWORD dwTriggerTick = (DWORD)wParam;
	CSWDateTime cTime(dwTriggerTick);

	SW_TRACE_DEBUG("Info: OnTriggerVideoSaving, time:%4d-%2d-%2d %2d:%2d:%2d %3d\n",
		cTime.GetYear(), cTime.GetMonth(), cTime.GetDay(),
		cTime.GetHour(), cTime.GetMinute(), cTime.GetSecond(), cTime.GetMSSecond());
	

	if( m_fInited && m_pcTickLock != NULL )
	{
		m_pcTickLock->Pend();
		m_lstTriggerTick.AddTail(dwTriggerTick);
		m_pcTickLock->Post();
	}

	
	return S_OK;
}

HRESULT CSWH264HDDTransformFilter::OnGB28181VideoSavingEnable(PVOID pvBuffer, INT iSize)
{
	DWORD* pdwRecordEnable = (DWORD*)pvBuffer;
	SW_TRACE_DEBUG("pdwRecordEnable:%d %d\n",*pdwRecordEnable,iSize);
	if( *pdwRecordEnable == 1 )
	{
		m_fGBVideoSaveRecord = TRUE;
	}
	else
	{
		m_fGBVideoSaveRecord = FALSE;
	}
	SW_TRACE_DEBUG("m_fGBVideoSaveRecord:%d\n",m_fGBVideoSaveRecord);
	return S_OK;
}

HRESULT CSWH264HDDTransformFilter::SetVideoSaveEnable(const BOOL fVideoSaveEnable )
{
	m_fGBVideoSaveCtlEnable = fVideoSaveEnable;
	SW_TRACE_DEBUG("fVideoSaveEnable:%d,m_fGBVideoSaveCtlEnable:%d\n",fVideoSaveEnable,m_fGBVideoSaveCtlEnable);
}

BOOL CSWH264HDDTransformFilter::GetVideoSaveEnable(VOID)
{
	return m_fGBVideoSaveCtlEnable;
}

HRESULT CSWH264HDDTransformFilter::OnGB28181VideoSendScale(PVOID pvBuffer, INT iSize)
{
	DWORD* pdwValue = (DWORD*)pvBuffer;
	DWORD dwValue = *pdwValue;
	WORD wplay,wscale;
	wplay = dwValue >> 16;
	wscale = dwValue;
	SW_TRACE_DEBUG("wplay:%d,wscale:%d\n",wplay,wscale);
	
	switch (wplay)
	{
		case 1:
			{
				FLOAT fltScale = wscale;
				fltScale = fltScale / 100.0f;
				INT ioldScale = GetSendInterval();
				if( m_iRecordSendInterval == -1 )
				{
					m_iRecordSendInterval = GetSendInterval();
				}
				
				ioldScale = (INT)((FLOAT)m_iRecordSendInterval / fltScale);
				SetSendInterval(ioldScale);
				SW_TRACE_DEBUG("fltScale:%f,ioldScale:%d\n",fltScale,ioldScale);
				m_iSendPauseTime = 0;
			}
			break;
		case 2:
			m_iSendPauseTime = wscale * 1000;
			SW_TRACE_DEBUG("m_iSendPauseTime:%d\n",m_iSendPauseTime);
			break;
		case 3:
			m_iGBVideoSkipBlock = 15;
			m_iSendPauseTime = 0;
			SW_TRACE_DEBUG("skip range %d\n", 1);
			break;
	}
		
}
 