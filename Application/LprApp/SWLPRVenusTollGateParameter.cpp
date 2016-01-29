/*
 * SWLPRVenusTollGateParameter.cpp
 *
 *  Created on: 2013年12月27日
 *      Author: qinjj
 */
#include "SWFC.h"
#include "SWLPRApplication.h"
#include "SWLPRVenusTollGateParameter.h"
typedef struct Point{
    DOUBLE X;
    DOUBLE Y;
    Point(DOUBLE x,DOUBLE y)
    {
        X = x;
        Y = y;
    }
    Point()
    {
        memset(this,0,sizeof(this));
    }
}Point;


Point CalcLineCrossPoint(Point line1From, Point line1End, Point line2From, Point line2End)
{
    double k1 = 0, c1 = 0, k2 = 0, c2 = 0, x = -1, y = -1;
    bool Vertical1 = false, Vertical2 = false;
    if (line1From.X == line1End.X)
    {
        Vertical1 = true;
    }
    else
    {
        k1 = (line1End.Y - line1From.Y) / (line1End.X - line1From.X);
        c1 = line1From.Y - k1 * line1From.X;
    }

    if (line2From.X == line2End.X)
    {
        Vertical2 = true;
    }
    else
    {
        k2 = (line2End.Y - line2From.Y) / (line2End.X - line2From.X);
        c2 = line2From.Y - k2 * line2From.X;
    }

    if (Vertical1 && Vertical2)
    {
        x = -1;
        y = -1;
    }
    else if (Vertical1)
    {
        x = line1From.X;
        y = k2 * x + c2;
    }
    else if (Vertical2)
    {
        x = line2From.X;
        y = k1 * x + c1;
    }
    else if (k1 != k2)
    {
        x = (k1 * line1From.X - k2 * line2From.X - line1From.Y + line2From.Y) / (k1 - k2);
        y = k1 * x + c1;
    }
//    Point point;
//    point.X = x;
//    point.Y = y;
    return Point(x,y);
}

CSWLPRVenusTollGateParameter::CSWLPRVenusTollGateParameter()
{
	ResetParam();
}

CSWLPRVenusTollGateParameter::~CSWLPRVenusTollGateParameter()
{
}

void CSWLPRVenusTollGateParameter::ResetParam(VOID)
{
	//先调用基类
	CSWLPRParameter::ResetParam();

	/*根据具体方案配置参数*/

    Get().nWorkModeIndex = PRM_TOLLGATE;
    m_strWorkMode.Format("收费站", Get().nWorkModeIndex);
	Get().nCameraWorkMode = 0;		//纯相机、收费站方案相机工作模式
	Get().nMCFWUsecase = IMX185;

    //车道默认值
    for (int i = 0; i < MAX_ROADLINE_NUM; i++)
    {
        swpa_memset(&Get().cTrackerCfgParam.rgcRoadInfo[i], 0, sizeof(ROAD_INFO));
        swpa_strcpy(Get().cTrackerCfgParam.rgcRoadInfo[i].szRoadTypeName, "NULL");
        Get().cTrackerCfgParam.rgcRoadInfo[i].iLineType = 1;
        Get().cTrackerCfgParam.nRoadLineNumber = 2;
        Get().cTrackerCfgParam.rgcRoadInfo[i].iRoadType = (int)(RRT_FORWARD | RRT_LEFT | RRT_RIGHT | RRT_TURN);
        Get().cTrackerCfgParam.cActionDetect.iIsYellowLine[i] = 0;
        Get().cTrackerCfgParam.cActionDetect.iIsCrossLine[i] = 0;
    }


    //投票参数
    Get().cTrackerCfgParam.nVoteFrameNum = 16;
    Get().cTrackerCfgParam.nMaxEqualFrameNumForVote = 8;

    Get().cTrackerCfgParam.iRoadNumberBegin  = 0;
    Get().cTrackerCfgParam.iStartRoadNum = 0;

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

//	// 抓拍位置初始值
//	Get().cTrackerCfgParam.nCaptureOnePos = 50;

	// 金星无频闪补光灯
	Get().cTrackerCfgParam.iUsedLight = 0;

	//金星扫描区域默认打开
	Get().cTrackerCfgParam.cTrapArea.fEnableDetAreaCtrl = TRUE;

	//金星RGB分量配置范围为36~255,默认值不能为0
	Get().cCamAppParam.iGainR = 36;
	Get().cCamAppParam.iGainG = 36;
	Get().cCamAppParam.iGainB = 36;

}


