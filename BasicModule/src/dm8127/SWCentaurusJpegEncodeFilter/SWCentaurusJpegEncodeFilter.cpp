/**
*@file 
*@brief
*
*/

#include "SWFC.h"
#include "SWMessage.h"
#include "SWFilterStruct.h"
#include "SWPosImage.h"

#include "SWCentaurusJpegEncodeFilter.h"


CSWCentaurusJpegEncodeFilter::CSWCentaurusJpegEncodeFilter()
	: CSWBaseFilter(1,1)
	, m_fInited(FALSE)
	, m_dwQueueSize(3)
{

	SW_TRACE_DEBUG("<>SWCentaurusJpegEncodeFilter().\n");
	GetIn(0)->AddObject(CLASSID(CSWImage));
	GetOut(0)->AddObject(CLASSID(CSWPosImage));

	m_pSemaLock = new CSWSemaphore(1, 1);
}

CSWCentaurusJpegEncodeFilter::~CSWCentaurusJpegEncodeFilter()
{
	//clear the callback 
	swpa_ipnc_setcallback(SWPA_LINK_VIDEO, 1, NULL, NULL);

	if (NULL != m_pSemaLock)
	{
		m_pSemaLock->Pend();
		while (!m_lstJpeg.IsEmpty())
		{
			CSWImage * pImage = m_lstJpeg.RemoveHead();
			SAFE_RELEASE(pImage);
		}
		m_pSemaLock->Post();

		SAFE_RELEASE(m_pSemaLock);
	}

	
	m_fInited = FALSE;
}



HRESULT CSWCentaurusJpegEncodeFilter::Initialize(PVOID pvParam)
{
	if (NULL == m_pSemaLock)
	{
		SW_TRACE_NORMAL("Err: no mem for m_pSemaLock.\n");
		return E_OUTOFMEMORY;
	}

	
	m_lstJpeg.SetMaxCount(m_dwQueueSize);

	//register data callback function
	swpa_ipnc_setcallback(SWPA_LINK_VIDEO, 1, PackJpegFrameCallback, this);

	m_fInited = TRUE;

	SW_TRACE_DEBUG("<>SWCentaurusJpegEncodeFilter init.\n");

	return S_OK;
}

HRESULT CSWCentaurusJpegEncodeFilter::Receive(CSWObject* obj)
{
	if (IsDecendant(CSWImage, obj))
	{
		CSWImage* pImage = (CSWImage *)obj;

		if (SWPAR_OK != swpa_ipnc_send_data(SWPA_LINK_VIDEO, pImage->GetImageBuffer(), pImage->GetSize(), NULL, 0))
		{
			SW_TRACE_NORMAL("Err: failed to send JPEG YUV image data.\n");
			return E_FAIL;
		}
	}
	
	return S_OK;
}

