#ifndef _IMAGERECVTEST_H__
#define _IMAGERECVTEST_H__

#include "resource.h"
#include "HvDeviceNew.h"
#include "hvvartype.h"
#include "afxwin.h"

using namespace HiVideo;

//typedef struct _JpegFrameData
//{
//	DWORD	dwImageType;
//	DWORD	dwImageDataLen;
//	DWORD	dwImageExInfoLen;
//	PBYTE	pImageData;
//	char*	pszImageExInfo;
//
//	_JpegFrameData()
//	{
//		pImageData = NULL;
//		pszImageExInfo = NULL;
//	}
//}JpegFrameData;

class CImageRecvTestDlg : public CDialog
{
	DECLARE_DYNAMIC(CImageRecvTestDlg)
public:
	CImageRecvTestDlg(HVAPI_HANDLE_EX* phHandle, CWnd* pParent = NULL);
	virtual ~CImageRecvTestDlg();
	enum {IDD = IDD_DIALOG2};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual void OnCancel();

	DECLARE_MESSAGE_MAP()

private:
	HICON m_hIcon;
	HVAPI_HANDLE_EX* m_hDevice;
	//CHvList<JpegFrameData> m_JpegFrameData;
	//CRITICAL_SECTION m_csList;
	//BOOL m_fIsQuit;
	//HANDLE m_hProceThread;
	BOOL m_fIsShowLpr;
	int  m_nLprSet; 
	CString m_strIP;

public:
    void GetIP(CString strIP);

private:
	static int OnImage(PVOID pUserData,
		PBYTE pbImageData,
		DWORD dwImageDataLen,
		DWORD dwImageType,
		LPCSTR szImageExtInfo);
	static int OnImageEx(
		PVOID pUserData,  
		DWORD dwImageFlag,
		DWORD dwImageType, 
		DWORD dwWidth,
		DWORD dwHeight,
		DWORD64 dw64TimeMS,
		PBYTE pbImageData, 
		DWORD dwImageDataLen,
		LPCSTR szImageExtInfo);

public:
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg LRESULT OnUpdateFps(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnProceJpegFrame(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();

	afx_msg void OnBnClickedRadio3();
	afx_msg void OnBnClickedRadio4();

public:
	void SetIni();

protected:
	virtual void OnOK();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

public:
	afx_msg void OnBnClickedBtnCap();
	afx_msg void OnClose();
	virtual void PostNcDestroy();

};

#endif
