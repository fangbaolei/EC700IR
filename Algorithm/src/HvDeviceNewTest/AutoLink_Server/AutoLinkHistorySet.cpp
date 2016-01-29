// AutoLinkHistorySet.cpp : implementation file
//

#include "stdafx.h"
#include "AutoLink_demo.h"
#include "AutoLinkHistorySet.h"


// AutoLinkHistorySet dialog

IMPLEMENT_DYNAMIC(AutoLinkHistorySet, CDialog)
AutoLinkHistorySet::AutoLinkHistorySet(CWnd* pParent /*=NULL*/)
	: CDialog(AutoLinkHistorySet::IDD, pParent)
{
}

AutoLinkHistorySet::~AutoLinkHistorySet()
{
}

void AutoLinkHistorySet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BEGIN_TIME, m_beginTimeCtrl);
	DDX_Control(pDX, IDC_ENABLE_HISTORY_CHECK, m_enableRecvHistoryBut);
	DDX_Control(pDX, IDC_INDEX_EDIT, m_indexEdit);
	DDX_Control(pDX, IDC_ENABLE_END_TIME_CHECK, m_enableEndTimeBut);
	DDX_Control(pDX, IDC_END_TIME, m_endTimeCtrl);
	DDX_Control(pDX, IDC_ENABLE_ONLYVIOLATION_CHECK, m_onlyViolationBut);
}


BEGIN_MESSAGE_MAP(AutoLinkHistorySet, CDialog)
	ON_BN_CLICKED(IDC_SAVE_BUTTON, OnBnClickedSaveButton)
	ON_BN_CLICKED(IDC_CANCEL_BUTTON, OnBnClickedCancelButton)
	ON_BN_CLICKED(IDC_ENABLE_HISTORY_CHECK, OnEnableHistory)
	ON_BN_CLICKED(IDC_ENABLE_END_TIME_CHECK, OnEnableEndTime)
END_MESSAGE_MAP()


// AutoLinkHistorySet message handlers
void AutoLinkHistorySet::InitHistorySet(char* pSN, bool* fSetParam)
{
	if (NULL == pSN || NULL == fSetParam)
	{
		return;
	}
	strcpy(m_szDevSN, pSN);

	*fSetParam = false;
	m_fSetParam = fSetParam;
}

BOOL AutoLinkHistorySet::OnInitDialog()
{
	CDialog::OnInitDialog();

	InitParamDlg();
	return TRUE;
}

// 读ini
void AutoLinkHistorySet::ReadIniFile()
{
	TCHAR fileName[MAX_PATH];
	GetModuleFileName(NULL, fileName, MAX_PATH-1);
	PathRemoveFileSpec(fileName);
	TCHAR iniFileName[MAX_PATH] = { 0 };
	strcpy(iniFileName, fileName);
	strcat(iniFileName, _T("\\SafeModeConfig.ini"));

	SYSTEMTIME systemTime;
	GetSystemTime(& systemTime);
	char szDefaultTime[64];
	ZeroMemory(szDefaultTime , sizeof(szDefaultTime));
	sprintf(szDefaultTime , "%4d.%2d.%2d_%2d" , systemTime.wYear ,systemTime.wMonth , systemTime.wDay ,systemTime.wHour );
	CString strDefaultTime(szDefaultTime);
	strDefaultTime.Replace(' ','0');

	m_safeModeInfo.iEanbleSafeMode = GetPrivateProfileInt(m_szDevSN, "SafeModeEnable", 0, iniFileName);
	GetPrivateProfileString(m_szDevSN, "BeginTime", strDefaultTime, m_safeModeInfo.szBeginTime, 256, iniFileName);
	GetPrivateProfileString(m_szDevSN, "EndTime", "0", m_safeModeInfo.szEndTime, 256, iniFileName);
	m_safeModeInfo.index = GetPrivateProfileInt(m_szDevSN, "Index", 0, iniFileName);
	m_safeModeInfo.DataInfo = GetPrivateProfileInt(m_szDevSN, "DataType", 0, iniFileName);
}

