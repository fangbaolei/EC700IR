/*
 * CSWLPRJupiterDomeCameraParameter.cpp
 *
 *  Created on: 2013年12月27日
 *      Author: qinjj
 */
#include "SWFC.h"
#include "SWLPRApplication.h"
#include "SWLPRJupiterDomeCameraParameter.h"



CSWLPRJupiterDomeCameraParameter::CSWLPRJupiterDomeCameraParameter()
{
	ResetParam();
}

CSWLPRJupiterDomeCameraParameter::~CSWLPRJupiterDomeCameraParameter()
{
}

void CSWLPRJupiterDomeCameraParameter::ResetParam(VOID)
{
	//先调用基类
	CSWLPRParameter::ResetParam();

	/*根据具体方案配置参数*/
	
	Get().nWorkModeIndex = PRM_DOME_CAMERA;
	m_strWorkMode.Format("违停抓拍");
	Get().nCameraWorkMode = 0;		//todo: 这里应该是多少?
	Get().nMCFWUsecase = DOME_CAMERA;

 
	//先初始化扫描区域，每种模式都有自己的扫描区域
	Get().cTrackerCfgParam.cDetectArea.DetectorAreaLeft = 20;
	Get().cTrackerCfgParam.cDetectArea.DetectorAreaTop = 20;
	Get().cTrackerCfgParam.cDetectArea.DetectorAreaRight = 80;
	Get().cTrackerCfgParam.cDetectArea.DetectorAreaBottom = 90;
	Get().cTrackerCfgParam.cDetectArea.nDetectorMinScaleNum = 11;
	Get().cTrackerCfgParam.cDetectArea.nDetectorMaxScaleNum = 18;

	Get().cTrackerCfgParam.cVideoDet.nVideoDetMode = 2;

	//初始化梯形区域
	Get().cTrackerCfgParam.cTrapArea.TopLeftX = Get().cTrackerCfgParam.cDetectArea.DetectorAreaLeft;
	Get().cTrackerCfgParam.cTrapArea.TopRightX = Get().cTrackerCfgParam.cDetectArea.DetectorAreaRight;
	Get().cTrackerCfgParam.cTrapArea.BottomLeftX = Get().cTrackerCfgParam.cDetectArea.DetectorAreaLeft;
	Get().cTrackerCfgParam.cTrapArea.BottomRightX = Get().cTrackerCfgParam.cDetectArea.DetectorAreaRight;

	Get().cTrackerCfgParam.cTrapArea.TopLeftY = Get().cTrackerCfgParam.cDetectArea.DetectorAreaTop;
	Get().cTrackerCfgParam.cTrapArea.TopRightY = Get().cTrackerCfgParam.cDetectArea.DetectorAreaTop;
	Get().cTrackerCfgParam.cTrapArea.BottomLeftY = Get().cTrackerCfgParam.cDetectArea.DetectorAreaBottom;
	Get().cTrackerCfgParam.cTrapArea.BottomRightY = Get().cTrackerCfgParam.cDetectArea.DetectorAreaBottom;

	Get().cTrackerCfgParam.cTrapArea.nDetectorMinScaleNum = Get().cTrackerCfgParam.cDetectArea.nDetectorMinScaleNum;
	Get().cTrackerCfgParam.cTrapArea.nDetectorMaxScaleNum = Get().cTrackerCfgParam.cDetectArea.nDetectorMaxScaleNum;

	Get().cCamCfgParam.iMaxExposureTime = 3800;
	Get().cCamCfgParam.iMinExposureTime = 0;
	Get().cCamCfgParam.iMinAGCLimit = 50;
	Get().cCamCfgParam.iMaxAGCLimit = 140;
	Get().cCamAppParam.iAGCGainHOri = 100;
	Get().cCamCfgParam.iNightShutterThreshold = 3000;
	Get().cCamCfgParam.iNightAvgYThreshold = 100;
	
	//卡口默认的视频测速修正系数
	Get().cTrackerCfgParam.cScaleSpeed.fltAdjustCoef = 0.96;

	Get().cDevParam[1].iCommType = 1;	//金星只有一个串口COM1,只支持485

	Get().cCamAppParam.iResolution = 0;	//默认为1080P
	
	// 抓拍位置初始值
	Get().cTrackerCfgParam.nCaptureOnePos = 50;
	Get().cTrackerCfgParam.nCaptureTwoPos = 80;

	// 金星无频闪补光灯
	Get().cTrackerCfgParam.iUsedLight = 0;

	//金星扫描区域默认打开
	Get().cTrackerCfgParam.cTrapArea.fEnableDetAreaCtrl = TRUE;

	//金星RGB分量配置范围为36~255,默认值不能为0
	Get().cCamAppParam.iGainR = 36;
	Get().cCamAppParam.iGainG = 36;
	Get().cCamAppParam.iGainB = 36;

	// WDR
	Get().cCamAppParam.iWDRValue = 2;

		//初始化停车检测相关参数
	swpa_memset(&Get().cMatchParam.cPeccancyParkingParam, 0, 
				sizeof(PECCANCY_PARKING_PARAM));

}


HRESULT CSWLPRJupiterDomeCameraParameter::Initialize(CSWString strFilePath)
{
	// 初始化前重新设置默认值
	ResetParam();
	
    // 先基类的初始化
    if (S_OK == CSWParameter<ModuleParams>::Initialize(strFilePath)
		&& S_OK == InitSystem()
		&& S_OK == InitTracker()
		&& S_OK == InitHvDsp()
        && S_OK == InitOuterCtrl()
        && S_OK == InitIPTCtrl()
        && S_OK == InitCamApp()
        && S_OK == InitCamera()
        && S_OK == InitCharacter()
        && S_OK == InitGB28181()
//        && S_OK == InitAutoReboot()
        && S_OK == InitONVIF())
    {
        return S_OK ;
    }

    SW_TRACE_DEBUG("<CSWLPRJupiterDomeCameraParameter> Initialize failed.\n");
    return E_FAIL ;
}

HRESULT CSWLPRJupiterDomeCameraParameter::InitSystem(VOID)
{
    if (S_OK == CSWLPRParameter::InitSystem())
    {
		
        return S_OK ;
    }
    return E_FAIL ;

}

HRESULT CSWLPRJupiterDomeCameraParameter::InitTracker(VOID)
{


	return S_OK;
}

