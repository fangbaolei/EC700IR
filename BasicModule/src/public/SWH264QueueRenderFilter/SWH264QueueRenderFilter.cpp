// 该文件编码格式必须是WIN936
/**
* @file SWH264QueueRenderFilter.cpp
* @brief Implementation of CSWH264QueueRenderFilter
* 
* @copyright Signalway All Rights Reserved
* @author zhouy
* @date 2013-03-22
* @version 1.0
*/
	
#include "SWFC.h"
#include "SWH264QueueRenderFilter.h"
#include "SWTransmittingOpt.h"




CSWH264QueueRenderFilter::CSWH264QueueRenderFilter()
  : CSWBaseFilter(1, 0)
  , CSWMessage(MSG_H264_QUEUE_BEGIN, MSG_H264_QUEUE_END)
  , m_fInited(FALSE)
  , m_dwQueueSize(30)
  , m_pcMemoryFactory(NULL)
{
	GetIn(0)->AddObject(CLASSID(CSWImage));
}


CSWH264QueueRenderFilter::~CSWH264QueueRenderFilter()
{
	if (!m_fInited)
	{
		return ;
	}
	
	Stop();

	m_pcMemoryFactory = NULL;
	
	m_fInited = FALSE;
	
}


HRESULT CSWH264QueueRenderFilter::Initialize()
{
	
	m_pcMemoryFactory = CSWMemoryFactory::GetInstance(SW_SHARED_MEMORY);
	if (NULL == m_pcMemoryFactory)
	{
		SW_TRACE_DEBUG("Err: failed to get m_pcMemoryFactory instance\n");
		return E_FAIL;
	}
	
	m_cObjLock.Create(1, 1);
	m_cFileLock.Create(1, 1);

	m_cSemaObjSync.Create(0, 256);

	m_lstFile.SetMaxCount(m_dwQueueSize);
	
	SW_TRACE_DEBUG("Info: init ok\n");
	m_fInited = TRUE;

	return S_OK;
}


HRESULT CSWH264QueueRenderFilter::Run()
{
	if (!m_fInited)
	{
		SW_TRACE_DEBUG("Err: not init yet\n");
		return E_FAIL;
	}
	
	if (FAILED(CSWBaseFilter::Run()))
	{
		SW_TRACE_DEBUG("Err: Failed to run this filter\n");
		return E_FAIL;
	}

	if (FAILED(m_cThread.Start(H264EnqueueProxy, this)))
	{
		SW_TRACE_DEBUG("Err: failed to start H264Enqueue thread\n");
		return E_FAIL;
	}

	return S_OK;
}



HRESULT CSWH264QueueRenderFilter::Stop()
{
	if (!m_fInited)
	{
		SW_TRACE_DEBUG("Err: not init yet\n");
		return E_FAIL;
	}
	
	if (FAILED(CSWBaseFilter::Stop()))
	{
		SW_TRACE_DEBUG("Err: Failed to stop this filter\n");
		return E_FAIL;
	}

	//free the frame list
	while (!m_lstFrame.IsEmpty())
	{
		CSWImage * pImage = m_lstFrame.RemoveHead();
		SAFE_RELEASE(pImage);
	}
	
	//free the file list
	m_cFileLock.Pend();
	while (!m_lstFile.IsEmpty())
	{
		CSWImage * pcFile = m_lstFile.RemoveHead();
		SAFE_RELEASE(pcFile);
	}	
	m_cFileLock.Post();

	m_cObjLock.Pend();
	while (!m_lstObj.IsEmpty())
	{
		CSWImage * pImage = (CSWImage * )m_lstObj.RemoveHead();
		SAFE_RELEASE(pImage);
	}	
	m_cObjLock.Post();

	if (FAILED(m_cThread.Stop()))
	{
		SW_TRACE_DEBUG("Err: failed to stop H264Enqueue thread\n");
		return E_FAIL;
	}

	return S_OK;
}





PVOID CSWH264QueueRenderFilter::H264EnqueueProxy(PVOID pvArg)
{
	if (NULL != pvArg)
	{
		CSWH264QueueRenderFilter* pThis = (CSWH264QueueRenderFilter*)pvArg;
		pThis->H264Enqueue();
	}
}

HRESULT CSWH264QueueRenderFilter::H264Enqueue()
{
	static BOOL fFirstIFrameFound = FALSE, fSecondIFrameFound = FALSE;;
	static SWPA_TIME sFirstIFTime, sSecondIFTime ;
	static DWORD dwFrameCount = 0;

	SW_TRACE_DEBUG("Info: running...\n");
	if (!m_fInited)
	{
		SW_TRACE_DEBUG("Err: not init yet\n");
		return E_FAIL;
	}

	while (FILTER_RUNNING == GetState())
	{
		if (FAILED(m_cSemaObjSync.Pend(2000)))
		{
			SW_TRACE_DEBUG("Info: failed to pend\n");
			continue;
		}
		
		m_cObjLock.Pend();
		CSWImage *pImage = (CSWImage*)m_lstObj.RemoveHead();
		m_cObjLock.Post();

		if (NULL != pImage)
		{
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
							//SW_TRACE_DEBUG("Info: sSecondIFTime - sFirstIFTime = %d\n", sSecondIFTime.sec - sFirstIFTime.sec);
						}
						
						if (25 < dwFrameCount)
						{
							//SW_TRACE_DEBUG("Info: dwFrameCount = %d\n", dwFrameCount);
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

			SAFE_RELEASE(pImage);
		}
		
	}
	return S_OK;
}





