/******************************************************************************/
/*            Copyright (c) 2006 Texas Instruments, Incorporated.             */
/*                           All Rights Reserved.                             */
/******************************************************************************/

/*!
********************************************************************************
  @file     TestAppDecoder.h
  @brief    This is the top level client header file that drives the H264
            (Baseline Profile) Video Decoder Call using XDM Interface
  @author   Multimedia Codecs TI India
  @version  0.1 - Jan 24,2006    initial version
********************************************************************************
*/

#ifndef _TI_IMAGE_PROCESSER_
#define _TI_IMAGE_PROCESSER_


#include <xdas.h>
#include <xdm.h>
#include <iimgdec.h>


#include "swBaseType.h"
#include "swWinError.h"
#include "swImage.h"

#ifdef __cplusplus
	extern "C" {
#endif	/* #ifdef __cplusplus */

#define JPEGDEC_IMAGE_MAX_WIDTH            2500      /* Width of the Display Buffer       */      
#define JPEGDEC_IMAGE_MAX_HEIGHT           2100      /* Height of the Display Buffer      */
#define JPEGDEC_OUTPUT_BUFFER_SIZE     	   ( JPEGDEC_IMAGE_MAX_WIDTH  * JPEGDEC_IMAGE_MAX_HEIGHT * 2 )

#define JPEG_HEADER_LEN			2048

HRESULT XDM_JpegDecode(
	PBYTE8 				pbJpegData,
	DWORD32 			dwJpegLen,
	PBYTE8				pbYUVData,
	PDWORD32			pdwYUVLen,
	PDWORD32			pdwWidth,
	PDWORD32			pdwHeight,
	PDWORD32			pdwStride
);

#ifdef __cplusplus
	}
#endif	/* #ifdef __cplusplus */

#endif //_TESTAPP_DECODER_

/******************************************************************************/
/*    Copyright (c) 2006 Texas Instruments, Incorporated                      */
/*    All Rights Reserved                                                     */
/******************************************************************************/
