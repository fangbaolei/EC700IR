/**
* @file
* @brief 
*
*/

#ifndef _AUTOTEST_H_
#define _AUTOTEST_H_

#include "SWThread.h"

typedef struct __TEST_RESULT
{
	CHAR	szItemName[32];
	BOOL	fOK;
	DWORD   dwTestCount;
	DWORD   dwFailCount;
}_TEST_RESULT;



class CAutotest// : public CSWApplication
{
	//CLASSINFO(CAutotest, CSWApplication);
public:
	
	/**
	* @brief 
	*/
	CAutotest();

	/**
	* @brief 
	*/
	virtual ~CAutotest();

	HRESULT ReleaseObj();

	/**
	* @brief 
	* @param []
	* @retval 
	*/
    virtual HRESULT Initialize();

	HRESULT ReceiveCmd(const DWORD dwCmdID, const PBYTE pbCmdData, const DWORD dwDataSize, PVOID* ppvOutBuf, DWORD* pdwOutSize);

	
    virtual HRESULT RestoreAutotest();

    virtual HRESULT AutotestRunOnce(_TEST_RESULT* sResult, DWORD& dwCount);

    virtual HRESULT Autotest();

protected:	
	

	HRESULT GenerateReport(const DWORD dwReportNo, const _TEST_RESULT* psResult, const DWORD dwCount, CHAR** ppszReport, BOOL* pbOK);

	
	HRESULT SendReport(const CHAR* pszReport, const BOOL bOK);

	
	HRESULT SaveReport(const CHAR* pszReport);


	HRESULT LoadFactoryModeFPGA();

	
	HRESULT ProcessAutotestCmd(const INT iTestCount, const CHAR* szSendIp, const WORD wSendPort);

	HRESULT ProcessXmlCommand(const PBYTE pbBuf, const DWORD dwSize, PVOID* ppvOutBuf, DWORD* pdwOutSize);
	
	HRESULT SendReportSavedInEEPROM(const CHAR* szSendIp, const WORD wSendPort);


	HRESULT SaveAutotestProgress();

	
	HRESULT GetAutotestStatus(PVOID* ppvOutBuf, DWORD* pdwOutSize);

	
	HRESULT SetAutotestStopFlag(const BOOL fStop);

	
	BOOL GetAutotestStopFlag();
		

private:
	
	static PVOID AutotestProxy(PVOID pvArg);

protected:
	
	BOOL	m_fInited;
	BOOL	m_fStop;
	
	INT		m_iTestCount;
	INT		m_iNGCount;
	INT		m_iRunCount;
	//CHAR	m_szSavePath[256];

	CHAR	m_szSendIp[32];
	WORD	m_wSendPort;
	
	CSWThread* 		m_pAutotestThread;
	DWORD	m_dwTestProgress;
	
	_TEST_RESULT m_sResultTable[16];
};



#endif //_AUTOTEST_H_

