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

// 二进制信息输出接口
typedef void (*F_OUTPUT_DEBUG_INFO)(
		unsigned char* pbData, unsigned int cbDataSize
		);

typedef struct
{
	DEBUG_OUTPUT pfDebugOutPut;
	F_CACHE_WBINV pfCacheWbInv;
	F_CACHE_INV pfCacheInv;
	GET_SYSTEM_TICK_MS pfGetSystemTick;
	F_OUTPUT_DEBUG_INFO pfOutPutDebugInfo;
}PLATFORM_FUCTION_LIST;



int InitVideoParam(void* pvParam, int iSize, void* pvRespond, int iRespondSize);
int ModifyVideoParam(void* pvParam, int iSize, void* pvRespond, int iRespondSize);
int ProcessOneFrame(void* pvParam, int iSize, void* pvRespond, int iRespondSize);

void SetFuctionCallBack(PLATFORM_FUCTION_LIST* pcList);

#ifdef __cplusplus
}
#endif		// #ifdef __cplusplus

#endif
