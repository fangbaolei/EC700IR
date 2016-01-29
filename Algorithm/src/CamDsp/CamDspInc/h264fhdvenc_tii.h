/*
//============================================================================
//
//    FILE NAME : H264FHDVENC_TII.h
//
//    ALGORITHM : H264FHDVENC
//
//    VENDOR    : TII
//
//    TARGET DSP: C64x
//
//    PURPOSE   : Interface for the H264FHDVENC_TII module; TII's implementation
//                of the IH264FHDVENC interface.
//
//============================================================================
*/

#ifndef H264FHDVENC_TII_
#define H264FHDVENC_TII_

#include "ih264fhdvenc.h"
//#include <ti/xdais/ialg.h>
//#include <idma3.h>
#include "ires.h"
/*
 *  ======== H264FHDVENC_TII__IDMA3 ========
 *  TI's implementation of WMV9Dec's IDMA2 interface
 */
//extern IDMA3_Fxns H264FHDVENC_TII_IDMA3;

/*
 *  ======== H264FHDVENC_TII_IRES ========
 *  TI's implementation of h264VEnc's IRES interface
 */

extern IRES_Fxns H264FHDVENC_TII_IRES;


/*
//============================================================================
// H264FHDVENC_TII_IALG
//
// TII's implementation of the IALG interface for H264FHDVENC
*/
extern IALG_Fxns H264FHDVENC_TII_IALG;

/*
//============================================================================
// H264FHDVENC_TII_IH264FHDVENC
//
// TII's implementation of the IH264FHDVENC interface
*/
extern IH264FHDVENC_Fxns H264FHDVENC_TII_IH264FHDVENC;

/*
//============================================================================
// H264FHDVENC_TII_init
//
// Initialize the H264FHDVENC_TII module as a whole
*/
extern Void H264FHDVENC_TII_init(Void);

/*
//============================================================================
// H264FHDVENC_TII_exit
//
// Exit the H264FHDVENC_TII module as a whole
*/
extern Void H264FHDVENC_TII_exit(Void);


#endif  /* H264FHDVENC_TII_ */
