#ifndef _BOOTCTRL_H
#define _BOOTCTRL_H

#include "swbasetype.h"
#include "swWinError.h"

#ifdef __cplusplus
	extern "C" {
#endif	//#ifdef __cplusplus

typedef enum {
	BOOT_DOWNLOAD = 0,
	BOOT_TESTBEGIN = 1,
	BOOT_TESTNEW = 2,
	BOOT_NORMAL = 3,
	BOOT_PRODUCT = 4,
	BOOT_MAX_MODE_NUM = 5
} BootMode;

/* 函数说明:取得启动模式版本号,函数返回0表示操作正确,返回-1表示操作EEPROM失败. */
extern int GetBootConfigVer( WORD16 *pVersion );
/* 函数说明:设置启动模式版本号,函数返回0表示操作正确,返回-1表示操作EEPROM失败. */
extern int SetBootConfigVer( WORD16 wVersion );
/* 函数说明:取得启动配置信息,函数返回0表示操作正确,返回-1表示操作EEPROM失败. */
extern int GetBootMode( BootMode *bootMode, DWORD32 *ptrBootPrg );
/* 函数说明:设置启动配置信息,函数返回0表示操作正确,返回-1表示操作EEPROM失败. */
extern int SetBootMode( BootMode bootMode, DWORD32 dwBootPrg );


#ifdef __cplusplus
	}
#endif	//#ifdef __cplusplus

#endif	//#ifndef _BOOTCTRL_H
