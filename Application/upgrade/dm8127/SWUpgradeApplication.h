/**
* @file
* @brief 
*
*/

#ifndef _SW_UPGRADE_APPLICATION_H_
#define _SW_UPGRADE_APPLICATION_H_

#include "SWApplication.h"

#include "SWThread.h"


#include "Autotest.h"
#include "AutotestJupiter.h"

#include "swpa.h"
#include "SWFifo.h"

#include "SWProcessXMLCmd.h"

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

	HRESULT UpgradeImage(const DWORD dwSize, CSWTCPSocket& sockData);
	
	HRESULT GetUpgradeImageStatus(PVOID* pvBuf, DWORD* pdwSize);

	HRESULT OnUpgradeImage();
	
	static PVOID OnUpgradeImageProxy(PVOID pvArg);
	
	HRESULT OnOptResetMode(PVOID* ppvOutBuf, DWORD* pdwOutSize);

	inline DWORD MyNetToHostLong(DWORD);
	
	inline DWORD MyHostToNetLong(const DWORD dwVal);

	HRESULT ProcessXmlCmd(const PBYTE pbBuf, const DWORD dwSize, PVOID* ppvOutBuf, DWORD* pdwOutSize);	
	
	VOID DecryptPackageByMAC(BYTE* pbData, INT iLen, BYTE* pbKeyMAC, ULONG* ulPos);
	
	HRESULT DecryptPackage(PBYTE& pbUpgradePacket);
	
	HRESULT GetDeviceInfo(PVOID* ppvOutBuf, DWORD* pdwPackLen);

	HRESULT OnForceUpgrade(const PVOID pvBuf, const DWORD dwSize);
	
	HRESULT OnWriteSecCode(const CHAR* szServerIp, const WORD wServerPort);
	
	INT GetVersion(VOID);
	
	HRESULT CheckResetMode();
	
	HRESULT IPConfig();
	
	HRESULT LEDBreath();
	
	HRESULT ProcessNetCommand(SWPA_SOCKET_T outSock);
	
	HRESULT ReadPartitionCrc(const CHAR *szPartition,DWORD &dwCrc);
	
	HRESULT SavePartitionCrc(const CHAR *szPartition,const DWORD dwCrc);
	
	BOOL CheckAllPacketCouldUpgrade(const CHAR* pbData, const DWORD dwCount);
	
	HRESULT UMountPartition(const CHAR *szImageName);
	
	BOOL NeedUpgradeBlock(const CHAR* szName, 
		const DWORD dwNowCrc,CHAR *pBlockName,PBOOL pUbiImage,PBOOL pDecrypt);
	
	HRESULT WritePackageToBlock(const PVOID pvHeader, 
		const PBYTE pbData, const DWORD dwSize,const CHAR *szBlockName,BOOL fUbiImage);
	
	HRESULT OnWriteImage();
	
	static PVOID OnWriteImageProxy(PVOID pvArg);
	
	static INT GetDataFromFifo(void *buf,unsigned int unLen,void *lp);
	
	HRESULT UpgradeSingleImage(DWORD &dwImageSize,BOOL fMacDecrypt,PBYTE pMac,ULONG ulDecryptPos);
	
	HRESULT OnUpgradeLargeImage();
	
	static PVOID OnUpgradeLargeImageProxy(PVOID pvArg);
	
	HRESULT UpgradeLargeImage(const DWORD dwSize);
	
	HRESULT SaveBackupSystemVersion(const DWORD dwVersion);
	
	HRESULT ReadBackupSystemVersion(DWORD* pdwVersion);
	
	HRESULT SaveCustomizedDevName(LPCSTR szDevName);

	HRESULT ReadCustomizedDevName(LPSTR szDevName);

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

	BOOL IsBackupMode()
	{
		return (1 == m_dwMode);
	}

private:
	
	BOOL	m_fInited;
	WORD	m_wPort;
	WORD	m_wProbePort;
	
	CSWThread *		m_pProbeThread;
	CSWThread *		m_pUpgradeThread;

	DWORD	m_dwSETIP_COMMAND;
	DWORD	m_dwGETIP_COMMAND;

	BOOL	m_fUpgrading;
	BOOL	m_fWriting;
	BOOL	m_fSwitchingSystem;

	PBYTE	m_pbUpgradeImage;
	DWORD	m_dwImageLen;
	CSWTCPSocket	m_sockData;

	INT		m_iUpgradeProgress;	
    BOOL 	m_fIsUpgradeFailed;		//升级失败标记

	BOOL	m_fGoingToExit;
	INT		m_iExitCode;

	BOOL    m_fAutotestStarted;
    CAutotest *m_pAutotest;

	CSWFifo * m_pFifo;

	DWORD	m_dwSingleImageLen;//单个镜像大小
	IMAGE_PACKAGE_HEADER m_cImageHeader;
	CHAR	m_szBlockName[32];
	BOOL	m_fUbiImage;

	DWORD	m_dwMode; //工作模式: 0-升级模式；1-备份模式
    DWORD   m_dwDevType; // 设备类型 0-金星 1-木星

	BOOL	m_fForceUpgrade; //强制升级分区标记
	BOOL	m_fSupportNoBreakUpgrade;	//支持不复位设备即可获取模式及升级设备的标记

    CSWProcessXMLCmd m_ProcessXMLCmd;   //命令处理对象
};


//extern CSWUpgradeApp theApp;


#endif //_SW_UPGRADE_APPLICATION_H_

