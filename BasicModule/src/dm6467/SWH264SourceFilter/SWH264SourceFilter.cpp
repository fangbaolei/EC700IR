#include "SWFC.h"
#include "SWH264SourceFilter.h"
#include "tinyxml.h"

CSWH264SourceFilter::CSWH264SourceFilter()
	: CSWBaseFilter(0,1)
	, CSWMessage(MSG_DM368_CTRL_START, MSG_DM368_CTRL_END)
	, m_iBitrate(6 * 1024 * 1024)
	, m_iHeight(1080)
	, m_iWidth(1920)
	, m_iFps(25)
	, m_fInitialized(FALSE)
	, m_pMemoryFactory(NULL)
	, m_dwLastSyncTime(0)
	, m_fEnableOverlay(FALSE)
	, m_dwResolution(1080)
{
	swpa_memset(&m_cOverlayInfo, 0, sizeof(m_cOverlayInfo));
	m_cOverlayInfo.dwColor = 0x00FF0000;
	m_cH264Param.iForceFrameType = -1;
	m_cH264Param.intraFrameInterval = 4;
	m_cH264Param.iTargetBitrate = 600 * 1024 * 1024;

	GetOut(0)->AddObject(CLASSID(CSWImage));
}

CSWH264SourceFilter::~CSWH264SourceFilter()
{
	Clear();
}

HRESULT CSWH264SourceFilter::Initialize(BOOL fEnableOverlay, PVOID pvParam)
{
	if( m_fInitialized )
	{
		return S_OK;
	}

	if( pvParam == 0 )
	{
		return E_INVALIDARG;
	}
	
	m_fEnableOverlay = fEnableOverlay;
	H264_FILTER_PARAM* pParam = (H264_FILTER_PARAM*)pvParam;
	swpa_memcpy(&m_cOverlayInfo, &pParam->cOverlayInfo, sizeof(m_cOverlayInfo));
	swpa_memcpy(&m_cH264Param, &pParam->cH264Param, sizeof(m_cH264Param));

	//设置分辨率
	
	if (FAILED(SetResolution(pParam->cH264Param.iResolution)))
	{
		SW_TRACE_NORMAL("Err: failed to set H.264 resolution\n");
		return E_FAIL;
	}

	// 限制I帧间隔
	if( m_cH264Param.intraFrameInterval <= 0 )
	{
		m_cH264Param.intraFrameInterval = 1;
	}

	// 初始化，默认连接123.123.219.219 DM368的USB IP.
	strcpy( m_szCtrlIp, "123.123.219.219" );
	strcpy( m_szH264Ip, "123.123.219.219" );
	m_wCtrlPort = 2133;
	m_wH264Port = 2132;

	m_pMemoryFactory = CSWMemoryFactory::GetInstance(SW_NORMAL_MEMORY);
	if( NULL == m_pMemoryFactory )
	{
		Clear();
		return E_OUTOFMEMORY;
	}

	m_fInitialized = TRUE;
	return S_OK;
}


HRESULT CSWH264SourceFilter::SetResolution(INT iResolution)
{
	if (720 == iResolution || 1080 == iResolution)
	{
		m_dwResolution = iResolution;
		CSWFile cH264ResolutionFile;
		if (FAILED(cH264ResolutionFile.Open("BLOCK/.H264ResolutionInfo", "r")))
		{
			INT iRetryCount = 3;
			while (SWPAR_OK != swpa_device_set_resolution(720 == m_dwResolution ? 0 : 1))
			{
				if (0 > --iRetryCount)
				{
					break;
				}
				CSWApplication::Sleep(1000);
			}

			if (0 > iRetryCount)
			{
				SW_TRACE_NORMAL("Err: failed to set H.264 resolution(%dp) to DM368\n", m_dwResolution);
				return E_FAIL;
			}
			else
			{
				if (SUCCEEDED(cH264ResolutionFile.Open("BLOCK/.H264ResolutionInfo", "w")))
				{
					cH264ResolutionFile.Write(&m_dwResolution, sizeof(m_dwResolution), NULL);
				}
			}
		}
		else 
		{
			DWORD dwResolution = 0;
			INT iRetryCount = 3;
			while (FAILED(cH264ResolutionFile.Read(&dwResolution, sizeof(dwResolution), NULL)))
			{
				CSWApplication::Sleep(200);
				if (0 > --iRetryCount)
				{
					break;
				}
			}

			if (0 > iRetryCount)
			{
				SW_TRACE_NORMAL("Err: failed to read H.264 resolution file\n");
				return E_FAIL;
			}
			else
			{
				if (dwResolution == m_dwResolution)
				{
					//do nothing
				}
				else if (720 == dwResolution || 1080 == dwResolution)
				{
					return CSWMessage::SendMessage(MSG_APP_RESETDEVICE, (WPARAM)2, 0); //reset device
				}
				else
				{
					cH264ResolutionFile.Close();
					if (SUCCEEDED(cH264ResolutionFile.Open("BLOCK/.H264ResolutionInfo", "w")))
					{
						cH264ResolutionFile.Write(&m_dwResolution, sizeof(m_dwResolution), NULL);
					}
				}
			}
		}

		return S_OK;
	}
	else
	{
		SW_TRACE_NORMAL("Err: invalid H.264 resolution: %d\n", iResolution);
		return E_INVALIDARG;
	}
	
}


