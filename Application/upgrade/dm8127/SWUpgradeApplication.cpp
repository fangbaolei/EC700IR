/**
*/
#include "SWFC.h"
#include "swpa.h"
#include "SWUpgradeApplication.h"
#include "Tester.h"
#include "SWTCPSocket.h"
#include "drv_device.h"
#include "SWNetOpt.h"
#include "CryptOpt.h"
#include "FlashOpt.h"


#include "config.h" //for version info


// 命令包头
typedef struct tag_CameraCmdHeader
{
    DWORD dwID;
    DWORD dwInfoSize;
}
CAMERA_CMD_HEADER;


// 响应包头
typedef struct tag_CameraResponseHeader
{
    DWORD dwID;
    DWORD dwInfoSize;
    INT      iReturn;
}
RESPONSE_CMD_HEADER;


//命令类型
#define CAMERA_XML_EXT_CMD        0x000F4000    //XML协议
#define UPGRADE_KERNEL_BAK_CMD    0xFFEE0001    //升级备份内核
#define UPGRADE_ROOTFS_BAK_CMD    0xFFEE0002    //升级备份文件系统
#define UPGRADE_WHOLE_PKT_CMD     0xFFEE0003    //升级多个内容
#define GET_UPGRADE_STATUS_CMD    0xFFEE0004    //获取升级状态
#define GET_DEV_INFO_CMD          0xFFEE0005    //获取设备信息
#define FORCE_UPGRADE_CMD         0xFFEE0006    //强制升级分区


#define IMAGE_TYPE_UPGRADE        "upgrade"        //限号包，需要解密
#define MAX_UPGRADE_PACKET_SIZE    (30*1024*1024)    //30MByte

#define DATA_MOUNT_POINT        "/mnt/nand"
#define HV_PARTITION_CRC_PATH        DATA_MOUNT_POINT    "/PartitionCrcFile"
#define HV_BAKSYS_VERSION_PATH       DATA_MOUNT_POINT    "/BakSysVersion"

typedef struct t_UpgradeInfo
{
    const CHAR *ImageName;        //升级包镜像名
    const CHAR *Partition;        //对应分区
    const CHAR *MountPoint;        //分区挂载点
    const INT iCouldUpgrade;    //可以升级,1-可以，0-不可以
    const BOOL fUbiFormat;        //是否为UBI镜像,1-是，0-不是
    const BOOL fDecrypt;        //否需要解密，1-需要，0-不需要
}tUpGradeInfo;

#define MAX_PARTITION_NUM    10

//分区名对应数组下标
typedef enum e_Partition
{
    I_U_BOOT_MIN = 0,
    I_U_BOOT,
    I_BOOT_ENV,
    I_KERNEL_BAK,
    I_ROOTFS_BAK,
    I_SW_APP_BAK,
    I_KERNEL,
    I_ROOTFS,
    I_SW_APP,
    I_NET_FW
    //I_DATA,
    //I_TEST
}E_PARTITION_INDEX;

//正常系统分区名、升级镜像名、挂载点，是否可升级对应关系
static tUpGradeInfo sImageMapNormalSys[MAX_PARTITION_NUM] = {
    /*镜像名称        分区号        挂载点        可否升级    是否UBI镜像  是否解密*/
    {"uboot_min",       "mtd0",        "",                1,            0,            0},
    {"uboot",           "mtd1",        "",                1,            0,            0},
    {"",                "mtd2",        "",                0,            0,            0},
    {"kernel_bak",      "mtd3",        "",                1,            0,            1},
    {"rootfs_bak",      "mtd4",        "",                1,            0,            1},
    {"app_bak",         "mtd5",        "",                1,            1,            1},
    {"kernel",          "mtd6",        "",                1,            0,            1},
    {"rootfs",          "mtd7",        "",                0,            0,            1},
    {"app",             "mtd8",        "/opt/ipnc",       0,            1,            1},
    {"net_fw",          "mtd9",        "/var/www/net",    1,            1,            1}
    //{"data",            "mtd9",        "/mnt/nand",       0,            1,            0}
    //{"test",            "mtd10",       "",                0,            0,            0}
};


//备份系统分区名、升级镜像名、挂载点，是否可升级对应关系
static tUpGradeInfo sImageMapBackupSys[MAX_PARTITION_NUM] = {
    /*镜像名称        分区号        挂载点        可否升级    是否UBI镜像  是否解密*/
    {"uboot_min",       "mtd0",        "",                1,            0,            0},
    {"uboot",           "mtd1",        "",                1,            0,            0},
    {"",                "mtd2",        "",                0,            0,            0},
    {"kernel_bak",      "mtd3",        "",                1,            0,            1},
    {"rootfs_bak",      "mtd4",        "",                0,            0,            1},
    {"app_bak",         "mtd5",        "",                0,            1,            1},
    {"kernel",          "mtd6",        "",                1,            0,            1},
    {"rootfs",          "mtd7",        "",                1,            0,            1},
    {"app",             "mtd8",        "/opt/ipnc",       1,            1,            1},
    {"net_fw",          "mtd9",        "/var/www/net",    1,            1,            1}
    //{"data",            "mtd9",        "/mnt/nand",       0,            1,            0}
    //{"test",            "mtd10",       "",                0,            0,            0}
};


static tUpGradeInfo* psImageMap = NULL;

#define PROCESS_CMD_NO_INPUT(CMD_ID, PROC_FNC)\
{\
    hr = PROC_FNC(&pvOutBuf, &dwOutSize);\
    sResponse.dwID = CMD_ID;\
    sResponse.dwInfoSize = dwOutSize;\
    sResponse.iReturn = FAILED(hr) ? -1 : 0;\
}

#define PROCESS_CMD_NO_OUTPUT(CMD_ID, PROC_FNC)\
{\
    DWORD dwSize = sHeader.dwInfoSize;\
    SW_TRACE_DEBUG("Info: Data size = %d!\n", dwSize);\
    if (0 < dwSize)\
    {            \
        DWORD dwLen = 0;\
        PBYTE pbBuf = NULL;\
        pbBuf = (PBYTE)swpa_mem_alloc(dwSize);\
        if (NULL == pbBuf)\
        {\
            SW_TRACE_DEBUG("Err: no memory for pbBuf!\n");\
            return E_OUTOFMEMORY;\
        }\
        if (FAILED(sockData.Read(pbBuf, dwSize, &dwLen))\
            || dwSize != dwLen)\
        {\
            SW_TRACE_DEBUG("Err: failed to read xml data!\n");\
            SAFE_MEM_FREE(pbBuf);\
            return E_FAIL;\
        }\
        SW_TRACE_DEBUG("Info: Received data size = %d!\n", dwLen);\
        hr = PROC_FNC(pbBuf, dwSize);\
        SAFE_MEM_FREE(pbBuf);\
    }\
    \
    sResponse.dwID = CMD_ID;\
    sResponse.dwInfoSize = dwOutSize;\
    sResponse.iReturn = FAILED(hr) ? -1 : 0;\
}


#define PROCESS_CMD_INOUT(CMD_ID, PROC_FNC)\
{\
    DWORD dwSize = sHeader.dwInfoSize;\
    SW_TRACE_DEBUG("Info: Data size = %d!\n", dwSize);\
    if (0 < dwSize)\
    {            \
        DWORD dwLen = 0;\
        PBYTE pbBuf = NULL;\
        pbBuf = (PBYTE)swpa_mem_alloc(dwSize);\
        if (NULL == pbBuf)\
        {\
            SW_TRACE_DEBUG("Err: no memory for pbBuf!\n");\
            return E_OUTOFMEMORY;\
        }\
        if (FAILED(sockData.Read(pbBuf, dwSize, &dwLen))\
            || dwSize != dwLen)\
        {\
            SW_TRACE_DEBUG("Err: failed to read xml data!\n");\
            SAFE_MEM_FREE(pbBuf);\
            return E_FAIL;\
        }\
        SW_TRACE_DEBUG("Info: Received data size = %d!\n", dwLen);\
        \
        hr = PROC_FNC(pbBuf, dwSize, &pvOutBuf, &dwOutSize);\
        SAFE_MEM_FREE(pbBuf);\
    }\
    \
    sResponse.dwID = CMD_ID;\
    sResponse.dwInfoSize = dwOutSize;\
    sResponse.iReturn = FAILED(hr) ? -1 : 0;\
}





CREATE_PROCESS(CSWUpgradeApp);


CSWUpgradeApp::CSWUpgradeApp()
    : m_fInited(FALSE)
    , m_wPort(9910)
    , m_pUpgradeThread(NULL)
    , m_pProbeThread(NULL)
    , m_pAutotest(NULL)
    , m_dwGETIP_COMMAND(0x00000101)
    , m_dwSETIP_COMMAND(0x00000102)
    , m_wProbePort(6666)
    , m_fIsUpgradeFailed(FALSE)
    , m_iUpgradeProgress(-1)
    , m_fGoingToExit(FALSE)
    , m_iExitCode(0)
    , m_fAutotestStarted(FALSE)
    , m_fUpgrading(FALSE)
    , m_fWriting(FALSE)
    , m_pFifo(NULL)
    , m_dwSingleImageLen(0)
    , m_fUbiImage(FALSE)
    , m_pbUpgradeImage(NULL)
    , m_dwImageLen(0)
    , m_dwMode(0)
    , m_dwDevType(0)
    , m_fSwitchingSystem(FALSE)
    , m_fForceUpgrade(FALSE)
    , m_fSupportNoBreakUpgrade(FALSE)
    
{
    swpa_memset(&m_cImageHeader, 0, sizeof(m_cImageHeader));
    swpa_memset(m_szBlockName, 0, sizeof(m_szBlockName));
}

CSWUpgradeApp::~CSWUpgradeApp()
{
    
}

HRESULT CSWUpgradeApp::ReleaseObj()
{
    if (!m_fInited)
    {
        return S_OK;
    }
    
    SW_TRACE_NORMAL("Info: releasing the app!!\n");


    if (NULL != m_pUpgradeThread)
    {
        delete m_pUpgradeThread;
        m_pUpgradeThread = NULL;
    }

    //停止设备侦测监听线程
    if (NULL != m_pProbeThread)
    {
        delete m_pProbeThread;
        m_pProbeThread = NULL;
    }
    
    if (IsBackupMode())
    {
        if (NULL != m_pAutotest)
        {
            if (m_fAutotestStarted)
                m_pAutotest->ReleaseObj();
            delete m_pAutotest;
            m_pAutotest = NULL;
        }
    }

    m_fInited = FALSE;

    return S_OK;
}

HRESULT CSWUpgradeApp::OnException(INT iSignalNo)
{
    SW_TRACE_NORMAL("Info: Got exception %d!!\n", iSignalNo);
    
    CSWApplication::OnException(iSignalNo);
    exit(0);
    
    return S_OK;
}

HRESULT CSWUpgradeApp::InitInstance(const WORD wArgc, const CHAR** szArgv)
{
    if(FAILED(CSWApplication::InitInstance(wArgc, szArgv)))
    {
        SW_TRACE_DEBUG("Err: failed to init CSWApplication\n");
        return E_FAIL;
    }

	CSWString strMode = GetCommandString("-mode", "normal");
	if (0 == swpa_stricmp((LPCSTR)strMode, "backup"))
	{
		m_dwMode = 1;//备份模式
	}

	SW_TRACE_NORMAL("Info: strMode = %s, m_dwMode = %d\n", (LPCSTR)strMode, m_dwMode);

	psImageMap = (IsBackupMode()) ? sImageMapBackupSys : sImageMapNormalSys;

    CSWString strDevType = GetCommandString("-devtype", "venus");
    if (0 == swpa_stricmp((LPCSTR)strDevType, "jupiter"))
    {
        m_dwDevType = DEVTYPE_JUPITER;// jupiter platform
    }
    else if (0 == swpa_stricmp((LPCSTR)strDevType, "saturn"))
    {
        m_dwDevType = DEVTYPE_SATURN;// saturn platform
    }
    else
    {
        m_dwDevType = DEVTYPE_VENUS;// venus platform
    }
    SW_TRACE_NORMAL("Info: strDevType = %s, m_dwDevType = %d\n", (LPCSTR)strDevType, m_dwDevType);

    if(FAILED(m_ProcessXMLCmd.Initialize(IsBackupMode(),m_dwDevType)))
    {
        SW_TRACE_NORMAL("Info: ProcessXMLCmd Initialize Fail.\n");
        return E_FAIL;
    }

    return Initialize();
}

HRESULT CSWUpgradeApp::ReleaseInstance()
{
    if(FAILED(CSWApplication::ReleaseInstance()))
    {
        SW_TRACE_DEBUG("Err: failed to release CSWApplication\n");
        return E_FAIL;
    }

    return ReleaseObj();
}

HRESULT CSWUpgradeApp::Run()
{    
    if (!m_fInited)
    {
        SW_TRACE_DEBUG("Err: not inited yet!\n");
        return E_NOTIMPL;
    }

    HeartBeat();

    SW_TRACE_NORMAL("Info: UpgradeAPP Version: %d\n", GetVersion());

	if (IsBackupMode())
	{
		//保存备份系统版本信息
		DWORD dwVersionNo = 0;
		if (FAILED(ReadBackupSystemVersion(&dwVersionNo))
			|| (DWORD)GetVersion() != dwVersionNo)
		{
			SW_TRACE_NORMAL("Info: update BakSys Version Info\n");
			if (FAILED(SaveBackupSystemVersion((DWORD)GetVersion())))
			{
				SW_TRACE_NORMAL("Err: failed to update BakSys Version Info\n");
			}
		}
		
        m_pAutotest->RestoreAutotest();
	}

    if (NULL == m_pProbeThread 
        || FAILED(m_pProbeThread->Start(OnProbeProxy, (VOID*)this)))
    {
        SW_TRACE_DEBUG("Err: failed to start probe thread\n");
        return E_FAIL;
    }
	
    CSWTCPSocket sockServer;
    if (FAILED(sockServer.Create()))
    {
        SW_TRACE_DEBUG("Err: failed to create pServer!\n");
        return E_FAIL;
    }

    while (FAILED(sockServer.Bind(NULL, m_wPort)))
    {
        SW_TRACE_DEBUG("Err: failed to bind Port %d, trying again... !\n", m_wPort);
        CSWApplication::Sleep(1000);
    }

    sockServer.SetRecvTimeout(4000);
    sockServer.SetSendTimeout(4000);
    
    if (FAILED(sockServer.Listen()))
    {
        SW_TRACE_DEBUG("Err: failed to listen!\n");
        return E_FAIL;
    }


	BOOL fSetBootCountFlag = TRUE;
	DWORD dwStartTick = CSWDateTime::GetSystemTick();
    while (!IsExited())
    {        
        if(FAILED(HeartBeat()))
        {
            SW_TRACE_DEBUG("Error: HeartBeat Fail.\n");
        }

        LEDBreath();

        SWPA_SOCKET_T outSock;

        if (SUCCEEDED(sockServer.Accept(outSock)))
        {
            ProcessNetCommand(outSock);
        }

		if (!m_fUpgrading
			&& !m_fSwitchingSystem)
		{
			DWORD dwTick = CSWDateTime::GetSystemTick();
			if (fSetBootCountFlag
				&& (dwTick > dwStartTick) && (dwTick - dwStartTick) > 30000)
			{
				if (IsBackupMode())
				{
                    if(SWPAR_OK == swpa_device_set_backup_bootcount(0))
                    {
                        SW_TRACE_NORMAL("info: set backup boot flag 0 ok.\n");
                    }
                    else
                    {
                        SW_TRACE_NORMAL("info: set backup boot flag 0 failed.\n");
                    }
				}
				
				// 设置启动标志
				if( SWPAR_OK == swpa_device_set_bootcount_flag(0) )
				{
                    SW_TRACE_NORMAL("info: set normal boot flag 0 ok.\n");
				}
				else
				{
                    SW_TRACE_NORMAL("Err: set normal boot flag 0 failed.\n");
				}

				fSetBootCountFlag = FALSE;
			}
		}
		else if (fSetBootCountFlag)
		{
			fSetBootCountFlag = FALSE;
		}
			
		
        if (m_fGoingToExit)
        {
            Exit(m_iExitCode);
            break;
        }

		CSWApplication::Sleep(100);
    }
    
    sockServer.Close();

    return S_OK;
}

