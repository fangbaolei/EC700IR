#include "SWFC.h"
#include "SWMemoryFactory.h"
#include "SWNormalMemoryFactory.h"
#include "SWSharedMemoryFactory.h"
#include "SWMappedMemoryFactory.h"
#include "SWCacheMemoryFactory.h"
#include "SWSharedSRMemoryFactory.h"

/**
* @brief 构造函数
*/
CSWMemoryFactory::CSWMemoryFactory()
{
}
/**
* @brief 析构函数
*/
CSWMemoryFactory::~CSWMemoryFactory()
{
}

CSWMemoryFactory* CSWMemoryFactory::GetInstance(const SW_MEMORY_TYPE& cMemoryType)
{
	CSWMemoryFactory *pMemory = NULL;
	switch(cMemoryType)
	{
	case SW_NORMAL_MEMORY:
		{
			static CSWNormalMemoryFactory normalMemory;
			pMemory = &normalMemory;
		}
		break;
		
	case SW_SHARED_MEMORY:
		{
			static CSWSharedMemoryFactory shareMemory;
			pMemory = &shareMemory;
		}
		break;

	case SW_CACHE_MEMORY:
		{
			static CSWCacheMemoryFactory cacheMemory;
			pMemory = &cacheMemory;
		}
		break;

	case SW_MAPPED_MEMORY:
		{
			static CSWMappedMemoryFactory mmapMemory;
			pMemory = &mmapMemory;
		}
		break;
	case SW_SHARED_SR_MEMORY:
		{
			static CSWSharedSRMemoryFactory srShareMemory;
			pMemory = &srShareMemory;
		}	
		break;
	}
	return pMemory;
}

/**
 * @brief 申请内存
 * 
 * @param [in] dwSize : 要申请内存的大小
 * @return
 * - NULL : 申请失败
 * - 其它: 分配到的内存对象指针
 */
CSWMemory* CSWMemoryFactory::Alloc(const INT& iSize)
{  
	return NULL;
}

CSWMemory* CSWMemoryFactory::Alloc(PVOID pAddr, PVOID pPhys, const INT& iSize)
{
	return Alloc(iSize);
}

/**
 * @brief 释放内存
 * 
 * @param [in] pMemory : 要释放的内存对象指针
 * @return
 * - S_OK : 成功
 * - E_FAIL: 失败
 */
HRESULT CSWMemoryFactory::Free(CSWMemory* pMemory)
{
	return E_NOTIMPL;
}

/**
 * @brief 获取内存的可用量，单位为字节
 * 
 * @param [out] pdwSize : 获取到的可用量
 * @return
 * - S_OK : 成功
 * - E_FAIL : 失败
 */
HRESULT CSWMemoryFactory::GetAvailableSize(DWORD * pdwSize)
{
	return E_NOTIMPL;
}

/**
 * @brief 获取内存的总量，单位为字节
 * 
 * @param [out] pdwSize : 获取到的总量
 * @return
 * - S_OK : 成功
 * - E_FAIL : 失败
 */
HRESULT CSWMemoryFactory::GetTotalSize(DWORD * pdwSize)
{
	return E_NOTIMPL;
}