HRESULT CSWH264SourceFilter::SetFps(INT iFps)
{
	m_iFps = iFps;
	return S_OK;
}

HRESULT CSWH264SourceFilter::SetBitrate(INT iBitrate)
{
	m_iBitrate = iBitrate;
	return S_OK;
}

VOID CSWH264SourceFilter::Clear()
{
	m_cThreadH264.Stop();
	m_cSocketH264.Close();

	m_pMemoryFactory = NULL;
	m_fInitialized = FALSE;
}

HRESULT CSWH264SourceFilter::ConnectCtrl()
{
	if( !m_fInitialized )
	{
		return E_FAIL;
	}

	int iRevTimeMs = 4000;
	m_cSocketCtrl.Close();
	HRESULT hr = m_cSocketCtrl.Create();
	if( S_OK != hr )
	{
		SW_TRACE_DEBUG("<H264SourceFilter>Ctrl create socket failed!.\n");
		return hr;
	}

	hr  = m_cSocketCtrl.Connect(m_szCtrlIp, m_wCtrlPort);
	if( S_OK == hr )
	{
		SW_TRACE_DEBUG("<H264SourceFilter>Ctrl connet %s, %d.ok!\n", m_szCtrlIp, m_wCtrlPort);
		m_cSocketCtrl.SetRecvTimeout(iRevTimeMs);
	}
	else
	{
		SW_TRACE_DEBUG("<H264SourceFilter>Ctrl connet %s, %d.failed!\n", m_szCtrlIp, m_wCtrlPort);
	}
	return hr;
}
HRESULT CSWH264SourceFilter::DisconnectCtrl()
{
	return m_cSocketCtrl.Close();
}

// 数据解析
HRESULT CSWH264SourceFilter::GetFrameInfo(PBYTE pbInfo, const DWORD& dwInfoSize, PDWORD pdwTick, PDWORD pdwFrameType, PDWORD pdwWidth, PDWORD pdwHeight)
{
	if( pbInfo == NULL || dwInfoSize < 8 )
	{
		return E_INVALIDARG;
	}

	const DWORD NODE_XML = 0xCCDD0001;

	DWORD dwId = 0;
	DWORD dwNodeSize = 0;
	DWORD dwSize = dwInfoSize;
	DWORD dwOffset = 0;
	while(true)
	{
		if( dwSize < 8 )
		{
			break;
		}
		memcpy(&dwId, pbInfo + dwOffset, 4);
		dwOffset += 4;
		dwSize -= 4;
		memcpy(&dwNodeSize, pbInfo + dwOffset, 4);
		dwOffset += 4;
		dwSize -= 4;
		if( dwNodeSize > dwSize )
		{
			break;
		}
		if( dwId == NODE_XML )
		{
			CHAR* pszXML = (CHAR*)swpa_mem_alloc(dwNodeSize + 1);
			if( pszXML == NULL )
			{
				return E_OUTOFMEMORY;
			}
			memcpy(pszXML, pbInfo + dwOffset, dwNodeSize);
			pszXML[dwNodeSize] = 0;
			TiXmlDocument cXmlDoc;
			if( cXmlDoc.Parse(pszXML) )
			{
				const TiXmlElement* pRootElement = cXmlDoc.RootElement();
				if ( NULL != pRootElement )
				{
					const TiXmlElement* pSubElement = pRootElement->FirstChildElement("FrameType");
					if( pSubElement != NULL )
					{
						const char* pszFrameType = pSubElement->GetText();
						if( pszFrameType != NULL && pdwFrameType != NULL )
						{
							*pdwFrameType = strcmp(pszFrameType, "I") == 0 ? 0 : 1;
						}
					}
					pSubElement = pRootElement->FirstChildElement("Tick");
					if( pSubElement != NULL )
					{
						const char* pszTick = pSubElement->GetText();
						if( pszTick != NULL && pdwTick != NULL )
						{
							sscanf(pszTick, "%d", pdwTick);
						}
					}
					pSubElement = pRootElement->FirstChildElement("Width");
					if( pSubElement != NULL )
					{
						const char* pszWidth = pSubElement->GetText();
						if( pszWidth != NULL && pdwWidth != NULL )
						{
							sscanf(pszWidth, "%d", pdwWidth);
						}
					}
					pSubElement = pRootElement->FirstChildElement("Height");
					if( pSubElement != NULL )
					{
						const char* pszHeight = pSubElement->GetText();
						if( pszHeight != NULL && pdwHeight != NULL )
						{
							sscanf(pszHeight, "%d", pdwHeight);
						}
					}
				}
			}
			if(pszXML != NULL)
			{
				swpa_mem_free(pszXML);
				pszXML = NULL;
			}
		}

		dwOffset += dwNodeSize;
		dwSize -= dwNodeSize;
	}
}

HRESULT CSWH264SourceFilter::Run()
{
	if( !m_fInitialized )
	{
		return E_FAIL;
	}

	CSWBaseFilter::Run();

	// 启动线程。
	HRESULT	hr = m_cThreadH264.Start((START_ROUTINE)(&CSWH264SourceFilter::OnH264Proxy), (PVOID)this);
	if( hr != S_OK )
	{
		SW_TRACE_NORMAL("<H264SourceFilter>H264 thread start failed!.\n");
	}

	return hr;
}