/**
 *@brief 取得代码的版本号 
 *@return 返回版本号,失败返回-1
 */
INT CSWUpgradeApp::GetVersion(VOID)
{
#ifdef APP_VERSION
    return APP_VERSION;
#else
    return  -1;
#endif
}

HRESULT CSWUpgradeApp::Initialize()
{
    if (m_fInited)
    {
        return S_OK;
    }
    
    HeartBeat();

    SW_TRACE_NORMAL("Info: waiting CommandApp ready...\n");
    CSWApplication::Sleep(4000);

    HeartBeat();
    
    CheckResetMode();

    if (FAILED(IPConfig()))
    {
        SW_TRACE_DEBUG("Err: failed to config ip\n");
        return E_FAIL;
    }

    //开启设备侦测监听线程
    if (NULL == m_pProbeThread)
    {
        m_pProbeThread = new CSWThread();
        if (NULL == m_pProbeThread)
        {
            SW_TRACE_DEBUG("Err: no memory for m_pProbeThread\n");
            return E_OUTOFMEMORY;
        }
    }

    if (NULL == m_pUpgradeThread)
    {
        m_pUpgradeThread = new CSWThread();
        if (NULL == m_pUpgradeThread)
        {
            SW_TRACE_DEBUG("Err: no memory for m_pUpgradeThread\n");
            return E_OUTOFMEMORY;
        }        
    }

    if (IsBackupMode() && NULL == m_pAutotest)
    {
        if (m_dwDevType == DEVTYPE_JUPITER) // jupiter
            m_pAutotest = new CAutotestJupiter();
        else
            m_pAutotest = new CAutotest();

        if (NULL == m_pAutotest)
        {
            SW_TRACE_DEBUG("Err: no memory for m_pAutotest");
            return E_OUTOFMEMORY;
        }
    }

    m_fInited = TRUE;
    
    return S_OK;
}

HRESULT CSWUpgradeApp::CheckResetMode()
{
	if (!IsBackupMode())
	{
		INT iMode = 2;
	    if (SWPAR_OK != swpa_device_get_resetmode(&iMode))
	    {
	        SW_TRACE_DEBUG("Err: failed to get reset_mode, set it to Upgrade Mode!\n");
	        if (SWPAR_OK != swpa_device_set_resetmode(2))
	        {
	            SW_TRACE_DEBUG("Err: failed to set working_mode to Upgrade Mode!\n");
	            return E_FAIL;
	        }
	    }
	    else if (0 == iMode)
	    {
	        SW_TRACE_DEBUG("Info: Resetmode is Normal Mode, switch to Normal Mode!\n");
	        Exit(1);//switch to Normal Mode
	        return E_FAIL;
	    }
	}

    return S_OK;

}

HRESULT CSWUpgradeApp::IPConfig()
{
    CHAR szIP[32] = {0};
    CHAR szMask[32] = {0};
    CHAR szGateway[32] = {0};

	if (IsBackupMode())
	{
	    if (SWPAR_OK != swpa_utils_shell("ifconfig eth0 100.100.100.101 netmask 255.0.0.0"))
	    {
	        SW_TRACE_DEBUG("Err: failed to SET default ip (100.100.100.101) info to system!\n");
	        //return E_FAIL;
	    }
	}
    
    if (SWPAR_OK != swpa_device_read_ipinfo(szIP, sizeof(szIP)-1, szMask, sizeof(szMask)-1, szGateway, sizeof(szGateway)-1))
    {
        SW_TRACE_NORMAL("Err: failed to GET ipinfo from device!\n");
        return E_NOTIMPL;
    }

    SW_TRACE_NORMAL("Info: from EEPROM: IP = %s, Mask = %s, Gateway = %s!\n", szIP, szMask, szGateway);

    if (SWPAR_OK != swpa_tcpip_setinfo("eth0", szIP, szMask, szGateway))
    {
        SW_TRACE_NORMAL("Err: failed to SET ipinfo to system!\n");
		CHAR szCmd[256] = {0};
		swpa_snprintf(szCmd, sizeof(szCmd), "ifconfig eth0 %s netmask %s", szIP, szMask);
        if (SWPAR_OK != swpa_utils_shell(szCmd, NULL))
        {
            SW_TRACE_NORMAL("Err: failed to SET default ip (%s) info to system!\n", szIP);
            //return E_FAIL;
        }
    }
	else
	{
		SW_TRACE_DEBUG("Info: SET ipinfo to system -- OK!\n");
	}

    return S_OK;
}

HRESULT CSWUpgradeApp::LEDBreath()
{
    static BOOL fLedOn = TRUE;

    if (fLedOn)
    {
        swpa_alarm_led_on();
        swpa_heart_led_on();
    }
    else
    {
        swpa_alarm_led_off();
        swpa_heart_led_off();
    }

    fLedOn = !fLedOn;

    return S_OK;
}

HRESULT CSWUpgradeApp::ProcessNetCommand(SWPA_SOCKET_T outSock)
{
    if (0 == outSock)
    {
        SW_TRACE_DEBUG("Err: outSock(%d) is invalid.\n", outSock);
        return E_INVALIDARG;
    }
    
    SW_TRACE_NORMAL("Info: Cmd socket got a connection...\n");
    
    
    DWORD dwLen = 0;
    CAMERA_CMD_HEADER    sHeader;
    HRESULT hr = S_OK;

    CSWTCPSocket sockData;
    sockData.Attach(outSock);
    sockData.SetRecvTimeout(10000);
    if (FAILED(sockData.Read(&sHeader, sizeof(sHeader), &dwLen))
        || sizeof(sHeader) != dwLen)
    {
        SW_TRACE_DEBUG("Err: failed to read command header!\n");
        return E_FAIL;
    }            

    RESPONSE_CMD_HEADER sResponse;
    PVOID pvOutBuf = NULL;
    DWORD dwOutSize = 0;
    
    switch (sHeader.dwID) 
    {
    	case UPGRADE_WHOLE_PKT_CMD:    // Upgrade the whole package        
        {
            SW_TRACE_NORMAL("Info: Got Upgrade Package cmd!\n");

            if (!m_fUpgrading)
            {
				// 设置启动标志
				if( SWPAR_OK == swpa_device_set_bootcount_flag(1) )
				{
					SW_TRACE_DEBUG("info: set boot flag 1 ok.\n");
				}
				else
				{
					SW_TRACE_DEBUG("Err: set boot flag 1 failed.\n");
				}

				m_fSupportNoBreakUpgrade = TRUE; //打开该标记
                m_ProcessXMLCmd.SetSupportNoBreakUpgrade(m_fSupportNoBreakUpgrade);
                m_fIsUpgradeFailed = FALSE;	//收到升级命令，先设置升级失败为FALSE,否则立即查询的话可能会有问题
                if (sHeader.dwInfoSize < MAX_UPGRADE_PACKET_SIZE)
                {
                	//小包可以全部接收下来完，校验后写入
                    SW_TRACE_DEBUG("Info: Upgrade with Tradition Type. Recv All Data Before Write Flash!");                	
                    
                    if (FAILED(UpgradeImage(sHeader.dwInfoSize, sockData)))
                    {
                        SW_TRACE_NORMAL("Err: Upgrade Image failed!\n");
						hr = E_FAIL;
                    }
                }
                else
                {
                	//大包由于没有足够内存，只能接收一部分数据之后马上写入flash,数据接收放到其他线程
                    SW_TRACE_DEBUG("Info: Upgrade Packet is too large,So Writing Flash While Reading Data!");

					sockData.Detach();
					m_sockData.Attach(outSock);
					m_sockData.SetRecvTimeout(10000);
					UpgradeLargeImage(sHeader.dwInfoSize);
                    return S_OK;	//立即返回，应答放到升级线程中
                }
            }
            else
            {
            	//正在升级时，不允许接收升级包
                SW_TRACE_NORMAL("Err: Upgrading...Cannot accept another package!\n");
                hr = E_FAIL;
            }

            sResponse.dwID = UPGRADE_WHOLE_PKT_CMD;
            sResponse.dwInfoSize = 0;
            sResponse.iReturn = FAILED(hr) ? -1 : 0;

        	break;
        }
        case CAMERA_XML_EXT_CMD:// XML cmd
        {
            SW_TRACE_NORMAL("Info: Got XML cmd!\n");
            PROCESS_CMD_INOUT(CAMERA_XML_EXT_CMD, ProcessXmlCmd);
            SW_TRACE_NORMAL("Info: XML cmd processing returns 0x%x!\n", hr);

        	break;
        }
        case CAMERA_PROTOCOL_COMMAND: //获取XML协议版本
        {
            SW_TRACE_NORMAL("Info: Got XML Protocol cmd!\n");
            sResponse.dwID = CAMERA_PROTOCOL_COMMAND;
            sResponse.dwInfoSize = 0;
            sResponse.iReturn = PROTOCOL_MERCURY;

        	break;
        }
    	case GET_UPGRADE_STATUS_CMD:    // get Upgrade status            
        {
            SW_TRACE_DEBUG("Info: Got GetUpgradeStatus cmd!\n");        
            PROCESS_CMD_NO_INPUT(0xFFEE0004, GetUpgradeImageStatus);
            if (FAILED(hr))
            {
                SW_TRACE_NORMAL("Err: GetUpgradeStatus cmd processing returns 0x%x!\n", hr);
            }

        	break;
        }
        case GET_DEV_INFO_CMD:   // get DeviceInfo         
        {
            SW_TRACE_NORMAL("Info: Got GetDeviceInfo cmd!\n");
            PROCESS_CMD_NO_INPUT(0xFFEE0005, GetDeviceInfo);
            SW_TRACE_NORMAL("Info: GetDeviceInfo cmd processing returns 0x%x!\n", hr);

        	break;
        }
		case FORCE_UPGRADE_CMD:   // force upgrade
        {
            SW_TRACE_NORMAL("Info: Got ForceUpgrade cmd!\n");
            PROCESS_CMD_NO_OUTPUT(0xFFEE0006, OnForceUpgrade);
            SW_TRACE_NORMAL("Info: ForceUpgrade cmd processing returns 0x%x!\n", hr);

        	break;
        }
        default:
        {
            SW_TRACE_NORMAL("Info: Got unsupported cmd : 0x%x, ignore it!\n", sHeader.dwID);
            return S_OK;
        }
    }

    
    //SW_TRACE_DEBUG("Info: Sending response data...\n");
	
    if (FAILED(sockData.Send(&sResponse, sizeof(sResponse), &dwLen))
        || sizeof(sResponse) != dwLen)
    {
        SW_TRACE_DEBUG("Err: failed to send response msg!\n");
        return E_FAIL;
    }

    if (0 != sResponse.dwInfoSize && NULL != pvOutBuf && 0 != dwOutSize)
    {
        if (FAILED(sockData.Send(pvOutBuf, dwOutSize, &dwLen))
            || dwOutSize != dwLen)
        {
            SW_TRACE_DEBUG("Err: failed to send response data!\n");

            SAFE_MEM_FREE(pvOutBuf);
            dwOutSize = 0;
            return E_FAIL;
        }

        SAFE_MEM_FREE(pvOutBuf);
        dwOutSize = 0;
    }
    
    SW_TRACE_DEBUG("Info: Sent response data -- OK (len = %d)\n", dwLen);

    return S_OK;
}

//读取分区的CRC，用于判断是否需要升级
HRESULT CSWUpgradeApp::ReadPartitionCrc(const CHAR *szPartition,DWORD &dwCrc)
{
    if ( 0 == swpa_strcmp(szPartition, "mtd0") //uboot-min
        || 0 == swpa_strcmp(szPartition, "mtd1") //uboot
        || 0 == swpa_strcmp(szPartition, "mtd3") //kernel-bak
        || 0 == swpa_strcmp(szPartition, "mtd4") //rootfs-bak
        || 0 == swpa_strcmp(szPartition, "mtd6") //kernel
        || 0 == swpa_strcmp(szPartition, "mtd7") //rootfs
        )
    {
        SW_TRACE_DEBUG("Info: force upgrade %s\n", szPartition);
        dwCrc = 0xFFFFFFFF;
        return S_OK;
    }
    
    CHAR szFileName[64];
    sprintf(szFileName,"%s/%s_CRC.data",HV_PARTITION_CRC_PATH,szPartition);
    if (0 != access(szFileName,/*R_OK | W_OK |*/ F_OK))    //第一次升级没有CRC文件，可认为需要升级
    {
        SW_TRACE_DEBUG("Info: Crc file %s Not Exist\n",szFileName);
        dwCrc = 0xFFFFFFFF;
        return S_OK;
    }

    FILE *fp = fopen(szFileName,"r");
    if (NULL == fp)                
    {
        SW_TRACE_DEBUG("Err: fopen %s failed\n",szFileName);
        return E_FAIL;
    }
    DWORD dwCrcTemp;
    int iReadCnt = fread((CHAR *)&dwCrcTemp,1,sizeof(dwCrcTemp),fp);
    if(iReadCnt != sizeof(dwCrcTemp))
    {
        SW_TRACE_DEBUG("Err: failed to read %s\n",szFileName);
        fclose(fp);
        return E_FAIL;
    }

    fclose(fp);

    dwCrc = dwCrcTemp;

    return S_OK;
}

//保存分区的CRC,升级完成后保存
HRESULT CSWUpgradeApp::SavePartitionCrc(const CHAR *szPartition,const DWORD dwCrc)
{
    CHAR szFileName[64];

    //目录不存在，创建
    if (0 != access(HV_PARTITION_CRC_PATH,/*R_OK | W_OK| */ F_OK))
    {
        if (0 != mkdir(HV_PARTITION_CRC_PATH,0755)) 
        {
            SW_TRACE_DEBUG("Err: mkdir %s failed\n",HV_PARTITION_CRC_PATH);
            return E_FAIL;
        }
    }

    sprintf(szFileName,"%s/%s_CRC.data",HV_PARTITION_CRC_PATH,szPartition);

    FILE *fp = fopen(szFileName,"wb");
    if (NULL == fp)                //第一次升级没有CRC文件，可认为需要升级
    {
        SW_TRACE_DEBUG("Err: fopen %s failed\n",szFileName);
        return E_FAIL;
    }

    DWORD dwCrcTemp = dwCrc;
    int iWriteCnt = fwrite((CHAR *)&dwCrcTemp,1,sizeof(dwCrcTemp),fp);
    if (iWriteCnt != sizeof(dwCrcTemp))
    {
        SW_TRACE_DEBUG("Err: Write Crc to %s failed\n",szFileName);
        fclose(fp);
        return E_FAIL;
    }
    //fflush(fp);

    fclose(fp);

    sync();

    SW_TRACE_DEBUG("Info: Save Crc 0x%x To File %s Successd!",dwCrc,szFileName);

    return S_OK;
}

