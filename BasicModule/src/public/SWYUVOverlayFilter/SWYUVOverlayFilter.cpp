#include "SWFC.h"
#include "SWMessage.h"
#include "SWYUVOverlayFilter.h"

CSWYUVOverlayFilter::CSWYUVOverlayFilter():CSWBaseFilter(1,1)
{
	m_pTTFBitmap = NULL;
	m_pMemory = NULL;
	m_iOffsetMemory = 0;
	m_fEnable = TRUE;
	m_fEnableTime = TRUE;
	m_iFontSize = 32;
	m_iNowFontSize = 0;
	m_dwChangeFontSizeLast = CSWDateTime::GetSystemTick();
	m_iX = 0;
	m_iY = 0;
	m_iYColor = 255;
	m_iUColor = 128;
	m_iVColor = 128;	
    m_iRColor = 255;
    m_iGColor = 255;
    m_iBColor = 255;
	m_fRuleString = FALSE;
	m_pMemory = CSWMemoryFactory::GetInstance(SW_SHARED_MEMORY)->Alloc(2*1024*1024);

	m_pTTFBitmapPTZ = NULL;
	m_fOverlayPTZ = FALSE;
	m_fOverlayDomeInfo = FALSE;
	m_iFontSizePTZ = 56;
	m_pMemoryPTZ = CSWMemoryFactory::GetInstance(SW_SHARED_MEMORY)->Alloc(3*1024*1024);
	m_iOffsetMemoryPTZ = 0;
	swpa_memset(m_szDomeInfo, 0, sizeof(m_szDomeInfo));

	m_iDomeYColor = 0, m_iDomeUColor = 0, m_iDomeVColor = 0;
	CSWUtils::RGB2YUV((BYTE)255, (BYTE)255, (BYTE)255, (BYTE*)&m_iDomeYColor, (BYTE*)&m_iDomeUColor, (BYTE*)&m_iDomeVColor);
	
	if(NULL != m_pTTFBitmapPTZ)
	{
		//析构函数中会自动释放资源，调用Destroy会出现kill掉的情况
		//m_pTTFBitmapPTZ->Destroy();
		delete m_pTTFBitmapPTZ;
		m_pTTFBitmapPTZ = NULL;
	}
	m_pTTFBitmapPTZ = new CTTF2Bitmap;
	if (NULL == m_pTTFBitmapPTZ)
	{
		SW_TRACE_NORMAL("Err: no memory for m_pTTFBitmapPTZ\n");
		return ;
	}
	m_pTTFBitmapPTZ->Init("fz_songti.ttf", m_iFontSizePTZ, 0);
	swpa_memset(m_pMemoryPTZ->GetBuffer(), 0, m_pMemoryPTZ->GetSize());
	m_iOffsetMemoryPTZ = 0;
    m_fPTZUpdated = FALSE;
    m_dwPanCoord = 0;
    m_dwTiltCoord = 0;
    m_dwZoomValue = 0;
}

CSWYUVOverlayFilter::~CSWYUVOverlayFilter()
{
	if(NULL != m_pMemory)
	{
		CSWMemoryFactory::GetInstance(SW_SHARED_MEMORY)->Free(m_pMemory);
	}
	if(NULL != m_pTTFBitmap)
	{
		//析构函数中会自动释放资源，调用Destroy会出现退出kill掉的情况
		//m_pTTFBitmap->Destroy();
		delete m_pTTFBitmap;
	}

	if(NULL != m_pMemoryPTZ)
	{
		CSWMemoryFactory::GetInstance(SW_SHARED_MEMORY)->Free(m_pMemoryPTZ);
	}
	if (NULL != m_pTTFBitmapPTZ)
	{
		delete m_pTTFBitmapPTZ;
	}
	
}

