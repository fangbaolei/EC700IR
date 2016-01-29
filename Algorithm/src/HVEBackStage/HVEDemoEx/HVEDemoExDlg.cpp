// HVEDemoExDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "HVEDemoEx.h"
#include "HVEDemoExDlg.h"
#include "SystemSettingDlg.h"
#include ".\hvedemoexdlg.h"
#include "QueryDlg.h"
#include "iptypes.h"
#include "iphlpapi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CHVEDemoExDlg 对话框

#pragma comment(lib, "iphlpapi.lib")

CString g_strMacAddr = "";

CHVEDemoExDlg::CHVEDemoExDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHVEDemoExDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_fIsConnect = FALSE;
	m_strSourcePath = MAKEIPADDRESS(127, 0, 0, 1);
	m_strDataBaseName = "HVE_DATA";
	m_strUserName = "sa";
	m_strPassWord = "12345678";
	m_nCurrentRoadID = 0;
	m_iLocationCount = 0;
	m_fIsConnect = FALSE;
	m_fIsRunning = FALSE;
	m_QueryDlg = NULL;
	m_fIsShowQuery = FALSE;
	m_SetDlg = NULL;
	m_fIsShowSetDlg = FALSE;
	m_fIsADOConnected = FALSE;
	m_strIniFilePath = "";
}

void CHVEDemoExDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_listRoadInfo);
	DDX_Control(pDX, IDC_LIST2, m_listReport);
	DDX_Control(pDX, IDC_STATICMINROAD1CARNUM, m_staticMinRoad1CarNum);
	DDX_Control(pDX, IDC_STATICMINROAD1SPEED, m_staticMinRoad1AvrSpeed);
	DDX_Control(pDX, IDC_STATICMINROAD1CROSS, m_staticMinRoad1Avr);
	DDX_Control(pDX, IDC_STATICMINROAD1STATUE, m_staticMinRoad1Statu);

	DDX_Control(pDX, IDC_STATICMINROAD2CARNUM, m_staticMinRoad2CarNum);
	DDX_Control(pDX, IDC_STATICMINROAD2SPEED, m_staticMinRoad2AvrSpeed);
	DDX_Control(pDX, IDC_STATICMINROAD2CROSS, m_staticMinRoad2Avr);
	DDX_Control(pDX, IDC_STATICMINROAD2STATUE, m_staticMinRoad2Statu);

	DDX_Control(pDX, IDC_STATICMINROAD3CARNUM, m_staticMinRoad3CarNum);
	DDX_Control(pDX, IDC_STATICMINROAD3SPEED, m_staticMinRoad3AvrSpeed);
	DDX_Control(pDX, IDC_STATICMINROAD3CROSS, m_staticMinRoad3Avr);
	DDX_Control(pDX, IDC_STATICMINROAD3STATUE, m_staticMinRoad3Statu);

	DDX_Control(pDX, IDC_STATICMINROAD4CARNUM, m_staticMinRoad4CarNum);
	DDX_Control(pDX, IDC_STATICMINROAD4SPEED, m_staticMinRoad4AvrSpeed);
	DDX_Control(pDX, IDC_STATICMINROAD4CROSS, m_staticMinRoad4Avr);
	DDX_Control(pDX, IDC_STATICMINROAD4STATUE, m_staticMinRoad4Statu);

	DDX_Control(pDX, IDC_STATICHOURROAD1CARNUM, m_staticHourRoad1CarNum);
	DDX_Control(pDX, IDC_STATICHOURROAD1SPEED, m_staticHourRoad1AvrSpeed);
	DDX_Control(pDX, IDC_STATICHOURROAD1CROSS, m_staticHourRoad1Avr);
	DDX_Control(pDX, IDC_STATICHOURROAD1STATUE, m_staticHourRoad1Statu);

	DDX_Control(pDX, IDC_STATICHOURROAD2CARNUM, m_staticHourRoad2CarNum);
	DDX_Control(pDX, IDC_STATICHOURROAD2SPEED, m_staticHourRoad2AvrSpeed);
	DDX_Control(pDX, IDC_STATICHOURROAD2CROSS, m_staticHourRoad2Avr);
	DDX_Control(pDX, IDC_STATICHOURROAD2STATUE, m_staticHourRoad2Statu);

	DDX_Control(pDX, IDC_STATICHOURROAD3CARNUM, m_staticHourRoad3CarNum);
	DDX_Control(pDX, IDC_STATICHOURROAD3SPEED, m_staticHourRoad3AvrSpeed);
	DDX_Control(pDX, IDC_STATICHOURROAD3CROSS, m_staticHourRoad3Avr);
	DDX_Control(pDX, IDC_STATICHOURROAD3STATUE, m_staticHourRoad3Statu);

	DDX_Control(pDX, IDC_STATICHOURROAD4CARNUM, m_staticHourRoad4CarNum);
	DDX_Control(pDX, IDC_STATICHOURROAD4SPEED, m_staticHourRoad4AvrSpeed);
	DDX_Control(pDX, IDC_STATICHOURROAD4CROSS, m_staticHourRoad4Avr);
	DDX_Control(pDX, IDC_STATICHOURROAD4STATUE, m_staticHourRoad4Statu);

	DDX_Control(pDX, IDC_STATICCURRENTSTATS, m_staticCurrentStatue);
	DDX_Control(pDX, IDC_COMBO1, m_ComBoxRoad);
}

BEGIN_MESSAGE_MAP(CHVEDemoExDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_MENUSELECT()
	ON_WM_CTLCOLOR()
	ON_MESSAGE(WM_DLGENDING, OnQueryQuit)
	ON_MESSAGE(WM_SET_ONOK, OnSetOk)
	ON_MESSAGE(WM_SET_ONCACAL, OnSetCancel)
	ON_CBN_SELCHANGE(IDC_COMBO1, OnCbnSelchangeCombo1)
	ON_BN_CLICKED(IDC_CHECK1, OnBnClickedCheck1)
END_MESSAGE_MAP()


// CHVEDemoExDlg 消息处理程序

BOOL CHVEDemoExDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	HANDLE hMutex = NULL;
	hMutex = ::CreateMutex(NULL, TRUE, _T("HVEDemoEx.exe"));
	if(GetLastError() == ERROR_ALREADY_EXISTS)
	{
		hMutex = NULL;
		OnCancel();
		return FALSE;
	}
	if(hMutex)
	{
		::ReleaseMutex(hMutex);
	}

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

	CButton* pBut = (CButton*)GetDlgItem(IDC_CHECK1);
	pBut->SetCheck(0);
	m_fIsReverse = FALSE;

	//try
	//{
	//	dbAx::Init();
	//	m_cAxConn.Create();
	//}
	//catch (dbAx::CAxException* e)
	//{
	//	MessageBox(e->m_szErrorDesc, "数据库操作异常", MB_OK | MB_ICONERROR);
	//	OnCancel();
	//}

	//try
	//{
	//	m_cRec.Create();
	//	m_cRec.CursorType(adOpenForwardOnly);
	//	m_cRec.CacheSize(100);
	//}
	//catch (dbAx::CAxException* e)
	//{
	//	delete e;
	//}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
	CMenu DlgMenu;
	DlgMenu.LoadMenu(IDR_MENU1);
	SetMenu(&DlgMenu);

	::CoInitialize(NULL);

	m_listRoadInfo.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_listRoadInfo.InsertColumn(0, "车道编号", LVCFMT_LEFT, 80);
	m_listRoadInfo.InsertColumn(1, "设备IP", LVCFMT_LEFT, 160);
	m_listRoadInfo.InsertColumn(2, "监测断面名称", LVCFMT_LEFT, 355);
	m_listRoadInfo.SetBkColor(RGB(255, 255, 180));
	m_listRoadInfo.SetTextBkColor(RGB(255, 255, 180));
	
	m_listReport.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_listReport.InsertColumn(0, "小时", LVCFMT_LEFT, 40);
	m_listReport.InsertColumn(1, "车道1", LVCFMT_LEFT, 77);
	m_listReport.InsertColumn(2, "车道2", LVCFMT_LEFT, 77);
	m_listReport.InsertColumn(3, "车道3", LVCFMT_LEFT, 78);
	m_listReport.InsertColumn(4, "车道4", LVCFMT_LEFT, 78);
	m_listReport.SetBkColor(RGB(160, 220, 220));
	m_listReport.SetTextBkColor(RGB(160, 220, 220));

	//m_QueryDlg = new CQuery(this);
	//m_QueryDlg->Create(IDD_DIALOGQUERY, this);
	m_QueryDlg = NULL;
	m_fIsShowQuery = FALSE;
	ShowWindow(SW_SHOW);
	UpdateWindow();

	GetIniFileInfo();
	CString strIP;
	CString strConnect;
	strIP.Format("%d.%d.%d.%d", (m_strSourcePath>>24)&0xFF, (m_strSourcePath>>16)&0xFF, 
		(m_strSourcePath>>8)&0xFF, (m_strSourcePath)&0xFF);
	if(strIP == "0.0.0.0") strIP = ".";

	strConnect.Format("Provider=SQLOLEDB.1;Password=%s;User ID=%s;Initial Catalog=%s;Data Source=%s",
		m_strPassWord, m_strUserName, m_strDataBaseName, strIP);

	DoConnectDataBase(strConnect);
	m_fIsNewInfo = TRUE;
	if(m_fIsADOConnected)
	{
		m_staticCurrentStatue.SetWindowText("当前状态：已连接到数据库");
		Starting();
	}
	else
	{
		m_SetDlg = new CSystemSettingDlg(this);
		m_SetDlg->Create(IDD_DIALOGSYSSET, this);
		m_SetDlg->ShowWindow(SW_SHOW);
		m_SetDlg->UpdateWindow();
		m_fIsShowSetDlg = TRUE;
	}

	return TRUE;  // 除非设置了控件的焦点，否则返回 TRUE
}

void CHVEDemoExDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialog::OnSysCommand(nID, lParam);
}

void CHVEDemoExDlg::OnCancel()
{
	TRACE("Cancel\n");
	if(m_fIsRunning)
	{
		TRACE("brfore stop\n");
		Stop();
		TRACE("brfore after\n");
	}
	
	if(m_fIsConnect)
	{
		TRACE("before disconect\n");
		DisConnect();
		TRACE("after disconect\n");
	}

	CDialog::OnCancel();
}

void CHVEDemoExDlg::OnOK()
{
	
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CHVEDemoExDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
HCURSOR CHVEDemoExDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CHVEDemoExDlg::OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu)
{
	if(nItemID == IDC_SYSTEMSET)
	{
		if(m_fIsShowQuery)
		{
			MessageBox("查询窗口已打开，请先关闭查询窗口才能打开数据库设置窗口");
			return;
		}
		if(m_fIsShowSetDlg) return;
		m_fIsShowSetDlg = TRUE;
		if(m_SetDlg != NULL)
		{
			delete m_SetDlg;
			m_SetDlg = NULL;
		}
		m_SetDlg = new CSystemSettingDlg(this);
		m_SetDlg->Create(IDD_DIALOGSYSSET, this);
		m_SetDlg->ShowWindow(SW_SHOW);
	}
	else if(nItemID == IDC_QUREPRINT)
	{
		if(m_fIsShowSetDlg)
		{
			MessageBox("设置数据库窗口已打开，请先关闭数据库设置窗口才能打开查询窗口");
			return;
		}
		if(m_fIsShowQuery) return;
		m_fIsShowQuery = TRUE;
		if(m_QueryDlg != NULL)
		{
			delete m_QueryDlg;
			m_QueryDlg = NULL;
		}
		m_QueryDlg = new CQuery(this);
		m_QueryDlg->Create(IDD_DIALOGQUERY, this);
		m_QueryDlg->ShowWindow(SW_SHOW);
	}
	CDialog::OnMenuSelect(nItemID, nFlags, hSysMenu);
}

BOOL CHVEDemoExDlg::DoConnectDataBase(CString strConnect)
{	
	if(m_fIsADOConnected) return TRUE;
	m_fIsADOConnected = TRUE;
	m_staticCurrentStatue.SetWindowText("当前状态：正在连接到数据库");
	HRESULT hr = S_OK;
	try
	{
		hr = m_pConnection.CreateInstance("ADODB.Connection");
		if(SUCCEEDED(hr))
		{
			m_pConnection->ConnectionTimeout = 10;
			m_pConnection->CommandTimeout = 0;
			m_pConnection->CursorLocation = adUseServer;
			m_pConnection->IsolationLevel = adXactReadCommitted;
			hr = m_pConnection->Open(
				(_bstr_t)(strConnect.GetBuffer(0)), "", "", adModeUnknown);
		}
	}
	catch (_com_error e)
	{
		m_staticCurrentStatue.SetWindowText("当前状态：连接数据库失败");
		m_fIsADOConnected = FALSE;
		return FALSE;
	}
	if(FAILED(hr))
	{
		m_fIsADOConnected = FALSE;
		return FALSE;
	}
	m_staticCurrentStatue.SetWindowText("当前状态：连接数据库成功");
	return QueryLocation();
	//try
	//{
	//	if(m_cAxConn._IsOpen())
	//	{
	//		m_cAxConn.Close();
	//	}
	//	m_cAxConn.CursorLocation(adUseServer);
	//	m_cAxConn.ConnectionTimeout(10);
	//	m_cAxConn.CommandTimeout(20);
	//	m_cAxConn.Open(strConnect);
	//}
	//catch (dbAx::CAxException* e)
	//{
	//	MessageBox("连接数据库失败");
	//	delete e;
	//	CoUninitialize();
	//	return FALSE;
	//}
	//CoUninitialize();
	//m_fIsConnect = TRUE;
	//return QueryLocation();
}

