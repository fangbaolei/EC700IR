#include "stdafx.h"
#include "PlateFilter.h"
#include "HveManager.h"

extern void OutPutMessage(LPSTR szMsg);

BOOL ComparePlateString(char* pszStr1, char* pszStr2)
{
	static int STR_BEGING_POS = 4;
	if(pszStr1 == NULL || pszStr2 == NULL)
	{
		return FALSE;
	}

	int iLen1 = (int)strlen(pszStr1);
	int iLen2 = (int)strlen(pszStr2);

	if(iLen1 != iLen2)
	{
		return FALSE;
	}

	if(iLen1 <= STR_BEGING_POS)
	{
		return FALSE;
	}
	int iCount = STR_BEGING_POS;
	for(int i=STR_BEGING_POS; i<iLen1; i++)
	{
		if(pszStr1[i] == pszStr2[i])
		{
			iCount++;
		}
	}
	if(iCount < iLen1 - 1)
	{
		return FALSE;
	}
	return TRUE;
}

CPlateFilter::CPlateFilter(void* pParent)
{
	m_pParent = pParent;
	m_dw64CurTime = 0;
	m_iTwinsTime = 500;
	m_iMaxPlateWaitTime = 2000;
	sprintf(m_szSpecialFiltrateChar, "无车牌");
	char szAppPath[MAX_PATH] = {0};
	GetModuleFileName(NULL, szAppPath, MAX_PATH);
	PathRemoveFileSpec(szAppPath);
	strcat(szAppPath, "\\Config.ini");
	char szTmp[256] = {0};
	GetPrivateProfileStringA("FilterInfo", "TwinsTime", "", szTmp, 256, szAppPath);
	if(strcmp(szTmp, "") == 0)
	{
		WritePrivateProfileStringA("FilterInfo", "TwinsTime", "500", szAppPath);
	}
	else
	{
		m_iTwinsTime = atoi(szTmp);
	}

	GetPrivateProfileStringA("FilterInfo", "MaxPlateWaitTime", "", szTmp, 256, szAppPath);
	if (strcmp(szTmp, "") == 0)
	{
		WritePrivateProfileStringA("FilterInfo", "MaxPlateWaitTime", "2000", szAppPath);
	}
	else
	{
		m_iMaxPlateWaitTime = atoi(szTmp);
	}

	GetPrivateProfileStringA("FilterInfo", "SpecialFiltrateChar", "", szTmp, 256, szAppPath);
	if (strcmp(szTmp, "") == 0)
	{
		WritePrivateProfileStringA("FilterInfo", "SpecialFiltrateChar", "无车牌", szAppPath);
	}
	else
	{
		strcpy(m_szSpecialFiltrateChar, szTmp);
	}
	m_fExit = FALSE;
	DWORD	dwThreadId;
	m_hFilterThread = CreateThread(NULL, 0, FilterProcProxy, this, 0, &dwThreadId);
}

CPlateFilter::~CPlateFilter(void)
{
	m_fExit = TRUE;
	if(m_hFilterThread)
	{
		int iWaitTimes = 0;
		int MAX_WAIT_TIME = 10;
		while(WaitForSingleObject(m_hFilterThread, 500) == WAIT_TIMEOUT && iWaitTimes < MAX_WAIT_TIME)
		{
			iWaitTimes++;
		}
		if(iWaitTimes >= MAX_WAIT_TIME)
		{
			TerminateThread(m_hFilterThread, 0);
			OutPutMessage("强行终止入库线程...");
		}
		CloseHandle(m_hFilterThread);
	}
}

DWORD CPlateFilter::FilterProc()
{
	char szInfo[100];
	while(!m_fExit)
	{
		Sleep(5);
//		m_csResult.Lock();
		for(int i=0; i<MAX_RESULT_BUF; i++)
		{
			for(int j=0; j<MAX_RESULT_BUF; j++)
			{
				if(m_rgResultBuf[i].iLocationId == m_rgResultBuf[j].iLocationId
					&& abs(m_rgResultBuf[i].iRoadNo - m_rgResultBuf[j].iRoadNo) == 1
					&& abs(int(m_rgResultBuf[i].dw64TimeMs - m_rgResultBuf[j].dw64TimeMs)) < m_iTwinsTime
					&& m_rgResultBuf[i].szPlateNo[0] != 0
					&& m_rgResultBuf[j].szPlateNo[0] != 0)
				{
					if(ComparePlateString(m_rgResultBuf[i].szPlateNo, m_rgResultBuf[j].szPlateNo))
					{
						sprintf(szInfo, "ResultDataFilter: %s", m_rgResultBuf[i].szPlateNo);
						OutPutMessage(szInfo);
						((CHveManager*)m_pParent)->m_pDbContoller->SaveLog(szInfo);
						m_rgResultBuf[i].szPlateNo[0] = 0;
					}
					else if(strstr(m_rgResultBuf[i].szPlateNo, m_szSpecialFiltrateChar) != NULL)
					{
						sprintf(szInfo, "ResultDataFilter SpecialFiltrateChar:%s", m_rgResultBuf[i].szPlateNo);
						OutPutMessage(szInfo);
						((CHveManager*)m_pParent)->m_pDbContoller->SaveLog(szInfo);
						m_rgResultBuf[i].szPlateNo[0] = 0;
					}
					else if(strstr(m_rgResultBuf[j].szPlateNo, m_szSpecialFiltrateChar) != NULL)
					{
						sprintf(szInfo, "ResultDataFilter SpecialFiltrateChar:%s", m_rgResultBuf[j].szPlateNo);
						OutPutMessage(szInfo);
						((CHveManager*)m_pParent)->m_pDbContoller->SaveLog(szInfo);
						m_rgResultBuf[j].szPlateNo[0] = 0;
					}
				}
			}
		}
//		m_csResult.Unlock();
		for(int i=0; i<MAX_RESULT_BUF; i++)
		{	
			if((m_rgResultBuf[i].szPlateNo[0] != 0 && m_dw64CurTime - m_rgResultBuf[i].dw64TimeMs >= m_iMaxPlateWaitTime) || 
				(m_rgResultBuf[i].szPlateNo[0] != 0 && GetTickCount() - m_rgResultBuf[i].dw64CreateTimeMs >= m_iMaxPlateWaitTime  )) 
			{
				//识别结果重复写入数据库
				//OutPutMessage((LPSTR)m_rgResultBuf[i].szPlateNo);
				((CHveManager*)m_pParent)->AddResult(m_rgResultBuf[i]);
				m_rgResultBuf[i].szPlateNo[0] = 0;
			}
		}
	}
	return 0;
}

HRESULT CPlateFilter::AddResult(RESULT_ELEMENT result)
{
	HRESULT hr = S_OK;
//	m_csResult.Lock();
	m_dw64CurTime = result.dw64TimeMs;
//	m_dw64CurTime = GetTickCount();
	int i;
	for(i=0; i<MAX_RESULT_BUF; i++)
	{
		if(0 == m_rgResultBuf[i].szPlateNo[0])
		{
			break;
		}
	}

	if(i < MAX_RESULT_BUF)
	{
		m_rgResultBuf[i] = result;
	}
	else
	{
		OutPutMessage("Result buf full!!!");
		hr = E_OUTOFMEMORY;
	}
//	m_csResult.Unlock();
//	((CHveManager*)m_pParent)->AddResult(result);
	return S_OK;
}