HRESULT CSWYUVOverlayFilter::Initialize(BOOL fEnable, 
	BOOL fEnableTime, LPCSTR szOverlay, INT iFontSize, 
    INT iX, INT iY, INT iYColor, INT iUColor, INT iVColor, LPCSTR szDeviceID, INT iSpeedLimit, DWORD* dwArrayRGB)
{
#ifdef YUV_TEST
	m_fEnable     = TRUE;
	m_fEnableTime = TRUE;
#else	
	m_fEnable     = fEnable;   
	m_fEnableTime = fEnableTime;
#endif		
	m_iFontSize   = iFontSize;
	m_iX          = iX;  
	m_iY          = iY;
	m_iYColor     = iYColor;
	m_iUColor     = iUColor;
	m_iVColor     = iVColor;
	m_strOverlay  = szOverlay;
	m_strDeviceID = szDeviceID;
	// 替换特殊的换行符
	m_strOverlay.Replace("{0D0A}", "\n");
	m_strOverlay.Replace("\r\n", "\n");

    m_iSpeedLimit = iSpeedLimit;

    m_iRColor = dwArrayRGB[0];
    m_iGColor = dwArrayRGB[1];
    m_iBColor = dwArrayRGB[2];

    SW_TRACE_NORMAL("[%s]overlay enable:%d,enable time:%d,x:%d,y:%d,[size:%d,y:%d,u:%d,v:%d]DevID:%s, [R:%d, G:%d, B:%d]"
		, Name()
		, m_fEnable
		, m_fEnableTime
		, m_iX
		, m_iY
		, m_iFontSize
		, m_iYColor
		, m_iUColor
		, m_iVColor
		, (LPCSTR)m_strDeviceID
        , m_iRColor
        , m_iGColor
        , m_iBColor
	);

	m_pTTFBitmap = new CTTF2Bitmap;
	m_pTTFBitmap->Init("fz_songti.ttf", m_iFontSize, 0);
	swpa_memset(m_pMemory->GetBuffer(), 0, m_pMemory->GetSize());
	m_iOffsetMemory = 0;

	return S_OK;
}

HRESULT CSWYUVOverlayFilter::Receive(CSWObject* obj)
{
	if(IsDecendant(CSWImage, obj))
	{
		CSWImage* pImage = (CSWImage *)obj;

        if (0 != swpa_strcmp(GetName(), "CSWH264SecondOverlayFilter")) //第二路码流不叠加球机相关字符
        {
            DomeOverlay(pImage);
        }
        
		DoOverlay(pImage, RuleString(pImage->GetRefTime()));
		GetOut(0)->Deliver(pImage);
	}
	return S_OK;
}


HRESULT CSWYUVOverlayFilter::Run()
{
    if (FAILED(CSWBaseFilter::Run()))
    {
        SW_TRACE_NORMAL("Err: failed to start.\n");
        return E_FAIL;
    }

    if (FAILED(m_cPTZUpdateThread.Start(UpdatePTZProxy, this)))
    {
        SW_TRACE_NORMAL("Err: failed to start PTZUpdate thread.\n");
        return E_FAIL;
    }

    return S_OK;
}




HRESULT CSWYUVOverlayFilter::UpdatePTZ()
{
    m_fPTZUpdated = FALSE;
    while (FILTER_RUNNING == GetState())
    {
        CSWApplication::Sleep(50);
        
        if (m_fOverlayPTZ)
        {
            INT iPanTilt = -1, iZoom = -1;
            if (SUCCEEDED(CSWMessage::SendMessage(MSG_GET_PT_COORDINATE, 0, LPARAM(&iPanTilt)))
                && iPanTilt > 0
                )
            {
                m_dwPanCoord = (iPanTilt >> 16) / 10;
                m_dwTiltCoord = (iPanTilt & 0xFFFF) / 10;
            }

            if (SUCCEEDED(CSWMessage::SendMessage(MSG_GET_ZOOM, 0, LPARAM(&iZoom)))
                && iZoom > 0
                )
            {
                m_dwZoomValue = iZoom / 100;
            }

            m_fPTZUpdated = TRUE;
            CSWApplication::Sleep(100);
        }
        else
        {
            m_fPTZUpdated = FALSE;
        }
    }

    return S_OK;
}


PVOID CSWYUVOverlayFilter::UpdatePTZProxy(PVOID pvArg)
{
    CSWYUVOverlayFilter* pThis = (CSWYUVOverlayFilter*)pvArg;
    
    if (NULL != pThis)
    {   
        pThis->UpdatePTZ();
    }
}

