#include <std.h>
#include <alg.h>
#include <ialg.h>
#include <csl_cache.h>
#include "CamDsp.h"
#include "StringOverlay.h"

extern int g_iErrCode;

extern void HvOpenCameraDma();
extern void HvCloseCameraDma();

extern BYTE8 g_bInBuffer[];
extern BYTE8 g_bOutBuffer[];

HRESULT YUV2JPG_TI(
    const JPEG_ENCODE_PARAM* pJpegEncodeParam,
    const JPEG_ENCODE_DATA* pJpegEncodeData,
    JPEG_ENCODE_RESPOND* pJpegEncodeRespond
)
{
	DWORD32 dwCompressRate = pJpegEncodeParam->dwCompressRate;
	DWORD32 dwJpegDataType = pJpegEncodeParam->dwJpegDataType;
    HV_COMPONENT_IMAGE hvImageSrc = pJpegEncodeData->hvImageYuv;
    HV_COMPONENT_IMAGE hvImageDst = pJpegEncodeData->hvImageJpg;

    PBYTE8 pbSrcYUV422P_Y = NULL;
    PBYTE8 pbSrcYUV422P_U = NULL;
    PBYTE8 pbSrcYUV422P_V = NULL;
    DWORD32 dwJpegLen = 0;

    if ( HV_IMAGE_JPEG != hvImageDst.nImgType )
    {
        return S_FALSE;
    }

    if ( HV_IMAGE_BT1120_UV == hvImageSrc.nImgType )
    {
		if( 0 == dwJpegDataType )
		{
			pbSrcYUV422P_Y = (PBYTE8)hvImageSrc.rgImageData[0].phys;
	    	pbSrcYUV422P_U = (PBYTE8)hvImageSrc.rgImageData[2].phys;
	    	pbSrcYUV422P_V = (PBYTE8)hvImageSrc.rgImageData[2].phys + (hvImageSrc.iWidth * hvImageSrc.iHeight >> 1);
		}
		else
		{
			HvOpenCameraDma();
			BT1120_1600_1200_To_UYVY_1600_1200(
				(PBYTE8)hvImageSrc.rgImageData[0].phys,
				(PBYTE8)hvImageSrc.rgImageData[1].phys,
				g_bInBuffer
			);
			HvCloseCameraDma();

			if ( dwCompressRate < 1 || dwCompressRate > 100 )
	        {
	            dwCompressRate = 86;  // Ti的Jpeg编码库默认的压缩率即为86
	        }

	        dwJpegLen = hvImageDst.iWidth;

			if ( S_OK == XDM_JpegEncode_CbYCrY(
			            g_bInBuffer,
			            g_bOutBuffer,
			            &dwJpegLen,
			            hvImageDst.iHeight&0xffff,
			            (hvImageDst.iHeight>>16)&0xffff,
			            dwCompressRate) )
	        {
				if ( dwJpegLen < hvImageDst.iWidth )
				{
					memcpy((PBYTE8)hvImageDst.rgImageData[0].phys, g_bOutBuffer, dwJpegLen);
		            pJpegEncodeRespond->dwJpegLen = dwJpegLen;
		            return S_OK;
				}
	        }

            pJpegEncodeRespond->dwJpegLen = dwJpegLen;
            pJpegEncodeRespond->dwExtErrCode = g_iErrCode;
            return E_FAIL;
		}
    }
    else if ( HV_IMAGE_BT1120 == hvImageSrc.nImgType )
    {
		if ( 2448 == hvImageSrc.iWidth && 1024 == hvImageSrc.iHeight )
		{
			if ( 0 == dwJpegDataType )
			{
				PBYTE8 pbSrcYUV420P_Y = g_bInBuffer;
				PBYTE8 pbSrcYUV420P_U = g_bInBuffer + 2448*2048;
				PBYTE8 pbSrcYUV420P_V = g_bInBuffer + 2448*2048 + 1224*1024;

				// 在tskMessageDispatch函数中已经对整个L2进行了wbInv
				//CACHE_wbInvL2((PBYTE8)hvImageSrc.rgImageData[0].phys, 2448*1024, CACHE_WAIT);
				//CACHE_wbInvL2((PBYTE8)hvImageSrc.rgImageData[1].phys, 2448*1024, CACHE_WAIT);
				//CACHE_wbInvL2(g_bInBuffer, sizeof(g_bInBuffer), CACHE_WAIT);
				HvOpenCameraDma();
				BT1120_2448_1024_To_YUV420P_2448_2048(
					(PBYTE8)hvImageSrc.rgImageData[0].phys,
					(PBYTE8)hvImageSrc.rgImageData[1].phys,
					pbSrcYUV420P_Y,
					pbSrcYUV420P_U,
					pbSrcYUV420P_V
				);
				HvCloseCameraDma();

				if ( dwCompressRate < 1 || dwCompressRate > 100 )
		        {
		            dwCompressRate = 86;  // Ti的Jpeg编码库默认的压缩率即为86
		        }

		        dwJpegLen = hvImageDst.iWidth;

				DoStringOverlayYUV420P(
					(PBYTE8)pJpegEncodeData->szDateTimeStrings,
					pbSrcYUV420P_Y, (hvImageDst.iHeight&0xffff),
					pbSrcYUV420P_U, (hvImageDst.iHeight&0xffff)>>1,
					pbSrcYUV420P_V, (hvImageDst.iHeight&0xffff)>>1,
					hvImageDst.iHeight&0xffff, (hvImageDst.iHeight>>16)&0xffff
				);

		        if ( S_OK == XDM_JpegEncode_Yuv420P(
		                    pbSrcYUV420P_Y,
		                    pbSrcYUV420P_U,
		                    pbSrcYUV420P_V,
		                    g_bOutBuffer,
		                    &dwJpegLen,
		                    hvImageDst.iHeight&0xffff,
		                    (hvImageDst.iHeight>>16)&0xffff,
		                    dwCompressRate) )
		        {
					if ( dwJpegLen < hvImageDst.iWidth )
					{
						memcpy((PBYTE8)hvImageDst.rgImageData[0].phys, g_bOutBuffer, dwJpegLen);
			            pJpegEncodeRespond->dwJpegLen = dwJpegLen;
			            return S_OK;
					}
		        }

	            pJpegEncodeRespond->dwJpegLen = dwJpegLen;
	            pJpegEncodeRespond->dwExtErrCode = g_iErrCode;
	            return E_FAIL;
			}
			else
			{
				HvOpenCameraDma();
				BT1120Field_2448_1024_To_UYVY_2448_2048(
					(PBYTE8)hvImageSrc.rgImageData[0].phys,
					(PBYTE8)hvImageSrc.rgImageData[1].phys,
					g_bInBuffer
				);
				HvCloseCameraDma();

				if ( dwCompressRate < 1 || dwCompressRate > 100 )
		        {
		            dwCompressRate = 86;  // Ti的Jpeg编码库默认的压缩率即为86
		        }

		        dwJpegLen = hvImageDst.iWidth;

				if ( S_OK == XDM_JpegEncode_CbYCrY(
				            g_bInBuffer,
				            g_bOutBuffer,
				            &dwJpegLen,
				            hvImageDst.iHeight&0xffff,
				            (hvImageDst.iHeight>>16)&0xffff,
				            dwCompressRate) )
		        {
					if ( dwJpegLen < hvImageDst.iWidth )
					{
						memcpy((PBYTE8)hvImageDst.rgImageData[0].phys, g_bOutBuffer, dwJpegLen);
			            pJpegEncodeRespond->dwJpegLen = dwJpegLen;
			            return S_OK;
					}
		        }

	            pJpegEncodeRespond->dwJpegLen = dwJpegLen;
	            pJpegEncodeRespond->dwExtErrCode = g_iErrCode;
	            return E_FAIL;
			}
		}
		else if ( 1600 == hvImageSrc.iWidth && 1200 == hvImageSrc.iHeight )
		{
			if ( 0 == dwJpegDataType )
			{
				pbSrcYUV422P_Y = (PBYTE8)hvImageSrc.rgImageData[0].phys;
		    	pbSrcYUV422P_U = g_bInBuffer;
		    	pbSrcYUV422P_V = g_bInBuffer + 800*1200;

				HvOpenCameraDma();
				BT1120UV_1600_1200_SplitUV(
					(PBYTE8)hvImageSrc.rgImageData[1].phys,
					pbSrcYUV422P_U,
					pbSrcYUV422P_V
				);
				HvCloseCameraDma();
			}
			else
			{
				HvOpenCameraDma();
				BT1120_1600_1200_To_UYVY_1600_1200(
					(PBYTE8)hvImageSrc.rgImageData[0].phys,
					(PBYTE8)hvImageSrc.rgImageData[1].phys,
					g_bInBuffer
				);
				HvCloseCameraDma();

				if ( dwCompressRate < 1 || dwCompressRate > 100 )
		        {
		            dwCompressRate = 86;  // Ti的Jpeg编码库默认的压缩率即为86
		        }

		        dwJpegLen = hvImageDst.iWidth;

				if ( S_OK == XDM_JpegEncode_CbYCrY(
				            g_bInBuffer,
				            g_bOutBuffer,
				            &dwJpegLen,
				            hvImageDst.iHeight&0xffff,
				            (hvImageDst.iHeight>>16)&0xffff,
				            dwCompressRate) )
		        {
					if ( dwJpegLen < hvImageDst.iWidth )
					{
						memcpy((PBYTE8)hvImageDst.rgImageData[0].phys, g_bOutBuffer, dwJpegLen);
			            pJpegEncodeRespond->dwJpegLen = dwJpegLen;
			            return S_OK;
					}
		        }

	            pJpegEncodeRespond->dwJpegLen = dwJpegLen;
	            pJpegEncodeRespond->dwExtErrCode = g_iErrCode;
	            return E_FAIL;
			}
		}
		else if ( 1920 == hvImageSrc.iWidth && 1080 == hvImageSrc.iHeight )
		{
			pbSrcYUV422P_Y = (PBYTE8)hvImageSrc.rgImageData[0].phys;
	    	pbSrcYUV422P_U = g_bInBuffer;
	    	pbSrcYUV422P_V = g_bInBuffer + 960*1080;

			HvOpenCameraDma();
			BT1120UV_1920_1080_SplitUV(
				(PBYTE8)hvImageSrc.rgImageData[1].phys,
				pbSrcYUV422P_U,
				pbSrcYUV422P_V
			);
			HvCloseCameraDma();
		}
        else
        {
            return S_FALSE;
        }
    }
    else if ( HV_IMAGE_BT1120_ROTATE_Y == hvImageSrc.nImgType )
    {
        if ( 1200 == hvImageSrc.iWidth && 1600 == hvImageSrc.iHeight )
        {
			if( 0 == dwJpegDataType )
			{
				HvOpenCameraDma();
	            BT1120_ROTATE_Y_1200_1600_To_YUV422P_1200_1600(
	                (PBYTE8)hvImageSrc.rgImageData[0].phys,
	                (PBYTE8)hvImageSrc.rgImageData[1].phys,
	                g_bInBuffer,
	                g_bInBuffer + 1600*1200,
	                g_bInBuffer + 1600*1200 + 800*1200
	            );
				HvCloseCameraDma();

	            pbSrcYUV422P_Y = g_bInBuffer;
	            pbSrcYUV422P_U = g_bInBuffer + 1600*1200;
	            pbSrcYUV422P_V = g_bInBuffer + 1600*1200 + 800*1200;
			}
			else
			{
				HvOpenCameraDma();
				BT1120_ROTATE_Y_1200_1600_To_UYVY_1200_1600(
					(PBYTE8)hvImageSrc.rgImageData[0].phys,
					(PBYTE8)hvImageSrc.rgImageData[1].phys,
					g_bInBuffer
				);
				HvCloseCameraDma();

				if ( dwCompressRate < 1 || dwCompressRate > 100 )
		        {
		            dwCompressRate = 86;  // Ti的Jpeg编码库默认的压缩率即为86
		        }

		        dwJpegLen = hvImageDst.iWidth;

				if ( S_OK == XDM_JpegEncode_CbYCrY(
				            g_bInBuffer,
				            g_bOutBuffer,
				            &dwJpegLen,
				            hvImageDst.iHeight&0xffff,
				            (hvImageDst.iHeight>>16)&0xffff,
				            dwCompressRate) )
		        {
					if ( dwJpegLen < hvImageDst.iWidth )
					{
						memcpy((PBYTE8)hvImageDst.rgImageData[0].phys, g_bOutBuffer, dwJpegLen);
			            pJpegEncodeRespond->dwJpegLen = dwJpegLen;
			            return S_OK;
					}
		        }

	            pJpegEncodeRespond->dwJpegLen = dwJpegLen;
	            pJpegEncodeRespond->dwExtErrCode = g_iErrCode;
	            return E_FAIL;

			}
        }
        else
        {
            return S_FALSE;
        }
    }
    else
    {
        return E_NOTIMPL;
    }

    if ( dwCompressRate < 1 || dwCompressRate > 100 )
    {
        dwCompressRate = 86;  // Ti的Jpeg编码库默认的压缩率即为86
    }

    dwJpegLen = hvImageDst.iWidth;

	DoStringOverlayYUV422P(
		(PBYTE8)pJpegEncodeData->szDateTimeStrings,
		pbSrcYUV422P_Y, (hvImageDst.iHeight&0xffff),
		pbSrcYUV422P_U, (hvImageDst.iHeight&0xffff)>>1,
		pbSrcYUV422P_V, (hvImageDst.iHeight&0xffff)>>1,
		hvImageDst.iHeight&0xffff, (hvImageDst.iHeight>>16)&0xffff
	);

	// 编码1600*1200规格的图片，耗时约28ms
	if ( S_OK == XDM_JpegEncode_Yuv422P(
                pbSrcYUV422P_Y,
                pbSrcYUV422P_U,
                pbSrcYUV422P_V,
                g_bOutBuffer,
                &dwJpegLen,
                hvImageDst.iHeight&0xffff,
                (hvImageDst.iHeight>>16)&0xffff,
                dwCompressRate) )
    {
		if ( dwJpegLen < hvImageDst.iWidth )
		{
			memcpy((PBYTE8)hvImageDst.rgImageData[0].phys, g_bOutBuffer, dwJpegLen);
            pJpegEncodeRespond->dwJpegLen = dwJpegLen;
            return S_OK;
		}
    }

    pJpegEncodeRespond->dwJpegLen = dwJpegLen;
    pJpegEncodeRespond->dwExtErrCode = g_iErrCode;
    return E_FAIL;
}

