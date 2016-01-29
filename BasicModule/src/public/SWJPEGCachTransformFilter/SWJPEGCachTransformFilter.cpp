
#include "SWFC.h"
#include "SWJPEGCachTransformFilter.h"
#include "SWGB28181Parameter.h"
CSWJPEGCachTransformFilter::CSWJPEGCachTransformFilter()
    : CSWBaseFilter(1,1)
	, CSWMessage(MSG_RECOGNIZE_CTRL_START, MSG_RECOGNIZE_CTRL_END)
	, m_pTrackerCfg(NULL)
	, m_fSendJPEG(FALSE)
	, m_fSendDebug(FALSE)
	, m_pThread(NULL)
	, m_fInitialized(FALSE)
	, m_iCarArriveTriggerType(0)
	, m_iIOLevel(0)
	, m_fReverseRunFilterFlag(FALSE)
{
	GetIn(0)->AddObject(CLASSID(CSWImage));
    GetOut(0)->AddObject(CLASSID(CSWPosImage));
}

CSWJPEGCachTransformFilter::~CSWJPEGCachTransformFilter()
{
	Clear();
}

VOID CSWJPEGCachTransformFilter::Clear()
{
	SAFE_RELEASE(m_pThread);
}

HRESULT CSWJPEGCachTransformFilter::Initialize(
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

    SW_TRACE_DEBUG("<CSWJPEGCachTransformFilter>work index : %d.", cInitParam.nPlateRecogParamIndex);
	HRESULT hr = CSWBaseLinkCtrl::GetInstance()->InitVideoRecoger(cInitParam);
	if(FAILED(hr))
	{
        SW_TRACE_DEBUG("CSWJPEGCachTransformFilter algorithm initialize failed!\n");
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

HRESULT CSWJPEGCachTransformFilter::RegisterCallBackFunction(PVOID OnDSPAlarm, PVOID pvParam)
{
	typedef VOID (*fnOnDSPAlarm)(PVOID pvParam, INT iInfo);
	HRESULT hr = CSWBaseLinkCtrl::GetInstance()->RegisterCallBack((fnOnDSPAlarm)OnDSPAlarm, pvParam);
	SW_TRACE_DEBUG("RegisterCallBackFunction(0x%08x,0x%08x) return 0x%08x\n", (INT)OnDSPAlarm, (INT)pvParam, hr);
	return hr;
}

VOID CSWJPEGCachTransformFilter::OnProcessProxy(PVOID pvParam)
{
	if(pvParam != NULL)
	{
        CSWJPEGCachTransformFilter* pThis = (CSWJPEGCachTransformFilter*)pvParam;
		pThis->OnProcess();
	}
}

HRESULT CSWJPEGCachTransformFilter::Run()
{
	if( !m_fInitialized )
	{
        SW_TRACE_DEBUG("CSWJPEGCachTransformFilter does not Initialize");
		return E_FAIL;
	}

	HRESULT hr = S_OK;
	if( GetState() != FILTER_RUNNING )
	{
		CSWBaseFilter::Run();
        hr = m_pThread->Start((START_ROUTINE)&CSWJPEGCachTransformFilter::OnProcessProxy, (PVOID)this);
	}

	return hr;
}

HRESULT CSWJPEGCachTransformFilter::Stop()
{
	if(NULL != m_pThread)
	{
		m_pThread->Stop();
	}
	return CSWBaseFilter::Stop();
}

HRESULT CSWJPEGCachTransformFilter::OnRecognizePhoto(WPARAM wParam, LPARAM lParam)
{
    return S_OK;
}

HRESULT CSWJPEGCachTransformFilter::OnRecognizeGetJPEG(WPARAM wParam, LPARAM lParam)
{
	return S_OK;
}


HRESULT CSWJPEGCachTransformFilter::OnRecognizeOutPutDebug(WPARAM wParam, LPARAM lParam)
{
	return S_OK;
}

HRESULT CSWJPEGCachTransformFilter::OnTriggerEnable(WPARAM wParam, LPARAM lParam)
{
	SW_TRACE_DEBUG("OnTriggerEnable(0x%08x,0x%08x)", wParam, lParam);
	m_iCarArriveTriggerType = (INT)wParam;
	return S_OK;
}

HRESULT CSWJPEGCachTransformFilter::OnIOEvent(WPARAM wParam, LPARAM lParam)
{
	m_iIOLevel = (INT)wParam;
	SW_TRACE_DEBUG("OnIOEvent(0x%08x,0x%08x)", wParam, lParam);
	return S_OK;
}

/**
 *@brief 动态修改识别参数
 */
HRESULT CSWJPEGCachTransformFilter::OnModifyParam(WPARAM wParam, LPARAM lParam)
{
    return S_OK;
}

HRESULT CSWJPEGCachTransformFilter::Receive(CSWObject* obj)
{
	if(IsDecendant(CSWImage, obj))
	{
		CSWImage* pImage = (CSWImage*)obj;
        // 输出不丢抓拍图
		if(pImage->IsCaptureImage())
		{
            CSWPosImage* pPosImage = new CSWPosImage(pImage, 0, m_fSendDebug);
			if (NULL == pPosImage)
			{
				SW_TRACE_ERROR("Err: no memory\n");
				return E_OUTOFMEMORY;
			}
            GetOut(0)->Deliver(pPosImage);
            pPosImage->Release();
            return S_OK;
		}

		//放入队列
		m_cMutexImage.Lock();
		if( m_lstImage.GetCount() < MAX_IMAGE_COUNT )
		{
			pImage->AddRef();
			m_lstImage.AddTail(pImage);
			if( S_OK != m_cSemImage.Post() )
			{
                SW_TRACE_DEBUG("<CSWJPEGCachTransformFilter>SemImage post failed. list count:%d.\n", m_lstImage.GetCount());
			}
		}
		m_cMutexImage.Unlock();
	}

	return S_OK;
}

HRESULT CSWJPEGCachTransformFilter::OnProcess()
{
    SW_TRACE_DEBUG("CSWJPEGCachTransformFilter run.\n");

	DWORD dwFps = 0;
	DWORD dwBeginTick = CSWDateTime::GetSystemTick();

    CSWPosImage* pPosImage = NULL;

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
			CSWImage* pImage = m_lstImage.RemoveHead();
            m_cMutexImage.Unlock();

			// print fps
			if( dwFps++ >= 50 )
			{
				DWORD dwCurTick = CSWDateTime::GetSystemTick();
                CHAR szInfo[256] = {0};
                swpa_sprintf(szInfo, "==JPEG fps:%.1f==", float(50*1000) / (dwCurTick - dwBeginTick));

                SW_TRACE_NORMAL("--- %s---\n", szInfo);

				dwBeginTick = dwCurTick;
				dwFps = 0;
			}

            if(NULL == pPosImage)
            {
                pPosImage = new CSWPosImage(pImage, 0, m_fSendDebug);
            }

            if(NULL != pPosImage)
            {
                GetOut(0)->Deliver(pPosImage);
                SAFE_RELEASE(pPosImage);
            }
			SAFE_RELEASE(pImage);
		}
		else
		{
			m_cMutexImage.Unlock();
		}
	}

    SW_TRACE_DEBUG("CSWJPEGCachTransformFilter exit.\n");
	return S_OK;
}

