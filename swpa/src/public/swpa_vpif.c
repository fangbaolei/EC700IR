/**  
* @file  swpa_vpif.h   
* @brief  抓拍设备相关
* @author luoqz@signalway.com.cn
* @date  2013-3-22
* @version  V0.1
* @note     
*/ 

#include "drv_vpif.h"
#include "swpa_vpif.h"
#include "swpa_private.h"
#include "swpa_list.h"
#include "swpa_camera.h"

#ifdef SWPA_VPIF
#define SWPA_VPIF_PRINT(fmt, ...) SWPA_PRINT("[%s:%d]"fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#define SWPA_VPIF_CHECK(arg)      {if (!(arg)){SWPA_PRINT("[%s:%d]Check failed : %s [%d]\n", __FILE__, __LINE__, #arg, SWPAR_INVALIDARG);return SWPAR_INVALIDARG;}}
#else
#define SWPA_VPIF_PRINT(fmt, ...)
#define SWPA_VPIF_CHECK(arg)
#endif


static int swpa_vpif_width = 0;                    /** 图像宽度 */
static int swpa_vpif_height = 0;                   /** 图像高度 */
static int swpa_vpif_initialized = 0;              /** 此模块是否已初始化 */
static int swpa_vpif_y_sum_rows = 1;               /** 通道边界相邻像素Y值求和的行（列）数 */
static int swpa_vpif_ad_channels = 0;              /** AD通道数 */

static int swpa_vpif_imginfo_callback_list = 0;

int swpa_capture_init(
	int is_user_mem, 
	int* width, 
	int* height
)
{
	int result = 0;
	int max_shutter = 0;
	int max_gain = 0;
	
	SWPA_VPIF_PRINT("is_user_mem=%d\n", is_user_mem);
	SWPA_VPIF_CHECK(is_user_mem >= 0 && is_user_mem <= 1);
	SWPA_VPIF_CHECK(width != 0);
	SWPA_VPIF_CHECK(height != 0);
	SWPA_VPIF_CHECK(swpa_vpif_initialized == 0);

	result = drv_capture_init(is_user_mem, width, height);
	if (0 != result)
	{
		SWPA_VPIF_PRINT("result=%d\n", result);
		return SWPAR_FAIL;
	}
	
	swpa_vpif_width = *width;
	swpa_vpif_height = *height;
	
	// 获取AD通道数
	result = swpa_camera_init_get_device_dependent_info(&max_shutter, &max_gain, &swpa_vpif_ad_channels);
	if (-1 == result)
	{
		SWPA_VPIF_PRINT("result=%d\n", result);
		return SWPAR_FAIL;
	}
	
	swpa_vpif_initialized = 1;
	
	return SWPAR_OK;
}

int swpa_capture_start(void)
{
	SWPA_VPIF_CHECK(swpa_vpif_initialized == 1);

	return drv_capture_start();
}

int swpa_capture_stop(void)
{
	SWPA_VPIF_CHECK(swpa_vpif_initialized == 1);

	// todo. 
	return 0;
	//return drv_capture_stop();
}