HRESULT CSWLPRVenusTollGateParameter::Initialize(CSWString strFilePath)
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
        && S_OK == InitAutoReboot())
    {
        return S_OK ;
    }

    SW_TRACE_DEBUG("<CSWLPRVenusTollGateParameter> Initialize failed.\n");
    return E_FAIL ;
}

HRESULT CSWLPRVenusTollGateParameter::InitSystem(VOID)
{
    if (S_OK == CSWLPRParameter::InitSystem())
    {

        return S_OK ;
    }
    return E_FAIL ;

}

HRESULT CSWLPRVenusTollGateParameter::InitTracker(VOID)
{
//#define YYY_DEBUG
#ifdef YYY_DEBUG
    GetInt("\\Tracker\\TrafficLight"
        , "OnePos"
        , &Get().cTrackerCfgParam.nCaptureOnePos
        , Get().cTrackerCfgParam.nCaptureOnePos
        , 0
        , 100
        , "抓拍第一张图的位置"
        , ""
        , PROJECT_LEVEL
    );

    GetInt("\\Tracker\\TrafficLight"
        , "TwoPos"
        , &Get().cTrackerCfgParam.nCaptureTwoPos
        , Get().cTrackerCfgParam.nCaptureTwoPos
        , 0
        , 100
        , "抓拍第二张图的位置"
        , ""
        , PROJECT_LEVEL
    );
    Get().cTrackerCfgParam.nCaptureThreePos = 10;
    GetInt("\\Tracker\\TrafficLight"
        , "ThreePos"
        , &Get().cTrackerCfgParam.nCaptureThreePos
        , Get().cTrackerCfgParam.nCaptureThreePos
        , 0
        , 100
        , "抓拍第三张图的位置"
        , ""
        , PROJECT_LEVEL
    );
#endif
//    GetEnum("Tracker\\Misc"
//        , "OutPutNoPlate"
//        , &Get().cTrackerCfgParam.iOutPutNoPlate
//        , Get().cTrackerCfgParam.iOutPutNoPlate
//        , "0:关闭;1:打开"
//        , "输出无牌车"
//        , "视频流收费站输出无车牌结果"
//        , PROJECT_LEVEL
//        );


    GetEnum("\\Tracker\\ProcessPlate"
        , "EnableDBGreenSegment"
        , &Get().cTrackerCfgParam.nPlateDetect_Green
        , Get().cTrackerCfgParam.nPlateDetect_Green
        , "0:关闭;1:打开"
        , "绿牌识别开关"
        , ""
        , PROJECT_LEVEL
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
        , &Get().cTrackerCfgParam.nNightThreshold
        , Get().cTrackerCfgParam.nNightThreshold
        , 0
        , 240
        , "夜晚模式亮度上限"
        , ""
        , PROJECT_LEVEL				//算法优化相关放到3级参数
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
        , PROJECT_LEVEL
        );

    Get().cTrackerCfgParam.nProcessPlate_BlackPlate_L = 30;
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

    GetInt("\\Tracker\\RoadInfo"
        , "RoadLineNumber"
        , &Get().cTrackerCfgParam.nRoadLineNumber
        , Get().cTrackerCfgParam.nRoadLineNumber
        , 2
        , 2
        , "车道线数量"
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

    for (int i = 0; i < 2; i++)
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
            , PROJECT_LEVEL
        );

        GetInt(szSection
            , "RoadType"
            , &Get().cTrackerCfgParam.rgcRoadInfo[i].iRoadType
            , Get().cTrackerCfgParam.rgcRoadInfo[i].iRoadType
            , 1
            , 9999
            , "对应的车道类型"
            , ""
            , PROJECT_LEVEL
        );
    }

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
        , 1 //default value
        , "0:关闭;1:打开"
        , "黄牌字母识别开关"
        , ""
        , PROJECT_LEVEL
        );

    Get().cTrackerCfgParam.iOutPutNoPlate = 1;
    GetEnum("Tracker\\Misc"
	   , "OutPutNoPlate"
	   , &Get().cTrackerCfgParam.iOutPutNoPlate
	   , Get().cTrackerCfgParam.iOutPutNoPlate
	   , "0:关闭;1:打开"
	   , "无牌车识别开关"
	   , ""
	   , PROJECT_LEVEL
	   );

   GetInt("Tracker\\Misc"
	   , "ScanAreaTopOffset"
	   , &Get().cTrackerCfgParam.iScanAreaTopOffset
	   , Get().cTrackerCfgParam.iScanAreaTopOffset
	   , 0
	   , 100
	   , "检测区顶部离触发线百分比"
	   , ""
	   , CUSTOM_LEVEL
	   );

   GetInt("Tracker\\Misc"
	   , "ScanAreaBottomOffset"
	   , &Get().cTrackerCfgParam.iScanAreaBottomOffset
	   , Get().cTrackerCfgParam.iScanAreaBottomOffset
	   , 0
	   , 100
	   , "检测区底部离触发线百分比"
	   , ""
	   , CUSTOM_LEVEL
	   );

	Get().cTrackerCfgParam.nRecogInVaildRatioTH = 40;
	GetInt("Tracker\\Misc"
		, "RecogInVaildRatioTH"
		, &Get().cTrackerCfgParam.nRecogInVaildRatioTH
		, Get().cTrackerCfgParam.nRecogInVaildRatioTH
		, 0
		, 100
		, "识别无效比例阈值"
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

    Get().cTrackerCfgParam.nCarArrivedPos = 60;
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

    Get().cTrackerCfgParam.iBuleTriggerToYellowOffset = 12;
    GetInt("Tracker\\Misc"
        , "BuleTriggerToYellowOffset"
        , &Get().cTrackerCfgParam.iBuleTriggerToYellowOffset
        , Get().cTrackerCfgParam.iBuleTriggerToYellowOffset
        , 0
        , 100
        , "蓝牌触发线到黄牌触发线百分比"
        , ""
        , CUSTOM_LEVEL
        );
/*
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
*/
  //  Get().cTrackerCfgParam.nCarArrivedPosYellow = 50;
//    GetInt("Tracker\\Misc"
//        , "CarArrivedPosYellow"
//        , &Get().cTrackerCfgParam.nCarArrivedPosYellow
//        , Get().cTrackerCfgParam.nCarArrivedPosYellow
//        , 0
//        , 100
//        , "车辆到达位置(黄牌)"
//        , ""
//        , CUSTOM_LEVEL
//        );

	GetEnum("Tracker\\Misc"
        , "MovingDirection"
        , &Get().cTrackerCfgParam.iMovingDirection
        , Get().cTrackerCfgParam.iMovingDirection
        , "0:由远及近;1:由近及远;4:任意方向;"
        , "车辆行驶方向"
        , ""
        , CUSTOM_LEVEL
        );

	GetEnum("Tracker\\Misc"
        , "FilterReverseEnable"
        , &Get().cTrackerCfgParam.fFilterReverseEnable
        , Get().cTrackerCfgParam.fFilterReverseEnable
        , "0:不过滤;1:过滤"
        , "过滤逆行方向"
        , ""
        , CUSTOM_LEVEL
        );

	GetInt("Tracker\\Misc"
        , "nSpan"
        , &Get().cTrackerCfgParam.nSpan
        , Get().cTrackerCfgParam.nSpan
        , -100
        , 100
        , "逆行方向阈值"
        , ""
        , PROJECT_LEVEL
        );

    Get().cTrackerCfgParam.nVoteFrameNum = 6;
    GetInt("Tracker\\TrackInfo"
        , "VoteFrameNum"
        , &Get().cTrackerCfgParam.nVoteFrameNum
        , Get().cTrackerCfgParam.nVoteFrameNum
        , 2
        , 1000
        , "投票的结果数"
        , ""
        , CUSTOM_LEVEL
        );
		
    Get().cTrackerCfgParam.nMaxEqualFrameNumForVote = 3;
    GetInt("Tracker\\TrackInfo"
        , "MaxEqualFrameNumForVote"
        , &Get().cTrackerCfgParam.nMaxEqualFrameNumForVote
        , Get().cTrackerCfgParam.nMaxEqualFrameNumForVote
        , 1, 1000
        , "连续相同结果出牌条件"
        , ""
        , CUSTOM_LEVEL
        );
    Get().cTrackerCfgParam.nBlockTwinsTimeout = 60;
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

//    GetInt("Tracker\\TrackInfo"
//        , "MJPEGHeight"
//        , &Get().nMJPEGHeight
//        , Get().nMJPEGHeight
//        , 0
//        , 10000
//        , "JPEG图像高度"
//        , ""
//        , PROJECT_LEVEL
//        );
//    GetInt("Tracker\\TrackInfo"
//        , "MJPEGWidth"
//        , &Get().nMJPEGWidth
//        , Get().nMJPEGWidth
//        , 0
//        , 10000
//        , "JPEG图像宽度"
//        , ""
//        , PROJECT_LEVEL
//        );

//#ifndef YYY_DEBUG
//    INT CaptureOnePos = 28;
//    INT CaptureTwoPos = 12;
//#else
//    INT CaptureOnePos = Get().cTrackerCfgParam.nCaptureOnePos;
//    INT CaptureTwoPos = Get().cTrackerCfgParam.nCaptureTwoPos;
//#endif


    //道路线1
    Point line1From(Get().cTrackerCfgParam.rgcRoadInfo[0].ptTop.x,Get().cTrackerCfgParam.rgcRoadInfo[0].ptTop.y);
    Point line1End(Get().cTrackerCfgParam.rgcRoadInfo[0].ptBottom.x,Get().cTrackerCfgParam.rgcRoadInfo[0].ptBottom.y);
    //道路线2
    Point line2From(Get().cTrackerCfgParam.rgcRoadInfo[1].ptTop.x,Get().cTrackerCfgParam.rgcRoadInfo[1].ptTop.y);
    Point line2End(Get().cTrackerCfgParam.rgcRoadInfo[1].ptBottom.x,Get().cTrackerCfgParam.rgcRoadInfo[1].ptBottom.y);

//    GetEnum("Tracker\\DetAreaCtrl\\Normal"
//        , "EnableTrap"
//        , &Get().cTrackerCfgParam.cTrapArea.fEnableDetAreaCtrl
//        , Get().cTrackerCfgParam.cTrapArea.fEnableDetAreaCtrl
//        , "0:关闭;1:打开"
//        , "梯形扫描区域控制开关"
//        , ""
//        , PROJECT_LEVEL
//        );

    GetEnum("\\Tracker\\Recognition"
        , "EnableRecogCarColor"
        , &Get().cTrackerCfgParam.fEnableRecgCarColor
        , Get().cTrackerCfgParam.fEnableRecgCarColor
        , "0:关;1:开"
        , "车身颜色识别开关"
        , ""
        , CUSTOM_LEVEL
        );

    GetInt("\\Tracker\\ProcessPlate"
        , "SegWhitePlate"
        , &Get().cTrackerCfgParam.nSegWhitePlate
        , Get().cTrackerCfgParam.nSegWhitePlate
        , 0
        , 1
        , "强制白牌分割"
        , ""
        , PROJECT_LEVEL				//算法优化相关放到3级参数
        );

	//车身颜色识别控制
	GetInt("\\Tracker\\ProcessPlate"
        , "EnableColorReRecog"
        , &Get().cTrackerCfgParam.nEnableColorReRecog
        , 3	//default value
        , 0
        , 7
        , "黑白灰重识别"
        , ""
        , PROJECT_LEVEL
        );

	GetInt("\\Tracker\\ProcessPlate"
        , "nBColorReThr"
        , &Get().cTrackerCfgParam.nBColorReThr
        , Get().cTrackerCfgParam.nBColorReThr
        , -60
        , 60
        , "黑重识别阈值"
        , ""
        , PROJECT_LEVEL
        );

	GetInt("\\Tracker\\ProcessPlate"
        , "nWColorReThr"
        , &Get().cTrackerCfgParam.nWColorReThr
        , Get().cTrackerCfgParam.nWColorReThr
        , 0
        , 240
        , "白重识别阈值"
        , ""
        , PROJECT_LEVEL
        );

	GetInt("\\Tracker\\ProcessPlate"
        , "nWGColorReThr"
        , &Get().cTrackerCfgParam.nWGColorReThr
        , Get().cTrackerCfgParam.nWGColorReThr
        , 0
        , 240
        , "灰白重识别阈值"
        , ""
        , PROJECT_LEVEL
        );

	GetInt("\\Tracker\\ProcessPlate"
        , "EnableColorThr"
        , &Get().cTrackerCfgParam.fEnableCorlorThr
        , 0 //default
        , 0
        , 1
        , "灰黑控制开关"
        , ""
        , PROJECT_LEVEL
        );

	GetInt("\\Tracker\\ProcessPlate"
        , "nBGColorThr"
        , &Get().cTrackerCfgParam.nBGColorThr
        , Get().cTrackerCfgParam.nBGColorThr
        , 0
        , 240
        , "灰黑控制阈值"
        , ""
        , PROJECT_LEVEL
        );

	GetInt("\\Tracker\\ProcessPlate"
        , "nWGColorThr"
        , &Get().cTrackerCfgParam.nWGColorThr
        , Get().cTrackerCfgParam.nWGColorThr
        , 0
        , 240
        , "灰白控制阈值"
        , ""
        , PROJECT_LEVEL
        );

    return S_OK;
}

