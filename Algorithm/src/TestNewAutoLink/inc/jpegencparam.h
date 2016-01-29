#ifndef JPEGENCPARAM_H
#define JPEGENCPARAM_H

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

extern ICELL_Fxns JPEGENC_CELLFXNS;
extern far IJPEGENC_Fxns JPEGENC_IJPEGENC;
extern Bool JPEGENC_cellClose( ICELL_Handle handle );
extern Bool JPEGENC_cellExecute( ICELL_Handle handle, Arg arg );
extern Bool JPEGENC_cellOpen( ICELL_Handle handle );
extern Int JPEGENC_cellControl(ICELL_Handle handle, IALG_Cmd cmd, IALG_Status *status );

enum SSCRBUCKETS {
    THRIOSSCRBUCKET,
    SCRBUCKETS               // total number of shared scratch buckets
    };

#ifdef __cplusplus
}
#endif // extern "C" 

#endif

