#include "stdafx.h"
#include "QueryDlg.h"
#include ".\querydlg.h"
#include "PrintfView.h"
#include <afxole.h>

#include "CApplication.h"
#include "CWorkbook.h"
#include "CWorkbooks.h"
#include "CWorksheet.h"
#include "CWorksheets.h"
#include "CRange.h"

#define ID_OUTPUTRESULTTOEXECL	7777

IMPLEMENT_DYNAMIC(CQuery, CDialog)

extern CString g_strMacAddr;

CQuery::CQuery(CWnd* pParent /* = NULL */)
: CDialog(CQuery::IDD, pParent)
, m_bShowChart(FALSE)
{
	m_ParentFarm = (CHVEDemoExDlg*)pParent;
	m_GetDateThread = NULL;
	m_GetDateProcessThread = NULL;
	m_fIsGetDate = FALSE;
	m_fIsProcsee = FALSE;
	m_lpback = NULL;
	m_lpdds = NULL;
	m_fIsQuit = FALSE;

	m_pBrushChart = NULL;
	

}

void CQuery::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST3, m_ListReport);
	DDX_Control(pDX, IDC_DATETIMEPICKER1, m_Data);
	DDX_Control(pDX, IDC_COMBO3, m_MothYear);
	DDX_Control(pDX, IDC_COMBO2, m_MothMoth);
	DDX_Control(pDX, IDC_COMBO1, m_Year);
	DDX_Control(pDX, IDC_PROGRESS1, m_Process);
	DDX_Control(pDX, IDC_EDIT1, m_EditSun);
	DDX_Control(pDX, IDC_EDIT3, m_EditSpeed);
	DDX_Control(pDX, IDC_EDIT2, m_EditCross);
	DDX_Control(pDX, IDC_BUTTON1, m_ButtonQuery);
	DDX_Control(pDX, IDC_BUTTON5, m_ButtonPrint);
	DDX_Control(pDX, IDC_BUTTON6, m_ButtonReset);
	DDX_Control(pDX, IDC_BUTTON2, m_ButtonPrintView);
	DDX_Control(pDX, IDC_BUTTON3, m_ButtomPrintSet);
	DDX_Control(pDX, IDC_STATICREPORTTYPE, m_StaticReportType);
	DDX_Control(pDX, IDC_STATICREPORTTIME, m_StaticReportTimes);
	DDX_Control(pDX, IDC_MSCHART1, m_Chart);
	DDX_Check(pDX, IDC_CHECKSHOWCHART, m_bShowChart);
}

