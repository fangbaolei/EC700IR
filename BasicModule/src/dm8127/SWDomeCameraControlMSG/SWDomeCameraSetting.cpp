
#include "SWFC.h"
#include "SWClassFactory.h"
#include "SWDomeCameraSetting.h"
#include "SWDomeCameraPreset.h"


REGISTER_CLASS(CSWDomeCameraSetting)

CSWDomeCameraSetting::CSWDomeCameraSetting()
{
    //preset 
    
    for (DWORD i = 0; i < MAX_PRESET; i++)
    {
    	swpa_memset(&Get().sPresetParam[i], 0, sizeof(Get().sPresetParam[i]));
    	swpa_snprintf(Get().sPresetParam[i].szName, 31, "预置位%d", i);
		if (FUNC_ZERO_CORRECTION == i)
		{
			swpa_snprintf(Get().sPresetParam[i].szName, 31, "0°校正");
		}
		else if(FUNC_DEFOG_START == i)
		{
			swpa_snprintf(Get().sPresetParam[i].szName, 31, "启动除雾");
		}
    	Get().sPresetParam[i].iFocusMode = 2;
		Get().sPresetParam[i].iPanCoordinate = 14*i;
        Get().sPresetParam[i].iTiltCoordinate = 50; //Version B dome fw : 0 ~950
		Get().sPresetParam[i].fValid = TRUE;
    }

	//hscan
    for (DWORD i = 0; i < MAX_HSCAN_PATH; i++)
    {
    	swpa_memset(&Get().sHScanParam[i], 0, sizeof(Get().sHScanParam[i]));
    	swpa_snprintf(Get().sHScanParam[i].szName, 31, "水平扫描%d", i);
		Get().sHScanParam[i].iSpeed = 15;//default
    }

	//fscan
    for (DWORD i = 0; i < MAX_FSCAN_PATH; i++)
    {
    	swpa_memset(&Get().sFScanParam[i], 0, sizeof(Get().sFScanParam[i]));
    	swpa_snprintf(Get().sFScanParam[i].szName, 31, "花样扫描%d", i);
    	Get().sFScanParam[i].sInitialState.iFocusMode = 2;
    }

	//cruise
    for (DWORD i = 0; i < MAX_CRUISE; i++)
    {
    	swpa_memset(&Get().sCruiseParam[i], 0, sizeof(Get().sCruiseParam[i]));
    	swpa_snprintf(Get().sCruiseParam[i].szName, 31, "巡航扫描%d", i);
    }

	//picturefreeze
	Get().sPictureFreezeParam.fEnable = FALSE;

	//mask
	for (DWORD i = 0; i < MAX_MASK; i++)
    {
    	swpa_memset(&Get().sMaskParam[i], 0, sizeof(Get().sMaskParam[i]));
    	swpa_snprintf(Get().sMaskParam[i].szName, 31, "隐私区域%d", i);
		Get().sMaskParam[i].iPresetID = -1;
    }

	//watchkeeping
	swpa_memset(&Get().sWatchKeepingParam, 0, sizeof(Get().sWatchKeepingParam));
	Get().sWatchKeepingParam.iMotionType = MOVETYPE_CRUISE;
	Get().sWatchKeepingParam.iWatchTime = 300;

	//timer
	for (DWORD i = 0; i < MAX_TIMER; i++)
    {
    	swpa_memset(&Get().sTimerParam[i], 0, sizeof(Get().sTimerParam[i]));
		Get().sTimerParam[i].iMotionType = MOVETYPE_CRUISE;
		Get().sTimerParam[i].iMotionID = 0;
		Get().sTimerParam[i].iBeginTime = 0;
		Get().sTimerParam[i].iEndTime = 1439;
    	swpa_snprintf(Get().sTimerParam[i].szName, 31, "定时任务%d", i);
    }
	
	//standby
	swpa_memset(&Get().sStandbyParam, 0, sizeof(Get().sStandbyParam));

	// 有效的值。
	Get().iFocusMode = 2;

}