BOOL CSWUpgradeApp::CheckAllPacketCouldUpgrade(const CHAR* pbData, const DWORD dwCount)
{
    if ( NULL == pbData || dwCount <= 0 )
    {
        SW_TRACE_DEBUG("Err: NULL == pbData || dwCount <= 0\n");
        return E_FAIL;
    }

    BYTE* pbUpgradePacket = (BYTE*)pbData;

    for (int i=0; i<dwCount; i++)
    {
        //提取镜像头
        IMAGE_PACKAGE_HEADER sHeaderForWrite;
        swpa_memcpy(&sHeaderForWrite, pbUpgradePacket, sizeof(sHeaderForWrite));
    
        pbUpgradePacket += sizeof(sHeaderForWrite);

		E_PARTITION_INDEX eMapIndex = I_ROOTFS;
		if (IsBackupMode())//备份模式下
		{
			E_PARTITION_INDEX eMapIndex = I_ROOTFS_BAK;
		}

        //SW_TRACE_DEBUG("strcmp <%s> and <%s>",(CHAR *)sHeaderForWrite.ih_name,psImageMap[eMapIndex].ImageName);
        //文件系统和内核分主、备，不能自己升级自己
        if (0 == swpa_strcmp((CHAR *)sHeaderForWrite.ih_name,psImageMap[eMapIndex].ImageName))
        {
            SW_TRACE_DEBUG("Err:Upgrade failed! Please change mode or remove"
                "image [%s] form upgrade packet\n",(CHAR *)sHeaderForWrite.ih_name);
            return FALSE;
        }

        DWORD dwImageSize = MyNetToHostLong(sHeaderForWrite.ih_size);
        pbUpgradePacket += dwImageSize;
        //镜像部件数据要求4字节对齐
        if ( (dwImageSize%4) != 0 )
        {
            pbUpgradePacket += (4 - (dwImageSize%4));
        }

    }
    return TRUE;
}

HRESULT CSWUpgradeApp::UMountPartition(const CHAR *szImageName)
{
    //system("echo 3 > /proc/sys/vm/drop_caches");

    if ( swpa_strcmp(szImageName, psImageMap[I_SW_APP].ImageName) == 0 )
    {
        CHAR szCmd[32];
        sprintf(szCmd,"umount -l %s",psImageMap[I_SW_APP].MountPoint);
        if (FAILED(swpa_utils_shell(szCmd, 0)))
        {
            SW_TRACE_DEBUG("Err: Umount app path %s failed\n",psImageMap[I_SW_APP].MountPoint);
            //return E_FAIL;
        }
        //detach ubi
        sprintf(szCmd,"ubidetach /dev/ubi_ctrl -m %d",I_SW_APP);    //mtd7
        if (FAILED(swpa_utils_shell(szCmd, 0)))
        {
            SW_TRACE_DEBUG("Err: Detach app Ubifs Failed!\n");
            //return E_FAIL;
        }
        
    }
    else if ( swpa_strcmp(szImageName, psImageMap[I_NET_FW].ImageName) == 0 )
    {
        CHAR szCmd[32];
        sprintf(szCmd,"umount -l %s",psImageMap[I_NET_FW].MountPoint);
        if (FAILED(swpa_utils_shell(szCmd, 0)))
        {
            SW_TRACE_DEBUG("Err: Umount net_fw path %s failed\n",psImageMap[I_NET_FW].MountPoint);
            //return E_FAIL;
        }

        //detach ubi
        sprintf(szCmd,"ubidetach /dev/ubi_ctrl -m %d",I_NET_FW);//mtd8
        if (FAILED(swpa_utils_shell(szCmd, 0)))
        {
            SW_TRACE_DEBUG("Err: Detach Net Framework Ubifs Failed!\n");
            //return E_FAIL;
        }
    }
    else 
    {
        SW_TRACE_DEBUG("Info: Upgrade %s umount nothing\n",szImageName);
    }

    return S_OK;
}

//是否需要升级、升级到哪个分区
BOOL CSWUpgradeApp::NeedUpgradeBlock(const CHAR* szName, 
    const DWORD dwNowCrc,CHAR *pBlockName,PBOOL pUbiImage,PBOOL pDecrypt)
{
    for (int i=0; i<MAX_PARTITION_NUM; i++)
    {
        const tUpGradeInfo *pUpGradeInfo = &psImageMap[i];
        //SW_TRACE_DEBUG("strcmp <%s> and <%s>",szName,pUpGradeInfo->ImageName);
        if ( swpa_strcmp(szName,pUpGradeInfo->ImageName) == 0 && 1 == pUpGradeInfo->iCouldUpgrade)
        {
            swpa_strcpy(pBlockName,pUpGradeInfo->Partition);
            *pUbiImage = pUpGradeInfo->fUbiFormat;
            *pDecrypt = pUpGradeInfo->fDecrypt;

			if (m_fForceUpgrade)
			{
				SW_TRACE_DEBUG("Info: force upgrade %s\n", szName);
				return TRUE;
			}
            
            DWORD dwCRC;
            if (FAILED(ReadPartitionCrc(pUpGradeInfo->Partition,dwCRC)))
            {
                SW_TRACE_DEBUG("Info: Read Crc File Failed! Upgrade Force\n");
                return TRUE;
            }

            SW_TRACE_DEBUG("Info: Partition Crc = %x, NowCrc = %x\n",dwCRC,dwNowCrc);
            if (dwCRC != dwNowCrc)
            {
                SW_TRACE_DEBUG("Info: %s needs upgrade!\n",pUpGradeInfo->ImageName);
                return TRUE;
            }

            break;        //找到，退出循环
        }
    }
	
    SW_TRACE_DEBUG("Info: %s do not needs upgrade!\n",szName);

    return FALSE;
}

//将镜像写入分区
HRESULT CSWUpgradeApp::WritePackageToBlock(const PVOID pvHeader, 
    const PBYTE pbData, const DWORD dwSize,const CHAR *szBlockName,BOOL fUbiImage)
{
    IMAGE_PACKAGE_HEADER cHeader;
    swpa_memcpy(&cHeader, pvHeader, sizeof(cHeader));
	
    SW_TRACE_DEBUG("Info: Upgrading %s Partition...!\n",szBlockName);

    // umount some path
    if (FAILED(UMountPartition((const CHAR *)cHeader.ih_name)))
    {
        SW_TRACE_DEBUG("Warn: Umount path failed\n");
        //return E_FAIL;
    }

    //1.clear old crc file
    DWORD dwZeroCrc = 0xFFFFFFFF;
    if (FAILED(SavePartitionCrc(szBlockName,dwZeroCrc)))
    {
        SW_TRACE_DEBUG("Err: Clear Crc File %s Failed\n",szBlockName);
        return E_FAIL;
    }

    //2.write data
    INT iRet = 0;
    if (fUbiImage)
    {
        iRet = CFlashOpt::WriteUbiImage(szBlockName,pbData,dwSize);
    }
    else
    {
        iRet = CFlashOpt::WriteBinImage(szBlockName,pbData,dwSize);
		//iRet = CFlashOpt::WriteBinImage(szBlockName,pbData,dwSize); //todo: temp solution
    }
    if (iRet != 0)
    {
        SW_TRACE_DEBUG("Err: Upgrade %s Partition...NG!\n",szBlockName);
        return E_FAIL;
    }

    //3.updata crc file    
    if (FAILED(SavePartitionCrc(szBlockName,cHeader.ih_dcrc)))
    {
        SW_TRACE_DEBUG("Err: Updata Crc File %s Failed\n",szBlockName);
        return E_FAIL;
    }

    SW_TRACE_DEBUG("Info: Upgrade %s Success!\n",(CHAR *)cHeader.ih_name);
    sync();//todo: replace this

    return S_OK;
}

HRESULT CSWUpgradeApp::OnWriteImage()
{
    if (0 == m_dwSingleImageLen)
    {
        SW_TRACE_DEBUG("Err: 0 == m_dwSingleImageLen\n");
        return E_FAIL;
    }
    if (FAILED(WritePackageToBlock(&m_cImageHeader,NULL,m_dwSingleImageLen,m_szBlockName,m_fUbiImage)))
    {
        SW_TRACE_DEBUG("Err: Write Package to block failed!\n");
        return E_FAIL;
    }
    return S_OK;
}

PVOID CSWUpgradeApp::OnWriteImageProxy(PVOID pvArg)
{
    if (NULL == pvArg)    //没有传参的话无法释放任何资源
    {
        return (PVOID)E_INVALIDARG;
    }
    SW_TRACE_DEBUG("Info: Start Write Image Thread OK!");
    CSWUpgradeApp* pThis = (CSWUpgradeApp*)pvArg;
    pThis->m_fWriting = TRUE;
    HRESULT hr = pThis->OnWriteImage();
    pThis->m_fWriting = FALSE;

    return (PVOID)hr;
}

INT CSWUpgradeApp::GetDataFromFifo(void *buf,unsigned int unLen,void *lp)
{
    CSWUpgradeApp *pThis = (CSWUpgradeApp *)lp;
    if (NULL == pThis->m_pFifo)
    {
        return -1;
    }

    return pThis->m_pFifo->Get(buf,unLen);
}

HRESULT CSWUpgradeApp::UpgradeSingleImage(DWORD &dwImageSize,BOOL fMacDecrypt,
	PBYTE pMac,ULONG ulDecryptPos)
{
    IMAGE_PACKAGE_HEADER sHeaderForWrite;
    IMAGE_PACKAGE_HEADER cHeader;
    
    swpa_memset(&cHeader,0x00,sizeof(cHeader));
    ULONG ulMacDecryptPos = ulDecryptPos;
    
    DWORD dwReadLen = sizeof(cHeader);
    if (FAILED(m_sockData.Read(&cHeader,sizeof(cHeader), &dwReadLen))
        || dwReadLen != sizeof(cHeader))
    {
        SW_TRACE_DEBUG("Err: Read Image Header Failed!\n");
        return E_FAIL;
    }
    
    if (fMacDecrypt)
    {
        DecryptPackageByMAC((PBYTE)&cHeader,dwReadLen,pMac,&ulMacDecryptPos);
    }
        
    DWORD dwOriginalDataCRC = cHeader.ih_dcrc;
    swpa_memcpy(&sHeaderForWrite,&cHeader,sizeof(sHeaderForWrite));
    cHeader.ih_size = MyNetToHostLong(cHeader.ih_size);
    cHeader.ih_dcrc = MyNetToHostLong(cHeader.ih_dcrc);

    SW_TRACE_DEBUG("Info: ih_name = [%s] [%d] type:[%d]\n", cHeader.ih_name, cHeader.ih_size, cHeader.ih_type);

    if (cHeader.ih_size > MAX_UPGRADE_PACKET_SIZE)//> 30M
    {
        SW_TRACE_DEBUG("Info: %s Image is Too Large,So Write Flash while Recv Net Data!\n",cHeader.ih_name);

        //判断该文件是否需要升级,及升级分区
        CHAR szBlockName[32];
        BOOL fUbiImage = FALSE;
        BOOL fDecrypt = FALSE;
        //不需要升级，接收完数据
        if (!NeedUpgradeBlock((CHAR *)cHeader.ih_name,dwOriginalDataCRC,szBlockName,&fUbiImage,&fDecrypt))
        {
            SW_TRACE_DEBUG("Info: No Need To Upgrade Flash! Recv Net Data and Do Nothing!\n");
            DWORD dwReadImage = cHeader.ih_size;
            while(dwReadImage > 0)
            {
                dwReadLen = 4096;
                DWORD dwNeed = dwReadImage > 4096 ? 4096 : dwReadImage;
                CHAR Buf[4096];
                if (FAILED(m_sockData.Read(Buf, dwNeed, &dwReadLen))
                    || dwReadLen != dwNeed)
                {
                    SW_TRACE_DEBUG("Err: Read Image Data Failed!\n");
                    return E_FAIL;
                }

                dwReadImage -= dwReadLen;
            }
            return S_OK;
        }

		CSWThread cWriteFlashThread;

        m_pFifo = new CSWFifo();
        if (NULL == m_pFifo)
        {
            SW_TRACE_DEBUG("Err: new ring buffer failed!");
            return E_FAIL;
        }

        if (0 != m_pFifo->SetBufSize(10*1024*1024))//10M
        {
            SW_TRACE_DEBUG("Err: set bufsize failed!\n");
            SAFE_DELETE(m_pFifo);
            return E_FAIL;
        }

        swpa_memcpy(&m_cImageHeader,&sHeaderForWrite,sizeof(m_cImageHeader));
        m_dwSingleImageLen = cHeader.ih_size;
        swpa_strcpy(m_szBlockName,szBlockName);
        m_fUbiImage = fUbiImage;
            
        CFlashOpt::RegCallBackFunc(GetDataFromFifo,this);

        cWriteFlashThread.Start(OnWriteImageProxy,this);
        
        DWORD dwReadImage = cHeader.ih_size;
        unsigned long ulPos = 0;
        while (dwReadImage > 0)
        {
            dwReadLen = 4096;
            DWORD dwNeed = dwReadImage > 4096 ? 4096 : dwReadImage;
            CHAR Buf[4096];
            if (FAILED(m_sockData.Read(Buf,dwNeed, &dwReadLen))
                || dwReadLen != dwNeed)
            {
                SW_TRACE_DEBUG("Err: Read Image Data Failed!\n");
                goto WaitWriteThread;
            }
            if (fMacDecrypt)
            {
                DecryptPackageByMAC((PBYTE)Buf,dwReadLen,pMac,&ulMacDecryptPos);
            }
            
            dwReadImage -= dwNeed;

            if (fDecrypt)
            {
                CCryptOpt::SimpleDecryptData((PBYTE)Buf,dwNeed,&ulPos);
            }
            
            if(m_pFifo->Push(Buf,dwNeed) < 0)
            {
                SW_TRACE_DEBUG("Push data to Fifo Failed!\n");
                goto WaitWriteThread;
            }
        }

        SW_TRACE_DEBUG("Info: Recv Net Data Finish,Wait Write Flash Finish!\n");
        
        while (m_fWriting)
        {
            CSWApplication::Sleep(1000);
        }
        cWriteFlashThread.Stop();

        m_pFifo->DestroyFifo();
        SAFE_DELETE(m_pFifo);
        SW_TRACE_DEBUG("Info: Write %s Finished!\n",cHeader.ih_name);
    }
    else
    {
        BYTE *pbUpgradePacket = (BYTE *)swpa_mem_alloc(cHeader.ih_size);
        if (NULL == pbUpgradePacket)
        {
            SW_TRACE_DEBUG("Err: Alloc mem Failed!\n");
            return E_OUTOFMEMORY;
        }

        dwReadLen = cHeader.ih_size;

        if (FAILED(m_sockData.Read(pbUpgradePacket, cHeader.ih_size, &dwReadLen))
            || dwReadLen != cHeader.ih_size)
        {
            SW_TRACE_DEBUG("Err: Read Image Data Failed!\n");
            SAFE_MEM_FREE(pbUpgradePacket);
            return E_FAIL;
        }
        if (fMacDecrypt)
    	{
            DecryptPackageByMAC(pbUpgradePacket,cHeader.ih_size,pMac,&ulMacDecryptPos);
    	}
		
        //check crc
        //确保镜像部件数据的正确性
        if ( cHeader.ih_dcrc != CSWUtils::CalcCrc32(0, (BYTE*)pbUpgradePacket, cHeader.ih_size) ) 
        {
            SW_TRACE_DEBUG("Err: %s CRC Check failed\n", cHeader.ih_name);
            SAFE_MEM_FREE(pbUpgradePacket);
            return E_FAIL;
        }

        CHAR szBlockName[32];
        BOOL fUbiImage = FALSE;
        BOOL fDecrypt = FALSE;
        //判断该文件是否需要升级,及升级分区
        if (NeedUpgradeBlock((CHAR *)cHeader.ih_name,dwOriginalDataCRC,szBlockName,&fUbiImage,&fDecrypt))
        {
            if (fDecrypt)    //uboot和uboot_min不需要解密
            {
                SW_TRACE_DEBUG("Info: Decrypt %s Image data ...!\n",cHeader.ih_name);
                unsigned long ulPos = 0;
                CCryptOpt::SimpleDecryptData(pbUpgradePacket,cHeader.ih_size,&ulPos);
            }

            //四字节对齐？
            if (FAILED(WritePackageToBlock(&sHeaderForWrite,pbUpgradePacket,cHeader.ih_size,szBlockName,fUbiImage)))
            {
                SW_TRACE_DEBUG("Err: failed to upgrade %s\n", cHeader.ih_name );
                SAFE_MEM_FREE(pbUpgradePacket);
                return E_FAIL;
            }    
        }

        SAFE_MEM_FREE(pbUpgradePacket);
    }    
    
    dwImageSize = cHeader.ih_size;

    //dwUpgradedSize += cHeader.ih_size;

    //m_iUpgradeProgress = dwUpgradedSize * 100/ dwTotalSize;

    return S_OK;

WaitWriteThread:
    SW_TRACE_DEBUG("Info: Stop Recv Upgrade Data,and wait write flash exit!\n");
    m_pFifo->SetGetTimeOutMs(0);
    while (m_fWriting)
    {
        CSWApplication::Sleep(1000);
    }

    m_pFifo->DestroyFifo();
    SAFE_DELETE(m_pFifo);

    return E_FAIL;
}