BEGIN_MESSAGE_MAP(CQuery, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_MESSAGE(WM_SEARCHFINISH, OnSearchDone)
	ON_BN_CLICKED(IDC_BUTTON5, OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON6, OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON3, OnBnClickedButton3)
	ON_MESSAGE(WM_VIEWQUIT, OnViewQuit)
	ON_BN_CLICKED(IDC_CHECK2, OnBnClickedCheck2)
	ON_NOTIFY(NM_RCLICK, IDC_LIST3, OnNMRclickList3)
	ON_COMMAND(ID_OUTPUTRESULTTOEXECL, OnOutPutResultToExecl)
//ON_NOTIFY(NM_THEMECHANGED, IDC_RADIO2, OnNMThemeChangedRadio2)

ON_BN_CLICKED(IDC_CHECKSHOWCHART, OnBnClickedCheckshowchart)
END_MESSAGE_MAP()

BOOL CQuery::OnInitDialog()
{
	CDialog::OnInitDialog();

	CoInitialize(NULL);
//  	AfxOleInit();
	m_fIsADOConnected = FALSE;
	((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(TRUE);
	((CButton*)GetDlgItem(IDC_RADIO2))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_RADIO3))->SetCheck(FALSE);
	m_Process.ShowWindow(SW_HIDE);
	m_ListReport.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_ListReport.InsertColumn(0, "时间", LVCFMT_LEFT, 42);
	m_ListReport.InsertColumn(1, "总车流量", LVCFMT_LEFT, 65);
	m_ListReport.InsertColumn(2, "占有率", LVCFMT_LEFT, 65);
	m_ListReport.InsertColumn(3, "平均车速", LVCFMT_LEFT, 66);
	m_ListReport.InsertColumn(4, "总车流量", LVCFMT_LEFT, 65);
	m_ListReport.InsertColumn(5, "占有率", LVCFMT_LEFT, 65);
	m_ListReport.InsertColumn(6, "平均车速", LVCFMT_LEFT, 66);
	m_ListReport.InsertColumn(7, "总车流量", LVCFMT_LEFT, 65);
	m_ListReport.InsertColumn(8, "占有率", LVCFMT_LEFT, 65);
	m_ListReport.InsertColumn(9, "平均车速", LVCFMT_LEFT, 66);
	m_ListReport.InsertColumn(10, "总车流量", LVCFMT_LEFT, 65);
	m_ListReport.InsertColumn(11, "占有率", LVCFMT_LEFT, 65);
	m_ListReport.InsertColumn(12, "平均车速", LVCFMT_LEFT, 70);
	m_ListReport.InsertColumn(13, "断面流量", LVCFMT_LEFT, 80);
	m_ListReport.SetBkColor(RGB(190, 190, 190));
	m_ListReport.SetTextBkColor(RGB(190, 190, 190));
	CButton* pBt = (CButton*)GetDlgItem(IDC_CHECK2);
	pBt->SetCheck(0);
	m_fIsReverse = 0;
	INT index;
	char Year[10];
	COleDateTime NowTime = COleDateTime::GetCurrentTime();
	for(index=0; index<20; index++)
	{
		sprintf(Year, "%d", 2000+index);
		m_MothYear.AddString(Year);
		m_Year.AddString(Year);
	}
	m_MothYear.SetCurSel(NowTime.GetYear() - 2000);
	m_Year.SetCurSel(NowTime.GetYear() - 2000);

	for(index=1; index<13; index++)
	{
		sprintf(Year, "%d", index);
		m_MothMoth.AddString(Year);
	}
	m_MothMoth.SetCurSel(NowTime.GetMonth() - 1);
	m_StaticReportType.SetWindowText("");
	m_StaticReportTimes.SetWindowText("");
	m_nReportType = -1;
	DirectDrawCreateEx(NULL, (void **)& m_lpdds, IID_IDirectDraw7, NULL);
	m_lpdds->SetCooperativeLevel(GetSafeHwnd(), DDSCL_NORMAL);

	InitChart();

	m_pBrushChart = new CBrush(RGB(255,255,255));
	m_fontChart.CreateFont(13, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, 
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH, "宋体");

	return TRUE;
}

void CQuery::OnBnClickedButton1()
{
	if(m_fIsGetDate || m_fIsProcsee) return;
	//if(m_ParentFarm == NULL)
	//{
	//	MessageBox("获取数据源出错，无法进行查询");
	//	OnCancel();
	//	return;
	//}
	//if(!m_ParentFarm->GetConnectStatus())
	//{
	//	MessageBox("未连接到数据库，请先进入系统设置连接数据");
	//	OnCancel();
	//	return;
	//}
 	m_ButtonQuery.EnableWindow(FALSE);
	m_ButtonReset.EnableWindow(FALSE);
	m_ButtonPrint.EnableWindow(FALSE);
	m_ButtonPrintView.EnableWindow(FALSE);
	m_ButtomPrintSet.EnableWindow(FALSE);
	m_Data.EnableWindow(FALSE);
	m_MothYear.EnableWindow(FALSE);
	m_MothMoth.EnableWindow(FALSE);
	m_Year.EnableWindow(FALSE);

	// hide chart
	if (m_bShowChart)
	{
		m_bShowChart = FALSE;
		UpdateData(FALSE);
		OnBnClickedCheckshowchart();
	}
	
	GetDlgItem(IDC_CHECKSHOWCHART)->EnableWindow(FALSE);


	LVCOLUMN    col;     
	col.fmt    =    LVIF_TEXT;   
	col.cchTextMax    =    10;
	char chText[10] = {0};
	col.pszText = chText;
	m_ListReport.GetColumn(13, &col);	

	// init chart
	InitChartData(0);

	if(((CButton*)GetDlgItem(IDC_RADIO1))->GetCheck())
	{
		sprintf(col.pszText, "断面流量");
		m_ListReport.SetColumn(13, &col);

// 		m_Chart.SetColumn(5);
// 		m_Chart.SetColumnLabel((LPCTSTR)"断面流量");
// 		InitChartData(24);

		((CButton*)GetDlgItem(IDC_RADIO1))->EnableWindow(FALSE);
		((CButton*)GetDlgItem(IDC_RADIO2))->EnableWindow(FALSE);
		((CButton*)GetDlgItem(IDC_RADIO3))->EnableWindow(FALSE);
		m_fIsGetDate = TRUE;
		m_GetDateThread = AfxBeginThread(GetDayDataThread, this);
		m_fIsProcsee = TRUE;
		m_GetDateProcessThread = AfxBeginThread(GetDayProcessThread, this);
	}
	else if(((CButton*)GetDlgItem(IDC_RADIO2))->GetCheck())
	{

		sprintf(col.pszText, "每天流量");
		m_ListReport.SetColumn(13, &col);

// 		m_Chart.SetColumn(5);
// 		m_Chart.SetColumnLabel((LPCTSTR)"每天流量");
// 
// 		char szTimeStr[10];
// 		m_MothYear.GetWindowText(szTimeStr, 10);
// 		int year = atoi(szTimeStr);
// 		m_MothMoth.GetWindowText(szTimeStr, 10);
// 		int moth = atoi(szTimeStr);
// 		InitChartData(GetMothDayCount(year, moth));

		((CButton*)GetDlgItem(IDC_RADIO1))->EnableWindow(FALSE);
		((CButton*)GetDlgItem(IDC_RADIO2))->EnableWindow(FALSE);
		((CButton*)GetDlgItem(IDC_RADIO3))->EnableWindow(FALSE);
		m_fIsGetDate = TRUE;
		m_GetDateThread = AfxBeginThread(GetMothDataThread, this);
		m_fIsProcsee = TRUE;
		m_GetDateProcessThread = AfxBeginThread(GetDayProcessThread, this);
	}
	else
	{
		sprintf(col.pszText, "每月流量");
		m_ListReport.SetColumn(13, &col);

// 		m_Chart.SetColumn(5);
// 		m_Chart.SetColumnLabel((LPCTSTR)"每月流量");
// 		InitChartData(12);

		((CButton*)GetDlgItem(IDC_RADIO1))->EnableWindow(FALSE);
		((CButton*)GetDlgItem(IDC_RADIO2))->EnableWindow(FALSE);
		((CButton*)GetDlgItem(IDC_RADIO3))->EnableWindow(FALSE);
		m_fIsGetDate = TRUE;
		m_GetDateThread = AfxBeginThread(GetYearDataThread, this);
		m_fIsProcsee = TRUE;
		m_GetDateProcessThread = AfxBeginThread(GetDayProcessThread, this);
	}
}

BOOL CQuery::ConnectDataBase()
{
	if(m_fIsQuit)return TRUE;
	if(m_fIsADOConnected) return TRUE;
	SetWindowText("正在连接数据库...");
	m_fIsADOConnected = TRUE;
	HRESULT hr = S_OK;
	try
	{

		if (m_pConnect)
		{
			if (m_pConnect->State == adStateOpen)
			{
				m_pConnect->Close();
			}
			m_pConnect->Cancel();
			m_pConnect = NULL;
		}


		CString strSql;
		CString strIP;
		DWORD dwIP = m_ParentFarm->GetSourcePath();
		strIP.Format("%d.%d.%d.%d", (dwIP>>24)&0xFF, (dwIP>>16)&0xFF, (dwIP>>8)&0xFF, (dwIP)&0xFF);
		if(strIP == "0.0.0.0")strIP = ".";
		strSql.Format("Provider=SQLOLEDB.1;Password=%s;User ID=%s;Initial Catalog=%s;Data Source=%s",
			m_ParentFarm->m_strPassWord, m_ParentFarm->m_strUserName, m_ParentFarm->m_strDataBaseName, strIP);
		hr = m_pConnect.CreateInstance("ADODB.Connection");
		if(SUCCEEDED(hr))
		{
			m_pConnect->ConnectionTimeout = 20;
			m_pConnect->CommandTimeout = 0;
			m_pConnect->CursorLocation = adUseServer;
			m_pConnect->IsolationLevel = adXactReadCommitted;
			if(m_fIsQuit) return FALSE;
			hr = m_pConnect->Open(
				(_bstr_t)(strSql.GetBuffer(0)), "", "", adModeUnknown);
		}
	}
	catch (_com_error e)
	{
		SetWindowText("连接数据库失败!");
		m_fIsADOConnected = FALSE;
		return FALSE;
	}
	SetWindowText("已与数据库建立连接");
	return TRUE;
}

BOOL CQuery::IsTableExists(CString strTableName)
{
	CString strSql;
	HRESULT hr = E_FAIL;
	strSql.Format("SELECT name FROM sysobjects WHERE (name = '%s')", strTableName.GetBuffer());
	_RecordsetPtr pPtr;
	hr = pPtr.CreateInstance("ADODB.Recordset");
	if(hr != S_OK) return FALSE;
	hr = pPtr->Open(_variant_t(strSql), m_pConnect.GetInterfacePtr(), 
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

BOOL CQuery::IsTableLikeThisExists(CString strTableName)
{
	CString strSql;
	HRESULT hr = E_FAIL;
	strSql.Format("SELECT name FROM sysobjects WHERE (name like '%%%s%%' and xtype = 'u')", strTableName.GetBuffer());
	_RecordsetPtr pPtr;

	try
	{
		hr = pPtr.CreateInstance("ADODB.Recordset");
	}
	catch (_com_error e)
	{
		AfxMessageBox(e.Description());
		return FALSE;		
	}
	
	if(hr != S_OK) 
		return FALSE;

	try
	{
		hr = pPtr->Open(_variant_t(strSql), m_pConnect.GetInterfacePtr(), 
			adOpenDynamic, adLockOptimistic, adCmdText);
	}
	catch (_com_error e)
	{
		AfxMessageBox(e.Description());
		return FALSE;		
	}

	if(hr != S_OK) 
		return FALSE;

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

BOOL CQuery::CreateView(COleDateTime& startTime, COleDateTime& endTime)
{
	CString strViewName = "Hve_Data_View1_";
	strViewName += g_strMacAddr;
	CString strSql;
	strSql.Format("IF EXISTS (SELECT TABLE_NAME FROM INFORMATION_SCHEMA.VIEWS WHERE TABLE_NAME = N'%s') DROP VIEW %s",
		strViewName.GetBuffer(), strViewName.GetBuffer());
	_variant_t RecordsetAffected;
	try
	{
		m_pConnect->Execute((_bstr_t)strSql.GetBuffer(), &RecordsetAffected, adCmdText);
	}
	catch (_com_error e)
	{
		AfxMessageBox(e.Description());
	}
	CTime cTmpStartTime(startTime.GetYear(), startTime.GetMonth(), startTime.GetDay(),
		startTime.GetHour(), startTime.GetMinute(), startTime.GetSecond());
	CTime cTmpEndTime(endTime.GetYear(), endTime.GetMonth(), endTime.GetDay(),
		endTime.GetHour(), endTime.GetMinute(), endTime.GetSecond());
	DWORD64 dwTmpStartTime = cTmpStartTime.GetTime();
	DWORD64 dwTmpEndTime = cTmpEndTime.GetTime();
	CString strTableList = "";
	if(endTime.GetYear() > startTime.GetYear()
		|| endTime.GetMonth() > startTime.GetMonth()
		|| endTime.GetDay() > startTime.GetDay())
	{
		while(dwTmpStartTime <= dwTmpEndTime)
		{
			CTime cTmpCurrentTime(dwTmpStartTime);
			CString strTmp = cTmpCurrentTime.Format("Hve_Data_%Y%m%d");
			if(IsTableExists(strTmp) == TRUE)
			{
				if(strTableList == "")
				// strTableList.Format("(SELECT * FROM %s)", strTmp);
				strTableList.Format("(SELECT Record_Id,Pass_Time,Road_No,Car_Speed,Location_Id,Special_Result FROM %s)", strTmp);
				else			
				// strTableList.AppendFormat("union (SELECT * FROM %s) ", strTmp.GetBuffer());
				strTableList.AppendFormat("union (SELECT Record_Id,Pass_Time,Road_No,Car_Speed,Location_Id,Special_Result FROM %s) ", 
				strTmp.GetBuffer());
			}
			dwTmpStartTime += 86400;
		}
	}
	else
	{
		CString strTmp = startTime.Format("Hve_Data_%Y%m%d");
		if(IsTableExists(strTmp) == TRUE)
			strTableList.Format("(SELECT * FROM %s)", strTmp.GetBuffer());
	}

	if(strTableList == "")
	{
		CString strTmpSql;
		HRESULT hr = E_FAIL;
		strTmpSql = "SELECT name FROM sysobjects WHERE (xtype = 'u' and name LIKE 'Hve_Data_%')";			//add "xtype = 'u' and ",ensure select table, by zhut 20121112
		_RecordsetPtr pPtr;
		hr = pPtr.CreateInstance("ADODB.Recordset");
		if(hr != S_OK) return FALSE;
		hr = pPtr->Open(_variant_t(strTmpSql), m_pConnect.GetInterfacePtr(), 
			adOpenDynamic, adLockOptimistic, adCmdText);
		if(hr != S_OK) return FALSE;
		if(pPtr->GetadoEOF())
		{
			pPtr->Close();
			pPtr.Release();
			return FALSE;
		}
		CString strTmpTable = (char*)_bstr_t(pPtr->GetCollect("name"));
		strTmpTable.TrimRight();
		strTableList.Format("(SELECT * FROM %s)", strTmpTable);
		pPtr->Close();
		pPtr.Release();
	}

	strSql.Format("CREATE VIEW %s AS %s", strViewName.GetBuffer(), strTableList.GetBuffer());
	try
	{
		m_pConnect->Execute((_bstr_t)strSql.GetBuffer(), &RecordsetAffected, adCmdText);
	}
	catch (_com_error e)
	{
		AfxMessageBox(e.Description());
		
	}
	return TRUE;
}

void CQuery::DropView()
{
	CString strViewName = "Hve_Data_View1_";
	strViewName += g_strMacAddr;
	CString strSql;
	strSql.Format("IF EXISTS (SELECT TABLE_NAME FROM INFORMATION_SCHEMA.VIEWS WHERE TABLE_NAME = N'%s') DROP VIEW %s",
		strViewName.GetBuffer(), strViewName.GetBuffer());
	_variant_t RecordsetAffected;
	try
	{
		m_pConnect->Execute((_bstr_t)strSql.GetBuffer(), &RecordsetAffected, adCmdText);
	}
	catch (_com_error e)
	{
		return;
	}
}

BOOL CQuery::GetCountByTime(CString StartTime, CString EndTime, QueryStruc& res)
{
	try
	{
		CString strSql;
		CString strPeriodTable = "strPeriodTable1_";
		strPeriodTable += g_strMacAddr;
		CString strViewName = "Hve_Data_View1_";
		strViewName += g_strMacAddr;
		_variant_t var;
		HRESULT hr = S_OK;
		if(m_fIsQuit)return FALSE;
		//strSql = "IF EXISTS(SELECT * FROM sysobjects WHERE name='Period1Table' AND xtype='U') ";
		//strSql += "DROP TABLE Period1Table";
		strSql.Format("IF EXISTS(SELECT * FROM sysobjects WHERE name='%s' AND xtype='U') DROP TABLE %s", 
			strPeriodTable.GetBuffer(), strPeriodTable.GetBuffer());
		_variant_t RecordsetAffected;
		m_pConnect->Execute((_bstr_t)strSql.GetBuffer(0), &RecordsetAffected, adCmdText);
		RecordsetAffected.Clear();

		if(m_fIsQuit)return FALSE;
		if(m_fIsReverse == 1)
		strSql.Format("SELECT Record_Id, Pass_Time, Road_No, '%s' AS PeriodTime, CASE WHEN Car_Speed=0 THEN 0 ELSE 10/(CONVERT(float, Car_Speed)*1000/3600) END AS PreCarCross \
					  INTO %s FROM %s WHERE Pass_Time BETWEEN '%s' AND '%s' AND Location_Id=%d AND Road_No < 0 ORDER BY %s.Pass_Time",
					  StartTime, strPeriodTable.GetBuffer(), strViewName.GetBuffer(),
					  StartTime, EndTime, m_ParentFarm->m_rgLocation[m_ParentFarm->m_nCurrentRoadID].nLocation_Id, strViewName.GetBuffer());
		else
			strSql.Format("SELECT Record_Id, Pass_Time, Road_No, '%s' AS PeriodTime, CASE WHEN Car_Speed=0 THEN 0 ELSE 10/(CONVERT(float, Car_Speed)*1000/3600) END AS PreCarCross \
						  INTO %s FROM %s WHERE Pass_Time BETWEEN '%s' AND '%s' AND Location_Id=%d AND Road_No > 0 ORDER BY %s.Pass_Time",
						  StartTime, strPeriodTable.GetBuffer(), strViewName.GetBuffer(),
						  StartTime, EndTime, m_ParentFarm->m_rgLocation[m_ParentFarm->m_nCurrentRoadID].nLocation_Id, strViewName.GetBuffer());
			
		m_pConnect->Execute((_bstr_t)strSql.GetBuffer(0), &RecordsetAffected, adCmdText);
		RecordsetAffected.Clear();
		//_RecordsetPtr pRenRecordset;
	
		if(m_fIsQuit)return FALSE;
		int iIndex = 0;
		hr = pRenRecordset.CreateInstance("ADODB.Recordset");
		if(m_fIsReverse == 1)
		{
			iIndex = -1;
		}
		else
		{
			iIndex = 1;
		}
		strSql.Format("SELECT COUNT(PeriodTime) AS CarCount"
			", AVG(CONVERT(bigint, Car_Speed)) AS CarSpeed "
			", 100*AVG(PreCarCross)/60/10 AS CarCross"
			", CASE WHEN SUM(CASE WHEN Special_Result = 1 THEN 1 ELSE 0 END) > 0 THEN 1 ELSE 0 END AS Violate"
			" FROM %s, %s "
			" WHERE (%s.Pass_Time = %s.Pass_Time AND %s.Record_Id = %s.Record_Id AND %s.Road_No = %d) ", 
			strViewName.GetBuffer(), strPeriodTable.GetBuffer(),
			strViewName.GetBuffer(), strPeriodTable.GetBuffer(),
			strViewName.GetBuffer(), strPeriodTable.GetBuffer(),
			strPeriodTable.GetBuffer(), iIndex);
		pRenRecordset->CursorLocation = adUseServer;
		hr = pRenRecordset->Open(_variant_t(strSql), m_pConnect.GetInterfacePtr(), 
			adOpenDynamic, adLockOptimistic, adCmdText);
		if(SUCCEEDED(hr))
		{
			var = pRenRecordset->GetCollect((long)0);
			res.CarCount_1 = (int)var.lVal;
			var = pRenRecordset->GetCollect((long)1);
			res.CarSpeed_1 = (int)var.lVal;
			var = pRenRecordset->GetCollect((long)2);
			res.Cross_1 = (double)var.dblVal * 100;
			pRenRecordset->Close();
			pRenRecordset = NULL;
		}

		if(m_fIsQuit)return FALSE;
		hr = pRenRecordset.CreateInstance("ADODB.Recordset");
		if(m_fIsReverse == 1)
		{
			iIndex = -2;
		}
		else
		{
			iIndex = 2;
		}
		strSql.Format("SELECT COUNT(PeriodTime) AS CarCount"
			", AVG(CONVERT(bigint, Car_Speed)) AS CarSpeed "
			", 100*AVG(PreCarCross)/60/10 AS CarCross"
			", CASE WHEN SUM(CASE WHEN Special_Result = 1 THEN 1 ELSE 0 END) > 0 THEN 1 ELSE 0 END AS Violate"
			" FROM %s, %s "
			" WHERE (%s.Pass_Time = %s.Pass_Time AND %s.Record_Id = %s.Record_Id AND %s.Road_No = %d) ",
			strViewName.GetBuffer(), strPeriodTable.GetBuffer(),
			strViewName.GetBuffer(), strPeriodTable.GetBuffer(),
			strViewName.GetBuffer(), strPeriodTable.GetBuffer(),
			strPeriodTable.GetBuffer(), iIndex);
		pRenRecordset->CursorLocation = adUseServer;
		hr = pRenRecordset->Open(_variant_t(strSql), m_pConnect.GetInterfacePtr(), 
			adOpenDynamic, adLockOptimistic, adCmdText);
		if(SUCCEEDED(hr))
		{
			var = pRenRecordset->GetCollect((long)0);
			res.CarCount_2 = (int)var.lVal;
			var = pRenRecordset->GetCollect((long)1);
			res.CarSpeed_2 = (int)var.lVal;
			var = pRenRecordset->GetCollect((long)2);
			res.Cross_2 = (double)var.dblVal * 100;
			pRenRecordset->Close();
			pRenRecordset = NULL;
		}

		if(m_fIsQuit)return FALSE;
		hr = pRenRecordset.CreateInstance("ADODB.Recordset");
		if(m_fIsReverse == 1)
		{
			iIndex = -3;
		}
		else
		{
			iIndex = 3;
		}
		strSql.Format("SELECT COUNT(PeriodTime) AS CarCount"
			", AVG(CONVERT(bigint, Car_Speed)) AS CarSpeed "
			", 100*AVG(PreCarCross)/60/10 AS CarCross"
			", CASE WHEN SUM(CASE WHEN Special_Result = 1 THEN 1 ELSE 0 END) > 0 THEN 1 ELSE 0 END AS Violate"
			" FROM %s, %s "
			" WHERE (%s.Pass_Time = %s.Pass_Time AND %s.Record_Id = %s.Record_Id AND %s.Road_No = %d) ",
			strViewName.GetBuffer(), strPeriodTable.GetBuffer(),
			strViewName.GetBuffer(), strPeriodTable.GetBuffer(),
			strViewName.GetBuffer(), strPeriodTable.GetBuffer(),
			strPeriodTable.GetBuffer(), iIndex);
		pRenRecordset->CursorLocation = adUseServer;
		hr = pRenRecordset->Open(_variant_t(strSql), m_pConnect.GetInterfacePtr(), 
			adOpenDynamic, adLockOptimistic, adCmdText);
		if(SUCCEEDED(hr))
		{
			var = pRenRecordset->GetCollect((long)0);
			res.CarCount_3 = (int)var.lVal;
			var = pRenRecordset->GetCollect((long)1);
			res.CarSpeed_3 = (int)var.lVal;
			var = pRenRecordset->GetCollect((long)2);
			res.Cross_3 = (double)var.dblVal * 100;
			pRenRecordset->Close();
			pRenRecordset = NULL;
		}

		if(m_fIsQuit)return FALSE;
		hr = pRenRecordset.CreateInstance("ADODB.Recordset");
		if(m_fIsReverse == 1)
		{
			iIndex = -4;
		}
		else
		{
			iIndex = 4;
		}
		strSql.Format("SELECT COUNT(PeriodTime) AS CarCount"
			", AVG(CONVERT(bigint, Car_Speed)) AS CarSpeed "
			", 100*AVG(PreCarCross)/60/10 AS CarCross"
			", CASE WHEN SUM(CASE WHEN Special_Result = 1 THEN 1 ELSE 0 END) > 0 THEN 1 ELSE 0 END AS Violate"
			" FROM %s, %s "
			" WHERE (%s.Pass_Time = %s.Pass_Time AND %s.Record_Id = %s.Record_Id AND %s.Road_No = %d) ",
			strViewName.GetBuffer(), strPeriodTable.GetBuffer(),
			strViewName.GetBuffer(), strPeriodTable.GetBuffer(),
			strViewName.GetBuffer(), strPeriodTable.GetBuffer(),
			strPeriodTable.GetBuffer(), iIndex);
		pRenRecordset->CursorLocation = adUseServer;
		hr = pRenRecordset->Open(_variant_t(strSql), m_pConnect.GetInterfacePtr(), 
			adOpenDynamic, adLockOptimistic, adCmdText);
		if(SUCCEEDED(hr))
		{
			var = pRenRecordset->GetCollect((long)0);
			res.CarCount_4 = (int)var.lVal;
			var = pRenRecordset->GetCollect((long)1);
			res.CarSpeed_4 = (int)var.lVal;
			var = pRenRecordset->GetCollect((long)2);
			res.Cross_4 = (double)var.dblVal * 100;
			pRenRecordset->Close();
			pRenRecordset = NULL;
		}
	}
	catch (_com_error e)
	{
		m_pConnect->Close();
		if(!m_fIsQuit)
		SetWindowText("获取数据失败");
		m_fIsADOConnected = FALSE;
		MessageBox(e.Description());
		
		return FALSE;
	}
	return TRUE;
	//try
	//{
	//	CAxRecordset cRec;
	//	cRec.Create();
	//	cRec.CursorType(adOpenForwardOnly);
	//	cRec.CacheSize(100);
	//	CString strQuery;
	//	strQuery = "IF EXISTS(SELECT * FROM sysobjects WHERE name='Period1Table' AND xtype='U') ";
	//	strQuery += "DROP TABLE Period1Table";
	//	cRec.Open(strQuery, &m_ParentFarm->m_cAxConn);
	//	cRec.Close();

	//	strQuery.Format(
	//		"SELECT Record_Id, Pass_Time, '%s' AS PeriodTime, CASE WHEN Car_Speed=0 THEN 0 ELSE 10/(CONVERT(float, Car_Speed)*1000/3600) END AS PreCarCross \
	//		INTO Period1Table FROM Hve_Data_View WHERE Pass_Time BETWEEN '%s' AND '%s' AND Location_Id=%d ORDER BY Hve_Data_View.Pass_Time",
	//		StartTime, StartTime, EndTime, m_ParentFarm->m_rgLocation[m_ParentFarm->m_nCurrentRoadID].nLocation_Id);
	//	cRec.Open(strQuery, &m_ParentFarm->m_cAxConn);
	//	cRec.Close();

	//	strQuery.Format(
	//		"SELECT COUNT(PeriodTime) AS CarCount"
	//		", AVG(CONVERT(bigint, Car_Speed)) AS CarSpeed "
	//		", 100*AVG(PreCarCross)/60/10 AS CarCross"
	//		", CASE WHEN SUM(CASE WHEN Special_Result = 1 THEN 1 ELSE 0 END) > 0 THEN 1 ELSE 0 END AS Violate"
	//		" FROM Hve_Data_View, Period1Table "
	//		" WHERE (Hve_Data_View.Pass_Time = Period1Table.Pass_Time AND Hve_Data_View.Record_Id = Period1Table.Record_Id AND Hve_Data_View.Road_No = 0) ");
	//	cRec.Open(strQuery, &m_ParentFarm->m_cAxConn);
	//	cRec.FX_Integer(FALSE, "CarCount", res.CarCount_1);
	//	cRec.FX_Integer(FALSE, "CarSpeed", res.CarSpeed_1);
	//	cRec.FX_Float(FALSE, "CarCross", res.Cross_1);
	//	if(res.CarCount_1 <= 0)
	//	{
	//		res.CarCount_1 = 0;
	//		res.CarSpeed_1 = 0;
	//		res.Cross_1 = 0;
	//	}
	//	cRec.Close();

	//	strQuery.Format(
	//		"SELECT COUNT(PeriodTime) AS CarCount"
	//		", AVG(CONVERT(bigint, Car_Speed)) AS CarSpeed "
	//		", 100*AVG(PreCarCross)/60/10 AS CarCross"
	//		", CASE WHEN SUM(CASE WHEN Special_Result = 1 THEN 1 ELSE 0 END) > 0 THEN 1 ELSE 0 END AS Violate"
	//		" FROM Hve_Data_View, Period1Table "
	//		" WHERE (Hve_Data_View.Pass_Time = Period1Table.Pass_Time AND Hve_Data_View.Record_Id = Period1Table.Record_Id AND Hve_Data_View.Road_No = 1) ");
	//	cRec.Open(strQuery, &m_ParentFarm->m_cAxConn);
	//	cRec.FX_Integer(FALSE, "CarCount", res.CarCount_2);
	//	cRec.FX_Integer(FALSE, "CarSpeed", res.CarSpeed_2);
	//	cRec.FX_Float(FALSE, "CarCross", res.Cross_2);
	//	if(res.CarCount_2 <= 0)
	//	{
	//		res.CarCount_2 = 0;
	//		res.CarSpeed_2 = 0;
	//		res.Cross_2 = 0;
	//	}
	//	cRec.Close();

	//	strQuery.Format(
	//		"SELECT COUNT(PeriodTime) AS CarCount"
	//		", AVG(CONVERT(bigint, Car_Speed)) AS CarSpeed "
	//		", 100*AVG(PreCarCross)/60/10 AS CarCross"
	//		", CASE WHEN SUM(CASE WHEN Special_Result = 1 THEN 1 ELSE 0 END) > 0 THEN 1 ELSE 0 END AS Violate"
	//		" FROM Hve_Data_View, Period1Table "
	//		" WHERE (Hve_Data_View.Pass_Time = Period1Table.Pass_Time AND Hve_Data_View.Record_Id = Period1Table.Record_Id AND Hve_Data_View.Road_No = 2) ");
	//	cRec.Open(strQuery, &m_ParentFarm->m_cAxConn);
	//	cRec.FX_Integer(FALSE, "CarCount", res.CarCount_3);
	//	cRec.FX_Integer(FALSE, "CarSpeed", res.CarSpeed_3);
	//	cRec.FX_Float(FALSE, "CarCross", res.Cross_3);
	//	if(res.CarCount_3 <= 0)
	//	{
	//		res.CarCount_3 = 0;
	//		res.CarSpeed_3 = 0;
	//		res.Cross_3 = 0;
	//	}
	//	cRec.Close();

	//	strQuery.Format(
	//		"SELECT COUNT(PeriodTime) AS CarCount"
	//		", AVG(CONVERT(bigint, Car_Speed)) AS CarSpeed "
	//		", 100*AVG(PreCarCross)/60/10 AS CarCross"
	//		", CASE WHEN SUM(CASE WHEN Special_Result = 1 THEN 1 ELSE 0 END) > 0 THEN 1 ELSE 0 END AS Violate"
	//		" FROM Hve_Data_View, Period1Table "
	//		" WHERE (Hve_Data_View.Pass_Time = Period1Table.Pass_Time AND Hve_Data_View.Record_Id = Period1Table.Record_Id AND Hve_Data_View.Road_No = 3) ");
	//	cRec.Open(strQuery, &m_ParentFarm->m_cAxConn);
	//	cRec.FX_Integer(FALSE, "CarCount", res.CarCount_4);
	//	cRec.FX_Integer(FALSE, "CarSpeed", res.CarSpeed_4);
	//	cRec.FX_Float(FALSE, "CarCross", res.Cross_4);
	//	if(res.CarCount_4 <= 0)
	//	{
	//		res.CarCount_4 = 0;
	//		res.CarSpeed_4 = 0;
	//		res.Cross_4 = 0;
	//	}
	//	cRec.Close();
	//}
	//catch (dbAx::CAxException* e)
	//{
	//	delete e;
	//}
}

BOOL CQuery::GetTotleCountByTime(CString StartTime, CString EndTime, int& CarCount, int& CarSpeed, double& Cross)
{
	try
	{
		CString strSql;
		CString strPeriodTable = "strPeriodTable1_";
		strPeriodTable += g_strMacAddr;
		CString strViewName = "Hve_Data_View1_";
		strViewName += g_strMacAddr;
		_variant_t var;
		HRESULT hr = S_OK;
		//strSql = "IF EXISTS(SELECT * FROM sysobjects WHERE name='Period1Table' AND xtype='U') ";
		//strSql += "DROP TABLE Period1Table";
		//_variant_t RecordsetAffected;
		//m_pConnect->Execute((_bstr_t)strSql.GetBuffer(0), &RecordsetAffected, adCmdText);
		//RecordsetAffected.Clear();

		//strSql.Format("SELECT Record_Id, Pass_Time, '%s' AS PeriodTime, CASE WHEN Car_Speed=0 THEN 0 ELSE 10/(CONVERT(float, Car_Speed)*1000/3600) END AS PreCarCross \
		//			  INTO Period1Table FROM Hve_Data_View WHERE Pass_Time BETWEEN '%s' AND '%s' AND Location_Id=%d ORDER BY Hve_Data_View.Pass_Time",
		//			  StartTime, StartTime, EndTime, m_ParentFarm->m_rgLocation[m_ParentFarm->m_nCurrentRoadID].nLocation_Id);
		//m_pConnect->Execute((_bstr_t)strSql.GetBuffer(0), &RecordsetAffected, adCmdText);
		//RecordsetAffected.Clear();
		if(m_fIsQuit)return FALSE;
		//_RecordsetPtr pRenRecordset;
		hr = pRenRecordset.CreateInstance("ADODB.Recordset");
		strSql.Format("SELECT " //COUNT(PeriodTime) AS CarCount
					"AVG(CONVERT(bigint, Car_Speed)) AS CarSpeed "
					", 100*AVG(PreCarCross)/60/10 AS CarCross"
					//", CASE WHEN SUM(CASE WHEN Special_Result = 1 THEN 1 ELSE 0 END) > 0 THEN 1 ELSE 0 END AS Violate"
					" FROM %s, %s "
					" WHERE (%s.Pass_Time = %s.Pass_Time AND %s.Record_Id = %s.Record_Id ) ",
					strViewName.GetBuffer(), strPeriodTable.GetBuffer(),
					strViewName.GetBuffer(), strPeriodTable.GetBuffer(),
					strViewName.GetBuffer(), strPeriodTable.GetBuffer());
		pRenRecordset->CursorLocation = adUseServer;
		hr = pRenRecordset->Open(_variant_t(strSql), m_pConnect.GetInterfacePtr(), 
			adOpenDynamic, adLockOptimistic, adCmdText);
		if(SUCCEEDED(hr))
		{
			//var = pRenRecordset->GetCollect((long)0);
			CarCount = 0;//(int)var.lVal;
			var = pRenRecordset->GetCollect((long)0);
			CarSpeed = (int)var.lVal;
			var = pRenRecordset->GetCollect((long)1);
			Cross = (double)var.dblVal * 100;
			pRenRecordset->Close();
			pRenRecordset = NULL;
		}
	}
	catch (_com_error e)
	{
		m_pConnect->Close();
		if(!m_fIsQuit)
		SetWindowText("获取数据失败");
		m_fIsADOConnected = FALSE;
		return FALSE;
	}
	return TRUE;
	//try
	//{
	//	CAxRecordset cRec;
	//	cRec.Create();
	//	cRec.CursorType(adOpenForwardOnly);
	//	cRec.CacheSize(100);
	//	CString strQuery;
	//	strQuery = "IF EXISTS(SELECT * FROM sysobjects WHERE name='Period1Table' AND xtype='U') ";
	//	strQuery += "DROP TABLE Period1Table";
	//	cRec.Open(strQuery, &m_ParentFarm->m_cAxConn);
	//	cRec.Close();

	//	strQuery.Format(
	//		"SELECT Record_Id, Pass_Time, '%s' AS PeriodTime, CASE WHEN Car_Speed=0 THEN 0 ELSE 10/(CONVERT(float, Car_Speed)*1000/3600) END AS PreCarCross \
	//		INTO Period1Table FROM Hve_Data_View WHERE Pass_Time BETWEEN '%s' AND '%s' AND Location_Id=%d ORDER BY Hve_Data_View.Pass_Time",
	//		StartTime, StartTime, EndTime, m_ParentFarm->m_rgLocation[m_ParentFarm->m_nCurrentRoadID].nLocation_Id);
	//	cRec.Open(strQuery, &m_ParentFarm->m_cAxConn);
	//	cRec.Close();

	//	strQuery.Format(
	//		"SELECT COUNT(PeriodTime) AS CarCount"
	//		", AVG(CONVERT(bigint, Car_Speed)) AS CarSpeed "
	//		", 100*AVG(PreCarCross)/60/10 AS CarCross"
	//		", CASE WHEN SUM(CASE WHEN Special_Result = 1 THEN 1 ELSE 0 END) > 0 THEN 1 ELSE 0 END AS Violate"
	//		" FROM Hve_Data_View, Period1Table "
	//		" WHERE (Hve_Data_View.Pass_Time = Period1Table.Pass_Time AND Hve_Data_View.Record_Id = Period1Table.Record_Id ) ");
	//	cRec.Open(strQuery, &m_ParentFarm->m_cAxConn);
	//	cRec.FX_Integer(FALSE, "CarCount", CarCount);
	//	cRec.FX_Integer(FALSE, "CarSpeed", CarSpeed);
	//	cRec.FX_Float(FALSE, "CarCross", Cross);
	//	if(CarCount <= 0)
	//	{
	//		CarCount = 0;
	//		CarSpeed = 0;
	//		Cross = 0;
	//	}
	//	cRec.Close();
	//}
	//catch (dbAx::CAxException* e)
	//{
	//	delete e;
	//}
}

UINT CQuery::GetDayDataThread(LPVOID userData)
{
	CQuery* dlg = (CQuery*)userData;
	while(dlg->m_fIsGetDate)
	{
		COleDateTime DayTime;
		dlg->m_Data.GetTime(DayTime);
		CString strStartTime, strEndTime;
		QueryStruc rec;
		INT Index;
		dlg->m_ListReport.DeleteAllItems();
		dlg->m_EditSun.SetWindowText("");
		dlg->m_EditSpeed.SetWindowText("");
		dlg->m_EditCross.SetWindowText("");
		int ConnectTime = 0;
		int GetErrorTime = 0;
		int RealCount1 = 0,
			RealCount2 = 0,
			RealCount3 = 0,
			RealCount4 = 0;

		double dblSpeedCount1 = 0;
		double dblSpeedCount2 = 0;
		double dblSpeedCount3 = 0;
		double dblSpeedCount4 = 0;

		double dblCrossCount1 = 0.0;
		double dblCrossCount2 = 0.0;
		double dblCrossCount3 = 0.0;
		double dblCrossCount4 = 0.0;

		// check data exist or not
		CString strSelDay = DayTime.Format("Hve_Data_%Y%m%d");
		dlg->ConnectDataBase();
		if (FALSE == dlg->IsTableLikeThisExists(strSelDay))
		{
			dlg->MessageBox("所选时间无数据!", "错误", MB_ICONERROR);
			goto EXITTHREAD;
		}


		for(Index=0; Index<24; Index++)
		{
			if(!dlg->m_fIsGetDate)goto EXITTHREAD;
			for(ConnectTime=0; ConnectTime<5; ConnectTime++)
			{
				if(dlg->ConnectDataBase())break;
			}
			if(ConnectTime >= 5)
			{
				dlg->MessageBox("查询数据失败，统计日报表失败!", "错误", MB_OK | MB_ICONERROR);
				dlg->m_nReportType = 5;
				goto EXITTHREAD;
			}
			int year, moth, day;
			year = DayTime.GetYear();
			moth = DayTime.GetMonth();
			day = DayTime.GetDay();
			COleDateTime cTmpStartTime, cTmpEndTime;
			cTmpStartTime.SetDateTime(year, moth, day, Index, 0, 0);
			cTmpEndTime.SetDateTime(year, moth, day, Index, 59, 59);
			dlg->CreateView(cTmpStartTime, cTmpEndTime);
			strStartTime.Format("%d%d%d%d-%d%d-%d%d %d%d:%d%d:%d%d",
				year/1000, (year%1000)/100, (year%100)/10, year%10,
				moth/10, moth%10, day/10, day%10, Index/10, Index%10,
				0, 0, 0, 0);
			strEndTime.Format("%d%d%d%d-%d%d-%d%d %d%d:%d%d:%d%d",
				year/1000, (year%1000)/100, (year%100)/10, year%10,
				moth/10, moth%10, day/10, day%10, Index/10, Index%10,
				5, 9, 5, 9);
			if(!dlg->GetCountByTime(strStartTime, strEndTime, rec))
			{
				Index--;
				GetErrorTime++;
				if(GetErrorTime >= 5 || !dlg->m_fIsGetDate)
				{
					goto EXITTHREAD;
				}
				continue;
			}
			dlg->DropView();
			if(!dlg->m_fIsGetDate) goto EXITTHREAD;
			GetErrorTime = 0;
			char szTmp[30];
			sprintf(szTmp, "%d", Index);
			dlg->m_ListReport.InsertItem(Index, "", 0);
			dlg->m_ListReport.SetItemText(Index, 0, szTmp);
			sprintf(szTmp, "%d", rec.CarCount_1);
			dlg->m_ListReport.SetItemText(Index, 1, szTmp);
			sprintf(szTmp, "%d", rec.CarSpeed_1);
			dlg->m_ListReport.SetItemText(Index, 3, szTmp);
			sprintf(szTmp, "%.3f", rec.Cross_1);
			dlg->m_ListReport.SetItemText(Index, 2, szTmp);
			RealCount1 += rec.CarCount_1;
			dblSpeedCount1 += rec.CarCount_1 * rec.CarSpeed_1;
			dblCrossCount1 += rec.CarCount_1 * rec.Cross_1;

			sprintf(szTmp, "%d", rec.CarCount_2);
			dlg->m_ListReport.SetItemText(Index, 4, szTmp);
			sprintf(szTmp, "%d", rec.CarSpeed_2);
			dlg->m_ListReport.SetItemText(Index, 6, szTmp);
			sprintf(szTmp, "%.3f", rec.Cross_2);
			dlg->m_ListReport.SetItemText(Index, 5, szTmp);
			RealCount2 += rec.CarCount_2;
			dblSpeedCount2 += rec.CarCount_2 * rec.CarSpeed_2;
			dblCrossCount2 += rec.CarCount_2 * rec.Cross_2;

			sprintf(szTmp, "%d", rec.CarCount_3);
			dlg->m_ListReport.SetItemText(Index, 7, szTmp);
			sprintf(szTmp, "%d", rec.CarSpeed_3);
			dlg->m_ListReport.SetItemText(Index, 9, szTmp);
			sprintf(szTmp, "%.3f", rec.Cross_3);
			dlg->m_ListReport.SetItemText(Index, 8, szTmp);
			RealCount3 += rec.CarCount_3;
			dblSpeedCount3 += rec.CarCount_3 * rec.CarSpeed_3;
			dblCrossCount3 += rec.CarCount_3 * rec.Cross_3;

			sprintf(szTmp, "%d", rec.CarCount_4);
			dlg->m_ListReport.SetItemText(Index, 10, szTmp);
			sprintf(szTmp, "%d", rec.CarSpeed_4);
			dlg->m_ListReport.SetItemText(Index, 12, szTmp);
			sprintf(szTmp, "%.3f", rec.Cross_4);
			dlg->m_ListReport.SetItemText(Index, 11, szTmp);
			RealCount4 += rec.CarCount_4;
			dblSpeedCount4 += rec.CarCount_4 * rec.CarSpeed_4;
			dblCrossCount4 += rec.CarCount_4 * rec.Cross_4;

			sprintf(szTmp, "%d", rec.CarCount_1 + rec.CarCount_2 + rec.CarCount_3 + rec.CarCount_4);
			dlg->m_ListReport.SetItemText(Index, 13, szTmp);

// 			dlg->SetChartData(Index+1,rec.CarCount_1, rec.CarCount_2, rec.CarCount_3, rec.CarCount_4, 
// 				rec.CarCount_1 + rec.CarCount_2 + rec.CarCount_3 + rec.CarCount_4);
			
			if(!dlg->m_fIsGetDate)break;
		}
		if(!dlg->m_fIsGetDate)break;

		char szTmp[30];
		dlg->m_ListReport.InsertItem(Index, "", 0);
		dlg->m_ListReport.SetItemText(Index, 0, "统计：");
		sprintf(szTmp, "%d", RealCount1);
		dlg->m_ListReport.SetItemText(Index, 1, szTmp);
		sprintf(szTmp, "%d", (RealCount1>0) ? (int)(dblSpeedCount1/RealCount1+0.5) : 0);
		dlg->m_ListReport.SetItemText(Index, 3, szTmp);
		sprintf(szTmp, "%.3f", (RealCount1>0) ? (dblCrossCount1/RealCount1) : 0);
		dlg->m_ListReport.SetItemText(Index, 2, szTmp);

		sprintf(szTmp, "%d", RealCount2);
		dlg->m_ListReport.SetItemText(Index, 4, szTmp);
		sprintf(szTmp, "%d", (RealCount2>0) ? (int)(dblSpeedCount2/RealCount2+0.5) : 0);
		dlg->m_ListReport.SetItemText(Index, 6, szTmp);
		sprintf(szTmp, "%.3f", (RealCount2>0) ? (dblCrossCount2/RealCount2) : 0);
		dlg->m_ListReport.SetItemText(Index, 5, szTmp);

		sprintf(szTmp, "%d", RealCount3);
		dlg->m_ListReport.SetItemText(Index, 7, szTmp);
		sprintf(szTmp, "%d", (RealCount3>0) ? (int)(dblSpeedCount3/RealCount3+0.5) : 0);
		dlg->m_ListReport.SetItemText(Index, 9, szTmp);
		sprintf(szTmp, "%.3f", (RealCount3>0) ? (dblCrossCount3/RealCount3) : 0);
		dlg->m_ListReport.SetItemText(Index, 8, szTmp);

		sprintf(szTmp, "%d", RealCount4);
		dlg->m_ListReport.SetItemText(Index, 10, szTmp);
		sprintf(szTmp, "%d", (RealCount4>0) ? (int)(dblSpeedCount4/RealCount4+0.5) : 0);
		dlg->m_ListReport.SetItemText(Index, 12, szTmp);
		sprintf(szTmp, "%.3f", (RealCount4>0) ? (dblCrossCount1/RealCount4) : 0);
		dlg->m_ListReport.SetItemText(Index, 11, szTmp);

		sprintf(szTmp, "%d", RealCount1 + RealCount2 + RealCount3 + RealCount4);
		dlg->m_ListReport.SetItemText(Index, 13, szTmp);



		if(!dlg->m_fIsGetDate)break;
		int CarCount = 0;
		double CarSpeedCount = 0.0;
		double CrossCount = 0.0;
		CarCount = RealCount1 + RealCount2 + RealCount3 + RealCount4;
		sprintf(szTmp, "%d", CarCount);
		dlg->m_EditSun.SetWindowText(szTmp);

		CarSpeedCount = dblSpeedCount1 + dblSpeedCount2 + dblSpeedCount3 + dblSpeedCount4;
		sprintf(szTmp, "%d", (CarCount>0) ? (int)(CarSpeedCount/CarCount+0.5) : 0);
		dlg->m_EditSpeed.SetWindowText(szTmp);

		CrossCount = dblCrossCount1 + dblCrossCount2 + dblCrossCount3 + dblCrossCount4;
		sprintf(szTmp, "%.3f", (CarCount>0) ? (CrossCount/CarCount) : 0);
		dlg->m_EditCross.SetWindowText(szTmp);

		// draw chart by zhut 20121207
		int iChartItemCount = dlg->m_ListReport.GetItemCount()-1;
		int iChartCar=0;
		int iChartCar1=0;
		int iChartCar2=0;
		int iChartCar3=0;
		int iChartCar4=0;

		dlg->m_Chart.SetColumn(5);
		dlg->m_Chart.SetColumnLabel((LPCTSTR)"断面流量");
		dlg->InitChartData(iChartItemCount);

		for (int i=0; i<iChartItemCount; i++)
		{
			iChartCar1 = StrToInt(dlg->m_ListReport.GetItemText(i, 1));
			iChartCar2 = StrToInt(dlg->m_ListReport.GetItemText(i, 4));
			iChartCar3 = StrToInt(dlg->m_ListReport.GetItemText(i, 7));
			iChartCar4 = StrToInt(dlg->m_ListReport.GetItemText(i, 10));
			iChartCar = StrToInt(dlg->m_ListReport.GetItemText(i, 13));

			dlg->SetChartData(i+1, iChartCar1, iChartCar2, iChartCar3, iChartCar4, iChartCar);
		}

		break;
	}
EXITTHREAD:	dlg->m_fIsADOConnected = FALSE;
	dlg->m_fIsGetDate = FALSE;
	dlg->m_GetDateThread = NULL;
	if(dlg->m_nReportType != 5)dlg->m_nReportType = 1;
	if(!dlg->m_fIsQuit)
	dlg->SendMessage(WM_SEARCHFINISH, 0, 0);
	return 0;
}

UINT CQuery::GetMothDataThread(LPVOID userData)
{
	CQuery* dlg = (CQuery*)userData;
	while(dlg->m_fIsGetDate)
	{
		char szTimeStr[10];
		int year;
		int moth;
		int dayCount;
		dlg->m_MothYear.GetWindowText(szTimeStr, 10);
		year = atoi(szTimeStr);
		dlg->m_MothMoth.GetWindowText(szTimeStr, 10);
		moth = atoi(szTimeStr);
		dayCount = dlg->GetMothDayCount(year, moth);
		CString strStartTime, strEndTime;
		QueryStruc rec;
		dlg->m_ListReport.DeleteAllItems();
		dlg->m_EditSun.SetWindowText("");
		dlg->m_EditSpeed.SetWindowText("");
		dlg->m_EditCross.SetWindowText("");
		int ConnectTime = 0;
		int GetErrorTime = 0;
		int RealGetCount1 = 0,
			RealGetCount2 = 0,
			RealGetCount3 = 0,
			RealGetCount4 = 0;

		double dblSpeedCount1 = 0;
		double dblSpeedCount2 = 0;
		double dblSpeedCount3 = 0;
		double dblSpeedCount4 = 0;

		double dblCrossCount1 = 0.0;
		double dblCrossCount2 = 0.0;
		double dblCrossCount3 = 0.0;
		double dblCrossCount4 = 0.0;


		// check data exist or not
		CString strSelMonth;
		strSelMonth.Format("Hve_Data_%d%02d", year, moth);
		dlg->ConnectDataBase();
		if (FALSE == dlg->IsTableLikeThisExists(strSelMonth))
		{
			dlg->MessageBox("所选时间无数据!", "错误", MB_ICONERROR);
			goto EXITTHREAD1;
		}

		for(int Index=0; Index<dayCount; Index++)
		{
			if(dlg->m_fIsQuit)goto EXITTHREAD1;
			for(ConnectTime=0; ConnectTime<5; ConnectTime++)
			{
				if(dlg->ConnectDataBase())break;
			}
			if(ConnectTime >= 5)
			{
				dlg->MessageBox("查询数据失败，统计月报表失败!", "错误", MB_OK | MB_ICONERROR);
				dlg->m_nReportType = 5;
				goto EXITTHREAD1;
			}
			COleDateTime cTmpStartTime, cTmpEndTime;
			cTmpStartTime.SetDateTime(year, moth, Index+1, 0, 0, 0);
			cTmpEndTime.SetDateTime(year, moth, Index+1, 23, 59, 59);
			dlg->CreateView(cTmpStartTime, cTmpEndTime);
			strStartTime.Format("%d%d%d%d-%d%d-%d%d %d%d:%d%d:%d%d",
				year/1000, (year%1000)/100, (year%100)/10, year%10,
				moth/10, moth%10, (Index+1)/10, (Index+1)%10, 0, 0,
				0, 0, 0, 0);
			strEndTime.Format("%d%d%d%d-%d%d-%d%d %d%d:%d%d:%d%d",
				year/1000, (year%1000)/100, (year%100)/10, year%10,
				moth/10, moth%10, (Index+1)/10, (Index+1)%10, 2, 3,
				5, 9, 5, 9);
			if(!dlg->GetCountByTime(strStartTime, strEndTime, rec))
			{
				Index--;
				GetErrorTime++;
				if(GetErrorTime >= 5 || dlg->m_fIsQuit)
				{
					goto EXITTHREAD1;
				}
				continue;
			}
			dlg->DropView();
			if(dlg->m_fIsQuit) goto EXITTHREAD1;
			GetErrorTime = 0;
			char szTmp[30];
			sprintf(szTmp, "%d", Index+1);
			dlg->m_ListReport.InsertItem(Index, "", 0);
			dlg->m_ListReport.SetItemText(Index, 0, szTmp);
			sprintf(szTmp, "%d", rec.CarCount_1);
			dlg->m_ListReport.SetItemText(Index, 1, szTmp);
			sprintf(szTmp, "%d", rec.CarSpeed_1);
			dlg->m_ListReport.SetItemText(Index, 3, szTmp);
			sprintf(szTmp, "%.3f", rec.Cross_1);
			dlg->m_ListReport.SetItemText(Index, 2, szTmp);
			RealGetCount1 += rec.CarCount_1;
			dblSpeedCount1 += rec.CarCount_1 * rec.CarSpeed_1;
			dblCrossCount1 += rec.CarCount_1 * rec.Cross_1;

			sprintf(szTmp, "%d", rec.CarCount_2);
			dlg->m_ListReport.SetItemText(Index, 4, szTmp);
			sprintf(szTmp, "%d", rec.CarSpeed_2);
			dlg->m_ListReport.SetItemText(Index, 6, szTmp);
			sprintf(szTmp, "%.3f", rec.Cross_2);
			dlg->m_ListReport.SetItemText(Index, 5, szTmp);
			RealGetCount2 += rec.CarCount_2;
			dblSpeedCount2 += rec.CarCount_2 * rec.CarSpeed_2;
			dblCrossCount2 += rec.CarCount_2 * rec.Cross_2;

			sprintf(szTmp, "%d", rec.CarCount_3);
			dlg->m_ListReport.SetItemText(Index, 7, szTmp);
			sprintf(szTmp, "%d", rec.CarSpeed_3);
			dlg->m_ListReport.SetItemText(Index, 9, szTmp);
			sprintf(szTmp, "%.3f", rec.Cross_3);
			dlg->m_ListReport.SetItemText(Index, 8, szTmp);
			RealGetCount3 += rec.CarCount_3;
			dblSpeedCount3 += rec.CarCount_3 * rec.CarSpeed_3;
			dblCrossCount3 += rec.CarCount_3 * rec.Cross_3;

			sprintf(szTmp, "%d", rec.CarCount_4);
			dlg->m_ListReport.SetItemText(Index, 10, szTmp);
			sprintf(szTmp, "%d", rec.CarSpeed_4);
			dlg->m_ListReport.SetItemText(Index, 12, szTmp);
			sprintf(szTmp, "%.3f", rec.Cross_4);
			dlg->m_ListReport.SetItemText(Index, 11, szTmp);
			RealGetCount4 += rec.CarCount_4;
			dblSpeedCount4 += rec.CarCount_4 * rec.CarSpeed_4;
			dblCrossCount4 += rec.CarCount_4 * rec.Cross_4;

			sprintf(szTmp, "%d", rec.CarCount_1 + rec.CarCount_2 + rec.CarCount_3 + rec.CarCount_4);
			dlg->m_ListReport.SetItemText(Index, 13, szTmp);

// 			dlg->SetChartData(Index+1,rec.CarCount_1, rec.CarCount_2, rec.CarCount_3, rec.CarCount_4, 
// 				rec.CarCount_1 + rec.CarCount_2 + rec.CarCount_3 + rec.CarCount_4);

			if(!dlg->m_fIsGetDate)break;
		}
		if(!dlg->m_fIsGetDate)break;
		if(dlg->m_fIsQuit)break;

		char szTmp[30];
		dlg->m_ListReport.InsertItem(Index, "", 0);
		dlg->m_ListReport.SetItemText(Index, 0, "统计：");
		sprintf(szTmp, "%d", RealGetCount1);
		dlg->m_ListReport.SetItemText(Index, 1, szTmp);
		sprintf(szTmp, "%d", (RealGetCount1>0) ? (int)(dblSpeedCount1/RealGetCount1+0.5) : 0);
		dlg->m_ListReport.SetItemText(Index, 3, szTmp);
		sprintf(szTmp, "%.3f", (RealGetCount1>0) ? (dblCrossCount1/RealGetCount1) : 0);
		dlg->m_ListReport.SetItemText(Index, 2, szTmp);

		sprintf(szTmp, "%d", RealGetCount2);
		dlg->m_ListReport.SetItemText(Index, 4, szTmp);
		sprintf(szTmp, "%d", (RealGetCount2>0) ? (int)(dblSpeedCount2/RealGetCount2+0.5) : 0);
		dlg->m_ListReport.SetItemText(Index, 6, szTmp);
		sprintf(szTmp, "%.3f", (RealGetCount2>0) ? (dblCrossCount2/RealGetCount2) : 0);
		dlg->m_ListReport.SetItemText(Index, 5, szTmp);

		sprintf(szTmp, "%d", RealGetCount3);
		dlg->m_ListReport.SetItemText(Index, 7, szTmp);
		sprintf(szTmp, "%d", (RealGetCount3>0) ? (int)(dblSpeedCount3/RealGetCount3+0.5) : 0);
		dlg->m_ListReport.SetItemText(Index, 9, szTmp);
		sprintf(szTmp, "%.3f", (RealGetCount3>0) ? (dblCrossCount3/RealGetCount3) : 0);
		dlg->m_ListReport.SetItemText(Index, 8, szTmp);

		sprintf(szTmp, "%d", RealGetCount4);
		dlg->m_ListReport.SetItemText(Index, 10, szTmp);
		sprintf(szTmp, "%d", (RealGetCount4>0) ? (int)(dblSpeedCount4/RealGetCount4+0.5) : 0);
		dlg->m_ListReport.SetItemText(Index, 12, szTmp);
		sprintf(szTmp, "%.3f", (RealGetCount4>0) ? (dblCrossCount4/RealGetCount4) : 0);
		dlg->m_ListReport.SetItemText(Index, 11, szTmp);

		sprintf(szTmp, "%d", RealGetCount1 + RealGetCount2 + RealGetCount3 + RealGetCount4);
		dlg->m_ListReport.SetItemText(Index, 13, szTmp);



		if(!dlg->m_fIsGetDate)break;
		int CarCount = 0;
		double CarSpeedCount = 0.0;
		double CrossCount = 0.0;

		if(dlg->m_fIsQuit)break;

		CarCount = RealGetCount1 + RealGetCount2 + RealGetCount3 + RealGetCount4;
		sprintf(szTmp, "%d", CarCount);
		dlg->m_EditSun.SetWindowText(szTmp);

		CarSpeedCount = dblSpeedCount1 + dblSpeedCount2 + dblSpeedCount3 + dblSpeedCount4;
		sprintf(szTmp, "%d", (CarCount>0) ? (int)(CarSpeedCount/CarCount+0.5) : 0);
		dlg->m_EditSpeed.SetWindowText(szTmp);

		CrossCount = dblCrossCount1 + dblCrossCount2 + dblCrossCount3 + dblCrossCount4;
		sprintf(szTmp, "%.3f", (CarCount>0) ? (CrossCount/CarCount) : 0);
		dlg->m_EditCross.SetWindowText(szTmp);

		// draw chart by zhut 20121207
		int iChartItemCount = dlg->m_ListReport.GetItemCount()-1;
		int iChartCar=0;
		int iChartCar1=0;
		int iChartCar2=0;
		int iChartCar3=0;
		int iChartCar4=0;

		dlg->m_Chart.SetColumn(5);
		dlg->m_Chart.SetColumnLabel((LPCTSTR)"每天流量");
		dlg->InitChartData(iChartItemCount);

		for (int i=0; i<iChartItemCount; i++)
		{
			iChartCar1 = StrToInt(dlg->m_ListReport.GetItemText(i, 1));
			iChartCar2 = StrToInt(dlg->m_ListReport.GetItemText(i, 4));
			iChartCar3 = StrToInt(dlg->m_ListReport.GetItemText(i, 7));
			iChartCar4 = StrToInt(dlg->m_ListReport.GetItemText(i, 10));
			iChartCar = StrToInt(dlg->m_ListReport.GetItemText(i, 13));

			dlg->SetChartData(i+1, iChartCar1, iChartCar2, iChartCar3, iChartCar4, iChartCar);
		}


		break;
	}
EXITTHREAD1: dlg->m_fIsADOConnected = FALSE;
	dlg->m_fIsGetDate = FALSE;
	dlg->m_GetDateThread = NULL;
	if(dlg->m_nReportType != 5)dlg->m_nReportType = 2;
	if(!dlg->m_fIsQuit)
	dlg->SendMessage(WM_SEARCHFINISH, 0, 0);
	return 0;
}

int CQuery::GetMothDayCount(int year, int moth)
{
	int dayCount = 0;
	if(moth == 2)
	{
		if((year%4==0 && year%100!=0) || (year%400==0))
			dayCount = 29;
		else
			dayCount = 28;
	}
	switch(moth)
	{
	case 1:
	case 3:
	case 5:
	case 7:
	case 8:
	case 10:
	case 12:
		dayCount = 31;
		break;
	case 4:
	case 6:
	case 9:
	case 11:
		dayCount = 30;
		break;
	}
	return dayCount;
}

UINT CQuery::GetYearDataThread(LPVOID userData)
{
	CQuery* dlg = (CQuery*)userData;
	while(dlg->m_fIsGetDate)
	{
		char szTimeStr[10];
		int year;
		int dayCount;
		dlg->m_Year.GetWindowText(szTimeStr, 10);
		year = atoi(szTimeStr);
		CString strStartTime, strEndTime;
		QueryStruc rec;
		dlg->m_ListReport.DeleteAllItems();
		dlg->m_EditSun.SetWindowText("");
		dlg->m_EditSpeed.SetWindowText("");
		dlg->m_EditCross.SetWindowText("");
		int ConnectTime = 0;
		int GetErrorCount = 0;
		int RealCarCount1 = 0,
			RealCarCount2 = 0,
			RealCarCount3 = 0,
			RealCarCount4 = 0;

		double dblSpeedCount1 = 0;
		double dblSpeedCount2 = 0;
		double dblSpeedCount3 = 0;
		double dblSpeedCount4 = 0;

		double dblCrossCount1 = 0.0;
		double dblCrossCount2 = 0.0;
		double dblCrossCount3 = 0.0;
		double dblCrossCount4 = 0.0;


		// check data exist or not
		CString strSelYear;
		strSelYear.Format("Hve_Data_%d", year);
		dlg->ConnectDataBase();
		if (FALSE == dlg->IsTableLikeThisExists(strSelYear))
		{
			dlg->MessageBox("所选时间无数据!", "错误", MB_ICONERROR);
			goto EXITTHREAD2;
		}

		for(int index=0; index<12; index++)
		{
			if(dlg->m_fIsQuit)goto EXITTHREAD2;
			for(ConnectTime=0; ConnectTime<5; ConnectTime++)
			{
				if(dlg->ConnectDataBase())break;
			}
			if(ConnectTime >= 5)
			{
				dlg->MessageBox("查询数据失败，统计年报表失败!", "错误", MB_OK | MB_ICONERROR);
				dlg->m_nReportType = 5;
				goto EXITTHREAD2;
			}
			dayCount = dlg->GetMothDayCount(year, index+1);
			COleDateTime cTmpStartTime, cTmpEndTime;
			cTmpStartTime.SetDateTime(year, index+1, 1, 0, 0, 0);
			cTmpEndTime.SetDateTime(year, index+1, dayCount, 23, 59, 59);
			dlg->CreateView(cTmpStartTime, cTmpEndTime);
			strStartTime.Format("%d%d%d%d-%d%d-%d%d %d%d:%d%d:%d%d",
				year/1000, (year%1000)/100, (year%100)/10, year%10,
				(index+1)/10, (index+1)%10, 0, 1, 0, 0,
				0, 0, 0, 0);
			strEndTime.Format("%d%d%d%d-%d%d-%d%d %d%d:%d%d:%d%d",
				year/1000, (year%1000)/100, (year%100)/10, year%10,
				(index+1)/10, (index+1)%10, dayCount/10, dayCount%10, 2, 3,
				5, 9, 5, 9);
			if(!dlg->GetCountByTime(strStartTime, strEndTime, rec))
			{
				index--;
				GetErrorCount++;
				if(GetErrorCount >= 5 || dlg->m_fIsQuit)
				{
					goto EXITTHREAD2;
				}
				continue;
			}
			dlg->DropView();
			if(dlg->m_fIsQuit)goto EXITTHREAD2;
			GetErrorCount = 0;
			char szTmp[30];
			sprintf(szTmp, "%d", index+1);
			dlg->m_ListReport.InsertItem(index, "", 0);
			dlg->m_ListReport.SetItemText(index, 0, szTmp);
			sprintf(szTmp, "%d", rec.CarCount_1);
			dlg->m_ListReport.SetItemText(index, 1, szTmp);
			sprintf(szTmp, "%d", rec.CarSpeed_1);
			dlg->m_ListReport.SetItemText(index, 3, szTmp);
			sprintf(szTmp, "%.3f", rec.Cross_1);
			dlg->m_ListReport.SetItemText(index, 2, szTmp);
			RealCarCount1 += rec.CarCount_1;
			dblSpeedCount1 += rec.CarCount_1 * rec.CarSpeed_1;
			dblCrossCount1 += rec.CarCount_1 * rec.Cross_1;

			sprintf(szTmp, "%d", rec.CarCount_2);
			dlg->m_ListReport.SetItemText(index, 4, szTmp);
			sprintf(szTmp, "%d", rec.CarSpeed_2);
			dlg->m_ListReport.SetItemText(index, 6, szTmp);
			sprintf(szTmp, "%.3f", rec.Cross_2);
			dlg->m_ListReport.SetItemText(index, 5, szTmp);
			RealCarCount2 += rec.CarCount_2;
			dblSpeedCount2 += rec.CarCount_2 * rec.CarSpeed_2;
			dblCrossCount2 += rec.CarCount_2 * rec.Cross_2;

			sprintf(szTmp, "%d", rec.CarCount_3);
			dlg->m_ListReport.SetItemText(index, 7, szTmp);
			sprintf(szTmp, "%d", rec.CarSpeed_3);
			dlg->m_ListReport.SetItemText(index, 9, szTmp);
			sprintf(szTmp, "%.3f", rec.Cross_3);
			dlg->m_ListReport.SetItemText(index, 8, szTmp);
			RealCarCount3 += rec.CarCount_3;
			dblSpeedCount3 += rec.CarCount_3 * rec.CarSpeed_3;
			dblCrossCount3 += rec.CarCount_3 * rec.Cross_3;

			sprintf(szTmp, "%d", rec.CarCount_4);
			dlg->m_ListReport.SetItemText(index, 10, szTmp);
			sprintf(szTmp, "%d", rec.CarSpeed_4);
			dlg->m_ListReport.SetItemText(index, 12, szTmp);
			sprintf(szTmp, "%.3f", rec.Cross_4);
			dlg->m_ListReport.SetItemText(index, 11, szTmp);
			RealCarCount4 += rec.CarCount_4;
			dblSpeedCount4 += rec.CarCount_4 * rec.CarSpeed_4;
			dblCrossCount4 += rec.CarCount_4 * rec.Cross_4;

			sprintf(szTmp, "%d", rec.CarCount_1 + rec.CarCount_2 + rec.CarCount_3 + rec.CarCount_4);
			dlg->m_ListReport.SetItemText(index, 13, szTmp);

// 			dlg->SetChartData(index+1,rec.CarCount_1, rec.CarCount_2, rec.CarCount_3, rec.CarCount_4, 
// 				rec.CarCount_1 + rec.CarCount_2 + rec.CarCount_3 + rec.CarCount_4);

			if(!dlg->m_fIsGetDate)break;
		}


		if(!dlg->m_fIsGetDate)break;
		if(dlg->m_fIsQuit)break;
		
		char szTmp[30];
		dlg->m_ListReport.InsertItem(index, "", 0);
		dlg->m_ListReport.SetItemText(index, 0, "统计：");
		sprintf(szTmp, "%d", RealCarCount1);
		dlg->m_ListReport.SetItemText(index, 1, szTmp);
		sprintf(szTmp, "%d", (RealCarCount1>0) ? (int)(dblSpeedCount1/RealCarCount1+0.5) : 0);
		dlg->m_ListReport.SetItemText(index, 3, szTmp);
		sprintf(szTmp, "%.3f", (RealCarCount1>0) ? (dblCrossCount1/RealCarCount1) : 0);
		dlg->m_ListReport.SetItemText(index, 2, szTmp);

		sprintf(szTmp, "%d", RealCarCount2);
		dlg->m_ListReport.SetItemText(index, 4, szTmp);
		sprintf(szTmp, "%d", (RealCarCount2>0) ? (int)(dblSpeedCount2/RealCarCount2+0.5) : 0);
		dlg->m_ListReport.SetItemText(index, 6, szTmp);
		sprintf(szTmp, "%.3f", (RealCarCount2>0) ? (dblCrossCount2/RealCarCount2) : 0);
		dlg->m_ListReport.SetItemText(index, 5, szTmp);

		sprintf(szTmp, "%d", RealCarCount3);
		dlg->m_ListReport.SetItemText(index, 7, szTmp);
		sprintf(szTmp, "%d", (RealCarCount3>0) ? (int)(dblSpeedCount3/RealCarCount3+0.5) : 0);
		dlg->m_ListReport.SetItemText(index, 9, szTmp);
		sprintf(szTmp, "%.3f", (RealCarCount3>0) ? (dblCrossCount3/RealCarCount3) : 0);
		dlg->m_ListReport.SetItemText(index, 8, szTmp);

		sprintf(szTmp, "%d", RealCarCount4);
		dlg->m_ListReport.SetItemText(index, 10, szTmp);
		sprintf(szTmp, "%d", (RealCarCount4>0) ? (int)(dblSpeedCount4/RealCarCount4+0.5) : 0);
		dlg->m_ListReport.SetItemText(index, 12, szTmp);
		sprintf(szTmp, "%.3f", (RealCarCount4>0) ? (dblCrossCount4/RealCarCount4) : 0);
		dlg->m_ListReport.SetItemText(index, 11, szTmp);

		sprintf(szTmp, "%d", RealCarCount1 + RealCarCount2 + RealCarCount3 + RealCarCount4);
		dlg->m_ListReport.SetItemText(index, 13, szTmp);


		if(!dlg->m_fIsGetDate)break;
		int CarCount = 0;
		double CarSpeedCount = 0.0;
		double CrossCount = 0.0;
		
		CarCount = RealCarCount1 + RealCarCount2 + RealCarCount3 + RealCarCount4;
		sprintf(szTmp, "%d", CarCount);
		dlg->m_EditSun.SetWindowText(szTmp);

		CarSpeedCount = dblSpeedCount1 + dblSpeedCount2 + dblSpeedCount3 + dblSpeedCount4;
		sprintf(szTmp, "%d", (CarCount>0) ? (int)(CarSpeedCount/CarCount+0.5) : 0);
		dlg->m_EditSpeed.SetWindowText(szTmp);

		CrossCount = dblCrossCount1 + dblCrossCount2 + dblCrossCount3 + dblCrossCount4;
		sprintf(szTmp, "%.3f", (CarCount>0) ? (CrossCount/CarCount) : 0);		
		dlg->m_EditCross.SetWindowText(szTmp);

		// draw chart by zhut 20121207
		int iChartItemCount = dlg->m_ListReport.GetItemCount()-1;
		int iChartCar=0;
		int iChartCar1=0;
		int iChartCar2=0;
		int iChartCar3=0;
		int iChartCar4=0;

		dlg->m_Chart.SetColumn(5);
		dlg->m_Chart.SetColumnLabel((LPCTSTR)"每月流量");
		dlg->InitChartData(iChartItemCount);

		for (int i=0; i<iChartItemCount; i++)
		{
			iChartCar1 = StrToInt(dlg->m_ListReport.GetItemText(i, 1));
			iChartCar2 = StrToInt(dlg->m_ListReport.GetItemText(i, 4));
			iChartCar3 = StrToInt(dlg->m_ListReport.GetItemText(i, 7));
			iChartCar4 = StrToInt(dlg->m_ListReport.GetItemText(i, 10));
			iChartCar = StrToInt(dlg->m_ListReport.GetItemText(i, 13));

			dlg->SetChartData(i+1, iChartCar1, iChartCar2, iChartCar3, iChartCar4, iChartCar);
		}

		break;
	}
EXITTHREAD2: dlg->m_fIsADOConnected = FALSE;
	dlg->m_fIsGetDate = FALSE;
	dlg->m_GetDateThread = NULL;
	if(dlg->m_nReportType != 5)dlg->m_nReportType = 3;
	if(!dlg->m_fIsQuit)
	dlg->SendMessage(WM_SEARCHFINISH, 0, 0);
	return 0;
}

void CQuery::OnCancel()
{
	if(m_fIsGetDate || m_fIsProcsee)
	{
		AfxMessageBox(_T("正在查询，请勿关闭"));
		return;
	}

	AfxOleTerm();
	m_fIsGetDate = FALSE;
	m_fIsProcsee = FALSE;
	m_fIsQuit = TRUE;
	if(m_lpback != NULL)
	{
		m_lpback->Release();
		m_lpback = NULL;
	}
	if(m_lpdds)
	{
		m_lpdds->Release();
		m_lpdds = NULL;
	}
	DWORD Time = 0;
	if(m_pConnect)
	m_pConnect->Cancel();
	if(pRenRecordset)
	pRenRecordset->Cancel();
	SetWindowText("正在停止查询数据操作，请稍侯...");
	if(m_GetDateThread)
	{
		m_fIsQuit = TRUE;
		DWORD dwWaitCount = 0;
		while(WaitForSingleObject(m_GetDateThread, 500) == WAIT_TIMEOUT && dwWaitCount < 8)
		{
			dwWaitCount++;
		}
		if(dwWaitCount >= 8)
		{
			TerminateThread(m_GetDateThread, 0);
		}
	}
	Time = 0;
	if(m_GetDateProcessThread)
	{
		m_fIsProcsee = FALSE;
		DWORD dwWaitCount = 0;
		while(WaitForSingleObject(m_GetDateProcessThread, 500) == WAIT_TIMEOUT && dwWaitCount < 8)
		{
			dwWaitCount++;
		}
		if(dwWaitCount >= 8)
		{
			TerminateThread(m_GetDateProcessThread, 0);
		}
	}

	if (m_pBrushChart)
	{
		delete m_pBrushChart;
	}
	m_fontChart.DeleteObject();

	::PostMessage(AfxGetMainWnd()->m_hWnd, WM_DLGENDING, 0, 0);
	CDialog::OnCancel();
	CDialog::DestroyWindow();
}

void CQuery::OnOK()
{

}

UINT CQuery::GetDayProcessThread(LPVOID userData)
{
	CQuery* dlg = (CQuery*)userData;
	dlg->m_Process.ShowWindow(SW_SHOW);
	dlg->m_Process.SetRange(0, 100);
	dlg->m_Process.SetPos(0);
	DWORD Index = 0;
	while(dlg->m_fIsProcsee)
	{
		if(dlg->m_fIsQuit)break;
		dlg->m_Process.SetPos(Index);
		Index++;
		if(Index>=100)Index = 0;
		Sleep(100);
	}
	if(!dlg->m_fIsQuit)
	dlg->m_Process.ShowWindow(SW_HIDE);
	dlg->m_GetDateProcessThread = NULL;
	return 0;
}

LRESULT  CQuery::OnSearchDone(WPARAM wParam, LPARAM lParam)
{
	if(m_fIsQuit) return 0;
	m_fIsProcsee = FALSE;
	Sleep(500);
	if(m_GetDateProcessThread != NULL)
	{
		m_GetDateProcessThread->ExitInstance();
		m_GetDateProcessThread = NULL;
	}
	m_ButtonQuery.EnableWindow(TRUE);
	m_ButtonReset.EnableWindow(TRUE);
	m_ButtonPrint.EnableWindow(TRUE);
	m_ButtonPrintView.EnableWindow(TRUE);
	m_ButtomPrintSet.EnableWindow(TRUE);
	GetDlgItem(IDC_CHECKSHOWCHART)->EnableWindow(TRUE);

	switch(m_nReportType)
	{
	case 1:
		{
			COleDateTime GetTime;
			m_Data.GetTime(GetTime);
			m_StaticReportType.SetWindowText("报表类型：日报表");
			CString szStr;
			int year, moth, day;
			year = GetTime.GetYear();
			moth = GetTime.GetMonth();
			day = GetTime.GetDay();
			szStr.Format("%d%d%d%d年%d%d月%d%d日", 
				year/1000, (year%1000)/100, (year%100)/10, year%10,
				moth/10, moth%10, day/10, day%10);
			m_StaticReportTimes.SetWindowText(szStr.GetBuffer(0));
		}
		break;
	case 2:
		{
			m_StaticReportType.SetWindowText("报表类型：月报表");
			char szTimeStr[10];
			int year;
			int moth;
			m_MothYear.GetWindowText(szTimeStr, 10);
			year = atoi(szTimeStr);
			m_MothMoth.GetWindowText(szTimeStr, 10);
			moth = atoi(szTimeStr);
			CString strTime;
			strTime.Format("%d%d%d%d年%d%d月", 
				year/1000, (year%1000)/100, (year%100)/10, year%10,
				moth/10, moth%10);
			m_StaticReportTimes.SetWindowText(strTime.GetBuffer(0));
		}
		break;
	case 3:
		{
			m_StaticReportType.SetWindowText("报表类型：年报表");
			char szTimeStr[10];
			m_Year.GetWindowText(szTimeStr, 10);
			CString strTime;
			strTime.Format("%s年", szTimeStr);
			m_StaticReportTimes.SetWindowText(strTime.GetBuffer(0));
		}
		break;
	default:
		m_StaticReportType.SetWindowText("生成报表失败");
		m_StaticReportTimes.SetWindowText("");
		break;
	}
	SetWindowText("查询统计报表");
	m_Data.EnableWindow(TRUE);
	m_MothYear.EnableWindow(TRUE);
	m_MothMoth.EnableWindow(TRUE);
	m_Year.EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_RADIO1))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_RADIO2))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_RADIO3))->EnableWindow(TRUE);
	return 0;
}

