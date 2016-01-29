/*
 * CSWLPRVenusEPoliceGateParameter.cpp
 *
 *  Created on: 2013年12月27日
 *      Author: qinjj
 */
#include "SWFC.h"
#include "SWLPRApplication.h"
#include "SWLPRVenusEPoliceParameter.h"


CSWLPRVenusEPoliceParameter::CSWLPRVenusEPoliceParameter()
{
	//工作模式
	Get().nWorkModeIndex = PRM_ELECTRONIC_POLICE;
	m_strWorkMode.Format("电子警察");
	Get().nCameraWorkMode = 6;		//电子警察相机工作模式
	Get().nMCFWUsecase = IMX178_WITHOUTCVBS;

}

CSWLPRVenusEPoliceParameter::~CSWLPRVenusEPoliceParameter()
{
}

// 恢复默认
void CSWLPRVenusEPoliceParameter::ResetParam(VOID)
{

	//先调用基类
	CSWLPRParameter::ResetParam();

	/*根据具体方案配置参数*/
	
	Get().nLogLevel = 1;
	//DSP参数默认值
	Get().cTrackerCfgParam.fEnableVote = TRUE;
  Get().cTrackerCfgParam.fAlpha5 = FALSE;
  Get().cTrackerCfgParam.fAlpha6 = FALSE;
  Get().cTrackerCfgParam.fAlpha7 = FALSE;
  Get().cTrackerCfgParam.fEnableRecgCarColor = FALSE;
  Get().cTrackerCfgParam.fEnableFlashLight = FALSE;
  Get().cTrackerCfgParam.fltMinConfForOutput = 0.7f;
  Get().cTrackerCfgParam.nPlateEnhanceThreshold = 40;
  Get().cTrackerCfgParam.fProcessPlate_BlackPlate_Enable = FALSE;
  Get().cTrackerCfgParam.nProcessPlate_BlackPlate_S = 5;
  Get().cTrackerCfgParam.nProcessPlate_BlackPlate_L = 85;
  Get().cTrackerCfgParam.nProcessPlate_BlackPlateThreshold_H0 = 100;
  Get().cTrackerCfgParam.nProcessPlate_BlackPlateThreshold_H1 = 200;
  Get().cTrackerCfgParam.nProcessPlate_LightBlue = 1;
  Get().cTrackerCfgParam.nPlateDetect_Green = 0;
  Get().cTrackerCfgParam.nNightThreshold = 55;
  Get().cTrackerCfgParam.nPlateLightCheckCount = 5;
  Get().cTrackerCfgParam.nMinPlateBrightness = 80;
  Get().cTrackerCfgParam.nMaxPlateBrightness = 120;
  Get().cTrackerCfgParam.nCtrlCpl = 0;
  Get().cTrackerCfgParam.nLightTypeCpl = ((int)LIGHT_TYPE_COUNT / 2);
  Get().cTrackerCfgParam.nEnablePlateEnhance = 1;
  Get().cTrackerCfgParam.nPlateResizeThreshold = 120;
  Get().cTrackerCfgParam.nEnableBigPlate = 0;
  Get().cTrackerCfgParam.nFirstPlatePos = 0;
  Get().cTrackerCfgParam.fltPlateDetect_StepAdj = 1.5f;
  Get().cTrackerCfgParam.nMiddleWidth = 70;
  Get().cTrackerCfgParam.nBottomWidth = 90;
  Get().cTrackerCfgParam.iEddyType = 0;
  Get().cTrackerCfgParam.fltOverLineSensitivity = 2.75f;
  Get().cTrackerCfgParam.nRoadLineNumber = 2;
  Get().cTrackerCfgParam.nOutputRoadTypeName = 0;
  Get().cTrackerCfgParam.nCaptureOnePos = 0;
  Get().cTrackerCfgParam.nCaptureTwoPos = 0;
  Get().cTrackerCfgParam.nCaptureThreePos = 0;
  Get().cTrackerCfgParam.cActionDetect.iDetectCarStopTime = 10;
  Get().cTrackerCfgParam.nDetReverseRunEnable = 1;
  Get().cTrackerCfgParam.nSpan = 5;
  Get().cTrackerCfgParam.iRoadNumberBegin = 0;
  Get().cTrackerCfgParam.iStartRoadNum = 0;
  Get().cTrackerCfgParam.nFlashLightThreshold = 25;
  Get().cTrackerCfgParam.fDoublePlateEnable = 1;
  Get().cTrackerCfgParam.nRecogAsteriskThreshold = 0;
  Get().cTrackerCfgParam.fUseEdgeMethod = FALSE;
  Get().cTrackerCfgParam.fEnableT1Model = FALSE;
  Get().cTrackerCfgParam.fEnableDefaultWJ = TRUE;
  Get().cTrackerCfgParam.nRecogGxPolice = 1;
  Get().cTrackerCfgParam.fEnableAlpha_5 = FALSE;
  Get().cTrackerCfgParam.nCarArrivedPos = 75;
  Get().cTrackerCfgParam.nCarArrivedPosNoPlate = 55;
  Get().cTrackerCfgParam.nCarArrivedPosYellow = 70;
  Get().cTrackerCfgParam.nProcessType = 0;
  Get().cTrackerCfgParam.nOutPutType = 0;
  Get().cTrackerCfgParam.nNightPlus = 0;
  Get().cTrackerCfgParam.nWalkManSpeed = 100;
  Get().cTrackerCfgParam.fltBikeSensitivity = 4.0f;
  Get().cTrackerCfgParam.nCarArrivedDelay = 0;
  Get().cTrackerCfgParam.nVoteFrameNum = 35;
  Get().cTrackerCfgParam.nMaxEqualFrameNumForVote = 15;
  Get().cTrackerCfgParam.nBlockTwinsTimeout = 180;
  Get().cTrackerCfgParam.nFirstConfidenceQuan = 0;
  Get().cTrackerCfgParam.nRemoveLowConfForVote = 40;
  Get().cTrackerCfgParam.nAverageConfidenceQuan = 0;
  Get().cTrackerCfgParam.fOutputCarSize = FALSE;
  swpa_memset(Get().cTrackerCfgParam.szDefaultWJChar, 0, 4);
  swpa_memcpy(Get().cTrackerCfgParam.szDefaultWJChar, "16", 2);
  Get().cTrackerCfgParam.nVoteCount = 3;
  Get().cTrackerCfgParam.fOnePlateMode = TRUE;
  Get().cTrackerCfgParam.nSegHeight = 18;
  Get().cTrackerCfgParam.nDoublePlateTypeEnable = 0;
  Get().cTrackerCfgParam.nDefaultDBType = 0;
  Get().cTrackerCfgParam.nSegWhitePlate = 0;
  Get().cTrackerCfgParam.nCarArriveTrig = 0;
  Get().cTrackerCfgParam.nTriggerPlateType = 0;
  Get().cTrackerCfgParam.nEnableProcessBWPlate = 0;
	Get().cTrackerCfgParam.iPlatform = 0;
	Get().cTrackerCfgParam.iSpeedLimit = 0;
	Get().cTrackerCfgParam.iCapMode = 0;
	Get().cTrackerCfgParam.iOutPutNoPlate = 0;
  Get().cTrackerCfgParam.iPrevTime = 5000;
  Get().cTrackerCfgParam.iPostTime = 0;
	Get().cTrackerCfgParam.fPreVote = TRUE;
  Get().cTrackerCfgParam.fEnableDetFace = FALSE;
  Get().cTrackerCfgParam.nMinFaceScale = 2;
  Get().cTrackerCfgParam.nMaxFaceScale = 10;
  Get().cTrackerCfgParam.iForceLightOffAtDay = 0;
	Get().cTrackerCfgParam.iForceLightOffAtNight = 0;
	Get().cTrackerCfgParam.iDisableFlashAtDay = 0;
	Get().cTrackerCfgParam.iForceLightThreshold = 100;

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
  Get().cTrackerCfgParam.cDetectArea.DetectorAreaRight = 100;
  Get().cTrackerCfgParam.cDetectArea.DetectorAreaTop = 0;
  Get().cTrackerCfgParam.cDetectArea.DetectorAreaBottom = 40;
  Get().cTrackerCfgParam.cDetectArea.nDetectorMinScaleNum = 6;
  Get().cTrackerCfgParam.cDetectArea.nDetectorMaxScaleNum = 12;

  //梯形扫描区域默认值
  Get().cTrackerCfgParam.cTrapArea.fEnableDetAreaCtrl = FALSE;
	Get().cTrackerCfgParam.cTrapArea.TopLeftX = 0;
	Get().cTrackerCfgParam.cTrapArea.TopLeftY = 0;
	Get().cTrackerCfgParam.cTrapArea.TopRightX = 100;
	Get().cTrackerCfgParam.cTrapArea.TopRightY = 100;
	Get().cTrackerCfgParam.cTrapArea.BottomLeftX = 0;
	Get().cTrackerCfgParam.cTrapArea.BottomLeftY = 0;
	Get().cTrackerCfgParam.cTrapArea.BottomRightX = 100;
	Get().cTrackerCfgParam.cTrapArea.BottomRightY = 100;
	Get().cTrackerCfgParam.cTrapArea.nDetectorMinScaleNum = 6;
	Get().cTrackerCfgParam.cTrapArea.nDetectorMaxScaleNum = 12;

	// 抓拍图识别参数
	Get().cTrackerCfgParam.cRecogSnapArea.fEnableDetAreaCtrl = FALSE;
  Get().cTrackerCfgParam.cRecogSnapArea.DetectorAreaLeft = 0;
  Get().cTrackerCfgParam.cRecogSnapArea.DetectorAreaRight = 100;
  Get().cTrackerCfgParam.cRecogSnapArea.DetectorAreaTop = 0;
  Get().cTrackerCfgParam.cRecogSnapArea.DetectorAreaBottom = 40;
  Get().cTrackerCfgParam.cRecogSnapArea.nDetectorMinScaleNum = 6;
  Get().cTrackerCfgParam.cRecogSnapArea.nDetectorMaxScaleNum = 12;

  //红绿灯
  Get().cTrackerCfgParam.cTrafficLight.nCheckType = 0;
  Get().cTrackerCfgParam.cTrafficLight.nAutoScanLight = 1;
  Get().cTrackerCfgParam.cTrafficLight.nSenceCheckMode = 0;
  Get().cTrackerCfgParam.cTrafficLight.nStopLinePos = 65;
  Get().cTrackerCfgParam.cTrafficLight.nLeftStopLinePos = 65;
  Get().cTrackerCfgParam.cTrafficLight.nMatchScanSpeed = 0;
  Get().cTrackerCfgParam.cTrafficLight.nLightCount = 0;
  Get().cTrackerCfgParam.cTrafficLight.nValidSceneStatusCount = 5;
  Get().cTrackerCfgParam.cTrafficLight.nOutputInLine = 1;
  Get().cTrackerCfgParam.cTrafficLight.nRedLightDelay = 0;
  Get().cTrackerCfgParam.cTrafficLight.nSpecificLeft = 0;
  Get().cTrackerCfgParam.cTrafficLight.nRunTypeEnable = 0;
  Get().cTrackerCfgParam.cTrafficLight.nSceneCount = 0;
  Get().cTrackerCfgParam.cTrafficLight.nTrafficLightThread = 3;
  Get().cTrackerCfgParam.cTrafficLight.nTrafficLightBrightValue = 3;
  Get().cTrackerCfgParam.cTrafficLight.nAutoScanThread = 10;
  Get().cTrackerCfgParam.cTrafficLight.nSkipFrameNO = 30;
  Get().cTrackerCfgParam.cTrafficLight.fIsLightAdhesion = FALSE;
  Get().cTrackerCfgParam.cTrafficLight.fDeleteLightEdge = FALSE;
  Get().cTrackerCfgParam.cTrafficLight.fFilterNoPlatePeccancy = FALSE;

  for (int i = 0; i < MAX_TRAFFICLIGHT_COUNT; i++)
  {
      swpa_strcpy(Get().cTrackerCfgParam.cTrafficLight.rgszLightPos[i], "(0,0,0,0),0,0,00");
  }
  for (int i = 0; i < MAX_SCENE_COUNT; i++)
  {
      swpa_strcpy(Get().cTrackerCfgParam.cTrafficLight.rgszScene[i], "(0,0,0,0),0");
  }

  Get().cTrackerCfgParam.cTrafficLight.nFilterRushPeccancy = 0;
  Get().cTrackerCfgParam.cTrafficLight.nFilterMoreReview = 0;
  Get().cTrackerCfgParam.cTrafficLight.fltHMThreld = 48;       //白天阈值
  Get().cTrackerCfgParam.cTrafficLight.fltHMThreldNight = 48;  //夜景阈值
  //模板匹配直方图延迟更新帧数
  Get().cTrackerCfgParam.cTrafficLight.iHMDelay = 7;             //白天延迟帧数
  Get().cTrackerCfgParam.cTrafficLight.iHMDelayNight = 7;        //夜景延迟帧数
  Get().cTrackerCfgParam.cTrafficLight.iMaxMatchDiff = 18;
  Get().cTrackerCfgParam.cTrafficLight.iRealForward = 40;
  strcpy(Get().cTrackerCfgParam.cTrafficLight.szIOConfig, "00,00,00,00,00,00,00,00");
  Get().cTrackerCfgParam.cTrafficLight.fEnhanceRedLight = TRUE;

  //软件测速参数
  Get().cTrackerCfgParam.cScaleSpeed.fEnable = FALSE;
  Get().cTrackerCfgParam.cScaleSpeed.nMethodsFlag = 0;
  Get().cTrackerCfgParam.cScaleSpeed.fltLeftDistance = 25.0f;
  Get().cTrackerCfgParam.cScaleSpeed.fltFullDistance = 75.0f;
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

  //视频检测参数
  Get().cTrackerCfgParam.cVideoDet.nVideoDetMode = USE_VIDEODET;
  Get().cTrackerCfgParam.cVideoDet.nDayBackgroudThreshold = 20;
  Get().cTrackerCfgParam.cVideoDet.nNightBackgroudThreshold = 50;
  Get().cTrackerCfgParam.cVideoDet.nVoteFrameCount = 30;
  Get().cTrackerCfgParam.cVideoDet.nCheckAcross = 0;
  Get().cTrackerCfgParam.cVideoDet.nShadowDetMode = 0;
  Get().cTrackerCfgParam.cVideoDet.nShadowDetEffect = 0;
  Get().cTrackerCfgParam.cVideoDet.nDiffType = 0;
  Get().cTrackerCfgParam.cVideoDet.nMedFilter = 0;
  Get().cTrackerCfgParam.cVideoDet.nMedFilterUpLine = 15;
  Get().cTrackerCfgParam.cVideoDet.nMedFilterDownLine = 100;
  Get().cTrackerCfgParam.cVideoDet.rcVideoDetArea.left = 0;
  Get().cTrackerCfgParam.cVideoDet.rcVideoDetArea.top = 0;
  Get().cTrackerCfgParam.cVideoDet.rcVideoDetArea.right = 100;
  Get().cTrackerCfgParam.cVideoDet.rcVideoDetArea.bottom = 100;
  Get().cTrackerCfgParam.cVideoDet.nBigCarDelayFrameNo = 32;
	Get().cTrackerCfgParam.cVideoDet.iEPOutPutDetectLine = 63;         //电警检测到跟踪模块交接线
	Get().cTrackerCfgParam.cVideoDet.iEPFGDetectLine = 45;             //无牌车检测屏蔽区域(距离顶部百分比)
	Get().cTrackerCfgParam.cVideoDet.iNoPlateDetMode = 0;
	Get().cTrackerCfgParam.cVideoDet.iEPUseCarTailModel = 0;
	Get().cTrackerCfgParam.cVideoDet.iEPNoPlateScoreThreld = 300;

	// 默认红灯加红方式
	Get().cTrackerCfgParam.cTrafficLight.fEnhanceRedLight = FALSE;

	//主动连接默认值
	Get().cResultSenderParam.cAutoLinkParam.iAutoLinkPort = 6665;

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
	Get().cTrackerCfgParam.nCaptureOnePos = 85;
	Get().cTrackerCfgParam.nCaptureTwoPos = 65;
	Get().cTrackerCfgParam.nCaptureThreePos = 45;

	Get().cTrackerCfgParam.cTrafficLight.nFilterRushPeccancy = 1;

	// 电警默认只输出一张大图
	Get().cResultSenderParam.iBestSnapshotOutput = 1;
	Get().cResultSenderParam.iLastSnapshotOutput = 0;
	//无车牌检测模式
	Get().cTrackerCfgParam.cVideoDet.iNoPlateDetMode = 1;
	Get().cDevParam[1].iCommType = 1;	//金星只有一个串口COM1,只支持485

	Get().cCamAppParam.iResolution = 0;	//默认为1080P

	// 金星无频闪补光灯
	Get().cTrackerCfgParam.iUsedLight = 0;

	//金星扫描区域默认打开
	Get().cTrackerCfgParam.cTrapArea.fEnableDetAreaCtrl = TRUE;

	//金星RGB分量配置范围为36~255,默认值不能为0
	Get().cCamAppParam.iGainR = 36;
	Get().cCamAppParam.iGainG = 36;
	Get().cCamAppParam.iGainB = 36;

	Get().cCamAppParam.iAGCEnable = 1;
	Get().cCamAppParam.iAGCTh = 100;
	Get().cCamAppParam.iAGCShutterLOri = 100;
	Get().cCamAppParam.iAGCShutterHOri = 5500;
	Get().cCamAppParam.iAGCGainLOri = 0;
	Get().cCamAppParam.iAGCGainHOri = 200;
	Get().cCamAppParam.iAGCScene = 2;

	Get().cCamCfgParam.iDynamicCfgEnable = 1;
	Get().cCamCfgParam.iMaxAGCLimit = 150;
	Get().cCamCfgParam.iMinAGCLimit = 50;

	// 默认的降噪。
	Get().cCamAppParam.iSNFTNFMode = 3;
	Get().cCamAppParam.iTNFSNFValue = 1;

	// 默认的对比度
	Get().cCamAppParam.iContrastValue = 19;


	// 色阶默认为1
	Get().cCamAppParam.iColorGradation = 1;

	Get().cTrackerCfgParam.nLightTypeCpl = 13;

	Get().cCamCfgParam.iNightShutterThreshold = 4000;
	Get().cCamCfgParam.iNightAvgYThreshold = 80;

	// 路口名称方向
	strcpy(Get().cResultSenderParam.szStreetName, "NULL");
	strcpy(Get().cResultSenderParam.szStreetDirection, "NULL");
}

