// 该文件编码格式必须是WIN936
#include "SWFC.h"
#include "SWCentaurusSourceFilter.h"
#include "SWPosImage.h"
#include "SWMessage.h"

FLOAT	CSWCentaurusSourceFilter::m_fltJpegRawFps = 12.5;
FLOAT	CSWCentaurusSourceFilter::m_fltH264RawFps = 25.0;
FLOAT 	CSWCentaurusSourceFilter::m_fltH264SecondRawFps = 25.0;
INT CSWCentaurusSourceFilter::m_iJpegFrameCount = 0;
INT CSWCentaurusSourceFilter::m_iH264FrameCount = 0;
INT CSWCentaurusSourceFilter::m_iJpegDropCount = 0;
INT CSWCentaurusSourceFilter::m_iH264DropCount = 0;
INT CSWCentaurusSourceFilter::m_iH264SecondFrameCount = 0;
INT CSWCentaurusSourceFilter::m_iAvgY = 0;
INT CSWCentaurusSourceFilter::m_iRawAvgY = 0;
INT CSWCentaurusSourceFilter::m_iCVBSExport = 0;
INT CSWCentaurusSourceFilter::m_iCVBSCropStartX = 600;
INT CSWCentaurusSourceFilter::m_iCVBSCropStartY = 252;

//NEON指令优化
#define WITH_NEON

#ifdef WITH_NEON
#include <arm_neon.h>		//arm neon

static void resize_line (uint8_t * __restrict src1, uint8_t * __restrict dest)
{
	int i;
  	for (i=0; i<1440; i+=16)
  	{
    	// load upper line and add neighbor pixels:
    	uint16x8_t a = vpaddlq_u8 (vld1q_u8 (src1));

    	// divide by 2, convert to char and store:
    	vst1_u8 (dest, vshrn_n_u16 (a, 1));

    	// move pointers to next chunk of data
    	src1+=16;
    	dest+=8;
   	}
}

void DoCrop1080Pto576PNEON(unsigned char* pSrc, unsigned char* pSrcUv, int iStrideWidth, unsigned char* pDst)
{
	int h; 
	unsigned char* pSrc_l = pSrc;
    unsigned char* pDst_l = pDst;
	
	//y
  	for (h = 0; h < 576; h++)
  	{
  	    if (h<18 || h>=558)
        {
            memset(pDst_l, 0, 720);
            pDst_l+=720;
            continue;
        }

        resize_line(pSrc_l + 240,pDst_l);
		pSrc_l += (iStrideWidth+iStrideWidth);
		pDst_l += 720;
  	}
	//uv
	pSrc_l=pSrcUv;
    pDst_l=pDst+720*576;
	unsigned short *pusDet = (unsigned short *)pDst_l;
	for (int i=0;i<288;i++)
    {
        //printf("do i = %d.\n", i);
        if (i<9 || i>=279)
        {
            memset((unsigned char *)pusDet, 0x80, 720);
            pusDet+=720/2;
            continue;
        }
		unsigned short *pusSrc = (unsigned short *)(pSrc_l + 240);
        for (int j=0;j<360;j++)
        {
			*pusDet = *pusSrc;		//uv
			pusDet++;
			pusSrc+=2;   
        }
		pSrc_l += (iStrideWidth+iStrideWidth);
    }
}
#endif

