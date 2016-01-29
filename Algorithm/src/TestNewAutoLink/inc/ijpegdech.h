/*
  *  Copyright 2006 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *
 */
/*
 *  ======== ijpegdec.h ========
 *  IJPEGDEC Interface Header
 */
#ifndef IJPEGDEC_
#define IJPEGDEC_

#include <xdas.h>
#include <ialg.h>                        
#include "iimgdec.h"

/* JPEG decoder Error status messages */
typedef enum{
    
    
	JPEGDEC_SUCCESS = 0,					/*Successful decoding*/

	
	JPEGDEC_ERROR_UNSUPPORTED_FORMAT = 1,    /*SOI not found*/

	/* I/P image is progressive , but progressiveDecFlag of Dynamicparams 
	   is not set So heap required for decoding progressive image is not 
	   allocated */

	JPEGDEC_ERROR_PROG_MEM_ALOC = 2,	
	
	JPEGDEC_ERROR_DQT_NF = 3,				/*Quantization table not found*/
	
	JPEGDEC_ERROR_SOS= 4,					/*Start of scan marker not found*/
	
	JPEGDEC_ERROR_INSUFFICIENT_DATA= 5,		/*Input buffer underflow*/
	
    JPEGDEC_ERROR_DISPLAY_WIDTH= 6,			/*Invalid display width (displaywidth < imagewidth)*/
	
	JPEGDEC_ERROR_VLD= 7,					/* Error in VLD */

    JPEGDEC_ERROR_MARKER= 8,				/* invalid or reserved marker    */
  
	JPEGDEC_ERROR_BAD_MARKER_LENGTH= 9,		/* Invalid marker length         */
 
	JPEGDEC_END_OF_IMAGE= 10,				/* We reached end of picture     */

    JPEGDEC_ERROR_DQT_TYPE= 11,				/* Invalid quant table type or Q-table not supported */
 
	JPEGDEC_ERROR_BAD_DQT_LEN= 12,			/* invalid DQT segment length      */
 
	JPEGDEC_ERROR_SOS_COMP_ID= 13,			/* Invalid component ID in SOS Marker*/
 
	JPEGDEC_ERROR_BAD_SOS_LEN= 14,			/* Invalid length of SOS or Bad component numbers*/
 
	JPEGDEC_ERROR_SOS_INVALID= 15,			/* SOS marker is invalid */
 
	JPEGDEC_ERROR_SOS_NO_SOF= 16,			/* Invalid JPEG file structure: SOS before SOF */
 
	JPEGDEC_ERROR_BAD_PROGRESSION= 17,		/* Invalid progressive parameters */

	JPEGDEC_ERROR_COMPONENT_COUNT= 18,		/* Too many or few color components in the scan or frame */
	
	JPEGDEC_ERROR_BAD_MCU_SIZE= 19,			/* Sampling factors too large for interleaved scan */
 
	JPEGDEC_ERROR_OUTSIZE= 20,				/* Too big Image, Output buffer size is too Small */
 
	JPEGDEC_ERROR_IMAGE_SIZE= 21,		    /* Input Image size is greater than Maximum set size. 
	                                           Resizing factor will be calculated to fit the image 
											   in a given output buffer size & give a resized output image*/

	JPEGDEC_ERROR_RST_NF= 22,				/* RST not found                   */
 
	JPEGDEC_ERROR_BAD_HUFF_TABLE= 23,		/* Improper Huffman table definition*/
 
	JPEGDEC_ERROR_DHT_INDEX= 24,			/* unknown huffman table index  */
 
	JPEGDEC_ERROR_DHT_TYPE= 25,				/* unknown huffman table type      */
 
	JPEGDEC_ERROR_DHT_LEN= 26,				/* invalid DHT segment length      */
 
	JPEGDEC_ERROR_BAD_PRECISION= 27,		/* sample precision != 8           */
 
	JPEGDEC_ERROR_SOF_DUPLICATE= 28,		/* Invalid JPEG file structure: two SOF markers */
 
	JPEGDEC_ERROR_EMPTY_IMAGE= 29,			/* Image width | height | numcomponents invalid*/
 
	JPEGDEC_ERROR_BAD_SOF_DATA = 30,		/* Bad SOF Marker length or component  */

    JPEGDEC_ERROR_BAD_DRI_LEN= 31,			/* Bad DRI length */
	
	JPEGDEC_ERROR_EOI_EXPECTED= 32,			/* Not expected multiple scans in Baseline format. Invalid jpg*/
	
	JPEGDEC_ERROR_HUFFTAB_DEST = 33,		/* Wrong huffman table destination identifier*/  

	JPEGDEC_ERROR_NULL_STREAM_PTR = 34,		/* NULL input stream pointer */
    
	JPEGDEC_ERROR_HA_FLAG = 35,				/* Invalid Header analysis flag, it can take value of only 0 or 1*/ 
    
	JPEGDEC_ERROR_NULL_OUT_PTR = 36,		/* NULL output buffer pointer*/
    
	JPEGDEC_ERROR_OUTPUT_FORMAT = 37,		/* Invalid output format option */
    
	JPEGDEC_ERROR_NO_DATA = 38,				/* bytesInStream <= 0 */
	JPEGDEC_ERROR_DC_TABLE_SIZE = 39,		/*Insufficient DC huffman table size*/
    JPEGDEC_ERROR_AC_TABLE_SIZE = 40,		/*Insufficient AC huffman table size*/
	JPEGDEC_ERROR_NOT_BASELINE_INTL = 41,	/* Image is not sequential */
	JPEGDEC_ERROR_CORRUPTED_BITSTREAM = 42, /* Corrupted Bit stream */
	JPEGDEC_ERROR_SCAN_FREQ = 43,			/* Inavlid Scan frequency */
	JPEGDEC_ERROR_RST_MARKER = 44,			/* Missing Restart Marker */
	JPEGDEC_ERROR_MISSING_MARKER = 45,		/* Missing either SOS, SOF , DHT or DQT marker */
	JPEGDEC_ERROR_INPUT_PARAMETER = 46		/* Error occured in the interface parameter */


}IJPEGDEC_ErrorStatus;

