/**
* @file swpa.h 
* @brief PlatformAPI层对外开放的数据定义
* 
* - 定义了PlatformAPI层通用的返回值
* 
* @copyright Signalway All Rights Reserved
* @author zhouy
* @date 2013-02-27
* @version 1.0
*/


#ifndef _SWPA_H_
#define _SWPA_H_ 



//-------------------- _XOPEN_SOURCE ------------------------//
///必须保证_XOPEN_SOURCE有被define且被define成500，否则涉及POSIX的相关文件在arm_v5t_le-gcc中编译出错！！
#ifndef _XOPEN_SOURCE

  #define _XOPEN_SOURCE 500

#elif 500 != _XOPEN_SOURCE

  #undef _XOPEN_SOURCE
  #define _XOPEN_SOURCE 500
  
#endif  


//版本号
#include "swpa_version.h"
//OS Lib相关
#include "swpa_datetime.h"
#include "swpa_mem.h"
#include "swpa_sem.h"
#include "swpa_socket.h"
#include "swpa_tcpip.h"
#include "swpa_thread.h"
#include "swpa_timer.h"
#include "swpa_hdd.h"
#include "swpa_string.h"
#include "swpa_utils.h"
#include "swpa_list.h"
#include "swpa_map.h"
#include "swpa_math.h"

//Driver Lib相关
#include "swpa_device.h"
#include "swpa_file.h"
#include "swpa_dma.h"
#include "swpa_device_setting.h"

//相机相关
#include "swpa_camera.h"
#include "swpa_vpif.h"
#include "swpa_ipnc.h"

#ifdef __cplusplus
extern "C"
{
#endif

//返回值定义
///成功
#define SWPAR_OK 				( 0)

///失败
#define SWPAR_FAIL 				(-1)

///非法参数
#define SWPAR_INVALIDARG		(-2)

///没有实现
#define SWPAR_NOTIMPL			(-3)

///内存不足
#define SWPAR_OUTOFMEMORY 		(-4)

///没有初始化
#define SWPAR_NOTINITED			(-5)

///操作超时
#define SWPAR_OUTOFTIME			(-6)



// 最小错误值定义。
#define SWPAR_MIN				(-10)



#ifdef __cplusplus
}
#endif

#endif //_SWPA_H_



