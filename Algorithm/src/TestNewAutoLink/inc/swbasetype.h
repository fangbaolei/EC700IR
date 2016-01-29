#ifndef _SWBASEHV_INCLUDED__
#define _SWBASEHV_INCLUDED__

#include "config.h"

#ifndef NULL
	#define NULL	0
#endif

#ifndef TRUE
	#define TRUE		1
#endif

#ifndef FALSE
	#define FALSE		0
#endif

// 矩形的坐标
#ifdef __ASM_FILE__
	// 定义这些常量是因为某些汇编器可能无法使用 struct 的定义, 因此在汇编中访问结构的成员请使用这些常量
	#define RECT_left				0
	#define RECT_top				4
	#define RECT_right				8
	#define RECT_bottom				12

	#define POINT_x					0
	#define POINT_y					4

	#define SIZE_cx 				8
	#define SIZE_cy					12
#else		// #ifdef __ASM_FILE__
	typedef unsigned char	BYTE8, *PBYTE8, *LPBYTE8;
	typedef unsigned short	WORD16, *PWORD16, *LPWORD16;
	typedef unsigned int	DWORD32, *PDWORD32, *LPDWORD32;
	typedef char			SBYTE8, *PSBYTE8, *LPSBYTE8;
	typedef short			SWORD16, *PSWORD16, *LPSWORD16;
	typedef int				SDWORD32, *PSDWORD32, *LPSDWORD32;
	#if ( RUN_PLATFORM == PLATFORM_WINDOWS )
		typedef unsigned __int64	QWORD64, *PQWORD64, *LPQWORD64;
		typedef __int64				SQWORD64, *PSQWORD64, *LPSQWORD64;
		typedef long				HRESULT;
	#elif ( RUN_PLATFORM == PLATFORM_DSP_BIOS )
		typedef unsigned long	QWORD64, *PQWORD64, *LPQWORD64;
		typedef long			SQWORD64, *PSQWORD64, *LPSQWORD64;
		typedef SDWORD32		HRESULT;
	#endif
	
	typedef int INT, *PINT;
	typedef int BOOL, *PBOOL;
	typedef unsigned int UINT, *PUINT;

	typedef long LONG, *PLONG;
	typedef unsigned long ULONG, *PULONG;
	typedef short SHORT, *PSHORT;

	typedef char CHAR;
	typedef unsigned short WCHAR;

	typedef WCHAR *PWCHAR;
	typedef WCHAR *LPWCH, *PWCH;
	typedef const WCHAR *LPCWCH, *PCWCH;
	typedef WCHAR *NWPSTR;
	typedef WCHAR *LPWSTR, *PWSTR;

	typedef const WCHAR *LPCWSTR, *PCWSTR;

	//
	// ANSI (Multi-byte Character) types
	//
	typedef CHAR *PCHAR;
	typedef CHAR *LPCH, *PCH;

	typedef const CHAR *LPCCH, *PCCH;
	typedef CHAR *NPSTR;
	typedef CHAR *LPSTR, *PSTR;
	typedef const CHAR *LPCSTR, *PCSTR;

	typedef void *PVOID, *LPVOID;

	// left means the first point
	// right means the point after the last point, (out of the area)
	// by this means, width=right-left
	// in the same way, height=bottom-top
	typedef struct {
		int    left;
		int    top;
		int    right;
		int    bottom;
	} HV_RECT, *PHV_RECT, *LPHV_RECT;

	typedef const HV_RECT * PCHV_RECT, *LPCHV_RECT;

	typedef struct {
		int  x;
		int  y;
	} HV_POINT, *PHV_POINT, *LPHV_POINT;
	
	typedef const HV_POINT *PCHV_POINT, *LPCHV_POINT;

	typedef struct {
		int 	   cx;
		int 	   cy;
	} HV_SIZE, *PHV_SIZE, *LPHV_SIZE;

	typedef const HV_SIZE *PCHV_SIZE, *LPCHV_SIZE;

#endif		// #ifdef __ASM_FILE__


#if RUN_PLATFORM == PLATFORM_DSP_BIOS
	#ifndef RESTRICT_PBYTE8
		#define RESTRICT_PBYTE8 PBYTE8 restrict
	#endif
	
	#ifndef RESTRICT_PWORD16
		#define RESTRICT_PWORD16 PWORD16 restrict
	#endif

	#ifndef RESTRICT_PDWORD32
		#define RESTRICT_PDWORD32 PDWORD32 restrict
	#endif	

	#ifndef RESTRICT
		#define RESTRICT restrict
	#endif

#else

	#ifndef RESTRICT_PBYTE8
		#define RESTRICT_PBYTE8 PBYTE8
	#endif

	#ifndef RESTRICT_PWORD16
		#define RESTRICT_PWORD16 PWORD16
		#endif

	#ifndef RESTRICT_PDWORD32
		#define RESTRICT_PDWORD32 PDWORD32
	#endif	

	#ifndef RESTRICT
		#define RESTRICT
	#endif

#endif

#endif // _SWBASEHV_INCLUDED__

