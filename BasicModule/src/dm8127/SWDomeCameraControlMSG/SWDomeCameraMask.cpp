#include "SWDomeCameraMask.h"
#include "SWMessage.h"



CSWDomeCameraMask::CSWDomeCameraMask()
{
    m_fInit = FALSE;
}


CSWDomeCameraMask::~CSWDomeCameraMask()
{
    m_fInit = FALSE;
}


PVOID CSWDomeCameraMask::InitMasks(PVOID pvArg)
{
	/*
	因为机芯和球机都不保存隐私区域信息，掉电即失，
	因此需要应用层保存隐私区域的PTZ信息并在每次上电时设置到机芯；
	又因为球机要负责与机芯同步PT，因此需要调用球机的接口来触发同步开始
	而调用球机接口设置隐私区域，又需要转到正确的位置，因此需要调用关联的预制位
	而且，需要先设置到机芯，然后再触发，所以这个函数就成这个样子了。
	*/
	CSWDomeCameraMask* pThis = (CSWDomeCameraMask*)pvArg;

	
	swpa_camera_basicparam_clear_display(-1);

	INT iFirstID = -1;

	MASK_PARAM_STRUCT sSetting;
	for (INT i=0; i<MAX_MASK; i++)
    {	         
        pThis->Get(i, sSetting);

        if (sSetting.fValid && sSetting.fEnable)
        {
        	if (iFirstID < 0)
        	{
        		iFirstID = i;
        	}
			else
			{
				swpa_camera_basicparam_set_ptz(i, sSetting.iMaskPan, sSetting.iMaskTilt, sSetting.iMaskZoom);
				swpa_camera_basicparam_set_mask(i, sSetting.iWidth, sSetting.iHeight, 1);	
				swpa_camera_basicparam_set_display(i, 0);
			}
        }
		else
		{
			swpa_camera_basicparam_clear_display(i);
			swpa_camera_basicparam_set_mask(i, 0, 0, 1);
			swpa_camera_ball_set_privacyzone(i, 0, 0, 0);
		}
    }

	if (iFirstID >= 0)
	{
		pThis->Get(iFirstID, sSetting);
		
	    if (FAILED(CSWMessage::SendMessage(MSG_CALL_PRESET_POS, (WPARAM)sSetting.iPresetID, (LPARAM)1)))
	    {
	    	SW_TRACE_DEBUG("Err: failed to call Preset #%d to show Mask #%d\n", sSetting.iPresetID, iFirstID);
	    }
		else
		{
			DWORD dwNowTick = CSWDateTime::GetSystemTick();
			pThis->Show(iFirstID);
		}
    }
}


HRESULT CSWDomeCameraMask::Initialize(CSWDomeCameraSetting * pcSetting)
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

	if (FAILED(m_cInitThread.Start(InitMasks, this)))
	{
		SW_TRACE_DEBUG("Err: failed to init masks\n");
        return E_FAIL;
	}    

    m_fInit = TRUE;

    SW_TRACE_DEBUG("Info: CSWDomeCameraMask init -- ok.\n");

    return S_OK;
}



HRESULT CSWDomeCameraMask::Set(const DWORD& dwID, const MASK_PARAM_STRUCT& sSetting)
{
    CHECK_ID(dwID, MAX_MASK);

    swpa_memcpy(&GetSetting().Get().sMaskParam[dwID], &sSetting, sizeof(GetSetting().Get().sMaskParam[dwID]));

	if (FAILED(Show(dwID)))
	{
		SW_TRACE_DEBUG("Err: failed to show Mask #%d\n", dwID);
        return E_FAIL;
	}

	INT iPan = -1, iTilt = -1, iZoom = -1;
	swpa_camera_basicparam_get_pt(&iPan, &iTilt);
	GetSetting().Get().sMaskParam[dwID].iMaskPan = iPan;
	GetSetting().Get().sMaskParam[dwID].iMaskTilt = iTilt;
	swpa_camera_basicparam_get_zoom_value(&iZoom);
	GetSetting().Get().sMaskParam[dwID].iMaskZoom = iZoom;

    return SaveSetting(dwID);    
}


HRESULT CSWDomeCameraMask::Show(const DWORD& dwID)
{
    CHECK_ID(dwID, MAX_MASK);

    MASK_PARAM_STRUCT sSetting;
    Get(dwID, sSetting);

    if (!sSetting.fValid)
    {
        SW_TRACE_DEBUG("Info: Mask #%d is not set\n", dwID);
        return E_INVALIDARG;
    }
    
    if (!sSetting.fEnable)
    {
        SW_TRACE_DEBUG("Info: Mask #%d is not enabled\n", dwID);
        return S_OK;
    }


    SW_TRACE_DEBUG("Info: display Mask #%d ...\n", dwID);

	if (SWPAR_OK != swpa_camera_ball_set_privacycoord(
			dwID, 
			((sSetting.iLeft * 12)+((sSetting.iWidth * 12)>>1))*100/1920, //50
			((sSetting.iTop * 12)+((sSetting.iHeight * 12)>>1))*100/1080) //50
		)
    {
        SW_TRACE_DEBUG("Info: failed to set mask coord #%d\n", dwID);
        return E_FAIL;
    }

	swpa_thread_sleep_ms(50);
	SW_TRACE_DEBUG("---tobemarked: Mask #%d info (%d, %d, %d, %d)\n", dwID, sSetting.iLeft, sSetting.iTop, sSetting.iWidth, sSetting.iHeight);
	if (SWPAR_OK != swpa_camera_ball_set_privacyzone(
			dwID, 1, 
			sSetting.iWidth >> 1,//sSetting.iWidth*100/1920, 
			sSetting.iHeight >> 1//sSetting.iHeight*100/1080
		))
    {    
        SW_TRACE_DEBUG("Info: failed to set mask #%d\n", dwID);
        return E_FAIL;
    }

	//swpa_camera_basicparam_set_mask(dwID, sSetting.iWidth, sSetting.iHeight, 1);	
	swpa_camera_basicparam_set_display(dwID, 0);
    
    return S_OK;
}


