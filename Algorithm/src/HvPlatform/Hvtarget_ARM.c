#include "hvtarget_ARM.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <termios.h>
#include <stropts.h>
#include <errno.h>
#include <linux/types.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <pthread.h>
#include "swdrv.h"
#include "fastcrc32.h"
#include "HvDebug.h"
#include "HvExit.h"

/* hvutils.h */
extern __inline int HV_Trace(int nRank, char* szfmt, ...);

pthread_mutex_t g_Mutex = PTHREAD_MUTEX_INITIALIZER;

#ifdef _CAM_APP_
/* main.c */
extern pthread_mutex_t g_hMutuxEEPROM;   // ARM端读写EEPROM以及与DSP端的H.264编码互斥

int EEPROM_Lock()
{
    return pthread_mutex_lock(&g_hMutuxEEPROM);
}

int EEPROM_UnLock()
{
    return pthread_mutex_unlock(&g_hMutuxEEPROM);
}
#endif

#ifdef SINGLE_BOARD_PLATFORM
static BOOL g_fCanLightUp = FALSE;
#endif


#define START_STATUS_TABLE_FLAGS_IS_OK 0x20110923
#define START_STATUS_TABLE_VER_1109    0x10
#define START_STATUS_TABLE_VER_1201    0x88

struct start_status_table_t
{
    u32 flags;  // 标志：0为OK，其它值为异常
    u8 ver;   // 版本号
    u32 status; // 启动状态
    u8 reserve1; // 保留1
    u16 reserve2; // 保留2
    u32 crc1; // CRC32
    u32 camera_type;					// 相机类型
    u32 camera_sample_point_left;	// 相机采样点 左
    u32 camera_sample_point_right;	// 相机采样点 右
    u32 reserve3; // 保留
    u32 reserve4; // 保留
    u32 reserve5; // 保留
    u32 reserve6; // 保留
    u32 reserve7; // 保留
    u32 reserve8; // 保留
    u32 reserve9; // 保留
    u32 crc2; // CRC32
};

static int ReadStartStatusTable(struct start_status_table_t* pTable);
static int WriteStartStatusTable(struct start_status_table_t* pTable);

// 读取eeprom
// 返回, <0为出错,其它为正确
int EEPROM_Read(
    DWORD32 addr, // 地址
    BYTE8 *data, // 数据
    DWORD32 len // 长度
)
{
    int iRetryCount = 10;

#ifdef _CAM_APP_
    EEPROM_Lock();
#endif

    while ( iRetryCount-- )
    {
        if ( SwEepromRead(addr, data, len) < 0 )
        {
            WdtHandshake();
            HV_Trace(5, "<EEPROM_Read> Retry...\n");
            usleep(50*1000);
        }
        else
        {
#ifdef _CAM_APP_
            EEPROM_UnLock();
#endif
            return 0;
        }
    }

#ifdef _CAM_APP_
    EEPROM_UnLock();
#endif

    HV_Trace(5, "<EEPROM_Read> Error!\n");
    return -1;
}

// 写eeprom
// 返回, <0为出错,其它为正确
int EEPROM_Write(
    DWORD32 addr, // 地址
    BYTE8 *data, // 数据
    DWORD32 len // 长度
)
{
    int iRetryCount = 10;

#ifdef _CAM_APP_
    EEPROM_Lock();
#endif

    while ( iRetryCount-- )
    {
        if ( SwEepromWrite(addr, data, len) < 0 )
        {
            WdtHandshake();
            HV_Trace(5, "<EEPROM_Write> Retry...\n");
            usleep(50*1000);
        }
        else
        {
#ifdef _CAM_APP_
            EEPROM_UnLock();
#endif
            return 0;
        }
    }
#ifdef _CAM_APP_
    EEPROM_UnLock();
#endif
    HV_Trace(5, "<EEPROM_Write> Error!\n");
    return -1;
}

int g_iDisableResetWDT = 0;

// 喂狗
int WdtHandshake(void)
{
    return 0;

    // Comment by Shaorg: 由于使用了守护进程机制，所以这里暂时屏蔽。
    /*
    #if defined(_CAM_APP_) || defined(SINGLE_BOARD_PLATFORM)
    SwWTDCtrl(SW_DEVID_WTD_DEF, SW_MODE_WTD_RESET);
    #else
    return 0;
    #endif
    */
}