HRESULT CSWLPRJupiterDomeCameraParameter::InitHvDsp(VOID)
{

	GetEnum("HvDsp\\Misc"
    	, "IPNCLogOutput"
    	, &Get().cCamCfgParam.iIPNCLogOutput
    	, Get().cCamCfgParam.iIPNCLogOutput
    	, "0:不输出;1:输出"
    	, "输出IPNC日志信息"
    	, ""
    	, PROJECT_LEVEL
    );
	
   /* GetEnum("HvDsp\\Misc"
            , "OutputFilterInfo"
            , &Get().cResultSenderParam.fOutputFilterInfo
            , Get().cResultSenderParam.fOutputFilterInfo
            , "0:不输出;1:输出"
            , "输出后处理信息"
            , ""
            , CUSTOM_LEVEL
           );
           */

    // 参数传递.
    Get().cResultSenderParam.cProcRule.fOutPutFilterInfo = Get().cResultSenderParam.fOutputFilterInfo;

    GetEnum("\\HvDsp\\Misc"
            , "DrawRect"
            , &Get().cResultSenderParam.iDrawRect
            , Get().cResultSenderParam.iDrawRect
            , "0:不发送;1:发送"
            , "发送图像画红框"
            , ""
            , PROJECT_LEVEL
           );

	Get().cResultSenderParam.fSaveVideo = FALSE;

	Get().cResultSenderParam.fIsSafeSaver = FALSE;

	Get().cResultSenderParam.fInitHdd = FALSE;
	Get().cResultSenderParam.iFileSize = 512;
	Get().cResultSenderParam.iSaveSafeType = 0;
	Get().cResultSenderParam.iDiskType = 0;

	/*
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
	*/

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
            , 300
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
		, "TimeZone"
		, &Get().cResultSenderParam.iTimeZone
		, Get().cResultSenderParam.iTimeZone
		, "0:GMT-12;"
		  	"1:GMT-11;"
			"2:GMT-10;"
			"3:GMT-09;"
			"4:GMT-08;"
			"5:GMT-07;"
			"6:GMT-06;"
			"7:GMT-05;"
			"8:GMT-04;"
			"9:GMT-03;"
			"10:GMT-02;"
			"11:GMT-01;"
			"12:GMT+00;"
			"13:GMT+01;"
			"14:GMT+02;"
			"15:GMT+03;"
			"16:GMT+04;"
			"17:GMT+05;"
			"18:GMT+06;"
			"19:GMT+07;"
			"20:GMT+08;"
			"21:GMT+09;"
			"22:GMT+10;"
			"23:GMT+11;"
			"24:GMT+12"
		, "时区"
		, ""
		, CUSTOM_LEVEL
	);

    GetString("\\HvDsp\\FilterRule"
            , "Compaty"
            , Get().cResultSenderParam.cProcRule.szCompatyRule
            , Get().cResultSenderParam.cProcRule.szCompatyRule
            , sizeof(Get().cResultSenderParam.cProcRule.szCompatyRule)
            , "通配符规则"
            , ""
            , PROJECT_LEVEL
            );

    GetString("\\HvDsp\\FilterRule"
            , "Replace"
            , Get().cResultSenderParam.cProcRule.szReplaceRule
            , Get().cResultSenderParam.cProcRule.szReplaceRule
            , sizeof(Get().cResultSenderParam.cProcRule.szReplaceRule)
            , "替换规则"
            , ""
            , PROJECT_LEVEL
            );

    GetString("\\HvDsp\\FilterRule"
            , "Leach"
            , Get().cResultSenderParam.cProcRule.szLeachRule
            , Get().cResultSenderParam.cProcRule.szLeachRule
            , sizeof(Get().cResultSenderParam.cProcRule.szLeachRule)
            , "过滤规则"
            , ""
            , PROJECT_LEVEL
            );

    GetEnum("\\HvDsp\\Misc"
            , "LoopReplaceEnable"
            , &Get().cResultSenderParam.cProcRule.fLoopReplace
            , Get().cResultSenderParam.cProcRule.fLoopReplace
            , "0:不启用;1:启用"
            , "循环替换"
            , ""
            , PROJECT_LEVEL
           );

    GetString("\\HvDsp\\Identify", "StreetName"
            , Get().cResultSenderParam.szStreetName
            , Get().cResultSenderParam.szStreetName
            , sizeof(Get().cResultSenderParam.szStreetName)
            , "路口名称"
            , ""
            , CUSTOM_LEVEL
            );

	/*
    GetString("\\HvDsp\\Identify"
            , "StreetDirection"
            , Get().cResultSenderParam.szStreetDirection
            , Get().cResultSenderParam.szStreetDirection
            , sizeof(Get().cResultSenderParam.szStreetDirection)
            , "路口方向"
            , ""
            , CUSTOM_LEVEL
            );


    GetEnum("\\HvDsp\\Misc"
            , "OutputOnlyPeccancy"
            , &Get().cResultSenderParam.iOutputOnlyPeccancy
            , Get().cResultSenderParam.iOutputOnlyPeccancy
            , "0:全部输出;1:只输出违章结果;2:只输出非违章结果"
            , "输出结果方式"
            , ""
            , CUSTOM_LEVEL
           );

    GetInt("\\HvDsp\\Misc"
            , "SendRecordSpace"
            , &Get().cResultSenderParam.iSendRecordSpace
            , Get().cResultSenderParam.iSendRecordSpace
            , 2
            , 100
            , "结果发送间隔(*100ms)"
            , ""
            , CUSTOM_LEVEL
          );

    GetInt("\\HvDsp\\Misc"
            , "SendHisVideoSpace"
            , &Get().cResultSenderParam.iSendHisVideoSpace
            , Get().cResultSenderParam.iSendHisVideoSpace
            , 2
            , 20
            , "历史录像发送间隔(*100ms)"
            , ""
            , CUSTOM_LEVEL
          );


    GetEnum("\\HvDsp\\VideoProc\\MainVideo"
            , "OutputBestSnap"
            , &Get().cResultSenderParam.iBestSnapshotOutput
            , Get().cResultSenderParam.iBestSnapshotOutput
            , "0:不输出;1:输出"
            , "主视频最清晰大图输出"
            , ""
            , CUSTOM_LEVEL
           );

    GetEnum("\\HvDsp\\VideoProc\\MainVideo"
            , "OutputLastSnap"
            , &Get().cResultSenderParam.iLastSnapshotOutput
            , Get().cResultSenderParam.iLastSnapshotOutput
            , "0:不输出;1:输出"
            , "主视频最后大图输出"
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
*/

	GetEnum("\\HvDsp\\Misc"
		, "FtpEnable"
		, &Get().cResultSenderParam.cFtpParam.fFtpEnable
		, Get().cResultSenderParam.cFtpParam.fFtpEnable
		, "0:不使能;1:使能"
		, "FTP使能"
		, ""
		, CUSTOM_LEVEL
	);
	
	GetString("\\HvDsp\\Misc"
			, "FtpIP"
			, Get().cResultSenderParam.cFtpParam.szFtpIP
			, Get().cResultSenderParam.cFtpParam.szFtpIP
			, sizeof(Get().cResultSenderParam.cFtpParam.szFtpIP)
			, "FTP服务器IP"
			, ""
			, CUSTOM_LEVEL
	);
	GetString("\\HvDsp\\Misc"
			, "FtpUserName"
			, Get().cResultSenderParam.cFtpParam.szUserName
			, Get().cResultSenderParam.cFtpParam.szUserName
			, sizeof(Get().cResultSenderParam.cFtpParam.szUserName)
			, "FTP用户名"
			, ""
			, CUSTOM_LEVEL
	);
	GetString("\\HvDsp\\Misc"
			, "FtpPassWd"
			, Get().cResultSenderParam.cFtpParam.szPassWord
			, Get().cResultSenderParam.cFtpParam.szPassWord
			, sizeof(Get().cResultSenderParam.cFtpParam.szPassWord)
			, "FTP密码"
			, ""
			, CUSTOM_LEVEL
	);
	
	GetInt("HvDsp\\Misc"
		, "FTPTimeOut"
		, &Get().cResultSenderParam.cFtpParam.iTimeOut
		, 5
		, 0
		, 10
		, "FTP连接超时时间(单位秒)"
		, ""
		, CUSTOM_LEVEL
	);

	/* //球机不支持
    GetInt("HvDsp\\EventChecker"
            , "CheckEventTime"
            , &Get().cResultSenderParam.iCheckEventTime
            , Get().cResultSenderParam.iCheckEventTime
            , 0
            , 5
            , "事件检测周期(单位:分钟)"
            , ""
            , PROJECT_LEVEL
          );

    Get().cTrackerCfgParam.nDetReverseRunEnable = FALSE;
	Get().cTrackerCfgParam.nSpan = FALSE;
	

    Get().cTrackerCfgParam.fltOverLineSensitivity = 2.5;

	Get().cResultSenderParam.iSpeedLimit = 1000;
    Get().cTrackerCfgParam.iSpeedLimit = Get().cResultSenderParam.iSpeedLimit;

    GetEnum("\\HvDsp\\Misc"
            , "OutputCarSize"
            , &Get().cTrackerCfgParam.fOutputCarSize
            , Get().cTrackerCfgParam.fOutputCarSize
            , "0:不输出;1:输出"
            , "输出车辆尺寸"
            , ""
            , PROJECT_LEVEL
           );
		   */
    return S_OK;

}

