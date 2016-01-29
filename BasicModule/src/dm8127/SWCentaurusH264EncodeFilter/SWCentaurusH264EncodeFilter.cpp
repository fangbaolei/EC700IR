/**
*@file 
*@brief
*
*/

#include "SWFC.h"
#include "SWMessage.h"
#include "SWFilterStruct.h"

#include "SWCentaurusH264EncodeFilter.h"

CSWCentaurusH264EncodeFilter::CSWCentaurusH264EncodeFilter()
	: CSWBaseFilter(1,1), CSWMessage(MSG_SET_H264_START, MSG_SET_H264_END)
	, m_fInited(FALSE)
	, m_dwQueueSize(30)
	, m_iChannelId(0)
	, m_pSemaLock(NULL)
{
	GetIn(0)->AddObject(CLASSID(CSWImage));
	GetOut(0)->AddObject(CLASSID(CSWImage));
	/*float fltTmp = 220.0 / 256.0;
	float fltValue = 0.0;

	char szMsg[2048];
	char szTemp[32];
	for(int i = 0; i < 256; ++i)
	{
		fltValue = (i * fltTmp);
		fltValue += (16 + 0.5);
		m_rgbData[i] = fltValue;
		sprintf(szTemp, "<%02d>", (int)m_rgbData[i]);
		strcat(szMsg, szTemp);
		if( i % 16 == 0 && i != 0 )
		{
			//SW_TRACE_DEBUG(szMsg);
			szMsg[0] = 0;
		}
	}*/
	//SW_TRACE_DEBUG(szMsg);
	m_pSemaLock = new CSWSemaphore(1, 1);

}

CSWCentaurusH264EncodeFilter::~CSWCentaurusH264EncodeFilter()
{
	//clear the callback 
	swpa_ipnc_setcallback(SWPA_LINK_VIDEO, m_iChannelId, NULL, NULL);
	
	//m_mutexLock.Lock();
	if (NULL != m_pSemaLock)
	{
		m_pSemaLock->Pend();
		while (!m_lstH264Frame.IsEmpty())
		{
			CSWImage * pImage = m_lstH264Frame.RemoveHead();
			SAFE_RELEASE(pImage);
		}
		m_pSemaLock->Post();

		SAFE_RELEASE(m_pSemaLock);
	}
	//m_mutexLock.Unlock();
	m_fInited = FALSE;
}



