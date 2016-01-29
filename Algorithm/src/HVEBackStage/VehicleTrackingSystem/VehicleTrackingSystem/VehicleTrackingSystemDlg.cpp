#include "stdafx.h"
#include "VehicleTrackingSystem.h"
#include "VehicleTrackingSystemDlg.h"
#include ".\vehicletrackingsystemdlg.h"
#include "GetInternetMapDlg.h"
#include "SetLocaPoint.h"
#include "EditRoadLine.h"
#include "atlpath.h"
#include "LoadingFrame.h"
#include "TestNetWork.h"
#include "PicView.h"
#include "iphlpapi.h"

#include "CApplication.h"
#include "CWorkbook.h"
#include "CWorkbooks.h"
#include "CWorksheet.h"
#include "CWorksheets.h"
#include "CRange.h"

#include <Winnetwk.h>


#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "Mpr.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WM_GET_DEV_LIST			WM_USER + 1230
#define WM_UPDATE_SEARCH_INFO	WM_USER + 1240
#define WM_PROC_SEARCH_DONE		WM_USER + 1250
#define WM_SERCH1_DONE			WM_USER + 1260
#define WM_GET_LOCAINFO_DONE	WM_USER + 1270
#define WM_START_SYNC_SEARCH	WM_USER + 1280

#define ID_SETSTART		2222
#define ID_UNSETSTART	3333
#define ID_SETEND		4444
#define ID_UNSETEND		5555

#define ID_EDIT_SERVER		8888
#define ID_REMOVE_SERVER	9999

extern "C" {
#include "jpeglib.h"
}
#pragma comment(lib, "JPEGLIB2.lib")

CShowLoading* g_pShowLoading = NULL;
CTestNetWork* g_TestNet = NULL;
CRITICAL_SECTION g_csTestNet;

HFONT g_hFontComm;
HFONT g_hFontMin;
CString g_strMacAddr = "";

HRESULT WirteToJpgFile(LPCSTR szFileName, unsigned char* pbData, int iWidth, int iHeight, int iQuality)
{
	if(pbData == NULL || iWidth <= 0 || iHeight <= 0)
	{
		return S_OK;
	}
	struct jpeg_compress_struct cInfo;
	struct jpeg_error_mgr cJerr;
	FILE* pOutFile;
	int iRowStride;
	cInfo.err = jpeg_std_error(&cJerr);
	jpeg_create_compress(&cInfo);
	pOutFile = fopen(szFileName, "wb");
	if(pOutFile == NULL)
	{
		jpeg_destroy_compress(&cInfo);
		return E_FAIL;
	}
	jpeg_stdio_dest(&cInfo, pOutFile);
	cInfo.image_width = iWidth;
	cInfo.image_height = iHeight;
	cInfo.input_components = 3;
	cInfo.in_color_space = JCS_RGB;
	jpeg_set_defaults(&cInfo);
	jpeg_set_quality(&cInfo, iQuality, true);
	jpeg_start_compress(&cInfo, true);
	LPBYTE pRowPointer;
	iRowStride = cInfo.image_width * 3;
	while(cInfo.next_scanline < cInfo.image_height)
	{
		pRowPointer = pbData + cInfo.next_scanline * iRowStride;
		jpeg_write_scanlines(&cInfo, &pRowPointer, 1);
	}
	jpeg_finish_compress(&cInfo);
	jpeg_destroy_compress(&cInfo);
	fclose(pOutFile);
	return S_OK;
}

BOOL IsTableExists(_ConnectionPtr& pConnect, CString strTableName)
{
	if(pConnect == NULL) return FALSE;
	CString strSql;
	HRESULT hr = E_FAIL;
	strSql.Format("SELECT name FROM sysobjects WHERE (name = '%s')", strTableName.GetBuffer());
	_RecordsetPtr pPtr;
	hr = pPtr.CreateInstance("ADODB.Recordset");
	if(hr != S_OK) return FALSE;
	hr = pPtr->Open(_variant_t(strSql), pConnect.GetInterfacePtr(), 
		adOpenDynamic, adLockOptimistic, adCmdText);
	if(hr != S_OK) return FALSE;
	if(pPtr->GetadoEOF())
	{
		pPtr->Close();
		pPtr.Release();
		return FALSE;
	}
	pPtr->Close();
	pPtr.Release();
	return TRUE;
}









CSearchCarPassLocaType::CSearchCarPassLocaType(NewSeverType* pServer, char* pszSQL, BOOL fIsSetTime, CTime cStartTime, CTime cEndTime)
{
	memcpy(&m_Server, pServer, sizeof(NewSeverType));
	if(pszSQL)
	{
		int  iLen = (int)strlen(pszSQL);
		if(iLen > 0)
		m_pszSQL = new char[iLen+1];
		memcpy(m_pszSQL, pszSQL, iLen);
		m_pszSQL[iLen] = '\0';
	}
	m_Status = -1;
	m_fIsSetTime = fIsSetTime;
	m_cStartTime = cStartTime;
	m_cEndTime = cEndTime;
	InitializeCriticalSection(&m_csResultList);
}

CSearchCarPassLocaType::~CSearchCarPassLocaType()
{
	if(m_hProcess)
	{
		int iWaitTimes = 0;
		int MAXWAITTIME = 8;
		while(WaitForSingleObject(m_hProcess, 500) == WAIT_TIMEOUT && iWaitTimes < MAXWAITTIME)
		{
			iWaitTimes++;
		}

		if(iWaitTimes >= MAXWAITTIME)
		{
			TerminateThread(m_hProcess, 0);
		}
		else
		{
			CloseHandle(m_hProcess);
		}
	}
	EnterCriticalSection(&m_csResultList);
	while(m_ResultList.GetCount() > 0)
	{
		ResultInfoType* pTmpResult = m_ResultList.RemoveHead();
		delete pTmpResult;
		pTmpResult = NULL;
	}
	LeaveCriticalSection(&m_csResultList);
	DeleteCriticalSection(&m_csResultList);
	if(m_pszSQL)
	{
		delete[] m_pszSQL;
		m_pszSQL = NULL;
	}
}

int CSearchCarPassLocaType::GetStatus()
{
	return m_Status;
}

void CSearchCarPassLocaType::ClearLastResult()
{
	EnterCriticalSection(&m_csResultList);
	while(m_ResultList.GetCount() > 0)
	{
		ResultInfoType* pTmpResult = m_ResultList.RemoveHead();
		delete pTmpResult;
		pTmpResult = NULL;
	}
	LeaveCriticalSection(&m_csResultList);
}

DWORD WINAPI CSearchCarPassLocaType::ProcessThread(LPVOID lpParam)
{
	if(lpParam == NULL)
	{
		return 0xFFFFFFFF;
	}
	CSearchCarPassLocaType* pPoint = (CSearchCarPassLocaType*)lpParam;
	pPoint->m_Status = 1;
	
	if(pPoint->m_pszSQL == NULL || strlen(pPoint->m_pszSQL) <= 0)
	{
		pPoint->m_Status = 20086;
		return 0xFFFFFFFF;
	}

	int iSucessTimes = 0;
	EnterCriticalSection(&g_csTestNet);
	g_TestNet->StartTest(pPoint->m_Server.szServerIP, iSucessTimes);
	LeaveCriticalSection(&g_csTestNet);
	if(iSucessTimes <= 0)
	{
		pPoint->m_Status = 20086;
		return 0xFFFFFFFF;
	}
	::CoInitialize(NULL);
	CString strConnect;
	strConnect.Format("Provider=SQLOLEDB.1;Password=%s;User ID=%s;Initial Catalog=%s;Data Source=%s",
		pPoint->m_Server.szPassWord, pPoint->m_Server.szUserName, pPoint->m_Server.szDataBaseName, pPoint->m_Server.szServerIP);
	_ConnectionPtr pConnect = NULL;
	HRESULT hr = S_OK;
	try
	{
		hr = pConnect.CreateInstance("ADODB.Connection");
		if(SUCCEEDED(hr))
		{
			pConnect->ConnectionTimeout = 10;
			pConnect->CommandTimeout = 0;
			pConnect->CursorLocation = adUseServer;
			pConnect->IsolationLevel = adXactReadCommitted;
			hr = pConnect->Open((_bstr_t)(strConnect.GetBuffer()), "", "", adModeUnknown);
		}
	}
	catch (_com_error e)
	{
		if(pConnect != NULL)
		{
			pConnect->Cancel();
		}
		::CoUninitialize();
		pPoint->m_Status =20086;
		return 0xFFFFFFFF;
	}

	if(FAILED(hr))
	{
		::CoUninitialize();
		pPoint->m_Status = 20086;
		return 0xFFFFFFFF;
	}
	
	_RecordsetPtr pRecordset = NULL;
	try
	{
		CString strTableList = "";
		CString strSql;
		CString strViewName = "VTHVE_DATA_VIEW_";
		strViewName += g_strMacAddr;
		_variant_t RecordsetAffected;
		strSql.Format("IF EXISTS (SELECT TABLE_NAME FROM INFORMATION_SCHEMA.VIEWS WHERE TABLE_NAME = N'%s') DROP VIEW %s",
			strViewName.GetBuffer(), strViewName.GetBuffer());
		pConnect->Execute((_bstr_t)strSql.GetBuffer(), &RecordsetAffected, adCmdText);
		if(pPoint->m_fIsSetTime)
		{
			DWORD64 dwTmpStartTime = pPoint->m_cStartTime.GetTime();
			DWORD64 dwTmpEndTime = pPoint->m_cEndTime.GetTime();
			if(pPoint->m_cEndTime.GetYear() > pPoint->m_cStartTime.GetTime()
				|| pPoint->m_cEndTime.GetMonth() > pPoint->m_cStartTime.GetMonth()
				|| pPoint->m_cEndTime.GetDay() > pPoint->m_cStartTime.GetDay())
			{
				while(dwTmpStartTime <= dwTmpEndTime+86400)
				{
					CTime cTmpCurrentTime(dwTmpStartTime);
					CString strTmpTable = cTmpCurrentTime.Format("Hve_Data_%Y%m%d");
					if(IsTableExists(pConnect, strTmpTable) == TRUE)
					{
						if(strTableList == "")
							strTableList.Format("(SELECT Pass_Time, Plate_No, big_image_1_path, big_image_2_path,\
												Obj_Id,Speeding,Road_no,Location_Id	 FROM %s)", strTmpTable);
						else
							strTableList.AppendFormat("union (SELECT Pass_Time, Plate_No, big_image_1_path, big_image_2_path,\
													  Obj_Id,Speeding,Road_no,Location_Id	 FROM %s)", strTmpTable.GetBuffer());
					}
					dwTmpStartTime += 86400;
				}
			}
			else
			{
				CString strTmp = pPoint->m_cStartTime.Format("Hve_Data_%Y%m%d");
				if(IsTableExists(pConnect, strTmp) == TRUE)
					strTableList.Format("(SELECT * FROM %s)", strTmp.GetBuffer());
			}
			if(strTableList == "")
			{
				CString strTmpSql;
				HRESULT hr = E_FAIL;
				strTmpSql = "SELECT name FROM sysobjects WHERE (name LIKE 'Hve_Data_%')";
				_RecordsetPtr pPtr;
				hr = pPtr.CreateInstance("ADODB.Recordset");
				if(hr != S_OK) return FALSE;
				hr = pPtr->Open(_variant_t(strTmpSql), pConnect.GetInterfacePtr(), 
					adOpenDynamic, adLockOptimistic, adCmdText);
				if(hr != S_OK) return FALSE;
				if(pPtr->GetadoEOF())
				{
					pPtr->Close();
					pPtr.Release();
					::CoUninitialize();
					pPoint->m_Status = 20086;
					return 0xFFFFFFFF;
				}
				CString strTmpTable = (char*)_bstr_t(pPtr->GetCollect("name"));
				strTmpTable.TrimRight();
				strTableList.Format("(SELECT * FROM %s)", strTmpTable);
				pPtr->Close();
				pPtr.Release();
			}
		}
		else
		{
			CString strTmpSql;
			HRESULT hr = E_FAIL;
			strTmpSql = "SELECT name FROM sysobjects WHERE (name LIKE 'Hve_Data_%')";
			_RecordsetPtr pPtr;
			hr = pPtr.CreateInstance("ADODB.Recordset");
			if(hr != S_OK) return FALSE;
			hr = pPtr->Open(_variant_t(strTmpSql), pConnect.GetInterfacePtr(), 
				adOpenDynamic, adLockOptimistic, adCmdText);
			if(hr != S_OK) return FALSE;
			if(pPtr->GetadoEOF())
			{
				pPtr->Close();
				pPtr.Release();
				::CoUninitialize();
				pPoint->m_Status = 20086;
				return 0xFFFFFFFF;
			}
			while(!pPtr->GetadoEOF())
			{
				CString strTmpTable = (char*)_bstr_t(pPtr->GetCollect("name"));
				strTmpTable.TrimRight();
				if(strTableList == "")
					strTableList.Format("(SELECT Pass_Time, Plate_No, big_image_1_path, big_image_2_path,\
										Obj_Id,Speeding,Road_no,Location_Id	 FROM %s)", strTmpTable);
				else
					strTableList.AppendFormat("union (SELECT Pass_Time, Plate_No, big_image_1_path, big_image_2_path,\
											  Obj_Id,Speeding,Road_no,Location_Id	 FROM %s)", strTmpTable.GetBuffer());
				pPtr->MoveNext();
			}
			pPtr->Close();
			pPtr.Release();
		}
		strSql.Format("CREATE VIEW %s AS %s", strViewName.GetBuffer(), strTableList.GetBuffer());
		pConnect->Execute((_bstr_t)strSql.GetBuffer(), &RecordsetAffected, adCmdText);

		hr = pRecordset.CreateInstance("ADODB.Recordset");
		_variant_t var;
		hr = pRecordset->Open(_variant_t(pPoint->m_pszSQL), pConnect.GetInterfacePtr(),
			adOpenDynamic, adLockOptimistic, adCmdText);
		if(SUCCEEDED(hr))
		{
			pPoint->ClearLastResult();
			while(!pRecordset->adoEOF)
			{
				ResultInfoType* pNewResult = new ResultInfoType();
				memcpy(pNewResult->szServerIP, pPoint->m_Server.szServerIP, 30);
				memcpy(pNewResult->szDataBaseName, pPoint->m_Server.szDataBaseName, 32);
				
				var = pRecordset->GetCollect((long)0);
				pNewResult->iLocaIndex = (int)var.lVal;

				CString strTmp;
				var = pRecordset->GetCollect((long)1);

				// modify by zhut,20121120
// 				strTmp = (LPCSTR)_bstr_t(var);
// 				strTmp.Trim();
				COleDateTime time = (COleDateTime)(var);
				strTmp = time.Format("%Y-%m-%d %H:%M:%S");

				memcpy(pNewResult->szPassTime, strTmp.GetBuffer(), strTmp.GetLength());
				pNewResult->szPassTime[strTmp.GetLength()] = '\0';

				var = pRecordset->GetCollect((long)2);
				strTmp = (LPCSTR)_bstr_t(var);
				strTmp.Trim();
				memcpy(pNewResult->szPlate, strTmp.GetBuffer(), strTmp.GetLength());
				pNewResult->szPlate[strTmp.GetLength()] = '\0';

				var = pRecordset->GetCollect((long)3);
				strTmp = (LPCSTR)_bstr_t(var);
				strTmp.Trim();
				memcpy(pNewResult->szRPicPath, strTmp.GetBuffer(), strTmp.GetLength());
				pNewResult->szRPicPath[strTmp.GetLength()] = '\0';

				var = pRecordset->GetCollect((long)4);
				strTmp = (LPCSTR)_bstr_t(var);
				strTmp.Trim();
				memcpy(pNewResult->szCPicPath, strTmp.GetBuffer(), strTmp.GetLength());
				pNewResult->szCPicPath[strTmp.GetLength()] = '\0';

				memcpy(pNewResult->szServerIP, pPoint->m_Server.szServerIP, strlen(pPoint->m_Server.szServerIP));

				EnterCriticalSection(&pPoint->m_csResultList);
				pPoint->m_ResultList.AddTail(pNewResult);
				LeaveCriticalSection(&pPoint->m_csResultList);
				pRecordset->MoveNext();
			}
		}
		strSql.Format("IF EXISTS (SELECT TABLE_NAME FROM INFORMATION_SCHEMA.VIEWS WHERE TABLE_NAME = N'%s') DROP VIEW %s",
			strViewName.GetBuffer(), strViewName.GetBuffer());
		pConnect->Execute((_bstr_t)strSql.GetBuffer(), &RecordsetAffected, adCmdText);
	}
	catch (_com_error e)
	{
		if(pRecordset)
		{
			pRecordset->Close();
			pRecordset = NULL;
		}
		if(pConnect)
		{
			pConnect->Cancel();
			pConnect->Close();
			pConnect = NULL;
		}
		::CoUninitialize();
		pPoint->m_Status = 20086;
		return 0xFFFFFFFF;
	}

	if(pRecordset)
	{
		pRecordset->Close();
		pRecordset = NULL;
	}
	if(pConnect)
	{
		pConnect->Cancel();
		pConnect->Close();
		pConnect = NULL;
	}
	::CoUninitialize();

	pPoint->m_Status = 10086;
	return 0;
}

void CSearchCarPassLocaType::Start()
{
	m_Status = 30086;
	m_hProcess = CreateThread(NULL, 0, ProcessThread, this, 0, NULL);
}







CVehicleTrackingSystemDlg::CVehicleTrackingSystemDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CVehicleTrackingSystemDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_lpDDS = NULL;
	m_lpBack = NULL;
	m_pConnection == NULL;
	m_pRecordSet = NULL;
	m_iSelectIndex = -1;
	m_hSearchPassCarThread = NULL;
	m_fIsBreak = FALSE;
	m_hSearchLocaThread = NULL;
	m_fIsCancelGetLocaInfo = FALSE;
	m_iTempServerIndex = -1;
	m_iResultType = -1;
	m_iSelectResultIndex = -1;
	m_iStartResultIndex = -1;
	m_iEndResultIndex = -1;
	m_strRPicPath = "";
	m_strCPicPath = "";
	m_hLoadResultPic = NULL;
	m_fIsLoadPicBreak = FALSE;
	InitializeCriticalSection(&m_csServerList);
	InitializeCriticalSection(&m_csLocaList);
	InitializeCriticalSection(&m_csLocaRoadLine);
	InitializeCriticalSection(&m_csSearchClassList);
	InitializeCriticalSection(&m_csFinalResultList);
	InitializeCriticalSection(&g_csTestNet);
	g_pShowLoading = NULL;
	m_fIsInit = TRUE;
	m_fIsUpDataLoca = FALSE; 
}

void CVehicleTrackingSystemDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListSever);
	DDX_Control(pDX, IDC_LIST2, m_ListLoca);
	DDX_Control(pDX, IDC_DATETIMEPICKER2, m_DateStart);
	DDX_Control(pDX, IDC_DATETIMEPICKER3, m_DateEnd);
	DDX_Control(pDX, IDC_COMBO1, m_ComBoxCarType);
	DDX_Control(pDX, IDC_DATETIMEPICKER4, m_SearchCarStartTime);
	DDX_Control(pDX, IDC_DATETIMEPICKER5, m_SearchCarEndTime);
	DDX_Control(pDX, IDC_LIST3, m_SelectServer);
	DDX_Control(pDX, IDC_COMBO2, m_ComBoxServer);
	DDX_Control(pDX, IDC_LIST4, m_ListResult);
	DDX_Control(pDX, IDC_COMBODIR1, m_comboDir1);
	DDX_Control(pDX, IDC_COMBODIR2, m_comboDir2);
}

void CVehicleTrackingSystemDlg::OnOK()
{

}

BOOL CVehicleTrackingSystemDlg::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN)
	{
		switch(pMsg->wParam)
		{
		case VK_ESCAPE:
			return TRUE;
		case VK_RETURN:
			{
				CWnd* pw = CWnd::GetFocus();
				if(pw == GetDlgItem(IDC_EDIT3))
				{
					SendMessage(WM_START_SYNC_SEARCH, 0, 0);
				}
			}
			break;
		}
	}
	else if(pMsg->message == WM_RBUTTONDBLCLK)
	{
		if(pMsg->hwnd == GetDlgItem(IDC_LIST3)->GetSafeHwnd())
		{
			m_SelectServer.SendMessage(WM_LBUTTONDOWN, 0, MAKEWPARAM(pMsg->pt.y, pMsg->pt.x));
			int iTopIndex = m_SelectServer.GetTopIndex();
			POINT cPoint;
			cPoint.x = pMsg->pt.x;
			cPoint.y = pMsg->pt.y;
			m_SelectServer.ScreenToClient(&cPoint);
			int iIndex = iTopIndex + (int)(cPoint.y / m_SelectServer.GetItemHeight(0));
			if(iIndex >= 0 && iIndex < m_SelectServer.GetCount())
			{
				m_SelectServer.DeleteString(iIndex);
			}
		}
	}
	else if(pMsg->message == WM_MOUSEMOVE)
	{
		if(pMsg->hwnd == GetDlgItem(IDC_LIST2)->GetSafeHwnd())
		{
			if(m_ListLoca.GetItemCount() <= 0)
			{
				return CDialog::PreTranslateMessage(pMsg);
			}
			int iTopIndex = m_ListLoca.GetTopIndex();
			POINT cPoint;
			cPoint.x = pMsg->pt.x;
			cPoint.y = pMsg->pt.y;
			m_ListLoca.ScreenToClient(&cPoint);
			RECT cRect;
			m_ListLoca.GetItemRect(0, &cRect, LVIR_LABEL);
			int iOffSet = (int)(cPoint.y / (cRect.bottom-cRect.top));
			int iIndex = iTopIndex + iOffSet - 1;
			if(iIndex >= 0 && iIndex < m_ListLoca.GetItemCount())
			{
				char szInfo[256] = {0};
				EnterCriticalSection(&m_csLocaList);
				POSITION posLoca = m_LocaList.FindIndex(iIndex);
				if(posLoca)
				{
					LocaType* pLoca = m_LocaList.GetAt(posLoca);
					sprintf(szInfo, "<%s>  %s", pLoca->szServerIP, pLoca->szServerName);
				}
				LeaveCriticalSection(&m_csLocaList);
				if(strlen(szInfo) > 0)
				{
					m_Message.UpdateTipText(szInfo, GetDlgItem(IDC_LIST2));
					m_Message.RelayEvent(pMsg);
				}
				else
				{
					m_Message.UpdateTipText("", GetDlgItem(IDC_LIST2));
					m_Message.RelayEvent(pMsg);
				}
			}
			else
			{
				m_Message.UpdateTipText("", GetDlgItem(IDC_LIST2));
				m_Message.RelayEvent(pMsg);
			}
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

BEGIN_MESSAGE_MAP(CVehicleTrackingSystemDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_COMMAND(ID_32771, OnLoadFile)
	ON_COMMAND(ID_32775, OnLoadMapFromInterNet)
	ON_COMMAND(ID_32774, OnLoadMap)
	ON_WM_RBUTTONDBLCLK()
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, OnNMDblclkList1)
	ON_COMMAND(ID_32772, OnSave)
	ON_COMMAND(ID_32776, OnClearSencer)
	ON_NOTIFY(NM_RDBLCLK, IDC_LIST2, OnNMRdblclkList2)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST2, OnNMDblclkList2)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON3, OnBnClickedButton3)
	ON_COMMAND(ID_32777, OnEditRoadLine)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST4, OnNMDblclkList4)
	ON_BN_CLICKED(IDC_CHECK7, OnBnClickedCheck7)
	ON_BN_CLICKED(IDC_BUTTON4, OnBnClickedButton4)
	ON_BN_CLICKED(IDC_CHECK6, OnBnClickedCheck6)
	ON_WM_TIMER()
	ON_COMMAND(ID_32778, OnViewResult)
	ON_NOTIFY(NM_RCLICK, IDC_LIST4, OnNMRclickList4)
	ON_COMMAND(ID_SETSTART, OnSetStart)
	ON_COMMAND(ID_SETEND, OnSetEnd)
	ON_COMMAND(ID_UNSETSTART, OnUnSetStart)
	ON_COMMAND(ID_UNSETEND, OnUnSetEnd)
	ON_COMMAND(ID_32779, OnSaveResult)
	ON_COMMAND(ID_32780, OnViewSavedResult)
	ON_NOTIFY(NM_RCLICK, IDC_LIST1, OnNMRclickList1)
	ON_COMMAND(ID_EDIT_SERVER, OnEditServerComm)
	ON_COMMAND(ID_REMOVE_SERVER, OnDeleteServerComm)
	ON_COMMAND(ID_32781, OnOpenDemo)
	ON_BN_CLICKED(IDC_CHECK8, OnBnClickedCheck8)
	ON_COMMAND(ID_32782, On32782)
	ON_WM_SIZE()
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST4, OnLvnKeydownList4)
	ON_WM_LBUTTONDBLCLK()
	ON_NOTIFY(NM_CLICK, IDC_LIST2, OnNMClickList2)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST2, OnLvnKeydownList2)
	ON_NOTIFY(NM_CLICK, IDC_LIST4, OnNMClickList4)
//	ON_NOTIFY(NM_KILLFOCUS, IDC_LIST2, OnNMKillfocusList2)
END_MESSAGE_MAP()

void CVehicleTrackingSystemDlg::OnCancel()
{

	if(g_pShowLoading)
	{
		g_pShowLoading->Stop();
		delete g_pShowLoading;
		g_pShowLoading = NULL;
	}

	if(g_TestNet)
	{
		delete g_TestNet;
		g_TestNet = NULL;
	}
	DeleteCriticalSection(&g_csTestNet);
	CDialog::OnCancel();
	if(m_hSearchPassCarThread)
	{
		m_fIsBreak = TRUE;
		int iWaitTimes = 0;
		int MAXWAITTIME = 8;
		while(WaitForSingleObject(m_hSearchPassCarThread, 500) == WAIT_TIMEOUT && iWaitTimes < MAXWAITTIME)
		{
			iWaitTimes++;
		}

		if(iWaitTimes >= MAXWAITTIME)
		{
			TerminateThread(m_hSearchPassCarThread, 0);
		}
		else
		{
			CloseHandle(m_hSearchPassCarThread);
		}
	}
	m_hSearchPassCarThread = NULL;

	if(m_hSearchLocaThread)
	{
		m_fIsCancelGetLocaInfo = TRUE;
		int iWaitTimes = 0;
		int MAXWAITTIME = 8;
		while(WaitForSingleObject(m_hSearchLocaThread, 500) == WAIT_TIMEOUT && iWaitTimes < MAXWAITTIME)
		{
			iWaitTimes++;
		}

		if(iWaitTimes >= MAXWAITTIME)
		{
			TerminateThread(m_hSearchLocaThread, 0);
		}
		else
		{
			CloseHandle(m_hSearchLocaThread);
		}
	}
	m_hSearchLocaThread = NULL;

	if(m_lpRPic)
	{
		m_lpRPic->Release();
		m_lpRPic = NULL;
	}
	if(m_lpCPic)
	{
		m_lpCPic->Release();
		m_lpCPic = NULL;
	}
	if(m_lpOffScreen)
	{
		m_lpOffScreen->Release();
		m_lpOffScreen = NULL;
	}
	if(m_lpBack)
	{
		m_lpBack->Release();
		m_lpBack = NULL;
	}
	if(m_lpDDS)
	{
		m_lpDDS->Release();
		m_lpDDS = NULL;
	}
	CoUninitialize();
	EnterCriticalSection(&m_csServerList);
	while(m_ServerList.GetCount() > 0)
	{
		NewSeverType* pTmpServer = m_ServerList.RemoveHead();
		if(pTmpServer)
		{
			delete pTmpServer;
			pTmpServer = NULL;
		}
	}
	LeaveCriticalSection(&m_csServerList);
	DeleteCriticalSection(&m_csServerList);

	EnterCriticalSection(&m_csLocaRoadLine);
	while(m_LocaRoadLineType.GetCount() > 0)
	{
		LocaRoadLineType* pTmpRoadLine = m_LocaRoadLineType.RemoveHead();
		delete pTmpRoadLine;
		pTmpRoadLine = NULL;
	}
	LeaveCriticalSection(&m_csLocaRoadLine);
	DeleteCriticalSection(&m_csLocaRoadLine);

	EnterCriticalSection(&m_csLocaList);
	while(m_LocaList.GetCount() > 0)
	{
		LocaType* pTmpLoca = m_LocaList.RemoveHead();
		if(pTmpLoca)
		{
			delete pTmpLoca;
			pTmpLoca = NULL;
		}
	}
	LeaveCriticalSection(&m_csLocaList);
	DeleteCriticalSection(&m_csLocaList);

	EnterCriticalSection(&m_csSearchClassList);
	while(m_SearchClassList.GetCount() > 0)
	{
		CSearchCarPassLocaType* pTmpSearchClass = m_SearchClassList.RemoveHead();
		delete pTmpSearchClass;
		pTmpSearchClass = NULL;
	}
	LeaveCriticalSection(&m_csSearchClassList);
	DeleteCriticalSection(&m_csSearchClassList);

	EnterCriticalSection(&m_csFinalResultList);
	while(m_FinalResultList.GetCount() > 0)
	{
		FinalResultType* pTmpFinalResult = m_FinalResultList.RemoveHead();
		delete pTmpFinalResult;
		pTmpFinalResult = NULL;
	}
	LeaveCriticalSection(&m_csFinalResultList);
	DeleteCriticalSection(&m_csFinalResultList);
	char szCurrentPath[MAX_PATH] = {0};
	GetModuleFileName(NULL, szCurrentPath, MAX_PATH);
	PathRemoveFileSpec(szCurrentPath);
	CString strSavePath = szCurrentPath;
	strSavePath += "\\TmpResultTmp\\";
	ClearTmpResultPic(strSavePath.GetBuffer());
	strSavePath = szCurrentPath;
	strSavePath += "\\TmpFinalResult\\";
	ClearTmpResultPic(strSavePath.GetBuffer());
}

