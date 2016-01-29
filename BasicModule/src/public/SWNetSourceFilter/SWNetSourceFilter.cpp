#include "SWFC.h"
#include "SWNetSourceFilter.h"

CSWNetSourceFilter::CSWNetSourceFilter()
	: CSWBaseFilter(0,1)
	, m_iFps(12)
	, m_fInitialized(FALSE)
	, m_pThread(NULL)
	, m_iWidth(0)
	, m_iHeight(0)
	, m_pMemoryFactory(NULL)
{
	GetOut(0)->AddObject(CLASSID(CSWImage));
}

CSWNetSourceFilter::~CSWNetSourceFilter()
{
	Clear();
}

HRESULT CSWNetSourceFilter::Initialize(PVOID pvParam)
{
	if( m_fInitialized )
	{
		return S_OK;
	}

	if( NULL == pvParam )
	{
		return E_INVALIDARG;
	}

	memcpy(&m_cCameraInfo, (CAMERA_INFO*)pvParam, sizeof(m_cCameraInfo));

	m_pThread = new CSWThread(this->GetName());
	if( NULL == m_pThread )
	{
		return E_OUTOFMEMORY;
	}

	m_pMemoryFactory = CSWMemoryFactory::GetInstance(SW_SHARED_MEMORY);
	if( NULL == m_pMemoryFactory )
	{
		Clear();
		return E_OUTOFMEMORY;
	}

	m_fInitialized = TRUE;
	return S_OK;
}

VOID CSWNetSourceFilter::OnProcessProxy(PVOID pvParam)
{
	if(pvParam != NULL)
	{
		CSWNetSourceFilter* pThis = (CSWNetSourceFilter*)pvParam;
		pThis->OnProcess();
	}
}

HRESULT CSWNetSourceFilter::Run()
{
	if( !m_fInitialized )
	{
		return E_FAIL;
	}

	CSWBaseFilter::Run();

	return m_pThread->Start((START_ROUTINE)&CSWNetSourceFilter::OnProcessProxy, (PVOID)this);
}

HRESULT CSWNetSourceFilter::SetFps(INT iFps)
{
	m_iFps = iFps;
	return S_OK;
}

VOID CSWNetSourceFilter::Clear()
{
	if(m_pThread != NULL)
	{
		m_pThread->Stop();
		delete m_pThread;
		m_pThread = NULL;
	}

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
	m_cTcpSocket.Close();
	m_fInitialized = FALSE;
}