HRESULT CSWLPRJupiterDomeCameraParameter::InitCamApp(VOID)
{

    GetString("\\HvDsp\\Camera\\Ctrl"
            , "Addr"
            , Get().cCamCfgParam.szIP
            , Get().cCamCfgParam.szIP
            , (100-1)
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
            , PROJECT_LEVEL
           );

    GetInt("\\HvDsp\\Camera\\Ctrl"
            , "AddrPort"
            , &Get().cCamCfgParam.iCamPort
            , Get().cCamCfgParam.iCamPort
            , 1024
            , 99999
            , "测试协议端口号"
            , "端口号"
            , PROJECT_LEVEL
          );

    Get().cCamCfgParam.iDynamicCfgEnable = 1;
    GetEnum("\\HvDsp\\Camera\\Ctrl"
            , "DynamicCfgEnable"
            , &Get().cCamCfgParam.iDynamicCfgEnable
            , Get().cCamCfgParam.iDynamicCfgEnable
            , "0:关闭;1:打开"
            , "动态设置参数"
            , ""
            , CUSTOM_LEVEL
           );

	Get().cCamCfgParam.iDynamicTriggerEnable = 0;
	Get().cCamCfgParam.iTriggerDelay = 0;
	Get().cCamCfgParam.iNightShutterThreshold = 0;

    GetInt("\\HvDsp\\Camera\\Ctrl"
            , "NightShutterThreshold"
            , &Get().cCamCfgParam.iNightShutterThreshold
            , Get().cCamCfgParam.iNightShutterThreshold
            , 0
            , 30000
            , "夜晚判断快门阈值"
            , ""
            , PROJECT_LEVEL
          );

    GetInt("\\HvDsp\\Camera\\Ctrl"
            , "NightAvgYThreshold"
            , &Get().cCamCfgParam.iNightAvgYThreshold
            , Get().cCamCfgParam.iNightAvgYThreshold
            , 0
            , 255
            , "夜晚判断环境亮度阈值"
            , ""
            , PROJECT_LEVEL
          );

    Get().cCamCfgParam.iEnableAGC = 1;

/*
    Get().cCamCfgParam.iAutoParamEnable = 1;
    if (Get().cCamCfgParam.iAutoParamEnable == 1)
    {
        UpdateEnum("\\HvDsp\\Camera\\Ctrl"
                , "AutoParamEnable"
                , 0
                );
    }

    char szText[255] = {0};
    char szChnText[255] = {0};
    const char *szLightType[] =
    {
        "BigSuitlight",
        "BigSuitlight2",
        "Suitlight",
        "Suitlight2",
        "Day",
        "Day2",
        "Backlight",
        "Backlight2",
        "BigBacklight",
        "BigBacklight2",
        "Nightfall",
        "Nightfall2",
        "Night",
        "Night2"
    };

    for (int i = 0; i < MAX_LEVEL_COUNT; i++)
    {
        sprintf(szText, "\\HvDsp\\Camera\\%s", szLightType[i]);
        sprintf(szChnText, "%d级曝光时间", i);
        GetInt(szText
                , "ExposureTime"
                , &Get().cCamCfgParam.irgExposureTime[i]
                , Get().cCamCfgParam.irgExposureTime[i]
                , -1
                , 30000
                , szChnText
                , ""
                , PROJECT_LEVEL
              );

        sprintf(szChnText, "%d级增益", i);
        GetInt(szText
                , "Plus"
                , &Get().cCamCfgParam.irgGain[i]
                , Get().cCamCfgParam.irgGain[i]
                , -1
                , 360
                , szChnText
                , ""
                , PROJECT_LEVEL
              );

        sprintf(szChnText, "%d级AGC增益门限", i);
        GetInt(szText
                , "AGCLimit"
                , &Get().cCamCfgParam.irgAGCLimit[i]
                , Get().cCamCfgParam.irgAGCLimit[i]
                , -1
                , 255
                , szChnText
                , ""
                , CUSTOM_LEVEL
              );

    }

    if (Get().cCamCfgParam.iAutoParamEnable == 1)
    {
        InitCamCfgParam(&Get().cCamCfgParam, szLightType);
    }
  */

    return S_OK;
}

