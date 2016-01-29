

#ifndef __SLW_AT88SC__
#define __SLW_AT88SC__

#ifdef __cplusplus
extern "C" {
#endif

#include "slw_gpio.h"
#include "slw_gpio2iic.h"

//#define SECURITY_IC_INIT

#define AT88SC_EEPROM_ZONE_SIZE   32
#define AT88SC_EEPROM_ZONE_COUNT  4

#define CMC0	'N'
#define CMC1	'M'
#define CMC2	'L'
#define CMC3	'1'
	
///////这里是用户区的写密码
///////用户区的写密码权利比读密码大~注意
#define USER_ZONE_WPWD1 0x57
#define USER_ZONE_WPWD2 0x68
#define USER_ZONE_WPWD3 0x48
	
#define XXTEA_KEY0	0x20081010
#define XXTEA_KEY1	0x68757A68
#define XXTEA_KEY2	0x00536947
#define XXTEA_KEY3	0x73573031

// 写取熔断阻丝
HRESULT AT88SC_write_fuse(PBYTE8 pbSecureCode);

// 初始化配置
HRESULT AT88SC_init_config(
	PBYTE8 pbSecureCode,
	PBYTE8 pbNc,
	PBYTE8 pbReadOnlyData);

// 初始化EEPROM
HRESULT AT88SC_EEPROM_init();

/*
函数说明:该函数用来从EEPROM中读取一定数据的数据.
函数返回值:
	返回S_OK表示读取EEPROM操作成功.
	返回E_POINTER表示pbMemDst为INVALID_POINTER.
	返回E_INVALIDARG表示传入的参数有错.
	返回E_FAIL表示硬件底层操作出错.
	返回S_FALSE表示读EPROM出错. */
HRESULT AT88SC_EEPROM_crypt_read(
	DWORD32  dwReadPos,     //读取EEPROM的首位置.
	PBYTE8   pbMemDst,      //指向读出的数据.
	DWORD32  dwBytesToRead, //期望读取的数据字节长度.
	PDWORD32 pdwBytesRead); //实际读取的数据字节长度,为NULL则不填.

/*
函数说明:该函数用来向EEPROM中写入一定量数据.
函数返回值:
	返回S_OK表示写入EEPROM操作成功.
	返回E_POINTER表示pbMemSrc为INVALID_POINTER.
	返回E_INVALIDARG表示传入的参数有错.
	返回E_FAIL表示硬件底层操作出错.
	返回S_FALSE表示写入EEPROM出错. */
HRESULT AT88SC_EEPROM_crypt_write(
	const PBYTE8 pbMemSrc,       //指向写入的数据.
	DWORD32      dwWritePos,     //写入EEPROM的首地址.
	DWORD32      dwBytesToWrite, //期望写入的数据字节长度.
	PDWORD32     pdwBytesWrite);  //实际写入的数据字节长度,为NULL则不填.

#ifdef __cplusplus
}
#endif

#endif // __SLW_AT88SC__
