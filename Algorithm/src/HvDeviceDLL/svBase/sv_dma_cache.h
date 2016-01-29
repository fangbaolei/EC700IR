/// @file
/// @brief DMA与CACHE相关操作
/// @author ganzz
/// @date 10/29/2013 10:12:23
///
/// 修改说明:
/// [10/29/2013 10:12:23 ganzz] 最初版本

#pragma once

#include "sv_basetype.h"
#include "sv_error.h"

namespace sv
{

    // Cache相关
    /// 判断是否使能了CACHE操作
    SV_BOOL cacheIsOprEnable();
    SV_RESULT cacheInvalidate(void *ptr, SV_UINT32 count);
    SV_RESULT cacheWriteBack(void *ptr, SV_UINT32 count);
    SV_RESULT cacheWriteBackInvalidate(void *ptr, SV_UINT32 count);

    // DMA相关
    /// DMA句柄
    typedef int SV_DMA_HANDLE;

    /// 判断是否使能了DMA
    SV_BOOL dmaIsEnable();

    /// 打开DMA，并返回一个句柄，0为无效
    SV_DMA_HANDLE dmaOpen();

    /// DMA关闭
    void dmaClose(SV_DMA_HANDLE hDma);

    /// DMA拷贝
    /// DMA使能后可正常使用DMA，未使能DMA或失败时，dmaCopy操作将由memCpy代替
    SV_RESULT dmaCopy(
        SV_DMA_HANDLE hDma,
        void* pbDst,
        void* pbSrc,
        int iSize
    );

    /// DMA等待，阻塞式
    void dmaWait(SV_DMA_HANDLE hDma);

}
