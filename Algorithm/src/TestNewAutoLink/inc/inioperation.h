#ifndef _INI_OPERATION_H__
#define _INI_OPERATION_H__

#include "swBaseType.h"
#include "swWinError.h"

#ifdef __cplusplus
	extern "C" {
#endif	/* #ifdef __cplusplus */

typedef void * INIFILE_HANDLE;

/*
函数说明:该函数用来Ini文件.
函数返回值:
	返回S_OK表示初始化Ini文件成功.
	返回E_POINTER表示pFileHandle为NULL.
	返回E_NOTIMPL表示系统当前不支持Ini文件系统.
	返回E_FAIL表示初始化Ini文件失败. */
extern HRESULT InitIniFile( 
	LPCSTR					pcszFileName,	//指向文件名字符串.
	INIFILE_HANDLE			*pFileHandle	//指向Ini文件名柄.
);

/* 
函数说明:该函数用来将Ini文件真正对该文件所依赖的存储器写出.
函数返回值:
	返回S_OK表示操作成功.
	返回E_HANDLE表示hFile为NULL.
	返回E_FAIL表示操作失败. */
extern HRESULT FlushIniFile(
	INIFILE_HANDLE			hFile
);

/*
函数说明:该函数用来从Ini文件中读取某个组中某个项字符串.
函数返回值:
	返回S_OK表示读取数据成功.
	返回E_HANDLE表示hFile为NULL.
	返回E_POINTER表示pcszSubSet或者pcszItem,或者pszStr为NULL.
	返回E_FAIL表示当前所要读取的项不存在.
	返回S_FALSE表示所取得的字符串长度大于所给出的最大长度. */
extern HRESULT GetIniFileStr( 
	INIFILE_HANDLE			hFile,			//指向Ini文件句柄.
	LPCSTR					pcszSubSet,		//指向组名字符串.
	LPCSTR					pcszItem,		//指向项名字符串.
	LPSTR					pszRetStr,		//指向读取的字符串数据.
	DWORD32					dwSize			//目标缓存器的大小
);

/*
函数说明:该函数用来向Ini文件中某组某项写入一字符串.
函数返回值:
	返回S_OK表示写入数据成功.
	返回E_HANDLE表示hFile为NULL.
	返回E_POINTER表示pcszSubSet或者pcszItem,或者pcszStr为NULL.
	返回E_FAIL表示当前所要读取的项不存在. */
extern HRESULT WriteIniFileStr(
	INIFILE_HANDLE			hFile,			//指向Ini文件句柄.
	LPCSTR					pcszSubSet,		//指向组名字符串,如果不存在则创建.
	LPCSTR					pcszItem,		//指向项名字符串,如果不存在则创建.
	LPCSTR					pcszStr			//指向所要写入的字符串.
);

/*
函数说明:该函数用来从Ini文件中删除某组中的某项.
函数返回值:
	返回S_OK表示写入数据成功.
	返回E_HANDLE表示hFile为NULL.
	返回E_POINTER表示pcszSubSet为NULL.
	返回E_FAIL表示当前所要删除的项不存在. */
extern HRESULT DeleteIniFileItem( 
	INIFILE_HANDLE			hFile,			//指向Ini文件句柄.
	LPCSTR					pcszSubSet,		//指向组名字符串.
	LPCSTR					pcszItem		//指向项名字符串,如果为空则删除整个组.
);

/*
函数说明:该函数用来将Ini文件全部清空.
函数返回值:
	返回S_OK表示清空成功.
	返回E_HANDLE表示hFile为NULL.
	返回E_FAIL表示清空文件失败. */
extern HRESULT ClearIniFile(
	INIFILE_HANDLE			hFile			//指向Ini文件句柄.
);

//从Ini文件中读取整型参数
extern HRESULT GetIniFileInteger(
	INIFILE_HANDLE			hFile,			//指向Ini文件句柄.
	LPCSTR					pcszSubSet,		//指向组名字符串.
	LPCSTR					pcszItem,		//指向项名字符串.
	int						*piValue,		//指向读取的整型数据
	int						iDefault		//参数不存在时返回的缺省值
);

//从Ini文件中读取浮点型参数
extern HRESULT GetIniFileFloat(
	INIFILE_HANDLE			hFile,			//指向Ini文件句柄.
	LPCSTR					pcszSubSet,		//指向组名字符串.
	LPCSTR					pcszItem,		//指向项名字符串.
	float					*pfltValue,		//指向读取的浮点型数据
	float					fltDefault		//参数不存在时返回的缺省值
);

//向Ini文件中写入一整型参数.
extern HRESULT SetIniFileInteger(
	INIFILE_HANDLE			hFile,			//指向Ini文件句柄.
	LPCSTR					pcszSubSet,		//指向组名字符串.
	LPCSTR					pcszItem,		//指向项名字符串.
	int						iValue			//写入的数值.
);

//向Ini文件中写入一浮点数.
extern HRESULT SetIniFileFloat(
	INIFILE_HANDLE			hFile,			//指向Ini文件句柄.
	LPCSTR					pcszSubSet,		//指向组名字符串.
	LPCSTR					pcszItem,		//指向项名字符串.
	float					fValue			//写入的数值.
);

/*
函数说明:该函数用来将Ini文件文件关闭.
函数返回值:
	返回S_OK表示关闭成功.
	返回E_HANDLE表示hFile为NULL.
	返回E_FAIL表示关闭文件失败. */
extern HRESULT CloseIniFile(
	INIFILE_HANDLE			hFile			//指向Ini文件句柄.
);

#ifdef __cplusplus
	}
#endif /* #ifdef __cplusplus */

#endif //_INI_OPERATION_H__
