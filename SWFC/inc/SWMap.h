///////////////////////////////////////////////////////////
//  templateclass KEY,class VALUECSWMap.h
//  Implementation of the Class template<class KEY,class VALUE>CSWMap
//  Created on:      28-二月-2013 14:09:49
//  Original author: zhouy
///////////////////////////////////////////////////////////

#if !defined(EA_765F26B4_059F_4c89_A13D_66725C18520B__INCLUDED_)
#define EA_765F26B4_059F_4c89_A13D_66725C18520B__INCLUDED_

#include "SWObject.h"

/**
 * @brief 映射模板类
 */
template<class KEY,class VALUE>
class CSWMap : public CSWObject
{
CLASSINFO(CSWMap,CSWObject)
public:

	HRESULT Clear()
    {
        return TRUE;
    };
	DWORD Count()
    {
        return 0;
    };

	virtual ~CSWMap()
    {
    };
	CSWMap()
    {
    };
	
	KEY GetKey(VALUE val)
    {
        KEY t;
        return t;
    };
	VALUE GetValue(KEY key)
    {
        VALUE t;
        return t;
    };

    //todo:这些函数定义不完整，为编译通过，先注释掉。
	/**
	 * 插入元素val到位置pos，或者插入num个元素val到pos之前，或者插入start到end之间的元素到pos的位置。返回值是一个迭代器，指向被插入的元素。
	 */
	//Insert();
    //Remove();
    //Find();

	BOOL IsEmpty()
    {
        return FALSE;
    };
	//ValType operator[](const KeyType& ktKey);
	VALUE operator[](KEY key)
    {
        VALUE t;
        return t;
    };
	
	DWORD Size()
    {
        return 0;
    };

};
#endif // !defined(EA_765F26B4_059F_4c89_A13D_66725C18520B__INCLUDED_)

