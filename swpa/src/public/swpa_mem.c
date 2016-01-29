
#include "swpa_mem.h"
#include "swpa_private.h"
#include "swpa_sem.h"

#include <sys/shm.h>
#include <sys/ipc.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>

#ifdef SWPA_MEM
#define SWPA_MEM_PRINT(fmt, ...) SWPA_PRINT("[%s:%d]"fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#define SWPA_MEM_CHECK(arg)      {if (!(arg)){SWPA_CAMERA_PRINT("[%s:%d]Check failed : %s [%d]\n", __FILE__, __LINE__, #arg, SWPAR_INVALIDARG);return SWPAR_INVALIDARG;}}
#else
#define SWPA_MEM_PRINT(fmt, ...)
#define SWPA_MEM_CHECK(arg)
#endif
/**
* @brief 初始化系统堆内存
* @retval 0 成功
* @retval -1 失败
* @see swpa_mem.h
*/
int swpa_mem_init(void)
{
	return 0;
}

/**
* @brief 反初始化系统堆内存
* @retval 0 成功
* @retval -1 失败
* @see swpa_mem.h
*/
int swpa_mem_uninit(void)
{
	return 0;
}

/**
* @brief 分配系统堆内存
* @param [in] len 内存长度
* @retval 内存地址
* @see swpa_mem.h
*/
void* swpa_mem_alloc(unsigned int len)
{
	return len > 0 ?  malloc(len) : (void *)0;
}

/**
* @brief 释放系统堆内存
* @param [in] addr 内存地址
* @retval 0 成功
* @retval -1 失败
* @see swpa_mem.h
*/
int swpa_mem_free(void* addr)
{
	if( addr != NULL )
	{
		free(addr);
	}
	return 0;
}

/**
* @brief 获取系统堆内存信息
* @param [out] info 内存信息。其中包括内存的总量以及可用量等相关信息。
* @retval 0 成功
* @retval -1 失败
* @see swpa_mem.h
*/
int swpa_mem_getinfo(SWPA_MEM_INFO* info)
{
	SWPA_MEM_PRINT("info=0x%08x\n", (unsigned long)info);
	if( NULL == info )
	{
		return SWPAR_INVALIDARG;
	}
	return 0;
}

/**
* @brief 初始化快速内存
* @retval 0 成功
* @retval -1 失败
* @see swpa_mem.h
*/
int swpa_cachemem_init(void)
{
	return 0;
}

/**
* @brief 反初始化快速内存
* @retval 0 成功
* @retval -1 失败
* @see swpa_mem.h
*/
int swpa_cachemem_uninit(void)
{
	return 0;
}

/**
* @brief 分配快速内存
* @param [in] segid 内存位置标识
* @param [in] len 内存长度
* @param [in] align_size 内存首地址对齐大小
* @retval 内存地址
* @see swpa_mem.h
* @note 快速内存即片上（on-chip）内存，在DSP中可配。
*/
void* swpa_cachemem_alloc(int segid, unsigned int len, unsigned int align_size)
{
	return NULL;
}

/**
* @brief 释放快速内存
* @param [in] segid 内存位置标识
* @param [in] addr 内存地址
* @retval 0 成功
* @retval -1 失败
* @see swpa_mem.h
*/
int swpa_cachemem_free(int segid, void* addr)
{
	return SWPAR_NOTIMPL;
}

/**
* @brief 获取快速内存信息
* @param [out] info 内存信息。其中包括内存的总量以及可用量等相关信息。
* @retval 0 成功
* @retval -1 失败
* @see swpa_mem.h
*/
int swpa_cachemem_getinfo(SWPA_MEM_INFO* info)
{
	SWPA_MEM_PRINT("info=0x%08x\n", (unsigned long)info);
	if( NULL == info )
	{
		return SWPAR_INVALIDARG;
	}
	return 0;
}


