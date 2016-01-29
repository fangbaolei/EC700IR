#ifndef _OLD_SET_VIDEO_ENHANCE_H__
#define _OLD_SET_VIDEO_ENHANCE_H__

#include "resource.h"
#include "OldVideoRecvTestDlg.h"

class COldSetVideoEnahnceFlagDlg : public CDialog
{
	DECLARE_DYNAMIC(COldSetVideoEnahnceFlagDlg)
public:
	COldSetVideoEnahnceFlagDlg(CWnd* pParent = NULL);
	virtual ~COldSetVideoEnahnceFlagDlg();
	enum {IDD = IDD_DIALOG4};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

private:
	HICON	m_hIcon;
	COldVideoRecvTestDlg* m_pVideoDlg;

public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton8();
};

#endif