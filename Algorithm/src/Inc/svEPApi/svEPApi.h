/// @file
/// @brief svEPApi声明
/// @author ganzz
/// @date 2013/10/28 9:07:38
///
/// 修改说明:
/// [2013/10/28 9:07:38 ganzz] 最初版本

#pragma once

#include "svBase/svBase.h"
#include "swplatetype.h"

/// 前置声明，兼容旧车牌检测识别模型接口
namespace HvCore
{
    struct IHvModel;  //#include "hvinterface.h"
}
struct RECTA;

/// svEPApi命名空间
namespace svEPApi
{
    /// 获取取版本信息
    const char* GetRevInfo();

    /// 检测区域
    class DET_ROI : public sv::SV_RECT
    {
    public:
        int nType;
        int nSubType;
        float fltConf;
        sv::SV_UINT32 dwFlag;
    };

    /// 样本采集接口，仅工控调试使用
    class ISampGather
    {
    public:
        virtual sv::SV_RESULT OnDet(
            sv::SV_IMAGE* pImgFrame,
            sv::SV_RECT rcSamp
        ) = 0;

        virtual sv::SV_RESULT OnROI(
            sv::SV_IMAGE* pImgFrame,
            DET_ROI* pROI
        ) = 0;

        virtual sv::SV_RESULT OnResult(
            sv::SV_IMAGE* pImgFrame,
            DET_ROI* pROI
        ) = 0;
    };

    /// 车辆模型检测参数
    struct MOD_DET_INFO
    {
        /// 车身检测模型类型
        enum MOD_DET_INFO_TYPE
        {
            TYPE_UNKNOW = -1,
            DAY_SMALL_CAR = 0,
            DAY_BIG_CAR = 1,
            NIGHT_SMALL_CAR = 2,
            NIGHT_BIG_CAR = 3,
            TYPE_COUNT = 4
        };

        unsigned char* pbData;
        int nDataLen;

        MOD_DET_INFO_TYPE nDetType; /// 对应模型的检测类型
        float fltRoadRatio; /// 检测比例系数，与车道宽的百分比，0.f~1.f

        int nStepDivX;  /// X步长系数
        int nStepDivY;  /// Y步长系数

        int nScaleNum;  // SCALE数
        float fltScaleCoef; /// SCALE系数, nScaleNum>1有效
        int nMergeNum; /// 合并数

        int nMaxROI; /// 最大候选结果数

        MOD_DET_INFO()
        {
            pbData = NULL;
            nDataLen = 0;

            nDetType = TYPE_UNKNOW;
            fltRoadRatio = 0.6f;

            nStepDivX = 16;
            nStepDivY = 8;

            nScaleNum = 1;
            fltScaleCoef = 0.9f;
            nMergeNum = 2;

            nMaxROI = 1000;
        }
    };

    /// 车道信息（车道线）
    struct SV_ROAD_INFO
    {
        sv::SV_POINT ptTop;     /// 车道线上标定点，相对图像宽的百分比x100，0~100
        sv::SV_POINT ptBottom;  /// 车道线下标定点，相对图像宽的百分比x100，0~100
        int iRoadType;      /// 该线右边车道的类型，目前无效
    } ;

    /// 梯形检测区域
    /// 主要用于确定左右出结果边界
    struct SV_TRAP_TRACK_AREA 
    {
        //四边形扫描区域四个点的坐标，百分比
        int nTopLeftX;
        int nTopLeftY;
        int nTopRightX;
        int nTopRightY;
        int nBottomLeftX;
        int nBottomLeftY;
        int nBottomRightX;
        int nBottomRightY;
        SV_TRAP_TRACK_AREA()
        {
            nTopLeftX = 27;
            nTopLeftY = 24;
            nTopRightX = 67;
            nTopRightY = 24;
            nBottomLeftX = 7;
            nBottomLeftY = 100;
            nBottomRightX = 90;
            nBottomRightY = 100;
        }
    };

    struct EP_PARAM
    {
        //  场景参数, 相对坐标
        sv::SV_RECT rcDetArea;  /// 视频检测区域，相对于原图顶部百分比*100，0~100
        SV_TRAP_TRACK_AREA cTrapTrackArea;  /// 梯形跟踪区域，跟踪超出该区域则出结果
        int iTrackAreaPos;      /// 跟踪区域位置，相对于原图顶部百分比*100，0~100