CSWDomeCameraSetting::~CSWDomeCameraSetting()
{
}

HRESULT CSWDomeCameraSetting::Initialize(CSWString& strFilePath)
{
	m_strFilePath = strFilePath;
	if(S_OK == CSWParameter<DomeCameraParams>::Initialize(strFilePath)
		&& S_OK == InitBasicParam()
        && S_OK == InitPreset()
        && S_OK == InitHScan()
        && S_OK == InitFScan()
        && S_OK == InitCruise() 
        && S_OK == InitWatchKeeping()    
        && S_OK == InitTimer()
        && S_OK == InitMask()
        && S_OK == InitPictureFreeze()
        && S_OK == InitSleep()
	    )
	{
		return S_OK;		
	}

	return E_FAIL;
}




/**
*@brief 初始化基本参数
*@return 成功返回S_OK,其他值表示失败
*/
HRESULT CSWDomeCameraSetting::InitBasicParam()
{
    GetInt("\\DomeCamera\\Basic"
		, "PanCoord"
		, &Get().iPanCoordinate
		, Get().iPanCoordinate
		, 0
		, 3599 
		, "test"
		, "test"
		, CUSTOM_LEVEL
	);

	GetInt("\\DomeCamera\\Basic"
		, "TiltCoord"
		, &Get().iTiltCoordinate
		, Get().iTiltCoordinate
		, 0
		, 900 
		, "test"
		, "test"
		, CUSTOM_LEVEL
	);

	
	GetInt("\\DomeCamera\\Basic"
		, "PanSpeed"
		, &Get().iPanSpeed
		, Get().iPanSpeed
		, 0
		, MAX_SPEED //0x3F 
		, "test"
		, "test"
		, CUSTOM_LEVEL
	);

	GetInt("\\DomeCamera\\Basic"
		, "TiltSpeed"
		, &Get().iTiltSpeed
		, Get().iTiltSpeed
		, 0
		, MAX_SPEED //0x3F 
		, "test"
		, "test"
		, CUSTOM_LEVEL
	);

	GetInt("\\DomeCamera\\Basic"
		, "MaxTiltSpeed"
		, &Get().iMaxTiltSpeed
		, Get().iMaxTiltSpeed
		, 0
		, MAX_SPEED //0x3F 
		, "test"
		, "test"
		, CUSTOM_LEVEL
	);

	GetInt("\\DomeCamera\\Basic"
		, "MaxPanSpeed"
		, &Get().iMaxPanSpeed
		, Get().iMaxPanSpeed
		, 0
		, MAX_SPEED //0x3F 
		, "test"
		, "test"
		, CUSTOM_LEVEL
	);

	GetInt("\\DomeCamera\\Basic"
		, "FocusMode"
		, &Get().iFocusMode
		, Get().iFocusMode
		, 2
		, 3  //todo
		, "test"
		, "test"
		, CUSTOM_LEVEL
	);

	GetInt("\\DomeCamera\\Basic"
		, "LEDMode"
		, &Get().iLEDMode
		, Get().iLEDMode
		, 0
		, 2
		, "test"
		, "test"
		, CUSTOM_LEVEL
	);

	GetInt("\\DomeCamera\\Basic"
		, "LEDPower"
		, &Get().iLEDPower
		, Get().iLEDPower
		, 0
		, 3000 // = 30.00 倍
		, "test"
		, "test"
		, CUSTOM_LEVEL
	);

	
	GetInt("\\DomeCamera\\Basic"
		, "LastTask"
		, &Get().iLastTask
		, Get().iLastTask
		, -1
		, MOVETYPE_END - 1 
		, "test"
		, "test"
		, CUSTOM_LEVEL
	);

	GetInt("\\DomeCamera\\Basic"
		, "LastTaskID"
		, &Get().iLastTaskID
		, Get().iLastTaskID
		, -1
		, 1024 
		, "test"
		, "test"
		, CUSTOM_LEVEL
	);
	
    return S_OK;
}



