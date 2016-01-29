////---------------------------------------------------------------------------
//#include <stdio.h>
//#include <time.h>
//#include "Console.h"
//#include "Shlwapi.h"
////---------------------------------------------------------------------------
//int  CConsole::m_nInitNum = 0;
//bool CConsole::m_bAttach = false;
//bool CConsole::m_bLogFile = false;
//char CConsole::m_szLogFile[256] = "";
//CRITICAL_SECTION CConsole::m_csLock;
//bool CConsole::m_bInitLock = false;
//CConsole::CConsole()
//{
//}
//
//CConsole::~CConsole()
//{
//	Close();
//}
//
//bool CConsole::Open(void)
//{
//	if(!m_bInitLock)
//	{
//		m_bInitLock = true;
//		InitializeCriticalSection(&m_csLock);
//	}
//
//	if(!m_nInitNum++)
//	{
//		if(!AttachConsole(-1))
//		{
//			m_bAttach = false;
//			AllocConsole();
//		}
//		else
//			m_bAttach = true;
//		freopen( "CONOUT$", "w+t",stdout);
//		freopen( "CONIN$", "r+t",stdin);
//		char szTitle[MAX_PATH];
//		sprintf(szTitle, "%d", GetCurrentProcessId());
//		SetConsoleTitle(szTitle);
//		HWND hwndConsole = NULL;
//		while(!hwndConsole)
//		{
//			hwndConsole = FindWindow("ConsoleWindowClass", szTitle);
//			if(!hwndConsole)
//				Sleep(10);
//		}
//		HMENU hMenu = GetSystemMenu(hwndConsole, FALSE );
//		DeleteMenu(hMenu, SC_CLOSE, MF_BYCOMMAND);
//		DrawMenuBar(hwndConsole);
//		BringWindowToTop(hwndConsole);
//		SetConsoleTitle("控制台日志窗口");
//		return true;
//	}
//	return false;
//}
//
//void CConsole::Close(void)
//{
//	if(!--m_nInitNum)
//	{		
//		FreeConsole();
//	}
//}
//
//bool CConsole::SetLogFile(const char * fname)
//{
//	if(!m_bInitLock)
//	{
//		m_bInitLock = true;
//		InitializeCriticalSection(&m_csLock);
//
//		if(fname && strlen(fname))
//		{
//			m_bLogFile = true;
//			if(!strstr(fname, "\\") && !strstr(fname, "/"))
//			{
//				GetModuleFileName(NULL, m_szLogFile, MAX_PATH);
//				PathRemoveFileSpec(m_szLogFile);
//				strcat(m_szLogFile, "\\");
//			}
//			else
//			{
//				strcpy(m_szLogFile, "");
//			}
//			strcat(m_szLogFile, fname);
//			return true;
//		}
//	}
//	else
//	{
//		return true;
//	}
//}
//
//bool CConsole::Write(const char * fmt, ...)
//{
//	if(IsOpen() || m_bLogFile)
//	{
//		va_list argptr;
//		va_start(argptr, fmt);
//		WriteV(fmt, argptr);
//		va_end(argptr);
//		return true;
//	}
//	return false;
//}
//
//bool CConsole::WriteV(const char *fmt, va_list & argptr)
//{
//	if(IsOpen() || m_bLogFile)
//	{
//		EnterCriticalSection(&m_csLock);
//		time_t t = time(NULL);
//		char szTime[256];
//		strftime(szTime, 256, "[%Y-%m-%d %H:%M:%S]", localtime(&t));
//		if(m_bLogFile)
//		{
//			FILE * fp = fopen(m_szLogFile, "a+");
//			if(fp)
//			{
//				fprintf(fp, "%s\n", szTime);
//				vfprintf(fp, fmt, argptr);
//				fprintf(fp, "\n");
//				fclose(fp);
//			}
//		}
//		if(IsOpen())
//		{
//			printf("%s\n", szTime);
//			vprintf(fmt, argptr);
//			printf("\n");
//		}
//		LeaveCriticalSection(&m_csLock);
//		return true;
//	}
//	return false;
//}
