#include "SWFC.h"
#include "SWDomeCameraStandby.h"

CSWDomeCameraStandby::CSWDomeCameraStandby()
{
	m_dwStartTick = 0;
	m_fCountingDown = FALSE;
	m_fSleeping = FALSE;
	m_fPaused = FALSE;
}


CSWDomeCameraStandby::~CSWDomeCameraStandby()
{
	m_fPaused = FALSE;
	Stop();
	m_fCountingDown = FALSE;
	m_fSleeping = FALSE;
}


HRESULT CSWDomeCameraStandby::Initialize(CSWDomeCameraSetting * pcSetting)
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

	
	STANDBY_PARAM_STRUCT sSetting;	
	Get(sSetting);

	if (sSetting.fEnable
		&& FAILED(Call()))
	{	
		SW_TRACE_DEBUG("Err: failed to start standby count down\n");
		return E_FAIL;
	}

	m_fInit = TRUE;

	SW_TRACE_DEBUG("Info: CSWDomeCameraStandby init -- ok.\n");
		
	return S_OK;
}


HRESULT CSWDomeCameraStandby::Set(const STANDBY_PARAM_STRUCT& sSetting)
{
	GetSetting().Get().sStandbyParam.iWaitTime = sSetting.iWaitTime;
	GetSetting().Get().sStandbyParam.fEnable = sSetting.fEnable;

	if (sSetting.fEnable)
	{
		if (FAILED(Call()))
		{
			SW_TRACE_DEBUG("Err: failed to start standby count down\n");
			//return E_FAIL;
		}
	}
	else
	{
		if (FAILED(Stop()))
		{
			SW_TRACE_DEBUG("Err: failed to Stop standby count down\n");
			//return E_FAIL;
		}
	}
	
	return SaveSetting();	
}


HRESULT CSWDomeCameraStandby::Call()
{
	STANDBY_PARAM_STRUCT sSetting;
	Get(sSetting);
		
	if (!sSetting.fEnable)
	{
		SW_TRACE_DEBUG("Info: Standby is not enabled\n");
		return S_OK;
	}

	if (m_fCountingDown)
	{	
		SW_TRACE_DEBUG("Info: Standby is counting down\n");
		return S_OK;
	}

	m_fCountingDown = TRUE;
	m_cThread.Start(CheckTimeProxy, this);
		
	return S_OK;
}


HRESULT CSWDomeCameraStandby::Stop()
{
	if (m_fCountingDown
		|| m_fSleeping)
	{
		m_fCountingDown = FALSE;
		m_fSleeping = FALSE;
		m_cThread.Stop();
	}

	return S_OK;
}


HRESULT CSWDomeCameraStandby::Clear()
{	
	m_fCountingDown = FALSE;
	m_cThread.Stop();
	m_fSleeping = FALSE;
	
	GetSetting().Get().sStandbyParam.iWaitTime = 0;
	GetSetting().Get().sStandbyParam.fEnable = FALSE;
		
	return SaveSetting();
}


HRESULT CSWDomeCameraStandby::Get(STANDBY_PARAM_STRUCT& sSetting)
{

	sSetting.iWaitTime = GetSetting().Get().sStandbyParam.iWaitTime;
	sSetting.fEnable = GetSetting().Get().sStandbyParam.fEnable;

	return S_OK;
}


HRESULT CSWDomeCameraStandby::GetStatus(INT& iStatus)
{
	iStatus = m_fSleeping;

	return S_OK;
}

HRESULT CSWDomeCameraStandby::Pause()
{
	m_fPaused = TRUE;

	return S_OK;
}


HRESULT CSWDomeCameraStandby::Resume()
{
	m_fPaused = FALSE;

	return S_OK;
}



HRESULT CSWDomeCameraStandby::CheckTime()
{
	m_dwStartTick = CSWDateTime::GetSystemTick();
		
	while (m_fCountingDown)
	{	
		STANDBY_PARAM_STRUCT sSetting;

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

		if (!m_fSleeping
			&& dwTick > m_dwStartTick 
			&& (dwTick - m_dwStartTick)/1000 >= sSetting.iWaitTime //sec
			)
		{
			if (SWPAR_OK != swpa_camera_set_power(0))
			{	
				SW_TRACE_DEBUG("Err: failed to sleep camera.\n");
			}
			else
			{	
				SW_TRACE_DEBUG("Info: camera sleeping.\n");
				m_fSleeping = TRUE;
				m_dwStartTick = dwTick;
			}
		}

		swpa_thread_sleep_ms(1000);
	}
	
	m_fCountingDown = FALSE;
	return S_OK;	
}



PVOID CSWDomeCameraStandby::CheckTimeProxy(PVOID pvArg)
{
	if (NULL != pvArg)
	{
		CSWDomeCameraStandby * pThis = (CSWDomeCameraStandby*)pvArg;

		pThis->CheckTime();
	}
}

HRESULT CSWDomeCameraStandby::Update()
{
	if (m_fCountingDown)
	{
		m_dwStartTick = CSWDateTime::GetSystemTick();
	}
	
	if (m_fSleeping)
	{
		if (SWPAR_OK != swpa_camera_set_power(1))
		{	
			SW_TRACE_DEBUG("Err: failed to wake up camera.\n");
			return E_FAIL;
		}
		else
		{	
			SW_TRACE_DEBUG("Info: camera woke up.\n");
			m_fSleeping = FALSE;
		}
	}
	
	return S_OK;
}

HRESULT CSWDomeCameraStandby::SaveSetting()
{	
	GetSetting().UpdateInt("\\DomeCamera\\Standby", "Enable", GetSetting().Get().sStandbyParam.fEnable);
	GetSetting().UpdateInt("\\DomeCamera\\Standby", "WaitTime", GetSetting().Get().sStandbyParam.iWaitTime);

	return GetSetting().Commit();
}


