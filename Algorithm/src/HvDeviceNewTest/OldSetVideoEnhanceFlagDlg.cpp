#include "stdafx.h"
#include "OldSetVideoEnhanceFlagDlg.h"

IMPLEMENT_DYNAMIC(COldSetVideoEnahnceFlagDlg, CDialog)

COldSetVideoEnahnceFlagDlg::COldSetVideoEnahnceFlagDlg(CWnd* pParent /* = NULL */)
: CDialog(COldSetVideoEnahnceFlagDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pVideoDlg = (COldVideoRecvTestDlg*)pParent;
}

COldSetVideoEnahnceFlagDlg::~COldSetVideoEnahnceFlagDlg()
{

}

void COldSetVideoEnahnceFlagDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(COldSetVideoEnahnceFlagDlg, CDialog)

	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON8, OnBnClickedButton8)
END_MESSAGE_MAP()

BOOL COldSetVideoEnahnceFlagDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK1);
	if(m_pVideoDlg->m_iRedLightEnhanceFlag != 0 &&
		m_pVideoDlg->m_iRedLightEnhanceFlag != 1)
	m_pVideoDlg->m_iRedLightEnhanceFlag = 0;
	pBtn->SetCheck(m_pVideoDlg->m_iRedLightEnhanceFlag);

	char szTmpInfo[20] = {0};
	sprintf(szTmpInfo, "%d", m_pVideoDlg->m_iBrightness);
	SetDlgItemText(IDC_EDIT1, szTmpInfo);
	sprintf(szTmpInfo, "%d", m_pVideoDlg->m_iHueThreshold);
	SetDlgItemText(IDC_EDIT9, szTmpInfo);
	sprintf(szTmpInfo, "%d", m_pVideoDlg->m_CompressRate);
	SetDlgItemText(IDC_EDIT10, szTmpInfo);

	return TRUE;
}

void COldSetVideoEnahnceFlagDlg::OnBnClickedButton1()
{
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
	m_pVideoDlg->m_iRedLightEnhanceFlag = iMode;
	m_pVideoDlg->m_iBrightness = iBrightness;
	m_pVideoDlg->m_iHueThreshold = iHueThreshold;
	m_pVideoDlg->m_CompressRate = iCompressRate;
	MessageBox("…Ë÷√≥…π¶");
	OnCancel();
}

void COldSetVideoEnahnceFlagDlg::OnBnClickedButton8()
{
	OnCancel();
}