HRESULT CSWCentaurusH264EncodeFilter::Initialize(PVOID pvParam)
{
	H264_FILTER_PARAM* pParam = (H264_FILTER_PARAM*)pvParam;

	
	if (NULL == m_pSemaLock)
	{
		SW_TRACE_NORMAL("Err: no mem for m_pSemaLock.\n");
		return E_OUTOFMEMORY;
	}


	if (NULL == pParam)
	{
		SW_TRACE_NORMAL("Err: pParam(0x%x) is invalid.\n", pParam);
		return E_INVALIDARG;
	}

	m_lstH264Frame.SetMaxCount(m_dwQueueSize);

	swpa_memset(&m_sH264Param, 0, sizeof(m_sH264Param));
	swpa_memcpy(&m_sH264Param, &pParam->cH264Param, sizeof(m_sH264Param));
	m_iChannelId = pParam->iChannelNum;

	SW_TRACE_DEBUG("H264 resolution(%d).\n", m_sH264Param.iResolution);
	if (FAILED(SetResolution(m_sH264Param.iResolution,m_iChannelId)))
	{
		SW_TRACE_DEBUG("Err: failed to save H264 resolution(%d).\n", m_sH264Param.iResolution);
		return E_FAIL;
	}

	//set bitrate
	if (0 >= m_sH264Param.iTargetBitrate)	
	{
		SW_TRACE_NORMAL("Err: H264 bitrate(%d) is invalid.\n", m_sH264Param.iTargetBitrate);
		return E_INVALIDARG;
	}
	SW_TRACE_DEBUG("Info: setting %s H264 bitrate to %dKbps...\n", 
		(SWPA_VPSS_H264_CHANNEL == pParam->iChannelNum)? "first":"second",m_sH264Param.iTargetBitrate);
	INT iBitRate = m_sH264Param.iTargetBitrate << 10;	//Kbps -> bps

	if (SWPAR_OK != swpa_ipnc_control(SWPA_LINK_VIDEO
		, (SWPA_VPSS_H264_CHANNEL == pParam->iChannelNum) ? CMD_SET_H264_BITRATE : CMD_SET_H264_BITRATE2
		, &iBitRate, 4, 1000))
	{
		SW_TRACE_NORMAL("Err: failed to set %s H264 bitrate(%d)\n",
			(SWPA_VPSS_H264_CHANNEL == pParam->iChannelNum)? "first":"second", m_sH264Param.iTargetBitrate);
		return E_FAIL;
	}

	//set I-Frame interval
	if (SWPA_VPSS_H264_CHANNEL == pParam->iChannelNum)
	{
		if (0 >= m_sH264Param.intraFrameInterval)
		{
			SW_TRACE_NORMAL("Err: H264 intraFrameInterval(%d) is invalid.\n", m_sH264Param.intraFrameInterval);
			return E_INVALIDARG;
		}
		SW_TRACE_DEBUG("Info: setting H264 intraFrameInterval to %d...\n", m_sH264Param.intraFrameInterval);

		if (SWPAR_OK != swpa_ipnc_control(SWPA_LINK_VIDEO, CMD_SET_H264_IFRAME_INTERVAL, &m_sH264Param.intraFrameInterval, 4, 1000))
		{
			SW_TRACE_NORMAL("Err: failed to set H264 intraFrameInterval(%d)\n", m_sH264Param.intraFrameInterval);
			return E_FAIL;
		}
	}
	else
	{
		if (0 >= m_sH264Param.intraFrameInterval)
		{
			SW_TRACE_NORMAL("Err: H264 intraFrameInterval(%d) is invalid.\n", m_sH264Param.intraFrameInterval);
			return E_INVALIDARG;
		}
		SW_TRACE_DEBUG("Info: setting H264 second intraFrameInterval to %d...\n", m_sH264Param.intraFrameInterval);

		if (SWPAR_OK != swpa_ipnc_control(SWPA_LINK_VIDEO, CMD_SET_H264_IFRAME_INTERVAL2, &m_sH264Param.intraFrameInterval, 4, 1000))
		{
			SW_TRACE_NORMAL("Err: failed to set H264 second intraFrameInterval(%d)\n", m_sH264Param.intraFrameInterval);
			return E_FAIL;
		}
	}

	//set rate control
	if (SWPA_VPSS_H264_CHANNEL == pParam->iChannelNum)
	{
		OnSetH264RateControl((WPARAM)m_sH264Param.iRateControl, 0);
	}
	else
	{
		OnSetH264SecondRateControl((WPARAM)m_sH264Param.iRateControl, 0);
	}

	//register data callback function
	SW_TRACE_DEBUG("set call back channel num %d \n",pParam->iChannelNum);
	swpa_ipnc_setcallback(SWPA_LINK_VIDEO, pParam->iChannelNum, PackH264FrameCallback, this);

	if (FAILED(m_semaFrame.Create(0, m_dwQueueSize)))
	{
		SW_TRACE_NORMAL("Err: failed to create semaphore\n");
		return E_FAIL;
	}
	
	m_fInited = TRUE;

	return S_OK;
}

HRESULT CSWCentaurusH264EncodeFilter::Receive(CSWObject* obj)
{
	if (IsDecendant(CSWImage, obj))
	{
		CSWImage* pImage = (CSWImage *)obj;

		if (NULL == pImage)
		{
			return E_INVALIDARG;
		}

		if (pImage->IsCaptureImage())
		{
			/*H264码流不处理抓拍图*/
			SW_TRACE_DEBUG("Capture image is drop in h264 encode filter!\n");
			return S_OK;
		}

		pImage->GetImageExtInfo(&m_cExtInfo);

/*
		// 0~255 ==> 16~235
		// todo. YUV420SP
		SW_COMPONENT_IMAGE cImageInfo;
		pImage->GetImage(&cImageInfo);
		int iWidth = pImage->GetWidth();
		int iHeight = pImage->GetHeight();
		unsigned char* pbY = cImageInfo.rgpbData[0];
		unsigned char* pbUV = cImageInfo.rgpbData[1];

		DWORD dwTick = CSWDateTime::GetSystemTick();
		unsigned char* pbYDest = pbY;
		unsigned char* pbUVDest = pbUV;
		for(int i = 0; i < iHeight; ++i)
		{
			pbYDest = pbY + cImageInfo.rgiStrideWidth[0] * i;
			for(int j = 0; j < iWidth; ++j)
			{
				pbYDest[j] = m_rgbData[pbYDest[j]];
			}
		}
		for(int i = 0; i < iHeight / 2; ++i)
		{
			pbUVDest = pbUV + cImageInfo.rgiStrideWidth[1] * i;
			for(int j = 0; j < iWidth; ++j)
			{
				pbUVDest[j] = m_rgbData[pbUVDest[j]];
			}
		}
		//SW_TRACE_DEBUG("<123h264>relay:%d.", CSWDateTime::GetSystemTick() - dwTick);
*/
			
		if (SWPAR_OK != swpa_ipnc_send_data(SWPA_LINK_VIDEO, pImage->GetImageBuffer(), pImage->GetSize(), NULL, 0))
		{
			SW_TRACE_NORMAL("Err: failed to send H264 YUV image data.\n");
			return E_FAIL;
		}
	}
	
	return S_OK;
}

