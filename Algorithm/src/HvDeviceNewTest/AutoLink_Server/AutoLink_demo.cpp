// AutoLink_demo.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "AutoLink_demo.h"
#include "AutoLink_demoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAutoLink_demoApp

BEGIN_MESSAGE_MAP(CAutoLink_demoApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CAutoLink_demoApp 构造

CAutoLink_demoApp::CAutoLink_demoApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CAutoLink_demoApp 对象

CAutoLink_demoApp theApp;


HANDLE  g_hMutex = NULL;
// CAutoLink_demoApp 初始化

BOOL CAutoLink_demoApp::InitInstance()
{
	CWinApp::InitInstance();

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}
	// DEBUG下不创建守护进程
#ifndef _DEBUG
	g_hMutex = CreateMutex(NULL,TRUE, _T("AutoLink_Server.exe"));
	if (g_hMutex == NULL)
	{
		return FALSE;
	}
	if (GetLastError() == ERROR_SUCCESS)
	{
		//当前是守护进程
		
		BOOL fExit = FALSE;
		while (!fExit)
		{
			ReleaseMutex(g_hMutex);

			STARTUPINFO si;
			PROCESS_INFORMATION pi;

			ZeroMemory( &si, sizeof(si) );
			si.cb = sizeof(si);
			ZeroMemory( &pi, sizeof(pi) );

			// Start the child process. 
			TCHAR szFileName[MAX_PATH] = {0};
			GetModuleFileName(NULL, szFileName, MAX_PATH - 1);	//取得包括程序名的全路径
			if( !CreateProcess( NULL,	// No module name (use command line). 
				szFileName,					// Command line. 
				NULL,							// Process handle not inheritable. 
				NULL,							// Thread handle not inheritable. 
				FALSE,							// Set handle inheritance to FALSE. 
				0,									// No creation flags. 
				NULL,							// Use parent's environment block. 
				NULL,							// Use parent's starting directory. 
				&si,								// Pointer to STARTUPINFO structure.
				&pi )								// Pointer to PROCESS_INFORMATION structure.
				) 
			{
				AfxMessageBox(_T("创建子进程失败！"));
				CloseHandle(g_hMutex);
				return FALSE;
			}

			// Wait until child process exits.
			WaitForSingleObject( pi.hProcess, INFINITE );

			// Close process and thread handles. 
			CloseHandle( pi.hProcess );
			CloseHandle( pi.hThread );

			//判断子进程是否正常退出
			if (WaitForSingleObject(g_hMutex, INFINITE) == WAIT_OBJECT_0)
			{
				fExit = TRUE;
			}
		}
		return TRUE;
	}
	if(GetLastError() != ERROR_ALREADY_EXISTS)
	{
		return FALSE;
	}
	//当前是子进程
	if (WaitForSingleObject(g_hMutex, 0) != WAIT_OBJECT_0)
	{
		//防止重复启动
		CloseHandle(g_hMutex);
		return FALSE;
	}
#endif

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));

	CAutoLink_demoDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();

	if (nResponse == IDOK)
	{
		// TODO: 在此放置处理何时用“确定”来关闭
		//对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用“取消”来关闭
		//对话框的代码
	}

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	// 而不是启动应用程序的消息泵。
	return FALSE;
}

int CAutoLink_demoApp::ExitInstance()
{
	// TODO: Add your specialized code here and/or call the base class
	if (g_hMutex)
	{
		ReleaseMutex(g_hMutex);
		CloseHandle(g_hMutex);
	}
	return CWinApp::ExitInstance();
}