/* JPEG warnings */
#define WARN_JPGD_BAD_PROGRESSION  0x2500   /* Inconsistent progression sequence, AC coeff without DC coeff being decoded */
#define WARN_JPGD_APPx_MARKER_SKIP 0x2501   /* Decoding of APP0 marker not supported. It will be skipped.*/

/*JPEG default values */

#define DEFAULT_BITSTREAM_SIZE 1024*1536
#define DEFAULT_HEIGHT   1600
#define DEFAULT_WIDTH    2048
#define DEFAULT_SCAN	15

#define JPEGDEC_YUV420    XDM_YUV_420P
#define JPEGDEC_YUV422    XDM_YUV_422P
#define JPEGDEC_YUV444    XDM_YUV_444P
#define JPEGDEC_YUV411    XDM_YUV_411P
#define JPEGDEC_YUVGRAY   XDM_GRAY
#define JPEGDEC_YUV422ILE XDM_YUV_422ILE

/*
 *  ======== IJPEGDEC_Cmd ========
 *  The Cmd enumeration defines the control commands for the JPEGDEC
 *  control method.
 */

typedef IIMGDEC_Cmd IJPEGDEC_Cmd;

/*
// ===========================================================================
// control method commands
*/
#define IJPEGDEC_GETSTATUS      XDM_GETSTATUS
#define IJPEGDEC_SETPARAMS      XDM_SETPARAMS
#define IJPEGDEC_RESET          XDM_RESET
#define IJPEGDEC_FLUSH          XDM_FLUSH
#define IJPEGDEC_SETDEFAULT     XDM_SETDEFAULT
#define IJPEGDEC_GETBUFINFO     XDM_GETBUFINFO

/*
 *  ======== IJPEGDEC_Handle ========
 *  This handle is used to reference all JPEG_DEC instance objects
 */
