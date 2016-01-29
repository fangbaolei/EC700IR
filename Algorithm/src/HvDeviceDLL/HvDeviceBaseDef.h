#ifndef _HVDEVICEBASEDEF_H__
#define _HVDEVICEBASEDEF_H__

#define MAX_PATH	260

#ifndef NULL
#ifdef _cplusplus
#define NULL	0
#else
#define NULL	0//((void*)0)
#endif
#endif

#ifndef FALSE
#define FALSE	0
#endif

#ifndef TRUE
#define TRUE	1
#endif

#ifndef IN
#define IN
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
#define CDECL _cdecl
#endif

#undef FAR
#undef NEAR
#define FAR		far
#define NEAR	near
#ifndef CONST	
#define CONST	const
#endif

typedef unsigned long       DWORD;
typedef int                 BOOL;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef float               FLOAT;
typedef FLOAT               *PFLOAT;
typedef BOOL near           *PBOOL;
typedef BOOL far            *LPBOOL;
typedef BYTE near           *PBYTE;
typedef BYTE far            *LPBYTE;
typedef int near            *PINT;
typedef int far             *LPINT;
typedef WORD near           *PWORD;
typedef WORD far            *LPWORD;
typedef long far            *LPLONG;
typedef DWORD near          *PDWORD;
typedef DWORD far           *LPDWORD;
typedef void far            *LPVOID;
typedef CONST void far      *LPCVOID;

typedef int                 INT;
typedef unsigned int        UINT;
typedef unsigned int        *PUINT;

//
// Void
//

typedef void *PVOID;


//
// Basics
//

#ifndef VOID
#define VOID void
typedef char CHAR;
typedef short SHORT;
typedef long LONG;
#endif

//
// ANSI (Multi-byte Character) types
//
typedef CHAR *PCHAR;
typedef CHAR *LPCH, *PCH;

typedef CONST CHAR *LPCCH, *PCCH;
typedef CHAR *NPSTR;
typedef CHAR *LPSTR, *PSTR;
typedef CONST CHAR *LPCSTR, *PCSTR;

#ifndef _HRESULT_DEFINED
#define _HRESULT_DEFINED
typedef LONG HRESULT;

#endif // !_HRESULT_DEFINED

//
// The following types are guaranteed to be unsigned and 64 bits wide.
//

typedef unsigned __int64 ULONG64, *PULONG64;
typedef unsigned __int64 DWORD64, *PDWORD64;

typedef unsigned int DWORD32, *PDWORD32;

#endif
