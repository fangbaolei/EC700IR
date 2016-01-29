#ifndef _SET_ENHANCE_H__
#define _SET_ENHANCE_H__

#include "resource.h"
#include "HvDeviceNew.h"

class CSetEnahnceFlagDlg : public CDialog
{
	DECLARE_DYNAMIC(CSetEnahnceFlagDlg)
public:
	CSetEnahnceFlagDlg(HVAPI_HANDLE_EX* phHandle, CWnd* pParent = NULL);
	virtual ~CSetEnahnceFlagDlg();
	enum {IDD = IDD_DIALOG6};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

private:
	HICON	m_hIcon;
	HVAPI_HANDLE_EX* m_hDevice;
	CComboBox m_ComBoxStyle;
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton8();
};

#endif