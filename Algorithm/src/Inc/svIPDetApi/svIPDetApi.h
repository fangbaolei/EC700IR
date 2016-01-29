/// @file
/// @brief svIPDetApi声明
/// @author ganzz
/// @date 2014/5/28 17:10:58
///
/// 修改说明:
/// [2014/5/28 17:10:58 ganzz] 最初版本

#pragma once
#include "svBase/svBase.h"

// TODO: 命名空间修改为合适值
/// svIPDetApi命名空间
namespace svIPDetApi
{
    /// 获取取版本信息
    const char* GetRevInfo();

    class DET_ROI : public sv::SV_RECT
    {
    public:
        int m_nType;
        int m_nSubType;
        float m_fltConf;
        sv::SV_UINT32 m_nFlag;
    };

    //通用检测实现
    typedef struct _DET_PARAM
    {
        int nDetType; //对应模型的检测类型
        int nDetWidth; //检测比例系数

        int nStepDivX;  //步长系数
        int nStepDivY;

        int nScaleNum;  //SCALE数
        float fltScaleCoef; //SCALE系数, nScaleNum>1有效
        int nMergeNum; //合并数
        int nMergeOverlapRatio; //合并重叠百分比*100，重叠率大于该值将被合并

        int nMaxROI; //最大候选结果数

        _DET_PARAM()
        {
            nDetType = -1;
            nDetWidth = 256;

            nStepDivX = 16;
            nStepDivY = 8;

            nScaleNum = 1;
            fltScaleCoef = 0.9f;
            nMergeNum = 2;
            nMergeOverlapRatio = 90;

            nMaxROI = 1000;
        }
    }
    DET_PARAM;

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

    struct CTRL_PARAM
    {
        int nNightTH;  /// 晚上亮度阈值
        int nEnvLightMaxCheckFrameCount;   /// 环境亮度每组统计的帧数
        /// 环境亮度切换需要达到的帧数阈值，一组统计中被判定为某场景的次数大于该值才能够切换
        int nEnvLightSwitchFramesTH;

        int nL2DetCandidateNum;     // 二级检测候选框数
        int nL2DetVoteNum;          // 二级检测投票数，候选框中满足该数则可出结果

        CTRL_PARAM()
            :   nNightTH(60)
            ,   nEnvLightMaxCheckFrameCount(300)
            ,   nEnvLightSwitchFramesTH(256)
            ,   nL2DetCandidateNum(6)
            ,   nL2DetVoteNum(1)
        {
        }
    };

    class CDetCtrl
    {
    public:
        enum LIGHT_TYPE
        {
            LT_DAY,     /// 白天
            LT_NIGHT    /// 晚上
        };

        virtual ~CDetCtrl() {}
    public:

        //初始化接口，必须先初始化且确保成功
        virtual sv::SV_RESULT Init(CTRL_PARAM* pCtrlParam) = 0;

        //加载模型
        virtual sv::SV_RESULT LoadModel(
            int nType,       // 主类型，0为白天，1为晚上
            int nSubType,    // 子类型
            sv::SV_UINT8* pModelDat,
            int nDataLen
        ) = 0;

        //加载二级模型
        //注意：接口生命期内模型数据不能释放
        virtual sv::SV_RESULT LoadL2Model(
            int nType,       // 主类型，0为白天，1为晚上
            sv::SV_UINT8* pModelDat,
            int nDataLen
        ) = 0;

        //释放模型
        virtual sv::SV_RESULT ReleaseModel() = 0;

        //释放二级模型
        virtual sv::SV_RESULT ReleaseL2Model() = 0;

        //处理一帧
        virtual sv::SV_RESULT Process(
            sv::SV_IMAGE imgFrame,      //待检图片
            int nType,                  //检测类型, 0为白天，1为晚上
            DET_PARAM* rgParam,         //对应类型的检测参数
            DET_ROI* rgResult,          //检测结果
            int nMaxResult,
            int* pResultCount
        ) = 0;

        //计算环境亮度
        virtual sv::SV_RESULT CalcEnvLight(
            sv::SV_IMAGE imgFrame,      //待检图片
            LIGHT_TYPE* pLightType,      //当前环境类型
            int* pCurLight              //当前环境亮度
        ) = 0;

        /// 样本采集接口
        virtual sv::SV_RESULT SetSampGather(
            ISampGather* pGather
        ) = 0;
    };


    CDetCtrl* CreateDetCtrl();
    void FreeDetCtrl(CDetCtrl* pCtrl);

} // svIPDetApi
