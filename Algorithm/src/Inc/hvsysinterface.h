#ifndef _HV_SYS_INTERFACE_
#define _HV_SYS_INTERFACE_

#include "swbasetype.h"
#include "swobjbase.h"

BOOL MyGetIpDWord(LPSTR lpszIP, DWORD32& dwIP);
BOOL MyGetIpString(DWORD32 dwIP, LPSTR lpszIP);

extern const char* PSZ_DSP_BUILD_NO;
extern const char* DSP_BUILD_DATE;
extern const char* DSP_BUILD_TIME;

namespace HvSys
{
	typedef enum _SYS_INFO_TYPE
	{
		//系统信息 0x00 ~ 0x1F
		INFO_SYS_ID = 0x00,	//系统ID
		INFO_SYS_NAME = 0x01,	//系统名字
		INFO_SYS_VERSION = 0x02,	//版本号
		INFO_PRODUCT_NAME = 0x03,	//读取设备编号

		//模式信息 0x20 ~ 0x3F
		INFO_BOOTMODE = 0x20,	//复位模式

		INFO_FONTMODE = 0x21,	//设置和读取系统字符集选项
		INFO_FONTMODE_LIST = 0x22,	//字符集名称列表,可指定序号

		INFO_WORKMODE = 0x23,	//当前工作模式
		INFO_WORKMODE_LIST = 0x24,	//工作模式列表,可指定序号

		//系统参数 0x40 ~ 0x5F
		INFO_RESET_COUNT = 0x40, //读取系统复位次数
		INFO_SYS_LED = 0x41,		//设置和读取系统指示灯
		INFO_SYS_TIME = 0x42,		//系统时钟
		INFO_SYS_PWD = 0x43,	//密码

		//报告文件 0x60 ~ 0x7F
		INFO_FILELIST = 0x60,	//读取指定类型的文件列表及校验信息
		INFO_PRODUCTION_REPORT = 0x61,	//生产报告
		INFO_RESET_REPORT = 0x62	//复位记录报告
	}
	SYS_INFO_TYPE;

	typedef struct _SYS_INFO
	{
		SYS_INFO_TYPE nType;

		union _Info
		{
			struct _SysID
			{
				DWORD32 IDLow;
				DWORD32 IDHigh;
			}
			SysID;

			struct _SysName
			{
				char* pbNameBuf;
				UINT nLen;
			}
			SysName;

			struct _SysVersion
			{
				DWORD32 dwVersion;
			}
			SysVersion;

			struct _ProductName
			{
				char* pbNameBuf;
				UINT nLen;
			}
			ProductName;

			struct _BootMode
			{
				DWORD32 dwMode;
				DWORD32 dwBootPrg;
			}
			BootMode;

			struct _FontMode
			{
				DWORD32 dwMode;
				DWORD32 dwModeCount;
			}
			FontMode;

			struct _FontModeList
			{
				int nMode;
				char* pbListBuf;
				UINT nLen;
			}
			FontModeList;

			struct _WorkMode
			{
				DWORD32 dwMode;
				DWORD32 dwModeCount;
			}
			WorkMode;

			struct _WorkModeList
			{
				int nMode;
				char* pbListBuf;
				UINT nLen;
			}
			WorkModeList;

			struct _ResetCount
			{
				DWORD32 dwCount;
			}
			ResetCount;

			struct _SysLed
			{
				DWORD32 dwFlag;
			}
			SysLed;

			struct _SysTime
			{
				DWORD32 dwTimeLow;
				DWORD32 dwTimeHigh;
			}
			SysTime;

			struct _SysPWD
			{
				DWORD32 dwType;
				BYTE8* pbPWDBuf;
				UINT nLen;
			}
			SysPWD;

			struct _FileList
			{
				DWORD32 dwFlag;
				DWORD32* pListBuf;
				UINT nLen;
			}
			FileList;

			struct _ProductReport
			{
				char* pbReport;
				UINT nLen;
			}
			ProductReport;

			struct _ResetReport
			{
				char* pbReport;
				UINT nLen;
			}
			ResetReport;
		}Info;
	}
	SYS_INFO;

	typedef enum _MEDIUM_TYPE
	{
		MEDIUM_UNKNOWN = 0x00,
		MEDIUM_FLASH = 0x01,
		MEDIUM_EEPROM = 0x02,
		MEDIUM_NETVIDEO = 0x03
	}
	MEDIUM_TYPE;

