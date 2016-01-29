///////////////////////////////////////////////////////////
//  templateclass TCSWQueue.h
//  Implementation of the Class template<class T>CSWQueue
//  Created on:      28-二月-2013 14:09:54
//  Original author: zhouy
///////////////////////////////////////////////////////////

#if !defined(EA_5E198F93_5E2D_4ab9_9F56_FBEF348A3EB1__INCLUDED_)
#define EA_5E198F93_5E2D_4ab9_9F56_FBEF348A3EB1__INCLUDED_

#include "SWObject.h"

/**
 * @brief 队列管理基类
 */
template<class T>
class CSWQueue : public CSWObject
{
CLASSINFO(CSWQueue,CSWObject)
public:

	/**
	 * @brief 析构函数
	 */
	virtual ~CSWQueue()
    {
    };
	/**
	 * @brief 构造函数
	 */
	CSWQueue()
    {
    };
	/**
	 * @brief 构造函数
	 * 
	 * @param [in] dwSize : 制定队列的长度
	 */
	CSWQueue(DWORD dwSize)
    {
    };
	/**
	 * @brief 出队列
	 * 
	 * @param [in] Element : 出队列的元素的引用
	 * @return
	 * -S_OK : 成功
	 * - E_FAIL : 失败，队列为空时
	 */
	HRESULT Dequeue(T& Element)
    {
        return E_FAIL;
    };
	/**
	 * @brief 入队列
	 * 
	 * @param [in] Element : 入队列的元素
	 * @return
	 * -S_OK : 成功
	 * - E_FAIL : 失败，队列满时
	 */
	HRESULT Enqueue(T Element)
    {
        return E_FAIL;
    };
	/**
	 * @brief 队列查找函数
	 * 
	 * @param [in] Element : 要查找的元素
	 * @return
	 * -TRUE:找到
	 * -FALSE :没有找到
	 */
	BOOL Find(T Element)
    {
        return FALSE;
    };
	/**
	 * @brief 获取队列中元素的个数
	 */
	HRESULT GetCount(DWORD * pdwCount)
    {
        return E_FAIL;
    };
	/**
	 * @brief 测试是否队列为空
	 */
	BOOL IsEmpty()
    {
        return TRUE;
    };

};
#endif // !defined(EA_5E198F93_5E2D_4ab9_9F56_FBEF348A3EB1__INCLUDED_)