/**
*@brief 初始化预置位
*@return 成功返回S_OK,其他值表示失败
*/
HRESULT CSWDomeCameraSetting::InitPreset()
{

	for (DWORD i = 0; i < MAX_PRESET; i++)
	{
		CHAR szSection[256];
        swpa_sprintf(szSection, "\\DomeCamera\\Preset\\Pos%d", i);

		GetString(szSection
	    	, "Name"
	    	, Get().sPresetParam[i].szName
	    	, Get().sPresetParam[i].szName
	    	, 31
	    	, "test"
	    	, "test"
	    	, CUSTOM_LEVEL
	    );

		GetInt(szSection
	    	, "PanCoordinate"
	    	, &Get().sPresetParam[i].iPanCoordinate
	    	, Get().sPresetParam[i].iPanCoordinate
	    	, 0
	    	, 3599
	    	, "test"
	    	, "test"
	    	, CUSTOM_LEVEL
	    );

		GetInt(szSection
	    	, "TiltCoordinate"
	    	, &Get().sPresetParam[i].iTiltCoordinate
	    	, Get().sPresetParam[i].iTiltCoordinate
	    	, 0
	    	, 900
	    	, "test"
	    	, "test"
	    	, CUSTOM_LEVEL
	    );

		GetInt(szSection
	    	, "Zoom"
	    	, &Get().sPresetParam[i].iZoom
	    	, Get().sPresetParam[i].iZoom
	    	, 0
	    	, 31424
	    	, "test"
	    	, "test"
	    	, CUSTOM_LEVEL
	    );

		GetInt(szSection
	    	, "Iris"
	    	, &Get().sPresetParam[i].iIris
	    	, Get().sPresetParam[i].iIris
	    	, 0
	    	, 0x11
	    	, "test"
	    	, "test"
	    	, CUSTOM_LEVEL
	    );

		GetInt(szSection
	    	, "Focus"
	    	, &Get().sPresetParam[i].iFocus
	    	, Get().sPresetParam[i].iFocus
	    	, 0x1000
	    	, 0xc000
	    	, "test"
	    	, "test"
	    	, CUSTOM_LEVEL
	    );

		GetInt(szSection
	    	, "FocusMode"
	    	, &Get().sPresetParam[i].iFocusMode
	    	, Get().sPresetParam[i].iFocusMode
	    	, 2
	    	, 3
	    	, "test"
	    	, "test"
	    	, CUSTOM_LEVEL
	    );

		GetInt(szSection
	    	, "CheckPT"
	    	, &Get().sPresetParam[i].fCheckPT
	    	, Get().sPresetParam[i].fCheckPT
	    	, 0
	    	, 1
	    	, "test"
	    	, "test"
	    	, CUSTOM_LEVEL
	    );

		GetInt(szSection
	    	, "Valid"
	    	, &Get().sPresetParam[i].fValid
	    	, Get().sPresetParam[i].fValid
	    	, 0
	    	, 1
	    	, "test"
	    	, "test"
	    	, CUSTOM_LEVEL
	    );
	}

	SW_TRACE_DEBUG("Info: Preset Setting init -- ok\n");
	
    return S_OK;
}