BOOL CVehicleTrackingSystemDlg::OnInitDX()
{
	if(FAILED(DirectDrawCreateEx(NULL, (void**)&m_lpDDS, IID_IDirectDraw7, NULL)))
	{
		return FALSE;
	}
	if(FAILED(m_lpDDS->SetCooperativeLevel(GetDlgItem(IDC_STATIC)->GetSafeHwnd(), DDSCL_NORMAL)))
	{
		return FALSE;
	}
    DDSURFACEDESC2 ddsd;
	ZeroMemory(&ddsd, sizeof(DDSURFACEDESC2));
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
	ddsd.dwWidth = 1024;
	ddsd.dwHeight = 786;
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
	if(FAILED(m_lpDDS->CreateSurface(&ddsd, &m_lpBack, NULL)))
	{
		return FALSE;
	}
	if(FAILED(m_lpDDS->CreateSurface(&ddsd, &m_lpOffScreen, NULL)))
	{
		return FALSE;
	}
	DDSURFACEDESC2 ddsd1;
	ZeroMemory(&ddsd1, sizeof(ddsd1));
	ddsd1.dwSize = sizeof(ddsd1);
	m_lpBack->Lock(NULL, &ddsd1, 0, NULL);
	PBYTE pBuf = (PBYTE)ddsd1.lpSurface;
	memset(pBuf, 250, ddsd1.dwHeight * ddsd1.lPitch);
	m_lpBack->Unlock(NULL);
	ddsd.dwWidth = 315;
	ddsd.dwHeight = 220;
	if(FAILED(m_lpDDS->CreateSurface(&ddsd, &m_lpRPic, NULL)))
	{
		return FALSE;
	}
	if(FAILED(m_lpDDS->CreateSurface(&ddsd, &m_lpCPic, NULL)))
	{
		return FALSE;
	}

    return TRUE;
}

void CVehicleTrackingSystemDlg::SetDefualtPicBuffer()
{
	DDSURFACEDESC2 ddsd;
	ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	m_lpCPic->Lock(NULL, &ddsd, 0, NULL);
	BYTE* pBuf = (BYTE*)ddsd.lpSurface;
	memset(pBuf, 250, ddsd.dwHeight*ddsd.lPitch);
	m_lpCPic->Unlock(NULL);
	
	ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	m_lpRPic->Lock(NULL, &ddsd, 0, NULL);
	pBuf = (BYTE*)ddsd.lpSurface;
	memset(pBuf, 250, ddsd.dwHeight*ddsd.lPitch);
	m_lpRPic->Unlock(NULL);

	HDC hDC;
	m_lpRPic->GetDC(&hDC);
	SelectObject(hDC, g_hFontComm);
	SetBkMode(hDC, 3);
	SetTextColor(hDC, RGB(128, 128, 128));
	TextOut(hDC, 80, 80, "识别图", 6);
	m_lpRPic->ReleaseDC(hDC);

	m_lpCPic->GetDC(&hDC);
	SelectObject(hDC, g_hFontComm);
	SetBkMode(hDC, 3);
	SetTextColor(hDC, RGB(128, 128, 128));
	TextOut(hDC, 80, 80, "抓拍图", 6);
	m_lpCPic->ReleaseDC(hDC);
}

void CVehicleTrackingSystemDlg::SetDefaultSize(void)
{
	RECT cRect;
	cRect.left = (GetSystemMetrics(SM_CXSCREEN) - 1024) >> 1;
	cRect.right = cRect.left + 1024;
	cRect.top = (GetSystemMetrics(SM_CYSCREEN) - 768) >> 1;
	cRect.bottom = cRect.top + 768;
	MoveWindow(&cRect, TRUE);

	cRect.left = 2;
	cRect.right = 298;
	cRect.top = 2;
	cRect.bottom = 280;
	m_ListSever.MoveWindow(&cRect, TRUE);

	cRect.left = 165;
	cRect.right = 765;
	cRect.top = 285;
	cRect.bottom = 705;
	GetDlgItem(IDC_STATIC)->MoveWindow(&cRect, TRUE);

	cRect.left = 2;
	cRect.right = 162;
	cRect.top = 285;
	cRect.bottom = 705;
	m_ListLoca.MoveWindow(&cRect, TRUE);

	cRect.left = 300;
	cRect.right = 765;
	cRect.top = 5;
	cRect.bottom = 105;
	GetDlgItem(IDC_STATIC222)->MoveWindow(&cRect, TRUE);

	cRect.left = 305;
	cRect.right = 365;
	cRect.top = 25;
	cRect.bottom = 45;
	GetDlgItem(IDC_STATIC4)->MoveWindow(&cRect, TRUE);
	cRect.left = 370;
	cRect.right = 525;
	cRect.top = 25;
	cRect.bottom = 45;
	m_DateStart.MoveWindow(&cRect, TRUE);
	cRect.left = 530;
	cRect.right = 590;
	cRect.top = 25;
	cRect.bottom = 45;
	GetDlgItem(IDC_STATIC5)->MoveWindow(&cRect, TRUE);
	cRect.left = 592;
	cRect.right = 762;
	cRect.top = 25;
	cRect.bottom = 45;
	m_DateEnd.MoveWindow(&cRect, TRUE);

	cRect.left = 305;
	cRect.right = 365;
	cRect.top = 50;
	cRect.bottom = 70;
	GetDlgItem(IDC_CHECK1)->MoveWindow(&cRect, TRUE);

	cRect.left = 370;
	cRect.right = 430;
	cRect.top = 50;
	cRect.bottom = 70;
	GetDlgItem(IDC_CHECK2)->MoveWindow(&cRect, TRUE);

	cRect.left = 435;
	cRect.right = 505;
	cRect.top = 50;
	cRect.bottom = 70;
	GetDlgItem(IDC_CHECK3)->MoveWindow(&cRect, TRUE);

	cRect.left = 535;
	cRect.right = 595;
	cRect.top = 50;
	cRect.bottom = 70;
	GetDlgItem(IDC_CHECK4)->MoveWindow(&cRect, TRUE);

	cRect.left = 475;
	cRect.right = 535;
	cRect.top = 75;
	cRect.bottom = 95;
	GetDlgItem(IDC_CHECK5)->MoveWindow(&cRect, TRUE);

	cRect.left = 329;
	cRect.right = 367;
	cRect.top = 75;
	cRect.bottom = 95;
	GetDlgItem(IDC_STATICDIR1)->MoveWindow(&cRect, TRUE);

	cRect.left = 370;
	cRect.right = 470;
	cRect.top = 75;
	cRect.bottom = 95+200;
	GetDlgItem(IDC_COMBODIR1)->MoveWindow(&cRect, TRUE);


	cRect.left = 665;
	cRect.right = 725;
	cRect.top = 50;
	cRect.bottom = 70;
	GetDlgItem(IDC_CHECK8)->MoveWindow(&cRect, TRUE);

	cRect.left = 329;
	cRect.right = 367;
	cRect.top = 50;
	cRect.bottom = 70;
	GetDlgItem(IDC_STATIC7)->MoveWindow(&cRect, TRUE);
	cRect.left = 370;
	cRect.right = 470;
	cRect.top = 50;
	cRect.bottom = 70 + 200;
	m_ComBoxCarType.MoveWindow(&cRect, TRUE);

	cRect.left = 518;
	cRect.right = 590;
	cRect.top = 50;
	cRect.bottom = 70;
	GetDlgItem(IDC_STATIC6)->MoveWindow(&cRect, TRUE);
	cRect.left = 592;
	cRect.right = 762;
	cRect.top = 50;
	cRect.bottom = 70;
	GetDlgItem(IDC_EDIT1)->MoveWindow(&cRect, TRUE);
	


	cRect.left = 660;
	cRect.right = 760;
	cRect.top = 75;
	cRect.bottom = 95;
	GetDlgItem(IDC_BUTTON1)->MoveWindow(&cRect, TRUE);

	cRect.left = 300;
	cRect.right = 765;
	cRect.top = 110;
	cRect.bottom = 280;
	GetDlgItem(IDC_STATIC333)->MoveWindow(&cRect, TRUE);

	cRect.left = 305;
	cRect.right = 365;
	cRect.top = 130;
	cRect.bottom = 150;
	GetDlgItem(IDC_STATIC3331)->MoveWindow(&cRect, TRUE);
	cRect.left = 370;
	cRect.right = 525;
	cRect.top = 130;
	cRect.bottom = 150;
	GetDlgItem(IDC_EDIT3)->MoveWindow(&cRect, TRUE);

	cRect.left = 305;
	cRect.right = 415;
	cRect.top = 155;
	cRect.bottom = 175;
	GetDlgItem(IDC_CHECK6)->MoveWindow(&cRect, TRUE);


	cRect.left = 305;
	cRect.right = 345;
	cRect.top = 155;
	cRect.bottom = 175;
	GetDlgItem(IDC_STATICDIR2)->MoveWindow(&cRect, TRUE);

	cRect.left = 350;
	cRect.right = 430;
	cRect.top = 155;
	cRect.bottom = 175+200;
	GetDlgItem(IDC_COMBODIR2)->MoveWindow(&cRect, TRUE);

	cRect.left = 435;
	cRect.right = 525;
	cRect.top = 155;
	cRect.bottom = 175;
	GetDlgItem(IDC_CHECK7)->MoveWindow(&cRect, TRUE);

	cRect.left = 305;
	cRect.right = 365;
	cRect.top = 180;
	cRect.bottom = 200;
	GetDlgItem(IDC_STATIC8)->MoveWindow(&cRect, TRUE);
	cRect.left = 370;
	cRect.right = 525;
	cRect.top = 180;
	cRect.bottom = 200;
	m_SearchCarStartTime.MoveWindow(&cRect, TRUE);
	cRect.left = 305;
	cRect.right = 365;
	cRect.top = 205;
	cRect.bottom = 225;
	GetDlgItem(IDC_STATIC9)->MoveWindow(&cRect, TRUE);
	cRect.left = 370;
	cRect.right = 525;
	cRect.top = 205;
	cRect.bottom = 225;
	m_SearchCarEndTime.MoveWindow(&cRect, TRUE);

	cRect.left = 305;
	cRect.right = 365;
	cRect.top = 230;
	cRect.bottom = 250;
	GetDlgItem(IDC_STATIC10)->MoveWindow(&cRect, TRUE);
	cRect.left = 370;
	cRect.right = 525;
	cRect.top = 230;
	cRect.bottom = 250 + 200;
	m_ComBoxServer.MoveWindow(&cRect, TRUE);

	cRect.left = 305;
	cRect.right = 405;
	cRect.top = 255;
	cRect.bottom = 275;
	GetDlgItem(IDC_BUTTON3)->MoveWindow(&cRect, TRUE);
	cRect.left = 425;
	cRect.right = 525;
	cRect.top = 255;
	cRect.bottom = 275;
	GetDlgItem(IDC_BUTTON4)->MoveWindow(&cRect, TRUE);
	cRect.left = 530;
	cRect.right = 760;
	cRect.top = 130;
	cRect.bottom = 150;
	GetDlgItem(IDC_STATIC666)->MoveWindow(&cRect, FALSE);
	cRect.left = 530;
	cRect.right = 760;
	cRect.top = 150;
	cRect.bottom = 275;
	m_SelectServer.MoveWindow(&cRect, TRUE);

	cRect.left = 768;
	cRect.right = 1018;
	cRect.top = 5;
	cRect.bottom = 315;
	m_ListResult.MoveWindow(&cRect, TRUE);
	cRect.left = 768;
	cRect.right = 1018;
	cRect.top = 320;
	cRect.bottom = 510;
	GetDlgItem(IDC_STATIC_R_PIC)->MoveWindow(&cRect, TRUE);
	cRect.left = 768;
	cRect.right = 1018;
	cRect.top = 515;
	cRect.bottom = 705;
	GetDlgItem(IDC_STATIC_C_PIC)->MoveWindow(&cRect, TRUE);
}

BOOL CVehicleTrackingSystemDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	::CoInitialize(NULL);
	
	IP_ADAPTER_INFO AdapterInfo[16];
	DWORD dwBufLen = sizeof(AdapterInfo);
	DWORD dwStatus = GetAdaptersInfo(AdapterInfo, &dwBufLen);
	PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;
	if(pAdapterInfo)
	{
		g_strMacAddr.Format("%02X%02X%02X%02X%02X%02X", pAdapterInfo->Address[0], pAdapterInfo->Address[1],
			pAdapterInfo->Address[2], pAdapterInfo->Address[3], pAdapterInfo->Address[4], pAdapterInfo->Address[5]);
	}
	else
	{
		MessageBox("获取设备MAC地址失败,程序无法运行!", "ERROR", MB_OK|MB_ICONERROR);
		OnCancel();
		return FALSE;
	}

	g_pShowLoading = new CShowLoading(this);
	g_pShowLoading->CreateOwnerEx();
	g_TestNet = new CTestNetWork();
	g_TestNet->InitSock();

	SetIcon(m_hIcon, TRUE);	
	SetIcon(m_hIcon, FALSE);
	CMenu MainMenu;
	MainMenu.LoadMenu(IDR_MENU1);
	SetMenu(&MainMenu);

	g_hFontComm = CreateFont(50, 26, 0, 0, FW_HEAVY, false, false, false,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH, "宋体");
	g_hFontMin = CreateFont(14, 6, 0, 0, FW_LIGHT, false, false, false,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH, "Microsoft Sans Serif");

	if(OnInitDX() == FALSE)
	{
		MessageBox("程序程序化失败，无法启动!", "ERROR", MB_OK | MB_ICONERROR);
		OnCancel();
	}
	SetDefualtPicBuffer();
	m_ListSever.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_ListSever.InsertColumn(0, "数据站名", LVCFMT_LEFT, 80);
	m_ListSever.InsertColumn(1, "服务器IP", LVCFMT_LEFT, 100);
	m_ListSever.InsertColumn(2, "数据库名", LVCFMT_LEFT, 60);
	m_ListSever.InsertColumn(3, "登录名", LVCFMT_LEFT, 52);

	m_ListLoca.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_ListLoca.InsertColumn(0, "断面编号", LVCFMT_LEFT, 60);
	m_ListLoca.InsertColumn(1, "断面名称", LVCFMT_LEFT, 96);

	m_DateStart.SetFormat("yyyy年MM月dd日 HH:mm");
	m_DateEnd.SetFormat("yyyy年MM月dd日 HH:mm");
	m_SearchCarStartTime.SetFormat("yyyy年MM月dd日 HH:mm");
	m_SearchCarEndTime.SetFormat("yyyy年MM月dd日 HH:mm");
	SYSTEMTIME cStsTemTime;
	CTime cTime = CTime::GetCurrentTime();
	cTime.GetAsSystemTime(cStsTemTime);
	m_DateEnd.SetTime(&cStsTemTime);
	m_SearchCarEndTime.SetTime(&cStsTemTime);

	DWORD64 dw64Time = cTime.GetTime();
	dw64Time -= 3600;
	CTime cTimeEnd(dw64Time);
	cTimeEnd.GetAsSystemTime(cStsTemTime);
	m_DateStart.SetTime(&cStsTemTime);
	m_SearchCarStartTime.SetTime(&cStsTemTime);

	m_ComBoxCarType.AddString("全部车型");
	m_ComBoxCarType.SetCurSel(0);

	GetDlgItem(IDC_BUTTON4)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON3)->EnableWindow(FALSE);

// 	m_SearchCarStartTime.EnableWindow(FALSE);
// 	m_SearchCarEndTime.EnableWindow(FALSE);
	m_ComBoxServer.EnableWindow(FALSE);
	m_SelectServer.EnableWindow(FALSE);

	m_ListResult.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_ListResult.InsertColumn(0, "编号", LVCFMT_LEFT, 40);
	m_ListResult.InsertColumn(1, "车牌号", LVCFMT_LEFT, 60);
	m_ListResult.InsertColumn(2, "经过时间", LVCFMT_LEFT, 73);
	m_ListResult.InsertColumn(3, "断面名称", LVCFMT_LEFT, 73);
	m_ListResult.InsertColumn(4, "", LVCFMT_LEFT, 0);
	m_ListResult.InsertColumn(5, "", LVCFMT_LEFT, 0);
	m_ListResult.InsertColumn(6, "", LVCFMT_LEFT, 0);

	SetDefaultSize();
	m_Message.Create(this);
	m_Message.AddTool(GetDlgItem(IDC_LIST2), "");
	m_Message.SetDelayTime(0);
	m_Message.SetTipBkColor(RGB(225, 225, 225));
	m_Message.SetTipTextColor(RGB(32, 32, 32));
	m_Message.Activate(TRUE);
	m_fIsInit = FALSE;

// get server username, password, by zhut 20121112
	CString strIniFilePath;
	char szAppPath[MAX_PATH] = {0};
	GetModuleFileName(NULL, szAppPath, MAX_PATH);
	PathRemoveFileSpec(szAppPath);
	strIniFilePath.Format("%s\\Config.ini", szAppPath);

	GetPrivateProfileString("ServerInfo", "ServerUserName", "administrator",m_strServerName.GetBuffer(MAX_PATH), MAX_PATH, strIniFilePath);
	GetPrivateProfileString("ServerInfo", "ServerPassword", "123456", m_strServerPassword.GetBuffer(MAX_PATH), MAX_PATH, strIniFilePath);

	// CarTrack query condition
	CTime cEndTimeCarTrack = CTime::GetCurrentTime();
	DWORD64 dw64TimeCarTrack = cEndTimeCarTrack.GetTime();
	dw64TimeCarTrack -= 3600*24;
	CTime cStartTimeCarTrack(dw64TimeCarTrack);
	m_SearchCarStartTime.SetTime(&cStartTimeCarTrack);
	m_SearchCarEndTime.SetTime(&cEndTimeCarTrack);
	m_SearchCarStartTime.EnableWindow(TRUE);
	m_SearchCarEndTime.EnableWindow(TRUE);

	// direction init
	m_comboDir1.Clear();
	m_comboDir1.ResetContent();
	m_comboDir1.InsertString(-1, _T("不限"));
	m_comboDir1.InsertString(-1, _T("上行"));
	m_comboDir1.InsertString(-1, _T("下行"));
	m_comboDir1.SetCurSel(0);


	m_comboDir2.Clear();
	m_comboDir2.ResetContent();
	m_comboDir2.InsertString(-1, _T("不限"));
	m_comboDir2.InsertString(-1, _T("上行"));
	m_comboDir2.InsertString(-1, _T("下行"));
	m_comboDir2.SetCurSel(0);

	return TRUE; 
}

void CVehicleTrackingSystemDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialog::OnSysCommand(nID, lParam);
}

void CVehicleTrackingSystemDlg::DrawLine(HDC hDC, int x1, int y1, int x2, int y2, int iWidth, COLORREF color)
{
	HPEN hPen = CreatePen(PS_SOLID, iWidth, color);
	SelectObject(hDC, hPen);
	MoveToEx(hDC, x1, y1, NULL);
	LineTo(hDC, x2, y2);
	DeleteObject(hPen);
}

void CVehicleTrackingSystemDlg::DrawRectangle(HDC hDC, int x1, int y1, int x2, int y2, COLORREF color1, COLORREF color2)
{
	HPEN hPen = CreatePen(PS_SOLID, 1, color1);
	HBRUSH hBrush = CreateSolidBrush(color2);
	SelectObject(hDC, hPen);
	SelectObject(hDC, hBrush);
	Rectangle(hDC, x1, y1, x2, y2);
	DeleteObject(hPen);
	DeleteObject(hBrush);
}

void CVehicleTrackingSystemDlg::DrawEllsip(HDC hDC, int x1, int y1, int x2, int y2, COLORREF color)
{
	HPEN hPen = CreatePen(PS_SOLID, 1, color);
	HBRUSH hBrush = CreateSolidBrush(color);
	SelectObject(hDC, hPen);
	SelectObject(hDC, hBrush);
	Ellipse(hDC, x1, y1, x2, y2);
	DeleteObject(hPen);
	DeleteObject(hBrush);
}

void CVehicleTrackingSystemDlg::DrawTextThis(HDC hDC, int x, int y, LPCSTR pszText, int iTextLen, COLORREF color)
{
	SetBkMode(hDC, 3);
	SetTextColor(hDC, color);
	TextOut(hDC, x, y, pszText, iTextLen);
}

void CVehicleTrackingSystemDlg::ShowPosInfo(HDC hDC, int x, int y, char* pszInfo, int iFrameWidth, COLORREF color1, COLORREF color2)
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

void CVehicleTrackingSystemDlg::ShowResultInfo(HDC hDC)
{
	if(m_iResultType != 2)
	{
		return;
	}
	if(m_iSelectResultIndex < 0 || m_iSelectResultIndex >= m_FinalResultList.GetCount())
	{
		return;
	}
	POSITION posFinalResult = m_FinalResultList.FindIndex(m_iSelectResultIndex);
	if(posFinalResult)
	{
		FinalResultType* pTmpFinalResult = m_FinalResultList.GetAt(posFinalResult);
		if(pTmpFinalResult->Loca.x >= 0 && pTmpFinalResult->Loca.x <= 1024 
			&& pTmpFinalResult->Loca.y >= 0 && pTmpFinalResult->Loca.y <= 786)
		{
			CString strInfo;
			strInfo.Format("%s<%s>", pTmpFinalResult->szLocaName, pTmpFinalResult->szPassTime);
			if(strInfo.GetLength() > 40)
			{
				strInfo.SetAt(40, '\0');
			}
			SelectObject(hDC, g_hFontMin);
			ShowPosInfo(hDC, pTmpFinalResult->Loca.x, pTmpFinalResult->Loca.y, strInfo.GetBuffer(), 3, RGB(0, 0, 255), RGB(255, 255, 255));
		}
	}
}

void CVehicleTrackingSystemDlg::PrepareOutPut()
{
	if(m_lpOffScreen == NULL)
	{
		return;
	}
	HDC hOffBufDC;
	m_lpOffScreen->GetDC(&hOffBufDC);
	
	if(m_lpBack)
	{
		HDC hBackDC;
		m_lpBack->GetDC(&hBackDC);
		BitBlt(hOffBufDC, 0, 0, 1024, 786, hBackDC, 0, 0, SRCCOPY);
		m_lpBack->ReleaseDC(hBackDC);
	}

	int iIndex, iCount;
	iCount = (int)m_LocaList.GetCount();
	POSITION pos = m_LocaList.GetHeadPosition();
	int iTextLocaX, iTextLocaY;
	int iTextLen = 0;
	char szText[256] = {0};
	SelectObject(hOffBufDC, g_hFontMin);
	for(iIndex=0; iIndex<iCount; iIndex++)
	{
		LocaType* pTmpLoca = m_LocaList.GetNext(pos);
		if(pTmpLoca->iPositionX >= 0 && pTmpLoca->iPositionX <= 1024 
			&& pTmpLoca->iPositionY >= 0 && pTmpLoca->iPositionY <= 786)
		{	
			if(iIndex == m_iSelectIndex)
			{
				DrawEllsip(hOffBufDC, pTmpLoca->iPositionX-8, pTmpLoca->iPositionY-8,
					pTmpLoca->iPositionX+8, pTmpLoca->iPositionY+8, RGB(255, 0, 0));
			}
			else
			{
				DrawEllsip(hOffBufDC, pTmpLoca->iPositionX-8, pTmpLoca->iPositionY-8,
					pTmpLoca->iPositionX+8, pTmpLoca->iPositionY+8, RGB(0, 255, 0));
			}
		}
	}

	pos = m_LocaList.GetHeadPosition();
	for(iIndex=0; iIndex<iCount; iIndex++)
	{
		LocaType* pTmpLoca = m_LocaList.GetNext(pos);
		if(pTmpLoca->iPositionX >= 0 && pTmpLoca->iPositionX <= 1024 
			&& pTmpLoca->iPositionY >= 0 && pTmpLoca->iPositionY <= 786)
		{	
			int iLen = (int)strlen(pTmpLoca->szLocaName);
			int iLeft = pTmpLoca->iPositionX - iLen*3;
			int iTop = pTmpLoca->iPositionY + 12;

			if(iLeft < 0 ) iLeft = 0;
			if(iLeft > 824) iLeft = 824;
			if(iTop < 0) iTop = 0;
			if(iTop > 756) iTop = 756;
			iTextLocaX = iLeft;
			iTextLocaY = iTop;
			iTextLen = iLen;
			memcpy(szText, pTmpLoca->szLocaName, strlen(pTmpLoca->szLocaName));
			DrawTextThis(hOffBufDC, iTextLocaX, iTextLocaY, szText, iTextLen, RGB(0, 0, 255));
		}
	}

	ShowResultInfo(hOffBufDC);
	m_lpOffScreen->ReleaseDC(hOffBufDC);
}

void CVehicleTrackingSystemDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this);

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
	if(m_lpOffScreen)
	{
		PrepareOutPut();

		RECT cRect;
		::GetClientRect(GetDlgItem(IDC_STATIC)->GetSafeHwnd(), &cRect);
		HDC hSrcDC, hWinDC;
		m_lpOffScreen->GetDC(&hSrcDC);
		hWinDC = ::GetDC(GetDlgItem(IDC_STATIC)->GetSafeHwnd());
		SetStretchBltMode(hWinDC, COLORONCOLOR);
		StretchBlt(hWinDC, 0, 0, cRect.right-cRect.left, cRect.bottom-cRect.top,
			hSrcDC, 0, 0,1024, 786, SRCCOPY);
		::ReleaseDC(GetDlgItem(IDC_STATIC)->GetSafeHwnd(), hWinDC);
		m_lpOffScreen->ReleaseDC(hSrcDC);
	}

	if(m_lpRPic)
	{
		RECT cRect;
		::GetClientRect(GetDlgItem(IDC_STATIC_R_PIC)->GetSafeHwnd(), &cRect);
		HDC hSrcDC, hWinDC;
		m_lpRPic->GetDC(&hSrcDC);
		hWinDC = ::GetDC(GetDlgItem(IDC_STATIC_R_PIC)->GetSafeHwnd());
		SetStretchBltMode(hWinDC, COLORONCOLOR);
		StretchBlt(hWinDC, 0, 0, cRect.right-cRect.left, cRect.bottom-cRect.top,
			hSrcDC, 0, 0, 315, 220, SRCCOPY);
		::ReleaseDC(GetDlgItem(IDC_STATIC_R_PIC)->GetSafeHwnd(), hWinDC);
		m_lpRPic->ReleaseDC(hSrcDC);
	}

	if(m_lpCPic)
	{
		RECT cRect;
		::GetClientRect(GetDlgItem(IDC_STATIC_C_PIC)->GetSafeHwnd(), &cRect);
		HDC hSrcDC, hWinDC;
		m_lpCPic->GetDC(&hSrcDC);
		hWinDC = ::GetDC(GetDlgItem(IDC_STATIC_C_PIC)->GetSafeHwnd());
		SetStretchBltMode(hWinDC, COLORONCOLOR);
		StretchBlt(hWinDC, 0, 0, cRect.right-cRect.left, cRect.bottom-cRect.top,
			hSrcDC, 0, 0, 315, 220, SRCCOPY);
		::ReleaseDC(GetDlgItem(IDC_STATIC_C_PIC)->GetSafeHwnd(), hWinDC);
		m_lpCPic->ReleaseDC(hSrcDC);
	}
}

HCURSOR CVehicleTrackingSystemDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CVehicleTrackingSystemDlg::OnLoadFile()
{
	CFileDialog cGetOpenFile(TRUE, NULL, NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, "场景文件(*.SD)|*.SD||");
	if(cGetOpenFile.DoModal() == IDOK)
	{
		CString strOpenFileName = cGetOpenFile.GetPathName();
		HANDLE hFile = NULL;
		DWORD dwFileSize, dwByteRead;
		hFile = CreateFile(strOpenFileName.GetBuffer(), GENERIC_READ, FILE_SHARE_READ,
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if(hFile == INVALID_HANDLE_VALUE)
		{
			MessageBox("打开文件出错", "ERROR", MB_OK|MB_ICONERROR);
			return;
		}
		dwFileSize = GetFileSize(hFile, NULL);
		if(dwFileSize == 0xFFFFFFFF)
		{
			MessageBox("错误的文件长度，打开文件失败", "ERROR", MB_OK|MB_ICONERROR);
			CloseHandle(hFile);
			return;
		}
		PBYTE pbTmpData = new BYTE[dwFileSize];
		if(pbTmpData == NULL)
		{
			MessageBox("申请文件缓存失败，打开失败", "ERROR", MB_OK|MB_ICONERROR);
			CloseHandle(hFile);
			return;
		}
		if(ReadFile(hFile, pbTmpData, dwFileSize, &dwByteRead, NULL) != TRUE)
		{
			MessageBox("读文件失败，打开失败", "ERROR", MB_OK|MB_ICONERROR);
			SAFE_DELETE_ARRAY(pbTmpData);
			CloseHandle(hFile);
			return;
		}
		CloseHandle(hFile);
		if(dwByteRead <= 11)
		{
			MessageBox("读文件失败，打开失败", "ERROR", MB_OK|MB_ICONERROR);
			SAFE_DELETE_ARRAY(pbTmpData);
			return;
		}
		ClearSencer();
		PBYTE pDataPointer = pbTmpData;
		char szTmpInfo[64] = {0};
		memcpy(szTmpInfo, pDataPointer, 11);
		if(strcmp(szTmpInfo, "<SEVERINFO>") != 0)
		{
			MessageBox("错误的文件类型，载入失败", "ERROR", MB_OK|MB_ICONERROR);
			SAFE_DELETE_ARRAY(pbTmpData);
			return;
		}
		pDataPointer += 11;
		int iServerCount = 0;
		memcpy(&iServerCount, pDataPointer, sizeof(int));
		pDataPointer += sizeof(int);
		for(int iIndex=0; iIndex<iServerCount; iIndex++)
		{
			NewSeverType* pTmpServer = new NewSeverType();
			memcpy(pTmpServer, pDataPointer, sizeof(NewSeverType));
			for(int iIndex=0; iIndex<16; iIndex++)
			{
				pTmpServer->szPassWord[iIndex] = ~pTmpServer->szPassWord[iIndex];
			}
			pDataPointer += sizeof(NewSeverType);
			EnterCriticalSection(&m_csServerList);
			m_ServerList.AddTail(pTmpServer);
			LeaveCriticalSection(&m_csServerList);
		}
		if(iServerCount > 0)
		{
			UpDataListServer();
		}

		memset(szTmpInfo, 0, 64);
		memcpy(szTmpInfo, pDataPointer, 10);
		if(strcmp(szTmpInfo, "<LOCAINFO>") != 0)
		{
			MessageBox("错误的文件类型，载入失败", "ERROR", MB_OK|MB_ICONERROR);
			SAFE_DELETE_ARRAY(pbTmpData);
			return;
		}
		pDataPointer += 10;
		int iLocaCount = 0;
		memcpy(&iLocaCount, pDataPointer, sizeof(int));
		pDataPointer += sizeof(int);
		for(int iIndex=0; iIndex<iLocaCount; iIndex++)
		{
			LocaType* pTmpLoca = new LocaType();
			memcpy(pTmpLoca, pDataPointer, sizeof(LocaType));
			for(int iIndex=0; iIndex<16; iIndex++)
			{
				pTmpLoca->szPassWork[iIndex] = ~pTmpLoca->szPassWork[iIndex];
			}
			pDataPointer += sizeof(LocaType);
			EnterCriticalSection(&m_csLocaList);
			m_LocaList.AddTail(pTmpLoca);
			LeaveCriticalSection(&m_csLocaList);

			LocaRoadLineType* pTmpRoadLine = new LocaRoadLineType();
			pTmpRoadLine->pLoca = pTmpLoca;
			EnterCriticalSection(&m_csLocaRoadLine);
			m_LocaRoadLineType.AddTail(pTmpRoadLine);
			LeaveCriticalSection(&m_csLocaRoadLine);
		}
		if(iLocaCount > 0)
		{
			UpDataListLoca();
		}
		memset(szTmpInfo, 0, 64);
		memcpy(szTmpInfo, pDataPointer, 9);
		if(strcmp(szTmpInfo, "<MAPDATA>") != 0)
		{
			MessageBox("错误的文件类型，载入失败", "ERROR", MB_OK|MB_ICONERROR);
			SAFE_DELETE_ARRAY(pbTmpData);
			return;
		}
		pDataPointer += 9;
		int iSize = 0;
		memcpy(&iSize, pDataPointer, sizeof(int));
		pDataPointer += sizeof(int);
		if(iSize > 0)
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
		//加载路径数据
		memset(szTmpInfo, 0, 64);
		memcpy(szTmpInfo, pDataPointer, 14);
		if(strcmp(szTmpInfo, "<ROADLINEDATA>") != 0)
		{
			MessageBox("错误的文件类型，载入失败", "ERROR", MB_OK|MB_ICONERROR);
			return;
		}
		pDataPointer += 14;

		EnterCriticalSection(&m_csLocaList);
		POSITION posRoadLineLoca = m_LocaRoadLineType.GetHeadPosition();
		while(posRoadLineLoca)
		{
			LocaRoadLineType* pTmpRoadLine = m_LocaRoadLineType.GetNext(posRoadLineLoca);
			int iEndPointCount = -1;
			memcpy(&iEndPointCount, pDataPointer, sizeof(int));
			pDataPointer += sizeof(int);
			EnterCriticalSection(&pTmpRoadLine->csEndList);
			for(int i=0; i<iEndPointCount; i++)
			{
				int iEndLocaPointIndex = -1;
				memcpy(&iEndLocaPointIndex, pDataPointer, sizeof(int));
				pDataPointer += sizeof(int);
				POSITION posEndPoint = m_LocaRoadLineType.FindIndex(iEndLocaPointIndex);
				if(posEndPoint)
				{
					LocaRoadLineType* pTmpEndLoca = m_LocaRoadLineType.GetAt(posEndPoint);
					EndLocaType* pNewEndLoca = new EndLocaType();
					pNewEndLoca->pLoca = pTmpEndLoca->pLoca;
					pTmpRoadLine->pEndPosType.AddTail(pNewEndLoca);
					EnterCriticalSection(&pNewEndLoca->csPointList);
					int iPointCount = 0;
					memcpy(&iPointCount, pDataPointer, sizeof(int));
					pDataPointer += sizeof(int);
					for(int iP=0; iP<iPointCount; iP++)
					{
						POINT* pNewPoint = new POINT;
						memcpy(pNewPoint, pDataPointer, sizeof(POINT));
						pDataPointer += sizeof(POINT);
						pNewEndLoca->pRoadLinePointList.AddTail(pNewPoint);
					}
					LeaveCriticalSection(&pNewEndLoca->csPointList);
				}
			}
			LeaveCriticalSection(&pTmpRoadLine->csEndList);
		}
		LeaveCriticalSection(&m_csLocaList);

		CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK1);
		pBtn->SetCheck(0);
		pBtn->EnableWindow(TRUE);

		pBtn = (CButton*)GetDlgItem(IDC_CHECK2);
		pBtn->SetCheck(0);
		pBtn->EnableWindow(TRUE);

		pBtn = (CButton*)GetDlgItem(IDC_CHECK3);
		pBtn->SetCheck(0);
		pBtn->EnableWindow(TRUE);

		pBtn = (CButton*)GetDlgItem(IDC_CHECK4);
		pBtn->SetCheck(0);
		pBtn->EnableWindow(TRUE);

		pBtn = (CButton*)GetDlgItem(IDC_CHECK5);
		pBtn->SetCheck(0);
		pBtn->EnableWindow(TRUE);

		pBtn = (CButton*)GetDlgItem(IDC_CHECK8);
		pBtn->SetCheck(0);
		pBtn->EnableWindow(TRUE);

		pBtn = (CButton*)GetDlgItem(IDC_CHECK6);
		pBtn->SetCheck(0);
		pBtn->EnableWindow(TRUE);

		pBtn = (CButton*)GetDlgItem(IDC_CHECK7);
		pBtn->SetCheck(0);
		pBtn->EnableWindow(TRUE);

		m_ComBoxServer.ResetContent();
		m_ComBoxServer.EnableWindow(FALSE);
		m_SelectServer.EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON4)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK7)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK8)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK6)->EnableWindow(TRUE);

		m_SelectServer.ResetContent();

		SendMessage(WM_PAINT, 0, 0);
	}
}

void CVehicleTrackingSystemDlg::OnLoadMapFromInterNet()
{
	if(g_TestNet)
	{
		HOSTENT *host_entry = NULL; 
		host_entry = gethostbyname("www.sina.com");
		if(host_entry == NULL)
		{
			MessageBox("无法连接到Internet或网络状态不佳，无法使用该功能!");
			return;
		}
		int iSucessTimes = 0;
		char szIP[60] = {0};
		sprintf(szIP, "%d.%d.%d.%d" 
			,(host_entry->h_addr_list[0][0]&0xFF)
			,(host_entry->h_addr_list[0][1]&0xFF)
			,(host_entry->h_addr_list[0][2]&0xFF)
			,(host_entry->h_addr_list[0][3]&0xFF));
		g_TestNet->StartTest(szIP, iSucessTimes);
		if(iSucessTimes <= 1)
		{
			MessageBox("无法连接到Internet或网络状态不佳，无法使用该功能!");
			return;
		}
	}

	m_fIsUpdateInternetMap = FALSE;
	CGetInternetMapDlg cGetInternetMapDlg(this);
	cGetInternetMapDlg.DoModal();

	if(m_fIsUpdateInternetMap)
	{
		EnterCriticalSection(&m_csLocaList);
		if(m_LocaList.GetCount() > 0)
		{
			POSITION pos = m_LocaList.GetHeadPosition();
			BOOL fIsNeedMessage = FALSE;
			while(pos)
			{
				LocaType* pTmpLoca = m_LocaList.GetNext(pos);
				if(pTmpLoca->iPositionX >= 0 || pTmpLoca->iPositionY >= 0)
				{
					fIsNeedMessage = TRUE;
					break;
				}
			}
			if(fIsNeedMessage)
			{
				if(MessageBox("即将导入新地图，是否保留当前断面坐标信息?", "警告", MB_YESNO) == IDNO)
				{
					pos = m_LocaList.GetHeadPosition();
					while(pos)
					{
						LocaType* pTmpLoca = m_LocaList.GetNext(pos);
						pTmpLoca->iPositionX = -100;
						pTmpLoca->iPositionY = -100;
					}
					// clear info
					pos = m_FinalResultList.GetHeadPosition();
					while(pos)
					{
						FinalResultType* pTmpLoca = m_FinalResultList.GetNext(pos);
						pTmpLoca->Loca.x = -100;
						pTmpLoca->Loca.x = -100;
					}
				}
			}
		}
		LeaveCriticalSection(&m_csLocaList);
	}
	SendMessage(WM_PAINT, 0, 0);
}


void CVehicleTrackingSystemDlg::OnLoadMap()
{
	CFileDialog GetFile(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"JPEG文件(*.jpg)|*.jpg|位图文件(*.bmp)|*.bmp||");
	if(GetFile.DoModal() == IDOK)
	{
		EnterCriticalSection(&m_csLocaList);
		if(m_LocaList.GetCount() > 0)
		{
			POSITION pos = m_LocaList.GetHeadPosition();
			BOOL fIsNeedMessage = FALSE;
			while(pos)
			{
				LocaType* pTmpLoca = m_LocaList.GetNext(pos);
				if(pTmpLoca->iPositionX >= 0 || pTmpLoca->iPositionY >= 0)
				{
					fIsNeedMessage = TRUE;
					break;
				}
			}
			if(fIsNeedMessage)
			{
				if(MessageBox("即将导入新地图，是否保留当前断面坐标信息?", "警告", MB_YESNO) == IDNO)
				{
					pos = m_LocaList.GetHeadPosition();
					while(pos)
					{
						LocaType* pTmpLoca = m_LocaList.GetNext(pos);
						pTmpLoca->iPositionX = -100;
						pTmpLoca->iPositionY = -100;
					}
					// clear info
					pos = m_FinalResultList.GetHeadPosition();
					while(pos)
					{
						FinalResultType* pTmpLoca = m_FinalResultList.GetNext(pos);
						pTmpLoca->Loca.x = -100;
						pTmpLoca->Loca.x = -100;
					}

				}
			}
		}
		LeaveCriticalSection(&m_csLocaList);
		CString strFilePathName = GetFile.GetPathName();
		if(m_lpBack == NULL)
		{
			return;
		}
		IPicture* pPic;
		IStream* pStm;
		HANDLE hFile = NULL;
		DWORD dwFileSize, dwByteRead;
		hFile = CreateFile(strFilePathName.GetBuffer(), GENERIC_READ, FILE_SHARE_READ, NULL,
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

		HDC hTmpDC = ::GetDC(GetDlgItem(IDC_STATIC)->GetSafeHwnd());
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
		::ReleaseDC(GetDlgItem(IDC_STATIC)->GetSafeHwnd(), hTmpDC);

		HDC hTmpDC1;
		m_lpBack->GetDC(&hTmpDC1);
		pPic->Render(hTmpDC1, 0, 0, 1024, 786, 0, hmHeight, hmWidth, -hmHeight, NULL);
		m_lpBack->ReleaseDC(hTmpDC1);
		pPic->Release();
		pStm->Release();
		GlobalFree(hGlobal);
		SendMessage(WM_PAINT, 0, 0);
	}
}

void CVehicleTrackingSystemDlg::WindowsPointToBack(int x1, int y1, CRect* pcRect, int& iRx, int& iRy)
{
	iRx = x1 - pcRect->left;
	iRy = y1 - pcRect->top;
	iRx= (int)((float)(iRx * 1024) / (float)(pcRect->right-pcRect->left));
	iRy = (int)((float)(iRy * 786) / (float)(pcRect->bottom-pcRect->top)); 
}

void CVehicleTrackingSystemDlg::OnRButtonDblClk(UINT nFlags, CPoint point)
{
	CRect cTmpRect;
	GetDlgItem(IDC_STATIC)->GetWindowRect(&cTmpRect);
	ScreenToClient(&cTmpRect);
	if(cTmpRect.PtInRect(point))
	{

	}
	CDialog::OnRButtonDblClk(nFlags, point);
}

BOOL CVehicleTrackingSystemDlg::AddServerInfo(NewSeverType* pAddServer)
{
	if(pAddServer == NULL)
	{
		return FALSE;
	}
	BOOL fIsRecord = FALSE;
	EnterCriticalSection(&m_csServerList);
	POSITION pos = m_ServerList.GetHeadPosition();
	while(pos)
	{
		NewSeverType* pTmpServer = m_ServerList.GetNext(pos);
		if(strcmp(pTmpServer->szServerIP, pAddServer->szServerIP) == 0
			&& strcmp(pTmpServer->szDataBaseName, pAddServer->szDataBaseName) == 0)
		{
			fIsRecord = TRUE;
			break;
		}
	}
	if(fIsRecord == TRUE)
	{
		LeaveCriticalSection(&m_csServerList);
		return FALSE;
	}
	m_ServerList.AddTail(pAddServer);
	LeaveCriticalSection((&m_csServerList));
	return TRUE;
}

void CVehicleTrackingSystemDlg::OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	int iColumn = pNMListView->iItem;
	if(iColumn < 0 || iColumn >= m_ListSever.GetItemCount())
	{
		On32782();
	}
	else
	{
		m_ListSever.SetHotItem(iColumn);
		SendMessage(WM_GET_DEV_LIST, (WPARAM)iColumn, 0);
	}
	*pResult = 0;
}

void CVehicleTrackingSystemDlg::UpDataListServer()
{
	m_ListSever.DeleteAllItems();
	if(m_ServerList.GetCount() <= 0)
	{
		return;
	}
	int iIndex = 0;
	POSITION pos = m_ServerList.GetHeadPosition();
	while(pos)
	{
		NewSeverType* pTmpLoca = m_ServerList.GetNext(pos);
		m_ListSever.InsertItem(iIndex, "", 0);
		m_ListSever.SetItemText(iIndex, 0, pTmpLoca->szLocaName);
		m_ListSever.SetItemText(iIndex, 1, pTmpLoca->szServerIP);
		m_ListSever.SetItemText(iIndex, 2, pTmpLoca->szDataBaseName);
		m_ListSever.SetItemText(iIndex, 3, pTmpLoca->szUserName);
		m_ListSever.SetSelectionMark(iIndex);
		iIndex++;
	}
}

BOOL CVehicleTrackingSystemDlg::AddLocaInfo(LocaType* pAddLoca)
{
	if(pAddLoca == NULL)
	{
		return FALSE;
	}
	BOOL fIsRecorded = FALSE;
	EnterCriticalSection(&m_csLocaList);
	POSITION pos = m_LocaList.GetHeadPosition();
	while(pos)
	{
		LocaType* pTmpLoca = m_LocaList.GetNext(pos);
		if(pTmpLoca->iLocaID == pAddLoca->iLocaID 
			&& strcmp(pTmpLoca->szServerIP, pAddLoca->szServerIP) == 0
			&& strcmp(pTmpLoca->szLocaName, pAddLoca->szLocaName) == 0)
		{
			fIsRecorded = TRUE;
			break;
		}
	}
	if(fIsRecorded == TRUE)
	{
		LeaveCriticalSection(&m_csLocaList);
		return FALSE;
	}
	m_LocaList.AddTail(pAddLoca);
	LeaveCriticalSection(&m_csLocaList);

	LocaRoadLineType* pTmpRoadLine = new LocaRoadLineType();
	pTmpRoadLine->pLoca = pAddLoca;
	EnterCriticalSection(&m_csLocaRoadLine);
	m_LocaRoadLineType.AddTail(pTmpRoadLine);
	LeaveCriticalSection(&m_csLocaRoadLine);
	return TRUE;
}

DWORD WINAPI CVehicleTrackingSystemDlg::GetLocaInfoThread(LPVOID lpParam)
{
	if(lpParam == NULL)
	{
		::MessageBox(NULL, "错误的指针，程序即将强制退出!", "ERROR", MB_OK|MB_ICONERROR);
		PostQuitMessage(0);
	}
	CVehicleTrackingSystemDlg* pDlg = (CVehicleTrackingSystemDlg*)lpParam;
	POSITION pos = pDlg->m_ServerList.FindIndex(pDlg->m_iTempServerIndex);
	NewSeverType* pTmp = pDlg->m_ServerList.GetAt(pos);
	int iSucessTimes = 0;
	g_TestNet->StartTest(pTmp->szServerIP, iSucessTimes);
	if(iSucessTimes <= 0)
	{
		pDlg->SendMessage(WM_GET_LOCAINFO_DONE, 2222, 0);
		::CoUninitialize();
		return 2222;
	}
	::CoInitialize(NULL);
	CString strConnect;
	strConnect.Format("Provider=SQLOLEDB.1;Password=%s;User ID=%s;Initial Catalog=%s;Data Source=%s",
		pTmp->szPassWord, pTmp->szUserName, pTmp->szDataBaseName, pTmp->szServerIP);
	_ConnectionPtr pConnection = NULL;
	HRESULT hr = S_OK;
	if(pDlg->m_fIsCancelGetLocaInfo)
	{
		pDlg->SendMessage(WM_GET_LOCAINFO_DONE, 1111, 0);
		::CoUninitialize();
		return 1111;
	}
	try
	{
		hr = pConnection.CreateInstance("ADODB.Connection");
		if(SUCCEEDED(hr))
		{
			pConnection->ConnectionTimeout = 10;
			pConnection->CommandTimeout = 0;
			pConnection->CursorLocation = adUseServer;
			pConnection->IsolationLevel = adXactReadCommitted;
			if(pDlg->m_fIsCancelGetLocaInfo)
			{
				pDlg->SendMessage(WM_GET_LOCAINFO_DONE, 1111, 0);
				::CoUninitialize();
				return 1111;
			}
			hr = pConnection->Open((_bstr_t)(strConnect.GetBuffer()), "", "", adModeUnknown);
		}
	}
	catch (_com_error e)
	{
		if(pConnection != NULL)
		{
			pConnection->Cancel();
		}
		pDlg->SendMessage(WM_GET_LOCAINFO_DONE, 10, 0);
		::CoUninitialize();
		return 10;
	}
	if(FAILED(hr))
	{
		pDlg->SendMessage(WM_GET_LOCAINFO_DONE, 10, 0);
		::CoUninitialize();
		return 10;
	}

	_RecordsetPtr pRecordset = NULL;
	try
	{
		hr = pRecordset.CreateInstance("ADODB.Recordset");
		CString strSQL = "SELECT * FROM Hve_Location_Info ORDER BY Location_Id";
		_variant_t var;
		hr = pRecordset->Open(_variant_t(strSQL), pConnection.GetInterfacePtr(), 
			adOpenDynamic, adLockOptimistic, adCmdText);
		BOOL fIsAddNewLoca = FALSE;
		if(SUCCEEDED(hr))
		{
			while(!pRecordset->adoEOF)
			{
				int iTmpLocaId;
				var = pRecordset->GetCollect((long)0);
				iTmpLocaId = (int)var.lVal;

				var = pRecordset->GetCollect((long)1);
				CString strLocaName = (LPCSTR)_bstr_t(var);
				strLocaName.Trim();

				LocaType* pNewLoca = new LocaType();
				if(pNewLoca)
				{
					pNewLoca->iLocaID = iTmpLocaId;
					memcpy(pNewLoca->szLocaName, strLocaName.GetBuffer(), strLocaName.GetLength());
					memcpy(pNewLoca->szServerName, pTmp->szLocaName, strlen(pTmp->szLocaName));
					memcpy(pNewLoca->szServerIP, pTmp->szServerIP, strlen(pTmp->szServerIP));
					memcpy(pNewLoca->szDataBaseName, pTmp->szDataBaseName, strlen(pTmp->szDataBaseName));
					memcpy(pNewLoca->szUserName, pTmp->szUserName, strlen(pTmp->szUserName));
					memcpy(pNewLoca->szPassWork, pTmp->szPassWord, strlen(pTmp->szPassWord));
					if(pDlg->AddLocaInfo(pNewLoca) == TRUE)
					{
						fIsAddNewLoca = TRUE;
					}
					else
					{
						delete pNewLoca;
						pNewLoca = NULL;
					}
				}

				pRecordset->MoveNext();
			}
		}

		if(fIsAddNewLoca)
		{
			pDlg->UpDataListLoca();
		}
	}
	catch (_com_error e)
	{
		if(pRecordset)
		{
			pRecordset->Close();
			pRecordset = NULL;
		}
		if(pConnection)
		{
			pConnection->Cancel();
			pConnection->Close();
			pConnection = NULL;
		}
		pDlg->SendMessage(WM_GET_LOCAINFO_DONE, 20, 0);
		::CoUninitialize();
		return 20;
	}
	if(pRecordset)
	{
		pRecordset->Close();
		pRecordset = NULL;
	}
	if(pConnection)
	{
		pConnection->Cancel();
		pConnection->Close();
		pConnection = NULL;
	}
	pDlg->SendMessage(WM_GET_LOCAINFO_DONE, 0, 0);
	::CoUninitialize();
	return 0;
}

void CVehicleTrackingSystemDlg::GetLocaInfo(INT iSeverIndex)
{
	m_iTempServerIndex = iSeverIndex;
	EnableWindow(FALSE);
	g_pShowLoading->Start();
	g_pShowLoading->SetInfoWord("加载中，请稍候...");
	m_fIsCancelGetLocaInfo = FALSE;
	m_hSearchLocaThread = CreateThread(NULL, 0, GetLocaInfoThread, this, 0, NULL);
}

void CVehicleTrackingSystemDlg::UpDataListLoca()
{
	m_ListLoca.DeleteAllItems();
	if(m_LocaList.GetCount() <= 0)
	{
		return;
	}
	int iIndex = 0;
	POSITION pos = m_LocaList.GetHeadPosition();
	while(pos)
	{
		LocaType* pTmpLoca = m_LocaList.GetNext(pos);
		char szTmpIndex[20] = {0};
		sprintf(szTmpIndex, "%d", pTmpLoca->iLocaID);
		m_ListLoca.InsertItem(iIndex, "", 0);
		m_ListLoca.SetItemText(iIndex, 0, szTmpIndex);
		m_ListLoca.SetItemText(iIndex, 1, pTmpLoca->szLocaName);
		iIndex++;
	}
}

BOOL CVehicleTrackingSystemDlg::UpDataCarType(LocaType* pLocaType)
{
	if(pLocaType == NULL)
	{
		return FALSE;
	}
	int iSucessTimes = 0;
	g_TestNet->StartTest(pLocaType->szServerIP, iSucessTimes);
	if(iSucessTimes <= 0)
	{
		return FALSE;
	}
	CString strConnect;
	strConnect.Format("Provider=SQLOLEDB.1;Password=%s;User ID=%s;Initial Catalog=%s;Data Source=%s",
		pLocaType->szPassWork, pLocaType->szUserName, pLocaType->szDataBaseName, pLocaType->szServerIP);
	_ConnectionPtr pConnection = NULL;
	HRESULT hr = S_OK;
	try
	{
		hr = pConnection.CreateInstance("ADODB.Connection");
		if(SUCCEEDED(hr))
		{
			pConnection->ConnectionTimeout = 10;
			pConnection->CommandTimeout = 0;
			pConnection->CursorLocation = adUseServer;
			pConnection->IsolationLevel = adXactReadCommitted;
			hr = pConnection->Open((_bstr_t)(strConnect.GetBuffer()), "", "", adModeUnknown);
		}
	}
	catch (_com_error)
	{
		if(pConnection != NULL)
		{
			pConnection->Cancel();
		}
		return FALSE;
	}
	if(FAILED(hr))
	{
		MessageBox("连接数据库失败，获取车辆类型失败", "ERROR", MB_OK|MB_ICONERROR);
		return FALSE;
	}

	_RecordsetPtr pRecordset = NULL;
	try
	{
		hr = pRecordset.CreateInstance("ADODB.Recordset");
		CString strSQL = "SELECT * FROM Hve_Obj_Type ORDER BY Obj_Id";
		_variant_t var;
		hr = pRecordset->Open(_variant_t(strSQL), pConnection.GetInterfacePtr(), 
			adOpenDynamic, adLockOptimistic, adCmdText);
		BOOL fIsAddNewLoca = FALSE;
		if(SUCCEEDED(hr))
		{
			m_ComBoxCarType.ResetContent();
			while(!pRecordset->adoEOF)
			{
				var = pRecordset->GetCollect((long)1);
				CString strCarType = (LPCSTR)_bstr_t(var);
				strCarType.Trim();

				m_ComBoxCarType.AddString(strCarType.GetBuffer());
				pRecordset->MoveNext();
			}
			m_ComBoxCarType.AddString("全部类型");
			int iTotleCount = m_ComBoxCarType.GetCount();
			m_ComBoxCarType.SetCurSel(iTotleCount - 1);
		}
		else
		{
			if(pRecordset)
			{
				pRecordset->Close();
				pRecordset = NULL;
			}
			if(pConnection)
			{
				pConnection->Cancel();
				pConnection->Close();
				pConnection = NULL;
			}
			return FALSE;
		}
	}
	catch (_com_error e)
	{
		if(pRecordset)
		{
			pRecordset->Close();
			pRecordset = NULL;
		}
		if(pConnection)
		{
			pConnection->Cancel();
			pConnection->Close();
			pConnection = NULL;
		}
	}
	if(pRecordset)
	{
		pRecordset->Close();
		pRecordset = NULL;
	}
	if(pConnection)
	{
		pConnection->Cancel();
		pConnection->Close();
		pConnection = NULL;
	}
	return TRUE;
}

