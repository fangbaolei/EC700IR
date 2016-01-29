#include "SWFC.h"
#include "SWPosImage.h"
#include "SWJPEGOverlayFilter.h"

CSWJPEGOverlayFilter::CSWJPEGOverlayFilter()
	: CSWMessage(MSG_OVERLAY_JPEG_BEGIN, MSG_OVERLAY_JPEG_END)
	, m_fMJPEGDoStrOverlay(FALSE)
	, m_fDebug(FALSE)
{
	
	GetIn(0)->AddObject(CLASSID(CSWPosImage));
	GetOut(0)->AddObject(CLASSID(CSWPosImage));
}

CSWJPEGOverlayFilter::~CSWJPEGOverlayFilter()
{
}

HRESULT CSWJPEGOverlayFilter::Receive(CSWObject* obj)
{
	if(IsDecendant(CSWPosImage, obj))
	{
		CSWPosImage *pPosImage = (CSWPosImage *)obj;
		SW_COMPONENT_IMAGE img;
		if(swpa_strcmp("VPIF", pPosImage->GetImage()->GetFrameName()) != 0
				|| pPosImage->GetImage()->IsOverlayed())
		{
			GetOut(0)->Deliver(obj);
			return S_OK;
		}
		pPosImage->GetImage()->GetImage(&img);
		/*if(SENSOR_IMX178 == swpa_get_sensor_type() && pPosImage->GetImage()->GetType() == SW_IMAGE_YUV420SP)
		{
			if(SWPAR_OK == swpa_ipnc_resample(pPosImage->GetImage()->GetImageBuffer(), (void **)&img.rgpbData[0], (void **)&img.rgpbData[1]))
			{
				img.iHeight *= 2;
				img.len[0] *= 2;
				img.len[1] *= 2;
				pPosImage->GetImage()->SetImage(img);
			}
		}*/
		if(img.cImageType == SW_IMAGE_YUV420SP)
		{
#if 1
            PolygonOverlay(pPosImage);
#else
			
#ifdef YUV_TEST
			pPosImage->SetCount(1);
			SW_RECT &rc = pPosImage->GetRect(0);
			rc.left = img.iWidth/2 - 100;
			rc.top = img.iHeight/2 - 100;
			rc.right = rc.left + 200;
			rc.bottom = rc.top + 200;
#endif
			BYTE *yDst[2], *uvDst[2], y, u, v;
			CSWUtils::RGB2YUV(255, 0, 0, &y, &u, &v);
			for(INT i =  0; i < pPosImage->GetCount(); i++)
			{
				SW_RECT rc = pPosImage->GetRect(i);
#define my_mid(a,b,c) (((a) < (b)) ? (b) : (((a) > (c)) ? (c) : (a)))
				rc.left = my_mid(rc.left, 1, img.iWidth -1);
				rc.top = my_mid(rc.top, 1, img.iHeight - 1);
				rc.right = my_mid(rc.right, 1, img.iWidth - 1);
				rc.bottom = my_mid(rc.bottom, 1, img.iHeight - 1);
				
				rc.left &= ~1;
				rc.right &= ~1;
								
				yDst[0] = img.rgpbData[0] + img.rgiStrideWidth[0]*rc.top + rc.left;
				uvDst[0] = img.rgpbData[1] + img.rgiStrideWidth[1]*(rc.top/2) + rc.left;
				
				yDst[1] = img.rgpbData[0] + img.rgiStrideWidth[0]*rc.bottom + rc.left;
				uvDst[1] = img.rgpbData[1] + img.rgiStrideWidth[1]*(rc.bottom/2) + rc.left;

				int iyStride = img.rgiStrideWidth[0];
				int iuvStride = img.rgiStrideWidth[1];
				for(int w = rc.left; w < rc.right; w+= 2)
				{
					*yDst[0] = *(yDst[0] + 1) = *yDst[1] = *(yDst[1] + 1) = y;
					*(yDst[0] + iyStride) =
							*(yDst[0] + 1 + iyStride) =
									*(yDst[1] + iyStride) = *(yDst[1] + 1 + iyStride) = y;
					*uvDst[0] = *uvDst[1] = u;
					*(uvDst[0] + iuvStride) = *(uvDst[1] + iuvStride) = u;
					*(uvDst[0] + 1) = *(uvDst[1] + 1) = v;
					*(uvDst[0] + 1 + iuvStride) = *(uvDst[1] + 1 + iuvStride) = v;
					
					yDst[0] += 2;
					yDst[1] += 2;
					uvDst[0] += 2;
					uvDst[1] += 2;
				}
				for(int h = rc.top; h < rc.bottom; h++)
				{
					yDst[0] = img.rgpbData[0] + img.rgiStrideWidth[0]*h + rc.left;
					uvDst[0] = img.rgpbData[1] + img.rgiStrideWidth[1]*(h/2) + rc.left;
					
					yDst[1] = img.rgpbData[0] + img.rgiStrideWidth[0]*h + rc.right;
					uvDst[1] = img.rgpbData[1] + img.rgiStrideWidth[1]*(h/2) + rc.right;
					
					*yDst[0] = y;
					*(yDst[0] + 1) = y;

					*uvDst[0]++ = u;
					*uvDst[0]++ = v;
					
					*yDst[1] = y;
					*(yDst[1] + 1) = y;
					*uvDst[1]++ = u;
					*uvDst[1]++ = v;
				}
			}
			pPosImage->SetCount(0);
#endif

		}

		//JPEG时给出叠加的效果，不输出通配符
        CSWCarLeft carLeft;
		CSWCarLeft *pCarLeft = NULL;
		if (TRUE == m_fMJPEGDoStrOverlay)//MJPEG叠加假的结果信息，方便上位机及时看到叠加效果
		{
			pCarLeft = &carLeft;
			pCarLeft->SetPlateNo("蓝桂ABC123");
			pCarLeft->SetRoadNo(1);
			pCarLeft->SetCarType(CT_SMALL);
			pCarLeft->SetCarspeed(65.0);
		}

		OnOverlayProcess((WPARAM)pPosImage->GetImage(), (LPARAM)pCarLeft);

	}
	GetOut(0)->Deliver(obj);
	return S_OK;
}



