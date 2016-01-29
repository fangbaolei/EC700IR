#ifndef _WARNNING_H__
#define _WARNNING_H__

#include "resource.h"

class CWarnningDlg : public CDialog
{
	DECLARE_DYNAMIC(CWarnningDlg);
public:
	CWarnningDlg(DWORD* pdwProceMode, int iCRCValue, CWnd* pParnt = NULL);
	virtual ~CWarnningDlg();

	enum{IDD = IDD_DIALOG11};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	void OnCancel();
	DECLARE_MESSAGE_MAP()
	
private:
	HICON	m_hIcon;
	DWORD*	m_pdwProcMode;
	int		m_iCRCValue;

public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton8();
};

#endif

