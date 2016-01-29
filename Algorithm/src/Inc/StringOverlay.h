#ifndef _STRINGOVERLAY_H_
#define _STRINGOVERLAY_H_

#include "swbasetype.h"
#include "swwinerror.h"
#include "swimage.h"
#include "DspLink.h"
#include "DspLinkCmd.h"

#ifdef __cplusplus
extern "C" {
#endif

/** 
 * 初始化字符叠加DSP命令
 * 
 * @param pStringOverlayParam   参数
 * @param pStringOverlayData    点阵数据
 * @param pStringOverlayRespond 响应包
 * 
 * @return S_OK
 */
HRESULT InitStringOverlay(
	const STRING_OVERLAY_PARAM* pStringOverlayParam,
	const STRING_OVERLAY_DATA*  pStringOverlayData,
	STRING_OVERLAY_RESPOND* pStringOverlayRespond
);

/** 
 * 初始化字符叠加相关参数
 * 
 * @param x 叠加区域X坐标
 * @param y 叠加区域Y坐标
 * @param w 点阵宽度(总体宽度，非单个点阵)
 * @param h 点阵高度
 * @param iIsFixedLight 字体亮度固定 
 * @param iFontColor 字体颜色(RGB)
 * @param fIsSideInstall 侧装
 * @param rgbDotMatrix 点阵数据缓冲区
 * 
 * @note
 *       测试中发现侧装的H264高度为旋转高的一半，故需要此参数来判断
 *       问题：是否所有的侧装都如此？有待验证
 */
void SetOverlayMessageContent(
	int x, int y,
	int w, int h,
	int iIsFixedLight,
	int iFontColor,
	BOOL  fIsSideInstall,
	const PBYTE8 rgbDotMatrix
);

/** 
 * 叠加YUV422P空间(如200W JPEG码流)
 * 
 * @param szDateTimeStrings 时间字符串(由ARM端传入)
 * @param pbY Y缓冲区
 * @param iYStride Y跨度
 * @param pbU U缓冲区
 * @param iUStride U跨度
 * @param pbV V缓冲区
 * @param iVStride V跨度
 * @param iImageWidth 图像宽度
 * @param iImageHeight 图像高度
 *
 * @note 
 *        需要图像宽、高是因为需要将叠加的区域限制在可见图像范围内
 */
void DoStringOverlayYUV422P(
	const PBYTE8 szDateTimeStrings,
	PBYTE8 pbY, int iYStride,
	PBYTE8 pbU, int iUStride,
	PBYTE8 pbV, int iVStride,
	int iImageWidth, int iImageHeight
);

/** 
 * 叠加YUV422SP空间(如H264码流)
 * 
 * @param szDateTimeStrings 时间字符串(由ARM端传入)
 * @param fDoubleStream 双码流标志
 * @param pbY Y缓冲区
 * @param iYStride Y跨度
 * @param pbC UV缓冲区
 * @param iCStride UV跨度
 * @param iImageWidth 图像宽度
 * @param iImageHeight 图像高度
 *
 * @note 
 *        需要图像宽、高是因为需要将叠加的区域限制在可见图像范围内
 *        双码流时，H264最后编码，在此情况下不进行叠加，否则字体颜色
 *        与JPEG流不一致，但侧装时，需要进行叠加
 */
void DoStringOverlayYUV422SP(
	const PBYTE8 szDateTimeStrings,
	BOOL fDoubleStream,
	PBYTE8 pbY, int iYStride,
	PBYTE8 pbC, int iCStride,
	int iImageWidth, int iImageHeight
);

/** 
 * 叠加YUV420P空间(如500W JPEG码流)
 * 
 * @param szDateTimeStrings 时间字符串(由ARM端传入)
 * @param pbY Y缓冲区
 * @param iYStride Y跨度
 * @param pbU U缓冲区
 * @param iUStride U跨度
 * @param pbV V缓冲区
 * @param iVStride V跨度
 * @param iImageWidth 图像宽度
 * @param iImageHeight 图像高度
 *
 * @note 
 *        需要图像宽、高是因为需要将叠加的区域限制在可见图像范围内
 */
void DoStringOverlayYUV420P(
	const PBYTE8 szDateTimeStrings,
	PBYTE8 pbY, int iYStride,
	PBYTE8 pbU, int iUStride,
	PBYTE8 pbV, int iVStride,
	int iImageWidth, int iImageHeight
);

#ifdef __cplusplus
}
#endif

#endif

