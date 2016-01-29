#include <math.h>
#include "swimage.h"
#include "swimageobj.h"
#include "swbasetype.h"
#include "hv_math.h"
#include "swwinerror.h"
#include "hvutils.h"

#if (RUN_PLATFORM == PLATFORM_DSP_BIOS)
extern "C" void IMG_sobel
(
 const unsigned char *in_data,      /* Input image data  */
 unsigned char       *out_data,     /* Output image data */
 short cols, short rows             /* Image dimensions  */
 );

extern "C" void IMG_thr_le2min
(
 const unsigned char *in_data,     /*  Input image data  */
 unsigned char *restrict out_data, /*  Output image data */
 short cols, short rows,           /*  Image dimensions  */
 unsigned char       threshold     /*  Threshold value   */
 );

#endif

// hue 颜色表 0~239
// 0:蓝 1:黄 2:白 3:黑 4:红 5:绿
near BYTE8 g_hueColorTable[241];

HRESULT ConvertYCbCr2HSV( int iHSVStride, BYTE8 *pbHSVImg, HV_COMPONENT_IMAGE *pYCbCrImg )
{
	int iPosX, iHeight, iWidth, iStride;
	RESTRICT_PBYTE8 pbY;
	RESTRICT_PBYTE8 pbCb;
	RESTRICT_PBYTE8 pbCr;
	RESTRICT_PBYTE8 pbHSV;
	int iRed, iGreen, iBlue, iY, iCb, iCr, iMax, iMin;

	if ( ( pYCbCrImg == NULL ) || ( pbHSVImg == NULL ) )
	{
		return E_POINTER;
	}
	iWidth = pYCbCrImg->iWidth;
	if ( iWidth > HV_ABS( iHSVStride ) / 3 )
	{
		return E_INVALIDARG;
	}
	pbY = GetHvImageData(pYCbCrImg, 0);
	pbCb = GetHvImageData(pYCbCrImg, 1);
	pbCr = GetHvImageData(pYCbCrImg, 2);
	if ( ( pbY == NULL ) || ( pbCb == NULL ) || ( pbCr == NULL ) )
	{
		return E_INVALIDARG;
	}
	iHeight = pYCbCrImg->iHeight;
	iStride = pYCbCrImg->iStrideWidth[0];
	for ( ; iHeight; iHeight --, pbHSVImg += iHSVStride, pbY += iStride, pbCb += iStride >> 1, pbCr += iStride >> 1 )
	{
		pbHSV = pbHSVImg;
		for ( iPosX = 0; iPosX < iWidth; iPosX ++, pbHSV += 3 )
		{
			iY = pbY[ iPosX ];
			iCb = ( ( int )pbCb[ iPosX >> 1 ] ) - 128;
			iCr = ( ( int )pbCr[ iPosX >> 1 ] ) - 128;
			iRed = iY + ( ( 22970 * iCr ) >> 14 );
			iGreen = iY - ( ( 5638 * iCb + 11700 * iCr ) >> 14 );
			iBlue = iY + ( ( 29032 * iCb ) >> 14 );
			iRed = RANGE_INT( iRed, 0, 255 );
			iGreen = RANGE_INT( iGreen, 0, 255 );
			iBlue = RANGE_INT( iBlue, 0, 255 );
			if ( iRed > iGreen )
			{
				iMax = MAX_INT( iRed, iBlue );
				iMin = MIN_INT( iGreen, iBlue );
			}
			else
			{
				iMax = MAX_INT( iGreen, iBlue );
				iMin = MIN_INT( iRed, iBlue );
			}
			pbHSV[ 2 ] = iMax;
			iMin = iMax - iMin;
			if ( iMax ) {
				iCb = ROUND_DIV_POSITIVE( ( iMin << 8 ) - iMin, iMax );
				pbHSV[ 1 ] = MIN_INT( iCb, 255 );
				if ( pbHSV[ 1 ] ) {
					iCb = iMin << 2;
					iCr = iMin << 1;
					iMin = iCb + iCr;
					if ( iRed == iMax ) {
						if ( iGreen >= iBlue ) {
							iMax = iGreen - iBlue;
						} else {
							iMax = iMin + iGreen - iBlue;
						}
					} else {
						if ( iGreen == iMax ) {
							iMax = iCr + iBlue - iRed;
						} else {
							iMax = iCb + iRed - iGreen;
						}
					}
					iMax = ROUND_DIV_POSITIVE( ( iMax << 8 ) - iMax, iMin );
					pbHSV[ 0 ] = MIN_INT( iMax, 255 );
				} else {
					pbHSV[ 0 ] = 0;
				}
			} else {
				pbHSV[ 1 ] = 0;
				pbHSV[ 0 ] = 0;
			}
		}
	}
	return S_OK;
}

HRESULT ConvertYCbCr2HSB(
	int iHSBStride, BYTE8 *pbHSVImg,
	HV_COMPONENT_IMAGE *pYCbCrImg,
	int x, int y, int h, int w
)
{
	int iPosX, iHeight, iWidth, iStride;
	RESTRICT_PBYTE8 pbY;
	RESTRICT_PBYTE8 pbCb;
	RESTRICT_PBYTE8 pbCr;
	RESTRICT_PBYTE8 pbHSV;
	int iRed, iGreen, iBlue, iY, iCb, iCr, iMax, iMin;

	int iScore = 240;

	int iDelta, iDiff, iL, iS, iH, iTemp;

	if ((w > pYCbCrImg->iWidth) || (h > pYCbCrImg->iHeight)) return E_INVALIDARG;

	if ( ( pYCbCrImg == NULL ) || ( pbHSVImg == NULL ) ) return E_POINTER;
	iWidth = w;
	if ( iWidth > HV_ABS( iHSBStride ) / 3 ) return E_INVALIDARG;

	iStride = pYCbCrImg->iStrideWidth[0];

	//修改
	pbY = GetHvImageData(pYCbCrImg, 0) + (y * iStride);
	pbCb = GetHvImageData(pYCbCrImg, 1) + (y * (iStride >> 1));
	pbCr = GetHvImageData(pYCbCrImg, 2) + (y * (iStride >> 1));

	if ( ( pbY == NULL ) || ( pbCb == NULL ) || ( pbCr == NULL ) ) return E_INVALIDARG;
	iHeight = h;

	iTemp = 60 * iScore;

	for ( ; iHeight; iHeight --, pbHSVImg += iHSBStride, pbY += iStride, pbCb += iStride >> 1, pbCr += iStride >> 1 )
	{
		pbHSV = pbHSVImg;
		for ( iPosX = x; iPosX < iWidth + x; iPosX ++, pbHSV += 3 )
		{
			iY = pbY[ iPosX ];
			iCb = ( ( int )pbCb[ iPosX >> 1 ] ) - 128;
			iCr = ( ( int )pbCr[ iPosX >> 1 ] ) - 128;
			iRed = iY + ( ( 22970 * iCr ) >> 14 );
			iGreen = iY - ( ( 5638 * iCb + 11700 * iCr ) >> 14 );
			iBlue = iY + ( ( 29032 * iCb ) >> 14 );
			iRed = RANGE_INT( iRed, 0, 255 );
			iGreen = RANGE_INT( iGreen, 0, 255 );
			iBlue = RANGE_INT( iBlue, 0, 255 );
			if ( iRed > iGreen )
			{
				iMax = MAX_INT( iRed, iBlue );
				iMin = MIN_INT( iGreen, iBlue );
			}
			else
			{
				iMax = MAX_INT( iGreen, iBlue );
				iMin = MIN_INT( iRed, iBlue );
			}

			iDelta = iMax - iMin;
			iDiff = iMax + iMin;

			if ( iMax )
			{
				//determine l
				iL = ROUND_DIV_POSITIVE(iScore * iDiff, 510);

				// determine saturation
				iS = 0;
				if ((iDiff != 0) && (iDiff != 510))
				{
					if( iDiff <= 255)
						iS = ROUND_DIV_POSITIVE(iScore * iDelta, iDiff);
					else
						iS = ROUND_DIV_POSITIVE(iScore * iDelta, 510 - iDiff);
				}

				// determine hue
				iH = 0;
				if (iMax != iMin)
				{
					if (iMax == iRed)
					{
						iH = (iGreen - iBlue) * iTemp;
					}
					else if (iMax == iGreen)
					{
						iH = (2 * iDelta + (iBlue - iRed)) * iTemp;
					}
					else
					{
						iH = (4 * iDelta + (iRed - iGreen)) * iTemp;
					}

					if  (iH < 0) iH = iH + 360 * iDelta * iScore;
					iH = ROUND_DIV_POSITIVE(iH, 360 * iDelta);
				}

				pbHSV[ 0 ] = iH;
				pbHSV[ 1 ] = iS;
				pbHSV[ 2 ] = iL;
			}
			else
			{
				pbHSV[ 0 ] = pbHSV[ 1 ] = pbHSV[ 2 ] = 0;
			}
		}
	}
	return S_OK;
}

HRESULT CreateItgImage(
	int iItgStride, DWORD32 *pdwItgImg,
	int iSrcHeight, int iSrcWidth, int iSrcStride,
	RESTRICT_PBYTE8 pbSrcImg
)
{
	int iWidth, iSum;
	RESTRICT_PDWORD32 pdwUpLine = pdwItgImg;

	if ( ( pdwItgImg == NULL ) || ( pbSrcImg == NULL ) ) return E_POINTER;
	if ( HV_ABS( iItgStride >> 2 ) < iSrcWidth ) return E_INVALIDARG;
	for ( iWidth = 0; iWidth <= iSrcWidth; iWidth ++ ) pdwUpLine[ iWidth ] = 0;
	for ( ; iSrcHeight; iSrcHeight --, pbSrcImg += iSrcStride ) {
		pdwItgImg = ( DWORD32 * )( ( ( BYTE8 * )pdwUpLine ) + iItgStride );
		pdwItgImg[ 0 ] = 0;
		iSum = 0;
		for ( iWidth = 1; iWidth <= iSrcWidth; iWidth ++ ) {
			iSum += pbSrcImg[ iWidth - 1 ];
			pdwItgImg[ iWidth ] = pdwUpLine[ iWidth ] + iSum;
		}
		pdwUpLine = pdwItgImg;
	}
	return S_OK;
}

HRESULT CreatItgSqImage(
						int iItgStride, DWORD32 *pdwItgSqImg,
						int iSrcHeight, int iSrcWidth, int iSrcStride,
						RESTRICT_PBYTE8 pbSrcImg
						)
{
	int iWidth, iSum;
	RESTRICT_PDWORD32 pdwUpLine = pdwItgSqImg;

	if ( ( pdwItgSqImg == NULL ) || ( pbSrcImg == NULL ) ) return E_POINTER;
	if ( HV_ABS( iItgStride >> 2 ) < iSrcWidth ) return E_INVALIDARG;
	//	for ( iWidth = 0; iWidth <= iSrcWidth; iWidth ++ ) pdwUpLine[ iWidth ] = 0;
	memset(pdwUpLine, 0, sizeof(DWORD32) * (iSrcWidth + 1));
	for ( ; iSrcHeight; iSrcHeight --, pbSrcImg += iSrcStride ) {
		pdwItgSqImg = ( DWORD32 * )( ( ( BYTE8 * )pdwUpLine ) + iItgStride );
		pdwItgSqImg[ 0 ] = 0;
		iSum = 0;
		for ( iWidth = 1; iWidth <= iSrcWidth; iWidth ++ ) {
			iSum += pbSrcImg[ iWidth - 1 ] * pbSrcImg[ iWidth - 1 ];
			pdwItgSqImg[ iWidth ] = pdwUpLine[ iWidth ] + iSum;
		}
		pdwUpLine = pdwItgSqImg;
	}
	return S_OK;
}
HRESULT HvCopyImage(
	int iDestStride, RESTRICT_PBYTE8 pbDestImg,
	int iSrcHeight, int iSrcWidth, int iSrcStride,
	RESTRICT_PBYTE8 pbSrcImg
)
{
	if ( ( pbDestImg == NULL ) || ( pbSrcImg == NULL ) ) return E_POINTER;
	if ( HV_ABS( iDestStride ) < iSrcWidth ) return E_INVALIDARG;
	for ( ; iSrcHeight; iSrcHeight --, pbSrcImg += iSrcStride, pbDestImg += iDestStride )
		HV_memcpy( pbDestImg, pbSrcImg, ( size_t )iSrcWidth );
	return S_OK;
}

HRESULT CopyComponentImage(
	HV_COMPONENT_IMAGE *pDestImg,
	HV_COMPONENT_IMAGE *pSrcImg
)
{
	int iHeight, iWidth, iSrcStride, iDestStride;
	RESTRICT_PBYTE8 pbSrcY;
	RESTRICT_PBYTE8 pbSrcCb;
	RESTRICT_PBYTE8 pbSrcCr;
	RESTRICT_PBYTE8 pbDestY;
	RESTRICT_PBYTE8 pbDestCb;
	RESTRICT_PBYTE8 pbDestCr;

	if ( ( pDestImg == NULL ) || ( pSrcImg == NULL ) ) return E_POINTER;
	iDestStride = pDestImg->iStrideWidth[0];
	iWidth = pSrcImg->iWidth;
	if ( HV_ABS( iDestStride ) < iWidth ) return E_INVALIDARG;
	if(pSrcImg->nImgType == HV_IMAGE_BT1120_UV)
	{
	    pbSrcY = GetHvImageData(pSrcImg, 0);
        pbSrcCb = GetHvImageData(pSrcImg, 2);
        pbSrcCr = (RESTRICT_PBYTE8)(GetHvImageData(pSrcImg, 2) + (pSrcImg->iWidth * pSrcImg->iHeight >> 1));
	}
	else
	{
	    pbSrcY = GetHvImageData(pSrcImg, 0);
        pbSrcCb = GetHvImageData(pSrcImg, 1);
        pbSrcCr = GetHvImageData(pSrcImg, 2);
	}

	if ( ( pbSrcY == NULL ) || ( pbSrcCb == NULL ) || ( pbSrcCr == NULL ) ) return E_INVALIDARG;
	if(pSrcImg->nImgType == HV_IMAGE_BT1120_UV)
	{
	    pbDestY = GetHvImageData(pDestImg, 0);
        pbDestCb = GetHvImageData(pDestImg, 2);
        pbDestCr = (RESTRICT_PBYTE8)(GetHvImageData(pDestImg, 2) + (pSrcImg->iWidth * pSrcImg->iHeight >> 1));
	}
	else
	{
	    pbDestY = GetHvImageData(pDestImg, 0);
        pbDestCb = GetHvImageData(pDestImg, 1);
        pbDestCr = GetHvImageData(pDestImg, 2);
	}
	if ( ( pbDestY == NULL ) || ( pbDestCb == NULL ) || ( pbDestCr == NULL ) ) return E_INVALIDARG;
	iHeight = pDestImg->iHeight = pSrcImg->iHeight;
	pDestImg->iWidth = pSrcImg->iWidth;
	iSrcStride = pSrcImg->iStrideWidth[0];
	for ( ; iHeight; iHeight --, pbSrcY += iSrcStride, pbSrcCb += iSrcStride >> 1, pbSrcCr += iSrcStride >> 1 ) {
		HV_memcpy( pbDestY, pbSrcY, ( size_t )iWidth );
		HV_memcpy( pbDestCb, pbSrcCb, ( size_t )( ( iWidth + 1 ) >> 1 ) );
		HV_memcpy( pbDestCr, pbSrcCr, ( size_t )( ( iWidth + 1 ) >> 1 ) );
		pbDestY += iDestStride;
		pbDestCb += iDestStride >> 1;
		pbDestCr += iDestStride >> 1;
	}
	return S_OK;
}

HRESULT ConvertBGR2YCbCr(
	HV_COMPONENT_IMAGE *pYCbCrImg,
	int iWidth, int iHeight, int iStride,
	BYTE8 *pbRGBImg
)
{
	PBYTE8 pbY;
	PBYTE8 pbCb;
	PBYTE8 pbCr;
	PBYTE8 pbBGR;
	int iYCbCrStride, iPosX, iTemp, iCb = 0, iCr = 0;

	if ( ( pYCbCrImg == NULL ) || ( pbRGBImg == NULL ) ) return E_POINTER;
	iYCbCrStride = pYCbCrImg->iStrideWidth[0];
	if ( HV_ABS( iYCbCrStride ) < iWidth ) return E_INVALIDARG;
	pbY = GetHvImageData(pYCbCrImg, 0);
	pbCb = GetHvImageData(pYCbCrImg, 1);
	pbCr = GetHvImageData(pYCbCrImg, 2);
	if ( ( pbY == NULL ) || ( pbCb == NULL ) || ( pbCr == NULL ) ) return E_INVALIDARG;
	pYCbCrImg->iHeight = iHeight;
	pYCbCrImg->iWidth = iWidth;
	for ( ; iHeight; iHeight --, pbRGBImg += iStride ) {
		pbBGR = pbRGBImg;
		for ( iPosX = 0; iPosX < iWidth; iPosX ++, pbBGR += 3 ) {
			iTemp = 1868 * pbBGR[ 0 ] + 9617 * pbBGR[ 1 ] + 4899 * pbBGR[ 2 ];
			pbY[ iPosX ] = ( BYTE8 )ROUND_SHR_POSITIVE( iTemp, 14 );
			iTemp = 8192 * pbBGR[ 0 ] - 5428 * pbBGR[ 1 ] - 2758 * pbBGR[ 2 ];
			iTemp = ROUND_SHR( iTemp, 14 ) + 128;
			iTemp = RANGE_INT( iTemp, 0, 255 );
			if ( iPosX & 1 ) {
				pbCb[ iPosX >> 1 ] = ( BYTE8 )( ( iTemp + iCb ) >> 1 );
			} else {
				iCb = iTemp;
			}
			iTemp = -1332 * pbBGR[ 0 ] - 6860 * pbBGR[ 1 ] + 8192 * pbBGR[ 2 ];
			iTemp = ROUND_SHR( iTemp, 14 ) + 128;
			iTemp = RANGE_INT( iTemp, 0, 255 );
			if ( iPosX & 1 ) {
				pbCr[ iPosX >> 1 ] = ( BYTE8 )( ( iTemp + iCr ) >> 1 );
			} else {
				iCr = iTemp;
			}
		}
		pbY += iYCbCrStride;
		pbCb += iYCbCrStride >> 1;
		pbCr += iYCbCrStride >> 1;
	}
	return S_OK;
}

#define MY(r,g,b) ((r*0.2989 + g*0.5866 + b*0.1145))
#define MU(r,g,b) ((r*(-0.1688) + g*(-0.3312) + b*0.5000 + 128))
#define MV(r,g,b) ((r*0.5000 + g*(-0.4184) + b*(-0.0816) + 128))

HRESULT InvertGrayImage(
	int iDestStride, RESTRICT_PBYTE8 pbDestImg,
	int iHeight, int iWidth, int iSrcStride,
	RESTRICT_PBYTE8 pbSrcImg
)
{
	int iPosX;

	if ( ( pbDestImg == NULL ) || ( pbSrcImg == NULL ) ) return E_POINTER;
	if ( HV_ABS( iDestStride ) < iWidth ) return E_INVALIDARG;
	if ( ( ( ( DWORD32 )pbSrcImg ) & 0x03 ) || ( iSrcStride & 0x03 ) ||
		( ( ( DWORD32 )pbDestImg ) & 0x03 ) || ( iDestStride & 0x03 ) ) {
		for ( ; iHeight; iHeight --, pbSrcImg += iSrcStride, pbDestImg += iDestStride ) {
			for ( iPosX = 0; iPosX < iWidth; iPosX ++ ) pbDestImg[ iPosX ] = pbSrcImg[ iPosX ] ^ 0xFF;
		}
	} else {
		iWidth = ( iWidth & 0x03 ) ? ( iWidth >> 2 ) + 1 : ( iWidth >> 2 );
		for ( ; iHeight; iHeight --, pbSrcImg += iSrcStride, pbDestImg += iDestStride ) {
			for ( iPosX = 0; iPosX < iWidth; iPosX ++ )
				( ( DWORD32 * )pbDestImg )[ iPosX ] = ( ( DWORD32 * )pbSrcImg )[ iPosX ] ^ 0xFFFFFFFF;
		}
	}
	return S_OK;
}

HRESULT GrayImageGaussian3_3(
	int iDestStride, RESTRICT_PBYTE8 pbDestImg, int iSrcHeight, int iSrcWidth, int iSrcStride, RESTRICT_PBYTE8 pbSrcImg
) {
	int iPosX, iPosY;
	DWORD32 dwTemp1, dwTemp2, dwTemp3;
	BYTE8 *pbUpLine, *pbNextLine, *pbDest;

	if ( ( pbDestImg == NULL ) || ( pbSrcImg == NULL ) ) return E_POINTER;
	if ( HV_ABS( iDestStride ) < iSrcWidth ) return E_INVALIDARG;
	if ( ( iSrcHeight < 3 ) || ( iSrcWidth < 3 ) ) return E_INVALIDARG;
	iSrcWidth --;
	pbDest = pbDestImg + iDestStride;
	for ( iPosY = iSrcHeight - 2; iPosY; iPosY --, pbDest += iDestStride ) {
		pbUpLine = pbSrcImg;
		pbSrcImg += iSrcStride;
		pbNextLine = pbSrcImg + iSrcStride;
		dwTemp1 = pbUpLine[ 0 ] * 43 + pbSrcImg[ 0 ] * 426 + pbNextLine[ 0 ] * 43;
		dwTemp2 = pbUpLine[ 1 ] * 43 + pbSrcImg[ 1 ] * 426 + pbNextLine[ 1 ] * 43;
		for ( iPosX = 1; iPosX < iSrcWidth; iPosX ++ ) {
			dwTemp3 = pbUpLine[ iPosX + 1 ] * 43 + pbSrcImg[ iPosX + 1 ] * 426 + pbNextLine[ iPosX + 1 ] * 43;
			dwTemp1 = dwTemp1 * 43 + dwTemp2 * 426 + dwTemp3 * 43;
			pbDest[ iPosX ] = ( BYTE8 )ROUND_SHR_POSITIVE( dwTemp1, 18 );
			dwTemp1 = dwTemp2;
			dwTemp2 = dwTemp3;
		}
	}
	iSrcWidth --;
	HV_memcpy( pbDestImg + 1, pbDestImg + iDestStride + 1, ( size_t )iSrcWidth );
	pbDest ++;
	HV_memcpy( pbDest, pbDest - iDestStride, ( size_t )iSrcWidth );
	for ( ; iSrcHeight; iSrcHeight --, pbDestImg += iDestStride ) {
		pbDestImg[ 0 ] = pbDestImg[ 1 ];
		pbDestImg[ iSrcWidth + 1 ] = pbDestImg[ iSrcWidth ];
	}
	return S_OK;
}

