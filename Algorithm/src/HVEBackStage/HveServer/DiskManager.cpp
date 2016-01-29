#include "stdafx.h"
#include "DiskManager.h"
#include "resource.h"
#include "math.h"
//#include "dbghelp.h"

//in function SaveFile() call MakeSureDirectoryPathExists(sztmpPath);
#include <imagehlp.h>   
#include ".\diskmanager.h"
#pragma comment(lib,"imagehlp.lib") 

extern HWND	g_hWnd;
extern BOOL	g_fIsUpdate;
extern void OutPutMessage(LPSTR szMsg);

CDiskManager::CDiskManager()
{
	memset(m_szCurrentDisk, 0, 3);
	memset(m_szDiskList, 0, 21);
	m_nDiskCount = 0;
	char szAppPath[MAX_PATH] = {0};
	GetModuleFileName(NULL, szAppPath, MAX_PATH);
	PathRemoveFileSpec(szAppPath);
	strcat(szAppPath, "\\StartUp.config");
	CFileFind FindFile;
	if(!FindFile.FindFile(szAppPath))
	{
		//MessageBox(NULL, "获取配置信息失败，服务启动失败!", "ERROR", MB_OK | MB_ICONERROR);
		OutPutMessage("获取配置信息失败，服务启动失败!");
		ExitProcess(0);
	}
	char szTmp[256] = {0};
	GetPrivateProfileStringA("DiskInfo", "DiskList", "", szTmp, 256, szAppPath);
	if(strcmp(szTmp, "") == 0)
	{
		//MessageBox(NULL, "获取配置信息失败， 服务启动失败!", "ERROR", MB_OK | MB_ICONERROR);
		OutPutMessage("获取配置信息失败，服务启动失败!");
		ExitProcess(0);
	}
	memcpy(m_szDiskList, szTmp, strlen(szTmp));
	CountDiskNum();
	memset(szTmp, 0, 256);
	GetPrivateProfileStringA("DiskInfo", "CurrentDir", "", szTmp, 256, szAppPath);
	if(strcmp(szTmp, "") == 0)
	{
		//MessageBox(NULL, "获取配置信息失败， 服务启动失败!", "ERROR", MB_OK | MB_ICONERROR);
		OutPutMessage("获取配置信息失败，服务启动失败!");
		ExitProcess(0);
	}
	memcpy(m_szCurrentDisk, szTmp, strlen(szTmp));
	memcpy(m_szIniFileName, szAppPath, MAX_PATH);
	CountCurrentDiskIndex();
	CheckDiskIsExsit();
	_GetDiskFreeSpace(m_szCurrentDisk, m_lDiskFreeByte);
	m_fIsExit = FALSE;
	m_fIsSaving = FALSE;
	OutPutDiskListInfo();
	m_hManageThread = CreateThread(NULL, 0, ManageThread, this, 0, &m_dwManageThread);
}

CDiskManager::~CDiskManager()
{
	m_fIsExit = TRUE;
	if(m_hManageThread)
	{
		int iWaitTimes = 0;
		int MAX_WAIT_TIME = 10;
		while(WaitForSingleObject(m_hManageThread, 500) == WAIT_TIMEOUT && iWaitTimes < MAX_WAIT_TIME)
		{
			iWaitTimes++;
		}
		if(iWaitTimes >= MAX_WAIT_TIME)
		{
			TerminateThread(m_hManageThread, 0);
			OutPutMessage("强行关闭文件保存服务线程...");
		}
		CloseHandle(m_hManageThread);
	}
}

void CDiskManager::CountDiskNum()
{
	int nIndex, nLeght;
	nLeght = ((int)strlen(m_szDiskList) >> 1);
	for(nIndex=0; nIndex<nLeght; nIndex++)
	{
		if((m_szDiskList[(nIndex<<1)] >= 'A' && m_szDiskList[(nIndex<<1)] <= 'Z')
			|| (m_szDiskList[(nIndex<<1)] >= 'a' && m_szDiskList[(nIndex<<1)] <= 'z'))
		{
			m_nDiskCount++;
		}
	}
}

