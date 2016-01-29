#pragma once

// AutoLinkHistorySet dialog

class AutoLinkHistorySet : public CDialog
{
	DECLARE_DYNAMIC(AutoLinkHistorySet)

public:
	AutoLinkHistorySet(CWnd* pParent = NULL);   // standard constructor
	virtual ~AutoLinkHistorySet();

// Dialog Data
	enum { IDD = IDD_AUTOLINK_SETHISTORY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

public:
    char m_szDevSN[128];
	bool * m_fSetParam;
	SAFE_MODE_INFO m_safeModeInfo; // 历史结果

	CButton m_enableRecvHistoryBut;
	CDateTimeCtrl m_beginTimeCtrl;
	CEdit m_indexEdit;
	CButton m_enableEndTimeBut;
	CDateTimeCtrl m_endTimeCtrl;
	CButton m_onlyViolationBut;

	CCommon m_Common;
	
	void InitHistorySet(char* pSN, bool* fSetParam);
	void ReadIniFile();
	void WriteIniFile();
	void InitParamDlg();

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedSaveButton();
	afx_msg void OnBnClickedCancelButton();
	afx_msg void OnEnableHistory();
	afx_msg void OnEnableEndTime();
};
