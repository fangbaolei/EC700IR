#ifndef _HVDEVICECOMMDEF_H
#define _HVDEVICECOMMDEF_H

//pic TYPE
#define IMAGE_TYPE_UNKNOWN                  0xffff0100
#define IMAGE_TYPE_JPEG_NORMAL            0xffff0101
#define IMAGE_TYPE_JPEG_CAPTURE          0xffff0102
#define IMAGE_TYPE_JPEG_LPR                    0xffff0103

//video type
#define VIDEO_TYPE_UNKNOWN                  0xffff0200
#define VIDEO_TYPE_H264_NORMAL_I          0xffff0201
#define VIDEO_TYPE_H264_NORMAL_P        0xffff0202
#define VIDEO_TYPE_H264_HISTORY_I        0xffff0203
#define VIDEO_TYPE_H264_HISTORY_P       0xffff0205
//#define VIDEO_TYPE_JPEG_HISTROY          0xffff0204
#define VIDEO_TYPE_JPEG_HISTORY         0xffff0204
//record type
#define RECORD_TYPE_UNKNOWN             0xffff0300
#define RECORD_TYPE_NORMAL                 0xffff0301
#define RECORD_TYPE_HISTORY                0xffff0302
#define RECORD_TYPE_STRING                  0xffff0303
#define RECORD_TYPE_INLEFT                    0xffff0304

//connect status
#define CONN_STATUS_UNKNOWN             0xffff0400
#define CONN_STATUS_NORMAL                 0xffff0401
#define CONN_STATUS_DISCONN               0xffff0402
#define CONN_STATUS_RECONN                 0xffff0403
#define CONN_STATUS_RECVDONE            0xffff0404

//device TYPE
#define DEV_TYPE_UNKNOWN                        0x00000000
#define DEV_TYPE_HVCAM_200W                    0x00000001
#define DEV_TYPE_HVCAM_500W                    0x00000002
#define DEV_TYPE_HVSIGLE                           0x00000004
#define DEV_TYPE_HVCAM_SINGLE               0x00000008
#define DEV_TYPE_HVCAMERA                       0x00000010
#define DEV_TYPE_ALL                                     0x0000ffff

#define INVALID_SOCKET (int)(~0)

#define PLATE_RECT_BEST_SNAPSHOT           0x0001        /**< 最清晰识别图车牌坐标 */
#define PLATE_RECT_LAST_SNAPSHOT           0x0002        /**< 最后识别图车牌坐标 */
#define PLATE_RECT_BEGIN_CAPTURE           0x0003        /**< 开始抓拍图车牌坐标 */
#define PLATE_RECT_BEST_CAPTURE            0x0004        /**< 最清晰抓拍图车牌坐标 */
#define PLATE_RECT_LAST_CAPTURE            0x0005        /**< 最后抓拍图车牌坐标 */

#define FACE_RECT_BEST_SNAPSHOT            0x0006        /**< 最清晰识别图人脸信息 */
#define FACE_RECT_LAST_SNAPSHOT            0x0007        /**< 最后识别图人脸信息 */
#define FACE_RECT_BEGIN_CAPTURE            0x0008        /**< 开始抓拍图人脸信息 */
#define FACE_RECT_BEST_CAPTURE             0x0009        /**< 最清晰抓拍图人脸信息 */
#define FACE_RECT_LAST_CAPTURE             0x000A        /**< 最后抓拍图人脸信息 */

/* 通讯协议版本号 */
typedef enum _PROTOCOL_VERSION
{
	PROTOCOL_VERSION_1,
	PROTOCOL_VERSION_2,
	PROTOCOL_VERSION_MERCURY,
	PROTOCOL_VERSION_EARTH
} PROTOCOL_VERSION;




#endif // _HVDEVICECOMMDEF_H
