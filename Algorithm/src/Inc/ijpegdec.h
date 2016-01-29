/*
 *  ======== ijpegdec.h ========
 *  IJPEGDEC Interface Header
 */
#ifndef IJPEGDEC_
#define IJPEGDEC_

#include <xdas.h>
#include <ialg.h>                        
#include "ijpeg.h"


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
typedef struct IJPEGDEC_Params 
{
    Int size;	/* must be first field of all params structures */
    unsigned int pitch[3];
} IJPEGDEC_Params;

/*
 *  ======== IJPEGDEC_Status ========
 *  This structure defines the status parameters for all JPEG_DEC objects
 */
typedef struct IJPEGDEC_Status 
{
    Int size;	/* must be first field of all params structures */
    unsigned int    numLines[3];
    unsigned int    numSamples[3];
    unsigned int    pitch[3];
    unsigned int    grayFLAG;       
    unsigned int    outputSize;    
} IJPEGDEC_Status;

/*
 *  ======== IJPEGDEC_PARAMS ========
 *  Default parameter values for JPEG_DEC instance objects
 */
extern IJPEGDEC_Params IJPEGDEC_PARAMS;

/*
 *  ======== IJPEGDEC_Fxns ========
 *  This structure defines all of the operations on JPEG_DEC objects
 */
typedef struct IJPEGDEC_Fxns {
    IALG_Fxns	ialg;    /* IJPEGDEC extends IALG */
    XDAS_Bool  (*control)(IJPEGDEC_Handle handle, IJPEG_Cmd cmd, IJPEGDEC_Status *status);
    XDAS_Int32 (*decode)(IJPEGDEC_Handle handle, XDAS_Int8 *in, XDAS_Int8 **out);

} IJPEGDEC_Fxns;

#endif	/* IJPEGDEC_ */
