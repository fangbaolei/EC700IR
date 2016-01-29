#ifndef _HVNAVITEST_H__
#define _HVNAVITEST_H__

#include "resource.h"
#include "HvDevice.h"
#include "HvDeviceOldTestType.h"
#include <afxtempl.h>

typedef CList<CDeviceType*, CDeviceType*> CDeviceList;

class CHvDeviceOldTestDlg : public CDialog
{
	DECLARE_DYNAMIC(CHvDeviceOldTestDlg)
public:
	CHvDeviceOldTestDlg(CWnd* pParent = NULL);
	virtual ~CHvDeviceOldTestDlg();
	enum {IDD = IDD_HVNAVI};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual void OnCancel();

	afx_msg LRESULT OnSearchDev(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

private:
	HICON	m_hIcon;
	HCURSOR m_hCursor;
	CListBox m_ListInfo;
	CListCtrl m_ListShowFrame;

	INT m_iTotolDevice;
	INT m_iCurrentSelectDevice;
	CDeviceList	m_DeviceList;
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRclickList1(NMHDR *pNMHDR, LRESULT *pResult);

protected:
	virtual void OnOK();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

public:
	afx_msg void OnBnClickedButton4();
};

#endif