HRESULT CSWCentaurusJpegEncodeFilter::Run()
{
	SW_TRACE_DEBUG("<>SWCentaurusJpegEncodeFilter run.\n");
	if (!IsInited())
	{
		SW_TRACE_NORMAL("Err: CSWCentaurusJpegEncodeFilter not inited yet...\n");
		return E_OBJ_NO_INIT;
	}
	
	if (FAILED(CSWBaseFilter::Run()))
	{
		SW_TRACE_NORMAL("Err: failed to run SWCentaurusJpegEncodeFilter\n");
		return E_FAIL;
	}

	if (FAILED(m_cSendThread.Start(SendJpegProxy, this)))
	{
		SW_TRACE_NORMAL("Err: failed to start send thread\n");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CSWCentaurusJpegEncodeFilter::Stop()
{
	if (!IsInited())
	{
		SW_TRACE_DEBUG("Info: SWCentaurusJpegEncodeFilter not inited yet...\n");
		return S_OK;
	}

	if (FAILED(CSWBaseFilter::Stop()))
	{
		SW_TRACE_DEBUG("Err: failed to stop SWCentaurusJpegEncodeFilter \n");
		return E_FAIL;
	}

	if (FAILED(m_cSendThread.Stop()))
	{
		SW_TRACE_DEBUG("Err: failed to stop SendThread \n");
		return E_FAIL;
	}
	
	return S_OK;
}




HRESULT CSWCentaurusJpegEncodeFilter::SendJpeg()
{
	if (!IsInited())
	{
		SW_TRACE_NORMAL("Err: CSWCentaurusJpegEncodeFilter not inited yet...\n");
		return E_OBJ_NO_INIT;
	}
	SW_TRACE_DEBUG("Running ..........\n");

	while (FILTER_RUNNING == GetState())
	{
		if (SUCCEEDED(m_semaJpeg.Pend(200)))
		{
			m_pSemaLock->Pend();
			CSWImage * pImage = m_lstJpeg.RemoveHead();
			m_pSemaLock->Post();

			CSWPosImage* pPosImage = new CSWPosImage(pImage, 0, FALSE);
			if (NULL == pPosImage)
			{
				SW_TRACE_NORMAL("Err: no memory for Jpeg pos image!!\n");			
			}
			else
			{				
				Deliver(pPosImage);
				PrintRunningInfo(pImage);
				SAFE_RELEASE(pPosImage);
			}

			SAFE_RELEASE(pImage);
		}
	}

	return S_OK;
}


PVOID CSWCentaurusJpegEncodeFilter::SendJpegProxy(PVOID pvArg)
{
	if (NULL != pvArg)
	{
		CSWCentaurusJpegEncodeFilter * pThis = (CSWCentaurusJpegEncodeFilter*)pvArg;
		pThis->SendJpeg();
	}

	return NULL;
}




INT CSWCentaurusJpegEncodeFilter::PackJpegFrame(VOID *pvDataStruct)
{
	IMAGE *pImageInfo = (IMAGE *)pvDataStruct;

	if (NULL == pImageInfo)
	{
		SW_TRACE_NORMAL("Err: pImage(0x%x) is invalid.\n", pImageInfo);
		return 0;
	}

	// 如果非运行状态则不处理
	if( GetState() != FILTER_RUNNING )
	{
		return 0;
	}

	if (SWPA_IMAGE_JPEG != pImageInfo->type)
	{
		SW_TRACE_NORMAL("<Jpeg>Err: Image type(%d) is invalid.\n", pImageInfo->type);
		return 0;
	}

	static INT iFrameNum = 0;
	CSWImage* pImage = NULL;
	if ( S_OK == CSWImage::CreateSWImage(&pImage, 
			SW_IMAGE_JPEG, 
			pImageInfo->width, 
			pImageInfo->height, 
			CSWMemoryFactory::GetInstance(SW_SHARED_MEMORY), 
			++iFrameNum, 
			CSWDateTime::GetSystemTick(), 
			FALSE, 
			"JPEG",
			pImageInfo->data.size)
		)
	{
		SW_COMPONENT_IMAGE cImage;
		pImage->GetImage(&cImage);
		cImage.iSize = pImageInfo->data.size;
		swpa_memcpy(cImage.rgpbData[0], pImageInfo->data.addr[0], pImageInfo->data.size);
		pImage->SetImage(cImage);
					
		m_pSemaLock->Pend();
		if (m_lstJpeg.IsFull())
		{
			SW_TRACE_NORMAL("Err: CSWCentaurusJpegEncodeFilter Queue is full, discards this Jpeg!\n");
			m_pSemaLock->Post();
		}
		else
		{
			SAFE_ADDREF(pImage);
			m_lstJpeg.AddTail(pImage);
			m_pSemaLock->Post();

			m_semaJpeg.Post();
		}
		SAFE_RELEASE(pImage);
	}
	else
	{
		SW_TRACE_NORMAL("Err: failed to create Jpeg image...\n");
	}
	
	return 0;
}

INT CSWCentaurusJpegEncodeFilter::PackJpegFrameCallback(VOID *pvContext, INT iType, VOID *pvDataStruct)
{
	CSWCentaurusJpegEncodeFilter* pThis = (CSWCentaurusJpegEncodeFilter *)pvContext;
	
	if (iType == CALLBACK_TYPE_IMAGE)
	{
		return pThis->PackJpegFrame(pvDataStruct);
	}
	return 0;
}

HRESULT CSWCentaurusJpegEncodeFilter::PrintRunningInfo(CSWImage* pImage)
{
	static DWORD dwFps = 0;
	static DWORD dwBeginTick = CSWDateTime::GetSystemTick();

	if (NULL == pImage)
	{
		SW_TRACE_NORMAL("Err: pImage == NULL\n");
		return E_INVALIDARG;
	}

	// print fps
	if ( dwFps++ >= 100 )
	{
		DWORD dwCurTick = CSWDateTime::GetSystemTick();
		CHAR szInfo[256] = {0};
		CHAR szMsg[256] = {0};
		swpa_sprintf(szInfo, "JPEG fps: %.1f w:%d,h:%d.",
			float(100*1000) / (dwCurTick - dwBeginTick), pImage->GetWidth(), pImage->GetHeight());
		SW_TRACE_NORMAL("--- %s ---", szInfo);
		swpa_sprintf(szMsg, "JPEG压缩:%s", szInfo);
		CSWMessage::SendMessage(MSG_APP_UPDATE_STATUS, (WPARAM)szMsg, 0);

		dwBeginTick = dwCurTick;
		dwFps = 0;
	}


	return S_OK;

}

