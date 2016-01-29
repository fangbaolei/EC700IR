#include "SWDomeCameraTimer.h"
#include "SWMessage.h"

CSWDomeCameraTimer::CSWDomeCameraTimer()
{
	m_fWorking = FALSE;
}


CSWDomeCameraTimer::~CSWDomeCameraTimer()
{
	m_fWorking = FALSE;
	StopTimer();
}


HRESULT CSWDomeCameraTimer::Initialize(CSWDomeCameraSetting * pcSetting)
{
	if (NULL == pcSetting)
	{
		SW_TRACE_DEBUG("Err: invalid arg.\n");
		return E_FAIL;
	}
	
	if (FAILED(CSWDomeCameraBase::Initialize(pcSetting)))
	{
		SW_TRACE_DEBUG("Err: failed to init CSWDomeCameraBase\n");
		return E_FAIL;
	}

	
	BOOL fEnable = FALSE;

	for (INT i=0; i<MAX_TIMER; i++)
	{
		TIMER_PARAM_STRUCT sSetting;	
		Get(i, sSetting);

		fEnable = sSetting.fValid && sSetting.fEnable;
		if (fEnable)
		{
			break;
		}
	}

	if (fEnable
		&& FAILED(StartTimer()))
	{	
		SW_TRACE_DEBUG("Err: failed to start timer\n");
		return E_FAIL;
	}

	m_fInit = TRUE;

	SW_TRACE_DEBUG("Info: CSWDomeCameraTimer init -- ok.\n");
	
	return S_OK;
}


HRESULT CSWDomeCameraTimer::Set(const DWORD& dwID, const TIMER_PARAM_STRUCT& sSetting)
{
	CHECK_ID(dwID, MAX_TIMER);

	if (sSetting.iBeginTime >= sSetting.iEndTime)
	{
		return E_INVALIDARG;
	}

	if (sSetting.fValid)
	{
		if (sSetting.fEnable)
		{
			StartTimer();
		}
		else if (GetSetting().Get().sTimerParam[dwID].fEnable)
		{
			//stop previous motion
			StopMotion(GetSetting().Get().sTimerParam[dwID].iMotionType, GetSetting().Get().sTimerParam[dwID].iMotionID);
		}
	}
	
	swpa_memcpy(&GetSetting().Get().sTimerParam[dwID], &sSetting, sizeof(GetSetting().Get().sTimerParam[dwID]));
	
	return SaveSetting(dwID);
}


HRESULT CSWDomeCameraTimer::Call(const DWORD& dwID)
{
	
	//todo: a overall switch or each ??

    CHECK_ID(dwID, MAX_TIMER);

	TIMER_PARAM_STRUCT sSetting;
	Get(dwID, sSetting);
	
	if (!sSetting.fValid)
	{
		SW_TRACE_DEBUG("Info: Timer#%d is not set\n", dwID);
		return S_OK;
	}
	
	return S_OK;
}


HRESULT CSWDomeCameraTimer::StartTimer()
{
	StopTimer();
	
	if (!m_fWorking)
	{
		m_fWorking = TRUE;
		if (FAILED(m_cThread.Start(CheckTimeProxy, this)))
		{
			m_fWorking = FALSE;
			SW_TRACE_DEBUG("Info: failed to start timers\n");
			return E_FAIL;
		}		
	}

	return S_OK;
}


HRESULT CSWDomeCameraTimer::StopTimer()
{
	if (m_fWorking)
	{
		m_fWorking = FALSE;
		if (FAILED(m_cThread.Stop()))
		{
			m_fWorking = TRUE;
			SW_TRACE_DEBUG("Info: failed to stop timers\n");
			return E_FAIL;
		}		
	}

	return S_OK;
}


HRESULT CSWDomeCameraTimer::Stop(const DWORD& dwID)
{
    CHECK_ID(dwID, MAX_TIMER);

	TIMER_PARAM_STRUCT sSetting;
	Get(dwID, sSetting);
	
	if (!sSetting.fValid)
	{
		SW_TRACE_DEBUG("Info: Timer#%d is not set\n", dwID);
		return S_OK;
	}

	GetSetting().Get().sTimerParam[dwID].fEnable = FALSE;
	
	return S_OK;
}



