#pragma once
#include <afxtempl.h>
#include <atlstr.h>
#include <afxmt.h>
#include "HveConnecter.h"

const int MAX_RESULT_BUF = 400;

class CPlateFilter
{
public:
	CPlateFilter(void* pParent);
	~CPlateFilter(void);
	DWORD FilterProc();
	static DWORD WINAPI FilterProcProxy(LPVOID lpParament)
	{
		if(lpParament)
		{
			CPlateFilter* pFilter = (CPlateFilter*)lpParament;
			return pFilter->FilterProc();
		}
		return -1;
	}
	HRESULT AddResult(RESULT_ELEMENT cResult);
private:
	HANDLE	m_hFilterThread;
	BOOL	m_fExit;
	RESULT_ELEMENT m_rgResultBuf[MAX_RESULT_BUF];
	CCriticalSection m_csResult;
	DWORD64 m_dw64CurTime;
	int m_iTwinsTime;
	int m_iMaxPlateWaitTime;
	char m_szSpecialFiltrateChar[256];
	void* m_pParent;
};