int DoCrop1080Pto576P(unsigned char* pSrc, unsigned char* pSrcUv, int iStrideWidth,unsigned char* pDst)
{
    unsigned int i,j;
    unsigned char* pSrc_l = pSrc;
    unsigned char* pDst_l = pDst;

    unsigned int unYValue = 0;
    int iSrcLine = 240+iStrideWidth+(iStrideWidth-1920);

    //Y
    for (i=0;i<576;i++)
    {
        if (i<18 || i>=558)
        {
            memset(pDst_l, 0, 720);
            pDst_l+=720;
            continue;
        }
        pSrc_l+=240;
        for (j=0;j<720;j++)
        {
        	// 无差值
            //*pDst_l++ = *pSrc_l++;
            //pSrc_l++;

        	// 两点差值
        	unYValue = *pSrc_l + *(pSrc_l + 1);
        	*pDst_l++ = (unYValue >> 1);
			pSrc_l += 2;

        	// 四点差值
        	//iYValue = *pSrc_l + *(pSrc_l + 1)
        	//		+ *(pSrc_l + iSrcLine) + *(pSrc_l + iSrcLine + 1);
        	//*pDst_l++ = (iYValue >> 2);
        	//pSrc_l += 2;
        }
        pSrc_l+=iSrcLine;
    }
    pSrc_l=pSrcUv;
    pDst_l=pDst+720*576;
    //UV
    for (i=0;i<288;i++)
    {
        //printf("do i = %d.\n", i);
        if (i<9 || i>=279)
        {
            memset(pDst_l, 0x80, 720);
            pDst_l+=720;
            continue;
        }
        pSrc_l+=240;
        for (j=0;j<360;j++)
        {
            *pDst_l++ = *pSrc_l++;	//U
            *pDst_l++ = *pSrc_l++;	//V
            pSrc_l+=2;
        }
        pSrc_l+=iSrcLine;
    }
    return 0;
}

int DoCrop1080Pto576PByPos(unsigned char* pSrc, unsigned char* pSrcUv, int iStrideWidth, 
					unsigned int startX, unsigned int startY, unsigned char* pDst)
{
    unsigned int i,j;
    unsigned char* pSrc_l = pSrc;
    unsigned char* pDst_l = pDst;

    if (startX>1200 || startY>504)
        return -1;

        startX = startX - startX%2;
        startY = startY - startY%2;
    //Y
    for (i=0;i<576;i++)
    {
        memcpy(pDst_l, pSrc_l+ iStrideWidth * (startY+i)+startX, 720);
        pDst_l+=720;
    }
    pSrc_l=pSrcUv;
    pDst_l=pDst+720*576;
    //UV
    for (i=0;i<288;i++)
    {
        memcpy(pDst_l, pSrc_l+ iStrideWidth * (startY/2+i)+startX, 720);
        pDst_l+=720;
    }
    return 0;
}

int DoCrop720Pto576P(unsigned char* pSrc, unsigned char* pSrcUv, int iStrideWidth, unsigned char* pDst)
{
    unsigned int i,j;
    unsigned char* pSrc_l = pSrc;
    unsigned char* pDst_l = pDst;
	
	pSrc_l += 72*iStrideWidth;
    //Y
    for (i=0;i<576;i++)
    {
        pSrc_l+=280;
		swpa_memcpy(pDst_l,pSrc_l,720);
		pDst_l += 720;
		pSrc_l += 720;
        pSrc_l += 280+(iStrideWidth-1280);
    }
    pSrc_l = pSrcUv + 72*iStrideWidth/2;
    pDst_l = pDst + 720*576;
    //UV
    for (i=0;i<288;i++)
    {
        pSrc_l+=280;
		swpa_memcpy(pDst_l,pSrc_l,720);
		pDst_l += 720;
		pSrc_l += 720;
        pSrc_l += 280+(iStrideWidth-1280);
    }
    return 0;
}

CSWCentaurusSourceFilter::CSWCentaurusSourceFilter()
    : CSWBaseFilter(0,3)
    , CSWMessage( MSG_SOURCE_BEGIN, MSG_SOURCE_END )
    , m_pThreadH264(NULL)
    , m_pThreadJPEG(NULL)
    , m_fInitialized(FALSE)
    , m_iShutter(0)
    , m_iAGCGain(0)
{

    GetOut(0)->AddObject(CLASSID(CSWImage));
    GetOut(1)->AddObject(CLASSID(CSWImage));
	GetOut(2)->AddObject(CLASSID(CSWImage));
	memset(&m_iRGBGain, 0, sizeof(m_iRGBGain));
}

CSWCentaurusSourceFilter::~CSWCentaurusSourceFilter()
{
	SAFE_RELEASE(m_pThreadH264);
	SAFE_RELEASE(m_pThreadJPEG);
	m_fInitialized = FALSE;
}

