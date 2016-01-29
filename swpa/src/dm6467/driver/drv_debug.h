/**
 * @file   drv_debug.h
 * @author lijj
 * @date   2013-02-28
 *
 * @brief  调试宏
 *
 * @note   在包含本文件前必须定义_DRV_DEBUG_
 *         1、DRV_PRINT：打印，用于跟踪代码执行流程
 *         2、DRV_ERR：打印，用于提示错误信息
 *         3、DRV_DEBUG：可选打印，用于调试
 *
 * @todo
 *         1、为方便调试，在所有打印信息中加上函数名，正式使用时，是否应去掉??
 *
 */
#ifndef DRV_DEBUG_H
#define DRV_DEBUG_H

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "drv_errno.h"

#ifdef __cplusplus
extern "C"
{
#endif

/// 打印宏
//#define DRV_PRINT(fmt, ...) printf(fmt, ##__VA_ARGS__)

// 添加提示符前缀
#define DRV_PRINT(fmt, ...) printf("[%s @ %s]: " fmt, __func__, "drvlib", ##__VA_ARGS__)

/// debug调试信息，需要定义_DRV_DEBUG_宏
#ifdef _DRV_DEBUG_
#define DRV_DEBUG(fmt, ...) printf("[DEBUG %s().%d @ %s]: " fmt, __func__, __LINE__, "drvlib", ##__VA_ARGS__)
#else
#define DRV_DEBUG(fmt, ...)
#endif

////////////////////////////////////////////////////////////////////////////

/// 打印错误信息
#define DRV_ERR(fmt, ...) printf("[%s().%d @ %s]: " fmt, __func__, __LINE__, "drvlib", ##__VA_ARGS__)

/// 打印系统调用错误信息(只能在系统调用出错后使用)
#define DRV_SYSERR(fmt, ...) printf("[%s().%d @ %s | ERR msg: %s]: " fmt, __func__, __LINE__, "drvlib", strerror(errno), ##__VA_ARGS__)

////////////////////////////////////////////////////////////////////////////
/// 定位函数
#ifndef DEBUG_MARK
#define DEBUG_MARK printf("================%s %d\n", __func__, __LINE__);
#endif

///打印单个int 变量的值
#define DRV_PRINT_INT(int_var) 		DRV_PRINT("%s = %d\n", #int_var, int_var)

///打印单个unsigned int 变量的值
#define DRV_PRINT_UINT(uint_var)    DRV_PRINT("%s = %d\n", #uint_var, uint_var)

///打印单个long 变量的值
#define DRV_PRINT_LONG(long_var)    DRV_PRINT("%s = %d\n", #long_var, long_var)

///打印单个unsigned long 变量的值
#define DRV_PRINT_ULONG(ulong_var) 	DRV_PRINT("%s = %d\n", #ulong_var, ulong_var)

///打印单个float 变量的值
#define DRV_PRINT_FLOAT(float_var) 	DRV_PRINT("%s = %f\n", #float_var, float_var)

///打印单个char 变量的值
#define DRV_PRINT_CHAR(char_var) 	DRV_PRINT("%s = %c\n", #char_var, char_var)

///打印单个char* 变量的值
#define DRV_PRINT_STRING(string_var)    DRV_PRINT("%s = %s\n", #string_var, string_var)

///////////////////////////////////////////////////////////
// 开启_DRV_CHECK_ARG_宏则会提示参数错误信息

/// 打印参数合法性判断信息，需要定义_DRV_CHECK_ARG_宏
#ifdef _DRV_CHECK_ARG_
#define DRV_ARG_DEBUG(fmt, ...) DRV_PRINT(fmt, ##__VA_ARGS__)
#else
#define DRV_ARG_DEBUG(fmt, ...)
#endif

/// 检查参数
#define DRV_ARG_CHECK(x) \
do {\
if (!(x)) \
{ \
	DRV_ARG_DEBUG("%s().%d Check Arg failed: [%s].\n",__func__, __LINE__, #x);\
    return DRV_INVALIDARG;  \
}\
}while (0)


#ifdef __cplusplus
}
#endif

#endif // DRV_DEBUG_H
