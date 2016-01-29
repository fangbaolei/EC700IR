#ifndef _HVDEVIC_OLD_TEST_DLG_H__
#define _HVDEVIC_OLD_TEST_DLG_H__

#include "HvDevice.h"
#include "resource.h"
#include "OldResultRecvTest.h"
#include "OldImageRecvTestDlg.h"
#include "OldVideoRecvTestDlg.h"
#include "OldCmdTestDlg.h"
#include "OldGroupTestDlg.h"
#include "afxtempl.h"

typedef struct _DevIPInfo
{	
	char szIP[64];
	char szMask[64];
	char szGateWay[64];
	char szMac[128];
	_DevIPInfo()
	{
		memset(this, 0, sizeof(_DevIPInfo));
	}
}DevIPInfo;

class COldHvDeviceTestDlg : public CDialog
{
	DECLARE_DYNAMIC(COldHvDeviceTestDlg)
public:
	COldHvDeviceTestDlg(CWnd* pParent = NULL);
	virtual ~COldHvDeviceTestDlg();
	enum{IDD = IDD_HVDEVICENEWTEST_DIALOG};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
	void EmptyList(void);

protected:
	HICON	m_hIcon;

protected:
	CListCtrl m_ListDevice;
	HVAPI_HANDLE m_hDevice;
	int m_iTotleDevice;
	COldResultRecvTestDlg* m_pDlg;
	COldImageRecvTestDlg* m_pImageDlg;
	COldVideoRecvTestDlg* m_pVideoDlg;
	COldCmdTestDlg* m_pCmdDlg;
	CList<DevIPInfo*> m_DevIPList;
	CRITICAL_SECTION m_csDevIPList;

public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton8();
	afx_msg void OnBnClickedButton9();
	afx_msg void OnBnClickedButton10();
	afx_msg void OnBnClickedButton13();
	afx_msg void OnNMRclickList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnGetDevType();
	afx_msg void OnSetIP();
};

#endif
