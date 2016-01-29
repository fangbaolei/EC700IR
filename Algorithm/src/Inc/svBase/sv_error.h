/// @file
/// @brief 返回值定义
/// @author liaoy
/// @date 2013/8/26 10:46:40
///
/// 修改说明:
/// [2013/8/26 10:46:40 liaoy] 最初版本

#pragma once

/// 成功判断宏
#define SV_SUCCEEDED(r) ((r)>=0)

/// 失败判断宏
#define SV_FAILED(r) ((r)<0)

namespace sv
{
    /// 返回值定义
    enum SV_RESULT
    {
        RS_S_OK = (int)0x00000000,              ///< 成功且正确
        RS_S_FALSE = (int)0x00000001,           ///< 成功但不正确
        RS_E_FAIL = (int)0x80004005,            ///< 失败
        RS_E_NOTIMPL = (int)0x80004001,         ///< 失败: 未实现
        RS_E_OUTOFMEMORY = (int)0x8007000E,     ///< 失败: 内存不足
        RS_E_INVALIDARG = (int)0x80070057,      ///< 失败: 参数错误
        RS_E_OBJNOINIT = (int)0x8000000A,       ///< 失败: 未初始化
        RS_E_UNEXPECTED = (int)0x8000FFFF       ///< 失败: 严重异常
    };

} // sv

