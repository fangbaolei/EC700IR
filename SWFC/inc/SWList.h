///////////////////////////////////////////////////////////
//  CSWList.h
//  Implementation of the Class CSWList
//  Created on:      28-二月-2013 14:09:50
//  Original author: shaorg
///////////////////////////////////////////////////////////

#if !defined(EA_0A932506_CD6D_42e5_8479_E519E415C54D__INCLUDED_)
#define EA_0A932506_CD6D_42e5_8479_E519E415C54D__INCLUDED_

#include "SWObject.h"

#ifdef WIN32
#pragma warning(disable:4172) // 禁止“返回无效的引用”警告
#endif

#define SW_POSITION PVOID

/**
 * @brief 链表模板类,参考MFC相关接口
 */
template<class T, INT MAX_COUNT = 256>
class CSWList : public CSWObject
{
CLASSINFO(CSWList,CSWObject)

private:
    struct CSWListNode
    {
        CSWListNode* m_pNext;
		CSWListNode* m_pPrev;
		T m_element;
    };

    CSWListNode* m_pHead;
	CSWListNode* m_pTail;
	INT m_iElements;
    INT m_iMaxCount;
	T m_elemReturn;

public:
    /**
	 * @brief 构造函数
	 */
	CSWList()
    {
        m_pHead = NULL;
        m_pTail = NULL;
        m_iElements = 0;
        m_iMaxCount = MAX_COUNT;
    };

	/**
	 * @brief 析构函数
	 */
	virtual ~CSWList()
    {
        RemoveAll();
    };

    /**
	 * @brief 将元素插入链表头部
     * 
	 * @return 头节点的位置
	 */
    SW_POSITION AddHead(const T& newElement)
    {
        SW_POSITION pos = NULL;

        if ( !IsFull() )
        {
            CSWListNode* pcNode = new CSWListNode;
            pos = (SW_POSITION)pcNode;

            if (IsEmpty()) // 链表为空时
            {
                pcNode->m_element = newElement;
                pcNode->m_pNext = NULL;
                pcNode->m_pPrev = NULL;

                m_pTail = m_pHead = pcNode;
                m_iElements = 1;
            }
            else
            {
                // 将新节点指向旧的头节点。
                pcNode->m_element = newElement;
                pcNode->m_pNext = m_pHead;
                pcNode->m_pPrev = NULL;

                // 处理旧头节点
                m_pHead->m_pPrev = pcNode;

                // 更新相关成员变量
                m_pHead = pcNode;
                m_iElements++;
            }
        }

        return pos;
    };

    /**
	 * @brief 将元素插入链表尾部
     * 
	 * @return 尾节点的位置
	 */
	SW_POSITION AddTail(const T& newElement)
    {
        SW_POSITION pos = NULL;

        if ( !IsFull() )
        {
            if (IsEmpty()) // 链表为空时
            {
                pos = AddHead(newElement);
            }
            else
            {
                // 创建新节点，并指向旧的尾节点。
                CSWListNode* pcNode = new CSWListNode;
                pos = (SW_POSITION)pcNode;
                pcNode->m_element = newElement;
                pcNode->m_pNext = NULL;
                pcNode->m_pPrev = m_pTail;

                // 处理旧尾节点
                m_pTail->m_pNext = pcNode;

                // 更新相关成员变量
                m_pTail = pcNode;
                m_iElements++;
            }
        }

        return pos;
    };

	/**
	 * @brief 获取给定位置的元素的引用
	 * 
	 * @param [in] Pos : 链表中的位置
	 * @return
	 * @return 给定位置的元素的引用
	 * @note 如果位置无效，则将返回无效的引用。
	 */
	T& GetAt(SW_POSITION Pos)
    {
        if ( IsValid(Pos) && !IsEmpty() ) // 位置合法且链表非空
        {
            CSWListNode* pcNodeCur = (CSWListNode*)Pos;
            return pcNodeCur->m_element;
        }

		return m_elemReturn;
    };

    /**
	 * @brief 设定给定位置的元素的值
	 * 
	 * @param [in] Pos : 链表中的位置
	 * @param [in] newElement : 新设定的值
	 * @return
	 * -S_OK : 成功
	 * -E_FAIL : 失败
	 */
	HRESULT SetAt(SW_POSITION Pos, const T& newElement)
    {
        if ( IsValid(Pos) && !IsEmpty() ) // 位置合法且链表非空
        {
            CSWListNode* pcNodeCur = (CSWListNode*)Pos;
            pcNodeCur->m_element = newElement;
            return S_OK;
        }

        return E_FAIL;
    };