BOOL CQuery::GetPrintInfo()
{
	if(m_lpdds == NULL)return FALSE;
	if(m_lpback == NULL)
	{
		DDSURFACEDESC2 ddsd;
		ZeroMemory(&ddsd, sizeof(DDSURFACEDESC2));
		ddsd.dwSize = sizeof(DDSURFACEDESC2);
		ddsd.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
		ddsd.dwWidth = 794;
		ddsd.dwHeight = 1123;
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
		m_lpdds->CreateSurface(&ddsd, &m_lpback, NULL);
	}
	if(m_lpback == NULL) return FALSE;

	DDSURFACEDESC2 ddsd2;
	ZeroMemory(&ddsd2, sizeof(DDSURFACEDESC2));
	ddsd2.dwSize = sizeof(DDSURFACEDESC2);
	m_lpback->Lock(NULL, &ddsd2, 0, NULL);
	UCHAR* buffer = (UCHAR*)ddsd2.lpSurface;
	memset(buffer, 255, ddsd2.lPitch * ddsd2.dwHeight);
	m_lpback->Unlock(NULL);

	HDC tmpDC;
	m_lpback->GetDC(&tmpDC);
	HFONT g_hFont = CreateFont(30, 16, 0, 0, FW_HEAVY, false, false, false,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH, "宋体");
	HFONT hFontOld;
	hFontOld = (HFONT)SelectObject(tmpDC, g_hFont);
	CString strTmp = "HVE视频车检器——";
	char szReportType[30];
	m_StaticReportType.GetWindowText(szReportType, 30);
	if(strlen(szReportType) > 10)
		strTmp += (szReportType+10);
	TextOut(tmpDC, 200, 50, strTmp.GetBuffer(0), strTmp.GetLength());
	::DeleteObject(g_hFont);
	g_hFont = CreateFont(19, 9, 0, 0, FW_THIN, false, false, false,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH, "宋体");
	SelectObject(tmpDC, g_hFont);
	strTmp.Format("%s%s", "断面名称：", m_ParentFarm->m_rgLocation[m_ParentFarm->m_nCurrentRoadID].strLocation_Name);
	TextOut(tmpDC, 50, 100, strTmp.GetBuffer(0), strTmp.GetLength());
	m_StaticReportTimes.GetWindowText(szReportType, 30);
	strTmp.Format("日期：%s", szReportType);
	TextOut(tmpDC, 50, 125, strTmp.GetBuffer(0), strTmp.GetLength());
	char szSunCount[10];
	char szSpeed[10];
	char szCross[10];
	m_EditSun.GetWindowText(szSunCount, 10);
	m_EditSpeed.GetWindowText(szSpeed, 10);
	m_EditCross.GetWindowText(szCross, 10);
	strTmp.Format("总车流量：%s辆    平均占有率：%s%%    平均车速：%s公里/小时", szSunCount, szCross, szSpeed);
	TextOut(tmpDC, 50, 150, strTmp.GetBuffer(0), strTmp.GetLength());
	strTmp.Format("车流量单位：辆    占有率单位：%%    车速单位：公里/小时");
	TextOut(tmpDC, 50, 175, strTmp.GetBuffer(0), strTmp.GetLength());
	int Index;
	for(Index=0; Index<35; Index++)
	{
		if(Index == 1)
		{
			MoveToEx(tmpDC, 60, 210+Index*26, NULL);
			LineTo(tmpDC, 720, 210+Index*26);
		}
		else
		{
			MoveToEx(tmpDC, 10, 210+Index*26, NULL);
			LineTo(tmpDC, 784, 210+Index*26);
		}
	}

	MoveToEx(tmpDC, 10, 210, NULL);
	LineTo(tmpDC, 10, 1094);
	MoveToEx(tmpDC, 784, 210, NULL);
	LineTo(tmpDC, 784, 1094);

	for(Index=0; Index<13; Index++)
	{
		if((Index%3) == 0)
			MoveToEx(tmpDC, 60+Index*55, 210, NULL);
		else
			MoveToEx(tmpDC, 60+Index*55, 236, NULL);
		LineTo(tmpDC, 60+Index*55, 1094);

	}
	::DeleteObject(g_hFont);
	g_hFont = CreateFont(16, 7, 0, 0, FW_THIN, false, false, false,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH, "宋体");
	SelectObject(tmpDC, g_hFont);

	TextOut(tmpDC, 30, 228, "时", 2);
	TextOut(tmpDC, 132, 215, "车道1", 5);
	TextOut(tmpDC, 297, 215, "车道2", 5);
	TextOut(tmpDC, 462, 215, "车道3", 5);
	TextOut(tmpDC, 627, 215, "车道4", 5);

	LVCOLUMN    col;     
	col.fmt    =    LVIF_TEXT;   
	col.cchTextMax    =    10;
	col.pszText = new char[10];
	m_ListReport.GetColumn(13, &col);	
	TextOut(tmpDC, 725, 228, col.pszText, 8);
	delete[] col.pszText;

	DeleteObject(g_hFont);
	g_hFont = CreateFont(16, 6, 0, 0, FW_THIN, false, false, false,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH, "宋体");
	SelectObject(tmpDC, g_hFont);
	for(Index=0; Index<4; Index++)
	{
		TextOut(tmpDC, 65+Index*165, 241, "总车流量", 8);
		TextOut(tmpDC, 126+Index*165, 241, "占有率", 6);
		TextOut(tmpDC, 175+Index*165, 241, "平均车速", 8);
	}
	int nInfoCount = m_ListReport.GetItemCount();
	for(Index=0; Index<nInfoCount; Index++)
	{
		char szInfo[10];
		for(int IntemCount=0; IntemCount<14; IntemCount++)
		{
			m_ListReport.GetItemText(Index, IntemCount, szInfo, 10);
			if(IntemCount == 0)
				TextOut(tmpDC, 15, 265+Index*26, szInfo, (int)strlen(szInfo));
			else
				TextOut(tmpDC, 65+(IntemCount-1)*55, 265+Index*26, szInfo, (int)strlen(szInfo));
		}
	}

	DeleteObject(g_hFont);
	SelectObject(tmpDC, hFontOld);
	m_lpback->ReleaseDC(tmpDC);

	return TRUE;
}