static void* g_pvSharedMemoryAddr = NULL;
static void* g_pvSharedMemoryPhys = NULL;
static unsigned int g_nOffset = 0;
static int g_iAddrIsBig = 0;
static int g_iSharedMemorySize = 0;
static int g_iSharedMemoryInitialized = 0;
static int g_mutex;
static const unsigned int g_nUsedFlag = 0xAABBCCDD;

/**
* 共享内存节点结构。
* fix用于填充，确保128字节对齐，DMA操作要求。
**/
typedef struct tagMEM
{
	int size;
	struct tagMEM *prev;
	struct tagMEM *next;
	unsigned int rev;
	unsigned char fix[112];
}
MEM_NODE;

static MEM_NODE* g_pHeader = NULL;
static int g_iMemNodeCount = 0;


/**
* @brief 初始化共享内存
* @retval 0 成功
* @retval -1 失败
* @see swpa_mem.h
* @note 所谓的共享内存即TI达芬奇架构中能让ARM和DSP互访的内存。
*/
int swpa_sharedmem_init(void)
{
	int iRet = 0;
	if( g_iSharedMemoryInitialized == 0 )
	{
#ifdef CROSS_COMPILE
		iRet = drv_cmem_init(&g_pvSharedMemoryAddr, &g_pvSharedMemoryPhys, &g_iSharedMemorySize);
#else
		g_pvSharedMemoryAddr = malloc(128 * 1024 * 1024 + 1000);
		g_pvSharedMemoryPhys = g_pvSharedMemoryAddr + 1000;
		g_iSharedMemorySize = 128 * 1024 * 1024;
#endif

		if( 0 != iRet || g_iSharedMemorySize < sizeof(MEM_NODE) )
		{
			return iRet;
		}
		g_pHeader = (MEM_NODE*)g_pvSharedMemoryAddr;
		g_pHeader->size = 0;
		g_pHeader->next = NULL;
		g_pHeader->prev = NULL;
		g_pHeader->rev = g_nUsedFlag;
		g_iMemNodeCount = g_iSharedMemorySize / sizeof(MEM_NODE);
		if( (unsigned int)g_pvSharedMemoryAddr > (unsigned int)g_pvSharedMemoryPhys )
		{
			g_iAddrIsBig = 1;
			g_nOffset = (unsigned int)g_pvSharedMemoryAddr - (unsigned int)g_pvSharedMemoryPhys;
		}
		else
		{
			g_iAddrIsBig = 0;
			g_nOffset = (unsigned int)g_pvSharedMemoryPhys - (unsigned int)g_pvSharedMemoryAddr;
		}

		iRet = swpa_mutex_create(&g_mutex, NULL);
		if( iRet != 0 )
		{
			return iRet;
		}
		g_iSharedMemoryInitialized = 1;
	}
	return 0;
}

/**
* @brief 反初始化共享内存
* @retval 0 成功
* @retval -1 失败
* @see swpa_mem.h
*/
int swpa_sharedmem_uninit(void)
{
	if( g_iSharedMemoryInitialized == 1 )
	{
		g_pHeader = NULL;
		g_iMemNodeCount = 0;
		g_iSharedMemoryInitialized = 0;
		swpa_mutex_delete(&g_mutex);
		//return drv_cmem_exit();
	}
	return 0;
}

/**
* @brief 分配共享内存
* @param [out] addr 虚拟内存地址
* @param [out] phys 物理内存地址
* @param [in] len 内存长度
* @retval 0 成功
* @retval -1 失败
* @see swpa_mem.h
*/

int get_free_size(MEM_NODE* pNode1, MEM_NODE* pNode2)
{
	int iFreeNode = 0;
	if( pNode1 != NULL && pNode2 != NULL )
	{
		iFreeNode = pNode1 > pNode2 ? (pNode1 - &pNode2[1 + pNode2->size]) : (pNode2 - &pNode1[1 + pNode1->size]);
		if( iFreeNode > 0 )
		{
			iFreeNode -= 1;
		}
	}
	return iFreeNode;
}

