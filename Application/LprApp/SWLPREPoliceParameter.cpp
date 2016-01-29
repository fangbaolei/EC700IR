#include "SWFC.h"
#include "SWLPREPoliceParameter.h"

CSWLPREPoliceParameter::CSWLPREPoliceParameter()
{
	//工作模式
	Get().nWorkModeIndex = PRM_ELECTRONIC_POLICE;
	m_strWorkMode.Format("电子警察");
	//空间标定法进行测速
	Get().cTrackerCfgParam.cScaleSpeed.fEnable = 1;
	Get().cTrackerCfgParam.cScaleSpeed.nMethodsFlag = 1;
	Get().cTrackerCfgParam.cScaleSpeed.fltCameraHigh = 6.5f;
	Get().cTrackerCfgParam.cScaleSpeed.fltAdjustCoef = 1.050f;
	Get().cTrackerCfgParam.cScaleSpeed.rgfltTransMarix[0] = 165.472f;
	Get().cTrackerCfgParam.cScaleSpeed.rgfltTransMarix[1] = -38.447f;
	Get().cTrackerCfgParam.cScaleSpeed.rgfltTransMarix[2] = -4.968f;
	Get().cTrackerCfgParam.cScaleSpeed.rgfltTransMarix[3] = 606.269f;
	Get().cTrackerCfgParam.cScaleSpeed.rgfltTransMarix[4] = -2.743f;
	Get().cTrackerCfgParam.cScaleSpeed.rgfltTransMarix[5] = 1.813f;
	Get().cTrackerCfgParam.cScaleSpeed.rgfltTransMarix[6] = -177.545f;
	Get().cTrackerCfgParam.cScaleSpeed.rgfltTransMarix[7] = 1089.211f;
	Get().cTrackerCfgParam.cScaleSpeed.rgfltTransMarix[8] = 0.000f;
	Get().cTrackerCfgParam.cScaleSpeed.rgfltTransMarix[9] = -0.035f;
	Get().cTrackerCfgParam.cScaleSpeed.rgfltTransMarix[10] = -0.005f;
	//默认过滤不合格违章
	Get().cResultSenderParam.iFilterUnSurePeccancy = 1;
	//红灯加红阈值
	Get().cResultSenderParam.nRedPosBrightness = 55;
	//交通事件检测
	Get().cResultSenderParam.iCheckEventTime = 1;
	//电子警察，默认逆行监测开关为开
	Get().cTrackerCfgParam.nDetReverseRunEnable = 1;
	// T-1
	Get().cTrackerCfgParam.fEnableT1Model = 1;
	// 阴阳牌
	Get().cTrackerCfgParam.nEnableProcessBWPlate = 1;

	//电子警察视频检测区域默认值
	Get().cTrackerCfgParam.cVideoDet.rcVideoDetArea.left = 0;
  Get().cTrackerCfgParam.cVideoDet.rcVideoDetArea.top = 30;
  Get().cTrackerCfgParam.cVideoDet.rcVideoDetArea.right = 100;
  Get().cTrackerCfgParam.cVideoDet.rcVideoDetArea.bottom = 100; 
	//先初始化扫描区域，每种模式都有自己的扫描区域
	Get().cTrackerCfgParam.cDetectArea.DetectorAreaLeft = 2;
	Get().cTrackerCfgParam.cDetectArea.DetectorAreaTop = 60;
	Get().cTrackerCfgParam.cDetectArea.DetectorAreaRight = 98;
	Get().cTrackerCfgParam.cDetectArea.DetectorAreaBottom = 100;
	Get().cTrackerCfgParam.cDetectArea.nDetectorMinScaleNum = 6;
	Get().cTrackerCfgParam.cDetectArea.nDetectorMaxScaleNum = 10;
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

	// 抓拍位置初始值
	Get().cTrackerCfgParam.nCaptureOnePos = 80;
	Get().cTrackerCfgParam.nCaptureTwoPos = 60;
	Get().cTrackerCfgParam.nCaptureThreePos = 45;

	// 电警默认只输出一张大图
	Get().cResultSenderParam.iBestSnapshotOutput = 1;
	Get().cResultSenderParam.iLastSnapshotOutput = 0;
	//无车牌检测模式
	Get().cTrackerCfgParam.cVideoDet.iNoPlateDetMode = 1;
}

