#ifndef __SW_DEAMON_APPLICATION_H__
#define __SW_DEAMON_APPLICATION_H__
class CSWDeamonApp : public CSWApplication
{
	CLASSINFO(CSWDeamonApp, CSWApplication)
public:
	CSWDeamonApp();
	virtual ~CSWDeamonApp();
	virtual HRESULT InitInstance(const WORD wArgc, const CHAR** szArgv);
	virtual HRESULT ReleaseInstance();
	virtual HRESULT Run();
protected:
	HRESULT EnableDog(DWORD dwTimeOut = 6000);
	HRESULT FeedDog(VOID);
	HRESULT DisableDog(VOID);
	HRESULT CheckProcess(TiXmlElement *el, INT *dwExitCode);
#ifdef DM8127	
	HRESULT RestoreFactorySetting(VOID);
	BOOL ResetKeyPressed(VOID);
	HRESULT CheckResetKeyEvent(VOID);
#endif
	HRESULT StartApps();
	static PVOID MonitorProxy(PVOID pvArg);
	HRESULT Monitor();
	
private:
	TiXmlDocument m_xmlDoc;	
	DWORD m_dwTotalPID;
	DWORD m_dwLastTime;
	BOOL  m_fPrint;
	CSWFile m_clsFile;
	BOOL  m_fDisableDog;

	BOOL m_fExit;
	CSWThread m_cMonitorThread;
};
extern CSWDeamonApp* theApp;
#endif