HRESULT CSWLPRVenusTollGateParameter::InitHvDsp(VOID)
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

    // 默认输出车辆到达时间
    Get().cResultSenderParam.fOutputCarArriveTime = TRUE;

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

    GetInt("\\HvDsp\\Misc"
        , "SendRecordSpace"
        , &Get().cResultSenderParam.iSendRecordSpace
        , Get().cResultSenderParam.iSendRecordSpace
        , 0
        , 100
        , "结果发送间隔(*100ms)"
        , ""
        , CUSTOM_LEVEL
        );

    GetInt("\\HvDsp\\Misc"
        , "SendHisVideoSpace"
        , &Get().cResultSenderParam.iSendHisVideoSpace
        , Get().cResultSenderParam.iSendHisVideoSpace
        , 1
        , 20
        , "历史录像发送间隔(*100ms)"
        , ""
        , CUSTOM_LEVEL
        );

    Get().cResultSenderParam.iBestSnapshotOutput = 0;
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

HRESULT CSWLPRVenusTollGateParameter::InitOuterCtrl(VOID)
{
#if 0		//暂时去掉，后期要求外设接入再打开
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

    CSWString strTmp;
    for (int i = 0; i < sizeof(Get().cMatchParam.signal) / sizeof(SIGNAL_PARAM); i++)
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

#endif
    return S_OK ;
}