void CDiskManager::CountCurrentDiskIndex()
{
	int nIndex;
	for(nIndex=0; nIndex<m_nDiskCount; nIndex++)
	{
		if(m_szCurrentDisk[0] == m_szDiskList[(nIndex<<1)])
		{
			m_nCurrentDiskIndex = nIndex;
			return;
		}
	}
	//MessageBox(NULL ,"获取当前保存磁盘编号失败!", "ERROR", MB_OK | MB_ICONERROR);
	OutPutMessage("获取当前保存磁盘编号失败!");
	ExitProcess(0);
}

void CDiskManager::_GetDiskFreeSpace(char* szDiskChar, ULONGLONG& DiskFreeSpace)
{
	DiskFreeSpace = 0;
	if(szDiskChar == NULL)	return;
	_ULARGE_INTEGER lpFreeBytesAvailableToCaller, lpTotalNumberOfBytes, lpTotalNumberOfFreeBytes;
	GetDiskFreeSpaceEx(szDiskChar, &lpFreeBytesAvailableToCaller, &lpTotalNumberOfBytes, &lpTotalNumberOfFreeBytes);
	DiskFreeSpace = lpTotalNumberOfFreeBytes.QuadPart;
}

void CDiskManager::OutPutDiskListInfo()
{
	if(g_hWnd == NULL)return;
	int nIndex;
	for(nIndex=0; nIndex<m_nDiskCount; nIndex++)
	{
		char szTmp[3];
		szTmp[0] = m_szDiskList[nIndex<<1];
		szTmp[1] = ':';
		szTmp[2] = '\0';
		SendDlgItemMessage(g_hWnd, IDC_COMBO1, CB_ADDSTRING, (WPARAM)(0), (LPARAM)szTmp);
	}
	SendDlgItemMessage(g_hWnd, IDC_COMBO1, CB_SETCURSEL, (WPARAM)0, 0);

	DrawDiskInfo();
}

void CDiskManager::DrawDiskInfo()
{
	if(g_hWnd == NULL) return;
	char szTmp[30];
	GetDlgItemText(g_hWnd, IDC_COMBO1, szTmp, 10);
	_ULARGE_INTEGER lpFreeBytesAvailableToCaller, lpTotalNumberOfBytes, lpTotalNumberOfFreeBytes;
	GetDiskFreeSpaceEx(szTmp, &lpFreeBytesAvailableToCaller, &lpTotalNumberOfBytes, &lpTotalNumberOfFreeBytes);
	HWND hPicFram;
	hPicFram = GetDlgItem(g_hWnd, IDC_STATIC22);
	HDC tmpDC;
	tmpDC = GetDC(hPicFram);
	int nXOffSet = (int)(110 * cos(6.2831852 * (lpTotalNumberOfBytes.QuadPart - lpTotalNumberOfFreeBytes.QuadPart) / lpTotalNumberOfBytes.QuadPart));
	int nYOffSet = (int)(110 * sin(6.2831852 * (lpTotalNumberOfBytes.QuadPart - lpTotalNumberOfFreeBytes.QuadPart) / lpTotalNumberOfBytes.QuadPart));
	HBRUSH	tmpBrush;
	tmpBrush = CreateSolidBrush(RGB(0, 0, 255));
	SelectObject(tmpDC, tmpBrush);
	Pie(tmpDC, 30, 40, 250, 260, 140-nXOffSet, 150-nYOffSet, 30, 150);
	DeleteObject(tmpBrush);
	tmpBrush = CreateSolidBrush(RGB(255, 0, 255));
	SelectObject(tmpDC, tmpBrush);
	Pie(tmpDC, 30, 40, 250, 260, 30, 150, 140-nXOffSet, 150-nYOffSet);
	DeleteObject(tmpBrush);
	ReleaseDC(hPicFram, tmpDC);

	int nTotle,nFree, nTotleS, nFreeS;
	nTotle = (int)(lpTotalNumberOfBytes.QuadPart/1024/1024/1024);
	nTotleS = (int)((lpTotalNumberOfBytes.QuadPart%1073741824)/1024/10240);
	nFree = (int)(lpTotalNumberOfFreeBytes.QuadPart/1024/1024/1024);
	nFreeS = (int)((lpTotalNumberOfFreeBytes.QuadPart%1073741824)/1024/10240);
	sprintf(szTmp, "容量:%d.%2dGB,空闲:%d.%2dGB", nTotle, nTotleS, nFree, nFreeS);
	SetWindowText(GetDlgItem(g_hWnd, IDC_STATIC33), szTmp);
}

