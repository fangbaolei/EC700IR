/**
* @file swpa_string.h 
* @brief 定义字符串操作相关函数
*
* @copyright Signalway All Rights Reserved
* @author zhouy
* @date 2013-02-27
* @version 1.0
*/

#ifndef _SWPA_MATH_H_
#define _SWPA_MATH_H_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef __cplusplus
extern "C"
{
#endif


///求整型的绝对值
#define swpa_abs		abs


///求实型的绝对值
#define swpa_fabs	  fabs


///取下整
#define swpa_floor		floor


///取上整
#define swpa_ceil		ceil


///计算以e为底的对数
#define swpa_log 	log 


///计算以10为底的对数
#define swpa_log10		log10


///计算幂
#define swpa_pow		pow


///开平方
#define swpa_sqrt		sqrt

#define swpa_exp    exp

///计算最大最小值宏
#define swpa_min(a, b) ((a) < (b) ? (a) : (b))
#define swpa_max(a, b) ((a) > (b) ? (a) : (b))

#ifdef __cplusplus
}
#endif


#endif //_SWPA_MATH_H_


