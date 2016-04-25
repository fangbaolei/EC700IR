///////////////////////////////////////////////////////////
//  SWDspLinkCtrl.cpp
//  Implementation of the Class CSWImage
//  Created on:      
//  Original author: 
///////////////////////////////////////////////////////////
#include "SWFC.h"
#include "SWBaseLinkCtrl.h"
#include "SWDspLinkCtrl.h"
#include "SWSysLinkCtrl.h"
/**
* @brief 构造函数
*/
CSWBaseLinkCtrl::CSWBaseLinkCtrl()
: m_pMemoryData(NULL)
, m_pMemoryRespond(NULL)
, m_pMemoryPoint(NULL)
, m_pMemoryFactory(NULL)
, m_fInitialized(FALSE)
, m_fJpegIsWait(FALSE)
{
	m_cSem.Create(1, 1);
	m_cSemRecog.Create(1, 1);
	memset(m_rgpSmallImage, 0, sizeof(CSWImage*) * MAX_EVENT_COUNT);
	memset(m_rgpBinImage, 0, sizeof(CSWImage*) * MAX_EVENT_COUNT);
}	

/**
* @brief 析构函数
*/
CSWBaseLinkCtrl::~CSWBaseLinkCtrl()
{
}

CSWBaseLinkCtrl* CSWBaseLinkCtrl::GetInstance()
{
	static CSWBaseLinkCtrl * pLinkCtrl = NULL;
#if defined(DM6467)
	static CSWDspLinkCtrl cDspLinkCtrl;
	pLinkCtrl = &cDspLinkCtrl;
#elif defined(DM8127)
	static CSWSysLinkCtrl cSysLinkCtrl;
	pLinkCtrl = &cSysLinkCtrl;
#endif
	if(pLinkCtrl && !pLinkCtrl->IsInitialize())
	{
		SW_TRACE_NORMAL("link ctrl name:%s, initialize....", pLinkCtrl->Name());
		pLinkCtrl->Initialize();
	}
	return pLinkCtrl;
}

HRESULT CSWBaseLinkCtrl::Initialize(VOID)
{
	if(!m_fInitialized)
	{
		m_pMemoryFactory = CSWMemoryFactory::GetInstance(SW_SHARED_MEMORY);
		if( NULL == m_pMemoryFactory )
		{
			return E_OUTOFMEMORY;
		}
  	
		int iMaxSize = MAX_BUFFER_SIZE;
		m_pMemoryData = m_pMemoryFactory->Alloc(iMaxSize);
		if( NULL == m_pMemoryData )
		{
			return E_OUTOFMEMORY;
		}
  	
		m_pMemoryRespond = m_pMemoryFactory->Alloc(iMaxSize);
		if( NULL == m_pMemoryRespond )
		{
			return E_OUTOFMEMORY;
		}
  	
		m_pMemoryPoint = m_pMemoryFactory->Alloc(iMaxSize);
		if( NULL == m_pMemoryPoint )
		{
			return E_OUTOFMEMORY;
		}
		//在这里整合好数据	
		DSPLinkBuffer *buf = (DSPLinkBuffer *)m_pMemoryPoint->GetBuffer();
		//通知DSP端的数据
		buf[0].addr = m_pMemoryData->GetBuffer(SW_NORMAL_MEMORY);
		buf[0].phys = (DWORD)m_pMemoryData->GetBuffer(SW_SHARED_MEMORY);
		buf[0].len = m_pMemoryData->GetSize();
		//DSP反馈数据
		buf[1].addr = m_pMemoryRespond->GetBuffer(SW_NORMAL_MEMORY);
		buf[1].phys = (DWORD)m_pMemoryRespond->GetBuffer(SW_SHARED_MEMORY);
		buf[1].len = m_pMemoryRespond->GetSize();
  	
		m_fInitialized = TRUE;
	}
	return S_OK;
}

VOID CSWBaseLinkCtrl::Clear()
{
	if( NULL != m_pMemoryFactory )
	{
		if( NULL != m_pMemoryData )
		{
			m_pMemoryFactory->Free(m_pMemoryData);
			m_pMemoryData = NULL;
		}
		if( NULL != m_pMemoryRespond )
		{
			m_pMemoryFactory->Free(m_pMemoryRespond);
			m_pMemoryRespond = NULL;
		}
		if( NULL != m_pMemoryPoint )
		{
			m_pMemoryFactory->Free(m_pMemoryPoint);
			m_pMemoryPoint = NULL;
		}
		m_pMemoryFactory = NULL;
	}
	m_fInitialized = FALSE;
}

