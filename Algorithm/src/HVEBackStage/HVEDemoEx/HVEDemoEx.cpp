// HVEDemoEx.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "HVEDemoEx.h"
#include "HVEDemoExDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CHVEDemoExApp

BEGIN_MESSAGE_MAP(CHVEDemoExApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CHVEDemoExApp 构造

CHVEDemoExApp::CHVEDemoExApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CHVEDemoExApp 对象

CHVEDemoExApp theApp;


// CHVEDemoExApp 初始化

BOOL CHVEDemoExApp::InitInstance()
{
	InitCommonControls();

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));


	CHVEDemoExDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	//if (nResponse == IDOK)
	//{
		// TODO: 在此放置处理何时用“确定”来关闭
		//对话框的代码
	//}
	//else if (nResponse == IDCANCEL)
	//{
		// TODO: 在此放置处理何时用“取消”来关闭
		//对话框的代码
	//}

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	// 而不是启动应用程序的消息泵。
	return FALSE;
}