// 使能看门狗
int EnableWatchDog()
{
    return 0;

    // Comment by Shaorg: 由于使用了守护进程机制，所以这里暂时屏蔽。
    /*
    #if defined(_CAM_APP_) || defined(SINGLE_BOARD_PLATFORM)
    // 注：使能之前必须喂狗，防止遇到“打开看门狗的同时发生喂狗超时而导致设备复位”这样的情况。
    WdtHandshake();
    CPLD_Write(CPLD_WTD_ENABLE, 1); // 使能一体机主CPU端或单板的看门狗
    HV_Trace(5, "EnableWatchDog.\n");
    return 0;
    #else
    return 0;
    #endif
    */
}

#ifdef _CAM_APP_

// 获取CPU附近的温度
int GetCpuNearTemp(int* piTemp)
{
    return SwGetSysStatus(SW_DEVID_SYSSTAT_TEMPERATURE, piTemp);
}

void SetWorkLedOn()
{
    SwSetLight(SW_DEVID_LIGHT_WORK_LED_DEF, SW_MODE_LIGHT_ON);
}

void SetWorkLedOff()
{
    SwSetLight(SW_DEVID_LIGHT_WORK_LED_DEF, SW_MODE_LIGHT_OFF);
}

// 闪烁工作指示灯
void WorkLedLight()
{
    SwSetLight(SW_DEVID_LIGHT_WORK_LED_DEF, SW_MODE_LIGHT_FLASH);
}

void SetLan1LedOn()
{
}

void SetLan1LedOff()
{
}

void Lan1LedLight()
{
}

void SetLan2LedOn()
{
}

void SetLan2LedOff()
{
}

void Lan2LedLight()
{
}

void SetHddLedOn()
{
}

void SetHddLedOff()
{
}

void HddLedLight()
{
}

void SetStatLedOn()
{
}

void SetStatLedOff()
{
}

void StatLedLight()
{
}

void SetBuzzerOn()
{
}

void SetBuzzerOff()
{
}

void SetAllLightOn()
{
}

void SetAllLightOff()
{
}

void SetAllLightFlash()
{
}

#elif defined(SINGLE_BOARD_PLATFORM) // 单板平台
// 点LED
void SetWorkLedOn()
{
    if (!g_fCanLightUp) return;
    SwSetLight(SW_DEVID_LIGHT_WORK_LED_SINGLE, SW_MODE_LIGHT_ON);
}

void SetWorkLedOff()
{
    if (!g_fCanLightUp) return;
    SwSetLight(SW_DEVID_LIGHT_WORK_LED_SINGLE, SW_MODE_LIGHT_OFF);
}

void WorkLedLight()
{
    if (!g_fCanLightUp) return;
    SwSetLight(SW_DEVID_LIGHT_WORK_LED_SINGLE, SW_MODE_LIGHT_FLASH);
}

void SetLan1LedOn()
{
    if (!g_fCanLightUp) return;
    SwSetLight(SW_DEVID_LIGHT_LAN_LED_SINGLE_1, SW_MODE_LIGHT_ON);
}

void SetLan1LedOff()
{
    if (!g_fCanLightUp) return;
    SwSetLight(SW_DEVID_LIGHT_LAN_LED_SINGLE_1, SW_MODE_LIGHT_OFF);
}

void Lan1LedLight()
{
    if (!g_fCanLightUp) return;
    SwSetLight(SW_DEVID_LIGHT_LAN_LED_SINGLE_1, SW_MODE_LIGHT_FLASH);
}

void SetLan2LedOn()
{
    if (!g_fCanLightUp) return;
    SwSetLight(SW_DEVID_LIGHT_LAN_LED_SINGLE_2, SW_MODE_LIGHT_ON);
}

void SetLan2LedOff()
{
    if (!g_fCanLightUp) return;
    SwSetLight(SW_DEVID_LIGHT_LAN_LED_SINGLE_2, SW_MODE_LIGHT_OFF);
}

void Lan2LedLight()
{
    if (!g_fCanLightUp) return;
    SwSetLight(SW_DEVID_LIGHT_LAN_LED_SINGLE_2, SW_MODE_LIGHT_FLASH);
}

void SetHddLedOn()
{
    if (!g_fCanLightUp) return;
    SwSetLight(SW_DEVID_LIGHT_HDD_LED_SINGLE, SW_MODE_LIGHT_ON);
}

