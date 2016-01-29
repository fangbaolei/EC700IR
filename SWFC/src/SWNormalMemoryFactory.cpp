#include "swpa.h"
#include "SWFC.h"
#include "SWNormalMemoryFactory.h"

/**
* @brief 构造函数
*/
CSWNormalMemoryFactory::CSWNormalMemoryFactory()
{
	swpa_mem_init();
}
/**
* @brief 析构函数
*/
CSWNormalMemoryFactory::~CSWNormalMemoryFactory()
{
	CSWAutoLock aLock(&m_cMutex);
	while(!m_cMemoryList.IsEmpty())
	{
		CSWMemory* pMemory = m_cMemoryList.GetHead();
		Free(pMemory);
		m_cMemoryList.RemoveHead();
	}
	swpa_mem_uninit();
}

/**
* @brief 申请内存
* 
* @param [in] dwSize : 要申请内存的大小
* @return
* - NULL : 申请失败
* - 其它: 分配到的内存对象指针
*/
CSWMemory* CSWNormalMemoryFactory::Alloc(const INT& iSize)
{
	CSWAutoLock aLock(&m_cMutex);
	PVOID pvAddr = swpa_mem_alloc(iSize);
	PVOID pvPhys = pvAddr;
	if( pvAddr == NULL )
	{
		SWFC_DEBUG_ERR("swpa_mem_alloc failed!");
		return NULL;
	}
	CSWMemory* pMemory = new CSWMemory(pvAddr, pvPhys, iSize, this);
	m_cMemoryList.AddTail(pMemory);
	return pMemory;
}

/**
* @brief 释放内存
* 
* @param [in] pMemory : 要释放的内存对象指针
* @return
* - S_OK : 成功
* - E_FAIL: 失败
*/
HRESULT CSWNormalMemoryFactory::Free(CSWMemory* pMemory)
{
	CSWAutoLock aLock(&m_cMutex);
	if( NULL == pMemory )
	{
		return E_INVALIDARG;
	}

	SW_POSITION pos = m_cMemoryList.GetHeadPosition();
	while(m_cMemoryList.IsValid(pos))
	{
		SW_POSITION posLast = pos;
		CSWMemory* pMem = m_cMemoryList.GetNext(pos);
		if( pMem == pMemory )
		{
			INT iRet = swpa_mem_free(pMemory->GetBuffer());
			m_cMemoryList.RemoveAt(posLast);
			delete pMem;
			pMem = NULL;
			return iRet == 0 ? S_OK : E_FAIL;
		}
	}

	SWFC_DEBUG_ERR("CSWNormalMemoryFactory free failed!");
	return E_FAIL;
}

/**
* @brief 获取内存的可用量，单位为字节
* 
* @param [out] pdwSize : 获取到的可用量
* @return
* - S_OK : 成功
* - E_FAIL : 失败
*/
HRESULT CSWNormalMemoryFactory::GetAvailableSize(DWORD * pdwSize)
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
HRESULT CSWNormalMemoryFactory::GetTotalSize(DWORD * pdwSize)
{
	return E_NOTIMPL;
}