HBRUSH CHVEDemoExDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	int nID = pWnd->GetDlgCtrlID();
	if(nID == IDC_STATICMINROAD1 || nID == IDC_STATICMINROAD2 || nID == IDC_STATICMINROAD3 || nID == IDC_STATICMINROAD4)
	{
		pDC->SetTextColor(RGB(0, 0, 255));
	}
	else if(nID == IDC_STATICHOURROAD1 || nID == IDC_STATICHOURROAD2 || nID == IDC_STATICHOURROAD3 || nID == IDC_STATICHOURROAD4)
	{
		pDC->SetTextColor(RGB(0, 0, 255));
	}

	else if(nID == IDC_STATICMINROAD1CARNUM || nID == IDC_STATICMINROAD2CARNUM || nID == IDC_STATICMINROAD3CARNUM || nID == IDC_STATICMINROAD4CARNUM)
	{
		pDC->SetTextColor(RGB(40, 190, 40));
	}
	else if(nID == IDC_STATICMINROAD1SPEED || nID == IDC_STATICMINROAD2SPEED || nID == IDC_STATICMINROAD3SPEED || nID == IDC_STATICMINROAD4SPEED)
	{
		pDC->SetTextColor(RGB(40, 190, 40));
	}
	else if(nID == IDC_STATICMINROAD1CROSS || nID == IDC_STATICMINROAD2CROSS || nID == IDC_STATICMINROAD3CROSS || nID == IDC_STATICMINROAD4CROSS)
	{
		pDC->SetTextColor(RGB(40, 190, 40));
	}

	else if(nID == IDC_STATICHOURROAD1CARNUM || nID == IDC_STATICHOURROAD2CARNUM || nID == IDC_STATICHOURROAD3CARNUM || nID == IDC_STATICHOURROAD4CARNUM)
	{
		pDC->SetTextColor(RGB(40, 190, 40));
	}
	else if(nID == IDC_STATICHOURROAD1SPEED || nID == IDC_STATICHOURROAD2SPEED || nID == IDC_STATICHOURROAD3SPEED || nID == IDC_STATICHOURROAD4SPEED)
	{
		pDC->SetTextColor(RGB(40, 190, 40));
	}
	else if(nID == IDC_STATICHOURROAD1CROSS || nID == IDC_STATICHOURROAD2CROSS || nID == IDC_STATICHOURROAD3CROSS || nID == IDC_STATICHOURROAD4CROSS)
	{
		pDC->SetTextColor(RGB(40, 190, 40));
	}
	
	else  if(nID == IDC_STATICMINROAD1STATUE || nID == IDC_STATICMINROAD2STATUE || nID == IDC_STATICMINROAD3STATUE || nID == IDC_STATICMINROAD4STATUE
		|| nID == IDC_STATICHOURROAD1STATUE || nID == IDC_STATICHOURROAD2STATUE || nID == IDC_STATICHOURROAD3STATUE || nID == IDC_STATICHOURROAD4STATUE)
	{
		char szTmp[10];
		pWnd->GetWindowText(szTmp, 10);
		if(strcmp(szTmp, "畅通") == 0)
		{
			pDC->SetBkMode(3);
			pDC->SetBkColor(RGB(0, 255, 0));
			HBRUSH B = CreateSolidBrush(RGB(0, 255, 0));
			return B;
		}
		else if(strcmp(szTmp, "基本畅通") == 0)
		{
			pDC->SetBkMode(3);
			pDC->SetBkColor(RGB(0, 130, 0));
			HBRUSH B = CreateSolidBrush(RGB(0, 130, 0));
			return B;
		}
		else if(strcmp(szTmp, "拥挤") == 0)
		{
			pDC->SetBkMode(3);
			pDC->SetBkColor(RGB(255, 255, 0));
			HBRUSH B = CreateSolidBrush(RGB(255, 255, 0));
			return B;
		}
		else if(strcmp(szTmp, "堵塞") == 0)
		{
			pDC->SetBkMode(3);
			pDC->SetBkColor(RGB(250, 128, 10));
			HBRUSH B = CreateSolidBrush(RGB(250, 128, 10));
			return B;
		}
		else
		{
			pDC->SetBkMode(3);
			pDC->SetBkColor(RGB(250, 128, 10));
			HBRUSH B = CreateSolidBrush(RGB(0, 255, 255));
			return B;
		}
	}

	return hbr;
}

void CHVEDemoExDlg::AddLocaTionToCombox()
{
	m_ComBoxRoad.ResetContent();
	for(int index=0; index<m_iLocationCount; index++)
	{
		m_ComBoxRoad.InsertString(index, m_rgLocation[index].strLocation_Name.GetBuffer(0));
	}
	m_ComBoxRoad.SetCurSel(m_nCurrentRoadID);
}

BOOL CHVEDemoExDlg::QueryLocation()
{
	if(!m_fIsADOConnected) return FALSE;
	try
	{
		HRESULT	hr;
		//_RecordsetPtr	pRentRecordset;
		m_staticCurrentStatue.SetWindowText("当前状态：正在获取断面信息");
		hr = pRenRecordset.CreateInstance("ADODB.Recordset");
		CString strSql = "SELECT * FROM Hve_Location_Info ORDER BY Location_Id";
		_variant_t var;
		hr = pRenRecordset->Open(_variant_t(strSql), m_pConnection.GetInterfacePtr(), 
			adOpenDynamic, adLockOptimistic, adCmdText);
		if(SUCCEEDED(hr))
		{
			m_iLocationCount = 0;
			LOCATION_INFO* pLocationInfo;
			while(!pRenRecordset->adoEOF)
			{
				pLocationInfo = &m_rgLocation[m_iLocationCount++];
				pLocationInfo->rgRoadInfo.RemoveAll();

				var = pRenRecordset->GetCollect((long)0);
				pLocationInfo->nLocation_Id = (int)var.lVal;
				
				var = pRenRecordset->GetCollect((long)1);
				pLocationInfo->strLocation_Name = (LPCSTR)_bstr_t(var);

				var = pRenRecordset->GetCollect((long)3);
				pLocationInfo->nSite_Distance = (int)var.lVal;

				var = pRenRecordset->GetCollect((long)4);
				pLocationInfo->nRoad_Able_Adapt = (int)var.lVal;
				pLocationInfo->strLocation_Name.Trim();
				
				pRenRecordset->MoveNext();
			}
			pRenRecordset->Close();
			pRenRecordset = NULL;

			hr = pRenRecordset.CreateInstance("ADODB.Recordset");
			strSql = "SELECT * FROM Hve_Road_Info ORDER BY Location_id";
			hr = pRenRecordset->Open(_variant_t(strSql), m_pConnection.GetInterfacePtr(), 
				adOpenDynamic, adLockOptimistic, adCmdText);
			int nID = 0;
			ROAD_INFO cRoadInfo;
			if(SUCCEEDED(hr))
			{
				while(!pRenRecordset->adoEOF)
				{
					var = pRenRecordset->GetCollect((long)0);
					nID = (int)var.lVal;

					var = pRenRecordset->GetCollect((long)1);
					cRoadInfo.nRoad_No = (int)var.lVal;

					var = pRenRecordset->GetCollect((long)2);
					cRoadInfo.strHve_Addr = (LPCSTR)_bstr_t(var);

					var = pRenRecordset->GetCollect((long)3);
					cRoadInfo.nLimit_Speed = (int)var.lVal;
					cRoadInfo.strHve_Addr.Trim();
					pLocationInfo = GetLocationInfoByID(nID);
					if(pLocationInfo) pLocationInfo->rgRoadInfo.Add(cRoadInfo);
					pRenRecordset->MoveNext();
				}
				pRenRecordset->Close();
				pRenRecordset = NULL;
			}
		}
	}
	catch (_com_error e)
	{
		m_pConnection->Close();
		m_fIsADOConnected = FALSE;
		m_staticCurrentStatue.SetWindowText("当前状态：获取断面信息失败");
		return FALSE;
	}
	m_nCurrentRoadID = 0;
	m_staticCurrentStatue.SetWindowText("当前状态：获取断面信息成功");
	ShowRoadInfo();
	AddLocaTionToCombox();
	//if(!m_fIsConnect)return FALSE;
	//try
	//{
	//	CAxRecordset cRec;
	//	cRec.Create();
	//	cRec.CursorType(adOpenForwardOnly);
	//	cRec.CacheSize(100);

	//	CString strSql = "SELECT * FROM Hve_Location_Info ORDER BY Location_Id";
	//	cRec.Open(strSql, &m_cAxConn);

	//	m_iLocationCount = 0;
	//	LOCATION_INFO* pLocInfo;
	//	while(!cRec.IsEOF())
	//	{
	//		pLocInfo = &m_rgLocation[m_iLocationCount++];
	//		pLocInfo->rgRoadInfo.RemoveAll();

	//		cRec.FX_Integer(FALSE, "Location_Id", pLocInfo->nLocation_Id);
	//		cRec.FX_Char(FALSE, "Location_Name", pLocInfo->strLocation_Name);
	//		cRec.FX_Integer(FALSE, "Site_Distance", pLocInfo->nSite_Distance);
	//		cRec.FX_Integer(FALSE, "Road_Able_Adapt", pLocInfo->nRoad_Able_Adapt);
	//		pLocInfo->strLocation_Name.Trim();
	//		cRec.MoveNext();
	//	}
	//	cRec.Close();
	//	strSql = "SELECT * FROM Hve_Road_Info ORDER BY Location_id";
	//	cRec.Open(strSql, &m_cAxConn);
	//	int nID = 0;
	//	ROAD_INFO cRoadInfo;
	//	while(!cRec.IsEOF())
	//	{
	//		cRec.FX_Integer(FALSE, "Location_Id", nID);
	//		cRec.FX_Integer(FALSE, "Road_No", cRoadInfo.nRoad_No);
	//		cRec.FX_Char(FALSE, "Hve_Addr", cRoadInfo.strHve_Addr);
	//		cRec.FX_Integer(FALSE, "Limit_Speed", cRoadInfo.nLimit_Speed);
	//		cRoadInfo.strHve_Addr.Trim();
	//		pLocInfo = GetLocationInfoByID(nID);
	//		if(pLocInfo)
	//		{
	//			pLocInfo->rgRoadInfo.Add(cRoadInfo);
	//		}
	//		cRec.MoveNext();
	//	}
	//	cRec.Close();
	//}
	//catch(dbAx::CAxException *e)
	//{
	//	MessageBox(e->m_szErrorDesc, "数据库操作错误", MB_OK | MB_ICONERROR);
	//	delete e;
	//	return FALSE;
	//}
	//m_nCurrentRoadID = 0;
	//ShowRoadInfo();
	return TRUE;
}

LOCATION_INFO* CHVEDemoExDlg::GetLocationInfoByID(int ID)
{
	LOCATION_INFO* pInfo = NULL;
	for(int i=0; i<m_iLocationCount; i++)
	{
		if(m_rgLocation[i].nLocation_Id == ID)
		{
			pInfo = &m_rgLocation[i];
			break;
		}
	}
	return pInfo;
}

BOOL CHVEDemoExDlg::Starting()
{
	if(m_fIsRunning) return TRUE;
	m_updataThread = AfxBeginThread(UpDataThread, this);
	return TRUE;
}

BOOL CHVEDemoExDlg::Stop()
{
	if(!m_fIsRunning || m_updataThread == NULL) return TRUE;
	m_fIsRunning = FALSE;
	if(m_pConnection)
	m_pConnection->Cancel();
	if(pRenRecordset)
	pRenRecordset->Cancel();
	DWORD Index = 0;
	m_staticCurrentStatue.SetWindowText("当前状态：正在停止数据库查询的操作");
	while(m_updataThread != NULL)
	{
		Index++;
		if(Index > 100)
		{
			m_updataThread->ExitInstance();
			m_updataThread = NULL;
			break;
		}
		Sleep(20);
	}
	m_fIsADOConnected = FALSE;
	m_staticCurrentStatue.SetWindowText("当前状态：已停止数据库查询操作");
	m_listReport.DeleteAllItems();
	m_listRoadInfo.DeleteAllItems();
	m_staticMinRoad1CarNum.SetWindowText("");
	m_staticMinRoad1AvrSpeed.SetWindowText("");
	m_staticMinRoad1Avr.SetWindowText("");
	m_staticMinRoad1Statu.SetWindowText("");

	m_staticMinRoad2CarNum.SetWindowText("");
	m_staticMinRoad2AvrSpeed.SetWindowText("");
	m_staticMinRoad2Avr.SetWindowText("");
	m_staticMinRoad2Statu.SetWindowText("");

	m_staticMinRoad3CarNum.SetWindowText("");
	m_staticMinRoad3AvrSpeed.SetWindowText("");
	m_staticMinRoad3Avr.SetWindowText("");
	m_staticMinRoad3Statu.SetWindowText("");

	m_staticMinRoad4CarNum.SetWindowText("");
	m_staticMinRoad4AvrSpeed.SetWindowText("");
	m_staticMinRoad4Avr.SetWindowText("");
	m_staticMinRoad4Statu.SetWindowText("");

	m_staticHourRoad1CarNum.SetWindowText("");
	m_staticHourRoad1AvrSpeed.SetWindowText("");
	m_staticHourRoad1Avr.SetWindowText("");
	m_staticHourRoad1Statu.SetWindowText("");

	m_staticHourRoad2CarNum.SetWindowText("");
	m_staticHourRoad2AvrSpeed.SetWindowText("");
	m_staticHourRoad2Avr.SetWindowText("");
	m_staticHourRoad2Statu.SetWindowText("");

	m_staticHourRoad3CarNum.SetWindowText("");
	m_staticHourRoad3AvrSpeed.SetWindowText("");
	m_staticHourRoad3Avr.SetWindowText("");
	m_staticHourRoad3Statu.SetWindowText("");

	m_staticHourRoad4CarNum.SetWindowText("");
	m_staticHourRoad4AvrSpeed.SetWindowText("");
	m_staticHourRoad4Avr.SetWindowText("");
	m_staticHourRoad4Statu.SetWindowText("");
	return TRUE;
}

UINT CHVEDemoExDlg::UpDataThread(LPVOID userData)
{
	CHVEDemoExDlg* dlg = (CHVEDemoExDlg*)userData;
	if(dlg == NULL) return 0;
	dlg->m_fIsRunning = TRUE;
	time_t	lastMin;
	time_t	lastHour;
	time_t	nowTime;
//	BOOL	fFirstTime = TRUE;
	nowTime = time(&nowTime);
	lastMin = nowTime;
	lastHour = nowTime;
	dlg->ShowRoadInfo();
	dlg->m_fIsGetMiniDataFaile = FALSE;
	dlg->m_fIsGetHourDataFaile = FALSE;
	dlg->m_fIsGetReportFaile = FALSE;
	while(dlg->m_fIsRunning)
	{
		if(!dlg->m_fIsRunning)break;
		if(dlg->m_fIsNewInfo)
		{
			if(!dlg->m_fIsRunning)break;
			COleDateTime startTime(nowTime-80), endTime(nowTime-20);
			dlg->UpdataMinInfo(startTime, endTime);
			COleDateTime startHour(nowTime - 3600);
			dlg->UpdataHourInfo(startHour, endTime);
			if(!dlg->m_fIsRunning)break;
			dlg->InitReportList();
			if(!dlg->m_fIsRunning)break;
			dlg->m_fIsNewInfo = FALSE;
			continue;
		}
		if(!dlg->m_fIsRunning)break;
		time(&nowTime);
		if(nowTime - lastMin >= 60)
		{
			if(!dlg->m_fIsRunning)break;
			COleDateTime startTime(lastMin-20), endTime(nowTime-20);
			dlg->UpdataMinInfo(startTime, endTime);
			lastMin = nowTime;
		}
		else if(dlg->m_fIsGetMiniDataFaile)
		{
			if(!dlg->m_fIsRunning)break;
			COleDateTime startTime(nowTime-80), endTime(nowTime - 20);
			dlg->UpdataMinInfo(startTime, endTime);
			lastMin = nowTime;
		}

		if(!dlg->m_fIsRunning)break;
		if(nowTime - lastHour >= 3600)
		{
			if(!dlg->m_fIsRunning)break;
			COleDateTime startTime(lastHour), endTime(nowTime);
			dlg->UpdataHourInfo(startTime, endTime);
			lastHour = nowTime;
		}
		else if(dlg->m_fIsGetHourDataFaile)
		{
			if(!dlg->m_fIsRunning)break;
			COleDateTime startHour(nowTime - 3600), endTime(nowTime);
			dlg->UpdataHourInfo(startHour, endTime);
			lastHour = nowTime;
		}

		if(!dlg->m_fIsRunning)break;
		dlg->m_ShowCurrentHourDelay--;
		if(dlg->m_ShowCurrentHourDelay <= 0 )
		{
			dlg->ShowCurrentCarNum();
		}
		else if(dlg->m_fIsGetReportFaile)
		{
			if(!dlg->m_fIsRunning)break;
			dlg->InitReportList();
		}

		if(!dlg->m_fIsRunning)break;
		Sleep(2000);
	}
	dlg->m_updataThread = NULL;
	return 0;
}

