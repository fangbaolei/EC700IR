
#include "SWFC.h"
#include "SWRecognizeTransformEPFilter.h"

#include "svEPDetResult.h"

#include "SWGB28181Parameter.h"
CSWRecognizeTransformEPFilter::CSWRecognizeTransformEPFilter()
	: CSWBaseFilter(1,4)
	, CSWMessage(MSG_RECOGNIZE_CTRL_START, MSG_RECOGNIZE_CTRL_END)
	, m_pTrackerCfg(NULL)
	, m_fSendJPEG(FALSE)
	, m_fSendDebug(FALSE)
	, m_pThread(NULL)
	, m_fInitialized(FALSE)
	, m_iCarArriveTriggerType(0)
	, m_iIOLevel(0)
	, m_fReverseRunFilterFlag(FALSE)
    , m_pProcQueueThread(NULL)
	, m_dwPlateCount(0)
	, m_fEnableGB28181(FALSE)
{
	GetIn(0)->AddObject(CLASSID(CSWImage));
	GetOut(0)->AddObject(CLASSID(CSWPosImage));
	GetOut(1)->AddObject(CLASSID(CSWCarLeft));
	GetOut(2)->AddObject(CLASSID(CSWCameraDataPDU));
	GetOut(3)->AddObject(CLASSID(CSWImage));
}

CSWRecognizeTransformEPFilter::~CSWRecognizeTransformEPFilter()
{
	Clear();
}

VOID CSWRecognizeTransformEPFilter::Clear()
{
}

HRESULT CSWRecognizeTransformEPFilter::Initialize(
	INT iGlobalParamIndex
	, INT nLastLightType
	, INT nLastPulseLevel
	, INT nLastCplStatus
	, PVOID pvParam
	)
{
	if( m_fInitialized )
	{
		return E_FAIL;
	}
	if( NULL == pvParam )
	{
		return E_INVALIDARG;
	}

	m_pTrackerCfg = (TRACKER_CFG_PARAM *)pvParam;
	m_pTrackerCfg->iPlatform = 2;

	INIT_VIDEO_RECOGER_PARAM cInitParam;
	cInitParam.nPlateRecogParamIndex = iGlobalParamIndex;
	cInitParam.nLastLightType = nLastLightType;
	cInitParam.nLastPulseLevel = nLastPulseLevel;
	cInitParam.nLastCplStatus = nLastCplStatus;
	cInitParam.dwArmTick = CSWDateTime::GetSystemTick();
	swpa_memcpy(&cInitParam.cTrackerCfgParam, m_pTrackerCfg, sizeof(TRACKER_CFG_PARAM));

	SW_TRACE_DEBUG("<RecognizeTransformFilter>work index : %d.", cInitParam.nPlateRecogParamIndex);
	HRESULT hr = CSWBaseLinkCtrl::GetInstance()->InitVideoRecoger(cInitParam);
	if(FAILED(hr))
	{
		SW_TRACE_DEBUG("RecognizeTransformFilter algorithm initialize failed!\n");
		Clear();
		return hr;
	}

	m_pThread = new CSWThread(this->GetName());
	if( NULL == m_pThread )
	{
		SW_TRACE_DEBUG("RecognizeTransformFilter thread initialize failed!\n");
		Clear();
		return E_OUTOFMEMORY;
	}

    // ganzz 创建异步处理线程
    m_pProcQueueThread = new CSWThread(this->GetName());
    if( NULL == m_pThread )
    {
        SW_TRACE_DEBUG("RecognizeTransformFilter sync thread initialize failed!\n");
        Clear();
        return E_OUTOFMEMORY;
    }
	m_cSemImage.Create(0, MAX_IMAGE_COUNT);
    m_cSemProcQueue.Create(1, 1);
    m_nMaxProcQueueLen = 10;   // 最大异步处理缓存数

    m_semEP.Create(0, m_nMaxProcQueueLen + 1);
    m_lstEPList.SetMaxCount(m_nMaxProcQueueLen);

	m_fInitialized = TRUE;
	return S_OK;

}

