/// @file
/// @brief 图像类型及操作定义
/// @author liaoy
/// @date 2013/8/26 10:46:40
///
/// 修改说明:
/// [2013/8/26 10:46:40 liaoy] 最初版本

#pragma once

#include "sv_basetype.h"
#include "sv_error.h"
#include "sv_memory.h"
#include "sv_rect.h"

namespace sv
{
    /// 图象类型
    enum SV_IMAGE_TYPE
    {
        SV_IMAGE_UNKNOWN    = -1,   ///< 未知格式
        SV_IMAGE_YUV422     = 0,    ///< YUV分离,UV宽度减半
        SV_IMAGE_GRAY       = 1,    ///< 单通道灰度
        SV_IMAGE_RGB        = 2,    ///< 复合通道RGB
        SV_IMAGE_HSV        = 3,    ///< 复合通道HSV
        SV_IMAGE_ITG        = 4     ///< 单通道积分图
    };

    /// 图象结构体(位宽,通道数可变)
    template<class T, int N>
    struct SV_IMAGE_T
    {
        /// 图象类型
        SV_IMAGE_TYPE m_nType;
        /// 图象宽度
        int m_nWidth;
        /// 图象高度
        int m_nHeight;
        /// 图象内存行宽度(以字节为单位)
        int m_rgStrideWidth[N];
        /// 图象数据指针
        T* m_pData[N];

        SV_IMAGE_T()
        {
            m_nType = SV_IMAGE_UNKNOWN;
            m_nWidth = 0;
            m_nHeight = 0;

            for(int i = 0; i < N; i++)
            {
                m_rgStrideWidth[i] = 0;
                m_pData[i] = 0;
            }
        }
    };

    /// 8位3通道图像
    typedef SV_IMAGE_T<SV_UINT8, 3> SV_IMAGE;

    /// 32位单通道积分图
    typedef SV_IMAGE_T<SV_UINT32, 1> SV_ITG;

    /// 图像类
    class CSvImage : public SV_IMAGE
    {
    public:
        /// 构造函数
        CSvImage()
        {
            return;
        }
        /// 析构函数
        ~CSvImage()
        {
            Clear();
            return;
        }

    public:
        /// 清理函数
        void Clear()
        {
            FreeData();

            m_nType =  SV_IMAGE_UNKNOWN;
            m_nWidth = 0;
            m_nHeight = 0;
        }

        /// 释放图像内存,保留基本信息
        void FreeData()
        {
            m_cAlloc.Free();

            for(int i = 0; i < 3; i++)
            {
                m_rgStrideWidth[i] = 0;
                m_pData[i] = NULL;
            }
        }

        /// 拷贝函数
        SV_RESULT Copy(
            const SV_IMAGE* pImgSrc,    ///< 源图像
            SV_BOOL fFastMem = FALSE    ///< 是否拷贝到片内
        )
        {
            SV_BOOL fReCreate = FALSE;

            if(m_nType != pImgSrc->m_nType ||
                    m_nWidth < pImgSrc->m_nWidth ||
                    m_nHeight < pImgSrc->m_nHeight ||
                    fFastMem == TRUE
              )
            {
                fReCreate = TRUE;
            }

            if(fReCreate)
            {
                SV_RESULT hr = Create(pImgSrc->m_nType, pImgSrc->m_nWidth, pImgSrc->m_nHeight, fFastMem);

                if(SV_FAILED(hr))
                {
                    return hr;
                }
            }

            //拷第一个分量
            SV_UINT8* restrict pSrcLine = pImgSrc->m_pData[0];
            SV_UINT8* restrict pDestLine = m_pData[0];

            for(int i = 0; i < pImgSrc->m_nHeight; i++, pSrcLine += pImgSrc->m_rgStrideWidth[0], pDestLine += m_rgStrideWidth[0])
            {
                memCpy(pDestLine, pSrcLine, pImgSrc->m_nWidth);
            }

            //拷贝UV分量
            if(pImgSrc->m_nType == SV_IMAGE_YUV422)
            {
                for(int i = 1; i <= 2; i++)
                {
                    pSrcLine = pImgSrc->m_pData[i];
                    pDestLine = m_pData[i];

                    for(int j = 0; j < pImgSrc->m_nHeight; j++, pSrcLine += pImgSrc->m_rgStrideWidth[i], pDestLine += m_rgStrideWidth[i])
                    {
                        memCpy(pDestLine, pSrcLine, (pImgSrc->m_nWidth + 1) >> 1);
                    }
                }
            }

            m_nWidth = pImgSrc->m_nWidth;
            m_nHeight = pImgSrc->m_nHeight;

            return RS_S_OK;
        }