BOOL CHVEDemoExDlg::IsTableExists(CString strTableName)
{
	if(!m_fIsADOConnected)
	{
		CString strSql;
		CString strIP;
		strIP.Format("%d.%d.%d.%d", (m_strSourcePath>>24)&0xFF, (m_strSourcePath>>16)&0xFF, 
			(m_strSourcePath>>8)&0xFF, (m_strSourcePath)&0xFF);
		strSql.Format("Provider=SQLOLEDB.1;Password=%s;User ID=%s;Initial Catalog=%s;Data Source=%s",
			m_strPassWord, m_strUserName, m_strDataBaseName, strIP);
		if(!m_fIsRunning)return FALSE;
		if(!DoConnectDataBase(strSql))
		{
			return FALSE;
		}
	}

	try
	{
		CString strSql;
		HRESULT hr = E_FAIL;
		strSql.Format("SELECT name FROM sysobjects WHERE (name = '%s')", strTableName.GetBuffer());
		_RecordsetPtr pPtr;
		hr = pPtr.CreateInstance("ADODB.Recordset");
		if(hr != S_OK) return FALSE;
		hr = pPtr->Open(_variant_t(strSql), m_pConnection.GetInterfacePtr(), 
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

	}
	catch (_com_error e)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CHVEDemoExDlg::CreateView(COleDateTime& startTime, COleDateTime& endTime)
{
	if(!m_fIsADOConnected)
	{
		CString strSql;
		CString strIP;
		strIP.Format("%d.%d.%d.%d", (m_strSourcePath>>24)&0xFF, (m_strSourcePath>>16)&0xFF, 
			(m_strSourcePath>>8)&0xFF, (m_strSourcePath)&0xFF);
		strSql.Format("Provider=SQLOLEDB.1;Password=%s;User ID=%s;Initial Catalog=%s;Data Source=%s",
			m_strPassWord, m_strUserName, m_strDataBaseName, strIP);
		if(!m_fIsRunning)return FALSE;
		if(!DoConnectDataBase(strSql))
		{
			return FALSE;
		}
	}
	CString strViewName = "Hve_Data_View_";
	strViewName += g_strMacAddr;
	CString strSql;
	strSql.Format("IF EXISTS (SELECT TABLE_NAME FROM INFORMATION_SCHEMA.VIEWS WHERE TABLE_NAME = N'%s') DROP VIEW %s",
		strViewName.GetBuffer(), strViewName.GetBuffer());
	_variant_t RecordsetAffected;
	try
	{
		m_pConnection->Execute((_bstr_t)strSql.GetBuffer(), &RecordsetAffected, adCmdText);
	}
	catch (_com_error e)
	{
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
				strTableList.Format("(SELECT * FROM %s)", strTmp);
				else
				strTableList.AppendFormat("union (SELECT * FROM %s)", strTmp.GetBuffer());
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
		try
		{
			CString strTmpSql;
			HRESULT hr = E_FAIL;
			strTmpSql = "SELECT name FROM sysobjects WHERE (xtype = 'u' and name LIKE 'Hve_Data_%')";			//add "xtype = 'u' and ",ensure select table, by zhut 20121112
			_RecordsetPtr pPtr;
			hr = pPtr.CreateInstance("ADODB.Recordset");
			if(hr != S_OK) return FALSE;
			hr = pPtr->Open(_variant_t(strTmpSql), m_pConnection.GetInterfacePtr(), 
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
		catch (_com_error e)
		{
			return FALSE;
			
		}

	}
	strSql.Format("CREATE VIEW %s AS %s", strViewName.GetBuffer(), strTableList.GetBuffer());
	try
	{
		m_pConnection->Execute((_bstr_t)strSql.GetBuffer(), &RecordsetAffected, adCmdText);
	}
	catch (_com_error e)
	{
		return FALSE;
	}
	return TRUE;
}

void CHVEDemoExDlg::DropView()
{
	if(!m_fIsADOConnected)
	{
		CString strSql;
		CString strIP;
		strIP.Format("%d.%d.%d.%d", (m_strSourcePath>>24)&0xFF, (m_strSourcePath>>16)&0xFF, 
			(m_strSourcePath>>8)&0xFF, (m_strSourcePath)&0xFF);
		strSql.Format("Provider=SQLOLEDB.1;Password=%s;User ID=%s;Initial Catalog=%s;Data Source=%s",
			m_strPassWord, m_strUserName, m_strDataBaseName, strIP);
		if(!m_fIsRunning)return;
		if(!DoConnectDataBase(strSql))
		{
			return;
		}
	}
	CString strViewName = "Hve_Data_View_";
	strViewName += g_strMacAddr;
	CString strSql;
	strSql.Format("IF EXISTS (SELECT TABLE_NAME FROM INFORMATION_SCHEMA.VIEWS WHERE TABLE_NAME = N'%s') DROP VIEW %s",
		strViewName.GetBuffer(), strViewName.GetBuffer());
	_variant_t RecordsetAffected;
	try
	{
		m_pConnection->Execute((_bstr_t)strSql.GetBuffer(), &RecordsetAffected, adCmdText);
	}
	catch (_com_error e)
	{
		return;
	}
}

BOOL CHVEDemoExDlg::GetCountByTime(CString strStartTime, CString strEndTime, QureStr& stResult)
{
	if(!m_fIsADOConnected)
	{
		CString strSql;
		CString strIP;
		strIP.Format("%d.%d.%d.%d", (m_strSourcePath>>24)&0xFF, (m_strSourcePath>>16)&0xFF, 
			(m_strSourcePath>>8)&0xFF, (m_strSourcePath)&0xFF);
		strSql.Format("Provider=SQLOLEDB.1;Password=%s;User ID=%s;Initial Catalog=%s;Data Source=%s",
			m_strPassWord, m_strUserName, m_strDataBaseName, strIP);
		if(!m_fIsRunning)return FALSE;
		if(!DoConnectDataBase(strSql))
		{
			return FALSE;
		}
	}
	try
	{
		CString strSql;
		CString strPeriodTable = "strPeriodTable_";
		strPeriodTable += g_strMacAddr;
		CString strViewName = "Hve_Data_View_";
		strViewName += g_strMacAddr;
		_variant_t var;
		HRESULT hr = S_OK;
		m_staticCurrentStatue.SetWindowText("当前状态：正在获取数据");

		//strSql = "IF EXISTS(SELECT * FROM sysobjects WHERE name='PeriodTable' AND xtype='U') ";
		//strSql += "DROP TABLE PeriodTable";
		strSql.Format("IF EXISTS(SELECT * FROM sysobjects WHERE name='%s' AND xtype='U') DROP TABLE %s", 
			strPeriodTable.GetBuffer(), strPeriodTable.GetBuffer());
		_variant_t RecordsetAffected;
		m_pConnection->Execute((_bstr_t)strSql.GetBuffer(0), &RecordsetAffected, adCmdText);
		RecordsetAffected.Clear();
		if(!m_fIsRunning)return FALSE;
		strSql.Format("SELECT Record_Id, Pass_Time, Road_No, '%s' AS PeriodTime, CASE WHEN Car_Speed=0 THEN 0 ELSE 10/(CONVERT(float, Car_Speed)*1000/3600) END AS PreCarCross \
					  		INTO %s FROM %s WHERE Pass_Time BETWEEN '%s' AND '%s' AND Location_Id=%d ORDER BY %s.Pass_Time",
					  		strStartTime, strPeriodTable.GetBuffer(), strViewName.GetBuffer(), strStartTime, strEndTime,
							m_rgLocation[m_nCurrentRoadID].nLocation_Id, strViewName.GetBuffer());
		m_pConnection->Execute((_bstr_t)strSql.GetBuffer(0), &RecordsetAffected, adCmdText);
		RecordsetAffected.Clear();
		
		if(!m_fIsRunning)return FALSE;
		int iIndex = 0;
		//_RecordsetPtr pRenRecordset;
		hr = pRenRecordset.CreateInstance("ADODB.Recordset");
		if(m_fIsReverse)
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
					strPeriodTable.GetBuffer(), strViewName.GetBuffer(), strPeriodTable.GetBuffer(),
					strViewName.GetBuffer(), strPeriodTable.GetBuffer(), strViewName.GetBuffer(), strPeriodTable.GetBuffer(), iIndex);
		
		hr = pRenRecordset->Open(_variant_t(strSql), m_pConnection.GetInterfacePtr(), 
			adOpenDynamic, adLockOptimistic, adCmdText);
		if(SUCCEEDED(hr))
		{
			var = pRenRecordset->GetCollect((long)0);
			stResult.Count_1 = (int)var.lVal;
			var = pRenRecordset->GetCollect((long)1);
			stResult.Speed_1 = (int)var.lVal;
			var = pRenRecordset->GetCollect((long)2);
			stResult.Cross_1 = (double)var.dblVal * 100;
			pRenRecordset->Close();
			pRenRecordset = NULL;
		}
	
		if(!m_fIsRunning)return FALSE;
		hr = pRenRecordset.CreateInstance("ADODB.Recordset");
		if(m_fIsReverse)
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
			strPeriodTable.GetBuffer(), strViewName.GetBuffer(), strPeriodTable.GetBuffer(),
			strViewName.GetBuffer(), strPeriodTable.GetBuffer(), strViewName.GetBuffer(), strPeriodTable.GetBuffer(), iIndex);

		hr = pRenRecordset->Open(_variant_t(strSql), m_pConnection.GetInterfacePtr(), 
			adOpenDynamic, adLockOptimistic, adCmdText);
		if(SUCCEEDED(hr))
		{
			var = pRenRecordset->GetCollect((long)0);
			stResult.Count_2 = (int)var.lVal;
			var = pRenRecordset->GetCollect((long)1);
			stResult.Speed_2 = (int)var.lVal;
			var = pRenRecordset->GetCollect((long)2);
			stResult.Cross_2 = (double)var.dblVal * 100;
			pRenRecordset->Close();
			pRenRecordset = NULL;
		}
		
		if(!m_fIsRunning)return FALSE;
		hr = pRenRecordset.CreateInstance("ADODB.Recordset");
		if(m_fIsReverse)
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
			strPeriodTable.GetBuffer(), strViewName.GetBuffer(), strPeriodTable.GetBuffer(),
			strViewName.GetBuffer(), strPeriodTable.GetBuffer(), strViewName.GetBuffer(), strPeriodTable.GetBuffer(), iIndex);

		hr = pRenRecordset->Open(_variant_t(strSql), m_pConnection.GetInterfacePtr(), 
			adOpenDynamic, adLockOptimistic, adCmdText);
		if(SUCCEEDED(hr))
		{
			var = pRenRecordset->GetCollect((long)0);
			stResult.Count_3 = (int)var.lVal;
			var = pRenRecordset->GetCollect((long)1);
			stResult.Speed_3 = (int)var.lVal;
			var = pRenRecordset->GetCollect((long)2);
			stResult.Cross_3 = (double)var.dblVal * 100;
			pRenRecordset->Close();
			pRenRecordset = NULL;
		}

		if(!m_fIsRunning)return FALSE;
		hr = pRenRecordset.CreateInstance("ADODB.Recordset");
		if(m_fIsReverse)
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
			strPeriodTable.GetBuffer(), strViewName.GetBuffer(), strPeriodTable.GetBuffer(),
			strViewName.GetBuffer(), strPeriodTable.GetBuffer(), strViewName.GetBuffer(), strPeriodTable.GetBuffer(), iIndex);

		hr = pRenRecordset->Open(_variant_t(strSql), m_pConnection.GetInterfacePtr(), 
			adOpenDynamic, adLockOptimistic, adCmdText);
		if(SUCCEEDED(hr))
		{
			var = pRenRecordset->GetCollect((long)0);
			stResult.Count_4 = (int)var.lVal;
			var = pRenRecordset->GetCollect((long)1);
			stResult.Speed_4 = (int)var.lVal;
			var = pRenRecordset->GetCollect((long)2);
			stResult.Cross_4 = (double)var.dblVal * 100;
			pRenRecordset->Close();
			pRenRecordset = NULL;
		}

		strSql.Format("IF EXISTS(SELECT * FROM sysobjects WHERE name='%s' AND xtype='U') DROP TABLE %s", 
			strPeriodTable.GetBuffer(), strPeriodTable.GetBuffer());
		m_pConnection->Execute((_bstr_t)strSql.GetBuffer(0), &RecordsetAffected, adCmdText);
	}
	catch (_com_error e)
	{
		m_pConnection->Close();
		m_fIsADOConnected = FALSE;
		m_staticCurrentStatue.SetWindowText("当前状态：获取数据失败");
		return FALSE;
	}
	m_staticCurrentStatue.SetWindowText("当前状态：获取数据成功");

	return TRUE;
}