HRESULT CSWRecognizeTransformEPFilter::RegisterCallBackFunction(PVOID OnDSPAlarm, PVOID pvParam)
{
	typedef VOID (*fnOnDSPAlarm)(PVOID pvParam, INT iInfo);
	HRESULT hr = CSWBaseLinkCtrl::GetInstance()->RegisterCallBack((fnOnDSPAlarm)OnDSPAlarm, pvParam);
	SW_TRACE_DEBUG("RegisterCallBackFunction(0x%08x,0x%08x) return 0x%08x\n", (INT)OnDSPAlarm, (INT)pvParam, hr);
	return hr;
}

VOID CSWRecognizeTransformEPFilter::OnProcessProxy(PVOID pvParam)
{
	if(pvParam != NULL)
	{
		CSWRecognizeTransformEPFilter* pThis = (CSWRecognizeTransformEPFilter*)pvParam;
		pThis->OnProcess();
	}
}

VOID CSWRecognizeTransformEPFilter::OnProcessSyncProxy(PVOID pvParam)
{
    if(pvParam != NULL)
    {
        CSWRecognizeTransformEPFilter* pThis = (CSWRecognizeTransformEPFilter*)pvParam;
        pThis->OnProcessSync();
    }
}
HRESULT CSWRecognizeTransformEPFilter::Run()
{
	if( !m_fInitialized )
	{
		SW_TRACE_DEBUG("CSWRecognizeTransformEPFilter does not Initialize");
		return E_FAIL;
	}

	HRESULT hr = S_OK;
	if( GetState() != FILTER_RUNNING )
	{
		CSWBaseFilter::Run();
		hr = m_pThread->Start((START_ROUTINE)&CSWRecognizeTransformEPFilter::OnProcessProxy, (PVOID)this);
        if (hr == S_OK)
        {
            hr = m_pProcQueueThread->Start((START_ROUTINE)&CSWRecognizeTransformEPFilter::OnProcessSyncProxy, (PVOID)this);
        }
	}

	return hr;
}

HRESULT CSWRecognizeTransformEPFilter::Stop()
{
	if(NULL != m_pThread)
	{
		m_pThread->Stop();
	}
	return CSWBaseFilter::Stop();
}

HRESULT CSWRecognizeTransformEPFilter::OnRecognizePhoto(WPARAM wParam, LPARAM lParam)
{
	if( wParam == 0 && lParam == 0 )
	{
		return E_INVALIDARG;
	}

	CSWImage* pImage = (CSWImage*)wParam;
	CSWCarLeftArray *carLeft = (CSWCarLeftArray *)lParam;
	
	PROCESS_EVENT_STRUCT cEvent;

	// 判断是不是抓拍图。
	// 目前DSP端需要通过此标志来做处理。
	if( !pImage->IsCaptureImage() )
	{
		SW_TRACE_DEBUG("<RecognizeTransformFilter>Not captrue image, can't recognize!\n");
		return E_INVALIDARG;
	}
	DWORD dwNow = CSWDateTime::GetSystemTick();
	HRESULT hr = CSWBaseLinkCtrl::GetInstance()->ProcessOneFrame(pImage, &cEvent);	
	SW_TRACE_DEBUG("CSWBaseLinkCtrl::GetInstance()->ProcessOneFrame %d ms", CSWDateTime::GetSystemTick() - dwNow);
	if(swpa_utils_file_exist("./test.txt"))
	{
		cEvent.dwEventId = EVENT_CARLEFT;
		cEvent.iCarLeftInfoCount = 2;
	}
	
	if(S_OK == hr && (cEvent.dwEventId & EVENT_CARLEFT))
	{
		carLeft->SetCount(cEvent.iCarLeftInfoCount);
		for(int i = 0; i < cEvent.iCarLeftInfoCount; i++)
		{
			carLeft->Get(i)->SetParameter(m_pTrackerCfg, &cEvent.rgCarLeftInfo[i]);
			if(swpa_utils_file_exist("./test.txt"))
			{
				carLeft->Get(i)->SetPlateNo("测试");
				carLeft->Get(i)->SetCarColor(CC_WHITE);
			}
		}
	}
	return hr;
}