int swpa_sharedmem_alloc(void** addr, void** phys, unsigned int len)
{
	int iAllocSize = 0;
	MEM_NODE* pNode = NULL;
	MEM_NODE* pNewNode = NULL;
	MEM_NODE* pMatchNode = NULL;
	MEM_NODE* pLargeNode = NULL;
	int iFreeSize = 0;
	int iMinFreeSize = g_iSharedMemorySize;

	len = ( ( len + 1023 ) >> 10 ) << 10;

	iAllocSize = (len + sizeof(MEM_NODE) - 1)/ sizeof(MEM_NODE);

	SWPA_MEM_PRINT("addr=0x%08x\n", (unsigned int)addr);
	SWPA_MEM_PRINT("phys=0x%08x\n", (unsigned int)phys);
	SWPA_MEM_PRINT("len=%d\n", len);
	if( NULL == g_pHeader || g_iSharedMemoryInitialized != 1 )
	{
		return SWPAR_NOTINITED;
	}

	swpa_mutex_lock(&g_mutex, -1);

//	if( swpa_share_mem_check() != 0 )
//	{
//		printf("swpa_sharedmem_alloc pre.\n");
//	}

	//遍历内存链表
	for(pNode = g_pHeader; NULL != pNode && NULL != pNode->next; pNode = pNode->next)
	{	//先查找大小相等的内存块
		iFreeSize = get_free_size(pNode, pNode->next);
		if(iFreeSize == iAllocSize)
		{
			pMatchNode = pNode;
			break;
		}
		//如果发现有大于要申请空间大小的空闲空间，置标志位
		else if(iFreeSize > iAllocSize && iFreeSize < iMinFreeSize )
		{
			pLargeNode = pNode;
			iMinFreeSize = iFreeSize;
		}
	}

	if( pMatchNode != NULL )
	{
		pNewNode = &pMatchNode[1 + pMatchNode->size];
		pNewNode->size = iAllocSize;
		pNewNode->next = pMatchNode->next;
		pNewNode->prev = pMatchNode;
		pMatchNode->next->prev = pNewNode;
		pMatchNode->next = pNewNode;
		pNewNode->rev = g_nUsedFlag;
	}
	else if( pNode->next == NULL )
	{
		//先判断剩余的内存
		pNewNode = &pNode[1 + pNode->size];
		if((&g_pHeader[g_iMemNodeCount - 1] - pNewNode) >= iAllocSize)
		{
			pNewNode->size = iAllocSize;
			pNewNode->next = NULL;
			pNewNode->prev = pNode;
			pNode->next = pNewNode;
			pNewNode->rev = g_nUsedFlag;
		}
		//在判断释放的内存
		else if(pLargeNode)
		{
			pNewNode = &pLargeNode[1 + pLargeNode->size];
			pNewNode->size = iAllocSize;
			pNewNode->prev = pLargeNode;
			pLargeNode->next->prev = pNewNode;
			pNewNode->next = pLargeNode->next;
			pLargeNode->next = pNewNode;
			pNewNode->rev = g_nUsedFlag;
		}
		else
		{
			pNewNode = NULL;
		}
	}

	if( pNewNode != NULL )
	{

		(*addr) = (void*)(&pNewNode[1]);
		if( 1 == g_iAddrIsBig )
		{
			(*phys) = (void*)((unsigned int)(*addr) - g_nOffset);
		}
		else
		{
			(*phys) = (void*)((unsigned int)(*addr) + g_nOffset);
		}
	}

//	if( swpa_share_mem_check() != 0 )
//	{
//		printf("swpa_sharedmem_alloc end.\n");
//	}

	swpa_mutex_unlock(&g_mutex);

	return pNewNode != NULL ? 0 : SWPAR_OUTOFMEMORY;
}

