// AutoLinkParam.cpp : implementation file
//

#include "stdafx.h"
#include "AutoLink_demo.h"
#include "AutoLinkParam.h"
#include ".\autolinkparam.h"
#include <shlwapi.h>


HRESULT ReadConfig(char* pSN , DEV_CONFIG * pDevConfig)
{
	if (NULL == pDevConfig || NULL == pSN)
	{
		return E_FAIL;
	}
	const int MAXPATH = 260;
	TCHAR szFileName[ MAXPATH ] = {0};
	TCHAR szIniName[MAXPATH] = {0};
	TCHAR szBakName[MAXPATH] = {0};
	GetModuleFileName(NULL, szFileName, MAXPATH);	//取得包括程序名的全路径
	PathRemoveFileSpec(szFileName);				//去掉程序名
	_tcsncpy(szIniName, szFileName, MAXPATH);
	_tcsncpy(szBakName, szFileName, MAXPATH);
	_tcsncat(szIniName, _T("\\HvAutoLink.ini"), MAXPATH - 1);

	char szTemp[64];
	ZeroMemory(szTemp , sizeof(szTemp));

	SYSTEMTIME systemTime;
	GetSystemTime(& systemTime);
	char szDefaultTime[64];
	ZeroMemory(szDefaultTime , sizeof(szDefaultTime));
	sprintf(szDefaultTime , "%4d.%2d.%2d_%2d" , systemTime.wYear ,systemTime.wMonth , systemTime.wDay ,systemTime.wHour );
	CString strDefaultTime(szDefaultTime);
	strDefaultTime.Replace(' ','0');

	GetPrivateProfileString(pSN , "Enable" , "0", szTemp ,sizeof(szTemp),szIniName);
	pDevConfig->fEnableRecvHistory = atoi(szTemp);
	GetPrivateProfileString(pSN , "BeginTime" , strDefaultTime, szTemp ,sizeof(szTemp),szIniName);
	if (strlen(szTemp) < sizeof(pDevConfig->szBeginTime))
	{
		strcpy(pDevConfig->szBeginTime , szTemp);
	}
	GetPrivateProfileString(pSN , "Index" , "0", szTemp ,sizeof(szTemp),szIniName);
	pDevConfig->nIndex = atoi(szTemp);
	GetPrivateProfileString(pSN , "EndTime" , "0", szTemp ,sizeof(szTemp),szIniName);
	if (strlen(szTemp) < sizeof(pDevConfig->szEndTime))
	{
		strcpy(pDevConfig->szEndTime , szTemp);
	}

	GetPrivateProfileString(pSN , "OnlyViolation" , "0", szTemp ,sizeof(szTemp),szIniName);
	pDevConfig->fOnlyViolation = atoi(szTemp);

	return S_OK;
}
HRESULT WriteConfig(char* pSN , DEV_CONFIG * pDevConfig)
{
	if (NULL == pDevConfig || NULL == pSN)
	{
		return E_FAIL;
	}
	const int MAXPATH = 260;
	TCHAR szFileName[ MAXPATH ] = {0};
	TCHAR szIniName[MAXPATH] = {0};
	TCHAR szBakName[MAXPATH] = {0};
	GetModuleFileName(NULL, szFileName, MAXPATH);	//取得包括程序名的全路径
	PathRemoveFileSpec(szFileName);				//去掉程序名
	_tcsncpy(szIniName, szFileName, MAXPATH);
	_tcsncpy(szBakName, szFileName, MAXPATH);
	_tcsncat(szIniName, _T("\\HvAutoLink.ini"), MAXPATH - 1);

	char szIndex[32];
	ZeroMemory(szIndex , sizeof(szIndex));
	sprintf(szIndex , "%d", pDevConfig->nIndex);

	WritePrivateProfileString(pSN , "Enable" ,pDevConfig->fEnableRecvHistory?"1":"0", szIniName);
	WritePrivateProfileString(pSN , "BeginTime" ,pDevConfig->szBeginTime, szIniName);
	WritePrivateProfileString(pSN , "Index" ,szIndex, szIniName);
	WritePrivateProfileString(pSN , "EndTime" ,pDevConfig->szEndTime, szIniName);
	WritePrivateProfileString(pSN , "OnlyViolation" ,pDevConfig->fOnlyViolation?"1":"0", szIniName);

	return S_OK;
}