HRESULT CSWCentaurusH264EncodeFilter::Run()
{
	if (!IsInited())
	{
		SW_TRACE_NORMAL("Err: CSWCentaurusH264EncodeFilter not inited yet...\n");
		return E_OBJ_NO_INIT;
	}

	if (FAILED(CSWBaseFilter::Run()))
	{
		SW_TRACE_NORMAL("Err: failed to run CSWCentaurusH264EncodeFilter\n");
		return E_FAIL;
	}

	if (FAILED(m_cSendThread.Start(SendH264Proxy, this)))
	{
		SW_TRACE_NORMAL("Err: failed to start send thread\n");
		return E_FAIL;
	}
	
	return S_OK;
}

HRESULT CSWCentaurusH264EncodeFilter::Stop()
{
	if (!IsInited())
	{
		SW_TRACE_DEBUG("Info: CSWCentaurusH264EncodeFilter not inited yet...\n");
		return S_OK;
	}

	if (FAILED(CSWBaseFilter::Stop()))
	{
		SW_TRACE_DEBUG("Err: failed to stop CSWCentaurusH264EncodeFilter \n");
		return E_FAIL;
	}

	if (FAILED(m_cSendThread.Stop()))
	{
		SW_TRACE_DEBUG("Err: failed to stop SendThread \n");
		return E_FAIL;
	}
	
	return S_OK;
}



HRESULT CSWCentaurusH264EncodeFilter::SendH264()
{
	if (!IsInited())
	{
		SW_TRACE_NORMAL("Err: CSWCentaurusH264EncodeFilter not inited yet...\n");
		return E_OBJ_NO_INIT;
	}
	SW_TRACE_DEBUG("Running ..........\n");

	while (FILTER_RUNNING == GetState())
	{
		if (SUCCEEDED(m_semaFrame.Pend(200)))
		{
			//m_mutexLock.Lock();
			m_pSemaLock->Pend();
			CSWImage * pImage = m_lstH264Frame.RemoveHead();
			//m_mutexLock.Unlock();
			m_pSemaLock->Post();

			Deliver(pImage);
			
			if (SWPA_VPSS_H264_CHANNEL == m_iChannelId)			//只统计主路H264的
				PrintRunningInfo(pImage);
			
			SAFE_RELEASE(pImage);
		}
	}

	return S_OK;
}


PVOID CSWCentaurusH264EncodeFilter::SendH264Proxy(PVOID pvArg)
{
	if (NULL != pvArg)
	{
		CSWCentaurusH264EncodeFilter * pThis = (CSWCentaurusH264EncodeFilter*)pvArg;
		pThis->SendH264();
	}

	return NULL;
}



