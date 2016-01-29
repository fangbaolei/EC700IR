/******************************************************************************/
/*            Copyright (c) 2006 Texas Instruments, Incorporated.             */
/*                           All Rights Reserved.                             */
/******************************************************************************/

/*!
********************************************************************************
@file     TestAppDecoder.c
@brief    This is the top level client file that drives the JPEG
(Baseline Profile) Image Decoder Call using XDM Interface
@author   Multimedia Codecs TI India
@version  0.1 - Jan 24,2006    initial version
********************************************************************************
*/

/* Standard C header files */
#include <stdio.h>
#include <stdlib.h>

/* JPEG Interface header files */
#include "ijpegdech.h"
#include "jpegdec_ti.h"
#include "jpegdec.h"
#include "idmjpge.h"

#include "std.h"
#include "csl.h"
#include "mem.h"
#include "csl_chip.h"
#include "csl_cache.h"
#include "csl_dat.h"

int g_iErrCode = 0;

/* Client header file */
#include "TiImageProcesser.h"

HRESULT XDM_JpegDecode(
	PBYTE8 				pbJpegData,
	DWORD32 			dwJpegLen,
	PBYTE8				pbYUVData,
	PDWORD32			pdwYUVLen,
	PDWORD32			pdwWidth,
	PDWORD32			pdwHeight,
	PDWORD32			pdwStride
	)
{
	JPEGDEC_Params g_JpegDecParams = {0};
	JPEGDEC_DynamicParams g_JpegDecDynamicParams = {0};
	JPEGDEC_Status g_JpegDecStatus = {0};
	JPEGDEC_InArgs g_JpegDecInArgs = {0};
	JPEGDEC_OutArgs g_JpegDecOutArgs = {0};
	PSBYTE8 *g_pInputBuf[XDM_MAX_IO_BUFFERS] = {NULL};
	PSBYTE8 *g_pOutputBuf[XDM_MAX_IO_BUFFERS] = {NULL};
	SDWORD32 g_inBufSize[XDM_MAX_IO_BUFFERS] = {NULL};
	SDWORD32 g_outBufSize[XDM_MAX_IO_BUFFERS] = {NULL};
	XDM_BufDesc g_JpegDecInputBuffDesc = {NULL};
	XDM_BufDesc g_JpegDecOutputBuffDesc = {NULL};
	IALG_Handle g_JpegDecHandle = NULL;
	IIMGDEC_Fxns *IIMGDECFxns;
	int i, retVal;

	if (pbJpegData == NULL ||
		dwJpegLen == 0 ||
		pbYUVData == NULL ||
		pdwYUVLen == NULL ||
		pdwWidth == NULL ||
		pdwHeight == NULL ||
		pdwStride == NULL)
	{
		g_iErrCode = 0xffff0001;
		return E_POINTER;
	}

	g_JpegDecStatus.imgdecStatus.size = sizeof(IJPEGDEC_Status);
	g_JpegDecInArgs.imgdecInArgs.size = sizeof(IJPEGDEC_InArgs);
	g_JpegDecOutArgs.imgdecOutArgs.size = sizeof(IJPEGDEC_OutArgs);

	g_JpegDecParams.imgdecParams.size = sizeof(IJPEGDEC_Params);
	g_JpegDecParams.imgdecParams.maxWidth = JPEGDEC_IMAGE_MAX_WIDTH;
	g_JpegDecParams.imgdecParams.maxHeight = JPEGDEC_IMAGE_MAX_HEIGHT;
	g_JpegDecParams.imgdecParams.maxScans = 15;
	g_JpegDecParams.imgdecParams.dataEndianness = XDM_BYTE;
	g_JpegDecParams.imgdecParams.forceChromaFormat = XDM_YUV_422ILE;
	g_JpegDecParams.progressiveDecFlag = 0;
	
	if ((g_JpegDecHandle = ALG_create(
		(IALG_Fxns *) &JPEGDEC_TI_IJPEGDEC,
		(IALG_Handle) NULL, 
		(IALG_Params *) &g_JpegDecParams)) == NULL)
	{
		g_iErrCode = 0xffff0002;
		return E_FAIL;
	}
	g_JpegDecHandle->fxns->algActivate(g_JpegDecHandle);
	IIMGDECFxns = (IIMGDEC_Fxns*)g_JpegDecHandle->fxns;

	IIMGDECFxns->control(
		(IIMGDEC_Handle)g_JpegDecHandle,
		XDM_GETBUFINFO,
		(IIMGDEC_DynamicParams *)&g_JpegDecDynamicParams,
		(IIMGDEC_Status *)&g_JpegDecStatus );

	g_JpegDecInputBuffDesc.bufs = (XDAS_Int8**)g_pInputBuf;
	g_JpegDecInputBuffDesc.bufSizes = g_inBufSize;
	g_JpegDecInputBuffDesc.bufs[0] = (PSBYTE8)pbJpegData;	
	g_JpegDecInputBuffDesc.numBufs = g_JpegDecStatus.imgdecStatus.bufInfo.minNumInBufs;
	g_JpegDecInputBuffDesc.bufSizes[0] = g_JpegDecStatus.imgdecStatus.bufInfo.minInBufSize[0];
	for (i = 0; i < (g_JpegDecStatus.imgdecStatus.bufInfo.minNumInBufs - 1); i++)
	{
		g_JpegDecInputBuffDesc.bufs[i + 1] = g_JpegDecInputBuffDesc.bufs[i] + g_JpegDecStatus.imgdecStatus.bufInfo.minInBufSize[i];
		g_JpegDecInputBuffDesc.bufSizes[i + 1] = g_JpegDecStatus.imgdecStatus.bufInfo.minInBufSize[i + 1];
	}
	g_JpegDecInArgs.imgdecInArgs.numBytes = dwJpegLen;

	g_JpegDecOutputBuffDesc.bufs = (XDAS_Int8**)g_pOutputBuf;
	g_JpegDecOutputBuffDesc.bufSizes = g_outBufSize;
	g_JpegDecOutputBuffDesc.bufs[0] = (PSBYTE8)pbYUVData;
	g_JpegDecOutputBuffDesc.numBufs = g_JpegDecStatus.imgdecStatus.bufInfo.minNumOutBufs;
	g_JpegDecOutputBuffDesc.bufSizes[0] = g_JpegDecStatus.imgdecStatus.bufInfo.minOutBufSize[0];
	for (i = 0; i < (g_JpegDecStatus.imgdecStatus.bufInfo.minNumOutBufs - 1); i++)
	{
		g_JpegDecOutputBuffDesc.bufs[i + 1] = g_JpegDecOutputBuffDesc.bufs[i] + g_JpegDecStatus.imgdecStatus.bufInfo.minOutBufSize[i];
		g_JpegDecOutputBuffDesc.bufSizes[i + 1] = g_JpegDecStatus.imgdecStatus.bufInfo.minOutBufSize[i + 1];
	}

	g_JpegDecDynamicParams.imgdecDynamicParams.size = sizeof( IJPEGDEC_DynamicParams );
	g_JpegDecDynamicParams.imgdecDynamicParams.decodeHeader = XDM_DECODE_AU;
	g_JpegDecDynamicParams.imgdecDynamicParams.numAU = XDM_DEFAULT;
	g_JpegDecDynamicParams.imgdecDynamicParams.displayWidth = XDM_DEFAULT;
	g_JpegDecDynamicParams.progDisplay = 0;
	g_JpegDecDynamicParams.resizeOption = 0;

	IIMGDECFxns->control(
		(IIMGDEC_Handle)g_JpegDecHandle,
		XDM_SETPARAMS,
		(IIMGDEC_DynamicParams *)&g_JpegDecDynamicParams,
		(IIMGDEC_Status *)&g_JpegDecStatus );

	while (g_JpegDecOutArgs.end_of_seq != 1)
	{
		retVal = IIMGDECFxns->process(
			(IIMGDEC_Handle)g_JpegDecHandle,
			(XDM_BufDesc *)&g_JpegDecInputBuffDesc,
			(XDM_BufDesc *)&g_JpegDecOutputBuffDesc,
			(IIMGDEC_InArgs *)&g_JpegDecInArgs,
			(IIMGDEC_OutArgs *)&g_JpegDecOutArgs );
		if (retVal != XDM_EOK ||
			g_JpegDecOutArgs.imgdecOutArgs.extendedError != JPEGDEC_SUCCESS)
		{
			g_JpegDecHandle->fxns->algDeactivate(g_JpegDecHandle);
			ALG_delete(g_JpegDecHandle);
			g_iErrCode = g_JpegDecOutArgs.imgdecOutArgs.extendedError;
			return E_FAIL;
		}
	}
	IIMGDECFxns->control(
		(IIMGDEC_Handle)g_JpegDecHandle, 
		XDM_GETSTATUS,
		(IIMGDEC_DynamicParams *)&g_JpegDecDynamicParams,
		(IIMGDEC_Status *)&g_JpegDecStatus);
	
	g_JpegDecHandle->fxns->algDeactivate( g_JpegDecHandle );
	ALG_delete(g_JpegDecHandle);

	if ( g_JpegDecOutArgs.imgdecOutArgs.extendedError == JPEGDEC_SUCCESS )
	{
		*pdwWidth = g_JpegDecStatus.imgdecStatus.outputWidth;
		*pdwStride = (g_JpegDecStatus.imgdecStatus.outputWidth >> 1);
		*pdwHeight = g_JpegDecStatus.imgdecStatus.outputHeight;
		*pdwYUVLen = g_JpegDecOutputBuffDesc.bufSizes[0];
		return S_OK;
	}
	else
	{
		g_iErrCode = 0xffff0004;
		return E_FAIL;
	}
}
