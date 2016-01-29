#include "stdafx.h"
#include "OldSetGetHistoryRecordDlg.h"

IMPLEMENT_DYNAMIC(COldSetGetHistoryDlg, CDialog)

COldSetGetHistoryDlg::COldSetGetHistoryDlg(COldResultRecvTestDlg* pParent)
: CDialog(COldSetGetHistoryDlg::IDD)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pParent = pParent;
}

COldSetGetHistoryDlg::~COldSetGetHistoryDlg()
{

}

void COldSetGetHistoryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(COldSetGetHistoryDlg, CDialog)

	ON_BN_CLICKED(IDC_CHECK1, OnBnClickedCheck1)
	ON_BN_CLICKED(IDC_BUTTON3, OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
END_MESSAGE_MAP()

BOOL COldSetGetHistoryDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	CTime cTime = CTime::GetCurrentTime();
	char szTmp[20];
	int iTmp = cTime.GetYear();
	sprintf(szTmp, "%d", iTmp);
	SetDlgItemText(IDC_EDIT1, szTmp);

	iTmp = cTime.GetMonth();
	sprintf(szTmp, "%d", iTmp);
	SetDlgItemText(IDC_EDIT10, szTmp);

	iTmp = cTime.GetDay();
	sprintf(szTmp, "%d", iTmp);
	SetDlgItemText(IDC_EDIT11, szTmp);

	iTmp = cTime.GetHour();
	sprintf(szTmp, "%d", iTmp);
	SetDlgItemText(IDC_EDIT12, szTmp);

	sprintf(szTmp, "%d", m_pParent->dwIndex);
	SetDlgItemText(IDC_EDIT17, szTmp);

	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK1);
	pBtn->SetCheck(0);

	GetDlgItem(IDC_EDIT13)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT14)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT15)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT16)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHECK2)->ShowWindow(SW_HIDE);

	return TRUE;
}

void COldSetGetHistoryDlg::OnBnClickedCheck1()
{
	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK1);
	int iMode = pBtn->GetCheck();
	if(iMode == 0)
	{
		SetDlgItemText(IDC_EDIT13, "");
		SetDlgItemText(IDC_EDIT14, "");
		SetDlgItemText(IDC_EDIT15, "");
		SetDlgItemText(IDC_EDIT16, "");
		GetDlgItem(IDC_EDIT13)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT14)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT15)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT16)->EnableWindow(FALSE);
	}
	else
	{
		CTime cTime = CTime::GetCurrentTime();
		char szTmp[20];
		int iTmp = cTime.GetYear();
		sprintf(szTmp, "%d", iTmp);
		SetDlgItemText(IDC_EDIT13, szTmp);

		iTmp = cTime.GetMonth();
		sprintf(szTmp, "%d", iTmp);
		SetDlgItemText(IDC_EDIT14, szTmp);

		iTmp = cTime.GetDay();
		sprintf(szTmp, "%d", iTmp);
		SetDlgItemText(IDC_EDIT15, szTmp);

		iTmp = cTime.GetHour();
		sprintf(szTmp, "%d", iTmp);
		SetDlgItemText(IDC_EDIT16, szTmp);

		GetDlgItem(IDC_EDIT13)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT14)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT15)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT16)->EnableWindow(TRUE);
	}
}

void COldSetGetHistoryDlg::OnBnClickedButton3()
{
	m_pParent->dw64StartTime = 0;
	m_pParent->dw64EndTime = 0;
	m_pParent->m_fIsUsedEndTime = FALSE;
	m_pParent->m_fIsGetHistoryRecord = FALSE;
	OnCancel();
}

void COldSetGetHistoryDlg::OnBnClickedButton1()
{
	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK1);
	int iMode = pBtn->GetCheck();

	char szTmp[20] = {0};
	int iYear;
	int iMoth;
	int iDay;
	int iHour;
	GetDlgItemText(IDC_EDIT1, szTmp, 20);
	iYear = atoi(szTmp);
	GetDlgItemText(IDC_EDIT10, szTmp, 20);
	iMoth = atoi(szTmp);
	GetDlgItemText(IDC_EDIT11, szTmp, 20);
	iDay = atoi(szTmp);
	GetDlgItemText(IDC_EDIT12, szTmp, 20);
	iHour = atoi(szTmp);

	if (iYear <= 1970  || iYear >=2038)
	{
		MessageBox("年份必须大于1970年 小于2038年");
		return;
	}

	CTime cTime(iYear, iMoth, iDay, iHour, 0, 0);
	m_pParent->dw64StartTime = cTime.GetTime();

	if(iMode == 1)
	{
		GetDlgItemText(IDC_EDIT13, szTmp, 20);
		iYear = atoi(szTmp);
		GetDlgItemText(IDC_EDIT14, szTmp, 20);
		iMoth = atoi(szTmp);
		GetDlgItemText(IDC_EDIT15, szTmp, 20);
		iDay = atoi(szTmp);
		GetDlgItemText(IDC_EDIT16, szTmp, 20);
		iHour = atoi(szTmp);
		CTime cTimeEnd(iYear, iMoth, iDay, iHour, 0, 0);
		m_pParent->dw64EndTime = cTimeEnd.GetTime();
		m_pParent->m_fIsUsedEndTime = TRUE;
	}
	else
	{
		m_pParent->dw64EndTime = 0;
		m_pParent->m_fIsUsedEndTime = FALSE;
	}

	m_pParent->m_fIsGetHistoryRecord = TRUE;

	OnCancel();
}
