#include "stdafx.h"
#include "ResultView.h"
#include "VehicleTrackingSystemDlg.h"
#include "shlwapi.h "
#include "PicView.h"

#include "CApplication.h"
#include "CWorkbook.h"
#include "CWorkbooks.h"
#include "CWorksheet.h"
#include "CWorksheets.h"
#include "CRange.h"
#include ".\resultview.h"

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) if(p){delete[] p; p = NULL;}
#endif

#define WM_UPDATE_MESSAGE WM_USER+120

IMPLEMENT_DYNAMIC(CResultViewDlg, CDialog)

extern HFONT g_hFontMin;

CResultViewDlg::CResultViewDlg(CWnd* pParent /* = NULL */)
: CDialog(CResultViewDlg::IDD, pParent)
{
	m_lpDDS = NULL;
	m_lpOffScreen = NULL;
	m_lpBack = NULL;
	m_lpRPic = NULL;
	m_lpCPic = NULL;
	m_lpPlate = NULL;
	m_pParent = pParent;
	m_iResultCount = 0;
	m_iResultDelay = 0;
	memset(m_szFinalResultFilePath, 0, 256);
	m_fLoadFromFile = FALSE;
	InitializeCriticalSection(&m_csFinalResult);
	InitializeCriticalSection(&m_csLocaList);
	m_iCurrentSelect = -1;
	m_fExit = TRUE;
	m_MainFont = CreateFont(50, 26, 0, 0, FW_HEAVY, false, false, false,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH, "宋体");
	m_strRPicPath = "";
	m_strCPicPath = "";
}

CResultViewDlg::CResultViewDlg(LPCSTR lpszResultFilePath, CWnd* pParent /* = NULL */)
: CDialog(CResultViewDlg::IDD, pParent)
{
	m_lpDDS = NULL;
	m_lpOffScreen = NULL;
	m_lpBack = NULL;
	m_lpRPic = NULL;
	m_lpCPic = NULL;
	m_lpPlate = NULL;
	m_pParent = pParent;
	memset(m_szFinalResultFilePath, 0, 256);
	memcpy(m_szFinalResultFilePath, lpszResultFilePath, strlen(lpszResultFilePath));
	m_fLoadFromFile = TRUE;
	InitializeCriticalSection(&m_csFinalResult);
	InitializeCriticalSection(&m_csLocaList);
	m_iCurrentSelect = -1;
	m_fExit = TRUE;
	m_MainFont = CreateFont(50, 26, 0, 0, FW_HEAVY, false, false, false,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH, "宋体");
}

CResultViewDlg::~CResultViewDlg()
{
	if(m_hThreadMain)
	{
		m_fExit = TRUE;
		int iWaitTimes = 0;
		int MAXWAITTIME = 8;
		while(WaitForSingleObject(m_hThreadMain, 500) == WAIT_TIMEOUT && iWaitTimes < MAXWAITTIME)
		{
			iWaitTimes++;
		}

		if(iWaitTimes >= MAXWAITTIME)
		{
			TerminateThread(m_hThreadMain, 0);
		}
		else
		{
			CloseHandle(m_hThreadMain);
		}
	}
	m_hThreadMain = NULL;
	EnterCriticalSection(&m_csFinalResult);
	while(m_FinalResultList.GetCount() > 0)
	{
		FinalResultType* pTmpFinalResult = m_FinalResultList.RemoveHead();
		delete pTmpFinalResult;
		pTmpFinalResult = NULL;
	}
	LeaveCriticalSection(&m_csFinalResult);
	DeleteCriticalSection(&m_csFinalResult);

	EnterCriticalSection(&m_csLocaList);
	while(m_LocaList.GetCount() > 0)
	{
		LocaType* pTmpLoca = m_LocaList.RemoveHead();
		delete pTmpLoca;
		pTmpLoca = NULL;
	}
	LeaveCriticalSection(&m_csLocaList);
	DeleteCriticalSection(&m_csLocaList);
	if(m_lpCPic)
	{
		m_lpCPic->Release();
		m_lpCPic = NULL;
	}
	if(m_lpRPic)
	{
		m_lpRPic->Release();
		m_lpRPic = NULL;
	}
	if(m_lpPlate)
	{
		m_lpPlate->Release();
		m_lpPlate = NULL;
	}
	if(m_lpBack)
	{
		m_lpBack->Release();
		m_lpBack = NULL;
	}
	if(m_lpOffScreen)
	{
		m_lpOffScreen->Release();
		m_lpOffScreen = NULL;
	}
	if(m_lpDDS)
	{
		m_lpDDS->Release();
		m_lpDDS = NULL;
	}
}

void CResultViewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListFinalResult);
}

BEGIN_MESSAGE_MAP(CResultViewDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, OnNMDblclkList1)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST1, OnLvnKeydownList1)
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