HRESULT CSWH264SourceFilter::Stop()
{
	CSWBaseFilter::Stop();

	m_cThreadH264.Stop();

	return S_OK;
}

VOID CSWH264SourceFilter::OnH264Proxy(PVOID pvParam)
{
	if(pvParam != NULL)
	{
		CSWH264SourceFilter* pcCtrl = (CSWH264SourceFilter*)pvParam;
		pcCtrl->OnH264();
	}
}


HRESULT CSWH264SourceFilter::OnH264()
{
	BOOL fConnect = FALSE;
	INT iRevTimeMs = 4000;
	INT iInfoMaxSize = 1 * 1024 * 1024;
	INT iDataMaxSize = 4 * 1024 * 1024;

	int iWaitTimes = 13;
	while(S_OK != InitDM368())
	{
		if(iWaitTimes-- < 0)
		{
			SW_TRACE_DEBUG("<H264SourceFilter>Reset dm368.\n");
			ResetDM368();
			iWaitTimes = 13;
			swpa_thread_sleep_ms(10000);
		}
		swpa_thread_sleep_ms(10000);
	}

	// 数据ID定义
	enum
	{
		FRAME_H264 = 0xFFDD0001
	};

	// 数据包头
	struct tag_header
	{
		INT iID;
		INT iInfoSize;
		INT iDataSize;
	}cHeader;

	HRESULT hr = S_OK;
	DWORD dwFrameNo = 0;

	DWORD dwFps = 0;
	DWORD dwBeginTick = CSWDateTime::GetSystemTick();

	DWORD dwIPTotal = 0;
	DWORD dwIPSize = 0;
	DWORD dwIPCount = 0;
	DWORD dwIPFrameCount = 0;
	BOOL fIsReady = FALSE;
	BOOL fConnectFailed = FALSE;
	INT iConnectErrorCount = 0;
	IMAGE_EXT_INFO cExtInfo;

	swpa_memset(&cExtInfo, 0, sizeof(cExtInfo));
	if (FAILED(CSWMessage::SendMessage(MSG_GET_SHUTTER, 0, (LPARAM)&cExtInfo.iShutter)))
	{
		SW_TRACE_DEBUG("Err: failed to get SHUTTER\n");
	}
	if (FAILED(CSWMessage::SendMessage(MSG_GET_AGCGAIN, 0, (LPARAM)&cExtInfo.iGain)))
	{
		SW_TRACE_DEBUG("Err: failed to get AGCGAIN\n");
	}
	DWORD dwRGBGain = 0;
	if (FAILED(CSWMessage::SendMessage(MSG_GET_RGBGAIN, 0, (LPARAM)&dwRGBGain)))
	{
		SW_TRACE_DEBUG("Err: failed to get RGBGAIN\n");
	}
	else
	{
		cExtInfo.iRGain = (dwRGBGain >> 16) & 0xFF;
		cExtInfo.iGGain = (dwRGBGain >>  8) & 0xFF;
		cExtInfo.iBGain = (dwRGBGain >>  0) & 0xFF;
	}
	
	DWORD dwPrevTick = CSWDateTime::GetSystemTick();
	
	SW_TRACE_NORMAL("H264SourceFilter H264 Running.\n");
	while(GetState() == FILTER_RUNNING)
	{
		if( fConnectFailed )
		{
			/*
			ResetDM368();
			swpa_thread_sleep_ms(10000);
			iWaitTimes = 13;
			while(S_OK != InitDM368() && GetState() == FILTER_RUNNING )
			{
				if(iWaitTimes-- < 0)
				{
					SW_TRACE_DEBUG("<H264SourceFilter>Reset dm368.\n");
					ResetDM368();
					iWaitTimes = 13;
					swpa_thread_sleep_ms(10000);
				}
				swpa_thread_sleep_ms(10000);
			}
			fConnectFailed = FALSE;
			*/
			SW_TRACE_DEBUG("MSG_APP_RESETDEVICE 2\n");
			CSWMessage::SendMessage(MSG_APP_RESETDEVICE, (WPARAM)2, 0); //reset device
			break;
		}
		if( !fConnect )
		{
			if( S_OK != m_cSocketH264.Close() || S_OK != m_cSocketH264.Create() )
			{
				SW_TRACE_DEBUG("H264SourceFilter h264 socket create failed!\n");
				swpa_thread_sleep_ms(2000);
				continue;
			}

			m_cSocketH264.SetRecvTimeout(iRevTimeMs);
			m_cSocketH264.SetSendTimeout(iRevTimeMs);
			HRESULT hr  = m_cSocketH264.Connect(m_szH264Ip, m_wH264Port);
			if( S_OK == hr )
			{
				SW_TRACE_DEBUG("H264SourceFilter H264 Connet %s, %d.ok!\n", m_szH264Ip, m_wH264Port);
				iConnectErrorCount = 0;
				fConnect = TRUE;
			}
			else
			{
				SW_TRACE_DEBUG("H264SourceFilter H264 Connet %s, %d.failed!\n", m_szH264Ip, m_wH264Port);
				swpa_thread_sleep_ms(5000);
				++iConnectErrorCount;
				if( iConnectErrorCount > 5 )
				{
					fConnectFailed = TRUE;
				}
				continue;
			}
		}

		// 同步368的时间。
		if( CSWDateTime::GetSystemTick() - m_dwLastSyncTime > s_dwSyncTime )
		{
			if( S_OK == OnSyncTime(0,0) )
			{
				m_dwLastSyncTime = CSWDateTime::GetSystemTick();
			}
			else
			{
				m_dwLastSyncTime += 2000;
				SW_TRACE_DEBUG("<H264SourceFilter>synctime failed.\n");
			}
		}
		// 接收包头
		if( S_OK != m_cSocketH264.Read(&cHeader, sizeof(cHeader), NULL) )
		{
			SW_TRACE_DEBUG("H264SourceFilter H264 read data failed!\n");
			swpa_thread_sleep_ms(2000);
			fConnect = FALSE;
			continue;
		}
		// 对数据长度做限制。
		if( cHeader.iInfoSize > iInfoMaxSize || cHeader.iDataSize > iDataMaxSize )
		{
			SW_TRACE_DEBUG("H264SourceFilter h264 header failed! infosize:%d, datasize:%d.\n", cHeader.iInfoSize, cHeader.iDataSize);
			swpa_thread_sleep_ms(2000);
			fConnect = FALSE;
			continue;
		}

		switch(cHeader.iID)
		{
		case FRAME_H264:
			{
				DWORD dwTick = 0;
				DWORD dwFrameType = 0;
				DWORD dwWidth = 1920;
				DWORD dwHeight = 1080;
				if(720 == m_dwResolution)
				{
					dwWidth = 1280;
					dwHeight = 720;
				}

				// 接收数据
				PBYTE pbInfo = new BYTE[cHeader.iInfoSize];
				if( pbInfo == NULL || S_OK != m_cSocketH264.Read(pbInfo, cHeader.iInfoSize, NULL) )
				{
					SW_TRACE_DEBUG("H264SourceFilter h264 read info size:%d, addr:0x%08x failed!\n", cHeader.iInfoSize,(UINT)pbInfo);
					SAFE_DELETE(pbInfo);
					swpa_thread_sleep_ms(2000);
					fConnect = FALSE;
					continue;
				}
				// 解释数据
				GetFrameInfo(pbInfo, cHeader.iInfoSize, &dwTick, &dwFrameType, &dwWidth, &dwHeight);

				dwTick = CSWDateTime::GetSystemTick(); //use local tick
				
				if( 0 == dwFrameType )
				{
					if( dwIPCount > 0 )
					{
						dwIPTotal += dwIPSize;
					}
					dwIPSize = cHeader.iDataSize;
					dwIPCount++;
					fIsReady = (dwIPFrameCount == 0 ? FALSE : TRUE);
					dwIPFrameCount = fIsReady ? dwIPFrameCount : 1;
				}
				else
				{
					if( !fIsReady && dwIPFrameCount > 0)
					{
						dwIPFrameCount++;
					}
					dwIPSize += cHeader.iDataSize;
				}

				//  接收图片数据
				CSWImage* pImage = NULL;
				if( S_OK == CSWImage::CreateSWImage(&pImage, SW_IMAGE_H264, dwWidth, dwHeight, m_pMemoryFactory, dwFrameNo, dwTick, FALSE, dwFrameType == 0 ? "IFrame" : "PFrame", cHeader.iDataSize) )
				{
					dwFrameNo++;
					SW_COMPONENT_IMAGE cImage;
					pImage->GetImage(&cImage);
					if( pImage->GetImageBufferSize() < cHeader.iDataSize )
					{
						SW_TRACE_DEBUG("<H264SourceFilter>Recv data size %d > image buf size %d!\n", cHeader.iDataSize, pImage->GetImageBufferSize());
					}
					else
					{
						hr = m_cSocketH264.Read(cImage.rgpbData[0], cHeader.iDataSize, NULL);
						cImage.iSize = cHeader.iDataSize;
						pImage->SetImage(cImage);

						
						DWORD dwCurTick = CSWDateTime::GetSystemTick();
						pImage->SetRefTime(dwCurTick);
						if (2000 <= dwCurTick - dwPrevTick)
						{
							swpa_memset(&cExtInfo, 0, sizeof(cExtInfo));
							if (FAILED(CSWMessage::SendMessage(MSG_GET_SHUTTER, 0, (LPARAM)&cExtInfo.iShutter)))
							{
								SW_TRACE_DEBUG("Err: failed to get SHUTTER\n");
							}
							if (FAILED(CSWMessage::SendMessage(MSG_GET_AGCGAIN, 0, (LPARAM)&cExtInfo.iGain)))
							{
								SW_TRACE_DEBUG("Err: failed to get AGCGAIN\n");
							}
							DWORD dwRGBGain = 0;
							if (FAILED(CSWMessage::SendMessage(MSG_GET_RGBGAIN, 0, (LPARAM)&dwRGBGain)))
							{
								SW_TRACE_DEBUG("Err: failed to get RGBGAIN\n");
							}
							else
							{
								cExtInfo.iRGain = (dwRGBGain >> 16) & 0xFF;
								cExtInfo.iGGain = (dwRGBGain >>  8) & 0xFF;
								cExtInfo.iBGain = (dwRGBGain >>  0) & 0xFF;
							}
							dwPrevTick = dwCurTick;
						}
						pImage->SetImageExtInfo(cExtInfo);
						
						if( S_OK == hr )
						{
							GetOut(0)->Deliver(pImage);
						}
						else
						{
							SW_TRACE_DEBUG("H264SourceFilter h264 read data %d failed.\n", cHeader.iDataSize);
							swpa_thread_sleep_ms(2000);
							fConnect = FALSE;
						}
					}
					pImage->Release();
				}
				else
				{
					SW_TRACE_NORMAL("<H264SourceFilter> h264 CreateSWImage failed.\n");
				}

				// print fps
				if( dwFps++ >= 100 )
				{
					dwIPCount--;
					dwIPTotal = dwIPCount > 0 ? dwIPTotal / dwIPCount / 1024 : dwIPTotal / 1024;

					
					DWORD dwCurTick = CSWDateTime::GetSystemTick();

					CHAR szInfo[256] = {0};
					CHAR szMsg[256] = {0};
					swpa_sprintf(szInfo, "h264 fps: %.1f w:%d,h:%d IP size:%dKB IP:1:%d.", 
						float(100*1000) / (dwCurTick - dwBeginTick), dwWidth, dwHeight, dwIPTotal, dwIPFrameCount);
					SW_TRACE_NORMAL("--- %s ---", szInfo);
					swpa_sprintf(szMsg, "H264采集:%s", szInfo);
					CSWMessage::SendMessage(MSG_APP_UPDATE_STATUS, (WPARAM)szMsg, 0);

					dwBeginTick = dwCurTick;
					dwFps = dwIPCount = dwIPTotal = dwIPSize = dwIPFrameCount = 0;
					fIsReady = FALSE;
				}

				SAFE_DELETE(pbInfo);
			}

			break;
		default:
			SW_TRACE_DEBUG("<H264SourceFilter>h264 unknow type:%d.\n", cHeader.iID);
			break;
		}
	}

	SW_TRACE_NORMAL("H264SourceFilter h264 exit.\n");
	return S_OK;
}

