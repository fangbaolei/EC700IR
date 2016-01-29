#ifndef _HV_SYS_ERR_H
#define _HV_SYS_ERR_H

#include "interface.h"

#ifdef __cplusplus
extern "C" {
#endif	//#ifdef __cplusplus

#define SYS_ERR_LIST(MACRO_NAME)			\
	MACRO_NAME(IDICRD, "IC读")				\
   	MACRO_NAME(I2C, "I2C操作")			    \
   	MACRO_NAME(FALSH, "FLASH操作")			\
    MACRO_NAME(PARAM, "PARAM参数")		    \
	MACRO_NAME(MALLOC, "MEM ALLOC")			\
	MACRO_NAME(MFREE, "MEM FREE")			\
	MACRO_NAME(INIT, "初始化" )				\
	MACRO_NAME(RUNNING, "运行时")			\
	MACRO_NAME(SONE, "S0")				    \
	MACRO_NAME(STWO, "S1")				    \
	MACRO_NAME(STHREE, "S2")			    \
	MACRO_NAME(MONE, "M0")				    \
	MACRO_NAME(MTWO, "M1")				    \
	MACRO_NAME(MTHREE, "M2")

#define DEFINE_ERR_ENUM(a, b) E_##a,

typedef enum {
	SYS_ERR_LIST(DEFINE_ERR_ENUM)
	ERR_Max_Num
} SysErrType;

/* 函数说明:该函数用于系统输出字符,输出至调试端口(COM串口). */
extern int HV_Printf(
	LPCSTR 			format,     // 指向输出数据格式.
	...                         // 其他参数.
);

extern int HVDG_Printf( LPCSTR format, ... );

/* 函数说明:该函数用于系统输入,输入端口为COM串口. */
extern int HV_Scanf( 
	LPCSTR 			format,     // 指向输入数据格式.
	void 			*pvData,    // 指向取得的数据.
	char 			chBreak     // 输入中止字符.
);

/* 函数说明:该函数用于输入密码字符串 */
extern int HV_PWD_Scanf( 
            LPCSTR  format, 
            void    *pvData,
            int     nMaxPWDLenght, 
            char    chBreak
);

/* 函数说明:该函数用来处理系统可知崩溃级的错误事件,期望系统直接退出运行,中止所有的操作. */
extern void HV_Abort(
	LPCSTR			pszString,  // 指向信息字符串.
	int             iVal        // 函数返回值,或者其他信息.
);

/* 函数说明:该函数用来处理系统无法正常运行时的错误事件,期望系统退出运行,有必要进行一些处理工作. */
extern void HV_Exit(
    LPCSTR          pszString,  // 指向信息字符串.
    int             iExitFlag,  // 退出的标志指示.    
    int             iVal        // 函数返回值,或者其他信息.
);

/* 函数说明:该函数用来处理系统可知崩溃级的错误事件,期望系统直接退出运行,中止所有的操作. */
extern void HVDG_Abort(
	LPCSTR			pszString,  // 指向信息字符串.
	int             iVal        // 函数返回值,或者其他信息.
);

/* 函数说明:该函数用来处理系统无法正常运行时的错误事件,期望系统退出运行,有必要进行一些处理工作. */
extern void HVDG_Exit(
    LPCSTR          pszString,  // 指向信息字符串.
    int             iExitFlag,  // 退出的标志指示.    
    int             iVal        // 函数返回值,或者其他信息.
);

/* 函数说明:该函数用来处理系统错误事件,出现该类错误事件时系统会作出如何反应则根据需求不同而有所不同. */
extern int HV_Error(
	int             ErrType,    // 错误类型.
	LPCSTR			pszString,  // 指向信息字符串.
    int             iVal        // 函数返回值,或者其他信息.
);

extern BOOL g_fVpProcess[3];
extern BOOL g_fEthernetConnect[3];
extern BOOL g_fInputView;

#ifdef __cplusplus
}
#endif	//#ifdef __cplusplus

#endif	//#ifndef _HV_SYS_ERR_H