/**
*@brief 初始化水平扫描
*@return 成功返回S_OK,其他值表示失败
*/
HRESULT CSWDomeCameraSetting::InitHScan()
{
	
    for (DWORD i = 0; i < MAX_HSCAN_PATH; i++)
	{
		char szSection[256];
        swpa_sprintf(szSection, "\\DomeCamera\\HScan\\Path%d", i);

		GetString(szSection
	    	, "Name"
	    	, Get().sHScanParam[i].szName
	    	, Get().sHScanParam[i].szName
	    	, 31
	    	, "test"
	    	, "test"
	    	, CUSTOM_LEVEL
	    );

		GetInt(szSection
	    	, "LeftPos"
	    	, &Get().sHScanParam[i].iLeftPos
	    	, Get().sHScanParam[i].iLeftPos
	    	, 0
	    	, 3599
	    	, "test"
	    	, "test"
	    	, CUSTOM_LEVEL
	    );

		GetInt(szSection
	    	, "RightPos"
	    	, &Get().sHScanParam[i].iRightPos
	    	, Get().sHScanParam[i].iRightPos
	    	, 0
	    	, 3599
	    	, "test"
	    	, "test"
	    	, CUSTOM_LEVEL
	    );

		GetInt(szSection
	    	, "Direction"
	    	, &Get().sHScanParam[i].iDirection
	    	, Get().sHScanParam[i].iDirection
	    	, 0
	    	, 1
	    	, "test"
	    	, "test"
	    	, CUSTOM_LEVEL
	    );

		Get().sHScanParam[i].iSpeed = 15;
		GetInt(szSection
	    	, "Speed"
	    	, &Get().sHScanParam[i].iSpeed
	    	, Get().sHScanParam[i].iSpeed
	    	, 0
	    	, 30 //differs from other speed
	    	, "test"
	    	, "test"
	    	, CUSTOM_LEVEL
	    );

		Get().sHScanParam[i].fValid = TRUE;
		GetInt(szSection
	    	, "Valid"
	    	, &Get().sHScanParam[i].fValid
	    	, Get().sHScanParam[i].fValid
	    	, 0
	    	, 1
	    	, "test"
	    	, "test"
	    	, CUSTOM_LEVEL
	    );		
	}
	
	SW_TRACE_DEBUG("Info: HScan Setting init -- ok\n");
    return S_OK;
}


/**
*@brief 初始化巡航
*@return 成功返回S_OK,其他值表示失败
*/
HRESULT CSWDomeCameraSetting::InitFScan()
{
    for (DWORD i = 0; i < MAX_FSCAN_PATH; i++)
	{
		char szSection[256];
        swpa_sprintf(szSection, "\\DomeCamera\\FScan\\Path%d", i);

		GetString(szSection
	    	, "Name"
	    	, Get().sFScanParam[i].szName
	    	, Get().sFScanParam[i].szName
	    	, 31
	    	, "test"
	    	, "test"
	    	, CUSTOM_LEVEL
	    );

		GetInt(szSection
	    	, "PanCoordinate"
	    	, &Get().sFScanParam[i].sInitialState.iPanCoordinate
	    	, Get().sFScanParam[i].sInitialState.iPanCoordinate
	    	, 0
	    	, 3599
	    	, "test"
	    	, "test"
	    	, CUSTOM_LEVEL
	    );

		GetInt(szSection
	    	, "TiltCoordinate"
	    	, &Get().sFScanParam[i].sInitialState.iTiltCoordinate
	    	, Get().sFScanParam[i].sInitialState.iTiltCoordinate
	    	, 0
	    	, 3599
	    	, "test"
	    	, "test"
	    	, CUSTOM_LEVEL
	    );

		GetInt(szSection
	    	, "Zoom"
	    	, &Get().sFScanParam[i].sInitialState.iZoom
	    	, Get().sFScanParam[i].sInitialState.iZoom
	    	, 0
	    	, 31424
	    	, "test"
	    	, "test"
	    	, CUSTOM_LEVEL
	    );

		GetInt(szSection
	    	, "Iris"
	    	, &Get().sFScanParam[i].sInitialState.iIris
	    	, Get().sFScanParam[i].sInitialState.iIris
	    	, 0
	    	, 0x11
	    	, "test"
	    	, "test"
	    	, CUSTOM_LEVEL
	    );

		GetInt(szSection
	    	, "Focus"
	    	, &Get().sFScanParam[i].sInitialState.iFocus
	    	, Get().sFScanParam[i].sInitialState.iFocus
	    	, 0x1000
	    	, 0xc000
	    	, "test"
	    	, "test"
	    	, CUSTOM_LEVEL
	    );

		GetInt(szSection
	    	, "FocusMode"
	    	, &Get().sFScanParam[i].sInitialState.iFocusMode
	    	, Get().sFScanParam[i].sInitialState.iFocusMode
	    	, 2
	    	, 3
	    	, "test"
	    	, "test"
	    	, CUSTOM_LEVEL
	    );

		GetInt(szSection
	    	, "PanSpeed"
	    	, &Get().sFScanParam[i].sInitialState.iPanSpeed
	    	, Get().sFScanParam[i].sInitialState.iPanSpeed
	    	, 0
	    	, MAX_SPEED
	    	, "test"
	    	, "test"
	    	, CUSTOM_LEVEL
	    );

		
		GetInt(szSection
			, "TiltSpeed"
			, &Get().sFScanParam[i].sInitialState.iTiltSpeed
			, Get().sFScanParam[i].sInitialState.iTiltSpeed
			, 0
			, MAX_SPEED
			, "test"
			, "test"
			, CUSTOM_LEVEL
		);

		GetInt(szSection
	    	, "MotionCount"
	    	, &Get().sFScanParam[i].iMotionCount
	    	, Get().sFScanParam[i].iMotionCount
	    	, 0
	    	, MAX_MOTION_IN_FSCAN
	    	, "test"
	    	, "test"
	    	, CUSTOM_LEVEL
	    );
		
		GetInt(szSection
	    	, "Valid"
	    	, &Get().sFScanParam[i].fValid
	    	, Get().sFScanParam[i].fValid
	    	, 0
	    	, 1
	    	, "test"
	    	, "test"
	    	, CUSTOM_LEVEL
	    );
		
		for (DWORD j = 0; j < MAX_MOTION_IN_FSCAN; j++)
		{
			swpa_sprintf(szSection, "\\DomeCamera\\FScan\\Path%d\\Motion%d", i, j);
			GetInt(szSection
		    	, "Move"
		    	, &Get().sFScanParam[i].sMotion[j].iMove
		    	, Get().sFScanParam[i].sMotion[j].iMove
		    	, 0
		    	, 1000000000 //todo: 2^31
		    	, "test"
		    	, "test"
		    	, CUSTOM_LEVEL
		    );

			GetInt(szSection
		    	, "Arg"
		    	, &Get().sFScanParam[i].sMotion[j].iArg
		    	, Get().sFScanParam[i].sMotion[j].iArg
		    	, 0
		    	, 1000000000 //todo: 2^31
		    	, "test"
		    	, "test"
		    	, CUSTOM_LEVEL
		    );			
		}
	}

	
	SW_TRACE_DEBUG("Info: FScan Setting init -- ok\n");

    return S_OK;
}


