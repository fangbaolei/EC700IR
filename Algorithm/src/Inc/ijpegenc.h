/*
 *  ======== ijpegenc.h ========
 *  IJPEGENC Interface Header
 */
#ifndef IJPEGENC_
#define IJPEGENC_

#include <std.h>
#include <xdas.h>
#include <ialg.h>
#include "ijpeg.h"

/*
 *  ======== IJPEGENC_Handle ========
 *  This handle is used to reference all JPEGENC instance objects
 */
typedef struct IJPEGENC_Obj *IJPEGENC_Handle;

/*
 *  ======== IJPEGENC_Obj ========
 *  This structure must be the first field of all JPEGENC instance objects
 */
typedef struct IJPEGENC_Obj {
    struct IJPEGENC_Fxns *fxns;
} IJPEGENC_Obj;

/*
 *  ======== IJPEGENC_Params ========
 *  This structure defines the creation parameters for all JPEGENC objects
 */
typedef struct IJPEGENC_Params {
    Int size;	/* must be first field of all params structures */
    unsigned int  samplePrec;
    unsigned int  numComps;
    unsigned int  numQtables;
    unsigned int  interleaved;
    unsigned int  format;
    unsigned int  quality;
    unsigned int  numLines[3];
    unsigned int  numSamples[3]; 
    unsigned int  pitch[3];   
    unsigned int  outputSize;
} IJPEGENC_Params;

typedef IJPEGENC_Params IJPEGENC_Status;
/*
 *  ======== IJPEGENC_PARAMS ========
 *  Default parameter values for JPEGENC instance objects
 */
extern IJPEGENC_Params IJPEGENC_PARAMS;

/*
 *  ======== IJPEGENC_Fxns ========
 *  This structure defines all of the operations on JPEGENC objects
 */
typedef struct IJPEGENC_Fxns {
    IALG_Fxns	ialg;    /* IJPEGENC extends IALG */
    XDAS_Bool   (*control)(IJPEGENC_Handle handle, IJPEG_Cmd cmd, IJPEGENC_Status *status);
    XDAS_Int32  (*encode)(IJPEGENC_Handle handle, XDAS_Int8** in, XDAS_Int8* out);

} IJPEGENC_Fxns;

#endif	/* IJPEGENC_ */