HRESULT CSWJPEGOverlayFilter::LineOverlay(SW_COMPONENT_IMAGE& img, SW_POINT& sPointA, SW_POINT& sPointB)
{
	INT iStartX = sPointA.x < sPointB.x ? sPointA.x : sPointB.x;
	INT iStopX = sPointA.x > sPointB.x ? sPointA.x : sPointB.x;
	INT iStartY = sPointA.y < sPointB.y ? sPointA.y : sPointB.y;
	INT iStopY = sPointA.y > sPointB.y ? sPointA.y : sPointB.y;

	BYTE yy, uu, vv;
	CSWUtils::RGB2YUV(255, 0, 0, &yy, &uu, &vv);

	DOUBLE dblK = 0, dblC = 0;
	
	if (iStartX != iStopX)
	{
		dblK = (1.0 * sPointA.y - sPointB.y) / (1.0 * sPointA.x - sPointB.x);
		dblC = 1.0 * sPointA.y - dblK * sPointA.x;
	}
	INT iBold = (INT)swpa_fabs(dblK);
	iBold = iBold < 2 ? 2 : iBold;
	
	if (iStartX == iStopX || iBold > 2)
	{		
		for (INT i = iStartY; i<=iStopY; i++)
		{
			INT iX = iStartX == iStopX ? iStartX : (i - dblC) / dblK;
				
			if (i >=0 && i < img.iHeight)
			{
				PBYTE pbY = img.rgpbData[0] + img.rgiStrideWidth[0] * i + iX;
				*pbY = yy;
				PBYTE pbUV = img.rgpbData[1] + img.rgiStrideWidth[1]*(i/2) + (iX & ~1);
				*pbUV = uu;
				*(pbUV+1) = vv;

				if (iX>=0 && iX<img.iWidth-1)
				{
					*(pbY+1) = yy;
					*(pbUV+img.rgiStrideWidth[1]) = uu;
					*(pbUV+img.rgiStrideWidth[1]+1) = vv;
				}
			}
		}
	}
	else
	{
	    iStartX = (sPointA.y < sPointB.y) ? sPointA.x : sPointB.x;
        iStopX = (sPointA.y < sPointB.y) ? sPointB.x : sPointA.x;
		for (INT i = iStartX; 
             (iStartX < iStopX ? i<=iStopX : i>=iStopX);
             iStartX < iStopX ? i++ : i--)
		{
			INT iY = dblK * i + dblC;

			if (i >=0 && i < img.iWidth && iY >=0 && iY < img.iHeight)
			{
				PBYTE pbY = img.rgpbData[0] + img.rgiStrideWidth[0] * iY + i;
				*pbY = yy;
				PBYTE pbUV = img.rgpbData[1] + img.rgiStrideWidth[1]*(iY/2) + (i & ~1);
				*pbUV = uu;
				*(pbUV+1) = vv;

				//bold 
				/*for (INT j=1; j<iBold; j++)
				{
					if (iY + j < img.iHeight)
					{
						*(pbY + img.rgiStrideWidth[0] * j) = yy;
						*(pbUV+img.rgiStrideWidth[1] * j) = uu;
						*(pbUV+img.rgiStrideWidth[1] * j+1) = vv;
					}
				}*/
			}
		}
	}

	return S_OK;
}


