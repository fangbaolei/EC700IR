#ifndef _HV_VAR_TYPE_H_
#define _HV_VAR_TYPE_H_

#include "swbasetype.h"
#include "hvthread.h"
#include "hvutils.h"

#if (RUN_PLATFORM == PLATFORM_WINDOWS)
const int MAX_STRING_SIZE = 255;
#else
const int MAX_STRING_SIZE = 255;
#endif

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

//CHvList 实现
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

//CHvList 实现
//typedef  int HVPOSITION;

template < class T1, int MAX_COUNT >
class CHvListHxl
{
	typedef struct _LOST_POINT
	{
		bool fUse;
		HVPOSITION nPrev;
		HVPOSITION nNext;
	} LOST_POINT;

public:
	CHvListHxl()
		: m_iHead(0)
		, m_iTail(0)
		, m_pItem(NULL)
		, m_pPoint(NULL)
		, m_iBufferSize(0)
		, m_nCount(0)
	{
		m_iBufferSize = MAX_COUNT;					// 缓冲区 0～(MAX_COUNT-1)，pos 0～(MAX_COUNT - 1)
		m_pItem = new T1[m_iBufferSize];
		m_pPoint = new LOST_POINT[m_iBufferSize];
		for (int i = 0; i < m_iBufferSize; i++)
		{
			m_pPoint[i].fUse = false;
			m_pPoint[i].nPrev = -1;
			m_pPoint[i].nNext = -1;
		}
	}

	~CHvListHxl()
	{
		if( m_pItem != NULL )
		{
			delete[] m_pItem;
			m_pItem = NULL;
		}
		if (m_pPoint != NULL)
		{
			delete[] m_pPoint;
		}
	};

public:
	bool IsEmpty()
	{
		return (m_nCount == 0);
	}

	bool IsFull()
	{
		return (m_nCount == m_iBufferSize);
	}

	void RemoveAll()
	{
		m_iHead = m_iTail = 0;
		m_nCount = 0;
		for (int i = 0; i < m_iBufferSize; i++)
		{
			m_pPoint[i].fUse = false;
			m_pPoint[i].nPrev = -1;
			m_pPoint[i].nNext = -1;
		}
	}

	int GetSize()
	{
		return (m_nCount);
	}

	HVPOSITION AddHead(const T1& item)
	{
		HVPOSITION pos = -1;
		int nFind;
		if((m_pItem != NULL) && !IsFull())
		{
			// 向前找到第一个空的位置
			nFind = ((m_iHead - 1) < 0) ? (m_iBufferSize - 1) : (m_iHead - 1);
			
			// 找到空闲的位置，如果找到m_iTail，则队列满了
			while(m_pPoint[nFind].fUse)
			{
				nFind--;
				if (nFind < 0)
				{
					nFind = m_iBufferSize - 1;
				}
			}

			// 找到了空闲的nFind
			m_nCount++;
			m_pItem[nFind] = item;
			m_pPoint[nFind].fUse = true;
			m_pPoint[nFind].nNext = m_iHead;
			m_pPoint[m_iHead].nPrev = nFind;
			
			m_iHead = nFind;
			pos = m_iHead;
		}
		return pos;
	}

	HVPOSITION AddTail(const T1& item)
	{
		HVPOSITION pos = -1;
		int nFind;
		if( (m_pItem != NULL) && !IsFull() )
		{
			// 非满，则m_iTail所在位置无数据
			m_nCount++;
			m_pItem[m_iTail] = item;
			m_pPoint[m_iTail].fUse = true;
			
			// 找到下一个非空的位置(如果满了，则下个位置为m_nHead)
			if (IsFull())
			{
				nFind = m_iHead;
			}
			else
			{
				// 向后找到第一个空的位置
				nFind = ((m_iTail + 1) >= m_iBufferSize) ? 0 : (m_iTail + 1);
			
				// 找到空闲的位置
				while(m_pPoint[nFind].fUse)
				{
					nFind++;
					if (nFind >= m_iBufferSize)
					{
						nFind = 0;
					}
				}
			}

			// nFind为找到的位置
			m_pPoint[m_iTail].nNext = nFind;
			m_pPoint[nFind].nPrev = m_iTail;

			pos = m_iTail;
			m_iTail = nFind;
		}
		return pos;
	}

	T1 RemoveHead()
	{
		T1 item;
		int nTemp;
		if( !IsEmpty() )
		{
			// 非空，头指针所在位置一定有数据
			m_nCount--;			// 计数减1
			item = m_pItem[m_iHead];

			nTemp = m_pPoint[m_iHead].nNext;
			
			m_pPoint[m_iHead].fUse = false;		// 释放当前首节点
			m_pPoint[m_iHead].nNext = -1;

			m_iHead = nTemp;
			m_pPoint[m_iHead].nPrev = -1;		// 建立当前首节点
		}
		return item;
	}

	T1 RemoveTail()
	{
		T1 item;
		int nTemp;
		if( !IsEmpty() )
		{
			nTemp = m_pPoint[m_iTail].nPrev;

			m_pPoint[m_iTail].nPrev = -1;		// 释放当前尾节点
			m_iTail = nTemp;			// 建立当前尾节点
			m_pPoint[m_iTail].nNext = -1;

			// 释放当前尾节点数据
			m_nCount--;			// 计数减1
			item = m_pItem[m_iTail];
			m_pPoint[m_iTail].fUse = false;  //*************
		}
		return item;
	}





