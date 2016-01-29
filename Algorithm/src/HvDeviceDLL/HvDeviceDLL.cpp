// 主程

#include "HvDeviceDLL.h"
#if defined(_WIN32) && defined(_DEBUG)
#include <windows.h>
#include <dbghelp.h>
#pragma comment(lib, "dbghelp.lib")
#endif

/* HvDevice.cpp */
extern bool g_fHvDeviceInitialized;
extern CRITICAL_SECTION g_csGlobalSync;
extern CRITICAL_SECTION g_csWriteLog;
extern CRITICAL_SECTION g_csFilterSync;		// 全局同步

#include "HvDeviceUtils.h"

extern HRESULT  SetResultTransact( char* pszFilePath = NULL, WORD  fValid = 1 );
bool OnDllProcessAttach()
{
	bool fRet = InitWSA();
	InitializeCriticalSection(&g_csGlobalSync);
	InitializeCriticalSection(&g_csWriteLog);
	InitializeCriticalSection(&g_csFilterSync);
	GdiInitialized();
	LoadIniConfig();
	SetResultTransact( NULL, 1 );
	return fRet;
}

bool OnDllProcessDetach()
{
	bool fRet = UnInitWSA();
	DeleteCriticalSection(&g_csGlobalSync);
	DeleteCriticalSection(&g_csWriteLog);
	DeleteCriticalSection(&g_csFilterSync);
	return fRet;
}

#if defined(_WIN32) && defined(_DEBUG)
LONG WINAPI MyUnhandledFilter(struct _EXCEPTION_POINTERS *lpExceptionInfo)
{
	LONG ret = EXCEPTION_EXECUTE_HANDLER;

	TCHAR szFileName[64];
	SYSTEMTIME st;
	::GetLocalTime(&st);
	wsprintf(szFileName, TEXT("%04d-%02d-%02d-%02d-%02d-%02d-%02d-%02d.dmp"), st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, rand()%100);

	HANDLE hFile = ::CreateFile(szFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
	if (hFile != INVALID_HANDLE_VALUE)
	{
		MINIDUMP_EXCEPTION_INFORMATION ExInfo;

		ExInfo.ThreadId = ::GetCurrentThreadId();
		ExInfo.ExceptionPointers = lpExceptionInfo;
		ExInfo.ClientPointers = false;

		// write the dump

		BOOL bOK = MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, &ExInfo, NULL, NULL );

		if (bOK)
		{
			printf("Create Dump File Success!\n");
		}
		else
		{
			printf("MiniDumpWriteDump Failed: %d\n", GetLastError());
		}

		::CloseHandle(hFile);
	}
	else
	{
		printf("Create File %s Failed %d\n", szFileName, GetLastError());
	}
	return ret;
}

#endif

BOOL APIENTRY DllMain(
					  HANDLE hModule,
					  DWORD ul_reason_for_call,
					  LPVOID lpReserved
					  )
{
	#if defined(_WIN32) && defined(_DEBUG)
	SetUnhandledExceptionFilter(MyUnhandledFilter);
	#endif

	switch ( ul_reason_for_call )
	{
	case DLL_PROCESS_ATTACH:
		{
			OnDllProcessAttach();
		}
		break;

	case DLL_PROCESS_DETACH:
		{
			OnDllProcessDetach();
		}
		break;
	}

	return TRUE;
}
