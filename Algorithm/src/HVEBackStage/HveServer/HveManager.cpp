#include "stdafx.h"
#include "atltime.h"
#include "HveManager.h"

static DWORD g_dwRecvieCount = 0;
static DWORD g_dwSaveCount = 0;

CString g_rgstrObjType[MAX_OBJ_TYPE];
extern void OutPutMessage(LPSTR szMsg);

CHveManager::CHveManager(void)
{
	m_dwLastSetTimeTick = 0;
	m_iCurHveCount = 0;
	for(int i=0; i<MAX_HVE_COUNT; i++)
	{
		m_rgpHveConnecter[i] = NULL;
	}

//	HiVideo::ISemaphore::CreateInstance(&m_psemResult, 0, MAX_RESULT_LIST);
//	m_psemResult = new CSemaphore(MAX_RESULT_LIST, MAX_RESULT_LIST);
	ISemaphore::CreateInstance(&m_psemResult, 0, MAX_RESULT_LIST);
	m_pDiskManager = new CDiskManager;
	m_pDbContoller = new CHveDbController(&m_listResult, &m_rgSingleHve[i], m_psemResult, &m_csResult);
	if(m_pDbContoller->GetHveAddrInfo(m_rgSingleHve, &m_iCurHveCount) != S_OK)
	{
		ExitProcess(0);
	}
	m_pPlateFilter = new CPlateFilter(this);

	

	for(int i=0; i<m_iCurHveCount; i++)
	{
		m_rgpHveConnecter[i] = new CHveConnecter(&m_rgSingleHve[i], m_pPlateFilter, m_pDiskManager);
	}
	m_fExit = FALSE;
	DWORD dwThreadId;
	m_hCheckHveStatusThread = CreateThread(NULL, 0, CheckHeStatusThread, this, 0, &dwThreadId);

}

CHveManager::~CHveManager(void)
{
	m_fExit = TRUE;
	if(m_hCheckHveStatusThread)
	{
		int iWaitTimes = 0;
		int MAX_WAIT_TIME = 10;
		while(WaitForSingleObject(m_hCheckHveStatusThread, 500) == WAIT_TIMEOUT && iWaitTimes < MAX_WAIT_TIME)
		{
			iWaitTimes++;
		}
		if(iWaitTimes >= MAX_WAIT_TIME)
		{
			TerminateThread(m_hCheckHveStatusThread, 0);
			OutPutMessage("强行关闭侦听线程...");
		}
		CloseHandle(m_hCheckHveStatusThread);
	}
	for(int i=0; i<m_iCurHveCount; i++)
	{
		if(m_rgpHveConnecter[i])
		{
			delete m_rgpHveConnecter[i];
		}
	}
	if(m_pPlateFilter)
	{
		delete m_pPlateFilter;
	}
	if(m_pDbContoller)
	{
		delete m_pDbContoller;
	}
	if(m_psemResult)
	{
		delete m_psemResult;
		m_psemResult = NULL;
	}
}