/**
* @brief JPEG压缩接口
* 
* @param [in] pImageSrc : 待压缩的图片
* @param [out] ppImageJpeg : 压缩后的JPEG图片,由接口申请，使用者释放。
* @param [in] dwCompressRate : 压缩率
* @param [in] dwJpegDataType : JPEG图片格式（0：表示Jpeg数据段分三段；1：表示Jpeg数据段为一段）
* @return
* - S_OK : 成功
* - E_FAIL : 失败
*/
HRESULT CSWBaseLinkCtrl::EncodeJpeg(CSWImage* pImageSrc
    , CSWImage** ppImageJpeg
    , const DWORD& dwCompressRate
    , const DWORD& dwJpegDataType
    , const PBYTE bPhys
    , const CHAR* szTime
    , const INT iX
    , const INT iY
    , const BYTE yColor
    , const BYTE uColor
    , const BYTE vColor
    , const SW_RECT* pRect
    , const BOOL fCrop)
{
	if( !IsInitialize() )
	{
		return E_FAIL;
	}
	if( NULL == pImageSrc || NULL == ppImageJpeg )
	{
		return E_INVALIDARG;
	}
	if(pImageSrc->GetType() == SW_IMAGE_JPEG)
	{
		*ppImageJpeg = pImageSrc;
		pImageSrc->AddRef();
		return S_OK;
	}
	(*ppImageJpeg) = NULL;

	JPEG_ENCODE_PARAM cParam;
	JPEG_ENCODE_DATA cData;
	JPEG_ENCODE_RESPOND cRespond;

	cParam.dwCompressRate = dwCompressRate;
	cParam.dwJpegDataType = dwJpegDataType;
	cParam.iEddy = 0;

	ConvertImage(pImageSrc, cData.hvImageYuv);
	cData.szDateTimeStrings[0] = 0;
	CSWMemoryFactory* pMemoryFactory = CSWMemoryFactory::GetInstance(SW_SHARED_MEMORY);
	CSWImage* pImageJpeg = NULL;
	HRESULT hr = CSWImage::CreateSWImage(
		&pImageJpeg, 
		SW_IMAGE_JPEG, 
		cData.hvImageYuv.iWidth, cData.hvImageYuv.iHeight, 
		pMemoryFactory, 
		pImageSrc->GetFrameNo(), pImageSrc->GetRefTime(), pImageSrc->IsCaptureImage(), pImageSrc->GetFrameName());
	if( S_OK == hr )
	{
		ConvertImage(pImageJpeg, cData.hvImageJpg);
		m_fJpegIsWait = TRUE;

		cData.phys   = bPhys;
		if(szTime)
		{
			swpa_strcpy(cData.szTime, szTime);
		}
		else
		{
			swpa_strcpy(cData.szTime, "");
		}
		cData.x      = iX;
		cData.y      = iY;
		cData.yColor = yColor;
		cData.uColor = uColor;
		cData.vColor = vColor;
		hr = ProcessCmd(pImageSrc, DCI_JPEG_ENCODE, (PVOID)&cParam, sizeof(cParam), (PVOID)&cData, sizeof(cData), (PVOID)&cRespond, sizeof(cRespond), 2000);
		m_fJpegIsWait = FALSE;
		if( hr == S_OK )
		{
			SW_COMPONENT_IMAGE cJpegImage;
			pImageJpeg->GetImage(&cJpegImage);
			cJpegImage.iSize = cRespond.dwJpegLen;
			pImageJpeg->SetImage(cJpegImage);
			(*ppImageJpeg) = pImageJpeg;
			(*ppImageJpeg)->AddRef();
		}
	}

	pImageJpeg->Release();
}

/**
* @brief 视频流识别初始化接口
* 
* @param [in] cVideoRecogerParam : 视频流识别参数。
* @return
* - S_OK : 成功
* - E_FAIL : 失败
*/
HRESULT CSWBaseLinkCtrl::InitVideoRecoger(INIT_VIDEO_RECOGER_PARAM& cVideoRecogerParam)
{
	return ProcessCmd(NULL, DCI_INIT_VIDEO_RECOGER, (PVOID)&cVideoRecogerParam, sizeof(cVideoRecogerParam), NULL, 0, NULL, 0, 4000);
}

HRESULT CSWBaseLinkCtrl::ModifyVideoRecogerParam(TRACKER_CFG_PARAM& cVideoRecogerParam)
{
	return ProcessCmd(NULL, DCI_MODIFY_VIDEO_PARAM, (PVOID)&cVideoRecogerParam, sizeof(cVideoRecogerParam), NULL, 0, NULL, 0, 4000);
}

HRESULT CSWBaseLinkCtrl::UpdateAlgParam(unsigned char* pbData, int iDataSize)
{
	return ProcessCmd(
			NULL,
			DCI_UPDATE_ALG_PARAM,
			pbData, iDataSize, NULL, 0, NULL, 0, 4000);
}

/**
 * @brief 更新车辆检测模型。
 *
 * @param [in] uiModelId : 模型ID
 * @param [in] pbData : 模型数据
 * @param [in] iDataSize : 模型数据大小
 * @return
 * - S_OK : 成功
 * - E_FAIL : 失败
 */
HRESULT CSWBaseLinkCtrl::UpdateModel(unsigned int uiModelId, unsigned char* pbData, int iDataSize)
{
	return ProcessCmd(
			NULL,
			DCI_UPDATE_MODEL_DATA,
			&uiModelId, 4, pbData, iDataSize, NULL, 0, 4000);
}


/**
* @brief 图片版识别初始化接口
*
* @param [in] cPhotoRecogerParam : 图片版识别参数。
* @return
* - S_OK : 成功
* - E_FAIL : 失败
*/
HRESULT CSWBaseLinkCtrl::InitPhotoRecoger(INIT_PHOTO_RECOGER_PARAM& cPhotoRecogerParam)
{
	return ProcessCmd(NULL, DCI_INIT_PHOTO_RECOGER, (PVOID)&cPhotoRecogerParam, sizeof(cPhotoRecogerParam), NULL, 0, NULL, 0, 4000);
}


/**
 * @brief 视频流车辆检测接口
 *
 * @param [in] pImage : 当前处理帧图片。
 * @param [in] pDetParam : 检测参数。
 * @param [out] pFrameRespond : 一帧处理结束后的返回信息
 * @return
 * - S_OK : 成功
 * - E_FAIL : 失败
 */