HRESULT ConvertYCbCr2BGR( int iBGRStride, BYTE8 *pbBGRImg, const HV_COMPONENT_IMAGE *pYCbCrImg ) {
	int iPosX, iHeight, iWidth, iStride, iTemp;
	PBYTE8 pbY;
	PBYTE8 pbCb;
	PBYTE8 pbCr;
	PBYTE8 pbCbCr;

	if ( ( pYCbCrImg == NULL ) || ( pbBGRImg == NULL ) ) return E_POINTER;
	iWidth = pYCbCrImg->iWidth;
	if ( iWidth > HV_ABS( iBGRStride ) / 3 ) return E_INVALIDARG;

	if (pYCbCrImg->nImgType == HV_IMAGE_YUV_422)
	{
		iWidth = pYCbCrImg->iWidth;
		if ( iWidth > HV_ABS( iBGRStride ) / 3 ) return E_INVALIDARG;
		pbY = GetHvImageData(pYCbCrImg, 0);
		pbCb = GetHvImageData(pYCbCrImg, 1);
		pbCr = GetHvImageData(pYCbCrImg, 2);
		if ( ( pbY == NULL ) || ( pbCb == NULL ) || ( pbCr == NULL ) ) return E_INVALIDARG;
		iHeight = pYCbCrImg->iHeight;
		iStride = pYCbCrImg->iStrideWidth[0];
		for ( ; iHeight; iHeight --, pbBGRImg += iBGRStride, pbY += iStride, pbCb += iStride >> 1, pbCr += iStride >> 1 ) {
			PBYTE8 pbBGR = pbBGRImg;
			for ( iPosX = 0; iPosX < iWidth; iPosX ++, pbBGR += 3 ) {
				iTemp = ( ( ( int )pbY[ iPosX ] ) << 14 ) + 22970 * ( ( ( int )( pbCr[ iPosX >> 1 ] ) ) - 128 );
				iTemp = ROUND_SHR( iTemp, 14 );
				pbBGR[ 2 ] = ( BYTE8 )RANGE_INT( iTemp, 0, 255 );
				iTemp = ( ( ( int )pbY[ iPosX ] ) << 14 ) - 5638 * ( ( ( int )( pbCb[ iPosX >> 1 ] ) ) - 128 )
					- 11700 * ( ( ( int )( pbCr[ iPosX >> 1 ] ) ) - 128 );
				iTemp = ROUND_SHR( iTemp, 14 );
				pbBGR[ 1 ] = ( BYTE8 )RANGE_INT( iTemp, 0, 255 );
				iTemp = ( ( ( int )pbY[ iPosX ] ) << 14 ) + 29032 * ( ( ( int )( pbCb[ iPosX >> 1 ] ) ) - 128 );
				iTemp = ROUND_SHR( iTemp, 14 );
				pbBGR[ 0 ] = ( BYTE8 )RANGE_INT( iTemp, 0, 255 );
			}
		}
		return S_OK;
	}
	if (pYCbCrImg->nImgType == HV_IMAGE_YCbYCr ||
		pYCbCrImg->nImgType == HV_IMAGE_CbYCrY)
	{
		if (pYCbCrImg->nImgType == HV_IMAGE_YCbYCr)
		{
			pbY = GetHvImageData(pYCbCrImg, 0);
			pbCb = GetHvImageData(pYCbCrImg, 0)+ 1;
			pbCr = GetHvImageData(pYCbCrImg, 0) + 3;
		}
		else
		{
			pbY = GetHvImageData(pYCbCrImg, 0) + 1;
			pbCb = GetHvImageData(pYCbCrImg, 0);
			pbCr = GetHvImageData(pYCbCrImg, 0) + 2;
		}
		if (pbY == NULL) return E_INVALIDARG;
		iHeight = pYCbCrImg->iHeight;
		iStride = pYCbCrImg->iStrideWidth[0];
		for ( ; iHeight; iHeight --, pbBGRImg += iBGRStride, pbY += iStride, pbCb += iStride, pbCr += iStride ) {
			PBYTE8 pbBGR = pbBGRImg;
			for ( iPosX = 0; iPosX < iWidth; iPosX ++, pbBGR += 3 ) {
				iTemp = ( ( ( int )pbY[ iPosX << 1 ] ) << 14 ) + 22970 * ( ( ( int )( pbCr[ (iPosX >> 1) << 2 ] ) ) - 128 );
				iTemp = ROUND_SHR( iTemp, 14 );
				pbBGR[ 2 ] = ( BYTE8 )RANGE_INT( iTemp, 0, 255 );
				iTemp = ( ( ( int )pbY[ iPosX << 1 ] ) << 14 ) - 5638 * ( ( ( int )( pbCb[ (iPosX >> 1) << 2 ] ) ) - 128 )
					- 11700 * ( ( ( int )( pbCr[(iPosX >> 1) << 2 ] ) ) - 128 );
				iTemp = ROUND_SHR( iTemp, 14 );
				pbBGR[ 1 ] = ( BYTE8 )RANGE_INT( iTemp, 0, 255 );
				iTemp = ( ( ( int )pbY[ iPosX << 1 ] ) << 14 ) + 29032 * ( ( ( int )( pbCb[ (iPosX >> 1) << 2 ] ) ) - 128 );
				iTemp = ROUND_SHR( iTemp, 14 );
				pbBGR[ 0 ] = ( BYTE8 )RANGE_INT( iTemp, 0, 255 );
			}
		}
		return S_OK;
	}
	if (pYCbCrImg->nImgType == HV_IMAGE_BT1120_FIELD
	    || pYCbCrImg->nImgType == HV_IMAGE_BT1120)
	{
		iWidth = pYCbCrImg->iWidth;
		if ( iWidth > HV_ABS( iBGRStride ) / 3 ) return E_INVALIDARG;
		pbY = GetHvImageData(pYCbCrImg, 0);
		pbCbCr = GetHvImageData(pYCbCrImg, 1);
		if ( ( pbY == NULL ) || ( pbCbCr == NULL )) return E_INVALIDARG;
		iHeight = pYCbCrImg->iHeight;
		iStride = pYCbCrImg->iStrideWidth[0];
		for ( ; iHeight; iHeight --, pbBGRImg += iBGRStride, pbY += iStride, pbCbCr += iStride ) {
			PBYTE8 pbBGR = pbBGRImg;
			for ( iPosX = 0; iPosX < iWidth; iPosX ++, pbBGR += 3 ) {
				int iOffSet = (iPosX & ~1);
				iTemp = ( ( ( int )pbY[ iPosX ] ) << 14 ) + 22970 * ( ( ( int )( pbCbCr[ iOffSet + 1] ) ) - 128 );
				iTemp = ROUND_SHR( iTemp, 14 );
				pbBGR[ 2 ] = ( BYTE8 )RANGE_INT( iTemp, 0, 255 );
				iTemp = ( ( ( int )pbY[ iPosX ] ) << 14 ) - 5638 * ( ( ( int )( pbCbCr[ iOffSet] ) ) - 128 )
					- 11700 * ( ( ( int )( pbCbCr[ iOffSet + 1 ] ) ) - 128 );
				iTemp = ROUND_SHR( iTemp, 14 );
				pbBGR[ 1 ] = ( BYTE8 )RANGE_INT( iTemp, 0, 255 );
				iTemp = ( ( ( int )pbY[ iPosX ] ) << 14 ) + 29032 * ( ( ( int )( pbCbCr[ iOffSet ] ) ) - 128 );
				iTemp = ROUND_SHR( iTemp, 14 );
				pbBGR[ 0 ] = ( BYTE8 )RANGE_INT( iTemp, 0, 255 );
			}
		}
		return S_OK;
	}
	if (pYCbCrImg->nImgType == HV_IMAGE_YUV_420)
	{
		iWidth = pYCbCrImg->iWidth;
		if ( iWidth > HV_ABS( iBGRStride ) / 3 ) return E_INVALIDARG;
		pbY = GetHvImageData(pYCbCrImg, 0);
		pbCbCr = GetHvImageData(pYCbCrImg, 1);
		if ( ( pbY == NULL ) || ( pbCbCr == NULL )) return E_INVALIDARG;
		iHeight = pYCbCrImg->iHeight;
		iStride = pYCbCrImg->iStrideWidth[0];
		for ( ; iHeight; iHeight --, pbBGRImg += iBGRStride, pbY += iStride ) {
			PBYTE8 pbBGR = pbBGRImg;
			for ( iPosX = 0; iPosX < iWidth; iPosX ++, pbBGR += 3 ) {
				int iOffSet = (iPosX & ~1);
				iTemp = ( ( ( int )pbY[ iPosX ] ) << 14 ) + 22970 * ( ( ( int )( pbCbCr[ iOffSet + 1] ) ) - 128 );
				iTemp = ROUND_SHR( iTemp, 14 );
				pbBGR[ 2 ] = ( BYTE8 )RANGE_INT( iTemp, 0, 255 );
				iTemp = ( ( ( int )pbY[ iPosX ] ) << 14 ) - 5638 * ( ( ( int )( pbCbCr[ iOffSet] ) ) - 128 )
					- 11700 * ( ( ( int )( pbCbCr[ iOffSet + 1 ] ) ) - 128 );
				iTemp = ROUND_SHR( iTemp, 14 );
				pbBGR[ 1 ] = ( BYTE8 )RANGE_INT( iTemp, 0, 255 );
				iTemp = ( ( ( int )pbY[ iPosX ] ) << 14 ) + 29032 * ( ( ( int )( pbCbCr[ iOffSet ] ) ) - 128 );
				iTemp = ROUND_SHR( iTemp, 14 );
				pbBGR[ 0 ] = ( BYTE8 )RANGE_INT( iTemp, 0, 255 );
			}

            if (iHeight % 2 != 0) pbCbCr += iStride;
		}
		return S_OK;
	}
	return E_FAIL;
}

HRESULT YCbCr2BGR(BYTE8 Y, BYTE8 Cb, BYTE8 Cr, PBYTE8 b, PBYTE8 g, PBYTE8 r)
{
	int iTemp;

	iTemp = ( ( ( int )Y ) << 14 ) + 22970 * ( ( ( int )( Cr ) ) - 128 );
	iTemp = ROUND_SHR( iTemp, 14 );
	*r = ( BYTE8 )RANGE_INT( iTemp, 0, 255 );

	iTemp = ( ( ( int )Y ) << 14 ) - 5638 * ( ( ( int )( Cb ) ) - 128 )
		- 11700 * ( ( ( int )( Cr ) ) - 128 );
	iTemp = ROUND_SHR( iTemp, 14 );
	*g = ( BYTE8 )RANGE_INT( iTemp, 0, 255 );

	iTemp = ( ( ( int )Y ) << 14 ) + 29032 * ( ( ( int )( Cb ) ) - 128 );
	iTemp = ROUND_SHR( iTemp, 14 );
	*b = ( BYTE8 )RANGE_INT( iTemp, 0, 255 );

	return S_OK;
}

HRESULT BGR2HSL(BYTE8 bB, BYTE8 bG, BYTE8 bR, BYTE8 *pH, BYTE8 *pS, BYTE8 *pL)
{
	float B = bB / 255.0;
	float G = bG / 255.0;
	float R = bR / 255.0;

	float H, S, L, Max, Min, del_Max, del_R, del_G, del_B;
	Min = MIN_INT(R, MIN_INT(G, B));
	Max = MAX_INT(R, MAX_INT(G, B));
	L = (Min + Max) / 2;
	del_Max = Max - Min;

	if (0 == del_Max)
	{
		H = S = 0;	
	}
	else
	{
		if (L < 0.5)
		{
			S = del_Max / (Max + Min);
		}
		else
		{
			S = del_Max / (2 - Max - Min);
		}

		del_R = (((Max - R) / 6.0) + (del_Max / 2.0)) / del_Max;
		del_G = (((Max - G) / 6.0) + (del_Max / 2.0)) / del_Max;
		del_B = (((Max - B) / 6.0) + (del_Max / 2.0)) / del_Max;

		if (R == Max)
		{
			H = del_B - del_G;
		}
		else if (G == Max)
		{
			H = (1.0 / 3.0) + del_R - del_B;
		}
		else if (B == Max)
		{
			H = (2.0 / 3.0) + del_G - del_R;
		}

		if (H < 0)  H += 1;
		if (H > 1)  H -= 1;

	}
	H = H * 240 + 0.5;
	S = S * 240 + 0.5;
	L = L * 240 + 0.5;
	*pH = MIN_INT(H, 240);
	*pS = MIN_INT(S, 240);
	*pL = MIN_INT(L, 240);

	return S_OK;
}

HRESULT YCbCr2HSV(BYTE8 Y, BYTE8 Cb, BYTE8 Cr, PBYTE8 h, PBYTE8 s, PBYTE8 v)
{
	int iY = Y;
	int iCb = ( ( int )Cb ) - 128;
	int iCr = ( ( int )Cr ) - 128;

	int iRed = iY + ( ( 22970 * iCr ) >> 14 );
	int iGreen = iY - ( ( 5638 * iCb + 11700 * iCr ) >> 14 );
	int iBlue = iY + ( ( 29032 * iCb ) >> 14 );

	iRed = RANGE_INT( iRed, 0, 255 );
	iGreen = RANGE_INT( iGreen, 0, 255 );
	iBlue = RANGE_INT( iBlue, 0, 255 );

	int iMin, iMax;

	if ( iRed > iGreen ) {
		iMax = MAX_INT( iRed, iBlue );
		iMin = MIN_INT( iGreen, iBlue );
	} else {
		iMax = MAX_INT( iGreen, iBlue );
		iMin = MIN_INT( iRed, iBlue );
	}
	*v = iMax;
	iMin = iMax - iMin;
	if ( iMax ) {
		iCb = ROUND_DIV_POSITIVE( ( iMin << 8 ) - iMin, iMax );
		*s = MIN_INT( iCb, 255 );
		if ( *s ) {
			iCb = iMin << 2;
			iCr = iMin << 1;
			iMin = iCb + iCr;
			if ( iRed == iMax ) {
				if ( iGreen >= iBlue ) {
					iMax = iGreen - iBlue;
				} else {
					iMax = iMin + iGreen - iBlue;
				}
			} else {
				if ( iGreen == iMax ) {
					iMax = iCr + iBlue - iRed;
				} else {
					iMax = iCb + iRed - iGreen;
				}
			}
			iMax = ROUND_DIV_POSITIVE( ( iMax << 8 ) - iMax, iMin );
			*h = MIN_INT( iMax, 255 );
		} else {
			*h = 0;
		}
	} else {
		*s = 0;
		*h = 0;
	}
	return S_OK;
}

HRESULT MarkLine(
	HV_COMPONENT_IMAGE imgSegResult,
	int iLine,
	BYTE8 bColor
)
{
	PBYTE8 pbObjAddr = GetHvImageData(&imgSegResult, 2);
	if ( ( iLine >= 0 ) && ( iLine < imgSegResult.iWidth ) )
	{
		for (int i=0; i<imgSegResult.iHeight; i++)
		{
			pbObjAddr[(imgSegResult.iStrideWidth[0]*i+iLine)>>1]=bColor;
		}
	}
	return S_OK;
}

HRESULT MarkRect(
	HV_COMPONENT_IMAGE imgSegResult,
	HV_RECT rect,
	BYTE8 bColor
)
{
	PBYTE8 pbCr = GetHvImageData(&imgSegResult, 2);

	if ( ( rect.left >= 0 ) && ( rect.left <= imgSegResult.iWidth ) &&
		( rect.right >= 0 ) && ( rect.right <= imgSegResult.iWidth ) &&
		( rect.top >=0 ) && ( rect.top <= imgSegResult.iHeight ) &&
		( rect.bottom >= 0 ) && ( rect.bottom <= imgSegResult.iHeight ) )
	{

		for (int i=rect.left; i<rect.right; i++)
		{
			pbCr[(imgSegResult.iStrideWidth[0]*rect.top+i)>>1]=bColor;
			pbCr[(imgSegResult.iStrideWidth[0]*(rect.top+1)+i)>>1]=bColor;
			pbCr[(imgSegResult.iStrideWidth[0]*(rect.bottom-1)+i)>>1]=bColor;
			pbCr[(imgSegResult.iStrideWidth[0]*(rect.bottom-2)+i)>>1]=bColor;

		}
		HV_writeback( ( PBYTE8 )&pbCr[ (imgSegResult.iStrideWidth[0] * rect.top)>>1 ], imgSegResult.iStrideWidth[0] >> 1);
		HV_writeback( ( PBYTE8 )&pbCr[ (imgSegResult.iStrideWidth[0] * ( rect.top + 1 ))>>1 ], imgSegResult.iStrideWidth[0] >> 1);
		HV_writeback( ( PBYTE8 )&pbCr[ (imgSegResult.iStrideWidth[0] * ( rect.bottom - 1) )>>1 ], imgSegResult.iStrideWidth[0] >> 1);
		HV_writeback( ( PBYTE8 )&pbCr[ (imgSegResult.iStrideWidth[0] * ( rect.bottom - 2) )>>1 ], imgSegResult.iStrideWidth[0] >> 1);

		for ( int i=rect.top; i<rect.bottom; i++)
		{
			pbCr[(imgSegResult.iStrideWidth[0]*i+rect.left)>>1]=bColor;
			pbCr[(imgSegResult.iStrideWidth[0]*i+rect.left+1)>>1]=bColor;
			pbCr[(imgSegResult.iStrideWidth[0]*i+rect.right-1)>>1]=bColor;
			pbCr[(imgSegResult.iStrideWidth[0]*i+rect.right-2)>>1]=bColor;
		}
		for ( int i=rect.top; i<rect.bottom; i++)
		{
			HV_writeback( ( PBYTE8 )&pbCr[(imgSegResult.iStrideWidth[0]*i ) >> 1], imgSegResult.iStrideWidth[0] >> 1 );
		}
		HV_cachewait();
	}
	return S_OK;
}

//灰度转换成二值化图
HRESULT GrayToBin( int fIsWhiteChar, RESTRICT_PBYTE8 pbBin, const RESTRICT_PBYTE8 pbYUV, int iX, int iY, int iWidth, int iHeight, int iStride )
{
	const int MAX_GREY_COUNT = 256;
    //计算直方图
    int rgiImhistCenter[ MAX_GREY_COUNT ];
	HV_memset( rgiImhistCenter, 0, sizeof( rgiImhistCenter ) );
    for ( int j = iY; j < iY + iHeight; j++ )
    {
	    for ( int i = iX; i < iX + iWidth; i++ )
        {
            rgiImhistCenter[ pbYUV[ j * iStride + i ] ]++;
        }
    }
	//计算阈值
	int iGrey = MAX_GREY_COUNT >> 1;
	while ( true )
	{
		DWORD32 dwTemp1 = 0, dwTemp2 = 0, dwTemp3 = 0, dwTemp4 = 0;
		for ( int i = 0; i <= iGrey; i++ )
			dwTemp1 += rgiImhistCenter[ i ] * i;
		for ( int i = 0; i <= iGrey; i++ )
			dwTemp2 += rgiImhistCenter[ i ];
		for ( int i = iGrey + 1; i < MAX_GREY_COUNT; i++ )
			dwTemp3 += rgiImhistCenter[ i ] * i;
		for ( int i = iGrey + 1; i < MAX_GREY_COUNT; i++ )
			dwTemp4 += rgiImhistCenter[ i ];
		int iGrey2;
		if ( dwTemp2 == 0 && dwTemp4 == 0 )
			iGrey2 = 0;
		else if ( dwTemp2 == 0 )
			iGrey2 = ( int )( dwTemp3 / dwTemp4 );
		else if ( dwTemp4 == 0 )
			iGrey2 = ( int )( dwTemp1 / dwTemp2 );
		else
			iGrey2 = ( int )( ( dwTemp1 / dwTemp2 + dwTemp3 / dwTemp4 ) >> 1 );
		if ( iGrey2 == iGrey )
			break;
        else
            iGrey = iGrey2;
	}
	//设置二值图
	for ( int j = iY; j < iY + iHeight; j++ )
	{
		PBYTE8 pbSrcLine = pbYUV + j * iStride;
		PBYTE8 pbDstLine = pbBin + (j - iY) * iStride - iX;
		for ( int i = iX; i < iX + iWidth; i++ )
	    {
			if ( pbSrcLine[i] > iGrey )
			{
				pbDstLine[i] = (fIsWhiteChar ? 255 : 0);
			}
			else
			{
				pbDstLine[i] = (fIsWhiteChar ? 0 : 255);
			}
		}
	}
	return S_OK;
}

HRESULT IMAGE_GrayToBin( HV_COMPONENT_IMAGE imgSrc, RESTRICT_PBYTE8 pbBin)
{
	const int MAX_GREY_COUNT = 256;

	int iHeight = imgSrc.iHeight;
	int iWidth = imgSrc.iWidth;
	int iStrideWidth = imgSrc.iStrideWidth[0];

	//计算直方图
	int rgiImhistCenter[ MAX_GREY_COUNT ] = {0};

	RESTRICT_PBYTE8 pSrcLine = GetHvImageData(&imgSrc, 0), pDstLine;
	for ( int j = 0; j < iHeight; j++, pSrcLine += iStrideWidth)
	{
		for ( int i = 0; i < iWidth; i++ )
		{
			rgiImhistCenter[ pSrcLine[i] ]++;
		}
	}

	//计算阈值
	int iGrey = MAX_GREY_COUNT >> 1;
	while ( true )
	{
		DWORD32 dwTemp1 = 0, dwTemp2 = 0, dwTemp3 = 0, dwTemp4 = 0;
		for ( int i = 0; i <= iGrey; i++ )
			dwTemp1 += rgiImhistCenter[ i ] * i;
		for ( int i = 0; i <= iGrey; i++ )
			dwTemp2 += rgiImhistCenter[ i ];
		for ( int i = iGrey + 1; i < MAX_GREY_COUNT; i++ )
			dwTemp3 += rgiImhistCenter[ i ] * i;
		for ( int i = iGrey + 1; i < MAX_GREY_COUNT; i++ )
			dwTemp4 += rgiImhistCenter[ i ];
		int iGrey2;
		if ( dwTemp2 == 0 && dwTemp4 == 0 )
			iGrey2 = 0;
		else if ( dwTemp2 == 0 )
			iGrey2 = ( int )( dwTemp3 / dwTemp4 );
		else if ( dwTemp4 == 0 )
			iGrey2 = ( int )( dwTemp1 / dwTemp2 );
		else
			iGrey2 = ( int )( ( dwTemp1 / dwTemp2 + dwTemp3 / dwTemp4 ) / 2 );
		if ( iGrey2 == iGrey )
			break;
		else
			iGrey = iGrey2;
	}

	//设置二值图
	pSrcLine = GetHvImageData(&imgSrc, 0);
	pDstLine = pbBin;
	for ( int j = 0; j < iHeight; j++, pSrcLine += iStrideWidth, pDstLine += iStrideWidth )
	{
		for ( int i = 0; i < iWidth; i++ )
		{
			pDstLine[i] = pSrcLine[i] > iGrey?255:0;
		}
	}
	return S_OK;
}

HRESULT IMAGE_CalcBinaryThreshold(HV_COMPONENT_IMAGE imgSrc, int iA1, int iA2, PBYTE8 pbThreshold)
{
	if (imgSrc.nImgType != HV_IMAGE_GRAY ||
		GetHvImageData(&imgSrc, 0) == NULL)
	{
		return E_INVALIDARG;
	}

	const int MAX_GREY_COUNT = 256;
	int iHeight = imgSrc.iHeight;
	int iWidth = imgSrc.iWidth;
	int iStrideWidth = imgSrc.iStrideWidth[0];

	//计算直方图
	int rgiImhistCenter[ MAX_GREY_COUNT ] = {0};
	RESTRICT_PBYTE8 pSrcLine = GetHvImageData(&imgSrc, 0);
	for ( int j = 0; j < iHeight; j++, pSrcLine += iStrideWidth)
	{
		for ( int i = 0; i < iWidth; i++ )
		{
			rgiImhistCenter[ pSrcLine[i] ]++;
		}
	}

	//计算阈值
	int iGrey = MAX_GREY_COUNT >> 1;
	while ( true )
	{
		DWORD32 dwTemp1 = 0, dwTemp2 = 0, dwTemp3 = 0, dwTemp4 = 0;
		for ( int i = 0; i <= iGrey; i++ )
			dwTemp1 += rgiImhistCenter[ i ] * i;
		for ( int i = 0; i <= iGrey; i++ )
			dwTemp2 += rgiImhistCenter[ i ];
		for ( int i = iGrey + 1; i < MAX_GREY_COUNT; i++ )
			dwTemp3 += rgiImhistCenter[ i ] * i;
		for ( int i = iGrey + 1; i < MAX_GREY_COUNT; i++ )
			dwTemp4 += rgiImhistCenter[ i ];
		int iGrey2;
		if ( dwTemp2 == 0 && dwTemp4 == 0 )
			iGrey2 = 0;
		else if ( dwTemp2 == 0 )
			iGrey2 = ( int )( dwTemp3 / dwTemp4 );
		else if ( dwTemp4 == 0 )
			iGrey2 = ( int )( dwTemp1 / dwTemp2 );
		else
			iGrey2 = ( int )( ( dwTemp1 / dwTemp2 * iA1 + dwTemp3 / dwTemp4 * iA2 ) / (iA1 + iA2) );
		if ( iGrey2 == iGrey )
			break;
		else
			iGrey = iGrey2;
	}
	*pbThreshold = (BYTE8)iGrey;
	return S_OK;
}

//缩放灰度图
HRESULT ScaleGrey(
    RESTRICT_PBYTE8 pDstImg,
    int nDstWidth,
    int nDstHeight,
	int nDstStride,
    const RESTRICT_PBYTE8 pSrcImg,
    int nSrcWidth,
    int nSrcHeight,
	int nSrcStride)
{
	int iShiftNum = 16;

	int iX, iY;

	SDWORD32 dwXscl, dwYScl;
	SDWORD32 dwXS, dwYS;
	SDWORD32 dwWeightX, dwWeightY;
	SDWORD32 dwNewGray;

	if (nSrcWidth == nDstWidth && nSrcHeight == nDstHeight)
	{
		for (int i = 0; i < nSrcHeight; i++) {
			HV_memcpy(&pDstImg[i * nDstStride], &pSrcImg[i * nSrcStride], nSrcWidth);
		}
		return SUCCEEDED(S_OK);
	}

	//使用量化除法
	dwXscl = (nSrcWidth << iShiftNum) / nDstWidth;			//宽度放大倍数(已量化) 10位
	dwYScl = (nSrcHeight << iShiftNum) / nDstHeight;		//高度放大倍数(已量化)	10位

	int iShift10 = 0;
	if (iShiftNum > 10) {
		iShift10 = iShiftNum - 10;							//量化由16位转10位需要右移次数
	}

	for(int i = 0; i < nDstHeight; i++)
	{
		for(int j = 0; j < nDstWidth; j++)
		{
			dwXS = dwXscl * j;		//放大后的宽度（量化）
			dwYS = dwYScl * i;		//放大后的高度（量化）
			iX = MIN_INT((dwXS >> iShiftNum), nSrcWidth - 2);		//取整后的宽度（取消量化）
			iY = MIN_INT((dwYS >> iShiftNum), nSrcHeight - 2);		//取整后的高度（取消量化）

			dwWeightX = dwXS - (iX << iShiftNum);	//宽度误差（量化）
			dwWeightY = dwYS - (iY << iShiftNum);	//高度误差（量化）

			SDWORD32 dwWeightX10 = ROUND_SHR_POSITIVE(dwWeightX,iShift10);			//计算10位的WeightX，WeightY，防止溢出
			SDWORD32 dwWeightY10 = ROUND_SHR_POSITIVE(dwWeightY,iShift10);

			//新灰度的计算,dwNewGray(量化)
			int iFirstLine = iY * nSrcStride + iX;
			int iSecondLine = iFirstLine + nSrcStride;

			SDWORD32 dwA = pSrcImg[iFirstLine];
			SDWORD32 dwB = pSrcImg[iFirstLine + 1];
			SDWORD32 dwC = pSrcImg[iSecondLine];
			SDWORD32 dwD = pSrcImg[iSecondLine + 1];

			//简化后的公式，完整的公式看下面注释部分代码
			dwNewGray = (dwA << iShiftNum) + (dwB - dwA) * dwWeightX + (dwC - dwA) * dwWeightY + ROUND_SHR(((dwA + dwD - dwB - dwC) * dwWeightX10 * dwWeightY10), (10 - iShift10));		//以10位量化进行计算
			if (dwNewGray > 0)
			{
				dwNewGray = dwNewGray >> iShiftNum;		// 取消量化
			}
			else
			{
				dwNewGray = 0;							// 新加入小于0恒为0
			}
			dwNewGray = MIN_INT(dwNewGray, 255);		// 不能超过255
			pDstImg[i * nDstStride + j] = (BYTE8)dwNewGray;
		}
	}
	return SUCCEEDED(S_OK);
}

