#include "stdafx.h"
#include "SetEnhanceFlagDlg.h"

IMPLEMENT_DYNAMIC(CSetEnahnceFlagDlg, CDialog)

CSetEnahnceFlagDlg::CSetEnahnceFlagDlg(HVAPI_HANDLE_EX* phHandle, CWnd* pParent /* = NULL */)
: CDialog(CSetEnahnceFlagDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hDevice = phHandle;
}

CSetEnahnceFlagDlg::~CSetEnahnceFlagDlg()
{

}

void CSetEnahnceFlagDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_ComBoxStyle);
}

BEGIN_MESSAGE_MAP(CSetEnahnceFlagDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON8, OnBnClickedButton8)
END_MESSAGE_MAP()

BOOL CSetEnahnceFlagDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	m_ComBoxStyle.AddString("不加红");
	m_ComBoxStyle.AddString("只加红违章结果");
	m_ComBoxStyle.AddString("全加红");
	m_ComBoxStyle.AddString("只加红抓拍图");
	m_ComBoxStyle.AddString("只加红违章抓拍图");

	m_ComBoxStyle.SetCurSel(0);

	SetDlgItemText(IDC_EDIT1, "0");
	SetDlgItemText(IDC_EDIT9, "128");
	SetDlgItemText(IDC_EDIT10, "80");

	return TRUE;
}

void CSetEnahnceFlagDlg::OnBnClickedButton1()
{
	if(m_hDevice == NULL)
	{
		MessageBox("未连接到识别器，设置失败", "ERROR", MB_OK|MB_ICONERROR);
		OnCancel();
	}
	
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
	if(HVAPI_SetEnhanceRedLightFlagEx(*m_hDevice, m_ComBoxStyle.GetCurSel(), 
		iBrightness, iHueThreshold, iCompressRate) != S_OK)
	{
		MessageBox("设置失败", "ERROR", MB_OK|MB_ICONERROR);
		return;
	}
	MessageBox("设置成功");
	OnCancel();
}

void CSetEnahnceFlagDlg::OnBnClickedButton8()
{
	OnCancel();
}