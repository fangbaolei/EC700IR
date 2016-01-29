
#include "SWFC.h"
#include "SWRecognizeTransformPPFilter.h"

#include "geometry_search_header.h"
#include "swpa_camera.h"
#include "SWGB28181Parameter.h"

CSWRecognizeTransformPPFilter::CSWRecognizeTransformPPFilter()
	: CSWBaseFilter(1,4)
	, CSWMessage(MSG_RECOGNIZE_CTRL_START, MSG_RECOGNIZE_CTRL_END)
	, m_pTrackerCfg(NULL)
	, m_fSendJPEG(FALSE)
	, m_fSendDebug(FALSE)
	, m_fSendCarRedBox(FALSE)
	, m_fSendPlateRedBox(FALSE)
	, m_pThread(NULL)
	, m_fInitialized(FALSE)
	, m_iCarArriveTriggerType(0)
	, m_iIOLevel(0)
	, m_dwLastDomeCameraAdjustTick(0)
	, m_dwPlateCount(0)
	, m_fEnableGB28181(FALSE)
	, m_iCurProcessStatus(EPROCESS_STATUS_WAITTING)
	, m_fIsPresetPosition(FALSE)
	, m_fIsGammaMode(FALSE)
	, m_fIsDay(TRUE)
	, WAIT_DOME_READY(2000)
	, DETECT_CARBOX_BEGIN_TIME(2000)
	, DOME_MOVE_BLOCK_DONE_TIME(5000)
	, DOME_CAMERA_DO_ONE_FOCUS_TIME(6000)
	, DAY_DETECT_CARPLATE_BEGIN_TIME(9000)
	, NIGHT_DETECT_CARPLATE_BEGIN_TIME(12500)
	, DETECT_CARPLATE_END_TIME(17000)
	, DETECT_CARBOX_NUM(6)
	, DETECT_CARPLATE_NUM(3)
	, DETECT_PLATE_IMAGE_MAX_NUM(10)
	, DELETE_CARPLATE_MISS_THRESHOLD(4)
	, RECORD_VIDEO_BEGIN_TIME(1000)
	, DAY_RECORD_VIDEO_END_TIME(8000)
	, NIGHT_RECORD_VIDEO_END_TIME(12000)
{
	GetIn(0)->AddObject(CLASSID(CSWImage));
	GetOut(0)->AddObject(CLASSID(CSWPosImage));
	GetOut(1)->AddObject(CLASSID(CSWCarLeft));
//	GetOut(2)->AddObject(CLASSID(CSWCameraDataPDU));
//	GetOut(3)->AddObject(CLASSID(CSWImage));
}

CSWRecognizeTransformPPFilter::~CSWRecognizeTransformPPFilter()
{
	Clear();
}

VOID CSWRecognizeTransformPPFilter::Clear()
{
}

HRESULT CSWRecognizeTransformPPFilter::Initialize(
	INT iGlobalParamIndex
	, INT nLastLightType
	, INT nLastPulseLevel
	, INT nLastCplStatus
	, PVOID pvParam
	, PVOID pvMatchParam
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
	m_pMatchParam = (MATCHPARAM *)pvMatchParam;
	m_pTrackerCfg->iPlatform = 2;

	
	INIT_VIDEO_RECOGER_PARAM cInitParam;
	cInitParam.nPlateRecogParamIndex = iGlobalParamIndex;
	cInitParam.nLastLightType = nLastLightType;
	cInitParam.nLastPulseLevel = nLastPulseLevel;
	cInitParam.nLastCplStatus = nLastCplStatus;
	cInitParam.dwArmTick = CSWDateTime::GetSystemTick();
	swpa_memcpy(&cInitParam.cTrackerCfgParam, m_pTrackerCfg, sizeof(TRACKER_CFG_PARAM));

	SW_TRACE_DEBUG("<RecognizeTransformPPFilter>work index : %d.", cInitParam.nPlateRecogParamIndex);
	HRESULT hr = CSWBaseLinkCtrl::GetInstance()->InitVideoRecoger(cInitParam);
	if(FAILED(hr))
	{
		SW_TRACE_DEBUG("RecognizeTransformPPFilter algorithm initialize failed!\n");
		Clear();
		return hr;
	}

	m_pThread = new CSWThread(this->GetName());
	if( NULL == m_pThread )
	{
		SW_TRACE_DEBUG("RecognizeTransformPPFilter thread initialize failed!\n");
		Clear();
		return E_OUTOFMEMORY;
	}

	m_pJpegThread = new CSWThread(this->GetName());
	if(NULL == m_pJpegThread)
	{
		SW_TRACE_DEBUG("RecognizeTransformPPFilter JPEG thread initialize failed!\n"); 
		Clear();
		return E_OUTOFMEMORY;
	}

	swpa_memset(&m_cCurPanoramicInfo, 0, sizeof(CAR_PANORAMIC_INFO));
	swpa_memset(m_rgcCarWidthInfo, 0, 
				sizeof(CAR_WIDTH_INFO) * DETECT_AREA_COUNT_MAX);

	BOOL fUseLedLight = TRUE;
	if(m_pMatchParam->cPeccancyParkingParam.iNightUseLedLight)
		fUseLedLight = TRUE;
	else
		fUseLedLight = FALSE;
	m_cDomeCameraCtrl.SetNightUseLedLight(fUseLedLight);

	//INT iCameraModel = 0;
    CSWMessage::SendMessage(MSG_GET_CAMERA_MODEL, 0, (LPARAM)&m_iCameraModel);
	m_cDomeCameraCtrl.SetCameraModel(m_iCameraModel);

	m_lstCarLeftList.SetMaxCount(DETECT_CARPLATE_NUM);
	m_cSemImage.Create(0, MAX_IMAGE_COUNT);
	m_cSemJpegImage.Create(0, MAX_IMAGE_COUNT);
	m_fInitialized = TRUE;
	SW_TRACE_DEBUG("RecognizeTransformPPFilter initialize done!\n");
	return S_OK;

}




HRESULT CSWRecognizeTransformPPFilter::RegisterCallBackFunction(PVOID OnDSPAlarm, PVOID pvParam)
{
	typedef VOID (*fnOnDSPAlarm)(PVOID pvParam, INT iInfo);
	HRESULT hr = CSWBaseLinkCtrl::GetInstance()->RegisterCallBack((fnOnDSPAlarm)OnDSPAlarm, pvParam);
	SW_TRACE_DEBUG("RegisterCallBackFunction(0x%08x,0x%08x) return 0x%08x\n", (INT)OnDSPAlarm, (INT)pvParam, hr);
	return hr;
}

VOID CSWRecognizeTransformPPFilter::OnProcessProxy(PVOID pvParam)
{
	if(pvParam != NULL)
	{
		CSWRecognizeTransformPPFilter* pThis = (CSWRecognizeTransformPPFilter*)pvParam;
		pThis->OnProcess();
	}
}

VOID CSWRecognizeTransformPPFilter::OnSendJpegProxy(PVOID pvParam)
{
	if(pvParam != NULL)
	{
		CSWRecognizeTransformPPFilter *pThis = (CSWRecognizeTransformPPFilter *)pvParam;
		pThis->OnSendJpeg();
	}
}


HRESULT CSWRecognizeTransformPPFilter::Run()
{
	if( !m_fInitialized )
	{
		SW_TRACE_DEBUG("CSWRecognizeTransformPPFilter does not Initialize");
		return E_FAIL;
	}

	HRESULT hr = S_OK;
	if( GetState() != FILTER_RUNNING )
	{
		CSWBaseFilter::Run();
		hr = m_pThread->Start((START_ROUTINE)&CSWRecognizeTransformPPFilter::OnProcessProxy, (PVOID)this);
		if(hr == S_OK)
		{
			hr = m_pJpegThread->Start((START_ROUTINE)&CSWRecognizeTransformPPFilter::OnSendJpegProxy, (PVOID)this);
		}
	}

	return hr;
}

HRESULT CSWRecognizeTransformPPFilter::Stop()
{
	if(NULL != m_pThread)
	{
		m_pThread->Stop();
	}
	return CSWBaseFilter::Stop();
}

HRESULT CSWRecognizeTransformPPFilter::OnRecognizePhoto(WPARAM wParam, LPARAM lParam)
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
HRESULT CSWRecognizeTransformPPFilter::OnRecognizeGetJPEG(WPARAM wParam, LPARAM lParam)
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
HRESULT CSWRecognizeTransformPPFilter::OnRecognizeOutPutDebug(WPARAM wParam, LPARAM lParam)
{
	m_fSendDebug = (BOOL)wParam;
	SW_TRACE_DEBUG("<RecognizeTransformFilter>Set RecognizeOutPutDebug %d.,Debug:%d\n", (UINT)wParam, m_fSendDebug);
	return S_OK;
}

HRESULT CSWRecognizeTransformPPFilter::OnTriggerEnable(WPARAM wParam, LPARAM lParam)
{
	SW_TRACE_DEBUG("OnTriggerEnable(0x%08x,0x%08x)", wParam, lParam);
	m_iCarArriveTriggerType = (INT)wParam;
	return S_OK;
}

HRESULT CSWRecognizeTransformPPFilter::OnIOEvent(WPARAM wParam, LPARAM lParam)
{
	m_iIOLevel = (INT)wParam;
	SW_TRACE_DEBUG("OnIOEvent(0x%08x,0x%08x)", wParam, lParam);
	return S_OK;
}

/**
 @brief 动态修改识别参数
 */
HRESULT CSWRecognizeTransformPPFilter::OnModifyParam(WPARAM wParam, LPARAM lParam)
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

HRESULT CSWRecognizeTransformPPFilter::OnGetCarLeftCount(WPARAM wParam, LPARAM lParam)
{
	DWORD *pdwCarCount = (DWORD *)lParam;
	if (NULL == pdwCarCount)
	{
		return E_INVALIDARG;
	}
	*pdwCarCount = m_dwPlateCount;
	return S_OK;
}
HRESULT CSWRecognizeTransformPPFilter::Receive(CSWObject* obj)
{
	const DWORD SEND_NUM_MAX = 100000;
	static DWORD dwSendJudger = 0;
	if(IsDecendant(CSWImage, obj))
	{
		CSWImage* pImage = (CSWImage*)obj;

		// 测试协议等待队列有空闲空间
		// 默认VPIF采集的帧名都是“VPIF”。
		/*
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
		*/
		//放入JPEG队列
		m_cMutexJpegImage.Lock();
		if(dwSendJudger % 2 == 0)
		{
			if( m_lstJpegImage.GetCount() < MAX_IMAGE_COUNT )
			{
				pImage->AddRef();
				m_lstJpegImage.AddTail(pImage);
				if( S_OK != m_cSemJpegImage.Post() )
				{
					SW_TRACE_DEBUG("<RecognizeTransformFilter>SemImage post failed. list count:%d.\n", m_lstJpegImage.GetCount());
				}
			}
			else
			{
				//释放资源
				CSWImage *pDeleteImage = m_lstJpegImage.RemoveHead();
				SAFE_RELEASE(pDeleteImage);

				pImage->AddRef();
				m_lstJpegImage.AddTail(pImage);
			}
		}
		m_cMutexJpegImage.Unlock();

		//放入识别链表
		m_cMutexImage.Lock();
		if(dwSendJudger % 2 == 1)
		{
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
				//释放资源
				CSWImage *pDeleteImage = m_lstImage.RemoveHead();
				SAFE_RELEASE(pDeleteImage);

				pImage->AddRef();
				m_lstImage.AddTail(pImage);
			}
		}
		m_cMutexImage.Unlock();
	}
	dwSendJudger++;
	dwSendJudger = dwSendJudger >= SEND_NUM_MAX ? 0 : dwSendJudger;

	return S_OK;
}




