///////////////////////////////////////////////////////////
// CSWString.cpp
// Implementation of the Class CSWString
// Created on:   28-二月-2013 14:09:48
// Original author: Shaorg
///////////////////////////////////////////////////////////

#include "SWString.h"

VOID CSWString::InnerInit()
{
    m_szStr = NULL;
    m_iStrLen = 0;
    m_iContainerSize = 0;

    m_iGrowSize = 256;
}

CSWString::CSWString(){

    InnerInit();
}

CSWString::CSWString(CHAR ch){

    CHAR szStr[2];
    szStr[0] = ch;
    szStr[1] = '\0';

    InnerInit();
    Append(szStr);
}

CSWString::CSWString(const CHAR* szStr){

    InnerInit();
    Append(szStr);
}

CSWString::CSWString(const CSWString& Str){

    m_iGrowSize = Str.m_iGrowSize;
    m_iStrLen = Str.m_iStrLen;
    m_iContainerSize = Str.m_iContainerSize;

    if ( Str.m_szStr != NULL )
    {
        m_szStr = new CHAR[m_iContainerSize];
        StrCpy(m_szStr, Str.m_szStr);
    }
    else
    {
        m_szStr = NULL;
    }
}

CSWString::~CSWString(){

    Clear();
}

HRESULT CSWString::Clear(){

    if ( m_szStr != NULL )
    {
        delete[] m_szStr;
        m_szStr = NULL;

        m_iStrLen = 0;
        m_iContainerSize = 0;
    }

	return S_OK;
}

DWORD CSWString::Length(){

    return m_iStrLen;
}

DWORD CSWString::FindCharCount(CHAR chChar)
{
    DWORD dwCount = 0;
    for (int i = 0; i < m_iStrLen; ++i)
    {
        if (m_szStr[i] == chChar)
        {
            ++ dwCount;
        }
    }
    return dwCount;
}

BOOL CSWString::IsEmpty(){

    return (m_iStrLen <= 0) ? (TRUE) : (FALSE);
}

CSWString& CSWString::Append(const CHAR* szStr){

    if ( NULL == szStr || !StrLen(szStr))
    {
        return (*this);
    }

    m_iStrLen += StrLen(szStr);

    if ( m_iStrLen < m_iContainerSize ) // 原字串附加新字串后的总长度小于容器当前长度
    {
        StrCat(m_szStr, szStr);
    }
    else
    {
        // 计算所需容器大小
    	// todo. 不应该会有0
    	if( m_iGrowSize == 0 ) m_iGrowSize = 256;
        m_iContainerSize = m_iGrowSize * (m_iStrLen/m_iGrowSize + 1);

        if ( m_szStr != NULL )
        {
            CHAR* szStrOld = m_szStr;

            m_szStr = new CHAR[m_iContainerSize];
            StrCpy(m_szStr, szStrOld);
            StrCat(m_szStr, szStr);

            delete[] szStrOld;
            szStrOld = NULL;
        }
        else
        {
            m_szStr = new CHAR[m_iContainerSize];
            StrCpy(m_szStr, szStr);
        }
    }

	return (*this);
}

CSWString& CSWString::Append(CHAR chChar, DWORD dwNum){

    CHAR* szStrTmp = new CHAR[dwNum+1];
    MemSet(szStrTmp, chChar, dwNum);
    szStrTmp[dwNum] = '\0';
    Append(szStrTmp);
    delete[] szStrTmp;
    szStrTmp = NULL;

	return (*this);
}

CSWString& CSWString::Append(const CSWString& sStr){

    Append(sStr.m_szStr);
	return (*this);
}

INT CSWString::Compare(const CSWString& sStr){

	return StrCmp(m_szStr, sStr.m_szStr);
}

INT CSWString::Compare(const CHAR* szStr){

	return StrCmp(m_szStr, szStr);
}

CSWString& CSWString::Format(const CHAR* szFmt, ...){

	*this = "";
	CHAR *szTmp = new CHAR[1024];
	swpa_va_list marker;
	swpa_va_start(marker, szFmt);
    swpa_vsprintf(szTmp, szFmt, marker);
    swpa_va_end(marker);
    Append(szTmp);
    delete []szTmp;
    return *this;
}

