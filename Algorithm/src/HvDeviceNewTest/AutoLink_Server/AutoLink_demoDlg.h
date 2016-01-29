// AutoLink_demoDlg.h : 头文件

#pragma once

#include "Camera.h"

typedef struct _DEV_HANDLE
{
	HVAPI_HANDLE_EX handle;
	char szDevSN[128];

	_DEV_HANDLE()
		: handle(NULL)
	{
		ZeroMemory(szDevSN , sizeof(szDevSN));
	}
}DEV_HANDLE;


// CAutoLink_demoDlg 对话框
class CAutoLink_demoDlg : public CDialog
{
// 构造
public:
	CAutoLink_demoDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_AUTOLINK_DEMO_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	HICON m_hIcon;
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	int* m_pIsUsrDiscnn;
	int m_iLogEnable; // 日志开关
	
	bool m_fExit;
	CString m_strDir;  // 保存盘符目录
	int m_iPort;  // 监听端口
	int m_iDevMaxNum; // 监听设备数量

	HANDLE m_hShowThread;
	HANDLE m_hDeleteThread;
	CList<CCamera*, CCamera*> m_connDevList;

	CCommon m_Common; // 公共方法类
	
	HRESULT InitDevListCtrl();
	HRESULT CreatConnect(char* pSN);
	HRESULT DisConnectDev(char* pSN);
	int UpdateClient();
	HRESULT ShowClientInfor(char* pSN);

	void WriteFileIni();
	void ReadFileIni();

	void WriteDlgIni();
	void ReadDlgIni();
	void WriteLog(char* chText);

	// 循环覆盖
	int DeleteFileRun();
	int CheckDriveSpaceSize(char *szDriveName);
	int DeleteFileByName(const char* chFileName);
	int DeleteDirectory(const char* chDir);

public:
	static DWORD WINAPI UpdateClientThread(LPVOID lpParameter)
	{
		if (NULL == lpParameter)
			return 0;

		CAutoLink_demoDlg* pThis = (CAutoLink_demoDlg*)lpParameter;
		return pThis->UpdateClient();
	}

	static DWORD WINAPI DeleteThread(LPVOID lpParameter)
	{
		if (NULL == lpParameter)
			return 0;

		CAutoLink_demoDlg* pThis = (CAutoLink_demoDlg*)lpParameter;
		return pThis->DeleteFileRun();
	}

	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	CListCtrl m_devListCtrl;
	CEdit m_autoLinkPortEdit;
	CEdit m_maxListenNumEdit;
	CButton m_openAutoLinkBut;
	CButton m_closeAutoLinkBut;
	afx_msg void OnClose();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedFindDir();
	afx_msg void OnDBClick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnRClickDevList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSetHistoryConnect();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButtonDir();
	afx_msg void OnBnClickedButtonFilename();
};
