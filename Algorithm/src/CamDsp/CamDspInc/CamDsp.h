#ifndef _CAMDSPTYPE_H_
#define _CAMDSPTYPE_H_

#include "swbasetype.h"
#include "swwinerror.h"
#include "swimage.h"
#include "DspLink.h"
#include "DspLinkCmd.h"
#include "DmaCopyApi.h"
#include "JpegEncApi.h"
#include "H264EncApi.h"
#include "CamDspProc.h"

extern HRESULT YUV2JPG_TI(
	const JPEG_ENCODE_PARAM* pJpegEncodeParam,
	const JPEG_ENCODE_DATA* pJpegEncodeData,
	JPEG_ENCODE_RESPOND* pJpegEncodeRespond
);

extern HRESULT YUV2H264_TI(
	const H264_ENCODE_PARAM* pH264EncodeParam,
	const H264_ENCODE_DATA* pH264EncodeData,
	H264_ENCODE_RESPOND* pH264EncodeRespond
);

extern HRESULT CamDspProc(
	const CAM_DSP_PARAM* pCamDspParam,
	const CAM_DSP_DATA* pCamDspData,
	CAM_DSP_RESPOND* pCamDspRespond
);

#endif
