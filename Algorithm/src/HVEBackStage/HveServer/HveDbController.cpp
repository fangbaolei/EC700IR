#include "stdafx.h"
#include "HveDbController.h"

extern CString g_rgstrObjType[MAX_OBJ_TYPE];
extern void OutPutMessage(LPSTR szMsg);
extern CString m_strLogPath;

CHveDbController::CHveDbController(CList<RESULT_ELEMENT>* pListResult, SINGLE_HVE_ELEMENT* pSingleHveElement, ISemaphore* pSemaphore, CCriticalSection* pcsResult)
{
	m_pListResult = pListResult;
	m_SingleHveElement = pSingleHveElement;
	m_pSemaphore = pSemaphore;
	m_pcsResult = pcsResult;
	m_fComIsInited = FALSE;
	m_strServerIP = "127.0.0.1";
	m_strDataBase = "HVE_DATA";
	m_strUserName = "sa";
	m_strPassword = "12345678";
	char szAppPath[MAX_PATH] = {0};
	GetModuleFileName(NULL, szAppPath, MAX_PATH);
	PathRemoveFileSpec(szAppPath);
	strcat(szAppPath, "\\Config.ini");
	char szTmp[256] = {0};
	GetPrivateProfileStringA("DBInfo", "ServerIP", "", szTmp, 256, szAppPath);
	if(strcmp(szTmp, "") == 0)
	{
		WritePrivateProfileStringA("DBInfo", "ServerIP", m_strServerIP.GetBuffer(), szAppPath);
	}
	else
	{
		m_strServerIP = szTmp;
	}

	GetPrivateProfileStringA("DBInfo", "DataBase", "", szTmp, 256, szAppPath);
	if(strcmp(szTmp, "") == 0)
	{
		WritePrivateProfileStringA("DBInfo", "DataBase", m_strDataBase.GetBuffer(), szAppPath);
	}
	else
	{
		m_strDataBase = szTmp;
	}

	GetPrivateProfileStringA("DBInfo", "UserName", "", szTmp, 256, szAppPath);
	if(strcmp(szTmp, "") == 0)
	{
		WritePrivateProfileStringA("DBInfo", "UserName", m_strUserName.GetBuffer(), szAppPath);
	}
	else
	{
		m_strUserName = szTmp;
	}

	GetPrivateProfileStringA("DBInfo", "Password", "", szTmp, 256, szAppPath);
	if(strcmp(szTmp, "") == 0)
	{
		WritePrivateProfileStringA("DBInfo", "Password", m_strPassword.GetBuffer(), szAppPath);
	}
	else
	{
		m_strPassword = szTmp;
	}

	m_fIsConnecting = FALSE;
	m_strColumns.Format("Location_Id, Pass_Time, Plate_No, Road_No, Road_Name, Road_Direct, Car_Speed, Obj_Id, Time_Headway, Over_Line, Cross_Line, Emergency_Stop, Reverse_Run, Speeding, Special_Result, big_image_1_path, big_image_2_path, big_image_3_path, big_image_4_path, big_image_5_path, small_image_path, bin_image_path, video_path");
	m_fExit = FALSE;
	DWORD dwThreadId;
	m_hSaveThread = CreateThread(NULL, 0, SaveProcProxy, this, 0, &dwThreadId);

}

CHveDbController::~CHveDbController(void)
{
	m_fExit = TRUE;
	if(m_hSaveThread)
	{
		int iWatTimes = 0;
		int MAX_WAIT_TIME = 10;
		while(WaitForSingleObject(m_hSaveThread, 500) == WAIT_TIMEOUT && iWatTimes < MAX_WAIT_TIME)
		{
			iWatTimes++;
		}
		if(iWatTimes >= MAX_WAIT_TIME)
		{
			TerminateThread(m_hSaveThread, 0);
			OutPutMessage("强行关闭数据入库线程...");
		}
		CloseHandle(m_hSaveThread);
	}
	try
	{
		if(m_pConnection->State)
		{
			m_pConnection->Close();
		}
		m_pConnection = NULL;
	}
	catch(_com_error e)
	{

	}
	::CoUninitialize();
}

void CHveDbController::SaveLog(char* pszLog)
{
	if(pszLog == NULL)
	{
		return;
	}

	FILE* pFile = fopen(m_strLogPath.GetBuffer(), "a+");
	if(pFile)
	{
		CString strInfo;
		CTime cTime = CTime::GetCurrentTime();
		strInfo.Format("<%s>    %s\n",
			cTime.Format("%Y-%m-%d %H:%M:%S"), pszLog);
		fprintf(pFile, strInfo.GetBuffer());
		fclose(pFile);
	}
}