HRESULT CSWJPEGOverlayFilter::PolygonOverlay(CSWPosImage* pPosImage)
{
    if (NULL == pPosImage)
    {
        return E_INVALIDARG;
    }
    
#ifdef YUV_TEST {{100, 100}, {200, 1000}, {1700, 500}};//
    SW_POINT arrPointG1[] = {{0, 0}, {100, 345}, {0, 1080}, {1678, 789}, {1920, 1080}, {1900, 980}, {1920, 0}, {540, 200}};
    pPosImage->SetPolygon(0, (sizeof(arrPointG1)/sizeof(arrPointG1[0])), arrPointG1);
#endif

    SW_COMPONENT_IMAGE img;
    pPosImage->GetImage()->GetImage(&img);
    
    //SW_POINT* psPoints = new SW_POINT[pPosImage->GetPolygonMaxCount()];
    //if (NULL == psPoints)
    //{
    //    SW_TRACE_DEBUG("Err: no mem for polygon points.\n");
    //    return E_OUTOFMEMORY;
    //}

    SW_POINT psPoints[32] = {{0,0}};
    
    for (INT i=0; i<pPosImage->GetPolygonMaxCount(); i++)
    {    
        INT iVertexCount = 0;
        if (SUCCEEDED(pPosImage->GetPolygon(i, iVertexCount, psPoints)))
        {
            for (INT j=0; j<iVertexCount; j++)
            {
                LineOverlay(img, psPoints[j], psPoints[(j+1) % iVertexCount]);
            }
        }
    }

    //delete []psPoints;


#ifdef YUV_TEST
    pPosImage->SetCount(1);
    SW_RECT &rc = pPosImage->GetRect(0);
    rc.left = img.iWidth/2 - 100;
    rc.top = img.iHeight/2 - 100;
    rc.right = rc.left + 200;
    rc.bottom = rc.top + 200;
#endif

    
    for(INT i =  0; i < pPosImage->GetCount(); i++)
    {
        SW_RECT rc = pPosImage->GetRect(i);
        SW_POINT arrRectPoint[] = {{rc.left, rc.top}, {rc.left, rc.bottom}, {rc.right, rc.bottom}, {rc.right, rc.top}};
        for (INT j=0; j<4; j++)
        {
            LineOverlay(img, arrRectPoint[j], arrRectPoint[(j+1) % 4]);
        }
    }

    pPosImage->SetCount(0);
    
    return S_OK;
}