        // 车道标识座标
        SV_ROAD_INFO rgRoadLine[5]; /// 车道线, 目前最大只支持4车道
        int iRoadInfoCount;         /// 车道线数

        /// 0~3从下到上，三条抓拍线，相对于原图顶部百分比*100，0~100
        int rgiCapturePos[3];

        int nTh; /// 过滤平滑区域域值

        // 物体类型宽度相对路宽百分比*100
        /// 中型车最小宽度相对车道宽百分比*100，0~100，大于等于该值且小于大型车值则为中型车
        int iMidCarMinWidthRatio;
        /// 大型车最小宽度相对车道宽百分比*100，0~100，大于等于该值则为大型车
        int iLargeCarMinWidthRatio;
        /// 小型车最小宽度相对车道宽百分比*100，0~100，大于等于该值且小于大型车值则为大型车
        int iSmallCarMinWidthRatio;
        /// 行人速度比，平均每帧移动位移相对路宽百分比*100，0~100，如果大于该值则为非机动车
        int iWalkManSpeedRatio;

        /// 晚上类型判断Scale系数*100, 因晚上灯光与白天不同，检测出的框大小与白天
        /// 也会不同，物体类型宽度判断参数将乘以该系数再计算。
        /// 如80时，iMidCarMinWidthRatio_night = iMidCarMinWidthRatio*80/100。默认80
        int iNightTypeScaleRatio;

        /// 晚上最大亮度阈值，平均环境亮度低于该值则为晚上，0~155
        int iNightMaxLightTH;
        /// 傍晚最大亮度阈值，平均环境亮度低于该值且高于晚上阈值则为傍晚，否则为白天
        int iDuskMaxLightTH;

        float fltXScale;  /// 检测图像缩数系统
        float fltYScale;  /// 检测图像缩数系统

        int iStopLinePos;  /// 停止线位置(直行)，相对图像高度*100，0~100
        int iLeftStopLinePos; /// 左转道停车线位置与直行停车线的差值，相对图像高度*100，0~100.

        sv::SV_POINT rgptLeftTurnWaitArea[6];   /// 车转待转区域，相连的6个点确定

        // 原PlateRecogParam参数
        float g_fltTrackInflateX;       /// tracking时下一帧区域在X方向上的增大相对于trackrect_width的比例
        float g_fltTrackInflateY;       /// tracking时下一帧区域在Y方向上的增大相对于trackrect_height的比例

        int g_nContFrames_EstablishTrack;       /// of continuously observed frames for establishing a track
        int g_nMissFrames_EndTrack;             /// of missing frames for ending a track
        int g_nMissFrames_EndTrackQuick;        /// of missing frames for ending a track in EndTrackArea
        int g_nObservedFrames_ConsiderAsTrack;  /// of observed frames to be considered as a successful track
        float g_fltThreshold_StartTrack;        /// threshold of a good recognition for starting a track
        float g_fltThreshold_BeObservation;     /// threshold of a good recognition to be an observation
        float g_fltMinConfidenceForVote;        /// minimal confidence for voting, when there are not enought observations

        // for plate detection
        int g_nDetMinScaleNum;          /// 车牌检测框的最小宽度=56*1.1^g_nDetMinScaleNum
        int g_nDetMaxScaleNum;          /// 车牌检测框的最大宽度=56*1.1^g_nDetMaxScaleNum
        int g_nDetMinStdVar;            /// 方差>g_nDetMinStdVar的区域才认为可能是车牌
        int g_nMinStopPlateDetNum;      /// 达到这个数量就停止检测
        sv::SV_BOOL g_fSubscanPredict;         /// tracking时是否使用预测算法
        int g_nSubscanPredictMinScale;  /// tracking预测时scale变化范围的最小值
        int g_nSubscanPredictMaxScale;  /// tracking预测时scale变化范围的最大值

        float g_kfltPlateInflateRate;   /// 检测框增宽的比例
        float g_kfltPlateInflateRateV;  /// 检测框增高的比例
        float g_kfltHighInflateArea;    /// 车牌的y值>g_kfltHighInflateArea后，用第二套更大一点的比例膨胀检测框
        float g_kfltPlateInflateRate2;  /// 更大的检测框增宽的比例
        float g_kfltPlateInflateRateV2; /// 更大的检测框增高的比例