HRESULT CSWYUVOverlayFilter::DoOverlayPTZ(CSWImage *pImage, LPCSTR szText, INT iXCoord, INT iYCoord)
{
	INT xPosition = iXCoord;
	INT yPosition = iYCoord;
	
	BYTE* pbOverlay = NULL;

	SW_COMPONENT_IMAGE img;
	pImage->GetImage(&img);
	typedef struct tagPOS
	{
		DWORD yPos;
		DWORD h;
	}POS;

	INT iHeight = 1;
	for(LPCSTR ch = szText; *ch != '\0'; ch++)
	{
		if(*ch == '\n')
		{
			iHeight++;
		}
	}
	iHeight *= (m_iFontSizePTZ + 10);
	if(iHeight + yPosition > img.iHeight)
	{
		iHeight = img.iHeight - yPosition;
	}
	if(iHeight > 0)
	{
		//pImage->AllocOverlayBuffer(sizeof(POS) + iHeight*img.rgiStrideWidth[0] + (iHeight/2)*img.rgiStrideWidth[1]);
		//BYTE* pbOverlay = pImage->GetOverlayBuffer();
		INT iSize = sizeof(POS) + iHeight*img.rgiStrideWidth[0] + (iHeight/2)*img.rgiStrideWidth[1];
		pbOverlay = new BYTE[iSize];
		POS *pos = (POS *)pbOverlay; //pbOverlay += sizeof(POS);
		pos->yPos = yPosition;
		pos->h = iHeight;							
		BYTE* yDst = pbOverlay + sizeof(POS);
		BYTE* uvDst = pbOverlay + sizeof(POS) + img.rgiStrideWidth[0]*pos->h;			
		BYTE* ySrc = img.rgpbData[0] + img.rgiStrideWidth[0]*pos->yPos;
		BYTE* uvSrc = img.rgpbData[1] + img.rgiStrideWidth[1]*(pos->yPos/2);			
		swpa_memcpy(yDst, ySrc, img.rgiStrideWidth[0]*pos->h);
		swpa_memcpy(uvDst, uvSrc, img.rgiStrideWidth[1]*(pos->h/2));
	}		

	char szOverlay[3];
	INT x = xPosition;
	INT y = yPosition;		
	INT offsetH = 0;
	for(LPCSTR ch = szText; *ch != '\0'; ch++)
	{
		if(*ch == '\n')
		{
			offsetH++;
			x = xPosition;
			continue;
		}
			
		if(!(*ch & 0x80))
		{
			szOverlay[0] = *ch;
			szOverlay[1] = '\0';
		}
		else
		{
			szOverlay[0] = *ch;
			szOverlay[1] = *++ch;
			szOverlay[2] = '\0';
		}
		TEXT *txt = GetPTZTextBuffer(szOverlay);
		if(txt)
		{
			PBYTE pbBuf = (PBYTE)txt + sizeof(TEXT);
			if(x < img.iWidth && y + offsetH*txt->iHeight < img.iHeight)
			{
				INT width = txt->iWidth;
				if(width > img.iWidth - x)
				{
					width = img.iWidth - x;
				}
				for(int h = 0; h < txt->iHeight && h + y + offsetH*txt->iHeight < img.iHeight; h++)
				{
					PBYTE dstY  = img.rgpbData[0] + img.rgiStrideWidth[0]*(h + y + offsetH*txt->iHeight) + x;
					//U\V分量交织、U\V顺序保持不变
					PBYTE dstUV = img.rgpbData[1] + img.rgiStrideWidth[1]*((h + y + offsetH*txt->iHeight)/2) + (x & ~1);	
					for(int w = 0; w < width; w++)
					{	
						if(pbBuf[h*txt->iWidth + w])
						{
							//y
							*dstY++ = m_iDomeYColor;
							//uv
							if(!(w%2))
							{
								*dstUV++ = m_iDomeUColor;
								*dstUV++ = m_iDomeVColor;
							}
						}
						else
						{
							dstY++;
							if(!(w%2))
							{
								dstUV += 2;
							}
						}
					}
				}
			}
			x += ((txt->iWidth + 1) & ~1);
		}
		else
		{
			SW_TRACE_DEBUG("%s::GetTextBuffer(%s) error", Name(), szOverlay);
		}
	}
	
	// 图片区域缓存回写
	// 178 暂不做此操作稳定性不确认是否因此引起。
	if( swpa_get_sensor_type() != SENSOR_IMX178 )
	{
		swpa_ipnc_cache_wb (img.rgpbData[0], img.len[0], 0xffff, 1);
		swpa_ipnc_cache_wb (img.rgpbData[1], img.len[1], 0xffff, 1);
	}
	//pImage->SetOverlayFlag(TRUE);

	if (NULL != pbOverlay)
	{
		delete []pbOverlay;
	}

	return S_OK;
}

