/**
* @file swpa_private.h 
* @brief PlatformAPI层私有的通用数据和函数定义
* 
* - 定义了PlatformAPI层共用的debug宏:__SWPA_DEBUG
* - 定义了PlatformAPI层共用的日志打印接口:SWPA_PRINT()
* 
* @note 该文件只限PlatformAPI层内部使用，不对外公开!
* @copyright Signalway All Rights Reserved
* @author zhouy
* @date 2013-02-27
* @version 1.0
*/


#ifndef _SWPA_PRIVATE_H_
#define _SWPA_PRIVATE_H_ 

#include "swpa.h"
#include <stdio.h>

//定义打印函数
#define SWPA_PRINT(fmt, ...) printf(fmt, ##__VA_ARGS__)

#endif //_SWPA_PRIVATE_H_


