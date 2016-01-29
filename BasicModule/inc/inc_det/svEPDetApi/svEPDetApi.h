/// @file
/// @brief svEPDetApi声明
/// @author ganzz
/// @date 2014/1/2 10:44:33
///
/// 修改说明:
/// [2014/1/2 10:44:33 ganzz] 最初版本

#pragma once

#include "svBase/svBase.h"

/// 前置声明，兼容旧车牌检测识别模型接口
namespace HvCore
{
    struct IHvModel;  //#include "hvinterface.h"
}
struct RECTA;

/// svEPDetApi命名空间
namespace svEPDetApi
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
    };

    struct EP_DET_PARAM
    {
        // 场景参数, 相对坐标
        sv::SV_RECT rcDetArea;  /// 视频检测区域，相对于原图顶部百分比*100，0~100
        int nTrackPos;

        // 车道标识座标
        SV_ROAD_INFO rgRoadLine[5]; /// 车道线, 目前最大只支持4车道
        int iRoadInfoCount;         /// 车道线数

        int nTh; //过滤平滑区域域值

        float fltXScale;
        float fltYScale;

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

        int nPlateDetect_Green;     /// 绿牌识别开关
        float fltPlateDetect_StepAdj;   /// 步长调整系数

        EP_DET_PARAM()
            :   g_nDetMinScaleNum(6)   // 车牌检测框的最小宽度=56*1.1^g_nDetMinScaleNum
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
        {
            nTrackPos = 70;

            rcDetArea.m_nLeft = 2;
            rcDetArea.m_nTop = 40;
            rcDetArea.m_nRight = 98;
            rcDetArea.m_nBottom = 99;

            fltXScale = 0.25f;
            fltYScale = 0.25f;
        }
    };

    /// 电警控制接口
    class CEPDetCtrl
    {
    public:

        enum LIGHT_TYPE
        {
            LT_DAY,
            LT_DUSK,
            LT_NIGHT
        };

        virtual ~CEPDetCtrl() {};

    public:
        /// 加载车身检测模型接口
        virtual sv::SV_RESULT LoadDetModel(
            MOD_DET_INFO* pModDetInfo
            ) = 0;

        /// 释放车身检测模型
        virtual sv::SV_RESULT ReleaseDetModel() = 0;

        /// 初始化函数
        virtual sv::SV_RESULT Init(EP_DET_PARAM* pParam) = 0;

        /// 设置车牌检测分割识别库模型接口
        virtual sv::SV_RESULT SetHvModel(HvCore::IHvModel* pHvModel) = 0;

        /// 处理一帧
        virtual sv::SV_RESULT Process(
            sv::SV_IMAGE imgFrame,  /// 待检图片
            LIGHT_TYPE nLightTpye,  /// 环境亮度类型
            DET_ROI* rgDetROI,      /// 返回检测结果接口数组
            int nMaxDetROI,         /// 最大检测结果接口数组
            int* pDetROICnt,        /// 返回检测结果接口数组个数
            RECTA* rgPlateROI,      /// 返回车牌结果接口数组
            int nMaxPlateROI,       /// 最大车牌结果接口数组
            int* pPlateROICnt          /// 返回车牌结果接口数组个数
            ) = 0;
    };

    /// 电警总控创建接口
    CEPDetCtrl* CreateEPDetCtrl();

    /// 电警总控释放接口
    void FreeEPDetCtrl(CEPDetCtrl* pCtrl);

} // svEPDetApi