void CResultViewDlg::LoadFinalResultList()
{
	if(strstr(m_szFinalResultFilePath, ".xls"))
	{
		CApplication* cExcel = new CApplication;
		if(cExcel->CreateDispatch("Excel.Application") == TRUE)
		{
			cExcel->put_Visible(false);
			cExcel->put_UserControl(true);

			CWorkbook	m_workBook;
			CWorkbooks	m_workBooks;
			CWorksheet	m_workSheet;
			CWorksheets	m_workSheets;
			CRange		m_range;
			COleVariant	covOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);
			m_workBooks.AttachDispatch(cExcel->get_Workbooks());
			m_workBook = m_workBooks.Open(m_szFinalResultFilePath, covOptional, covOptional, covOptional, covOptional, covOptional, covOptional,
				covOptional, covOptional, covOptional, covOptional, covOptional, covOptional, covOptional, covOptional);
			m_workSheets = m_workBook.get_Worksheets();
			m_workSheet = m_workSheets.get_Item(COleVariant((short)1));

			CString strPlate = "";
			VARIANT var;
			m_range = m_workSheet.get_Range(COleVariant("B1"), covOptional);
			var = m_range.get_Value2();
			strPlate = (LPCSTR)_bstr_t(var);

			int iIndex = 4;
			m_range = m_workSheet.get_Range(COleVariant("A4"), covOptional);
			var = m_range.get_Value2();
			CString strLocaName;
			strLocaName = (LPCSTR)_bstr_t(var);
			EnterCriticalSection(&m_csFinalResult);
			while(strLocaName.GetLength() > 0)
			{
				char szRangeInfo[50] = {0};
				CString strPassTime = "";
				CString strRPicPath = "";
				CString strCPicPath = "";
				CString strLoca = "";

				sprintf(szRangeInfo, "B%d", iIndex);
				m_range = m_workSheet.get_Range(COleVariant(szRangeInfo), covOptional);
				var = m_range.get_Value2();

				strPassTime = (LPCSTR)_bstr_t(var);

				char szTmpPassTime[60] = {0};
				memcpy(szTmpPassTime, strPassTime.GetBuffer(), strPassTime.GetLength());
				szTmpPassTime[strPassTime.GetLength()-1] = '\0';
				strPassTime = szTmpPassTime+1;

				sprintf(szRangeInfo, "C%d", iIndex);
				m_range = m_workSheet.get_Range(COleVariant(szRangeInfo), covOptional);
				var = m_range.get_Value2();
				strRPicPath = (LPCSTR)_bstr_t(var);

				sprintf(szRangeInfo, "D%d", iIndex);
				m_range = m_workSheet.get_Range(COleVariant(szRangeInfo), covOptional);
				var = m_range.get_Value2();
				strCPicPath = (LPCSTR)_bstr_t(var);

				sprintf(szRangeInfo, "E%d", iIndex);
				m_range = m_workSheet.get_Range(COleVariant(szRangeInfo), covOptional);
				var = m_range.get_Value2();
				strLoca = (LPCSTR)_bstr_t(var);
				int iLocaX;
				int iLocaY;
				sscanf(strLoca.GetBuffer(), "\"%d,%d\"", &iLocaX, &iLocaY);

				FinalResultType* pNewFinalResult = new FinalResultType();
				memcpy(pNewFinalResult->szLocaName, strLocaName.GetBuffer(), strLocaName.GetLength());
				memcpy(pNewFinalResult->szPassTime, strPassTime.GetBuffer(), strPassTime.GetLength());
				memcpy(pNewFinalResult->szRPicPath, strRPicPath.GetBuffer(), strRPicPath.GetLength());
				memcpy(pNewFinalResult->szCPicPath, strCPicPath.GetBuffer(), strCPicPath.GetLength());
				memcpy(pNewFinalResult->szPlate, strPlate.GetBuffer(), strPlate.GetLength());
				pNewFinalResult->Loca.x = iLocaX;
				pNewFinalResult->Loca.y = iLocaY;
				m_FinalResultList.AddTail(pNewFinalResult);

				iIndex++;
				sprintf(szRangeInfo, "A%d", iIndex);
				m_range = m_workSheet.get_Range(COleVariant(szRangeInfo), covOptional);
				var = m_range.get_Value2();
				strLocaName = "";
				strLocaName = (LPCSTR)_bstr_t(var);
			}
			LeaveCriticalSection(&m_csFinalResult);
			m_range.ReleaseDispatch();
			m_workSheet.ReleaseDispatch();
			m_workSheets.ReleaseDispatch();
			m_workBook.ReleaseDispatch();
			m_workBooks.ReleaseDispatch();
			m_workBook.Close(covOptional, covOptional, covOptional);
			m_workBooks.Close();
			cExcel->Quit();
			cExcel->ReleaseDispatch();
			delete cExcel;
		}
		else
		{
			MessageBox("加载结果文件失败，即将退出预览", "ERROR", MB_OK|MB_ICONERROR);
			OnCancel();
		}
	}
	else
	{
		if(PathFileExists(m_szFinalResultFilePath))
		{
			HANDLE hFile = NULL;
			DWORD dwFileSize, dwByteRead;
			hFile = CreateFile(m_szFinalResultFilePath, GENERIC_READ, FILE_SHARE_READ,
				NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if(hFile == INVALID_HANDLE_VALUE)
			{
				MessageBox("加载结果文件失败，即将退出预览", "ERROR", MB_OK|MB_ICONERROR);
				OnCancel();
				return;
			}
			dwFileSize = GetFileSize(hFile, NULL);
			if(dwFileSize == 0xFFFFFFFF)
			{
				CloseHandle(hFile);
				MessageBox("加载结果文件失败，即将退出预览", "ERROR", MB_OK|MB_ICONERROR);
				OnCancel();
				return;
			}
			PBYTE pbTmpData = new BYTE[dwFileSize];
			if(pbTmpData == NULL)
			{
				CloseHandle(hFile);
				MessageBox("加载结果文件失败，即将退出预览", "ERROR", MB_OK|MB_ICONERROR);
				OnCancel();
				return;
			}
			if(ReadFile(hFile, pbTmpData, dwFileSize, &dwByteRead, NULL) != TRUE)
			{
				SAFE_DELETE_ARRAY(pbTmpData);
				CloseHandle(hFile);
				MessageBox("加载结果文件失败，即将退出预览", "ERROR", MB_OK|MB_ICONERROR);
				OnCancel();
				return;
			}
			CloseHandle(hFile);
			if(dwByteRead <= 11)
			{
				SAFE_DELETE_ARRAY(pbTmpData);
				MessageBox("加载结果文件失败，即将退出预览", "ERROR", MB_OK|MB_ICONERROR);
				OnCancel();
				return;
			}
			int iRemainByte = (int)dwByteRead;
			PBYTE pDataPointer = pbTmpData;
			pDataPointer += 11;
			iRemainByte -= 11;
			char szTmpData[257] = {0};
			CString strPlate = "";
			int iTmpDataIndex = 0;
			while(*pDataPointer != '\n' && iRemainByte >= 0)
			{
				szTmpData[iTmpDataIndex++] = *pDataPointer++;
				iRemainByte--;
			}
			if(iRemainByte < 2) return;
			pDataPointer += 2;
			iRemainByte -= 2;
			strPlate = szTmpData;

			if(iRemainByte < 14) return;
			pDataPointer += 14;
			iRemainByte -= 14;
			if(iRemainByte < 14) return;
			pDataPointer += 14;
			iRemainByte -= 14;
			if(iRemainByte < 16) return;
			pDataPointer += 16;
			iRemainByte -= 16;
			if(iRemainByte < 16) return;
			pDataPointer += 16;
			iRemainByte -= 16;
			if(iRemainByte < 9) return;
			pDataPointer += 9;
			iRemainByte -= 9;

			EnterCriticalSection(&m_csFinalResult);
			while(iRemainByte >= 0)
			{
				CString strLocaName = "";
				iTmpDataIndex = 0;
				memset(szTmpData, 0, 257);
				while(*pDataPointer != '\t' && iRemainByte >= 0)
				{
					szTmpData[iTmpDataIndex++] = *pDataPointer++;
					iRemainByte--;
				}
				strLocaName = szTmpData;
				if(iRemainByte < 2) break;
				pDataPointer += 2;
				iRemainByte -= 2;

				CString strPassTime = "";
				iTmpDataIndex = 0;
				memset(szTmpData, 0, 257);
				while(*pDataPointer != '\t' && iRemainByte >= 0)
				{
					szTmpData[iTmpDataIndex++] = *pDataPointer++;
					iRemainByte--;
				}
				strPassTime = szTmpData;
				if(iRemainByte < 2) break;
				pDataPointer += 2;
				iRemainByte -= 2;

				CString strRPicPath = "";
				iTmpDataIndex = 0;
				memset(szTmpData, 0, 257);
				while(*pDataPointer != '\t' && iRemainByte >= 0)
				{
					szTmpData[iTmpDataIndex++] = *pDataPointer++;
					iRemainByte--;
				}
				strRPicPath = szTmpData;
				if(iRemainByte < 2) break;
				pDataPointer += 2;
				iRemainByte -= 2;

				CString strCPicPath = "";
				iTmpDataIndex = 0;
				memset(szTmpData, 0, 257);
				while(*pDataPointer != '\t' && iRemainByte >= 0)
				{
					szTmpData[iTmpDataIndex++] = *pDataPointer++;
					iRemainByte--;
				}
				strCPicPath = szTmpData;
				if(iRemainByte < 2) break;
				pDataPointer += 2;
				iRemainByte -= 2;

				CString strPoint = "";
				iTmpDataIndex = 0;
				memset(szTmpData, 0, 257);
				while(*pDataPointer != '\n' && iRemainByte >= 0)
				{
					szTmpData[iTmpDataIndex++] = *pDataPointer++;
					iRemainByte--;
				}
				strPoint = szTmpData;

				int iLocaX, iLocaY;
				sscanf(strPoint, "\"%d,%d\"", &iLocaX, &iLocaY);

				FinalResultType* pNewFinalResult = new FinalResultType();
				memcpy(pNewFinalResult->szLocaName, strLocaName.GetBuffer(), strLocaName.GetLength());
				memcpy(pNewFinalResult->szPassTime, strPassTime.GetBuffer(), strPassTime.GetLength());
				memcpy(pNewFinalResult->szRPicPath, strRPicPath.GetBuffer(), strRPicPath.GetLength());
				memcpy(pNewFinalResult->szCPicPath, strCPicPath.GetBuffer(), strCPicPath.GetLength());
				memcpy(pNewFinalResult->szPlate, strPlate.GetBuffer(), strPlate.GetLength());
				pNewFinalResult->Loca.x = iLocaX;
				pNewFinalResult->Loca.y = iLocaY;
				m_FinalResultList.AddTail(pNewFinalResult);

				if(iRemainByte < 1) break;
				pDataPointer++;
				iRemainByte--;
			}
			LeaveCriticalSection(&m_csFinalResult);
		}
	}

	char szSystemData[256] = {0};
	int iLen = (int)strlen(m_szFinalResultFilePath);
	memcpy(szSystemData, m_szFinalResultFilePath, iLen);
    szSystemData[iLen-1] = 't';
	szSystemData[iLen-2] = 'a';
	szSystemData[iLen-3] = 'd';
	if(PathFileExists(szSystemData))
	{
		HANDLE hFile = NULL;
		DWORD dwFileSize, dwByteRead;
		hFile = CreateFile(szSystemData, GENERIC_READ, FILE_SHARE_READ,
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if(hFile == INVALID_HANDLE_VALUE)
		{
			MessageBox("地图场景数据加载失败,无法正常显示结果", "警告", MB_OK|MB_ICONERROR);
			return;
		}
		dwFileSize = GetFileSize(hFile, NULL);
		if(dwFileSize == 0xFFFFFFFF)
		{
			MessageBox("地图场景数据加载失败,无法正常显示结果", "警告", MB_OK|MB_ICONERROR);
			CloseHandle(hFile);
			return;
		}
		PBYTE pbTmpData = new BYTE[dwFileSize];
		if(pbTmpData == NULL)
		{
			MessageBox("地图场景数据加载失败,无法正常显示结果", "警告", MB_OK|MB_ICONERROR);
			CloseHandle(hFile);
			return;
		}
		if(ReadFile(hFile, pbTmpData, dwFileSize, &dwByteRead, NULL) != TRUE)
		{
			MessageBox("地图场景数据加载失败,无法正常显示结果", "警告", MB_OK|MB_ICONERROR);
			SAFE_DELETE_ARRAY(pbTmpData);
			CloseHandle(hFile);
			return;
		}
		CloseHandle(hFile);
		if(dwByteRead <= 11)
		{
			MessageBox("地图场景数据加载失败,无法正常显示结果", "警告", MB_OK|MB_ICONERROR);
			SAFE_DELETE_ARRAY(pbTmpData);
			return;
		}
		PBYTE pDataPointer = pbTmpData;
		char szTmpInfo[64] = {0};
		memcpy(szTmpInfo, pDataPointer, 10);
		if(strcmp(szTmpInfo, "<LOCAINFO>") != 0)
		{
			SAFE_DELETE_ARRAY(pbTmpData);
			MessageBox("地图场景数据加载失败,无法正常显示结果", "警告", MB_OK|MB_ICONERROR);
			return;
		}
		pDataPointer += 10;

		int iPointCount = 0;
		memcpy(&iPointCount, pDataPointer, sizeof(int));
		pDataPointer += 4;

		EnterCriticalSection(&m_csLocaList);
		for(int iIndexLoca=0; iIndexLoca<iPointCount; iIndexLoca++)
		{
			POINT cPoint;
			memcpy(&cPoint, pDataPointer, sizeof(cPoint));
			pDataPointer += sizeof(POINT);
			LocaType* pNewLoca = new LocaType;
			pNewLoca->iPositionX = cPoint.x;
			pNewLoca->iPositionY = cPoint.y;
			m_LocaList.AddTail(pNewLoca);
		}
		LeaveCriticalSection(&m_csLocaList);
	
		memset(szTmpInfo, 0, 64);
		memcpy(szTmpInfo, pDataPointer, 9);
		if(strcmp(szTmpInfo, "<MAPDATA>") != 0)
		{
			SAFE_DELETE_ARRAY(pbTmpData);
			MessageBox("地图场景数据加载失败,无法正常显示结果", "警告", MB_OK|MB_ICONERROR);
			return;
		}
		pDataPointer += 9;
		int iMapSize = 0;
		memcpy(&iMapSize, pDataPointer, sizeof(int));
		pDataPointer += sizeof(int);
		if(iMapSize > 0)
		{
			if(m_lpBack)
			{
				DDSURFACEDESC2 ddsd;
				ZeroMemory(&ddsd, sizeof(ddsd));
				ddsd.dwSize = sizeof(ddsd);
				m_lpBack->Lock(NULL, &ddsd, 0, NULL);
				PBYTE pBackBuf = (PBYTE)ddsd.lpSurface;
				INT indexX, indexY;
				PBYTE pImage = pBackBuf;

				if(ddsd.ddpfPixelFormat.dwRGBBitCount == 32)
				{
					for(indexY=0; indexY<786; indexY++)
					{
						for(indexX=0; indexX<1024; indexX++)
						{
							*pImage++ = *(pDataPointer + 2);
							*pImage++ = *(pDataPointer + 1);
							*pImage++ = *(pDataPointer);
							*pImage++;
							pDataPointer += 3;
						}
					}
				}
				else if(ddsd.ddpfPixelFormat.dwRGBBitCount == 16)
				{
					for(indexY=0; indexY<786; indexY++)
					{
						for(indexX=0; indexX<1024; indexX++)
						{
							BYTE b = *(pDataPointer + 2);
							BYTE g = *(pDataPointer + 1);
							BYTE r = *pDataPointer;
							r >>= 3;
							g >>= 2;
							b >>= 3;
							short dwColor = (short)(((short)r<<11) + ((short)g<<5) + (short)b);
							memcpy(pImage, &dwColor, 2);
							pImage += 2;
							pDataPointer += 3;
						}
					}
				}
				else
				{
					MessageBox("不是32位或16位色模式，无法载入地图");
					pDataPointer += 1024 * 786 * 3;
				}
				m_lpBack->Unlock(NULL);
			}
		}
		SAFE_DELETE_ARRAY(pbTmpData);
	}
	else
	{
		MessageBox("地图场景数据加载失败,无法正常显示结果", "警告", MB_OK|MB_ICONERROR);
		return;
	}
}

void CResultViewDlg::CopyFinalResultList()
{
	int iTotleGetCount = 0;
	CVehicleTrackingSystemDlg* pParent = (CVehicleTrackingSystemDlg*)m_pParent;
	EnterCriticalSection(&pParent->m_csFinalResultList);
	EnterCriticalSection(&m_csFinalResult);
	POSITION posTmpFinalResult;
	if(pParent->m_iStartResultIndex >= 0)
	posTmpFinalResult = pParent->m_FinalResultList.FindIndex(pParent->m_iStartResultIndex);
	else
	posTmpFinalResult = pParent->m_FinalResultList.GetHeadPosition();

	if(pParent->m_iStartResultIndex != -1)
	{
		if(pParent->m_iEndResultIndex == -1)
		iTotleGetCount = (int)pParent->m_FinalResultList.GetCount() - pParent->m_iStartResultIndex;
		else
		iTotleGetCount = pParent->m_iEndResultIndex - pParent->m_iStartResultIndex;
	}
	else
	{
		if(pParent->m_iEndResultIndex == -1)
		iTotleGetCount = (int)pParent->m_FinalResultList.GetCount();
		else
		iTotleGetCount = pParent->m_iEndResultIndex;
	}
	int iIndex = 0;
	while(posTmpFinalResult)
	{
		FinalResultType* pTmpFinalResult = pParent->m_FinalResultList.GetNext(posTmpFinalResult);
		FinalResultType* pNewFinalResult = new FinalResultType();
		memcpy(pNewFinalResult, pTmpFinalResult, sizeof(FinalResultType));
		m_FinalResultList.AddTail(pNewFinalResult);
		iIndex++;
		if(iIndex > iTotleGetCount)break;
	}
	LeaveCriticalSection(&m_csFinalResult);
	LeaveCriticalSection(&pParent->m_csFinalResultList);

	EnterCriticalSection(&pParent->m_csLocaList);
	EnterCriticalSection(&m_csLocaList);
	POSITION posLoca = pParent->m_LocaList.GetHeadPosition();
	while(posLoca)
	{
		LocaType* pTmpLoca = pParent->m_LocaList.GetNext(posLoca);
		LocaType* pNewLoca = new LocaType();
		memcpy(pNewLoca, pTmpLoca, sizeof(LocaType));
		m_LocaList.AddTail(pNewLoca);
	}
	LeaveCriticalSection(&m_csLocaList);
	LeaveCriticalSection(&pParent->m_csLocaList);

	HDC hSrcDC, hDecDC;
	m_lpBack->GetDC(&hDecDC);
	pParent->m_lpBack->GetDC(&hSrcDC);
	BitBlt(hDecDC, 0, 0, 1024, 786, hSrcDC, 0, 0, SRCCOPY);
	pParent->m_lpBack->ReleaseDC(hSrcDC);
	m_lpBack->ReleaseDC(hDecDC);
}

BOOL CResultViewDlg::InitDX()
{
	if(FAILED(DirectDrawCreateEx(NULL, (void**)&m_lpDDS, IID_IDirectDraw7, NULL)))
	{
		return FALSE;
	}
	if(FAILED(m_lpDDS->SetCooperativeLevel(GetDlgItem(IDC_STATIC_MAP)->GetSafeHwnd(), DDSCL_NORMAL)))
	{
		return FALSE;
	}
	DDSURFACEDESC2 ddsd;
	ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
	ddsd.dwWidth = 1024;
	ddsd.dwHeight = 786;
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
	if(FAILED(m_lpDDS->CreateSurface(&ddsd, &m_lpOffScreen, NULL)))
	{
		return FALSE;
	}
	if(FAILED(m_lpDDS->CreateSurface(&ddsd, &m_lpBack, NULL)))
	{
		return FALSE;
	}

	ddsd.dwWidth = 318;
	ddsd.dwHeight = 80;
	if(FAILED(m_lpDDS->CreateSurface(&ddsd, &m_lpPlate, NULL)))
	{
		return FALSE;
	}
	DDSURFACEDESC2 ddsd1;
	ZeroMemory(&ddsd1, sizeof(ddsd1));
	ddsd1.dwSize = sizeof(ddsd1);
	PBYTE pBuf = NULL;
	m_lpPlate->Lock(NULL, &ddsd1, 0, NULL);
	pBuf = (PBYTE)ddsd1.lpSurface;
	memset(pBuf, 250, ddsd1.dwHeight * ddsd1.lPitch);
	m_lpPlate->Unlock(NULL);

	ddsd.dwWidth = 318;
	ddsd.dwHeight = 240;
	if(FAILED(m_lpDDS->CreateSurface(&ddsd, &m_lpRPic, NULL)))
	{
		return FALSE;
	}
	if(FAILED(m_lpDDS->CreateSurface(&ddsd, &m_lpCPic, NULL)))
	{
		return FALSE;
	}
	ZeroMemory(&ddsd1, sizeof(ddsd1));
	ddsd1.dwSize = sizeof(ddsd1);
	m_lpRPic->Lock(NULL, &ddsd1, 0, NULL);
	pBuf = (PBYTE)ddsd1.lpSurface;
	memset(pBuf, 250, ddsd1.dwHeight * ddsd1.lPitch);
	m_lpRPic->Unlock(NULL);
	ZeroMemory(&ddsd1, sizeof(ddsd1));
	ddsd1.dwSize = sizeof(ddsd1);
	m_lpCPic->Lock(NULL, &ddsd1, 0, NULL);
	pBuf = (PBYTE)ddsd1.lpSurface;
	memset(pBuf, 250, ddsd1.dwHeight * ddsd1.lPitch);
	m_lpCPic->Unlock(NULL);

	if(m_fLoadFromFile)
	{
		LoadFinalResultList();
	}
	else
	{
		CopyFinalResultList();
	}

	return TRUE;
}

void CResultViewDlg::DrawRect(HDC hDC, int x, int y, int x1, int y1, COLORREF color)
{
	HPEN hPen = CreatePen(PS_SOLID, 1, color);
	HBRUSH hBrush = CreateSolidBrush(color);
	SelectObject(hDC, hPen);
	SelectObject(hDC, hBrush);
	Rectangle(hDC, x, y, x1, y1);
	DeleteObject(hPen);
	DeleteObject(hBrush);
}

void CResultViewDlg::DrawRectangle(HDC hDC, int x1, int y1, int x2, int y2, COLORREF color1, COLORREF color2)
{
	HPEN hPen = CreatePen(PS_SOLID, 1, color1);
	HBRUSH hBrush = CreateSolidBrush(color2);
	SelectObject(hDC, hPen);
	SelectObject(hDC, hBrush);
	Rectangle(hDC, x1, y1, x2, y2);
	DeleteObject(hPen);
	DeleteObject(hBrush);
}


void CResultViewDlg::DrawEllsip(HDC hDC, int x, int y, int x1, int y1, COLORREF color1, COLORREF corlor2)
{
	HPEN hPen = CreatePen(PS_SOLID, 1, color1);
	HBRUSH hBrush = CreateSolidBrush(corlor2);
	SelectObject(hDC, hPen);
	SelectObject(hDC, hBrush);
	Ellipse(hDC, x, y, x1, y1);
	DeleteObject(hPen);
	DeleteObject(hBrush);
}

void CResultViewDlg::DrawLine(HDC hDC, int x1, int y1, int x2, int y2, int iWidth, COLORREF color)
{
	HPEN hPen = CreatePen(PS_SOLID, iWidth, color);
	SelectObject(hDC, hPen);
	MoveToEx(hDC, x1, y1, NULL);
	LineTo(hDC, x2, y2);
	DeleteObject(hPen);
}

void CResultViewDlg::DrawText(HDC hDC, int x, int y, HFONT hFont, char* pszText, int iLen, COLORREF corlor)
{
	if(hFont)
	SelectObject(hDC, hFont);
	SetBkMode(hDC, 3);
	SetTextColor(hDC, corlor);
	TextOut(hDC, x, y, pszText, iLen);
}

void CResultViewDlg::DrawTextThis(HDC hDC, int x, int y, LPCSTR pszText, int iTextLen, COLORREF color)
{
	SetBkMode(hDC, 3);
	SetTextColor(hDC, color);
	TextOut(hDC, x, y, pszText, iTextLen);
}

void CResultViewDlg::DrawPlate()
{
	if(m_lpPlate)
	{
		EnterCriticalSection(&m_csFinalResult);
		if(m_iCurrentSelect < 0 || m_iCurrentSelect >= m_FinalResultList.GetCount())
		{
			LeaveCriticalSection(&m_csFinalResult);
			return;
		}
		POSITION posFinalResult = m_FinalResultList.FindIndex(m_iCurrentSelect);
		if(posFinalResult)
		{
			FinalResultType* pTmpFinalResult = m_FinalResultList.GetAt(posFinalResult);
			COLORREF bgColor, wordColor;
			if(strstr(pTmpFinalResult->szPlate, "无车牌"))
			{
				bgColor = RGB(255, 0, 0);
				wordColor = RGB(255, 255, 255);
			}
			else if(strstr(pTmpFinalResult->szPlate, "蓝"))
			{
				bgColor = RGB(0, 0, 255);
				wordColor = RGB(255, 255, 255);
			}
			else if(strstr(pTmpFinalResult->szPlate, "黄"))
			{
				bgColor = RGB(255, 255, 0);
				wordColor = RGB(0, 0, 0);
			}
			else if(strstr(pTmpFinalResult->szPlate, "黑"))
			{
				bgColor = RGB(0, 0, 0);
				wordColor = RGB(255, 255, 255);
			}
			else if(strstr(pTmpFinalResult->szPlate, "白"))
			{
				bgColor = RGB(255, 255, 255);
				wordColor = RGB(0, 0, 0);
			}
			else
			{
				bgColor = RGB(0, 0, 255);
				wordColor = RGB(255, 255, 255);
			}
			HDC hDC;
			m_lpPlate->GetDC(&hDC);
			DrawRect(hDC, 0, 0, 318, 80, bgColor);
			DrawText(hDC, 10, 20, m_MainFont, pTmpFinalResult->szPlate, (int)strlen(pTmpFinalResult->szPlate), wordColor);
			m_lpPlate->ReleaseDC(hDC);
		}
		LeaveCriticalSection(&m_csFinalResult);
	}
}

void CResultViewDlg::UpDateResultList()
{
	m_ListFinalResult.DeleteAllItems();
	int iIndex = 0;
	EnterCriticalSection(&m_csFinalResult);
	POSITION posFinalResult = m_FinalResultList.GetHeadPosition();
	while(posFinalResult)
	{
		FinalResultType* pTmpFinalResult = m_FinalResultList.GetNext(posFinalResult);
		m_ListFinalResult.InsertItem(iIndex, "", 0);
		m_ListFinalResult.SetItemText(iIndex, 0, pTmpFinalResult->szPlate);
		m_ListFinalResult.SetItemText(iIndex, 1, pTmpFinalResult->szPassTime);
		m_ListFinalResult.SetItemText(iIndex, 2, pTmpFinalResult->szLocaName);
		iIndex++;
	}
	LeaveCriticalSection(&m_csFinalResult);
	if(iIndex > 0)
	{
		m_iCurrentSelect = 0;
		m_fIsNeedUpDatePicBuffer = TRUE;
		DrawPlate();
	}
}

BOOL CResultViewDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	RECT cRect;
	cRect.left = (GetSystemMetrics(SM_CXSCREEN) - 1359) >> 1;
	cRect.top = (GetSystemMetrics(SM_CYSCREEN) - 836 - 10) >> 1;
	cRect.right = cRect.left + 1359;
	cRect.bottom = cRect.top + 836;
	MoveWindow(&cRect, TRUE);

	cRect.left = 5;
	cRect.right = 1026;
	cRect.top = 5;
	cRect.bottom = 791;
	GetDlgItem(IDC_STATIC_MAP)->MoveWindow(&cRect, TRUE);

	cRect.left = 1031;
	cRect.right = 1349;
	cRect.top = 5;
	cRect.bottom = 85;
	GetDlgItem(IDC_STATIC_PLATE)->MoveWindow(&cRect, TRUE);

	cRect.top = 90;
	cRect.bottom = 290;
	m_ListFinalResult.MoveWindow(&cRect, TRUE);

	cRect.top = 295;
	cRect.bottom = 525;
	GetDlgItem(IDC_STATIC_RPIC)->MoveWindow(&cRect, TRUE);
	cRect.top = 530;
	cRect.bottom = 790;
	GetDlgItem(IDC_STATIC_CPIC)->MoveWindow(&cRect, TRUE);

	m_fIsNeedUpDatePicBuffer = TRUE;
	if(InitDX() == FALSE)
	{
		OnCancel();
		return FALSE;
	}

	m_ListFinalResult.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_ListFinalResult.InsertColumn(0, "车牌号", LVCFMT_LEFT, 80);
	m_ListFinalResult.InsertColumn(1, "经过时间", LVCFMT_LEFT, 116);
	m_ListFinalResult.InsertColumn(2, "断面名称", LVCFMT_LEFT, 118);
	UpDateResultList();
	m_fExit = FALSE;
	m_hThreadMain = CreateThread(NULL, 0, MainThread, this, 0, NULL);
	if(m_hThreadMain == INVALID_HANDLE_VALUE)
	{
		OnCancel();
		return FALSE;
	}
	EnterCriticalSection(&m_csFinalResult);
	m_iResultCount = (int)m_FinalResultList.GetCount();
	LeaveCriticalSection(&m_csFinalResult);
	m_iResultDelay = 0;
	return TRUE;
}

