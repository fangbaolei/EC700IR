#include "stdafx.h"
#include "SetVideoEnhanceFlagDlg.h"
#include ".\setvideoenhanceflagdlg.h"

IMPLEMENT_DYNAMIC(CSetVideoEnahnceFlagDlg, CDialog)

CSetVideoEnahnceFlagDlg::CSetVideoEnahnceFlagDlg(HVAPI_HANDLE_EX* phHandle, CWnd* pParent /* = NULL */)
: CDialog(CSetVideoEnahnceFlagDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hDevice = phHandle;
}

CSetVideoEnahnceFlagDlg::~CSetVideoEnahnceFlagDlg()
{

}

void CSetVideoEnahnceFlagDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CSetVideoEnahnceFlagDlg, CDialog)
	
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON8, OnBnClickedButton8)
END_MESSAGE_MAP()

BOOL CSetVideoEnahnceFlagDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK1);
	pBtn->SetCheck(0);

	SetDlgItemText(IDC_EDIT1, "0");
	SetDlgItemText(IDC_EDIT9, "128");
	SetDlgItemText(IDC_EDIT10, "80");

	return TRUE;
}

void CSetVideoEnahnceFlagDlg::OnBnClickedButton1()
{
	if(*m_hDevice == NULL)
	{
		MessageBox("未连接到识别器，设置失败", "ERROR", MB_OK|MB_ICONERROR);
		OnCancel();
		return;
	}
	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK1);
	int iMode = pBtn->GetCheck();
	char szTemp[20] = {0};
	int iBrightness = 0;
	GetDlgItemText(IDC_EDIT1, szTemp, 20);
	iBrightness = atoi(szTemp);
	int iHueThreshold = 0;
	GetDlgItemText(IDC_EDIT9, szTemp, 20);
	iHueThreshold = atoi(szTemp);
	int iCompressRate = 0;
	GetDlgItemText(IDC_EDIT10, szTemp, 20);
	iCompressRate = atoi(szTemp);
	if(HVAPI_SetHistoryVideoEnhanceRedLightFlagEx(*m_hDevice, iMode, 
		iBrightness, iHueThreshold, iCompressRate) != S_OK)
	{
		MessageBox("设置失败", "ERROR", MB_OK|MB_ICONERROR);
		return;
	}
	MessageBox("设置成功");
	OnCancel();
}

void CSetVideoEnahnceFlagDlg::OnBnClickedButton8()
{
	OnCancel();
}