HRESULT CSWBaseLinkCtrl::ProessDetOneFrame(CSWImage* pImage, CAR_DETECT_PARAM* pDetParam, PROCESS_DETECT_INFO* pFrameRespond)
{
	if( !IsInitialize() )
	{
		return E_FAIL;
	}
	if( NULL == pImage || NULL == pFrameRespond ||  NULL == pDetParam )
	{
		return E_INVALIDARG;
	}

	PROCESS_ONE_FRAME_PARAM cProcessParam;
	PROCESS_ONE_FRAME_DATA cProcessData;
	PROCESS_ONE_FRAME_RESPOND cProcessRespond;

	swpa_memcpy(&cProcessParam.cDetectParam, pDetParam, sizeof(CAR_DETECT_PARAM));

	cProcessParam.dwCurTick = CSWDateTime::GetSystemTick();
	cProcessParam.dwImageTime = pImage->GetRefTime();
	cProcessParam.dwFrameNo = pImage->GetFrameNo();
	cProcessParam.fIsCaptureImage = pImage->IsCaptureImage();
	cProcessParam.pCurFrame = (IReferenceComponentImage *)pImage;

	// data
	ConvertImage(pImage, cProcessData.hvImage);
	if (cProcessData.hvImage.nImgType == HV_IMAGE_JPEG)
	{
		return E_INVALIDARG;
	}
	CSWMemoryFactory* pMemoryFactory = CSWMemoryFactory::GetInstance(SW_SHARED_MEMORY);
	if(NULL == pMemoryFactory)
	{
		return E_OUTOFMEMORY;
	}
	// 创建YUV数据
	CSWImage *pRefImageYuv = NULL;
	HRESULT hr = S_OK;

	pRefImageYuv = pImage;
	pImage->AddRef();

	if (NULL != pRefImageYuv)
	{
		ConvertImage(pRefImageYuv, cProcessData.hvImageYuv);
	}
	else
	{
		return E_OUTOFMEMORY;
	}

	if( S_OK == hr )
	{
		if( m_fJpegIsWait )
		{
			SW_TRACE_DEBUG("<dsp>processoneframe wait.\n");
			swpa_thread_sleep_ms(10);
		}
		hr = ProcessCmd(pImage, DCI_PROCESS_ONE_FRAME, (PVOID)&cProcessParam, sizeof(cProcessParam), (PVOID)&cProcessData, sizeof(cProcessData), (PVOID)&cProcessRespond, sizeof(cProcessRespond), 4000);
		if( S_OK == hr )
		{
			// 检测结果赋值
			pFrameRespond->iDetectCount = cProcessRespond.cTrackRectInfo.dwTrackCount;
			swpa_memcpy(pFrameRespond->rgcDetect, cProcessRespond.cTrackRectInfo.rgTrackRect, sizeof(pFrameRespond->rgcDetect));
			swpa_memcpy(pFrameRespond->rgiDetectType, cProcessRespond.iDetType, sizeof(pFrameRespond->rgiDetectType));

			//处理图片内存LOG
			for (int i = 0; i < cProcessRespond.cImgMemOperLog.nCount; i++)
			{
				if (cProcessRespond.cImgMemOperLog.rgOperInfo[i].nOperFlag == IOF_ADD_REF)
				{
					((CSWImage*)cProcessRespond.cImgMemOperLog.rgOperInfo[i].pMemAddr)->AddRef();
				}
				else if (cProcessRespond.cImgMemOperLog.rgOperInfo[i].nOperFlag == IOF_RELEASE)
				{
					((CSWImage*)cProcessRespond.cImgMemOperLog.rgOperInfo[i].pMemAddr)->Release();
				}
			}
		}
		else
		{
			SW_TRACE_NORMAL("Dsplink ProcessCmd failed.\n");
		}
	}

	SAFE_RELEASE(pRefImageYuv);

	return hr;
}

