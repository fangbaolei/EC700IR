#include "SWFC.h"
#include "FreetypeUtils.h"
#include "SWJPEGEncodeTransformFilter.h"
#include "SWPosImage.h"
#include "SWBaseLinkCtrl.h"
#include "SWExif.h"
#include "SWCarLeft.h"

CSWJPEGEncodeTransformFilter::CSWJPEGEncodeTransformFilter()
:CSWBaseFilter(1,1)
,CSWMessage(MSG_JPEGENCODE_START, MSG_JPEGENCODE_END)
,ENCODE_FAIL_MAX_COUNT(10)
{
	m_iJPEGType = 0;
	m_iCompressType = 0;
	m_iMinQuantity = 10;
	m_iMaxQuantity = 90;
	m_iQuantity = 60;
	m_iQuantityCapture = 80;
	m_iSize = 0;
	m_iMinSize = 0;
	m_iMaxSize = 0;
	m_pJPEGOverlayInfo = NULL;
	m_iYColor = 255;
	m_iUColor = 128;
	m_iVColor = 128;
	m_pMemory = NULL;
	m_iPlateOverlayOffset = 0;
	m_iEncFailCounter = 0;
	Initialize(0, 0, 60, 10, 90, 80);
	GetIn(0)->AddObject(CLASSID(CSWPosImage));
	GetOut(0)->AddObject(CLASSID(CSWPosImage));

	swpa_memset(&m_cRect, 0, sizeof(m_cRect));
}

CSWJPEGEncodeTransformFilter::~CSWJPEGEncodeTransformFilter()
{
	CSWMemoryFactory *pMemoryFactory = CSWMemoryFactory::GetInstance(SW_SHARED_MEMORY);
	if(m_pMemory)
	{
		pMemoryFactory->Free(m_pMemory);
	}
}

