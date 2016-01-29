#include "stdafx.h"
#include "ResultRecvTestDlg.h"
#include ".\resultrecvtestdlg.h"
#include "atlimage.h"
#include "SetSavePathDlg.h"
#include "SetEnhanceFlagDlg.h"
#include "SetGetHistoryRecordDlg.h"
#include "GetparamName.h"

using namespace Gdiplus;

#ifndef SAFE_DELETE
#define SAFE_DELETE(p) if(p) {delete[] p; p = NULL;}
#endif

#define WM_PROCE_RESULTDATA		(WM_USER + 1001)
#define WM_PROCE_RECORDBEGIN	(WM_USER + 1002)
#define WM_PROCE_RECORDEND		(WM_USER + 1003)


extern CListCtrl* g_pList;
extern DWORD g_dwCurrentConnectIndex;

HRESULT Yuv2Rgb(
				BYTE *pbDest,
				BYTE *pbSrc,
				int iSrcWidth,
				int iSrcHeight,
				int iBGRStride
				)
{
	bool fBottomUp = true;
	if (iSrcHeight < 0)
	{
		iSrcHeight = -iSrcHeight;
		fBottomUp = false;
	}
	int x, y;
	unsigned char *pY = reinterpret_cast< unsigned char* >(pbSrc);
	unsigned char *pCb = reinterpret_cast< unsigned char* >(pbSrc) + iSrcWidth * iSrcHeight;
	unsigned char *pCr = reinterpret_cast< unsigned char* >(pbSrc) + iSrcWidth * iSrcHeight + (iSrcWidth >> 1) * iSrcHeight;
	for (y = 0; y < iSrcHeight; y++)
		for (x = 0; x < iSrcWidth; x++)
		{
			int iY = *(pY + y * iSrcWidth + x);
			int iCb = *(pCb + y * (iSrcWidth >> 1) + (x >> 1));
			int iCr = *(pCr + y * (iSrcWidth >> 1) + (x >> 1));
			int iR = static_cast< int >(1.402 * (iCr - 128) + iY);
			int iG = static_cast< int >(-0.34414 * (iCb - 128) - 0.71414 * (iCr - 128) + iY);
			int iB = static_cast< int >(1.772 * (iCb - 128) + iY);
			if (iR > 255)
				iR = 255;
			if (iR < 0)
				iR = 0;
			if (iG > 255)
				iG = 255;
			if (iG < 0)
				iG = 0;
			if (iB > 255)
				iB = 255;
			if (iB < 0)
				iB = 0;
			if (fBottomUp)
			{
				pbDest[ (iSrcHeight - y - 1) * iBGRStride + x * 3 ] = iB;
				pbDest[ (iSrcHeight - y - 1) * iBGRStride + x * 3 + 1 ] = iG;
				pbDest[ (iSrcHeight - y - 1) * iBGRStride + x * 3 + 2 ] = iR;
			}
			else
			{
				pbDest[ y * iBGRStride + x * 3 ] = iB;
				pbDest[ y * iBGRStride + x * 3 + 1 ] = iG;
				pbDest[ y * iBGRStride + x * 3 + 2 ] = iR;
			}
		}
		return S_OK;
}

HFONT g_hFont;
static bool m_fisweizhang = false;

IMPLEMENT_DYNAMIC(CResultRecvTestDlg, CDialog)

CResultRecvTestDlg::CResultRecvTestDlg(HVAPI_HANDLE_EX* phHandle, CWnd* pParent /* = NULL */)
: CDialog(CResultRecvTestDlg::IDD, pParent)
, m_parastring(_T(""))
, m_fIsGetweizang(FALSE)
, m_showvalue(_T(""))
, m_fIsSetStringCallBack(false)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hDevice = phHandle;
	m_pBestSnapshotBuffer = NULL;
	m_pLastSnapshotBuffer = NULL;
	m_iBigImageBufferSize = 0;

	m_fIsSetPlateCallBack = FALSE;
	m_fIsSetBigImageCallBack = FALSE;
	m_fIsSetSmallImageCallBack = FALSE;
	m_fIsSetBinaryCallBack = FALSE;
	m_fIsSaveRecord = FALSE;
	m_strSavePath = "";

	m_fIsSetHistoryFlag = FALSE;
	m_fIsSetRecordFlag = FALSE;
	m_fBeginCallBack = FALSE;

	InitializeCriticalSection(&m_csList);
	m_fIsQuite = FALSE;
	m_hProceThread = CreateThread(NULL, 0, ProceThread, this, 0, NULL);
}

CResultRecvTestDlg::~CResultRecvTestDlg()
{
	if(m_pBestSnapshotBuffer)
	{
		delete[] m_pBestSnapshotBuffer;
	}
	if(m_pLastSnapshotBuffer)
	{
		delete[] m_pLastSnapshotBuffer;
	}

	HVAPI_SetCallBackEx(*m_hDevice, NULL, NULL, 0, CALLBACK_TYPE_RECORD_PLATE, NULL);
	HVAPI_SetCallBackEx(*m_hDevice, NULL, NULL, 0, CALLBACK_TYPE_STRING, NULL);
	if(m_hProceThread)
	{
		m_fIsQuite= TRUE;
		int iWaitTimes = 0;
		int MAX_WAIT_TIME = 8;
		while(WaitForSingleObject(m_hProceThread, 500) == WAIT_TIMEOUT
			&& iWaitTimes < MAX_WAIT_TIME)
		{
			iWaitTimes++;
		}
		if(iWaitTimes >= MAX_WAIT_TIME)
		{
			TerminateThread(m_hProceThread, 0);
		}
		CloseHandle(m_hProceThread);
		m_hProceThread = NULL;
	}

	while (!m_ResultList.IsEmpty())
	{
		ResultData cTmpInfo = m_ResultList.RemoveHead();
		if (cTmpInfo.pcPlateNo)
		{
			delete [] cTmpInfo.pcPlateNo;
			cTmpInfo.pcPlateNo = NULL;
		}

		if(cTmpInfo.pcAppendInfo)
		{
			delete[] cTmpInfo.pcAppendInfo;
			cTmpInfo.pcAppendInfo = NULL;
		}

		if(cTmpInfo.pImageData)
		{
			delete[] cTmpInfo.pImageData;
			cTmpInfo.pImageData = NULL;
		}
	}
}

void CResultRecvTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CResultRecvTestDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON7, OnBnClickedButton7)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON5, OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON6, OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON12, OnBnClickedButton12)
	ON_BN_CLICKED(IDC_BUTTON11, OnBnClickedButton11)
	ON_BN_CLICKED(IDC_BUTTON17, OnBnClickedButton17)
	ON_BN_CLICKED(IDC_BUTTON18, OnBnClickedButton18)
	ON_BN_CLICKED(IDC_START_RECORD , OnBnClickedStartRecord)
	ON_MESSAGE(WM_PROCE_RESULTDATA, OnProcResultData)
	ON_MESSAGE(WM_PROCE_RECORDBEGIN, OnProcRecordBegin)
	ON_MESSAGE(WM_PROCE_RECORDEND, OnProcRecordEnd)
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON15, OnBnClickedButton15)
	ON_BN_CLICKED(IDC_BTN_PRTSCR, OnBnClickedBtnPrtscr)
	ON_BN_CLICKED(IDC_START_HISTORY ,OnBnClickedStartHistory)
	ON_BN_CLICKED(IDC_BUTTON_GETIMAGE, OnBnClickedButtonGetimage)
	

END_MESSAGE_MAP()

BOOL CResultRecvTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_dwTotleResult = 0;
	g_hFont = CreateFont(50, 26, 0, 0, FW_HEAVY, false, false, false,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH, "宋体");
	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);
	MoveWindow((GetSystemMetrics(SM_CXSCREEN)-1100)>>1,
		(GetSystemMetrics(SM_CYSCREEN)-850)>>1, 1100, 850, TRUE);
	::MoveWindow(GetDlgItem(IDC_STATIC_BEST)->GetSafeHwnd(), 5, 5, 640, 400, TRUE);
	::MoveWindow(GetDlgItem(IDC_STATIC_LAST)->GetSafeHwnd(), 5, 410, 640, 400, TRUE);
	::MoveWindow(GetDlgItem(IDC_EDIT_RECT)->GetSafeHwnd(), 955, 350, 130, 120, TRUE);
	::MoveWindow(GetDlgItem(IDC_EDIT_FACE)->GetSafeHwnd(), 955, 470, 130, 300, TRUE);
	::MoveWindow(GetDlgItem(IDC_STATIC_SMALL)->GetSafeHwnd(), 650, 5, 300, 70, TRUE);
	::MoveWindow(GetDlgItem(IDC_STATIC_BINARY)->GetSafeHwnd(), 650, 80, 300, 70, TRUE);
	::MoveWindow(GetDlgItem(IDC_STATIC_PLATE)->GetSafeHwnd(), 650, 155, 300, 70, TRUE);
	::MoveWindow(GetDlgItem(IDC_EDIT1)->GetSafeHwnd(), 650, 230, 300, 430, TRUE);
	::MoveWindow(GetDlgItem(IDC_EDIT2)->GetSafeHwnd(), 650, 665, 300, 150, TRUE);
	::MoveWindow(GetDlgItem(IDC_BUTTON1)->GetSafeHwnd(), 955, 5, 130, 30, TRUE);
	::MoveWindow(GetDlgItem(IDC_BUTTON5)->GetSafeHwnd(), 955, 35, 130, 30, TRUE);
	::MoveWindow(GetDlgItem(IDC_BUTTON6)->GetSafeHwnd(), 955, 65, 130, 30, TRUE);
	::MoveWindow(GetDlgItem(IDC_BUTTON7)->GetSafeHwnd(), 955, 95, 130, 30, TRUE);
	::MoveWindow(GetDlgItem(IDC_BUTTON11)->GetSafeHwnd(), 955, 125, 130, 30, TRUE);
	::MoveWindow(GetDlgItem(IDC_BUTTON12)->GetSafeHwnd(), 955, 155, 130, 30, TRUE);
	::MoveWindow(GetDlgItem(IDC_BUTTON15)->GetSafeHwnd(), 955, 185, 130, 30, TRUE);
	::MoveWindow(GetDlgItem(IDC_BUTTON17)->GetSafeHwnd(), 955, 215, 130, 30, TRUE);
	::MoveWindow(GetDlgItem(IDC_BUTTON18)->GetSafeHwnd(), 955, 245, 130, 30, TRUE);
	::MoveWindow(GetDlgItem(IDC_BUTTON_GETIMAGE)->GetSafeHwnd(), 955, 275, 130, 30, TRUE);

	::MoveWindow(GetDlgItem(IDC_START_RECORD)->GetSafeHwnd(), 955, 305, 130, 30, TRUE);
	::MoveWindow(GetDlgItem(IDC_START_HISTORY)->GetSafeHwnd(), 955, 335, 130, 30, TRUE);

	::MoveWindow(GetDlgItem(IDC_BTN_PRTSCR)->GetSafeHwnd(), 955, 275, 130, 30, TRUE);
	::MoveWindow(GetDlgItem(IDC_EDIT4)->GetSafeHwnd(), 955, 775, 130, 40, TRUE);


	CButton* pBut =(CButton*)GetDlgItem(IDC_BTN_PRTSCR);
	pBut->ShowWindow(FALSE);

	SetTimer(1002, 1000, NULL);
	return TRUE;
}