void CVehicleTrackingSystemDlg::UpdateSearchInfo()
{
	if(m_iSelectIndex < 0 || m_iSelectIndex >= m_LocaList.GetCount())
	{
		m_fIsUpDataLoca = FALSE;
		return;
	}
	POSITION pos = m_LocaList.FindIndex(m_iSelectIndex);
	if(!pos)
	{
		m_fIsUpDataLoca = FALSE;
		return;
	}
	LocaType* pTmpLoca = m_LocaList.GetAt(pos);
	if(pTmpLoca == NULL)
	{
		m_fIsUpDataLoca = FALSE;
		return;
	}
	char szTitleInfo[256] = {0};
	sprintf(szTitleInfo, "断面%s过往车辆信息查询", pTmpLoca->szLocaName);
	SetDlgItemText(IDC_STATIC222, szTitleInfo);

	SYSTEMTIME cStsTemTime;
	CTime cTime = CTime::GetCurrentTime();
	cTime.GetAsSystemTime(cStsTemTime);
	m_DateEnd.SetTime(&cStsTemTime);

	DWORD64 dw64Time = cTime.GetTime();
	dw64Time -= 3600;
	CTime cTimeEnd(dw64Time);
	cTimeEnd.GetAsSystemTime(cStsTemTime);
	m_DateStart.SetTime(&cStsTemTime);
	SetDlgItemText(IDC_EDIT1, "");
	if(UpDataCarType(pTmpLoca) != TRUE)
	{
		m_ComBoxCarType.ResetContent();
		m_ComBoxCarType.AddString("全部车型");
		m_ComBoxCarType.SetCurSel(0);
	}

	m_comboDir1.SetCurSel(0);
	m_comboDir2.SetCurSel(0);

	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK1);
	pBtn->SetCheck(0);
	pBtn->EnableWindow(TRUE);

	pBtn = (CButton*)GetDlgItem(IDC_CHECK2);
	pBtn->SetCheck(0);
	pBtn->EnableWindow(TRUE);

	pBtn = (CButton*)GetDlgItem(IDC_CHECK3);
	pBtn->SetCheck(0);
	pBtn->EnableWindow(TRUE);

	pBtn = (CButton*)GetDlgItem(IDC_CHECK4);
	pBtn->SetCheck(0);
	pBtn->EnableWindow(TRUE);

	pBtn = (CButton*)GetDlgItem(IDC_CHECK5);
	pBtn->SetCheck(0);
	pBtn->EnableWindow(TRUE);

	pBtn = (CButton*)GetDlgItem(IDC_CHECK8);
	pBtn->SetCheck(0);
	pBtn->EnableWindow(TRUE);

	pBtn = (CButton*)GetDlgItem(IDC_CHECK6);
	pBtn->SetCheck(0);
	pBtn->EnableWindow(TRUE);

	pBtn = (CButton*)GetDlgItem(IDC_CHECK7);
	pBtn->SetCheck(0);
	pBtn->EnableWindow(TRUE);

	m_ComBoxServer.EnableWindow(FALSE);
// 	m_SearchCarStartTime.EnableWindow(FALSE);
// 	m_SearchCarEndTime.EnableWindow(FALSE);
	m_SelectServer.ResetContent();
	GetDlgItem(IDC_BUTTON1)->EnableWindow(TRUE);
	m_fIsUpDataLoca = FALSE;
}

void CVehicleTrackingSystemDlg::ProceSearch1Done(int iRetType)
{
	g_pShowLoading->Stop();
	CloseHandle(m_hSearchPassCarThread);
	m_hSearchPassCarThread = NULL;
	switch(iRetType)
	{
	case 0:
		{
			m_iResultType = 1;
			int iCount = m_ListResult.GetItemCount();
			if(iCount > 0)
			{
				LoadResultPic(0);
				UpDatePicBuffer();
				CString strInfo;
				strInfo.Format("搜索完成，共搜到%d条信息", iCount);
				MessageBox(strInfo.GetBuffer());
			}
			else
			{
				MessageBox("搜索完成,未搜索到任何记录!");
			}
		}
		break;
	case 10:
		MessageBox("搜索被中断,已停止搜索");
		break;
	case 11:
		MessageBox("搜索失败,错误的断面信息");
		break;
	case 12:
	case 13:
		MessageBox("搜索失败,连接数据库失败");
		break;
	case 14:
		MessageBox("搜索失败,查询数据库出错");
		break;
	case 2222:
		MessageBox("搜索失败,创建网络连接失败");
		break;
	}
	GetDlgItem(IDC_BUTTON1)->EnableWindow(TRUE);
	EnableWindow(TRUE);
}

void CVehicleTrackingSystemDlg::ProcGetLocaInfoDone(int iRetType)
{
	g_pShowLoading->Stop();
	CloseHandle(m_hSearchLocaThread);
	m_hSearchLocaThread = NULL;
	m_iTmpSelectIndex = 0;
	switch(iRetType)
	{
	case 2222:
		MessageBox("无法建立网络连接，获取失败!");
		break;
	case 1111:
		MessageBox("已中止获取断面信息!");
		break;
	case 10:
		MessageBox("连接数据库失败，获取失败!");
		break;
	case 20:
		MessageBox("获取断面信息错误，获取失败!");
		break;
	case 0:
		break;
	default:
		MessageBox("未知的执行结果!");
		break;
	}
	EnableWindow(TRUE);
	SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	SetWindowPos(&CWnd::wndNoTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
}

void CVehicleTrackingSystemDlg::OnSyncSearch()
{
	int iServerCount = 0;
	EnterCriticalSection(&m_csServerList);
	iServerCount = (int)m_ServerList.GetCount();
	LeaveCriticalSection(&m_csServerList);
	if(iServerCount <= 0)
	{
		MessageBox("未设置数据站点，无法搜索!");
		return;
	}
	GetDlgItem(IDC_BUTTON3)->EnableWindow(FALSE);
	m_ListResult.EnableWindow(FALSE);
	char szPlate[30] = {0};
	int iLen = GetDlgItemText(IDC_EDIT3, szPlate, 30);
	if(iLen <= 0)
	{
		MessageBox("车牌号码不能为空", "ERROR", MB_OK|MB_ICONERROR);
		GetDlgItem(IDC_BUTTON3)->EnableWindow(TRUE);
		m_ListResult.EnableWindow(TRUE);
		return;
	}
	CString strViewName = "VTHVE_DATA_VIEW_";
	strViewName += g_strMacAddr;
	CString strSQL;
	strSQL.Format("SELECT Location_Id, Pass_Time, Plate_No, big_image_1_path, big_image_2_path FROM %s WHERE Plate_No='",
		strViewName.GetBuffer());
	strSQL += szPlate;
	strSQL += "'";

	BOOL fIsSetTime = FALSE;
	CTime cTmpStartTime = CTime::GetCurrentTime();
	CTime cTmpEndTime = CTime::GetCurrentTime();
	CButton* pBut = (CButton*)GetDlgItem(IDC_CHECK6);
	if(pBut->GetCheck() == 1)
	{
		CTime cTmpTime;
		DWORD64 dwTmpSecond = 0;
		m_DateStart.GetTime(cTmpTime);
		dwTmpSecond = cTmpTime.GetTime();
		dwTmpSecond = (dwTmpSecond / 60) * 60;
		CTime cStartTime(dwTmpSecond);

		dwTmpSecond = 0;
		m_DateEnd.GetTime(cTmpTime);
		dwTmpSecond = cTmpTime.GetTime();
		dwTmpSecond = (dwTmpSecond / 60) * 60 + 59;
		CTime cEndTime(dwTmpSecond);
		if(cStartTime >= cEndTime)
		{
			if(MessageBox("时间片错误,是否确认查询", "警告", MB_YESNO) == IDNO)
			{
				GetDlgItem(IDC_BUTTON3)->EnableWindow(TRUE);
				m_ListResult.EnableWindow(TRUE);
				return;
			}
		}
		CString strUsedTime;
		strUsedTime.Format(" AND Pass_Time BETWEEN '%s' AND '%s'", 
			cStartTime.Format("%Y-%m-%d %H:%M:00"), cEndTime.Format("%Y-%m-%d %H:%M:00"));
		strSQL += strUsedTime;
		fIsSetTime = TRUE;
		cTmpStartTime = cStartTime;
		cTmpEndTime = cEndTime;
	}
	ClearSearchClass();

	char szCurrentPath[MAX_PATH] = {0};
	GetModuleFileName(NULL, szCurrentPath, MAX_PATH);
	PathRemoveFileSpec(szCurrentPath);
	CString strPath = szCurrentPath;
	strPath += "\\TmpFinalResult\\";
	if(PathFileExists(strPath.GetBuffer()))
	{
		ClearTmpResultPic(strPath.GetBuffer());
	}

	g_pShowLoading->Start();
	g_pShowLoading->SetInfoWord("查询中，请稍候...");
	EnableWindow(FALSE);
	pBut = (CButton*)GetDlgItem(IDC_CHECK7);
	if(pBut->GetCheck() == 1)
	{
		int iSeverCount = m_SelectServer.GetCount();
		if(iSeverCount <= 0)
		{
			if(MessageBox("未指定任何服务器，是否使用默认设置搜索?", "警告", MB_YESNO) == IDNO)
			{
				GetDlgItem(IDC_BUTTON3)->EnableWindow(TRUE);
				m_ListResult.EnableWindow(TRUE);
				return;
			}
			EnterCriticalSection(&m_csServerList);
			POSITION pos = m_ServerList.GetHeadPosition();
			while(pos)
			{
				NewSeverType* pTmpServer = m_ServerList.GetNext(pos);
				CSearchCarPassLocaType* pTmpSearchClass = new CSearchCarPassLocaType(pTmpServer, strSQL.GetBuffer(), fIsSetTime, cTmpStartTime, cTmpEndTime);
				pTmpSearchClass->Start();
				EnterCriticalSection(&m_csSearchClassList);
				m_SearchClassList.AddTail(pTmpSearchClass);
				LeaveCriticalSection(&m_csSearchClassList);
			}
			LeaveCriticalSection(&m_csServerList);
		}
		else
		{
			EnterCriticalSection(&m_csServerList);
			for(int iIndex=0; iIndex<iSeverCount; iIndex++)
			{
				int iServerIndex = (int)m_SelectServer.GetItemData(iIndex);
				POSITION pos = m_ServerList.FindIndex(iServerIndex);
				if(pos)
				{
					NewSeverType* pTmpServer = m_ServerList.GetAt(pos);
					CSearchCarPassLocaType* pTmpSearchClass = new CSearchCarPassLocaType(pTmpServer, strSQL.GetBuffer(), fIsSetTime, cTmpStartTime, cTmpEndTime);
					pTmpSearchClass->Start();
					EnterCriticalSection(&m_csSearchClassList);
					m_SearchClassList.AddTail(pTmpSearchClass);
					LeaveCriticalSection(&m_csSearchClassList);
				}
			}
			LeaveCriticalSection(&m_csServerList);
		}
	}
	else
	{
		EnterCriticalSection(&m_csServerList);
		POSITION pos = m_ServerList.GetHeadPosition();
		while(pos)
		{
			NewSeverType* pTmpServer = m_ServerList.GetNext(pos);
			CSearchCarPassLocaType* pTmpSearchClass = new CSearchCarPassLocaType(pTmpServer, strSQL.GetBuffer(), fIsSetTime, cTmpStartTime, cTmpEndTime);
			pTmpSearchClass->Start();
			EnterCriticalSection(&m_csSearchClassList);
			m_SearchClassList.AddTail(pTmpSearchClass);
			LeaveCriticalSection(&m_csSearchClassList);
		}
		LeaveCriticalSection(&m_csServerList);
	}
	SetTimer(10086, 1000, NULL);
	//OnBnClickedButton3();
}

BOOL CVehicleTrackingSystemDlg::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	switch(message)
	{
	case WM_GET_DEV_LIST:
		GetLocaInfo((int)wParam);
		break;
	case WM_UPDATE_SEARCH_INFO:
		UpdateSearchInfo();
		break;
	case WM_PROC_SEARCH_DONE:
		ProceSearchDone();
		break;
	case WM_SERCH1_DONE:
		ProceSearch1Done((int)wParam);
		break;
	case WM_GET_LOCAINFO_DONE:
		ProcGetLocaInfoDone((int)wParam);
		break;
	case WM_START_SYNC_SEARCH:
		// OnSyncSearch();
		OnBnClickedButton3();
		break;
	}
	return CDialog::OnWndMsg(message, wParam, lParam, pResult);
}

int CVehicleTrackingSystemDlg::GetLocaIndex(LocaType* pLoca)
{
	if(pLoca == NULL)
	{
		return -1;
	}
	int iIndex = 0;
	EnterCriticalSection(&m_csLocaList);
	POSITION pos = m_LocaList.GetHeadPosition();
	while(pos)
	{
		LocaType* pTmpLoca = m_LocaList.GetNext(pos);
		if(pTmpLoca->iLocaID == pLoca->iLocaID && pTmpLoca->iPositionX == pLoca->iPositionX
			&& pTmpLoca->iPositionY == pLoca->iPositionY && pTmpLoca->iPositionY == pLoca->iPositionY
			&& strcmp(pTmpLoca->szServerIP, pLoca->szServerIP) == 0
			&& strcmp(pTmpLoca->szLocaName, pLoca->szLocaName) == 0)
		{
			LeaveCriticalSection(&m_csLocaList);
			return iIndex;
		}
		iIndex++;
	}
	LeaveCriticalSection(&m_csLocaList);
	return -1;
}

void CVehicleTrackingSystemDlg::OnSave()
{
	CFileDialog cSaveFile(FALSE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "场景文件(*.SD)|*.SD||");
	if(cSaveFile.DoModal() == IDOK)
	{
		CString strSaveFileName = cSaveFile.GetFileName();
		if(strSaveFileName.GetLength() <= 3)
		{
			MessageBox("文件保存名错误，保存失败!", "ERROR", MB_OK|MB_ICONERROR);
			return;
		}
		if(strSaveFileName[strSaveFileName.GetLength()-1] != 'D'
			&& strSaveFileName[strSaveFileName.GetLength()-1] != 'd')
		{
			strSaveFileName += ".SD";
		}
		FILE* fp = fopen(strSaveFileName.GetBuffer(), "wb");
		if(fp)
		{
			fwrite("<SEVERINFO>", 11, 1, fp);
			int iServerCount = (int)m_ServerList.GetCount();
			fwrite(&iServerCount, sizeof(int), 1, fp);
			POSITION pos = m_ServerList.GetHeadPosition();
			NewSeverType cTmpServer;
			for(int iIndex=0; iIndex<iServerCount; iIndex++)
			{
				NewSeverType* pTmp = m_ServerList.GetNext(pos);
				if(pTmp)
				{
					memcpy(&cTmpServer, pTmp, sizeof(NewSeverType));
					for(int iIndex=0; iIndex<16; iIndex++)
					{
						cTmpServer.szPassWord[iIndex] = ~cTmpServer.szPassWord[iIndex];
					}
					fwrite(&cTmpServer, sizeof(NewSeverType), 1, fp);
				}
			}

			fwrite("<LOCAINFO>", 10, 1, fp);
			int iLocaCount = (int)m_LocaList.GetCount();
			fwrite(&iLocaCount, sizeof(int), 1, fp);
			pos = m_LocaList.GetHeadPosition();
			LocaType pTmpLoca;
			for(int iIndex=0; iIndex<iLocaCount; iIndex++)
			{
				LocaType* pTmp = m_LocaList.GetNext(pos);
				if(pTmp)
				{
					memcpy(&pTmpLoca, pTmp, sizeof(LocaType));
					for(int iIndex=0; iIndex<16; iIndex++)
					{
						pTmpLoca.szPassWork[iIndex] = ~pTmpLoca.szPassWork[iIndex];
					}
					fwrite(&pTmpLoca, sizeof(LocaType), 1, fp);
				}
			}

			fwrite("<MAPDATA>", 9, 1, fp);
			int iSize = 1024*786*3;
			PBYTE pMapData = new BYTE[iSize];
			if(pMapData == NULL)
			{
				iSize = 0;
			}
			fwrite(&iSize, sizeof(int), 1, fp);
			if(iSize > 0)
			{
				memset(pMapData, 0, iSize);
				if(m_lpBack)
				{
					DDSURFACEDESC2 ddsd;
					ZeroMemory(&ddsd, sizeof(ddsd));
					ddsd.dwSize = sizeof(ddsd);
					m_lpBack->Lock(NULL, &ddsd, 0, NULL);
					PBYTE pFrameData = (PBYTE)ddsd.lpSurface;
					INT indexX, indexY;
					PBYTE tmpImage = pMapData;
					PBYTE pImage = pFrameData;
					if(ddsd.ddpfPixelFormat.dwRGBBitCount == 32)
					{
						for(indexY=0; indexY<786; indexY++)
						{
							for(indexX=0; indexX<1024; indexX++)
							{
								*tmpImage++ = *(pImage+2); 
								*tmpImage++ = *(pImage+1); 
								*tmpImage++ = *(pImage);
								pImage += 4;
							}
						}
					}
					else if(ddsd.ddpfPixelFormat.dwRGBBitCount == 16)
					{
						for(indexY=0; indexY<786; indexY++)
						{
							for(indexX=0; indexX<1024; indexX++)
							{
								WORD wColor;
								memcpy(&wColor, pImage, 2);
								pImage += 2;
								int r = (BYTE)((wColor >> 11) & 0x1F);
								int g = (BYTE)((wColor >> 5) & 0x3F);
								int b = (BYTE)(wColor & 0x1F);
								r <<= 3;
								g <<= 2;
								b <<= 3;
								if(r < 0) r = 0;
								else if(r > 255) r = 255;
								if(g < 0) g = 0;
								else if(g > 255) g = 255;
								if(b < 0) b = 0;
								else if(b > 255) b = 255;
								*tmpImage++ = (BYTE)r; 
								*tmpImage++ = (BYTE)g; 
								*tmpImage++ = (BYTE)b;
							}
						}
					}
					m_lpBack->Unlock(NULL);
				}
				fwrite(pMapData, iSize, 1, fp);
			}

			fwrite("<ROADLINEDATA>", 14, 1, fp);
			int iEndPointCount;
			EnterCriticalSection(&m_csLocaRoadLine);
			POSITION posRoadLine = m_LocaRoadLineType.GetHeadPosition();
			while(posRoadLine)
			{
				LocaRoadLineType* pTmpRoadLine = m_LocaRoadLineType.GetNext(posRoadLine);
				EnterCriticalSection(&pTmpRoadLine->csEndList);
				iEndPointCount = (int)pTmpRoadLine->pEndPosType.GetCount();
				fwrite(&iEndPointCount, sizeof(int), 1, fp);
				POSITION posEndLoca = pTmpRoadLine->pEndPosType.GetHeadPosition();
				while(posEndLoca)
				{
					EndLocaType* pTmpEndLoca = pTmpRoadLine->pEndPosType.GetNext(posEndLoca);
					int iIndexLoca = GetLocaIndex(pTmpEndLoca->pLoca);
					fwrite(&iIndexLoca, sizeof(iIndexLoca), 1, fp);
					EnterCriticalSection(&pTmpEndLoca->csPointList);
					int iPointCount = (int)pTmpEndLoca->pRoadLinePointList.GetCount();
					fwrite(&iPointCount, sizeof(int), 1, fp);
					POSITION posPoint = pTmpEndLoca->pRoadLinePointList.GetHeadPosition();
					while(posPoint)
					{
						POINT* pTmpPoint = pTmpEndLoca->pRoadLinePointList.GetNext(posPoint);
						fwrite(pTmpPoint, sizeof(POINT), 1, fp);
					}

					LeaveCriticalSection(&pTmpEndLoca->csPointList);
				}
				LeaveCriticalSection(&pTmpRoadLine->csEndList);
			}
			LeaveCriticalSection(&m_csLocaRoadLine);

			fclose(fp);
		}
	}
}

void CVehicleTrackingSystemDlg::ClearSencer()
{
	if(m_lpBack)
	{
		DDSURFACEDESC2 ddsd;
		ZeroMemory(&ddsd, sizeof(ddsd));
		ddsd.dwSize = sizeof(ddsd);
		m_lpBack->Lock(NULL, &ddsd, 0, NULL);
		PBYTE pbBuf = (PBYTE)ddsd.lpSurface;
		memset(pbBuf, 250, ddsd.dwHeight * ddsd.lPitch);
		m_lpBack->Unlock(NULL);
	}

	EnterCriticalSection(&m_csLocaList);
	while(m_LocaList.GetCount() > 0)
	{
		LocaType* pTmpLoca = m_LocaList.RemoveHead();
		delete pTmpLoca;
		pTmpLoca = NULL;
	}
	LeaveCriticalSection(&m_csLocaList);
	UpDataListLoca();

	EnterCriticalSection(&m_csServerList);
	while(m_ServerList.GetCount() > 0)
	{
		NewSeverType* pTmpServer = m_ServerList.RemoveHead();
		delete pTmpServer;
		pTmpServer = NULL;
	}
	LeaveCriticalSection(&m_csServerList);
	UpDataListServer();

	EnterCriticalSection(&m_csLocaRoadLine);
	while(m_LocaRoadLineType.GetCount() > 0)
	{
		LocaRoadLineType* pTmpLocaRoadLine = m_LocaRoadLineType.RemoveHead();
		delete pTmpLocaRoadLine;
		pTmpLocaRoadLine = NULL;
	}
	LeaveCriticalSection(&m_csLocaRoadLine);

	EnterCriticalSection(&m_csFinalResultList);
	while(m_FinalResultList.GetCount() > 0)
	{
		FinalResultType* pTmpFinalResult = m_FinalResultList.RemoveHead();
		delete pTmpFinalResult;
		pTmpFinalResult = NULL;
	}
	LeaveCriticalSection(&m_csFinalResultList);
	m_strRPicPath = "";
	m_strCPicPath = "";

	m_iSelectIndex = -1;
	GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON3)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON4)->EnableWindow(FALSE);
	SetDlgItemText(IDC_EDIT1, "");
	m_ComBoxCarType.ResetContent();
	m_ComBoxCarType.AddString("全部车型");
	m_ComBoxCarType.SetCurSel(0);
	SetDlgItemText(IDC_EDIT3, "");
// 	m_SearchCarStartTime.EnableWindow(FALSE);
// 	m_SearchCarEndTime.EnableWindow(FALSE);
	m_ComBoxServer.ResetContent();
	m_iResultType = -1;
	m_iSelectResultIndex = -1;
	m_iTmpSelectIndex = -1;
	m_ListResult.DeleteAllItems();
	SetDefualtPicBuffer();

	SendMessage(WM_PAINT, 0, 0);
}

void CVehicleTrackingSystemDlg::OnClearSencer()
{
	ClearSencer();
	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK1);
	pBtn->SetCheck(0);
	pBtn->EnableWindow(TRUE);

	pBtn = (CButton*)GetDlgItem(IDC_CHECK2);
	pBtn->SetCheck(0);
	pBtn->EnableWindow(TRUE);

	pBtn = (CButton*)GetDlgItem(IDC_CHECK3);
	pBtn->SetCheck(0);
	pBtn->EnableWindow(TRUE);

	pBtn = (CButton*)GetDlgItem(IDC_CHECK4);
	pBtn->SetCheck(0);
	pBtn->EnableWindow(TRUE);

	pBtn = (CButton*)GetDlgItem(IDC_CHECK5);
	pBtn->SetCheck(0);
	pBtn->EnableWindow(TRUE);

	pBtn = (CButton*)GetDlgItem(IDC_CHECK8);
	pBtn->SetCheck(0);
	pBtn->EnableWindow(TRUE);

	pBtn = (CButton*)GetDlgItem(IDC_CHECK6);
	pBtn->SetCheck(0);
	pBtn->EnableWindow(TRUE);

	pBtn = (CButton*)GetDlgItem(IDC_CHECK7);
	pBtn->SetCheck(0);
	pBtn->EnableWindow(TRUE);

	SetDlgItemText(IDC_EDIT1, "");
	SetDlgItemText(IDC_EDIT3, "");
	m_SelectServer.ResetContent();
	m_ComBoxServer.ResetContent();
	m_ComBoxCarType.ResetContent();
	m_ComBoxServer.EnableWindow(FALSE);
// 	m_SearchCarStartTime.EnableWindow(FALSE);
// 	m_SearchCarEndTime.EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON3)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON4)->EnableWindow(FALSE);
}

void CVehicleTrackingSystemDlg::ReFreshFinalREsultLocaPoint(LocaType* pLoca)
{
	EnterCriticalSection(&m_csFinalResultList);
	POSITION posFinalResult = m_FinalResultList.GetHeadPosition();
	while(posFinalResult)
	{
		FinalResultType* pTmpFinalResult = m_FinalResultList.GetNext(posFinalResult);
		if(strcmp(pTmpFinalResult->szLocaName, pLoca->szLocaName) == 0
			&& strcmp(pTmpFinalResult->szServerIP, pLoca->szServerIP) == 0)
		{
			pTmpFinalResult->Loca.x = pLoca->iPositionX;
			pTmpFinalResult->Loca.y = pLoca->iPositionY;
		}
	}
	LeaveCriticalSection(&m_csFinalResultList);
}

void CVehicleTrackingSystemDlg::OnNMRdblclkList2(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
}

void CVehicleTrackingSystemDlg::OnNMDblclkList2(NMHDR *pNMHDR, LRESULT *pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	int iColumn = pNMListView->iItem;
	if(iColumn < 0 || iColumn > m_ListLoca.GetItemCount())
	{
		return;
	}

	EnterCriticalSection(&m_csLocaList);
	POSITION pos = m_LocaList.FindIndex(iColumn);
	if(pos)
	{
		LocaType* pTmpLoca = m_LocaList.GetAt(pos);
		POINT cPoint;
		cPoint.x = pTmpLoca->iPositionX;
		cPoint.y = pTmpLoca->iPositionY;
		LeaveCriticalSection(&m_csLocaList);
		CSetLocaPointDlg cSetPointDlg(&cPoint, this);
		cSetPointDlg.DoModal();
		//pTmpLoca->iPositionX = cPoint.x;
		//pTmpLoca->iPositionY = cPoint.y;
		if(m_iResultType == 2) ReFreshFinalREsultLocaPoint(pTmpLoca);
		SendMessage(WM_PAINT, 0, 0 );
	}
	else
	{
		LeaveCriticalSection(&m_csLocaList);
	}
	*pResult = 0;
}

