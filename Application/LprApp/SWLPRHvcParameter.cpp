#include "SWFC.h"
#include "SWLPRHvcParameter.h"

BOOL CheckArea(int nLeft, int nTop, int nRight, int nBottom)
{
    if (nLeft < 0 || nTop < 0 || nRight < 0 || nBottom < 0
        || nLeft > 100 || nTop > 100 || nRight > 100 || nBottom > 100
        || (nLeft >= nRight) || (nTop >= nBottom))
    {
        return FALSE;
    }
    return TRUE;
}

CSWLPRHvcParameter::CSWLPRHvcParameter()
{
    Get().nWorkModeIndex = PRM_HVC;
    m_strWorkMode.Format("抓拍识别");

    Get().cImgFrameParam.iCamNum = 1;
    strcpy(Get().cImgFrameParam.rgstrHVCParm[0], "[0,0,100,100],200,300,30");
    Get().cTrackerCfgParam.fltMinConfForOutput = 1.0f;
}

CSWLPRHvcParameter::~CSWLPRHvcParameter()
{
}

HRESULT CSWLPRHvcParameter::Initialize(CSWString strFilePath)
{
    // 先基类的初始化
    if (S_OK == CSWParameter<ModuleParams>::Initialize(strFilePath)
        && S_OK == InitSystem()
        && S_OK == InitTracker()
        && S_OK == InitHvDsp()
        && S_OK == InitCamApp()
        && S_OK == Init368()
        && S_OK == InitCharacter() )
    {
        return S_OK ;
    }

    SW_TRACE_DEBUG("<LPRParameter> Initialize failed.\n");
    return E_FAIL ;
}

HRESULT CSWLPRHvcParameter::Init368(VOID)
{
    if (S_OK == CSWLPRParameter::Init368())
    {
    	Get().cCamAppParam.iFlashDifferentLaneExt = 0;
        GetEnum("\\CamApp"
        	, "FlashDifferentLaneEx"
        	, &Get().cCamAppParam.iFlashDifferentLaneExt
        	, Get().cCamAppParam.iFlashDifferentLaneExt
        	, "0:不分车道闪;1:分车道闪"
        	, "闪光灯分车道闪(硬触发)"
        	, ""
        	, PROJECT_LEVEL
        );
        return S_OK ;
    }
    return E_FAIL ;
}

HRESULT CSWLPRHvcParameter::InitSystem(VOID)
{
    if (S_OK == CSWLPRParameter::InitSystem())
    {
        return S_OK ;
    }
    return E_FAIL ;
}