HRESULT CSWJPEGEncodeTransformFilter::Initialize(
	INT iJPEGType
	, INT iCompressType
	, INT iValue
	, INT iMinQuantity
	, INT iMaxQuantity
	, INT iQuantityCapture
	, PVOID pvInfo)
{
	m_iJPEGType = iJPEGType;
	m_iCompressType = iCompressType;
	m_iMinQuantity = iMinQuantity;
	m_iMaxQuantity = iMaxQuantity;
	switch(iCompressType)
	{
	case 0 :
		m_iQuantity = iValue;
		break;
	case 1 :
		m_iQuantity = 50;
        m_iSize = iValue * 1024;
		m_iMinSize = m_iSize - (m_iSize / 5);
		m_iMaxSize = m_iSize + (m_iSize / 5);
		break;
	}
	
	OnJpegSetCompressRate((WPARAM)m_iQuantity, 0);

	m_iQuantityCapture = iQuantityCapture;
	
	m_pJPEGOverlayInfo = (JPEG_OVERLAY_INFO *)pvInfo;
	if(m_pJPEGOverlayInfo)
	{
		BYTE r = (BYTE)( m_pJPEGOverlayInfo->iColor & 0x000000FF);
		BYTE g = (BYTE)((m_pJPEGOverlayInfo->iColor & 0x0000FF00) >> 8);
		BYTE b = (BYTE)((m_pJPEGOverlayInfo->iColor & 0x00FF0000) >> 16);
		
		m_iYColor = (INT)(0.299f * r + 0.587f * g + 0.114f * b);
		m_iUColor = (INT)((b - m_iYColor) * 0.565f + 128);
		m_iVColor = (INT)((r - m_iYColor) * 0.713f + 128);
		
		if(m_iYColor > 255)
		{
			m_iYColor = 255;
		}
		if(m_iUColor > 255)
		{
			m_iUColor = 255;
		}
		if(m_iVColor > 255)
		{
			m_iVColor = 255;
		}
		if(!swpa_strcmp(m_pJPEGOverlayInfo->szInfo, "NULL"))
		{
			swpa_strcpy(m_pJPEGOverlayInfo->szInfo, "");
		}
		else
		{
			//swpa_strcat(m_pJPEGOverlayInfo->szInfo, "       ");
		}
		m_cTTFBitmap.Init("fz_songti.ttf", m_pJPEGOverlayInfo->iFontSize, 0);
		//申请字符叠加的信息
		CSWMemoryFactory *pMemoryFactory = CSWMemoryFactory::GetInstance(SW_SHARED_MEMORY);
		if(NULL != pMemoryFactory)
		{
			m_pMemory = pMemoryFactory->Alloc(2*1024*1024);
			if(NULL != m_pMemory)
			{
				swpa_memset(m_pMemory->GetBuffer(), 0, m_pMemory->GetSize());
			}
		}
		
		PBYTE pbBuf = (PBYTE)m_pMemory->GetBuffer();	
		if(m_pJPEGOverlayInfo->fEnable && NULL != m_pMemory)
  	{  
  		PBYTE pbBitmap = NULL;
  		INT iBitmapSize = 0;
  		INT iBitmapWidth = 0;
  		INT iBitmapHeight = 0;  
  		//生成时间位图			
  		//if(m_pJPEGOverlayInfo->fEnableTime)
  		{ 
  			char *szInfo[] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "-", ":", " ", m_pJPEGOverlayInfo->szInfo, 0};  			
  			for(INT i = 0; szInfo[i]; i++)
  			{
  				if(!swpa_strlen(szInfo[i]))
  				{
  					continue;
  				}
  				m_cTTFBitmap.GenerateBitmap(szInfo[i], ENCODE_GB2312, &pbBitmap, iBitmapSize, iBitmapWidth, iBitmapHeight);
  				if(iBitmapSize > 0)
  				{
  					iBitmapSize = ALGIN_SIZE(iBitmapSize, 4);
  					SW_TRACE_DEBUG("pbBitmap:0x%08x,iBitmapSize:%d,iBitmapWidth:%d,iBitmapHeight:%d", 
						(int)pbBitmap, iBitmapSize, iBitmapWidth, iBitmapHeight);
  					if(pbBuf + sizeof(TEXT) + iBitmapSize > (PBYTE)m_pMemory->GetBuffer() + m_pMemory->GetSize())
 						{
 							SW_TRACE_DEBUG("buffer is too small");
 							iBitmapSize = (PBYTE)m_pMemory->GetBuffer() + m_pMemory->GetSize() - pbBuf - sizeof(TEXT);
 						}
 						if(iBitmapSize > 0)
 						{
  						TEXT *txt = (TEXT *)pbBuf; pbBuf += sizeof(TEXT);
  						if(swpa_strlen(szInfo[i]) == 1)
  						{
  							txt->iAlpha = szInfo[i][0];
  							SW_TRACE_DEBUG("iAlpha:%d,szInfo[i][0]:%c", txt->iAlpha, szInfo[i][0]);
  						}
  						else
  						{
  							txt->iAlpha = '*';
  						}
  						txt->iSize = iBitmapSize;
  						txt->iWidth = iBitmapWidth;
  						txt->iHeight = iBitmapHeight;
  						swpa_memcpy(pbBuf, pbBitmap, iBitmapSize); pbBuf += iBitmapSize;
  					}
  				}
  			}  			
  		}
		}
		m_iPlateOverlayOffset = pbBuf - (PBYTE)m_pMemory->GetBuffer();
	}
	return S_OK;
}

HRESULT CSWJPEGEncodeTransformFilter::OnJpegSetCompressRate(WPARAM wParam, LPARAM lParam)
{
	HRESULT hr = S_OK;
	if( wParam < 100 && wParam > 0 )
	{
		m_iQuantity = wParam;
		DWORD Value = (DWORD)m_iQuantity;
		
		Value = (Value > 80) ? 80 : Value;		//限制为最大80
		
		hr = (0 == swpa_ipnc_control(0, CMD_SET_JPEG_QUALITY ,(void*)&Value , sizeof(DWORD), 0)?S_OK:E_FAIL);
//		SW_TRACE_DEBUG("<JPEGEncodeTransformFilter>JpegSetCompressRate %d.\n", Value);
	}
	return hr;
}

HRESULT CSWJPEGEncodeTransformFilter::OnJpegGetCompressRate(WPARAM wParam, LPARAM lParam)
{
	if(m_iQuantity < 0)
	{
		m_iQuantity = 0;
	}
	else if(m_iQuantity > 100)
	{
		m_iQuantity = 100;
	}
	*(INT *)lParam = m_iQuantity;
	return S_OK;
}

HRESULT CSWJPEGEncodeTransformFilter::OnJpegSetCompressRateCapture(WPARAM wParam, LPARAM lParam)
{
	if( wParam < 100 && wParam > 0 )
	{
		m_iQuantityCapture = wParam;
		SW_TRACE_DEBUG("<JPEGEncodeTransformFilter>JpegSetCompressRateCapture %d.\n", m_iQuantityCapture);
	}
	return S_OK;
}

