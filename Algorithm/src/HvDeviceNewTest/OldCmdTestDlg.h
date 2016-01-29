#ifndef _OLD_CMDTESTDLG_H__
#define _OLD_CMDTESTDLG_H__

#include "resource.h"
#include "HvDevice.h"

class COldCmdTestDlg : public CDialog
{
	DECLARE_DYNAMIC(COldCmdTestDlg)
public:
	COldCmdTestDlg(HVAPI_HANDLE* phHandle, CWnd* pParent = NULL);
	virtual ~COldCmdTestDlg();
	enum {IDD = IDD_DIALOG5};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

private:
	HICON	m_hIcon;
	HVAPI_HANDLE* m_hDevice;

public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton6();
};

#endif