HRESULT CSWCentaurusSourceFilter::Initialize(
	INT iCVBSExport
	, INT iCVBSCropStartX
	, INT iCVBSCropStartY)
{

	m_iCVBSExport = iCVBSExport;
	m_iCVBSCropStartX = iCVBSCropStartX;
	m_iCVBSCropStartY = iCVBSCropStartY;

	if (TRUE == m_fInitialized)
	{
		SW_TRACE_NORMAL("Initialize ............");
		return E_FAIL;
	}

	m_pThreadH264 = new CSWThread(this->GetName());
	if (NULL == m_pThreadH264)
	{
		SW_TRACE_NORMAL("Create H264 Thread Failed\n");
		return E_OUTOFMEMORY;
	}
	m_pThreadJPEG = new CSWThread(this->GetName());
	if (NULL == m_pThreadJPEG)
	{
		SW_TRACE_NORMAL("Create JPEG Thread Failed\n");
		return E_OUTOFMEMORY;
	}

	m_cSemImageH264.Create(0, MAX_IMAGE_COUNT);
	m_cSemImageJPEG.Create(0, MAX_IMAGE_COUNT);

	m_fInitialized = TRUE;
	return S_OK;
}


HRESULT CSWCentaurusSourceFilter::Run()
{
    swpa_ipnc_setcallback(SWPA_LINK_VPSS, 0xFF, OnResult,this);

	HRESULT hr = S_OK;
	if( GetState() != FILTER_RUNNING )
	{
		CSWBaseFilter::Run();
		hr = m_pThreadH264->Start((START_ROUTINE)&CSWCentaurusSourceFilter::OnProcessH264Proxy, (PVOID)this);
		if (hr != S_OK)
			return hr;

		hr = m_pThreadJPEG->Start((START_ROUTINE)&CSWCentaurusSourceFilter::OnProcessJPEGProxy, (PVOID)this);
		if (hr != S_OK)
			return hr;
	}

	return hr;
}

HRESULT CSWCentaurusSourceFilter::Stop()
{
	//clear the callback 
	swpa_ipnc_setcallback(SWPA_LINK_VPSS, 0xFF, NULL, NULL);
	
	if (m_pThreadJPEG != NULL)
	{
		m_pThreadJPEG->Stop();
	}
	if (m_pThreadH264 != NULL)
	{
		m_pThreadH264->Stop();
	}

	m_cThreadExtInfo.Stop();
	
    return CSWBaseFilter::Stop();
}

