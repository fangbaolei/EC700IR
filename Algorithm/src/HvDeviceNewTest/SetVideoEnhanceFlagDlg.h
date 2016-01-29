#ifndef _SET_VIDEO_ENHANCE_H__
#define _SET_VIDEO_ENHANCE_H__

#include "resource.h"
#include "HvDeviceNew.h"

class CSetVideoEnahnceFlagDlg : public CDialog
{
	DECLARE_DYNAMIC(CSetVideoEnahnceFlagDlg)
public:
	CSetVideoEnahnceFlagDlg(HVAPI_HANDLE_EX* phHandle, CWnd* pParent = NULL);
	virtual ~CSetVideoEnahnceFlagDlg();
	enum {IDD = IDD_DIALOG4};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

private:
	HICON	m_hIcon;
	HVAPI_HANDLE_EX* m_hDevice;
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton8();
};

#endif

