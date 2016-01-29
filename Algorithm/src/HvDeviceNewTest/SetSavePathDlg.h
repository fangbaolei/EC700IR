#ifndef _SETSAVEPATH_H__
#define _SETSAVEPATH_H__

#include "resource.h"
#include "HvDevice.h"
#include "ResultRecvTestDlg.h"

class CSetSavePathDlg : public CDialog
{
	DECLARE_DYNAMIC(CSetSavePathDlg)
public:
	CSetSavePathDlg(HVAPI_HANDLE* phHandle, CWnd* pParent = NULL);
	virtual ~CSetSavePathDlg();
	enum {IDD = IDD_DIALOG7};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

private:
	HICON	m_hIcon;
	HVAPI_HANDLE* m_hDevice;
	CResultRecvTestDlg* m_pParentDlg;
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton3();

protected:

	virtual BOOL PreTranslateMessage(MSG* pMsg);
    virtual void OnOK();


};

#endif