/**
*@brief 初始化花样扫描
*@return 成功返回S_OK,其他值表示失败
*/
HRESULT CSWDomeCameraSetting::InitCruise()
{
    for (DWORD i = 0; i < MAX_CRUISE; i++)
	{
		char szSection[256];
        swpa_sprintf(szSection, "\\DomeCamera\\Cruise\\Path%d", i);

		GetString(szSection
	    	, "Name"
	    	, Get().sCruiseParam[i].szName
	    	, Get().sCruiseParam[i].szName
	    	, 31
	    	, "test"
	    	, "test"
	    	, CUSTOM_LEVEL
	    );

		GetInt(szSection
	    	, "CheckPT"
	    	, &Get().sCruiseParam[i].fCheckPT
	    	, Get().sCruiseParam[i].fCheckPT
	    	, 0
	    	, 1
	    	, "test"
	    	, "test"
	    	, CUSTOM_LEVEL
	    );
		
		GetInt(szSection
	    	, "Valid"
	    	, &Get().sCruiseParam[i].fValid
	    	, Get().sCruiseParam[i].fValid
	    	, 0
	    	, 1
	    	, "test"
	    	, "test"
	    	, CUSTOM_LEVEL
	    );

		GetInt(szSection
	    	, "MotionCount"
	    	, &Get().sCruiseParam[i].iMotionCount
	    	, Get().sCruiseParam[i].iMotionCount
	    	, 0
	    	, MAX_MOTION_IN_CRUISE
	    	, "test"
	    	, "test"
	    	, CUSTOM_LEVEL
	    );

		for (DWORD j = 0; j < MAX_MOTION_IN_CRUISE; j++)
		{
			swpa_sprintf(szSection, "\\DomeCamera\\Cruise\\Path%d\\Motion%d", i, j);
			
			GetInt(szSection
		    	, "MoveType"
		    	, &Get().sCruiseParam[i].sMotion[j].iMoveType
		    	, Get().sCruiseParam[i].sMotion[j].iMoveType
		    	, 0
		    	, 1000000000 //todo: 2^31
		    	, "test"
		    	, "test"
		    	, CUSTOM_LEVEL
		    );

			GetInt(szSection
		    	, "MoveID"
		    	, &Get().sCruiseParam[i].sMotion[j].iMoveID
		    	, Get().sCruiseParam[i].sMotion[j].iMoveID
		    	, 0
		    	, 1000000000 //todo: 2^31
		    	, "test"
		    	, "test"
		    	, CUSTOM_LEVEL
		    );

			GetInt(szSection
		    	, "LingerTime"
		    	, &Get().sCruiseParam[i].sMotion[j].iLingerTime
		    	, Get().sCruiseParam[i].sMotion[j].iLingerTime
		    	, 0
		    	, 1000000000 //todo: 2^31
		    	, "test"
		    	, "test"
		    	, CUSTOM_LEVEL
		    );
		}

		
	}

	SW_TRACE_DEBUG("Info: Cruise Setting init -- ok\n");


	return S_OK;
}



