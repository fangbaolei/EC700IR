/**
* @file swpa_string.h 
* @brief 定义字符串操作相关函数
*
* @copyright Signalway All Rights Reserved
* @author zhouy
* @date 2013-02-27
* @version 1.0
*/

#ifndef _SWPA_STRING_H_
#define _SWPA_STRING_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C"
{
#endif


///字符串比较函数
#define swpa_strcmp		strcmp


///限定长度的字符串比较函数
#define swpa_strncmp	strncmp


///查找字符子串
#define swpa_strstr		strstr


///字符串拷贝
#define swpa_strcpy		strcpy


///定长字符串拷贝
#define swpa_strncpy	strncpy


///获取字符串长度
#define swpa_strlen		strlen


///字符串拼接
#define swpa_strcat		strcat


///在一个串中查找给定字符的第一个匹配之处
#define swpa_strchr		strchr


///将一个串中的一部分与另一个串比较, 不管大小写
#define swpa_strnicmp	strncasecmp


///以大小写不敏感方式比较两个串
#define swpa_stricmp	strcasecmp


///将一个串中的所有字符都设为指定字符
#define swpa_strset		strset


///将一个串中的前n个字符都设为指定字符
#define swpa_strnset	strnset




///从字符串读取数据
#define swpa_sscanf		sscanf


///打印数据到字符串
#define swpa_sprintf	sprintf
#define swpa_vsprintf vsprintf


///打印数据到定长字符串
#define swpa_snprintf	snprintf




///字符串转整型
#define swpa_atoi	atoi


///字符串转长整型
#define swpa_atol	atol


///字符串转浮点型
#define swpa_atof	atof

#define swpa_va_list  va_list
#define swpa_va_start va_start
#define swpa_va_end   va_end
#define swpa_va_arg   va_arg

/* BEGIN: Modified by huangdch, 2015/8/20 
*字符串解析*/
#define swpa_strtok_r  strtok_r

#ifdef __cplusplus
}
#endif


#endif //_SWPA_STRING_H_