	HVPOSITION GetHeadPosition()
	{
		HVPOSITION pos = -1;
		if( !IsEmpty() )
		{
			pos = m_iHead;
		}
		return pos;
	}

	HVPOSITION GetTailPosition()
	{
		HVPOSITION pos = -1;
		if( !IsEmpty() )
		{
			pos = m_pPoint[m_iTail].nPrev;
		}
		return pos;
	}

	T1& GetNext(HVPOSITION& rPosition)
	{
		HVPOSITION nItem = rPosition;
		HVPOSITION pos = -1;
		if( !IsEmpty() && (0 <= nItem) && (nItem < m_iBufferSize))
		{
			pos = m_pPoint[nItem].nNext;
			if (pos == m_iTail)
			{
				pos = -1;
			}
		}

		rPosition = pos;
		return m_pItem[nItem];
	}

	T1& GetPrev( HVPOSITION& rPosition )
	{
		HVPOSITION nItem = rPosition;
		HVPOSITION pos = -1;
		if( !IsEmpty() && (0 <= nItem) && (nItem < m_iBufferSize))
		{
			if (pos == m_iHead)
			{
				pos = -1;
			}
			else
			{
				pos = m_pPoint[nItem ].nPrev;
			}
		}

		rPosition = pos;
		return m_pItem[nItem];
	}

	/*************************************/
	//T1& GetAt(HVPOSITION& position)
	//{

	//}

	void RemoveAt(HVPOSITION& rPosition)
	{
		int iItem = rPosition;
		if ((!IsEmpty()) && (0 <= iItem) && (iItem < m_iBufferSize))
		{
			if (m_pPoint[iItem].fUse)
			{
				// 使用中的才能删除
				if (iItem == m_iHead)
				{
					// 首指针, 使用RemoveHead()
					RemoveHead();
				}
				else if (iItem == m_pPoint[m_iTail].nPrev)
				{
					// 最后一个数据，采用RemoveTail()
					RemoveTail();
				}
				else
				{
					// 中间节点
					HVPOSITION nPrev = m_pPoint[iItem].nPrev;
					HVPOSITION nNext = m_pPoint[iItem].nNext;						
					m_pPoint[nPrev].nNext = nNext;
					m_pPoint[nNext].nPrev = nPrev;

					if (m_iHead == m_iTail)
					{
						// 原来是满的
						nPrev = m_pPoint[m_iTail].nPrev;
						m_pPoint[nPrev].nNext = iItem;
						m_pPoint[iItem].nPrev = nPrev;

						m_iTail = iItem;
						m_pPoint[m_iHead].nPrev = -1;
						m_pPoint[m_iTail].nNext = -1;

					}
					else
					{
						// 原来非满
						m_pPoint[iItem].nPrev = -1;
						m_pPoint[iItem].nNext = -1;
					}
					m_pPoint[iItem].fUse = false;
					m_nCount--;
				}
			}
		}
	}

	bool IsValidPos(HVPOSITION& rPosition)
	{

		return ((rPosition >= 0) && m_pPoint[rPosition].fUse);
	}
private:
	T1* m_pItem;
	LOST_POINT *m_pPoint;
	int m_iHead;
	int m_iTail;
	int m_iBufferSize;
	int m_nCount;
};

template < DWORD32 BLOCK_COUNT, DWORD32 BLOCK_SIZE >
class CHvMemBlock
{
public:
	CHvMemBlock()
	{
		m_psemMem = NULL;
		m_iMemIndex = 0;
		HiVideo::ISemaphore::CreateInstance(&m_psemMem, 1, 1);
		HV_memset(m_rgMemFrame, 0, sizeof(m_rgMemFrame));
	}
	~CHvMemBlock()
	{
		for (int i = 0; i < BLOCK_COUNT; i++)
		{
			if (m_rgMemFrame[i] != NULL)
			{
				HV_FreeMem(m_rgMemFrame[i], BLOCK_SIZE);
			}
		}
		if (m_psemMem)
		{
			delete m_psemMem;
		}
	}
	void* AllocMem()
	{
		void* pvDest = NULL;
		m_psemMem->Pend();
		if (m_iMemIndex < 0)
		{
			pvDest = HV_AllocMem(BLOCK_SIZE);
		}
		else if (m_rgMemFrame[m_iMemIndex] != NULL)
		{
			pvDest = m_rgMemFrame[m_iMemIndex];
			m_iMemIndex--;
		}
		else
		{
			pvDest = HV_AllocMem(BLOCK_SIZE);
		}
		m_psemMem->Post();
		return pvDest;
	}
	HRESULT FreeMem(void* pvBuffer)
	{
		if (NULL == pvBuffer) return S_FALSE;
		m_psemMem->Pend();
		if (m_iMemIndex >= (int)BLOCK_COUNT - 1)
		{
			HV_FreeMem(pvBuffer, BLOCK_SIZE);
		}
		else
		{
			m_rgMemFrame[++m_iMemIndex] = pvBuffer;
		}
		m_psemMem->Post();
		return S_OK;
	}
private:
	void* m_rgMemFrame[BLOCK_COUNT];
	HiVideo::ISemaphore* m_psemMem;
	int m_iMemIndex;
};
#endif
