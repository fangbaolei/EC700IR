#ifndef _HVDEVICECOMMDEF_H__
#define _HVDEVICECOMMDEF_H__

/* 图片类型 */
#define IMAGE_TYPE_UNKNOWN          0xffff0100	/**< 未知 */
#define IMAGE_TYPE_JPEG_NORMAL      0xffff0101	/**< Jpeg普通图 */
#define IMAGE_TYPE_JPEG_CAPTURE     0xffff0102	/**< Jpeg抓拍图 */
#define IMAGE_TYPE_JPEG_LPR         0xffff0103	/**< Jpeg调试码流 */

/* 视频类型 */
#define VIDEO_TYPE_UNKNOWN          0xffff0200	/**< 未知 */
#define VIDEO_TYPE_H264_NORMAL_I    0xffff0201	/**< H.264实时视频流I帧 */
#define VIDEO_TYPE_H264_NORMAL_P    0xffff0202	/**< H.264实时视频流P帧 */
#define VIDEO_TYPE_H264_HISTORY_I   0xffff0203	/**< H.264硬盘录像视频流I帧 */
#define VIDEO_TYPE_H264_HISTORY_P   0xffff0205	/**< H.264硬盘录像视频流I帧 */
#define VIDEO_TYPE_JPEG_HISTORY     0xffff0204	/**< JPEG硬盘录像视频流 */

/* 识别结果类型 */
#define RECORD_TYPE_UNKNOWN         0xffff0300	/**< 未知 */
#define RECORD_TYPE_NORMAL          0xffff0301	/**< 实时结果 */
#define RECORD_TYPE_HISTORY         0xffff0302	/**< 历史结果 */
#define RECORD_TYPE_STRING          0xffff0303	/**< 事件检测信息 */
#define RECORD_TYPE_INLEFT          0xffff0304	/**< 车牌到达离开标识 */

/* 连接状态 */
#define CONN_STATUS_UNKNOWN         0xffff0400	/**< 未知 */
#define CONN_STATUS_NORMAL          0xffff0401	/**< 正常 */
#define CONN_STATUS_DISCONN         0xffff0402	/**< 断开 */
#define CONN_STATUS_RECONN          0xffff0403	/**< 重连中 */
#define CONN_STATUS_RECVDONE        0xffff0404	/**< 历史数据接收完成 */

/* 设备类型 */
#define DEV_TYPE_UNKNOWN		0x00000000 /**< 未知设备 */
#define DEV_TYPE_HVCAM_200W		0x00000001 /**< 200万一体机 */
#define DEV_TYPE_HVCAM_500W		0x00000002 /**< 500万一体机 */
#define DEV_TYPE_HVSIGLE		0x00000004 /**< 高清2.0识别器 */
#define DEV_TYPE_HVCAM_SINGLE	0x00000008 /**< 高清2.0识别器 */
#define DEV_TYPE_ALL            0x0000ffff	/**< 所有设备 */

#endif

