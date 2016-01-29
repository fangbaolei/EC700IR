#ifndef _JPEGENCAPI_H_
#define _JPEGENCAPI_H_

#include "swbasetype.h"

#ifdef __cplusplus
extern "C" {
#endif

HRESULT XDM_JpegEncode_CbYCrY(
	const PBYTE8 	pbSrcCbYCrY,
    PBYTE8 			pbJpegData,
    PDWORD32 		pdwJpegLen,
    DWORD32			dwWidth,
    DWORD32			dwHeight,
    DWORD32			dwQValue
);

HRESULT XDM_JpegEncode_Yuv422P(
	const PBYTE8 	pbSrcYUV422P_Y,
	const PBYTE8	pbSrcYUV422P_U,
	const PBYTE8	pbSrcYUV422P_V,
    PBYTE8 			pbJpegData,
    PDWORD32 		pdwJpegLen,
    DWORD32			dwWidth,
    DWORD32			dwHeight,
    DWORD32			dwQValue
);

HRESULT XDM_JpegEncode_Yuv420P(
	const PBYTE8 	pbSrcYUV420P_Y,
	const PBYTE8	pbSrcYUV420P_U,
	const PBYTE8	pbSrcYUV420P_V,
    PBYTE8 			pbJpegData,
    PDWORD32 		pdwJpegLen,
    DWORD32			dwWidth,
    DWORD32			dwHeight,
    DWORD32			dwQValue
);

#ifdef __cplusplus
}
#endif

#endif