// AutoLinkParam dialog

IMPLEMENT_DYNAMIC(AutoLinkParam, CDialog)
AutoLinkParam::AutoLinkParam(CWnd* pParent /*=NULL*/)
	: CDialog(AutoLinkParam::IDD, pParent)
{
}

AutoLinkParam::~AutoLinkParam()
{
}

void AutoLinkParam::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BEGIN_TIME, m_beginTimeCtrl);
	DDX_Control(pDX, IDC_ENABLE_HISTORY_CHECK, m_enableRecvHistoryBut);
	DDX_Control(pDX, IDC_INDEX_EDIT, m_indexEdit);
	DDX_Control(pDX, IDC_ENABLE_END_TIME_CHECK, m_enableEndTimeBut);
	DDX_Control(pDX, IDC_END_TIME, m_endTimeCtrl);
	DDX_Control(pDX, IDC_CHECK1, m_onlyViolationBut);
}


BEGIN_MESSAGE_MAP(AutoLinkParam, CDialog)
	ON_BN_CLICKED(IDC_SAVE_BUTTON, OnBnClickedSaveButton)
	ON_BN_CLICKED(IDC_CANCEL_BUTTON, OnBnClickedCancelButton)
	ON_BN_CLICKED(IDC_ENABLE_HISTORY_CHECK, OnEnableHistory)
	ON_BN_CLICKED(IDC_ENABLE_END_TIME_CHECK, OnEnableEndTime)
END_MESSAGE_MAP()


// AutoLinkParam message handlers

void AutoLinkParam::OnBnClickedSaveButton()
{
	CString strBeginTime;
	CString strEndTime ;
	CString strIndex;

	GetDlgItem(IDC_BEGIN_TIME)->GetWindowText(strBeginTime);
	GetDlgItem(IDC_INDEX_EDIT)->GetWindowText(strIndex);
	GetDlgItem(IDC_END_TIME)->GetWindowText(strEndTime);

	m_pDevConfig->fEnableRecvHistory = ((CButton*)GetDlgItem(IDC_ENABLE_HISTORY_CHECK))->GetCheck() == BST_CHECKED;
	strcpy(m_pDevConfig->szBeginTime ,strBeginTime );
	m_pDevConfig->nIndex = atoi(strIndex);

	if (BST_CHECKED == ((CButton*)GetDlgItem(IDC_ENABLE_END_TIME_CHECK))->GetCheck())
	{
		strcpy(m_pDevConfig->szEndTime , strEndTime);
	}
	else
	{
		strcpy(m_pDevConfig->szEndTime , "0");
	}
	m_pDevConfig->fOnlyViolation = ((CButton*)GetDlgItem(IDC_ENABLE_ONLYVIOLATION_CHECK))->GetCheck() == BST_CHECKED;
	*m_fSetParam = TRUE;
	SendMessage(WM_CLOSE , 0 , 0);
}

void AutoLinkParam::OnBnClickedCancelButton()
{
	SendMessage(WM_CLOSE , 0 , 0);
	return ;
}

HRESULT AutoLinkParam::InitParamDialog(char* pSN , DEV_CONFIG* pDevConfig , bool * fSetParam)
{
	if (NULL == pSN || NULL == pDevConfig || NULL == fSetParam)
	{
		return E_POINTER;
	}
	strcpy(m_szDevSN ,pSN);
	m_pDevConfig = pDevConfig;

	*fSetParam = FALSE;
	m_fSetParam = fSetParam;

	return S_OK;
}