HRESULT CSWYUVOverlayFilter::DomeOverlay(CSWImage *pImage)
{
	if (NULL == pImage)
	{
		SW_TRACE_DEBUG("Err: invalid arg: pImage\n");
		return E_INVALIDARG;
	}

	static INT dwPTZOverlayCount = 0;

	if ((m_fOverlayPTZ && m_fPTZUpdated ) || dwPTZOverlayCount > 0)
	{
		if (m_fOverlayPTZ)
		{
			dwPTZOverlayCount = 1;
		}
		
		if (dwPTZOverlayCount++ > 100)
		{
			dwPTZOverlayCount = 0;
		}
		else
		{
			CHAR szInfo[256] = {0};
            swpa_sprintf(szInfo, "P%03d | T%02d | Z%02d", m_dwPanCoord, m_dwTiltCoord, m_dwZoomValue);
			DoOverlayPTZ(pImage, szInfo, 100, 800);
		}
	}

	
	static INT dwDomeInfoOverlayCount = 0;
	if (m_fOverlayDomeInfo)
	{
		dwDomeInfoOverlayCount = 1;
		m_fOverlayDomeInfo = FALSE;
	}
	
	if (0 < dwDomeInfoOverlayCount)
	{		
		if (dwDomeInfoOverlayCount++ > 150)
		{
			dwDomeInfoOverlayCount = 0;
		}
		else
		{
			DoOverlayPTZ(pImage, m_szDomeInfo, 100, 900);
		}
	}

	return S_OK;
}

HRESULT CSWYUVOverlayFilter::CalcOSDInfoHeight(LPCSTR szText,
	INT iImgWidth,INT *piOSDHeight,
	INT *pnLineNum, INT nLimitLine)
{
	if (NULL == piOSDHeight)
	{
		return E_INVALIDARG;
	}

	//计算所有叠加字符高度
	INT iOSDInfoHeight = 0;
	INT xPosition = m_iX;
	INT iOSDLineNum = 1;	//至少一行
	char szOverlay[3];
	INT x = xPosition;
	INT iCharHeight = 0;
	for(LPCSTR ch = szText; *ch != '\0'; ch++)
	{
		if(*ch == '\n')
		{
			iOSDLineNum++;
			x = xPosition;
			continue;
		}
		
		if(!(*ch & 0x80))
		{
			szOverlay[0] = *ch;
			szOverlay[1] = '\0';
		}
		else
		{
			szOverlay[0] = *ch;
			szOverlay[1] = *++ch;
			szOverlay[2] = '\0';
		}
		
		TEXT *txt = GetTextBuffer(szOverlay);
		if(txt)
		{
			if ((x+((txt->iWidth + 1) & ~1)) > iImgWidth)	//叠加超过图像宽度，则自动换行
			{
			    if (iOSDLineNum >= nLimitLine)
			    {
			        break;
			    }
				iOSDLineNum++;
				x = xPosition;
			}
			x += ((txt->iWidth + 1) & ~1);
			iCharHeight = txt->iHeight;
		}
	}
	iOSDInfoHeight = iOSDLineNum*iCharHeight;
	if (piOSDHeight != NULL)
	{
		*piOSDHeight = iOSDInfoHeight;
	}
	if (pnLineNum != NULL)
	{
	    (*pnLineNum) = iOSDLineNum;
	}
	
	return S_OK;
}


