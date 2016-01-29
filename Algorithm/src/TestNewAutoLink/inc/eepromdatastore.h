#ifndef _EEPROM_DATA_STORE_H_
#define _EEPROM_DATA_STORE_H_

#include "Basecommon.h"

#define EEPROM_BLOCK_MAX_LEN	256

class CEepromDataStore {
	private:
		DWORD32			m_dwBlockStartPos;
		DWORD32			m_dwBakBlockStartPos;
		DWORD32			m_dwBlockUsedLen;
		DWORD32			m_dwBlockMaxLen;		
		BOOL			m_fInitialized;
		BOOL 			m_fCrcCheck;
		int				m_iStoreTableFlag;
		
		BYTE8			m_rgbBuff[EEPROM_BLOCK_MAX_LEN];
		HRESULT SyncDataStore( int iSrcTable, int iDstTable );
		HRESULT IsOneDataStoreActive( int iTableType );
		HRESULT SetDataOne( 
			int			iTableType,		//表类型
			PBYTE8		pData,			//数据.
			DWORD32		dwLen,			//长度.
			DWORD32		dwOffset		//起始需要操作地址.
		);
	public:
	
		virtual ~CEepromDataStore();
		CEepromDataStore();
		
		//用来初始化数据校验有关信息,校验永远只存放在UsedLen之后,如果越界则初始化不成功.
		HRESULT InitDataStore
		(			
			BOOL		fUsedCRCCheck,	//是否使用CRC校验.
			DWORD32 	dwStartPos,		//存储起始地址.
			DWORD32		dwBakStartPos,	//副存储起始地址.
			DWORD32		dwUsedLen,		//实际数据长度.
			DWORD32		dwMaxLen		//充许数据存储的最大长度.
		);
		HRESULT IsDataStoreActive( void );		//判定当前数据有效性.
		//设置数据,此时自动进行相关校验运算以及保存双备表.
		HRESULT SetData
		(
			PBYTE8		pData,			//数据.
			DWORD32		dwLen,			//长度.
			DWORD32		dwOffset		//起始需要操作地址.
		);
		HRESULT GetData
		(
			PBYTE8		pData,			//数据.
			DWORD32		dwLen,			//长度.
			DWORD32		dwOffset		//起始操作地址.
		);
};

#endif
