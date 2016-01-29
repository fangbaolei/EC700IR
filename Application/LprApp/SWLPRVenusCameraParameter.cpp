/*
 * SWLPRVenusTollGateParameter.cpp
 *
 *  Created on: 2013年12月27日
 *      Author: qinjj
 */
#include "SWFC.h"
#include "SWLPRApplication.h"
#include "SWLPRVenusCameraParameter.h"


SWLPRVenusCameraParameter::SWLPRVenusCameraParameter()
{
	ResetParam();
}

SWLPRVenusCameraParameter::~SWLPRVenusCameraParameter()
{
}

void SWLPRVenusCameraParameter::ResetParam(VOID)
{
	//先调用基类
	CSWLPRParameter::ResetParam();

	/*根据具体方案配置参数*/

    Get().nWorkModeIndex = PRM_CAMERA;
    m_strWorkMode.Format("纯相机", Get().nWorkModeIndex);
	Get().nCameraWorkMode = 0;				//纯相机、收费站方案相机工作模式
	Get().nMCFWUsecase = IMX185_DUALVIDEO;
    //车道默认值
    for (int i = 0; i < MAX_ROADLINE_NUM; i++)
    {
        swpa_memset(&Get().cTrackerCfgParam.rgcRoadInfo[i], 0, sizeof(ROAD_INFO));
        swpa_strcpy(Get().cTrackerCfgParam.rgcRoadInfo[i].szRoadTypeName, "NULL");
        Get().cTrackerCfgParam.rgcRoadInfo[i].iLineType = 1;
        Get().cTrackerCfgParam.rgcRoadInfo[i].iRoadType = (int)(RRT_FORWARD | RRT_LEFT | RRT_RIGHT | RRT_TURN);
        Get().cTrackerCfgParam.cActionDetect.iIsYellowLine[i] = 0;
        Get().cTrackerCfgParam.cActionDetect.iIsCrossLine[i] = 0;
    }

    //扫描区域默认值
    Get().cTrackerCfgParam.cDetectArea.fEnableDetAreaCtrl = FALSE;
    Get().cTrackerCfgParam.cDetectArea.DetectorAreaLeft = 0;
    Get().cTrackerCfgParam.cDetectArea.DetectorAreaRight = 95;
    Get().cTrackerCfgParam.cDetectArea.DetectorAreaTop = 30;
    Get().cTrackerCfgParam.cDetectArea.DetectorAreaBottom = 90;
    Get().cTrackerCfgParam.cDetectArea.nDetectorMinScaleNum = 6;
    Get().cTrackerCfgParam.cDetectArea.nDetectorMaxScaleNum = 14;

    //投票参数
    Get().cTrackerCfgParam.nVoteFrameNum = 16;
    Get().cTrackerCfgParam.nMaxEqualFrameNumForVote = 8;

    //视频检测参数
    Get().cTrackerCfgParam.cVideoDet.nVideoDetMode = NO_VIDEODET;

    // 前端相机参数
    Get().cCamCfgParam.iMaxExposureTime = 15000;
    Get().cCamCfgParam.iMinExposureTime = 0;
    Get().cCamCfgParam.iMaxGain = 180;

    // 脉宽默认参数值
    Get().cCamCfgParam.iMinPSD = 300;
    Get().cCamCfgParam.iMaxPSD = 1500;

    //外总控默认值
    Get().cMatchParam.dwSignalKeepTime = 6000;
        Get().cMatchParam.dwPlateKeepTime = 2000;
        Get().cMatchParam.dwMatchMinTime = 3000;
        Get().cMatchParam.dwMatchMaxTime = 3000;

    //收费站默认不检测逆行
    Get().cTrackerCfgParam.nDetReverseRunEnable = 0;

    Get().cDevParam[1].iCommType = 1;	//金星只有一个串口COM1,只支持485

    Get().cCamAppParam.iResolution = 0;	//默认为1080P

    // 抓拍位置初始值
    Get().cTrackerCfgParam.nCaptureOnePos = 50;

    // 金星无频闪补光灯
    Get().cTrackerCfgParam.iUsedLight = 0;

    //金星扫描区域默认打开
    Get().cTrackerCfgParam.cTrapArea.fEnableDetAreaCtrl = TRUE;

    //金星RGB分量配置范围为36~255,默认值不能为0
    Get().cCamAppParam.iGainR = 36;
    Get().cCamAppParam.iGainG = 36;
    Get().cCamAppParam.iGainB = 36;

}