void CHveManager::CheckHveStatus(void)
{
	bool fTimeSet = false;
	for(int i=0; i<m_iCurHveCount; i++)
	{	
		if(NULL == m_rgpHveConnecter[i]->m_hHv)
		{
			m_rgpHveConnecter[i]->m_hHv = HVAPI_OpenEx(m_rgpHveConnecter[i]->m_pSingleHveEleMent->strAddr.GetBuffer(), NULL);
			if(m_rgpHveConnecter[i]->m_hHv)
			{
				if(m_rgpHveConnecter[i]->SetCallBack() != S_OK)
				{
					HVAPI_CloseEx(m_rgpHveConnecter[i]->m_hHv);
					m_rgpHveConnecter[i]->m_hHv = NULL;
				}
				else
				{
					char szOutPutMsg[100];
					sprintf(szOutPutMsg, "连接成功:%s", m_rgpHveConnecter[i]->m_pSingleHveEleMent->strAddr.GetBuffer());
					OutPutMessage(szOutPutMsg);
				}
			}

			if(m_rgpHveConnecter[i]->m_hHv)
			{
				char* szXmlParam = new char[512*1024];
				INT nRetLen = 0;
				if(HVAPI_GetParamEx(m_rgpHveConnecter[i]->m_hHv, szXmlParam, 512*1024, &nRetLen) == S_OK)
				{
					CString strParam;
					int iplace = 0;
					strParam.Format("%s",szXmlParam);
					iplace = strParam.Find("StreetName");
					strParam.Delete(0,iplace);
					iplace = strParam.Find("路口名称");
					strParam.Delete(iplace,strParam.GetLength()-1);
					iplace = strParam.Find("</VALUE>");
					strParam.Delete(iplace,strParam.GetLength()-1);
					strParam.Delete(0,strParam.Find("STR")+5);
					m_pDbContoller->ConnectDB();
					_variant_t RecordsAffected;
					char szCmdText[1024] = {0};
					sprintf(szCmdText, "update hve_road_info set road_name ='%s' where hve_addr = '%s'", strParam.GetBuffer(), m_rgpHveConnecter[i]->m_pSingleHveEleMent->strAddr.GetBuffer());
					m_pDbContoller->m_pConnection->Execute((_bstr_t)szCmdText, &RecordsAffected, adCmdText);

				}
				delete[] szXmlParam;
			}
		}
		//else
		//{
		//	DWORD iState =  CONN_STATUS_NORMAL;
		//	//if(S_OK != HvIsConnected(m_rgpHveConnecter[i]->m_hHv, &iState)) 
		//	if(S_OK != HVAPI_GetConnStatusEx(m_rgpHveConnecter[i]->m_hHv, CONN_TYPE_RECORD, &iState))
		//	{
		//		iState = CONN_STATUS_DISCONN;
		//	}
		//	if(iState !=  CONN_STATUS_NORMAL)
		//	{
		//		HVAPI_CloseEx(m_rgpHveConnecter[i]->m_hHv);
		//		m_rgpHveConnecter[i]->m_hHv = NULL;
		//		char szOutPutInfo[100];
		//		sprintf(szOutPutInfo, "连接断开:%s", m_rgpHveConnecter[i]->m_pSingleHveEleMent->strAddr.GetBuffer());
		//		OutPutMessage(szOutPutInfo);
		//	}
		//}
		if(m_rgpHveConnecter[i]->m_hHv)
		{
			if(GetTickCount() - m_dwLastSetTimeTick >= 3600000 || m_dwLastSetTimeTick == 0)
			{
				//__time64_t tm;
				//tm = ::_time64(NULL);
				//if(SetHvTime(m_rgpHveConnecter[i]->m_hHv, tm * 1000) == S_OK)
				char szRetBuf[512] = {0};
				int iRetLen = 512;
				CTime t = CTime::GetCurrentTime();
				CString str;
				str.Format("SetTime,Date[%d.%02d.%02d],Time[%02d:%02d:%02d]",t.GetYear(),t.GetMonth(),t.GetDay(),t.GetHour(), t.GetMinute(), t.GetSecond());
				LPSTR szParamDoc = str.GetBuffer(str.GetLength()+1);
				if(HVAPI_ExecCmdEx(m_rgpHveConnecter[i]->m_hHv, szParamDoc, szRetBuf, 512, &iRetLen) == S_OK)
				{
					char szOutPutInfo[100];
					sprintf(szOutPutInfo, "同步时间成功:%s", m_rgpHveConnecter[i]->m_pSingleHveEleMent->strAddr.GetBuffer());
					OutPutMessage(szOutPutInfo);
				}
				fTimeSet = true;
			}
		}
		else
		{
			char szOutPutInfo[100];
			sprintf(szOutPutInfo, "连接失败:%s", m_rgpHveConnecter[i]->m_pSingleHveEleMent->strAddr.GetBuffer());
			OutPutMessage(szOutPutInfo);
		}
	}
	if(fTimeSet)
	{
		m_dwLastSetTimeTick = GetTickCount();
	}
}

void CHveManager::AddResult(RESULT_ELEMENT result)
{
	m_csResult.Lock();
	m_listResult.AddTail(result);
	m_csResult.Unlock();
	m_psemResult->Post(ISemaphore::WAIT);
}

DWORD CHveManager::CheckHeStatusThread(LPVOID lpParameter)
{
	CHveManager* m_manager = (CHveManager*)lpParameter;
	while(!m_manager->m_fExit)
	{
		m_manager->CheckHveStatus();
		Sleep(1000);
	}
	return 0;
}
