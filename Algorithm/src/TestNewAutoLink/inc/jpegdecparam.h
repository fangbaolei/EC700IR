#ifndef JPEGDECPARAM_H
#define JPEGDECPARAM_H

#include <std.h>

#include "chan.h"
#include "icc.h"
#include "icell.h"
#include "ijpeg.h"
#include "ijpegdec.h"
#include "ijpegenc.h"

#ifdef __cplusplus
extern "C" {
#endif

extern ICELL_Fxns JPEGDEC_CELLFXNS;
extern far IJPEGDEC_Fxns JPEGDEC_IJPEGDEC;

extern Bool JPEGDEC_cellClose( ICELL_Handle handle );
extern Bool JPEGDEC_cellExecute( ICELL_Handle handle, Arg arg );
extern Bool JPEGDEC_cellOpen( ICELL_Handle handle );

typedef struct JPEGDEC_Env {
    Char * butterflyIntBuf;
    Char * procIntBuf;
    Char * intYBuf;
    Char * intCrBuf;
    Char * intCbBuf;
} JPEGDEC_Env;

#ifdef __cplusplus
}
#endif // extern "C" 

#endif