HRESULT CSWDomeCameraMask::Hide(const DWORD& dwID)
{
    CHECK_ID(dwID, MAX_MASK+1);

    MASK_PARAM_STRUCT sSetting;
    Get(dwID, sSetting);

    if (!sSetting.fValid)
    {
        SW_TRACE_DEBUG("Info: Mask #%d is not set\n", dwID);
        return S_OK;
    }
    
    if (!sSetting.fEnable)
    {
        SW_TRACE_DEBUG("Info: Mask #%d is not enabled\n", dwID);
        return S_OK;
    }

    if (MAX_MASK == dwID)
    {    
        if (SWPAR_OK != swpa_camera_basicparam_clear_display(-1))
        {    
            SW_TRACE_DEBUG("Err: failed to clear all masks\n", dwID);
            return E_FAIL;
        }
    }
    else
    {
    	if (SWPAR_OK != swpa_camera_basicparam_clear_display(dwID))
        {    
            SW_TRACE_DEBUG("Err: failed to clear mask display (id = %d)\n", dwID);
            return E_FAIL;
        }
		
		if (SWPAR_OK != swpa_camera_ball_set_privacyzone(dwID, 0, 
			0, 
			0)
			)
        {    
            SW_TRACE_DEBUG("Info: failed to hide mask #%d\n", dwID);
            return E_FAIL;
        }
    }
    
    return S_OK;
}


HRESULT CSWDomeCameraMask::Clear(const DWORD& dwID)
{
    CHECK_ID(dwID, MAX_MASK+1);

	Hide(dwID);

    INT iStart = (MAX_MASK == dwID) ? 0 : dwID;
    INT iMax = (MAX_MASK == dwID) ? MAX_MASK : dwID + 1;

    for (INT i=iStart; i<iMax; i++)
    {
    	swpa_camera_basicparam_set_mask(i, 0, 0, 1);
		
        swpa_memset(&GetSetting().Get().sMaskParam[i], 0, sizeof(GetSetting().Get().sMaskParam[i]));
        GetSetting().Get().sMaskParam[i].fEnable = FALSE;
        GetSetting().Get().sMaskParam[i].iPresetID = -1;
        swpa_snprintf(GetSetting().Get().sMaskParam[i].szName, 31, "隐私区域%d", i);
    }
    
    return SaveSetting(dwID);
}


HRESULT CSWDomeCameraMask::Get(const DWORD& dwID, MASK_PARAM_STRUCT& sSetting)
{
    CHECK_ID(dwID, MAX_MASK);

    swpa_memcpy(&sSetting, &GetSetting().Get().sMaskParam[dwID], sizeof(sSetting));

    return S_OK;
}



HRESULT CSWDomeCameraMask::SaveSetting(const DWORD& dwID)
{
    INT iID = (INT)dwID;
    CHECK_ID(iID, MAX_MASK);

    INT iStart = (-1 == iID) ? 0 : iID;
    INT iMax = (-1 == iID) ? MAX_MASK : iID + 1;

    for (INT i=iStart; i<iMax; i++)
    {
        CHAR szSection[256];
        swpa_sprintf(szSection, "\\DomeCamera\\Mask\\Area%d", iID);

        GetSetting().UpdateString(szSection, "Name", GetSetting().Get().sMaskParam[i].szName);
        GetSetting().UpdateInt(szSection, "Top", GetSetting().Get().sMaskParam[i].iTop);
        GetSetting().UpdateInt(szSection, "Left", GetSetting().Get().sMaskParam[i].iLeft);
        GetSetting().UpdateInt(szSection, "Height", GetSetting().Get().sMaskParam[i].iHeight);
        GetSetting().UpdateInt(szSection, "Width", GetSetting().Get().sMaskParam[i].iWidth);
        GetSetting().UpdateInt(szSection, "MaskPan", GetSetting().Get().sMaskParam[i].iMaskPan);
        GetSetting().UpdateInt(szSection, "MaskTilt", GetSetting().Get().sMaskParam[i].iMaskTilt);
        GetSetting().UpdateInt(szSection, "MaskZoom", GetSetting().Get().sMaskParam[i].iMaskZoom);
        GetSetting().UpdateInt(szSection, "Color", GetSetting().Get().sMaskParam[i].iColor);
        GetSetting().UpdateInt(szSection, "Enable", GetSetting().Get().sMaskParam[i].fEnable);
        GetSetting().UpdateInt(szSection, "RelatedPreset", GetSetting().Get().sMaskParam[i].iPresetID);
        GetSetting().UpdateInt(szSection, "Valid", GetSetting().Get().sMaskParam[i].fValid);
    }
    
    return GetSetting().Commit();
}


