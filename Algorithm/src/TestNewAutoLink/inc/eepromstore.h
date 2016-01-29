#ifndef _EEPROMSTORE_H
#define _EEPROMSTORE_H

#ifdef __cplusplus
	extern "C" {
#endif	// #ifdef __cplusplus

/*
每个在EEPROM中存储的数据块数据结构如下:
struct typedef _DATA_STORE_STRUCT {
	BYTE8			rgbActiveFlag[2];
	DWORD32			dwDataLen;
	BYTE8			rgbData[];
	DWORD32			dwCrcValue;
} DATA_STORE_STRUCT;
*/

// 该格举用来指示放置在外部非易失性存储器中的数据文件索引编号.
typedef enum {
	BOOT_FILE_NUMBER		= 0,		// Bootloader参数设计文件索引号.
	COUNT_FILE_NUMBER		= 1,		// 系统级计数器值文件索引号,如复位计数值.
	REPORT_FILE_NUMBER		= 2,		// 生产报告数据文件索引号.
	CONFIG_FILE_NUMBER		= 3,		// 系统工作配置数据文件索引号.
	MAX_EEPROM_FILE_NUM
} STORE_FILE_INDEX;

// 如果系统当前操作的文件号索引大于10000时表示为操作EEPROM文件.
#define EEPROM_FILE_NUMBER_BEGIN		10000
#define EEPROM_FILE_NUMBER_END			( EEPROM_FILE_NUMBER_BEGIN + MAX_EEPROM_FILE_NUM )

/*
函数说明:该函数用来从非易失性存储器中一次读出相应文件号所对应的文件所有数据.
函数返回值:
	返回S_OK表示读取数据成功.
	返回E_POINTER表示pbDest为NULL.
	返回E_INVALIDARG表示FileNum为非法.
	返回E_FAIL表示当前该文件数据校验不通过.
	返回S_FALSE表示当前文件的数据无效. */
extern HRESULT GetFileStoreData(
	STORE_FILE_INDEX	FileNum,		// 所要取得的数据文件索引号.
	PBYTE8				pbDest,			// 指向目标数据.
	DWORD32				dwBytesToRead,	// 期望读出的数据个数.
	PDWORD32			pdwBytesRead	// 为NULL时表示不关心实际读出的数据个数.
);

/*
函数说明:该函数用来向非易失性存储器中一次写入需要写的数据,默认为将这些直接打包成为一个文件.
函数返回值:
	返回S_OK表示写操作成功.
	返回E_POINTER表示pbSrc为NULL.
    返回E_INVALIDARG表示FileNum为非法.
	返回E_FAIL表示写数据失败. */
extern HRESULT SaveFileStoreData(
	STORE_FILE_INDEX	FileNum,		// 所要保存数据文件索引号.
	const PBYTE8		pbSrc,			// 指向源数据.
	DWORD32				dwBytesToWrite,	// 期望写入的数据个数.
	PDWORD32			pdwBytesWritten	// 为NULL时表示不关心实际写入的数据个数.
);

#ifdef __cplusplus
	}
#endif /* #ifdef __cplusplus */

#endif