        /// 创建图像
        SV_RESULT Create(
            SV_IMAGE_TYPE nType,        ///< 创建图像类型
            SV_INT32 nWidth,            ///< 图像宽度
            SV_INT32 nHeight,           ///< 图像高度
            SV_BOOL fFastMem = FALSE    ///< 是否使用片内内存创建
        )
        {
            Clear();

            int nBufSize = 0;

            switch(nType)
            {
            case SV_IMAGE_YUV422:
                m_rgStrideWidth[0] = (nWidth + 31) / 32 * 32;
                m_rgStrideWidth[1] = (((nWidth + 1) >> 1) + 31) / 32 * 32;
                m_rgStrideWidth[2] = (((nWidth + 1) >> 1) + 31) / 32 * 32;
                nBufSize = (m_rgStrideWidth[0] + m_rgStrideWidth[1] + m_rgStrideWidth[2]) * nHeight;
                break;

            case SV_IMAGE_GRAY:
                m_rgStrideWidth[0] = (nWidth + 31) / 32 * 32;
                m_rgStrideWidth[1] = 0;
                m_rgStrideWidth[2] = 0;
                nBufSize = m_rgStrideWidth[0] * nHeight;
                break;

            case SV_IMAGE_RGB:
            case SV_IMAGE_HSV:
                m_rgStrideWidth[0] = (nWidth * 3 + 3) / 4 * 4;
                m_rgStrideWidth[1] = (nWidth * 3 + 3) / 4 * 4;
                m_rgStrideWidth[2] = (nWidth * 3 + 3) / 4 * 4;
                nBufSize = m_rgStrideWidth[0] * nHeight;
                break;

            default:
                m_rgStrideWidth[0] = 0;
                m_rgStrideWidth[1] = 0;
                m_rgStrideWidth[2] = 0;
                break;
            }

            if(nBufSize == 0)
            {
                return RS_E_INVALIDARG;
            }

            SV_UINT8* pBuf = (SV_UINT8*)m_cAlloc.Alloc(nBufSize, fFastMem);

            if(pBuf == NULL)
            {
                return RS_E_OUTOFMEMORY;
            }

            switch(nType)
            {
            case SV_IMAGE_YUV422:
                m_pData[0] = pBuf;
                m_pData[1] = m_pData[0] + m_rgStrideWidth[0] * nHeight;
                m_pData[2] = m_pData[1] + m_rgStrideWidth[1] * nHeight;
                break;

            case SV_IMAGE_GRAY:
                m_pData[0] = pBuf;
                m_pData[1] = NULL;
                m_pData[2] = NULL;
                break;

            case SV_IMAGE_RGB:
            case SV_IMAGE_HSV:
                m_pData[0] = pBuf;
                m_pData[1] = pBuf + 1;
                m_pData[2] = pBuf + 2;
                break;

            default:
                break;
            }

            m_nType = nType;
            m_nWidth = nWidth;
            m_nHeight = nHeight;

            return RS_S_OK;
        }

    private:
        /// 内存分配器
        CMemAlloc m_cAlloc;
    };

    /// 积分图类
    class CSvItg : public SV_ITG
    {
    public:
        /// 构造函数
        CSvItg()
        {
            return;
        }
        /// 析构函数
        ~CSvItg()
        {
            Clear();
            return;
        }

    public:
        /// 清理函数
        void Clear()
        {
            m_cAlloc.Free();

            m_nType = SV_IMAGE_UNKNOWN;
            m_nWidth = 0;
            m_nHeight = 0;
            m_rgStrideWidth[0] = 0;
            m_pData[0] = NULL;
        }

        /// 拷贝函数
        SV_RESULT Copy(
            const SV_ITG* pItgSrc,      ///< 源积分图
            SV_BOOL fFastMem = FALSE    ///< 是否拷贝到片内
        )
        {
            SV_BOOL fReCreate = FALSE;

            if(m_nWidth < pItgSrc->m_nWidth ||
                    m_nHeight < pItgSrc->m_nHeight ||
                    fFastMem == TRUE)
            {
                fReCreate = TRUE;
            }

            if(fReCreate)
            {
                SV_RESULT hr = Create(pItgSrc->m_nWidth, pItgSrc->m_nHeight, fFastMem);

                if(SV_FAILED(hr))
                {
                    return hr;
                }
            }

            SV_UINT32* pSrcLine = pItgSrc->m_pData[0];
            SV_UINT32* pDestLine = m_pData[0];

            for(int i = 0; i < pItgSrc->m_nHeight; i++, pSrcLine += pItgSrc->m_rgStrideWidth[0], pDestLine += m_rgStrideWidth[0])
            {
                memCpy(pDestLine, pSrcLine, (pItgSrc->m_nWidth + 1) * sizeof(SV_UINT32));
            }

            m_nType = pItgSrc->m_nType;
            m_nWidth = pItgSrc->m_nWidth;
            m_nHeight = pItgSrc->m_nHeight;

            return RS_S_OK;
        }

