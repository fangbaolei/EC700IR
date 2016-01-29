/**  
* @file  swpa_dma.c
* @brief  dma相关接口
* @author  
* @date  2013-2-20
* @version  V0.1
* @note     
*/ 


#include "drv_dma.h"

#ifdef SWPA_DMA
#define SWPA_DMA_PRINT(fmt, ...) SWPA_PRINT("[%s:%d]"fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#define SWPA_DMA_CHECK(arg)      {if (!(arg)){SWPA_PRINT("[%s:%d]Check failed : %s [%d]\n", __FILE__, __LINE__, #arg, SWPAR_INVALIDARG);return SWPAR_INVALIDARG;}}
#else
#define SWPA_DMA_PRINT(fmt, ...)
#define SWPA_DMA_CHECK(arg)
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
int swpa_dma_copy_1d(
	unsigned char * src_phys_addr, 
	unsigned char * dst_phys_addr, 	
	int size
)
{
	return drv_dma_copy_1d(src_phys_addr, dst_phys_addr, size);
}



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
int swpa_dma_copy_2d(
	unsigned char * src_phys_addr, 
	int src_stride,
	unsigned char * dst_phys_addr, 
	int dst_stride,
	int width, 
	int height
)
{
	return drv_dma_copy_2d(src_phys_addr, src_stride, dst_phys_addr, dst_stride, width, height);
}




