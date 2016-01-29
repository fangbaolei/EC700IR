/*
 *	(C)版权所有 2010 北京信路威科技发展有限公司
 */

/**
* @file		HvList.h
* @version	1.0
* @brief	CHvList模板类的定义以及实现
* @author	Shaorg
* @date		2010-7-26
*/

#ifndef _HVLIST_H_
#define _HVLIST_H_

#include <list>
using namespace std;

namespace HiVideo
{
	typedef int HVPOSITION;  /**< CHvList的位置类型*/

	/*! @class	CHvList
	* @brief	基于STL中的list模板类实现，类似于MFC中的CList类。
	*/
	template<class T>
	class CHvList
	{
	public:
		/**
		* @brief	默认构造函数
		*/
		CHvList()
		{
		};

		/**
		* @brief	析构函数
		*/
		virtual ~CHvList()
		{
		};

	public:
		/**
		* @brief	在头节点处插入一个新的元素
		* @param	item	插入的元素
		* @return	头节点的位置（即：第一个数据存在的位置）
		*/
		HVPOSITION AddHead(const T& item)
		{
			m_list.push_front(item);
			return 0;
		};

		/**
		* @brief	在尾节点处插入一个新的元素
		* @param	item	插入的元素
		* @return	尾节点的位置（即：最后一个数据存在的位置）
		*/
		HVPOSITION AddTail(const T& item)
		{
			m_list.push_back(item);
			return (int)m_list.size()-1;
		};

		/**
		* @brief	获取头节点的位置
		* @return	成功：0，失败：-1
		*/
		HVPOSITION GetHeadPosition()
		{
			return m_list.empty() ? -1 : 0;
		};

		/**
		* @brief	获取尾节点的位置
		* @return	成功：尾节点的位置，失败：返回-1
		*/
		HVPOSITION GetTailPosition()
		{
			return m_list.empty() ? -1 : ((int)m_list.size()-1);
		};

		/**
		* @brief	获取指定位置的元素的引用的同时将该位置后移一位
		* @param	rPosition	指定的位置
		* @return	返回位于rPosition的元素的引用，之后rPosition向后移一位,如果移到非法位置，则rPosition被置为-1。
		* @details	如果传入的rPosition小于0，则返回位于头节点元素的引用；
		*			如果传入的rPosition大于等于尾节点的位置，则返回位于尾节点元素的引用，
		*			同时，rPosition都将被置为-1。
		* @warning	在调用此函数之前，请确保list不为空。
		*/
		T& GetNext(HVPOSITION& rPosition)
		{
			HVPOSITION rOldPos = rPosition;

			//当rPosition合法且不为尾节点的位置
			rPosition = ( 0 <= rPosition && rPosition < ((int)m_list.size()-1) ) ? (rPosition+1) : -1;

			//当rOldPos处于合法位置，或者处于末尾时，将rOldPos位置的元素找到，并返回给应用层。
			if ( rPosition != -1 || ((int)m_list.size()-1) == rOldPos )
			{
				return GetAt(rOldPos);
			}
			else
			{
				//rPosition在非法位置时的处理
				if ( rOldPos < 0 )
				{
					rOldPos = 0;
				}
				else if ( rOldPos > ( (int)m_list.size()-1 ) )
				{
					rOldPos = ( (int)m_list.size()-1 );
				}
				return GetAt(rOldPos);
			}
		};

		/**
		* @brief	获取指定位置的元素的引用的同时将该位置前移一位
		* @param	rPosition	指定的位置
		* @return	返回位于rPosition的元素的引用，之后rPosition向后前一位,如果移到非法位置，则rPosition被置为-1。
		* @details	如果传入的rPosition为非法值，则处理同GetNext()
		* @see		GetNext()
		* @warning	在调用此函数之前，请确保list不为空。
		*/
		T& GetPrev(HVPOSITION& rPosition)
		{
			HVPOSITION rOldPos = rPosition;

			//如果rPosition合法且不为头节点的位置
			rPosition = ( 0 < rPosition && rPosition <= ((int)m_list.size()-1) ) ? (rPosition-1) : -1;

			if ( rPosition != -1 || 0 == rOldPos )
			{
				return GetAt(rOldPos);
			}
			else
			{
				//rPosition在非法位置时的处理
				if ( rOldPos < 0 )
				{
					rOldPos = 0;
				}
				else if ( rOldPos > ( (int)m_list.size()-1 ) )
				{
					rOldPos = ( (int)m_list.size()-1 );
				}
				return GetAt(rOldPos);
			}
		};

