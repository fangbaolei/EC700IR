#ifndef _DSPSYS_DEF_H
#define _DSPSYS_DEF_H

#include "swBaseType.h"
#include "swWinError.h"
#include "swimage.h"
#include "HvDspTime.h"

//#define _DEBUG_HZH
#define _EEPROM_CRC_CHECK
/*
//_BOOTLOADER_OK 的作用:是否允许设备进入生产模式,如果要进入生产模式,需要将该宏屏蔽
//制作生产测试程序时,需要将_BOOTLOADER_OK屏蔽掉,但编译正常的升级程序时需要将该宏还原回来
//韦开拓增加注释,2011-3-3
*/
#define _BOOTLOADER_OK

/* 宏定义说明:无效句柄和指针常量定义. */
#ifndef INVALID_HANDLE
	#define INVALID_HANDLE 				NULL
#endif

#ifndef INVALID_POINTER
	#define INVALID_POINTER 			NULL
#endif



/* 宏定义说明:用来作为某项功能使能开关,用于配置时使用. */
#ifndef FUNC_ENABLE
	#define FUNC_ENABLE					TRUE
#endif

#ifndef FUNC_DISABLE
	#define FUNC_DISABLE				FALSE
#endif


#endif

