///////////////////////////////////////////////////////////
//  SWDspLinkCtrl.cpp
//  Implementation of the Class CSWImage
//  Created on:      
//  Original author: 
///////////////////////////////////////////////////////////
#include "SWFC.h"
#include "SWSysLinkCtrl.h"
#include "SWExif.h"

/**
* @brief 构造函数
*/
CSWSysLinkCtrl::CSWSysLinkCtrl()
{
	m_hlstJpeg = swpa_list_create();
	m_hlstDspRes = swpa_list_create();
#ifdef PR_IN_ARM
	m_fInitEPApp = FALSE;
#endif	
}

/**
* @brief 析构函数
*/
CSWSysLinkCtrl::~CSWSysLinkCtrl()
{
	if(m_hlstJpeg)
	{
		CSWImage *pImage = (CSWImage *)swpa_list_remove(m_hlstJpeg, 0, 100);
		while (NULL != pImage)
		{
			SAFE_RELEASE(pImage);
			pImage = (CSWImage *)swpa_list_remove(m_hlstJpeg, 0, 100);
		} 

		swpa_list_destroy(m_hlstJpeg);
	}
	if(m_hlstDspRes)
	{
		IPNC_DATA* data = (IPNC_DATA *)swpa_list_remove(m_hlstDspRes, 0, 100);
		while (NULL != data)
		{
			delete data;
			data = (IPNC_DATA *)swpa_list_remove(m_hlstDspRes, 0, 100);
		}
		
		swpa_list_destroy(m_hlstDspRes);
	}
}

HRESULT CSWSysLinkCtrl::Initialize(VOID)
{
	swpa_ipnc_setcallback(SWPA_LINK_DSP, 0xFF, OnResult, this);
	swpa_ipnc_setcallback(SWPA_LINK_VIDEO, 1, OnResult, this);
		
	return CSWBaseLinkCtrl::Initialize();
}

VOID CSWSysLinkCtrl::Clear()
{
}

int CSWSysLinkCtrl::OnResult(void *pContext, int type, void *struct_ptr)
{
	CSWSysLinkCtrl *pThis = (CSWSysLinkCtrl *)pContext;
	int ret = 0;
	switch(type)
	{
	case CALLBACK_TYPE_IMAGE:
		{
			IMAGE *image = (IMAGE *)struct_ptr;
			if(image->type == SWPA_IMAGE_JPEG)
			{
				CSWImage *pImage = NULL;
				if(S_OK == CSWImage::CreateSWImage(&pImage
					, SW_IMAGE_JPEG
					, image->pitch
					, image->width
					, image->height
					, CSWMemoryFactory::GetInstance(SW_SHARED_MEMORY)
					, image->data.addr
					, image->data.phys
					, image->data.size
					)
				)
				{
					swpa_list_add(pThis->m_hlstJpeg, pImage);
					ret = 1;
				}
			}
			break;
		}
		
	case CALLBACK_TYPE_DATA:
		{
            ret = 1;
			IPNC_DATA *pData = new IPNC_DATA;
			swpa_memcpy(pData, struct_ptr, sizeof(IPNC_DATA));
			swpa_list_add(pThis->m_hlstDspRes, pData);
			break;
		}
	}
	return ret;
}

/**
* @brief 视频流识别初始化接口
* 
* @param [in] cVideoRecogerParam : 视频流识别参数。
* @return
* - S_OK : 成功
* - E_FAIL : 失败
*/
HRESULT CSWSysLinkCtrl::InitVideoRecoger(INIT_VIDEO_RECOGER_PARAM& cVideoRecogerParam)
{
	SW_TRACE_DEBUG("CSWSysLinkCtrl::InitVideoRecoger...");
#ifdef PR_IN_ARM
	memcpy(&m_cVideoRecogerParam, &cVideoRecogerParam, sizeof(INIT_VIDEO_RECOGER_PARAM));
#endif	
	//金星平台
	cVideoRecogerParam.cTrackerCfgParam.iPlatform = 2;
#ifdef USE_IPNC_CONTROL	
	return SWPAR_OK == swpa_ipnc_control(SWPA_LINK_DSP, 0, &cVideoRecogerParam, sizeof(cVideoRecogerParam), -1) ? S_OK : E_FAIL;
#else
	return CSWBaseLinkCtrl::InitVideoRecoger(cVideoRecogerParam);
#endif		
}

