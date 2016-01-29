// HvDeviceNewTestDlg.h : 头文件
//

#pragma once

#include "HvDeviceNew.h"
#include "HvCamera.h"
#include "ResultRecvTestDlg.h"
#include "ImageRecvTestDlg.h"
#include "VideoRecvTestDlg.h"
#include "CmdTestDlg.h"
#include "HvDeviceOldTest.h"
#include "HvDeviceOldTestDlg.h"
#include "CUpdateDlg.h"
#include "AutoLink_demoDlg.h"
#include "NewSDKTestDialog.h"
#include ".\newsdktestdialog.h"

// CHvDeviceNewTestDlg 对话框
class CHvDeviceNewTestDlg : public CDialog
{
// 构造
public:
	CHvDeviceNewTestDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_HVDEVICENEWTEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnCancel();
	DECLARE_MESSAGE_MAP()

protected:
	CListCtrl	m_ListDevice;
	HVAPI_HANDLE_EX m_hDevice;
	int m_iTotleDeveci;
	CResultRecvTestDlg* m_pDlg;
	CImageRecvTestDlg* m_pImageDlg;
	CVideoRecvTestDlg* m_pVideoDlg;
	CCmdTestDlg* m_pCmdDlg;
	COldHvDeviceTestDlg* m_pOldTestDlg;
	CUpdateDlg* m_pupdataDlg;
	CNewSDKTestDialog* m_pNewSDKTestDlg;

public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton8();
	afx_msg void OnBnClickedButton9();
	afx_msg void OnBnClickedButton10();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton13();
	afx_msg void OnBnClickedButton14();
	afx_msg void OnBnClickedButton16();
	afx_msg void OnNMRclickList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnGetDevType();
	afx_msg void OnSetIP();
	afx_msg void OnCaptureImage();
	afx_msg void OnBnClickedButton12();
	afx_msg void OnBnClickedButton18();
	afx_msg void OnBnClickedButton15();


private:
	int m_BeforeIndex;
public:
	afx_msg void OnClose();
private:
	CAutoLink_demoDlg* m_autolinkDlg;

protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
    virtual void OnOK();

public:
	afx_msg void OnBnClickedButtonAddStaticIp();
	afx_msg void OnBnClickedButtonCaptueimage();
};
