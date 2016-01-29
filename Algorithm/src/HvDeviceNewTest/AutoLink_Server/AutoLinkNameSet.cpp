// AutoLinkNameSet.cpp : implementation file
//

#include "stdafx.h"
#include "AutoLink_demo.h"
#include "AutoLinkNameSet.h"
#include "AutoLinkChangeString.h"
#include ".\autolinknameset.h"

// AutoLinkNameSet dialog

IMPLEMENT_DYNAMIC(AutoLinkNameSet, CDialog)
AutoLinkNameSet::AutoLinkNameSet(CWnd* pParent /*=NULL*/)
	: CDialog(AutoLinkNameSet::IDD, pParent)
{
}

AutoLinkNameSet::~AutoLinkNameSet()
{
}

void AutoLinkNameSet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_BIGIMG, m_BigImgButton);
	DDX_Control(pDX, IDC_CHECK_SMALLIMG, m_SmallImgButton);
	DDX_Control(pDX, IDC_CHECK_BIN, m_BinButton);
	DDX_Control(pDX, IDC_CHECK_INFO, m_InfoButton);
	DDX_Control(pDX, IDC_CHECK_PLATE, m_PlateButton);
	DDX_Control(pDX, IDC_CHECK_PLATENO, m_PlateNoButton);
	DDX_Control(pDX, IDC_EDIT_NAME1, m_BigImgEdit);
	DDX_Control(pDX, IDC_EDIT_NAME2, m_SmallImgEdit);
	DDX_Control(pDX, IDC_EDIT_NAME3, m_BinEdit);
	DDX_Control(pDX, IDC_EDIT_NAME4, m_InfoEdit);
	DDX_Control(pDX, IDC_EDIT_NAME5, m_PlateEdit);
	DDX_Control(pDX, IDC_EDIT_NAME6, m_PlateNoEdit);
}


BEGIN_MESSAGE_MAP(AutoLinkNameSet, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_NAMEOK, OnBnClickedButtonNameok)
	ON_BN_CLICKED(IDC_BUTTON_NAMECANCEL, OnBnClickedButtonNamecancel)
	ON_BN_CLICKED(IDC_BUTTON_NAMEDEFAULT, OnBnClickedButtonNamedefault)
	ON_BN_CLICKED(IDC_BUTTON_NAMESET1, OnBnClickedButtonNameset1)
	ON_BN_CLICKED(IDC_BUTTON_NAMESET2, OnBnClickedButtonNameset2)
	ON_BN_CLICKED(IDC_BUTTON_NAMESET3, OnBnClickedButtonNameset3)
	ON_BN_CLICKED(IDC_BUTTON_NAMESET4, OnBnClickedButtonNameset4)
	ON_BN_CLICKED(IDC_BUTTON_NAMESET5, OnBnClickedButtonNameset5)
	ON_BN_CLICKED(IDC_BUTTON_NAMEINIT, OnBnClickedButtonNameinit)
END_MESSAGE_MAP()

BOOL AutoLinkNameSet::OnInitDialog()
{
	CDialog::OnInitDialog();

	InitNameSet();

	return TRUE;
}
// AutoLinkNameSet message handlers

// 初始化
void AutoLinkNameSet::InitNameSet()
{
	ReadFileNameIni();

	if (0 == m_FileNameInfo.iBigImgEnable)
		m_BigImgButton.SetCheck(FALSE);
	else
		m_BigImgButton.SetCheck(TRUE);

	if (0 == m_FileNameInfo.iSmallImgEnable)
		m_SmallImgButton.SetCheck(FALSE);
	else
		m_SmallImgButton.SetCheck(TRUE);

	if (0 == m_FileNameInfo.iBinEnable)
		m_BinButton.SetCheck(FALSE);
	else
		m_BinButton.SetCheck(TRUE);

	if (0 == m_FileNameInfo.iInfoEnable)
		m_InfoButton.SetCheck(FALSE);
	else
		m_InfoButton.SetCheck(TRUE);

	if (0 == m_FileNameInfo.iPlateEnable)
		m_PlateButton.SetCheck(FALSE);
	else
		m_PlateButton.SetCheck(TRUE);

	if (0 == m_FileNameInfo.iPlateNoEnable)
		m_PlateNoButton.SetCheck(FALSE);
	else
		m_PlateNoButton.SetCheck(TRUE);

	m_BigImgEdit.SetWindowText(m_FileNameInfo.chBigImgFileName);
	m_SmallImgEdit.SetWindowText(m_FileNameInfo.chSmallImgFileName);
	m_BinEdit.SetWindowText(m_FileNameInfo.chBinFileName);
	m_InfoEdit.SetWindowText(m_FileNameInfo.chInfoFileName);
	m_PlateEdit.SetWindowText(m_FileNameInfo.chPlateFileName);
	m_PlateNoEdit.SetWindowText(m_FileNameInfo.chPlateNoFileName);
}