CSWString& CSWString::Remove(const CHAR chChar){

    CHAR szStr[2];
    szStr[0] = chChar;
    szStr[1] = '\0';
    
    return Remove(szStr);
}

CSWString& CSWString::Remove(const CHAR* szStr){

    return Replace(szStr, "");
}

/**
 * 拷贝自己的dwNum个字符到szStr中（从索引dwIndex开始）。返回值是拷贝的字符数
 */
DWORD CSWString::Copy(CHAR* szStr, DWORD dwNum, DWORD dwIndex){

    if ( szStr != NULL && (INT)dwIndex >= 0 && (INT)dwIndex < m_iStrLen && dwNum <= (m_iStrLen - dwIndex) ) // 参数合法
    {
		DWORD n = 0;
        for ( ; n < dwNum; ++n )
        {
            szStr[n] = m_szStr[n + dwIndex];
        }
        szStr[n] = '\0';
    }

	return dwNum;
}

CSWString& CSWString::Erase(DWORD dwPos, DWORD dwNum){

    if ( (INT)dwPos >= 0 && (INT)dwPos < m_iStrLen && dwNum <= (m_iStrLen - dwPos) ) // 参数合法
    {
        CHAR* szStrTmp = new CHAR[m_iStrLen-dwNum+1];
        INT iIndexStrTmp = 0;

        INT n;
        for ( n = 0; n < (INT)dwPos; ++n )
        {
            szStrTmp[iIndexStrTmp++] = m_szStr[n];
        }
        for ( n = dwPos+dwNum; n < m_iStrLen; ++n )
        {
            szStrTmp[iIndexStrTmp++] = m_szStr[n];
        }
        szStrTmp[iIndexStrTmp] = '\0';

        (*this) = szStrTmp;

        delete[] szStrTmp;
        szStrTmp = NULL;
    }

	return (*this);
}

/**
 * 返回chChar在字符串中第一次出现的位置（从dwIndex开始查找）
 */
INT CSWString::Find(CHAR chChar, DWORD dwIndex){

    if ( (INT)dwIndex >= 0 && (INT)dwIndex < m_iStrLen )
    {
        for ( INT n = dwIndex; n < m_iStrLen; ++n )
        {
            if ( m_szStr[n] == chChar )
            {
                return n;
            }
        }
    }

	return -1;
}

/**
 * 返回szStr在字符串中第一次出现的位置（从dwIndex开始查找）
 */
INT CSWString::Find(const CHAR* szStr, DWORD dwIndex){

	if ( szStr != NULL && (INT)dwIndex >= 0 && (INT)dwIndex < m_iStrLen )
    {
        INT iLen = StrLen(szStr);

        if ( iLen > 0 )
        {
            INT iCount = m_iStrLen - iLen + 1;
            for ( INT n = dwIndex; n < iCount; ++n )
            {
                if ( 0 == MemCmp(m_szStr+n, szStr, iLen) )
                {
                    return n;
                }
            }
        }
    }

	return -1;
}

/**
 * 返回sStr在字符串中第一次出现的位置（从dwIndex开始查找）
 */
INT CSWString::Find(const CSWString& sStr, DWORD dwIndex){

	return Find(sStr.m_szStr, dwIndex);
}

INT CSWString::Insert(DWORD dwPos, CHAR chChar, DWORD dwNum){

    CHAR* szStrTmp = new CHAR[dwNum+1];
    MemSet(szStrTmp, chChar, dwNum);
    szStrTmp[dwNum] = '\0';
    
    INT iRet = Insert(dwPos, szStrTmp);

    delete[] szStrTmp;
    szStrTmp = NULL;

	return iRet;
}