HRESULT CSWLPRJupiterDomeCameraParameter::InitIPTCtrl(VOID)
{
	CSWString strCOMM;
    CSWString strDomeRocker;
	  

  //////////////////////////////////////////
  strDomeRocker.Format("\\Device[外部设备]\\DomeRocker");
  Get().cDomeRockerParam.fRockerEnable = TRUE;
  Get().cDomeRockerParam.iBaudrate = 115200;
  Get().cDomeRockerParam.iProtocol = 0;
  /*
  GetEnum((LPCSTR)strDomeRocker
            , "EnableRocker"
            , &Get().cDomeRockerParam.fRockerEnable
            , Get().cDomeRockerParam.fRockerEnable
            , "0:不使能;1:使能"
            , "外接摇杆使能"
            , ""
            , CUSTOM_LEVEL
           );



  GetEnum((LPCSTR)strDomeRocker
			, "Baudrate"
			, &Get().cDomeRockerParam.iBaudrate
			, Get().cDomeRockerParam.iBaudrate
			, "300:300"
			  ";600:600"
			  ";1200:1200"
			  ";2400:2400"
			  ";4800:4800"
			  ";9600:9600"
			  ";19200:19200"
			  ";38400:38400"
			  ";43000:43000"
			  ";56000:56000"
			  ";57600:57600"
			  ";115200:115200"
			, "波特率"
			, ""
			, PROJECT_LEVEL
		);

  GetEnum((LPCSTR)strDomeRocker
            , "RockerProtocol"
            , &Get().cDomeRockerParam.iProtocol
            , Get().cDomeRockerParam.iProtocol
            , "0:PELCO-D;"  // todo  1:PELCO-P
            , "摇杆协议"
            , "目前只支持PELCO-D协议"
            , PROJECT_LEVEL
           );
  */

  
  return S_OK;
}

HRESULT CSWLPRJupiterDomeCameraParameter::InitOuterCtrl(VOID)
{
	Get().cMatchParam.cPeccancyParkingParam.iNightUseLedLight = 1;
	GetEnum("\\OuterCtrl[外总控]\\PeccancyParking"
    	, "NightUseLedLight"
    	, &Get().cMatchParam.cPeccancyParkingParam.iNightUseLedLight
    	, Get().cMatchParam.cPeccancyParkingParam.iNightUseLedLight
    	, "0:不启用;1:启用"
    	, "夜晚识别使用LED灯"
    	, ""
    	, CUSTOM_LEVEL
    );

	CHAR szConfidenceFilter[64] = "0.0";
	GetString("\\OuterCtrl[外总控]\\PeccancyParking"
			  , "ConfidenceFilter"
			  , szConfidenceFilter
			  , "1.0"
			  , 64
			  , "可信度过滤器"
			  , "可信度过滤器，可信度低于此值过滤"
			  , CUSTOM_LEVEL);
     	swpa_sscanf(szConfidenceFilter, "%f",
					&Get().cMatchParam.cPeccancyParkingParam.fltConfidenceFilter);

		

		//初始化预置位信息
	for(INT i = 0; i < DETECT_AREA_COUNT_MAX; i++)
	{
		//TODO 目前只有四个点
		CHAR szDetectArea1[64] = "[900,500];[1100,500];[1100,600];[900,600]";
		CSWString strDetAreaNode;
		CSWString strDetAreaName;
		strDetAreaNode.Format("\\OuterCtrl[外总控]\\PeccancyParking\\DetectArea%d", i + 1);
		strDetAreaName.Format("DetectArea");


		Get().cMatchParam.cPeccancyParkingParam.cPeccancyParkingArea[i].iPresetNum = -1;
		GetInt((LPCSTR)strDetAreaNode
			   , "PresetNumber"
			   , &Get().cMatchParam.cPeccancyParkingParam.cPeccancyParkingArea[i].iPresetNum
			   , Get().cMatchParam.cPeccancyParkingParam.cPeccancyParkingArea[i].iPresetNum
			   , -1
			   , 256
			   , "从属预置位编号"
			   , ""
			   , CUSTOM_LEVEL
				);
					
		Get().cMatchParam.cPeccancyParkingParam.cPeccancyParkingArea[i].iDetAreaNum = i + 1;
		CHAR szPeccancyTime[64] = "00:00";
		GetString((LPCSTR)strDetAreaNode
				  , "BeginTime"
				  , szPeccancyTime
				  , "00:00"
				  , 64
				  , "违章开始时间"
				  , "违章开始时间，时:分"
				  , CUSTOM_LEVEL);
     	swpa_sscanf(szPeccancyTime, "%d:%d",
					&Get().cMatchParam.cPeccancyParkingParam.cPeccancyParkingArea[i].iBeginTime[0],
					&Get().cMatchParam.cPeccancyParkingParam.cPeccancyParkingArea[i].iBeginTime[1]);
							
								
		swpa_strcpy(szPeccancyTime, "23:59");
		GetString((LPCSTR)strDetAreaNode
				  , "EndTime"
				  , szPeccancyTime
				  , "23:59"
				  , 64
				  , "违章结束时间"
				  , "违章结束时间，时:分"
				  , CUSTOM_LEVEL);
		swpa_sscanf(szPeccancyTime, "%d:%d",
					&Get().cMatchParam.cPeccancyParkingParam.cPeccancyParkingArea[i].iEndTime[0],
					&Get().cMatchParam.cPeccancyParkingParam.cPeccancyParkingArea[i].iEndTime[1]);
									
		Get().cMatchParam.cPeccancyParkingParam.cPeccancyParkingArea[i].iDetAreaPosCount = 4;

		GetString((LPCSTR)strDetAreaNode
				  , (LPCSTR)strDetAreaName
			   , szDetectArea1
			   , szDetectArea1
			   , 64
			   , "识别检测区域"
			   , "识别检测区域(单位:像素)"
			   , CUSTOM_LEVEL);
		swpa_sscanf(szDetectArea1, "[%d,%d];[%d,%d];[%d,%d];[%d,%d]",
					&Get().cMatchParam.cPeccancyParkingParam.cPeccancyParkingArea[i].rgiPosX[0],
					&Get().cMatchParam.cPeccancyParkingParam.cPeccancyParkingArea[i].rgiPosY[0],
					&Get().cMatchParam.cPeccancyParkingParam.cPeccancyParkingArea[i].rgiPosX[1],
					&Get().cMatchParam.cPeccancyParkingParam.cPeccancyParkingArea[i].rgiPosY[1],
					&Get().cMatchParam.cPeccancyParkingParam.cPeccancyParkingArea[i].rgiPosX[2],
					&Get().cMatchParam.cPeccancyParkingParam.cPeccancyParkingArea[i].rgiPosY[2],
					&Get().cMatchParam.cPeccancyParkingParam.cPeccancyParkingArea[i].rgiPosX[3],
					&Get().cMatchParam.cPeccancyParkingParam.cPeccancyParkingArea[i].rgiPosY[3]);

		
		Get().cMatchParam.cPeccancyParkingParam.cPeccancyParkingArea[i].iLimitTime = 900;
		GetInt((LPCSTR)strDetAreaNode
			   , "LimitTime"
			   , &Get().cMatchParam.cPeccancyParkingParam.cPeccancyParkingArea[i].iLimitTime
			   , Get().cMatchParam.cPeccancyParkingParam.cPeccancyParkingArea[i].iLimitTime
			   , 0
			   , 3600
			   , "违章时间阈值"
			   , "停车多长时间达到违章，单位:秒"
			   , CUSTOM_LEVEL);
	}


    Get().cMatchParam.fCaptureRecong = FALSE;



return S_OK;
}

