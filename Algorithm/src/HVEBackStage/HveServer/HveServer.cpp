#include "stdafx.h"
#include "winsvc.h"
#include "HveManager.h"
#include "HveServer.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CWinApp theApp;

#define WM_SHOWTASK		WM_USER+130

#define OUT_PUT_LENGHT 

using namespace std;

SERVICE_STATUS_HANDLE		ssh;
SC_HANDLE					scm, svc;
SERVICE_STATUS				ss;
SERVICE_FAILURE_ACTIONS		sfa;
CString m_strServiceName = "HEV后台系统入库服务";
CString m_strServiceDisplayName = "HVE结果保存";
CString	m_strServiceDescription = "接收识别器的车牌识别结果，将识别结果保存入库";
CString m_strLogPath = "";
CString m_strRunningInfo = "";

HWND g_hWnd = NULL;
BOOL	g_fIsUpdate;

void InstallService()
{
	LPTSTR	lpSysPath = new char[MAX_PATH];
	GetSystemDirectory(lpSysPath, MAX_PATH);
	LPCTSTR	lpSysFileName;
	SERVICE_DESCRIPTION	stgSvrDes;
	stgSvrDes.lpDescription = m_strServiceDescription.GetBuffer();
	CString	strRunPath;
	::GetModuleFileName(::AfxGetInstanceHandle(), strRunPath.GetBuffer(MAX_PATH), MAX_PATH);
	lpSysFileName = strRunPath;
	scm = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if(scm == NULL)
	{
		printf("加载服务组件失败!\n");
		delete lpSysPath;
		return;
	}
	svc = CreateService(scm, m_strServiceName, m_strServiceDisplayName, SERVICE_ALL_ACCESS,
		SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS, SERVICE_AUTO_START,
		SERVICE_ERROR_IGNORE, lpSysFileName, NULL, NULL, NULL, NULL, NULL);
	Sleep(1000);
	ChangeServiceConfig2(svc, SERVICE_CONFIG_DESCRIPTION, &stgSvrDes);
	SC_ACTION	ssa[1];
	ssa[0].Delay = 5000;
	ssa[0].Type = SC_ACTION_RESTART;
	sfa.dwResetPeriod = 0;
	sfa.lpCommand = NULL;
	sfa.lpRebootMsg = NULL;
	sfa.cActions = 1;
	sfa.lpsaActions = ssa;
	ChangeServiceConfig2(svc, SERVICE_CONFIG_FAILURE_ACTIONS, &sfa);
	Sleep(1000);
	if(svc == NULL)
	{
		printf("加载服务组件失败!\n");
		delete lpSysPath;
		return;
	}
	svc = OpenService(scm, m_strServiceName, SERVICE_START);
	if(svc == NULL)
	{
		printf("加载服务组件失败!\n");
		delete lpSysPath;
		return;
	}
	StartService(svc, 0 , NULL);
	CloseServiceHandle(svc);
	CloseServiceHandle(scm);
	delete lpSysPath;
	printf("加载服务组件成功\n");
}

void RemoveService()
{
	SC_HANDLE	schService;
	SC_HANDLE	schSCManager;
	SERVICE_STATUS ssStatus;
	schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if(schSCManager)
	{
		schService = OpenService(schSCManager, m_strServiceName, SERVICE_ALL_ACCESS);
		if(schService)
		{
			if(ControlService(schService, SERVICE_CONTROL_STOP, &ssStatus))
			{
				printf("正在停止服务");
				Sleep(1000);
				while(QueryServiceStatus(schService, &ssStatus))
				{
					if(SERVICE_STOP_PENDING == ssStatus.dwCurrentState)
					{
						printf(".");
						Sleep(1000);
					}
					else break;
				}
				if(SERVICE_STOPPED == ssStatus.dwCurrentState)
					printf("\n停止服务成功\n");
				else
					printf("\n停止服务失败\n");
			}
			if(DeleteService(schService))
				printf("移除服务成功\n");
			else
				printf("移除服务失败\n");
			CloseServiceHandle(schSCManager);
		}
		else
			printf("移除服务失败\n");
	}
}

void ToTray(void)
{
	NOTIFYICONDATA nid;
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = g_hWnd;
	nid.uID = IDI_ICON1;
	nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	nid.uCallbackMessage = WM_SHOWTASK;
	nid.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
	strcpy(nid.szTip, "HveServer");
	Shell_NotifyIcon(NIM_ADD, &nid);
	ShowWindow(g_hWnd, SW_HIDE);
}

void ExitTray(WPARAM wParam, LPARAM lParam)
{
	if(wParam != IDI_ICON1) return;
	if(lParam == WM_LBUTTONDBLCLK)
	{
		NOTIFYICONDATA	nid;
		nid.cbSize = sizeof(NOTIFYICONDATA);
		nid.hWnd = g_hWnd;
		nid.uID = IDI_ICON1;
		nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
		nid.uCallbackMessage = WM_SHOWTASK;
		nid.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
		strcpy(nid.szTip, "HveSever");
		Shell_NotifyIcon(NIM_DELETE, &nid);
		ShowWindow(g_hWnd, SW_SHOW);
	}
}

BOOL CALLBACK DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case WM_INITDIALOG:
		
		break;
	case WM_PAINT:
		g_fIsUpdate = TRUE;
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDC_COMBO1:
			switch(HIWORD(wParam))
			{
			case CBN_SELCHANGE:
				g_fIsUpdate = TRUE;
				break;
			}
			break;
		case IDCANCEL:
			ToTray();
			break;
		}
		break;
	case WM_ACTIVATE:
		g_fIsUpdate = TRUE;
		break;
	case WM_SHOWTASK:
		ExitTray(wParam, lParam);
		g_fIsUpdate = TRUE;
		break;
	}
	return FALSE;
}

