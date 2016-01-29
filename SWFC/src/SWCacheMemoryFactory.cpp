#include "SWFC.h"
#include "SWCacheMemoryFactory.h"

/**
* @brief 构造函数
*/
CSWCacheMemoryFactory::CSWCacheMemoryFactory()
{
}
/**
* @brief 析构函数
*/
CSWCacheMemoryFactory::~CSWCacheMemoryFactory()
{
}

/**
* @brief 申请内存
* 
* @param [in] dwSize : 要申请内存的大小
* @return
* - NULL : 申请失败
* - 其它: 分配到的内存对象指针
*/
CSWMemory* CSWCacheMemoryFactory::Alloc(const INT& iSize)
{
	return NULL;
}

/**
* @brief 释放内存
* 
* @param [in] pMemory : 要释放的内存对象指针
* @return
* - S_OK : 成功
* - E_FAIL: 失败
*/
HRESULT CSWCacheMemoryFactory::Free(CSWMemory* pMemory)
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
HRESULT CSWCacheMemoryFactory::GetAvailableSize(DWORD * pdwSize)
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
HRESULT CSWCacheMemoryFactory::GetTotalSize(DWORD * pdwSize)
{
	return E_NOTIMPL;
}