HRESULT CSWJPEGEncodeTransformFilter::OnJpegGetCompressRateCapture(WPARAM wParam, LPARAM lParam)
{
	if(m_iQuantityCapture < 0)
	{
		m_iQuantityCapture = 0;
	}
	else if(m_iQuantityCapture > 100)
	{
		m_iQuantityCapture = 100;
	}
	*(INT *)lParam = m_iQuantityCapture;
	return S_OK;
}


HRESULT CSWJPEGEncodeTransformFilter::OnSetAutoJPEGCompressEnable(WPARAM wParam, LPARAM lParam)
{
	SW_TRACE_DEBUG("CSWJPEGEncodeTransformFilter::OnSetAutoJPEGCompressEnable(0x%08x, 0x%08x)", wParam, lParam);
	m_iCompressType = (INT)wParam;
	
	return S_OK;
}

HRESULT CSWJPEGEncodeTransformFilter::OnGetAutoJPEGCompressEnable(WPARAM wParam, LPARAM lParam)
{
	SW_TRACE_DEBUG("CSWJPEGEncodeTransformFilter::OnGetAutoJPEGCompressEnable(0x%08x, 0x%08x)", wParam, lParam);
	*(INT *)lParam = m_iCompressType;
	
	return S_OK;
}

HRESULT CSWJPEGEncodeTransformFilter::OnSetAutoJPEGCompressParam(WPARAM wParam, LPARAM lParam)
{
	SW_TRACE_DEBUG("CSWJPEGEncodeTransformFilter::OnSetAutoJPEGCompressParam(0x%08x, 0x%08x)", wParam, lParam);
	INT *pParam = (INT *)wParam;
	m_iMaxQuantity = pParam[0];
	m_iMinQuantity = pParam[1];
    m_iSize = pParam[2] * 1024;
	m_iMinSize = m_iSize - (m_iSize / 5);
	m_iMaxSize = m_iSize + (m_iSize / 5);
	
	return S_OK;
}

HRESULT CSWJPEGEncodeTransformFilter::OnGetAutoJPEGCompressParam(WPARAM wParam, LPARAM lParam)
{
	SW_TRACE_DEBUG("CSWJPEGEncodeTransformFilter::OnGetAutoJPEGCompressParam(0x%08x, 0x%08x)", wParam, lParam);

	INT *pParam = (INT *)lParam;
	pParam[0] = m_iMaxQuantity;
	pParam[1] = m_iMinQuantity;
    pParam[2] = m_iSize / 1024;

	return S_OK;
}

HRESULT CSWJPEGEncodeTransformFilter::Receive(CSWObject* obj)
{
	if(IsDecendant(CSWPosImage, obj))
	{
		CSWPosImage* pPosImage = (CSWPosImage *)obj;
		if(swpa_strcmp("VPIF", pPosImage->GetImage()->GetFrameName()) == 0)
		{
			//压缩JPEG
			CSWImage * pImage = pPosImage->GetImage();
			if(S_OK == OnJpegEncode((WPARAM)&pImage, 0))
			{ //传送
				pPosImage->SetImage(pImage);
				SAFE_RELEASE(pImage);
				GetOut(0)->Deliver(pPosImage);	
			}
		}
		else
		{
			CSWPosImage *pPosImageTmp = new CSWPosImage(pPosImage->GetImage()->GetLastImage(), pPosImage->GetCount(), pPosImage->GetDebugFlag());
			for(INT i = 0; i < pPosImageTmp->GetCount(); i++)
			{
				pPosImageTmp->GetRect(i) = pPosImage->GetRect(i);
			}			
			GetOut(0)->Deliver(pPosImageTmp);	
			pPosImageTmp->Release();
		}
	}
	return S_OK;
}

HRESULT CSWJPEGEncodeTransformFilter::OnSetJPEGCompressRect(WPARAM wParam, LPARAM lParam)
{
	if( wParam == NULL )
	{
		return E_POINTER;
	}
	int *prgiTemp = (int*)wParam;
	m_cRect.left = prgiTemp[0];
	m_cRect.top = prgiTemp[1];
	m_cRect.right = prgiTemp[2];
	m_cRect.bottom = prgiTemp[3];

	SW_TRACE_DEBUG("<SetJPEGRect:(%d,%d,%d,%d)>",
			m_cRect.left, m_cRect.top, m_cRect.right, m_cRect.bottom);

	return S_OK;
}