HRESULT SWLPRVenusCameraParameter::Initialize(CSWString strFilePath)
{
	// 初始化前重新设置默认值
	ResetParam();
	
    // 先基类的初始化
    if (S_OK == CSWParameter<ModuleParams>::Initialize(strFilePath)
        && S_OK == InitSystem()
        && S_OK == InitCamera()
        && S_OK == InitHvDsp()
        && S_OK == InitCharacter()
        && S_OK == InitGB28181()
		&& S_OK == InitONVIF())
    {
        return S_OK ;
    }

    SW_TRACE_DEBUG("<SWLPRVenusCameraParameter> Initialize failed.\n");
    return E_FAIL ;
}

HRESULT SWLPRVenusCameraParameter::InitSystem(VOID)
{
    if (S_OK == CSWLPRParameter::InitSystem())
    {
        return S_OK ;
    }
    return E_FAIL ;

}

HRESULT SWLPRVenusCameraParameter::InitTracker(VOID)
{
    return S_OK;
}

HRESULT SWLPRVenusCameraParameter::InitHvDsp(VOID)
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
        , PROJECT_LEVEL
    );

    GetEnum("\\HvDsp\\Misc"
        , "InitHdd"
        , &Get().cResultSenderParam.fInitHdd
        , Get().cResultSenderParam.fInitHdd
        , "0:无;1:分区格式化"
        , "初始化SD卡。注：一次性有效"
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
        , "SD卡存储方式"
        , ""
        , PROJECT_LEVEL
    );

    GetEnum("\\HvDsp\\Misc"
        , "DiskType"
        , &Get().cResultSenderParam.iDiskType
        , Get().cResultSenderParam.iDiskType
        , "0:无;2:SD卡"
        , "存储类型"
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

    /*GetString("\\HvDsp\\Misc"
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
    );*/

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


    /*GetString("\\HvDsp\\Identify", "StreetName"
        , Get().cResultSenderParam.szStreetName
        , Get().cResultSenderParam.szStreetName
        , sizeof(Get().cResultSenderParam.szStreetName)
        , "路口名称"
        , ""
        , CUSTOM_LEVEL
        );

    GetString("\\HvDsp\\Identify"
        , "StreetDirection"
        , Get().cResultSenderParam.szStreetDirection
        , Get().cResultSenderParam.szStreetDirection
        , sizeof(Get().cResultSenderParam.szStreetDirection)
        , "路口方向"
        , ""
        , CUSTOM_LEVEL
        );*/

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

    GetEnum("\\HvDsp\\Trigger"
        , "TriggerOutNormalStatus"
        , &Get().cResultSenderParam.nTriggerOutNormalStatus
        , Get().cResultSenderParam.nTriggerOutNormalStatus
        , "0:高电平;1:低电平"
        , "触发输出常态控制"
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

    return S_OK ;
}

HRESULT SWLPRVenusCameraParameter::InitOuterCtrl(VOID)
{
    return S_OK ;
}


HRESULT SWLPRVenusCameraParameter::InitIPTCtrl(VOID)
{
    return S_OK;
}


HRESULT SWLPRVenusCameraParameter::InitCamApp(VOID)
{
    return S_OK;
}

HRESULT SWLPRVenusCameraParameter::InitCamera(VOID)
{
    GetInt("\\CamApp"
        , "JpegCompressRate"
        , &Get().cCamAppParam.iJpegCompressRate
        , Get().cCamAppParam.iJpegCompressRate
        , 1
        , 99
        , "视频流Jpeg压缩品质"
        , ""
        , CUSTOM_LEVEL
    );

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

    GetInt("\\CamApp"
        , "TargetBitRate"
        , &Get().cCamAppParam.iTargetBitRate
        , Get().cCamAppParam.iTargetBitRate
        , 512
        , 16*1024
        , "H.264流输出比特率"
        , "单位：Kbps（千比特每秒）"
        , CUSTOM_LEVEL
    );
//第二路H264参数
//暂时不支持单独设置I帧间隔
/*
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
*/
	GetEnum("\\CamApp"
		, "ResolutionSecond"
		, &Get().cCamAppParam.iResolutionSecond
		, Get().cCamAppParam.iResolutionSecond
		, "0:1080P;1:720P;2:576P;3:480P"
		, "第二路H.264图像分辨率"
		, ""
		, CUSTOM_LEVEL
	);

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
//end

    GetInt("\\CamApp"
        , "AGCTh"
        , &Get().cCamAppParam.iAGCTh
        , Get().cCamAppParam.iAGCTh
        , 0
        , 255
        , "AGC测光基准值"
        , ""
        , CUSTOM_LEVEL
    );

    GetInt("\\CamApp"
        , "AGCShutterLOri"
        , &Get().cCamAppParam.iAGCShutterLOri
        , Get().cCamAppParam.iAGCShutterLOri
        , 0
        , 30000
        , "AGC快门调节下限"
        , ""
        , CUSTOM_LEVEL
    );

    Get().cCamAppParam.iAGCShutterHOri = 10000;
    GetInt("\\CamApp"
        , "AGCShutterHOri"
        , &Get().cCamAppParam.iAGCShutterHOri
        , Get().cCamAppParam.iAGCShutterHOri
        , 0
        , 30000
        , "AGC快门调节上限"
        , ""
        , CUSTOM_LEVEL
    );

    GetInt("\\CamApp"
        , "AGCGainLOri"
        , &Get().cCamAppParam.iAGCGainLOri
        , Get().cCamAppParam.iAGCGainLOri
        , 0
        , 480					//金星185前端增益范围为0~480
        , "AGC增益调节下限"
        , ""
        , PROJECT_LEVEL
    );

    Get().cCamAppParam.iAGCGainHOri = 100;
    GetInt("\\CamApp"
        , "AGCGainHOri"
        , &Get().cCamAppParam.iAGCGainHOri
        , Get().cCamAppParam.iAGCGainHOri
        , 0
        , 480					//金星185前端增益范围为0~480
        , "AGC增益调节上限"
        , ""
        , CUSTOM_LEVEL
    );

    GetEnum("\\CamApp"
        , "AGCScene"
        , &Get().cCamAppParam.iAGCScene
        , Get().cCamAppParam.iAGCScene
        , "0:自动;1:偏暗;2:标准;3:较亮"
        , "情景模式"
        , ""
        , CUSTOM_LEVEL
    );

    GetInt("\\CamApp"
        , "GainR"
        , &Get().cCamAppParam.iGainR
        , Get().cCamAppParam.iGainR
        , 36
        , 255
        , "R增益"
        , ""
        , CUSTOM_LEVEL
    );

    GetInt("\\CamApp"
        , "GainG"
        , &Get().cCamAppParam.iGainG
        , Get().cCamAppParam.iGainG
        , 36
        , 255
        , "G增益"
        , ""
        , CUSTOM_LEVEL
    );

    GetInt("\\CamApp"
        , "GainB"
        , &Get().cCamAppParam.iGainB
        , Get().cCamAppParam.iGainB
        , 36
        , 255
        , "B增益"
        , ""
        , CUSTOM_LEVEL
    );

    GetInt("\\CamApp"
        , "Gain"
        , &Get().cCamAppParam.iGain
        , Get().cCamAppParam.iGain
        , 0
        , 480					//金星185前端最大增益为480
        , "增益"
        , ""
        , CUSTOM_LEVEL
    );

    GetInt("\\CamApp"
        , "Shutter"
        , &Get().cCamAppParam.iShutter
        , Get().cCamAppParam.iShutter
        , 0
        , 30000
        , "快门"
        , "单位：us"
        , CUSTOM_LEVEL
    );

    GetEnum("\\CamApp"
        , "JpegAutoCompressEnable"
        , &Get().cCamAppParam.iJpegAutoCompressEnable
        , Get().cCamAppParam.iJpegAutoCompressEnable
        , "0:不使能;1:使能"
        , "Jpeg自动调节开关"
        , ""
        , CUSTOM_LEVEL
    );

    GetInt("\\CamApp"
        , "JpegExpectSize"
        , &Get().cCamAppParam.iJpegExpectSize
        , Get().cCamAppParam.iJpegExpectSize
        , 0
        , 1024*1024
        , "Jpeg图片期望大小"
        , "单位：Byte（字节）注：为0则表示忽略该参数"
        , CUSTOM_LEVEL
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
        , 79
        , "Jpeg压缩率自动调节下限"
        , ""
        , CUSTOM_LEVEL
    );

    Get().cCamAppParam.iJpegCompressRateH = 80;
    GetInt("\\CamApp"
        , "JpegCompressRateHigh"
        , &Get().cCamAppParam.iJpegCompressRateH
        , Get().cCamAppParam.iJpegCompressRateH
        , 1
        , 80
        , "Jpeg压缩率自动调节上限"
        , ""
        , PROJECT_LEVEL 
    );

    Get().cCamAppParam.iAGCEnable = 1;
    GetEnum("\\CamApp"
        , "AGCEnable"
        , &Get().cCamAppParam.iAGCEnable
        , Get().cCamAppParam.iAGCEnable
        , "0:不使能;1:使能"
        , "AGC使能"
        , ""
        , CUSTOM_LEVEL
    );

    Get().cCamAppParam.iAWBEnable = 1;
    GetEnum("\\CamApp"
        , "AWBEnable"
        , &Get().cCamAppParam.iAWBEnable
        , Get().cCamAppParam.iAWBEnable
        , "0:不使能;1:使能"
        , "AWB使能"
        , ""
        , CUSTOM_LEVEL
    );

    //该参数不光跟抓拍有关，还跟"TriggerOutEnable""触发输出使能开关"有关，不能去掉
    GetEnum("\\CamApp"
        , "CaptureSynOutputType"
        , &Get().cCamAppParam.iCaptureSynOutputType
        , Get().cCamAppParam.iCaptureSynOutputType
        , "0:上拉(电平);1:OC（开关）"
        , "抓拍输出类型"
        , ""
        , CUSTOM_LEVEL
        );

    GetEnum("\\CamApp"
        , "ExternSyncMode"
        , &Get().cCamAppParam.iExternalSyncMode
        , Get().cCamAppParam.iExternalSyncMode
        , "0:不同步;1:内部电网同步"
        , "外同步模式"
        , ""
        , CUSTOM_LEVEL
        );
    GetInt("\\CamApp"
        , "SyncRelay"
        , &Get().cCamAppParam.iSyncRelay
        , Get().cCamAppParam.iSyncRelay
        , 0
        , 20000
        , "外同步信号延时"
        , "单位:微秒"
        , CUSTOM_LEVEL
        );

    /*GetEnum("\\CamApp"
        , "CaptureEdge"
        , &Get().cCamAppParam.iCaptureEdge
        , Get().cCamAppParam.iCaptureEdge
        , "0:不触发;1:外部下降沿触发;2:外部上升沿触发;3:上升沿下降沿都触发"
        , "触发抓拍沿"
        , ""
        , CUSTOM_LEVEL
    );*/

    char szAGC[255];
    //BOOL fAllAGCZoneIsOff = TRUE;
    for (int i = 0; i < 16; i++)
    {
        sprintf(szAGC, "AGC%d", i+1);
        GetEnum("\\CamApp\\AGCZone"
            , szAGC
            , &Get().cCamAppParam.rgiAGCZone[i]
            , Get().cCamAppParam.rgiAGCZone[i]
            , "0:关闭;1:打开"
            , ""
            , ""
            , CUSTOM_LEVEL
        );
        /*
        if (1 == Get().cCamAppParam.rgiAGCZone[i])
        {
            fAllAGCZoneIsOff = FALSE;
        }*/
    }
    //底层做限制
    /*
    //测光区域必须选一个
    if (TRUE == fAllAGCZoneIsOff)
    {
        Get().cCamAppParam.rgiAGCZone[0] = 1;
        UpdateInt("\\CamApp\\AGCZone"
            , "AGC0"
            , Get().cCamAppParam.rgiAGCZone[0]
            );
    }
    */

    GetEnum("\\CamApp"
        , "EnableDCAperture"
        , &Get().cCamAppParam.iEnableDCAperture
        , Get().cCamAppParam.iEnableDCAperture
        , "0:不使能;1:使能"
        , "使能DC光圈"
        , ""
        , CUSTOM_LEVEL
    );

    GetEnum("\\CamApp"
        , "ColorMatrixMode"
        , &Get().cCamAppParam.iColorMatrixMode
        , Get().cCamAppParam.iColorMatrixMode
        , "0:不使能;1:使能"
        , "图像增强"
        , ""
        , CUSTOM_LEVEL
    );

    GetInt("\\CamApp"
        , "ContrastValue"
        , &Get().cCamAppParam.iContrastValue
        , Get().cCamAppParam.iContrastValue
        , -100
        , 100
        , "对比度"
        , ""
        , CUSTOM_LEVEL
    );

    GetInt("\\CamApp"
        , "SaturationValue"
        , &Get().cCamAppParam.iSaturationValue
        , Get().cCamAppParam.iSaturationValue
        , -100
        , 100
        , "饱和度"
        , ""
        , CUSTOM_LEVEL
    );

    GetInt("\\CamApp"
        , "WDRThreshold"
        , &Get().cCamAppParam.iWDRValue
        , Get().cCamAppParam.iWDRValue
        , 0
        , 4095					//宽动态范围为 0~4095
        , "WDR强度"
        , ""
        , CUSTOM_LEVEL
    );

    Get().cCamAppParam.iSNFTNFMode == 3;
    GetEnum("\\CamApp"
            , "NoiseReduction"
            , &Get().cCamAppParam.iSNFTNFMode
            , Get().cCamAppParam.iSNFTNFMode
            , "0:不使能;1:TNF模式;2:SNF模式;3:TNF与SNF模式"
            , "降噪模式"
            , ""
            , CUSTOM_LEVEL
            );

    Get().cCamAppParam.iTNFSNFValue = 0;
    GetEnum("\\CamApp"
            , "NoiseReductionThreshold"
            , &Get().cCamAppParam.iTNFSNFValue
            , Get().cCamAppParam.iTNFSNFValue
            , "0:自动;1:低;2:中;3:高"
            , "降噪强度"
            , ""
            , CUSTOM_LEVEL
            );

    GetEnum("\\CamApp"
        , "CVBSMode"
        , &Get().cCamAppParam.iCVBSMode
        , Get().cCamAppParam.iCVBSMode
        , "0:PAL;1:NTSC"
        , "CVBS制式"
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
        , CUSTOM_LEVEL
    );
    //todo使能伽马会导致图像偏色
    /*
    GetInt("\\CamApp"
        , "GammaValue"
        , &Get().cCamAppParam.iGammaValue
        , Get().cCamAppParam.iGammaValue
        , 10
        , 22
        , "相机伽玛"
        , ""
        , CUSTOM_LEVEL
    );
    GetEnum("\\CamApp"
        , "GammaEnable"
        , &Get().cCamAppParam.iGammaEnable
        , Get().cCamAppParam.iGammaEnable
        ,"0:不使能;1:使能"
        , "相机伽玛使能"
        , ""
        , CUSTOM_LEVEL
    );
    */

    GetInt("\\CamApp"
        , "EdgeValue"
        , &Get().cCamAppParam.iEdgeValue
        , Get().cCamAppParam.iEdgeValue
        ,0
        ,255
        , "图像边缘增强"
        , "图像边缘增强"
        , CUSTOM_LEVEL
    );

    /*GetEnum("\\CamApp"
        , "LEDPolarity"
        , &Get().cCamAppParam.iLEDPolarity
        , Get().cCamAppParam.iLEDPolarity
        , "0:负极性;1:正极性"
        , "补光灯控制极性"
        , ""
        , CUSTOM_LEVEL
    );

    GetEnum("\\CamApp"
        , "LEDOutputType"
        , &Get().cCamAppParam.iLEDOutputType
        , Get().cCamAppParam.iLEDOutputType
        , "0:上拉（电平）;1:OC(开关)"
        , "补光灯输出类型"
        , ""
        , CUSTOM_LEVEL
    );*/

    GetEnum("\\CamApp"
        , "ALMPolarity"
        , &Get().cCamAppParam.iALMPolarity
        , Get().cCamAppParam.iALMPolarity
        , "0:负极性;1:正极性"
        , "报警控制极性"
        , ""
        , PROJECT_LEVEL
    );
    GetEnum("\\CamApp"
        , "ALMOutputType"
        , &Get().cCamAppParam.iALMOutputType
        , Get().cCamAppParam.iALMOutputType
        , "0:上拉（电平）;1:OC(开关)"
        , "报警输出类型"
        , ""
        , PROJECT_LEVEL
    );
    GetEnum("\\CamApp"
        , "ColorGradation"
        , &Get().cCamAppParam.iColorGradation
        , Get().cCamAppParam.iColorGradation
        , "0:0~255;1:16~234"
        , "色阶"
        , "色阶"
        , CUSTOM_LEVEL
    );
	
	//Get().cCamAppParam.iCVBSExport = 1;
	GetEnum("\\CamApp"
		, "CVBSIMAGEMode"
		, &Get().cCamAppParam.iCVBSExport
		, Get().cCamAppParam.iCVBSExport
		, "0:CVBS 缩小输出;1:CVBS 剪切输出;2:自定义剪切输出"
		, "CVBS图像模式"
		, ""
		, CUSTOM_LEVEL
	);
	if (Get().cCamAppParam.iCVBSExport != 0)
	{
		Get().nMCFWUsecase = IMX185_DUALVIDEO_CVBSEXPORT;
	}

    Get().cCamAppParam.iCVBSCropStartX = 600;
    GetInt("\\CamApp"
		, "CVBSCropStartX"
		, &Get().cCamAppParam.iCVBSCropStartX
		, Get().cCamAppParam.iCVBSCropStartX
		, 0
		, 1199
		, "CVBS裁剪模式起始横坐标"
		, "CVBS裁剪模式起始横坐标"
		, CUSTOM_LEVEL
	);

    Get().cCamAppParam.iCVBSCropStartY = 252;
    GetInt("\\CamApp"
		, "CVBSCropStartY"
		, &Get().cCamAppParam.iCVBSCropStartY
		, Get().cCamAppParam.iCVBSCropStartY
		, 0
		, 503
		, "CVBS裁剪模式起始纵坐标"
		, "CVBS裁剪模式起始纵坐标"
		, CUSTOM_LEVEL
	);


    return S_OK;
}

/**
*
*/
HRESULT SWLPRVenusCameraParameter::InitCharacter(VOID)
{
//第一路H264字符叠加
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

    GetInt("\\Overlay\\H264"
          , "Size"
          , &Get().cOverlay.cH264Info.iFontSize
          , 32
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
    GetEnum("\\Overlay\\H264"
          , "DateTime"
          , &Get().cOverlay.cH264Info.fEnableTime
          , Get().cOverlay.cH264Info.fEnableTime
          , "0:不使能;1:使能"
          , "叠加时间"
          , ""
          , CUSTOM_LEVEL
      );

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
//第二路H264字符叠加
	  
	GetEnum("\\Overlay\\H264Second"
          , "Enable"
          , &Get().cOverlay.fH264SecondEnable
          , Get().cOverlay.fH264SecondEnable
          , "0:不使能;1:使能"
          , "H264字符叠加使能"
          , ""
          , CUSTOM_LEVEL
    );

    GetInt("\\Overlay\\H264Second"
          , "X"
          , &Get().cOverlay.cH264SecondInfo.iTopX
          , Get().cOverlay.cH264SecondInfo.iTopX
          , 0
          , 1920
          , "X坐标"
          , ""
          , CUSTOM_LEVEL
      );

    GetInt("\\Overlay\\H264Second"
          , "Y"
          , &Get().cOverlay.cH264SecondInfo.iTopY
          , Get().cOverlay.cH264SecondInfo.iTopY
          , 0
          , 1080
          , "Y坐标"
          , ""
          , CUSTOM_LEVEL
      );

    GetInt("\\Overlay\\H264Second"
          , "Size"
          , &Get().cOverlay.cH264SecondInfo.iFontSize
          , 32
          , 16
          , 128
          , "字体大小"
          , ""
          , CUSTOM_LEVEL
      );
   	dwR = 255, dwG = 0,dwB = 0;
    GetInt("\\Overlay\\H264Second"
          , "R"
          , (INT *)&dwR
          , dwR
          , 0
          , 255
          , "字体颜色R分量"
          , ""
          , CUSTOM_LEVEL
      );
    GetInt("\\Overlay\\H264Second"
          , "G"
          , (INT *)&dwG
          , dwG
          , 0
          , 255
          , "字体颜色G分量"
          , ""
          , CUSTOM_LEVEL
      );
    GetInt("\\Overlay\\H264Second"
          , "B"
          , (INT *)&dwB
          , dwB
          , 0
          , 255
          , "字体颜色B分量"
          , ""
          , CUSTOM_LEVEL
      );
    Get().cOverlay.cH264SecondInfo.dwColor = (dwB | (dwG << 8) | (dwR << 16));
    SW_TRACE_DEBUG("h264 second color[0x%08x][0x%02x,0x%02x,0x%02x]", 
		Get().cOverlay.cH264SecondInfo.dwColor, dwR, dwG, dwB);
    GetEnum("\\Overlay\\H264Second"
          , "DateTime"
          , &Get().cOverlay.cH264SecondInfo.fEnableTime
          , Get().cOverlay.cH264SecondInfo.fEnableTime
          , "0:不使能;1:使能"
          , "叠加时间"
          , ""
          , CUSTOM_LEVEL
      );

     GetString("\\Overlay\\H264Second"
          , "String"
          , Get().cOverlay.cH264SecondInfo.szInfo
          , Get().cOverlay.cH264SecondInfo.szInfo
          , sizeof(Get().cOverlay.cH264SecondInfo.szInfo)
          , ""
          , "叠加信息"
          , CUSTOM_LEVEL
      );
    if(!swpa_strcmp(Get().cOverlay.cH264SecondInfo.szInfo, "NULL"))
    {
      swpa_strcpy(Get().cOverlay.cH264SecondInfo.szInfo, "");
    }

    // todo.字符叠加计算字符总长度有问题，在设置时多加两个空格在尾部，避免因过长叠加显示不全。
    if( strlen(Get().cOverlay.cH264SecondInfo.szInfo) + 3 < sizeof(Get().cOverlay.cH264SecondInfo.szInfo) )
    {
        strcpy(Get().cOverlay.cH264SecondInfo.szInfo + strlen(Get().cOverlay.cH264SecondInfo.szInfo), "  ");
        SW_TRACE_NORMAL("<param>change overlay h264 len.%s", Get().cOverlay.cH264SecondInfo.szInfo);
    }
//end

      GetEnum("\\Overlay\\JPEG"
          , "Enable"
          , &Get().cOverlay.cJPEGInfo.fEnable
          , Get().cOverlay.cJPEGInfo.fEnable
          , "0:不使能;1:使能"
          , "JPEG字符叠加使能"
          , ""
          , CUSTOM_LEVEL
      );

    GetInt("\\Overlay\\JPEG"
          , "X"
          , &Get().cOverlay.cJPEGInfo.iX
          , Get().cOverlay.cJPEGInfo.iX
          , 0
          , 1920
          , "X坐标"
          , ""
          , CUSTOM_LEVEL
      );

    GetInt("\\Overlay\\JPEG"
          , "Y"
          , &Get().cOverlay.cJPEGInfo.iY
          , Get().cOverlay.cJPEGInfo.iY
          , 0
          , 1080
          , "Y坐标"
          , ""
          , CUSTOM_LEVEL
      );

    GetInt("\\Overlay\\JPEG"
          , "Size"
          , &Get().cOverlay.cJPEGInfo.iFontSize
          , Get().cOverlay.cJPEGInfo.iFontSize
          , 16
          , 128
          , "字体大小"
          , ""
          , CUSTOM_LEVEL
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
          , CUSTOM_LEVEL
      );
    GetInt("\\Overlay\\JPEG"
          , "G"
          , (INT *)&dwG
          , dwG
          , 0
          , 255
          , "字体颜色G分量"
          , ""
          , CUSTOM_LEVEL
      );
    GetInt("\\Overlay\\JPEG"
          , "B"
          , (INT *)&dwB
          , dwB
          , 0
          , 255
          , "字体颜色B分量"
          , ""
          , CUSTOM_LEVEL
      );
    Get().cOverlay.cJPEGInfo.iColor = (dwB | (dwG << 8) | (dwR << 16));
    SW_TRACE_DEBUG("jpeg color[0x%08x][0x%02x,0x%02x,0x%02x]", Get().cOverlay.cJPEGInfo.iColor, dwR, dwG, dwB);
    GetEnum("\\Overlay\\JPEG"
          , "DateTime"
          , &Get().cOverlay.cJPEGInfo.fEnableTime
          , Get().cOverlay.cJPEGInfo.fEnableTime
          , "0:不使能;1:使能"
          , "叠加时间"
          , ""
          , CUSTOM_LEVEL
      );

     GetString("\\Overlay\\JPEG"
          , "String"
          , Get().cOverlay.cJPEGInfo.szInfo
          , Get().cOverlay.cJPEGInfo.szInfo
          , sizeof(Get().cOverlay.cJPEGInfo.szInfo)
          , ""
          , "叠加信息"
          , CUSTOM_LEVEL
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


HRESULT SWLPRVenusCameraParameter::InitGB28181(VOID)
{
    return CSWLPRParameter::InitGB28181();
}


HRESULT SWLPRVenusCameraParameter::InitONVIF(VOID)
{

    SW_TRACE_NORMAL(" HRESULT SWLPRVenusCameraParameter::InitONVIF(VOID)");
	


	 GetEnum("\\ONVIF"
			 , "OnvifEnable"
			 , &Get().cOnvifParam.iOnvifEnable
			 , Get().cOnvifParam.iOnvifEnable
			 , "0:不使能;1:使能"
			 , "开关"
			 , ""
			 , CUSTOM_LEVEL);
	 GetEnum("\\ONVIF"
			 , "AuthenticateEnable"
			 , &Get().cOnvifParam.iAuthenticateEnable
			 , Get().cOnvifParam.iAuthenticateEnable
			 , "0:不使能;1:使能"
			 , "鉴权"
			 , "使能时需要用户密码才能进行操作"
			 , CUSTOM_LEVEL);

	 //为了统一，RTSP的鉴权和ONVIF的保持一致
	 Get().cRtspParam.iAuthenticateEnable = Get().cOnvifParam.iAuthenticateEnable;
	 Get().cRtspParam.iRTSPStreamNum = 2;
	 GetEnum("\\ONVIF"
			 , "AutoControlBitrateEnable"
			 , &Get().cRtspParam.iAutoControlBitrateEnable
			 , Get().cRtspParam.iAutoControlBitrateEnable
			 , "0:不使能;1:使能"
			 , "自动控制码率开关"
			 , "当网络出现拥塞会自动控制码率适应网络"
			 , CUSTOM_LEVEL);


    SW_TRACE_NORMAL(" HRESULT SWLPRVenusCameraParameter::InitONVIF(VOID) End");
}


