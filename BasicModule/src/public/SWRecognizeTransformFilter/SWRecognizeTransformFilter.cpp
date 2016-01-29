
#include "SWFC.h"
#include "SWRecognizeTransformFilter.h"
#include "SWGB28181Parameter.h"
CSWRecognizeTransformFilter::CSWRecognizeTransformFilter()
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
	, m_dwPlateCount(0)
{
	GetIn(0)->AddObject(CLASSID(CSWImage));
	GetOut(0)->AddObject(CLASSID(CSWPosImage));
	GetOut(1)->AddObject(CLASSID(CSWCarLeft));
	GetOut(2)->AddObject(CLASSID(CSWCameraDataPDU));
	GetOut(3)->AddObject(CLASSID(CSWImage));
}

CSWRecognizeTransformFilter::~CSWRecognizeTransformFilter()
{
	Clear();
}

VOID CSWRecognizeTransformFilter::Clear()
{
	SAFE_RELEASE(m_pThread);
}

HRESULT CSWRecognizeTransformFilter::Initialize(
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

	m_cSemImage.Create(0, MAX_IMAGE_COUNT);

	m_fInitialized = TRUE;
	return S_OK;

}

HRESULT CSWRecognizeTransformFilter::RegisterCallBackFunction(PVOID OnDSPAlarm, PVOID pvParam)
{
	typedef VOID (*fnOnDSPAlarm)(PVOID pvParam, INT iInfo);
	HRESULT hr = CSWBaseLinkCtrl::GetInstance()->RegisterCallBack((fnOnDSPAlarm)OnDSPAlarm, pvParam);
	SW_TRACE_DEBUG("RegisterCallBackFunction(0x%08x,0x%08x) return 0x%08x\n", (INT)OnDSPAlarm, (INT)pvParam, hr);
	return hr;
}

VOID CSWRecognizeTransformFilter::OnProcessProxy(PVOID pvParam)
{
	if(pvParam != NULL)
	{
		CSWRecognizeTransformFilter* pThis = (CSWRecognizeTransformFilter*)pvParam;
		pThis->OnProcess();
	}
}

HRESULT CSWRecognizeTransformFilter::Run()
{
	if( !m_fInitialized )
	{
		SW_TRACE_DEBUG("CSWRecognizeTransformFilter does not Initialize");
		return E_FAIL;
	}

	HRESULT hr = S_OK;
	if( GetState() != FILTER_RUNNING )
	{
		CSWBaseFilter::Run();
		hr = m_pThread->Start((START_ROUTINE)&CSWRecognizeTransformFilter::OnProcessProxy, (PVOID)this);
	}

	return hr;
}

HRESULT CSWRecognizeTransformFilter::Stop()
{
	if(NULL != m_pThread)
	{
		m_pThread->Stop();
	}
	return CSWBaseFilter::Stop();
}

HRESULT CSWRecognizeTransformFilter::OnRecognizePhoto(WPARAM wParam, LPARAM lParam)
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
	SW_TRACE_DEBUG("CSWBaseLinkCtrl::GetInstance()->ProessOneFrame %d ms", CSWDateTime::GetSystemTick() - dwNow);
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
HRESULT CSWRecognizeTransformFilter::OnRecognizeGetJPEG(WPARAM wParam, LPARAM lParam)
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
HRESULT CSWRecognizeTransformFilter::OnRecognizeOutPutDebug(WPARAM wParam, LPARAM lParam)
{
	m_fSendDebug = (BOOL)wParam;
	SW_TRACE_DEBUG("<RecognizeTransformFilter>Set RecognizeOutPutDebug %d.,Debug:%d\n", (UINT)wParam, m_fSendDebug);
	return S_OK;
}

HRESULT CSWRecognizeTransformFilter::OnTriggerEnable(WPARAM wParam, LPARAM lParam)
{
	SW_TRACE_DEBUG("OnTriggerEnable(0x%08x,0x%08x)", wParam, lParam);
	m_iCarArriveTriggerType = (INT)wParam;
	return S_OK;
}

HRESULT CSWRecognizeTransformFilter::OnIOEvent(WPARAM wParam, LPARAM lParam)
{
	m_iIOLevel = (INT)wParam;
	SW_TRACE_DEBUG("OnIOEvent(0x%08x,0x%08x)", wParam, lParam);
	return S_OK;
}

/**
 *@brief 动态修改识别参数
 */
HRESULT CSWRecognizeTransformFilter::OnModifyParam(WPARAM wParam, LPARAM lParam)
{
	TRACKER_CFG_PARAM* pCfgParam = (TRACKER_CFG_PARAM*)wParam;
	HRESULT hr = CSWBaseLinkCtrl::GetInstance()->ModifyVideoRecogerParam((*pCfgParam));
	if(FAILED(hr))
	{
		SW_TRACE_DEBUG("ModifyVideoRecogerParam failed!\n");
	}

	return hr;
}

HRESULT CSWRecognizeTransformFilter::OnGetCarLeftCount(WPARAM wParam, LPARAM lParam)
{
	DWORD *pdwCarCount = (DWORD *)lParam;
	if (NULL == pdwCarCount)
	{
		return E_INVALIDARG;
	}
	*pdwCarCount = m_dwPlateCount;
	return S_OK;
}