HRESULT CSWJPEGEncodeTransformFilter::OnJpegEncode(WPARAM wParam, LPARAM lParam)
{
    CSWImage** ppYUVImage = (CSWImage **)wParam;
    CSWCarLeft* pCarLeft = (CSWCarLeft*)lParam;
    LPCSTR szPlate = NULL;
    //BOOL fCrop = FALSE;
   	if (NULL != pCarLeft)
    {
        szPlate = (LPCSTR)pCarLeft->GetPlateNo();
        //fCrop = pCarLeft->m_fCropLastImage;
    }

    CSWImage* pJPEGImage = NULL;
	
	if(NULL == ppYUVImage || NULL == (*ppYUVImage))
	{
		return E_FAIL;
	}
	//测试协议JPEG图
	if((*ppYUVImage)->GetLastImage())
	{
		*ppYUVImage = (*ppYUVImage)->GetLastImage();
		(*ppYUVImage)->AddRef();
		return S_OK;
	}

	m_cJpegMutex.Lock();

	if( (*ppYUVImage)->GetType() == SW_IMAGE_JPEG )
	{
		SW_TRACE_NORMAL("<jpeg encode filter>image type jpeg.");
		m_cJpegMutex.Unlock();
		return S_OK;
	}

	INT iQuantity = (*ppYUVImage)->IsCaptureImage() ? m_iQuantityCapture : m_iQuantity;
	  
  CSWMemoryFactory* pMemoryFactory = CSWMemoryFactory::GetInstance(SW_SHARED_MEMORY);
	//CSWMemory *pMemory = NULL;
			
	CSWString strInfo;
  /*if(m_pJPEGOverlayInfo && m_pJPEGOverlayInfo->fEnable)
  {  		
  		if(m_pJPEGOverlayInfo->fEnableTime)
  		{
  			CSWDateTime dtNow((*ppYUVImage)->GetRefTime());  		
  			strInfo.Format("%04d-%02d-%02d %02d:%02d:%02d %03d\n"
					, dtNow.GetYear()
					, dtNow.GetMonth()
					, dtNow.GetDay()
					, dtNow.GetHour()
					, dtNow.GetMinute()
					, dtNow.GetSecond()
					, dtNow.GetMSSecond()
				);
  		}
  		strInfo += "*";
        if(m_pJPEGOverlayInfo->fEnablePlate && NULL != szPlate && swpa_strlen(szPlate) > 0)
  		{
  			strInfo += " ?";
  			PBYTE pbBitmap = NULL;
  			INT iBitmapSize = 0;
  			INT iBitmapWidth = 0;
  			INT iBitmapHeight = 0;
  			PBYTE pbBuf = (PBYTE)m_pMemory->GetBuffer() + m_iPlateOverlayOffset;
  			CSWString strPlate;
  			strPlate.Format("%s   ", szPlate);
  			m_cTTFBitmap.GenerateBitmap((LPCSTR)strPlate, ENCODE_GB2312, &pbBitmap, iBitmapSize, iBitmapWidth, iBitmapHeight);
  			if(iBitmapSize > 0)
  			{
  				iBitmapSize = ALGIN_SIZE(iBitmapSize, 4);
  				
  				TEXT *txt = (TEXT *)pbBuf; pbBuf += sizeof(TEXT);
 					txt->iAlpha = '?';
  				txt->iSize = iBitmapSize;
  				txt->iWidth = iBitmapWidth;
  				txt->iHeight = iBitmapHeight;
  				swpa_memcpy(pbBuf, pbBitmap, iBitmapSize); pbBuf += iBitmapSize;
  			}
  		}
	}*/
	HRESULT hr = E_FAIL;		
	//限制最大80
	iQuantity = (iQuantity > 80) ? 80 : iQuantity;
    /*if (TRUE == fCrop)      // 需要截取特征图
    {
        //计算坐标中心点
        HV_RECT pLastRect = pCarLeft->GetLastPlateRect();
        // 原来的坐标是场格式，高度需要 *２
        INT iLeft = pLastRect.left;
        INT iTop = pLastRect.top * 2;
        INT iRight = pLastRect.right;
        INT iBottom = pLastRect.bottom * 2;

        INT iCenterX = (iLeft + iRight) / 2;
        INT iCenterY = (iTop + iBottom) / 2;

        INT iCropWidth = 1280;
        INT iCropHeight = 640;
        CSWCarLeft::GetCropWidthHeight(iCropWidth, iCropHeight);

        //调整中心点
        if(iCenterX < iCropWidth / 2)    //如果中心点距离左边缘 < 固定截图的分辨率宽度的一半
        {
            iCenterX = iCropWidth / 2;
        }
        else if(iCenterX > 3392 - iCropWidth / 2 - 1) //如果中心点距离右边缘 > 固定截图的分辨率宽度的一半
        {
            iCenterX = 3392 - iCropWidth / 2 - 1;
        }

        if(iCenterY < iCropHeight / 2)  //如果中心点距离上边缘 < 固定截图的分辨率高度的一半
        {
            iCenterY = iCropHeight / 2;
        }
        else if(iCenterY > 2000 - iCropHeight / 2 - 1)  //如果中心点距离下边缘 > 固定截图的分辨率高度的一半
        {
            iCenterY = 2000 - iCropHeight / 2 - 1;
        }
        //得到区域
        iLeft = iCenterX - iCropWidth / 2;
        iTop = iCenterY - iCropHeight / 2;
        iRight = iCenterX + iCropWidth / 2;
        iBottom = iCenterY + iCropHeight / 2;
        SW_RECT rectCrop = {iLeft, iTop, iRight, iBottom};

        hr = CSWBaseLinkCtrl::GetInstance()->EncodeJpeg(
                (*ppYUVImage)
                , &pJPEGImage
                , iQuantity
                , m_iJPEGType
                , m_pJPEGOverlayInfo->fEnable ? (PBYTE)m_pMemory->GetBuffer(MEM_PHY_ADDR) : NULL
                , m_pJPEGOverlayInfo->fEnable ? (LPCSTR)strInfo : NULL
                , m_pJPEGOverlayInfo->iX
                , m_pJPEGOverlayInfo->iY
                , m_iYColor
                , m_iUColor
                , m_iVColor
                , &rectCrop
                , fCrop);
    }
    else
    {
        hr = CSWBaseLinkCtrl::GetInstance()->EncodeJpeg(
                (*ppYUVImage)
                , &pJPEGImage
                , iQuantity
                , m_iJPEGType
                , m_pJPEGOverlayInfo->fEnable ? (PBYTE)m_pMemory->GetBuffer(MEM_PHY_ADDR) : NULL
                , m_pJPEGOverlayInfo->fEnable ? (LPCSTR)strInfo : NULL
                , m_pJPEGOverlayInfo->iX
                , m_pJPEGOverlayInfo->iY
                , m_iYColor
                , m_iUColor
                , m_iVColor
                , &m_cRect
                , fCrop);
    }*/
	
	hr = CSWBaseLinkCtrl::GetInstance()->EncodeJpeg(
			(*ppYUVImage)
			, &pJPEGImage
			, iQuantity
			, m_iJPEGType
			, m_pJPEGOverlayInfo->fEnable ? (PBYTE)m_pMemory->GetBuffer(MEM_PHY_ADDR) : NULL
			, m_pJPEGOverlayInfo->fEnable ? (LPCSTR)strInfo : NULL
			, m_pJPEGOverlayInfo->iX
			, m_pJPEGOverlayInfo->iY
			, m_iYColor
			, m_iUColor
			, m_iVColor
			, &m_cRect);
	
	if(S_OK == hr)
	{
		if(m_iCompressType == 1 && !(*ppYUVImage)->IsCaptureImage())
		{
			if(pJPEGImage->GetSize() > m_iMaxSize && m_iQuantity > m_iMinQuantity && m_iQuantity > 0)
			{
//				SW_TRACE_DEBUG("======-Jpeg Size %d Exp Size %d(%d~%d),Q %d max %d min %d\n",
//					pJPEGImage->GetSize(),m_iSize,m_iMinSize,m_iMaxSize, m_iQuantity,m_iMaxQuantity,m_iMinQuantity);
				m_iQuantity-=3;
				m_iQuantity = m_iQuantity < m_iMinQuantity ? m_iMinQuantity : m_iQuantity;
				OnJpegSetCompressRate(m_iQuantity,0);
			}
			else if(pJPEGImage->GetSize() < m_iMinSize && m_iQuantity < m_iMaxQuantity && m_iQuantity < 100)
			{
//				SW_TRACE_DEBUG("======+Jpeg Size %d Exp Size %d(%d~%d),Q %d max %d min %d\n",
//					pJPEGImage->GetSize(),m_iSize,m_iMinSize,m_iMaxSize,m_iQuantity,m_iMaxQuantity,m_iMinQuantity);
				m_iQuantity+=3;
				m_iQuantity = m_iQuantity > m_iMaxQuantity ? m_iMaxQuantity : m_iQuantity;
				OnJpegSetCompressRate(m_iQuantity,0);
			}
		}
#ifdef _EXIF_H
		INT iMode = swpa_get_sensor_type();
		if(SENSOR_IMX178 == iMode || SENSOR_IMX185 == iMode 
			|| SENSOR_IMX174 == iMode || SENSOR_IMX249 == iMode
			|| SENSOR_ICX816 == iMode)
		{
			CSWDateTime dt((*ppYUVImage)->GetRefTime());
			strInfo.Format("%04d:%02d:%02d %02d:%02d:%02d", dt.GetYear(), dt.GetMonth(), dt.GetDay(), dt.GetHour(), dt.GetMinute(), dt.GetSecond());		
			IMAGE_EXT_INFO info;
			(*ppYUVImage)->GetImageExtInfo(&info);
			BYTE pbExif[EXIF_BUFFER_MAX_SIZE] = {0};
			HvExifInfo exif;
	  	
			if(SENSOR_IMX178 == iMode)
			{
				swpa_strcpy(exif.szCameraMake,"IMX178");
				swpa_strcpy(exif.szCameraModel,"IMX178");
			}
			else if (SENSOR_IMX185 == iMode)
			{
				swpa_strcpy(exif.szCameraMake,"IMX185");
				swpa_strcpy(exif.szCameraModel,"IMX185");
			}
			else if (SENSOR_IMX174 == iMode)
			{
				swpa_strcpy(exif.szCameraMake, "IMX174");
				swpa_strcpy(exif.szCameraModel, "IMX174");
			}
			else if (SENSOR_IMX249 == iMode)
			{
				swpa_strcpy(exif.szCameraMake, "IMX249");
				swpa_strcpy(exif.szCameraModel, "IMX249");
			}
			else if (SENSOR_ICX816 == iMode)
			{
				swpa_strcpy(exif.szCameraMake, "ICX816");
				swpa_strcpy(exif.szCameraModel, "ICX816");
			}
			
			swpa_strcpy(exif.szDateTime,(LPCSTR)strInfo);
			exif.iExposureTime=info.iShutter;
			exif.iGain = info.iGain;
			exif.iFilter=0;
			exif.iContrast = info.iRGain;
			exif.iSaturation = info.iGGain;
			exif.iSharpness = info.iBGain;
			INT iSize = 0;
			CreateExif(&exif,(char *)pbExif, iSize);

			//pYUVImage->Attach(pJPEGImage, exif.GetBuffer(), exif.GetSize());
			SW_COMPONENT_IMAGE img;
			pJPEGImage->GetImage(&img);
			swpa_memmove(img.rgpbData[0] + 2 + iSize, img.rgpbData[0] + 2, img.iSize);
			swpa_memcpy(img.rgpbData[0] + 2, pbExif, iSize);
			img.iSize += iSize;
			pJPEGImage->SetImage(img);
			SAFE_ADDREF(pJPEGImage);
			*ppYUVImage = pJPEGImage;
		}
		else
#endif
		{
			//pYUVImage->Attach(pJPEGImage);
			SAFE_ADDREF(pJPEGImage);
			*ppYUVImage = pJPEGImage;
		}
		pJPEGImage->Release();
		m_iEncFailCounter = 0;				//只要有成功的就重置
		hr = S_OK;
	}
	else
	{
		// 如果是超大就自动减压缩率。
		if( hr == E_OUTOFMEMORY )
		{
			m_iQuantity -= (m_iQuantity * 20 / 100);
			if( m_iQuantity <= 0 )
			{
				m_iQuantity = 5;
			}
			OnJpegSetCompressRate(m_iQuantity,0);
			SW_TRACE_DEBUG("<JPEGEncodeTransformFilter>EncodeJpeg failed: out of memory.\n");
		}
		if(m_iEncFailCounter >= ENCODE_FAIL_MAX_COUNT)
		{	
			SW_TRACE_DEBUG("Reset Device\n");		
			CSWMessage::SendMessage(MSG_APP_RESETDEVICE, 0, 0);
		}
		else
		{
			m_iEncFailCounter++;
		}
		SW_TRACE_DEBUG("<JPEGEncodeTransformFilter>EncodeJpeg failed.\n");
	}

	m_cJpegMutex.Unlock();
    return hr;
}


