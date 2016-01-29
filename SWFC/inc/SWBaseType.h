#ifndef _SWBASETYPE_INCLUDED_H_
#define _SWBASETYPE_INCLUDED_H_

#include "SWConfig.h"

#ifndef NULL
#define NULL    0
#endif

#ifndef TRUE
#define TRUE    1
#endif

#ifndef FALSE
#define FALSE   0
#endif

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

#if ( RUN_PLATFORM == PLATFORM_WINDOWS )

#include <WinDef.h>

#else

typedef unsigned char BYTE, *PBYTE, *LPBYTE;
typedef char CHAR;
typedef char *PSTR, *LPSTR;
typedef const char *PCSTR, *LPCSTR;
typedef unsigned short WCHAR;
typedef WCHAR *PWCHAR;
typedef WCHAR *LPWCH, *PWCH;
typedef const WCHAR *LPCWCH, *PCWCH;
typedef WCHAR *LPWSTR, *PWSTR;
typedef const WCHAR *LPCWSTR, *PCWSTR;

typedef unsigned short WORD, *PWORD, *LPWORD;
typedef short SHORT, *PSHORT, *LPSHORT;

typedef int INT, *PINT;
typedef unsigned int DWORD, *PDWORD, *LPDWORD;
typedef unsigned int UINT, *PUINT;

typedef long LONG, *PLONG;
typedef unsigned long ULONG, *PULONG;
typedef long long LONGLONG, *PLONGLONG;

typedef float FLOAT, *PFLOAT;
typedef double DOUBLE, *PDOUBLE;

#define VOID void //typedef void VOID;
typedef void *PVOID, *LPVOID;
typedef const void* LPCVOID;

typedef int BOOL, *PBOOL;

typedef int HRESULT;

typedef long long SInt64 ;
typedef unsigned long long UInt64 ;

#endif


// left means the first point
// right means the point after the last point, (out of the area)
// by this means, width=right-left
// in the same way, height=bottom-top
typedef struct tagRect{
	INT    left;
	INT    top;
	INT    right;
	INT    bottom;
} SW_RECT, *PSW_RECT, *LPSW_RECT;

typedef const SW_RECT *PCSW_RECT, *LPCSW_RECT;

typedef struct tagPoint{
	INT  x;
	INT  y;
} SW_POINT, *PSW_POINT, *LPSW_POINT;

typedef const SW_POINT *PCSW_POINT, *LPCSW_POINT;

typedef struct tagSize{
	INT 	   cx;
	INT 	   cy;
} SW_SIZE, *PSW_SIZE, *LPSW_SIZE;

typedef const SW_SIZE *PCSW_SIZE, *LPCSW_SIZE;


#endif // _SWBASEHV_INCLUDED__