HRESULT CSWH264QueueRenderFilter::FrameEnqueue(CSWImage* pImage)
{
	if (!m_fInited)
	{
		SW_TRACE_DEBUG("Err: not initialized yet\n");
		return E_NOTIMPL;
	}
	
	if (NULL == pImage)
	{
		SW_TRACE_DEBUG("Err: (NULL == pImage!\n");
		return E_INVALIDARG;
	}

	SAFE_ADDREF(pImage);
	m_lstFrame.AddTail(pImage);


	return S_OK;
}

HRESULT CSWH264QueueRenderFilter::FileEnqueue(const SWPA_TIME& sTimeStamp)
{
	if (!m_fInited)
	{
		SW_TRACE_DEBUG("Err: not initialized yet\n");
		return E_NOTIMPL;
	}

	if (m_lstFrame.IsEmpty())
	{
		SW_TRACE_DEBUG("Err: Frame List is empty\n");
		return E_INVALIDARG;
	}
		

	INT iBufSize = 0;
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

//	SW_TRACE_DEBUG("Info: iBufSize = %d\n", iBufSize);
	CSWMemory* pcMemBuf = m_pcMemoryFactory->Alloc(ALGIN_SIZE(iBufSize, 128*1024));//alignment, reduce memory fragment
	if (NULL == pcMemBuf || NULL == pcMemBuf->GetBuffer())
	{
		SW_TRACE_DEBUG("Err: no memory for pcMemBuf\n");
		while (!m_lstFrame.IsEmpty())
		{
			CSWImage* pImage = m_lstFrame.RemoveHead();	
			SAFE_RELEASE(pImage);
		}
		return E_OUTOFMEMORY;
	}
	
	PBYTE pbBuf = (PBYTE)pcMemBuf->GetBuffer();

	swpa_memset(pbBuf, 0, iBufSize);

	BYTE * pbAddr = pbBuf;
	CSWImage* pFirstImage = NULL;
	SW_COMPONENT_IMAGE sImageComponent;
	swpa_memset(&sImageComponent, 0, sizeof(sImageComponent));
	while (!m_lstFrame.IsEmpty())
	{
		CSWImage* pImage = m_lstFrame.RemoveHead();
		if (NULL == pImage)
		{
			continue;
		}

		if (pFirstImage == NULL)
		{
			pFirstImage = pImage;
			pFirstImage->GetImage(&sImageComponent);
			SAFE_ADDREF(pFirstImage);
		}


		INT iFrameType = (0 == swpa_strcmp(pImage->GetFrameName(), "IFrame")) ? 4096 : 4097;//'4097' means "HISTORY P-FRAME"
		INT iFrameSize = pImage->GetSize();
		
		swpa_memcpy(pbAddr, &iFrameType, sizeof(iFrameType));
		pbAddr += sizeof(iFrameType);
		
		swpa_memcpy(pbAddr, &iFrameSize, sizeof(iFrameSize));
		pbAddr += sizeof(iFrameSize);

		
		SW_COMPONENT_IMAGE sComponent;
		pImage->GetImage(&sComponent);

		swpa_memcpy(pbAddr, sComponent.rgpbData[0], pImage->GetSize());
		pbAddr += pImage->GetSize();
		

		SAFE_RELEASE(pImage);

	}


	if (NULL == pFirstImage)
	{
		SW_TRACE_DEBUG("Err: no valid h264 frame\n");
		m_pcMemoryFactory->Free(pcMemBuf);
		return E_INVALIDARG;
	}
		
	CSWImage * pNewImage = NULL;		
	HRESULT hr = CSWImage::CreateSWImage(&pNewImage, 
		sImageComponent.cImageType, 
		sImageComponent.iWidth, 
		sImageComponent.iHeight,
		m_pcMemoryFactory, 
		pFirstImage->GetFrameNo(), 
		pFirstImage->GetRefTime(), 
		FALSE,
		pFirstImage->GetFrameName(),
		iBufSize);
	if (S_OK != hr || NULL == pNewImage)
	{
		SW_TRACE_DEBUG("Err: failed to CreateSWImage() [0x%x]\n", hr);
		m_pcMemoryFactory->Free(pcMemBuf);
		SAFE_RELEASE(pFirstImage);
		return E_FAIL;
	}

	SAFE_RELEASE(pFirstImage);
	
	pNewImage->GetImage(&sImageComponent);

	swpa_memcpy(sImageComponent.rgpbData[0], (VOID*)pbBuf, iBufSize);
	sImageComponent.iSize = iBufSize;
	
	pNewImage->SetImage(sImageComponent);

	m_pcMemoryFactory->Free(pcMemBuf);	

	
	CSWDateTime FileTime(pNewImage->GetRefTime());
	//SW_TRACE_DEBUG("Info: enqueuing h264 video file (time %u.%u)\n", sImageInfo.sTimeStamp.sec, sImageInfo.sTimeStamp.msec);
	// SW_TRACE_DEBUG("Info: enqueuing h264 video file (time: %d/%d/%d/%d:%d:%d)\n", 
	// 		FileTime.GetYear(), FileTime.GetMonth(), FileTime.GetDay(),
	// 		FileTime.GetHour(), FileTime.GetMinute(), FileTime.GetSecond());	


	m_cFileLock.Pend();
	if (m_lstFile.IsFull())
	{
//		SW_TRACE_DEBUG("Warning: Save Queue is full! discards the earlist data\n");

		CSWImage* pImage = m_lstFile.RemoveHead();
		SAFE_RELEASE(pImage);//todo: only release?

		m_lstFile.AddTail(pNewImage);
		
	}
	else
	{
		m_lstFile.AddTail(pNewImage);
	}
	
	m_cFileLock.Post();
	
	return S_OK;
}