INT CSWCentaurusH264EncodeFilter::PackH264Frame(VOID *pvDataStruct)
{
	IMAGE *pImageInfo = (IMAGE *)pvDataStruct;

	if (NULL == pImageInfo)
	{
		SW_TRACE_NORMAL("Err: pImage(0x%x) is invalid.\n", pImageInfo);
		return 0;
	}

	if (SWPA_IMAGE_H264 != pImageInfo->type) //H264
	{
		SW_TRACE_NORMAL("Err: Image type(%d) is invalid.\n", pImageInfo->type);
		return 0;
	}
	
	// 如果非运行状态则不处理
	if( GetState() != FILTER_RUNNING )
	{
		return 0;
	}

	static INT iFrameNum = 0;
	CSWImage* pImage = NULL;
	if ( S_OK == CSWImage::CreateSWImage(&pImage, 
			SW_IMAGE_H264, 
			pImageInfo->width, 
			pImageInfo->height, 
			CSWMemoryFactory::GetInstance(SW_SHARED_MEMORY), 
			++iFrameNum, 
			CSWDateTime::GetSystemTick(), 
			FALSE, 
			pImageInfo->frameType == 0 ? "IFrame" : "PFrame",
			pImageInfo->data.size)
		)
	{
		SW_COMPONENT_IMAGE cImage;
		pImage->GetImage(&cImage);
		cImage.iSize = pImageInfo->data.size;
		swpa_memcpy(cImage.rgpbData[0], pImageInfo->data.addr[0], pImageInfo->data.size);
		pImage->SetImage(cImage);
					
		
		pImage->SetImageExtInfo(m_cExtInfo);
		//m_mutexLock.Lock();
		m_pSemaLock->Pend();
		if (m_lstH264Frame.IsFull())
		{
			SW_TRACE_NORMAL("Err: CSWCentaurusH264EncodeFilter H264 Queue is full, discards this frame!\n");
			//m_mutexLock.Unlock();
			m_pSemaLock->Post();
		}
		else
		{
			SAFE_ADDREF(pImage);
			m_lstH264Frame.AddTail(pImage);
			//m_mutexLock.Unlock();
			m_pSemaLock->Post();

			m_semaFrame.Post();
		}
		SAFE_RELEASE(pImage);
	}
	else
	{
		SW_TRACE_NORMAL("Err: failed to create image...\n");
	}
	
	return 0;
}

INT CSWCentaurusH264EncodeFilter::PackH264FrameCallback(VOID *pvContext, INT iType, VOID *pvDataStruct)
{
	CSWCentaurusH264EncodeFilter* pThis = (CSWCentaurusH264EncodeFilter *)pvContext;
	
	if (iType == CALLBACK_TYPE_IMAGE)
	{
		return pThis->PackH264Frame(pvDataStruct);
	}
	return 0;
}


HRESULT CSWCentaurusH264EncodeFilter::PrintRunningInfo(CSWImage* pImage)
{
	static DWORD dwFps = 0;
	static DWORD dwBeginTick = CSWDateTime::GetSystemTick();

	static DWORD dwIPTotal = 0;
	static DWORD dwIPSize = 0;
	static DWORD dwIPCount = 0;
	static DWORD dwIPFrameCount = 0;
	static BOOL fIsReady = FALSE;

	if (NULL == pImage)
	{
		SW_TRACE_NORMAL("Err: pImage == NULL\n");
		return E_INVALIDARG;
	}

	if (0 == swpa_strcmp(pImage->GetFrameName(), "IFrame"))
	{
		if ( dwIPCount > 0 )
		{
			dwIPTotal += dwIPSize;
		}
		dwIPSize = pImage->GetSize();
		dwIPCount++;
		fIsReady = (dwIPFrameCount == 0 ? FALSE : TRUE);
		dwIPFrameCount = fIsReady ? dwIPFrameCount : 1;
	}
	else
	{
		if ( !fIsReady && dwIPFrameCount > 0)
		{
			dwIPFrameCount++;
		}
		dwIPSize += pImage->GetSize();
	}

	// print fps
	if ( dwFps++ >= 100 )
	{
		dwIPCount--;
		dwIPTotal = dwIPCount > 0 ? dwIPTotal / dwIPCount / 1024 : dwIPTotal / 1024;
		
		DWORD dwCurTick = CSWDateTime::GetSystemTick();

		CHAR szInfo[256] = {0};
		CHAR szMsg[256] = {0};
		swpa_sprintf(szInfo, "h264 fps: %.1f w:%d,h:%d IP size:%dKB IP:1:%d.", 
			float(100*1000) / (dwCurTick - dwBeginTick), pImage->GetWidth(), pImage->GetHeight(), dwIPTotal, dwIPFrameCount);
		SW_TRACE_NORMAL("--- %s ---", szInfo);
		swpa_sprintf(szMsg, "H264压缩:%s", szInfo);
		CSWMessage::SendMessage(MSG_APP_UPDATE_STATUS, (WPARAM)szMsg, 0);

		dwBeginTick = dwCurTick;
		dwFps = dwIPCount = dwIPTotal = dwIPSize = dwIPFrameCount = 0;
		fIsReady = FALSE;
	}

	return S_OK;	
}