HRESULT CSWRecognizeTransformPPFilter::OnProcess()
{
	static DWORD dwFps = 0;
	static DWORD dwLastDistanceTick = 0;

	DWORD dwBeginTick = CSWDateTime::GetSystemTick();
	DWORD dwLastDetectCarTick = 0;

	INT iCurPresetPos = 0;
	CSWImage *pImage = NULL;
    while(S_OK == m_pThread->IsValid() && GetState() == FILTER_RUNNING )
    {
        if( S_OK != m_cSemImage.Pend(1000) )
		{
			 SW_TRACE_DEBUG("m_semImage.Pend failed.");
			 continue;
		}

        m_cMutexImage.Lock();
        if( !m_lstImage.IsEmpty() )
        {
        	pImage = m_lstImage.RemoveHead();
        }
        else
        {
        	m_cMutexImage.Unlock();
        	continue;
        }
        m_cMutexImage.Unlock();

		DWORD dwCurTick = CSWDateTime::GetSystemTick();
		//等待球机准备好
		if((dwCurTick - m_dwGetDetectSignalTick) < 3000)
		{
			SAFE_RELEASE(pImage);
			continue;
		}


/*
		if((m_iCurProcessStatus == EPROCESS_STATUS_CAR_DETECTING) || 
		   (m_iCurProcessStatus == EPROCESS_STATUS_WAITTING) ||
		   (m_iCurProcessStatus != EPROCESS_STATUS_DOME_ADJUSTING && 
			m_iCurProcessStatus != EPROCESS_STATUS_PLATE_DETECTING))
*/
		if((m_iCurProcessStatus == EPROCESS_STATUS_CAR_DETECTING)) 
		{
			DetectDayOrNight(pImage);
		}
/*
	//FIXME
	static DWORD dwLastDet = 0;
	static DWORD dwTestCounter = 0;
		if((dwCurTick - dwLastDet) > 13000 && dwTestCounter == 3)
		{
			m_cDomeCameraCtrl.ResetConfig();
			dwLastDet = dwCurTick;
			dwTestCounter = 0;
			SW_TRACE_DEBUG("*****End*********");
		}
		else if((dwCurTick - dwLastDet) > 9000 && dwTestCounter == 2)
		{
			m_cDomeCameraCtrl.SetDetectPlateConfig(201);
			//m_cDomeCameraCtrl.SetGainLimit(0x5);
			dwTestCounter++;
		}
		else if((dwCurTick - dwLastDet) > 5000 && dwTestCounter == 1)
		{
			SW_TRACE_DEBUG("*****INFO: Set Focus One Push Mode.");
			m_cDomeCameraCtrl.DoOneFocus();
			//m_cDomeCameraCtrl.SetGainLimit(0x9);
			dwTestCounter++;
		}
		else if((dwCurTick - dwLastDet) > 3000 && dwTestCounter == 0)
		{
			SW_TRACE_DEBUG("*****Start*********");
			m_cDomeCameraCtrl.SetDetectCarConfig(FALSE);
			//m_cDomeCameraCtrl.SetGainLimit(0xF);
			m_cDomeCameraCtrl.SetDetectPlateConfig();
			dwTestCounter++;
		}
*/


		PROCESS_EVENT_STRUCT cProcessEvent, *pProcessEvent;
		DWORD dwBeginTickRecong = dwCurTick;

        HRESULT hr = S_OK;
        INT iRoiCount = 0;

		pProcessEvent = &cProcessEvent;
		swpa_memset(&cProcessEvent, 0, sizeof(PROCESS_EVENT_STRUCT));
		
		switch(m_iCurProcessStatus){
		case EPROCESS_STATUS_CAR_DETECTING:
			if(dwCurTick - dwLastDetectCarTick > 500)
			{
				if(m_iCurDetectCarCounter == 0)
				{
					AllCarBoxSetStatus(EDET_STATUS_MISSING);
				}
				m_iCurDetectCarCounter++;
				hr = GetCarInDetectArea(pImage);
				dwLastDetectCarTick = dwCurTick;
			}
			dwFps++;
			//多次检测车框
			if(m_iCurDetectCarCounter >= DETECT_CARBOX_NUM)
			{
				m_iCurProcessStatus = EPROCESS_STATUS_DOME_ADJUSTING;
//				SW_TRACE_DEBUG("INFO:+++++m_lstCarBoxList Count=%d\n", 
//							   m_lstCarBoxList[m_cCurPreset.iPresetNum].GetCount());
				DeleteMissCarBox();
				
				AllPlateMissIncrement();
				SetDebugRedBox();
				m_iCurDetectCarCounter = 0;
//				SW_TRACE_DEBUG("INFO:-----m_lstCarBoxList Count=%d\n", 
//							   m_lstCarBoxList[m_cCurPreset.iPresetNum].GetCount());
				m_cCurCarPosition = m_lstCarBoxList[m_cCurPreset.iPresetNum].GetHeadPosition();
				m_cDomeCameraCtrl.SetDetectCarConfig(IsDay());
				m_iCurCarPositionIndex = 0;

			}
			break;
		case EPROCESS_STATUS_DOME_ADJUSTING:
			hr = DomeCameraAdjust(pImage);
			if(hr == E_FAIL)
			{				
				m_cDomeCameraCtrl.DomeCameraCruiseResume();
				DeleteMissPlate();
				UpdateCarWidth();
				m_iCurProcessStatus = EPROCESS_STATUS_WAITTING;
			}
			break;
		case EPROCESS_STATUS_PLATE_DETECTING:
			hr = DetectPlate(pImage, &cProcessEvent);

			dwFps++;
			break;
		defualt: break;
		}

		if(m_iRecognizeRunFlag == RECOGNIZE_STOP)
		{
			SW_TRACE_DEBUG("INFO: Peccancy Detect STOP!\n");
			SAFE_RELEASE(m_cCurPanoramicInfo.pImage);
//			DeleteAllPlate();       //停止后删除所有车牌
//			ReleaseAllCarLeft();
			m_fSendCarRedBox = FALSE;
			m_fSendPlateRedBox = FALSE;
			m_fIsPresetPosition = FALSE;
			m_iCurDetectCarCounter = 0;
			m_iRecognizeRunFlag = RECOGNIZE_NOT_START;
			m_cDomeCameraCtrl.ResetConfig();
			m_iCurProcessStatus = EPROCESS_STATUS_WAITTING;
		}

        // print fps
        if( dwFps >= 50 )
        {
			// DWORD dwCurTick = CSWDateTime::GetSystemTick();
            CHAR szInfo[256] = {0};
            CHAR szMsg[256] = {0};
			DWORD dwSRAvailableSize = 0;
			CSWMemoryFactory * pMemoryFactory = CSWMemoryFactory::GetInstance(SW_SHARED_SR_MEMORY);
			
			
            swpa_sprintf(szInfo, "recognize fps: %.1f JPEG:%d,Debug:%d.", float(50*1000) / (dwCurTick - dwBeginTick), m_fSendJPEG, m_fSendDebug);

            SW_TRACE_NORMAL("--- %s---\n", szInfo);
			if(pMemoryFactory != NULL)
			{
				pMemoryFactory->GetAvailableSize(&dwSRAvailableSize);
				SW_TRACE_NORMAL("--- Share Region Free Size:%d MB---\n", (dwSRAvailableSize >> 20));
			}

            swpa_sprintf(szMsg, "识别:%s", szInfo);
            CSWMessage::SendMessage(MSG_APP_UPDATE_STATUS, (WPARAM)szMsg, 0);

            dwBeginTick = dwCurTick;
            dwFps = 0;
        }
        SAFE_RELEASE(pImage);
    }

	return E_FAIL;
}


HRESULT CSWRecognizeTransformPPFilter::OnSendJpeg()
{
	CSWImage *pImage = NULL;

    while(S_OK == m_pJpegThread->IsValid() && GetState() == FILTER_RUNNING )
	{
		if( S_OK != m_cSemJpegImage.Pend(1000) )
		{
			SW_TRACE_DEBUG("m_cSemJpegImage.Pend failed.");
			continue;
		}

        m_cMutexJpegImage.Lock();

        if( !m_lstJpegImage.IsEmpty() )
        {
        	pImage = m_lstJpegImage.RemoveHead();
        }
        else
        {
        	m_cMutexJpegImage.Unlock();
        	continue;
        }
		m_cMutexJpegImage.Unlock();

		CSWPosImage *pPosImage = NULL;
		if(m_fSendCarRedBox)
		{
			pPosImage = new CSWPosImage(pImage, m_iCarDebugCount, TRUE);
			for(int i = 0; i < m_iCarDebugCount; i++)
			{
				pPosImage->GetRect(i).left = m_rgcCarDebugRect[i].left;
				pPosImage->GetRect(i).top = m_rgcCarDebugRect[i].top;
				pPosImage->GetRect(i).right = m_rgcCarDebugRect[i].right;
				pPosImage->GetRect(i).bottom = m_rgcCarDebugRect[i].bottom;
			}
			if(m_iCurDetectAreaCount > 8)
				m_iCurDetectAreaCount = 8;
			for(INT i = 0; i < m_iCurDetectAreaCount; i++)
			{
				SW_POINT DetectAreaPoint[8];
				swpa_memset(DetectAreaPoint, 0, sizeof(SW_POINT)*8);
				if(m_rgcCurDetectArea[i].iValidPointCount > 8)
					m_rgcCurDetectArea[i].iValidPointCount = 8;
				for(INT j = 0; j < m_rgcCurDetectArea[i].iValidPointCount; j++)
				{
					DetectAreaPoint[j].x = m_rgcCurDetectArea[i].iValidArea_x[j];
					DetectAreaPoint[j].y = m_rgcCurDetectArea[i].iValidArea_y[j];
				}
				pPosImage->SetPolygon(i, m_rgcCurDetectArea[i].iValidPointCount,
									  DetectAreaPoint);
			}
		}
		else if(m_fSendPlateRedBox)
		{
			pPosImage = new CSWPosImage(pImage, m_iPlateDebugCount, TRUE);
			for(INT i = 0; i < m_iPlateDebugCount; i++)
			{
				pPosImage->GetRect(i).left = m_rgcPlateDebugRect[i].left;
				pPosImage->GetRect(i).top = m_rgcPlateDebugRect[i].top;
				pPosImage->GetRect(i).right = m_rgcPlateDebugRect[i].right;
				pPosImage->GetRect(i).bottom = m_rgcPlateDebugRect[i].bottom;
			}
		}
		else
		{
			pPosImage = new CSWPosImage(pImage, 0, FALSE);
		}

		if(pPosImage != NULL)
		{
			GetOut(0)->Deliver(pPosImage);
			pPosImage->Release();
		}


		SAFE_RELEASE(pImage);
	}

	return E_FAIL;
}

HRESULT CSWRecognizeTransformPPFilter::CarArriveEvent(CARARRIVE_INFO_STRUCT *pCarArriveInfo)
{
	//todo..
	return S_OK;
}

HRESULT CSWRecognizeTransformPPFilter::CarLeftEvent(CARLEFT_INFO_STRUCT *pCarLeftInfo,
	CSWImage *pImage)
{
	BOOL fIsPeccancy = FALSE;
	INT iBrightness = 0;
	m_dwPlateCount++;
	SW_TRACE_DEBUG("<RecognizeTransformPPFilter>car:%d.\n", m_dwPlateCount);


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
		if(pCarLeft == NULL)
			return E_FAIL;
	}
	pCarLeft->SetParameter(m_pTrackerCfg, pCarLeftInfo);

	if(!m_lstCarLeftList.IsFull())
	{
		//相对坐标转换为百分比
		SW_RECT cPlateRect;
		cPlateRect.left = (INT)((FLOAT)pCarLeftInfo->cCoreResult.rcBestPlatePos.left / 1920.0 * 100);
		cPlateRect.top = (INT)((FLOAT)(pCarLeftInfo->cCoreResult.rcBestPlatePos.top * 2) / 1080.0 * 100);
		cPlateRect.right = (INT)((FLOAT)pCarLeftInfo->cCoreResult.rcBestPlatePos.right / 1920.0 * 100);
		cPlateRect.bottom = (INT)((FLOAT)(pCarLeftInfo->cCoreResult.rcBestPlatePos.bottom * 2) / 1080.0 * 100);
		pCarLeft->SetImage(4, pImage, &cPlateRect);
		m_lstCarLeftList.AddTail(pCarLeft);
	}
	else
	{
		SAFE_RELEASE(pCarLeft);
	}

	iBrightness = (INT)(pCarLeft->GetCarAvgY());
	if((!IsSuitableBrightness(iBrightness)) && (!m_lstCarLeftList.IsFull()))
	{
		SW_TRACE_DEBUG("====INFO: --- plate brightness : %d\n", iBrightness);
		m_cDomeCameraCtrl.SetDetectPlateConfig(iBrightness);
		m_iFrameDelayCount = 20;   //800 ms  12.5 FPS
		return E_FAIL;
	}

	if(m_lstCarLeftList.GetCount() < DETECT_CARPLATE_NUM)
	{
		return S_OK;
	}

	GetMaxConfidenceCarLeft(&pCarLeft);

	
	if(pCarLeft == NULL)
	{
		SW_TRACE_DEBUG("ERROR: pCarLeft == NULL");
		return E_FAIL;
	}
	
	fIsPeccancy = IsPeccancyParking(pCarLeft, pImage);



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
		SW_TRACE_NORMAL("object carleft name:%s,PlateNo:%s %f, avgY:%d\n",
						pCarLeft->Name(), 
						(LPCSTR)pCarLeft->GetPlateNo(),
						pCarLeft->GetAverageConfidence(),
						(INT)pCarLeft->GetCarAvgY());
		SW_TRACE_NORMAL("[0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x]\n", 
						pCarLeft->GetImage(0), 
						pCarLeft->GetImage(1), 
						pCarLeft->GetImage(2), 
						pCarLeft->GetImage(3), 
						pCarLeft->GetImage(4), 
						pCarLeft->GetImage(5), 
						pCarLeft->GetImage(6));

		if(fIsPeccancy)
		{
			GetOut(1)->Deliver(pCarLeft);

		}
	}
	return S_OK;
}


HRESULT CSWRecognizeTransformPPFilter::GetVideo(CSWImage **ppVideo, INT *piVideoCount)
{
	if(piVideoCount == NULL || *piVideoCount >= 30)
	{
		SW_TRACE_DEBUG("ERROR: Invalid arguments.\n");
		return E_FAIL;
	}

	DWORD dwStartTick = m_dwRecordVideoStartTick;
	DWORD dwEndTick = m_dwRecordVideoEndTick;
	CSWImage *pVideo = NULL;
	CSWImage *pNewVideo = NULL;
	CSWMemoryFactory * pMemoryFactory = CSWMemoryFactory::GetInstance(SW_SHARED_SR_MEMORY);
	INT iNewVideoSize = 0;
	HRESULT hr = S_OK;
	if(dwStartTick > dwEndTick)
	{
		SW_TRACE_DEBUG("ERROR: get video tick error.\n");
		return E_FAIL;
	}
	else if((dwEndTick - dwStartTick) > NIGHT_RECORD_VIDEO_END_TIME)
	{
		SW_TRACE_DEBUG("ERROR: tick diff = %d.\n", dwEndTick - dwStartTick);
		SW_TRACE_DEBUG("INFO:dwStartTick=%d dwEndTick=%d\n",dwStartTick, dwEndTick);
		if(IsDay())
			dwStartTick = dwEndTick - DAY_RECORD_VIDEO_END_TIME - 1000;
		else
			dwStartTick = dwEndTick - NIGHT_RECORD_VIDEO_END_TIME - 1000;
	}
	else if((dwEndTick - dwStartTick) < 5000)
	{
		SW_TRACE_DEBUG("ERROR: tick diff = %d.\n", dwEndTick - dwStartTick);
		SW_TRACE_DEBUG("INFO:dwStartTick=%d dwEndTick=%d\n",dwStartTick, dwEndTick);
	}

	for(DWORD i = dwStartTick; i < dwEndTick; i += 1000)
	{
		if (FAILED(CSWMessage::SendMessage(MSG_H264_QUEUE_GET_VIDEO, WPARAM(i), LPARAM(&pVideo)))
			|| (NULL == pVideo))
		{
			SW_TRACE_DEBUG("Err: failed to get video\n");
			continue;
		}
		else
		{

			hr = CSWImage::CreateSWImage(&pNewVideo,
										 pVideo->GetType(), 
										 pVideo->GetWidth(), 
										 pVideo->GetHeight(),
										 pMemoryFactory, 
										 pVideo->GetFrameNo(), 
										 pVideo->GetRefTime(), 
										 pVideo->IsCaptureImage(),
										 pVideo->GetFrameName(),
										 pVideo->GetSize());
			if (S_OK != hr)
			{
				SW_TRACE_DEBUG("Err: failed to CreateSWImage() [0x%x]\n", hr);
				return E_FAIL;
			}
			SW_COMPONENT_IMAGE sNewComponentImage, sComponentImage;
			pNewVideo->GetImage(&sNewComponentImage);
			pVideo->GetImage(&sComponentImage);
	
			swpa_memcpy(sNewComponentImage.rgpbData[0], 
						sComponentImage.rgpbData[0], 
						sComponentImage.iSize);

			sNewComponentImage.iSize = sComponentImage.iSize;
			pNewVideo->SetImage(sNewComponentImage);

			if(*piVideoCount < 30)
			{
				ppVideo[*piVideoCount] = pNewVideo;
				(*piVideoCount)++;
			}
		}
	}
	return S_OK;
}

BOOL CSWRecognizeTransformPPFilter::IsInPeccancyTime(SWPA_DATETIME_TM *pCurTime, 
													 PECCANCY_PARKING_AREA *pPeccancyParkingArea)
{
	if(pCurTime == NULL || pPeccancyParkingArea == NULL)
	{
		SW_TRACE_DEBUG("ERROR: Invalid argument.\n");
		return FALSE;
	}
	INT i = 0;
	INT iBeginHour = pPeccancyParkingArea->iBeginTime[0];
	INT iBeginMin = pPeccancyParkingArea->iBeginTime[1];
	INT iEndHour = pPeccancyParkingArea->iEndTime[0];
	INT iEndMin = pPeccancyParkingArea->iEndTime[1];

	if((pCurTime->hour < iBeginHour) || 
		(pCurTime->hour > iEndHour) ||
	   (pCurTime->hour == iBeginHour && pCurTime->min < iBeginMin) ||
	   (pCurTime->hour == iEndHour && pCurTime->min > iEndMin))
	{
		return FALSE;
	}

	return TRUE;

}