int swpa_capture_get_buffer(
	CAPTURE_BUFFER* img_buffer,
	CAPTURE_IMAGE_INFO_Y ** img_info_y,
	CAPTURE_IMAGE_INFO_C ** img_info_c
)
{
	int result = 0;
	V4L2_BUFFER drv_buffer;
	int sum_y1 = 10, sum_y2 = 10, sum_y3 = 0, sum_y4 = 0;
	int sum_y5 = 0, sum_y6 = 0, sum_y7 = 0, sum_y8 = 0;
	int list_index = -1;
	int list_size = 0;
	int i = 0;
	SWPA_VPIF_IMGINFO_CALLBACK list_element = 0;
	int half_width = swpa_vpif_width / 2;
	int half_height = swpa_vpif_height / 2;
	unsigned char *y_index1 = 0, *y_index2 = 0, *y_index3 = 0, *y_index4 = 0;
	unsigned char *y_index5 = 0, *y_index6 = 0, *y_index7 = 0, *y_index8 = 0;
	float fDiff = 0.0;
	
	SWPA_VPIF_CHECK(swpa_vpif_initialized == 1);
	SWPA_VPIF_CHECK(img_buffer != 0);
	SWPA_VPIF_CHECK(img_info_y != 0);
	SWPA_VPIF_CHECK(img_info_c != 0);
	
	SWPA_VPIF_PRINT("Before drv_capture_get_buffer\n");
	result = drv_capture_get_buffer(&drv_buffer);
	if (0 != result)
	{
		SWPA_VPIF_PRINT("result=%d\n", result);
		return SWPAR_FAIL;
	}
	SWPA_VPIF_PRINT("After drv_capture_get_buffer\n");

	img_buffer->virt_addr = drv_buffer.virt_addr;
	img_buffer->phys_addr = drv_buffer.phys_addr;
	img_buffer->len = drv_buffer.len;
	
	SWPA_VPIF_CHECK(img_buffer->len >= swpa_vpif_width * swpa_vpif_height * 2);
	
	// 计算BT1120图像信息偏移量
	*img_info_y = (CAPTURE_IMAGE_INFO_Y*)(drv_buffer.virt_addr + swpa_vpif_width * swpa_vpif_height - swpa_vpif_width);
	*img_info_c = (CAPTURE_IMAGE_INFO_C*)(drv_buffer.virt_addr + swpa_vpif_width * swpa_vpif_height * 2 - swpa_vpif_width);
	
	// 调用回调函数
	SWPA_VPIF_PRINT("Before swpa_list_open\n");
	list_index = swpa_list_open(swpa_vpif_imginfo_callback_list, 0);
	if (-1 != list_index) // -1表示空列表
	{
		// 计算通道边界相邻像素Y值和
		SWPA_VPIF_PRINT("Before Y sum\n");
		if (swpa_vpif_ad_channels == 2)
		{

			if (swpa_vpif_y_sum_rows == 1)
			{
				y_index1 = (unsigned char*)(img_buffer->virt_addr) + half_width - 1;
				for (i = 0; i < swpa_vpif_height; i++)
				{
						sum_y1 += y_index1[0];
						sum_y2 += y_index1[1];

						if( y_index1[0] > y_index1[1] )
						{
							sum_y4++;
							sum_y3 += y_index1[0];
							sum_y6 += y_index1[1];
						}
						else
						{
							sum_y5++;
							sum_y7 += y_index1[0];
							sum_y8 += y_index1[1];
						}
						y_index1 += swpa_vpif_width;
				}

				fDiff = sum_y1;
				fDiff /= swpa_vpif_height;
				if( fDiff < 30.0 )
				{
					sum_y4 = sum_y5 = 0;
				}
			}
			else if (swpa_vpif_y_sum_rows == 2)
			{
				y_index1 = (unsigned char*)(img_buffer->virt_addr) + half_width - 2;
				for (i = 0; i < swpa_vpif_height; i++)
				{
					sum_y1 += y_index1[0] + y_index1[1];
					sum_y2 += y_index1[2] + y_index1[3];
					y_index1 += swpa_vpif_width;
				}
			}
		}
		SWPA_VPIF_PRINT("After  Y sum\n");
		// 暂时把四通道的算法屏蔽掉
	//	else if (swpa_vpif_ad_channels == 4)
	//	{
	//		if (swpa_vpif_y_sum_rows == 1)
	//		{
	//			y_index1 = (unsigned char*)(img_buffer->virt_addr) + half_width - 1;
	//			y_index2 = (unsigned char*)(img_buffer->virt_addr) + swpa_vpif_width * half_height + half_width - 1;
	//			for (i = 0; i < half_height; i++)
	//			{
	//				sum_y1 += y_index1[0];
	//				sum_y2 += y_index1[1];
	//				sum_y5 += y_index2[0];
	//				sum_y6 += y_index2[1];
	//				y_index1 += swpa_vpif_width;
	//				y_index2 += swpa_vpif_width;
	//			}
	//			y_index1 = (unsigned char*)(img_buffer->virt_addr) + swpa_vpif_width * half_height - swpa_vpif_width;
	//			y_index2 = (unsigned char*)(img_buffer->virt_addr) + swpa_vpif_width * half_height - swpa_vpif_width + half_width;
	//			y_index3 = (unsigned char*)(img_buffer->virt_addr) + swpa_vpif_width * half_height;
	//			y_index4 = (unsigned char*)(img_buffer->virt_addr) + swpa_vpif_width * half_height + half_width;
	//			for (i = 0; i < half_width; i++)
	//			{
	//				sum_y3 += y_index1[i];
	//				sum_y4 += y_index2[i];
	//				sum_y7 += y_index3[i];
	//				sum_y8 += y_index4[i];
	//			}
	//		}
	//		else if (swpa_vpif_y_sum_rows == 2)
	//		{
	//			y_index1 = (unsigned char*)(img_buffer->virt_addr) + half_width - 2;
	//			y_index2 = (unsigned char*)(img_buffer->virt_addr) + swpa_vpif_width * half_height + half_width - 2;
	//			for (i = 0; i < half_height; i++)
	//			{
	//				sum_y1 += y_index1[0] + y_index1[1];
	//				sum_y2 += y_index1[2] + y_index1[3];
	//				sum_y5 += y_index2[0] + y_index2[1];
	//				sum_y6 += y_index2[2] + y_index2[3];
	//				y_index1 += swpa_vpif_width;
	//				y_index2 += swpa_vpif_width;
	//			}
	//			y_index1 = (unsigned char*)(img_buffer->virt_addr) + swpa_vpif_width * half_height - swpa_vpif_width * 2;
	//			y_index2 = (unsigned char*)(img_buffer->virt_addr) + swpa_vpif_width * half_height - swpa_vpif_width;
	//			y_index3 = (unsigned char*)(img_buffer->virt_addr) + swpa_vpif_width * half_height - swpa_vpif_width * 2 + half_width;
	//			y_index4 = (unsigned char*)(img_buffer->virt_addr) + swpa_vpif_width * half_height - swpa_vpif_width + half_width;
	//			y_index5 = (unsigned char*)(img_buffer->virt_addr) + swpa_vpif_width * half_height;
	//			y_index6 = (unsigned char*)(img_buffer->virt_addr) + swpa_vpif_width * half_height + swpa_vpif_width;
	//			y_index7 = (unsigned char*)(img_buffer->virt_addr) + swpa_vpif_width * half_height + half_width;
	//			y_index8 = (unsigned char*)(img_buffer->virt_addr) + swpa_vpif_width * half_height + swpa_vpif_width + half_width;
	//			for (i = 0; i < half_width; i++)
	//			{
	//				sum_y3 += y_index1[i] + y_index2[i];
	//				sum_y4 += y_index3[i] + y_index4[i];
	//				sum_y7 += y_index5[i] + y_index6[i];
	//				sum_y8 += y_index7[i] + y_index8[i];
	//			}
	//		}
	//	}
	
		SWPA_VPIF_PRINT("Before swpa_list_size\n");
		list_size = swpa_list_size(swpa_vpif_imginfo_callback_list);
		for (i = 0; i < list_size; i++)
		{
			list_element = (SWPA_VPIF_IMGINFO_CALLBACK)swpa_list_value(swpa_vpif_imginfo_callback_list, list_index);
			
			list_element(*img_info_y, *img_info_c, sum_y1, sum_y2, sum_y3, sum_y4, sum_y5, sum_y6, sum_y7, sum_y8);
			
			list_index = swpa_list_next(swpa_vpif_imginfo_callback_list); // 故意不判断返回值，因为最后一个元素会返回-1，由swpa_list_size保证不越界访问
		}
		SWPA_VPIF_PRINT("Before swpa_list_close\n");
		result = swpa_list_close(swpa_vpif_imginfo_callback_list);
		if (0 != result)
		{
			SWPA_VPIF_PRINT("result=%d\n", result);
			return SWPAR_FAIL;
		}
		SWPA_VPIF_PRINT("After swpa_list_close\n");
	}
	
	return SWPAR_OK;
}

