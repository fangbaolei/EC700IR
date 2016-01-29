#pragma once
#include <atlstr.h>

#define MAX_DISK_COUNT	10

class CDiskManager
{
public:
	CDiskManager();
	~CDiskManager();
	static DWORD WINAPI ManageThread(LPVOID lpParma);
	BOOL SaveFile(LPSTR	szSaveName, PBYTE pbImage, DWORD32 dwImgSize, CString& strSavePath);
	BOOL SaveFile(LPSTR	szSaveName, CString& strSavePath);
	BOOL SaveTextFile(LPSTR	szSaveName, char* pText, DWORD32 dwTextSize, CString& strSavePath);
	

private:
	void CountDiskNum(void);
	void CountCurrentDiskIndex(void);
	void _GetDiskFreeSpace(char* szDiskChar, ULONGLONG& DiskFreeSpace);
	void OutPutDiskListInfo(void);
	void DrawDiskInfo(void);
	void ShowCurrentDiskInfo(void);
	BOOL CheckDiskIsEmpty(LPCSTR lpszDir);
	BOOL CheckDiskIsExsit(void);

private:
	char		m_szCurrentDisk[3];
	char		m_szDiskList[(MAX_DISK_COUNT<<1) + 1];
	char		m_szIniFileName[MAX_PATH];
	int			m_nDiskCount;
	int			m_nCurrentDiskIndex;
	ULONGLONG	m_lDiskFreeByte;
	BOOL		m_fIsExit;
	DWORD		m_dwManageThread;
	HANDLE		m_hManageThread;
	BOOL		m_fIsSaving;
public:
//	BOOL GetSaveFileDir(LPSTR szSaveName, CString& strSavePath);
//	int GetSaveImagePath(char pSavePath[]);
//	BOOL CDiskManager::SaveRecordLog(LPSTR LogName, char* LogMsg, DWORD32 MsgLen);
};