HRESULT CSWRecognizeTransformPPFilter::BuildCarLeft(CSWImage *pDetPanoramicImage, 
													CSWImage *pDetCarFocusImage,
													CSWImage *pPeccancyPanoramicImage,
													CSWImage *pPeccancyCarFocusImage,
													SW_RECT *pDetCarFocusPlatePos,
													SW_RECT *pPeccancyCarFocusPlatePos,
													SWPA_DATETIME_TM *pDetCarFocusTime,
													SWPA_DATETIME_TM *pPeccancyCarFocusTime,
													LPCSTR szPresetName,
													CSWCarLeft *pCarLeft)
{
	//达到违章，出牌
	/*
	  0首次检到全景图 1首次检到缩放图 2达到违章全景图 3达到违章缩放图
	*/
	SWPA_DATETIME_TM cDetCarFocusTime, cPeccancyCarFocusTime;
	SW_RECT tmpRect;
	swpa_memset(&tmpRect, 0, sizeof(SW_RECT));

	cDetCarFocusTime = *pDetCarFocusTime;
	cPeccancyCarFocusTime = *pPeccancyCarFocusTime;
	pCarLeft->SetImage(0, pDetPanoramicImage, &tmpRect);
	pCarLeft->SetImage(1, pDetCarFocusImage, pDetCarFocusPlatePos);
	pCarLeft->SetImage(2, pPeccancyPanoramicImage, &tmpRect);
	pCarLeft->SetImage(3, pPeccancyCarFocusImage, pPeccancyCarFocusPlatePos);


	pCarLeft->SetDetectCarTime(cDetCarFocusTime);
	pCarLeft->SetPeccancyTime(cPeccancyCarFocusTime);
	pCarLeft->SetPresetName(szPresetName);


	return S_OK;
}


BOOL CSWRecognizeTransformPPFilter::IsPeccancyParking(CSWCarLeft *pCarLeft,
													  CSWImage *pImage)
{
	if(pCarLeft == NULL || pImage == NULL)
	{
		SW_TRACE_DEBUG("ERROR: Invalid argument.\n");
		return FALSE;
	}

	INT iPresetNum = m_cCurPreset.iPresetNum;
	SW_POSITION pos = m_lstCarPlateList[iPresetNum].GetHeadPosition();
	DWORD dwCount = m_lstCarPlateList[iPresetNum].GetCount();
	DWORD i = 0;
    SWPA_DATETIME_TM tm;
    CSWDateTime dt;
	HRESULT hr = E_FAIL;
	BOOL fIsPeccancy = TRUE;

	//车牌可信度过低，排除多检问题
	if(pCarLeft->GetAverageConfidence() < m_pMatchParam->cPeccancyParkingParam.fltConfidenceFilter)
	{
		SW_TRACE_DEBUG("INFO: CarLeft confidence:%f\n", 
					   pCarLeft->GetAverageConfidence());
		return FALSE;
	}


	if(m_iCurPeccancyAreaLimitTime == 0)
	{
		CSWImage *rgpVideo[30];
		INT iCurVideoCount = 0;
		INT i;
		dt.GetTime(&tm);
		SW_RECT PlatePos;
		CSWImage *pTmpImage = pCarLeft->GetImage(4, &PlatePos);
		BuildCarLeft(m_cCurPanoramicInfo.pImage,
					 pTmpImage,
					 m_cCurPanoramicInfo.pImage,
					 pTmpImage,
					 &PlatePos,
					 &PlatePos,
					 &tm,
					 &tm,
					 m_cCurPreset.szName,
					 pCarLeft);
		pCarLeft->SetImage(4, NULL);
		GetVideo(rgpVideo, &iCurVideoCount);
		for(i = 0; i < iCurVideoCount; i++)
		{
			pCarLeft->SetVideo(i, rgpVideo[i]);
			SAFE_RELEASE(rgpVideo[i]);
		}
		SW_TRACE_DEBUG("**************** Video Count = %d\n", iCurVideoCount);
		
		return fIsPeccancy;
	}

	for(i = 0; i < dwCount; i++)
	{
		CAR_PLATE_INFO *pCarPlateInfo = NULL;
		SW_POSITION curPos = pos;
		pCarPlateInfo = (CAR_PLATE_INFO *)m_lstCarPlateList[iPresetNum].GetNext(pos);
		if(pCarPlateInfo == NULL)
			return FALSE;
		SW_TRACE_DEBUG("*******%s - %s\n", (LPCSTR)pCarLeft->GetPlateNo(),
					   (LPCSTR)pCarPlateInfo->pCarLeft->GetPlateNo());

		if(!IsSimilarityPlate(pCarLeft, &m_CurDetectCarRect, 
							  pCarPlateInfo->pCarLeft, &pCarPlateInfo->CarRect) ||
		   IsMovingPlate(pCarLeft, &m_CurDetectCarRect, 
							  pCarPlateInfo->pCarLeft, &pCarPlateInfo->CarRect))
		{
			continue;
		}
		if((pCarPlateInfo->eDetectStatus == EDET_STATUS_PECCANCY))
		{
			SW_TRACE_DEBUG("Detect status: peccancy.\n");
			//更新检到的停车时间
			pCarPlateInfo->dwDetectParkingTick = CSWDateTime::GetSystemTick();
			return FALSE;
		}

		if(pCarPlateInfo->eDetectStatus == EDET_STATUS_PARKING)
		{
			DWORD dwCurTick = CSWDateTime::GetSystemTick();
			DWORD dwThreshold = m_iCurPeccancyAreaLimitTime * 1000;//ms
			CSWImage *rgpVideo[30];
			SW_RECT DetCarFocusPlatePos, PeccancyCarFocusPlatePos;
			CSWImage *pPanoramicImage = NULL;
			CSWImage *pDetCarImage = NULL;
			CSWImage *pPeccancyCarImage = NULL;
			IPC_SHARE_REGION rgeIpcShareRegion[30] = {IPC_SHARE_REGION_2};
			INT rgiVideoSize[30] = {0};
			INT iCurVideoCount = 0;
			INT iCurIndex = 0;
			INT i;
			if((dwCurTick - pCarPlateInfo->dwDetectParkingTick) < dwThreshold)
			{
				SW_TRACE_DEBUG("************* %d detect time is not enough.", pCarPlateInfo->dwDetectParkingTick);
				pCarPlateInfo->iMissCounter = 0;
				return FALSE;		//没到违章时间阈值
			}
			swpa_memset(rgpVideo, 0, sizeof(VOID *) * 30);
			dt.GetTime(&tm);
			swpa_memcpy(&pCarPlateInfo->cPlateRealTime[1], &tm, 
						sizeof(SWPA_DATETIME_TM));

			pPanoramicImage = pCarPlateInfo->pCarLeft->GetImage(0);
			pDetCarImage = pCarPlateInfo->pCarLeft->GetImage(1, &DetCarFocusPlatePos);
			pPeccancyCarImage = pCarLeft->GetImage(4, &PeccancyCarFocusPlatePos);
			//达到违章，出牌
			/*
			  0首次检到全景图 1首次检到缩放图 2达到违章全景图 3达到违章缩放图
			 */
			BuildCarLeft(pPanoramicImage,
						 pDetCarImage,
						 m_cCurPanoramicInfo.pImage,
						 pPeccancyCarImage,
						 &DetCarFocusPlatePos,
						 &PeccancyCarFocusPlatePos,
						 &pCarPlateInfo->cPlateRealTime[0],
						 &pCarPlateInfo->cPlateRealTime[1],
						 m_cCurPreset.szName,
						 pCarLeft);
			pCarLeft->SetImage(4, NULL);
			pCarPlateInfo->pCarLeft->SetImage(0, NULL);
			pCarPlateInfo->pCarLeft->SetImage(1, NULL);
			pCarPlateInfo->pCarLeft->SetImage(5, NULL);
			pCarPlateInfo->pCarLeft->SetImage(6, NULL);

			//组建第一次检测到的视频
			for(i = 0; i < pCarPlateInfo->iVideoCount; i++)
			{
				pCarLeft->SetVideo(i, pCarPlateInfo->rgpVideo[i]);
				SAFE_RELEASE(pCarPlateInfo->rgpVideo[i]);
				//pCarPlateInfo->rgpVideo[i] = NULL;
			}

			iCurIndex = pCarPlateInfo->iVideoCount;
			
			hr = GetVideo(rgpVideo, &iCurVideoCount);
			if(hr == E_FAIL)
				fIsPeccancy = FALSE;
			//组建第二次检测到的视频
			for(i = 0; i < iCurVideoCount; i++)
			{
				pCarLeft->SetVideo(iCurIndex + i, rgpVideo[i]);
				SAFE_RELEASE(rgpVideo[i]);
			}
			if(fIsPeccancy)
			{
				pCarPlateInfo->iMissCounter = 0;
				pCarPlateInfo->iVideoCount = 0;
				pCarPlateInfo->eDetectStatus = EDET_STATUS_PECCANCY;
				pCarPlateInfo->dwDetectParkingTick = m_cCurPanoramicInfo.dwPanoramicTick;
			}
			else //出错，内存不足之类错误，强制让其删除
			{
				pCarPlateInfo->iMissCounter = 4;
				pCarPlateInfo->iVideoCount = 0;
				pCarPlateInfo->eDetectStatus = EDET_STATUS_PECCANCY;
				pCarPlateInfo->dwDetectParkingTick -= DELETE_PLATE_TIME_OUT;
			}
			
			return fIsPeccancy;
		}
	}

	if(i == dwCount)		//新检到车牌
	{
		if(m_lstCarPlateList[iPresetNum].IsFull())
			return FALSE;

		//DeleteSameCarRectPlateInfo();
		dt.GetTime(&tm);
		CAR_PLATE_INFO *pNewCarPlateInfo = new CAR_PLATE_INFO;
		CSWImage *rgpVideo[30];
		INT iCurVideoCount = 0;
		if(pNewCarPlateInfo == NULL)
		{
			SW_TRACE_DEBUG("ERROR:out of memery.\n");
			return FALSE;
		}
		swpa_memset(pNewCarPlateInfo, 0, sizeof(CAR_PLATE_INFO));

		SW_RECT PlateRect;
		CSWImage *pTmpImage = pCarLeft->GetImage(4, &PlateRect);
		if(pTmpImage != NULL)
		{
			pCarLeft->SetImage(1, pTmpImage, &PlateRect);
			pCarLeft->SetImage(4, NULL);
		}
		
		pCarLeft->SetImage(0, m_cCurPanoramicInfo.pImage);
		pNewCarPlateInfo->dwPanoramicTick[0] = m_cCurPanoramicInfo.dwPanoramicTick;
		pNewCarPlateInfo->cPanoramicRealTime[0] = m_cCurPanoramicInfo.cPanoramicRealTime;
		
		pNewCarPlateInfo->pCarLeft = pCarLeft;
		pNewCarPlateInfo->dwDetectParkingTick = CSWDateTime::GetSystemTick();// m_cCurPanoramicInfo.dwPanoramicTick;
		pNewCarPlateInfo->eDetectStatus = EDET_STATUS_PARKING;
		pNewCarPlateInfo->iMissCounter = 0;
		swpa_memcpy(&pNewCarPlateInfo->cPlateRealTime[0], &tm, sizeof(SWPA_DATETIME_TM));
		swpa_memcpy(&pNewCarPlateInfo->CarRect, &m_CurDetectCarRect, sizeof(HV_RECT));
			
		SAFE_ADDREF(pCarLeft);

		GetVideo(pNewCarPlateInfo->rgpVideo, &pNewCarPlateInfo->iVideoCount); 
		m_lstCarPlateList[iPresetNum].AddTail(pNewCarPlateInfo);

		return FALSE;
	}

	return TRUE;

}

HRESULT CSWRecognizeTransformPPFilter::AllPlateMissIncrement()
{
	INT iPresetNum = m_cCurPreset.iPresetNum;
	SW_POSITION pos = m_lstCarPlateList[iPresetNum].GetHeadPosition();
	DWORD dwCount = m_lstCarPlateList[iPresetNum].GetCount();
	DWORD i = 0;
	for(i = 0; i < dwCount; i++)
	{
		CAR_PLATE_INFO *pCarPlateInfo = (CAR_PLATE_INFO *)m_lstCarPlateList[iPresetNum].GetNext(pos);
		if(pCarPlateInfo != NULL)
		{
			pCarPlateInfo->iMissCounter++;
		}
	}
	return S_OK;
}


HRESULT CSWRecognizeTransformPPFilter::DeleteMissPlate()
{
	SW_TRACE_DEBUG("--------------------INFO: delete miss plate.");
	BOOL fIsFinish = FALSE;
	INT iPresetNum = m_cCurPreset.iPresetNum;
	const INT iTimeOut = DELETE_PLATE_TIME_OUT;
	DWORD dwCurTick = CSWDateTime::GetSystemTick();
	while(!fIsFinish)
	{
		SW_POSITION pos = m_lstCarPlateList[iPresetNum].GetHeadPosition();
		DWORD dwCount = m_lstCarPlateList[iPresetNum].GetCount();
		DWORD i = 0;
		for(i = 0; i < dwCount; i++)
		{
			SW_POSITION delPos = pos;
			CAR_PLATE_INFO *pCarPlateInfo = (CAR_PLATE_INFO *)m_lstCarPlateList[iPresetNum].GetNext(pos);
			if(pCarPlateInfo == NULL)
			{
				continue;
			}

			//delete this
/*
			if(pCarPlateInfo->iVideoCount > 0)
			{
				INT tmpSum = 0;
				for(INT n = 0; n < pCarPlateInfo->iVideoCount; n++)
				{
					tmpSum += pCarPlateInfo->rgiVideoSize[n];
				}
				SW_TRACE_DEBUG("INFO: Car plate video size: %d KB", tmpSum / 1024);
			}
*/
			//----
			

			if((pCarPlateInfo->iMissCounter > DELETE_CARPLATE_MISS_THRESHOLD) &&
			   ((dwCurTick - pCarPlateInfo->dwDetectParkingTick) >= iTimeOut))
			{
				SW_TRACE_DEBUG("INFO: plate %s will be delete.\n",
							   (LPCSTR)pCarPlateInfo->pCarLeft->GetPlateNo());

				ReleaseCarPlateInfo(pCarPlateInfo);
				pCarPlateInfo = NULL;
				m_lstCarPlateList[iPresetNum].RemoveAt(delPos);
				break;
			}
		}
		if(i == dwCount)
		{
			SW_TRACE_DEBUG("INFO:Current Car Plate:%d\n", dwCount);
			fIsFinish = TRUE;
		}
	}

	return S_OK;
}


