/*****************************************************************************/
/*                              INCLUDE FILES                                */
/*****************************************************************************/

/* ---------------------- SYSTEM AND PLATFORM FILES -------------------------*/
#include "stdio.h"
#include "tistdtypes.h"
#include <std.h>
#include <alg.h>
#include <xdas.h>

/*----------------------------- PROGRAM FILES -------------------------------*/
#include <xdm.h>
#include "ih264fhdvenc.h"
#include "h264fhdvenc_tii.h"
#include "h264venc.h"
#include <ires.h>

extern int H264FHDVENC_TII_scratchId;

H264FHDVENC_Handle H264FHDVENC_create(
    const IH264FHDVENC_Fxns *fxns,
    const H264FHDVENC_Params *prms
)
{
    return (
               (H264FHDVENC_Handle)ALG_create(
                   (IALG_Fxns *)fxns,
                   NULL,
                   (IALG_Params *)prms
               )
           );
}

Int H264FHDVENC_control(
    H264FHDVENC_Handle handle,
    H264FHDVENC_Cmd cmd,
    H264FHDVENC_DynamicParams *params,
    H264FHDVENC_Status *status
)
{
    Int error;

    if(handle == NULL)
    {
        return (IALG_EFAIL);
    }

    ALG_activate((IALG_Handle)handle);
    error = handle->fxns->ividenc.control(
                (IVIDENC1_Obj *)handle,
                cmd,
                (IVIDENC1_DynamicParams *)params,
                (IVIDENC1_Status *)status
            );
    ALG_deactivate((IALG_Handle)handle);

    return(error);
}

Void H264FHDVENC_delete(H264FHDVENC_Handle handle)
{
    ALG_delete((IALG_Handle)handle);
}

Void  H264FHDVENC_init(Void)
{
}

Void  H264FHDVENC_exit(Void)
{
}

Int H264FHDVENC_encode(
    H264FHDVENC_Handle handle,
    IVIDEO1_BufDescIn *Input,
    XDM_BufDesc *Output,
    H264FHDVENC_InArgs *inargs,
    H264FHDVENC_OutArgs *outarg
)
{
    XDAS_Int32 result=0;

    if(handle == NULL)
    {
        return (IALG_EFAIL);
    }

    ALG_activate((IALG_Handle)handle);
    RMAN_activateAllResources((IALG_Handle)handle, &H264FHDVENC_TII_IRES, H264FHDVENC_TII_scratchId);
    result = handle->fxns->ividenc.process(
                 (IVIDENC1_Handle)handle,
                 Input,
                 Output,
                 (IVIDENC1_InArgs *)inargs,
                 (IVIDENC1_OutArgs *)outarg
             );
    RMAN_deactivateAllResources((IALG_Handle)handle, &H264FHDVENC_TII_IRES, H264FHDVENC_TII_scratchId);
    ALG_deactivate((IALG_Handle)handle);

    return(result);
}