void CHVEDemoExDlg::UpdataMinInfo(COleDateTime& startTime, COleDateTime& endTime)
{
	CString strStartTime;
	CString strEndTime;
	strStartTime = startTime.Format("%Y-%m-%d %H:%M:%S");
	strEndTime = endTime.Format("%Y-%m-%d %H:%M:%S");
	QureStr	stResult;
	memset(&stResult, 0, sizeof(QureStr));
	if(!m_fIsRunning)return;
	if(CreateView(startTime, endTime) == FALSE) return;
	if(GetCountByTime(strStartTime, strEndTime, stResult))
	{
		char szInfo[30];
		sprintf(szInfo, "%d辆", stResult.Count_1);
		m_staticMinRoad1CarNum.SetWindowText(szInfo);
		sprintf(szInfo, "%dkm/h", stResult.Speed_1);
		m_staticMinRoad1AvrSpeed.SetWindowText(szInfo);
		sprintf(szInfo, "%.3f%c", stResult.Cross_1, '%');
		m_staticMinRoad1Avr.SetWindowText(szInfo);
		if(stResult.Count_1 <= 0)
		{
			m_staticMinRoad1Statu.SetWindowText("无数据");
		}
		else
		{
			if(stResult.Speed_1 >= 40)
			m_staticMinRoad1Statu.SetWindowText("畅通");	//白色
			else if(stResult.Speed_1 >= 32 && stResult.Speed_1 < 40)
			m_staticMinRoad1Statu.SetWindowText("基本畅通");//绿色
			else if(stResult.Speed_1 >= 24 && stResult.Speed_1 < 32)
			m_staticMinRoad1Statu.SetWindowText("拥挤");//黄色
			else
			m_staticMinRoad1Statu.SetWindowText("堵塞");//橙色
		}

		sprintf(szInfo, "%d辆", stResult.Count_2);
		m_staticMinRoad2CarNum.SetWindowText(szInfo);
		sprintf(szInfo, "%dkm/h", stResult.Speed_2);
		m_staticMinRoad2AvrSpeed.SetWindowText(szInfo);
		sprintf(szInfo, "%.3f%c", stResult.Cross_2, '%');
		m_staticMinRoad2Avr.SetWindowText(szInfo);
		if(stResult.Count_2 <= 0)
		{
			m_staticMinRoad2Statu.SetWindowText("无数据");
		}
		else
		{
			if(stResult.Speed_2 >= 40)
				m_staticMinRoad2Statu.SetWindowText("畅通");	//白色
			else if(stResult.Speed_2 >= 32 && stResult.Speed_2 < 40)
				m_staticMinRoad2Statu.SetWindowText("基本畅通");//绿色
			else if(stResult.Speed_2 >= 24 && stResult.Speed_2 < 32)
				m_staticMinRoad2Statu.SetWindowText("拥挤");//黄色
			else
				m_staticMinRoad2Statu.SetWindowText("堵塞");//橙色
		}

		sprintf(szInfo, "%d辆", stResult.Count_3);
		m_staticMinRoad3CarNum.SetWindowText(szInfo);
		sprintf(szInfo, "%dkm/h", stResult.Speed_3);
		m_staticMinRoad3AvrSpeed.SetWindowText(szInfo);
		sprintf(szInfo, "%.3f%c", stResult.Cross_3, '%');
		m_staticMinRoad3Avr.SetWindowText(szInfo);
		if(stResult.Count_3 <= 0)
		{
			m_staticMinRoad3Statu.SetWindowText("无数据");
		}
		else
		{
			if(stResult.Speed_3 >= 40)
				m_staticMinRoad3Statu.SetWindowText("畅通");	//白色
			else if(stResult.Speed_3 >= 32 && stResult.Speed_3 < 40)
				m_staticMinRoad3Statu.SetWindowText("基本畅通");//绿色
			else if(stResult.Speed_3 >= 24 && stResult.Speed_3 < 32)
				m_staticMinRoad3Statu.SetWindowText("拥挤");//黄色
			else
				m_staticMinRoad3Statu.SetWindowText("堵塞");//橙色
		}

		sprintf(szInfo, "%d辆", stResult.Count_4);
		m_staticMinRoad4CarNum.SetWindowText(szInfo);
		sprintf(szInfo, "%dkm/h", stResult.Speed_4);
		m_staticMinRoad4AvrSpeed.SetWindowText(szInfo);
		sprintf(szInfo, "%.3f%c", stResult.Cross_4, '%');
		m_staticMinRoad4Avr.SetWindowText(szInfo);
		if(stResult.Count_4 <= 0)
		{
			m_staticMinRoad4Statu.SetWindowText("无数据");
		}
		else
		{
			if(stResult.Speed_4 >= 40)
				m_staticMinRoad4Statu.SetWindowText("畅通");	//白色
			else if(stResult.Speed_4 >= 32 && stResult.Speed_4 < 40)
				m_staticMinRoad4Statu.SetWindowText("基本畅通");//绿色
			else if(stResult.Speed_4 >= 24 && stResult.Speed_4 < 32)
				m_staticMinRoad4Statu.SetWindowText("拥挤");//黄色
			else
				m_staticMinRoad4Statu.SetWindowText("堵塞");//橙色
		}
		m_fIsGetMiniDataFaile = FALSE;
	}
	else
	{
		m_fIsGetMiniDataFaile = TRUE;
	}
	DropView();

	//if(!m_fIsConnect) return;
	//CString strStartTime;
	//CString strEndTime;
	//CString strStartTimeTM;
	//strStartTime = startTime.Format("%Y-%m-%d %H:%M:%S");
	//strEndTime = endTime.Format("%Y-%m-%d %H:%M:%S");
	//strStartTimeTM = startTime.Format("%Y-%m-%d %H:%M:%S");
	//try
	//{
	//	CAxRecordset* cRec = new CAxRecordset;
	//	cRec->Create();
	//	cRec->CursorType(adOpenForwardOnly);
	//	cRec->CacheSize(100);
	//	CString strQuery;
	//	strQuery = "IF EXISTS(SELECT * FROM sysobjects WHERE name='PeriodTable' AND xtype='U') ";
	//	strQuery += "DROP TABLE PeriodTable";
	//	cRec->Open(strQuery, &m_cAxConn);
	//	cRec->Close();
	//	
	//	if(!m_fIsRunning)return;
	//	strQuery.Format(
	//		"SELECT Record_Id, Pass_Time, '%s' AS PeriodTime, CASE WHEN Car_Speed=0 THEN 0 ELSE 10/(CONVERT(float, Car_Speed)*1000/3600) END AS PreCarCross \
	//		INTO PeriodTable FROM Hve_Data_View WHERE Pass_Time BETWEEN '%s' AND '%s' AND Location_Id=%d ORDER BY Hve_Data_View.Pass_Time",
	//		strStartTimeTM, strStartTime, strEndTime, m_rgLocation[m_nCurrentRoadID].nLocation_Id);
	//	cRec->Open(strQuery, &m_cAxConn);
	//	cRec->Close();
	//	
	//	if(!m_fIsRunning)return;
	//	strQuery.Format(
	//		"SELECT COUNT(PeriodTime) AS CarCount"
	//		", AVG(CONVERT(bigint, Car_Speed)) AS CarSpeed "
	//		", 100*AVG(PreCarCross)/60/10 AS CarCross"
	//		", CASE WHEN SUM(CASE WHEN Special_Result = 1 THEN 1 ELSE 0 END) > 0 THEN 1 ELSE 0 END AS Violate"
	//		" FROM Hve_Data_View, PeriodTable "
	//		" WHERE (Hve_Data_View.Pass_Time = PeriodTable.Pass_Time AND Hve_Data_View.Record_Id = PeriodTable.Record_Id AND Hve_Data_View.Road_No = 0) ");
	//	cRec->Open(strQuery, &m_cAxConn);
	//	
	//	int nCarCount;
	//	int nCarSpeed;
	//	double dCarCross;
	//	cRec->FX_Integer(FALSE, "CarCount", nCarCount);
	//	cRec->FX_Integer(FALSE, "CarSpeed", nCarSpeed);
	//	cRec->FX_Float(FALSE, "CarCross", dCarCross);
	//	if(nCarCount <= 0)
	//	{
	//		nCarCount = 0;
	//		nCarSpeed = 0;
	//		dCarCross = 0;
	//	}
	//	char szTmp[20];
	//	sprintf(szTmp, "%d辆", nCarCount);
	//	m_staticMinRoad1CarNum.SetWindowText(szTmp);
	//	sprintf(szTmp, "%dkm/h", nCarSpeed);
	//	m_staticMinRoad1AvrSpeed.SetWindowText(szTmp);
	//	sprintf(szTmp, "%.3f%c", dCarCross, '%');
	//	m_staticMinRoad1Avr.SetWindowText(szTmp);
	//	m_staticMinRoad1Statu.SetWindowText("通畅");
	//	cRec->Close();
	//	
	//	if(!m_fIsRunning) return;
	//	strQuery.Format(
	//		"SELECT COUNT(PeriodTime) AS CarCount"
	//		", AVG(CONVERT(bigint, Car_Speed)) AS CarSpeed "
	//		", 100*AVG(PreCarCross)/60/10 AS CarCross"
	//		", CASE WHEN SUM(CASE WHEN Special_Result = 1 THEN 1 ELSE 0 END) > 0 THEN 1 ELSE 0 END AS Violate"
	//		" FROM Hve_Data_View, PeriodTable "
	//		" WHERE (Hve_Data_View.Pass_Time = PeriodTable.Pass_Time AND Hve_Data_View.Record_Id = PeriodTable.Record_Id AND Hve_Data_View.Road_No = 1) ");
	//	cRec->Open(strQuery, &m_cAxConn);

	//	cRec->FX_Integer(FALSE, "CarCount", nCarCount);
	//	cRec->FX_Integer(FALSE, "CarSpeed", nCarSpeed);
	//	cRec->FX_Float(FALSE, "CarCross", dCarCross);
	//	if(nCarCount <= 0)
	//	{
	//		nCarCount = 0;
	//		nCarSpeed = 0;
	//		dCarCross = 0;
	//	}
	//	sprintf(szTmp, "%d辆", nCarCount);
	//	m_staticMinRoad2CarNum.SetWindowText(szTmp);
	//	sprintf(szTmp, "%dkm/h", nCarSpeed);
	//	m_staticMinRoad2AvrSpeed.SetWindowText(szTmp);
	//	sprintf(szTmp, "%.3f%c", dCarCross, '%');
	//	m_staticMinRoad2Avr.SetWindowText(szTmp);
	//	m_staticMinRoad2Statu.SetWindowText("通畅");
	//	cRec->Close();
	//	
	//	if(!m_fIsRunning)return;
	//	strQuery.Format(
	//		"SELECT COUNT(PeriodTime) AS CarCount"
	//		", AVG(CONVERT(bigint, Car_Speed)) AS CarSpeed "
	//		", 100*AVG(PreCarCross)/60/10 AS CarCross"
	//		", CASE WHEN SUM(CASE WHEN Special_Result = 1 THEN 1 ELSE 0 END) > 0 THEN 1 ELSE 0 END AS Violate"
	//		" FROM Hve_Data_View, PeriodTable "
	//		" WHERE (Hve_Data_View.Pass_Time = PeriodTable.Pass_Time AND Hve_Data_View.Record_Id = PeriodTable.Record_Id AND Hve_Data_View.Road_No = 2) ");
	//	cRec->Open(strQuery, &m_cAxConn);

	//	cRec->FX_Integer(FALSE, "CarCount", nCarCount);
	//	cRec->FX_Integer(FALSE, "CarSpeed", nCarSpeed);
	//	cRec->FX_Float(FALSE, "CarCross", dCarCross);
	//	if(nCarCount <= 0)
	//	{
	//		nCarCount = 0;
	//		nCarSpeed = 0;
	//		dCarCross = 0;
	//	}
	//	sprintf(szTmp, "%d辆", nCarCount);
	//	m_staticMinRoad3CarNum.SetWindowText(szTmp);
	//	sprintf(szTmp, "%dkm/h", nCarSpeed);
	//	m_staticMinRoad3AvrSpeed.SetWindowText(szTmp);
	//	sprintf(szTmp, "%.3f%c", dCarCross, '%');
	//	m_staticMinRoad3Avr.SetWindowText(szTmp);
	//	m_staticMinRoad3Statu.SetWindowText("通畅");
	//	cRec->Close();

	//	if(!m_fIsRunning)return;
	//	strQuery.Format(
	//		"SELECT COUNT(PeriodTime) AS CarCount"
	//		", AVG(CONVERT(bigint, Car_Speed)) AS CarSpeed "
	//		", 100*AVG(PreCarCross)/60/10 AS CarCross"
	//		", CASE WHEN SUM(CASE WHEN Special_Result = 1 THEN 1 ELSE 0 END) > 0 THEN 1 ELSE 0 END AS Violate"
	//		" FROM Hve_Data_View, PeriodTable "
	//		" WHERE (Hve_Data_View.Pass_Time = PeriodTable.Pass_Time AND Hve_Data_View.Record_Id = PeriodTable.Record_Id AND Hve_Data_View.Road_No = 3) ");
	//	cRec->Open(strQuery, &m_cAxConn);

	//	cRec->FX_Integer(FALSE, "CarCount", nCarCount);
	//	cRec->FX_Integer(FALSE, "CarSpeed", nCarSpeed);
	//	cRec->FX_Float(FALSE, "CarCross", dCarCross);
	//	if(nCarCount <= 0)
	//	{
	//		nCarCount = 0;
	//		nCarSpeed = 0;
	//		dCarCross = 0;
	//	}
	//	sprintf(szTmp, "%d辆", nCarCount);
	//	m_staticMinRoad4CarNum.SetWindowText(szTmp);
	//	sprintf(szTmp, "%dkm/h", nCarSpeed);
	//	m_staticMinRoad4AvrSpeed.SetWindowText(szTmp);
	//	sprintf(szTmp, "%.3f%c", dCarCross, '%');
	//	m_staticMinRoad4Avr.SetWindowText(szTmp);
	//	m_staticMinRoad4Statu.SetWindowText("通畅");
	//	cRec->Close();
	//	delete cRec;
	//	cRec = NULL;
	//}
	//catch (dbAx::CAxException* e)
	//{
	//	delete e;
	//}
}