void CQuery::OnBnClickedButton5()
{
	if(m_ListReport.GetItemCount() <= 0)
	{
		MessageBox("没有需要打印的报表");
		return;
	}
	m_ButtonPrint.EnableWindow(FALSE);
	if(!GetPrintInfo())
	{
		MessageBox("获取打印数据失败");
		m_ButtonPrint.EnableWindow(TRUE);
		return;
	}

	CDC dc;
	CPrintDialog printDlg(FALSE);
	if(printDlg.DoModal() != IDOK) return;
	if(!dc.Attach(printDlg.GetPrinterDC()))
	{
		MessageBox("找不到有效的打印设备，请检查与打印机设备连接是否正常!", "错误", MB_OK | MB_ICONERROR);
		return;
	}
	dc.m_bPrinting = TRUE;
	DOCINFO di;
	ZeroMemory(&di, sizeof(DOCINFO));
	di.cbSize = sizeof(DOCINFO);
	di.lpszDocName = "打印报表";
	BOOL bPrintingOK = dc.StartDoc(&di);
	CPrintInfo Info;
	Info.SetMaxPage(1);
	int maxw = dc.GetDeviceCaps(HORZRES);
	int maxh = dc.GetDeviceCaps(VERTRES);
	Info.m_rectDraw.SetRect(0, 0, maxw, maxh);
	RECT tmpRect;
	GetClientRect(&tmpRect);
	HDC tmpDC;
	m_lpback->GetDC(&tmpDC);
	for(UINT page = Info.GetMinPage(); page<=Info.GetMaxPage()&&bPrintingOK; page++)
	{
		dc.StartPage();
		dc.SetStretchBltMode(COLORONCOLOR);
		::StretchBlt(dc.m_hDC, 0, 0, maxw, maxh, tmpDC, 0, 0, 794, 1123, SRCCOPY);
		bPrintingOK = (dc.EndPage() > 0);
	}
	m_lpback->ReleaseDC(tmpDC);

// print Chart, by zhut 20121108
	CDC dcChart;
	int iWidth = 0;
	int iHeight = 0;

	GetChartDC(&dcChart, iWidth, iHeight);

	for(UINT page = Info.GetMinPage(); page<=Info.GetMaxPage()&&bPrintingOK; page++)
	{
		dc.StartPage();
		dc.SetStretchBltMode(COLORONCOLOR);
		::StretchBlt(dc.m_hDC, 0, 0, maxw, maxh, dcChart.m_hDC, 0, 0, iWidth, iHeight, SRCCOPY);
		bPrintingOK = (dc.EndPage() > 0);
	}


	if(bPrintingOK)dc.EndDoc();
	else dc.AbortDoc();

	if (NULL == dcChart.m_hDC)
	{
		dcChart.DeleteDC();
	}
	
}

