#pragma once

// AutoLinkNameSet dialog

class AutoLinkNameSet : public CDialog
{
	DECLARE_DYNAMIC(AutoLinkNameSet)

public:
	AutoLinkNameSet(CWnd* pParent = NULL);   // standard constructor
	virtual ~AutoLinkNameSet();

// Dialog Data
	enum { IDD = IDD_AUTOLINK_NAMESET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

public:
	DEV_FILENAME m_FileNameInfo;
	DEV_FILENAME m_DefaultFileNameInfo;

	CCommon m_Common;

	CButton m_BigImgButton;
	CButton m_SmallImgButton;
	CButton m_BinButton;
	CButton m_InfoButton;
	CButton m_PlateButton;
	CButton m_PlateNoButton;

	CEdit m_BigImgEdit;
	CEdit m_SmallImgEdit;
	CEdit m_BinEdit;
	CEdit m_InfoEdit;
	CEdit m_PlateEdit;
	CEdit m_PlateNoEdit;

	void InitNameSet();
	void ReadFileNameIni();
	void WriteFileNameIni();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButtonNameok();
	afx_msg void OnBnClickedButtonNamecancel();
	afx_msg void OnBnClickedButtonNamedefault();
	afx_msg void OnBnClickedButtonNameset1();
	afx_msg void OnBnClickedButtonNameset2();
	afx_msg void OnBnClickedButtonNameset3();
	afx_msg void OnBnClickedButtonNameset4();
	afx_msg void OnBnClickedButtonNameset5();
	afx_msg void OnBnClickedButtonNameinit();
};