        sv::SV_BOOL g_fSegPredict;             /// 是否使用切分的预测算法

        int g_nMinPlateWidth;           /// 进行识别的最小车牌宽度（以分割后的结果为准）
        int g_nMaxPlateWidth;           /// 进行识别的最大车牌宽度（以分割后的结果为准）
        sv::SV_BOOL g_fBestResultOnly;         /// 如果图像中有多个识别结果，只输出最好的

        int g_nMinDownSampleWidthInSeg;     /// 分割时DownSample的最小宽度
        int g_nMinDownSampleHeightInSeg;    /// 分割时DownSample的最小高度
        // 原PlateRecogParam参数

        int fEnableProcessBWPlate;
        int fEnablePlateEnhance;
        int iPlateResizeThreshold;
        int fEnableBigPlate;
        int iEnableDefaultDBType;   ///默认双层牌类型开关
        int nDefaultDBType;         /// 默认双层牌类型
        int iScrMidFiveCharWidth;   /// 屏幕中间单层蓝牌后五字宽度 old m_iMiddleWidth
        int iScrBotFiveCharWidth;   /// 屏幕底部单层蓝牌后五字宽度 old m_iBottomWidth
        int fUseEdgeMethod;         /// 加强清晰图识别

        int fEnableDefaultWJ;       ///"本地新武警字符开关"
        char strDefaultWJChar[4];
        int fRecogGxPolice;         ///"地方警牌识别开关"
        int nDefaultChnId;          /// 本地化汉字，58为“桂”

        int nRemoveLowConfForVote;  /// 投票前去掉低得分车牌百分比
        int iMinVariance;           /// 过滤非车牌区域阈值
        int iMinPlateBrightness;
        int iMaxPlateBrightness;

        // plate det cfg param
        int nPlateDetect_Green;     /// 绿牌识别开关
        float fltPlateDetect_StepAdj;   /// 步长调整系数

        // recog cfg param
        int fEnableAlphaRecog;  /// 第7位字母识别开关
        int fEnableT1Model;     /// T-1模型开关
        int fEnableAlpha_5;     /// 黄牌字母识别开关

        // proc plate cfg param
        int nBlackPlate_S;      /// 黑牌的饱和度上限
        int nBlackPlate_L;      /// 黑牌亮度上限
        int nBlackPlateThreshold_H0;    /// 蓝牌色度下限
        int nBlackPlateThreshold_H1;    /// 蓝牌色度上限
        int nProcessPlate_LightBlue;    /// 浅蓝牌开关
        int fEnableDoublePlate;         /// 双层牌开关
        int iSegWhitePlate;             /// 强制白牌分割

        int fEnableNewWJSegment;
        int fRecogNewMilitary;
        int fOutputYellowMidCar;        /// 输出黄牌中型车

        int fEnableDynamicDetMinStdVar;  /// 使能动态检测最小标准差