void CHVEDemoExDlg::UpdataHourInfo(COleDateTime& startTime, COleDateTime& endTime)
{
	CString strStartTime;
	CString strEndTime;
	strStartTime = startTime.Format("%Y-%m-%d %H:%M:%S");
	strEndTime = endTime.Format("%Y-%m-%d %H:%M:%S");
	QureStr	stResult;
	memset(&stResult, 0, sizeof(QureStr));
	if(!m_fIsRunning)return;
	if(CreateView(startTime, endTime) != TRUE) return;
	if(GetCountByTime(strStartTime, strEndTime, stResult))
	{
		char szInfo[30];
		sprintf(szInfo, "%d辆", stResult.Count_1);
		m_staticHourRoad1CarNum.SetWindowText(szInfo);
		sprintf(szInfo, "%dkm/h", stResult.Speed_1);
		m_staticHourRoad1AvrSpeed.SetWindowText(szInfo);
		sprintf(szInfo, "%.3f%c", stResult.Cross_1, '%');
		m_staticHourRoad1Avr.SetWindowText(szInfo);
		if(stResult.Count_1 <= 0)
		{
			m_staticHourRoad1Statu.SetWindowText("无数据");
		}
		else
		{
			if(stResult.Speed_1 >= 40)
				m_staticHourRoad1Statu.SetWindowText("畅通");	//白色
			else if(stResult.Speed_1 >= 32 && stResult.Speed_1 < 40)
				m_staticHourRoad1Statu.SetWindowText("基本畅通");//绿色
			else if(stResult.Speed_1 >= 24 && stResult.Speed_1 < 32)
				m_staticHourRoad1Statu.SetWindowText("拥挤");//黄色
			else
				m_staticHourRoad1Statu.SetWindowText("堵塞");//橙色
		}

		sprintf(szInfo, "%d辆", stResult.Count_2);
		m_staticHourRoad2CarNum.SetWindowText(szInfo);
		sprintf(szInfo, "%dkm/h", stResult.Speed_2);
		m_staticHourRoad2AvrSpeed.SetWindowText(szInfo);
		sprintf(szInfo, "%.3f%c", stResult.Cross_2, '%');
		m_staticHourRoad2Avr.SetWindowText(szInfo);
		if(stResult.Count_2 <= 0)
		{
			m_staticHourRoad2Statu.SetWindowText("无数据");
		}
		else
		{
			if(stResult.Speed_2 >= 40)
				m_staticHourRoad2Statu.SetWindowText("畅通");	//白色
			else if(stResult.Speed_2 >= 32 && stResult.Speed_2 < 40)
				m_staticHourRoad2Statu.SetWindowText("基本畅通");//绿色
			else if(stResult.Speed_2 >= 24 && stResult.Speed_2 < 32)
				m_staticHourRoad2Statu.SetWindowText("拥挤");//黄色
			else
				m_staticHourRoad2Statu.SetWindowText("堵塞");//橙色
		}

		sprintf(szInfo, "%d辆", stResult.Count_3);
		m_staticHourRoad3CarNum.SetWindowText(szInfo);
		sprintf(szInfo, "%dkm/h", stResult.Speed_3);
		m_staticHourRoad3AvrSpeed.SetWindowText(szInfo);
		sprintf(szInfo, "%.3f%c", stResult.Cross_3, '%');
		m_staticHourRoad3Avr.SetWindowText(szInfo);
		if(stResult.Count_3 <= 0)
		{
			m_staticHourRoad3Statu.SetWindowText("无数据");
		}
		else
		{
			if(stResult.Speed_3 >= 40)
				m_staticHourRoad3Statu.SetWindowText("畅通");	//白色
			else if(stResult.Speed_3 >= 32 && stResult.Speed_3 < 40)
				m_staticHourRoad3Statu.SetWindowText("基本畅通");//绿色
			else if(stResult.Speed_3 >= 24 && stResult.Speed_3 < 32)
				m_staticHourRoad3Statu.SetWindowText("拥挤");//黄色
			else
				m_staticHourRoad3Statu.SetWindowText("堵塞");//橙色
		}

		sprintf(szInfo, "%d辆", stResult.Count_4);
		m_staticHourRoad4CarNum.SetWindowText(szInfo);
		sprintf(szInfo, "%dkm/h", stResult.Speed_4);
		m_staticHourRoad4AvrSpeed.SetWindowText(szInfo);
		sprintf(szInfo, "%.3f%c", stResult.Cross_4, '%');
		m_staticHourRoad4Avr.SetWindowText(szInfo);
		if(stResult.Count_4 <= 0)
		{
			m_staticHourRoad4Statu.SetWindowText("无数据");
		}
		else
		{
			if(stResult.Speed_4 >= 40)
				m_staticHourRoad4Statu.SetWindowText("畅通");	//白色
			else if(stResult.Speed_4 >= 32 && stResult.Speed_4 < 40)
				m_staticHourRoad4Statu.SetWindowText("基本畅通");//绿色
			else if(stResult.Speed_4 >= 24 && stResult.Speed_4 < 32)
				m_staticHourRoad4Statu.SetWindowText("拥挤");//黄色
			else
				m_staticHourRoad4Statu.SetWindowText("堵塞");//橙色
		}
		m_fIsGetHourDataFaile = FALSE;
	}
	else
	{
		m_fIsGetHourDataFaile = TRUE;
	}
	DropView();
	//if(!m_fIsConnect) return;
	//CString strStartTime;
	//CString strEndTime;
	//CString strStartTimeTM;
	//strStartTime = startTime.Format("%Y-%m-%d %H:%M:%S");
	//strEndTime = endTime.Format("%Y-%m-%d %H:%M:%S");
	//strStartTimeTM = startTime.Format("%Y-%m-%d %H:%M:%S");
	//try
	//{
	//	CAxRecordset cRec;
	//	cRec.Create();
	//	cRec.CursorType(adOpenForwardOnly);
	//	cRec.CacheSize(100);
	//	CString strQuery;
	//	strQuery = "IF EXISTS(SELECT * FROM sysobjects WHERE name='PeriodTable' AND xtype='U') ";
	//	strQuery += "DROP TABLE PeriodTable";
	//	cRec.Open(strQuery, &m_cAxConn);
	//	cRec.Close();

	//	if(!m_fIsRunning)return;
	//	strQuery.Format(
	//		"SELECT Record_Id, Pass_Time, '%s' AS PeriodTime, CASE WHEN Car_Speed=0 THEN 0 ELSE 10/(CONVERT(float, Car_Speed)*1000/3600) END AS PreCarCross \
	//		INTO PeriodTable FROM Hve_Data_View WHERE Pass_Time BETWEEN '%s' AND '%s' AND Location_Id=%d ORDER BY Hve_Data_View.Pass_Time",
	//		strStartTimeTM, strStartTime, strEndTime, m_rgLocation[m_nCurrentRoadID].nLocation_Id);
	//	cRec.Open(strQuery, &m_cAxConn);
	//	cRec.Close();

	//	if(!m_fIsRunning)return;
	//	strQuery.Format(
	//		"SELECT COUNT(PeriodTime) AS CarCount"
	//		", AVG(CONVERT(bigint, Car_Speed)) AS CarSpeed "
	//		", 100*AVG(PreCarCross)/60/10 AS CarCross"
	//		", CASE WHEN SUM(CASE WHEN Special_Result = 1 THEN 1 ELSE 0 END) > 0 THEN 1 ELSE 0 END AS Violate"
	//		" FROM Hve_Data_View, PeriodTable "
	//		" WHERE (Hve_Data_View.Pass_Time = PeriodTable.Pass_Time AND Hve_Data_View.Record_Id = PeriodTable.Record_Id AND Hve_Data_View.Road_No = 0) ");
	//	cRec.Open(strQuery, &m_cAxConn);

	//	int nCarCount;
	//	int nCarSpeed;
	//	double dCarCross;
	//	cRec.FX_Integer(FALSE, "CarCount", nCarCount);
	//	cRec.FX_Integer(FALSE, "CarSpeed", nCarSpeed);
	//	cRec.FX_Float(FALSE, "CarCross", dCarCross);
	//	if(nCarCount <= 0)
	//	{
	//		nCarCount = 0;
	//		nCarSpeed = 0;
	//		dCarCross = 0;
	//	}
	//	char szTmp[20];
	//	sprintf(szTmp, "%d辆", nCarCount);
	//	m_staticHourRoad1CarNum.SetWindowText(szTmp);
	//	sprintf(szTmp, "%dkm/h", nCarSpeed);
	//	m_staticHourRoad1AvrSpeed.SetWindowText(szTmp);
	//	sprintf(szTmp, "%.3f%c", dCarCross, '%');
	//	m_staticHourRoad1Avr.SetWindowText(szTmp);
	//	m_staticHourRoad1Statu.SetWindowText("通畅");
	//	cRec.Close();

	//	if(!m_fIsRunning)return;
	//	strQuery.Format(
	//		"SELECT COUNT(PeriodTime) AS CarCount"
	//		", AVG(CONVERT(bigint, Car_Speed)) AS CarSpeed "
	//		", 100*AVG(PreCarCross)/60/10 AS CarCross"
	//		", CASE WHEN SUM(CASE WHEN Special_Result = 1 THEN 1 ELSE 0 END) > 0 THEN 1 ELSE 0 END AS Violate"
	//		" FROM Hve_Data_View, PeriodTable "
	//		" WHERE (Hve_Data_View.Pass_Time = PeriodTable.Pass_Time AND Hve_Data_View.Record_Id = PeriodTable.Record_Id AND Hve_Data_View.Road_No = 1) ");
	//	cRec.Open(strQuery, &m_cAxConn);

	//	cRec.FX_Integer(FALSE, "CarCount", nCarCount);
	//	cRec.FX_Integer(FALSE, "CarSpeed", nCarSpeed);
	//	cRec.FX_Float(FALSE, "CarCross", dCarCross);
	//	if(nCarCount <= 0)
	//	{
	//		nCarCount = 0;
	//		nCarSpeed = 0;
	//		dCarCross = 0;
	//	}
	//	sprintf(szTmp, "%d辆", nCarCount);
	//	m_staticHourRoad2CarNum.SetWindowText(szTmp);
	//	sprintf(szTmp, "%dkm/h", nCarSpeed);
	//	m_staticHourRoad2AvrSpeed.SetWindowText(szTmp);
	//	sprintf(szTmp, "%.3f%c", dCarCross, '%');
	//	m_staticHourRoad2Avr.SetWindowText(szTmp);
	//	m_staticHourRoad2Statu.SetWindowText("通畅");
	//	cRec.Close();

	//	if(!m_fIsRunning)return;
	//	strQuery.Format(
	//		"SELECT COUNT(PeriodTime) AS CarCount"
	//		", AVG(CONVERT(bigint, Car_Speed)) AS CarSpeed "
	//		", 100*AVG(PreCarCross)/60/10 AS CarCross"
	//		", CASE WHEN SUM(CASE WHEN Special_Result = 1 THEN 1 ELSE 0 END) > 0 THEN 1 ELSE 0 END AS Violate"
	//		" FROM Hve_Data_View, PeriodTable "
	//		" WHERE (Hve_Data_View.Pass_Time = PeriodTable.Pass_Time AND Hve_Data_View.Record_Id = PeriodTable.Record_Id AND Hve_Data_View.Road_No = 2) ");
	//	cRec.Open(strQuery, &m_cAxConn);

	//	cRec.FX_Integer(FALSE, "CarCount", nCarCount);
	//	cRec.FX_Integer(FALSE, "CarSpeed", nCarSpeed);
	//	cRec.FX_Float(FALSE, "CarCross", dCarCross);
	//	if(nCarCount <= 0)
	//	{
	//		nCarCount = 0;
	//		nCarSpeed = 0;
	//		dCarCross = 0;
	//	}
	//	sprintf(szTmp, "%d辆", nCarCount);
	//	m_staticHourRoad3CarNum.SetWindowText(szTmp);
	//	sprintf(szTmp, "%dkm/h", nCarSpeed);
	//	m_staticHourRoad3AvrSpeed.SetWindowText(szTmp);
	//	sprintf(szTmp, "%.3f%c", dCarCross, '%');
	//	m_staticHourRoad3Avr.SetWindowText(szTmp);
	//	m_staticHourRoad3Statu.SetWindowText("通畅");
	//	cRec.Close();

	//	if(!m_fIsRunning)return;
	//	strQuery.Format(
	//		"SELECT COUNT(PeriodTime) AS CarCount"
	//		", AVG(CONVERT(bigint, Car_Speed)) AS CarSpeed "
	//		", 100*AVG(PreCarCross)/60/10 AS CarCross"
	//		", CASE WHEN SUM(CASE WHEN Special_Result = 1 THEN 1 ELSE 0 END) > 0 THEN 1 ELSE 0 END AS Violate"
	//		" FROM Hve_Data_View, PeriodTable "
	//		" WHERE (Hve_Data_View.Pass_Time = PeriodTable.Pass_Time AND Hve_Data_View.Record_Id = PeriodTable.Record_Id AND Hve_Data_View.Road_No = 3) ");
	//	cRec.Open(strQuery, &m_cAxConn);

	//	cRec.FX_Integer(FALSE, "CarCount", nCarCount);
	//	cRec.FX_Integer(FALSE, "CarSpeed", nCarSpeed);
	//	cRec.FX_Float(FALSE, "CarCross", dCarCross);
	//	if(nCarCount <= 0)
	//	{
	//		nCarCount = 0;
	//		nCarSpeed = 0;
	//		dCarCross = 0;
	//	}
	//	sprintf(szTmp, "%d辆", nCarCount);
	//	m_staticHourRoad4CarNum.SetWindowText(szTmp);
	//	sprintf(szTmp, "%dkm/h", nCarSpeed);
	//	m_staticHourRoad4AvrSpeed.SetWindowText(szTmp);
	//	sprintf(szTmp, "%.3f%c", dCarCross, '%');
	//	m_staticHourRoad4Avr.SetWindowText(szTmp);
	//	m_staticHourRoad4Statu.SetWindowText("通畅");
	//	cRec.Close();
	//}
	//catch (dbAx::CAxException* e)
	//{
	//	delete e;
	//}
}

void CHVEDemoExDlg::ShowRoadInfo()
{
	if(!m_fIsADOConnected)return;
	if (m_iLocationCount <= 0)return;
	int iRoadCount = (int)m_rgLocation[m_nCurrentRoadID].rgRoadInfo.GetCount();
	m_listRoadInfo.DeleteAllItems();
	for(int index=0; index<iRoadCount; index++)
	{
		char szTmp[30];
		m_listRoadInfo.InsertItem(index, "", 0);
		CString strTmp;
		if(m_rgLocation[m_nCurrentRoadID].rgRoadInfo[index].nRoad_No < 0)
		{
			strTmp = "上行车道";
			strTmp += itoa(-m_rgLocation[m_nCurrentRoadID].rgRoadInfo[index].nRoad_No, szTmp, 30);
		}
		else
		{
			strTmp = "下行车道";
			strTmp += itoa(m_rgLocation[m_nCurrentRoadID].rgRoadInfo[index].nRoad_No, szTmp, 30);
		}
		m_listRoadInfo.SetItemText(index, 0, strTmp.GetBuffer(0));
		m_listRoadInfo.SetItemText(index, 1, m_rgLocation[m_nCurrentRoadID].rgRoadInfo[index].strHve_Addr);
		m_listRoadInfo.SetItemText(index, 2, m_rgLocation[m_nCurrentRoadID].strLocation_Name.GetBuffer(0));
	}
}