void AutoLinkNameSet::ReadFileNameIni()
{
	TCHAR chFileName[MAX_PATH];
	GetModuleFileName(NULL, chFileName, MAX_PATH-1);
	PathRemoveFileSpec(chFileName);
	TCHAR chIniFileName[MAX_PATH] = { 0 };
	strcpy(chIniFileName, chFileName);
	strcat(chIniFileName, "\\Config.ini");

	m_FileNameInfo.iBigImgEnable = GetPrivateProfileInt("FILENAME_SET", "BigImgEnable", 1, chIniFileName);
	m_FileNameInfo.iSmallImgEnable = GetPrivateProfileInt("FILENAME_SET", "SmallImgEnable", 1, chIniFileName);
	m_FileNameInfo.iBinEnable = GetPrivateProfileInt("FILENAME_SET", "BinEnable", 1, chIniFileName);
	m_FileNameInfo.iInfoEnable = GetPrivateProfileInt("FILENAME_SET", "InfoEnable", 1, chIniFileName);
	m_FileNameInfo.iPlateEnable = GetPrivateProfileInt("FILENAME_SET", "PlateEnable", 1, chIniFileName);
	m_FileNameInfo.iPlateNoEnable = GetPrivateProfileInt("FILENAME_SET", "PlateNoEnable", 0, chIniFileName);

	GetPrivateProfileString("FILENAME_SET", "BigImgFileName", "$(Year)$(Month)$(Day)$(Hour)$(Minute)$(Second)$(MilliSecond)_$(BigImageID_Letter).jpg", m_FileNameInfo.chBigImgFileName, 1024, chIniFileName);
	GetPrivateProfileString("FILENAME_SET", "SmallImgName", "$(Year)$(Month)$(Day)$(Hour)$(Minute)$(Second)$(MilliSecond).bmp", m_FileNameInfo.chSmallImgFileName, 1024, chIniFileName);
	GetPrivateProfileString("FILENAME_SET", "BinFileName", "$(Year)$(Month)$(Day)$(Hour)$(Minute)$(Second)$(MilliSecond).bin", m_FileNameInfo.chBinFileName, 1024, chIniFileName);
	GetPrivateProfileString("FILENAME_SET", "InfoFileName", "$(Year)$(Month)$(Day)$(Hour)$(Minute)$(Second)$(MilliSecond).inf", m_FileNameInfo.chInfoFileName, 1024, chIniFileName);
	GetPrivateProfileString("FILENAME_SET", "PlateFileName", "$(Year)$(Month)$(Day)$(Hour)$(Minute)$(Second)$(MilliSecond).txt", m_FileNameInfo.chPlateFileName, 1024, chIniFileName);
	GetPrivateProfileString("FILENAME_SET", "PlateNoFileName", "PlateNo.txt", m_FileNameInfo.chPlateNoFileName, 1024, chIniFileName);

	m_DefaultFileNameInfo.iBigImgEnable = GetPrivateProfileInt("ALL_SET", "Default_BigImgEnable", 1, chIniFileName);
	m_DefaultFileNameInfo.iSmallImgEnable = GetPrivateProfileInt("ALL_SET", "Default_SmallImgEnable", 1, chIniFileName);
	m_DefaultFileNameInfo.iBinEnable = GetPrivateProfileInt("ALL_SET", "Default_BinEnable", 1, chIniFileName);
	m_DefaultFileNameInfo.iInfoEnable = GetPrivateProfileInt("ALL_SET", "Default_InfoEnable", 1, chIniFileName);
	m_DefaultFileNameInfo.iPlateEnable = GetPrivateProfileInt("ALL_SET", "Default_PlateEnable", 1, chIniFileName);
	m_DefaultFileNameInfo.iPlateNoEnable = GetPrivateProfileInt("ALL_SET", "Default_PlateNoEnable", 0, chIniFileName);

	GetPrivateProfileString("ALL_SET", "Default_BigImgFileName", "$(Year)$(Month)$(Day)$(Hour)$(Minute)$(Second)$(MilliSecond)_$(BigImageID_Letter).jpg", m_DefaultFileNameInfo.chBigImgFileName, 1024, chIniFileName);
	GetPrivateProfileString("ALL_SET", "Default_SmallImgName", "$(Year)$(Month)$(Day)$(Hour)$(Minute)$(Second)$(MilliSecond).bmp", m_DefaultFileNameInfo.chSmallImgFileName, 1024, chIniFileName);
	GetPrivateProfileString("ALL_SET", "Default_BinFileName", "$(Year)$(Month)$(Day)$(Hour)$(Minute)$(Second)$(MilliSecond).bin", m_DefaultFileNameInfo.chBinFileName, 1024, chIniFileName);
	GetPrivateProfileString("ALL_SET", "Default_InfoFileName", "$(Year)$(Month)$(Day)$(Hour)$(Minute)$(Second)$(MilliSecond).inf", m_DefaultFileNameInfo.chInfoFileName, 1024, chIniFileName);
	GetPrivateProfileString("ALL_SET", "Default_PlateFileName", "$(Year)$(Month)$(Day)$(Hour)$(Minute)$(Second)$(MilliSecond).txt", m_DefaultFileNameInfo.chPlateFileName, 1024, chIniFileName);
	GetPrivateProfileString("ALL_SET", "Default_PlateNoFileName", "PlateNo.txt", m_DefaultFileNameInfo.chPlateNoFileName, 1024, chIniFileName);
}