int CSWCentaurusSourceFilter::OnResult(void *pContext, int type, void *struct_ptr)
{
    CSWCentaurusSourceFilter *pThis= (CSWCentaurusSourceFilter *)pContext;

	if (NULL == pContext || NULL == struct_ptr)
	{
		SW_TRACE_NORMAL("Err: pContext/struct_ptr is invalid.\n");
		return 0;
	}

	// 如果非运行状态则不处理
	if( pThis->GetState() != FILTER_RUNNING )
	{
		return 0;
	}

    if(type == CALLBACK_TYPE_IMAGE)
    {
        IMAGE *image = (IMAGE *)struct_ptr;
        CSWImage* pImage = NULL;
        if(S_OK == CSWImage::CreateSWImage(
                    &pImage
                    , SW_IMAGE_YUV420SP
                    , image->pitch
                    , image->width
                    , image->height
                    , CSWMemoryFactory::GetInstance(SW_SHARED_MEMORY)
                    , image->data.addr
                    , image->data.phys
                    , image->data.size
                    , 0
                    , 0
                    , image->isCapture
                    , "VPIF") )
        {
            static INT iFrameNum = 0;
            pImage->SetFrameNo(++iFrameNum);
            pImage->SetRefTime(CSWDateTime::GetSystemTick());

			pImage->SetFlag(image->iCaptureFlag);	//抓拍信息(车道号、抓拍计数等)

			pThis->UpdateExtensionInfo(pImage, image);

			INT nRecognizeChannelNum = SWPA_VPSS_JPEG_CHANNEL;
			INT nIpncMode = swpa_ipnc_mode();
			if (ICX816 == nIpncMode 
				|| ICX816_640W == nIpncMode 
				|| ICX816_600W == nIpncMode 
				|| ICX816_SINGLE == nIpncMode)
			{
				nRecognizeChannelNum = SWPA_VPSS_JPEG_SECOND_CHANNEL;	//未经过降噪的数据通道，延时较小，利于抓拍
			}
            if (nRecognizeChannelNum == image->channel)
            {
            	pThis->m_cMutexJPEGImage.Lock();
				if( pThis->m_lstJPEGImage.GetCount() < MAX_IMAGE_COUNT )
				{
					pImage->AddRef();
					pThis->m_lstJPEGImage.AddTail(pImage);
					if( S_OK != pThis->m_cSemImageJPEG.Post() )
					{
						SW_TRACE_DEBUG("<JPEG>SemImage post failed. list count:%d.\n", pThis->m_lstJPEGImage.GetCount());
					}
				}
				else
				{
					m_iJpegDropCount++;
					//SW_TRACE_NORMAL("<JPEG> m_lstJPEGImage is full!");
				}
				pThis->m_cMutexJPEGImage.Unlock();
				
                m_iJpegFrameCount++;
            }
            else if (SWPA_VPSS_H264_CHANNEL == image->channel)
            {
            	pThis->m_cMutexH264Image.Lock();
				if( pThis->m_lstH264Image.GetCount() < MAX_IMAGE_COUNT )
				{
					pImage->AddRef();
					pThis->m_lstH264Image.AddTail(pImage);
					if( S_OK != pThis->m_cSemImageH264.Post() )
					{
						SW_TRACE_DEBUG("<H264>SemImage post failed. list count:%d.\n", pThis->m_lstH264Image.GetCount());
					}
				}
				else
				{
					m_iH264DropCount++;
					//SW_TRACE_NORMAL("<H264> m_lstH264Image is full!");
				}
				pThis->m_cMutexH264Image.Unlock();
				
                m_iH264FrameCount++;
            }
			else if (SWPA_VPSS_H264_SECOND_CHANNEL == image->channel)
			{
				pThis->GetOut(2)->Deliver(pImage);
				m_iH264SecondFrameCount++;
			}
			else 
			{	
				SW_TRACE_NORMAL("channel id is %d\n",image->channel);
			}

            pImage->Release();
            return 1;
        }
        else
        {
            SW_TRACE_NORMAL("CSWCentaurusSourceFilter::OnResult, failed...");
        }
    }
    return 0;
}


