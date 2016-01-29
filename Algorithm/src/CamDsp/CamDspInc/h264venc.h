/** ===========================================================================
* @file H264FHDVENC.h
*
* @path $(PROJDIR)\..\include\XDAS\
*
* @desc This File definesinterface used by clients of the H264FHDVENC module
* =============================================================================
* Copyright (c) Texas Instruments Inc 2005, 2006
*
* Use of this software is controlled by the terms and conditions found
* in the license agreement under which this software has been supplied 
* ===========================================================================*/
/*!
*! Revision History
*! ================
*! 23-01-2006 Anirban Basu: Created.
*/

#ifndef __H264FHDVENC__
#define __H264FHDVENC__

/*****************************************************************************/
/*                              INCLUDE FILES                                */
/*****************************************************************************/

/* ---------------------- SYSTEM AND PLATFORM FILES -------------------------*/

/*----------------------------- PROGRAM FILES -------------------------------*/
//#include "alg.h"
//#include "xdas.h"
#include "ih264fhdvenc.h"

/*****************************************************************************/
/*               PUBLIC DECLARATIONS Defined here, used elsewhere            */
/*****************************************************************************/

/*--------------------------- DATA DECLARATIONS -----------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/*
// ===========================================================================
// H264FHDVENC_Handle
//
// This pointer is used to reference all H264FHDVENC instance objects
*/
typedef struct IH264FHDVENC_Obj *H264FHDVENC_Handle;

/*
// ===========================================================================
// H264FHDVENC_Params
//
// This structure defines the creation parameters for all H264FHDVENC objects
*/
typedef IH264FHDVENC_Params H264FHDVENC_Params;

/*
// ===========================================================================
// H264FHDVENC_PARAMS
//
// This structure defines the default creation parameters for H264FHDVENC objects
*/
#define H264FHDVENC_PARAMS   IH264FHDVENC_PARAMS


/*
// ===========================================================================
// H264FHDVENC_DynamicParams
//
// This structure defines the creation parameters for all H264FHDVENC objects
*/
typedef IH264FHDVENC_DynamicParams H264FHDVENC_DynamicParams;

/*
// ===========================================================================
// H264FHDVENC_Status
//
// This structure defines the real-time parameters for H264FHDVENC objects
*/
typedef struct IH264FHDVENC_Status   H264FHDVENC_Status;

/*
// ===========================================================================
// H264FHDVENC_InArgs
//
// This structure defines the real-time input arguments for H264FHDVENC objects
*/
typedef struct IH264FHDVENC_InArgs   H264FHDVENC_InArgs;

/*
// ===========================================================================
// H264FHDVENC_OutArgs
//
// This structure defines input arguments for all H264FHDVENC objects
*/
typedef struct IH264FHDVENC_OutArgs H264FHDVENC_OutArgs;

/*
// ===========================================================================
// H264FHDVENC_Cmd
//
// This typedef defines the control commands H264FHDVENC objects
*/
typedef IH264FHDVENC_Cmd   H264FHDVENC_Cmd;

/*
// ===========================================================================
// control method commands
*/
#define H264FHDVENC_GETSTATUS     IH264FHDVENC_GETSTATUS
#define H264FHDVENC_SETPARAMS     IH264FHDVENC_SETPARAMS
#define H264FHDVENC_RESET         IH264FHDVENC_RESET
#define H264FHDVENC_SETDEFAULT    IH264FHDVENC_SETDEFAULT
#define H264FHDVENC_FLUSH         IH264FHDVENC_FLUSH
#define H264FHDVENC_GETBUFINFO    IH264FHDVENC_GETBUFINFO


/*-------------------------- FUNCTION PROTOTYPES ----------------------------*/

extern Void  H264FHDVENC_init();

/*
// ===========================================================================
// H264FHDVENC_create
//
// Create an H264FHDVENC instance object (using parameters specified by prms)
*/
extern H264FHDVENC_Handle H264FHDVENC_create(
                                   const IH264FHDVENC_Fxns *fxns,
                                   const H264FHDVENC_Params *prms
                                  );

/*
// ===========================================================================
// H264FHDVENC_control
//
// Get, set, and change the parameters of the H264FHDVENC function (using parameters specified by status).
*/
extern Int H264FHDVENC_control(
                          H264FHDVENC_Handle handle,
                          H264FHDVENC_Cmd cmd,
                          H264FHDVENC_DynamicParams *, 
                          H264FHDVENC_Status *status
                         );

/*
// ===========================================================================
// H264FHDVENC_delete
// Delete the H264FHDVENC instance object specified by handle
*/
extern Void H264FHDVENC_delete(H264FHDVENC_Handle handle);


/*
// ===========================================================================
// H264FHDVENC_exit
//
// H264FHDVENC module exit
*/
extern Void  H264FHDVENC_exit();

/*
// ===========================================================================
// H264FHDVENC_decodeFrame
*/
extern Int H264FHDVENC_encode(
                          H264FHDVENC_Handle handle,
                          IVIDEO1_BufDescIn * ,
                          XDM_BufDesc *,
                          H264FHDVENC_InArgs *,
                          H264FHDVENC_OutArgs *
                         );



#ifdef __cplusplus
}
#endif

#endif /* __H264FHDVENC__ */




