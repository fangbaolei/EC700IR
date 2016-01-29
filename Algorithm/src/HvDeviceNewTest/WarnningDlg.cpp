#include "stdafx.h"
#include "WarnningDlg.h"
#include ".\warnningdlg.h"

IMPLEMENT_DYNAMIC(CWarnningDlg, CDialog)

CWarnningDlg::CWarnningDlg(DWORD* pdwProceMode, int iCRCValue, CWnd* pParnt /* = NULL */)
: CDialog(CWarnningDlg::IDD, pParnt)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pdwProcMode = pdwProceMode;
	m_iCRCValue = iCRCValue;
}

CWarnningDlg::~CWarnningDlg()
{

}

void CWarnningDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CWarnningDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON8, OnBnClickedButton8)
END_MESSAGE_MAP()

BOOL CWarnningDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	char szInfo[512] = {0};
	if(m_iCRCValue != -1)
	sprintf(szInfo, "原始升级文件CRC校验值已丢失，无法进行升级有效性验证，控制板自校验值为：%d,请确认切换到正常运行模式或重新进行升级",
		m_iCRCValue);
	else
	sprintf(szInfo, "原始升级文件CRC校验值已丢失，且获取控制板自校验值也失败，无法确保升级成功，建议重新进行升级");
	SetDlgItemText(IDC_STATIC, szInfo);

	return TRUE;
}

void CWarnningDlg::OnCancel()
{
	
}



void CWarnningDlg::OnBnClickedButton1()
{
	*m_pdwProcMode = 1;
	CDialog::OnCancel();
}

void CWarnningDlg::OnBnClickedButton8()
{
	*m_pdwProcMode = 2;
	CDialog::OnCancel();
}