HRESULT CSWLPRVenusTollGateParameter::InitIPTCtrl(VOID)
{
#if 1		//串口测试需要，打开参数，但是三级参数

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
		, PROJECT_LEVEL
	);
	  
	GetEnum((LPCSTR)strCOMM
		, "DeviceType"
		, &Get().cDevParam[i].iDeviceType
		, 0
		, "0:无;1:川速雷达;2:苏江车检器;4:奥利维亚雷达;5:四川九洲雷达"
		, "外部设备类型"
		, ""
		, PROJECT_LEVEL
	);
	GetInt((LPCSTR)strCOMM
		, "Road"
		, &Get().cDevParam[i].iRoadNo
		, Get().cDevParam[i].iRoadNo
		, 0
		, 255
		, "所属车道编号"
		, "所属车道编号"
		, PROJECT_LEVEL
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
		, PROJECT_LEVEL
	);
#endif
		  
	return S_OK;
}


HRESULT CSWLPRVenusTollGateParameter::InitCamApp(VOID)
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
      Get().cCamCfgParam.iEnableAGC = 1;
//	  GetEnum("\\HvDsp\\Camera\\Ctrl"
//		  , "AGCEnable"
//		  , &Get().cCamCfgParam.iEnableAGC
//		  , Get().cCamCfgParam.iEnableAGC
//		, "0:关闭;1:打开"
//		, "使能AGC"
//		, ""
//		, CUSTOM_LEVEL
//	  );