HRESULT CSWJPEGOverlayFilter::OnOverlayDoProcess(WPARAM wParam, LPARAM lParam)
{
	CSWObject *obj = (CSWObject *)wParam;
	if(IsDecendant(CSWPosImage, obj))
	{
		CSWPosImage *pPosImage = (CSWPosImage *)wParam;
		SW_COMPONENT_IMAGE img;
		pPosImage->GetImage()->GetImage(&img);
		if(pPosImage->GetImage()->IsOverlayed())
		{
			return S_OK;
		}
		if(SENSOR_IMX178 == swpa_get_sensor_type() && pPosImage->GetImage()->GetType() == SW_IMAGE_YUV420SP)
		{
			if(SWPAR_OK == swpa_ipnc_resample(pPosImage->GetImage()->GetImageBuffer(), (void **)&img.rgpbData[0], (void **)&img.rgpbData[1]))
			{
				img.iHeight *= 2;
				img.len[0] *= 2;
				img.len[1] *= 2;
				pPosImage->GetImage()->SetImage(img);
			}
		}

		if(img.cImageType == SW_IMAGE_YUV420SP)
		{
#if 1
            PolygonOverlay(pPosImage);
#else
#ifdef YUV_TEST
			pPosImage->SetCount(1);
			SW_RECT &rc = pPosImage->GetRect(0);
			rc.left = img.iWidth/2 - 100;
			rc.top = img.iHeight/2 - 100;
			rc.right = rc.left + 200;
			rc.bottom = rc.top + 200;
#endif
			BYTE *yDst[2], *uvDst[2], y, u, v;
			CSWUtils::RGB2YUV(255, 0, 0, &y, &u, &v);
			for(INT i =  0; i < pPosImage->GetCount(); i++)
			{
				SW_RECT rc = pPosImage->GetRect(i);
#define my_mid(a,b,c) (((a) < (b)) ? (b) : (((a) > (c)) ? (c) : (a)))
				rc.left = my_mid(rc.left, 1, img.iWidth -1);
				rc.top = my_mid(rc.top, 1, img.iHeight - 1);
				rc.right = my_mid(rc.right, 1, img.iWidth - 1);
				rc.bottom = my_mid(rc.bottom, 1, img.iHeight - 1);
				
				rc.left &= ~1;
				rc.right &= ~1;
								
				yDst[0] = img.rgpbData[0] + img.rgiStrideWidth[0]*rc.top + rc.left;
				uvDst[0] = img.rgpbData[1] + img.rgiStrideWidth[1]*(rc.top/2) + rc.left;
				
				yDst[1] = img.rgpbData[0] + img.rgiStrideWidth[0]*rc.bottom + rc.left;
				uvDst[1] = img.rgpbData[1] + img.rgiStrideWidth[1]*(rc.bottom/2) + rc.left;

				int iyStride = img.rgiStrideWidth[0];
				int iuvStride = img.rgiStrideWidth[1];
				for(int w = rc.left; w < rc.right; w+= 2)
				{
					*yDst[0] = *(yDst[0] + 1) = *yDst[1] = *(yDst[1] + 1) = y;
					*(yDst[0] + iyStride) =
							*(yDst[0] + 1 + iyStride) =
									*(yDst[1] + iyStride) = *(yDst[1] + 1 + iyStride) = y;
					*uvDst[0] = *uvDst[1] = u;
					*(uvDst[0] + iuvStride) = *(uvDst[1] + iuvStride) = u;
					*(uvDst[0] + 1) = *(uvDst[1] + 1) = v;
					*(uvDst[0] + 1 + iuvStride) = *(uvDst[1] + 1 + iuvStride) = v;
					
					yDst[0] += 2;
					yDst[1] += 2;
					uvDst[0] += 2;
					uvDst[1] += 2;
				}
				for(int h = rc.top; h < rc.bottom; h++)
				{
					yDst[0] = img.rgpbData[0] + img.rgiStrideWidth[0]*h + rc.left;
					uvDst[0] = img.rgpbData[1] + img.rgiStrideWidth[1]*(h/2) + rc.left;
					
					yDst[1] = img.rgpbData[0] + img.rgiStrideWidth[0]*h + rc.right;
					uvDst[1] = img.rgpbData[1] + img.rgiStrideWidth[1]*(h/2) + rc.right;
					
					*yDst[0] = y;
					*(yDst[0] + 1) = y;

					*uvDst[0]++ = u;
					*uvDst[0]++ = v;
					
					*yDst[1] = y;
					*(yDst[1] + 1) = y;
					*uvDst[1]++ = u;
					*uvDst[1]++ = v;
				}
			}
			pPosImage->SetCount(0);
#endif
		}
#ifdef YUV_TEST
			CSWCarLeft carLeft;
			carLeft.SetRoadInfo("测试", "正常ok");
			OnOverlayProcess((WPARAM)pPosImage->GetImage(), (LPARAM)&carLeft);
#else
			OnOverlayProcess((WPARAM)pPosImage->GetImage(), NULL);
#endif	
	}
	return S_OK;
}

