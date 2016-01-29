#include "SWFC.h"
#include "SWSharedMemoryFactory.h"
#include "swpa_mem.h"

/**
* @brief 构造函数
*/
CSWSharedMemoryFactory::CSWSharedMemoryFactory()
{
	swpa_sharedmem_init();
}
/**
* @brief 析构函数
*/
CSWSharedMemoryFactory::~CSWSharedMemoryFactory()
{
	CSWAutoLock aLock(&m_cMutex);
	while(!m_cMemoryList.IsEmpty())
	{
		CSWMemory* pMemory = m_cMemoryList.GetHead();
		Free(pMemory);
		m_cMemoryList.RemoveHead();
	}
	swpa_sharedmem_uninit();
}

/**
* @brief 申请内存
* 
* @param [in] dwSize : 要申请内存的大小
* @return
* - NULL : 申请失败
* - 其它: 分配到的内存对象指针
*/
CSWMemory* CSWSharedMemoryFactory::Alloc(const INT& iSize)
{
	CSWAutoLock aLock(&m_cMutex);
	PVOID pvAddr = NULL;
	PVOID pvPhys = NULL;
	INT iRet = swpa_sharedmem_alloc(&pvAddr, &pvPhys, iSize);
	if( iRet != 0 )
	{
		SWFC_DEBUG_ERR("swpa_sharedmem_alloc failed!");
		return NULL;
	}
	CSWMemory* pMemory = new CSWMemory(pvAddr, pvPhys, iSize, this);
	SW_POSITION pos = m_cMemoryList.AddTail(pMemory);
	if( !m_cMemoryList.IsValid(pos) )
	{
		SW_TRACE_DEBUG("SharedMemoryFactory alloc list full!.\n");
	}
	return pMemory;
}

CSWMemory* CSWSharedMemoryFactory::Alloc(PVOID pAddr, PVOID pPhys, const INT& iSize)
{
	CSWAutoLock aLock(&m_cMutex);
	CSWMemory* pMemory = new CSWMemory(pAddr, pPhys, iSize, this);
	pMemory->SetTag(1);
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
HRESULT CSWSharedMemoryFactory::Free(CSWMemory* pMemory)
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
			INT iRet = 0;
			if(!pMemory->GetTag())
			{
				iRet = swpa_sharedmem_free(pMemory->GetBuffer(MEM_PHY_ADDR), 1);
			}
			else
			{
				swpa_ipnc_release(pMemory->GetBuffer());
			}
			m_cMemoryList.RemoveAt(posLast);
			delete pMem;
			pMem = NULL;
			return iRet == 0 ? S_OK : E_FAIL;
		}
	}

	SWFC_DEBUG_ERR("CSWSharedMemoryFactory free failed!");
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
HRESULT CSWSharedMemoryFactory::GetAvailableSize(DWORD * pdwSize)
{
	if( pdwSize == NULL )
	{
		return E_INVALIDARG;
	}

	*pdwSize = 0;
	SWPA_MEM_INFO cMemInfo;
	if( 0 == swpa_sharedmem_getinfo(&cMemInfo) )
	{
		(*pdwSize) = cMemInfo.free;
		return S_OK;
	}

	return E_FAIL;
}
/**
* @brief 获取内存的总量，单位为字节
* 
* @param [out] pdwSize : 获取到的总量
* @return
* - S_OK : 成功
* - E_FAIL : 失败
*/
HRESULT CSWSharedMemoryFactory::GetTotalSize(DWORD * pdwSize)
{
	if( pdwSize == NULL )
	{
		return E_INVALIDARG;
	}

	*pdwSize = 0;
	SWPA_MEM_INFO cMemInfo;
	if( 0 == swpa_sharedmem_getinfo(&cMemInfo) )
	{
		(*pdwSize) = cMemInfo.size;
		return S_OK;
	}

	return E_FAIL;
}

