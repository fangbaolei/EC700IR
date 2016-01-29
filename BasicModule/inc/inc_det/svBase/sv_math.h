/// @file
/// @brief 数学操作定义
/// @author liaoy
/// @date 2013/8/26 10:46:40
///
/// 修改说明:
/// [2013/8/26 10:46:40 liaoy] 最初版本

#pragma once

#include "sv_basetype.h"

namespace sv
{
    /// 最小值
#define SV_MIN(x,y)     ((x)>(y)?(y):(x))

    /// 最大值
#define SV_MAX(x,y)     ((x)>(y)?(x):(y))

    /// 上下界限制
#define SV_LIMT(x,l,h)  ((x)<(l)?(l):((x)>(h)?(h):(x)))

    /// 绝对值
#define SV_ABS(x)       ((x)>0?(x):-(x))

    /// 求平方根
    SV_UINT16 mathSqrt(
        SV_UINT32 nRoot     ///< nRoot 待开方数, 0<=nRoot<=65535*65535=4294836225(0xFFFE0001)
    );

    /// 求16位量化的exp(x), x为正数
    /// @return 指数值(16位量化)
    /// @note 精度误差 < 2
    SV_UINT32 mathExpPos(
        SV_UINT32 nNum      ///< 16位量化数, 0<=x<11 => 0<= nNum=(SV_UINT32)(x*65536)<=726817
    );

    /// 求16位量化的exp(x), x为负数
    /// @return 指数值(16位量化)
    /// @note 精度误差 < 2
    SV_UINT32 mathExpNeg(
        SV_UINT32 nNum       ///< 16位量化的x绝对值, -16=<x<0 => dwNum=(SV_UINT32)(-x * 65536) <=1048576
    );

} // sv