DWORD WINAPI CResultRecvTestDlg::ProceThread(LPVOID pParam)
{
	if(pParam == NULL)
	{
		return -1;
	}
	CResultRecvTestDlg* pDlg = (CResultRecvTestDlg*)pParam;
	while(!pDlg->m_fIsQuite)
	{
		if(pDlg->m_ResultList.IsEmpty())
		{
			Sleep(100);
			continue;
		}
		ResultData TmpResult;
		EnterCriticalSection(&pDlg->m_csList);
		TmpResult = pDlg->m_ResultList.RemoveHead();
		LeaveCriticalSection(&pDlg->m_csList);
		
		pDlg->SendMessage(WM_PROCE_RESULTDATA, 0, (LPARAM)&TmpResult);

		if(TmpResult.pcPlateNo)
		{
			delete[] TmpResult.pcPlateNo;
			TmpResult.pcPlateNo = NULL;
		}

		if(TmpResult.pcAppendInfo)
		{
			delete[] TmpResult.pcAppendInfo;
			TmpResult.pcAppendInfo = NULL;
		}

		if(TmpResult.pImageData)
		{
			delete[] TmpResult.pImageData;
			TmpResult.pImageData = NULL;
		}
	}
	return 0;
}

int CResultRecvTestDlg::OnRecordBegin(PVOID pUserData, DWORD dwCarID)
{
	if(pUserData == NULL)
	{
		return -1;
	}
	CResultRecvTestDlg* pDlg = (CResultRecvTestDlg*)pUserData;
	pDlg->SendMessage(WM_PROCE_RECORDBEGIN, 0, (LPARAM)dwCarID);

	return 0;
}

int CResultRecvTestDlg::OnRecordEnd(PVOID pUserData, DWORD dwCarID)
{
	if(pUserData == NULL)
	{
		return -1;
	}
	CResultRecvTestDlg* pDlg = (CResultRecvTestDlg*)pUserData;
	pDlg->SendMessage(WM_PROCE_RECORDEND, 0, (LPARAM)dwCarID);

	return 0;
}

int CResultRecvTestDlg::OnPlate(PVOID pUserData, DWORD dwCarID, LPCSTR pcPlateNo, LPCSTR pcAppendInfo, DWORD dwRecordType, DWORD64 dw64TimeMS)
{
	if(pUserData == NULL)
	{
		return -1;
	}
	CResultRecvTestDlg* pDlg = (CResultRecvTestDlg*)pUserData;
	if(pDlg->m_ResultList.IsFull())
	{
		return -1;
	}

	ResultData NewResultData;
	NewResultData.iDataType = 1;
	NewResultData.dwCarID = dwCarID;
	NewResultData.dw64TimeMS = dw64TimeMS;
	NewResultData.dwResultType = dwRecordType;
	int iPlateStrLen = (int)strlen(pcPlateNo);
	int iAppendStrLen = (int)strlen(pcAppendInfo);
	NewResultData.pcPlateNo = new char[iPlateStrLen + 1];
	NewResultData.pcAppendInfo = new char[iAppendStrLen + 1];
	if(NewResultData.pcPlateNo == NULL || NewResultData.pcAppendInfo == NULL)
	{
		SAFE_DELETE(NewResultData.pcPlateNo);
		SAFE_DELETE(NewResultData.pcAppendInfo);
		return -1;
	}
	memcpy(NewResultData.pcPlateNo, pcPlateNo, iPlateStrLen);
	NewResultData.pcPlateNo[iPlateStrLen] = '\0';
	memcpy(NewResultData.pcAppendInfo, pcAppendInfo, iAppendStrLen);
	NewResultData.pcAppendInfo[iAppendStrLen] = '\0';
	EnterCriticalSection(&pDlg->m_csList);
	pDlg->m_ResultList.AddTail(NewResultData);
	LeaveCriticalSection(&pDlg->m_csList);
	if(strstr(pcAppendInfo,"违章:是"))
		m_fisweizhang = true ;
	else
		m_fisweizhang = false;
	return 0;
}

int CResultRecvTestDlg::OnBigImage(PVOID pUserData, DWORD dwCarID,  WORD  wImgType, WORD  wWidth, WORD  wHeight, PBYTE pbPicData, DWORD dwImgDataLen, DWORD dwRecordType, DWORD64 dw64TimeMS)
{
	if(pUserData == NULL)
	{
		return -1;
	}
	CResultRecvTestDlg* pDlg = (CResultRecvTestDlg*)pUserData;
	if(pDlg->m_ResultList.IsFull())
	{
		return -1;
	}
	ResultData NewResultData;
	NewResultData.iDataType = 2;
	NewResultData.dwCarID = dwCarID;
	NewResultData.dw64TimeMS = dw64TimeMS;
	NewResultData.dwImageType = wImgType;
	NewResultData.wImageWidth = wWidth;
	NewResultData.wImageHeight = wHeight;
	NewResultData.dwImageDataLen = dwImgDataLen;
	NewResultData.dwResultType = dwRecordType;
	NewResultData.pImageData = new BYTE[dwImgDataLen];
	int iBuffLen = sizeof(NewResultData.iRcplate);
	HVAPI_GetExtensionInfoEx(*pDlg->m_hDevice, wImgType, NewResultData.iRcplate, &iBuffLen);
	iBuffLen = NewResultData.iRcplate[0];
	iBuffLen = NewResultData.iRcplate[1];
	iBuffLen = NewResultData.iRcplate[2];
	iBuffLen = NewResultData.iRcplate[3];

	FACE_INFO FaceInfo;
	iBuffLen = sizeof(FaceInfo);
	HVAPI_GetExtensionInfoEx(*pDlg->m_hDevice, wImgType + 5, &FaceInfo, &iBuffLen);
	NewResultData.cFaceInfo = FaceInfo;
	

	if(NewResultData.pImageData == NULL)
	{
		return -1;
	}
	memcpy(NewResultData.pImageData, pbPicData, dwImgDataLen);
	EnterCriticalSection(&pDlg->m_csList);
	pDlg->m_ResultList.AddTail(NewResultData);
	LeaveCriticalSection(&pDlg->m_csList);

	if(pDlg->m_fIsSaveRecord && pDlg->m_strSavePath != "")
	{
		CString strSaveFileName;
		CTime cTime(dw64TimeMS/1000);
		unsigned int MS = (unsigned int)dw64TimeMS%1000;
		CString Ms;
		Ms.Format("%d",MS);
		//zhanghz，在大图回调时打印	取得真正的图片时间
		if(MS < 100)
		{
			Ms.Insert(0,"0");
			if(MS < 10)
			Ms.Insert(0,"0");
		}
		CString pathWZ;
		if(m_fisweizhang == true)
			pathWZ = "违章结果\\";
		else
			pathWZ = "非违章结果\\";
		switch(wImgType)
		{
		case RECORD_BIGIMG_BEST_SNAPSHOT:
			strSaveFileName.Format("%s%s%s%s_%d_01.jpg", pDlg->m_strSavePath.GetBuffer(),pathWZ.GetBuffer(),
			cTime.Format("%Y%m%d%H%M%S"), Ms.GetBuffer(), dwCarID);
		break;
		case RECORD_BIGIMG_LAST_SNAPSHOT:
			strSaveFileName.Format("%s%s%s%s_%d_02.jpg", pDlg->m_strSavePath.GetBuffer(),pathWZ.GetBuffer(),
			cTime.Format("%Y%m%d%H%M%S"), Ms.GetBuffer(), dwCarID);
		break;
		case RECORD_BIGIMG_BEGIN_CAPTURE:
			strSaveFileName.Format("%s%s%s%s_%d_03.jpg", pDlg->m_strSavePath.GetBuffer(),pathWZ.GetBuffer(),
		cTime.Format("%Y%m%d%H%M%S"), Ms.GetBuffer(), dwCarID);
		break;
		case RECORD_BIGIMG_BEST_CAPTURE:
			strSaveFileName.Format("%s%s%s%s_%d_04.jpg", pDlg->m_strSavePath.GetBuffer(),pathWZ.GetBuffer(),
		cTime.Format("%Y%m%d%H%M%S"), Ms.GetBuffer(), dwCarID);
		break;
		case RECORD_BIGIMG_LAST_CAPTURE:
			strSaveFileName.Format("%s%s%s%s_%d_05.jpg", pDlg->m_strSavePath.GetBuffer(),pathWZ.GetBuffer(),
		cTime.Format("%Y%m%d%H%M%S"), Ms.GetBuffer(), dwCarID);
		break;
		}
		FILE* fp = fopen(strSaveFileName.GetBuffer(), "wb");
		if(fp)
		{
		fwrite(pbPicData, dwImgDataLen, 1, fp);
		fclose(fp);
		}
	}
	return 0;
}

INT CResultRecvTestDlg::OnSmall(PVOID pUserData, DWORD dwCarID, WORD wWidth, WORD wHeight, PBYTE pbPicData, DWORD dwImgDataLen, DWORD dwRecordType, DWORD64 dwTimeMS)
{
	if(pUserData == NULL)
	{
		return -1;
	}
	CResultRecvTestDlg* pDlg = (CResultRecvTestDlg*)pUserData;
	if(pDlg->m_ResultList.IsFull())
	{
		return -1;
	}
	ResultData NewResultData;
	NewResultData.iDataType = 3;
	NewResultData.dwCarID = dwCarID;
	NewResultData.dw64TimeMS = dwTimeMS;
	NewResultData.dwImageType = 0;
	NewResultData.wImageWidth = wWidth;
	NewResultData.wImageHeight = wHeight;
	NewResultData.dwResultType = dwRecordType;
	NewResultData.dwImageDataLen = dwImgDataLen;
	NewResultData.pImageData = new BYTE[dwImgDataLen];
	if(NewResultData.pImageData == NULL)
	{
		return -1;
	}
	memcpy(NewResultData.pImageData, pbPicData, dwImgDataLen);
	EnterCriticalSection(&pDlg->m_csList);
	pDlg->m_ResultList.AddTail(NewResultData);
	LeaveCriticalSection(&pDlg->m_csList);
	return 0;
}

