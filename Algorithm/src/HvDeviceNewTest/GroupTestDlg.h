#ifndef _GROUP_TEST_H__
#define _GROUP_TEST_H__

#include "resource.h"
#include "HvDeviceNew.h"
#include "HvVarType.h"

using namespace HiVideo;

typedef struct _GroupResultData
{
	INT iDataType;
	DWORD dwCarID;
	DWORD64 dw64TimeMS;
	char* pcPlateNo;
	char* pcAppendInfo;
	DWORD dwImageType;
	WORD wImageWidth;
	WORD wImageHeight;
	DWORD dwImageDataLen;
	PBYTE pImageData;

	_GroupResultData()
	{
		pcPlateNo = NULL;
		pcAppendInfo = NULL;
		pImageData = NULL;
	}

}GroupResultData;

typedef struct _GroupVideoFrameData
{
	DWORD	dwPackeType;
	DWORD	dwVideoType;
	DWORD	dwVideoDataLen;
	DWORD	dwVideoExInfoLen;
	PBYTE	pVideoData;
	char*	pszVideoExInfo;

	_GroupVideoFrameData()
	{
		pVideoData = NULL;
		pszVideoExInfo = NULL;
	}
}GroupVideoFrameData;

class CTestType
{
public:
	CTestType(char* szIP, CListCtrl* pListBox, DWORD dwListIndex, CString strSavePath, CWnd* pParent);
	~CTestType();

private:
	static DWORD WINAPI ProceThread(LPVOID pParam);
	static DWORD WINAPI ProceResultDataThread(LPVOID pParam);
	static DWORD WINAPI ProceVideoDataThread(LPVOID pParam);
	static int OnPlate(PVOID pUserData, DWORD dwCarID, LPCSTR pcPlateNo, LPCSTR pcAppendInfo, DWORD dwRecordType, DWORD64 dw64TimeMS);
	static int OnBigImage(PVOID pUserData, DWORD dwCarID,  WORD  wImgType, WORD  wWidth, WORD  wHeight, PBYTE pbPicData, DWORD dwImgDataLen, DWORD dwRecordType, DWORD64 dw64TimeMS);
	static int OnSmall(PVOID pUserData, DWORD dwCarID, WORD wWidth, WORD wHeight, PBYTE pbPicData, DWORD dwImgDataLen, DWORD dwRecordType, DWORD64 dwTimeMS);
	static int OnBinary(PVOID pUserData, DWORD dwCarID, WORD wWidth, WORD wHeight, PBYTE pbPicData, DWORD dwImgDataLen, DWORD dwRecordType, DWORD64 dwTimeMS);
	static int OnJpegFrame(PVOID pUserData, PBYTE pbImageData, DWORD dwImageDataLen, DWORD dwImageType, LPCSTR szImageExtInfo);
	static int OnH264Frame(PVOID pUserData, PBYTE pbVideoData, DWORD dwVideoDataLen, DWORD dwVideoType, LPCSTR szVideoExtInfo);

private:
	HVAPI_HANDLE_EX	m_hDevice;
	HANDLE	m_hProceThread;
	HANDLE	m_hProceResultDataThread;
	HANDLE	m_hProceVideoDataThread;
	DWORD   m_dwStatus;
	char	m_szIP[20];
	BOOL	m_fIsThreadExit;
	BOOL	m_fIsProceResultThreadExit;
	BOOL	m_fIsProceVideoThreadExit;
	CListCtrl* m_ListBox;
	DWORD m_dwIndex;
	CString m_strSavePath;
	BOOL m_fIsCanSave;
	CHvList<GroupResultData>	m_ResultList;
	CHvList<GroupVideoFrameData> m_VideoList;
	CRITICAL_SECTION	m_csVideoList;
	CRITICAL_SECTION	m_csList;
	float m_fJPEGFps;
	float m_fH264Fps;
	DWORD m_dwJPEGLastTick;
	DWORD m_dwH264LastTick;
	DWORD m_dwJPEGFrameCount;
	DWORD m_dwH264FrameCount;
	CWnd* m_pParent;
};

class CGroupTestDlg : public CDialog
{
	DECLARE_DYNAMIC(CGroupTestDlg)
public:
	CGroupTestDlg(CWnd* pParent = NULL);
	virtual ~CGroupTestDlg();
	enum {IDD = IDD_DIALOG8};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	afx_msg void OnCancel();
	DECLARE_MESSAGE_MAP()

private:
	HICON	m_hIcon;
	CWnd* m_pParentDlg;
	CIPAddressCtrl m_IPControll;
	CListBox m_ListBox;
	CListCtrl m_ListControll;
	CString m_SaveFilePath;
	CToolTipCtrl m_Messge;
	CTestType* m_TestType[20];
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton8();
protected:
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);

protected:
	virtual void OnOK();
	virtual void PostNcDestroy();

public:
	afx_msg void OnClose();
};

#endif
