/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
    \ingroup MCFW_API
    \defgroup MCFW_COMMON_API McFW Common Const's and Data structure's used by all McFW Sub-system's

    @{
*/

/**
    \file ti_media_common_def.h
    \brief McFW Common Const's and Data structure's used by all McFW Sub-system's
*/

#ifndef __TI_MEDIA_COMMON_DEF_H__
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define __TI_MEDIA_COMMON_DEF_H__

#include "ti_media_std.h"
#include "ti_media_error_def.h"

/* =============================================================================
 * Defines
 * =============================================================================
 */

/**
    Maximum Number of Bistream buffers in VCODEC_BITSBUF_LIST_S
*/
#define     VCODEC_BITSBUF_MAX             (64)

/**
    Maximum Number of video frame buffers in VIDEO_FRAMEBUF_LIST_S
*/
#define     VIDEO_FRAMEBUF_MAX             (16)

/**
    Maximum Number of video fields in VIDEO frame (TOP/BOTTOM)
*/
#define     VIDEO_MAX_FIELDS               (2)

/**
    Maximum Number of video planes in video frame (Y/U/V)
*/
#define     VIDEO_MAX_PLANES               (3)

/**
 * Maximum  Number of input channels to a MCFW Module
 */
#define     VIDEO_MAX_NUM_CHANNELS         (64)

/**
    Const to use when infinite timeout is needed
*/
#define     TIMEOUT_WAIT_FOREVER           ((UInt32)-1)

/**
    Const to use when NO timeout is needed
*/
#define     TIMEOUT_NO_WAIT                ((UInt32)0)

/**
    Capture Channel ID Data Type, can take values from 0..(VCAP_CHN_MAX-1)
*/
#define VCAP_CHN    UInt32

/**
    Camera Channel ID Data Type, can take values from 0..(VCAM_CHN_MAX-1)
*/
#define VCAM_CHN    UInt32

/**
    Capture Stream ID Data Type, can take values from 0..(VCAP_STRM_MAX-1)
*/
#define VCAP_STRM   UInt32

/**
    Camera Stream ID Data Type, can take values from 0..(VCAM_STRM_MAX-1)
*/
#define VCAM_STRM 	UInt32

/**
    Encode Channel ID Data Type, can take values from 0..(VENC_CHN_MAX-1)
*/
#define VENC_CHN    UInt32

/**
    Encode Stream ID Data Type, can take values from 0..(VENC_STRM_MAX-1)
*/
#define VENC_STRM   UInt32

/**
    Simcop Channel ID Data Type, can take values from 0..(MJPEG_CHN_MAX-1)
*/
#define MJPEG_CHN  UInt32

/**
    Simcop Stream ID Data Type, can take values from 0..(MJPEG_STRM_MAX-1)
*/
#define MJPEG_STRM UInt32

/**
    Decode Channel ID Data Type, can take values from 0..(VDEC_CHN_MAX-1)
*/
#define VDEC_CHN    UInt32

/**
    Display Channel ID Data Type, can take values from 0..(VDIS_CHN_MAX-1)
*/
#define VDIS_CHN    UInt32

/**
    Display Mosaic Window ID Data Type, can take values from 0..(VDIS_MOSAIC_WIN_MAX-1)
*/
#define VDIS_WIN    UInt32

/**
    Spl channel number indicating API applies to all display channels
*/
#define VDIS_CHN_ALL      (VIDEO_MAX_NUM_CHANNELS + 1)


/* =============================================================================
 * Enums
 * =============================================================================
 */

/**
    \brief Video Data Format's
*/
typedef enum VIDEO_FORMATE_E
{
    VF_YUV422I_UYVY = 0,
    /**< YUV 422 Interleaved format - UYVY. */

    VF_YUV422I_YUYV,
    /**< YUV 422 Interleaved format - YUYV. */

    VF_YUV422I_YVYU,
    /**< YUV 422 Interleaved format - YVYU. NOT USED */

    VF_YUV422I_VYUY,
    /**< YUV 422 Interleaved format - VYUY. NOT USED */

    VF_YUV422SP_UV,
    /**< YUV 422 Semi-Planar - Y separate, UV interleaved. */

    VF_YUV422SP_VU,
    /**< YUV 422 Semi-Planar - Y separate, VU interleaved. NOT USED */

    VF_YUV422P,
    /**< YUV 422 Planar - Y, U and V separate. NOT USED */

    VF_YUV420SP_UV,
    /**< YUV 420 Semi-Planar - Y separate, UV interleaved. */

    VF_YUV420SP_VU,
    /**< YUV 420 Semi-Planar - Y separate, VU interleaved. NOT USED */

    VF_YUV420P,
    /**< YUV 420 Planar - Y, U and V separate. NOT USED */

} VIDEO_FORMATE_E;

/**
    \brief Video Scan Type - Progressive or Interlaced or Auto-detect
*/
typedef enum  VIDEO_TYPE_E //
{
    PROGRESSIVE =0,
    /**< Progressive mode. */

    INTERLACED,
    /**< Interlaced mode. */

    VT_AUTO_DETECT
    /**< Auto Detect mode */

} VIDEO_TYPE_E;

/**
    \brief Video System Standard - NTSC or PAL or Auto-detect
*/
typedef enum  VIDEO_STANDARD_E
{
    NTSC = 0,
    /**< NTSC mode. */

    PAL,
    /**< PAL mode. */

    VS_AUTO_DETECT
    /**< Auto Detect mode */

} VIDEO_STANDARD_E;

/**
    \brief Noise Filter Setting - ON or OFF or AUTO
*/
typedef enum NF_FLAG_E
{
    NF_FALSE =0,
    /**< Noise Filter is OFF */

    NF_TRUE,
    /**< Noise Filter is ON */

    NF_AUTO,
    /**< Noise Filter is in auto-adjustment mode */

} NF_FLAG_E;

/**
    \brief Encoder is frame based (progressive) encode or field based (interlaced) encode or auto decided by framework
*/
typedef enum ENCODER_TYPE_E
{
    FIELD_BASED = 0,
    /**< Field based encoder */

    FRAME_BASED,
    /**< Frame based encoder */

    ET_AUTO,
    /**< Decide by framework based on system use-case */

} ENCODER_TYPE_E;

/**
    \brief Encoder/Decoder compression type
*/
typedef enum VCODEC_TYPE_E
{
    VCODEC_TYPE_MJPEG = 0,
    /**< MJPEG */
    VCODEC_TYPE_MPEG4 = 5,
    /**< MPEG4 */
    VCODEC_TYPE_H264BP = 7,
    VCODEC_TYPE_H264MP = 8,
    VCODEC_TYPE_H264HP = 9,
    /**< H264，区分BaseLineProfile， MainProfile，HighProfile，但都为H.264码流 */

    VCODEC_TYPE_MPEG2,
    /**< MPEG2 */

    VCODEC_TYPE_MAX,
    /**< Max Codec Type */

} VCODEC_TYPE_E;

/**
    \brief Encoded frame type
*/
typedef enum {

    VCODEC_FRAME_TYPE_I_FRAME,
    /**< I-frame or Key Frame */

    VCODEC_FRAME_TYPE_P_FRAME,
    /**< P-frame */

    VCODEC_FRAME_TYPE_B_FRAME,
    /**< B-frame */

} VCODEC_FRAME_TYPE_E;

/**
    \brief Encoded frame type
*/
typedef enum {

    VIDEO_FID_TYPE_TOP_FIELD,
    /**< Top field of video frame */

    VIDEO_FID_TYPE_BOT_FIELD,
    /**< Bot field of video frame */

    VIDEO_FID_TYPE_FRAME = VIDEO_FID_TYPE_TOP_FIELD,
    /**< Progressive frame.
     *   Progressive frame is assigned the same FID
     *   as top field (0) which is waht is expected
     *   by low level drivers
     **/
} VIDEO_FID_TYPE_E;

/* =============================================================================
 * Structures
 * =============================================================================
 */


/**
    \brief Window Position Data structure

    Used to specific only position of a window without width and height
*/
typedef struct POSITION_S
{
    UInt32 start_X;
    /**< Horizontal offset in pixels, from which picture needs to be positioned. */

    UInt32 start_Y;
    /**< Vertical offset from which picture needs to be positioned. */

} POSITION_S;

/**
    \brief Video Bitstream Buffer Information

    This structure is used to get information of encoded frames from VENC
    OR to pass information of encoded frames to VDEC
*/
typedef struct {
    UInt32              reserved; /**< 保留值，内部使用，应用程序不能修改此值 */
    VENC_CHN            chnId; /**< 编码/解码器通道ID，对应输入通道ID 0..(VENC_CHN_MAX-1)  */
    VENC_STRM           strmId; /**< Encoder stream ID, not valid for decoder, 0..(VENC_STRM_MAX-1)，用户应不关心  */
    VCODEC_TYPE_E       codecType;/**< 图像编码格式 */
    VCODEC_FRAME_TYPE_E frameType;/**< 编码帧类型 */
    UInt32              bufSize; /**< 图像缓冲区大小 */
    UInt32		        startOffset; /**< 有效数据在图像缓冲区的起始偏移量 in byte */
    UInt32              filledBufSize; /**< 有效数据占图像缓冲区的大小 */
    UInt32              mvDataOffset; /**< MV数据在图像缓冲区的起始偏移量 in byte */
    UInt32              mvDataFilledSize; /**< MV数据占图像缓冲区的大小 */
    Void               *bufVirtAddr; /**< 图像缓冲区虚拟地址指针 can be used with write() or fwrite() APIs */
    Void               *bufPhysAddr; /**< 图像缓冲区物理地址指针, can be used with EDMA APIs */
    UInt32              timestamp; /**< 原始图像捕获时标, in msec */
    UInt32              temporalId; /**< SVC TemporalId， 用户应不关心 */
    UInt32              upperTimeStamp; /**< Original Capture time stamp:Upper 32 bit value，用户应不关心 */
    UInt32              lowerTimeStamp; /**< Original Capture time stamp:Lower 32 bit value，用户应不关心 */
    UInt32              encodeTimestamp; /**< 编码完成时的时标 */
    UInt32              fieldId; /**< 场ID，0: Even field or Frame based, 1: Odd Field */
    UInt32              frameWidth; /**< 图像宽度, in pixels */
    UInt32              frameHeight; /**< 图像高度, in lines */
    UInt32              doNotDisplay;
    /**< Flag indicating frame should not be displayed
     *   This is useful when display should start from a
     *   particular frame.
     *   This is temporary until Avsync suuports seek functionality，用户应不关心 */
     UInt32 bottomFieldBitBufSize;
     /**< Size of the bottom field Bitstream. Filled by field Merged
      *   interlaced encoders，用户应不关心     */
} VCODEC_BITSBUF_S;

/**
    \brief Video Bitstream Buffer List

    List of Video Bitstream Buffer's allows user to efficient exchange
    multiple frames with VENC, VDEC sub-system with less overhead
*/
typedef struct {
    UInt32              numBufs; /**< bitsBuf[]中的有效帧数. 必须 <= VCODEC_BITSBUF_MAX */
    VCODEC_BITSBUF_S    bitsBuf[VCODEC_BITSBUF_MAX]; /**< 比特流信息列表 */
} VCODEC_BITSBUF_LIST_S;

/// 图像信息(最后一行)结构体
#define IMAGE_INFO_ENTRY(name) int name;
// int dummy_##name;

typedef struct IPC_CAPTURE_IMAGE_INFO_Y
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
    IMAGE_INFO_ENTRY(rsv_0)
    IMAGE_INFO_ENTRY(rsv_1)
    IMAGE_INFO_ENTRY(rsv_2)
    IMAGE_INFO_ENTRY(rsv_3)
    IMAGE_INFO_ENTRY(capture_edge_ext)
	IMAGE_INFO_ENTRY(Sum_R_raw)
	IMAGE_INFO_ENTRY(Sum_G_raw)
	IMAGE_INFO_ENTRY(Sum_B_raw)
} IPC_CAPTURE_IMAGE_INFO_Y;