int swpa_capture_put_buffer(
	CAPTURE_BUFFER* img_buffer
)
{
	V4L2_BUFFER drv_buffer;
	
	SWPA_VPIF_CHECK(swpa_vpif_initialized == 1);
	SWPA_VPIF_CHECK(img_buffer != 0);
	
	drv_buffer.virt_addr = img_buffer->virt_addr;
	drv_buffer.phys_addr = img_buffer->phys_addr;
	drv_buffer.len = img_buffer->len;
	
	return drv_capture_put_buffer(&drv_buffer);
}

int swpa_capture_release(
	void
)
{
	int result = 0;

	SWPA_VPIF_CHECK(swpa_vpif_initialized == 1);
	
	// 释放回调函数列表
	result = swpa_list_destroy(swpa_vpif_imginfo_callback_list);
	if (0 != result)
	{
		SWPA_VPIF_PRINT("result=%d\n", result);
		return SWPAR_FAIL;
	}
	swpa_vpif_imginfo_callback_list = 0;
	
	result = drv_capture_release();
	if (0 != result)
	{
		SWPA_VPIF_PRINT("result=%d\n", result);
		return SWPAR_FAIL;
	}
	
	swpa_vpif_initialized = 0;
	
	return SWPAR_OK;
}

int swpa_vpif_set_imginfo_callback(SWPA_VPIF_IMGINFO_CALLBACK func)
{
	int result = 0;
	int list_index = -1;
	int list_size = 0;
	int i = 0;
	SWPA_VPIF_IMGINFO_CALLBACK list_element = 0;

	SWPA_VPIF_CHECK(func != 0);
	
	// 创建回调函数列表
	if (swpa_vpif_imginfo_callback_list == 0)
	{
		result = swpa_list_create(0);
		if (-1 == result)
		{
			SWPA_VPIF_PRINT("result=%d\n", result);
			return SWPAR_FAIL;
		}
		else
		{
			swpa_vpif_imginfo_callback_list = result;
		}
	}
	
	// 遍历列表看是否已有此元素
	SWPA_VPIF_PRINT("Before swpa_list_open\n");
	list_index = swpa_list_open(swpa_vpif_imginfo_callback_list, 0);
	if (-1 != list_index)
	{
		SWPA_VPIF_PRINT("Before swpa_list_size\n");
		list_size = swpa_list_size(swpa_vpif_imginfo_callback_list);
		for (i = 0; i < list_size; i++)
		{
			list_element = (SWPA_VPIF_IMGINFO_CALLBACK)swpa_list_value(swpa_vpif_imginfo_callback_list, list_index);
			if (list_element == func)
			{
				break;
			}
			list_index = swpa_list_next(swpa_vpif_imginfo_callback_list); // 故意不判断返回值，因为最后一个元素会返回-1，由swpa_list_size保证不越界访问
		}
		SWPA_VPIF_PRINT("Before swpa_list_close\n");
		result = swpa_list_close(swpa_vpif_imginfo_callback_list);
		if (0 != result)
		{
			SWPA_VPIF_PRINT("result=%d\n", result);
			return SWPAR_FAIL;
		}
		SWPA_VPIF_PRINT("After swpa_list_close\n");
	}
	if (list_index == -1) // 列表为空或没有找到与func相同的元素
	{
		SWPA_VPIF_PRINT("Before swpa_list_add\n");
		result = swpa_list_add(swpa_vpif_imginfo_callback_list, func);
		SWPA_VPIF_PRINT("After swpa_list_add\n");
		if (-1 == result)
		{
			SWPA_VPIF_PRINT("result=%d\n", result);
			return SWPAR_FAIL;
		}
	}
	
	return SWPAR_OK;
}

