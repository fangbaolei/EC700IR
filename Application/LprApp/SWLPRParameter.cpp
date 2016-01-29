#include "config.h"
#include "SWFC.h"
#include "SWLPRParameter.h"
#include "SWLPRApplication.h"

REGISTER_CLASS(CSWLPRParameter)
CSWLPRParameter::CSWLPRParameter()
{
	ResetParam();
}

CSWLPRParameter::~CSWLPRParameter()
{
}

void CSWLPRParameter::SetDefaultTracker()
{
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
    Get().cTrackerCfgParam.nProcessPlate_LightBlue = 0;
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
    Get().cTrackerCfgParam.nRoadLineNumber = 3;
	Get().cTrackerCfgParam.nOutputRoadTypeName = 0;
    Get().cTrackerCfgParam.nCaptureOnePos = 50;
    Get().cTrackerCfgParam.nCaptureTwoPos = 80;
	Get().cTrackerCfgParam.nCaptureThreePos = 0;
    Get().cTrackerCfgParam.cActionDetect.iDetectCarStopTime = 10;
    Get().cTrackerCfgParam.nDetReverseRunEnable = 0;
	Get().cTrackerCfgParam.nSpan = 5;
	Get().cTrackerCfgParam.iRoadNumberBegin = 0;
    Get().cTrackerCfgParam.iStartRoadNum = 1;
	Get().cTrackerCfgParam.nFlashLightThreshold = 25;
	Get().cTrackerCfgParam.fDoublePlateEnable = 1;
	Get().cTrackerCfgParam.nRecogAsteriskThreshold = 0;
	Get().cTrackerCfgParam.fUseEdgeMethod = FALSE;
	Get().cTrackerCfgParam.fEnableT1Model = FALSE;
	Get().cTrackerCfgParam.fEnableDefaultWJ = TRUE;
	Get().cTrackerCfgParam.nRecogGxPolice = 1;
	Get().cTrackerCfgParam.fEnableAlpha_5 = FALSE;
    Get().cTrackerCfgParam.nCarArrivedPos = 60;
	Get().cTrackerCfgParam.nCarArrivedPosNoPlate = 55;
	Get().cTrackerCfgParam.nCarArrivedPosYellow = 70;
	Get().cTrackerCfgParam.nProcessType = 0;
	Get().cTrackerCfgParam.nOutPutType = 0;
	Get().cTrackerCfgParam.nNightPlus = 0;
    Get().cTrackerCfgParam.nWalkManSpeed = 200;
	Get().cTrackerCfgParam.fltBikeSensitivity = 4.0f;
	Get().cTrackerCfgParam.nCarArrivedDelay = 0;
	Get().cTrackerCfgParam.nVoteFrameNum = 35;
    Get().cTrackerCfgParam.nMaxEqualFrameNumForVote = 15;
    Get().cTrackerCfgParam.nBlockTwinsTimeout = 60;
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
    Get().cTrackerCfgParam.nCarArriveTrig = Get().cCamCfgParam.iDynamicTriggerEnable;
	Get().cTrackerCfgParam.nTriggerPlateType = 0;
	Get().cTrackerCfgParam.nEnableProcessBWPlate = 0;
	Get().cTrackerCfgParam.iPlatform = 0;
    Get().cTrackerCfgParam.iSpeedLimit = Get().cResultSenderParam.iSpeedLimit;
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

    //默认车道线
    Get().cTrackerCfgParam.rgcRoadInfo[0].ptTop.x = 1303;
    Get().cTrackerCfgParam.rgcRoadInfo[0].ptTop.y = 0;
    Get().cTrackerCfgParam.rgcRoadInfo[0].ptBottom.x = 5;
    Get().cTrackerCfgParam.rgcRoadInfo[0].ptBottom.y = 1761;

    Get().cTrackerCfgParam.rgcRoadInfo[1].ptTop.x = 1551;
    Get().cTrackerCfgParam.rgcRoadInfo[1].ptTop.y = 0;
    Get().cTrackerCfgParam.rgcRoadInfo[1].ptBottom.x = 1046;
    Get().cTrackerCfgParam.rgcRoadInfo[1].ptBottom.y = 1968;

    Get().cTrackerCfgParam.rgcRoadInfo[2].ptTop.x = 1816;
    Get().cTrackerCfgParam.rgcRoadInfo[2].ptTop.y = 0;
    Get().cTrackerCfgParam.rgcRoadInfo[2].ptBottom.x = 2269;
    Get().cTrackerCfgParam.rgcRoadInfo[2].ptBottom.y = 1986;

    Get().cTrackerCfgParam.rgcRoadInfo[3].ptTop.x = 2091;
    Get().cTrackerCfgParam.rgcRoadInfo[3].ptTop.y = 0;
    Get().cTrackerCfgParam.rgcRoadInfo[3].ptBottom.x = 3385;
    Get().cTrackerCfgParam.rgcRoadInfo[3].ptBottom.y = 1709;

	//扫描区域默认值
    Get().cTrackerCfgParam.cDetectArea.fEnableDetAreaCtrl = TRUE;
	Get().cTrackerCfgParam.cDetectArea.DetectorAreaLeft = 0;
	Get().cTrackerCfgParam.cDetectArea.DetectorAreaRight = 100;
    Get().cTrackerCfgParam.cDetectArea.DetectorAreaTop = 15;
    Get().cTrackerCfgParam.cDetectArea.DetectorAreaBottom = 55;
    Get().cTrackerCfgParam.cDetectArea.nDetectorMinScaleNum = 5;
    Get().cTrackerCfgParam.cDetectArea.nDetectorMaxScaleNum = 9;

	//梯形扫描区域默认值
    Get().cTrackerCfgParam.cTrapArea.fEnableDetAreaCtrl = TRUE;
    //初始化梯形区域
    Get().cTrackerCfgParam.cTrapArea.TopLeftX =     Get().cTrackerCfgParam.cDetectArea.DetectorAreaLeft;
    Get().cTrackerCfgParam.cTrapArea.TopRightX =    Get().cTrackerCfgParam.cDetectArea.DetectorAreaRight;
    Get().cTrackerCfgParam.cTrapArea.BottomLeftX =  Get().cTrackerCfgParam.cDetectArea.DetectorAreaLeft;
    Get().cTrackerCfgParam.cTrapArea.BottomRightX = Get().cTrackerCfgParam.cDetectArea.DetectorAreaRight;

    Get().cTrackerCfgParam.cTrapArea.TopLeftY =     Get().cTrackerCfgParam.cDetectArea.DetectorAreaTop;
    Get().cTrackerCfgParam.cTrapArea.TopRightY =    Get().cTrackerCfgParam.cDetectArea.DetectorAreaTop;
    Get().cTrackerCfgParam.cTrapArea.BottomLeftY =  Get().cTrackerCfgParam.cDetectArea.DetectorAreaBottom;
    Get().cTrackerCfgParam.cTrapArea.BottomRightY = Get().cTrackerCfgParam.cDetectArea.DetectorAreaBottom;

    Get().cTrackerCfgParam.cTrapArea.nDetectorMinScaleNum = Get().cTrackerCfgParam.cDetectArea.nDetectorMinScaleNum;
    Get().cTrackerCfgParam.cTrapArea.nDetectorMaxScaleNum = Get().cTrackerCfgParam.cDetectArea.nDetectorMaxScaleNum;

	// 抓拍图识别参数  
    Get().cTrackerCfgParam.cRecogSnapArea.fEnableDetAreaCtrl = TRUE;
    Get().cTrackerCfgParam.cRecogSnapArea.DetectorAreaLeft = Get().cMatchParam.iDetectorAreaLeft;
    Get().cTrackerCfgParam.cRecogSnapArea.DetectorAreaTop = Get().cMatchParam.iDetectorAreaTop;
    Get().cTrackerCfgParam.cRecogSnapArea.DetectorAreaRight = Get().cMatchParam.iDetectorAreaRight;
    Get().cTrackerCfgParam.cRecogSnapArea.DetectorAreaBottom = Get().cMatchParam.iDetectorAreaBottom;
    Get().cTrackerCfgParam.cRecogSnapArea.nDetectorMinScaleNum = Get().cMatchParam.iDetectorMinScaleNum;
    Get().cTrackerCfgParam.cRecogSnapArea.nDetectorMaxScaleNum = Get().cMatchParam.iDetectorMaxScaleNum;

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
    Get().cTrackerCfgParam.cScaleSpeed.fltDistance = 90;
    Get().cTrackerCfgParam.cScaleSpeed.fltRoadWidth = 3.75;
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

    // 新的软件测速参数
    Get().cTrackerCfgParam.cScaleSpeed.fEnable = TRUE;
    Get().cTrackerCfgParam.cScaleSpeed.fltDistance = 90;
    Get().cTrackerCfgParam.cScaleSpeed.fltRoadWidth = 3.75;

	//视频检测参数
	Get().cTrackerCfgParam.cVideoDet.nVideoDetMode = USE_VIDEODET;
	Get().cTrackerCfgParam.cVideoDet.nDayBackgroudThreshold = 20;
    Get().cTrackerCfgParam.cVideoDet.nNightBackgroudThreshold = 28;
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

}

void CSWLPRParameter::SetDefaultCamApp()
{
	//相机相关参数

	Get().cCamAppParam.iJpegStream = 1;
	Get().cCamAppParam.iH264Stream = 0;

	Get().cCamAppParam.iIFrameInterval = 10;
    Get().cCamAppParam.iTargetBitRate = 2 * 1024; //Kbps
    Get().cCamAppParam.iMaxBitRate = 4 * 1024;  //16 * 1024 Kbps
	Get().cCamAppParam.iFrameRate = 12;
    Get().cCamAppParam.iResolution = 0;
	Get().cCamAppParam.iRateControl = 0;       //0:VBR 1:CBR
	Get().cCamAppParam.iVbrDuration = 0;
	Get().cCamAppParam.iVbrSensivitity = 0;

	Get().cCamAppParam.iIFrameIntervalSecond = 10;	  /**< 第二路H.264码流中的I帧间隔 */
	Get().cCamAppParam.iTargetBitRateSecond = 6*1024;	  /**< 第二路H.264码流比特率 */
	Get().cCamAppParam.iMaxBitRateSecond = 16 * 1024;
	Get().cCamAppParam.iFrameRateSecond = 25;		  /**< 第二路H.264码流帧率 */
	Get().cCamAppParam.iResolutionSecond = 0;		  /**< 第二路H.264图像分辨率 */
	Get().cCamAppParam.iRateControlSecond = 0;
	Get().cCamAppParam.iVbrDurationSecond = 0;
	Get().cCamAppParam.iVbrSensivititySecond = 0;

	Get().cCamAppParam.iAGCTh = 100;
    Get().cCamAppParam.iAGCShutterLOri = 100;
    Get().cCamAppParam.iAGCShutterHOri = 3000;
    Get().cCamAppParam.iAGCGainLOri = 10;
    Get().cCamAppParam.iAGCGainHOri = 150;
	Get().cCamAppParam.iAGCScene = 0;
    Get().cCamAppParam.iAGCDayNightShutterControl = 0;
    Get().cCamAppParam.iAGCNightShutterHOri = 3500;

    Get().cCamAppParam.iGainR = 36;
    Get().cCamAppParam.iGainG = 36;
    Get().cCamAppParam.iGainB = 36;
	Get().cCamAppParam.iGain = 70;
	Get().cCamAppParam.iShutter = 2000;

	Get().cCamAppParam.iIris = 5;
	Get().cCamAppParam.iAutoFocus = 2;
	Get().cCamAppParam.iAWBMode = 0;
	Get().cCamAppParam.iAEMode = 0;
	Get().cCamAppParam.iWDREnable = 0;
	Get().cCamAppParam.iNRLevel = 0;

    Get().cCamAppParam.iCaptureGainR = 46;
	Get().cCamAppParam.iCaptureGainG = 36;
    Get().cCamAppParam.iCaptureGainB = 54;
    Get().cCamAppParam.iCaptureGain = 80;
    Get().cCamAppParam.iCaptureShutter = 1000;
    Get().cCamAppParam.iCaptureSharpenThreshold = 200;
    Get().cCamAppParam.iCaptureAutoParamEnable = 0;
    Get().cCamAppParam.iEnableCaptureGainRGB = 0;
    Get().cCamAppParam.iEnableCaptureGain = 1;
    Get().cCamAppParam.iEnableCaptureShutter = 1;
    Get().cCamAppParam.iEnableCaptureSharpen = 1;

	Get().cCamAppParam.iJpegCompressRate = 60;
	Get().cCamAppParam.iJpegCompressRateCapture = 80;


	strcpy(Get().cCamAppParam.szNFSParam, "-o timeo=1,soft,tcp,nolock,retry=1");
	memset(Get().cCamAppParam.szSafeSaverInitStr, 0, sizeof(Get().cCamAppParam.szSafeSaverInitStr));

    Get().cCamAppParam.iJpegExpectSize = 300;
	Get().cCamAppParam.iJpegCompressRateL = 30;
    Get().cCamAppParam.iJpegCompressRateH = 80;

	Get().cCamAppParam.iJpegType = 0;
    Get().cCamAppParam.iAGCEnable = 1;
	Get().cCamAppParam.iAWBEnable = 1;
	Get().cCamAppParam.iFlashRateSynSignalEnable = 0;
	Get().cCamAppParam.iFlashRateMultiplication = 1;
	Get().cCamAppParam.iFlashRatePolarity = 1;
	Get().cCamAppParam.iFlashRatePulseWidth = 4000;
	Get().cCamAppParam.iFlashRateOutputType = 0;
	Get().cCamAppParam.iCaptureSynPolarity = 1;			
	Get().cCamAppParam.iCaptureSynPulseWidth = 1000;		
	Get().cCamAppParam.iCaptureSynOutputType = 1;			
	Get().cCamAppParam.iCaptureSynCoupling = 0;			
	Get().cCamAppParam.iCaptureSynSignalEnable = 1;
	Get().cCamAppParam.iCaptureEdge = 0;
	Get().cCamAppParam.iFlashDifferentLane = 0;
	Get().cCamAppParam.iFlashDifferentLaneExt = 0;
	Get().cCamAppParam.iEnableDCAperture = 0;
	Get().cCamAppParam.iEnableSharpen = 0;
	Get().cCamAppParam.iSharpenCapture = 1;
	Get().cCamAppParam.iSharpenThreshold = 7;
	Get().cCamAppParam.iColorMatrixMode = 1;
	Get().cCamAppParam.iContrastValue = 0;
	Get().cCamAppParam.iSaturationValue = 8;
	Get().cCamAppParam.iTemThreshold = 32;
	Get().cCamAppParam.iHueThreshold = 0;
	Get().cCamAppParam.iHazeRemovalMode = 1;
	Get().cCamAppParam.iHazeRemovalThreshold = 0;

	//strcpy(szNetDiskIP, "172.18.10.10");
	strcpy(Get().cCamAppParam.szFtpRoot, "/array1");
	strcpy(Get().cCamAppParam.szNetDiskUser, "No User");
	strcpy(Get().cCamAppParam.szNetDiskPwd, "123");
	//strcpy(szNFS, "/volume1/nfs");
	//iNetDiskSpace = 1;

	Get().cCamAppParam.iAvgY = 30;

	Get().cCamAppParam.iGammaMode = 2;
	Get().cCamAppParam.rgiGamma[0][0] = 32;
	Get().cCamAppParam.rgiGamma[0][1] = 32;

	Get().cCamAppParam.rgiGamma[1][0] = 64;
	Get().cCamAppParam.rgiGamma[1][1] = 64;

	Get().cCamAppParam.rgiGamma[2][0] = 96;
	Get().cCamAppParam.rgiGamma[2][1] = 96;

	Get().cCamAppParam.rgiGamma[3][0] = 128;
	Get().cCamAppParam.rgiGamma[3][1] = 128;

	Get().cCamAppParam.rgiGamma[4][0] = 160;
	Get().cCamAppParam.rgiGamma[4][1] = 160;

	Get().cCamAppParam.rgiGamma[5][0] = 192;
	Get().cCamAppParam.rgiGamma[5][1] = 192;

	Get().cCamAppParam.rgiGamma[6][0] = 222;
	Get().cCamAppParam.rgiGamma[6][1] = 222;

	Get().cCamAppParam.rgiGamma[7][0] = 255;
	Get().cCamAppParam.rgiGamma[7][1] = 255;

	memcpy(Get().cCamAppParam.rgiDefGamma, Get().cCamAppParam.rgiGamma, 64);
	for(INT i = 0; i < 16; i++)
	{
	Get().cCamAppParam.rgiAGCZone[i] = 1;
	}
	// swpa_memset(rgiTrafficLigtZone,0,sizeof(rgiTrafficLigtZone));
	Get().cCamAppParam.iSaturationEnable = 0;
	Get().cCamAppParam.iContrastEnalbe = 0;
	//iSharpenEnable = 0;
	Get().cCamAppParam.iWDRValue = 256;		//默认为256
	Get().cCamAppParam.iSNFTNFMode = 0;
    Get().cCamAppParam.iTNFSNFValue = 1;
	Get().cCamAppParam.iGrayImageEnable = 0;
	Get().cCamAppParam.iCVBSMode = 0;                     /** CVBS制式 范围:0~1   0：PAL，1：NTSC*/
    Get().cCamAppParam.iJpegAutoCompressEnable = 1;        /** Jpeg自动调节开关 */
	Get().cCamAppParam.iGammaValue = 10;
	Get().cCamAppParam.iGammaEnable = 0;
	Get().cCamAppParam.iEdgeValue = 0x80;

	Get().cCamAppParam.iX = 0;
	Get().cCamAppParam.iY = 0;
	Get().cCamAppParam.iFontSize = 32;             // 注：默认是32点阵
	Get().cCamAppParam.iDateFormat = 0;
	Get().cCamAppParam.iFontColor = 0x00ff0000;    // 注：临时将默认值改为红色(0x00ff0000)
	strcpy(Get().cCamAppParam.szCharater, "");
	Get().cCamAppParam.iEnableCharacterOverlap = 0;
	Get().cCamAppParam.iEnableFixedLight = 0;      // 注：默认情况，字体随场景亮度而改变

    Get().cCamAppParam.fEnableH264BrightnessAdjust = FALSE;
	Get().cCamAppParam.iAdjustPointX = 10;
	Get().cCamAppParam.iAdjustPointY = 25;
	Get().cCamAppParam.iEnableCCDSignalCheck = 1;

	Get().cCamAppParam.iExternalSyncMode = 1;
	Get().cCamAppParam.iSyncRelay = 2200;

    Get().cCamAppParam.iLEDPolarity = 0;
	Get().cCamAppParam.iLEDOutputType = 0;
	Get().cCamAppParam.iLEDPluseWidth = 5;

	Get().cCamAppParam.iALMPolarity = 1;
	Get().cCamAppParam.iALMOutputType = 0;

    Get().cCamAppParam.iF1OutputType = 1;

    Get().cCamAppParam.iFlashEnable		= 1;
	Get().cCamAppParam.iFlashPolarity		= 0;
    Get().cCamAppParam.iFlashOutputType	= 1;
	Get().cCamAppParam.iFlashCoupling  	= 0;
    Get().cCamAppParam.iFlashPluseWidth	= 50;

	Get().cCamAppParam.iColorGradation = 0;

	Get().cCamAppParam.iCVBSExport = 1;
	Get().cCamAppParam.iCVBSCropStartX = 600;
	Get().cCamAppParam.iCVBSCropStartY = 252;

    Get().cCamAppParam.iAWBWorkMode = 2;
	Get().cCamAppParam.iAutoControlCammeraAll = 1;
}