HRESULT CSWLPRVenusEPoliceParameter::Initialize(CSWString strFilePath)
{
	// 恢复默认
	ResetParam();
    // 先基类的初始化
    if (S_OK == CSWParameter<ModuleParams>::Initialize(strFilePath)
		&& S_OK == InitSystem()
		&& S_OK == InitTracker()
		&& S_OK == InitHvDsp()
        && S_OK == InitOuterCtrl()
        && S_OK == InitIPTCtrl()
        && S_OK == InitCamApp()
        && S_OK == InitScaleSpeed()
        && S_OK == InitCamera()
        && S_OK == InitCharacter() 
        && S_OK == InitGB28181() 
        && S_OK == InitAutoReboot()
		&& S_OK == InitONVIF())
    {
        return S_OK ;
    }

    SW_TRACE_DEBUG("<CSWLPRVenusEPoliceParameter> Initialize failed.\n");
    return E_FAIL ;
}

HRESULT CSWLPRVenusEPoliceParameter::InitSystem(VOID)
{
    if (S_OK == CSWLPRParameter::InitSystem())
    {
        return S_OK ;
    }
    return E_FAIL ;

}

// 屏蔽检测参数
void CSWLPRVenusEPoliceParameter::DeleteTrackerParam(VOID)
{
	// 新电警不需要设置扫描区域，屏蔽。

    GetEnum("Tracker\\DetAreaCtrl\\Normal"
    	,"EnableTrap"
    	, &Get().cTrackerCfgParam.cTrapArea.fEnableDetAreaCtrl
    	, Get().cTrackerCfgParam.cTrapArea.fEnableDetAreaCtrl
    	, "0:关闭;1:打开"
    	, "梯形扫描区域控制开关"
    	, ""
    	, DELETE_LEVEL
    );

    GetInt("Tracker\\DetAreaCtrl\\Normal"
    	, "TopLeftX"
    	, &Get().cTrackerCfgParam.cTrapArea.TopLeftX
    	, Get().cTrackerCfgParam.cTrapArea.TopLeftX
    	, 0
    	, 100
    	, "扫描左上区域坐标X"
    	, ""
    	, DELETE_LEVEL
    );

    GetInt("Tracker\\DetAreaCtrl\\Normal"
    	, "TopLeftY"
    	, &Get().cTrackerCfgParam.cTrapArea.TopLeftY
    	, Get().cTrackerCfgParam.cTrapArea.TopLeftY
    	, 0
    	, 100
    	, "扫描左上区域坐标Y"
    	, ""
    	, DELETE_LEVEL
    );

    GetInt("Tracker\\DetAreaCtrl\\Normal"
    	, "TopRightX"
    	, &Get().cTrackerCfgParam.cTrapArea.TopRightX
    	, Get().cTrackerCfgParam.cTrapArea.TopRightX
    	, 0
    	, 100
    	, "扫描右上区域坐标X"
    	, ""
    	, DELETE_LEVEL
    );

    GetInt("Tracker\\DetAreaCtrl\\Normal"
    	, "TopRightY"
    	, &Get().cTrackerCfgParam.cTrapArea.TopRightY
    	, Get().cTrackerCfgParam.cTrapArea.TopRightY
    	, 0
    	, 100
    	, "扫描右上区域坐标Y"
    	, ""
    	, DELETE_LEVEL
    );

    GetInt("Tracker\\DetAreaCtrl\\Normal","BottomLeftX"
    	, &Get().cTrackerCfgParam.cTrapArea.BottomLeftX
    	, Get().cTrackerCfgParam.cTrapArea.BottomLeftX
    	, 0
    	, 100
    	, "扫描左下区域坐标X"
    	, ""
    	, DELETE_LEVEL
    );

    GetInt("Tracker\\DetAreaCtrl\\Normal"
    	, "BottomLeftY"
    	, &Get().cTrackerCfgParam.cTrapArea.BottomLeftY
    	, Get().cTrackerCfgParam.cTrapArea.BottomLeftY
    	, 0
    	, 100
    	, "扫描左下区域坐标Y"
    	, ""
    	, DELETE_LEVEL
    );

    GetInt("Tracker\\DetAreaCtrl\\Normal"
    	, "BottomRightX"
    	, &Get().cTrackerCfgParam.cTrapArea.BottomRightX
    	, Get().cTrackerCfgParam.cTrapArea.BottomRightX
    	, 0
    	, 100
    	, "扫描右下区域坐标X"
    	, ""
    	, DELETE_LEVEL
    );

    GetInt("Tracker\\DetAreaCtrl\\Normal"
    	, "BottomRightY"
    	, &Get().cTrackerCfgParam.cTrapArea.BottomRightY
    	, Get().cTrackerCfgParam.cTrapArea.BottomRightY
    	, 0
    	, 100
    	, "扫描右下区域坐标Y"
    	, ""
    	, DELETE_LEVEL
    );

    Get().cTrackerCfgParam.cVideoDet.nVideoDetMode = 2;
    GetEnum("\\Tracker\\VideoDet"
    	, "VideoDetMode"
    	, &Get().cTrackerCfgParam.cVideoDet.nVideoDetMode
    	, Get().cTrackerCfgParam.cVideoDet.nVideoDetMode
    	, "0:不使用视频检测;1:只使用背景检测;2:使用视频检测"
    	, "视频检测模式"
    	, ""
    	, DELETE_LEVEL
    );

    GetInt("\\Tracker\\VideoDet"
        	, "DayBackgroudThreshold"
        	, &Get().cTrackerCfgParam.cVideoDet.nDayBackgroudThreshold
        	, Get().cTrackerCfgParam.cVideoDet.nDayBackgroudThreshold
        	, 0
        	, 255
        	, "白天背景检测阈值"
        	, ""
        	, DELETE_LEVEL
        );

        GetInt("\\Tracker\\VideoDet"
        	, "nBackgroudThreshold"
        	, &Get().cTrackerCfgParam.cVideoDet.nNightBackgroudThreshold
        	, Get().cTrackerCfgParam.cVideoDet.nNightBackgroudThreshold
        	, 0
        	, 255
        	, "晚上背景检测阈值"
        	, ""
        	, DELETE_LEVEL
        );

        GetInt("\\Tracker\\VideoDet"
        	, "VoteFrameCount"
        	, &Get().cTrackerCfgParam.cVideoDet.nVoteFrameCount
        	, Get().cTrackerCfgParam.cVideoDet.nVoteFrameCount
        	, 15
        	, 100
        	, "投票有效帧数，当帧数大于此值就投票"
        	, ""
        	, DELETE_LEVEL
        );

        GetEnum("\\Tracker\\VideoDet"
        	, "CheckAcross"
        	, &Get().cTrackerCfgParam.cVideoDet.nCheckAcross
        	, Get().cTrackerCfgParam.cVideoDet.nCheckAcross
        	, "0:否;1:是"
        	, "是否检测横向行驶的物体"
        	, ""
        	, DELETE_LEVEL
        );

        GetEnum("\\Tracker\\VideoDet"
        	, "ShadowDetMode"
        	, &Get().cTrackerCfgParam.cVideoDet.nShadowDetMode
        	, Get().cTrackerCfgParam.cVideoDet.nShadowDetMode
        	, "0:不使用阴影检测;1:使用阴影检测"
        	, "阴影检测模式"
        	, ""
        	, DELETE_LEVEL
        );

        GetInt("\\Tracker\\VideoDet"
        	, "ShadowDetEffect"
        	, &Get().cTrackerCfgParam.cVideoDet.nShadowDetEffect
        	, Get().cTrackerCfgParam.cVideoDet.nShadowDetEffect
        	, 0
        	, 100
        	, "阴影检测强弱程度值"
        	, "0:使用缺省值(不精确), 1:阴影检测强弱程度最小, 100:阴影检测强弱程度最大"
        	, DELETE_LEVEL
        );

        GetEnum("\\Tracker\\VideoDet"
        	, "DiffType"
        	, &Get().cTrackerCfgParam.cVideoDet.nDiffType
        	, Get().cTrackerCfgParam.cVideoDet.nDiffType
        	, "0:帧差提取;1:帧差后sobel前景"
        	, "前景提取方式"
        	, "值为0:帧差提取 值为1:帧差后sobel前景，针对高速路卡口两车道车身多检"
        	, DELETE_LEVEL
        );

        GetEnum("\\Tracker\\VideoDet"
        	, "MedFilter"
        	, &Get().cTrackerCfgParam.cVideoDet.nMedFilter
        	, Get().cTrackerCfgParam.cVideoDet.nMedFilter
        	, "0:关闭;1:打开"
        	, "抑制抖动开关"
        	, ""
        	, DELETE_LEVEL
        );

        GetInt("\\Tracker\\VideoDet"
        	, "MedFilterUpLine"
        	, &Get().cTrackerCfgParam.cVideoDet.nMedFilterUpLine
        	, Get().cTrackerCfgParam.cVideoDet.nMedFilterUpLine
        	, 0
        	, 100
        	, "抑制抖动作用区域上区域"
        	, "值为0:屏幕最上方 值为100:屏幕最下方"
        	, DELETE_LEVEL
        );

        GetInt("\\Tracker\\VideoDet"
        	, "MedFilterDownLine"
        	, &Get().cTrackerCfgParam.cVideoDet.nMedFilterDownLine
        	, Get().cTrackerCfgParam.cVideoDet.nMedFilterDownLine
        	, 0
        	, 100
        	, "抑制抖动作用区域下区域"
        	, "值为0:屏幕最上方 值为100:屏幕最下方"
        	, DELETE_LEVEL
        );

        GetInt("Tracker\\Misc"
			, "CarArrivedPos"
			, &Get().cTrackerCfgParam.nCarArrivedPos
			, Get().cTrackerCfgParam.nCarArrivedPos
			, 0
			, 100
			, "车辆到达位置(除黄牌外所有牌)"
			, ""
			, DELETE_LEVEL
		);

		GetInt("Tracker\\Misc"
			, "CarArrivedPosNoPlate"
			, &Get().cTrackerCfgParam.nCarArrivedPosNoPlate
			, Get().cTrackerCfgParam.nCarArrivedPosNoPlate
			, 0
			, 100
			, "无牌车到达位置"
			, ""
			, DELETE_LEVEL
		);

		GetInt("Tracker\\Misc"
			, "CarArrivedPosYellow"
			, &Get().cTrackerCfgParam.nCarArrivedPosYellow
			, Get().cTrackerCfgParam.nCarArrivedPosYellow
			, 0
			, 100
			, "车辆到达位置(黄牌)"
			, ""
			, DELETE_LEVEL
		);

	    GetInt("Tracker\\TrackInfo"
	    	, "VoteFrameNum"
	    	, &Get().cTrackerCfgParam.nVoteFrameNum
	    	, Get().cTrackerCfgParam.nVoteFrameNum
	    	, 4
	    	, 1000
	    	, "投票的结果数"
	    	, ""
	    	, DELETE_LEVEL
	    );

	    GetInt("Tracker\\TrackInfo"
	    	, "MaxEqualFrameNumForVote"
	    	, &Get().cTrackerCfgParam.nMaxEqualFrameNumForVote
	    	, Get().cTrackerCfgParam.nMaxEqualFrameNumForVote
	    	, 4, 1000
	    	, "连续相同结果出牌条件"
	    	, ""
	    	, DELETE_LEVEL
	    );

	    GetInt("\\Tracker\\ProcessPlate\\BlackPlate"
	    	, "NightThreshold"
	    	,	&Get().cTrackerCfgParam.nNightThreshold
	    	, Get().cTrackerCfgParam.nNightThreshold
	    	, 0
	    	, 240
	    	, "夜晚模式亮度上限"
	    	, ""
	    	, DELETE_LEVEL
	    );


	    GetInt("\\Tracker\\VideoDet\\AreaCtrl"
	    	, "Left"
	    	, &Get().cTrackerCfgParam.cVideoDet.rcVideoDetArea.left
	    	, Get().cTrackerCfgParam.cVideoDet.rcVideoDetArea.left
	    	, 0
	    	, 100
	    	, "视频检测左区域"
	    	, ""
	    	, DELETE_LEVEL
	    );

	    GetInt("\\Tracker\\VideoDet\\AreaCtrl"
	    	, "Right"
	    	, &Get().cTrackerCfgParam.cVideoDet.rcVideoDetArea.right
	    	, Get().cTrackerCfgParam.cVideoDet.rcVideoDetArea.right
	    	, 0
	    	, 100
	    	, "视频检测右区域"
	    	, ""
	    	, DELETE_LEVEL
	    );

	    GetInt("\\Tracker\\VideoDet\\AreaCtrl"
	    	, "Top"
	    	, &Get().cTrackerCfgParam.cVideoDet.rcVideoDetArea.top
	    	, Get().cTrackerCfgParam.cVideoDet.rcVideoDetArea.top
	    	, 0
	    	, 100
	    	, "视频检测上区域"
	    	, ""
	    	, DELETE_LEVEL
	    );

	    GetInt("\\Tracker\\VideoDet\\AreaCtrl"
	    	, "Bottom"
	    	, &Get().cTrackerCfgParam.cVideoDet.rcVideoDetArea.bottom
	    	, Get().cTrackerCfgParam.cVideoDet.rcVideoDetArea.bottom
	    	, 0
	    	, 100
	    	, "视频检测下区域"
	    	, ""
	    	, DELETE_LEVEL
	    );

}

