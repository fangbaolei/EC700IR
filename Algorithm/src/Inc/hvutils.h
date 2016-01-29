// 该文件编码格式必需为WINDOWS-936格式

#ifndef HVUTILS_H_INCLUDED
#define HVUTILS_H_INCLUDED

#include "config.h"
#if RUN_PLATFORM == PLATFORM_LINUX
#include <sys/statfs.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#ifndef _NO_DSPBIOS_
#include "stack_alloc.h"
#include "interface.h"
#include "hv_math.h"
#include "fastcrc32.h"
#endif

#include "swobjbase.h"
#include "swbasetype.h"
#include "swwinerror.h"

#if RUN_PLATFORM == PLATFORM_WINDOWS //Win32
//header for Win32
#include "windows.h"
#elif RUN_PLATFORM == PLATFORM_DSP_BIOS //Dsp

#ifndef CHIP_8127
//header for DSP/BIOS
#include <std.h>
#include "csl_dat.h"
#include "csl_cache.h"
#include <mem.h>
#include <tsk.h>
#include "basecommon.h"
#include "HvSysErr.h"
#include "hvtarget.h"

#ifdef CHIP_6467
#include "../CamDsp/CamDspInc/DmaCopyApi.h"
#else
#include "edma3.h"
#endif

extern int intHeap;
extern int extHeap;

#else
#include <xdc/std.h>
#include <xdc/runtime/IHeap.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Memory.h>
#include <xdc/runtime/Error.h>
//#include <ti/syslink/utils/Cache.h>

extern IHeap_Handle DSP_HEAPINT_MEM;
extern IHeap_Handle DSP_HEAPEXT_MEM;

#endif // 8127

#elif RUN_PLATFORM == PLATFORM_LINUX //Linux
//header for Linux
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/sysinfo.h>
#include <sys/signal.h>
#include "assert.h"
#include "HvLinuxDef.h"
#include "HvTime.h"
#include "HvVarType.h"
#include "HVTelnet.h"
#include "SerialBase.h"
#include "HvDebugStateInfo.h"
#include "hvtarget_ARM.h"
#include "hvthread.h"
#include "HvExit.h"
#include "version.h"
#include <list>
#include <deque>
#endif

/* interface.c */
extern char g_szDebugInfo[256];
extern int g_nTraceRank;

