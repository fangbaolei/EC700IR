#include "SWDomeCameraHScan.h"

CSWDomeCameraHScan::CSWDomeCameraHScan()
{
}


CSWDomeCameraHScan::~CSWDomeCameraHScan()
{
	Stop(m_dwScanningID);
}


HRESULT CSWDomeCameraHScan::SetLeftPos(const DWORD& dwID)
{
	CHECK_ID(dwID, MAX_HSCAN_PATH);

	swpa_camera_ball_stop();
	CSWApplication::Sleep(200);
	
	GetSetting().Get().sHScanParam[dwID].iLeftPos = 1;

	//设置第N组扫描左边界: 调用80+调用pp+设置92,pp为组号：1-8
	if (SWPAR_OK != swpa_camera_ball_preset(PRESET_CALL, 80) || FAILED(CSWApplication::Sleep(500))
		|| SWPAR_OK != swpa_camera_ball_preset(PRESET_CALL, dwID+1) || FAILED(CSWApplication::Sleep(500))
		|| SWPAR_OK != swpa_camera_ball_preset(PRESET_SET, 92) || FAILED(CSWApplication::Sleep(10))
		)
	{
		SW_TRACE_DEBUG("Err: failed to set HScan #%d left position\n", dwID);
		return E_FAIL;
	}
	
	return SaveSetting(dwID);
}


HRESULT CSWDomeCameraHScan::GetLeftPos(const DWORD& dwID, INT& iLeftPos)
{
	CHECK_ID(dwID, MAX_HSCAN_PATH);
	
	HSCAN_PARAM_STRUCT sHScanParam;

	Get(dwID, sHScanParam);
	
	iLeftPos = sHScanParam.iLeftPos;
	
	return S_OK;
}


HRESULT CSWDomeCameraHScan::SetRightPos(const DWORD& dwID)
{
	CHECK_ID(dwID, MAX_HSCAN_PATH);

	swpa_camera_ball_stop();
	CSWApplication::Sleep(200);
	
	GetSetting().Get().sHScanParam[dwID].iRightPos = 1;

	//设置第N组扫描右边界: 调用80+调用pp+设置93,pp为组号：1-8
	if (SWPAR_OK != swpa_camera_ball_preset(PRESET_CALL, 80) || FAILED(CSWApplication::Sleep(500))
		|| SWPAR_OK != swpa_camera_ball_preset(PRESET_CALL, dwID+1) || FAILED(CSWApplication::Sleep(500))
		|| SWPAR_OK != swpa_camera_ball_preset(PRESET_SET, 93) || FAILED(CSWApplication::Sleep(10))
	)
	{
		SW_TRACE_DEBUG("Err: failed to set HScan #%d right position\n", dwID);
		return E_FAIL;
	}
	
	return SaveSetting(dwID);
}



HRESULT CSWDomeCameraHScan::GetRightPos(const DWORD& dwID, INT& iRightPos)
{
	CHECK_ID(dwID, MAX_HSCAN_PATH);
	
	HSCAN_PARAM_STRUCT sHScanParam;

	Get(dwID, sHScanParam);
	
	iRightPos = sHScanParam.iRightPos;
	
	return S_OK;
}


HRESULT CSWDomeCameraHScan::Set(const DWORD& dwID, const HSCAN_PARAM_STRUCT& sHScanParam )
{
	CHECK_ID(dwID, MAX_HSCAN_PATH);

	if (sHScanParam.iSpeed < 1 || sHScanParam.iSpeed > 30)
	{
		SW_TRACE_DEBUG("Err: invalid speed value %d\n",  sHScanParam.iSpeed);
		return E_INVALIDARG;
	}

	swpa_strncpy(GetSetting().Get().sHScanParam[dwID].szName, sHScanParam.szName, sizeof(sHScanParam.szName) - 1);
	GetSetting().Get().sHScanParam[dwID].iSpeed = sHScanParam.iSpeed;
	GetSetting().Get().sHScanParam[dwID].fValid = TRUE;

	SaveSetting(dwID);
	
	if (GetSetting().Get().sHScanParam[dwID].fValid
		&& m_dwScanningID == dwID)
	{
		Stop(dwID);
		SW_TRACE_DEBUG("tobedeleted: going to call %d\n",  dwID);
		if (FAILED(Call(dwID)))
		{
			SW_TRACE_DEBUG("Err: failed to call HScan #%d\n",  dwID);
			return E_FAIL;
		}
	}

	return S_OK;
}


HRESULT CSWDomeCameraHScan::Get(const DWORD& dwID, HSCAN_PARAM_STRUCT& sHScanParam )
{
	CHECK_ID(dwID, MAX_HSCAN_PATH);

	swpa_memcpy(&sHScanParam, &GetSetting().Get().sHScanParam[dwID], sizeof(sHScanParam));
	
	return S_OK;
}