HRESULT CSWNetSourceFilter::OnProcess()
{
	BOOL fConnect = FALSE;
	int iRevTimeMs = 2000;
	DWORD dwRefTime = 0;
	const int IMAGE_NAME_LEN = 128;
	DWORD dwFrameNo = 0;

	DWORD dwFps = 0;
	DWORD dwBeginTick = CSWDateTime::GetSystemTick();

	HRESULT hr = S_OK;
	SW_TRACE_DEBUG("NetSourceFilter Running.\n");

	DWORD dwSleepMs = 0;

	while(GetState() == FILTER_RUNNING)
	{
		DWORD dwRead = 0;
		if( !fConnect )
		{
			if( S_OK != m_cTcpSocket.Close() || S_OK != m_cTcpSocket.Create() )
			{
				SW_TRACE_DEBUG("NetSourceFilter socket create failed!\n");
				swpa_thread_sleep_ms(2000);
				continue;
			}

			hr  = m_cTcpSocket.Connect(m_cCameraInfo.szIp, m_cCameraInfo.wPort);
			if( S_OK == hr )
			{
				SW_TRACE_DEBUG("NetSourceFilter Connet %s, %d.ok!\n", m_cCameraInfo.szIp, m_cCameraInfo.wPort);
				m_cTcpSocket.SetRecvTimeout(iRevTimeMs);
				dwRefTime = 0;
				dwFrameNo = 0;
				fConnect = TRUE;
			}
			else
			{
				SW_TRACE_DEBUG("NetSourceFilter Connet %s, %d.failed!\n", m_cCameraInfo.szIp, m_cCameraInfo.wPort);
				swpa_thread_sleep_ms(5000);
				continue;
			}
		}

		CSWImage* pImage = NULL;
		if( S_OK != CSWImage::CreateSWImage(&pImage, SW_IMAGE_JPEG, 1600, 1200, m_pMemoryFactory, 0, 0, FALSE, "NULL") )
		{
			SW_TRACE_DEBUG("NetSourceFilter CreateSWImage failed!\n");
			swpa_thread_sleep_ms(2000);
			continue;
		}

		HRESULT hr = S_OK;
		dwRefTime += 80;
		dwFrameNo++;

		// 接收图片时标
		DWORD dwRefTimeTemp = 0;
		hr = m_cTcpSocket.Read(&dwRefTimeTemp, sizeof(dwRefTimeTemp), &dwRead);
		if( S_OK != hr )
		{
			if (pImage) pImage->Release();
			fConnect = FALSE;
			swpa_thread_sleep_ms(2000);
			continue;
		}
		// 接收图片名字
		CHAR szFileName[IMAGE_NAME_LEN] = {0};
		hr = m_cTcpSocket.Read(szFileName, IMAGE_NAME_LEN, &dwRead);
		if( S_OK != hr )
		{
			if (pImage) pImage->Release();
			fConnect = FALSE;
			swpa_thread_sleep_ms(2000);
			continue;
		}

		DWORD dwImageSize = 0;
		hr = m_cTcpSocket.Read(&dwImageSize, sizeof(dwImageSize), &dwRead);
		if( S_OK != hr )
		{
			if (pImage) pImage->Release();
			fConnect = FALSE;
			swpa_thread_sleep_ms(2000);
			continue;
		}
		SW_COMPONENT_IMAGE cComponentImage;
		pImage->GetImage(&cComponentImage);

		DWORD dwReadSize = dwImageSize;
		// 接收图片数据
		hr = m_cTcpSocket.Read(cComponentImage.rgpbData[0], dwReadSize, &dwRead);

		if( S_OK != hr )
		{
			if (pImage) pImage->Release();
			fConnect = FALSE;
			swpa_thread_sleep_ms(2000);
			continue;
		}
		//测试协议图片高宽存放在视频帧名最后8个字节中
		if (m_iWidth == 0 || m_iHeight == 0)
		{
			memcpy(&m_iWidth, &szFileName[120], 4);
			memcpy(&m_iHeight, &szFileName[124], 4);
			SW_TRACE_DEBUG("<NetSourceFilter>read w:%d,h:%d. \n", m_iWidth, m_iHeight);
		}
		if (m_iWidth == 0 || m_iHeight == 0)
		{
			m_iWidth = 1600;
			m_iHeight = 1200;
		}
		
		static BOOL s_fIsCapture = FALSE;
		memcpy(&s_fIsCapture, &szFileName[116], 4);//抓拍标记需要TestServer.exe（版本2014.9.15.1）及以上才支持
		// set Image
		cComponentImage.iSize = (INT)dwImageSize;
		cComponentImage.iWidth = 3072;//m_iWidth; //todo: 因上位机发送的图片宽高有问题，直接用最大值替换
		cComponentImage.iHeight = 2048;//m_iHeight; //todo: 因上位机发送的图片宽高有问题，直接用最大值替换
		pImage->SetImage(cComponentImage);
		pImage->SetFrameName(szFileName);
		pImage->SetFrameNo(dwFrameNo);
		pImage->SetRefTime(dwRefTime);
		pImage->SetCaptureFlag(s_fIsCapture);

		Deliver(pImage);
		pImage->Release();

		// print fps
		if( dwFps++ >= 100 )
		{
			DWORD dwCurTick = CSWDateTime::GetSystemTick();
			INT iFps = (100*1000) / (dwCurTick - dwBeginTick);
			SW_TRACE_NORMAL("--- net fps: %.1f ---\n", float(100*1000) / (dwCurTick - dwBeginTick));

			dwBeginTick = dwCurTick;
			dwFps = 0;

			if ( m_iFps > 0 && iFps > m_iFps)
			{
				dwSleepMs = 1000/m_iFps - 1000/iFps;
			}
		}

		if (dwSleepMs > 0)
		{
			//SW_TRACE_DEBUG("[zydebug] sleep %d ms!\n", dwSleepMs);
			swpa_thread_sleep_ms(dwSleepMs<<1);
		}
	}

	SW_TRACE_DEBUG("NetSourceFilter Exit!\n");
	return S_OK;
}