HRESULT CSWH264SourceFilter::CreateMenuXML(TiXmlDocument& cXMLDoc)
{
	return E_NOTIMPL;
}

HRESULT CSWH264SourceFilter::SendCtrlCmd(const DWORD& dwId, const DWORD& dwInfoSize, const PBYTE pbInfo, PDWORD pdwRespondSize)
{
	struct
	{
		DWORD dwId;
		DWORD dwInfoSize;
	} cHeader;

	struct
	{
		DWORD dwId;
		DWORD dwRet;
		DWORD dwInfoSize;
	} cRespond;

	cHeader.dwId = dwId;
	cHeader.dwInfoSize = dwInfoSize;

	HRESULT hr = m_cSocketCtrl.Send(&cHeader, sizeof(cHeader), NULL);
	if (SUCCEEDED(hr))
	{
		if (pbInfo != NULL && cHeader.dwInfoSize > 0)
	{
		hr = m_cSocketCtrl.Send(pbInfo, cHeader.dwInfoSize, NULL);
		}
		
		if (SUCCEEDED(hr))
		{
			hr = m_cSocketCtrl.Read(&cRespond, sizeof(cRespond), NULL );
			if( S_OK == hr )
			{
				if( cRespond.dwId != cHeader.dwId || cRespond.dwRet != 0 )
				{
					SW_TRACE_DEBUG("<H264SourceFilter>send cmd return failed! id:%d, respond:%d,%d.\n", cHeader.dwId, cRespond.dwId, cRespond.dwRet );
					hr = E_FAIL;
				}
				if( NULL != pdwRespondSize )
				{
					*pdwRespondSize = cRespond.dwInfoSize;
				}
			}
		}
		else
		{
			SW_TRACE_DEBUG("<H264SourceFilter>send pbInfo failed!\n");
		}
	}
	else
	{
		SW_TRACE_DEBUG("<H264SourceFilter>send cmd failed!\n");
	}

	return hr;
}

