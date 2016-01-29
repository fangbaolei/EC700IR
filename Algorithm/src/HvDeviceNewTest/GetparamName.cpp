// GetparamName.cpp : implementation file
//

#include "stdafx.h"
#include "HvDeviceNewTest.h"
#include "GetparamName.h"
#include ".\getparamname.h"


// CGetparamName dialog

IMPLEMENT_DYNAMIC(CGetparamName, CDialog)
CGetparamName::CGetparamName(CWnd* pParent /*=NULL*/)
	: CDialog(CGetparamName::IDD, pParent)
	, m_paramname(_T(""))
{
}

CGetparamName::~CGetparamName()
{
}

void CGetparamName::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_paramname);
}


BEGIN_MESSAGE_MAP(CGetparamName, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CGetparamName message handlers

void CGetparamName::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	UpdateData(true); 
	OnOK();
}