// 写ini
void AutoLinkHistorySet::WriteIniFile()
{
	TCHAR fileName[MAX_PATH];
	GetModuleFileName(NULL, fileName, MAX_PATH-1);
	PathRemoveFileSpec(fileName);
	TCHAR iniFileName[MAX_PATH] = { 0 };
	strcpy(iniFileName, fileName);
	strcat(iniFileName, _T("\\SafeModeConfig.ini"));

	TCHAR chTemp[256] = {0};
	sprintf(chTemp, "%d", m_safeModeInfo.iEanbleSafeMode);
	if (m_safeModeInfo.iEanbleSafeMode == 0 )
	{
		CTime tm = CTime::GetCurrentTime();
		sprintf(m_safeModeInfo.szBeginTime,"%s", tm.Format("%Y.%m.%d_%H"));
	}

	WritePrivateProfileString(m_szDevSN, "SafeModeEnable", chTemp, iniFileName);
	WritePrivateProfileString(m_szDevSN, "BeginTime", m_safeModeInfo.szBeginTime, iniFileName);
	WritePrivateProfileString(m_szDevSN, "EndTime", m_safeModeInfo.szEndTime, iniFileName);
	sprintf(chTemp, "%d", m_safeModeInfo.index);
	WritePrivateProfileString(m_szDevSN, "Index", chTemp, iniFileName);
	sprintf(chTemp, "%d", m_safeModeInfo.DataInfo);
	WritePrivateProfileString(m_szDevSN, "DataType", chTemp, iniFileName);
}

// 初始化界面
void AutoLinkHistorySet::InitParamDlg()
{
	ReadIniFile();
	//((CButton*)GetDlgItem(IDD_AUTOLINK_SETHISTORY))->SetWindowText(m_szDevSN);
	
	m_beginTimeCtrl.SetFormat("yyyy.MM.dd_HH");
	SYSTEMTIME sysTime;
	m_beginTimeCtrl.GetTime(&sysTime);

	sscanf(m_safeModeInfo.szBeginTime, "%d.%d.%d_%d", &sysTime.wYear,
		&sysTime.wMonth, &sysTime.wDay, &sysTime.wHour);
	m_beginTimeCtrl.SetTime(&sysTime);

	CString strTemp;
	strTemp.Format("%d", m_safeModeInfo.index);
	m_indexEdit.SetWindowText(strTemp);

	m_endTimeCtrl.SetFormat("yyyy.MM.dd_HH");
	if (0 != strcmp( m_safeModeInfo.szEndTime, "0"))
	{
		sscanf(m_safeModeInfo.szEndTime, "%d.%d.%d_%d", &sysTime.wYear,
			&sysTime.wMonth, &sysTime.wDay, &sysTime.wHour);
		m_endTimeCtrl.SetTime(&sysTime);
	}
	
	if (0 != m_safeModeInfo.iEanbleSafeMode)
	{
		m_enableRecvHistoryBut.SetCheck(BST_CHECKED);
		m_beginTimeCtrl.EnableWindow(TRUE);
		m_indexEdit.EnableWindow(TRUE);
		m_endTimeCtrl.EnableWindow(TRUE);
		m_onlyViolationBut.EnableWindow(TRUE);
		
		if (0 == strcmp( m_safeModeInfo.szEndTime, "0"))
		{
			m_enableEndTimeBut.SetCheck(BST_UNCHECKED);
			m_endTimeCtrl.EnableWindow(FALSE);
		}
		else
		{
			m_enableEndTimeBut.SetCheck(BST_CHECKED);
			m_endTimeCtrl.EnableWindow(TRUE);
		}
	}
	else
	{
		m_enableRecvHistoryBut.SetCheck(BST_UNCHECKED);
		m_beginTimeCtrl.EnableWindow(FALSE);
		m_indexEdit.EnableWindow(FALSE);
		m_enableEndTimeBut.EnableWindow(FALSE);
		m_endTimeCtrl.EnableWindow(FALSE);
		m_onlyViolationBut.EnableWindow(FALSE);
		
		if (0 == strcmp( m_safeModeInfo.szEndTime,"0"))
		{
			m_enableEndTimeBut.SetCheck(BST_UNCHECKED);
		}
		else
		{
			m_enableEndTimeBut.SetCheck(BST_CHECKED);
		}
	}

	if (0 != m_safeModeInfo.DataInfo)
	{
		m_onlyViolationBut.SetCheck(BST_CHECKED);
	}
	else
	{
		m_onlyViolationBut.SetCheck(BST_UNCHECKED);
	}
}