HRESULT CSWH264SourceFilter::OnInitMenu(WPARAM wParam, LPARAM lParam)
{
	// 生成固定的菜单。
	TiXmlDocument cXMLDoc;
	HRESULT hr = CreateMenuXML(cXMLDoc);
	if( S_OK != hr )
	{
		return hr;
	}

	m_cCtrlMutex.Lock();
	hr = ConnectCtrl();
	if( S_OK != hr )
	{
		m_cCtrlMutex.Unlock();
		return hr;
	}

	TiXmlPrinter XmlPrinter;
	cXMLDoc.Accept(&XmlPrinter);

	DWORD dwRespondSize = 0;
	hr = SendCtrlCmd(CMD_INIT_MENU, swpa_strlen(XmlPrinter.CStr()) + 1, (const PBYTE)XmlPrinter.CStr(), &dwRespondSize);

	DisconnectCtrl();
	m_cCtrlMutex.Unlock();
	return hr;
}

HRESULT CSWH264SourceFilter::OnSetCharOverlay(WPARAM wParam, LPARAM lParam)
{
	if( wParam == 0 )
	{
		return E_INVALIDARG;
	}

	m_cCtrlMutex.Lock();
	HRESULT hr = ConnectCtrl();
	if( S_OK != hr )
	{
		m_cCtrlMutex.Unlock();
		return hr;
	}

	H264_OVERLAY_INFO cOverlayInfo;
	swpa_memcpy(&cOverlayInfo, (PVOID)wParam, sizeof(cOverlayInfo));
	DWORD dwRespondSize = 0;
	INT iInfoSize = sizeof(cOverlayInfo);
	if( !m_fEnableOverlay )
	{
		iInfoSize = 0;
	}
	hr = SendCtrlCmd(CMD_CHAR_OVERLAY, iInfoSize, (const PBYTE)(&cOverlayInfo), &dwRespondSize);

	DisconnectCtrl();
	m_cCtrlMutex.Unlock();
	return hr;
}
/**
*@brief 读取DM368心跳包信息。
*/
HRESULT CSWH264SourceFilter::OnGetHeartbeat(WPARAM wParam, LPARAM lParam)
{
	if( lParam == 0 || wParam == 0 )
	{
		return E_INVALIDARG;
	}

	DWORD dwSize = wParam;
	PBYTE pbData = (PBYTE)lParam;

	m_cCtrlMutex.Lock();
	HRESULT hr = ConnectCtrl();
	if( S_OK != hr )
	{
		m_cCtrlMutex.Unlock();
		return hr;
	}

	DWORD dwRespondSize = 0;
	hr = SendCtrlCmd(CMD_HEARTBEAT, 0, NULL, &dwRespondSize);
	if( S_OK == hr && dwRespondSize > 0 )
	{
		CHAR* pszInfo = (CHAR*)swpa_mem_alloc(dwRespondSize + 1);
		if (NULL != pszInfo)
		{
			hr = m_cSocketCtrl.Read(pszInfo, dwRespondSize, NULL);
			if( S_OK == hr )
			{
				swpa_memcpy(pbData, pszInfo, dwRespondSize <= dwSize ? dwRespondSize : dwSize);
				if( dwRespondSize > dwSize )
				{
					hr = E_OUTOFMEMORY;
				}
			}
			swpa_mem_free(pszInfo);
		}
	}

	DisconnectCtrl();
	m_cCtrlMutex.Unlock();
	return hr;
}
/**
*@brief 同步DM368时间。
*/
HRESULT CSWH264SourceFilter::OnSyncTime(WPARAM wParam, LPARAM lParam)
{
	m_cCtrlMutex.Lock();
	HRESULT hr = ConnectCtrl();
	if( S_OK != hr )
	{
		m_cCtrlMutex.Unlock();
		return hr;
	}

	DWORD dwRespondSize = 0;
	CSWDateTime cTime;
	char pszTime[128] = {0};
	sprintf(pszTime, "%04d/%02d/%02d %02d:%02d:%02d %03d", 
		cTime.GetYear(), cTime.GetMonth(), cTime.GetDay(), 
		cTime.GetHour(), cTime.GetMinute(), cTime.GetSecond(), cTime.GetMSSecond());

	hr = SendCtrlCmd(CMD_SYNC_TIME, swpa_strlen(pszTime) + 1, (const PBYTE)pszTime, &dwRespondSize);

	DisconnectCtrl();
	m_cCtrlMutex.Unlock();
	return hr;
}
/**
*@brief CVBS菜单左按键。
*/
HRESULT CSWH264SourceFilter::OnMenuLeft(WPARAM wParam, LPARAM lParam)
{
	m_cCtrlMutex.Lock();
	HRESULT hr = ConnectCtrl();
	if( S_OK != hr )
	{
		m_cCtrlMutex.Unlock();
		return hr;
	}

	DWORD dwRespondSize = 0;
	DWORD dwKey = KEY_LEFT;
	hr = SendCtrlCmd(CMD_MENU_CTRL, 4, (const PBYTE)(&dwKey), &dwRespondSize);
	if( S_OK == hr && dwRespondSize > 0 )
	{
		CHAR* pszInfo = (CHAR*)swpa_mem_alloc(dwRespondSize + 1);
		hr = m_cSocketCtrl.Read(pszInfo, dwRespondSize, NULL);
		if( S_OK == hr )
		{
			//todo...
		}
		if( pszInfo != NULL )
		{
			swpa_mem_free(pszInfo);
			pszInfo = NULL;
		}
	}

	DisconnectCtrl();
	m_cCtrlMutex.Unlock();
	return hr;
}
/**
*@brief CVBS菜单右按键。
*/
HRESULT CSWH264SourceFilter::OnMenuRight(WPARAM wParam, LPARAM lParam)
{
	m_cCtrlMutex.Lock();
	HRESULT hr = ConnectCtrl();
	if( S_OK != hr )
	{
		m_cCtrlMutex.Unlock();
		return hr;
	}

	DWORD dwRespondSize = 0;
	DWORD dwKey = KEY_RIGHT;
	hr = SendCtrlCmd(CMD_MENU_CTRL, 4, (const PBYTE)(&dwKey), &dwRespondSize);
	if( S_OK == hr && dwRespondSize > 0 )
	{
		CHAR* pszInfo = (CHAR*)swpa_mem_alloc(dwRespondSize + 1);
		hr = m_cSocketCtrl.Read(pszInfo, dwRespondSize, NULL);
		if( S_OK == hr )
		{
			//todo...
		}
		if( pszInfo != NULL )
		{
			swpa_mem_free(pszInfo);
			pszInfo = NULL;
		}
	}

	DisconnectCtrl();
	m_cCtrlMutex.Unlock();
	return hr;
}
/**
*@brief CVBS菜单上按键。
*/
HRESULT CSWH264SourceFilter::OnMenuUp(WPARAM wParam, LPARAM lParam)
{
	m_cCtrlMutex.Lock();
	HRESULT hr = ConnectCtrl();
	if( S_OK != hr )
	{
		m_cCtrlMutex.Unlock();
		return hr;
	}

	DWORD dwRespondSize = 0;
	DWORD dwKey = KEY_UP;
	hr = SendCtrlCmd(CMD_MENU_CTRL, 4, (const PBYTE)(&dwKey), &dwRespondSize);
	if( S_OK == hr && dwRespondSize > 0 )
	{
		CHAR* pszInfo = (CHAR*)swpa_mem_alloc(dwRespondSize + 1);
		hr = m_cSocketCtrl.Read(pszInfo, dwRespondSize, NULL);
		if( S_OK == hr )
		{
			//todo...
		}
		if( pszInfo != NULL )
		{
			swpa_mem_free(pszInfo);
			pszInfo = NULL;
		}
	}

	DisconnectCtrl();
	m_cCtrlMutex.Unlock();
	return hr;
}
/**
*@brief CVBS菜单下按键。
*/
HRESULT CSWH264SourceFilter::OnMenuDwon(WPARAM wParam, LPARAM lParam)
{
	m_cCtrlMutex.Lock();
	HRESULT hr = ConnectCtrl();
	if( S_OK != hr )
	{
		m_cCtrlMutex.Unlock();
		return hr;
	}

	DWORD dwRespondSize = 0;
	DWORD dwKey = KEY_DWON;
	hr = SendCtrlCmd(CMD_MENU_CTRL, 4, (const PBYTE)(&dwKey), &dwRespondSize);
	if( S_OK == hr && dwRespondSize > 0 )
	{
		CHAR* pszInfo = (CHAR*)swpa_mem_alloc(dwRespondSize + 1);
		hr = m_cSocketCtrl.Read(pszInfo, dwRespondSize, NULL);
		if( S_OK == hr )
		{
			//todo...
		}
		if( pszInfo != NULL )
		{
			swpa_mem_free(pszInfo);
			pszInfo = NULL;
		}
	}

	DisconnectCtrl();
	m_cCtrlMutex.Unlock();
	return hr;
}
/**
*@brief CVBS菜单确定按键。
*/
HRESULT CSWH264SourceFilter::OnMenuOK(WPARAM wParam, LPARAM lParam)
{
	m_cCtrlMutex.Lock();
	HRESULT hr = ConnectCtrl();
	if( S_OK != hr )
	{
		m_cCtrlMutex.Unlock();
		return hr;
	}

	DWORD dwRespondSize = 0;
	DWORD dwKey = KEY_OK;
	hr = SendCtrlCmd(CMD_MENU_CTRL, 4, (const PBYTE)(&dwKey), &dwRespondSize);
	if( S_OK == hr && dwRespondSize > 0 )
	{
		CHAR* pszInfo = (CHAR*)swpa_mem_alloc(dwRespondSize + 1);
		hr = m_cSocketCtrl.Read(pszInfo, dwRespondSize, NULL);
		if( S_OK == hr )
		{
			//todo...
		}
		if( pszInfo != NULL )
		{
			swpa_mem_free(pszInfo);
			pszInfo = NULL;
		}
	}

	DisconnectCtrl();
	m_cCtrlMutex.Unlock();
	return hr;
}
/**
*@brief CVBS菜单取消按键。
*/
HRESULT CSWH264SourceFilter::OnMenuCancel(WPARAM wParam, LPARAM lParam)
{
	m_cCtrlMutex.Lock();
	HRESULT hr = ConnectCtrl();
	if( S_OK != hr )
	{
		m_cCtrlMutex.Unlock();
		return hr;
	}

	DWORD dwRespondSize = 0;
	DWORD dwKey = KEY_CANCEL;
	hr = SendCtrlCmd(CMD_MENU_CTRL, 4, (const PBYTE)(&dwKey), &dwRespondSize);
	if( S_OK == hr && dwRespondSize > 0 )
	{
		CHAR* pszInfo = (CHAR*)swpa_mem_alloc(dwRespondSize + 1);
		hr = m_cSocketCtrl.Read(pszInfo, dwRespondSize, NULL);
		if( S_OK == hr )
		{
			//todo...
		}
		if( pszInfo != NULL )
		{
			swpa_mem_free(pszInfo);
			pszInfo = NULL;
		}
	}

	DisconnectCtrl();
	m_cCtrlMutex.Unlock();
	return hr;
}