void SetHddLedOff()
{
    if (!g_fCanLightUp) return;
    SwSetLight(SW_DEVID_LIGHT_HDD_LED_SINGLE, SW_MODE_LIGHT_OFF);
}

void HddLedLight()
{
    if (!g_fCanLightUp) return;
    SwSetLight(SW_DEVID_LIGHT_HDD_LED_SINGLE, SW_MODE_LIGHT_FLASH);
}

void SetStatLedOn()
{
    if (!g_fCanLightUp) return;
    SwSetLight(SW_DEVID_LIGHT_STAT_LED_SINGLE, SW_MODE_LIGHT_ON);
}

void SetStatLedOff()
{
    if (!g_fCanLightUp) return;
    SwSetLight(SW_DEVID_LIGHT_STAT_LED_SINGLE, SW_MODE_LIGHT_OFF);
}

void StatLedLight()
{
    if (!g_fCanLightUp) return;
    SwSetLight(SW_DEVID_LIGHT_STAT_LED_SINGLE, SW_MODE_LIGHT_FLASH);
}

void SetAllLightOn()
{
    SwSetLight(SW_DEVID_LIGHT_ALL_LED_SINGLE, SW_MODE_LIGHT_ON);
}

void SetAllLightOff()
{
    SwSetLight(SW_DEVID_LIGHT_ALL_LED_SINGLE, SW_MODE_LIGHT_OFF);
}

void SetAllLightFlash()
{
    g_fCanLightUp = FALSE;
    SwSetLight(SW_DEVID_LIGHT_ALL_LED_SINGLE, SW_MODE_LIGHT_FLASH);
}

// 蜂鸣器
void SetBuzzerOn()
{
    if (!g_fCanLightUp)
    {
        SwSetBuzz(SW_MODE_BUZZ_ON);
    }
}

void SetBuzzerOff()
{
    SwSetBuzz(SW_MODE_BUZZ_OFF);
    g_fCanLightUp = TRUE;
}
#else  // 双模模式下的从CPU端

void SetWorkLedOn()
{
}

void SetWorkLedOff()
{
}

void WorkLedLight()
{
}

void SetLan1LedOn()
{
}

void SetLan1LedOff()
{
}

void Lan1LedLight()
{
}

void SetLan2LedOn()
{
}

void SetLan2LedOff()
{
}

void Lan2LedLight()
{
}

void SetHddLedOn()
{
}

void SetHddLedOff()
{
}

void HddLedLight()
{
}

void SetStatLedOn()
{
}

void SetStatLedOff()
{
}

void StatLedLight()
{
}

void SetBuzzerOn()
{
}

void SetBuzzerOff()
{
}

#endif // _CAM_APP_

// ------------------------------------------------------

// 获取CPLD版本号(长度：1字节)
int GetCPLDVersion(char* pbVerion, int iLen)
{
    if ( iLen >= 1 )
    {
        return SwCpldRead(0x10, pbVerion);
    }
    else
    {
        return -1;
    }
}

// 获取Uboot版本信息(长度：128字节)
int GetUbootVersion(char* pbVerion, int iLen)
{
    if ( iLen >= UBOOT_VERSION_LEN )
    {
        return EEPROM_Read(UBOOT_VERSION_ADDR, pbVerion, UBOOT_VERSION_LEN);
    }
    else
    {
        return -1;
    }
}

// 获取EEPROM保存变量与报表的格式版本号(长度：4字节)
int GetEEPROMInfoFormatVersion(char* pbVerion, int iLen)
{
    if ( iLen >= VERSION_LEN )
    {
        return EEPROM_Read(VERSION_ADDR, pbVerion, VERSION_LEN);
    }
    else
    {
        return -1;
    }
}