void CQuery::OnBnClickedButton2()
{
	if(m_ListReport.GetItemCount() <= 0)
	{
		MessageBox("没有需要打印的报表");
		return;
	}

	m_PrintfViewDlg = new CPrintfView(this);
	RECT Rect;
	Rect.left = 20;
	Rect.right = 660;
	Rect.top = 20;
	Rect.bottom = 500;
	m_PrintfViewDlg->Create(NULL, "打印预览", WS_OVERLAPPED | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX, 
		Rect, NULL, NULL);
	m_PrintfViewDlg->ShowWindow(SW_SHOWMAXIMIZED);
	m_PrintfViewDlg->UpdateWindow();
	::EnableWindow(::GetDlgItem(GetSafeHwnd(), IDC_BUTTON2), FALSE);
}

void CQuery::OnBnClickedButton6()
{
	COleDateTime NowTime = COleDateTime::GetCurrentTime();
	m_Data.SetTime(NowTime);
	m_MothYear.SetCurSel(NowTime.GetYear()-2000);
	m_MothMoth.SetCurSel(NowTime.GetMonth()-1);
	m_Year.SetCurSel(NowTime.GetYear()-2000);
	m_ListReport.DeleteAllItems();
	m_StaticReportType.SetWindowText("");
	m_StaticReportTimes.SetWindowText("");
	m_EditSun.SetWindowText("");
	m_EditSpeed.SetWindowText("");
	m_EditCross.SetWindowText("");
	m_EditSun.SetWindowText("");
	m_nReportType = -1;

	InitChartData(0);
}