HRESULT CSWYUVOverlayFilter::DoOverlay(CSWImage *pImage, LPCSTR szText, INT nLineNum)
{
	INT xPosition = m_iX;
	INT yPosition = m_iY;

	
	DWORD dwNow = CSWDateTime::GetSystemTick();
	if(m_fEnable && !pImage->IsOverlayed())
	{

		SW_COMPONENT_IMAGE img;
		pImage->GetImage(&img);		
			
		if(m_iNowFontSize != m_iFontSize)
		{
			if ((CSWDateTime::GetSystemTick() - m_dwChangeFontSizeLast > 2000) || NULL == m_pTTFBitmap)
			{
				SW_TRACE_NORMAL("Change Font Size %d => %d, m_iOffsetMemory %d\n",
					m_iNowFontSize,m_iFontSize,m_iOffsetMemory);
				
				m_iNowFontSize = m_iFontSize;
				if(NULL != m_pTTFBitmap)
				{
					//析构函数中会自动释放资源，调用Destroy会出现kill掉的情况
					//m_pTTFBitmap->Destroy();
					delete m_pTTFBitmap;
					m_pTTFBitmap = NULL;
				}
				m_pTTFBitmap = new CTTF2Bitmap;
				m_pTTFBitmap->Init("fz_songti.ttf", m_iNowFontSize, 0);
				swpa_memset(m_pMemory->GetBuffer(), 0, m_pMemory->GetSize());
				m_iOffsetMemory = 0;
			
				m_dwChangeFontSizeLast = CSWDateTime::GetSystemTick();				
			}
		}

		if(m_fRuleString)
		{
			typedef struct tagPOS
			{
				DWORD yPos;
				DWORD h;
			}POS;

			if(!pImage->GetOverlayBuffer())
			{
				INT iHeight = 1;
				if (nLineNum == -1)
				{
                    for(LPCSTR ch = szText; *ch != '\0'; ch++)
                    {
                        if(*ch == '\n')
                        {
                            iHeight++;
                        }
                    }
				}
				else
				{
				    iHeight = nLineNum;
				}
				iHeight *= (m_iNowFontSize + 10);
				if(iHeight + yPosition > img.iHeight)
				{
					iHeight = img.iHeight - yPosition;
				}
				if(iHeight > 0)
				{
					pImage->AllocOverlayBuffer(sizeof(POS) + iHeight*img.rgiStrideWidth[0] + (iHeight/2)*img.rgiStrideWidth[1]);
					BYTE* pbOverlay = pImage->GetOverlayBuffer();
					POS *pos = (POS *)pbOverlay; pbOverlay += sizeof(POS);
					pos->yPos = yPosition;
					pos->h = iHeight;							
					BYTE* yDst = pbOverlay;
					BYTE* uvDst = pbOverlay + img.rgiStrideWidth[0]*pos->h;			
					BYTE* ySrc = img.rgpbData[0] + img.rgiStrideWidth[0]*pos->yPos;
					BYTE* uvSrc = img.rgpbData[1] + img.rgiStrideWidth[1]*(pos->yPos/2);			
					swpa_memcpy(yDst, ySrc, img.rgiStrideWidth[0]*pos->h);
					swpa_memcpy(uvDst, uvSrc, img.rgiStrideWidth[1]*(pos->h/2));
				}
			}			
			else
			{
				BYTE* pbOverlay = pImage->GetOverlayBuffer();
				POS *pos = (POS *)pbOverlay; pbOverlay += sizeof(POS);
				BYTE* ySrc = pbOverlay;
				BYTE* uvSrc = pbOverlay + img.rgiStrideWidth[0]*pos->h;			
				BYTE* yDst = img.rgpbData[0] + img.rgiStrideWidth[0]*pos->yPos;
				BYTE* uvDst = img.rgpbData[1] + img.rgiStrideWidth[1]*(pos->yPos/2);			
				swpa_memcpy(yDst, ySrc, img.rgiStrideWidth[0]*pos->h);
				swpa_memcpy(uvDst, uvSrc, img.rgiStrideWidth[1]*(pos->h/2));			
			}
		}

		char szOverlay[3];
		INT x = xPosition;
		INT y = yPosition;		
		INT offsetH = 0;
		for(LPCSTR ch = szText; *ch != '\0'; ch++)
		{
			if(*ch == '\n')
			{
				offsetH++;
				x = xPosition;
				continue;
			}
				
			if(!(*ch & 0x80))
			{
				szOverlay[0] = *ch;
				szOverlay[1] = '\0';
			}
			else
			{
				szOverlay[0] = *ch;
				szOverlay[1] = *++ch;
				szOverlay[2] = '\0';
			}
			TEXT *txt = GetTextBuffer(szOverlay);
			if(txt)
			{
				if ((x+((txt->iWidth + 1) & ~1)) > img.iWidth)	//叠加超过图像宽度，则自动换行
				{
					offsetH++;
					if (nLineNum > 0 && offsetH >= nLineNum)
					{
					    break;
					}
					x = xPosition;
				}
				
				PBYTE pbBuf = (PBYTE)txt + sizeof(TEXT);
				if(x < img.iWidth && y + offsetH*txt->iHeight < img.iHeight)
				{
					INT width = txt->iWidth;
					if(width > img.iWidth - x)
					{
						width = img.iWidth - x;
					}
					for(int h = 0; h < txt->iHeight && h + y + offsetH*txt->iHeight < img.iHeight; h++)
					{
						PBYTE dstY  = img.rgpbData[0] + img.rgiStrideWidth[0]*(h + y + offsetH*txt->iHeight) + x;
						//U\V分量交织、U\V顺序保持不变
						PBYTE dstUV = img.rgpbData[1] + img.rgiStrideWidth[1]*((h + y + offsetH*txt->iHeight)/2) + (x & ~1);	
						for(int w = 0; w < width; w++)
						{	
							if(pbBuf[h*txt->iWidth + w])
							{
								//y
								*dstY++ = m_iYColor;
								//uv
								if(!(w%2))
								{
									*dstUV++ = m_iUColor;
									*dstUV++ = m_iVColor;
								}
							}
							else
							{
								dstY++;
								if(!(w%2))
								{
									dstUV += 2;
								}
							}
						}
					}
				}
				x += ((txt->iWidth + 1) & ~1);
			}
			else
			{
				SW_TRACE_DEBUG("%s::GetTextBuffer(%s) error", Name(), szOverlay);
			}
		}
		
		// 图片区域缓存回写
		// 178 暂不做此操作稳定性不确认是否因此引起。
		if( swpa_get_sensor_type() != SENSOR_IMX178 )
		{
			swpa_ipnc_cache_wb (img.rgpbData[0], img.len[0], 0xffff, 1);
			swpa_ipnc_cache_wb (img.rgpbData[1], img.len[1], 0xffff, 1);
		}
		//pImage->SetOverlayFlag(TRUE);
	}
	return S_OK;
}

