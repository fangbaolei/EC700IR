#ifndef _OLD_VIDEORECVTEST_H__
#define _OLD_VIDEORECVTEST_H__

#include "resource.h"
#include "HvDevice.h"
#include "hvvartype.h"

using namespace HiVideo;

typedef struct _VIDEO_PACKET_DATA
{
	DWORD	dwVideoType;
	DWORD	dwVideoDataLen;
	DWORD	dwVideoExInfoLen;
	PBYTE	pVideoData;
	char*	pszVideoExInfo;

	_VIDEO_PACKET_DATA()
	{
		pVideoData = NULL;
		pszVideoExInfo = NULL;
	}
}VIDEO_PACKET_DATA;

class COldVideoRecvTestDlg : public CDialog
{
	DECLARE_DYNAMIC(COldVideoRecvTestDlg)
public:
	COldVideoRecvTestDlg(HVAPI_HANDLE* phHandle, CWnd* pParent = NULL);
	virtual ~COldVideoRecvTestDlg();
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
	HVAPI_HANDLE* m_hDevice;
	BOOL	m_fIsConnectH264Video;
	BOOL	m_fIsRecvHistoryVideo;
	DWORD	m_dwStartTime;
	DWORD	m_dwEndTime;
	RECT	m_ShowFrameRect;
	CHvList<VIDEO_PACKET_DATA>	m_VideoFrameData;
	CRITICAL_SECTION	m_csList;
	BOOL	m_fIsQuit;
	HANDLE	m_hProceThread;
	DWORD	m_dwLastStatus;
	DWORD	m_dwReconnectTimes;

public:
	INT  m_iRedLightEnhanceFlag;
	INT	 m_iBrightness;
	INT  m_iHueThreshold;
	INT  m_CompressRate;

private:
	static int OnH264Video
		(PVOID pUserData,
		PBYTE pbVideoData,
		DWORD dwVideoDataLen,
		DWORD dwVideoType,
		LPCSTR szVideoExtInfo);
	static DWORD WINAPI ProceThread(LPVOID lParam);

public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnTimer(UINT nIDEvent);
};

#endif