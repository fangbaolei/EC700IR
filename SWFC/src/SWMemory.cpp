///////////////////////////////////////////////////////////
//  CSWMemory.cpp
//  Implementation of the Class CSWMemory
//  Created on:      28-二月-2013 14:09:48
//  Original author: zhouy
///////////////////////////////////////////////////////////
#include "SWFC.h"
#include "SWMemory.h"

/**
 * @brief 构造函数
 */
CSWMemory::CSWMemory(PVOID pvAddr, PVOID pvPhys, const INT& iSize, CSWMemoryFactory* pMemoryFactory)
	: m_pvAddr(pvAddr)
	, m_pvPhys(pvPhys)
	, m_iSize(iSize)
	, m_iTag(0)
	, m_pMemoryFactory(pMemoryFactory)
{
}

/**
 * @brief 析构函数
 */
CSWMemory::~CSWMemory()
{
	
}

/**
* @brief  获取对象已申请的内存。
*
* @param [in] iAddrType : 地址类型：0: 默认与运行平台有关，1：强制取物理地址，2：强制取虚拟地址。
* @return
* - 内存首地址
*/
PVOID CSWMemory::GetBuffer(const INT& iAddrType)
{
	PVOID pvRet = NULL;
	switch(iAddrType)
	{
	case 0:
#if RUN_PLATFORM == PLATFORM_DSP_BIOS
		pvRet = m_pvPhys;
#else 
		pvRet = m_pvAddr;
#endif
		break;
	case 1:
		pvRet = m_pvPhys;
		break;
	case 2:
		pvRet = m_pvAddr;
		break;
	default:
		pvRet = NULL;
		break;
	}
	return pvRet;
}

/**
* @brief  获取对象已申请内存的大小。
*
* @param [in] iAddrType : 地址类型：0: 默认与运行平台有关，1：强制取物理地址，2：强制取虚拟地址。
* @return
*  -大于零：内存的大小。
*  -小于等于零：错误。
*/ 
INT CSWMemory::GetSize()
{
	return m_iSize;
}



