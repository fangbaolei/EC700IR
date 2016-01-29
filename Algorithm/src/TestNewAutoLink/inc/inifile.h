#ifndef _INIFILE_INCLUDED__
#define _INIFILE_INCLUDED__

#include "HvStream.h"

#if ( RUN_PLATFORM == PLATFORM_WINDOWS )
#include <windows.h>
#endif

#if RUN_PLATFORM == PLATFORM_DSP_BIOS
#include "AddtionParam.h"
#endif

const INT g_uMaxSectionLen    = 48;
const INT g_uMaxSectionNumber = 80;
const INT g_uMaxKeyLen        = 48;
const INT g_uMaxValueLen      = 80;
const INT g_uMaxKeyNumber     = 800;
const INT g_uMaxLine          = 1024;

///关键字
class Keys
{
public:
	//所属的section编号(从1开始,小于0则为已经被删除的关键字,0则不属于任何小节的关键字)
	INT uSectionNumber;
	char* szKeyName;   
	char* szKeyValue;
	char* szComment;
public:
	Keys()
		: uSectionNumber( -1 ),
		szKeyName( NULL ),
		szKeyValue( NULL ),
		szComment( NULL )
	{}
	
	~Keys()
	{
		Delete();
	}
	void Delete()
	{
		uSectionNumber = -1;
		if( szKeyName != NULL )
		{
			delete[] szKeyName;
			szKeyName = NULL;
		}
		
		if( szKeyValue != NULL )
		{
			delete[] szKeyValue;
			szKeyValue = NULL;
		}
		
		if( szComment != NULL )
		{
			delete[] szComment;
			szComment = NULL;
		}
	}
};

//小节section
class Sections{
public:
	char* szSectionName;
public:
	Sections()
		: szSectionName( NULL )
	{}
	~Sections()
	{
		Delete();
	}
	void Delete()
	{
		if( szSectionName != NULL )
		{
			delete[] szSectionName;
			szSectionName = NULL;
		}
	}
};

class CIniFile
{
public:
	CIniFile(void);
	~CIniFile(void);

public:
	//加载INI流
	HVRESULT ReadStream( 
		HiVideo::ISequentialStream* pStream,
		BOOL fClear = FALSE,
		HiVideo::ISequentialStream* pRevert = NULL,
		HiVideo::ISequentialStream* pModification = NULL
		);
#if RUN_PLATFORM == PLATFORM_DSP_BIOS
	HVRESULT SetAddtionParam( CAddtionParam* pAddtionParam );
#endif
	//ini文件读取
	HVRESULT ReadString( 
		LPCSTR lpszSection, 
		LPCSTR lpszKey, 
		LPCSTR lpszDefault, 
		LPSTR  lpszResult, 
		INT    dwSize, 
		PINT   pcbLen = NULL
		);
	HVRESULT ReadInteger( 
		LPCSTR lpszSection, 
		LPCSTR lpszKey, 
		INT uDefault, 
		PINT puResult
		);
	HVRESULT ReadBoolean( 
		LPCSTR lpszSection, 
		LPCSTR lpszKey, 
		BOOL fDefault, 
		PBOOL pfResult
		);
	HVRESULT ReadBinary(
		LPCSTR lpszSection,
		LPCSTR lpszKey,
		PBYTE8 lpbDefault,
		PBYTE8 lpbResult,
		INT    iSize,
		PINT   pcbRead = NULL
		);
	//写入INI
	HVRESULT WriteString(
		LPCSTR lpszSection,
		LPCSTR lpszKey,
		LPCSTR lpszValue,
		LPCSTR lpszComment = NULL
		);

#if ( RUN_PLATFORM == PLATFORM_WINDOWS )
	//从注册表追加
	HVRESULT AppendFromRegistry(
		const HKEY& hKey, 
		HiVideo::ISequentialStream* pRevert = NULL,
		HiVideo::ISequentialStream* pModification = NULL
		);
#endif
	//把内存数据写入流
	HVRESULT WriteToStream( HiVideo::ISequentialStream* pStream );
	//获得小节数
	HVRESULT GetSectionNumber( PINT pcbSection );
	//获得相应小节下的关键字数
	HVRESULT GetKeyNumber( LPCTSTR lpszSection, PINT pcbKey );
	//枚举小节
	HVRESULT EnumSection( const INT& uSection, LPTSTR lpszSection, PINT pcbLen );
	//枚举相应小节下的关键字
	HVRESULT EnumKey( 
		const INT& uKey, LPCTSTR lpszSection, 
		LPTSTR lpszKeyName, PINT pcbNameSize,
		LPTSTR lpszValue, PINT pcbValueSize
		);
	//得到相应小节下关键字的注释
	HVRESULT ReadComment( 
		LPCTSTR lpszSection, LPCTSTR lpszKey,
		LPTSTR lpszResult, const INT& cbLen
		);

	//HVRESULT GetMaxInteger( LPCSTR lpszSection, LPCSTR lpszKey, PINT piResult );
	//HVRESULT GetMinInteger( LPCSTR lpszSection, LPCSTR lpszKey, PINT piResult );
	//HVRESULT GetDefaultInteger( LPCSTR lpszSection, LPCSTR lpszKey, PINT piResult );

private:
	//处理附加信息
	INT Superaddition( LPCTSTR lpszLine, LPTSTR lpszValue = NULL, const INT& iLen = 0 );
	//从流的当前位置读取一行数据(如果长度超过uLen,则含去不要)
	BOOL ReadOneLine( HiVideo::ISequentialStream* pStream, LPTSTR lpszLine, INT uLen );
	//通过一字符串来设置内存的数据
	BOOL SetDataFromLine( 
		LPTSTR lpszLine,
		HiVideo::ISequentialStream* pResert,
		HiVideo::ISequentialStream* pModification,
		const BOOL& fInitialize
		);
	//判断是否是不用处理的行数据
    BOOL IsComplete( const BYTE8 bMaker );
	//得到小节的编号
	INT GetSection( LPCSTR lpszSection );
	//判断是否有此KEY,有则取其值
	BOOL GetKeyValue( const INT& uPos, LPCSTR lpszKey, LPSTR lpszValue, QWORD64 dwLen );
	//刷新内存中的数据,主要是删除没有关键字的小节
	void RebirthData();

#if ( RUN_PLATFORM == PLATFORM_WINDOWS )
	//从注册表追加到内存数据中
	BOOL SetDataFromRegistry( 
		const HKEY& hKey,
		HiVideo::ISequentialStream* pResert,
		HiVideo::ISequentialStream* pModification
		);
#endif

private:
	//小节数组
	Sections m_rgSection[ g_uMaxSectionNumber ];
	INT m_uSectionNumber;
	//关键字数组
	Keys m_rgKey[ g_uMaxKeyNumber ];
	INT m_uKeyNumber;
	//当前小节下标
	INT m_uCurSection;
    //是否加载过INI
	BOOL m_fInitialize;

#if RUN_PLATFORM == PLATFORM_DSP_BIOS
	//用来操作非INI的系统参数
	CAddtionParam* m_pAddtionParam;
#endif
};

#endif
