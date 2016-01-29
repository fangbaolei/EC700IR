#include "SWDomeCameraPreset.h"
#include "SWMessage.h"

CSWDomeCameraPreset::CSWDomeCameraPreset()
{
    m_dwCallingID = MAX_PRESET;
}


CSWDomeCameraPreset::~CSWDomeCameraPreset()
{
}


HRESULT CSWDomeCameraPreset::Set(const DWORD& dwID, const PRESETPOS_PARAM_STRUCT& sParam)
{
    CHECK_ID(dwID, MAX_PRESET);

    if (FUNC_ZERO_CORRECTION == dwID ||
		FUNC_DEFOG_START == dwID) //0°校正位，固定
    {
        return S_OK;
    }

    
    INT iPanCoord = 0, iTiltCoord = 0, iZoom = 0, iFocus = 0, iIris = 0;
    AF_MODE iFocusMode = AF_AUTO;

    if (SWPAR_OK != swpa_camera_ball_get_pt_coordinates(&iPanCoord, &iTiltCoord)
        || SWPAR_OK != swpa_camera_basicparam_get_zoom_value(&iZoom)
        || SWPAR_OK != swpa_camera_basicparam_get_focus_value(&iFocus)
        //|| SWPAR_OK != swpa_camera_basicparam_get_iris(&iIris)
        || SWPAR_OK != swpa_camera_basicparam_get_AF(&iFocusMode)
        )
    {
        SW_TRACE_DEBUG("Err: failed to get preset dome camera params\n");
        return E_FAIL;
    }

    swpa_memset(GetSetting().Get().sPresetParam[dwID].szName, 0, sizeof(GetSetting().Get().sPresetParam[dwID].szName));
    swpa_strncpy(GetSetting().Get().sPresetParam[dwID].szName, sParam.szName, sizeof(GetSetting().Get().sPresetParam[dwID].szName) - 1);
    GetSetting().Get().sPresetParam[dwID].iPanCoordinate = iPanCoord;
    GetSetting().Get().sPresetParam[dwID].iTiltCoordinate = iTiltCoord;
    GetSetting().Get().sPresetParam[dwID].iZoom = iZoom;
    //GetSetting().Get().sPresetParam[dwID].iIris = iIris;
    GetSetting().Get().sPresetParam[dwID].iFocus = iFocus;
    GetSetting().Get().sPresetParam[dwID].iFocusMode = (INT)iFocusMode;
    GetSetting().Get().sPresetParam[dwID].fCheckPT = sParam.fCheckPT;
    GetSetting().Get().sPresetParam[dwID].fValid = TRUE;

    return SaveSetting(dwID);
}


HRESULT CSWDomeCameraPreset::Call(const DWORD& dwID)
{
    CHECK_ID(dwID, MAX_PRESET);

    if (FUNC_ZERO_CORRECTION == dwID) //0°校正位，固定
    {
        if (FAILED(swpa_camera_ball_calibration()))//swpa_camera_ball_remote_reset();
        {
            SW_TRACE_DEBUG("Err: failed to calibrate dome\n");
            return E_FAIL;
        }
        return S_OK;
    }
	else if(FUNC_DEFOG_START == dwID)
	{
        return S_OK;
	}


    if (!GetSetting().Get().sPresetParam[dwID].fValid)
    {
        SW_TRACE_DEBUG("Info: Prest#%d is not set\n", dwID);
        return S_OK;
    }

    m_dwCallingID = dwID;

    //picture freeze
    PICTUREFREEZE_PARAM_STRUCT sFreezeSetting;
    GetPictureFreezeInstance().Get(sFreezeSetting);
    if (sFreezeSetting.fEnable)
    {
        GetPictureFreezeInstance().Freeze(TRUE);
    }

    
    INT iPanCoord = GetSetting().Get().sPresetParam[dwID].iPanCoordinate, 
        iTiltCoord = GetSetting().Get().sPresetParam[dwID].iTiltCoordinate, 
        iZoom = GetSetting().Get().sPresetParam[dwID].iZoom, 
        iFocus = GetSetting().Get().sPresetParam[dwID].iFocus;
    //INT iIris = GetSetting().Get().sPresetParam[dwID].iIris;
    AF_MODE    iFocusMode = (AF_MODE)GetSetting().Get().sPresetParam[dwID].iFocusMode;
    

    INT iTryCount = 3;
	HRESULT hr = E_FAIL;
    while (iTryCount-- > 0)
    {
    	if (m_dwCallingID != dwID)
    	{
    	    SW_TRACE_DEBUG("Info: stop calling Preset #%d.\n", dwID);
			hr = E_ABORT;
			break;
    	}
		
        if (SUCCEEDED(GotoPos(iPanCoord, iTiltCoord, iZoom, iFocusMode, iFocus)))
        {
        	hr = S_OK;
            break;
        }
    }
    
    //picture de-freeze
    if (sFreezeSetting.fEnable)
    {
        // 延迟200ms后再还原冻结。
        //CSWApplication::Sleep(200);
        if (FAILED(GetPictureFreezeInstance().Freeze(FALSE)))
        {
        	SW_TRACE_DEBUG("Err: failed to de-freeze image\n");
			return E_FAIL;
        }
		CSWApplication::Sleep(200);
    }

    SW_TRACE_DEBUG("Info: call Prest #%d -- %s\n", dwID, (hr == S_OK) ? "OK" : "FAILED");
    
    return hr;
}