HRESULT CSWDomeCameraTimer::Clear(const DWORD& dwID)
{
	CHECK_ID(dwID, MAX_TIMER);

	Stop(dwID);
	
	swpa_memset(&GetSetting().Get().sTimerParam[dwID], 0, sizeof(GetSetting().Get().sTimerParam[dwID]));
	swpa_snprintf(GetSetting().Get().sTimerParam[dwID].szName, 31, "定时任务%d", dwID);
	GetSetting().Get().sTimerParam[dwID].iMotionType = MOVETYPE_CRUISE;
	GetSetting().Get().sTimerParam[dwID].iMotionID = 0;
	GetSetting().Get().sTimerParam[dwID].iBeginTime = 0;
	GetSetting().Get().sTimerParam[dwID].iEndTime = 1439;
	GetSetting().Get().sTimerParam[dwID].fValid = FALSE;
	
	return SaveSetting(dwID);
}


HRESULT CSWDomeCameraTimer::Get(const DWORD& dwID, TIMER_PARAM_STRUCT& sSetting)
{
	CHECK_ID(dwID, MAX_TIMER);

	swpa_memcpy(&sSetting, &GetSetting().Get().sTimerParam[dwID], sizeof(sSetting));

	//SW_TRACE_DEBUG("tobemarked: get timer %d: MotionType=%d, MotionID=%d\n", dwID, GetSetting().Get().sTimerParam[dwID].iMotionType,
	//	GetSetting().Get().sTimerParam[dwID].iMotionID);
		
	return S_OK;
}


HRESULT CSWDomeCameraTimer::SaveSetting(const DWORD& dwID)
{
	CHECK_ID(dwID, MAX_TIMER);
	
	char szSection[256];
    swpa_sprintf(szSection, "\\DomeCamera\\Timer\\Timer%d", dwID);

	//SW_TRACE_DEBUG("tobemarked: save timer %d : MotionType=%d, MotionID=%d\n", dwID, GetSetting().Get().sTimerParam[dwID].iMotionType,
	//	GetSetting().Get().sTimerParam[dwID].iMotionID);

	GetSetting().UpdateString(szSection, "Name", GetSetting().Get().sTimerParam[dwID].szName);
	GetSetting().UpdateInt(szSection, "BeginTime", GetSetting().Get().sTimerParam[dwID].iBeginTime);
	GetSetting().UpdateInt(szSection, "EndTime", GetSetting().Get().sTimerParam[dwID].iEndTime);
	GetSetting().UpdateInt(szSection, "Weekday", GetSetting().Get().sTimerParam[dwID].iWeekday);
	GetSetting().UpdateInt(szSection, "MotionType", GetSetting().Get().sTimerParam[dwID].iMotionType);
	GetSetting().UpdateInt(szSection, "MotionID", GetSetting().Get().sTimerParam[dwID].iMotionID);
	GetSetting().UpdateInt(szSection, "Enable", GetSetting().Get().sTimerParam[dwID].fEnable);
	GetSetting().UpdateInt(szSection, "Valid", GetSetting().Get().sTimerParam[dwID].fValid);
	
	return GetSetting().Commit();
}



HRESULT CSWDomeCameraTimer::StartMotion(const DWORD& dwMotionType, const DWORD& dwMotionID)
{
	switch (dwMotionType)
	{
		case MOVETYPE_PRESET:
		{
			if (CSWMessage::SendMessage(MSG_CALL_PRESET_POS, WPARAM(dwMotionID), OP_FROM_USER))
			{	
				SW_TRACE_DEBUG("Err:failed to call Preset #%d\n", dwMotionID);
				return E_FAIL;
			}
		}
		break;
		
		case MOVETYPE_HSCAN:
		{
			if (CSWMessage::SendMessage(MSG_START_HSCAN, WPARAM(dwMotionID), OP_FROM_USER))
			{	
				SW_TRACE_DEBUG("Err:failed to call HScan #%d\n", dwMotionID);
				return E_FAIL;
			}
		}
		break;
		
		case MOVETYPE_FSCAN:
		{
			if (CSWMessage::SendMessage(MSG_CALL_FSCAN, WPARAM(dwMotionID), OP_FROM_USER))
			{	
				SW_TRACE_DEBUG("Err:failed to call FScan #%d\n", dwMotionID);
				return E_FAIL;
			}
		}
		break;
		
		case MOVETYPE_CRUISE:
		{
			if (CSWMessage::SendMessage(MSG_CALL_CRUISE, WPARAM(dwMotionID), OP_FROM_USER))
			{	
				SW_TRACE_DEBUG("Err:failed to call Cruise #%d\n", dwMotionID);
				return E_FAIL;
			}
		}
		break;
		default:
		break;
	}

	return S_OK;
}