CSWLPREPoliceParameter::~CSWLPREPoliceParameter()
{
}

HRESULT CSWLPREPoliceParameter::InitTracker(VOID)
{
	if(S_OK == CSWLPRParameter::InitTracker())
	{
		GetEnum("Tracker\\Misc"
    	, "CoreProcessType"
    	, &Get().cTrackerCfgParam.nProcessType
    	, Get().cTrackerCfgParam.nProcessType
    	, "0:只区分大中小型车;1:隧道内区分非机动和行人;2:隧道外区分非机动车和行人"
    	, "处理类型"
    	, ""
    	, PROJECT_LEVEL
    );
    
    GetEnum("Tracker\\Misc"
    	, "CoreOutPutType"
    	, &Get().cTrackerCfgParam.nOutPutType
    	, Get().cTrackerCfgParam.nOutPutType
    	, "0:全类型输出;1:大中小型车输出为机动机"
    	, "输出类型 0:全类型输出;1:大中小型车输出为机动机"
    	, ""
    	, PROJECT_LEVEL
    );
    
		GetEnum("\\Tracker\\DoublePlate"
			, "DoublePlateTypeEnable"
			, &Get().cTrackerCfgParam.nDoublePlateTypeEnable
			, Get().cTrackerCfgParam.nDoublePlateTypeEnable
			, "0:黄牌车牌;1:摩托车牌;3:自动检测"
      , "双层牌类型开关"
      , ""
      , CUSTOM_LEVEL
    );

    GetEnum("\\Tracker\\DoublePlate"
    	, "DefaultType"
    	, &Get().cTrackerCfgParam.nDefaultDBType
    	, Get().cTrackerCfgParam.nDefaultDBType
    	, "0:摩托车牌;1:黄牌车牌"
      , "默认双层牌类型"
      , ""
      , CUSTOM_LEVEL
    );

	GetEnum("\\Tracker\\RoadInfo"
		, "OutputRoadTypeNameFlag"
		, &Get().cTrackerCfgParam.nOutputRoadTypeName
		, Get().cTrackerCfgParam.nOutputRoadTypeName
		, "0:不输出;1:输出"
		, "输出车道类型标识"
		, ""
		, CUSTOM_LEVEL
		);
	for (int i = 0; i < MAX_ROADLINE_NUM; i++)
    {
        char szSection[256];
        sprintf(szSection, "\\Tracker\\RoadInfo\\Road%02d", i);

        GetString(szSection
        	, "RoadTypeName"
        	, Get().cTrackerCfgParam.rgcRoadInfo[i].szRoadTypeName
        	, Get().cTrackerCfgParam.rgcRoadInfo[i].szRoadTypeName
        	, sizeof(Get().cTrackerCfgParam.rgcRoadInfo[i].szRoadTypeName)
        	, "车道类型名称"
        	, "若为NULL，则取该车道类型的中文名"
        	, CUSTOM_LEVEL
        );
    }

	GetEnum("\\Tracker\\VideoDet"
		, "NoPlateDetMode"
		, &Get().cTrackerCfgParam.cVideoDet.iNoPlateDetMode
		, Get().cTrackerCfgParam.cVideoDet.iNoPlateDetMode
		, "0:标准模式;1:模板模式"
		, "无牌车检测模式"
		, ""
		, CUSTOM_LEVEL
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
    
    GetInt("\\Tracker\\TrafficLight"
    	, "OnePos"
    	, &Get().cTrackerCfgParam.nCaptureOnePos
    	, Get().cTrackerCfgParam.nCaptureOnePos
    	, 0
    	, 100
    	, "抓拍第一张图的位置"
    	, ""
    	, CUSTOM_LEVEL
    );
    
    GetInt("\\Tracker\\TrafficLight"
    	, "TwoPos"
    	, &Get().cTrackerCfgParam.nCaptureTwoPos
    	, Get().cTrackerCfgParam.nCaptureTwoPos
    	, 0
    	, 100
    	, "抓拍第二张图的位置"
    	, ""
    	, CUSTOM_LEVEL
    );
    
    GetInt("\\Tracker\\TrafficLight"
    	, "ThreePos"
    	, &Get().cTrackerCfgParam.nCaptureThreePos
    	, Get().cTrackerCfgParam.nCaptureThreePos
    	, 0
    	, 100
    	, "抓拍第三张图的位置"
    	, ""
    	, CUSTOM_LEVEL
    );

    for (int i = 0; i < MAX_ROADLINE_NUM; i++)
    {
        char szSection[256];
        sprintf(szSection, "\\Tracker\\RoadInfo\\Road%02d", i);
        
		// todo. 集合的类型处理有问题，先以INT来代替。
		GetInt(szSection
			, "FilterRunType"
			, &Get().cTrackerCfgParam.rgcRoadInfo[i].iFilterRunType
			, Get().cTrackerCfgParam.rgcRoadInfo[i].iFilterRunType
			, 0
			, 100
			, "过滤行驶类型"
			, "1:直行;2:左转;4:右转;8:调头;16:逆行"
			, CUSTOM_LEVEL
			);
		/*
    		GetSet(szSection
    		, "FilterRunType"
    		, &Get().cTrackerCfgParam.rgcRoadInfo[i].iFilterRunType
    		, Get().cTrackerCfgParam.rgcRoadInfo[i].iFilterRunType
    		, "1:直行;2:左转;4:右转;8:调头;16:逆行"
    		, 255
    		, "过滤行驶类型"
    		, ""
    		, CUSTOM_LEVEL
        );
		*/
    }    
    
    GetEnum("\\Tracker\\TrafficLight", "FilterMoreReview"
    	, &Get().cTrackerCfgParam.cTrafficLight.nFilterMoreReview
    	, Get().cTrackerCfgParam.cTrafficLight.nFilterMoreReview
    	, "0:关闭;1:打开"
    	, "过滤大车车身多检开关"
    	, ""
    	, CUSTOM_LEVEL
    );
    
    GetInt("\\Tracker\\VideoDet"
    	, "EPOutPutDetectLine"
    	, &Get().cTrackerCfgParam.cVideoDet.iEPOutPutDetectLine
    	, Get().cTrackerCfgParam.cVideoDet.iEPOutPutDetectLine
    	, 0
    	, 100
    	, "电子警察检测与跟踪交接线"
    	, ""
    	, PROJECT_LEVEL
    );

    GetInt("\\Tracker\\VideoDet"
    	, "EPFGDetectLine"
    	, &Get().cTrackerCfgParam.cVideoDet.iEPFGDetectLine
    	, Get().cTrackerCfgParam.cVideoDet.iEPFGDetectLine
    	, 0
    	, 100
    	, "电警无牌车检测屏蔽区域(距离顶部百分比)"
    	, ""
    	, PROJECT_LEVEL
    );

		//视频检测需要校正坐标
		Get().cTrackerCfgParam.cVideoDet.iEPOutPutDetectLine = 100 - Get().cTrackerCfgParam.cVideoDet.iEPOutPutDetectLine;
		Get().cTrackerCfgParam.cVideoDet.iEPFGDetectLine -= Get().cTrackerCfgParam.cVideoDet.rcVideoDetArea.top;
		if(Get().cTrackerCfgParam.cVideoDet.iEPFGDetectLine < 0)
		{
			Get().cTrackerCfgParam.cVideoDet.iEPFGDetectLine = 0;
		}
		int width = Get().cTrackerCfgParam.cVideoDet.rcVideoDetArea.bottom - Get().cTrackerCfgParam.cVideoDet.rcVideoDetArea.top;
		if(width <= 0)
		{
			width = 100;
		}
		Get().cTrackerCfgParam.cVideoDet.iEPFGDetectLine *= 100.0f / width;
   
    LoadTrafficLightParam();
		return S_OK;
	}
	return E_FAIL;
}

VOID CSWLPREPoliceParameter::LoadTrafficLightParam(VOID)
{
    GetEnum("\\Tracker\\TrafficLight"
    	, "CheckType"
    	, &Get().cTrackerCfgParam.cTrafficLight.nCheckType
    	, Get().cTrackerCfgParam.cTrafficLight.nCheckType
    	, "0:视频检测;1:外接红绿灯"
    	, "红绿灯检测方式"
    	, ""
    	, CUSTOM_LEVEL
    );
    
    GetEnum("\\Tracker\\TrafficLight"
    	, "AutoScanLight"
    	, &Get().cTrackerCfgParam.cTrafficLight.nAutoScanLight
    	, Get().cTrackerCfgParam.cTrafficLight.nAutoScanLight
    	, "0:关闭;1:打开"
    	, "动态检测红绿灯位置开关"
    	, ""
    	, CUSTOM_LEVEL
    );
    
    GetEnum("Tracker\\TrafficLight"
    	, "DeleteLightEdge"
    	, &Get().cTrackerCfgParam.cTrafficLight.fDeleteLightEdge
    	, Get().cTrackerCfgParam.cTrafficLight.fDeleteLightEdge
    	, "0:关闭;1:打开"
    	, "去除灯框边缘开关"
    	, ""
    	, PROJECT_LEVEL
    );

    GetEnum("\\Tracker\\TrafficLight"
    	, "SceneCheckMode"
    	, &Get().cTrackerCfgParam.cTrafficLight.nSenceCheckMode
    	, Get().cTrackerCfgParam.cTrafficLight.nSenceCheckMode
    	, "0:自动检测;1:手动绑定"
    	, "场景检测模式"
    	, ""
    	, CUSTOM_LEVEL
    );
    
    GetInt("\\Tracker\\TrafficLight"
    	, "StopLinePos"
    	, &Get().cTrackerCfgParam.cTrafficLight.nStopLinePos
    	, Get().cTrackerCfgParam.cTrafficLight.nStopLinePos
    	, 0
    	, 100
    	, "停止线位置(直行)"
    	, ""
    	, CUSTOM_LEVEL
    );
    
    GetInt("\\Tracker\\TrafficLight"
    	, "LeftStopLinePos"
    	, &Get().cTrackerCfgParam.cTrafficLight.nLeftStopLinePos
    	, Get().cTrackerCfgParam.cTrafficLight.nLeftStopLinePos
    	, 0
    	, 100
    	, "停止线位置(左转)"
    	, ""
    	, CUSTOM_LEVEL
    );
    GetInt("\\Tracker\\Templet"
    	, "ScanSpeed"
    	, &Get().cTrackerCfgParam.cTrafficLight.nMatchScanSpeed
    	, Get().cTrackerCfgParam.cTrafficLight.nMatchScanSpeed
    	, 0
    	, 100
    	, "扫描速度(0:快, 1:慢)"
    	, ""
    	, PROJECT_LEVEL
    );
    GetInt("\\Tracker\\TrafficLight"
    	, "LightCount"
    	, &Get().cTrackerCfgParam.cTrafficLight.nLightCount
    	, Get().cTrackerCfgParam.cTrafficLight.nLightCount
    	, 0
    	, MAX_TRAFFICLIGHT_COUNT
    	, "灯组数量"
    	, ""
    	, CUSTOM_LEVEL
    );
    
    GetInt("\\Tracker\\TrafficLight"
    	, "ValidSceneStatus"
    	, &Get().cTrackerCfgParam.cTrafficLight.nValidSceneStatusCount
    	, Get().cTrackerCfgParam.cTrafficLight.nValidSceneStatusCount
    	, 1
    	, 100
    	, "有效场景状态计数"
    	, "相同状态要达到此值才输出"
    	, CUSTOM_LEVEL
    );
    
    GetEnum("\\Tracker\\TrafficLight"
    	, "OutputInLine"
    	, &Get().cTrackerCfgParam.cTrafficLight.nOutputInLine
    	, Get().cTrackerCfgParam.cTrafficLight.nOutputInLine
    	, "0:不输出;1:输出"
    	, "输出未过停止线的车辆"
    	, ""
    	, CUSTOM_LEVEL
    );
    
    GetInt("\\Tracker\\TrafficLight"
    	, "RedLightDelay"
    	, &Get().cTrackerCfgParam.cTrafficLight.nRedLightDelay
    	, Get().cTrackerCfgParam.cTrafficLight.nRedLightDelay
    	, 0
    	, 300
    	, "多色灯黄灯变红灯间隔(秒)"
    	, ""
    	, CUSTOM_LEVEL
    );
    
    GetEnum("\\Tracker\\TrafficLight"
    	, "SpecificLeft"
    	, &Get().cTrackerCfgParam.cTrafficLight.nSpecificLeft
    	, Get().cTrackerCfgParam.cTrafficLight.nSpecificLeft
    	, "0:否;1:是"
    	, "左转道相对"
    	, ""
    	, CUSTOM_LEVEL
    );
    
    GetEnum("\\Tracker\\TrafficLight"
    	, "RunTypeEnable"
    	, &Get().cTrackerCfgParam.cTrafficLight.nRunTypeEnable
    	, Get().cTrackerCfgParam.cTrafficLight.nRunTypeEnable
    	, "0:否;1:是"
    	, "行驶类型强制输出"
    	, ""
    	, CUSTOM_LEVEL
    );

	GetEnum("\\Tracker\\TrafficLight"
		, "FilterRushPeccancy"
		, &Get().cTrackerCfgParam.cTrafficLight.nFilterRushPeccancy
		, Get().cTrackerCfgParam.cTrafficLight.nFilterRushPeccancy
		, "0:不过滤;1:过滤"
		, "过滤掉第三张图为绿灯时的违章"
		, ""
		, CUSTOM_LEVEL
		);
    
    for (int i = 0; i < MAX_TRAFFICLIGHT_COUNT; i++)
    {
        char szLightPosKey[32];
        char szLightPosInfo[32];
        sprintf(szLightPosKey, "LightPos_%d", i + 1);
        sprintf(szLightPosInfo, "灯组信息_%d", i + 1);
        
        GetString("\\Tracker\\TrafficLight"
        	, szLightPosKey
        	, Get().cTrackerCfgParam.cTrafficLight.rgszLightPos[i]
        	, Get().cTrackerCfgParam.cTrafficLight.rgszLightPos[i]
        	, (64-1)
        	, szLightPosInfo
        	, ""
        	, CUSTOM_LEVEL
        );
    }
    
    GetInt("\\Tracker\\TrafficLight\\Scene"
    	, "SceneCount"
    	, &Get().cTrackerCfgParam.cTrafficLight.nSceneCount
    	, Get().cTrackerCfgParam.cTrafficLight.nSceneCount
    	, 0
    	, MAX_SCENE_COUNT
    	, "红绿灯场景数"
    	, ""
    	, CUSTOM_LEVEL
    );
    
    for (int i = 0; i < MAX_SCENE_COUNT; i++)
    {
        char szSceneKey[32];
        char szSceneInfo[32];
        sprintf(szSceneKey, "Scene_%d", i + 1);
        sprintf(szSceneInfo, "场景信息_%d", i + 1);
        
        GetString("\\Tracker\\TrafficLight\\Scene"
        	, szSceneKey
        	, Get().cTrackerCfgParam.cTrafficLight.rgszScene[i]
        	, Get().cTrackerCfgParam.cTrafficLight.rgszScene[i]
        	, (32-1)
        	, szSceneInfo
        	, ""
        	, CUSTOM_LEVEL
        );
    }

    GetString("Tracker\\TrafficLight"
    	, "IOConfig"
    	, Get().cTrackerCfgParam.cTrafficLight.szIOConfig
    	, Get().cTrackerCfgParam.cTrafficLight.szIOConfig
    	, sizeof(Get().cTrackerCfgParam.cTrafficLight.szIOConfig)
    	, "外接IO红绿灯参数"
    	, ""
    	, CUSTOM_LEVEL
    );
    
    GetEnum("Tracker\\TrafficLight"
    	, "EnhanceRedLight"
    	, &Get().cTrackerCfgParam.cTrafficLight.fEnhanceRedLight
    	, Get().cTrackerCfgParam.cTrafficLight.fEnhanceRedLight
    	, "0:不加红;1:加红"
    	, "红灯发白区域是否加红"
    	, ""
    	, CUSTOM_LEVEL
    );
    
    GetInt("Tracker\\TrafficLight"
    	, "TrafficLightThread"
    	, &Get().cTrackerCfgParam.cTrafficLight.nTrafficLightThread
    	, Get().cTrackerCfgParam.cTrafficLight.nTrafficLightThread
    	, 0
    	, 5
    	, "红绿灯灵敏度"
    	, ""
    	, CUSTOM_LEVEL
    );
    
    GetInt("Tracker\\TrafficLight"
    	, "TrafficLightBrightValue"
    	, &Get().cTrackerCfgParam.cTrafficLight.nTrafficLightBrightValue
    	, Get().cTrackerCfgParam.cTrafficLight.nTrafficLightBrightValue
    	, 0
    	, 5
    	, "红绿灯亮度等级"
    	, ""
    	, CUSTOM_LEVEL
    );
    
    GetInt("Tracker\\TrafficLight"
    	, "AutoScanThread"
    	, &Get().cTrackerCfgParam.cTrafficLight.nAutoScanThread
    	, Get().cTrackerCfgParam.cTrafficLight.nAutoScanThread
    	, 0
    	, 10
    	, "红绿灯动态检测阈值"
    	, ""
    	, CUSTOM_LEVEL
    );
    
    GetInt("Tracker\\TrafficLight"
    	, "SkipFrameNO"
    	, &Get().cTrackerCfgParam.cTrafficLight.nSkipFrameNO
    	, Get().cTrackerCfgParam.cTrafficLight.nSkipFrameNO
    	, 0
    	, 50
    	, "动态检测找灯间隔帧"
    	, ""
    	, CUSTOM_LEVEL
    );
    
    GetEnum("Tracker\\TrafficLight"
    	, "LightAdhesion"
    	, &Get().cTrackerCfgParam.cTrafficLight.fIsLightAdhesion
    	, Get().cTrackerCfgParam.cTrafficLight.fIsLightAdhesion
    	, "0:否;1:是"
    	, "红绿灯是否粘连在一起"
    	, ""
    	, CUSTOM_LEVEL
    );
    
    GetInt("Tracker\\TrafficLight"
    	, "HMThreld"
    	, &Get().cTrackerCfgParam.cTrafficLight.fltHMThreld
    	, Get().cTrackerCfgParam.cTrafficLight.fltHMThreld
    	, 0
    	, 100
    	, "白天阈值"
    	, ""
    	, PROJECT_LEVEL
    );
    
    GetInt("Tracker\\TrafficLight"
    	, "HMThreldNight"
    	, &Get().cTrackerCfgParam.cTrafficLight.fltHMThreldNight
    	, Get().cTrackerCfgParam.cTrafficLight.fltHMThreldNight
    	, 0
    	, 100
    	, "夜景阈值"
    	, ""
    	, PROJECT_LEVEL
    );
    
    GetInt("Tracker\\TrafficLight","HMDelay"
    	, &Get().cTrackerCfgParam.cTrafficLight.iHMDelay
    	, Get().cTrackerCfgParam.cTrafficLight.iHMDelay
    	, 1
    	, 100
    	, "白天延迟帧数"
    	, ""
    	, PROJECT_LEVEL
    );
    
    GetInt("Tracker\\TrafficLight"
    	, "HMDelayNight"
    	, &Get().cTrackerCfgParam.cTrafficLight.iHMDelayNight
    	, Get().cTrackerCfgParam.cTrafficLight.iHMDelayNight
    	, 1
    	, 100
    	, "夜景延迟帧数"
    	, ""
    	, PROJECT_LEVEL
    );
    
    GetInt("Tracker\\TrafficLight"
    	, "MaxMatchDiff"
    	, &Get().cTrackerCfgParam.cTrafficLight.iMaxMatchDiff
    	, Get().cTrackerCfgParam.cTrafficLight.iMaxMatchDiff
    	, 1
    	, 100
    	, "差异阈值"
    	, ""
    	, PROJECT_LEVEL
    );
    GetInt("Tracker\\TrafficLight"
    	, "RealForward"
    	, &Get().cTrackerCfgParam.cTrafficLight.iRealForward
    	, Get().cTrackerCfgParam.cTrafficLight.iRealForward
    	, 0
    	, 100
    	, "直行判断位置"
    	, ""
    	, CUSTOM_LEVEL
    );
    
    GetEnum("\\Tracker\\TrafficLight"
    	, "FilterNoPlatePeccancy"
    	, &Get().cTrackerCfgParam.cTrafficLight.fFilterNoPlatePeccancy
    	, Get().cTrackerCfgParam.cTrafficLight.fFilterNoPlatePeccancy
    	, "0:不过滤;1:过滤"
    	, "过滤无车牌违章结果"
    	, ""
    	, CUSTOM_LEVEL
    );

	// 目前不考虑支持抓拍。
	/*
	GetEnum("\\Tracker\\TrafficLight"
		, "EnableFlashLight"
		, &Get().cTrackerCfgParam.fEnableFlashLight
		, Get().cTrackerCfgParam.fEnableFlashLight
		, "0:关闭;1打开"
		, "抓拍补光灯模式开关"
		, ""
		, CUSTOM_LEVEL
		);

	GetInt("\\Tracker\\TrafficLight"
		, "FlashLightThreshold"
		, &Get().cTrackerCfgParam.nFlashLightThreshold
		, Get().cTrackerCfgParam.nFlashLightThreshold
		, 0
		, 255
		, "抓拍补光亮度阈值"
		, ""
		, CUSTOM_LEVEL
		);
		*/
}
	
HRESULT CSWLPREPoliceParameter::InitHvDsp(VOID)
{
	if(S_OK == CSWLPRParameter::InitHvDsp())
	{
		GetEnum("\\HvDsp\\Misc"
			, "FilterUnSurePeccancy"
			, &Get().cResultSenderParam.iFilterUnSurePeccancy
			, Get().cResultSenderParam.iFilterUnSurePeccancy
			, "0:不过滤;1:过滤"
			, "过滤不合格的违章记录"
			, ""
			, PROJECT_LEVEL
			);


		GetEnum("\\HvDsp\\VideoProc\\MainVideo"
			, "OutputCaptureImage"
			, &Get().cResultSenderParam.iOutputCaptureImage
			, Get().cResultSenderParam.iOutputCaptureImage
			, "0:只有违章车辆输出三张轨迹图;1:所有车辆都输出三张轨迹图"
			, "非违章车辆输出三张抓拍图"
			, ""
			, CUSTOM_LEVEL
			);


		
    GetEnum("\\Tracker\\TrafficLight"
    	, "OutputPeccancyType"
    	, &Get().cResultSenderParam.nOutputPeccancyType
    	, Get().cResultSenderParam.nOutputPeccancyType
    	, "0:按照优先级输出;1:输出所有违章类型"
    	, "输出违章类型方式"
    	, ""
    	, CUSTOM_LEVEL
    );
    
    GetString("\\Tracker\\TrafficLight"
    	, "OutputPeccancyPriority"
    	, Get().cResultSenderParam.szPeccancyPriority
    	, Get().cResultSenderParam.szPeccancyPriority
    	, sizeof(Get().cResultSenderParam.szPeccancyPriority)
    	, "违章类型优先级"
    	, "违章优先级排序"
    	, CUSTOM_LEVEL
    );

    GetInt("\\HvDsp\\Misc"
    	, "RedPosBrightness"
    	, &Get().cResultSenderParam.nRedPosBrightness
    	, Get().cResultSenderParam.nRedPosBrightness
    	, 0
    	, 255
    	, "发送红灯位置环境亮度阈值"
    	, ""
    	, PROJECT_LEVEL
    );
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CSWLPREPoliceParameter::InitScaleSpeed(VOID)
{
	GetEnum("\\Tracker\\ScaleSpeed"
		, "Enable"
		, &Get().cTrackerCfgParam.cScaleSpeed.fEnable
		, Get().cTrackerCfgParam.cScaleSpeed.fEnable
		, "0:关闭;1:打开"
		, "软件测速开关"
		, ""
		, CUSTOM_LEVEL
		);

	GetEnum("\\Tracker\\ScaleSpeed"
			, "MethodsFlag"
			, &Get().cTrackerCfgParam.cScaleSpeed.nMethodsFlag
			, Get().cTrackerCfgParam.cScaleSpeed.nMethodsFlag
			, "1:空间标定法"
			, "计算模式"
			, ""
			, CUSTOM_LEVEL
			);
	
	GetFloat("\\Tracker\\ScaleSpeed"
		, "AdjustCoef"
		, &Get().cTrackerCfgParam.cScaleSpeed.fltAdjustCoef
		, Get().cTrackerCfgParam.cScaleSpeed.fltAdjustCoef
		, 0.01f
		, 10.0f
		, "修正系数"
		, ""
		, CUSTOM_LEVEL
		);
		
	for (int i = 0; i < 11; i++)
	{
		char szTransMarixText[32];
		char szTransMarixKey[32];
		sprintf(szTransMarixText, "变换矩阵_%d", i);
		sprintf(szTransMarixKey, "TransMarix_%d", i);

		GetFloat("\\Tracker\\ScaleSpeed"
			, szTransMarixKey
			, &Get().cTrackerCfgParam.cScaleSpeed.rgfltTransMarix[i]
			, Get().cTrackerCfgParam.cScaleSpeed.rgfltTransMarix[i]
			, -3.40282347e+37F
			, 3.40282347e+37F
			, szTransMarixText
			, ""
			, CUSTOM_LEVEL
			);
	}	
	return S_OK;
}

HRESULT CSWLPREPoliceParameter::InitCamApp(VOID)
{
	if(S_OK == CSWLPRParameter::InitCamApp())
	{
		GetEnum("\\HvDsp\\Camera\\Ctrl"
    	, "DynamicTriggerEnable"
    	, &Get().cCamCfgParam.iDynamicTriggerEnable
    	, Get().cCamCfgParam.iDynamicTriggerEnable
      , "0:关闭;1:打开"
      , "视频触发抓拍相机"
      , "对创宇J系列相机及HV摄像机有效"
      , PROJECT_LEVEL
    );
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CSWLPREPoliceParameter::InitOuterCtrl(VOID)
{
	if(S_OK == CSWLPRParameter::InitOuterCtrl())
	{
		GetEnum("\\OuterCtrl[外总控]"
		, "RecogSnapImg"
		, &Get().cMatchParam.fCaptureRecong
		, Get().cMatchParam.fCaptureRecong
		, "0:关闭;1:打开"
		, "抓拍图片识别开关"
		, ""
		, PROJECT_LEVEL
		);

		CHAR szRecogArea[64] = {0};
		swpa_strcpy(szRecogArea, "[0,50,100,100],9,14");
		GetString("\\OuterCtrl[外总控]"
			, "RecogArea"
			, szRecogArea
			, szRecogArea
			, 64
			, "抓拍识别区域(%)[左,上,右,下],最小车牌，最大车牌"
			, "抓拍识别区域(%)[左,上,右,下],最小车牌，最大车牌"
			, PROJECT_LEVEL
			);
			Get().cTrackerCfgParam.cRecogSnapArea.fEnableDetAreaCtrl = FALSE;
			CSWString strTmp;
			for(int i = 0; i < sizeof(Get().cMatchParam.signal)/sizeof(SIGNAL_PARAM); i++)
			{
				strTmp.Format("\\OuterCtrl[外总控]\\Signal%02d", i);
				GetEnum((LPCSTR)strTmp
  		  	, "SignalType"
  		  	, (INT *)&Get().cMatchParam.signal[i].dwType
  		  	, Get().cMatchParam.signal[i].dwType
  		  	, "0:无;1:速度;2:触发"
  		  	, "信号类型"
  		  	, ""
  		  	, CUSTOM_LEVEL
  		  );
  		
				GetInt((LPCSTR)strTmp
					, "RoadID"
					, (INT *)&Get().cMatchParam.signal[i].dwRoadID
					, 0
					, 0
					, 0xFF
					, "车道号"
					, ""
					, CUSTOM_LEVEL
				);		
  		  
  		  GetEnum((LPCSTR)strTmp
					, "SignalID"
					, (INT *)&Get().cMatchParam.signal[i].dwSignalID
					, Get().cMatchParam.signal[i].dwSignalID
					, "0:0;1:1;2:2"
					, "同一车道的信号索引"
					, ""
					, PROJECT_LEVEL
				);
				
				GetEnum((LPCSTR)strTmp
					, "Trigger"
					, (INT *)&Get().cMatchParam.signal[i].fTrigger
					, Get().cMatchParam.signal[i].fTrigger
					, "0:不触发抓拍"
					, "此信号是否要触发抓拍"
					, ""
					, CUSTOM_LEVEL
				);
				
				GetEnum((LPCSTR)strTmp
					, "Recognize"
					, (INT *)&Get().cMatchParam.signal[i].fRecognize
					, Get().cMatchParam.signal[i].fRecognize
					, "0:不识别"
					, "图片是否要识别"
					, ""
					, PROJECT_LEVEL
				);
				
				GetEnum((LPCSTR)strTmp
					, "Condition"
					, (INT *)&Get().cMatchParam.signal[i].dwCondition
					, Get().cMatchParam.signal[i].dwCondition
					, "0:此信号可有可无,只要有车牌即可出牌;1:必须有此信号和车牌才能出结果;2:只要有此信号,就必定出牌"
					, "匹配条件"
					, ""
					, CUSTOM_LEVEL
				);
			}
			return S_OK;
	}
	return E_FAIL;
}

