#pragma once
#include <afxtempl.h>
#include <atlstr.h>
#include <afxmt.h>
#include "HveConnecter.h"
#include "Semaphore.h"

#import "c:\program files\common files\system\ado\msado15.dll" no_namespace rename ("EOF", "adoEOF") 

class CHveDbController
{
public:
	CHveDbController(CList<RESULT_ELEMENT>* pListResult, SINGLE_HVE_ELEMENT* pSingleHveElement,
		ISemaphore* pSemaphore, CCriticalSection* pcsResult);
	~CHveDbController(void);
	HRESULT GetHveAddrInfo(SINGLE_HVE_ELEMENT* pSingleHveElement, int* pnCount);
	HRESULT SaveDBElement(RESULT_ELEMENT* pElement);
	HRESULT ConnectDB();
	void SaveLog(char* pszLog);

	DWORD SaveProc();
	static DWORD WINAPI SaveProcProxy(LPVOID lpParameter)
	{
		if(lpParameter)
		{
			CHveDbController* pHveDbController = (CHveDbController*)lpParameter;
			return pHveDbController->SaveProc();
		}
		return -1;
	}

public:
	_ConnectionPtr	m_pConnection;
	CString m_strColumns;
	CString m_strServerIP;
	CString m_strDataBase;
	CString m_strUserName;
	CString m_strPassword;
	CString m_strIniPath;
	BOOL	m_fIsConnecting;
	BOOL	m_fComIsInited;
	HANDLE	m_hSaveThread;
	BOOL	m_fExit;
private:
	CList<RESULT_ELEMENT>*	m_pListResult;
	SINGLE_HVE_ELEMENT*		m_SingleHveElement;
	ISemaphore*				m_pSemaphore;
	CCriticalSection*		m_pcsResult;
	CString					m_strLastTable;
};