/**
* @brief 释放共享内存
* @param [in] addr 内存地址
* @param [in] is_phys_addr 指明内存地址是否为物理地址
* @retval 0 成功
* @retval -1 失败
* @see swpa_mem.h
*/
int swpa_sharedmem_free(void* addr, unsigned int is_phys_addr)
{
	MEM_NODE* pNode = NULL;
	void* pAddr = addr;

	SWPA_MEM_PRINT("addr=0x%08x\n", (unsigned int)addr);
	SWPA_MEM_PRINT("is_phys_addr=%d\n", is_phys_addr);
	if(addr == NULL)
	{
		return SWPAR_INVALIDARG;
	}
	if(NULL == g_pHeader)
	{
		return SWPAR_NOTINITED;
	}

	if( is_phys_addr == 1 )
	{
		pAddr = g_iAddrIsBig == 1 ? (void*)((unsigned int)pAddr + g_nOffset) : (void*)((unsigned int)pAddr - g_nOffset);
	}

	swpa_mutex_lock(&g_mutex, -1);

//	if( swpa_share_mem_check() != 0 )
//	{
//		printf("swpa_sharedmem_free pre.\n");
//	}

	pNode = (MEM_NODE*)pAddr;
	--pNode;

	if(NULL != g_pHeader && pNode > g_pHeader && pNode < g_pHeader + g_iMemNodeCount && pNode->rev == g_nUsedFlag)
	{
		pNode->rev = 0;
		if(pNode->prev)
		{
			pNode->prev->next = pNode->next;
		}
		if(pNode->next)
		{
			pNode->next->prev = pNode->prev;
		}
		pNode->prev = NULL;
		pNode->next = NULL;
		pNode->size = 0;
	}

//	if( swpa_share_mem_check() != 0 )
//	{
//		printf("swpa_sharedmem_free end.\n");
//	}

	swpa_mutex_unlock(&g_mutex);

	return SWPAR_OK;
}

/**
* @brief 获取共享内存信息
* @param [out] info 内存信息。其中包括内存的总量以及可用量等相关信息。
* @retval 0 成功
* @retval -1 失败
* @see swpa_mem.h
*/
int swpa_sharedmem_getinfo(SWPA_MEM_INFO* info)
{
	MEM_NODE* pNode = NULL;
	MEM_NODE* pNewNode = NULL;
	int iFreeSizeByte = 0;
	int iFreeSize = 0;
	int iUsedSize = 0;
	int iUsedSizeByte = 0;

	SWPA_MEM_PRINT("info=0x%08x\n", (unsigned long)info);
	if( NULL == info )
	{
		return SWPAR_INVALIDARG;
	}

	swpa_mutex_lock(&g_mutex, -1);
	//遍历内存链表
	for(pNode = g_pHeader; NULL != pNode && NULL != pNode->next; pNode = pNode->next)
	{	//先查找大小相等的内存块
		iFreeSize = get_free_size(pNode, pNode->next);
		iFreeSizeByte += (iFreeSize * sizeof(MEM_NODE));
		iUsedSize = pNode->size;
		iUsedSizeByte += (iUsedSize * sizeof(MEM_NODE));
	}
	if( pNode != NULL )
	{
		pNewNode = &pNode[1 + pNode->size];
		iFreeSize = (&g_pHeader[g_iMemNodeCount - 1] - pNewNode);
		iFreeSizeByte += (iFreeSize * sizeof(MEM_NODE));

		iUsedSize = pNode->size;
		iUsedSizeByte += (iUsedSize * sizeof(MEM_NODE));
	}
	info->free = iFreeSizeByte;
	info->size = g_iSharedMemorySize;
	info->used = iUsedSizeByte;

	swpa_mutex_unlock(&g_mutex);
	return 0;
}

