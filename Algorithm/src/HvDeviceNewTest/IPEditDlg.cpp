#include "stdafx.h"
#include "IPEditDlg.h"
#include ".\ipeditdlg.h"

IMPLEMENT_DYNAMIC(CIPEditDlg, CDialog)

CIPEditDlg::CIPEditDlg(IPInfo* pIpInfp, CWnd* pParent /* = NULL */)
:CDialog(CIPEditDlg::IDD, pParent)
{
	m_pIPInfo = pIpInfp;
}

CIPEditDlg::~CIPEditDlg()
{
	m_pIPInfo = NULL;
}

void CIPEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IPADDRESS1, m_IP);
	DDX_Control(pDX, IDC_IPADDRESS2, m_Mask);
	DDX_Control(pDX, IDC_IPADDRESS3, m_GateWay);
}

BEGIN_MESSAGE_MAP(CIPEditDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
END_MESSAGE_MAP()

BOOL CIPEditDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	if(m_pIPInfo == NULL)
	{
		OnCancel();
		return FALSE;
	}

	PBYTE pByte=(PBYTE)&m_pIPInfo->dw64Mac;
	char szMacInfo[128] = {0};
	sprintf(szMacInfo, "%02X-%02X-%02X-%02X-%02X-%02X", pByte[0],pByte[1],pByte[2],pByte[3],pByte[4],pByte[5]);

	SetDlgItemText(IDC_EDIT2, szMacInfo);
	pByte = (PBYTE)&m_pIPInfo->dwIP;
	m_IP.SetAddress(pByte[3], pByte[2], pByte[1], pByte[0]);
	pByte = (PBYTE)&m_pIPInfo->dwMask;
	m_Mask.SetAddress(pByte[3], pByte[2], pByte[1], pByte[0]);
	pByte = (PBYTE)&m_pIPInfo->dwGateWay;
	m_GateWay.SetAddress(pByte[3], pByte[2], pByte[1], pByte[0]);

	return TRUE;
}

void CIPEditDlg::OnBnClickedButton1()
{
	DWORD dwIP, dwMask, dwGateWay;
	m_IP.GetAddress(dwIP);
	m_Mask.GetAddress(dwMask);
	m_GateWay.GetAddress(dwGateWay);
    if(dwIP == 0)
	{
		MessageBox("IP´íÎó£¡");
		return ;
	}
	m_pIPInfo->dwIP = dwIP;
	m_pIPInfo->dwMask = dwMask;
	m_pIPInfo->dwGateWay = dwGateWay;

	OnOK();
}

void CIPEditDlg::OnBnClickedButton2()
{
	OnCancel();
}