//设置二值图指定的二进制位
static inline void SetBin(
	BYTE8 *pbBin,
	int iPosition,
	bool bValue )
{
	if ( bValue )
		pbBin[ iPosition >> 3 ] |= ( 1 << ( iPosition & 0x07 ) );
	else
		pbBin[ iPosition >> 3 ] &= ~( 1 << ( iPosition & 0x07 ) );
}

// 由中心的灰度计算二值化阈值并返回
int CalcBinaryThreByCenter(const BYTE8 *pbYUV, int iWidth, int iHeight, int iStride, HV_RECT *rtCenter)
{
	const int MAX_GREY_COUNT = 256;
    //计算中心部分的直方图
    int rgiImhistCenter[ MAX_GREY_COUNT ];
	HV_memset( rgiImhistCenter, 0, sizeof( rgiImhistCenter ) );
    for ( int iY = rtCenter->top; iY < rtCenter->bottom; iY++ )
	{
		const BYTE8 *pbLine = pbYUV + iY * iStride;
        for ( int iX = rtCenter->left; iX < rtCenter->right; iX++ )
            rgiImhistCenter[ pbLine[iX] ]++;
	}
	//计算阈值
	int iGrey = MAX_GREY_COUNT >> 1;
	while ( true )
	{
		DWORD32 dwTemp1 = 0, dwTemp2 = 0, dwTemp3 = 0, dwTemp4 = 0;
		for ( int i = 0; i <= iGrey; i++ )
			dwTemp1 += rgiImhistCenter[ i ] * i;
		for ( int i = 0; i <= iGrey; i++ )
			dwTemp2 += rgiImhistCenter[ i ];
		for ( int i = iGrey + 1; i < MAX_GREY_COUNT; i++ )
			dwTemp3 += rgiImhistCenter[ i ] * i;
		for ( int i = iGrey + 1; i < MAX_GREY_COUNT; i++ )
			dwTemp4 += rgiImhistCenter[ i ];
		int iGrey2;
		if ( dwTemp2 == 0 && dwTemp4 == 0 )
			iGrey2 = 0;
		else if ( dwTemp2 == 0 )
			iGrey2 = ( int )( dwTemp3 / dwTemp4 );
		else if ( dwTemp4 == 0 )
			iGrey2 = ( int )( dwTemp1 / dwTemp2 );
		else
			iGrey2 = ( int )( ( dwTemp1 / dwTemp2 + dwTemp3 / dwTemp4 ) >> 1 );
		if ( iGrey2 == iGrey )
			break;
        else
            iGrey = iGrey2;
	}
	return iGrey;
}

// 由阈值二值化并输出二值化图
int GrayToBinByThre(BOOL fIsWhiteChar, const RESTRICT_PBYTE8 pbYUV, int iWidth, int iHeight, int iStride, int iThre, RESTRICT_PBYTE8 pbBin)
{
	//设置二值图
	int iLen = iWidth * iHeight;
	if ( fIsWhiteChar )
	{
		// 黑底白字
		for ( int i = 0; i < iHeight; i++ )
		{
			for ( int j = 0; j < iWidth; j++ )
			{
				if( pbYUV[i * iStride + j] > iThre)
					SetBin(pbBin, i * iWidth + j, true);
				else
					SetBin(pbBin, i * iWidth + j, false);
			}
		}
	}
	else
	{
		// 白底黑字
		for ( int i = 0; i < iHeight; i++ )
		{
			for ( int j = 0; j < iWidth; j++ )
			{
				if (pbYUV[i * iStride + j] > iThre)
					SetBin(pbBin, i * iWidth + j, false);
				else
					SetBin(pbBin, i * iWidth + j, true);
			}
		}
	}
	return ((iLen >> 3) + 1);
}

//灰度转换成二值化图,根据中心位置计算阈值
HRESULT GrayToBinByCenter(int fIsWhiteChar, RESTRICT_PBYTE8 pbBin, const RESTRICT_PBYTE8 pbGrey,
						  int iWidth, int iHeight, int iStride, HV_RECT *rtCenter)
{
	int iGrey = CalcBinaryThreByCenter(pbGrey, iWidth, iHeight, iStride, rtCenter);

	//设置二值图
	for ( int j = 0; j < iHeight; j++ )
	{
		PBYTE8 pbSrcLine = pbGrey + j * iStride;
		PBYTE8 pbDstLine = pbBin + j * iStride;
		for ( int i = 0; i < iWidth; i++ )
		{
			if ( pbSrcLine[i] > iGrey )
			{
				pbDstLine[i] = (fIsWhiteChar ? 255 : 0);
			}
			else
			{
				pbDstLine[i] = (fIsWhiteChar ? 0 : 255);
			}
		}
	}
	return S_OK;
}

#define ItgImageBox(a, imgItg,i1,j1,i2,j2)		\
	{											\
		(a) = (imgItg)[(i2)+1][(j2)+1];			\
		(a) -= (imgItg)[(i1)][(j2)+1];			\
		(a) -= (imgItg)[(i2)+1][(j1)];			\
		(a) += (imgItg)[(i1)][(j1)];			\
	}

const int Window_Size = 2;	//上下，左右各3个pixel
const int ScaleFactor = 3;

//////////////////////////////////////////////////////////////////////////
//
//	Function: hvImageResizeGray()
//
//	Author: 黄学雷
//	Written: 2005/3/26
//
//	Description: 图像大小规格化（变为标准的14*14或 16*16）
//	Returns:
//		S_OK --- normalize successfully
//
//////////////////////////////////////////////////////////////////////////
HRESULT hvImageResizeGray(HV_COMPONENT_IMAGE *hvDst, HV_COMPONENT_IMAGE *hvSrc)
{
	int nSrcWidth = hvSrc->iWidth;
	int nSrcHeight = hvSrc->iHeight;
	int nSrcStride = hvSrc->iStrideWidth[0];
	BYTE8  *pSrcImg = GetHvImageData(hvSrc, 0);

	int nDstWidth = hvDst->iWidth;
	int nDstHeight = hvDst->iHeight;
	int nDstStride = hvDst->iStrideWidth[0];
	BYTE8  *pDstImg = GetHvImageData(hvDst, 0);

	return ScaleGrey(pDstImg, nDstWidth, nDstHeight, nDstStride, pSrcImg, nSrcWidth, nSrcHeight, nSrcStride);
}

//////////////////////////////////////////////////////////////////////////
//
//	Function: hvNormalizeMeanVar()
//
//	Author: 黄学雷
//	Written: 2005/3/26
//
//	Description: 标准归一化算法
//	Returns:
//		S_OK --- normalize successfully
//
//////////////////////////////////////////////////////////////////////////
HRESULT hvNormalizeMeanVar(				//只考虑最大16* 16的点阵
	HV_COMPONENT_IMAGE *pImageNormalized,	// normalized image, buffer should be located outside
	const HV_COMPONENT_IMAGE *pImageSrc,	// source image
	int fltMean,				// mean of normalized image
	int fltVar)					// Var. of normalized image
{
	//输入检查
	if(!pImageSrc || !pImageNormalized)
	{
		return false;
	}

	int nWidth = pImageSrc->iWidth;
	int nHeight = pImageSrc->iHeight;
	int nStride = pImageSrc->iStrideWidth[0];
	int nSize = nWidth * nHeight;							//(196~256)

	//  计算当前图像的 mean 和 variance
	BYTE8 * pLine = GetHvImageData(pImageSrc, 0);
	DWORD32 dwSumMean = 0;
	DWORD32 dwSumVar  = 0;

	for(int i = 0; i < nHeight; i++)
	{
		const BYTE8 * pTmp = pLine;
		for(int j = 0; j < nWidth; j++)
		{
			dwSumMean += pTmp[j];							//未量化 (平均值)
			dwSumVar += pTmp[j] * pTmp[j];					//未量化
		}
		pLine += nStride;
	}

	DWORD32 dwImMean = ROUND_DIV_POSITIVE((dwSumMean << 8), (nSize >> 2));								//量化(均值) 10位

	DWORD32 dwImVar = (dwSumVar << 8) - ((dwSumMean * dwSumMean / (nSize >> 2)) << 6);					//8位量化
	dwImVar = ((dwImVar << 2) / (nSize - 1));				//10位量化,(最大位数)		ROUND_DIV有BUG
	dwImVar = quanSqrt(dwImVar, 10);						//10位量化(方差)

	SDWORD32 dwOffSet = fltMean;							// mean value of normalized image
	SDWORD32 dwScale(0);
	if(dwImVar != 0)
	{
		dwScale = (fltVar << 20) / dwImVar;					// 10位量化
	}

	pLine = GetHvImageData(pImageSrc, 0);
	BYTE8 * pNormalizedLine = GetHvImageData(pImageNormalized, 0);
	for(int i = 0; i < nHeight; i++)
	{
		const BYTE8 * pTmp = pLine;
		for(int j = 0; j < nWidth; j++)
		{
			SDWORD32 dwPixelVal = (((SDWORD32)pTmp[j] << 10) - dwImMean) * dwScale;
			dwPixelVal = (dwPixelVal >> 20) + dwOffSet;

			if( dwPixelVal > 255 )
			{
				dwPixelVal = 255;							// limit within [0, 255]
			}
			else if( dwPixelVal < 0 )
			{
				dwPixelVal = 0;								// limit within [0, 255]
			}
			pNormalizedLine[j] = BYTE8 (dwPixelVal);		// assign value
		}
		pLine += nStride;
		pNormalizedLine += nStride;
	}

	return SUCCEEDED(S_OK);
}

//////////////////////////////////////////////////////////////////////////
//
//	Function: hvLocalNormalizeMeanVar()
//
//	Author: 黄学雷
//	Written: 2005/3/26
//
//	Description: 局部均值方差增强	g(x,y) = k*Mean/var(x,y)*[f(x,y) - m(x,y] + m(x,y);
//	Returns:
//		S_OK --- normalize successfully
//
//////////////////////////////////////////////////////////////////////////
HRESULT hvLocalNormalizeMeanVar(
	HV_COMPONENT_IMAGE *pImageNormalized,	// normalized image, buffer should be located outside
	const HV_COMPONENT_IMAGE *pImageSrc,	// source image
	int fltMean,				// mean of normalized image
	int fltVar)
{
	// Check the input first
	if(!pImageSrc || !pImageNormalized)
	{
		return false;
	}

	int nWidth = pImageSrc->iWidth;
	int nHeight = pImageSrc->iHeight;
	int nStride = pImageSrc->iStrideWidth[0];
	int nSize = nWidth * nHeight;

	// 均值，方差积分图
	DWORD32 adwMeanData[17][17];
	DWORD32 adwVarData[17][17];
	int mHeight = nHeight + 1;
	int mWidth = nWidth + 1;

	for (int i=0; i<mHeight; i++)
	{
		adwMeanData[i][0] = 0;
		adwVarData[i][0] = 0;
	}
	for (int j=0; j<mWidth; j++)
	{
		adwMeanData[0][j] = 0;
		adwVarData[0][j] = 0;
	}

    BYTE8 * pSrc = GetHvImageData(pImageSrc, 0);
	for(int i = 1; i < mHeight; ++i)
	{
		for(int j = 1; j < mWidth; ++j)
		{
			adwMeanData[i][j] =
				(SDWORD32)pSrc[j-1] + adwMeanData[i][j-1] + adwMeanData[i-1][j] - adwMeanData[i-1][j-1];
			adwVarData[i][j] =
				(SDWORD32)pSrc[j-1] * pSrc[j-1] + adwVarData[i][j-1] + adwVarData[i-1][j] - adwVarData[i-1][j-1];
		}
		pSrc += nStride;
	}

	// 计算全局均值
	DWORD32 dwAllMean = 0;
	DWORD32 dwLocalImMean = 0;
	DWORD32 dwLocalImVar = 0;
	ItgImageBox(dwAllMean, adwMeanData, 0, 0, nHeight - 1, nWidth - 1);			//最大为256个象素的和
	dwAllMean = (DWORD32)((dwAllMean << 16)/ nSize);							//最大255,10位量化

	//计算每个窗口的均值方差
	int nLeft, nRight, nTop, nBottom;
	int nWidowHeight, nWindowWidth;

	SDWORD32 dwLocalScale(0);

	SDWORD32 tempImg[16 * 16];		//缓冲区用数组实现

	BYTE8 * pLine = GetHvImageData(pImageSrc, 0);
	SDWORD32 *pNormalizedLine = tempImg;

	for(int i = 0; i < nHeight; i++)
	{
		nTop = i - Window_Size;
		if(nTop < 0) nTop = 0;
		nBottom = i + Window_Size;
		nBottom = nBottom < nHeight ? nBottom : nHeight-1;
		nWidowHeight = nBottom - nTop + 1;						//窗口高度，3--5,窗口为3*3 -5*5
		for(int j = 0; j < nWidth; j++)
		{
			nLeft = j - Window_Size;
			if(nLeft < 0) nLeft = 0;
			nRight = j + Window_Size;
			nRight = nRight < nWidth ? nRight : nWidth - 1;
			nWindowWidth = nRight - nLeft + 1;					//窗口宽度,3--5
			int iWindowSize = nWidowHeight * nWindowWidth;		//当前窗口的大小

			//计算 local mean
			ItgImageBox(dwLocalImMean, adwMeanData, nTop, nLeft, nBottom, nRight);		//求窗口内所有点的均值，最大为(25*255)

			// local variance
			ItgImageBox(dwLocalImVar, adwVarData, nTop, nLeft, nBottom, nRight);		//求窗口内所有点的方差

			dwLocalImVar = ((dwLocalImVar - dwLocalImMean) << 10) / iWindowSize;		// iWindowSize;
			dwLocalImVar = quanSqrt(dwLocalImVar, 10);								//量化平方根,10位量化
			dwLocalImMean = (dwLocalImMean << 8) / iWindowSize;							//8量化dwLocalImMean,最大为255

			if(dwLocalImVar <  12288)						//12 * 量化因子(10量化为1024)
			{
				pNormalizedLine[j] = dwLocalImMean;			//量化的 pNormalizedLine
			}
			else
			{
				dwLocalScale = (dwAllMean * ScaleFactor) << 2;					//18位
				dwLocalScale = dwLocalScale / dwLocalImVar;						//8位量化的dwLocalScale

				SDWORD32 dwTmp = ((SDWORD32)pLine[j] << 8) - dwLocalImMean;
				dwTmp = ROUND_SHR_POSITIVE((dwTmp * dwLocalScale), 8) + dwLocalImMean;
				pNormalizedLine[j] = dwTmp;										//结果使用8量化    65*255(最大取值)

			}
		}
		pLine += nStride;
		pNormalizedLine += nStride;
	}

	// 归一化
	SQWORD64 dwSumVar  = 0;
	SDWORD32 dwSumMean = 0;

	DWORD32 dwImVar  = 0;				//无符号数
	SDWORD32 dwImMean = 0;				//无符号数
	SDWORD32 *pTempLine = tempImg;

	for(int i = 0; i < nHeight; i++)
	{
		for(int j = 0; j < nWidth; j++)
		{
			dwSumMean += pTempLine[j];							//8位量化
			//经测试，采用7位量化可保证精度，且1张牌平均不到1条64位乘法指令
			DWORD32 dwTmp = ROUND_SHR_POSITIVE(HV_ABS(pTempLine[j]), 1);			//根据测试，平方前至少保持6位量化才能保证精度，可选范围（6－8）
			if (dwTmp < 0x10000) {
				dwSumVar += ((dwTmp * dwTmp) >> 6);
			} else {
				SQWORD64 qwTmp = dwTmp;							//这里为了保持精度，在平方之前至少保持6位量化，平方后为8为量化
				dwSumVar += ((qwTmp * qwTmp) >> 6);
			}
		}
		pTempLine += nStride;
	}

	if (dwSumVar >= 0xffffffff )			//防止溢出
	{
		dwImVar = 0xffffffff;
	} else {
		dwImVar = (DWORD32)dwSumVar;
	}
	if (dwSumMean >= 0x1000000)				//防止溢出
	{
		dwImMean = 0xffffff;
	} else {
		dwImMean = (DWORD32)dwSumMean;
	}

	SQWORD64 qwTmp = dwImMean;
	qwTmp = ROUND_SHR_POSITIVE(qwTmp, 4);									//4位量化
	qwTmp = qwTmp * qwTmp;
	qwTmp = qwTmp / nSize;								//8位量化
	qwTmp = dwSumVar - qwTmp;
	dwImVar = (DWORD32)qwTmp / (nSize - 1);

	dwImVar = quanSqrt(dwImVar, 8);					//8位量化(方差)
	dwImMean = ROUND_DIV_POSITIVE(dwImMean, nSize);		//量化(均值) 8位

	//计算刻度
	SDWORD32 dwOffSet = fltMean	;						// mean value of normalized image 8位量化
	SDWORD32 dwScale;
	if (dwImVar != 0)
	{
		dwScale =  fltVar;
		dwScale = (dwScale << 16) / dwImVar;			//8位(量化)  normalized image will have variance of 300
	} else {
		dwScale = -1 << 8;								//8 量化,根据和浮点的结果得出的值
	}

	pTempLine = tempImg;
	pLine = GetHvImageData(pImageNormalized, 0);
	for(int i = 0; i < nHeight; i++)
	{
		for(int j = 0; j < nWidth; j++)
		{
			if (dwScale < 0) {
				pLine[j] = 0;						// assign value
			} else {
				SDWORD32 dwPixelVal = pTempLine[j]  - dwImMean ;				//8位量化

				dwPixelVal = (dwPixelVal * dwScale) >> 16;						//取消量化
				dwPixelVal = dwPixelVal  + dwOffSet;

				if( dwPixelVal > 255 )
				{
					dwPixelVal = 255;				// limit within [0, 255]
				}
				else if( dwPixelVal < 0 )
				{
					dwPixelVal = 0;					// limit within [0, 255]
				}
				pLine[j] = BYTE8 (dwPixelVal);		// assign value
			}
		}
		pTempLine += nStride;
		pLine += nStride;
	}
	return SUCCEEDED(S_OK);
}

HRESULT hvLocalNormalizeMeanVar_IR(
	HV_COMPONENT_IMAGE *pImageNormalized,	// normalized image, buffer should be located outside
	const HV_COMPONENT_IMAGE *pImageSrc,	// source image
	int fltMean,				// mean of normalized image
	int fltVar)
{
	// Check the input first
	if(!pImageSrc || !pImageNormalized)
	{
		return false;
	}

	int nWidth = pImageSrc->iWidth;
	int nHeight = pImageSrc->iHeight;
	int nStride = pImageSrc->iStrideWidth[0];
	int nSize = nWidth * nHeight;

	// 均值，方差积分图
	DWORD32 adwMeanData[17][17];
	DWORD32 adwVarData[17][17];
	int mHeight = nHeight + 1;
	int mWidth = nWidth + 1;

	for (int i=0; i<mHeight; i++)
	{
		adwMeanData[i][0] = 0;
		adwVarData[i][0] = 0;
	}
	for (int j=0; j<mWidth; j++)
	{
		adwMeanData[0][j] = 0;
		adwVarData[0][j] = 0;
	}

    BYTE8 * pSrc = GetHvImageData(pImageSrc, 0);
	for(int i = 1; i < mHeight; ++i)
	{
		for(int j = 1; j < mWidth; ++j)
		{
			adwMeanData[i][j] =
				(SDWORD32)pSrc[j-1] + adwMeanData[i][j-1] + adwMeanData[i-1][j] - adwMeanData[i-1][j-1];
			adwVarData[i][j] =
				(SDWORD32)pSrc[j-1] * pSrc[j-1] + adwVarData[i][j-1] + adwVarData[i-1][j] - adwVarData[i-1][j-1];
		}
		pSrc += nStride;
	}

	// 计算全局均值
	DWORD32 dwAllMean = 0;
	DWORD32 dwLocalImMean = 0;
	DWORD32 dwLocalImVar = 0;
	ItgImageBox(dwAllMean, adwMeanData, 0, 0, nHeight - 1, nWidth - 1);			//最大为256个象素的和
	dwAllMean = (DWORD32)((dwAllMean << 16)/ nSize);							//最大255,10位量化

	//计算每个窗口的均值方差
	int nLeft, nRight, nTop, nBottom;
	int nWidowHeight, nWindowWidth;

	SDWORD32 dwLocalScale(0);

	SDWORD32 tempImg[16 * 16];		//缓冲区用数组实现

	BYTE8 * pLine = GetHvImageData(pImageSrc, 0);
	SDWORD32 *pNormalizedLine = tempImg;

	for(int i = 0; i < nHeight; i++)
	{
//		nTop = i - Window_Size;
// 		if(nTop < 0) nTop = 0;
// 		nBottom = i + Window_Size;
// 		nBottom = nBottom < nHeight ? nBottom : nHeight-1;
// 		nWidowHeight = nBottom - nTop + 1;						//窗口高度，3--5,窗口为3*3 -5*5
// 		for(int j = 0; j < nWidth; j++)
// 		{
// 			nLeft = j - Window_Size;
// 			if(nLeft < 0) nLeft = 0;
// 			nRight = j + Window_Size;
// 			nRight = nRight < nWidth ? nRight : nWidth - 1;
// 			nWindowWidth = nRight - nLeft + 1;					//窗口宽度,3--5
// 			int iWindowSize = nWidowHeight * nWindowWidth;		//当前窗口的大小
		nTop = RANGE_INT(i - Window_Size, 0, nHeight - 5);
		nBottom = nTop + 4;
		nWidowHeight = 5;						//窗口高度 5*5
		for(int j = 0; j < nWidth; j++)
		{
			nLeft = RANGE_INT(j - Window_Size, 0 , nWidth - 5);
			nRight = nLeft + 5 - 1;
			nWindowWidth = 5;
			int iWindowSize = nWidowHeight * nWindowWidth;

			//计算 local mean
			ItgImageBox(dwLocalImMean, adwMeanData, nTop, nLeft, nBottom, nRight);		//求窗口内所有点的均值，最大为(25*255)

			// local variance
			ItgImageBox(dwLocalImVar, adwVarData, nTop, nLeft, nBottom, nRight);		//求窗口内所有点的方差

			dwLocalImVar = ((dwLocalImVar - dwLocalImMean) << 10) / iWindowSize;		// iWindowSize;
			dwLocalImVar = quanSqrt(dwLocalImVar, 10);								//量化平方根,10位量化
			dwLocalImMean = (dwLocalImMean << 8) / iWindowSize;							//8量化dwLocalImMean,最大为255

			if(dwLocalImVar <  12288)						//12 * 量化因子(10量化为1024)
			{
				pNormalizedLine[j] = dwLocalImMean;			//量化的 pNormalizedLine
			}
			else
			{
				dwLocalScale = (dwAllMean * ScaleFactor) << 2;					//18位
				dwLocalScale = dwLocalScale / dwLocalImVar;						//8位量化的dwLocalScale

				SDWORD32 dwTmp = ((SDWORD32)pLine[j] << 8) - dwLocalImMean;
				dwTmp = ROUND_SHR_POSITIVE((dwTmp * dwLocalScale), 8) + dwLocalImMean;
				pNormalizedLine[j] = dwTmp;										//结果使用8量化    65*255(最大取值)

			}
		}
		pLine += nStride;
		pNormalizedLine += nStride;
	}


	// 归一化
	SQWORD64 dwSumVar  = 0;
	SDWORD32 dwSumMean = 0;

	DWORD32 dwImVar  = 0;				//无符号数
	SDWORD32 dwImMean = 0;				//无符号数
	SDWORD32 *pTempLine = tempImg;

	for(int i = 0; i < nHeight; i++)
	{
		for(int j = 0; j < nWidth; j++)
		{
			dwSumMean += pTempLine[j];							//8位量化
			//经测试，采用7位量化可保证精度，且1张牌平均不到1条64位乘法指令
			DWORD32 dwTmp = ROUND_SHR_POSITIVE(HV_ABS(pTempLine[j]), 1);			//根据测试，平方前至少保持6位量化才能保证精度，可选范围（6－8）
			if (dwTmp < 0x10000) {
				dwSumVar += ((dwTmp * dwTmp) >> 6);
			} else {
				SQWORD64 qwTmp = dwTmp;							//这里为了保持精度，在平方之前至少保持6位量化，平方后为8为量化
				dwSumVar += ((qwTmp * qwTmp) >> 6);
			}
		}
		pTempLine += nStride;
	}

	if (dwSumVar >= 0xffffffff)			//防止溢出
	{
		dwImVar = 0xffffffff;
	} else {
		dwImVar = (DWORD32)dwSumVar;
	}
	if (dwSumMean >= 0x1000000)				//防止溢出
	{
		dwImMean = 0xffffff;
	} else {
		dwImMean = (DWORD32)dwSumMean;
	}

	SQWORD64 qwTmp = dwImMean;
	qwTmp = ROUND_SHR_POSITIVE(qwTmp, 4);									//4位量化
	qwTmp = qwTmp * qwTmp;
	qwTmp = qwTmp / nSize;								//8位量化
	qwTmp = dwSumVar - qwTmp;
	dwImVar = (DWORD32)qwTmp / (nSize - 1);

	dwImVar = quanSqrt(dwImVar, 8);					//8位量化(方差)
	dwImMean = ROUND_DIV_POSITIVE(dwImMean, nSize);		//量化(均值) 8位

	//计算刻度
	SDWORD32 dwOffSet = fltMean	;						// mean value of normalized image 8位量化
	SDWORD32 dwScale;
	if (dwImVar != 0)
	{
		dwScale =  fltVar;
		dwScale = (dwScale << 16) / dwImVar;			//8位(量化)  normalized image will have variance of 300
	} else {
		dwScale = -1 << 8;								//8 量化,根据和浮点的结果得出的值
	}

	pTempLine = tempImg;
	pLine = GetHvImageData(pImageNormalized, 0);
	for(int i = 0; i < nHeight; i++)
	{
		// li 加入局部行均值
		int iTop = RANGE_INT(i - 3, 0, nHeight - 7);
		int iDow = iTop + 7;
		int iLeft = 0;
		int iRight = nWidth;
		nSize = (iDow - iTop) * (iRight - iLeft);
		dwSumMean = dwSumVar = 0;
		int *pTempLineEX = tempImg + iTop * nStride;

		for(int m = iTop; m < iDow; m++)
		{
			for(int n = iLeft; n < iRight; n++)
			{
				dwSumMean += pTempLineEX[n];							//8位量化
				//经测试，采用7位量化可保证精度，且1张牌平均不到1条64位乘法指令
				DWORD32 dwTmp = ROUND_SHR_POSITIVE(HV_ABS(pTempLineEX[n]), 1);			//根据测试，平方前至少保持6位量化才能保证精度，可选范围（6－8）
				if (dwTmp < 0x10000) {
					dwSumVar += ((dwTmp * dwTmp) >> 6);
				} else {
					SQWORD64 qwTmp = dwTmp;							//这里为了保持精度，在平方之前至少保持6位量化，平方后为8为量化
					dwSumVar += ((qwTmp * qwTmp) >> 6);
				}
			}
			pTempLineEX += nStride;
		}

		if (dwSumVar >= 0xffffffff)			//防止溢出
		{
			dwImVar = 0xffffffff;
		} else {
			dwImVar = (DWORD32)dwSumVar;
		}
		if (dwSumMean >= 0xffffffff)				//防止溢出
		{
			dwImMean = 0xffffff;
		} else {
			dwImMean = (DWORD32)dwSumMean;
		}

		SQWORD64 qwTmp = dwImMean;
		qwTmp = ROUND_SHR_POSITIVE(qwTmp, 4);									//4位量化
		qwTmp = qwTmp * qwTmp;
		qwTmp = qwTmp / nSize;								//8位量化
		qwTmp = dwSumVar - qwTmp;
		dwImVar = (DWORD32)qwTmp / (nSize - 1);

		dwImVar = quanSqrt(dwImVar, 8);					//8位量化(方差)
		dwImMean = ROUND_DIV_POSITIVE(dwImMean, nSize);		//量化(均值) 8位

		for(int j = 0; j < nWidth; j++)
		{
			if (dwScale < 0) {
				pLine[j] = 0;						// assign value
			} else {
				SDWORD32 dwPixelVal = pTempLine[j]  - dwImMean ;				//8位量化

				dwPixelVal = (dwPixelVal * dwScale) >> 16;						//取消量化
				dwPixelVal = dwPixelVal  + dwOffSet;

				if( dwPixelVal > 255 )
				{
					dwPixelVal = 255;				// limit within [0, 255]
				}
				else if( dwPixelVal < 0 )
				{
					dwPixelVal = 0;					// limit within [0, 255]
				}
				pLine[j] = BYTE8 (dwPixelVal);		// assign value
			}
		}
		pTempLine += nStride;
		pLine += nStride;
	}
	return SUCCEEDED(S_OK);

}

