

#ifndef __SLW_I2C_DEV__
#define __SLW_I2C_DEV__

#ifdef __cplusplus
extern "C" {
#endif

#include "slw_gpio.h"
#include "slw_gpio2iic.h"

#define EEPROM_AT24C1024_MAX_SIZE     0x20000
#define EEPROM_AT24C1024_SECTION_SIZE 0x10000
#define EEPROM_AT24C1024_PAGE_SIZE    256

/*------------------RTC--------------------*/
// RTC访问起始地址常量定义.
#define RTC_TIME_START_ADDR 0x00 //RTC芯片中时间寄存器的起始地址.
#define RTC_INTOUT_ADDR     0x0E //RTC芯片中INT输出寄存器的起始地址.
#define RTC_TIME_LEN        7    //RTC芯片中用来放置时间所占用寄存器的个数.
#define RTC_INTOUT_LEN      1    //RTC芯片中用来配置中断输出的寄存器个数.

#define Bcd2Hex(bSrcData) (((bSrcData) >> 4) * 10 + ((bSrcData) & 0xF))
#define Hex2Bcd(bSrcData)	((((bSrcData) / 10) << 4) + ((bSrcData) % 10))

/* 枚举说明:该枚举为RTC时钟中断输出的时钟周期,
            因为1.0版本看门狗的工作时钟为该时钟,
            其基本工作时钟频率为32K*/
typedef enum {
	RTC_INT_1HZ   = 0,
	RTC_INT_4KHZ  = 1,
	RTC_INT_8KHZ  = 2,
	RTC_INT_32KHZ = 3
} RTC_INT_FREQUENCY;

/* 数据结构说明:用来定义实时时间. */
//typedef struct {
//	Uint16 wYear;    //年数.
//	Uint16 wMonth;   //月数.
//	Uint16 wDay;     //号数.
//	Uint16 wWeekNum; //当前日期的星期数.
//	Uint16 wHour;    //小数数,24小时制.
//	Uint16 wMinute;  //小时数.
//	Uint16 wSecond;  //秒数.
//	Uint16 wMSecond; //毫秒数.
//} REAL_TIME_STRUCT;


/*------------------LM75 -------------------*/
#define LM75_Temp_START_ADDR 0x00

typedef struct{
	Uint8   TempPolar; //温度的正负极性 0: +  ,1: - ;
	Uint16  Temp;      //温度绝对值
}TEMP_STRUCT;


/*
函数说明:该函数用来从EEPROM中读取一定数据的数据.
函数返回值:
	返回S_OK表示读取EEPROM操作成功.
	返回E_POINTER表示pbMemDst为INVALID_POINTER.
	返回E_INVALIDARG表示传入的参数有错.
	返回E_FAIL表示硬件底层操作出错.
	返回S_FALSE表示读取EEPROM出错. */
HRESULT EEPROM_ReadSection(
	DWORD32  dwReadPos,	    // 读取EEPROM的首位置.
	PBYTE8   pbMemDst, 	    // 指向读出的数据.
	DWORD32  dwBytesToRead,	// 期望读取的数据字节长度.
	PDWORD32 pdwBytesRead,  // 实际读取的数据字节长为NULL则不填
	BYTE8    bDeviceAddr);  // 设备地址

/*
函数说明:该函数用来向EEPROM中写入一定量数据.
函数返回值:
	返回S_OK表示写入EEPROM操作成功.
	返回E_POINTER表示pbMemSrc为INVALID_POINTER.
	返回E_INVALIDARG表示传入的参数有错.
	返回E_FAIL表示硬件底层操作出错.
	返回S_FALSE表示写入EEPROM出错. */
HRESULT EEPROM_WriteSection(
	const PBYTE8 pbMemSrc, 		  //指向写入的数据.
	DWORD32      dwWritePos, 	  //写入EEPROM的首地址.
	DWORD32      dwBytesToWrite,//期望写入的数据字节长度.
	PDWORD32     pdwBytesWrite,	//实际写入的数据字节长度,为NULL则不填
	BYTE8        bDeviceAddr);   // 设备地址

/*
函数说明:该函数用来从EEPROM中读取一定数据长度
函数返回值:
	返回S_OK表示读取EEPROM操作成功.
	返回E_POINTER表示pbMemDst为INVALID_POINTER.
	返回E_INVALIDARG表示传入的参数有错.
	返回E_FAIL表示硬件底层操作出错.
	返回S_FALSE表示读取EEPROM出错. */
//HRESULT EEPROM_Read(
//	DWORD32  dwReadPos,		//读取EEPROM的首位置.
//	PBYTE8   pbMemDst, 		//指向读出的数据.
//	DWORD32  dwBytesToRead,	//期望读取的数据字节长度.
//	PDWORD32 pdwBytesRead);	//实际读取的数据字节长度,为NULL则不填.

/*
函数说明:该函数用来从EEPROM中读取一定数据的数据.
函数返回值:
	返回S_OK表示读取EEPROM操作成功.
	返回E_POINTER表示pbMemDst为INVALID_POINTER.
	返回E_INVALIDARG表示传入的参数有错.
	返回E_FAIL表示硬件底层操作出错.
	返回S_FALSE表示读取EEPROM出错. */
//HRESULT EEPROM_Write(
//	const PBYTE8 pbMemSrc, 		//指向写入的数据.
//	DWORD32      dwWritePos, 	//写入EEPROM的首地址.
//	DWORD32      dwBytesToWrite,	//期望写入的数据字节长度.
//	PDWORD32     pdwBytesWrite);	//实际写入的数据字节长度,为NULL则不填.

/*
函数说明:该函数用来读取实时时钟芯片中的实时时间值(含日期),精确到秒,但是当前的实时时钟芯片年误差为100分钟左右.
函数返:
	返回S_OK表示读取RTC日期时间成功.
	返回E_POINTER表示pRealTime为INVALID_POINTER.
	返回E_FAIL表示硬件撞悴僮鞒龃?
	返回S_FALSE表示读取RTC日期时间出错. */
//HRESULT RTC_GetDateTime(
//	REAL_TIME_STRUCT *pRealTime);		//指向读出的时间.

/*
函数说明:该函数用来设置实时时钟芯片时间(包含日期),精确到秒.其数据串的各字节意义见上描述.根据用户所设置的数据
	写入至RTC相关时间日期寄存器中.
函数返回值:
	返回S_OK表示设置RTC日期时间成功.
	返回E_POINTER表示pbTimeVal为INVALID_POINTER.
	返回E_INVALIDARG表示传入的实时时间数据非法.
	返回E_FAIL表示硬件底层操作出错.
	返回S_FALSE表示设置RTC日期时间出错. */
//HRESULT RTC_SetDateTime(
//	const REAL_TIME_STRUCT *pRealTime); //指向设置的时间.

/*
函数说明:该函数用来初始化时钟的中断输出时钟频率.
函数返回值:
	返回S_OK表示操作成功.
	返回E_FAIL表示操作出现系统级异常.
	返回S_FALSE表示操作失败. */
HRESULT InitRTCInt(BYTE8 IntFrequency);

/*
函数说明:该函数用来向温度传感器LM75读取实时温度
	温度数据9位，存放在LM75的16位TEMP寄存器中，
函数返回值:
	返回S_OK表示读取温度值成功.
	返回E_POINTER表示pTemp为INVALID_POINTER.
	返回E_FAIL表示硬件底层操作出错.
	返回S_FALSE表示读取的温度值出错. */
HRESULT LM75_GetTempData(TEMP_STRUCT *pTemp);

//////////////////// DS2460 加密芯片 ////////////////////
// address
#define DS2460_MEM_SHA      0x00
#define DS2460_MEM_MAC      0x40
#define DS2460_MEM_SSECRET  0x54
#define DS2460_MEM_CMD      0x5C
#define DS2460_MEM_MATCHMAC 0x5E
#define DS2460_MEM_ESECRET1 0x60
#define DS2460_MEM_ESECRET2 0x68
#define DS2460_MEM_ESECRET3 0x70
#define DS2460_MEM_EEPROM   0x80
#define DS2460_MEM_SERIAL   0xF0

// command
#define DS2460_CMD_COMPUTE  0x80
#define DS2460_CMD_TXSECRET 0x40

// 初始化
HRESULT DS2460_init();
HRESULT DS2460_ReadSerialNumber(BYTE8* buffer, int len);
HRESULT DS2460_ComputeSHA(BYTE8 gpSHA, BYTE8 srcSecret, BYTE8 toMACBuffer);
HRESULT DS2460_MatchMAC(BYTE8* buffer, int len);
HRESULT DS2460_ReadMAC(BYTE8* buffer, int len);
HRESULT DS2460_WriteESecret1(BYTE8* buffer, int len);
HRESULT DS2460_WriteESecret2(BYTE8* buffer, int len);
HRESULT DS2460_WriteESecret3(BYTE8* buffer, int len);

HRESULT DS2460_WriteInputSHA(BYTE8* buffer, int len);
HRESULT DS2460_ReadInputSHA(BYTE8* buffer, int len);
HRESULT DS2460_WriteMAC(BYTE8* buffer, int len);
HRESULT DS2460_TransferSecret(BYTE8 dstSecret);
HRESULT DS2460_WriteSSecret(BYTE8* buffer, int len);

void ComputeSHAEE(unsigned char *MT,long *A,long *B, long *C, long *D,long *E);

#ifdef __cplusplus
}
#endif

#endif // __SLW_I2C_DEV__
