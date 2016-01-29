#include "SWDomeCameraWatchkeeping.h"
#include "SWMessage.h"

CSWDomeCameraWatchKeeping::CSWDomeCameraWatchKeeping()
{
	m_fWatching = FALSE;
	m_fCountingDown = FALSE;
	m_fInit = FALSE;
	m_fPaused = FALSE;
}


CSWDomeCameraWatchKeeping::~CSWDomeCameraWatchKeeping()
{
	m_fPaused = FALSE;
	Stop();
	m_fCountingDown = FALSE;
	m_fInit = FALSE;
}


HRESULT CSWDomeCameraWatchKeeping::Initialize(CSWDomeCameraSetting * pcSetting)
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

	
	WATCHKEEPING_PARAM_STRUCT sSetting;	
	Get(sSetting);

	if (sSetting.fEnable
		&& FAILED(Call()))
	{	
		SW_TRACE_DEBUG("Err: failed to start watchkeeping \n");
		return E_FAIL;
	}
	
	return S_OK;
}



HRESULT CSWDomeCameraWatchKeeping::Set(const WATCHKEEPING_PARAM_STRUCT& sSetting)
{
	swpa_memcpy(&GetSetting().Get().sWatchKeepingParam, &sSetting, sizeof(GetSetting().Get().sWatchKeepingParam));

	if (sSetting.fEnable)
	{
		if (FAILED(Call()))
		{
			SW_TRACE_DEBUG("Err: failed to start watchkeeping \n");
			//return E_FAIL;
		}
	}
	else
	{
		if (FAILED(Stop()))
		{
			SW_TRACE_DEBUG("Err: failed to Stop watchkeeping\n");
			//return E_FAIL;
		}
	}

	return SaveSetting();
}


HRESULT CSWDomeCameraWatchKeeping::Call()
{
	WATCHKEEPING_PARAM_STRUCT sSetting;
	Get(sSetting);
		
	if (!sSetting.fEnable)
	{
		SW_TRACE_DEBUG("Info: WatchKeeping is not enabled\n");
		return S_OK;
	}

	m_fCountingDown = TRUE;
	m_cThread.Start(CheckTimeProxy, this);
	
	return S_OK;
}


HRESULT CSWDomeCameraWatchKeeping::Stop()
{
	if (m_fCountingDown
		|| m_fWatching)
	{
		m_fCountingDown = FALSE;
		m_fWatching = FALSE;
		m_cThread.Stop();
	}

	return S_OK;
}


HRESULT CSWDomeCameraWatchKeeping::Clear()
{	
	m_fCountingDown = FALSE;
	m_cThread.Stop();
	
	swpa_memset(&GetSetting().Get().sWatchKeepingParam, 0, sizeof(GetSetting().Get().sWatchKeepingParam));
	GetSetting().Get().sWatchKeepingParam.fEnable = FALSE;
	GetSetting().Get().sWatchKeepingParam.iMotionType = MOVETYPE_CRUISE;
	GetSetting().Get().sWatchKeepingParam.iWatchTime = 300;
	
	return SaveSetting();
}


HRESULT CSWDomeCameraWatchKeeping::Get(WATCHKEEPING_PARAM_STRUCT& sSetting)
{
	swpa_memcpy(&sSetting, &GetSetting().Get().sWatchKeepingParam, sizeof(sSetting));

	return S_OK;
}

HRESULT CSWDomeCameraWatchKeeping::SaveSetting()
{
	GetSetting().UpdateInt("\\DomeCamera\\WatchKeeping", "Enable", GetSetting().Get().sWatchKeepingParam.fEnable);
	GetSetting().UpdateInt("\\DomeCamera\\WatchKeeping", "WatchTime", GetSetting().Get().sWatchKeepingParam.iWatchTime);
	GetSetting().UpdateInt("\\DomeCamera\\WatchKeeping", "MotionType", GetSetting().Get().sWatchKeepingParam.iMotionType);
	GetSetting().UpdateInt("\\DomeCamera\\WatchKeeping", "MotionID", GetSetting().Get().sWatchKeepingParam.iMotionID);
	

	return GetSetting().Commit();
}


HRESULT CSWDomeCameraWatchKeeping::Pause()
{
	m_fPaused = TRUE;

	return S_OK;
}


HRESULT CSWDomeCameraWatchKeeping::Resume()
{
	m_fPaused = FALSE;

	return S_OK;
}


HRESULT CSWDomeCameraWatchKeeping::GetStatus(INT& iStatus)
{
	iStatus = m_fWatching;

	return S_OK;
}