BOOL CHVEDemoExDlg::GetCarCountByTime(CString strStartTime, CString strEndTime, QureStr& stResult)
{
	if(!m_fIsADOConnected)
	{
		CString strSql;
		CString strIP;
		strIP.Format("%d.%d.%d.%d", (m_strSourcePath>>24)&0xFF, (m_strSourcePath>>16)&0xFF, 
			(m_strSourcePath>>8)&0xFF, (m_strSourcePath)&0xFF);
		strSql.Format("Provider=SQLOLEDB.1;Password=%s;User ID=%s;Initial Catalog=%s;Data Source=%s",
			m_strPassWord, m_strUserName, m_strDataBaseName, strIP);
		if(!m_fIsRunning)return FALSE;
		if(!DoConnectDataBase(strSql))
		{
			return FALSE;
		}
	}
	try
	{
		if(!m_fIsRunning)return FALSE;
		CString strSql;
		CString strPeriodTable = "PeriodTable_";
		strPeriodTable += g_strMacAddr;
		CString strViewName = "Hve_Data_View_";
		strViewName += g_strMacAddr;
		_variant_t var;
		HRESULT hr = S_OK;
		m_staticCurrentStatue.SetWindowText("当前状态：正在获取车流量信息");
		//strSql = "IF EXISTS(SELECT * FROM sysobjects WHERE name='PeriodTable' AND xtype='U') ";
		//strSql += "DROP TABLE PeriodTable";
		strSql.Format("IF EXISTS(SELECT * FROM sysobjects WHERE name='%s' AND xtype='U') DROP TABLE %s",
			strPeriodTable.GetBuffer(), strPeriodTable.GetBuffer());
		_variant_t RecordsetAffected;
		m_pConnection->Execute((_bstr_t)strSql.GetBuffer(0), &RecordsetAffected, adCmdText);
		RecordsetAffected.Clear();

		strSql.Format("SELECT Record_Id, Pass_Time, Road_No, '%s' AS PeriodTime, CASE WHEN Car_Speed=0 THEN 0 ELSE 10/(CONVERT(float, Car_Speed)*1000/3600) END AS PreCarCross \
						INTO %s FROM %s WHERE Pass_Time BETWEEN '%s' AND '%s' AND Location_Id=%d ORDER BY %s.Pass_Time",
						strStartTime, strPeriodTable.GetBuffer(), strViewName.GetBuffer(),
						strStartTime, strEndTime, m_rgLocation[m_nCurrentRoadID].nLocation_Id, strViewName.GetBuffer());
			
		m_pConnection->Execute((_bstr_t)strSql.GetBuffer(0), &RecordsetAffected, adCmdText);
		RecordsetAffected.Clear();

		if(!m_fIsRunning) return FALSE;
		int iIndex = 0;
		//_RecordsetPtr pRenRecordset;
		hr = pRenRecordset.CreateInstance("ADODB.Recordset");
		if(m_fIsReverse)
		{
			iIndex = -1;
		}
		else
		{
			iIndex = 1;
		}
		strSql.Format("SELECT COUNT(PeriodTime) AS CarCount"
						" FROM %s, %s "
						" WHERE (%s.Pass_Time = %s.Pass_Time AND %s.Record_Id = %s.Record_Id AND %s.Road_No = %d) ",
						strViewName.GetBuffer(), strPeriodTable.GetBuffer(),
						strViewName.GetBuffer(), strPeriodTable.GetBuffer(),
						strViewName.GetBuffer(), strPeriodTable.GetBuffer(), strPeriodTable.GetBuffer(), iIndex);
		pRenRecordset->CursorLocation = adUseServer;
		hr = pRenRecordset->Open(_variant_t(strSql), m_pConnection.GetInterfacePtr(), 
			adOpenDynamic, adLockOptimistic, adCmdText);
		if(SUCCEEDED(hr))
		{
			var = pRenRecordset->GetCollect((long)0);
			stResult.Count_1 = (int)var.lVal;
			pRenRecordset->Close();
			pRenRecordset = NULL;
		}
		if(!m_fIsRunning)return FALSE;
		hr = pRenRecordset.CreateInstance("ADODB.Recordset");
		if(m_fIsReverse)
		{
			iIndex = -2;
		}
		else
		{
			iIndex = 2;
		}
		strSql.Format("SELECT COUNT(PeriodTime) AS CarCount"
			" FROM %s, %s "
			" WHERE (%s.Pass_Time = %s.Pass_Time AND %s.Record_Id = %s.Record_Id AND %s.Road_No = %d) ",
			strViewName.GetBuffer(), strPeriodTable.GetBuffer(),
			strViewName.GetBuffer(), strPeriodTable.GetBuffer(),
			strViewName.GetBuffer(), strPeriodTable.GetBuffer(), strPeriodTable.GetBuffer(), iIndex);
		hr = pRenRecordset->Open(_variant_t(strSql), m_pConnection.GetInterfacePtr(), 
			adOpenDynamic, adLockOptimistic, adCmdText);
		if(SUCCEEDED(hr))
		{
			var = pRenRecordset->GetCollect((long)0);
			stResult.Count_2 = (int)var.lVal;
			pRenRecordset->Close();
			pRenRecordset = NULL;
		}

		if(!m_fIsRunning)return FALSE;
		hr = pRenRecordset.CreateInstance("ADODB.Recordset");
		if(m_fIsReverse)
		{
			iIndex = -3;
		}
		else
		{
			iIndex = 3;
		}
		strSql.Format("SELECT COUNT(PeriodTime) AS CarCount"
			" FROM %s, %s "
			" WHERE (%s.Pass_Time = %s.Pass_Time AND %s.Record_Id = %s.Record_Id AND %s.Road_No = %d) ",
			strViewName.GetBuffer(), strPeriodTable.GetBuffer(),
			strViewName.GetBuffer(), strPeriodTable.GetBuffer(),
			strViewName.GetBuffer(), strPeriodTable.GetBuffer(), strPeriodTable.GetBuffer(), iIndex);
		hr = pRenRecordset->Open(_variant_t(strSql), m_pConnection.GetInterfacePtr(), 
			adOpenDynamic, adLockOptimistic, adCmdText);
		if(SUCCEEDED(hr))
		{
			var = pRenRecordset->GetCollect((long)0);
			stResult.Count_3 = (int)var.lVal;
			pRenRecordset->Close();
			pRenRecordset = NULL;
		}

		if(!m_fIsRunning)return FALSE;
		hr = pRenRecordset.CreateInstance("ADODB.Recordset");
		if(m_fIsReverse)
		{
			iIndex = -4;
		}
		else
		{
			iIndex = 4;
		}
		strSql.Format("SELECT COUNT(PeriodTime) AS CarCount"
			" FROM %s, %s "
			" WHERE (%s.Pass_Time = %s.Pass_Time AND %s.Record_Id = %s.Record_Id AND %s.Road_No = %d) ",
			strViewName.GetBuffer(), strPeriodTable.GetBuffer(),
			strViewName.GetBuffer(), strPeriodTable.GetBuffer(),
			strViewName.GetBuffer(), strPeriodTable.GetBuffer(), strPeriodTable.GetBuffer(), iIndex);
		hr = pRenRecordset->Open(_variant_t(strSql), m_pConnection.GetInterfacePtr(), 
			adOpenDynamic, adLockOptimistic, adCmdText);
		if(SUCCEEDED(hr))
		{
			var = pRenRecordset->GetCollect((long)0);
			stResult.Count_4 = (int)var.lVal;
			pRenRecordset->Close();
			pRenRecordset = NULL;
		}
		strSql.Format("IF EXISTS(SELECT * FROM sysobjects WHERE name='%s' AND xtype='U') DROP TABLE %s",
			strPeriodTable.GetBuffer(), strPeriodTable.GetBuffer());
		m_pConnection->Execute((_bstr_t)strSql.GetBuffer(0), &RecordsetAffected, adCmdText);
	}
	catch (_com_error e)
	{
		m_pConnection->Close();
		m_fIsADOConnected = FALSE;
		m_staticCurrentStatue.SetWindowText("当前状态：获取车流量信息失败");
		return FALSE;
	}
	m_staticCurrentStatue.SetWindowText("当前状态：获取车流量信息成功");

	return TRUE;
}

void CHVEDemoExDlg::InitReportList()
{
	COleDateTime CurrentTime = COleDateTime::GetCurrentTime();
	m_nCurrentHour = CurrentTime.GetHour();
	m_listReport.DeleteAllItems();
	for(int index=0; index<m_nCurrentHour; index++)
	{
		if(!m_fIsRunning)return;
		m_listReport.InsertItem(index, "", 0);
		char szTmp[10];
		sprintf(szTmp, "%d", index);
		m_listReport.SetItemText(index, 0, szTmp);

		CString strStartTime;
		CString strEndTime;
	//	CString strStartTimeTM;
		int year, moth, day, hour;
		year = CurrentTime.GetYear();
		moth = CurrentTime.GetMonth();
		day = CurrentTime.GetDay();
		hour = index;
		COleDateTime cTmpStartTime, cTmpEndTime;
		cTmpStartTime.SetDateTime(year, moth, day, hour, 0, 0);
		cTmpEndTime.SetDateTime(year, moth, day, hour, 59, 59);
		if(CreateView(cTmpStartTime, cTmpEndTime) != TRUE)
		{
			m_fIsGetReportFaile = TRUE;
			return;
		}
		strStartTime.Format("%d%d%d%d-%d%d-%d%d %d%d:%d%d:%d%d",
			year/1000, (year%1000)/100, (year%100)/10, year%10,
			moth/10, moth%10, day/10, day%10, hour/10, hour%10,
			0, 0, 0, 0);
		strEndTime.Format("%d%d%d%d-%d%d-%d%d %d%d:%d%d:%d%d",
			year/1000, (year%1000)/100, (year%100)/10, year%10,
			moth/10, moth%10, day/10, day%10, hour/10, hour%10,
			5, 9, 5, 9);
		QureStr	stResult;
		memset(&stResult, 0, sizeof(QureStr));
		if(GetCarCountByTime(strStartTime, strEndTime, stResult))
		{
			char szInfo[10];
			sprintf(szInfo, "%d", stResult.Count_1);
			m_listReport.SetItemText(index, 1, szInfo);
			sprintf(szInfo, "%d", stResult.Count_2);
			m_listReport.SetItemText(index, 2, szInfo);
			sprintf(szInfo, "%d", stResult.Count_3);
			m_listReport.SetItemText(index, 3, szInfo);
			sprintf(szInfo, "%d", stResult.Count_4);
			m_listReport.SetItemText(index, 4, szInfo);
			m_fIsGetReportFaile = FALSE;
			DropView();
		}
		else
		{
			m_fIsGetReportFaile = TRUE;
			return;
		}
	//	strStartTimeTM = CurrentTime.Format("%Y-%m-%d %H:%M:%S");
	//	//while(1)
	//	//{
	//	try
	//	{
	//		//cRec.Create();
	//		//cRec.CursorType(adOpenForwardOnly);
	//		//cRec.CacheSize(100);
	//		
	//		CString strQuery;
	//		while (1)
	//		{
	//		strQuery = "IF EXISTS(SELECT * FROM sysobjects WHERE name='PeriodTable' AND xtype='U') ";
	//		strQuery += "DROP TABLE PeriodTable";
	//		m_cRec.Open(strQuery, &m_cAxConn);
	//		m_cRec.Close();
	//		Sleep(100);
	//		}

	//		if(!m_fIsRunning)return;
	//		strQuery.Format(
	//			"SELECT Record_Id, Pass_Time, '%s' AS PeriodTime, CASE WHEN Car_Speed=0 THEN 0 ELSE 10/(CONVERT(float, Car_Speed)*1000/3600) END AS PreCarCross \
	//			INTO PeriodTable FROM Hve_Data_View WHERE Pass_Time BETWEEN '%s' AND '%s' AND Location_Id=%d ORDER BY Hve_Data_View.Pass_Time",
	//			strStartTimeTM, strStartTime, strEndTime, m_rgLocation[m_nCurrentRoadID].nLocation_Id);
	//		m_cRec.Open(strQuery, &m_cAxConn);
	//		m_cRec.Close();

	//		if(!m_fIsRunning)return;
	//		strQuery.Format(
	//			"SELECT COUNT(PeriodTime) AS CarCount"
	//			" FROM Hve_Data_View, PeriodTable "
	//			" WHERE (Hve_Data_View.Pass_Time = PeriodTable.Pass_Time AND Hve_Data_View.Record_Id = PeriodTable.Record_Id AND Hve_Data_View.Road_No = 0) ");
	//		m_cRec.Open(strQuery, &m_cAxConn);
	//		int nCarCount;
	//		m_cRec.FX_Integer(FALSE, "CarCount", nCarCount);
	//		char szCarCount[20];
	//		sprintf(szCarCount, "%d", nCarCount);
	//		m_listReport.SetItemText(index, 1, szCarCount);
	//		m_cRec.Close();

	//		if(!m_fIsRunning)return;
	//		strQuery.Format(
	//			"SELECT COUNT(PeriodTime) AS CarCount"
	//			" FROM Hve_Data_View, PeriodTable "
	//			" WHERE (Hve_Data_View.Pass_Time = PeriodTable.Pass_Time AND Hve_Data_View.Record_Id = PeriodTable.Record_Id AND Hve_Data_View.Road_No = 1) ");
	//		m_cRec.Open(strQuery, &m_cAxConn);
	//		m_cRec.FX_Integer(FALSE, "CarCount", nCarCount);
	//		sprintf(szCarCount, "%d", nCarCount);
	//		m_listReport.SetItemText(index, 2, szCarCount);
	//		m_cRec.Close();

	//		if(!m_fIsRunning)return;
	//		strQuery.Format(
	//			"SELECT COUNT(PeriodTime) AS CarCount"
	//			" FROM Hve_Data_View, PeriodTable "
	//			" WHERE (Hve_Data_View.Pass_Time = PeriodTable.Pass_Time AND Hve_Data_View.Record_Id = PeriodTable.Record_Id AND Hve_Data_View.Road_No = 2) ");
	//		m_cRec.Open(strQuery, &m_cAxConn);
	//		m_cRec.FX_Integer(FALSE, "CarCount", nCarCount);
	//		sprintf(szCarCount, "%d", nCarCount);
	//		m_listReport.SetItemText(index, 3, szCarCount);
	//		m_cRec.Close();

	//		if(!m_fIsRunning)return;
	//		strQuery.Format(
	//			"SELECT COUNT(PeriodTime) AS CarCount"
	//			" FROM Hve_Data_View, PeriodTable "
	//			" WHERE (Hve_Data_View.Pass_Time = PeriodTable.Pass_Time AND Hve_Data_View.Record_Id = PeriodTable.Record_Id AND Hve_Data_View.Road_No = 3) ");
	//		m_cRec.Open(strQuery, &m_cAxConn);
	//		m_cRec.FX_Integer(FALSE, "CarCount", nCarCount);
	//		sprintf(szCarCount, "%d", nCarCount);
	//		m_listReport.SetItemText(index, 4, szCarCount);
	//		m_cRec.Close();
	//		Sleep(500);
	//	}
	//	catch(dbAx::CAxException* e)
	//	{
	//		delete e;
	//		return;
	//	}
	//	//}
	}
	m_listReport.InsertItem(m_nCurrentHour, "", 0);
	char szTmp[10];
	sprintf(szTmp, "%d", m_nCurrentHour);
	m_listReport.SetItemText(m_nCurrentHour, 0, szTmp);
	m_listReport.SetHotItem(m_nCurrentHour);
	//if(!m_fIsRunning)return;
	ShowCurrentCarNum();
}