//	  GetEnum("\\HvDsp\\Camera\\Ctrl"
//		  , "AutoParamEnable"
//		  , &Get().cCamCfgParam.iAutoParamEnable
//		  , Get().cCamCfgParam.iAutoParamEnable
//		, "0:关闭;1:打开"
//		, "自动填充相机参数"
//		, ""
//		, CUSTOM_LEVEL
//	  );
      Get().cCamCfgParam.iAutoParamEnable = 1;
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
	  
//	  GetFloat("\\HvDsp\\Camera\\Ctrl"
//		  , "DB"
//		  , &Get().cCamCfgParam.fltDB
//		  , Get().cCamCfgParam.fltDB
//		  , 1.0f
//		  , 10.0f
//		  , "步长"
//		  , ""
//		  , CUSTOM_LEVEL
//	  );
	  
//	  GetInt("\\HvDsp\\Camera\\Ctrl"
//		  , "MinExposureTime"
//		  , &Get().cCamCfgParam.iMinExposureTime
//		  , Get().cCamCfgParam.iMinExposureTime
//		  , 0
//		  , 30000
//		  , "最小曝光时间"
//		  , ""
//		  , CUSTOM_LEVEL
//	  );
	  
//	  GetInt("\\HvDsp\\Camera\\Ctrl"
//		  , "MinPlus"
//		  , &Get().cCamCfgParam.iMinGain
//		  , Get().cCamCfgParam.iMinGain
//		  , 70
//		  , 360
//		  , "最小增益"
//		  , ""
//		  , CUSTOM_LEVEL
//	  );
	  