HRESULT CSWJPEGOverlayFilter::OnOverlayProcess(WPARAM wParam, LPARAM lParam)
{
	CSWAutoLock sLock(&m_cMutex);

	CSWImage * pImage = (CSWImage *)wParam;
	CSWCarLeft *pCarLeft = (CSWCarLeft *)lParam;
	int nLineNum = 1;

	DomeOverlay(pImage);

	if(m_fEnable)
	{	
		INT iSensorType = swpa_get_sensor_type();
		//电警卡口字符叠加叠加到图像外，会修改JPEG图像分辨率，通常情况下MJPEG流不允许叠加字符，会影响到车道线设置
        if((SENSOR_IMX249 == iSensorType || SENSOR_ICX816 == iSensorType) &&  NULL == pCarLeft)
		{
			return S_OK;
		}
		
		CSWString strOSDInfo = RuleString(pImage->GetRefTime(), pCarLeft, pImage);
			
		//其实扩大图片高度放这边会有问题，当字体大小有变的时候需要有一帧的错误叠加
        if(SENSOR_IMX249 == iSensorType
			&& SW_IMAGE_YUV420SP == pImage->GetType()
			&& !strOSDInfo.IsEmpty())
		{
			INT iOSDInfoHeight = 128;
			INT iImgWidthLast = pImage->GetWidth();
			//先计算字符叠加高度，根据叠加信息高度，确定图片扩大高度
			if (S_OK == CalcOSDInfoHeight((LPCSTR)strOSDInfo, pImage->GetWidth(), &iOSDInfoHeight, &nLineNum)
				&& iOSDInfoHeight > 0)
			{
				SW_COMPONENT_IMAGE img;
				pImage->GetImage(&img);
				INT iNewHeight = 0;
				INT iScaleOffset = (iOSDInfoHeight>128)?128:iOSDInfoHeight;
				//DWORD dwTickBegin = CSWDateTime::GetSystemTick();
				if (SWPAR_OK == swpa_ipnc_scale_up(pImage->GetImageBuffer(),
					(1080==m_iY)?0:1/*Y=0时叠加图片外上边，Y=1080时叠加图片外下边*/,iScaleOffset,&iNewHeight))
				{
					img.len[0] = iNewHeight*img.rgiStrideWidth[0];
					img.len[1] = (iNewHeight/2)*img.rgiStrideWidth[1];
					img.iHeight = iNewHeight;
					img.iSize = img.len[0] + img.len[1];
					pImage->SetImage(img);
				}
				/*SW_TRACE_NORMAL("IMAGE Scale Up %dX%d to %dX%d takes %d ms",
					iImgWidthLast,pImage->GetHeight(),pImage->GetWidth(),
					pImage->GetHeight(),CSWDateTime::GetSystemTick() - dwTickBegin);*/
			}
		}
		
		DoOverlay(pImage, (LPCSTR)strOSDInfo, nLineNum);
		
	}
	return S_OK;
}

HRESULT CSWJPEGOverlayFilter::OnOSDGetJPEGEnable(WPARAM wParam, LPARAM lParam)
{
	SW_TRACE_DEBUG("CSWJPEGOverlayFilter::OnOSDGetJPEGEnable(0x%08x, 0x%08x)", wParam, lParam);
	*(INT *)lParam = m_fEnable;
	return S_OK;
}

HRESULT CSWJPEGOverlayFilter::OnOSDGetJPEGPlateEnable(WPARAM wParam, LPARAM lParam)
{
	SW_TRACE_DEBUG("CSWJPEGOverlayFilter::OnOSDGetJPEGPlateEnable(0x%08x, 0x%08x)", wParam, lParam);
	return E_NOTIMPL;
}