HRESULT IMAGE_sobel_V(
	RESTRICT_PBYTE8 pbIn,
	RESTRICT_PBYTE8 pbOut,
	int iCols,
	int iRows
)
{
	int V, i;
	int i00, i02;
	int i10, i12;
	int i20, i22;
	int w = iCols;
	int w1 = w << 1;

	for ( i = 0; i < iCols * ( iRows - 2 ) - 2; i ++ )
	{
		i00 = pbIn[i ]; i02 = pbIn[i +2];
		i10 = pbIn[i + w]; i12 = pbIn[i + w + 2];
		i20 = pbIn[i + w1]; i22 = pbIn[i + w1 + 2];
		V = i02 + ( i12 << 1 ) + i22 - i00 - ( i10 << 1 ) - i20;
		if ( V < 0 ) V = -V;
		if ( V > 255 ) V = 255;
		pbOut[ i + 1 ] = V;
	}

	return S_OK;
}

HRESULT IMAGE_sobel_V_NEW(
					  RESTRICT_PBYTE8 pbIn,
					  RESTRICT_PBYTE8 pbOut,
					  int iCols,
					  int iRows,
					  int iStride
					  )
{
	int V, i, j;
	int i00, i02;
	int i10, i12;
	int i20, i22;
	int w = iStride;
	int w1 = w << 1;

	PBYTE8 pSrc = pbIn;
	PBYTE8 pDst= pbOut;
	for ( i = 0; i < iRows - 2; i++, pSrc += iStride, pDst += iCols)
	{
		for (j = 0; j < (iCols - 2); j++)
		{
			i00 = pSrc[j]; i02 = pSrc[j + 2];
			i10 = pSrc[j + w]; i12 = pSrc[j + w + 2];
			i20 = pSrc[j + w1]; i22 = pSrc[j + w1 + 2];

			V = i02 + ( i12 << 1 ) + i22 - i00 - ( i10 << 1 ) - i20;
			if ( V < 0 ) V = -V;
			if ( V > 255 ) V = 255;

			pDst[j + 1] = V;
		}
	}

	return S_OK;
}

HRESULT hvImageSobelV(
					  HV_COMPONENT_IMAGE imgSrc,
					  HV_COMPONENT_IMAGE imgDst)
{
	int V, i, j;
	int i00, i02;
	int i10, i12;
	int i20, i22;
	int w = imgSrc.iStrideWidth[0];

	PBYTE8 pSrc = GetHvImageData(&imgSrc, 0) + imgSrc.iStrideWidth[0];
	PBYTE8 pDst = GetHvImageData(&imgDst, 0);
	memset(pDst, 0, imgDst.iWidth);
	pDst += imgDst.iStrideWidth[0];
	for (i = 1; i < imgDst.iHeight - 1; i++)
	{
		pDst[0] = 0;
		pDst[imgDst.iWidth - 1] = 0;
		for (j = 1; j < imgDst.iWidth - 1; j++)
		{
			i00 = pSrc[j - w - 1];		i02 = pSrc[j - w + 1];
			i10 = pSrc[j - 1];			i12 = pSrc[j + 1];
			i20 = pSrc[j + w - 1];		i22 = pSrc[j + w + 1];

			V = i02 + ( i12 << 1 ) + i22 - i00 - ( i10 << 1 ) - i20;
			if ( V < 0 ) V = -V;
			if ( V > 255 ) V = 255;

			pDst[j] = V;
		}
		pDst += imgDst.iStrideWidth[0];
		pSrc += imgSrc.iStrideWidth[0];
	}
	memset(pDst, 0, imgDst.iWidth);
	return S_OK;
}

HRESULT IMAGE_sobel_H(
	RESTRICT_PBYTE8 pbIn,
	RESTRICT_PBYTE8 pbOut,
	int iCols,
	int iRows)
{
	int H, i;
	int i00, i01, i02;
	int i20, i21, i22;
	int w = iCols;

	for ( i = 0; i < iCols * ( iRows - 2 ) - 2; i ++ )
	{
		i00 = pbIn[i]; i01 = pbIn[i + 1]; i02 = pbIn[i + 2];
		i20 = pbIn[i + 2 * w]; i21 = pbIn[i + 2 * w + 1]; i22 = pbIn[i + 2 * w + 2];
		H = i20 + 2 * i21 + i22 - i00 - 2 * i01 - i02;
		if ( H < 0 ) H = -H;
		if ( H > 255 ) H = 255;
		pbOut[ i + 1 ] = H;
	}

	return S_OK;
}

HRESULT IMAGE_sobel_H_New(
					  RESTRICT_PBYTE8 pbIn,
					  RESTRICT_PBYTE8 pbOut,
					  int iCols,
					  int iRows)
{
	int H, i;
	int i00, i01, i02;
	int i20, i21, i22;
	int w = iCols;

	for ( i = 0; i < iCols * ( iRows - 2 ) - 2; i ++ )
	{
		i00 = pbIn[i]; i01 = pbIn[i + 1]; i02 = pbIn[i + 2];
		i20 = pbIn[i + 2 * w]; i21 = pbIn[i + 2 * w + 1]; i22 = pbIn[i + 2 * w + 2];
		H = i20 + 2 * i21 + i22 - i00 - 2 * i01 - i02;
		if ( H < 0 ) H = -H;
		if ( H > 255 ) H = 255;
		pbOut[ i + 1 ] = H;
	}

	return S_OK;
}

HRESULT hvImageSobelH(
					  HV_COMPONENT_IMAGE imgSrc,
					  HV_COMPONENT_IMAGE imgDst)
{
	int H, i, j;
	int i00, i01, i02;
	int i20, i21, i22;
	int w = imgSrc.iStrideWidth[0];

	PBYTE8 pSrc = GetHvImageData(&imgSrc, 0) + imgSrc.iStrideWidth[0];
	PBYTE8 pDst = GetHvImageData(&imgDst, 0);
	memset(pDst, 0, imgDst.iWidth);
	pDst += imgDst.iStrideWidth[0];
	for (i = 1; i < imgDst.iHeight - 1; i++)
	{
		pDst[0] = 0;
		pDst[imgDst.iWidth - 1] = 0;
		for (j = 1; j < imgDst.iWidth - 1; j++)
		{
			i00 = pSrc[j - w - 1];		i01 = pSrc[j - w];		i02 = pSrc[j - w + 1];
			i20 = pSrc[j + w - 1];		i21 = pSrc[j + w];		i22 = pSrc[j + w + 1];

			H = i20 + 2 * i21 + i22 - i00 - 2 * i01 - i02;
			if ( H < 0 ) H = -H;
			if ( H > 255 ) H = 255;

			pDst[j] = H;
		}
		pDst += imgDst.iStrideWidth[0];
		pSrc += imgSrc.iStrideWidth[0];
	}
	memset(pDst, 0, imgDst.iWidth);
	return S_OK;
}

HRESULT IMAGE_sobel_New(
					  RESTRICT_PBYTE8 pbIn,
					  RESTRICT_PBYTE8 pbOut,
					  int iCols,
					  int iRows)
{
#ifdef CHIP_6455 //(defined (CHIP_6455) || defined (CHIP_6467))
	IMG_sobel(pbIn, pbOut, iCols, iRows);
#else
	int O, V, H;
	int i00, i01, i02;
	int i10, i12;
	int i20, i21, i22;
	int w = iCols;

	for (int i = 0; i < iCols * ( iRows - 2 ) - 2; i ++ )
	{
		i00 = pbIn[i];			i01 = pbIn[i + 1];			i02 = pbIn[i + 2];
		i10 = pbIn[i + w];									i12 = pbIn[i + w + 2];
		i20 = pbIn[i + 2 * w];	i21 = pbIn[i + 2 * w + 1];	i22 = pbIn[i + 2 * w + 2];

		H = i20 + ( i21 << 1 ) + i22 - i00 - ( i01 << 1 ) - i02;
		if ( H < 0 ) H = -H;
		if ( H > 255 ) H = 255;

		V = i02 + ( i12 << 1 ) + i22 - i00 - ( i10 << 1 ) - i20;
		if ( V < 0 ) V = -V;
		if ( V > 255 ) V = 255;

		O = H + V;
		if (O > 255) O = 255;
		pbOut[i + 1] = O;
	}
#endif
	//add by: qinyj for: sobel修正
	for (int i = iCols * ( iRows - 2 ) - 2; i < iCols * iRows; i++)
	{
		pbOut[i] = 0;
	}

	return S_OK;
}

// 厦门隧道内专用
HRESULT IMAGE_sobel_New_XM(
						RESTRICT_PBYTE8 pbIn,
						RESTRICT_PBYTE8 pbOut,
						int iCols,
						int iRows)
{
	int O, V, H;
	int i00, i01, i02;
	int i10, i12;
	int i20, i21, i22;
	int w = iCols;

	for (int i = 0; i < iCols * ( iRows - 2 ) - 2; i ++ )
	{
		i00 = pbIn[i];			i01 = pbIn[i + 1];			i02 = pbIn[i + 2];
		i10 = pbIn[i + w];									i12 = pbIn[i + w + 2];
		i20 = pbIn[i + 2 * w];	i21 = pbIn[i + 2 * w + 1];	i22 = pbIn[i + 2 * w + 2];

		H = i20 + ( i21 << 1 ) + i22 - i00 - ( i01 << 1 ) - i02;
		if ( H < 0 ) H = -H;
		if ( H > 255 ) H = 255;

		V = i02 + ( i12 << 1 ) + i22 - i00 - ( i10 << 1 ) - i20;
		if ( V < 0 ) V = -V;
		if ( V > 255 ) V = 255;

		O = (H > V ? H : V);
		if (O > 255) O = 255;
		pbOut[i + 1] = O;
	}

	//add by: qinyj for: sobel修正
	for (int i = iCols * ( iRows - 2 ) - 2; i < iCols * iRows; i++)
	{
		pbOut[i] = 0;
	}

	return S_OK;
}

//切图
HRESULT CropImage(
	const HV_COMPONENT_IMAGE &imgInput,
	HV_RECT& rcCrop,
	HV_COMPONENT_IMAGE *pimgOutput
)
{
	if (rcCrop.left < 0 || rcCrop.top < 0 ||
		rcCrop.right < rcCrop.left || rcCrop.bottom < rcCrop.top ||
		rcCrop.right > imgInput.iWidth ||
  		rcCrop.bottom > imgInput.iHeight)
	{
		return E_INVALIDARG;
	}
	rcCrop.left &= ~1;

	int rgiOffset[3] = {0};
	switch (imgInput.nImgType)
	{
	case HV_IMAGE_YUV_422:
		rgiOffset[0] = imgInput.iStrideWidth[0] * rcCrop.top + rcCrop.left;
		rgiOffset[1] = rgiOffset[2] = (rgiOffset[0] >> 1);
		break;
	case HV_IMAGE_YCbYCr:
	case HV_IMAGE_CbYCrY:
		rgiOffset[0] = imgInput.iStrideWidth[0] * rcCrop.top + (rcCrop.left << 1);
		rgiOffset[1] = rgiOffset[2] = 0;
		break;
	case HV_IMAGE_BT1120:
	case HV_IMAGE_BT1120_FIELD:
		rgiOffset[0] = imgInput.iStrideWidth[0] * rcCrop.top + rcCrop.left;
		rgiOffset[1] = imgInput.iStrideWidth[1] * rcCrop.top + rcCrop.left;
		rgiOffset[1] &= ~1;
		rgiOffset[2] = 0;
		break;
	// HV_IMAGE_BT1120_ROTATE_Y格式只切Y分量
	case HV_IMAGE_BT1120_ROTATE_Y:
		rgiOffset[0] = imgInput.iStrideWidth[0] * rcCrop.top + rcCrop.left;
		rgiOffset[1] = 0;
		rgiOffset[2] = 0;
		break;
	case HV_IMAGE_YUV_420:
		rgiOffset[0] = imgInput.iStrideWidth[0] * rcCrop.top + rcCrop.left;
		rgiOffset[1] = imgInput.iStrideWidth[1] * (rcCrop.top / 2) + (rcCrop.left);
		rgiOffset[1] &= ~1;
		rgiOffset[2] = 0;
		break;
	default:
		return E_INVALIDARG;
	}

	//同步类型信息
	//指针后面会赋值,故可以直接拷贝
	*pimgOutput = imgInput;

	SetHvImageData(pimgOutput, 0, GetHvImageData(&imgInput, 0) + rgiOffset[0]);
	SetHvImageData(pimgOutput, 1, GetHvImageData(&imgInput, 1) + rgiOffset[1]);
	SetHvImageData(pimgOutput, 2, GetHvImageData(&imgInput, 2) + rgiOffset[2]);
	pimgOutput->iWidth = rcCrop.right - rcCrop.left;
	pimgOutput->iHeight = rcCrop.bottom - rcCrop.top;

	return S_OK;
}

//使用全局阈值进行二值图转换
HRESULT IMAGE_Gray2Bin( const HV_COMPONENT_IMAGE *pImg, BYTE8 *pbBin, int *piThreshold = NULL )
{
	if (pbBin == NULL || pImg == NULL)
	{
		return E_POINTER;
	}
	const int MAX_GRAY_COUNT = 256;

	int iWidth = pImg->iWidth;
	int iHeight = pImg->iHeight;
	int iGrayStride = pImg->iStrideWidth[0];
	BYTE8* pbY = GetHvImageData(pImg, 0);

	//计算直方图
	int aiImhistCenter[ MAX_GRAY_COUNT ] = {0};

	for ( int iY = 0, iPosY = iY * iGrayStride; iY < iHeight;
		iY++, iPosY += iGrayStride )
	{
		for ( int iX = 0; iX < iWidth; iX++ )
		{
			aiImhistCenter[ pbY[ iPosY + iX ] ]++;
		}
	}

	int iPixelNum = iHeight * iWidth;
	int itemp(0);
	int iGray(128);
	for ( int i = 255; i > 0; i-- )
	{
		itemp += aiImhistCenter[i];
		if ( itemp > ( iPixelNum >> 3 ) )
		{
			iGray = i;
			break;
		}
	}
	//计算阈值
	while ( 1 )
	{
		int iTemp1 = 0, iTemp2 = 0, iTemp3 = 0, iTemp4 = 0;
		for ( int i = 0; i <= iGray; i++ )
		{
			iTemp1 += aiImhistCenter[ i ] * i;
		}
		for ( int i = 0; i <= iGray; i++ )
		{
			iTemp2 += aiImhistCenter[ i ];
		}
		for ( int i = iGray + 1; i < MAX_GRAY_COUNT; i++ )
		{
			iTemp3 += aiImhistCenter[ i ] * i;
		}
		for ( int i = iGray + 1; i < MAX_GRAY_COUNT; i++ )
		{
			iTemp4 += aiImhistCenter[ i ];
		}
		if ( 0 == iTemp2 || 0 == iTemp4 )
		{
			break;
		}
		int iGray2 = ( ( (int)( (double)iTemp1 / iTemp2 +
			(double)iTemp3 / iTemp4 ) ) >> 1 );
		if ( iGray2 == iGray )
		{
			break;
		}
		else
		{
			iGray = iGray2;
		}
	}
	if (piThreshold)
	{
		*piThreshold = iGray;
	}
	//设置二值图
	for ( int iY = 0, iBinPosY = 0, iPosY = 0 ; iY < iHeight ;
		iY++, iBinPosY += iWidth, iPosY += iGrayStride )
	{
		for ( int iX = 0,iGrayX = 0; iX < iWidth; iX++, iGrayX++ )
		{
			if ( pbY[ iPosY+iGrayX ] > iGray )
			{
				pbBin[iBinPosY + iX] = 255;
			}
			else
			{
				pbBin[iBinPosY + iX] = 0;
			}
		}
	}
	return S_OK;
}

//将H,S,V分量分开返回
HRESULT IMAGE_ConvertYCbCr2HSV(
						 const HV_COMPONENT_IMAGE *pYCbCrImg, //输入
						 BYTE8* pbH,	//H分量
						 BYTE8* pbS,	//S分量
						 BYTE8* pbV,	//V分量
						 BYTE8* pbSat_H = NULL,	//各分量统计直方图(256), 及附加信息,
						 BYTE8* pbSat_S = NULL,	//最大值(1),最小值(1),总和(4),
						 BYTE8* pbSat_V = NULL		//由调用者负责缓存清零
						 )
{
	int iPosX, iHeight, iWidth, iStride;
	RESTRICT_PBYTE8 pbY;
	RESTRICT_PBYTE8 pbCb;
	RESTRICT_PBYTE8 pbCr;
	int iRed, iGreen, iBlue, iY, iCb, iCr, iMax, iMin;
	int iMaxH(0), iMinH(256), iMaxS(0), iMinS(256), iMaxV(0), iMinV(256);
	int iSumH(0), iSumS(0), iSumV(0);

	if ( ( pYCbCrImg == NULL ) || ( pbH == NULL ) || (pbS == NULL) || (pbV == NULL) )
	{
		return E_POINTER;
	}

	iWidth = pYCbCrImg->iWidth;

	pbY = GetHvImageData(pYCbCrImg, 0);
	pbCb = GetHvImageData(pYCbCrImg, 1);
	pbCr = GetHvImageData(pYCbCrImg, 2);
	if ( ( pbY == NULL ) || ( pbCb == NULL ) || ( pbCr == NULL ) )
	{
		return E_INVALIDARG;
	}
	iHeight = pYCbCrImg->iHeight;
	iStride = pYCbCrImg->iStrideWidth[0];
	for ( ; iHeight; iHeight --, pbY += iStride, pbCb += iStride >> 1, pbCr += iStride >> 1 )
	{
		for ( iPosX = 0; iPosX < iWidth; iPosX ++, pbH++, pbS++, pbV++ )
		{
			iY = pbY[ iPosX ];
			iCb = ( ( int )pbCb[ iPosX >> 1 ] ) - 128;
			iCr = ( ( int )pbCr[ iPosX >> 1 ] ) - 128;
			iRed = iY + ( ( 22970 * iCr ) >> 14 );
			iGreen = iY - ( ( 5638 * iCb + 11700 * iCr ) >> 14 );
			iBlue = iY + ( ( 29032 * iCb ) >> 14 );
			iRed = RANGE_INT( iRed, 0, 255 );
			iGreen = RANGE_INT( iGreen, 0, 255 );
			iBlue = RANGE_INT( iBlue, 0, 255 );
			if ( iRed > iGreen )
			{
				iMax = MAX_INT( iRed, iBlue );
				iMin = MIN_INT( iGreen, iBlue );
			}
			else
			{
				iMax = MAX_INT( iGreen, iBlue );
				iMin = MIN_INT( iRed, iBlue );
			}
			*pbV = iMax;

			iSumV += *pbV;
			iMaxV = MAX_INT(iMaxV, *pbV);
			iMinV = MIN_INT(iMinV, *pbV);

			if (pbSat_V) pbSat_V[*pbV]++;

			iMin = iMax - iMin;
			if ( iMax )
			{
				iCb = ROUND_DIV_POSITIVE( ( iMin << 8 ) - iMin, iMax );
				*pbS = MIN_INT( iCb, 255 );

				iSumS += *pbS;
				iMaxS = MAX_INT(iMaxS, *pbS);
				iMinS = MIN_INT(iMinS, *pbS);

				if (pbSat_S)  pbSat_S[*pbS]++;

				if ( *pbS )
				{
					iCb = iMin << 2;
					iCr = iMin << 1;
					iMin = iCb + iCr;
					if ( iRed == iMax )
					{
						if ( iGreen >= iBlue )
						{
							iMax = iGreen - iBlue;
						}
						else
						{
							iMax = iMin + iGreen - iBlue;
						}
					}
					else
					{
						if ( iGreen == iMax )
						{
							iMax = iCr + iBlue - iRed;
						}
						else
						{
							iMax = iCb + iRed - iGreen;
						}
					}
					iMax = ROUND_DIV_POSITIVE( ( iMax << 8 ) - iMax, iMin );
					*pbH = MIN_INT( iMax, 255 );

					iSumH += *pbH;
					iMaxH = MAX_INT(iMaxH, *pbH);
					iMinH = MIN_INT(iMinH, *pbH);

					if (pbSat_H)  pbSat_H[*pbH]++;
				}
				else
				{
					*pbH = 0;
				}
			}
			else
			{
				*pbS = 0;
				*pbH = 0;
			}
		}
	}

	if (pbSat_H)
	{
		pbSat_H[256] = iMaxH;
		pbSat_H[257] = iMinH;
		*(int*)(pbSat_H+258) = iSumH;
	}

	if (pbSat_S)
	{
		pbSat_S[256] = iMaxS;
		pbSat_S[257] = iMinS;
		*(int*)(pbSat_S+258) = iSumS;
	}

	if (pbSat_V)
	{
		pbSat_V[256] = iMaxV;
		pbSat_V[257] = iMinV;
		*(int*)(pbSat_V+258) = iSumV;
	}

	return S_OK;
}

//将R,G,B分量分开返回
HRESULT IMAGE_ConvertYCbCr2BGR( const HV_COMPONENT_IMAGE *pYCbCrImg, BYTE8* pbR, BYTE8* pbG, BYTE8* pbB,  int iBGRStride = 0)
{
	int iPosX, iHeight, iWidth, iStride, iTemp;
	RESTRICT_PBYTE8 pbY;
	RESTRICT_PBYTE8 pbCb;
	RESTRICT_PBYTE8 pbCr;

	if ( ( pYCbCrImg == NULL ) || ( pbR == NULL ) || ( pbG == NULL ) || ( pbB == NULL ) ) return E_POINTER;

	iWidth = pYCbCrImg->iWidth;
	if ( iBGRStride == 0) iBGRStride = iWidth;
	if ( iWidth > iBGRStride ) return E_INVALIDARG;

	pbY = GetHvImageData(pYCbCrImg, 0);
	pbCb = GetHvImageData(pYCbCrImg, 1);
	pbCr = GetHvImageData(pYCbCrImg, 2);
	if ( ( pbY == NULL ) || ( pbCb == NULL ) || ( pbCr == NULL ) ) return E_INVALIDARG;

	iHeight = pYCbCrImg->iHeight;
	iStride = pYCbCrImg->iStrideWidth[0];

	for ( ; iHeight; iHeight --, pbB += iBGRStride, pbG += iBGRStride, pbR += iBGRStride, pbY += iStride, pbCb += iStride >> 1, pbCr += iStride >> 1 )
	{
		for ( iPosX = 0; iPosX < iWidth; iPosX ++ )
		{
			iTemp = ( ( ( int )pbY[ iPosX ] ) << 14 ) + 22970 * ( ( ( int )( pbCr[ iPosX >> 1 ] ) ) - 128 );
			iTemp = ROUND_SHR( iTemp, 14 );
			pbR[iPosX] = ( BYTE8 )RANGE_INT( iTemp, 0, 255 );

			iTemp = ( ( ( int )pbY[ iPosX ] ) << 14 ) - 5638 * ( ( ( int )( pbCb[ iPosX >> 1 ] ) ) - 128 )
				- 11700 * ( ( ( int )( pbCr[ iPosX >> 1 ] ) ) - 128 );
			iTemp = ROUND_SHR( iTemp, 14 );
			pbG[iPosX] = ( BYTE8 )RANGE_INT( iTemp, 0, 255 );

			iTemp = ( ( ( int )pbY[ iPosX ] ) << 14 ) + 29032 * ( ( ( int )( pbCb[ iPosX >> 1 ] ) ) - 128 );
			iTemp = ROUND_SHR( iTemp, 14 );
			pbB[iPosX] = ( BYTE8 )RANGE_INT( iTemp, 0, 255 );
		}
	}
	return S_OK;
}