void CHVEDemoExDlg::ShowCurrentCarNum()
{
	COleDateTime CurrentTime = COleDateTime::GetCurrentTime();
	if(m_nCurrentHour > CurrentTime.GetHour())
	{
		InitReportList();
		m_ShowCurrentHourDelay = 30;
		return;
	}
	if(CurrentTime.GetHour() == m_nCurrentHour + 1)
	{
		CString strStartTime;
		CString strEndTime;
		CString strStartTimeTM;
		int year, moth, day, hour;
		year = CurrentTime.GetYear();
		moth = CurrentTime.GetMonth();
		day = CurrentTime.GetDay();
		hour = m_nCurrentHour;

		COleDateTime cTmpStartTime, cTmpEndTime;
		cTmpStartTime.SetDateTime(year, moth, day, hour, 0, 0);
		cTmpEndTime.SetDateTime(year, moth, day, hour, 59, 59);
		CreateView(cTmpStartTime, cTmpEndTime);
		strStartTime.Format("%d%d%d%d-%d%d-%d%d %d%d:%d%d:%d%d",
			year/1000, (year%1000)/100, (year%100)/10, year%10,
			moth/10, moth%10, day/10, day%10, hour/10, hour%10,
			0, 0, 0, 0);
		strEndTime.Format("%d%d%d%d-%d%d-%d%d %d%d:%d%d:%d%d",
			year/1000, (year%1000)/100, (year%100)/10, year%10,
			moth/10, moth%10, day/10, day%10, hour/10, hour%10,
			5, 9, 5, 9);
		QureStr stResult;
		memset(&stResult, 0, sizeof(stResult));
		if(GetCarCountByTime(strStartTime, strEndTime, stResult))
		{
			char szInfo[10];
			sprintf(szInfo, "%d", stResult.Count_1);
			m_listReport.SetItemText(m_nCurrentHour, 1, szInfo);
			sprintf(szInfo, "%d", stResult.Count_2);
			m_listReport.SetItemText(m_nCurrentHour, 2, szInfo);
			sprintf(szInfo, "%d", stResult.Count_3);
			m_listReport.SetItemText(m_nCurrentHour, 3, szInfo);
			sprintf(szInfo, "%d", stResult.Count_4);
			m_listReport.SetItemText(m_nCurrentHour, 4, szInfo);
			DropView();
		}
		m_nCurrentHour++;
		m_listReport.InsertItem(m_nCurrentHour, "", 0);
		char szTmp[10];
		sprintf(szTmp, "%d", m_nCurrentHour);
		m_listReport.SetItemText(m_nCurrentHour, 0, szTmp);
		hour = m_nCurrentHour;
		cTmpStartTime.SetDateTime(year, moth, day, hour, 0, 0);
		cTmpEndTime.SetDateTime(year, moth, day, hour, 59, 59);
		CreateView(cTmpStartTime, cTmpEndTime);
		strStartTime.Format("%d%d%d%d-%d%d-%d%d %d%d:%d%d:%d%d",
			year/1000, (year%1000)/100, (year%100)/10, year%10,
			moth/10, moth%10, day/10, day%10, hour/10, hour%10,
			0, 0, 0, 0);
		strEndTime = CurrentTime.Format("%Y-%m-%d %H:%M:%S");
		memset(&stResult, 0, sizeof(QureStr));
		if(GetCarCountByTime(strStartTime, strEndTime, stResult))
		{
			char szInfo[10];
			sprintf(szInfo, "%d", stResult.Count_1);
			m_listReport.SetItemText(m_nCurrentHour, 1, szInfo);
			sprintf(szInfo, "%d", stResult.Count_2);
			m_listReport.SetItemText(m_nCurrentHour, 2, szInfo);
			sprintf(szInfo, "%d", stResult.Count_3);
			m_listReport.SetItemText(m_nCurrentHour, 3, szInfo);
			sprintf(szInfo, "%d", stResult.Count_4);
			m_listReport.SetItemText(m_nCurrentHour, 4, szInfo);
			m_listReport.SetHotItem(m_nCurrentHour);
			DropView();
		}
		m_ShowCurrentHourDelay = 30;
		return;
	}
	else if(CurrentTime.GetHour() == m_nCurrentHour)
	{
		CString strStartTime;
		CString strEndTime;
		CString strStartTimeTM;
		int year, moth, day, hour;
		year = CurrentTime.GetYear();
		moth = CurrentTime.GetMonth();
		day = CurrentTime.GetDay();
		hour = m_nCurrentHour;
		COleDateTime cTmpStartTime, cTmpEndTime;
		cTmpStartTime.SetDateTime(year, moth, day, hour, 0, 0);
		cTmpEndTime.SetDateTime(year, moth, day, hour, 59, 59);
		CreateView(cTmpStartTime, cTmpEndTime);
		strStartTime.Format("%d%d%d%d-%d%d-%d%d %d%d:%d%d:%d%d",
			year/1000, (year%1000)/100, (year%100)/10, year%10,
			moth/10, moth%10, day/10, day%10, hour/10, hour%10,
			0, 0, 0, 0);
		strEndTime = CurrentTime.Format("%Y-%m-%d %H:%M:%S");
		QureStr stResult;
		memset(&stResult, 0, sizeof(QureStr));
		if(GetCarCountByTime(strStartTime, strEndTime, stResult))
		{
			char szInfo[10];
			sprintf(szInfo, "%d", stResult.Count_1);
			m_listReport.SetItemText(m_nCurrentHour, 1, szInfo);
			sprintf(szInfo, "%d", stResult.Count_2);
			m_listReport.SetItemText(m_nCurrentHour, 2, szInfo);
			sprintf(szInfo, "%d", stResult.Count_3);
			m_listReport.SetItemText(m_nCurrentHour, 3, szInfo);
			sprintf(szInfo, "%d", stResult.Count_4);
			m_listReport.SetItemText(m_nCurrentHour, 4, szInfo);
			DropView();
		}
		m_ShowCurrentHourDelay = 30;
		return;
	}
	else
	{
		InitReportList();
		m_ShowCurrentHourDelay = 30;
	}
	//	try
	//	{
	//		CAxRecordset cRec;
	//		cRec.Create();
	//		cRec.CursorType(adOpenForwardOnly);
	//		cRec.CacheSize(100);
	//		CString strQuery;
	//		strQuery = "IF EXISTS(SELECT * FROM sysobjects WHERE name='PeriodTable' AND xtype='U') ";
	//		strQuery += "DROP TABLE PeriodTable";
	//		cRec.Open(strQuery, &m_cAxConn);
	//		cRec.Close();

	//		strQuery.Format(
	//			"SELECT Record_Id, Pass_Time, '%s' AS PeriodTime, CASE WHEN Car_Speed=0 THEN 0 ELSE 10/(CONVERT(float, Car_Speed)*1000/3600) END AS PreCarCross \
	//			INTO PeriodTable FROM Hve_Data_View WHERE Pass_Time BETWEEN '%s' AND '%s' AND Location_Id=%d ORDER BY Hve_Data_View.Pass_Time",
	//			strStartTimeTM, strStartTime, strEndTime, m_rgLocation[m_nCurrentRoadID].nLocation_Id);
	//		cRec.Open(strQuery, &m_cAxConn);
	//		cRec.Close();

	//		strQuery.Format(
	//			"SELECT COUNT(PeriodTime) AS CarCount"
	//			" FROM Hve_Data_View, PeriodTable "
	//			" WHERE (Hve_Data_View.Pass_Time = PeriodTable.Pass_Time AND Hve_Data_View.Record_Id = PeriodTable.Record_Id AND Hve_Data_View.Road_No = 0) ");
	//		cRec.Open(strQuery, &m_cAxConn);
	//		int nCarCount;
	//		cRec.FX_Integer(FALSE, "CarCount", nCarCount);
	//		char szCarCount[20];
	//		sprintf(szCarCount, "%d", nCarCount);
	//		m_listReport.SetItemText(m_nCurrentHour, 1, szCarCount);
	//		cRec.Close();

	//		strQuery.Format(
	//			"SELECT COUNT(PeriodTime) AS CarCount"
	//			" FROM Hve_Data_View, PeriodTable "
	//			" WHERE (Hve_Data_View.Pass_Time = PeriodTable.Pass_Time AND Hve_Data_View.Record_Id = PeriodTable.Record_Id AND Hve_Data_View.Road_No = 1) ");
	//		cRec.Open(strQuery, &m_cAxConn);
	//		cRec.FX_Integer(FALSE, "CarCount", nCarCount);
	//		sprintf(szCarCount, "%d", nCarCount);
	//		m_listReport.SetItemText(m_nCurrentHour, 2, szCarCount);
	//		cRec.Close();

	//		strQuery.Format(
	//			"SELECT COUNT(PeriodTime) AS CarCount"
	//			" FROM Hve_Data_View, PeriodTable "
	//			" WHERE (Hve_Data_View.Pass_Time = PeriodTable.Pass_Time AND Hve_Data_View.Record_Id = PeriodTable.Record_Id AND Hve_Data_View.Road_No = 2) ");
	//		cRec.Open(strQuery, &m_cAxConn);
	//		cRec.FX_Integer(FALSE, "CarCount", nCarCount);
	//		sprintf(szCarCount, "%d", nCarCount);
	//		m_listReport.SetItemText(m_nCurrentHour, 3, szCarCount);
	//		cRec.Close();

	//		strQuery.Format(
	//			"SELECT COUNT(PeriodTime) AS CarCount"
	//			" FROM Hve_Data_View, PeriodTable "
	//			" WHERE (Hve_Data_View.Pass_Time = PeriodTable.Pass_Time AND Hve_Data_View.Record_Id = PeriodTable.Record_Id AND Hve_Data_View.Road_No = 3) ");
	//		cRec.Open(strQuery, &m_cAxConn);
	//		cRec.FX_Integer(FALSE, "CarCount", nCarCount);
	//		sprintf(szCarCount, "%d", nCarCount);
	//		m_listReport.SetItemText(m_nCurrentHour, 4, szCarCount);
	//		cRec.Close();
	//	}
	//	catch(dbAx::CAxException* e)
	//	{
	//		delete e;
	//		m_ShowCurrentHourDelay = 12;
	//		return;
	//	}

	//	m_nCurrentHour++;
	//	m_listReport.InsertItem(m_nCurrentHour, "", 0);
	//	char szTmp[10];
	//	sprintf(szTmp, "%d", m_nCurrentHour);
	//	m_listReport.SetItemText(m_nCurrentHour, 0, szTmp);
	//	hour = m_nCurrentHour;
	//	strStartTime.Format("%d%d%d%d-%d%d-%d%d %d%d:%d%d:%d%d",
	//		year/1000, (year%1000)/100, (year%100)/10, year%10,
	//		moth/10, moth%10, day/10, day%10, hour/10, hour%10,
	//		0, 0, 0, 0);
	//	strEndTime = CurrentTime.Format("%Y-%m-%d %H:%M:%S");
 //		strStartTimeTM = CurrentTime.Format("%Y-%m-%d %H:%M:%S");
	//	try
	//	{
	//		CAxRecordset cRec;
	//		cRec.Create();
	//		cRec.CursorType(adOpenForwardOnly);
	//		cRec.CacheSize(100);
	//		CString strQuery;
	//		strQuery = "IF EXISTS(SELECT * FROM sysobjects WHERE name='PeriodTable' AND xtype='U') ";
	//		strQuery += "DROP TABLE PeriodTable";
	//		cRec.Open(strQuery, &m_cAxConn);
	//		cRec.Close();

	//		strQuery.Format(
	//			"SELECT Record_Id, Pass_Time, '%s' AS PeriodTime, CASE WHEN Car_Speed=0 THEN 0 ELSE 10/(CONVERT(float, Car_Speed)*1000/3600) END AS PreCarCross \
	//			INTO PeriodTable FROM Hve_Data_View WHERE Pass_Time BETWEEN '%s' AND '%s' AND Location_Id=%d ORDER BY Hve_Data_View.Pass_Time",
	//			strStartTimeTM, strStartTime, strEndTime, m_rgLocation[m_nCurrentRoadID].nLocation_Id);
	//		cRec.Open(strQuery, &m_cAxConn);
	//		cRec.Close();

	//		strQuery.Format(
	//			"SELECT COUNT(PeriodTime) AS CarCount"
	//			" FROM Hve_Data_View, PeriodTable "
	//			" WHERE (Hve_Data_View.Pass_Time = PeriodTable.Pass_Time AND Hve_Data_View.Record_Id = PeriodTable.Record_Id AND Hve_Data_View.Road_No = 0) ");
	//		cRec.Open(strQuery, &m_cAxConn);
	//		int nCarCount;
	//		cRec.FX_Integer(FALSE, "CarCount", nCarCount);
	//		char szCarCount[20];
	//		sprintf(szCarCount, "%d", nCarCount);
	//		m_listReport.SetItemText(m_nCurrentHour, 1, szCarCount);
	//		cRec.Close();

	//		strQuery.Format(
	//			"SELECT COUNT(PeriodTime) AS CarCount"
	//			" FROM Hve_Data_View, PeriodTable "
	//			" WHERE (Hve_Data_View.Pass_Time = PeriodTable.Pass_Time AND Hve_Data_View.Record_Id = PeriodTable.Record_Id AND Hve_Data_View.Road_No = 1) ");
	//		cRec.Open(strQuery, &m_cAxConn);
	//		cRec.FX_Integer(FALSE, "CarCount", nCarCount);
	//		sprintf(szCarCount, "%d", nCarCount);
	//		m_listReport.SetItemText(m_nCurrentHour, 2, szCarCount);
	//		cRec.Close();

	//		strQuery.Format(
	//			"SELECT COUNT(PeriodTime) AS CarCount"
	//			" FROM Hve_Data_View, PeriodTable "
	//			" WHERE (Hve_Data_View.Pass_Time = PeriodTable.Pass_Time AND Hve_Data_View.Record_Id = PeriodTable.Record_Id AND Hve_Data_View.Road_No = 2) ");
	//		cRec.Open(strQuery, &m_cAxConn);
	//		cRec.FX_Integer(FALSE, "CarCount", nCarCount);
	//		sprintf(szCarCount, "%d", nCarCount);
	//		m_listReport.SetItemText(m_nCurrentHour, 3, szCarCount);
	//		cRec.Close();

	//		strQuery.Format(
	//			"SELECT COUNT(PeriodTime) AS CarCount"
	//			" FROM Hve_Data_View, PeriodTable "
	//			" WHERE (Hve_Data_View.Pass_Time = PeriodTable.Pass_Time AND Hve_Data_View.Record_Id = PeriodTable.Record_Id AND Hve_Data_View.Road_No = 3) ");
	//		cRec.Open(strQuery, &m_cAxConn);
	//		cRec.FX_Integer(FALSE, "CarCount", nCarCount);
	//		sprintf(szCarCount, "%d", nCarCount);
	//		m_listReport.SetItemText(m_nCurrentHour, 4, szCarCount);
	//		cRec.Close();
	//	}
	//	catch(dbAx::CAxException* e)
	//	{
	//		m_ShowCurrentHourDelay = 12;
	//		delete e;
	//		return;
	//	}
	//	m_ShowCurrentHourDelay = 12;
	//}
	//else if(CurrentTime.GetHour() == m_nCurrentHour)
	//{
	//	CString strStartTime;
	//	CString strEndTime;
	//	CString strStartTimeTM;
	//	int year, moth, day, hour;
	//	year = CurrentTime.GetYear();
	//	moth = CurrentTime.GetMonth();
	//	day = CurrentTime.GetDay();
	//	hour = m_nCurrentHour;
	//	strStartTime.Format("%d%d%d%d-%d%d-%d%d %d%d:%d%d:%d%d",
	//		year/1000, (year%1000)/100, (year%100)/10, year%10,
	//		moth/10, moth%10, day/10, day%10, hour/10, hour%10,
	//		0, 0, 0, 0);
	//	strEndTime = CurrentTime.Format("%Y-%m-%d %H:%M:%S");
	//	strStartTimeTM = CurrentTime.Format("%Y-%m-%d %H:%M:%S");
	//	try
	//	{
	//		CAxRecordset cRec;
	//		cRec.Create();
	//		cRec.CursorType(adOpenForwardOnly);
	//		cRec.CacheSize(100);
	//		CString strQuery;
	//		strQuery = "IF EXISTS(SELECT * FROM sysobjects WHERE name='PeriodTable' AND xtype='U') ";
	//		strQuery += "DROP TABLE PeriodTable";
	//		cRec.Open(strQuery, &m_cAxConn);
	//		cRec.Close();

	//		strQuery.Format(
	//			"SELECT Record_Id, Pass_Time, '%s' AS PeriodTime, CASE WHEN Car_Speed=0 THEN 0 ELSE 10/(CONVERT(float, Car_Speed)*1000/3600) END AS PreCarCross \
	//			INTO PeriodTable FROM Hve_Data_View WHERE Pass_Time BETWEEN '%s' AND '%s' AND Location_Id=%d ORDER BY Hve_Data_View.Pass_Time",
	//			strStartTimeTM, strStartTime, strEndTime, m_rgLocation[m_nCurrentRoadID].nLocation_Id);
	//		cRec.Open(strQuery, &m_cAxConn);
	//		cRec.Close();

	//		strQuery.Format(
	//			"SELECT COUNT(PeriodTime) AS CarCount"
	//			" FROM Hve_Data_View, PeriodTable "
	//			" WHERE (Hve_Data_View.Pass_Time = PeriodTable.Pass_Time AND Hve_Data_View.Record_Id = PeriodTable.Record_Id AND Hve_Data_View.Road_No = 0) ");
	//		cRec.Open(strQuery, &m_cAxConn);
	//		int nCarCount;
	//		cRec.FX_Integer(FALSE, "CarCount", nCarCount);
	//		char szCarCount[20];
	//		sprintf(szCarCount, "%d", nCarCount);
	//		m_listReport.SetItemText(m_nCurrentHour, 1, szCarCount);
	//		cRec.Close();

	//		strQuery.Format(
	//			"SELECT COUNT(PeriodTime) AS CarCount"
	//			" FROM Hve_Data_View, PeriodTable "
	//			" WHERE (Hve_Data_View.Pass_Time = PeriodTable.Pass_Time AND Hve_Data_View.Record_Id = PeriodTable.Record_Id AND Hve_Data_View.Road_No = 1) ");
	//		cRec.Open(strQuery, &m_cAxConn);
	//		cRec.FX_Integer(FALSE, "CarCount", nCarCount);
	//		sprintf(szCarCount, "%d", nCarCount);
	//		m_listReport.SetItemText(m_nCurrentHour, 2, szCarCount);
	//		cRec.Close();

	//		strQuery.Format(
	//			"SELECT COUNT(PeriodTime) AS CarCount"
	//			" FROM Hve_Data_View, PeriodTable "
	//			" WHERE (Hve_Data_View.Pass_Time = PeriodTable.Pass_Time AND Hve_Data_View.Record_Id = PeriodTable.Record_Id AND Hve_Data_View.Road_No = 2) ");
	//		cRec.Open(strQuery, &m_cAxConn);
	//		cRec.FX_Integer(FALSE, "CarCount", nCarCount);
	//		sprintf(szCarCount, "%d", nCarCount);
	//		m_listReport.SetItemText(m_nCurrentHour, 3, szCarCount);
	//		cRec.Close();

	//		strQuery.Format(
	//			"SELECT COUNT(PeriodTime) AS CarCount"
	//			" FROM Hve_Data_View, PeriodTable "
	//			" WHERE (Hve_Data_View.Pass_Time = PeriodTable.Pass_Time AND Hve_Data_View.Record_Id = PeriodTable.Record_Id AND Hve_Data_View.Road_No = 3) ");
	//		cRec.Open(strQuery, &m_cAxConn);
	//		cRec.FX_Integer(FALSE, "CarCount", nCarCount);
	//		sprintf(szCarCount, "%d", nCarCount);
	//		m_listReport.SetItemText(m_nCurrentHour, 4, szCarCount);
	//		cRec.Close();
	//	}
	//	catch(dbAx::CAxException* e)
	//	{
	//		delete e;
	//		m_ShowCurrentHourDelay = 12;
	//		return;
	//	}
	//	m_ShowCurrentHourDelay = 12;
	//}
	//else
	//{
	//	InitReportList();
	//	m_ShowCurrentHourDelay = 12;
	//	return;
	//}
}