        EP_PARAM()
            :   fEnableProcessBWPlate(TRUE)
            ,   fEnablePlateEnhance(TRUE)
            ,   iPlateResizeThreshold(120)
            ,   fEnableBigPlate(FALSE)
            ,   iEnableDefaultDBType(0)
            ,   nDefaultDBType(0)
            ,   iScrMidFiveCharWidth(0)
            ,   iScrBotFiveCharWidth(0)
            ,   fUseEdgeMethod(TRUE)
            ,   fEnableDefaultWJ(TRUE)
            ,   fRecogGxPolice(TRUE)
            ,   nDefaultChnId(58)
            ,   nRemoveLowConfForVote(40)
            ,   iMinVariance(9)
            ,   iMinPlateBrightness(80)
            ,   iMaxPlateBrightness(120)
            // 以下是原PlateRecogParam参数
            ,   g_nDetMinScaleNum(6)   // 车牌检测框的最小宽度=56*1.1^g_nDetMinScaleNum
            ,   g_nDetMaxScaleNum(10)  // 车牌检测框的最大宽度=56*1.1^g_nDetMaxScaleNum
            ,   g_nDetMinStdVar(16)    // 方差>g_nDetMinStdVar的区域才认为可能是车牌
            ,   g_nMinStopPlateDetNum(0)      // 达到这个数量就停止检测
            ,   g_fSubscanPredict(TRUE)       // tracking时是否使用预测算法
            ,   g_nSubscanPredictMinScale(-2)    // tracking预测时scale变化范围的最小值
            ,   g_nSubscanPredictMaxScale(1)     // tracking预测时scale变化范围的最大值
            ,   g_kfltPlateInflateRate(0.05f)    // 检测框增宽的比例
            ,   g_kfltPlateInflateRateV(0.10f)   // 检测框增高的比例
            ,   g_kfltHighInflateArea(0.0f)     // 车牌的y值>g_kfltHighInflateArea后，用第二套更大一点的比例膨胀检测框
            ,   g_kfltPlateInflateRate2(0.0f)   // 更大的检测框增宽的比例
            ,   g_kfltPlateInflateRateV2(0.0f)  // 更大的检测框增高的比例
            ,   g_fSegPredict(TRUE)             // 是否使用切分的预测算法
            ,   g_nMinPlateWidth(60) // 进行识别的最小车牌宽度（以分割后的结果为准）
            ,   g_nMaxPlateWidth(200)           // 进行识别的最大车牌宽度（以分割后的结果为准）
            ,   g_fBestResultOnly(FALSE)         // 如果图像中有多个识别结果，只输出最好的
            ,   g_nMinDownSampleWidthInSeg(400) // 分割时DownSample的最小宽度
            ,   g_nMinDownSampleHeightInSeg(52) // 分割时DownSample的最小高度

            ,   nPlateDetect_Green(0)
            ,   fltPlateDetect_StepAdj(1.0f)
            ,   fEnableAlphaRecog(FALSE)
            ,   fEnableT1Model(FALSE)
            ,   fEnableAlpha_5(FALSE)
            ,   nBlackPlate_S(10)
            ,   nBlackPlate_L(85)
            ,   nProcessPlate_LightBlue(1)
            ,   fEnableDoublePlate(TRUE)
            ,   nBlackPlateThreshold_H0(100)
            ,   nBlackPlateThreshold_H1(200)
            ,   iSegWhitePlate(0)
            ,   fOutputYellowMidCar(TRUE)
            ,   fEnableDynamicDetMinStdVar(TRUE)
        {
            rcDetArea.m_nLeft = 2;
            rcDetArea.m_nTop = 40;
            rcDetArea.m_nRight = 98;
            rcDetArea.m_nBottom = 99;

            iTrackAreaPos = 55;

            rgRoadLine[0].ptTop.m_nX = 19;
            rgRoadLine[0].ptTop.m_nY = 51;
            rgRoadLine[0].ptBottom.m_nX = 2;
            rgRoadLine[0].ptBottom.m_nY = 79;
            rgRoadLine[1].ptTop.m_nX = 42;
            rgRoadLine[1].ptTop.m_nY = 45;
            rgRoadLine[1].ptBottom.m_nX = 31;
            rgRoadLine[1].ptBottom.m_nY = 93;
            rgRoadLine[2].ptTop.m_nX = 59;
            rgRoadLine[2].ptTop.m_nY = 42;
            rgRoadLine[2].ptBottom.m_nX = 65;
            rgRoadLine[2].ptBottom.m_nY = 92;
            rgRoadLine[3].ptTop.m_nX = 74;
            rgRoadLine[3].ptTop.m_nY = 43;
            rgRoadLine[3].ptBottom.m_nX = 95;
            rgRoadLine[3].ptBottom.m_nY = 81;
            iRoadInfoCount = 4;

            nTh = 4;

            // 0~3从下到上，三条抓拍线，相对于原图顶部百分比*100
            rgiCapturePos[0] = 85;
            rgiCapturePos[1] = 70;
            rgiCapturePos[2] = 40;

            // 物体宽度相对路宽最大百分比*100
            iLargeCarMinWidthRatio = 95;
            iMidCarMinWidthRatio = 85;
            iSmallCarMinWidthRatio = 42;

            // 行人速度比，每帧移动位移相对路宽百分比*100
            iWalkManSpeedRatio = 4;

            iNightTypeScaleRatio = 80;

            iNightMaxLightTH = 22;
            iDuskMaxLightTH = 45;

            fltXScale = 0.25f;
            fltYScale = 0.25f;

            iStopLinePos = 65;
            iLeftStopLinePos = 65;

            memcpy(strDefaultWJChar, "16", 3);
        }
    };

