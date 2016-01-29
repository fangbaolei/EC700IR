#ifndef _OLDTESTSETSAVEPATH_H__
#define _OLDTESTSETSAVEPATH_H__

#include "resource.h"
#include "HvDevice.h"
#include "HvDeviceOldTestDlg.h"

class COldTestSetSavePathDlg : public CDialog
{
	DECLARE_DYNAMIC(COldTestSetSavePathDlg)
public:
	COldTestSetSavePathDlg(HVAPI_HANDLE* phHandle, CWnd* pParent = NULL);
	virtual ~COldTestSetSavePathDlg();
	enum {IDD = IDD_DIALOG7};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

private:
	HICON	m_hIcon;
	HVAPI_HANDLE* m_hDevice;
	COldResultRecvTestDlg* m_pParentDlg;

public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton3();
};

#endif