int swpa_sharedmem_check()
{
	if( g_pHeader == NULL ) return 0;
	MEM_NODE* pNode = NULL;
	MEM_NODE* pLastNode = NULL;

	swpa_mutex_lock(&g_mutex, -1);
	for(pNode = g_pHeader; NULL != pNode && NULL != pNode->next; pNode = pNode->next)	
	{
		pLastNode = pNode;
		if( pNode->next->prev != pNode )
		{
			printf("<share mem check> node:0x%08x, next:0x%08x, nextpre:0x%08x.\n", (unsigned int)pNode, (unsigned int)pNode->next, (unsigned int)pNode->next->prev);
			swpa_mutex_unlock(&g_mutex);
			return 1;
		}
	}
	swpa_mutex_unlock(&g_mutex);

	if( pNode != NULL )
	{
		if( pNode->prev != pLastNode || pNode->next != NULL )
		{
			printf("<share mem check> node:0x%08x, lastnode:0x%08x, next:0x%08x, pre:0x%08x.\n", (unsigned int)pNode, (unsigned int)pLastNode, (unsigned int)pNode->next, (unsigned int)pNode->prev);
			return 1;
		}
	}
	return 0;
}

/* BEGIN: Modified by huangdch, 2015/9/2 
*共享内存操作接口*/
int swpa_normal_sharedmem_init()
{
	return 0;
}

int swpa_normal_sharedmem_uninit(void)
{
	return 0;
}

/* BEGIN: Modified by huangdch, 2015/8/26 
*只在当前这个.c文件中用*/

#define SHM_ACCESS_PRIVELEGE 0660   //创建共享内存的访问权限

typedef struct shmid_ds swpa_shmid_ds;

/*****************************************************************************
 函 数 名  : get_shm_sate
 功能描述  : 获取共享内存的状态
 输入参数  : int id                  
             swpa_shmid_ds* tshm_ds  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 注意事项  : 只在当前文件中使用
 
 修改历史      :
  1.日    期   : 2015年9月4日
    作    者   : huangdch
    修改内容   : 新生成函数

*****************************************************************************/
int shm_sate(int id, swpa_shmid_ds* tshm_ds)
{
	int ret = 0;
	int shm_id = shmget(id, 0, SHM_ACCESS_PRIVELEGE);
	if (shm_id < 0)
	{
		return -1;
	}
	return shmctl(shm_id, IPC_STAT, tshm_ds);
}

/* BEGIN: Modified by huangdch, 2015/9/6 
*共享内存创建*/
int shm_create(const int key, const int size, int* shm_id)
{
	int temp_shm_id = 0;
	temp_shm_id = shmget(key, size, SHM_ACCESS_PRIVELEGE | IPC_CREAT | IPC_EXCL);
	if (temp_shm_id == -1)
	{
		if (EEXIST == errno)
		{
			return 1;/* 共享内存已经存在 */
		}
		return -1;
	}

	struct shmid_ds tshm_ds;
	int iRet = shmctl(temp_shm_id, IPC_STAT, &tshm_ds);
	if (iRet != 0)
	{
		shmctl(temp_shm_id, IPC_RMID, NULL);
		temp_shm_id = -1;
		return -2;
	}
	if (tshm_ds.shm_segsz != size)
	{
		shmctl(temp_shm_id, IPC_RMID, NULL);
		temp_shm_id = -1;
		return -3;
	}
	*shm_id = temp_shm_id;

	return 0;
}

/* BEGIN: Modified by huangdch, 2015/9/6 
*共享内存连接*/
int shm_attach(const int key, void** paddr)
{
	int ret = 0;
	int shm_id = shmget(key, 0, SHM_ACCESS_PRIVELEGE);
	if (shm_id < 0)
	{
		return -1;
	}
	*paddr = 0;
	*paddr = shmat(shm_id, 0, 0);
	if ((int)*paddr == -1)
	{
		return -2;
	}
	
	return SWPAR_OK;
}

/* BEGIN: Modified by huangdch, 2015/9/6 
*共享内存断开*/
int shm_detach(void* paddr)
{
	int ret = shmdt(paddr);
	if (ret != 0)
	{
		return -1;
	}

	return 0;
}