    /// 车牌检测结果信息
    struct EP_PLATE_INFO
    {
        sv::SV_RECT rcPos;          /// 最近识别车牌位置
        PLATE_COLOR color;
        PLATE_TYPE nPlateType;
        sv::SV_UINT8 rgbContent[8];     /// 车牌字符信息
        sv::SV_RECT rcPlatePos;         /// 最近识别车牌分割后的外围位置，以rcPos起始的相对位置
        int iHL;                        /// 车牌位置亮度
        sv::SV_UINT32 nVariance;        /// 车牌对比度
        sv::SV_UINT32 nAvgY;            /// 车牌亮度

        sv::SV_UINT32 nRecogTime;  /// 最近识别车牌的时间

        /// 识别车牌在整个跟踪中的轨迹下标，并不等于检测了就一定识别
        /// 注意：采用隔帧识别机制时，识别将在检测后一帧进行，且该帧不进行检测，
        ///       识别帧的nRecogPosIndex只与nDetPosIndex一致，而并非等于轨迹数-1，
        ///       即该值只表明识别的牌与轨迹下标的关系，与轨迹数不直接相关
        int nRecogPosIndex;
        /// 检测车牌在整个跟踪中的轨迹下标，与轨迹数-1相等则当前帧检到牌
        int nDetPosIndex;
        sv::SV_RECT rcDetPos;  /// 检到车牌位置，注意检到不一定识别

        float fltConf;       /// 整牌可信度，跟踪结束后为投票后的平均可信度
        float fltFirstConf;  /// 首字可信度，跟踪结束后为投票后的平均首字可信度

        int nSimilaryCount;  /// 车牌识别相似数，仅跟踪结束后有效

        /// 取投票得到的车牌类型在本跟踪所有车牌中的计数，仅跟踪结束后有效
        int nVotePlateTypeCount;

        char szPlate[16];   /// 最终车牌字符串，仅跟踪结束后有效
        int nRecogInValidCount;  /// 识别无效次数，仅跟踪结束后有效

        int nPlatePosXOffset; /// 车牌位置X方向偏移百分比，-100~100，0为正中间，用于压线判断

        EP_PLATE_INFO()
            :   color(PC_UNKNOWN)
            ,   nPlateType(PLATE_UNKNOWN)
            ,   iHL(0)
            ,   nVariance(0)
            ,   nAvgY(0)
            ,   nRecogTime(0)
            ,   nRecogPosIndex(-1)
            ,   nDetPosIndex(-1)
            ,   fltConf(0.f)
            ,   fltFirstConf(0.f)
            ,   nSimilaryCount(0)
            ,   nVotePlateTypeCount(0)
        	,   nRecogInValidCount(0)
            ,   nPlatePosXOffset(0)
        {
            rcPos.m_nLeft = rcPos.m_nRight
                            = rcPos.m_nTop = rcPos.m_nBottom = 0;
            rcDetPos.m_nLeft = rcDetPos.m_nRight
                               = rcDetPos.m_nTop = rcDetPos.m_nBottom = 0;
            memset(rgbContent, 0, sizeof(rgbContent));
            szPlate[0] = '\0';
        }
    };

    /// 电警跟踪结果接口
    class IEPTrack
    {
    public:
        enum TRACK_STATE
        {
            TS_FREE = -1,   /// 跟踪无效状态
            TS_NEW = 0,     /// 表示处于新建立的不可信状态，并非仅首次输出有该状态
            TS_TRACKING = 1,   /// 常规跟踪状态
            TS_RESET = 2       /// 跟踪结束，即将被重置状态，将于下帧不再输出
        };
        enum TRACK_TYPE
        {
            TT_UNKNOW = -1,
            TT_SMALL_CAR = 0,
            TT_MID_CAR = 1,
            TT_LARGE_CAR = 2,
            TT_BIKE = 3,
            TT_WALK_MAN = 4
        };

    public:
        virtual int GetID() = 0;
        virtual int GetState() = 0;
        virtual int GetType() = 0;
        virtual int GetPosCount() = 0;
        virtual sv::SV_RECT GetPos(int nIndex) = 0;
        virtual sv::SV_RECT GetLastPos() = 0;
        virtual void Reset() = 0;

