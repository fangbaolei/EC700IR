#include "SWFC.h"
#include "SWDomeCameraFScan.h"
#include "SWMessage.h"

CSWDomeCameraFScan::CSWDomeCameraFScan()
{
	m_dwScanningID = MAX_FSCAN_PATH;
	m_fIsIdle = TRUE;
}

CSWDomeCameraFScan::~CSWDomeCameraFScan()
{
	Stop(m_dwScanningID);
	m_dwScanningID = MAX_FSCAN_PATH;
	m_cThread.Stop();
	m_fIsIdle = TRUE;
	m_fInit = FALSE;
}



HRESULT CSWDomeCameraFScan::SetInitialState(const DWORD& dwID)
{
	CHECK_ID(dwID, MAX_FSCAN_PATH);

	POS_PARAM_STRUCT sPosParam;
	if (FAILED(GetPosParam(sPosParam)))
	{
		SW_TRACE_DEBUG("Err: failed to get position param.\n");
		return E_FAIL;
	}

	GetSetting().Get().sFScanParam[dwID].sInitialState = sPosParam;
	
	return S_OK;
}


HRESULT CSWDomeCameraFScan::GotoInitialState(const DWORD& dwID)
{
	CHECK_ID(dwID, MAX_FSCAN_PATH);

	FSCAN_PARAM_STRUCT sFScanParam;
		
	Get(dwID, sFScanParam);

	if (FAILED(SetPosParam(sFScanParam.sInitialState)))
	{
		SW_TRACE_DEBUG("Err: failed to set position param.\n");
		return E_FAIL;
	}

	
	return S_OK;
}



HRESULT CSWDomeCameraFScan::AddMotion(const DWORD& dwID, const INT& iMove, const INT& iArg)
{
	if ((dwID) >= (MAX_FSCAN_PATH))
	{
		//SW_TRACE_DEBUG("Err: invalid arg dwID (=%d)\n", dwID);
		return E_INVALIDARG;
	}
	
	
	if (FSCAN_M_NOP != iMove)
	{
		DWORD dwTick = CSWDateTime::GetSystemTick();
		INT iNopTimeMs = (dwTick > m_dwLastTick) ? (dwTick - m_dwLastTick) : 0;
		
		INT iMotionCount = 0;
		iMotionCount = GetSetting().Get().sFScanParam[dwID].iMotionCount;
		if (iMotionCount < MAX_MOTION_IN_FSCAN)
		{
			GetSetting().Get().sFScanParam[dwID].sMotion[iMotionCount].iMove = FSCAN_M_NOP;
			GetSetting().Get().sFScanParam[dwID].sMotion[iMotionCount].iArg = iNopTimeMs;
			GetSetting().Get().sFScanParam[dwID].iMotionCount ++;
		}
		else
		{	
			SW_TRACE_DEBUG("Err: no space for fscan #%d recording.\n", dwID);
			m_fIsIdle = TRUE;
			return E_OUTOFMEMORY;
		}

		iMotionCount = GetSetting().Get().sFScanParam[dwID].iMotionCount;
		if (iMotionCount < MAX_MOTION_IN_FSCAN)
		{
			GetSetting().Get().sFScanParam[dwID].sMotion[iMotionCount].iMove = iMove;
			GetSetting().Get().sFScanParam[dwID].sMotion[iMotionCount].iArg = iArg;
			
			GetSetting().Get().sFScanParam[dwID].iMotionCount ++;
		}
		else
		{	
			SW_TRACE_DEBUG("Err: no space for fscan #%d recording.\n", dwID);
			m_fIsIdle = TRUE;
			return E_OUTOFMEMORY;
		}
	}
	else
	{
		m_fIsIdle = FALSE;

		swpa_camera_ball_sync_zoom();
		
		if (FAILED(SetInitialState(dwID)))
		{	
			SW_TRACE_DEBUG("Err: failed to set FScan #%d Initial State.\n", dwID);
			return E_FAIL;
		}

		GetSetting().Get().sFScanParam[dwID].iMotionCount = 0;
		swpa_memset(&GetSetting().Get().sFScanParam[dwID].sMotion, 0, sizeof(GetSetting().Get().sFScanParam[dwID].sMotion));
	}

	//todo: 发消息告知当前存储状态
		
	m_dwLastTick = CSWDateTime::GetSystemTick();
	
    return S_OK;
}


