#include "hvutils.h"
#include "swimageobj.h"
#include "TiImageProcesser.h"
#include "gba-jpeg-decode.h"
#include "DspLinkCmd.h"

extern "C" BYTE8 g_bOutBuffer[];

extern IReferenceComponentImage* g_pRefImageCbYCrY;

//----------------------------------------------------

// BT1120 UV to SplitUV
extern "C" void BT1120UV_To_SplitUV(
	PBYTE8 pbSrcUV,
	int iWidth,
	int iHeight,
	PBYTE8 pbDstU,
	PBYTE8 pbDstV
);

extern "C" void BT1120UV_To_SplitUV_ex(
	PBYTE8 pbSrcUV,
	int iWidth,
	int iHeight,
	PBYTE8 pbDstU,
	PBYTE8 pbDstV
);

extern "C" void BT1120Field_To_YUV422P(
    RESTRICT_PBYTE8 pbSrcY,
    RESTRICT_PBYTE8 pbSrcUV,
	int iWidth,
	int iHeigth,
    RESTRICT_PBYTE8 pbDstY,
    RESTRICT_PBYTE8 pbDstU,
    RESTRICT_PBYTE8 pbDstV
);

extern "C" void BT1120UV_1600_1200_SplitUV(
	const PBYTE8 pbSrcBT1120UV,
	PBYTE8 pbDstU,
	PBYTE8 pbDstV
	);

extern "C" void BT1120Field_1600_600_To_YUV422P_1600_1200(
	RESTRICT_PBYTE8 pbSrcY,
	RESTRICT_PBYTE8 pbSrcUV,
	RESTRICT_PBYTE8 pbDstY,
	RESTRICT_PBYTE8 pbDstU,
	RESTRICT_PBYTE8 pbDstV
	);

extern "C" void BT1120_ROTATE_Y_1200_1600_To_YUV422P_1200_1600(
	const PBYTE8 pbSrcY,
	const PBYTE8 pbSrcUV,
	PBYTE8 pbDstY,
	PBYTE8 pbDstU,
	PBYTE8 pbDstV
	);

extern "C" void BT1120_2448_1024_To_YUV420P_2448_2048(
	PBYTE8 pbSrcY,
	PBYTE8 pbSrcUV,
	PBYTE8 pbDstY,
	PBYTE8 pbDstU,
	PBYTE8 pbDstV
	);

extern "C" void BT1120_1600_1200_To_UYVY_1600_1200(
	const PBYTE8 pbImgSrcY, 
	const PBYTE8 pbImgSrcUV, 
	PBYTE8 pbCbYCrY
	);

extern "C" void BT1120Field_1600_600_To_UYVY_1600_1200(
	const PBYTE8 pbImgSrcY, 
	const PBYTE8 pbImgSrcUV, 
	PBYTE8 pbCbYCrY
	);

extern "C" void BT1120_ROTATE_Y_1200_1600_To_UYVY_1200_1600(
	const PBYTE8 pbSrcY,
	const PBYTE8 pbSrcUV,
	PBYTE8 pbDstUYVY
	);

//----------------------------------------------------

extern "C" HRESULT XDM_JpegEncode_CbYCrY(
	const PBYTE8 	pbSrcCbYCrY,
	PBYTE8 			pbJpegData,
	PDWORD32 		pdwJpegLen,
	DWORD32			dwWidth,
	DWORD32			dwHeight,
	DWORD32			dwQValue
	);

extern "C" HRESULT XDM_JpegEncode_Yuv422P(
	const PBYTE8 	pbSrcYUV422P_Y,
	const PBYTE8	pbSrcYUV422P_U,
	const PBYTE8	pbSrcYUV422P_V,
	PBYTE8 			pbJpegData,
	PDWORD32 		pdwJpegLen,
	DWORD32			dwWidth,
	DWORD32			dwHeight,
	DWORD32			dwQValue
	);

extern "C" HRESULT XDM_JpegEncode_Yuv420P(
	const PBYTE8 	pbSrcYUV420P_Y,
	const PBYTE8	pbSrcYUV420P_U,
	const PBYTE8	pbSrcYUV420P_V,
	PBYTE8 			pbJpegData,
	PDWORD32 		pdwJpegLen,
	DWORD32			dwWidth,
	DWORD32			dwHeight,
	DWORD32			dwQValue
	);