void AutoLinkNameSet::WriteFileNameIni()
{
	TCHAR chFileName[MAX_PATH];
	GetModuleFileName(NULL, chFileName, MAX_PATH-1);
	PathRemoveFileSpec(chFileName);
	TCHAR chIniFileName[MAX_PATH] = { 0 };
	strcpy(chIniFileName, chFileName);
	strcat(chIniFileName, "\\Config.ini");

	char chTemp[32] = {0};

	sprintf(chTemp, "%d", m_FileNameInfo.iBigImgEnable);
	WritePrivateProfileString("FILENAME_SET", "BigImgEnable", chTemp, chIniFileName);
	sprintf(chTemp, "%d", m_FileNameInfo.iSmallImgEnable);
	WritePrivateProfileString("FILENAME_SET", "SmallImgEnable", chTemp, chIniFileName);
	sprintf(chTemp, "%d", m_FileNameInfo.iBinEnable);
	WritePrivateProfileString("FILENAME_SET", "BinEnable", chTemp, chIniFileName);
	sprintf(chTemp, "%d", m_FileNameInfo.iInfoEnable);
	WritePrivateProfileString("FILENAME_SET", "InfoEnable", chTemp, chIniFileName);
	sprintf(chTemp, "%d", m_FileNameInfo.iPlateEnable);
	WritePrivateProfileString("FILENAME_SET", "PlateEnable", chTemp, chIniFileName);
	sprintf(chTemp, "%d", m_FileNameInfo.iPlateNoEnable);
	WritePrivateProfileString("FILENAME_SET", "PlateNoEnable", chTemp, chIniFileName);

	WritePrivateProfileString("FILENAME_SET", "BigImgFileName", m_FileNameInfo.chBigImgFileName, chIniFileName);
	WritePrivateProfileString("FILENAME_SET", "SmallImgName", m_FileNameInfo.chSmallImgFileName, chIniFileName);
	WritePrivateProfileString("FILENAME_SET", "BinFileName", m_FileNameInfo.chBinFileName, chIniFileName);
	WritePrivateProfileString("FILENAME_SET", "InfoFileName", m_FileNameInfo.chInfoFileName, chIniFileName);
	WritePrivateProfileString("FILENAME_SET", "PlateFileName", m_FileNameInfo.chPlateFileName, chIniFileName);
	WritePrivateProfileString("FILENAME_SET", "PlateNoFileName", m_FileNameInfo.chPlateNoFileName, chIniFileName);

	sprintf(chTemp, "%d", m_DefaultFileNameInfo.iBigImgEnable);
	WritePrivateProfileString("ALL_SET", "Default_BigImgEnable", chTemp, chIniFileName);
	sprintf(chTemp, "%d", m_DefaultFileNameInfo.iSmallImgEnable);
	WritePrivateProfileString("ALL_SET", "Default_SmallImgEnable", chTemp, chIniFileName);
	sprintf(chTemp, "%d", m_DefaultFileNameInfo.iBinEnable);
	WritePrivateProfileString("ALL_SET", "Default_BinEnable", chTemp, chIniFileName);
	sprintf(chTemp, "%d", m_DefaultFileNameInfo.iInfoEnable);
	WritePrivateProfileString("ALL_SET", "Default_InfoEnable", chTemp, chIniFileName);
	sprintf(chTemp, "%d", m_DefaultFileNameInfo.iPlateEnable);
	WritePrivateProfileString("ALL_SET", "Default_PlateEnable", chTemp, chIniFileName);
	sprintf(chTemp, "%d", m_DefaultFileNameInfo.iPlateNoEnable);
	WritePrivateProfileString("ALL_SET", "Default_PlateNoEnable", chTemp, chIniFileName);

	WritePrivateProfileString("ALL_SET", "Default_BigImgFileName", m_DefaultFileNameInfo.chBigImgFileName, chIniFileName);
	WritePrivateProfileString("ALL_SET", "Default_SmallImgName", m_DefaultFileNameInfo.chSmallImgFileName, chIniFileName);
	WritePrivateProfileString("ALL_SET", "Default_BinFileName", m_DefaultFileNameInfo.chBinFileName, chIniFileName);
	WritePrivateProfileString("ALL_SET", "Default_InfoFileName", m_DefaultFileNameInfo.chInfoFileName, chIniFileName);
	WritePrivateProfileString("ALL_SET", "Default_PlateFileName", m_DefaultFileNameInfo.chPlateFileName, chIniFileName);
	WritePrivateProfileString("ALL_SET", "Default_PlateNoFileName", m_DefaultFileNameInfo.chPlateNoFileName, chIniFileName);
}