/**
* @brief 视频流识别帧接口
* 
* @param [in] pImage : 当前处理帧图片。
* @param [in] cOtherInfo : 当前处理帧的其它附加信息。
* @param [out] pFrameRespond : 一帧处理结束后的返回信息。
* @return
* - S_OK : 成功
* - E_FAIL : 失败
*/
HRESULT CSWBaseLinkCtrl::ProcessOneFrame(CSWImage* pImage, PROCESS_EVENT_STRUCT* pProcessEvent, FRAME_RECOGNIZE_PARAM* pFrameRecognizeParam /*= NULL*/)
{
	if( !IsInitialize() )
	{
		return E_FAIL;
	}
	if( NULL == pImage || NULL == pProcessEvent )
	{
		return E_INVALIDARG;
	}
	BOOL fIsJpegImage = FALSE;

	m_cSemRecog.Pend();

	PROCESS_ONE_FRAME_PARAM *pProcessParam = new PROCESS_ONE_FRAME_PARAM;
	PROCESS_ONE_FRAME_DATA *pProcessData = new PROCESS_ONE_FRAME_DATA;
	PROCESS_ONE_FRAME_RESPOND *pProcessRespond = new PROCESS_ONE_FRAME_RESPOND;

    if (pProcessParam == NULL
      || pProcessData == NULL
      || pProcessRespond == NULL)
    {
      SAFE_DELETE(pProcessParam);
      SAFE_DELETE(pProcessData);
      SAFE_DELETE(pProcessRespond);
      SW_TRACE_NORMAL("<CSWBaseLinkCtrl::ProessOneFrame>New obj failed!");
      m_cSemRecog.Post();
      return E_FAIL;
    }


	if( pFrameRecognizeParam != NULL )
	{
		swpa_memcpy(&pProcessParam->cFrameRecognizeParam, pFrameRecognizeParam, sizeof(FRAME_RECOGNIZE_PARAM));
	}

	// param
	// 两个时间是一样的。
	// zhaopy
	pProcessParam->dwCurTick = CSWDateTime::GetSystemTick();
	pProcessParam->dwImageTime = pImage->GetRefTime();
	pProcessParam->dwFrameNo = pImage->GetFrameNo();
	pProcessParam->fIsCaptureImage = pImage->IsCaptureImage();
	pProcessParam->pCurFrame = (IReferenceComponentImage *)pImage;
	pProcessParam->dwIOLevel = pProcessEvent->nIOLevel;
	pProcessParam->fIsCheckLightType = pProcessEvent->fIsCheckLight;
	pProcessParam->iCarArriveTriggerType = pProcessEvent->iCarArriveTriggerType;
	pProcessParam->iDiffTick = 0;
	pProcessParam->iVideoID = 0;
	pProcessParam->fIsARMNight = (pProcessEvent->nEnvLightType == 2);	//ARM端白天晚上的判断结果传入DSP
    /*if (!pImage->IsCaptureImage())
    {
        if (NULL != pFrameRecognizeParam)
        {
            pProcessParam->iEnvStatus = pFrameRecognizeParam->iEnvStatus;       // 3:day, 2:dusk, 1:night
        }
    }*/

	// data
	ConvertImage(pImage, pProcessData->hvImage);
	if (pProcessData->hvImage.nImgType == HV_IMAGE_JPEG)
	{
		fIsJpegImage = TRUE;
	}
	CSWMemoryFactory* pMemoryFactory = CSWMemoryFactory::GetInstance(SW_SHARED_MEMORY);
	if(NULL == pMemoryFactory)
	{
        SAFE_DELETE(pProcessParam);
        SAFE_DELETE(pProcessData);
        SAFE_DELETE(pProcessRespond);
        m_cSemRecog.Post();
		return E_OUTOFMEMORY;
	}
	// 创建YUV数据
	CSWImage *pRefImageYuv = NULL;
	HRESULT hr = S_OK;

	if(TRUE == fIsJpegImage)
	{	
		int width = pProcessData->hvImage.iHeight & 0x0000FFFF;
		int height = pProcessData->hvImage.iHeight >> 16;

		//需要创建静态共享内存,能如此做的原因是因为识别完毕之后，该内存不在被引用，避免网络发送线程占用太多共享内存导致无法识别
		static CSWImage * yuvImage = NULL;
		if (!yuvImage)
		{
			// 由于GBA解码库解码300W图片快，而对200W图片解码慢
			// 因此300W图片用GBA解码库，其它用TI解码库
			// huanggr 2011-12-01
			if (width > 2000 && width < 2100)
			{
				hr = CSWImage::CreateSWImage(&yuvImage
					, SW_IMAGE_YUV_422
					, width
					, height
					, pMemoryFactory
					, pImage->GetFrameNo()
					, pImage->GetRefTime()
					, pImage->IsCaptureImage()
					, pImage->GetFrameName()
					);
			}
			else
			{
				hr = CSWImage::CreateSWImage(&yuvImage
					, SW_IMAGE_CbYCrY
					, width
					, height
					, pMemoryFactory
					, pImage->GetFrameNo()
					, pImage->GetRefTime()
					, pImage->IsCaptureImage()
					, pImage->GetFrameName()
					);
			}
		}
		if (yuvImage)
		{
			pRefImageYuv = yuvImage;
			yuvImage->SetRefTime(pProcessParam->dwImageTime);
			yuvImage->AddRef();
			hr = S_OK;
		}
		else
		{
			SW_TRACE_NORMAL("create yuvimage failed.\n");
            SAFE_DELETE(pProcessParam);
            SAFE_DELETE(pProcessData);
            SAFE_DELETE(pProcessRespond);
            m_cSemRecog.Post();
			return E_FAIL;
		}
	}
	else
	{
		pRefImageYuv = pImage;
		pImage->AddRef();
	}  
	if (NULL != pRefImageYuv)
	{
		ConvertImage(pRefImageYuv, pProcessData->hvImageYuv);
	}
	else
	{
        SAFE_DELETE(pProcessParam);
        SAFE_DELETE(pProcessData);
        SAFE_DELETE(pProcessRespond);
        m_cSemRecog.Post();
		return E_OUTOFMEMORY;
	}

	// 创建小图数据

	for (int i = 0; i < MAX_EVENT_COUNT; i++)
	{
		if( m_rgpSmallImage[i] == NULL )
		{
			hr = CSWImage::CreateSWImage(
				&m_rgpSmallImage[i], SW_IMAGE_YUV_422, 800, 200, pMemoryFactory, 0, pImage->GetRefTime(), FALSE, "NULL");
		}
		if (S_OK == hr)
		{
			pProcessParam->rgpSmallImage[i] = (IReferenceComponentImage *)m_rgpSmallImage[i];
			ConvertImage(m_rgpSmallImage[i], pProcessData->rghvImageSmall[i]);
			// 清cache
			if( m_rgpSmallImage[i] != NULL )
			{
				swpa_ipnc_cache_inv(
						m_rgpSmallImage[i]->GetImageBuffer(),
						m_rgpSmallImage[i]->GetImageBufferSize(),
						0xFFFF, 1);
			}
		}
		else
		{
            SW_TRACE_DEBUG("create SmallImage failed.\n");
			break;
		}

		if( NULL == m_rgpBinImage[i] )
		{
			hr = CSWImage::CreateSWImage(
				&m_rgpBinImage[i], SW_IMAGE_BIN, BINARY_IMAGE_WIDTH, BINARY_IMAGE_HEIGHT, pMemoryFactory, 0, pImage->GetRefTime(), FALSE, "NULL");
		}
		if (S_OK == hr)
		{
			pProcessParam->rgpBinImage[i] = (IReferenceComponentImage *)m_rgpBinImage[i];
			ConvertImage(m_rgpBinImage[i], pProcessData->rghvImageBin[i]);
		}
		else
		{
			SW_TRACE_NORMAL("create BinImage failed.\n");
			break;
		}
	}
    // ganzz，创建异步检测数据空间并拷贝
	// todo. 需要实地址。
    CSWMemory* pSyncDetDataMem = NULL;
    if (pProcessEvent->cSyncDetData.pbData != NULL)
    {        
        pSyncDetDataMem = pMemoryFactory->Alloc(
            pProcessEvent->cSyncDetData.nLen);
        if (pSyncDetDataMem != NULL)
        {
            pProcessData->cSyncDetData.pbData = pSyncDetDataMem->GetBuffer(0);
            memcpy(pProcessData->cSyncDetData.pbData,
                pProcessEvent->cSyncDetData.pbData, 
                pProcessEvent->cSyncDetData.nLen
                );
            pProcessData->cSyncDetData.pbData = pSyncDetDataMem->GetBuffer(1);
            swpa_ipnc_cache_wb(pSyncDetDataMem->GetBuffer(), pSyncDetDataMem->GetSize(), 0xFFFF, 1);
            pProcessData->cSyncDetData.nLen = pProcessEvent->cSyncDetData.nLen;
        }
        else
        {
            SW_TRACE_NORMAL("create SyncDetData(share mem) failed.\n");
        }
    }
	if( S_OK == hr )
	{
		if( m_fJpegIsWait )
		{
			SW_TRACE_DEBUG("<dsp>processoneframe wait.\n");
			swpa_thread_sleep_ms(10);
		}
		hr = ProcessCmd(pImage, DCI_PROCESS_ONE_FRAME, (PVOID)pProcessParam, sizeof(PROCESS_ONE_FRAME_PARAM), (PVOID)pProcessData, sizeof(PROCESS_ONE_FRAME_DATA), (PVOID)pProcessRespond, sizeof(PROCESS_ONE_FRAME_RESPOND), 4000);
		if( S_OK == hr )
		{
			pProcessEvent->dwEventId = pProcessRespond->cTrigEvent.dwEventId;
			pProcessEvent->iCarArriveInfoCount = pProcessRespond->cTrigEvent.iCarArriveCount;
			pProcessEvent->iCarLeftInfoCount = pProcessRespond->cTrigEvent.iCarLeftCount;
			pProcessEvent->cLightType = pProcessRespond->cLightType;
			pProcessEvent->iCplStatus = pProcessRespond->iCplStatus;
			pProcessEvent->iPulseLevel = pProcessRespond->iPulseLevel;
			pProcessEvent->cTrackRectInfo = pProcessRespond->cTrackRectInfo;
			pProcessEvent->iFrameAvgY = pProcessRespond->iFrameAvgY;
			pProcessEvent->nEnvLightType = pProcessRespond->nEnvLightType;
			pProcessEvent->nWDRLevel = pProcessRespond->nWDRLevel;
			//拷贝红灯坐标，要求这2个数组的大小一致
			memcpy(pProcessEvent->rcRedLight, pProcessRespond->rcRedLight, sizeof(pProcessEvent->rcRedLight));
			memcpy(pProcessEvent->rgiAllCarTrigger, pProcessRespond->cTrigEvent.rgiAllCarTrigger, sizeof(pProcessEvent->rgiAllCarTrigger));
			if (pProcessEvent->dwEventId & EVENT_CARARRIVE)
			{
				for (int i = 0; i < pProcessEvent->iCarArriveInfoCount; i++)
				{
					memcpy(&pProcessEvent->rgCarArriveInfo[i], &pProcessRespond->cTrigEvent.rgCarArriveInfo[i], sizeof(CARARRIVE_INFO_STRUCT));
				}
			}	
			if (pProcessEvent->dwEventId & EVENT_CARLEFT)
			{
				for (int i = 0; i < pProcessEvent->iCarLeftInfoCount; i++)
				{
					memcpy(&pProcessEvent->rgCarLeftInfo[i].cCoreResult, &pProcessRespond->cTrigEvent.rgCarLeftCoreInfo[i], sizeof(PROCESS_IMAGE_CORE_RESULT));
					CSWImage *rgpImage[] = 
					{
						  (CSWImage*)pProcessEvent->rgCarLeftInfo[i].cCoreResult.cResultImg.pimgBestSnapShot
						, (CSWImage*)pProcessEvent->rgCarLeftInfo[i].cCoreResult.cResultImg.pimgLastSnapShot
						, (CSWImage*)pProcessEvent->rgCarLeftInfo[i].cCoreResult.cResultImg.pimgBeginCapture
						, (CSWImage*)pProcessEvent->rgCarLeftInfo[i].cCoreResult.cResultImg.pimgBestCapture
						, (CSWImage*)pProcessEvent->rgCarLeftInfo[i].cCoreResult.cResultImg.pimgLastCapture
					};
					for(int k = 0; k < sizeof(rgpImage)/sizeof(CSWImage *); k++)
					{
						SAFE_ADDREF(rgpImage[k]);
					}

					if (pProcessRespond->cTrigEvent.rgCarLeftCoreInfo[i].nPlateWidth == 0 || pProcessRespond->cTrigEvent.rgCarLeftCoreInfo[i].nPlateHeight == 0)
					{
						pProcessEvent->rgCarLeftInfo[i].cCoreResult.cResultImg.pimgPlate = NULL;
						pProcessEvent->rgCarLeftInfo[i].cCoreResult.cResultImg.pimgPlateBin = NULL;
					}
					else
					{ //车牌小图
						CSWImage *pSmallImage = (CSWImage *)pProcessParam->rgpSmallImage[i];

						// 清cache
						if( pSmallImage != NULL )
						{
							swpa_ipnc_cache_inv(
									pSmallImage->GetImageBuffer(),
									pSmallImage->GetImageBufferSize(),
									0xFFFF, 1);
						}

						pSmallImage->AddRef();

						// 释放小图
						SAFE_RELEASE(m_rgpSmallImage[i]);

						//修改小图大小 
						SW_COMPONENT_IMAGE swImage;
						pSmallImage->GetImage(&swImage);
						swImage.iWidth = pProcessRespond->cTrigEvent.rgCarLeftCoreInfo[i].nPlateWidth;
						swImage.iHeight = pProcessRespond->cTrigEvent.rgCarLeftCoreInfo[i].nPlateHeight;
						swImage.rgiStrideWidth[0] = (swImage.iWidth + 3) & (~3);
						swImage.rgiStrideWidth[1] = swImage.rgiStrideWidth[0] >> 1;
						swImage.rgiStrideWidth[2] = swImage.rgiStrideWidth[1];

						// 修正宽度
						//swImage.iWidth = swImage.rgiStrideWidth[0];

						swImage.iSize = swImage.iWidth * swImage.iHeight + (swImage.iWidth / 2) * swImage.iHeight + (swImage.iWidth / 2) * swImage.iHeight;
						pSmallImage->SetImage(swImage);
						pProcessEvent->rgCarLeftInfo[i].cCoreResult.cResultImg.pimgPlate = (IReferenceComponentImage *)pSmallImage;
						//车牌二值图
						pSmallImage = (CSWImage *)pProcessParam->rgpBinImage[i];
						// 清cache
						if( pSmallImage != NULL )
						{
							swpa_ipnc_cache_inv(
									pSmallImage->GetImageBuffer(),
									pSmallImage->GetImageBufferSize(),
									0xFFFF, 1);
						}
						pSmallImage->AddRef();

						// 释放小图
						SAFE_RELEASE(m_rgpBinImage[i]);

						SW_COMPONENT_IMAGE swImageBin;
						pSmallImage->GetImage(&swImageBin);
						swImageBin.iSize = ((swImageBin.iWidth >> 3) * swImageBin.iHeight);
						pSmallImage->SetImage(swImageBin);						
						pProcessEvent->rgCarLeftInfo[i].cCoreResult.cResultImg.pimgPlateBin = (IReferenceComponentImage *)pSmallImage;
					}
				}
			}

			//处理图片内存LOG
			for (int i = 0; i < pProcessRespond->cImgMemOperLog.nCount; i++)
			{
				if (pProcessRespond->cImgMemOperLog.rgOperInfo[i].nOperFlag == IOF_ADD_REF)
				{
					((CSWImage*)pProcessRespond->cImgMemOperLog.rgOperInfo[i].pMemAddr)->AddRef();
				}
				else if (pProcessRespond->cImgMemOperLog.rgOperInfo[i].nOperFlag == IOF_RELEASE)
				{
					((CSWImage*)pProcessRespond->cImgMemOperLog.rgOperInfo[i].pMemAddr)->Release();
				}
			}
		}
		else
		{
			SW_TRACE_NORMAL("Dsplink ProcessCmd failed.\n");
		}
	}

	if( pMemoryFactory != NULL )
	{
		SAFE_RELEASE(pRefImageYuv);
        if (pSyncDetDataMem != NULL)
        {
            pMemoryFactory->Free(pSyncDetDataMem);
        }
		pMemoryFactory = NULL;
	}
    SAFE_DELETE(pProcessParam);
    SAFE_DELETE(pProcessData);
    SAFE_DELETE(pProcessRespond);
    m_cSemRecog.Post();
	return hr;
}

