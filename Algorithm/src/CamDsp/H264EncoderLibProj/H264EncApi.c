#include "H264EncApi.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tistdtypes.h>
#include <std.h>
#include "csl_cache.h"

extern int g_iErrCode;

extern int H264FHDVENC_RMAN_setup();
extern int H264FHDVENC_RMAN_cleanup();

int H264FHDVENC_TII_scratchId = -1;

int H264Enc_Open(
	H264EncHandle* pHandle,
	DWORD32 dwIntraFrameInterval,
	DWORD32 dwTargetBitRate,
	DWORD32 dwFrameRate,
	DWORD32 dwInputWidth,
	DWORD32 dwInputHeight,
	DWORD32 dwOutputBufSize
)
{
	// 注: 输入的宽高必须满足以下条件：
	// 1、必须是32的整数倍。
	// 2、宽度范围：352 ~ 1920，高度范围：288 ~ 1088。
	if ( dwInputWidth%32 != 0
		|| dwInputWidth < 352
		|| dwInputWidth > 1920
		|| dwInputHeight%32 != 0
		|| dwInputHeight < 288
		|| dwInputHeight > 1088 )
	{
		g_iErrCode = 0xffff0001;
		return -1;
	}

	InitH264OnChipRAM();

	if ( -1 == H264FHDVENC_RMAN_setup() )
	{
		g_iErrCode = 0xffff0002;
		return -1;
	}

	memset(pHandle, 0, sizeof(H264EncHandle));

	pHandle->fxns = H264FHDVENC_TII_IH264FHDVENC;

	pHandle->status.videncStatus.size                  = sizeof(IH264FHDVENC_Status);
	pHandle->inargs.videncInArgs.size                  = sizeof(IH264FHDVENC_InArgs);
	pHandle->outargs.videncOutArgs.size                = sizeof(IH264FHDVENC_OutArgs);

	// ------------- 参数填充 BEGIN -------------

	// 视频编码器参数
	pHandle->params.videncParams.size                  = sizeof(IH264FHDVENC_Params);
	pHandle->params.videncParams.encodingPreset        = XDM_DEFAULT;
	pHandle->params.videncParams.rateControlPreset     = IVIDEO_STORAGE;  // 码率控制预设：IVIDEO_LOW_DELAY、IVIDEO_STORAGE、IVIDEO_NONE
	pHandle->params.videncParams.maxHeight             = 1088;
	pHandle->params.videncParams.maxWidth              = 1920;
	pHandle->params.videncParams.maxFrameRate          = 30000;
	pHandle->params.videncParams.maxBitRate            = 10000000;
	pHandle->params.videncParams.dataEndianness        = XDM_LE_32;
	pHandle->params.videncParams.maxInterFrameInterval = 0;
	pHandle->params.videncParams.inputChromaFormat     = H264FHDVENC_TI_422SP;  // 输入格式：XDM_YUV_420SP、H264FHDVENC_TI_422SP
	pHandle->params.videncParams.inputContentType      = IVIDEO_PROGRESSIVE;
	pHandle->params.videncParams.reconChromaFormat     = XDM_CHROMA_NA;

	// 视频编码器扩展参数
	pHandle->params.profileIdc                         = 66;
	pHandle->params.levelIdc                           = 40;
	pHandle->params.EntropyCodingMode                  = 0;

	// 视频编码器动态参数
	pHandle->dynamicparams.videncDynamicParams.size                 = sizeof(IH264FHDVENC_DynamicParams);
	pHandle->dynamicparams.videncDynamicParams.inputHeight          = dwInputHeight;
	pHandle->dynamicparams.videncDynamicParams.inputWidth           = dwInputWidth;
	pHandle->dynamicparams.videncDynamicParams.refFrameRate         = dwFrameRate*1000;
	pHandle->dynamicparams.videncDynamicParams.targetFrameRate      = dwFrameRate*1000;
	pHandle->dynamicparams.videncDynamicParams.targetBitRate        = dwTargetBitRate;
	pHandle->dynamicparams.videncDynamicParams.intraFrameInterval   = dwIntraFrameInterval; // I帧间隔
	pHandle->dynamicparams.videncDynamicParams.generateHeader       = XDM_ENCODE_AU;
	pHandle->dynamicparams.videncDynamicParams.captureWidth         = 0;
	pHandle->dynamicparams.videncDynamicParams.forceFrame           = IVIDEO_NA_FRAME;
	pHandle->dynamicparams.videncDynamicParams.interFrameInterval   = 0;
	pHandle->dynamicparams.videncDynamicParams.mbDataFlag           = 0;

	// 视频编码器扩展动态参数
	pHandle->dynamicparams.sliceCodingParams.sliceCodingPreset = IH264_SLICECODING_DEFAULT;
	pHandle->dynamicparams.sliceCodingParams.sliceMode         = IH264_SLICEMODE_BYTES;
	pHandle->dynamicparams.sliceCodingParams.sliceUnitSize     = 1500;
	pHandle->dynamicparams.sliceCodingParams.streamFormat      = IH264_BYTE_STREAM;
	pHandle->dynamicparams.OutBufSize                          = -1;
	pHandle->dynamicparams.QPISlice                            = 28;
	pHandle->dynamicparams.QPSlice                             = 28;
	pHandle->dynamicparams.RateCtrlQpMax                       = 51;
	pHandle->dynamicparams.RateCtrlQpMin                       = 0;
	pHandle->dynamicparams.NumRowsInSlice                      = 0;
	pHandle->dynamicparams.LfDisableIdc                        = 0;
	pHandle->dynamicparams.LFAlphaC0Offset                     = 0;
	pHandle->dynamicparams.LFBetaOffset                        = 0;
	pHandle->dynamicparams.ChromaQPOffset                      = 0;
	pHandle->dynamicparams.SecChromaQPOffset                   = 0;
	pHandle->dynamicparams.PicAFFFlag                          = 0;
	pHandle->dynamicparams.PicOrderCountType                   = 0;
	pHandle->dynamicparams.AdaptiveMBs                         = 0;
	pHandle->dynamicparams.SEIParametersFlag                   = 0;
	pHandle->dynamicparams.VUIParametersFlag                   = 0;
	pHandle->dynamicparams.NALUnitCallback                     = NULL;
	pHandle->dynamicparams.SkipStartCodesInCallback            = 1;
	pHandle->dynamicparams.Intra4x4EnableFlag                  = 0;
	pHandle->dynamicparams.MESelect                            = 1;
	pHandle->dynamicparams.MVDataFlag                          = 0;
	pHandle->dynamicparams.Transform8x8DisableFlag             = 1;
	pHandle->dynamicparams.Intra8x8EnableFlag                  = 0;
	pHandle->dynamicparams.InterlaceReferenceMode              = 0;
	pHandle->dynamicparams.ChromaConversionMode                = 0; // 422转420色彩空间模式选择：0（Line drop）、1（Average）
	pHandle->dynamicparams.maxDelay                            = 1000;
	pHandle->dynamicparams.MaxSlicesSupported_IFrame           = 90;
	pHandle->dynamicparams.MaxSlicesSupported_PFrame           = 90;

	// ------------- 参数填充 END -------------

	if ((pHandle->handle = H264FHDVENC_create(&pHandle->fxns, &pHandle->params)) == NULL)
	{
		H264FHDVENC_RMAN_cleanup();
		g_iErrCode = 0xffff0003;
		return -1;
	}

	if ( H264FHDVENC_control(
		pHandle->handle,        /* Instance Handle					  */
		XDM_SETPARAMS,          /* Command							  */
		&pHandle->dynamicparams,/* Pointer to Dynamic structure-Input */
		&pHandle->status        /* Pointer to status structure-Output */
		) < 0 )
	{
		H264FHDVENC_delete(pHandle->handle);
		H264FHDVENC_RMAN_cleanup();
		g_iErrCode = 0xffff0004;
		return -1;
	}

	if ( H264FHDVENC_control(
		pHandle->handle,         /* Instance Handle                        */
		XDM_GETBUFINFO,          /* Command								   */
		&pHandle->dynamicparams, /* Pointer to Dynamic Params struct-Input */
		&pHandle->status         /* Pointer to the status struce - Output  */
		) < 0 )
	{
		H264FHDVENC_delete(pHandle->handle);
		H264FHDVENC_RMAN_cleanup();
		g_iErrCode = 0xffff0005;
		return -1;
	}

	// In缓冲区填充
	pHandle->inobj.frameWidth = dwInputWidth;
	pHandle->inobj.frameHeight = dwInputHeight;
	pHandle->inobj.framePitch = dwInputWidth;
	pHandle->inobj.numBufs = pHandle->status.videncStatus.bufInfo.minNumInBufs;
	pHandle->inobj.bufDesc[0].bufSize = pHandle->status.videncStatus.bufInfo.minInBufSize[0];
	pHandle->inobj.bufDesc[1].bufSize = pHandle->status.videncStatus.bufInfo.minInBufSize[1];

	// Out缓冲区填充
	pHandle->outobj.numBufs = pHandle->status.videncStatus.bufInfo.minNumOutBufs;
	pHandle->outobj.bufs = (XDAS_Int8**)pHandle->rgBufs;
	pHandle->outobj.bufSizes = pHandle->rgBufSizes;
	pHandle->outobj.bufSizes[0] = dwOutputBufSize;

	return 0;
}

