
#include "SWFC.h"
#include "SWVPIFSourceFilter.h"

CSWVPIFSourceFilter::CSWVPIFSourceFilter()
	: CSWBaseFilter(0,1)
	, m_iFps(-1)
	, m_fInitialized(FALSE)
	, m_pThread(NULL)
	, m_iWidth(1600)
	, m_iHeight(1200)
	, m_pMemoryFactory(NULL)
{
	GetOut(0)->AddObject(CLASSID(CSWImage));
}

CSWVPIFSourceFilter::~CSWVPIFSourceFilter()
{
	Clear();
	swpa_capture_release();
}

HRESULT CSWVPIFSourceFilter::Initialize(PVOID pvParam)
{
	SW_TRACE_DEBUG("VPIFSourceFilter Initialize.\n");

	if( m_fInitialized )
	{
		return S_OK;
	}

	m_pThread = new CSWThread(this->GetName());
	if( NULL == m_pThread )
	{
		SW_TRACE_NORMAL("VPIFSourceFilter thread create failed!\n");
		return E_OUTOFMEMORY;
	}


	m_pMemoryFactory = CSWMemoryFactory::GetInstance(SW_SHARED_MEMORY);
	if( NULL == m_pMemoryFactory )
	{
		SW_TRACE_NORMAL("VPIFSourceFilter MemoryFactory GetInstance failed!\n");
		Clear();
		return E_OUTOFMEMORY;
	}

	if( 0 != swpa_capture_init(1, &m_iWidth, &m_iHeight) )
	{
		SW_TRACE_NORMAL("swpa_capture_init failed!\n");
		Clear();
		return E_OUTOFMEMORY;
	}

	SW_TRACE_NORMAL("<VPIFSourceFilter>swpa_capture_init w:%d,h:%d.\n", m_iWidth, m_iHeight);

	// put image
	for( int i = 0; i < MAX_IMAGE_COUNT; ++i )
	{
		CSWImage* pImage = NULL;
		if( S_OK != CSWImage::CreateSWImage(&pImage, SW_IMAGE_BT1120, m_iWidth, m_iHeight, m_pMemoryFactory, 0, 0, FALSE, "VPIF") )
		{
			SW_TRACE_NORMAL("VPIF CreateSWImage failed!\n");
			Clear();
			return E_OUTOFMEMORY;
		}
		else
		{
			CAPTURE_BUFFER cBuffer;
			cBuffer.virt_addr = pImage->GetImageBuffer(2);
			cBuffer.phys_addr = pImage->GetImageBuffer(1);
			cBuffer.len = (INT)pImage->GetImageBufferSize();
			SW_TRACE_DEBUG("<VPIFSourceFilter>put buffer: v:0x%08x, p:0x%08x, len:%d. image:0x%08x\n", cBuffer.virt_addr, cBuffer.phys_addr, cBuffer.len, (DWORD)pImage);
			if( 0 != swpa_capture_put_buffer(&cBuffer) )
			{
				SW_TRACE_NORMAL("swpa_capture_put_buffer failed!\n");
				Clear();
				return E_FAIL;
			}
			m_lstImage.AddTail(pImage);
		}
	}

	m_fInitialized = TRUE;
	return S_OK;
}

VOID CSWVPIFSourceFilter::OnProcessProxy(PVOID pvParam)
{
	if(pvParam != NULL)
	{
		CSWVPIFSourceFilter* pThis = (CSWVPIFSourceFilter*)pvParam;
		pThis->OnProcess();
	}
}

HRESULT CSWVPIFSourceFilter::Run()
{
	if( !m_fInitialized )
	{
		return E_FAIL;
	}

	HRESULT hr = S_OK;
	if( GetState() != FILTER_RUNNING )
	{
		CSWBaseFilter::Run();
		hr = m_pThread->Start((START_ROUTINE)&CSWVPIFSourceFilter::OnProcessProxy, (PVOID)this);
	}

	return hr;
}

HRESULT CSWVPIFSourceFilter::Stop()
{
	CSWBaseFilter::Stop();
	Clear();
	return S_OK;
}