/**
*@brief 设置识别开关
*/
HRESULT CSWRecognizeTransformEPFilter::OnRecognizeGetJPEG(WPARAM wParam, LPARAM lParam)
{
	m_fSendJPEG = (BOOL)wParam;
	if(!m_fSendJPEG)
	{
		m_fSendDebug = FALSE;
	}
	SW_TRACE_DEBUG("<RecognizeTransformFilter>Set RecognizeEnable %d,JPEG:%d.\n", (UINT)wParam, m_fSendJPEG);
	return S_OK;
}

/**
*@brief 输出调试码流
*/
HRESULT CSWRecognizeTransformEPFilter::OnRecognizeOutPutDebug(WPARAM wParam, LPARAM lParam)
{
	m_fSendDebug = (BOOL)wParam;
	SW_TRACE_DEBUG("<RecognizeTransformFilter>Set RecognizeOutPutDebug %d.,Debug:%d\n", (UINT)wParam, m_fSendDebug);
	return S_OK;
}

HRESULT CSWRecognizeTransformEPFilter::OnTriggerEnable(WPARAM wParam, LPARAM lParam)
{
	SW_TRACE_DEBUG("OnTriggerEnable(0x%08x,0x%08x)", wParam, lParam);
	m_iCarArriveTriggerType = (INT)wParam;
	return S_OK;
}

HRESULT CSWRecognizeTransformEPFilter::OnIOEvent(WPARAM wParam, LPARAM lParam)
{
	m_iIOLevel = (INT)wParam;
	SW_TRACE_DEBUG("OnIOEvent(0x%08x,0x%08x)", wParam, lParam);
	return S_OK;
}

/**
 *@brief 动态修改识别参数
 */
HRESULT CSWRecognizeTransformEPFilter::OnModifyParam(WPARAM wParam, LPARAM lParam)
{
	TRACKER_CFG_PARAM* pCfgParam = (TRACKER_CFG_PARAM*)wParam;
	HRESULT hr = CSWBaseLinkCtrl::GetInstance()->ModifyVideoRecogerParam((*pCfgParam));
	if(FAILED(hr))
	{
		SW_TRACE_DEBUG("ModifyVideoRecogerParam failed!\n");
	}
	m_fModifyParam = TRUE;
	return hr;
}

HRESULT CSWRecognizeTransformEPFilter::OnGetCarLeftCount(WPARAM wParam, LPARAM lParam)
{
	DWORD *pdwCarCount = (DWORD *)lParam;
	if (NULL == pdwCarCount)
	{
		return E_INVALIDARG;
	}
	*pdwCarCount = m_dwPlateCount;
	return S_OK;
}
HRESULT CSWRecognizeTransformEPFilter::Receive(CSWObject* obj)
{
	if(IsDecendant(CSWImage, obj))
	{
		CSWImage* pImage = (CSWImage*)obj;
		// 识别的时候不接收抓拍图
		if(pImage->IsCaptureImage())
		{
			if(m_fSendJPEG)
			{
				CSWPosImage* pPosImage = new CSWPosImage(pImage, 0, m_fSendDebug);
				GetOut(0)->Deliver(pPosImage);
				pPosImage->Release();
			}
			return S_OK;
		}
		// 测试协议等待队列有空闲空间
		// 默认VPIF采集的帧名都是“VPIF”。
		if(swpa_strcmp("VPIF", pImage->GetFrameName()) != 0 )
		{
			while( TRUE )
			{
				m_cMutexImage.Lock();
				if( m_lstImage.GetCount() < MAX_IMAGE_COUNT )
				{
					m_cMutexImage.Unlock();
					break;
				}
				else
				{
					m_cMutexImage.Unlock();
					CSWApplication::Sleep(100);
				}
			}
		}
		//放入队列
		m_cMutexImage.Lock();
		if( m_lstImage.GetCount() < MAX_IMAGE_COUNT )
		{
			pImage->AddRef();
			m_lstImage.AddTail(pImage);
			if( S_OK != m_cSemImage.Post() )
			{
				SW_TRACE_DEBUG("<RecognizeTransformFilter>SemImage post failed. list count:%d.\n", m_lstImage.GetCount());
			}
		}
		else
		{
			SW_TRACE_DEBUG("Arm process image list full!!");
		}
		m_cMutexImage.Unlock();
	}

	return S_OK;
}

