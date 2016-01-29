#include "config.h"
#include "interface.h"
#include "swbasetype.h"
#include <stdarg.h>

#if RUN_PLATFORM == PLATFORM_WINDOWS
	#include "windows.h"
	#include "stdio.h"
#elif RUN_PLATFORM == PLATFORM_DSP_BIOS

#ifndef CHIP_8127
	#include "log.h"
	#include "hvdspsys.h"
	#include "clk.h"
#endif

#elif RUN_PLATFORM == PLATFORM_LINUX
    #include <time.h>
#endif

// 调试信息
char g_szDebugInfo[256];

// 调试信息输出等级
// 1：研发级别
// 3：工程级别
// 5：用户级别
int g_nTraceRank = 5;

// ARM和DSP之间时钟矫正
DWORD32 g_dwDiffTime = 0;

int HV_DebugInfo(int iDeviceID, char* szfmt, ...)
{
//	if ( !IsDebugDeviceEnabled(iDeviceID) ) return 0;
#ifdef LINUX
	va_list arglist;
	int iRetVal = 0;

	va_start( arglist, szfmt );
	iRetVal = vsprintf( g_szDebugInfo, szfmt, arglist );
	va_end( arglist );
	g_szDebugInfo[255] = 0;

	return iRetVal;
#else
	return 0;
#endif
}

/*int MAX_ALLOC_SIZE = 1024 * 1024;
int g_nMemCount = 0;
char *g_rgMemPtr[512] = {0};

void HV_TestMemory(char *pszFlag)
{
    while(1)
    {
        g_rgMemPtr[g_nMemCount] = (char*)malloc(MAX_ALLOC_SIZE);
        if (g_rgMemPtr[g_nMemCount] == NULL)
        {
            printf("\n%s: Total size = %dM ======\n", pszFlag, g_nMemCount);
            break;
        }
        else
        {
            printf("**********  %02d  ***************\n", g_nMemCount);
            memset(g_rgMemPtr[g_nMemCount], 0, MAX_ALLOC_SIZE);
            g_nMemCount++;
        }
    }

    int i;
    for (i = 0; i < g_nMemCount; i++)
    {
        if (g_rgMemPtr[i] != NULL)
        {
            free(g_rgMemPtr[i]);
            g_rgMemPtr[i] = NULL;
        }
    }

    g_nMemCount = 0;
}*/

DWORD32 GetSystemTick( void )
{
#if RUN_PLATFORM == PLATFORM_WINDOWS
	LARGE_INTEGER tick,freq;
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&tick);
	return (DWORD32)(1000*((double)tick.QuadPart/freq.QuadPart));
#elif RUN_PLATFORM == PLATFORM_DSP_BIOS

#ifndef CHIP_8127
	return CLK_getltime() + g_dwDiffTime;
#else
	return Utils_getCurTimeInMsec() + g_dwDiffTime;
#endif

#elif RUN_PLATFORM == PLATFORM_LINUX
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    DWORD32 dwTime = ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
    static DWORD32 dwLastTime = 0;
    if(dwTime >= dwLastTime)
    {
        dwLastTime = dwTime;
    }
    else
    {
        printf("\n----%d--->%d---\n", dwTime, dwLastTime);
    }
    return dwTime;
#endif
}

void RectifySystemTick(DWORD32 dwArmCurTime)
{
#if RUN_PLATFORM == PLATFORM_DSP_BIOS

	// todo.
#ifndef CHIP_8127
	g_dwDiffTime = dwArmCurTime - CLK_getltime();
#else
	g_dwDiffTime = dwArmCurTime - Utils_getCurTimeInMsec();
#endif

#endif
}

#define DEFINE_DEBUG_DEVICE_ENABLED(a) 0,

int g_rgfDebugDeviceEnabled[ DEBUG_STR_DEVICE_COUNT ] = {
	LOG_CAT_LIST(DEFINE_DEBUG_DEVICE_ENABLED)
};

#define DEFINE_DEBUG_DEVICE_NAME(a) #a,

LPCSTR g_rgkszDebugDeviceName[ DEBUG_STR_DEVICE_COUNT ] =
{
	LOG_CAT_LIST(DEFINE_DEBUG_DEVICE_NAME)
};

#if RUN_PLATFORM == PLATFORM_WINDOWS

BOOL g_fSaveLogFile = FALSE;

#define DEFINE_DEBUG_FILE_NAME(a) "c:\\video_output\\debug\\"#a"\\",

char g_rgkszDebugLogPath[ DEBUG_STR_DEVICE_COUNT ][260] =
{
	LOG_CAT_LIST(DEFINE_DEBUG_FILE_NAME)
};

#define DEFINE_DEBUG_FILE(a) NULL,

FILE *g_fpLogFile[DEBUG_STR_DEVICE_COUNT] = {
	LOG_CAT_LIST(DEFINE_DEBUG_FILE)
};

