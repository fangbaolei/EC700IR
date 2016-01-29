// AutoLink_demoDlg.h : 头文件
//

#pragma once


#include "../HvDeviceDLL/HvDevice.h"
#include "afxcmn.h"
#include "afxwin.h"
#include <afxtempl.h>


typedef struct _DEV_HANDLE
{
	HVAPI_HANDLE handle;
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
public:
	HRESULT ShowClientInfor( char* pSN, char* pStatus,int nReConnectCount , char* dwIP, int port);

public:
	DWORD m_dwTotleCar;
	BOOL  m_fIsSaveResult;
	BOOL  m_fIsExit;
	CIPAddressCtrl m_IPAdressCtrl;
	INT	  m_iAddRedMode;
	HVAPI_HANDLE m_hHandleHvCam;
	HVAPI_HANDLE m_hServerHandle;

	CString m_strSNSelDev;

	bool m_fExit;

// 实现
protected:
	HICON m_hIcon;
	HANDLE m_hShowThread;
	CList<DEV_HANDLE* , DEV_HANDLE*> m_connDevList;
	
	HRESULT InitDevListCtrl();
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
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
	afx_msg void OnSetConfig();

	void deleteSrc();

protected:
	afx_msg void OnCancel();
	virtual void PostNcDestroy();
protected:
	virtual void OnOK();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

};