HRESULT CSWCentaurusSourceFilter::UpdateExtensionInfo(CSWImage* pImage, IMAGE *pImageInfo)
{
	if (NULL == pImage || NULL == pImageInfo)
	{
		return E_INVALIDARG;
	}
	
	IMAGE_EXT_INFO cExtInfo;
	pImage->GetImageExtInfo(&cExtInfo);
	cExtInfo.iAvgY = pImageInfo->iAvgY;

	//抓拍图使用抓拍信息
	if(pImageInfo->isCapture)
	{
		cExtInfo.iShutter = (pImageInfo->iCaptureEnableFlag & 0x01) ? pImageInfo->iCaptureShutter : pImageInfo->shutter;
		cExtInfo.iGain = (pImageInfo->iCaptureEnableFlag & 0x02) ? pImageInfo->iCaptureGain : pImageInfo->gain;
		if(pImageInfo->iCaptureEnableFlag & 0x08)
		{
			cExtInfo.iRGain = pImageInfo->iCaptureRGain;
			cExtInfo.iGGain = pImageInfo->iCaptureGGain;
			cExtInfo.iBGain = pImageInfo->iCaptureBGain;
		}
		else
		{
			cExtInfo.iRGain =  pImageInfo->rGain;
			cExtInfo.iGGain =  pImageInfo->gGain;
			cExtInfo.iBGain =  pImageInfo->bGain;
		}
	}
	else
	{
		cExtInfo.iShutter = pImageInfo->shutter;
		cExtInfo.iGain =  pImageInfo->gain;
		cExtInfo.iRGain =  pImageInfo->rGain;
		cExtInfo.iGGain =  pImageInfo->gGain;
		cExtInfo.iBGain =  pImageInfo->bGain;
	}

	if (swpa_ipnc_mode() == DOME_CAMERA)//木星专有
	{
		if ((cExtInfo.iShutter > 30000 || cExtInfo.iShutter < 0)
			|| (cExtInfo.iGain > 255 || cExtInfo.iGain < 0))
		{
			if (S_OK != m_cThreadExtInfo.IsValid())
			{
				if (FAILED(m_cThreadExtInfo.Start(GetExtensionInfo, (PVOID)this)))
				{
					SW_TRACE_DEBUG("Err: failed to start get image extinfo thread.\n");
					return E_FAIL;
				}

				//CSWApplication::Sleep(100); //wait info be updated.
			}
		}

		if (cExtInfo.iShutter > 30000 || cExtInfo.iShutter < 0)
		{
			cExtInfo.iShutter = m_iShutter;
		}
		
		if (cExtInfo.iGain > 255 || cExtInfo.iGain < 0)
		{
			cExtInfo.iGain = m_iAGCGain;
		}
		
		if (cExtInfo.iRGain > 255 || cExtInfo.iRGain < 0)
		{
			cExtInfo.iRGain = m_iRGBGain[0];
		}
		
		if (cExtInfo.iGGain > 255 || cExtInfo.iGGain < 0)
		{
			cExtInfo.iGGain = m_iRGBGain[1];
		}
		
		if (cExtInfo.iBGain > 255 || cExtInfo.iBGain < 0)
		{
			cExtInfo.iBGain = m_iRGBGain[2];
		}
	}

	
	pImage->SetImageExtInfo(cExtInfo);

	return S_OK;
}


PVOID CSWCentaurusSourceFilter::GetExtensionInfo(PVOID pvArg)
{
	CSWCentaurusSourceFilter* pThis = (CSWCentaurusSourceFilter*)pvArg;
	
	while (S_OK == pThis->m_cThreadExtInfo.IsValid() && pThis->GetState() == FILTER_RUNNING)
	{
		static DWORD dwPrevTick = 0, dwCurTick = 0;
		INT iShutter = 0, iAGCGain = 0, iRGBGain[3] = {0};

		dwCurTick = CSWDateTime::GetSystemTick();

		if (dwCurTick > dwPrevTick + 5000)
		{
			if (FAILED(CSWMessage::SendMessage(MSG_GET_SHUTTER, 1, (LPARAM)&iShutter)))
			{
				SW_TRACE_DEBUG("Err: failed to get Shutter value.\n");
			}
			else
			{
				INT iShutterValue[] = {
					1000000/1, 1000000/2, 1000000/3, 1000000/6, 1000000/12, 1000000/25, 1000000/50, 1000000/75, 
					1000000/100, 1000000/120, 1000000/150, 1000000/215, 1000000/300, 1000000/425, 1000000/600, 1000000/1000, 
					1000000/1250, 1000000/1750, 1000000/2500, 1000000/3500, 1000000/6000, 1000000/10000};
				if (iShutter >= 0 && iShutter < sizeof(iShutterValue)/sizeof(iShutterValue[0]))
				{
					pThis->m_iShutter = iShutterValue[iShutter];
				}
				else
				{
					SW_TRACE_DEBUG("Err: shutter value out of range.\n");
				}
			}
			
			if (FAILED(CSWMessage::SendMessage(MSG_GET_AGCGAIN, 1, (LPARAM)&iAGCGain)))
			{
				SW_TRACE_DEBUG("Err: failed to get AGCGain value.\n");
			}
			else
			{
				INT iGainValue[] = {-3, 0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28};
				if (iAGCGain >= 0 && iAGCGain < sizeof(iGainValue)/sizeof(iGainValue[0]))
				{
					pThis->m_iAGCGain = iGainValue[iAGCGain];
				}
				else
				{
					SW_TRACE_DEBUG("Err: AGCGain value out of range.\n");
				}
			}

			if (FAILED(CSWMessage::SendMessage(MSG_GET_RGBGAIN, 1, (LPARAM)pThis->m_iRGBGain)))
			{
				SW_TRACE_DEBUG("Err: failed to get RGBGain value.\n");
			}
			
			
			dwPrevTick = dwCurTick;
		}

		CSWApplication::Sleep(1000);
	}
}