DWORD WINAPI CVehicleTrackingSystemDlg::SearchPassCarThread(LPVOID lpParam)
{
	if(lpParam == NULL)
	{
		return 0xFFFFFFFF;
	}
	CVehicleTrackingSystemDlg* pDlg = (CVehicleTrackingSystemDlg*)lpParam;
	pDlg->EnableWindow(FALSE);
	g_pShowLoading->Start();
	g_pShowLoading->SetInfoWord("查询中，请稍候...");
	if(pDlg->m_fIsBreak)
	{
		pDlg->SendMessage(WM_SERCH1_DONE, (WPARAM)10, 0);
		return 10;
	}
	EnterCriticalSection(&pDlg->m_csLocaList);
	POSITION pos = pDlg->m_LocaList.FindIndex(pDlg->m_iSelectIndex);
	if(!pos)
	{
		pDlg->SendMessage(WM_SERCH1_DONE, (WPARAM)11, 0);
		return 11;
	}
	LocaType* pTmpLoca = pDlg->m_LocaList.GetAt(pos);
	LocaType UsedLocaType;
	memcpy(&UsedLocaType, pTmpLoca, sizeof(LocaType));
	LeaveCriticalSection(&pDlg->m_csLocaList);
	if(pDlg->m_fIsBreak)
	{
		pDlg->SendMessage(WM_SERCH1_DONE, (WPARAM)10, 0);
		return 10;
	}
	int iSucessTimes = 0;
	g_TestNet->StartTest(UsedLocaType.szServerIP, iSucessTimes);
	if(iSucessTimes <= 0)
	{
		pDlg->SendMessage(WM_SERCH1_DONE, (WPARAM)2222, 0);
		return 2222;
	}
	::CoInitialize(NULL);
	CString strConnect;
	strConnect.Format("Provider=SQLOLEDB.1;Password=%s;User ID=%s;Initial Catalog=%s;Data Source=%s",
	UsedLocaType.szPassWork, UsedLocaType.szUserName, UsedLocaType.szDataBaseName, UsedLocaType.szServerIP);
	_ConnectionPtr pConnection = NULL;
	HRESULT hr = S_OK;
	try
	{
		hr = pConnection.CreateInstance("ADODB.Connection");
		if(pDlg->m_fIsBreak)
		{
			pDlg->SendMessage(WM_SERCH1_DONE, (WPARAM)10, 0);
			return 10;
		}
		if(SUCCEEDED(hr))
		{
			pConnection->ConnectionTimeout = 10;
			pConnection->CommandTimeout = 0;
			pConnection->CursorLocation = adUseServer;
			pConnection->IsolationLevel = adXactReadCommitted;
			hr = pConnection->Open((_bstr_t)(strConnect.GetBuffer()), "", "", adModeUnknown);
		}
	}
	catch(_com_error e)
	{
		if(pConnection != NULL)
		{
			pConnection->Cancel();
		}
		::CoUninitialize();
		pDlg->SendMessage(WM_SERCH1_DONE, (WPARAM)12, 0);
		return 12;
	}
	if(FAILED(hr))
	{
		::CoUninitialize();
		pDlg->SendMessage(WM_SERCH1_DONE, (WPARAM)13, 0);
		return 13;
	}
	_RecordsetPtr pRecordset = NULL;
	try
	{
		if(pDlg->m_fIsBreak)
		{
			pDlg->SendMessage(WM_SERCH1_DONE, (WPARAM)10, 0);
			return 10;
		}
		hr = pRecordset.CreateInstance("ADODB.Recordset");
		CTime cStartTime;
		CTime cEndTime;
		pDlg->m_DateStart.GetTime(cStartTime);
		pDlg->m_DateEnd.GetTime(cEndTime);

		CString strViewName = "VTHVE_DATA_VIEW_";
		strViewName += g_strMacAddr;
		CString strSql1;
		strSql1.Format("IF EXISTS (SELECT TABLE_NAME FROM INFORMATION_SCHEMA.VIEWS WHERE TABLE_NAME = N'%s') DROP VIEW %s",
			strViewName.GetBuffer(), strViewName.GetBuffer());
		_variant_t RecordsetAffected;
		pConnection->Execute((_bstr_t)strSql1.GetBuffer(), &RecordsetAffected, adCmdText);
		DWORD64 dwTmpStartTime = cStartTime.GetTime();
		DWORD64 dwTmpEndTime = cEndTime.GetTime();
		CString strTableList = "";
		if(cEndTime.GetYear() > cStartTime.GetYear()
			|| cEndTime.GetMonth() > cStartTime.GetMonth()
			|| cEndTime.GetDay() > cStartTime.GetDay())
		{
			while(dwTmpStartTime <= dwTmpEndTime+86400)
			{
				CTime cTmpCurrentTime(dwTmpStartTime);
				CString strTmpCommand = cTmpCurrentTime.Format("Hve_Data_%Y%m%d");
				if(IsTableExists(pConnection, strTmpCommand) == TRUE)
				{
					if(strTableList == "")
						strTableList.Format("(SELECT Pass_Time, Plate_No, big_image_1_path, big_image_2_path,\
											Obj_Id,Speeding,Road_no,Location_Id	FROM %s)", strTmpCommand);
					else
						strTableList.AppendFormat("union (SELECT Pass_Time, Plate_No, big_image_1_path, big_image_2_path,\
												  Obj_Id,Speeding,Road_no,Location_Id FROM %s)", strTmpCommand.GetBuffer());
				}
				dwTmpStartTime += 86400;
			}
		}
		else
		{
			CString strTmpCommand = cStartTime.Format("Hve_Data_%Y%m%d");
			if(IsTableExists(pConnection, strTmpCommand) == TRUE)
				strTableList.Format("(SELECT * FROM %s)", strTmpCommand.GetBuffer());
		}

		if(strTableList == "")
		{
			CString strTmpSql;
			HRESULT hr = E_FAIL;
			strTmpSql = "SELECT name FROM sysobjects WHERE (name LIKE 'Hve_Data_%')";
			_RecordsetPtr pPtr;
			hr = pPtr.CreateInstance("ADODB.Recordset");
			if(hr != S_OK) return FALSE;
			hr = pPtr->Open(_variant_t(strTmpSql), pConnection.GetInterfacePtr(), 
				adOpenDynamic, adLockOptimistic, adCmdText);
			if(hr != S_OK) return FALSE;
			if(pPtr->GetadoEOF())
			{
				pPtr->Close();
				pPtr.Release();

				pDlg->m_ListResult.DeleteAllItems();
				pDlg->SendMessage(WM_SERCH1_DONE, (WPARAM)0, 0);

				return FALSE;
			}
			CString strTmpTable = (char*)_bstr_t(pPtr->GetCollect("name"));
			strTmpTable.TrimRight();
			strTableList.Format("(SELECT * FROM %s)", strTmpTable);
			pPtr->Close();
			pPtr.Release();
		}
		strSql1.Format("CREATE VIEW %s AS %s", strViewName.GetBuffer(), strTableList.GetBuffer());
		pConnection->Execute((_bstr_t)strSql1.GetBuffer(), &RecordsetAffected, adCmdText);

		CString strStartTime = cStartTime.Format("%Y-%m-%d %H:%M:%S");
		CString strEndTime = cEndTime.Format("%Y-%m-%d %H:%M:%S");
		CString strCarColor = "";
		char szTmpColor[20] = {0};
		int iRetLen = pDlg->GetDlgItemText(IDC_EDIT1, szTmpColor, 20);
		if(iRetLen > 0)
		{
			strCarColor = " AND Plate_No LIKE '%";
			strCarColor += szTmpColor;
			strCarColor += "%' ";
		}

		CString strCarType = "";
		if(pDlg->m_ComBoxCarType.GetCurSel() != (pDlg->m_ComBoxCarType.GetCount() - 1))
		{
			strCarType = "AND Obj_Id=";
			char szObjIndex[20] = {0};
			sprintf(szObjIndex, "%d", pDlg->m_ComBoxCarType.GetCurSel());
			strCarType += szObjIndex;
			strCarType += ' ';
		}
		CString strOverLine = "";
		CButton* pBut = (CButton*)pDlg->GetDlgItem(IDC_CHECK1);
		if(pBut->GetCheck() == 1)
		{
			strOverLine = " AND Over_Line>0 ";
		}

		CString strCrossLine = "";
		pBut = (CButton*)pDlg->GetDlgItem(IDC_CHECK2);
		if(pBut->GetCheck() == 1)
		{
			strCrossLine = " AND Cross_Line>0 ";
		}

		CString strEmergencyStop = "";
		pBut = (CButton*)pDlg->GetDlgItem(IDC_CHECK3);
		if(pBut->GetCheck() == 1)
		{
			strEmergencyStop = " AND Emergency_Stop=1 ";
		}

		CString strReverseRun = "";
		pBut = (CButton*)pDlg->GetDlgItem(IDC_CHECK4);
		if(pBut->GetCheck() == 1)
		{
			strReverseRun = " AND Reverse_Run=1 ";
		}

		CString strSpeeding = "";
		pBut = (CButton*)pDlg->GetDlgItem(IDC_CHECK5);
		if(pBut->GetCheck() == 1)
		{
			strSpeeding = " AND Speeding=1 ";
		}

		CString strSpecial = "";
		pBut = (CButton*)pDlg->GetDlgItem(IDC_CHECK8);
		if(pBut->GetCheck() == 1)
		{
			strSpecial = " AND Special_Result=1 ";
			strOverLine = "";
			strCrossLine = "";
			strEmergencyStop = "";
			strReverseRun = "";
			strSpeeding = "";
		}

		CString strDir = "";
		CString strDirIn;
		pDlg->GetDlgItemText(IDC_COMBODIR1, strDirIn);
		if ("上行" == strDirIn)
		{
			strDir = " AND Road_no<0 ";
		} 
		else if("下行" == strDirIn)
		{
			strDir = " AND Road_no>0 ";
		}


		CString strSQL;
		strSQL.Format("SELECT Pass_Time, Plate_No, big_image_1_path, big_image_2_path FROM %s WHERE Pass_Time BETWEEN '%s' AND '%s'\
AND Location_Id=%d %s%s%s%s%s%s%s%s%s%s\
ORDER BY %s.Pass_Time", strViewName.GetBuffer(), strStartTime.GetBuffer(), strEndTime.GetBuffer(), 
UsedLocaType.iLocaID, strCarColor.GetBuffer(), strCarType.GetBuffer(),
strOverLine.GetBuffer(), strCrossLine.GetBuffer(), strEmergencyStop.GetBuffer(),
strReverseRun.GetBuffer(), strReverseRun.GetBuffer(), strSpeeding.GetBuffer(),
strSpecial.GetBuffer(), strDir.GetBuffer(), strViewName.GetBuffer());
		_variant_t var;
		if(pDlg->m_fIsBreak)
		{
			pDlg->SendMessage(WM_SERCH1_DONE, (WPARAM)10, 0);
			return 10;
		}
		hr = pRecordset->Open(_variant_t(strSQL), pConnection.GetInterfacePtr(),
			adOpenDynamic, adLockOptimistic, adCmdText);
		if(SUCCEEDED(hr))
		{
			pDlg->m_ListResult.DeleteAllItems();
			int iIndex = 0;
			while(!pRecordset->adoEOF)
			{
				var = pRecordset->GetCollect((long)1);
				CString strPlate = (LPCSTR)_bstr_t(var);
				strPlate.Trim();
				pDlg->m_ListResult.InsertItem(iIndex, "", 0);

				char szIndex[30] = {0};
				sprintf(szIndex, "%d", iIndex+1);
				pDlg->m_ListResult.SetItemText(iIndex, 0, szIndex);

				pDlg->m_ListResult.SetItemText(iIndex, 1, strPlate.GetBuffer());

 				var = pRecordset->GetCollect((long)0);
// 				strPlate = (LPCSTR)_bstr_t(var);
// 				strPlate.Trim();

				COleDateTime time = (COleDateTime)(var);
				strPlate = time.Format("%Y-%m-%d %H:%M:%S");

				pDlg->m_ListResult.SetItemText(iIndex, 2, strPlate.GetBuffer());

				pDlg->m_ListResult.SetItemText(iIndex, 3, UsedLocaType.szLocaName);

				var = pRecordset->GetCollect((long)2);
				strPlate = (LPCSTR)_bstr_t(var);
				strPlate.Trim();
				pDlg->m_ListResult.SetItemText(iIndex, 4, strPlate.GetBuffer());

				var = pRecordset->GetCollect((long)3);
				strPlate = (LPCSTR)_bstr_t(var);
				strPlate.Trim();
				pDlg->m_ListResult.SetItemText(iIndex, 5, strPlate.GetBuffer());

				pDlg->m_ListResult.SetItemText(iIndex, 6, UsedLocaType.szServerIP);

				iIndex++;
				pRecordset->MoveNext();
			}
			if(iIndex > 0)
			{
				pDlg->m_iResultType = 1;
			}
		}
		strSql1.Format("IF EXISTS (SELECT TABLE_NAME FROM INFORMATION_SCHEMA.VIEWS WHERE TABLE_NAME = N'%s') DROP VIEW %s",
			strViewName.GetBuffer(), strViewName.GetBuffer());
		pConnection->Execute((_bstr_t)strSql1.GetBuffer(), &RecordsetAffected, adCmdText);
	}
	catch(_com_error e)
	{
		if(pRecordset)
		{
			pRecordset->Cancel();
			pRecordset = NULL;
		}
		if(pConnection)
		{
			pConnection->Cancel();
			pConnection->Close();
			pConnection = NULL;
		}
		::CoUninitialize();
		pDlg->SendMessage(WM_SERCH1_DONE, (WPARAM)14, 0);
		return 14;
	}

	if(pRecordset)
	{
		pRecordset->Close();
		pRecordset = NULL;
	}
	if(pConnection)
	{
		pConnection->Cancel();
		pConnection->Close();
		pConnection = NULL;
	}
	::CoUninitialize();
	pDlg->SendMessage(WM_SERCH1_DONE, (WPARAM)0, 0);
 	return 0;
}

void CVehicleTrackingSystemDlg::OnBnClickedButton1()
{

	if(m_iSelectIndex < 0 || m_iSelectIndex >= m_LocaList.GetCount())
	{
		MessageBox("未选定数据站，查询失败!", "提示", MB_OK|MB_ICONWARNING);
		return;
	}

	CTime cTmpTime;
	DWORD64 dwTmpSecond = 0;
	m_DateStart.GetTime(cTmpTime);
	dwTmpSecond = cTmpTime.GetTime();
	dwTmpSecond = (dwTmpSecond / 60) * 60;
	CTime cStartTime(dwTmpSecond);

	dwTmpSecond = 0;
	m_DateEnd.GetTime(cTmpTime);
	dwTmpSecond = cTmpTime.GetTime();
	dwTmpSecond = (dwTmpSecond / 60) * 60 + 59;
	CTime cEndTime(dwTmpSecond);

	if(cStartTime >= cEndTime)
	{
		MessageBox("时间片区错误，查询失败!", "ERROR", MB_OK|MB_ICONERROR);
		return;
	}

	if(cEndTime - cStartTime > 259200)
	{
		if(MessageBox("查询时间片区跨度超过3天，过长的时间跨度会\n使查询时间过长，查询结果记录数过多\n不便于查阅，并且可能造\
成系统缓存空间不足而崩溃\n或丢失结果,是否确认提交查询?", "警告", MB_YESNO | MB_ICONWARNING) == IDNO)
		{
			return;
		}
	}

	m_DateStart.SetTime(&cStartTime);
	m_DateEnd.SetTime(&cEndTime);
	char szCurrentPath[MAX_PATH] = {0};
	GetModuleFileName(NULL, szCurrentPath, MAX_PATH);
	PathRemoveFileSpec(szCurrentPath);
	CString strPath = szCurrentPath;
	strPath += "\\TmpResultTmp\\";
	if(PathFileExists(strPath.GetBuffer()))
	{
		ClearTmpResultPic(strPath.GetBuffer());
	}

	GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);
	m_fIsBreak = FALSE;
	m_hSearchPassCarThread = CreateThread(NULL, 0, SearchPassCarThread, this, 0, NULL);
}

void CVehicleTrackingSystemDlg::ClearSearchClass()
{
	EnterCriticalSection(&m_csSearchClassList);
	while(m_SearchClassList.GetCount() > 0)
	{
		CSearchCarPassLocaType* pTmpSearch = m_SearchClassList.RemoveHead();
		delete pTmpSearch;
		pTmpSearch = NULL;
	}
	LeaveCriticalSection(&m_csSearchClassList);
}

void CVehicleTrackingSystemDlg::OnBnClickedButton3()
{
	GetDlgItem(IDC_BUTTON3)->EnableWindow(FALSE);
	m_ListResult.EnableWindow(FALSE);
	char szPlate[30] = {0};
	int iLen = GetDlgItemText(IDC_EDIT3, szPlate, 30);
	if(iLen <= 0)
	{
		MessageBox("车牌号码不能为空", "ERROR", MB_OK|MB_ICONERROR);
		GetDlgItem(IDC_BUTTON3)->EnableWindow(TRUE);
		m_ListResult.EnableWindow(TRUE);
		return;
	}
	if(strstr(szPlate, "无车牌"))
	{
		MessageBox("程序无法对“无车牌”进行运行轨迹跟踪查询操\n作，请选择确定的车牌再进行此操作！", "WARNING", MB_OK|MB_ICONWARNING);
		GetDlgItem(IDC_BUTTON3)->EnableWindow(TRUE);
		m_ListResult.EnableWindow(TRUE);
		return;
	}
	CString strViewName = "VTHVE_DATA_VIEW_";
	strViewName += g_strMacAddr;
	CString strSQL;
	strSQL.Format("SELECT Location_Id, Pass_Time, Plate_No, big_image_1_path, big_image_2_path FROM %s WHERE Plate_No='",
		strViewName.GetBuffer());
	strSQL += szPlate;
	strSQL += "'";

	CString strDir = "";
	CString strDirIn;
	GetDlgItemText(IDC_COMBODIR2, strDirIn);
	if ("上行" == strDirIn)
	{
		strDir = " AND Road_no<0 ";
	} 
	else if("下行" == strDirIn)
	{
		strDir = " AND Road_no>0 ";
	}
	strSQL += strDir;

	
	BOOL fIsSetTime = FALSE;
	CTime cTmpStartTime = CTime::GetCurrentTime();
	CTime cTmpEndTime = CTime::GetCurrentTime();
 	CButton* pBut = (CButton*)GetDlgItem(IDC_CHECK6);
// 	if(pBut->GetCheck() == 1)

	// enable time query ,modify by zhut 20121116
	{
		CTime cTmpTime;
		DWORD64 dwTmpSecond = 0;
		m_SearchCarStartTime.GetTime(cTmpTime);
		dwTmpSecond = cTmpTime.GetTime();
		dwTmpSecond = (dwTmpSecond / 60) * 60;
		CTime cStartTime(dwTmpSecond);

		dwTmpSecond = 0;
		m_SearchCarEndTime.GetTime(cTmpTime);
		dwTmpSecond = cTmpTime.GetTime();
		dwTmpSecond = (dwTmpSecond / 60) * 60 + 59;
		CTime cEndTime(dwTmpSecond);
		if(cStartTime >= cEndTime)
		{
			if(MessageBox("时间片错误,是否确认查询", "警告", MB_YESNO) == IDNO)
			{
				GetDlgItem(IDC_BUTTON3)->EnableWindow(TRUE);
				m_ListResult.EnableWindow(TRUE);
				return;
			}
		}
		CString strUsedTime;
		strUsedTime.Format(" AND Pass_Time BETWEEN '%s' AND '%s'", 
			cStartTime.Format("%Y-%m-%d %H:%M:00"), cEndTime.Format("%Y-%m-%d %H:%M:59"));
		strSQL += strUsedTime;
		fIsSetTime = TRUE;
		cTmpStartTime = cStartTime;
		cTmpEndTime = cEndTime;
	}
// 	else		// add by zhut, 20121107
// 	{
// 		CTime cEndTime = CTime::GetCurrentTime();
// 		DWORD64 dwStartSecond = cEndTime.GetTime();
// 		dwStartSecond -= 60*60*24;
// 		CTime cStartTime(dwStartSecond);
// 
// 		CString strUsedTime;
// 		strUsedTime.Format(" AND Pass_Time BETWEEN '%s' AND '%s'", 
// 			cStartTime.Format("%Y-%m-%d %H:%M:00"), cEndTime.Format("%Y-%m-%d %H:%M:00"));
// 		strSQL += strUsedTime;
// 		fIsSetTime = TRUE;
// 		cTmpStartTime = cStartTime;
// 		cTmpEndTime = cEndTime;
// 	}

	ClearSearchClass();

	char szCurrentPath[MAX_PATH] = {0};
	GetModuleFileName(NULL, szCurrentPath, MAX_PATH);
	PathRemoveFileSpec(szCurrentPath);
	CString strPath = szCurrentPath;
	strPath += "\\TmpFinalResult\\";
	if(PathFileExists(strPath.GetBuffer()))
	{
		ClearTmpResultPic(strPath.GetBuffer());
	}

	g_pShowLoading->Start();
	g_pShowLoading->SetInfoWord("查询中，请稍候...");
	EnableWindow(FALSE);
	pBut = (CButton*)GetDlgItem(IDC_CHECK7);
	if(pBut->GetCheck() == 1)
	{
		int iSeverCount = m_SelectServer.GetCount();
		if(iSeverCount <= 0)
		{
			if(MessageBox("未指定任何服务器，是否使用默认设置搜索?", "警告", MB_YESNO) == IDNO)
			{
				GetDlgItem(IDC_BUTTON3)->EnableWindow(TRUE);
				m_ListResult.EnableWindow(TRUE);
				g_pShowLoading->Stop();
				EnableWindow(TRUE);
				SendMessage(WM_PAINT, 0, 0);
				return;
			}
			EnterCriticalSection(&m_csServerList);
			POSITION pos = m_ServerList.GetHeadPosition();
			while(pos)
			{
				NewSeverType* pTmpServer = m_ServerList.GetNext(pos);
				CSearchCarPassLocaType* pTmpSearchClass = new CSearchCarPassLocaType(pTmpServer, strSQL.GetBuffer(), FALSE, cTmpStartTime, cTmpEndTime);
				pTmpSearchClass->Start();
				EnterCriticalSection(&m_csSearchClassList);
				m_SearchClassList.AddTail(pTmpSearchClass);
				LeaveCriticalSection(&m_csSearchClassList);
			}
			LeaveCriticalSection(&m_csServerList);
		}
		else
		{
			EnterCriticalSection(&m_csServerList);
			for(int iIndex=0; iIndex<iSeverCount; iIndex++)
			{
				int iServerIndex = (int)m_SelectServer.GetItemData(iIndex);
				POSITION pos = m_ServerList.FindIndex(iServerIndex);
				if(pos)
				{
					NewSeverType* pTmpServer = m_ServerList.GetAt(pos);
					CSearchCarPassLocaType* pTmpSearchClass = new CSearchCarPassLocaType(pTmpServer, strSQL.GetBuffer(), fIsSetTime, cTmpStartTime, cTmpEndTime);
					pTmpSearchClass->Start();
					EnterCriticalSection(&m_csSearchClassList);
					m_SearchClassList.AddTail(pTmpSearchClass);
					LeaveCriticalSection(&m_csSearchClassList);
				}
			}
			LeaveCriticalSection(&m_csServerList);
		}
	}
	else
	{
		EnterCriticalSection(&m_csServerList);
		POSITION pos = m_ServerList.GetHeadPosition();
		while(pos)
		{
			NewSeverType* pTmpServer = m_ServerList.GetNext(pos);
			CSearchCarPassLocaType* pTmpSearchClass = new CSearchCarPassLocaType(pTmpServer, strSQL.GetBuffer(), fIsSetTime, cTmpStartTime, cTmpEndTime);
			pTmpSearchClass->Start();
			EnterCriticalSection(&m_csSearchClassList);
			m_SearchClassList.AddTail(pTmpSearchClass);
			LeaveCriticalSection(&m_csSearchClassList);
		}
		LeaveCriticalSection(&m_csServerList);
	}
	SetTimer(10086, 1000, NULL);
}

void CVehicleTrackingSystemDlg::OnEditRoadLine()
{
	CEditRoadLineDlg EditRoadLine(this);
	EditRoadLine.DoModal();
}

void CVehicleTrackingSystemDlg::ClearTmpResultPic(char* sDirName)
{
	CFileFind tempFind; 
	char sTempFileFind[256] ;

	sprintf(sTempFileFind,"%s\\*.*",sDirName); 
	BOOL IsFinded = tempFind.FindFile(sTempFileFind); 
	while (IsFinded) 
	{ 
		IsFinded = tempFind.FindNextFile(); 

		if (!tempFind.IsDots()) 
		{ 
			char sFoundFileName[256]; 
			strcpy(sFoundFileName,tempFind.GetFileName().GetBuffer(200)); 

			if (tempFind.IsDirectory()) 
			{ 
				char sTempDir[256]; 
				sprintf(sTempDir,"%s\\%s",sDirName,sFoundFileName); 
				ClearTmpResultPic(sTempDir); 
			} 
			else 
			{ 
				char sTempFileName[256]; 
				sprintf(sTempFileName,"%s\\%s",sDirName,sFoundFileName); 
				DeleteFile(sTempFileName); 
			} 
		} 
	} 
}

void CVehicleTrackingSystemDlg::LoadFinalResultPic(FinalResultType* pTmpFinalResult)
{
	char szCurrentPath[MAX_PATH] = {0};
	GetModuleFileName(NULL, szCurrentPath, MAX_PATH);
	PathRemoveFileSpec(szCurrentPath);
	CString strSavePath = szCurrentPath;
	strSavePath += "\\TmpFinalResult\\";
	if(!PathFileExists(strSavePath.GetBuffer()))
	{
		if(CreateDirectory(strSavePath.GetBuffer(), NULL) == TRUE)
		{
			SetFileAttributes(strSavePath.GetBuffer(), FILE_ATTRIBUTE_HIDDEN);
		}
	}
	if(pTmpFinalResult == NULL) return;

	char szIP[30] = {0};
	memcpy(szIP, pTmpFinalResult->szServerIP, strlen(pTmpFinalResult->szServerIP));
	char szFileName[256] = {0};
	memcpy(szFileName, pTmpFinalResult->szRPicPath, strlen(pTmpFinalResult->szRPicPath));
	CString strUrl;
	strUrl.Format("\\\\%s\\", szIP);
	strUrl += szFileName[0];
	strUrl += szFileName+2;
	CString strFileName = szFileName;
	int iIndex = strFileName.ReverseFind('\\');
	strFileName = strFileName.Mid(iIndex+1, strFileName.GetLength()-iIndex);
	CString strSaveName = strSavePath;
	strSaveName += strFileName;
	HRESULT hr = URLDownloadToFile(NULL, _T(strUrl.GetBuffer()),
		_T(strSaveName.GetBuffer()), 0, NULL);

	memset(szFileName, 0, 256);
	strUrl.Format("\\\\%s\\", szIP);
	memcpy(szFileName, pTmpFinalResult->szCPicPath, strlen(pTmpFinalResult->szCPicPath));
	strUrl += szFileName[0];
	strUrl += szFileName + 2;
	strFileName = szFileName;
	iIndex = strFileName.ReverseFind('\\');
	strFileName = strFileName.Mid(iIndex+1, strFileName.GetLength()-iIndex);
	strSaveName = strSavePath;
	strSaveName += strFileName;
	hr = URLDownloadToFile(NULL, _T(strUrl.GetBuffer()),
		_T(strSaveName.GetBuffer()), 0, NULL);
}