void OutPutMessage(LPSTR szMsg);

UINT ServiceProcess(LPVOID lpVoid)
{
	InitCommonControls();
	g_hWnd = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);
	ShowWindow(g_hWnd, TRUE);
	UpdateWindow(g_hWnd);
	g_fIsUpdate = FALSE;

	CHveManager* pManager = new CHveManager();

	MSG msg;
	while(GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}

void WINAPI ServiceCtrl(DWORD dwOpcode)
{
	switch(dwOpcode)
	{
	case SERVICE_CONTROL_STOP:
		ss.dwCurrentState = SERVICE_STOPPED;
		SetServiceStatus(ssh, &ss);
		break;
	case SERVICE_CONTROL_CONTINUE:
		ss.dwCurrentState = SERVICE_RUNNING;
		SetServiceStatus(ssh, &ss);
		break;
	case SERVICE_CONTROL_PAUSE:
		ss.dwCurrentState = SERVICE_PAUSED;
		SetServiceStatus(ssh, &ss);
		break;
	case SERVICE_CONTROL_INTERROGATE:
		break;
	}
	SetServiceStatus(ssh, &ss);
}

void WINAPI ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv)
{
	ss.dwServiceType = SERVICE_WIN32;
	ss.dwCurrentState = SERVICE_START_PENDING;
	ss.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PAUSE_CONTINUE;
	ss.dwServiceSpecificExitCode = 0;
	ss.dwWin32ExitCode = 0;
	ss.dwCheckPoint = 0;
	ss.dwWaitHint = 0;
	ssh = RegisterServiceCtrlHandler(m_strServiceName, ServiceCtrl);
	ss.dwCurrentState = SERVICE_RUNNING;
	ss.dwCheckPoint = 0;
	ss.dwWaitHint = 0;
	SetServiceStatus(ssh, &ss);
	AfxBeginThread(ServiceProcess, NULL, NULL);
	ss.dwCurrentState = SERVICE_RUNNING;
	ss.dwCheckPoint = 0;
	ss.dwWaitHint = 0;
	SetServiceStatus(ssh, &ss);
}

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{

	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		_tprintf(_T("程序初始化失败\n"));
		return -1;
	}

	char szTempPath[MAX_PATH] = {0};
	::GetModuleFileName(::AfxGetInstanceHandle(), szTempPath, MAX_PATH);
	PathRemoveFileSpec(szTempPath);
	m_strLogPath = szTempPath;
	m_strRunningInfo = szTempPath;
	m_strLogPath += "\\HveDataBaseController.log";
	m_strRunningInfo += "\\HveServerRunning.log";
	//ServiceProcess(NULL);
	//return 0;

	SC_HANDLE schService;
	SC_HANDLE schSCManager;
	schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if(schSCManager)
	{
		schService = OpenService(schSCManager, m_strServiceName, SERVICE_ALL_ACCESS);
		if(schService)
		{

		}
		else
		{
			InstallService();
		}
	}
	
	if(argc >= 2)
	{
		if(strcmp(argv[1], "Install") == 0)
		{
			InstallService();
			return 0;
		}
		else if(strcmp(argv[1], "Remove") == 0)
		{
			RemoveService();
			return 0;
		}
		else
		{
			printf("错误的命令\n\"Install\"   加载启动服务\n\"Remove\"   停止移除服务");
			return 0;
		}
	}
	else
	{
		SERVICE_TABLE_ENTRYA ste[2];
		ste[0].lpServiceName = m_strServiceName.GetBuffer();
		ste[0].lpServiceProc = ServiceMain;
		ste[1].lpServiceName = NULL;
		ste[1].lpServiceProc = NULL;
		StartServiceCtrlDispatcher(ste);
	}
	return 0;
}

void OutPutMessage(LPSTR szMsg)
{
#ifdef OUT_PUT_LENGHT
	HANDLE hFile;
	DWORD dwFileSize;
	hFile = CreateFile(m_strRunningInfo.GetBuffer(), GENERIC_READ | GENERIC_WRITE,
		0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile != INVALID_HANDLE_VALUE)
	{
		dwFileSize = GetFileSize(hFile, NULL);
		CloseHandle(hFile);
		if(dwFileSize >= 1048576)
		{
			DeleteFile(m_strRunningInfo.GetBuffer());
		}
	}
	CTime cTime = CTime::GetCurrentTime();
	CString strTime = cTime.Format("<%Y-%m-%d %H:%M:%S>");
	FILE* fp = fopen(m_strRunningInfo.GetBuffer(), "a+");
	if(fp)
	{
		fwrite(strTime.GetBuffer(), strTime.GetLength(), 1, fp);
		fwrite(szMsg, strlen(szMsg), 1, fp);
		fwrite("\n", 1, 1, fp);
		fclose(fp);
	}
#endif
	if(g_hWnd == NULL)
	{
		return;
	}

	int iIndex;
	iIndex = (int)SendDlgItemMessage(g_hWnd, IDC_LIST1, LB_GETCOUNT, 0, 0);
	if(iIndex > 20)
	{
		SendDlgItemMessage(g_hWnd, IDC_LIST1, LB_DELETESTRING, 0, 0);
	}
	SendDlgItemMessage(g_hWnd, IDC_LIST1, LB_ADDSTRING, 0, (LPARAM)szMsg);
}