VOID CSWCentaurusSourceFilter::OnProcessH264Proxy(PVOID pvParam)
{
	if(pvParam != NULL)
	{
		CSWCentaurusSourceFilter* pThis = (CSWCentaurusSourceFilter*)pvParam;
		pThis->OnProcessH264();
	}
}

HRESULT CSWCentaurusSourceFilter::OnProcessH264()
{
	SW_TRACE_DEBUG("running ..........");
	while(S_OK == m_pThreadH264->IsValid() && GetState() == FILTER_RUNNING)
	{
		if( S_OK != m_cSemImageH264.Pend(1000) )
		{
			SW_TRACE_DEBUG("m_cSemImageH264.Pend failed.");
			continue;
		}
		m_cMutexH264Image.Lock();
		if( m_lstH264Image.IsEmpty() )
		{
			m_cMutexH264Image.Unlock();
			continue;
		}

		CSWImage* pImage = m_lstH264Image.RemoveHead();
		m_cMutexH264Image.Unlock();

		// H264 要做CVBS处理
		if(swpa_ipnc_mode() == IMX185_CVBSEXPORT || swpa_ipnc_mode() == IMX185_DUALVIDEO_CVBSEXPORT)
		{
			SW_COMPONENT_IMAGE cImageInfo;
			pImage->GetImage(&cImageInfo);
			unsigned char* pbSrcY = cImageInfo.rgpbData[0];
			unsigned char* pbSrcUV = cImageInfo.rgpbData[1];
			unsigned char* pbDet = pbSrcUV + (cImageInfo.rgiStrideWidth[1]*cImageInfo.iHeight/2);
		
			if (1920 == cImageInfo.iWidth){
				if (m_iCVBSExport == 2) {
					// 自定义裁剪模式
					DoCrop1080Pto576PByPos(pbSrcY, pbSrcUV, cImageInfo.rgiStrideWidth[1], m_iCVBSCropStartX, m_iCVBSCropStartY, pbDet);
				} else {
#ifdef WITH_NEON
					DoCrop1080Pto576PNEON(pbSrcY, pbSrcUV, cImageInfo.rgiStrideWidth[1], pbDet);
#else
					DoCrop1080Pto576P(pbSrcY, pbSrcUV,cImageInfo.rgiStrideWidth[1], pbDet);
#endif
				}
			}else if (1280 == cImageInfo.iWidth){
				DoCrop720Pto576P(pbSrcY, pbSrcUV, cImageInfo.rgiStrideWidth[1], pbDet);
			}else{
				SW_TRACE_NORMAL("%d x %d is not suport cvbs export\n",cImageInfo.iWidth,cImageInfo.iHeight);
			}
			
			swpa_ipnc_send_data(SWPA_LINK_VPSS, pbDet, 0, NULL, 0);
		}
		
		GetOut(1)->Deliver(pImage);
		
		pImage->Release();
	}
	return S_OK;
}

VOID CSWCentaurusSourceFilter::OnProcessJPEGProxy(PVOID pvParam)
{
	if(pvParam != NULL)
	{
		CSWCentaurusSourceFilter* pThis = (CSWCentaurusSourceFilter*)pvParam;
		pThis->OnProcessJPEG();
	}
}