HRESULT CSWLPRHvcParameter::InitTracker(VOID)
{
    GetEnum("\\Tracker\\Misc"
        , "VoteCount"
        , &Get().cTrackerCfgParam.nVoteCount
        , Get().cTrackerCfgParam.nVoteCount
        , "0:不输出;1:输出"
        , "主视频最清晰大图输出"
        , ""
        , PROJECT_LEVEL
        );

    GetEnum("\\Tracker\\Misc"
        , "Alpha5"
        , &Get().cTrackerCfgParam.fAlpha5
        , Get().cTrackerCfgParam.fAlpha5
        , "0:关闭;1:开启"
        , "第五位字母识别"
        , ""
        , CUSTOM_LEVEL
        );

    GetEnum("\\Tracker\\Misc"
        , "Alpha6"
        , &Get().cTrackerCfgParam.fAlpha6
        , Get().cTrackerCfgParam.fAlpha6
        , "0:关闭;1:开启"
        , "第六位字母识别"
        , ""
        , CUSTOM_LEVEL
        );

    GetEnum("\\Tracker\\Misc"
        , "Alpha7"
        , &Get().cTrackerCfgParam.fAlpha7
        , Get().cTrackerCfgParam.fAlpha7
        , "0:关闭;1:开启"
        , "第七位字母识别"
        , ""
        , CUSTOM_LEVEL
        );

    GetInt("\\Tracker\\Misc"
        , "PlateEnhanceThreshold"
        , &Get().cTrackerCfgParam.nPlateEnhanceThreshold
        , Get().cTrackerCfgParam.nPlateEnhanceThreshold
        , 0
        , 255
        , "车牌增强阈值"
        , ""
        , PROJECT_LEVEL
        );

    GetFloat("\\Tracker\\Misc"
        , "MinConfForOutput"
        , &Get().cTrackerCfgParam.fltMinConfForOutput
        , Get().cTrackerCfgParam.fltMinConfForOutput
        , 0
        , 1
        , "可信度阈值"
        , ""
        , PROJECT_LEVEL
        );

    GetInt("\\Tracker\\Misc"
        , "FirstPlatePos"
        , &Get().cTrackerCfgParam.nFirstPlatePos
        , Get().cTrackerCfgParam.nFirstPlatePos
        , 0
        , 1
        , "优先出牌方位"
        , ""
        , PROJECT_LEVEL
        );

    GetInt("\\Tracker\\DoublePlate"
        , "MiddleWidth"
        , &Get().cTrackerCfgParam.nMiddleWidth
        , Get().cTrackerCfgParam.nMiddleWidth
        , 0
        , 300
        , "屏幕中间单层蓝牌后五字宽度"
        , ""
        , CUSTOM_LEVEL
        );

    GetInt("\\Tracker\\DoublePlate"
        , "BottomWidth"
        , &Get().cTrackerCfgParam.nBottomWidth
        , Get().cTrackerCfgParam.nBottomWidth
        , 0
        , 300
        , "屏幕底部单层蓝牌后五字宽度"
        , ""
        , CUSTOM_LEVEL
        );

    GetEnum("\\Tracker\\Recognition"
        , "EnableRecogCarColor"
        , &Get().cTrackerCfgParam.fEnableRecgCarColor
        , Get().cTrackerCfgParam.fEnableRecgCarColor
        , "0:关;1:开"
        , "车身颜色识别开关"
        , ""
        , CUSTOM_LEVEL
        );

    GetInt("\\Tracker\\Recognition"
        , "EnableT1Model"
        , &Get().cTrackerCfgParam.fEnableT1Model
        , Get().cTrackerCfgParam.fEnableT1Model
        , 0
        , 1
        , "T-1模型开关"
        , ""
        , PROJECT_LEVEL
        );

     GetInt("\\Tracker\\Misc"
        , "RecogGXPolice"
        , &Get().cTrackerCfgParam.nRecogGxPolice
        , Get().cTrackerCfgParam.nRecogGxPolice
        , 0
        , 1
        , "地方警牌识别开关"
        , ""
        , INNER_LEVEL
        );

    GetFloat("\\Tracker\\Misc"
        , "DetSetpAdj"
        , &Get().cTrackerCfgParam.fltPlateDetect_StepAdj
        , Get().cTrackerCfgParam.fltPlateDetect_StepAdj
        , 1.0
        , 5.0
        , "步长调整系数"
        , ""
        , PROJECT_LEVEL
        );

    GetInt("\\Tracker\\Misc"
        , "OnePlateMode"
        , &Get().cTrackerCfgParam.fOnePlateMode
        , Get().cTrackerCfgParam.fOnePlateMode
        , 0
        , 1
        , "检测单车牌"
        , ""
        , PROJECT_LEVEL
        );

    GetInt("\\Tracker\\Misc"
        , "SegHeight"
        , &Get().cTrackerCfgParam.nSegHeight
        , Get().cTrackerCfgParam.nSegHeight
        , 10
        , 100
        , "分割参考高度"
        , ""
        , PROJECT_LEVEL
        );

    GetInt("\\Tracker\\ProcessPlate"
        , "EnableDBGreenSegment"
        , &Get().cTrackerCfgParam.nPlateDetect_Green
        , Get().cTrackerCfgParam.nPlateDetect_Green
        , 0
        , 1
        , "绿牌识别开关"
        , ""
        , CUSTOM_LEVEL
        );

    GetInt("\\Tracker\\DoublePlate"
        , "DoublePlateEnable"
        , &Get().cTrackerCfgParam.fDoublePlateEnable
        , Get().cTrackerCfgParam.fDoublePlateEnable
        , 1
        , 3
        , "双层牌检测开关"
        , ""
        , PROJECT_LEVEL
        );

    GetInt("\\Tracker\\ProcessPlate"
        , "LightBlueFlag"
        , &Get().cTrackerCfgParam.nProcessPlate_LightBlue
        , Get().cTrackerCfgParam.nProcessPlate_LightBlue
        , 0
        , 1
        , "浅蓝牌开关"
        , ""
        , PROJECT_LEVEL
        );

    GetInt("\\Tracker\\ProcessPlate\\BlackPlate"
        , "Enable"
        , &Get().cTrackerCfgParam.fProcessPlate_BlackPlate_Enable
        , Get().cTrackerCfgParam.fProcessPlate_BlackPlate_Enable
        , 0
        , 1
        , "使能黑牌判断规则"
        , ""
        , CUSTOM_LEVEL
        );

    GetInt("\\Tracker\\ProcessPlate\\BlackPlate"
        , "H0"
        , &Get().cTrackerCfgParam.nProcessPlate_BlackPlateThreshold_H0
        , Get().cTrackerCfgParam.nProcessPlate_BlackPlateThreshold_H0
        , 0
        , 240
        , "蓝牌色度下限"
        , ""
        , CUSTOM_LEVEL
        );

    GetInt("\\Tracker\\ProcessPlate\\BlackPlate"
        , "H1"
        , &Get().cTrackerCfgParam.nProcessPlate_BlackPlateThreshold_H1
        , Get().cTrackerCfgParam.nProcessPlate_BlackPlateThreshold_H1
        , Get().cTrackerCfgParam.nProcessPlate_BlackPlateThreshold_H0
        , 240
        , "蓝牌色度上限"
        , ""
        , CUSTOM_LEVEL
        );

    GetInt("\\Tracker\\ProcessPlate\\BlackPlate"
        , "S"
        , &Get().cTrackerCfgParam.nProcessPlate_BlackPlate_S
        , Get().cTrackerCfgParam.nProcessPlate_BlackPlate_S
        , 0
        , 240
        , "黑牌饱和度上限"
        , ""
        , CUSTOM_LEVEL
        );

    GetInt("\\Tracker\\ProcessPlate\\BlackPlate"
        , "L"
        , &Get().cTrackerCfgParam.nProcessPlate_BlackPlate_L
        , Get().cTrackerCfgParam.nProcessPlate_BlackPlate_L
        , 0
        , 240
        , "黑牌亮度上限"
        , ""
        , CUSTOM_LEVEL
        );
    return S_OK ;
}

