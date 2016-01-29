#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "HvVarTypeEx.h"

CHvString::CHvString()
	:m_iStrLen(0)
{
	m_szBuffer[0] = 0;
}

CHvString::CHvString(const char* szString)
{
	if (szString == NULL)
	{
		m_iStrLen = 0;
		m_szBuffer[0] = 0;
	}
	else
	{
		m_iStrLen = (int)strlen(szString);
		if (m_iStrLen > MAX_STRING_SIZE )
		{
			m_iStrLen = MAX_STRING_SIZE;
		}
		memcpy(m_szBuffer, szString, m_iStrLen);
		m_szBuffer[m_iStrLen] = 0;
	}
}

CHvString::~CHvString()
{
}

int CHvString::Format(const char* format, ...)
{
	va_list arglist;
	int iRetVal;

	va_start(arglist, format);
	iRetVal = vsprintf(m_szBuffer, format, arglist);
	va_end(arglist);
	m_iStrLen = (int)strlen(m_szBuffer);
	return iRetVal;
}

char* CHvString::GetBuffer()
{
	return m_szBuffer;
}

void CHvString::ReleaseBuffer()
{
	m_iStrLen = (int)strlen(m_szBuffer);
	if (m_iStrLen > MAX_STRING_SIZE)
	{
		m_iStrLen = MAX_STRING_SIZE;
		m_szBuffer[m_iStrLen] = 0;
	}
}

CHvString::operator const char*()
{
	return m_szBuffer;
}

CHvString &CHvString::operator =(const CHvString &str)
{
	m_iStrLen = str.m_iStrLen;
	memcpy(m_szBuffer, str.m_szBuffer, m_iStrLen);
	m_szBuffer[m_iStrLen] = 0;
	return *this;
}

CHvString &CHvString::operator =(const char* pchar)
{
	if (pchar == NULL)
	{
		m_iStrLen = 0;
		m_szBuffer[0] = 0;
	}
	else
	{
		m_iStrLen = (int)strlen(pchar);
		if (m_iStrLen > MAX_STRING_SIZE)
		{
			m_iStrLen = MAX_STRING_SIZE;
		}
		memcpy(m_szBuffer, pchar, m_iStrLen);
		m_szBuffer[m_iStrLen] = 0;
	}
	return *this;
}

CHvString &CHvString::operator +=(const CHvString &str)
{
	int iAppendLen = str.m_iStrLen;
	if (m_iStrLen + iAppendLen > MAX_STRING_SIZE)
	{
		iAppendLen = MAX_STRING_SIZE - m_iStrLen;
	}
	memcpy(m_szBuffer + m_iStrLen, str.m_szBuffer, iAppendLen);
	m_iStrLen += iAppendLen;
	m_szBuffer[m_iStrLen] = 0;
	return *this;
}

CHvString &CHvString::operator +=(const char* pchar)
{
	if (pchar == NULL)
	{
		return *this;
	}
	int iAppendLen = (int)strlen(pchar);
	if (m_iStrLen + iAppendLen > MAX_STRING_SIZE)
	{
		iAppendLen = MAX_STRING_SIZE - m_iStrLen;
	}
	memcpy(m_szBuffer + m_iStrLen, pchar, iAppendLen);
	m_iStrLen += iAppendLen;
	m_szBuffer[m_iStrLen] = 0;
	return *this;
}

CHvString &CHvString::operator +=(const char ch)
{ 
	if (m_iStrLen == MAX_STRING_SIZE)
	{
		return *this;
	}
	m_szBuffer[m_iStrLen] = ch;
	m_iStrLen++;
	m_szBuffer[m_iStrLen] = 0;
	return *this;
}

CHvString CHvString::operator +(const CHvString &str)
{
	CHvString strTmp(*this);
	strTmp += str;
	return strTmp;
}

CHvString CHvString::operator +(const char* pchar)
{
	CHvString strTmp(*this);
	strTmp += pchar;
	return strTmp;
}

bool CHvString::operator !=(const char* pchar)
{
	return (strcmp(m_szBuffer, pchar) != 0);
}

