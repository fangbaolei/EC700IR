#ifndef _H264ENCAPI_H_
#define _H264ENCAPI_H_

/*
 * If you are using an evaluation version of this codec, there will be a limit of 
 * encoding up to 54000 frames in the usage of the encoder. 
 */

#include "swBaseType.h"

#include "h264venc.h"
#include "ih264fhdvenc.h"
#include "h264fhdvenc_tii.h"

#ifdef __cplusplus
extern "C" {
#endif

// H.264编码器句柄结构
typedef struct tagH264EncHandle
{
	H264FHDVENC_Handle            handle;  /* Handle to the Encoder instance       - XDAIS */
	IH264FHDVENC_Fxns             fxns;    /* Function table for the H.264 Encoder - XDAIS */

	H264FHDVENC_InArgs            inargs;  /* Input Parameter to the process call  - XDM */
	H264FHDVENC_OutArgs           outargs; /* Ouput parameters from process call   - XDM */
	IVIDEO1_BufDescIn             inobj;   /* Input Buffer description array       - XDM */
	XDM_BufDesc                   outobj;  /* Output Buffer description array      - XDM */
	H264FHDVENC_Status            status;  /* Stores the status of process call    - XDM */
	H264FHDVENC_Params            params;  /* Instance creation Parameter          - XDM */
	H264FHDVENC_DynamicParams     dynamicparams; /* Dynamic Parameter struct       - XDM */
	IH264FHDVENC_VUIDataStructure vui_params;

	XDAS_Int32                    rgBufSizes[2];
	XDAS_Int8*                    rgBufs[2]; 
} H264EncHandle;

/* alg_malloc.c */
void InitH264OnChipRAM();

/**
* @brief		打开H.264编码器
* @param[out]	pHandle					返回的编码器句柄
* @param[in]	dwIntraFrameInterval	I帧间隔
* @param[in]	dwTargetBitRate			目标比特率
* @param[in]	dwFrameRate				传入帧率
* @param[in]	dwInputWidth			源图宽度
* @param[out]	dwInputHeight			源图高度
* @param[out]	dwOutputBufSize			输出缓冲区大小
* @return		成功 !NULL 失败 NULL
*/
int H264Enc_Open(
	H264EncHandle* pHandle,
	DWORD32 dwIntraFrameInterval,
	DWORD32 dwTargetBitRate,
	DWORD32 dwFrameRate,
	DWORD32 dwInputWidth,
	DWORD32 dwInputHeight,
	DWORD32 dwOutputBufSize
);

/**
* @brief		进行H.264编码
* @param[in]	pHandle					编码器句柄
* @param[in]	pbSrcYUV422SP_Y			待编码图片Y数据
* @param[in]	pbSrcYUV422SP_UV		待编码图片UV数据
* @param[out]	pbH264BitStream			编码后缓冲区指针
* @param[out]	pdwFrameLen				编码后的帧长度
* @param[out]	pdwFrameType			编码后的帧类型（I/P）
* @return		成功 0 失败 -1
*/
int H264Enc_Encode(
	H264EncHandle* pHandle,
	const PBYTE8 pbSrcYUV422SP_Y,
	const PBYTE8 pbSrcYUV422SP_UV,
	PBYTE8 pbH264BitStream,
	PDWORD32 pdwFrameLen,
	PDWORD32 pdwFrameType
);

/**
* @brief		关闭H.264编码器
* @param[in]	pHandle					编码器句柄
* @return		0
*/
int H264Enc_Close(H264EncHandle* pHandle);

#ifdef __cplusplus
}
#endif

#endif