// --- JpegDecode ---

HRESULT JPG2YUV(HV_COMPONENT_IMAGE imgJpeg, HV_COMPONENT_IMAGE& imgYuv)
{
	HRESULT hr = S_FALSE;
	JPEG_Decoder cJpegDecoder;
	const BYTE8* pJpegData = GetHvImageData(&imgJpeg, 0);
	BOOL fTurn = FALSE;

	if	(JPEG_Decoder_ReadHeaders(&cJpegDecoder, &pJpegData) 
		&& cJpegDecoder.frame.width > 0 
		&& cJpegDecoder.frame.height > 0)
	{
		if (JPEG_Decoder_ReadImage_YUV_EX(&cJpegDecoder, &pJpegData, &imgYuv, NULL, fTurn))
		{
			hr = S_OK;
		}
		else
		{
			hr = E_FAIL;
		}
	}
	return hr;
}

HRESULT JPG2YUV_TI(HV_COMPONENT_IMAGE imgJpeg, HV_COMPONENT_IMAGE& imgYuv)
{
	HRESULT hr = S_OK;

	if (imgJpeg.nImgType == HV_IMAGE_JPEG )
	{
		HV_cachewait();
		HV_invalidate(GetHvImageData(&imgYuv, 0), imgYuv.iStrideWidth[0]);

		DWORD32 dwStride;
		DWORD32 dwYUVBufferSize;
		if ( XDM_JpegDecode(
			GetHvImageData(&imgJpeg, 0),
			imgJpeg.iWidth,
			GetHvImageData(&imgYuv, 0),
			&dwYUVBufferSize,
			(DWORD32*)&imgYuv.iWidth,
			(DWORD32*)&imgYuv.iHeight,
			&dwStride) != S_OK )
		{
			return E_FAIL;
		}
		if (imgYuv.iHeight > (imgYuv.iWidth >> 1))
		{
			imgYuv.iStrideWidth[0] = (imgYuv.iWidth << 2);
			imgYuv.iHeight >>= 1;
		}
		else
		{
			imgYuv.iStrideWidth[0] = (imgYuv.iWidth << 1);
		}
	}
	else
	{
		hr = E_FAIL;
	}
	return hr;
}

// --- JpegEncode ---

TEXT * GetTextBuffer(BYTE8 *pbBuf, int ch)
{
	for(;;)
	{
		TEXT *txt = (TEXT *)pbBuf; pbBuf += sizeof(TEXT);
		if(txt->iSize <= 0)
		{
			return NULL;
		}		
		if(txt->iAlpha == ch)
		{
			return txt;
		}
		pbBuf += txt->iSize;
	}
	return NULL;
}