bool CHvString::operator !=(const CHvString &str)
{
	return (strcmp(m_szBuffer, str.m_szBuffer) != 0);
}

bool CHvString::operator ==(char* pchar)
{
	return (strcmp(m_szBuffer, pchar) == 0);
}

bool CHvString::operator >=(const CHvString &str)
{
	return (strcmp(m_szBuffer, str.m_szBuffer) >= 0);
}

bool CHvString::operator <=(const CHvString &str)
{
	return (strcmp(m_szBuffer, str.m_szBuffer) <= 0);
}

bool CHvString::operator ==(const CHvString &str)
{
	return (strcmp(m_szBuffer, str.m_szBuffer) == 0);
}

char CHvString::operator [](int iIndex)
{
	if (iIndex >= m_iStrLen)
	{
		return 0;
	}
	return m_szBuffer[iIndex];
}

int CHvString::Insert(int iIndex, const char* psz)
{
	if (iIndex > m_iStrLen)
	{
		return m_iStrLen;
	}
	CHvString strTmp(m_szBuffer + iIndex);
	m_szBuffer[iIndex] = 0;
	m_iStrLen = iIndex;
	*this += psz;
	*this += strTmp;
	return m_iStrLen;
}

int CHvString::GetLength()
{
	return m_iStrLen;
}

bool CHvString::IsEmpty()
{
	return (m_iStrLen == 0);
}

void CHvString::Append(const CHvString &str)
{
	*this += str;
}

CHvString CHvString::MakeLower()
{
	CHvString strTmp(m_szBuffer);
	char* pch = strTmp.m_szBuffer;
	for (int i = 0; i < strTmp.m_iStrLen; i++)
	{
		if (pch[i] >= 'A' && pch[i] <= 'Z')
		{
			pch[i] += ('a' - 'A');
		}
	}
	return strTmp;
}

CHvString CHvString::MakeUpper()
{
	CHvString strTmp(m_szBuffer);
	char* pch = strTmp.m_szBuffer;
	for (int i = 0; i < strTmp.m_iStrLen; i++)
	{
		if (pch[i] >= 'a' && pch[i] <= 'z')
		{
			pch[i] += ('A' - 'a');
		}
	}
	return strTmp;
}

CHvString CHvString::Left(int nCount)
{
	if (nCount < 0)
	{
		return "";
	}
	if (nCount > m_iStrLen)
	{
		nCount = m_iStrLen;
	}
	CHvString strTmp(*this);
	strTmp.m_szBuffer[nCount] = 0;
	strTmp.m_iStrLen = nCount;
	return strTmp;
}

CHvString CHvString::Mid(int iFirst)
{
	CHvString strTmp;
	if (iFirst > m_iStrLen - 1)
	{
		return strTmp;
	}
	strTmp = m_szBuffer + iFirst;
	return strTmp;
}

CHvString CHvString::Mid(int iFirst, int nCount)
{
	CHvString strTmp;
	if (iFirst > m_iStrLen - 1)
	{
		return strTmp;
	}
	strTmp = m_szBuffer + iFirst;
	if (strTmp.m_iStrLen >= nCount)
	{
		strTmp.m_szBuffer[nCount] = 0;
		strTmp.m_iStrLen = nCount;
	}
	return strTmp;
}

int CHvString::Remove(char ch)
{
	int nCount = 0;
	int iIndex = 0;
	char *pTemp =  m_szBuffer;
	while (*pTemp)
	{
		if (*pTemp != ch)
		{
			m_szBuffer[iIndex++] = *pTemp;
		}
		else
		{
			nCount++;
		}
		pTemp++;
	}
	m_iStrLen -= nCount;
	m_szBuffer[m_iStrLen] = 0;
	return nCount;
}

int CHvString::Find(const char* pszSub, int iStart/* = 0*/)
{
	if (iStart >= m_iStrLen)
	{
		return -1;
	}
	char *pTemp = strstr(m_szBuffer + iStart, pszSub);
	if (pTemp == NULL)
	{
		return -1;
	}
	return (int)(pTemp - m_szBuffer);
}

