/**
 * @file   drv_cmem.h
 * @author lijj
 * @date   2013-02-18
 *
 * @brief
 *         共享内存cmem相关API
 * @note   内存地址规划：

 ARM(0~96MB，共96MB):0x80000000 ~ 0x86000000
 CMEM(96MB~448MB，共352MB): 0x86000000 ~ 0x9c000000
 DSP(448MB~512MB，共64MB):0x9c000000 ~ 0x20000000

 */

#ifndef DRV_CMEM_H
#define DRV_CMEM_H

//#include <cmem.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * 初始化CMEM模块并申请共享内存
 *
 * @param virt_addr [out] : 申请到的内存虚拟地址
 * @param phys_addr [out] : 申请到的内存物理地址
 * @param len [out]       : 共享内存总长度
 *
 * @return 成功：0  失败：-1 参数非法：-2
 *
 * @note
 *         本函数只申请最大可用的共享内存，len为该内存大小，\n
 *         至于如何使用此内存，由调用者决定。
 * @sa drv_cmem_exit
 */
int drv_cmem_init(void** virt_addr, void** phys_addr, int* len);

/**
 * 释放共享内存并退出CMEM模块
 *
 * @return 成功：0  失败：-1
 *
 * @note
 *         调用本函数后，共享内存将不可用。
 * @sa drv_cmem_init
 */
int drv_cmem_exit(void);

#ifdef __cplusplus
}
#endif

#endif /* DRV_CMEM_H */