static H264EncHandle g_hH264EncHandle;
static DWORD32 g_dwH264InputWidth;
static DWORD32 g_dwH264Width;
static DWORD32 g_dwH264Height;

HRESULT YUV2H264_TI(
    const H264_ENCODE_PARAM* pH264EncodeParam,
    const H264_ENCODE_DATA* pH264EncodeData,
    H264_ENCODE_RESPOND* pH264EncodeRespond
)
{
	static DWORD32 dwOutputBufSize = 0;

    DWORD32 dwOpType = pH264EncodeParam->dwOpType;

    HV_COMPONENT_IMAGE imgYuv = pH264EncodeData->hvImageYuv;
    HV_COMPONENT_IMAGE imgH264Frame = pH264EncodeData->hvImageFrame;

    DWORD32 dwFrameLen = 0;
    DWORD32 dwFrameType = 0;

    if ( OPTYPE_OPEN == dwOpType )
    {
		int iRet = 0;

		DWORD32 dwIntraFrameInterval = pH264EncodeParam->dwIntraFrameInterval;
	    DWORD32 dwTargetBitRate = pH264EncodeParam->dwTargetBitRate;
	    DWORD32 dwFrameRate = pH264EncodeParam->dwFrameRate;
	    DWORD32 dwInputWidth = pH264EncodeParam->dwInputWidth;
	    DWORD32 dwInputHeight = pH264EncodeParam->dwInputHeight;

		dwOutputBufSize = pH264EncodeParam->dwOutputBufSize;

		g_dwH264InputWidth = dwInputWidth;
		g_dwH264Width = dwInputWidth;
		g_dwH264Height = dwInputHeight;

        iRet |= H264Enc_Open(
					&g_hH264EncHandle,
	                dwIntraFrameInterval,
	                dwTargetBitRate,
	                dwFrameRate,
	                dwInputWidth,
	                dwInputHeight,
	                dwOutputBufSize
                );

		pH264EncodeRespond->dwExtErrCode = g_iErrCode;

        return ( 0 == iRet ) ? S_OK : E_FAIL;
    }

    if ( OPTYPE_CLOSE == dwOpType )
    {
		int iRet = 0;
		iRet |= H264Enc_Close(&g_hH264EncHandle);
        return ( 0 == iRet ) ? S_OK : E_FAIL;
    }

    if ( OPTYPE_ENCODE == dwOpType )
    {
        int iRet = 0;
        PBYTE8 pbSrcYUV422SP_Y = NULL;
        PBYTE8 pbSrcYUV422SP_UV = NULL;

        if ( HV_IMAGE_BT1120 != imgYuv.nImgType
                && HV_IMAGE_BT1120_UV != imgYuv.nImgType
                && HV_IMAGE_BT1120_ROTATE_Y != imgYuv.nImgType
                && HV_IMAGE_RAW12 == imgYuv.nImgType )
        {
            return S_FALSE;
        }

        if ( HV_IMAGE_H264 != imgH264Frame.nImgType )
        {
            return S_FALSE;
        }

        if ( HV_IMAGE_BT1120 == imgYuv.nImgType || HV_IMAGE_BT1120_UV == imgYuv.nImgType )
        {
			if ( 2448 == imgYuv.iWidth && 1024 == imgYuv.iHeight )
			{
				pbSrcYUV422SP_Y = g_bInBuffer;
                pbSrcYUV422SP_UV = g_bInBuffer + 1920*1088;

				// 在tskMessageDispatch函数中已经对整个L2进行了wbInv
				//CACHE_wbInvL2((PBYTE8)imgYuv.rgImageData[0].phys, 2448*1024, CACHE_WAIT);
				//CACHE_wbInvL2((PBYTE8)imgYuv.rgImageData[1].phys, 2448*1024, CACHE_WAIT);
				//CACHE_wbInvL2(g_bInBuffer, sizeof(g_bInBuffer), CACHE_WAIT);
				HvOpenCameraDma();
				BT1120_2448_1024_To_YUV422SP_1216_1024(
					(PBYTE8)imgYuv.rgImageData[0].phys,
					(PBYTE8)imgYuv.rgImageData[1].phys,
					pbSrcYUV422SP_Y,
					pbSrcYUV422SP_UV
				);
				HvCloseCameraDma();
			}
			else
			{
            	pbSrcYUV422SP_Y = (PBYTE8)imgYuv.rgImageData[0].phys;
            	pbSrcYUV422SP_UV = (PBYTE8)imgYuv.rgImageData[1].phys;
			}
        }
        else if ( HV_IMAGE_BT1120_ROTATE_Y == imgYuv.nImgType )
        {
            if ( 1200 == imgYuv.iWidth && 1600 == imgYuv.iHeight )
            {
                pbSrcYUV422SP_Y = g_bInBuffer;
                pbSrcYUV422SP_UV = g_bInBuffer + 1920*1088;

				HvOpenCameraDma();
                BT1120_ROTATE_Y_1200_1600_To_YUV422SP_1184_800(
                    (PBYTE8)imgYuv.rgImageData[0].phys,
                    (PBYTE8)imgYuv.rgImageData[1].phys,
                    pbSrcYUV422SP_Y,
                    pbSrcYUV422SP_UV
                );
		        HvCloseCameraDma();
            }
            else
            {
                return S_FALSE;
            }
        }
        else
        {
            return E_NOTIMPL;
        }

		DoStringOverlayYUV422SP(
			(PBYTE8)pH264EncodeData->szDateTimeStrings,
			pH264EncodeData->fDoubleStream,
			pbSrcYUV422SP_Y, g_dwH264InputWidth,
			pbSrcYUV422SP_UV, g_dwH264InputWidth,
			g_dwH264Width, g_dwH264Height
		);

        iRet |= H264Enc_Encode(
					&g_hH264EncHandle,
					pbSrcYUV422SP_Y,
					pbSrcYUV422SP_UV,
					g_bOutBuffer,
					&dwFrameLen,
					&dwFrameType
               );

        if ( 0 == iRet )
        {
			// Comment by Shaorg: 预留64字节是给I帧的帧头。
			if ( dwFrameLen+64 < dwOutputBufSize )
			{
				memcpy((PBYTE8)imgH264Frame.rgImageData[0].phys, g_bOutBuffer, dwFrameLen);
	            pH264EncodeRespond->dwFrameLen = dwFrameLen;
	            pH264EncodeRespond->dwFrameType = dwFrameType;
	            return S_OK;
			}
        }

		pH264EncodeRespond->dwFrameLen = dwFrameLen;
		pH264EncodeRespond->dwFrameType = dwFrameType;
		pH264EncodeRespond->dwExtErrCode = g_iErrCode;
        return E_FAIL;
    }

    return S_FALSE;
}