void CDiskManager::ShowCurrentDiskInfo()
{
	_ULARGE_INTEGER lpFreeBytesAvailableToCaller, lpTotalNumberOfBytes, lpTotalNumberOfFreeBytes;
	GetDiskFreeSpaceEx(m_szCurrentDisk, &lpFreeBytesAvailableToCaller, &lpTotalNumberOfBytes, &lpTotalNumberOfFreeBytes);
	SetWindowText(GetDlgItem(g_hWnd, IDC_STATIC11), m_szCurrentDisk);
	int nUsed;
	nUsed = (int)((lpTotalNumberOfBytes.QuadPart-lpTotalNumberOfFreeBytes.QuadPart) * 5000 / (float)(lpTotalNumberOfBytes.QuadPart));
	SendDlgItemMessage(g_hWnd, IDC_PROGRESS1, PBM_SETRANGE, 0, MAKELPARAM(0, 5000));
	SendDlgItemMessage(g_hWnd, IDC_PROGRESS1, PBM_SETPOS, (WPARAM)nUsed, 0);
}

BOOL CDiskManager::CheckDiskIsEmpty(LPCSTR lpszDir)
{
	char szTmpFileFind[MAX_PATH];
	sprintf(szTmpFileFind, "%s\\*", lpszDir);
	CFileFind tmpFind;
	if(tmpFind.FindFile(szTmpFileFind))return FALSE;
	return TRUE;
}

BOOL CDiskManager::CheckDiskIsExsit()
{
	WORD nIndex;
	char szDiskName[3];
	UINT	uType;
	for(nIndex=0; nIndex<m_nDiskCount; nIndex++)
	{
		szDiskName[0] = m_szDiskList[nIndex<<1];
		szDiskName[1] = ':';
		szDiskName[2] = '\0';
		uType = GetDriveType(szDiskName);
		if(uType != DRIVE_FIXED)
		{
			char szInfo[80];
			sprintf(szInfo, "盘符%s的磁盘格式不对或不存在，请重新配置", szDiskName);
			//MessageBox(NULL, szInfo, "错误", MB_OK | MB_ICONERROR);
			OutPutMessage(szInfo);
			ExitProcess(0);
		}
	}
	return TRUE;
}

