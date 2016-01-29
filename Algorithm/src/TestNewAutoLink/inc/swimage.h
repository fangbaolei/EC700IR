#ifndef __IMAGE_H__
#define __IMAGE_H__

#include "swBaseType.h"
#include "swPlate.h"

#ifdef __cplusplus
extern "C" {
#endif		// #ifdef __cplusplus

#ifndef PI
#define PI 3.1415926f
#endif

typedef enum {
	HV_IMAGE_YUV_422 = 0,
	HV_IMAGE_YUV_411,
	HV_IMAGE_CbYCrY,
	HV_IMAGE_YCbYCr,
	HV_IMAGE_BGR,
	HV_IMAGE_GRAY,
	HV_IMAGE_HSV,
	HV_IMAGE_HSB,
	HV_IMAGE_BIN,
	HV_IMAGE_JPEG,			// jpeg压缩类型
	HV_IMAGE_TYPE_COUNT
} HV_IMAGE_TYPE;

typedef struct _HV_COMPONENT_IMAGE {
	BYTE8 *pbData[3];			// 图象数据
	int iStrideWidth[3];		// 图象存储宽度(以字节为单位).
	int iHeight;				// 图象高度
	int iWidth;					// 图象实际宽度
	HV_IMAGE_TYPE nImgType;		// 图象类型
} HV_COMPONENT_IMAGE;

HRESULT ConvertYCbCr2HSV( int iHSVStride, BYTE8 *pbHSVImg, HV_COMPONENT_IMAGE *pYCbCrImg );
HRESULT ConvertYCbCr2HSB( int iHSBStride, BYTE8 *pbHSVImg, HV_COMPONENT_IMAGE *pYCbCrImg, int x, int y, int h, int w );

HRESULT CreateItgImage(
	int iItgStride, DWORD32 *pdwItgImg, int iSrcHeight, int iSrcWidth, int iSrcStride, BYTE8 *pSrcImg );
HRESULT HvCopyImage(
	int iDestStride, BYTE8 *pbDestImg, int iSrcHeight, int iSrcWidth, int iSrcStride, BYTE8 *pbSrcImg );
HRESULT CopyComponentImage( HV_COMPONENT_IMAGE *pDestImg, HV_COMPONENT_IMAGE *pSrcImg );
HRESULT ConvertBGR2YCbCr(
	HV_COMPONENT_IMAGE *pYCbCrImg, int iWidth, int iHeight, int iStride, BYTE8 *pbRGBImg );

HRESULT ConvertBGR2YCbCrEx(
			 HV_COMPONENT_IMAGE *pYCbCrImg, int iWidth, int iHeight, int iStride, BYTE8 *pbRGBImg );
HRESULT ConvertBayer2YCbCr(
			   HV_COMPONENT_IMAGE *pYCbCrImg,
			   int iWidth, int iHeight, int iStride,
			   BYTE8 *pbBayerBuff
			   );
HRESULT InvertGrayImage(
	int iDestStride, BYTE8 *pbDestImg, int iHeight, int iWidth, int iSrcStride, BYTE8 *pbSrcImg );
HRESULT GrayImageGaussian3_3(
	int iDestStride, BYTE8 *pbDestImg, int iSrcHeight, int iSrcWidth, int iSrcStride, BYTE8 *pbSrcImg );
HRESULT ConvertYCbCr2BGR( int iBGRStride, BYTE8 *pbBGRImg, const HV_COMPONENT_IMAGE *pYCbCrImg );
HRESULT ConvertYCbCr2BGREx( int iBGRStride, BYTE8 *pbBGRImg, const HV_COMPONENT_IMAGE *pYCbCrImg );
HRESULT MarkLine(HV_COMPONENT_IMAGE imgSegResult, int iLine, BYTE8 bColor);
HRESULT MarkRect(HV_COMPONENT_IMAGE imgSegResult, HV_RECT rect, BYTE8 bColor);
HRESULT YCbCr2BGR(BYTE8 Y, BYTE8 Cb, BYTE8 Cr, PBYTE8 b, PBYTE8 g, PBYTE8 r);
HRESULT YCbCr2HSV(BYTE8 Y, BYTE8 Cb, BYTE8 Cr, PBYTE8 h, PBYTE8 s, PBYTE8 v);
HRESULT IMAGE_ConvertYCbCr2BGR( const HV_COMPONENT_IMAGE *pYCbCrImg, BYTE8* pbR, BYTE8* pbG, BYTE8* pbB,  int iBGRStride);

HRESULT hvImageResizeGray(HV_COMPONENT_IMAGE *hvDst, HV_COMPONENT_IMAGE *hvSrc);	//改变图像大小
HRESULT hvNormalizeMeanVar(										//标准的归一化方法
	HV_COMPONENT_IMAGE* pImageNormalized,	// normalized image, buffer should be located outside
	const HV_COMPONENT_IMAGE* pImageSrc,	// source image
	int fltMean,				// mean of normalized image
	int fltVar);				// Var. of normalized image
HRESULT hvLocalNormalizeMeanVar(									//局部归一化
	HV_COMPONENT_IMAGE* pImageNormalized,	// normalized image, buffer should be located outside
	const HV_COMPONENT_IMAGE* pImageSrc,	// source image
	int fltMean,				// mean of normalized image
	int fltVar);

//灰度转换成二值化图
HRESULT GrayToBin( int fIsWhiteChar, RESTRICT_PBYTE8 pbBin, const RESTRICT_PBYTE8 pbYUV, int iX, int iY, int iWidth, int iHeight, int iStride );
HRESULT IMAGE_GrayToBin( HV_COMPONENT_IMAGE imgSrc, RESTRICT_PBYTE8 pbBin);
HRESULT IMAGE_CalcBinaryThreshold(HV_COMPONENT_IMAGE imgSrc, int iA1, int iA2, PBYTE8 pbThreshold);

//缩放灰度图
HRESULT ScaleGrey(
				  RESTRICT_PBYTE8 pDstImg,
				  int nDstWidth,
				  int nDstHeight,
				  int nDstStride,
				  const RESTRICT_PBYTE8 pSrcImg,
				  int nSrcWidth,
				  int nSrcHeight,
				  int nSrcStride);

// 由中心的灰度计算二值化阈值并返回
int CalcBinaryThreByCenter(const BYTE8 *pbYUV, int iWidth, int iHeight, int iStride, HV_RECT *rtCenter);
// 由阈值二值化并输出二值化图
int GrayToBinByThre(BOOL fIsWhiteChar, const RESTRICT_PBYTE8 pbYUV, int iWidth, int iHeight, int iStride, int iThre, RESTRICT_PBYTE8 pbBin);

//灰度转换成二值化图,根据中心位置计算阈值
HRESULT GrayToBinByCenter(int fIsWhiteChar, RESTRICT_PBYTE8 pbBin, const RESTRICT_PBYTE8 pbGrey,
						  int iWidth, int iHeight, int iStride, HV_RECT *rtCenter);

// Sobel 算子求垂直和水平边缘
HRESULT IMAGE_sobel_V_NEW(
	RESTRICT_PBYTE8  pbIn,
	RESTRICT_PBYTE8  pbOut,
	int iCols,
	int iRows,
	int	iStride
);

HRESULT IMAGE_sobel_V(
	RESTRICT_PBYTE8  pbIn,
	RESTRICT_PBYTE8  pbOut,
	int     iCols,
	int     iRows
);

HRESULT hvImageSobelV(
					  HV_COMPONENT_IMAGE imgSrc,
					  HV_COMPONENT_IMAGE imgDst);

HRESULT IMAGE_sobel_H(
	RESTRICT_PBYTE8  pbIn,
	RESTRICT_PBYTE8  pbOut,
	int iCols,
	int iRows
);

HRESULT hvImageSobelH(
					  HV_COMPONENT_IMAGE imgSrc,
					  HV_COMPONENT_IMAGE imgDst);

HRESULT IMAGE_sobel_New(
	RESTRICT_PBYTE8  pbIn,
	RESTRICT_PBYTE8  pbOut,
	int iCols,
	int iRows
);

HRESULT IMAGE_sobel_New_XM(
						RESTRICT_PBYTE8  pbIn,
						RESTRICT_PBYTE8  pbOut,
						int iCols,
						int iRows
						);

int IMAGE_SegLocalMeanBin( 
						  HV_COMPONENT_IMAGE* pImg, 
						  BYTE8 *pbBinImg,
						  int iIsWhiteChar
						  );

typedef struct _COLOR_INFO
{
	PLATE_COLOR nPlateColor;

	BYTE8 nCharType;		//0: 白字 1: 黑字 2: 未知

	BYTE8 Hue_1;	//亮点区平均色度
	BYTE8 Sat_1;	//亮点区平均饱合度

	BYTE8 Hue_0;	//暗点区平均色度
	BYTE8 Sat_0;	//暗点区平均饱合度
}
COLOR_INFO;

HRESULT GetColorInfo(
	const HV_COMPONENT_IMAGE* pImage,
	COLOR_INFO* pColorInfo
);

//单通道均衡化
HRESULT GreyEqualization(HV_COMPONENT_IMAGE image);

//BGR均衡化
HRESULT BgrEqualization(PBYTE8 pBGR, int iWidth, int iHeight, int iStride);

//灰度拉伸
void GrayStretch(
				 RESTRICT_PBYTE8 pBuf, 
				 DWORD32 nWidth, 
				 DWORD32 nHeight, 
				 DWORD32 nStrideWidth,
				 BYTE8 nMin,
				 BYTE8 nMax
				 );

//图像增强, 灰度按分布拉伸+强化边缘
HRESULT PlateEnhance( HV_COMPONENT_IMAGE* pSrcImg );

//处理半黑白牌
HRESULT ProcessBWPlate( HV_COMPONENT_IMAGE* pSrcImg );
//车牌缩放
HRESULT PlateResize(HV_COMPONENT_IMAGE imgSrc, HV_COMPONENT_IMAGE imgDst);

// 图像逆时针旋转90°
HRESULT ImageRotate90(HV_COMPONENT_IMAGE imgSrc, HV_COMPONENT_IMAGE imgDst);

#ifdef __cplusplus
}
#endif		// #ifdef __cplusplus

#endif		// #ifndef __IMAGE_H__