HRESULT CSWDomeCameraFScan::Call(const DWORD& dwID)
{
	CHECK_ID(dwID, MAX_FSCAN_PATH);

	//if (m_dwScanningID < MAX_FSCAN_PATH) //每次调用前都先stop一下
	{
		m_dwScanningID = MAX_FSCAN_PATH;
		m_cThread.Stop();
	}

	FSCAN_PARAM_STRUCT sFScanParam;

	Get(dwID, sFScanParam);

	if (!sFScanParam.fValid || 0 >= sFScanParam.iMotionCount)
	{
		SW_TRACE_DEBUG("Err: FScan #%d has no motion...\n", dwID);
		return E_FAIL;
	}
	
	m_dwScanningID = dwID;
	
	m_cThread.Start(FScanProxy, this);
}


HRESULT CSWDomeCameraFScan::Stop(const DWORD& dwID)
{
	CHECK_ID(dwID, MAX_FSCAN_PATH);

	if (dwID != m_dwScanningID)
	{
		SW_TRACE_DEBUG("Warning: FScan #%d is not scanning...\n", dwID);
		return S_OK;
	}

	//SW_TRACE_DEBUG("stop fscan %d\n", dwID);

	BreakWalk(TRUE);
	m_dwScanningID = MAX_FSCAN_PATH;
	//为快速停止，不在这里等待线程退出，留到下次调用或者程序退出时再调用Stop()
	/*if (FAILED(m_cThread.Stop())) 
	{
		SW_TRACE_DEBUG("Err: failed to stop FScan #%d\n", dwID);
		return E_INVALIDARG;
	}*/

	if (SWPAR_OK != swpa_camera_ball_stop()
		|| FAILED(CSWMessage::SendMessage(MSG_STOP_ZOOM, 0, OP_FROM_PROGRAM))/* to SyncZoom*/
		)
	{
		SW_TRACE_DEBUG("Err: failed to stop FScan #%d\n", dwID);
		return E_INVALIDARG;
	}
	
	SW_TRACE_DEBUG("fscan #%d stoped!!\n", dwID);
	
	
	return S_OK;
}



HRESULT CSWDomeCameraFScan::ClearPath(const DWORD& dwID)
{
	CHECK_ID(dwID, MAX_FSCAN_PATH);

	Stop(dwID);
	
	swpa_memset(&GetSetting().Get().sFScanParam[dwID], 0, sizeof(GetSetting().Get().sFScanParam[dwID]));
	swpa_snprintf(GetSetting().Get().sFScanParam[dwID].szName, 31, "花样扫描%d", dwID);
	GetSetting().Get().sFScanParam[dwID].fValid = FALSE;

	return SaveSetting(dwID);
}

HRESULT CSWDomeCameraFScan::SavePath(const DWORD& dwID)
{
	CHECK_ID(dwID, MAX_FSCAN_PATH);

	DWORD dwTick = CSWDateTime::GetSystemTick();
	INT iNopTimeMs = (dwTick > m_dwLastTick) ? (dwTick - m_dwLastTick) : 0;
	
	INT iMotionCount = 0;
	iMotionCount = GetSetting().Get().sFScanParam[dwID].iMotionCount;
	if (iMotionCount < MAX_MOTION_IN_FSCAN)
	{
		GetSetting().Get().sFScanParam[dwID].sMotion[iMotionCount].iMove = FSCAN_M_NOP;
		GetSetting().Get().sFScanParam[dwID].sMotion[iMotionCount].iArg = iNopTimeMs;
		GetSetting().Get().sFScanParam[dwID].iMotionCount ++;
	}
	else
	{	
		SW_TRACE_DEBUG("Err: no space for fscan recording.\n");
		return E_OUTOFMEMORY;
	}

	GetSetting().Get().sFScanParam[dwID].fValid = TRUE;

	m_fIsIdle = TRUE;
		
    return SaveSetting(dwID);
}


