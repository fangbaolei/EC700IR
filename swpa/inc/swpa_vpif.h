/**  
* @file  swpa_vpif.h   
* @brief  抓拍设备相关
 *         VPIF图像采集模块
 *         采集模型：
 *         开始：swpa_capture_init -> swpa_capture_put_buffer->swpa_vpif_set_imginfo_callback->swpa_capture_start \n
 *         循环采集：while (1) { get_buffer -> process frame -> put_buffer } \n
 *         结束：swpa_capture_stop -> swpa_capture_release
* @author  
* @date  2013-2-20
* @version  V0.1
* @note     
*/ 

#ifndef _SWPA_VPIF_H_
#define _SWPA_VPIF_H_

#ifdef __cplusplus
extern "C"
{
#endif

/// 图像信息结构体
#define IMAGE_INFO_ENTRY(name) int name; int dummy_##name;

typedef struct _CAPTURE_IMAGE_INFO_Y // Y通道输出的信息
{
	IMAGE_INFO_ENTRY(vgagain)
	IMAGE_INFO_ENTRY(work_mode)
	IMAGE_INFO_ENTRY(syn_ac_ext)
	IMAGE_INFO_ENTRY(shutter_v)
	IMAGE_INFO_ENTRY(r_gain_v)
	IMAGE_INFO_ENTRY(g_gain_v)
	IMAGE_INFO_ENTRY(b_gain_v)
	IMAGE_INFO_ENTRY(capture_en)
	IMAGE_INFO_ENTRY(capture_inf)
	IMAGE_INFO_ENTRY(capture_en_flag)
	IMAGE_INFO_ENTRY(shutter_c)
	IMAGE_INFO_ENTRY(r_gain_c)
	IMAGE_INFO_ENTRY(g_gain_c)
	IMAGE_INFO_ENTRY(b_gain_c)
	IMAGE_INFO_ENTRY(vga_gain_c_1)
	IMAGE_INFO_ENTRY(vga_gain_c_2)
	IMAGE_INFO_ENTRY(vga_gain_c_3)
	IMAGE_INFO_ENTRY(vga_gain_c_4)
	IMAGE_INFO_ENTRY(clamplevel_c_1)
	IMAGE_INFO_ENTRY(clamplevel_c_2)
	IMAGE_INFO_ENTRY(clamplevel_c_3)
	IMAGE_INFO_ENTRY(clamplevel_c_4)
	IMAGE_INFO_ENTRY(led_set)
	IMAGE_INFO_ENTRY(flash_set_1)
	IMAGE_INFO_ENTRY(flash_set_2)
	IMAGE_INFO_ENTRY(flash_set_3)
	IMAGE_INFO_ENTRY(aoto_iris_ctrl)
	IMAGE_INFO_ENTRY(switcher_ctrl)
	IMAGE_INFO_ENTRY(floor_lamps)
	IMAGE_INFO_ENTRY(time_cnt_out)
	IMAGE_INFO_ENTRY(time_cnt_out_vd)
	IMAGE_INFO_ENTRY(time_cap1)
	IMAGE_INFO_ENTRY(time_cap2)
	IMAGE_INFO_ENTRY(capture_flag)
	IMAGE_INFO_ENTRY(factor_r_0)
	IMAGE_INFO_ENTRY(factor_r_1)
	IMAGE_INFO_ENTRY(factor_r_2)
	IMAGE_INFO_ENTRY(factor_r_3)
	IMAGE_INFO_ENTRY(factor_g_0)
	IMAGE_INFO_ENTRY(factor_g_1)
	IMAGE_INFO_ENTRY(factor_g_2)
	IMAGE_INFO_ENTRY(factor_g_3)
	IMAGE_INFO_ENTRY(factor_b_0)
	IMAGE_INFO_ENTRY(factor_b_1)
	IMAGE_INFO_ENTRY(factor_b_2)
	IMAGE_INFO_ENTRY(factor_b_3)
	IMAGE_INFO_ENTRY(vgagain_a)
	IMAGE_INFO_ENTRY(vgagain_b)
	IMAGE_INFO_ENTRY(vgagain_c)
	IMAGE_INFO_ENTRY(vgagain_d)
	IMAGE_INFO_ENTRY(clamplevel_a)
	IMAGE_INFO_ENTRY(clamplevel_b)
	IMAGE_INFO_ENTRY(clamplevel_c)
	IMAGE_INFO_ENTRY(clamplevel_d)
} CAPTURE_IMAGE_INFO_Y;

typedef struct _CAPTURE_IMAGE_INFO_C // C通道输出的信息
{
	IMAGE_INFO_ENTRY(th_custom)
	IMAGE_INFO_ENTRY(th_count)
	IMAGE_INFO_ENTRY(gamma_data)
	IMAGE_INFO_ENTRY(gamma_addr_prt)
	IMAGE_INFO_ENTRY(addr_prt)
	IMAGE_INFO_ENTRY(cod_x_min_prt)
	IMAGE_INFO_ENTRY(cod_x_max_prt)
	IMAGE_INFO_ENTRY(cod_y_min_prt)
	IMAGE_INFO_ENTRY(cod_y_max_prt)
	IMAGE_INFO_ENTRY(sum_r)
	IMAGE_INFO_ENTRY(sum_g)
	IMAGE_INFO_ENTRY(sum_b)
	IMAGE_INFO_ENTRY(sum_y)
	IMAGE_INFO_ENTRY(sum_u)
	IMAGE_INFO_ENTRY(sum_v)
	IMAGE_INFO_ENTRY(p_count)
	IMAGE_INFO_ENTRY(avg_y_15)
	IMAGE_INFO_ENTRY(avg_y_14)
	IMAGE_INFO_ENTRY(avg_y_13)
	IMAGE_INFO_ENTRY(avg_y_12)
	IMAGE_INFO_ENTRY(avg_y_11)
	IMAGE_INFO_ENTRY(avg_y_10)
	IMAGE_INFO_ENTRY(avg_y_9)
	IMAGE_INFO_ENTRY(avg_y_8)
	IMAGE_INFO_ENTRY(avg_y_7)
	IMAGE_INFO_ENTRY(avg_y_6)
	IMAGE_INFO_ENTRY(avg_y_5)
	IMAGE_INFO_ENTRY(avg_y_4)
	IMAGE_INFO_ENTRY(avg_y_3)
	IMAGE_INFO_ENTRY(avg_y_2)
	IMAGE_INFO_ENTRY(avg_y_1)
	IMAGE_INFO_ENTRY(avg_y_0)
} CAPTURE_IMAGE_INFO_C;

/// 采集图像数据结构体
typedef struct _CAPTURE_BUFFER
{
    void* virt_addr;     /**< 虚拟地址 */
    void* phys_addr;     /**< 物理地址 */
    int len;            /**< 图像大小 */

} CAPTURE_BUFFER;

/**
 * @brief 获取bt1120附带图像信息的回调函数
 *
 * @param  [out] img_info_y : 通过BT1120的Y通道输出的图像信息
 * @param  [out] img_info_c : 通过BT1120的C通道输出的图像信息
 * @param  [out] sum_y1-sum_y8 : 通道边界相邻像素Y值和，可通过swpa_vpif_set_y_sum_rows配置求和的行列数
 *                               如果有两个通道，sum_y1对应左通道的最右边像素，sum_y2对应右通道的最左边像素，其余参数无效
 *                               如果有四个通道，sum_y1对应左上通道的最右边像素，sum_y2对应右上通道的最左边像素，
 *                                              sum_y3对应左上通道的最下边像素，sum_y4对应右上通道的最下边像素，
 *                                              sum_y5对应左下通道的最右边像素，sum_y6对应右下通道的最左边像素，
 *                                              sum_y7对应左下通道的最上边像素，sum_y8对应右下通道的最上边像素
 *
 * @note .
 * @see 《水星平台fpga寄存器输出说明文档》
 */
typedef void (*SWPA_VPIF_IMGINFO_CALLBACK)(
	CAPTURE_IMAGE_INFO_Y * img_info_y, CAPTURE_IMAGE_INFO_C * img_info_c,
	int sum_y1, int sum_y2, int sum_y3, int sum_y4, 
	int sum_y5, int sum_y6, int sum_y7, int sum_y8
);


/**
 * @brief 初始化VPIF
 *
 * @param [in] is_user_mem : 是使用用户分配的内存，还是内核自动分配(1：用户内存，即共享内存 0：内核自动分配)
 * @param [out] width      : 图像宽
 * @param [out] height     : 图像高
 * @retval 0 : 执行成功
 * @retval -1 : 执行失败
 * @retval -2 : 参数非法
 *
 * @note
 * - 如果为用户分配内存，在swpa_capture_start之前需要先通过swpa_capture_put_buffer提供分配好的内存
 *
 */
int swpa_capture_init(
	int is_user_mem, 
	int* width, 
	int* height
);


/**
 * @brief 开始采集
 *
 *
 * @retval 0 : 执行成功
 * @retval -1 : 执行失败
 */
int swpa_capture_start(void);



/**
 * @brief  停止采集
 *
 *
 * @retval 0 : 执行成功
 * @retval -1 : 执行失败
 *
 */
int swpa_capture_stop(void);


/**
 * @brief 获取图像数据和图像参数
 *
 * @param [out] img_buffer : 指向图像数据存储结构体指针的指针
 * @param [out] img_info_y : 指向图像Y通道信息存储结构体指针的指针，指向的是img_buffer内部偏移量
 * @param [out] img_info_c : 指向图像C通道信息存储结构体指针的指针，指向的是img_buffer内部偏移量
 * @retval 0 : 执行成功
 * @retval -1 : 执行失败
 * @retval -2 : 参数非法
 * @retval -6 : 操作超时
 * @note 内部会调用swpa_vpif_set_imginfo_callback注册的回调函数，需要等待该回调函数完成后才能返回
 *       非阻塞式访问，当获取不到图像时，本函数最多尝试15次(共15*10ms = 150ms)，后返回-6
 */
int swpa_capture_get_buffer(
	CAPTURE_BUFFER* img_buffer,
	CAPTURE_IMAGE_INFO_Y ** img_info_y,
	CAPTURE_IMAGE_INFO_C ** img_info_c
);



/**
 * @brief 放回图像数据
 *
 * @param  [in] img_buffer : 图像数据结构体指针
 *
 * @retval 0 : 执行成功
 * @retval -1 : 执行失败
 * @retval -2 : 参数非法
 *
 * @note
 *         1、本函数有2个作用：申请内存、放回图像数据\n
 *         申请内存需要循环调用本函数，如要分配5个缓冲区，则要调用5次本函数，\n
 *         如果使用内核内存，img_buffer可为NULL
 */
int swpa_capture_put_buffer(
	CAPTURE_BUFFER* img_buffer
);



/**
 * @brief 释放资源，关闭设备
 *
 *
 * @retval 0 : 执行成功
 * @retval -1 : 执行失败
 *
 */
int swpa_capture_release(
	void
);

/**
 * @brief 设置获取bt1120附带图像信息的回调函数
 *
 * @param  [in] func : 获取图像信息的回调函数，此回调函数将在swpa_capture_get_buffer取到新图像数据时被调用
 *
 * @retval 0 : 执行成功
 * @retval -1 : 执行失败
 * @retval -4 : 内存不足
 * @note 可以注册多个回调函数
 *       回调函数不应该占用太多时间，否则会导致get_buffer被卡住
 *       img_info_y和img_info_c指针只在回调函数内部有效，如果后续还需访问，需把数据拷贝到结构体缓存
 * @see .
 */
int swpa_vpif_set_imginfo_callback(SWPA_VPIF_IMGINFO_CALLBACK func);

/**
 * @brief 删除获取bt1120附带图像信息的回调函数
 *
 * @param  [in] func : 要删除的图像信息回调函数
 *
 * @retval 0 : 执行成功
 * @retval -1 : 执行失败
 * @note 如果原来没有设置过此func，则同样返回0
 * @see .
 */
int swpa_vpif_remove_imginfo_callback(SWPA_VPIF_IMGINFO_CALLBACK func);

/**
 * @brief 设置通道边界相邻像素Y值求和的行（列）数
 *
 * @param  [in] rows : 行列数，范围1-2
 *
 * @retval 0 : 执行成功
 * @retval -1 : 执行失败
 * @note 默认值为2
 * @see .
 */
int swpa_vpif_set_y_sum_rows(
	int rows
);

#ifdef __cplusplus
}
#endif

#endif // _SWPA_VPIF_H_