BOOL OpenLogFile(int iDeviceID,char* lpszFileName,char* lpszMode)
{
	char filepath[260];
	sprintf(filepath,"%s%s",GetDebugLogPath(iDeviceID),lpszFileName);
	g_fpLogFile[iDeviceID]=fopen(filepath,lpszMode);
	return ( g_fpLogFile != NULL ) ;
}

BOOL CloseLogFile(int iDeviceID)
{
	if(g_fpLogFile[iDeviceID])
	{
		if (0 == fclose(g_fpLogFile[iDeviceID]) )
		{
			g_fpLogFile[iDeviceID]=NULL;
			return TRUE;
		}
		else
		{
			OutputDebugStr("Close LogFile failed");
			return FALSE;
		}
	}
	return TRUE;
}

BOOL IsLogFileOpened(int iDeviceID)
{
	return (g_fpLogFile[iDeviceID] != NULL ) ;
}

int DoOutputString( char *pString, int iDeviceID )
{
#ifndef RELEASE_TO_MARKET	//RTM版本不输出调试信息
	if (pString == NULL) return INVALD_POINTER;
	if ( iDeviceID<0 || iDeviceID > DEBUG_STR_DEVICE_COUNT ) return INVALID_DEVICE;

	if ( IsDebugDeviceEnabled(iDeviceID) )
	{
		if (g_fSaveLogFile)
		{
			if (g_fpLogFile[iDeviceID] )
			{
				fprintf(g_fpLogFile[iDeviceID], "%s\n",pString);
				fflush(g_fpLogFile[iDeviceID]);
			}
			else
			{
				return DEVICE_FAIL;
			}
		}
		else
		{
			OutputDebugString( pString );
		}
	}
#endif
	return 0;
}

#elif RUN_PLATFORM == PLATFORM_DSP_BIOS
int DoOutputString( char *pString, int iDeviceID ) {
	if ( IsDebugDeviceEnabled(iDeviceID) ) {
		//UartWriteString( pString, 1 );
	}
	return 0;
}
#elif RUN_PLATFORM == PLATFORM_LINUX
BOOL g_fSaveLogFile = FALSE;

#define DEFINE_DEBUG_FILE_NAME(a) "./debug/"#a"/",

char g_rgkszDebugLogPath[ DEBUG_STR_DEVICE_COUNT ][260] =
{
	LOG_CAT_LIST(DEFINE_DEBUG_FILE_NAME)
};

#define DEFINE_DEBUG_FILE(a) NULL,

FILE *g_fpLogFile[DEBUG_STR_DEVICE_COUNT] = {
	LOG_CAT_LIST(DEFINE_DEBUG_FILE)
};

BOOL OpenLogFile(int iDeviceID,char* lpszFileName,char* lpszMode)
{
	char filepath[260];
	sprintf(filepath,"%s%s",GetDebugLogPath(iDeviceID),lpszFileName);
	g_fpLogFile[iDeviceID]=fopen(filepath,lpszMode);
	return ( g_fpLogFile != NULL ) ;
}

BOOL CloseLogFile(int iDeviceID)
{
	if(g_fpLogFile[iDeviceID])
	{
		if (0 == fclose(g_fpLogFile[iDeviceID]) )
		{
			g_fpLogFile[iDeviceID]=NULL;
			return TRUE;
		}
		else
		{
			printf("Close LogFile failed");
			return FALSE;
		}
	}
	return TRUE;
}

BOOL IsLogFileOpened(int iDeviceID)
{
	return (g_fpLogFile[iDeviceID] != NULL ) ;
}

int DoOutputString( char *pString, int iDeviceID )
{
	if (pString == NULL) return INVALD_POINTER;
	if ( iDeviceID<0 || iDeviceID > DEBUG_STR_DEVICE_COUNT ) return INVALID_DEVICE;

	if ( IsDebugDeviceEnabled(iDeviceID) )
	{
		if (g_fSaveLogFile)
		{
			if (g_fpLogFile[iDeviceID] )
			{
				fprintf(g_fpLogFile[iDeviceID], "%s\n",pString);
				fflush(g_fpLogFile[iDeviceID]);
			}
			else
			{
				return DEVICE_FAIL;
			}
		}
		else
		{
            //UartWriteString( pString, 1 );
		}
	}

	return 0;
}

#endif

#if RUN_PLATFORM == PLATFORM_WINDOWS
void BreakPoint()
{
#ifdef _DEBUG
	DebugBreak();
#endif
}
#elif RUN_PLATFORM == PLATFORM_DSP_BIOS
void BreakPoint()
{
#ifdef _DEBUG
	printf("DebugBreak!\n");
	for (;;);
#else
	//HV_Trace( "BreakPoint.\n" );
	for (;;);
#endif
}
#endif

int rand();
int Random()
{
	return rand();
}