HRESULT CSWSysLinkCtrl::ModifyVideoRecogerParam(TRACKER_CFG_PARAM& cVideoRecogerParam)
{
	SW_TRACE_DEBUG("CSWSysLinkCtrl::ModifyVideoRecogerParam...");
	//金星平台
	cVideoRecogerParam.iPlatform = 2;
#ifdef USE_IPNC_CONTROL
	return SWPAR_OK == swpa_ipnc_control(SWPA_LINK_DSP, 0, &cVideoRecogerParam, sizeof(cVideoRecogerParam), -1) ? S_OK : E_FAIL;
#else
	return CSWBaseLinkCtrl::ModifyVideoRecogerParam(cVideoRecogerParam);
#endif
}

HRESULT CSWSysLinkCtrl::UpdateAlgParam(unsigned char* pbData, int iDataSize)
{
	SW_TRACE_DEBUG("CSWSysLinkCtrl::UpdateAlgParam...");
	return CSWBaseLinkCtrl::UpdateAlgParam(pbData, iDataSize);
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
HRESULT CSWSysLinkCtrl::UpdateModel(unsigned int uiModelId, unsigned char* pbData, int iDataSize)
{
	SW_TRACE_DEBUG("CSWSysLinkCtrl::UpdateModel...");
	return CSWBaseLinkCtrl::UpdateModel(uiModelId, pbData, iDataSize);
}

/**
* @brief 图片版识别初始化接口
*
* @param [in] cPhotoRecogerParam : 图片版识别参数。
* @return
* - S_OK : 成功
* - E_FAIL : 失败
*/
HRESULT CSWSysLinkCtrl::InitPhotoRecoger(INIT_PHOTO_RECOGER_PARAM& cPhotoRecogerParam)
{
	cPhotoRecogerParam.cTrackerCfgParam.iPlatform = 2;
#ifdef USE_IPNC_CONTROL	
	return SWPAR_OK == swpa_ipnc_control(SWPA_LINK_DSP, 1, &cPhotoRecogerParam, sizeof(cPhotoRecogerParam), -1) ? S_OK : E_FAIL;
#else
	return CSWBaseLinkCtrl::InitPhotoRecoger(cPhotoRecogerParam);
#endif
}

HRESULT CSWSysLinkCtrl::RegisterCallBack(VOID (*OnDSPAlarm)(PVOID pvParam, INT iInfo), PVOID pvParam)
{
    HRESULT hr = (SWPAR_OK == swpa_device_register_callback_venus(OnDSPAlarm, pvParam) ? S_OK : E_FAIL);
    SW_TRACE_NORMAL("RegisterCallBack(0x%08x,0x%08x) return = 0x%08x.\n", OnDSPAlarm, pvParam, hr);
    return hr;
}

UINT SquareGradient( BYTE *pData
                     , INT nWidth
                     , INT nHeight
                     , SW_RECT rect )
{
    UINT uValue = 0;
    for( INT i = rect.top; i < rect.bottom; i++ )
    {
        BYTE *pYuvData = pData + i*nWidth + rect.left;
        for( INT j = 0 ; j < rect.right ; j++)
        {
            uValue += UINT(swpa_pow((INT)pYuvData[j] - (INT)pYuvData[ j + 1 ] , 2)) ;
        }
    }
    return uValue;
}

HRESULT CSWSysLinkCtrl::EncodeJpeg(CSWImage* pImageSrc
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
	HRESULT hr = E_FAIL;
    if (pImageSrc == NULL)
    {
        SW_TRACE_DEBUG("CSWSysLinkCtrl::EncodeJpeg <pImageSrc == NULL return E_FAIL>.\n");
        return hr;
    }

    BOOL fNeedCrop = FALSE;
    if (NULL != pRect)
    {
        fNeedCrop = !( pRect->left == 0 && pRect->top == 0 && pRect->right == 0 && pRect->bottom == 0 );
    }

    INT iResetWidth = pImageSrc->GetWidth();
    INT iResetHeight = pImageSrc->GetHeight();

    SW_COMPONENT_IMAGE img;
    pImageSrc->GetImage(&img);

    int iNewWidth = 0;
    int iNewHeight = 0;

    int iLeft, iTop, iRight, iBottom;
    iLeft = iTop = iRight = iBottom = 0;
    if( pRect != NULL )
    {
        iLeft = pRect->left;
        iTop = pRect->top;
        iRight = pRect->right;
        iBottom = pRect->bottom;
    }

    if((SENSOR_IMX178 == swpa_get_sensor_type() || SENSOR_ICX816 == swpa_get_sensor_type())
            && pImageSrc->GetType() == SW_IMAGE_YUV420SP && fNeedCrop )
	{
        SW_TRACE_DEBUG("CSWSysLinkCtrl::EncodeJpeg <Enter Crop Function$$$$$$$$$$$$$>\n");
		if( SWPAR_OK == swpa_ipnc_Crop(pImageSrc->GetImageBuffer(), iLeft, iTop, iRight, iBottom, &iNewWidth, &iNewHeight ) )
		{
//            SW_TRACE_DEBUG("swpa_ipnc_Crop ok.\n");
//			img.len[0] *= 2;
//			img.len[1] *= 2;
			img.iHeight = iNewHeight;
			img.iWidth = iNewWidth;
			pImageSrc->SetImage(img);
		}
    }
	if (NULL != pImageSrc && NULL != ppImageJpeg && SWPAR_OK == swpa_ipnc_send_data(SWPA_LINK_VIDEO, pImageSrc->GetImageBuffer(), pImageSrc->GetSize(), NULL, 0))
	{
		CSWImage* pImage = (CSWImage *)swpa_list_remove(m_hlstJpeg, 0, 1000);
		if (NULL == pImage)
		{
			SW_TRACE_DEBUG("Err: NULL == pImage");
			return E_FAIL;
		}
		// todo.
		//  由于VIDEO缓冲区不够，需要COPY
		// 对大小进行判断
		// 应加上重压缩
		INT iImageMemSize = pImage->GetImageBufferSize() + EXIF_BUFFER_MAX_SIZE;	//512 字节是为Exif预留的大小
		if( pImage->GetImageBufferSize() <= 1 * 1024 * 1024)
		{
			CSWImage* pImageCopy = NULL;
			if( S_OK == CSWImage::CreateSWImage(
					&pImageCopy, SW_IMAGE_JPEG, pImage->GetWidth(), pImage->GetHeight(),
					CSWMemoryFactory::GetInstance(SW_SHARED_MEMORY), pImageSrc->GetFrameNo(), pImageSrc->GetRefTime(), pImageSrc->IsCaptureImage(), pImageSrc->GetFrameName(), iImageMemSize))
			{
				swpa_memcpy(pImageCopy->GetImageBuffer(), pImage->GetImageBuffer(), pImage->GetImageBufferSize());
				SW_COMPONENT_IMAGE cImage;
				pImageCopy->GetImage(&cImage);
				cImage.iSize = pImage->GetImageBufferSize();
				pImageCopy->SetImage(cImage);
				*ppImageJpeg = pImageCopy;

	            IMAGE_EXT_INFO cExtInfo;
	            pImageSrc->GetImageExtInfo(&cExtInfo);
	            pImageCopy->SetImageExtInfo(cExtInfo);
				//Security code
				BYTE pbBuf[16];
				pImageSrc->GetSecurityCode(pbBuf,sizeof(pbBuf));
				pImageCopy->SetSecurityCode(pbBuf,sizeof(pbBuf));


				pImage->Release();
			}
			else
			{
				*ppImageJpeg = pImage;
			}
		}
		else
		{
			pImage->Release();
			// JPEG太大导致的压缩失败，返回特定值。
			hr = E_OUTOFMEMORY;
		}

		if(*ppImageJpeg)
		{
			hr = S_OK;
		}
	}

    // 截图后 这里需要还原 YUV
    if (TRUE == fNeedCrop)
    {
        if( SWPAR_OK == swpa_ipnc_Crop_reset(pImageSrc->GetImageBuffer(), iLeft, iTop, iRight, iBottom, iResetWidth, iResetHeight, &iNewWidth, &iNewHeight ) )
        {
//            SW_TRACE_DEBUG("swpa_ipnc_Crop_reset ok.\n");
//            img.len[0] *= 2;
//            img.len[1] *= 2;
            img.iHeight = iNewHeight;
            img.iWidth = iNewWidth;
            pImageSrc->SetImage(img);
        }
    }
	return hr;
}
	
HRESULT CSWSysLinkCtrl::DoProcess(CSWImage* pImage, const INT& iTimeOut)
{
#ifdef PR_IN_ARM
	if(NULL != pImage)
	{
		DWORD dwCmdID;		
		BYTE* pbBuf = (BYTE *)m_pMemoryData->GetBuffer();
		memcpy(&dwCmdID, pbBuf, sizeof(DWORD)); pbBuf += sizeof(DWORD);
		SW_TRACE_NORMAL("dwCmdID:0x%d", dwCmdID);
		if(dwCmdID == DCI_PROCESS_ONE_FRAME)
		{
			if(!m_fInitEPApp)
			{
				m_fInitEPApp = TRUE;
				SW_TRACE_NORMAL("param:0x%08x, width:%d,height:%d", &m_cVideoRecogerParam.cTrackerCfgParam, pImage->GetWidth(), pImage->GetHeight());
				m_cEPApp.Init(&m_cVideoRecogerParam.cTrackerCfgParam, NULL, NULL, pImage->GetWidth(), pImage->GetHeight());
			}		
			
			PROCESS_ONE_FRAME_PARAM* pProcessParam = (PROCESS_ONE_FRAME_PARAM *)(pbBuf + sizeof(DWORD)); pbBuf += sizeof(PROCESS_ONE_FRAME_PARAM) + sizeof(DWORD);
			PROCESS_ONE_FRAME_DATA* pProcessData = (PROCESS_ONE_FRAME_DATA *)(pbBuf + sizeof(DWORD)); pbBuf += sizeof(PROCESS_ONE_FRAME_DATA) + sizeof(DWORD);
			PROCESS_ONE_FRAME_RESPOND* pProcessRespond = (PROCESS_ONE_FRAME_RESPOND *)(pbBuf + sizeof(DWORD)); pbBuf += sizeof(PROCESS_ONE_FRAME_RESPOND) + sizeof(DWORD);
			DWORD dwNow = CSWDateTime::GetSystemTick();
		  HRESULT hr = m_cEPApp.Process(pProcessParam, pProcessData, pProcessRespond);
		  dwNow = CSWDateTime::GetSystemTick() - dwNow;
		  SW_TRACE_NORMAL("=======================m_cEPApp.Process: %d ms, hr:0x%08x=================", dwNow, hr);
		}
	}
#endif	
	HRESULT hr = E_FAIL;
	int iAddr = (int)m_pMemoryPoint->GetBuffer(1);

	swpa_ipnc_cache_wb(m_pMemoryData->GetBuffer(), m_pMemoryData->GetSize(), 0xFFFF, 1);
	if(NULL != pImage)
	{
		if(SWPAR_OK == swpa_ipnc_send_data(SWPA_LINK_DSP, pImage->GetImageBuffer(), pImage->GetSize(), &iAddr, 4))
		{
			IPNC_DATA* data = (IPNC_DATA *)swpa_list_remove(m_hlstDspRes, 0, iTimeOut);
			if(data)
			{
				hr = S_OK;
				swpa_ipnc_release(data->addr[0]);
				delete data;
			}
			else
			{
				hr = E_FAIL;
			}
		}
	}	
	else if(SWPAR_OK == swpa_ipnc_control(SWPA_LINK_DSP, CMD_SET_VIDEO_STREAM_RECOGNIZE_PARAMS, &iAddr, 4, iTimeOut))
	{
		hr = S_OK;
	}
	if(S_OK != hr)
	{
		SW_TRACE_NORMAL("CSWSysLinkCtrl::DoProcess(0x%08x,%d) failed.", pImage, iTimeOut);
	}

	swpa_ipnc_cache_inv(m_pMemoryRespond->GetBuffer(), m_pMemoryRespond->GetSize(), 0xFFFF, 1);
	return hr;
}