/**
    \brief Video Frame Buffer Information

    This structure is used to get information of video frames from VCAP
    OR to pass information of video frames to VDIS
*/
typedef struct {
    Ptr    addr[VIDEO_MAX_FIELDS][VIDEO_MAX_PLANES]; /**< 虚拟地址指针，分为2场，3Plane */
    Ptr    phyAddr[VIDEO_MAX_FIELDS][VIDEO_MAX_PLANES]; /**< 物理地址指针 ，分为2场，3Plane */
    VCAP_CHN channelNum; /**< 通道ID */
    UInt32 timeStamp; /**< 捕获时标 */
    UInt32 fid; /**< 场标记 (TOP/BOTTOM/FRAME) */
    VIDEO_FORMATE_E dataFormat; /**< 图像类型 */
    UInt32 frameWidth; /**< 帧宽 */
    UInt32 frameHeight; /**< 帧高 */
    UInt32 startX; /**< 有效数据起始列数 */
    UInt32 startY; /**< 有效数据起始行数 */
    UInt32 framePitch[VIDEO_MAX_PLANES]; /**< 帧跨度 */
    Ptr    linkPrivate; /**< Link私有数据，保留必须应用程序不能修改此值 */
    Void* pImageSpecDataPhyAddr;    /**< 图像属性数据物理地址 */
    Void* pImageSpecDataVirAddr;    /**< 图像属性数据虚拟地址 */
    UInt32 nImageSpecDataSize;      /**< 图像属性数据区域大小 */
    Void* pUserSpecDataPhyAddr;     /**< 用户自定义数据物理地址 */
    Void* pUserSpecDataVirAddr;     /**< 用户自定义数据虚拟地址 */
    UInt32 nUserSpecDataSize;       /**< 用户自定义数据区域大小 */
} VIDEO_FRAMEBUF_S;

