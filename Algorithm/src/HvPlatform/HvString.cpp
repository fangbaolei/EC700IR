#include "HvString.h"
#include <stdio.h>
#include <stdarg.h>

namespace HiVideo
{
	CHvString::CHvString()
	{
	}

	CHvString::CHvString(const char* szString)
	{
		m_str = szString;
	}

	CHvString::~CHvString()
	{
	}

	//-------------------------
	//CHvString类成员函数的实现
	//-------------------------

	const char* CHvString::GetBuffer()
	{
		return m_str.c_str();
	}

	bool CHvString::IsEmpty()
	{
		return m_str.empty();
	}

	void CHvString::Append(const CHvString& str)
	{
		m_str += str.m_str;
	}

	int CHvString::Format(const char* szFormat, ...)
	{
		if ( NULL == szFormat )
		{
			return -1;
		}

		char szBuf[CHvString::FORMAT_BUFMAXLEN];

		va_list arglist;
		va_start(arglist, szFormat);
		int iRet = vsprintf(szBuf, szFormat, arglist);
		va_end(arglist);

		m_str = szBuf;

		return iRet;
	}

	int CHvString::Insert(int iIndex, const char* psz)
	{
		int iStrCurLen = GetLength();
		if ( 0 <= iIndex && iIndex <= iStrCurLen && psz != NULL )
		{
			m_str.insert(iIndex, psz);
			return this->GetLength();
		}
		else
		{
			return iStrCurLen;
		}
	}

	int CHvString::GetLength()
	{
		return (int)m_str.size();
	}

	int CHvString::Remove(char ch)
	{
		int iOldLen = (int)m_str.size();
		int iCurLen = iOldLen;

		for ( int i=0; i<iCurLen; ++i )
		{
			if ( ch == m_str[i] )
			{
				m_str.erase(i, 1);
				iCurLen = m_str.size();
				i = 0;
			}
		}

		int nRemoveCount = iOldLen - m_str.size();
		return nRemoveCount;
	}

	int CHvString::Find(const char* pszSub, int iStart/*=0*/)
	{
		int iStrCurLen = GetLength();
		if ( pszSub != NULL && iStart < iStrCurLen )
		{
			return (int)m_str.find(pszSub, iStart);
		}
		else
		{
			return -1;
		}
	}

	CHvString CHvString::MakeLower()
	{
		string strOldStr = m_str;

		for ( string::iterator it = m_str.begin(); it != m_str.end(); ++it )
		{
			if ( 'A' <= (*it) && (*it) <= 'Z' )
			{
				m_str.replace(it, it+1, 1, (*it)+('a'-'A'));
			}
		}

		CHvString cResultStr(m_str.c_str());
		m_str = strOldStr;

		return cResultStr;
	}

	CHvString CHvString::MakeUpper()
	{
		string strOldStr = m_str;

		for ( string::iterator it = m_str.begin(); it != m_str.end(); ++it )
		{
			if ( 'a' <= (*it) && (*it) <= 'z' )
			{
				m_str.replace(it, it+1, 1, (*it)-('a'-'A'));
			}
		}

		CHvString cResultStr(m_str.c_str());
		m_str = strOldStr;

		return cResultStr;
	}

	CHvString CHvString::Left(int nCount)
	{
		string strOldStr = m_str;

		if ( 0 < nCount && nCount <= (int)m_str.size() )
		{
			m_str = m_str.substr(0, nCount);
		}
		else
		{
			m_str = "";
		}

		CHvString cResultStr(m_str.c_str());
		m_str = strOldStr;

		return cResultStr;
	}

	CHvString CHvString::Mid(int iFirst, int nCount/*=0*/)
	{
		string strOldStr = m_str;

		if ( 0 <= iFirst && iFirst < (int)m_str.size() )
		{
			if ( 0 == nCount )
			{
				m_str = m_str.substr(iFirst);
			}
			else if ( nCount > 0 )
			{
				m_str = m_str.substr(iFirst, nCount);
			}
			else
			{
				m_str = "";
			}
		}
		else
		{
			m_str = "";
		}

		CHvString cResultStr(m_str.c_str());
		m_str = strOldStr;

		return cResultStr;
	}

	//-----------------------------------------
	//CHvString类运算符重载及类型转换函数的实现
	//-----------------------------------------

	CHvString::operator const char*()
	{
		return m_str.c_str();
	}

	CHvString& CHvString::operator =(const CHvString& str)
	{
		m_str = str.m_str;
		return (*this);
	}

	CHvString& CHvString::operator =(const char* pchar)
	{
		if ( NULL == pchar )
		{
			m_str = "";
		}
		else
		{
			m_str = pchar;
		}

		return (*this);
	}

	CHvString& CHvString::operator +=(const CHvString& str)
	{
		m_str += str.m_str;
		return (*this);
	}

	CHvString& CHvString::operator +=(const char* pchar)
	{
		if ( pchar != NULL )
		{
			m_str += pchar;
		}

		return (*this);
	}

	CHvString& CHvString::operator +=(const char ch)
	{
		m_str += ch;
		return (*this);
	}

	CHvString CHvString::operator +(const CHvString& str)
	{
		CHvString cResultStr;
		cResultStr.m_str = this->m_str + str.m_str;
		return cResultStr;
	}

	CHvString CHvString::operator +(const char* pchar)
	{
		CHvString cResultStr;

		if ( NULL == pchar )
		{
			cResultStr.m_str = this->m_str;
		}
		else
		{
			cResultStr.m_str = this->m_str + pchar;
		}

		return cResultStr;
	}

	char CHvString::operator [](int iIndex)
	{
		if ( 0 <= iIndex && iIndex < GetLength() )
		{
			return m_str[iIndex];
		}
		else
		{
			return 0;
		}
	}

	bool CHvString::operator !=(const char* pchar)
	{
		return (m_str != pchar) ? true : false;
	}

	bool CHvString::operator !=(const CHvString& str)
	{
		return (this->m_str != str.m_str) ? true : false;
	}

	bool CHvString::operator ==(char* pchar)
	{
		return (m_str == pchar) ? true : false;
	}

	bool CHvString::operator ==(const CHvString& str)
	{
		return (this->m_str == str.m_str) ? true : false;
	}

	bool CHvString::operator >=(const CHvString& str)
	{
		return (this->m_str >= str.m_str) ? true : false;
	}

	bool CHvString::operator <=(const CHvString& str)
	{
		return (this->m_str <= str.m_str) ? true : false;
	}
}
