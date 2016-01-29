#ifndef _HV_VAR_TYPE_H_
#define _HV_VAR_TYPE_H_

#include "hvthreadex.h"

const int MAX_STRING_SIZE = 255;

class CHvString
{
private:
	char m_szBuffer[MAX_STRING_SIZE + 1];
	int m_iStrLen;
public:
	CHvString();
	CHvString(const char* szString);
	~CHvString();
public:
	int Format(const char* format, ...);
	char* GetBuffer();
	void ReleaseBuffer();
	operator const char*();
	CHvString &operator =(const CHvString &str);
	CHvString &operator =(const char* pchar);
	CHvString &operator +=(const CHvString &str);
	CHvString &operator +=(const char* pchar);
	CHvString &operator +=(const char ch);
	CHvString operator +(const CHvString &str);
	CHvString operator +(const char* pchar);
	char operator [](int iIndex);
	bool operator !=(const char* pchar);
	bool operator !=(const CHvString &str);
	bool operator ==(char* pchar);
	bool operator ==(const CHvString &str);
	bool operator >=(const CHvString &str);
	bool operator <=(const CHvString &str);
	int Insert(int iIndex, const char* psz);
	int GetLength();
	bool IsEmpty();
	void Append(const CHvString &str);
	CHvString MakeLower();
	CHvString MakeUpper();
	CHvString Left(int nCount);
	CHvString Mid(int iFirst);
	CHvString Mid(int iFirst, int nCount);
	int Remove(char ch);
	int Find(const char* pszSub, int iStart = 0);
};

//CHvList й╣ож
typedef  int HVPOSITION;

template < class T, int MAX_COUNT >
class CHvList
{
public:
	CHvList()
		: m_iHead(0)
		, m_iTail(0)
		, m_pItem(NULL)
		, m_iBufferSize(0)
		, m_pSem(NULL)
	{
		m_iBufferSize += MAX_COUNT + 1;
		m_pItem = new T[m_iBufferSize];
		HiVideo::ISemaphore::CreateInstance(&m_pSem, 1, 1);
	}
	~CHvList()
	{
		if( m_pItem != NULL )
		{
			delete[] m_pItem;
			m_pItem = NULL;
		}
		if (m_pSem)
		{
			delete m_pSem;
			m_pSem = NULL;
		}
	};

public:
	HVPOSITION AddHead(const T& item)
	{
		HVPOSITION pos = 0;
		m_pSem->Pend();
		if( (m_pItem != NULL) && !IsFull() )
		{
			m_iHead = (m_iHead + m_iBufferSize- 1) % m_iBufferSize;
			pos = m_iHead + 1;
			m_pItem[m_iHead] = item;
		}
		m_pSem->Post();
		return pos;
	}

	HVPOSITION AddTail(const T& item)
	{
		HVPOSITION pos = 0;
		m_pSem->Pend();
		if( (m_pItem != NULL) && !IsFull() )
		{
			pos = m_iTail + 1;
			m_pItem[m_iTail] = item;
			m_iTail = (m_iTail + 1) % m_iBufferSize;
		}
		m_pSem->Post();
		return pos;
	}

	bool IsEmpty()
	{
		return (m_iTail == m_iHead);
	}

	bool IsFull()
	{
		return (((m_iTail + 1) % m_iBufferSize) == m_iHead);
	}

	T RemoveHead()
	{
		T item;
		m_pSem->Pend();
		if( !IsEmpty() )
		{
			item = m_pItem[m_iHead];
			m_iHead = (m_iHead + 1) % m_iBufferSize;
		}
		m_pSem->Post();
		return item;
	}

	T RemoveTail()
	{
		T item;
		m_pSem->Pend();
		if( !IsEmpty() )
		{
			m_iTail = (m_iTail + m_iBufferSize - 1) % m_iBufferSize;
			item = m_pItem[ m_iTail];
		}
		m_pSem->Post();
		return item;
	}

	void RemoveAll()
	{
		m_pSem->Pend();
		m_iHead = m_iTail = 0;
		m_pSem->Post();
	}

	HVPOSITION GetHeadPosition()
	{
		HVPOSITION pos = 0;
		if( !IsEmpty() )
		{
			pos = m_iHead + 1;
		}
		return pos;
	}

	HVPOSITION GetTailPosition()
	{
		HVPOSITION pos = 0;
		if( !IsEmpty() )
		{
			pos = ((m_iTail + m_iBufferSize - 1) % m_iBufferSize) + 1;
		}
		return pos;
	}

	T& GetNext( HVPOSITION& rPosition )
	{
		int iItem = rPosition - 1;
		HVPOSITION pos = 0;
		if( !IsEmpty() && 0<= iItem && iItem < m_iBufferSize )
		{
			if( m_iHead < m_iTail 
				&& (iItem >= m_iHead && iItem < m_iTail) )
			{
				if( ((iItem + 1) % m_iBufferSize) != m_iTail )
				{
					pos = ((iItem + 1) % m_iBufferSize) + 1;
				}
			}
			if( m_iHead > m_iTail 
				&& !((iItem >= m_iTail) && (iItem < m_iHead)) )
			{
				if( ((iItem + 1) % m_iBufferSize) != m_iTail )
				{
					pos = ((iItem + 1) % m_iBufferSize) + 1;
				}
			}
		}
		else
		{
			iItem = 0;
		}

		rPosition = pos;
		return m_pItem[iItem];
	}

	T& GetPrev( HVPOSITION& rPosition )
	{
		int iItem = rPosition - 1;
		HVPOSITION pos = 0;
		if( !IsEmpty() && iItem >= 0 && iItem < m_iBufferSize )
		{
			if( m_iHead < m_iTail 
				&& (iItem >= m_iHead && iItem < m_iTail) )
			{
				if( iItem != m_iHead )
				{
					pos = ((iItem + m_iBufferSize - 1) % m_iBufferSize) + 1;
				}
			}
			if( m_iHead > m_iTail 
				&& !((iItem >= m_iTail) && (iItem < m_iHead)) )
			{
				if( iItem != m_iHead )
				{
					pos = ((iItem + m_iBufferSize - 1) % m_iBufferSize) + 1;
				}
			}
		}
		else
		{
			iItem = 0;
		}

		rPosition = pos;
		return m_pItem[iItem];
	}

	int GetSize()
	{
		return ((m_iTail + m_iBufferSize - m_iHead) % m_iBufferSize);
	}

private:
	T* m_pItem;
	int m_iHead;
	int m_iTail;
	int m_iBufferSize;
	HiVideo::ISemaphore* m_pSem;
};
#endif