CSWString CSWYUVOverlayFilter::RuleString(DWORD dwOverlayTime, CSWCarLeft *pCarLeft, CSWImage* pImage)
{
	CSWString strText, strTime;
	if(m_fEnableTime)
	{
		CSWDateTime dtNow(dwOverlayTime); 
		strTime.Format("%04d-%02d-%02d %02d:%02d:%02d %03d\n", dtNow.GetYear(), dtNow.GetMonth(), dtNow.GetDay(), dtNow.GetHour(), dtNow.GetMinute(), dtNow.GetSecond(), dtNow.GetMSSecond());
	}
#ifdef YUV_TEST	
	CSWCarLeft carLeft;
	pCarLeft = &carLeft;
	pCarLeft->SetPlateNo("蓝桂ABC123");
	m_strOverlay = "\n车牌号码:$(PlateName)\n车牌颜色:$(PlateColor)\n路口名称:$(RoadName)\n路口方向:$(RoadDir)\n错误叠加:$&&**()";
#endif	
	CSWString strKey;
	m_fRuleString = FALSE;

	// 如果没有结果信息，则无通配符的替换。
	if( pCarLeft == NULL )	//H264也支持通配符，叠加时间、路口名称、路口方向
	{
		//strText = m_strOverlay;
		for(LPCSTR ch = (LPCSTR)m_strOverlay; *ch != '\0'; ch++)
		{
			if(ch[0] == '$' && ch[1] == '(')
			{
				//m_fRuleString = TRUE;
				strKey = *ch;
			}
			else if(!strKey.IsEmpty())
			{
				strKey += *ch;
				if(*ch == ')')
				{
					if (strKey == (CSWString)"$(Time)")
					{
						CSWString strCarLeftTime;
						CSWDateTime dtNow(dwOverlayTime); 
                        strCarLeftTime.Format("%04d-%02d-%02d %02d:%02d:%02d %03d",
							dtNow.GetYear(), dtNow.GetMonth(), dtNow.GetDay(), 
                            dtNow.GetHour(), dtNow.GetMinute(), dtNow.GetSecond(),
                            dtNow.GetMSSecond());
						strText += strCarLeftTime;
					}
					else if(strKey == (CSWString)"$(RoadName)")
					{
						strText += CSWCarLeft::GetRoadName();
					}
					else if(strKey == (CSWString)"$(RoadDir)")
					{
						strText += CSWCarLeft::GetRoadDirection();
					}
					else if (strKey == (CSWString)"$(DevID)")
					{
						strText += m_strDeviceID;
					}

					strKey = "";
				}
			}
			else
			{
				strText += *ch;
			}
		}
	}
	else
	{
		for(LPCSTR ch = (LPCSTR)m_strOverlay; *ch != '\0'; ch++)
		{
			if(ch[0] == '$' && ch[1] == '(')
			{
				m_fRuleString = TRUE;
				strKey = *ch;
			}
			else if(!strKey.IsEmpty())
			{
				strKey += *ch;
				if(*ch == ')')
				{
					if(pCarLeft)
					{
						if (strKey == (CSWString)"$(Time)")
						{
							CSWString strCarLeftTime;
							CSWDateTime dtNow(dwOverlayTime); 
							strCarLeftTime.Format("%04d-%02d-%02d %02d:%02d:%02d %03d", 
								dtNow.GetYear(), dtNow.GetMonth(), dtNow.GetDay(), 
								dtNow.GetHour(), dtNow.GetMinute(), dtNow.GetSecond(), 
								dtNow.GetMSSecond());
							strText += strCarLeftTime;
						}
						else if(strKey == (CSWString)"$(PlateName)")
						{
							strText += pCarLeft->GetPlateNo();
						}
						else if(strKey == (CSWString)"$(PlateColor)")
						{
							CSWString strColor = pCarLeft->GetPlateNo().Substr(0, 2);
							strText += strColor;
						}
						else if(strKey == (CSWString)"$(RoadName)")
						{
							strText += pCarLeft->GetRoadName();
						}
						else if(strKey == (CSWString)"$(RoadDir)")
						{
							strText += pCarLeft->GetRoadDirection();
						}
						else if(strKey == (CSWString)"$(RoadNumber)")
						{
							CSWString strRoadNo;
							strRoadNo.Format("%d", pCarLeft->GetOutPutRoadNo());
							strText += strRoadNo;
						}
						else if(strKey == (CSWString)"$(CarType)")
						{
							CSWString strCarType;

							CHAR *szCarTypeMap[] = {"未知","小型车","中型车","大型车","行人","非机动车","机动车"};
							DWORD dwCarType = pCarLeft->GetCarType();
							if (dwCarType < CT_COUNT)
							{
								strCarType.Format("%s", szCarTypeMap[dwCarType]);
							}
							
							strText += strCarType;
						}
                        else if(strKey == (CSWString)"$(CarSpeed)")
						{
							CSWString strCarSpeed;
                            strCarSpeed.Format("%d", (int)pCarLeft->GetCarspeed());
							strText += strCarSpeed;
						}
						else if (strKey == (CSWString)"$(DevID)")
						{
							strText += m_strDeviceID;
						}
						else if (strKey == (CSWString)"$(EncryptCode)")
						{
							DWORD dwRand[4];
							PBYTE pRandInfo = (PBYTE)dwRand;
							swpa_utils_srand(CSWDateTime::GetSystemTick());
							for(int i = 0; i < 4; ++i)
							{
								dwRand[i] = swpa_utils_rand();
							}
							CSWString strCode;
							CHAR szTmp[32] = {0};
							for (int i = 0; i < 16; i++)
							{
								swpa_sprintf(szTmp, "%.2X", pRandInfo[i]);
								strCode.Append(szTmp);
							}
							
							strText += strCode;
							if (pImage)	//保存下来，生成JPEG图的时候加到末尾，然后再做防篡改
							{
								pImage->SetSecurityCode(pRandInfo,sizeof(dwRand));
							}
							
						}
                        else if(strKey == (CSWString)"$(LimitSpeed)")
						{
                            CSWString strLimitSpeed;
                            strLimitSpeed.Format("%d", m_iSpeedLimit);
                            strText += strLimitSpeed;
                        }
					}
					strKey = "";
				}
			}
			else
			{
				strText += *ch;
			}
		}
	}

	// 如果没有结果信息，则无通配符的替换。
	if( pCarLeft == NULL )
	{
		m_fRuleString = FALSE;
	}
	return strTime + strText;
}