/**
* @brief 图片版识别帧接口
*
* @param [in] pImage : 当前处理帧图片。
* @param [out] pFrameRespond : 一帧处理结束后的返回信息。
* @return
* - S_OK : 成功
* - E_FAIL : 失败
*/
HRESULT CSWBaseLinkCtrl::ProcessPhoto(CSWImage* pImage, PR_PARAM *pFrameParam, PROCESS_EVENT_STRUCT* pProcessEvent)
{
	if( !IsInitialize() )
  {
      return E_FAIL;
  }
  if( NULL == pImage || NULL == pProcessEvent )
  {
      return E_INVALIDARG;
  }

  BOOL fIsJpegImage = FALSE;

  PROCESS_PHOTO_PARAM cProcessParam;
  PROCESS_PHOTO_DATA cProcessData;
  PROCESS_PHOTO_RESPOND cProcessRespond;

  memcpy(&cProcessParam.prPram, pFrameParam, sizeof(PR_PARAM));

  // data
  ConvertImage(pImage, cProcessData.hvImage);
  if (cProcessData.hvImage.nImgType == HV_IMAGE_JPEG)
  {
      fIsJpegImage = TRUE;
  }
  CSWMemoryFactory* pMemoryFactory = CSWMemoryFactory::GetInstance(SW_SHARED_MEMORY);
  if(NULL == pMemoryFactory)
  {
      return E_OUTOFMEMORY;
  }
  // 创建YUV数据
  CSWImage *pRefImageYuv = NULL;
  HRESULT hr = S_OK;

  if(TRUE == fIsJpegImage)
  {
      int width = cProcessData.hvImage.iHeight & 0x0000FFFF;
      int height = cProcessData.hvImage.iHeight >> 16;

      //需要创建静态共享内存,能如此做的原因是因为识别完毕之后，该内存不在被引用，避免网络发送线程占用太多共享内存导致无法识别
      static CSWImage * yuvImage = NULL;
      if (!yuvImage)
      {
          // 由于GBA解码库解码300W图片快，而对200W图片解码慢
          // 因此300W图片用GBA解码库，其它用TI解码库
          // huanggr 2011-12-01
          if (width > 2000 && width < 2100)
          {
              hr = CSWImage::CreateSWImage(&yuvImage
                  , SW_IMAGE_YUV_422
                  , width
                  , height
                  , pMemoryFactory
                  , pImage->GetFrameNo()
                  , pImage->GetRefTime()
                  , pImage->IsCaptureImage()
                  , pImage->GetFrameName()
                  );
          }
          else
          {
              hr = CSWImage::CreateSWImage(&yuvImage
                  , SW_IMAGE_CbYCrY
                  , width
                  , height
                  , pMemoryFactory
                  , pImage->GetFrameNo()
                  , pImage->GetRefTime()
                  , pImage->IsCaptureImage()
                  , pImage->GetFrameName()
                  );
          }
      }
      if (yuvImage)
      {
          pRefImageYuv = yuvImage;
          yuvImage->SetRefTime(pImage->GetRefTime());
          yuvImage->AddRef();
          hr = S_OK;
      }
      else
      {
          SW_TRACE_NORMAL("create yuvimage failed.\n");
          return E_FAIL;
      }
  }
  else
  {
      pRefImageYuv = pImage;
      pImage->AddRef();
  }
  if (NULL != pRefImageYuv)
  {
      ConvertImage(pRefImageYuv, cProcessData.hvImageYuv);
  }
  else
  {
      return E_OUTOFMEMORY;
  }

  CSWImage* rgpSmallImage[MAX_EVENT_COUNT];
  CSWImage* rgpBinImage[MAX_EVENT_COUNT];

  memset(rgpSmallImage, 0, sizeof(CSWImage*) * MAX_EVENT_COUNT);
  memset(rgpBinImage, 0, sizeof(CSWImage*) * MAX_EVENT_COUNT);

  // 创建小图数据
  for (int i = 0; i < MAX_EVENT_COUNT; i++)
  {
      hr = CSWImage::CreateSWImage(&rgpSmallImage[i],
          SW_IMAGE_YUV_422,
          1200,
          240,
          pMemoryFactory,
          0,
          pImage->GetRefTime(),
          FALSE,
          "NULL"
          );
      if (S_OK == hr)
      {
          cProcessParam.rgpSmallImage[i] = (IReferenceComponentImage *)rgpSmallImage[i];
          ConvertImage(rgpSmallImage[i], cProcessData.rghvImageSmall[i]);
      }
      else
      {
          SW_TRACE_DEBUG("create SmallImage failed.\n");
          break;
      }

      hr = CSWImage::CreateSWImage(&rgpBinImage[i],
          SW_IMAGE_BIN,
          BINARY_IMAGE_WIDTH,
          BINARY_IMAGE_HEIGHT,
          pMemoryFactory,
          0,
          pImage->GetRefTime(),
          FALSE,
          "NULL"
          );

      if (S_OK == hr)
      {
          cProcessParam.rgpBinImage[i] = (IReferenceComponentImage *)rgpBinImage[i];
          ConvertImage(rgpBinImage[i], cProcessData.rghvImageBin[i]);
      }
      else
      {
          SW_TRACE_NORMAL("create BinImage failed.\n");
          break;
      }
  }

  if( S_OK == hr )
  {
      hr = ProcessCmd(
      		pImage,
          DCI_PROCESS_PHOTO,
          (PVOID)&cProcessParam,
          sizeof(cProcessParam),
          (PVOID)&cProcessData,
          sizeof(cProcessData),
          (PVOID)&cProcessRespond,
          sizeof(cProcessRespond),
          4000
          );
      if( S_OK == hr )
      {
          pProcessEvent->dwEventId |= EVENT_CARLEFT;
          pProcessEvent->iCarLeftInfoCount = cProcessRespond.iResultCount;

          if (pProcessEvent->iCarLeftInfoCount > 0)
          {
              for (int i = 0; i < pProcessEvent->iCarLeftInfoCount; i++)
              {
                  memcpy(&pProcessEvent->rgCarLeftInfo[i].cCoreResult, &cProcessRespond.rgProcessPhotoResult[i], sizeof(PROCESS_IMAGE_CORE_RESULT));
                  pProcessEvent->rgCarLeftInfo[i].cCoreResult.nCarArriveTime = pImage->GetRefTime();
                  pProcessEvent->rgCarLeftInfo[i].cCoreResult.cResultImg.pimgBestSnapShot = (IReferenceComponentImage *)pImage;
                  pImage->AddRef();

                  if (cProcessRespond.rgProcessPhotoResult[i].nPlateWidth == 0 || cProcessRespond.rgProcessPhotoResult[i].nPlateHeight == 0)
                  {
                      pProcessEvent->rgCarLeftInfo[i].cCoreResult.cResultImg.pimgPlate = NULL;
                      pProcessEvent->rgCarLeftInfo[i].cCoreResult.cResultImg.pimgPlateBin = NULL;
                  }
                  else
                  {
                      //车牌小图
                      CSWImage *pImagePlate = (CSWImage *)cProcessParam.rgpSmallImage[i];
                      pImagePlate->AddRef();
                      //修改小图大小
                      SW_COMPONENT_IMAGE swImage;
                      pImagePlate->GetImage(&swImage);
                      swImage.iWidth = cProcessRespond.rgProcessPhotoResult[i].nPlateWidth;
                      swImage.iHeight = cProcessRespond.rgProcessPhotoResult[i].nPlateHeight;
                      swImage.rgiStrideWidth[0] = (swImage.iWidth + 3) & (~3);
                      swImage.rgiStrideWidth[1] = swImage.rgiStrideWidth[0] >> 1;
                      swImage.rgiStrideWidth[2] = swImage.rgiStrideWidth[1];
                      swImage.iSize = swImage.iWidth * swImage.iHeight + (swImage.iWidth / 2) * swImage.iHeight + (swImage.iWidth / 2) * swImage.iHeight;
                      pImagePlate->SetImage(swImage);
                      pProcessEvent->rgCarLeftInfo[i].cCoreResult.cResultImg.pimgPlate = (IReferenceComponentImage *)pImagePlate;
                      //车牌二值图
                      pImagePlate = (CSWImage *)cProcessParam.rgpBinImage[i];
                      pImagePlate->AddRef();

                      SW_COMPONENT_IMAGE swImageBin;
                      pImagePlate->GetImage(&swImageBin);
                      swImageBin.iSize = ((swImageBin.iWidth >> 3) * swImageBin.iHeight);
                      pImagePlate->SetImage(swImageBin);

                      pProcessEvent->rgCarLeftInfo[i].cCoreResult.cResultImg.pimgPlateBin = (IReferenceComponentImage *)pImagePlate;
                  }
              }
          }
          else
          {
              pProcessEvent->iCarLeftInfoCount = 1;
              pProcessEvent->rgCarLeftInfo[0].cCoreResult.nCarArriveTime = pImage->GetRefTime();
              pProcessEvent->rgCarLeftInfo[0].cCoreResult.cResultImg.pimgBestSnapShot = (IReferenceComponentImage *)pImage;
              pProcessEvent->rgCarLeftInfo[0].cCoreResult.cResultImg.pimgPlate = NULL;
              pProcessEvent->rgCarLeftInfo[0].cCoreResult.cResultImg.pimgPlateBin = NULL;
              pImage->AddRef();
          }
      }
      else
      {
          SW_TRACE_NORMAL("Dsplink ProcessCmd failed.\n");
      }
  }

  if( pMemoryFactory != NULL )
  {
      for( int i = 0; i < MAX_EVENT_COUNT; ++i )
      {
          SAFE_RELEASE(rgpSmallImage[i]);
          SAFE_RELEASE(rgpBinImage[i]);
      }
      SAFE_RELEASE(pRefImageYuv);
      pMemoryFactory = NULL;
  }

  return hr;
}