HRESULT CSWLPRJupiterDomeCameraParameter::InitCamera(VOID)
{
	Get().cCamAppParam.iJpegCompressRate = 75;
/*
    GetInt("\\CamApp"
    	, "JpegCompressRate"
    	, &Get().cCamAppParam.iJpegCompressRate
    	, Get().cCamAppParam.iJpegCompressRate
    	, 1
    	, 99
    	, "视频流Jpeg压缩品质"
    	, ""
    	, PROJECT_LEVEL
    );
*/


    GetInt("\\CamApp"
    	, "IFrameInterval"
    	, &Get().cCamAppParam.iIFrameInterval
    	, Get().cCamAppParam.iIFrameInterval
    	, 2			//金星要求最小I帧间隔为2
    	, 25
    	, "H.264流I帧间隔"
    	, ""
    	, CUSTOM_LEVEL
    );

    if( Get().cGb28181.fEnalbe )
    {
    	Get().cCamAppParam.iIFrameInterval = 4;
        UpdateInt("\\CamApp"
            , "IFrameInterval"
            , Get().cCamAppParam.iIFrameInterval
            );
    }

	GetEnum("\\CamApp"
    	, "Resolution"
    	, &Get().cCamAppParam.iResolution
    	, Get().cCamAppParam.iResolution
    	, "0:1080P;1:720P"
    	, "H.264图像分辨率"
    	, ""
    	, CUSTOM_LEVEL
    );
    
	Get().cCamAppParam.iTargetBitRate = 2 * 1024;
    GetInt("\\CamApp"
    	, "TargetBitRate"
    	, &Get().cCamAppParam.iTargetBitRate
    	, Get().cCamAppParam.iTargetBitRate
    	, 512
    	, 16*1024
    	, "H.264流输出比特率"
    	, "单位：Kbps"
    	, CUSTOM_LEVEL
		);

	Get().cCamAppParam.iMaxBitRate = 2 * Get().cCamAppParam.iTargetBitRate;
    GetInt("\\CamApp"
    	, "MaxBitRate"
    	, &Get().cCamAppParam.iMaxBitRate
    	, Get().cCamAppParam.iMaxBitRate
    	, 512
    	, 16*1024
    	, "H.264流输出最大比特率"
    	, "单位：Kbps"
    	, CUSTOM_LEVEL
		);

	GetEnum("\\CamApp"
			, "RateControl"
			, &Get().cCamAppParam.iRateControl
			, Get().cCamAppParam.iRateControl
			, "0:VBR;1:CBR"
			, "H.264码率类型"
			, "0:VBR 可变码率;1:CBR 恒定码率"
			, CUSTOM_LEVEL);
	
	//第二路H264参数
	GetInt("\\CamApp"
		, "IFrameIntervalSecond"
		, &Get().cCamAppParam.iIFrameIntervalSecond
		, Get().cCamAppParam.iIFrameIntervalSecond
		, 2
		, 25
		, "第二路H.264流I帧间隔"
		, ""
		, CUSTOM_LEVEL
	);

	GetEnum("\\CamApp"
		, "ResolutionSecond"
		, &Get().cCamAppParam.iResolutionSecond
		, Get().cCamAppParam.iResolutionSecond
		, "0:1080P;1:720P;3:576P;4:480P"
		, "第二路H.264图像分辨率"
		, ""
		, CUSTOM_LEVEL
	);

	Get().cCamAppParam.iTargetBitRateSecond = 2 * 1024;
	GetInt("\\CamApp"
		, "TargetBitRateSecond"
		, &Get().cCamAppParam.iTargetBitRateSecond
		, Get().cCamAppParam.iTargetBitRateSecond
		, 512
		, 16*1024
		, "第二路H.264流输出比特率"
		, "单位：Kbps（千比特每秒）"
		, CUSTOM_LEVEL
	);

	GetInt("\\CamApp"
		, "MaxBitRateSecond"
		, &Get().cCamAppParam.iMaxBitRateSecond
		, Get().cCamAppParam.iMaxBitRateSecond
		, 512
		, 16*1024
		, "第二路H.264流输出最大比特率"
		, "单位：Kbps（千比特每秒）"
		, CUSTOM_LEVEL
	);

	//
	GetInt("\\CamApp"
		, "FrameRateSecond"
		, &Get().cCamAppParam.iFrameRateSecond
		, Get().cCamAppParam.iFrameRateSecond
		, 15
		, 25
		, "第二路H.264流输出帧率"
		, "单位：fps（帧每秒）"
		, CUSTOM_LEVEL
	);

	GetEnum("\\CamApp"
			, "RateControlSecond"
			, &Get().cCamAppParam.iRateControlSecond
			, Get().cCamAppParam.iRateControlSecond
			, "0:VBR;1:CBR"
			, "第二路H.264码率类型"
			, "0:VBR 可变码率;1:CBR 恒定码率"
			, CUSTOM_LEVEL);
	//end
	

    GetInt("\\CamApp"
    	, "GainR"
    	, &Get().cCamAppParam.iGainR
    	, Get().cCamAppParam.iGainR
    	, 0
    	, 255
    	, "R增益"
    	, "注意:AWB手动模式才下有效"
    	, CUSTOM_LEVEL
    );

    /* //球机机芯的G增益无法调
    GetInt("\\CamApp"
    	, "GainG"
    	, &Get().cCamAppParam.iGainG
    	, Get().cCamAppParam.iGainG
    	, 36
    	, 255
    	, "G增益"
    	, ""
    	, PROJECT_LEVEL
    );*/

    GetInt("\\CamApp"
    	, "GainB"
    	, &Get().cCamAppParam.iGainB
    	, Get().cCamAppParam.iGainB
    	, 0
    	, 255
    	, "B增益"
    	, "注意:AWB手动模式才下有效"
    	, CUSTOM_LEVEL
    );

	Get().cCamAppParam.iAutoFocus = 0;
	GetEnum("\\CamApp"
        , "AF"
        , &Get().cCamAppParam.iAutoFocus
        , Get().cCamAppParam.iAutoFocus
        , "0:自动聚焦;1:手动聚焦;"
        , "聚焦模式"
        , ""
        , CUSTOM_LEVEL
    );


	Get().cCamAppParam.iGain = 6;
	GetEnum("\\CamApp"
        , "Gain"
        , &Get().cCamAppParam.iGain
        , Get().cCamAppParam.iGain
        , "15:+28dB;"
			"14:+26dB;"
			"13:+24dB;"
			"12:+22dB;"
			"11:+20dB;"
			"10:+18dB;"
			"9:+16dB;"
			"8:+14dB;"
			"7:+12dB;" 
			"6:+10dB;"
			"5:+8dB;"
			"4:+6dB;"
			"3:+4dB;"
			"2:+2dB;"
			"1:0dB;"
			"0:-3dB"
        , "增益"
        , "注意:AE自动模式下设置该值无效!"
        , CUSTOM_LEVEL
    );


	Get().cCamAppParam.iShutter = 10;
	GetEnum("\\CamApp"
        , "Shutter"
        , &Get().cCamAppParam.iShutter
        , Get().cCamAppParam.iShutter
        , "21:1/10000;"
			"20:1/6000;"
			"19:1/3500;"
			"18:1/2500;"
			"17:1/1750;"
			"16:1/1250;"
			"15:1/1000;"
			"14:1/600;"
			"13:1/425;"
			"12:1/300;"
			"11:1/215;"
			"10:1/150;"
			"9:1/120;"
			"8:1/100;"
			"7:1/75;"
			"6:1/50;"
			"5:1/25;"
			"4:1/12;"
			"3:1/6;"
			"2:1/3;"
			"1:1/2;"
			"0:1/1"
        , "快门"
        , "注意:AE自动模式下设置该值无效!"
        , CUSTOM_LEVEL
    );

	Get().cCamAppParam.iIris = 7;
	GetEnum("\\CamApp"
        , "Iris"
        , &Get().cCamAppParam.iIris
        , Get().cCamAppParam.iIris
        , "13:F1.6;"
			"12:F2;"
			"11:F2.4;"
			"10:F2.8;"
			"9:F3.4;"
			"8:F4;"
			"7:F4.8;"
			"6:F5.6;"
			"5:F6.8;"
			"4:F8;"
			"3:F9.6;"
			"2:F11;"
			"1:F14;"
			"0:CLOSE"
        , "光圈"
        , "注意:AE自动模式下设置该值无效!"
        , CUSTOM_LEVEL
    );


	Get().cCamAppParam.iJpegAutoCompressEnable = 1;
	Get().cCamAppParam.iJpegExpectSize = 200 * 1024;  //默认200KB
/*
    GetEnum("\\CamApp"
        , "JpegAutoCompressEnable"
        , &Get().cCamAppParam.iJpegAutoCompressEnable
        , Get().cCamAppParam.iJpegAutoCompressEnable
        , "0:不使能;1:使能"
        , "Jpeg自动调节开关"
        , ""
        , PROJECT_LEVEL
    );


    GetInt("\\CamApp"
    	, "JpegExpectSize"
    	, &Get().cCamAppParam.iJpegExpectSize
    	, Get().cCamAppParam.iJpegExpectSize
    	, 0
    	, 1024*1024
    	, "Jpeg图片期望大小"
    	, "单位：Byte（字节）注：为0则表示忽略该参数"
    	, PROJECT_LEVEL
    );

    GetEnum("\\CamApp"
    	, "JpegType"
    	, &Get().cCamAppParam.iJpegType
    	, Get().cCamAppParam.iJpegType
    	, "0:YUV分开;1:YUV混合"
    	, "Jpeg图片格式"
    	, ""
    	, PROJECT_LEVEL
    );

    GetInt("\\CamApp"
    	, "JpegCompressRateLow"
    	, &Get().cCamAppParam.iJpegCompressRateL
    	, Get().cCamAppParam.iJpegCompressRateL
    	, 1
    	, 100
    	, "Jpeg压缩率自动调节下限"
    	, ""
    	, PROJECT_LEVEL
    );

    GetInt("\\CamApp"
    	, "JpegCompressRateHigh"
    	, &Get().cCamAppParam.iJpegCompressRateH
    	, Get().cCamAppParam.iJpegCompressRateH
    	, 1
    	, 100
    	, "Jpeg压缩率自动调节上限"
    	, ""
    	, PROJECT_LEVEL
    );
	*/
					

	Get().cCamAppParam.iAEMode = 0;
    GetEnum("\\CamApp"
    	, "AEMode"
    	, &Get().cCamAppParam.iAEMode
    	, Get().cCamAppParam.iAEMode
    	, "0:自动;1:手动;2:快门优先;3:光圈优先"
    	, "AE模式"
    	, ""
    	, CUSTOM_LEVEL
    );

	Get().cCamAppParam.iAWBMode = 0;
	GetEnum("\\CamApp"
    	, "AWBMode"
    	, &Get().cCamAppParam.iAWBMode
    	, Get().cCamAppParam.iAWBMode
    	, "0:自动;"
    		"1:ATW(Auto Tracing White balance);"
    		"2:室内;"
    		/*"3:室外;"*/ //Bug 9109: 因动力视讯机芯不支持该模式，且已有"6:自动户外;"模式，因此去掉
    		"4:触发一次调节(用白板调时可用到，断电后调节的值才会丢失);"
    		"5:手动;"
    		"6:自动户外;"
    		"7:自动纳灯;"
    		"8:纳灯"
    	, "AWB模式"
    	, ""
    	, CUSTOM_LEVEL
    );
    
    GetInt("\\CamApp"
    	, "SaturationValue"
    	, &Get().cCamAppParam.iSaturationValue
    	, Get().cCamAppParam.iSaturationValue
    	, 0
    	, 0xe
    	, "饱和度"
    	, ""
    	, CUSTOM_LEVEL
    );

	Get().cCamAppParam.iWDREnable = 0;
	GetEnum("\\CamApp"
            , "WDR"
            , &Get().cCamAppParam.iWDREnable
            , Get().cCamAppParam.iWDREnable
            , "1:开启;0:关闭"
            , "WDR"
            , ""
            , CUSTOM_LEVEL
            );

	Get().cCamAppParam.iSNFTNFMode = 0;
    GetEnum("\\CamApp"
            , "NoiseReduction"
            , &Get().cCamAppParam.iNRLevel
            , Get().cCamAppParam.iNRLevel
            , "0:不使能;1:等级1;2:等级2;3:等级3;4:等级4;5:等级5"
            , "降噪模式"
            , ""
            , CUSTOM_LEVEL
            );

    GetEnum("\\CamApp"
        , "BlackWhiteMode"
        , &Get().cCamAppParam.iGrayImageEnable
        , Get().cCamAppParam.iGrayImageEnable
        , "0:不使能;1:使能 "
        , "黑白图输出模式"
        ,""
        , PROJECT_LEVEL
    );

	Get().cCamAppParam.iGammaValue = 0;
	GetEnum("\\CamApp"
        , "GammaValue"
        , &Get().cCamAppParam.iGammaValue
        , Get().cCamAppParam.iGammaValue
        , "0:不使能;1:直线;2:S曲线-低;3:S曲线-中;4:S曲线-高"
        , "相机伽玛"
        ,""
        , CUSTOM_LEVEL
    );

	Get().cCamAppParam.iSharpenThreshold = 8;
	GetInt("\\CamApp"
        , "SharpenValue"
        , &Get().cCamAppParam.iSharpenThreshold
        , Get().cCamAppParam.iSharpenThreshold
        , 0
        , 15
        , "图像锐度"
        , "图像锐度"
        , PROJECT_LEVEL
    );

	Get().cCamAppParam.iEdgeValue = 0;
	GetInt("\\CamApp"
        , "EdgeValue"
        , &Get().cCamAppParam.iEdgeValue
        , Get().cCamAppParam.iEdgeValue
        , 0
        , 255
        , "图像边缘增强"
        , "图像边缘增强"
        , CUSTOM_LEVEL
    );

	
	/*
	球机无cvbs输出。
	GetEnum("\\CamApp"
        , "CVBSMode"
        , &Get().cCamAppParam.iCVBSMode
        , Get().cCamAppParam.iCVBSMode
        , "0:PAL;1:NTSC"
        , "CVBS制式"
        , ""
        , CUSTOM_LEVEL
    );
	
	Get().cCamAppParam.iCVBSExport = 1;		//默认 以 剪切输出
	GetEnum("\\CamApp"
		, "CVBSIMAGEMode"
		, &Get().cCamAppParam.iCVBSExport
		, Get().cCamAppParam.iCVBSExport
		, "0:CVBS 缩小输出;1:CVBS 剪切输出"
		, "CVBS图像模式"
		, ""
		, CUSTOM_LEVEL
	);*/

	return S_OK;
}

