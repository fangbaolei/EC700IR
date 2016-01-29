#ifndef HV_TARGET_ARM
#define HV_TARGET_ARM
#include "slw_dev.h"
#include "swdrv.h"

//#define SWDEV_NAME "/dev/swdev"
//#define SWPCI_NAME "/dev/swpci"

//////////////////////////////////////////////////////////////////////////
// EEPROM空间规划

// EEPROM保存变量与报表的地址
// 0~1K
#define VERSION_ADDR                    0x000 // 版本号 (4byte)
#define VERSION_LEN                     0x004
#define MACHINE_SN_ADDR                 0x004 // 序列号 (128byte)
#define MACHINE_SN_LEN                  0x080
#define UBOOT_VERSION_ADDR              0x100 // uboot的版本号和编译时间 (128byte)
#define UBOOT_VERSION_LEN               0x080
#define START_STATUS_TABLE_ADDR         0x200 // 启动状态表 (4byte)
#define BACKUP_START_STATUS_TABLE_ADDR  0x300 // 启动状态备份表 (4byte)
// 1~2K
#define RESET_COUNTER_ADDR          0x400 // 复位计数器 (4byte)
#define ERROR_COUNTER_ADDR          0x404 // 启动失败计数器 (4byte)
#define IP_ADDR                     0x40C // IP地址 (4byte)
#define GATEWAY_ADDR                0x410 // 网关地址 (4byte)
#define NETMASK_ADDR                0x414 // 子网掩码 (4byte)
#define UPGRADE_STATUS_ADDR         0x418 // 升级状态 (4byte)
#define LAN_PORT_TEST_FALG_ADDR     0x41C // 相机网口测试标志 (4byte)
// 2~3K
#define START_TIME_TABLE_ADDR 0x800 // 启动时间表 (1Kbyte)
#define START_TIME_TABLE_LEN  0x400
// 3~4K
#define AUTO_TEST_REPORT_ADDR 0xC00 // 自动化测试报表 (64byte)
// 8K~72K 设备参数存储
#define PARAM_START_ADDR 0x2000
#define PARAM_SIZE 0x10000           //64K
// 72K~76K 字符点阵数据存储
#define CHARACTER_START_ADDR    (PARAM_START_ADDR + PARAM_SIZE)
#define CHARACTER_SIZE          0x1000       // 4KB
// 76K~112K 预留
#define RESERVE_START_ADDR      (CHARACTER_START_ADDR + CHARACTER_SIZE)
#define RESERVE_SIZE            0x9000       //36K
// 112K~128K 设备状态记录
#define DEV_STAT_LOG_START_ADDR 0x0001C000 // 黑匣子专用地址块，位于EEPROM尾部
#define DEV_STAT_LOG_SIZE 0x4000    //16K

//////////////////////////////////////////////////////////////////////////

// ------------- 生产升级相关的UDP协议命令 START ------------

// 密码
#define COMMAND_PROTOCOL_PASS 0x20101213

#define GET_STATUS_COMMAND_PROTOCOL_TYPE 0x0000010A // 取状态命令类型
#define GET_STATUS_ID_GET 8 // 取状态

#define GETSN_COMMAND_PROTOCOL_TYPE 0x0000010E // 取序列号命令类型
#define GETSN_ID_GET  0 // 取序列号
#define GETSN_ID_SUCC 1
#define GETSN_ID_FAIL 2

#define GET_NETINFO_COMMAND_PROTOCOL_TYPE 0x00000114 // 读取网络信息，MAC地址，IP地址等
#define GET_NETINFO_GET 0 // 读取
#define GET_NETINFO_GET_SUCC 0 // 读取成功
#define GET_NETINFO_GET_FAIL 1 // 读取失败

#define RESET_COMMAND_PROTOCOL_TYPE 0x0000010B // Reset命令类型
#define RESET_ID_UPGRADE    0 // Reset到升级状态
#define RESET_ID_TEST       1 // RESET到测试状态
#define RESET_ID_AUTOTEST   2 // RESET到自动化测试状态
#define RESET_ID_NORMAL     3 // Reset到正常状态

