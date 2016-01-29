#ifndef _SWCORE_INCLUDE_H
#define _SWCORE_INCLUDE_H

#ifdef __cplusplus
extern "C" {
#endif		// #ifdef __cplusplus


enum
{
	CACHE_YTPE_ALL
};

/// 信息输出
typedef void (*DEBUG_OUTPUT)(
    char* szMsg
    );

typedef void (*F_CACHE_WBINV)(
		void* blockPtr, unsigned int byteCnt, unsigned short type, unsigned short wait
    );

typedef void (*F_CACHE_INV)(
		void* blockPtr, unsigned int byteCnt, unsigned short type, unsigned short wait
		);

typedef unsigned int (*GET_SYSTEM_TICK_MS)();
/// 打开一个DMA操作，返回DMA操作句柄，返回0表示失败、无效句柄
typedef int (*F_DMA_OPEN)(void);

/// DMA拷贝，返回0表示成功
typedef int (*F_DMA_COPY)(
    int nHandle,   ///< SV_CALLBACK_DMA_OPEN 打开的句柄
    void* pSrc,    ///< 要拷贝的源数据地址
    void* pDst,    ///< 目标地址
    int nLen       ///< 要拷贝的数据长度
);

/// 等待DMA拷贝结束，返回0表示成功。该函数为阻塞函数。
typedef int (*F_DMA_WAIT)(
    int nHandle            ///< SV_CALLBACK_DMA_OPEN 打开的句柄
);

/// DMA关闭函数，释放DMA句柄
typedef int (*F_DMA_CLOSE)(
    int nHandle            ///< SV_CALLBACK_DMA_OPEN 打开的句柄
);

typedef struct
{
	DEBUG_OUTPUT pfDebugOutPut;
	F_CACHE_WBINV pfCacheWbInv;
	F_CACHE_INV pfCacheInv;
	GET_SYSTEM_TICK_MS pfGetSystemTick;
	F_DMA_OPEN pfDMAOpen;
	F_DMA_COPY pfDMACopy;
	F_DMA_WAIT pfDMAWait;
	F_DMA_CLOSE pfDMAClose;
}PLATFORM_FUCTION_LIST;


int InitVideoParam(void* pvParam, int iSize, void* pvRespond, int iRespondSize);
int ModifyVideoParam(void* pvParam, int iSize, void* pvRespond, int iRespondSize);
int ProcessOneFrame(void* pvParam, int iSize, void* pvRespond, int iRespondSize);

void SetFuctionCallBack(PLATFORM_FUCTION_LIST* pcList);

#ifdef __cplusplus
}
#endif		// #ifdef __cplusplus

#endif