extern HRESULT IntegralPreprocess(
						   PDWORD32 pdwItgImage,
						   PDWORD32 pdwItgSqImage,
						   WORD16 &wItgWidth,
						   WORD16 &wItgHeight,
						   WORD16 nItgStrideWidth,
						   HV_COMPONENT_IMAGE *pImageSrc 				//指向原始图象数据.
						   );

#define AREA_FROM_2D_ARRAY( dwaItg, iTop, iLeft, iHeight, iWidth , iStride ) \
	(*(dwaItg + (iTop + iHeight) * iStride + iLeft + iWidth) - \
	*(dwaItg + iTop * iStride + iLeft + iWidth) - \
	*(dwaItg + (iTop + iHeight) * iStride + iLeft) + \
	*(dwaItg + iTop * iStride + iLeft))

#define SEGMENT_FLOAT_SHR 10
#define SEG_BIN_DIFF_THRESHOLD			30
#define SEG_BIN_WHITE_THRESHOLD			90
#define SEG_BIN_BLACK_THRESHOLD			160
#define SEG_BIN_H_SHRINK_RATIO_WHITE	205
#define SEG_BIN_H_SHRINK_RATIO_BLACK	307
#define SEG_BIN_WIDTH_RATIO_BLACK		512
#define SEG_BIN_V_SHRINK_RATIO			205
#define SEG_BIN_WIDTH_LOCAL_RATIO		164
#define SEG_BIN_HIGH_LOCAL_RATIO		82
int IMAGE_SegLocalMeanBin(
			HV_COMPONENT_IMAGE* pImg,
			BYTE8 *pbBinImg,
			int iIsWhiteChar
			)
{
	int iTop, iLeft, iVarHeight, iVarWidth, iPosX, iPosY, iLastHeight;
	int iVariance, iMean, iTotlePixel, iHalfHeight, iHalfWidth;
	int iHeight, iWidth;
	BYTE8 *pbScan;
	int iGrayThreshold;
	int iDiffThreshold = SEG_BIN_DIFF_THRESHOLD;

	iHeight = pImg->iHeight;
	iWidth = pImg->iWidth;
	int iItgWidth = iWidth + 1;
	int iItgHeight = iHeight + 1;
	int iItgBufLen = iItgWidth * iItgHeight;
	RESTRICT_PBYTE8 pbGrayImg = GetHvImageData(pImg, 0);
	int iGrayStride = pImg->iStrideWidth[0];

	CFastMemAlloc cStack;
	RESTRICT_PDWORD32 pItgBuf = (DWORD32*)cStack.StackAlloc(iItgBufLen*sizeof(DWORD32));
	CreateItgImage(iItgWidth<<2, pItgBuf, iHeight, iWidth, iGrayStride, GetHvImageData(pImg, 0));


	if ( !iIsWhiteChar ) {
		iVarHeight = ROUND_SHR_POSITIVE( iHeight * 614, 10 );
		iTop = ROUND_SHR_POSITIVE( iHeight * 205, 10 );
		iVarWidth = iWidth >> 1;
		iVariance = AREA_FROM_2D_ARRAY( pItgBuf, iTop, 0, iVarHeight, iVarWidth, iItgWidth );
		iVariance /= iVarWidth;
		iPosX = ( iWidth * 51 ) >> SEGMENT_FLOAT_SHR;
		//for ( ; iPosX < iVarWidth ; iPosX ++ ) {
		for ( ; iPosX < (iVarWidth - 1); iPosX ++ ) {
			if ( ( int )( AREA_FROM_2D_ARRAY( pItgBuf, iTop, iPosX, iVarHeight, 1, iItgWidth) ) >= iVariance ) break;
		}
		iHalfWidth = iWidth - iVarWidth;
		iVariance = AREA_FROM_2D_ARRAY( pItgBuf, iTop, iVarWidth, iVarHeight, iHalfWidth, iItgWidth);
		iVariance /= iVarWidth;
		iPosY = ( ( iWidth * 973 ) >> SEGMENT_FLOAT_SHR ) - 1;
		//for ( ; iPosY >= iHalfWidth; iPosY -- ) {
		for ( ; iPosY > iHalfWidth; iPosY -- ) {
			if ( ( int )( AREA_FROM_2D_ARRAY( pItgBuf, iTop, iPosY, iVarHeight, 1, iItgWidth ) ) >= iVariance ) break;
		}
		iLeft = ( iWidth * SEG_BIN_H_SHRINK_RATIO_BLACK ) >> SEGMENT_FLOAT_SHR;
		iVarWidth = ( iWidth * SEG_BIN_WIDTH_RATIO_BLACK ) >> SEGMENT_FLOAT_SHR;
		iPosY -= iPosX - 1;
		iLeft = MAX_INT( iLeft, iPosX );
		iVarWidth = MIN_INT( iVarWidth, iPosY );
	} else {
		iLeft = ( iWidth * SEG_BIN_H_SHRINK_RATIO_WHITE ) >> SEGMENT_FLOAT_SHR;
		iVarWidth = iWidth - ( iLeft << 1 );
	}
	iTop = ( iHeight * SEG_BIN_V_SHRINK_RATIO ) >> SEGMENT_FLOAT_SHR;
	iVarHeight = iHeight - ( iTop << 1 ) - 1;
	iVariance = 0;
	iTotlePixel = iLeft + iVarWidth;
	pbScan = pbGrayImg + iGrayStride * iTop;
	for ( iPosY = 0; iPosY < iVarHeight; iPosY ++, pbScan += iGrayStride ) {
		for ( iPosX = iLeft; iPosX < iTotlePixel; iPosX ++ )
			iVariance += pbScan[ iPosX ] * pbScan[ iPosX ];
	}
	iTotlePixel = iVarHeight * iVarWidth;
	iVariance /= iTotlePixel;
	iMean = AREA_FROM_2D_ARRAY( pItgBuf, iTop, iLeft, iVarHeight, iVarWidth, iItgWidth ) / iTotlePixel;
	iVariance -= iMean * iMean;
	if ( iIsWhiteChar ) {
		iGrayThreshold = ( iMean > SEG_BIN_BLACK_THRESHOLD ) ? iMean : SEG_BIN_WHITE_THRESHOLD;
		iGrayThreshold = MIN_INT( iGrayThreshold, ( ( iGrayThreshold + iMean ) >> 1 ) );
		if ( iVariance < 3025 ) {	//需要改进
			iVariance = SQRT_FROM_TABLE( iVariance );
			iDiffThreshold = ( ( iVariance - 12 ) * ( iDiffThreshold - 1 ) ) / 43;
			iDiffThreshold = MAX_INT( 1 + iDiffThreshold, 1 );
			iGrayThreshold -= 7;
			iVariance = ( ( iVariance - 12 ) * 7 ) / 43;
			iGrayThreshold = ( iVariance <= 0 ) ? iGrayThreshold : iVariance + iGrayThreshold;
		}
	} else {
		if ( iMean > 200 ) iDiffThreshold = MAX_INT( 15, 30 - ( 40 * ( iMean - 200 ) ) / 55 );	//黄牌, 发白
		iGrayThreshold = MAX_INT( SEG_BIN_BLACK_THRESHOLD, iMean );
		if ( iVariance < 3025 ) {	//需要改进
			iVariance = SQRT_FROM_TABLE( iVariance );
			iDiffThreshold = ( ( iVariance - 12 ) * ( iDiffThreshold - 1 ) ) / 43;
			iDiffThreshold = MAX_INT( 1 + iDiffThreshold, 1 );
			iGrayThreshold += 40;
			iVariance = ( ( 10 - iVariance ) * 40 ) / 43;
			iGrayThreshold = ( iVariance >= 0 ) ? iGrayThreshold : iVariance + iGrayThreshold;
		}
	}
	iVarHeight = ( ROUND_SHR_POSITIVE( iHeight * SEG_BIN_HIGH_LOCAL_RATIO, SEGMENT_FLOAT_SHR ) << 1 ) + 1;
	iVarHeight = MAX_INT( iVarHeight, 5 );
	iVarWidth = ( ROUND_SHR_POSITIVE( iHeight * SEG_BIN_WIDTH_LOCAL_RATIO, SEGMENT_FLOAT_SHR ) << 1 ) + 1;
	iVarWidth = MAX_INT( iVarWidth, 5 );
	iHalfHeight = iVarHeight >> 1;
	iHalfWidth = iVarWidth >> 1;
	iLastHeight = iHeight - iVarHeight;
	iVariance = iWidth - iVarWidth;
	iTotlePixel = iVarHeight * iVarWidth;
	pbScan = pbGrayImg;

	if ( iIsWhiteChar ) {
		for ( iPosY = 0; iPosY < iHeight; iPosY ++, pbScan += iGrayStride ) {
			iTop = RANGE_INT( iPosY - iHalfHeight, 0, iLastHeight );
			for ( iPosX = 0; iPosX < iWidth; iPosX ++, pbBinImg ++ ) {
				iLeft = RANGE_INT( iPosX - iHalfWidth, 0, iVariance );
				iMean = AREA_FROM_2D_ARRAY( pItgBuf, iTop, iLeft, iVarHeight, iVarWidth, iItgWidth );
				iMean = ROUND_DIV_POSITIVE( iMean, iTotlePixel );
				*pbBinImg = ( ( pbScan[ iPosX ] > ( iMean + iDiffThreshold ) ) &&
					( pbScan[ iPosX ] > iGrayThreshold ) ) ? 0xFF : 0;
			}
		}
	} else {
		for ( iPosY = 0; iPosY < iHeight; iPosY ++, pbScan += iGrayStride ) {
			iTop = RANGE_INT( iPosY - iHalfHeight, 0, iLastHeight );
			for ( iPosX = 0; iPosX < iWidth; iPosX ++, pbBinImg ++ ) {
				iLeft = RANGE_INT( iPosX - iHalfWidth, 0, iVariance );
				iMean = AREA_FROM_2D_ARRAY( pItgBuf, iTop, iLeft, iVarHeight, iVarWidth, iItgWidth );
				iMean = ROUND_DIV_POSITIVE( iMean, iTotlePixel );
				*pbBinImg = ( ( pbScan[ iPosX ] < ( iMean - iDiffThreshold ) ) &&
					( pbScan[ iPosX ] < iGrayThreshold ) ) ? 0xFF : 0;
			}
		}
	}
	return 0;
}

//#include "HvCrop.h"

#define STD_CALC(a,b,c,m) ( (a-m)*(a-m) + (b-m)*(b-m) + (c-m)*(c-m) )

//by liaoy
//提取车牌颜色
//pColorInfo中需指定字符类型,否则只进行黄牌判断
//结果从pColorInfo中的nPlateColor成员取得
HRESULT GetColorInfo(
	const HV_COMPONENT_IMAGE *pImage,
	COLOR_INFO* pColorInfo )
{
	if ( pImage == NULL || pColorInfo == NULL ) return E_POINTER;

	pColorInfo->nPlateColor = PC_UNKNOWN;
	pColorInfo->Hue_1 = 0;	//字区
	pColorInfo->Sat_1 = 0;
	pColorInfo->Hue_0 = 0;	//底色区
	pColorInfo->Sat_0 = 0;

	if ( pColorInfo->nCharType > 2 ) return E_INVALIDARG;

	int iWidth = pImage->iWidth;
	int iHeight = pImage->iHeight;

	if ( iWidth < 50 || iHeight <  10 ) return E_INVALIDARG;

	//切图
	HV_RECT rcCrop = { iWidth/3, iHeight/4, iWidth*2/3, iHeight*3/4 };

	HV_COMPONENT_IMAGE imgCrop;
	CropImage( *pImage, rcCrop, &imgCrop);

	iWidth = imgCrop.iWidth;
	iHeight = imgCrop.iHeight;
	int iBufSize = iWidth * iHeight;

	//转为二值图
	CFastMemAlloc cStack;
	BYTE8* pbBin = (BYTE8*)cStack.StackAlloc( iBufSize);
	if (pbBin == NULL) return E_OUTOFMEMORY;
	HV_memset(pbBin, 0, iBufSize);

	if( 0 != IMAGE_SegLocalMeanBin(
				&imgCrop,
				pbBin,
				pColorInfo->nCharType == 0
				) )
	{
		return E_FAIL;
	}

	//转为HSV
	RESTRICT_PBYTE8 pbH = (BYTE8*)cStack.StackAlloc(iBufSize);
	RESTRICT_PBYTE8 pbS = (BYTE8*)cStack.StackAlloc(iBufSize);
	RESTRICT_PBYTE8 pbV = (BYTE8*)cStack.StackAlloc(iBufSize);
	RESTRICT_PBYTE8 pbSat_H = (BYTE8*)cStack.StackAlloc(300);
	RESTRICT_PBYTE8 pbSat_S = (BYTE8*)cStack.StackAlloc(300);
	RESTRICT_PBYTE8 pbSat_V = (BYTE8*)cStack.StackAlloc(300);

	if ( !pbH || !pbS || !pbV || !pbSat_H || !pbSat_S || !pbSat_V) return E_OUTOFMEMORY;

	HV_memset( pbH, 0, iBufSize);
	HV_memset( pbS, 0, iBufSize);
	HV_memset( pbV, 0, iBufSize);
	HV_memset( pbSat_H, 0, 300);
	HV_memset( pbSat_S, 0, 300);
	HV_memset( pbSat_V, 0, 300);

	if (FAILED( IMAGE_ConvertYCbCr2HSV(&imgCrop, pbH, pbS, pbV, pbSat_H, pbSat_S, pbSat_V))) return E_FAIL;

	//转为RGB
	RESTRICT_PBYTE8 pbR = (BYTE8*)cStack.StackAlloc(iBufSize);
	RESTRICT_PBYTE8 pbG = (BYTE8*)cStack.StackAlloc(iBufSize);
	RESTRICT_PBYTE8 pbB = (BYTE8*)cStack.StackAlloc(iBufSize);

	if ( !pbR || !pbG || !pbB ) return E_OUTOFMEMORY;

	HV_memset( pbR, 0, iBufSize);
	HV_memset( pbG, 0, iBufSize);
	HV_memset( pbB, 0, iBufSize);

	if (FAILED( IMAGE_ConvertYCbCr2BGR( &imgCrop, pbR, pbG, pbB))) return E_FAIL;

	//RGB方差统计
	int nStdRGB0 = 0;
	int nStdRGB1 = 0;
	int nMeanRGB = 0;
	int nStdRGB = 0;
	int nCount0 = 0;
	int nCount1 = 0;

	//色度值统计
	int nSumH1 = 0;
	int nSumH0 = 0;
	int nSumS1 = 0;
	int nSumS0 = 0;

	int nMeanH1 = 0;
	int nMeanH0 = 0;
	int nMeanS1 = 0;
	int nMeanS0 = 0;

	for (int i = 0; i < iBufSize; i++)
	{
		nMeanRGB = (pbR[i] + pbG[i] + pbB[i])/3;
		nStdRGB = STD_CALC(pbR[i], pbG[i], pbB[i], nMeanRGB);

		if ( pbBin[i] ) //亮点
		{
			nStdRGB1 += nStdRGB;
			nSumH1 += pbH[i];
			nSumS1 += pbS[i];
			nCount1++;
		}
		else
		{
			nStdRGB0 += nStdRGB;
			nSumH0 += pbH[i];
			nSumS0 += pbS[i];
			nCount0++;
		}
	}

	if (nCount1 == 0) nCount1 = 1;
	if (nCount0 == 0) nCount0 = 1;

	nStdRGB1 = nStdRGB1/nCount1;
	nStdRGB0 = nStdRGB0/nCount0;

	nMeanH1 = nSumH1/nCount1;
	nMeanH0 = nSumH0/nCount0;

	nMeanS1 = nSumS1/nCount1;
	nMeanS0 = nSumS0/nCount0;

	pColorInfo->Hue_1 = nMeanH1;
	pColorInfo->Sat_1 = nMeanS1;

	pColorInfo->Hue_0 = nMeanH0;
	pColorInfo->Sat_0 = nMeanS0;

	//色度
	const int YELLOW_H = 35;
	const int YELLOW_SAT = 160;

	const int BLUE_H = 150;
	const int BLUE_SAT = 100;

	int nD1_y = 0;
	int nD0_y = 0;

	int nD1_b = 0;
	int nD0_b = 0;

	for (int i = 0; i < iBufSize; i++)
	{
		if ( pbBin[i] )
		{
			nD1_y += ( pbH[i] - YELLOW_H ) * ( pbH[i] - YELLOW_H) + ( pbS[i] - YELLOW_SAT ) * ( pbS[i] - YELLOW_SAT );
			nD1_b += ( pbH[i] - BLUE_H ) * ( pbH[i] - BLUE_H) + ( pbS[i] - BLUE_SAT ) * ( pbS[i] - BLUE_SAT );
		}
		else
		{
			nD0_y += ( pbH[i] - YELLOW_H ) * ( pbH[i] - YELLOW_H) + ( pbS[i] - YELLOW_SAT ) * ( pbS[i] - YELLOW_SAT );
			nD0_b += ( pbH[i] - BLUE_H ) * ( pbH[i] - BLUE_H) + ( pbS[i] - BLUE_SAT ) * ( pbS[i] - BLUE_SAT );
		}
	}

	nD1_y = nD1_y / nCount1;
	nD0_y = nD0_y / nCount0;

	nD1_b = nD1_b / nCount1;
	nD0_b = nD0_b / nCount0;

	//默认为PC_UNKNOWN
	if ( pColorInfo->nCharType == 0 ) //白字,区分蓝,黑,域值需要调整
	{
		if ( nStdRGB0 < 150 )
		{
			pColorInfo->nPlateColor = PC_BLACK;
		}
		else if ( ( nStdRGB0 < 500 ) && MIN_INT( nD0_b, nD0_y ) > 6000 )
		{
			pColorInfo->nPlateColor = PC_BLACK;
		}
		else if( nD0_b < MIN_INT(nD0_y, 8000) )
		{
			pColorInfo->nPlateColor = PC_BLUE;
		}
		else if ( nStdRGB0 < 1000)
		{
			pColorInfo->nPlateColor = PC_BLACK;
		}
	}
	else if ( pColorInfo->nCharType == 1 ) //黑字,区分黄,浅蓝,白
	{
		if ( nStdRGB1 < 400 )
		{
			pColorInfo->nPlateColor = PC_WHITE;
		}
		else if ( nD1_y < MIN_INT(nD1_b, 10000) )
		{
			pColorInfo->nPlateColor = PC_YELLOW;
		}
		else if ( nD1_b < MIN_INT(nD1_y, 10000) )
		{
			pColorInfo->nPlateColor = PC_LIGHTBLUE;
		}
		else if ( nStdRGB1 < 1000)
		{
			pColorInfo->nPlateColor = PC_WHITE;
		}
	}
	else if ( pColorInfo->nCharType == 2) //未知,判定是否黄牌
	{
		if ( (nStdRGB > 400) && (nD1_y < MIN_INT(nD1_b, 10000) ) )
		{
			pColorInfo->nPlateColor = PC_YELLOW;
		}
	}

	return S_OK;
}

//单通道均衡化
HRESULT GreyEqualization(HV_COMPONENT_IMAGE image)
{
	DWORD32 rgHistogram[256] = {0};
	int iTotalCount = image.iWidth * image.iHeight;
	if (iTotalCount == 0)
	{
		return E_INVALIDARG;
	}
	PBYTE8 pGrey = NULL;
	int iChannelCount = 0;
	switch(image.nImgType)
	{
	case HV_IMAGE_YUV_422:
		pGrey = GetHvImageData(&image, 0);
		iChannelCount = 1;
		break;
	case HV_IMAGE_YCbYCr:
		pGrey = GetHvImageData(&image, 0);
		iChannelCount = 2;
		break;
	case HV_IMAGE_CbYCrY:
		pGrey = GetHvImageData(&image, 0) + 1;
		iChannelCount = 2;
		break;
    default:
        break;
	}
	if (pGrey == NULL)
	{
		return E_INVALIDARG;
	}
	PBYTE8 pLine = pGrey;
	for (int i = 0; i < image.iHeight; i++, pLine += image.iStrideWidth[0])
	{
		for (int j = 0; j < image.iWidth; j++)
		{
			rgHistogram[pLine[j * iChannelCount]]++;
		}
	}
	for (int i = 1; i < 256; i++)
	{
		rgHistogram[i] += rgHistogram[i - 1];
	}
	pLine = pGrey;
	for (int i = 0; i < image.iHeight; i++, pLine += image.iStrideWidth[0])
	{
		for (int j = 0; j < image.iWidth; j++)
		{
			pLine[j * iChannelCount] = (BYTE8)(rgHistogram[pLine[j * iChannelCount]] * 255 / iTotalCount);
		}
	}
	return S_OK;
}

//BGR均衡化
HRESULT BgrEqualization(PBYTE8 pBGR, int iWidth, int iHeight, int iStride)
{
	DWORD32 rgHistogram[3][256] = {0};
	int iTotalCount = iWidth * iHeight;
	if (iTotalCount == 0 || iStride == 0)
	{
		return E_INVALIDARG;
	}
	for (int i = 0; i < iHeight; i++)
	{
		PBYTE8 pLine = pBGR + i * iStride;
		for (int j = 0; j < iWidth; j++)
		{
			rgHistogram[0][pLine[j * 3 + 0]]++;
			rgHistogram[1][pLine[j * 3 + 1]]++;
			rgHistogram[2][pLine[j * 3 + 2]]++;
		}
	}
	for (int i = 1; i < 256; i++)
	{
		rgHistogram[0][i] += rgHistogram[0][i - 1];
		rgHistogram[1][i] += rgHistogram[0][i - 1];
		rgHistogram[2][i] += rgHistogram[0][i - 1];
	}
	for (int i = 0; i < iHeight; i++)
	{
		PBYTE8 pLine = pBGR + i * iStride;
		for (int j = 0; j < iWidth; j++)
		{
			pLine[j * 3 + 0] = (BYTE8)(rgHistogram[0][pLine[j * 3 + 0]] * 255 / iTotalCount);
			pLine[j * 3 + 1] = (BYTE8)(rgHistogram[0][pLine[j * 3 + 1]] * 255 / iTotalCount);
			pLine[j * 3 + 2] = (BYTE8)(rgHistogram[0][pLine[j * 3 + 2]] * 255 / iTotalCount);
		}
	}
	return S_OK;
}

//图像增强, 灰度按分布拉伸+强化边缘
HRESULT GrayStretch2(
			  BYTE8* pSrcBuf,	//源图
			  int nStrideWidth,
			  int nWidth,
			  int nHeight,
			  BYTE8* pDestBuf,	//目标图
			  int nDestStrideWidth,
			  UINT nFilterSize,	//均值滤波窗口大小
			  UINT nEdgeAdj	//边缘增强系数
			  )
{
	CFastMemAlloc cStack;

	DWORD32 nPixelCount = nHeight * nWidth;

	RESTRICT_PBYTE8 pSrcLine, pDestLine;
	int i(0),j(0);

	//算直方图
	pSrcLine = pSrcBuf;
	int rgHist[256] = {0};
	for(i=0; i < nHeight; i++, pSrcLine += nStrideWidth)
	{
		for(j=0; j < nWidth; j++)
		{
			rgHist[pSrcLine[j]]++;
		}
	}

	//算直方图积分
	int rgItgHist[256] = {0};
	rgItgHist[0] = rgHist[0];
	for(i = 1; i < 256; i++)
	{
		rgItgHist[i] = rgItgHist[i-1] + rgHist[i];
	}

	//算映射表
	BYTE8 rgMap[256] = {0};
	rgMap[0] = 0;
	rgMap[255] = 255;
	for(i = 1; i < 255; i ++)
	{
		rgMap[i] = (65280 * rgItgHist[i] / (nPixelCount - rgHist[i]))>>8;	//8位量化
	}

	//算结果图
	pSrcLine = pSrcBuf;
	pDestLine = pDestBuf;
	for(i = 0; i < nHeight; i++, pSrcLine += nStrideWidth, pDestLine += nDestStrideWidth)
	{
		for(j = 0; j < nWidth; j++)
		{
			pDestLine[j] = rgMap[pSrcLine[j]];
		}
	}

	return S_OK;
}

HRESULT IMAGE_ConvertBGR2YUV(
							 RESTRICT_PBYTE8 pR,
							 RESTRICT_PBYTE8 pG,
							 RESTRICT_PBYTE8 pB,
							 int nWidth,
							 int nHeight,
							 int nStrideWidth,
							 HV_COMPONENT_IMAGE* pImg
							 )
{
	if(	nWidth > pImg->iWidth ||
		nHeight > pImg->iHeight ||
		pImg->nImgType != HV_IMAGE_YUV_422 )
	{
		return E_INVALIDARG;
	}

	RESTRICT_PBYTE8 pbY = GetHvImageData(pImg, 0);
	RESTRICT_PBYTE8 pbCb = GetHvImageData(pImg, 1);
	RESTRICT_PBYTE8 pbCr = GetHvImageData(pImg, 2);

	DWORD32 dwYStrideWidth = pImg->iStrideWidth[0];
	DWORD32 dwUVStrideWidth = pImg->iStrideWidth[0]>>1;

	int iCb = 0, iCr = 0, iTempCb, iTempCr, i, j;

	for(	i = 0;
			i < nHeight;
			i ++,
			pbY += dwYStrideWidth,
			pbCb += dwUVStrideWidth,
			pbCr += dwUVStrideWidth,
			pR += nStrideWidth,
			pG += nStrideWidth,
			pB += nStrideWidth
			)
	{
		for(	j = 0; j < nWidth; j++)
		{
			pbY[j] = ROUND_SHR(1868 * pB[ j ] + 9617 * pG[ j ] + 4899 * pR[ j ], 14);

			iTempCb =  128 + ROUND_SHR(8192 * pB[ j ] - 5428 * pG[ j ] - 2758 * pR[ j ], 14);
			iTempCr = 128 + ROUND_SHR(-1332 * pB[ j ] - 6860 * pG[ j ] + 8192 * pR[ j ], 14);

			if(j&0x01)
			{
				pbCb[nWidth>>1] = (iTempCb + iCb)>>1;
				pbCr[nWidth>>1] = (iTempCr + iCr)>>1;
			}
			else
			{
				iCb = iTempCb;
				iCr = iTempCr;
			}
		}
	}

	return S_OK;
}