HRESULT CSWLPRHvcParameter::InitHvDsp(VOID)
{
    GetEnum("\\HvDsp\\VideoProc\\MainVideo"
        , "OutputBestSnap"
        , &Get().cResultSenderParam.iBestSnapshotOutput
        , Get().cResultSenderParam.iBestSnapshotOutput
        , "0:不输出;1:输出"
        , "主视频最清晰大图输出"
        , ""
        , CUSTOM_LEVEL
        );

    GetEnum("\\HvDsp\\Misc"
        , "LoopReplaceEnable"
        , &Get().cResultSenderParam.cProcRule.fLoopReplace
        , Get().cResultSenderParam.cProcRule.fLoopReplace
        , "0:不启用;1:启用"
        , "循环替换"
        , ""
        , CUSTOM_LEVEL
        );
/*
    GetEnum("\\HvDsp\\Misc"
        , "OutputAppendInfo"
        , &Get().cResultSenderParam.fOutputAppendInfo
        , Get().cResultSenderParam.fOutputAppendInfo
        , "0:关闭;1:打开"
        , "输出附加信息开关"
        , ""
        , PROJECT_LEVEL
        );

    GetEnum("\\HvDsp\\Misc"
        , "OutputObservedFrames"
        , &Get().cResultSenderParam.fOutputObservedFrames
        , Get().cResultSenderParam.fOutputObservedFrames
        , "0:不输出;1:输出"
        , "输出有效帧数开关"
        , ""
        , PROJECT_LEVEL
        );
*/
    GetEnum("HvDsp\\Misc"
        , "OutputFilterInfo"
        , &Get().cResultSenderParam.fOutputFilterInfo
        , Get().cResultSenderParam.fOutputFilterInfo
        , "0:不输出;1:输出"
        , "输出后处理信息"
        , ""
        , CUSTOM_LEVEL
        );

    // 参数传递.
    Get().cResultSenderParam.cProcRule.fOutPutFilterInfo = Get().cResultSenderParam.fOutputFilterInfo;

    GetString("\\HvDsp\\FilterRule"
        , "Compaty"
        , Get().cResultSenderParam.cProcRule.szCompatyRule
        , Get().cResultSenderParam.cProcRule.szCompatyRule
        , sizeof(Get().cResultSenderParam.cProcRule.szCompatyRule)
        , "通配符规则"
        , ""
        , CUSTOM_LEVEL
        );

    GetString("\\HvDsp\\FilterRule"
        , "Replace"
        , Get().cResultSenderParam.cProcRule.szReplaceRule
        , Get().cResultSenderParam.cProcRule.szReplaceRule
        , sizeof(Get().cResultSenderParam.cProcRule.szReplaceRule)
        , "替换规则"
        , ""
        , CUSTOM_LEVEL
        );

    GetString("\\HvDsp\\FilterRule"
        , "Leach"
        , Get().cResultSenderParam.cProcRule.szLeachRule
        , Get().cResultSenderParam.cProcRule.szLeachRule
        , sizeof(Get().cResultSenderParam.cProcRule.szLeachRule)
        , "过滤规则"
        , ""
        , CUSTOM_LEVEL
        );

    GetEnum("\\HvDsp\\Misc"
        , "AutoLinkEnable"
        , &Get().cResultSenderParam.cAutoLinkParam.fAutoLinkEnable
        , Get().cResultSenderParam.cAutoLinkParam.fAutoLinkEnable
        , "0:不使能;1:使能"
        , "主动连接使能"
        , ""
        , CUSTOM_LEVEL
        );

    GetString("\\HvDsp\\Misc"
        , "AutoLinkIP"
        , Get().cResultSenderParam.cAutoLinkParam.szAutoLinkIP
        , Get().cResultSenderParam.cAutoLinkParam.szAutoLinkIP
        , sizeof(Get().cResultSenderParam.cAutoLinkParam.szAutoLinkIP)
        , "主动连接服务器IP"
        , ""
        , CUSTOM_LEVEL
        );

    GetInt("\\HvDsp\\Misc"
        , "AutoLinkPort"
        , &Get().cResultSenderParam.cAutoLinkParam.iAutoLinkPort
        , Get().cResultSenderParam.cAutoLinkParam.iAutoLinkPort
        , 0
        , 10000
        , "主动连接服务器端口"
        , ""
        , CUSTOM_LEVEL
        );

    GetInt("\\HvDsp\\Trigger"
        , "TriggerOutEnable"
        , &Get().cResultSenderParam.fEnableTriggerOut
        , Get().cResultSenderParam.fEnableTriggerOut
        , 0
        , 1
        , "触发输出使能开关"
        , ""
        , CUSTOM_LEVEL
        );
    GetInt("\\HvDsp\\Trigger"
        , "TriggerOutNormalStatus"
        , &Get().cResultSenderParam.nTriggerOutNormalStatus
        , Get().cResultSenderParam.nTriggerOutNormalStatus
        , 0
        , 1
        , "触发输出常态控制，0:高电平，1:低电平"
        , ""
        , CUSTOM_LEVEL
        );
    GetInt("\\HvDsp\\Trigger"
        , "TriggerOutPlusWidth"
        , &Get().cResultSenderParam.nTriggerOutPlusWidth
        , Get().cResultSenderParam.nTriggerOutPlusWidth
        , 100
        , 30000
        , "触发输出通断时间，单位为ms"
        , ""
        , CUSTOM_LEVEL
        );

    GetString("\\HvDsp\\VideoProc\\HvcCamCfg"
        , "HVCParm_00"
        , Get().cImgFrameParam.rgstrHVCParm[0]
        , Get().cImgFrameParam.rgstrHVCParm[0]
        , sizeof(Get().cImgFrameParam.rgstrHVCParm[0])
        , "抓拍识别参数"
        , ""
        , CUSTOM_LEVEL
        );


    GetEnum("\\HvDsp\\Misc"
        	, "HDVideoEnable"
        	, &Get().cResultSenderParam.fSaveVideo
        	, Get().cResultSenderParam.fSaveVideo
        	, "0:不录像;1:录像"
        	, "硬盘录像开关"
        	, ""
        	, CUSTOM_LEVEL
        );


    	GetEnum("\\HvDsp\\Misc"
        	, "SafeSaver"
        	, &Get().cResultSenderParam.fIsSafeSaver
        	, Get().cResultSenderParam.fIsSafeSaver
        	, "0:不使能;1:使能"
        	, "安全存储使能"
        	, ""
        	, CUSTOM_LEVEL
        );

    	GetEnum("\\HvDsp\\Misc"
        	, "InitHdd"
        	, &Get().cResultSenderParam.fInitHdd
        	, Get().cResultSenderParam.fInitHdd
        	, "0:无;1:分区格式化"
        	, "初始化硬盘。注：一次性有效"
        	, ""
        	, CUSTOM_LEVEL
        );


        GetInt("\\HvDsp\\Misc"
        	, "OneFileSize"
        	, &Get().cResultSenderParam.iFileSize
        	, Get().cResultSenderParam.iFileSize
        	, 1
        	, 1024
        	, "定长存储单个文件大小,(单位K)"
        	, ""
        	, PROJECT_LEVEL
        );

        GetEnum("\\HvDsp\\Misc"
        	, "SaveSafeType"
        	, &Get().cResultSenderParam.iSaveSafeType
        	, Get().cResultSenderParam.iSaveSafeType
        	, "0:没有客户端连接时才存储;1:一直存储"
        	, "固态硬盘存储方式"
        	, ""
        	, CUSTOM_LEVEL
        );

        GetEnum("\\HvDsp\\Misc"
    	    , "DiskType"
    	    , &Get().cResultSenderParam.iDiskType
    	    , Get().cResultSenderParam.iDiskType
    	    , "0:网络硬盘;2:固态硬盘"
    	    , "硬盘类型"
    	    , ""
    	    , CUSTOM_LEVEL
        );

    	// zhaopy
    	// 如果是固态硬盘需要设置是否使能硬盘的标志。
    	if( 2 == Get().cResultSenderParam.iDiskType )
    	{
    		INT iUsed = (Get().cResultSenderParam.fIsSafeSaver || Get().cResultSenderParam.fSaveVideo) ? 1 : 0;
    		INT iSet = 0;
    		INT iRet = swpa_device_get_hdd_flag(&iSet);
    		SW_TRACE_NORMAL("<Param>get hdd flag:%d. return:%d.", iSet, iRet);
    		if( 0 == iRet && iSet != iUsed )
    		{
    			iRet = swpa_device_set_hdd_flag(iUsed);
    			SW_TRACE_NORMAL("<Param>set hdd flag:%d. return:%d.", iUsed, iRet);
    		}
    	}


    	GetString("\\HvDsp\\Misc"
        	, "NetDiskIP"
        	, Get().cResultSenderParam.szNetDiskIP
        	, Get().cResultSenderParam.szNetDiskIP
        	, sizeof(Get().cResultSenderParam.szNetDiskIP)
        	, "网络存储的IP地址"
        	, ""
        	, CUSTOM_LEVEL
        );

        GetInt("\\HvDsp\\Misc"
        	, "NetDiskSpace"
        	, &Get().cResultSenderParam.iNetDiskSpace
        	, Get().cResultSenderParam.iNetDiskSpace
        	, 1
        	, 3696
        	, "网络存储的容量,单位G (存储初始化时有意义)"
        	, ""
        	, CUSTOM_LEVEL
        );

        GetString("\\HvDsp\\Misc"
        	, "NFS"
        	, Get().cResultSenderParam.szNFS
        	, Get().cResultSenderParam.szNFS
        	, sizeof(Get().cResultSenderParam.szNFS)
        	, "NFS路径"
        	, ""
        	, CUSTOM_LEVEL
        );

        GetString("\\HvDsp\\Misc"
    	    , "NFSParam"
    	    , Get().cResultSenderParam.szNFSParam
    	    , Get().cResultSenderParam.szNFSParam
    	    , sizeof(Get().cResultSenderParam.szNFSParam)
    	    , "NFS挂载参数"
    	    , ""
    	    , PROJECT_LEVEL
        );


    	GetEnum("\\HvDsp\\Misc"
        	, "EnableNTP"
        	, &Get().cResultSenderParam.fEnableNtp
        	, Get().cResultSenderParam.fEnableNtp
        	, "0:不使能;1:使能"
        	, "NTP使能开关"
        	, ""
        	, CUSTOM_LEVEL
        );

    	GetInt("\\HvDsp\\Misc"
        	, "NTPSyncInterval"
        	, &Get().cResultSenderParam.iNtpSyncInterval
        	, Get().cResultSenderParam.iNtpSyncInterval
        	, 1
        	, 2147483647
        	, "NTP时间同步间隔(单位秒)"
        	, ""
        	, CUSTOM_LEVEL
        );

    	GetString("\\HvDsp\\Misc"
        	, "NTPServerIP"
        	, Get().cResultSenderParam.szNtpServerIP
        	, Get().cResultSenderParam.szNtpServerIP
        	, sizeof(Get().cResultSenderParam.szNtpServerIP)
        	, "NTP服务器IP地址"
        	, ""
        	, CUSTOM_LEVEL
        );


        GetEnum("\\HvDsp\\Misc"
            , "UseRushRule"
            , &Get().cResultSenderParam.iUseRushRule
            , Get().cResultSenderParam.iUseRushRule
            , "0:不使能;1:使能"
            , "逃费冲卡检测使能开关"
            , ""
            , CUSTOM_LEVEL
        );

        GetInt("\\HvDsp\\Misc"
            , "TimeIn"
            , &Get().cResultSenderParam.iTimeIn
            , Get().cResultSenderParam.iTimeIn
            , 100
            , 2147483647
            , "跟车冲卡判断阈值(单位毫秒)"
            , ""
            , CUSTOM_LEVEL
        );

        GetInt("\\HvDsp\\Misc"
            , "EnableBackupIO"
            , &Get().cResultSenderParam.iEnableBackupIO
            , Get().cResultSenderParam.iEnableBackupIO
            , 0
            , 1
            , "启用备用IO"
            , ""
            , PROJECT_LEVEL
        );

        GetInt("\\HvDsp\\Misc"
            , "BarrierNormalState"
            , &Get().cResultSenderParam.iBarrierStatus
            , Get().cResultSenderParam.iBarrierStatus
            , 0
            , 1
            , "栏杆机抬起电平"
            , ""
            , CUSTOM_LEVEL
        );


    int nLeft = 0;
    int nTop = 0;
    int nRight = 100;
    int nBottom = 100;
    int nMinPlateWidth = 200;
    int nMaxPlateWidth = 300;
    int nVariance = 30;
    char szArea[128] = {0};
    BOOL fChange = FALSE;
    sscanf(Get().cImgFrameParam.rgstrHVCParm[0],
        "[%d,%d,%d,%d],%d,%d,%d",
        &nLeft, &nTop, &nRight, &nBottom,
        &nMinPlateWidth, &nMaxPlateWidth,
        &nVariance
        );
    if ((nMinPlateWidth <= 0 || nMinPlateWidth > 650)
        || (nMaxPlateWidth <= 0 || nMaxPlateWidth > 650)
        || nMinPlateWidth >= nMaxPlateWidth)
    {
        nMinPlateWidth = 200;
        nMaxPlateWidth = 300;
        fChange = TRUE;
    }
    if (!CheckArea(nLeft, nTop, nRight, nBottom))
    {
        nLeft = nTop = 0;
        nRight = nBottom = 100;
        fChange = TRUE;
    }
    if (fChange)
    {
        sprintf(szArea,
            "[%d,%d,%d,%d],%d,%d,%d",
            nLeft, nTop, nRight, nBottom,
            nMinPlateWidth, nMaxPlateWidth,
            nVariance
            );
        memcpy(Get().cImgFrameParam.rgstrHVCParm[0], szArea, strlen(szArea));
    }
    return S_OK ;
}

