/**
 * @file   drv_vpif.h
 * @author lijj
 * @date   2013-02-18
 *
 * @brief
 *         VPIF图像采集模块　金星平台保留此文件接口
 * @note
 *         1、使用VPIF模块之前<b>必须先初始化FPGA</b>！！
 *         2、get的地址和put的地址可以不一致，地址的正确性由用户保证。\n
 *         3、采集缓冲区最大为32个，至少3个。\n
 *         4、采集图像的格式：YUV422SP，图像大小：w*h*2。\n
 *         5、采集模型：\n
 *         开始：drv_capture_init -> drv_capture_put_buffer->drv_capture_start \n
 *         循环采集：while (1) { get_buffer -> process frame -> put_buffer } \n
 *         结束：drv_capture_stop -> drv_capture_release \n
 * @log
 *      2013-04-15 机制调整，改为非阻塞式访问。
 */

#ifndef DRV_VPIF_H
#define DRV_VPIF_H

#ifdef __cplusplus
extern "C"
{
#endif

/// 采集图像结构体
/**
@note
此结构体是为了方便DM6467平台共享内存的使用，
当仅使用ARM内存进行采集时(即由内核分配内存)，不用理会phys_addr。
**/
typedef struct _V4L2_BUFFER
{
    void* virt_addr;     /**< 虚拟地址 */
    void* phys_addr;     /**< 物理地址 */
    int len;            /**< 图像大小 */

#ifdef __cplusplus
    _V4L2_BUFFER()
    {
        virt_addr = NULL;
        phys_addr = NULL;
        len = 0;
    };
#endif

} V4L2_BUFFER;

/**
 * 初始化VPIF，包括打开设备、设置视频格式
 *
 * @param is_user_mem [in] : 是使用用户分配的内存，还是内核自动分配(1：用户内存，即共享内存 0：内核自动分配)
 * @param width [out]      : 图像宽
 * @param height [out]     : 图像高
 *
 * @return 成功：0  失败：-1 参数非法：-2
 *
 * @note
 *	       1、如果为用户分配内存，在drv_capture_start之前需要先通过drv_capture_put_buffer提供分配好的内存\n
 *         宽、高为传出参数，从内核获取。\n
 *         2、访问方式为非阻塞。
 *         3、在调用VPIF模块函数前(包括本函数)，必须先加载FPGA！！！
 *
 * @sa drv_capture_start drv_capture_put_buffer
 */
int drv_capture_init(int is_user_mem, int* width, int* height);

/**
 * 开始采集
 *
 * @return 成功：0  失败：-1
 *
 * @note
 *
 */
int drv_capture_start(void);

/**
 * 停止采集
 *
 * @return 成功：0  失败：-1
 *
 */
int drv_capture_stop(void);

/**
 * 获取图像数据
 *
 * @param img_buffer [out] : 图像数据结构体
 *
 * @return 成功：0  失败：-1 参数非法：-2 超时：-6
 *
 * @note
 *        1、本函数返回失败时，img_buffer是未定义的\n
 *        2、当获取不到图像时，本函数最多尝试15次(共15*10ms = 150ms)，后返回-6
 */
int drv_capture_get_buffer(V4L2_BUFFER* img_buffer);

/**
 * 放回图像数据
 *
 * @param img_buffer [in] : 图像数据结构体
 *
 * @return 成功：0  失败：-1 参数非法：-2 超时：-6
 *
 * @note
 *         1、本函数有2个作用：申请内存、放回图像数据\n
 *         申请内存：需要循环调用本函数，如要分配5个缓冲区，则要调用5次本函数，\n
 *         如果使用内核内存，img_buffer可为NULL
 *         放回图像：img_buffer可以与drv_capture_get_buffer获取的地址不一致。
 *         2、当放回图像数据超时时，本函数最多尝试15次(共15*10ms = 150ms)，后返回-6
 *
 */
int drv_capture_put_buffer(V4L2_BUFFER* img_buffer);

/**
 * 释放资源，关闭设备
 *
 * @return 成功：0  失败：-1
 *
 */
int drv_capture_release(void);

#ifdef __cplusplus
}
#endif

#endif /* DRV_VPIF_H */
