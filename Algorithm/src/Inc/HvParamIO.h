/*
 *	(C)版权所有 2010 北京信路威科技发展有限公司
 */

/**
* @file		HvParamIO.h
* @version	1.0
* @brief	对识别器端参数进行读写操作的接口
* @author	Shaorg
* @date		2010-8-6
*/

#ifndef _HVPARAMIO_H_
#define _HVPARAMIO_H_

#include "hvutils.h"
#include "HvParamStore.h"

/**
* @brief		写入键值类型为整型的键值及其相关附加信息，并存储。
* @param[in]	pParam			一个已初始化的CParamStore类指针
* @param[in]	szSection		节名
* @param[in]	szKey			键名
* @param[in]	iIntVar		写入的键值
* @param[in]	iDefault		默认值
* @param[in]	iMin			最小值
* @param[in]	iMax			最大值
* @param[in]	szChName		中文名
* @param[in]	szComment		注释
* @param[in]	nRank			权限等级
* @param[in]	fSaveNow		是否立即存储
* @return		成功：S_OK，失败：E_FAIL
*/
HRESULT HvParamWriteInt(
    CParamStore* pParam,
    LPCSTR szSection,
    LPCSTR szKey,
    INT iIntVar,
    INT iDefault,
    INT iMin,
    INT iMax,
    LPCSTR szChName,
    LPCSTR szComment,
    BYTE8 nRank,
    BOOL fSaveNow = FALSE
);

/**
* @brief		写入键值类型为浮点型的键值及其相关附加信息，并存储。
* @param[in]	pParam			一个已初始化的CParamStore类指针
* @param[in]	szSection		节名
* @param[in]	szKey			键名
* @param[in]	iFloatVar		写入的键值
* @param[in]	fltDefault		默认值
* @param[in]	fltMin			最小值
* @param[in]	fltMax			最大值
* @param[in]	szChName		中文名
* @param[in]	szComment		注释
* @param[in]	nRank			权限等级
* @param[in]	fSaveNow		是否立即存储
* @return		成功：S_OK，失败：E_FAIL
*/
HRESULT HvParamWriteFloat(
    CParamStore* pParam,
    LPCSTR szSection,
    LPCSTR szKey,
    FLOAT iFloatVar,
    FLOAT fltDefault,
    FLOAT fltMin,
    FLOAT fltMax,
    LPCSTR szChName,
    LPCSTR szComment,
    BYTE8 nRank,
    BOOL fSaveNow = FALSE
);

/**
* @brief		写入键值类型为字符串型的键值及其相关附加信息，并存储。
* @param[in]	pParam			一个已初始化的CParamStore类指针
* @param[in]	szSection		节名
* @param[in]	szKey			键名
* @param[in]	pszStr			写入的键值
* @param[in]	iLen			缓冲区pszStr的大小
* @param[in]	szChName		中文名
* @param[in]	szComment		注释
* @param[in]	nRank			权限等级
* @param[in]	fSaveNow		是否立即存储
* @return		成功：S_OK，失败：E_FAIL
*/
HRESULT HvParamWriteString(
    CParamStore* pParam,
    LPCSTR szSection,
    LPCSTR szKey,
    LPCSTR szStr,
    INT iLen,
    LPCSTR szChName,
    LPCSTR szComment,
    BYTE8 nRank,
    BOOL fSaveNow = FALSE
);

/**
* @brief		写入键值类型为二进制型的键值及其相关附加信息，并存储。
* @param[in]	pParam			一个已初始化的CParamStore类指针
* @param[in]	szSection		节名
* @param[in]	szKey			键名
* @param[in]	pBinData		写入的键值
* @param[in]	iLen			pBinData的大小
* @param[in]	szChName		中文名
* @param[in]	szComment		注释
* @param[in]	nRank			权限等级
* @param[in]	fSaveNow		是否立即存储附加信息
* @return		成功：S_OK，失败：E_FAIL
*/
HRESULT HvParamWriteBin(
    CParamStore* pParam,
    LPCSTR szSection,
    LPCSTR szKey,
    LPCVOID pBinData,
    INT iLen,
    LPCSTR szChName,
    LPCSTR szComment,
    BYTE8 nRank,
    BOOL fSaveNow = FALSE
);

/**
* @brief		读取键值类型为整型的键值
* @param[in]	pParam			一个已初始化的CParamStore类指针
* @param[in]	szSection		节名
* @param[in]	szKey			键名
* @param[out]	piVal			读取到的整数值
* @param[in]	iDefault		读取失败时的默认值
* @return		成功：S_OK，失败：S_FALSE
*/
HRESULT HvParamReadInt(
    CParamStore* pParam,
    LPCSTR szSection,
    LPCSTR szKey,
    INT* piVal,
    INT iDefault
);