void CVehicleTrackingSystemDlg::LoadResultPic(int iIndex)
{
	EnableWindow(FALSE);
	char szCurrentPath[MAX_PATH] = {0};
	GetModuleFileName(NULL, szCurrentPath, MAX_PATH);
	PathRemoveFileSpec(szCurrentPath);
	CString strSavePath = szCurrentPath;
	strSavePath += "\\TmpResultTmp\\";
	if(!PathFileExists(strSavePath.GetBuffer()))
	{
		if(CreateDirectory(strSavePath.GetBuffer(), NULL) == TRUE)
		{
			SetFileAttributes(strSavePath.GetBuffer(), FILE_ATTRIBUTE_HIDDEN);
		}
	}
	if(m_iResultType == 1)
	{
		if(iIndex < 0 || iIndex >= m_ListResult.GetItemCount()) return;

		char szIP[30] = {0};
		if(m_ListResult.GetItemText(iIndex, 6, szIP, 30) <= 0)
		{
			EnableWindow(TRUE);
			return;
		}

		NETRESOURCE cNetRes;
		memset(&cNetRes, 0, sizeof(cNetRes));
		CString strServerIP;
		strServerIP.Format("\\\\%s", szIP);
		cNetRes.lpRemoteName = strServerIP.GetBuffer(30);		
		DWORD dwRet = WNetAddConnection2(&cNetRes,  m_strServerPassword, m_strServerName,0); 
		strServerIP.ReleaseBuffer();

		char szFileName[256] = {0};
		m_ListResult.GetItemText(iIndex, 4, szFileName, 256);
		CString strUrl;
		strUrl.Format("\\\\%s\\", szIP);
		strUrl += szFileName[0];
		strUrl += szFileName+2;
		CString strFileName = szFileName;
		int iIndexChar = strFileName.ReverseFind('\\');
		strFileName = strFileName.Mid(iIndexChar+1, strFileName.GetLength()-iIndexChar);
		CString strSaveName = strSavePath;
		strSaveName += strFileName;

		HRESULT hr = URLDownloadToFile(NULL, _T(strUrl.GetBuffer()),
			_T(strSaveName.GetBuffer()), 0, NULL);
		if(SUCCEEDED(hr))
		{
			m_strRPicPath = strSaveName;
		}
		else
		{
			m_strRPicPath = "";
		}
		
		memset(szFileName, 0, 256);
		strUrl.Format("\\\\%s\\", szIP);
		m_ListResult.GetItemText(iIndex, 5, szFileName, 256);
		strUrl += szFileName[0];
		strUrl += szFileName + 2;
		strFileName = szFileName;
		iIndexChar = strFileName.ReverseFind('\\');
		strFileName = strFileName.Mid(iIndexChar+1, strFileName.GetLength()-iIndexChar);
		strSaveName = strSavePath;
		strSaveName += strFileName;
		hr = URLDownloadToFile(NULL, _T(strUrl.GetBuffer()),
			_T(strSaveName.GetBuffer()), 0, NULL);
		if(SUCCEEDED(hr))
		{
			m_strCPicPath = strSaveName;
		}
		else
		{
			m_strCPicPath = "";
		}
	}
	EnableWindow(TRUE);
}

void CVehicleTrackingSystemDlg::UpDatePicBuffer()
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
			return;
		}

		OLE_XSIZE_HIMETRIC hmWidth;
		OLE_YSIZE_HIMETRIC hmHeight;
		pPic->get_Width(&hmWidth);
		pPic->get_Height(&hmHeight);
		int iWidth = MulDiv(hmWidth, GetDeviceCaps(hTmpDC, LOGPIXELSX), 2540);
		int iHeight = MulDiv(hmHeight, GetDeviceCaps(hTmpDC, LOGPIXELSY), 2540);
		::ReleaseDC(GetDlgItem(IDC_STATIC_R_PIC)->GetSafeHwnd(), hTmpDC);

		HDC hTmpDC1;
		m_lpRPic->GetDC(&hTmpDC1);
		pPic->Render(hTmpDC1, 0, 0, 315, 220, 0, hmHeight, hmWidth, -hmHeight, NULL);
		m_lpRPic->ReleaseDC(hTmpDC1);
		pPic->Release();
		pStm->Release();
		GlobalFree(hGlobal);
	}
	else
	{
		HDC hTmpDC1;
		m_lpRPic->GetDC(&hTmpDC1);
		SelectObject(hTmpDC1, g_hFontMin);
		DrawRectangle(hTmpDC1, 0, 0, 315, 220, RGB(250, 250, 250), RGB(250, 250, 250));
		CString strFileName = m_strRPicPath;
		int iIndexChar = strFileName.ReverseFind('\\');
		strFileName = strFileName.Mid(iIndexChar+1, strFileName.GetLength()-iIndexChar);
		DrawTextThis(hTmpDC1, 10, 10, "识别图:", 7, RGB(32, 32, 32));
		iIndexChar = strFileName.GetLength();
		if(iIndexChar > 50)
		{
			iIndexChar = 50;
		}
		DrawTextThis(hTmpDC1, 10, 30, strFileName.GetBuffer(), iIndexChar, RGB(255, 0, 0));
		DrawTextThis(hTmpDC1, 10, 50, "获取失败!", 9, RGB(32, 32, 32));
		m_lpRPic->ReleaseDC(hTmpDC1);
	}
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

		HDC hTmpDC = ::GetDC(GetDlgItem(IDC_STATIC_C_PIC)->GetSafeHwnd());
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
			return;
		}

		OLE_XSIZE_HIMETRIC hmWidth;
		OLE_YSIZE_HIMETRIC hmHeight;
		pPic->get_Width(&hmWidth);
		pPic->get_Height(&hmHeight);
		int iWidth = MulDiv(hmWidth, GetDeviceCaps(hTmpDC, LOGPIXELSX), 2540);
		int iHeight = MulDiv(hmHeight, GetDeviceCaps(hTmpDC, LOGPIXELSY), 2540);
		::ReleaseDC(GetDlgItem(IDC_STATIC_C_PIC)->GetSafeHwnd(), hTmpDC);

		HDC hTmpDC1;
		m_lpCPic->GetDC(&hTmpDC1);
		pPic->Render(hTmpDC1, 0, 0, 315, 220, 0, hmHeight, hmWidth, -hmHeight, NULL);
		m_lpCPic->ReleaseDC(hTmpDC1);
		pPic->Release();
		pStm->Release();
		GlobalFree(hGlobal);
	}
	else
	{
		HDC hTmpDC1;
		m_lpCPic->GetDC(&hTmpDC1);
		SelectObject(hTmpDC1, g_hFontMin);
		DrawRectangle(hTmpDC1, 0, 0, 315, 220, RGB(250, 250, 250), RGB(250, 250, 250));
		CString strFileName = m_strCPicPath;
		int iIndexChar = strFileName.ReverseFind('\\');
		strFileName = strFileName.Mid(iIndexChar+1, strFileName.GetLength()-iIndexChar);
		DrawTextThis(hTmpDC1, 10, 10, "抓拍图:", 7, RGB(32, 32, 32));
		iIndexChar = strFileName.GetLength();
		if(iIndexChar > 50)
		{
			iIndexChar = 50;
		}
		DrawTextThis(hTmpDC1, 10, 30, strFileName.GetBuffer(), iIndexChar, RGB(255, 0, 0));
		DrawTextThis(hTmpDC1, 10, 50, "获取失败!", 9, RGB(32, 32, 32));
		m_lpCPic->ReleaseDC(hTmpDC1);
	}
	SendMessage(WM_PAINT, 0, 0);
}

void CVehicleTrackingSystemDlg::UpDateFinalResultPic()
{
	if(m_iResultType != 2)
	{
		return;
	}
	EnterCriticalSection(&m_csFinalResultList);
	POSITION posFinalResult = m_FinalResultList.FindIndex(m_iSelectResultIndex);
	if(posFinalResult)
	{
		FinalResultType* pTmpFinalResult = m_FinalResultList.GetAt(posFinalResult);
		char szCurrentPath[MAX_PATH] = {0};
		GetModuleFileName(NULL, szCurrentPath, MAX_PATH);
		PathRemoveFileSpec(szCurrentPath);
		CString strSavePath = szCurrentPath;
		strSavePath += "\\TmpFinalResult\\";

		CString strFileName = pTmpFinalResult->szRPicPath;
		int iIndex = strFileName.ReverseFind('\\');
		strFileName = strFileName.Mid(iIndex+1, strFileName.GetLength()-iIndex);
		CString strSaveName = strSavePath;
		strSaveName += strFileName;
		m_strRPicPath = strSaveName;

		strFileName = pTmpFinalResult->szCPicPath;
		iIndex = strFileName.ReverseFind('\\');
		strFileName = strFileName.Mid(iIndex+1, strFileName.GetLength()-iIndex);
		strSaveName = strSavePath;
		strSaveName += strFileName;
		m_strCPicPath = strSaveName;
	}
	LeaveCriticalSection(&m_csFinalResultList);
}

void CVehicleTrackingSystemDlg::OnNMDblclkList4(NMHDR *pNMHDR, LRESULT *pResult)
{
	//if(m_iResultType != 1 && m_iResultType != 2)
	//{
	//	return;
	//}
	//
	//int iSelectIndex = m_ListResult.GetSelectionMark();
	//if(iSelectIndex < 0 || iSelectIndex >= m_ListResult.GetItemCount())
	//{
	//	return;
	//}

	//if(m_iResultType == 1)
	//{
	//	char szPlateNo[20] = {0};
	//	m_ListResult.GetItemText(iSelectIndex, 1, szPlateNo, 20);
	//	SetDlgItemText(IDC_EDIT3, szPlateNo);
	//	LoadResultPic(iSelectIndex);
	//	UpDatePicBuffer();
	//	GetDlgItem(IDC_BUTTON3)->EnableWindow(TRUE);
	//	GetDlgItem(IDC_CHECK7)->EnableWindow(TRUE);
	//}
	//else
	//{
	//	m_iSelectResultIndex = iSelectIndex;
	//	UpDateFinalResultPic();
	//	UpDatePicBuffer();
	//	SendMessage(WM_PAINT, 0, 0);
	//}

	*pResult = 0;
}

void CVehicleTrackingSystemDlg::OnBnClickedCheck7()
{
	CButton* pBut = (CButton*)GetDlgItem(IDC_CHECK7);
	if(pBut->GetCheck() == 1)
	{
		EnterCriticalSection(&m_csServerList);
		if(m_ServerList.GetCount() > 0)
		{
			POSITION pos = m_ServerList.GetHeadPosition();
			while(pos)
			{
				NewSeverType* pTmpServer = m_ServerList.GetNext(pos);
				m_ComBoxServer.AddString(pTmpServer->szLocaName);
			}
			m_ComBoxServer.SetCurSel(0);
			m_ComBoxServer.EnableWindow(TRUE);
			m_SelectServer.EnableWindow(TRUE);
			GetDlgItem(IDC_BUTTON4)->EnableWindow(TRUE);
		}
		LeaveCriticalSection(&m_csServerList);
	}
	else
	{
		m_ComBoxServer.ResetContent();
		m_SelectServer.ResetContent();
		m_SelectServer.EnableWindow(FALSE);
		m_ComBoxServer.EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON4)->EnableWindow(FALSE);
	}
}

void CVehicleTrackingSystemDlg::OnBnClickedButton4()
{
	int iAddIndex = m_ComBoxServer.GetCurSel();
	int iAddedCount = m_SelectServer.GetCount();
	for(int i=0; i<iAddedCount; i++)
	{
		int iTmpIndex = (int)m_SelectServer.GetItemData(i);
		if(iAddIndex == iTmpIndex)
		{
			MessageBox("已添加到列表中");
			return;
		}
	}
	char szTmpName[256] = {0};
	m_ComBoxServer.GetWindowText(szTmpName, 256);
	HDC hTmpDC;
	SIZE s;
	hTmpDC = ::GetDC(m_SelectServer.GetSafeHwnd());
	GetTextExtentPoint32(hTmpDC, szTmpName, (int)strlen(szTmpName), &s);
	::ReleaseDC(m_SelectServer.GetSafeHwnd(), hTmpDC);
	if(s.cx > m_SelectServer.GetHorizontalExtent())
	{
		m_SelectServer.SetHorizontalExtent(s.cx);
	}
	m_SelectServer.AddString(szTmpName);
	m_SelectServer.SetItemData(iAddedCount, iAddIndex);
}

void CVehicleTrackingSystemDlg::OnBnClickedCheck6()
{
	CButton* pBut = (CButton*)GetDlgItem(IDC_CHECK6);
	if(pBut->GetCheck() == 1)
	{
		CTime cEndTime = CTime::GetCurrentTime();
		DWORD64 dw64Time = cEndTime.GetTime();
		dw64Time -= 3600;
		CTime cStartTime(dw64Time);
		m_SearchCarStartTime.SetTime(&cStartTime);
		m_SearchCarEndTime.SetTime(&cEndTime);
		m_SearchCarStartTime.EnableWindow(TRUE);
		m_SearchCarEndTime.EnableWindow(TRUE);
	}
	else
	{
// 		m_SearchCarStartTime.EnableWindow(FALSE);
// 		m_SearchCarEndTime.EnableWindow(FALSE);
	}
}

void CVehicleTrackingSystemDlg::OnTimer(UINT nIDEvent)
{
	if(nIDEvent == 10086)
	{
		EnterCriticalSection(&m_csSearchClassList);
		POSITION pos = m_SearchClassList.GetHeadPosition();
		while(pos)
		{
			CSearchCarPassLocaType* pTmpSearchClass = m_SearchClassList.GetNext(pos);
			int iStatus = pTmpSearchClass->GetStatus();
			if(iStatus != 10086 && iStatus != 20086)
			{
				LeaveCriticalSection(&m_csSearchClassList);
				CDialog::OnTimer(nIDEvent);
				return;
			}
		}
		LeaveCriticalSection(&m_csSearchClassList);
		KillTimer(10086);
		SendMessage(WM_PROC_SEARCH_DONE, 0, 0);
	}
	else if(nIDEvent == 6666)
	{
		char szTmpInfo[60] = {0};
		GetDlgItemText(IDC_EDIT3, szTmpInfo, 60);
		if(strlen(szTmpInfo) > 0)
		{
			GetDlgItem(IDC_BUTTON3)->EnableWindow(TRUE);
		}
		else
		{
			GetDlgItem(IDC_BUTTON3)->EnableWindow(FALSE);
		}
	}
	CDialog::OnTimer(nIDEvent);
}

void CVehicleTrackingSystemDlg::CleanFinalResult()
{
	EnterCriticalSection(&m_csFinalResultList);
	while(m_FinalResultList.GetCount() > 0)
	{
		FinalResultType* pTmpFinalResult = m_FinalResultList.RemoveHead();
		delete pTmpFinalResult;
		pTmpFinalResult = NULL;
	}
	LeaveCriticalSection(&m_csFinalResultList);
}

int CVehicleTrackingSystemDlg::GetResultLoca(ResultInfoType* pResultInfo)
{
	EnterCriticalSection(&m_csLocaList);
	POSITION posLoca  = m_LocaList.GetHeadPosition();
	int iIndex = 0;
	while(posLoca)
	{
		LocaType* pTmpLoca = m_LocaList.GetNext(posLoca);
		if(pResultInfo->iLocaIndex == pTmpLoca->iLocaID
			&& strcmp(pTmpLoca->szServerIP, pResultInfo->szServerIP) == 0
			&& strcmp(pTmpLoca->szDataBaseName, pResultInfo->szDataBaseName) == 0)
		{
			LeaveCriticalSection(&m_csLocaList);
			return iIndex;
		}
		iIndex++;
	}
	LeaveCriticalSection(&m_csLocaList);
	return -1;
}

void CVehicleTrackingSystemDlg::SortFinalResult()
{
	CList<FinalResultType*> pTmpFinalList;
	EnterCriticalSection(&m_csFinalResultList);
	DWORD64 dwLastBigTime = 0;
	int iLastIndex = 0;
	int iIndex = 0;
	while(m_FinalResultList.GetCount() > 0)
	{

		dwLastBigTime = 0;
		iLastIndex = 0;
		iIndex = 0;
		POSITION posFinalResult = m_FinalResultList.GetHeadPosition();
		while(posFinalResult)
		{
			FinalResultType* pTmpFinalResult = m_FinalResultList.GetNext(posFinalResult);
			int iY, iM, iD, iH, iMi, iS;
			char a, b;
			sscanf(pTmpFinalResult->szPassTime, "%d%c%d%c%d %d:%d:%d", &iY, &a, &iM, &b,
				&iD, &iH, &iMi, &iS);
			CTime cTime(iY, iM, iD, iH, iMi, iS);
			DWORD64 dwTmpTime = cTime.GetTime();
			if(dwTmpTime >= dwLastBigTime)
			{
				dwLastBigTime = dwTmpTime;
				iLastIndex = iIndex;
			}
			iIndex++;
		}
		posFinalResult = m_FinalResultList.FindIndex(iLastIndex);
		FinalResultType* pTmpFinalResult = m_FinalResultList.GetAt(posFinalResult);
		pTmpFinalList.AddTail(pTmpFinalResult);
		m_FinalResultList.RemoveAt(posFinalResult);
	}

	while(pTmpFinalList.GetCount() > 0)
	{
		FinalResultType* pTmpFinalResult = pTmpFinalList.RemoveTail();
		m_FinalResultList.AddTail(pTmpFinalResult);
	}

	LeaveCriticalSection(&m_csFinalResultList);
}

void CVehicleTrackingSystemDlg::UpDataFinalResultList()
{
	m_ListResult.DeleteAllItems();
	int iIndex = 0;
	EnterCriticalSection(&m_csFinalResultList);
	POSITION posFinalResult = m_FinalResultList.GetHeadPosition();
	while(posFinalResult)
	{
		FinalResultType* pTmpFinalResult = m_FinalResultList.GetNext(posFinalResult);
		m_ListResult.InsertItem(iIndex, "", 0);
		char szIndex[30] = {0};
		sprintf(szIndex, "%d", iIndex+1);
		m_ListResult.SetItemText(iIndex, 0, szIndex);
		m_ListResult.SetItemText(iIndex, 1, pTmpFinalResult->szPlate);
		m_ListResult.SetItemText(iIndex, 2, pTmpFinalResult->szPassTime);
		m_ListResult.SetItemText(iIndex, 3, pTmpFinalResult->szLocaName);
		iIndex++;
	}
	LeaveCriticalSection(&m_csFinalResultList);
	m_iResultType = 2;
}

void CVehicleTrackingSystemDlg::ProceSearchDone()
{
	BOOL fIsPlate = TRUE;
	CleanFinalResult();
	EnterCriticalSection(&m_csSearchClassList);
	POSITION posSearchClass = m_SearchClassList.GetHeadPosition();
	while(posSearchClass)
	{
		CSearchCarPassLocaType *pTmpSearchCarPassLocaType = m_SearchClassList.GetNext(posSearchClass);
		EnterCriticalSection(&pTmpSearchCarPassLocaType->m_csResultList);
		POSITION posResult = pTmpSearchCarPassLocaType->m_ResultList.GetHeadPosition();
		while(posResult)
		{
			ResultInfoType* pTmpResult = pTmpSearchCarPassLocaType->m_ResultList.GetNext(posResult);
			FinalResultType* pNewFinalResult = new FinalResultType();
			memcpy(pNewFinalResult->szPlate, pTmpResult->szPlate, strlen(pTmpResult->szPlate));
			memcpy(pNewFinalResult->szPassTime, pTmpResult->szPassTime, strlen(pTmpResult->szPassTime));
			memcpy(pNewFinalResult->szRPicPath, pTmpResult->szRPicPath, strlen(pTmpResult->szRPicPath));
			memcpy(pNewFinalResult->szCPicPath, pTmpResult->szCPicPath, strlen(pTmpResult->szCPicPath));
			memcpy(pNewFinalResult->szServerIP, pTmpResult->szServerIP, strlen(pTmpResult->szServerIP));
			int iIndex = GetResultLoca(pTmpResult);
			if(iIndex != -1)
			{
				EnterCriticalSection(&m_csLocaList);
				POSITION posTmpLoca = m_LocaList.FindIndex(iIndex);
				if(posTmpLoca)
				{
					LocaType* pTmpLoca = m_LocaList.GetAt(posTmpLoca);
					pNewFinalResult->Loca.x = pTmpLoca->iPositionX;
					pNewFinalResult->Loca.y = pTmpLoca->iPositionY;
					memcpy(pNewFinalResult->szLocaName, pTmpLoca->szLocaName, strlen(pTmpLoca->szLocaName));
				}
				LeaveCriticalSection(&m_csLocaList);
			}
			if(strstr(pNewFinalResult->szPlate, "无车牌")) 
			fIsPlate = FALSE;
			EnterCriticalSection(&m_csFinalResultList);
			m_FinalResultList.AddTail(pNewFinalResult);
			LeaveCriticalSection(&m_csFinalResultList);
		}
		LeaveCriticalSection(&pTmpSearchCarPassLocaType->m_csResultList);
	}
	LeaveCriticalSection(&m_csSearchClassList);
	m_iResultType = 2;
	if(fIsPlate)
	SortFinalResult();
	UpDataFinalResultList();
	g_pShowLoading->SetInfoWord("正在获取图片...");
	m_fIsLoadPicBreak = FALSE;
	m_hLoadResultPic = CreateThread(NULL, 0, LoadResultPicThread, this, 0, NULL);
}

void CVehicleTrackingSystemDlg::OnViewResult()
{
	CResultViewDlg pDlg(this);
	pDlg.DoModal();
}

void CVehicleTrackingSystemDlg::OnNMRclickList4(NMHDR *pNMHDR, LRESULT *pResult)
{
	if(m_iResultType != 2)
	{
		return;
	}
	int iSelectIndex = m_ListResult.GetSelectionMark();
	if(iSelectIndex < 0 || iSelectIndex >= m_ListResult.GetItemCount())
	{
		return;
	}

	m_iTmpSelectIndex = iSelectIndex;
	CMenu cMecu;
	cMecu.CreatePopupMenu();
	cMecu.AppendMenu(MF_BYCOMMAND | MF_STRING, ID_SETSTART, "设置为起始结果");
	cMecu.AppendMenu(MF_BYCOMMAND | MF_STRING, ID_SETEND, "设置为结束结果");
	cMecu.AppendMenu(MF_BYCOMMAND | MF_STRING, ID_UNSETSTART, "取消起始结果设置");
	cMecu.AppendMenu(MF_BYCOMMAND | MF_STRING, ID_UNSETEND, "取消结束结果设置");
	CPoint cTmpPoint;
	GetCursorPos(&cTmpPoint);
	cMecu.TrackPopupMenu(TPM_LEFTALIGN, cTmpPoint.x, cTmpPoint.y, this);

	*pResult = 0;
}

void CVehicleTrackingSystemDlg::OnSetStart()
{
	if(m_iTmpSelectIndex > m_ListResult.GetItemCount()) return;
	if(m_iTmpSelectIndex == m_iStartResultIndex) return;
	m_iStartResultIndex = m_iTmpSelectIndex;
}

void CVehicleTrackingSystemDlg::OnSetEnd()
{
	if(m_iTmpSelectIndex <= 0) return;
	if(m_iTmpSelectIndex == m_iEndResultIndex) return;
	if(m_iStartResultIndex >= 0 && m_iTmpSelectIndex <= m_iStartResultIndex) return;
	m_iEndResultIndex = m_iTmpSelectIndex;
}

void CVehicleTrackingSystemDlg::OnUnSetStart()
{
	m_iStartResultIndex = -1;
}

void CVehicleTrackingSystemDlg::OnUnSetEnd()
{
	m_iEndResultIndex = -1;
}

DWORD WINAPI CVehicleTrackingSystemDlg::LoadResultPicThread(LPVOID lParam)
{
	if(lParam == NULL)
	{
		return 0xFFFFFFFF;
	}
	CVehicleTrackingSystemDlg* pDlg = (CVehicleTrackingSystemDlg*)lParam;
	EnterCriticalSection(&pDlg->m_csFinalResultList);
	int iResultCount = (int)pDlg->m_FinalResultList.GetCount();
	POSITION posFinalResult = pDlg->m_FinalResultList.GetHeadPosition();
	FinalResultType* pFirstResult = NULL;
	while(posFinalResult)
	{
		FinalResultType* pTmpFinalResult = pDlg->m_FinalResultList.GetNext(posFinalResult);
		if(pFirstResult == NULL) pFirstResult = pTmpFinalResult;
		pDlg->LoadFinalResultPic(pTmpFinalResult);
	}

	if(pFirstResult == NULL)
	{
		pDlg->m_strRPicPath = "";
		pDlg->m_strCPicPath = "";
		pDlg->MessageBox("搜索不到任何记录", "提示", MB_OK|MB_ICONWARNING);
	}
	else
	{
		char szCurrentPath[MAX_PATH] = {0};
		GetModuleFileName(NULL, szCurrentPath, MAX_PATH);
		PathRemoveFileSpec(szCurrentPath);
		CString strSavePath = szCurrentPath;
		strSavePath += "\\TmpFinalResult\\";

		CString strFileName = pFirstResult->szRPicPath;
		int iIndex = strFileName.ReverseFind('\\');
		strFileName = strFileName.Mid(iIndex+1, strFileName.GetLength()-iIndex);
		CString strSaveName = strSavePath;
		strSaveName += strFileName;
		pDlg->m_strRPicPath = strSaveName;

		strFileName = pFirstResult->szCPicPath;
		iIndex = strFileName.ReverseFind('\\');
		strFileName = strFileName.Mid(iIndex+1, strFileName.GetLength()-iIndex);
		strSaveName = strSavePath;
		strSaveName += strFileName;
		pDlg->m_strCPicPath = strSaveName;
	}

	LeaveCriticalSection(&pDlg->m_csFinalResultList);
	pDlg->GetDlgItem(IDC_BUTTON3)->EnableWindow(TRUE);
	pDlg->m_ListResult.EnableWindow(TRUE);
	pDlg->UpDatePicBuffer();
	g_pShowLoading->Stop();
	pDlg->EnableWindow(TRUE);
	pDlg->SendMessage(WM_PAINT, 0, 0);
	if(iResultCount > 0)
	{
		CString strInfo;
		strInfo.Format("搜索完成，共搜索到%d条信息", iResultCount);
		pDlg->MessageBox(strInfo.GetBuffer());
	}
	return 0;
}

void CVehicleTrackingSystemDlg::CopyResultPicToSavePath(char* pszFilePathName, char* pszSavePath, CString& strSavePathName)
{
	if(pszFilePathName == NULL || pszSavePath == NULL)
	{
		return;
	}
	CString strFilePathName = pszFilePathName;
	int iIndex = strFilePathName.ReverseFind('\\');
	strFilePathName = strFilePathName.Mid(iIndex, strFilePathName.GetLength()-iIndex);
	strSavePathName = pszSavePath;
	if(strSavePathName[strSavePathName.GetLength()-1] != '\\')
	strSavePathName += "\\";
	strSavePathName += strFilePathName;

	char szCurrentPath[MAX_PATH] = {0};
	GetModuleFileName(NULL, szCurrentPath, MAX_PATH);
	PathRemoveFileSpec(szCurrentPath);
	CString strSrcPath = szCurrentPath;
	strSrcPath += "\\TmpFinalResult\\";
	strSrcPath += strFilePathName;
	if(PathFileExists(strSrcPath.GetBuffer()))
	{
		CopyFile(strSrcPath.GetBuffer(), strSavePathName.GetBuffer(), FALSE);
	}
}

