/// @file
/// @brief svBase声明
/// @author liaoy
/// @date 2013/8/26 10:46:40
/// @note 只可用于内部实现,不可对外发布
///
/// 修改说明:
/// [2013/8/26 10:46:40 liaoy] 最初版本

#pragma once

#include "sv_basetype.h"
#include "sv_error.h"
#include "sv_memory.h"
#include "sv_math.h"
#include "sv_image.h"
#include "sv_point.h"
#include "sv_rect.h"
#include "sv_utils.h"
#include "sv_callback.h"
#include "sv_dma_cache.h"

/// svBase命名空间
namespace sv
{
    /// 获取取版本信息
    const char* GetRevInfo();

    // 设置文本输出回调
    void utSetTraceCallBack_TXT(
        TRACE_CALLBACK_TXT hCallBack
    );

} // sv
