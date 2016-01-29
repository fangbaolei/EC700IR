#pragma once
#include "afxtempl.h"
#include "atlstr.h"
#include "afxmt.h"
#include "DiskManager.h"
#include "HveConnecter.h"
#include "HveDbController.h"
#include "PlateFilter.h"
#include "Semaphore.h"


const int MAX_HVE_COUNT	= 100;
const int MAX_RESULT_LIST = 2000;

class CHveManager
{
public:
	CHveManager(void);
	~CHveManager(void);
	void CheckHveStatus(void);
	void AddResult(RESULT_ELEMENT result);
	static DWORD WINAPI CheckHeStatusThread(LPVOID lpParameter);

private:
	SINGLE_HVE_ELEMENT	m_rgSingleHve[MAX_HVE_COUNT];
	CHveConnecter*		m_rgpHveConnecter[MAX_HVE_COUNT];
	CDiskManager*		m_pDiskManager;
	int					m_iCurHveCount;
public:
	CHveDbController*	m_pDbContoller;
private:
	CList<RESULT_ELEMENT>	m_listResult;
	ISemaphore*				m_psemResult;
	CCriticalSection		m_csResult;
	DWORD32					m_dwLastSetTimeTick;
	CPlateFilter*			m_pPlateFilter;
	HANDLE					m_hCheckHveStatusThread;
	BOOL					m_fExit;
};