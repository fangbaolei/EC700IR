#ifndef _CUPDATEDLG_H__
#define _CUPDATEDLG_H__

#include "resource.h"
#include "HvDevice.h"

//#define OLD_XML_FORMAT

class CUpdateDlg : public CDialog
{
	DECLARE_DYNAMIC(CUpdateDlg);
public:
	CUpdateDlg(HVAPI_HANDLE& hHandle, CWnd* pParent = NULL);
	virtual ~CUpdateDlg(void);

	enum {IDD = IDD_DIALOG1};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	void OnCancel();
	DECLARE_MESSAGE_MAP()
	BOOL CheckUpdateFile(void);
	void GetControllPannelStatus(int& iWorkMode, int& iWorkStatus);

private:
	HVAPI_HANDLE m_HvDevice;
	CWinThread* m_pProcessUpDateThread;
	BOOL m_fIsRunningUpdata;
	int m_iUpdateStatus;
	unsigned char* m_rgbUpdateFileBuffer;
	UINT8 m_iCRCValue;
	CProgressCtrl m_Progress;

public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton8();
	afx_msg void OnBnClickedButton9();
	afx_msg LRESULT OnShearFinsh(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnShowMessage(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateProgress(WPARAM wParam, LPARAM lParam);
	static UINT ProcessUpDateThread(LPVOID pUserData);
	static UINT ProcessUpDateThread2(LPVOID pUserData);
};

#endif