void CVehicleTrackingSystemDlg::OnSaveResult()
{
	EnterCriticalSection(&m_csFinalResultList);
	if(m_FinalResultList.GetCount() <= 0)
	{
		MessageBox("车辆行驶路径查询结果为空,无法保存!", "提示", MB_OK|MB_ICONWARNING);
		LeaveCriticalSection(&m_csFinalResultList);
		return;
	}
	LeaveCriticalSection(&m_csFinalResultList);
	
	int iTotolSaveCount = 0;
	if(m_iStartResultIndex != -1)
	{
		if(m_iEndResultIndex == -1)
			iTotolSaveCount = (int)m_FinalResultList.GetCount() - m_iStartResultIndex;
		else
			iTotolSaveCount = m_iEndResultIndex - m_iStartResultIndex;
	}
	else
	{
		if(m_iEndResultIndex == -1)
			iTotolSaveCount = (int)m_FinalResultList.GetCount();
		else
			iTotolSaveCount = m_iEndResultIndex;
	}
	if(iTotolSaveCount <= 0)
	{
		MessageBox("未选中任何记录,无法保存", "提示", MB_OK|MB_ICONWARNING);
		return;
	}
	BROWSEINFO stInfo = {NULL};
	LPCITEMIDLIST pIdlst;
	TCHAR szPath[MAX_PATH];
	stInfo.ulFlags = BIF_DONTGOBELOWDOMAIN | BIF_RETURNONLYFSDIRS | BIF_USENEWUI;
	stInfo.lpszTitle= "请选择导出路径:";
	pIdlst = SHBrowseForFolder(&stInfo);
	if(!pIdlst) return;
	if(SHGetPathFromIDList(pIdlst, szPath) == TRUE)
	{
		CString strSavePath = szPath;
		if(strSavePath[strSavePath.GetLength()-1] != '\\')
		{
			strSavePath += "\\";
		}
		
		CString strPlate = "";
		CString strStartPassTime = "";
		CString strEndPassTime = "";
		EnterCriticalSection(&m_csFinalResultList);
		POSITION posTmpFinalResult;
		if(m_iStartResultIndex >= 0)
			posTmpFinalResult = m_FinalResultList.FindIndex(m_iStartResultIndex);
		else
			posTmpFinalResult = m_FinalResultList.GetHeadPosition();

		POSITION posEndResult;
		if(m_iStartResultIndex == -1)
		{
			posEndResult = m_FinalResultList.FindIndex(iTotolSaveCount-1);
		}
		else
		{
			posEndResult = m_FinalResultList.FindIndex(m_iStartResultIndex+iTotolSaveCount-1);
		}
		if(!posTmpFinalResult || !posEndResult)
		{
			MessageBox("获取查询结果失败，保存失败", "ERROR", MB_OK|MB_ICONERROR);
			LeaveCriticalSection(&m_csFinalResultList);
			return;
		}
		
		FinalResultType* pTmpFinalResult = m_FinalResultList.GetAt(posTmpFinalResult);
		strPlate = pTmpFinalResult->szPlate;
		strStartPassTime = pTmpFinalResult->szPassTime;
		pTmpFinalResult = m_FinalResultList.GetAt(posEndResult);
		strEndPassTime = pTmpFinalResult->szPassTime;
		
		//去掉无车牌结果前面的空格以免保存失败
		char szPlate[30] = {0};
		memcpy(szPlate, strPlate.GetBuffer(), strPlate.GetLength());
		if(szPlate[0] == '0')
		{
			strPlate = szPlate+2;
		}
		
		//去掉时间空格
		int iY, iM, iD, iH, iMi, iS;
		char a, b;
		sscanf(strStartPassTime.GetBuffer(), "%d%c%d%c%d %d:%d:%d", &iY, &a, &iM, &b, &iD, &iH, &iMi, &iS);
		CTime cTmpStartTime(iY, iM, iD, iH, iMi, iS);
		strStartPassTime = cTmpStartTime.Format("%Y%m%d%H%M%S");
		sscanf(strEndPassTime.GetBuffer(), "%d%c%d%c%d %d:%d:%d", &iY, &a, &iM, &b, &iD, &iH, &iMi, &iS);
		CTime cTmpEndTime(iY, iM, iD, iH, iMi, iS);
		strEndPassTime = cTmpEndTime.Format("%Y%m%d%H%M%S");

		//保存查询结果
		CString strSaveFileName;
		strSaveFileName.Format("%s%s_%s_%s.xls", strSavePath.GetBuffer(),
			strPlate.GetBuffer(), strStartPassTime.GetBuffer(),
			strEndPassTime.GetBuffer());
		if(PathFileExists(strSaveFileName.GetBuffer()))
		{
			CString strInfo;
			strInfo.Format("%s\n文件已经存在，是否覆盖?", strSaveFileName.GetBuffer());
			if(MessageBox(strInfo.GetBuffer(), "警告", MB_OK|MB_ICONWARNING) == IDNO)
			{
				LeaveCriticalSection(&m_csFinalResultList);
				return;
			}
			else
			{
				DeleteFile(strSaveFileName.GetBuffer());
			}
		}

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
			m_workBook = m_workBooks.Add(covOptional);
			m_workSheets = m_workBook.get_Worksheets();
			m_workSheet = m_workSheets.get_Item(COleVariant((short)1));

			m_range = m_workSheet.get_Range(COleVariant("A1"), covOptional);
			m_range.put_Value2(COleVariant("车牌号码"));
			m_range.put_ColumnWidth(COleVariant(long(40)));
			m_range.BorderAround(COleVariant((short)1), (long)2, (long)1, covOptional);
			m_range = m_workSheet.get_Range(COleVariant("B1"), covOptional);
			m_range.put_Value2(COleVariant(strPlate.GetBuffer()));
			m_range.put_ColumnWidth(COleVariant(long(20)));
			m_range.BorderAround(COleVariant((short)1), (long)2, (long)1, covOptional);
			m_range = m_workSheet.get_Range(COleVariant("A3"), covOptional);
			m_range.put_Value2(COleVariant("经过断面名称"));
			m_range.BorderAround(COleVariant((short)1), (long)2, (long)1, covOptional);
			m_range = m_workSheet.get_Range(COleVariant("B3"), covOptional);
			m_range.put_Value2(COleVariant("经过断面时间"));
			m_range.BorderAround(COleVariant((short)1), (long)2, (long)1, covOptional);
			m_range = m_workSheet.get_Range(COleVariant("C3"), covOptional);
			m_range.put_ColumnWidth(COleVariant(long(40)));
			m_range.put_Value2(COleVariant("识别图保存路径"));
			m_range.BorderAround(COleVariant((short)1), (long)2, (long)1, covOptional);
			m_range = m_workSheet.get_Range(COleVariant("D3"), covOptional);
			m_range.put_ColumnWidth(COleVariant(long(40)));
			m_range.put_Value2(COleVariant("抓拍图保存路径"));
			m_range.BorderAround(COleVariant((short)1), (long)2, (long)1, covOptional);
			m_range = m_workSheet.get_Range(COleVariant("E3"), covOptional);
			m_range.put_ColumnWidth(COleVariant(long(10)));
			m_range.put_Value2(COleVariant("地图坐标"));
			m_range.BorderAround(COleVariant((short)1), (long)2, (long)1, covOptional);

			if(m_iStartResultIndex >= 0)
				posTmpFinalResult = m_FinalResultList.FindIndex(m_iStartResultIndex);
			else
				posTmpFinalResult = m_FinalResultList.GetHeadPosition();
			int iIndexTotle = 0;
			while(posTmpFinalResult)
			{
				pTmpFinalResult = m_FinalResultList.GetNext(posTmpFinalResult);

				char szRangInfo[20];
				sprintf(szRangInfo, "A%d", iIndexTotle+4);
				m_range = m_workSheet.get_Range(COleVariant(szRangInfo), covOptional);
				m_range.put_Value2(COleVariant(pTmpFinalResult->szLocaName));
				m_range.BorderAround(COleVariant((short)1), (long)2, (long)1, covOptional);

				CString strTime;
				strTime.Format("\"%s\"", pTmpFinalResult->szPassTime);
				sprintf(szRangInfo, "B%d", iIndexTotle+4);
				m_range = m_workSheet.get_Range(COleVariant(szRangInfo), covOptional);
				m_range.put_Value2(COleVariant(strTime.GetBuffer()));
				m_range.BorderAround(COleVariant((short)1), (long)2, (long)1, covOptional);

				CString strTmpSaveFileName = "";
				CopyResultPicToSavePath(pTmpFinalResult->szRPicPath, strSavePath.GetBuffer(), strTmpSaveFileName);
				sprintf(szRangInfo, "C%d", iIndexTotle+4);
				m_range = m_workSheet.get_Range(COleVariant(szRangInfo), covOptional);
				m_range.put_Value2(COleVariant(strTmpSaveFileName.GetBuffer()));
				m_range.BorderAround(COleVariant((short)1), (long)2, (long)1, covOptional);

				strTmpSaveFileName = "";
				CopyResultPicToSavePath(pTmpFinalResult->szCPicPath, strSavePath.GetBuffer(), strTmpSaveFileName);
				sprintf(szRangInfo, "D%d", iIndexTotle+4);
				m_range = m_workSheet.get_Range(COleVariant(szRangInfo), covOptional);
				m_range.put_Value2(COleVariant(strTmpSaveFileName.GetBuffer()));
				m_range.BorderAround(COleVariant((short)1), (long)2, (long)1, covOptional);

				char szLocaInfo[256];
				sprintf(szLocaInfo, "\"%d,%d\"", pTmpFinalResult->Loca.x, pTmpFinalResult->Loca.y);
				sprintf(szRangInfo, "E%d", iIndexTotle+4);
				m_range = m_workSheet.get_Range(COleVariant(szRangInfo), covOptional);
				m_range.put_Value2(COleVariant(szLocaInfo));
				m_range.BorderAround(COleVariant((short)1), (long)2, (long)1, covOptional);

				iIndexTotle++;
				if(iIndexTotle > iTotolSaveCount) break;
			}

			m_workBook.SaveAs(COleVariant(strSaveFileName.GetBuffer(0)), COleVariant((short)-4143), covOptional,
				covOptional, covOptional, covOptional, 0, covOptional, covOptional, covOptional, covOptional, covOptional);
			m_workBook.put_Saved(true);
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
			MessageBox("Execl操作失败，结果将以TXT格式保存", "提示", MB_OK|MB_ICONWARNING);
			//以TXT格式保存
			strSaveFileName.Format("%s%s_%s_%s.txt", strSavePath.GetBuffer(),
				strPlate.GetBuffer(), strStartPassTime.GetBuffer(),
				strEndPassTime.GetBuffer());
			if(PathFileExists(strSaveFileName.GetBuffer()))
			{
				CString strInfo;
				strInfo.Format("%s\n文件已经存在，是否覆盖?", strSaveFileName.GetBuffer());
				if(MessageBox(strInfo.GetBuffer(), "警告", MB_OK|MB_ICONWARNING) == IDNO)
				{
					LeaveCriticalSection(&m_csFinalResultList);
					return;
				}
				else
				{
					DeleteFile(strSaveFileName.GetBuffer());
				}
			}

			FILE* fp = fopen(strSaveFileName.GetBuffer(), "wb");
			if(fp)
			{
				fwrite("车牌号码:\t\t", 11, 1, fp);
				fwrite(strPlate.GetBuffer(), strPlate.GetLength(), 1, fp);
				fwrite("\n\n", 2, 1, fp);

				fwrite("经过断面名称\t\t", 14, 1, fp);
				fwrite("经过断面时间\t\t", 14, 1, fp);
				fwrite("识别图保存路径\t\t", 16, 1, fp);
				fwrite("抓拍图保存路径\t\t", 16, 1, fp);
				fwrite("地图坐标\n", 9, 1, fp);

				if(m_iStartResultIndex >= 0)
					posTmpFinalResult = m_FinalResultList.FindIndex(m_iStartResultIndex);
				else
					posTmpFinalResult = m_FinalResultList.GetHeadPosition();
				while(posTmpFinalResult)
				{
					pTmpFinalResult = m_FinalResultList.GetNext(posTmpFinalResult);
					fwrite(pTmpFinalResult->szLocaName, strlen(pTmpFinalResult->szLocaName), 1, fp);
					fwrite("\t\t", 2, 1, fp);

					fwrite(pTmpFinalResult->szPassTime, strlen(pTmpFinalResult->szPassTime), 1, fp);
					fwrite("\t\t", 2, 1, fp);

					CString strTmpSaveFileName = "";
					CopyResultPicToSavePath(pTmpFinalResult->szRPicPath, strSavePath.GetBuffer(), strTmpSaveFileName);
					fwrite(strTmpSaveFileName.GetBuffer(), strTmpSaveFileName.GetLength(), 1, fp);
					fwrite("\t\t", 2, 1, fp);
					
					strTmpSaveFileName = "";
					CopyResultPicToSavePath(pTmpFinalResult->szCPicPath, strSavePath.GetBuffer(), strTmpSaveFileName);
					fwrite(strTmpSaveFileName.GetBuffer(), strTmpSaveFileName.GetLength(), 1, fp);
					fwrite("\t\t", 2, 1, fp);

					char szLocaInfo[256];
					sprintf(szLocaInfo, "\"%d,%d\"", pTmpFinalResult->Loca.x, pTmpFinalResult->Loca.y);
					fwrite(szLocaInfo, strlen(szLocaInfo), 1, fp);
					fwrite("\n", 1, 1, fp);
				}
				fclose(fp);
			}
		}
		LeaveCriticalSection(&m_csFinalResultList);

		//保存场景信息
		strSaveFileName.Format("%s%s_%s_%s.dat", strSavePath.GetBuffer(),
			strPlate.GetBuffer(), strStartPassTime.GetBuffer(),
			strEndPassTime.GetBuffer());
		FILE* fp = fopen(strSaveFileName.GetBuffer(), "wb");
		if(fp)
		{
			EnterCriticalSection(&m_csLocaList);
			fwrite("<LOCAINFO>", 10, 1, fp);
			int iPointCount = (int)m_LocaList.GetCount();
			fwrite(&iPointCount, sizeof(int), 1, fp);
			POSITION posLoca = m_LocaList.GetHeadPosition();
			while(posLoca)
			{
				LocaType* pTmpLoca = m_LocaList.GetNext(posLoca);
				POINT cPoint;
				cPoint.x = pTmpLoca->iPositionX;
				cPoint.y = pTmpLoca->iPositionY;
				fwrite(&cPoint, sizeof(POINT), 1, fp);
			}
			LeaveCriticalSection(&m_csLocaList);

			fwrite("<MAPDATA>", 9, 1, fp);
			int iSize = 1024*786*3;
			PBYTE pMapData = new BYTE[iSize];
			if(pMapData == NULL)
			{
				iSize = 0;
			}
			fwrite(&iSize, sizeof(int), 1, fp);
			if(iSize > 0)
			{
				memset(pMapData, 0, iSize);
				if(m_lpBack)
				{
					DDSURFACEDESC2 ddsd;
					ZeroMemory(&ddsd, sizeof(ddsd));
					ddsd.dwSize = sizeof(ddsd);
					m_lpBack->Lock(NULL, &ddsd, 0, NULL);
					PBYTE pFrameData = (PBYTE)ddsd.lpSurface;
					INT indexX, indexY;
					PBYTE tmpImage = pMapData;
					PBYTE pImage = pFrameData;

					if(ddsd.ddpfPixelFormat.dwRGBBitCount == 32)
					{
						for(indexY=0; indexY<786; indexY++)
						{
							for(indexX=0; indexX<1024; indexX++)
							{
								*tmpImage++ = *(pImage+2); 
								*tmpImage++ = *(pImage+1); 
								*tmpImage++ = *(pImage);
								pImage += 4;
							}
						}
					}
					else if(ddsd.ddpfPixelFormat.dwRGBBitCount == 16)
					{
						for(indexY=0; indexY<786; indexY++)
						{
							for(indexX=0; indexX<1024; indexX++)
							{
								WORD wColor;
								memcpy(&wColor, pImage, 2);
								pImage += 2;
								int r = (BYTE)((wColor >> 11) & 0x1F);
								int g = (BYTE)((wColor >> 5) & 0x3F);
								int b = (BYTE)(wColor & 0x1F);
								r <<= 3;
								g <<= 2;
								b <<= 3;
								if(r < 0) r = 0;
								else if(r > 255) r = 255;
								if(g < 0) g = 0;
								else if(g > 255) g = 255;
								if(b < 0) b = 0;
								else if(b > 255) b = 255;
								*tmpImage++ = (BYTE)r; 
								*tmpImage++ = (BYTE)g; 
								*tmpImage++ = (BYTE)b;
							}
						}
					}
					m_lpBack->Unlock(NULL);
				}
				fwrite(pMapData, iSize, 1, fp);
			}

			fclose(fp);
		}
	}
}

void CVehicleTrackingSystemDlg::OnViewSavedResult()
{
	CFileDialog cGetOpenFile(TRUE, NULL, NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, "Excel文件(*.xls)|*.xls|文本文件(*.txt)|*.txt||");
	if(cGetOpenFile.DoModal() == IDOK)
	{
		CString strOpenFileName = cGetOpenFile.GetPathName();
		if(strstr(strOpenFileName.GetBuffer(), ".xls") == NULL
			&& strstr(strOpenFileName.GetBuffer(), ".txt") == NULL)
		{
			MessageBox("不正确的文件类型,无法打开!", "ERROR", MB_OK|MB_ICONERROR);
			return;
		}
		CResultViewDlg pDlg(strOpenFileName.GetBuffer(), this);
		pDlg.DoModal();
	}
}

void CVehicleTrackingSystemDlg::OnNMRclickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	int iColumn = pNMListView->iItem;
	if(iColumn < 0 || iColumn >= m_ListSever.GetItemCount())
	{
		return;
	}
	
	CMenu cMecu;
	cMecu.CreatePopupMenu();
	cMecu.AppendMenu(MF_BYCOMMAND | MF_STRING, ID_EDIT_SERVER, "修改服务器信息");
	cMecu.AppendMenu(MF_BYCOMMAND | MF_STRING, ID_REMOVE_SERVER, "删除服务器信息");
	CPoint cTmpPoint;
	GetCursorPos(&cTmpPoint);
	cMecu.TrackPopupMenu(TPM_LEFTALIGN, cTmpPoint.x, cTmpPoint.y, this);

	*pResult = 0;
}

void CVehicleTrackingSystemDlg::OnEditServerComm()
{
	int iSelectIndex = m_ListSever.GetSelectionMark();
	if(iSelectIndex < 0 || iSelectIndex >= m_ListSever.GetItemCount())
	{
		return;
	}
	EnterCriticalSection(&m_csServerList);
	POSITION posTmpServer = m_ServerList.FindIndex(iSelectIndex);
	if(posTmpServer)
	{
		NewSeverType* pTmpServer = m_ServerList.GetAt(posTmpServer);

		NewSeverType pTestChange;
		memcpy(&pTestChange, pTmpServer, sizeof(NewSeverType));
		CAddNewSever pAddNewTest(&pTestChange, FALSE, this);
		pAddNewTest.DoModal();

		if(strcmp(pTestChange.szLocaName, pTmpServer->szLocaName) != 0 ||
			strcmp(pTestChange.szServerIP, pTmpServer->szServerIP) != 0 ||
			strcmp(pTestChange.szDataBaseName, pTmpServer->szDataBaseName) != 0 ||
			strcmp(pTestChange.szUserName, pTmpServer->szUserName) != 0 ||
			strcmp(pTestChange.szPassWord, pTmpServer->szPassWord) != 0)
		{
			EnterCriticalSection(&m_csLocaList);
			int iPosIndex = 0;
			int iTotolPos = (int)m_LocaList.GetCount();
			for(iPosIndex=0; iPosIndex<iTotolPos; iPosIndex++)
			{
				POSITION posLoca = m_LocaList.FindIndex(iPosIndex);
				if(posLoca)
				{
					LocaType* pTmpLoca = m_LocaList.GetAt(posLoca);
					if(strcmp(pTmpLoca->szServerName, pTmpServer->szLocaName) == 0
						&& strcmp(pTmpLoca->szServerIP, pTmpServer->szServerIP) == 0
						&& strcmp(pTmpLoca->szDataBaseName, pTmpServer->szDataBaseName) == 0)
					{
						delete pTmpLoca;
						pTmpLoca = NULL;
						m_LocaList.RemoveAt(posLoca);
						iPosIndex--;
						iTotolPos--;
					}
				}
			}

			m_ServerList.RemoveAt(posTmpServer);
			memcpy(pTmpServer, &pTestChange, sizeof(NewSeverType));
			m_ServerList.AddTail(pTmpServer);

			LeaveCriticalSection(&m_csLocaList);
		}
	}
	LeaveCriticalSection(&m_csServerList);
	UpDataListServer();
	UpDataListLoca();
	GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON3)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON4)->EnableWindow(FALSE);
	m_ComBoxCarType.ResetContent();
	m_ComBoxCarType.AddString("全部车型");
	m_ComBoxCarType.SetCurSel(0);
	m_ComBoxServer.ResetContent();
}

void CVehicleTrackingSystemDlg::OnDeleteServerComm()
{
	int iSelectIndex = m_ListSever.GetSelectionMark();
	if(iSelectIndex < 0 || iSelectIndex >= m_ListSever.GetItemCount())
	{
		return;
	}
	EnterCriticalSection(&m_csServerList);
	POSITION posTmpServer = m_ServerList.FindIndex(iSelectIndex);
	if(posTmpServer)
	{
		NewSeverType* pTmpServer = m_ServerList.GetAt(posTmpServer);
		CString strInfo;
		strInfo.Format("确定删除%s<%s>的数据站点?", 
			pTmpServer->szLocaName, pTmpServer->szServerIP);
		if(MessageBox(strInfo.GetBuffer(), "警告", MB_YESNO) == IDNO)
		{
			LeaveCriticalSection(&m_csServerList);
			return;
		}
		EnterCriticalSection(&m_csLocaList);
		EnterCriticalSection(&m_csLocaRoadLine);
		int iPosIndex = 0;
		int iTotolPos = (int)m_LocaList.GetCount();
		for(iPosIndex=0; iPosIndex<iTotolPos; iPosIndex++)
		{
			POSITION posLoca = m_LocaList.FindIndex(iPosIndex);
			if(posLoca)
			{
				LocaType* pTmpLoca = m_LocaList.GetAt(posLoca);
				if(strcmp(pTmpLoca->szServerName, pTmpServer->szLocaName) == 0
					&& strcmp(pTmpLoca->szServerIP, pTmpServer->szServerIP) == 0
					&& strcmp(pTmpLoca->szDataBaseName, pTmpServer->szDataBaseName) == 0)
				{
					POSITION posRoadLine = m_LocaRoadLineType.FindIndex(iPosIndex);
					LocaRoadLineType* pLocaRoadLine = m_LocaRoadLineType.GetAt(posRoadLine);
					delete pLocaRoadLine;
					pLocaRoadLine = NULL;
					m_LocaRoadLineType.RemoveAt(posRoadLine);
					delete pTmpLoca;
					pTmpLoca = NULL;
					m_LocaList.RemoveAt(posLoca);
					iPosIndex--;
					iTotolPos--;
				}
			}
		}
		LeaveCriticalSection(&m_csLocaRoadLine);
		LeaveCriticalSection(&m_csLocaList);

		m_ServerList.RemoveAt(posTmpServer);
		delete pTmpServer;
		pTmpServer = NULL;
	}
	LeaveCriticalSection(&m_csServerList);
	UpDataListServer();
	UpDataListLoca();
	GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON3)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON4)->EnableWindow(FALSE);
	m_ComBoxCarType.ResetContent();
	m_ComBoxCarType.AddString("全部车型");
	m_ComBoxCarType.SetCurSel(0);
	m_ComBoxServer.ResetContent();
	SendMessage(WM_PAINT, 0, 0);
}

void CVehicleTrackingSystemDlg::OnOpenDemo()
{
	CString strServerIP = "";
	CString strDataBase = "";
	CString strUserName = "";
	CString strPassWord = "";
	CString strIniFilePath;
	char szAppPath[MAX_PATH] = {0};
	GetModuleFileName(NULL, szAppPath, MAX_PATH);
	PathRemoveFileSpec(szAppPath);
	strIniFilePath.Format("%s\\Config.ini", szAppPath);
	
	NewSeverType* pTmpServer = NULL;
	int iSelectItem = m_ListSever.GetSelectionMark();
	EnterCriticalSection(&m_csServerList);
	POSITION posServer = m_ServerList.FindIndex(iSelectItem);
	if(posServer)
	pTmpServer = m_ServerList.GetAt(posServer);
	LeaveCriticalSection(&m_csServerList);

	if(pTmpServer)
	{
		strServerIP = pTmpServer->szServerIP;
		strDataBase = pTmpServer->szDataBaseName;
		strUserName = pTmpServer->szUserName;
		strPassWord = pTmpServer->szPassWord;
	}
	else
	{
		strServerIP = "127.0.0.1";
		strDataBase = "HVE_DATA";
		strUserName = "sa";
		strPassWord = "123456";
	}

	WritePrivateProfileStringA("DBInfo", "ServerIP", strServerIP.GetBuffer(0), strIniFilePath.GetBuffer(0));
	WritePrivateProfileStringA("DBInfo", "DataBase", strDataBase.GetBuffer(0), strIniFilePath.GetBuffer(0));
	WritePrivateProfileStringA("DBInfo", "UserName", strUserName.GetBuffer(0), strIniFilePath.GetBuffer(0));
	WritePrivateProfileStringA("DBInfo", "Password", strPassWord.GetBuffer(0), strIniFilePath.GetBuffer(0));

	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	memset(&si, 0, sizeof(si));
	si.cb =sizeof(si);
	si.wShowWindow = SW_SHOW;
	si.dwFlags = STARTF_USESHOWWINDOW;
	strIniFilePath.Format("%s\\HVEDemoEx.exe", szAppPath);
	CreateProcess(strIniFilePath.GetBuffer(),
		NULL, NULL, FALSE, 
		NULL, NULL, NULL, NULL, &si, &pi);
}

void CVehicleTrackingSystemDlg::OnBnClickedCheck8()
{
	CButton* pBut = (CButton*)GetDlgItem(IDC_CHECK8);
	if(pBut->GetCheck() == 1)
	{
		GetDlgItem(IDC_CHECK1)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK2)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK3)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK4)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK5)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_CHECK1)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK2)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK3)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK4)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK5)->EnableWindow(TRUE);
	}
}

void CVehicleTrackingSystemDlg::On32782()
{
	NewSeverType* cNewServer = new NewSeverType();
	CAddNewSever pAddNewSeverDlg(cNewServer, TRUE, this);
	if(pAddNewSeverDlg.DoModal() == IDOK)
	{
		if(AddServerInfo(cNewServer) == TRUE)
		{
			UpDataListServer();
		}
		else
		{
			delete cNewServer;
			cNewServer = NULL;
		}
	}
	else
	{
		delete cNewServer;
		cNewServer = NULL;
	}
}

void CVehicleTrackingSystemDlg::ConventerRect(RECT& srcRect, RECT& desRect, float fRatioX, float fRatioY)
{
	desRect.left = (int)(srcRect.left * fRatioX);
	desRect.right = desRect.left + (int)((srcRect.right-srcRect.left) * fRatioX);
	desRect.top = (int)(srcRect.top * fRatioY);
	desRect.bottom = desRect.top + (int)((srcRect.bottom-srcRect.top) * fRatioY);

	if(desRect.left < 0) desRect.left = 0;
	if(desRect.left > 65536) desRect.right = 65536;
	if(desRect.right < 0) desRect.right = 0;
	if(desRect.right > 65536) desRect.right = 65536;
	if(desRect.top < 0) desRect.top = 0;
	if(desRect.top > 65536) desRect.top = 65536;
	if(desRect.bottom < 0) desRect.bottom = 0;
	if(desRect.bottom > 65536) desRect.bottom = 65536;
}