DWORD CDiskManager::ManageThread(LPVOID lpParma)
{
	DWORD	dwLastFreshTimes;
	dwLastFreshTimes = GetTickCount();
	CDiskManager* m_manager = (CDiskManager*)lpParma;
	while(!m_manager->m_fIsExit)
	{
		if(GetTickCount() - dwLastFreshTimes >= 120000)
		{
			ULONGLONG	lTmpFreeSpace = 0;
			if(m_manager->m_fIsSaving) continue;
			m_manager->_GetDiskFreeSpace(m_manager->m_szCurrentDisk, lTmpFreeSpace);
			if(m_manager->m_lDiskFreeByte != lTmpFreeSpace)
			{
				m_manager->m_lDiskFreeByte = lTmpFreeSpace;
			}
			dwLastFreshTimes = GetTickCount();
		}
		
		if(m_manager->m_lDiskFreeByte <= 5368709120)
		{
			int nNewDiskIndex;
			char szNewDiskChar[3];
			if(m_manager->m_fIsSaving) continue;

			nNewDiskIndex = m_manager->m_nCurrentDiskIndex + 1;
			if(nNewDiskIndex >= m_manager->m_nDiskCount)
			{
				nNewDiskIndex = 0;
			}
			szNewDiskChar[0] = m_manager->m_szDiskList[(nNewDiskIndex<<1)];
			szNewDiskChar[1] = ':';
			szNewDiskChar[2] = '\0';
			UINT	nDiskType;
			nDiskType = GetDriveType(szNewDiskChar);
			if(nDiskType != DRIVE_FIXED)
			{
				//MessageBox(NULL, "更换新的存储盘失败，程序退出!", "ERROR", MB_OK | MB_ICONERROR);
				OutPutMessage("更换新的存储盘失败，程序退出!");
				ExitProcess(0);
			}
			


	//		DWORD64	dw64Begin = GetTickCount();

			//char szCmdLine[100];

			//system弹窗
			//sprintf(szCmdLine, "%s %s /Q /Y","format", szNewDiskChar);
			//system(szCmdLine);

			//ShellExecute 不阻塞
			//sprintf(szCmdLine, "%s /Q /Y", szNewDiskChar);
			//ShellExecute(NULL, "", "format", szCmdLine, NULL, SW_HIDE);
			//while(!m_manager->CheckDiskIsEmpty(szNewDiskChar))
			//{
			//	Sleep(100);
			//}

			//---------------------------------------------------
			char szParam[100];
			sprintf(szParam, " %s /Q /Y", szNewDiskChar);
			SHELLEXECUTEINFO ShellInfo;
			memset(&ShellInfo, 0, sizeof(ShellInfo));
		    ShellInfo.cbSize = sizeof(ShellInfo);
			ShellInfo.hwnd = NULL;
			ShellInfo.lpVerb = NULL;
			ShellInfo.lpFile = "C:\\WINDOWS\\system32\\format";
			ShellInfo.lpParameters = szParam;
			ShellInfo.nShow = SW_HIDE;
			ShellInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
			ShellExecuteEx(&ShellInfo);
			WaitForSingleObject(ShellInfo.hProcess,INFINITE);
			//---------------------------------------------------

	//		Sleep(2000);
			//DWORD64	dw64End = GetTickCount();
			//CString strUsedTime;
			//strUsedTime.Format("%d", dw64End - dw64Begin);
			//AfxMessageBox(strUsedTime);
			memcpy(m_manager->m_szCurrentDisk, szNewDiskChar, 3);
			m_manager->m_nCurrentDiskIndex = nNewDiskIndex;
			WritePrivateProfileStringA("DiskInfo", "CurrentDir", m_manager->m_szCurrentDisk, m_manager->m_szIniFileName);
			m_manager->_GetDiskFreeSpace(m_manager->m_szCurrentDisk, m_manager->m_lDiskFreeByte);

		}

		if(g_fIsUpdate)
		{
			m_manager->DrawDiskInfo();
			g_fIsUpdate = FALSE;
		}

		m_manager->ShowCurrentDiskInfo();
		Sleep(2000);
	}

	return 0;
}
BOOL CDiskManager::SaveTextFile(LPSTR	szSaveName, char* pText, DWORD32 dwTextSize, CString& strSavePath)
{
	char szSavePath[MAX_PATH] = {0};
	char sztmpPath[MAX_PATH] = {0};
	if(m_fIsExit)return FALSE;
	m_fIsSaving = TRUE;

	strSavePath.Format("");
	sprintf(szSavePath, "%s\\%s", m_szCurrentDisk, szSaveName);
	memcpy(sztmpPath, szSavePath, strlen(szSavePath));
	PathRemoveFileSpec(sztmpPath);
	strcat(sztmpPath, "\\");
	MakeSureDirectoryPathExists(sztmpPath);
	FILE* fp;
	fp = fopen(szSavePath, "aw");
	if(fp)
	{
		fwrite(pText, dwTextSize, 1, fp);
		fwrite("\n",1,1,fp); 
		fclose(fp);
		m_lDiskFreeByte -= (dwTextSize+1);
		strSavePath.Format("%s", szSavePath);
		m_fIsSaving = FALSE;
		return TRUE;
	}
	m_fIsSaving = FALSE;
	return FALSE;

}

