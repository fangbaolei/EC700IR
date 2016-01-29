#pragma once
#include "afxwin.h"
#include "resource.h"
#include "HVEDemoExDlg.h"

class CSystemSettingDlg : public CDialog
{
	DECLARE_DYNAMIC(CSystemSettingDlg)
public:
	CSystemSettingDlg(CWnd* pParent = NULL);
	enum { IDD = IDD_DIALOGSYSSET};
protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
//	void	SetComBo(void);
	DECLARE_MESSAGE_MAP()
protected:
	CIPAddressCtrl		m_IPControl;
	CEdit				m_EditDataBaseName;
	CEdit				m_EditUserName;
	CEdit				m_EditPassWord;
	CStatic				m_StaticCurrentStatue;
	CButton				m_ButtonConnect;
	CButton				m_ButtonDisConnect;

	CHVEDemoExDlg*		m_ParentFrame;
private:
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnCbnSelchangeCombo1();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DWORD	IPStr2DWord(char* szIP);
	afx_msg void OnCancel();
	afx_msg void OnOK();
	BOOL PreTranslateMessage(MSG* pMsg);
};