/**
*@brief 初始化守望
*@return 成功返回S_OK,其他值表示失败
*/
HRESULT CSWDomeCameraSetting::InitWatchKeeping()
{
    GetInt("\\DomeCamera\\WatchKeeping"
    	, "Enable"
    	, &Get().sWatchKeepingParam.fEnable
    	, Get().sWatchKeepingParam.fEnable
    	, 0
    	, 1
    	, "test"
    	, "test"
    	, CUSTOM_LEVEL
    );

	Get().sWatchKeepingParam.iWatchTime = 300;
	GetInt("\\DomeCamera\\WatchKeeping"
    	, "WatchTime"
    	, &Get().sWatchKeepingParam.iWatchTime
    	, Get().sWatchKeepingParam.iWatchTime
    	, 120
    	, 1000000000 //todo: 2^31
    	, "test"
    	, "test"
    	, CUSTOM_LEVEL
    );

	GetInt("\\DomeCamera\\WatchKeeping"
    	, "MotionType"
    	, &Get().sWatchKeepingParam.iMotionType
    	, Get().sWatchKeepingParam.iMotionType
    	, 0
    	, 1000000000 //todo: 2^31
    	, "test"
    	, "test"
    	, CUSTOM_LEVEL
    );

	GetInt("\\DomeCamera\\WatchKeeping"
    	, "MotionID"
    	, &Get().sWatchKeepingParam.iMotionID
    	, Get().sWatchKeepingParam.iMotionID
    	, 0
    	, 1000000000 //todo: 2^31
    	, "test"
    	, "test"
    	, CUSTOM_LEVEL
    );
	SW_TRACE_DEBUG("Info: Watchkeeping Setting init -- ok\n");


    return S_OK;
}