BOOL CHVEDemoExDlg::DisConnect()
{
	//if(!m_fIsConnect) return TRUE;
	//if(m_cAxConn._IsOpen())
	//{
	//	m_cAxConn.Close();
	//}
	//m_fIsConnect = FALSE;

	if(!m_fIsADOConnected)
	{
		m_staticCurrentStatue.SetWindowText("当前状态：已断开数据库连接");
		return TRUE;
	}
	m_staticCurrentStatue.SetWindowText("当前状态：正在断开与数据库连接");
	if(m_pConnection->GetState() != adStateClosed)
	{
		m_pConnection->Cancel();
		m_pConnection->Close();
	}
	m_fIsADOConnected = FALSE;
	m_staticCurrentStatue.SetWindowText("当前状态：已断开数据库连接");
	return TRUE;
}

LRESULT CHVEDemoExDlg::OnQueryQuit(WPARAM wParam, LPARAM lParam)
{
	if(m_QueryDlg != NULL)
	{
		delete m_QueryDlg;
		m_QueryDlg = NULL;
	}
	m_fIsShowQuery = FALSE;
	return 0;
}

LRESULT CHVEDemoExDlg::OnSetOk(WPARAM wParam, LPARAM lParam)
{
	if(m_SetDlg != NULL)
	{
		delete m_SetDlg;
		m_SetDlg = NULL;
	}
	m_fIsShowSetDlg = FALSE;
	BOOL fIsNew = FALSE;
	if(m_strSourcePath != m_rgSettingInfo.dwIP)
	{
		m_strSourcePath = m_rgSettingInfo.dwIP;
		fIsNew = TRUE;
	}
	if(m_strDataBaseName != m_rgSettingInfo.strDataBaseName)
	{
		m_strDataBaseName = m_rgSettingInfo.strDataBaseName;
		fIsNew = TRUE;
	}
	if(m_strUserName != m_rgSettingInfo.strUserName)
	{
		m_strUserName = m_rgSettingInfo.strUserName;
		fIsNew = TRUE;
	}
	if(m_strPassWord != m_rgSettingInfo.strPassWord)
	{
		m_strPassWord = m_rgSettingInfo.strPassWord;
		fIsNew = TRUE;
	}
	
	if(fIsNew)
	{
		SaveIniInfo();
		Stop();
		DisConnect();

		m_fIsNewInfo = TRUE;
		CString strIP;
		CString strConnect;
		strIP.Format("%d.%d.%d.%d", (m_strSourcePath>>24)&0xFF, (m_strSourcePath>>16)&0xFF, 
			(m_strSourcePath>>8)&0xFF, (m_strSourcePath)&0xFF);
		strConnect.Format("Provider=SQLOLEDB.1;Password=%s;User ID=%s;Initial Catalog=%s;Data Source=%s",
			m_strPassWord, m_strUserName, m_strDataBaseName, strIP);
		DoConnectDataBase(strConnect);
		m_fIsNewInfo = TRUE;
		if(m_fIsADOConnected)
		{
			m_staticCurrentStatue.SetWindowText("当前状态：已连接到数据库");
			Starting();
		}
		else
		{
			m_SetDlg = new CSystemSettingDlg(this);
			m_SetDlg->Create(IDD_DIALOGSYSSET, this);
			m_SetDlg->ShowWindow(SW_SHOW);
			m_SetDlg->UpdateWindow();
			m_fIsShowSetDlg = TRUE;
		}
	}
	else if(!m_fIsRunning)
	{
		m_fIsNewInfo = TRUE;
		CString strIP;
		CString strConnect;
		strIP.Format("%d.%d.%d.%d", (m_strSourcePath>>24)&0xFF, (m_strSourcePath>>16)&0xFF, 
			(m_strSourcePath>>8)&0xFF, (m_strSourcePath)&0xFF);
		strConnect.Format("Provider=SQLOLEDB.1;Password=%s;User ID=%s;Initial Catalog=%s;Data Source=%s",
			m_strPassWord, m_strUserName, m_strDataBaseName, strIP);
		DoConnectDataBase(strConnect);
		m_fIsNewInfo = TRUE;
		if(m_fIsADOConnected)
		{
			m_staticCurrentStatue.SetWindowText("当前状态：已连接到数据库");
			Starting();
		}
		else
		{
			m_SetDlg = new CSystemSettingDlg(this);
			m_SetDlg->Create(IDD_DIALOGSYSSET, this);
			m_SetDlg->ShowWindow(SW_SHOW);
			m_SetDlg->UpdateWindow();
			m_fIsShowSetDlg = TRUE;
		}
	}
	else
	{
		MessageBox("输入的参数与当前参数相同无需修改");
	}
	return 0;
}

LRESULT CHVEDemoExDlg::OnSetCancel(WPARAM wParam, LPARAM lParam)
{
	if(m_SetDlg != NULL)
	{
		delete m_SetDlg;
		m_SetDlg = NULL;
	}
	m_fIsShowSetDlg = FALSE;
	return 0;
}

DWORD CHVEDemoExDlg::IPStr2DWord(char* szIP)
{
	DWORD ret;
	if(szIP == NULL || strlen(szIP) < 8) return 0;
	int pTmp[4];
	BYTE* pDst = (BYTE*)&ret;
	sscanf(szIP, "%d.%d.%d.%d", &pTmp[0], &pTmp[1], &pTmp[2], &pTmp[3]);
	pDst[0] = (BYTE)pTmp[3];
	pDst[1] = (BYTE)pTmp[2];
	pDst[2] = (BYTE)pTmp[1];
	pDst[3] = (BYTE)pTmp[0];

	return ret;
}

void CHVEDemoExDlg::GetIniFileInfo()
{
	char szAppPath[MAX_PATH] = {0};
	GetModuleFileName(NULL, szAppPath, MAX_PATH);
	PathRemoveFileSpec(szAppPath);
	m_strIniFilePath.Format("%s\\Config.ini", szAppPath);

	char szTmp[256] = {0};
	GetPrivateProfileStringA("DBInfo", "ServerIP", "", szTmp, 256, m_strIniFilePath.GetBuffer(0));
	CString m_CurrentIP;
	m_CurrentIP.Format("%d.%d.%d.%d", 
		(m_strSourcePath>>24)&0xFF, (m_strSourcePath>>16)&0xFF, (m_strSourcePath>>8)&0xFF, (m_strSourcePath)&0xFF);
	if(strcmp(szTmp, "") == 0)
	{
		WritePrivateProfileStringA("DBInfo", "ServerIP", m_CurrentIP.GetBuffer(0), m_strIniFilePath.GetBuffer(0));
	}
	else
	{
		m_strSourcePath = IPStr2DWord(szTmp);
	}

	GetPrivateProfileStringA("DBInfo", "DataBase", "", szTmp, 256, m_strIniFilePath.GetBuffer());
	if(strcmp(szTmp, "") == 0)
	{
		WritePrivateProfileStringA("DBInfo", "DataBase", m_strDataBaseName.GetBuffer(), m_strIniFilePath.GetBuffer());
	}
	else
	{
		m_strDataBaseName = szTmp;
	}

	GetPrivateProfileStringA("DBInfo", "UserName", "", szTmp, 256, m_strIniFilePath.GetBuffer());
	if(strcmp(szTmp, "") == 0)
	{
		WritePrivateProfileStringA("DBInfo", "UserName", m_strUserName.GetBuffer(), m_strIniFilePath.GetBuffer());
	}
	else
	{
		m_strUserName = szTmp;
	}

	GetPrivateProfileStringA("DBInfo", "Password", "", szTmp, 256, m_strIniFilePath.GetBuffer());
	if(strcmp(szTmp, "") == 0)
	{
		WritePrivateProfileStringA("DBInfo", "Password", m_strPassWord.GetBuffer(), m_strIniFilePath.GetBuffer());
	}
	else
	{
		m_strPassWord = szTmp;
	}
}

void CHVEDemoExDlg::SaveIniInfo()
{
	CString m_CurrentIP;
	m_CurrentIP.Format("%d.%d.%d.%d", 
		(m_strSourcePath>>24)&0xFF, (m_strSourcePath>>16)&0xFF, (m_strSourcePath>>8)&0xFF, (m_strSourcePath)&0xFF);
	WritePrivateProfileStringA("DBInfo", "ServerIP", m_CurrentIP.GetBuffer(0), m_strIniFilePath.GetBuffer(0));
	WritePrivateProfileStringA("DBInfo", "DataBase", m_strDataBaseName.GetBuffer(), m_strIniFilePath.GetBuffer());
	WritePrivateProfileStringA("DBInfo", "UserName", m_strUserName.GetBuffer(), m_strIniFilePath.GetBuffer());
	WritePrivateProfileStringA("DBInfo", "Password", m_strPassWord.GetBuffer(), m_strIniFilePath.GetBuffer());
}

void CHVEDemoExDlg::OnCbnSelchangeCombo1()
{
	DWORD CurSel = m_ComBoxRoad.GetCurSel();
	if(CurSel == m_nCurrentRoadID || CurSel < 0)return;
	m_nCurrentRoadID = CurSel;
	ShowRoadInfo();
	m_fIsNewInfo = TRUE;
}

BOOL CHVEDemoExDlg::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN)
	return TRUE;

	return CDialog::PreTranslateMessage(pMsg);
}


void CHVEDemoExDlg::OnBnClickedCheck1()
{
	CButton* pBtn =(CButton*)GetDlgItem(IDC_CHECK1);
	pBtn->EnableWindow(FALSE);
	if(pBtn->GetCheck() == 1)
	{
		m_fIsReverse = TRUE;
	}
	else
	{
		m_fIsReverse = FALSE;
	}
	m_fIsNewInfo = TRUE;
	pBtn->EnableWindow(TRUE);
}