        /// 创建积分图并分配内存
        /// @note 宽高对应原始图像尺寸,非积分图内存尺寸
        SV_RESULT Create(
            SV_INT32 nWidth,            ///< 图像宽度
            SV_INT32 nHeight,           ///< 图像高度
            SV_BOOL fFastMem = FALSE    ///< 是否使用片内内存创建
        )
        {
            Clear();

            m_rgStrideWidth[0] = (nWidth + 1) * sizeof(SV_UINT32);
            int nBufSize = m_rgStrideWidth[0] * (nHeight + 1);

            if(nBufSize <= 0)
            {
                return RS_E_INVALIDARG;
            }

            SV_UINT32* pBuf = (SV_UINT32*)m_cAlloc.Alloc(nBufSize, fFastMem);

            if(pBuf == NULL)
            {
                return RS_E_OUTOFMEMORY;
            }

            m_pData[0] = pBuf;

            m_nType = SV_IMAGE_ITG;
            m_nWidth = nWidth;
            m_nHeight = nHeight;

            return RS_S_OK;
        }

    private:
        /// 内存分配器
        CMemAlloc m_cAlloc;
    };

    /// 截图
    /// @note pRectCrop是内存坐标,即x:[0,nWidth-1], y:[0,nHeight-1],截图包括边框
    SV_RESULT imCrop(
        const SV_IMAGE* pImgSrc,        ///< 源图像
        const SV_RECT* pRectCrop,       ///< 截图区域
        SV_IMAGE* pImgCrop              ///< 结果图
    );

    /// 拷贝图像
    /// @note 只支持YUV422、GRAY
    SV_RESULT imCopy(
        const SV_IMAGE* pImgSrc,
        SV_IMAGE* pImgDest
    );

    /// RGB转YCbCr
    /// @note RGB存储结构为 B|G|R
    SV_RESULT imRGB2YCbCr(
        const SV_IMAGE* pImgRGB,    ///< RGB图
        SV_IMAGE* pImgYCbCr     ///< YCbCr结果图
    );

    /// YCbCr转RGB
    /// @note RGB存储结构为 B|G|R
    SV_RESULT imYCbCr2RGB(
        const SV_IMAGE* pImgYCbCr,      ///< YCbCr图
        SV_IMAGE* pImgRGB               ///< RGB结果图
    );

    /// Gray转RGB
    SV_RESULT imGray2RGB(
        const SV_IMAGE* pImgGray,   ///< GRAY图
        SV_IMAGE* pImgRGB           ///< RGB图
    );

    /// YCbCr转HSV
    SV_RESULT imYCbCr2HSV(
        const SV_IMAGE* pImgYCbCr,      ///< YCbCr源图
        SV_IMAGE* pImgHSV               ///< HSV图
    );

    /// 二值化类型
    enum BW_TYPE
    {
        SV_THRESH_BINARY        =   0,  ///< value = value > threshold ? max_value : 0
        SV_THRESH_BINARY_INV    =   1,  ///< value = value > threshold ? 0 : max_value
        SV_THRESH_TRUNC         =   2,  ///< value = value > threshold ? threshold : value
        SV_THRESH_TOZERO        =   3,  ///< value = value > threshold ? value : 0
        SV_THRESH_TOZERO_INV    =   4   ///< value = value > threshold ? 0 : value
    };

    /// 二值化函数
    SV_RESULT imTransBW(
        const SV_IMAGE* pImgSrc,    ///< 源图像
        BW_TYPE nType,              ///< 变换类型
        SV_UINT8 nThresh,           ///< 阈值
        SV_UINT8 nMaxVal,           ///< 最大值
        SV_IMAGE* pImgDst           ///< 结果图
    );

    /// 求图像和
    SV_UINT32 imCalcSum(
        const SV_IMAGE* pImgSrc ///< 源图像
    );

    /// 求指定区域均值
    SV_FLOAT imCalcAvg(
        const SV_IMAGE* pImgSrc,            ///< 源图像
        const SV_IMAGE* pImgMask = NULL ///< 模板图
    );

    /// 图像加
    SV_RESULT imAdd(
        const SV_IMAGE* pImgSrc0,       ///< 源图像0
        const SV_IMAGE* pImgSrc1,       ///< 源图像1
        SV_IMAGE* pImgDst               ///< 结果图
    );

    /// 图像绝对值差分
    SV_RESULT imAbsDiff(
        const SV_IMAGE* pImgSrc0,       ///< 源图像0
        const SV_IMAGE* pImgSrc1,       ///< 源图像1
        SV_IMAGE* pImgDst               ///< 结果图
    );