HRESULT CSWDomeCameraHScan::Call(const DWORD& dwID)
{
	CHECK_ID(dwID, MAX_HSCAN_PATH);

	HSCAN_PARAM_STRUCT sHScanParam;

	Get(dwID, sHScanParam);
	
	if (!sHScanParam.fValid || 0 == sHScanParam.iLeftPos || 0 == sHScanParam.iRightPos)
	{
		SW_TRACE_DEBUG("Info: HScan #%d is not set (%d, %d, %d)\n", dwID, sHScanParam.fValid, sHScanParam.iLeftPos, sHScanParam.iRightPos);
		return E_FAIL;
	}

	m_dwScanningID = dwID;

	SW_TRACE_DEBUG("tobedeleted: iSpeed = %d\n", sHScanParam.iSpeed);

	swpa_camera_ball_stop();
	CSWApplication::Sleep(300);

	//设置第N组扫描速度: 调用80+调用pp+设置87+调用qq, pp为组号：1-8，qq为速度值：1-30
	if (SWPAR_OK != swpa_camera_ball_preset(PRESET_CALL, 80) || FAILED(CSWApplication::Sleep(100))
		|| SWPAR_OK != swpa_camera_ball_preset(PRESET_CALL, dwID+1) || FAILED(CSWApplication::Sleep(100))
		|| SWPAR_OK != swpa_camera_ball_preset(PRESET_SET, 87) || FAILED(CSWApplication::Sleep(100))
		|| SWPAR_OK != swpa_camera_ball_preset(PRESET_CALL, sHScanParam.iSpeed) || FAILED(CSWApplication::Sleep(100))
	)
	{
		SW_TRACE_DEBUG("Err: failed to set HScan #%d speed\n", dwID);
		return E_FAIL;
	}

	CSWApplication::Sleep(300);
	
	//启动第N组扫描	:调用80+调用pp+调用99, pp为组号：1-8
	if (SWPAR_OK != swpa_camera_ball_preset(PRESET_CALL, 80) || FAILED(CSWApplication::Sleep(100))
		|| SWPAR_OK != swpa_camera_ball_preset(PRESET_CALL, dwID+1) || FAILED(CSWApplication::Sleep(100))
		|| SWPAR_OK != swpa_camera_ball_preset(PRESET_CALL, 99) || FAILED(CSWApplication::Sleep(100))
	)
	{
		SW_TRACE_DEBUG("Err: failed to call HScan #%d\n", dwID);
		return E_FAIL;
	}
	
	return S_OK;
}


HRESULT CSWDomeCameraHScan::Stop(const DWORD& dwID)
{
	CHECK_ID(dwID, MAX_HSCAN_PATH);

	if (dwID != m_dwScanningID)
	{
		SW_TRACE_DEBUG("Warning: HScan #%d is not scanning...\n", dwID);
		return S_OK;
	}

	m_dwScanningID = MAX_HSCAN_PATH;

	if (SWPAR_OK != swpa_camera_ball_stop())
	{
		SW_TRACE_DEBUG("Err: failed to stop HScan #%d\n", dwID);
		return E_INVALIDARG;
	}
	
	return S_OK;
}




HRESULT CSWDomeCameraHScan::Clear(const DWORD& dwID)
{
	CHECK_ID(dwID, MAX_HSCAN_PATH);

	Stop(dwID);
	
	swpa_memset(&GetSetting().Get().sHScanParam[dwID], 0, sizeof(GetSetting().Get().sHScanParam[dwID]));
	swpa_snprintf(GetSetting().Get().sHScanParam[dwID].szName, 31, "水平扫描%d", dwID);
	GetSetting().Get().sHScanParam[dwID].iSpeed = 15;//default
	GetSetting().Get().sHScanParam[dwID].fValid = TRUE; //default

	return SaveSetting(dwID);
}


HRESULT CSWDomeCameraHScan::SaveSetting(const DWORD& dwID)
{
	CHECK_ID(dwID, MAX_HSCAN_PATH);

	char szSection[256];
    swpa_sprintf(szSection, "\\DomeCamera\\HScan\\Path%d", dwID);
		
	GetSetting().UpdateString(szSection, "Name", GetSetting().Get().sHScanParam[dwID].szName);
	GetSetting().UpdateInt(szSection, "LeftPos", GetSetting().Get().sHScanParam[dwID].iLeftPos);
	GetSetting().UpdateInt(szSection, "RightPos", GetSetting().Get().sHScanParam[dwID].iRightPos);
	GetSetting().UpdateInt(szSection, "Speed", GetSetting().Get().sHScanParam[dwID].iSpeed);
	GetSetting().UpdateInt(szSection, "Direction", GetSetting().Get().sHScanParam[dwID].iDirection);
	GetSetting().UpdateInt(szSection, "Valid", GetSetting().Get().sHScanParam[dwID].fValid);

	return GetSetting().Commit();
}