HRESULT CamDspProc(
    const CAM_DSP_PARAM* pCamDspParam,
    const CAM_DSP_DATA* pCamDspData,
    CAM_DSP_RESPOND* pCamDspRespond
)
{
	HRESULT hr = E_FAIL;

	if ( PROC_TYPE_JPEGENC == pCamDspParam->dwProcType ) // Jpeg编码
	{
		hr = YUV2JPG_TI(
                   &pCamDspParam->cJpegEncodeParam,
                   &pCamDspData->cJpegEncodeData,
                   &pCamDspRespond->cJpegEncodeRespond
               );
	}
	else if ( PROC_TYPE_H264ENC == pCamDspParam->dwProcType ) // H.264编码
    {
        hr = YUV2H264_TI(
                   &pCamDspParam->cH264EncodeParam,
                   &pCamDspData->cH264EncodeData,
                   &pCamDspRespond->cH264EncodeRespond
               );
    }
	else if ( PROC_TYPE_JPEGH264ENC == pCamDspParam->dwProcType ) // Jpeg和H.264编码，产生双码流
	{
		hr = S_OK;

		hr |= YUV2JPG_TI(
			&pCamDspParam->cJpegEncodeParam,
			&pCamDspData->cJpegEncodeData,
			&pCamDspRespond->cJpegEncodeRespond
		);

		hr |= YUV2H264_TI(
			&pCamDspParam->cH264EncodeParam,
			&pCamDspData->cH264EncodeData,
			&pCamDspRespond->cH264EncodeRespond
		);
	}
	else
	{
		hr = E_NOTIMPL;
		g_iErrCode = hr;
		pCamDspRespond->dwExtErrCode = g_iErrCode;
	}

	return hr;
}
