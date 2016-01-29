#ifndef _SWIMAGEBASE_H_
#define _SWIMAGEBASE_H_

#include "SWBaseType.h"

/**
 * @brief 图像类型枚举
 */
typedef enum
{
	SW_IMAGE_YUV_422 = 0,
	SW_IMAGE_YUV_411,
	SW_IMAGE_CbYCrY,
	SW_IMAGE_YCbYCr,
	SW_IMAGE_BGR,
	SW_IMAGE_GRAY,
	SW_IMAGE_HSV,
	SW_IMAGE_HSB,
	SW_IMAGE_BIN,
	SW_IMAGE_JPEG,              // jpeg压缩类型
	SW_IMAGE_BT1120,            // BT1120类型，排列方式：YYYY....UVUV....
	SW_IMAGE_RAW12,             // Bayer12原始数据类型
	SW_IMAGE_BT1120_ROTATE_Y,   // Y逆时针旋转90度并抽取奇数场；UV保持不变。
	SW_IMAGE_BT1120_UV,         // 对BT1120的交织型UV进行分离后放在第三个段的类型
	SW_IMAGE_H264,              // h.264压缩类型
	SW_IMAGE_H264_HISTORY,              // h.264压缩类型
	SW_IMAGE_BT1120_FIELD,      // 对BT1120进行奇数场抽取后的类型.
	SW_IMAGE_YUV420SP,
	SW_IMAGE_TYPE_COUNT
}
SW_IMAGE_TYPE;

/**
 * @brief 图像数据结构
 */
typedef struct _tag_component_image
{
	SW_IMAGE_TYPE  cImageType;		// 图像类型
	PBYTE rgpbData[3];	    // 图像内存数据
	DWORD phys[3];          // 图像物理地址
	DWORD len[3];           // 每个分量的图像大小
	INT rgiStrideWidth[3];  // 图像存储宽度(以字节为单位).
	INT iWidth;					    // 图像实际宽度
	INT iHeight;				    // 图像实际高度
	INT iSize;						  // 图像大小
} SW_COMPONENT_IMAGE;

/*
//-=- 兼容结构定义
typedef struct _DSPLinkBuffer_
{
	void *addr; // virtual address for linux(arm)
	unsigned int phys; // physics address for dsp
	unsigned int len;  // len of the buffer
}DSPLinkBuffer;

typedef enum
{
	HV_IMAGE_YUV_422 = 0,
	HV_IMAGE_YUV_411,
	HV_IMAGE_CbYCrY,
	HV_IMAGE_YCbYCr,
	HV_IMAGE_BGR,
	HV_IMAGE_GRAY,
	HV_IMAGE_HSV,
	HV_IMAGE_HSB,
	HV_IMAGE_BIN,
	HV_IMAGE_JPEG,              // jpeg压缩类型
	HV_IMAGE_BT1120,            // BT1120类型，排列方式：YYYY....UVUV....
	HV_IMAGE_RAW12,             // Bayer12原始数据类型
	HV_IMAGE_BT1120_ROTATE_Y,   // Y逆时针旋转90度并抽取奇数场；UV保持不变。
	HV_IMAGE_BT1120_UV,         // 对BT1120的交织型UV进行分离后放在第三个段的类型
	HV_IMAGE_H264,              // h.264压缩类型
	HV_IMAGE_BT1120_FIELD,      // 对BT1120进行奇数场抽取后的类型
	HV_IMAGE_TYPE_COUNT
} HV_IMAGE_TYPE;

typedef struct _HV_COMPONENT_IMAGE
{
	DSPLinkBuffer rgImageData[3];   // 图象数据
	int iStrideWidth[3];		    // 图象存储宽度(以字节为单位).
	int iHeight;				    // 图象高度
	int iWidth;					    // 图象实际宽度
	HV_IMAGE_TYPE nImgType;		    // 图象类型
	int iMemPos;				    // 图像所在内存位置 0:片外内存 1:片内内存 2:共享内存（DM6467）
} HV_COMPONENT_IMAGE;
//===========
*/

#endif