typedef struct IJPEGDEC_Obj *IJPEGDEC_Handle;

/*
 *  ======== IJPEGDEC_Obj ========
 *  This structure must be the first field of all JPEG_DEC instance objects
 */
typedef struct IJPEGDEC_Obj {
    struct IJPEGDEC_Fxns *fxns;
} IJPEGDEC_Obj;

/*
 *  ======== IJPEGDEC_Params ========
 *  This structure defines the creation parameters for all JPEG_DEC objects
 */
typedef struct IJPEGDEC_Params {
    /* -------------------------------------------------------------------- */
    /* Global Memory Pool                                                   */
    /* -------------------------------------------------------------------- */
	
	IIMGDEC_Params imgdecParams;
  
    XDAS_Int32	progressiveDecFlag; 	 /* Set this flag to 1 if progressive 
									      decoding is required, in additional 
									      to baseline sequential mode */
 
} IJPEGDEC_Params;

/*
 *  ======== IJPEGDEC_Params ========
 *  This structure defines the Dynamic parameters for all JPEG_DEC objects
 */
typedef struct IJPEGDEC_DynamicParams {
     	
	IIMGDEC_DynamicParams imgdecDynamicParams;
	XDAS_Int32			progDisplay;	
    	/* Set the display option for progressive mode:           */
		/* By Seting progDisplay to 1, The output buffer contains */
		/* the partially (progressively)decoded image after each  */
		/* scan is decoded.                                       */
		/* By Seting progDisplay to 0, The output buffer contains */
		/* the decoded image only after all the scans are decoded */    
     
   
    XDAS_Int32         resizeOption;		/* Set the resizing option: */
									/* 0: No resizing           */
									/* 1: resize the o/p image by 1/2   		*/
									/* 2: resize the o/p image by 1/4   		*/
									/* 3: resize the o/p image by 1/8   		*/
  
} IJPEGDEC_DynamicParams;

/*
 *  ======== IJPEGDEC_Status ========
 *  This structure defines the status parameters for all JPEG_DEC objects
 *  Presently IJPEGDEC_Status is same as IJPEGDEC_Output structure
 */
typedef struct IJPEGDEC_Status {
  IIMGDEC_Status imgdecStatus;
  XDAS_Int32        mode;           /* 0: baseline sequential, 1: progressive*/
  XDAS_Int32        imageHeight;    /* Actual height of the image*/
  XDAS_Int32        stride[3];      /* stride values for Y,U and V components */
  XDAS_Int32        decImageSize;   /* size of the decoded image in bytes*/
  XDAS_Int32        lastMCU;        /* 1: last MCU in the frame, 0: not last*/
  
} IJPEGDEC_Status;

/*
 *  ======== IJPEGDEC_InArgs ========
 *  This structure defines the runtime input arguments for IJPEGDEC::process
 */

typedef struct IJPEGDEC_InArgs{
    	
	IIMGDEC_InArgs imgdecInArgs;    
	
} IJPEGDEC_InArgs;

/*
 *  ======== IJPEGDEC_OutArgs ========
 *  This structure defines the run time output arguments for IJPEGDEC::process
 *  function.
 */

typedef struct IJPEGDEC_OutArgs{
    	
	IIMGDEC_OutArgs imgdecOutArgs;    
    XDAS_Int8 end_of_seq;                  /* end of seq flag */
	
} IJPEGDEC_OutArgs;


/*
 *  ======== IJPEGDEC_PARAMS ========
 *  Default parameter values for JPEG_DEC instance objects
 */
extern IJPEGDEC_Params IJPEGDEC_PARAMS;
extern IJPEGDEC_DynamicParams IJPEGDEC_DYNAMICPARAMS;

/*
 *  ======== IJPEGDEC_Fxns ========
 *  This structure defines all of the operations on JPEG_DEC objects
 */
typedef struct IJPEGDEC_Fxns {
    	IIMGDEC_Fxns iimgdec; 
} IJPEGDEC_Fxns;

#endif	/* IJPEGDEC_ */