HRESULT CSWCentaurusSourceFilter::OnProcessJPEG()
{
	SW_TRACE_DEBUG("running ..........");

	while(S_OK == m_pThreadJPEG->IsValid() && GetState() == FILTER_RUNNING)
	{
		if( S_OK != m_cSemImageJPEG.Pend(1000) )
		{
			SW_TRACE_DEBUG("m_cSemImageJPEG.Pend failed.");
			continue;
		}
		m_cMutexJPEGImage.Lock();
		if( m_lstJPEGImage.IsEmpty() )
		{
			m_cMutexJPEGImage.Unlock();
			continue;
		}
		
		CSWImage* pImage = m_lstJPEGImage.RemoveHead();
		m_cMutexJPEGImage.Unlock();
		
		GetOut(0)->Deliver(pImage);
		
		pImage->Release();
	}
	return S_OK;
}


HRESULT CSWCentaurusSourceFilter::OnGetJpegRawFps(WPARAM wParam,LPARAM lParam)
{
	FLOAT* pfltFps = (FLOAT*)lParam;
	if (NULL == pfltFps)
	{
		SW_TRACE_DEBUG("Err: NULL == pfltFps\n");
		return E_FAIL;
	}

	static DWORD dwLastTick = CSWDateTime::GetSystemTick();
	static INT s_iJpegFrameCountLast = 0;
	DWORD dwCurTick = CSWDateTime::GetSystemTick();
	if (dwCurTick - dwLastTick > 20000)//20s
    {
		m_fltJpegRawFps = float((m_iJpegFrameCount - s_iJpegFrameCountLast)*1000) / (dwCurTick - dwLastTick);

        SW_TRACE_DEBUG("OnGetJpegRawFps jpeg fps: %.1f AvgY:%d, RawAvgY:%d, drop:%d.",
			m_fltJpegRawFps, m_iAvgY, m_iRawAvgY, m_iJpegDropCount);
		
    	dwLastTick = dwCurTick;
		s_iJpegFrameCountLast = m_iJpegFrameCount;
    }

	*pfltFps = m_fltJpegRawFps;
	return S_OK;
}


HRESULT CSWCentaurusSourceFilter::OnGetH264RawFps(WPARAM wParam,LPARAM lParam)
{
	FLOAT* pfltFps = (FLOAT*)lParam;
	if (NULL == pfltFps)
	{
		SW_TRACE_DEBUG("Err: NULL == pfltFps\n");
		return E_FAIL;
	}
	
	static DWORD dwLastTick = CSWDateTime::GetSystemTick();
	static INT s_iH264FrameCountLast = 0;
	DWORD dwCurTick = CSWDateTime::GetSystemTick();
	if (dwCurTick - dwLastTick > 20000)//20s
    {
		m_fltH264RawFps = float((m_iH264FrameCount - s_iH264FrameCountLast)*1000) / (dwCurTick - dwLastTick);

        SW_TRACE_DEBUG("OnGetH264RawFps h264 fps: %.1f, drop:%d",m_fltH264RawFps,m_iH264DropCount);
		
    	dwLastTick = dwCurTick;
		s_iH264FrameCountLast = m_iH264FrameCount;
    }

	*pfltFps = m_fltH264RawFps;
	return S_OK;
}


HRESULT CSWCentaurusSourceFilter::OnGetH264SecondRawFps(WPARAM wParam,LPARAM lParam)
{

	FLOAT* pfltFps = (FLOAT*)lParam;
	if (NULL == pfltFps)
	{
		SW_TRACE_DEBUG("Err: NULL == pfltFps\n");
		return E_FAIL;
	}
	
	static DWORD dwLastTick = CSWDateTime::GetSystemTick();
	static INT s_iH264FrameCountLast = 0;
	DWORD dwCurTick = CSWDateTime::GetSystemTick();
	if (dwCurTick - dwLastTick > 20000)//20s
    {
		m_fltH264SecondRawFps = float((m_iH264SecondFrameCount - s_iH264FrameCountLast)*1000) / (dwCurTick - dwLastTick);

        SW_TRACE_DEBUG("OnGetH264SecondRawFps h264 fps: %.1f",m_fltH264SecondRawFps);
		
    	dwLastTick = dwCurTick;
		s_iH264FrameCountLast = m_iH264SecondFrameCount;
    }

	*pfltFps = m_fltH264SecondRawFps;
	return S_OK;
}


