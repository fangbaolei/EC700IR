#pragma once

/// 申请常规内存
typedef void* (*SV_CALLBACK_MEM_ALLOC)(
    int nSize  ///< 申请长度
);

/// 申请片内内存
typedef void* (*SV_CALLBACK_FAST_MEM_ALLOC)(
    int nSize  ///< 申请长度
);

/// 释放内存
typedef void (*SV_CALLBACK_MEM_FREE)(
    void* pAddr,                    ///< 要释放的地址
    int nSize                       ///< 内存长度
);

/// 释放片内内存
typedef void (*SV_CALLBACK_FAST_MEM_FREE)(
    void* pAddr,                    ///< 要释放的地址
    int nSize                       ///< 内存长度
);

/// Cache无效
typedef int (*SV_CALLBACK_CACHE_INVAILD)(
    void* pAddr,                    ///< Cache无效的起始地址
    int nSize                       ///< Cache无效的长度
);

/// Cache回写
typedef int (*SV_CALLBACK_CACHE_WRITE_BACK)(
    void* pAddr,                    ///< Cache回写的起始地址
    int nSize                       ///< Cache回写的长度
);

/// Cache回写并无效
typedef int (*SV_CALLBACK_CACHE_WRITE_BACK_INVAILD)(
    void* pAddr,                    ///< Cache回写并无效的起始地址
    int nSize                       ///< Cache回写并无效的长度
);

/// 打开一个DMA操作，返回DMA操作句柄，返回0表示失败、无效句柄
typedef int (*SV_CALLBACK_DMA_OPEN)(void);

/// DMA拷贝，返回0表示成功
typedef int (*SV_CALLBACK_DMA_COPY)(
    int nHandle,   ///< SV_CALLBACK_DMA_OPEN 打开的句柄
    void* pDst,    ///< 目标地址
    void* pSrc,    ///< 要拷贝的源数据地址
    int nLen       ///< 要拷贝的数据长度
);

/// 等待DMA拷贝结束，返回0表示成功。该函数为阻塞函数。
typedef int (*SV_CALLBACK_DMA_WAIT)(
    int nHandle            ///< SV_CALLBACK_DMA_OPEN 打开的句柄
);

/// DMA关闭函数，释放DMA句柄
typedef int (*SV_CALLBACK_DMA_CLOSE)(
    int nHandle            ///< SV_CALLBACK_DMA_OPEN 打开的句柄
);

/// 取系统时间
typedef unsigned int (*SV_CALLBACK_GET_SYS_TIME)();

/// 返回类型
namespace sv
{
    // 内存
    int SetCallBack_MemAlloc(SV_CALLBACK_MEM_ALLOC func);
    int SetCallBack_FastMemAlloc(SV_CALLBACK_FAST_MEM_ALLOC func);
    int SetCallBack_MemFree(SV_CALLBACK_MEM_FREE func);
    int SetCallBack_FastMemFree(SV_CALLBACK_FAST_MEM_FREE func);

    // cache
    int SetCallBack_CacheInvaild(SV_CALLBACK_CACHE_INVAILD func);
    int SetCallBack_CacheWriteBack(SV_CALLBACK_CACHE_WRITE_BACK func);
    int SetCallBack_CacheWriteBackInvaild(SV_CALLBACK_CACHE_WRITE_BACK_INVAILD func);

    // dma
    int SetCallBack_DmaOpen(SV_CALLBACK_DMA_OPEN func);
    int SetCallBack_DmaCopy(SV_CALLBACK_DMA_COPY func);
    int SetCallBack_DmaWait(SV_CALLBACK_DMA_WAIT func);
    int SetCallBack_DmaClose(SV_CALLBACK_DMA_CLOSE func);

    // 取系统时间
    int SetCallBack_GetSysTime(SV_CALLBACK_GET_SYS_TIME func);
}