BOOL CDiskManager::SaveFile(LPSTR szSaveName, PBYTE pbImage, DWORD32 dwImgSize, CString& strSavePath)
{
	char szSavePath[MAX_PATH] = {0};
	char sztmpPath[MAX_PATH] = {0};
	if(m_fIsExit)return FALSE;
	m_fIsSaving = TRUE;

	strSavePath.Format("");
	sprintf(szSavePath, "%s\\%s", m_szCurrentDisk, szSaveName);
	memcpy(sztmpPath, szSavePath, strlen(szSavePath));
	PathRemoveFileSpec(sztmpPath);
	strcat(sztmpPath, "\\");
	MakeSureDirectoryPathExists(sztmpPath);
	
	FILE* fp;
	fp = fopen(szSavePath, "wb");
	if(fp)
	{
		fwrite(pbImage, dwImgSize, 1, fp);
		fclose(fp);
		m_lDiskFreeByte -= dwImgSize;
		strSavePath.Format("%s", szSavePath);
		m_fIsSaving = FALSE;
		return TRUE;
	}
	//else
	//{
	//	CString strTmp;
	//	strTmp.Format("出错代码:%d",errno);
	//	OutPutMessage(strTmp.GetBuffer());
	//	AfxMessageBox(szSavePath);
	//	
	//}
	m_fIsSaving = FALSE;
	return FALSE;



	////----------------  读取ini文件中的保存路径 -------------

	//char szSavePath[MAX_PATH] = {0};
	//char sztmpPath[MAX_PATH] = {0};
	//if(m_fIsExit)return FALSE;
	//m_fIsSaving = TRUE;

	//char szAppPath[MAX_PATH] = {0};
	//GetModuleFileName (NULL, szAppPath, MAX_PATH);
	//PathRemoveFileSpec(szAppPath);
	//strcat(szAppPath, "\\Config.ini");
	//char szTmp[256] = {0};

	//GetPrivateProfileStringA("HVEInfo", "ImagePath", "", szTmp, 256, szAppPath);
	//if(strcmp(szTmp, "") == 0)
	//{
	//	WritePrivateProfileStringA("HVEInfo", "ImagePath","", szAppPath);
	//}
	//
	//strSavePath.Format("");
	//if(szTmp)
	//{
	//	sprintf(szSavePath, "%s\\%s", szTmp, szSaveName);
	//}
	//else
	//{
	//	sprintf(szSavePath, "%s\\%s", m_szCurrentDisk, szSaveName);
	//}
	//memcpy(sztmpPath, szSavePath, strlen(szSavePath));
	//PathRemoveFileSpec(sztmpPath);
	//strcat(sztmpPath, "\\");
	//MakeSureDirectoryPathExists(sztmpPath);
	//FILE* fp;
	//fp = fopen(szSavePath, "wb");
	//if(fp)
	//{
	//	fwrite(pbImage, dwImgSize, 1, fp);
	//	fclose(fp);
	//	m_lDiskFreeByte -= dwImgSize;
	//	strSavePath.Format("%s", szSavePath);
	//	m_fIsSaving = FALSE;
	//	return TRUE;
	//}
	//m_fIsSaving = FALSE;
	//return FALSE;

	////-----------------------------------------------------------
}

//预先获取图片保存路径
BOOL CDiskManager::SaveFile(LPSTR szSaveName, CString& strSavePath)
{
	char szSavePath[MAX_PATH] = {0};
	char sztmpPath[MAX_PATH] = {0};
	if(m_fIsExit)return FALSE;


	strSavePath.Format("");
	sprintf(szSavePath, "%s\\%s", m_szCurrentDisk, szSaveName);
	memcpy(sztmpPath, szSavePath, strlen(szSavePath));
	PathRemoveFileSpec(sztmpPath);
	strcat(sztmpPath, "\\");
	MakeSureDirectoryPathExists(sztmpPath);
	
	strSavePath.Format("%s", szSavePath);

	return TRUE;
}

//BOOL CDiskManager::GetSaveFileDir(LPSTR szSaveName,  CString& strSavePath)
//{
//
//	////----------------  读取ini文件中的保存路径 -------------
//
//	char szSavePath[MAX_PATH] = {0};
//	char sztmpPath[MAX_PATH] = {0};
//
//	char szAppPath[MAX_PATH] = {0};
//	GetModuleFileName (NULL, szAppPath, MAX_PATH);
//	PathRemoveFileSpec(szAppPath);
//	strcat(szAppPath, "\\Config.ini");
//	char szTmp[256] = {0};
//
//	GetPrivateProfileStringA("HVEInfo", "ImagePath", "", szTmp, 256, szAppPath);
//	if(strcmp(szTmp, "") == 0)
//	{
//		WritePrivateProfileStringA("HVEInfo", "ImagePath","", szAppPath);
//	}
//	
//	strSavePath.Format("");
//	if(szTmp)
//	{
//		sprintf(szSavePath, "%s\\%s", szTmp, szSaveName);
//	}
//	else
//	{
//		sprintf(szSavePath, "%s\\%s", m_szCurrentDisk, szSaveName);
//	}
//
//	strSavePath.Format("%s", szSavePath);
//
//	return 0;
//	////-----------------------------------------------------------
//}


