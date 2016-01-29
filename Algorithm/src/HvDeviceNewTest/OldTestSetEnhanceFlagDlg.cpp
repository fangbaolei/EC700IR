#include "stdafx.h"
#include "OldTestSetEnhanceFlagDlg.h"

IMPLEMENT_DYNAMIC(COldTestSetEnahnceFlagDlg, CDialog)

COldTestSetEnahnceFlagDlg::COldTestSetEnahnceFlagDlg(HVAPI_HANDLE* phHandle, CWnd* pParent /* = NULL */)
: CDialog(COldTestSetEnahnceFlagDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hDevice = phHandle;
	m_pParent = (COldResultRecvTestDlg*)pParent;
}

COldTestSetEnahnceFlagDlg::~COldTestSetEnahnceFlagDlg()
{

}

void COldTestSetEnahnceFlagDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_ComBoxStyle);
}

BEGIN_MESSAGE_MAP(COldTestSetEnahnceFlagDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON8, OnBnClickedButton8)
END_MESSAGE_MAP()

BOOL COldTestSetEnahnceFlagDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	m_ComBoxStyle.AddString("不加红");
	m_ComBoxStyle.AddString("只加红违章结果");
	m_ComBoxStyle.AddString("全加红");
	m_ComBoxStyle.AddString("只加红抓拍图");

	if(m_pParent->m_iRedLightEnhanceFlag < 0 || m_pParent->m_iRedLightEnhanceFlag > 3)
	{
		m_pParent->m_iRedLightEnhanceFlag = 0;
	}

	m_ComBoxStyle.SetCurSel(m_pParent->m_iRedLightEnhanceFlag);
	char szTmp[20] ={0};
	sprintf(szTmp, "%d", m_pParent->m_iBrightness);
	SetDlgItemText(IDC_EDIT1, szTmp);
	sprintf(szTmp, "%d", m_pParent->m_iHueThreshold);
	SetDlgItemText(IDC_EDIT9, szTmp);
	sprintf(szTmp, "%d", m_pParent->m_CompressRate);
	SetDlgItemText(IDC_EDIT10, szTmp);

	return TRUE;
}

void COldTestSetEnahnceFlagDlg::OnBnClickedButton1()
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
	/*if(HVAPI_SetEnhanceRedLightFlagEx(*m_hDevice, m_ComBoxStyle.GetCurSel(), 
		iBrightness, iHueThreshold, iCompressRate) != S_OK)
	{
		MessageBox("设置失败", "ERROR", MB_OK|MB_ICONERROR);
		return;
	}
	MessageBox("设置成功");*/
	m_pParent->m_iRedLightEnhanceFlag = m_ComBoxStyle.GetCurSel();
	m_pParent->m_iBrightness = iBrightness;
	m_pParent->m_iHueThreshold = iHueThreshold;
	m_pParent->m_CompressRate, iCompressRate;
	MessageBox("设置成功");
	OnCancel();
}

void COldTestSetEnahnceFlagDlg::OnBnClickedButton8()
{
	OnCancel();
}