    /**
	 * @brief 查找函数
	 * 
	 * @param [in] searchValue : 要查找的数据
	 * @param [in] startAfter : 查找起始点，默认为NULL(此处表示头节点位置)。
	 * @return
	 * - 有效的位置 : 找到，返回数据所在的位置
	 * - 无效的位置 : 没有找到
	 */
	SW_POSITION Find(const T& searchValue, SW_POSITION startAfter = NULL)
    {
        if ( !IsEmpty() ) // 链表非空
        {
            CSWListNode* pcNodeCur = m_pHead;
            if ( IsValid(startAfter) )
            {
                pcNodeCur = (CSWListNode*)startAfter;
            }

            while ( pcNodeCur != NULL )
            {
                if ( pcNodeCur->m_element == searchValue )
                {
                    return (SW_POSITION)pcNodeCur;
                }

                pcNodeCur = pcNodeCur->m_pNext; // 节点向后移动
            }
        }

        return NULL;
    };

	/**
	 * @brief 获取链表中元素个数
	 * 
	 * @return
	 * - 元素个数
	 */
	DWORD GetCount()
    {
        return m_iElements;
    };

    /**
	 * @brief 判断链表是否为空
     * 
	 * @return
	 * - TRUE : 为空
	 * - FALSE : 非空
	 */
    BOOL IsEmpty()
    {
        return (m_iElements <= 0) ? (TRUE) : (FALSE);
    };