HRESULT CSWLPRJupiterDomeCameraParameter::InitCharacter(VOID)
{
	
	Get().cOverlay.fH264Eanble = 1;
	GetEnum("\\Overlay\\H264"
		  , "Enable"
		  , &Get().cOverlay.fH264Eanble
		  , Get().cOverlay.fH264Eanble
		  , "0:不使能;1:使能"
		  , "H264字符叠加使能"
		  , ""
		  , CUSTOM_LEVEL
	);
	
	GetInt("\\Overlay\\H264"
		  , "X"
		  , &Get().cOverlay.cH264Info.iTopX
		  , Get().cOverlay.cH264Info.iTopX
		  , 0
		  , 1920
		  , "X坐标"
		  , ""
		  , CUSTOM_LEVEL
	  );
	  
	GetInt("\\Overlay\\H264"
		  , "Y"
		  , &Get().cOverlay.cH264Info.iTopY
		  , Get().cOverlay.cH264Info.iTopY
		  , 0
		  , 1080
		  , "Y坐标"
		  , ""
		  , CUSTOM_LEVEL
	  ); 

	Get().cOverlay.cH264Info.iFontSize = 64;
	GetInt("\\Overlay\\H264"
		  , "Size"
		  , &Get().cOverlay.cH264Info.iFontSize
		  , Get().cOverlay.cH264Info.iFontSize
		  , 16
		  , 128
		  , "字体大小"
		  , ""
		  , CUSTOM_LEVEL
	  ); 
	DWORD dwR = 255, dwG = 0,dwB = 0;  
	GetInt("\\Overlay\\H264"
		  , "R"
		  , (INT *)&dwR
		  , dwR
		  , 0
		  , 255
		  , "字体颜色R分量"
		  , ""
		  , CUSTOM_LEVEL
	  );
	GetInt("\\Overlay\\H264"
		  , "G"
		  , (INT *)&dwG
		  , dwG
		  , 0
		  , 255
		  , "字体颜色G分量"
		  , ""
		  , CUSTOM_LEVEL
	  );
	GetInt("\\Overlay\\H264"
		  , "B"
		  , (INT *)&dwB
		  , dwB
		  , 0
		  , 255
		  , "字体颜色B分量"
		  , ""
		  , CUSTOM_LEVEL
	  );
	Get().cOverlay.cH264Info.dwColor = (dwB | (dwG << 8) | (dwR << 16));
	SW_TRACE_DEBUG("h264 color[0x%08x][0x%02x,0x%02x,0x%02x]", Get().cOverlay.cH264Info.dwColor, dwR, dwG, dwB);
		Get().cOverlay.cH264Info.fEnableTime = 1;  //默认时能时间叠加
	GetEnum("\\Overlay\\H264"
		  , "DateTime"
		  , &Get().cOverlay.cH264Info.fEnableTime
		  , Get().cOverlay.cH264Info.fEnableTime
		  , "0:不使能;1:使能"
		  , "叠加时间"
		  , ""
		  , CUSTOM_LEVEL
	);


	swpa_sprintf(Get().cOverlay.cH264Info.szInfo, " ");
	GetString("\\Overlay\\H264"
		  , "String"
		  , Get().cOverlay.cH264Info.szInfo
		  , Get().cOverlay.cH264Info.szInfo
		  , sizeof(Get().cOverlay.cH264Info.szInfo)
		  , ""
		  , "叠加信息"
		  , CUSTOM_LEVEL
	);
    if(!swpa_strcmp(Get().cOverlay.cH264Info.szInfo, "NULL"))
	{
	    swpa_strcpy(Get().cOverlay.cH264Info.szInfo, "");
	}
	
	// todo.字符叠加计算字符总长度有问题，在设置时多加两个空格在尾部，避免因过长叠加显示不全。
	if( strlen(Get().cOverlay.cH264Info.szInfo) + 3 < sizeof(Get().cOverlay.cH264Info.szInfo) )
	{
		strcpy(Get().cOverlay.cH264Info.szInfo + strlen(Get().cOverlay.cH264Info.szInfo), "  ");
		SW_TRACE_NORMAL("<param>change overlay h264 len.%s", Get().cOverlay.cH264Info.szInfo);
	}
	

	  Get().cOverlay.cJPEGInfo.fEnable = 1;
	  GetEnum("\\Overlay\\JPEG"
		  , "Enable"
		  , &Get().cOverlay.cJPEGInfo.fEnable
		  , Get().cOverlay.cJPEGInfo.fEnable
		  , "0:不使能;1:使能"
		  , "JPEG字符叠加使能"
		  , ""
		  , PROJECT_LEVEL
	  );
	  
	GetInt("\\Overlay\\JPEG"
		  , "X"
		  , &Get().cOverlay.cJPEGInfo.iX
		  , Get().cOverlay.cJPEGInfo.iX
		  , 0
		  , 1920
		  , "X坐标"
		  , ""
		  , PROJECT_LEVEL
	  );
	  
	GetInt("\\Overlay\\JPEG"
		  , "Y"
		  , &Get().cOverlay.cJPEGInfo.iY
		  , Get().cOverlay.cJPEGInfo.iY
		  , 0
		  , 1080
		  , "Y坐标"
		  , ""
		  , PROJECT_LEVEL
	  ); 

	Get().cOverlay.cJPEGInfo.iFontSize = 64;
	GetInt("\\Overlay\\JPEG"
		  , "Size"
		  , &Get().cOverlay.cJPEGInfo.iFontSize
		  , Get().cOverlay.cJPEGInfo.iFontSize
		  , 16
		  , 128
		  , "字体大小"
		  , ""
		  , PROJECT_LEVEL
	  ); 

	dwR = 255;
	dwG = 0;
	dwB = 0;
	GetInt("\\Overlay\\JPEG"
		  , "R"
		  , (INT *)&dwR
		  , dwR
		  , 0
		  , 255
		  , "字体颜色R分量"
		  , ""
		  , PROJECT_LEVEL
	  );
	GetInt("\\Overlay\\JPEG"
		  , "G"
		  , (INT *)&dwG
		  , dwG
		  , 0
		  , 255
		  , "字体颜色G分量"
		  , ""
		  , PROJECT_LEVEL
	  );
	GetInt("\\Overlay\\JPEG"
		  , "B"
		  , (INT *)&dwB
		  , dwB
		  , 0
		  , 255
		  , "字体颜色B分量"
		  , ""
		  , PROJECT_LEVEL
	  );
	Get().cOverlay.cJPEGInfo.iColor = (dwB | (dwG << 8) | (dwR << 16));
	SW_TRACE_DEBUG("jpeg color[0x%08x][0x%02x,0x%02x,0x%02x]", Get().cOverlay.cJPEGInfo.iColor, dwR, dwG, dwB);
	Get().cOverlay.cJPEGInfo.fEnableTime = 1;
	GetEnum("\\Overlay\\JPEG"
		  , "DateTime"
		  , &Get().cOverlay.cJPEGInfo.fEnableTime
		  , Get().cOverlay.cJPEGInfo.fEnableTime
		  , "0:不使能;1:使能"
		  , "叠加时间"
		  , ""
		  , PROJECT_LEVEL
	  );

	 //swpa_sprintf(Get().cOverlay.cJPEGInfo.szInfo, "字符叠加样例");
	 swpa_sprintf(Get().cOverlay.cJPEGInfo.szInfo, " ");
	 GetString("\\Overlay\\JPEG"
		  , "String"
		  , Get().cOverlay.cJPEGInfo.szInfo
		  , " " //Get().cOverlay.cJPEGInfo.szInfo
		  , sizeof(Get().cOverlay.cJPEGInfo.szInfo)
		  , ""
		  , "叠加信息"
		  , PROJECT_LEVEL
	  );

	if(!swpa_strcmp(Get().cOverlay.cJPEGInfo.szInfo, "NULL"))
	{
	  swpa_strcpy(Get().cOverlay.cJPEGInfo.szInfo, "");
	}
	 // todo.字符叠加计算字符总长度有问题，在设置时多加两个空格在尾部，避免因过长叠加显示不全。
	 if( strlen(Get().cOverlay.cJPEGInfo.szInfo) + 3 < sizeof(Get().cOverlay.cJPEGInfo.szInfo) )
	 {
		strcpy(Get().cOverlay.cJPEGInfo.szInfo + strlen(Get().cOverlay.cJPEGInfo.szInfo), "  ");
		SW_TRACE_NORMAL("<param>change overlay jpeg len.%s", Get().cOverlay.cJPEGInfo.szInfo);
	 }

	  //金星没有叠加车牌号码使能开关
	/*GetEnum("\\Overlay\\JPEG"
		  , "Plate"
		  , &Get().cOverlay.cJPEGInfo.fEnablePlate
		  , Get().cOverlay.cJPEGInfo.fEnablePlate
		  , "0:不使能;1:使能"
		  , "叠加车牌号码"
		  , ""
		  , CUSTOM_LEVEL
	  );*/
	  return S_OK;
}