/**
    \brief Video Bitstream Buffer List

    List of Video Bitstream Buffer's allows user to efficient exchange
    multiple frames with VENC, VDEC sub-system with less overhead
*/
typedef struct {
    UInt32              numFrames; /**< frames[] 中的有效帧数. 必须 <= VIDEO_FRAMEBUF_MAX */
    VIDEO_FRAMEBUF_S    frames[VIDEO_FRAMEBUF_MAX]; /**< 视频帧信息列表 */
} VIDEO_FRAMEBUF_LIST_S;


/**
    \brief CHannel specific video info

    Provides channel specific info for each channel. This is used
    by the application to query a module input channel information
*/
typedef struct  VIDEO_CHANNEL_INFO_S {

    UInt32  width;
    UInt32  height;
} VIDEO_CHANNEL_INFO_S;

/**
    \brief Channel info structure

    This structure provides info on the number of channels and
    each channels input frame info such as width/height etc.
*/
typedef struct VIDEO_CHANNEL_LIST_INFO_S {

    UInt32              numCh;
    /**< Number of input channels processed by a MCFW module */

    VIDEO_CHANNEL_INFO_S  chInfo[VIDEO_MAX_NUM_CHANNELS];
    /**< Channel specific information list */

} VIDEO_CHANNEL_LIST_INFO_S;

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

/* @} */