	typedef enum SYSIO_CMD
	{
		//文件系统相关 0x00~0x1f
		CMD_FILESYS_FORMAT = 0x00,
		CMD_FILESYS_PACK = 0x01,
		CMD_SET_FILEINFO = 0x02,
		CMD_GET_FILEINFO = 0x03,
		CMD_GET_FILESYSINFO = 0x04,
		CMD_CLEAR_TEST_INFO = 0x05,

		//数据输入输出
		CMD_PUT_FILE = 0x20,
		CMD_GET_FILE = 0x21,
		CMD_PUT_DAT = 0x22,
		CMD_GET_DAT = 0x23,
		CMD_GET_BUF =0x24
	}
	SYSIO_CMD;

	typedef struct _SYSIO_INFO
	{
		SYSIO_CMD nCmdID;
		MEDIUM_TYPE nMediumType;

		union _Info
		{
			struct _FILE_SYS_INFO
			{
				DWORD32 dwFileSysInfo;
			}
			FileSysInfo;

			struct _FILE_INFO
			{
				DWORD32 dwFlag;
				DWORD32 dwFileID;
				char* szFileInfo;
				DWORD32 dwFileVer;
				DWORD32 dwMaxLen;
			}
			FileInfo;

			struct _PUT_FILE_INFO
			{
				DWORD32 dwFileID;
				PVOID pFileDat;
				UINT nFileLen;
				DWORD32 dwFileVer;
				char* szFileInfo;
				DWORD32 dwFlag;
			}
			PutFileInfo;

			struct _GET_FILE_INFO
			{
				MEDIUM_TYPE nType;
				DWORD32 dwFileID;
				DWORD32 dwVer;
				PVOID pFileDatBuf;
				UINT nLen;
				DWORD32 dwFlag;
			}
			GetFileInfo;

			struct _PUT_DAT_INFO
			{
				DWORD32 dwAddr;
				PVOID pDatBuf;
				UINT nLen;
				DWORD32 dwFlag;
			}
			PutDatInfo;

			struct _GET_DAT_INFO
			{
				DWORD32 dwAddr;
				PVOID pDatBuf;
				UINT nLen;
				DWORD32 dwFlag;
			}
			GetDatInfo;
		}Info;
	}
	SYSIO_INFO;

	typedef enum _PARAM_APPEND
	{
		ADD_OVERWRITE = 0x00,	//覆盖原有项
		ADD_NEWONLY = 0x01,	 //只追加新项
		REPLACE_ALL = 0x02	//替换全部
	}
	PARAM_APPEND;

	typedef enum _PARAM_RESET
	{
		RESET_ALL = 0x00,

		KEEP_ADDR = 0x01,
		KEEP_SYS = 0x02,
		KEEP_HVDSP = 0x04,
		KEEP_TRACKER = 0x08,
		CLEAR_CFG_ALL = 0xFF
	}
	PARAM_RESET;

	typedef enum _PWD_TYPE
	{
		PWT_SETTING = 0x00,
		PWT_DEBUG = 0x01,
		PWT_MAINTAIN = 0x02
	}
	PWD_TYPE;

	interface ISysControl
	{
		//系统信息
		STDMETHOD(GetSysInfo)(
			SYS_INFO* pInfo
			) = 0;

		STDMETHOD(SetSysInfo)(
			SYS_INFO* pInfo
			) = 0;

		//地址信息
		STDMETHOD(GetTcpipAddr)(
			BYTE8 *pMac,
			DWORD32 *pdwIP,
			DWORD32 *pdwMask,
			DWORD32 *pdwGateway
			) = 0;

		STDMETHOD(SetTcpipAddr)(
			const BYTE8 *pMac,
			DWORD32 dwIP,
			DWORD32 dwMask,
			DWORD32 dwGateway
			) = 0;

		STDMETHOD(GetStdPtlParam)(
			BYTE8 *pucgMode,
			WORD16 *pwgLocalPort,
			WORD16 *pwRemotePort,
			DWORD32 *pdwRemoteAddr
			) = 0;

		STDMETHOD(SetStdPtlParam)(
			BYTE8 ucMode,
			WORD16 wLocalPort,
			WORD16 wRemotePort,
			DWORD32 dwRemoteAddr
			) = 0;

		STDMETHOD(GetMacAddr)(
			BYTE8* rgMac	//至少为6字节的地址缓存
			) = 0;

		//复位
		STDMETHOD(ResetHv)(
			DWORD32 dwFlag = 0
			) = 0;

		//文件系统操作
		STDMETHOD(SendIOCommand)(
			SYSIO_INFO* pInfo
			) = 0;

		//参数设置
		STDMETHOD(GetParamDat)(
			const char* szSection,
			const char* szKey,
			BYTE8* pbParamBuf,
			UINT* pBufLen
			) = 0;