void GrayStretch(
				 RESTRICT_PBYTE8 pBuf,
				 DWORD32 nWidth,
				 DWORD32 nHeight,
				 DWORD32 nStrideWidth,
				 BYTE8 nMin,
				 BYTE8 nMax
				 )
{
		if(nWidth < 20 || nHeight < 10) return;

		int nCropHeight = 2*nHeight/3;
		int nCropWidth = 2*nWidth/3;
		int nStartX = (nWidth - nCropWidth)/2;
		int nStartY = (nHeight - nCropHeight)/2;

		//算直方图
		RESTRICT_PBYTE8 pBufLine = pBuf + nStartY * nStrideWidth + nStartX;
		DWORD32 rgHist[256] = {0};
		int i,j;
		for(i=0; i < nCropHeight; i++, pBufLine += nStrideWidth)
		{
			for(j=0; j < nCropWidth; j++)
			{
				rgHist[pBufLine[j]]++;
			}
		}

		//算直方图积分
		DWORD32 rgItgHist[256] = {0};
		rgItgHist[0] = rgHist[0];
		for(i = 1; i < 256; i++)
		{
			rgItgHist[i] = rgItgHist[i-1] + rgHist[i];
		}

		DWORD32 nCount = nCropWidth * nCropHeight;
		int nL1 = (nCount * 20) >> 10;	//0.02
		int nL2 = (nCount * 1003) >> 10;	//0.98

		DWORD32 nMinL(0),nMaxL(255);

		while(rgItgHist[nMinL] < (DWORD32)nL1) nMinL++;
		while(rgItgHist[nMaxL - 1] > (DWORD32)nL2) nMaxL--;

		if(nMinL >= nMaxL) return;

		//算映射表
		BYTE8 rgMap[256] = {0};
		HV_memset(rgMap, nMin, nMinL + 1);
		HV_memset(rgMap + nMaxL, nMax, 255 - nMaxL + 1);

		int nScale = (nMax - nMin)<<8;

		for(i = nMinL; i <= (int)nMaxL; i ++)
		{
			rgMap[i] = (nScale * (i - nMinL) / (nMaxL - nMinL))>>8;	//8位量化
		}

		//算结果图
		pBufLine = pBuf;
		for(i = 0; i < (int)nHeight; i++, pBufLine += nStrideWidth)
		{
			for(j = 0; j < (int)nWidth; j++)
			{
				pBufLine[j] = rgMap[pBufLine[j]];
			}
		}
}

void GrayStretch_IR(
				 RESTRICT_PBYTE8 pBuf,
				 DWORD32 nWidth,
				 DWORD32 nHeight,
				 DWORD32 nStrideWidth,
				 BYTE8 nMin,
				 BYTE8 nMax
				 )
{
// modify for IR
//		if(nWidth < 20 || nHeight < 10) return;

		int nCropHeight = 2*nHeight/3;
		int nCropWidth = 2*nWidth/3;
		int nStartX = (nWidth - nCropWidth)/2;
		int nStartY = (nHeight - nCropHeight)/2;

		//算直方图
		RESTRICT_PBYTE8 pBufLine = pBuf + nStartY * nStrideWidth + nStartX;
		DWORD32 rgHist[256] = {0};
		int i,j;
		for(i=0; i < nCropHeight; i++, pBufLine += nStrideWidth)
		{
			for(j=0; j < nCropWidth; j++)
			{
				rgHist[pBufLine[j]]++;
			}
		}

		//算直方图积分
		DWORD32 rgItgHist[256] = {0};
		rgItgHist[0] = rgHist[0];
		for(i = 1; i < 256; i++)
		{
			rgItgHist[i] = rgItgHist[i-1] + rgHist[i];
		}

		DWORD32 nCount = nCropWidth * nCropHeight;
		int nL1 = (nCount * 20) >> 10;	//0.02
		int nL2 = (nCount * 1003) >> 10;	//0.98

		DWORD32 nMinL(0),nMaxL(255);

		while(rgItgHist[nMinL] < (DWORD32)nL1) nMinL++;
		while(rgItgHist[nMaxL - 1] > (DWORD32)nL2) nMaxL--;

		if(nMinL >= nMaxL) return;

		//算映射表
		BYTE8 rgMap[256] = {0};
		HV_memset(rgMap, nMin, nMinL + 1);
		HV_memset(rgMap + nMaxL, nMax, 255 - nMaxL + 1);

		int nScale = (nMax - nMin)<<8;

		for(i = nMinL; i <= (int)nMaxL; i ++)
		{
			rgMap[i] = (nScale * (i - nMinL) / (nMaxL - nMinL))>>8;	//8位量化
		}

		//算结果图
		pBufLine = pBuf;
		for(i = 0; i < (int)nHeight; i++, pBufLine += nStrideWidth)
		{
			for(j = 0; j < (int)nWidth; j++)
			{
				pBufLine[j] = rgMap[pBufLine[j]];
			}
		}
}

void GrayStretch_IR2(
				  RESTRICT_PBYTE8 pBuf, 
				  DWORD32 nWidth, 
				  DWORD32 nHeight, 
				  DWORD32 nStrideWidth,
				  BYTE8 nMin,
				  BYTE8 nMax,
				  int nMinY,
				  int nMaxY
				  )
{
	DWORD32 nCropHeight = 2*nHeight/3;
	DWORD32 nCropWidth = 2*nWidth/3;
	DWORD32 nStartX = (nWidth - nCropWidth)/2;
	DWORD32 nStartY = (nHeight - nCropHeight)/2;

	//算直方图
	RESTRICT_PBYTE8 pBufLine = pBuf + nStartY * nStrideWidth + nStartX;
	DWORD32 rgHist[256] = {0};
	DWORD32 i,j;
	for(i=0; i < nCropHeight; i++, pBufLine += nStrideWidth)
	{
		for(j=0; j < nCropWidth; j++)
		{
			rgHist[pBufLine[j]]++;
		}
	}

	//算直方图积分
	DWORD32 rgItgHist[256] = {0};
	rgItgHist[0] = rgHist[0];
	for(i = 1; i < 256; i++)
	{
		rgItgHist[i] = rgItgHist[i-1] + rgHist[i];
	}

	DWORD32 nCount = nCropWidth * nCropHeight;
	DWORD32 nL1 = (nCount * 100) >> 10;	//0.1
	DWORD32 nL2 = (nCount * 980) >> 10;	//0.95

	DWORD32 nMinL(0),nMaxL(255);

	while(rgItgHist[nMinL] < nL1) nMinL++;
	while(rgItgHist[nMaxL - 1] > nL2) nMaxL--;

	if(nMinL >= nMaxL) return;

	nMinL = MAX_INT(nMinY,nMinL);
	nMaxL = MIN_INT(nMaxY,nMaxL);
	//算映射表
	BYTE8 rgMap[256] = {0}; 
	HV_memset(rgMap, nMin, nMinL + 1);
	HV_memset(rgMap + nMaxL, nMax, 255 - nMaxL + 1);

	int nScale = (nMax - nMin)<<8;
	for(i = nMinL; i <= nMaxL; i ++)
	{
		rgMap[i] = (nScale * (i - nMinL) / (nMaxL - nMinL))>>8;	//8位量化
	}

	//算结果图
	pBufLine = pBuf;
	for(i = 0; i < nHeight; i++, pBufLine += nStrideWidth)
	{
		for(j = 0; j < nWidth; j++)
		{
			pBufLine[j] = rgMap[pBufLine[j]];
		}
	}
}

//图像增强
HRESULT PlateEnhance( HV_COMPONENT_IMAGE* pSrcImg )
{
	if (!pSrcImg) return E_INVALIDARG;

	GrayStretch(
		GetHvImageData(pSrcImg, 0),
		pSrcImg->iWidth,
		pSrcImg->iHeight,
		pSrcImg->iStrideWidth[0],
		0,255
		);

	return S_OK;
}

HRESULT PlateEnhance_IR( HV_COMPONENT_IMAGE* pSrcImg )
{
	if (!pSrcImg) return E_INVALIDARG;

	GrayStretch_IR(
		GetHvImageData(pSrcImg, 0),
		pSrcImg->iWidth,
		pSrcImg->iHeight,
		pSrcImg->iStrideWidth[0],
		0,255
		);

	return S_OK;
}
HRESULT PlateEnhance_IR2( HV_COMPONENT_IMAGE* pSrcImg ,int nMinY,int nMaxY)
{
	if (!pSrcImg) return E_INVALIDARG;

	GrayStretch_IR2( 
		GetHvImageData(pSrcImg, 0),
		pSrcImg->iWidth, 
		pSrcImg->iHeight, 
		pSrcImg->iStrideWidth[0],
		0,255,
		nMinY,
		nMaxY
		);

	return S_OK;
}

__inline int CalcPixelValue(
							RESTRICT_PBYTE8 pSrcBuf,
							WORD16 x, WORD16 y,
							WORD16 wXRaito, WORD16 wYRatio,
							int iWidth, int iHeight,
							int iStrideWidth
							)
{
	DWORD32 dwX = x * wXRaito;
	DWORD32 dwY = y * wYRatio;

	WORD16 x0 = MIN_INT((int)(dwX>>8), iWidth - 1);
	WORD16 y0 = MIN_INT((int)(dwY>>8), iHeight - 1);
	WORD16 x1 = MIN_INT((int)(x0 + 1), iWidth - 1);
	WORD16 y1 = MIN_INT((int)(y0 + 1), iHeight - 1);

	BYTE8 dwRX = dwX&0xff;
	BYTE8 dwRY = dwY&0xff;

	BYTE8 v0 = *(pSrcBuf + y0 * iStrideWidth + x0);
	BYTE8 v1 = *(pSrcBuf + y0 * iStrideWidth + x1);
	BYTE8 v2 = *(pSrcBuf + y1 * iStrideWidth + x0);
	BYTE8 v3 = *(pSrcBuf + y1 * iStrideWidth + x1);

	BYTE8 v01 = (v0 * (256 - dwRX) + v1 * dwRX)>>8;
	BYTE8 v23 = (v2 * (256 - dwRX) + v3 * dwRX)>>8;
	return  (v01 * (256- dwRY) + v23 * dwRY)>>8;
}

HRESULT PlateResize(HV_COMPONENT_IMAGE imgSrc, HV_COMPONENT_IMAGE imgDst)
{
	int nSrcWidth = imgSrc.iWidth;
	int nSrcHeight = imgSrc.iHeight;
	int nDstWidth = imgDst.iWidth;
	int nDstHeight = imgDst.iHeight;

	int nSrcStrideWidth = imgSrc.iStrideWidth[0];
	int nSrcStrideWidth2 = imgSrc.iStrideWidth[0]>>1;
	int nDstStrideWidth = imgDst.iStrideWidth[0];
	int nDstStrideWidth2 = imgDst.iStrideWidth[0]>>1;

	HV_memset(GetHvImageData(&imgDst, 0), 0, nDstStrideWidth * nDstHeight);
	HV_memset(GetHvImageData(&imgDst, 1), 128, nDstStrideWidth2 * nDstHeight);
	HV_memset(GetHvImageData(&imgDst, 2), 128, nDstStrideWidth2 * nDstHeight);

	WORD16 wXRaito = (WORD16)(256 * (float)nSrcWidth / nDstWidth); //8位量化
	WORD16 wYRatio = (WORD16)(256 * (float)nSrcHeight / nDstHeight);

	RESTRICT_PBYTE8 pSrcBuf0 = GetHvImageData(&imgSrc, 0), pSrcBuf1;
	RESTRICT_PBYTE8 pBufLine0 = GetHvImageData(&imgDst, 0), pBufLine1;
	for(int y = 0; y < nDstHeight; y++, pBufLine0 += nDstStrideWidth)
	{
		for(int x = 0; x < nDstWidth; x++)
		{
			pBufLine0[x] = CalcPixelValue(pSrcBuf0, x, y, wXRaito, wYRatio, nSrcWidth, nSrcHeight, nSrcStrideWidth);
		}
	}

	// ?????
	// YUV422格式pBufLine1 应该是NULL的，此处无任何此类的判断？
	pSrcBuf0 = GetHvImageData(&imgSrc, 1);
	pSrcBuf1 = GetHvImageData(&imgSrc, 2);
	pBufLine0 = GetHvImageData(&imgDst, 1);
	pBufLine1 = GetHvImageData(&imgDst, 2);
	nSrcWidth >>= 1;
	nDstWidth >>= 1;
	for(int y = 0; y < nDstHeight; y++, pBufLine0 += nDstStrideWidth2, pBufLine1 += nDstStrideWidth2)
	{
		for(int x = 0; x < nDstWidth; x++)
		{
			pBufLine0[x] = CalcPixelValue(pSrcBuf0, x, y, wXRaito, wYRatio, nSrcWidth, nSrcHeight, nSrcStrideWidth2);
			pBufLine1[x] = CalcPixelValue(pSrcBuf1, x, y, wXRaito, wYRatio, nSrcWidth, nSrcHeight, nSrcStrideWidth2);
		}
	}

	return S_OK;
}

// todo.
// 未验证。
HRESULT PlateResizeYUV420ToYUV422(HV_COMPONENT_IMAGE imgSrc, HV_COMPONENT_IMAGE imgDst)
{
	if( imgSrc.nImgType != HV_IMAGE_YUV_420 || imgDst.nImgType != HV_IMAGE_YUV_422 )
	{
		return E_NOTIMPL;
	}

	int nSrcWidth = imgSrc.iWidth;
	int nSrcHeight = imgSrc.iHeight;
	int nDstWidth = imgDst.iWidth;
	int nDstHeight = imgDst.iHeight;

	int nSrcStrideWidth = imgSrc.iStrideWidth[0];
	int nSrcStrideWidth2 = imgSrc.iStrideWidth[0]>>1;
	int nDstStrideWidth = imgDst.iStrideWidth[0];
	int nDstStrideWidth2 = imgDst.iStrideWidth[0]>>1;

	HV_memset(GetHvImageData(&imgDst, 0), 0, nDstStrideWidth * nDstHeight);
	HV_memset(GetHvImageData(&imgDst, 1), 128, nDstStrideWidth2 * nDstHeight);
	HV_memset(GetHvImageData(&imgDst, 2), 128, nDstStrideWidth2 * nDstHeight);

	WORD16 wXRaito = (WORD16)(256 * (float)nSrcWidth / nDstWidth); //8位量化
	WORD16 wYRatio = (WORD16)(256 * (float)nSrcHeight / nDstHeight);

	RESTRICT_PBYTE8 pSrcBuf0 = GetHvImageData(&imgSrc, 0), pSrcBuf1;
	RESTRICT_PBYTE8 pBufLine0 = GetHvImageData(&imgDst, 0), pBufLine1;
	for(int y = 0; y < nDstHeight; y++, pBufLine0 += nDstStrideWidth)
	{
		for(int x = 0; x < nDstWidth; x++)
		{
			pBufLine0[x] = CalcPixelValue(pSrcBuf0, x, y, wXRaito, wYRatio, nSrcWidth, nSrcHeight, nSrcStrideWidth);
		}
	}

	pSrcBuf0 = GetHvImageData(&imgSrc, 1);
	pSrcBuf1 = pSrcBuf0;
	pBufLine0 = GetHvImageData(&imgDst, 1);
	pBufLine1 = GetHvImageData(&imgDst, 2);
	nSrcWidth >>= 1;
	nDstWidth >>= 1;
	for(int y = 0; y < nDstHeight; y++, pBufLine0 += nDstStrideWidth2, pBufLine1 += nDstStrideWidth2)
	{
		int indexU = 0;
		int indexV = 0;
		for(int x = 0; x < nDstWidth; x++)
		{
			if( x % 2 == 0 )
			{
				pBufLine0[indexU] = CalcPixelValue(pSrcBuf0, x, y, wXRaito, wYRatio, nSrcWidth, nSrcHeight, nSrcStrideWidth2);
				pBufLine0[indexU + 1] = pBufLine0[indexU];
				indexU++;
			}
			else
			{
				pBufLine1[indexV] = CalcPixelValue(pSrcBuf1, x, y, wXRaito, wYRatio, nSrcWidth, nSrcHeight, nSrcStrideWidth2);
				pBufLine1[indexV + 1] = pBufLine1[indexV];
				indexV++;
			}
			//pBufLine0[x] = CalcPixelValue(pSrcBuf0, x, y, wXRaito, wYRatio, nSrcWidth, nSrcHeight, nSrcStrideWidth2);
			//pBufLine1[x] = CalcPixelValue(pSrcBuf1, x, y, wXRaito, wYRatio, nSrcWidth, nSrcHeight, nSrcStrideWidth2);
		}
	}

	return S_OK;
}

int CaclPlateLight(HV_COMPONENT_IMAGE imgCropPlate)
{
	int nWidth = imgCropPlate.iWidth;
	int nHeight = imgCropPlate.iHeight;
	int nCount = nWidth * nHeight;
	if(nCount == 0) return 0;

	int nSum = 0;
	RESTRICT_PBYTE8 pLine = GetHvImageData(&imgCropPlate, 0);
	for(int i = 0; i < nHeight; i++, pLine += imgCropPlate.iStrideWidth[0])
	{
		for(int j = 0; j < nWidth; j++)
		{
			nSum += pLine[j];
		}
	}

	return nSum/nCount;
}

// 图像逆时针旋转90°
//HRESULT ImageRotate90(HV_COMPONENT_IMAGE imgSrc, HV_COMPONENT_IMAGE imgDst)
//
//{
//
//	if (imgSrc.nImgType != imgDst.nImgType ||
//		imgSrc.iWidth != imgDst.iHeight ||
//		imgSrc.iHeight != imgDst.iWidth)
//	{
//		return E_INVALIDARG;
//	}
//
//	if ((imgSrc.nImgType != HV_IMAGE_YUV_422) && (imgSrc.nImgType != HV_IMAGE_YCbYCr))
//	{
//		return E_INVALIDARG;
//	}
//
//	// 先实现HV_IMAGE_YCbYCr 算法
//	int nSrcStride = imgSrc.iStrideWidth[0];
//	int nDstStride = imgDst.iStrideWidth[0];
//	int nSrcStride2 = nSrcStride << 1;
//	int nDstStride2 = nDstStride << 1;
//
//	BYTE8 *pSrc = imgSrc.pbData[0];
//	BYTE8 *pDst = imgDst.pbData[0] + (imgDst.iHeight - 1) * nDstStride;						// 目标图像对应的开始位置
//	for(int y = 0; y < imgSrc.iHeight; y += 2, pSrc += nSrcStride2, pDst += 4)				// 一次处理两行
//	{
//		BYTE8 *pSrc1 = pSrc;
//		BYTE8 *pDst1 = pDst;
//		for(int x = 0; x < imgSrc.iWidth; x += 2, pSrc1 += 4, pDst1 -= nDstStride2)			// 一次处理4个数据(2个Y.1个Cb,1个Cr)
//		{
//			*pDst1 = *pSrc1;
//			*(pDst1 + 2) = *(pSrc1 + nSrcStride);
//			*(pDst1 - nDstStride) = *(pSrc1 + 2);
//			*(pDst1 - nDstStride + 2) = *(pSrc1 + 2 + nSrcStride);
//
//			*(pDst1 + 1) = *(pDst1 + 1 - nDstStride) = *(pSrc1 + 1);
//			*(pDst1 + 3) = *(pDst1 + 3 - nDstStride) = *(pSrc1 + 3);
//		}
//	}
//
//	return S_OK;
//}

// 图像逆时针旋转90°
// 这个是优化算法，减少了内存访问次数
HRESULT ImageRotate90(HV_COMPONENT_IMAGE imgSrc, HV_COMPONENT_IMAGE imgDst)

{

	if (imgSrc.nImgType != imgDst.nImgType ||
		imgSrc.iWidth != imgDst.iHeight ||
		imgSrc.iHeight != imgDst.iWidth)
	{
		return E_INVALIDARG;
	}

	if ((imgSrc.nImgType != HV_IMAGE_YUV_422) && (imgSrc.nImgType != HV_IMAGE_YCbYCr))
	{
		return E_INVALIDARG;
	}

	// 先实现HV_IMAGE_YCbYCr 算法
	int nSrcStride = imgSrc.iStrideWidth[0];
	int nDstStride = imgDst.iStrideWidth[0];
	int nSrcStride2 = nSrcStride << 1;
	int nDstStride2 = nDstStride << 1;

	BYTE8 *pSrc = GetHvImageData(&imgSrc, 0);
	BYTE8 *pDst = GetHvImageData(&imgDst, 0) + (imgDst.iHeight - 1) * nDstStride;						// 目标图像对应的开始位置
	DWORD32 dwSrcL1, dwSrcL2;
	DWORD32 dwDstL1, dwDstL2;

	for(int y = 0; y < imgSrc.iHeight; y += 2, pSrc += nSrcStride2, pDst += 4)				// 一次处理两行
	{
		BYTE8 *pSrc1 = pSrc;
		BYTE8 *pDst1 = pDst;
		for(int x = 0; x < imgSrc.iWidth; x += 2, pSrc1 += 4, pDst1 -= nDstStride2)			// 一次处理4个数据(2个Y.1个Cb,1个Cr)
		{
			// 一次读取两行8个数据
			dwSrcL1 = *((DWORD32 *)pSrc1);
			dwSrcL2 = *((DWORD32 *)(pSrc1 + nSrcStride));

			dwDstL1 = dwSrcL1 & 0xFF00FF00;
			dwDstL2 = dwDstL1;

			dwDstL1 |= (dwSrcL1 & 0xFF);
			dwDstL1 |= ((dwSrcL2 & 0xFF) << 16);

			dwDstL2 |= ((dwSrcL1 >> 16) & 0xFF);
			dwDstL2 |= (dwSrcL2 & 0x00FF0000);

			*((DWORD32 *)pDst1) = dwDstL1;
			*((DWORD32 *)(pDst1 - nDstStride)) = dwDstL2;
		}
	}

	return S_OK;
}

// 电警为性能实现在svEPApi.67P.lib里
#ifdef FAST_ITG
HRESULT IntegralPreprocessForCompactY_Fast(
	PDWORD32 pdwItgImage,
	PDWORD32 pdwItgSqImage,
	WORD16 &wItgWidth,
	WORD16 &wItgHeight,
	WORD16 nItgStrideWidth,
	HV_COMPONENT_IMAGE *pImageSrc 				//指向原始图象数据.
);
#endif