HRESULT CSWVPIFSourceFilter::SetFps(INT iFps)
{
	m_iFps = iFps;
	return S_OK;
}

VOID CSWVPIFSourceFilter::Clear()
{
	if(m_pThread != NULL)
	{
		m_pThread->Stop();
		delete m_pThread;
		m_pThread = NULL;
	}

	swpa_capture_stop();
	while(!m_lstImage.IsEmpty())
	{
		CSWImage* pImage = m_lstImage.RemoveHead();
		if( pImage != NULL )
		{
			pImage->Release();
			pImage = NULL;
		}
	}
	m_pMemoryFactory = NULL;
	m_fInitialized = FALSE;
}

HRESULT CSWVPIFSourceFilter::OnProcess()
{
	SW_TRACE_DEBUG("VPIFSourceFilter run...\n");
	DWORD dwFrameNo = 0;
	DWORD dwFps = 0;
	DWORD dwBeginTick = CSWDateTime::GetSystemTick();

	if( 0 != swpa_capture_start() )
	{
		SW_TRACE_NORMAL("swpa_capture_start failed!\n");
		return E_FAIL;
	}

	BOOL fIsNeedReset = FALSE;
	while(GetState() == FILTER_RUNNING)
	{
		CAPTURE_BUFFER cBuffer;
		CAPTURE_IMAGE_INFO_Y* pImageInfoY = NULL;
		CAPTURE_IMAGE_INFO_C* pImageInfoC = NULL;

		int iRet = swpa_capture_get_buffer(&cBuffer, &pImageInfoY, &pImageInfoC);
		if( iRet != 0 )
		{
			SW_TRACE_DEBUG("<VPIFSourceFilter>swpa_capture_get_buffer ret:%d.\n", iRet);
			swpa_thread_sleep_ms(20);
			continue;
		}
		
		SW_POSITION cPos = m_lstImage.GetHeadPosition();
		SW_POSITION cLastPos = cPos;
		BOOL fSame = FALSE;
		CSWImage* pImage = NULL;
		while(m_lstImage.IsValid(cPos))
		{
			cLastPos = cPos;
			pImage = m_lstImage.GetNext(cPos);
			if( pImage->GetImageBuffer(2) == cBuffer.virt_addr )
			{
				fSame = TRUE;
				break;
			}
		}


		if( !fSame )
		{
			SW_TRACE_NORMAL("<ERROR>VPIFSourceFilter list find image failed! 0x%08x.\n", cBuffer.phys_addr);
			fIsNeedReset = TRUE;
			break;
		}
		else
		{
			pImage = m_lstImage.RemoveAt(cLastPos);
			if( NULL == pImage )
			{
				SW_TRACE_NORMAL("<ERROR>VPIFSourceFilter list remove image failed!\n");
				fIsNeedReset = TRUE;
				break;
			}
		}

		IMAGE_EXT_INFO cInfo;

		// set Image
		cInfo.iShutter = swpa_camera_conver_shutter(pImageInfoY->shutter_v);
		cInfo.iGain = swpa_camera_conver_gain(pImageInfoY->vgagain_a);
		cInfo.iRGain = swpa_camera_conver_rgb_gain(pImageInfoY->r_gain_v);
		cInfo.iGGain = swpa_camera_conver_rgb_gain(pImageInfoY->g_gain_v);
		cInfo.iBGain = swpa_camera_conver_rgb_gain(pImageInfoY->b_gain_v);

		BOOL fIsCapture = (1 == pImageInfoY->capture_en);

		if( fIsCapture )
		{
			cInfo.iAvgY = pImageInfoC->avg_y_8 + pImageInfoC->avg_y_9
				+ pImageInfoC->avg_y_10 + pImageInfoC->avg_y_11 + pImageInfoC->avg_y_12 + pImageInfoC->avg_y_13 + pImageInfoC->avg_y_14 + pImageInfoC->avg_y_15;
			cInfo.iAvgY /= 8;
		}
		else
		{
			cInfo.iAvgY = pImageInfoC->avg_y_0 + pImageInfoC->avg_y_1 + pImageInfoC->avg_y_2 + pImageInfoC->avg_y_3 + pImageInfoC->avg_y_4
				+ pImageInfoC->avg_y_5 + pImageInfoC->avg_y_6 + pImageInfoC->avg_y_7 + pImageInfoC->avg_y_8 + pImageInfoC->avg_y_9
				+ pImageInfoC->avg_y_10 + pImageInfoC->avg_y_11 + pImageInfoC->avg_y_12 + pImageInfoC->avg_y_13 + pImageInfoC->avg_y_14 + pImageInfoC->avg_y_15;
			cInfo.iAvgY /= 16;
		}


		pImage->SetImageExtInfo(cInfo);
		//  pImage->SetRefTime((DWORD)pImageInfoY->time_cnt_out_vd);
		pImage->SetRefTime(CSWDateTime::GetSystemTick());
		pImage->SetFrameNo(dwFrameNo);

		pImage->SetCaptureFlag(fIsCapture);
		pImage->SetFlag(pImageInfoY->capture_inf);
		if( fIsCapture )
		{
			SW_TRACE_DEBUG("<VPIF>Image captrue time: %d,flag:%d. iAvgY:%d. shutter:%d,gain:%d.\n", 
				pImage->GetRefTime(), pImageInfoY->capture_inf, cInfo.iAvgY, swpa_camera_conver_shutter(pImageInfoY->shutter_c), swpa_camera_conver_gain(pImageInfoY->vga_gain_c_1) );
		}

		dwFrameNo++;
		GetOut(0)->Deliver(pImage);

		pImage->Release();


		CSWImage* pNewImage = NULL;
		if( S_OK != CSWImage::CreateSWImage(&pNewImage, SW_IMAGE_BT1120, m_iWidth, m_iHeight, m_pMemoryFactory, 0, 0, FALSE, "VPIF") )
		{
			SW_TRACE_NORMAL("VPIFSourceFilter CreateSWImage failed!\n");
			fIsNeedReset = TRUE;
			break;
		}

		CAPTURE_BUFFER cNewBuffer;
		cNewBuffer.virt_addr = pNewImage->GetImageBuffer(2);
		cNewBuffer.phys_addr = pNewImage->GetImageBuffer(1);
		cNewBuffer.len = (INT)pNewImage->GetImageBufferSize();
		if( 0 != swpa_capture_put_buffer(&cNewBuffer) )
		{
			SW_TRACE_NORMAL("swpa_capture_put_buffer failed!\n");
			fIsNeedReset = TRUE;
			break;
		}

		m_lstImage.AddTail(pNewImage);

		// print fps
		if( dwFps++ >= 100 )
		{
			DWORD dwCurTick = CSWDateTime::GetSystemTick();

			CHAR szInfo[256] = {0};
			CHAR szMsg[256] = {0};
			swpa_sprintf(szInfo, "vpif fps: %.1f w:%d,h:%d.", float(100*1000) / (dwCurTick - dwBeginTick), m_iWidth, m_iHeight);
			SW_TRACE_NORMAL("--- %s ---\n", szInfo);
			swpa_sprintf(szMsg, "VPIF²É¼¯:%s", szInfo);
			CSWMessage::SendMessage(MSG_APP_UPDATE_STATUS, (WPARAM)szMsg, 0);

			SW_TRACE_NORMAL("<info>shutter:%d,gain:%d,rgbGain:%d,%d,%d.\n", cInfo.iShutter, cInfo.iGain, cInfo.iRGain, cInfo.iGGain, cInfo.iBGain);

			dwBeginTick = dwCurTick;
			dwFps = 0;
		}
	}

	if( 0 != swpa_capture_stop() )
	{
		SW_TRACE_NORMAL("drv_capture_stop failed!\n");
	}

	SW_TRACE_DEBUG("VPIFSourceFilter exit.\n");

	if( fIsNeedReset )
	{
		SW_TRACE_DEBUG("VPIFSourceFilter error reset.\n");
		CSWMessage::SendMessage(MSG_APP_RESETDEVICE, 0, 0);
	}
	return S_OK;
}


