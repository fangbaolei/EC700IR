#include "stdafx.h"
#include "AddNewSever.h"

IMPLEMENT_DYNAMIC(CAddNewSever, CDialog)

CAddNewSever::CAddNewSever(NewSeverType* pNewServerType, BOOL fIsNew, CWnd* pParent /* = NULL */)
: CDialog(CAddNewSever::IDD, pParent)
{
	m_pNewSever = pNewServerType;
	m_fIsNew = fIsNew;
}

CAddNewSever::~CAddNewSever()
{
	m_pNewSever = NULL;
}

void CAddNewSever::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAddNewSever, CDialog)
END_MESSAGE_MAP()

BOOL CAddNewSever::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	RECT cRect;
	GetClientRect(&cRect);
	ClientToScreen((LPPOINT)&cRect);
	ClientToScreen((LPPOINT)&cRect + 1);
	cRect.left = cRect.left + 10;
	cRect.right = cRect.left + 260;
	cRect.top = cRect.top + 30;
	cRect.bottom = cRect.top + 220;
	MoveWindow(&cRect, TRUE);

	SetDlgItemText(IDC_EDIT1, m_pNewSever->szLocaName);
	SetDlgItemText(IDC_EDIT2, m_pNewSever->szServerIP);
	SetDlgItemText(IDC_EDIT3, m_pNewSever->szDataBaseName);
	SetDlgItemText(IDC_EDIT4, m_pNewSever->szUserName);
	SetDlgItemText(IDC_EDIT5, m_pNewSever->szPassWord);

	if(m_fIsNew) SetDlgItemText(IDOK, "确定添加");
	else SetDlgItemText(IDOK, "确定修改");

	return TRUE;
}

BOOL CAddNewSever::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN)
	{
		switch(pMsg->wParam)
		{
		case VK_ESCAPE:
			return TRUE;
		case VK_RETURN:
			return TRUE;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CAddNewSever::OnCancel()
{
	if(m_fIsNew)
	{
		memset(m_pNewSever->szLocaName, 0, 256);
		memset(m_pNewSever->szServerIP, 0, 30);
		memset(m_pNewSever->szDataBaseName, 0, 32);
		memset(m_pNewSever->szUserName, 0, 32);
		memset(m_pNewSever->szPassWord, 0, 16);
	}
	m_pNewSever = NULL;
	CDialog::OnCancel();
}

void CAddNewSever::OnOK()
{
	int iRetLen;
	iRetLen = GetDlgItemText(IDC_EDIT1, m_pNewSever->szLocaName, 256);
	if(iRetLen <= 0)
	{
		MessageBox("数据站名不能为空", "ERROR", MB_OK|MB_ICONERROR);
		return;
	}
	iRetLen = GetDlgItemText(IDC_EDIT2, m_pNewSever->szServerIP, 30);
	if(iRetLen <= 0)
	{
		MessageBox("服务器IP不能为空", "ERROR", MB_OK|MB_ICONERROR);
		return;
	}
	iRetLen = GetDlgItemText(IDC_EDIT3, m_pNewSever->szDataBaseName, 32);
	if(iRetLen <= 0)
	{
		MessageBox("数据库名不能为空", "ERROR", MB_OK|MB_ICONERROR);
		return;
	}
	GetDlgItemText(IDC_EDIT4, m_pNewSever->szUserName, 32);
	GetDlgItemText(IDC_EDIT5, m_pNewSever->szPassWord, 16);
	CDialog::OnOK();
}