/**
*@brief 取DM368版本信息。
*/
HRESULT CSWH264SourceFilter::OnGetVersion(WPARAM wParam, LPARAM lParam)
{
	if( lParam == 0 )
	{
		return E_INVALIDARG;
	}
	m_cCtrlMutex.Lock();
	HRESULT hr = ConnectCtrl();
	if( S_OK != hr )
	{
		m_cCtrlMutex.Unlock();
		return hr;
	}

	DWORD dwRespondSize = 0;
	hr = SendCtrlCmd(CMD_GET_VERSION, 0, NULL, &dwRespondSize);
	if( S_OK == hr && dwRespondSize > 0 )
	{
		CHAR* pszInfo = (CHAR*)swpa_mem_alloc(dwRespondSize + 1);
		hr = m_cSocketCtrl.Read(pszInfo, dwRespondSize, NULL);
		if( S_OK == hr )
		{
			if( wParam > dwRespondSize )
			{
				swpa_memcpy((PVOID)lParam, pszInfo, dwRespondSize);
			}
			else
			{
				hr = E_OUTOFMEMORY;
			}
		}
		if( pszInfo != NULL )
		{
			swpa_mem_free(pszInfo);
			pszInfo = NULL;
		}
	}

	DisconnectCtrl();
	m_cCtrlMutex.Unlock();
	return hr;
}