INT CResultRecvTestDlg::OnBinary(PVOID pUserData, DWORD dwCarID, WORD wWidth, WORD wHeight, PBYTE pbPicData, DWORD dwImgDataLen, DWORD dwRecordType, DWORD64 dwTimeMS)
{
	if(pUserData == NULL)
	{
		return -1;
	}
	CResultRecvTestDlg* pDlg = (CResultRecvTestDlg*)pUserData;
	if(pDlg->m_ResultList.IsFull())
	{
		return -1;
	}
	ResultData NewResultData;
	NewResultData.iDataType = 4;
	NewResultData.dwCarID = dwCarID;
	NewResultData.dw64TimeMS = dwTimeMS;
	NewResultData.dwImageType = 0;
	NewResultData.wImageWidth = wWidth;
	NewResultData.wImageHeight = wHeight;
	NewResultData.dwResultType = dwRecordType;
	NewResultData.dwImageDataLen = dwImgDataLen;
	NewResultData.pImageData = new BYTE[dwImgDataLen];
	if(NewResultData.pImageData == NULL)
	{
		return -1;
	}
	memcpy(NewResultData.pImageData, pbPicData, dwImgDataLen);
	EnterCriticalSection(&pDlg->m_csList);
	pDlg->m_ResultList.AddTail(NewResultData);
	LeaveCriticalSection(&pDlg->m_csList);

	if(pDlg->m_fIsSaveRecord && pDlg->m_strSavePath != "")
	{
		CString strSaveFileName;
		CTime cTime(dwTimeMS/1000);
		unsigned int MS = (unsigned int)dwTimeMS%1000;
		CString Ms;
		Ms.Format("%d",MS);
		//zhanghz，为了把二值图转BMP格式
		if(MS < 100)
		{
			Ms.Insert(0,"0");
			if(MS < 10)
			Ms.Insert(0,"0");
		}
		int dwlen=512;
		PBYTE pb_Data = new BYTE[dwlen];
		HVAPIUTILS_BinImageToBitmapEx(pbPicData, pb_Data, &dwlen);
		CString pathWZ;
		if(m_fisweizhang == true)
			pathWZ = "违章结果\\";
		else
			pathWZ = "非违章结果\\";
		strSaveFileName.Format("%s%s%s%s_%d_00.bmp", pDlg->m_strSavePath.GetBuffer(),pathWZ.GetBuffer(),
			cTime.Format("%Y%m%d%H%M%S"), Ms.GetBuffer(), dwCarID);

		FILE* fp = fopen(strSaveFileName.GetBuffer(), "wb");
		if(fp)
		{
		fwrite(pb_Data, dwlen, 1, fp);
		fclose(fp);
		}
		delete[] pb_Data;
	}

	return 0;
}

void CResultRecvTestDlg::OnBnClickedButton7()
{
	if(m_hDevice == NULL)
	{
		MessageBox("设置结果接收回调失败", "ERROR", MB_OK|MB_ICONERROR);
		return;
	}
	GetDlgItem(IDC_BUTTON7)->EnableWindow(FALSE);
	if(m_fIsSetPlateCallBack)
	{	
		::Sleep(1000);
		if(HVAPI_SetCallBackEx(*m_hDevice, NULL, NULL, 0, CALLBACK_TYPE_RECORD_PLATE, NULL) != S_OK)
		{
			MessageBox("取消结果接收回调失败", "ERROR", MB_OK|MB_ICONERROR);
			GetDlgItem(IDC_BUTTON7)->EnableWindow(TRUE);
			return;
		}

		CString str;
		//DWORD dwStatus = CONN_STATUS_UNKNOWN;
		//HVAPI_GetConnStatusEx(*m_hDevice, STREAM_TYPE_RECORD, &dwStatus);
		//str.Format("dwStatus = %0x",dwStatus);

		GetDlgItem(IDC_BUTTON18)->GetWindowText(str);

		if(str == "取消状态信息回调")
		{
			HVAPI_SetCallBackEx(*m_hDevice, OnGetherInfo, this, 0, CALLBACK_TYPE_STRING, NULL);
		//	m_fIsSetStringCallBack = true;		
		}

		m_fIsSetPlateCallBack = FALSE;
		//if(HVAPI_SetCallBackEx(*m_hDevice, NULL, NULL, 0, CALLBACK_TYPE_STRING, NULL) != S_OK)
		//{
		//	MessageBox("取消交通信息采集回调失败...");
		//}

		HVAPI_SetCallBackEx(*m_hDevice, NULL, NULL, 0, CALLBACK_TYPE_RECORD_INFOBEGIN, NULL);
		HVAPI_SetCallBackEx(*m_hDevice, NULL, NULL, 0, CALLBACK_TYPE_RECORD_INFOEND, NULL);
		::Sleep(1000);

				
		
		SetDlgItemText(IDC_BUTTON7, "设置车牌回调");
	}
	else
	{	
		::Sleep(1000);
		if(HVAPI_SetCallBackEx(*m_hDevice, OnPlate, this, 0, CALLBACK_TYPE_RECORD_PLATE, NULL) != S_OK)
		{
			MessageBox("设置结果接收回调失败", "ERROR", MB_OK|MB_ICONERROR);
			GetDlgItem(IDC_BUTTON7)->EnableWindow(TRUE);
			return;
		}
		m_fIsSetPlateCallBack = TRUE;
		//if(HVAPI_SetCallBackEx(*m_hDevice, OnGetherInfo, this, 0, CALLBACK_TYPE_STRING, NULL) != S_OK)
		//{
		//	MessageBox("设置交通信息采集回调失败...");
		//}
		//if (!m_fBeginCallBack)
		//{

			HVAPI_SetCallBackEx(*m_hDevice, OnRecordBegin, this, 0, CALLBACK_TYPE_RECORD_INFOBEGIN, NULL);
			HVAPI_SetCallBackEx(*m_hDevice, OnRecordEnd, this, 0, CALLBACK_TYPE_RECORD_INFOEND, NULL);
		//	m_fBeginCallBack = TRUE;
		//}

		::Sleep(1000);
		SetDlgItemText(IDC_BUTTON7, "取消车牌回调");
	}
	GetDlgItem(IDC_BUTTON7)->EnableWindow(TRUE);
}

void CResultRecvTestDlg::OnBnClickedButton1()
{
	if(*m_hDevice == NULL)
	{
		MessageBox("设置大图接收回调失败", "ERROR", MB_OK|MB_ICONERROR);
		return;
	}
	GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);
	if(m_fIsSetBigImageCallBack)
	{
		if(HVAPI_SetCallBackEx(*m_hDevice, NULL, NULL, 0, CALLBACK_TYPE_RECORD_BIGIMAGE, NULL) != S_OK)
		{
			MessageBox("取消大图接收回调失败", "ERROR", MB_OK|MB_ICONERROR);
			GetDlgItem(IDC_BUTTON1)->EnableWindow(TRUE);
			return;
		}
		m_fIsSetBigImageCallBack = FALSE;
		SetDlgItemText(IDC_BUTTON1, "设置大图回调");
	}
	else
	{
		if(HVAPI_SetCallBackEx(*m_hDevice, OnBigImage, this, 0, CALLBACK_TYPE_RECORD_BIGIMAGE, NULL) != S_OK)
		{
			MessageBox("设置大图接收回调失败", "ERROR", MB_OK|MB_ICONERROR);
			GetDlgItem(IDC_BUTTON1)->EnableWindow(TRUE);
			return;
		}
		m_fIsSetBigImageCallBack = TRUE;
		SetDlgItemText(IDC_BUTTON1, "取消大图回调");
		//if (!m_fBeginCallBack)
		//{
		//	HVAPI_SetCallBackEx(*m_hDevice, OnRecordBegin, this, 0, CALLBACK_TYPE_RECORD_INFOBEGIN, NULL);
		//	HVAPI_SetCallBackEx(*m_hDevice, OnRecordEnd, this, 0, CALLBACK_TYPE_RECORD_INFOEND, NULL);
		//	m_fBeginCallBack = TRUE;
		//}
	}
	
	GetDlgItem(IDC_BUTTON1)->EnableWindow(TRUE);
	m_iBigImageBufferSize = 1024 * 1024;
	if(m_pBestSnapshotBuffer == NULL)
	{
		m_pBestSnapshotBuffer = new BYTE[m_iBigImageBufferSize];
	}
	if(m_pLastSnapshotBuffer == NULL)
	{
		m_pLastSnapshotBuffer = new BYTE[m_iBigImageBufferSize];
	}
}

void CResultRecvTestDlg::OnBnClickedButton5()
{
	if(m_hDevice == NULL)
	{
		MessageBox("设置小图接收回调失败", "ERROR", MB_OK|MB_ICONERROR);
		return;
	}
	GetDlgItem(IDC_BUTTON5)->EnableWindow(FALSE);
	if(m_fIsSetSmallImageCallBack)
	{
		if(HVAPI_SetCallBackEx(*m_hDevice, NULL, NULL, 0, CALLBACK_TYPE_RECORD_SMALLIMAGE, NULL) != S_OK)
		{
			MessageBox("取消小图接收回调失败", "ERROR", MB_OK|MB_ICONERROR);
			GetDlgItem(IDC_BUTTON5)->EnableWindow(TRUE);
			return;
		}
		m_fIsSetSmallImageCallBack = FALSE;
		SetDlgItemText(IDC_BUTTON5, "设置小图回调");
	}
	else
	{
		if(HVAPI_SetCallBackEx(*m_hDevice, OnSmall, this, 0, CALLBACK_TYPE_RECORD_SMALLIMAGE, NULL) != S_OK)
		{
			MessageBox("设置小图接收回调失败", "ERROR", MB_OK|MB_ICONERROR);
			GetDlgItem(IDC_BUTTON5)->EnableWindow(TRUE);
			return;
		}
		m_fIsSetSmallImageCallBack = TRUE;
		SetDlgItemText(IDC_BUTTON5, "取消小图回调");
		//if (!m_fBeginCallBack)
		//{
		//	HVAPI_SetCallBackEx(*m_hDevice, OnRecordBegin, this, 0, CALLBACK_TYPE_RECORD_INFOBEGIN, NULL);
		//	HVAPI_SetCallBackEx(*m_hDevice, OnRecordEnd, this, 0, CALLBACK_TYPE_RECORD_INFOEND, NULL);
		//	m_fBeginCallBack = TRUE;
		//}
	}
	GetDlgItem(IDC_BUTTON5)->EnableWindow(TRUE);
}