#ifdef __cplusplus
extern "C"
{
#endif

    /* interface.c */
    int HV_DebugInfo(int iDeviceID, char* szfmt, ...);
//void HV_TestMemory(char *pszFlag);

#ifdef _CAM_APP_
    /* main.c */
    extern int Com_Lock(int iTimeOutMS);
    extern int Com_UnLock();
#endif

#ifdef LINUX
    /* misc.cpp */
    extern void OutputDebugInfoWithTime(char* szDebugInfo);

    __inline int HV_TraceInThread(int nRank, const char* szfmt, ...)
    {
#ifdef LINUX
        static char szBuff[4096] = {0};
        static time_t tNowTime = 0;
        static struct tm cNowDateTime;

#if !(defined(_CAM_APP_) && !defined(_HVCAM_PLATFORM_RTM_))
        // HV_Trace锁
        static pthread_mutex_t cHvTraceMutex = PTHREAD_MUTEX_INITIALIZER;
#endif

// 非RTM版的主端使用串口锁，其它使用HV_Trace锁
#if defined(_CAM_APP_) && !defined(_HVCAM_PLATFORM_RTM_)
        // 因为共用主串口，即/dev/ttyS0，故在此用串口锁加锁互斥
        if (S_OK != Com_Lock(0))
        {
            return -1;
        }
#else
        pthread_mutex_lock(&cHvTraceMutex);
#endif

        int iRetVal = 0;
        char* pBufPos = szBuff;

        memset((void*)&cNowDateTime, 0, sizeof(cNowDateTime));
        tNowTime = time(NULL);
        if ( NULL != gmtime_r((const time_t*)&tNowTime, &cNowDateTime) )
        {
            iRetVal = sprintf(pBufPos, "\n[%d/%02d/%02d %02d:%02d:%02d] ",
                              cNowDateTime.tm_year + 1900,
                              cNowDateTime.tm_mon + 1,
                              cNowDateTime.tm_mday,
                              cNowDateTime.tm_hour,
                              cNowDateTime.tm_min,
                              cNowDateTime.tm_sec);
            if (iRetVal > 0)
            {
                pBufPos += iRetVal;

                va_list arglist;
                va_start(arglist, szfmt);
                iRetVal = vsprintf(pBufPos, szfmt, arglist);
                va_end(arglist);

                szBuff[4095] = '\0';
#if 0

                TelnetOutputDataString(nRank, szBuff);

                if (nRank >= g_nTraceRank)
                {
                    static DWORD32 dwLines = 0;
                    //修改在log目录下增加日志文件，方便出问题时查看
                    FILE *fp = NULL;
                    if(dwLines < 1000)
                    {
                        fp = fopen("/log/App.txt", "a+");
                    }
                    else
                    {
                        fp = fopen("/log/App.txt", "w");
                        dwLines = 0;
                    }
                    if(fp)
                    {
                        dwLines++;
                        fputs(szBuff, fp);
                        fclose(fp);
                    }
                }
#endif
#ifndef _HVCAM_PLATFORM_RTM_
#if 0
                // 非RTM版才向串口打印调试信息
                if (nRank >= g_nTraceRank)
                {
                    printf(szBuff);
                    fflush(stdout);
                }
#endif
#endif
            }
        }

#if defined(_CAM_APP_) && !defined(_HVCAM_PLATFORM_RTM_)
        Com_UnLock();
#else
        pthread_mutex_unlock(&cHvTraceMutex);
#endif

        return iRetVal;
#else
        return 0;
#endif // LINUX
    }

    static pthread_mutex_t g_traceMutex = PTHREAD_MUTEX_INITIALIZER;
    typedef struct tagLOG
    {
        int nRank;
        string strLog;
    }LOG;
    static std::deque<LOG>g_lstLog;
    static bool fInitialize = false;
    static void *OnTrace(void *pContext)
    {
        LOG log;
        while(1)
        {
            pthread_mutex_lock(&g_traceMutex);
            log.nRank = 0;
            if(!g_lstLog.empty())
            {
                log = g_lstLog.front();
                g_lstLog.pop_front();
            }
            pthread_mutex_unlock(&g_traceMutex);
            if(!log.nRank)
            {
                usleep(100000);
                continue;
            }
            HV_TraceInThread(log.nRank, log.strLog.c_str());
        }
        return 0;
    }
#endif
    __inline int HV_Trace(int nRank, const char* szfmt, ...)
    {
#ifdef LINUX
#ifdef _CAM_APP_
        if(!fInitialize)
        {
            fInitialize = true;
            pthread_t tid;
            pthread_create(&tid, NULL, OnTrace, NULL);
        }
        static char szBuff[4096] = {0};

        pthread_mutex_lock(&g_traceMutex);
        va_list arglist;
        va_start(arglist, szfmt);
        vsprintf(szBuff, szfmt, arglist);
        va_end(arglist);
        LOG log;
        log.nRank = nRank;
        log.strLog = szBuff;
        g_lstLog.push_back(log);
        pthread_mutex_unlock(&g_traceMutex);
#else
        static char szBuff[4096] = {0};
        va_list arglist;
        va_start(arglist, szfmt);
        vsprintf(szBuff, szfmt, arglist);
        va_end(arglist);
        HV_TraceInThread(nRank, szBuff);
#endif
#endif
        return 0;
    }

#define START_CALC_TIME(a) DWORD32 start_time_##a = GetSystemTick();
#define END_CALC_TIME(a, escape, extMsg)   {DWORD32 ms = GetSystemTick() - start_time_##a; if(ms > escape) HV_Trace(5, "%s relay:%d", extMsg, ms);}

#ifdef LINUX
#define Trace(format, ...) HV_Trace(5, format, ## __VA_ARGS__)
#else
#define Trace printf
#endif

    /*
    __inline int HV_Trace(int nRank, char* szfmt, ...)
    {
    #ifdef LINUX
        static char szBuff[1024] = {0};
        int iRetVal = 0;

        LockTelnetOutputDataString();

        va_list arglist;
        va_start(arglist, szfmt);
        iRetVal = vsprintf(szBuff, szfmt, arglist);
        va_end(arglist);

        TelnetOutputDataString(nRank, szBuff);
        if (nRank >= g_nTraceRank)
        {
            OutputDebugInfoWithTime(szBuff);
        }

        UnlockTelnetOutputDataString();

        return iRetVal;
    #else
        return 0;
    #endif
    }
    */

    __inline void* HV_memcpy(void* dest, const void* src, size_t len)
    {
        return memcpy(dest, src, len);
    }

    __inline void* HV_memset(void* p, int val, size_t len)
    {
        return memset(p, val, len);
    }

    __inline int HV_memcmp(const void* p1, const void* p2, size_t len)
    {
        return memcmp(p1, p2, len);
    }

#ifdef LINUX
    __inline int HV_SystemWithTimeOut(const char * szCommand, int iTimeOut)
    {
        static pthread_mutex_t* pMutex = NULL;
        static int num = 0;
        char szCmd[255];
        if (!pMutex)
        {
            pMutex = new pthread_mutex_t;
            pthread_mutex_init(pMutex, NULL);
        }
        pthread_mutex_lock(pMutex);
        strcpy(szCmd, szCommand);
        char* szFileName = strstr(szCmd, "|");
        if (!szFileName)
        {
            sprintf(szCmd, "%s|%d.txt", szCommand, ++num);
            szFileName = strstr(szCmd, "|");
        }
        if (szFileName)
        {
            ++szFileName;
            HV_Trace(5, "HV_System: [%s] need return, return file=%s\n", szCmd, szFileName);
            if (!access(szFileName,F_OK))
            {
                HV_Trace(5, "delete file %s\n", szFileName);
                unlink(szFileName);
            }
        }
        else
        {
            HV_Trace(5, "HV_System: [%s]\n", szCmd);
        }
        int nTimes = 30;
        while ((!access("cmd.txt", F_OK) || !access("exec.txt", F_OK)) && nTimes-- > 0)
        {
            //HV_Trace(5, "wait capture cmd.txt...");
            usleep(100000);
        }
        if (nTimes <= 0)
        {
            pthread_mutex_unlock(pMutex);
            return -1;
        }
        FILE* fp = NULL;
        nTimes = 10;
        while (!(fp = fopen("cmd.txt", "w")) && nTimes-- > 0)
        {
            HV_Trace(5, "open file cmd.txt error, %s, retry again...\n", strerror(errno));
            usleep(100000);
        }
        if (fp)
        {
            fprintf(fp, "%s\n", szCmd);
            fclose(fp);

            fp = fopen("exec.txt", "w");
            if (fp)
            {
                fclose(fp);
            }
            else
            {
                HV_Trace(5, "HV_System: [%s] exec error, %s.\n", szCommand, strerror(errno));
                pthread_mutex_unlock(pMutex);
                return -1;
            }
        }
        else
        {
            HV_Trace(5, "HV_System: [%s] open error, %s\n", szCommand, strerror(errno));
            pthread_mutex_unlock(pMutex);
            return -1;
        }
        pthread_mutex_unlock(pMutex);

        int iReturnValue = -1;
        if (szFileName)
        {
            iTimeOut /= 100;
            char szTemp[32];
            while (iTimeOut > 0)
            {
                WdtHandshake();
                fp = fopen(szFileName, "r");
                if (!fp)
                {
                    usleep(100000);
                    --iTimeOut;
                    continue;
                }
                else
                {
                    fgets(szTemp, sizeof(szTemp), fp);
                    iReturnValue = atoi(szTemp);
                    fclose(fp);
                    unlink(szFileName);
                    break;
                }
            }
        }
        return iReturnValue;
    }

    __inline int HV_System(const char* szCommand)
    {
        return HV_SystemWithTimeOut(szCommand, 10000);
    }
#endif // LINUX

#if RUN_PLATFORM == PLATFORM_WINDOWS

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

//for Linux
#elif RUN_PLATFORM == PLATFORM_LINUX

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

#ifndef CHIP_8127
    __inline void *HV_AllocFastMem(int iMemSize)
    {
        void *pTemp = MEM_alloc(intHeap, iMemSize, 128);
        if (pTemp == MEM_ILLEGAL)
        {
            HV_Trace(1, "\nHV_AllocFastMem failed!");
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
            return NULL;
        }
        return pTemp;
    }

    __inline void HV_FreeMem(void *pMem, int iMemSize)
    {
        MEM_free(extHeap, pMem, iMemSize);
    }

    __inline int HV_dmacpy( void *dest, void *src, short len )
    {
        memcpy(dest, src, len);
        return 1;
    }

    __inline void HV_dmawait( int dwid )
    {
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
#else

__inline void *HV_AllocFastMem(int iMemSize)
{
	Error_Block err;
	void *pTemp = Memory_alloc(DSP_HEAPINT_MEM, iMemSize, 128, &err);
    return pTemp;
}

__inline void HV_FreeFastMem(void *pMem, int iMemSize)
{
	Memory_free(DSP_HEAPINT_MEM, pMem, iMemSize);
}

__inline void *HV_AllocMem(int iMemSize)
{
	Error_Block err;
	void *pTemp = Memory_alloc(DSP_HEAPEXT_MEM, iMemSize, 128, &err);
	return pTemp;
}

__inline void HV_FreeMem(void *pMem, int iMemSize)
{
	Memory_free(DSP_HEAPEXT_MEM, pMem, iMemSize);
}

__inline int HV_dmacpy( void *dest, void *src, short len )
{
	memcpy(dest, src, len);
	return 1;
}

__inline void HV_dmawait( int dwid )
{
	return;
}

__inline void HV_invalidate( void *ptr, DWORD32 count )
{
    //return Cache_inv(ptr, count, Cache_Type_ALL, 1);
	return;
}

__inline void HV_writeback( void *ptr, DWORD32 count )
{
    //return Cache_wb(ptr, count, Cache_Type_ALL, 1);
	return;
}

__inline void HV_writebackInv( void *ptr, DWORD32 count )
{
	//return Cache_wbInv(ptr, count, Cache_Type_ALL, 1);
	return;
}

__inline void HV_cachewait()
{
    //return Cache_wait();
	return;
}

#endif // CHIP_8127

#endif //PLATFORM_DSP_BIOS

#ifndef RTN_HR_IF_FAILED

#ifdef RELEASE_TO_MARKET

#define RTN_HR_IF_FAILED(func)                      \
{                                                   \
	HRESULT hr = func;                              \
	if (FAILED(hr))                                 \
	{                                               \
		exit(0);                                    \
	}                                               \
}

#define NOTIFY_IF_FAILED(func)						\
	hr=func;										\
	if (FAILED(hr))									\
	{												\
		HV_DebugInfo(		                        \
			DEBUG_STR_ERROR_FILE,                   \
			"FAILED (hr=0x%08X): ", #func"\n", hr   \
		);                                          \
		return;										\
	}												\

#else

#define RTN_HR_IF_FAILED(func)                          \
{	                                                    \
	HRESULT hr=func;                                    \
	if (FAILED(hr))                                     \
	{                                                   \
		HV_DebugInfo(		                            \
				DEBUG_STR_ERROR_FILE,                   \
				"FAILED %s(hr=0x%08X): ", #func"\n", hr \
				);                                      \
		return hr;	                                    \
	}	                                                \
}

#define NOTIFY_IF_FAILED(func)						\
	hr=func;										\
	if (FAILED(hr))									\
	{												\
		HV_DebugInfo(		                        \
			DEBUG_STR_ERROR_FILE,                   \
			"FAILED %s(hr=0x%08X): ", #func"\n", hr \
		);                                          \
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

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(rg)						\
	if (rg)										    \
	{												\
		delete[] rg;								\
		rg = NULL;								    \
	}
#endif

#ifndef SAFE_FREE
#define SAFE_FREE(p)						    	\
	if (p)									    	\
	{										    	\
		free(p);							    	\
		p = NULL;							    	\
	}
#endif

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p) { if ( p != NULL ) { p->Release(); p = NULL; } }
#endif

    __inline unsigned short Hv_ntohs(unsigned short usValue)
    {
        return (usValue >> 8 & 0x00FF) | (usValue << 8 & 0xFF00);
    }

#if RUN_PLATFORM == PLATFORM_WINDOWS

    __inline void SystemDog()
    {
        return;
    }

    __inline void HV_Sleep(DWORD32 dwMilliseconds)
    {
        Sleep(dwMilliseconds);
    }

#elif RUN_PLATFORM == PLATFORM_DSP_BIOS

    __inline void SystemDog()
    {
        /* if(!GetResetFlag()) ResWDT(); */
    }

#ifndef CHIP_8127
__inline void HV_Sleep(DWORD32 dwMilliseconds)
{
    TSK_sleep(dwMilliseconds);
}
#else
__inline void HV_Sleep(DWORD32 dwMilliseconds)
{
	// todo.
    //TSK_sleep(dwMilliseconds);
}
#endif // CHIP_8127

#elif RUN_PLATFORM == PLATFORM_LINUX

    __inline void SystemDog()
    {
        return;
    }

// TODO use select redefine
    __inline void HV_Sleep(DWORD32 dwMilliseconds)
    {
        usleep(1000 * dwMilliseconds);
    }

    __inline void WORD16_BYTE8(
        WORD16 wValue,
        PBYTE8 pbData
    )
    {
        BYTE8 *pbTemp = pbData;

        *( pbTemp ++ ) = wValue & 0xFF;
        *pbTemp = ( wValue >> 8 ) & 0xFF;
    }

    __inline DWORD32 BYTE8_DWORD32( BYTE8 *pbData )
    {
        return ( ( pbData[3] << 24 ) | ( pbData[2] << 16 ) | ( pbData[1] << 8 ) | pbData[0] );
    }

    __inline WORD16 BYTE8_WORD16( BYTE8 *pbData )
    {
        return ( ( pbData[1] << 8 ) | pbData[0] );
    }

    __inline void DWORD32_BYTE8(
        DWORD32 dwValue,
        PBYTE8 pbData
    )
    {
        BYTE8 *pbTemp = pbData;

        *( pbTemp ++ ) = dwValue & 0xFF;
        *( pbTemp ++ ) = ( dwValue >> 8 ) & 0xFF;
        *( pbTemp ++ ) = ( dwValue >> 16 ) & 0xFF;
        *pbTemp = ( dwValue >> 24 ) & 0xFF;
    }
#endif

#ifdef __cplusplus
}
#endif

#if RUN_PLATFORM == PLATFORM_LINUX
#include "HvDebug.h"
#endif

#endif // HVUTILS_H_INCLUDED