HRESULT CSWUpgradeApp::OnUpgradeLargeImage()
{
    HRESULT hr = S_OK;
    DWORD dwUpgradedSize = 0;
    IMAGE_PACKAGE_HEADER cHeader;
    swpa_memset(&cHeader,0x0,sizeof(cHeader));

    DWORD dwReadLen = sizeof(cHeader);
    CHAR cTemp[64];
    INT rgiFileSize[16];
    DWORD dwCount = 0;
    DWORD dwTotalSize = 0;
    INT iRet = 0;
    BOOL fMacDecrypt = FALSE;
    BYTE bMac[6] = {0};
	ULONG ulPos = 0;
	
    if (!m_fInited)
    {
        SW_TRACE_NORMAL("Err: Not Inited !!!!\n");
        hr = E_NOTIMPL;
        goto ErrExit;
    }
	
    m_iUpgradeProgress = 0;
    
    SW_TRACE_NORMAL("Info: Upgrading Image ...\n");

    //1.recv total head data
    if (FAILED(m_sockData.Read(&cHeader, sizeof(cHeader), &dwReadLen))
        || dwReadLen != sizeof(cHeader))
    {
        SW_TRACE_NORMAL("Err: Read Total Header Failed!\n");
        hr = E_FAIL;
        goto ErrExit;
    }

    if (0 == swpa_strcmp(IMAGE_TYPE_UPGRADE, (const CHAR*)cHeader.ih_name))
    {
        fMacDecrypt = TRUE;
        CHAR szIP[32] = {0};
		CHAR szNetMask[32] = {0};
		CHAR szGateway[32] = {0};
		CHAR szMAC[32] = {0};
        DWORD dwMAC[6] = {0};
		
		DWORD dwNetID = 0;
		CHAR szEthernet[8] = {0};
		do { 	   
			swpa_sprintf(szEthernet, "eth%d", dwNetID);
			iRet = swpa_tcpip_getinfo(szEthernet, szIP, 32, szNetMask, 32, szGateway, 32, szMAC, 32);
			dwNetID++;
		} while ( SWPAR_OK != iRet && 5 > dwNetID );

		if (5 <= dwNetID)
		{
			SW_TRACE_DEBUG("Err: failed to get net info! (eth0~eth%d)\n", dwNetID);
			return E_FAIL;
		}

        swpa_sscanf(szMAC, "%x:%x:%x:%x:%x:%x", &dwMAC[0], &dwMAC[1], &dwMAC[2], &dwMAC[3], &dwMAC[4], &dwMAC[5]);
        for (DWORD i=0; i<sizeof(bMac); i++)
        {
            bMac[i] = dwMAC[i] & 0xFF;
        }
		INT iImageSize[2] = {0};
		if (FAILED(m_sockData.Read(iImageSize, sizeof(iImageSize), &dwReadLen))
			|| dwReadLen != sizeof(iImageSize))
		{
			SW_TRACE_NORMAL("Err: Read Image Size Failed!\n");
			hr = E_FAIL;
			goto ErrExit;
		}

		INT iEncryptedSize = 0;
        swpa_memcpy(&iEncryptedSize, &iImageSize[0], sizeof(INT));
        iEncryptedSize = MyNetToHostLong(iEncryptedSize);
		SW_TRACE_DEBUG("Info:EncryptedSize = %d\n",iEncryptedSize);

        INT iFlag = 0;
        swpa_memcpy(&iFlag, &iImageSize[1], sizeof(INT));
        iFlag = MyNetToHostLong(iFlag);

        if (0 != iFlag)
        {
            //只支持一个加密包，因此这4个字节需为0
            SW_TRACE_DEBUG("Err: 0 != iFlag\n");
            return E_FAIL; 
        }

		//加密包总包头
		if (FAILED(m_sockData.Read(&cHeader, sizeof(cHeader), &dwReadLen))
        	|| dwReadLen != sizeof(cHeader))
    	{
        	SW_TRACE_NORMAL("Err: Read Total Header Failed!!\n");
        	hr = E_FAIL;
        	goto ErrExit;
    	}
        
        DecryptPackageByMAC((BYTE*)&cHeader, sizeof(cHeader), bMac, &ulPos);
    }

    cHeader.ih_magic = MyNetToHostLong(cHeader.ih_magic);
    //cHeader.ih_hcrc = MyNetToHostLong(cHeader.ih_hcrc);
    cHeader.ih_size = MyNetToHostLong(cHeader.ih_size);
    cHeader.ih_dcrc = MyNetToHostLong(cHeader.ih_dcrc);

    if ( cHeader.ih_type != IH_TYPE_MULTI )
    {
        SW_TRACE_NORMAL("Err: cHeader.ih_type != IH_TYPE_MULTI\n");
        hr = E_FAIL;
        goto ErrExit;
    }
    
    SW_TRACE_NORMAL("Info: ih_name = [%s] [%d] type:[%d]\n", cHeader.ih_name, cHeader.ih_size, cHeader.ih_type);
    //check total crc 
    //分开接收的话无法做总数据的CRC校验了

    //2.for image len
    for (dwCount = 0; dwCount < 16; ++dwCount)
    {
        dwReadLen = sizeof(INT);
        if (FAILED(m_sockData.Read(&(rgiFileSize[dwCount]), sizeof(INT), &dwReadLen))
            || dwReadLen != sizeof(INT))
        {
            SW_TRACE_NORMAL("Err: Read Image Len Data Failed!\n");
            hr = E_FAIL;
            goto ErrExit;
        }
		
		if (fMacDecrypt)
		{
        	DecryptPackageByMAC((BYTE*)&(rgiFileSize[dwCount]), sizeof(INT), bMac, &ulPos);
			SW_TRACE_DEBUG("count %d file size 0x%x pos %d\n",
				dwCount,rgiFileSize[dwCount],ulPos);
		}
        rgiFileSize[dwCount] = MyNetToHostLong(rgiFileSize[dwCount]);

        SW_TRACE_DEBUG("Info: Image Len Data fileSize = %d",rgiFileSize[dwCount]);

        if ( 0 == rgiFileSize[dwCount] )
        {
            break;
        }
        
        dwTotalSize += rgiFileSize[dwCount];

    }

    //4.for recv image head and recv image data write to fifo
    for (INT n=0; n<dwCount; ++n)
    {
        DWORD dwImageLen = 0;
        if (FAILED(UpgradeSingleImage(dwImageLen,fMacDecrypt,bMac,ulPos)))
        {
            SW_TRACE_NORMAL("Err: Upgrade %dth Image Failed!\n",n);
            hr = E_FAIL;
            goto ErrExit;
        }
    
        dwUpgradedSize += dwImageLen;
		ulPos += dwImageLen;

        m_iUpgradeProgress = dwUpgradedSize * 100 / dwTotalSize;
    }

ErrExit:

    RESPONSE_CMD_HEADER sResponse;
    sResponse.dwID = UPGRADE_WHOLE_PKT_CMD;
    sResponse.dwInfoSize = 0;
    sResponse.iReturn = FAILED(hr) ? -1 : 0;
    m_iUpgradeProgress = FAILED(hr) ? -1 : 100; 
    m_fIsUpgradeFailed = FAILED(hr) ? TRUE : FALSE;		//升级失败

    if (sizeof(sResponse) != m_sockData.Send(&sResponse,sizeof(sResponse)))    //接收完数据，应答
    {
        SW_TRACE_NORMAL("Err: failed to send response msg!\n");
    }

    SW_TRACE_NORMAL("Info: Send Upgrade Cmd Response Success!\n");
    m_sockData.Close();
	
	m_pUpgradeThread->Stop();

    return hr;
}

PVOID CSWUpgradeApp::OnUpgradeLargeImageProxy(PVOID pvArg)
{
    if (NULL == pvArg)    //没有传参的话无法释放任何资源
    {
        return (PVOID)E_INVALIDARG;
    }

    CSWUpgradeApp* pThis = (CSWUpgradeApp*)pvArg;

    HRESULT hr = pThis->OnUpgradeLargeImage();
    pThis->m_fUpgrading = FALSE;

    return (PVOID)hr;
}

HRESULT CSWUpgradeApp::UpgradeLargeImage(const DWORD dwSize)
{
    //create thread to recv net data and fun return immediately!
    if (0 == dwSize)
    {
        SW_TRACE_NORMAL("Err: 0 == dwSize\n");
        return E_INVALIDARG;
    }

	//m_dwImageLen = dwSize;

    if (NULL == m_pUpgradeThread)
    {
        SW_TRACE_NORMAL("Err: NULL == m_pUpgradeThread\n");
        return E_NOTIMPL;
    }

	if (!m_pUpgradeThread->IsValid())
    {
    	return E_ACCESSDENIED;
    }

    HRESULT hr = m_pUpgradeThread->Start(OnUpgradeLargeImageProxy, this);
    if (FAILED(hr))
    {
        SW_TRACE_NORMAL("Err: Start Thread Failed!\n");
    }
    else
    {
        m_fUpgrading = TRUE;
    }

    return S_OK;
}

HRESULT CSWUpgradeApp::GenerateProbeReplyXml(CHAR ** ppszXmlBuf, DWORD *pdwXmlLen)
{
#define HV_XML_CMD_VERSION_NO "3.0"
    
    
#define HVXML_VER                     "Ver"
#define HVXML_HVCMD                    "HvCmd"
#define HVXML_HVCMDRESPOND             "HvCmdRespond"
#define HVXML_RETCODE                 "RetCode"
#define HVXML_RETMSG                 "RetMsg"
#define HVXML_CMDNAME                "CmdName"

    const DWORD _INFO_LEN = 256;
    const DWORD _MSG_LEN = _INFO_LEN * 2;
    HRESULT hr = S_OK;
    INT    iRet = SWPAR_OK;
    //CHAR szMsg[_MSG_LEN] = {0};
    CHAR szSN[_INFO_LEN] = {0};
    CHAR szMode[_INFO_LEN] = {0};//{0};
    DWORD dwLen = _INFO_LEN;

    CHAR * pszXmlData = NULL;
    TiXmlDocument * pXmlOutputDoc = NULL;
    TiXmlDeclaration *pDeclaration = NULL;
    TiXmlElement *pRootEle = NULL;
    TiXmlElement *pReplyEle = NULL;    
    TiXmlPrinter * pXmlPrinter = NULL; 
    TiXmlText *pReplyText = NULL;
    
    CHAR szIP[32] = {0};
    CHAR szNetMask[32] = {0};
    CHAR szMAC[32] = {0};
    CHAR szGateway[32] = {0};


    if (NULL == ppszXmlBuf || NULL == pdwXmlLen)
    {
        SW_TRACE_DEBUG("Err: NULL == ppszXmlBuf || NULL == pdwXmlLen\n");
        return E_INVALIDARG;
    }

    //CAMERA_CMD_RESPOND sCmdHeader;
    //swpa_memset(&sCmdHeader, 0, sizeof(sCmdHeader));
    
    
    DWORD dwNetID = 0;
    CHAR szEthernet[8] = {0};
    do {        
        swpa_sprintf(szEthernet, "eth%d", dwNetID);
        iRet = swpa_tcpip_getinfo(szEthernet, szIP, 32, szNetMask, 32, szGateway, 32, szMAC, 32);
        dwNetID++;
    } while ( SWPAR_OK != iRet && 5 > dwNetID );

    if (5 <= dwNetID)
    {
        SW_TRACE_NORMAL("Err: failed to get net info! (eth0~eth%d)\n", dwNetID);
        return E_FAIL;
    }

    iRet = swpa_device_read_sn(szSN, &dwLen);
    if (SWPAR_OK != iRet)
    {
        SW_TRACE_NORMAL("Err: failed to get device serial number\n");
        return E_FAIL;
    }

    //SW_TRACE_DEBUG("Info: Probe sn = %s\n", szSN);

	if (IsBackupMode())
	{
		swpa_snprintf(szMode, sizeof(szMode)-1, "%s", "生产模式");    
	}
	else
	{
		INT iMode = 0;
	    iRet = swpa_device_get_resetmode(&iMode);
	    if (SWPAR_OK != iRet)
	    {
	        SW_TRACE_NORMAL("Err: failed to get device working mode\n");
	        //todo: return E_FAIL;
	    }
		swpa_snprintf(szMode, sizeof(szMode)-1, "%s", 0 == iMode ? "正常模式" : "升级模式");    
	}


	CHAR szDevName[255];
	hr = ReadCustomizedDevName(szDevName);
	if (E_NOTIMPL == hr)
	{
		swpa_strcpy(szDevName, "Unnamed");
		SaveCustomizedDevName("Unnamed");
	}
	else if (FAILED(hr)) //other errors
	{
		SW_TRACE_NORMAL("Err: failed to get customized dev name\n");
		swpa_strcpy(szDevName, "Get Fail");
		return E_FAIL;
	}
    
    //swpa_snprintf(szMsg, _MSG_LEN - 1, "IP:%s;MAC:%s;SN:%s;MODE:%s;", szIP, szMAC, szSN, szMode);

    pXmlOutputDoc = new TiXmlDocument();
    if (NULL == pXmlOutputDoc)
    {
        SW_TRACE_DEBUG("Err: no memory for pXmlOutputDoc\n");
        hr = E_OUTOFMEMORY;
        goto OUT;
    }    

    pDeclaration = new TiXmlDeclaration("1.0","GB2312","yes");
    if (NULL == pDeclaration)
    {
        SW_TRACE_DEBUG("Err: no memory for pDeclaration\n");
        hr = E_OUTOFMEMORY;
        goto OUT;
    }
    pXmlOutputDoc->LinkEndChild(pDeclaration);

    pRootEle = new TiXmlElement(HVXML_HVCMDRESPOND);
    if (NULL == pRootEle)
    {
        SW_TRACE_DEBUG("Err: no memory for pRootEle\n");
        hr = E_OUTOFMEMORY;
        goto OUT;
    }
    pRootEle->SetAttribute(HVXML_VER, HV_XML_CMD_VERSION_NO);
    

    pReplyEle = new TiXmlElement(HVXML_CMDNAME);
    if (NULL == pReplyEle)
    {
        SW_TRACE_DEBUG("Err: no memory for pReplyEle\n");
        hr = E_OUTOFMEMORY;
        goto OUT;
    }

    pReplyEle->SetAttribute("IP", szIP);
    pReplyEle->SetAttribute("Mask", szNetMask);
    pReplyEle->SetAttribute("Gateway", szGateway);
    pReplyEle->SetAttribute("MAC", szMAC);
    pReplyEle->SetAttribute("SN", szSN);
    pReplyEle->SetAttribute("Mode", szMode);
    if (m_dwDevType == DEVTYPE_JUPITER)
    {
        pReplyEle->SetAttribute("DevType", "SDC200");
    }
    else
    {
        pReplyEle->SetAttribute("DevType", "Venus");
    }
	pReplyEle->SetAttribute("Version", GetVersion());
    //pReplyEle->SetAttribute("DevType", "Unable to fetch");//金星设备分200W和600W，在备份系统无法确定
    pReplyEle->SetAttribute("Remark", szDevName);
	
    pReplyEle->SetAttribute(HVXML_RETCODE, 0);
    pReplyEle->SetAttribute(HVXML_RETMSG, "OK");

    pReplyText = new TiXmlText("Probe");  
    if (NULL == pReplyText)
    {
        SW_TRACE_DEBUG("Err: no memory for pReplyText\n");
        hr = E_OUTOFMEMORY;
        goto OUT;
    }
    pReplyEle->LinkEndChild(pReplyText);
    
    pRootEle->LinkEndChild(pReplyEle);
    
    pXmlOutputDoc->LinkEndChild(pRootEle);

    pXmlPrinter = new TiXmlPrinter();
    if (NULL == pXmlPrinter)
    {
        SW_TRACE_DEBUG("Err: no memory for pXmlPrinter\n");
        hr = E_OUTOFMEMORY;
        goto OUT;
    }
    
    pXmlOutputDoc->Accept(pXmlPrinter);
    
    pszXmlData = (CHAR*)swpa_mem_alloc(pXmlPrinter->Size() /* + sizeof(sCmdHeader)*/);
    if (NULL == pszXmlData)
    {
        SW_TRACE_DEBUG("Err: no memory for pszXmlData\n");
        hr = E_OUTOFMEMORY;
        goto OUT;
    }

    //sCmdHeader.dwID = CAMERA_XML_EXT_CMD;
    //sCmdHeader.dwInfoSize = pXmlPrinter->Size();
    //sCmdHeader.iResult = 0;
    
    swpa_memset(pszXmlData, 0x0, pXmlPrinter->Size() /*+ sizeof(sCmdHeader)*/);
    
    //swpa_memcpy(pszXmlData, &sCmdHeader, sizeof(sCmdHeader));
    swpa_memcpy(pszXmlData/*+sizeof(sCmdHeader)*/, pXmlPrinter->CStr(), pXmlPrinter->Size());

    *ppszXmlBuf = pszXmlData;
    *pdwXmlLen = pXmlPrinter->Size() /*+sizeof(sCmdHeader)*/;
         
OUT:

    SAFE_DELETE(pXmlPrinter);
    SAFE_DELETE(pXmlOutputDoc);

    return hr;
}