void CSWLPRParameter::SetDefaultCharacter()
{
	memset(&Get().cOverlay, 0, sizeof(Get().cOverlay));
    Get().cOverlay.cJPEGInfo.iColor = 0x00FF0000;
    Get().cOverlay.cJPEGInfo.iFontSize = 50;
 
	Get().cOverlay.cH264Info.iFontSize = 32;
	Get().cOverlay.cH264SecondInfo.iFontSize = 32;

    swpa_strcpy(Get().cOverlay.cH264Info.szInfo, "");
	swpa_strcpy(Get().cOverlay.cH264SecondInfo.szInfo, "第二路H264流字符叠加样例");
    swpa_strcpy(Get().cOverlay.cJPEGInfo.szInfo, "");
}

void CSWLPRParameter::SetDefaultOnvif()
{
	Get().cOnvifParam.iOnvifEnable = 1;
	Get().cOnvifParam.iAuthenticateEnable = 0;

	
	Get().cRtspParam.iRTSPStreamNum			= 2;				
	Get().cRtspParam.rgCommunicationMode[0]	= 0;
	Get().cRtspParam.rgCommunicationMode[1]	= 0;				
	Get().cRtspParam.iAuthenticateEnable		= 0;				
    Get().cRtspParam.iAutoControlBitrateEnable = 0;
	Get().cRtspParam.iRTSPServerPort			= 554;				
	strcpy(Get().cRtspParam.rgStreamName[0], "h264ESVideoTest");
	strcpy(Get().cRtspParam.rgStreamName[1], "h264ESVideoTestSecond");
}

void CSWLPRParameter::SetDefaultGB28181()
{
	swpa_memset(&Get().cGb28181, 0, sizeof(Get().cGb28181));
	Get().cGb28181.fEnalbe = FALSE;
	Get().cGb28181.fVideoSaveEnable = FALSE;

	swpa_strcpy(Get().cGb28181.szServerID,"34020000002000000001");
	swpa_strcpy(Get().cGb28181.szIPC_ID,"34020000001320000001");
	swpa_strcpy(Get().cGb28181.szServerIP,"172.18.11.123");
	swpa_strcpy(Get().cGb28181.szAlarmID,"34020000001340000001");
	swpa_strcpy(Get().cGb28181.szPassWord,"12345678");
	Get().cGb28181.iServerPort = 5060;
	Get().cGb28181.iIPC_Port = 5060;
	Get().cGb28181.iIPC_VideoPort = 5080;

	swpa_strcpy(Get().cGb28181.szLongitude,"0.0");
	swpa_strcpy(Get().cGb28181.szLatitude,"0.0");

	swpa_strcpy(Get().cGb28181.szManufacturer,"NULL");
	swpa_strcpy(Get().cGb28181.szBlock,"NULL");
	swpa_strcpy(Get().cGb28181.szAddress,"NULL");
	swpa_strcpy(Get().cGb28181.szOwner,"NULL");
	swpa_strcpy(Get().cGb28181.szCivilCode,"NULL");

}


void CSWLPRParameter::SetDefaultFrontPannel()
{
	Get().cFrontPannelParam.iUsedAutoControllMode = 0;
	Get().cFrontPannelParam.iAutoRunFlag = 1;
	Get().cFrontPannelParam.iPulseWidthMin = 5;
	Get().cFrontPannelParam.iPulseWidthMax = 42;
	Get().cFrontPannelParam.iPulseStep = 5;
	Get().cFrontPannelParam.iPulseLevel = 0;
	Get().cFrontPannelParam.iPolarizingPrismMode = -1;
	Get().cFrontPannelParam.iPalanceLightStatus = 0;
	Get().cFrontPannelParam.iFlash1PulseWidth = 100;
	Get().cFrontPannelParam.iFlash1Polarity = 0;
	Get().cFrontPannelParam.iFlash1Coupling = 0;
	Get().cFrontPannelParam.iFlash1ResistorMode = 0;
	Get().cFrontPannelParam.iFlash2PulseWidth = 100;
	Get().cFrontPannelParam.iFlash2Polarity = 0;
	Get().cFrontPannelParam.iFlash2Coupling = 0;
	Get().cFrontPannelParam.iFlash2ResistorMode = 0;
	Get().cFrontPannelParam.iFlash3PulseWidth = 100;
	Get().cFrontPannelParam.iFlash3Polarity = 0;
	Get().cFrontPannelParam.iFlash3Coupling = 0;
	Get().cFrontPannelParam.iFlash3ResistorMode = 0;
	Get().cFrontPannelParam.iDivFrequency = 1;
	Get().cFrontPannelParam.iOutPutDelay = 0;
}


void CSWLPRParameter::SetDefaultCamCfgParam()
{
	sprintf(Get().cCamCfgParam.szIP, "172.18.23.42");     
	Get().cCamCfgParam.iCamType = 0;
	Get().cCamCfgParam.iCamPort = 8888;
    Get().cCamCfgParam.iDynamicCfgEnable = 1;
    Get().cCamCfgParam.iDynamicTriggerEnable =1;
	Get().cCamCfgParam.iTriggerDelay = 0;
	Get().cCamCfgParam.iTriggerPlateType = 0;
	Get().cCamCfgParam.iDynamicCfgEnableOnTrigger = 0;
    Get().cCamCfgParam.iNightShutterThreshold = -1;// 此参数仅金星使用，因此将默认值设置如此
    Get().cCamCfgParam.iNightAvgYThreshold = 10;
	Get().cCamCfgParam.iJpegBufferCount = 1;
    Get().cCamCfgParam.iEnableAGC = 1;
    Get().cCamCfgParam.iAutoParamEnable = 1;
	Get().cCamCfgParam.fltDB = 3.0;
    Get().cCamCfgParam.iMinExposureTime  = 0;
	Get().cCamCfgParam.iMinGain  = 70;
    Get().cCamCfgParam.iMaxExposureTime  = 2500;
	Get().cCamCfgParam.iMaxGain  = 220;
    Get().cCamCfgParam.iMaxAGCLimit = 163;
    Get().cCamCfgParam.iMinAGCLimit = 20;
    Get().cCamCfgParam.iMaxPSD = 3000;
    Get().cCamCfgParam.iMinPSD = 500;
	Get().cCamCfgParam.iMinPlateLight = 140;
	Get().cCamCfgParam.iMaxPlateLight = 160;
	Get().cCamCfgParam.nTestProtocolPort = SVR_PORT;

	Get().cCamCfgParam.iIPNCLogOutput = 0;
		
	INT irgExposureTimeTemp[MAX_LEVEL_COUNT] =
	{
		8, 8, 8, 8, 12, 17, 24, 34, 48, 68, 96, 96, 96, 96
	};

	for (INT i = 0; i < MAX_LEVEL_COUNT; i++)
	{
		Get().cCamCfgParam.irgExposureTime[i] = irgExposureTimeTemp[i];
		Get().cCamCfgParam.irgAGCLimit[i] = -1;
		Get().cCamCfgParam.irgGain[i] = -1;
	}

	Get().cCamCfgParam.iCaptureAutoParamEnable = 0;
	Get().cCamCfgParam.iCaptureDayShutterMax = 1500;
	Get().cCamCfgParam.iCaptureDayGainMax	= 180;
	Get().cCamCfgParam.iCaptureNightShutterMax = 2000;
	Get().cCamCfgParam.iCaptureNightGainMax = 180;
}


void CSWLPRParameter::SetDefaultHvDSP()
{
	Get().cResultSenderParam.iBestSnapshotOutput = 1;
	Get().cResultSenderParam.iLastSnapshotOutput = 1;
	Get().cResultSenderParam.iOutputCaptureImage = 0;
	Get().cResultSenderParam.iRecordFreeSpacePerDisk = 10;
	Get().cResultSenderParam.iVideoFreeSpacePerDisk = 10;
	Get().cResultSenderParam.iVideoDisplayTime = 500;
	Get().cResultSenderParam.iDrawRect = 1;
	Get().cResultSenderParam.iEddyType = 0;
	Get().cResultSenderParam.iSpeedLimit = 1000;
	Get().cResultSenderParam.iSaveType = 0;
	Get().cResultSenderParam.iWidth = 720;
	Get().cResultSenderParam.iHeight = 288;
	Get().cResultSenderParam.iCheckEventTime = 1;
	Get().cResultSenderParam.iFilterUnSurePeccancy = 0;
	Get().cResultSenderParam.fOutputAppendInfo = TRUE;
	Get().cResultSenderParam.fOutputObservedFrames = TRUE;
	Get().cResultSenderParam.fOutputCarArriveTime = FALSE;
	strcpy(Get().cResultSenderParam.szSafeSaverInitStr, "NULL");
	strcpy(Get().cResultSenderParam.szStreetName, "NULL");
	strcpy(Get().cResultSenderParam.szStreetDirection, "NULL");
	Get().cResultSenderParam.iOutputOnlyPeccancy = 0;
	Get().cResultSenderParam.iSendRecordSpace = 5;
	Get().cResultSenderParam.iSendHisVideoSpace = 3;
	Get().cResultSenderParam.nOutputPeccancyType = 0;
	Get().cResultSenderParam.fltCarAGCLinkage = 0.0f;
	Get().cResultSenderParam.iAWBLinkage = 0;
	Get().cResultSenderParam.fOutputFilterInfo = 0;
	Get().cResultSenderParam.fInitHdd = FALSE;
	Get().cResultSenderParam.iFileSize = 512;		
	Get().cResultSenderParam.iDiskType = 2;
	Get().cResultSenderParam.fIsSafeSaver = FALSE;
	Get().cResultSenderParam.iSaveSafeType = 1;
	Get().cResultSenderParam.fSaveVideo = FALSE;
	Get().cResultSenderParam.iSaveVideoType = 0;
	Get().cResultSenderParam.fMTUSetEnable = FALSE;
	strcpy(Get().cResultSenderParam.szPeccancyPriority, "逆行-非机动车道-闯红灯-不按车道行驶-压线-超速");
	Get().cResultSenderParam.iCapMode = 0;
	Get().cResultSenderParam.iFlashDifferentLane = 0;
    Get().cResultSenderParam.iFlashOneByOne = 0;

	Get().cResultSenderParam.fEnableTriggerOut = 1;
	Get().cResultSenderParam.nTriggerOutNormalStatus = 1;
	Get().cResultSenderParam.nTriggerOutPlusWidth = 5000;

	Get().cResultSenderParam.fEnableNtp = 0;
	Get().cResultSenderParam.iNtpSyncInterval = 3600; // 1小时
	Get().cResultSenderParam.iTimeZone = 20;		//默认东8区，GMT+8,0为GMT-12
	swpa_memset(Get().cResultSenderParam.szNtpServerIP, 0, sizeof(Get().cResultSenderParam.szNtpServerIP));
	swpa_strcpy(Get().cResultSenderParam.szNtpServerIP, "210.72.145.44");
	swpa_strcpy(Get().cResultSenderParam.szNetDiskIP, "172.18.10.10");
	swpa_strcpy(Get().cResultSenderParam.szNFS, "/volume1/nfs");
	swpa_strcpy(Get().cResultSenderParam.szNFSParam, "-o nolock,tcp");
	Get().cResultSenderParam.iNetDiskSpace = 1;

	Get().cResultSenderParam.iUseRushRule = 0;
	Get().cResultSenderParam.iEnableBackupIO = 0;
	Get().cResultSenderParam.iTimeIn = 1500;
	Get().cResultSenderParam.iBarrierStatus = 0;

		//主动连接默认值
	Get().cResultSenderParam.cAutoLinkParam.iAutoLinkPort = 6665;

    // 截图参数
    Get().cResultSenderParam.iLastSnapshotCropOutput = 0;
    Get().cResultSenderParam.iCropWidthLevel = 6;
    Get().cResultSenderParam.iCropHeightLevel = 4;

}


void CSWLPRParameter::SetDefaultIPTCtrl()
{
	for(int i = 0; i < 2; i++)
	{
		Get().cDevParam[i].iBaudrate = 9600;
		Get().cDevParam[i].iDeviceType = 0;
		Get().cDevParam[i].iRoadNo = 0;
		//金星只能用RS485
		Get().cDevParam[i].iCommType = 1;
	}
}

void CSWLPRParameter::SetDefaultOuterCtrl()
{
	swpa_memset(&Get().cMatchParam, 0, sizeof(Get().cMatchParam));
    Get().cMatchParam.dwPlateKeepTime = 1500;
    Get().cMatchParam.dwSignalKeepTime = 2000;
    Get().cMatchParam.dwMatchMaxTime = 1500;
    Get().cMatchParam.dwMatchMinTime = 1500;
    Get().cMatchParam.fCaptureRecong = TRUE;

    Get().cMatchParam.signal[0].dwType = 3;
    Get().cMatchParam.signal[0].dwRoadID = 0xFF;
}



void CSWLPRParameter::ResetParam(VOID)
{	
	//日志默认等级
	Get().nLogLevel = 1;
    Get().nWorkModeIndex = PRM_CAP_FACE;
    Get().nCameraWorkMode = 19;
    Get().nMJPEGWidth = 3392;
    Get().nMJPEGWidth = 2000;

	SetDefaultCamApp();
	SetDefaultCamCfgParam();
	SetDefaultCharacter();
	SetDefaultFrontPannel();
	SetDefaultGB28181();
	SetDefaultHvDSP();
	SetDefaultOnvif();
	SetDefaultOuterCtrl();
	SetDefaultTracker();
	SetDefaultIPTCtrl();
	
		
	Get().cAutoRebootParam.fEnable = FALSE;			
	Get().cAutoRebootParam.iCarLeftCountLimit = 50;
	Get().cAutoRebootParam.iSysUpTimeLimit = 48;	

	Get().cDomeRockerParam.fRockerEnable = FALSE;
	Get().cDomeRockerParam.iBaudrate = 9600;
    Get().cDomeRockerParam.iProtocol = 0;

	sprintf(Get().cTcpipCfgParam.szIp, "100.100.100.101");
	sprintf(Get().cTcpipCfgParam.szNetmask, "255.0.0.0");
	sprintf(Get().cTcpipCfgParam.szGateway, "100.100.1.1");

	//前端图像采集模块参数
	Get().cImgFrameParam.iCamNum = 1;
	for ( INT i=0; i<MAX_CAM_COUNT; ++i )
	{
		memset(Get().cImgFrameParam.rgstrCamIP[i], 0, 32);
		memset(Get().cImgFrameParam.rgstrHVCParm[i], 0, 128);
	}
	Get().cImgFrameParam.fltCaptureFrameRate = 12.5;
	Get().cImgFrameParam.iOutputFrameRate = 15;
	
}

HRESULT CSWLPRParameter::Initialize(CSWString strFilePath)
{
	// 初始化前重新设置默认值
	ResetParam();
	
	if(S_OK == CSWParameter<ModuleParams>::Initialize(strFilePath)
	&& S_OK == InitSystem()
	&& S_OK == InitTracker()
	&& S_OK == InitHvDsp()
	&& S_OK == InitOuterCtrl()
	&& S_OK == InitIPTCtrl()
	&& S_OK == InitCamApp()
	&& S_OK == InitScaleSpeed()
	&& S_OK == Init368()
	&& S_OK == InitCharacter()
	&& S_OK == InitGB28181()
	&& S_OK == InitONVIF()
	)
	{
		return S_OK;		
	}

	SW_TRACE_DEBUG("<LPRParameter> Initialize failed.\n");
	return E_FAIL;
}