HRESULT CHveDbController::ConnectDB()
{
	if(!m_fComIsInited)
	{
		::CoInitialize(NULL);
		m_fComIsInited = TRUE;
	}

	HRESULT hr = S_OK;
	m_fIsConnecting = TRUE;
	try
	{
		hr = m_pConnection.CreateInstance("ADODB.Connection");
		if(SUCCEEDED(hr))
		{
			m_pConnection->CommandTimeout = 15;
			m_pConnection->CursorLocation = adUseServer;
			m_pConnection->IsolationLevel = adXactReadCommitted;
			CString strConnectInfo;
			strConnectInfo.Format("Provider=SQLOLEDB;Server=%s;DATABASE=%s;UID=%s;PWD=%s",
				m_strServerIP, m_strDataBase, m_strUserName, m_strPassword);
			hr = m_pConnection->Open((_bstr_t)(strConnectInfo.GetBuffer()), "", "", adModeUnknown);

			SaveLog("数据库连接成功");
		}
	}
	catch (_com_error e)
	{
		m_fIsConnecting = FALSE;
		CString strErrorMessage;
		strErrorMessage.Format("数据库连接失败，错误信息：%s 错误码：%d\n", e.ErrorMessage(), GetLastError());
		SaveLog(strErrorMessage.GetBuffer());
		SaveLog(e.Description());
		OutPutMessage("数据库连接失败...");
	}
	return hr;
}