HRESULT CSWLPRJupiterDomeCameraParameter::InitGB28181(VOID)
{
	return CSWLPRParameter::InitGB28181();
}


HRESULT CSWLPRJupiterDomeCameraParameter::InitAutoReboot(VOID)
{	

	GetEnum("\\AutoReboot"
	  , "Enable"
	  , &Get().cAutoRebootParam.fEnable
	  , Get().cAutoRebootParam.fEnable
	  , "0:不使能;1:使能"
	  , "自动复位"
	  , ""
	  , CUSTOM_LEVEL
	);	

	GetInt("\\AutoReboot"
		  , "TrafficFlows"
		  , &Get().cAutoRebootParam.iCarLeftCountLimit
		  , Get().cAutoRebootParam.iCarLeftCountLimit
		  , 0
		  , 1000
		  , "交通流量"
		  , "单位为十分钟内车辆总数"
		  , CUSTOM_LEVEL
	  );

	GetInt("\\AutoReboot"
		  , "SystemUpTime"
		  , &Get().cAutoRebootParam.iSysUpTimeLimit
		  , Get().cAutoRebootParam.iSysUpTimeLimit
		  , 24
		  , 720
		  , "运行时间"
		  , "单位:小时"
		  , CUSTOM_LEVEL
	  );


	return S_OK;
}



