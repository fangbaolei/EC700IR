/// @file
/// @brief svTgIrDetApi声明
/// @author ganzz
/// @date 2014/11/17 10:35:42
///
/// 修改说明:
/// [2014/11/17 10:35:42 ganzz] 最初版本

#pragma once


#include "svBase/svBase.h"

// TODO: 命名空间修改为合适值
/// svTgIrDetApi命名空间
namespace svTgIrDetApi
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
            //BIKE_AND_WALKMAN =    8,
            //BIKE_AND_WALKMAN_EX = 9,
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

    struct TG_DET_PARAM
    {
        // 场景参数, 相对坐标
        int nDetTopLine;  // 检测起始Y
        int nDetBottomLine;    // 检测结束Y

        // 车道标识座标
        SV_ROAD_INFO rgRoadLine[5]; /// 车道线, 目前最大只支持4车道
        int nRoadLineCount;         /// 车道线数

        float fltXScale;
        float fltYScale;

        int nMergeOverlapRatio;  /// 检测合并率 0~100

        sv::SV_BOOL fEnableWalkManDet;      /// 行人及非机动车检测开关

        sv::SV_BOOL fEnableDynamicDetStep;      /// 使能动态检测步长

        TG_DET_PARAM()
        {
            nDetTopLine = 30;
            nDetBottomLine = 75;

            fltXScale = 0.25f;
            fltYScale = 0.5f;

            nRoadLineCount = 0;
            memset(rgRoadLine, 0, sizeof(rgRoadLine));

            nMergeOverlapRatio = 50;
            fEnableWalkManDet = FALSE;
            fEnableDynamicDetStep = TRUE;
        }
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

    /// 控制接口
    class CTgDetCtrl
    {
    public:

        enum LIGHT_TYPE
        {
            LT_DAY,
            LT_DUSK,
            LT_NIGHT
        };

        virtual ~CTgDetCtrl() {};

    public:
        /// 加载车身检测模型接口
        virtual sv::SV_RESULT LoadDetModel(
            MOD_DET_INFO* pModDetInfo
        ) = 0;

        /// 释放车身检测模型
        virtual sv::SV_RESULT ReleaseDetModel() = 0;

        /// 初始化函数
        virtual sv::SV_RESULT Init(const TG_DET_PARAM* pParam) = 0;

        /// 处理一帧
        virtual sv::SV_RESULT Process(
            sv::SV_IMAGE imgFrame,  /// 待检图片
            LIGHT_TYPE nLightTpye,  /// 环境亮度类型
            DET_ROI* rgDetROI,      /// 返回检测结果接口数组
            int nMaxDetROI,         /// 最大检测结果接口数组
            int* pDetROICnt         /// 返回检测结果接口数组个数
        ) = 0;

        /// 样本采集接口
        virtual sv::SV_RESULT SetSampGather(
            ISampGather* pGather
        ) = 0;

        // 强制傍晚模式，需Process前调用，Process调用后失效，下帧需重新调用
        virtual void ForceDuskEnvLightType() = 0;
    };

    /// 电警总控创建接口
    CTgDetCtrl* CreateTgDetCtrl();

    /// 电警总控释放接口
    void FreeTgDetCtrl(CTgDetCtrl* pCtrl);

} // svTgIrDetApi
