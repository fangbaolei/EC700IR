/*
 *  Copyright 2005 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *
 */
/*
 *  ======== jpegdec.h ========
 *  This header defines the interface used by clients of the JPEGDEC module
 */
#ifndef JPEGDEC_
#define JPEGDEC_

#include <ijpegdech.h>
#include <alg.h>

/*
 *  ======== JPEGDEC_Handle ========
 *  This pointer is used to reference all JPEGDEC instance objects
 */
typedef struct IJPEGDEC_Obj *JPEGDEC_Handle;

/*
 *  ======== JPEGDEC_Params ========
 *  This structure defines the creation parameters for all JPEGDEC objects
 */
typedef IJPEGDEC_Params JPEGDEC_Params;

/*
// ===========================================================================
// JPEGDEC_DynamicParams
//
// This structure defines the creation parameters for all JPEGDEC objects
*/

typedef IJPEGDEC_DynamicParams JPEGDEC_DynamicParams;

/*
// ===========================================================================
// JPEGDEC_Status
//
// This structure defines the real-time parameters for JPEGDEC objects
*/

typedef IJPEGDEC_Status JPEGDEC_Status;

/*
// ===========================================================================
// JPEGDEC_InArgs
//
// This structure defines the real-time input arguments for JPEGDEC objects
*/

typedef IJPEGDEC_InArgs JPEGDEC_InArgs;

/*
// ===========================================================================
// JPEGDEC_OutArgs
//
// This structure defines the real-time output arguments for JPEGDEC objects
*/

typedef IJPEGDEC_OutArgs JPEGDEC_OutArgs;
 
/*
// ===========================================================================
// JPEGDEC_Cmd
//
// This typedef defines the control commands MP4VE objects
*/
typedef IJPEGDEC_Cmd  JPEGDEC_Cmd;

/*
// ===========================================================================
// control method commands
*/
#define JPEGDEC_GETSTATUS    IJPEGDEC_GETSTATUS
#define JPEGDEC_SETPARAMS    IJPEGDEC_SETPARAMS
#define JPEGDEC_PREPROCESS   IJPEGDEC_PREPROCESS
#define JPEGDEC_RESET        IJPEGDEC_RESET
#define JPEGDEC_FLUSH        IJPEGDEC_FLUSH
#define JPEGDEC_SETDEFAULT   IJPEGDEC_SETDEFAULT
#define JPEGDEC_GETBUFINFO   IJPEGDEC_GETBUFINFO

/*
 *  ======== JPEGDEC_PARAMS ========
 *  This structure defines the default creation parameters for JPEGDEC objects
 */
#define JPEGDEC_PARAMS   IJPEGDEC_PARAMS

/*
 *  ======== JPEGDEC_create ========
 *  Create an JPEGDEC instance object (using parameters specified by prms)
 */
extern JPEGDEC_Handle JPEGDEC_create(const IJPEGDEC_Fxns *fxns, const JPEGDEC_Params *prms);

/*
 *  ======== JPEGDEC_delete ========
 *  Delete the JPEGDEC instance object specified by handle
 */
extern Void JPEGDEC_delete(JPEGDEC_Handle handle);

/*
 *  ======== JPEGDEC_control ========
 *  Function to either write to the read/write parameters in the status
 *  structure or to read all the parameters in the status structure.
 */
extern XDAS_Int32 JPEGDEC_control(IJPEGDEC_Handle handle, IIMGDEC_Cmd id, 
            IIMGDEC_DynamicParams *params, IIMGDEC_Status *status);
/*
 *  ======== JPEGDEC_decode ========
 */
extern XDAS_Int32 JPEGDEC_decode(IJPEGDEC_Handle handle, XDM_BufDesc *inBufs, 
            XDM_BufDesc *outBufs, IIMGDEC_InArgs *inargs, 
            IIMGDEC_OutArgs *outargs);    




#endif	/* JPEGDEC_ */