        /// 获取车牌
        virtual bool GetPlate(
            EP_PLATE_INFO* pPlateInfo /// 车牌指针
        ) = 0;
        virtual int GetPlateRecogCount() = 0;

        /// 取有效检测数，如果车牌信息不可靠，仍可能为无牌车多检出牌或坏牌
        /// 此时判断有效检测数，如果较高(例如>10)则可判定为无牌车或坏牌车
        virtual int GetValidDetCount() = 0;

        /// 所在车道号，左起0。投票前为当前帧车道号，投票后以投票为准
        virtual int GetRoadNum() = 0;

        /// 取车身颜色
        virtual CAR_COLOR GetColor() = 0;

#if SV_RUN_PLATFORM == SV_PLATFORM_WIN
        virtual sv::SV_BOOL IsModelDetRes()
        {
            return TRUE;
        }
#endif
    };

    struct EP_RESULT_INFO
    {
        /// 环境光线类型
        enum LIGHT_TYPE
        {
            LT_DAY,     /// 白天
            LT_NIGHT    /// 晚上
        };
        LIGHT_TYPE nLightType;
        int nAvgBrightness;
    };

    /// 文本信息输出回调原型
    typedef int (*EP_CALLBACK_CAPTURE)(
        svEPApi::IEPTrack* pTrack,     ///< 跟踪指针
        int nCapPosIndex      ///< 抓拍位置下标:0，1，2，从近到远
    );

    /// 电警控制接口
    class CEPCtrl
    {
    public:

        virtual ~CEPCtrl() {};

    public:
        /// 加载车身检测模型接口
        virtual sv::SV_RESULT LoadDetModel(
            MOD_DET_INFO* pModDetInfo
        ) = 0;

        /// 释放车身检测模型
        virtual sv::SV_RESULT ReleaseDetModel() = 0;

        /// 初始化函数
        virtual sv::SV_RESULT Init(EP_PARAM* pParam) = 0;

        /// 设置车牌检测分割识别库模型接口
        virtual sv::SV_RESULT SetHvModel(HvCore::IHvModel* pHvModel) = 0;

        /// 处理一帧
        virtual sv::SV_RESULT Process(
            sv::SV_IMAGE imgFrame,  /// 待检图片
            sv::SV_UINT32 dwFrameTimeMs,  /// 该帧时标，精确到毫秒
            IEPTrack** rgTrack,     /// 返回跟踪结果接口数组
            int nMaxTrack,          /// 最大跟踪结果接口数组
            int* pTrackCount,       /// 返回跟踪结果接口数组个数
            EP_RESULT_INFO* pResultInfo = NULL  /// 结果信息，详见其类型定义，可为空
        ) = 0;

        /// 处理一帧，异步机制调用此接口，需传入物体检测框、车牌检测框
        virtual sv::SV_RESULT ProcessTrack(
            sv::SV_IMAGE imgFrame,  /// 待检图片
            sv::SV_UINT32 dwFrameTimeMs,  /// 该帧时标，精确到毫秒
            DET_ROI* rgDetROI,      /// 物体检测框数组
            int nDetROICnt,         /// 物体检测框个数
            RECTA* pPlateROI,       /// 车牌检测框数组
            int nPlateROICnt,       /// 车牌检测框个数
            IEPTrack** rgTrack,     /// 返回跟踪结果接口数组
            int nMaxTrack,          /// 最大跟踪结果接口数组
            int* pTrackCount,       /// 返回跟踪结果接口数组个数
            EP_RESULT_INFO* pResultInfo = NULL  /// 结果信息，详见其类型定义，可为空
        ) = 0;

        /// 抓拍回调接口
        virtual sv::SV_RESULT SetCaptureCallBack(
            EP_CALLBACK_CAPTURE pFuncCallBack
        ) = 0;

        /// 样本采集接口
        virtual sv::SV_RESULT SetSampGather(
            ISampGather* pGather
        ) = 0;

    };

    /// 电警总控创建接口
    CEPCtrl* CreateEPCtrl();

    /// 电警总控释放接口
    void FreeEPCtrl(CEPCtrl* pCtrl);

} // svEPApi