HRESULT IntegralPreprocessForCompactY(
	PDWORD32 pdwItgImage,
	PDWORD32 pdwItgSqImage,
	WORD16 &wItgWidth,
	WORD16 &wItgHeight,
	WORD16 nItgStrideWidth,
	HV_COMPONENT_IMAGE *pImageSrc 				//指向原始图象数据.
)
{
	RESTRICT_PDWORD32 pdwItgOrigin, pdwFastItg, pdwFastItgUp;
	RESTRICT_PDWORD32 pdwItgSqOrigin, pdwFastItgSq, pdwFastItgSqUp;
	DWORD32 dwItgValue, dwItgSqValue;
	RESTRICT_PBYTE8 pbImageOrigin;
	BYTE8 bTemp;
	int i, j;
	int iSrcStep;
	CFastMemAlloc stack;

	if (pdwItgImage == NULL ||
		pdwItgSqImage == NULL ||
		pImageSrc == NULL)
	{
		return E_POINTER;
	}

	pdwFastItg = (PDWORD32)stack.StackAlloc(nItgStrideWidth * sizeof(DWORD32));
	pdwFastItgUp = (PDWORD32)stack.StackAlloc(nItgStrideWidth * sizeof(DWORD32));
	pdwFastItgSq = (PDWORD32)stack.StackAlloc(nItgStrideWidth * sizeof(DWORD32));
	pdwFastItgSqUp = (PDWORD32)stack.StackAlloc(nItgStrideWidth * sizeof(DWORD32));

	switch (pImageSrc->nImgType)
	{
	case HV_IMAGE_YUV_422:
		pbImageOrigin = GetHvImageData(pImageSrc, 0);
		iSrcStep = 1;
		break;
	case HV_IMAGE_YUV_420:
	case HV_IMAGE_BT1120:
	case HV_IMAGE_BT1120_FIELD:
	case HV_IMAGE_BT1120_ROTATE_Y:
		pbImageOrigin = GetHvImageData(pImageSrc, 0);
		iSrcStep = 1;
		break;
	case HV_IMAGE_YCbYCr:
		pbImageOrigin = GetHvImageData(pImageSrc, 0);
		iSrcStep = 2;
		break;
	case HV_IMAGE_CbYCrY:
		pbImageOrigin = GetHvImageData(pImageSrc, 0) + 1;
		iSrcStep = 2;
		break;
	default:
		return E_NOTIMPL;
	}
	pdwItgOrigin = pdwItgImage;
	pdwItgSqOrigin = pdwItgSqImage;

	wItgWidth = pImageSrc->iWidth + 1;
	wItgHeight = pImageSrc->iHeight + 1;

	// 将积分图的第一行数据置为0.
	memset( pdwFastItgUp, 0, wItgWidth * sizeof( DWORD32 ) );
	memset( pdwFastItgSqUp, 0, wItgWidth * sizeof( DWORD32 ) );

    memcpy((PBYTE8)pdwItgOrigin, (PBYTE8)pdwFastItgUp, wItgWidth * sizeof(DWORD32));
    memcpy((PBYTE8)pdwItgSqOrigin, (PBYTE8)pdwFastItgSqUp, wItgWidth * sizeof(DWORD32));

	pdwItgOrigin += nItgStrideWidth;
	pdwItgSqOrigin += nItgStrideWidth;

	dwItgValue = 0;
	dwItgSqValue = 0;

	PDWORD32 pdwTemp1, pdwFastItgTemp1, pdwFastItgSqTemp1, pdwFastItgUpTemp, pdwFastItgSqUpTemp;
	DWORD32 dwTemp1;//, dwTemp2;
	DWORD32 bTemp1, bTemp2, bTemp3, bTemp4;

	WORD16 wWidthNew = wItgWidth;
	if (wItgWidth & 3 )
	{
		wWidthNew = (wItgWidth & ~3);
	}
	// 进行积分图运算
	for ( i = 1; i < wItgHeight; ++ i )
	{
		pdwFastItg[0] = 0;
		pdwFastItgSq[0] = 0;

		pdwTemp1 = ( PDWORD32 )pbImageOrigin;
		pdwFastItgTemp1 = pdwFastItg;
		pdwFastItgSqTemp1 = pdwFastItgSq;
		pdwFastItgUpTemp = pdwFastItgUp;
		pdwFastItgSqUpTemp = pdwFastItgSqUp;

		for ( j = 1; j < wWidthNew; j += 4 )
		{
			dwTemp1 = *( pdwTemp1 ++ );
			//dwTemp2 = *( pdwTemp1 ++ );

			bTemp1 = ( dwTemp1 ) & 0xFF;
			dwItgValue += bTemp1;
			dwItgSqValue += bTemp1 * bTemp1;
			*(++pdwFastItgTemp1) = dwItgValue + *(++pdwFastItgUpTemp);
			*(++pdwFastItgSqTemp1) = dwItgSqValue + *(++pdwFastItgSqUpTemp);

			bTemp2 = ( dwTemp1 >> 8 ) & 0xFF;
			dwItgValue += bTemp2;
			dwItgSqValue += bTemp2 * bTemp2;
			*(++pdwFastItgTemp1) = dwItgValue + *(++pdwFastItgUpTemp);
			*(++pdwFastItgSqTemp1) = dwItgSqValue + *(++pdwFastItgSqUpTemp);

			bTemp3 = ( dwTemp1 >> 16 ) & 0xFF;
			dwItgValue += bTemp3;
			dwItgSqValue += bTemp3 * bTemp3;
			*(++pdwFastItgTemp1) = dwItgValue + *(++pdwFastItgUpTemp);
			*(++pdwFastItgSqTemp1) = dwItgSqValue + *(++pdwFastItgSqUpTemp);

			bTemp4 = ( dwTemp1 >> 24 ) & 0xFF;
			dwItgValue += bTemp4;
			dwItgSqValue += bTemp4 * bTemp4;
			*(++pdwFastItgTemp1) = dwItgValue + *(++pdwFastItgUpTemp);
			*(++pdwFastItgSqTemp1) = dwItgSqValue + *(++pdwFastItgSqUpTemp);
		}
		for ( ; j < wItgWidth; ++ j )
		{
			bTemp = pbImageOrigin[(j - 1) * iSrcStep];
			dwItgValue += bTemp;
			dwItgSqValue += bTemp * bTemp;

			pdwFastItg[j] = dwItgValue + pdwFastItgUp[j];
			pdwFastItgSq[j] = dwItgSqValue + pdwFastItgSqUp[j];
		}

		PDWORD32 pdwTemp1 = pdwFastItgUp;
		PDWORD32 pdwTemp2 = pdwFastItgSqUp;
		pdwFastItgUp = pdwFastItg;
		pdwFastItgSqUp = pdwFastItgSq;
		pdwFastItg = pdwTemp1;
		pdwFastItgSq = pdwTemp2;

		memcpy((PBYTE8)pdwItgOrigin, (PBYTE8)pdwFastItgUp, wItgWidth * sizeof(DWORD32));
		memcpy((PBYTE8)pdwItgSqOrigin, (PBYTE8)pdwFastItgSqUp, wItgWidth * sizeof(DWORD32));

		pdwItgOrigin += nItgStrideWidth;
		pdwItgSqOrigin += nItgStrideWidth;
		pbImageOrigin += pImageSrc->iStrideWidth[0];

		dwItgValue = 0;
		dwItgSqValue = 0;
	}

	return( S_OK );
}

/* 函数说明:该函数用来积分图,对输入的原始图象(亮度)进行积分运算,生成一般积分图和平方积分图. */
HRESULT IntegralPreprocess(
	PDWORD32 pdwItgImage,
	PDWORD32 pdwItgSqImage,
	WORD16 &wItgWidth,
	WORD16 &wItgHeight,
	WORD16 nItgStrideWidth,
	HV_COMPONENT_IMAGE *pImageSrc 				//指向原始图象数据.
)
{
	if ((HV_IMAGE_BT1120 == pImageSrc->nImgType) ||
		(HV_IMAGE_BT1120_FIELD == pImageSrc->nImgType) ||
		(HV_IMAGE_BT1120_ROTATE_Y == pImageSrc->nImgType) ||
		(HV_IMAGE_YUV_420 == pImageSrc->nImgType) ||
		(HV_IMAGE_YUV_422 == pImageSrc->nImgType))
	{
#ifndef FAST_ITG
		return IntegralPreprocessForCompactY(pdwItgImage, pdwItgSqImage, wItgWidth, wItgHeight, nItgStrideWidth, pImageSrc);
#else
		return IntegralPreprocessForCompactY_Fast(pdwItgImage, pdwItgSqImage, wItgWidth, wItgHeight, nItgStrideWidth, pImageSrc);
#endif
	}


	RESTRICT_PDWORD32 pdwItgOrigin, pdwFastItg, pdwFastItgUp;
	RESTRICT_PDWORD32 pdwItgSqOrigin, pdwFastItgSq, pdwFastItgSqUp;
	DWORD32 dwItgValue, dwItgSqValue;
	RESTRICT_PBYTE8 pbImageOrigin;
	BYTE8 bTemp;
	int i, j;
	int iSrcStep;
	CFastMemAlloc stack;
	int iDmaItgHandle, iDmaItgSqHandle;

	if (pdwItgImage == NULL ||
		pdwItgSqImage == NULL ||
		pImageSrc == NULL)
	{
		return E_POINTER;
	}

	HV_invalidate(pdwItgImage, nItgStrideWidth * wItgHeight * sizeof(DWORD32));
	HV_invalidate(pdwItgSqImage, nItgStrideWidth * wItgHeight * sizeof(DWORD32));

	pdwFastItg = (PDWORD32)stack.StackAlloc(nItgStrideWidth * sizeof(DWORD32));
	pdwFastItgUp = (PDWORD32)stack.StackAlloc(nItgStrideWidth * sizeof(DWORD32));
	pdwFastItgSq = (PDWORD32)stack.StackAlloc(nItgStrideWidth * sizeof(DWORD32));
	pdwFastItgSqUp = (PDWORD32)stack.StackAlloc(nItgStrideWidth * sizeof(DWORD32));

	switch (pImageSrc->nImgType)
	{
	case HV_IMAGE_YUV_422:
		pbImageOrigin = GetHvImageData(pImageSrc, 0);
		iSrcStep = 1;
		break;
	case HV_IMAGE_YUV_420:
	case HV_IMAGE_BT1120:
	case HV_IMAGE_BT1120_FIELD:
	case HV_IMAGE_BT1120_ROTATE_Y:
		pbImageOrigin = GetHvImageData(pImageSrc, 0);
		iSrcStep = 1;
		break;
	case HV_IMAGE_YCbYCr:
		pbImageOrigin = GetHvImageData(pImageSrc, 0);
		iSrcStep = 2;
		break;
	case HV_IMAGE_CbYCrY:
		pbImageOrigin = GetHvImageData(pImageSrc, 0) + 1;
		iSrcStep = 2;
		break;
	default:
		return E_NOTIMPL;
	}
	pdwItgOrigin = pdwItgImage;
	pdwItgSqOrigin = pdwItgSqImage;

	wItgWidth = pImageSrc->iWidth + 1;
	wItgHeight = pImageSrc->iHeight + 1;

	// 将积分图的第一行数据置为0.
	memset( pdwFastItgUp, 0, wItgWidth * sizeof( DWORD32 ) );
	memset( pdwFastItgSqUp, 0, wItgWidth * sizeof( DWORD32 ) );
	iDmaItgHandle = HV_dmacpy(pdwItgOrigin, pdwFastItgUp, wItgWidth * sizeof( DWORD32 ));
	iDmaItgSqHandle = HV_dmacpy(pdwItgSqOrigin, pdwFastItgSqUp, wItgWidth * sizeof( DWORD32 ));

	pdwItgOrigin += nItgStrideWidth;
	pdwItgSqOrigin += nItgStrideWidth;

	dwItgValue = 0;
	dwItgSqValue = 0;

	// 进行积分图运算
	for ( i = 1; i < wItgHeight; ++ i )
	{
		pdwFastItg[0] = 0;
		pdwFastItgSq[0] = 0;

#if (RUN_PLATFORM == PLATFORM_DSP_BIOS)
#pragma MUST_ITERATE (8);
#endif
		for ( j = 1; j < wItgWidth; ++ j )
		{
			bTemp = pbImageOrigin[(j - 1) * iSrcStep];
			dwItgValue += bTemp;
			dwItgSqValue += bTemp * bTemp;

			pdwFastItg[j] = dwItgValue + pdwFastItgUp[j];
			pdwFastItgSq[j] = dwItgSqValue + pdwFastItgSqUp[j];
		}
		PDWORD32 pdwTemp1 = pdwFastItgUp;
		PDWORD32 pdwTemp2 = pdwFastItgSqUp;
		pdwFastItgUp = pdwFastItg;
		pdwFastItgSqUp = pdwFastItgSq;
		pdwFastItg = pdwTemp1;
		pdwFastItgSq = pdwTemp2;
		HV_dmawait(iDmaItgHandle);
		HV_dmawait(iDmaItgSqHandle);
		iDmaItgHandle = HV_dmacpy(pdwItgOrigin, pdwFastItgUp, wItgWidth * sizeof( DWORD32 ));
		iDmaItgSqHandle = HV_dmacpy(pdwItgSqOrigin, pdwFastItgSqUp, wItgWidth * sizeof( DWORD32 ));

		pdwItgOrigin += nItgStrideWidth;
		pdwItgSqOrigin += nItgStrideWidth;
		pbImageOrigin += pImageSrc->iStrideWidth[0];

		dwItgValue = 0;
		dwItgSqValue = 0;
	}

	return( S_OK );
}

HRESULT hvCalcRectifyInfo(HV_COMPONENT_IMAGE &imgSrc, float &fltVAngle, float &fltHAngle)
{
	CPersistentComponentImage imgSobalH, imgSobalV;
	RTN_HR_IF_FAILED(imgSobalH.Create(HV_IMAGE_GRAY, imgSrc.iWidth, imgSrc.iHeight));
	RTN_HR_IF_FAILED(imgSobalV.Create(HV_IMAGE_GRAY, imgSrc.iWidth, imgSrc.iHeight));
	RTN_HR_IF_FAILED(hvImageSobelH(imgSrc, imgSobalH));
	RTN_HR_IF_FAILED(hvImageSobelV(imgSrc, imgSobalV));

	int iMaxSum = 0;
	//计算水平角度
	PBYTE8 pbImgSobalH = GetHvImageData(&imgSobalH, 0);
	for (float fltAngle = 70.0f; fltAngle < 110.1f; fltAngle += 1.0f)
	{
		float fltCos = cos(fltAngle / 180.0f * PI);
		float fltSin = sin(fltAngle / 180.0f * PI);
		for (int iDistance = 0; iDistance < imgSrc.iHeight; iDistance++)
		{
			int iSum = 0;
			for (int y = 0; y < imgSobalH.iHeight; y++)
			{
				for (int x = 0; x < imgSobalH.iWidth; x++)
				{
					float fltTemp = (float)iDistance - x * fltCos - y * fltSin;
					if (fltTemp < 1.0f && fltTemp > -1.0f)
					{
						iSum += pbImgSobalH[y * imgSobalH.iStrideWidth[0] + x];
					}
				}
			}
			if (iSum > iMaxSum)
			{
				iMaxSum = iSum;
				fltHAngle = 90.0f - fltAngle;
			}
		}
	}

	//计算垂直角度
	iMaxSum = 0;
	PBYTE8 pbImgSobalV = GetHvImageData(&imgSobalV, 0);
	for (float fltAngle = -20.0f; fltAngle < 20.1f; fltAngle += 1.0f)
	{
		float fltCos = cos(fltAngle / 180.0f * PI);
		float fltSin = sin(fltAngle / 180.0f * PI);
		for (int iDistance = 0; iDistance < imgSrc.iWidth; iDistance++)
		{
			int iSum = 0;
			for (int y = 0; y < imgSobalH.iHeight; y++)
			{
				for (int x = 0; x < imgSobalH.iWidth; x++)
				{
					float fltTemp = (float)iDistance - x * fltCos - y * fltSin;
					if (fltTemp < 1.0f && fltTemp > -1.0f)
					{
						iSum += pbImgSobalV[y * imgSobalV.iStrideWidth[0] + x];
					}
				}
			}
			if (iSum > iMaxSum)
			{
				iMaxSum = iSum;
				fltVAngle = 90.0f - fltAngle;
			}
		}
	}
	return S_OK;
}

#if defined(_OPENCV_DEBUG) && defined(_DEBUG) && (RUN_PLATFORM != PLATFORM_DSP_BIOS)

HRESULT HvImageToIplImageBGR(IplImage *pDst, const HV_COMPONENT_IMAGE *pSrc)
{
	if (pDst == NULL || pSrc == NULL)
	{
		return E_POINTER;
	}
	if (pSrc->nImgType != HV_IMAGE_YUV_422)
	{
		return E_INVALIDARG;
	}
	int iDstWidth = pSrc->iWidth;
	int iDstHeight = pSrc->iHeight;
	if (pDst->width != iDstWidth ||
		pDst->height != iDstHeight ||
		pDst->nChannels != 3 ||
		pDst->depth != IPL_DEPTH_8U)
	{
		return E_INVALIDARG;
	}

	for (int y = 0; y < iDstHeight; y++)
	{
		PBYTE8 pDstData = (PBYTE8)(pDst->imageData + y * pDst->widthStep);
		PBYTE8 pSrcY = (PBYTE8)(pSrc->pbData[0] + y * pSrc->iStrideWidth[0]);
		PBYTE8 pSrcCb = (PBYTE8)(pSrc->pbData[1] + y * pSrc->iStrideWidth[1]);
		PBYTE8 pSrcCr = (PBYTE8)(pSrc->pbData[2] + y * pSrc->iStrideWidth[2]);
		for (int x = 0; x < iDstWidth; x++)
		{
			YCbCr2BGR(pSrcY[x], pSrcCb[x/2], pSrcCr[x/2], &pDstData[x * 3], &pDstData[x * 3 + 1], &pDstData[x * 3 + 2]);
		}
	}
	return S_OK;
}

HRESULT HvImageToIplImageGrey(IplImage *pDst, const HV_COMPONENT_IMAGE *pSrc)
{
	if (pDst == NULL || pSrc == NULL)
	{
		return E_POINTER;
	}
	if (pSrc->nImgType != HV_IMAGE_GRAY)
	{
		return E_INVALIDARG;
	}
	int iDstWidth = pSrc->iWidth;
	int iDstHeight = pSrc->iHeight;
	if (pDst->width != iDstWidth ||
		pDst->height != iDstHeight ||
		pDst->nChannels != 1 ||
		pDst->depth != IPL_DEPTH_8U)
	{
		return E_INVALIDARG;
	}

	memset(pDst->imageData, 0, pDst->widthStep * pDst->height);
	for (int y = 0; y < iDstHeight; y++)
	{
		PBYTE8 pDstData = (PBYTE8)(pDst->imageData + y * pDst->widthStep);
		PBYTE8 pSrcData = (PBYTE8)(pSrc->pbData[0] + y * pSrc->iStrideWidth[0]);
		for (int x = 0; x < iDstWidth; x++)
		{
			pDstData[x] = pSrcData[x];
		}
	}
	return S_OK;
}

HRESULT HvImageDebugShow(const HV_COMPONENT_IMAGE *pImg, CvSize csShowWinSize)
{
	static bool s_bNeverShow = false;
	static bool s_bCtrlMode = true;
	static LONG s_iShowCount = 0;
	if (s_bNeverShow)
	{
		return S_OK;
	}

	HRESULT hr = S_OK;
	if (pImg == NULL)
	{
		return E_POINTER;
	}
	if ((csShowWinSize.height <= 0) || (csShowWinSize.width <=0 ))
	{
		return E_INVALIDARG;
	}
	if ((pImg->nImgType != HV_IMAGE_GRAY) &&
		(pImg->nImgType != HV_IMAGE_YUV_422))
	{
		return E_INVALIDARG;
	}
	if (s_iShowCount == 0)
	{
		cvNamedWindow("ImageDebugShow", 0);
		cvResizeWindow("ImageDebugShow", csShowWinSize.width, csShowWinSize.height);
		cvMoveWindow("ImageDebugShow", 0, 500);
	}
	s_iShowCount++;

	IplImage *pFrame = NULL;

	switch (pImg->nImgType)
	{
	case HV_IMAGE_GRAY:
		{
			pFrame = cvCreateImage(cvSize(pImg->iWidth, pImg->iHeight), IPL_DEPTH_8U, 1);
			hr = HvImageToIplImageGrey(pFrame, pImg);
		}
		break;
	case HV_IMAGE_YUV_422:
		{
			pFrame = cvCreateImage(cvSize(pImg->iWidth, pImg->iHeight), IPL_DEPTH_8U, 3);
			hr = HvImageToIplImageBGR(pFrame, pImg);
		}
		break;
	default:
		break;
	}

	if (hr != S_OK)
	{
		return hr;
	}

	cvShowImage("ImageDebugShow", pFrame);

	int key = 0;
	bool bPause;
	if (s_bCtrlMode == true)
	{
		bPause = true;
		while (bPause)
		{
			key = cvWaitKey(50);
			if (key == (int)'c')
			{
				s_bCtrlMode = false;
				bPause = false;
			}
			if (key == (int)'p')
			{
				bPause = false;
			}
			if (key == (int)'q')
			{
				bPause = false;
				s_bNeverShow = true;
			}
		}
	}
	else
	{
		bPause = false;
		do
		{
			key = cvWaitKey(50);
			if (key == (int)'c')
			{
				s_bCtrlMode = true;
				bPause = false;
			}
			if (key == (int)'q')
			{
				bPause = false;
				s_bNeverShow = true;
			}
			if (key == (int)'p')
			{
				bPause = !bPause;
			}
		} while (bPause);
	}

	cvReleaseImage(&pFrame);
	if (s_bNeverShow)
	{
		cvDestroyWindow("ImageDebugShow");
	}
	return S_OK;
}

#endif

void EnhanceColor(HV_COMPONENT_IMAGE* pImg)
{
	if(!pImg) return;

	RESTRICT_PBYTE8 pbCbBuf = GetHvImageData(pImg, 1);
	int iWidth = pImg->iWidth / 2;
	int iHeight = pImg->iHeight;
	int iStrideWidth = pImg->iStrideWidth[0]/2;

	for(int i = 0; i < iHeight; i++, pbCbBuf += iStrideWidth)
	{
		for(int j = 0; j < iWidth; j++)
		{
			pbCbBuf[j] = MAX_INT(MIN_INT((pbCbBuf[j] - 128) * 2 + 128, 255), 0);
		}
	}
}


HRESULT ProcessBWPlate( HV_COMPONENT_IMAGE* pSrcImg )
{
	if (!pSrcImg) return E_INVALIDARG;
	if (pSrcImg->iHeight > 100 || pSrcImg->iHeight < 4 || pSrcImg->iWidth < 20)
	{
		return S_OK;
	}

	int i, j, iWidth, iHeight, iStride, iSumAll(0), iOffset, iLen;
	BYTE8 *pbData, *pbData2;
	int iSum[100] = {0};

	iWidth = pSrcImg->iWidth;
	iHeight = pSrcImg->iHeight;
	iStride = pSrcImg->iStrideWidth[0];
	pbData = GetHvImageData(pSrcImg, 0);
	iOffset = iWidth >> 2;
	iLen = iWidth >> 1;

	for (i = 0; i < iHeight; i++, pbData += iStride)
	{
		pbData2 = pbData + iOffset;
		for (j = 0; j < iLen; j++, pbData2++)
		{
			iSum[i] += *pbData2;
		}
	}
	iOffset = iHeight >> 1;
	iLen = iHeight - (iHeight >> 2);
	for (i = iOffset; i <= iLen; i++)
	{
		iSumAll += iSum[i];
	}
	iSumAll /= iLen - iOffset + 1;

	if (iSumAll < 100 * (iWidth >> 1))
	{
		return S_OK;
	}

	for (i = 0; i < iHeight; i++)
	{
		if (0 != iSum[i])
		{
			iSum[i] = (iSumAll << 10) / iSum[i];
			iSum[i] = MIN_INT(iSum[i], 1536);
		}
	}

	iSumAll = -1;
	iOffset = 0;
	for (i = iLen; i >= 0; i--)
	{
		if (iSum[i] > 1228)
		{
			iOffset++;
		}
		else
		{
			iOffset = 0;
		}
		if (iOffset > 2)
		{
			iSumAll = i;
			break;
		}
	}

	if (-1 == iSumAll)
	{
		return S_OK;
	}

	pbData = GetHvImageData(pSrcImg, 0);
	for (i = 0; i < iLen; i++, pbData += iStride)
	{
		if (iSum[i] > 1024)
		{
			pbData2 = pbData;
			for (j = 0; j < iWidth; j++, pbData2++)
			{
				iOffset = (*pbData2 * iSum[i]) >> 10;
				*pbData2 = MIN_INT(iOffset, 255);
			}
		}
	}

	return S_OK;
}
// 用大津法进行二值化
HRESULT Otsu(HV_COMPONENT_IMAGE* pImage, HV_COMPONENT_IMAGE* pImageOut,  int* piCount, int iIsWhiteChar)
{
	BYTE8* pbImage;				// 图像指针
	int pixel;
	int iPointAll, iNo1, iNo2;
	SDWORD32 iSum, iSumP;
	DWORD32 dwVar, dwMax;
	int* piHist;
	int iThresholdValue = -1;
	CFastMemAlloc FastStack;
	piHist = (int*)FastStack.StackAlloc(256 * sizeof(int));

	pImageOut->iHeight = pImage->iHeight;
	pImageOut->iWidth = pImage->iWidth;
	pImageOut->iStrideWidth[0] = pImage->iWidth;
	memset(piHist, 0, 256 * sizeof(int));	// 对直方图置零
	memset(GetHvImageData(pImageOut, 0), 0, pImageOut->iHeight * pImageOut->iStrideWidth[0]);

	*piCount = 0;
	for (int i = 0; i < pImage->iHeight; i++)		// 生成直方图
	{
		pbImage = (GetHvImageData(pImage, 0) + pImage->iStrideWidth[0] * i);
		for (int j = 0; j < pImage->iWidth; j++)
		{
			pixel = pbImage[j];
			piHist[pixel]++;
		}
	}

	iSum = iSumP = 0;
	iPointAll = 0;

	for (int k = 0; k <= 255; k++)
	{
		iSum += k * piHist[k];		// x*f(x) 质量矩
	}

	iPointAll = pImage->iHeight * pImage->iWidth;
	if (iPointAll == 0)
	{
		*piCount = 0;
		return S_FALSE;
	}
	dwMax = 0;
	iNo1 = 0;
	SDWORD32 sdwTemp1, sdwTemp2;
	for (int k = 0; k < 254; k++)
	{
		iNo1 += piHist[k];
		if (!iNo1)	{ continue; }
		iNo2 = iPointAll - iNo1;
		if (0 == iNo2)	{ break; }
		iSumP += (k *piHist[k]);
		sdwTemp1 = iSumP / iNo1;
		sdwTemp2 = (iSum - iSumP) / iNo2;
		dwVar = ((iNo1 * iNo2) >> 6) * (sdwTemp1 - sdwTemp2) * (sdwTemp1 - sdwTemp2);		// 在200 * 30的图像中 右移6位才能保证不越界
		//dwVar = (iSumP * iNo2 - (iSum - iSumP) * iNo1) / (iNo1 * iNo2) * (iSumP * iNo2 - (iSum - iSumP) * iNo1); // iVar为类间方差
		if (dwVar > dwMax)
		{
			dwMax = dwVar;
			iThresholdValue = k;
		}
	}

	BYTE8* pbImageOutLine;
	if (iIsWhiteChar)
	{
		for (int i = 0; i < pImage->iHeight; i++)
		{
			pbImage = GetHvImageData(pImage, 0) + pImage->iStrideWidth[0] * i;
			pbImageOutLine  = GetHvImageData(pImageOut, 0) + pImageOut->iStrideWidth[0] * i;
			for (int j = 0; j < pImage->iWidth; j++)
			{
				if (pbImage[j] > iThresholdValue)
				{
					(*piCount)++;
					pbImageOutLine[j] = 255;
				}
			}
		}
	}
	else
	{
		for (int i = 0; i < pImage->iHeight; i++)
		{
			pbImage = GetHvImageData(pImage, 0) + pImage->iStrideWidth[0] * i;
			pbImageOutLine  = GetHvImageData(pImageOut, 0) + pImageOut->iStrideWidth[0] * i;
			for (int j = 0; j < pImage->iWidth; j++)
			{
				if (pbImage[j] < iThresholdValue)
				{
					(*piCount)++;
					pbImageOutLine[j] = 255;
				}
			}
		}
	}

	return S_OK;
}