HRESULT CSWRecognizeTransformEPFilter::OnProcess()
{
	SW_TRACE_DEBUG("CSWRecognizeTransformEPFilter run.\n");

	DWORD dwFps = 0;
	DWORD dwBeginTick = CSWDateTime::GetSystemTick();
	CSWCameraDataPDU* pCamPDU = NULL;
	m_fModifyParam = TRUE;

	while(S_OK == m_pThread->IsValid() && GetState() == FILTER_RUNNING)
	{
		if( S_OK != m_cSemImage.Pend(1000) )
		{
			SW_TRACE_DEBUG("m_cSemImage.Pend failed.");
			continue;
		}
		m_cMutexImage.Lock();
		if( !m_lstImage.IsEmpty() )
		{
			//SW_TRACE_DEBUG("m_fEnable=%d,m_fOutPutDebug=%d", m_fEnable, m_fOutPutDebug);
			CSWImage* pImage = m_lstImage.RemoveHead();
			m_cMutexImage.Unlock();

			if(NULL != pImage && m_fModifyParam )
			{
				// 测试协议时是帧的图, 需换成场图。
				if( swpa_strcmp("VPIF", pImage->GetFrameName()) == 0 )
				{
					m_sEPCtrl.Init(m_pTrackerCfg, NULL, pImage->GetWidth(), pImage->GetHeight());
				}
				else
				{
					m_sEPCtrl.Init(m_pTrackerCfg, NULL, pImage->GetWidth(), pImage->GetHeight() / 2);
				}
			    m_fModifyParam = FALSE;
			    SAFE_RELEASE(pImage);
			    continue;
			}

			// TODO Process
			SW_COMPONENT_IMAGE cImageCop;
			pImage->GetImage(&cImageCop);

			HV_COMPONENT_IMAGE hvImageYuv;

			hvImageYuv.nImgType = HV_IMAGE_YUV_420;

			// 测试协议时是帧的图
			if( swpa_strcmp("VPIF", pImage->GetFrameName()) == 0 )
			{
				hvImageYuv.iHeight = (pImage->GetHeight());
				hvImageYuv.iWidth = pImage->GetWidth();

				hvImageYuv.iStrideWidth[0] = (cImageCop.rgiStrideWidth[0]);
				hvImageYuv.iStrideWidth[1] = (cImageCop.rgiStrideWidth[1]);
				hvImageYuv.iStrideWidth[2] = 0;

				hvImageYuv.rgImageData[0].addr = cImageCop.rgpbData[0];
				hvImageYuv.rgImageData[1].addr = cImageCop.rgpbData[1];
				hvImageYuv.rgImageData[2].addr = 0;

				if (hvImageYuv.iHeight > 1024)
				{
					hvImageYuv.iHeight /= 2;
					hvImageYuv.iStrideWidth[0] = (cImageCop.rgiStrideWidth[0] << 1);
					hvImageYuv.iStrideWidth[1] = (cImageCop.rgiStrideWidth[1] << 1);
				}
			}
			else
			{
				hvImageYuv.iHeight = (pImage->GetHeight() / 2);
				hvImageYuv.iWidth = pImage->GetWidth();

				hvImageYuv.iStrideWidth[0] = (cImageCop.rgiStrideWidth[0] << 1);
				hvImageYuv.iStrideWidth[1] = (cImageCop.rgiStrideWidth[1] << 1);
				hvImageYuv.iStrideWidth[2] = 0;

				hvImageYuv.rgImageData[0].addr = cImageCop.rgpbData[0];
				hvImageYuv.rgImageData[1].addr = cImageCop.rgpbData[1];
				hvImageYuv.rgImageData[2].addr = 0;
			}

			//todo.
			int iDataSize = 1024 * 32;
			unsigned char* pbData = new unsigned char[iDataSize];

			if( !m_fSendJPEG || m_fSendDebug )
			{
				DWORD dwArmProcess = CSWDateTime::GetSystemTick();
				m_sEPCtrl.Process(hvImageYuv,
						pbData, iDataSize);
				DWORD dwArmProcessEnd = CSWDateTime::GetSystemTick();
				if( dwArmProcessEnd - dwArmProcess > 100 )
				{
					SW_TRACE_DEBUG("Arm process relay ms:%d.", dwArmProcessEnd - dwArmProcess);
				}
			}

			// 测试协议不丢帧。
			if( swpa_strcmp("VPIF", pImage->GetFrameName()) != 0 )
			{
				while(TRUE)
				{
					m_cMutexEPList.Lock();
					if( !m_lstEPList.IsFull() )
					{
						m_cMutexEPList.Unlock();
						break;
					}
					else
					{
						m_cMutexEPList.Unlock();
						CSWApplication::Sleep(200);
					}
				}
			}

			m_cMutexEPList.Lock();
			if( m_lstEPList.IsFull() )
			{
				SW_POSITION pos = m_lstEPList.GetHeadPosition();
				SW_POSITION lastpos = pos;
				INT index = 0;
				int iDeleteCount = 0;
				while(m_lstEPList.IsValid(pos))
				{
					lastpos = pos;
					PROC_QUEUE_ELEM cElem = m_lstEPList.GetNext(pos);
					if(  index % 3 == 2 )
					{
						m_lstEPList.RemoveAt(lastpos);
						SAFE_RELEASE(cElem.pImage);
						PROCESS_EVENT_STRUCT* pProcessEvent =
							(PROCESS_EVENT_STRUCT*)cElem.pData;
						if( pProcessEvent != NULL
								&& NULL != pProcessEvent->cSyncDetData.pbData )
						{
							delete[] pProcessEvent->cSyncDetData.pbData;
							pProcessEvent->cSyncDetData.pbData = NULL;
						}
						SAFE_DELETE(pProcessEvent);
						++iDeleteCount;
					}
					++index;
				}

				SW_TRACE_DEBUG("Arm process delete count:%d.", iDeleteCount);

				if( iDeleteCount == 0 )
				{
					SAFE_DELETE(pbData);
		            //释放资源
		            SAFE_RELEASE(pImage);
		            m_cMutexEPList.Unlock();
		            continue;
				}
			}

			 // 创建元素空间
			PROCESS_EVENT_STRUCT* pProcessEvent = new PROCESS_EVENT_STRUCT;
			if (pProcessEvent == NULL)
			{
				SW_TRACE_DEBUG("Sync queue out of mem.");
				m_cMutexEPList.Unlock();
				continue;
			}

			swpa_memset(pProcessEvent, 0, sizeof(PROCESS_EVENT_STRUCT));
			pProcessEvent->fIsCheckLight = true;
			pProcessEvent->iCarArriveTriggerType = m_iCarArriveTriggerType;
			pProcessEvent->nIOLevel = m_iIOLevel;
			PROC_QUEUE_ELEM cNewElem;
			pImage->AddRef();
			cNewElem.pImage = pImage;
			cNewElem.pData = pProcessEvent;
			pProcessEvent->cSyncDetData.pbData = pbData;
			pProcessEvent->cSyncDetData.nLen = iDataSize;
			m_lstEPList.AddTail(cNewElem);
			m_semEP.Post();
			m_cMutexEPList.Unlock();

            //释放资源
            SAFE_RELEASE(pImage);
        }
        else
        {
            m_cMutexImage.Unlock();
        }
	}

    SW_TRACE_DEBUG("RecognizeTransformFilter exit.\n");
    return S_OK;
}