CSWYUVOverlayFilter::TEXT* CSWYUVOverlayFilter::GetTextBuffer(LPCSTR szText)
{
	TEXT *pTxt = NULL;
	INT iOffset = 0;
	//找到已经生成到内存中的字
	while(NULL == pTxt && iOffset < m_iOffsetMemory)
	{
		TEXT* pTxtTmp = (TEXT *)((CHAR *)m_pMemory->GetBuffer() + iOffset);
		if(!swpa_strcmp(pTxtTmp->szText,szText))
		{
			pTxt = pTxtTmp;
			break;
		}
		iOffset += sizeof(TEXT) + pTxtTmp->iSize;
	}

	//这个字还没生成过，则生成到内存
	if(NULL == pTxt && m_pMemory->GetSize() > m_iOffsetMemory + m_iNowFontSize*2)
	{
		TEXT* pTxtTmp = (TEXT *)((CHAR *)m_pMemory->GetBuffer() + m_iOffsetMemory);
		swpa_memset(pTxtTmp, 0, sizeof(TEXT));
		PBYTE pbBitmap = NULL;
		m_pTTFBitmap->GenerateBitmap(szText, ENCODE_GB2312, &pbBitmap, pTxtTmp->iSize, pTxtTmp->iWidth, pTxtTmp->iHeight);
  		if(pTxtTmp->iSize > 0 && NULL != pbBitmap)
  		{
  			swpa_strcpy(pTxtTmp->szText, szText);
  			m_iOffsetMemory += sizeof(TEXT);
  		
  			swpa_memcpy((CHAR *)m_pMemory->GetBuffer() + m_iOffsetMemory, pbBitmap, pTxtTmp->iSize);
  			pTxtTmp->iSize = ALGIN_SIZE(pTxtTmp->iSize, 4);
  			m_iOffsetMemory += pTxtTmp->iSize;
  		
  			pTxt = pTxtTmp;
  		}
  		else
  		{
  			SW_TRACE_NORMAL("Warning: create %s error. use □ instead.\n", szText);
			pTxt = GetTextBuffer("□");
			if (NULL == pTxt)
			{
				SW_TRACE_NORMAL("Err: create □ error. skip this charactor.\n");
			}
  		}
	}
	return pTxt;
}