HRESULT CSWUpgradeApp::ParseProbeXmlMsg(CHAR * pszMsg)
{
    if (NULL == pszMsg)
    {
        SW_TRACE_DEBUG("Err: NULL == pszMsg\n");
        return E_INVALIDARG;
    }
    
    TiXmlDocument  XmlDoc;

    XmlDoc.Parse(pszMsg);

    TiXmlElement * pEleRoot = XmlDoc.RootElement();
    if(NULL != pEleRoot)
    {
        for (TiXmlElement *Ele = pEleRoot->FirstChildElement(); NULL != Ele; Ele = Ele->NextSiblingElement())
        {            
            if (NULL != Ele->GetText()
                && 0 == swpa_stricmp("Probe", Ele->GetText()))
            {
                return S_OK;
            }
        }
    }

    return E_FAIL;
}

HRESULT CSWUpgradeApp::OnForceUpgrade(const PVOID pvBuf, const DWORD dwSize)
{
	if (NULL == pvBuf || 1 != dwSize)
    {
        return E_INVALIDARG;
    }

	m_fForceUpgrade = *(BOOL*)pvBuf ? TRUE : FALSE;

	return S_OK;
}

HRESULT CSWUpgradeApp::GetDeviceInfo(PVOID* ppvOutBuf, DWORD* pdwOutLen)
{
    if (NULL == ppvOutBuf || NULL == pdwOutLen)
    {
        return E_INVALIDARG;
    }

    *ppvOutBuf = NULL;
    *pdwOutLen = 0;

    INT iRet = SWPAR_OK;    
    CHAR szIP[32] = {0};
    CHAR szMask[32] = {0};
    CHAR szGateway[32] = {0};
    CHAR szMAC[32] = {0};
    
    BYTE bMsg[256] = {0};

    DWORD dwIP[4] = {0};
    DWORD dwMask[4] = {0};
    DWORD dwGateway[4] = {0};
    DWORD dwMAC[6] = {0};

    CHAR szSN[256] = {0};
    CHAR szMode[128] = {0};
    DWORD dwLen = 0;

    DWORD dwNetID = 0;
    CHAR szEthernet[8] = {0};
    do {        
        swpa_sprintf(szEthernet, "eth%d", dwNetID);
        iRet = swpa_tcpip_getinfo(szEthernet, szIP, 32, szMask, 32, szGateway, 32, szMAC, 32);
        dwNetID++;
    } while ( SWPAR_OK != iRet && 5 > dwNetID );

    if (5 <= dwNetID)
    {
        SW_TRACE_DEBUG("Err: failed to get net info! (eth0~eth%d)\n", dwNetID);
        return E_FAIL;
    }
    
    
    SW_TRACE_DEBUG("Info: got ip info...\n");
    SW_TRACE_DEBUG("Info: szIP: %s; szMask: %s; szMAC: %s; szGateway: %s\n", szIP,  szMask,  szMAC, szGateway);
    
    swpa_sscanf(szIP, "%d.%d.%d.%d", &dwIP[0], &dwIP[1], &dwIP[2], &dwIP[3]);
    swpa_sscanf(szMask, "%d.%d.%d.%d", &dwMask[0], &dwMask[1], &dwMask[2], &dwMask[3]);
    swpa_sscanf(szGateway, "%d.%d.%d.%d", &dwGateway[0], &dwGateway[1], &dwGateway[2], &dwGateway[3]);
    swpa_sscanf(szMAC, "%x:%x:%x:%x:%x:%x", &dwMAC[0], &dwMAC[1], &dwMAC[2], &dwMAC[3], &dwMAC[4], &dwMAC[5]);

    DWORD dwIPVal = 0;
    DWORD dwMaskVal = 0;
    DWORD dwGatewayVal = 0;
    BYTE bMACVal[6] = {0};
    //DWORD dwIPVal = 0;
    dwIPVal = dwIP[0]<<24 | dwIP[1]<<16 | dwIP[2]<<8 | dwIP[3];    
    dwMaskVal = dwMask[0]<<24 | dwMask[1]<<16 | dwMask[2]<<8 | dwMask[3];
    dwGatewayVal = dwGateway[0]<<24 | dwGateway[1]<<16 | dwGateway[2]<<8 | dwGateway[3];
    for (DWORD i=0; i<sizeof(bMACVal); i++)
    {
        bMACVal[i] = dwMAC[i] & 0xFF;
        //SW_TRACE_DEBUG("Info: bMACVal[i] = %#x\n", bMACVal[i]);
    }

    DWORD dwCmd = 0xFFEE0005;
    swpa_memcpy(&bMsg[0], &dwCmd, 4);
    swpa_memcpy(&bMsg[4], bMACVal, 6);
    swpa_memcpy(&bMsg[10], &dwIPVal, 4);
    swpa_memcpy(&bMsg[14], &dwMaskVal, 4);
    swpa_memcpy(&bMsg[18], &dwGatewayVal, 4);
        
    dwLen = sizeof(szSN);
    iRet = swpa_device_read_sn(szSN, &dwLen);
    if (SWPAR_OK != iRet)
    {
        SW_TRACE_DEBUG("Err: failed to get device serial number\n");
        return E_FAIL;
    }

    if (dwLen > 0 && dwLen < 32)
    {
        swpa_memcpy(&bMsg[22], szSN, dwLen);
        *pdwOutLen = 22+dwLen;
    }
    else
    {
        SW_TRACE_DEBUG("Err: Got invalid SN from EEPROM\n");
        return E_FAIL;
    }
    
    *ppvOutBuf = swpa_mem_alloc(*pdwOutLen);
    if (NULL == *ppvOutBuf)
    {
        SW_TRACE_DEBUG("Err: no memory for *pszReplyPack\n");
        return E_OUTOFMEMORY;
    }
    swpa_memset(*ppvOutBuf, 0, *pdwOutLen);
    swpa_memcpy(*ppvOutBuf, bMsg, *pdwOutLen);
    
    return S_OK;
}

HRESULT CSWUpgradeApp::GenerateSetIPCMDReplay(const BYTE* pbMsg, const DWORD dwMsgLen, BYTE** pbReplyPack, DWORD* pdwPackLen)
{
    
    if (NULL == pbMsg || NULL == pbReplyPack || NULL == pdwPackLen)
    {
        return E_INVALIDARG;
    }

    INT iRet = SWPAR_OK;    
    CHAR szIP[32] = {0};
    CHAR szMask[32] = {0};
    CHAR szGateway[32] = {0};
    CHAR szMAC[32] = {0};
    
    BYTE bMsg[256] = {0};

    BYTE bIP[4] = {0};
    BYTE bMask[4] = {0};
    BYTE bGateway[4] = {0};
    BYTE bMAC[6] = {0};
    BYTE bThisMAC[6] = {0};
    DWORD dwThisMAC[6] = {0};

    CHAR szSN[128] = {0};
    CHAR szMode[128] = {0};
    INT iLen = 0;

    DWORD dwNetID = 0;
    CHAR szEthernet[8] = {0};
    do {        
        swpa_sprintf(szEthernet, "eth%d", dwNetID);
        iRet = swpa_tcpip_getinfo(szEthernet, szIP, 32, szMask, 32, szGateway, 32, szMAC, 32);
        dwNetID++;
    } while ( SWPAR_OK != iRet && 5 > dwNetID );

    if (5 <= dwNetID)
    {
        SW_TRACE_DEBUG("Err: failed to get net info! (eth0~eth%d)\n", dwNetID);
        return E_FAIL;
    }
    
    swpa_sscanf(szMAC, "%x:%x:%x:%x:%x:%x", &dwThisMAC[0], &dwThisMAC[1], &dwThisMAC[2], &dwThisMAC[3], &dwThisMAC[4], &dwThisMAC[5]);
    for (DWORD i=0; i<sizeof(bThisMAC); i++)
    {
        bThisMAC[i] = dwThisMAC[i] & 0xFF;
    }
    swpa_memcpy(bMAC, &pbMsg[4], 6);

    if (0 == swpa_memcmp(bMAC, bThisMAC, sizeof(bMAC)))
    {
        swpa_memcpy(bIP, &pbMsg[10], 4);
        swpa_memcpy(bMask, &pbMsg[14], 4);
        swpa_memcpy(bGateway, &pbMsg[18], 4);

        swpa_snprintf(szIP, sizeof(szIP)-1, "%d.%d.%d.%d", bIP[3], bIP[2], bIP[1], bIP[0]);
        swpa_snprintf(szMask, sizeof(szMask)-1, "%d.%d.%d.%d", bMask[3], bMask[2], bMask[1], bMask[0]);
        swpa_snprintf(szGateway, sizeof(szGateway)-1, "%d.%d.%d.%d", bGateway[3], bGateway[2], bGateway[1], bGateway[0]);
        
        SW_TRACE_DEBUG("Info: szIP = %s\n", szIP);
        SW_TRACE_DEBUG("Info: szMask = %s\n", szMask);
        SW_TRACE_DEBUG("Info: szGateway = %s\n", szGateway);

		if (SWPAR_OK != swpa_tcpip_checkinfo(szIP, szMask, szGateway))
		{
			SW_TRACE_DEBUG("Err: check ip info failed\n");
            return E_FAIL;
		}

		iRet = swpa_device_write_ipinfo(szIP, szMask, szGateway);
        if (SWPAR_OK != iRet)
        {    
            SW_TRACE_DEBUG("Err: failed to save net info\n");
            return E_FAIL;
        }

        *pdwPackLen = 8;
        *pbReplyPack = (BYTE*)swpa_mem_alloc(*pdwPackLen);
        if (NULL == *pbReplyPack)
        {
            SW_TRACE_DEBUG("Err: no memory for *pszReplyPack\n");
            return E_OUTOFMEMORY;
        }
        swpa_memset(*pbReplyPack, 0, *pdwPackLen);
        swpa_memcpy(*pbReplyPack, &m_dwSETIP_COMMAND, sizeof(m_dwSETIP_COMMAND));

		return S_OK;
    }


    return E_INVALIDARG;    
}

HRESULT CSWUpgradeApp::OnProbe(VOID)
{
    HRESULT hr = S_OK;

    if (!m_fInited)
    {
        SW_TRACE_DEBUG("Err: not inited yet\n");
        return E_NOTIMPL;
    }    

    CSWUDPSocket cUDPSock;
    if (FAILED(cUDPSock.Create()))
    {
        SW_TRACE_DEBUG("Err: failed to create UDPSock\n");
        return E_FAIL;
    }

    while (FAILED(cUDPSock.Bind(NULL, m_wProbePort)))
    {
        SW_TRACE_DEBUG("Warning: udpsock failed to bind to Port #%d\n", m_wProbePort);
        CSWApplication::Sleep(1000);
    }

    cUDPSock.SetRecvTimeout(4000);
    cUDPSock.SetSendTimeout(4000);

    SW_TRACE_NORMAL("Info: Probe Thread listening Port #%d\n", m_wProbePort);
    while (!IsExited())
    {
        HeartBeat();	//temp solution, keep upgrade alive when receiving huge upgrade package

        BYTE bMsgIn[512] = {0};
        DWORD dwRecvLen = 0;
        DWORD dwSendLen = 0;
		BOOL  fNeedReboot = FALSE;

        hr = cUDPSock.RecvFrom((VOID*)bMsgIn, sizeof(bMsgIn), &dwRecvLen);
        if (FAILED(hr) || 0 >= dwRecvLen)
        {            
            continue;
        }

        SW_TRACE_DEBUG("Info: dwRecvLen = %d\n", dwRecvLen);
        
        BYTE* pbReplyPack = NULL;
        DWORD dwPackLen = 0;
        if (sizeof(DWORD) == dwRecvLen && (0 == swpa_memcmp(&bMsgIn[0], &m_dwGETIP_COMMAND, sizeof(m_dwGETIP_COMMAND))))
        {
            SW_TRACE_NORMAL("Info: Got GetIp CMD\n");
            
            if (FAILED(GenerateProbeReplyXml((CHAR**)&pbReplyPack, &dwPackLen)))
            {
                SW_TRACE_DEBUG("Err: failed to generate GetIP reply xml\n");
                
                SAFE_MEM_FREE(pbReplyPack);
                dwPackLen = 0;
                continue;
            }

            SW_TRACE_NORMAL("Info: GetIp -- OK\n");
        }
        else if (22 == dwRecvLen && 0 == swpa_memcmp(&bMsgIn[0], &m_dwSETIP_COMMAND, sizeof(m_dwSETIP_COMMAND)))
        {
            SW_TRACE_NORMAL("Info: Got SetIp CMD\n");
            
            if (FAILED(GenerateSetIPCMDReplay(bMsgIn, dwRecvLen, &pbReplyPack, &dwPackLen)))
            {
                SW_TRACE_DEBUG("Err: failed to generate SetIP Reply Msg\n");
                SAFE_MEM_FREE(pbReplyPack);
                dwPackLen = 0;
                continue;
            }

            SW_TRACE_NORMAL("Info: SetIp -- OK\n");
			fNeedReboot = TRUE; // reboot automatically 
        }
        else
        {
            //SW_TRACE_NORMAL("Err: discards unknown cmd\n");
            continue;
        }

        if (NULL != pbReplyPack && 0 != dwPackLen)
        {
            WORD wDstPort = 0;
            cUDPSock.GetPeerName(NULL, &wDstPort);
			//网络内设备过多时，随机休眠一定时间后再回应，可以减小网络广播压力
			swpa_utils_srand(CSWDateTime::GetSystemTick());
			swpa_thread_sleep_ms(swpa_utils_rand()%800);
            if (FAILED(cUDPSock.Broadcast(wDstPort, pbReplyPack, dwPackLen)) )
            {
                SW_TRACE_NORMAL("Err: Broadcast response -- FAILED\n");
                
                SAFE_MEM_FREE(pbReplyPack);
                dwPackLen = 0;
                continue;                
            }

            SW_TRACE_NORMAL("Info: Broadcast response -- OK\n");

            SAFE_MEM_FREE(pbReplyPack);
            dwPackLen = 0;
        }

		if (fNeedReboot)
		{
			Exit(2);
		}
    }

    return hr;
}

