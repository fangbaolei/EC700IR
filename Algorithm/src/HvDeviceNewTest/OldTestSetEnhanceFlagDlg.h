#ifndef _OLD_TEST_SET_ENHANCE_H__
#define _OLD_TEST_SET_ENHANCE_H__

#include "resource.h"
#include "HvDevice.h"
#include "OldResultRecvTest.h"

class COldTestSetEnahnceFlagDlg : public CDialog
{
	DECLARE_DYNAMIC(COldTestSetEnahnceFlagDlg)
public:
	COldTestSetEnahnceFlagDlg(HVAPI_HANDLE* phHandle, CWnd* pParent = NULL);
	virtual ~COldTestSetEnahnceFlagDlg();
	enum {IDD = IDD_DIALOG6};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

private:
	HICON	m_hIcon;
	HVAPI_HANDLE* m_hDevice;
	CComboBox m_ComBoxStyle;
	COldResultRecvTestDlg* m_pParent;
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton8();
};

#endif