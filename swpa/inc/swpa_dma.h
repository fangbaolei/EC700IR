/**  
* @file  swpa_dma.h   
* @brief  dma相关接口
*
* @copyright Signalway All Rights Reserved
* @author zhouy
* @date 2013-02-27
* @version 1.0
*/ 


#ifndef _SWPA_DMA_H_
#define _SWPA_DMA_H_

#ifdef __cplusplus
extern "C"
{
#endif



/**
* @brief DMA一维拷贝\n
*
* 
* @param [in] src_phys_addr : 源数据块所在物理地址
* @param [in] dst_phys_addr : 目的数据块所在物理地址
* @param [in] size : 拷贝的数据长度
* @retval 0 : 表示拷贝成功
* @retval 其他 : 表示拷贝失败
* @note .
* @see .
*/
extern int swpa_dma_copy_1d(
	unsigned char * src_phys_addr, 
	unsigned char * dst_phys_addr, 	
	int size
); 



/**
* @brief DMA二维拷贝\n
*
* 
* @param [in] src_phys_addr : 源数据块所在物理地址
* @param [in] src_stride : 源数据块跨度
* @param [in] dst_phys_addr : 目的数据块所在物理地址
* @param [in] dst_stride : 目的数据块跨度
* @param [in] width : 拷贝的数据宽度
* @param [in] height : 拷贝的数据高度
* @retval 0 : 表示拷贝成功
* @retval 其他 : 表示拷贝失败
* @note .
* @see .
*/
extern int swpa_dma_copy_2d(
	unsigned char * src_phys_addr, int src_stride,
	unsigned char * dst_phys_addr, int dst_stride,
	int width, int height
);


#ifdef __cplusplus
}
#endif

#endif // _SWPA_DMA_H_

