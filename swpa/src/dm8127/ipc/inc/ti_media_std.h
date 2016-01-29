/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#if !defined(STD_H)
#define STD_H

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>


#if defined (__cplusplus)
extern "C" {
#endif

typedef char              Char;
typedef unsigned char     UChar;
typedef short             Short;
typedef unsigned short    UShort;
typedef int               Int;
typedef unsigned int      UInt;
typedef long              Long;
typedef unsigned long     ULong;
typedef float             Float;
typedef double            Double;
typedef long double       LDouble;
typedef void              Void;


typedef unsigned short    Bool;
typedef void            * Ptr;       /* data pointer */
typedef char            * String;    /* null terminated string */


typedef int            *  IArg;
typedef unsigned int   *  UArg;
typedef char              Int8;
typedef short             Int16;
typedef int               Int32;

typedef unsigned char     UInt8;
typedef unsigned short    UInt16;
typedef unsigned int      UInt32;
typedef unsigned int      SizeT;
typedef unsigned char     Bits8;
typedef unsigned short    Bits16;
typedef UInt32            Bits32;
typedef unsigned long long UInt64;
#ifndef TRUE
#define TRUE              1
#endif

#ifndef FALSE
#define FALSE             0
#endif

/*! Data type for errors */
typedef UInt32            Error_Block;

/*! Initialize error block */
#define Error_init(eb) *eb = 0


#if defined (__cplusplus)
}
#endif

#endif