void CResultRecvTestDlg::OnBnClickedButton6()
{
	if(m_hDevice == NULL)
	{
		MessageBox("设置二值图接收回调失败", "ERROR", MB_OK|MB_ICONERROR);
		return;
	}
	GetDlgItem(IDC_BUTTON6)->EnableWindow(FALSE);
	if(m_fIsSetBinaryCallBack)
	{
		if(HVAPI_SetCallBackEx(*m_hDevice, NULL, NULL, 0, CALLBACK_TYPE_RECORD_BINARYIMAGE, NULL) != S_OK)
		{
			MessageBox("取消二值图接收回调失败", "ERROR", MB_OK|MB_ICONERROR);
			GetDlgItem(IDC_BUTTON6)->EnableWindow(TRUE);
			return;
		}
		m_fIsSetBinaryCallBack = FALSE;
		SetDlgItemText(IDC_BUTTON6, "设置二值图回调");
	}
	else
	{
		if(HVAPI_SetCallBackEx(*m_hDevice, OnBinary, this, 0, CALLBACK_TYPE_RECORD_BINARYIMAGE, NULL) != S_OK)
		{
			MessageBox("设置二值图接收回调失败", "ERROR", MB_OK|MB_ICONERROR);
			GetDlgItem(IDC_BUTTON6)->EnableWindow(TRUE);
			return;
		}
		m_fIsSetBinaryCallBack = TRUE;
		SetDlgItemText(IDC_BUTTON6, "取消二值图回调");
		//if (!m_fBeginCallBack)
		//{
		//	HVAPI_SetCallBackEx(*m_hDevice, OnRecordBegin, this, 0, CALLBACK_TYPE_RECORD_INFOBEGIN, NULL);
		//	HVAPI_SetCallBackEx(*m_hDevice, OnRecordEnd, this, 0, CALLBACK_TYPE_RECORD_INFOEND, NULL);
		//	m_fBeginCallBack = TRUE;
		//}
	}
	GetDlgItem(IDC_BUTTON6)->EnableWindow(TRUE);
}

void CResultRecvTestDlg::OnBnClickedButton12()
{
	CSetSavePathDlg Dlg(m_hDevice, this);
	Dlg.DoModal();
}

void CResultRecvTestDlg::OnBnClickedButton11()
{
	if(m_hDevice == NULL)
	{
		MessageBox("未连接识别器", "ERROR", MB_OK|MB_ICONERROR);
		return;
	}
	CSetEnahnceFlagDlg Dlg(m_hDevice, this);
	Dlg.DoModal();
}

void CResultRecvTestDlg::OnTimer(UINT nIDEvent)
{
	if(*m_hDevice && nIDEvent == 1002)
	{
		DWORD dwStatus = CONN_STATUS_UNKNOWN;
		DWORD dwReConnectTimes = 0;
		char szTmpInfo[20] = {0};
		HVAPI_GetConnStatusEx(*m_hDevice, STREAM_TYPE_RECORD, &dwStatus);
		switch(dwStatus)
		{
		case CONN_STATUS_NORMAL:
			sprintf(szTmpInfo, "%s", "连接");
			break;
		case CONN_STATUS_RECONN:
			sprintf(szTmpInfo, "%s", "重连");
			break;
		case CONN_STATUS_RECVDONE:
			sprintf(szTmpInfo, "%s", "接收完成");
			break;
		case CONN_STATUS_DISCONN:
			sprintf(szTmpInfo, "%s", "断开");
			break;
		default:
			sprintf(szTmpInfo, "%s", "未知");
			break;
		}
		SetWindowText(szTmpInfo);
		HVAPI_GetReConnectTimesEx(*m_hDevice, STREAM_TYPE_RECORD, &dwReConnectTimes, FALSE);
		sprintf(szTmpInfo, "%d", dwReConnectTimes);
		g_pList->SetItemText(g_dwCurrentConnectIndex, 4, szTmpInfo);
	}
	CDialog::OnTimer(nIDEvent);
}


