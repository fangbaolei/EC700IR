/**
 * @file   drv_dsplink.h
 * @author lijj
 * @date   2013-02-19
 *
 * @brief
 *         DspLink API　金星平台保留此文件接口
 *
 * @note  DSPLINK入口地址已经修改为0x9FFFF000
 * @todo
           一体机的CheckHDVICP01()函数是否需要？
 */

#ifndef DRV_DSPLINK_H
#define DRV_DSPLINK_H

#ifdef __cplusplus
extern "C"
{
#endif
 
/**
 * 初始化DSP环境并将可执行程序加载到DSP上运行
 *
 * @param dsp_img [in] : DSP镜像缓冲区指针(并非DSP文件)
 * @param len [in]     : 缓冲区大小
 *
 * @return 成功：0 失败：-1
 *
 * @note
 *         这里的dsp_img是解密后的dsp程序，如dsp程序是加密过的，则需自行解密
 */
int drv_dsplink_setup(unsigned char* dsp_img, int len);

/**
 * 调用DSP处理
 *
 * @param phy_addr [in/out] : 输入/输出数据的物理地址
 * @param timeout_ms [in]   : 超时时间(单位为毫秒)
 *
 * @note
 *         1、本函数只负责在ARM与DSP之间传递phy_addr，不涉及其协议机制。\n
 *         2、多线程情况下，此函数需要由<b>调用者</b>保证原子操作，\n
 *            以便每个DSP处理对应ARM端的处理
 * @return 成功：0  失败：-1 参数非法：-2
 */
int drv_dsplink_doprocess(unsigned int phy_addr, int timeout_ms);

/**
 * 退出DSPLink
 *
 * @return 成功：0 失败：-1
 */
int drv_dsplink_exit(void);

#ifdef __cplusplus
}
#endif

#endif /* DRV_DSPLINK_H */
