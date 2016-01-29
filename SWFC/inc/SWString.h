///////////////////////////////////////////////////////////
//  CSWString.h
//  Implementation of the Class CSWString
//  Created on:      28-二月-2013 14:09:48
//  Original author: zy
///////////////////////////////////////////////////////////

#if !defined(EA_26483279_A9CA_43ae_9ADE_F3DB4F81F3F9__INCLUDED_)
#define EA_26483279_A9CA_43ae_9ADE_F3DB4F81F3F9__INCLUDED_

#include "SWObject.h"

/**
 * @brief 字符串基类
 */
class CSWString : public CSWObject
{
CLASSINFO(CSWString,CSWObject)

public:
	CSWString();
	CSWString(CHAR ch);
	CSWString(const CHAR* szStr);
	CSWString(const CSWString& Str);
	virtual ~CSWString();

    /**
	 * @brief 清空字串
	 */
	HRESULT Clear();

    /**
	 * @brief 获取字串长度
	 */
    DWORD Length();

    /**
     * @brief 获取指定字符个数
     */
    DWORD FindCharCount(CHAR chChar);

    /**
	 * @brief 判断字串是否为空
     * 
     * @return
	 * -TRUE : 为空
	 * -FALSE : 非空
	 */
    BOOL IsEmpty();

    /**
	 * @brief 将szStr附加到当前字串尾部。
	 */
	CSWString& Append(const CHAR* szStr);
    /**
	 * @brief 将dwNum个chChar附加到当前字串尾部。
	 */
	CSWString& Append(CHAR chChar, DWORD dwNum);
    /**
	 * @brief 将sStr附加到当前字串尾部。
	 */
	CSWString& Append(const CSWString& sStr);

    /**
	 * @brief 将当前字串与sStr比较大小。
     * @return
     * - < 0 : 当前字串小于sStr
	 * - = 0 : 当前字串等于sStr
     * - > 0 : 当前字串大于sStr
	 */
	INT Compare(const CSWString& sStr);
    /**
	 * @brief 将当前字串与szStr比较大小。
     * @return
     * - < 0 : 当前字串小于szStr
	 * - = 0 : 当前字串等于szStr
     * - > 0 : 当前字串大于szStr
	 */
	INT Compare(const CHAR* szStr);
	
    /**
	 * @brief 格式化字符串
	 */
	CSWString& Format(const CHAR* szFmt, ...);
	
    /**
	 * @brief 移除指定字符
	 */
	CSWString& Remove(const CHAR chChar);
	
    /**
	 * @brief 移除指定字符串
	 */
	CSWString& Remove(const CHAR* szStr);

	/**
	 * @brief 拷贝自己的dwNum个字符到szStr中（从索引dwIndex开始）。返回值是拷贝的字符数
	 */
	DWORD Copy(CHAR* szStr, DWORD dwNum, DWORD dwIndex);

    /**
	 * @brief 擦除自己的dwNum个字符（从索引dwIndex开始）
	 */
	CSWString& Erase(DWORD dwPos, DWORD dwNum);

    /**
	 * @brief 返回chChar在字符串中第一次出现的位置（从dwIndex开始查找）
	 */
	INT Find(CHAR chChar, DWORD dwIndex=0);
	/**
	 * @brief 返回szStr在字符串中第一次出现的位置（从dwIndex开始查找）
	 */
	INT Find(const CHAR* szStr, DWORD dwIndex=0);
	/**
	 * @brief 返回sStr在字符串中第一次出现的位置（从dwIndex开始查找）
	 */
	INT Find(const CSWString& sStr, DWORD dwIndex=0);

    /**
	 * @brief 将dwNum个chChar插入到位置dwPos处。
	 */
	INT Insert(DWORD dwPos, CHAR chChar, DWORD dwNum);
    /**
	 * @brief 将szStr插入到位置dwPos处。
	 */
	INT Insert(DWORD dwPos, const CHAR* szStr);
    /**
	 * @brief 将sStr插入到位置dwPos处。
	 */
    INT Insert(DWORD dwPos, const CSWString& sStr);

    /**
	 * @brief 将本字符串中的所有szStrOld替换为szStrNew
	 */
    CSWString& Replace(const CHAR* szStrOld, const CHAR* szStrNew);

	/**
	 * @brief 返回本字符串的一个子串，从dwIndex开始，长dwNum个字符。
	 */
	CSWString Substr(DWORD dwPos, DWORD dwNum=0);

    /**
	 * @brief 将字串中的所有大写字母变为小写。
	 */
	CSWString& ToLower();

    /**
	 * @brief 将字串中的所有小写字母变为大写。
	 */
	CSWString& ToUpper();

    operator const char*();

    CHAR operator[](INT sdwIdx);

	CSWString operator+(const CSWString& Str);
	CSWString operator+(const CHAR* szStr);
	CSWString operator+(CHAR chChar);

	CSWString& operator+=(const CSWString& Str);
	CSWString& operator+=(const CHAR* szStr);
	CSWString& operator+=(CHAR chChar);
	CSWString& operator=(const CSWString& Str);
	CSWString& operator=(const CHAR* szStr);
	CSWString& operator=(CHAR chChar);

    BOOL operator!=(const CSWString& Str);
    BOOL operator<(const CSWString& Str);
	BOOL operator<=(const CSWString& Str);
	BOOL operator==(const CSWString& Str);
	BOOL operator>(const CSWString& Str);
	BOOL operator>=(const CSWString& Str);

public:
    static DWORD StrLen(const CHAR* szFrom);
	static INT StrCmp(const CHAR* szStr1, const CHAR* szStr2);
	static INT StrICmp(const CHAR* szStr1, const CHAR* szStr2);
	static INT StrNICmp(const CHAR* szStr1, const CHAR* szStr2, const DWORD dwLen);
	static CHAR* StrCat(CHAR* szTo, const CHAR* szFrom);
    static CHAR* StrCpy(CHAR* szStr1, const CHAR* szStr2);
    static CHAR* StrNCpy(CHAR* szStr1, const CHAR* szStr2, DWORD dwLen);
    static PVOID MemSet(CHAR* szStr, CHAR ch, DWORD dwLen);
    static INT MemCmp(const CHAR* szStr1, const CHAR* szStr2, DWORD dwLen);

private:
    VOID InnerInit();

	CHAR* m_szStr; // 字符串指针
    INT m_iStrLen; // 字符串当前长度
    INT m_iContainerSize; // 容器当前大小

    INT m_iGrowSize; // 容器每次增减的长度
};

#endif // !defined(EA_26483279_A9CA_43ae_9ADE_F3DB4F81F3F9__INCLUDED_)