HRESULT CSWLPRParameter::GetWorkMode(INT &iWorkMode, CSWString & strWorkKode)
{
	iWorkMode = Get().nWorkModeIndex;
	strWorkKode = m_strWorkMode;
	return S_OK;
}

/**
 *@brief 初始化system节点
 *@return 成功返回S_OK,其他值表示失败
 */
HRESULT CSWLPRParameter::InitSystem()
{
	/*
	GetEnum("\\Tracker"
		, "PlateRecogMode"
		, &Get().nWorkModeIndex
		, Get().nWorkModeIndex
		, (PSTR)(LPCSTR)m_strWorkMode
  	, "工作模式"
  	, ""
  	, INNER_LEVEL
	, TRUE
  );
  */
  		
	char szTemp[512];	
/*	
	GetString("\\System[系统]"
		, "ComMac"
		, szTemp
		, ""
		, sizeof(szTemp)
		, "MAC地址"
		, ""
		, CUSTOM_LEVEL
		, TRUE
		);
*/		
	GetEnum("\\System[系统]"
  	, "LogLevel"
  	, &Get().nLogLevel
  	, Get().nLogLevel
  	, "0:错误等级;1:常规等级;2:操作等级;3:调试等级"
  	, ""
  	, ""
  	, PROJECT_LEVEL
    );  
    
	DWORD dwLen = sizeof(szTemp);
	swpa_device_read_sn(szTemp, &dwLen);
	GetString("\\System[系统]"
		, "CommDevNo"
		, szTemp
		, ""
		, sizeof(szTemp)
		, "设备序列号"
		, ""
		, CUSTOM_LEVEL
		, TRUE
		);
/*		
	GetString("\\System[系统]"
		, "Version"
		, szTemp
		, ""
		, sizeof(szTemp)
		, "版本信息"
		, ""
		, CUSTOM_LEVEL
		, TRUE
		);
*/	
	INT iVersionLen = 512;
	char szVersion[iVersionLen];
	if( 0 != swpa_info_get_firmware_version(szVersion, &iVersionLen) )
	{
		SW_TRACE_DEBUG("<app>get firmware version failed!");
		szVersion[0] = 0;
	}
	else
	{
		CSWString str = szVersion;
		// kernel的版本太长，去掉不显示。
		// todo.
		INT iPos = str.Find("kernel_dm6467");
		CSWString strVersion;
		if( -1 != iPos )
		{
			strVersion = str.Substr(0, iPos);
		}
		swpa_strcpy(szVersion, (const CHAR*)strVersion);
		SW_TRACE_DEBUG("<app>firmware version:%s.", szVersion);
	}
	sprintf(szTemp
		, "Algorithm:%d\n"
		"BasicModule:%d\n"
		"SWFC:%d\n"
		"PlatformAPI:%d\n"
		"App:%d\n"
		"Config:%d\n"
		"FileSystem:%d\n"
		"Plugin:%d\n"
		, DSP_VERSION
		, MODULE_VERSION
		, SWFC_VERSION
		, SWPA_VERSION
		, APP_VERSION
		, CONFIG_VERSION
		, ROOTFS_VERSION
		, PLUGIN_VERSION
		);

    GetString("\\System[系统]\\Build[编译]"
    	, "Module No."
    	, szTemp
    	, szTemp
    	, sizeof(szTemp)
      , "模块版本号"
      , ""
      , CUSTOM_LEVEL
      , TRUE
    );
	GetString("\\System[系统]\\Build[编译]"
		, "Firmware No."
		, szVersion
		, szVersion
		, sizeof(szVersion)
		, "固件版本号"
		, ""
		, CUSTOM_LEVEL
		, TRUE
		);
		
	sprintf(szTemp
    , "%s%d%s" 
		, VERSIONHEADER
        , MAIN_VERSION
#ifdef USER_VERSION
        , USER_VERSION
#else
		,""
#endif
		);
		
    GetString("\\System[系统]\\Build[编译]"
    	, "No."
    	, szTemp
    	, szTemp
    	, sizeof(szTemp)
      , "软件版本号"
      , ""
      , CUSTOM_LEVEL
      , TRUE
    );
    UpdateString("\\System[系统]\\Build[编译]", "No.", szTemp);
    m_strVersion = szTemp;
      
    GetString("\\System[系统]\\TcpipCfg[网络配置]"
        , "CommIPAddr"
        , Get().cTcpipCfgParam.szIp
        , Get().cTcpipCfgParam.szIp
        , 31
        , "IP地址"
        , ""
        , CUSTOM_LEVEL
        , TRUE
    );
    
    GetString("\\System[系统]\\TcpipCfg[网络配置]"
    	, "CommMask"
    	, Get().cTcpipCfgParam.szNetmask
    	, Get().cTcpipCfgParam.szNetmask
    	, 31
    	, "子网掩码"
    	, ""
    	, CUSTOM_LEVEL
    	, TRUE
    );
    
    GetString("\\System[系统]\\TcpipCfg[网络配置]"
    	, "CommGateway"
    	, Get().cTcpipCfgParam.szGateway
    	, Get().cTcpipCfgParam.szGateway
    	, 31
    	, "网关"
    	, ""
    	, CUSTOM_LEVEL
    	, TRUE
    );

	char szIp[32];
	char szMask[32];
	char szGateway[32];
	char szMac[32];

	if( 0 == swpa_device_read_ipinfo(szIp, 32, szMask, 32, szGateway, 32) )
	{
		if( swpa_strcmp(szGateway, "0.0.0.0") == 0 )
		{
			swpa_strcpy(szGateway, "100.100.100.11");
		}
		SW_TRACE_DEBUG("<ip>SetIp:%s,%s,%s.\n", szIp, szMask, szGateway);
		if( 0 != swpa_tcpip_setinfo("eth0", szIp, szMask, szGateway) )
		{
			SW_TRACE_DEBUG("<ip>SetIp failed.:%s,%s,%s.\n", szIp, szMask, szGateway);
		}
		else if( swpa_strcmp(Get().cTcpipCfgParam.szIp, szIp) != 0
				|| swpa_strcmp(Get().cTcpipCfgParam.szNetmask, szMask) != 0
				|| swpa_strcmp(Get().cTcpipCfgParam.szGateway, szGateway) != 0 )
		{
			UpdateString("System[系统]\\TcpipCfg[网络配置]", "CommIPAddr", (const CHAR*)szIp);
			UpdateString("System[系统]\\TcpipCfg[网络配置]", "CommMask", (const CHAR*)szMask);
			UpdateString("System[系统]\\TcpipCfg[网络配置]", "CommGateway", (const CHAR*)szGateway);
		}

		if( 0 == swpa_tcpip_getinfo("eth0", szIp, 32, szMask, 32, szGateway, 32, szMac, 32) )
		{
			GetString("\\System[系统]\\TcpipCfg[网络配置]"
				, "CommMac"
				, szMac
				, szMac
				, 32
				, "MAC地址"
				, ""
				, CUSTOM_LEVEL
				, TRUE
				);
		}
	}
	// 默认要有个IP，否则设备会无法连接。
	else
	{
		swpa_tcpip_setinfo("eth0", "100.100.100.101", "255.0.0.0", "100.100.100.1");
	}

   return S_OK;
}

/**
 *@brief 
 */
