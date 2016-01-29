/*******************************************************************/
/*                                                                 */
/*         Copyright (C) 2007 SafeNet, Inc.                        */
/*                   All Rights Reserved                           */
/*                                                                 */
/*     This Module contains Proprietary Information of             */
/*    SafeNet, Inc, and should be treated as Confidential.         */
/*******************************************************************/

/********************************************************************
* SENTINELKEYSTYPES.H
*
* Description - Defines types used in SafeNet code for C/C++
*               compilers.
*
*********************************************************************/
#ifndef _SENTINEL_KEYS_TYPES_H
#define _SENTINEL_KEYS_TYPES_H 
/***************************************************************************
* DEFINE THE TARGETED PLATFORM
****************************************************************************/
#if (!(defined(_WIN32_) || defined(WIN32) || defined(_WIN32)) && !defined(_WINDOWS))
#define _DOS_   1
#define _16BIT_ 1
#elif (defined(_WINDOWS) && !(defined(_WIN32_) || defined(_WIN32) || defined(WIN32)))
#define _WIN_   1
#define _16BIT_ 1
#elif (defined(_WIN32_) || defined(WIN32) || defined(_WIN32))
#define _WIN32_ 1
#define _32BIT_ 1
#endif
/***************************************************************************/

/****************************************************************************
* SETTINGS FOR C
****************************************************************************/
#if defined(_WIN32_)
 #define SP_EXPAPI   __export
 #define SP_ASM      __asm
 #define SP_STDCALL  __stdcall
 #define SP_FASTCALL __fastcall
 #define SP_PASCAL
 #define SP_CDECL    __cdecl
#define SP_FAR
#define SP_NEAR
#define SP_HUGE
#endif /* _WIN32_ */
/***************************************************************************/

/***************************************************************************
* DEFINE POINTER TYPES
****************************************************************************/
#if defined(_WIN_)
	#define SP_PTR     SP_FAR *
#elif defined(_WIN32_)
	#define SP_PTR     *
 #define _SP_API     __stdcall
 #define SP_API   _SP_API
#else
 #define SP_API 
	#define SP_PTR     *
#endif

/***************************************************************************/
#define SP_IN
#define SP_OUT
#define SP_INOUT
#define SP_IO
#define SP_STRUCT typedef struct
#define SP_UNION  typedef union

#define SP_TRUE   1
#define SP_FALSE  0

#ifdef __cplusplus
#define SP_EXPORT extern "C"
#else
#define SP_EXPORT extern
#endif
/* __cplusplus */

#define SP_LOCAL  static

#ifdef _SHK_LINUX64_
	typedef          int    SP_LONG;
	typedef unsigned int    SP_ULONG;
        typedef unsigned int    SP_DWORD;
#else
	typedef                long   SP_LONG;
	typedef unsigned       long   SP_ULONG;
	typedef unsigned       long   SP_DWORD;
#endif

/* define SafeNet types */
typedef                 void  SP_VOID;
typedef                 char  SP_CHAR;
typedef 	        int   SP_INT;	
typedef          short  int   SP_SHORT;

typedef unsigned       char   SP_BOOLEAN;
typedef unsigned       char   SP_BYTE;
typedef unsigned       char   SP_UCHAR;
typedef unsigned short int    SP_USHORT;
typedef unsigned short int    SP_WORD;

#ifndef SP_HANDLE
typedef                void * SP_HANDLE;
#endif
typedef                void * SP_SYS_HANDLE;


typedef SP_CHAR             * SPP_CHAR;
typedef SP_SHORT            * SPP_SHORT;
typedef SP_LONG             * SPP_LONG;
typedef SP_BOOLEAN          * SPP_BOOLEAN;
typedef SP_BYTE             * SPP_BYTE;
typedef SP_UCHAR            * SPP_UCHAR;
typedef SP_USHORT           * SPP_USHORT;
typedef SP_WORD             * SPP_WORD;
typedef SP_ULONG            * SPP_ULONG;
typedef SP_DWORD            * SPP_DWORD;
typedef SP_VOID             * SPP_VOID;
typedef SP_HANDLE           * SPP_HANDLE;
typedef SP_SYS_HANDLE       * SPP_SYS_HANDLE;
typedef SP_DWORD	      SP_STATUS;

/*
* Error code base are defined here for all components in the product
*/
#define SP_SUCCESS	                  0
#define SP_FAIL                       1