void CQuery::OnBnClickedButton3()
{
	CPrintDialog printfDlg(TRUE);
	printfDlg.DoModal();
}

LRESULT CQuery::OnViewQuit(WPARAM wParam, LPARAM lParam)
{
	if(m_PrintfViewDlg != NULL)
	{
		m_PrintfViewDlg = NULL;
	}
	::EnableWindow(::GetDlgItem(GetSafeHwnd(), IDC_BUTTON2), TRUE);
	return 0;
}

BOOL CQuery::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN)
		return TRUE;

	return CDialog::PreTranslateMessage(pMsg);
}

void CQuery::OnBnClickedCheck2()
{
	CButton* pBut = (CButton*)GetDlgItem(IDC_CHECK2);
	if(pBut->GetCheck() == 1)
	{
		m_fIsReverse = 1;
	}
	else
	{
		m_fIsReverse = 0;
	}
}

void CQuery::OnOutPutResultToExecl()
{
	int iItemCount = m_ListReport.GetItemCount();
	if(iItemCount <= 0)
	{
		MessageBox("没有任何数据记录，无法导出");
		return;
	}
	CFileDialog cSaveFile(FALSE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "Execl文档文件(*.xls)|*.xls||");
	if(cSaveFile.DoModal() == IDOK)
	{
		CString strSaveFileName = cSaveFile.GetPathName();
		if(strSaveFileName.GetLength() <= 3)
		{
			MessageBox("文件保存名错误，保存失败!", "ERROR", MB_OK|MB_ICONERROR);
			return;
		}
		if(strSaveFileName[strSaveFileName.GetLength()-1] != 'S'
			&& strSaveFileName[strSaveFileName.GetLength()-1] != 's')
		{
			strSaveFileName += ".xls";
		}

		CFileFind FileFind;
		if(FileFind.FindFile(strSaveFileName.GetBuffer()))
		{
			MessageBox("不能导出同名文件，导出失败！");
			return;
		}
		char szAppPath[MAX_PATH] = {0};
		GetModuleFileName(NULL, szAppPath, MAX_PATH);
		PathRemoveFileSpec(szAppPath);
		strcat(szAppPath, "\\template.ini");
		if(!FileFind.FindFile(szAppPath))
		{
			MessageBox("模板文件不存在程序运行路径下，导出失败!");
			return;
		}
		if(!CopyFile(szAppPath, strSaveFileName.GetBuffer(), TRUE))
		{
			MessageBox("创建文档失败，导出失败！");
			return;
		}

		m_ListReport.EnableWindow(FALSE);
		AfxOleGetMessageFilter()->EnableBusyDialog(FALSE);
		AfxOleGetMessageFilter()->SetBusyReply(SERVERCALL_RETRYLATER);
		AfxOleGetMessageFilter()->EnableNotRespondingDialog(TRUE);
		AfxOleGetMessageFilter()->SetMessagePendingDelay(-1);
		CApplication*	m_app = new CApplication;
		if(!m_app->CreateDispatch("Excel.Application"))
		{
			MessageBox("无法启动Excel服务器，请确认PC上已经装上MicroSoft Ofiic Excel2003 或更高版本,数据导出失败");
			m_ListReport.EnableWindow(true);
			return;
		}
		m_app->put_Visible(false);
		m_app->put_UserControl(true);

		CWorkbook	m_workBook;
		CWorkbooks	m_workBooks;
		CWorksheet	m_workSheet;
		CWorksheets m_workSheets;
		CRange		m_range;
		COleVariant	covOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);
		m_workBooks.AttachDispatch(m_app->get_Workbooks());
		m_workBook = m_workBooks.Open(strSaveFileName.GetBuffer(), covOptional, covOptional, covOptional, covOptional, covOptional, covOptional,
			covOptional, covOptional, covOptional, covOptional, covOptional, covOptional, covOptional, covOptional);
		m_workSheets = m_workBook.get_Worksheets();
		m_workSheet = m_workSheets.get_Item(COleVariant((short)1));
		
		for(int iIndex=0; iIndex<iItemCount; iIndex++)
		{
			char c = 'A';
			char szTmpInfo[64] = {0};
			char szRang[10];
			for(int iItem=0; iItem<13; iItem++)
			{
				sprintf(szRang, "%c%d", c++, iIndex+3);
				memset(szTmpInfo, 0, 64);
				m_ListReport.GetItemText(iIndex, iItem, szTmpInfo, 64);
				m_range = m_workSheet.get_Range(COleVariant(szRang), covOptional);
				m_range.put_Value2(COleVariant(szTmpInfo));
			}
		}

		m_workBook.Save();
		m_workBook.put_Saved(true);
		m_range.ReleaseDispatch();
		m_workSheet.ReleaseDispatch();
		m_workSheets.ReleaseDispatch();
		m_workBook.ReleaseDispatch();
		m_workBooks.ReleaseDispatch();
		m_workBook.Close(covOptional, covOptional, covOptional);
		m_workBooks.Close();
		m_app->Quit();
		m_app->ReleaseDispatch();
		delete m_app;
		m_app = NULL;
	}
}

