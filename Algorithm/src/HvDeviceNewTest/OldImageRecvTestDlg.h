#ifndef _OLD_IMAGERECVTEST_H__
#define _OLD_IMAGERECVTEST_H__

#include "resource.h"
#include "HvDevice.h"

class COldImageRecvTestDlg : public CDialog
{
	DECLARE_DYNAMIC(COldImageRecvTestDlg)
public:
	COldImageRecvTestDlg(HVAPI_HANDLE* phHandle, CWnd* pParent = NULL);
	virtual ~COldImageRecvTestDlg();
	enum {IDD = IDD_DIALOG2};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual void OnCancel();

	DECLARE_MESSAGE_MAP()

private:
	HICON m_hIcon;
	HVAPI_HANDLE* m_hDevice;
	DWORD	m_dwLastStatus;
	DWORD	m_dwReconnectTimes;

private:
	static int OnImage(PVOID pUserData,
		PBYTE pbImageData,
		DWORD dwImageDataLen,
		PBYTE pbImageInfoData,
		DWORD dwImageInfoLen,
		DWORD dwImageType,
		LPCSTR szImageExtInfo);

public:
	afx_msg void OnTimer(UINT nIDEvent);
};

#endif