HRESULT CSWRecognizeTransformPPFilter::GetMaxConfidenceCarLeft(CSWCarLeft **ppCarLeft)
{
	if(ppCarLeft == NULL)
	{
		SW_TRACE_DEBUG("Invalid argument.\n");
		return E_FAIL;
	}
	*ppCarLeft = NULL;
	INT iCenterIndex = 0;
	BOOL fIsMovingCar = FALSE;
	CSWCarLeft *pCurCarLeft = NULL;
	HV_RECT CurCarLeftRect;
	HV_RECT CarLeftRect;
	HV_RECT TmpRect[2];
	SW_POSITION pos = m_lstCarLeftList.GetHeadPosition();
	DWORD dwCount = m_lstCarLeftList.GetCount();

	swpa_memset(TmpRect, 0, sizeof(HV_RECT) * 2);
	for(DWORD i = 0; i < dwCount; i++)
	{
		pCurCarLeft = (CSWCarLeft *)m_lstCarLeftList.GetNext(pos);
		if(pCurCarLeft == NULL)
		{
			continue;
		}
		SW_TRACE_DEBUG("----INFO: Configdence=%f\n", pCurCarLeft->GetAverageConfidence());
		SW_TRACE_DEBUG("----INFO: --- plate brightness : %d\n", (INT)(pCurCarLeft->GetCarAvgY()));
		if((*ppCarLeft) == NULL)
		{
			if(!fIsMovingCar)
			{
				*ppCarLeft = pCurCarLeft;
			}
			continue;
		}

		pCurCarLeft->GetImage(4, (SW_RECT *)&CurCarLeftRect);
		(*ppCarLeft)->GetImage(4, (SW_RECT *)&CarLeftRect);
		swpa_memcpy(&TmpRect[0], &CurCarLeftRect, sizeof(HV_RECT));
		swpa_memcpy(&TmpRect[1], &CarLeftRect, sizeof(HV_RECT));



		if(0 == GetCenterPlate(TmpRect, 2))		//当前车牌比较居中
		{
			if(IsSimilarityPlate(pCurCarLeft, &CurCarLeftRect,  //是否相似
								 (*ppCarLeft), &CarLeftRect))
			{
				if(IsMovingPlate(pCurCarLeft, &CurCarLeftRect,  //是否移动
								 (*ppCarLeft), &CarLeftRect))
				{
					*ppCarLeft = NULL;
					fIsMovingCar = TRUE;
					return S_OK;
				}

				//同一位置的车牌选取亮度合适的车牌
				if((100 <= pCurCarLeft->GetCarAvgY()) && 
				   (pCurCarLeft->GetCarAvgY() <= m_cDomeCameraCtrl.GetMaxPlateAvgY()))
				{
					if((100 <= (*ppCarLeft)->GetCarAvgY()) && 
					   ((*ppCarLeft)->GetCarAvgY() <= m_cDomeCameraCtrl.GetMaxPlateAvgY()))
					{
						//亮度合适的车牌选取可信度最高的车牌
						if((*ppCarLeft)->GetAverageConfidence() <= pCurCarLeft->GetAverageConfidence())
						{
							*ppCarLeft = pCurCarLeft;
							continue;
						}
					}
					else
					{
						*ppCarLeft = pCurCarLeft;
						continue;
					}
				}
				continue;			//相似车牌以上条件不符合就不替换
			}
			*ppCarLeft = pCurCarLeft;
		}
	}  // for end

	return S_OK;
}

HRESULT CSWRecognizeTransformPPFilter::ReleaseAllCarLeft()
{
	while(!m_lstCarLeftList.IsEmpty())
	{
		CSWCarLeft *pCurCarLeft = (CSWCarLeft *)m_lstCarLeftList.RemoveHead();
		SAFE_RELEASE(pCurCarLeft);
	}
	m_lstCarLeftList.RemoveAll();
	return S_OK;
}

HRESULT CSWRecognizeTransformPPFilter::DeleteAllPlate()
{
	BOOL fIsFinish = FALSE;
	INT iPresetNum = 0;
	DWORD dwCurTick = CSWDateTime::GetSystemTick();
	for(INT n = 0; n < 256; n++)
	{
		iPresetNum = n;
		SW_POSITION pos = m_lstCarPlateList[iPresetNum].GetHeadPosition();
		DWORD dwCount = m_lstCarPlateList[iPresetNum].GetCount();
		DWORD i = 0;
		for(i = 0; i < dwCount; i++)
		{
			CAR_PLATE_INFO *pCarPlateInfo = (CAR_PLATE_INFO *)m_lstCarPlateList[iPresetNum].GetNext(pos);
			if(pCarPlateInfo == NULL)
			{
				continue;
			}

			ReleaseCarPlateInfo(pCarPlateInfo);
			pCarPlateInfo = NULL;
		}
		m_lstCarPlateList[iPresetNum].RemoveAll();
		
	}


	return S_OK;
}

HRESULT CSWRecognizeTransformPPFilter::GB28181Alarm(LPCSTR szMsg)
{
    GB28181_AlarmInfo info;
    swpa_strcpy(info.szAlarmMsg,szMsg);
    SWPA_DATETIME_TM tm;
    CSWDateTime dt;
    dt.GetTime(&tm);
    swpa_sprintf(info.szAlarmTime,"%d-%02d-%02dT%02d:%02d:%02d.%03d",tm.year,tm.month,tm.day,tm.hour,tm.min,tm.sec,tm.msec);
    SW_TRACE_NORMAL("CSWRecognizeTransformPPFilter::OnSendAlarm %s %s %d ",info.szAlarmTime,info.szAlarmMsg,MSG_GB28181_SEND_ALARM);
    SendRemoteMessage(MSG_GB28181_SEND_ALARM,&info,sizeof(info));
    return S_OK;
}

HRESULT CSWRecognizeTransformPPFilter::GetPanoramicImage(CSWImage *pImage)
{
	if(pImage != NULL)
	{
		SWPA_DATETIME_TM tm;
		CSWDateTime dt;
		dt.GetTime(&tm);

		CSWPosImage *pPosImage = new CSWPosImage(pImage, 0, FALSE);
		if(S_OK != CSWMessage::SendMessage(MSG_OVERLAY_DOPROCESS, (WPARAM)pPosImage, 0))
		{
			SW_TRACE_DEBUG("ERROR: do process overlay fail.\n");
			pPosImage->Release();
			return E_FAIL;
		}

		if(S_OK != CSWMessage::SendMessage(MSG_JPEGENCODE, (WPARAM)&pImage, NULL))
		{
			SW_TRACE_DEBUG("ERROR: encode jpeg fail.\n");
			pPosImage->Release();
			return E_FAIL;
		}


		if(m_cCurPanoramicInfo.pImage != NULL)
		{
			SAFE_RELEASE(m_cCurPanoramicInfo.pImage);
		}
		m_cCurPanoramicInfo.pImage = pImage;
		m_cCurPanoramicInfo.dwPanoramicTick = CSWDateTime::GetSystemTick();
		swpa_memcpy(&m_cCurPanoramicInfo.cPanoramicRealTime, &tm,
				sizeof(SWPA_DATETIME_TM));
		pPosImage->Release();
		return S_OK;
	}

	return E_FAIL;
}

HRESULT CSWRecognizeTransformPPFilter::SetDomeCameraZoomSpeed(INT iSpeed)
{
	if(swpa_camera_basicparam_set_zoom_speed(ZOOM_TELE, iSpeed))
		return E_FAIL;

	return S_OK;
}


/**
  @brief 判断当前位置是否在detArea中
  @param [IN] pRect 矩形区域
  @param [IN] detArea 检测区域,结构体里包含有效检测区域
  @retval 
  TRUE - 在检测区域内
  FALSE - 不在检测区域内
*/
BOOL CSWRecognizeTransformPPFilter::IsInArea( HV_RECT *pRect, DETAREA *detArea)
{
    int icenterX = 0;
    int icenterY = 0;
	INT iLeft = pRect->left;
	INT iRight = pRect->right;
	INT iTop = pRect->top;
	INT iBottom = pRect->bottom;

	icenterX = (iRight-iLeft)/2+iLeft;
	icenterY = (iBottom-iTop)/2+iTop;

	Polygon_result.Removeall();

	for( int j = detArea->iValidPointCount - 1; j >= 0; j-- )
	{
		Point3D Point_3D;
		Point_3D.m_x = detArea->iValidArea_x[j];
		Point_3D.m_y = detArea->iValidArea_y[j];
		Point_3D.m_z = 0;
		Polygon_result.add_element(Point_3D);
	}

	Point3D  curPos;
	curPos.m_x = icenterX;
	curPos.m_y = icenterY;
	curPos.m_z = 0;

	int ret = Polygon_result.HasInnerPoint(curPos);
	if( ret == 1 )
	{
		return true;
	}

    return false;
}



