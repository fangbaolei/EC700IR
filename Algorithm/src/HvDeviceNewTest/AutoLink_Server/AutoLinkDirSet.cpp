// AutoLinkDirSet.cpp : implementation file
//

#include "stdafx.h"
#include "AutoLink_demo.h"
#include "AutoLinkDirSet.h"
#include "AutoLinkChangeString.h"
#include ".\autolinkdirset.h"


// AutoLinkDirSet dialog

IMPLEMENT_DYNAMIC(AutoLinkDirSet, CDialog)
AutoLinkDirSet::AutoLinkDirSet(CWnd* pParent /*=NULL*/)
	: CDialog(AutoLinkDirSet::IDD, pParent)
{
}

AutoLinkDirSet::~AutoLinkDirSet()
{
}

void AutoLinkDirSet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_DIR1, m_IllegalEdit);
	DDX_Control(pDX, IDC_EDIT_DIR2, m_NormalEdit);
}


BOOL AutoLinkDirSet::OnInitDialog()
{
	CDialog::OnInitDialog();

	InitDirDlg();

	return TRUE;
}


BEGIN_MESSAGE_MAP(AutoLinkDirSet, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_DIROK, OnBnClickedButtonDirok)
	ON_BN_CLICKED(IDC_BUTTON_DIRCANCEL, OnBnClickedButtonDircancel)
	ON_BN_CLICKED(IDC_BUTTON_DIRDEFAULT, OnBnClickedButtonDirdefault)
	ON_BN_CLICKED(IDC_BUTTON_DIRSET1, OnBnClickedButtonDirset1)
	ON_BN_CLICKED(IDC_BUTTON_DIRSET2, OnBnClickedButtonDirset2)
	ON_BN_CLICKED(IDC_BUTTON_INIT, OnBnClickedButtonInit)
END_MESSAGE_MAP()


// AutoLinkDirSet message handlers

void AutoLinkDirSet::InitDirDlg()
{
	ReadDirIni();

	m_IllegalEdit.SetWindowText(m_chIllegalDir);
	m_NormalEdit.SetWindowText(m_chNormalDir);

}

void AutoLinkDirSet::ReadDirIni()
{
	TCHAR chFileName[MAX_PATH];
	GetModuleFileName(NULL, chFileName, MAX_PATH-1);
	PathRemoveFileSpec(chFileName);
	TCHAR chIniFileName[MAX_PATH] = { 0 };
	strcpy(chIniFileName, chFileName);
	strcat(chIniFileName, "\\Config.ini");

	GetPrivateProfileString("DIR_SET", "IllegalDir", "$(DeviceIP)\\$(IllegalInfo)违法\\$(StreetName)\\$(Year)$(Month)$(Day)$(Hour)$(Minute)$(Second)\\", m_chIllegalDir, 1024, chIniFileName);
	GetPrivateProfileString("DIR_SET", "NormalDir", "$(DeviceIP)\\卡口\\$(StreetName)\\$(Year)$(Month)$(Day)\\$(Hour)\\", m_chNormalDir, 1024, chIniFileName);
	
	// 取初始化的值
	GetPrivateProfileString("ALL_SET", "Default_IllegalDir", "$(DeviceIP)\\$(IllegalInfo)违法\\$(StreetName)\\$(Year)$(Month)$(Day)$(Hour)$(Minute)$(Second)\\", m_chDefaultIllegalDir, 1024, chIniFileName);
	GetPrivateProfileString("ALL_SET", "Default_NormalDir", "$(DeviceIP)\\卡口\\$(StreetName)\\$(Year)$(Month)$(Day)\\$(Hour)\\", m_chDefaultNormalDir, 1024, chIniFileName);
}

void AutoLinkDirSet::WriteDirIni()
{
	TCHAR chFileName[MAX_PATH];
	GetModuleFileName(NULL, chFileName, MAX_PATH-1);
	PathRemoveFileSpec(chFileName);
	TCHAR chIniFileName[MAX_PATH] = { 0 };
	strcpy(chIniFileName, chFileName);
	strcat(chIniFileName, "\\Config.ini");

	WritePrivateProfileString("DIR_SET", "IllegalDir", m_chIllegalDir, chIniFileName);
	WritePrivateProfileString("DIR_SET", "NormalDir", m_chNormalDir, chIniFileName);

	WritePrivateProfileString("ALL_SET", "Default_IllegalDir", m_chDefaultIllegalDir, chIniFileName);
	WritePrivateProfileString("ALL_SET", "Default_NormalDir", m_chDefaultNormalDir, chIniFileName);
}

void AutoLinkDirSet::OnBnClickedButtonDirok()
{
	// TODO: Add your control notification handler code here
	CString szTemp;
	m_IllegalEdit.GetWindowText(szTemp);
	szTemp += _T("\\");
	szTemp.Replace("\\\\", "\\");
	if (m_Common.IsIllegalDir(szTemp))
	{
		AfxMessageBox("违法路径中包含非法字符，请重新输入！");
		return;
	}
	sprintf(m_chIllegalDir, "%s", szTemp);

	m_NormalEdit.GetWindowText(szTemp);
	szTemp += _T("\\");
	szTemp.Replace("\\\\", "\\");
	if (m_Common.IsIllegalDir(szTemp))
	{
		AfxMessageBox("正常路径中包含非法字符，请重新输入！");
		return;
	}
	sprintf(m_chNormalDir, "%s", szTemp);

	WriteDirIni();
	OnOK();
}

void AutoLinkDirSet::OnBnClickedButtonDircancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}

void AutoLinkDirSet::OnBnClickedButtonDirdefault()
{
	// TODO: Add your control notification handler code here
	InitDirDlg();
}

void AutoLinkDirSet::OnBnClickedButtonDirset1()
{
	// TODO: Add your control notification handler code here
	AutoLinkChangeString dlg;
	dlg.SetInitText(m_chIllegalDir);
	dlg.DoModal();
	char chTemp[1024] = {0};
	dlg.GetReturnText(chTemp);
	m_IllegalEdit.SetWindowText(chTemp);
}

void AutoLinkDirSet::OnBnClickedButtonDirset2()
{
	// TODO: Add your control notification handler code here
	AutoLinkChangeString dlg;
	dlg.SetInitText(m_chNormalDir);
	dlg.DoModal();
	char chTemp[1024] = {0};
	dlg.GetReturnText(chTemp);
	m_NormalEdit.SetWindowText(chTemp);
}

void AutoLinkDirSet::OnBnClickedButtonInit()
{
	// TODO: Add your control notification handler code here
	m_IllegalEdit.SetWindowText(m_chDefaultIllegalDir);
	m_NormalEdit.SetWindowText(m_chDefaultNormalDir);
}

// 屏蔽回车和ESC
BOOL AutoLinkDirSet::PreTranslateMessage(MSG* pMsg)
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