HRESULT CSWLPRHvcParameter::InitOuterCtrl(VOID)
{
    return S_OK;
}

HRESULT CSWLPRHvcParameter::InitIPTCtrl(VOID)
{
    return S_OK;
}

HRESULT CSWLPRHvcParameter::InitCamApp(VOID)
{
    GetString("\\HvDsp\\Camera\\Ctrl"
        , "Addr"
        , Get().cCamCfgParam.szIP
        , Get().cCamCfgParam.szIP
        , (100 - 1)
        , "识别相机IP"
        , ""
        , CUSTOM_LEVEL
        );

    GetEnum("\\HvDsp\\Camera\\Ctrl"
        , "ProtocolType"
        , &Get().cCamCfgParam.iCamType
        , Get().cCamCfgParam.iCamType
        , "0:一体机协议;1:测试协议"
        , "协议类型"
        , ""
        , CUSTOM_LEVEL
        );

    /*
    GetEnum("\\HvDsp\\Camera\\Ctrl"
      	, "DynamicCfgEnable"
      	, &Get().cCamCfgParam.iDynamicCfgEnable
      	, Get().cCamCfgParam.iDynamicCfgEnable
        , "0:关闭;1:打开"
        , "动态设置参数"
        , ""
        , CUSTOM_LEVEL
        );
        */

    GetEnum("\\HvDsp\\Camera\\Ctrl"
    			, "CaptureAutoParamEnable"
    			, &Get().cCamCfgParam.iCaptureAutoParamEnable
    			, Get().cCamCfgParam.iCaptureAutoParamEnable
    			, "0:关闭;1:打开"
    			, "抓拍独立参数自动调整"
    			, "0:关闭;1:打开"
    			, CUSTOM_LEVEL
    			);

    Get().cCamCfgParam.iDynamicCfgEnable = 0;
    		GetInt("\\HvDsp\\Camera\\Ctrl"
    			, "CaptureDayShutterMax"
    			, &Get().cCamCfgParam.iCaptureDayShutterMax
    			, Get().cCamCfgParam.iCaptureDayShutterMax
    			, 400
    			, 3000
    			, "抓拍白天最大快门"
    			, ""
    			, CUSTOM_LEVEL
    			);
    		GetInt("\\HvDsp\\Camera\\Ctrl"
    			, "CaptureDayGainMax"
    			, &Get().cCamCfgParam.iCaptureDayGainMax
    			, Get().cCamCfgParam.iCaptureDayGainMax
    			, 100
    			, 280
    			, "抓拍白天最大增益"
    			, ""
    			, CUSTOM_LEVEL
    			);
    		GetInt("\\HvDsp\\Camera\\Ctrl"
    			, "CaptureNightShutterMax"
    			, &Get().cCamCfgParam.iCaptureNightShutterMax
    			, Get().cCamCfgParam.iCaptureNightShutterMax
    			, 400
    			, 3000
    			, "抓拍晚上最大快门"
    			, ""
    			, CUSTOM_LEVEL
    			);
    		GetInt("\\HvDsp\\Camera\\Ctrl"
    			, "CaptureNightGainMax"
    			, &Get().cCamCfgParam.iCaptureNightGainMax
    			, Get().cCamCfgParam.iCaptureNightGainMax
    			, 100
    			, 280
    			, "抓拍晚上最大增益"
    			, ""
    			, CUSTOM_LEVEL
    			);

    return S_OK ;
}
