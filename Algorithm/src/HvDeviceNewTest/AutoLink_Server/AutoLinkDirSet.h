#pragma once

// AutoLinkDirSet dialog

class AutoLinkDirSet : public CDialog
{
	DECLARE_DYNAMIC(AutoLinkDirSet)

public:
	AutoLinkDirSet(CWnd* pParent = NULL);   // standard constructor
	virtual ~AutoLinkDirSet();

// Dialog Data
	enum { IDD = IDD_DIALOG_DIRSET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	char m_chIllegalDir[1024];
	char m_chNormalDir[1024];
	char m_chDefaultIllegalDir[1024];
	char m_chDefaultNormalDir[1024];
	CEdit m_IllegalEdit;
	CEdit m_NormalEdit;

	CCommon m_Common;

	void InitDirDlg();
	void ReadDirIni();
	void WriteDirIni();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButtonDirok();
	afx_msg void OnBnClickedButtonDircancel();
	afx_msg void OnBnClickedButtonDirdefault();
	afx_msg void OnBnClickedButtonDirset1();
	afx_msg void OnBnClickedButtonDirset2();
	afx_msg void OnBnClickedButtonInit();
};