void CQuery::OnNMRclickList3(NMHDR *pNMHDR, LRESULT *pResult)
{
	CMenu cMecu;
	cMecu.CreatePopupMenu();
	cMecu.AppendMenu(MF_BYCOMMAND | MF_STRING, ID_OUTPUTRESULTTOEXECL, "导出查询结果到EXECL文档");
	CPoint cTmpPoint;
	GetCursorPos(&cTmpPoint);
	cMecu.TrackPopupMenu(TPM_LEFTALIGN, cTmpPoint.x, cTmpPoint.y, this);
	*pResult = 0;
}

int CQuery::ShowCarCountPerDay(bool bShow)
{
	if (bShow)
	{
		// add CarCountPerDay column
		if (!m_ListReport.DeleteColumn(13))
		{
			TRACE("删除列13失败\n");

			CRect rc;
			GetWindowRect(rc);
			SetWindowPos(NULL, 0, 0, rc.Width()+80, rc.Height(), SWP_NOMOVE | SWP_NOZORDER);

			CRect rclist;
			m_ListReport.GetWindowRect(rclist);
			m_ListReport.SetWindowPos(NULL, 0, 0, rclist.Width()+80, rclist.Height(), SWP_NOMOVE | SWP_NOZORDER);

		}
		
		
	} 
	else
	{
		// delete CarCountPerDay column
		if (!m_ListReport.DeleteColumn(13))
		{
			TRACE("删除列13失败\n");
			return 0;
		}

		CRect rc;
		GetWindowRect(rc);
		SetWindowPos(NULL, 0, 0, rc.Width()-80, rc.Height(), SWP_NOMOVE | SWP_NOZORDER);

		CRect rclist;
		m_ListReport.GetWindowRect(rclist);
		m_ListReport.SetWindowPos(NULL, 0, 0, rclist.Width()-80, rclist.Height(), SWP_NOMOVE | SWP_NOZORDER);

	}
	return 0;
}