BOOL AutoLinkParam::OnInitDialog()
{

	//((CButton*)GetDlgItem(IDD_AUTOLINK_SETHISTORY))->SetWindowText(m_szDevSN);

	((CDateTimeCtrl*)GetDlgItem(IDC_BEGIN_TIME))->SetFormat("yyyy.MM.dd_HH");
	SYSTEMTIME sysTime;

	((CDateTimeCtrl*)GetDlgItem(IDC_BEGIN_TIME))->GetTime(&sysTime);
	sscanf(m_pDevConfig->szBeginTime , "%d.%d.%d_%d" , &sysTime.wYear ,
		&sysTime.wMonth , &sysTime.wDay ,&sysTime.wHour);
	((CDateTimeCtrl*)GetDlgItem(IDC_BEGIN_TIME))->SetTime(&sysTime);

	CString strTemp;
	strTemp.Format("%d" , m_pDevConfig->nIndex);
	GetDlgItem(IDC_INDEX_EDIT)->SetWindowText(strTemp);

	((CDateTimeCtrl*)GetDlgItem(IDC_END_TIME))->SetFormat("yyyy.MM.dd_HH");
	if (0 != strcmp( m_pDevConfig->szEndTime ,"0"))
	{
		sscanf(m_pDevConfig->szEndTime , "%d.%d.%d_%d" , &sysTime.wYear ,
			&sysTime.wMonth , &sysTime.wDay ,&sysTime.wHour);
		((CDateTimeCtrl*)GetDlgItem(IDC_END_TIME))->SetTime(&sysTime);
	}
	
	if (m_pDevConfig->fEnableRecvHistory)
	{
		((CButton*)GetDlgItem(IDC_ENABLE_HISTORY_CHECK))->SetCheck(BST_CHECKED);
		GetDlgItem(IDC_BEGIN_TIME)->EnableWindow(TRUE);
		GetDlgItem(IDC_INDEX_EDIT)->EnableWindow(TRUE);
		GetDlgItem(IDC_ENABLE_END_TIME_CHECK)->EnableWindow(TRUE);
		GetDlgItem(IDC_ENABLE_ONLYVIOLATION_CHECK)->EnableWindow(TRUE);
		
		if (0 == strcmp(m_pDevConfig->szEndTime , "0"))
		{
			((CButton*)GetDlgItem(IDC_ENABLE_END_TIME_CHECK))->SetCheck(BST_UNCHECKED);
			GetDlgItem(IDC_END_TIME)->EnableWindow(FALSE);
		}
		else
		{
			((CButton*)GetDlgItem(IDC_ENABLE_END_TIME_CHECK))->SetCheck(BST_CHECKED);
			GetDlgItem(IDC_END_TIME)->EnableWindow(TRUE);
		}
	}
	else
	{
		((CButton*)GetDlgItem(IDC_ENABLE_HISTORY_CHECK))->SetCheck(BST_UNCHECKED);
		GetDlgItem(IDC_BEGIN_TIME)->EnableWindow(FALSE);
		GetDlgItem(IDC_INDEX_EDIT)->EnableWindow(FALSE);
		GetDlgItem(IDC_ENABLE_END_TIME_CHECK)->EnableWindow(FALSE);
		GetDlgItem(IDC_END_TIME)->EnableWindow(FALSE);
		GetDlgItem(IDC_ENABLE_ONLYVIOLATION_CHECK)->EnableWindow(FALSE);
		
		if (0 == strcmp(m_pDevConfig->szEndTime , "0"))
		{
			((CButton*)GetDlgItem(IDC_ENABLE_END_TIME_CHECK))->SetCheck(BST_UNCHECKED);
		}
		else
		{
			((CButton*)GetDlgItem(IDC_ENABLE_END_TIME_CHECK))->SetCheck(BST_CHECKED);
		}

	}

	if (m_pDevConfig->fOnlyViolation)
	{
		((CButton*)GetDlgItem(IDC_ENABLE_ONLYVIOLATION_CHECK))->SetCheck(BST_CHECKED);
	}
	else
	{
		((CButton*)GetDlgItem(IDC_ENABLE_ONLYVIOLATION_CHECK))->SetCheck(BST_UNCHECKED);
	}

	return TRUE;
}

void AutoLinkParam::OnEnableHistory()
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

void AutoLinkParam::OnEnableEndTime()
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