HRESULT CSWDomeCameraWatchKeeping::CheckTime()
{
	m_dwStartTick = CSWDateTime::GetSystemTick();

	SW_TRACE_DEBUG("Info: begin to count down for watchkeeping\n");
		
	while (m_fCountingDown)
	{	
		WATCHKEEPING_PARAM_STRUCT sSetting;

		Get(sSetting);

		if (!sSetting.fEnable)
		{
			break;
		}
		
		if (m_fPaused)
		{
			CSWApplication::Sleep(1000);
			continue;
		}
		
		DWORD dwTick = CSWDateTime::GetSystemTick();
		SW_TRACE_DEBUG("---tobedeleted: %d, %d, %d, %d\n", m_fCountingDown, m_fWatching, sSetting.iWatchTime, (dwTick - m_dwStartTick)/1000);
		
		if (!m_fWatching
			&& dwTick > m_dwStartTick 
			&& (dwTick - m_dwStartTick)/1000 >= sSetting.iWatchTime //sec
			)
		{
			switch (sSetting.iMotionType)
			{	
				case MOVETYPE_PRESET:
				{
					if (CSWMessage::SendMessage(MSG_CALL_PRESET_POS, WPARAM(sSetting.iMotionID), OP_FROM_PROGRAM))
					{	
						SW_TRACE_DEBUG("Err:failed to call Preset #%d\n", sSetting.iMotionID);
					}
				}
				break;
				case MOVETYPE_HSCAN:
				{
					if (CSWMessage::SendMessage(MSG_START_HSCAN, WPARAM(sSetting.iMotionID), OP_FROM_PROGRAM))
					{	
						SW_TRACE_DEBUG("Err:failed to call HScan #%d\n", sSetting.iMotionID);
					}
				}
				break;
				case MOVETYPE_FSCAN:
				{
					if (CSWMessage::SendMessage(MSG_CALL_FSCAN, WPARAM(sSetting.iMotionID), OP_FROM_PROGRAM))
					{	
						SW_TRACE_DEBUG("Err:failed to call FScan #%d\n", sSetting.iMotionID);
					}
				}
				break;
				case MOVETYPE_CRUISE:
				{
					if (CSWMessage::SendMessage(MSG_CALL_CRUISE, WPARAM(sSetting.iMotionID), OP_FROM_PROGRAM))
					{
						SW_TRACE_DEBUG("Err:failed to call Cruise #%d\n", sSetting.iMotionID);
					}
				}
				break;
				default:
				break;
			}

			m_fWatching = TRUE;
		}

		swpa_thread_sleep_ms(1000);
	}

	SW_TRACE_DEBUG("Info: stop counting down for watchkeeping\n");
	
	m_fCountingDown = FALSE;
	return S_OK;	
}



PVOID CSWDomeCameraWatchKeeping::CheckTimeProxy(PVOID pvArg)
{
	if (NULL != pvArg)
	{
		CSWDomeCameraWatchKeeping * pThis = (CSWDomeCameraWatchKeeping*)pvArg;

		pThis->CheckTime();
	}
}

HRESULT CSWDomeCameraWatchKeeping::Update()
{
	if (m_fCountingDown)
	{
		m_dwStartTick = CSWDateTime::GetSystemTick();
	}
	
	if (m_fWatching)
	{
		WATCHKEEPING_PARAM_STRUCT sSetting;

		Get(sSetting);

		BOOL fOK = TRUE;

		switch (sSetting.iMotionType)
		{	
			case MOVETYPE_PRESET:
			{
				//do nothing
			}
			break;
			case MOVETYPE_HSCAN:
			{
				if (CSWMessage::SendMessage(MSG_STOP_HSCAN, WPARAM(sSetting.iMotionID), OP_FROM_PROGRAM))
				{	
					SW_TRACE_DEBUG("Err:failed to stop HScan #%d\n", sSetting.iMotionID);
					fOK = FALSE;
				}
			}
			break;
			case MOVETYPE_FSCAN:
			{
				
				if (CSWMessage::SendMessage(MSG_STOP_FSCAN, WPARAM(sSetting.iMotionID), OP_FROM_PROGRAM))
				{	
					SW_TRACE_DEBUG("Err:failed to stop FScan #%d\n", sSetting.iMotionID);
					fOK = FALSE;
				}
			}
			break;
			case MOVETYPE_CRUISE:
			{
				if (CSWMessage::SendMessage(MSG_STOP_CRUISE, WPARAM(sSetting.iMotionID), OP_FROM_PROGRAM))
				{	
					SW_TRACE_DEBUG("Err:failed to stop Cruise #%d\n", sSetting.iMotionID);
					fOK = FALSE;
				}
			}
			break;
			default:
			break;
		}
		
		if (fOK)
		{	
			SW_TRACE_DEBUG("Info: camera watchkeeping broke.\n");
			m_fWatching = FALSE;
			m_fCountingDown = TRUE;
		}
		else
		
		{	
			SW_TRACE_DEBUG("Err: failed to break watchkeeping.\n");
			return E_FAIL;
		}
	}
	

	return S_OK;
}

