#ifndef _HV_FLASH_FILE_H
#define _HV_FLASH_FILE_H

#ifdef __cplusplus
	extern "C" {
#endif	//#ifdef __cplusplus

#include "dspsys_def.h"

/*	Flash操作说明:
		为了对上层屏蔽掉FLASH的操作,对FLASH使用Cache方式读写操作.
	在外部存储器SDRAM中为FLASH开辟一块内存区域128K,当上层要读取FLASH时,先将该段FLASH读至内存,
	再将其放置到用户所要的内存区域,当用户想要修改该段FLASH中的任何一个数据时,直接修改内存中的
	数据,当FLASH访问跨过该128K区域时,底层判断该段内存是否被修改,如果修改则直接将该段内存中的
	内容写入至FLASH中对应块;用户对一些非常敏感的FLASH块(数据比较关键),当上层执行了写操作之后
	应马上运行Flash_flush操作,将内存中的相应FLASH块内部马上更新至FLASH.
		使用了CACHE方式来实现FLASH读写操作,虽然可以实现对FLASH的随机写操作,但是同时也带来了
	另外一个问题,可能会有正在flush时,系统复位或掉电,导致信息丢失.实现此操作,需要将在硬件FLASH
	操作层和上层之间加入一个接口层,并需要添加新的命令Flash_flush操作,用来更新当前操作.
*/
typedef enum {
	FILE_PROGRAM_START_NUM		= 1,		//1	~	10:			用户程序文件1~10.
	FILE_PROGRAM_STOP_NUM		= 10,
	FILE_DETMODEL_START_NUM		= 11,		//11	~	30:		检测部分模型文件1~20.
	FILE_DETMODEL_STOP_NUM		= 30,
	FILE_RECOGMODEL_START_NUM	= 31,		//31	~	70:		识别部分模型文件1~40.
	FILE_RECOGMODEL_STOP_NUM	= 70,
	FILE_LOG_START_NUM			= 71,		//71	~	100:	LOG类型文件1~30.
	FILE_LOG_STOP_NUM			= 100,
	FILE_CONFIG_START_NUM		= 101,		//101	~	140:	配置类型文件1~40.	
	FILE_CONFIG_STOP_NUM		= 140,	
	FILE_DETMODEL_START_NUM_BAK	= 511,		//511	~	530:	检测部分模型备份文件1~20.
	FILE_DETMODEL_STOP_NUM_BAK	= 530,
	FILE_RECOGMODEL_START_NUM_BAK	= 531,	//531	~	570:	识别部分模型备份文件1~40.
	FILE_RECOGMODEL_STOP_NUM_BAK	= 570,
	MAX_FILE_NUMBER				= 1000
} FILE_SERIAL_NUM_DEF;

#define MAX_FILE_STRING_LEN			32

/* 数据结构说明:该结构为文件系统结构定义,用于管理文件系统. */
typedef struct _FILE_SYSTEM {
	DWORD32 		dwSize;				//文件系统表大小,以字节为单位.
	DWORD32 		dwVer;				//文件系统版本号,为兼容性考虑.
	DWORD32 		dwMainFileTablePos;	//文件主分配表的起始位置.
	DWORD32 		dwBakFileTablePos;	//文件备份分配表的起始位置.
	DWORD32 		dwFileBlockPos;		//文件存放的起始位置.
	DWORD32 		dwFileTableSize;	//文件分配表的大小,以字节为单位.
	DWORD32 		dwReserved[4];		//文件系统表保留字.
	DWORD32 		dwCheckCode;		//文件系统表数据校验和.
} FILE_SYSTEM;


/* 数据结构说明:该结构为文件分配表结构定义,用于管理文件. */
typedef struct _FILE_TAB {
	DWORD32 		dwSize;				//文件分配表大小,以字节为单位.
	DWORD32 		dwVer;				//文件分配表的版本号.
	DWORD32 		dwActiveFile;		//当前文件系统中有多少个有效文件.	
	DWORD32 		dwMaxFileCount;		//当前文件分配表所支持的最大文件个数.
	DWORD32 		dwFileHeaderSize;	//文件头大小,以字节为单位.
	DWORD32 		dwTrailingBlockPos;	//FLASH中的空闲区域的起始位置.
	DWORD32 		dwTrailingBlockLen;	//当前余一下的连续FLASH空间总和.
	DWORD32 		dwReserved[4];		//文件分配表保留字.
	DWORD32 		dwCheckCode;		//文件分配表数据校验和.
} FILE_TAB;


typedef struct _FILE_HEADER {
	DWORD32 		dwVer;				//文件版本号,用来区分当前所操作文件的版本号,该版本号的意义由使用该文件的用户解析.
	WORD16 			wFlag;				//文件标志,当其标志为0x0101表示该文件有效,其他任何值则视该文件无效.
	WORD16 			wSerialNumber;		//参照文件类型将文件序号.
	char 			szName[ MAX_FILE_STRING_LEN ];	//文件标识信息,最后必须以'\0'为结束标志符.
	DWORD32 		dwFileStartPos;		//文件在FLASH中存放的起始位置.
	DWORD32 		dwNowLen;			//文件当前的长度,即文件指针,如果该项大于dwMaxLength则视为非法,表示该文件出现异常.
	DWORD32 		dwMaxLength;		//文件的最大长度,以字节为单位.
	DWORD32			dwFileCrc;			//文件CRC值
	DWORD32 		dwReserved[2];		//为文件头的保留字段,当前将该保留字段全部置为0.
} FILE_HEADER;


/* 枚举结构说明:文件打开方式.	*/
typedef enum {
	FILE_READ 			= 0x01,			//读方式,同时可以支持多个读操作发生; 
	FILE_WRITE 			= 0x02,			//写方式,当且仅当只同时支持一个写操作发生;
	FILE_WRITE_CLEAR 	= 0x03			//写清除方式,当前文件全部清空;
} FILE_OPEN_MODE;


/* 枚举结构说明:文件指针移动方式(相对起始点). */
typedef enum {
	FILE_BEGIN			= 0x01,
	FILE_CURRENT		= 0x02,
	FILE_END			= 0x03	
} FILE_MOVE_METHOD;


typedef void * FILE_HANDLE;				//文件操作句柄
/*-----------------------------------------------------------------------------------------------------------------------------------------------*/




/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
/*
函数说明:该函数用来格式化FLASH的文件系统,包括文件系统和文件分配表.
函数返回值:
	返回S_OK表示格式化成功.
	返回E_FAIL表示硬件底层操作出现异常.
	返回S_FALSE表示格式化失败. */
extern HRESULT FILE_FormatFileSystem( void );

/*
函数说明:该函数用来整理FLASH上的文件,将文件与文件中间的空隙空间集整到FLASH起来.
函数返回值:
	返回S_OK表示文件整理成功.
	返回E_FAIL表示硬件底层操作出现异常.
	返回S_FALSE表示文件整理失败. */
extern HRESULT FILE_PackFileSystem( void );

/*
函数说明:该函数用来取得当前文件系统的信息.
函数返回值:
	返回S_OK表示成功取得文件系统信息.
	返回E_POINTER表示pFileSystemInfo为INVALID_POINTER.
	返回E_FAIL表示当前取信息出错,没有可用的文件系统构架.
	返回S_FALSE表示取信息失败. */
extern HRESULT FILE_GetFileSystemInfo( 
	FILE_SYSTEM 	*pFileSystemInfo	//指向文件系统信息.
);

/* 函数说明:该函数用来设置文件系统信息.
函数返回值:
	返回S_OK表示成功设置文件系统信息.
	返回E_POINTER表示pFileSystemInfo为INVALID_POINTER.
	返回E_FAIL表示当前取信息出错,没有可用的文件系统构架.
	返回S_FALSE表示设置文件系统信息失败. */
extern HRESULT FILE_SetFileSystemInfo( 
	const FILE_SYSTEM *pFileSystemInfo	//指向文件系统信息.
);

/*
函数说明:该函数用来取得文件分配表信息.
函数返回值:
	返回S_OK表示成功取得文件分配表信息.
	返回E_POINTER表示pFileTableInfo为INVALID_POINTER.
	返回E_FAIL表示当前取信息出错,没有可用的文件系统构架.
	返回S_FALSE表示设置文件系统信息失败. */
extern HRESULT FILE_GetFileTableInfo( 
	FILE_TAB		*pFileTableInfo		//指向文件系统信息.
);

/*
函数说明:该函数用来设置文件分配表信息.
函数返回值:
	返回S_OK表示成功取得文件分配表信息.
	返回E_POINTER表示pFileTableInfo为INVALID_POINTER.
	返回E_FAIL表示当前取信息出错,没有可用的文件系统构架.
	返回S_FALSE表示设置文件分配表信息失败. */
extern HRESULT FILE_SetFileTableInfo( 
	const FILE_TAB	*pFileTableInfo		//指向文件分配表信息.
);

/*
函数说明:该函数用来创建新文件.
函数返回值:
	返回S_OK表示创建新文件,*pFileHandle有意义.
	返回E_POINTER表示pFileHandle为INVALID_POINTER.	
	返回E_INVALIDARG表示传入的相关参数非法.
	返回E_FAIL表示当前取信息出错,没有可用的文件系统构架.	
	返回S_FALSE创建新文件失败. */
extern HRESULT FILE_Create( 
	WORD16 			wFileSerialNum, 	//所要创建的文件文件序号.
	DWORD32 		dwFileMaxSize, 		//新创建的文件最大长度,以字节为单位.
	FILE_HANDLE 	*pFileHandle		//指向新创建的文件操作句柄.
);

/*
函数说明:该函数用来打开已存在的文件.
函数返回值:
	返回S_OK表示打开文件成功,*pFileHandle有意义.
	返回E_POINTER表示pFileHandle为INVALID_POINTER.
	返回E_INVALIDARG表示传入的相关参数非法,文件序号不合法.
	返回E_FAIL表示当前取信息出错,没有可用的文件系统构架.	
	返回S_FALSE打开文件失败,文件不存在或者其他异常. */
extern HRESULT FILE_Open( 
	WORD16 			wFileSerialNum,		//所要打开的文件文件序号.
	FILE_OPEN_MODE 	FileOpenMode,		//文件打开模式.
	FILE_HANDLE		*pFileHandle		//指向新打开的文件操作句柄.
);

/*
函数说明:该函数用来从文件句柄所指示的文件中读取一定量的数据.
函数返回值:
	返回S_OK表示文件读取成功.
	返回E_HANDLE表示hFile为INVALID_HANDLE.
	返回E_POINTER表示pbDest为INVALID_POINTER.
	返回E_INVALIDARG表示文件句柄所指向的文件句柄没有实际意义.
	返回E_FAIL表示当前取信息出错,没有可用的文件系统构架.	
	返回S_FALSE表示文件读取失败. */
extern HRESULT FILE_Read( 
	FILE_HANDLE 	hFile, 				//指向文件操作句柄.
	PBYTE8 			pbDest, 			//指向读出的数据.
	DWORD32			dwBytesToRead,		//需要读取数据的字节数.
	PDWORD32 		pdwBytesRead		//返回实际读到的数据个数,如果为NULL则不填.
);

/*
函数说明:该函数用来向文件句柄所指示的文件写入一定量的数据.
函数返回值:
	返回S_OK表示文件写入成功.
	返回E_HANDLE表示hFile为INVALID_HANDLE.
	返回E_POINTER表示pbSrc为INVALID_POINTER.
	返回E_INVALIDARG表示文件句柄所指向的文件句柄没有实际意义.
	返回E_FAIL表示当前取信息出错,没有可用的文件系统构架.
	返回S_FALSE表示文件定义失败. */
extern HRESULT FILE_Write( 
	FILE_HANDLE 	hFile, 				//指向文件操作句柄.
	const PBYTE8 	pbSrc, 				//指向所要写入的数据.
	DWORD32			dwBytesToWrite,		//需要写入数据的字节数.
	PDWORD32 		pdwBytesWritten		//返回实际写入的数据个数,如果为NULL则不填.
);

/*
函数说明:该函数用来关闭已打开的文件.
函数返回值:
	返回S_OK表示文件关闭成功.
	返回E_HANDLE表示hFile为INVALID_HANDLE.
	返回E_INVALIDARG表示文件句柄所指向的文件句柄没有实际意义.
	返回E_FAIL表示当前取信息出错,没有可用的文件系统构架.
	返回S_FALSE表示文件关闭失败或者其他异常. */
extern HRESULT FILE_Close( FILE_HANDLE hFile );

/*
函数说明:该函数用来删除文件.
函数返回值:
	返回S_OK表示文件关闭成功.	
	返回E_INVALIDARG表示参数不正确,文件不存在.
	返回E_FAIL表示当前取信息出错,没有可用的文件系统构架.
	返回S_FALSE表示删除文件失败. */
extern HRESULT FILE_Delete( WORD16 wFileSerialNum );

/*
函数说明:该函数用来移动文件当前指针.
函数返回值:
	返回S_OK表示文件指针移动成功.
	返回E_HANDLE表示hFile为INVALID_HANDLE.
	返回E_FAIL表示当前取信息出错,没有可用的文件系统构架.
	返回E_INVALIDARG表示参数不正确.
	返回S_FALSE表示文件指针移动失败. */
extern HRESULT FILE_Seek(
	FILE_HANDLE 		hFile,				//指向文件操作句柄.
	int 				iSeekLocation,		//当前文件指针位置.
	FILE_MOVE_METHOD	MoveMethod			//移动方式			
);

/*
函数说明:该函数用来取得文件有关信息.
函数返回值:
	返回S_OK表示文件信息取得成功.
	返回E_HANDLE表示hFile为INVALID_HANDLE.
	返回E_POINTER表示pdwFileVer,或pszInfoString,或pdwMaxLen为INVALID_POINTER.
	返回E_FAIL表示当前取信息出错,没有可用的文件系统构架.
	返回S_FALSE表示文件信息获取失败. */
extern HRESULT FILE_GetFileInfo( 
	FILE_HANDLE 	hFile, 				//指向文件操作句柄.
	PDWORD32 		pdwFileVer, 		//指向文件版本号.
	PCHAR 			pszInfoString, 		//指向文件附加字符串信息.
	PDWORD32 		pdwMaxLen			//指向文件最大长度.
);

/*
函数说明:该函数用来设置文件信息.
函数返回值:
	返回S_OK表示文件信息设置成功.
	返回E_HANDLE表示hFile为INVALID_HANDLE.
	返回E_POINTER表示pszInfoString为INVALID_POINTER.
	返回E_FAIL表示当前取信息出错,没有可用的文件系统构架.
	返回S_FALSE表示文件信息设置失败. */
extern HRESULT FILE_SetFileInfo( 
	FILE_HANDLE 	hFile, 				//指向文件操作句柄.
	DWORD32 		dwFileVer, 			//文件版本号.
	const PCHAR 	pszInfoString		//指向文件符加信息.
);

/*
函数说明:该函数用来读取文件CRC校验值.
函数返回值:
	返回S_OK表示读取文件CRC校验值成功.
	返回E_HANDLE表示hFile为INVALID_HANDLE.
	返回E_FAIL表示读取文件CRC校验值出错,没有可用的文件系统构架.
	返回S_FALSE表示读取文件CRC校验值失败. */
extern HRESULT FILE_GetFileCRC( 
	FILE_HANDLE 	hFile, 				//指向文件操作句柄.
	DWORD32* 		pdwFileCrc			//文件CRC值
);

/*
函数说明:该函数用来读取文件CRC校验值.
函数返回值:
	返回S_OK表示读取文件CRC校验值成功.
	返回E_HANDLE表示文件打开失败.
	返回E_FAIL表示读取文件CRC校验值出错,没有可用的文件系统构架.
	返回S_FALSE表示读取文件CRC校验值失败.*/ 
extern HRESULT FILE_GetFileCRCFromID( 
	DWORD32 	dwFileID, 			//指向文件的文件序号.
	DWORD32* 	pdwFileCrc			//文件CRC值
);

/*
函数说明:该函数用来设置文件CRC校验值.
函数返回值:
	返回S_OK表示设置文件CRC校验值成功.
	返回E_HANDLE表示hFile为INVALID_HANDLE.
	返回E_FAIL表示设置文件CRC校验值出错,没有可用的文件系统构架.
	返回S_FALSE表示设置文件CRC校验值失败. */
extern HRESULT FILE_SetFileCRC( 
	FILE_HANDLE 	hFile, 				//指向文件操作句柄.
	DWORD32 		dwFileCrc			//文件CRC值
);

/*
函数说明:该函数用来读取FLASH当前使用量，单位为字节.
函数返回值:
	返回S_OK表示读取FLASH当前使用量成功.
	返回E_HANDLE表示pdwFileSize为INVALID_HANDLE.
	返回E_FAIL表示读取FLASH当前使用量出错,没有可用的文件系统构架. */
extern HRESULT FILE_GetUsedSize(DWORD32 *pdwFileSize);

/*-----------------------------------------------------------------------------------------------------------------------------------------------*/

#ifdef __cplusplus
	}
#endif	//#ifdef __cplusplus

#endif	//#ifndef _HV_FLASH_FILE_H