CSWYUVOverlayFilter::TEXT* CSWYUVOverlayFilter::GetPTZTextBuffer(LPCSTR szText)
{
	TEXT *pTxt = NULL;
	INT iOffset = 0;
	//找到已经生成到内存中的字
	while(NULL == pTxt && iOffset < m_iOffsetMemoryPTZ)
	{
		TEXT* pTxtTmp = (TEXT *)((CHAR *)m_pMemoryPTZ->GetBuffer() + iOffset);
		if(!swpa_strcmp(pTxtTmp->szText,szText))
		{
			pTxt = pTxtTmp;
			break;
		}
		iOffset += sizeof(TEXT) + pTxtTmp->iSize;
	}

	if (!pTxt)
	{
		//SW_TRACE_DEBUG("--tobedeleted: no BMP found in cache of %s\n", szText);
	}

	//这个字还没生成过，则生成到内存
	if(NULL == pTxt && m_pMemoryPTZ->GetSize() > m_iOffsetMemoryPTZ + m_iFontSizePTZ*m_iFontSizePTZ*2)
	{
		TEXT* pTxtTmp = (TEXT *)((CHAR *)m_pMemoryPTZ->GetBuffer() + m_iOffsetMemoryPTZ);
		swpa_memset(pTxtTmp, 0, sizeof(TEXT));
		PBYTE pbBitmap = NULL;
		m_pTTFBitmapPTZ->GenerateBitmap(szText, ENCODE_GB2312, &pbBitmap, pTxtTmp->iSize, pTxtTmp->iWidth, pTxtTmp->iHeight);
  		if(pTxtTmp->iSize > 0 && NULL != pbBitmap)
  		{
  			
  			swpa_strcpy(pTxtTmp->szText, szText);
  			m_iOffsetMemoryPTZ += sizeof(TEXT);

			//todo: test code, tobedeleted
			BOOL fBlankChar =  TRUE;
			for (INT i= 0; i<pTxtTmp->iSize; i++)
			{
				if (*(pbBitmap+i))
				{	
					fBlankChar = FALSE;
				}
			}

			if (fBlankChar)
			{
				SW_TRACE_DEBUG("tobedeleted: breaking: %s is a blank char!!! (%d, %d, %d)\n", szText, pTxtTmp->iSize, pTxtTmp->iWidth, pTxtTmp->iHeight);
			}
			//test code end.

			swpa_memset((CHAR *)m_pMemoryPTZ->GetBuffer() + m_iOffsetMemoryPTZ, 1, sizeof(pTxtTmp->iSize));
  			swpa_memcpy((CHAR *)m_pMemoryPTZ->GetBuffer() + m_iOffsetMemoryPTZ, pbBitmap, pTxtTmp->iSize);
  			pTxtTmp->iSize = ALGIN_SIZE(pTxtTmp->iSize, 4);
  			m_iOffsetMemoryPTZ += pTxtTmp->iSize;
  		
  			pTxt = pTxtTmp;
  		}
  		else
  		{
			SW_TRACE_NORMAL("Err: create %s error. skip this charactor.\n", szText);
  		}
	}
	return pTxt;
}



HRESULT CSWYUVOverlayFilter::OnOSDSetDomeInfoOverlay(WPARAM wParam, LPARAM lParam)
{
	//SW_TRACE_DEBUG("CSWYUVOverlayFilter::OnOSDEnableDomeInfoOverlay(0x%08x, 0x%08x)", wParam, lParam);
	m_fOverlayDomeInfo = (BOOL)lParam;
	if (0 != wParam)
	{
		swpa_memcpy(m_szDomeInfo, (PVOID)wParam, sizeof(m_szDomeInfo)-1);
	}
	
	return S_OK;
}


HRESULT CSWYUVOverlayFilter::OnOSDEnablePTZOverlay(WPARAM wParam, LPARAM lParam)
{
    //SW_TRACE_DEBUG("CSWYUVOverlayFilter::OnOSDEnablePTZOverlay(0x%08x, 0x%08x)", wParam, lParam);
    m_fOverlayPTZ = (BOOL)wParam;
    return S_OK;
}

HRESULT CSWYUVOverlayFilter::OnOSDSetDevID(WPARAM wParam, LPARAM lParam)
{
	SW_TRACE_DEBUG("OnOSDSetDevID(0x%08x, 0x%08x) DevID:%s",
		wParam,lParam,(LPCSTR)wParam);
	m_strDeviceID= (LPCSTR)wParam;
	return S_OK;
}