VOID* CSWUpgradeApp::OnProbeProxy(VOID* pvParam)
{
    if (NULL == pvParam)
    {
        return (VOID*)E_INVALIDARG;
    }

    CSWUpgradeApp * pThis = (CSWUpgradeApp *)pvParam;

    return (VOID*)pThis->OnProbe();
}

/*该函数在水星NorFlash上使用，金星及之后产品NandFlash上不再使用*/
HRESULT CSWUpgradeApp::UpgradeRootfsBak(const PVOID pvBuf, const DWORD dwSize)
{
    if (NULL == pvBuf || 0 == dwSize)
    {
        SW_TRACE_DEBUG("Err: NULL == pvBuf || 0 == dwSize\n");
        return E_INVALIDARG;
    }

    SW_TRACE_NORMAL("Info: Upgrading RootfsBak...\n");
    
    CSWFile sFile;

    if (FAILED(sFile.Open("FLASH/0/ROOTFS_BAK", "w")))
    {
        SW_TRACE_DEBUG("Err: failed to open %s\n", "FLASH/0/ROOTFS_BAK");
        return E_FAIL;
    }

    DWORD dwWrittenLen = 0;
    if (FAILED(sFile.Write(pvBuf, dwSize, &dwWrittenLen)) || dwSize != dwWrittenLen)
    {
        SW_TRACE_DEBUG("Err: failed to write %s\n", "FLASH/0/ROOTFS_BAK");
        return E_FAIL;
    }

    SW_TRACE_NORMAL("Info: Upgrading RootfsBak -- OK\n");
    
    return sFile.Close();
}

/*该函数在水星NorFlash上使用，金星及之后产品NandFlash上不再使用*/
HRESULT CSWUpgradeApp::UpgradeKernelBak(const PVOID pvBuf, const DWORD dwSize)
{
    if (NULL == pvBuf || 0 == dwSize)
    {
        SW_TRACE_DEBUG("Err: NULL == pvBuf || 0 == dwSize\n");
        return E_INVALIDARG;
    }

    SW_TRACE_NORMAL("Info: Upgrading KernelBak...\n");
    
    CSWFile sFile;

    if (FAILED(sFile.Open("FLASH/0/KERNEL_BAK", "w")))
    {
        SW_TRACE_DEBUG("Err: failed to open %s\n", "FLASH/0/KERNEL_BAK");
        return E_FAIL;
    }

    DWORD dwWrittenLen = 0;
    if (FAILED(sFile.Write(pvBuf, dwSize, &dwWrittenLen)) || dwSize != dwWrittenLen)
    {
        SW_TRACE_DEBUG("Err: failed to write %s\n", "FLASH/0/KERNEL_BAK");
        return E_FAIL;
    }

    SW_TRACE_NORMAL("Info: Upgrading KernelBak -- OK\n");
    
    return sFile.Close();
}

HRESULT CSWUpgradeApp::OnUpgradeImage()
{
    if (!m_fInited)
    {
        return E_NOTIMPL;
    }

    if (NULL == m_pbUpgradeImage || 0 == m_dwImageLen)
    {
        return E_INVALIDARG;
    }
    
    SW_TRACE_NORMAL("Info: Upgrading Image ...\n");

    HRESULT hr = WriteUpgradePacket((const CHAR*)m_pbUpgradeImage, m_dwImageLen);
    if (FAILED(hr))
    {
        SW_TRACE_DEBUG("Err: failed to Write Upgrade Image\n");
    }
	else
	{
		SW_TRACE_NORMAL("Info: Upgrading Image -- OK\n");
	}

    SAFE_MEM_FREE(m_pbUpgradeImage);
    m_dwImageLen = 0;

	m_pUpgradeThread->Stop();

    return hr;
}

PVOID CSWUpgradeApp::OnUpgradeImageProxy(PVOID pvArg)
{
    if (NULL == pvArg)
    {
        return (PVOID)E_INVALIDARG;
    }
    
    CSWUpgradeApp* pThis = (CSWUpgradeApp*)pvArg;

    pThis->OnUpgradeImage();
	pThis->m_fUpgrading = FALSE;

	return 0;
}

HRESULT CSWUpgradeApp::ProcessXmlCmd(const PBYTE pbBuf, const DWORD dwSize, PVOID* ppvOutBuf, DWORD* pdwOutSize)
{
    if(NULL != pbBuf)
    {
        SW_TRACE_DEBUG("Info: Recv XML Content = %s\n", (const CHAR*)pbBuf);
    }

    HRESULT hr = m_ProcessXMLCmd.ProcessXMLCmd(pbBuf,dwSize,ppvOutBuf,pdwOutSize,&m_iExitCode,&m_fGoingToExit,&m_fSwitchingSystem);
    if(FAILED(hr))
    {
        /*如果返回失败，说明命令不被支持，检查是否旧版本命令*/
        TiXmlDocument xmlDoc;
        if (NULL == pbBuf || 0 == dwSize || NULL == ppvOutBuf || NULL == pdwOutSize)
        {
            SW_TRACE_NORMAL("Err: NULL == pbBuf || 0 == dwSize || NULL == ppvOutBuf || NULL == pdwOutSize\n");
            return E_INVALIDARG;
        }

        if (0 == xmlDoc.Parse((const CHAR*)pbBuf))
        {
           SW_TRACE_NORMAL("Err: failed to parse cmd xml\n");
            return E_FAIL;
        }

        TiXmlElement* pEleRoot = NULL;
        pEleRoot = xmlDoc.RootElement();
        for (TiXmlElement *Ele = pEleRoot->FirstChildElement(); Ele; Ele = Ele->NextSiblingElement())
        {
            if (NULL != Ele->GetText())
            {
                if (0 == swpa_strcmp(Ele->GetText(), "ProgramEncryptionChip") && IsBackupMode())
                {
                    SW_TRACE_NORMAL("Info: got ProgramEncryptionChip Cmd\n");

                    SAFE_MEM_FREE(*ppvOutBuf);
                    *pdwOutSize = 0;

                    const CHAR* szServerIp = (CHAR*)Ele->Attribute("ServerIp");
                    if (NULL == szServerIp)
                    {
                        SW_TRACE_NORMAL("Err:Cmd=ProgramEncryptionChip no ServerIp info.\n");
                        return E_FAIL;
                    }

                    WORD wServerPort = 0;
                    if (NULL == Ele->Attribute("ServerPort"))
                    {
                        SW_TRACE_NORMAL("Err:Cmd=ProgramEncryptionChip no ServerPort info.\n");
                        return E_FAIL;
                    }
                    wServerPort = (WORD)swpa_atoi(Ele->Attribute("ServerPort"));

                    if (FAILED(OnWriteSecCode(szServerIp, wServerPort)))
                    {
                        SW_TRACE_NORMAL("Err: failed to process ProgramEncryptionChip cmd.\n");
                        return E_FAIL;

                    }

                    hr = S_OK;
                }
                else if (IsBackupMode() && (0 == swpa_strcmp(Ele->GetText(), "StartAutotest")
                                            || 0 == swpa_strcmp(Ele->GetText(), "GetLatestReport")
                                            || 0 == swpa_strcmp(Ele->GetText(), "GetAutotestProgress")))
                {
                    SAFE_MEM_FREE(*ppvOutBuf);
                    *pdwOutSize = 0;

                    if (!m_fAutotestStarted)
                    {
                        HeartBeat();
                        if (FAILED(m_pAutotest->Initialize()))
                        {
                            SW_TRACE_NORMAL("Err: Failed to start Autotest\n");
                            return E_FAIL;
                        }

                        m_fAutotestStarted = TRUE;
                    }
                    HeartBeat();
                    return m_pAutotest->ReceiveCmd(CAMERA_XML_EXT_CMD, pbBuf, dwSize, ppvOutBuf, pdwOutSize);
                }
                else
                {
                    SW_TRACE_NORMAL("Err: got invalid cmd: %s, ignore it\n", Ele->GetText());
                    hr = E_FAIL;
                }
            }
        }
    }

    if(NULL != *ppvOutBuf)
    {
        SW_TRACE_DEBUG("Info: Respond XML Content = %s\n", (const CHAR*)*ppvOutBuf);
    }

    return hr;
}

HRESULT CSWUpgradeApp::UpgradeImage(const DWORD dwSize, CSWTCPSocket& sockData)
{
	if (0 == dwSize)
	{
		SW_TRACE_DEBUG("Err: 0 == dwSize\n");
		return E_INVALIDARG;
	}
	
    if (NULL != m_pbUpgradeImage || 0 != m_dwImageLen)
    {
        SW_TRACE_DEBUG("Err: NULL != m_pbUpgradeImage || 0 != m_dwImageLen\n");
        return E_FAIL;
    }


	m_pbUpgradeImage = (PBYTE)swpa_mem_alloc(dwSize);
    if (NULL == m_pbUpgradeImage)
    {
        SW_TRACE_NORMAL("Err: no memory for pbBuf!\n");
        return E_FAIL;
    }
    
    SW_TRACE_DEBUG("Info: Receiving data...!\n");
	DWORD dwRecvLen = 0;
    if (FAILED(sockData.Read(m_pbUpgradeImage, dwSize, &dwRecvLen))
		|| dwSize != dwRecvLen)
    {
        SW_TRACE_NORMAL("Err: failed to read command data recv len %d!\n",dwRecvLen);
        SAFE_MEM_FREE(m_pbUpgradeImage);
        return E_FAIL;
    }

    SW_TRACE_DEBUG("Info: Received data size = %d!\n", dwRecvLen);
	
	m_dwImageLen = dwSize;

    if (NULL != m_pUpgradeThread)
    {
        if (!m_pUpgradeThread->IsValid())
        {
            return E_ACCESSDENIED;
        }
        m_fIsUpgradeFailed = FALSE;
		if (FAILED(m_pUpgradeThread->Start(OnUpgradeImageProxy, this)))
	    {
	        SW_TRACE_NORMAL("Err: Start Upgrade Thread Failed!\n");
			return E_FAIL;
	    }
	    else
	    {
	        m_fUpgrading = TRUE;
	    }
    }
    else
    {
        return E_NOTIMPL;
    }
	return S_OK;
}

HRESULT CSWUpgradeApp::GetUpgradeImageStatus(PVOID* pvBuf, DWORD* pdwSize)
{
    if (NULL == pvBuf || NULL == pdwSize)
    {
        SW_TRACE_DEBUG("Err: NULL == pvBuf || NULL == pdwSize\n");
        return E_FAIL;
    }

    TiXmlDocument XmlOutputDoc;
    TiXmlDeclaration *pDeclaration = NULL;
    TiXmlElement *pRootEle = NULL;
    TiXmlElement *pStatusEle = NULL;
    
    pDeclaration = new TiXmlDeclaration("1.0","GB2312","yes");
    if (NULL == pDeclaration)
    {
        SW_TRACE_DEBUG("Err: no memory for pDeclaration\n");
        return E_OUTOFMEMORY;
    }
    XmlOutputDoc.LinkEndChild(pDeclaration);

    pRootEle = new TiXmlElement("UpgradeInfo");
    if (NULL == pRootEle)
    {    
        SW_TRACE_DEBUG("Err: no memory for pRootEle\n");
        return E_OUTOFMEMORY;
    }
    XmlOutputDoc.LinkEndChild(pRootEle);
    
    pRootEle->SetAttribute(HVXML_VER, HV_XML_CMD_VERSION_NO);

    pStatusEle = new TiXmlElement("Status");
    if (NULL == pStatusEle)
    {    
        SW_TRACE_DEBUG("Err: no memory for pStatusEle\n");
        return E_OUTOFMEMORY;
    }
    pRootEle->LinkEndChild(pStatusEle);

    CHAR szText[32] = {0};
    if (TRUE == m_fIsUpgradeFailed)			//升级失败
    {
        swpa_strncpy(szText, "Failed", sizeof(szText)-1);
    }
    else
    {
        if (0 > m_iUpgradeProgress)
        {
            swpa_strncpy(szText, "NotStarted", sizeof(szText)-1);
        }
        else if (100 <= m_iUpgradeProgress)
        {
            swpa_strncpy(szText, "Finished", sizeof(szText)-1);
        }
        else
        {
            swpa_strncpy(szText, "Upgrading", sizeof(szText)-1);
            pStatusEle->SetAttribute("Progress", m_iUpgradeProgress);
        }
    }


    TiXmlText* pStatusEleText = new TiXmlText(szText);
    if (NULL == pStatusEleText)
    {
        SW_TRACE_DEBUG("Err: no memory for pStatusEleText\n");
        return E_OUTOFMEMORY;
    }
    pStatusEle->LinkEndChild(pStatusEleText);


    TiXmlPrinter XmlPrinter;
    XmlOutputDoc.Accept(&XmlPrinter);

    *pvBuf = swpa_mem_alloc(XmlPrinter.Size() + 1);
    if (NULL == *pvBuf)
    {
        SW_TRACE_DEBUG("Err: no memory for *pvBuf\n");
        return E_OUTOFMEMORY;
    }
    swpa_memset(*pvBuf, 0, XmlPrinter.Size() + 1);
    swpa_strncpy((CHAR*)*pvBuf, XmlPrinter.CStr(), XmlPrinter.Size());

    *pdwSize = XmlPrinter.Size() + 1;

    return S_OK;

}