BOOL CResultViewDlg::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN)
	{
		switch(pMsg->wParam)
		{
		case VK_ESCAPE:
			return TRUE;
		case VK_RETURN:
			return TRUE;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CResultViewDlg::OnPaint()
{
	CDialog::OnPaint();
	if(m_lpOffScreen)
	{
		RECT cRect;
		::GetClientRect(GetDlgItem(IDC_STATIC_MAP)->GetSafeHwnd(), &cRect);
		HDC hSrcDC, hWinDC;
		m_lpOffScreen->GetDC(&hSrcDC);
		hWinDC = ::GetDC(GetDlgItem(IDC_STATIC_MAP)->GetSafeHwnd());
		SetStretchBltMode(hWinDC, COLORONCOLOR);
		StretchBlt(hWinDC, 0, 0, cRect.right-cRect.left, cRect.bottom-cRect.top,
			hSrcDC, 0, 0,1024, 786, SRCCOPY);
		::ReleaseDC(GetDlgItem(IDC_STATIC_MAP)->GetSafeHwnd(), hWinDC);
		m_lpOffScreen->ReleaseDC(hSrcDC);
	}
	if(m_lpPlate)
	{
		RECT cRect;
		::GetClientRect(GetDlgItem(IDC_STATIC_PLATE)->GetSafeHwnd(), &cRect);
		HDC hSrcDC, hWinDC;
		m_lpPlate->GetDC(&hSrcDC);
		hWinDC = ::GetDC(GetDlgItem(IDC_STATIC_PLATE)->GetSafeHwnd());
		SetStretchBltMode(hWinDC, COLORONCOLOR);
		StretchBlt(hWinDC, 0, 0, cRect.right-cRect.left, cRect.bottom-cRect.top,
			hSrcDC, 0, 0,318, 80, SRCCOPY);
		::ReleaseDC(GetDlgItem(IDC_STATIC_PLATE)->GetSafeHwnd(), hWinDC);
		m_lpPlate->ReleaseDC(hSrcDC);
	}
	if(m_lpRPic)
	{
		RECT cRect;
		::GetClientRect(GetDlgItem(IDC_STATIC_RPIC)->GetSafeHwnd(), &cRect);
		HDC hSrcDC, hWinDC;
		m_lpRPic->GetDC(&hSrcDC);
		hWinDC = ::GetDC(GetDlgItem(IDC_STATIC_RPIC)->GetSafeHwnd());
		SetStretchBltMode(hWinDC, COLORONCOLOR);
		StretchBlt(hWinDC, 0, 0, cRect.right-cRect.left, cRect.bottom-cRect.top,
			hSrcDC, 0, 0,318, 240, SRCCOPY);
		::ReleaseDC(GetDlgItem(IDC_STATIC_RPIC)->GetSafeHwnd(), hWinDC);
		m_lpRPic->ReleaseDC(hSrcDC);
	}
	if(m_lpCPic)
	{
		RECT cRect;
		::GetClientRect(GetDlgItem(IDC_STATIC_CPIC)->GetSafeHwnd(), &cRect);
		HDC hSrcDC, hWinDC;
		m_lpCPic->GetDC(&hSrcDC);
		hWinDC = ::GetDC(GetDlgItem(IDC_STATIC_CPIC)->GetSafeHwnd());
		SetStretchBltMode(hWinDC, COLORONCOLOR);
		StretchBlt(hWinDC, 0, 0, cRect.right-cRect.left, cRect.bottom-cRect.top,
			hSrcDC, 0, 0,318, 240, SRCCOPY);
		::ReleaseDC(GetDlgItem(IDC_STATIC_CPIC)->GetSafeHwnd(), hWinDC);
		m_lpCPic->ReleaseDC(hSrcDC);
	}
}

void CResultViewDlg::PrepareOutPut()
{
	if(m_lpOffScreen)
	{
		HDC hOffDC;
		m_lpOffScreen->GetDC(&hOffDC);
		if(m_lpBack)
		{
			HDC hBackDC;
			m_lpBack->GetDC(&hBackDC);
			BitBlt(hOffDC, 0, 0, 1024, 786, hBackDC, 0, 0, SRCCOPY);
			m_lpBack->ReleaseDC(hBackDC);
		}

		EnterCriticalSection(&m_csLocaList);
		POSITION posLoca = m_LocaList.GetHeadPosition();
		while(posLoca)
		{
			LocaType* pTmpLoca = m_LocaList.GetNext(posLoca);
			DrawEllsip(hOffDC, pTmpLoca->iPositionX-10, pTmpLoca->iPositionY-10, 
				pTmpLoca->iPositionX+10, pTmpLoca->iPositionY+10, RGB(0, 255, 0), RGB(0, 255, 0));
		}
		LeaveCriticalSection(&m_csLocaList);

		m_lpOffScreen->ReleaseDC(hOffDC);
	}
}

void CResultViewDlg::ShowPosInfo(HDC hDC, int x, int y, char* pszInfo, int iFrameWidth, COLORREF color1, COLORREF color2)
{
	HPEN hPen;
	HBRUSH hBrush;
	hPen = CreatePen(PS_SOLID, iFrameWidth, color1);
	hBrush = CreateSolidBrush(color2);
	SelectObject(hDC, hPen);
	SelectObject(hDC, hBrush);
	BOOL fIsLeft = FALSE;
	BOOL fIsButton = FALSE;

	if(x > 764) fIsLeft = TRUE;
	if(y < 60) fIsButton = TRUE;

	int StartX;
	int StartY;
	if(fIsLeft) StartX = x - 230;
	else StartX = x + 30;
	if(fIsButton) StartY = y + 20;
	else StartY = y - 50;
	int iWidth = 0;
	if(fIsLeft) iWidth = 200;
	else iWidth = 200;

	RoundRect(hDC, StartX, StartY, StartX+iWidth, StartY+30, 30, 15);
	POINT cPoint1, cPoint2;
	if(fIsLeft)
	{
		cPoint1.x = StartX+200;
		cPoint2.x = StartX+180;
	}
	else
	{
		cPoint1.x = StartX;
		cPoint2.x = StartX+10;
	}
	if(fIsButton)
	{
		cPoint1.y = StartY+5;
		cPoint2.y = StartY;
	}
	else
	{
		cPoint1.y = StartY+25;
		cPoint2.y = StartY+30;
	}

	DrawLine(hDC, x, y, cPoint1.x, cPoint1.y, iFrameWidth, color1);
	DrawLine(hDC, x, y, cPoint2.x, cPoint2.y, iFrameWidth, color1);

	SetTextColor(hDC, RGB(0, 0, 0));
	SetBkMode(hDC, 3);
	int iTextLen = (int)strlen(pszInfo);
	if(iTextLen > 26) iTextLen = 26;
	TextOut(hDC, StartX+5, StartY+5, pszInfo, iTextLen);

	DeleteObject(hBrush);
	DeleteObject(hPen);
}

void CResultViewDlg::GetPos()
{
	RECT cRect;
	GetDlgItem(IDC_STATIC_MAP)->GetClientRect(&cRect);
	GetDlgItem(IDC_STATIC_MAP)->ClientToScreen((LPPOINT)&cRect);
	GetDlgItem(IDC_STATIC_MAP)->ClientToScreen((LPPOINT)&cRect+1);
	POINT cPoint;
	GetCursorPos(&cPoint);
	cPoint.x -= cRect.left;
	cPoint.y -= cRect.top;
	m_TmpPoint.x = cPoint.x;
	m_TmpPoint.y = cPoint.y;
}

void CResultViewDlg::ShowCurrentResultInfo(HDC hDC)
{
	EnterCriticalSection(&m_csFinalResult);
	if(m_iCurrentSelect < 0 || m_iCurrentSelect >= m_FinalResultList.GetCount())
	{
		LeaveCriticalSection(&m_csFinalResult);
		return;
	}
	POSITION posFinalResult = m_FinalResultList.FindIndex(m_iCurrentSelect);
	if(posFinalResult)
	{
		FinalResultType* pTmpFinalResult = m_FinalResultList.GetAt(posFinalResult);
		DrawLine(hDC, pTmpFinalResult->Loca.x-10, pTmpFinalResult->Loca.y, pTmpFinalResult->Loca.x,
			pTmpFinalResult->Loca.y-20,3, RGB(255, 0, 0));
		DrawLine(hDC, pTmpFinalResult->Loca.x, pTmpFinalResult->Loca.y-20, pTmpFinalResult->Loca.x+10,
			pTmpFinalResult->Loca.y,3, RGB(255, 0, 0));
		DrawLine(hDC, pTmpFinalResult->Loca.x+10, pTmpFinalResult->Loca.y, pTmpFinalResult->Loca.x,
			pTmpFinalResult->Loca.y+20,3, RGB(255, 0, 0));
		DrawLine(hDC, pTmpFinalResult->Loca.x, pTmpFinalResult->Loca.y+20, pTmpFinalResult->Loca.x-10,
			pTmpFinalResult->Loca.y,3, RGB(255, 0, 0));

		int iLen = (int)strlen(pTmpFinalResult->szLocaName);
		int iLeft = pTmpFinalResult->Loca.x - iLen*3;
		int iTop = pTmpFinalResult->Loca.y + 22;

		if(iLeft < 0 ) iLeft = 0;
		if(iLeft > 824) iLeft = 824;
		if(iTop < 0) iTop = 0;
		if(iTop > 756) iTop = 756;
		DrawText(hDC, iLeft, iTop, NULL, pTmpFinalResult->szLocaName, iLen, RGB(255, 0, 0));

		if(m_TmpPoint.x>=pTmpFinalResult->Loca.x-10 && m_TmpPoint.x<=pTmpFinalResult->Loca.x+10
			&& m_TmpPoint.y>=pTmpFinalResult->Loca.y-10 && m_TmpPoint.y<=pTmpFinalResult->Loca.y+10)
		{
			ShowPosInfo(hDC, pTmpFinalResult->Loca.x, pTmpFinalResult->Loca.y, pTmpFinalResult->szPassTime, 3, 
				RGB(255, 0, 0), RGB(255, 255, 255));
		}
		else
		{
			m_iDelay++;
			if(m_iDelay >= 0 && m_iDelay <= 5)
			ShowPosInfo(hDC, pTmpFinalResult->Loca.x, pTmpFinalResult->Loca.y, pTmpFinalResult->szPassTime, 3, 
				RGB(255, 0, 0), RGB(255, 255, 255));
			else if(m_iDelay > 5 && m_iDelay <= 10)
			{

			}
			else
			{
				m_iDelay = 0;
			}
		}
	}
	LeaveCriticalSection(&m_csFinalResult);
}

DWORD WINAPI CResultViewDlg::MainThread(LPVOID pParam)
{
	CResultViewDlg* pDlg = (CResultViewDlg*)pParam;
	while(!pDlg->m_fExit)
	{
		pDlg->PrepareOutPut();
		if(pDlg->m_fIsNeedUpDatePicBuffer == TRUE)
		{
			pDlg->UpDateResultPic();
		}
		pDlg->GetPos();	
		HDC hDC;
		pDlg->m_lpOffScreen->GetDC(&hDC);
		pDlg->ShowCurrentResultInfo(hDC);
		pDlg->m_lpOffScreen->ReleaseDC(hDC);

		if(pDlg->m_iResultCount > 1)
		{
			pDlg->m_iResultDelay++;
			if(pDlg->m_iResultDelay >= 300)
			{
				pDlg->m_iResultDelay = 0;
				pDlg->m_iCurrentSelect++;
				if(pDlg->m_iCurrentSelect >= pDlg->m_iResultCount)
				{
					pDlg->m_iCurrentSelect = 0;
				}
			}
		}
		pDlg->SendMessage(WM_PAINT, 0, 0);
		Sleep(100);
	}
	return 0;
}

void CResultViewDlg::OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	int iColumn = pNMListView->iItem;
	if(iColumn < 0 || iColumn > m_ListFinalResult.GetItemCount())
	{
		return;
	}
	m_iCurrentSelect = iColumn;
	m_fIsNeedUpDatePicBuffer = TRUE;
	*pResult = 0;
}

void CResultViewDlg::UpDateResultPic()
{
	m_fIsNeedUpDatePicBuffer = FALSE;
	EnterCriticalSection(&m_csFinalResult);
	if(m_iCurrentSelect < 0 || m_iCurrentSelect >= m_FinalResultList.GetCount())
	{
		LeaveCriticalSection(&m_csFinalResult);
		return;
	}
	POSITION posFinalResult = m_FinalResultList.FindIndex(m_iCurrentSelect);
	if(posFinalResult)
	{
		FinalResultType* pTmpFinalResult = m_FinalResultList.GetAt(posFinalResult);
		CString strTmpFilePath = "";
		if(m_fLoadFromFile)
		{
			strTmpFilePath = pTmpFinalResult->szRPicPath;
		}
		else
		{
			char szCurrentPath[MAX_PATH] = {0};
			GetModuleFileName(NULL, szCurrentPath, MAX_PATH);
			PathRemoveFileSpec(szCurrentPath);
			strTmpFilePath = szCurrentPath;
			strTmpFilePath += "\\TmpFinalResult\\";
			CString strFileName = pTmpFinalResult->szRPicPath;
			int iIndex = strFileName.ReverseFind('\\');
			strFileName = strFileName.Mid(iIndex+1, strFileName.GetLength()-iIndex);
			strTmpFilePath += strFileName;
		}
		if(PathFileExists(strTmpFilePath.GetBuffer()))
		{
			IPicture* pPic;
			IStream* pStm;
			HANDLE hFile = NULL;
			DWORD dwFileSize, dwByteRead;
			hFile = CreateFile(strTmpFilePath.GetBuffer(), GENERIC_READ, FILE_SHARE_READ, NULL,
				OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if(hFile == INVALID_HANDLE_VALUE)
			{
				return;
			}
			dwFileSize = GetFileSize(hFile, NULL);
			if(dwFileSize == 0xFFFFFFFF)
			{
				CloseHandle(hFile);
				return;
			}
			HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, dwFileSize);
			LPVOID pvData = NULL;
			if(hGlobal == NULL)
			{
				CloseHandle(hFile);
				return;
			}
			if((pvData = GlobalLock(hGlobal)) == NULL)
			{
				CloseHandle(hFile);
				GlobalFree(hGlobal);
				return;
			}
			ReadFile(hFile, pvData, dwFileSize, &dwByteRead, NULL);
			GlobalUnlock(hGlobal);
			CloseHandle(hFile);

			HDC hTmpDC = ::GetDC(GetDlgItem(IDC_STATIC_RPIC)->GetSafeHwnd());
			CreateStreamOnHGlobal(hGlobal, TRUE, &pStm);
			if(pStm == NULL)
			{
				GlobalFree(hGlobal);
				return;
			}
			HRESULT hResult = OleLoadPicture(pStm, dwFileSize, TRUE, IID_IPicture, (LPVOID*)&pPic);
			if(FAILED(hResult))
			{
				pStm->Release();
				GlobalFree(hGlobal);
			}

			OLE_XSIZE_HIMETRIC hmWidth;
			OLE_YSIZE_HIMETRIC hmHeight;
			pPic->get_Width(&hmWidth);
			pPic->get_Height(&hmHeight);
			int iWidth = MulDiv(hmWidth, GetDeviceCaps(hTmpDC, LOGPIXELSX), 2540);
			int iHeight = MulDiv(hmHeight, GetDeviceCaps(hTmpDC, LOGPIXELSY), 2540);
			::ReleaseDC(GetDlgItem(IDC_STATIC_RPIC)->GetSafeHwnd(), hTmpDC);

			HDC hTmpDC1;
			m_lpRPic->GetDC(&hTmpDC1);
			pPic->Render(hTmpDC1, 0, 0, 318, 240, 0, hmHeight, hmWidth, -hmHeight, NULL);
			m_lpRPic->ReleaseDC(hTmpDC1);
			pPic->Release();
			pStm->Release();
			GlobalFree(hGlobal);
			m_strRPicPath = strTmpFilePath;
		}
		else
		{
			HDC hTmpDC1;
			m_lpRPic->GetDC(&hTmpDC1);
			SelectObject(hTmpDC1, g_hFontMin);
			DrawRectangle(hTmpDC1, 0, 0, 318, 240, RGB(250, 250, 250), RGB(250, 250, 250));
			CString strFileName = pTmpFinalResult->szRPicPath;
			int iIndexChar = strFileName.ReverseFind('\\');
			strFileName = strFileName.Mid(iIndexChar+1, strFileName.GetLength()-iIndexChar);
			DrawTextThis(hTmpDC1, 10, 10, "识别图:", 5, RGB(32, 32, 32));
			iIndexChar = strFileName.GetLength();
			if(iIndexChar > 50)
			{
				iIndexChar = 50;
			}
			DrawTextThis(hTmpDC1, 10, 30, strFileName.GetBuffer(), iIndexChar, RGB(32, 32, 32));
			DrawTextThis(hTmpDC1, 10, 50, "获取失败!", 9, RGB(32, 32, 32));
			m_lpRPic->ReleaseDC(hTmpDC1);
			m_strRPicPath = "";
		}
		if(m_fLoadFromFile)
		{
			strTmpFilePath = pTmpFinalResult->szCPicPath;
		}
		else
		{
			char szCurrentPath[MAX_PATH] = {0};
			GetModuleFileName(NULL, szCurrentPath, MAX_PATH);
			PathRemoveFileSpec(szCurrentPath);
			strTmpFilePath = szCurrentPath;
			strTmpFilePath += "\\TmpFinalResult\\";
			CString strFileName = pTmpFinalResult->szCPicPath;
			int iIndex = strFileName.ReverseFind('\\');
			strFileName = strFileName.Mid(iIndex+1, strFileName.GetLength()-iIndex);
			strTmpFilePath += strFileName;
		}
		if(PathFileExists(strTmpFilePath.GetBuffer()))
		{
			IPicture* pPic;
			IStream* pStm;
			HANDLE hFile = NULL;
			DWORD dwFileSize, dwByteRead;
			hFile = CreateFile(strTmpFilePath.GetBuffer(), GENERIC_READ, FILE_SHARE_READ, NULL,
				OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if(hFile == INVALID_HANDLE_VALUE)
			{
				return;
			}
			dwFileSize = GetFileSize(hFile, NULL);
			if(dwFileSize == 0xFFFFFFFF)
			{
				CloseHandle(hFile);
				return;
			}
			HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, dwFileSize);
			LPVOID pvData = NULL;
			if(hGlobal == NULL)
			{
				CloseHandle(hFile);
				return;
			}
			if((pvData = GlobalLock(hGlobal)) == NULL)
			{
				CloseHandle(hFile);
				GlobalFree(hGlobal);
				return;
			}
			ReadFile(hFile, pvData, dwFileSize, &dwByteRead, NULL);
			GlobalUnlock(hGlobal);
			CloseHandle(hFile);

			HDC hTmpDC = ::GetDC(GetDlgItem(IDC_STATIC_CPIC)->GetSafeHwnd());
			CreateStreamOnHGlobal(hGlobal, TRUE, &pStm);
			if(pStm == NULL)
			{
				GlobalFree(hGlobal);
				return;
			}
			HRESULT hResult = OleLoadPicture(pStm, dwFileSize, TRUE, IID_IPicture, (LPVOID*)&pPic);
			if(FAILED(hResult))
			{
				pStm->Release();
				GlobalFree(hGlobal);
			}

			OLE_XSIZE_HIMETRIC hmWidth;
			OLE_YSIZE_HIMETRIC hmHeight;
			pPic->get_Width(&hmWidth);
			pPic->get_Height(&hmHeight);
			int iWidth = MulDiv(hmWidth, GetDeviceCaps(hTmpDC, LOGPIXELSX), 2540);
			int iHeight = MulDiv(hmHeight, GetDeviceCaps(hTmpDC, LOGPIXELSY), 2540);
			::ReleaseDC(GetDlgItem(IDC_STATIC_CPIC)->GetSafeHwnd(), hTmpDC);

			HDC hTmpDC1;
			m_lpCPic->GetDC(&hTmpDC1);
			pPic->Render(hTmpDC1, 0, 0, 318, 240, 0, hmHeight, hmWidth, -hmHeight, NULL);
			m_lpCPic->ReleaseDC(hTmpDC1);
			pPic->Release();
			pStm->Release();
			GlobalFree(hGlobal);
			m_strCPicPath = strTmpFilePath;
		}
		else
		{
			HDC hTmpDC1;
			m_lpCPic->GetDC(&hTmpDC1);
			SelectObject(hTmpDC1, g_hFontMin);
			DrawRectangle(hTmpDC1, 0, 0, 318, 240, RGB(250, 250, 250), RGB(250, 250, 250));
			CString strFileName = pTmpFinalResult->szCPicPath;
			int iIndexChar = strFileName.ReverseFind('\\');
			strFileName = strFileName.Mid(iIndexChar+1, strFileName.GetLength()-iIndexChar);
			DrawTextThis(hTmpDC1, 10, 10, "抓拍图:", 5, RGB(32, 32, 32));
			iIndexChar = strFileName.GetLength();
			if(iIndexChar > 50)
			{
				iIndexChar = 50;
			}
			DrawTextThis(hTmpDC1, 10, 30, strFileName.GetBuffer(), iIndexChar, RGB(32, 32, 32));
			DrawTextThis(hTmpDC1, 10, 50, "获取失败!", 9, RGB(32, 32, 32));
			m_lpCPic->ReleaseDC(hTmpDC1);
			m_strCPicPath = "";
		}
	}
	LeaveCriticalSection(&m_csFinalResult);
}

BOOL CResultViewDlg::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	switch(message)
	{
	case WM_UPDATE_MESSAGE:
		OnUpdatePicBuffer();
		break;
	}
	return CDialog::OnWndMsg(message, wParam, lParam, pResult);
}