HRESULT CSWH264SourceFilter::OnSetH264Param(WPARAM wParam, LPARAM lParam)
{
	if( 0 == wParam )
	{
		return E_INVALIDARG;
	}

	H264_PARAM* pcH264Param = (H264_PARAM*)wParam;

	m_cCtrlMutex.Lock();
	HRESULT hr = ConnectCtrl();
	if( S_OK != hr )
	{
		m_cCtrlMutex.Unlock();
		return hr;
	}

	DWORD dwRespondSize = 0;
	hr = SendCtrlCmd(CMD_SET_H264_PARAM, sizeof(H264_PARAM), (const PBYTE)(pcH264Param), &dwRespondSize);
	if( S_OK == hr && dwRespondSize > 0 )
	{
		CHAR* pszInfo = (CHAR*)swpa_mem_alloc(dwRespondSize + 1);
		hr = m_cSocketCtrl.Read(pszInfo, dwRespondSize, NULL);
		if( S_OK == hr )
		{
			//todo...
		}
		if( pszInfo != NULL )
		{
			swpa_mem_free(pszInfo);
			pszInfo = NULL;
		}
	}

	DisconnectCtrl();
	m_cCtrlMutex.Unlock();
	return hr;
}

HRESULT CSWH264SourceFilter::ResetDM368()
{
	//return 0;
	return (0 == swpa_device_reset(SWPA_DEVICE_DM368)) ? S_OK : E_FAIL;
}