INT CSWString::Insert(DWORD dwPos, const CHAR* szStr){

    if ( szStr != NULL )
    {
        INT iLen = StrLen(szStr);
        if ( iLen > 0 )
        {
            if ( dwPos < 0 )
            {
                dwPos = 0;
            }

            if ( (INT)dwPos > m_iStrLen )
            {
                dwPos = m_iStrLen;
            }

            CHAR* szStrTmp = new CHAR[m_iStrLen+iLen+1];

            DWORD n = 0;
            for ( n = 0; n < dwPos; ++n )
            {
                szStrTmp[n] = m_szStr[n];
            }
            szStrTmp[n] = '\0';
            StrCat(szStrTmp, szStr);
            StrCat(szStrTmp, m_szStr+dwPos);

            (*this) = szStrTmp;

            delete[] szStrTmp;
            szStrTmp = NULL;

            return 0;
        }
    }

	return -1;
}

INT CSWString::Insert(DWORD dwPos, const CSWString& sStr){

	return Insert(dwPos, sStr.m_szStr);
}

CSWString& CSWString::Replace(const CHAR* szStrOld, const CHAR* szStrNew)
{
    CSWString strResult;
    CSWString strSub;

    DWORD dwPosStart = 0;
    DWORD dwPosFound = 0;

    if ( NULL == szStrOld || NULL == szStrNew )
    {
        return (*this);
    }

    if ( -1 == Find(szStrOld, dwPosStart) )
    {
        return (*this);
    }

    INT iStrOldLen = StrLen(szStrOld);

    while (true)
    {
        dwPosFound = Find(szStrOld, dwPosStart);
        if ( dwPosFound != -1 )
        {
            if ( dwPosFound - dwPosStart > 0 )
            {
                strSub = Substr(dwPosStart, dwPosFound - dwPosStart);
                strResult.Append(strSub);
            }
            strResult.Append(szStrNew);

            dwPosStart = dwPosFound + iStrOldLen;
        }
        else
        {
            dwPosFound = Length();
            if ( dwPosFound - dwPosStart > 0 )
            {
                strSub = Substr(dwPosStart, dwPosFound - dwPosStart);
                strResult.Append(strSub);
            }

            break;
        }
    }

    (*this) = strResult;
    return (*this);
}

/**
 * 返回本字符串的一个子串，从dwIndex开始，长dwNum个字符。如果没有指定，将是默认值 CString::
 * npos。这样，Substr()函数将简单的返回从dwIndex开始的剩余的字符串
 */
CSWString CSWString::Substr(DWORD dwPos, DWORD dwNum){

    CSWString strNew;
    
    if(!dwNum)
    {
    	dwNum = m_iStrLen - dwPos;
    }

    if ( (INT)dwPos >= 0 && (INT)dwPos < m_iStrLen && dwNum <= (m_iStrLen - dwPos) ) // 参数合法
    {
        strNew = (*this); // 为了分配内存
        strNew.m_iStrLen = dwNum;

		DWORD n = 0;
        for ( ; n < dwNum; ++n )
        {
            strNew.m_szStr[n] = m_szStr[n + dwPos];
        }
        strNew.m_szStr[n] = '\0';
    }

	return strNew;
}

CSWString& CSWString::ToLower(){

    for ( INT n = 0; n < m_iStrLen; ++n )
    {
        if ( m_szStr[n] >= 'A' && m_szStr[n] <= 'Z' )
        {
            m_szStr[n] -= ('A' - 'a');
        }
    }

	return (*this);
}

CSWString& CSWString::ToUpper(){

    for ( INT n = 0; n < m_iStrLen; ++n )
    {
        if ( m_szStr[n] >= 'a' && m_szStr[n] <= 'z' )
        {
            m_szStr[n] += ('A' - 'a');
        }
    }

	return (*this);
}

CSWString::operator const char*()
{
    return NULL == m_szStr ? "" : m_szStr;
}

CHAR CSWString::operator[](INT sdwIdx){

    if ( 0 <= sdwIdx && sdwIdx < (INT)Length() )
	{
		return m_szStr[sdwIdx];
	}
	else
	{
		return 0;
	}
}

CSWString CSWString::operator+(const CSWString& Str){

    CSWString strNew(*this);
    strNew += Str;
	return strNew;
}

CSWString CSWString::operator+(const CHAR* szStr){

	CSWString strNew(*this);
    strNew += szStr;
	return strNew;
}