HRESULT CHveDbController::SaveDBElement(RESULT_ELEMENT* pElement)
{
	HRESULT hr = S_OK;
	if(!m_fIsConnecting)
	{
		SaveLog("数据库连接已经断开，尝试重新连接...");
		hr = ConnectDB();
	}

	if(!m_fIsConnecting)
	{
		char szOutPutInfo[100];
		sprintf(szOutPutInfo, "连接数据库失败，保存%s结果失败...错误码:%d", pElement->szPlateNo, GetLastError());
		SaveLog(szOutPutInfo);
		OutPutMessage(szOutPutInfo);
		return E_FAIL;
	}
	_variant_t RecordsAffected;

	CTime cTime(pElement->dw64TimeMs / 1000);
	CString strTime = cTime.Format("%Y-%m-%d %H:%M:%S");
	CString strCurTable = cTime.Format("Hve_Data_%Y%m%d");

	//int iMinute = cTime.GetMinute(); 
	//CString strTmp = cTime.Format("Hve_Data_%Y%m%d%H");
	//strCurTable.Format("%s_%d",strTmp.GetBuffer(), iMinute/10); // 10分钟一张表
	//strTmp.ReleaseBuffer();

	if(m_strLastTable != strCurTable)
	{
		char szCmdText[1024] = {0};

		//try
		//{
		//	sprintf(szCmdText, "Create TABLE %s(Record_Id bigint primary key IDENTITY   (1,   1)   NOT   NULL,\
		//					   Location_Id int NOT NULL,Pass_Time datetime NOT NULL, Plate_No char(20), Road_No int,\
		//					   Car_Speed int, Obj_Id int, Time_Headway int, Over_Line int, Cross_Line int,\
		//					   Emergency_Stop int, Reverse_Run bit, Special_Result bit, big_image_1_path char(256),\
		//					   big_image_2_path char(256), big_image_3_path char(256), big_image_4_path char(256),\
		//					   big_image_5_path char(256), small_image_path char(256), bin_image_path char(256),\
		//					   video_path char(256))", strCurTable.GetBuffer());
		//	m_pConnection->Execute((_bstr_t)szCmdText, &RecordsAffected, adCmdText);
		//	sprintf(szCmdText, "CREATE INDEX [Pass_Time] ON [dbo].[%s] ([Pass_Time])", strCurTable.GetBuffer());
		//	m_pConnection->Execute((_bstr_t)szCmdText, &RecordsAffected, adCmdText);
		//	sprintf(szCmdText, "CREATE INDEX [Obj_Id] ON [dbo].[%s] ([Obj_Id])", strCurTable.GetBuffer());
		//	m_pConnection->Execute((_bstr_t)szCmdText, &RecordsAffected, adCmdText);
		//	sprintf(szCmdText, "CREATE INDEX [Special_Result] ON [dbo].[%s] ([Special_Result])", strCurTable.GetBuffer());
		//	m_pConnection->Execute((_bstr_t)szCmdText, &RecordsAffected, adCmdText);
		//}

		//表存在则不创建
		try
		{
			sprintf(szCmdText, "if not exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[%s]') and OBJECTPROPERTY(id, N'IsUserTable') = 1) \
							   Create TABLE %s(Record_Id bigint primary key IDENTITY   (1,   1)   NOT   NULL,\
							   Location_Id int NOT NULL,Pass_Time datetime NOT NULL, Plate_No char(20), Road_No int, Road_Name char(25),Road_Direct char(10),\
							   Car_Speed int, Obj_Id int, Time_Headway int, Over_Line int, Cross_Line int,\
							   Emergency_Stop int, Reverse_Run bit, Speeding bit, Special_Result bit, big_image_1_path varchar(256),\
							   big_image_2_path varchar(256), big_image_3_path varchar(256), big_image_4_path varchar(256),\
							   big_image_5_path varchar(256), small_image_path varchar(256), bin_image_path varchar(256),\
							   video_path varchar(256))",strCurTable.GetBuffer(),strCurTable.GetBuffer());
			m_pConnection->Execute((_bstr_t)szCmdText, &RecordsAffected, adCmdText);
			sprintf(szCmdText, "if not exists (select * from sysindexes where name='Pass_Time')  CREATE INDEX [Pass_Time] ON [dbo].[%s] ([Pass_Time])", strCurTable.GetBuffer());
			m_pConnection->Execute((_bstr_t)szCmdText, &RecordsAffected, adCmdText);
			sprintf(szCmdText, "if not exists (select * from sysindexes where name='Obj_Id') CREATE INDEX [Obj_Id] ON [dbo].[%s] ([Obj_Id])", strCurTable.GetBuffer());
			m_pConnection->Execute((_bstr_t)szCmdText, &RecordsAffected, adCmdText);
			sprintf(szCmdText, "if not exists (select * from sysindexes where name='Special_Result') CREATE INDEX [Special_Result] ON [dbo].[%s] ([Special_Result])", strCurTable.GetBuffer());
			m_pConnection->Execute((_bstr_t)szCmdText, &RecordsAffected, adCmdText);
		}
		catch (_com_error e)
		{
			m_fIsConnecting = FALSE;
			CString strErrorMessage;
			strErrorMessage.Format("创建表失败!错误信息:%s  错误码:%d", e.ErrorMessage(), GetLastError());
			SaveLog(strErrorMessage.GetBuffer());
			SaveLog(e.Description());
			OutPutMessage("创建数据表失败!");
		}

		///////视图创建 由查询软件创建
		//try
		//{
		//	char szCmdText[1024] = {0};
		//	sprintf(szCmdText, "IF EXISTS (SELECT TABLE_NAME FROM INFORMATION_SCHEMA.VIEWS WHERE TABLE_NAME = N'Hve_Data_View') DROP VIEW Hve_Data_View");
		//	m_pConnection->Execute((_bstr_t)szCmdText, &RecordsAffected, adCmdText);

		//}
		//catch (_com_error e)
		//{
		//	CString strErrorMessage;
		//	strErrorMessage.Format("删除视图失败! 错误信息:%s 错误码:%d", e.ErrorMessage(), GetLastError());
		//	SaveLog(strErrorMessage.GetBuffer());
		//	SaveLog(e.Description());
		//	OutPutMessage("删除视图失败...");
		//}
		
		//bool fIsCreateViewFault = true;
		//int iRetry = 0;
		//while(fIsCreateViewFault)
		//{
		//	if(iRetry++ == 5)
		//	{
		//		exit(1);  //reboot
		//	}

		//	try
		//	{
		//		_RecordsetPtr pPtr;
		//		CString strName;
		//		if(FAILED(pPtr.CreateInstance("ADODB.Recordset")))
		//		{
		//			OutPutMessage("创建ADO数据集失败...");
		//			return E_FAIL;
		//		}

		//		CString strConnectInfo;
		//		strConnectInfo.Format("Driver=SQL Server;Server=%s;DATABASE=%s;UID=%s;PWD=%s",
		//			m_strServerIP, m_strDataBase, m_strUserName, m_strPassword);
		//		CString strCommand = "SELECT name FROM sysobjects WHERE (name LIKE 'Hve_Data_%')";;
		//		if(FAILED(pPtr->Open(_variant_t(strCommand), _variant_t(strConnectInfo), adOpenStatic, adLockOptimistic, adCmdText)))
		//		{
		//			OutPutMessage("获取数据表名字失败...");
		//			pPtr.Release();
		//			return E_FAIL;
		//		}
		//		while(!pPtr->GetadoEOF())
		//		{
		//			CString strTmp = (char*)_bstr_t(pPtr->GetCollect("name"));
		//			strTmp.TrimRight();
		//			if(strName == "")
		//			{
		//				strName.Format("(SELECT * FROM %s)", strTmp);
		//			}
		//			else
		//			{
		//				strName.AppendFormat("union (SELECT * FROM %s)", strTmp);
		//			}
		//			pPtr->MoveNext();
		//		}
		//		pPtr->Close();
		//		pPtr.Release();

		//		char szCmdText[1024] = {0};
		//		sprintf(szCmdText,  "CREATE VIEW Hve_Data_View AS %s", strName);
		//		m_pConnection->Execute((_bstr_t)szCmdText, &RecordsAffected, adCmdText);
		//		fIsCreateViewFault = false;
		//	}
		//	catch (_com_error e)
		//	{
		//		fIsCreateViewFault = true;
		//		CString strErrorMessage;
		//		strErrorMessage.Format("创建视图失败... 错误码:%d", GetLastError());
		//		SaveLog(strErrorMessage.GetBuffer());
		//		SaveLog(e.Description());
		//		OutPutMessage("创建视图失败...");
		//	}
		//}
	
	}

	char szCmdText[4096] = {0};
	CString strValues;
	try
	{
		strValues.Format(	"(%d,'%s','%s',%d,'%s','%s',%d,%d,%d,%d,%d,%d,%d,%d,%d,'%s','%s','%s','%s','%s','%s','%s','%s')", 
			pElement->iLocationId, strTime.GetBuffer(), pElement->szPlateNo, pElement->iRoadNo, pElement->szRoadName, pElement->szRoadDirect, pElement->iCarSpeed,
			pElement->iObjId, pElement->iTimeHeadway, pElement->iOverLine, pElement->iCorssLine,
			pElement->iEmergencyStop, pElement->iReverseRun,pElement->iSpeeding,pElement->iSpecialResult, pElement->szBigImagePath_1,
			pElement->szBigImagePath_2, pElement->szBigImagePath_3, pElement->szBigImagePath_4,
			pElement->szBigImagePath_5, pElement->szSmallImgePath, pElement->szBinImagePath,
			pElement->szVideoPath);
	
		sprintf(szCmdText, "INSERT INTO %s (%s) VALUES%s", strCurTable.GetBuffer(), m_strColumns.GetBuffer(), strValues.GetBuffer());
		m_pConnection->Execute((_bstr_t)szCmdText, &RecordsAffected, adCmdText);
		m_strLastTable = strCurTable;
	}
	catch (_com_error e)
	{
		m_fIsConnecting = FALSE;
		CString strErrorMessage;
		strErrorMessage.Format("数据库写入失败！错误信息:%s 错误码:%d", e.ErrorMessage(), GetLastError());
		SaveLog(strErrorMessage.GetBuffer());
		SaveLog(e.Description());
		OutPutMessage("数据库写入失败...");
	}
	return hr;
}