HRESULT CSWJPEGCachTransformFilter::CarArriveEvent(CARARRIVE_INFO_STRUCT *pCarArriveInfo)
{
	return S_OK;
}

HRESULT CSWJPEGCachTransformFilter::CarLeftEvent(CARLEFT_INFO_STRUCT *pCarLeftInfo)
{
	return S_OK;
}
HRESULT CSWJPEGCachTransformFilter::GB28181Alarm(LPCSTR szMsg)
{
    printf("%s %d\n",__FUNCTION__,__LINE__);
    GB28181_AlarmInfo info;
    swpa_strcpy(info.szAlarmMsg,szMsg);
    SWPA_DATETIME_TM tm;
    CSWDateTime dt;
    dt.GetTime(&tm);
    swpa_sprintf(info.szAlarmTime,"%d-%02d-%02dT%02d:%02d:%02d.%03d",tm.year,tm.month,tm.day,tm.hour,tm.min,tm.sec,tm.msec);
    SW_TRACE_NORMAL("CSWJPEGCachTransformFilter::OnSendAlarm %s %s %d ",info.szAlarmTime,info.szAlarmMsg,MSG_GB28181_SEND_ALARM);
    SendRemoteMessage(MSG_GB28181_SEND_ALARM,&info,sizeof(info));
    return S_OK;
}

