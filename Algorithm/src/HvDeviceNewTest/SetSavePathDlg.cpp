#include "stdafx.h"
#include "SetSavePathDlg.h"
#include ".\setsavepathdlg.h"
#include "shlwapi.h"

IMPLEMENT_DYNAMIC(CSetSavePathDlg, CDialog)

CSetSavePathDlg::CSetSavePathDlg(HVAPI_HANDLE* phHandle, CWnd* pParent /* = NULL */)
: CDialog(CSetSavePathDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hDevice = phHandle;
	if(pParent)
	m_pParentDlg = (CResultRecvTestDlg*)pParent;
}

CSetSavePathDlg::~CSetSavePathDlg()
{

}

void CSetSavePathDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CSetSavePathDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON3, OnBnClickedButton3)
END_MESSAGE_MAP()

BOOL CSetSavePathDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK1);
	if(m_pParentDlg->m_fIsSaveRecord == TRUE)
	pBtn->SetCheck(1);
	else
	pBtn->SetCheck(0);
	SetDlgItemText(IDC_EDIT1, m_pParentDlg->m_strSavePath);
	return TRUE;
}

void CSetSavePathDlg::OnBnClickedButton1()
{
	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK1);
	int iMode = pBtn->GetCheck();
	if(iMode == 0)
	{
		m_pParentDlg->m_fIsSaveRecord = FALSE;
		m_pParentDlg->m_strSavePath = "";
	}
	else
	{
		char szSavePath[MAX_PATH] = {0};
		GetDlgItemText(IDC_EDIT1, szSavePath, MAX_PATH);
		if(strlen(szSavePath) <= 3)
		{
			MessageBox("不正确的保存路径", "ERROR", MB_OK|MB_ICONERROR);
			return;
		}
		if(szSavePath[strlen(szSavePath)] != '\\')
		{
			szSavePath[strlen(szSavePath)] = '\\';
		}
		m_pParentDlg->m_strSavePath = szSavePath;
		m_pParentDlg->m_fIsSaveRecord = TRUE;
		CreateDirectory(szSavePath, NULL);
		CString pathtemp;
		pathtemp.Format("%s违章结果\\",szSavePath);
		CreateDirectory(pathtemp, NULL);
		pathtemp.Empty();
		pathtemp.Format("%s非违章结果\\",szSavePath);
		CreateDirectory(pathtemp, NULL);
		pathtemp.Empty();
		if(!PathFileExists(szSavePath))
		{
			CreateDirectory(szSavePath, NULL);
		}
	}
	OnCancel();
}

void CSetSavePathDlg::OnBnClickedButton3()
{
	OnCancel();
}

//////////////////////
void CSetSavePathDlg::OnOK()
{
    // 防止按回车退出
}



BOOL CSetSavePathDlg::PreTranslateMessage(MSG* pMsg) 
 {

	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
	{
		 return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
 } 