#define SP_DRIVER_LIBRARY_ERROR_BASE      100
#define SP_DUAL_LIBRARY_ERROR_BASE        200
#define SP_SERVER_ERROR_BASE              300
#define SP_SHELL_ERROR_BASE               400
#define SP_SECURE_UPDATE_ERROR_BASE       500

/*
* Dual Library Error Codes:
*/
#define SP_ERR_INVALID_PARAMETER		(SP_DUAL_LIBRARY_ERROR_BASE + 1)
#define SP_ERR_SOFTWARE_KEY			(SP_DUAL_LIBRARY_ERROR_BASE + 2)
#define SP_ERR_INVALID_LICENSE			(SP_DUAL_LIBRARY_ERROR_BASE + 3)
#define SP_ERR_INVALID_FEATURE			(SP_DUAL_LIBRARY_ERROR_BASE + 4)
#define SP_ERR_INVALID_TOKEN			(SP_DUAL_LIBRARY_ERROR_BASE + 5)
#define SP_ERR_NO_LICENSE			(SP_DUAL_LIBRARY_ERROR_BASE + 6)
#define SP_ERR_INSUFFICIENT_BUFFER		(SP_DUAL_LIBRARY_ERROR_BASE + 7)
#define SP_ERR_VERIFY_FAILED			(SP_DUAL_LIBRARY_ERROR_BASE + 8)
#define SP_ERR_CANNOT_OPEN_DRIVER		(SP_DUAL_LIBRARY_ERROR_BASE + 9)
#define SP_ERR_ACCESS_DENIED			(SP_DUAL_LIBRARY_ERROR_BASE + 10)
#define SP_ERR_INVALID_DEVICE_RESPONSE          (SP_DUAL_LIBRARY_ERROR_BASE + 11)
#define SP_ERR_COMMUNICATIONS_ERROR		(SP_DUAL_LIBRARY_ERROR_BASE + 12)
#define SP_ERR_COUNTER_LIMIT			(SP_DUAL_LIBRARY_ERROR_BASE + 13)
#define SP_ERR_MEM_CORRUPT			(SP_DUAL_LIBRARY_ERROR_BASE + 14)
#define SP_ERR_INVALID_FEATURE_TYPE		(SP_DUAL_LIBRARY_ERROR_BASE + 15)
#define SP_ERR_DEVICE_IN_USE			(SP_DUAL_LIBRARY_ERROR_BASE + 16)
#define SP_ERR_INVALID_API_VERSION		(SP_DUAL_LIBRARY_ERROR_BASE + 17)
#define SP_ERR_TIME_OUT_ERROR			(SP_DUAL_LIBRARY_ERROR_BASE + 18)
#define SP_ERR_INVALID_PACKET			(SP_DUAL_LIBRARY_ERROR_BASE + 19)
#define SP_ERR_KEY_NOT_ACTIVE			(SP_DUAL_LIBRARY_ERROR_BASE + 20)
#define SP_ERR_FUNCTION_NOT_ENABLED		(SP_DUAL_LIBRARY_ERROR_BASE + 21)
#define SP_ERR_DEVICE_RESET			(SP_DUAL_LIBRARY_ERROR_BASE + 22)
#define SP_ERR_TIME_CHEAT			(SP_DUAL_LIBRARY_ERROR_BASE + 23)
#define SP_ERR_INVALID_COMMAND			(SP_DUAL_LIBRARY_ERROR_BASE + 24)
#define SP_ERR_RESOURCE				(SP_DUAL_LIBRARY_ERROR_BASE + 25)
#define SP_ERR_UNIT_NOT_FOUND			(SP_DUAL_LIBRARY_ERROR_BASE + 26)
#define SP_ERR_DEMO_EXPIRED			(SP_DUAL_LIBRARY_ERROR_BASE + 27)
#define SP_ERR_QUERY_TOO_LONG			(SP_DUAL_LIBRARY_ERROR_BASE + 28)

/*Server Error Codes*/
#define SP_ERR_SERVER_PROBABLY_NOT_UP       (SP_SERVER_ERROR_BASE + 1)
#define SP_ERR_UNKNOWN_HOST                 (SP_SERVER_ERROR_BASE + 2)
#define SP_ERR_BAD_SERVER_MESSAGE           (SP_SERVER_ERROR_BASE + 3)
#define SP_ERR_NO_LICENSE_AVAILABLE         (SP_SERVER_ERROR_BASE + 4)
#define SP_ERR_INVALID_OPERATION            (SP_SERVER_ERROR_BASE + 5)
#define SP_ERR_INTERNAL_ERROR               (SP_SERVER_ERROR_BASE + 6)
#define SP_ERR_PROTOCOL_NOT_INSTALLED       (SP_SERVER_ERROR_BASE + 7)
#define SP_ERR_BAD_CLIENT_MESSAGE           (SP_SERVER_ERROR_BASE + 8)
#define SP_ERR_SOCKET_OPERATION             (SP_SERVER_ERROR_BASE + 9)
#define SP_ERR_NO_SERVER_RESPONSE           (SP_SERVER_ERROR_BASE + 10)