void CResultViewDlg::OnUpdatePicBuffer()
{
	UpDateResultPic();
}

void CResultViewDlg::OnPrevResult()
{
	m_ListFinalResult.EnableWindow(FALSE);
	m_iResultDelay = 0;
	m_iCurrentSelect--;
	if(m_iCurrentSelect < 0)
	{
		m_iCurrentSelect = m_iResultCount - 1;
	}
	m_fIsNeedUpDatePicBuffer = TRUE;
	m_ListFinalResult.EnableWindow(TRUE);
	m_ListFinalResult.SetFocus();
}

void CResultViewDlg::OnNextResult()
{
	m_ListFinalResult.EnableWindow(FALSE);
	m_iResultDelay = 0;
	m_iCurrentSelect++;
	if(m_iCurrentSelect >= m_iResultCount)
	{
		m_iCurrentSelect = 0;
	}
	m_fIsNeedUpDatePicBuffer = TRUE;
	m_ListFinalResult.EnableWindow(TRUE);
	m_ListFinalResult.SetFocus();
}

void CResultViewDlg::OnLvnKeydownList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);
	if(pLVKeyDow->wVKey == VK_DOWN)
	{
		OnNextResult();
	}
	else if(pLVKeyDow->wVKey == VK_UP)
	{
		OnPrevResult();
	}
	*pResult = 0;
}

void CResultViewDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CRect cTmpRect;
	GetDlgItem(IDC_STATIC_RPIC)->GetWindowRect(&cTmpRect);
	ScreenToClient(&cTmpRect);
	if(cTmpRect.PtInRect(point))
	{
		if(PathFileExists(m_strRPicPath.GetBuffer()))
		{
			IPicture* pPic;
			IStream* pStm;
			HANDLE hFile = NULL;
			DWORD dwFileSize, dwByteRead;
			hFile = CreateFile(m_strRPicPath.GetBuffer(), GENERIC_READ, FILE_SHARE_READ, NULL,
				OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if(hFile == INVALID_HANDLE_VALUE)
			{
				return;
			}
			dwFileSize = GetFileSize(hFile, NULL);
			if(dwFileSize == 0xFFFFFFFF)
			{
				CloseHandle(hFile);
				return;
			}
			HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, dwFileSize);
			LPVOID pvData = NULL;
			if(hGlobal == NULL)
			{
				CloseHandle(hFile);
				return;
			}
			if((pvData = GlobalLock(hGlobal)) == NULL)
			{
				CloseHandle(hFile);
				GlobalFree(hGlobal);
				return;
			}
			ReadFile(hFile, pvData, dwFileSize, &dwByteRead, NULL);
			GlobalUnlock(hGlobal);
			CloseHandle(hFile);

			HDC hTmpDC = ::GetDC(GetDlgItem(IDC_STATIC_R_PIC)->GetSafeHwnd());
			CreateStreamOnHGlobal(hGlobal, TRUE, &pStm);
			if(pStm == NULL)
			{
				GlobalFree(hGlobal);
				return;
			}
			HRESULT hResult = OleLoadPicture(pStm, dwFileSize, TRUE, IID_IPicture, (LPVOID*)&pPic);
			if(FAILED(hResult))
			{
				pStm->Release();
				GlobalFree(hGlobal);
			}

			OLE_XSIZE_HIMETRIC hmWidth;
			OLE_YSIZE_HIMETRIC hmHeight;
			pPic->get_Width(&hmWidth);
			pPic->get_Height(&hmHeight);
			int iWidth = MulDiv(hmWidth, GetDeviceCaps(hTmpDC, LOGPIXELSX), 2540);
			int iHeight = MulDiv(hmHeight, GetDeviceCaps(hTmpDC, LOGPIXELSY), 2540);
			::ReleaseDC(GetDlgItem(IDC_STATIC_R_PIC)->GetSafeHwnd(), hTmpDC);

			LPDIRECTDRAWSURFACE7 lpTmp;
			DDSURFACEDESC2 ddsd;
			ZeroMemory(&ddsd, sizeof(ddsd));
			ddsd.dwSize = sizeof(ddsd);
			ddsd.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
			ddsd.dwWidth = iWidth;
			ddsd.dwHeight = iHeight;
			ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
			m_lpDDS->CreateSurface(&ddsd, &lpTmp, NULL);
			HDC hTmpDC1;
			if(lpTmp)
			{
				lpTmp->GetDC(&hTmpDC1);
				pPic->Render(hTmpDC1, 0, 0, iWidth, iHeight, 0, hmHeight, hmWidth, -hmHeight, NULL);
				lpTmp->ReleaseDC(hTmpDC1);
			}
			pPic->Release();
			pStm->Release();
			GlobalFree(hGlobal);
			if(lpTmp)
			{
				CPicView* pViewPicDlg = new CPicView(lpTmp, this);
				pViewPicDlg->DoModal();
				delete pViewPicDlg;
				pViewPicDlg = NULL;
				lpTmp->Release();
				lpTmp = NULL;
			}
		}
		CDialog::OnLButtonDblClk(nFlags, point);
		return;
	}

	GetDlgItem(IDC_STATIC_CPIC)->GetWindowRect(&cTmpRect);
	ScreenToClient(&cTmpRect);
	if(cTmpRect.PtInRect(point))
	{
		if(PathFileExists(m_strCPicPath.GetBuffer()))
		{
			IPicture* pPic;
			IStream* pStm;
			HANDLE hFile = NULL;
			DWORD dwFileSize, dwByteRead;
			hFile = CreateFile(m_strCPicPath.GetBuffer(), GENERIC_READ, FILE_SHARE_READ, NULL,
				OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if(hFile == INVALID_HANDLE_VALUE)
			{
				return;
			}
			dwFileSize = GetFileSize(hFile, NULL);
			if(dwFileSize == 0xFFFFFFFF)
			{
				CloseHandle(hFile);
				return;
			}
			HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, dwFileSize);
			LPVOID pvData = NULL;
			if(hGlobal == NULL)
			{
				CloseHandle(hFile);
				return;
			}
			if((pvData = GlobalLock(hGlobal)) == NULL)
			{
				CloseHandle(hFile);
				GlobalFree(hGlobal);
				return;
			}
			ReadFile(hFile, pvData, dwFileSize, &dwByteRead, NULL);
			GlobalUnlock(hGlobal);
			CloseHandle(hFile);

			HDC hTmpDC = ::GetDC(GetDlgItem(IDC_STATIC_R_PIC)->GetSafeHwnd());
			CreateStreamOnHGlobal(hGlobal, TRUE, &pStm);
			if(pStm == NULL)
			{
				GlobalFree(hGlobal);
				return;
			}
			HRESULT hResult = OleLoadPicture(pStm, dwFileSize, TRUE, IID_IPicture, (LPVOID*)&pPic);
			if(FAILED(hResult))
			{
				pStm->Release();
				GlobalFree(hGlobal);
			}

			OLE_XSIZE_HIMETRIC hmWidth;
			OLE_YSIZE_HIMETRIC hmHeight;
			pPic->get_Width(&hmWidth);
			pPic->get_Height(&hmHeight);
			int iWidth = MulDiv(hmWidth, GetDeviceCaps(hTmpDC, LOGPIXELSX), 2540);
			int iHeight = MulDiv(hmHeight, GetDeviceCaps(hTmpDC, LOGPIXELSY), 2540);
			::ReleaseDC(GetDlgItem(IDC_STATIC_R_PIC)->GetSafeHwnd(), hTmpDC);

			LPDIRECTDRAWSURFACE7 lpTmp;
			DDSURFACEDESC2 ddsd;
			ZeroMemory(&ddsd, sizeof(ddsd));
			ddsd.dwSize = sizeof(ddsd);
			ddsd.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
			ddsd.dwWidth = iWidth;
			ddsd.dwHeight = iHeight;
			ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
			m_lpDDS->CreateSurface(&ddsd, &lpTmp, NULL);
			HDC hTmpDC1;
			if(lpTmp)
			{
				lpTmp->GetDC(&hTmpDC1);
				pPic->Render(hTmpDC1, 0, 0, iWidth, iHeight, 0, hmHeight, hmWidth, -hmHeight, NULL);
				lpTmp->ReleaseDC(hTmpDC1);
			}
			pPic->Release();
			pStm->Release();
			GlobalFree(hGlobal);
			if(lpTmp)
			{
				CPicView* pViewPicDlg = new CPicView(lpTmp, this);
				pViewPicDlg->DoModal();
				delete pViewPicDlg;
				pViewPicDlg = NULL;
				lpTmp->Release();
				lpTmp = NULL;
			}
		}
		CDialog::OnLButtonDblClk(nFlags, point);
		return;
	}
	CDialog::OnLButtonDblClk(nFlags, point);
}