HRESULT CHveDbController::GetHveAddrInfo(SINGLE_HVE_ELEMENT* pSingleHveElement, int* pnCount)
{
	HRESULT hr = S_OK;
	if(!m_fComIsInited)
	{
		::CoInitialize(NULL);
		m_fComIsInited;
	}

	try
	{
		_RecordsetPtr pPtr;
		if(FAILED(pPtr.CreateInstance("ADODB.Recordset")))
		{
			OutPutMessage("创建ADO数据集失败，获取设备IP地址失败...");
			return E_FAIL;
		}

		CString strConnectInfo;
		strConnectInfo.Format("Driver=SQL Server;Server=%s;DATABASE=%s;UID=%s;PWD=%s",
			m_strServerIP, m_strDataBase, m_strUserName, m_strPassword);
		CString strCommand = "if not exists(select * from syscolumns where id=object_id('Hve_Road_Info') and name='Road_Name') begin ALTER TABLE hve_road_info add Road_Name char(25) end select Hve_Addr from Hve_Road_Info group by Hve_Addr";
		if(FAILED(pPtr->Open(_variant_t(strCommand), _variant_t(strConnectInfo), adOpenStatic, adLockOptimistic, adCmdText)))
		{
			OutPutMessage("Open Hve_Road_Info failed...");
			pPtr.Release();
			return E_FAIL;
		}
		int iIndex = 0;
		while(!pPtr->GetadoEOF())
		{
			CString strHveAddr = (char*)_bstr_t(pPtr->GetCollect("Hve_Addr"));
			strHveAddr.TrimRight();
			pSingleHveElement[iIndex++].strAddr = strHveAddr;
			pPtr->MoveNext();
		}
		*pnCount = iIndex;
		pPtr->Close();
		
		char szOutPutInfo[256];
		sprintf(szOutPutInfo, "连接Hve总数:%d", *pnCount);
		OutPutMessage(szOutPutInfo);
		for(int i=0; i<*pnCount; i++)
		{
			sprintf(szOutPutInfo, "Hve地址%02d : %s", i, pSingleHveElement[i].strAddr);
			OutPutMessage(szOutPutInfo);
		}

		for(int i=0; i<*pnCount; i++)
		{
			strCommand.Format("select Location_Id, Road_No from Hve_Road_Info where Hve_Addr='%s' order by Road_No",
				pSingleHveElement[i].strAddr);
			if(FAILED(pPtr->Open(_variant_t(strCommand), _variant_t(strConnectInfo), adOpenStatic, adLockOptimistic, adCmdText)))
			{
				OutPutMessage("Open Hve_Road_Info failed...");
				pPtr.Release();
				return E_FAIL;
			}
			int iIndex = 0;
			while(!pPtr->GetadoEOF())
			{
				int iLocationId = int(pPtr->GetCollect("Location_Id"));
				int iRoadNo = int(pPtr->GetCollect("Road_No"));
				pSingleHveElement[i].iLocationId = iLocationId;
				pSingleHveElement[i].rgiRoadNo[iIndex++] = iRoadNo;
				pPtr->MoveNext();
			}
			pPtr->Close();
		}
		strCommand = "select Obj_Id, Obj_Name from Hve_Obj_Type";
		if(FAILED(pPtr->Open(_variant_t(strCommand), _variant_t(strConnectInfo), adOpenStatic, adLockOptimistic, adCmdText)))
		{
			OutPutMessage("Open Hve_Road_Info failed...");
			pPtr.Release();
			return E_FAIL;
		}

		while(!pPtr->GetadoEOF())
		{
			int iObjId = (int)(pPtr->GetCollect("Obj_Id"));
			CString strObjName = (char*)_bstr_t(pPtr->GetCollect("Obj_Name"));
			strObjName.TrimRight();
			if(iObjId >= 0 && iObjId < MAX_OBJ_TYPE)
			g_rgstrObjType[iObjId] = strObjName;
			pPtr->MoveNext();
		}
		pPtr->Close();
		pPtr.Release();
	}
	catch (_com_error e)
	{
		CString strErrorMessage;
		strErrorMessage.Format("连接数据库失败! 错误信息:%s", e.ErrorMessage());
		SaveLog(strErrorMessage.GetBuffer());
		SaveLog(e.Description());
		OutPutMessage("连接数据库失败，获取设备IP地址失败...");
		return E_FAIL;
	}

	return S_OK;
}

DWORD CHveDbController::SaveProc()
{
	//while(!m_fExit)
	//{
	//	if(m_pSemaphore->Lock(1000) == S_OK)
	//	{
	//		m_pcsResult->Lock();
	//		RESULT_ELEMENT cResult = m_pListResult->RemoveHead();
	//		m_pcsResult->Unlock();
	//		if(cResult.dw64TimeMs != 0)
	//		{
	//			SaveDBElement(&cResult);
	//		}
	//	}
	//}
	//return 0;


	while(!m_fExit)
	{
		if(m_pSemaphore->Pend(1000) == S_OK)
		{
			if(m_pListResult->GetCount() >  0)
			{
				m_pcsResult->Lock();
				RESULT_ELEMENT cResult = m_pListResult->RemoveHead();
				m_pcsResult->Unlock();
				if(cResult.dw64TimeMs != 0)
				{
					SaveDBElement(&cResult);
				}
			}
		}
	}
	return 0;


}