// 获取硬件序列号(长度：128字节)
int GetSN(char* pbSerialNo, int iLen)
{
    int iRet = -1;

    if ( iLen >= 128 )
    {
        iRet = EEPROM_Read(MACHINE_SN_ADDR, pbSerialNo, MACHINE_SN_LEN);

        if (iRet == 0)
        {
            int i = 0;
            pbSerialNo[127] = '\0';

            int iLen = strlen(pbSerialNo);

            for (i=0; i<iLen; i++)
            {
                if ( ((pbSerialNo[i] >= 'a') && (pbSerialNo[i] <= 'z'))
                        || ((pbSerialNo[i] >= 'A') && (pbSerialNo[i] <= 'Z'))
                        || ((pbSerialNo[i] >= '0') && (pbSerialNo[i] <= '9'))
                        || (pbSerialNo[i] == '_')
                        || (pbSerialNo[i] == '-')
                        || (pbSerialNo[i] == '+')
                        || (pbSerialNo[i] == '*')
                        || (pbSerialNo[i] == '#') )
                {
                    continue;
                }
                else
                {
                    memcpy(pbSerialNo, "null", 5);
                    break;
                }
            }
        }
    }

    return iRet;
}

// 获取复位次数(长度：4字节)
int GetResetCount(int* piResetCount)
{
    return EEPROM_Read(RESET_COUNTER_ADDR, (BYTE8*)piResetCount, 4);
}

// 清零“复位次数”
int EmptyResetCount()
{
    unsigned long data = 0;
    return EEPROM_Write(RESET_COUNTER_ADDR, (u8*)&data, 4);
}

/* 数据结构说明:修改自实时时间结构体，将第4个成员Uint16最高3位作为复位类型使用. */
typedef struct
{
    Uint16 wYear;    //年数.
    Uint16 wMonth;   //月数.
    Uint16 wDay;     //号数.
    Uint16 wResetType; //复位类型（最高3位）
    Uint16 wHour;    //小数数,24小时制.
    Uint16 wMinute;  //小时数.
    Uint16 wSecond;  //秒数.
    Uint16 wMSecond; //毫秒数.
} RESET_RECORD_STRUCT;

int ReadResetReport(char* szResetReport, const int iLen)
{
    static BYTE8 szStartTimeTable[1024];

    int i = 0;
    int iResetCount = 0;
    int iStartIndex = 0;
    int iCount = 0;
    int iDestLen = 0;
    int iTempLen = 0;
    char szReportTmp[100];
    RESET_RECORD_STRUCT cRealTime;

    if ( NULL == szResetReport )
    {
        HV_Trace(5, "<ReadResetReport> szResetReport is NULL!\n");
        return -1;
    }
    // 填写记录报告头
    strcpy(szResetReport, "");
    if ( GetSN(szResetReport, iLen) < 0 )
    {
        HV_Trace(5, "<ReadResetReport> GetSN is FAILED!\n");
        return -1;
    }
    strcat(szResetReport, "\nResetRecordReport:\n");
    iDestLen = strlen(szResetReport);

    if ( GetResetCount(&iResetCount) < 0 )
    {
        HV_Trace(5, "<ReadResetReport> GetResetCount is FAILED!\n");
        return -1;
    }
    else
    {
        if ( iResetCount > 0 )
        {
            if ( iResetCount <= 64 )
            {
                iStartIndex = 0;
                iCount = iResetCount;
            }
            else
            {
                iStartIndex = iResetCount % 64;
                iCount = 64;
            }

            if ( EEPROM_Read(START_TIME_TABLE_ADDR, szStartTimeTable, iCount*sizeof(RESET_RECORD_STRUCT)) < 0 )
            {
                HV_Trace(5, "<ReadResetReport> EEPROM_Read is FAILED!\n");
                return -1;
            }

            for ( i = (iCount - 1); i >= 0; --i )
            {
                memcpy(&cRealTime,
                       szStartTimeTable + ((iStartIndex + i) % 64) * sizeof(RESET_RECORD_STRUCT),
                       sizeof(RESET_RECORD_STRUCT));

                sprintf(szReportTmp, "%08d :%04d/%02d/%02d %02d:%02d:%02d T:%02d\n",
                        (int)(iResetCount-iCount+1 + i),
                        (int)cRealTime.wYear,
                        (int)cRealTime.wMonth,
                        (int)cRealTime.wDay,
                        (int)cRealTime.wHour,
                        (int)cRealTime.wMinute,
                        (int)cRealTime.wSecond,
                        (int)(cRealTime.wResetType>>13));

                iTempLen = strlen(szReportTmp);
                if ( (iDestLen + iTempLen) < iLen )
                {
                    strcat(szResetReport, szReportTmp);
                    iDestLen += iTempLen;
                }
                else
                {
                    HV_Trace(5, "<ReadResetReport> iLen not long enough.\n");
                    break;
                }
            }
        }
        else
        {
            HV_Trace(5, "<ReadResetReport> ResetCount is Zero.\n");
        }

        return 0;
    }
}

