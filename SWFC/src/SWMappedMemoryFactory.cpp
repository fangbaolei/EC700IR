#include "SWMappedMemoryFactory.h"

/**
* @brief 构造函数
*/
CSWMappedMemoryFactory::CSWMappedMemoryFactory()
{

}
/**
* @brief 析构函数
*/
CSWMappedMemoryFactory::~CSWMappedMemoryFactory()
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
CSWMemory* CSWMappedMemoryFactory::Alloc(const INT& iSize)
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
HRESULT CSWMappedMemoryFactory::Free(CSWMemory* pMemory)
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
HRESULT CSWMappedMemoryFactory::GetAvailableSize(DWORD * pdwSize)
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
HRESULT CSWMappedMemoryFactory::GetTotalSize(DWORD * pdwSize)
{
	return E_NOTIMPL;
}

