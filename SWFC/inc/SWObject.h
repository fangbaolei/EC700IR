///////////////////////////////////////////////////////////
//  CSWObject.h
//  Implementation of the Class CSWObject
//  Created on:      28-二月-2013 14:09:46
//  Original author: zy
///////////////////////////////////////////////////////////

#if !defined(EA_C467BC58_35F4_49e2_9076_E1667B943475__INCLUDED_)
#define EA_C467BC58_35F4_49e2_9076_E1667B943475__INCLUDED_

#include "SWBaseType.h"
#include "SWErrCode.h"
#include "SWBaseDefine.h"
#include "SWUtils.h"
#include "SWVariant.h"
/**
 * @brief SWFC所有基类的父类
 * 
 * 只实现对实例引用计数的增加和减少，并在引用计数减少为0时销毁掉实例。
 */
class CSWObject
{
CLASSINFO(CSWObject,CSWObject)

public:
	/**
	 * @brief 构造函数
	 */
	CSWObject();

	/**
	 * @brief 析构函数
	 */
	virtual ~CSWObject();

	/**
	 * @brief 引用计数加1
	 */
	virtual DWORD AddRef();

	/**
	 * @brief 引用计数减1，并在计数为0时销毁掉该对象
	 * 
	 * @return
	 * - S_OK : 成功
	 * - E_FAIL : 失败
	 */
	virtual HRESULT Release();

	/**
	   @brief 返回引用数
	 */
	virtual DWORD GetRefCount();

	//自动化函数列表宏定义
	SW_BEGIN_DISP_MAP(CSWObject,CSWObject)
	SW_END_DISP_MAP()
private:
	/**
	 * @brief 引用计数器
	 */
	DWORD m_dwRef;
	INT   m_hMutex;
};

#endif // !defined(EA_C467BC58_35F4_49e2_9076_E1667B943475__INCLUDED_)