int WriteResetReport(const int iType)
{
    int iResetCount = 0;
    int iWriteIndex = 0;
    static time_t t = 0;
    static struct tm* tt = NULL;
    RESET_RECORD_STRUCT cRealTime;

    if ( GetResetCount(&iResetCount) < 0 )
    {
        HV_Trace(5, "<ReadResetReport> GetResetCount is FAILED!\n");
        return -1;
    }
    else
    {
        if ( iResetCount >= 0 )
        {
            t = time(NULL);
            tt = gmtime(&t);

            if ( iResetCount > 63 )
            {
                iWriteIndex = iResetCount % 64;
            }

            cRealTime.wYear     = tt->tm_year + 1900;
            cRealTime.wMonth    = tt->tm_mon + 1;
            cRealTime.wDay      = tt->tm_mday;
            cRealTime.wHour     = tt->tm_hour;
            cRealTime.wMinute   = tt->tm_min;
            cRealTime.wSecond   = tt->tm_sec;

            cRealTime.wResetType = (iType&0x07)<<13;

            if ( EEPROM_Write(START_TIME_TABLE_ADDR+iWriteIndex, (Uint8*)&cRealTime, sizeof(RESET_RECORD_STRUCT)) < 0 )
            {
                HV_Trace(5, "<ReadResetReport> EEPROM_Read is FAILED!\n");
                return -1;
            }
        }
        return 0;
    }
}

// 设置Uboot网络地址（网络字节序）
int SetUbootNetAddr(DWORD32 dwIP, DWORD32 dwNetmask, DWORD32 dwGateway)
{
    if ( 0 == EEPROM_Write(IP_ADDR, (u8*)&dwIP, 4)
            && 0 == EEPROM_Write(GATEWAY_ADDR, (u8*)&dwGateway, 4)
            && 0 == EEPROM_Write(NETMASK_ADDR, (u8*)&dwNetmask, 4) )
    {
        return 0;
    }
    return -1;
}

// 获取Uboot网络地址（网络字节序）
int GetUbootNetAddr(DWORD32* pdwIP, DWORD32* pdwNetmask, DWORD32* pdwGateway)
{
    if ( 0 == EEPROM_Read(IP_ADDR, (BYTE8*)pdwIP, 4)
            && 0 == EEPROM_Read(GATEWAY_ADDR, (BYTE8*)pdwGateway, 4)
            && 0 == EEPROM_Read(NETMASK_ADDR, (BYTE8*)pdwNetmask, 4) )
    {
        return 0;
    }
    return -1;
}

// 是否测试相机网口
int IsTestCamLanPort()
{
    static int iTestPort = -1;

    if (iTestPort != -1)
    {
        return iTestPort;
    }

    char chRet = 0;
    DWORD32 dwFlag = 0;

    if ( 0 == EEPROM_Read(LAN_PORT_TEST_FALG_ADDR, (BYTE8*)&dwFlag, 4) )
    {
        // 为0时不测试，为1时测试，其它值时更正为0
        if (dwFlag == 1)
        {
            chRet = 1;
        }
        else if (dwFlag != 0)
        {
            dwFlag = 0;
            EEPROM_Write(LAN_PORT_TEST_FALG_ADDR, (BYTE8*)&dwFlag, 4);
        }
    }

    iTestPort = chRet;
    HV_Trace(5, "TestCamLanPort Mode [%d].\n", iTestPort);

    return chRet;
}

// 读AD采样点
int EprGetADSamplingValue(int* piValueA, int* piValueB)
{
    struct start_status_table_t table;
    if (ReadStartStatusTable(&table) < 0)
    {
        return -1;
    }

    *piValueA = table.camera_sample_point_left;
    *piValueB = table.camera_sample_point_right;

    return 0;
}

// 设置AD采样点
int EprSetADSamplingValue(int iValueA, int iValueB)
{
    struct start_status_table_t table;
    if (ReadStartStatusTable(&table) < 0)
    {
        return -1;
    }

    table.camera_sample_point_left = iValueA;
    table.camera_sample_point_right = iValueB;

    if (WriteStartStatusTable(&table) < 0)
    {
        return -1;
    }

    return 0;
}