HRESULT CSWRecognizeTransformFilter::Receive(CSWObject* obj)
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
		m_cMutexImage.Unlock();
	}

	return S_OK;
}

HRESULT CSWRecognizeTransformFilter::OnProcess()
{
	SW_TRACE_DEBUG("CSWRecognizeTransformFilter run.\n");

	DWORD dwFps = 0;
	DWORD dwBeginTick = CSWDateTime::GetSystemTick();

	PROCESS_EVENT_STRUCT cProcessEvent;
	CSWPosImage* pPosImage = NULL;
	CSWCameraDataPDU* pCamPDU = NULL;

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
			if(!m_fSendJPEG || m_fSendDebug)
			{
				swpa_memset(&cProcessEvent, 0, sizeof(PROCESS_EVENT_STRUCT));
				cProcessEvent.fIsCheckLight = true;
				cProcessEvent.iCarArriveTriggerType = m_iCarArriveTriggerType;
				cProcessEvent.nIOLevel = m_iIOLevel;
				HRESULT hr = CSWBaseLinkCtrl::GetInstance()->ProcessOneFrame(pImage, &cProcessEvent);
				//释放资源
				if(pImage->GetLastImage())
				{
					pImage->Clear();
				}

				if(S_OK == hr)
				{
					if ( cProcessEvent.dwEventId & EVENT_CARARRIVE )
					{
						for( int i = 0; i < cProcessEvent.iCarArriveInfoCount; ++i )
						{	 
							CarArriveEvent(&cProcessEvent.rgCarArriveInfo[i]);
						}
					}
					if( cProcessEvent.dwEventId & EVENT_CARLEFT )
					{
						for( int i = 0; i < cProcessEvent.iCarLeftInfoCount; ++i )
						{
							CarLeftEvent(&cProcessEvent.rgCarLeftInfo[i]);
						}
					}					
					
					if(cProcessEvent.cTrackRectInfo.dwTrackCount > 0)
					{
						pPosImage = new CSWPosImage(pImage, cProcessEvent.cTrackRectInfo.dwTrackCount, m_fSendDebug);
						for(int i = 0; i < pPosImage->GetCount(); i++)
						{
							cProcessEvent.cTrackRectInfo.rgTrackRect[i].top *= 2;
							cProcessEvent.cTrackRectInfo.rgTrackRect[i].bottom *= 2;
							pPosImage->GetRect(i) = *(SW_RECT *)&cProcessEvent.cTrackRectInfo.rgTrackRect[i];
							//SW_TRACE_DEBUG("<%d><%d,%d,%d,%d>.\n", cProcessEvent.cTrackRectInfo.dwTrackCount, cProcessEvent.cTrackRectInfo.rgTrackRect[i].left,cProcessEvent.cTrackRectInfo.rgTrackRect[i].right,cProcessEvent.cTrackRectInfo.rgTrackRect[i].top,cProcessEvent.cTrackRectInfo.rgTrackRect[i].bottom);
						}
					}
					
					//摄像机参数
					pCamPDU = new CSWCameraDataPDU();
					pCamPDU->SetLightType(cProcessEvent.cLightType);
					pCamPDU->SetCplStatus(cProcessEvent.iCplStatus);
					pCamPDU->SetPluseLevel(cProcessEvent.iPulseLevel);
					GetOut(2)->Deliver(pCamPDU);
					SAFE_RELEASE(pCamPDU);		
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

			if( m_fSendJPEG)
			{
				//带车牌坐标的YUV流
				if(NULL == pPosImage)
				{
					pPosImage = new CSWPosImage(pImage, 0, m_fSendDebug);
				}
				GetOut(0)->Deliver(pPosImage);
			}
			SAFE_RELEASE(pPosImage);
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

HRESULT CSWRecognizeTransformFilter::CarArriveEvent(CARARRIVE_INFO_STRUCT *pCarArriveInfo)
{
	//todo..
	return S_OK;
}

HRESULT CSWRecognizeTransformFilter::CarLeftEvent(CARLEFT_INFO_STRUCT *pCarLeftInfo)
{
	m_dwPlateCount++;
	SW_TRACE_DEBUG("<RecognizeTransformFilter>car:%d.\n", m_dwPlateCount);


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
        GB28181Alarm((LPCSTR)pCarLeft->GetPlateNo());
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
HRESULT CSWRecognizeTransformFilter::GB28181Alarm(LPCSTR szMsg)
{
    printf("%s %d\n",__FUNCTION__,__LINE__);
    GB28181_AlarmInfo info;
    swpa_strcpy(info.szAlarmMsg,szMsg);
    SWPA_DATETIME_TM tm;
    CSWDateTime dt;
    dt.GetTime(&tm);
    swpa_sprintf(info.szAlarmTime,"%d-%02d-%02dT%02d:%02d:%02d.%03d",tm.year,tm.month,tm.day,tm.hour,tm.min,tm.sec,tm.msec);
    SW_TRACE_NORMAL("CSWRecognizeTransformFilter::OnSendAlarm %s %s %d ",info.szAlarmTime,info.szAlarmMsg,MSG_GB28181_SEND_ALARM);
    SendRemoteMessage(MSG_GB28181_SEND_ALARM,&info,sizeof(info));
    return S_OK;
}

