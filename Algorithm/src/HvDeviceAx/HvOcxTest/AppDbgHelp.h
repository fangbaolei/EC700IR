#pragma once

#define DEBUG_HELP_FLAG 1

#include <windows.h>
#include <dbghelp.h>
#pragma comment(lib, "dbghelp.lib")

LONG WINAPI MyUnhandledFilter(struct _EXCEPTION_POINTERS *lpExceptionInfo)
{
	LONG ret = EXCEPTION_EXECUTE_HANDLER;

	TCHAR szFileName[64];
	SYSTEMTIME st;
	::GetLocalTime(&st);
	wsprintf(szFileName, TEXT("HvOcxTestDlg-%04d-%02d-%02d-%02d-%02d-%02d-%02d-%02d.dmp"), st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, rand()%100);

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