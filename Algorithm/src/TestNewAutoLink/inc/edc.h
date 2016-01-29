/* 
    该文档为EDC设置宏定义和接口声明,EDC顾名思义即为外部设备控制,在该IOM设备驱动程序中是用来作为连接与
协调DSP视频端口与外部视频编解码芯片.
    胡中华于2004年10月15日10:24作出修改.
*/
#ifndef _EDC_H
#define _EDC_H

#include <std.h>                                 
#ifdef __cplusplus
extern "C" {
#endif


/* EDC所支持的控制命令集. */
/*------------------------------------------------------------------------------------------------------------------------------------*/
#define EDC_CONFIG          0x00000001       //config基本命令.
#define EDC_RESET           0x00000002       //reset基本命令.
#define EDC_START           0x00000003       //start基本命令.
#define EDC_STOP            0x00000004       //stop基本命令.
#define EDC_GET_CONFIG      0x00000005       //getconfig基本命令.
#define EDC_GET_STATUS      0x00000006       //getstatus基本命令.
#define EDC_SET_BRIGHT		0x00000007       //setbright基本命令.
#define EDC_GET_BRIGHT		0x00000008       //getbright基本命令.
#define EDC_SET_SATURATION	0x00000009       //setsaturation基本命令.
#define EDC_GET_SATURATION	0x0000000A       //getsaturation基本命令.
#define EDC_SET_USATURATION	0x00000019       //setsaturation基本命令.
#define EDC_GET_USATURATION	0x0000001A       //getsaturation基本命令.
#define EDC_SET_VSATURATION	0x00000039       //setsaturation基本命令.
#define EDC_GET_VSATURATION	0x0000003A       //getsaturation基本命令.
#define EDC_SET_CONTRAST	0x0000000B       //setcontrast基本命令.
#define EDC_GET_CONTRAST	0x0000000C       //getcontrast基本命令.
#define EDC_SET_HUE			0x0000000D       //sethue基本命令.
#define EDC_GET_HUE			0x0000000E       //gethue基本命令.
#define EDC_SET_BRIGHTS		0x0000000F       //设置相关参数值.
#define EDC_CONFIG_DIR		0x00000010       //直接配置AD芯片,按照可用模式.

#define EDC_USER			0x10000000
/*------------------------------------------------------------------------------------------------------------------------------------*/


/* EDC所支持的函数集. */
/*------------------------------------------------------------------------------------------------------------------------------------*/
#define EDC_SUCCESS         0
#define EDC_FAILED          -1

typedef void*  EDC_Handle;

typedef struct EDC_Fxns {
    EDC_Handle  ( *open )( String name, Arg optArg );
    Int ( *close )( EDC_Handle devHandle );
    Int ( *ctrl )( EDC_Handle devHandle, Uns portNum, Uns cmd, Arg arg );
} EDC_Fxns;
/*------------------------------------------------------------------------------------------------------------------------------------*/




#ifdef __cplusplus
}
#endif
#endif
