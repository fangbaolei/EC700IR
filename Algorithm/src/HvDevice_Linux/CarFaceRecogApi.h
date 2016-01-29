/// @file
/// @brief CarFaceRecogApi声明
/// @author liaoy
/// @date 2014/6/6 10:23:18
///
/// 修改说明:
/// [2014/8/27 10:00:00 yangws] 增加提取特征和匹配特征
/// [2014/8/18 14:00:00 yangws] 增加SVM模型处理
/// [2014/6/6 10:23:18 liaoy] 最初版本

#pragma once
#include "svBase/svBase.h"

/// CarFaceRecogApi命名空间
namespace CarFaceRecogApi
{
    /// 获取取版本信息
    const char* GetRevInfo();

    /// 车脸参数结构体
    struct SCAN_FACE_PARAM
    {
        /// 车牌左侧初始扩展比例(10Bit量化)
        int m_nInitLeftExtRatio[4];
        /// 车牌右侧初始扩展比例(10Bit量化)
        int m_nInitRightExtRatio[4];
        /// 车牌底部初始扩展比例(10Bit量化)
        int m_nInitBottomExtRatio[4];
        /// 车脸高度初始扩展比例(10Bit量化)
        int m_nInitHeightExtRatio[4];
        /// 车牌上部扩展比例(10Bit量化)
        int m_nOutTopExtRatio[4];
        /// 车牌底部扩展比例(10Bit量化)
        int m_nOutBottomExtRatio[4];
        /// 车脸高宽比(10Bit量化)
        int m_nOutH2WRatio[4];

        /// 构造函数
        SCAN_FACE_PARAM()
        {
            m_nInitRightExtRatio[0] = m_nInitLeftExtRatio[0] = 1413;
            m_nInitRightExtRatio[1] = m_nInitLeftExtRatio[1] = 1843;
            m_nInitRightExtRatio[2] = m_nInitLeftExtRatio[2] = 1413;
            m_nInitRightExtRatio[3] = m_nInitLeftExtRatio[3] = 1843;

            m_nInitBottomExtRatio[0] = 512;
            m_nInitBottomExtRatio[1] = 205;
            m_nInitBottomExtRatio[2] = 512;
            m_nInitBottomExtRatio[3] = 0;

            m_nInitHeightExtRatio[0] = 3072;
            m_nInitHeightExtRatio[1] = 2560;
            m_nInitHeightExtRatio[2] = 3072;
            m_nInitHeightExtRatio[3] = 2560;

            m_nOutTopExtRatio[2] = m_nOutTopExtRatio[0] = 317;
            m_nOutTopExtRatio[3] = m_nOutTopExtRatio[1] = 317;

            m_nOutBottomExtRatio[2] = m_nOutBottomExtRatio[0] = 512;
            m_nOutBottomExtRatio[3] = m_nOutBottomExtRatio[1] = 512;

            m_nOutH2WRatio[2] = m_nOutH2WRatio[0] = 585;
            m_nOutH2WRatio[3] = m_nOutH2WRatio[1] = 630;
        }
    };

    class ICarFaceRecogCtrl
    {
    public:
        virtual ~ICarFaceRecogCtrl() {}
    public:
        /// 加载模型
        /// @note 1. 接口生命期内模型数据不能释放,内部用指标解析
        ///       2. 相同TYPE可加载不同的模型,合并进行识别.
        ///       3. 模型数限制最多为10个
        virtual sv::SV_RESULT LoadModel(
            int nTypeID,                    ///< 识别类别ID
            sv::SV_UINT8* pModelBuf,        ///< 模型缓存
            int nModelLen                   ///< 模型缓存长度
        ) = 0;

        /// 加载svm模型
        virtual sv::SV_RESULT LoadSvmModel(
            int nTypeID,                    ///< 识别类别ID
            sv::SV_UINT8* pModelBuf,        ///< 模型缓存
            int nModelLen                   ///< 模型缓存长度
        ) = 0;

        /// 释放所有模型
        virtual sv::SV_RESULT FreeModel(
        ) = 0;

        ///车脸识别
        virtual sv::SV_RESULT RecogCarFace(
            sv::SV_IMAGE* pImgSrc,                    ///原图像
            sv::SV_BOOL fIsNight,        ///场景模式
            sv::SV_BOOL fNoPlate,     //是否是无牌车
            sv::SV_RECT* pPlatePos,        ///车牌位置
            sv::SV_BOOL fYellowPlate,          ///车牌颜色
            sv::SV_RECT* pCarFacePos,    ///车脸位置
            int nIDCount,
            sv::SV_UINT32* rgRegID                   ///识别类别ID
        ) = 0;

        /// 定位车脸
        /// @note 有牌车车牌位置为实际位置,车脸位置为默认值0,
        ///       无牌车车牌位置为默认值0，车脸位置为检测位置
        virtual sv::SV_RESULT ScanCarFaceApi(
            const sv::SV_IMAGE* pImage,         ///< 车辆大图
            sv::SV_RECT* pRect,                 ///< 车牌位置
            const SCAN_FACE_PARAM* pScanFaceParam,///< 车脸提取参数
            sv::SV_BOOL fYellowPlate,           ///< 是否黄牌
            int nEnvType,                       ///< 场景模式:白天(0)/晚上(1)
            sv::SV_RECT* pRectFace              ///< 车脸位置
        ) = 0;

        /// 识别
        /// @note 相同TYPE的图像尺寸要保持一致,否则影响性能
        virtual sv::SV_RESULT RecogSamp(
            int nTypeID,                        ///< 识别类别ID
            const sv::SV_IMAGE* pImgSamp,       ///< 待识别的样本
            int nMaxMatch,                  ///< 最大匹配数
            sv::SV_UINT32* rgID,                ///< 结果ID数组
            sv::SV_UINT32* rgConf               ///< 结果可信度
        ) = 0;

        /// Svm识别
        /// @note 相同TYPE的图像尺寸要保持一致,否则影响性能
        virtual sv::SV_RESULT RecogSampBySvm(
            int nTypeID,                        ///< 识别类别ID
            const sv::SV_IMAGE* pImgSamp,       ///< 待识别的样本
            int nMaxMatch,                  ///< 最大匹配数
            sv::SV_UINT32* rgID,                ///< 结果ID数组
            sv::SV_INT32* rgConf               ///< 结果可信度
        ) = 0;

        /// 提取特征
        /// @note *pnFeatBufLen必须初始化,初始值为设定的最大长度,返回实际长度
        virtual sv::SV_RESULT ExtractFeature(
            const sv::SV_IMAGE* pImgSamp,     ///< 样本图像
            sv::SV_BOOL fYellowPlate,         ///< 是否黄牌
            sv::SV_UINT8* pFeatBuf,           ///< 特征数据缓存
            int* pnFeatBufLen                 ///< 特征缓存长度
        ) = 0;

        /// 特征配准
        virtual sv::SV_RESULT MatchFeatures(
            const sv::SV_UINT8* pFeatDat,    ///< 特征数据
            int nFeatDatLen,                 ///< 特征数据长度
            int nMaxMatch,                   ///< 最大匹配数
            sv::SV_UINT32* rgID,             ///< 结果ID数组
            sv::SV_INT32* rgConf             ///< 结果可信度
        ) = 0;
    };

    ICarFaceRecogCtrl* CreateCarFaceRecogCtrl();
    sv::SV_RESULT FreeCarFaceRecogCtrl(ICarFaceRecogCtrl* pCtrl);
} // CarFaceRecogApi
