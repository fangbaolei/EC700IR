///////////////////////////////////////////////////////////
//  templateclass TCSWArray.h
//  Implementation of the Class template<class T>CSWArray
//  Created on:      28-二月-2013 14:09:49
//  Original author: zhouy
///////////////////////////////////////////////////////////

#if !defined(EA_9EE290E3_5969_4aa4_875D_09AA5C11640A__INCLUDED_)
#define EA_9EE290E3_5969_4aa4_875D_09AA5C11640A__INCLUDED_

#include "SWObject.h"

/**
 * @brief 数组模板类
 */
template<class T>
class CSWArray : public CSWObject
{
CLASSINFO(CSWArray,CSWObject)

public:
	/**
	 * @brief 清除数组中所有元素
	 * 
	 * @return
	 * -S_OK : 成功
	 * -E_FAIL : 失败
	 */
	HRESULT Clear()
    {
        return S_OK;
    };

	/**
	 * @brief 取得数组中元素个数
	 * 
	 * @return
	 * -元素个数
	 */
	DWORD Count()
    {
        return 0;
    };

	/**
	 * @brief 析构函数
	 */
	virtual ~CSWArray()
    {
        Clear();
    };

	/**
	 * @brief 构造函数
	 * 
	 * @param [in] dwCnt : 创建数组的大小
	 */
	CSWArray(DWORD dwCnt)
    {
    };

	/**
	 * @brief 判断数组是否为空
	 * 
	 * @return
	 * -TRUE : 数组为空
	 * -FALSE : 数组非空
	 */
	BOOL IsEmpty()
    {
        return TRUE;
    };

	/**
	 * @brief 取得数组中对应下标的元素引用
	 * 
	 * @return
	 * - 元素的引用
	 */
	T& operator[](const DWORD dwIndex)
    {
        return (*this);
    };
};

#endif // !defined(EA_9EE290E3_5969_4aa4_875D_09AA5C11640A__INCLUDED_)

