/*
 *  Copyright 2005 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *
 */

/*
 *  ======== xdm.h ========
 *  This header defines all common types, constants, enums and functions shared 
 *  across the various XDM classes
 *  Version 0.4
 */
#ifndef XDM_
#define XDM_

#include <ialg.h>
#include <xdas.h>

#ifdef __cplusplus
extern "C" {
#endif

#define XDM_EOK       IALG_EOK
#define XDM_EFAIL     IALG_EFAIL
#define XDM_ERUNTIME  -2

#define XDM_MAX_IO_BUFFERS    16 

/* 
 *  Buffer desciptor for input and output buffers:
 */
typedef struct XDM_BufDesc {
    XDAS_Int8   **bufs;    /* Pointer to vector containing buffer addresses */
    XDAS_Int32   numBufs;  /* number of buffers                              */
    XDAS_Int32  *bufSizes; /* size of each buffer in 8-bit bytes             */
} XDM_BufDesc;

/* 
 *  Buffer information desciptor for input and output buffers: 
 *  To be filled by explicit control command.
 */
typedef struct XDM_AlgBufInfo {
    XDAS_Int32 minNumInBufs;       /* Min number of input buffers    */
    XDAS_Int32 minNumOutBufs;      /* Min number of output buffers   */
    XDAS_Int32 minInBufSize[XDM_MAX_IO_BUFFERS];  /* Min size in 8-bit bytes 
                                    * needed for each input buffer.    
                                    */
    XDAS_Int32 minOutBufSize[XDM_MAX_IO_BUFFERS]; /* Min size in 8-bit bytes 
                                    * needed for each output buffer 
                                    */
} XDM_AlgBufInfo ;


/*
 *  ======== XDM_CmdId ========
 *  Standard control commands that must be implemented by XDM compliant 
 *  multimedia algorithms.
 *
 *  XDM_GETSTATUS  : To query multimedia algorithm to fill status structure 
 *                   explicitly.
 *  XDM_SETPARAMS  : To command multimedia algorithm to set run time parameters
 *                   via parameter structure explicitly.
 *  XDM_RESET      : To command multimedia algorithm to reset the algorithm. 
 *                   This may be init function or special function to recover 
 *                   after error or data skip kind of conditions.
 *  XDM_SETDEFAULT: To ask algorithm to initialize all params to their library
 *                   defaults . The application can chage specific parameter 
 *                   using SETPARAM.
 *  XDM_FLUSH      : To handle end of stream conditions. XDM_FLUSH command  
 *                   forces algorithm to output data without additional input. 
 *                   The recommended sequence is to call CONTROL API (with 
 *                   XDM_FLUSH) followed repeated calls to PROCESS APIs.
 *  XDM_GETBUFINFO : To query algorsithm instance regarding its properties 
 *                   of input and output buffers. 
 *  Note :
 *  ======
 *  Any control ID extension in IMOD interface should start from 256 onward. 
 *  The ID range from 5 to 255 is reserved for future use.
 */
typedef enum {
    XDM_GETSTATUS =0, XDM_SETPARAMS, XDM_RESET, XDM_SETDEFAULT, 
    XDM_FLUSH, XDM_GETBUFINFO
} XDM_CmdId;

/*
 *  Extended error enumeration for XDM compliant encoders and decoders:
 *
 *  Bit 16-32 : Reserved 
 *  Bit XDM_FATALERROR:       1 => Fatal error (Stop decoding) 
 *                            0 => Recoverable error 
 *  Bit XDM_UNSUPPORTEDPARAM: 1 => Unsupported input parameter or configuration 
 *                            0 => Ignore
 *  Bit XDM_UNSUPPORTEDINPUT: 1 => Unsupported feature/ parameter in input, 
 *                            0 => Ignore 
 *  Bit XDM_CORRUPTEDHEADER:  1 => Header problem/ corruption, 
 *                            0 => Ignore
 *  Bit XDM_CORRUPTEDDATA:    1 => Data problem /corruption, 
 *                            0 => Ignore
 *  Bit XDM_INSUFFICIENTDATA: 1 => Insufficient data, 
 *                            0 => Ignore
 *  Bit  XDM_APPLIEDCONCEALMENT:  1 => Applied concelement,  
 *                            0=> Ignore
 *  Bit  8 :    Reserved   
 *  Bit 7-0 :   Codec & Implementation specific
 *
 *  Notes :
 *  =====
 *  1) Algorithm will set multiple bits to 1 based on conditions. 
 *     e.g. it will set bits XDM_FATALERROR (fatal) and XDM_UNSUPPORTEDPARAM 
 *     (unsupported params) in case of unsupported run time parameters
 *  2) Some erors are applicable to decoders only.
 */
typedef enum {
    XDM_APPLIEDCONCEALMENT=9, XDM_INSUFFICIENTDATA=10, XDM_CORRUPTEDDATA=11,
    XDM_CORRUPTEDHEADER=12, XDM_UNSUPPORTEDINPUT=13, XDM_UNSUPPORTEDPARAM=14,
    XDM_FATALERROR=15
} XDM_ErrorBit;
 
#define XDM_ISFATALERROR(x)         (((x) >> XDM_FATALERROR) & 0x1) 
#define XDM_ISUNSUPPORTEDPARAM(x)   (((x) >> XDM_UNSUPPORTEDPARAM) & 0x1) 
#define XDM_ISUNSUPPORTEDINPUT(x)   (((x) >> XDM_UNSUPPORTEDINPUT) & 0x1) 
#define XDM_ISCORRUPTEDHEADER(x)    (((x) >> XDM_CORRUPTEDHEADER) & 0x1) 
#define XDM_ISCORRUPTEDDATA(x)      (((x) >> XDM_CORRUPTEDDATA) & 0x1) 
#define XDM_ISINSUFFICIENTDATA(x)   (((x) >> XDM_INSUFFICIENTDATA) & 0x1) 
#define XDM_ISAPPLIEDCONCEALMENT(x) (((x) >> XDM_APPLIEDCONCEALMENT) & 0x1) 

#define XDM_SETFATALERROR(x)         ((x)|=(0x1 << XDM_FATALERROR)) 
#define XDM_SETUNSUPPORTEDPARAM(x)   ((x)|=(0x1 << XDM_UNSUPPORTEDPARAM) ) 
#define XDM_SETUNSUPPORTEDINPUT(x)   ((x)|=(0x1 << XDM_UNSUPPORTEDINPUT) ) 
#define XDM_SETCORRUPTEDHEADER(x)    ((x)|=(0x1 << XDM_CORRUPTEDHEADER) ) 
#define XDM_SETCORRUPTEDDATA(x)      ((x)|=(0x1 << XDM_CORRUPTEDDATA) ) 
#define XDM_SETINSUFFICIENTDATA(x)   ((x)|=(0x1 << XDM_INSUFFICIENTDATA) ) 
#define XDM_SETAPPLIEDCONCEALMENT(x) ((x)|=(0x1 << XDM_APPLIEDCONCEALMENT) ) 

/* 
 * ======== XDM_DataFormat ========
 * Endian-ness of data
 * XDM_BYTE  : Big endian stream
 * XDM_LE_16 : 16 bit little endian stream
 * XDM_LE_32 : 32 bit little endian stream
 */
typedef enum {
    XDM_BYTE =1, XDM_LE_16, XDM_LE_32
} XDM_DataFormat;

/* 
 * ======== XDM_EncodingPreset ========
 *  Encoding presets:
 *  XDM_DEFAULT => Default setting of encoder
 *  XDM_HIGH_QUALITY => High quality Encoding (Default setting)
 *  XDM_HIGH_SPEED => High speed  Encoding (Default setting)
 *  XDM_USER_DEFINED => User defined configuration using advanced parameters 
 */
typedef enum {
    XDM_DEFAULT=0, XDM_HIGH_QUALITY, XDM_HIGH_SPEED, XDM_USER_DEFINED
} XDM_EncodingPreset; 

/* Decode entire access unit or only header */
typedef enum {
    XDM_DECODE_AU =0, XDM_PARSE_HEADER
} XDM_DecMode;

/* Encode entire access unit or only header */
typedef enum {
    XDM_ENCODE_AU =0, XDM_GENERATE_HEADER
} XDM_EncMode;

/*
 * ======== XDM_ChromaFormat ========
 * XDM supported chroma formats:
 * XDM_YUV_420P   : YUV 4:2:0 planer
 * XDM_YUV_422IBE : YUV 4:2:2 interleaved (big endian)
 * XDM_YUV_422ILE : YUV 4:2:2 interleaved (little endian)
 * XDM_YUV_444P   : YUV 4:4:4 planer
 * XDM_YUV_411P   : YUV 4:1:1 palner
 * XDM_GRAY       : Gray format onwards
 * Others     : User defined
 */
typedef enum {
    XDM_YUV_420P = 1, XDM_YUV_422P, XDM_YUV_422IBE, 
    XDM_YUV_422ILE, XDM_YUV_444P, XDM_YUV_411P, 
    XDM_GRAY, XDM_RGB
} XDM_ChromaFormat;

#ifdef __cplusplus
}
#endif

#endif  /* XDM_ */