HRESULT CSWBaseLinkCtrl::RegisterCallBack(VOID (*OnDSPAlarm)(PVOID pvParam, INT iInfo), PVOID pvParam)
{
	return E_NOTIMPL;
}

HRESULT CSWBaseLinkCtrl::ProcessCmd(
		CSWImage* pImage,
		const INT& iCmdId, 
		PVOID pvParam, const DWORD& dwParamSize, 
		PVOID pvData, const DWORD& dwDataSize,
		PVOID pvRespond, const DWORD& dwRespondSize, 
		const INT& iTimeOut
		)
{
	if( !IsInitialize() )
	{
		SW_TRACE_NORMAL("<DspLink don't Initialized>.\n");
		return E_FAIL;
	}

	m_cSem.Pend();

	// 如果大于1M，需要申请
	CSWMemory* pMemData = NULL;
	PBYTE pbDataSrc = NULL;
	bool fIsBig = false;
	if( (dwDataSize + dwParamSize) > 1 * 1024 * 1024 )
	{
		pMemData = m_pMemoryFactory->Alloc(dwDataSize + dwParamSize + 128);
		if( NULL == pMemData )
		{
			m_cSem.Post();
			SW_TRACE_ERROR("DSP process failed out of memory:%d.",
					dwParamSize);
			return E_OUTOFMEMORY;
		}
		pbDataSrc = (PBYTE)pMemData->GetBuffer();
		//在这里整合好数据
		DSPLinkBuffer *buf = (DSPLinkBuffer *)m_pMemoryPoint->GetBuffer();
		//通知DSP端的数据
		buf[0].addr = pMemData->GetBuffer(SW_NORMAL_MEMORY);
		buf[0].phys = (DWORD)pMemData->GetBuffer(SW_SHARED_MEMORY);
		buf[0].len = pMemData->GetSize();

		swpa_ipnc_cache_wb(m_pMemoryPoint->GetBuffer(), m_pMemoryPoint->GetSize(), 0xFFFF, 1);

		fIsBig = true;
	}
	else
	{
		pbDataSrc = (PBYTE)m_pMemoryData->GetBuffer();
	}

	swpa_memcpy(pbDataSrc, &iCmdId, sizeof(DWORD));
	pbDataSrc += sizeof(DWORD);
	if( NULL != pvParam )
	{
		swpa_memcpy(pbDataSrc, &dwParamSize, sizeof(DWORD));
		pbDataSrc += sizeof(DWORD);

		swpa_memcpy(pbDataSrc, pvParam, dwParamSize);
		pbDataSrc += dwParamSize;
	}
	else
	{
		*(DWORD *)pbDataSrc = 0;
		pbDataSrc += sizeof(DWORD);
	}
	if( NULL != pvData )
	{
		swpa_memcpy(pbDataSrc, &dwDataSize, sizeof(DWORD));
		pbDataSrc += sizeof(DWORD);

		swpa_memcpy(pbDataSrc, pvData, dwDataSize);
		pbDataSrc += dwDataSize;
	}
	else
	{
		*(DWORD *)pbDataSrc = 0;
		pbDataSrc += sizeof(DWORD);
	}

	static INT siProcessTimes = 0;
	static INT siProcessMs = 0;
	DWORD dwTick = CSWDateTime::GetSystemTick();
	if(iCmdId == DCI_JPEG_ENCODE )
	{
		if( siProcessTimes > 10 )
		{
			SW_TRACE_DEBUG("<JPEG>encode ms:%d.", siProcessMs / siProcessTimes);
			siProcessTimes = 0;
			siProcessMs = 0;
		}
	}
    DWORD dwProcessBeforeTick = CSWDateTime::GetSystemTick();
	HRESULT hr = DoProcess(pImage, iTimeOut);
    DWORD dwProcessAfterTick = CSWDateTime::GetSystemTick();

	// 如果是大于1M，还原
	if( fIsBig )
	{
		//在这里整合好数据
		DSPLinkBuffer *buf = (DSPLinkBuffer *)m_pMemoryPoint->GetBuffer();
		//通知DSP端的数据
		buf[0].addr = m_pMemoryData->GetBuffer(SW_NORMAL_MEMORY);
		buf[0].phys = (DWORD)m_pMemoryData->GetBuffer(SW_SHARED_MEMORY);
		buf[0].len = m_pMemoryData->GetSize();
		swpa_ipnc_cache_wb(m_pMemoryPoint->GetBuffer(), m_pMemoryPoint->GetSize(), 0xFFFF, 1);
	}

	DSP_RESPOND_HEAD* pRespondHead = (DSP_RESPOND_HEAD*)m_pMemoryRespond->GetBuffer();
	if( S_OK != hr )
	{
        SW_TRACE_NORMAL("<DspLink Write failed. 0x%08x>.[%s], DoProcessTime: %d ms, CmdId: %d, iResult: %d\n",
                        hr, strlen(pRespondHead->szErrorInfo) > 0 ? pRespondHead->szErrorInfo : "null",
                        dwProcessAfterTick - dwProcessBeforeTick, pRespondHead->dwCmdId, pRespondHead->iResult);
		m_cSem.Post();
		// DSP超时失败要返回指定的值。
		hr = E_UNEXPECTED;
		return hr;
	}

	//DSP_RESPOND_HEAD* pRespondHead = (DSP_RESPOND_HEAD*)m_pMemoryRespond->GetBuffer();
	if( pRespondHead->dwCmdId != iCmdId )
	{
		SW_TRACE_NORMAL("<CmdID:%d->%d, result:0x%08x,error:%s>.\n"
			, iCmdId
			, pRespondHead->dwCmdId
			, pRespondHead->iResult
			, pRespondHead->szErrorInfo
			);
		m_cSem.Post();
		return E_FAIL;
	}

	if( S_OK != (hr = pRespondHead->iResult))
	{
		SW_TRACE_NORMAL("<CmdID:%d->%d, result:0x%08x,error:%s>.\n"
			, iCmdId
			, pRespondHead->dwCmdId
			, pRespondHead->iResult
			, pRespondHead->szErrorInfo
			);
	}
#if 1	
	//if( CSWDateTime::GetSystemTick() - dwTick > 100 )
	{
		if(swpa_strlen(pRespondHead->szErrorInfo) > 0)
		{
			SW_TRACE_DSP("<CmdID:%d->%d, result:0x%08x,info:%s>.\n"
				, iCmdId
				, pRespondHead->dwCmdId
				, pRespondHead->iResult
				, pRespondHead->szErrorInfo
				);
		}
	}
#endif

	if(pvRespond != NULL)
	{
		if( dwRespondSize > (MAX_BUFFER_SIZE - sizeof(DSP_RESPOND_HEAD)))
		{
			SW_TRACE_NORMAL("Dsplink respond outsize.\n");
			m_cSem.Post();
			return E_OUTOFMEMORY;
		}
		memcpy(pvRespond, (PBYTE)m_pMemoryRespond->GetBuffer() + sizeof(DSP_RESPOND_HEAD), dwRespondSize);
	}

	if(iCmdId == DCI_JPEG_ENCODE )
	{
		++siProcessTimes;
		siProcessMs += (CSWDateTime::GetSystemTick() - dwTick);
	}
	m_cSem.Post();
	return hr;
}		