// ------------------------------------------------------
static struct start_status_table_t g_start_status_table;
static int g_is_read_start_status_table = 0;

static int _read_start_tatus_table(struct start_status_table_t* pTable, int is_backup_table)
{
    HRESULT ret;
    struct start_status_table_t *start_status;
    u32 crc1,crc2;
    u32 eeprom_addr;

    start_status = pTable;
    if (is_backup_table)
    {
        eeprom_addr = BACKUP_START_STATUS_TABLE_ADDR;
        printf("backup table ");
    }
    else
    {
        eeprom_addr = START_STATUS_TABLE_ADDR;
        printf("master table ");
    }

    ret = EEPROM_Read(eeprom_addr,  (u8 *)start_status,  sizeof(struct start_status_table_t));
    if (ret != S_OK)
    {
        // 读失败就再读一次
        printf("fail ");
        ret = EEPROM_Read(eeprom_addr,  (u8 *)start_status,  sizeof(struct start_status_table_t));
        if (ret != S_OK)
        {
            // 再次失败返回错
            printf("fail\n");
            return -1;
        }
    }
    printf("succ ");

    if (start_status->flags != START_STATUS_TABLE_FLAGS_IS_OK)
    {
        // 标志位非OK
        printf("flags error\n");
        return -2;
    }

    printf("start table version %02x\n",start_status->ver);
    /* 读版本号*/
    if ( (start_status->ver != START_STATUS_TABLE_VER_1109)
            && (start_status->ver != START_STATUS_TABLE_VER_1201) )
    {
        printf("ver error\n");
        return -5;
    }
    if ( start_status->ver == START_STATUS_TABLE_VER_1109 )
    {
        crc1 = CalcFastCrc32(0, (u8*)start_status, sizeof(struct start_status_table_t)-sizeof(u32));
        if (crc1 != start_status->crc1)
        {
            // CRC不相等
            printf("crc32 error\n");
            return -3;
        }
    }
    else  if ( start_status->ver == START_STATUS_TABLE_VER_1201 )
    {
        // 新版本号 都校验
        crc1 = CalcFastCrc32(0, (u8*)start_status, sizeof(struct start_status_table_t)-(12*sizeof(u32)));
        crc2 = CalcFastCrc32(0, (u8*)start_status, sizeof(struct start_status_table_t)-sizeof(u32));
        if ((crc1 != start_status->crc1) || (crc2 != start_status->crc2))
        {
            // CRC不相等
            printf("crc32 error\n");
            return -3;
        }
    }

    return 0;
}

static int _write_start_status_table(struct start_status_table_t* pTable, int is_backup_table)
{
    HRESULT ret;
    struct start_status_table_t *start_status;
    u32 eeprom_addr;

    start_status = pTable;
    if (is_backup_table)
    {
        eeprom_addr = BACKUP_START_STATUS_TABLE_ADDR;
        printf("backup table ");
    }
    else
    {
        eeprom_addr = START_STATUS_TABLE_ADDR;
        printf("master table ");
    }

    start_status->crc1 = CalcFastCrc32(0, (u8*)start_status,
                                       sizeof(struct start_status_table_t)-12*sizeof(u32)); // 计算CRC
    start_status->crc2 = CalcFastCrc32(0, (u8*)start_status,
                                       sizeof(struct start_status_table_t)-sizeof(u32)); // 计算CRC

    // 写入
    ret = EEPROM_Write(eeprom_addr, (u8 *)start_status, sizeof(struct start_status_table_t));
    if (ret != S_OK)
    {
        // 失败再试一回
        printf("fail ");
        ret = EEPROM_Write(eeprom_addr, (u8 *)start_status, sizeof(struct start_status_table_t));
        if (ret != S_OK)
        {
            printf("fail\n");
            return -1;
        }
    }

    return 0;
}

static int // 成功返回0，负数为失败
_fix_start_status(
    int is_backup_table // 是否为副表，1为是，0为否
)
{
    struct start_status_table_t table;
    memset(&table, 0, sizeof(struct start_status_table_t));

    _read_start_tatus_table(&table, is_backup_table);

    table.flags = START_STATUS_TABLE_FLAGS_IS_OK;
    table.ver = START_STATUS_TABLE_VER_1201;
    table.status = GET_STATUS_ID_NORMAL;

    if (_write_start_status_table(&table, is_backup_table) < 0)
    {
        return -1;
    }

    return 0;
}