HRESULT CSWRecognizeTransformEPFilter::OnProcessSync()
{
	static DWORD dwFps = 0;
	DWORD dwBeginTick = CSWDateTime::GetSystemTick();
	CSWCameraDataPDU* pCamPDU = NULL;

    while(S_OK == m_pProcQueueThread->IsValid() && GetState() == FILTER_RUNNING )
    {
        if( S_OK != m_semEP.Pend(1000) )
		{
			 SW_TRACE_DEBUG("m_semEP.Pend failed.");
			 continue;
		}

        m_cMutexEPList.Lock();
        PROC_QUEUE_ELEM cQueueElem;
        if( !m_lstEPList.IsEmpty() )
        {
        	cQueueElem = m_lstEPList.RemoveHead();
        }
        else
        {
        	m_cMutexEPList.Unlock();
        	continue;
        }
        m_cMutexEPList.Unlock();

        CSWImage* pImage = cQueueElem.pImage;
        PROCESS_EVENT_STRUCT* pProcessEvent = (PROCESS_EVENT_STRUCT*)(cQueueElem.pData);

        DWORD dwBeginTickRecong = CSWDateTime::GetSystemTick();
        HRESULT hr = S_OK;
        svEPDetApi::DET_ROI* pRoi = NULL;
        int iRoiCount = 0;
        if( !m_fSendJPEG || m_fSendDebug )
        {
        	hr = CSWBaseLinkCtrl::GetInstance()->ProcessOneFrame(pImage, pProcessEvent );
        	//svEPDetApi::SeparateResultRefPoint(pProcessEvent->cSyncDetData.pbData,
        	//		pProcessEvent->cSyncDetData.nLen, NULL,
        	//		&pRoi, &iRoiCount, NULL, NULL);
        }
        DWORD dwRelayTick = CSWDateTime::GetSystemTick() - dwBeginTickRecong;
        if( dwRelayTick > 100 )
        {
        	//SW_TRACE_DEBUG("<Recog>Relay:%d!!!!!!!!!!!!!!!", dwRelayTick);
        }

        //HRESULT hr = S_OK;
        if(S_OK == hr)
        {
        	if( !m_fSendJPEG || m_fSendDebug )
        	{
				m_sEPCtrl.SetLightType(pProcessEvent->nEnvLightType);

				if ( pProcessEvent->dwEventId & EVENT_CARARRIVE )
				{
					for( int i = 0; i < pProcessEvent->iCarArriveInfoCount; ++i )
					{
						CarArriveEvent(&pProcessEvent->rgCarArriveInfo[i]);
					}
				}
				if( pProcessEvent->dwEventId & EVENT_CARLEFT )
				{
					for( int i = 0; i < pProcessEvent->iCarLeftInfoCount; ++i )
					{
						CarLeftEvent(&pProcessEvent->rgCarLeftInfo[i]);
					}
				}

				//摄像机参数
				pCamPDU = new CSWCameraDataPDU();
				pCamPDU->SetLightType(pProcessEvent->cLightType);
				pCamPDU->SetCplStatus(pProcessEvent->iCplStatus);
				pCamPDU->SetPluseLevel(pProcessEvent->iPulseLevel);
				GetOut(2)->Deliver(pCamPDU);
				SAFE_RELEASE(pCamPDU);

				// 动态红灯加红
				// todo.  是否不定时而是判断灯的位移？
				static int siLastUpdateTick = CSWDateTime::GetSystemTick();
				DWORD dwCurTick = CSWDateTime::GetSystemTick();
				if( dwCurTick - siLastUpdateTick > 10 * 1000 * 60 )
				{
					SW_TRACE_NORMAL("<Recog>UpdateRedLight.");
					const int MAX_RED_RECT_COUNT = 8;
					HV_RECT rgRedRect[MAX_RED_RECT_COUNT];
					memcpy(rgRedRect, pProcessEvent->rcRedLight, sizeof(HV_RECT) * MAX_RED_RECT_COUNT);

					SendMessage(MSG_SET_REDLIGHT_RECT, (WPARAM)rgRedRect, MAX_RED_RECT_COUNT);
					siLastUpdateTick = dwCurTick;
				}
        	}
        }
        else
        {
            SW_TRACE_DEBUG("<RecognizeTransformFilter>ProceeOneFrame failed. 0x%08x.\n", hr);
            // 如果是DSP超时,则复位设备.
            if( hr == E_UNEXPECTED )
            {
                CSWMessage::SendMessage(MSG_APP_RESETDEVICE, 0, 0);
            }
        }

        static INT sLastSendTimeMs = 0;
        if( m_fSendJPEG || m_fSendDebug )
        //if( 0 )
        {
        	// 跳帧显示，避免对性能的影响。
        	BOOL fVideoSend = TRUE;
        	if( m_fSendDebug )
        	{
        		DWORD dwCurSendTick = CSWDateTime::GetSystemTick();
        		if( dwCurSendTick - sLastSendTimeMs < 300 )
        		{
        			fVideoSend = FALSE;
        		}
        	}

        	if( fVideoSend )
        	{
				CSWPosImage* pPosImage = NULL;
				pPosImage = new CSWPosImage(pImage, pProcessEvent->cTrackRectInfo.dwTrackCount, m_fSendDebug);
				for(int i = 0; i < pPosImage->GetCount(); i++)
				{
					pPosImage->GetRect(i).left = pProcessEvent->cTrackRectInfo.rgTrackRect[i].left;
					pPosImage->GetRect(i).top = pProcessEvent->cTrackRectInfo.rgTrackRect[i].top * 2;
					pPosImage->GetRect(i).right = pProcessEvent->cTrackRectInfo.rgTrackRect[i].right;
					pPosImage->GetRect(i).bottom = pProcessEvent->cTrackRectInfo.rgTrackRect[i].bottom * 2;
				}
				GetOut(0)->Deliver(pPosImage);
				SAFE_RELEASE(pPosImage);
				sLastSendTimeMs = CSWDateTime::GetSystemTick();
        	}
        }

       //if( m_fSendJPEG || m_fSendDebug )
        if( 0 )
        {
        	CSWPosImage* pPosImage = NULL;
			pPosImage = new CSWPosImage(pImage, iRoiCount, m_fSendDebug);
			for(int i = 0; i < iRoiCount; i++)
			{
				// todo.
				//pProcessEvent->cTrackRectInfo.rgTrackRect[i].top *= 2;
				//pProcessEvent->cTrackRectInfo.rgTrackRect[i].bottom *= 2;
				pPosImage->GetRect(i).left = pRoi[i].m_nLeft;
				pPosImage->GetRect(i).top = pRoi[i].m_nTop *= 2;
				pPosImage->GetRect(i).right = pRoi[i].m_nRight;
				pPosImage->GetRect(i).bottom = pRoi[i].m_nBottom *= 2;
			}
			GetOut(0)->Deliver(pPosImage);;
			SAFE_RELEASE(pPosImage);
        }

        // print fps
        if( dwFps++ >= 50 )
        {
            DWORD dwCurTick = CSWDateTime::GetSystemTick();
            CHAR szInfo[256] = {0};
            CHAR szMsg[256] = {0};
            swpa_sprintf(szInfo, "recognize fps: %.1f JPEG:%d,Debug:%d.", float(50*1000) / (dwCurTick - dwBeginTick), m_fSendJPEG, m_fSendDebug);

            SW_TRACE_NORMAL("--- %s---\n", szInfo);

            swpa_sprintf(szMsg, "识别:%s", szInfo);
            CSWMessage::SendMessage(MSG_APP_UPDATE_STATUS, (WPARAM)szMsg, 0);

            dwBeginTick = dwCurTick;
            dwFps = 0;
        }
        //释放资源
		if(pImage->GetLastImage())
		{
			pImage->Clear();
		}
        SAFE_RELEASE(pImage);

        if( NULL != pProcessEvent->cSyncDetData.pbData )
        {
        	delete[] pProcessEvent->cSyncDetData.pbData;
        	pProcessEvent->cSyncDetData.pbData = NULL;
        }
        SAFE_DELETE(pProcessEvent);
    }
}
HRESULT CSWRecognizeTransformEPFilter::CarArriveEvent(CARARRIVE_INFO_STRUCT *pCarArriveInfo)
{
	//todo..
	return S_OK;
}