LRESULT CResultRecvTestDlg::OnProcResultData(WPARAM wParam, LPARAM lParam)
{
	if(lParam == NULL)
	{
		return E_FAIL;
	}

	ResultData* pTmpResult = (ResultData*)lParam;
	switch(pTmpResult->iDataType)
	{
	case 1:
		{
			COLORREF bgColor, wordColor;
			if(strstr(pTmpResult->pcPlateNo, "无车牌"))
			{	
				bgColor = RGB(255, 0, 0);
				wordColor = RGB(255, 255, 255);
			}
			else if(strstr(pTmpResult->pcPlateNo, "蓝"))
			{
				bgColor = RGB(0, 0, 255);
				wordColor = RGB(255, 255, 255);
			}
			else if(strstr(pTmpResult->pcPlateNo, "黄"))
			{
				bgColor = RGB(255, 255, 0);
				wordColor = RGB(0, 0, 0);
			}
			else if(strstr(pTmpResult->pcPlateNo, "黑"))
			{
				bgColor = RGB(0, 0, 0);
				wordColor = RGB(255, 255, 255);
			}
			else if(strstr(pTmpResult->pcPlateNo, "白"))
			{
				bgColor = RGB(255, 255, 255);
				wordColor = RGB(0, 0, 0);
			}
			else
			{
				bgColor = RGB(0, 0, 255);
				wordColor = RGB(255, 255, 255);
			}
			HWND hTmpHwnd = GetDlgItem(IDC_STATIC_BINARY)->GetSafeHwnd();
			HDC hDC = ::GetDC(hTmpHwnd);
			HPEN hPen;
			HBRUSH hBrush;
			hPen = CreatePen(PS_SOLID, 1, bgColor);
			hBrush = CreateSolidBrush(bgColor);
			SelectObject(hDC, hPen);
			SelectObject(hDC, hBrush);
			SelectObject(hDC, g_hFont);
			Rectangle(hDC, 0, 0, 300, 70);
			DeleteObject(hPen);
			DeleteObject(hBrush);
			SetBkMode(hDC, 3);
			SetTextColor(hDC, wordColor);
			TextOut(hDC, 5, 10, pTmpResult->pcPlateNo, (int)strlen(pTmpResult->pcPlateNo));
			::ReleaseDC(hTmpHwnd, hDC);

			m_dwTotleResult++;
			hTmpHwnd = GetDlgItem(IDC_STATIC_PLATE)->GetSafeHwnd();
			hDC = ::GetDC(hTmpHwnd);
			hPen = CreatePen(PS_SOLID, 1, RGB(5, 5, 5));
			hBrush = CreateSolidBrush(RGB(5, 5, 5));
			SelectObject(hDC, hPen);
			SelectObject(hDC, hBrush);
			SelectObject(hDC, g_hFont);
			Rectangle(hDC, 0, 0, 300, 70);
			DeleteObject(hPen);
			DeleteObject(hBrush);
			SetBkMode(hDC, 3);
			SetTextColor(hDC, RGB(0, 192, 0));
			char szTmpeInfo[20] = {0};
			sprintf(szTmpeInfo, "%d", m_dwTotleResult);
			TextOut(hDC, 5, 10, szTmpeInfo, (int)strlen(szTmpeInfo));
			::ReleaseDC(hTmpHwnd, hDC);

			char* pszPlateInfo = new char [1024 << 5];
			if(pszPlateInfo == NULL)
			{
				break;
			}
			memset(pszPlateInfo, 0, (1024<<5));
			HVAPIUTILS_ParsePlateXmlStringEx(pTmpResult->pcAppendInfo, pszPlateInfo, (1024<<5));
			CString strAppend;

			if(!m_parastring.IsEmpty())
			{
				DWORD dwBufLen = 500;
				char* pszPlateBuf = new char[500];
				if(strstr(m_parastring , ",")&& strstr(m_parastring , "TimeLow")&& strstr(m_parastring , "TimeHigh"))
				{
					DWORD TIMELOW,TIMEHIGH;
					if(HVAPIUTILS_GetRecordInfoFromAppenedStringEx(pTmpResult->pcAppendInfo, "TimeLow", pszPlateBuf, dwBufLen) == S_OK )
						TIMELOW = atoi(pszPlateBuf);
						memset(pszPlateBuf, 0, dwBufLen);
					if(HVAPIUTILS_GetRecordInfoFromAppenedStringEx(pTmpResult->pcAppendInfo, "TimeHigh", pszPlateBuf, dwBufLen) == S_OK )
						TIMEHIGH =  atoi(pszPlateBuf);
					DWORD64 dw64TimeMS = ((DWORD64)(TIMEHIGH)<<32) | TIMELOW;
					memset(pszPlateBuf, 0, dwBufLen);
					_ui64toa( dw64TimeMS, pszPlateBuf, 10);
					GetDlgItem(IDC_EDIT4)->SetWindowText(pszPlateBuf);

				}
				else
				{
					if(S_OK == HVAPIUTILS_GetRecordInfoFromAppenedStringEx(pTmpResult->pcAppendInfo, m_parastring, pszPlateBuf, dwBufLen))
					{
						GetDlgItem(IDC_EDIT4)->SetWindowText(pszPlateBuf);
					}
				}
				SAFE_DELETE(pszPlateBuf);
			}
			int i=0;
			while(pszPlateInfo[i] != '\0')
			{
				if(pszPlateInfo[i] == '\n')
				{
				strAppend += "\r\n";
				i++;
				}
				else
				{
				strAppend += pszPlateInfo[i];
				i++;
				}
			}


			char* pstrTime = strstr(pszPlateInfo, "车辆检测时间:");
			/*if(pstrTime)
			{
				DWORD64 dw64TimeMS = 0;
				sscanf(pstrTime, "车辆检测时间:%I64u", &dw64TimeMS);
				CTime cTime(dw64TimeMS / 1000);
				CString strTime;
				strTime.Format("\r\n\r\n\r\n车辆检测时间转换后：%s", cTime.Format("%Y-%m-%d %H:%M:%S"));
				strAppend += strTime;
			}*/

			if(pTmpResult->dwResultType == RECORD_TYPE_HISTORY)
			{
				strAppend += "\r\n结果类型:历史";
			}
			else
			{
				strAppend += "\r\n结果类型:实时";
			}
			SetDlgItemText(IDC_EDIT1, strAppend.GetBuffer());

			if(m_fIsSaveRecord && m_strSavePath != "")
			{
				CString strSaveFileName;
				CTime cTime(pTmpResult->dw64TimeMS/1000);
				CString pathWZ;
				if(m_fisweizhang == true)
					pathWZ = "违章结果\\";
				else
					pathWZ = "非违章结果\\";
				strSaveFileName.Format("%s%s%s_%d.xml", m_strSavePath.GetBuffer(),pathWZ.GetBuffer(),
					cTime.Format("%Y%m%d%H%M%S"), pTmpResult->dwCarID);
				FILE* fp = fopen(strSaveFileName.GetBuffer(), "wb");
				if(fp)
				{
					fwrite(pTmpResult->pcAppendInfo, strlen(pTmpResult->pcAppendInfo), 1, fp);
					fclose(fp);
				}

				strSaveFileName.Format("%s%s%s_%d.txt", m_strSavePath.GetBuffer(),pathWZ.GetBuffer(),
				cTime.Format("%Y%m%d%H%M%S"), pTmpResult->dwCarID);
				fp = fopen(strSaveFileName.GetBuffer(), "wb");
				if(fp)
				{
					fwrite(strAppend, strlen(strAppend), 1, fp);
					fclose(fp);
				}

				strSaveFileName.Format("%sPlateInfo.txt", m_strSavePath.GetBuffer());
				fp = fopen(strSaveFileName.GetBuffer(), "a+");
				if(fp)
				{
					fwrite(pTmpResult->pcPlateNo, strlen(pTmpResult->pcPlateNo), 1, fp);
					fwrite("\n", 1, 1, fp);
					fclose(fp);
				}
			}
			SAFE_DELETE(pszPlateInfo);
		}
		break;
	case 2:
		if(pTmpResult->dwImageType == RECORD_BIGIMG_BEST_SNAPSHOT ||
		pTmpResult->dwImageType == RECORD_BIGIMG_LAST_SNAPSHOT ||
		pTmpResult->dwImageType == RECORD_BIGIMG_BEGIN_CAPTURE ||
		pTmpResult->dwImageType == RECORD_BIGIMG_BEST_CAPTURE ||
		pTmpResult->dwImageType == RECORD_BIGIMG_LAST_CAPTURE)
		{
			CString strTmp;
			if(pTmpResult->dwImageType == RECORD_BIGIMG_LAST_SNAPSHOT 
			|| pTmpResult->dwImageType == RECORD_BIGIMG_BEST_SNAPSHOT)
			{
				IStream* pStm = NULL;
				CreateStreamOnHGlobal(NULL, TRUE, &pStm);
				IPicture* picholder;
				LARGE_INTEGER liTempStar = {0};
				pStm->Seek(liTempStar, STREAM_SEEK_SET, NULL);
				ULONG iWritten = NULL;
				pStm->Write(pTmpResult->pImageData, pTmpResult->dwImageDataLen, &iWritten);
				pStm->Seek(liTempStar, STREAM_SEEK_SET, NULL);
				if(FAILED(OleLoadPicture(pStm, pTmpResult->dwImageDataLen, TRUE, IID_IPicture, (void**)&picholder)))
				{
				pStm->Release();
				return 0;
				}

				HDC TempDC;
				if(pTmpResult->dwImageType == RECORD_BIGIMG_BEST_SNAPSHOT)
				TempDC = ::GetDC(GetDlgItem(IDC_STATIC_BEST)->GetSafeHwnd());
				else 
				TempDC = ::GetDC(GetDlgItem(IDC_STATIC_LAST)->GetSafeHwnd());
				OLE_XSIZE_HIMETRIC hmWidth;
				OLE_YSIZE_HIMETRIC hmHeight;
				picholder->get_Width(&hmWidth);
				picholder->get_Height(&hmHeight);
				int nWidth =MulDiv(hmWidth, GetDeviceCaps(TempDC, LOGPIXELSX), 2540);
				int nHeight = MulDiv(hmHeight, GetDeviceCaps(TempDC, LOGPIXELSY), 2540);
				picholder->Render(TempDC, 0, 0, 640, 400, 0, hmHeight, hmWidth, -hmHeight, NULL); 
				if(pTmpResult->dwImageType == RECORD_BIGIMG_BEST_SNAPSHOT)
				::ReleaseDC(GetDlgItem(IDC_STATIC_BEST)->GetSafeHwnd(), TempDC);
				else 
				::ReleaseDC(GetDlgItem(IDC_STATIC_LAST)->GetSafeHwnd(), TempDC);
				picholder->Release();
				pStm->Release();

				int IDC;
				if (pTmpResult->dwImageType == RECORD_BIGIMG_BEST_SNAPSHOT)
				{
					IDC = IDC_STATIC_BEST;
				}
				else
				{
					IDC = IDC_STATIC_LAST;
				}

				
				strTmp.Format("(%d, %d),(%d, %d)", 
					(long)pTmpResult->iRcplate[1], (long)pTmpResult->iRcplate[0], 
					(long)pTmpResult->iRcplate[3], (long)pTmpResult->iRcplate[2]);

                /****张焕周暂时增*******/
				if(pTmpResult->iRcplate[1]>100
					||pTmpResult->iRcplate[0]>100
					||pTmpResult->iRcplate[2]>100
					||pTmpResult->iRcplate[3]>100)
				{	
					pTmpResult->iRcplate[1] = (pTmpResult->iRcplate[1]*100)/pTmpResult->wImageWidth;
					pTmpResult->iRcplate[0] = (pTmpResult->iRcplate[0]*100)/pTmpResult->wImageHeight;
					pTmpResult->iRcplate[3] = (pTmpResult->iRcplate[3]*100)/pTmpResult->wImageWidth;
					pTmpResult->iRcplate[2] = (pTmpResult->iRcplate[2]*100)/pTmpResult->wImageHeight;
				}
				RECT rect;
				Rect tmpRect;
				GetDlgItem(IDC)->GetWindowRect(&rect);
				ScreenToClient(&rect);
				int a[4] = {0};
				INT m_X,m_Y,iHeight,iWidth,m_Height,m_Width,w_iTmp,h_iTmp;
				iHeight = rect.bottom - rect.top;
				iWidth = rect.right - rect.left;
				m_Y = rect.top + (iHeight * pTmpResult->iRcplate[0]) / 100;
				m_X = rect.left + (iWidth * pTmpResult->iRcplate[1]) / 100;
				h_iTmp = pTmpResult->iRcplate[2] - pTmpResult->iRcplate[0];
				m_Height = (h_iTmp) * iHeight / 100;
				w_iTmp = pTmpResult->iRcplate[3] - pTmpResult->iRcplate[1];
				m_Width = (w_iTmp) * iWidth / 100;

				Graphics grf( GetDlgItem(IDC)->m_hWnd);

			//	float ftLeft(0), ftTop(0), ftRight(0), ftBottom(0);

				tmpRect.X = m_X;
				tmpRect.Y = m_Y;
				tmpRect.Width = m_Width;
				tmpRect.Height = m_Height;
				Pen redPen(Color(255, 255, 0, 0), 3);
				Status st = grf.DrawRectangle(&redPen, tmpRect);
			}

			CString strFace;
			m_strFace.Empty();
			for (int i = 0; i < 20; i ++)
			{
				CString strniNIMA;
				strniNIMA.Format("x:%d ", pTmpResult->cFaceInfo.rcPlate[i][0]);
				strFace = strFace + strniNIMA;
				strniNIMA.Format("y:%d\r\n", pTmpResult->cFaceInfo.rcPlate[i][1]);
				strFace = strFace + strniNIMA;
				strniNIMA.Format("x:%d ", pTmpResult->cFaceInfo.rcPlate[i][2]);
				strFace = strFace + strniNIMA;
				strniNIMA.Format("y:%d\r\n", pTmpResult->cFaceInfo.rcPlate[i][3]);
				strFace = strFace + strniNIMA;
			}
			m_strFace = strFace;
			if (pTmpResult->dwImageType == RECORD_BIGIMG_LAST_SNAPSHOT)
			{
				GetDlgItem(IDC_EDIT_FACE)->SetWindowText(strFace);
			}


			if (pTmpResult->dwImageType == RECORD_BIGIMG_BEST_SNAPSHOT)
			{
				m_showvalue.Empty();
				strTmp = "第一张"+strTmp;
				m_showvalue = strTmp;
			}
			else if(pTmpResult->dwImageType == RECORD_BIGIMG_LAST_SNAPSHOT)
			{
				strTmp = "\r\n第二张"+strTmp;
				m_showvalue += strTmp;
			}
			else if(pTmpResult->dwImageType == RECORD_BIGIMG_BEGIN_CAPTURE)
			{
				if(strstr(m_showvalue,"第三张"))
				{
					m_showvalue.Empty();
				}
				strTmp = "\r\n第三张"+strTmp;
				m_showvalue += strTmp;
			}
			else if(pTmpResult->dwImageType == RECORD_BIGIMG_BEST_CAPTURE)
			{
					strTmp = "\r\n第四张"+strTmp;
					m_showvalue += strTmp;
			}
			else if(pTmpResult->dwImageType == RECORD_BIGIMG_LAST_CAPTURE)
			{
				strTmp = "\r\n第五张"+strTmp;
				m_showvalue += strTmp;
			}

			GetDlgItem(IDC_EDIT_RECT)->SetWindowText(m_showvalue);
		}
		break;
	case 3:
		{
			CImage image;
			if(image.Create(pTmpResult->wImageWidth, pTmpResult->wImageHeight, 24))
			{
				Yuv2Rgb((BYTE*)image.GetBits(), pTmpResult->pImageData, pTmpResult->wImageWidth,
					-pTmpResult->wImageHeight, image.GetPitch());
				IStream* pStream = NULL;
				IPicture* picholder;
				CreateStreamOnHGlobal(NULL, TRUE, &pStream);
				image.Save(pStream, ImageFormatJPEG);
				LARGE_INTEGER liTemp = {0};
				pStream->Seek(liTemp, STREAM_SEEK_SET, NULL);
				if(FAILED(OleLoadPicture(pStream, pTmpResult->dwImageDataLen, TRUE, IID_IPicture, (void**)&picholder)))
				{
					pStream->Release();
					return 0;
				}
				HDC TempDC;
				TempDC = ::GetDC(GetDlgItem(IDC_STATIC_SMALL)->GetSafeHwnd());
				OLE_XSIZE_HIMETRIC hmWidth;
				OLE_YSIZE_HIMETRIC hmHeight;
				picholder->get_Width(&hmWidth);
				picholder->get_Height(&hmHeight);
				int nWidth =MulDiv(hmWidth, GetDeviceCaps(TempDC, LOGPIXELSX), 2540);
				int nHeight = MulDiv(hmHeight, GetDeviceCaps(TempDC, LOGPIXELSY), 2540);
				picholder->Render(TempDC, 0, 0, 300, 70, 0, hmHeight, hmWidth, -hmHeight, NULL); 
				::ReleaseDC(GetDlgItem(IDC_STATIC_SMALL)->GetSafeHwnd(), TempDC);
				picholder->Release();
				pStream->Release();
			}

			if(m_fIsSaveRecord && m_strSavePath != "")
			{
				CString strSaveFileName;
				CTime cTime(pTmpResult->dw64TimeMS/1000);
				CString pathWZ;
				if(m_fisweizhang == true)
					pathWZ = "违章结果\\";
				else
					pathWZ = "非违章结果\\";
				strSaveFileName.Format("%s%s%s_%d.bmp", m_strSavePath.GetBuffer(),pathWZ.GetBuffer(),
					cTime.Format("%Y%m%d%H%M%S"), pTmpResult->dwCarID);
				FILE* fp = fopen(strSaveFileName.GetBuffer(), "wb");
				if(fp)
				{
					INT iBitmapBufLen = 1024 << 10;
					PBYTE pBitmapBuf = new BYTE[iBitmapBufLen];
					if(pBitmapBuf == NULL)
					{
						return 0;
					}
					if(HVAPIUTILS_SmallImageToBitmap(pTmpResult->pImageData, pTmpResult->wImageWidth,
						pTmpResult->wImageHeight, pBitmapBuf, &iBitmapBufLen) == S_OK)
					{
						fwrite(pBitmapBuf, iBitmapBufLen, 1, fp);
					}
					fclose(fp);
					SAFE_DELETE(pBitmapBuf);
				}
			}
		}
		break;
	case 4:
		if(m_fIsSaveRecord && m_strSavePath != "")
		{
			CString strSaveFileName;
			CTime cTime(pTmpResult->dw64TimeMS/1000);
			CString pathWZ;
			if(m_fisweizhang == true)
				pathWZ = "违章结果\\";
			else
				pathWZ = "非违章结果\\";
			strSaveFileName.Format("%s%s%s_%d.bin", m_strSavePath.GetBuffer(),pathWZ.GetBuffer(),
				cTime.Format("%Y%m%d%H%M%S"), pTmpResult->dwCarID);
			FILE* fp = fopen(strSaveFileName.GetBuffer(), "wb");
			if(fp)
			{
				fwrite(pTmpResult->pImageData, pTmpResult->dwImageDataLen, 1, fp);
				fclose(fp);
			}
		}
		break;
	}
	return S_OK;
}