HRESULT YUV2JPG_TI(DWORD32 dwCompressRate, DWORD32 dwJpegType, JPEG_ENCODE_DATA *pData)
{
	HV_COMPONENT_IMAGE &imgYuv = pData->hvImageYuv;
    HV_COMPONENT_IMAGE &imgJpeg = pData->hvImageJpg;

	if ( imgJpeg.nImgType == HV_IMAGE_JPEG )
	{
		if ( dwCompressRate < 1 || dwCompressRate > 100 )
		{
			dwCompressRate = 86;  // Ti的Jpeg编码库默认的压缩率即为86
		}

		DWORD32 dwJPEGBufferSize = imgJpeg.iWidth;  // Jpeg缓冲区大小

		if ( imgYuv.nImgType == HV_IMAGE_CbYCrY )
		{
			if ( XDM_JpegEncode_CbYCrY(
				GetHvImageData(&imgYuv, 0),
				g_bOutBuffer,
				&dwJPEGBufferSize,
				imgYuv.iWidth,
				imgYuv.iHeight,
				dwCompressRate
				) == S_OK )
			{
				if ( dwJPEGBufferSize < imgJpeg.iWidth )
				{
					memcpy(GetHvImageData(&imgJpeg, 0), g_bOutBuffer, dwJPEGBufferSize);
					imgJpeg.iWidth = dwJPEGBufferSize;
					return S_OK;
				}
			}
		}
		else if ( imgYuv.nImgType == HV_IMAGE_YUV_422 )
		{
			if ( XDM_JpegEncode_Yuv422P(
				GetHvImageData(&imgYuv, 0),
				GetHvImageData(&imgYuv, 1),
				GetHvImageData(&imgYuv, 2),
				g_bOutBuffer,
				&dwJPEGBufferSize,
				imgYuv.iWidth,
				imgYuv.iHeight,
				dwCompressRate
				) == S_OK )
			{
				if ( dwJPEGBufferSize < imgJpeg.iWidth )
				{
					memcpy(GetHvImageData(&imgJpeg, 0), g_bOutBuffer, dwJPEGBufferSize);
					imgJpeg.iWidth = dwJPEGBufferSize;
					return S_OK;
				}
			}
		}
		else if ( imgYuv.nImgType == HV_IMAGE_BT1120 )
		{	
			//时间+位图叠加
			int orgX = pData->x;
			int offsetH = 0;
			for(char *ch = pData->szTime; *ch != '\0'; ch++)
			{
				if(*ch == '\n')
				{
					offsetH++;
					pData->x = orgX;
					continue;
				}

				TEXT *txt = GetTextBuffer(pData->phys, *ch);
				if(NULL == txt)
				{
					continue;
				}

				BYTE8 *pbBuf = (BYTE8 *)txt + sizeof(TEXT);
				if(pData->x < imgYuv.iWidth && pData->y + offsetH*txt->iHeight < imgYuv.iHeight)
				{
					int width = txt->iWidth;
					if(width > imgYuv.iWidth - pData->x)
					{
						width = imgYuv.iWidth - pData->x;
					}
					for(int h = 0; h < txt->iHeight && h + pData->y + offsetH*txt->iHeight < imgYuv.iHeight; h++)
					{
						PBYTE8 dstY  = GetHvImageData(&imgYuv, 0) + imgYuv.iWidth*(h + pData->y + offsetH*txt->iHeight) + pData->x;
						PBYTE8 dstUV = GetHvImageData(&imgYuv, 1) + imgYuv.iWidth*(h + pData->y + offsetH*txt->iHeight) + pData->x;

						for(int w = 0; w < width; w++)
						{	
							if(pbBuf[h*txt->iWidth + w])
							{
								//y
								*dstY++ = pData->yColor;
								//uv
								if(!(w%2))
								{
									*dstUV++ = pData->uColor;
									*dstUV++ = pData->vColor;
								}
							}
							else
							{
								dstY++;
								if(!(w%2))
								{
									dstUV += 2;
								}
							}
						}
					}
				}
				//偏移一个字
				pData->x += ((txt->iWidth+1)&~1);
			}

			if ( 1936 == imgYuv.iWidth && 1452 == imgYuv.iHeight )
			{
				HV_COMPONENT_IMAGE imgCbYCrY;
				if (g_pRefImageCbYCrY->GetImage(&imgCbYCrY) == S_OK)
				{
					if(0 == dwJpegType)
					{
						//DWORD32 dwTick = GetSystemTick();
						BT1120UV_To_SplitUV(
							GetHvImageData(&imgYuv, 1),
							imgYuv.iWidth,
							imgYuv.iHeight,
							GetHvImageData(&imgCbYCrY, 0),
							GetHvImageData(&imgCbYCrY, 0) + imgYuv.iWidth / 2 * imgYuv.iHeight
							);
						//DWORD32 dwConverMs = GetSystemTick() - dwTick;
						//char szMsg[64] = {0};
						//sprintf( szMsg, "<conver:%d>", dwConverMs );
						//strcat(g_szDebugInfo, szMsg );

						//dwTick = GetSystemTick();
						if ( XDM_JpegEncode_Yuv422P(
							GetHvImageData(&imgYuv, 0),
							GetHvImageData(&imgCbYCrY, 0),
							GetHvImageData(&imgCbYCrY, 0) + imgYuv.iWidth / 2 * imgYuv.iHeight,
							g_bOutBuffer,
							&dwJPEGBufferSize,
							imgYuv.iWidth,
							imgYuv.iHeight,
							dwCompressRate
							) == S_OK )
						{

							if ( dwJPEGBufferSize < imgJpeg.iWidth )
							{
								memcpy(GetHvImageData(&imgJpeg, 0), g_bOutBuffer, dwJPEGBufferSize);
								imgJpeg.iWidth = dwJPEGBufferSize;
								//DWORD32 dwEncode = GetSystemTick() - dwTick;
								//sprintf( szMsg, "<encode:%d>", dwEncode );
								//strcat(g_szDebugInfo, szMsg );
								return S_OK;
							}
						}
					}
					else
					{
						// todo.
						return E_FAIL;

						BT1120_1600_1200_To_UYVY_1600_1200(
							(PBYTE8)GetHvImageData(&imgYuv, 0),
							(PBYTE8)GetHvImageData(&imgYuv, 1),
							GetHvImageData(&imgCbYCrY, 0)
							);

						if ( dwCompressRate < 1 || dwCompressRate > 100 )
						{
							dwCompressRate = 86;  // Ti的Jpeg编码库默认的压缩率即为86
						}

						if ( S_OK == XDM_JpegEncode_CbYCrY(
							GetHvImageData(&imgCbYCrY, 0),
							g_bOutBuffer,
							&dwJPEGBufferSize,
							1600,//imgYuv.iWidth,
							1200,//imgYuv.iHeight,
							dwCompressRate) )
						{
							if ( dwJPEGBufferSize < imgJpeg.iWidth )
							{
								memcpy(GetHvImageData(&imgJpeg, 0), g_bOutBuffer, dwJPEGBufferSize);
								imgJpeg.iWidth = dwJPEGBufferSize;
								return S_OK;
							}
						}

					}
				}
			}
			else if ( 1600 == imgYuv.iWidth && 1200 == imgYuv.iHeight )
			{
				HV_COMPONENT_IMAGE imgCbYCrY;
				if (g_pRefImageCbYCrY->GetImage(&imgCbYCrY) == S_OK)
				{
					if(0 == dwJpegType)
					{
						BT1120UV_1600_1200_SplitUV(
							GetHvImageData(&imgYuv, 1),
							GetHvImageData(&imgCbYCrY, 0),
							GetHvImageData(&imgCbYCrY, 0) + 800*1200
							);

						if ( XDM_JpegEncode_Yuv422P(
							GetHvImageData(&imgYuv, 0),
							GetHvImageData(&imgCbYCrY, 0),
							GetHvImageData(&imgCbYCrY, 0) + 800*1200,
							g_bOutBuffer,
							&dwJPEGBufferSize,
							1600,
							1200,
							dwCompressRate
							) == S_OK )
						{

							if ( dwJPEGBufferSize < imgJpeg.iWidth )
							{
								memcpy(GetHvImageData(&imgJpeg, 0), g_bOutBuffer, dwJPEGBufferSize);
								imgJpeg.iWidth = dwJPEGBufferSize;
								return S_OK;
							}
						}
					}
					else
					{
						BT1120_1600_1200_To_UYVY_1600_1200(
							(PBYTE8)GetHvImageData(&imgYuv, 0),
							(PBYTE8)GetHvImageData(&imgYuv, 1),
							GetHvImageData(&imgCbYCrY, 0)
							);

						if ( dwCompressRate < 1 || dwCompressRate > 100 )
						{
							dwCompressRate = 86;  // Ti的Jpeg编码库默认的压缩率即为86
						}

						if ( S_OK == XDM_JpegEncode_CbYCrY(
							GetHvImageData(&imgCbYCrY, 0),
							g_bOutBuffer,
							&dwJPEGBufferSize,
							1600,//imgYuv.iWidth,
							1200,//imgYuv.iHeight,
							dwCompressRate) )
						{
							if ( dwJPEGBufferSize < imgJpeg.iWidth )
							{
								memcpy(GetHvImageData(&imgJpeg, 0), g_bOutBuffer, dwJPEGBufferSize);
								imgJpeg.iWidth = dwJPEGBufferSize;
								return S_OK;
							}
						}

					}
				}
			}
			else if ( 2448 == imgYuv.iWidth && 1024 == imgYuv.iHeight )
			{
				HV_COMPONENT_IMAGE imgCbYCrY;
				if (g_pRefImageCbYCrY->GetImage(&imgCbYCrY) == S_OK)
				{
					PBYTE8 pbSrcYUV420P_Y = GetHvImageData(&imgCbYCrY, 0);
					PBYTE8 pbSrcYUV420P_U = GetHvImageData(&imgCbYCrY, 0) + 2448*2048;
					PBYTE8 pbSrcYUV420P_V = GetHvImageData(&imgCbYCrY, 0) + 2448*2048 + 1224*1024;

					// 在tskMessageDispatch函数中已经对整个L2进行了wbInv
					//CACHE_wbInvL2(GetHvImageData(&imgYuv, 0), 2448*1024, CACHE_WAIT);
					//CACHE_wbInvL2(GetHvImageData(&imgYuv, 1), 2448*1024, CACHE_WAIT);
					//CACHE_wbInvL2(GetHvImageData(&imgCbYCrY, 0), imgCbYCrY.iWidth*imgCbYCrY.iHeight*2, CACHE_WAIT);
					BT1120_2448_1024_To_YUV420P_2448_2048(
						GetHvImageData(&imgYuv, 0),
						GetHvImageData(&imgYuv, 1),
						pbSrcYUV420P_Y,
						pbSrcYUV420P_U,
						pbSrcYUV420P_V
						);

					if ( S_OK == XDM_JpegEncode_Yuv420P(
						pbSrcYUV420P_Y,
						pbSrcYUV420P_U,
						pbSrcYUV420P_V,
						g_bOutBuffer,
						&dwJPEGBufferSize,
						2448,
						2048,
						dwCompressRate) )
					{
						if ( dwJPEGBufferSize < imgJpeg.iWidth )
						{
							memcpy(GetHvImageData(&imgJpeg, 0), g_bOutBuffer, dwJPEGBufferSize);
							imgJpeg.iWidth = dwJPEGBufferSize;
							return S_OK;
						}
					}
				}
			}
		}
		else if ( imgYuv.nImgType == HV_IMAGE_BT1120_FIELD )
		{
			if ( 1936 == imgYuv.iWidth && 726 == imgYuv.iHeight )
			{
				HV_COMPONENT_IMAGE imgCbYCrY;
				if (g_pRefImageCbYCrY->GetImage(&imgCbYCrY) == S_OK)
				{
					if( 0 == dwJpegType )
					{
						BT1120Field_To_YUV422P(
							GetHvImageData(&imgYuv, 0),
							GetHvImageData(&imgYuv, 1),
							imgYuv.iWidth, 
							imgYuv.iHeight,
							GetHvImageData(&imgCbYCrY, 0),
							GetHvImageData(&imgCbYCrY, 0) + imgYuv.iWidth*imgYuv.iHeight*2,
							GetHvImageData(&imgCbYCrY, 0) + imgYuv.iWidth*imgYuv.iHeight*2 + imgYuv.iWidth*imgYuv.iHeight
							);

						if ( XDM_JpegEncode_Yuv422P(
							GetHvImageData(&imgCbYCrY, 0),
							GetHvImageData(&imgCbYCrY, 0) + imgYuv.iWidth*imgYuv.iHeight*2,
							GetHvImageData(&imgCbYCrY, 0) + imgYuv.iWidth*imgYuv.iHeight*2 + imgYuv.iWidth*imgYuv.iHeight,
							g_bOutBuffer,
							&dwJPEGBufferSize,
							imgYuv.iWidth,
							imgYuv.iHeight * 2,
							dwCompressRate
							) == S_OK )
						{
							if ( dwJPEGBufferSize < imgJpeg.iWidth )
							{
								memcpy(GetHvImageData(&imgJpeg, 0), g_bOutBuffer, dwJPEGBufferSize);
								imgJpeg.iWidth = dwJPEGBufferSize;
								return S_OK;
							}
						}
					}
					else
					{
						return E_NOTIMPL;
					}
				}
			}
			else if ( 1600 == imgYuv.iWidth && 600 == imgYuv.iHeight )
			{
				HV_COMPONENT_IMAGE imgCbYCrY;
				if (g_pRefImageCbYCrY->GetImage(&imgCbYCrY) == S_OK)
				{
					if( 0 == dwJpegType )
					{
						BT1120Field_1600_600_To_YUV422P_1600_1200(
							GetHvImageData(&imgYuv, 0),
							GetHvImageData(&imgYuv, 1),
							GetHvImageData(&imgCbYCrY, 0),
							GetHvImageData(&imgCbYCrY, 0) + 1600*1200,
							GetHvImageData(&imgCbYCrY, 0) + 1600*1200 + 800*1200
							);

						if ( XDM_JpegEncode_Yuv422P(
							GetHvImageData(&imgCbYCrY, 0),
							GetHvImageData(&imgCbYCrY, 0) + 1600*1200,
							GetHvImageData(&imgCbYCrY, 0) + 1600*1200 + 800*1200,
							g_bOutBuffer,
							&dwJPEGBufferSize,
							1600,
							1200,
							dwCompressRate
							) == S_OK )
						{
							if ( dwJPEGBufferSize < imgJpeg.iWidth )
							{
								memcpy(GetHvImageData(&imgJpeg, 0), g_bOutBuffer, dwJPEGBufferSize);
								imgJpeg.iWidth = dwJPEGBufferSize;
								return S_OK;
							}
						}
					}
					else
					{
						BT1120Field_1600_600_To_UYVY_1600_1200(
							(PBYTE8)GetHvImageData(&imgYuv, 0),
							(PBYTE8)GetHvImageData(&imgYuv, 1),
							GetHvImageData(&imgCbYCrY, 0)
							);

						if ( dwCompressRate < 1 || dwCompressRate > 100 )
						{
							dwCompressRate = 86;  // Ti的Jpeg编码库默认的压缩率即为86
						}

						if ( S_OK == XDM_JpegEncode_CbYCrY(
							GetHvImageData(&imgCbYCrY, 0),
							g_bOutBuffer,
							&dwJPEGBufferSize,
							1600,//imgYuv.iWidth,
							1200,//imgYuv.iHeight * 2,
							dwCompressRate) )
						{
							if ( dwJPEGBufferSize < imgJpeg.iWidth )
							{
								memcpy(GetHvImageData(&imgJpeg, 0), g_bOutBuffer, dwJPEGBufferSize);
								imgJpeg.iWidth = dwJPEGBufferSize;
								return S_OK;
							}
						}
					}
				}
			}
		}
		else if ( imgYuv.nImgType == HV_IMAGE_BT1120_ROTATE_Y )
		{
			if ( 1200 == imgYuv.iWidth && 1600 == imgYuv.iHeight )
			{
				HV_COMPONENT_IMAGE imgCbYCrY;
				if (g_pRefImageCbYCrY->GetImage(&imgCbYCrY) == S_OK)
				{
					if( 0 == dwJpegType )
					{
						BT1120_ROTATE_Y_1200_1600_To_YUV422P_1200_1600(
							GetHvImageData(&imgYuv, 0),
							GetHvImageData(&imgYuv, 1),
							GetHvImageData(&imgCbYCrY, 0),
							GetHvImageData(&imgCbYCrY, 0) + 1600*1200,
							GetHvImageData(&imgCbYCrY, 0) + 1600*1200 + 800*1200
							);

						if ( XDM_JpegEncode_Yuv422P(
							GetHvImageData(&imgCbYCrY, 0),
							GetHvImageData(&imgCbYCrY, 0) + 1600*1200,
							GetHvImageData(&imgCbYCrY, 0) + 1600*1200 + 800*1200,
							g_bOutBuffer,
							&dwJPEGBufferSize,
							1200,
							1600,
							dwCompressRate
							) == S_OK )
						{
							if ( dwJPEGBufferSize < imgJpeg.iWidth )
							{
								memcpy(GetHvImageData(&imgJpeg, 0), g_bOutBuffer, dwJPEGBufferSize);
								imgJpeg.iWidth = dwJPEGBufferSize;
								return S_OK;
							}
						}
					}
					else
					{
						BT1120_ROTATE_Y_1200_1600_To_UYVY_1200_1600(
							(PBYTE8)GetHvImageData(&imgYuv, 0),
							(PBYTE8)GetHvImageData(&imgYuv, 1),
							GetHvImageData(&imgCbYCrY, 0)
							);

						if ( dwCompressRate < 1 || dwCompressRate > 100 )
						{
							dwCompressRate = 86;  // Ti的Jpeg编码库默认的压缩率即为86
						}

						if ( S_OK == XDM_JpegEncode_CbYCrY(
							GetHvImageData(&imgCbYCrY, 0),
							g_bOutBuffer,
							&dwJPEGBufferSize,
							1200,
							1600,
							dwCompressRate) )
						{
							if ( dwJPEGBufferSize < imgJpeg.iWidth )
							{
								memcpy(GetHvImageData(&imgJpeg, 0), g_bOutBuffer, dwJPEGBufferSize);
								imgJpeg.iWidth = dwJPEGBufferSize;
								return S_OK;
							}
						}
					}
				}
			}
		}
	}

	return E_FAIL;
}
