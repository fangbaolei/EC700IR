#pragma once

// AutoLinkChangeString dialog

class AutoLinkChangeString : public CDialog
{
	DECLARE_DYNAMIC(AutoLinkChangeString)

public:
	AutoLinkChangeString(CWnd* pParent = NULL);   // standard constructor
	virtual ~AutoLinkChangeString();

// Dialog Data
	enum { IDD = IDD_DIALOG_CHANGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

public:
	char m_chInitText[1024];
	CString m_strReturnText;
	CEdit m_ExampleEdit;
	CEdit m_InfoEdit;
	CEdit m_ChangeEdit;
	CComboBox m_TimeDropList;
	CComboBox m_PlateDropList;
	CComboBox m_DevInfoDropList;
	CComboBox m_OtherDropList;
	CComboBox m_TypeDropList;

	bool m_bFirstUseDropLise; // 是否初次使用下拉列表框

	CCommon m_Common;

	void InitChangeDlg();
	void SetInitText(char* chText);
	void GetReturnText(char* chText);
	void ShowText(CString strText);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButtonMatchok();
	afx_msg void OnBnClickedButtonMatchcancel();
	afx_msg void OnClose();
	afx_msg void OnCbnSelchangeComboMatchtime();
	afx_msg void OnCbnSelchangeComboMatchplate();
	afx_msg void OnCbnSelchangeComboMatchdev();
	afx_msg void OnCbnSelchangeComboMatchother();
	afx_msg void OnCbnSelchangeComboType();
};
