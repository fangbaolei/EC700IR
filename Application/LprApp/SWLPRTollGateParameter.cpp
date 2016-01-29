#include "SWFC.h"
#include "SWLPRApplication.h"
#include "SWLPRTollGateParameter.h"


CSWLPRTollGateParameter::CSWLPRTollGateParameter()
{
    Get().nWorkModeIndex = PRM_TOLLGATE;
    m_strWorkMode.Format("收费站", Get().nWorkModeIndex);

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
}

CSWLPRTollGateParameter::~CSWLPRTollGateParameter()
{
}

HRESULT CSWLPRTollGateParameter::InitTracker(VOID)
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
        , CUSTOM_LEVEL
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
        , CUSTOM_LEVEL
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

    GetEnum("Tracker\\DetAreaCtrl\\Normal"
        , "EnableTrap"
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

    GetInt("Tracker\\DetAreaCtrl\\Normal", "BottomLeftX"
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
    Get().cTrackerCfgParam.cDetectArea.DetectorAreaLeft =
        Get().cTrackerCfgParam.cTrapArea.TopLeftX > Get().cTrackerCfgParam.cTrapArea.BottomLeftX ?
            Get().cTrackerCfgParam.cTrapArea.BottomLeftX : Get().cTrackerCfgParam.cTrapArea.TopLeftX;
    Get().cTrackerCfgParam.cDetectArea.DetectorAreaRight =
        Get().cTrackerCfgParam.cTrapArea.TopRightX < Get().cTrackerCfgParam.cTrapArea.BottomRightX ?
            Get().cTrackerCfgParam.cTrapArea.BottomRightX : Get().cTrackerCfgParam.cTrapArea.TopRightX;
    Get().cTrackerCfgParam.cDetectArea.DetectorAreaTop =
        Get().cTrackerCfgParam.cTrapArea.TopLeftY > Get().cTrackerCfgParam.cTrapArea.TopRightY ?
            Get().cTrackerCfgParam.cTrapArea.TopRightY : Get().cTrackerCfgParam.cTrapArea.TopLeftY;
    Get().cTrackerCfgParam.cDetectArea.DetectorAreaBottom =
        Get().cTrackerCfgParam.cTrapArea.BottomLeftY > Get().cTrackerCfgParam.cTrapArea.BottomRightY ?
            Get().cTrackerCfgParam.cTrapArea.BottomLeftY : Get().cTrackerCfgParam.cTrapArea.BottomRightY;

    //Scale有效性判断
    if (Get().cTrackerCfgParam.cDetectArea.nDetectorMinScaleNum
        > Get().cTrackerCfgParam.cDetectArea.nDetectorMaxScaleNum)
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
        , CUSTOM_LEVEL
        );

	//车身颜色识别控制	
	GetInt("\\Tracker\\ProcessPlate"
        , "EnableColorReRecog"
        , &Get().cTrackerCfgParam.nEnableColorReRecog
        , Get().cTrackerCfgParam.nEnableColorReRecog
        , 0
        , 7
        , "黑白灰重识别"
        , ""
        , CUSTOM_LEVEL
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
        , Get().cTrackerCfgParam.fEnableCorlorThr
        , 0
        , 1
        , "灰黑控制开关"
        , ""
        , CUSTOM_LEVEL
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

HRESULT CSWLPRTollGateParameter::InitHvDsp(VOID)
{
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

    // 参数传递.
    Get().cResultSenderParam.cProcRule.fOutPutFilterInfo = Get().cResultSenderParam.fOutputAppendInfo;

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

	//收费站默认不检测逆行
    /*
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
        */
    return S_OK ;
}

HRESULT CSWLPRTollGateParameter::InitOuterCtrl(VOID)
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

    CSWString strTmp;
    for (int i = 0; i < sizeof(Get().cMatchParam.signal) / sizeof(SIGNAL_PARAM); i++)
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
            , "Condition"
            , (INT *)&Get().cMatchParam.signal[i].dwCondition
            , Get().cMatchParam.signal[i].dwCondition
            , "0:此信号可有可无,只要有车牌即可出牌;1:必须有此信号和车牌才能出结果;2:只要有此信号,就必定出牌"
            , "匹配条件"
            , ""
            , CUSTOM_LEVEL
            );
    }
    return S_OK ;
}

HRESULT CSWLPRTollGateParameter::InitScaleSpeed(VOID)
{
	return S_OK;
}

HRESULT CSWLPRTollGateParameter::InitIPTCtrl(VOID)
{
	if(S_OK == CSWLPRParameter::InitIPTCtrl())
	{
		CSWString strCOMM;
		for(int i = 0; i < 2; i++)
		{
			strCOMM.Format("\\Device[外部设备]\\COM%d[端口%02d]", i, i);
			GetEnum((LPCSTR)strCOMM
  		  	, "DeviceType"
  		  	, &Get().cDevParam[i].iDeviceType
  		  	, 0
  		  	, "0:无;1:川速雷达;2:苏江车检器"
  		  	, "外部设备类型"
  		  	, ""
  		  	, CUSTOM_LEVEL
  		);
		}
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CSWLPRTollGateParameter::Init368(VOID)
{
	if(S_OK == CSWLPRParameter::Init368())
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
		return S_OK;
	}
	return E_FAIL;
}