HRESULT CSWUpgradeApp::WriteUpgradePacket(const CHAR* pbData, const INT iLen)
{
    if ( NULL == pbData || iLen <= 0 )
    {
        SW_TRACE_DEBUG("Err: NULL == pbData || iLen <= 0\n");
        return E_FAIL;
    }
    
    BYTE* pbUpgradePacket = (BYTE*)pbData;
    IMAGE_PACKAGE_HEADER cHeader;

    m_iUpgradeProgress = 0;

    if (FAILED(DecryptPackage(pbUpgradePacket)))
    {
        SW_TRACE_DEBUG("Err: failed to decrypt the package\n");
        m_iUpgradeProgress = -1;
        m_fIsUpgradeFailed = TRUE;
        return E_FAIL; 
    }    

    //提取镜像头
    swpa_memcpy(&cHeader, pbUpgradePacket, sizeof(cHeader));
    cHeader.ih_magic = MyNetToHostLong(cHeader.ih_magic);
    cHeader.ih_hcrc = MyNetToHostLong(cHeader.ih_hcrc);
    //cHeader.ih_time = MyNetToHostLong(cHeader.ih_time);
    cHeader.ih_size = MyNetToHostLong(cHeader.ih_size);
    //cHeader.ih_load = MyNetToHostLong(cHeader.ih_load);
    //cHeader.ih_ep = MyNetToHostLong(cHeader.ih_ep);
    cHeader.ih_dcrc = MyNetToHostLong(cHeader.ih_dcrc);
    pbUpgradePacket += sizeof(cHeader);
    
    
    if ( cHeader.ih_type != IH_TYPE_MULTI )
    {
        SW_TRACE_DEBUG("Err: cHeader.ih_type != IH_TYPE_MULTI\n");
        m_iUpgradeProgress = -1; 
        m_fIsUpgradeFailed = TRUE;
        return E_FAIL; // 不被支持的升级包
    }
    
    SW_TRACE_DEBUG("Info: ih_name = [%s] [%d] type:[%d]\n", cHeader.ih_name, cHeader.ih_size, cHeader.ih_type);

    if ( cHeader.ih_dcrc != CSWUtils::CalcCrc32(0, (BYTE*)pbUpgradePacket, cHeader.ih_size) ) //确保整个升级包镜像数据的正确性
    {
        SW_TRACE_DEBUG("Err: CRC check failed!!\n");
        m_iUpgradeProgress = -1; 
        m_fIsUpgradeFailed = TRUE;
        return E_FAIL; //CRC异常！
    }
    
    INT rgiFileSize[16];
    DWORD dwCount = 0;
    DWORD dwTotalSize = 0;
    for (dwCount = 0; dwCount < 16; ++dwCount )
    {
        swpa_memcpy(&(rgiFileSize[dwCount]), pbUpgradePacket, sizeof(INT));
        rgiFileSize[dwCount] = MyNetToHostLong(rgiFileSize[dwCount]);
        pbUpgradePacket += sizeof(INT);
        
        SW_TRACE_DEBUG("Info: fileSize = [%d]\n", rgiFileSize[dwCount]);
        if ( 0 == rgiFileSize[dwCount] )
        {
            break;
        }
        dwTotalSize += rgiFileSize[dwCount];
    }

    DWORD dwUpgradedSize = 0;
    for (INT n = 0; n < dwCount; ++n )
    {
        //提取镜像头
        IMAGE_PACKAGE_HEADER sHeaderForWrite;
        swpa_memcpy(&sHeaderForWrite, pbUpgradePacket, sizeof(sHeaderForWrite));
        swpa_memcpy(&cHeader, pbUpgradePacket, sizeof(cHeader));
        DWORD dwOriginalDataCRC = cHeader.ih_dcrc;
        //cHeader.ih_magic = MyNetToHostLong(cHeader.ih_magic);
        //cHeader.ih_hcrc = MyNetToHostLong(cHeader.ih_hcrc);
        //cHeader.ih_time = MyNetToHostLong(cHeader.ih_time);
        cHeader.ih_size = MyNetToHostLong(cHeader.ih_size);
        //cHeader.ih_load = MyNetToHostLong(cHeader.ih_load);
        //cHeader.ih_ep = MyNetToHostLong(cHeader.ih_ep);
        cHeader.ih_dcrc = MyNetToHostLong(cHeader.ih_dcrc);
        pbUpgradePacket += sizeof(cHeader);
        
        SW_TRACE_DEBUG("Info: ih_name = [%s] [%d] type:[%d]\n", cHeader.ih_name, cHeader.ih_size, cHeader.ih_type);        
        
        if ( cHeader.ih_dcrc != CSWUtils::CalcCrc32(0, (BYTE*)pbUpgradePacket, cHeader.ih_size) ) //确保镜像部件数据的正确性
        {
            SW_TRACE_DEBUG("Err: %s CRC Check failed\n", cHeader.ih_name);
            m_fIsUpgradeFailed = TRUE;
            return E_FAIL; 
        }

		
		CHAR szBlockName[32];
        BOOL fUbiImage = FALSE;
        BOOL fDecrypt = FALSE;
		
        if (NeedUpgradeBlock((CHAR *)cHeader.ih_name,dwOriginalDataCRC,szBlockName,&fUbiImage,&fDecrypt) )//( NeedUpgrade((CHAR*)cHeader.ih_name, dwOriginalDataCRC/*cHeader.ih_dcrc*/) ) // 判断该文件是否需要升级
        {
        	if (TRUE == fDecrypt)    //uboot和uboot_min不需要解密
            {
                SW_TRACE_DEBUG("Info: Decrypt %s Image data ...!\n",cHeader.ih_name);
                unsigned long ulPos = 0;
                CCryptOpt::SimpleDecryptData(pbUpgradePacket,cHeader.ih_size,&ulPos);
            }
			
            if (FAILED(WritePackageToBlock(&sHeaderForWrite,pbUpgradePacket,cHeader.ih_size,szBlockName,fUbiImage)))//( FAILED( WritePackage(&sHeaderForWrite, pbUpgradePacket, cHeader.ih_size) )) // 将该文件写入Flash中对应的位置
            {
                SW_TRACE_DEBUG("Err: failed to upgrade %s\n", cHeader.ih_name );
                m_iUpgradeProgress = -1; //
                m_fIsUpgradeFailed = TRUE;
                return E_FAIL; //写Flash异常！
            }
        }

        dwUpgradedSize += cHeader.ih_size;

        m_iUpgradeProgress = dwUpgradedSize * 100/ dwTotalSize;
        
        pbUpgradePacket += cHeader.ih_size;
        //镜像部件数据要求4字节对齐
        if ( (cHeader.ih_size%4) != 0 )
        {
            pbUpgradePacket += (4 - (cHeader.ih_size%4));
        }
    }

    m_iUpgradeProgress = 100;
    m_fIsUpgradeFailed = FALSE;
    SW_TRACE_DEBUG("Info: Write Package -- OK!\n");
    
    return S_OK; // 升级成功

}



BOOL CSWUpgradeApp::NeedUpgrade(const CHAR* szName, const DWORD dwNowCrc)
{
#define CHECK_UPGRADE(UPGRADE_NAME, FILE_NAME, PARTITION_NAME)\
if ( swpa_strcmp(szName, UPGRADE_NAME) == 0 )\
{\      
	CSWFile cFile;\
	DWORD dwReadLen = 0;\
	IMAGE_PACKAGE_HEADER cHeader;\
	if (FAILED(cFile.Open(FILE_NAME, "r"))\
        || FAILED(cFile.Read(&cHeader, sizeof(cHeader), &dwReadLen))\
        || sizeof(cHeader) != dwReadLen)\
    {\
    	SW_TRACE_DEBUG("Err: Failed to read %s header!\n", PARTITION_NAME);\
    	return TRUE;\
    }\
    \
	SW_TRACE_DEBUG("Info: cHeader.ih_dcrc = %x, dwNowCrc = %x", cHeader.ih_dcrc, dwNowCrc);\
    \
	if (cHeader.ih_dcrc != dwNowCrc)\
    {\
    	SW_TRACE_DEBUG("Info: %s  needs upgrade!\n", PARTITION_NAME);\
    	return TRUE;\
    }\
	else\
    {\
    	return FALSE;\
    }\
}

    CHECK_UPGRADE("kernel",     "FLASH/0/KERNEL",     "DM6467 KERNEL");
    CHECK_UPGRADE("rootfs",     "FLASH/0/ROOTFS",     "DM6467 ROOTFS");
    CHECK_UPGRADE("app",         "FLASH/0/ARM_APP",     "DM6467 ARM_APP");
    CHECK_UPGRADE("dsp",         "FLASH/0/DSP_APP",     "DM6467 DSP_APP");
    CHECK_UPGRADE("fpga",         "FLASH/0/FFGA_ROM", "FFGA_ROM");
    CHECK_UPGRADE("kernel_368", "FLASH/1/KERNEL",     "DM368 KERNEL");
    CHECK_UPGRADE("rootfs_368", "FLASH/1/ROOTFS",     "DM368 ROOTFS");
    CHECK_UPGRADE("kernel_bak", "FLASH/0/KERNEL_BAK", "DM6467 KERNEL_BAK");
    CHECK_UPGRADE("rootfs_bak", "FLASH/0/ROOTFS_BAK", "DM6467 ROOTFS_BAK");

    if ( swpa_strcmp(szName, "uboot_master") == 0 )
    {    
        return TRUE; //NOTE: return TRUE directly if the Image Package has uboot data
    }
    else if ( swpa_strcmp(szName, "ubl_master") == 0 )
    {    
        return TRUE; //NOTE: return TRUE directly if the Image Package has ubl data
    }
    else if ( swpa_strcmp(szName, "uBoot_368") == 0 )
    {    
        return TRUE; //NOTE: return TRUE directly if the Image Package has uboot data
    }
    else if ( swpa_strcmp(szName, "ubl_368") == 0 )
    {    
        return TRUE; //NOTE: return TRUE directly if the Image Package has ubl data
    }
    else
    {
        //todo:其它部件
        return FALSE;
    }

    return FALSE;
}


HRESULT CSWUpgradeApp::WriteFile(const CHAR* szFileName, const PVOID pvHeader, const PBYTE pbData, const DWORD dwSize)
{
    if (NULL == szFileName || NULL == pbData || 0 == dwSize)
    {
        SW_TRACE_DEBUG("Err: NULL == szFileName || NULL == pbData || 0 == dwSize\n");
        return E_INVALIDARG;
    }    

    SW_TRACE_DEBUG("Info: Writing %s...!\n", szFileName);
    
    CSWFile cFile;
    DWORD dwWrittenLen = 0;
    DWORD dwNewPos = 0;

    if (FAILED(cFile.Open(szFileName, "w+")))
    {
        SW_TRACE_NORMAL("Err: failed to open %s\n", szFileName);
        return E_FAIL;
    }
    
    if (NULL == pvHeader)
    {
        if (FAILED(cFile.Seek(0, SWPA_SEEK_SET, &dwNewPos))
            || 0 != dwNewPos
            || FAILED(cFile.Write(pbData, dwSize, &dwWrittenLen))
            || dwWrittenLen != dwSize)
        {
            SW_TRACE_NORMAL("Err: Failed to write %s!\n", szFileName);
            return E_FAIL;
        }
    }
    else
    {
        IMAGE_PACKAGE_HEADER cNewHeader;
        swpa_memcpy(&cNewHeader, pvHeader, sizeof(cNewHeader));

        IMAGE_PACKAGE_HEADER cNullHeader;
        swpa_memset(&cNullHeader, 0, sizeof(cNullHeader));

        if (FAILED(cFile.Write((PVOID)&cNullHeader, sizeof(cNullHeader), &dwWrittenLen))     //1. clear the old header
            || sizeof(cNullHeader) != dwWrittenLen
            || FAILED(cFile.Write(pbData, dwSize, &dwWrittenLen))                            //2. write data
            || dwWrittenLen != dwSize            
            || FAILED(cFile.Seek(0, SWPA_SEEK_SET, &dwNewPos))
            || 0 != dwNewPos
            || FAILED(cFile.Write((PVOID)&cNewHeader, sizeof(cNewHeader), &dwWrittenLen))            //3. write new header
            || sizeof(cNewHeader) != dwWrittenLen
            )
        {
            SW_TRACE_NORMAL("Err: Failed to write %s!\n", szFileName);
            return E_FAIL;
        }        
    }

    SW_TRACE_DEBUG("Info: Write %s -- OK!\n", szFileName);
    
    return cFile.Close();        
}




HRESULT CSWUpgradeApp::WritePackage(const PVOID pvHeader, const PBYTE pbData, const DWORD dwSize)
{    
    IMAGE_PACKAGE_HEADER cHeader;
    swpa_memcpy(&cHeader, pvHeader, sizeof(cHeader));

#define WRITE_PARTITION_FILE(UPGRADE_NAME, FILE_NAME, PARTITION_NAME, HEADER_PTR)\
if ( swpa_strcmp((const char*)cHeader.ih_name, UPGRADE_NAME) == 0 )\
{\
	SW_TRACE_DEBUG("Info: Upgrading %s...!\n", PARTITION_NAME);\
	if (FAILED(WriteFile(FILE_NAME, HEADER_PTR, pbData, dwSize)))\
    {\
    	SW_TRACE_NORMAL("Err: Upgrade %s ---- NG!\n", PARTITION_NAME);\
    	return E_FAIL;\
    }\
	SW_TRACE_DEBUG("Info: Upgrade %s -- OK!\n", PARTITION_NAME);\
	return S_OK;\
}
    
    WRITE_PARTITION_FILE("kernel",         "FLASH/0/KERNEL",     "DM8127 KERNEL", 	pvHeader);
	WRITE_PARTITION_FILE("rootfs",         "FLASH/0/ROOTFS",     "DM8127 ROOTFS", 	pvHeader);
	WRITE_PARTITION_FILE("app",         "FLASH/0/ARM_APP",     "DM8127 ARM_APP", 	pvHeader);
	WRITE_PARTITION_FILE("dsp",         "FLASH/0/DSP_APP",     "DSP_APP",         	pvHeader);
	WRITE_PARTITION_FILE("fpga",         "FLASH/0/FFGA_ROM", "FFGA_ROM",     	pvHeader);
	WRITE_PARTITION_FILE("rootfs_bak",     "FLASH/0/ROOTFS_BAK", "DM8127 ROOTFS_BAK", 	pvHeader);
	WRITE_PARTITION_FILE("kernel_bak",     "FLASH/0/KERNEL_BAK", "DM8127 KERNEL_BAK", 	pvHeader);
	WRITE_PARTITION_FILE("uboot_master", "FLASH/0/UBOOT",     "UBOOT", 	NULL/*no header to write*/);

    //todo: how to drvlib api -> swfc?
	if ( swpa_strcmp((const char*)cHeader.ih_name, "ubl_master") == 0 )
    {    
        SW_TRACE_DEBUG("Info: Upgrading DM6467 UBL...!\n");

        UINT uiLen = dwSize;
        INT iRet = drv_eeprom_write(DEVID_SPI_EEPROM, 0x0, pbData, &uiLen);
        if (0 != iRet 
            || dwSize != uiLen)
        {
            SW_TRACE_DEBUG("Err: Failed to upgrade DM6467 UBL!\n");
            return E_FAIL;
        }
        
        SW_TRACE_DEBUG("Info: Upgrade DM6467 UBL...OK!\n");
        
        return S_OK;
    }
    else
    {
        //todo:其它部件
        SW_TRACE_DEBUG("Err: Upgrade %s -- Not implemented!\n", cHeader.ih_name);
        return E_NOTIMPL;
    }

    return S_OK;
}


DWORD CSWUpgradeApp::MyHostToNetLong(const DWORD dwVal)
{
    return MyNetToHostLong(dwVal);
}



DWORD CSWUpgradeApp::MyNetToHostLong(const DWORD dwVal)
{
    BYTE pbTemp[4] = {0};
    BYTE bTemp = 0;
    DWORD dwRetVal = 0;

    swpa_memcpy(pbTemp, &dwVal, 4);
    bTemp = pbTemp[0];
    pbTemp[0] = pbTemp[3];
    pbTemp[3] = bTemp;

    bTemp = pbTemp[1];
    pbTemp[1] = pbTemp[2];
    pbTemp[2] = bTemp;

    swpa_memcpy(&dwRetVal, pbTemp, 4);

    return dwRetVal;
}