static int ReadStartStatusTable(struct start_status_table_t* pTable)
{
    int iRet;

    if (pTable == NULL)
    {
        return -1;
    }

    if (g_is_read_start_status_table != 0)
    {
        memcpy(pTable, &g_start_status_table, sizeof(struct start_status_table_t));
        return 0;
    }

    iRet = _read_start_tatus_table(pTable, 0);

    if (iRet < 0)
    {
        iRet = _read_start_tatus_table(pTable, 1);
        if (iRet < 0)
        {
            printf("ReadStartStatusTable backup failed.\n");

            if (_fix_start_status(0) < 0)
            {
                printf("ReadStartStatusTable _fix_start_status failed.\n");
            }
            if (_fix_start_status(1) < 0)
            {
                printf("ReadStartStatusTable _fix_start_status backup-table failed.\n");
            }

            iRet = _read_start_tatus_table(pTable, 1);
        }
        else
        {
            if (_write_start_status_table(pTable, 0) < 0)
            {
                printf("ReadStartStatusTable backup-table recover failed.\n");
            }
        }
    }

    if (iRet >= 0)
    {
        g_is_read_start_status_table = 1;
        memcpy(&g_start_status_table, pTable, sizeof(struct start_status_table_t));
    }

    return iRet;
}

static int WriteStartStatusTable(struct start_status_table_t* pTable)
{
    int iRet = _write_start_status_table(pTable, 0);

    if (iRet < 0)
    {
        return iRet;
    }

    iRet = _write_start_status_table(pTable, 1);

    if (iRet < 0)
    {
        return iRet;
    }

    memcpy(&g_start_status_table, pTable, sizeof(struct start_status_table_t));

    return iRet;
}

// 读启动状态信息表
static int // 成功返回正常的启动状态，负数为失败
ReadStartStatus()
{
    struct start_status_table_t table;
    int iRet = ReadStartStatusTable(&table);
    if (iRet < 0)
    {
        return -4;
    }

    switch (table.status)
    {
    case GET_STATUS_ID_UPGRADE: // 升级状态
    case GET_STATUS_ID_TEST: // 测试状态
    case GET_STATUS_ID_AUTOTEST: // 自动化测试状态
    case GET_STATUS_ID_NORMAL: // 正常状态
    case GET_STATUS_ID_UPGRADE_TEST: // 升级测试状态
        break;
    default:
        // 启动状态不对
        printf("status ininval\n");
        return -4;
    }

    printf("status=%08x\n", table.status);

    return table.status;
}

// 写启动状态
static int // 成功返回0，负数为失败
WriteStartStatus(
    int status) // 启动状态
{
    struct start_status_table_t table;

    int iRet = ReadStartStatusTable(&table);
    if (iRet < 0)
    {
        return iRet;
    }

    table.status = status;

    iRet = WriteStartStatusTable(&table);
    if (iRet < 0)
    {
        return iRet;
    }
    return 0;
}

// ------------------------------------------------------

// 获取设备状态
int GetDeviceState(unsigned long* pulStateCode)
{
    int iStatus = ReadStartStatus();

    if ( iStatus < 0 )
    {
        return iStatus;
    }
    else
    {
        if ( pulStateCode )
        {
            *pulStateCode = iStatus;
            return 0;
        }
        else
        {
            return -1;
        }
    }
}

// 设置设备状态
int SetDeviceState(unsigned long ulStateCode)
{
    return WriteStartStatus(ulStateCode);
}

void DmaCopy1D(
    int iSwDevFd,
    PBYTE8 pbDstPhys,
    PBYTE8 pbSrcPhys,
    int iSize
)
{
    SwDmaCopy1D(iSwDevFd, pbDstPhys, pbSrcPhys, iSize);
}

void DmaCopy2D(
    int iSwDevFd,
    PBYTE8 pbDstPhys, int iDstStride,
    PBYTE8 pbSrcPhys, int iSrcStride,
    int iWidth, int iHeight
)
{
    SwDmaCopy2D(
        iSwDevFd,
        pbDstPhys, iDstStride,
        pbSrcPhys, iSrcStride,
        iWidth, iHeight
    );
}
