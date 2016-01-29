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

#ifndef ti_xdais_dm_XDM_
#define ti_xdais_dm_XDM_

#include <ialg.h>
#include <xdas.h>

#ifdef __cplusplus
extern "C" {
#endif


#define XDM_EOK                 IALG_EOK    
#define XDM_EFAIL               IALG_EFAIL  
#define XDM_EUNSUPPORTED        -3          
#ifdef XDM_INCLUDE_DOT9_SUPPORT

#define XDM_ERUNTIME            -2
#endif

#define XDM_MAX_IO_BUFFERS      16          
typedef struct XDM_BufDesc {
    XDAS_Int8   **bufs;     
    XDAS_Int32   numBufs;   
    XDAS_Int32  *bufSizes;  
} XDM_BufDesc;


typedef struct XDM_SingleBufDesc {
    XDAS_Int8   *buf;       
    XDAS_Int32  bufSize;    
} XDM_SingleBufDesc;



typedef struct XDM1_SingleBufDesc {
    XDAS_Int8   *buf;       
    XDAS_Int32  bufSize;    
    XDAS_Int32  accessMask; 
} XDM1_SingleBufDesc;


typedef struct XDM1_BufDesc {
    XDAS_Int32   numBufs;   
    XDM1_SingleBufDesc descs[XDM_MAX_IO_BUFFERS]; 
} XDM1_BufDesc;


typedef enum {
    XDM_ACCESSMODE_READ = 0,      
    XDM_ACCESSMODE_WRITE = 1      
} XDM_AccessMode;


#define XDM_ISACCESSMODE_READ(x)    (((x) >> XDM_ACCESSMODE_READ) & 0x1)


#define XDM_ISACCESSMODE_WRITE(x)   (((x) >> XDM_ACCESSMODE_WRITE) & 0x1)


#define XDM_CLEARACCESSMODE_READ(x)   ((x) &= (~(0x1 << XDM_ACCESSMODE_READ)))


#define XDM_CLEARACCESSMODE_WRITE(x)   ((x) &= (~(0x1 << XDM_ACCESSMODE_WRITE)))


#define XDM_SETACCESSMODE_READ(x)   ((x) |= (0x1 << XDM_ACCESSMODE_READ))


#define XDM_SETACCESSMODE_WRITE(x)  ((x) |= (0x1 << XDM_ACCESSMODE_WRITE))


typedef struct XDM_AlgBufInfo {
    XDAS_Int32 minNumInBufs;       
    XDAS_Int32 minNumOutBufs;      
    XDAS_Int32 minInBufSize[XDM_MAX_IO_BUFFERS];  
    XDAS_Int32 minOutBufSize[XDM_MAX_IO_BUFFERS]; 
} XDM_AlgBufInfo;


#define XDM_CUSTOMENUMBASE 0x100


#define XDM_CUSTOMCMDBASE 0x100

typedef enum {
    XDM_GETSTATUS = 0,      
    XDM_SETPARAMS = 1,      
    XDM_RESET = 2,          
    XDM_SETDEFAULT = 3,     
    XDM_FLUSH = 4,          
    XDM_GETBUFINFO = 5,     
    XDM_GETVERSION = 6,     
    XDM_GETCONTEXTINFO = 7  
} XDM_CmdId;


typedef enum {
    XDM_PARAMSCHANGE = 8,       
    XDM_APPLIEDCONCEALMENT = 9,  
    XDM_INSUFFICIENTDATA = 10,   
    XDM_CORRUPTEDDATA = 11,      
    XDM_CORRUPTEDHEADER = 12,    
    XDM_UNSUPPORTEDINPUT = 13,   
    XDM_UNSUPPORTEDPARAM = 14,   
    XDM_FATALERROR = 15          
} XDM_ErrorBit;

#define XDM_ISFATALERROR(x)         (((x) >> XDM_FATALERROR) & 0x1)

#define XDM_ISUNSUPPORTEDPARAM(x)   (((x) >> XDM_UNSUPPORTEDPARAM) & 0x1)

#define XDM_ISUNSUPPORTEDINPUT(x)   (((x) >> XDM_UNSUPPORTEDINPUT) & 0x1)

#define XDM_ISCORRUPTEDHEADER(x)    (((x) >> XDM_CORRUPTEDHEADER) & 0x1)

#define XDM_ISCORRUPTEDDATA(x)      (((x) >> XDM_CORRUPTEDDATA) & 0x1)

#define XDM_ISINSUFFICIENTDATA(x)   (((x) >> XDM_INSUFFICIENTDATA) & 0x1)

#define XDM_ISAPPLIEDCONCEALMENT(x) (((x) >> XDM_APPLIEDCONCEALMENT) & 0x1)

#define XDM_SETFATALERROR(x)         ((x) |= (0x1 << XDM_FATALERROR))

#define XDM_SETUNSUPPORTEDPARAM(x)   ((x) |= (0x1 << XDM_UNSUPPORTEDPARAM))

#define XDM_SETUNSUPPORTEDINPUT(x)   ((x) |= (0x1 << XDM_UNSUPPORTEDINPUT))

#define XDM_SETCORRUPTEDHEADER(x)    ((x) |= (0x1 << XDM_CORRUPTEDHEADER))

#define XDM_SETCORRUPTEDDATA(x)      ((x) |= (0x1 << XDM_CORRUPTEDDATA))

#define XDM_SETINSUFFICIENTDATA(x)   ((x) |= (0x1 << XDM_INSUFFICIENTDATA))

#define XDM_SETAPPLIEDCONCEALMENT(x) ((x) |= (0x1 << XDM_APPLIEDCONCEALMENT))


typedef enum {
    XDM_BYTE = 1,           
    XDM_LE_16 = 2,          
    XDM_LE_32 = 3,          
    XDM_LE_64 = 4,          
    XDM_BE_16 = 5,          
    XDM_BE_32 = 6,          
    XDM_BE_64 = 7           
} XDM_DataFormat;


typedef struct XDM_Date {
    XDAS_Int32 msecsOfDay;   
    XDAS_Int32 month;        
    XDAS_Int32 dayOfMonth;   
    XDAS_Int32 dayOfWeek;    
    XDAS_Int32 year;         
} XDM_Date;


typedef struct XDM_Point {
    XDAS_Int32 x;
    XDAS_Int32 y;
} XDM_Point;


typedef struct XDM_Rect {
    XDM_Point topLeft;
    XDM_Point bottomRight;
} XDM_Rect;

#define XDM_MAX_CONTEXT_BUFFERS   32


typedef struct XDM_ContextInfo {
    XDAS_Int32 minContextSize;     
    XDAS_Int32 minIntermediateBufSizes[XDM_MAX_CONTEXT_BUFFERS];  
} XDM_ContextInfo;


typedef struct XDM_Context {
    XDM1_SingleBufDesc algContext;  
    XDAS_Int32 numInBufs;           
    XDAS_Int32 numOutBufs;          
    XDAS_Int32 numInOutBufs;        
    XDM1_SingleBufDesc inBufs[XDM_MAX_CONTEXT_BUFFERS];  
    XDM1_SingleBufDesc outBufs[XDM_MAX_CONTEXT_BUFFERS]; 
    XDM1_SingleBufDesc inOutBufs[XDM_MAX_CONTEXT_BUFFERS]; 
    XDM1_SingleBufDesc intermediateBufs[XDM_MAX_CONTEXT_BUFFERS]; 
} XDM_Context;


typedef enum {
    XDM_DEFAULT = 0,        
    XDM_HIGH_QUALITY = 1,   
    XDM_HIGH_SPEED = 2,     
    XDM_USER_DEFINED = 3    
} XDM_EncodingPreset;


typedef enum {
    XDM_DECODE_AU = 0,      
    XDM_PARSE_HEADER = 1    
} XDM_DecMode;


typedef enum {
    XDM_ENCODE_AU = 0,      
    XDM_GENERATE_HEADER = 1 
} XDM_EncMode;


typedef enum {
    XDM_CHROMA_NA = -1,     
    XDM_YUV_420P = 1,       
    XDM_YUV_422P = 2,       
    XDM_YUV_422IBE = 3,     
    XDM_YUV_422ILE = 4,     
    XDM_YUV_444P = 5,       
    XDM_YUV_411P = 6,       
    XDM_GRAY = 7,           
    XDM_RGB = 8,            
    XDM_YUV_420SP = 9,      
    XDM_ARGB8888 = 10,      
    XDM_RGB555 = 11,        
    XDM_RGB565 = 12,        
    XDM_YUV_444ILE = 13,    
    XDM_CHROMAFORMAT_DEFAULT = XDM_YUV_422ILE
} XDM_ChromaFormat;


#ifdef __cplusplus
}
#endif

#endif  /* ti_xdais_dm_XDM_ */
/*
 *  @(#) ti.xdais.dm; 1, 0, 5,238; 12-1-2009 06:46:17; /db/wtree/library/trees/dais/dais-q08x/src/
 */