void CResultRecvTestDlg::OnCancel()
{
	HVAPI_SetCallBackEx(*m_hDevice, NULL, NULL, 0, CALLBACK_TYPE_RECORD_PLATE, NULL);
	HVAPI_SetCallBackEx(*m_hDevice, NULL, NULL, 0, CALLBACK_TYPE_STRING, NULL);
	HVAPI_SetCallBackEx(*m_hDevice, NULL, NULL, 0, CALLBACK_TYPE_RECORD_BIGIMAGE, NULL);
	HVAPI_SetCallBackEx(*m_hDevice, NULL, NULL, 0, CALLBACK_TYPE_RECORD_SMALLIMAGE, NULL);
	HVAPI_SetCallBackEx(*m_hDevice, NULL, NULL, 0, CALLBACK_TYPE_RECORD_BINARYIMAGE, NULL);
	HVAPI_SetCallBackEx(*m_hDevice, NULL, NULL, 0, CALLBACK_TYPE_RECORD_INFOBEGIN, NULL);
	HVAPI_SetCallBackEx(*m_hDevice, NULL, NULL, 0, CALLBACK_TYPE_RECORD_INFOEND, NULL);

	
	if(m_hProceThread)
	{
		m_fIsQuite= TRUE;
		int iWaitTimes = 0;
		int MAX_WAIT_TIME = 8;
		while(WaitForSingleObject(m_hProceThread, 500) == WAIT_TIMEOUT
			&& iWaitTimes < MAX_WAIT_TIME)
		{
			iWaitTimes++;
		}
		if(iWaitTimes >= MAX_WAIT_TIME)
		{
			TerminateThread(m_hProceThread, 0);
		}
		CloseHandle(m_hProceThread);
		m_hProceThread = NULL;
	}
	
	while (!m_ResultList.IsEmpty())
	{
		ResultData cTmpInfo = m_ResultList.RemoveHead();
		if (cTmpInfo.pcPlateNo)
		{
			delete [] cTmpInfo.pcPlateNo;
			cTmpInfo.pcPlateNo = NULL;
		}

		if(cTmpInfo.pcAppendInfo)
		{
			delete[] cTmpInfo.pcAppendInfo;
			cTmpInfo.pcAppendInfo = NULL;
		}

		if(cTmpInfo.pImageData)
		{
			delete[] cTmpInfo.pImageData;
			cTmpInfo.pImageData = NULL;
		}
	}
	DeleteCriticalSection(&m_csList);
//	CDialog::OnCancel();
	DestroyWindow();
}



void CResultRecvTestDlg::PostNcDestroy()
{
	// TODO: Add your specialized code here and/or call the base class
//	CDialog::PostNcDestroy();
    delete this;
}



void CResultRecvTestDlg::OnClose()
{
	HVAPI_SetCallBackEx(*m_hDevice, NULL, NULL, 0, CALLBACK_TYPE_RECORD_PLATE, NULL);
	HVAPI_SetCallBackEx(*m_hDevice, NULL, NULL, 0, CALLBACK_TYPE_STRING, NULL);
	if(m_hProceThread)
	{
		m_fIsQuite= TRUE;
		int iWaitTimes = 0;
		int MAX_WAIT_TIME = 8;
		while(WaitForSingleObject(m_hProceThread, 500) == WAIT_TIMEOUT
			&& iWaitTimes < MAX_WAIT_TIME)
		{
			iWaitTimes++;
		}
		if(iWaitTimes >= MAX_WAIT_TIME)
		{
			TerminateThread(m_hProceThread, 0);
		}
		CloseHandle(m_hProceThread);
		m_hProceThread = NULL;
	}

	while (!m_ResultList.IsEmpty())
	{
		ResultData cTmpInfo = m_ResultList.RemoveHead();
		if (cTmpInfo.pcPlateNo)
		{
			delete [] cTmpInfo.pcPlateNo;
			cTmpInfo.pcPlateNo = NULL;
		}

		if(cTmpInfo.pcAppendInfo)
		{
			delete[] cTmpInfo.pcAppendInfo;
			cTmpInfo.pcAppendInfo = NULL;
		}

		if(cTmpInfo.pImageData)
		{
			delete[] cTmpInfo.pImageData;
			cTmpInfo.pImageData = NULL;
		}
	}

	CDialog::OnClose();
}

int CResultRecvTestDlg::OnGetherInfo(PVOID pUserData, LPCSTR pString, DWORD dwStrLen)
{
	if(pUserData == NULL || pString == NULL || dwStrLen <= 0)
	{
		return -1;
	}
	CResultRecvTestDlg* pDlg = (CResultRecvTestDlg*)pUserData;

	DWORD dwStatus = CONN_STATUS_UNKNOWN;
	HVAPI_GetConnStatusEx(*pDlg->m_hDevice, STREAM_TYPE_RECORD, &dwStatus);
	CString str;
	str.Format("dwStatus = %0x",dwStatus);


	if(!strstr(pString, "处理帧率"))
	{
		CString strTmp;
		DWORD dwIndex = 0;
		while(dwIndex < dwStrLen)
		{
			if(pString[dwIndex] == '\n')
			{
				strTmp += "\r\n";
				dwIndex++;
			}
			else
			{
				strTmp += pString[dwIndex];
				dwIndex++;
			}
		}
		pDlg->GetDlgItem(IDC_EDIT2)->SetWindowText(strTmp);
	}
	return 0;
}

