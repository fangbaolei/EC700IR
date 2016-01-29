#ifndef _DSPSYS_DEF_H
#define _DSPSYS_DEF_H

#include "swbasetype.h"
#include "swwinerror.h"
#include "swimage.h"
#include "HvDspTime.h"

//#define _DEBUG_HZH
#define _EEPROM_CRC_CHECK
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