HRESULT CSWRecognizeTransformEPFilter::CarLeftEvent(CARLEFT_INFO_STRUCT *pCarLeftInfo)
{
	m_dwPlateCount++;
	SW_TRACE_DEBUG("<RecognizeTransformEPFilter>car:%d.\n", m_dwPlateCount);


	if(NULL == pCarLeftInfo)
	{
		return E_INVALIDARG;
	}
	//通知Graph构造CarLeft对象
	CSWCarLeft* pCarLeft = NULL;
	SendCommand(1, (WPARAM)&pCarLeft, (LPARAM)pCarLeftInfo);
	if( pCarLeft == NULL || !IsDecendant(CSWCarLeft, pCarLeft))
	{
		SAFE_RELEASE(pCarLeft);
		SW_TRACE_DEBUG("pCarLeft == NULL or is not CSWCarLeft object, used default CSWCarLeft\n");
		pCarLeft = new CSWCarLeft();
	}
	pCarLeft->SetParameter(m_pTrackerCfg, pCarLeftInfo);

	if(NULL != pCarLeft)
	{
		if( m_fEnableGB28181 )
		{
			GB28181Alarm((LPCSTR)pCarLeft->GetPlateNo());
		}
		CSWString s;
		switch(pCarLeftInfo->cCoreResult.nVoteCondition)
		{
		case NEW2END : s = "NEW状态下跟踪丢失";break;
		case LEAVE_TRACK_RECT: s = "高速模式下车牌离开跟踪区域";break;
		case TRACK_MISS: s = "在跟踪区域内跟踪丢失";break;
		case ENDRECT_MISS: s = "在跟踪区域外跟踪丢失";break;
		case MAX_EQUAL_FRAME: s = "连续识别结果相同";break;
		case MAX_VOTE_FRAME: s = "有效结果超过阈值";break;
		}
		SW_TRACE_NORMAL("object carleft name:%s,PlateNo:%s[0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x][%s]", pCarLeft->Name(), (LPCSTR)pCarLeft->GetPlateNo(), pCarLeft->GetImage(0), pCarLeft->GetImage(1), pCarLeft->GetImage(2), pCarLeft->GetImage(3), pCarLeft->GetImage(4), pCarLeft->GetImage(5), pCarLeft->GetImage(6), (LPCSTR)s);

		if (pCarLeft->GetReverseRun()
			&& GetReverseRunFilterFlag())
		{
			SW_TRACE_DEBUG("Info: ReverseRun. Discard this carleft obj.\n");
		}
		else
		{
		        GetOut(1)->Deliver(pCarLeft);
		}
		
		GetOut(3)->Deliver(pCarLeft->GetImage(PLATE_IMAGE));
		pCarLeft->Release();
		SW_TRACE_DEBUG("object carleft done.\n");
	}
	return S_OK;
}
HRESULT CSWRecognizeTransformEPFilter::GB28181Alarm(LPCSTR szMsg)
{
    printf("%s %d\n",__FUNCTION__,__LINE__);
    GB28181_AlarmInfo info;
    swpa_strcpy(info.szAlarmMsg,szMsg);
    SWPA_DATETIME_TM tm;
    CSWDateTime dt;
    dt.GetTime(&tm);
    swpa_sprintf(info.szAlarmTime,"%d-%02d-%02dT%02d:%02d:%02d.%03d",tm.year,tm.month,tm.day,tm.hour,tm.min,tm.sec,tm.msec);
    SW_TRACE_NORMAL("CSWRecognizeTransformEPFilter::OnSendAlarm %s %s %d ",info.szAlarmTime,info.szAlarmMsg,MSG_GB28181_SEND_ALARM);
    SendRemoteMessage(MSG_GB28181_SEND_ALARM,&info,sizeof(info));
    return S_OK;
}

