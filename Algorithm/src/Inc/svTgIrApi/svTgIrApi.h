/// @file
/// @brief svTgIrApi声明
/// @author ganzz
/// @date 2014/11/17 10:20:53
///
/// 修改说明:
/// [2014/11/17 10:20:53 ganzz] 最初版本

#pragma once

#include "svBase/svBase.h"

/// 抓拍回调函数，返回是否触发成功了
typedef bool (*FUNC_CALLBACK_TRIIGER)(
    sv::SV_RECT rcPos,     /// 回调的位置
    int nTrackID,          ///
    void* pVoid            /// 自定义数据
);

/// svTgIrApi命名空间
namespace svTgIrApi
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

    /// 车牌颜色
    enum PLATE_COLOR
    {
        PC_UNKNOWN = 0,
        PC_BLUE,
        PC_YELLOW,
        PC_BLACK,
        PC_WHITE,
        PC_LIGHTBLUE,
        PC_GREEN,
        PC_COUNT
    };

    /// 车牌类型
    enum PLATE_TYPE
    {
        PLATE_UNKNOWN = 0,                    //未知
        PLATE_NORMAL,                         //所有蓝牌，黑牌
        PLATE_WJ,                             //武警车牌
        PLATE_POLICE,                         //包括所有单行黄牌，及与黄牌格式相同的警车
        PLATE_POLICE2,                        //辽-A1234-警类车牌
        PLATE_MILITARY,                       //军货
        PLATE_DOUBLE_YELLOW,                  //双行黄牌，双行白牌
        PLATE_DOUBLE_MOTO,                    //双行摩托牌
        PLATE_INDIVIDUAL,                     //个性化车牌
        PLATE_DOUBLE_GREEN,                   //双层绿牌
        PLATE_DOUBLE_WJ,                      //武警车牌
        PLATE_MH,                             //民航车牌
        PLATE_SHI,                            //使馆牌
        PLATE_DOUBLE_MILITARY,                //双层军牌
        PLATE_TYPE_COUNT
    };

    //车身颜色
    typedef enum
    {
        CC_UNKNOWN = 0,         //未知
        CC_WHITE,               //白
        CC_GREY,                //灰
        CC_BLACK,               //黑
        CC_RED,                 //红
        CC_YELLOW,              //黄
        CC_GREEN,               //绿
        CC_BLUE,                //蓝
        CC_COUNT
    } CAR_COLOR;

    /// 车辆模型检测参数
    struct MOD_DET_INFO
    {
        /// 车身检测模型类型
        enum MOD_DET_INFO_TYPE
        {
            TYPE_UNKNOW =       -1,
            DAY_SMALL_CAR =     0,
            DAY_LARGE_CAR =     1,
            DUSK_SMALL_CAR =    2,
            DUSK_LARGE_CAR =    3,
            DUSK_SMALL_CAR_EX = 4,
            DUSK_LARGE_CAR_EX = 5,
            NIGHT_SMALL_CAR =   6,
            NIGHT_LARGE_CAR =   7,
            TYPE_COUNT =        8
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
    };

    /// 车牌检测结果信息
    /// 与内部车牌信息保持一致
    struct TG_PLATE_INFO
    {
        sv::CSvRect rcPos;          /// 最近识别车牌位置
        PLATE_COLOR nColor;
        PLATE_TYPE nPlateType;
        sv::SV_UINT8 rgbContent[8];     /// 车牌字符信息
        sv::CSvRect rcPlatePos;         /// 最近识别车牌分割后的外围位置，以rcPos起始的相对位置
        int iHL;                        /// 车牌位置亮度
        sv::SV_UINT32 nVariance;        /// 车牌对比度
        sv::SV_UINT32 nAvgY;            /// 车牌亮度
        float fltDetConf;               /// 检牌可信度
        int nInValidCharCount;          /// 车牌中识别无效的字符数

        sv::SV_FLOAT rgfltConf[8];      /// 车牌字符可信度数组
        sv::SV_FLOAT fltTotalConf;      /// 整牌可信度

        TG_PLATE_INFO()
        {
        }
    };

    /// 每帧跟踪可获取的扩展信息
    struct TG_TRACK_EX_INFO
    {
        /// 所在车道号，左起0。投票前为当前帧车道号，投票后以投票为准
        int nRoadNum;
        CAR_COLOR nCarColor;            /// 当前识别颜色
        sv::SV_BOOL fIsTrackingMiss;    /// 当前帧是否跟踪丢失
        sv::CSvRect rcPredictCur;      /// 当前帧预测位置，一般fIsTrackingMiss为True时使用
        sv::CSvRect rcPredict80ms;      /// 80ms预测位置
        sv::CSvRect rcPredict160ms;     /// 160ms预测位置
    };

    /// 轨迹单位
    struct TG_TRACK_LOCUS
    {
        sv::SV_RECT m_rcPos;
        TG_PLATE_INFO* m_pPlateInfo;  // 车牌指针，当该帧检不到牌时为空
        int m_nPlateSimilarCnt;             //与所有车牌的投票结果的相似度
        sv::SV_UINT32 m_nFrameTime;
    };

    /// 物体跟踪结果接口
    class ITgTrack
    {
    public:
        enum TRACK_STATE
        {
            TS_FREE = -1,   /// 跟踪无效状态，无论如何，应用不会取到该状态
            TS_NEW = 0,     /// 表示处于新建立的不可信状态，并非仅首次输出有该状态
            TS_TRACKING = 1,   /// 常规跟踪状态
            TS_END = 2         /// 跟踪结束，即将被重置状态，将于下帧不再输出
        };
        enum TRACK_TYPE
        {
            TT_UNKNOW = -1,
            TT_SMALL_CAR = 0,
            TT_MID_CAR = 1,
            TT_LARGE_CAR = 2,
            TT_BIKE = 3,        /// 非机动车
            TT_WALK_MAN = 4         /// 行人
        };

        struct TG_RESULT_INFO
        {
            char szPlate[16];
            PLATE_COLOR nPlateColor;
            PLATE_TYPE nPlateType;
            sv::SV_UINT8 nPlateAvgLight;            /// 车牌平均亮度
            int nPlateVariance;
            sv::SV_UINT8 rgPlateContent[8];         /// 车牌字符信息
            sv::SV_INT32 nPlateRecogInvalidCount;   /// 车牌识别无效的次数
            sv::SV_FLOAT fltPlateFirstConf;         /// 首字可信度
            sv::SV_FLOAT fltPlateTotalConf;         /// 整牌可信度
            sv::SV_FLOAT fltPlateDetConf;

            int nPlateResSimilaryCount;  /// 所有车牌与车牌投票结果相似数，仅跟踪结束后有效

            /// 取投票得到的车牌类型在本跟踪所有车牌中的计数，仅跟踪结束后有效
            int nVotePlateTypeCount;

            ITgTrack::TRACK_TYPE nTrackType;  /// 车辆类型

            int nValidDetCount;             /// 无牌有效检测数
            CAR_COLOR nCarColor;            /// 车身颜色
            sv::SV_FLOAT fltSpeed;          /// 车速
        };
    public:
        virtual unsigned int GetID() = 0;

        virtual TRACK_STATE GetState() = 0;

        virtual TRACK_TYPE GetType() = 0;

        /// 获取轨迹信息，包括位置、车牌
        virtual const TG_TRACK_LOCUS& GetLocus(int nIndex) = 0;

        /// 取当前时刻最后一个轨迹
        virtual sv::SV_RECT GetLastPos() = 0;

        /// 取轨迹总数
        virtual int GetLocusCount() = 0;

        /// 取车牌总数
        virtual const TG_PLATE_INFO& GetPlate(int nIndex) = 0;

        /// 取车牌总数
        virtual int GetPlateCount() = 0;

        /// 取扩展信息
        virtual sv::SV_RESULT GetExInfo(TG_TRACK_EX_INFO* pExInfo) = 0;

        /// 结束该跟踪，会转成 TS_END 状态，且下帧不会再输出。
        virtual void End() = 0;

        /// 获取结果信息，仅在跟踪结束时（TS_END状态）才会有该信息
        virtual sv::SV_RESULT GetResult(TG_RESULT_INFO* pResult) = 0;
    };

    // 参数
    struct TG_PARAM
    {
        // 场景参数, 相对坐标
        int nDetTopLine;        /// 检测起始Y，百分比*100
        int nDetBottomLine;     /// 检测结束Y，百分比*100

        // 车道标识座标
        SV_ROAD_INFO rgRoadLine[5]; /// 车道线, 目前最大只支持4车道
        int nRoadLineCount;         /// 车道线数

        float fltXScale;
        float fltYScale;

        int nMergeOverlapRatio;  /// 检测合并率 0~100

        int nDuskMaxLightTH;
        int nNightMaxLightTH;

        // 车牌相关参数
        int nDetMinScaleNum;           /// 车牌检测框的最小宽度=56*1.1^nDetMinScaleNum，-1为自动计算
        int nDetMaxScaleNum;           /// 车牌检测框的最大宽度=56*1.1^nDetMaxScaleNum，-1为自动计算

        sv::SV_BOOL fEnableAlpha_5;     ///黄牌字母识别开关
        float fltMinPlateConfTH;        /// 最低车牌可信度阈值
        sv::SV_BOOL fEnableGreenPlate;
        int nBlackPlate_S;  /// 黑牌的饱和度上限
        int nBlackPlate_L;  /// 黑牌亮度上限
        int nBlackPlateThreshold_H0;    /// 蓝牌色度下限，低于此值则判定为黑牌
        int nBlackPlateThreshold_H1;    /// 蓝牌色度上限，高于此值则判定为黑牌
        int nProcessPlate_LightBlue;    /// 浅蓝牌开关
        sv::SV_BOOL fEnableDoublePlate;     /// 双层牌开关
        sv::SV_BOOL fEnableShiGuanPlate;    /// 使用使馆牌检测

        // 行人速度比阈值，每帧移动位移相对路宽百分比*100。
        // 非机动车速度低于该阈值则判为行人
        int nWalkManSpeedRatioTH;

        // 有牌跟踪输出线占图像高度的百分比*100，如30，为过了30%才能出有牌结果
        int nPlateTrackerOutLine;
        // 车牌输出线模式，0:只要没过线就不出，1:没过线且运行缓慢才不给出。
        int nPlateOutLineMode;

        // 傍晚、晚上触发抓拍线
        int nTriggerLine;

        /// 使能软件测速
        sv::SV_BOOL fEnableCalcSpeed;
        // 车道实际宽度
        sv::SV_FLOAT fltRoadWidth;
        // 车道实际长度(图像中最远端到最近端的距离)
        sv::SV_FLOAT fltRoadLength;

        /// 使能车身颜色识别，仅白天有效，傍晚、晚上无效
        sv::SV_BOOL fEnableRecogCarColor;

        TG_PARAM()
            :   nDetTopLine(30)
            ,   nDetBottomLine(75)
            ,   fltXScale(0.25f)
            ,   fltYScale(0.5f)
            ,   nRoadLineCount(0)
            ,   nMergeOverlapRatio(50)
            ,   nDuskMaxLightTH(70)
            ,   nNightMaxLightTH(20)

            ,   nDetMinScaleNum(-1)
            ,   nDetMaxScaleNum(-1)
            ,   fEnableAlpha_5(TRUE)
            ,   fltMinPlateConfTH(0.02f)
            ,   fEnableGreenPlate(FALSE)
            ,   nBlackPlate_S(10)
            ,   nBlackPlate_L(60)
            ,   nProcessPlate_LightBlue(1)
            ,   fEnableDoublePlate(1)
            ,   nBlackPlateThreshold_H0(100)
            ,   nBlackPlateThreshold_H1(200)
            ,   fEnableShiGuanPlate(FALSE)
            ,   nWalkManSpeedRatioTH(2)
            ,   nPlateTrackerOutLine(0)
            ,   nPlateOutLineMode(0)
            ,   nTriggerLine(60)
            ,   fEnableCalcSpeed(FALSE)
            ,   fltRoadWidth(3.75f)
            ,   fltRoadLength(55.0f)
            ,   fEnableRecogCarColor(FALSE)
        {
            memset(rgRoadLine, 0, sizeof(rgRoadLine));
        }
    };

    /// 控制接口
    class CTgCtrl
    {
    public:

        enum LIGHT_TYPE
        {
            LT_DAY,
            LT_DUSK,
            LT_NIGHT
        };

        struct TG_CTRL_INFO
        {
            /// 环境光线类型
            LIGHT_TYPE nLightType;
            int nAvgBrightness;
        };

        virtual ~CTgCtrl() {};

    public:
        /// 加载车身检测模型接口
        virtual sv::SV_RESULT LoadDetModel(
            MOD_DET_INFO* pModDetInfo
        ) = 0;

        /// 加载识别模型
        virtual sv::SV_RESULT LoadRecogModel(
            char* szModelName,    //< 模型名，如"CHNSF"
            void* pBuf,
            int nBufLen
        ) = 0;

        /// 释放车身检测模型
        virtual sv::SV_RESULT ReleaseDetModel() = 0;

        /// 初始化函数
        virtual sv::SV_RESULT Init(const TG_PARAM* pParam) = 0;

        /// 处理一帧
        virtual sv::SV_RESULT Process(
            sv::SV_IMAGE imgFrame,      /// 待检图片
            sv::SV_UINT32 nFrameTime,   /// 帧时间
            DET_ROI* rgDetROI,          /// 传入物体检测框数组
            int nDetROICnt,             /// 传入物体检测框个数
            ITgTrack* rgpITrack[],      /// 返回检测结果接口数组
            int nMaxITrack,             /// 最大检测结果接口数组
            int* pITrackCnt,            /// 返回检测结果接口数组个数
            TG_CTRL_INFO* pInfo         /// 获取全局信息
        ) = 0;

        // 识别一帧，用于抓拍识别
        virtual sv::SV_RESULT RecogOneFrame(
            sv::SV_IMAGE imgFrame,           /// 抓拍检图片
            sv::SV_UINT32 nFrameTime,        /// 帧时标
            sv::SV_RECT rcPlateDetAear,      /// 车牌检测区域，实际坐标
            TG_PLATE_INFO* rgPlateRes,       /// 输出区域内检到的所有车牌
            int nMaxPlateRes,                /// 最大车牌结果数
            int& nPlateResCnt,               /// 返回的车牌结果数
            int nDetMinScaleNum = -1,        /// 车牌检测框的最小宽度=56*1.1^nDetMinScaleNum，-1为自动计算
            int nDetMaxScaleNum = -1         /// 车牌检测框的最大宽度=56*1.1^nDetMaxScaleNum，-1为自动计算
        ) = 0;

        virtual void SetTriggerCallBack(FUNC_CALLBACK_TRIIGER func, void* pVoid) = 0;

        // 强制傍晚模式，需Process前调用，Process调用后失效，下帧需重新调用
        virtual void ForceDuskEnvLightType() = 0;

        // 强制白天模式，需Process前调用，Process调用后失效，下帧需重新调用
        virtual void ForceDayEnvLightType() = 0;
    };

    /// 总控创建接口
    CTgCtrl* CreateTgCtrl();

    /// 总控释放接口
    void FreeTgCtrl(CTgCtrl* pCtrl);

    void GetPlateStr(
        const sv::SV_UINT8 rgbContent[],  // 车牌字符
        char* szName                // 输出的车牌名
    );
} // svTgIrApi