		/**
		* @brief	删除头节点
		* @return	被移除的头节点中的元素
		* @warning	在调用此函数之前，请确保list不为空。
		*/
		T RemoveHead()
		{
			class list<T>::iterator it_head = m_list.begin();
			T delItem = (*it_head);

			m_list.pop_front();
			return delItem;
		};

		/**
		* @brief	删除尾节点
		* @return	被移除的尾节点中的元素
		* @warning	在调用此函数之前，请确保list不为空。
		*/
		T RemoveTail()
		{
			class list<T>::reverse_iterator it_tail = m_list.rbegin();
			T delItem = (*it_tail);

			m_list.pop_back();
			return delItem;
		};

		/**
		* @brief	删除指定位置的节点
		* @param	rPosition	指定的位置
		* @return	void
		*/
		void RemoveAt(const HVPOSITION& rPosition)
		{
			if ( 0 <= rPosition && rPosition < (int)m_list.size() )
			{
				int nPos = 0;
				for ( class list<T>::iterator it = m_list.begin(); it != m_list.end(); ++it )
				{
					if ( nPos == rPosition )
					{
						m_list.erase(it);
						return ;
					}
					++nPos;
				}
			}
		};

		/**
		* @brief	删除所有节点
		* @return	void
		*/
		void RemoveAll()
		{
			m_list.clear();
		};

		/**
		* @brief	判断指定的位置是否为合法位置
		* @param	rPosition	指定的位置
		* @return	合法：true，非法：false
		*/
		bool IsValidPos(const HVPOSITION& rPosition)
		{
			return ( 0 <= rPosition && rPosition < (int)m_list.size() ) ? true : false;
		};

		/**
		* @brief	判断list是否为空
		* @return	为空：true，不为空：false
		*/
		bool IsEmpty()
		{
			return m_list.empty();
		};

		/**
		* @brief	判断list是否已满（即：已经不能再插入元素）
		* @return	总是为false。
		* @detail	该成员函数的目的是为了兼容旧版本
		*/
		bool IsFull()
		{
			//这个函数的实现是为了考虑兼容性，否则可以考虑去除。
			//因为是用STL的list类实现的，所以内存是动态在堆上分配的，基本无需考虑是否会满，除非内存用完了！
			return false;
		};

		/**
		* @brief	获取list的长度
		* @return	list的长度
		*/
		int GetSize()
		{
			return (int)m_list.size();
		};
       /**
		* @brief	获取list列表
		* @return	list列表
		*/
		const list<T>& GetList() const
		{
			return m_list;
		}
	private:
		//注：作为内部私有函数，Position的合法性以及m_list的非空性由调用函数保证。
		T& GetAt(const HVPOSITION& rPosition)
		{
		    int nPos;

			//这个if判断属于优化行为。即：位置在中间之前的正向查找，在中间之后的反向查找。
			if ( rPosition <= ((int)m_list.size()/2) )
			{
				nPos = 0;
				for ( class list<T>::iterator it = m_list.begin(); it != m_list.end(); ++it )
				{
					if ( nPos == rPosition )
					{
						return (*it);
					}
					++nPos;
				}
			}
			else
			{
			    nPos = 0;
				for ( class list<T>::reverse_iterator it_r = m_list.rbegin(); it_r != m_list.rend(); ++it_r )
				{
				    HVPOSITION rPosition_r = m_list.size()-1 - rPosition;
					if ( nPos == rPosition_r )
					{
						return (*it_r);
					}
					++nPos;
				}
			}
			return *m_list.end();
		};

	private:
		list<T> m_list;
	};
}

#endif