HRESULT CSWBaseLinkCtrl::DoProcess(CSWImage* pImage, const INT& iTimeOut)
{
	return E_NOTIMPL;
}

HRESULT CSWBaseLinkCtrl::ConvertImage(CSWImage *pImage, HV_COMPONENT_IMAGE& hvImage)
{
	if(NULL == pImage)
	{
		return E_POINTER;
	}

	SW_COMPONENT_IMAGE img;
	pImage->GetImage(&img);
	swpa_memcpy(hvImage.iStrideWidth, img.rgiStrideWidth, sizeof(hvImage.iStrideWidth));
	
	if(img.cImageType == SW_IMAGE_YUV420SP)
	{
		img.cImageType = SW_IMAGE_BT1120;
	}
	
	hvImage.nImgType = (HV_IMAGE_TYPE)img.cImageType;
	if(hvImage.nImgType == HV_IMAGE_JPEG)
	{
		hvImage.iHeight = pImage->GetWidth()|(pImage->GetHeight() << 16);
		hvImage.iWidth = pImage->GetSize();
	}
	else
	{
		hvImage.iHeight = pImage->GetHeight();
		hvImage.iWidth = pImage->GetWidth();
	}
	hvImage.iMemPos = 2;
	for(int i = 0; i < 3; i++)
	{
		hvImage.rgImageData[i].addr = img.rgpbData[i];
		hvImage.rgImageData[i].phys = img.phys[i];
		hvImage.rgImageData[i].len = img.len[i];
	}
	return S_OK;
}

