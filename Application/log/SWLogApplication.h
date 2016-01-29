#ifndef __SW_LOG_APPLICATION_H__
#define __SW_LOG_APPLICATION_H__

#define USE_LOCALSOCKET
//#define USE_TAIL

class CSWLogApplication : public CSWApplication
{
	CLASSINFO(CSWLogApplication, CSWApplication)
public:
	CSWLogApplication();
	virtual ~CSWLogApplication();
protected:	
	virtual HRESULT InitInstance(const WORD wArgc, const CHAR** szArgv);
	virtual HRESULT ReleaseInstance();
	virtual HRESULT Run();
#ifdef USE_LOCALSOCKET
	static PVOID OnLog(PVOID pvParam);
	static PVOID OnPrint(PVOID pvParam);

	CSWThread m_cLogThreads[8];
	CSWThread m_cPrintThread;
	CSWSemaphore m_cSema[8];
	CSWSemaphore m_cSemaLock[8];
	CSWList<PVOID> m_lstLog[8];
#else
#ifndef USE_TAIL
	DWORD   GetINode(LPCSTR szFileName);	
#else
	static PVOID OnLog(PVOID pvParam);
	CSWThread m_cLogThread;
#endif
#endif
};
CREATE_PROCESS(CSWLogApplication)
#endif