int CQuery::InitChart(void)
{
	CRect rclist;
	m_ListReport.GetWindowRect(rclist);
	ScreenToClient(rclist);
	m_Chart.SetWindowPos(NULL, rclist.left, rclist.top + rclist.Height()/2, rclist.Width(), rclist.Height()/2, SWP_HIDEWINDOW);

	m_Chart.EnableWindow(FALSE);

// 	m_Chart.GetTitle().GetVtFont().SetStyle(1);
// 	m_Chart.GetTitle().GetVtFont().SetSize(15);
	m_Chart.SetTitleText("车流量柱状图");

	// 下面两句改变背景色
	m_Chart.GetBackdrop().GetFill().SetStyle(1);
	m_Chart.GetBackdrop().GetFill().GetBrush().GetFillColor().Set(240, 240, 240);

	m_Chart.SetColumnCount(5); 
	m_Chart.SetShowLegend(TRUE);
	m_Chart.SetColumn(1);
	m_Chart.SetColumnLabel((LPCTSTR)"1车道");
	m_Chart.SetColumn(2);
	m_Chart.SetColumnLabel((LPCTSTR)"2车道");
	m_Chart.SetColumn(3);
	m_Chart.SetColumnLabel((LPCTSTR)"3车道");
	m_Chart.SetColumn(4);
	m_Chart.SetColumnLabel((LPCTSTR)"4车道");
	m_Chart.SetColumn(5);
	m_Chart.SetColumnLabel((LPCTSTR)"断面流量");

	// 栈模式
	m_Chart.SetStacking(FALSE);

	// Y轴设置
	VARIANT var;
	m_Chart.GetPlot().GetAxis(1,var).GetValueScale().SetAuto(TRUE);	
// 	m_Chart.GetPlot().GetAxis(1,var).GetValueScale().SetMaximum(100);	// Y轴最大刻度
// 	m_Chart.GetPlot().GetAxis(1,var).GetValueScale().SetMinimum(0);		// Y轴最小刻度
	m_Chart.GetPlot().GetAxis(1,var).GetValueScale().SetMajorDivision(5);	// Y轴刻度5等分
	m_Chart.GetPlot().GetAxis(1,var).GetValueScale().SetMinorDivision(1);	// 每刻度一个刻度线
// 	m_Chart.GetPlot().GetAxis(1,var).GetAxisTitle().GetVtFont().SetStyle(1);
// 	m_Chart.GetPlot().GetAxis(1,var).GetAxisTitle().GetVtFont().SetSize(15);
	m_Chart.GetPlot().GetAxis(1,var).GetAxisTitle().GetTextLayout().SetOrientation(0);
 	m_Chart.GetPlot().GetAxis(1,var).GetAxisTitle().SetText("流量");	// Y轴名称
	
	m_Chart.GetPlot().GetAxis(0,var).GetCategoryScale().SetAuto(FALSE);			// 不自动标注X轴刻度
	m_Chart.GetPlot().GetAxis(0,var).GetCategoryScale().SetDivisionsPerLabel(1);// 每刻度一个标注
	m_Chart.GetPlot().GetAxis(0,var).GetCategoryScale().SetDivisionsPerTick(1); // 每刻度一个刻度线
// 	m_Chart.GetPlot().GetAxis(0,var).GetAxisTitle().GetVtFont().SetStyle(1);
// 	m_Chart.GetPlot().GetAxis(0,var).GetAxisTitle().GetVtFont().SetSize(15);
	m_Chart.GetPlot().GetAxis(0,var).GetAxisTitle().SetText("时间");			// X轴名称
	

	// 线色
	m_Chart.GetPlot().GetSeriesCollection().GetItem(1).GetPen().GetVtColor().Set(0, 0, 255);
	m_Chart.GetPlot().GetSeriesCollection().GetItem(2).GetPen().GetVtColor().Set(255, 0, 0);
	m_Chart.GetPlot().GetSeriesCollection().GetItem(3).GetPen().GetVtColor().Set(0, 255, 0);
	m_Chart.GetPlot().GetSeriesCollection().GetItem(4).GetPen().GetVtColor().Set(255, 255, 0);
	m_Chart.GetPlot().GetSeriesCollection().GetItem(5).GetPen().GetVtColor().Set(0, 255, 255);

	InitChartData(0);

	return 0;
}

int CQuery::InitChartData(short nRowCount)
{
	m_Chart.SetRowCount(nRowCount);

	for(int row = 1; row <= nRowCount; ++row)
	{
		m_Chart.SetRow(row);
		m_Chart.SetRowLabel("");

		m_Chart.GetDataGrid().SetData(row, 1, 0, 0);
		m_Chart.GetDataGrid().SetData(row, 2, 0, 0);
		m_Chart.GetDataGrid().SetData(row, 3, 0, 0);
		m_Chart.GetDataGrid().SetData(row, 4, 0, 0);
		m_Chart.GetDataGrid().SetData(row, 5, 0, 0);
	}

	m_Chart.Refresh();	

	return 0;
}

int CQuery::SetChartData(short nRow, double dblCarNum1, double dblCarNum2, double dblCarNum3, double dblCarNum4, double dblCarCount)
{
	m_Chart.SetRow(nRow);

	m_Chart.SetRowLabel(m_ListReport.GetItemText(nRow-1, 0));

	m_Chart.GetDataGrid().SetData(nRow, 1, dblCarNum1, 0);
	m_Chart.GetDataGrid().SetData(nRow, 2, dblCarNum2, 0);
	m_Chart.GetDataGrid().SetData(nRow, 3, dblCarNum3, 0);
	m_Chart.GetDataGrid().SetData(nRow, 4, dblCarNum4, 0);
	m_Chart.GetDataGrid().SetData(nRow, 5, dblCarCount, 0);

	m_Chart.Refresh();

	return 0;
}
BEGIN_EVENTSINK_MAP(CQuery, CDialog)
//	ON_EVENT(CQuery, IDC_MSCHART1, DISPID_DBLCLICK, DblClickMschart1, VTS_NONE)
END_EVENTSINK_MAP()

//void CQuery::DblClickMschart1()
//{
//	// TODO: Add your message handler code here
//
//}



int CQuery::GetChartDC(CDC* pDc, int& iWidth, int& iHeight)
{

	CRect rc;
	m_Chart.GetClientRect(rc);
	m_Chart.SetWindowPos(NULL, 0, 0, 1123, 794, SWP_NOMOVE | SWP_NOZORDER);
	m_Chart.GetBackdrop().GetFill().SetStyle(0);

	m_Chart.EditCopy();

	m_Chart.SetWindowPos(NULL, 0, 0, rc.Width(), rc.Height(), SWP_NOMOVE | SWP_NOZORDER);
	m_Chart.GetBackdrop().GetFill().SetStyle(1);

	if (!OpenClipboard())
	{
		CloseClipboard();
		CString strerr;
		strerr.Format(_T("剪贴板打开失败，柱状图将无法打印，请先释放剪贴板再重试\n代码：%d"), GetLastError());
		AfxMessageBox(strerr);		
		return -1;
	}

	HBITMAP hBmp = (HBITMAP)GetClipboardData(CF_BITMAP);

	if (NULL == hBmp)
	{
		CloseClipboard();
		CString strerr;
		strerr.Format(_T("获取剪贴板失败\n代码：%d"), GetLastError());
		AfxMessageBox(strerr);
		return -1;
	}
	
	CBitmap* pBmp = CBitmap::FromHandle(hBmp);
	if (NULL == pBmp)
	{
		CloseClipboard();
		CString strerr;
		strerr.Format(_T("获取位图失败\n代码：%d"), GetLastError());
		AfxMessageBox(strerr);
		return -1;
	}

	CDC* pdc0 = GetDC();
	CDC   dcChart;   
	if (FALSE == dcChart.CreateCompatibleDC(pdc0))
	{
		CloseClipboard();
		CString strerr;
		strerr.Format(_T("创建dcChart失败\n代码：%d"), GetLastError());
		AfxMessageBox(strerr);
		return -1;
	}

	dcChart.SelectObject(pBmp); 

	CRect rcfill;
	rcfill.left = 0;
	rcfill.right = 40;
	rcfill.top = 377;
	rcfill.bottom = 417;

	dcChart.FillRect(&rcfill, m_pBrushChart);
	dcChart.SelectObject(&m_fontChart);
	dcChart.TextOut(10, 397, "流量");

	BITMAP bmp;
	pBmp->GetBitmap(&bmp);

	if( FALSE == pDc->CreateCompatibleDC(&dcChart) )
	{
		CloseClipboard();
		CString strerr;
		strerr.Format(_T("创建dcChart2失败\n代码：%d"), GetLastError());
		AfxMessageBox(strerr);
		return -1;

	}

	HBITMAP hBmp2 = CreateCompatibleBitmap(dcChart.m_hDC, bmp.bmHeight, bmp.bmWidth);
	if (NULL == hBmp2)
	{
		CloseClipboard();
		CString strerr;
		strerr.Format(_T("创建hBmp2失败\n代码：%d"), GetLastError());
		AfxMessageBox(strerr);
		return -1;
	}

	pDc->SelectObject(hBmp2);

	for (int i=0; i< bmp.bmWidth;i++)
	{
		for (int j=0; j<bmp.bmHeight;j++)
		{
			BitBlt(pDc->m_hDC, bmp.bmHeight - j - 1, i, 1, 1, dcChart.m_hDC,i,j,SRCCOPY);
		}
	}

	iWidth = bmp.bmHeight;
	iHeight = bmp.bmWidth;

	::DeleteObject(hBmp2);
	dcChart.DeleteDC();	

	if (FALSE == EmptyClipboard())
	{
		CloseClipboard();
		CString strerr;
		strerr.Format(_T("清空剪贴板错误\n代码：%d"), GetLastError());
		AfxMessageBox(strerr);
		return -1;
	}

	if (!CloseClipboard())
	{
		CString strerr;
		strerr.Format(_T("释放剪贴板错误\n代码：%d"), GetLastError());
		AfxMessageBox(strerr);
		return -1;
	}
	
	return 0;
}

void CQuery::OnBnClickedCheckshowchart()
{
	// TODO: Add your control notification handler code here

	UpdateData();

	CRect rclist;
	m_ListReport.GetWindowRect(rclist);

	if (m_bShowChart)
	{
		m_ListReport.SetWindowPos(NULL, 0, 0, rclist.Width(), rclist.Height()/2, SWP_NOMOVE);
		m_Chart.ShowWindow(SW_SHOW);
	} 
	else
	{
		m_ListReport.SetWindowPos(NULL, 0, 0, rclist.Width(), rclist.Height()*2, SWP_NOMOVE);
		m_Chart.ShowWindow(SW_HIDE);
		
	}

}