HRESULT CSWLPRVenusEPoliceParameter::InitTracker(VOID)
{
	if(S_OK == CSWLPRParameter::InitTracker())
	{

		// 新电警不需要设置扫描区域，屏蔽。
		DeleteTrackerParam();
		Get().cTrackerCfgParam.cTrapArea.fEnableDetAreaCtrl = 1;




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

		GetEnum("Tracker\\Misc"
	        , "EnableAlphaRecog"
	        , &Get().cTrackerCfgParam.fEnableAlpha_5
	        , 1 //default value
	        , "0:关闭;1:打开"
	        , "黄牌字母识别开关"
	        , ""
	        , PROJECT_LEVEL
	        );

		GetEnum("\\Tracker\\ProcessPlate"
	        , "EnableDBGreenSegment"
	        , &Get().cTrackerCfgParam.nPlateDetect_Green
	        , Get().cTrackerCfgParam.nPlateDetect_Green
	        , "0:关闭;1:打开"
	        , "绿牌识别开关"
	        , ""
	        , PROJECT_LEVEL
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

	        // todo.
	        //处理帧场的差异。
	        Get().cTrackerCfgParam.rgcRoadInfo[i].ptTop.y /= 2;
	        Get().cTrackerCfgParam.rgcRoadInfo[i].ptBottom.y /= 2;
	    }




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

	    GetInt("\\Tracker\\ProcessPlate\\BlackPlate"
	    	, "S"
	    	, &Get().cTrackerCfgParam.nProcessPlate_BlackPlate_S
	    	, Get().cTrackerCfgParam.nProcessPlate_BlackPlate_S
	    	, 0
	    	, 240
	    	, "黑牌饱和度上限"
	    	, ""
	    	, PROJECT_LEVEL
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

	    // 梯形视频检测区域
		// 根据车道线和第三条抓拍线算出默认值。
	    Get().cTrackerCfgParam.cTrapArea.TopLeftX = 27;
	    Get().cTrackerCfgParam.cTrapArea.TopLeftY = 24;
	    Get().cTrackerCfgParam.cTrapArea.TopRightX = 67;
	    Get().cTrackerCfgParam.cTrapArea.TopRightY = 24;
	    Get().cTrackerCfgParam.cTrapArea.BottomLeftX = 7;
	    Get().cTrackerCfgParam.cTrapArea.BottomLeftY = 100;
	    Get().cTrackerCfgParam.cTrapArea.BottomRightX = 90;
	    Get().cTrackerCfgParam.cTrapArea.BottomRightY = 100;

		GetInt("\\Tracker\\VideoDet\\AreaCtrl"
			, "TopLeftX"
			, &Get().cTrackerCfgParam.cTrapArea.TopLeftX
			, Get().cTrackerCfgParam.cTrapArea.TopLeftX
			, 0
			, 100
			, "左上区域坐标X"
			, ""
			, CUSTOM_LEVEL
		);

		GetInt("\\Tracker\\VideoDet\\AreaCtrl"
			, "TopLeftY"
			, &Get().cTrackerCfgParam.cTrapArea.TopLeftY
			, Get().cTrackerCfgParam.cTrapArea.TopLeftY
			, 0
			, 100
			, "左上区域坐标Y"
			, ""
			, CUSTOM_LEVEL
		);

		GetInt("\\Tracker\\VideoDet\\AreaCtrl"
			, "TopRightX"
			, &Get().cTrackerCfgParam.cTrapArea.TopRightX
			, Get().cTrackerCfgParam.cTrapArea.TopRightX
			, 0
			, 100
			, "右上区域坐标X"
			, ""
			, CUSTOM_LEVEL
		);

		GetInt("\\Tracker\\VideoDet\\AreaCtrl"
			, "TopRightY"
			, &Get().cTrackerCfgParam.cTrapArea.TopRightY
			, Get().cTrackerCfgParam.cTrapArea.TopRightY
			, 0
			, 100
			, "右上区域坐标Y"
			, ""
			, CUSTOM_LEVEL
		);

		GetInt("\\Tracker\\VideoDet\\AreaCtrl","BottomLeftX"
			, &Get().cTrackerCfgParam.cTrapArea.BottomLeftX
			, Get().cTrackerCfgParam.cTrapArea.BottomLeftX
			, 0
			, 100
			, "左下区域坐标X"
			, ""
			, CUSTOM_LEVEL
		);

		GetInt("\\Tracker\\VideoDet\\AreaCtrl"
			, "BottomLeftY"
			, &Get().cTrackerCfgParam.cTrapArea.BottomLeftY
			, Get().cTrackerCfgParam.cTrapArea.BottomLeftY
			, 0
			, 100
			, "左下区域坐标Y"
			, ""
			, CUSTOM_LEVEL
		);

		GetInt("\\Tracker\\VideoDet\\AreaCtrl"
			, "BottomRightX"
			, &Get().cTrackerCfgParam.cTrapArea.BottomRightX
			, Get().cTrackerCfgParam.cTrapArea.BottomRightX
			, 0
			, 100
			, "右下区域坐标X"
			, ""
			, CUSTOM_LEVEL
		);

		GetInt("\\Tracker\\VideoDet\\AreaCtrl"
			, "BottomRightY"
			, &Get().cTrackerCfgParam.cTrapArea.BottomRightY
			, Get().cTrackerCfgParam.cTrapArea.BottomRightY
			, 0
			, 100
			, "右下区域坐标Y"
			, ""
			, CUSTOM_LEVEL
		);

		// 根据梯形区域算出矩形区域。
		Get().cTrackerCfgParam.cVideoDet.rcVideoDetArea.left =
			    Get().cTrackerCfgParam.cTrapArea.TopLeftX > Get().cTrackerCfgParam.cTrapArea.BottomLeftX ?
			    		Get().cTrackerCfgParam.cTrapArea.BottomLeftX : Get().cTrackerCfgParam.cTrapArea.TopLeftX;
		Get().cTrackerCfgParam.cVideoDet.rcVideoDetArea.right =
				Get().cTrackerCfgParam.cTrapArea.TopRightX < Get().cTrackerCfgParam.cTrapArea.BottomRightX ?
						Get().cTrackerCfgParam.cTrapArea.BottomRightX: Get().cTrackerCfgParam.cTrapArea.TopRightX;
		Get().cTrackerCfgParam.cVideoDet.rcVideoDetArea.top =
				Get().cTrackerCfgParam.cTrapArea.TopLeftY > Get().cTrackerCfgParam.cTrapArea.TopRightY ?
						Get().cTrackerCfgParam.cTrapArea.TopRightY   : Get().cTrackerCfgParam.cTrapArea.TopLeftY;
		Get().cTrackerCfgParam.cVideoDet.rcVideoDetArea.bottom =
				Get().cTrackerCfgParam.cTrapArea.BottomLeftY > Get().cTrackerCfgParam.cTrapArea.BottomRightY ?
						Get().cTrackerCfgParam.cTrapArea.BottomLeftY : Get().cTrackerCfgParam.cTrapArea.BottomRightY;


		return S_OK;
	}
	return E_FAIL;
}

VOID CSWLPRVenusEPoliceParameter::LoadTrafficLightParam(VOID)
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

    // todo.
    // 现机制不出停止线不出结果。
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
		, PROJECT_LEVEL
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

    // 用第三条抓拍线做为直行判断线。
    /*
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
    */
    
    GetEnum("\\Tracker\\TrafficLight"
    	, "FilterNoPlatePeccancy"
    	, &Get().cTrackerCfgParam.cTrafficLight.fFilterNoPlatePeccancy
    	, Get().cTrackerCfgParam.cTrafficLight.fFilterNoPlatePeccancy
    	, "0:不过滤;1:过滤"
    	, "过滤无车牌违章结果"
    	, ""
    	, CUSTOM_LEVEL
    );
}