int H264Enc_Encode(
	H264EncHandle* pHandle,
	const PBYTE8 pbSrcYUV422SP_Y,
	const PBYTE8 pbSrcYUV422SP_UV,
	PBYTE8 pbH264BitStream,
	PDWORD32 pdwFrameLen,
	PDWORD32 pdwFrameType
)
{
	XDAS_Int32 iErrorFlag = 0;             /* Variable to Keep track of errors */

	if ( NULL == pHandle || NULL == pHandle->handle )
	{
		return -1;
	}

	pHandle->inobj.bufDesc[0].buf = (XDAS_Int8 *)pbSrcYUV422SP_Y;
	pHandle->inobj.bufDesc[1].buf = (XDAS_Int8 *)pbSrcYUV422SP_UV;
	pHandle->outobj.bufs[0] = (XDAS_Int8 *)pbH264BitStream;
	*pdwFrameLen = 0;
	*pdwFrameType = 0;

	if (IRES_OK != RMAN_assignResources((IALG_Handle)(pHandle->handle), &H264FHDVENC_TII_IRES, H264FHDVENC_TII_scratchId))
	{
		return -1;
	}

	CACHE_wbInvAllL2(CACHE_WAIT);
	iErrorFlag = H264FHDVENC_encode(
		pHandle->handle,   /* Instance Handle   - Input	 */
		&pHandle->inobj,   /* Input Buffers     - Input  */
		&pHandle->outobj,  /* Output Buffers    - Output */
		&pHandle->inargs,  /* Input Parameters  - Input  */
		&pHandle->outargs  /* Output Parameters - Output */
		);

	if (IRES_OK != RMAN_freeResources((IALG_Handle)(pHandle->handle), &H264FHDVENC_TII_IRES, H264FHDVENC_TII_scratchId))
	{
		return -1;
	}

	if (iErrorFlag == XDM_EOK)
	{
		if (pHandle->outargs.videncOutArgs.inputFrameSkip == IVIDEO_FRAME_SKIPPED)
		{
			pHandle->outargs.videncOutArgs.inputFrameSkip = IVIDEO_FRAME_ENCODED;

			*pdwFrameLen = pHandle->outargs.videncOutArgs.bytesGenerated;
			*pdwFrameType = 2; /* 2 == FRAME_TYPE_H264_SKIP */
			return 0;
		}
		else if (pHandle->outargs.videncOutArgs.bytesGenerated > 0)
		{
			*pdwFrameLen = pHandle->outargs.videncOutArgs.bytesGenerated;
			*pdwFrameType = pHandle->outargs.videncOutArgs.encodedFrameType;
			return 0;
		}
		else
		{
			return -1;
		}
	}
	else if (iErrorFlag == XDM_EFAIL)
	{
		H264FHDVENC_control(
			pHandle->handle,
			XDM_GETSTATUS,
			&pHandle->dynamicparams,
			&pHandle->status
			);

		g_iErrCode = pHandle->status.videncStatus.extendedError;
	}

	return -1;
}

int H264Enc_Close(H264EncHandle* pHandle)
{
	if ( pHandle != NULL && pHandle->handle != NULL )
	{
		H264FHDVENC_delete(pHandle->handle);
		H264FHDVENC_RMAN_cleanup();
		memset(pHandle, 0, sizeof(H264EncHandle));
	}

	return 0;
}