void AutoLinkNameSet::OnBnClickedButtonNameok()
{
	// TODO: Add your control notification handler code here
	if (BST_CHECKED == m_BigImgButton.GetCheck())
		m_FileNameInfo.iBigImgEnable = 1;
	else if (BST_UNCHECKED == m_BigImgButton.GetCheck())
		m_FileNameInfo.iBigImgEnable = 0;

	if (BST_CHECKED == m_SmallImgButton.GetCheck())
		m_FileNameInfo.iSmallImgEnable = 1;
	else if (BST_UNCHECKED == m_SmallImgButton.GetCheck())
		m_FileNameInfo.iSmallImgEnable = 0;

	if (BST_CHECKED == m_BinButton.GetCheck())
		m_FileNameInfo.iBinEnable = 1;
	else if (BST_UNCHECKED == m_BinButton.GetCheck())
		m_FileNameInfo.iBinEnable = 0;

	if (BST_CHECKED == m_InfoButton.GetCheck())
		m_FileNameInfo.iInfoEnable = 1;
	else if (BST_UNCHECKED == m_InfoButton.GetCheck())
		m_FileNameInfo.iInfoEnable = 0;

	if (BST_CHECKED == m_PlateButton.GetCheck())
		m_FileNameInfo.iPlateEnable = 1;
	else if (BST_UNCHECKED == m_PlateButton.GetCheck())
		m_FileNameInfo.iPlateEnable = 0;
	
	if (BST_CHECKED == m_PlateNoButton.GetCheck())
		m_FileNameInfo.iPlateNoEnable = 1;
	else if (BST_UNCHECKED == m_PlateNoButton.GetCheck())
		m_FileNameInfo.iPlateNoEnable = 0;

	CString szTemp;
	m_BigImgEdit.GetWindowText(szTemp);
	if (m_Common.IsIllegalFileName(szTemp))
	{
		AfxMessageBox("大图文件名包含非法字符，请重新输入！");
		return;
	}
	sprintf(m_FileNameInfo.chBigImgFileName, "%s", szTemp);

	m_SmallImgEdit.GetWindowText(szTemp);
	if (m_Common.IsIllegalFileName(szTemp))
	{
		AfxMessageBox("小图文件名包含非法字符，请重新输入！");
		return;
	}
	sprintf(m_FileNameInfo.chSmallImgFileName, "%s", szTemp);

	m_BinEdit.GetWindowText(szTemp);
	if (m_Common.IsIllegalFileName(szTemp))
	{
		AfxMessageBox("二值图文件名包含非法字符，请重新输入！");
		return;
	}
	sprintf(m_FileNameInfo.chBinFileName, "%s", szTemp);

	m_InfoEdit.GetWindowText(szTemp);
	if (m_Common.IsIllegalFileName(szTemp))
	{
		AfxMessageBox("附加信息文件名包含非法字符，请重新输入！");
		return;
	}
	sprintf(m_FileNameInfo.chInfoFileName, "%s", szTemp);

	m_PlateEdit.GetWindowText(szTemp);
	if (m_Common.IsIllegalFileName(szTemp))
	{
		AfxMessageBox("车牌信息文件名包含非法字符，请重新输入！");
		return;
	}
	sprintf(m_FileNameInfo.chPlateFileName, "%s", szTemp);

	m_PlateNoEdit.GetWindowText(szTemp);
	if (m_Common.IsIllegalFileName(szTemp))
	{
		AfxMessageBox("车牌文件名包含非法字符，请重新输入！");
		return;
	}
	sprintf(m_FileNameInfo.chPlateNoFileName, "%s", szTemp);

	WriteFileNameIni();
	
	OnOK();
}

