#ifndef _HVUTILS_H_
#define _HVUTILS_H_

//头文件
#include "config.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "swBaseType.h"
#include "stack_alloc.h"
#include "interface.h"
#include "Hv_Math.h"
#include "FastCrc32.h"
#include "HvDspTime.h"

#if RUN_PLATFORM == PLATFORM_WINDOWS //Win32
//header for Win32
	#include <windows.h>
	#include <shlwapi.h>
#elif RUN_PLATFORM == PLATFORM_DSP_BIOS //Dsp
//header for DSP_BIOS

	#include <std.h>
	#include "csl_dat.h"
	#include "csl_cache.h"
	#include <mem.h>
	#include <tsk.h>
    #include "basecommon.h"
   	#include "HvSysErr.h"

	#include "hvtarget.h"
	#include "..\HighHard\edma3.h"

	extern int intHeap;
	extern int extHeap;
#endif

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "swBaseType.h"
#include "swWinError.h"
#include "stack_alloc.h"
#include "interface.h"

#ifdef __cplusplus
	extern "C" {
#endif

//调试信息输出
__inline int HV_DebugInfo(int iDeviceID, char* szfmt, ...)
{
	if ( !IsDebugDeviceEnabled(iDeviceID) ) return 0;

	va_list arglist;
	int iRetVal = 0;
	
	char szBuff[256] = {0};

	va_start( arglist, szfmt );
	iRetVal = vsprintf( szBuff, szfmt, arglist );	
	va_end( arglist );
	OutputString( szBuff, iDeviceID );
	
	return iRetVal;
}

__inline int HV_Trace(char* szfmt, ...)
{
	if ( !IsDebugDeviceEnabled(DEBUG_STR_DISPLAY_FILE) ) return 0;

	va_list arglist;
	int iRetVal = 0;

	char szBuff[640] = {0};

	va_start( arglist, szfmt );
	//iRetVal = vsprintf( szBuff, szfmt, arglist );
#if RUN_PLATFORM == PLATFORM_WINDOWS
	iRetVal = _vsnprintf( szBuff, sizeof(szBuff)-1, szfmt, arglist );	 
#elif RUN_PLATFORM == PLATFORM_DSP_BIOS
	iRetVal = std::vsnprintf( szBuff, sizeof(szBuff)-1, szfmt, arglist );
#endif
	va_end( arglist );
	OutputString( szBuff, DEBUG_STR_DISPLAY_FILE );
	
	return iRetVal;	
}

//内存操作
__inline void* HV_memcpy(void* dest, const void* src, size_t len)
{
	return memcpy(dest, src, len);
}

__inline void* HV_memset(void* p, int val, size_t len)
{
	return memset(p, val, len);
}

__inline int	HV_memcmp(const void * p1, const void * p2, size_t len)

{
	return memcmp(p1, p2, len);
}

#if RUN_PLATFORM == PLATFORM_WINDOWS

//内存分配
struct MEM_STRUCT 
{
	int iTemp;
};
__inline void *HV_AllocFastMem(int iMemSize)
{
	return new MEM_STRUCT[(iMemSize + sizeof(MEM_STRUCT) - 1) / sizeof(MEM_STRUCT)];
}

__inline void HV_FreeFastMem(void *pMem, int iMemSize)
{
	delete[] (MEM_STRUCT*)pMem;
}

__inline void *HV_AllocMem(int iMemSize)
{
	return new MEM_STRUCT[(iMemSize + sizeof(MEM_STRUCT) - 1) / sizeof(MEM_STRUCT)];
}

__inline void HV_FreeMem(void *pMem, int iMemSize)
{
	delete[] (MEM_STRUCT*)pMem;
}

//DMA操作

#define DAT_XFRID_WAITNONE 0

__inline int HV_dmacpy( void *dest, void *src, short len )
{
	memcpy( dest, src, len );
	return 1;
}

__inline void HV_dmawait( DWORD32 dwid )
{
	return;
}

__inline void HV_invalidate( void *ptr, DWORD32 count )
{
	return;
}

__inline void HV_writeback( void *ptr, DWORD32 count )
{
	return;
}

__inline void HV_writebackInv( void *ptr, DWORD32 count )
{
	return;
}

__inline void HV_cachewait()
{
	return;
}

//for DSP
#elif RUN_PLATFORM == PLATFORM_DSP_BIOS

//内存分配
__inline void *HV_AllocFastMem(int iMemSize)
{
	void *pTemp = MEM_alloc(intHeap, iMemSize, 128);
	if (pTemp == MEM_ILLEGAL)
	{
		//HV_Trace("\nHV_AllocFastMem failed!");
		return NULL;
	}
	return pTemp;
}

__inline void HV_FreeFastMem(void *pMem, int iMemSize)
{
	MEM_free(intHeap, pMem, iMemSize);
}

__inline void *HV_AllocMem(int iMemSize)
{
	void *pTemp = MEM_alloc(extHeap, iMemSize, 128);
	if (pTemp == MEM_ILLEGAL)
	{
		HV_Trace("\nHV_AllocMem failed!");
		return NULL;
	}
	return pTemp;
}
	
__inline void HV_FreeMem(void *pMem, int iMemSize)
{
	MEM_free(extHeap, pMem, iMemSize);
}

//DMA操作
__inline int HV_dmacpy( void *dest, void *src, short len )
{
#ifdef CHIP_6455
	return HV_dmacpy2D((PBYTE8)dest, len, (PBYTE8)src, len, len, 1);
#else
	return DAT_copy( src, dest, len );
#endif
}

__inline void HV_dmawait( int dwid )
{
#ifdef CHIP_6455
	HV_dmawait2D(dwid);
#else
	DAT_wait(dwid);
#endif
}

__inline void HV_invalidate( void *ptr, DWORD32 count )
{
	CACHE_invL2( ptr, count, CACHE_NOWAIT );
}

__inline void HV_writeback( void *ptr, DWORD32 count )
{
	CACHE_wbL2( ptr, count, CACHE_NOWAIT );
}

__inline void HV_writebackInv( void *ptr, DWORD32 count )
{
	CACHE_wbInvL2( ptr, count, CACHE_NOWAIT );
}

__inline void HV_cachewait()
{
	CACHE_wait();
}

#endif //PLATFORM_DSP_BIOS

//公用宏
#ifndef RTN_HR_IF_FAILED

#ifdef RELEASE_TO_MARKET

#define RTN_HR_IF_FAILED(func) \
{ \
	HRESULT hr = func; \
	if (FAILED(hr)) \
	{ \
		exit(0); \
	} \
}

#define NOTIFY_IF_FAILED(func)						\
	hr=func;										\
	if (FAILED(hr))									\
	{												\
		HV_DebugInfo(		\
			DEBUG_STR_ERROR_FILE,  \
			"FAILED (hr=0x%08X): ", #func"\n", hr \
		); \
		return;										\
	}												\

#else

#define RTN_HR_IF_FAILED(func) \
{	\
	HRESULT hr=func; \
	if (FAILED(hr)) \
	{ \
		HV_DebugInfo(		\
				DEBUG_STR_ERROR_FILE,  \
				"FAILED %s(hr=0x%08X): ", #func"\n", hr \
				); \
		HV_Sleep(500);\
		exit(0); \
		return hr;	 \
	}	\
}

#define NOTIFY_IF_FAILED(func)						\
	hr=func;										\
	if (FAILED(hr))									\
	{												\
		HV_DebugInfo(		\
			DEBUG_STR_ERROR_FILE,  \
			"FAILED %s(hr=0x%08X): ", #func"\n", hr \
		); \
		DebugBreak();								\
		return;										\
	}												\

#endif
#endif

#ifndef ARRSIZE
#define ARRSIZE(a) sizeof(a)/sizeof(a[0])
#endif

#ifndef INIT_ARRAY
#define INIT_ARRAY(array, first)		\
{										\
	memset(array, 0, sizeof(array));	\
	array[0]=first;						\
}
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(pObj)							\
	if (pObj)										\
	{												\
		delete pObj;								\
		pObj = NULL;								\
	}												
#endif

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p) { if ( p != NULL ) { p->Release(); p = NULL; } }
#endif	

// 大小字节序转换
__inline unsigned short Hv_ntohs(unsigned short usValue)
{
	return (usValue >> 8 & 0x00FF) | (usValue << 8 & 0xFF00);
}


//系统函数调用
#if RUN_PLATFORM == PLATFORM_WINDOWS

__inline void SystemDog() { return; }

__inline void HV_Sleep(DWORD32 dwMilliseconds) { Sleep(dwMilliseconds); }

#elif RUN_PLATFORM == PLATFORM_DSP_BIOS

__inline void SystemDog() { if(!GetResetFlag()) ResWDT();}

__inline void HV_Sleep(DWORD32 dwMilliseconds) { TSK_sleep(dwMilliseconds); }

#endif 

#ifdef __cplusplus
}
#endif

#endif
