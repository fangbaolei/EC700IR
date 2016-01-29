#include "JpegEncApi.h"
#include "swwinerror.h"
#include "idmjpge.h"

#include <ialg.h>
typedef IALG_Handle ALG_Handle;

extern int g_iErrCode;

extern ALG_Handle ALG_create_JpegEnc(IALG_Fxns *fxns, IALG_Handle p, IALG_Params *params);
extern Void ALG_delete_JpegEnc(ALG_Handle alg);

HRESULT XDM_JpegEncode_CbYCrY(
	const PBYTE8 	pbSrcCbYCrY,
    PBYTE8 			pbJpegData,
    PDWORD32 		pdwJpegLen,
    DWORD32			dwWidth,
    DWORD32			dwHeight,
    DWORD32			dwQValue
)
{
    IIMGENC1_Status			status = {0};
    IIMGENC1_InArgs			inArgs = {0};
    IIMGENC1_OutArgs		outArgs= {0};
    IIMGENC1_Params			params = {0};
    IIMGENC1_DynamicParams	dynamicParams = {0};

    XDM1_BufDesc inputBufDesc = {NULL};
    XDM1_BufDesc outputBufDesc = {NULL};

    IALG_Handle handle = NULL;
    IIMGENC1_Fxns *IIMGENC1fxns = NULL;

    XDAS_Int32 retVal = XDM_EFAIL;

    if ( pbSrcCbYCrY == NULL
            || pbJpegData == NULL
            || pdwJpegLen == NULL )
    {
		g_iErrCode = 0x00010001;
        return E_POINTER;
    }

    status.size                     = sizeof(IIMGENC1_Status);

    inArgs.size                     = sizeof(IIMGENC1_InArgs);

    outArgs.size                    = sizeof(IIMGENC1_OutArgs);
    outArgs.extendedError           = 0;
    outArgs.bytesGenerated          = 0;
    outArgs.currentAU               = 0;

    params.size						= sizeof(IIMGENC1_Params);
    params.maxWidth					= dwWidth;
    params.maxHeight				= dwHeight;
    params.maxScans					= XDM_DEFAULT;
    params.dataEndianness			= XDM_BYTE;
    params.forceChromaFormat		= XDM_YUV_422P;

    dynamicParams.size				= sizeof(IIMGENC1_DynamicParams);
    dynamicParams.inputChromaFormat	= XDM_YUV_422ILE;
    dynamicParams.numAU				= XDM_DEFAULT;
    dynamicParams.generateHeader	= XDM_ENCODE_AU;
    dynamicParams.qValue			= dwQValue;
    dynamicParams.inputWidth		= dwWidth;
    dynamicParams.inputHeight		= dwHeight;
    dynamicParams.captureWidth		= XDM_DEFAULT;

    if ((handle = (IALG_Handle)ALG_create_JpegEnc(
                      (IALG_Fxns *) &DMJPGE_TIGEM_IDMJPGE,
                      (IALG_Handle) NULL,
                      (IALG_Params *) &params)) == NULL)
    {
		g_iErrCode = 0x00010002;
        return E_FAIL;
    }

    IIMGENC1fxns = (IIMGENC1_Fxns *)handle->fxns;

	handle->fxns->algActivate(handle);

	IIMGENC1fxns->control(
        (IIMGENC1_Handle)handle,
        XDM_SETPARAMS,
        (IIMGENC1_DynamicParams *)&dynamicParams,
        (IIMGENC1_Status *)&status
    );

    IIMGENC1fxns->control(
        (IIMGENC1_Handle)handle,
        XDM_GETBUFINFO,
        (IIMGENC1_DynamicParams *)&dynamicParams,
        (IIMGENC1_Status *)&status
    );

    inputBufDesc.numBufs = status.bufInfo.minNumInBufs;
    if ( inputBufDesc.numBufs != 1 )
    {
		g_iErrCode = 0x00010003;
		handle->fxns->algDeactivate(handle);
		ALG_delete_JpegEnc(handle);
    	return E_FAIL;
	}
    inputBufDesc.descs[0].buf     = (XDAS_Int8 *)pbSrcCbYCrY;
    inputBufDesc.descs[0].bufSize = status.bufInfo.minInBufSize[0];

    outputBufDesc.numBufs = status.bufInfo.minNumOutBufs;
    if ( outputBufDesc.numBufs != 1 )
    {
		g_iErrCode = 0x00010004;
		handle->fxns->algDeactivate(handle);
		ALG_delete_JpegEnc(handle);
    	return E_FAIL;
	}
	outputBufDesc.descs[0].buf     = (XDAS_Int8 *)pbJpegData;
    outputBufDesc.descs[0].bufSize = *pdwJpegLen;

    retVal = IIMGENC1fxns->process(
                 (IIMGENC1_Handle)handle,
                 (XDM1_BufDesc *)&inputBufDesc,
                 (XDM1_BufDesc *)&outputBufDesc,
                 (IIMGENC1_InArgs *)&inArgs,
                 (IIMGENC1_OutArgs *)&outArgs
             );

    if (retVal == XDM_EFAIL)
    {
	    IIMGENC1fxns->control(
	        (IIMGENC1_Handle)handle,
	        XDM_GETSTATUS,
	        (IIMGENC1_DynamicParams *)&dynamicParams,
	        (IIMGENC1_Status *)&status
	    );
		g_iErrCode = status.extendedError;
		if ( JPEGENC_OUTPUT_BUFF_SIZE == g_iErrCode )
		{
			*pdwJpegLen = status.bufInfo.minOutBufSize[0];  // Comment by Shaorg: 这个最小输出缓冲区尺寸大小并不准确。
		}
		else
		{
			*pdwJpegLen = 0;
		}

		handle->fxns->algDeactivate(handle);
	    ALG_delete_JpegEnc(handle);
        return E_FAIL;
    }

    IIMGENC1fxns->control(
        (IIMGENC1_Handle)handle,
        XDM_GETSTATUS,
        (IIMGENC1_DynamicParams *)&dynamicParams,
        (IIMGENC1_Status *)&status
    );

	handle->fxns->algDeactivate(handle);
    ALG_delete_JpegEnc(handle);

    if ( status.extendedError == JPEGENC_SUCCESS )
    {
        *pdwJpegLen = outArgs.bytesGenerated;

		g_iErrCode = 0;
        return S_OK;
    }
	else
	{
		g_iErrCode = 0x00010005;
    	return E_FAIL;
	}
}

