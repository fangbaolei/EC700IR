/*
 *  Copyright 2005 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *
 */
/*
 *  ======== iimgdec.h ========
 *  This header defines all types, constants, and functions shared by all
 *  implementations of the image decoder algorithms.
 *  Version 0.4
 */

#ifndef IIMGDEC_
#define IIMGDEC_

#include <ialg.h>
#include <xdas.h>
#include "xdm.h"

#ifdef __cplusplus
extern "C" {
#endif

#define IIMGDEC_CID      "ti.xdais.xdm.IIMGDEC"

#define IIMGDEC_ERUNTIME  XDM_ERUNTIME
#define IIMGDEC_EOK       XDM_EOK
#define IIMGDEC_EFAIL     XDM_EFAIL

/*
 *  ======== IIMGDEC_Obj ========
 *  This structure must be the first field of all IMGDEC instance objects.
 */
typedef struct IIMGDEC_Obj {
    struct IIMGDEC_Fxns *fxns;
} IIMGDEC_Obj;

/*
 *  ======== IIMGDEC_Handle ========
 *  This handle is used to reference all IMGDEC instance objects.
 */
typedef struct IIMGDEC_Obj  *IIMGDEC_Handle;

/*
 *  ======== IIMGDEC_Params ========
 *  This structure defines the creation parameters for all IMGDEC instance
 *  objects.
 */
typedef struct IIMGDEC_Params {
    XDAS_Int32 size;             /* size of this structure */
    XDAS_Int32 maxHeight;        /* Maximum height */    
    XDAS_Int32 maxWidth;         /* Maximum width */
    XDAS_Int32 maxScans;         /* Maximum number of scans */
    XDAS_Int32 dataEndianness;   /* Endianness of output data. 
                                  * (see definition in XDM_DataFormat)
                                  */
    XDAS_Int32 forceChromaFormat;/* Force encoding in given Chroma format. 
                                  * Use definition in XDM_ChromaFormat,
                                  * or set to XDM_DEFAULT to avoid this.
                                  */
}IIMGDEC_Params;

/*  ======== IIMGDEC_DynamicParams ========
 *  This structure defines the codec parameters that can be modified after 
 *  creation via 'control' calls. 
 *
 *  It is not necessary that given implementation support all dynamic 
 *  parameters to be configurable at run time. If a particular codec
 *  does not support run-time updates to a parameter that  the application
 *  is attempting to change at runtime, the codec may indicate this as an
 *  error. 
 */
typedef struct IIMGDEC_DynamicParams {
    XDAS_Int32 size;         /* size of this structure */
    XDAS_Int32 numAU;        /* Number of Access unit to encode, 
                              * set to XDM_DEFAULT in case of entire frame
                              */
    XDAS_Int32 decodeHeader; /* XDM_DecMode: XDM_DECODE_AU or XDM_PARSE_HEADER*/
    XDAS_Int32 displayWidth; /* DEFAULT (0): use imagewidth as pitch, else use 
                              * given DisplayWidth (if > imagewidth) for pitch
                              */
} IIMGDEC_DynamicParams;

/*
 *  ======== IIMGDEC_InArgs ========
 *  This structure defines the runtime input arguments for IIMGDEC::process
 */
typedef struct IIMGDEC_InArgs {
    XDAS_Int32 size;     /* size of this structure */
    XDAS_Int32 numBytes; /* Number of valid input data in bytes in input buffer
                          */
} IIMGDEC_InArgs;
/*
 *  ======== IIMGDEC_Status ========
 *  This structure defines instance status parameters (read-only).
 */
typedef struct IIMGDEC_Status {
    XDAS_Int32 size;           /* size of this structure */
    XDAS_Int32 extendedError;  /* Extended Error code. (see XDM_ErrorBit) */
    XDAS_Int32 outputHeight;   /* Output Height */ 
    XDAS_Int32 outputWidth;    /* Output Width (it takes care of padding 
                                * by decoder)
                                */ 
    XDAS_Int32 imageWidth;     /* Image  Width */ 
    XDAS_Int32 outChromaformat;/* Output chroma format: XDM_ChromaFormat */
    XDAS_Int32 totalAU;        /* Total number of Access Units (say MCU) */
    XDAS_Int32 totalScan;      /* Total number of scans */
    XDM_AlgBufInfo  bufInfo;   /* Input & output buffer information */
} IIMGDEC_Status;

/*
 *  ======== IIMGDEC_OutArgs ========
 *  This structure defines the run time output arguments for IMGDEC::process
 *  function.
 */
typedef struct IIMGDEC_OutArgs {
    XDAS_Int32 size;           /*  size of this structure */
    XDAS_Int32 extendedError;  /*  Extended Error code (see XDM_ErrorBit) */
    XDAS_Int32 bytesconsumed;  /*  The bytes consumed */
    XDAS_Int32 currentAU;      /*  Current Access Unit Number */
    XDAS_Int32 currentScan;    /*  Current Scan Number */
} IIMGDEC_OutArgs;

/*
 *  ======== IIMGDEC_Cmd ========
 *  This structure defines the control commands for the IMGENC module.
 */

typedef  IALG_Cmd IIMGDEC_Cmd;


/*
 *  ======== IIMGDEC_Fxns ========
 *  This structure defines all of the operations on IMGDEC objects.
 
 *  process() - Basic imaging encode call. It takes input and output buffer
 *              descriptors and structures for input and output arguments. 
 *              The algorithm may also modify the output buffer pointers.
 *              The return value is IALG_EOK for success or IALG_EFAIL
 *              in case of failure. The extendedError field of the outArgs
 *              contain further error conditions flagged by the algorithm.
 *
 *  control() - This will control behaviour of algorithm via Cmd ID. 
 *              All XDM Control Commands are supported:
 *                  XDM_GETSTATUS =0, XDM_SETPARAMS, XDM_RESET, XDM_SETDEFAULT,
 *                  XDM_FLUSH, XDM_GETBUFINFO
 *              See xdm.h for details. 
 *              The return value is IALG_EOK for success or IALG_EFAIL.
 *              In case of failure extended error code contains details. 
 *
 */
typedef struct IIMGDEC_Fxns{
    IALG_Fxns  ialg;
    XDAS_Int32 (*process)(IIMGDEC_Handle handle, XDM_BufDesc *inBufs, 
            XDM_BufDesc *outBufs, IIMGDEC_InArgs *inargs, 
            IIMGDEC_OutArgs *outargs);
    XDAS_Int32 (*control)(IIMGDEC_Handle handle, IIMGDEC_Cmd id, 
            IIMGDEC_DynamicParams *params, IIMGDEC_Status *status);
}IIMGDEC_Fxns; 

#ifdef __cplusplus
}
#endif

#endif  /* IIMGDEC_ */