/* BEGIN: Modified by huangdch, 2015/9/6 
*共享内存释放*/
int shm_destroy(const int key, const int model)
{
	int shm_id;
	struct shmid_ds ds_shmid;

	shm_id = shmget(key, 0, 0);
	if (shm_id < 0)
	{
		return -1;
	}

	shmctl(shm_id, IPC_STAT, &ds_shmid);
	if (ds_shmid.shm_nattch > 0)
	{
		return ds_shmid.shm_nattch; /* 返回共享内存当前的连接数 */
	}

	/* 只有创建者才能真正销毁 */
	if (!model && ds_shmid.shm_cpid != getpid())
	{
		return -2;
	}

	shmctl(shm_id, IPC_RMID, 0);

	return 0;
}


/*****************************************************************************
 函 数 名  : get_shm_sate
 功能描述  : 获取共享内存的状态
 输入参数  : int id                  
             swpa_shmid_ds* tshm_ds  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 注意事项  : 只在当前文件中使用
 
 修改历史      :
  1.日    期   : 2015年9月4日
    作    者   : huangdch
    修改内容   : 新生成函数

*****************************************************************************/
int get_shm_sate(int id, swpa_shmid_ds* tshm_ds)
{
	int ret = 0;
	int shm_id = shmget(id, 0, SHM_ACCESS_PRIVELEGE);
	if (shm_id < 0)
	{
		return -1;
	}
	return shmctl(shm_id, IPC_STAT, tshm_ds);
}

void* swpa_normal_sharedmem_alloc(const int id, const int size, int* state)
{
	if (id < 1 || size < 1)
	{
		SWPA_MEM_PRINT("alloc id=0x%08x(%d) size=[%d].\n", id, id, size);
		return -1;
	}

	int ret = 0;
	int shm_id = 0;
	int temp_state = 0;
	void* pAdd = NULL;

	/* 先连接 */
	temp_state = *state;
	ret = shm_attach(id, (void**)&pAdd);
	if (0 == ret)
	{
		swpa_shmid_ds* ptshm_ds = (swpa_shmid_ds*)swpa_mem_alloc(sizeof(*ptshm_ds));
		if (NULL != ptshm_ds)
		{
			get_shm_sate(id, ptshm_ds);
			*state = ptshm_ds->shm_segsz; //返回实际的大小
			swpa_mem_free(ptshm_ds);

			goto ALLOC_SUCCESS;
		}
		swpa_mem_free(ptshm_ds);

		SWPA_MEM_PRINT("attch key_id=0x%08x(%d) size=[%d].\n", id, id, size);

		*state = 1;
		goto ALLOC_SUCCESS;
	}

	/* 如果内存不存在且不需要创建返回失败 */
	if (0 != ret && -1 == temp_state)
	{
		*state = -2;
		goto ALLOC_SUCCESS;
	}

	/* 重新建共享内存 */
	ret = shm_create(id, size, &shm_id);
	if (ret < 0)
	{
		*state = -3;
		goto ALLOC_SUCCESS;
	}

	pAdd = NULL;		
	ret = shm_attach(id, (void**)&pAdd);
	if (0 != ret)
	{
		*state = -4;
		goto ALLOC_SUCCESS;
	}

	SWPA_MEM_PRINT("alloc key_id=0x%08x(%d) size=[%d].\n", id, id, size);
	*state = 0; /* 第一次创建 */

	ALLOC_SUCCESS :/* 分配统一出口 */

	return pAdd;
}

int swpa_normal_sharedmem_free(const int id, void* pAdd)
{
	if (0 == id)
	{
		SWPA_MEM_PRINT("free:id=[%d],pAdd=[%p]\n", id, pAdd);
		return -1;
	}

	int ret = 0;

	if (NULL != pAdd)
	{
		ret = shm_detach(pAdd);
	}
	if (0 == ret)
	{
		ret = shm_destroy(id, 1);
		if (0 == ret)
		{
			SWPA_MEM_PRINT("free key_id=0x%08x(%d) success.\n", id, id);
		}
		else
		{
		    SWPA_MEM_PRINT("free key_id=0x%08x(%d) failed,ret=[%d].\n", id, id, ret);
		}
	}
	
	return ret;
}


