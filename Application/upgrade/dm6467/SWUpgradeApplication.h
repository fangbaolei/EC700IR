/**
* @file
* @brief 
*
*/

#ifndef _SW_UPGRADE_APPLICATION_H_
#define _SW_UPGRADE_APPLICATION_H_

#include "SWApplication.h"

#include "SWThread.h"

#ifdef ENABLE_AUTOTEST
#include "Autotest.h"
#endif



class CSWUpgradeApp : public CSWApplication
{
	CLASSINFO(CSWUpgradeApp, CSWApplication);
public:
	
	/**
	* @brief 
	*/
	CSWUpgradeApp();

	/**
	* @brief 
	*/
	virtual ~CSWUpgradeApp();

	virtual HRESULT InitInstance(const WORD wArgc, const CHAR** szArgv);
	virtual HRESULT ReleaseInstance();

	/**
	* @brief 
	* @param []
	* @retval 
	*/
	HRESULT Initialize();

	virtual HRESULT Run();

	virtual HRESULT OnException(INT iSignalNo);

	HRESULT ReleaseObj();

	
	static HRESULT Breath(VOID)
	{
		return HeartBeat();
	}


private:	

	
	
	HRESULT ParseProbeXmlMsg(CHAR * pszMsg);
	
	HRESULT GenerateProbeReplyXml(CHAR ** ppszXmlBuf, DWORD *pdwXmlLen);
	
	HRESULT GenerateSetIPCMDReplay(const BYTE* pbMsg, const DWORD dwMsgLen, BYTE** pbReplyPack, DWORD* pdwPackLen);

	HRESULT OnProbe(VOID);

	
	HRESULT UpgradeKernelBak(const PVOID pvBuf, const DWORD dwSize);

	HRESULT UpgradeRootfsBak(const PVOID pvBuf, const DWORD dwSize);
	

	HRESULT UpgradeImage();

	
	HRESULT GetUpgradeImageStatus(PVOID* pvBuf, DWORD* pdwSize);

	
	HRESULT OnUpgradeImage();
	
	static PVOID OnUpgradeImageProxy(PVOID pvArg);
	
	HRESULT OnOptResetMode(PVOID* ppvOutBuf, DWORD* pdwOutSize);

	inline DWORD MyNetToHostLong(DWORD);
	
	inline DWORD MyHostToNetLong(const DWORD dwVal);

	HRESULT ProcessXmlCmd(const PBYTE pbBuf, const DWORD dwSize, PVOID* ppvOutBuf, DWORD* pdwOutSize);
	
	HRESULT OnRestoreFactorySetting(PVOID* ppvOutBuf, DWORD* pdwOutSize);
	
	HRESULT OnUnlockFactorySetting(PVOID* ppvOutBuf, DWORD* pdwOutSize);

	
	HRESULT OnResetDevice(const INT iExitCode);

	
	HRESULT OnWriteSN(const CHAR* szSN, PVOID* ppvOutBuf, DWORD* pdwOutSize);

	
	VOID DecryptPackageByMAC(BYTE* pbData, INT iLen, BYTE* pbKeyMAC, ULONG* ulPos);

	
	HRESULT DecryptPackage(PBYTE& pbUpgradePacket);

	
	HRESULT GetDeviceInfo(PVOID* ppvOutBuf, DWORD* pdwPackLen);

	
	HRESULT ProcessWriteSecCodeCmd(const CHAR* szServerIp, const WORD wServerPort);

	
	INT GetVersion(VOID);
	

private:
	
#define IH_NMLEN 32	/* Image Name Length */
#define IH_TYPE_MULTI 4

	typedef struct image_header {
		DWORD	ih_magic;	/* Image Header Magic Number	*/
		DWORD	ih_hcrc;	/* Image Header CRC Checksum	*/
		DWORD	ih_time;	/* Image Creation Timestamp		*/
		DWORD	ih_size;	/* Image Data Size				*/
		DWORD	ih_load;	/* Data	 Load  Address			*/
		DWORD	ih_ep;		/* Entry Point Address			*/
		DWORD	ih_dcrc;	/* Image Data CRC Checksum		*/
		BYTE		ih_os;		/* Operating System		*/
		BYTE		ih_arch;	/* CPU architecture		*/
		BYTE		ih_type;	/* Image Type			*/
		BYTE		ih_comp;	/* Compression Type		*/
		BYTE		ih_name[IH_NMLEN];	/* Image Name	*/
	} IMAGE_PACKAGE_HEADER;

	static PVOID UpgradeProxy(PVOID pvArg);


	static VOID* OnProbeProxy(VOID* pvParam);

	HRESULT WriteUpgradePacket(const CHAR* pbData, INT iLen);

	
	BOOL NeedUpgrade(const CHAR* szName, const DWORD dwNowCrc);

	
	HRESULT WritePackage(const PVOID pvHeader, const PBYTE pbData, const DWORD dwSize);

	
	HRESULT WriteFile(const CHAR* szFileName, const PVOID pvHeader, const PBYTE pbData, const DWORD dwSize);

private:
	
	BOOL	m_fInited;
	WORD	m_wPort;
	WORD	m_wProbePort;
	
	CSWThread *		m_pProbeThread;
	CSWThread *		m_pUpgradeThread;

	//CSWTCPSocket*	m_pSockServer;
	DWORD	m_dwSETIP_COMMAND;
	DWORD	m_dwGETIP_COMMAND;

	PBYTE	m_pbUpgradeImage;
	DWORD	m_dwImageLen;

	INT		m_iUpgradeProgress;	

	BOOL	m_fGoingToExit;
	INT		m_iExitCode;

#ifdef ENABLE_AUTOTEST
	BOOL    m_fAutotestStarted;
	CAutotest m_cAutotest;
#endif
	
};


//extern CSWUpgradeApp theApp;


#endif //_SW_UPGRADE_APPLICATION_H_