HRESULT CSWJPEGOverlayFilter::OnOSDGetJPEGTimeEnable(WPARAM wParam, LPARAM lParam)
{
	SW_TRACE_DEBUG("CSWJPEGOverlayFilter::OnOSDGetJPEGTimeEnable(0x%08x, 0x%08x)", wParam, lParam);
	*(INT *)lParam = m_fEnableTime;
	return S_OK;
}

HRESULT CSWJPEGOverlayFilter::OnOSDGetJPEGText(WPARAM wParam, LPARAM lParam)
{
	SW_TRACE_DEBUG("CSWJPEGOverlayFilter::OnOSDGetJPEGText(0x%08x, 0x%08x)", wParam, lParam);
	CHAR* szText = (CHAR *)lParam;

	// todo.
	// 需要转换换行符
	CSWString str = m_strOverlay;
	if( str.Find("\r") == -1 )
	{
		str.Replace("\n", "\r\n");
	}
	swpa_strcpy(szText, (LPCSTR)str);
	return S_OK;
}

HRESULT CSWJPEGOverlayFilter::OnOSDGetJPEGFontSize(WPARAM wParam, LPARAM lParam)
{
	SW_TRACE_DEBUG("CSWJPEGOverlayFilter::OnOSDGetJPEGFontSize(0x%08x, 0x%08x)", wParam, lParam);
	*(INT *)lParam = m_iFontSize;
	return S_OK;
}

HRESULT CSWJPEGOverlayFilter::OnOSDGetJPEGFontRBG(WPARAM wParam, LPARAM lParam)
{
	SW_TRACE_DEBUG("CSWJPEGOverlayFilter::OnOSDGetJPEGFontRBG(0x%08x, 0x%08x)", wParam, lParam);
	BYTE r, g, b;
	CSWUtils::YUV2RGB(m_iYColor, m_iUColor, m_iVColor, &r, &g, &b);
    SW_TRACE_DEBUG("CSWJPEGOverlayFilter::OnOSDGetJPEGFontRBG %d %d %d,%d %d %d",
        r, g ,b,m_iYColor,m_iUColor,m_iVColor);

    INT *iTmp = (INT *)lParam;
    iTmp[0] = m_iRColor;//r;
    iTmp[1] = m_iGColor;//g;
    iTmp[2] = m_iBColor;//b;
    SW_TRACE_DEBUG("CSWJPEGOverlayFilter::OnOSDGetJPEGFontRBG m_iRColor: %d, m_iGColor: %d, m_iBColor: %d\n", m_iRColor, m_iGColor, m_iBColor);
	return S_OK;
}

HRESULT CSWJPEGOverlayFilter::OnOSDGetJPEGPox(WPARAM wParam, LPARAM lParam)
{
	SW_TRACE_DEBUG("CSWJPEGOverlayFilter::OnOSDGetJPEGPox(0x%08x, 0x%08x)", wParam, lParam);
	INT *iPos = (INT *)lParam;
	iPos[0] = m_iX;
	iPos[1] = m_iY;
	return S_OK;
}

HRESULT CSWJPEGOverlayFilter::OnOSDSetJPEGEnable(WPARAM wParam, LPARAM lParam)
{
	SW_TRACE_DEBUG("CSWJPEGOverlayFilter::OnOSDGetJPEGPox(0x%08x, 0x%08x)", wParam, lParam);
	m_fEnable = (INT)wParam;
	return S_OK;
}

HRESULT CSWJPEGOverlayFilter::OnOSDSetJPEGPlateEnable(WPARAM wParam, LPARAM lParam)
{
	SW_TRACE_DEBUG("CSWJPEGOverlayFilter::OnOSDSetJPEGPlateEnable(0x%08x, 0x%08x)", wParam, lParam);
	return E_NOTIMPL;
}

HRESULT CSWJPEGOverlayFilter::OnOSDSetJPEGTimeEnable(WPARAM wParam, LPARAM lParam)
{
	SW_TRACE_DEBUG("CSWJPEGOverlayFilter::OnOSDSetJPEGTimeEnable(0x%08x, 0x%08x)", wParam, lParam);
	m_fEnableTime = (INT)wParam;
	return S_OK;
}