#define GET_STATUS_ID_UPGRADE       0 // 升级状态
#define GET_STATUS_ID_TEST          1 // 测试状态
#define GET_STATUS_ID_AUTOTEST      2 // 自动化测试状态
#define GET_STATUS_ID_NORMAL        3 // 正常状态
#define GET_STATUS_ID_UPGRADE_TEST  4 // 升级测试状态

// 命令结构
struct command_protocol_t
{
    unsigned long len; // 数据长度（默认12）
    unsigned long type; // 命令类型
    unsigned long id; // 子命令ID
    unsigned long pass; // 密码（默认COMMAND_PROTOCOL_PASS）
};

// ------------- 生产升级相关的UDP协议命令 END ------------

//////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif


// 读取eeprom
// 返回, <0为出错,其它为正确
int EEPROM_Read(
		DWORD32 addr, // 地址
		BYTE8 *data, // 数据
		DWORD32 len // 长度
		);

// 写eeprom
// 返回, <0为出错,其它为正确
int EEPROM_Write(
		 DWORD32 addr, // 地址
		 BYTE8 *data, // 数据
		 DWORD32 len // 长度
		 );
// 喂狗
int WdtHandshake(void);

// 使能看门狗
int EnableWatchDog();

// 点LED
void SetWorkLedOn();
void SetWorkLedOff();
void WorkLedLight();
void SetLan1LedOn();
void SetLan1LedOff();
void Lan1LedLight();
void SetLan2LedOn();
void SetLan2LedOff();
void Lan2LedLight();
void SetHddLedOn();
void SetHddLedOff();
void HddLedLight();
void SetStatLedOn();
void SetStatLedOff();
void StatLedLight();
void SetAllLightOn();
void SetAllLightOff();
void SetAllLightFlash();

// 蜂鸣器
void SetBuzzerOn();
void SetBuzzerOff();

// 安全认证以及加密相关
/* verify.c */
int write_crypt(unsigned int* rgNC);
int verify_crypt(void);
int get_nc_key(unsigned int (*rgdwNC)[4], unsigned int (*prgdwKey)[4]);

// 获取CPLD版本号(长度：1字节)
int GetCPLDVersion(char* pbVerion, int iLen);

// 获取Uboot版本信息(长度：128字节)
int GetUbootVersion(char* pbVerion, int iLen);

// 获取EEPROM保存变量与报表的格式版本号(长度：4字节)
int GetEEPROMInfoFormatVersion(char* pbVerion, int iLen);

// 获取硬件序列号(长度：128字节)
int GetSN(char* pbSerialNo, int iLen);

// 获取复位次数(长度：4字节)
int GetResetCount(int* piResetCount);

// 清零“复位次数”
int EmptyResetCount();

// 获取复位报告
int ReadResetReport(char* szResetReport, const int iLen);

// 写复位报告
int WriteResetReport(const int iType);

// 设置Uboot网络地址（网络字节序）
int SetUbootNetAddr(DWORD32 dwIP, DWORD32 dwNetmask, DWORD32 dwGateway);

// 获取Uboot网络地址（网络字节序）
int GetUbootNetAddr(DWORD32* pdwIP, DWORD32* pdwNetmask, DWORD32* pdwGateway);

// 获取设备状态
int GetDeviceState(unsigned long* pulStateCode);

// 设置设备状态
int SetDeviceState(unsigned long ulStateCode);

// 是否测试相机网口
int IsTestCamLanPort();

// 读AD采样点
int EprGetADSamplingValue(int* piValueA, int* piValueB);

// 设置AD采样点
int EprSetADSamplingValue(int iValueA, int iValueB);

// 1D的DMA拷贝
void DmaCopy1D(
    int iSwDevFd,
    PBYTE8 pbDstPhys,
    PBYTE8 pbSrcPhys,
    int iSize
);

// 2D的DMA拷贝
void DmaCopy2D(
    int iSwDevFd,
    PBYTE8 pbDstPhys, int iDstStride,
    PBYTE8 pbSrcPhys, int iSrcStride,
    int iWidth, int iHeight
);

#ifdef __cplusplus
}
#endif

#endif