/* Shell Error Codes */
#define SP_ERR_BAD_ALGO                      (SP_SHELL_ERROR_BASE + 1)
#define SP_ERR_LONG_MSG                      (SP_SHELL_ERROR_BASE + 2)
#define SP_ERR_READ_ERROR                    (SP_SHELL_ERROR_BASE + 3)
#define SP_ERR_NOT_ENOUGH_MEMORY             (SP_SHELL_ERROR_BASE + 4)
#define SP_ERR_CANNOT_OPEN                   (SP_SHELL_ERROR_BASE + 5)
#define SP_ERR_WRITE_ERROR                   (SP_SHELL_ERROR_BASE + 6)
#define SP_ERR_CANNOT_OVERWRITE              (SP_SHELL_ERROR_BASE + 7)
#define SP_ERR_INVALID_HEADER                (SP_SHELL_ERROR_BASE + 8)
#define SP_ERR_TMP_CREATE_ERROR              (SP_SHELL_ERROR_BASE + 9)
#define SP_ERR_PATH_NOT_THERE                (SP_SHELL_ERROR_BASE + 10)
#define SP_ERR_BAD_FILE_INFO                 (SP_SHELL_ERROR_BASE + 11)
#define SP_ERR_NOT_WIN32_FILE                (SP_SHELL_ERROR_BASE + 12)
#define SP_ERR_INVALID_MACHINE               (SP_SHELL_ERROR_BASE + 13)
#define SP_ERR_INVALID_SECTION               (SP_SHELL_ERROR_BASE + 14)
#define SP_ERR_INVALID_RELOC                 (SP_SHELL_ERROR_BASE + 15)
#define SP_ERR_CRYPT_ERROR                   (SP_SHELL_ERROR_BASE + 16)
#define SP_ERR_SMARTHEAP_ERROR               (SP_SHELL_ERROR_BASE + 17)
#define SP_ERR_IMPORT_OVERWRITE_ERROR        (SP_SHELL_ERROR_BASE + 18)
#define SP_ERR_FRAMEWORK_REQUIRED            (SP_SHELL_ERROR_BASE + 21)
#define SP_ERR_CANNOT_HANDLE_FILE            (SP_SHELL_ERROR_BASE + 22)

/*Secure Update error codes*/
#define SP_ERR_KEY_NOT_FOUND                (SP_SECURE_UPDATE_ERROR_BASE + 1)
#define SP_ERR_ILLEGAL_UPDATE               (SP_SECURE_UPDATE_ERROR_BASE + 2)
#define SP_ERR_DLL_LOAD_ERROR               (SP_SECURE_UPDATE_ERROR_BASE + 3)
#define SP_ERR_NO_CONFIG_FILE               (SP_SECURE_UPDATE_ERROR_BASE + 4)
#define SP_ERR_INVALID_CONFIG_FILE          (SP_SECURE_UPDATE_ERROR_BASE + 5) 
#define SP_ERR_UPDATE_WIZARD_NOT_FOUND      (SP_SECURE_UPDATE_ERROR_BASE + 6)
#define SP_ERR_UPDATE_WIZARD_SPAWN_ERROR    (SP_SECURE_UPDATE_ERROR_BASE + 7)
#define SP_ERR_EXCEPTION_ERROR              (SP_SECURE_UPDATE_ERROR_BASE + 8)
#define SP_ERR_INVALID_CLIENT_LIB           (SP_SECURE_UPDATE_ERROR_BASE + 9)
#define SP_ERR_CABINET_DLL                  (SP_SECURE_UPDATE_ERROR_BASE + 10)
#define SP_ERR_INSUFFICIENT_REQ_CODE_BUFFER (SP_SECURE_UPDATE_ERROR_BASE + 11)
#define SP_ERR_UPDATE_WIZARD_USER_CANCELED  (SP_SECURE_UPDATE_ERROR_BASE + 12)

#endif 
