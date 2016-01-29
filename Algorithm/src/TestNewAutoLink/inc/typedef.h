//程序名称：typedef.h
//版 本 号：1.0
//功    能：提供对程序中使用的类型定义。用于简化写法，明确类型
//整 理 人：lrn
//整理时间：2005-03-01


//数据类型定义
#ifndef _TYPEDEF_H
#define _TYPEDEF_H



typedef char             int8;
typedef short            int16;
typedef long             int32;
typedef unsigned char    uint8;
typedef unsigned char    byte;
typedef unsigned short   uint16;
typedef unsigned long    uint32;

//常规使用的输入字串
#ifdef WIN32
    #define PRN    Debug_Var   //变量输出
#else
    #define PRN    printf
#endif

//_DEBUG在TYPEDEF.H中定义，WINDOWS下不用定义，由调试版本决定。
//#define _DEBUG

//以下为调试函数输出宏定义。
#ifdef _DEBUG

#ifdef WIN32
    #define PRND    Debug_Var   //变量输出
#else
    #define PRND    printf
#endif

#define PRNF    Debug_For   //字节数组打印

#else  //_DEBUG

#define PRND      //变量输出
#define PRNF      //字节数组打印
 
#endif

#endif