HRESULT CSWH264QueueRenderFilter::Receive(CSWObject * pObj)
{
	
	if (!m_fInited)
	{
		SW_TRACE_DEBUG("Err: not initialized yet\n");
		return E_NOTIMPL;
	}

	if (IsDecendant(CSWImage, pObj))
	{
		//SW_TRACE_DEBUG("Info: received an h264 frame\n");

		m_cObjLock.Pend();
		if (!m_lstObj.IsFull())
		{
			SAFE_ADDREF(pObj);
			m_lstObj.AddTail(pObj);

			m_cSemaObjSync.Post();
		}
		m_cObjLock.Post();
	}

	return S_OK;
}



HRESULT CSWH264QueueRenderFilter::GetImageTimeStamp(CSWImage * pImage, SWPA_TIME* pTimeStamp)
{
	CSWDateTime objTime(pImage->GetRefTime());
	
	if (NULL == pImage || NULL == pTimeStamp)
	{
		SW_TRACE_DEBUG("Err: NULL == pImage || NULL == pTimeStamp\n");
		return E_INVALIDARG;
	}

	DWORD dwTimeHigh = 0;
	DWORD dwTimeLow  = 0;

	pImage->GetRealTime(&dwTimeHigh, &dwTimeLow);

	//SW_TRACE_DEBUG("Info: dwTimeHigh = %lu, dwTimeLow = %lu\n", dwTimeHigh, dwTimeLow);
	
	if (0 == dwTimeHigh && 0 == dwTimeLow)
	{
		
		return objTime.GetTime(pTimeStamp);
	}
	else
	{
		return CSWDateTime::TimeConvert(dwTimeHigh, dwTimeLow, pTimeStamp);
	}
}




HRESULT CSWH264QueueRenderFilter::ClearBuffer()
{
	
	while (!m_lstFrame.IsEmpty())
	{
		CSWImage * pImage = m_lstFrame.RemoveHead();
		SAFE_RELEASE(pImage);
	}
	
	//free the file list
	m_cFileLock.Pend();
	while (!m_lstFile.IsEmpty())
	{
		CSWImage * pcFile = m_lstFile.RemoveHead();
		SAFE_RELEASE(pcFile);
	}	
	m_cFileLock.Post();

	return S_OK;
}




HRESULT CSWH264QueueRenderFilter::OnGetH264(WPARAM wParam, LPARAM lParam)
{
	DWORD dwTriggerTick = (DWORD)wParam;
	CSWImage** pH264Image = (CSWImage**)lParam;
	if (NULL == lParam)
	{
		SW_TRACE_DEBUG("Err: invalid arg lParam\n");
		return E_INVALIDARG;
	}
	
	* pH264Image = NULL;
	
	CSWDateTime cTime(dwTriggerTick);
	SWPA_TIME sTime;
	cTime.GetTime(&sTime);

	// SW_TRACE_DEBUG("Info: OnGetH264, time:%04d-%02d-%02d %02d:%02d:%02d %03d\n",
	// 	cTime.GetYear(), cTime.GetMonth(), cTime.GetDay(),
	// 	cTime.GetHour(), cTime.GetMinute(), cTime.GetSecond(), cTime.GetMSSecond());
	
	if( m_fInited )
	{
		m_cFileLock.Pend();

		SW_POSITION pos = m_lstFile.GetHeadPosition();
		while (m_lstFile.IsValid(pos))
		{
			CSWImage * pImage = m_lstFile.GetNext(pos);
			if (NULL != pImage)
			{

				CSWDateTime cH264Time(pImage->GetRefTime());
				SWPA_TIME sH264Time;
				cH264Time.GetTime(&sH264Time);

				if (sH264Time.sec == sTime.sec)
				{
					//SAFE_ADDREF(pImage);
					
					*pH264Image = pImage;
					break;
				}
			}
		}
		
		m_cFileLock.Post();
	}

	if (NULL == *pH264Image)
	{
		SW_TRACE_DEBUG("Err: failed to get video.\n");
		return E_FAIL;
	}

	return S_OK;
}