    /**
	 * @brief 判断位置是否有效
     * 
	 * @return
	 * - TRUE : 有效
	 * - FALSE : 无效
	 */
    BOOL IsValid(SW_POSITION pos)
    {
        if ( pos != NULL )
        {
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    };

    /**
	 * @brief 设置链表最大节点数
     * 
	 * @return
	 * - S_OK : 成功
	 * - E_FAIL : 失败
	 */
    HRESULT SetMaxCount(DWORD dwCount)
    {
        if ( dwCount > 0 )
        {
            m_iMaxCount = dwCount;
            return S_OK;
        }
        return E_FAIL;
    };

    /**
	 * @brief 获取链表最大节点数
     * 
	 * @return
	 * - 链表最大节点数
	 */
    DWORD GetMaxCount(void)
    {
        return m_iMaxCount;
    };

    /**
	 * @brief 判断链表中的当前节点数是否已达最大值（即：是否已满）。
     * 
	 * @return
	 * - TRUE : 已满
	 * - FALSE : 未满
	 */
    BOOL IsFull()
    {
        return (m_iElements >= m_iMaxCount) ? (TRUE) : (FALSE);
    };

	/**
	 * @brief 获取头节点元素的引用
     * 
     * @return 头节点元素的引用
	 * @note 如果链表为空，则将返回无效的引用。
	 */
	T& GetHead()
    {
        if ( m_pHead != NULL )
        {
            return m_pHead->m_element;
        }

        return m_elemReturn;
    };

    /**
	 * @brief 获取尾节点元素的引用
     * 
	 * @return 尾节点元素的引用
     * @note 如果链表为空，则将返回无效的引用。
	 */
	T& GetTail()
    {
        if ( m_pTail != NULL )
        {
            return m_pTail->m_element;
        }

        return m_elemReturn;
    };

    /**
	 * @brief 获取头节点的位置
     * 
	 * @return 头节点的位置
	 */
	SW_POSITION GetHeadPosition()
    {
        return (SW_POSITION)m_pHead;
    };

    /**
	 * @brief 获取尾节点的位置
     *
     * @return 尾节点的位置
	 */
    SW_POSITION GetTailPosition()
    {
        return (SW_POSITION)m_pTail;
    };

    /**
	 * @brief 获取当前位置节点，之后该位置自动移动至下一节点。
     * 
	 * @return 获取到的元素引用
     * @note 如果传入无效的位置，则将返回无效的引用。
	 */
	T& GetNext(SW_POSITION& rPosition)
    {
        if ( IsValid(rPosition) && !IsEmpty() ) // 位置合法且链表非空
        {
            CSWListNode* pcNodeCur = (CSWListNode*)rPosition;
            rPosition = (SW_POSITION)(pcNodeCur->m_pNext);
            return pcNodeCur->m_element;
        }

        return m_elemReturn;
    };

    /**
	 * @brief 获取当前位置节点，之后该位置自动移动至上一节点。
     * 
	 * @return 获取到的元素引用
     * @note 如果传入无效的位置，则将返回无效的引用。
	 */
	T& GetPrev(SW_POSITION& rPosition)
    {
        if ( IsValid(rPosition) && !IsEmpty() ) // 位置合法且链表非空
        {
            CSWListNode* pcNodeCur = (CSWListNode*)rPosition;
            rPosition = (SW_POSITION)(pcNodeCur->m_pPrev);
            return pcNodeCur->m_element;
        }

        return m_elemReturn;
    };

    /**
	 * @brief 向当前位置节点之后插入新元素。
     * 
	 * @return 新元素的位置
	 */
	SW_POSITION InsertAfter(SW_POSITION rPosition, const T& Var)
    {
        SW_POSITION pos = NULL;

        if ( !IsFull() )
        {
            if ( IsValid(rPosition) && !IsEmpty() ) // 位置合法且链表非空
            {
                if ( GetTailPosition() == rPosition ) // 尾节点
                {
                    return AddTail(Var);
                }
                else
                {
                    CSWListNode* pcNodeCur = (CSWListNode*)rPosition;

                    // 创建新节点，并链入链表中。
                    CSWListNode* pcNode = new CSWListNode;
                    pos = (SW_POSITION)pcNode;
                    pcNode->m_element = Var;
                    pcNode->m_pNext = pcNodeCur->m_pNext;
                    pcNode->m_pPrev = pcNodeCur;

                    pcNodeCur->m_pNext->m_pPrev = pcNode;
                    pcNodeCur->m_pNext = pcNode;

                    m_iElements++;
                }
            }
        }

        return pos;
    };

    /**
	 * @brief 向当前位置节点之前插入新元素。
     * 
	 * @return 新元素的位置
	 */
	SW_POSITION InsertBefore(SW_POSITION rPosition, const T& Var)
    {
        SW_POSITION pos = NULL;

        if ( !IsFull() )
        {
            if ( IsValid(rPosition) && !IsEmpty() ) // 位置合法且链表非空
            {
                if ( GetHeadPosition() == rPosition ) // 头节点
                {
                    return AddHead(Var);
                }
                else
                {
                    CSWListNode* pcNodeCur = (CSWListNode*)rPosition;

                    // 创建新节点，并链入链表中。
                    CSWListNode* pcNode = new CSWListNode;
                    pos = (SW_POSITION)pcNode;
                    pcNode->m_element = Var;
                    pcNode->m_pNext = pcNodeCur;
                    pcNode->m_pPrev = pcNodeCur->m_pPrev;

                    pcNodeCur->m_pPrev->m_pNext = pcNode;
                    pcNodeCur->m_pPrev = pcNode;

                    m_iElements++;
                }
            }
        }

        return pos;
    };

	/**
	 * @brief 移除链表中所有元素
	 * 
	 * @return
	 * - S_OK : 成功
	 * - E_FAIL : 失败
	 */
	HRESULT RemoveAll()
    {
        if ( !IsEmpty() ) // 链表非空
        {
            CSWListNode* pcNodeCur = m_pHead;
            CSWListNode* pcNodeDel = NULL;
            for ( INT n = 0; n < m_iElements; ++n )
            {
                pcNodeDel = pcNodeCur;
                pcNodeCur = pcNodeCur->m_pNext;
                delete pcNodeDel;
                pcNodeDel = NULL;
            }

            m_pHead = NULL;
            m_pTail = NULL;
            m_iElements = 0;
            return S_OK;
        }

        return E_FAIL;
    };

    /**
	 * @brief 移除指定位置的节点
     * 
	 * @return
	 * - S_OK : 成功
	 * - E_FAIL : 失败
	 */
	T RemoveAt(SW_POSITION Pos)
    {
        T t;

        if ( IsValid(Pos) && !IsEmpty() ) // 位置合法且链表非空
        {
            t = ((CSWListNode*)Pos)->m_element;

            if ( GetHeadPosition() == Pos ) // 头节点
            {
                RemoveHead();
            }
            else if ( GetTailPosition() == Pos ) // 尾节点
            {
                RemoveTail();
            }
            else // 中间节点
            {
                // 移动链表到指定位置
                CSWListNode* pcNodeCur = (CSWListNode*)Pos;

                // 将该当前位置节点移除链表
                pcNodeCur->m_pPrev->m_pNext = pcNodeCur->m_pNext;
                pcNodeCur->m_pNext->m_pPrev = pcNodeCur->m_pPrev;

                delete pcNodeCur;
                pcNodeCur = NULL;
                m_iElements--;
            }
        }

        return t;
    };

    /**
	 * @brief 移除头节点
     * 
	 * @return
	 * - S_OK : 成功
	 * - E_FAIL : 失败
	 */
	T RemoveHead()
    {
        T t;

        if ( !IsEmpty() ) // 链表非空
        {
            t = m_pHead->m_element;

            if ( 1 == m_iElements ) // 链表仅有一个元素
            {
                RemoveAll();
            }
            else
            {
                // 移除头节点
                CSWListNode* pcNodeDel = m_pHead;
                m_pHead = m_pHead->m_pNext;
                m_pHead->m_pPrev = NULL;
                delete pcNodeDel;
                pcNodeDel = NULL;

                m_iElements--;
            }
        }

        return t;
    };

    /**
	 * @brief 移除尾节点
     * 
	 * @return
	 * - S_OK : 成功
	 * - E_FAIL : 失败
	 */
	T RemoveTail()
    {
        T t;

        if ( !IsEmpty() ) // 链表非空
        {
            t = m_pTail->m_element;

            if ( 1 == m_iElements ) // 链表仅有一个元素
            {
                RemoveAll();
            }
            else
            {
                // 移除尾节点
                CSWListNode* pcNodeDel = m_pTail;
                m_pTail = m_pTail->m_pPrev;
                m_pTail->m_pNext = NULL;
                delete pcNodeDel;
                pcNodeDel = NULL;

                m_iElements--;
            }
        }

        return t;
    };
};

#endif // !defined(EA_0A932506_CD6D_42e5_8479_E519E415C54D__INCLUDED_)

