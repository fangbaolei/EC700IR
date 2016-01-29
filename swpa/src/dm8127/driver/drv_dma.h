/**
 * @file   drv_dma.h
 * @author lijj
 * @date   2013-02-06
 *
 * @brief
 *         DMA API　金星平台保留此文件接口
 *
 * @log
 *        2013-04-07 lijj：完善2个函数的注释
 *
 */
 
#ifndef DRV_DMA_H
#define DRV_DMA_H

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * DMA 1维拷贝
 *
 * @param dst_phys [in] : 目标物理地址
 * @param src_phys [in] : 源物理地址
 * @param size [in]     : 数据大小
 *
 * @return 成功：0  失败：-1 参数非法：-2
 * @note
 *        !!! 调用本函数(1维拷贝)缓冲区长度合法范围：1~65535字节，这是由硬件特性决定的。
 *        如缓冲区较大，请使用2维拷贝 !!!
 */
int drv_dma_copy_1d(unsigned char* dst_phys, unsigned char* src_phys, unsigned int size);

/**
 * DMA 2维拷贝
 *
 * @param dst_phys [in]   : 目标物理地址
 * @param dst_stride [in] : 跨度
 * @param src_phys [in]   : 源物理地址
 * @param src_stride [in] : 跨度
 * @param width [in]      : 宽
 * @param height [in]     : 高
 *
 * @return 成功：0  失败：-1 参数非法：-2
 *
 * @note
 *        宽、高范围：1~65535
 */
int drv_dma_copy_2d(unsigned char* dst_phys, unsigned int dst_stride,
                    unsigned char* src_phys, unsigned int src_stride,
                    unsigned int width, unsigned int height);

#ifdef __cplusplus
}
#endif

#endif /* DRV_DMA_H */