HRESULT XDM_JpegEncode_Yuv422P(
	const PBYTE8 	pbSrcYUV422P_Y,
	const PBYTE8	pbSrcYUV422P_U,
	const PBYTE8	pbSrcYUV422P_V,
    PBYTE8 			pbJpegData,
    PDWORD32 		pdwJpegLen,
    DWORD32			dwWidth,
    DWORD32			dwHeight,
    DWORD32			dwQValue
)
{
    IIMGENC1_Status			status = {0};
    IIMGENC1_InArgs			inArgs = {0};
    IIMGENC1_OutArgs		outArgs= {0};
    IIMGENC1_Params			params = {0};
    IIMGENC1_DynamicParams	dynamicParams = {0};

    XDM1_BufDesc inputBufDesc = {NULL};
    XDM1_BufDesc outputBufDesc = {NULL};

    IALG_Handle handle = NULL;
    IIMGENC1_Fxns *IIMGENC1fxns = NULL;

    XDAS_Int32 retVal = XDM_EFAIL;

    if ( pbSrcYUV422P_Y == NULL
			|| pbSrcYUV422P_U == NULL
			|| pbSrcYUV422P_V == NULL
            || pbJpegData == NULL
            || pdwJpegLen == NULL )
    {
		g_iErrCode = 0x00020001;
        return E_POINTER;
    }

    status.size                     = sizeof(IIMGENC1_Status);

    inArgs.size                     = sizeof(IIMGENC1_InArgs);

    outArgs.size                    = sizeof(IIMGENC1_OutArgs);
    outArgs.extendedError           = 0;
    outArgs.bytesGenerated          = 0;
    outArgs.currentAU               = 0;

    params.size						= sizeof(IIMGENC1_Params);
    params.maxWidth					= dwWidth;
    params.maxHeight				= dwHeight;
    params.maxScans					= XDM_DEFAULT;
    params.dataEndianness			= XDM_BYTE;
    params.forceChromaFormat		= XDM_YUV_422P;

    dynamicParams.size				= sizeof(IIMGENC1_DynamicParams);
    dynamicParams.inputChromaFormat	= XDM_YUV_422P;
    dynamicParams.numAU				= XDM_DEFAULT;
    dynamicParams.generateHeader	= XDM_ENCODE_AU;
    dynamicParams.qValue			= dwQValue;
    dynamicParams.inputWidth		= dwWidth;
    dynamicParams.inputHeight		= dwHeight;
    dynamicParams.captureWidth		= XDM_DEFAULT;

    if ((handle = (IALG_Handle)ALG_create_JpegEnc(
                      (IALG_Fxns *) &DMJPGE_TIGEM_IDMJPGE,
                      (IALG_Handle) NULL,
                      (IALG_Params *) &params)) == NULL)
    {
		g_iErrCode = 0x00020002;
        return E_FAIL;
    }

    IIMGENC1fxns = (IIMGENC1_Fxns *)handle->fxns;

	handle->fxns->algActivate(handle);

	IIMGENC1fxns->control(
        (IIMGENC1_Handle)handle,
        XDM_SETPARAMS,
        (IIMGENC1_DynamicParams *)&dynamicParams,
        (IIMGENC1_Status *)&status
    );

    IIMGENC1fxns->control(
        (IIMGENC1_Handle)handle,
        XDM_GETBUFINFO,
        (IIMGENC1_DynamicParams *)&dynamicParams,
        (IIMGENC1_Status *)&status
    );

    inputBufDesc.numBufs = status.bufInfo.minNumInBufs;
    if ( inputBufDesc.numBufs != 3 )
    {
		g_iErrCode = 0x00020003;
		handle->fxns->algDeactivate(handle);
		ALG_delete_JpegEnc(handle);
    	return E_FAIL;
	}
    inputBufDesc.descs[0].buf     = (XDAS_Int8 *)pbSrcYUV422P_Y;
    inputBufDesc.descs[0].bufSize = status.bufInfo.minInBufSize[0];
	inputBufDesc.descs[1].buf     = (XDAS_Int8 *)pbSrcYUV422P_U;
    inputBufDesc.descs[1].bufSize = status.bufInfo.minInBufSize[1];
	inputBufDesc.descs[2].buf     = (XDAS_Int8 *)pbSrcYUV422P_V;
    inputBufDesc.descs[2].bufSize = status.bufInfo.minInBufSize[2];

    outputBufDesc.numBufs = status.bufInfo.minNumOutBufs;
    if ( outputBufDesc.numBufs != 1 )
    {
		g_iErrCode = 0x00020004;
		handle->fxns->algDeactivate(handle);
		ALG_delete_JpegEnc(handle);
    	return E_FAIL;
	}
    outputBufDesc.descs[0].buf     = (XDAS_Int8 *)pbJpegData;
    outputBufDesc.descs[0].bufSize = *pdwJpegLen;

    retVal = IIMGENC1fxns->process(
                 (IIMGENC1_Handle)handle,
                 (XDM1_BufDesc *)&inputBufDesc,
                 (XDM1_BufDesc *)&outputBufDesc,
                 (IIMGENC1_InArgs *)&inArgs,
                 (IIMGENC1_OutArgs *)&outArgs
             );

    if (retVal == XDM_EFAIL)
    {
	    IIMGENC1fxns->control(
	        (IIMGENC1_Handle)handle,
	        XDM_GETSTATUS,
	        (IIMGENC1_DynamicParams *)&dynamicParams,
	        (IIMGENC1_Status *)&status
	    );
		g_iErrCode = status.extendedError;
		if ( JPEGENC_OUTPUT_BUFF_SIZE == g_iErrCode )
		{
			*pdwJpegLen = status.bufInfo.minOutBufSize[0];  // Comment by Shaorg: 这个最小输出缓冲区尺寸大小并不准确。
		}
		else
		{
			*pdwJpegLen = 0;
		}

		handle->fxns->algDeactivate(handle);
	    ALG_delete_JpegEnc(handle);
        return E_FAIL;
    }

    IIMGENC1fxns->control(
        (IIMGENC1_Handle)handle,
        XDM_GETSTATUS,
        (IIMGENC1_DynamicParams *)&dynamicParams,
        (IIMGENC1_Status *)&status
    );

	handle->fxns->algDeactivate(handle);
    ALG_delete_JpegEnc(handle);

    if ( status.extendedError == JPEGENC_SUCCESS )
    {
        *pdwJpegLen = outArgs.bytesGenerated;

		g_iErrCode = 0;
        return S_OK;
    }
	else
	{
		g_iErrCode = 0x00020005;
    	return E_FAIL;
	}
}