HRESULT CSWDomeCameraFScan::SetName(const DWORD& dwID, const CSWString& strName)
{
	CHECK_ID(dwID, MAX_HSCAN_PATH);

	FSCAN_PARAM_STRUCT sFScanParam;

	Get(dwID, sFScanParam);
	
	swpa_strncpy(sFScanParam.szName, (LPCSTR)(CSWString)strName, sizeof(sFScanParam.szName) - 1);    
	
	return Set(dwID, sFScanParam);
}


HRESULT CSWDomeCameraFScan::GetName(const DWORD& dwID, CSWString& strName)
{
	CHECK_ID(dwID, MAX_HSCAN_PATH);

	
	FSCAN_PARAM_STRUCT sFScanParam;

	Get(dwID, sFScanParam);
	
	strName = sFScanParam.szName;
		
	return S_OK;
}



HRESULT CSWDomeCameraFScan::Get(const DWORD& dwID, FSCAN_PARAM_STRUCT& sFScanParam )
{
	CHECK_ID(dwID, MAX_HSCAN_PATH);

	swpa_memcpy(&sFScanParam, &GetSetting().Get().sFScanParam[dwID], sizeof(sFScanParam));
	
	return S_OK;
}


HRESULT CSWDomeCameraFScan::Set(const DWORD& dwID, const FSCAN_PARAM_STRUCT& sFScanParam )
{
	CHECK_ID(dwID, MAX_HSCAN_PATH);

	swpa_memcpy(&GetSetting().Get().sFScanParam[dwID], &sFScanParam, sizeof(GetSetting().Get().sFScanParam[dwID]));
	
	return SaveSetting(dwID);
}