HRESULT CSWDomeCameraTimer::StopMotion(const DWORD& dwMotionType, const DWORD& dwMotionID)
{
	switch (dwMotionType)
	{
		case MOVETYPE_PRESET:
		{
			if (FAILED(GetPresetInstance().Stop(dwMotionID)))
			{	
				SW_TRACE_DEBUG("Err:failed to stop preset #%d\n", dwMotionID);
				return E_FAIL;
			}
		}
		break;
		
		case MOVETYPE_HSCAN:
		{
			if (CSWMessage::SendMessage(MSG_STOP_HSCAN, WPARAM(dwMotionID), OP_FROM_USER))
			{	
				SW_TRACE_DEBUG("Err:failed to stop HScan #%d\n", dwMotionID);
				return E_FAIL;
			}
		}
		break;
		
		case MOVETYPE_FSCAN:
		{
			if (CSWMessage::SendMessage(MSG_STOP_FSCAN, WPARAM(dwMotionID), OP_FROM_USER))
			{	
				SW_TRACE_DEBUG("Err:failed to stop FScan #%d\n", dwMotionID);
				return E_FAIL;
			}
		}
		break;
		
		case MOVETYPE_CRUISE:
		{
			if (CSWMessage::SendMessage(MSG_STOP_CRUISE, WPARAM(dwMotionID), OP_FROM_USER))
			{	
				SW_TRACE_DEBUG("Err:failed to stop Cruise #%d\n", dwMotionID);
				return E_FAIL;
			}
		}
		break;
		default:
		break;
	}

	return S_OK;
}


HRESULT CSWDomeCameraTimer::CheckTime()
{
	
	BOOL afRunning[MAX_TIMER] = {FALSE};

	SW_TRACE_DEBUG("Info: CheckTime runnung...\n");
	
	while (m_fWorking)
	{
		for (INT i=0; i<MAX_TIMER && m_fWorking; i++)
		{
			TIMER_PARAM_STRUCT sSetting;	
			Get(i, sSetting);

			CSWDateTime cCurTime;

			BOOL fStartIt = FALSE, fStopIt = FALSE;

			if (sSetting.fValid && sSetting.fEnable)
			{
				DWORD dwCurMin = cCurTime.GetHour()*60 + cCurTime.GetMinute();
				
				if ((sSetting.iWeekday & (0x1<<cCurTime.GetWeekday()))
					&& sSetting.iBeginTime <= dwCurMin 
					&& sSetting.iEndTime >= dwCurMin)
				{
					if (!afRunning[i])
					{
						fStartIt = TRUE;						
					}					
				}
				else if (afRunning[i])
				{
					fStopIt = TRUE;
				}
			}
			else if (afRunning[i])
			{
				fStopIt = TRUE;
			}

			if (fStartIt)
			{
				if (FAILED(StartMotion(sSetting.iMotionType, sSetting.iMotionID)))
				{
					SW_TRACE_DEBUG("Err:failed to start Timer #%d motion(%d, %d)\n",
						i, sSetting.iMotionType, sSetting.iMotionID);
				}
				else
				{
					afRunning[i] = TRUE;
				}
			}

			if (fStopIt)
			{
				if (FAILED(StopMotion(sSetting.iMotionType, sSetting.iMotionID)))
				{
					SW_TRACE_DEBUG("Err:failed to stop Timer #%d motion(%d, %d)\n",
						i, sSetting.iMotionType, sSetting.iMotionID);
				}
				else
				{
					afRunning[i] = FALSE;
				}
			}
		}

		swpa_thread_sleep_ms(1000);
	}
	
	return S_OK;	
}



PVOID CSWDomeCameraTimer::CheckTimeProxy(PVOID pvArg)
{
	if (NULL != pvArg)
	{
		CSWDomeCameraTimer * pThis = (CSWDomeCameraTimer*)pvArg;

		pThis->CheckTime();
	}
}