HRESULT CSWCentaurusH264EncodeFilter::SetResolution(INT iResolution,INT ChannelNum )
{

	
	if(SWPA_VPSS_H264_CHANNEL == ChannelNum)
	{
		if (720 == iResolution || 1080 == iResolution)
		{
			CSWFile cH264ResolutionFile;
			if (FAILED(cH264ResolutionFile.Open("/tmp/.H264ResolutionInfo", "w")))
			{
				SW_TRACE_DEBUG("Err: failed to open H.264 resolution file\n");
				return E_FAIL;
			}
			else 
			{
				if (FAILED(cH264ResolutionFile.Write(&iResolution, sizeof(iResolution), NULL)))
				{
					SW_TRACE_DEBUG("Err: failed to save H.264 resolution file\n");
					return E_FAIL;
				}
			}

			cH264ResolutionFile.Close();
			
			return S_OK;
		}
		else
		{
			SW_TRACE_NORMAL("Err: invalid H.264 resolution: %d\n", iResolution);
			return E_INVALIDARG;
		}
	}

	/*ChannelNum表示通道号，不是通道的个数*/
	if(SWPA_VPSS_H264_SECOND_CHANNEL == ChannelNum )	//双码流的时候ChannelNum才会等于2，不然是不会等于2
	{
	//	printf("ChannelNum=2\n");
		if (480 == iResolution || 576 == iResolution ||720 == iResolution || 1080 == iResolution)
		{
			CSWFile cH264ResolutionFile_EX;
			if (FAILED(cH264ResolutionFile_EX.Open("/tmp/.H264ResolutionInfo_Ex", "w")))
				{
					SW_TRACE_DEBUG("Err: failed to open H.264 resolution_Ex file\n");
					return E_FAIL;
				}
				else 
				{
					if (FAILED(cH264ResolutionFile_EX.Write(&iResolution, sizeof(iResolution), NULL)))
					{
						SW_TRACE_DEBUG("Err: failed to save H.264 resolution file\n");
						return E_FAIL;
					}
				}

				cH264ResolutionFile_EX.Close();
				
				return S_OK;
		}
		else
		{
			SW_TRACE_NORMAL("Err: invalid H.264 resolution: %d\n", iResolution);
			return E_INVALIDARG;
		}
	
	}

		
}


HRESULT CSWCentaurusH264EncodeFilter::OnSetH264BitRate(WPARAM wParam, LPARAM lParam)
{
	INT iBitRate = (INT)wParam << 10; //Kbps -> bps
	swpa_ipnc_control(SWPA_LINK_VIDEO, CMD_SET_H264_BITRATE, &iBitRate, sizeof(INT));
	SW_TRACE_DEBUG("CSWH264OverlayFilter::OnSetH264BitRate %d", (INT)wParam);
	return S_OK;
}

HRESULT CSWCentaurusH264EncodeFilter::OnGetH264BitRate(WPARAM wParam, LPARAM lParam)
{
	INT iBitRate = 0;
	INT *pParam = (INT *)lParam;
	swpa_ipnc_control(SWPA_LINK_VIDEO, CMD_GET_H264_BITRATE, &iBitRate, sizeof(INT));
	SW_TRACE_DEBUG("CSWH264OverlayFilter::OnGetH264BitRate(0x%08x, 0x%08x)", wParam, lParam);
	pParam[0] = iBitRate >> 10; //bps -> Kbps
	
	return S_OK;
}

HRESULT CSWCentaurusH264EncodeFilter::OnSetH264SecondBitRate(WPARAM wParam, LPARAM lParam)
{
	INT iBitRate = (INT)wParam << 10; //Kbps -> bps
	swpa_ipnc_control(SWPA_LINK_VIDEO, CMD_SET_H264_BITRATE2, &iBitRate, sizeof(INT));
	SW_TRACE_DEBUG("CSWCentaurusH264EncodeFilter::OnSetH264SecondBitRate %d", (INT)wParam);
	return S_OK;
}

HRESULT CSWCentaurusH264EncodeFilter::OnGetH264SecondBitRate(WPARAM wParam, LPARAM lParam)
{
	INT iBitRate = 0;
	INT *pParam = (INT *)lParam;
	swpa_ipnc_control(SWPA_LINK_VIDEO, CMD_GET_H264_BITRATE2, &iBitRate, sizeof(INT));
	SW_TRACE_DEBUG("CSWCentaurusH264EncodeFilter::OnGetH264BitRate(0x%08x, 0x%08x)", wParam, lParam);
	pParam[0] = iBitRate >> 10; //bps -> Kbps
	
	return S_OK;
}

HRESULT CSWCentaurusH264EncodeFilter::OnSetH264IFrameInterval(WPARAM wParam, LPARAM lParam)
{
	INT iInterval = (INT)wParam;
	swpa_ipnc_set_iframe_interval(SWPA_VPSS_H264_CHANNEL, iInterval);
	SW_TRACE_DEBUG("CSWCentaurusH264EncodeFilter::OnSetH264IFrame");
	return S_OK;
}