/**
   @brief 两个区域是否重叠
   @param [IN] pCRect0 区域1
   @param [IN] pCRect1 区域2
   @retval TRUE - 重叠 FALSE - 不重叠
*/
BOOL CSWRecognizeTransformPPFilter::IsOverlap(HV_RECT *pCRect0, HV_RECT *pCRect1)
{
    int nMaxLeft = 0;
    int nMaxTop = 0;
    int nMinRight = 0;
    int nMinBottom = 0;
    int nFlag = 0;
	INT Rect0Width = pCRect0->right - pCRect0->left;
	INT Rect0Height = pCRect0->bottom - pCRect0->top;
	INT Rect1Width = pCRect1->right - pCRect1->left;
	INT Rect1Height = pCRect1->bottom - pCRect1->top;
    // Get the max left.
	nMaxLeft = pCRect0->left >= pCRect1->left ? pCRect0->left : pCRect1->left;
    // Get the max top.
	nMaxTop = pCRect0->top >= pCRect1->top ? pCRect0->top : pCRect1->top;
    // Get the min right.
	nMinRight = pCRect0->right <= pCRect0->right ? pCRect0->right : pCRect1->right;
    // Get the min bottom.
	nMinBottom = pCRect0->bottom <= pCRect1->bottom ? pCRect0->bottom:pCRect1->bottom;
    if ((nMaxLeft > nMinRight) || (nMaxTop > nMinBottom))
    {
        //不相交
        nFlag=0;
        return false;
    }
    else
    {
        //相交 FIXME
        //int tempSame = m_cModuleParams.cIllegalparkingCfgParam.iSameDeviation;
		int tempSame = 1;
        float fltSameScale = (float)tempSame/100.0f;

        int area = 0;
        area = (nMinRight - nMaxLeft   + 1 ) * (nMinBottom - nMaxTop + 1);
        int iarea0 = (Rect0Width+1) * (Rect0Height+1);
        int iarea1 = (Rect1Width+1) * (Rect1Height+1);
        if(area > (int)((float)iarea0*fltSameScale)
                || area > (int)((float)iarea1*fltSameScale)
          )
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    return false;
}

/**
   @brief 检测是否是相同区域
   @param [IN] pCRect0 区域1
   @param [IN] pCRect1 区域2
   @retval TRUE - 相同 FALSE - 不相同
*/
BOOL CSWRecognizeTransformPPFilter::IsSameRect( HV_RECT *pCRect0, HV_RECT *pCRect1, INT *pFlag)
{
	//FIXME:
    int tempSame = 70;//m_cModuleParams.cIllegalparkingCfgParam.iSameDeviation;
    float fltSameScale = (float)tempSame/100.0f;

    int nMaxLeft = 0;
    int nMaxTop = 0;
    int nMinRight = 0;
    int nMinBottom = 0;
    int nFlag = 0;
	INT Rect0Width = pCRect0->right - pCRect0->left;
	INT Rect0Height = pCRect0->bottom - pCRect0->top;
	INT Rect1Width = pCRect1->right - pCRect1->left;
	INT Rect1Height = pCRect1->bottom - pCRect1->top;
    // Get the max left.
	nMaxLeft = pCRect0->left >= pCRect1->left ? pCRect0->left : pCRect1->left;

    // // Get the max top.
	nMaxTop = pCRect0->top >= pCRect1->top ? pCRect0->top : pCRect1->top;


    // // Get the min right.
	nMinRight = pCRect0->right <= pCRect1->right ? pCRect0->right : pCRect1->right;


    // // Get the min bottom.
	nMinBottom = pCRect0->bottom <= pCRect1->bottom ? pCRect0->bottom:pCRect1->bottom;

    if ((nMaxLeft > nMinRight) || (nMaxTop > nMinBottom))
    {
        //不相交
        nFlag=0;
        return false;
    }
    else
    {
        //相交
        //B1:相交为一个区域
        int area = 0;
        nFlag = 1;
        area = (nMinRight - nMaxLeft   + 1 ) * (nMinBottom - nMaxTop + 1);


        if ((pCRect0->left == pCRect1->left) && 
			(pCRect0->right == pCRect1->right) && 
			(pCRect0->top == pCRect1->top) && 
			(pCRect0->bottom == pCRect1->bottom))
        {
            //B13:完全重合
            *pFlag = 0;
            return true;
        }
        else if ( ((nMaxLeft == pCRect0->left) && 
				  (nMinRight == pCRect0->right) && 
				  (nMaxTop == pCRect0->top) && 
				   (nMinBottom == pCRect0->bottom)))
		{
			*pFlag = 1;
            return true;
		}
		else if( ((nMaxLeft == pCRect1->left) && 
				  (nMinRight == pCRect1->right) && 
				   (nMaxTop == pCRect1->top) && 
				   (nMinBottom == pCRect1->bottom)) )
        {
            //B12:包含
            *pFlag = 0;
            return true;
        }
        else if ((nMaxLeft == nMinRight) && (nMaxTop == nMinBottom))
        {
            //B2:交点为1个点
            *pFlag = 0;
            return false;
        }
        else if (((nMaxLeft == nMinRight) && (nMaxTop < nMinBottom))
                 || ((nMaxLeft < nMinRight) && (nMaxTop == nMinBottom)))
        {
            //B3:交点为1条线段
            *pFlag = 0;
            return false;
        }

        int iarea0 = (Rect0Width+1) * (Rect0Height+1);
        int iarea1 = (Rect1Width+1) * (Rect1Height+1);
        if(area > (int)((float)iarea0*fltSameScale))
		{
			SW_TRACE_DEBUG("INFO:area0 left%d right%d top%d bottom%d\n",
						   pCRect0->left, pCRect0->right, pCRect0->top, pCRect0->bottom);
			SW_TRACE_DEBUG("INFO:*****SameScale area0=%f area1=%f\n", 
						   (float)area/(float)iarea0,(float)area/(float)iarea1);
			*pFlag = 0;
            return true;
		}
		else if(area > (int)((float)iarea1*fltSameScale)
          )
        {
			SW_TRACE_DEBUG("INFO:area1 left%d right%d top%d bottom%d\n",
						   pCRect1->left, pCRect1->right, pCRect1->top, pCRect1->bottom);
			SW_TRACE_DEBUG("INFO:*****SameScale area0=%f area1=%f\n", 
						   (float)area/(float)iarea0,(float)area/(float)iarea1);
			*pFlag = 1;
            return true;
        }
        else
        {
            return false;
        }
    }

    return false;
}


HRESULT CSWRecognizeTransformPPFilter::OnPresetPosRecognizeStart(WPARAM wParam, 
																LPARAM lParam)
{
	INT iPresetNum = (INT)wParam;
	m_cDomeCameraCtrl.DomeCameraCruisePause();
	SW_TRACE_DEBUG("INFO: OnPresetPosRecognizeStart preset %d.\n", iPresetNum);
	
	m_cCurPreset.iPresetNum = iPresetNum;
//	m_cDomeCameraCtrl.GetDomeCameraPresetPTZ(&m_cCurPreset.fltPan, 
//											 &m_cCurPreset.fltTilt, 
//											 &m_cCurPreset.fltZoom);
	m_cDomeCameraCtrl.GetDomeCameraPresetName(iPresetNum, m_cCurPreset.szName, 32);
	m_dwGetDetectSignalTick 	= CSWDateTime::GetSystemTick();
	m_iCurDetectCarCounter 		= 0;
	m_iCurDetectPlateCounter 	= 0;
	m_iCurLosePlateCounter 		= 0;
	m_dwRecordVideoStartTick 	= 0;
	m_dwRecordVideoEndTick 		= 0;
	m_iCarDebugCount 			= 0;
	m_iPlateDebugCount			= 0;
	m_iRecognizeRunFlag 		= RECOGNIZE_RUNNING;
	m_fIsPresetPosition 		= FALSE;
	swpa_memset(m_rgcCarDebugRect, 0, 32 * sizeof(HV_RECT));	
	m_iCurProcessStatus = EPROCESS_STATUS_CAR_DETECTING;
	return S_OK;
}

HRESULT CSWRecognizeTransformPPFilter::OnPresetPosRecognizeStop(WPARAM wParam, 
																LPARAM lParam)
{
	m_iRecognizeRunFlag = RECOGNIZE_STOP;
	SW_TRACE_DEBUG("INFO: OnPresetPosRecognizeStop.\n");
	return S_OK;
}

HRESULT CSWRecognizeTransformPPFilter::OnModifyPeccancyParkingParam(WPARAM wParam,
																	LPARAM lParam)
{
	m_pMatchParam = (MATCHPARAM *)wParam;
	BOOL fUseLedLight = TRUE;
	if(m_pMatchParam->cPeccancyParkingParam.iNightUseLedLight)
		fUseLedLight = TRUE;
	else
		fUseLedLight = FALSE;
	m_cDomeCameraCtrl.SetNightUseLedLight(fUseLedLight);
	return S_OK;
}

HRESULT CSWRecognizeTransformPPFilter::CalcBlowUpBox(HV_RECT *pSrcRect,
													HV_RECT *pDstRect)
{
	if(pSrcRect == NULL || pDstRect == NULL)
	{
		SW_TRACE_DEBUG("ERROR: Invalid argument.\n");
		return E_FAIL;
	}
	
	CPoint tlp, brp;
	INT iSrcWidth = pSrcRect->right - pSrcRect->left;
	INT iDstWidth = iSrcWidth * 2;
	INT iDstHeight = (INT)((FLOAT)iDstWidth * 1080 / 1920);
    int imageWidth = 1920;
    int imageHeight = 1080;

	tlp.x = pSrcRect->left;
	tlp.y = pSrcRect->top;
	brp.x = pSrcRect->right;
	brp.y = pSrcRect->bottom;
	INT iSrcCenterX = (tlp.x + (brp.x - tlp.x)/2);
	INT iSrcCenterY = (tlp.y + (brp.y - tlp.y)/2);

	pDstRect->left = iSrcCenterX - (iDstWidth / 2);
	pDstRect->top = iSrcCenterY - (iDstHeight / 2);
	pDstRect->right = iSrcCenterX + (iDstWidth / 2);
	pDstRect->bottom = iSrcCenterY + (iDstHeight / 2);
	

	return S_OK;
}

HRESULT CSWRecognizeTransformPPFilter::DomeCameraAdjust(CSWImage* pImage)
{
	if(pImage == NULL)
	{
		SW_TRACE_DEBUG("ERROR: Invalid argument.\n");
		return E_FAIL;		
	}

	DWORD dwCurTick = 0;
	INT iPresetNum = m_cCurPreset.iPresetNum;
	INT iCarCount = m_lstCarBoxList[iPresetNum].GetCount();
	HV_RECT BlowUpBox;

	if(!m_fIsPresetPosition)
	{
		m_fSendPlateRedBox = FALSE;
		m_iPlateDebugCount = 0;
		m_cDomeCameraCtrl.CallDomeCameraPreset(m_cCurPreset.iPresetNum);
		m_fIsPresetPosition = TRUE;
		m_dwLastDomeCameraAdjustTick = CSWDateTime::GetSystemTick();
		//TODO 发送消息录像
	}

	m_fSendCarRedBox = m_fSendDebug ? TRUE : FALSE;
	dwCurTick = CSWDateTime::GetSystemTick();

	//录像开始
	if((m_dwRecordVideoStartTick == 0) &&
	   ((dwCurTick - m_dwLastDomeCameraAdjustTick) > RECORD_VIDEO_BEGIN_TIME))
	{
		
		m_dwRecordVideoStartTick = dwCurTick;
	}
	//等待球机运动完毕及录像时间
	if((dwCurTick - m_dwLastDomeCameraAdjustTick) < DETECT_CARBOX_BEGIN_TIME)
		return S_OK;
	m_fIsPresetPosition = FALSE;
	m_fSendCarRedBox = FALSE;

	CAR_BOX_INFO *pCarDetectInfo = (CAR_BOX_INFO *)m_lstCarBoxList[iPresetNum].GetNext(m_cCurCarPosition);

	m_iCurCarPositionIndex++;
	if((pCarDetectInfo == NULL) || 
	   (m_iCurCarPositionIndex > m_lstCarBoxList[iPresetNum].GetCount()))		//当前预置位链表检测完
	{
		SW_TRACE_DEBUG("INFO: no more car.\n");
		return E_FAIL;
	}

	if( E_FAIL == GetPanoramicImage(pImage))			//获取全景图片
	{
		SW_TRACE_DEBUG("ERROR: Get panoramic image error.\n");
		return E_FAIL;
	}

	m_iCurPeccancyAreaLimitTime = pCarDetectInfo->iLimitTime;


	if(CalcBlowUpBox(&pCarDetectInfo->CarPosition, &BlowUpBox))
	{
		SW_TRACE_DEBUG("ERROR: CalcBlowUpBox fail.\n");
		return E_FAIL;
	}

	m_cDomeCameraCtrl.MoveBlockToCenter(&BlowUpBox);
	swpa_memcpy(&m_cCurCarBoxInfo, pCarDetectInfo, sizeof(CAR_BOX_INFO));
	swpa_memcpy(&m_CurDetectCarRect, &pCarDetectInfo->CarPosition, sizeof(HV_RECT));


	m_iFrameDelayCount = 0;
	m_iCurDetectPlateCounter = 0;
	m_iCurLosePlateCounter = 0;
	//开始车牌检测
 	m_iCurProcessStatus = EPROCESS_STATUS_PLATE_DETECTING;

	return S_OK;
}

INT CSWRecognizeTransformPPFilter::GetCenterPlate(HV_RECT *rgpPlateRect,
													  INT iPlateCount)
{
	if(rgpPlateRect == NULL || iPlateCount < 2)
	{
		SW_TRACE_DEBUG("ERROR: Invalid argument.\n");
		return -1;
	}


	HV_RECT *pCurRect = &rgpPlateRect[0];
	INT iCenterIndex = 0;
	for(INT i = 0; i < iPlateCount - 1; i++)
	{
		HV_RECT *pNextRect = &rgpPlateRect[i + 1];
		INT iCurWidth = pCurRect->right - pCurRect->left;
		INT iCurCenterX = pCurRect->left + iCurWidth / 2;
		INT iNextWidth = pNextRect->right - pNextRect->left;
		INT iNextCenterX = pNextRect->left + iNextWidth / 2;
		INT iCurCenterXDis = abs(iCurCenterX-(BALLIMAGE_W/2));
		INT iNextCenterXDis = abs(iNextCenterX-(BALLIMAGE_W/2));
		if((iCurCenterXDis > iNextCenterXDis) &&
		   (abs(iCurCenterXDis - iNextCenterXDis) > 100))
		{
			pCurRect = &rgpPlateRect[i + 1];
			iCenterIndex = i + 1;
		}
	}

	return iCenterIndex;
}


HRESULT CSWRecognizeTransformPPFilter::DeleteCarLeft(CARLEFT_INFO_STRUCT *pCarLeftInfo)
{
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
		if(pCarLeft == NULL)
			return E_FAIL;
	}
	pCarLeft->SetParameter(m_pTrackerCfg, pCarLeftInfo);

	SAFE_RELEASE(pCarLeft);
	return S_OK;
}

BOOL CSWRecognizeTransformPPFilter::IsBeginDetectPlate()
{
	//等待球机调整完毕, 除去等待视频的3秒，实际这里只等待3秒,然后开始检测车牌
	DWORD dwCurTick = CSWDateTime::GetSystemTick();

	if(IsDay())
	{
		if((m_dwRecordVideoEndTick == 0) &&
		   ((dwCurTick - m_dwLastDomeCameraAdjustTick) >= DAY_RECORD_VIDEO_END_TIME))
		{
			m_dwRecordVideoEndTick = dwCurTick;
		}
		if(((dwCurTick - m_dwLastDomeCameraAdjustTick) < DAY_DETECT_CARPLATE_BEGIN_TIME))
		{
			return FALSE;
		}

	}
	else
	{
		if((m_dwRecordVideoEndTick == 0) &&
		   ((dwCurTick - m_dwLastDomeCameraAdjustTick) >= NIGHT_RECORD_VIDEO_END_TIME))
		{
			m_dwRecordVideoEndTick = dwCurTick;
		}

		if((!m_cDomeCameraCtrl.IsNightDetectPlateConfig()) && 
		   (dwCurTick - m_dwLastDomeCameraAdjustTick) >= DOME_MOVE_BLOCK_DONE_TIME)
		{
			m_cDomeCameraCtrl.SetDetectPlateConfig();
		}

		if((dwCurTick-m_dwLastDomeCameraAdjustTick) >= DOME_CAMERA_DO_ONE_FOCUS_TIME)
		{
			if(!m_cDomeCameraCtrl.IsFocused())
			{
				m_cDomeCameraCtrl.DoOneFocus();
			}
		}
		if(((dwCurTick - m_dwLastDomeCameraAdjustTick) < NIGHT_DETECT_CARPLATE_BEGIN_TIME))
		{
			return FALSE;
		}
	}
	return TRUE;
}


HRESULT CSWRecognizeTransformPPFilter::DetectPlate(CSWImage* pImage,
												   PROCESS_EVENT_STRUCT *pProcessEvent)
												  