HRESULT CSWLPRParameter::InitTracker(VOID)
{
	GetEnum("\\Tracker\\ProcessPlate"
		, "EnableDBGreenSegment"
		, &Get().cTrackerCfgParam.nPlateDetect_Green
		, Get().cTrackerCfgParam.nPlateDetect_Green
		, "0:关闭;1:打开"
    , "绿牌识别开关"
    , ""
    , CUSTOM_LEVEL
    );

  GetEnum("\\Tracker\\ProcessPlate"
  	, "LightBlueFlag"
  	, &Get().cTrackerCfgParam.nProcessPlate_LightBlue
  	, Get().cTrackerCfgParam.nProcessPlate_LightBlue
  	, "0:关闭;1:打开"
  	, "浅蓝牌开关"
  	, ""
  	, PROJECT_LEVEL
    );  
    
  GetEnum("\\Tracker\\ProcessPlate"
  	, "EnableBigPlate"
  	, &Get().cTrackerCfgParam.nEnableBigPlate
  	, Get().cTrackerCfgParam.nEnableBigPlate
  	, "0:关闭;1:打开"
  	, "大牌识别开关"
  	, ""
  	, PROJECT_LEVEL			//算法优化相关放到3级参数
    );

    GetEnum("\\Tracker\\ProcessPlate"
    	, "EnablePlateEnhance"
    	, &Get().cTrackerCfgParam.nEnablePlateEnhance
    	, Get().cTrackerCfgParam.nEnablePlateEnhance
    	, "0:关闭;1:打开"
    	, "分割前是否进行图片增强"
    	, ""
    	, PROJECT_LEVEL
    );

    GetEnum("\\Tracker\\ProcessPlate"
    	, "EnableProcessBWPlate"
    	, &Get().cTrackerCfgParam.nEnableProcessBWPlate
    	, Get().cTrackerCfgParam.nEnableProcessBWPlate
    	, "0:关闭;1:打开"
    	, "是否处理半黑半白牌"
    	, ""
    	, PROJECT_LEVEL
    );

    GetInt("\\Tracker\\ProcessPlate"
    	, "PlateResizeThreshold"
    	, &Get().cTrackerCfgParam.nPlateResizeThreshold
    	, Get().cTrackerCfgParam.nPlateResizeThreshold
    	, 0
    	, 200
    	, "分割前小图拉伸宽度阈值"
    	, ""
    	, PROJECT_LEVEL
    );

    GetInt("\\Tracker\\ProcessPlate\\BlackPlate"
    	, "NightThreshold"
    	,	&Get().cTrackerCfgParam.nNightThreshold
    	, Get().cTrackerCfgParam.nNightThreshold
    	, 0
    	, 240
    	, "夜晚模式亮度上限"
    	, ""
    	, CUSTOM_LEVEL
    );

    GetInt("\\Tracker\\ProcessPlate\\BlackPlate"
    	, "PlateLightCheckCount"
    	, &Get().cTrackerCfgParam.nPlateLightCheckCount
    	, Get().cTrackerCfgParam.nPlateLightCheckCount
    	, 3
    	, 50
    	, "亮度调整间隔"
    	, ""
    	, CUSTOM_LEVEL
    );

    GetInt("\\Tracker\\ProcessPlate\\BlackPlate"
    	, "MinPlateBrightness"
    	, &Get().cTrackerCfgParam.nMinPlateBrightness
    	, Get().cTrackerCfgParam.nMinPlateBrightness
    	, 1
    	, 255
    	, "车牌最低亮度"
    	, ""
    	, CUSTOM_LEVEL
    );
		
    GetInt("\\Tracker\\ProcessPlate\\BlackPlate"
    	, "MaxPlateBrightness"
    	, &Get().cTrackerCfgParam.nMaxPlateBrightness
    	, Get().cTrackerCfgParam.nMaxPlateBrightness
    	, 1
    	, 255
    	, "车牌最高亮度"
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
    	, PROJECT_LEVEL
    );

    GetInt("\\Tracker\\ProcessPlate\\BlackPlate"
    	, "H1"
    	, &Get().cTrackerCfgParam.nProcessPlate_BlackPlateThreshold_H1
    	, Get().cTrackerCfgParam.nProcessPlate_BlackPlateThreshold_H1
    	, Get().cTrackerCfgParam.nProcessPlate_BlackPlateThreshold_H0
    	, 240
    	, "蓝牌色度上限"
    	, ""
    	, PROJECT_LEVEL
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
    	, PROJECT_LEVEL
    );

    GetEnum("Tracker\\Recognition"
    	, "UseEdgeMethod"
    	, &Get().cTrackerCfgParam.fUseEdgeMethod
    	, Get().cTrackerCfgParam.fUseEdgeMethod
    	, "0:不加强;1:加强"
    	, "加强清晰图识别"
    	, ""
    	, PROJECT_LEVEL
    );
    
    GetEnum("Tracker\\Recognition"
    	, "EnableT1Model"
    	, &Get().cTrackerCfgParam.fEnableT1Model
    	, Get().cTrackerCfgParam.fEnableT1Model
    	, "0:关闭;1:打开"
    	, "T-1模型开关"
    	, ""
    	, PROJECT_LEVEL
    );

    GetEnum("Tracker\\Misc"
    	, "EnableAlphaRecog"
    	, &Get().cTrackerCfgParam.fEnableAlpha_5
    	, Get().cTrackerCfgParam.fEnableAlpha_5
    	, "0:关闭;1:打开"
    	, "黄牌字母识别开关"
    	, ""
    	, CUSTOM_LEVEL
    );
    
    GetInt("Tracker\\Recognition"
    	, "RecogAsteriskThreshold"
    	, &Get().cTrackerCfgParam.nRecogAsteriskThreshold
    	, Get().cTrackerCfgParam.nRecogAsteriskThreshold
    	, 0
    	, 100
    	, "得分低于阈值(百分比)用#表示"
    	, ""
    	, PROJECT_LEVEL
    );
    
    GetEnum("Tracker\\Misc"
    	, "RecogGXPolice"
    	, &Get().cTrackerCfgParam.nRecogGxPolice
    	, Get().cTrackerCfgParam.nRecogGxPolice
    	, "0:关闭;1:打开"
    	, "地方警牌识别开关"
    	, ""
    	, PROJECT_LEVEL
    );
    
    GetInt("Tracker\\Misc"
    	, "CarArrivedPos"
    	, &Get().cTrackerCfgParam.nCarArrivedPos
    	, Get().cTrackerCfgParam.nCarArrivedPos
    	, 0
    	, 100
    	, "车辆到达位置(除黄牌外所有牌)"
    	, ""
    	, CUSTOM_LEVEL
    );
    
    GetInt("Tracker\\Misc"
    	, "CarArrivedPosNoPlate"
    	, &Get().cTrackerCfgParam.nCarArrivedPosNoPlate
    	, Get().cTrackerCfgParam.nCarArrivedPosNoPlate
    	, 0
    	, 100
    	, "无牌车到达位置"
    	, ""
    	, CUSTOM_LEVEL
    );
    
    GetInt("Tracker\\Misc"
    	, "CarArrivedPosYellow"
    	, &Get().cTrackerCfgParam.nCarArrivedPosYellow
    	, Get().cTrackerCfgParam.nCarArrivedPosYellow
    	, 0
    	, 100
    	, "车辆到达位置(黄牌)"
    	, ""
    	, CUSTOM_LEVEL
    );
    
    GetEnum("Tracker\\Misc"
    	, "CoreProcessType"
    	, &Get().cTrackerCfgParam.nProcessType
    	, Get().cTrackerCfgParam.nProcessType
    	, "0:只区分大中小型车;1:隧道内区分非机动和行人;2:隧道外区分非机动车和行人"
    	, "处理类型"
    	, ""
    	, CUSTOM_LEVEL
    );
    
    GetEnum("Tracker\\Misc"
    	, "CoreOutPutType"
    	, &Get().cTrackerCfgParam.nOutPutType
    	, Get().cTrackerCfgParam.nOutPutType
    	, "0:全类型输出;1:大中小型车输出为机动机"
    	, "输出类型 0:全类型输出;1:大中小型车输出为机动机"
    	, ""
    	, CUSTOM_LEVEL
    );
    
    GetEnum("Tracker\\Misc"
    	, "NightPlus"
    	, &Get().cTrackerCfgParam.nNightPlus
    	, Get().cTrackerCfgParam.nNightPlus
    	, "0:关闭;1:打开"
    	, "晚上加强非机动车检测,只有场景足够亮时可用,否则会增加多检"
    	, ""
    	, PROJECT_LEVEL
    );
    
    GetInt("Tracker\\Misc"
    	, "WalkManSpeed"
    	, &Get().cTrackerCfgParam.nWalkManSpeed
    	, Get().cTrackerCfgParam.nWalkManSpeed
    	, 0
    	, 300
    	, "行人判断的灵敏度"
    	, ""
    	, PROJECT_LEVEL
    );
    
    GetFloat("Tracker\\Misc"
    	, "BikeSensitivity"
    	, &Get().cTrackerCfgParam.fltBikeSensitivity
    	, Get().cTrackerCfgParam.fltBikeSensitivity
    	, 0.0f
    	, 10.0f
    	, "非机动车灵敏度"
    	, ""
    	, PROJECT_LEVEL
    );
    
    GetInt("Tracker\\Misc"
    	, "CarArrivedDelay"
    	, &Get().cTrackerCfgParam.nCarArrivedDelay
    	, Get().cTrackerCfgParam.nCarArrivedDelay
    	, 0
    	, 100
    	, "车辆到达触发延迟距离(米)"
    	, ""
    	, PROJECT_LEVEL
    );
    
    GetInt("Tracker\\TrackInfo"
    	, "VoteFrameNum"
    	, &Get().cTrackerCfgParam.nVoteFrameNum
    	, Get().cTrackerCfgParam.nVoteFrameNum
    	, 4
    	, 1000
    	, "投票的结果数"
    	, ""
    	, CUSTOM_LEVEL
    );
    
    GetInt("Tracker\\TrackInfo"
    	, "MaxEqualFrameNumForVote"
    	, &Get().cTrackerCfgParam.nMaxEqualFrameNumForVote
    	, Get().cTrackerCfgParam.nMaxEqualFrameNumForVote
    	, 4, 1000
    	, "连续相同结果出牌条件"
    	, ""
    	, CUSTOM_LEVEL
    );
    
    GetInt("Tracker\\TrackInfo"
    	, "BlockTwinsTimeout"
    	, &Get().cTrackerCfgParam.nBlockTwinsTimeout
    	, Get().cTrackerCfgParam.nBlockTwinsTimeout
    	, 0
    	, 3600
    	, "相同结果最小时间间隔(S)"
    	, ""
    	, CUSTOM_LEVEL
    );
    
    GetInt("Tracker\\TrackInfo"
    	, "AverageConfidenceQuan"
    	, &Get().cTrackerCfgParam.nAverageConfidenceQuan
    	, Get().cTrackerCfgParam.nAverageConfidenceQuan
    	, 0
    	, 65536
    	, "平均得分下限"
    	, ""
    	, 1
    );
    
    GetInt("Tracker\\TrackInfo"
    	, "FirstConfidenceQuan"
    	, &Get().cTrackerCfgParam.nFirstConfidenceQuan
    	, Get().cTrackerCfgParam.nFirstConfidenceQuan
    	, 0
    	, 65536
    	, "汉字得分下限"
    	, ""
    	, PROJECT_LEVEL
    );
    
    GetInt("Tracker\\TrackInfo"
    	, "RemoveLowConfForVote"
    	, &Get().cTrackerCfgParam.nRemoveLowConfForVote
    	, Get().cTrackerCfgParam.nRemoveLowConfForVote
    	, 0
    	, 100
    	, "投票前去掉低得分车牌百分比"
    	, ""
    	, PROJECT_LEVEL
    );
    
    //初始化梯形区域
    Get().cTrackerCfgParam.cTrapArea.TopLeftX =     Get().cTrackerCfgParam.cDetectArea.DetectorAreaLeft;
    Get().cTrackerCfgParam.cTrapArea.TopRightX =    Get().cTrackerCfgParam.cDetectArea.DetectorAreaRight;
    Get().cTrackerCfgParam.cTrapArea.BottomLeftX =  Get().cTrackerCfgParam.cDetectArea.DetectorAreaLeft;
    Get().cTrackerCfgParam.cTrapArea.BottomRightX = Get().cTrackerCfgParam.cDetectArea.DetectorAreaRight;

    Get().cTrackerCfgParam.cTrapArea.TopLeftY =     Get().cTrackerCfgParam.cDetectArea.DetectorAreaTop;
    Get().cTrackerCfgParam.cTrapArea.TopRightY =    Get().cTrackerCfgParam.cDetectArea.DetectorAreaTop;
    Get().cTrackerCfgParam.cTrapArea.BottomLeftY =  Get().cTrackerCfgParam.cDetectArea.DetectorAreaBottom;
    Get().cTrackerCfgParam.cTrapArea.BottomRightY = Get().cTrackerCfgParam.cDetectArea.DetectorAreaBottom;

    Get().cTrackerCfgParam.cTrapArea.nDetectorMinScaleNum = Get().cTrackerCfgParam.cDetectArea.nDetectorMinScaleNum;
    Get().cTrackerCfgParam.cTrapArea.nDetectorMaxScaleNum = Get().cTrackerCfgParam.cDetectArea.nDetectorMaxScaleNum;

    GetEnum("Tracker\\DetAreaCtrl\\Normal"
    	,"EnableTrap"
    	, &Get().cTrackerCfgParam.cTrapArea.fEnableDetAreaCtrl
    	, Get().cTrackerCfgParam.cTrapArea.fEnableDetAreaCtrl
    	, "0:关闭;1:打开"
    	, "梯形扫描区域控制开关"
    	, ""
    	, CUSTOM_LEVEL
    );

    GetInt("Tracker\\DetAreaCtrl\\Normal"
    	, "TopLeftX"
    	, &Get().cTrackerCfgParam.cTrapArea.TopLeftX
    	, Get().cTrackerCfgParam.cTrapArea.TopLeftX
    	, 0
    	, 100
    	, "扫描左上区域坐标X"
    	, ""
    	, CUSTOM_LEVEL
    );
    
    GetInt("Tracker\\DetAreaCtrl\\Normal"
    	, "TopLeftY" 
    	, &Get().cTrackerCfgParam.cTrapArea.TopLeftY
    	, Get().cTrackerCfgParam.cTrapArea.TopLeftY
    	, 0
    	, 100
    	, "扫描左上区域坐标Y"
    	, ""
    	, CUSTOM_LEVEL
    );
    
    GetInt("Tracker\\DetAreaCtrl\\Normal"
    	, "TopRightX"
    	, &Get().cTrackerCfgParam.cTrapArea.TopRightX
    	, Get().cTrackerCfgParam.cTrapArea.TopRightX
    	, 0
    	, 100
    	, "扫描右上区域坐标X"
    	, ""
    	, CUSTOM_LEVEL
    );
    
    GetInt("Tracker\\DetAreaCtrl\\Normal"
    	, "TopRightY"
    	, &Get().cTrackerCfgParam.cTrapArea.TopRightY
    	, Get().cTrackerCfgParam.cTrapArea.TopRightY
    	, 0
    	, 100
    	, "扫描右上区域坐标Y"
    	, ""
    	, CUSTOM_LEVEL
    );

    GetInt("Tracker\\DetAreaCtrl\\Normal","BottomLeftX"
    	, &Get().cTrackerCfgParam.cTrapArea.BottomLeftX
    	, Get().cTrackerCfgParam.cTrapArea.BottomLeftX
    	, 0 
    	, 100
    	, "扫描左下区域坐标X"
    	, ""
    	, CUSTOM_LEVEL
    );
    
    GetInt("Tracker\\DetAreaCtrl\\Normal"
    	, "BottomLeftY"
    	, &Get().cTrackerCfgParam.cTrapArea.BottomLeftY
    	, Get().cTrackerCfgParam.cTrapArea.BottomLeftY
    	, 0
    	, 100
    	, "扫描左下区域坐标Y"
    	, ""
    	, CUSTOM_LEVEL
    );
    
    GetInt("Tracker\\DetAreaCtrl\\Normal"
    	, "BottomRightX"
    	, &Get().cTrackerCfgParam.cTrapArea.BottomRightX
    	, Get().cTrackerCfgParam.cTrapArea.BottomRightX
    	, 0
    	, 100
    	, "扫描右下区域坐标X"
    	, ""
    	, CUSTOM_LEVEL
    );
    
    GetInt("Tracker\\DetAreaCtrl\\Normal"
    	, "BottomRightY"
    	, &Get().cTrackerCfgParam.cTrapArea.BottomRightY
    	, Get().cTrackerCfgParam.cTrapArea.BottomRightY
    	, 0
    	, 100
    	, "扫描右下区域坐标Y"
    	, ""
    	, CUSTOM_LEVEL
    );
    
    GetInt("\\Tracker\\DetAreaCtrl\\Normal"
    	, "MinScale"
    	, &Get().cTrackerCfgParam.cDetectArea.nDetectorMinScaleNum
    	, Get().cTrackerCfgParam.cDetectArea.nDetectorMinScaleNum
    	, 0
    	, 16
    	, "检测的最小宽度"
    	, "56*(1.1^MinScale)"
    	, CUSTOM_LEVEL
    );
    GetInt("\\Tracker\\DetAreaCtrl\\Normal"
    	, "MaxScale"
    	, &Get().cTrackerCfgParam.cDetectArea.nDetectorMaxScaleNum
    	, Get().cTrackerCfgParam.cDetectArea.nDetectorMaxScaleNum
    	, 0
    	, 16
    	, "检测的最大宽度"
    	, "56*(1.1^MaxScale)"
    	, CUSTOM_LEVEL
    );
    
    Get().cTrackerCfgParam.cDetectArea.fEnableDetAreaCtrl = Get().cTrackerCfgParam.cTrapArea.fEnableDetAreaCtrl;
    Get().cTrackerCfgParam.cDetectArea.DetectorAreaLeft   = Get().cTrackerCfgParam.cTrapArea.TopLeftX    > Get().cTrackerCfgParam.cTrapArea.BottomLeftX  ? Get().cTrackerCfgParam.cTrapArea.BottomLeftX : Get().cTrackerCfgParam.cTrapArea.TopLeftX;
    Get().cTrackerCfgParam.cDetectArea.DetectorAreaRight  = Get().cTrackerCfgParam.cTrapArea.TopRightX   < Get().cTrackerCfgParam.cTrapArea.BottomRightX ? Get().cTrackerCfgParam.cTrapArea.BottomRightX: Get().cTrackerCfgParam.cTrapArea.TopRightX;
    Get().cTrackerCfgParam.cDetectArea.DetectorAreaTop    = Get().cTrackerCfgParam.cTrapArea.TopLeftY    > Get().cTrackerCfgParam.cTrapArea.TopRightY    ? Get().cTrackerCfgParam.cTrapArea.TopRightY   : Get().cTrackerCfgParam.cTrapArea.TopLeftY;
    Get().cTrackerCfgParam.cDetectArea.DetectorAreaBottom = Get().cTrackerCfgParam.cTrapArea.BottomLeftY > Get().cTrackerCfgParam.cTrapArea.BottomRightY ? Get().cTrackerCfgParam.cTrapArea.BottomLeftY : Get().cTrackerCfgParam.cTrapArea.BottomRightY;

    GetInt("\\Tracker\\RoadInfo"
    	, "RoadLineNumber"
    	, &Get().cTrackerCfgParam.nRoadLineNumber
    	, Get().cTrackerCfgParam.nRoadLineNumber
    	, 2
        , MAX_ROADLINE_NUM
    	, "车道线数量"
    	, ""
    	, CUSTOM_LEVEL
    );
    
    GetEnum("\\Tracker\\RoadInfo"
    	, "RoadNumberBegin"
    	, &Get().cTrackerCfgParam.iRoadNumberBegin 
    	, Get().cTrackerCfgParam.iRoadNumberBegin
    	, "0:从左开始;1:从右开始"
      , "车道编号起始方向"
      , ""
      , CUSTOM_LEVEL
    );

    GetEnum("\\Tracker\\RoadInfo"
    	, "StartRoadNumber"
    	, &Get().cTrackerCfgParam.iStartRoadNum
    	, Get().cTrackerCfgParam.iStartRoadNum
    	, "0:0;1:1;2:2;3:3;4:4;5:5;6:6;7:7;8:8;9:9"
    	, "车道号起始编号"
    	, "0:从0开始,1:从1开始,以此类推..."
    	, CUSTOM_LEVEL
    );

    for (int i = 0; i < MAX_ROADLINE_NUM; i++)
    {
        char szSection[256];
        sprintf(szSection, "\\Tracker\\RoadInfo\\Road%02d", i);

        GetInt(szSection
        	, "TopX"
        	, &Get().cTrackerCfgParam.rgcRoadInfo[i].ptTop.x
        	, Get().cTrackerCfgParam.rgcRoadInfo[i].ptTop.x
        	, 0
        	, 5000
        	, "顶X坐标"
        	, ""
        	, CUSTOM_LEVEL
        );
        
        GetInt(szSection
        	, "TopY"
        	, &Get().cTrackerCfgParam.rgcRoadInfo[i].ptTop.y
        	, Get().cTrackerCfgParam.rgcRoadInfo[i].ptTop.y
        	, 0
        	, 5000
        	, "顶Y坐标"
        	, ""
        	, CUSTOM_LEVEL
        );
        
        GetInt(szSection
        	, "BottomX"
        	, &Get().cTrackerCfgParam.rgcRoadInfo[i].ptBottom.x
        	, Get().cTrackerCfgParam.rgcRoadInfo[i].ptBottom.x
        	, 0
        	, 5000
        	, "底X坐标"
        	, ""
        	, CUSTOM_LEVEL
        );
        
        GetInt(szSection
        	, "BottomY"
        	, &Get().cTrackerCfgParam.rgcRoadInfo[i].ptBottom.y
        	, Get().cTrackerCfgParam.rgcRoadInfo[i].ptBottom.y
        	, 0
        	, 5000
        	, "底Y坐标"
        	, ""
        	, CUSTOM_LEVEL
        );
        
        GetInt(szSection
        	, "LineType"
        	, &Get().cTrackerCfgParam.rgcRoadInfo[i].iLineType
        	, Get().cTrackerCfgParam.rgcRoadInfo[i].iLineType
        	, 1
        	, 9999
        	, "类型"
        	, ""
        	, CUSTOM_LEVEL
        );
        
        GetInt(szSection
        	, "RoadType"
        	, &Get().cTrackerCfgParam.rgcRoadInfo[i].iRoadType
        	, Get().cTrackerCfgParam.rgcRoadInfo[i].iRoadType
        	, 1
        	, 9999
        	, "对应的车道类型"
        	, ""
        	, CUSTOM_LEVEL
        );
    }
    GetEnum("\\Tracker\\VideoDet"
    	, "VideoDetMode"
    	, &Get().cTrackerCfgParam.cVideoDet.nVideoDetMode
    	, Get().cTrackerCfgParam.cVideoDet.nVideoDetMode
    	, "0:不使用视频检测;1:只使用背景检测;2:使用视频检测"
    	, "视频检测模式"
    	, ""
    	, CUSTOM_LEVEL
    );
    
    GetInt("\\Tracker\\VideoDet\\AreaCtrl"
    	, "Left"
    	, &Get().cTrackerCfgParam.cVideoDet.rcVideoDetArea.left
    	, Get().cTrackerCfgParam.cVideoDet.rcVideoDetArea.left
    	, 0
    	, 100
    	, "视频检测左区域"
    	, ""
    	, CUSTOM_LEVEL
    );
    
    GetInt("\\Tracker\\VideoDet\\AreaCtrl"
    	, "Right"
    	, &Get().cTrackerCfgParam.cVideoDet.rcVideoDetArea.right
    	, Get().cTrackerCfgParam.cVideoDet.rcVideoDetArea.right
    	, 0
    	, 100
    	, "视频检测右区域"
    	, ""
    	, CUSTOM_LEVEL
    );
    
    GetInt("\\Tracker\\VideoDet\\AreaCtrl"
    	, "Top"
    	, &Get().cTrackerCfgParam.cVideoDet.rcVideoDetArea.top
    	, Get().cTrackerCfgParam.cVideoDet.rcVideoDetArea.top
    	, 0
    	, 100
    	, "视频检测上区域"
    	, ""
    	, CUSTOM_LEVEL
    );
    
    GetInt("\\Tracker\\VideoDet\\AreaCtrl"
    	, "Bottom"
    	, &Get().cTrackerCfgParam.cVideoDet.rcVideoDetArea.bottom
    	, Get().cTrackerCfgParam.cVideoDet.rcVideoDetArea.bottom
    	, 0
    	, 100
    	, "视频检测下区域"
    	, ""
    	, CUSTOM_LEVEL
    );
    
    GetInt("\\Tracker\\VideoDet"
    	, "DayBackgroudThreshold"
    	, &Get().cTrackerCfgParam.cVideoDet.nDayBackgroudThreshold
    	, Get().cTrackerCfgParam.cVideoDet.nDayBackgroudThreshold
    	, 0
    	, 255
    	, "白天背景检测阈值"
    	, ""
    	, PROJECT_LEVEL
    );
    
    GetInt("\\Tracker\\VideoDet"
    	, "nBackgroudThreshold"
    	, &Get().cTrackerCfgParam.cVideoDet.nNightBackgroudThreshold
    	, Get().cTrackerCfgParam.cVideoDet.nNightBackgroudThreshold
    	, 0
    	, 255
    	, "晚上背景检测阈值"
    	, ""
    	, PROJECT_LEVEL
    ); 
    
    GetInt("\\Tracker\\VideoDet"
    	, "VoteFrameCount"
    	, &Get().cTrackerCfgParam.cVideoDet.nVoteFrameCount
    	, Get().cTrackerCfgParam.cVideoDet.nVoteFrameCount
    	, 15
    	, 100
    	, "投票有效帧数，当帧数大于此值就投票"
    	, ""
    	, PROJECT_LEVEL
    );
    
    GetEnum("\\Tracker\\VideoDet"
    	, "CheckAcross"
    	, &Get().cTrackerCfgParam.cVideoDet.nCheckAcross
    	, Get().cTrackerCfgParam.cVideoDet.nCheckAcross
    	, "0:否;1:是"
    	, "是否检测横向行驶的物体"
    	, ""
    	, PROJECT_LEVEL
    );
    
    GetEnum("\\Tracker\\VideoDet"
    	, "ShadowDetMode"
    	, &Get().cTrackerCfgParam.cVideoDet.nShadowDetMode
    	, Get().cTrackerCfgParam.cVideoDet.nShadowDetMode
    	, "0:不使用阴影检测;1:使用阴影检测"
    	, "阴影检测模式"
    	, ""
    	, PROJECT_LEVEL
    );
    
    GetInt("\\Tracker\\VideoDet"
    	, "ShadowDetEffect"
    	, &Get().cTrackerCfgParam.cVideoDet.nShadowDetEffect
    	, Get().cTrackerCfgParam.cVideoDet.nShadowDetEffect
    	, 0
    	, 100
    	, "阴影检测强弱程度值"
    	, "0:使用缺省值(不精确), 1:阴影检测强弱程度最小, 100:阴影检测强弱程度最大"
    	, PROJECT_LEVEL
    );
    
    GetEnum("\\Tracker\\VideoDet"
    	, "DiffType"
    	, &Get().cTrackerCfgParam.cVideoDet.nDiffType
    	, Get().cTrackerCfgParam.cVideoDet.nDiffType
    	, "0:帧差提取;1:帧差后sobel前景"
    	, "前景提取方式"
    	, "值为0:帧差提取 值为1:帧差后sobel前景，针对高速路卡口两车道车身多检"
    	, PROJECT_LEVEL
    );
    
    GetEnum("\\Tracker\\VideoDet"
    	, "MedFilter"
    	, &Get().cTrackerCfgParam.cVideoDet.nMedFilter
    	, Get().cTrackerCfgParam.cVideoDet.nMedFilter
    	, "0:关闭;1:打开"
    	, "抑制抖动开关"
    	, ""
    	, PROJECT_LEVEL
    );
    
    GetInt("\\Tracker\\VideoDet"
    	, "MedFilterUpLine"
    	, &Get().cTrackerCfgParam.cVideoDet.nMedFilterUpLine
    	, Get().cTrackerCfgParam.cVideoDet.nMedFilterUpLine
    	, 0
    	, 100
    	, "抑制抖动作用区域上区域"
    	, "值为0:屏幕最上方 值为100:屏幕最下方"
    	, PROJECT_LEVEL
    );
    
    GetInt("\\Tracker\\VideoDet"
    	, "MedFilterDownLine"
    	, &Get().cTrackerCfgParam.cVideoDet.nMedFilterDownLine
    	, Get().cTrackerCfgParam.cVideoDet.nMedFilterDownLine
    	, 0
    	, 100
    	, "抑制抖动作用区域下区域"
    	, "值为0:屏幕最上方 值为100:屏幕最下方"
    	, PROJECT_LEVEL
    );
  
  //Scale有效性判断
  if (Get().cTrackerCfgParam.cDetectArea.nDetectorMinScaleNum > Get().cTrackerCfgParam.cDetectArea.nDetectorMaxScaleNum)
  {
      UpdateInt("\\Tracker\\DetAreaCtrl\\Normal"
      	, "MinScale"
      	, 6
      );
      UpdateInt("\\Tracker\\DetAreaCtrl\\Normal"
      	, "MaxScale"
      	, 10
      );
      
      Get().cTrackerCfgParam.cDetectArea.nDetectorMinScaleNum = 5;
      Get().cTrackerCfgParam.cDetectArea.nDetectorMaxScaleNum = 9;
  }

  //视频检测区域有效性判断
  if (!CheckArea(
              Get().cTrackerCfgParam.cVideoDet.rcVideoDetArea.left,
              Get().cTrackerCfgParam.cVideoDet.rcVideoDetArea.top,
              Get().cTrackerCfgParam.cVideoDet.rcVideoDetArea.right,
              Get().cTrackerCfgParam.cVideoDet.rcVideoDetArea.bottom
          ))
  {
      UpdateInt("\\Tracker\\VideoDet\\AreaCtrl"
      	, "Left"
      	, 0
      );
      UpdateInt("\\Tracker\\VideoDet\\AreaCtrl"
      	, "Top"
      	, 0
      );
      UpdateInt("\\Tracker\\VideoDet\\AreaCtrl"
      	, "Right"
      	, 100
      );
      UpdateInt("\\Tracker\\VideoDet\\AreaCtrl"
      	, "Bottom"
      	, 100
      );
	
      Get().cTrackerCfgParam.cVideoDet.rcVideoDetArea.left = 0;
      Get().cTrackerCfgParam.cVideoDet.rcVideoDetArea.top =0;
      Get().cTrackerCfgParam.cVideoDet.rcVideoDetArea.right = 100;
      Get().cTrackerCfgParam.cVideoDet.rcVideoDetArea.bottom = 100;
  }
	return S_OK;
}