HRESULT CSWCentaurusH264EncodeFilter::OnSetH264SecondIFrameInterval(WPARAM wParam, LPARAM lParam)
{
	INT iInterval = (INT)wParam;
	swpa_ipnc_set_iframe_interval(SWPA_VPSS_H264_SECOND_CHANNEL, iInterval);
	SW_TRACE_DEBUG("CSWCentaurusH264EncodeFilter::OnSetH264SecondIFrame");
	return S_OK;
}

HRESULT CSWCentaurusH264EncodeFilter::OnGetH264IFrameInterval(WPARAM wParam, LPARAM lParam)
{
	INT *pInterval = (INT *)lParam;
	swpa_ipnc_get_iframe_interval(SWPA_VPSS_H264_CHANNEL, pInterval);
	SW_TRACE_DEBUG("CSWCentaurusH264EncodeFilter::OnGetH264IFrame");
	return S_OK;
}

HRESULT CSWCentaurusH264EncodeFilter::OnGetH264SecondIFrameInterval(WPARAM wParam, LPARAM lParam)
{
	INT *pInterval = (INT *)lParam;
	swpa_ipnc_get_iframe_interval(SWPA_VPSS_H264_SECOND_CHANNEL, pInterval);
	SW_TRACE_DEBUG("CSWCentaurusH264EncodeFilter::OnGetH264SecondIFrame");
	return S_OK;
}


HRESULT CSWCentaurusH264EncodeFilter::OnSetH264RateControl(WPARAM wParam, LPARAM lParam)
{
	INT iRateControl = (INT)wParam;
	iRateControl = (iRateControl < 0) ? 0 : ((iRateControl > 1) ? 1 : iRateControl);
	swpa_ipnc_set_rate_control(SWPA_VPSS_H264_CHANNEL, iRateControl);
	SW_TRACE_DEBUG("CSWCentaurusH264EncodeFilter::OnSetH264RateControl %d", iRateControl);
	return S_OK;
}

HRESULT CSWCentaurusH264EncodeFilter::OnSetH264SecondRateControl(WPARAM wParam, LPARAM lParam)
{
	INT iRateControl = (INT)wParam;
	iRateControl = (iRateControl < 0) ? 0 : ((iRateControl > 1) ? 1 : iRateControl);
	swpa_ipnc_set_rate_control(SWPA_VPSS_H264_SECOND_CHANNEL, iRateControl);
	SW_TRACE_DEBUG("CSWCentaurusH264EncodeFilter::OnSetH264SecondRateControl %d", iRateControl);
	return S_OK;
}

HRESULT CSWCentaurusH264EncodeFilter::OnSetH264VbrDuration(WPARAM wParam, LPARAM lParam)
{
	INT iDuration = (INT)wParam;
	swpa_ipnc_set_vbr_duration(SWPA_VPSS_H264_CHANNEL, iDuration);
	SW_TRACE_DEBUG("CSWCentaurusH264EncodeFilter::OnSetH264VbrDuration");
	return S_OK;
}

HRESULT CSWCentaurusH264EncodeFilter::OnSetH264SecondVbrDuration(WPARAM wParam, LPARAM lParam)
{
	INT iDuration = (INT)wParam;
	swpa_ipnc_set_vbr_duration(SWPA_VPSS_H264_SECOND_CHANNEL, iDuration);
	SW_TRACE_DEBUG("CSWCentaurusH264EncodeFilter::OnSetH264SecondVbrDuration");
	return S_OK;
}

HRESULT CSWCentaurusH264EncodeFilter::OnSetH264VbrSensitivity(WPARAM wParam, LPARAM lParam)
{
	INT iSensitivity = (INT)wParam;
	swpa_ipnc_set_vbr_sensitivity(SWPA_VPSS_H264_CHANNEL, iSensitivity);
	SW_TRACE_DEBUG("CSWCentaurusH264EncodeFilter::OnSetH264VbrSensitivity");
	return S_OK;
}

HRESULT CSWCentaurusH264EncodeFilter::OnSetH264SecondVbrSensitivity(WPARAM wParam, LPARAM lParam)
{
	INT iSensitivity = (INT)wParam;
	swpa_ipnc_set_vbr_sensitivity(SWPA_VPSS_H264_SECOND_CHANNEL, iSensitivity);
	SW_TRACE_DEBUG("CSWCentaurusH264EncodeFilter::OnSetH264SecondVbrSensitivity");
	return S_OK;
}
