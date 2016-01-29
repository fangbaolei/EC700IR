// 该文件编码格式必须是WIN936

#include "SWFC.h"
#include "SWDomeCameraControlMSG.h"
#include "SWCameraControlParameter.h"
#include "SWPTZParameter.h"



#define HANDLEOPERATION(op, arg) \
if (FAILED(HandleOperation(op, arg)))\
{\
	SW_TRACE_DEBUG("Err: failed to handle user operation %d\n", (INT)op);\
	return E_FAIL;\
}\

#define CHECKFSCANSTATUS()\
if (m_dwFScanRecordingID != MAX_FSCAN_PATH || !m_cFScan.IsIdle())	\
{\
	SW_TRACE_DEBUG("Err: FScan is busy, %s refused\n", __FUNCTION__);\
	return E_ACCESSDENIED;\
}

REGISTER_CLASS(CSWDomeCameraControlMSG)//!!!!!!!!!!!!!!!!!!!!!!!! testonly //!!!!!!!!!!!!!!!!!!!!!!!!


CSWDomeCameraControlMSG::CSWDomeCameraControlMSG()
    : CSWMessage( MSG_CAMERA_CONTROL_START, MSG_CAMERA_CONTROL_END )
    , m_iLedMode(0)

{
    m_fInited = FALSE;
    m_dwFScanRecordingID = MAX_FSCAN_PATH;
    m_fInfinityPanStarted = FALSE;
    m_fVTurnOver = FALSE;
	m_fRepeatPanTilt = FALSE;
	m_fSyncZoom = FALSE;
	m_fDefogStarted = FALSE;
}

CSWDomeCameraControlMSG::~CSWDomeCameraControlMSG()
{
    swpa_camera_deinit();

    m_fInited = FALSE;
    m_dwFScanRecordingID = MAX_FSCAN_PATH;
    m_fInfinityPanStarted = FALSE;
	m_fDefogStarted = FALSE;
    m_cInfinityPanThread.Stop();
	m_cDefogThread.Stop();
	SAFE_RELEASE(m_pCamera);
}

HRESULT CSWDomeCameraControlMSG::OnResetDefaultDomeCamera(WPARAM wParam,LPARAM lParam)
{
	OnClearMask((WPARAM)MAX_MASK, 0);
    OnResetDomePosition(0, 0);
	
    return m_cSetting.ResetDefault();
}


HRESULT CSWDomeCameraControlMSG::OnRestoreFactorySetting(WPARAM wParam,LPARAM lParam)
{
	OnClearMask((WPARAM)MAX_MASK, 0);
	
    if (SWPAR_OK != swpa_camera_reset()
		|| SWPAR_OK != swpa_camera_ball_restore_factory_settings())
    {
    	SW_TRACE_DEBUG("Err: failed to restore factory setting\n");
		return E_FAIL;
    }
    return m_cSetting.ResetDefault();
}


HRESULT CSWDomeCameraControlMSG::OnInitialDomeCamera( WPARAM wParam,LPARAM lParam )
{
    if (0 == wParam)
    {
        SW_TRACE_DEBUG("Err: invalid arg wParam\n");
        return E_INVALIDARG;
    }

    
    LPCAMERAPARAM_INFO psParam = (LPCAMERAPARAM_INFO)wParam;
	CHAR CameraVersion[8] = {0};
	SHORT nVendorID = 0, nModelID = 0, nROMVersion = 0;;
	INT iCamera = -1;

    if (SWPAR_OK != swpa_camera_init())
    {    
        SW_TRACE_DEBUG("Err: failed to init camera\n");
        return E_FAIL;
    }

    if (SWPAR_OK != swpa_camera_set_power(1))
    {    
        SW_TRACE_DEBUG("Err: failed to set camera power to 1\n");
        return E_FAIL;
    }    

    // 读取机芯初始参数，判断是否需要设置，一般在生产时已设置。
    // 需断电上电才能生效。
    MONITOR_MODE  mMode;
    int iSyncMode;
    if( 0 != swpa_camera_get_monitoring_mode(&mMode)
            || mMode != MODE_1080P_25 )
    {
        SW_TRACE_DEBUG("Err: failed to get monitoring_mode %d.\n", MODE_1080P_25);
    }
    swpa_camera_set_monitoring_mode(MODE_1080P_25);

    if( 0 != swpa_camera_get_sync_mode(&iSyncMode)
            || iSyncMode != 0)
    {
        SW_TRACE_DEBUG("Err: failed to get sync_mode 0.\n");
    }
    swpa_camera_set_sync_mode(0);

    // 禁止数字变焦
    swpa_camera_basicparam_dzoom_enable(0);

    // 默认使能High Resulotion模式
    if (SWPAR_OK != swpa_camera_basicparam_set_hrmode(1))
    {
        SW_TRACE_DEBUG("Err: failed to set HR mode to %d!\n", 1);
        return E_FAIL;
    }

    // 默认使能曝光补偿模式
    if (SWPAR_OK != swpa_camera_basicparam_set_expcomp_mode(1))
    {
        SW_TRACE_DEBUG("Err: failed to set exposure compensation mode to %d!\n", 1);
        return E_FAIL;
    }


    CSWString strFilePath("/mnt/nand/DomeCameraSetting.dat");
    if (FAILED(m_cSetting.Initialize(strFilePath)))
    {
        SW_TRACE_DEBUG("Err: failed to init dome settings\n");
        return E_FAIL;
    }

    
    if (FAILED(m_cSetting.Commit()))
    {
        SW_TRACE_DEBUG("Err: failed to commit dome settings\n");
        return E_FAIL;
    }
    
    if (   FAILED(m_cPictureFreeze.Initialize(&m_cSetting))
        || FAILED(m_cPreset.Initialize(&m_cSetting))
        || FAILED(m_cPreset.SetPictureFreezeInstance(&m_cPictureFreeze))
        || FAILED(m_cMask.Initialize(&m_cSetting))
        || FAILED(m_cHScan.Initialize(&m_cSetting))
        || FAILED(m_cFScan.Initialize(&m_cSetting))
        || FAILED(m_cCruise.Initialize(&m_cSetting))
        || FAILED(m_cStandby.Initialize(&m_cSetting))
        || FAILED(m_cWatchKeeping.Initialize(&m_cSetting))
        || FAILED(m_cTimer.Initialize(&m_cSetting))
        )
    {
        SW_TRACE_DEBUG("Err: failed to init dome camera\n");
        return E_FAIL;
    }


	OnGetCameraVersion(0, (LPARAM)CameraVersion);
	nVendorID = (CameraVersion[0] << 8) | CameraVersion[1];
	nModelID = (CameraVersion[2] << 8) | CameraVersion[3];
	nROMVersion =  (CameraVersion[4] << 8) | CameraVersion[5];
	SW_TRACE_DEBUG("VenderID:%04x ModelID:%04x ROMVersoin:%04x\n",
				   nVendorID, nModelID, nROMVersion);
				  

	m_pCamera = new CSWCamera();
	if(NULL == m_pCamera)
	{
		SW_TRACE_DEBUG("Err: out of memory.\n");
		return E_OUTOFMEMORY;
	}
	if(FAILED(m_pCamera->Initialize(nVendorID, nModelID, nROMVersion)))
	{
		SW_TRACE_DEBUG("Err: camera initialize error.\n");
		return E_FAIL;
	}
	


    // 一些初始状态的设置
    // 初始冻结功能无效
    m_cPictureFreeze.Freeze(FALSE);

    m_cCruise.SetPresetInstance(&m_cPreset);
    m_cCruise.SetHScanInstance(&m_cHScan);
    m_cCruise.SetFScanInstance(&m_cFScan);

    m_cWatchKeeping.SetPresetInstance(&m_cPreset);
    m_cWatchKeeping.SetHScanInstance(&m_cHScan);
    m_cWatchKeeping.SetFScanInstance(&m_cFScan);
    m_cWatchKeeping.SetCruiseInstance(&m_cCruise);
    
    m_cTimer.SetPresetInstance(&m_cPreset);
    m_cTimer.SetHScanInstance(&m_cHScan);
    m_cTimer.SetFScanInstance(&m_cFScan);
    m_cTimer.SetCruiseInstance(&m_cCruise);
    

    //相机参数设置
    OnSetAEMode(WPARAM(psParam->iAEMode), 0);
    OnSetAWBMode(WPARAM(psParam->iAWBMode), 0);
    OnSetShutter(WPARAM(psParam->iShutter), 0);
    OnSetAGCGain(WPARAM(psParam->iGain), 0);
    OnSetIris(WPARAM(psParam->iIris), 0);
    OnSetGammaMode(WPARAM(psParam->iGammaValue), 0);
    OnSetWDRStrength(WPARAM(psParam->iWDREnable), 0);
    OnSetAFEnable(WPARAM(psParam->iAutoFocus), 0);
    OnSetSaturationThreshold(WPARAM(psParam->iSaturationValue), 0);
    OnSetNRLevel(WPARAM(psParam->iNRLevel), 0);
    OnSetSharpeThreshold(WPARAM(psParam->iSharpenThreshold), 0);
    OnSetEdgeEnhance(WPARAM(psParam->iEdgeValue), 0);

    //SW_TRACE_DEBUG("---tobedeleted: iLEDMode = %d\n", GetSetting().Get().iLEDMode);
    OnSetLedMode(GetSetting().Get().iLEDMode, 0);
	OnSetDefog((WPARAM)0, 0);			//停止鼓风


    

    SW_TRACE_DEBUG("OK to init dome camera\n");

    m_fInited = TRUE;

    HANDLEOPERATION(0, 0);

    DWORD dwSNFEnable = FALSE, dwTNFEnable=FALSE;
    if( 0 != swpa_ipnc_control(0, CMD_SET_TNF_STATE ,(void*)&dwTNFEnable , sizeof(DWORD), 0)
        || 0 != swpa_ipnc_control(0, CMD_SET_SNF_STATE ,(void*)&dwSNFEnable , sizeof(DWORD), 0)
        )
    {
        return E_FAIL;
    }

    //OnResetDomePosition(0,0);
    //swpa_camera_reset();

	
	//默认 不使能防抖功能
    if (SWPAR_OK != swpa_camera_basicparam_set_stabilizer(0))
    {
    	SW_TRACE_DEBUG("err: failed to enable stabilizer!\n");
    }

	// 同步焦距
    if (SWPAR_OK != swpa_camera_ball_sync_zoom())
    {
    	swpa_thread_sleep_ms(200);
		if (SWPAR_OK != swpa_camera_ball_sync_zoom())
		{
    		SW_TRACE_DEBUG("err: failed to sync zoom!\n");
		}
    }
    
    return S_OK;
}


HRESULT CSWDomeCameraControlMSG::SetLastTask(const INT& iTask, const INT& iTaskID)
{
    GetSetting().Get().iLastTask = iTask;
    GetSetting().Get().iLastTaskID = iTaskID;

    return S_OK;

    // 不进行参数保存。
    /*
    GetSetting().UpdateInt("\\DomeCamera\\Basic", "LastTask", iTask);
    GetSetting().UpdateInt("\\DomeCamera\\Basic", "LastTaskID", iTaskID);

    return GetSetting().Commit();
    */
}



HRESULT CSWDomeCameraControlMSG::GetLastTask(INT& iTask, INT& iTaskID)
{
    INT iSleeping = -1;
    m_cStandby.GetStatus(iSleeping);
    if (iSleeping)
    {
        iTask = MOVETYPE_STANDBY;
        iTaskID = 0;
    }
	else if (m_dwFScanRecordingID != MAX_FSCAN_PATH)
	{
		iTask = MOVETYPE_FSCANRECORD;
		iTaskID = m_dwFScanRecordingID;
	}
    else
    {
        iTask = GetSetting().Get().iLastTask;
        iTaskID = GetSetting().Get().iLastTaskID;
    }

    return S_OK;
}


HRESULT CSWDomeCameraControlMSG::SwitchTask(const BOOL& fSwitchOn, const INT& iTask, const INT& iArg)
{
    static INT iCurTask = -1;
    static INT iCurTaskID = -1;

    if (fSwitchOn)
    {
        SW_TRACE_DEBUG("Info: SwitchTask: TaskType #%d - Off; TaskType #%d - On.\n", iCurTask, iTask);
        switch (iCurTask)
        {
            case MOVETYPE_PRESET: break;
            case MOVETYPE_RESET: break;
            case MOVETYPE_HSCAN: OnStopHScan(WPARAM(iCurTaskID), 0); break;
            case MOVETYPE_FSCAN: OnStopFScan(WPARAM(iCurTaskID), 0); break;
            case MOVETYPE_CRUISE: OnStopCruise(WPARAM(iCurTaskID), 0); break;
            case MOVETYPE_INFINITY_PAN: OnStopInfinityPan(0, 0); break;
            case MOVETYPE_FSCANRECORD: 
                if (iTask != MOVETYPE_OPERATOR && iTask != MOVETYPE_PRESET)
                {
                    OnStopFScanRecord(iCurTaskID, 0); 
                }
                else
                {
                	//录制花样扫描中，
                	//来自用户的PTZ和调用预置位等操作都不改变当前任务状态
                    return S_OK;
                }
                break;
            default:
                break;
        }

        if (MOVETYPE_OPERATOR != iTask && MOVETYPE_PRESET != iTask)
        {
            //1. pause watchkeeping if any
            m_cWatchKeeping.Pause();
            
            //2. pause standby count down if any
            m_cStandby.Pause();
        }
        
        iCurTask = iTask;
        iCurTaskID = iArg;
    }
    else
    {
        //1. resume watchkeeping if any
        m_cWatchKeeping.Resume();
        
        //2. resume standby count down if any
        m_cStandby.Resume();

        if (MOVETYPE_FSCANRECORD != iCurTask
            || MOVETYPE_FSCANRECORD == iTask)
        {
            iCurTask = -1;
            iCurTaskID = -1;
        }
    }

    SetLastTask(iCurTask, iCurTaskID);

    return S_OK;
}


HRESULT CSWDomeCameraControlMSG::OnGetCurTask(WPARAM wParam,LPARAM lParam)
{
    DWORD *pdwTaskInfo = (DWORD*)lParam;
    if (NULL == pdwTaskInfo)
    {
        SW_TRACE_DEBUG("Err: invalid arg lParam(=%d)!\n", lParam);
        return E_INVALIDARG;
    }

    INT iTask = -1, iTaskID = -1;
    GetLastTask(iTask, iTaskID);

    if (-1 == iTask || -1 == iTaskID)
    {
        SW_TRACE_DEBUG("Info: no task running!\n");
        *pdwTaskInfo = 0xFFFFFFFF;
    }

    *pdwTaskInfo = iTask << 16 | iTaskID;

    return S_OK;
}


HRESULT CSWDomeCameraControlMSG::HandleOperation(const INT& iOperation, const INT& iArg)
{
    //1. stop watchkeeping if any
    m_cWatchKeeping.Update();
    
    //2. stop standby count down or wake it up if any
    BOOL fSleeping = FALSE;
    m_cStandby.GetStatus(fSleeping);
    m_cStandby.Update();
    
    if (fSleeping)
    {
        CSWApplication::Sleep(2000);
    }
    

    //3. record FScan if any
    if (iOperation > 0 && m_dwFScanRecordingID != MAX_FSCAN_PATH)
    {
        if (FAILED(m_cFScan.AddMotion(m_dwFScanRecordingID, iOperation, iArg)))
        {
        	SW_TRACE_DEBUG("Err: failed to record FScan #%d\n", m_dwFScanRecordingID);
			return E_FAIL;
        }
    }
    
    //4. others
    
    return S_OK;
}



HRESULT CSWDomeCameraControlMSG::OnTouchDomeCamera(WPARAM wParam,LPARAM lParam)
{
    if (OP_FROM_USER == lParam)
    {
        HANDLEOPERATION(0, 0);
    }

    SwitchTask(TRUE, MOVETYPE_OPERATOR);

    return S_OK;
}


PVOID CSWDomeCameraControlMSG::ResetDome(PVOID pvArg)
{
    CSWDomeCameraControlMSG* pThis = (CSWDomeCameraControlMSG*)pvArg;

    if (NULL != pThis)
    {
        CSWApplication::Sleep(48*1000);

		
        pThis->OnSetLedMode(pThis->GetSetting().Get().iLEDMode, 0);
        pThis->OnSetZoom(100, 0);
        pThis->OnSetPanTiltCoordinate(0, 50);
		swpa_camera_ball_sync_zoom();
    }

}

HRESULT CSWDomeCameraControlMSG::OnResetDomePosition( WPARAM wParam,LPARAM lParam )
{
    HANDLEOPERATION(0, 0);
    SwitchTask(TRUE, MOVETYPE_RESET);
 
    if (
        SWPAR_OK != swpa_camera_reset()
        || SWPAR_OK != swpa_camera_ball_remote_reset()
        )
    {
        SW_TRACE_DEBUG("Err: failed to reset camera & dome!\n");
        return E_FAIL;
    }

    m_cResetDomeThread.Start(ResetDome, this);
    
    return S_OK;
}

HRESULT CSWDomeCameraControlMSG::OnSetPanCoordinate( WPARAM wParam,LPARAM lParam )
{
    INT iValue = (INT)wParam;
    
    if (iValue < 0 || iValue > 3599)
    {
        SW_TRACE_DEBUG("Err: invalid arg wParam(=%d)!\n", iValue);
        return E_INVALIDARG;
    }

    if (OP_FROM_USER == lParam)
    {
        HANDLEOPERATION(FSCAN_M_SET_PAN_COORDINATE, iValue);
    }

    INT iPan = 0, iTilt = 0;
    if (SWPAR_OK != swpa_camera_ball_get_pt_coordinates(&iPan, &iTilt)
        || SWPAR_OK != swpa_camera_ball_set_pt_coordinates(iValue, iTilt))
    {
        SW_TRACE_DEBUG("Info: current pt = ( %d, %d)!\n", iPan, iTilt);
        SW_TRACE_DEBUG("Err: failed to set pan coord to %d!\n", iValue);
        return E_FAIL;
    }

    /*INT iPCoord = 0, iTCoord = 0;
    iPCoord = CSWDomeCameraBase::ConvertPCoordinate(iValue);
    iTCoord = CSWDomeCameraBase::ConvertTCoordinate(iTilt);
    
    if (SWPAR_OK != swpa_camera_basicparam_set_pt(iPCoord, iTCoord))
    {    
        SW_TRACE_DEBUG("Info: failed to set pt (%d, %d) of camera\n", iPCoord, iTCoord);
        return E_FAIL;
    }*/
    
    return S_OK;
}

