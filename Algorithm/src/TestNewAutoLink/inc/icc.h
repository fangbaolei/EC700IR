/*
 *  Copyright 2003 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *  
 */
/* "@(#) ReferenceFrameworks 2.10.00.11 04-30-03 (swat-d15)" */
/*
 *  ======== icc.h ========
 *  Inter-cell communication module header file
 */ 
#ifndef ICC_
#define ICC_

#ifdef __cplusplus
extern "C" {
#endif    

/*
 *  In the future, ICC might be expanded to support other types of 
 *  mechanisms beside a raw buffer.  For example: circular queue or
 *  SIO.  The following enum and defines are here to perserve an 
 *  easy migration path.
 */
#define ICC_USERSTART  32

typedef enum ICC_ObjType
{
   ICC_NULLOBJ,
   ICC_LINEAROBJ,   
   ICC_USEROBJ=ICC_USERSTART,
   ICC_MAXTYPES
} ICC_ObjType;

typedef struct ICC_Obj *ICC_Handle;

typedef struct ICC_Obj
{  
  Ptr            buffer;         // Pointer to the buffer
  Uns            nmaus;          // Size of the buffer  
  ICC_ObjType    objType;        // Type of ICC  
} ICC_Obj;

/* Inline to get the buffer and the size of the buffer */
static inline Void ICC_getBuf(ICC_Handle iccHandle, Ptr *buffer, Uns *nmaus)
{    
    *buffer = iccHandle->buffer;
    *nmaus  = iccHandle->nmaus;
}

/* Inline to set the buffer and the size of the buffer */
static inline Void ICC_setBuf(ICC_Handle iccHandle, Ptr buffer, Uns nmaus)
{
    iccHandle->buffer = buffer;
    iccHandle->nmaus  = nmaus;
}

/* exit function for ICC module */
extern Void ICC_exit(Void);

/* init function for ICC module */
extern Void ICC_init(Void);

#ifdef __cplusplus
}
#endif // extern "C" 

#endif // ICC_

