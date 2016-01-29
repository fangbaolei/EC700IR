///////////////////////////////////////////////////////////
//  CSWMemory.h
//  Implementation of the Class CSWMemory
//  Created on:      28-二月-2013 14:09:48
//  Original author: zhouy
///////////////////////////////////////////////////////////

#if !defined(EA_7CC7DD84_1E9A_48b1_A204_2C2984898AAD__INCLUDED_)
#define EA_7CC7DD84_1E9A_48b1_A204_2C2984898AAD__INCLUDED_
#include "SWObject.h"

enum
{
	MEM_PLATFORM_DEFAULT = 0,
	MEM_PHY_ADDR = 1,
	MEM_VIR_ADDR = 2
};

/**
 * @brief 内存类
 */
class CSWMemoryFactory;
class CSWNormalMemoryFactory;
class CSWSharedMemoryFactory;
class CSWCacheMemoryFactory;
class CSWMappedMemoryFactory;
class CSWSharedSRMemoryFactory;
class CSWMemory : public CSWObject
{
	friend class CSWMemoryFactory;
	friend class CSWNormalMemoryFactory;
	friend class CSWSharedMemoryFactory;
	friend class CSWCacheMemoryFactory;
	friend class CSWMappedMemoryFactory;
	friend class CSWSharedSRMemoryFactory;
	CLASSINFO(CSWMemory,CSWObject)
public:
	/**
	 * @brief  获取对象已申请的内存。
	 *
	 * @param [in] iAddrType : 地址类型：MEM_PLATFORM_DEFAULT: 默认与运行平台有关，MEM_PHY_ADDR：强制取物理地址，MEM_VIR_ADDR：强制取虚拟地址。
	 * @return
	 * - 内存首地址
	 */
	PVOID GetBuffer(const INT& iAddrType = MEM_PLATFORM_DEFAULT);
	/**
	 * @brief  获取对象已申请内存的大小。
	 *
	 * @param [in] iAddrType : 地址类型：0: 默认与运行平台有关，1：强制取物理地址，2：强制取虚拟地址。
	 * @return
	 *  -大于零：内存的大小。
	 *  -小于等于零：错误。
	 */ 
	INT GetSize();
	
	VOID SetTag(INT iTag){m_iTag = iTag;}
	INT  GetTag(VOID)    {return m_iTag;}
	
private:
	/**
	 * @brief 构造函数
	*/
	CSWMemory(PVOID pvAddr, PVOID pvPhys, const INT& iSize, CSWMemoryFactory* pMemoryFactory);
	/**
	 * @brief 析构函数
	 */
	~CSWMemory();

private:
	PVOID m_pvAddr;
	PVOID m_pvPhys;
	INT	  m_iSize;
	INT   m_iTag;
	CSWMemoryFactory* m_pMemoryFactory;
};
#endif // !defined(EA_7CC7DD84_1E9A_48b1_A204_2C2984898AAD__INCLUDED_)

