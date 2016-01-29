/// @file
/// @brief 平台标志定义
/// @author liaoy
/// @date 2013/8/26 10:46:40
///
/// 修改说明:
/// [2013/8/26 10:46:40 liaoy] 最初版本

#pragma once

/// WINDOWS平台
#define SV_PLATFORM_WIN         1

/// DSP平台
#define SV_PLATFORM_DSP         2

/// DSP平台
#define SV_PLATFORM_LINUX       3

#ifdef WIN32
/// 当前平台
#define SV_RUN_PLATFORM         SV_PLATFORM_WIN

/// 禁止DSP平台的#pragma警告
#pragma warning(disable : 4068)
#elif defined LINUX
#define SV_RUN_PLATFORM         SV_PLATFORM_LINUX
#else
/// 当前平台
#define SV_RUN_PLATFORM         SV_PLATFORM_DSP
#endif