void CResultRecvTestDlg::OnBnClickedButton15()
{
	if(m_fIsSetHistoryFlag)
	{
		GetDlgItem(IDC_BUTTON15)->EnableWindow(FALSE);
		::Sleep(1000);

		HVAPI_SetCallBackEx(*m_hDevice, NULL, NULL, 0, CALLBACK_TYPE_RECORD_PLATE, NULL);
		HVAPI_SetCallBackEx(*m_hDevice, NULL, NULL, 0, CALLBACK_TYPE_RECORD_BIGIMAGE, NULL);
		HVAPI_SetCallBackEx(*m_hDevice, NULL, NULL, 0, CALLBACK_TYPE_RECORD_SMALLIMAGE, NULL);
		HVAPI_SetCallBackEx(*m_hDevice, NULL, NULL, 0, CALLBACK_TYPE_RECORD_BINARYIMAGE, NULL);
		HVAPI_SetCallBackEx(*m_hDevice, NULL, NULL, 0, CALLBACK_TYPE_RECORD_INFOBEGIN, NULL);
		HVAPI_SetCallBackEx(*m_hDevice, NULL, NULL, 0, CALLBACK_TYPE_RECORD_INFOEND, NULL);
		HVAPI_SetCallBackEx(*m_hDevice, NULL, NULL, 0, CALLBACK_TYPE_STRING, NULL);

		m_dwTotleResult = 0;
		if(m_fIsSetBigImageCallBack)
		{
			HVAPI_SetCallBackEx(*m_hDevice, OnBigImage, this, 0, CALLBACK_TYPE_RECORD_BIGIMAGE, NULL);
		}
		if(m_fIsSetSmallImageCallBack)
		{
			HVAPI_SetCallBackEx(*m_hDevice, OnSmall, this, 0, CALLBACK_TYPE_RECORD_SMALLIMAGE, NULL);
		}
		if(m_fIsSetBinaryCallBack)
		{
			HVAPI_SetCallBackEx(*m_hDevice, OnBinary, this, 0, CALLBACK_TYPE_RECORD_BINARYIMAGE, NULL);
		}
		if(m_fIsSetStringCallBack)
		{
			HVAPI_SetCallBackEx(*m_hDevice, OnGetherInfo, this, 0, CALLBACK_TYPE_STRING, NULL);
		}
		if(m_fIsSetPlateCallBack)
		{
			DWORD dwRetTryTimes = 0;
			while(dwRetTryTimes < 3)
			{
				if(HVAPI_SetCallBackEx(*m_hDevice, OnPlate, this, 0, CALLBACK_TYPE_RECORD_PLATE, NULL)
					== S_OK)
				{
					break;
				}
				dwRetTryTimes++;
				Sleep(100);
			}
			if(dwRetTryTimes >= 3)
			{
				m_fIsSetPlateCallBack = FALSE;
				SetDlgItemText(IDC_BUTTON7, "设置车牌回调");
		//		HVAPI_SetCallBackEx(*m_hDevice, NULL, NULL, 0, CALLBACK_TYPE_STRING, NULL);
			}
			else
			{
		//		HVAPI_SetCallBackEx(*m_hDevice, OnGetherInfo, this, 0, CALLBACK_TYPE_STRING, NULL);
				HVAPI_SetCallBackEx(*m_hDevice, OnRecordBegin, this, 0, CALLBACK_TYPE_RECORD_INFOBEGIN, NULL);
				HVAPI_SetCallBackEx(*m_hDevice, OnRecordEnd, this, 0, CALLBACK_TYPE_RECORD_INFOEND, NULL);
			}
		}

		GetDlgItem(IDC_BUTTON1)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON5)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON6)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON7)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON15)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON18)->EnableWindow(TRUE);
		SetDlgItemText(IDC_BUTTON15, "接收历史结果");
		m_fIsSetHistoryFlag = FALSE;
	}
	else
	{
		dw64StartTime = 0;
		dw64EndTime = 0;
		dwIndex = 0;
		m_fIsUsedEndTime = FALSE;
		m_fIsGetHistoryRecord = FALSE;
		CSetGetHistoryDlg Dlg(this);
		Dlg.DoModal();
		if(m_fIsGetHistoryRecord)
		{
			GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);
			GetDlgItem(IDC_BUTTON5)->EnableWindow(FALSE);
			GetDlgItem(IDC_BUTTON6)->EnableWindow(FALSE);
			GetDlgItem(IDC_BUTTON7)->EnableWindow(FALSE);
			GetDlgItem(IDC_BUTTON15)->EnableWindow(FALSE);
			GetDlgItem(IDC_BUTTON18)->EnableWindow(FALSE);
			if(m_fIsSetPlateCallBack)
			{
				HVAPI_SetCallBackEx(*m_hDevice, NULL, NULL, 0, CALLBACK_TYPE_RECORD_PLATE, NULL);
			}
			if(m_fIsSetBigImageCallBack)
			{
				HVAPI_SetCallBackEx(*m_hDevice, NULL, NULL, 0, CALLBACK_TYPE_RECORD_BIGIMAGE, NULL);
			}
			if(m_fIsSetSmallImageCallBack)
			{
				HVAPI_SetCallBackEx(*m_hDevice, NULL, NULL, 0, CALLBACK_TYPE_RECORD_SMALLIMAGE, NULL);
			}
			if(m_fIsSetBinaryCallBack)
			{
				HVAPI_SetCallBackEx(*m_hDevice, NULL, NULL, 0, CALLBACK_TYPE_RECORD_BINARYIMAGE, NULL);
			}
			if(m_fIsSetStringCallBack)
			{
				HVAPI_SetCallBackEx(*m_hDevice, NULL, NULL, 0, CALLBACK_TYPE_STRING, NULL);
			}
			
			HVAPI_SetCallBackEx(*m_hDevice, NULL, NULL, 0, CALLBACK_TYPE_RECORD_INFOBEGIN, NULL);
			HVAPI_SetCallBackEx(*m_hDevice, NULL, NULL, 0, CALLBACK_TYPE_RECORD_INFOEND, NULL);

			HVAPI_SetCallBackEx(*m_hDevice, OnBigImage, this, 0, CALLBACK_TYPE_RECORD_BIGIMAGE, NULL);
			HVAPI_SetCallBackEx(*m_hDevice, OnSmall, this, 0, CALLBACK_TYPE_RECORD_SMALLIMAGE, NULL);
			HVAPI_SetCallBackEx(*m_hDevice, OnBinary, this, 0, CALLBACK_TYPE_RECORD_BINARYIMAGE, NULL);
			HVAPI_SetCallBackEx(*m_hDevice, OnRecordBegin, this, 0, CALLBACK_TYPE_RECORD_INFOBEGIN, NULL);
			HVAPI_SetCallBackEx(*m_hDevice, OnRecordEnd, this, 0, CALLBACK_TYPE_RECORD_INFOEND, NULL);
			CString strCmd;
			if(m_fIsUsedEndTime)
			{
				if(dw64EndTime < dw64StartTime)
				{
					MessageBox("设置错误的时间片，下载失败", "ERROR", MB_OK|MB_ICONERROR);
					return;
				}
				CTime cTimeStart(dw64StartTime);
				CTime cTimeEnd(dw64EndTime);
				if(m_fIsGetweizang == TRUE)
				{
				strCmd.Format("DownloadRecord,BeginTime[%s],EndTime[%s],Index[%d],Enable[1],DataInfo[1]",
					cTimeStart.Format("%Y.%m.%d_%H"), cTimeEnd.Format("%Y.%m.%d_%H"), dwIndex);
				}
				else
				{
				strCmd.Format("DownloadRecord,BeginTime[%s],EndTime[%s],Index[%d],Enable[1],DataInfo[0]",
					cTimeStart.Format("%Y.%m.%d_%H"), cTimeEnd.Format("%Y.%m.%d_%H"), dwIndex);
				}
			}
			else
			{
				CTime cTimeStart(dw64StartTime);
				if(m_fIsGetweizang == TRUE)
				{
				strCmd.Format("DownloadRecord,BeginTime[%s],EndTime[0],Index[%d],Enable[1],DataInfo[1]",
					cTimeStart.Format("%Y.%m.%d_%H"), dwIndex);
				}
				else
				{
				strCmd.Format("DownloadRecord,BeginTime[%s],EndTime[0],Index[%d],Enable[1],DataInfo[0]",
					cTimeStart.Format("%Y.%m.%d_%H"), dwIndex);
				}
			}
			m_dwTotleResult = 0;
			DWORD dwRetTryTimes = 0;
			while(dwRetTryTimes < 3)
			{
			::Sleep(2000);

				if(HVAPI_SetCallBackEx(*m_hDevice, OnPlate, this, 0, CALLBACK_TYPE_RECORD_PLATE, strCmd.GetBuffer())
					== S_OK)
				{
					break;
				}
				dwRetTryTimes++;
				Sleep(100);
			}
			if(dwRetTryTimes >= 3)
			{
				MessageBox("设置获取历史结果失败", "ERROR", MB_OK|MB_ICONERROR);
				GetDlgItem(IDC_BUTTON1)->EnableWindow(TRUE);
				GetDlgItem(IDC_BUTTON5)->EnableWindow(TRUE);
				GetDlgItem(IDC_BUTTON6)->EnableWindow(TRUE);
				GetDlgItem(IDC_BUTTON7)->EnableWindow(TRUE);
				GetDlgItem(IDC_BUTTON15)->EnableWindow(TRUE);
				GetDlgItem(IDC_BUTTON18)->EnableWindow(TRUE);
				return;
			}
			else
			{
				GetDlgItem(IDC_BUTTON15)->EnableWindow(TRUE);
				SetDlgItemText(IDC_BUTTON15, "停止接收历史结果");
				m_fIsSetHistoryFlag = TRUE;
			}
		}
		else
		{

		}
	}
}

LRESULT CResultRecvTestDlg::OnProcRecordBegin(WPARAM wParam, LPARAM lParam)
{
	HWND hTmpHwnd = GetSafeHwnd();
	HDC hDC = ::GetDC(hTmpHwnd);
	HPEN hPen;
	HBRUSH hBrush;
	hPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
	hBrush = CreateSolidBrush(RGB(255, 0, 0));
	SelectObject(hDC, hPen);
	SelectObject(hDC, hBrush);
	//SelectObject(hDC, g_hFont);
	Rectangle(hDC, 955, 320, 1085, 360);
	DeleteObject(hPen);
	DeleteObject(hBrush);
	SetBkMode(hDC, 3);
	SetTextColor(hDC, RGB(255, 255, 0));
	char szTmp[20] = {0};
	sprintf(szTmp, "%d", (DWORD)lParam);
	TextOut(hDC, 960, 330, szTmp, (int)strlen(szTmp));
	::ReleaseDC(hTmpHwnd, hDC);
	return S_OK;
}

LRESULT CResultRecvTestDlg::OnProcRecordEnd(WPARAM wParam, LPARAM lParam)
{
	HWND hTmpHwnd = GetSafeHwnd();
	HDC hDC = ::GetDC(hTmpHwnd);
	HPEN hPen;
	HBRUSH hBrush;
	hPen = CreatePen(PS_SOLID, 1, RGB(0, 255, 0));
	hBrush = CreateSolidBrush(RGB(0, 255, 0));
	SelectObject(hDC, hPen);
	SelectObject(hDC, hBrush);
	//SelectObject(hDC, g_hFont);
	Rectangle(hDC, 955, 320, 1085, 360);
	DeleteObject(hPen);
	DeleteObject(hBrush);
	SetBkMode(hDC, 3);
	SetTextColor(hDC, RGB(255, 255, 0));
	char szTmp[20] = {0};
	sprintf(szTmp, "%d", (DWORD)lParam);
	TextOut(hDC, 960, 330, szTmp, (int)strlen(szTmp));
	::ReleaseDC(hTmpHwnd, hDC);
	return S_OK;
}

void CResultRecvTestDlg::OnBnClickedButton17()
{
	CGetparamName dlg;
	if(dlg.DoModal() == IDOK)
	{
		if(dlg.m_paramname.IsEmpty())
		{
			return ;
		}
		else
		{
			m_parastring = dlg.m_paramname;
		}
	}
	else
	{
		return ;
	}
}