HRESULT CSWDomeCameraFScan::Walk(const DWORD& dwID, const DWORD& dwStep)
{
	CHECK_ID(dwID, MAX_FSCAN_PATH);

	m_fIsIdle = FALSE;

	FSCAN_PARAM_STRUCT sFScanParam;
		
	Get(dwID, sFScanParam);
	
	INT iMotionCount = 0;
	iMotionCount = sFScanParam.iMotionCount;
	
	if (dwStep >= iMotionCount)
	{
		SW_TRACE_DEBUG("Err: invalid arg dwStep (=%d) > MotionCount (=%d)\n", dwID, iMotionCount);
		return E_UNEXPECTED;
	}

	if (0 == dwStep)
	{
		if (FAILED(GotoInitialState(dwID)))
		{
			SW_TRACE_DEBUG("Err: FScan #%d failed to goto init state.\n", dwID);
			return E_FAIL;
		}
		CSWApplication::Sleep(500);
	}

	FSCAN_MOTION_STRUCT sMotion = sFScanParam.sMotion[dwStep];

	static DWORD dwPrevMove = FSCAN_M_END;
		
	//swpa_camera_ball_sync_zoom();
	
	switch (sMotion.iMove)
	{	
		case FSCAN_M_NOP:
		{
			SW_TRACE_DEBUG("Info: FSCAN_M_NOP: %d\n", sMotion.iArg);
			if (dwPrevMove == FSCAN_M_START_PAN
				||dwPrevMove == FSCAN_M_START_TILT
				|| dwPrevMove == FSCAN_M_START_PAN_TILT
				)
			{
				sMotion.iArg += 30; //fine tune
			}
			else if (dwPrevMove == FSCAN_M_START_ZOOM)
			{
				sMotion.iArg += 20; //fine tune
			}

			m_fIsIdle = TRUE;

			for (INT iCount = 0; iCount < sMotion.iArg/500 && (m_dwScanningID == dwID || !WalkBroken()); iCount++)
			{
				
				//SW_TRACE_DEBUG("Info: FSCAN_M_NOP: sleep 500 (%d, %d)\n", m_dwScanningID, dwID);
				swpa_thread_sleep_ms(500);
			}

			if (sMotion.iArg%500 > 0 && (m_dwScanningID == dwID || !WalkBroken()))
			{
				//SW_TRACE_DEBUG("Info: FSCAN_M_NOP: sleep sMotion.iArg%500 = %d\n", sMotion.iArg%500);
				swpa_thread_sleep_ms(sMotion.iArg%500);
			}
			
		}
		break;

		case FSCAN_M_CALL_PRESET:
		{
			SW_TRACE_DEBUG("Info: FSCAN_M_CALL_PRESET: %d\n", sMotion.iArg);
			GetPresetInstance().Call((DWORD)sMotion.iArg);
		}
		break;

		case FSCAN_M_START_PAN:
		{
			SW_TRACE_DEBUG("Info: FSCAN_M_START_PAN: %d\n", sMotion.iArg);
			swpa_camera_ball_pan((PAN_TYPE)((sMotion.iArg >> 16) & 0xFF), 
				(sMotion.iArg & 0xFF));
		}
		break;

		case FSCAN_M_STOP_PAN:
		{
			SW_TRACE_DEBUG("Info: FSCAN_M_STOP_PAN: %d\n", sMotion.iArg);
			swpa_camera_ball_stop();
		}
		break;

		case FSCAN_M_START_TILT:
		{
			SW_TRACE_DEBUG("Info: FSCAN_M_START_TILT: %d\n", sMotion.iArg);
			swpa_camera_ball_tilt((TILT_TYPE)((sMotion.iArg >> 16) & 0xFF), 
				(sMotion.iArg & 0xFF));
			//CSWMessage::SendMessage(DWORD id,WPARAM wParam,LPARAM lParam)
		}
		break;

		case FSCAN_M_STOP_TILT:
		{
			SW_TRACE_DEBUG("Info: FSCAN_M_STOP_TILT: %d\n", sMotion.iArg);
			swpa_camera_ball_stop();
		}
		break;

		case FSCAN_M_START_PAN_TILT:
		{
			SW_TRACE_DEBUG("Info: FSCAN_M_START_PAN_TILT: %d\n", sMotion.iArg);
			PAN_TYPE ePan = PAN_TYPE((sMotion.iArg >> 24) & 0xFF);
			INT iPanSpeed = (sMotion.iArg >> 16) & 0xFF;
			TILT_TYPE eTilt = TILT_TYPE((sMotion.iArg >> 8) & 0xFF);
			INT iTiltSpeed = (sMotion.iArg >> 0) & 0xFF;
			swpa_camera_ball_pan_tilt(ePan, iPanSpeed, eTilt, iTiltSpeed);
		}
		break;

		case FSCAN_M_STOP_PAN_TILT:
		{
			SW_TRACE_DEBUG("Info: FSCAN_M_STOP_PAN_TILT: %d\n", sMotion.iArg);
			swpa_camera_ball_stop();
		}
		break;

		/*case FSCAN_M_SET_ZOOM:
		{
			SW_TRACE_DEBUG("Info: FSCAN_M_SET_ZOOM: %d\n", sMotion.iArg);
			swpa_camera_basicparam_set_zoom_value(sMotion.iArg);
		}
		break;*/

		case FSCAN_M_START_ZOOM:
		{
			SW_TRACE_DEBUG("Info: FSCAN_M_START_ZOOM: %d\n", sMotion.iArg);
			//swpa_camera_basicparam_set_zoom_speed((ZOOM_MODE)((sMotion.iArg >> 16) & 0xFF), 
			//	(sMotion.iArg & 0xFF));
			CSWMessage::SendMessage(MSG_START_ZOOM, (ZOOM_MODE)((sMotion.iArg >> 16) & 0xFF), OP_FROM_PROGRAM);
		}
		break;

		case FSCAN_M_STOP_ZOOM:
		{
			SW_TRACE_DEBUG("Info: FSCAN_M_STOP_ZOOM: %d\n", sMotion.iArg);
			//swpa_camera_basicparam_set_zoom_mode(ZOOM_STOP);
			CSWMessage::SendMessage(MSG_STOP_ZOOM, 0, OP_FROM_PROGRAM);
		}
		break;
		
		default:
		{
		}
		break;
	}

	dwPrevMove = sMotion.iMove;
	
	
    return S_OK;
}