HRESULT CSWH264SourceFilter::InitDM368()
{
	if( 0 != swpa_device_dm368_ready() )
	{
		return E_FAIL;
	}

	SW_TRACE_DEBUG("<H264SourceFilter>set usb0 ip 123.123.219.218.\n");
	if( 0 != swpa_utils_shell("ifconfig usb0 123.123.219.218", NULL) )
	{
		SW_TRACE_DEBUG("<H264SourceFilter>set usb0 ip 123.123.219.218 failed!\n");
		return E_FAIL;
	}
	else 
	{
		// 设置子网掩码
		swpa_utils_shell("ifconfig usb0 netmask 255.255.255.0", NULL);
	}

	// 设备368的配置。

	// 同步时间。
	HRESULT hr = S_OK;
	hr = OnSyncTime(0,0);
	if( S_OK != hr )
	{
		SW_TRACE_DEBUG("<H264SourceFilter>synctime return:0x%08x.\n", hr);
	}
	else
	{
		m_dwLastSyncTime = CSWDateTime::GetSystemTick();
	}
	// 字符叠加.
	hr = OnSetCharOverlay((WPARAM)(&m_cOverlayInfo), 0);
	if( S_OK != hr )
	{
		SW_TRACE_DEBUG("<H264SourceFilter>SetCharOverlay return:0x%08x.\n", hr);
	}
	else
	{
		SW_TRACE_DEBUG("<H264SourceFilter>SetCharOverlay return:0x%08x. info:%s,size:%d,xy:(%d,%d),color:0x%08x.\n", 
			hr, m_cOverlayInfo.szInfo, m_cOverlayInfo.iFontSize, m_cOverlayInfo.iTopX, m_cOverlayInfo.iTopY, m_cOverlayInfo.dwColor);
	}

	// 设置压缩参数
	hr = OnSetH264Param((WPARAM)&m_cH264Param, 0);
	if( S_OK != hr )
	{
		SW_TRACE_DEBUG("<H264SourceFilter>SetH264Param return:0x%08x.\n", hr);
	}
	else
	{
		SW_TRACE_DEBUG("<H264SourceFilter>SetH264Param rate:%d i:%d.\n", m_cH264Param.iTargetBitrate, m_cH264Param.intraFrameInterval);
	}

	// 取版本号。
	char szVersion[1024] = {0};
	if(S_OK == OnGetVersion(1024, (LPARAM)szVersion))
	{
		SW_TRACE_NORMAL("DM368 version:%s\n", szVersion);
	}
	else
	{
		SW_TRACE_NORMAL("DM368 get version failed\n");
	}

	return S_OK;
}