{
	if(pImage == NULL || pProcessEvent == NULL)
	{
		SW_TRACE_DEBUG("ERROR: Invalid argument.\n");
		return E_FAIL;
	}

	//是否开始检测车牌
	if(!IsBeginDetectPlate())
	{
		return S_OK;
	}

	//通过帧数进行延时
	if(m_iFrameDelayCount > 0)
	{
		m_iFrameDelayCount--;
		return S_OK;
	}

	//检牌计数
	m_iCurDetectPlateCounter++;

//	if((dwCurTick - m_dwLastDomeCameraAdjustTick) > DETECT_CARPLATE_END_TIME)
	if(m_iCurDetectPlateCounter > DETECT_PLATE_IMAGE_MAX_NUM)
	{
		if(m_lstCarLeftList.GetCount() > 0)
		{
			CSWCarLeft *pCarLeft = NULL;
			BOOL fIsPeccancy = FALSE;
			GetMaxConfidenceCarLeft(&pCarLeft);
	
			if(pCarLeft != NULL)
			{
				fIsPeccancy = IsPeccancyParking(pCarLeft, pImage);	
				if(fIsPeccancy)
				{
					GetOut(1)->Deliver(pCarLeft);
				}
			}
		}

		m_dwRecordVideoStartTick = 0;
		m_dwRecordVideoEndTick = 0;
		m_fIsPresetPosition = FALSE;
		m_cDomeCameraCtrl.RestoreConfig();
		SAFE_RELEASE(m_cCurPanoramicInfo.pImage);
		ReleaseAllCarLeft();
		m_iCurProcessStatus = EPROCESS_STATUS_DOME_ADJUSTING;
		SW_TRACE_DEBUG("INFO: detect car plate timeout.\n");
		return S_OK;
	}

	FRAME_RECOGNIZE_PARAM cFrameRecogParam;
	InitPlateDetectParam(&cFrameRecogParam);

	HRESULT hr = S_OK;
	HRESULT CarLefthr = S_OK;

	DWORD dwProcessStartTick = CSWDateTime::GetSystemTick();
	//强制设置为抓拍图，识别完后恢复
	pImage->SetCaptureFlag(TRUE);
	hr = CSWBaseLinkCtrl::GetInstance()->ProcessOneFrame(pImage, pProcessEvent, 
														&cFrameRecogParam);
	pImage->SetCaptureFlag(FALSE);
	DWORD dwProcessEndTick = CSWDateTime::GetSystemTick();
	if((dwProcessEndTick - dwProcessStartTick) >= 2000)
	{
		SW_TRACE_DEBUG("INFO:ProcessOneFrame spend time %d\n",
					   dwProcessEndTick - dwProcessStartTick);
	}

	if(S_OK == hr)
    {
		if ( pProcessEvent->dwEventId & EVENT_CARARRIVE )
		{
			for( int i = 0; i < pProcessEvent->iCarArriveInfoCount; ++i )
			{
				CarArriveEvent(&pProcessEvent->rgCarArriveInfo[i]);
			}
		}
		if( pProcessEvent->dwEventId & EVENT_CARLEFT )
		{
			//jpeg 字符叠加
			CSWPosImage *pPosImage = new CSWPosImage(pImage, 0, FALSE);
			if(S_OK != CSWMessage::SendMessage(MSG_OVERLAY_DOPROCESS, (WPARAM)pPosImage, 0))
			{
				SW_TRACE_DEBUG("ERROR: do process overlay fail.\n");
				pPosImage->Release();
				m_cDomeCameraCtrl.RestoreConfig();
				return E_FAIL;
			}
			// jpeg 压缩
			if(S_OK != CSWMessage::SendMessage(MSG_JPEGENCODE, (WPARAM)&pImage, NULL))
			{
				SW_TRACE_DEBUG("ERROR: encode jpeg fail.\n");
				pPosImage->Release();
				m_cDomeCameraCtrl.RestoreConfig();

				return E_FAIL;
			}

			//选出最靠近X轴中心的车牌
			if(pProcessEvent->iCarLeftInfoCount > 1)
			{
				INT iCenterIndex = 0;
				HV_RECT TmpRect[30];
				swpa_memset(TmpRect, 0, sizeof(HV_RECT) * 30);
				for(INT i = 0; i < pProcessEvent->iCarLeftInfoCount; i++)
				{
					swpa_memcpy(&TmpRect[i], &pProcessEvent->rgCarLeftInfo[i].cCoreResult.rcBestPlatePos, sizeof(HV_RECT));
				}
				iCenterIndex = GetCenterPlate(TmpRect, 
											  pProcessEvent->iCarLeftInfoCount);
				CarLefthr = CarLeftEvent(&pProcessEvent->rgCarLeftInfo[iCenterIndex],pImage);
				for(INT i = 0; i < pProcessEvent->iCarLeftInfoCount; i++)
				{
					if(i != iCenterIndex)
						DeleteCarLeft(&pProcessEvent->rgCarLeftInfo[i]);
				}
			}
			else if(pProcessEvent->iCarLeftInfoCount == 1)
			{
				CarLefthr = CarLeftEvent(&pProcessEvent->rgCarLeftInfo[0], pImage);
			}

			//设置车牌红框
			if(pProcessEvent->iCarLeftInfoCount > 0 && m_fSendDebug)
			{
				for(INT i = 0; i < pProcessEvent->iCarLeftInfoCount; i++)
				{
					m_rgcPlateDebugRect[i].left = pProcessEvent->rgCarLeftInfo[i].cCoreResult.rcBestPlatePos.left;
					m_rgcPlateDebugRect[i].top = pProcessEvent->rgCarLeftInfo[i].cCoreResult.rcBestPlatePos.top * 2;
					m_rgcPlateDebugRect[i].right = pProcessEvent->rgCarLeftInfo[i].cCoreResult.rcBestPlatePos.right;
					m_rgcPlateDebugRect[i].bottom = pProcessEvent->rgCarLeftInfo[i].cCoreResult.rcBestPlatePos.bottom * 2;
				}
				m_iPlateDebugCount = pProcessEvent->iCarLeftInfoCount;
				m_fSendPlateRedBox = TRUE;
			}


			SAFE_RELEASE(pPosImage);
			SAFE_RELEASE(pImage);

			if(m_lstCarLeftList.GetCount() < DETECT_CARPLATE_NUM)		//达到检牌次数才返回
			{
				SW_TRACE_DEBUG("INFO:carleft count=%d\n", m_lstCarLeftList.GetCount());
				return S_OK;
			}

			ReleaseAllCarLeft();
			if(CarLefthr != S_OK)
			{
				SW_TRACE_DEBUG("------ERROR: CarLeftEvent");
				return E_FAIL;
			}

			SAFE_RELEASE(m_cCurPanoramicInfo.pImage);
			m_dwRecordVideoStartTick = 0;
			m_dwRecordVideoEndTick = 0;
			if(m_cCurCarBoxInfo.iDetectAreaNum < DETECT_AREA_COUNT_MAX)
			{
				INT iDetectAreaNum = m_cCurCarBoxInfo.iDetectAreaNum;
				INT iWidth = m_cCurCarBoxInfo.CarPosition.right - m_cCurCarBoxInfo.CarPosition.left;
				INT iCount = m_rgcCarWidthInfo[iDetectAreaNum].iDetectPlateCount; 
				if(iCount < 32)
					m_rgcCarWidthInfo[iDetectAreaNum].iDetectPlateWidth[iCount] += iWidth;
				m_rgcCarWidthInfo[iDetectAreaNum].iDetectPlateCount++;

			}
			m_cDomeCameraCtrl.RestoreConfig();

			m_iCurProcessStatus = EPROCESS_STATUS_DOME_ADJUSTING;
		}

		if(!(pProcessEvent->dwEventId & EVENT_CARLEFT))
		{
			m_iCurLosePlateCounter++;
		}

		//当晚上车牌太爆检不到的情况, 强制降低亮度
		if((m_iCurLosePlateCounter > 0) && 
		   ((m_iCurLosePlateCounter % 4) == 0) &&
		   (!(pProcessEvent->dwEventId & EVENT_CARLEFT)))
		{
			SW_TRACE_DEBUG("INFO: set detect plate config.");
			m_cDomeCameraCtrl.SetDetectPlateConfig(200);
			m_iFrameDelayCount = 20;   //800 ms  12.5 FPS
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

	return hr;
}

HRESULT CSWRecognizeTransformPPFilter::InitCarDetectParam(
	HV_RECT *pRect,
	INT iCarBoxWidth,
	INT iScaleNum,
	CAR_DETECT_PARAM *pCarDetectParam)
{
	if(pCarDetectParam == NULL || pRect == NULL)
	{
		SW_TRACE_DEBUG("ERROR: Invalid argument.\n");
		return E_FAIL;
	}
	//FIXME 1920 1080
	float fltLeftPercent	= (float)pRect->left / 1920;
	float fltTopPercent		= (float)pRect->top / 1080;
	float fltRightPercent	= (float)pRect->right / 1920;
	float fltBottomPercent  = (float)pRect->bottom / 1080;

	pCarDetectParam->fIsDetectStopBox 		= TRUE;
	pCarDetectParam->icarBoxwidth			= iCarBoxWidth;
	pCarDetectParam->fltCarBoxleft			= fltLeftPercent;
	pCarDetectParam->fltCarBoxTop			= fltTopPercent;
	pCarDetectParam->fltCarBoxright			= fltRightPercent;
	pCarDetectParam->fltCarBoxbottom		= fltBottomPercent;	
	pCarDetectParam->iStepDivX				= 16;
	pCarDetectParam->iStepDivY				= 8;
	pCarDetectParam->iScaleNum				= iScaleNum;
	pCarDetectParam->fltScaleCoef			= 0.8;
	pCarDetectParam->iMergeNum				= 2;
	pCarDetectParam->iMaxROI				= 1000;
	pCarDetectParam->iDetType				= 1;
	pCarDetectParam->iDetNO					= 1;
	pCarDetectParam->fltDetsalc				= 6;
	pCarDetectParam->iDetSelTypeNO			= 1;
	pCarDetectParam->fIsDay					= IsDay();
	//动力视讯灯光较亮，强制使用白天检测模型，避免晚上多检
	// if(m_iCameraModel == SONY_FCBCH6500)
	// 	pCarDetectParam->fIsDay				= IsDay();
	// else
	// 	pCarDetectParam->fIsDay				= TRUE;
	

	return S_OK;
}

HRESULT CSWRecognizeTransformPPFilter::InitPlateDetectParam(FRAME_RECOGNIZE_PARAM *pFrameRecogParam)
{
	pFrameRecogParam->cRecogSnapArea.DetectorAreaLeft = 0;
	pFrameRecogParam->cRecogSnapArea.DetectorAreaRight = 100;
	pFrameRecogParam->cRecogSnapArea.DetectorAreaTop = 0;
	pFrameRecogParam->cRecogSnapArea.DetectorAreaBottom = 100;
	pFrameRecogParam->cRecogSnapArea.nDetectorMinScaleNum = 10;
	pFrameRecogParam->cRecogSnapArea.nDetectorMaxScaleNum = 18;

	return S_OK;
}

HRESULT CSWRecognizeTransformPPFilter::DeteleInvalidCarBox(
	PROCESS_DETECT_INFO *pAlgoDetCarBoxInfo, DETAREA *pDetectArea)
{
	if(pAlgoDetCarBoxInfo == NULL || pDetectArea == NULL)
	{
		SW_TRACE_DEBUG("ERROR: Invalid argument.\n");
		return E_FAIL;
	}

	PROCESS_DETECT_INFO tmpAlgoDetCarBoxInfo;
	INT iTmpCounter = 0;
	memset(&tmpAlgoDetCarBoxInfo, 0, sizeof(PROCESS_DETECT_INFO));

	for(INT i = 0; i < pAlgoDetCarBoxInfo->iDetectCount; i++)
	{
		if(IsInArea(&pAlgoDetCarBoxInfo->rgcDetect[i], pDetectArea))
		{
			memcpy(&tmpAlgoDetCarBoxInfo.rgcDetect[iTmpCounter],
				   &pAlgoDetCarBoxInfo->rgcDetect[i], 
				   sizeof(HV_RECT));
			tmpAlgoDetCarBoxInfo.rgiDetectType[iTmpCounter] = 
									pAlgoDetCarBoxInfo->rgiDetectType[i];
			iTmpCounter++;
			continue;
		}
		SW_TRACE_DEBUG("INFO:Invalid rectangle %d %d %d %d\n",
					   pAlgoDetCarBoxInfo->rgcDetect[i].top,
					   pAlgoDetCarBoxInfo->rgcDetect[i].left,
					   pAlgoDetCarBoxInfo->rgcDetect[i].right,
					   pAlgoDetCarBoxInfo->rgcDetect[i].bottom);
	}
	
	tmpAlgoDetCarBoxInfo.iDetectCount = iTmpCounter;
	memset(pAlgoDetCarBoxInfo, 0, sizeof(PROCESS_DETECT_INFO));
	if(iTmpCounter > 0)
		memcpy(pAlgoDetCarBoxInfo, &tmpAlgoDetCarBoxInfo, sizeof(PROCESS_DETECT_INFO));

	return S_OK;
}


HRESULT CSWRecognizeTransformPPFilter::AllCarBoxSetStatus(
						ENUM_DETECT_STATUS eDetectStatus)
{
	INT iPresetNum = m_cCurPreset.iPresetNum;
	SW_POSITION pos = m_lstCarBoxList[iPresetNum].GetHeadPosition();
	DWORD dwCount = m_lstCarBoxList[iPresetNum].GetCount();
	for(DWORD i = 0; i < dwCount; i++)
	{
		CAR_BOX_INFO *pCarBoxInfo = (CAR_BOX_INFO *)m_lstCarBoxList[iPresetNum].GetNext(pos);
		if(pCarBoxInfo == NULL)
			return S_OK;
		pCarBoxInfo->eDetectStatus = eDetectStatus;
		pCarBoxInfo->iDetectNum = 0;
	}
	m_lstCarBoxList[iPresetNum].RemoveAll();

	return S_OK;
}

HRESULT CSWRecognizeTransformPPFilter::AllCarBoxSetDetectStatus()
{
	INT iPresetNum = m_cCurPreset.iPresetNum;
	SW_POSITION pos = m_lstCarBoxList[iPresetNum].GetHeadPosition();
	DWORD dwCount = m_lstCarBoxList[iPresetNum].GetCount();
	for(DWORD i = 0; i < dwCount; i++)
	{
		CAR_BOX_INFO *pCarBoxInfo = (CAR_BOX_INFO *)m_lstCarBoxList[iPresetNum].GetNext(pos);
		if(pCarBoxInfo == NULL)
			return S_OK;
		pCarBoxInfo->fDetectedCar = FALSE;
	}


	return S_OK;
}


HRESULT CSWRecognizeTransformPPFilter::SetDebugRedBox()
{
	INT iPresetNum = m_cCurPreset.iPresetNum;
	SW_POSITION pos = m_lstCarBoxList[iPresetNum].GetHeadPosition();
	DWORD dwCount = m_lstCarBoxList[iPresetNum].GetCount();
	m_iCarDebugCount = (INT)dwCount;
	for(DWORD i = 0; i < dwCount; i++)
	{
		CAR_BOX_INFO *pCarBoxInfo = (CAR_BOX_INFO *)m_lstCarBoxList[iPresetNum].GetNext(pos);
		if(pCarBoxInfo == NULL)
			return S_OK;
		swpa_memcpy(&m_rgcCarDebugRect[i], &pCarBoxInfo->CarPosition, sizeof(HV_RECT));
	}

	return S_OK;
}


HRESULT CSWRecognizeTransformPPFilter::DeleteMissCarBox()
{
	BOOL fIsFinish = FALSE;
	INT iPresetNum = m_cCurPreset.iPresetNum;
	while(!fIsFinish)
	{
		SW_POSITION pos = m_lstCarBoxList[iPresetNum].GetHeadPosition();
		DWORD dwCount = m_lstCarBoxList[iPresetNum].GetCount();
		DWORD i = 0;
		for(i = 0; i < dwCount; i++)
		{
			SW_POSITION delPos = pos;
			CAR_BOX_INFO *pCarBoxInfo = (CAR_BOX_INFO *)m_lstCarBoxList[iPresetNum].GetNext(pos);
			if(pCarBoxInfo == NULL)
			{
				SW_TRACE_DEBUG("ERROR: Invalid Pointer.\n");
				return E_FAIL;
			}
		
			if(pCarBoxInfo->eDetectStatus == EDET_STATUS_MISSING ||
			   pCarBoxInfo->iDetectNum < ((DETECT_CARBOX_NUM + 1) / 2))
			{
				SW_TRACE_DEBUG("INFO:delete car------------");
				m_lstCarBoxList[iPresetNum].RemoveAt(delPos);
				break;
			}
		}
		if(i == dwCount)
		{
			fIsFinish = TRUE;
		}
	}

	return S_OK;
}

HRESULT CSWRecognizeTransformPPFilter::AddToCarBoxList(
	PROCESS_DETECT_INFO *pAlgoDetCarBoxInfo, INT iLimitTime, INT iDetectAreaNum)
{
	if(pAlgoDetCarBoxInfo == NULL)
	{
		SW_TRACE_DEBUG("Invalid argument!\n");
		return E_FAIL;
	}
	
	BOOL fIsExist = FALSE;
	BOOL fFinish = FALSE;
	BOOL fContinue = FALSE;
	HV_RECT *pRect = NULL;
	INT iPresetNum = m_cCurPreset.iPresetNum;
	INT iFlag = 0;
	AllCarBoxSetDetectStatus();	
		

	for(INT i = 0; i < pAlgoDetCarBoxInfo->iDetectCount; i++)
	{
		fIsExist = FALSE;
		pRect = &pAlgoDetCarBoxInfo->rgcDetect[i];
		SW_POSITION pos = m_lstCarBoxList[iPresetNum].GetHeadPosition();
		DWORD dwCount = m_lstCarBoxList[iPresetNum].GetCount();

		for(DWORD j = 0; j < dwCount; j++)
		{
			CAR_BOX_INFO *pCarBoxInfo = (CAR_BOX_INFO *)m_lstCarBoxList[iPresetNum].GetNext(pos);
			if(IsSameRect(&pCarBoxInfo->CarPosition, pRect, &iFlag))
			{
				HV_RECT TmpRect;

				TmpRect.left = (pCarBoxInfo->CarPosition.left + pRect->left) / 2;
				TmpRect.top = (pCarBoxInfo->CarPosition.top + pRect->top) / 2;
				TmpRect.right = (pCarBoxInfo->CarPosition.right + pRect->right) / 2;
				TmpRect.bottom = (pCarBoxInfo->CarPosition.bottom + pRect->bottom) / 2;

				pCarBoxInfo->eDetectStatus = EDET_STATUS_DETECTING;
				if(!pCarBoxInfo->fDetectedCar)
				{
					pCarBoxInfo->fDetectedCar = TRUE;
					pCarBoxInfo->iDetectNum++;
				}
				pCarBoxInfo->iLimitTime = iLimitTime;
				pCarBoxInfo->iDetectAreaNum  = iDetectAreaNum;
				pCarBoxInfo->fDetectedPlate = FALSE;
//				if(iFlag == 1)
//				{
				swpa_memcpy(&pCarBoxInfo->CarPosition, &TmpRect, sizeof(HV_RECT));
//				}
				fIsExist = TRUE;
				break;
			}
			else
			{
				continue;
			}
		}

		if(!fIsExist)
		{
			SW_TRACE_DEBUG("--------A new car.");
			if(m_lstCarBoxList[iPresetNum].IsFull())
				return E_FAIL;
			if((pRect->right - pRect->left) < 100)
				continue;
			CAR_BOX_INFO *pTmpCarBoxInfo = new CAR_BOX_INFO;
			if(pTmpCarBoxInfo == NULL)
			{
				SW_TRACE_DEBUG("ERROR: out of memery.\n");
				return E_FAIL;
			}
			swpa_memcpy(&pTmpCarBoxInfo->cDomePresetInfo,
				   &m_cCurPreset, sizeof(DOME_PRESET_INFO));
			swpa_memcpy(&pTmpCarBoxInfo->CarPosition, pRect, sizeof(HV_RECT));
			pTmpCarBoxInfo->eDetectStatus	= EDET_STATUS_DETECTING;
			pTmpCarBoxInfo->iDetectType		= pAlgoDetCarBoxInfo->rgiDetectType[i];
			pTmpCarBoxInfo->fDetectedCar = TRUE;
			pTmpCarBoxInfo->iDetectNum		= 1;
			pTmpCarBoxInfo->iLimitTime      = iLimitTime;
			pTmpCarBoxInfo->iDetectAreaNum  = iDetectAreaNum;
			pTmpCarBoxInfo->fDetectedPlate = FALSE;

			m_lstCarBoxList[iPresetNum].AddHead(pTmpCarBoxInfo);
		}
	}

	//删除链表中重叠的车框
	while(!fFinish)
	{
		SW_POSITION pos = m_lstCarBoxList[iPresetNum].GetHeadPosition();
		SW_POSITION nextPos;
		SW_POSITION delPos;
		INT n = 0;
		DWORD dwCount = m_lstCarBoxList[iPresetNum].GetCount();
		fContinue = FALSE;
		if(dwCount < 2)
		{
			break;
		}
		for(n = 0; n < dwCount; n++)
		{
			CAR_BOX_INFO *pCarBoxInfo = (CAR_BOX_INFO *)m_lstCarBoxList[iPresetNum].GetNext(pos); 
			nextPos = pos;
			for(INT m = n + 1; m < dwCount; m++)
			{
				delPos = nextPos;
				CAR_BOX_INFO *pNextCarBoxInfo = (CAR_BOX_INFO *)m_lstCarBoxList[iPresetNum].GetNext(nextPos);
				if(IsSameRect(&pCarBoxInfo->CarPosition, &pNextCarBoxInfo->CarPosition, &iFlag))
				{
					m_lstCarBoxList[iPresetNum].RemoveAt(delPos);
					fContinue = TRUE;
					break;
				}
			}
			if(fContinue)
				break;
		}
		if(n == dwCount)
			fFinish = TRUE;
	}


	return S_OK;
}

HRESULT CSWRecognizeTransformPPFilter::GetCarInDetectArea(CSWImage* pImage)
{
	if(pImage == NULL)
	{
		SW_TRACE_DEBUG("Invalid argument!\n");
		return E_FAIL;
	}

	DETAREA *pDetectArea = new DETAREA;
	if(pDetectArea == NULL)
	{
		SW_TRACE_DEBUG("ERROR: out of memery.\n");
		return E_FAIL;
	}
	CAR_DETECT_PARAM CarDetectParam;
	PROCESS_DETECT_INFO CarBoxInfo;
	HV_RECT cDetectRect;
	PECCANCY_PARKING_AREA *pPeccancyParkingArea;
	SWPA_DATETIME_TM tm;
	CSWDateTime dt;
	INT iDetectMinSide = 0;
	INT i = 0;
	INT iPresetNum = m_cCurPreset.iPresetNum;
	INT iCarBoxWidth = 0, iScaleNum = 0;
	INT iLimitTime = 0;
	INT iDetAreaPosCount = 0;
	m_iCurDetectAreaCount = 0;

	dt.GetTime(&tm);
	memset(&CarDetectParam, 0, sizeof(CAR_DETECT_PARAM));

	for(i = 0; i < DETECT_AREA_COUNT_MAX; i++)
	{
		pPeccancyParkingArea = &m_pMatchParam->cPeccancyParkingParam.cPeccancyParkingArea[i];
		if(pPeccancyParkingArea->iPresetNum != iPresetNum)
			continue;

		if(!IsInPeccancyTime(&tm, pPeccancyParkingArea))
			continue;

		iLimitTime = pPeccancyParkingArea->iLimitTime;


		GetDetectRect(pPeccancyParkingArea->iDetAreaPosCount, 
					  pPeccancyParkingArea->rgiPosX,
					  pPeccancyParkingArea->rgiPosY,
					  &cDetectRect);

		GetDetectRectSide(pPeccancyParkingArea->iDetAreaPosCount,
						  pPeccancyParkingArea->rgiPosX,
						  pPeccancyParkingArea->rgiPosY,
						  &cDetectRect,
						  &iDetectMinSide);

		if(m_rgcCarWidthInfo[i].iCurDetectAreaMinSide != iDetectMinSide)
		{
			m_rgcCarWidthInfo[i].iCurDetectAreaMinSide = iDetectMinSide;
			m_rgcCarWidthInfo[i].iCurMaxCarWidth = iDetectMinSide;
		}
		
		pDetectArea->DetectRect = cDetectRect;
		pDetectArea->iValidPointCount = pPeccancyParkingArea->iDetAreaPosCount;

		if(pPeccancyParkingArea->iDetAreaPosCount < 32)
			iDetAreaPosCount = pPeccancyParkingArea->iDetAreaPosCount;
		else
			iDetAreaPosCount = 32;

		for(INT j = 0; j < iDetAreaPosCount; j++)
		{
			pDetectArea->iValidArea_x[j] = pPeccancyParkingArea->rgiPosX[j];
			pDetectArea->iValidArea_y[j] = pPeccancyParkingArea->rgiPosY[j];
		}

		swpa_memcpy(&m_rgcCurDetectArea[m_iCurDetectAreaCount],
					pDetectArea, sizeof(DETAREA));
		m_iCurDetectAreaCount++;
		swpa_memset(&CarDetectParam, 0, sizeof(CAR_DETECT_PARAM));
		swpa_memset(&CarBoxInfo, 0, sizeof(PROCESS_DETECT_INFO));

		GetCurCarWidthScaleNum(i, &iCarBoxWidth, &iScaleNum);
		SW_TRACE_DEBUG("iCarBoxWidth = %d scaleNum=%d\n",
					   iCarBoxWidth, iScaleNum);
		SW_TRACE_DEBUG("detect area, TopLeft:%d %d width:%d height:%d\n",
					   pDetectArea->DetectRect.top,
					   pDetectArea->DetectRect.left,
					   pDetectArea->DetectRect.right - pDetectArea->DetectRect.left,
					   pDetectArea->DetectRect.bottom -pDetectArea->DetectRect.top);

		InitCarDetectParam(&pDetectArea->DetectRect, iCarBoxWidth, 
						   iScaleNum, &CarDetectParam);



		CSWBaseLinkCtrl::GetInstance()->ProessDetOneFrame(pImage, 
														  &CarDetectParam, 
														  &CarBoxInfo);


		for(INT n = 0; n < CarBoxInfo.iDetectCount; n++)
		{
			SW_TRACE_DEBUG("FFFFF INFO:%d - %d %d %d %d\n",
						   n,
						   CarBoxInfo.rgcDetect[n].left,
						   CarBoxInfo.rgcDetect[n].top,
						   CarBoxInfo.rgcDetect[n].right,
						   CarBoxInfo.rgcDetect[n].bottom);
		}

		DeteleInvalidCarBox(&CarBoxInfo, pDetectArea);
		AddToCarBoxList(&CarBoxInfo, iLimitTime, i);
	}

	delete pDetectArea;
	return S_OK;
}

HRESULT CSWRecognizeTransformPPFilter::GetDetectRect(INT iPointCount,
													 INT *pPointX,
													 INT *pPointY,
													 HV_RECT *pDetectRect)
{
	if(pPointX == NULL || pPointY == NULL || pDetectRect == NULL)
	{
		SW_TRACE_DEBUG("Invalid argument.\n");
		return E_FAIL;
	}
	
	INT iTop, iLeft, iRight, iBottom;
	iLeft = *pPointX;
	iRight = *pPointX;
	iTop = *pPointY;
	iBottom = *pPointY;

	for(INT i = 1; i < iPointCount; i++)
	{
		INT X = *(pPointX + i);
		INT Y = *(pPointY + i);
		if(iLeft > X)
			iLeft = X;
		if(iRight < X)
			iRight = X;
		if(iTop > Y)
			iTop = Y;
		if(iBottom < Y)
			iBottom = Y;
	}

	pDetectRect->left = iLeft;
	pDetectRect->top = iTop;
	pDetectRect->right = iRight;
	pDetectRect->bottom = iBottom;

	return S_OK;
}

HRESULT CSWRecognizeTransformPPFilter::GetDetectRectSide(INT iDetAreaPosCount, 
														 INT *rgiPosX, 
														 INT *rgiPosY, 
														 HV_RECT *pRect,
														 INT *pMinSide)
{
	if(iDetAreaPosCount != 4 || rgiPosX == NULL || rgiPosY == NULL)
	{
		SW_TRACE_DEBUG("ERROR:Invalid argument.\n");
		return E_FAIL;
	}
	INT rgiSide[4] = {0};
	for(INT i = 0; i < iDetAreaPosCount; i++)
	{
		if(i != (iDetAreaPosCount - 1))
		{
			INT iDiffX = abs(rgiPosX[i] - rgiPosX[i + 1]);
			INT iDiffY = abs(rgiPosY[i] - rgiPosY[i + 1]);
			rgiSide[i] = (INT)sqrt(pow(iDiffX , 2) + pow(iDiffY, 2));
		}
		else 
		{
			INT iDiffX = abs(rgiPosX[i] - rgiPosX[0]);
			INT iDiffY = abs(rgiPosY[i] - rgiPosY[0]);
			rgiSide[i] = (INT)sqrt(pow(iDiffX , 2) + pow(iDiffY, 2));
		}
	}
	
	for(INT i = 0; i < iDetAreaPosCount - 1; i++)
	{
		for(INT j = i + 1; j < iDetAreaPosCount; j++)
		{
			if(rgiSide[i] > rgiSide[j])
			{
				INT iTmp = rgiSide[i];
				rgiSide[i] = rgiSide[j];
				rgiSide[j] = iTmp;
			}
		}
	}

	INT iWidth = pRect->right - pRect->left;
	INT iHeight = pRect->bottom - pRect->top;
	INT iMinRectSide = iWidth > iHeight ? iHeight : iWidth;

   
	if(iMinRectSide > rgiSide[1])
	{
		//第二小的边一定会大于车宽
		*pMinSide = rgiSide[1];
	}
	else
	{
		*pMinSide = iMinRectSide;
	}

	return S_OK;
}

HRESULT CSWRecognizeTransformPPFilter::GetCurCarWidthScaleNum(INT iIndex, INT *pCurrentCarWidth, INT *pScaleNum)
{
	if(iIndex >= DETECT_AREA_COUNT_MAX)
	{
		SW_TRACE_DEBUG("ERROR:Invalid index.\n");
		return E_FAIL;
	}
	if(pCurrentCarWidth == NULL || pScaleNum == 0)
	{
		SW_TRACE_DEBUG("ERROR:Invalid argument.\n");
		return E_FAIL;
	}
	const INT MIN_CAR_WIDTH = 100;
	const INT MAX_SCALE_NUM = 10;
	const INT MIN_SCALE_NUM = 3;
	CAR_WIDTH_INFO *pCarWidthInfo = &m_rgcCarWidthInfo[iIndex];
	INT iCurMinSide = pCarWidthInfo->iCurDetectAreaMinSide;
	if(pCarWidthInfo->iCurMaxCarWidth == 0)
		pCarWidthInfo->iCurMaxCarWidth = iCurMinSide;
	if(pCarWidthInfo->iLastCarWidthAverage == 0)
	{
		INT i = MAX_SCALE_NUM;
		//算出当前值到最小值的scalenum
		for(; i > MIN_SCALE_NUM; i--)
		{
			if((powf(0.8, i) * (FLOAT)iCurMinSide) > MIN_CAR_WIDTH)
			{
				i++;
				break;
			}
		}
		*pCurrentCarWidth = iCurMinSide;
		*pScaleNum = i;
	}
	else
	{
		INT i = MAX_SCALE_NUM;
		INT iMinCarWidth = MIN_CAR_WIDTH;
		FLOAT fltFactor = 1.6;
		if(m_iCurDetectCarCounter <= (DETECT_CARBOX_NUM / 2))
		{
			*pCurrentCarWidth = pCarWidthInfo->iCurDetectAreaMinSide;
			INT iTmpCarWidth = *pCurrentCarWidth;
			for(i = MAX_SCALE_NUM; i > MIN_SCALE_NUM; i--)
			{
				if((powf(0.8, (FLOAT)i) * (FLOAT)iTmpCarWidth) > iMinCarWidth)
				{
					i++;
					break;
				}
			}
			*pScaleNum = i;
			return S_OK;
		}

		iMinCarWidth = (INT)((FLOAT)pCarWidthInfo->iLastCarWidthAverage / fltFactor);
		*pCurrentCarWidth = (INT)((FLOAT)pCarWidthInfo->iLastCarWidthAverage * fltFactor);
		if(iMinCarWidth >= pCarWidthInfo->iCurMinCarWidth)
		{
			iMinCarWidth = (INT)((FLOAT)pCarWidthInfo->iCurMinCarWidth / 1.2);
		
		}
		if(iMinCarWidth < MIN_CAR_WIDTH)
			iMinCarWidth = MIN_CAR_WIDTH;

		if(*pCurrentCarWidth <= pCarWidthInfo->iCurMaxCarWidth)
		{
			*pCurrentCarWidth = (INT)((FLOAT)pCarWidthInfo->iCurMaxCarWidth * 1.2);
		}
		if(*pCurrentCarWidth <= iMinCarWidth)
		{
			*pCurrentCarWidth = MIN_CAR_WIDTH * 2;
		}
		INT iTmpCarWidth = *pCurrentCarWidth;

		for(i = MAX_SCALE_NUM; i > MIN_SCALE_NUM; i--)
		{
			if((powf(0.8, (FLOAT)i) * (FLOAT)iTmpCarWidth) > iMinCarWidth)
			{
				i++;
				break;
			}
		}
		*pScaleNum = i;
	}

	return S_OK;
}

HRESULT CSWRecognizeTransformPPFilter::UpdateCarWidth()
{

	INT iPresetNum = m_cCurPreset.iPresetNum;
	PECCANCY_PARKING_AREA *pPeccancyParkingArea;


	for(INT i = 0; i < DETECT_AREA_COUNT_MAX; i++)
	{
		pPeccancyParkingArea = &m_pMatchParam->cPeccancyParkingParam.cPeccancyParkingArea[i];
		if(pPeccancyParkingArea->iPresetNum != iPresetNum)
			continue;

		INT iTmpCount = m_rgcCarWidthInfo[i].iDetectPlateCount;
		INT iTmpSum = 0;

		for(INT j = 0; j < m_rgcCarWidthInfo[i].iDetectPlateCount; j++)
		{
			INT iWidth = m_rgcCarWidthInfo[i].iDetectPlateWidth[j];
			if(j == 0)
			{
				m_rgcCarWidthInfo[i].iCurMinCarWidth = m_rgcCarWidthInfo[i].iDetectPlateWidth[j];
				m_rgcCarWidthInfo[i].iCurMaxCarWidth = m_rgcCarWidthInfo[i].iDetectPlateWidth[j];
			}
			if(m_rgcCarWidthInfo[i].iCurMinCarWidth > iWidth)
			    m_rgcCarWidthInfo[i].iCurMinCarWidth = iWidth;
			else if(m_rgcCarWidthInfo[i].iCurMaxCarWidth < iWidth)
				m_rgcCarWidthInfo[i].iCurMaxCarWidth = iWidth;
			iTmpSum += m_rgcCarWidthInfo[i].iDetectPlateWidth[j];
		}
		if(iTmpSum != 0 && iTmpSum > 0)
			m_rgcCarWidthInfo[i].iLastCarWidthAverage = iTmpSum / iTmpCount;
		else
			m_rgcCarWidthInfo[i].iLastCarWidthAverage = 0;

		m_rgcCarWidthInfo[i].iDetectPlateCount = 0;
		swpa_memset(m_rgcCarWidthInfo[i].iDetectPlateWidth, 0, 32);
	}	

	return S_OK;
}

BOOL CSWRecognizeTransformPPFilter::IsSuitableBrightness(INT iBrightness)
{
	const INT MIN_PLATE_AVG_Y = m_cDomeCameraCtrl.GetMinPlateAvgY();	//车牌最小亮度
	const INT MAX_PLATE_AVG_Y = m_cDomeCameraCtrl.GetMaxPlateAvgY();	//车牌最大亮度
	if(iBrightness < MIN_PLATE_AVG_Y || iBrightness > MAX_PLATE_AVG_Y)
		return FALSE;
	return TRUE;
}

HRESULT CSWRecognizeTransformPPFilter::CalcImageBrightness(CSWImage *pImage,
														DWORD *pBrightness)
{
	if(pImage == NULL)
	{
		SW_TRACE_DEBUG("Invalid argument.\n");
		return E_FAIL;
	}
	BYTE *Y = NULL;
	DWORD dwYSum = 0;
	SW_COMPONENT_IMAGE sComponentImage;
	pImage->GetImage(&sComponentImage);
	Y = (BYTE *)sComponentImage.rgpbData[0];
	DWORD YSize = sComponentImage.rgiStrideWidth[0] * sComponentImage.iHeight;
	if((YSize % 8) == 0)		//此处为了减少判断，提高运算速度
	{
		for(DWORD i = 0; i < YSize; i += 8)
		{
			dwYSum += *(Y + i);
			dwYSum += *(Y + i + 1);
			dwYSum += *(Y + i + 2);
			dwYSum += *(Y + i + 3);
			dwYSum += *(Y + i + 4);
			dwYSum += *(Y + i + 5);
			dwYSum += *(Y + i + 6);
			dwYSum += *(Y + i + 7);
		}
	}
	else
	{
		for(DWORD i = 0; i < YSize; i++)
		{
			dwYSum += *(Y + i);
		}
	}

	if(YSize != 0)
		*pBrightness = dwYSum / YSize;
	else
		*pBrightness = 0;
	
	return S_OK;
}

HRESULT CSWRecognizeTransformPPFilter::CalcPlateBrightness(CSWImage *pImage,
														   HV_RECT *pPlateRect,
														   DWORD *pBrightness)
{
	INT i = 0;
	BYTE *Y = NULL;
	DWORD dwYSum = 0;
	DWORD dwPlateHeight = pPlateRect->bottom - pPlateRect->top;
	DWORD dwPlateWidth = pPlateRect->right - pPlateRect->left;
	
	SW_COMPONENT_IMAGE sComponentImage;
	pImage->GetImage(&sComponentImage);

	for(INT iY = pPlateRect->top; iY < pPlateRect->bottom; iY++)
	{
		INT iXStart = (iY * 1920) + pPlateRect->left;
		for(INT iX = iXStart; iX < pPlateRect->right; iX++)
		{
			Y = (BYTE *)sComponentImage.rgpbData[iX];
			dwYSum += *Y;
		}
	}

	*pBrightness = dwYSum / (dwPlateHeight * dwPlateWidth);
	return S_OK;
}

HRESULT CSWRecognizeTransformPPFilter::DetectDayOrNight(CSWImage *pImage)
{
	//TODO 使用亮度值及其他条件判断白天夜晚
	const INT DETECT_LIGHT_NUM = 20;
	const INT NIGHT_LIGHT_THRESHOLD = 15;	//到达150张夜晚则认为是夜晚
	const INT NIGHT_AGCGAIN_THRESHOLD = 9;  //增益高于此值则认为时夜晚
	const INT NIGHT_IRIS_THRESHOLD = 10;    //光圈高于此值则认为时夜晚
	const INT DAY_AGCGAIN_THRESHOLD = 6;    //低于此值则认为是白天
	const INT DAY_IRIS_THRESHOLD = 7;       //低于此值则认为是白天
	const DWORD dwNightLightValue = 110;	//小于次值是夜晚
	const DWORD dwDayLightValue = 150;
	const INT GET_DOME_INTERVAL = 6;
	static DWORD rgImageLight[DETECT_LIGHT_NUM] = {0};
	static INT iDetectLightCounter = 0;
	static INT iGetDomeInterval = 0;
	static INT iDomeInfoCounter = 0;
//	static INT rgShutter[10] = {0};
	static INT rgIris[10] = {0};
	static INT rgAGCGain[10] = {0};
	INT iAvgIris = 0, iAvgAGCGain = 0;
	INT iSumIris = 0, iSumAGCGain = 0;
	DWORD dwLight = 0;
	DWORD dwNightCounter = 0;
	DWORD dwDayCounter = 0;
	SWPA_DATETIME_TM tm;
	CSWDateTime dt;

	CalcImageBrightness(pImage, &dwLight);
	rgImageLight[iDetectLightCounter] = dwLight;

	iDetectLightCounter++;
	iGetDomeInterval++;
	if(iGetDomeInterval == GET_DOME_INTERVAL)
	{
		INT iTmpIris = 0, iTmpAGCGain = 0;
		m_cDomeCameraCtrl.GetIris(&iTmpIris);
		//m_cDomeCameraCtrl.GetShutter(&iTmpShutter);
		m_cDomeCameraCtrl.GetAGCGain(&iTmpAGCGain);

		//rgShutter[iDomeInfoCounter] = iTmpShutter;
		rgIris[iDomeInfoCounter] = iTmpIris;
		rgAGCGain[iDomeInfoCounter] = iTmpAGCGain;
		SW_TRACE_DEBUG("====INFO: Iris:%d Gain:%d\n",
					   iTmpIris, iTmpAGCGain);
		SW_TRACE_DEBUG("====INFO: light value %d.\n", dwLight);
		iDomeInfoCounter++;
		if(iDomeInfoCounter >= 10)
			iDomeInfoCounter = 0;
		
		iGetDomeInterval = 0;
	}

	if(iDetectLightCounter >= DETECT_LIGHT_NUM)
	{
		dt.GetTime(&tm);
		iDetectLightCounter = 0;
		for(INT i = 0; i < DETECT_LIGHT_NUM; i++)
		{
			if(rgImageLight[i] < dwNightLightValue)
				dwNightCounter++;
			if(rgImageLight[i] > dwDayLightValue)
				dwDayCounter++;
		}
		for(INT i = 0; i < 10; i++)
		{
			//iSumShutter += rgShutter[i];
			iSumIris += rgIris[i];
			iSumAGCGain += rgAGCGain[i];
		}
		//iAvgShutter = iSumShutter / 10;
		iAvgIris = iSumIris / 10;
		iAvgAGCGain = iSumAGCGain / 10;
		
		if((dwNightCounter >= NIGHT_LIGHT_THRESHOLD) &&
		   (iAvgAGCGain >= NIGHT_AGCGAIN_THRESHOLD))
		{
			SW_TRACE_DEBUG("INFO: night %d.\n", dwNightCounter);
			m_fIsDay = FALSE;
		}
		else if((dwNightCounter <= NIGHT_LIGHT_THRESHOLD) &&
				(iAvgAGCGain <= DAY_AGCGAIN_THRESHOLD)/* &&
														 (iAvgIris <= DAY_IRIS_THRESHOLD)*/)
		{
			SW_TRACE_DEBUG("INFO: day %d.\n", dwNightCounter);
			m_fIsDay = TRUE;
		}
		else if(dwDayCounter >= NIGHT_LIGHT_THRESHOLD)
		{
			SW_TRACE_DEBUG("INFO: day %d.\n", dwNightCounter);
			m_fIsDay = TRUE;
		}
	}

	return S_OK;
}

BOOL CSWRecognizeTransformPPFilter::IsDay()
{
	return m_fIsDay;
}

BOOL CSWRecognizeTransformPPFilter::IsSimilarityPlate(CSWCarLeft *pCurCarLeft, 
													  HV_RECT *pCurCarRect,
													  CSWCarLeft *pSaveCarLeft,
													  HV_RECT *pSaveCarRect)
{
	if(pCurCarLeft == NULL || pSaveCarLeft == NULL || pCurCarRect == NULL || pSaveCarRect == NULL)
	{
		SW_TRACE_DEBUG("Invalid argument.\n");
		return FALSE;
	}
	CHAR szCurPlateNo[32] = {0};
	CHAR szSavePlateNo[32] = {0};
	INT iCharOffset = 4;
	INT iSimilarityCounter = 0;
	INT iPlateNoLength = pCurCarLeft->GetPlateNo().Length();
	// INT iCurCarRectCenterX = ((pCurCarRect->left - pCurCarRect->right) / 2) + pCurCarRect->right;
	// INT iCurCarRectCenterY = ((pCurCarRect->bottom - pCurCarRect->top) / 2) + pCurCarRect->top;
	// INT iSaveCarRectCenterX = ((pSaveCarRect->left - pSaveCarRect->right) / 2) + pSaveCarRect->right;
	// INT iSaveCarRectCenterY = ((pSaveCarRect->bottom - pSaveCarRect->top) / 2) + pSaveCarRect->top;

	if(pCurCarLeft->GetPlateNo() == pSaveCarLeft->GetPlateNo())
		return TRUE;

	if(iPlateNoLength >= 32)
	{
		swpa_memcpy(szCurPlateNo, (LPCSTR)pCurCarLeft->GetPlateNo(), 32);
		swpa_memcpy(szSavePlateNo, (LPCSTR)pSaveCarLeft->GetPlateNo(), 32);
	}
	else
	{
		swpa_memcpy(szCurPlateNo, (LPCSTR)pCurCarLeft->GetPlateNo(), iPlateNoLength);
		swpa_memcpy(szSavePlateNo, (LPCSTR)pSaveCarLeft->GetPlateNo(),iPlateNoLength);
	}

	for(INT i = iCharOffset; i < iPlateNoLength; i++)
	{
		if(szCurPlateNo[i] == szSavePlateNo[i])
			iSimilarityCounter++;
	}

	//模糊条件1 相似数字字母大于等于5， 2 车框中心位置偏移小于100像素
	// if((iSimilarityCounter >= 5) && 
	//    (swpa_abs(iCurCarRectCenterX - iSaveCarRectCenterX) <= 100) &&
	//    (swpa_abs(iCurCarRectCenterY - iSaveCarRectCenterY) <= 100))
	if(iSimilarityCounter >= 5)
	{
		return TRUE;
	}

	return FALSE;
}

BOOL CSWRecognizeTransformPPFilter::IsMovingPlate(CSWCarLeft *pCurCarLeft, 
													  HV_RECT *pCurCarRect,
													  CSWCarLeft *pSaveCarLeft,
													  HV_RECT *pSaveCarRect)
{
	if(pCurCarLeft == NULL || pSaveCarLeft == NULL || 
	   pCurCarRect == NULL || pSaveCarRect == NULL)
	{
		SW_TRACE_DEBUG("Invalid argument.\n");
		return FALSE;
	}
	INT iCurCarRectCenterX = ((pCurCarRect->left - pCurCarRect->right) / 2) + pCurCarRect->right;
	INT iCurCarRectCenterY = ((pCurCarRect->bottom - pCurCarRect->top) / 2) + pCurCarRect->top;
	INT iSaveCarRectCenterX = ((pSaveCarRect->left - pSaveCarRect->right) / 2) + pSaveCarRect->right;
	INT iSaveCarRectCenterY = ((pSaveCarRect->bottom - pSaveCarRect->top) / 2) + pSaveCarRect->top;

	//车框中心位置偏移小于100像素
	if((swpa_abs(iCurCarRectCenterX - iSaveCarRectCenterX) <= 120) &&
	   (swpa_abs(iCurCarRectCenterY - iSaveCarRectCenterY) <= 120))
	{
		return FALSE;
	}

	return TRUE;
}

/*
HRESULT CSWRecognizeTransformPPFilter::DeleteSameCarRectPlateInfo()
{
	INT iPresetNum = m_cCurPreset.iPresetNum;
	SW_POSITION pos = m_lstCarPlateList[iPresetNum].GetHeadPosition();
	DWORD dwCount = m_lstCarPlateList[iPresetNum].GetCount();
	DWORD i = 0;

	HV_RECT *pCurCarRect = &m_CurDetectCarRect;


	INT iCurCarRectCenterX = ((pCurCarRect->left - pCurCarRect->right) / 2) + pCurCarRect->right;
	INT iCurCarRectCenterY = ((pCurCarRect->bottom - pCurCarRect->top) / 2) + pCurCarRect->top;


	for(i = 0; i < dwCount; i++)
	{
		CAR_PLATE_INFO *pCarPlateInfo = NULL;
		SW_POSITION curPos = pos;
		pCarPlateInfo = (CAR_PLATE_INFO *)m_lstCarPlateList[iPresetNum].GetNext(pos);
		if(pCarPlateInfo == NULL)
			return FALSE;
		HV_RECT *pSaveCarRect = &pCarPlateInfo->CarRect;
		INT iSaveCarRectCenterX = ((pSaveCarRect->left - pSaveCarRect->right) / 2) + pSaveCarRect->right;
		INT iSaveCarRectCenterY = ((pSaveCarRect->bottom - pSaveCarRect->top) / 2) + pSaveCarRect->top;

		if(((swpa_abs(iCurCarRectCenterX - iSaveCarRectCenterX) <= 100) &&
			(swpa_abs(iCurCarRectCenterY - iSaveCarRectCenterY) <= 100)))
		{
			ReleaseCarPlateInfo(pCarPlateInfo);
			pCarPlateInfo = NULL;
			m_lstCarPlateList[iPresetNum].RemoveAt(curPos);
			return S_OK;
		}
	}

	return S_OK;
}
*/


HRESULT CSWRecognizeTransformPPFilter::ReleaseCarPlateInfo(CAR_PLATE_INFO *pCarPlateInfo)
{
	if(pCarPlateInfo == NULL)
	{
		SW_TRACE_DEBUG("ERROR:Invalid Argument.");
		return E_FAIL;
	}
	SW_TRACE_DEBUG("Release car plate info: %s\n", (LPCSTR)pCarPlateInfo->pCarLeft->GetPlateNo());

	SAFE_RELEASE(pCarPlateInfo->pCarLeft);

	for(INT j = 0; j < 30; j++)
	{
		if(pCarPlateInfo->rgpVideo[j] != NULL)
		{
			SAFE_RELEASE(pCarPlateInfo->rgpVideo[j]);
			pCarPlateInfo->rgpVideo[j] = NULL;
		}
	}

	pCarPlateInfo->iVideoCount = 0;
	delete pCarPlateInfo;

	return S_OK;
}