HRESULT CSWDomeCameraPreset::Stop(const DWORD& dwID)
{
    if (MAX_PRESET != dwID)
    {
        if (dwID != m_dwCallingID)
        {
            return S_OK;
        }
    }

    m_dwCallingID = MAX_PRESET;

    return S_OK;
}


HRESULT CSWDomeCameraPreset::GotoPos(
    const INT& iPanCoord, const INT& iTiltCoord, const INT& iZoomValue, 
    const INT& iFocusMode, const INT& iFocusValue)
{	
#if 0
    if (SWPAR_OK != swpa_camera_basicparam_set_zoom_value(iZoomValue))
    {
        SW_TRACE_DEBUG("Err: failed to set zoom to %d\n", iZoomValue);
        return E_FAIL;
    }
    if (SWPAR_OK != swpa_camera_ball_set_pt_coordinates(iPanCoord, iTiltCoord))
    {
        SW_TRACE_DEBUG("Err: failed to set pt to (%d, %d)\n", iPanCoord, iTiltCoord);
        return E_FAIL;
    }   
    if (SWPAR_OK != swpa_camera_basicparam_set_AF((AF_MODE)iFocusMode))
    {
        SW_TRACE_DEBUG("Err: failed to set AF to %d\n", iFocusMode);
        return E_FAIL;
    }   

    if (AF_MANUAL == iFocusMode)
    {
        if (SWPAR_OK != swpa_camera_basicparam_set_focus_value(iFocusValue))
        {
            SW_TRACE_DEBUG("Err: failed to set focus value %d\n", iFocusValue);
            return E_FAIL;
        }
    }

    BOOL fOK = FALSE;
    INT iTryCount = 10;
    do 
    {
        CSWApplication::Sleep(500);

        INT iPan = -1, iTilt = -1, iZoom = -1, iAF = -1, iFocus = -1;
        swpa_camera_ball_get_pt_coordinates(&iPan, &iTilt);
        swpa_camera_basicparam_get_zoom_value(&iZoom);
		//SW_TRACE_DEBUG("---tobemarked: preset goto zoom %d, now %d\n", iZoomValue, iZoom);
		swpa_thread_sleep_ms(50);
		swpa_camera_ball_sync_zoom();
		swpa_thread_sleep_ms(50);
        swpa_camera_basicparam_get_AF((AF_MODE*)&iAF);
        swpa_camera_basicparam_get_focus_value(&iFocus);

        BOOL fPanOK = FALSE, fTiltOK = FALSE, fZoomOK = FALSE, fAFOK = FALSE, fFocusOK = FALSE;
        fPanOK = (swpa_abs(iPanCoord - iPan) < 5);
        if (!fPanOK && iPanCoord > 3595)
        {
            fPanOK = (iPan > 0 && iPan < 5 && swpa_abs(3600 + iPan - iPanCoord));
        }
        else if (!fPanOK && iPanCoord < 5)
        {
            fPanOK = (iPan > 3595 && swpa_abs(3600 + iPanCoord - iPan));
        }

        fTiltOK = (swpa_abs(iTiltCoord - iTilt) < 5);
        fZoomOK = (swpa_abs(iZoomValue - iZoom) < 5);
        fAFOK = iFocusMode == iAF;
        fFocusOK = iFocusMode == (AF_MANUAL) ? (swpa_abs(iFocusValue - iFocus) < 5) : TRUE;
        
        fOK = fPanOK && fTiltOK && fZoomOK && fAFOK && fFocusOK;
        iTryCount --;

        //SW_TRACE_DEBUG("---tobemarked: (%d, %d, %d, %d, %d)\n", iPan, iTilt, iZoom, iAF, iFocus);
        //SW_TRACE_DEBUG("---tobemarked: (%d, %d, %d, %d, %d)\n", fPanOK, fTiltOK, fZoomOK, fAFOK, fFocusOK);
    } while ( !fOK && iTryCount != 0);

    if (0 == iTryCount && !fOK)
    {
        return E_FAIL;
    }

	swpa_camera_ball_sync_zoom();
#else

	POS_PARAM_STRUCT sPos;
	sPos.iPanCoordinate = iPanCoord;
	sPos.iTiltCoordinate = iTiltCoord;
	sPos.iZoom = iZoomValue;
	sPos.iFocus = iFocusValue;
	sPos.iFocusMode = iFocusMode;
	if (FAILED(SetPosParam(sPos)))
	{
		SW_TRACE_DEBUG("Err: failed to set position param.\n");
		return E_FAIL;
	}
#endif

    return S_OK;
}




