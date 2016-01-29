/// @file
/// @brief svTLDetApi声明
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

/// svTLDetApi命名空间
namespace svTLDetApi
{
    /// 获取取版本信息
    const char* GetRevInfo();

    struct TL_DET_PARAM
    {
        // 场景参数, 相对坐标
        int nRoadX00;   // 左上X
        int nRoadY00;   // 左上Y
        int nRoadX01;   // 左下X
        int nRoadY01;   // 左下Y
        int nRoadX10;   // 右上X
        int nRoadY10;   // 右上Y
        int nRoadX11;   // 右上X
        int nRoadY11;   // 右上Y

        int nStartDetLine;  // 起始检测线, 图像高度百分比,0~100
        int nEndDetLine;    // 停止检测线, 图像高度百分比, 0~100

        // for plate detection
        int nDetMinScaleNum;          /// 车牌检测框的最小宽度=56*1.1^nDetMinScaleNum
        int nDetMaxScaleNum;          /// 车牌检测框的最大宽度=56*1.1^nDetMaxScaleNum

        TL_DET_PARAM()
            :   nDetMinScaleNum(5)   // 车牌检测框的最小宽度=56*1.1^nDetMinScaleNum
            ,   nDetMaxScaleNum(9)  // 车牌检测框的最大宽度=56*1.1^nDetMaxScaleNum
        {
            nRoadX00 = 70;
            nRoadY00 = 50;
            nRoadX01 = 30;
            nRoadY01 = 75;
            nRoadX10 = 86;
            nRoadY10 = 50;
            nRoadX11 = 78;
            nRoadY11 = 75;
            nStartDetLine = 35;
            nEndDetLine = 65;
        }
    };

    ///车牌颜色
    enum SV_PLATE_COLOR
    {
        PC_UNKNOWN = -1,    ///<未知
        PC_BLUE    = 0,     ///<蓝色
        PC_BLACK   = 1,     ///<黑色
        PC_GREEN   = 2,     ///<绿色
        PC_YELLOW  = 3,     ///<黄色
        PC_WHITE   = 4      ///<白色
    };

    ///车牌类型
    enum SV_PLATE_TYPE
    {
        PT_UNKNOWN       = -1,       ///<未知
        PT_POLICE        = 0,        ///<普通警牌
        PT_LOCALPOLICE   = 1,        ///<地方警牌
        PT_MILITARY      = 2,        ///<军牌
        PT_WJ            = 3,        ///<武警牌
        PT_MH            = 4,        ///<民航牌
        PT_SHI           = 5,        ///<使馆牌
        PT_MOTO          = 6         ///<摩托车牌
    };

    struct SV_PLATERECT
    {
        sv::CSvRect cPlatePos;           ///<车牌位置SV_RECT
        sv::SV_INT32 nPlateLine;            ///<车牌字符行数，1：单层牌，2：双层牌
        sv::SV_BOOL fWhiteChar;             ///<黑白牌标识，0：黑牌，1：白牌
        SV_PLATE_COLOR nPlateColor;     ///<车牌颜色？API层
        SV_PLATE_TYPE nPlateType;       ///<车牌类型？
        sv::SV_UINT32 nVariance;            ///<车牌方差阈值
        sv::SV_INT32 nConf;                 ///<车牌可信度
        sv::SV_INT32 nFlag;                 ///<车牌标识，综合使用
    };

    /// 收费站控制接口
    class CTLDetCtrl
    {
    public:

        virtual ~CTLDetCtrl() {};

    public:
        /// 初始化函数
        virtual sv::SV_RESULT Init(TL_DET_PARAM* pParam) = 0;

        /// 处理一帧
        virtual sv::SV_RESULT Process(
            sv::SV_IMAGE imgFrame,  /// 待检图片
            int* pnAvgY,            /// 检测区平均亮度
            SV_PLATERECT* rgPlateROI,      /// 返回车牌结果接口数组
            int nMaxPlateROI,       /// 最大车牌结果接口数组
            int* pPlateROICnt          /// 返回车牌结果接口数组个数
        ) = 0;
    };

    /// 总控创建接口
    CTLDetCtrl* CreateTLDetCtrl();

    /// 总控释放接口
    void FreeTLDetCtrl(CTLDetCtrl* pCtrl);

    //////////////////////////////////////////////////////////////////////////
    /// 模型收费站检测
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
            NIGHT_SMALL_CAR = 1,
            TYPE_COUNT = 2
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

    struct TL_CAR_DET_PARAM
    {
        // 场景参数, 相对坐标
        int nRoadX00;   // 左上X
        int nRoadY00;   // 左上Y
        int nRoadX01;   // 左下X
        int nRoadY01;   // 左下Y
        int nRoadX10;   // 右上X
        int nRoadY10;   // 右上Y
        int nRoadX11;   // 右上X
        int nRoadY11;   // 右上Y

        int nStartDetLine;  // 起始检测线, 图像高度百分比,0~100
        int nEndDetLine;    // 停止检测线, 图像高度百分比, 0~100

        float fltXScale;
        float fltYScale;

        int nNightEnvLightTh;  // 晚上环境亮度阈值，低于该值则晚上

        TL_CAR_DET_PARAM()
            :   fltXScale(0.25f)
            ,   fltYScale(0.25f)
        {
            nRoadX00 = 70;
            nRoadY00 = 50;
            nRoadX01 = 30;
            nRoadY01 = 75;
            nRoadX10 = 86;
            nRoadY10 = 50;
            nRoadX11 = 78;
            nRoadY11 = 75;
            nStartDetLine = 35;
            nEndDetLine = 65;
            nNightEnvLightTh = 50;
        }
    };

    /// 收费站控制接口
    class CTLCarDetCtrl
    {
    public:

        virtual ~CTLCarDetCtrl() {};

    public:
        /// 初始化函数
        virtual sv::SV_RESULT Init(TL_CAR_DET_PARAM* pParam) = 0;
        
        virtual sv::SV_RESULT LoadDetModel(
            MOD_DET_INFO* pParam
            ) = 0;
        virtual sv::SV_RESULT ReleaseDetModel() = 0;

        /// 处理一帧
        virtual sv::SV_RESULT Process(
            sv::SV_IMAGE imgFrame,  /// 待检图片
            DET_ROI* rgDetROI,      /// 返回检测结果接口数组
            int nMaxDetROI,         /// 最大检测结果接口数组
            int* pDetROICnt         /// 返回检测结果接口数组个数
            ) = 0;
    };

    /// 总控创建接口
    CTLCarDetCtrl* CreateTLCarDetCtrl();

    /// 总控释放接口
    void FreeTLCarDetCtrl(CTLCarDetCtrl* pCtrl);

} // svTLDetApi
