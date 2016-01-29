/// @file
/// @brief 矩形类型及操作定义
/// @author liaoy
/// @date 2013/8/26 10:46:40
///
/// 修改说明:
/// [2013/8/26 10:46:40 liaoy] 最初版本

#pragma once

#include "sv_basetype.h"
#include "sv_point.h"

namespace sv
{
    /// 矩形结构体
    struct SV_RECT
    {
        /// 左
        int m_nLeft;
        /// 上
        int m_nTop;
        /// 右
        int m_nRight;
        /// 下
        int m_nBottom;
    };

    /// 矩形类
    class CSvRect : public SV_RECT
    {
    public:
        /// 构造函数
        CSvRect() {};

        /// 构造函数
        CSvRect(
            int nLeft,      ///< 左
            int nTop,       ///< 上
            int nRight,     ///< 右
            int nBottom ///< 下
        )
        {
            m_nLeft = nLeft;
            m_nTop = nTop;
            m_nRight = nRight;
            m_nBottom = nBottom;
        }

        /// 拷贝构造函数
        CSvRect(
            const SV_RECT& rectSrc      ///< 源矩形
        )
        {
            m_nLeft = rectSrc.m_nLeft;
            m_nTop = rectSrc.m_nTop;
            m_nRight = rectSrc.m_nRight;
            m_nBottom = rectSrc.m_nBottom;
        }

    public:
        /// 设置矩形坐标
        void SetRect(
            int nLeft,      ///< 左
            int nTop,       ///< 上
            int nRight,     ///< 右
            int nBottom ///< 下
        )
        {
            m_nLeft = nLeft;
            m_nTop  = nTop;
            m_nRight = nRight;
            m_nBottom = nBottom;
        }

        /// 宽度
        /// @note 宽度为右-左+1
        int GetWidth()
        {
            return m_nRight - m_nLeft + 1;
        }

        /// 高度
        /// @note 高度为下-上+1
        int GetHeight()
        {
            return m_nBottom - m_nTop + 1;
        }

        /// 面积
        int GetArea()
        {
            return (m_nRight - m_nLeft + 1) * (m_nBottom - m_nTop + 1);
        }

        /// 中心点
        SV_POINT CenterPoint()
        {
            SV_POINT cCentPoint;
            cCentPoint.m_nX = (m_nLeft + m_nRight) >> 1;
            cCentPoint.m_nY = (m_nBottom + m_nTop) >> 1;
            return cCentPoint;
        }
    };

    /// 矩形重叠判断函数
    SV_BOOL rectIsOverlap(
        const SV_RECT* pRect0,      ///< 矩形0
        const SV_RECT* pRect1,      ///< 矩形1
        int p                       ///< 重叠百分比阈值
    );

    /// 获取重叠区域
    /// return 是否重叠
    SV_BOOL rectGetOverlap(
        const SV_RECT* pRect0,      ///< 矩形0
        const SV_RECT* pRect1,      ///< 矩形1
        SV_RECT* pRectOverlap       ///< 重叠区域
    );

} // sv