HRESULT CSWDomeCameraPreset::Clear(const DWORD& dwID)
{
    CHECK_ID(dwID, MAX_PRESET);
    
    swpa_memset(&GetSetting().Get().sPresetParam[dwID], 0, sizeof(GetSetting().Get().sPresetParam[dwID]));
    swpa_snprintf(GetSetting().Get().sPresetParam[dwID].szName, 31, "预置位%d", dwID);
    GetSetting().Get().sPresetParam[dwID].fValid = TRUE;
    GetSetting().Get().sPresetParam[dwID].iFocusMode = AF_AUTO;
    GetSetting().Get().sPresetParam[dwID].iPanCoordinate = 14*dwID;
    GetSetting().Get().sPresetParam[dwID].iTiltCoordinate = 50;//Version B dome fw, 0~950

    if (FUNC_ZERO_CORRECTION == dwID)
    {    
        swpa_snprintf(GetSetting().Get().sPresetParam[dwID].szName, 31, "0°校正");
    }
	else if(FUNC_DEFOG_START == dwID)
	{
		swpa_snprintf(GetSetting().Get().sPresetParam[dwID].szName, 31, "启动除雾");
	}
    
    return SaveSetting(dwID);
}


HRESULT CSWDomeCameraPreset::Get(const DWORD& dwID, PRESETPOS_PARAM_STRUCT& sParam)
{
    CHECK_ID(dwID, MAX_PRESET);

    swpa_memcpy(&sParam, &GetSetting().Get().sPresetParam[dwID], sizeof(sParam));

    return S_OK;
}



HRESULT CSWDomeCameraPreset::SaveSetting(const DWORD& dwID)
{
    CHECK_ID(dwID, MAX_PRESET);
    
    CHAR szSection[256];
    swpa_sprintf(szSection, "\\DomeCamera\\Preset\\Pos%d", dwID);


    GetSetting().UpdateString(szSection, "Name", GetSetting().Get().sPresetParam[dwID].szName);
    GetSetting().UpdateInt(szSection, "PanCoordinate", GetSetting().Get().sPresetParam[dwID].iPanCoordinate);
    GetSetting().UpdateInt(szSection, "TiltCoordinate", GetSetting().Get().sPresetParam[dwID].iTiltCoordinate);
    GetSetting().UpdateInt(szSection, "Zoom", GetSetting().Get().sPresetParam[dwID].iZoom);
    GetSetting().UpdateInt(szSection, "Iris", GetSetting().Get().sPresetParam[dwID].iIris);
    GetSetting().UpdateInt(szSection, "Focus", GetSetting().Get().sPresetParam[dwID].iFocus);
    GetSetting().UpdateInt(szSection, "FocusMode", GetSetting().Get().sPresetParam[dwID].iFocusMode);
    GetSetting().UpdateInt(szSection, "CheckPT", GetSetting().Get().sPresetParam[dwID].fCheckPT);
    GetSetting().UpdateInt(szSection, "Valid", GetSetting().Get().sPresetParam[dwID].fValid);
    
    return GetSetting().Commit();
}


