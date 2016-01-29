// DiskSettingToolDlg.h : 头文件
//

#pragma once


// CDiskSettingToolDlg 对话框
class CDiskSettingToolDlg : public CDialog
{
// 构造
public:
	CDiskSettingToolDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_DISKSETTINGTOOL_DIALOG };

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
	afx_msg void OnOK();
	afx_msg void OnCancel();
	afx_msg void OnDClickList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRDClickList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedButton1(void);
	afx_msg void OnBnClickedButton2(void);
	DECLARE_MESSAGE_MAP()

private:
	afx_msg void StartChecking(void);
	void GetDiskListInfo(void);
	BOOL GetDiskInfo(CString strDiskChar, DWORD& dwDiskSize);
private:
	CString		m_strSystemDisk;
	CString		m_strAppPath;
	CButton		m_btOK;
	CButton		m_btCancel;
	CComboBox	m_cbCurrentDisk;
	CListCtrl	m_lcDiskInfo;
};