int swpa_vpif_remove_imginfo_callback(SWPA_VPIF_IMGINFO_CALLBACK func)
{
	int result = 0;
	int list_index = -1;
	int list_size = 0;
	int i = 0;
	SWPA_VPIF_IMGINFO_CALLBACK list_element = 0;
	
	SWPA_VPIF_CHECK(func != 0);
	
	// 遍历列表看是否已有此元素
	list_index = swpa_list_open(swpa_vpif_imginfo_callback_list, 0);
	if (-1 != list_index)
	{
		list_size = swpa_list_size(swpa_vpif_imginfo_callback_list);
		for (i = 0; i < list_size; i++)
		{
			list_element = (SWPA_VPIF_IMGINFO_CALLBACK)swpa_list_value(swpa_vpif_imginfo_callback_list, list_index);
			if (list_element == func)
			{
				break;
			}
			list_index = swpa_list_next(swpa_vpif_imginfo_callback_list); // 故意不判断返回值，因为最后一个元素会返回-1，由swpa_list_size保证不越界访问
		}
		result = swpa_list_close(swpa_vpif_imginfo_callback_list);
		if (0 != result)
		{
			SWPA_VPIF_PRINT("result=%d\n", result);
			return SWPAR_FAIL;
		}
	}
	if (list_index != -1) // 找到与func相同的元素
	{
		result = swpa_list_delete(swpa_vpif_imginfo_callback_list, list_index);
		if (-1 == result)
		{
			SWPA_VPIF_PRINT("result=%d\n", result);
			return SWPAR_FAIL;
		}
	}
		
	return SWPAR_OK;
}

int swpa_vpif_set_y_sum_rows(
	int rows
)
{
	SWPA_VPIF_CHECK(rows >= 1 && rows <= 2);
	
	swpa_vpif_y_sum_rows = rows;
	
	return SWPAR_OK;
}