CSWString CSWString::operator+(CHAR chChar){

	CSWString strNew(*this);
    strNew += chChar;
	return strNew;
}

CSWString& CSWString::operator+=(const CSWString& Str){

    Append(Str);
	return (*this);
}

CSWString& CSWString::operator+=(const CHAR* szStr){

    Append(szStr);
	return (*this);
}

CSWString& CSWString::operator+=(CHAR chChar){

    Append(chChar, 1);
	return (*this);
}

CSWString& CSWString::operator=(const CSWString& Str){

    Clear();
    Append(Str);
	return (*this);
}

CSWString& CSWString::operator=(const CHAR* szStr){

    Clear();
    Append(szStr);
	return (*this);
}

CSWString& CSWString::operator=(CHAR chChar){

    Clear();
    Append(chChar, 1);
	return (*this);
}

BOOL CSWString::operator!=(const CSWString& Str){

	return ( Compare(Str) != 0 ) ? (TRUE) : (FALSE);
}

BOOL CSWString::operator==(const CSWString& Str){

    return ( Compare(Str) == 0 ) ? (TRUE) : (FALSE);
}

BOOL CSWString::operator>(const CSWString& Str){

	return ( Compare(Str) > 0 ) ? (TRUE) : (FALSE);
}

BOOL CSWString::operator>=(const CSWString& Str){

	return ( Compare(Str) >= 0 ) ? (TRUE) : (FALSE);
}

BOOL CSWString::operator<(const CSWString& Str){

	return ( Compare(Str) < 0 ) ? (TRUE) : (FALSE);
}

BOOL CSWString::operator<=(const CSWString& Str){

	return ( Compare(Str) <= 0 ) ? (TRUE) : (FALSE);
}

// 目前都通过使用C库实现以下这些基础函数

DWORD CSWString::StrLen(const CHAR* szFrom)
{
	if(NULL != szFrom)
	{
		return swpa_strlen(szFrom);
	}
	return 0;
}

INT CSWString::StrCmp(const CHAR* szStr1, const CHAR* szStr2)
{
	if(NULL != szStr1 && NULL != szStr2)
	{
		return swpa_strcmp(szStr1, szStr2);
	}
	return -1;
}


INT CSWString::StrICmp(const CHAR* szStr1, const CHAR* szStr2)
{
	if(NULL != szStr1 && NULL != szStr2)
	{
		return swpa_stricmp(szStr1, szStr2);
	}
	return -1;
}


INT CSWString::StrNICmp(const CHAR* szStr1, const CHAR* szStr2, const DWORD dwLen)
{
	if(NULL != szStr1 && NULL != szStr2 && dwLen >= 0)
	{
		return swpa_strnicmp(szStr1, szStr2, dwLen);
	}
	return -1;
}

CHAR* CSWString::StrCat(CHAR* szTo, const CHAR* szFrom)
{
	if(NULL != szTo && NULL != szFrom)
  {
  	return swpa_strcat(szTo, szFrom);
  }
  return szTo;
}

CHAR* CSWString::StrCpy(CHAR* szStr1, const CHAR* szStr2)
{
	if(NULL != szStr1 && NULL != szStr2)
  {
  	 return swpa_strcpy(szStr1, szStr2);
  }
  return szStr1;
}

CHAR* CSWString::StrNCpy(CHAR* szStr1, const CHAR* szStr2, DWORD dwLen)
{
	if(NULL != szStr1 && NULL != szStr2)
  {
  	return swpa_strncpy(szStr1, szStr2, dwLen);
  }
  return szStr1;
}

PVOID CSWString::MemSet(CHAR* szStr, CHAR ch, DWORD dwLen)
{
	if(NULL != szStr)
  {
  	return swpa_memset(szStr, ch, dwLen);
  }
  return NULL;
}

INT CSWString::MemCmp(const CHAR* szStr1, const CHAR* szStr2, DWORD dwLen)
{
	if(NULL != szStr1 && NULL != szStr2)
  {
  	return swpa_memcmp(szStr1, szStr2, dwLen);
  }
  return -1;
}

