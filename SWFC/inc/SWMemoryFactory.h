///////////////////////////////////////////////////////////
//  CSWMemory.h
//  Implementation of the Class CSWMemory
//  Created on:      28-二月-2013 14:09:48
//  Original author: zhouy
///////////////////////////////////////////////////////////

#if !defined(SWMEMORYFACTORY__INCLUDED_)
#define SWMEMORYFACTORY__INCLUDED_

#include "SWObject.h"
#include "SWMemory.h"

typedef enum
{
	SW_NORMAL_MEMORY = 0,
	SW_SHARED_MEMORY,
	SW_CACHE_MEMORY,
	SW_MAPPED_MEMORY,
	SW_SHARED_SR_MEMORY
}
SW_MEMORY_TYPE;


/**
 * @brief 内存管理基类
 */
class CSWMemoryFactory : public CSWObject
{
	
CLASSINFO(CSWMemoryFactory,CSWObject)

public:
	static CSWMemoryFactory* GetInstance(const SW_MEMORY_TYPE& cMemoryType = SW_SHARED_MEMORY);

public:
	/**
	 * @brief 申请内存
	 * 
	 * @param [in] dwSize : 要申请内存的大小
	 * @return
	 * - NULL : 申请失败
	 * - 其它: 分配到的内存对象指针
	 */
	virtual CSWMemory* Alloc(const INT& iSize);
	
	/**
	 * @brief 申请内存
	 * 
	 * @param [in] pAddr  : 内存虚地址
	 * @param [in] pPhys  : 内存实际地址
	 * @param [in] dwSize : 要申请内存的大小
	 * @return
	 * - NULL : 申请失败
	 * - 其它: 分配到的内存对象指针
	 */
	virtual CSWMemory* Alloc(PVOID pAddr, PVOID pPhys, const INT& iSize);

	/**
	 * @brief 释放内存
	 * 
	 * @param [in] pMemory : 要释放的内存对象指针
	 * @return
	 * - S_OK : 成功
	 * - E_FAIL: 失败
	 */
	virtual HRESULT Free(CSWMemory* pMemory);

	/**
	 * @brief 获取内存的可用量，单位为字节
	 * 
	 * @param [out] pdwSize : 获取到的可用量
	 * @return
	 * - S_OK : 成功
	 * - E_FAIL : 失败
	 */
	virtual HRESULT GetAvailableSize(DWORD * pdwSize);
	/**
	 * @brief 获取内存的总量，单位为字节
	 * 
	 * @param [out] pdwSize : 获取到的总量
	 * @return
	 * - S_OK : 成功
	 * - E_FAIL : 失败
	 */
	virtual HRESULT GetTotalSize(DWORD * pdwSize);

	virtual DWORD AddRef()
	{
		return 0;
	}
	virtual HRESULT Release()
	{
		return S_OK;
	}

protected:
	/**
	 * @brief 构造函数
	 */
	CSWMemoryFactory();
	/**
	 * @brief 析构函数
	 */
	virtual ~CSWMemoryFactory();
};
#endif // !defined(SWMEMORYFACTORY__INCLUDED_)