HRESULT CSWLPRVenusEPoliceParameter::InitHvDsp(VOID)
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

	GetEnum("\\HvDsp\\Misc"
		, "DrawRect"
		, &Get().cResultSenderParam.iDrawRect
		, Get().cResultSenderParam.iDrawRect
		, "0:不发送;1:发送"
		, "发送图像画红框"
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
		, "SaveVideoType"
		, &Get().cResultSenderParam.iSaveVideoType
		, Get().cResultSenderParam.iSaveVideoType
		, "0:一直录像;1:只存违章结果前后共15秒录像（且只保存违章结果）"
		, "硬盘录像类型"
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
		, "LoopReplaceEnable"
		, &Get().cResultSenderParam.cProcRule.fLoopReplace
		, Get().cResultSenderParam.cProcRule.fLoopReplace
		, "0:不启用;1:启用"
		, "循环替换"
		, ""
		, CUSTOM_LEVEL
	);

	GetString("\\HvDsp\\Identify", "StreetName"
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

	GetEnum("\\Tracker\\DetReverseRun"
		, "DetReverseRunEnable"
		, &Get().cTrackerCfgParam.nDetReverseRunEnable
		, Get().cTrackerCfgParam.nDetReverseRunEnable
		, "0:关;1:开"
		, "逆行检测开关"
		, ""
		, PROJECT_LEVEL
	);

	GetInt("\\Tracker\\DetReverseRun"
		, "Span"
		, &Get().cTrackerCfgParam.nSpan
		, Get().cTrackerCfgParam.nSpan
		, -100
		, 100
		, "过滤慢速逆行车辆跨度"
		, ""
		, CUSTOM_LEVEL
	);

	GetInt("\\HvDsp\\EventChecker"
		, "SpeedLimit"
		, &Get().cResultSenderParam.iSpeedLimit
		, Get().cResultSenderParam.iSpeedLimit
		, 0
		, 1000
		, "速度上限值(km/h)"
		, ""
		, CUSTOM_LEVEL
	);
	Get().cTrackerCfgParam.iSpeedLimit = Get().cResultSenderParam.iSpeedLimit;

	char szSection[64];
	char szChName[256];
	for (int i = 1; i < MAX_ROADLINE_NUM - 1; i++)
	{
		sprintf(szSection, "IsCrossLine%d%d", i, i + 1);
		sprintf(szChName, "车道线%d检测越线", i + 1);
		GetEnum("\\Tracker\\ActionDetect"
			, szSection
			, &Get().cTrackerCfgParam.cActionDetect.iIsCrossLine[i]
			, Get().cTrackerCfgParam.cActionDetect.iIsCrossLine[i]
			, "0:不检测;1:检测"
			, szChName
			, ""
			, CUSTOM_LEVEL
		);
	}

	for (int i = 0; i < MAX_ROADLINE_NUM; i++)
	{
		sprintf(szSection, "IsYellowLine%d%d", i, i + 1);
		sprintf(szChName, "车道线%d检测压线", i + 1);
		GetEnum("\\Tracker\\ActionDetect"
			, szSection
			, &Get().cTrackerCfgParam.cActionDetect.iIsYellowLine[i]
			, Get().cTrackerCfgParam.cActionDetect.iIsYellowLine[i]
			, "0:不判断压线;1:压实线;2:压黄线"
			, szChName
			, ""
			, CUSTOM_LEVEL
		);
	}


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

	// 电警非违章只输出一张大图，不提供可配。
	Get().cResultSenderParam.iBestSnapshotOutput = 1;
	Get().cResultSenderParam.iLastSnapshotOutput = 0;


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

	GetEnum("\\HvDsp\\Trigger"
		, "TriggerOutNormalStatus"
		, &Get().cResultSenderParam.nTriggerOutNormalStatus
		, Get().cResultSenderParam.nTriggerOutNormalStatus
		, "0:高电平;1:低电平"
		, "触发输出常态控制"
		, ""
		, PROJECT_LEVEL
	);

	 int iTemp = 1;
	 GetEnum("\\HvDsp\\EventChecker"
			, "OverLineSensitivity"
			, &iTemp
			, iTemp
			, "0:高;1:中;2:低"
			, "压线检测灵敏度"
			, ""
			, CUSTOM_LEVEL
		);
	 if( iTemp == 0 )
	 {
		 Get().cTrackerCfgParam.fltOverLineSensitivity = 2.5;
	 }
	 else if(  iTemp == 1 )
	 {
		 Get().cTrackerCfgParam.fltOverLineSensitivity = 1.75;
	 }
	 else if( iTemp == 2 )
	 {
		 Get().cTrackerCfgParam.fltOverLineSensitivity = 1.0;
	 }

	return S_OK;

}