HRESULT CSWDomeCameraFScan::FScan()
{
	DWORD dwScanID = m_dwScanningID;
	
	FSCAN_PARAM_STRUCT sSetting;
	Get(dwScanID,  sSetting);

	SW_TRACE_DEBUG("Info: Fscan started (id = %d)...\n", dwScanID);
	
	while (m_dwScanningID < MAX_FSCAN_PATH &&  S_OK == m_cThread.IsValid())
	{
		// 如果 是空的。
		// 直接退出
		if( sSetting.iMotionCount <= 0 )
		{
			break;
		}

		BreakWalk(FALSE);
		for (INT i=0; i<sSetting.iMotionCount && m_dwScanningID < MAX_FSCAN_PATH; i++)
		{
	    	// 是否已经停止
    		if( S_OK != m_cThread.IsValid() )
    		{
    			break;
    		}

			SW_TRACE_DEBUG("Info: in Fscan Motion #%d, m_dwScanningID = %d....\n", i, m_dwScanningID);
			
			Walk(dwScanID, (DWORD)i);
		}
		
		swpa_thread_sleep_ms(200);

	}

	SW_TRACE_DEBUG("Info: Fscan stopped...\n");
	
	return S_OK;	
}



PVOID CSWDomeCameraFScan::FScanProxy(PVOID pvArg)
{
	if (NULL != pvArg)
	{
		CSWDomeCameraFScan * pThis = (CSWDomeCameraFScan*)pvArg;

		pThis->FScan();
	}
}



HRESULT CSWDomeCameraFScan::SaveSetting(const DWORD& dwID)
{
	CHECK_ID(dwID, MAX_HSCAN_PATH);

	char szSection[256];
    swpa_sprintf(szSection, "\\DomeCamera\\FScan\\Path%d", dwID);
		
	GetSetting().UpdateString(szSection, "Name", GetSetting().Get().sFScanParam[dwID].szName);
	GetSetting().UpdateInt(szSection, "Valid", GetSetting().Get().sFScanParam[dwID].fValid);
	GetSetting().UpdateInt(szSection, "MotionCount", GetSetting().Get().sFScanParam[dwID].iMotionCount);

	GetSetting().UpdateInt(szSection, "PanCoordinate", GetSetting().Get().sFScanParam[dwID].sInitialState.iPanCoordinate);
	GetSetting().UpdateInt(szSection, "TiltCoordinate", GetSetting().Get().sFScanParam[dwID].sInitialState.iTiltCoordinate);
	GetSetting().UpdateInt(szSection, "Zoom", GetSetting().Get().sFScanParam[dwID].sInitialState.iZoom);
	GetSetting().UpdateInt(szSection, "Iris", GetSetting().Get().sFScanParam[dwID].sInitialState.iIris);
	GetSetting().UpdateInt(szSection, "Focus", GetSetting().Get().sFScanParam[dwID].sInitialState.iFocus);
	GetSetting().UpdateInt(szSection, "FocusMode", GetSetting().Get().sFScanParam[dwID].sInitialState.iFocusMode);
	GetSetting().UpdateInt(szSection, "PanSpeed", GetSetting().Get().sFScanParam[dwID].sInitialState.iPanSpeed);
	GetSetting().UpdateInt(szSection, "TiltSpeed", GetSetting().Get().sFScanParam[dwID].sInitialState.iTiltSpeed);

	
	for (int j = 0; j < GetSetting().Get().sFScanParam[dwID].iMotionCount; j++)
	{
		swpa_sprintf(szSection, "\\DomeCamera\\FScan\\Path%d\\Motion%d", dwID, j);
		GetSetting().UpdateInt(szSection, "Move", GetSetting().Get().sFScanParam[dwID].sMotion[j].iMove);
		GetSetting().UpdateInt(szSection, "Arg", GetSetting().Get().sFScanParam[dwID].sMotion[j].iArg);
	}

	return GetSetting().Commit();
}