HRESULT CSWLPRJupiterDomeCameraParameter::InitONVIF(VOID)
{

    SW_TRACE_NORMAL(" HRESULT CSWLPRJupiterDomeCameraParameter::InitONVIF(VOID)");

	Get().cRtspParam.iRTSPStreamNum = 2;
    Get().cRtspParam.rgCommunicationMode[RTSP_STREAM_MAJOR] = 0;
    Get().cRtspParam.rgCommunicationMode[RTSP_STREAM_MINOR] = 0;


/*
	GetEnum("\\RTSP"
        , "CommunicationMode"
        , &Get().cRtspParam.iMajorCommunicationMode
        , Get().cRtspParam.iMajorCommunicationMode
        , "0:单播;1:组播"
        , "通迅方式"
        , ""
        , PROJECT_LEVEL
     );


	GetInt("\\RTSP"
		   , "iRTSPStreamNum"
		   , &Get().cRtspParam.iRTSPStreamNum
		   , Get().cRtspParam.iRTSPStreamNum
		   , 1
		   , 2
		   , "RTSP视频流数量"
		   , ""
		   , PROJECT_LEVEL
		);
*/

    SW_TRACE_NORMAL(" HRESULT CSWLPRJupiterDomeCameraParameter::InitONVIF(VOID) End");

    return S_OK;
}



