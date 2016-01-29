#ifndef _JPEGENC_H
#define _JPEGENC_H

#ifdef __cplusplus
	extern "C" {
#endif	/* #ifdef __cplusplus */

#include "dspsys_def.h"

/*
函数说明:
	对传入的原始YUV图象进行JPEG压缩处理.
	如果提供的JPEG Buf不够大，则通过pdwImgSize返回需要的大小	
函数返回值:
	返回S_OK表示JPEG压缩成功.
	返回E_POINTER表示pImage或者pbJpegImgData为INVALID_POINTER.
	返回E_INVALIDARG表示传入的参数有错.
	返回E_FAIL表示JPEG压缩失败. */
extern HRESULT Jpeg_Enc(
	HV_COMPONENT_IMAGE 	*pImage,		// 输入图像
	int 				nQuality, 		// JPEG图象质量.
	PBYTE8 				pbJpegImgData, 	// 指向JPEG压缩后的数据.
	DWORD32				dwMaxImgSize,	// JPEG压缩缓冲器最大字节长度.
	PDWORD32			pdwImgSize,		// JPEG压缩后数据的实际字节长度, 为NULL则不填数据.
	BOOL				fVScale			// 垂直方向是否拉伸,0=不拉, 1=拉伸
);

/*
函数说明:
	对传入的原始YUV图象进行JPEG解压处理.
函数返回值:
	返回S_OK表示JPEG解压成功.
	返回E_POINTER表示pbJpegImgData或者pImage为INVALID_POINTER.
	返回E_INVALIDARG表示传入的参数有错.
	返回E_FAIL表示JPEG解压失败. */
extern HRESULT Jpeg_Dec(
	PBYTE8 				pbJpegImgData, 	// 指向JPEG数据.
	DWORD32				dwDataLen,		// JPEG数据长度.
	HV_COMPONENT_IMAGE 	*pImage			// 输出图像
);


#ifdef __cplusplus
	}
#endif	//#ifdef __cplusplus

#endif	//#ifndef _JPEGENC_H
