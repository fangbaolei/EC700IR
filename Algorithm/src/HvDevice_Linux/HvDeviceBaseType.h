#ifndef _HVDEVICEBASETYPE_H
#define _HVDEVICEBASETYPE_H

#define MAX_PAHT 260

#ifndef NULL
#ifdef __cplusplus
#define NULL 0
#else
#define NULL ((void*)0)
#endif
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef OUT
#define OUT
#endif

#ifndef OPTIONAL
#define OPTIONAL
#endif

#undef far
#undef near
#undef pascal

#define far
#define near

#ifndef CDECL
#define CDECL __attribute__((__cdecl__))
#endif

#undef FAR
#undef NEAR
#define FAR  far
#define NEAR near

#ifndef CONST
#define CONST const
#endif



//
typedef unsigned long           DWORD;
typedef int                                     BOOL;
typedef unsigned char           BYTE;
typedef unsigned short          WORD;
typedef float                               FLOAT;
typedef FLOAT                              *PFLOAT;
typedef BOOL near                      *PBOOL;
typedef BOOL far                          *LPBOOL;
typedef BYTE near                       *PBYTE;
typedef BYTE far                            *LPBYTE;
typedef int near                            *PINT;
typedef int far                                 *LPINT;
typedef WORD near                       *PWORD;
typedef WORD far                           *LPWORD;
typedef long far                              *LPLONG;
typedef DWORD near                     *PDWORD;
typedef DWORD far                       *LPDWORD;
typedef void far                            *LPVOID;
typedef CONST void far              *LPCVOID;
typedef void                                     *PVOID;

typedef unsigned short              INT16;
typedef int                                         INT;
typedef unsigned int                    UINT;
typedef unsigned int                    *PUINT;
typedef int  INT32;


typedef unsigned int DWORD32,*PDWORD32;
typedef unsigned long long DWORD64,*PDWORD64;





#ifndef VOID
#define VOID void
typedef char CHAR;
typedef short SHORT;
typedef long LONG;
#endif


typedef CHAR *PCHAR;
typedef CHAR *LPCH, *PCH;

typedef CONST CHAR *LPCCH,*PCCH;
typedef CHAR *NPSTR;
typedef CHAR *LPSTR,*PSTR;
typedef CONST CHAR *LPCSTR,*PCSTR;

typedef int SOCKET;

#ifndef _HRESULT_DEFINED
#define _HRESULT_DEFINED
typedef LONG HRESULT;
#endif


#endif // _HVDEVICEBASETYPE_H