void CResultRecvTestDlg::OnBnClickedButton18()
{
	// TODO: Add your control notification handler code here
	if(m_hDevice == NULL)
	{
		MessageBox("设置状态信息回调失败", "ERROR", MB_OK|MB_ICONERROR);
		return;
	}
	if(!m_fIsSetStringCallBack)
	{
		//if(HVAPI_SetCallBackEx(*m_hDevice, OnGetherInfo, this, 0, CALLBACK_TYPE_STRING, NULL) != S_OK)
		if ( S_OK != HVAPI_StartRecvMsg(*m_hDevice , OnGetherInfo , this , 0 , 0) )
		{
			MessageBox("设置状态信息回调失败", "ERROR", MB_OK|MB_ICONERROR);
		}
		else
		{
			m_fIsSetStringCallBack = true;
			GetDlgItem(IDC_BUTTON18)->SetWindowText("取消状态信息回调");
		}

		DWORD dwStatus = CONN_STATUS_UNKNOWN;
		HVAPI_GetConnStatusEx(*m_hDevice, STREAM_TYPE_RECORD, &dwStatus);
		CString str;
		str.Format("dwStatus = %0x",dwStatus);	

	}
	else
	{
		//if(HVAPI_SetCallBackEx(*m_hDevice, NULL, NULL, 0, CALLBACK_TYPE_STRING, NULL) != S_OK)
		if ( HVAPI_StopRecvMsg( *m_hDevice  ))
		{
			MessageBox("取消交通信息采集回调失败...");
		}
		else
		{
			m_fIsSetStringCallBack = false;
			GetDlgItem(IDC_BUTTON18)->SetWindowText("设置状态信息回调");
		}
	}
}



void CResultRecvTestDlg::OnOK()
{
    // 防止按回车退出
}


BOOL CResultRecvTestDlg::PreTranslateMessage(MSG* pMsg) 
{

	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
	{
		 return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
 } 


void CResultRecvTestDlg::OnBnClickedBtnPrtscr()
{
	MessageBox("str","",MB_OK);
}

void CResultRecvTestDlg::OnBnClickedStartRecord()
{
	if ( NULL == m_hDevice )
	{
		AfxMessageBox("无效句柄");
		return ;
	}
	char szDebug[128]={0};
	sprintf( szDebug , "Flag:%d\n" , m_fIsSetRecordFlag );
	AfxMessageBox(szDebug);
	if ( !m_fIsSetRecordFlag )
	{
		GetDlgItem(IDC_START_RECORD)->EnableWindow(FALSE);
		
		HVAPI_StartRecvResult( *m_hDevice , OnRecord ,this , 0 , 0 , 0 , 0 , RESULT_RECV_FLAG_REALTIME );

		m_fIsSetRecordFlag = TRUE;
		GetDlgItem(IDC_START_RECORD)->SetWindowText("取消结果回调");
		GetDlgItem(IDC_START_RECORD)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_START_RECORD)->EnableWindow(FALSE);
		

		HVAPI_StopRecvResult( *m_hDevice );
		Sleep(2000);
		m_fIsSetRecordFlag = FALSE;
		GetDlgItem(IDC_START_RECORD)->SetWindowText("设置结果回调");
		GetDlgItem(IDC_START_RECORD)->EnableWindow(TRUE);

		
	}
	
}


int CResultRecvTestDlg::OnRecord(	PVOID pUserData, 
					DWORD dwResultFlag,			
					DWORD dwResultType,		
					DWORD dwCarID,
					LPCSTR pcPlateNo,
					LPCSTR pcAppendInfo,
					DWORD64 dw64TimeMS,
					CImageInfo  Plate,
					CImageInfo  PlateBin,
					CImageInfo  BestSnapshot,
					CImageInfo  LastSnapshot,
					CImageInfo  BeginCapture,
					CImageInfo  BestCapture,
					CImageInfo  LastCapture
					)
{
	//无效结果
	if ( RESULT_FLAG_INVAIL == dwResultFlag )
	{
		AfxMessageBox("无效结果");
	}
	//有效结果
	else if( RESULT_FLAG_VAIL == dwResultFlag )
	{
		//开始标志
		OnRecordBegin(pUserData, dwCarID);
		//车牌信息
		OnPlate(pUserData, dwCarID, pcPlateNo, pcAppendInfo, dwResultType, dw64TimeMS);
		//大图信息
		if(  false != BestSnapshot.fHasData )
		{
			OnBigImage(pUserData, dwCarID,
				BestSnapshot.wImgType, 
				BestSnapshot.wWidth, 
				BestSnapshot.wHeight, 
				BestSnapshot.pbData, 
				BestSnapshot.dwDataLen, 
				dwResultType, 
				BestSnapshot.dw64TimeMS);

		}
		if ( false != LastSnapshot.fHasData )
		{
			OnBigImage(pUserData, dwCarID,
				LastSnapshot.wImgType, 
				LastSnapshot.wWidth, 
				LastSnapshot.wHeight, 
				LastSnapshot.pbData, 
				LastSnapshot.dwDataLen, 
				dwResultType, 
				LastSnapshot.dw64TimeMS);

		}
		if ( false != BeginCapture.fHasData )
		{
			OnBigImage(pUserData, dwCarID,
				BeginCapture.wImgType, 
				BeginCapture.wWidth, 
				BeginCapture.wHeight, 
				BeginCapture.pbData, 
				BeginCapture.dwDataLen, 
				dwResultType, 
				BeginCapture.dw64TimeMS);

		}
		if ( false != BestCapture.fHasData )
		{
			OnBigImage(pUserData, dwCarID,
				BestCapture.wImgType, 
				BestCapture.wWidth, 
				BestCapture.wHeight, 
				BestCapture.pbData, 
				BestCapture.dwDataLen, 
				dwResultType, 
				BestCapture.dw64TimeMS);

		}
		if ( false != LastCapture.fHasData )
		{
			OnBigImage(pUserData, dwCarID,
				LastCapture.wImgType, 
				LastCapture.wWidth, 
				LastCapture.wHeight, 
				LastCapture.pbData, 
				LastCapture.dwDataLen, 
				dwResultType, 
				LastCapture.dw64TimeMS);

		}
     	//小图信息
		if ( false != Plate.fHasData )
		{
			OnSmall(pUserData, dwCarID,
				Plate.wWidth, 
				Plate.wHeight, 
				Plate.pbData, 
				Plate.dwDataLen, 
				dwResultType, 
				Plate.dw64TimeMS);

		}
		//BIN图信息
		if ( false != PlateBin.fHasData )
		{
			OnBinary(pUserData, dwCarID,
				PlateBin.wWidth, 
				PlateBin.wHeight, 
				PlateBin.pbData, 
				PlateBin.dwDataLen, 
				dwResultType, 
				PlateBin.dw64TimeMS);
		}
		//结束标志
		OnRecordEnd(pUserData, dwCarID);
	}

	//历史结果结束标志
	else if ( RESULT_FLAG_HISTROY_END == dwResultFlag )
	{
		AfxMessageBox("历史结束标志");
	}
	//无效标志
	else 
	{
		//AfxMessageBox("无效标志");
	}
	return S_OK;
}
void CResultRecvTestDlg::OnBnClickedStartHistory()
{
	if ( NULL == m_hDevice )
	{
		AfxMessageBox("无效句柄");
		return ;
	}

	if(m_fIsSetHistoryFlag)
	{
		GetDlgItem(IDC_START_HISTORY)->EnableWindow(FALSE);
		::Sleep(1000);

		m_dwTotleResult = 0;
		HVAPI_StopRecvResult( *m_hDevice );
		if ( m_fIsSetRecordFlag )
		{
			HVAPI_StartRecvResult( *m_hDevice , OnRecord ,this , 0 , 0 , 0 , 0 ,RESULT_RECV_FLAG_REALTIME );
		}
		GetDlgItem(IDC_START_HISTORY)->EnableWindow(TRUE);
		SetDlgItemText(IDC_START_HISTORY, "设置历史结果回调");
		m_fIsSetHistoryFlag = FALSE;
	}
	else
	{
		dw64StartTime = 0;
		dw64EndTime = 0;
		dwIndex = 0;
		m_fIsUsedEndTime = FALSE;
		m_fIsGetHistoryRecord = FALSE;
		CSetGetHistoryDlg Dlg(this);
		Dlg.DoModal();
		if(m_fIsGetHistoryRecord)
		{
			GetDlgItem(IDC_START_HISTORY)->EnableWindow(FALSE);

			HVAPI_StopRecvResult( *m_hDevice );

			CString strCmd;
			if(m_fIsUsedEndTime && dw64EndTime < dw64StartTime)
			{
				MessageBox("设置错误的时间片，下载失败", "ERROR", MB_OK|MB_ICONERROR);
				return;
			}
			
			char szDebug[128];
			DWORD dwRecvFlag = m_fIsGetweizang? RESULT_RECV_FLAG_HISTROY_ONLY_PECCANCY:RESULT_RECV_FLAG_HISTORY;
			DWORD64 dwEndTime = m_fIsUsedEndTime? dw64EndTime:0 ;
			//AfxMessageBox(szDebug);
			m_dwTotleResult = 0;
			DWORD dwRetTryTimes = 0;
			while(dwRetTryTimes < 3)
			{
				if(
					S_OK == HVAPI_StartRecvResult( *m_hDevice , OnRecord ,this , 0 
					, dw64StartTime 
					, dwEndTime
					, dwIndex 
					, dwRecvFlag )
					)
				{
					break;
				}
				::Sleep(2000);
				dwRetTryTimes++;
				Sleep(100);
			}
			if(dwRetTryTimes >= 3)
			{
				MessageBox("设置获取历史结果失败", "ERROR", MB_OK|MB_ICONERROR);
				GetDlgItem(IDC_START_HISTORY)->EnableWindow(TRUE);

				return;
			}
			else
			{
				GetDlgItem(IDC_START_HISTORY)->EnableWindow(TRUE);
				SetDlgItemText(IDC_START_HISTORY, "停止接收历史结果");
				m_fIsSetHistoryFlag = TRUE;
			}
		}

	}
}


void CResultRecvTestDlg::OnBnClickedButtonGetimage()
{
	// TODO: Add your control notification handler code here
	static int s_index = 0;
	
	if ( *m_hDevice != NULL )
	{
		char szFileName[1024] = { "c:\\cap.jpg" };
		CTime tm =CTime::GetCurrentTime();

		sprintf(szFileName, "D:\\result\\%s_%d.jpg", tm.Format("%Y%m%d%H%M%S"), s_index++);

		if ( HVAPI_GetCaptureImage(*m_hDevice, 0, szFileName) == S_OK )
		{
			//MessageBox("str","",MB_OK);
			CString str;
			str.Format("保存图片成功:%s", szFileName);
			AfxMessageBox(str);
		}
		else
		{
			AfxMessageBox("保存图片失败");
		}

	}
	else
	{
		AfxMessageBox("设备未连接");
	}
	
}