    /// 图像与
    /// @note 只处理第一个分量
    SV_RESULT imAnd(
        const SV_IMAGE* pImgSrc0,       ///< 源图像0
        const SV_IMAGE* pImgSrc1,       ///< 源图像1
        SV_IMAGE* pImgDst               ///< 结果图
    );

    /// 图像或
    /// @note 只处理第一个分量
    SV_RESULT imOr(
        const SV_IMAGE* pImgSrc0,       ///< 源图像0
        const SV_IMAGE* pImgSrc1,       ///< 源图像1
        SV_IMAGE* pImgDst               ///< 结果图
    );

    /// 图像非
    /// @note 只处理第一个分量
    SV_RESULT imNot(
        const SV_IMAGE* pImgSrc,        ///< 源图像
        SV_IMAGE* pImgDst               ///< 结果图
    );

    /// 图像填充
    SV_RESULT imFillGray(
        SV_IMAGE* pImgSrc,              ///< 源图像
        int nValue                      ///< 填充值
    );

    /// 求水平方向Sobel
    SV_RESULT imSobelH(
        const SV_IMAGE* pImgSrc,        ///< 源图像
        SV_IMAGE* pImgDst               ///< 目标图
    );

    /// 求竖直方向Sobel
    SV_RESULT imSobelV(
        const SV_IMAGE* pImgSrc,        ///< 源图像
        SV_IMAGE* pImgDst               ///< 目标图
    );

    /// 求Sobel图
    SV_RESULT imSobel(
        const SV_IMAGE* pImgSrc,        ///< 源图像
        SV_IMAGE* pImgDst               ///< 目标图
    );

    /// 任意比例缩放
    SV_RESULT imResize(
        const SV_IMAGE* pImgSrc,        ///< 源图像
        SV_IMAGE* pImgDst               ///< 目标图
    );

    SV_RESULT imResizeCube(
        const  SV_IMAGE* pImgSrc,
        SV_IMAGE* pImgDst
    );

    /// 缩小2^nRatio倍
    SV_RESULT imDownSamp(
        const SV_IMAGE* pImgSrc,
        SV_IMAGE* pImgDst,
        int nRatio
    );

    /// 高斯滤波
    /// @note 5x5,方差=0.5,只处理Y分量
    SV_RESULT imGaussian(
        SV_IMAGE* pSrc,
        SV_IMAGE* pDest
    );

    /// 32位积分图
    SV_RESULT imCalcItg(
        const SV_IMAGE* pImgSrc,        ///< 源图
        SV_ITG* pItg                    ///< 积分图
    );

    /// 32位平方积分图
    SV_RESULT imCalcSqItg(
        const SV_IMAGE* pImgSrc,        ///< 源图
        SV_ITG* pItg                    ///< 积分图
    );

    /// 用积分图计算区域灰度和
    /// @note 为效率考虑不做有效性判断
    __inline SV_UINT32 imCalcAreaSum(
        const SV_ITG* pItg,         ///< 积分图
        const SV_RECT* pArea        ///< 求和区域
    )
    {
        int x0 = pArea->m_nLeft;
        int y0 = pArea->m_nTop;
        int x1 = pArea->m_nRight + 1;
        int y1 = pArea->m_nBottom + 1;
        int w = pItg->m_nWidth + 1;
        return  pItg->m_pData[0][ y1 * w + x1] -
                pItg->m_pData[0][ y0 * w + x1] -
                pItg->m_pData[0][ y1 * w + x0] +
                pItg->m_pData[0][ y0 * w + x0];
    }

    /// 形态学膨胀操作
    /// @note 只支持矩形膨胀模板
    SV_RESULT imDilate(
        const SV_IMAGE* pImgSrc,  ///< 源图像
        int nElemW,               ///< 膨胀模板宽
        int nElemH,               ///< 膨胀模板高
        SV_IMAGE* pImgDst         ///< 目标图
    );

    /// 形态学腐蚀操作
    /// @note 只支持矩形腐蚀模板
    SV_RESULT imErode(
        const SV_IMAGE* pImgSrc,  ///< 源图像
        int nElemW,               ///< 腐蚀模板宽
        int nElemH,               ///< 腐蚀模板高
        SV_IMAGE* pImgDst         ///< 目标图
    );

    /// 找轮廓矩形框
    /// @note 源图像将被改变
    SV_RESULT imFindContoursRect(
        SV_IMAGE* pImgSrc,        ///< 源图像
        SV_RECT* rgRects,         ///< 矩形数组
        int nRectMax,             ///< 矩形数组最大个数
        int* pnRectCnt            ///< 找到的矩形个数
    );

    /// 2-SIGMA 图像拉伸
    /// @note 直接在源图上变换
    SV_RESULT imHistAdj(
        SV_IMAGE* pImgTrans
    );
} // sv