HRESULT CSWDomeCameraControlMSG::OnGetPanCoordinate( WPARAM wParam,LPARAM lParam )
{
    
    if (NULL == lParam)
    {
        SW_TRACE_DEBUG("Err: invalid arg lParam!\n");
        return E_INVALIDARG;
    }

	if (OP_FROM_USER != wParam) CHECKFSCANSTATUS();

    INT * piCoordinate = (INT*)lParam;

    if (SWPAR_OK != swpa_camera_ball_get_pt_coordinates(piCoordinate, NULL))
    {
        SW_TRACE_DEBUG("Err: failed to get pan coordinate!\n");
        return E_FAIL;
    }
    
    return S_OK;
}


HRESULT CSWDomeCameraControlMSG::OnSetTiltCoordinate( WPARAM wParam,LPARAM lParam )
{
    INT iValue = (INT)wParam;
    
    if (iValue < 0 || iValue > 900)
    {
        SW_TRACE_DEBUG("Err: invalid arg wParam(=%d)!\n", iValue);
        return E_INVALIDARG;
    }

    if (OP_FROM_USER == lParam)
    {
        HANDLEOPERATION(0, 0);
    }
    
    INT iPan = 0, iTilt = 0;
    
    if (SWPAR_OK != swpa_camera_ball_get_pt_coordinates(&iPan, &iTilt)
        || SWPAR_OK != swpa_camera_ball_set_pt_coordinates(iPan, iValue))
    {
        SW_TRACE_DEBUG("Err: failed to set tilt coord to %d!\n", iValue);
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CSWDomeCameraControlMSG::OnGetTiltCoordinate( WPARAM wParam,LPARAM lParam )
{
    if (NULL == lParam)
    {
        SW_TRACE_DEBUG("Err: invalid arg lParam!\n");
        return E_INVALIDARG;
    }
	
	if (OP_FROM_USER != wParam) CHECKFSCANSTATUS();

    INT * piCoordinate = (INT*)lParam;

    if (SWPAR_OK != swpa_camera_ball_get_pt_coordinates(NULL, piCoordinate))
    {
        SW_TRACE_DEBUG("Err: failed to get tilt coordinate!\n");
        return E_FAIL;
    }
    
    return S_OK;
}


HRESULT CSWDomeCameraControlMSG::OnSetPanTiltCoordinate( WPARAM wParam,LPARAM lParam )
{
    INT iValue = (INT)wParam;

    if (OP_FROM_USER == lParam)
    {
        HANDLEOPERATION(0, 0);
        HANDLEOPERATION(0, 0);
    }
    
    INT iPan = iValue >> 16, iTilt = iValue & 0xFFFF;
    
    if (SWPAR_OK != swpa_camera_ball_set_pt_coordinates(iPan, iTilt))
    {
        SW_TRACE_DEBUG("Err: failed to set pantilt coord to (%d, %d)!\n", iPan, iTilt);
        return E_FAIL;
    }

    /*INT iPCoord = 0, iTCoord = 0;
    iPCoord = CSWDomeCameraBase::ConvertPCoordinate(iPan);
    iTCoord = CSWDomeCameraBase::ConvertTCoordinate(iTilt);
    
    if (SWPAR_OK != swpa_camera_basicparam_set_pt(iPCoord, iTCoord))
    {    
        SW_TRACE_DEBUG("Info: failed to set pt (%d, %d) of camera\n", iPCoord, iTCoord);
        return E_FAIL;
    }*/

    return S_OK;
}

HRESULT CSWDomeCameraControlMSG::OnGetPanTiltCoordinate( WPARAM wParam,LPARAM lParam )
{
    if (NULL == lParam)
    {
        SW_TRACE_DEBUG("Err: invalid arg lParam!\n");
        return E_INVALIDARG;
    }

	if (OP_FROM_USER != wParam) CHECKFSCANSTATUS();

    INT * piCoordinate = (INT*)lParam;
    *piCoordinate = -1;
    INT iPCoord = -1, iTCood = -1;

    if (SWPAR_OK != swpa_camera_ball_get_pt_coordinates(&iPCoord, &iTCood))
    {
        SW_TRACE_DEBUG("Err: failed to get pt coordinate!\n");
        return E_FAIL;
    }

    * piCoordinate = (iPCoord << 16) | (iTCood & 0xFFFF);
    
    return S_OK;
}


HRESULT CSWDomeCameraControlMSG::OnSetZoom( WPARAM wParam,LPARAM lParam )
{
    INT iValue = (INT)wParam;
    
    if (iValue < 0 || iValue > 3000)
    {
        SW_TRACE_DEBUG("Err: invalid arg wParam(=%f)!\n", iValue);
        return E_INVALIDARG;
    }

//    INT iZoomValue = CSWDomeCameraBase::ZoomRatioToValue(iValue/100.0);
    INT iZoomValue = m_pCamera->ZoomRatioToValue(iValue/100.0);
    iZoomValue = (-1 == iZoomValue ? 0 : iZoomValue);
    
    if (OP_FROM_USER == lParam)
    {
        SwitchTask(TRUE, MOVETYPE_OPERATOR);
        
        HANDLEOPERATION(FSCAN_M_SET_ZOOM, iZoomValue);
    }

    if (SWPAR_OK != swpa_camera_basicparam_set_zoom_value(iZoomValue))
    {
        SW_TRACE_DEBUG("Err: failed to set zoom to %d!\n", iZoomValue);
        return E_FAIL;
    }

	INT iZoom = 0;
	INT iTryCount = 10;
	while (iTryCount-- > 0)
	{		
	    if (SWPAR_OK != swpa_camera_basicparam_get_zoom_value(&iZoom))
        {
            SW_TRACE_DEBUG("Err: failed to get zoom!\n");
            return E_FAIL;
        }

		swpa_camera_ball_sync_zoom();

		if (swpa_abs(iZoom - iZoomValue) < 5)
		{
			return S_OK;
		}
		else
		{
			swpa_thread_sleep_ms(200);
		}		
	}    
	
    return E_FAIL;
}


HRESULT CSWDomeCameraControlMSG::OnGetZoom( WPARAM wParam,LPARAM lParam )    
{
    if (NULL == lParam)
    {
        SW_TRACE_DEBUG("Err: invalid arg lParam!\n");
        return E_INVALIDARG;
    }

	if (OP_FROM_USER != wParam) CHECKFSCANSTATUS();
	
    INT * piValue = (INT*)lParam;
    INT iZoom = 0;

    if (SWPAR_OK != swpa_camera_basicparam_get_zoom_value(&iZoom))
    {
        SW_TRACE_DEBUG("Err: failed to get zoom!\n");
        return E_FAIL;
    }

//    * piValue = (INT)(CSWDomeCameraBase::ZoomValueToRatio(iZoom)*100);
	*piValue = (INT)(m_pCamera->ZoomValueToRatio(iZoom)*100);
    if (0 > * piValue)
    {
        SW_TRACE_DEBUG("Err: failed to convert zoom value to ratio!\n");
        return E_FAIL;
    }

    return S_OK;
}


HRESULT CSWDomeCameraControlMSG::OnStartPanTilt( WPARAM wParam,LPARAM lParam )
{
    INT iValue = (INT)wParam;
    
    if (iValue < 0 || iValue > 3)
    {
        SW_TRACE_DEBUG("Err: invalid arg wParam(=%d)!\n", iValue);
        return E_INVALIDARG;
    }
    

    PAN_TYPE ePan = (PAN_TYPE)((iValue & 0x2) >> 1);
    TILT_TYPE eTilt = (TILT_TYPE)(iValue & 0x1);


	if (!m_fRepeatPanTilt)
	{    
        swpa_camera_ball_stop();
		
		INT iPrevTask = -1, iTaskID = -1;
		GetLastTask(iPrevTask, iTaskID);
    
        if (MOVETYPE_INFINITY_PAN == iPrevTask)
        {
            CSWApplication::Sleep(500);
        }
	}

	if (OP_FROM_USER == lParam)
    {
    	SwitchTask(TRUE, MOVETYPE_OPERATOR);
		
        CSWMessage::SendMessage(MSG_OSD_ENABLE_JPEG_PTZ_OVERLAY, WPARAM(TRUE), 0);
        CSWMessage::SendMessage(MSG_OSD_ENABLE_H264_PTZ_OVERLAY, WPARAM(TRUE), 0);

        DWORD dwArg = ((ePan & 0xFF)<<24) | ((GetSetting().Get().iPanSpeed & 0xFF)<<16) | ((eTilt & 0xFF)<<8) | (GetSetting().Get().iTiltSpeed & 0xFF);
        HANDLEOPERATION(FSCAN_M_START_PAN_TILT, dwArg);
    }
	
	if (SWPAR_OK != swpa_camera_ball_pan_tilt(ePan, GetSetting().Get().iPanSpeed, eTilt, GetSetting().Get().iTiltSpeed))
    {
        SW_TRACE_DEBUG("Err: failed to start pantilt %d!\n", iValue);
        return E_FAIL;
    }

    m_fRepeatPanTilt = TRUE;
	
    return S_OK;
}

HRESULT CSWDomeCameraControlMSG::OnStopPanTilt( WPARAM wParam,LPARAM lParam )
{
    SwitchTask(FALSE, MOVETYPE_OPERATOR);

	m_fRepeatPanTilt = FALSE;

	if (OP_FROM_USER == lParam)
    {
		CSWMessage::SendMessage(MSG_OSD_ENABLE_JPEG_PTZ_OVERLAY, WPARAM(FALSE), 0);
		CSWMessage::SendMessage(MSG_OSD_ENABLE_H264_PTZ_OVERLAY, WPARAM(FALSE), 0);

        HANDLEOPERATION(FSCAN_M_STOP_PAN_TILT, 0);
    }

    if (SWPAR_OK != swpa_camera_ball_stop())
    {
        SW_TRACE_DEBUG("Err: failed to stop pantilt!\n");
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CSWDomeCameraControlMSG::OnSetPanSpeed( WPARAM wParam,LPARAM lParam )
{
    INT iValue = (INT)wParam;
    
    if (iValue < 0 || iValue > MAX_SPEED)
    {
        SW_TRACE_DEBUG("Err: invalid arg wParam(=%d)!\n", iValue);
        return E_INVALIDARG;
    }

    GetSetting().Get().iPanSpeed = iValue;
    GetSetting().UpdateInt("\\DomeCamera\\Basic", "PanSpeed", iValue);
    GetSetting().Commit();
    
    if (OP_FROM_USER == lParam)
    {
        HANDLEOPERATION(0, 0);
    }
    
    return S_OK;
}

HRESULT CSWDomeCameraControlMSG::OnGetPanSpeed( WPARAM wParam,LPARAM lParam )
{
    if (NULL == lParam)
    {
        SW_TRACE_DEBUG("Err: invalid arg lParam!\n");
        return E_INVALIDARG;
    }

    INT * piValue = (INT*)lParam;
    
    *piValue = GetSetting().Get().iPanSpeed;
    
    return S_OK;
}

HRESULT CSWDomeCameraControlMSG::OnSetMaxPanSpeed( WPARAM wParam,LPARAM lParam )
{
    INT iValue = (INT)wParam;
    
    if (iValue < 0 || iValue > MAX_SPEED)
    {
        SW_TRACE_DEBUG("Err: invalid arg wParam(=%d)!\n", iValue);
        return E_INVALIDARG;
    }

    GetSetting().Get().iMaxPanSpeed = iValue;
    GetSetting().UpdateInt("\\DomeCamera\\Basic", "MaxPanSpeed", iValue);
    GetSetting().Commit();
    
    if (OP_FROM_USER == lParam)
    {
        HANDLEOPERATION(0, 0);
    }
    
    return S_OK;
}

HRESULT CSWDomeCameraControlMSG::OnGetMaxPanSpeed( WPARAM wParam,LPARAM lParam )
{
    if (NULL == lParam)
    {
        SW_TRACE_DEBUG("Err: invalid arg lParam!\n");
        return E_INVALIDARG;
    }

    INT * piValue = (INT*)lParam;
    
    *piValue = GetSetting().Get().iMaxPanSpeed;
    
    return S_OK;
}

HRESULT CSWDomeCameraControlMSG::OnSetTiltSpeed( WPARAM wParam,LPARAM lParam )
{
    INT iValue = (INT)wParam;
    
    if (iValue < 0 || iValue > MAX_SPEED)
    {
        SW_TRACE_DEBUG("Err: invalid arg wParam(=%d)!\n", iValue);
        return E_INVALIDARG;
    }

    GetSetting().Get().iTiltSpeed = iValue;
    GetSetting().UpdateInt("\\DomeCamera\\Basic", "TiltSpeed", iValue);
    GetSetting().Commit();
    
    if (OP_FROM_USER == lParam)
    {
        HANDLEOPERATION(0, 0);
    }
    
    return S_OK;
}

HRESULT CSWDomeCameraControlMSG::OnGetTiltSpeed( WPARAM wParam,LPARAM lParam )
{
    if (NULL == lParam)
    {
        SW_TRACE_DEBUG("Err: invalid arg lParam!\n");
        return E_INVALIDARG;
    }

    INT * piValue = (INT*)lParam;
    
    *piValue = GetSetting().Get().iTiltSpeed;
    
    return S_OK;
}

HRESULT CSWDomeCameraControlMSG::OnSetMaxTiltSpeed( WPARAM wParam,LPARAM lParam )
{
    INT iValue = (INT)wParam;
    
    if (iValue < 0 || iValue > MAX_SPEED)
    {
        SW_TRACE_DEBUG("Err: invalid arg wParam(=%d)!\n", iValue);
        return E_INVALIDARG;
    }

    GetSetting().Get().iMaxTiltSpeed = iValue;
    GetSetting().UpdateInt("\\DomeCamera\\Basic", "MaxTiltSpeed", iValue);
    GetSetting().Commit();

    if (OP_FROM_USER == lParam)
    {
        HANDLEOPERATION(0, 0);
    }
    
    return S_OK;
}

HRESULT CSWDomeCameraControlMSG::OnGetMaxTiltSpeed( WPARAM wParam,LPARAM lParam )
{
    if (NULL == lParam)
    {
        SW_TRACE_DEBUG("Err: invalid arg lParam!\n");
        return E_INVALIDARG;
    }

    INT * piValue = (INT*)lParam;
    
    *piValue = GetSetting().Get().iMaxTiltSpeed;
    
    return S_OK;
}

HRESULT CSWDomeCameraControlMSG::OnSetFocus( WPARAM wParam,LPARAM lParam )
{
    INT iValue = (INT)wParam;
    
    if (iValue < 0x1000 || iValue > 0xd000)
    {
        SW_TRACE_DEBUG("Err: invalid arg wParam(=%d)!\n", iValue);
        return E_INVALIDARG;
    }
    
    if (OP_FROM_USER == lParam)
    {
        SwitchTask(TRUE, MOVETYPE_OPERATOR);
        HANDLEOPERATION(FSCAN_M_SET_FOCUS, iValue);
    }

    if (SWPAR_OK != swpa_camera_basicparam_set_focus_value(iValue))
    {
        SW_TRACE_DEBUG("Err: failed to set focus to %d!\n", iValue);
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CSWDomeCameraControlMSG::OnGetFocus( WPARAM wParam,LPARAM lParam )
{
    if (NULL == lParam)
    {
        SW_TRACE_DEBUG("Err: invalid arg lParam!\n");
        return E_INVALIDARG;
    }

	if (OP_FROM_USER != wParam) CHECKFSCANSTATUS();

    INT * piValue = (INT*)lParam;

    if (SWPAR_OK != swpa_camera_basicparam_get_focus_value(piValue))
    {
        SW_TRACE_DEBUG("Err: failed to get focus!\n");
        return E_FAIL;
    }
    
    return S_OK;
}

HRESULT CSWDomeCameraControlMSG::OnSetFocusMode( WPARAM wParam,LPARAM lParam )
{
    INT iValue = (INT)wParam;
    
    if (iValue < FOCUS_NEAR || iValue > FOCUS_STOP)
    {
        SW_TRACE_DEBUG("Err: invalid arg wParam(=%d)!\n", iValue);
        return E_INVALIDARG;
    }
    
    if (OP_FROM_USER == lParam)
    {
        HANDLEOPERATION(0, 0);
    }

    if (SWPAR_OK != swpa_camera_basicparam_set_AF((AF_MODE)iValue))
    {
        SW_TRACE_DEBUG("Err: failed to set Fouce mode to %d!\n", iValue);
        return E_FAIL;
    }
    
    return S_OK;
}

HRESULT CSWDomeCameraControlMSG::OnGetFocusMode( WPARAM wParam,LPARAM lParam )
{
    if (NULL == lParam)
    {
        SW_TRACE_DEBUG("Err: invalid arg lParam!\n");
        return E_INVALIDARG;
    }

	if (OP_FROM_USER != wParam) CHECKFSCANSTATUS();

    INT * piValue = (INT*)lParam;
    AF_MODE iAFMode;
    if (SWPAR_OK != swpa_camera_basicparam_get_AF(&iAFMode))
    {
        SW_TRACE_DEBUG("Err: failed to get fouce mode!\n");
        return E_FAIL;
    }

    *piValue = iAFMode;
    
    return S_OK;
}

HRESULT CSWDomeCameraControlMSG::OnSetAFEnable( WPARAM wParam,LPARAM lParam )
{
    INT iValue = (INT)wParam;

    if (iValue != 0 && iValue != 1)
    {
        SW_TRACE_DEBUG("Err: invalid arg wParam(=%d)!\n", iValue);
        return E_INVALIDARG;
    }

    if (OP_FROM_USER == lParam)
    {
        HANDLEOPERATION(0, 0);
    }

    AF_MODE eAFMode = 0 == iValue ? AF_AUTO : AF_MANUAL;
    if (SWPAR_OK != swpa_camera_basicparam_set_AF(eAFMode))
    {
        SW_TRACE_DEBUG("Err: failed to set AF to %d!\n", iValue);
        return E_FAIL;
    }


    return S_OK;
}

HRESULT CSWDomeCameraControlMSG::OnGetAFEnable( WPARAM wParam,LPARAM lParam )
{
    if (NULL == lParam)
    {
        SW_TRACE_DEBUG("Err: invalid arg lParam!\n");
        return E_INVALIDARG;
    }

	if (OP_FROM_USER != wParam) CHECKFSCANSTATUS();

    INT * piValue = (INT*)lParam;
    AF_MODE eAFMode;
    if (SWPAR_OK != swpa_camera_basicparam_get_AF(&eAFMode))
    {
        SW_TRACE_DEBUG("Err: failed to get AF!\n");
        return E_FAIL;
    }

    *piValue = AF_AUTO == eAFMode ? 0 : 1;

    return S_OK;
}

HRESULT CSWDomeCameraControlMSG::OnDoOneFocus( WPARAM wParam,LPARAM lParam )
{
    if (OP_FROM_USER == lParam)
    {
        HANDLEOPERATION(0, 0);
    }
    
    if (SWPAR_OK != swpa_camera_basicparam_set_focus(FOCUS_ONE_PUSH))
    {
        SW_TRACE_DEBUG("Err: failed to set fouce mode to %d!\n", FOCUS_ONE_PUSH);
        return E_FAIL;
    }    
    
    return S_OK;
}


HRESULT CSWDomeCameraControlMSG::OnMovePointToCenter( WPARAM wParam,LPARAM lParam )
{
    INT* piValue = (INT*)wParam;
    if (NULL == wParam)
    {
        SW_TRACE_DEBUG("Err: invalid arg wParam!\n");
        return E_INVALIDARG;
    }

    INT iZoomOut = 0;
    SW_POINT sPoint;
    sPoint.x = *piValue;
    sPoint.y = *(++piValue);
    iZoomOut = *(++piValue);

    if (sPoint.x < 0 || sPoint.x > 1920
        || sPoint.y < 0 || sPoint.y > 1080)
    {
        SW_TRACE_DEBUG("Err: invalid arg wParam(%d, %d)!\n", sPoint.x, sPoint.y);
        return E_INVALIDARG;
    }

    if (OP_FROM_USER == lParam)
    {
        SwitchTask(TRUE, MOVETYPE_OPERATOR);
        
        HANDLEOPERATION(0, 0);
    }

	if (m_fSyncZoom)
    {
    	m_fSyncZoom = FALSE;
		m_cSyncZoomThread.Stop();
		swpa_thread_sleep_ms(100);
    }

#if 1
    DOME_SRC_STATUS sSrcParam;
    DOME_DST_STATUS sDstParam;
    INT iPan = 0, iTilt = 0, iZoom = 0;
    
    
    swpa_camera_ball_get_pt_coordinates(&iPan, &iTilt);
    swpa_camera_basicparam_get_zoom_value(&iZoom);

	FLOAT fltZoom = m_pCamera->ZoomValueToRatio(iZoom);

    sSrcParam.dbMaxHViewAng        = m_pCamera->ZoomToHViewAngle(1);
    sSrcParam.dbMinHViewAng        = m_pCamera->ZoomToHViewAngle(m_pCamera->m_iMaxZoom);
    sSrcParam.dbMaxVViewAng        = m_pCamera->ZoomToVViewAngle(1);
    sSrcParam.dbMinVViewAng        = m_pCamera->ZoomToVViewAngle(m_pCamera->m_iMaxZoom);
	sSrcParam.dbMaxFocalDis        = m_pCamera->GetMaxFocalDis();        
    sSrcParam.dbMinFocalDis        = m_pCamera->GetMinFocalDis();
    sSrcParam.dbCurHViewAng        = m_pCamera->ZoomToHViewAngle(fltZoom);
    sSrcParam.dbCurVViewAng        = m_pCamera->ZoomToVViewAngle(fltZoom);
    sSrcParam.dbCurHTurnAng        = (DOUBLE)iPan / 10.0;
    sSrcParam.dbCurVTurnAng        = (DOUBLE)(iTilt-50) / 10.0;
    sSrcParam.dbImageCenterX       = 1920 / 2;  //TODO
    sSrcParam.dbImageCenterY       = 1080 / 2;
    sSrcParam.dbInputLTopX         = 0;
    sSrcParam.dbInputLTopY         = 0;
    sSrcParam.dbInputRLowerX       = 0;
    sSrcParam.dbInputRLowerY       = 0;
    sSrcParam.dbInputPointX        = sPoint.x;
    sSrcParam.dbInputPointY        = sPoint.y;

    //SW_TRACE_DEBUG("--------tobedeleted: sSrcParam(%lf, %lf, %lf)\n", 
	//			   sSrcParam.dbCurHTurnAng, sSrcParam.dbCurVTurnAng, fltZoom);
    
	// CSWDomeCameraBase::ComputeDemoCameraPTZ(&sSrcParam, &sDstParam);
	m_pCamera->ComputeDemoCameraPTZ(&sSrcParam, &sDstParam);

    //SW_TRACE_DEBUG("--------tobedeleted: sDstParam(%lf, %lf, %lf, %lf)\n", 
	//			   sDstParam.dbPan, sDstParam.dbTilt, sDstParam.dbZoom, sDstParam.dbZoomPosition);

    //swpa_camera_ball_set_pt_coordinates((INT)(sDstParam.dbPan*10.0), (INT)(sDstParam.dbTilt*10.0));
    //swpa_camera_basicparam_set_zoom_value((0 != iZoomOut) ? 0 : ZoomRatioToValue(sDstParam.dbZoom));
    
    OnSetPanTiltCoordinate(((INT)(sDstParam.dbPan*10.0)<<16) | ((INT)(sDstParam.dbTilt*10.0 + 50.0)), lParam);
    if ((0 != iZoomOut))
    {        
        OnSetZoom(1, lParam); //right click, zoom out
    }
#else
    INT iCenterX = 100 * (sPoint.x) / 1920;
    INT iCenterY = 100 * (sPoint.y) / 1080;

    if (0 != swpa_camera_ball_3d(iCenterX, iCenterY, 0, 0))
    {
        SW_TRACE_DEBUG("----tobedeleted: swpa_camera_ball_3d failed\n");
        return E_FAIL;
    }
#endif
    return S_OK;
}


HRESULT CSWDomeCameraControlMSG::OnMoveBlockToCenter( WPARAM wParam,LPARAM lParam )
{
    INT* piValue = (INT*)wParam;
    if (NULL == wParam)
    {
        SW_TRACE_DEBUG("Err: invalid arg wParam!\n");
        return E_INVALIDARG;
    }
    
    INT iLeftTopX = *piValue;
    INT iLeftTopY = *(piValue + 1);
    INT iWidth = *(piValue + 2);
    INT iHeight = *(piValue + 3);

    if (/*iLeftTopX < 0 || iLeftTopX > 1920
        || iLeftTopY < 0 || iLeftTopY > 1080
        || iWidth < 0 || iHeight < 0
        || iLeftTopX + iWidth > 1920
        || iLeftTopY + iHeight > 1080
        */
        iWidth > 1920 || iHeight > 1080
        )
    {
        SW_TRACE_DEBUG("Err: invalid arg wParam(%d, %d, %d, %d)!\n", iLeftTopX, iLeftTopY, iWidth, iHeight);
        return E_INVALIDARG;
    }

    if (OP_FROM_USER == lParam)
    {
        SwitchTask(TRUE, MOVETYPE_OPERATOR);
        
        HANDLEOPERATION(0, 0);
    }

	if (m_fSyncZoom)
    {
    	m_fSyncZoom = FALSE;
		m_cSyncZoomThread.Stop();
		swpa_thread_sleep_ms(100);
    }
	
#if 11
    DOME_SRC_STATUS sSrcParam;
    DOME_DST_STATUS sDstParam;
    INT iPan = 0, iTilt = 0, iZoom = 0;
    
    
    swpa_camera_ball_get_pt_coordinates(&iPan, &iTilt);
    swpa_camera_basicparam_get_zoom_value(&iZoom);

    FLOAT fltZoom = m_pCamera->ZoomValueToRatio(iZoom);
    
    sSrcParam.dbMaxHViewAng        = m_pCamera->ZoomToHViewAngle(1);
    sSrcParam.dbMinHViewAng        = m_pCamera->ZoomToHViewAngle(m_pCamera->m_iMaxZoom);
    sSrcParam.dbMaxVViewAng        = m_pCamera->ZoomToVViewAngle(1);
    sSrcParam.dbMinVViewAng        = m_pCamera->ZoomToVViewAngle(m_pCamera->m_iMaxZoom);
    sSrcParam.dbMaxFocalDis        = m_pCamera->GetMaxFocalDis();        
    sSrcParam.dbMinFocalDis        = m_pCamera->GetMinFocalDis();
    sSrcParam.dbCurHViewAng        = m_pCamera->ZoomToHViewAngle(fltZoom);
    sSrcParam.dbCurVViewAng        = m_pCamera->ZoomToVViewAngle(fltZoom);
    sSrcParam.dbCurHTurnAng        = (DOUBLE)iPan / 10.0;
    sSrcParam.dbCurVTurnAng        = (DOUBLE)(iTilt-50)/ 10.0;
    sSrcParam.dbImageCenterX       = 1920 / 2;  //TODO
    sSrcParam.dbImageCenterY       = 1080 / 2;
    sSrcParam.dbInputLTopX         = iLeftTopX;
    sSrcParam.dbInputLTopY         = iLeftTopY;
    sSrcParam.dbInputRLowerX       = iLeftTopX + iWidth;
    sSrcParam.dbInputRLowerY       = iLeftTopY + iHeight;
    sSrcParam.dbInputPointX        = 0;
    sSrcParam.dbInputPointY        = 0;

    //SW_TRACE_DEBUG("----tobedeleted: sSrcParam(%lf, %lf, %lf)\n", 
     //   sSrcParam.dbCurHTurnAng, sSrcParam.dbCurVTurnAng, fltZoom);
    
    //CSWDomeCameraBase::ComputeDemoCameraPTZ(&sSrcParam, &sDstParam);
	m_pCamera->ComputeDemoCameraPTZ(&sSrcParam, &sDstParam);

    //SW_TRACE_DEBUG("----tobedeleted: sDstParam(%lf, %lf, %lf, %lf)\n", 
    //    sDstParam.dbPan, sDstParam.dbTilt, sDstParam.dbZoom, sDstParam.dbZoomPosition);


    //swpa_camera_ball_set_pt_coordinates((INT)(sDstParam.dbPan*10.0), (INT)(sDstParam.dbTilt*10.0));
    //swpa_camera_basicparam_set_zoom_value(ZoomRatioToValue(sDstParam.dbZoom));
    OnSetPanTiltCoordinate(((INT)(sDstParam.dbPan*10.0)<<16) | ((INT)(sDstParam.dbTilt*10.0 + 50.0)), 1);
    OnSetZoom((INT)(sDstParam.dbZoom*100), 1);
#else

    INT iCenterX = 100 * (iLeftTopX + iWidth>>1) / 1920;
    INT iCenterY = 100 * (iLeftTopY + iHeight>>1) / 1080;

    if (0 != swpa_camera_ball_3d(10, 50, 10, 10))//(iCenterX, iCenterY, 100*iWidth/1920, 100*iHeight/1080))
    {
        SW_TRACE_DEBUG("----tobedeleted: swpa_camera_ball_3d failed\n");
        return E_FAIL;
    }
#endif
    return S_OK;
}


HRESULT CSWDomeCameraControlMSG::OnSetIris( WPARAM wParam,LPARAM lParam )
{
    INT iValue = (INT)wParam;
    
    if (iValue < 0 || iValue > 13)
    {
        SW_TRACE_DEBUG("Err: invalid arg wParam(=%d)!\n", iValue);
        return E_INVALIDARG;
    }
    
    SwitchTask(TRUE, MOVETYPE_OPERATOR);

    if (0 < iValue)
    {
        iValue += 4;
    }

    if (SWPAR_OK != swpa_camera_basicparam_set_iris(iValue))
    {
        SW_TRACE_DEBUG("Err: failed to set iris to %d!\n", iValue);
        return E_FAIL;
    }

    
    INT iIris = -1;
    INT iCount = 20;
    do
    {
        CSWApplication::Sleep(100);
        if (SWPAR_OK != swpa_camera_basicparam_get_iris(&iIris))
        {
            SW_TRACE_DEBUG("Err: failed to get iris!\n");
            return E_FAIL;
        }
        iCount--;
    } while (iValue != iIris && iCount > 0);
    
    if (iCount <= 0)
    {
        SW_TRACE_DEBUG("Err: failed to set iris to %d!\n", iValue);
        return E_FAIL;
    }
    
    if (OP_FROM_USER == lParam)
    {
        HANDLEOPERATION(0, 0);
    }
    
    return S_OK;
}

HRESULT CSWDomeCameraControlMSG::OnGetIris( WPARAM wParam,LPARAM lParam )
{
    if (NULL == lParam)
    {
        SW_TRACE_DEBUG("Err: invalid arg lParam!\n");
        return E_INVALIDARG;
    }

	if (OP_FROM_USER != wParam) CHECKFSCANSTATUS();
	
    INT * piValue = (INT*)lParam;

    if (SWPAR_OK != swpa_camera_basicparam_get_iris(piValue))
    {
        SW_TRACE_DEBUG("Err: failed to get iris!\n");
        return E_FAIL;
    }

    //todo: 有些时候，底层会返回1，合法的范围是0, 5~0x11
    SW_TRACE_DEBUG("--tobedeleted: get iris : %d!\n", *piValue);

    if (0<*piValue && 5 > *piValue)
    {
        CSWApplication::Sleep(100);
        if (SWPAR_OK != swpa_camera_basicparam_get_iris(piValue))
        {
            SW_TRACE_DEBUG("Err: failed to get iris!\n");
            return E_FAIL;
        }
        //SW_TRACE_DEBUG("--tobedeleted: get iris 2: %d!\n", *piValue);
    }
    
    if (4 < *piValue)
    {
        *piValue -= 4;
    }
    
    return S_OK;
}

HRESULT CSWDomeCameraControlMSG::OnSetIrisMode( WPARAM wParam,LPARAM lParam )
{    
    if (OP_FROM_USER == lParam)
    {
        HANDLEOPERATION(0, 0);
    }

    SW_TRACE_DEBUG("Warning: GetWiperSpeed not supported!\n");
    return E_NOTIMPL;
}

HRESULT CSWDomeCameraControlMSG::OnGetIrisMode( WPARAM wParam,LPARAM lParam )
{
	if (OP_FROM_USER != wParam) CHECKFSCANSTATUS();
	
    SW_TRACE_DEBUG("Warning: GetWiperSpeed not supported!\n");
	
    return E_NOTIMPL;
}


HRESULT CSWDomeCameraControlMSG::OnTuneIris( WPARAM wParam,LPARAM lParam )
{
    INT iTune = (INT)wParam;
    
    if (iTune < 0 || iTune > 1)
    {
        SW_TRACE_DEBUG("Err: invalid arg wParam(=%d)!\n", iTune);
        return E_INVALIDARG;
    }

    if (OP_FROM_USER == lParam)
    {
        SwitchTask(TRUE, MOVETYPE_OPERATOR);
        HANDLEOPERATION(0, 0);
    }
    
    INT iIris = 0;
    if (FAILED(OnGetIris(0, LPARAM(&iIris))))
    {
        SW_TRACE_DEBUG("Err: failed to get current iris!\n");
        return E_FAIL;
    }

    BOOL fTune = TRUE;
    if ((0 == iTune && iIris <= 0) //tune down
        || (1 == iTune && iIris >= 13) //tune up
        )
    {
        SW_TRACE_DEBUG("Info: current iris touches limitation!\n");
        fTune = FALSE;
    }
    else
    {
        iIris += (0 == iTune) ? -1 : 1;
    }

    if (fTune)
    {
        iIris = iIris < 0 ? 0 : iIris;
        iIris = iIris > 13 ? 13 : iIris;
        if (FAILED(OnSetIris(WPARAM(iIris), 0)))
        {
            SW_TRACE_DEBUG("Err: failed to tune iris!\n");
            return E_FAIL;
        }
    }    
    
    return S_OK;
}



HRESULT CSWDomeCameraControlMSG::OnTuneZoom( WPARAM wParam,LPARAM lParam )
{
    INT iTune = (INT)wParam;
    
    if (iTune < 0 || iTune > 1)
    {
        SW_TRACE_DEBUG("Err: invalid arg wParam(=%d)!\n", iTune);
        return E_INVALIDARG;
    }
    
    if (OP_FROM_USER == lParam)
    {
        SwitchTask(TRUE, MOVETYPE_OPERATOR);
        HANDLEOPERATION(0, 0);
    }
    
    INT iZoom = 0;
    if (FAILED(OnGetZoom(0, LPARAM(&iZoom))))
    {
        SW_TRACE_DEBUG("Err: failed to get current Zoom!\n");
        return E_FAIL;
    }

    BOOL fTune = TRUE;
    if ((0 == iTune && iZoom < 100) //tune down
        || (1 == iTune && iZoom > 3000) //tune up
        )
    {
        SW_TRACE_DEBUG("Info: current Zoom touches limitation!\n");
        fTune = FALSE;
    }
    else
    {
        iZoom += ((0 == iTune) ? -100 : 100);
    }

    if (fTune)
    {
        iZoom = iZoom < 100 ? 100 : iZoom;
        iZoom = iZoom > 3000 ? 3000 : iZoom;
        if (FAILED(OnSetZoom(WPARAM(iZoom), 1)))
        {
            SW_TRACE_DEBUG("Err: failed to tune Zoom!\n");
            return E_FAIL;
        }
    }

    
    return S_OK;
}



HRESULT CSWDomeCameraControlMSG::OnTuneFocus( WPARAM wParam,LPARAM lParam )
{
    INT iTune = (INT)wParam;

#if 0
	
    INT iSpan = 1024;
    static DWORD dwLastTick = 0;
    DWORD dwCurTick = CSWDateTime::GetSystemTick();
    if (dwCurTick > dwLastTick + 1000)
    {
        iSpan = 32;//1024;
    }
    else if (dwCurTick > dwLastTick + 800)
    {
        iSpan = 64;//512;
    }
    else if (dwCurTick > dwLastTick + 500)
    {
        iSpan = 128;//256;
    }
	else if (dwCurTick > dwLastTick + 300)
    {
        iSpan = 256;//256;
    }
    else if (dwCurTick > dwLastTick + 100)
    {
        iSpan = 512;//128;
    }
    else
    {
        iSpan = 1024;//64;
    }
	
	SW_TRACE_DEBUG("tobedeleted: tick (%d, %d)\n", dwLastTick, dwCurTick);
    dwLastTick = dwCurTick;

    
    if (iTune < 0 || iTune > 1)
    {
        SW_TRACE_DEBUG("Err: invalid arg wParam(=%d)!\n", iTune);
        return E_INVALIDARG;
    }
    
    if (OP_FROM_USER == lParam)
    {
        SwitchTask(TRUE, MOVETYPE_OPERATOR);
        HANDLEOPERATION(0, 0);
    }
    
    INT iFocus = 0;
    if (FAILED(OnGetFocus(0, LPARAM(&iFocus))))
    {
        SW_TRACE_DEBUG("Err: failed to get current Focus!\n");
        return E_FAIL;
    }

    BOOL fTune = TRUE;
    if ((0 == iTune && iFocus <= 0x1000) //tune down
        || (1 == iTune && iFocus >= 0xD000) //tune up
        )
    {
        SW_TRACE_DEBUG("Info: current Focus touches limitation (%d)!\n", iFocus);
        fTune = FALSE;
    }
    else
    {
        iFocus += (0 == iTune) ? -1*iSpan : iSpan;
    }

    if (fTune)
    {
        iFocus = iFocus < 0x1000 ? 0x1000 : iFocus;
        iFocus = iFocus > 0xD000 ? 0xD000 : iFocus;
        if (FAILED(OnSetFocus(WPARAM(iFocus), 0)))
        {
            SW_TRACE_DEBUG("Err: failed to tune Focus!\n");
            return E_FAIL;
        }
    }
#else
	FOCUS_MODE iMode = (0 == iTune) ? FOCUS_NEAR : FOCUS_FAR;
	/*if (SWPAR_OK != swpa_camera_basicparam_set_focus(iMode))
	{
		SW_TRACE_DEBUG("Err: failed to set focus to %d\n", iMode);
		return E_FAIL;
	}*/

	if (SWPAR_OK != swpa_camera_basicparam_set_focus_speed(iMode, 2))
	{
		SW_TRACE_DEBUG("Err: failed to set focus speed\n");
		return E_FAIL;
	}
	
	if (SWPAR_OK != swpa_camera_basicparam_set_focus(FOCUS_STOP))
	{
		SW_TRACE_DEBUG("Err: failed to stop focus\n");
		return E_FAIL;
	}
#endif
    
    return S_OK;
}



PVOID CSWDomeCameraControlMSG::SyncZoom(PVOID pvArg)
{
	CSWDomeCameraControlMSG* pThis = (CSWDomeCameraControlMSG*)pvArg;
	DWORD dwCount = 0;

	dwCount = 0;
	BOOL fPrevSync = pThis->m_fSyncZoom;
	DWORD dwPrevTick = CSWDateTime::GetSystemTick();
	while (pThis->m_fSyncZoom || dwCount < 5)
	{
		if (fPrevSync && !pThis->m_fSyncZoom)
		{
			dwCount ++;
		}
		
		SW_TRACE_DEBUG("---tobedeleted: SyncZoom (%d, %d, %d)\n", fPrevSync, pThis->m_fSyncZoom, dwCount);

		swpa_camera_ball_sync_zoom();

		DWORD dwCurTick = CSWDateTime::GetSystemTick();
		if (dwCurTick > dwPrevTick + 5000)
		{
			SW_TRACE_DEBUG("Info: SyncZoom time out (3secs).\n");
			break;
		}
		swpa_thread_sleep_ms(100);
	}
	pThis->m_fSyncZoom = FALSE;

	return NULL;
}

PVOID CSWDomeCameraControlMSG::Defog(PVOID pvArg)
{
	CSWDomeCameraControlMSG* pThis = (CSWDomeCameraControlMSG*)pvArg;
	INT iDefogTime = 1800000;   //鼓风时长 30分钟
	INT iSleepTime = 1000;

	pThis->m_fDefogStarted = TRUE;
	pThis->OnSetDefog((WPARAM)1, 0);

	while(pThis->m_fDefogStarted)
	{
		swpa_thread_sleep_ms(iSleepTime);
		iDefogTime -= iSleepTime;
		if(iDefogTime <= 0)
			break;
	}

	pThis->m_fDefogStarted = FALSE;
	pThis->OnSetDefog((WPARAM)0, 0);

	return NULL;
}


HRESULT CSWDomeCameraControlMSG::OnStartZoom(WPARAM wParam,LPARAM lParam)
{
    INT iValue = (INT)wParam;
    
    if (iValue < 0 || iValue > 1)
    {
        SW_TRACE_DEBUG("Err: invalid arg wParam(=%d)!\n", iValue);
        return E_INVALIDARG;
    }


    INT iZoomSpeed = 0x5;//default speed, equals to the value in domerocker    
    

    /*if (m_fSyncZoom)
    {
    	m_fSyncZoom = FALSE;
		swpa_thread_sleep_ms(200);
		m_cSyncZoomThread.Stop();
    }*/

	//m_fSyncZoom = TRUE;
	//m_cSyncZoomThread.Start(SyncZoom, this);

	ZOOM_MODE eMode = ZOOM_TELE;
    if (1 == iValue) eMode  = ZOOM_WIDE;
	
	if (OP_FROM_USER == lParam)
    {
        SwitchTask(TRUE, MOVETYPE_OPERATOR);

        CSWMessage::SendMessage(MSG_OSD_ENABLE_JPEG_PTZ_OVERLAY, WPARAM(TRUE), 0);
        CSWMessage::SendMessage(MSG_OSD_ENABLE_H264_PTZ_OVERLAY, WPARAM(TRUE), 0);

		HANDLEOPERATION(FSCAN_M_START_ZOOM, (eMode<<16) | (iZoomSpeed & 0xFF));
    }	
	
    if (SWPAR_OK != swpa_camera_basicparam_set_zoom_speed(eMode, iZoomSpeed))
    {
        SW_TRACE_DEBUG("Err: failed to start zoom %d!\n", iValue);
        return E_FAIL;
    }
    
    return S_OK;
}


HRESULT CSWDomeCameraControlMSG::OnStopZoom(WPARAM wParam,LPARAM lParam)
{
    if (OP_FROM_USER == lParam)
    {
        SwitchTask(FALSE, MOVETYPE_OPERATOR);
        
        CSWMessage::SendMessage(MSG_OSD_ENABLE_JPEG_PTZ_OVERLAY, WPARAM(FALSE), 0);
        CSWMessage::SendMessage(MSG_OSD_ENABLE_H264_PTZ_OVERLAY, WPARAM(FALSE), 0);

		HANDLEOPERATION(FSCAN_M_STOP_ZOOM, 0);
    }

    //swpa_camera_basicparam_set_zoom_mode(ZOOM_STOP);//todo: temp solution
    //CSWApplication::Sleep(30);
    if (SWPAR_OK != swpa_camera_basicparam_set_zoom_mode(ZOOM_STOP))
    {
        SW_TRACE_DEBUG("Err: failed to stop zoom!\n");
        return E_FAIL;
    }

	//m_fSyncZoom = FALSE;
	//swpa_thread_sleep_ms(500);
	//m_cSyncZoomThread.Stop();

	for (INT iCount=0; iCount<3; iCount++)
	{
		swpa_thread_sleep_ms(200);
		swpa_camera_ball_sync_zoom();
	}
	
    return S_OK;
}


HRESULT CSWDomeCameraControlMSG::OnStartPan(WPARAM wParam,LPARAM lParam)
{
    INT iValue = (INT)wParam;
    
    if (iValue < 0 || iValue > 1)
    {
        SW_TRACE_DEBUG("Err: invalid arg wParam(=%d)!\n", iValue);
        return E_INVALIDARG;
    }

    PAN_TYPE eType = (PAN_TYPE)iValue;
    
	if (!m_fRepeatPanTilt)
	{
        swpa_camera_ball_stop();

		INT iPrevTask = -1, iTaskID = -1;
	    GetLastTask(iPrevTask, iTaskID);
    
        if (MOVETYPE_INFINITY_PAN == iPrevTask)
        {
            CSWApplication::Sleep(500);
        }
	}

	
	if (OP_FROM_USER == lParam)
	{
		SwitchTask(TRUE, MOVETYPE_OPERATOR);
		
        CSWMessage::SendMessage(MSG_OSD_ENABLE_JPEG_PTZ_OVERLAY, WPARAM(TRUE), 0);
        CSWMessage::SendMessage(MSG_OSD_ENABLE_H264_PTZ_OVERLAY, WPARAM(TRUE), 0);

		HANDLEOPERATION(FSCAN_M_START_PAN, eType<<16 | (GetSetting().Get().iPanSpeed & 0xFF));
	}
	
    if (SWPAR_OK != swpa_camera_ball_pan(eType, GetSetting().Get().iPanSpeed))
    {
        SW_TRACE_DEBUG("Err: failed to start pan %d!\n", eType);
        return E_FAIL;
    }

	m_fRepeatPanTilt = TRUE;
    
    return S_OK;
}

HRESULT CSWDomeCameraControlMSG::OnStopPan(WPARAM wParam,LPARAM lParam)
{
   
	m_fRepeatPanTilt = FALSE;
	
	if (OP_FROM_USER == lParam)
	{
		CSWMessage::SendMessage(MSG_OSD_ENABLE_JPEG_PTZ_OVERLAY, WPARAM(FALSE), 0);
		CSWMessage::SendMessage(MSG_OSD_ENABLE_H264_PTZ_OVERLAY, WPARAM(FALSE), 0);

		SwitchTask(FALSE, MOVETYPE_OPERATOR);
		HANDLEOPERATION(FSCAN_M_STOP_PAN, 0);
	}

    if (SWPAR_OK != swpa_camera_ball_stop())
    {
        SW_TRACE_DEBUG("Err: failed to stop pan!\n");
        return E_FAIL;
    }
    
    return S_OK;
}


PVOID CSWDomeCameraControlMSG::InfinityPan(PVOID pvArg)
{
    if (NULL == pvArg)
    {
        SW_TRACE_DEBUG("Err: invalid arg!\n");
        return 0;
    }

    CSWDomeCameraControlMSG* pThis = (CSWDomeCameraControlMSG*)pvArg;

    while (pThis->m_fInfinityPanStarted)
    {
        if (SWPAR_OK != swpa_camera_ball_pan(PAN_LEFT, pThis->GetSetting().Get().iPanSpeed))
        {
            SW_TRACE_DEBUG("Err: failed to start infinity pan!\n");
        }
        CSWApplication::Sleep(200);
    }

    SW_TRACE_DEBUG("Info: InfinityPan exited!\n");
}


HRESULT CSWDomeCameraControlMSG::OnStartInfinityPan(WPARAM wParam,LPARAM lParam)
{
    if (OP_FROM_USER == lParam)
    {
        HANDLEOPERATION(0, 0);
    }

    SwitchTask(TRUE, MOVETYPE_INFINITY_PAN, 0);
    
    swpa_camera_ball_stop();

    m_fInfinityPanStarted = FALSE;

    if (S_OK == m_cInfinityPanThread.IsValid())
    {
        m_cInfinityPanThread.Stop();
    }
    
    m_fInfinityPanStarted = TRUE;
    if (FAILED(m_cInfinityPanThread.Start(InfinityPan, this)))
    {
        SW_TRACE_DEBUG("Err: failed to start infinity pan!\n");
        return E_FAIL;
    }
    
    return S_OK;
}


HRESULT CSWDomeCameraControlMSG::OnStopInfinityPan(WPARAM wParam,LPARAM lParam)
{    
    m_fInfinityPanStarted = FALSE;
    m_cInfinityPanThread.Stop();
    
    if (OP_FROM_USER == lParam)
    {
        HANDLEOPERATION(0, 0);
    }

    DWORD dwCount = 0;
    while (S_OK == m_cInfinityPanThread.IsValid())
    {
        if (3 < ++dwCount)
        {
            SW_TRACE_DEBUG("Err: failed to stop infinity pan!\n");
            return E_FAIL;
        }
        CSWApplication::Sleep(200);
    }
    
    if (SWPAR_OK != swpa_camera_ball_stop())
    {
        SW_TRACE_DEBUG("Err: failed to stop infinity pan!\n");
        return E_FAIL;
    }
    
    SwitchTask(FALSE, MOVETYPE_INFINITY_PAN);

    return S_OK;
}



HRESULT CSWDomeCameraControlMSG::OnStartTilt(WPARAM wParam,LPARAM lParam)
{
    INT iValue = (INT)wParam;
    
    if (iValue < 0 || iValue > 1)
    {
        SW_TRACE_DEBUG("Err: invalid arg wParam(=%d)!\n", iValue);
        return E_INVALIDARG;
    }

	if (!m_fRepeatPanTilt)
    {
        swpa_camera_ball_stop();
    }
    
    if (OP_FROM_USER == lParam)
    {
		SwitchTask(TRUE, MOVETYPE_OPERATOR);
		
		CSWMessage::SendMessage(MSG_OSD_ENABLE_JPEG_PTZ_OVERLAY, WPARAM(TRUE), 0);
        CSWMessage::SendMessage(MSG_OSD_ENABLE_H264_PTZ_OVERLAY, WPARAM(TRUE), 0);
		
		HANDLEOPERATION(FSCAN_M_START_TILT, iValue<<16 | (GetSetting().Get().iTiltSpeed & 0xFF));
    }
    
    if (SWPAR_OK != swpa_camera_ball_tilt((TILT_TYPE)iValue, GetSetting().Get().iTiltSpeed))
    {
        SW_TRACE_DEBUG("Err: failed to start tilt %d!\n", iValue);
        return E_FAIL;
    }

	m_fRepeatPanTilt = TRUE;
    
    return S_OK;
}

HRESULT CSWDomeCameraControlMSG::OnStopTilt(WPARAM wParam,LPARAM lParam)
{
	m_fRepeatPanTilt = FALSE;
	
    if (OP_FROM_USER == lParam)
    {
		CSWMessage::SendMessage(MSG_OSD_ENABLE_JPEG_PTZ_OVERLAY, WPARAM(FALSE), 0);
		CSWMessage::SendMessage(MSG_OSD_ENABLE_H264_PTZ_OVERLAY, WPARAM(FALSE), 0);
		
		SwitchTask(FALSE, MOVETYPE_OPERATOR);
		HANDLEOPERATION(FSCAN_M_STOP_TILT, 0);
    }

	
    if (SWPAR_OK != swpa_camera_ball_stop())
    {
        SW_TRACE_DEBUG("Err: failed to stop tilt!\n");
        return E_FAIL;
    }
        
    return S_OK;
}


PVOID CSWDomeCameraControlMSG::VTurnOver(PVOID pvArg)
{
    INT iPanCoord = 0, iTiltCoord = 0;

    CSWDomeCameraControlMSG* pThis = (CSWDomeCameraControlMSG*)pvArg;
    
    if (!pThis->m_fVTurnOver)  return NULL;
    
    if (SWPAR_OK != swpa_camera_ball_get_pt_coordinates(&iPanCoord, &iTiltCoord))
    {    
        SW_TRACE_DEBUG("Err: failed to get pt\n");
        return NULL;
    }

    if (!pThis->m_fVTurnOver)  return NULL;
    CSWApplication::Sleep(1000);
    if (!pThis->m_fVTurnOver)  return NULL;
    
    if (SWPAR_OK != swpa_camera_ball_set_pt_coordinates(iPanCoord, 900))
    {    
        SW_TRACE_DEBUG("Err: failed to set tilt to 900\n");
        return NULL;
    }

    if (!pThis->m_fVTurnOver)  return NULL;
    CSWApplication::Sleep(1000);
    if (!pThis->m_fVTurnOver)  return NULL;

    if (SWPAR_OK != swpa_camera_ball_set_pt_coordinates((iPanCoord+1800)%3600, 900))
    {    
        SW_TRACE_DEBUG("Err: failed to set tilt to 900\n");
        return NULL;
    }

    if (!pThis->m_fVTurnOver)  return NULL;
    CSWApplication::Sleep(1000);
    if (!pThis->m_fVTurnOver)  return NULL;

    if (SWPAR_OK != swpa_camera_ball_set_pt_coordinates((iPanCoord+1800)%3600, iTiltCoord))
    {    
        SW_TRACE_DEBUG("Err: failed to set tilt to 900\n");
        return NULL;
    }    

    return NULL;
}




HRESULT CSWDomeCameraControlMSG::OnStartVTurnOver(WPARAM wParam,LPARAM lParam)
{    
    if (OP_FROM_USER == lParam)
    {
        HANDLEOPERATION(0, 0);
        SwitchTask(TRUE, MOVETYPE_OPERATOR);
    }
    
    swpa_camera_ball_stop();

    m_fVTurnOver = TRUE;

    if (FAILED(m_cVTurnOverThread.Start(VTurnOver, this)))
    {
        SW_TRACE_DEBUG("Err: failed to start VTurnOver\n");
        return E_FAIL;
    }
    
    return S_OK;
}

HRESULT CSWDomeCameraControlMSG::OnStopVTurnOver(WPARAM wParam,LPARAM lParam)
{
    if (OP_FROM_USER == lParam)
    {
        HANDLEOPERATION(0, 0);
    }
    
    m_fVTurnOver = FALSE;
    m_cVTurnOverThread.Stop();
    
    if (SWPAR_OK != swpa_camera_ball_stop())
    {
        SW_TRACE_DEBUG("Err: failed to stop VTureOver\n");
        return E_FAIL;
    }
    
    return S_OK;
}

HRESULT CSWDomeCameraControlMSG::OnStartWiper(WPARAM wParam,LPARAM lParam)
{
    if (OP_FROM_USER == lParam)
    {
        HANDLEOPERATION(0, 0);
    }

	if(!m_fDefogStarted)
	{
	 	m_cDefogThread.Stop();
		if(FAILED(m_cDefogThread.Start(Defog, this)))
		{
			SW_TRACE_DEBUG("Err: failed to start defog!\n");
			return E_FAIL;
		}	
	}

    if (SWPAR_OK != swpa_camera_ball_set_wiper())
    {
        SW_TRACE_DEBUG("Err: failed to start wiper!\n");
        return E_FAIL;
    }

    
    return S_OK;
}

HRESULT CSWDomeCameraControlMSG::OnStopWiper(WPARAM wParam,LPARAM lParam)
{
    if (OP_FROM_USER == lParam)
    {
        HANDLEOPERATION(0, 0);
    }

    SW_TRACE_DEBUG("Warning: StopWiper not supported!\n");
    return E_NOTIMPL;
}

HRESULT CSWDomeCameraControlMSG::OnSetWiperSpeed( WPARAM wParam,LPARAM lParam )
{
    
    if (OP_FROM_USER == lParam)
    {
        HANDLEOPERATION(0, 0); 
    }

    SW_TRACE_DEBUG("Warning: SetWiperSpeed not supported!\n");
    return E_NOTIMPL;
}

HRESULT CSWDomeCameraControlMSG::OnGetWiperSpeed( WPARAM wParam,LPARAM lParam )
{
	if (OP_FROM_USER != wParam) CHECKFSCANSTATUS();
	
    SW_TRACE_DEBUG("Warning: GetWiperSpeed not supported!\n");
    return E_NOTIMPL;
}

HRESULT CSWDomeCameraControlMSG::OnSetWiperAutoStartTime( WPARAM wParam,LPARAM lParam )
{
    
    if (OP_FROM_USER == lParam)
    {
        HANDLEOPERATION(0, 0); 
    }
    
    SW_TRACE_DEBUG("Warning: SetWiperAutoStartTime not supported!\n");
    return E_NOTIMPL;
}

HRESULT CSWDomeCameraControlMSG::OnGetWiperAutoStartTime( WPARAM wParam,LPARAM lParam )
{
	if (OP_FROM_USER != wParam) CHECKFSCANSTATUS();
	
    SW_TRACE_DEBUG("Warning: GetWiperAutoStartTime not supported!\n");
    return E_NOTIMPL;
}


HRESULT CSWDomeCameraControlMSG::OnSetLedMode(WPARAM wParam,LPARAM lParam)
{
    INT iMode = (INT)wParam;
    if (iMode != 0 && iMode != 1 && iMode != 2)
    {
        SW_TRACE_DEBUG("Err: invalid arg wParam = %d!\n", iMode);
        return E_INVALIDARG;
    }

    
    if (OP_FROM_USER == lParam)
    {
        HANDLEOPERATION(0, 0); 
    }

    INT iLEDMode = 0;
    switch (iMode)
    {
        case 0: iLEDMode = 3; /*off*/ break;            
        case 1: iLEDMode = 2; /*on*/  break;
        case 2: iLEDMode = 0; /*auto*/break;
        default:
            break;
    }	

    if (SWPAR_OK != swpa_camera_ball_set_led_mode(iLEDMode, (OP_FROM_USER == lParam) ? 0x0 : 0x2))
    {
        SW_TRACE_DEBUG("Err: failed to set LED to %d!\n", iMode);
        return E_FAIL;
    }

	//if (iMode == 1)
	{
		swpa_thread_sleep_ms(100);
		swpa_camera_ball_sync_zoom();
    	swpa_thread_sleep_ms(50);
	}
	
    if (GetSetting().Get().iLEDMode != iMode)
    {
        GetSetting().Get().iLEDMode = iMode;
        GetSetting().UpdateInt("\\DomeCamera\\Basic", "LEDMode", iMode);
        GetSetting().Commit();
    }

    return S_OK;
}

HRESULT CSWDomeCameraControlMSG::OnGetLedMode(WPARAM wParam,LPARAM lParam)
{
    INT * piMode = (INT*)lParam;
    if (NULL == lParam)
    {
        SW_TRACE_DEBUG("Err: invalid arg wParam!\n");
        return E_INVALIDARG;
    }

    *piMode = GetSetting().Get().iLEDMode;
    
    return S_OK;
}


HRESULT CSWDomeCameraControlMSG::OnSetLedPower(WPARAM wParam,LPARAM lParam)
{
    INT iPower = (INT)wParam;
    if (iPower < 0)
    {
        SW_TRACE_DEBUG("Err: invalid arg wParam = %d!\n", iPower);
        return E_INVALIDARG;
    }

    if (OP_FROM_USER == lParam)
    {
        HANDLEOPERATION(0, 0);
    }

    if (1 != GetSetting().Get().iLEDMode)
    {
        SW_TRACE_DEBUG("Err: LED is not ON!\n");
        return E_FAIL;
    }
    
    if (SWPAR_OK != swpa_camera_ball_set_led_power((iPower >> 16) & 0xFF, (iPower >> 8) & 0xFF, (iPower >> 0) & 0xFF))
    {
        SW_TRACE_DEBUG("Err: failed to set LED power to %d!\n", iPower);
        return E_FAIL;
    }

    GetSetting().Get().iLEDPower = iPower;
    GetSetting().UpdateInt("\\DomeCamera\\Basic", "LEDPower", iPower);
    //GetSetting().Commit(); //not necessary, reduce flash write op

    return S_OK;
}

HRESULT CSWDomeCameraControlMSG::OnGetLedPower(WPARAM wParam,LPARAM lParam)
{
    INT * piPower = (INT*)lParam;
    if (NULL == lParam)
    {
        SW_TRACE_DEBUG("Err: invalid arg wParam!\n");
        return E_INVALIDARG;
    }

    *piPower = GetSetting().Get().iLEDPower;

    return S_OK;
}



HRESULT CSWDomeCameraControlMSG::OnSetDefog(WPARAM wParam,LPARAM lParam)
{
    
    if (OP_FROM_USER == lParam)
    {
        HANDLEOPERATION(0, 0); 
    }

    //SwitchTask(TRUE, MOVETYPE_OPERATOR);

    INT iValue = (INT)wParam;

    if (iValue != 0 && iValue != 1)
    {
        SW_TRACE_DEBUG("Err: invalid arg wParam(=%d)!\n", (INT)wParam);
        return E_INVALIDARG;
    }

    if (SWPAR_OK != swpa_camera_ball_set_defog_fan(iValue))
    {
        SW_TRACE_DEBUG("Err: failed to set defog to %d!\n", iValue);
        return E_FAIL;
    }

    return S_OK;
}


HRESULT CSWDomeCameraControlMSG::OnSetPresetPos( WPARAM wParam,LPARAM lParam )
{
    DWORD dwID = (DWORD)wParam;
    if (NULL == lParam)
    {
        SW_TRACE_DEBUG("Err: invalid arg lParam!\n");
        return E_FAIL;
    }

    if (OP_FROM_USER == lParam)
    {
        HANDLEOPERATION(0, 0); 
    }

    SwitchTask(TRUE, MOVETYPE_OPERATOR);
    
    if (FAILED(m_cPreset.Set(dwID, *(PRESETPOS_PARAM_STRUCT*)lParam)))
    {
        SW_TRACE_DEBUG("Err: failed to set Preset #%d!\n", dwID);
        return E_FAIL;
    }
    
    return S_OK;
}

HRESULT CSWDomeCameraControlMSG::OnGetPresetPos( WPARAM wParam,LPARAM lParam )
{
    DWORD dwID = (DWORD)wParam;
    if (NULL == lParam)
    {
        SW_TRACE_DEBUG("Err: invalid arg lParam!\n");
        return E_FAIL;
    }

    if (FAILED(m_cPreset.Get(dwID, *(PRESETPOS_PARAM_STRUCT*)lParam)))
    {
        SW_TRACE_DEBUG("Err: failed to get Preset #%d!\n", dwID);
        return E_FAIL;
    }

    
    return S_OK;
}

HRESULT CSWDomeCameraControlMSG::OnCallPresetPos( WPARAM wParam,LPARAM lParam )
{
    DWORD dwID = (DWORD)wParam;
    INT iFlag = (INT)lParam;

    SW_TRACE_DEBUG("tobedeleted: call Preset #%d!, flag = %d\n", dwID, iFlag);
    
    if (0 == iFlag) //iFlag:0 - Call Preset from User
    {
        HANDLEOPERATION(FSCAN_M_CALL_PRESET, dwID);

        SwitchTask(TRUE, MOVETYPE_OPERATOR);
    }
    
    //if (1 != iFlag) //iFlag:1 - Call Preset from RecogFilter
    {
        CSWMessage::SendMessage(MSG_OSD_SET_JPEG_DOMEINFO_OVERLAY, 0, 0);
        CSWMessage::SendMessage(MSG_OSD_SET_H264_DOMEINFO_OVERLAY, 0, 0);
    }

	if(FUNC_DEFOG_START == dwID)
	{
		if(!m_fDefogStarted)
		{
			m_cDefogThread.Stop();
			if(FAILED(m_cDefogThread.Start(Defog, this)))
			{
				SW_TRACE_DEBUG("Err: failed to start defog!\n");
				return E_FAIL;
			}	
		}
	}
    
    if (FAILED(m_cPreset.Call(dwID)))
    {
        SW_TRACE_DEBUG("Err: failed to call Preset #%d!\n", dwID);
        return E_FAIL;
    }


    //if (1 != iFlag) //iFlag:1 - Call Preset from RecogFilter
    {
        PRESETPOS_PARAM_STRUCT sParam;
        swpa_memset(&sParam, 0, sizeof(sParam));
        m_cPreset.Get(dwID, sParam);
        CSWMessage::SendMessage(MSG_OSD_SET_JPEG_DOMEINFO_OVERLAY, WPARAM(sParam.szName), 1);
        CSWMessage::SendMessage(MSG_OSD_SET_H264_DOMEINFO_OVERLAY, WPARAM(sParam.szName), 1);
    }

    if (0 == iFlag)
    {
        SwitchTask(TRUE, MOVETYPE_PRESET);
    }
    
    return S_OK;
}

HRESULT CSWDomeCameraControlMSG::OnClearPresetPos( WPARAM wParam,LPARAM lParam )
{
    DWORD dwID = (DWORD)wParam;

    
    if (OP_FROM_USER == lParam)
    {
        HANDLEOPERATION(0, 0); 
    }

    //SwitchTask(TRUE, MOVETYPE_OPERATOR);
    
    if (FAILED(m_cPreset.Clear(dwID)))
    {
        SW_TRACE_DEBUG("Err: failed to clear Preset #%d!\n", dwID);
        return E_FAIL;
    }

    
    return S_OK;
}


HRESULT CSWDomeCameraControlMSG::OnGetPresetNameList( WPARAM wParam,LPARAM lParam )
{
    if (NULL == lParam)
    {
        SW_TRACE_DEBUG("Err: invalid arg lParam!\n");
        return E_FAIL;
    }

    //CHAR szName[]
    CSWString strList;
    //CSWString strCheckPT;
    for (DWORD dwID=0; dwID<MAX_PRESET; dwID++)
    {
        PRESETPOS_PARAM_STRUCT sParam;
        swpa_memset(&sParam, 0, sizeof(sParam));
        strList.Clear();
        
        m_cPreset.Get(dwID, sParam);

        strList.Append(sParam.szName);
        strList.Append(":");
        strList.Append(sParam.fCheckPT ? "1" : "0");
        strList.Append(";");

        swpa_strcat((CHAR*)lParam, (const CHAR*)strList);
    }
    
    return S_OK;
}



HRESULT CSWDomeCameraControlMSG::OnSetHScanLeftPos( WPARAM wParam,LPARAM lParam )
{
    DWORD dwID = (DWORD)wParam;

    if (OP_FROM_USER == lParam)
    {
        HANDLEOPERATION(0, 0); 
    }

    SwitchTask(TRUE, MOVETYPE_OPERATOR);

    if (FAILED(m_cHScan.SetLeftPos(dwID)))
    {
        SW_TRACE_DEBUG("Err: failed to set left position of HScan #%d!\n", dwID);
        return E_FAIL;
    }
    
    return S_OK;
}

HRESULT CSWDomeCameraControlMSG::OnGetHScanLeftPos( WPARAM wParam,LPARAM lParam )
{
    if (NULL == lParam)
    {
        SW_TRACE_DEBUG("Err: invalid arg lParam!\n");
        return E_FAIL;
    }

    if (FAILED(m_cHScan.GetLeftPos((DWORD)wParam, *(INT*)lParam)))
    {
        SW_TRACE_DEBUG("Err: failed to get left position of HScan #%d!\n", (DWORD)wParam);
        return E_FAIL;
    }
    
    return S_OK;
}

HRESULT CSWDomeCameraControlMSG::OnSetHScanRightPos( WPARAM wParam,LPARAM lParam )
{
    DWORD dwID = (DWORD)wParam;

    if (OP_FROM_USER == lParam)
    {
        HANDLEOPERATION(0, 0); 
    }

    SwitchTask(TRUE, MOVETYPE_OPERATOR);

    if (FAILED(m_cHScan.SetRightPos(dwID)))
    {
        SW_TRACE_DEBUG("Err: failed to set right position of HScan #%d!\n", dwID);
        return E_FAIL;
    }
    
    return S_OK;
}


HRESULT CSWDomeCameraControlMSG::OnGetHScanRightPos( WPARAM wParam,LPARAM lParam )
{
    if (NULL == lParam)
    {
        SW_TRACE_DEBUG("Err: invalid arg lParam!\n");
        return E_FAIL;
    }

    if (FAILED(m_cHScan.GetRightPos((DWORD)wParam, *(INT*)lParam)))
    {
        SW_TRACE_DEBUG("Err: failed to get right position of HScan #%d!\n", (DWORD)wParam);
        return E_FAIL;
    }
    
    return S_OK;
}




HRESULT CSWDomeCameraControlMSG::OnSetHScanParam( WPARAM wParam,LPARAM lParam )
{
    DWORD dwID = (DWORD)(wParam);
    if (NULL == lParam)
    {
        SW_TRACE_DEBUG("Err: invalid arg lParam!\n");
        return E_INVALIDARG;
    }

    if (OP_FROM_USER == lParam)
    {
        HANDLEOPERATION(0, 0); 
    }
    //SwitchTask(TRUE, MOVETYPE_OPERATOR);

    HSCAN_PARAM_STRUCT* pParam = (HSCAN_PARAM_STRUCT*)lParam;


    if (FAILED(m_cHScan.Set(dwID, * pParam)))
    {
        SW_TRACE_DEBUG("Err: failed to set param of HScan #%d!\n", dwID);
        return E_FAIL;
    }
    
    return S_OK;
}

HRESULT CSWDomeCameraControlMSG::OnGetHScanParam( WPARAM wParam,LPARAM lParam )
{
    DWORD dwID = (DWORD)wParam;
    if (NULL == lParam)
    {
        SW_TRACE_DEBUG("Err: invalid arg lParam!\n");
        return E_INVALIDARG;
    }

    HSCAN_PARAM_STRUCT sParam;
    if (FAILED(m_cHScan.Get(dwID, *(HSCAN_PARAM_STRUCT*)lParam)))
    {
        SW_TRACE_DEBUG("Err: failed to get param of HScan #%d!\n", dwID);
        return E_FAIL;
    }

    //swpa_memcpy(lParam, &sParam, sizeof(sParam));
    
    return S_OK;
}

HRESULT CSWDomeCameraControlMSG::OnCallHScan( WPARAM wParam,LPARAM lParam )
{
    INT iID = (INT)wParam;
        
    if (iID < 0 || iID >= MAX_HSCAN_PATH)
    {
        SW_TRACE_DEBUG("Err: invalid arg wParam(=%d)!\n", iID);
        return E_INVALIDARG;
    }

    if (OP_FROM_USER == lParam)
    {
        HANDLEOPERATION(0, 0);
    
        SwitchTask(TRUE, MOVETYPE_OPERATOR);
    }
    
    swpa_camera_ball_stop();

	//swpa_camera_ball_sync_zoom();
        
    if (FAILED(m_cHScan.Call(iID)))
    {
        SW_TRACE_DEBUG("Err: failed to call HScan #%d!\n", iID);
        return E_FAIL;
    }
    
    SwitchTask(TRUE, MOVETYPE_HSCAN, iID);

    return S_OK;
}

HRESULT CSWDomeCameraControlMSG::OnStopHScan( WPARAM wParam,LPARAM lParam )
{

    INT iID = (INT)wParam;
        
    if (iID < 0 || iID >= MAX_HSCAN_PATH)
    {
        SW_TRACE_DEBUG("Err: invalid arg wParam(=%d)!\n", iID);
        return E_INVALIDARG;
    }
    
    if (OP_FROM_USER == lParam)
    {
        HANDLEOPERATION(0, 0);            
    }

    if (FAILED(m_cHScan.Stop(iID)))
    {
        SW_TRACE_DEBUG("Err: failed to stop HScan #%d!\n", iID);
        return E_FAIL;
    }

    SwitchTask(FALSE, MOVETYPE_HSCAN, iID);

	swpa_camera_ball_sync_zoom();
    
    return S_OK;
}


HRESULT CSWDomeCameraControlMSG::OnClearHScan( WPARAM wParam,LPARAM lParam )
{

    DWORD dwID = (DWORD)wParam;
        
    if (dwID >= MAX_HSCAN_PATH)
    {
        SW_TRACE_DEBUG("Err: invalid arg wParam(=%d)!\n", dwID);
        return E_INVALIDARG;
    }
    
    if (OP_FROM_USER == lParam)
    {
        HANDLEOPERATION(0, 0); 
    }

    if (m_cHScan.GetRunningID() == dwID)
    {
        SwitchTask(TRUE, MOVETYPE_OPERATOR);
    }

    if (FAILED(m_cHScan.Clear(dwID)))
    {
        SW_TRACE_DEBUG("Err: failed to clear HScan #%d!\n", dwID);
        return E_FAIL;
    }
    
    return S_OK;
}


HRESULT CSWDomeCameraControlMSG::OnStartFScanRecord( WPARAM wParam,LPARAM lParam )
{
    INT iID = (INT)wParam;
    
    if (iID < 0 || iID >= MAX_FSCAN_PATH)
    {
        SW_TRACE_DEBUG("Err: invalid arg wParam(=%d)!\n", iID);
        return E_INVALIDARG;
    }

    if (m_dwFScanRecordingID != MAX_FSCAN_PATH)
    {
        SW_TRACE_DEBUG("Err: FScan #%d is recording, won't record #d!\n", m_dwFScanRecordingID, iID);
        return E_UNEXPECTED;
    }

    m_dwFScanRecordingID = iID;
    HANDLEOPERATION(FSCAN_M_NOP, 0);// the first motion.
    SwitchTask(TRUE, MOVETYPE_FSCANRECORD, iID);
    
    return S_OK;
}

HRESULT CSWDomeCameraControlMSG::OnStopFScanRecord( WPARAM wParam,LPARAM lParam )
{
    INT iID = (INT)wParam;
    
    if (iID < 0 || iID >= MAX_FSCAN_PATH)
    {
        SW_TRACE_DEBUG("Err: invalid arg wParam(=%d)!\n", iID);
        return E_INVALIDARG;
    }

    if (m_dwFScanRecordingID != iID)
    {
        SW_TRACE_DEBUG("Err: FScan #%d is not recording!\n", iID);
        return E_UNEXPECTED;
    }

    m_dwFScanRecordingID = MAX_FSCAN_PATH;

    if (OP_FROM_USER == lParam)
    {
        HANDLEOPERATION(0, 0);
        SwitchTask(FALSE, MOVETYPE_FSCANRECORD);
    }

    if (FAILED(m_cFScan.SavePath(iID)))
    {
        SW_TRACE_DEBUG("Err: failed to stop FScan #%d recording!\n", iID);
        return E_FAIL;
    }
    
    return S_OK;
}

HRESULT CSWDomeCameraControlMSG::OnCallFScan( WPARAM wParam,LPARAM lParam )
{
    INT iID = (INT)wParam;
        
    if (iID < 0 || iID >= MAX_FSCAN_PATH)
    {
        SW_TRACE_DEBUG("Err: invalid arg wParam(=%d)!\n", iID);
        return E_INVALIDARG;
    }

    //OnStopFScanRecord((WPARAM)iID, 0);

    if (OP_FROM_USER == lParam)
    {
        HANDLEOPERATION(0, 0);

        SwitchTask(TRUE, MOVETYPE_OPERATOR);
    }

    swpa_camera_ball_stop();

    if (FAILED(m_cFScan.Call(iID)))
    {
        SW_TRACE_DEBUG("Err: failed to call FScan #%d!\n", iID);
        return E_FAIL;
    }

    SwitchTask(TRUE, MOVETYPE_FSCAN, iID);

    return S_OK;
}


HRESULT CSWDomeCameraControlMSG::OnStopFScan( WPARAM wParam,LPARAM lParam )
{
    INT iID = (INT)wParam;
        
    if (iID < 0 || iID >= MAX_FSCAN_PATH)
    {
        SW_TRACE_DEBUG("Err: invalid arg wParam(=%d)!\n", iID);
        return E_INVALIDARG;
    }
    
    if (OP_FROM_USER == lParam)
    {
        HANDLEOPERATION(0, 0);            
    }

    if (FAILED(m_cFScan.Stop(iID)))
    {
        SW_TRACE_DEBUG("Err: failed to stop FScan #%d!\n", iID);
        return E_FAIL;
    }
	
    SwitchTask(FALSE, MOVETYPE_FSCAN, iID);
	
	swpa_camera_ball_sync_zoom();
    
    return S_OK;
}


HRESULT CSWDomeCameraControlMSG::OnClearFScan( WPARAM wParam,LPARAM lParam )
{
    DWORD dwID = (DWORD)wParam;
        
    if (dwID >= MAX_FSCAN_PATH)
    {
        SW_TRACE_DEBUG("Err: invalid arg wParam(=%d)!\n", dwID);
        return E_INVALIDARG;
    }

    if (m_dwFScanRecordingID == dwID)
    {
        OnStopFScanRecord((WPARAM)dwID, 0);
    }

    if (OP_FROM_USER == lParam)
    {
        HANDLEOPERATION(0, 0);
    }
    
    if (m_cFScan.GetRunningID() == dwID)
    {
        SwitchTask(TRUE, MOVETYPE_OPERATOR);
    }

    if (FAILED(m_cFScan.ClearPath(dwID)))
    {
        SW_TRACE_DEBUG("Err: failed to clear FScan #%d!\n", dwID);
        return E_FAIL;
    }
    
    return S_OK;
}

HRESULT CSWDomeCameraControlMSG::OnSetFScanParam( WPARAM wParam,LPARAM lParam )
{
    DWORD dwID = (DWORD)wParam;
    CSWString strName = (CHAR*) lParam;
    if (NULL == lParam)
    {
        SW_TRACE_DEBUG("Err: invalid arg lParam!\n");
        return E_FAIL;
    }

    if (m_dwFScanRecordingID == dwID)
    {
        OnStopFScanRecord((WPARAM)dwID, 0);
        SwitchTask(TRUE, MOVETYPE_OPERATOR);
    }

    if (OP_FROM_USER == lParam)
    {
        HANDLEOPERATION(0, 0);
    }

    if (FAILED(m_cFScan.SetName(dwID, strName)))
    {
        SW_TRACE_DEBUG("Err: failed to set FScan #%d!\n", dwID);
        return E_FAIL;
    }
    
    return S_OK;
}

HRESULT CSWDomeCameraControlMSG::OnGetFScanParam( WPARAM wParam,LPARAM lParam )
{
    DWORD dwID = (DWORD)wParam;
    CHAR * szName = (CHAR*) lParam;

    CSWString strName;

    if (FAILED(m_cFScan.GetName(dwID, strName)))
    {
        SW_TRACE_DEBUG("Err: failed to get FScan #%d!\n", dwID);
        return E_FAIL;
    }

    swpa_strcpy(szName, (LPCSTR)strName);
    
    return S_OK;
}


HRESULT CSWDomeCameraControlMSG::OnGetFScanNameList( WPARAM wParam,LPARAM lParam )
{
    if (NULL == lParam)
    {
        SW_TRACE_DEBUG("Err: invalid arg lParam!\n");
        return E_FAIL;
    }
    
    for (DWORD dwID=0; dwID<MAX_FSCAN_PATH; dwID++)
    {
        CSWString strName;
        m_cFScan.GetName(dwID, strName);
        swpa_strcat((CHAR*)lParam, (const CHAR*)strName);
        swpa_strcat((CHAR*)lParam, ";");
    }
    
    return S_OK;
}



HRESULT CSWDomeCameraControlMSG::OnSetCruise( WPARAM wParam,LPARAM lParam )
{
    DWORD dwID = (DWORD)wParam;
    CRUISE_PARAM_STRUCT * psCruiseParam = (CRUISE_PARAM_STRUCT *)lParam;

    if (NULL == psCruiseParam)
    {
        SW_TRACE_DEBUG("Err: invalid arg lParam!\n");
        return E_INVALIDARG;
    }

    if (OP_FROM_USER == lParam)
    {
        HANDLEOPERATION(0, 0);
    }
    //SwitchTask(TRUE, MOVETYPE_OPERATOR);

    if (FAILED(m_cCruise.Set(dwID, *psCruiseParam)))
    {
        SW_TRACE_DEBUG("Err: failed to set cruise #%d!\n", dwID);
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CSWDomeCameraControlMSG::OnGetCruise( WPARAM wParam,LPARAM lParam )
{
    DWORD dwID = (DWORD)wParam;
    CRUISE_PARAM_STRUCT * psCruiseParam = (CRUISE_PARAM_STRUCT *)lParam;

    if (NULL == psCruiseParam)
    {
        SW_TRACE_DEBUG("Err: invalid arg lParam!\n");
        return E_INVALIDARG;
    }
    
    if (FAILED(m_cCruise.Get(dwID, *psCruiseParam)))
    {
        SW_TRACE_DEBUG("Err: failed to get cruise #%d!\n", dwID);
        return E_FAIL;
    }
    
    return S_OK;
}

HRESULT CSWDomeCameraControlMSG::OnCallCruise( WPARAM wParam,LPARAM lParam )
{
    INT iID = (INT)wParam;
        
    if (iID < 0 || iID >= MAX_CRUISE)
    {
        SW_TRACE_DEBUG("Err: invalid arg wParam(=%d)!\n", iID);
        return E_INVALIDARG;
    }

    SW_TRACE_DEBUG("tobedeleted: OnCallCruise %d, %d\n", iID, lParam);

    if (OP_FROM_USER == lParam)
    {
        HANDLEOPERATION(0, 0);

        SwitchTask(TRUE, MOVETYPE_OPERATOR);
    }

    swpa_camera_ball_stop();
    
    if (FAILED(m_cCruise.Call(iID)))
    {
        SW_TRACE_DEBUG("Err: failed to call cruise #%d!\n", iID);
        return E_FAIL;
    }

    SwitchTask(TRUE, MOVETYPE_CRUISE, iID);
    
    return S_OK;
}


HRESULT CSWDomeCameraControlMSG::OnStopCruise( WPARAM wParam,LPARAM lParam )
{
    INT iID = (INT)wParam;
        
    if (iID < 0 || iID >= MAX_CRUISE)
    {
        SW_TRACE_DEBUG("Err: invalid arg wParam(=%d)!\n", iID);
        return E_INVALIDARG;
    }

    if (OP_FROM_USER == lParam)
    {
        HANDLEOPERATION(0, 0);            
    }    

    if (FAILED(m_cCruise.Stop(iID)))
    {
        SW_TRACE_DEBUG("Err: failed to stop cruise #%d!\n", iID);
        return E_FAIL;
    }

    SwitchTask(FALSE, MOVETYPE_CRUISE, iID);
	
	swpa_camera_ball_sync_zoom();
    
    return S_OK;
}


HRESULT CSWDomeCameraControlMSG::OnClearCruise( WPARAM wParam,LPARAM lParam )
{
    DWORD dwID = (INT)wParam;
        
    if (dwID >= MAX_CRUISE)
    {
        SW_TRACE_DEBUG("Err: invalid arg wParam(=%d)!\n", dwID);
        return E_INVALIDARG;
    }
    
    if (OP_FROM_USER == lParam)
    {
        HANDLEOPERATION(0, 0);
    }

    if (m_cCruise.GetRunningID() == dwID)
    {
        SwitchTask(TRUE, MOVETYPE_OPERATOR);
    }

    if (FAILED(m_cCruise.Clear(dwID)))
    {
        SW_TRACE_DEBUG("Err: failed to clear cruise #%d!\n", dwID);
        return E_FAIL;
    }
    
    return S_OK;
}



HRESULT CSWDomeCameraControlMSG::OnSetWatchKeeping( WPARAM wParam,LPARAM lParam )
{
    WATCHKEEPING_PARAM_STRUCT * psParam = (WATCHKEEPING_PARAM_STRUCT *)wParam;

    if (NULL == psParam)
    {
        SW_TRACE_DEBUG("Err: invalid arg wParam!\n");
        return E_INVALIDARG;
    }

    if (OP_FROM_USER == lParam)
    {
        HANDLEOPERATION(0, 0);
    }
    
    if (FAILED(m_cWatchKeeping.Set(*psParam)))
    {
        SW_TRACE_DEBUG("Err: failed to set watchkerrping param!\n");
        return E_FAIL;
    }

    SwitchTask(FALSE, MOVETYPE_OPERATOR);
    
    return S_OK;
}

HRESULT CSWDomeCameraControlMSG::OnGetWatchKeeping( WPARAM wParam,LPARAM lParam )
{
    WATCHKEEPING_PARAM_STRUCT * psParam = (WATCHKEEPING_PARAM_STRUCT *)lParam;

    if (NULL == psParam)
    {
        SW_TRACE_DEBUG("Err: invalid arg lParam!\n");
        return E_INVALIDARG;
    }

    if (FAILED(m_cWatchKeeping.Get(*psParam)))
    {
        SW_TRACE_DEBUG("Err: failed to get watchkerrping param!\n");
        return E_FAIL;
    }

    
    return S_OK;
}

HRESULT CSWDomeCameraControlMSG::OnClearWatchKeeping( WPARAM wParam,LPARAM lParam )
{
    if (OP_FROM_USER == lParam)
    {
        HANDLEOPERATION(0, 0);
    }
    
    if (FAILED(m_cWatchKeeping.Clear()))
    {
        SW_TRACE_DEBUG("Err: failed to clear watchkerrping!\n");
        return E_FAIL;
    }
    
    return S_OK;
}

HRESULT CSWDomeCameraControlMSG::OnStartWatchKeeping( WPARAM wParam,LPARAM lParam )
{
    if (OP_FROM_USER == lParam)
    {
        HANDLEOPERATION(0, 0);
    }
    
    if (FAILED(m_cWatchKeeping.Call()))
    {
        SW_TRACE_DEBUG("Err: failed to call watchkerrping!\n");
        return E_FAIL;
    }
    
    return S_OK;
}

HRESULT CSWDomeCameraControlMSG::OnStopWatchKeeping( WPARAM wParam,LPARAM lParam )
{
    if (OP_FROM_USER == lParam)
    {
        HANDLEOPERATION(0, 0);
    }
    
    if (FAILED(m_cWatchKeeping.Stop()))
    {
        SW_TRACE_DEBUG("Err: failed to stop watchkerrping!\n");
        return E_FAIL;
    }
    
    return S_OK;
}

HRESULT CSWDomeCameraControlMSG::OnSetMask( WPARAM wParam,LPARAM lParam )
{
    DWORD dwID = (DWORD)wParam;
    MASK_PARAM_STRUCT * psParam = (MASK_PARAM_STRUCT *)lParam;

    if (NULL == psParam)
    {
        SW_TRACE_DEBUG("Err: invalid arg lParam!\n");
        return E_INVALIDARG;
    }
    
    if (OP_FROM_USER == lParam)
    {
        HANDLEOPERATION(0, 0);
    }

    DOME_SRC_STATUS sSrcParam;
    DOME_DST_STATUS sDstParam;
    INT iPan = 0, iTilt = 0, iZoom = 0;
    
    
    swpa_camera_ball_get_pt_coordinates(&iPan, &iTilt);
    swpa_camera_basicparam_get_zoom_value(&iZoom);

//    FLOAT fltZoom = CSWDomeCameraBase::ZoomValueToRatio(iZoom);
    FLOAT fltZoom = m_pCamera->ZoomValueToRatio(iZoom);


    SW_TRACE_DEBUG("INFO: (%d, %d, %d -> %.2f | %d %d %d %d)\n", 
        iPan, iTilt, iZoom, fltZoom, psParam->iLeft, psParam->iTop, psParam->iWidth, psParam->iHeight);
    
    /*
    sSrcParam.dbMaxHViewAng        = 59.500000;
    sSrcParam.dbMinHViewAng        = 2.182885;
    sSrcParam.dbMaxVViewAng        = 35.644701;
    sSrcParam.dbMinVViewAng        = 1.227974;
    sSrcParam.dbMaxFocalDis        = 129;        
    sSrcParam.dbMinFocalDis        = 4.3;
    sSrcParam.dbCurHViewAng        = CSWDomeCameraBase::ZoomToHViewAngle(fltZoom);
    sSrcParam.dbCurVViewAng        = CSWDomeCameraBase::ZoomToVViewAngle(fltZoom);
    sSrcParam.dbCurHTurnAng        = (DOUBLE)iPan / 10.0;
    sSrcParam.dbCurVTurnAng        = ((DOUBLE)iTilt -50) / 10.0;
    sSrcParam.dbImageCenterX       = 1920 / 2;  //TODO
    sSrcParam.dbImageCenterY       = 1080 / 2;
    sSrcParam.dbInputLTopX         = psParam->iLeft;
    sSrcParam.dbInputLTopY         = psParam->iTop;
    sSrcParam.dbInputRLowerX       = psParam->iLeft + psParam->iWidth;
    sSrcParam.dbInputRLowerY       = psParam->iTop + psParam->iHeight;
    sSrcParam.dbInputPointX        = 0;
    sSrcParam.dbInputPointY        = 0;
    
    CSWDomeCameraBase::ComputeDemoCameraPTZ(&sSrcParam, &sDstParam);

    

    psParam->iMaskPan = CSWDomeCameraBase::ConvertPCoordinate((INT)(sDstParam.dbPan*10.0));
    psParam->iMaskTilt = CSWDomeCameraBase::ConvertTCoordinate((INT)(sDstParam.dbTilt*10.0 + 50.0));
    psParam->iMaskZoom = iZoom;//CSWDomeCameraBase::ZoomRatioToValue(sDstParam.dbZoom);

    SW_TRACE_DEBUG("---tobedeleted: %d, %d, %d; %d, %d, %d; %d, %d, %d\n",
        iPan, iTilt, iZoom, 
        (INT)(sDstParam.dbPan*10.0),
        (INT)(sDstParam.dbTilt*10.0),
        (INT) sDstParam.dbZoom,
        psParam->iMaskPan, psParam->iMaskTilt, psParam->iMaskZoom);*/
	if(!m_cMask.IsInitialized())
	{
		SW_TRACE_DEBUG("Err: mask not initialize!\n");
		return E_FAIL;
	}
    
    if (FAILED(m_cMask.Set(dwID, *psParam)))
    {
        SW_TRACE_DEBUG("Err: failed to set mask #%d param!\n", dwID);
        return E_FAIL;
    }
    
    return S_OK;
}

HRESULT CSWDomeCameraControlMSG::OnGetMask( WPARAM wParam,LPARAM lParam )
{
    DWORD dwID = (DWORD)wParam;
    MASK_PARAM_STRUCT * psParam = (MASK_PARAM_STRUCT *)lParam;

    if (NULL == psParam)
    {
        SW_TRACE_DEBUG("Err: invalid arg lParam!\n");
        return E_INVALIDARG;
    }
	if(!m_cMask.IsInitialized())
	{
		SW_TRACE_DEBUG("Err: mask not initialize!\n");
		return E_FAIL;
	}

    
    if (FAILED(m_cMask.Get(dwID, *psParam)))
    {
        SW_TRACE_DEBUG("Err: failed to get mask #%d param!\n", dwID);
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CSWDomeCameraControlMSG::OnClearMask( WPARAM wParam,LPARAM lParam )
{
    DWORD dwID = (DWORD)wParam;
    
    if (OP_FROM_USER == lParam)
    {
        HANDLEOPERATION(0, 0);
    }
	if(!m_cMask.IsInitialized())
	{
		SW_TRACE_DEBUG("Err: mask not initialize!\n");
		return E_FAIL;
	}
    
    if (FAILED(m_cMask.Clear(dwID)))
    {
        SW_TRACE_DEBUG("Err: failed to clear mask #%d param!\n", dwID);
        return E_FAIL;
    }
    
    return S_OK;
}


HRESULT CSWDomeCameraControlMSG::OnShowMask( WPARAM wParam,LPARAM lParam )
{
    DWORD dwID = (DWORD)wParam;
	if(!m_cMask.IsInitialized())
	{
		SW_TRACE_DEBUG("Err: mask not initialize!\n");
		return E_FAIL;
	}

    if (FAILED(m_cMask.Show(dwID)))
    {
        SW_TRACE_DEBUG("Err: failed to show mask #%d!\n", dwID);
        return E_FAIL;
    }
    
    return S_OK;
}

HRESULT CSWDomeCameraControlMSG::OnHideMask( WPARAM wParam,LPARAM lParam )
{
    DWORD dwID = (DWORD)((INT)wParam == -1 ? MAX_MASK : wParam);
	if(!m_cMask.IsInitialized())
	{
		SW_TRACE_DEBUG("Err: mask not initialize!\n");
		return E_FAIL;
	}

    if (FAILED(m_cMask.Hide(dwID)))
    {
        SW_TRACE_DEBUG("Err: failed to hide mask #%d!\n", dwID);
        return E_FAIL;
    }
    
    return S_OK;
}



HRESULT CSWDomeCameraControlMSG::OnSetMaskFlag( WPARAM wParam,LPARAM lParam )
{
    
    return S_OK;
}

HRESULT CSWDomeCameraControlMSG::OnGetMaskFlag( WPARAM wParam,LPARAM lParam )
{
    
    return S_OK;
}

HRESULT CSWDomeCameraControlMSG::OnSetTimer( WPARAM wParam,LPARAM lParam )
{
    if (NULL == lParam)
    {
        SW_TRACE_DEBUG("Err: invalid arg lParam\n");
        return E_INVALIDARG;
    }

    if (OP_FROM_USER == lParam)
    {
        HANDLEOPERATION(0, 0);
    }

    DWORD dwID = (DWORD)wParam;
    TIMER_PARAM_STRUCT* psSetting = (TIMER_PARAM_STRUCT*)lParam;

    if (FAILED(m_cTimer.Set(dwID,*psSetting)))    
    {
        SW_TRACE_DEBUG("Err: failed to set Timer #%d\n", dwID);
        return E_INVALIDARG;
    }
    
    return S_OK;
}

HRESULT CSWDomeCameraControlMSG::OnGetTimer( WPARAM wParam,LPARAM lParam )
{
    if (NULL == lParam)
    {
        SW_TRACE_DEBUG("Err: invalid arg lParam\n");
        return E_INVALIDARG;
    }

    DWORD dwID = (DWORD)wParam;
    TIMER_PARAM_STRUCT* psSetting = (TIMER_PARAM_STRUCT*)lParam;

    if (FAILED(m_cTimer.Get(dwID,*psSetting)))    
    {
        SW_TRACE_DEBUG("Err: failed to get Timer #%d\n", dwID);
        return E_INVALIDARG;
    }
    
    return S_OK;
}

HRESULT CSWDomeCameraControlMSG::OnClearTimer( WPARAM wParam,LPARAM lParam )
{
    DWORD dwID = (DWORD)wParam;

    if (OP_FROM_USER == lParam)
    {
        HANDLEOPERATION(0, 0);
    }
    
    if (FAILED(m_cTimer.Clear(dwID)))    
    {
        SW_TRACE_DEBUG("Err: failed to clear Timer #%d\n", dwID);
        return E_INVALIDARG;
    }
    
    return S_OK;
}

HRESULT CSWDomeCameraControlMSG::OnSetTimerFlag( WPARAM wParam,LPARAM lParam )
{

    if (OP_FROM_USER == lParam)
    {
        HANDLEOPERATION(0, 0);
    }
    
    return S_OK;
}

HRESULT CSWDomeCameraControlMSG::OnGetTimerFlag( WPARAM wParam,LPARAM lParam )
{
    
    return S_OK;
}

HRESULT CSWDomeCameraControlMSG::OnSetPictureFreeze( WPARAM wParam,LPARAM lParam )
{
    PICTUREFREEZE_PARAM_STRUCT sSetting;
    sSetting.fEnable = (0 != (DWORD)wParam) ? TRUE : FALSE;

    if (OP_FROM_USER == lParam)
    {
        HANDLEOPERATION(0, 0);
    }
    
    if (FAILED(m_cPictureFreeze.Set(sSetting)))
    {
        SW_TRACE_DEBUG("Err: failed to set picture freeze to %d\n", sSetting.fEnable);
        return E_FAIL;
    }    
    
    return S_OK;
}

HRESULT CSWDomeCameraControlMSG::OnGetPictureFreeze( WPARAM wParam,LPARAM lParam )
{
    if (NULL == lParam)
    {
        SW_TRACE_DEBUG("Err: invalid arg lParam\n");
        return E_INVALIDARG;
    }

    PICTUREFREEZE_PARAM_STRUCT sSetting;
    if (FAILED(m_cPictureFreeze.Get(sSetting)))
    {
        SW_TRACE_DEBUG("Err: failed to get picture freeze flag\n");
        return E_FAIL;
    }

    *(BOOL*)lParam = sSetting.fEnable;
    
    return S_OK;
}

HRESULT CSWDomeCameraControlMSG::OnSavePictureFreeze( WPARAM wParam,LPARAM lParam )
{
	return m_cPictureFreeze.SaveSetting();
}

HRESULT CSWDomeCameraControlMSG::OnSetStandbyFlag( WPARAM wParam,LPARAM lParam )
{
    STANDBY_PARAM_STRUCT sSetting;

    if (OP_FROM_USER == lParam)
    {
        HANDLEOPERATION(0, 0);
    }
    
    if (FAILED(m_cStandby.Get(sSetting)))
    {
        SW_TRACE_DEBUG("Err: failed to get standby param\n");
        return E_FAIL;
    }

    sSetting.fEnable = INT(wParam) != 0 ? TRUE : FALSE;

    if (FAILED(m_cStandby.Set(sSetting)))
    {
        SW_TRACE_DEBUG("Err: failed to set standby flag\n");
        return E_FAIL;
    }
    
    return S_OK;
}

HRESULT CSWDomeCameraControlMSG::OnGetStandbyFlag( WPARAM wParam,LPARAM lParam )
{
    if (NULL == lParam)
    {
        SW_TRACE_DEBUG("Err: invalid arg lParam\n");
        return E_INVALIDARG;
    }

    STANDBY_PARAM_STRUCT sSetting;
    if (FAILED(m_cStandby.Get(sSetting)))
    {
        SW_TRACE_DEBUG("Err: failed to get standby param\n");
        return E_FAIL;
    }

    *(BOOL*)lParam = sSetting.fEnable;
    
    return S_OK;
}


HRESULT CSWDomeCameraControlMSG::OnSetIdleTimeToStandby( WPARAM wParam,LPARAM lParam )
{
    if (0 > wParam)
    {
        SW_TRACE_DEBUG("Err: invalid arg wParam = %d\n", wParam);
        return E_INVALIDARG;
    }

    if (OP_FROM_USER == lParam)
    {
        HANDLEOPERATION(0, 0);
    }
    
    STANDBY_PARAM_STRUCT sSetting;

    if (FAILED(m_cStandby.Get(sSetting)))
    {
        SW_TRACE_DEBUG("Err: failed to get standby param\n");
        return E_FAIL;
    }

    sSetting.iWaitTime = INT(wParam);

    if (FAILED(m_cStandby.Set(sSetting)))
    {
        SW_TRACE_DEBUG("Err: failed to set standby Idle time\n");
        return E_FAIL;
    }
    
    return S_OK;
}

HRESULT CSWDomeCameraControlMSG::OnGetIdleTimeToStandby( WPARAM wParam,LPARAM lParam )
{
    if (NULL == lParam)
    {
        SW_TRACE_DEBUG("Err: invalid arg lParam\n");
        return E_INVALIDARG;
    }

    STANDBY_PARAM_STRUCT sSetting;
    if (FAILED(m_cStandby.Get(sSetting)))
    {
        SW_TRACE_DEBUG("Err: failed to get standby idle time\n");
        return E_FAIL;
    }

    *(BOOL*)lParam = sSetting.iWaitTime;
    
    return S_OK;
}


HRESULT CSWDomeCameraControlMSG::OnStartStandbyCountDown( WPARAM wParam,LPARAM lParam )
{
    if (OP_FROM_USER == lParam)
    {
        HANDLEOPERATION(0, 0);
    }
    
    if (FAILED(m_cStandby.Call()))
    {
        SW_TRACE_DEBUG("Err: failed to start standby count down\n");
        return E_FAIL;
    }

    return S_OK;
}


HRESULT CSWDomeCameraControlMSG::OnGetStandbyStatus( WPARAM wParam,LPARAM lParam )
{
    if (NULL == lParam)
    {
        SW_TRACE_DEBUG("Err: invalid arg lParam\n");
        return E_INVALIDARG;
    }
    
    if (FAILED(m_cStandby.GetStatus(*(INT*)lParam)))
    {
        SW_TRACE_DEBUG("Err: failed to get standby status\n");
        return E_FAIL;
    }

    return S_OK;
}


HRESULT CSWDomeCameraControlMSG::OnSetXml( WPARAM wParam,LPARAM lParam )
{
    HRESULT hr = E_FAIL;
    char *szXML = (char *)wParam;

    if (OP_FROM_USER == lParam)
    {
        HANDLEOPERATION(0, 0);
    }
    
    if(NULL != szXML)
    {
        hr = GetSetting().LoadFromXml(szXML, swpa_strlen(szXML) + 1);
        if(S_OK == hr)
        {
            GetSetting().Commit();
        }
    }
    SW_TRACE_DEBUG("OnSetXML(0x%08x, 0x%08x) return 0x%08x\n", wParam, lParam, hr);
    return hr;
}

HRESULT CSWDomeCameraControlMSG::OnGetXml( WPARAM wParam,LPARAM lParam )
{
    HRESULT hr = E_FAIL;
    CSWString *strXML = (CSWString *)lParam;
    if(NULL != strXML)
    {
        hr = GetSetting().ToXml(*strXML);
    }
    SW_TRACE_DEBUG("OnGetXML(0x%08x, 0x%08x) return 0x%08x\n", wParam, lParam, hr);
    return hr;
}



HRESULT CSWDomeCameraControlMSG::OnPause( WPARAM wParam,LPARAM lParam )
{
    INT iTask = -1, iTaskID = -1;
    
    GetLastTask(iTask, iTaskID);

    switch (iTask)
    {
        case MOVETYPE_CRUISE: return m_cCruise.Pause(iTaskID);
        //case MOVETYPE_HSCAN: return m_cHScan->Pause(iTaskID);
        //case MOVETYPE_FSCAN: return m_cFScan->Pause(iTaskID);
        default: return S_OK;
    }
}


HRESULT CSWDomeCameraControlMSG::OnResume( WPARAM wParam,LPARAM lParam )
{
    INT iTask = -1, iTaskID = -1;
    
    GetLastTask(iTask, iTaskID);

    SW_TRACE_DEBUG("tobedeleted: resume %d, %d\n", iTask, iTaskID);

    switch (iTask)
    {
        case MOVETYPE_CRUISE: return m_cCruise.Resume(iTaskID);
        //case MOVETYPE_HSCAN: return m_cHScan->Resume(iTaskID);
        //case MOVETYPE_FSCAN: return m_cFScan->Resume(iTaskID);
        default: return S_OK;
    }
}



//---================ 相机参数 ==================================

HRESULT CSWDomeCameraControlMSG::OnSetAEMode( WPARAM wParam,LPARAM lParam )
{
    INT iValue = (INT)wParam;

    if (OP_FROM_USER == lParam)
    {
        HANDLEOPERATION(0, 0);
    }
    
    if (iValue != 0 && iValue != 1 && iValue != 2 && iValue != 3)
    {
        SW_TRACE_DEBUG("Err: invalid arg wParam(=%d)!\n", iValue);
        return E_INVALIDARG;
    }

    AE_MODE arrAEMode[] = {AE_FULL_AUTO, AE_MANUAL, AE_SHUTTER_PRIORITY, AE_IRIS_PRIORITY};
    
    if (SWPAR_OK != swpa_camera_basicparam_set_AE(arrAEMode[iValue]))
    {
        SW_TRACE_DEBUG("Err: failed to set AE mode to %d!\n", iValue);
        return E_FAIL;
    }

    return S_OK;
}


HRESULT CSWDomeCameraControlMSG::OnGetAEMode( WPARAM wParam,LPARAM lParam )
{
    if (NULL == lParam)
    {
        SW_TRACE_DEBUG("Err: invalid arg lParam!\n");
        return E_INVALIDARG;
    }

	if (OP_FROM_USER != wParam) CHECKFSCANSTATUS();

    INT * piValue = (INT*)lParam;

    AE_MODE eAEMode = AE_FULL_AUTO;
    if (SWPAR_OK != swpa_camera_basicparam_get_AE(&eAEMode))
    {
        SW_TRACE_DEBUG("Err: failed to get AE mode!\n");
        return E_FAIL;
    }

    switch (eAEMode)
    {
        case AE_FULL_AUTO: * piValue = 0; break;
        case AE_MANUAL: * piValue = 1; break;
        case AE_SHUTTER_PRIORITY: * piValue = 2; break;
        case AE_IRIS_PRIORITY: * piValue = 3; break;
        default: SW_TRACE_DEBUG("Err: got invalid AE mode(%d)!\n", eAEMode); return E_FAIL;
    }

    return S_OK;
}

HRESULT CSWDomeCameraControlMSG::OnSetShutter( WPARAM wParam,LPARAM lParam )
{
    INT iValue = (INT)wParam;

    if (OP_FROM_USER == lParam)
    {
        HANDLEOPERATION(0, 0);
    }

    if (iValue < 0 || iValue > 0x15)
    {
        SW_TRACE_DEBUG("Err: invalid arg wParam(=%d)!\n", iValue);
        return E_INVALIDARG;
    }

    if (SWPAR_OK != swpa_camera_basicparam_set_shutter(iValue))
    {
        SW_TRACE_DEBUG("Err: failed to set shutter to %d!\n", iValue);
        return E_FAIL;
    }

    return S_OK;
}


HRESULT CSWDomeCameraControlMSG::OnGetShutter( WPARAM wParam,LPARAM lParam )
{
    if (NULL == lParam)
    {
        SW_TRACE_DEBUG("Err: invalid arg lParam!\n");
        return E_INVALIDARG;
    }

	if (OP_FROM_USER != wParam) CHECKFSCANSTATUS();
	
    INT * piValue = (INT*)lParam;

    if (SWPAR_OK != swpa_camera_basicparam_get_shutter(piValue))
    {
        SW_TRACE_DEBUG("Err: failed to get shutter!\n");
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CSWDomeCameraControlMSG::OnSetAGCGain( WPARAM wParam,LPARAM lParam )
{
    INT iValue = (INT)wParam;

    if (OP_FROM_USER == lParam)
    {
        HANDLEOPERATION(0, 0);
    }

    if (iValue < 0 || iValue > 0xf)
    {
        SW_TRACE_DEBUG("Err: invalid arg wParam(=%d)!\n", iValue);
        return E_INVALIDARG;
    }

    if (SWPAR_OK != swpa_camera_basicparam_set_gain(iValue))
    {
        SW_TRACE_DEBUG("Err: failed to set gain to %d!\n", iValue);
        return E_FAIL;
    }

    return S_OK;
}


HRESULT CSWDomeCameraControlMSG::OnGetAGCGain( WPARAM wParam,LPARAM lParam )
{
    if (NULL == lParam)
    {
        SW_TRACE_DEBUG("Err: invalid arg lParam!\n");
        return E_INVALIDARG;
    }

	if (OP_FROM_USER != wParam) CHECKFSCANSTATUS();

    INT * piValue = (INT*)lParam;

    if (SWPAR_OK != swpa_camera_basicparam_get_gain(piValue))
    {
        SW_TRACE_DEBUG("Err: failed to get shutter!\n");
        return E_FAIL;
    }

    return S_OK;
}


HRESULT CSWDomeCameraControlMSG::OnSetAWBMode( WPARAM wParam,LPARAM lParam )
{
    INT iValue = (INT)wParam;

    if (iValue < AWB_AUTO || iValue > AWB_SODIUM_LAMP)
    {
        SW_TRACE_DEBUG("Err: invalid arg wParam(=%d)!\n", iValue);
        return E_INVALIDARG;
    }
    
    if (OP_FROM_USER == lParam)
    {
        HANDLEOPERATION(0, 0);
    }

    if (SWPAR_OK != swpa_camera_basicparam_set_AWB((AWB_MODE)iValue))
    {
        SW_TRACE_DEBUG("Err: failed to set awb mode to %d!\n", iValue);
        return E_FAIL;
    }

    return S_OK;
}


HRESULT CSWDomeCameraControlMSG::OnGetAWBMode( WPARAM wParam,LPARAM lParam )
{
    if (NULL == lParam)
    {
        SW_TRACE_DEBUG("Err: invalid arg lParam!\n");
        return E_INVALIDARG;
    }

	if (OP_FROM_USER != wParam) CHECKFSCANSTATUS();

    INT * piValue = (INT*)lParam;

    if (SWPAR_OK != swpa_camera_basicparam_get_AWB((AWB_MODE*)piValue))
    {
        SW_TRACE_DEBUG("Err: failed to get awb mode!\n");
        return E_FAIL;
    }

    
    return S_OK;
}


HRESULT CSWDomeCameraControlMSG::OnSetRGBGain(WPARAM wParam,LPARAM lParam)
{
    DWORD *tmp = (DWORD *)wParam;
    if(tmp == NULL || tmp+1 == NULL || tmp+2== NULL)
    {
        SW_TRACE_DEBUG("Err: invalid arg lParam!\n");
        return E_INVALIDARG;
    }

    if (OP_FROM_USER == lParam)
    {
        HANDLEOPERATION(0, 0);
    }

    INT iGainR = tmp[0];
    INT iGainG = tmp[1];
    INT iGainB = tmp[2];

    if (SWPAR_OK != swpa_camera_basicparam_set_rgb_gain(iGainR, iGainG, iGainB))
    {
        SW_TRACE_DEBUG("Err: failed to set rgb to %d,%d,%d!\n",
                iGainR, iGainG, iGainB);
        return E_FAIL;
    }

    return S_OK;
}


HRESULT CSWDomeCameraControlMSG::OnGetRGBGain(WPARAM wParam,LPARAM lParam)
{
    int *temp = (int *)lParam;
    if(temp == NULL || temp+1 == NULL || temp +2 == NULL)
    {
        SW_TRACE_DEBUG("Err: invalid arg lParam!\n");
        return E_INVALIDARG;
    }

	if (OP_FROM_USER != wParam) CHECKFSCANSTATUS();

    if (SWPAR_OK != swpa_camera_basicparam_get_rgb_gain(&temp[0], &temp[1], &temp[2]))
    {
        SW_TRACE_DEBUG("Err: failed to get rgb!\n");
        return E_FAIL;
    }

    return S_OK;
}


HRESULT CSWDomeCameraControlMSG::OnSetSaturationThreshold(WPARAM wParam,LPARAM lParam)
{
    INT iValue = (INT)wParam;

    if (iValue < 0 || iValue > 0x0e)
    {
        SW_TRACE_DEBUG("Err: invalid arg wParam(=%d)!\n", iValue);
        return E_INVALIDARG;
    }

    if (OP_FROM_USER == lParam)
    {
        HANDLEOPERATION(0, 0);
    }

    if (SWPAR_OK != swpa_camera_imgproc_set_saturation(iValue))
    {
        SW_TRACE_DEBUG("Err: failed to set saturation to %d!\n", iValue);
        return E_FAIL;
    }

    return S_OK;
}


HRESULT CSWDomeCameraControlMSG::OnGetSaturationThreshold(WPARAM wParam,LPARAM lParam)
{
    if (NULL == lParam)
    {
        SW_TRACE_DEBUG("Err: invalid arg lParam!\n");
        return E_INVALIDARG;
    }

	if (OP_FROM_USER != wParam) CHECKFSCANSTATUS();

    INT * piValue = (INT*)lParam;

    if (SWPAR_OK != swpa_camera_imgproc_get_saturation(piValue))
    {
        SW_TRACE_DEBUG("Err: failed to get saturation!\n");
        return E_FAIL;
    }

    return S_OK;
}


HRESULT CSWDomeCameraControlMSG::OnSetSharpeThreshold(WPARAM wParam,LPARAM lParam)
{
    INT iValue = (INT)wParam;

    if (iValue < 0 || iValue > 15)
    {
        SW_TRACE_DEBUG("Err: invalid arg wParam(=%d)!\n", iValue);
        return E_INVALIDARG;
    }

    if (OP_FROM_USER == lParam)
    {
        HANDLEOPERATION(0, 0);
    }

    if (SWPAR_OK != swpa_camera_imgproc_set_sharpen_param(0, iValue))
    {
        SW_TRACE_DEBUG("Err: failed to set sharpen to %d!\n", iValue);
        return E_FAIL;
    }

    return S_OK;
}


HRESULT CSWDomeCameraControlMSG::OnGetSharpeThreshold(WPARAM wParam,LPARAM lParam)
{
    if (NULL == lParam)
    {
        SW_TRACE_DEBUG("Err: invalid arg lParam!\n");
        return E_INVALIDARG;
    }

	if (OP_FROM_USER != wParam) CHECKFSCANSTATUS();

    INT * piValue = (INT*)lParam;
    int iMode;

    if (SWPAR_OK != swpa_camera_imgproc_get_sharpen_param(&iMode, piValue))
    {
        SW_TRACE_DEBUG("Err: failed to get sharpen!\n");
        return E_FAIL;
    }

    return S_OK;
}


HRESULT CSWDomeCameraControlMSG::OnSetGammaMode(WPARAM wParam,LPARAM lParam)
{
    INT iValue = (INT)wParam;

    if (iValue < 0 || iValue > 4)
    {
        SW_TRACE_DEBUG("Err: invalid arg wParam(=%d)!\n", iValue);
        return E_INVALIDARG;
    }

    if (OP_FROM_USER == lParam)
    {
        HANDLEOPERATION(0, 0);
    }

    if (SWPAR_OK != swpa_camera_imgproc_set_gamma_type(iValue))
    {
        SW_TRACE_DEBUG("Err: failed to set gamma to %d!\n", iValue);
        return E_FAIL;
    }

    return S_OK;
}


HRESULT CSWDomeCameraControlMSG::OnGetGammaMode(WPARAM wParam,LPARAM lParam)
{
    if (NULL == lParam)
    {
        SW_TRACE_DEBUG("Err: invalid arg lParam!\n");
        return E_INVALIDARG;
    }

	if (OP_FROM_USER != wParam) CHECKFSCANSTATUS();

    INT * piValue = (INT*)lParam;

    if (SWPAR_OK != swpa_camera_imgproc_get_gamma_type(piValue))
    {
        SW_TRACE_DEBUG("Err: failed to get gamma!\n");
        return E_FAIL;
    }

    return S_OK;
}


HRESULT CSWDomeCameraControlMSG::OnSetWDRStrength(WPARAM wParam,LPARAM lParam)
{
    INT iValue = (INT)wParam;

    if (iValue < 0 || iValue > 1)
    {
        SW_TRACE_DEBUG("Err: invalid arg wParam(=%d)!\n", iValue);
        return E_INVALIDARG;
    }

    if (OP_FROM_USER == lParam)
    {
        HANDLEOPERATION(0, 0);
    }

    WDR_MODE eWDRMode = 0 == iValue ? WDR_OFF : WDR_ON;
    if (SWPAR_OK != swpa_camera_imgproc_set_WDR(eWDRMode))
    {
        SW_TRACE_DEBUG("Err: failed to set WDR to %d!\n", iValue);
        return E_FAIL;
    }

    return S_OK;
}


HRESULT CSWDomeCameraControlMSG::OnGetWDRStrength(WPARAM wParam,LPARAM lParam)
{
    if (NULL == lParam)
    {
        SW_TRACE_DEBUG("Err: invalid arg lParam!\n");
        return E_INVALIDARG;
    }

	if (OP_FROM_USER != wParam) CHECKFSCANSTATUS();

    INT * piValue = (INT*)lParam;
    WDR_MODE eWDRMode = WDR_OFF;

    if (SWPAR_OK != swpa_camera_imgproc_get_WDR(&eWDRMode))
    {
        SW_TRACE_DEBUG("Err: failed to get WDR!\n");
        return E_FAIL;
    }

    *piValue = WDR_OFF == eWDRMode ? 0 : 1;

    return S_OK;
}


HRESULT CSWDomeCameraControlMSG::OnSetNRLevel(WPARAM wParam,LPARAM lParam)
{
    INT iValue = (INT)wParam;

    if (iValue < 0 || iValue > 5)
    {
        SW_TRACE_DEBUG("Err: invalid arg wParam(=%d)!\n", iValue);
        return E_INVALIDARG;
    }

    if (OP_FROM_USER == lParam)
    {
        HANDLEOPERATION(0, 0);
    }

    if (SWPAR_OK != swpa_camera_imgproc_set_NR(iValue))
    {
        SW_TRACE_DEBUG("Err: failed to set NR to %d!\n", iValue);
        return E_FAIL;
    }
    
    return S_OK;
}


HRESULT CSWDomeCameraControlMSG::OnGetNRLevel(WPARAM wParam,LPARAM lParam)
{
    if (NULL == lParam)
    {
        SW_TRACE_DEBUG("Err: invalid arg lParam!\n");
        return E_INVALIDARG;
    }

	if (OP_FROM_USER != wParam) CHECKFSCANSTATUS();

    INT * piValue = (INT*)lParam;

    if (SWPAR_OK != swpa_camera_imgproc_get_NR(piValue))
    {
        SW_TRACE_DEBUG("Err: failed to get NRLevel!\n");
        return E_FAIL;
    }

    return S_OK;
}



HRESULT CSWDomeCameraControlMSG::OnSetEdgeEnhance(WPARAM wParam,LPARAM lParam)
{
    DWORD dwValue = (DWORD)wParam;
    if (dwValue < 0 || dwValue > 255)
    {
        SW_TRACE_DEBUG("Err: invalid arg wParam(=%d)!\n", dwValue);
        return E_INVALIDARG;
    }

    return 0 == swpa_ipnc_control(0, CMD_SET_EDGE_ENHANCE ,(void*)&dwValue , sizeof(DWORD*), 0) ? S_OK : E_FAIL;
}



HRESULT CSWDomeCameraControlMSG::OnGetEdgeEnhance(WPARAM wParam,LPARAM lParam)
{
    if (OP_FROM_USER == lParam)
    {
        SW_TRACE_DEBUG("Err: invalid arg lParam(=%d)!\n", lParam);
        return E_INVALIDARG;
    }
    
    return 0 == swpa_ipnc_control(0, CMD_GET_EDGE_ENHANCE ,(void*)lParam , sizeof(DWORD*), 0) ? S_OK : E_FAIL;
}


HRESULT CSWDomeCameraControlMSG::OnSetExpoCompValue(WPARAM wParam,LPARAM lParam)
{
    INT iValue = (INT)wParam;
    if (0x0 > iValue || 0xE < iValue)
    {
        SW_TRACE_DEBUG("Err: invalid arg wParam(=%d)!\n", wParam);
        return E_INVALIDARG;
    }
    
    if (SWPAR_OK != swpa_camera_basicparam_set_expcomp_value(iValue))
    {
        SW_TRACE_DEBUG("Err: failed to set exposure compensation value to %d!\n", iValue);
        return E_FAIL;;
    }

    return S_OK;
}

HRESULT CSWDomeCameraControlMSG::OnGetExpoCompValue(WPARAM wParam,LPARAM lParam)
{
    INT iValue = 0;
    if (OP_FROM_USER == lParam)
    {
        SW_TRACE_DEBUG("Err: invalid arg lParam(=%d)!\n", lParam);
        return E_INVALIDARG;
    }

	if (OP_FROM_USER != wParam) CHECKFSCANSTATUS();
    
    if (SWPAR_OK != swpa_camera_basicparam_get_expcomp_value(&iValue))
    {
        SW_TRACE_DEBUG("Err: failed to get exposure compensation value!\n");
        return E_FAIL;;
    }

    *(INT*)lParam = iValue;

    return S_OK;
}


HRESULT CSWDomeCameraControlMSG::OnCameraDoCmd(WPARAM wParam,LPARAM lParam)
{
    PBYTE pbBuf = (PBYTE)wParam;
    INT iCmdLen = (INT)lParam;
    if (NULL == pbBuf || 0 >= iCmdLen)
    {
        SW_TRACE_DEBUG("Err: invalid arg 0x%x, 0x%x!\n", wParam, lParam);
        return E_INVALIDARG;
    }

    printf("iCmdLen = %d\n", iCmdLen);
    for (int i = 0; i<iCmdLen; i++)
    {
        printf("0x%x ", pbBuf[i]);
    }
    printf("\n");

    INT iRet = spwa_camera_send_cmd(PROTOCOL_VISCA, pbBuf, iCmdLen);
    
    if (SWPAR_OK != iRet)
    {
        SW_TRACE_DEBUG("Err: failed to do cmd %d!\n", iRet);
        return E_FAIL;;
    }

    return S_OK;
}


HRESULT CSWDomeCameraControlMSG::OnPTZControl(PVOID pvBuffer, INT iSize)
{
    SW_TRACE_DEBUG("CSWDomeCameraControlMSG::OnPTZ\n");
    
    DWORD* pdwMsg = (DWORD*)pvBuffer;
    DWORD dwCmd = *pdwMsg;
    DWORD dwDir = *(pdwMsg+1);
    DWORD dwSpeed = *(pdwMsg+2);
    DWORD dwPresetID = *(pdwMsg+1);
    //CHAR* szPresetName = (CHAR*)*(pdwMsg+2);
    

    switch (dwCmd)
    {
        case MSG_START_PAN:
            CSWMessage::SendMessage(MSG_SET_PAN_SPEED,WPARAM(dwSpeed), 0);
            CSWMessage::SendMessage(MSG_START_PAN,WPARAM(dwDir), 0);
        break;

        case MSG_STOP_PAN:
            CSWMessage::SendMessage(MSG_STOP_PAN, 0, 0);
        break;

        case MSG_START_TILT:
            CSWMessage::SendMessage(MSG_SET_TILT_SPEED,WPARAM(dwSpeed), 0);
            CSWMessage::SendMessage(MSG_START_TILT,WPARAM(dwDir), 0);
        break;

        case MSG_STOP_TILT:
            CSWMessage::SendMessage(MSG_STOP_TILT, 0, 0);
        break;
        
        case MSG_START_PAN_TILT:
            CSWMessage::SendMessage(MSG_SET_PAN_SPEED,WPARAM(dwSpeed), 0);
            CSWMessage::SendMessage(MSG_SET_TILT_SPEED,WPARAM(dwSpeed), 0);
            CSWMessage::SendMessage(MSG_START_PAN_TILT,WPARAM(dwDir), 0);
        break;

        case MSG_STOP_PAN_TILT:
            CSWMessage::SendMessage(MSG_STOP_PAN_TILT, 0, 0);
        break;

        case MSG_START_ZOOM:
            //CSWMessage::SendMessage(MSG_SET_PAN_SPEED,WPARAM(dwSpeed), 0);
            CSWMessage::SendMessage(MSG_START_ZOOM,WPARAM(dwDir), 0);
        break;

        case MSG_STOP_ZOOM:
            CSWMessage::SendMessage(MSG_STOP_ZOOM, 0, 0);
        break;

        case MSG_SET_PT_COORDINATE:
        {
            DWORD dwPTCoord = *(pdwMsg+1);
            CSWMessage::SendMessage(MSG_SET_PT_COORDINATE, WPARAM(dwPTCoord), 0);
        }
        break;

        case MSG_SET_ZOOM:
        {
            DWORD dwZoom = *(pdwMsg+1);
            CSWMessage::SendMessage(MSG_SET_PT_COORDINATE, WPARAM(dwZoom), 0);
        }
        break;

        case MSG_CALL_PRESET_POS:
        {
            CSWMessage::SendMessage(MSG_CALL_PRESET_POS, WPARAM(dwPresetID), 0);
        }
        break;

        case MSG_SET_PRESET_POS:
        {
            PRESETPOS_PARAM_STRUCT sParam;
            swpa_memset(&sParam, 0, sizeof(sParam));

            //get first, to keep name and flag unchanged
            CSWMessage::SendMessage(MSG_GET_PRESET_POS, WPARAM(dwPresetID), LPARAM(&sParam));
            
            //if (NULL != szPresetName && swpa_strlen(szPresetName) > 0 && swpa_strlen(szPresetName) < 32)
            //{
            //    swpa_strcpy(sParam.szName, szPresetName);
            //}
            
            CSWMessage::SendMessage(MSG_SET_PRESET_POS, WPARAM(dwPresetID), LPARAM(&sParam));
        }
        break;

        case MSG_CLEAR_PRESET_POS:
            CSWMessage::SendMessage(MSG_CLEAR_PRESET_POS, WPARAM(dwPresetID), 0);
        break;
    }

    return S_OK;
}


HRESULT CSWDomeCameraControlMSG::OnGetPTZInfo(PVOID pvBuffer, INT iSize)
{
    SW_TRACE_DEBUG("Info: got GetPTZInfo msg...\n");

    //send remote msg to notify the ptz info
    PTZ_INFO sInfo;
    
    sInfo.fHomeSupported = TRUE;
    sInfo.fFixedHomePos = TRUE;
    sInfo.iPresetCount = 256;

    sInfo.fAbsoluteMoveSupport = TRUE;
    sInfo.iAbsolutePRangeMax = 3600;
    sInfo.iAbsolutePRangeMin = 0;
    sInfo.iAbsoluteTRangeMax = 900;
    sInfo.iAbsoluteTRangeMin = 0;
    sInfo.iAbsoluteZRangeMax = 30;
    sInfo.iAbsoluteZRangeMin = 1;

    sInfo.fContinuousMoveSupport = TRUE;
    sInfo.iContinuousPSpeedMax = 30;
    sInfo.iContinuousPSpeedMin = 1;
    sInfo.iContinuousTSpeedMax = 30;
    sInfo.iContinuousTSpeedMin = 1;
    sInfo.iContinuousZSpeedMax = 7;
    sInfo.iContinuousZSpeedMin = 1;
    
    return SendRemoteMessage(MSG_APP_REMOTE_SET_PTZ_INFO, (PVOID)&sInfo, sizeof(sInfo));
}


HRESULT CSWDomeCameraControlMSG::OnGetDomeVersion(WPARAM wParam, LPARAM lParam)
{
	INT *pDomeVersion = (INT *)lParam;

	if (SWPAR_OK != swpa_camera_ball_get_dome_version(pDomeVersion))
    {
        SW_TRACE_DEBUG("Err: failed to get dome version %d.%d\n", 
					   (*pDomeVersion >> 8) & 0xFF, (*pDomeVersion) & 0xFF);
        return E_FAIL;
    }

	return S_OK;
}

HRESULT CSWDomeCameraControlMSG::OnGetCameraVersion(WPARAM wParam, LPARAM lParam)
{
	CHAR CameraVersion[8] = {0};

	if (SWPAR_OK != swpa_camera_ball_get_camera_version(CameraVersion))
    {
        SW_TRACE_DEBUG("Err: failed to get camera version\n");
					   
        return E_FAIL;
    }
	swpa_memcpy((CHAR *)lParam, CameraVersion, 8);

	return S_OK;
}

HRESULT CSWDomeCameraControlMSG::OnGetCameraModel(WPARAM wParam, LPARAM lParam)
{
	INT *Model = (INT *)lParam;
	
	if(m_pCamera != NULL)
	{
		*Model = m_pCamera->GetCamID();
		return S_OK;
	}

	return E_FAIL;
}




//========================================================================