/**
* @brief		读取键值类型为浮点型的键值
* @param[in]	pParam			一个已初始化的CParamStore类指针
* @param[in]	szSection		节名
* @param[in]	szKey			键名
* @param[out]	fltVal			读取到的浮点值
* @param[in]	fltDefault		读取失败时的默认值
* @return		成功：S_OK，失败：S_FALSE
*/
HRESULT HvParamReadFloat(
    CParamStore* pParam,
    LPCSTR szSection,
    LPCSTR szKey,
    FLOAT* pfltVal,
    FLOAT fltDefault
);

/**
* @brief		读取键值类型为字符串型的键值
* @param[in]	pParam			一个已初始化的CParamStore类指针
* @param[in]	szSection		节名
* @param[in]	szKey			键名
* @param[out]	szString		读取到的字符串
* @param[in]	iLen			szString的大小
* @return		成功：S_OK，失败：S_FALSE
*/
HRESULT HvParamReadString(
    CParamStore* pParam,
    LPCSTR szSection,
    LPCSTR szKey,
    LPSTR szString,
    INT iLen
);

/**
* @brief			读取键值类型为二进制型的键值
* @param[in]		pParam			一个已初始化的CParamStore类指针
* @param[in]		szSection		节名
* @param[in]		szKey			键名
* @param[out]		pBuf			读取到的二进制数据
* @param[int,out]	piBufLen		缓冲区pBuf的大小，函数返回后，其值为pBuf中数据的大小
* @return			成功：S_OK，失败：S_FALSE
*/
HRESULT HvParamReadBin(
    CParamStore* pParam,
    LPCSTR szSection,
    LPCSTR szKey,
    LPVOID pBuf,
    INT* piBufLen
);

/**
* @brief			获取xml格式的总参数
* @return			成功：S_OK
*/
HRESULT GetParamXml(
    CParamStore* pParamStore,
    char* szBuf,
    int nBufLen,
    DWORD32 dwFlag = 0
);

/**
* @brief			以xml格式设置总参数
* @return			成功：S_OK
*/
HRESULT SetParamXml(
    CParamStore* pParamStore,
    char* szBuf,
    DWORD32 dwFlag = 0
);

/**
* @brief			带“自动创建并写入”功能的整型参数读取
* @return			成功：S_OK
*/
HRESULT HvParamReadIntWithWrite(
    CParamStore* pParam,
    const char* szSection,
    const char* szKey,
    int* pVal,
    int nDefault,
    int nMin,
    int nMax,
    const char* szChName,
    const char* szComment,
    BYTE8 nRank,
    BOOL fSaveNow = FALSE
);

/**
* @brief			带“自动创建并写入”功能的浮点型参数读取
* @return			成功：S_OK
*/
HRESULT HvParamReadFloatWithWrite(
    CParamStore* pParam,
    const char* szSection,
    const char* szKey,
    float* pVal,
    float fltDefault,
    float fltMin,
    float fltMax,
    const char* szChName,
    const char* szComment,
    BYTE8 nRank,
    BOOL fSaveNow = FALSE
);

/**
* @brief			带“自动创建并写入”功能的字符串参数读取
* @return			成功：S_OK
*/
HRESULT HvParamReadStringWithWrite(
    CParamStore* pParam,
    const char* szSection,
    const char* szKey,
    char* szRet,
    int nLen,
    const char* szChName,
    const char* szComment,
    BYTE8 nRank,
    BOOL fSaveNow = FALSE
);

/**
* @brief			将“主CPU”端和“从CPU”端的xml参数合并为一个。
* @return			成功：S_OK
*/
HRESULT CombineMasterSlaveXmlParam(
    const char* szXmlParamMaster,
    const char* szXmlParamSlave,
    char* szXmlParamAll,
    DWORD32& dwLen
);

/**
* @brief            恢复默认参数
* @return           成功：S_OK
*/
HRESULT RestoreDefaultParam();

/**
* @brief            恢复出厂参数
* @return           成功：S_OK
*/
HRESULT RestoreFactoryParam();

/**
* @brief            判断是否恢复出厂设置
* @return           是：true，否：false
*/
bool IsRecoverFactorySettings();

/**
* @brief            恢复出厂设置
* @return           成功：S_OK，失败：E_FAIL
*/
int RecoverFactorySettings();

#endif