void CVehicleTrackingSystemDlg::SetMaxSize()
{
	RECT cRect;
	GetClientRect(&cRect);
	int iWidht = cRect.right - cRect.left;
	int iHeight = cRect.bottom - cRect.top;
	float fRatioX = (float)((float)iWidht / 1014.);
	float fRatioY = (float)((float)iHeight / 708.);

	RECT cDesRect;
	cRect.left = 2;
	cRect.right = 298;
	cRect.top = 2;
	cRect.bottom = 280;
	ConventerRect(cRect, cDesRect, fRatioX, fRatioY);
	m_ListSever.MoveWindow(&cDesRect, TRUE);

	cRect.left = 165;
	cRect.right = 765;
	cRect.top = 285;
	cRect.bottom = 705;
	ConventerRect(cRect, cDesRect, fRatioX, fRatioY);
	GetDlgItem(IDC_STATIC)->MoveWindow(&cDesRect, TRUE);

	cRect.left = 2;
	cRect.right = 162;
	cRect.top = 285;
	cRect.bottom = 705;
	ConventerRect(cRect, cDesRect, fRatioX, fRatioY);
	m_ListLoca.MoveWindow(&cDesRect, TRUE);

	cRect.left = 300;
	cRect.right = 765;
	cRect.top = 5;
	cRect.bottom = 105;
	ConventerRect(cRect, cDesRect, fRatioX, fRatioY);
	GetDlgItem(IDC_STATIC222)->MoveWindow(&cDesRect, TRUE);

	cRect.left = 305;
	cRect.right = 365;
	cRect.top = 25;
	cRect.bottom = 45;
	ConventerRect(cRect, cDesRect, fRatioX, fRatioY);
	GetDlgItem(IDC_STATIC4)->MoveWindow(&cDesRect, TRUE);
	cRect.left = 370;
	cRect.right = 525;
	cRect.top = 25;
	cRect.bottom = 45;
	ConventerRect(cRect, cDesRect, fRatioX, fRatioY);
	m_DateStart.MoveWindow(&cDesRect, TRUE);
	cRect.left = 530;
	cRect.right = 590;
	cRect.top = 25;
	cRect.bottom = 45;
	ConventerRect(cRect, cDesRect, fRatioX, fRatioY);
	GetDlgItem(IDC_STATIC5)->MoveWindow(&cDesRect, TRUE);
	cRect.left = 592;
	cRect.right = 762;
	cRect.top = 25;
	cRect.bottom = 45;
	ConventerRect(cRect, cDesRect, fRatioX, fRatioY);
	m_DateEnd.MoveWindow(&cDesRect, TRUE);

	cRect.left = 305;
	cRect.right = 365;
	cRect.top = 50;
	cRect.bottom = 70;
	ConventerRect(cRect, cDesRect, fRatioX, fRatioY);
	GetDlgItem(IDC_CHECK1)->MoveWindow(&cDesRect, TRUE);

	cRect.left = 370;
	cRect.right = 430;
	cRect.top = 50;
	cRect.bottom = 70;
	ConventerRect(cRect, cDesRect, fRatioX, fRatioY);
	GetDlgItem(IDC_CHECK2)->MoveWindow(&cDesRect, TRUE);

	cRect.left = 435;
	cRect.right = 505;
	cRect.top = 50;
	cRect.bottom = 70;
	ConventerRect(cRect, cDesRect, fRatioX, fRatioY);
	GetDlgItem(IDC_CHECK3)->MoveWindow(&cDesRect, TRUE);

	cRect.left = 535;
	cRect.right = 595;
	cRect.top = 50;
	cRect.bottom = 70;
	ConventerRect(cRect, cDesRect, fRatioX, fRatioY);
	GetDlgItem(IDC_CHECK4)->MoveWindow(&cDesRect, TRUE);

	cRect.left = 475;
	cRect.right = 535;
	cRect.top = 75;
	cRect.bottom = 95;
	ConventerRect(cRect, cDesRect, fRatioX, fRatioY);
	GetDlgItem(IDC_CHECK5)->MoveWindow(&cDesRect, TRUE);

	cRect.left = 329;
	cRect.right = 367;
	cRect.top = 75;
	cRect.bottom = 95;
	ConventerRect(cRect, cDesRect, fRatioX, fRatioY);
	GetDlgItem(IDC_STATICDIR1)->MoveWindow(&cDesRect, TRUE);

	cRect.left = 370;
	cRect.right = 470;
	cRect.top = 75;
	cRect.bottom = 95+200;
	ConventerRect(cRect, cDesRect, fRatioX, fRatioY);
	GetDlgItem(IDC_COMBODIR1)->MoveWindow(&cDesRect, TRUE);

	cRect.left = 665;
	cRect.right = 725;
	cRect.top = 50;
	cRect.bottom = 70;
	ConventerRect(cRect, cDesRect, fRatioX, fRatioY);
	GetDlgItem(IDC_CHECK8)->MoveWindow(&cDesRect, TRUE);

	cRect.left = 329;
	cRect.right = 367;
	cRect.top = 50;
	cRect.bottom = 70;
	ConventerRect(cRect, cDesRect, fRatioX, fRatioY);
	GetDlgItem(IDC_STATIC7)->MoveWindow(&cDesRect, TRUE);
	cRect.left = 370;
	cRect.right = 470;
	cRect.top = 50;
	cRect.bottom = 70 + 200;
	ConventerRect(cRect, cDesRect, fRatioX, fRatioY);
	m_ComBoxCarType.MoveWindow(&cDesRect, TRUE);

	cRect.left = 518;
	cRect.right = 590;
	cRect.top = 50;
	cRect.bottom = 70;
	ConventerRect(cRect, cDesRect, fRatioX, fRatioY);
	GetDlgItem(IDC_STATIC6)->MoveWindow(&cDesRect, TRUE);
	cRect.left = 592;
	cRect.right = 762;
	cRect.top = 50;
	cRect.bottom = 70;
	ConventerRect(cRect, cDesRect, fRatioX, fRatioY);
	GetDlgItem(IDC_EDIT1)->MoveWindow(&cDesRect, TRUE);


	cRect.left = 660;
	cRect.right = 760;
	cRect.top = 75;
	cRect.bottom = 95;
	ConventerRect(cRect, cDesRect, fRatioX, fRatioY);
	GetDlgItem(IDC_BUTTON1)->MoveWindow(&cDesRect, TRUE);

	cRect.left = 300;
	cRect.right = 765;
	cRect.top = 110;
	cRect.bottom = 280;
	ConventerRect(cRect, cDesRect, fRatioX, fRatioY);
	GetDlgItem(IDC_STATIC333)->MoveWindow(&cDesRect, TRUE);

	cRect.left = 305;
	cRect.right = 365;
	cRect.top = 130;
	cRect.bottom = 150;
	ConventerRect(cRect, cDesRect, fRatioX, fRatioY);
	GetDlgItem(IDC_STATIC3331)->MoveWindow(&cDesRect, TRUE);
	cRect.left = 370;
	cRect.right = 525;
	cRect.top = 130;
	cRect.bottom = 150;
	ConventerRect(cRect, cDesRect, fRatioX, fRatioY);
	GetDlgItem(IDC_EDIT3)->MoveWindow(&cDesRect, TRUE);

	cRect.left = 305;
	cRect.right = 415;
	cRect.top = 155;
	cRect.bottom = 175;
	ConventerRect(cRect, cDesRect, fRatioX, fRatioY);
	GetDlgItem(IDC_CHECK6)->MoveWindow(&cDesRect, TRUE);

	cRect.left = 305;
	cRect.right = 345;
	cRect.top = 155;
	cRect.bottom = 175;
	ConventerRect(cRect, cDesRect, fRatioX, fRatioY);
	GetDlgItem(IDC_STATICDIR2)->MoveWindow(&cDesRect, TRUE);

	cRect.left = 350;
	cRect.right = 430;
	cRect.top = 155;
	cRect.bottom = 175+200;
	ConventerRect(cRect, cDesRect, fRatioX, fRatioY);
	GetDlgItem(IDC_COMBODIR2)->MoveWindow(&cDesRect, TRUE);

	cRect.left = 435;
	cRect.right = 525;
	cRect.top = 155;
	cRect.bottom = 175;
	ConventerRect(cRect, cDesRect, fRatioX, fRatioY);
	GetDlgItem(IDC_CHECK7)->MoveWindow(&cDesRect, TRUE);

	cRect.left = 305;
	cRect.right = 365;
	cRect.top = 180;
	cRect.bottom = 200;
	ConventerRect(cRect, cDesRect, fRatioX, fRatioY);
	GetDlgItem(IDC_STATIC8)->MoveWindow(&cDesRect, TRUE);
	cRect.left = 370;
	cRect.right = 525;
	cRect.top = 180;
	cRect.bottom = 200;
	ConventerRect(cRect, cDesRect, fRatioX, fRatioY);
	m_SearchCarStartTime.MoveWindow(&cDesRect, TRUE);
	cRect.left = 305;
	cRect.right = 365;
	cRect.top = 205;
	cRect.bottom = 225;
	ConventerRect(cRect, cDesRect, fRatioX, fRatioY);
	GetDlgItem(IDC_STATIC9)->MoveWindow(&cDesRect, TRUE);
	cRect.left = 370;
	cRect.right = 525;
	cRect.top = 205;
	cRect.bottom = 225;
	ConventerRect(cRect, cDesRect, fRatioX, fRatioY);
	m_SearchCarEndTime.MoveWindow(&cDesRect, TRUE);

	cRect.left = 305;
	cRect.right = 365;
	cRect.top = 230;
	cRect.bottom = 250;
	ConventerRect(cRect, cDesRect, fRatioX, fRatioY);
	GetDlgItem(IDC_STATIC10)->MoveWindow(&cDesRect, TRUE);
	cRect.left = 370;
	cRect.right = 525;
	cRect.top = 230;
	cRect.bottom = 250 + 200;
	ConventerRect(cRect, cDesRect, fRatioX, fRatioY);
	m_ComBoxServer.MoveWindow(&cDesRect, TRUE);

	cRect.left = 305;
	cRect.right = 405;
	cRect.top = 255;
	cRect.bottom = 275;
	ConventerRect(cRect, cDesRect, fRatioX, fRatioY);
	GetDlgItem(IDC_BUTTON3)->MoveWindow(&cDesRect, TRUE);
	cRect.left = 425;
	cRect.right = 525;
	cRect.top = 255;
	cRect.bottom = 275;
	ConventerRect(cRect, cDesRect, fRatioX, fRatioY);
	GetDlgItem(IDC_BUTTON4)->MoveWindow(&cDesRect, TRUE);
	cRect.left = 530;
	cRect.right = 760;
	cRect.top = 130;
	cRect.bottom = 150;
	ConventerRect(cRect, cDesRect, fRatioX, fRatioY);
	GetDlgItem(IDC_STATIC666)->MoveWindow(&cDesRect, FALSE);
	cRect.left = 530;
	cRect.right = 760;
	cRect.top = 150;
	cRect.bottom = 275;
	ConventerRect(cRect, cDesRect, fRatioX, fRatioY);
	m_SelectServer.MoveWindow(&cDesRect, TRUE);

	cRect.left = 768;
	cRect.right = 1018;
	cRect.top = 5;
	cRect.bottom = 315;
	ConventerRect(cRect, cDesRect, fRatioX, fRatioY);
	m_ListResult.MoveWindow(&cDesRect, TRUE);
	cRect.left = 768;
	cRect.right = 1018;
	cRect.top = 320;
	cRect.bottom = 510;
	ConventerRect(cRect, cDesRect, fRatioX, fRatioY);
	GetDlgItem(IDC_STATIC_R_PIC)->MoveWindow(&cDesRect, TRUE);
	cRect.left = 768;
	cRect.right = 1018;
	cRect.top = 515;
	cRect.bottom = 705;
	ConventerRect(cRect, cDesRect, fRatioX, fRatioY);
	GetDlgItem(IDC_STATIC_C_PIC)->MoveWindow(&cDesRect, TRUE);

	int iWidth = 0;
	iWidth = (int)(80 * fRatioX);
	m_ListSever.SetColumnWidth(0, iWidth);
	iWidth = (int)(100 * fRatioX);
	m_ListSever.SetColumnWidth(1, iWidth);
	iWidth = (int)(60 * fRatioX);
	m_ListSever.SetColumnWidth(2, iWidth);
	iWidth = (int)(52 * fRatioX);
	m_ListSever.SetColumnWidth(3, iWidth);

	iWidth = (int)(60 * fRatioX);
	m_ListLoca.SetColumnWidth(0, iWidth);
	iWidth = (int)(96 * fRatioX);
	m_ListLoca.SetColumnWidth(1, iWidth);

	iWidth = (int)(40 * fRatioX);
	m_ListResult.SetColumnWidth(0, iWidth);
	iWidth = (int)(60 * fRatioX);
	m_ListResult.SetColumnWidth(1, iWidth);
	iWidth = (int)(73 * fRatioX);
	m_ListResult.SetColumnWidth(2, iWidth);
	iWidth = (int)(73 * fRatioX);
	m_ListResult.SetColumnWidth(3, iWidth);
 }

void CVehicleTrackingSystemDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	if(nType == SIZE_MAXIMIZED)
	{
		SetMaxSize();
	}
	else if(nType == SIZE_RESTORED)
	{
		if(!m_fIsInit)
		{
			SetDefaultSize();
			m_ListSever.SetColumnWidth(0, 80);
			m_ListSever.SetColumnWidth(1, 100);
			m_ListSever.SetColumnWidth(2, 60);
			m_ListSever.SetColumnWidth(3, 52);

			m_ListLoca.SetColumnWidth(0, 60);
			m_ListLoca.SetColumnWidth(1, 96);

			m_ListResult.SetColumnWidth(0, 40);
			m_ListResult.SetColumnWidth(1, 60);
			m_ListResult.SetColumnWidth(2, 73);
			m_ListResult.SetColumnWidth(3, 73);
			m_ListResult.SetColumnWidth(4, 0);
			m_ListResult.SetColumnWidth(5, 0);
			m_ListResult.SetColumnWidth(6, 0);
		}
	}
}

void CVehicleTrackingSystemDlg::OnPrevResult()
{
	if(m_iResultType != 1 && m_iResultType != 2) return;
	int iTotol = m_ListResult.GetItemCount();
	if(iTotol <= 0) return;
	m_ListResult.EnableWindow(FALSE);
	int iSelectIndex = m_ListResult.GetSelectionMark();
	if(iSelectIndex < 0 || iSelectIndex >= iTotol)
	{
		iSelectIndex = iTotol;
		m_ListResult.SetSelectionMark(iSelectIndex);
	}
	else
	{
		iSelectIndex--;
		if(iSelectIndex < 0)
		{
			m_ListResult.EnableWindow(TRUE);
			m_ListResult.SetFocus();
			MessageBox("已到达第一条记录");
			return;
		}
		m_ListResult.SetSelectionMark(iSelectIndex);
	}

	m_ListResult.EnsureVisible(iSelectIndex, TRUE);
	if(m_iResultType == 1)
	{
		char szPlateNo[20] = {0};
		m_ListResult.GetItemText(iSelectIndex, 1, szPlateNo, 20);
		SetDlgItemText(IDC_EDIT3, szPlateNo);
		LoadResultPic(iSelectIndex);
		UpDatePicBuffer();
		GetDlgItem(IDC_BUTTON3)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK7)->EnableWindow(TRUE);
	}
	else
	{
		m_iSelectResultIndex = iSelectIndex;
		UpDateFinalResultPic();
		UpDatePicBuffer();
		SendMessage(WM_PAINT, 0, 0);
	}
	m_ListResult.EnableWindow(TRUE);
	m_ListResult.SetFocus();
}

void CVehicleTrackingSystemDlg::OnNextResult()
{
	if(m_iResultType != 1 && m_iResultType != 2) return;
	int iTotol = m_ListResult.GetItemCount();
	if(iTotol <= 0) return;
	m_ListResult.EnableWindow(FALSE);
	int iSelectIndex = m_ListResult.GetSelectionMark();
	if(iSelectIndex < 0 || iSelectIndex >= iTotol)
	{
		iSelectIndex = 0;
		m_ListResult.SetSelectionMark(iSelectIndex);
	}
	else
	{
		iSelectIndex++;
		if(iSelectIndex >= iTotol)
		{
			m_ListResult.EnableWindow(TRUE);
			m_ListResult.SetFocus();
			MessageBox("已到达最后一条记录");
			return;
		}
		m_ListResult.SetSelectionMark(iSelectIndex);
	}

	m_ListResult.EnsureVisible(iSelectIndex, TRUE);
	if(m_iResultType == 1)
	{
		char szPlateNo[20] = {0};
		m_ListResult.GetItemText(iSelectIndex, 1, szPlateNo, 20);
		SetDlgItemText(IDC_EDIT3, szPlateNo);
		LoadResultPic(iSelectIndex);
		UpDatePicBuffer();
		GetDlgItem(IDC_BUTTON3)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK7)->EnableWindow(TRUE);
	}
	else
	{
		m_iSelectResultIndex = iSelectIndex;
		UpDateFinalResultPic();
		UpDatePicBuffer();
		SendMessage(WM_PAINT, 0, 0);
	}
	m_ListResult.EnableWindow(TRUE);
	m_ListResult.SetFocus();
}

void CVehicleTrackingSystemDlg::OnLvnKeydownList4(NMHDR *pNMHDR, LRESULT *pResult)
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

void CVehicleTrackingSystemDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CRect cTmpRect;
	GetDlgItem(IDC_STATIC)->GetWindowRect(&cTmpRect);
	ScreenToClient(&cTmpRect);
	if(cTmpRect.PtInRect(point))
	{
		CPicView* pViewPicDlg = new CPicView(m_lpOffScreen, this);
		pViewPicDlg->DoModal();
		delete pViewPicDlg;
		pViewPicDlg = NULL;
		
		CDialog::OnLButtonDblClk(nFlags, point);
		return;
	}

	GetDlgItem(IDC_STATIC_R_PIC)->GetWindowRect(&cTmpRect);
	ScreenToClient(&cTmpRect);
	if(cTmpRect.PtInRect(point))
	{
		if(PathFileExists(m_strRPicPath.GetBuffer()))
		{
			ShellExecute(m_hWnd, "open", m_strRPicPath, NULL, NULL, SW_SHOWMAXIMIZED);
		}

// 		if(PathFileExists(m_strRPicPath.GetBuffer()))
// 		{
// 			IPicture* pPic;
// 			IStream* pStm;
// 			HANDLE hFile = NULL;
// 			DWORD dwFileSize, dwByteRead;
// 			hFile = CreateFile(m_strRPicPath.GetBuffer(), GENERIC_READ, FILE_SHARE_READ, NULL,
// 				OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
// 			if(hFile == INVALID_HANDLE_VALUE)
// 			{
// 				return;
// 			}
// 			dwFileSize = GetFileSize(hFile, NULL);
// 			if(dwFileSize == 0xFFFFFFFF)
// 			{
// 				CloseHandle(hFile);
// 				return;
// 			}
// 			HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, dwFileSize);
// 			LPVOID pvData = NULL;
// 			if(hGlobal == NULL)
// 			{
// 				CloseHandle(hFile);
// 				return;
// 			}
// 			if((pvData = GlobalLock(hGlobal)) == NULL)
// 			{
// 				CloseHandle(hFile);
// 				GlobalFree(hGlobal);
// 				return;
// 			}
// 			ReadFile(hFile, pvData, dwFileSize, &dwByteRead, NULL);
// 			GlobalUnlock(hGlobal);
// 			CloseHandle(hFile);
// 
// 			HDC hTmpDC = ::GetDC(GetDlgItem(IDC_STATIC_R_PIC)->GetSafeHwnd());
// 			CreateStreamOnHGlobal(hGlobal, TRUE, &pStm);
// 			if(pStm == NULL)
// 			{
// 				GlobalFree(hGlobal);
// 				return;
// 			}
// 			HRESULT hResult = OleLoadPicture(pStm, dwFileSize, TRUE, IID_IPicture, (LPVOID*)&pPic);
// 			if(FAILED(hResult))
// 			{
// 				pStm->Release();
// 				GlobalFree(hGlobal);
// 			}
// 
// 			OLE_XSIZE_HIMETRIC hmWidth;
// 			OLE_YSIZE_HIMETRIC hmHeight;
// 			pPic->get_Width(&hmWidth);
// 			pPic->get_Height(&hmHeight);
// 			int iWidth = MulDiv(hmWidth, GetDeviceCaps(hTmpDC, LOGPIXELSX), 2540);
// 			int iHeight = MulDiv(hmHeight, GetDeviceCaps(hTmpDC, LOGPIXELSY), 2540);
// 			::ReleaseDC(GetDlgItem(IDC_STATIC_R_PIC)->GetSafeHwnd(), hTmpDC);
// 
// 			LPDIRECTDRAWSURFACE7 lpTmp;
// 			DDSURFACEDESC2 ddsd;
// 			ZeroMemory(&ddsd, sizeof(ddsd));
// 			ddsd.dwSize = sizeof(ddsd);
// 			ddsd.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
// 			ddsd.dwWidth = iWidth;
// 			ddsd.dwHeight = iHeight;
// 			ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
// 			m_lpDDS->CreateSurface(&ddsd, &lpTmp, NULL);
// 			HDC hTmpDC1;
// 			if(lpTmp)
// 			{
// 				lpTmp->GetDC(&hTmpDC1);
// 				pPic->Render(hTmpDC1, 0, 0, iWidth, iHeight, 0, hmHeight, hmWidth, -hmHeight, NULL);
// 				lpTmp->ReleaseDC(hTmpDC1);
// 			}
// 			pPic->Release();
// 			pStm->Release();
// 			GlobalFree(hGlobal);
// 			if(lpTmp)
// 			{
// 				CPicView* pViewPicDlg = new CPicView(lpTmp, this);
// 				pViewPicDlg->DoModal();
// 				delete pViewPicDlg;
// 				pViewPicDlg = NULL;
// 				lpTmp->Release();
// 				lpTmp = NULL;
// 			}
// 		}

		CDialog::OnLButtonDblClk(nFlags, point);
		return;
	}

	GetDlgItem(IDC_STATIC_C_PIC)->GetWindowRect(&cTmpRect);
	ScreenToClient(&cTmpRect);
	if(cTmpRect.PtInRect(point))
	{
		if(PathFileExists(m_strCPicPath.GetBuffer()))
		{
			ShellExecute(m_hWnd, "open", m_strCPicPath, NULL, NULL, SW_SHOWMAXIMIZED);
		}

// 		if(PathFileExists(m_strCPicPath.GetBuffer()))
// 		{
// 			IPicture* pPic;
// 			IStream* pStm;
// 			HANDLE hFile = NULL;
// 			DWORD dwFileSize, dwByteRead;
// 			hFile = CreateFile(m_strCPicPath.GetBuffer(), GENERIC_READ, FILE_SHARE_READ, NULL,
// 				OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
// 			if(hFile == INVALID_HANDLE_VALUE)
// 			{
// 				return;
// 			}
// 			dwFileSize = GetFileSize(hFile, NULL);
// 			if(dwFileSize == 0xFFFFFFFF)
// 			{
// 				CloseHandle(hFile);
// 				return;
// 			}
// 			HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, dwFileSize);
// 			LPVOID pvData = NULL;
// 			if(hGlobal == NULL)
// 			{
// 				CloseHandle(hFile);
// 				return;
// 			}
// 			if((pvData = GlobalLock(hGlobal)) == NULL)
// 			{
// 				CloseHandle(hFile);
// 				GlobalFree(hGlobal);
// 				return;
// 			}
// 			ReadFile(hFile, pvData, dwFileSize, &dwByteRead, NULL);
// 			GlobalUnlock(hGlobal);
// 			CloseHandle(hFile);
// 
// 			HDC hTmpDC = ::GetDC(GetDlgItem(IDC_STATIC_R_PIC)->GetSafeHwnd());
// 			CreateStreamOnHGlobal(hGlobal, TRUE, &pStm);
// 			if(pStm == NULL)
// 			{
// 				GlobalFree(hGlobal);
// 				return;
// 			}
// 			HRESULT hResult = OleLoadPicture(pStm, dwFileSize, TRUE, IID_IPicture, (LPVOID*)&pPic);
// 			if(FAILED(hResult))
// 			{
// 				pStm->Release();
// 				GlobalFree(hGlobal);
// 			}
// 
// 			OLE_XSIZE_HIMETRIC hmWidth;
// 			OLE_YSIZE_HIMETRIC hmHeight;
// 			pPic->get_Width(&hmWidth);
// 			pPic->get_Height(&hmHeight);
// 			int iWidth = MulDiv(hmWidth, GetDeviceCaps(hTmpDC, LOGPIXELSX), 2540);
// 			int iHeight = MulDiv(hmHeight, GetDeviceCaps(hTmpDC, LOGPIXELSY), 2540);
// 			::ReleaseDC(GetDlgItem(IDC_STATIC_R_PIC)->GetSafeHwnd(), hTmpDC);
// 
// 			LPDIRECTDRAWSURFACE7 lpTmp;
// 			DDSURFACEDESC2 ddsd;
// 			ZeroMemory(&ddsd, sizeof(ddsd));
// 			ddsd.dwSize = sizeof(ddsd);
// 			ddsd.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
// 			ddsd.dwWidth = iWidth;
// 			ddsd.dwHeight = iHeight;
// 			ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
// 			m_lpDDS->CreateSurface(&ddsd, &lpTmp, NULL);
// 			HDC hTmpDC1;
// 			if(lpTmp)
// 			{
// 				lpTmp->GetDC(&hTmpDC1);
// 				pPic->Render(hTmpDC1, 0, 0, iWidth, iHeight, 0, hmHeight, hmWidth, -hmHeight, NULL);
// 				lpTmp->ReleaseDC(hTmpDC1);
// 			}
// 			pPic->Release();
// 			pStm->Release();
// 			GlobalFree(hGlobal);
// 			if(lpTmp)
// 			{
// 				CPicView* pViewPicDlg = new CPicView(lpTmp, this);
// 				pViewPicDlg->DoModal();
// 				delete pViewPicDlg;
// 				pViewPicDlg = NULL;
// 				lpTmp->Release();
// 				lpTmp = NULL;
// 			}
// 		}


		CDialog::OnLButtonDblClk(nFlags, point);
		return;
	}
	CDialog::OnLButtonDblClk(nFlags, point);
}

void CVehicleTrackingSystemDlg::OnNMClickList2(NMHDR *pNMHDR, LRESULT *pResult)
{
	if(m_fIsUpDataLoca)
	{
		*pResult = 0;
		return;
	}
	m_fIsUpDataLoca = TRUE;
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	int iColumn = pNMListView->iItem;
	if(iColumn < 0 || iColumn > m_ListLoca.GetItemCount())
	{
		*pResult = 0;
		m_fIsUpDataLoca = FALSE;
		return;
	}
	m_iSelectIndex = iColumn;
	SendMessage(WM_UPDATE_SEARCH_INFO, 0, 0);
	SendMessage(WM_PAINT, 0, 0);
	*pResult = 0;
}

void CVehicleTrackingSystemDlg::OnLvnKeydownList2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);
	if(pLVKeyDow->wVKey == VK_DOWN)
	{
		int iTotol = m_ListLoca.GetItemCount();
		if(iTotol <= 0) return;
		m_ListLoca.EnableWindow(FALSE);
		int iSelectIndex = m_ListLoca.GetSelectionMark();
		if(iSelectIndex < 0 || iSelectIndex >= iTotol)
		{
			iSelectIndex = 0;
			m_ListLoca.SetSelectionMark(iSelectIndex);
		}
		else
		{
			iSelectIndex++;
			if(iSelectIndex >= iTotol)
			{
				m_ListLoca.EnableWindow(TRUE);
				m_ListLoca.SetFocus();
				MessageBox("已到达最后一条记录");
				return;
			}
			m_ListLoca.SetSelectionMark(iSelectIndex);
		}

		m_ListLoca.EnsureVisible(iSelectIndex, TRUE);
		m_iSelectIndex = iSelectIndex;
		m_ListLoca.EnableWindow(TRUE);
		m_ListLoca.SetFocus();
		SendMessage(WM_UPDATE_SEARCH_INFO, 0, 0);
		SendMessage(WM_PAINT, 0, 0);
	}
	else if(pLVKeyDow->wVKey == VK_UP)
	{
		int iTotol = m_ListLoca.GetItemCount();
		if(iTotol <= 0) return;
		m_ListLoca.EnableWindow(FALSE);
		int iSelectIndex = m_ListLoca.GetSelectionMark();
		if(iSelectIndex < 0 || iSelectIndex >= iTotol)
		{
			iSelectIndex = iTotol;
			m_ListLoca.SetSelectionMark(iSelectIndex);
		}
		else
		{
			iSelectIndex--;
			if(iSelectIndex < 0)
			{
				m_ListLoca.EnableWindow(TRUE);
				m_ListLoca.SetFocus();
				MessageBox("已到达第一条记录");
				return;
			}
			m_ListLoca.SetSelectionMark(iSelectIndex);
		}

		m_ListLoca.EnsureVisible(iSelectIndex, TRUE);
		m_iSelectIndex = iSelectIndex;
		m_ListLoca.EnableWindow(TRUE);
		m_ListLoca.SetFocus();
		SendMessage(WM_UPDATE_SEARCH_INFO, 0, 0);
		SendMessage(WM_PAINT, 0, 0);
	}
	*pResult = 0;
}

void CVehicleTrackingSystemDlg::OnNMClickList4(NMHDR *pNMHDR, LRESULT *pResult)
{
	if(m_iResultType != 1 && m_iResultType != 2)
	{
		return;
	}
	
	int iSelectIndex = m_ListResult.GetSelectionMark();
	if(iSelectIndex < 0 || iSelectIndex >= m_ListResult.GetItemCount())
	{
		return;
	}

	if(m_iResultType == 1)
	{
		char szPlateNo[20] = {0};
		m_ListResult.GetItemText(iSelectIndex, 1, szPlateNo, 20);
		SetDlgItemText(IDC_EDIT3, szPlateNo);
		LoadResultPic(iSelectIndex);
		UpDatePicBuffer();
		GetDlgItem(IDC_BUTTON3)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK7)->EnableWindow(TRUE);
	}
	else
	{
		m_iSelectResultIndex = iSelectIndex;
		UpDateFinalResultPic();
		UpDatePicBuffer();
		SendMessage(WM_PAINT, 0, 0);
	}

	*pResult = 0;
}