		STDMETHOD(SetParamDat)(
			const char* szSection,
			const char* szKey,
			BYTE8* pbParamBuf,
			UINT nBufLen,
			DWORD32 dwMode
			) = 0;

		STDMETHOD(ResetParam)(
			DWORD32 dwMode
			) = 0;

		STDMETHOD(SetParam)(
			const char* szParamCmd,
			BYTE8* pRetBuf,
			UINT* pRetLen
			) = 0;

		// 读取Ini
		STDMETHOD(GetIniFile)(
			const char* szBuffer,
			UINT* pnBufLen
			) = 0;

		// 设置Ini
		STDMETHOD(SetIniFile)(
			const char* szBuffer,
			UINT nBufLen
			) = 0;

		// 写串口数据
		STDMETHOD(ComWriteData)(
			UINT nComPort,	// 表示当前操作的串口端口号.
			const PBYTE8 pbData,	// 指向所要写入的数据
			DWORD32 dwBytesToWrite,	// 期望写入的数据字节个数.
			PDWORD32 pdwBytesWrite	// 实际写入的数据字节个数,为NULL则不填.
			) = 0;

		// 读串口数据
		STDMETHOD(ComReadData)(
			UINT nComPort,	// 表示当前操作的串口端口号.
			PBYTE8 pbData,	// 指向读出的数据
			DWORD32 dwBytesToRead,	// 期望读入的字符数目.
			PDWORD32 pdwBytesRead	// 实际读到的字符长度,如果为NULL则不填.
			) = 0;

		// 获取连接IP信息
		STDMETHOD(GetConnectedIP)(
			DWORD32* pdwCount,
			DWORD32* pdwIP,
			DWORD32* pdwType
			) = 0;

		// 软触发
		STDMETHOD(ForceSend)(DWORD32 dwVideoID) = 0;

		STDMETHOD(AT88_InitConfig)(
			unsigned char* pbSecureCode,
			unsigned char* pbNc,
			unsigned char* pbReadOnlyData
			) = 0;

		STDMETHOD(At88_WriteFuse)(
			unsigned char* pbSecureCode
			) = 0;

		// 测试JPGE解压
		STDMETHOD(TestJpeg)(
			DWORD32* pdwJpegLen,
			unsigned char* pdwJpegBuf
			) = 0;

		//身份校验
		STDMETHOD(CheckIdentifyPassword)(
			char* pszPassword,
			BOOL &fSuccessed
			) = 0;

		// 上海交计特有协议
		STDMETHOD(GetJiaojiRecoDevCount)(
			DWORD32* pdwRecDevCount,
			DWORD32* pdwRecDevAddr
			) = 0;

		STDMETHOD(GetJiaojiRecoDevState)(
			DWORD32 dwRecDevAddr,
			DWORD32* pdwRecDevState
			) = 0;

		STDMETHOD(GetJiaojiResultData)(
			DWORD32 dwTimeLow,
			DWORD32 dwTimeHigh,
			DWORD32 dwIndex,
			char* pszPlateInfo,
			DWORD32* pdwSnapImageSize,
			PBYTE8 pbSnapImageData,
			DWORD32* pdwCaptureImageSize,
			PBYTE8 pbCaptureImageData
			) = 0;

		STDMETHOD(GetDetectInfoData)(
			DWORD32& dwTimeLow,
			DWORD32& dwTimeHigh,
			char* pszDetectInfo
			) = 0;
	};

	#define NOT_SEND_RESULT 0x1000

	typedef struct _HV_CMD_INFO
	{
		DWORD32 dwFlag;		//第一个字节做为类型标识
		DWORD32 dwCmdID;
		DWORD32 dwArgLen;
	}
	HV_CMD_INFO;

	typedef struct _HV_CMD_RESPOND
	{
		DWORD32 dwFlag;
		DWORD32 dwCmdID;
		int nResult;
		DWORD32 dwArgLen;
	}
	HV_CMD_RESPOND;

	interface ICmdDataLink
	{
		STDMETHOD(RecvCmdHeader)(
			HV_CMD_INFO *pInfo
			) = 0;
		STDMETHOD(SendRespond)(
			HV_CMD_RESPOND* pInfo
			) = 0;
		STDMETHOD(ReceiveData)(
			PVOID pbBuf,
			UINT nLen,
			UINT* pReceivedLen
			) = 0;
		STDMETHOD(SendData)(
			PVOID pbBuf,
			UINT nLen,
			UINT* pSendLen
			) = 0;
	};

	interface ICmdProcess
	{
		STDMETHOD(Process)(
			HV_CMD_INFO* pCmdInfo,
			ICmdDataLink* pCmdDataLink
			) = 0;
	};
}

#endif