// 保存按钮
void AutoLinkHistorySet::OnBnClickedSaveButton()
{
	CString strBeginTime;
	CString strEndTime ;
	CString strIndex;

	m_beginTimeCtrl.GetWindowText(strBeginTime);
	m_indexEdit.GetWindowText(strIndex);
	m_endTimeCtrl.GetWindowText(strEndTime);

	if (m_enableRecvHistoryBut.GetCheck() == BST_CHECKED)
		 m_safeModeInfo.iEanbleSafeMode = 1;
	else
		m_safeModeInfo.iEanbleSafeMode = 0;

	sprintf(m_safeModeInfo.szBeginTime, "%s", strBeginTime.GetBuffer(strBeginTime.GetLength()));
	strBeginTime.ReleaseBuffer();

	m_safeModeInfo.index = atoi(strIndex.GetBuffer(strIndex.GetLength()));
	strIndex.ReleaseBuffer();

	if (BST_CHECKED == m_enableEndTimeBut.GetCheck())
	{
		sprintf(m_safeModeInfo.szEndTime, "%s", strEndTime.GetBuffer(strEndTime.GetLength()));
		strEndTime.ReleaseBuffer();

		// 判断
		SYSTEMTIME sysBeginTime;
		SYSTEMTIME sysEndTime;
		sscanf(m_safeModeInfo.szBeginTime, "%d.%d.%d_%d", &sysBeginTime.wYear,
			&sysBeginTime.wMonth, &sysBeginTime.wDay, &sysBeginTime.wHour);
		sscanf(m_safeModeInfo.szEndTime, "%d.%d.%d_%d", &sysEndTime.wYear,
			&sysEndTime.wMonth, &sysEndTime.wDay, &sysEndTime.wHour);

		// 判断时间大小
		if (sysBeginTime.wYear != sysEndTime.wYear || sysBeginTime.wMonth != sysEndTime.wMonth
			|| sysBeginTime.wDay != sysEndTime.wDay || sysBeginTime.wHour != sysEndTime.wHour)
		{
			sysBeginTime.wDayOfWeek = 0;
			sysEndTime.wDayOfWeek = 0;
			if (memcmp(&sysBeginTime, &sysEndTime, sizeof(SYSTEMTIME)) > 0)
			{
				AfxMessageBox("开始时间不应晚于结束时间，请输入正确的开始及结束时间！");
				return;
			}
		}
	}
	else
	{
		sprintf(m_safeModeInfo.szEndTime, "0"); 
	}

	if (m_onlyViolationBut.GetCheck() == BST_CHECKED)
		 m_safeModeInfo.DataInfo = 1;
	else
		m_safeModeInfo.DataInfo = 0;

	*m_fSetParam = true;

	WriteIniFile();
	OnOK();
}

// 取消按钮
void AutoLinkHistorySet::OnBnClickedCancelButton()
{
	OnCancel();
	return ;
}

// 屏蔽回车和ESC
BOOL AutoLinkHistorySet::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN
			|| pMsg->wParam == VK_ESCAPE)
		{
			return TRUE;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void AutoLinkHistorySet::OnEnableHistory()
{
	if (BST_CHECKED == ((CButton*)GetDlgItem(IDC_ENABLE_HISTORY_CHECK))->GetCheck())
	{
		GetDlgItem(IDC_BEGIN_TIME)->EnableWindow(TRUE);
		GetDlgItem(IDC_INDEX_EDIT)->EnableWindow(TRUE);
		GetDlgItem(IDC_ENABLE_END_TIME_CHECK)->EnableWindow(TRUE);
		GetDlgItem(IDC_ENABLE_ONLYVIOLATION_CHECK)->EnableWindow(TRUE);

		if (BST_CHECKED == ((CButton*)GetDlgItem(IDC_ENABLE_END_TIME_CHECK))->GetCheck())
		{
			GetDlgItem(IDC_END_TIME)->EnableWindow(TRUE);
		}
		else
		{
			GetDlgItem(IDC_END_TIME)->EnableWindow(FALSE);
		}
	}
	else
	{
		GetDlgItem(IDC_BEGIN_TIME)->EnableWindow(FALSE);
		GetDlgItem(IDC_INDEX_EDIT)->EnableWindow(FALSE);
		GetDlgItem(IDC_ENABLE_END_TIME_CHECK)->EnableWindow(FALSE);
		GetDlgItem(IDC_END_TIME)->EnableWindow(FALSE);
		GetDlgItem(IDC_ENABLE_ONLYVIOLATION_CHECK)->EnableWindow(FALSE);
	}
}

void AutoLinkHistorySet::OnEnableEndTime()
{
	if (BST_CHECKED == ((CButton*)GetDlgItem(IDC_ENABLE_END_TIME_CHECK))->GetCheck())
	{
		GetDlgItem(IDC_END_TIME)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_END_TIME)->EnableWindow(FALSE);
	}
}
