#ifndef _HV_ENUM_H
#define _HV_ENUM_H

#include "hvutils.h"

#include "swobjbase.h"

//枚举器基类
template<class T>
class CHvEnumBase
{
public:
	//节点定义
	typedef struct _NODE
	{
		_NODE* pPre;
		_NODE* pNext;

		T Dat;

		_NODE()
			:pPre(NULL)
			,pNext(NULL)
		{
		}
	} NODE;

	//取数据
	//返回值  S_OK:  获取指定个数节点成功
	//					S_FALSE:  其他
	//备注: 必须先定位m_pCur,如通过Reset Skip FindNode
	STDMETHOD(EnumBase_Next)(
		ULONG cCount,	//请求个数
		T* rgDat,	//目的数组
		ULONG *pcFetched	//实际取得个数
		)
	{
		if (pcFetched != NULL) *pcFetched = 0;

		if ( rgDat == NULL ) return S_FALSE;

		ULONG cCopied(0);
		//当前节点指针不为NULL且拷贝数小于需要数
		while(m_pCur != NULL && cCopied < cCount)
		{
			*rgDat = m_pCur->Dat;
			m_pCur = m_pCur->pNext;

			rgDat++;
			cCopied++;
		}

		if ( pcFetched != NULL ) *pcFetched = cCopied;

		return (cCount == cCopied)?S_OK:S_FALSE;
	}

	//跳过指定个数节点
	//返回值  S_OK: 跳过指定个数节点成功
	//				   S_FALSE: 其他

	STDMETHOD(EnumBase_Skip)(
		ULONG cCount
		)
	{
		ULONG cSkipped(0);
		while(m_pCur != NULL && cSkipped < cCount)
		{
			m_pCur = m_pCur->pNext;
			cSkipped++;
		}
		return (cCount == cSkipped)?S_OK:S_FALSE;
	}

	//重置链表指针
	//返回值 S_OK
	STDMETHOD(EnumBase_Reset)(void)
	{
		m_pCur = m_pHead;
		return S_OK;
	}

public:
	//添加
	//返回值 S_OK: 添加成功
	//					E_OUTOFMEMORY: 生成新节点失败
	STDMETHOD(EnumBase_Add)(const T& dat)
	{
		NODE* pNewNode = new NODE;
		if (pNewNode == NULL) return E_OUTOFMEMORY;

		pNewNode->Dat = dat;

		NODE* pTail(m_pHead);

		if (pTail == NULL)
		{
			m_pHead = pNewNode;
			m_pCur = m_pHead;
		}
		else
		{
			//找链尾
			while(pTail->pNext != NULL)
			{
				pTail = pTail->pNext;
			}
			pNewNode->pPre = pTail;
			pTail->pNext=pNewNode;
		}

		return S_OK;
	}

	//寻找指定节点
	//返回值 S_OK: 找到并定位成功,当前节点指针指向匹配的节点
	//					S_FALSE: 找不到,不改变当前节点指针
	STDMETHOD(EnumBase_FindNode)(const T& dat)
	{
		NODE* pNode(m_pHead);
		NODE TempNode;
		BOOL fFound = FALSE;

		while( pNode != NULL )
		{
			if (IsMatch(dat,pNode->Dat))
			{
				m_pCur = pNode;
				fFound = TRUE;
				break;
			}
			pNode = pNode->pNext;
		}

		return fFound?S_OK:S_FALSE;
	}

	//删除当前节点
	//返回值	S_OK:	 删除成功,当前节点指针指向下一个节点(链尾则为空)
	//					S_FALSE: 当前节点指针为空(已到链尾)
	STDMETHOD(EnumBase_Remove)(void)
	{
		if (m_pCur == NULL) return S_FALSE;

		NODE* pPre = m_pCur->pPre;
		NODE* pNext = m_pCur->pNext;

		OnDelNode(m_pCur);

		delete m_pCur;
		m_pCur = pNext;

		if (pPre == NULL && pNext == NULL)		//唯一元素
		{
			m_pHead = NULL;
		}
		else if (pPre != NULL && pNext == NULL)	//删除尾
		{
			pPre->pNext = NULL;
		}
		else if ( pPre == NULL && pNext != NULL)		//删除头
		{
			m_pHead = pNext;
			pNext->pPre = NULL;
		}
		else	//删除中间
		{
			pPre->pNext = pNext;
			pNext->pPre = pPre;
		}

		return S_OK;
	}

	//删除指定节点
	//返回值 S_OK: 找到并删除节点成功
	//					S_FALSE: 找不到并未做删除
	STDMETHOD(EnumBase_Remove)(const T& dat)
	{
		HRESULT hr = EnumBase_FindNode(dat);	//定位节点
		if (S_OK == hr)
		{
			hr = EnumBase_Remove();	//定位成功则删除当前节点
		}
		return hr;
	}
public:
	CHvEnumBase()
		:m_pHead(NULL)
		,m_pCur(NULL)
	{
	}

	virtual ~CHvEnumBase()
	{
		NODE *pDel,*pNode(m_pHead);
		while(pNode != NULL)
		{
			pDel = pNode;	//保留要删除的节点指针
			pNode = pNode->pNext;	//指到下一个
			delete pDel;	//删除
		}
	}

	BOOL GetHead( T* pRet)
	{
		if (!pRet) return FALSE;

		m_pCur = m_pHead;

		if (m_pCur == NULL) return FALSE;

		*pRet = m_pCur->Dat;

		return TRUE;
	}

	BOOL GetTail( T* pRet)
	{
		if (!pRet) return FALSE;

		EnumBase_Reset();

		if (m_pCur == NULL) return FALSE;

		while ( m_pCur->pNext != NULL)
		{
			m_pCur = m_pCur->pNext;
		}

		*pRet = m_pCur->Dat;

		return TRUE;
	}

	BOOL IsEmpty()
	{
		return (m_pHead == NULL);
	}

protected:
	//判断匹配情况,派生类可自定义
	virtual BOOL IsMatch(const T& dat1, const T& dat2)
	{
		return (HV_memcmp(&dat1,&dat2,sizeof(T) ) == 0);
	}
	virtual HRESULT OnDelNode(NODE* pNode)
	{
		return S_OK;
	}
	NODE* m_pHead;
	NODE* m_pCur;
};

template<class T>
class CSimpStack
{
public:
	BOOL Push(const T& dat)
	{
		return S_OK == m_StackStore.EnumBase_Add(dat);
	}
	BOOL Pop(T* pRet)
	{
		BOOL fRet = FALSE;
		if ( TRUE == m_StackStore.GetTail(pRet) )
		{
			m_StackStore.EnumBase_Remove(); //此时pCur指向最后一个NODE
			fRet = TRUE;
		}
		return fRet;
	}
	BOOL GetTop(T* pRet)
	{
		if ( !m_StackStore.IsEmpty() )
		{
			return m_StackStore.GetTail(pRet);
		}
		else
		{
			return FALSE;
		}
	}
	BOOL IsEmpty()
	{
		return m_StackStore.IsEmpty();
	}

protected:
	CHvEnumBase<T> m_StackStore;
};

#endif// _HV_ENUM_H
