/// @file
/// @brief 算法库基础类型定义
/// @author liaoy
/// @date 2013/8/26 10:46:40
///
/// 修改说明:
/// [2013/8/26 10:46:40 liaoy] 最初版本

#pragma once

#include "sv_config.h"

/// 布尔值真
#ifndef TRUE
#define TRUE 1
#endif

/// 布尔值假
#ifndef FALSE
#define FALSE 0
#endif

/// 空指针
#ifndef NULL
#define NULL 0
#endif

/// restrict关键字, DSP平台默认有
#if SV_RUN_PLATFORM == SV_PLATFORM_WIN
#define restrict
#elif SV_RUN_PLATFORM == SV_PLATFORM_LINUX
#define restrict __restrict
#endif

namespace sv
{
    /// 8位整形
    typedef signed char          SV_INT8;
    /// 16位整形
    typedef short               SV_INT16;
    /// 32位整形
    typedef int                 SV_INT32;
    /// 64位整形
    typedef long long           SV_INT64;
    /// 8位无符号整形
    typedef unsigned char       SV_UINT8;
    /// 16位无符号整形
    typedef unsigned short      SV_UINT16;
    /// 32位无符号整形
    typedef unsigned int        SV_UINT32;
    /// 64位无符号整形
    typedef unsigned long long  SV_UINT64;
    /// 32位单精度浮点
    typedef float               SV_FLOAT;
    /// 64位双精度浮点
    typedef double              SV_DOUBLE;
    /// 布尔型
    typedef int                 SV_BOOL;
    /// 宽字符型
    typedef unsigned short      SV_WCHAR;

} // sv

