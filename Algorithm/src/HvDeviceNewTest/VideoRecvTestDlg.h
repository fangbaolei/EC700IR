#ifndef _VIDEORECVTEST_H__
#define _VIDEORECVTEST_H__

#include "resource.h"
#include "HvDeviceNew.h"
#include "hvvartype.h"

using namespace HiVideo;

typedef struct _VideoFrameData
{
	DWORD	dwVideoType;
	DWORD	dwVideoDataLen;
	DWORD	dwVideoExInfoLen;
	PBYTE	pVideoData;
	char*	pszVideoExInfo;

	_VideoFrameData()
	{
		pVideoData = NULL;
		pszVideoExInfo = NULL;
	}
}VideoFrameData;

class CVideoRecvTestDlg : public CDialog
{
	DECLARE_DYNAMIC(CVideoRecvTestDlg)
public:
	CVideoRecvTestDlg(HVAPI_HANDLE_EX* phHandle, CWnd* pParent = NULL);
	virtual ~CVideoRecvTestDlg();
	enum {IDD = IDD_DIALOG3};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	afx_msg void OnCancel();
	afx_msg LRESULT OnGetFinish(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnProceVideo(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

private:
	HICON	m_hIcon;
	HVAPI_HANDLE_EX* m_hDevice;
	BOOL	m_fIsConnectH264Video;
	BOOL	m_fIsRecvHistoryVideo;
	DWORD	m_dwStartTime;
	DWORD	m_dwEndTime;
	RECT	m_ShowFrameRect;
	CHvList<VideoFrameData>	m_VideoFrameData;
	CRITICAL_SECTION	m_csList;
	BOOL	m_fIsQuit;
	HANDLE	m_hProceThread;

private:
	static int OnH264Video(PVOID pUserData,
		PBYTE pbVideoData,
		DWORD dwVideoDataLen,
		DWORD dwVideoType,
		LPCSTR szVideoExtInfo);

	static INT OnH264VideoEx(
		PVOID pUserData,  
		DWORD dwVedioFlag,
		DWORD dwVideoType, 
		DWORD dwWidth,
		DWORD dwHeight,
		DWORD64 dw64TimeMS,
		PBYTE pbVideoData, 
		DWORD dwVideoDataLen,
		LPCSTR szVideoExtInfo);

	static int OnHistoryVideo(PVOID pUserData,
		PBYTE pbVideoData,
		DWORD dwVideoDataLen,
		DWORD dwVideoType,
		LPCSTR szVideoExtInfo);

	static INT OnHistoryVideoEx(
		PVOID pUserData,  
		DWORD dwVedioFlag,
		DWORD dwVideoType, 
		DWORD dwWidth,
		DWORD dwHeight,
		DWORD64 dw64TimeMS,
		PBYTE pbVideoData, 
		DWORD dwVideoDataLen,
		LPCSTR szVideoExtInfo);
	static DWORD WINAPI ProceThread(LPVOID lParam);

public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnTimer(UINT nIDEvent);

protected:
	//virtual void OnOK();
	//virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void PostNcDestroy();

public:
	afx_msg void OnClose();
};

#endif