HRESULT XDM_JpegEncode_Yuv420P(
	const PBYTE8 	pbSrcYUV420P_Y,
	const PBYTE8	pbSrcYUV420P_U,
	const PBYTE8	pbSrcYUV420P_V,
    PBYTE8 			pbJpegData,
    PDWORD32 		pdwJpegLen,
    DWORD32			dwWidth,
    DWORD32			dwHeight,
    DWORD32			dwQValue
)
{
    IIMGENC1_Status			status = {0};
    IIMGENC1_InArgs			inArgs = {0};
    IIMGENC1_OutArgs		outArgs= {0};
    IIMGENC1_Params			params = {0};
    IIMGENC1_DynamicParams	dynamicParams = {0};

    XDM1_BufDesc inputBufDesc = {NULL};
    XDM1_BufDesc outputBufDesc = {NULL};

    IALG_Handle handle = NULL;
    IIMGENC1_Fxns *IIMGENC1fxns = NULL;

    XDAS_Int32 retVal = XDM_EFAIL;

    if ( pbSrcYUV420P_Y == NULL
			|| pbSrcYUV420P_U == NULL
			|| pbSrcYUV420P_V == NULL
            || pbJpegData == NULL
            || pdwJpegLen == NULL )
    {
		g_iErrCode = 0x00020001;
        return E_POINTER;
    }

    status.size                     = sizeof(IIMGENC1_Status);

    inArgs.size                     = sizeof(IIMGENC1_InArgs);

    outArgs.size                    = sizeof(IIMGENC1_OutArgs);
    outArgs.extendedError           = 0;
    outArgs.bytesGenerated          = 0;
    outArgs.currentAU               = 0;

    params.size						= sizeof(IIMGENC1_Params);
    params.maxWidth					= dwWidth;
    params.maxHeight				= dwHeight;
    params.maxScans					= XDM_DEFAULT;
    params.dataEndianness			= XDM_BYTE;
    params.forceChromaFormat		= XDM_YUV_420P;

    dynamicParams.size				= sizeof(IIMGENC1_DynamicParams);
    dynamicParams.inputChromaFormat	= XDM_YUV_420P;
    dynamicParams.numAU				= XDM_DEFAULT;
    dynamicParams.generateHeader	= XDM_ENCODE_AU;
    dynamicParams.qValue			= dwQValue;
    dynamicParams.inputWidth		= dwWidth;
    dynamicParams.inputHeight		= dwHeight;
    dynamicParams.captureWidth		= XDM_DEFAULT;

    if ((handle = (IALG_Handle)ALG_create_JpegEnc(
                      (IALG_Fxns *) &DMJPGE_TIGEM_IDMJPGE,
                      (IALG_Handle) NULL,
                      (IALG_Params *) &params)) == NULL)
    {
		g_iErrCode = 0x00020002;
        return E_FAIL;
    }

    IIMGENC1fxns = (IIMGENC1_Fxns *)handle->fxns;

	handle->fxns->algActivate(handle);

	IIMGENC1fxns->control(
        (IIMGENC1_Handle)handle,
        XDM_SETPARAMS,
        (IIMGENC1_DynamicParams *)&dynamicParams,
        (IIMGENC1_Status *)&status
    );

    IIMGENC1fxns->control(
        (IIMGENC1_Handle)handle,
        XDM_GETBUFINFO,
        (IIMGENC1_DynamicParams *)&dynamicParams,
        (IIMGENC1_Status *)&status
    );

    inputBufDesc.numBufs = status.bufInfo.minNumInBufs;
    if ( inputBufDesc.numBufs != 3 )
    {
		g_iErrCode = 0x00020003;
		handle->fxns->algDeactivate(handle);
		ALG_delete_JpegEnc(handle);
    	return E_FAIL;
	}
    inputBufDesc.descs[0].buf     = (XDAS_Int8 *)pbSrcYUV420P_Y;
    inputBufDesc.descs[0].bufSize = status.bufInfo.minInBufSize[0];
	inputBufDesc.descs[1].buf     = (XDAS_Int8 *)pbSrcYUV420P_U;
    inputBufDesc.descs[1].bufSize = status.bufInfo.minInBufSize[1];
	inputBufDesc.descs[2].buf     = (XDAS_Int8 *)pbSrcYUV420P_V;
    inputBufDesc.descs[2].bufSize = status.bufInfo.minInBufSize[2];

    outputBufDesc.numBufs = status.bufInfo.minNumOutBufs;
    if ( outputBufDesc.numBufs != 1 )
    {
		g_iErrCode = 0x00020004;
		handle->fxns->algDeactivate(handle);
		ALG_delete_JpegEnc(handle);
    	return E_FAIL;
	}
    outputBufDesc.descs[0].buf     = (XDAS_Int8 *)pbJpegData;
    outputBufDesc.descs[0].bufSize = *pdwJpegLen;

    retVal = IIMGENC1fxns->process(
                 (IIMGENC1_Handle)handle,
                 (XDM1_BufDesc *)&inputBufDesc,
                 (XDM1_BufDesc *)&outputBufDesc,
                 (IIMGENC1_InArgs *)&inArgs,
                 (IIMGENC1_OutArgs *)&outArgs
             );

    if (retVal == XDM_EFAIL)
    {
	    IIMGENC1fxns->control(
	        (IIMGENC1_Handle)handle,
	        XDM_GETSTATUS,
	        (IIMGENC1_DynamicParams *)&dynamicParams,
	        (IIMGENC1_Status *)&status
	    );
		g_iErrCode = status.extendedError;
		if ( JPEGENC_OUTPUT_BUFF_SIZE == g_iErrCode )
		{
			*pdwJpegLen = status.bufInfo.minOutBufSize[0];  // Comment by Shaorg: 这个最小输出缓冲区尺寸大小并不准确。
		}
		else
		{
			*pdwJpegLen = 0;
		}

		handle->fxns->algDeactivate(handle);
	    ALG_delete_JpegEnc(handle);
        return E_FAIL;
    }

    IIMGENC1fxns->control(
        (IIMGENC1_Handle)handle,
        XDM_GETSTATUS,
        (IIMGENC1_DynamicParams *)&dynamicParams,
        (IIMGENC1_Status *)&status
    );

	handle->fxns->algDeactivate(handle);
    ALG_delete_JpegEnc(handle);

    if ( status.extendedError == JPEGENC_SUCCESS )
    {
        *pdwJpegLen = outArgs.bytesGenerated;

		g_iErrCode = 0;
        return S_OK;
    }
	else
	{
		g_iErrCode = 0x00020005;
    	return E_FAIL;
	}
}