//	  GetInt("\\HvDsp\\Camera\\Ctrl"
//		  , "MaxExposureTime"
//		  , &Get().cCamCfgParam.iMaxExposureTime
//		  , Get().cCamCfgParam.iMaxExposureTime
//		  , 0
//		  , 30000
//		  , "最大曝光时间"
//		  , ""
//		  , CUSTOM_LEVEL
//	  );
	  
//	  GetInt("\\HvDsp\\Camera\\Ctrl"
//		  , "MaxPlus"
//		  , &Get().cCamCfgParam.iMaxGain
//		  , Get().cCamCfgParam.iMaxGain
//		  , 70
//		  , 360
//		  , "最大增益"
//		  , ""
//		  , CUSTOM_LEVEL
//	  );
	
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

HRESULT CSWLPRVenusTollGateParameter::InitCamera(VOID)
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
//    JpegCompressRateHigh = 80;
//    GetInt("\\CamApp"
//    	, "JpegCompressRateHigh"
//    	, &Get().cCamAppParam.iJpegCompressRateH
//    	, Get().cCamAppParam.iJpegCompressRateH
//    	, 1
//    	, 100
//    	, "Jpeg压缩率自动调节上限"
//    	, ""
//    	, CUSTOM_LEVEL
//    );

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
    	, CUSTOM_LEVEL
    );

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

    Get().cCamAppParam.iSNFTNFMode = 3;
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

	GetEnum("\\CamApp"
		, "LEDPolarity"
		, &Get().cCamAppParam.iLEDPolarity
		, Get().cCamAppParam.iLEDPolarity
		, "0:负极性;1:正极性"
		, "补光灯控制极性"
		, ""
		, CUSTOM_LEVEL
	);
/*
	GetInt("\\CamApp"
		, "LEDPulseWidth"
		, &Get().cCamAppParam.iLEDPulseWidth
		, Get().cCamAppParam.iLEDPulseWidth
		, 100
		, 20000
		, "补光灯脉宽"
		, "100 到 20000. 微秒"
		, CUSTOM_LEVEL
	);
*/

	GetEnum("\\CamApp"
		, "LEDOutputType"
		, &Get().cCamAppParam.iLEDOutputType
		, Get().cCamAppParam.iLEDOutputType
		, "0:上拉（电平）;1:OC(开关)"
		, "补光灯输出类型"
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
		, "F1OutputType"
		, &Get().cCamAppParam.iF1OutputType
		, Get().cCamAppParam.iF1OutputType
		, "0:栏杆机输出信号;1:闪光灯输出信号"
		, "F1输出类型"
		, ""
		, CUSTOM_LEVEL
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

    Get().cCamAppParam.iCVBSExport = 1;		//默认 以 剪切输出
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
		Get().nMCFWUsecase = IMX185_CVBSEXPORT;
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
HRESULT CSWLPRVenusTollGateParameter::InitCharacter(VOID)
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
//	  SW_TRACE_DEBUG("h264 color[0x%08x][0x%02x,0x%02x,0x%02x]", Get().cOverlay.cH264Info.dwColor, dwR, dwG, dwB);
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
//	SW_TRACE_DEBUG("jpeg color[0x%08x][0x%02x,0x%02x,0x%02x]", Get().cOverlay.cJPEGInfo.iColor, dwR, dwG, dwB);
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


HRESULT CSWLPRVenusTollGateParameter::InitGB28181(VOID)
{
	return CSWLPRParameter::InitGB28181();
}

HRESULT CSWLPRVenusTollGateParameter::InitAutoReboot(VOID)
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