HRESULT CSWLPRVenusEPoliceParameter::InitScaleSpeed(VOID)
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

HRESULT CSWLPRVenusEPoliceParameter::InitCamApp(VOID)
{
	 GetString("\\HvDsp\\Camera\\Ctrl"
		 	, "Addr"
		 	, Get().cCamCfgParam.szIP
		 	, Get().cCamCfgParam.szIP
		 	, (100-1)
		 	, "识别相机IP"
		 	, ""
		 	, PROJECT_LEVEL
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

	
	  
	  GetEnum("\\HvDsp\\Camera\\Ctrl"
			, "DynamicCfgEnable"
			, &Get().cCamCfgParam.iDynamicCfgEnable
			, Get().cCamCfgParam.iDynamicCfgEnable
			, "0:关闭;1:打开"
			, "动态设置参数"
			, ""
			, CUSTOM_LEVEL
	    );
	    

	  GetInt("\\HvDsp\\Camera\\Ctrl"
	    	, "MaxAGCLimit"
	    	, &Get().cCamCfgParam.iMaxAGCLimit
	    	, Get().cCamCfgParam.iMaxAGCLimit
	    	, 0
	    	, 255
	    	, "最大AE门限值"
	    	, ""
	    	, CUSTOM_LEVEL
	      );

	  Get().cCamCfgParam.iMinAGCLimit = 50; //电警最小门限默认值修改为50
	  GetInt("\\HvDsp\\Camera\\Ctrl"
	      	, "MinAGCLimit"
	      	, &Get().cCamCfgParam.iMinAGCLimit
	      	, Get().cCamCfgParam.iMinAGCLimit
	      	, 0
	      	, 255
	      	, "最小AE门限值"
	      	, ""
	      	, CUSTOM_LEVEL
	      );

	  Get().cCamCfgParam.iEnableAGC = 1;

	    int iAGCDB = (Get().cCamCfgParam.iMaxAGCLimit - Get().cCamCfgParam.iMinAGCLimit) / 13;
	    if (iAGCDB > 0)
	    {
	        for (int i = 0; i < 14; i++)
	        {
	        	Get().cCamCfgParam.irgAGCLimit[i] = Get().cCamCfgParam.iMinAGCLimit + (iAGCDB * i);
	        }
	    }
	    GetEnum("\\HvDsp\\Camera\\Ctrl"
	    		, "CtrlCpl"
	    		, &Get().cTrackerCfgParam.nCtrlCpl
	    		, Get().cTrackerCfgParam.nCtrlCpl
	    		, "0:不使能;1:使能"
	    		, "自动控制偏光镜"
	    		, "白天使能，晚上不使能"
	    		, CUSTOM_LEVEL
	    );

	  return S_OK;
}

HRESULT CSWLPRVenusEPoliceParameter::InitIPTCtrl(VOID)
{
	CSWString strCOMM;
	INT i = 1;		//金星只有一个外设串口，为COM1
	strCOMM.Format("\\Device[外部设备]\\COM%d[端口%02d]", i, i);
	GetEnum((LPCSTR)strCOMM
		, "Baudrate"
		, &Get().cDevParam[i].iBaudrate
		, 9600
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
		, CUSTOM_LEVEL
	);
	  
	GetEnum((LPCSTR)strCOMM
		, "DeviceType"
		, &Get().cDevParam[i].iDeviceType
		, 0
		, "0:无;3:康耐德红绿灯转换器"
		, "外部设备类型"
		, ""
		, CUSTOM_LEVEL
	);

		
  return S_OK;
}

HRESULT CSWLPRVenusEPoliceParameter::InitOuterCtrl(VOID)
{
	return S_OK;
}

HRESULT CSWLPRVenusEPoliceParameter::InitCamera(VOID)
{

    GetInt("\\CamApp"
    	, "JpegCompressRate"
    	, &Get().cCamAppParam.iJpegCompressRate
    	, Get().cCamAppParam.iJpegCompressRate
    	, 1
    	, 99
    	, "视频流Jpeg压缩率"
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

	/*
    if( Get().cGb28181.fEnalbe )
    {
    	Get().cCamAppParam.iIFrameInterval = 4;
        UpdateInt("\\CamApp"
            , "IFrameInterval"
            , Get().cCamAppParam.iIFrameInterval
            );
    }
	*/
	
	GetEnum("\\CamApp"
    	, "Resolution"
    	, &Get().cCamAppParam.iResolution
    	, Get().cCamAppParam.iResolution
    	//, "1:720P;2:1080P"
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
    	, "单位：Kbps"
    	, CUSTOM_LEVEL
    );

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
    	, 100
    	, "Jpeg压缩率自动调节下限"
    	, ""
    	, CUSTOM_LEVEL
    );

    GetInt("\\CamApp"
    	, "JpegCompressRateHigh"
    	, &Get().cCamAppParam.iJpegCompressRateH
    	, Get().cCamAppParam.iJpegCompressRateH
    	, 1
    	, 100
    	, "Jpeg压缩率自动调节上限"
    	, ""
    	, CUSTOM_LEVEL
    );

    GetEnum("\\CamApp"
    	, "AGCEnable"
    	, &Get().cCamAppParam.iAGCEnable
    	, Get().cCamAppParam.iAGCEnable
    	, "0:不使能;1:使能"
    	, "AGC使能"
    	, ""
    	, CUSTOM_LEVEL
    );

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

	GetEnum("\\CamApp"
    	, "CaptureEdge"
    	, &Get().cCamAppParam.iCaptureEdge
    	, Get().cCamAppParam.iCaptureEdge
    	, "0:不触发;1:外部下降沿触发;2:外部上升沿触发;3:上升沿下降沿都触发"
    	, "触发抓拍沿"
    	, ""
    	, PROJECT_LEVEL
    );

    char szAGC[255];
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
    }
    
    GetEnum("\\CamApp"
    	, "EnableDCAperture"
    	, &Get().cCamAppParam.iEnableDCAperture
    	, Get().cCamAppParam.iEnableDCAperture
    	, "0:不使能;1:使能"
    	, "使能DC光圈"
    	, ""
    	, PROJECT_LEVEL
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
    GetEnum("\\CamApp"
            , "NoiseReduction"
            , &Get().cCamAppParam.iSNFTNFMode
            , Get().cCamAppParam.iSNFTNFMode
            , "0:不使能;3:使能"
            , "降噪模式"
            , ""
            , CUSTOM_LEVEL
            );
    GetEnum("\\CamApp"
            , "NoiseReductionThreshold"
            , &Get().cCamAppParam.iTNFSNFValue
            , Get().cCamAppParam.iTNFSNFValue
            , "0:自动;1:低;2:中;3:高"
            , "降噪强度"
            , ""
            , PROJECT_LEVEL
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
        , 0
        , 255
        , "图像边缘增强"
        , "图像边缘增强"
        , CUSTOM_LEVEL
    );

	GetEnum("\\CamApp"
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
	);
	
    GetInt("\\CamApp"
          , "LEDPluseWidth"
          , &Get().cCamAppParam.iLEDPluseWidth
          , Get().cCamAppParam.iLEDPluseWidth
          , 0
          , 255
          , "补光灯脉宽"
          , ""
          , CUSTOM_LEVEL
      );

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

	return S_OK;
}

HRESULT CSWLPRVenusEPoliceParameter::InitCharacter(VOID)
{
	
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
		  , 3072
		  , "X坐标"
		  , ""
		  , CUSTOM_LEVEL
	  );
	  
	GetInt("\\Overlay\\JPEG"
		  , "Y"
		  , &Get().cOverlay.cJPEGInfo.iY
		  , Get().cOverlay.cJPEGInfo.iY
		  , 0
		  , 2048
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


	  return S_OK;
}


HRESULT CSWLPRVenusEPoliceParameter::InitGB28181(VOID)
{
	return CSWLPRParameter::InitGB28181();
}


HRESULT CSWLPRVenusEPoliceParameter::InitAutoReboot(VOID)
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

HRESULT CSWLPRVenusEPoliceParameter::InitONVIF(VOID)
{

    SW_TRACE_NORMAL(" HRESULT CSWLPRVenusEPoliceParameter::InitONVIF(VOID)");
	
	return CSWLPRParameter::InitONVIF();

}