void AutoLinkNameSet::OnBnClickedButtonNamecancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}

void AutoLinkNameSet::OnBnClickedButtonNamedefault()
{
	// TODO: Add your control notification handler code here
	InitNameSet();
}

void AutoLinkNameSet::OnBnClickedButtonNameset1()
{
	// TODO: Add your control notification handler code here
	AutoLinkChangeString dlg;
	dlg.SetInitText(m_FileNameInfo.chBigImgFileName);
	dlg.DoModal();
	char chTemp[1024] = {0};
	dlg.GetReturnText(chTemp);
	m_BigImgEdit.SetWindowText(chTemp);
}

void AutoLinkNameSet::OnBnClickedButtonNameset2()
{
	// TODO: Add your control notification handler code here
	AutoLinkChangeString dlg;
	dlg.SetInitText(m_FileNameInfo.chSmallImgFileName);
	dlg.DoModal();
	char chTemp[1024] = {0};
	dlg.GetReturnText(chTemp);
	m_SmallImgEdit.SetWindowText(chTemp);
}

void AutoLinkNameSet::OnBnClickedButtonNameset3()
{
	// TODO: Add your control notification handler code here
	AutoLinkChangeString dlg;
	dlg.SetInitText(m_FileNameInfo.chBinFileName);
	dlg.DoModal();
	char chTemp[1024] = {0};
	dlg.GetReturnText(chTemp);
	m_BinEdit.SetWindowText(chTemp);
}

void AutoLinkNameSet::OnBnClickedButtonNameset4()
{
	// TODO: Add your control notification handler code here
	AutoLinkChangeString dlg;
	dlg.SetInitText(m_FileNameInfo.chInfoFileName);
	dlg.DoModal();
	char chTemp[1024] = {0};
	dlg.GetReturnText(chTemp);
	m_InfoEdit.SetWindowText(chTemp);
}

void AutoLinkNameSet::OnBnClickedButtonNameset5()
{
	// TODO: Add your control notification handler code here
	AutoLinkChangeString dlg;
	dlg.SetInitText(m_FileNameInfo.chPlateFileName);
	dlg.DoModal();
	char chTemp[1024] = {0};
	dlg.GetReturnText(chTemp);
	m_PlateEdit.SetWindowText(chTemp);
}

void AutoLinkNameSet::OnBnClickedButtonNameinit()
{
	// TODO: Add your control notification handler code here
		if (0 == m_DefaultFileNameInfo.iBigImgEnable)
		m_BigImgButton.SetCheck(FALSE);
	else
		m_BigImgButton.SetCheck(TRUE);

	if (0 == m_DefaultFileNameInfo.iSmallImgEnable)
		m_SmallImgButton.SetCheck(FALSE);
	else
		m_SmallImgButton.SetCheck(TRUE);

	if (0 == m_DefaultFileNameInfo.iBinEnable)
		m_BinButton.SetCheck(FALSE);
	else
		m_BinButton.SetCheck(TRUE);

	if (0 == m_DefaultFileNameInfo.iInfoEnable)
		m_InfoButton.SetCheck(FALSE);
	else
		m_InfoButton.SetCheck(TRUE);

	if (0 == m_DefaultFileNameInfo.iPlateEnable)
		m_PlateButton.SetCheck(FALSE);
	else
		m_PlateButton.SetCheck(TRUE);

	if (0 == m_DefaultFileNameInfo.iPlateNoEnable)
		m_PlateNoButton.SetCheck(FALSE);
	else
		m_PlateNoButton.SetCheck(TRUE);

	m_BigImgEdit.SetWindowText(m_DefaultFileNameInfo.chBigImgFileName);
	m_SmallImgEdit.SetWindowText(m_DefaultFileNameInfo.chSmallImgFileName);
	m_BinEdit.SetWindowText(m_DefaultFileNameInfo.chBinFileName);
	m_InfoEdit.SetWindowText(m_DefaultFileNameInfo.chInfoFileName);
	m_PlateEdit.SetWindowText(m_DefaultFileNameInfo.chPlateFileName);
	m_PlateNoEdit.SetWindowText(m_DefaultFileNameInfo.chPlateNoFileName);
}

// 屏蔽回车和ESC
BOOL AutoLinkNameSet::PreTranslateMessage(MSG* pMsg)
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
