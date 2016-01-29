#ifndef _SET_GET_HISTORY_RECORD_H__
#define _SET_GET_HISTORY_RECORD_H__

#include "resource.h"
#include "ResultRecvTestDlg.h"

class CSetGetHistoryDlg : public CDialog
{
	DECLARE_DYNAMIC(CSetGetHistoryDlg)
public:
	CSetGetHistoryDlg(CResultRecvTestDlg* pParent);
	virtual ~CSetGetHistoryDlg();
	enum {IDD = IDD_DIALOG9};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

private:
	HICON	m_hIcon;
	CResultRecvTestDlg* m_pParent;
public:
	afx_msg void OnBnClickedCheck1();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton1();
};

#endif