//int CDiskManager::GetSaveImagePath(char pSavePath[])
//{
//	char szSavePath[MAX_PATH] = {0};
//	char sztmpPath[MAX_PATH] = {0};
//
//	char szAppPath[MAX_PATH] = {0};
//	GetModuleFileName (NULL, szAppPath, MAX_PATH);
//	PathRemoveFileSpec(szAppPath);
//	strcat(szAppPath, "\\Config.ini");
//	char szTmp[256] = {0};
//
//	GetPrivateProfileStringA("HVEInfo", "ImagePath", "", szTmp, 256, szAppPath);
//	if(strcmp(szTmp, "") == 0)
//	{
//		WritePrivateProfileStringA("HVEInfo", "ImagePath","", szAppPath);
//	}
//	
//	if(szTmp)
//	{
////		sprintf(szSavePath, "%s", szTmp, );
//		memcpy(pSavePath, szTmp, MAX_PATH);
//	}
//	else
//	{
//		memcpy(pSavePath, m_szCurrentDisk, MAX_PATH);
////		sprintf(szSavePath, "%s", m_szCurrentDisk);
//	}
//	//memcpy(sztmpPath, szSavePath, strlen(szSavePath));
//	//PathRemoveFileSpec(sztmpPath);
//	//strcat(sztmpPath, "\\");
//	//MakeSureDirectoryPathExists(sztmpPath);
//
//	return 0;
//}

//BOOL CDiskManager::SaveRecordLog(LPSTR LogName, char* LogMsg, DWORD32 MsgLen)
//{
//	char szSavePath[MAX_PATH] = {0};
//	char sztmpPath[MAX_PATH] = {0};
//	if(m_fIsExit)return FALSE;
//	m_fIsSaving = TRUE;
//////------------------- 读取 ini 文件保存参数 ---------------------
//	//char szAppPath[MAX_PATH] = {0};
//	//GetModuleFileName (NULL, szAppPath, MAX_PATH);
//	//PathRemoveFileSpec(szAppPath);
//	//strcat(szAppPath, "\\Config.ini");
//	//char szTmp[256] = {0};
//
//	//GetPrivateProfileStringA("HVEInfo", "ImagePath", "", szTmp, 256, szAppPath);
//	//if(strcmp(szTmp, "") == 0)
//	//{
//	//	WritePrivateProfileStringA("HVEInfo", "ImagePath","", szAppPath);
//	//}
//	//
//	//if(szTmp)
//	//{
//	//	sprintf(szSavePath, "%s\\%s", szTmp, LogName);
//	//}
//	//else
//	//{
//	//	sprintf(szSavePath, "%s\\%s", m_szCurrentDisk, LogName);
//	//}
//////---------------------------------------------------------------
//	
//	strSavePath.Format("");
//	sprintf(szSavePath, "%s\\%s", m_szCurrentDisk, szSaveName);
//
//	memcpy(sztmpPath, szSavePath, strlen(szSavePath));
//	PathRemoveFileSpec(sztmpPath);
//	strcat(sztmpPath, "\\");
//	MakeSureDirectoryPathExists(sztmpPath);
//	FILE* fp;
//	fp = fopen(szSavePath, "aw");
//	if(fp)
//	{
//		fwrite(LogMsg, MsgLen, 1, fp);
//		fwrite("\n", 1, 1, fp);
//		fclose(fp);
//
//		m_fIsSaving = FALSE;
//		return TRUE;
//	}
//	m_fIsSaving = FALSE;
//	return FALSE;
//
//}