HRESULT CSWLPRParameter::InitScaleSpeed(VOID)
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
			, "0:平面标定法;1:空间标定法"
			, "计算模式"
			, ""
			, CUSTOM_LEVEL
			);
	
	GetFloat("\\Tracker\\ScaleSpeed"
		, "LeftDistance"
		, &Get().cTrackerCfgParam.cScaleSpeed.fltLeftDistance
		, Get().cTrackerCfgParam.cScaleSpeed.fltLeftDistance
		, 0.01f
		, 10000.0f
		, "屏幕下沿到龙门架距离"
		, ""
		, CUSTOM_LEVEL
		);
	GetFloat("\\Tracker\\ScaleSpeed"
		, "FullDistance"
		, &Get().cTrackerCfgParam.cScaleSpeed.fltFullDistance
		, Get().cTrackerCfgParam.cScaleSpeed.fltFullDistance
		, 0.01f
		, 10000.0f
		, "屏幕上沿到龙门架距离"
		, ""
		, CUSTOM_LEVEL
		);
	GetFloat("\\Tracker\\ScaleSpeed"
		, "CameraHigh"
		, &Get().cTrackerCfgParam.cScaleSpeed.fltCameraHigh
		, Get().cTrackerCfgParam.cScaleSpeed.fltCameraHigh
		, 0.01f
		, 10000.0f
		, "摄像机高度"
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


BOOL CSWLPRParameter::CheckArea(int nLeft, int nTop, int nRight, int nBottom)
{
    if (nLeft < 0 || nTop < 0 || nRight < 0 || nBottom < 0
            || nLeft > 100 || nTop > 100 || nRight > 100 || nBottom > 100
            || (nLeft >= nRight) || (nTop >= nBottom))
    {
        return FALSE;
    }
    return TRUE;
}
/**
 *@brief 
 */
HRESULT CSWLPRParameter::InitHvDsp(VOID)
{/*
    GetEnum( "\\HvDsp\\Misc"
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

    GetEnum("\\HvDsp\\Misc"
    	, "OutputCarArriveTime"
    	, &Get().cResultSenderParam.fOutputCarArriveTime
    	, Get().cResultSenderParam.fOutputCarArriveTime
    	, "0:不输出;1:输出"
    	, "输出检测时间开关"
    	, ""
    	, PROJECT_LEVEL
    );
*/
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
/*
    GetInt("\\HvDsp\\Misc"
    	, "VideoDisplayTime"
    	, &Get().cResultSenderParam.iVideoDisplayTime
    	, Get().cResultSenderParam.iVideoDisplayTime
    	, 1
    	, 1000000
    	, "发送视频时间间隔(MS)"
    	, ""
    	, CUSTOM_LEVEL
    );
*/
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
   		, "FlashDifferentLane"
   		, &Get().cResultSenderParam.iFlashDifferentLane
   		, Get().cResultSenderParam.iFlashDifferentLane
   		, "0:不分车道闪;1:分车道闪"
   		, "闪光灯分车道闪（软触发）"
   		, ""
   		, CUSTOM_LEVEL
   	);

    GetEnum("\\HvDsp\\Misc"
        , "FlashOneByOne"
        , &Get().cResultSenderParam.iFlashOneByOne
        , Get().cResultSenderParam.iFlashOneByOne
        , "0:关闭;1:开启"
        , "闪光灯轮闪"
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

    GetFloat("\\HvDsp\\EventChecker"
    	, "OverLineSensitivity"
    	, &Get().cTrackerCfgParam.fltOverLineSensitivity
    	, Get().cTrackerCfgParam.fltOverLineSensitivity
    	, 0.5
    	, 5.0
    	, "压线检测灵敏度"
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

	return S_OK;
}

HRESULT CSWLPRParameter::InitIPTCtrl(VOID)
{
	CSWString strCOMM;
	for(int i = 0; i < 2; i++)
	{
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
		  	  	, "0:无;1:川速雷达;2:苏江车检器;3:康耐德红绿灯转换器;4:奥利维亚雷达;5:四川九洲雷达"
		  	  	, "外部设备类型"
		  	  	, ""
		  	  	, CUSTOM_LEVEL
		  		);
		GetInt((LPCSTR)strCOMM
				, "Road"
				, &Get().cDevParam[i].iRoadNo
				, Get().cDevParam[i].iRoadNo
				, 0
				, 255
				, "所属车道编号"
				, "所属车道编号"
				, CUSTOM_LEVEL
				);
			
		GetEnum((LPCSTR)strCOMM
				, "CommType"
				, &Get().cDevParam[i].iCommType
				, Get().cDevParam[i].iCommType
				, "0:RS232;1:RS485"
				, "串口类型"
				, ""
				, CUSTOM_LEVEL
				);
  	  
  	//线圈的属性
  		GetInt((LPCSTR)strCOMM
				, "CoilInstance"
				, &Get().cDevParam[i].cCoilParam.iTowCoilInstance
				, Get().cDevParam[i].cCoilParam.iTowCoilInstance
				, 0
				, 100
				, "2个线圈的距离(分米)"
				, "线圈距离"
				, CUSTOM_LEVEL
				);
		
		GetEnum((LPCSTR)strCOMM
		  	  	, "CoilEgdeType"
		  	  	, &Get().cDevParam[i].cCoilParam.iCoilEgdeType
		  	  	, Get().cDevParam[i].cCoilParam.iCoilEgdeType
		  	  	, "0:上升沿;1:下降沿"
		  	  	, "触发类型"
		  	  	, ""
		  	  	, CUSTOM_LEVEL
		  		);
  	
  		GetInt((LPCSTR)strCOMM
		  	  	, "Trigger"
		  	  	, (INT *)&Get().cDevParam[i].cCoilParam.bTrigger
		  	  	, (INT)Get().cDevParam[i].cCoilParam.bTrigger
		  	  	, 0
		  	  	, 15
		  	  	, "触发抓拍"
		  	  	, "1:线圈1触发抓拍;2:线圈2触发抓拍;4:线圈3触发抓拍;8:线圈4触发抓拍"
		  	  	, CUSTOM_LEVEL
		  		);
  }    
	return S_OK;
}

HRESULT CSWLPRParameter::InitOuterCtrl(VOID)
{
	GetEnum("\\OuterCtrl[外总控]"
    	, "OutPutPlateMode"
    	, &Get().cMatchParam.fEnable
    	, Get().cMatchParam.fEnable
    	, "0:关闭;1:打开"
    	, "外总控使能开关"
    	, ""
    	, CUSTOM_LEVEL
    );
    
	GetInt("\\OuterCtrl[外总控]"
		, "SignalHoldTime"
		, (INT *)&Get().cMatchParam.dwSignalKeepTime
		, Get().cMatchParam.dwSignalKeepTime
		, 500
		, 600000
		, "信号保留时间"
		, ""
		, CUSTOM_LEVEL
	);
	
	GetInt("\\OuterCtrl[外总控]"
		, "PlateHoldTime"
		, (INT *)&Get().cMatchParam.dwPlateKeepTime
		, Get().cMatchParam.dwPlateKeepTime
		, 500
		, 600000
		, "车牌保留时间"
		, ""
		, CUSTOM_LEVEL
	);
	
	GetInt("\\OuterCtrl[外总控]"
		, "MatchMinTime"
		, (INT *)&Get().cMatchParam.dwMatchMinTime
		, Get().cMatchParam.dwMatchMinTime
		, 0
		, 600000
		, "车牌向前匹配信号最大时间差"
		, ""
		, CUSTOM_LEVEL
	);
	
	GetInt("\\OuterCtrl[外总控]"
		, "MatchMaxTime"
		, (INT *)&Get().cMatchParam.dwMatchMaxTime
		, Get().cMatchParam.dwMatchMaxTime
		, 0
		, 600000
		, "车牌向后匹配信号最大时间差"
		, ""
		, CUSTOM_LEVEL
	);

	GetEnum("\\OuterCtrl[外总控]"
		, "RecogSnapImg"
		, &Get().cMatchParam.fCaptureRecong
		, Get().cMatchParam.fCaptureRecong
		, "0:关闭;1:打开"
		, "抓拍图片识别开关"
		, ""
		, CUSTOM_LEVEL
		);
	if(Get().cTrackerCfgParam.fEnableDetFace)
	{
		Get().cMatchParam.fCaptureRecong = TRUE;
	}

	CHAR szRecogArea[64] = {0};
	swpa_strcpy(szRecogArea, "[0,50,100,100],6,12");
	GetString("\\OuterCtrl[外总控]"
		, "RecogArea"
		, szRecogArea
		, szRecogArea
		, 64
		, "抓拍识别区域(%)[左,上,右,下],最小车牌，最大车牌"
		, "抓拍识别区域(%)[左,上,右,下],最小车牌，最大车牌"
		, CUSTOM_LEVEL
		);
	swpa_sscanf(
		szRecogArea,
		"[%d,%d,%d,%d],%d,%d",
		&Get().cMatchParam.iDetectorAreaLeft,
		&Get().cMatchParam.iDetectorAreaTop,
		&Get().cMatchParam.iDetectorAreaRight,
		&Get().cMatchParam.iDetectorAreaBottom,
		&Get().cMatchParam.iDetectorMinScaleNum,
		&Get().cMatchParam.iDetectorMaxScaleNum
		);

	Get().cTrackerCfgParam.cRecogSnapArea.fEnableDetAreaCtrl = TRUE;
	Get().cTrackerCfgParam.cRecogSnapArea.DetectorAreaLeft = Get().cMatchParam.iDetectorAreaLeft;
	Get().cTrackerCfgParam.cRecogSnapArea.DetectorAreaTop = Get().cMatchParam.iDetectorAreaTop;
	Get().cTrackerCfgParam.cRecogSnapArea.DetectorAreaRight = Get().cMatchParam.iDetectorAreaRight;
	Get().cTrackerCfgParam.cRecogSnapArea.DetectorAreaBottom = Get().cMatchParam.iDetectorAreaBottom;
	Get().cTrackerCfgParam.cRecogSnapArea.nDetectorMinScaleNum = Get().cMatchParam.iDetectorMinScaleNum;
	Get().cTrackerCfgParam.cRecogSnapArea.nDetectorMaxScaleNum = Get().cMatchParam.iDetectorMaxScaleNum;

	
	CSWString strTmp;
	for(int i = 0; i < sizeof(Get().cMatchParam.signal)/sizeof(SIGNAL_PARAM); i++)
	{
		strTmp.Format("\\OuterCtrl[外总控]\\Signal%02d", i);
		GetEnum((LPCSTR)strTmp
    	, "SignalType"
    	, (INT *)&Get().cMatchParam.signal[i].dwType
    	, Get().cMatchParam.signal[i].dwType
    	, "0:无;1:速度;2:触发;3:抓拍图"
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
			, "0:不触发抓拍;1:触发抓拍"
			, "此信号是否要触发抓拍"
			, ""
			, CUSTOM_LEVEL
		);
		
		GetEnum((LPCSTR)strTmp
			, "Recognize"
			, (INT *)&Get().cMatchParam.signal[i].fRecognize
			, Get().cMatchParam.signal[i].fRecognize
			, "0:不识别;1:识别"
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

/**
 *
 */
HRESULT CSWLPRParameter::InitCamApp(VOID)
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
   	, CUSTOM_LEVEL
    );
  
  GetInt("\\HvDsp\\Camera\\Ctrl"
   	, "AddrPort"
   	, &Get().cCamCfgParam.iCamPort
   	, Get().cCamCfgParam.iCamPort
   	, 1024
   	, 99999
   	, "测试协议端口号"
   	, "端口号"
   	, CUSTOM_LEVEL
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
    
    GetEnum("\\HvDsp\\Camera\\Ctrl"
    	, "DynamicTriggerEnable"
    	, &Get().cCamCfgParam.iDynamicTriggerEnable
    	, Get().cCamCfgParam.iDynamicTriggerEnable
      , "0:关闭;1:打开"
      , "视频触发抓拍相机"
      , "对创宇J系列相机及HV摄像机有效"
      , CUSTOM_LEVEL
    );
    
    GetInt("\\HvDsp\\Camera\\Ctrl"
    	, "TriggerDelay"
    	, &Get().cCamCfgParam.iTriggerDelay
    	, Get().cCamCfgParam.iTriggerDelay
    	, 0
    	, 65535
    	, "触发延时(ms)"
    	, ""
    	, PROJECT_LEVEL
    );
    
    GetEnum("\\HvDsp\\Camera\\Ctrl"
    	, "AGCEnable"
    	, &Get().cCamCfgParam.iEnableAGC
    	, Get().cCamCfgParam.iEnableAGC
      , "0:关闭;1:打开"
      , "使能AGC"
      , ""
      , CUSTOM_LEVEL
    );
    
    GetEnum("\\HvDsp\\Camera\\Ctrl"
    	, "AutoParamEnable"
    	, &Get().cCamCfgParam.iAutoParamEnable
    	, Get().cCamCfgParam.iAutoParamEnable
      , "0:关闭;1:打开"
      , "自动填充相机参数"
      , ""
      , CUSTOM_LEVEL
    );
  	
  	if (Get().cCamCfgParam.iAutoParamEnable == 1)
  	{
  	    UpdateEnum("\\HvDsp\\Camera\\Ctrl"
  	    	, "AutoParamEnable"
  	    	, 0
  	    );
  	}

	GetInt("\\HvDsp\\Camera\\Ctrl"
  	, "MaxAGCLimit"
  	, &Get().cCamCfgParam.iMaxAGCLimit
  	, Get().cCamCfgParam.iMaxAGCLimit
  	, 0
  	, 255
  	, "最大AGC门限值"
  	, ""
  	, CUSTOM_LEVEL
    );
    
    GetInt("\\HvDsp\\Camera\\Ctrl"
    	, "MinAGCLimit"
    	, &Get().cCamCfgParam.iMinAGCLimit
    	, Get().cCamCfgParam.iMinAGCLimit
    	, 0
    	, 255
    	, "最小AGC门限值"
    	, ""
    	, CUSTOM_LEVEL
    );
    
    GetFloat("\\HvDsp\\Camera\\Ctrl"
    	, "DB"
    	, &Get().cCamCfgParam.fltDB
    	, Get().cCamCfgParam.fltDB
    	, 1.0f
    	, 10.0f
    	, "步长"
    	, ""
    	, CUSTOM_LEVEL
    );
    
    GetInt("\\HvDsp\\Camera\\Ctrl"
    	, "MinExposureTime"
    	, &Get().cCamCfgParam.iMinExposureTime
    	, Get().cCamCfgParam.iMinExposureTime
    	, 0
    	, 30000
    	, "最小曝光时间"
    	, ""
    	, CUSTOM_LEVEL
    );
    
    GetInt("\\HvDsp\\Camera\\Ctrl"
    	, "MinPlus"
    	, &Get().cCamCfgParam.iMinGain
    	, Get().cCamCfgParam.iMinGain
    	, 70
    	, 360
    	, "最小增益"
    	, ""
    	, CUSTOM_LEVEL
    );
    
    GetInt("\\HvDsp\\Camera\\Ctrl"
    	, "MaxExposureTime"
    	, &Get().cCamCfgParam.iMaxExposureTime
    	, Get().cCamCfgParam.iMaxExposureTime
    	, 0
    	, 30000
    	, "最大曝光时间"
    	, ""
    	, CUSTOM_LEVEL
    );
    
    GetInt("\\HvDsp\\Camera\\Ctrl"
    	, "MaxPlus"
    	, &Get().cCamCfgParam.iMaxGain
    	, Get().cCamCfgParam.iMaxGain
    	, 70
    	, 360
    	, "最大增益"
    	, ""
    	, CUSTOM_LEVEL
    );

	GetInt("\\HvDsp\\Camera\\Ctrl"
		, "MinPSD"
		, &Get().cCamCfgParam.iMinPSD
		, Get().cCamCfgParam.iMinPSD
		, 100
		, 20000
		, "频闪脉宽最小值"
		, ""
		, CUSTOM_LEVEL
		);

    GetInt("\\HvDsp\\Camera\\Ctrl"
    	, "MaxPSD"
    	, &Get().cCamCfgParam.iMaxPSD
    	, Get().cCamCfgParam.iMaxPSD
    	, 100
    	, 20000
    	, "频闪脉宽最大值"
    	, ""
    	, CUSTOM_LEVEL
    );

	GetEnum("\\HvDsp\\Camera\\Ctrl"
		, "CtrlCpl"
		, &Get().cTrackerCfgParam.nCtrlCpl
		, Get().cTrackerCfgParam.nCtrlCpl
		, "0:不使能;1:使能"
		, "自动控制偏光镜"
		, "0:不使能;1:使能"
		, CUSTOM_LEVEL
		);

	GetInt("\\HvDsp\\Camera\\Ctrl"
		, "LightTypeCpl"
		, &Get().cTrackerCfgParam.nLightTypeCpl
		, Get().cTrackerCfgParam.nLightTypeCpl
		, 0
		, 13
		, "偏光镜自动切换的亮度等级阈值"
		, ""
		, CUSTOM_LEVEL
		);
    
    if(Get().cCamCfgParam.iMinPSD >= Get().cCamCfgParam.iMaxPSD)
    {
    	Get().cCamCfgParam.iMinPSD = 100;
    	Get().cCamCfgParam.iMaxPSD = 20000;
    	UpdateInt("\\HvDsp\\Camera\\Ctrl", "MinPSD", Get().cCamCfgParam.iMinPSD);
    	UpdateInt("\\HvDsp\\Camera\\Ctrl", "MaxPSD", Get().cCamCfgParam.iMaxPSD);
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
        	, CUSTOM_LEVEL
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
        	, CUSTOM_LEVEL
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
    
	return S_OK;
}

HRESULT CSWLPRParameter::InitCamCfgParam(CAM_CFG_PARAM *pCfgCamParam, const char **szLightType)
{
    if (pCfgCamParam->iMinExposureTime == 0)
    {
        float fTmp = pCfgCamParam->fltDB / 20;
        fTmp = pow((float)10.0, fTmp);
        pCfgCamParam->irgExposureTime[13] = pCfgCamParam->iMaxExposureTime;
        pCfgCamParam->irgGain[13] = pCfgCamParam->iMaxGain;

        for (int i = 12; i >= 0; i--)
        {
            int iNewExposureTime, iNewGain;
            int iLastExposureTime, iLastGain;
            iLastGain = pCfgCamParam->irgGain[i + 1];
            iNewGain = iLastGain - (int)(pCfgCamParam->fltDB * 10);
            iLastExposureTime = pCfgCamParam->irgExposureTime[i + 1];
            if ( iNewGain >= pCfgCamParam->iMinGain )
            {
                pCfgCamParam->irgExposureTime[i] = iLastExposureTime;
                pCfgCamParam->irgGain[i] = iNewGain;
            }
            else
            {
                iNewExposureTime = (int)(((float)iLastExposureTime / fTmp) + 0.5);
                pCfgCamParam->irgExposureTime[i] = iNewExposureTime;
                pCfgCamParam->irgGain[i] = iLastGain;
            }
        }
    }
    else
    {
        float fTmp = pCfgCamParam->fltDB / 20;
        fTmp = pow((float)10.0, fTmp);
        int iGainLevel = (int)(pCfgCamParam->fltDB * 10);

        pCfgCamParam->irgExposureTime[0] = pCfgCamParam->iMinExposureTime;
        pCfgCamParam->irgGain[0] = pCfgCamParam->iMinGain;

        for (int i = 1; i < 14; i++)
        {
            int iNewExposureTime, iNewGain;
            int iLastExposureTime, iLastGain;
            iLastExposureTime = pCfgCamParam->irgExposureTime[i - 1];
            iLastGain = pCfgCamParam->irgGain[i - 1];
            iNewExposureTime = (int)(((float)iLastExposureTime * fTmp) + 0.5);
            if ( iNewExposureTime <= pCfgCamParam->iMaxExposureTime )
            {
                pCfgCamParam->irgExposureTime[i] = iNewExposureTime;
                pCfgCamParam->irgGain[i] = iLastGain;
            }
            else
            {
                iNewGain = iLastGain + iGainLevel;
                if ( iNewGain > pCfgCamParam->iMaxGain )
                {
                    pCfgCamParam->irgExposureTime[i] = iLastExposureTime;
                    pCfgCamParam->irgGain[i] = iLastGain;
                }
                else
                {
                    pCfgCamParam->irgExposureTime[i] = iLastExposureTime;
                    pCfgCamParam->irgGain[i] = iNewGain;
                }
            }
        }
    }

    char szText[255];
    int iAGCDB = (pCfgCamParam->iMaxAGCLimit - pCfgCamParam->iMinAGCLimit) / 13;
    if (iAGCDB > 0)
    {
        for (int i = 0; i < 14; i++)
        {
            sprintf(szText, "\\HvDsp\\Camera\\%s", szLightType[i]);
            pCfgCamParam->irgAGCLimit[i] = pCfgCamParam->iMinAGCLimit + (iAGCDB * i);
            UpdateInt(szText, "AGCLimit", pCfgCamParam->irgAGCLimit[i]);
        }
        Get().cTrackerCfgParam.nMaxAGCTH = pCfgCamParam->irgAGCLimit[13];
    }

    if (pCfgCamParam->iEnableAGC)
    {
        for (int i = 0; i < 14; i++)
        {
            pCfgCamParam->irgExposureTime[i] = -1;
            pCfgCamParam->irgGain[i] = -1;
        }
    }

    for (int i = 0; i < 14; i++)
    {
        sprintf(szText, "\\HvDsp\\Camera\\%s", szLightType[i]);
        UpdateInt(szText, "ExposureTime", pCfgCamParam->irgExposureTime[i]);
        UpdateInt(szText, "Plus", pCfgCamParam->irgGain[i]);
    }

    return S_OK;
}

HRESULT CSWLPRParameter::Init368(VOID)
{
	/*
    GetEnum("\\CamApp"
    	, "SendType"
    	, &Get().cCamAppParam.iSendType
    	, Get().cCamAppParam.iSendType
    	, "-1:不传输;0:HvCam传输协议"
    	, "流传输模式"
    	, ""
    	,PROJECT_LEVEL
    );
	*/

    GetEnum("\\CamApp"
    	, "JpegStream"
    	, &Get().cCamAppParam.iJpegStream
    	, Get().cCamAppParam.iJpegStream
    	, "0:不使能;1:使能"
    	, "Jpeg流使能"
    	, ""
    	, CUSTOM_LEVEL
    );

    GetEnum("\\CamApp"
    	, "H264Stream"
    	, &Get().cCamAppParam.iH264Stream
    	, Get().cCamAppParam.iH264Stream
    	, "0:不使能;1:使能"
    	, "H.264流使能"
    	, ""
    	, CUSTOM_LEVEL
    );

    if (1 == Get().cCamAppParam.iH264Stream)
    {
        Get().cCamAppParam.iJpegStream = 0;
    }

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
    	, "JpegCompressRateCapture"
    	, &Get().cCamAppParam.iJpegCompressRateCapture
    	, Get().cCamAppParam.iJpegCompressRateCapture
    	, 1
    	, 99
    	, "抓拍Jpeg压缩品质"
    	, ""
    	, CUSTOM_LEVEL
    );

    GetInt("\\CamApp"
    	, "IFrameInterval"
    	, &Get().cCamAppParam.iIFrameInterval
    	, Get().cCamAppParam.iIFrameInterval
    	, 1
    	, 15
    	, "H.264流I帧间隔"
    	, ""
    	, CUSTOM_LEVEL
    );

	GetEnum("\\CamApp"
    	, "Resolution"
    	, &Get().cCamAppParam.iResolution
    	, Get().cCamAppParam.iResolution
    	, "1:720P;2:1080P"
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

		GetEnum("\\CamApp"
    	, "EnableCCDSignalCheck"
    	, &Get().cCamAppParam.iEnableCCDSignalCheck
    	, Get().cCamAppParam.iEnableCCDSignalCheck
    	, "0:不矫正;1:矫正"
    	, "CCD矫正"
    	, ""
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
    	, 70
    	, 360
    	, "AGC增益调节下限"
    	, ""
    	, PROJECT_LEVEL
    );

    GetInt("\\CamApp"
    	, "AGCGainHOri"
    	, &Get().cCamAppParam.iAGCGainHOri
    	, Get().cCamAppParam.iAGCGainHOri
    	, 70
    	, 360
    	, "AGC增益调节上限"
    	, ""
    	, CUSTOM_LEVEL
    );


    GetInt("\\CamApp"
    	, "GainR"
    	, &Get().cCamAppParam.iGainR
    	, Get().cCamAppParam.iGainR
    	, 0
    	, 255
    	, "R增益"
    	, ""
    	, CUSTOM_LEVEL
    );

    GetInt("\\CamApp"
    	, "GainG"
    	, &Get().cCamAppParam.iGainG
    	, Get().cCamAppParam.iGainG
    	, 0
    	, 255
    	, "G增益"
    	, ""
    	, CUSTOM_LEVEL
    );

    GetInt("\\CamApp"
    	, "GainB"
    	, &Get().cCamAppParam.iGainB
    	, Get().cCamAppParam.iGainB
    	, 0
    	, 255
    	, "B增益"
    	, ""
    	, CUSTOM_LEVEL
    );

    GetInt("\\CamApp"
    	, "Gain"
    	, &Get().cCamAppParam.iGain
    	, Get().cCamAppParam.iGain
    	, 70
    	, 360
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

    GetInt("\\CamApp"
    	, "CaptureGainR"
    	, &Get().cCamAppParam.iCaptureGainR
    	, Get().cCamAppParam.iCaptureGainR
    	, 0
    	, 255
    	, "抓拍R增益"
    	, ""
    	, CUSTOM_LEVEL
    );

    GetInt("\\CamApp"
    	, "CaptureGainG"
    	, &Get().cCamAppParam.iCaptureGainG
    	, Get().cCamAppParam.iCaptureGainG
    	, 0
    	, 255
    	, "抓拍G增益"
    	, ""
    	, CUSTOM_LEVEL
    );

    GetInt("\\CamApp"
    	, "CaptureGainB"
    	, &Get().cCamAppParam.iCaptureGainB
    	, Get().cCamAppParam.iCaptureGainB
    	, 0
    	, 255
    	, "抓拍B增益"
    	, ""
    	, CUSTOM_LEVEL
    );

    GetInt("\\CamApp"
    	, "CaptureGain"
    	, &Get().cCamAppParam.iCaptureGain
    	, Get().cCamAppParam.iCaptureGain
    	, 70
    	, 360
    	, "抓拍增益"
    	, ""
    	, CUSTOM_LEVEL
    );

    GetInt("\\CamApp"
    	, "CaptureShutter"
    	, &Get().cCamAppParam.iCaptureShutter
    	, Get().cCamAppParam.iCaptureShutter
    	, 0
    	, 30000
    	, "抓拍快门"
    	, "单位：us"
    	, CUSTOM_LEVEL
    );

    GetEnum("\\CamApp"
    	, "EnableCaptureGainRGB"
    	, &Get().cCamAppParam.iEnableCaptureGainRGB
    	, Get().cCamAppParam.iEnableCaptureGainRGB
    	, "0:不使能;1:使能"
    	, "抓拍RGB增益使能"
    	, ""
    	, CUSTOM_LEVEL
    );

    GetEnum("\\CamApp"
    	, "EnableCaptureGain"
    	, &Get().cCamAppParam.iEnableCaptureGain
    	, Get().cCamAppParam.iEnableCaptureGain
    	, "0:不使能;1:使能"
    	, "抓拍增益使能"
    	, ""
    	, CUSTOM_LEVEL
    );

    GetEnum("\\CamApp"
    	, "EnableCaptureShutter"
    	, &Get().cCamAppParam.iEnableCaptureShutter
    	, Get().cCamAppParam.iEnableCaptureShutter
    	, "0:不使能;1:使能"
    	, "抓拍快门使能"
    	, ""
    	, CUSTOM_LEVEL
    );
/*
    GetEnum("\\CamApp"
    	, "CameraSideInstall"
    	, &Get().cCamAppParam.fIsSideInstall
    	, Get().cCamAppParam.fIsSideInstall
    	, "0:正装;1:侧装"
    	, "前端镜头侧装"
    	, ""
    	, CUSTOM_LEVEL
    );
	*/

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
/*
    GetInt("\\CamApp"
    	, "OutputFrameRate"
    	, &Get().cCamAppParam.iOutputFrameRate
    	, Get().cCamAppParam.iOutputFrameRate
    	, 0
    	, 15
    	, "相机输出帧率"
    	, "相机视频输出的最大帧率"
    	, CUSTOM_LEVEL
    );
	*/

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

    GetEnum("\\CamApp"
    	, "FlashRateSynSignalEnable"
    	, &Get().cCamAppParam.iFlashRateSynSignalEnable
    	, Get().cCamAppParam.iFlashRateSynSignalEnable
    	, "0:不使能;1:使能"
    	, "频闪同步信号使能"
    	, ""
    	, CUSTOM_LEVEL
    );
	GetEnum("\\CamApp"
		, "FlashRateMultiplication"
		, &Get().cCamAppParam.iFlashRateMultiplication
		, Get().cCamAppParam.iFlashRateMultiplication
		, "0:不使能倍频;1:使能倍频"
		, "频闪倍频"
		, ""
		, CUSTOM_LEVEL
		);
	GetEnum("\\CamApp"
		, "FlashRatePolarity"
		, &Get().cCamAppParam.iFlashRatePolarity
		, Get().cCamAppParam.iFlashRatePolarity
		, "0:负极性;1:正极性"
		, "频闪极性"
		, ""
		, CUSTOM_LEVEL
		);
	GetInt("\\CamApp"
		, "FlashRatePulseWidth"
		, &Get().cCamAppParam.iFlashRatePulseWidth
		, Get().cCamAppParam.iFlashRatePulseWidth
		, 100
		, 20000
		, "频闪脉宽（单位：微秒）"
		, ""
		, CUSTOM_LEVEL
		);
	GetEnum("\\CamApp"
		, "FlashRateOutputType"
		, &Get().cCamAppParam.iFlashRateOutputType
		, Get().cCamAppParam.iFlashRateOutputType
		, "0:上拉(电平);1:OC（开关）"
		, "频闪输出类型"
		, ""
		, CUSTOM_LEVEL
		);

    GetEnum("\\CamApp"
    	, "CaptureSynSignalEnable"
    	, &Get().cCamAppParam.iCaptureSynSignalEnable
    	, Get().cCamAppParam.iCaptureSynSignalEnable
    	, "0:不使能;1:使能"
    	, "抓拍同步信号使能"
    	, ""
    	, CUSTOM_LEVEL
    );
	GetEnum("\\CamApp"
		, "CaptureSynPolarity"
		, &Get().cCamAppParam.iCaptureSynPolarity
		, Get().cCamAppParam.iCaptureSynPolarity
		, "0:负极性;1:正极性"
		, "抓拍极性"
		, ""
		, CUSTOM_LEVEL
		);
	GetInt("\\CamApp"
		, "CaptureSynPulseWidth"
		, &Get().cCamAppParam.iCaptureSynPulseWidth
		, Get().cCamAppParam.iCaptureSynPulseWidth
		, 100
		, 10000
		, "抓拍脉宽（单位：微秒）"
		, ""
		, CUSTOM_LEVEL
		);
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
		, "CaptureSynCoupling"
		, &Get().cCamAppParam.iCaptureSynCoupling
		, Get().cCamAppParam.iCaptureSynCoupling
		, "0:不耦合;1:耦合"
		, "抓拍是否与频闪耦合"
		, ""
		, CUSTOM_LEVEL
		);
/*
    GetEnum("\\CamApp"
    	, "ENetSyn"
    	, &Get().cCamAppParam.iENetSyn
    	, Get().cCamAppParam.iENetSyn
    	, "0:15fps;1:12.5fps"
    	, "电网同步使能"
    	, ""
    	, CUSTOM_LEVEL
    );
	*/

	GetEnum("\\CamApp"
		, "ExternSyncMode"
		, &Get().cCamAppParam.iExternalSyncMode
		, Get().cCamAppParam.iExternalSyncMode
		, "0:不同步;1:内部电网同步;2:外部信号同步"
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
    	, "0:外部下降沿触发;1:外部上升沿触发;2:上升沿下降沿都触发;3:不触发"
    	, "触发抓拍沿"
    	, ""
    	, CUSTOM_LEVEL
    );

    GetEnum("\\CamApp"
    	, "FlashDifferentLaneEx"
    	, &Get().cCamAppParam.iFlashDifferentLaneExt
    	, Get().cCamAppParam.iFlashDifferentLaneExt
    	, "0:不分车道闪;1:分车道闪"
    	, "闪光灯分车道闪(硬触发)"
    	, ""
    	, CUSTOM_LEVEL
    );
/*
    GetInt("\\CamApp\\Overlay"
    	, "Character_X"
    	, &Get().cCamAppParam.iX
    	, Get().cCamAppParam.iX
    	, 0
    	, 1600
    	, "字符叠X坐标"
    	, "注：正装时最大宽度为1600，侧装时为1200，不能设置为边沿值"
    	, CUSTOM_LEVEL
    );

    GetInt("\\CamApp\\Overlay"
    	, "Character_Y"
    	, &Get().cCamAppParam.iY
    	, Get().cCamAppParam.iY
    	, 0
    	, 1600
    	, "字符叠Y坐标"
    	, "注：正装时最大宽度为1200，侧装时为1600，不能设置为边沿值"
    	, CUSTOM_LEVEL
    );

    GetEnum("\\CamApp\\Overlay"
    	, "Character_DateFormat"
    	, &Get().cCamAppParam.iDateFormat
    	, Get().cCamAppParam.iDateFormat
    	, "0: '/';1: '-'"
    	, "日期格式"
    	, ""
    	, CUSTOM_LEVEL
    );

    GetInt("\\CamApp\\Overlay"
    	, "Character_FontColor"
    	, &Get().cCamAppParam.iFontColor
    	, Get().cCamAppParam.iFontColor
    	, 0
    	, 0x00ffffff
    	, "字体颜色"
    	, "字体颜色为RGB格式"
    	, CUSTOM_LEVEL
    );

    GetString("\\CamApp\\Overlay"
    	, "Character"
    	, Get().cCamAppParam.szCharater
    	, Get().cCamAppParam.szCharater
    	, 255
    	, "叠加的字符串"
    	, ""
    	, CUSTOM_LEVEL
    );

    GetEnum("\\CamApp\\Overlay"
    	, "EnableCharacter"
    	, &Get().cCamAppParam.iEnableCharacterOverlap
    	, Get().cCamAppParam.iEnableCharacterOverlap
    	, "0:不使能;1:使能"
    	, "字符叠加使能"
    	, ""
    	, CUSTOM_LEVEL
    );

    GetEnum("\\CamApp\\Overlay"
    	, "EnableFixedLight"
    	, &Get().cCamAppParam.iEnableFixedLight
    	, Get().cCamAppParam.iEnableFixedLight
    	, "0:不固定;1:固定"
    	, "字符叠加字体固定亮度"
    	, ""
    	, CUSTOM_LEVEL
    );
	*/
   
	/*
    GetEnum("\\CamApp"
    	, "EnableH264BrightnessAdjust"
    	, &Get().cCamAppParam.fEnableH264BrightnessAdjust
    	, Get().cCamAppParam.fEnableH264BrightnessAdjust
    	, "0:不使能;1:使能"
    	, "使能H264亮度调节", "0:不使能;1:使能"
    	, CUSTOM_LEVEL
    );

    GetInt("\\CamApp"
    	, "AdjustPointX"
    	, &Get().cCamAppParam.iAdjustPointX
    	, Get().cCamAppParam.iAdjustPointX
    	, 1
    	, 254
    	, "H264亮度调节点X值"
    	, ""
    	, CUSTOM_LEVEL
    );
    
    GetInt("\\CamApp"
    	, "AdjustPointY"
    	, &Get().cCamAppParam.iAdjustPointY
    	, Get().cCamAppParam.iAdjustPointY
    	, 0
    	, 255
    	, "H264亮度调节点Y值"
    	, ""
    	, CUSTOM_LEVEL
    );
	*/
		
    /*GetString("\\CamApp\\NetDisk"
    	, "NetDiskIP"
    	, Get().cCamAppParam.szNetDiskIP
    	, Get().cCamAppParam.szNetDiskIP
    	, sizeof(Get().cCamAppParam.szNetDiskIP)
    	, "网络存储的IP地址"
    	, ""
    	, CUSTOM_LEVEL
    );
    
    GetInt("\\CamApp\\NetDisk"
    	, "NetDiskSpace"
    	, &Get().cCamAppParam.iNetDiskSpace
    	, Get().cCamAppParam.iNetDiskSpace
    	, 1
    	, 3696
    	, "网络存储的容量,单位G (存储初始化时有意义)"
    	, ""
    	, CUSTOM_LEVEL
    );

    GetString("\\CamApp\\NetDisk"
    	, "NFS"
    	, Get().cCamAppParam.szNFS
    	, Get().cCamAppParam.szNFS
    	, sizeof(Get().cCamAppParam.szNFS)
    	, "NFS路径"
    	, ""
    	, CUSTOM_LEVEL
    );

    GetString("\\CamApp\\NetDisk"
    , "NFSParam"
    , Get().cCamAppParam.szNFSParam
    , Get().cCamAppParam.szNFSParam
    , sizeof(Get().cCamAppParam.szNFSParam)
    , "NFS挂载参数"
    , ""
    , PROJECT_LEVEL
    );*/
    
/*
// 自动调节晚上判断的依据由自动相机控制模块来判断
// 
    GetInt("\\CamApp\\Gamma"
    	, "AvgY"
    	, &Get().cCamAppParam.iAvgY
    	, Get().cCamAppParam.iAvgY
    	, 0
    	, 255
    	, "晚上亮度阈值"
    	, "低于此阈值的才进行Gamma调整"
    	, CUSTOM_LEVEL
    );
	*/

	GetInt("\\CamApp\\Gamma"
		, "Mode"
		, &Get().cCamAppParam.iGammaMode
		, Get().cCamAppParam.iGammaMode
		, 0
		, 3
		, "LUT通道选择模式"
		, "通道选择值：0：JPEG和H264都无，1：JPEG有H264无，2：JPEG无H264有，3：JPEG有H264有"
		, CUSTOM_LEVEL
		);

    GetInt("\\CamApp\\Gamma"
    	, "X0"
    	, &Get().cCamAppParam.rgiGamma[0][0]
    	, Get().cCamAppParam.rgiGamma[0][0]
    	, 0
    	, 255
    	, "X0"
    	, ""
    	, CUSTOM_LEVEL
    );
    
    GetInt("\\CamApp\\Gamma"
    	, "Y0"
    	, &Get().cCamAppParam.rgiGamma[0][1]
    	, Get().cCamAppParam.rgiGamma[0][1]
    	, 0
    	, 255
    	, "Y0"
    	, ""
    	, CUSTOM_LEVEL
    );

    GetInt("\\CamApp\\Gamma"
    	, "X1"
    	, &Get().cCamAppParam.rgiGamma[1][0]
    	, Get().cCamAppParam.rgiGamma[1][0]
    	, 0
    	, 255
    	, "X1"
    	, ""
    	, CUSTOM_LEVEL
    );
    
    GetInt("\\CamApp\\Gamma"
    	, "Y1"
    	, &Get().cCamAppParam.rgiGamma[1][1]
    	, Get().cCamAppParam.rgiGamma[1][1]
    	, 0
    	, 255
    	, "Y1"
    	, ""
    	, CUSTOM_LEVEL
    );

    GetInt("\\CamApp\\Gamma"
    	, "X2"
    	, &Get().cCamAppParam.rgiGamma[2][0]
    	, Get().cCamAppParam.rgiGamma[2][0]
    	, 0
    	, 255
    	, "X2"
    	, ""
    	, CUSTOM_LEVEL
    );
    GetInt("\\CamApp\\Gamma"
    	, "Y2"
    	, &Get().cCamAppParam.rgiGamma[2][1]
    	, Get().cCamAppParam.rgiGamma[2][1]
    	, 0
    	, 255
    	, "Y2"
    	, ""
    	, CUSTOM_LEVEL
    );

    GetInt("\\CamApp\\Gamma"
    	, "X3"
    	, &Get().cCamAppParam.rgiGamma[3][0]
    	, Get().cCamAppParam.rgiGamma[3][0]
    	, 0
    	, 255
    	, "X3"
    	, ""
    	, CUSTOM_LEVEL
    );
    
    GetInt("\\CamApp\\Gamma"
    	, "Y3"
    	, &Get().cCamAppParam.rgiGamma[3][1]
    	, Get().cCamAppParam.rgiGamma[3][1]
    	, 0
    	, 255
    	, "Y3"
    	, ""
    	, CUSTOM_LEVEL
    );

    GetInt("\\CamApp\\Gamma"
    	, "X4"
    	, &Get().cCamAppParam.rgiGamma[4][0]
    	, Get().cCamAppParam.rgiGamma[4][0]
    	, 0
    	, 255
    	, "X4"
    	, ""
    	, CUSTOM_LEVEL
    );
    GetInt("\\CamApp\\Gamma"
    	, "Y4"
    	, &Get().cCamAppParam.rgiGamma[4][1]
    	, Get().cCamAppParam.rgiGamma[4][1]
    	, 0
    	, 255
    	, "Y4"
    	, ""
    	, CUSTOM_LEVEL
    );

    GetInt("\\CamApp\\Gamma"
    	, "X5"
    	, &Get().cCamAppParam.rgiGamma[5][0]
    	, Get().cCamAppParam.rgiGamma[5][0]
    	, 0
    	, 255
    	, "X5"
    	, ""
    	, CUSTOM_LEVEL
    );
    
    GetInt("\\CamApp\\Gamma"
    	, "Y5"
    	, &Get().cCamAppParam.rgiGamma[5][1]
    	, Get().cCamAppParam.rgiGamma[5][1]
    	, 0
    	, 255
    	, "Y5"
    	, ""
    	, CUSTOM_LEVEL
    );

    GetInt("\\CamApp\\Gamma"
    	, "X6"
    	, &Get().cCamAppParam.rgiGamma[6][0]
    	, Get().cCamAppParam.rgiGamma[6][0]
    	, 0
    	, 255
    	, "X6"
    	, ""
    	, CUSTOM_LEVEL
    );
    GetInt("\\CamApp\\Gamma"
    	, "Y6"
    	, &Get().cCamAppParam.rgiGamma[6][1]
    	, Get().cCamAppParam.rgiGamma[6][1]
    	, 0
    	, 255
    	, "Y6"
    	, ""
    	, CUSTOM_LEVEL
    );

    GetInt("\\CamApp\\Gamma"
    	, "X7"
    	, &Get().cCamAppParam.rgiGamma[7][0]
    	, Get().cCamAppParam.rgiGamma[7][0]
    	, 0
    	, 255
    	, "X7"
    	, ""
    	, CUSTOM_LEVEL
    );
    GetInt("\\CamApp\\Gamma"
    	, "Y7"
    	, &Get().cCamAppParam.rgiGamma[7][1]
    	, Get().cCamAppParam.rgiGamma[7][1]
    	, 0
    	, 255
    	, "Y7"
    	, ""
    	, CUSTOM_LEVEL
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
    //校验gamma是否合理
    bool fValid = true;
    for (int i = 1; i < 8; i++)
    {
        if (Get().cCamAppParam.rgiGamma[i - 1][0] > Get().cCamAppParam.rgiGamma[i][0]
                || Get().cCamAppParam.rgiGamma[i - 1][0] < 0 || Get().cCamAppParam.rgiGamma[i - 1][0] > 255
                || Get().cCamAppParam.rgiGamma[i][0] < 0 || Get().cCamAppParam.rgiGamma[i][0] > 255
                || Get().cCamAppParam.rgiGamma[i - 1][1] < 0 || Get().cCamAppParam.rgiGamma[i - 1][1] > 255
                || Get().cCamAppParam.rgiGamma[i][1] < 0 || Get().cCamAppParam.rgiGamma[i][1] > 255)
        {
            fValid = false;
            break;
        }
    }
    if (!fValid)
    {
        for (int i = 0; i < 8; i++)
        {
            Get().cCamAppParam.rgiGamma[i][0] = Get().cCamAppParam.rgiDefGamma[i][0];
            Get().cCamAppParam.rgiGamma[i][1] = Get().cCamAppParam.rgiDefGamma[i][1];
        }
        
        UpdateInt("\\CamApp\\Gamma", "X0", Get().cCamAppParam.rgiGamma[0][0]);
        UpdateInt("\\CamApp\\Gamma", "Y0", Get().cCamAppParam.rgiGamma[0][1]);
        UpdateInt("\\CamApp\\Gamma", "X1", Get().cCamAppParam.rgiGamma[1][0]);
        UpdateInt("\\CamApp\\Gamma", "Y1", Get().cCamAppParam.rgiGamma[1][1]);
        UpdateInt("\\CamApp\\Gamma", "X2", Get().cCamAppParam.rgiGamma[2][0]);
        UpdateInt("\\CamApp\\Gamma", "Y2", Get().cCamAppParam.rgiGamma[2][1]);
        UpdateInt("\\CamApp\\Gamma", "X3", Get().cCamAppParam.rgiGamma[3][0]);
        UpdateInt("\\CamApp\\Gamma", "Y3", Get().cCamAppParam.rgiGamma[3][1]);
        UpdateInt("\\CamApp\\Gamma", "X4", Get().cCamAppParam.rgiGamma[4][0]);
        UpdateInt("\\CamApp\\Gamma", "Y4", Get().cCamAppParam.rgiGamma[4][1]);
        UpdateInt("\\CamApp\\Gamma", "X5", Get().cCamAppParam.rgiGamma[5][0]);
        UpdateInt("\\CamApp\\Gamma", "Y5", Get().cCamAppParam.rgiGamma[5][1]);
        UpdateInt("\\CamApp\\Gamma", "X6", Get().cCamAppParam.rgiGamma[6][0]);
        UpdateInt("\\CamApp\\Gamma", "Y6", Get().cCamAppParam.rgiGamma[6][1]);
        UpdateInt("\\CamApp\\Gamma", "X7", Get().cCamAppParam.rgiGamma[7][0]);
        UpdateInt("\\CamApp\\Gamma", "Y7", Get().cCamAppParam.rgiGamma[7][1]);
    }
    
    GetEnum("\\CamApp"
    	, "EnableDCAperture"
    	, &Get().cCamAppParam.iEnableDCAperture
    	, Get().cCamAppParam.iEnableDCAperture
    	, "0:使能;1:使能"
    	, "使能DC光圈"
    	, ""
    	, CUSTOM_LEVEL
    );
    
    GetEnum("\\CamApp"
    	, "EnableSharpen"
    	, &Get().cCamAppParam.iEnableSharpen
    	, Get().cCamAppParam.iEnableSharpen
    	, "0:使能;1:使能"
    	, "使能锐化"
    	, ""
    	, CUSTOM_LEVEL
    );
    
    GetEnum("\\CamApp"
    	, "SharpenCapture"
    	, &Get().cCamAppParam.iSharpenCapture
    	, Get().cCamAppParam.iSharpenCapture
    	, "0:锐化所有图;1:锐化抓拍图"
    	, "锐化抓拍图"
    	, ""
    	, CUSTOM_LEVEL
    );
    
    GetInt("\\CamApp"
    	, "SharpenThreshold"
    	, &Get().cCamAppParam.iSharpenThreshold
    	, Get().cCamAppParam.iSharpenThreshold
    	, 0
    	, 100
    	, "锐化阈值"
    	, ""
    	, CUSTOM_LEVEL
    );
    
	// 这个参数很神奇，对外还是要转换一下。
	int iMode = Get().cCamAppParam.iColorMatrixMode - 1;
    GetEnum("\\CamApp"
    	, "ColorMatrixMode"
    	, &iMode
    	, iMode
    	, "0:为关闭使能;1:为使能抓拍;2:为使能所有帧"
    	, "图像增强"
    	, ""
    	, CUSTOM_LEVEL
    );
	Get().cCamAppParam.iColorMatrixMode = iMode + 1;
    
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
    	, "HueThreshold"
    	, &Get().cCamAppParam.iHueThreshold
    	, Get().cCamAppParam.iHueThreshold
    	, -180
    	, 180
    	, "色度"
    	, ""
    	, CUSTOM_LEVEL
    );
    
    GetEnum("\\CamApp"
    	, "TemThreshold"
    	, &Get().cCamAppParam.iTemThreshold
    	, Get().cCamAppParam.iTemThreshold
    	, "0:0;32:32;40:40;52:52;55:55;60:60;70:70"
    	, "色温"
    	, ""
    	, CUSTOM_LEVEL
    );    
    
    GetEnum("\\CamApp"
    	, "HazeRemovalMode"
    	, &Get().cCamAppParam.iHazeRemovalMode
    	, Get().cCamAppParam.iHazeRemovalMode
    	, "1:为关闭使能;2:为使能抓拍;3:为使能所有帧"
    	, "除雾功能"
    	, ""
    	, CUSTOM_LEVEL
    );
    
    GetInt("\\CamApp"
    	, "HazeRemovalThreshold"
    	, &Get().cCamAppParam.iHazeRemovalThreshold
    	, Get().cCamAppParam.iHazeRemovalThreshold
    	, 0
    	, 10
    	, "除雾阈值"
    	, ""
    	, CUSTOM_LEVEL
    );

	return S_OK;
}

HRESULT CSWLPRParameter::InitFrontPannel(VOID)
{
	  GetEnum("\\CamApp\\FrontPannel"
	  	, "PannelStyle"
	  	, &Get().cFrontPannelParam.iUsedAutoControllMode
	  	, Get().cFrontPannelParam.iUsedAutoControllMode
	  	, "0:拨码板;1:数控板"
	  	, "控制板类型"
	  	, ""
	  	, CUSTOM_LEVEL
	  );

    GetEnum("\\CamApp\\FrontPannel"
    	, "AutoRun"
    	, &Get().cFrontPannelParam.iAutoRunFlag
    	, Get().cFrontPannelParam.iAutoRunFlag
    	, "0:为手动运行;1:为自动运行"
    	, "运行方式"
    	, ""
    	, CUSTOM_LEVEL
    );

    GetEnum("\\CamApp\\FrontPannel"
    	, "DivFrequency"
    	, &Get().cFrontPannelParam.iDivFrequency
    	, Get().cFrontPannelParam.iDivFrequency
    	, "1:1;2:2;4:4;8:8"
    	, "分频系数"
    	, ""
    	, CUSTOM_LEVEL
    );

    GetInt("\\CamApp\\FrontPannel"
    	, "OutPutDelay"
    	, &Get().cFrontPannelParam.iOutPutDelay
    	, Get().cFrontPannelParam.iOutPutDelay
    	, 0
    	, 150
    	, "输出延迟"
    	, "1对应0.1MS"
    	, CUSTOM_LEVEL
    );

    GetInt("\\CamApp\\FrontPannel"
    	, "PulseWidthMin"
    	, &Get().cFrontPannelParam.iPulseWidthMin
    	, Get().cFrontPannelParam.iPulseWidthMin
    	, 1
    	, 72
    	, "频闪脉宽最小值"
    	, ""
    	, CUSTOM_LEVEL
    );

    GetInt("\\CamApp\\FrontPannel"
    	, "PulseWidthMax"
    	, &Get().cFrontPannelParam.iPulseWidthMax
    	, Get().cFrontPannelParam.iPulseWidthMax
    	, 9
    	, 80
    	, "频闪脉宽最大值"
    	, ""
    	, CUSTOM_LEVEL
    );

    if (Get().cFrontPannelParam.iPulseWidthMax - Get().cFrontPannelParam.iPulseWidthMin < 8)
    {
        Get().cFrontPannelParam.iPulseWidthMin = 5;
        Get().cFrontPannelParam.iPulseWidthMax = 42;
        
        UpdateInt("\\CamApp\\FrontPannel"
        	, "PulseWidthMin"
        	, Get().cFrontPannelParam.iPulseWidthMin
        );

        UpdateInt("\\CamApp\\FrontPannel"
        	, "PulseWidthMax"
        	, Get().cFrontPannelParam.iPulseWidthMax
      	);
    }

    Get().cFrontPannelParam.iPulseStep = (int)((Get().cFrontPannelParam.iPulseWidthMax - Get().cFrontPannelParam.iPulseWidthMin) / 7);
    GetInt("\\CamApp\\FrontPannel\\Flash1Info"
    	, "PulseWidth"
    	, &Get().cFrontPannelParam.iFlash1PulseWidth
    	, Get().cFrontPannelParam.iFlash1PulseWidth
    	, 100
    	, 4000
    	, "脉宽"
    	, ""
    	, CUSTOM_LEVEL
    );

    GetEnum("\\CamApp\\FrontPannel\\Flash1Info"
    	, "Polarity"
    	, &Get().cFrontPannelParam.iFlash1Polarity
    	, Get().cFrontPannelParam.iFlash1Polarity
    	, "0:负脉冲;1:正脉冲"
    	, "极性"
    	, ""
    	, CUSTOM_LEVEL
    );

    GetEnum("\\CamApp\\FrontPannel\\Flash1Info"
    	, "Coupling"
    	, &Get().cFrontPannelParam.iFlash1Coupling
    	, Get().cFrontPannelParam.iFlash1Coupling
    	, "0:不耦合到频闪;1:为耦合到频闪"
			, "耦合"
			, ""
			, CUSTOM_LEVEL
		);

    GetEnum("\\CamApp\\FrontPannel\\Flash1Info"
    	, "ResistorMode"
    	, &Get().cFrontPannelParam.iFlash1ResistorMode
    	, Get().cFrontPannelParam.iFlash1ResistorMode
    	, "0:短路触发;1:电平触发"
    	, "触发类型"
    	, ""
    	, CUSTOM_LEVEL
    );

    GetInt("\\CamApp\\FrontPannel\\Flash2Info"
    	, "PulseWidth"
    	, &Get().cFrontPannelParam.iFlash2PulseWidth
    	, Get().cFrontPannelParam.iFlash2PulseWidth
    	, 100
    	, 4000
    	, "脉宽"
    	, ""
    	, CUSTOM_LEVEL
    );

    GetEnum("\\CamApp\\FrontPannel\\Flash2Info"
    	, "Polarity"
    	, &Get().cFrontPannelParam.iFlash2Polarity
    	, Get().cFrontPannelParam.iFlash2Polarity
    	, "0:负脉冲;1:正脉冲"
    	, "极性"
    	, ""
    	, CUSTOM_LEVEL
    );

    GetEnum("\\CamApp\\FrontPannel\\Flash2Info"
    	, "Coupling"
    	, &Get().cFrontPannelParam.iFlash2Coupling
    	, Get().cFrontPannelParam.iFlash2Coupling
    	, "0:不耦合到频闪;1:耦合到频闪"
			, "耦合"
			, ""
			, CUSTOM_LEVEL
		);

    GetEnum("\\CamApp\\FrontPannel\\Flash2Info"
    	, "ResistorMode"
    	, &Get().cFrontPannelParam.iFlash2ResistorMode
    	, Get().cFrontPannelParam.iFlash2ResistorMode
    	, "0:短路触发;1:电平触发"
    	, "触发类型"
    	, ""
    	, CUSTOM_LEVEL
    );

    GetInt("\\CamApp\\FrontPannel\\Flash3Info"
    	, "PulseWidth"
    	, &Get().cFrontPannelParam.iFlash3PulseWidth
    	, Get().cFrontPannelParam.iFlash3PulseWidth
    	, 100
    	, 4000
    	, "脉宽"
    	, ""
    	, CUSTOM_LEVEL
    );

    GetEnum("\\CamApp\\FrontPannel\\Flash3Info"
    	, "Polarity"
    	, &Get().cFrontPannelParam.iFlash3Polarity
    	, Get().cFrontPannelParam.iFlash3Polarity
    	, "0:负脉冲;1:正脉冲"
    	, "极性"
    	, ""
    	, CUSTOM_LEVEL
    );

    GetEnum("\\CamApp\\FrontPannel\\Flash3Info"
    	, "Coupling"
    	, &Get().cFrontPannelParam.iFlash3Coupling
    	, Get().cFrontPannelParam.iFlash3Coupling
    	, "0:不耦合到频闪;1:耦合到频闪"
    	, "耦合"
    	, ""
    	, CUSTOM_LEVEL
    );

    GetEnum("\\CamApp\\FrontPannel\\Flash3Info"
    	, "ResistorMode"
    	, &Get().cFrontPannelParam.iFlash3ResistorMode
    	, Get().cFrontPannelParam.iFlash3ResistorMode
    	, "0:短路触发;1:电平触发"
    	, "触发类型"
    	, ""
    	, CUSTOM_LEVEL
    );
        
	return S_OK;
}

HRESULT CSWLPRParameter::InitCharacter(VOID)
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
    	, 0
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
    	, ""
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
    	, 1936
    	, "X坐标"
    	, ""
    	, CUSTOM_LEVEL
    );
    
  GetInt("\\Overlay\\JPEG"
    	, "Y"
    	, &Get().cOverlay.cJPEGInfo.iY
    	, Get().cOverlay.cJPEGInfo.iY
    	, 0
    	, 1425
    	, "Y坐标"
    	, ""
    	, CUSTOM_LEVEL
    ); 
    
  GetInt("\\Overlay\\JPEG"
    	, "Size"
    	, &Get().cOverlay.cJPEGInfo.iFontSize
    	, Get().cOverlay.cJPEGInfo.iFontSize
    	, 0
    	, 128
    	, "字体大小"
    	, ""
    	, CUSTOM_LEVEL
    ); 
  /*  
  GetInt("\\Overlay\\JPEG"
    	, "Color"
    	, (INT *)&Get().cOverlay.cJPEGInfo.iColor
    	, Get().cOverlay.cJPEGInfo.iColor
    	, 0x00000000
    	, 0x00FFFFFF
    	, "字体颜色"
    	, ""
    	, CUSTOM_LEVEL
    );
  */  
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
  Get().cOverlay.cJPEGInfo.iColor = (dwR | (dwG << 8) | (dwB << 16));
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
    	, ""
    	, sizeof(Get().cOverlay.cJPEGInfo.szInfo)
    	, ""
    	, "叠加信息"
    	, CUSTOM_LEVEL
    );
    
   // todo.字符叠加计算字符总长度有问题，在设置时多加两个空格在尾部，避免因过长叠加显示不全。
   if( strlen(Get().cOverlay.cJPEGInfo.szInfo) + 3 < sizeof(Get().cOverlay.cJPEGInfo.szInfo) )
   {
 	  strcpy(Get().cOverlay.cJPEGInfo.szInfo + strlen(Get().cOverlay.cJPEGInfo.szInfo), "  ");
 	  SW_TRACE_NORMAL("<param>change overlay jpeg len.%s", Get().cOverlay.cJPEGInfo.szInfo);
   }
    
  GetEnum("\\Overlay\\JPEG"
    	, "Plate"
    	, &Get().cOverlay.cJPEGInfo.fEnablePlate
    	, Get().cOverlay.cJPEGInfo.fEnablePlate
    	, "0:不使能;1:使能"
    	, "叠加车牌号码"
    	, ""
    	, CUSTOM_LEVEL
    );
	return S_OK;
}
 
 HRESULT CSWLPRParameter::InitGB28181(VOID)
 {

     SW_TRACE_NORMAL(" HRESULT CSWLPRParameter::InitGB28181(VOID)");

     GetEnum("\\GB28181"
         , "Enable"
         , &Get().cGb28181.fEnalbe
         , Get().cGb28181.fEnalbe
         , "0:关闭;1:开启"
         , "开关"
         , ""
         , CUSTOM_LEVEL
     );

     // 修改I帧间隔
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
     
     GetString("\\GB28181"
          , "ServerID"
          , Get().cGb28181.szServerID
          , ""
          , sizeof(Get().cGb28181.szServerID)
          , "服务器ID"
          , "服务器ID"
          , CUSTOM_LEVEL
      );
     GetString("\\GB28181"
          , "ServerRegion"
          , Get().cGb28181.szServerRegion
          , ""
          , sizeof(Get().cGb28181.szServerRegion)
          , "服务器域"
          , "服务器域 为空默默认区服务器ID前10位"
          , PROJECT_LEVEL
      );
     GetString("\\GB28181"
          , "IPC_ID"
          , Get().cGb28181.szIPC_ID
          , ""
          , sizeof(Get().cGb28181.szIPC_ID)
          , "设备ID"
          , "设备ID"
          , CUSTOM_LEVEL
      );
     GetString("\\GB28181"
          , "IPC_Region"
          , Get().cGb28181.szIPC_Region
          , ""
          , sizeof(Get().cGb28181.szIPC_Region)
          , "设备域"
          , "设备域 为空默默认区设备ID前10位"
          , PROJECT_LEVEL
      );
     GetString("\\GB28181"
          , "szAlarmID"
          , Get().cGb28181.szAlarmID
          , ""
          , sizeof(Get().cGb28181.szAlarmID)
          , "报警ID"
          , "报警ID"
          , CUSTOM_LEVEL
      );
     GetString("\\GB28181"
          , "szPassWord"
          , Get().cGb28181.szPassWord
          , ""
          , sizeof(Get().cGb28181.szPassWord)
          , "注册密码"
          , "注册密码"
          , CUSTOM_LEVEL
      );


     GetString("\\GB28181"
          , "ServerIP"
          , Get().cGb28181.szServerIP
          , ""
          , sizeof(Get().cGb28181.szServerIP)
          , "服务器IP地址"
          , "服务器IP地址"
          , CUSTOM_LEVEL
      );
     GetInt("\\GB28181"
         , "ServerPort"
         , &Get().cGb28181.iServerPort
         , Get().cGb28181.iServerPort
         , 0
         , 10000
         , "服务器端口"
         , ""
         , CUSTOM_LEVEL
     );

		 
	  GetEnum("\\GB28181"
         , "VideoSaveEnable"
         , &Get().cGb28181.fVideoSaveEnable
         , Get().cGb28181.fVideoSaveEnable
         , "0:关闭;1:开启"
         , "控制录像存储开关"
         , "是否实时控制录像存储开关。响应手动录像和手动停止录像"
         , CUSTOM_LEVEL
     );
     SW_TRACE_DEBUG("GetEnum.VideoSaveEnable:%d\n",Get().cGb28181.fVideoSaveEnable);
	 
	 
     GetInt("\\GB28181"
         , "IPC_Port"
         , &Get().cGb28181.iIPC_Port
         , Get().cGb28181.iIPC_Port
         , 0
         , 10000
         , "设备消息监听端口"
         , ""
         , PROJECT_LEVEL
     );
     GetInt("\\GB28181"
         , "IPC_VideoPort"
         , &Get().cGb28181.iIPC_VideoPort
         , Get().cGb28181.iIPC_VideoPort
         , 0
         , 10000
         , "设备媒体发送端口"
         , ""
         , PROJECT_LEVEL
     );

     GetString("\\GB28181"
          , "szLongitude"
          , Get().cGb28181.szLongitude
          , ""
          , sizeof(Get().cGb28181.szLongitude)
          , "经度"
          , "经度"
          , PROJECT_LEVEL
      );
     GetString("\\GB28181"
          , "szLatitude"
          , Get().cGb28181.szLatitude
          , ""
          , sizeof(Get().cGb28181.szLatitude)
          , "纬度"
          , "纬度"
          , PROJECT_LEVEL
      );
     GetString("\\GB28181"
          , "szManufacturer"
          , Get().cGb28181.szManufacturer
          , ""
          , sizeof(Get().cGb28181.szManufacturer)
          , "厂家"
          , "厂家"
          , PROJECT_LEVEL
      );
     GetString("\\GB28181"
          , "szBlock"
          , Get().cGb28181.szBlock
          , ""
          , sizeof(Get().cGb28181.szBlock)
          , "警区"
          , "警区"
          , PROJECT_LEVEL
      );
     GetString("\\GB28181"
          , "szAddress"
          , Get().cGb28181.szAddress
          , ""
          , sizeof(Get().cGb28181.szAddress)
          , "安装地址"
          , "安装地址"
          , PROJECT_LEVEL
      );
     GetString("\\GB28181"
          , "szOwner"
          , Get().cGb28181.szOwner
          , ""
          , sizeof(Get().cGb28181.szOwner)
          , "设备归属"
          , "设备归属"
          , PROJECT_LEVEL
      );
     GetString("\\GB28181"
          , "szCivilCode"
          , Get().cGb28181.szCivilCode
          , ""
          , sizeof(Get().cGb28181.szCivilCode)
          , "行政区域"
          , "行政区域"
          , PROJECT_LEVEL
           );


    SW_TRACE_NORMAL(" HRESULT CSWLPRParameter::InitGB28181(VOID) end");

    return S_OK;
 }

HRESULT CSWLPRParameter::InitONVIF(VOID)
{

    SW_TRACE_NORMAL(" HRESULT CSWLPRParameter::InitONVIF(VOID)");


	GetEnum("\\ONVIF"
			 , "OnvifEnable"
			 , &Get().cOnvifParam.iOnvifEnable
			 , Get().cOnvifParam.iOnvifEnable
			 , "0:不使能;1:使能"
			 , "开关"
			 , ""
             , PROJECT_LEVEL);
	 GetEnum("\\ONVIF"
			 , "AuthenticateEnable"
			 , &Get().cOnvifParam.iAuthenticateEnable
			 , Get().cOnvifParam.iAuthenticateEnable
			 , "0:不使能;1:使能"
			 , "鉴权"
			 , "使能时需要用户密码才能进行操作"
             , PROJECT_LEVEL);

	 //为了统一，RTSP的鉴权和ONVIF的保持一致
	 Get().cRtspParam.iAuthenticateEnable = Get().cOnvifParam.iAuthenticateEnable;
	 Get().cRtspParam.iRTSPStreamNum = 1;
	 GetEnum("\\ONVIF"
			 , "AutoControlBitrateEnable"
			 , &Get().cRtspParam.iAutoControlBitrateEnable
			 , Get().cRtspParam.iAutoControlBitrateEnable
			 , "0:不使能;1:使能"
			 , "自动控制码率开关"
			 , "当网络出现拥塞会自动控制码率适应网络"
             , PROJECT_LEVEL);

    SW_TRACE_NORMAL(" HRESULT CSWLPRParameter::InitONVIF(VOID) End");

	return S_OK;
}
