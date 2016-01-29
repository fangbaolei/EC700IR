#ifndef _RESULTRECVTEST_H__
#define _RESULTRECVTEST_H__

#include "resource.h"
#include "HvDeviceNew.h"
#include "hvvartype.h"
#include <afxtempl.h>

using namespace HiVideo;

typedef struct _tag_Face_Info
{
	int nCount;
	int rcPlate[20][4];
}FACE_INFO;

typedef struct _ResultData
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
	DWORD dwResultType;
	int iRcplate[4];
	FACE_INFO cFaceInfo;

	_ResultData()
	{
		pcPlateNo = NULL;
		pcAppendInfo = NULL;
		pImageData = NULL;
	}

}ResultData;

class CResultRecvTestDlg : public CDialog
{
	DECLARE_DYNAMIC(CResultRecvTestDlg)
public:
	CResultRecvTestDlg(HVAPI_HANDLE_EX* phHandle, CWnd* pParent = NULL);
	virtual ~CResultRecvTestDlg();

	enum {IDD = IDD_DIALOG1};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	LRESULT OnProcResultData(WPARAM wParam, LPARAM lParam);
	LRESULT OnProcRecordBegin(WPARAM wParam, LPARAM lParam);
	LRESULT OnProcRecordEnd(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
private:
	HICON m_hIcon;
	HVAPI_HANDLE_EX* m_hDevice;
	PBYTE	m_pBestSnapshotBuffer;
	PBYTE	m_pLastSnapshotBuffer;
	int		m_iBigImageBufferSize;

	BOOL m_fIsSetPlateCallBack;
	BOOL m_fIsSetBigImageCallBack;
	BOOL m_fIsSetSmallImageCallBack;
	BOOL m_fIsSetBinaryCallBack;

	CHvList<ResultData>	m_ResultList;
	CRITICAL_SECTION m_csList;
	BOOL m_fIsQuite;
	HANDLE	m_hProceThread;
	DWORD m_dwTotleResult;
	BOOL  m_fIsSetRecordFlag;
	BOOL  m_fIsSetHistoryFlag;
	CString m_strFace;

public:
	BOOL m_fIsSaveRecord;
	CString m_strSavePath;
	DWORD64 dw64StartTime;
	DWORD64 dw64EndTime;
	DWORD	dwIndex;
	BOOL	m_fIsUsedEndTime;
	BOOL	m_fIsGetHistoryRecord;
	BOOL	m_fBeginCallBack;

private:
	static int OnBigImage(PVOID pUserData, DWORD dwCarID,  WORD  wImgType, WORD  wWidth, WORD  wHeight, PBYTE pbPicData, DWORD dwImgDataLen, DWORD dwRecordType, DWORD64 dw64TimeMS);
	static int OnPlate(PVOID pUserData, DWORD dwCarID, LPCSTR pcPlateNo, LPCSTR pcAppendInfo, DWORD dwRecordType, DWORD64 dw64TimeMS);
	static int OnSmall(PVOID pUserData, DWORD dwCarID, WORD wWidth, WORD wHeight, PBYTE pbPicData, DWORD dwImgDataLen, DWORD dwRecordType, DWORD64 dwTimeMS);
	static int OnBinary(PVOID pUserData, DWORD dwCarID, WORD wWidth, WORD wHeight, PBYTE pbPicData, DWORD dwImgDataLen, DWORD dwRecordType, DWORD64 dwTimeMS);
	static int OnGetherInfo(PVOID pUserData, LPCSTR pString, DWORD dwStrLen);
	static DWORD WINAPI ProceThread(LPVOID pParam);
	static int OnRecordBegin(PVOID pUserData, DWORD dwCarID);
	static int OnRecordEnd(PVOID pUserData, DWORD dwCarID);
	

	//新接口 结果回调
	static int OnRecord(	PVOID pUserData, 
		DWORD dwResultFlag,			
		DWORD dwResultType,		
		DWORD dwCarID,
		LPCSTR pcPlateNo,
		LPCSTR pcAppendInfo,
		DWORD64 dw64TimeMS,
		CImageInfo  pPlate,
		CImageInfo  pPlateBin,
		CImageInfo  pBestSnapshot,
		CImageInfo  pLastSnapshot,
		CImageInfo  pBeginCapture,
		CImageInfo  pBestCapture,
		CImageInfo  pLastCapture
		);
public:
	afx_msg void OnBnClickedButton7();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton6();
	afx_msg void OnBnClickedButton12();
	afx_msg void OnBnClickedButton11();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnClose();
	afx_msg void OnBnClickedButton15();
	afx_msg void OnBnClickedButton17();
	afx_msg void OnBnClickedButton18();
	afx_msg void OnBnClickedBtnPrtscr();
	afx_msg void OnBnClickedStartRecord();
	afx_msg void OnBnClickedStartHistory();
	afx_msg void OnBnClickedButtonGetimage();

private:
	CString m_parastring;
public:
	BOOL m_fIsGetweizang;
private:
	CString m_showvalue;
protected:
	bool m_fIsSetStringCallBack;

protected:
	virtual void OnOK();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void PostNcDestroy();
};

#endif
