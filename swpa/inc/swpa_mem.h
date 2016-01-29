/**
* @file swpa_mem.h
* @brief 内存管理
* @author Shaorg
* @date 2013-02-26
* @version v0.1
* @note 包含对各种内存进行申请、释放以及查询的一组函数。
*/

#ifndef _SWPA_MEM_H_
#define _SWPA_MEM_H_

#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct tag_meminfo
{
	unsigned int size;
	unsigned int used;
	unsigned int free;
}
SWPA_MEM_INFO;

/**
* @brief 初始化系统堆内存
* @retval 0 成功
* @retval -1 失败
* @see swpa_mem.h
*/
int swpa_mem_init(void);

/**
* @brief 反初始化系统堆内存
* @retval 0 成功
* @retval -1 失败
* @see swpa_mem.h
*/
int swpa_mem_uninit(void);

/**
* @brief 分配系统堆内存
* @param [in] len 内存长度
* @retval 内存地址
* @see swpa_mem.h
*/
void* swpa_mem_alloc(unsigned int len);

/**
* @brief 释放系统堆内存
* @param [in] addr 内存地址
* @retval 0 成功
* @retval -1 失败
* @see swpa_mem.h
*/
int swpa_mem_free(void* addr);

/**
* @brief 获取系统堆内存信息
* @param [out] info 内存信息。其中包括内存的总量以及可用量等相关信息。
* @retval 0 成功
* @retval -1 失败
* @see swpa_mem.h
*/
int swpa_mem_getinfo(SWPA_MEM_INFO* info);

/**
* @brief 初始化快速内存
* @retval 0 成功
* @retval -1 失败
* @see swpa_mem.h
*/
int swpa_cachemem_init(void);

/**
* @brief 反初始化快速内存
* @retval 0 成功
* @retval -1 失败
* @see swpa_mem.h
*/
int swpa_cachemem_uninit(void);

/**
* @brief 分配快速内存
* @param [in] segid 内存位置标识
* @param [in] len 内存长度
* @param [in] align_size 内存首地址对齐大小
* @retval 内存地址
* @see swpa_mem.h
* @note 快速内存即片上（on-chip）内存，在DSP中可配。
*/
void* swpa_cachemem_alloc(int segid, unsigned int len, unsigned int align_size);

/**
* @brief 释放快速内存
* @param [in] segid 内存位置标识
* @param [in] addr 内存地址
* @retval 0 成功
* @retval -1 失败
* @see swpa_mem.h
*/
int swpa_cachemem_free(int segid, void* addr);

/**
* @brief 获取快速内存信息
* @param [out] info 内存信息。其中包括内存的总量以及可用量等相关信息。
* @retval 0 成功
* @retval -1 失败
* @see swpa_mem.h
*/
int swpa_cachemem_getinfo(SWPA_MEM_INFO* info);

/**
* @brief 初始化共享内存
* @retval 0 成功
* @retval -1 失败
* @see swpa_mem.h
* @note 所谓的共享内存即TI达芬奇架构中能让ARM和DSP互访的内存。
*/
int swpa_sharedmem_init(void);

/**
* @brief 反初始化共享内存
* @retval 0 成功
* @retval -1 失败
* @see swpa_mem.h
*/
int swpa_sharedmem_uninit(void);

/**
* @brief 分配共享内存
* @param [out] addr 虚拟内存地址
* @param [out] phys 物理内存地址
* @param [in] len 内存长度
* @retval 0 成功
* @retval -1 失败
* @see swpa_mem.h
*/
int swpa_sharedmem_alloc(void** addr, void** phys, unsigned int len);

/**
* @brief 释放共享内存
* @param [in] addr 内存地址
* @param [in] is_phys_addr 指明内存地址是否为物理地址
* @retval 0 成功
* @retval -1 失败
* @see swpa_mem.h
*/
int swpa_sharedmem_free(void* addr, unsigned int is_phys_addr);

/**
* @brief 获取共享内存信息
* @param [out] info 内存信息。其中包括内存的总量以及可用量等相关信息。
* @retval 0 成功
* @retval -1 失败
* @see swpa_mem.h
*/
int swpa_sharedmem_getinfo(SWPA_MEM_INFO* info);

/**
* @brief 检测共享内存状态
* @retval 0 正常
* @retval 1 异常，数据链表异常。
* @see swpa_mem.h
*/
int swpa_sharedmem_check();

/* BEGIN: Modified by huangdch, 2015/8/21 
*系统普通共享内存统一操作接口*/

/*****************************************************************************
 函 数 名  : swpa_normal_sharedmem_init
 功能描述  : 普通共享内存初始化接口
 输入参数  : void  
 输出参数  : 无
 返 回 值  : int
 调用函数  : 
 注意事项  : 目前无实际操作
 
 修改历史      :
  1.日    期   : 2015年8月28日
	作    者   : huangdch
	修改内容   : 新生成函数
*****************************************************************************/
int swpa_normal_sharedmem_init(void);

/*****************************************************************************
 函 数 名  : swpa_normal_sharedmem_uninit
 功能描述  : 反初始化共享内存
 输入参数  : void  
 输出参数  : 无
 返 回 值  : int
 调用函数  : 
 注意事项  : 目前无实际操作
 
 修改历史      :
  1.日    期   : 2015年8月28日
	作    者   : huangdch
	修改内容   : 新生成函数
*****************************************************************************/
int swpa_normal_sharedmem_uninit(void);

/*****************************************************************************
 函 数 名  : swpa_normal_sharedmem_alloc
 功能描述  : 普通共享内存分配接口
 输入参数  : const int id  共享内存分配标识  
			 const int size  分配内存的大小
			 int* state -1表示如果分配的内存id不存在,直接返回失败
			 			0 表示如果分配的内存id不存在,就重新分配并返回
 输出参数  : int* state 
 						小于 0表示内存分配失败
 						大于 0表示内存id已经被其它进程分配,对应的值为已经分配的内存大小
 						等于 0表示内存id是第一次分配的,分配内存的大小为输入的size
 返 回 值  : void*  	非NULL表示分配成功,NULL表示分配失败
 调用函数  : 
 注意事项  : *sate可以作为输入和输出值
 
 修改历史      :
  1.日    期   : 2015年8月28日
	作    者   : huangdch
	修改内容   : 新生成函数
*****************************************************************************/
void* swpa_normal_sharedmem_alloc(const int id, const int size, int* state);

/*****************************************************************************
 函 数 名  : swpa_normal_sharedmem_free
 功能描述  : 普通共享内存的释放
 输入参数  : const int id 内存分配的id
			 void* pAdd swpa_normal_sharedmem_alloc()返回的地址
 输出参数  : 无
 返 回 值  : int 0释放成功 小于0释放失败 大于0表示共享内存被其它进程占用
 调用函数  : 
 注意事项  : 调用者要保证id和pAdd为分配的原始值，否则执行结果可能会异常异常或内存释放失败
 			 如果共享内存被其它进程占用,释放失败,同时返回占用的进程数
 
 修改历史      :
  1.日    期   : 2015年8月28日
	作    者   : huangdch
	修改内容   : 新生成函数
*****************************************************************************/
int swpa_normal_sharedmem_free(const int id, void* pAdd);


/**
* @brief 内存拷贝操作
*/
#define swpa_memcpy		memcpy
/**
* @brief 内存数据格式化
*/
#define swpa_memset		memset

/* 
* @brief 内存数据对比
*/
#define swpa_memcmp		memcmp

#define swpa_memmove  memmove

#ifdef __cplusplus
}
#endif

#endif

