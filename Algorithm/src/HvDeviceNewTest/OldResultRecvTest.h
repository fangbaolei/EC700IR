#ifndef _OLD_RESULT_RECVTEST_H__
#define _OLD_RESULT_RECVTEST_H__

#include "resource.h"
#include "HvDevice.h"
#include "HvVarType.h"

using namespace HiVideo;

typedef struct _RECORD_PACKET_TYPE
{
	DWORD	dwDataType;
	DWORD	dwDataLen;
	PBYTE	pData;
	char*	pszPlateInfo;

	_RECORD_PACKET_TYPE()
	{
		pData = NULL;
		pszPlateInfo = NULL;
	}
}RECORD_PACKET_TYPE;

class COldResultRecvTestDlg : public CDialog
{
	DECLARE_DYNAMIC(COldResultRecvTestDlg)
public:
	COldResultRecvTestDlg(HVAPI_HANDLE* phHandle, CWnd* pParent = NULL);
	virtual ~COldResultRecvTestDlg();
	enum {IDD = IDD_DIALOG1};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	LRESULT OnProcRecordData(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

private:
	HICON	m_hIcon;
	HVAPI_HANDLE* m_hDevice;
	CHvList<RECORD_PACKET_TYPE> m_RecordList;
	CRITICAL_SECTION m_csList;
	BOOL m_fIsQuit;
	BOOL m_fIsSetRecordCallback;
	HANDLE	m_hProceThread;
	DWORD	m_dwTotleResult;
	DWORD	m_dwLastStatus;
	DWORD	m_dwReconnectTimes;
	BOOL  m_fIsSetRecordFlag;
	BOOL  m_fIsSetHistoryFlag;

public:
	BOOL m_fIsSaveRecord;
	CString m_strSavePath;
	INT  m_iRedLightEnhanceFlag;
	INT	 m_iBrightness;
	INT  m_iHueThreshold;
	INT  m_CompressRate;
	DWORD64 dw64StartTime;
	DWORD64 dw64EndTime;
	DWORD	dwIndex;
	BOOL	m_fIsUsedEndTime;
	BOOL	m_fIsGetHistoryRecord;

private:
	static DWORD WINAPI ProceThread(LPVOID pParam);
	static INT OnRecord(PVOID pUserData,
		PBYTE pbResultPacket,
		DWORD dwPacketLen,
		DWORD dwRecordType,
		LPCSTR szResultInfo);
	static int OnGetherInfo(PVOID pUserData, LPCSTR pString, DWORD dwStrLen);
	void DrawPlate(char* pszPlate);
	void DrawRecordImage(RECORD_IMAGE_GROUP* pcRecordImage, BOOL fIsPeccancy, DWORD dwCarID, DWORD64 dw64TimeMS);
public:
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnBnClickedButton7();
	afx_msg void OnBnClickedButton11();
	afx_msg void OnBnClickedButton12();
	afx_msg void OnBnClickedButton15();
	afx_msg void OnBnClickedButton17();
	afx_msg void OnBnClickedButton18();
	afx_msg void OnBnClickedBtnPrtscr();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton6();
	afx_msg void OnBnClickedStartRecord();
	afx_msg void OnBnClickedButtonGetimage();
};

#endif