HRESULT CSWUpgradeApp::DecryptPackage(PBYTE& pbUpgradePacket)
{
    
    IMAGE_PACKAGE_HEADER cHeader;
    
    //提取镜像头
    swpa_memcpy(&cHeader, pbUpgradePacket, sizeof(cHeader));
    cHeader.ih_magic = MyNetToHostLong(cHeader.ih_magic);
    cHeader.ih_hcrc = MyNetToHostLong(cHeader.ih_hcrc);
    //cHeader.ih_time = MyNetToHostLong(cHeader.ih_time);
    cHeader.ih_size = MyNetToHostLong(cHeader.ih_size);
    //cHeader.ih_load = MyNetToHostLong(cHeader.ih_load);
    //cHeader.ih_ep = MyNetToHostLong(cHeader.ih_ep);
    cHeader.ih_dcrc = MyNetToHostLong(cHeader.ih_dcrc);
    
    if (0 == swpa_strcmp("upgrade", (const CHAR*)cHeader.ih_name))//encrypted
    {
        pbUpgradePacket += sizeof(cHeader);
            
        SW_TRACE_DEBUG("Info: ih_name = [%s] [%d] type:[%d]\n", cHeader.ih_name, cHeader.ih_size, cHeader.ih_type);
        
        CHAR szIp[32] = {0};
        CHAR szMask[32] = {0};
        CHAR szGateway[32] = {0};
        CHAR szMAC[32] = {0};
        DWORD dwMAC[6] = {0};
        BYTE bMac[6] = {0};
        
        if (SWPAR_OK != swpa_tcpip_getinfo("eth0", 
            szIp, sizeof(szIp)-1,
            szMask, sizeof(szMask)-1,
            szGateway, sizeof(szGateway)-1,
            szMAC, sizeof(szMAC)-1))
        {
            SW_TRACE_DEBUG("Err: failed to get Mac Addr\n");
            m_iUpgradeProgress = -1; //
            return E_FAIL; 
        }
        else
        {
            swpa_sscanf(szMAC, "%x:%x:%x:%x:%x:%x", &dwMAC[0], &dwMAC[1], &dwMAC[2], &dwMAC[3], &dwMAC[4], &dwMAC[5]);
            for (DWORD i=0; i<sizeof(bMac); i++)
            {
                bMac[i] = dwMAC[i] & 0xFF;
                //SW_TRACE_DEBUG("Info: bMACVal[i] = %#x\n", bMac[i]);
            }
        }

        INT iEncryptedSize = 0;
        swpa_memcpy(&iEncryptedSize, pbUpgradePacket, sizeof(INT));
        iEncryptedSize = MyNetToHostLong(iEncryptedSize);
        pbUpgradePacket += sizeof(iEncryptedSize);

        INT iFlag = 0;
        swpa_memcpy(&iFlag, pbUpgradePacket, sizeof(INT));
        iFlag = MyNetToHostLong(iFlag);
        pbUpgradePacket += sizeof(iFlag);

        if (0 != iFlag)
        {
            //只支持一个加密包，因此这4个字节需为0
            SW_TRACE_DEBUG("Err: 0 != iFlag\n");
            return E_FAIL; 
        }

        ULONG ulPos = 0;
        DecryptPackageByMAC(pbUpgradePacket, iEncryptedSize, bMac, &ulPos);        

    }

    return S_OK;
}


// 解密
VOID CSWUpgradeApp::DecryptPackageByMAC(BYTE* pbData, INT iLen, BYTE* pbKeyMAC, ULONG* ulPos)
{
#define M_NUM 0x75DA63BF
#define MAKE_KEY(m1, m2, m3) ((m1*m2+m3) + (~m1)*(m1<<2)*((~m2)<<3) + m2 + (~m3) + (m3>>1))
#define MAKE_XOR_CODE(m1, m2, m3) (m1*(M_NUM&m2)*m3)
#define ENCODE(m1, m2, m3) (m1 ^ MAKE_XOR_CODE(m2,m3,M_NUM))

    BYTE k0 = MAKE_KEY(pbKeyMAC[3], pbKeyMAC[4], pbKeyMAC[5]);
    while ( iLen-- )
    {
        *pbData++ = ENCODE(*pbData, k0, (BYTE)*ulPos);
        (*ulPos)++;
    }
}

HRESULT CSWUpgradeApp::OnWriteSecCode(const CHAR* szServerIp, const WORD wServerPort)
{
    CSWTCPSocket cSockClient;

    BYTE bFuseStatus = 0;
    if (SWPAR_OK != swpa_device_get_fuse_status(&bFuseStatus))
    {
        SW_TRACE_NORMAL("Err: failed to get fuse status.\n");
        return E_FAIL;
    }

    if (0 == bFuseStatus)//fused already, do nothing
    {
        INT iRet = swpa_device_crypt_init();

        BYTE bBuf[32] = {0};
        UINT iLen = 0x07;
        iRet = swpa_device_crypt_read(0x0, bBuf, &iLen);    // nc
        SW_TRACE_DEBUG("Info: NC= %02X %02X %02X %02X %02X %02X %02X\n", bBuf[0], bBuf[1], bBuf[2], bBuf[3], bBuf[4], bBuf[5], bBuf[6]);

        iLen = 0x10;
        iRet = swpa_device_crypt_read(0x40, bBuf, &iLen); // key
        SW_TRACE_NORMAL("Info: Already fused, do nothing and return OK.\n");
        return E_FAIL;
    }
    
    
    if (FAILED(cSockClient.Create()))
    {
        SW_TRACE_NORMAL("Err: failed to create client socket.\n");
        return E_FAIL;
    }

    cSockClient.SetRecvTimeout(4000);
    cSockClient.SetSendTimeout(4000);
    INT iTryCount = 10;
    while (iTryCount--)
    {
        if (FAILED(cSockClient.Connect(szServerIp, wServerPort)))
        {
            SW_TRACE_NORMAL("Err: failed to Connect to %s:%d, trying another %d times\n", szServerIp, wServerPort, iTryCount);
            swpa_thread_sleep_ms(1000);
        }
        else
        {
            break;
        }
    }

    if (0 > iTryCount)
    {
        SW_TRACE_NORMAL("Err: failed to Connect to %s:%d\n", szServerIp, wServerPort);
        return E_FAIL;
    }

    CHAR szSN[512] = {0};
    DWORD dwLen = sizeof(szSN);
    INT iRet = swpa_device_read_sn(szSN, &dwLen);
    if (SWPAR_OK != iRet)
    {
        SW_TRACE_NORMAL("Err: failed to get device serial number\n");
        return E_FAIL;
    }


    CHAR szIp[32]={0}, szMask[32]={0}, szGateway[32]={0}, szMac[32]={0};
    iRet = swpa_tcpip_getinfo("eth0", szIp, 31, szMask, 31, szGateway, 31, szMac, 31);
    if (SWPAR_OK != iRet)
    {
        SW_TRACE_NORMAL("Err: failed to get MAC\n");
        return E_FAIL;
    }
    DWORD dwThisMAC[6] = {0};
    BYTE bThisMAC[6] = {0};

    swpa_sscanf(szMac, "%x:%x:%x:%x:%x:%x", &dwThisMAC[0], &dwThisMAC[1], &dwThisMAC[2], &dwThisMAC[3], &dwThisMAC[4], &dwThisMAC[5]);
    for (DWORD i=0; i<sizeof(bThisMAC); i++)
    {
        bThisMAC[i] = dwThisMAC[i] & 0xFF;
    }

    // Network command head
    typedef struct _CMD_PROTOCOL_T {
        UINT uiLen;
        UINT uiType;
        UINT uiID;
        UINT uiPass;
    } CMD_PROTOCOL_T;

    struct CMD_INFO {
        CMD_PROTOCOL_T *cp;
        VOID *pvParam;
    };

#define COMMAND_PASSWRD    0x20101213

    CMD_PROTOCOL_T tCmd;
    
    tCmd.uiType = MyHostToNetLong(0x1204);
    tCmd.uiID = MyHostToNetLong(0);
    tCmd.uiLen = MyHostToNetLong(146);
    tCmd.uiPass = MyHostToNetLong(COMMAND_PASSWRD);
    
    BYTE pbBuf[256] = {0};

    swpa_memcpy(pbBuf, &tCmd, sizeof(tCmd));
    swpa_memcpy(pbBuf+16, szSN, 128);
    swpa_memcpy(pbBuf+16+128, bThisMAC, 6);
    if (FAILED(cSockClient.Send(pbBuf, sizeof(pbBuf), NULL)))
    {
        SW_TRACE_NORMAL("Err: failed to send data to %s:%d.\n", szServerIp, wServerPort);
        return E_FAIL;
    }
    
    if (FAILED(cSockClient.Read(pbBuf, sizeof(tCmd)+8+16, NULL)))
    {
        SW_TRACE_NORMAL("Err: failed to read data from %s:%d.\n", szServerIp, wServerPort);
        return E_FAIL;
    }

    swpa_memset(&tCmd, 0, sizeof(tCmd));
    swpa_memcpy(&tCmd, pbBuf, sizeof(tCmd));

    BYTE bDevNC[8]= {0};
    BYTE bDevData[16] = {0};

    if (1 == MyNetToHostLong(tCmd.uiID)
        && 36 == MyNetToHostLong(tCmd.uiLen)
        && 0x1204 == MyNetToHostLong(tCmd.uiType)
        && COMMAND_PASSWRD == MyNetToHostLong(tCmd.uiPass))
    {
        swpa_memcpy(bDevNC, pbBuf + 16, 8);
        swpa_memcpy(bDevData, pbBuf + 24, 16);
    }
    else
    {
        SW_TRACE_NORMAL("Err: Got invalid cmd response from server.\n");
        return E_FAIL;
    }

    if (0 != bDevNC[7])
    {
        SW_TRACE_NORMAL("Err: Got invalid NC from server.\n");
        return E_FAIL;
    }

    SW_TRACE_DEBUG("Info: Got Server NC= %02X %02X %02X %02X %02X %02X %02X\n", bDevNC[0], bDevNC[1], bDevNC[2], bDevNC[3], bDevNC[4], bDevNC[5], bDevNC[6]);

	iTryCount = 2;	//尝试3次
	do
	{
		SW_TRACE_NORMAL("Info: Config crypt device, %d times can try!",iTryCount);
		if (SWPAR_OK != swpa_device_init_crypt_config(bDevNC, bDevData))
		{
			SW_TRACE_NORMAL("Err: failed to init_crypt_config.\n");
			swpa_thread_sleep_ms(500);
			iTryCount--;
			continue;
		}
		
		if (SWPAR_OK != swpa_device_crypt_init())
		{
			SW_TRACE_NORMAL("Err: OnWriteSecCode swpa_device_crypt_init failed. config crypt device failed\n");
			swpa_thread_sleep_ms(500);
			iTryCount--;
			continue;
		}
		
		SW_TRACE_NORMAL("Config crypt device successful");
		break;
	}while(iTryCount--);

	if (iTryCount < 0)
	{
		SW_TRACE_NORMAL("Config crypt device failed,return \n");
		return SWPAR_FAIL;
	}
	
    if (SWPAR_OK != swpa_device_fuse_write())
    {
        SW_TRACE_NORMAL("Err: failed to fuse.\n");
        return E_FAIL;
    }    

    cSockClient.Close();

    SW_TRACE_NORMAL("Info: Program Sec Chip -- OK.\n");
    
    return S_OK;
}



//保存备份系统的版本号
HRESULT CSWUpgradeApp::SaveBackupSystemVersion(const DWORD dwVersion)
{
    CHAR szFileName[64];

    //目录不存在，创建
    if (SWPAR_OK != swpa_utils_dir_exist(HV_BAKSYS_VERSION_PATH))
    {
        if (SWPAR_OK != swpa_utils_dir_create(HV_BAKSYS_VERSION_PATH)) 
        {
            SW_TRACE_DEBUG("Err: mkdir %s failed\n",HV_BAKSYS_VERSION_PATH);
            return E_FAIL;
        }
    }

    swpa_sprintf(szFileName,"%s/Version.dat", HV_BAKSYS_VERSION_PATH);

	CSWFile cVerFile;
	if (FAILED(cVerFile.Open(szFileName, "w")))
    {
        SW_TRACE_DEBUG("Err: open %s failed\n",szFileName);
        return E_FAIL;
    }

	CHAR szVersion[32] = {0};
	swpa_snprintf(szVersion, sizeof(szVersion), "%d", dwVersion);
	SW_TRACE_NORMAL("Info: backsys version str = %s, strlen(str) = %d\n", szVersion, swpa_strlen(szVersion));
	if (FAILED(cVerFile.Write(szVersion, sizeof(szVersion))))
	{
		SW_TRACE_NORMAL("Err: Write backup sys version info to %s failed\n",szFileName);
		cVerFile.Close();
        return E_FAIL;
	}

	cVerFile.Close();

	sync();

    SW_TRACE_NORMAL("Info: Save BakSys Version %s To File %s -- OK!", szVersion, szFileName);

    return S_OK;
}



//读取备份系统的版本号
HRESULT CSWUpgradeApp::ReadBackupSystemVersion(DWORD* pdwVersion)
{
    CHAR szFileName[64];

    if (SWPAR_OK != swpa_utils_dir_exist(HV_BAKSYS_VERSION_PATH))
    {
        SW_TRACE_NORMAL("Err: No BakSys Version info...\n");
        return E_FAIL;
    }

    swpa_sprintf(szFileName,"%s/Version.dat", HV_BAKSYS_VERSION_PATH);

	CSWFile cVerFile;
	if (FAILED(cVerFile.Open(szFileName, "r")))
    {
        SW_TRACE_DEBUG("Err: open %s failed\n",szFileName);
        return E_FAIL;
    }

	CHAR szVersion[32] = {0};
	if (FAILED(cVerFile.Read(szVersion, sizeof(szVersion)-1)))
	{
        SW_TRACE_DEBUG("Err: failed to read %s\n", szFileName);
		cVerFile.Close();
        return E_FAIL;
	}
	*pdwVersion = swpa_atol(szVersion);
	cVerFile.Close();

    SW_TRACE_DEBUG("Info: BakSys Version %d", *pdwVersion);

    return S_OK;
}



#define HV_CUSTOMIZED_INFO_PATH       "/mnt/nand/CustomizedInfo"
//保存设备别名
HRESULT CSWUpgradeApp::SaveCustomizedDevName(LPCSTR szDevName)
{
    CHAR szFileName[64];

	if (NULL == szDevName)
	{
		SW_TRACE_DEBUG("Err: szDevName(%p) is invalid\n", szDevName);
        return E_INVALIDARG;
	}

    swpa_sprintf(szFileName,"%s/DevName.txt", HV_CUSTOMIZED_INFO_PATH);

    //目录不存在，创建
    if (SWPAR_OK != swpa_utils_dir_exist(HV_CUSTOMIZED_INFO_PATH))
    {
        if (SWPAR_OK != swpa_utils_dir_create(HV_CUSTOMIZED_INFO_PATH)) 
        {
            SW_TRACE_DEBUG("Err: mkdir %s failed\n",HV_CUSTOMIZED_INFO_PATH);
            return E_FAIL;
        }
    }

	CSWFile cFile;
	if (FAILED(cFile.Open(szFileName, "w")))
    {
        SW_TRACE_DEBUG("Err: open %s failed\n",szFileName);
        return E_FAIL;
    }

	CHAR szName[64] = {0};
	swpa_snprintf(szName, sizeof(szName), "%s", szDevName);
	SW_TRACE_NORMAL("Info: DevName str = %s, len = %d\n", szName, swpa_strlen(szName));
	if (FAILED(cFile.Write(szName, sizeof(szName))))
	{
		SW_TRACE_NORMAL("Err: Write DevName to %s failed\n",szFileName);
		cFile.Close();
        return E_FAIL;
	}

	cFile.Close();

    SW_TRACE_NORMAL("Info: Save DevName %s To File %s -- OK!", szName, szFileName);

    return S_OK;	
}



//读取设备别名
HRESULT CSWUpgradeApp::ReadCustomizedDevName(LPSTR pszName)
{
	CHAR szFileName[64];

	if (NULL == pszName)
	{
		SW_TRACE_NORMAL("Err: NULL == pszName\n");
		return E_INVALIDARG;
	}
	
	if (SWPAR_OK != swpa_utils_dir_exist(HV_CUSTOMIZED_INFO_PATH))
	{
		SW_TRACE_NORMAL("Err: No customized devname info...\n");
		return E_NOTIMPL;
	}

	swpa_sprintf(szFileName,"%s/DevName.txt", HV_CUSTOMIZED_INFO_PATH);

	CSWFile cFile;
	if (FAILED(cFile.Open(szFileName, "r")))
	{
		SW_TRACE_NORMAL("Err: open %s failed\n",szFileName);
		return E_NOTIMPL;
	}

	CHAR szDevName[64] = {0};
	if (FAILED(cFile.Read(szDevName, sizeof(szDevName))))
	{
		SW_TRACE_NORMAL("Err: Read customized devname info failed\n");
		cFile.Close();
		return E_FAIL;
	}

	cFile.Close();
	
	swpa_strcpy(pszName, szDevName);

	SW_TRACE_NORMAL("Info: customized dev name: %s", pszName);

	return S_OK;
}


