/* 
 * Copyright (c) 2009, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */

#ifndef ti_xdais_dm_IIMGENC1_
#define ti_xdais_dm_IIMGENC1_

#include <ialg.h>
#include <xdas.h>
#include "xdm.h"

#ifdef __cplusplus
extern "C" {
#endif



#define IIMGENC1_EOK        XDM_EOK            
#define IIMGENC1_EFAIL      XDM_EFAIL          
#define IIMGENC1_EUNSUPPORTED XDM_EUNSUPPORTED 
typedef struct IIMGENC1_Obj {
    struct IIMGENC1_Fxns *fxns;
} IIMGENC1_Obj;


typedef struct IIMGENC1_Obj  *IIMGENC1_Handle;


typedef struct IIMGENC1_Params {
    XDAS_Int32 size;            
    XDAS_Int32 maxHeight;       
    XDAS_Int32 maxWidth;        
    XDAS_Int32 maxScans;        
    XDAS_Int32 dataEndianness;  
    XDAS_Int32 forceChromaFormat;
} IIMGENC1_Params;


typedef struct IIMGENC1_DynamicParams {
    XDAS_Int32 size;            
    XDAS_Int32 numAU;           
    XDAS_Int32 inputChromaFormat;
    XDAS_Int32 inputHeight;     
    XDAS_Int32 inputWidth;      
    XDAS_Int32 captureWidth;    
    XDAS_Int32 generateHeader;  
    XDAS_Int32 qValue;          
} IIMGENC1_DynamicParams;


typedef struct IIMGENC1_InArgs {
    XDAS_Int32 size;            
} IIMGENC1_InArgs;


typedef struct IIMGENC1_Status {
    XDAS_Int32 size;            
    XDAS_Int32 extendedError;   
    XDM1_SingleBufDesc data;    
    XDAS_Int32 totalAU;         
    XDM_AlgBufInfo bufInfo;     
} IIMGENC1_Status;


typedef struct IIMGENC1_OutArgs {
    XDAS_Int32 size;            
    XDAS_Int32 extendedError;   
    XDAS_Int32 bytesGenerated;  
    XDAS_Int32 currentAU;       
} IIMGENC1_OutArgs;


typedef  IALG_Cmd IIMGENC1_Cmd;


typedef struct IIMGENC1_Fxns{
    IALG_Fxns   ialg;             
    XDAS_Int32  (*process)(IIMGENC1_Handle handle, XDM1_BufDesc *inBufs,
        XDM1_BufDesc *outBufs, IIMGENC1_InArgs *inArgs,
        IIMGENC1_OutArgs *outArgs);


    XDAS_Int32  (*control)(IIMGENC1_Handle handle, IIMGENC1_Cmd id,
        IIMGENC1_DynamicParams *params, IIMGENC1_Status *status);

} IIMGENC1_Fxns;


#ifdef __cplusplus
}
#endif

#endif
/*
 *  @(#) ti.xdais.dm; 1, 0, 5,238; 12-1-2009 06:46:16; /db/wtree/library/trees/dais/dais-q08x/src/
 */