HRESULT CSWJPEGOverlayFilter::OnOSDSetJPEGText(WPARAM wParam, LPARAM lParam)
{
    SW_TRACE_DEBUG("CSWJPEGOverlayFilter::OnOSDSetJPEGText(0x%08x, 0x%08x)", wParam, lParam);
	if (0 == swpa_strcmp((LPCSTR)wParam,"NULL"))
	{
		m_strOverlay.Clear();
	}
	else
	{
		m_strOverlay = (LPCSTR)wParam;
		// 替换特殊的换行符
		m_strOverlay.Replace("{0D0A}", "\n");
		m_strOverlay.Replace("\r\n", "\n");
	}
	
	return S_OK;
}

HRESULT CSWJPEGOverlayFilter::OnOSDSetJPEGFontSize(WPARAM wParam, LPARAM lParam)
{
	SW_TRACE_DEBUG("CSWJPEGOverlayFilter::OnOSDSetJPEGFontSize(0x%08x, 0x%08x)", wParam, lParam);
	m_iFontSize = (INT)wParam;
	return S_OK;
}

HRESULT CSWJPEGOverlayFilter::OnOSDSetJPEGFontRBG(WPARAM wParam, LPARAM lParam)
{
	SW_TRACE_DEBUG("CSWJPEGOverlayFilter::OnOSDSetJPEGFontRBG(0x%08x, 0x%08x)", wParam, lParam);
    INT *iTmp = (INT *)wParam;
    BYTE r = iTmp[0];//SW_R(wParam);
    BYTE g = iTmp[1];//SW_G(wParam);
    BYTE b = iTmp[2];//SW_B(wParam);

    m_iRColor = iTmp[0];
    m_iGColor = iTmp[1];
    m_iBColor = iTmp[2];
    
	CSWUtils::RGB2YUV(r, g, b, (BYTE *)&m_iYColor, (BYTE *)&m_iUColor, (BYTE *)&m_iVColor);
	SW_TRACE_DEBUG("CSWJPEGOverlayFilter::OnOSDSetJPEGFontRBG %d %d %d,%d %d %d", 
		r, g ,b,m_iYColor,m_iUColor,m_iVColor);

    SW_TRACE_DEBUG("CSWJPEGOverlayFilter::OnOSDSetJPEGFontRBG m_iRColor: %d, m_iGColor: %d, m_iBColor: %d\n", m_iRColor, m_iGColor, m_iBColor);
	
	return S_OK;
}

HRESULT CSWJPEGOverlayFilter::OnOSDSetJPEGPox(WPARAM wParam, LPARAM lParam)
{
	SW_TRACE_DEBUG("CSWJPEGOverlayFilter::OnOSDSetJPEGPox(0x%08x, 0x%08x)", wParam, lParam);
	INT* iPos = (INT *)wParam;
	m_iX = iPos[0];
	m_iY = iPos[1];
	SendMessage(MSG_RESULT_FILTER_OVERLAY_POS_CHANGED, wParam, lParam);
	return S_OK;
}
HRESULT CSWJPEGOverlayFilter::OnOSDGetDebugJPEGStatus(WPARAM wParam, LPARAM lParam)
{
	SW_TRACE_DEBUG("CSWJPEGOverlayFilter::OnOSDGetDebugJPEGStatus(0x%08x, 0x%08x)", wParam, lParam);
	*(INT *)lParam = m_fDebug;
	return S_OK;
}

HRESULT CSWJPEGOverlayFilter::OnOSDSetDebugJPEGStatus(WPARAM wParam, LPARAM lParam)
{
	SW_TRACE_DEBUG("CSWJPEGOverlayFilter::OnOSDSetDebugJPEGStatus(0x%08x, 0x%08x)", wParam, lParam);
	m_fDebug = (INT)wParam;
	return SendMessage(MSG_RECOGNIZE_OUTPUTDEBUG, wParam, lParam);
}

HRESULT CSWJPEGOverlayFilter::OnOSDSetMJPEGOverlay(WPARAM wParam, LPARAM lParam)
{
	BOOL fMJPEGOverlayEnable = (BOOL)wParam;
	SW_TRACE_DEBUG("Set MJPEG Overlay %d...",fMJPEGOverlayEnable);
	m_fMJPEGDoStrOverlay = fMJPEGOverlayEnable;
	return S_OK;
}