/**
*@brief 初始化定时
*@return 成功返回S_OK,其他值表示失败
*/
HRESULT CSWDomeCameraSetting::InitTimer()
{
	for (DWORD i = 0; i < MAX_TIMER; i++)
	{
		char szSection[256];
        swpa_sprintf(szSection, "\\DomeCamera\\Timer\\Timer%d", i);

		GetString(szSection
	    	, "Name"
	    	, Get().sTimerParam[i].szName
	    	, Get().sTimerParam[i].szName
	    	, 31
	    	, "test"
	    	, "test"
	    	, CUSTOM_LEVEL
	    );		
		
		GetInt(szSection
	    	, "Weekday"
	    	, &Get().sTimerParam[i].iWeekday
	    	, Get().sTimerParam[i].iWeekday
	    	, 0
	    	, 0x1111111
	    	, "test"
	    	, "test"
	    	, CUSTOM_LEVEL
	    );

		GetInt(szSection
	    	, "BeginTime"
	    	, &Get().sTimerParam[i].iBeginTime
	    	, Get().sTimerParam[i].iBeginTime
	    	, 0
	    	, 24*60 
	    	, "单位:分钟"
	    	, "test"
	    	, CUSTOM_LEVEL
	    );

		GetInt(szSection
	    	, "EndTime"
	    	, &Get().sTimerParam[i].iEndTime
	    	, Get().sTimerParam[i].iEndTime
	    	, 0
	    	, 24*60 
	    	, "单位:分钟"
	    	, "test"
	    	, CUSTOM_LEVEL
	    );
		
		GetInt(szSection
	    	, "MotionType"
	    	, &Get().sTimerParam[i].iMotionType
	    	, Get().sTimerParam[i].iMotionType
	    	, MOVETYPE_BEGIN
	    	, MOVETYPE_END
	    	, "test"
	    	, "test"
	    	, CUSTOM_LEVEL
	    );

		GetInt(szSection
	    	, "MotionID"
	    	, &Get().sTimerParam[i].iMotionID
	    	, Get().sTimerParam[i].iMotionID
	    	, 0
	    	, 65535
	    	, "test"
	    	, "test"
	    	, CUSTOM_LEVEL
	    );


		GetInt(szSection
	    	, "Enable"
	    	, &Get().sTimerParam[i].fEnable
	    	, Get().sTimerParam[i].fEnable
	    	, 0
	    	, 1
	    	, "test"
	    	, "test"
	    	, CUSTOM_LEVEL
	    );

		GetInt(szSection
	    	, "Valid"
	    	, &Get().sTimerParam[i].fValid
	    	, Get().sTimerParam[i].fValid
	    	, 0
	    	, 1
	    	, "test"
	    	, "test"
	    	, CUSTOM_LEVEL
	    );
		
	}

	SW_TRACE_DEBUG("Info: Timer Setting init -- ok\n");
	
	return S_OK;
}

