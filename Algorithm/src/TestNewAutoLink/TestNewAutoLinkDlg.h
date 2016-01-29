// TestNewAutoLinkDlg.h : 头文件
//

#pragma once

#include "HvAutoLinkDeviceNew.h"
#include "afxcmn.h"

// CTestNewAutoLinkDlg 对话框

#include "Afxtempl.h"

#include <afxmt.h>
#include "Device.h"
#include "afxdtctl.h"

typedef CList<CDevice*,CDevice*> CDeviceList;

class CTestNewAutoLinkDlg : public CDialog
{
// 构造
public:
	CTestNewAutoLinkDlg(CWnd* pParent = NULL);	// 标准构造函数

	virtual ~CTestNewAutoLinkDlg();

// 对话框数据
	enum { IDD = IDD_TESTNEWAUTOLINK_DIALOG };

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
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonStartserver();
	afx_msg void OnBnClickedButtonCloseserver();

	int OnLineNotice(HVAPI_OPERATE_HANDLE handle, char*szDeviceNo, char*szAddr,int iPort,char *szOtherXmlInfo);
	int OffLineNotice(HVAPI_OPERATE_HANDLE handle, char* szDeviceNo);

	HRESULT InitDevListCtrl();
	CListCtrl m_deviceList;
	CMutex    m_mtListCtrl;


	CDevice* GetSelDevice();
	
private:
	CDeviceList m_DeviceArray;
	int m_itemSel;

	CMutex m_mtDevcieList;

public:
	afx_msg void OnNMClickListDevicelist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRclickListDevicelist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnGetxml();
	afx_msg void OnSynctime();
	afx_msg void OnGetdevicetime();
	afx_msg void OnGettraficcinfo();
	afx_msg void OnCaptureimage();
	afx_msg void OnGetvideo();
	afx_msg void OnStopvideo();
	afx_msg void OnGetjpeg();
	afx_msg void OnStipjpeg();
	afx_msg void OnUploadxml();
	UINT m_uiTimeM;
	CDateTimeCtrl m_dtStartY;
	CDateTimeCtrl m_dtStartH;
	CDateTimeCtrl m_dtEndY;
	CDateTimeCtrl m_dtEndH;
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnClose();
};
