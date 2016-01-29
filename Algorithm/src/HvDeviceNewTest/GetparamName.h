#pragma once


// CGetparamName dialog

class CGetparamName : public CDialog
{
	DECLARE_DYNAMIC(CGetparamName)

public:
	CGetparamName(CWnd* pParent = NULL);   // standard constructor
	virtual ~CGetparamName();

// Dialog Data
	enum { IDD = IDD_DIALOG10 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CString m_paramname;
};
