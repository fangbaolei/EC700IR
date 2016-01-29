#ifndef _SET_GET_HISTORY_RECORD_H__
#define _SET_GET_HISTORY_RECORD_H__

#include "resource.h"
#include "OldResultRecvTest.h"

class COldSetGetHistoryDlg : public CDialog
{
	DECLARE_DYNAMIC(COldSetGetHistoryDlg)
public:
	COldSetGetHistoryDlg(COldResultRecvTestDlg* pParent);
	virtual ~COldSetGetHistoryDlg();
	enum {IDD = IDD_DIALOG9};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

private:
	HICON	m_hIcon;
	COldResultRecvTestDlg* m_pParent;

public:
	afx_msg void OnBnClickedCheck1();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton1();
};

#endif