// 用大津法进行二值化
// 注意 输入图像不能超过50*30 否则可能越界
HRESULT Otsu_IR(HV_COMPONENT_IMAGE *image, int *iThresholdValue, int *iCount1)
{
	BYTE8* pbImage;				// 图像指针
	int pixel;
	int iPointAll, iNo1, iNo2;
	SDWORD32 iSum, iSumP;
	DWORD32 dwVar, dwMax;
	int* piHist;
	CFastMemAlloc FastStack;
	piHist = (int*)FastStack.StackAlloc(256 * sizeof(int));

	memset(piHist, 0, 256 * sizeof(int));	// 对直方图置零

	*iCount1 = 0;
	for (int i = 0; i < image->iHeight; i++)		// 生成直方图
	{
		pbImage = (GetHvImageData(image, 0) + image->iStrideWidth[0] * i);
		for (int j = 0; j < image->iWidth; j++)
		{
			pixel = pbImage[j];
			piHist[pixel]++;
		}
	}

	iSum = iSumP = 0;
	iPointAll = 0;

	for (int k = 0; k <= 255; k++)
	{
		iSum += k * piHist[k];		// x*f(x) 质量矩
	}

	iPointAll = image->iHeight * image->iWidth;
	if (iPointAll == 0)
	{
		*iThresholdValue = 100;
		*iCount1 = 0;
		return 0;
	}
	dwMax = 0;
	iNo1 = 0;
	SDWORD32 sdwTemp1, sdwTemp2;
	for (int k = 0; k < 254; k++)
	{
		iNo1 += piHist[k];
		if (!iNo1)
		{ continue; }
		iNo2 = iPointAll - iNo1;
		if (0 == iNo2)
		{ break; }
 		iSumP += (k *piHist[k]);
		sdwTemp1 = ((iSumP + 8) << 4) / iNo1;				// 4位量化
 		sdwTemp2 = ((iSum - iSumP + 8) << 4) / iNo2;
 		dwVar = (((iNo1 * iNo2) * (sdwTemp1 - sdwTemp2) + 512) >> 10) * ((sdwTemp1 - sdwTemp2) >> 4);		// 在200 * 30的图像中 右移6位才能保证不越界
		//dwVar = (((iSumP * iNo2 - (iSum - iSumP) * iNo1) + 2) << 2) / (iNo1 * iNo2) * (((iSumP * iNo2 - (iSum - iSumP) * iNo1) + 128) >> 8); // iVar为类间方差
		if (dwVar > dwMax)
		{
			dwMax = dwVar;
			*iThresholdValue = k;
		}
	}
	for (int i = 0; i < image->iHeight; i++)
	{
		pbImage = (GetHvImageData(image, 0) + image->iStrideWidth[0] * i);
		for (int j = 0; j < image->iWidth; j++)
		{
			if(pbImage[j] >= *iThresholdValue)
				(*iCount1)++;
		}
	}
	return 1;
}

// 用大津法进行二值化
HRESULT OtsuImg_IR(HV_COMPONENT_IMAGE* pImage, HV_COMPONENT_IMAGE* pImageOut,  int* piCount, int iIsWhiteChar)
{
	BYTE8* pbImage;				// 图像指针
	int pixel;
	int iPointAll, iNo1, iNo2;
	SDWORD32 iSum, iSumP;
	DWORD32 dwVar, dwMax;
	int* piHist;
	int iThresholdValue;
	CFastMemAlloc FastStack;
	piHist = (int*)FastStack.StackAlloc(256 * sizeof(int));

	pImageOut->iHeight = pImage->iHeight;
	pImageOut->iWidth = pImage->iWidth;
	pImageOut->iStrideWidth[0] = pImage->iWidth;
	memset(piHist, 0, 256 * sizeof(int));	// 对直方图置零
	memset(GetHvImageData(pImageOut, 0), 0, pImageOut->iHeight * pImageOut->iStrideWidth[0]);

	*piCount = 0;
	for (int i = 0; i < pImage->iHeight; i++)		// 生成直方图
	{
		pbImage = (GetHvImageData(pImage, 0) + pImage->iStrideWidth[0] * i);
		for (int j = 0; j < pImage->iWidth; j++)
		{
			pixel = pbImage[j];
			piHist[pixel]++;
		}
	}

	iSum = iSumP = 0;
	iPointAll = 0;

	for (int k = 0; k <= 255; k++)
	{
		iSum += k * piHist[k];		// x*f(x) 质量矩
	}

	iPointAll = pImage->iHeight * pImage->iWidth;
	if (iPointAll == 0)
	{
		*piCount = 0;
		return 0;
	}
	dwMax = 0;
	iNo1 = 0;
	SDWORD32 sdwTemp1, sdwTemp2;
	for (int k = 0; k < 254; k++)
	{
		iNo1 += piHist[k];
		if (!iNo1)
		{ continue; }
		iNo2 = iPointAll - iNo1;
		if (0 == iNo2)
		{ break; }
		iSumP += (k *piHist[k]);
		sdwTemp1 = iSumP / iNo1;
		sdwTemp2 = (iSum - iSumP) / iNo2;
		dwVar = ((iNo1 * iNo2) >> 6) * (sdwTemp1 - sdwTemp2) * (sdwTemp1 - sdwTemp2);		// 在200 * 30的图像中 右移6位才能保证不越界
		//dwVar = (iSumP * iNo2 - (iSum - iSumP) * iNo1) / (iNo1 * iNo2) * (iSumP * iNo2 - (iSum - iSumP) * iNo1); // iVar为类间方差
		if (dwVar > dwMax)
		{
			dwMax = dwVar;
			iThresholdValue = k;
		}
	}

	BYTE8* pbImageOutLine;
	if(iIsWhiteChar)
	{
		for (int i = 0; i < pImage->iHeight; i++)
		{
			pbImage = GetHvImageData(pImage, 0) + pImage->iStrideWidth[0] * i;
			pbImageOutLine  = GetHvImageData(pImageOut, 0) + pImageOut->iStrideWidth[0] * i;
			for (int j = 0; j < pImage->iWidth; j++)
			{
				if(pbImage[j] > iThresholdValue)
				{
					(*piCount)++;
					pbImageOutLine[j] = 255;
				}
			}
		}
	}
	else
	{
		for (int i = 0; i < pImage->iHeight; i++)
		{
			pbImage = GetHvImageData(pImage, 0) + pImage->iStrideWidth[0] * i;
			pbImageOutLine  = GetHvImageData(pImageOut, 0) + pImageOut->iStrideWidth[0] * i;
			for (int j = 0; j < pImage->iWidth; j++)
			{
				if(pbImage[j] < iThresholdValue)
				{
					(*piCount)++;
					pbImageOutLine[j] = 255;
				}
			}
		}
	}

	return S_OK;
}

// 只输出二值化阈值
HRESULT OtsuForThresh(HV_COMPONENT_IMAGE* pImage, int* piThresh, int *iCount1)
{
	BYTE8* pbImage;				// 图像指针
	int pixel;
	int iPointAll, iNo1, iNo2;
	SDWORD32 iSum, iSumP;
	DWORD32 dwVar, dwMax;
	int* piHist;
	CFastMemAlloc FastStack;
	piHist = (int*)FastStack.StackAlloc(256 * sizeof(int));

	memset(piHist, 0, 256 * sizeof(int));	// 对直方图置零

	for (int i = 0; i < pImage->iHeight; i++)		// 生成直方图
	{
		pbImage = (GetHvImageData(pImage, 0) + pImage->iStrideWidth[0] * i);
		for (int j = 0; j < pImage->iWidth; j++)
		{
			pixel = pbImage[j];
			piHist[pixel]++;
		}
	}

	iSum = iSumP = 0;
	iPointAll = 0;

	for (int k = 0; k <= 255; k++)
	{
		iSum += k * piHist[k];		// x*f(x) 质量矩
	}

	iPointAll = pImage->iHeight * pImage->iWidth;
	if (iPointAll == 0)
	{
		return S_FALSE;
	}
	dwMax = 0;
	iNo1 = 0;
	SDWORD32 sdwTemp1, sdwTemp2;
	for (int k = 0; k < 254; k++)
	{
		iNo1 += piHist[k];
		if (!iNo1)	{ continue; }
		iNo2 = iPointAll - iNo1;
		if (0 == iNo2)	{ break; }
		iSumP += (k *piHist[k]);
		sdwTemp1 = iSumP / iNo1;
		sdwTemp2 = (iSum - iSumP) / iNo2;
		dwVar = ((iNo1 * iNo2) >> 6) * (sdwTemp1 - sdwTemp2) * (sdwTemp1 - sdwTemp2);		// 在200 * 30的图像中 右移6位才能保证不越界
		//dwVar = (iSumP * iNo2 - (iSum - iSumP) * iNo1) / (iNo1 * iNo2) * (iSumP * iNo2 - (iSum - iSumP) * iNo1); // iVar为类间方差
		if (dwVar > dwMax)
		{
			dwMax = dwVar;
			*piThresh = k;
		}
	}

	for (int i = 0; i < pImage->iHeight; i++)
	{
		pbImage = (GetHvImageData(pImage, 0) + pImage->iStrideWidth[0] * i);
		for (int j = 0; j < pImage->iWidth; j++)
		{
			if (pbImage[j] >= *piThresh)
				(*iCount1)++;
		}
	}
	return 1;
}

#define NAIL_SIZE 3

HRESULT GetNailPos(HV_COMPONENT_IMAGE imgPlate,
				   HV_RECT rgrcCharPos,
				   int* piCharTop,
				   int* piCharDown,
				   int* piBottomLine,
				   PLATE_COLOR PlateColor)
{
	int iIsWhiteChar = ( PlateColor == PC_BLACK ) || ( PlateColor == PC_BLUE )
		|| (PlateColor == PC_GREEN);

	*piCharTop = NAIL_SIZE;
	*piCharDown = rgrcCharPos.bottom - rgrcCharPos.top - 1 - NAIL_SIZE;
	HV_COMPONENT_IMAGE imgChar;
	imgChar.nImgType = HV_IMAGE_GRAY;

	SetHvImageData(&imgChar, 0, GetHvImageData(&imgPlate, 0)+rgrcCharPos.left+
		rgrcCharPos.top*imgPlate.iStrideWidth[0]);
	imgChar.iWidth=rgrcCharPos.right - rgrcCharPos.left;
	imgChar.iHeight=rgrcCharPos.bottom - rgrcCharPos.top;
	imgChar.iStrideWidth[0]=imgPlate.iStrideWidth[0];
	CFastMemAlloc FastStack;
	BYTE8* pbTemp = (BYTE8*)FastStack.StackAlloc(imgChar.iWidth * 2 * imgChar.iHeight);
	if (pbTemp == NULL)
	{
		return E_OUTOFMEMORY;
	}
	HV_COMPONENT_IMAGE imgTemp;
	imgTemp = imgChar;
	SetHvImageData(&imgTemp, 0, pbTemp);
	imgTemp.iStrideWidth[0] = imgChar.iWidth;

	int iCount = 0;
	Otsu(&imgChar, &imgTemp, &iCount, iIsWhiteChar);

	BYTE8* pbLine = GetHvImageData(&imgTemp, 0);
	int ithresh = MAX_INT(iCount * 3 / (imgChar.iHeight * 4), 2);
	int iTemp;
	int iCountLine = 0;
	for (int i = 0; i < NAIL_SIZE; i++)
	{
		pbLine = GetHvImageData(&imgTemp, 0) + imgTemp.iStrideWidth[0] * i;
		for (int j = 0; j < imgTemp.iWidth; j++)
		{
			iCountLine += pbLine[j];
		}
		if (iCountLine >= ithresh * 255)
		{
			if (0 == i)
			{
				iTemp = 0;
				for (int j = 0; j < imgTemp.iWidth; j++)
				{
					iTemp += pbLine[j + imgTemp.iStrideWidth[0]];
				}
				if (0 == iTemp)		// 第2行全为0时 该行不是字体
				{
					iCountLine = 0;
					continue;
				}
			}
			*piCharTop = i;
			break;
		}
	}

	iCountLine = 0;
	for (int i = imgTemp.iHeight - 1; i > imgTemp.iHeight - 1 - NAIL_SIZE; i--)
	{
		pbLine = GetHvImageData(&imgTemp, 0) + imgTemp.iStrideWidth[0] * i;
		for (int j = 0; j < imgTemp.iWidth; j++)
		{
			iCountLine += pbLine[j];
		}
		if (iCountLine >= ithresh * 255)
		{
			if (imgTemp.iHeight - 1 == i)
			{
				iTemp = 0;
				BYTE8* pbTemp = pbLine - imgTemp.iStrideWidth[0];
				for (int j = 0; j < imgTemp.iWidth; j++)
				{
					iTemp += pbTemp[j];
				}
				if (0 == iTemp)		// 第2行全为0时 该行不是字体
				{
					iCountLine = 0;
					continue;
				}
			}
			*piCharDown = i;
			break;
		}
	}

	imgChar.nImgType = HV_IMAGE_GRAY;
	imgChar.iWidth=rgrcCharPos.right - rgrcCharPos.left;
	imgChar.iHeight=rgrcCharPos.bottom - rgrcCharPos.top;
	int iRealWidth = imgChar.iWidth + (imgChar.iWidth / 3) * 2;
	int iLeft = RANGE_INT(rgrcCharPos.left - imgChar.iWidth / 3, 0,imgPlate.iWidth - iRealWidth);	// 左右扩大一些
	SetHvImageData(&imgChar, 0, GetHvImageData(&imgPlate, 0) + iLeft + rgrcCharPos.top * imgPlate.iStrideWidth[0]);
	imgChar.iWidth = iRealWidth;
	imgChar.iStrideWidth[0]=imgPlate.iStrideWidth[0];
	iCountLine = 0;

	imgTemp = imgChar;
	SetHvImageData(&imgTemp, 0, pbTemp);
	imgTemp.iStrideWidth[0] = imgChar.iWidth;

	iCount = 0;
	Otsu(&imgChar, &imgTemp, &iCount, iIsWhiteChar);

	for (int i = imgTemp.iHeight - 1; i > imgTemp.iHeight - 1 - 3; i--)
	{
		pbLine = GetHvImageData(&imgTemp, 0) + imgTemp.iStrideWidth[0] * i;
		for (int j = 0; j < imgTemp.iWidth; j++)
		{
			iCountLine += pbLine[j];
		}
		if (iCountLine + 510 >= imgChar.iWidth * 255)
		{
			*piBottomLine = i;
		}
		else
		{
			break;
		}
		iCountLine = 0;
	}

	return S_OK;
}

HRESULT GetNailPos_IR(HV_COMPONENT_IMAGE imgPlate,
				   HV_RECT rgrcCharPos,
				   int* piCharTop,
				   int* piCharDown,
				   int* piBottomLine,
				   PLATE_COLOR PlateColor)
{
	int iIsWhiteChar = ( PlateColor == PC_BLACK ) || ( PlateColor == PC_BLUE )
		|| (PlateColor == PC_GREEN);

	*piCharTop = NAIL_SIZE;
	*piCharDown = rgrcCharPos.bottom - rgrcCharPos.top - 1 - NAIL_SIZE;
	HV_COMPONENT_IMAGE imgChar;
	imgChar.nImgType = HV_IMAGE_GRAY;

	SetHvImageData(&imgChar, 0, GetHvImageData(&imgPlate, 0)+rgrcCharPos.left+
		rgrcCharPos.top*imgPlate.iStrideWidth[0]);
	imgChar.iWidth=rgrcCharPos.right - rgrcCharPos.left;
	imgChar.iHeight=rgrcCharPos.bottom - rgrcCharPos.top;
	imgChar.iStrideWidth[0]=imgPlate.iStrideWidth[0];
	CFastMemAlloc FastStack;
	BYTE8* pbTemp = (BYTE8*)FastStack.StackAlloc(imgChar.iWidth * 2 * imgChar.iHeight);
	if (pbTemp == NULL)
	{
		return E_OUTOFMEMORY;
	}
	HV_COMPONENT_IMAGE imgTemp;
	imgTemp = imgChar;
	SetHvImageData(&imgTemp, 0, pbTemp);
	imgTemp.iStrideWidth[0] = imgChar.iWidth;

	int iCount = 0;
	OtsuImg_IR(&imgChar, &imgTemp, &iCount, iIsWhiteChar);

	BYTE8* pbLine = GetHvImageData(&imgTemp, 0);
	int ithresh = MAX_INT(iCount * 3 / (imgChar.iHeight * 4), 2);
	int iTemp;
	int iCountLine = 0;
	for (int i = 0; i < NAIL_SIZE; i++)
	{
		pbLine = GetHvImageData(&imgTemp, 0) + imgTemp.iStrideWidth[0] * i;
		for (int j = 0; j < imgTemp.iWidth; j++)
		{
			iCountLine += pbLine[j];
		}
		if(iCountLine >= ithresh * 255)
		{
			if(0 == i)
			{
				iTemp = 0;
				for (int j = 0; j < imgTemp.iWidth; j++)
				{
					iTemp += pbLine[j + imgTemp.iStrideWidth[0]];
				}
				if(0 == iTemp)		// 第2行全为0时 该行不是字体
				{
					iCountLine = 0;
					continue;
				}
			}
			*piCharTop = i;
			break;
		}
	}

	iCountLine = 0;
	for (int i = imgTemp.iHeight - 1; i > imgTemp.iHeight - 1 - NAIL_SIZE; i--)
	{
		pbLine = GetHvImageData(&imgTemp, 0) + imgTemp.iStrideWidth[0] * i;
		for (int j = 0; j < imgTemp.iWidth; j++)
		{
			iCountLine += pbLine[j];
		}
		if (iCountLine >= ithresh * 255)
		{
			if(imgTemp.iHeight - 1 == i)
			{
				iTemp = 0;
				BYTE8* pbTemp = pbLine - imgTemp.iStrideWidth[0];
				for (int j = 0; j < imgTemp.iWidth; j++)
				{
					iTemp += pbTemp[j];
				}
				if(0 == iTemp)		// 第2行全为0时 该行不是字体
				{
					iCountLine = 0;
					continue;
				}
			}
			*piCharDown = i;
			break;
		}
	}

	return S_OK;
}
#define GRAYLEVEL 256
int Dynamic_Detect( HV_COMPONENT_IMAGE imgFrame, unsigned int &nThreshold )
{
	int nWidth = imgFrame.iWidth;
	int nHeight = imgFrame.iHeight;
	RESTRICT_PBYTE8 pbyImageGray = GetHvImageData(&imgFrame, 0);

	int i,j;
	RESTRICT_PBYTE8 pbyPtr = pbyImageGray;
	unsigned int nSquareSize = 0;
	int nPThrLeft, nPThrRight;
	int start, end;

	CFastMemAlloc cStack;
	unsigned int* nH0 = (unsigned int *)cStack.StackAlloc(sizeof(unsigned int) * GRAYLEVEL);
	memset(nH0, 0, GRAYLEVEL * sizeof(unsigned int));

	for (i = 8; i < nHeight-8; i+=4)
	{
		pbyPtr = pbyImageGray + i*imgFrame.iStrideWidth[0];
		for (j = 8; j < nWidth-8; j+=4)
		{
			nSquareSize++;
			++nH0[*(pbyPtr+j)];
		}
	}

	nPThrLeft = nSquareSize/100;
	nPThrLeft = nPThrRight = MAX_INT( 2, nPThrLeft );

	unsigned int nLeftGray = 0;
	for (start = 0; start < GRAYLEVEL - 2; ++start)
	{
		nLeftGray += nH0[start];
		nH0[start] = 0;
		if (nLeftGray > nPThrLeft)
			break;
	}

	unsigned int nRightGray = 0;
	for (end = GRAYLEVEL - 1; end > start + 1; --end)
	{
		nRightGray += nH0[end];
		nH0[end] = GRAYLEVEL - 1;
		if (nRightGray > nPThrRight)
			break;
	}

	nThreshold = 8;

	if ( (end-start) >= 230 )
	{
		nThreshold = 8;
	}
	else if ( (end-start) > 190 )
	{
		nThreshold = 8;
	}
	else if ( (end-start) > 150 )
	{
		nThreshold = 6;
	}
	else if ( (end-start) > 125 )
	{
		nThreshold = 5;
	}
	else if ( (end-start) > 90 )
	{
		nThreshold = 4;
	}
	else
		nThreshold = 3;

	return 0;
}
#define	WIN_WIDTH			16
#define	WIN_HEIGHT			4
#define	WIN_SIZE			(WIN_WIDTH * WIN_HEIGHT)
#define	HALF_WIN_WIDTH		(WIN_WIDTH  / 2)
#define	HALF_WIN_HEIGHT		(WIN_HEIGHT / 2)
#define	DEFAULT_BRIGHT		100
#define	DEFAULT_THRESHOLD	6
HRESULT IMAGE_Gray2BinNew( int iIsWhiteChar, BYTE8 *pbBinImg,  HV_COMPONENT_IMAGE imgFrame ,int nContrast) 
{
	unsigned int g_nThreshold = 10;
	if( nContrast == -1 )
	{
		Dynamic_Detect( imgFrame, g_nThreshold );
	}
	else
	{
		g_nThreshold = nContrast;
	}

	RESTRICT_PBYTE8 pbyGrayImage = GetHvImageData(&imgFrame, 0);
	int unWidth = imgFrame.iWidth;
	int unHeight = imgFrame.iHeight;
	int nStrideWidth = imgFrame.iStrideWidth[0];

	int i, j, k;

	unsigned int	unIntWidth, unWidth8, unResWidth;
	unsigned int unSum;
	int nMean; 
	int nGray, nTemp, nThreshold;
	unsigned char unBinLight;

	RESTRICT_PBYTE8 pSrc, pGray;
	RESTRICT_PDWORD32 punIntegral, punIntLine, punIntLast;
	RESTRICT_PDWORD32 punSum1;
	RESTRICT_PBYTE8 punBin;

	unWidth8	= unWidth & (~7);            
	unResWidth	= unWidth & (7);             
	unIntWidth  = unWidth8 + WIN_WIDTH;		

	int nIntegralBufSize = sizeof(DWORD32) * unIntWidth * (WIN_HEIGHT + 1); 
	CFastMemAlloc cStack;
	punIntegral = (DWORD32*)cStack.StackAlloc(nIntegralBufSize);   

	punBin = pbBinImg + HALF_WIN_HEIGHT * unWidth; 

	memset(punIntegral,0,unIntWidth*sizeof(DWORD32));  

	punIntLast = punIntegral;					
	punIntLine = punIntegral + unIntWidth;		
	pSrc 	   = pbyGrayImage;			       
	RESTRICT_PBYTE8 pSrcLine = pbyGrayImage;  

	for (i=0; i<WIN_HEIGHT; i++, pSrcLine += nStrideWidth)
	{
		pSrc = pSrcLine;
		unSum  = 0;

		for (j=7; j>=0; j--)
		{
			unSum += pSrc[j];
			*punIntLine++ = *punIntLast++ + unSum;
		}

		for (j=0; j<(int)unWidth8; j++)
		{
			unSum += *pSrc++;
			*punIntLine++ = *punIntLast++ + unSum;
		}

		for (j=1; j<=8; j++)
		{
			unSum += *(pSrc-j);
			*punIntLine++ = *punIntLast++ + unSum;
		}
	}

	RESTRICT_PBYTE8 pGrayLine = (pbyGrayImage + HALF_WIN_HEIGHT * nStrideWidth);  
	punSum1    = punIntegral + unIntWidth;		
	punIntLine = punIntegral;					

	for (i=WIN_HEIGHT; i<unHeight; i++, pSrcLine += nStrideWidth, pGrayLine += nStrideWidth)
	{
		nThreshold = g_nThreshold;	 	       

		pSrc = pSrcLine;
		pGray = pGrayLine;

		unSum  = 0;	
		for (j=7; j>=0; j--)  
		{
			unSum += pSrc[j];
			*punIntLine++ = (*punIntLast++) + unSum;
		}

		for (j=0; j<8; j++)
		{
			unSum += *pSrc++;
			*punIntLine++ = (*punIntLast++) + unSum;
		}
		for (j =0;j<unWidth8-8;j++)
		{
			unBinLight=0;

			unSum += *pSrc++;
			*punIntLine = unSum + (*punIntLast++);
			nMean 		  = ((*punIntLine) + (*punSum1) - *(punSum1+WIN_WIDTH) - *(punIntLine-WIN_WIDTH)) >> 6;
			punSum1++;     
			punIntLine++;  
			nGray 		  = *pGray++;     

			if(iIsWhiteChar) 
			{
				nTemp 	   		 =  nMean + nThreshold;		
				unBinLight = (nGray>nTemp) ? 0xFF : 0;
			}
			else 
			{
				nTemp 	   		 =  nMean - nThreshold;		
				unBinLight = (nGray>nTemp) ? 0 : 0xFF;
			}

			*punBin++ = unBinLight; 

		}	

		unBinLight=0;
		for (k=0; k<8; k++)
		{
			unSum += *(pSrc-k-1);
			punIntLine[0] = unSum + *punIntLast++;
			nMean 		  = (punIntLine[0] + *punSum1 - *(punSum1+WIN_WIDTH) - punIntLine[-WIN_WIDTH]) >> 6;
			punSum1++;
			nGray 		  = *pGray++;
			punIntLine++;


			if(iIsWhiteChar)
			{
				nTemp 	   		 = nMean + nThreshold;		
				unBinLight = (nGray>nTemp) ? 0xFF : 0;
			}
			else 
			{
				nTemp 	   		 = nMean - nThreshold;		
				unBinLight = (nGray>nTemp) ? 0 : 0xFF;
			}
			*punBin++ = unBinLight; 

		}

		punSum1 += WIN_WIDTH;    
		if (punSum1 >= punIntegral + 5 * unIntWidth)
		{
			punSum1 = punIntegral;   
		}

		if (punIntLast >= punIntegral + 5 * unIntWidth)
		{
			punIntLast = punIntegral;
		}

		if (punIntLine >= punIntegral + 5 * unIntWidth)
		{
			punIntLine = punIntegral;
		}
		punBin+=unResWidth;  
	}

	return 0;
}
