#ifndef _IMG_TRANS_H
#define _IMG_TRANS_H

#include "HvUtils.h"
#include "hvbaseobj.h"

HRESULT ImgTrans(
				 BYTE8* pSrcBuf, //图像数据
				 int nSrcBufLen,
				 BYTE8*	 pDstBuf, //输出缓存
				 int& nDstBufLen, //输出缓存大小
				 int nBrightness, //亮度增强, -255~255
				 int nContrast, //对比度, -100~100
				 int nThrehold, //亮度阈值
				 int nStretch, //亮度拉伸开关
				 int nCompressRate //压缩率, 0~100
				 );

HRESULT TrafficLightEnhance(
							BYTE8* pSrcBuf, //图像数据
							int nSrcBufLen,
							BYTE8*	 pDstBuf, //输出缓存
							int& nDstBufLen, //输出缓存大小
							HiVideo::CRect* rgPos = NULL,	 // 灯组位置及计数	
							int nPosCount = 1, 
							int nBrightness = 0, //亮度增强
							int nHueThreshold = 120,	//色度阈值
							int nCompressRate = 90 //压缩率
							);

#endif