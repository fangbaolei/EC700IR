#ifndef _CMDTESTDLG_H__
#define _CMDTESTDLG_H__

#include "resource.h"
#include "HvDeviceNew.h"

class CCmdTestDlg : public CDialog
{
	DECLARE_DYNAMIC(CCmdTestDlg)
public:
	CCmdTestDlg(HVAPI_HANDLE_EX* phHandle, CWnd* pParent = NULL);
	virtual ~CCmdTestDlg();
	enum {IDD = IDD_DIALOG5};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

private:
	HICON	m_hIcon;
	HVAPI_HANDLE_EX* m_hDevice;

public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton6();
	afx_msg void OnClose();

protected:
	virtual void OnOK();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	afx_msg void OnCancel();
	virtual void PostNcDestroy();


};

#endif