/**
*@brief 初始化隐私区域
*@return 成功返回S_OK,其他值表示失败
*/
HRESULT CSWDomeCameraSetting::InitMask()
{
	for (DWORD i = 0; i < MAX_MASK; i++)
	{
		char szSection[256];
        swpa_sprintf(szSection, "\\DomeCamera\\Mask\\Area%d", i);

		GetString(szSection
	    	, "Name"
	    	, Get().sMaskParam[i].szName
	    	, Get().sMaskParam[i].szName
	    	, 31
	    	, "test"
	    	, "test"
	    	, CUSTOM_LEVEL
	    );

		GetInt(szSection
	    	, "Top"
	    	, &Get().sMaskParam[i].iTop
	    	, Get().sMaskParam[i].iTop
	    	, 0
	    	, 1080
	    	, "test"
	    	, "test"
	    	, CUSTOM_LEVEL
	    );

		GetInt(szSection
	    	, "Left"
	    	, &Get().sMaskParam[i].iLeft
	    	, Get().sMaskParam[i].iLeft
	    	, 0
	    	, 1920
	    	, "test"
	    	, "test"
	    	, CUSTOM_LEVEL
	    );

		GetInt(szSection
	    	, "Height"
	    	, &Get().sMaskParam[i].iHeight
	    	, Get().sMaskParam[i].iHeight
	    	, 0
	    	, 1080
	    	, "test"
	    	, "test"
	    	, CUSTOM_LEVEL
	    );

		GetInt(szSection
	    	, "Width"
	    	, &Get().sMaskParam[i].iWidth
	    	, Get().sMaskParam[i].iWidth
	    	, 0
	    	, 1920
	    	, "test"
	    	, "test"
	    	, CUSTOM_LEVEL
	    );

		
		GetInt(szSection
			, "MaskPan"
			, &Get().sMaskParam[i].iMaskPan
			, Get().sMaskParam[i].iMaskPan
			, 0
			, 4096
			, "test"
			, "test"
			, CUSTOM_LEVEL
		);

		GetInt(szSection
			, "MaskTilt"
			, &Get().sMaskParam[i].iMaskTilt
			, Get().sMaskParam[i].iMaskTilt
			, 0
			, 4096
			, "test"
			, "test"
			, CUSTOM_LEVEL
		);

		GetInt(szSection
			, "CameraZoom"
			, &Get().sMaskParam[i].iMaskZoom
			, Get().sMaskParam[i].iMaskZoom
			, 0
			, 4096
			, "test"
			, "test"
			, CUSTOM_LEVEL
		);

		GetInt(szSection
	    	, "Color"
	    	, &Get().sMaskParam[i].iColor
	    	, Get().sMaskParam[i].iColor
	    	, 0
	    	, 14
	    	, "test"
	    	, "test"
	    	, CUSTOM_LEVEL
	    );

		GetInt(szSection
	    	, "Enable"
	    	, &Get().sMaskParam[i].fEnable
	    	, Get().sMaskParam[i].fEnable
	    	, 0
	    	, 1
	    	, "test"
	    	, "test"
	    	, CUSTOM_LEVEL
	    );

		GetInt(szSection
	    	, "Valid"
	    	, &Get().sMaskParam[i].fValid
	    	, Get().sMaskParam[i].fValid
	    	, 0
	    	, 1
	    	, "test"
	    	, "test"
	    	, CUSTOM_LEVEL
	    );		

		GetInt(szSection
	    	, "RelatedPreset"
	    	, &Get().sMaskParam[i].iPresetID
	    	, Get().sMaskParam[i].iPresetID
	    	, 0
	    	, MAX_PRESET - 1
	    	, "test"
	    	, "test"
	    	, CUSTOM_LEVEL
	    );		
	}

	SW_TRACE_DEBUG("Info: Mask Setting init -- ok\n");
	
    return S_OK;
}

/**
*@brief 初始化画面冻结
*@return 成功返回S_OK,其他值表示失败
*/
HRESULT CSWDomeCameraSetting::InitPictureFreeze()
{
    GetInt("\\DomeCamera\\PictureFreeze"
		, "Enable"
		, &Get().sPictureFreezeParam.fEnable
		, Get().sPictureFreezeParam.fEnable
		, 0
		, 1 
		, "test"
		, "test"
		, CUSTOM_LEVEL
	);

	SW_TRACE_DEBUG("Info: PictureFreeze Setting init -- ok\n");

    return S_OK;
}

/**
*@brief 初始化待机
*@return 成功返回S_OK,其他值表示失败
*/
HRESULT CSWDomeCameraSetting::InitSleep()
{
	GetInt("\\DomeCamera\\Standby"
		, "Enable"
		, &Get().sStandbyParam.fEnable
		, Get().sStandbyParam.fEnable
		, 0
		, 1 
		, "test"
		, "test"
		, CUSTOM_LEVEL
	);

	Get().sStandbyParam.iWaitTime = 3000; //5min
	GetInt("\\DomeCamera\\Standby"
		, "WaitTime"
		, &Get().sStandbyParam.iWaitTime
		, Get().sStandbyParam.iWaitTime
		, 60
		, 100000000 //todo 2^31 
		, "test"
		, "test"
		, CUSTOM_LEVEL
	);

	SW_TRACE_DEBUG("Info: Standby Setting init -- ok\n");
	
	return S_OK;
}

/**
*@brief 恢复默认参数
*@return 成功返回S_OK,其他值表示失败
*/
HRESULT CSWDomeCameraSetting::ResetDefault()
{
	if( !m_strFilePath.IsEmpty() )
	{
		SW_TRACE_DEBUG("<dome>delete param file %s.", (const CHAR*)m_strFilePath);
		swpa_utils_file_delete((const CHAR*)m_strFilePath);
	}
	return S_OK;
}

