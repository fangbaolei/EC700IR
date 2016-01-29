/*
 *  Copyright 2003 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *  
 */
/* "@(#) ReferenceFrameworks 2.10.00.11 04-30-03 (swat-d15)" */
/*
 *  ======== icc_linear.h ========
 *  
 */ 
#ifndef ICC_LINEAR_
#define ICC_LINEAR_

#include "icc.h"

#ifdef __cplusplus
extern "C" {
#endif    


typedef struct
{
   ICC_Obj  obj;   
} ICC_LinearObj, *ICC_LinearHandle;

/* Creation function */
extern ICC_LinearHandle ICC_linearCreate(Ptr buffer, Uns nmaus);

/* Deletion function */
extern Bool ICC_linearDelete(ICC_LinearHandle linearIccHandle);

#ifdef __cplusplus
}
#endif // extern "C" 

#endif // ICC_LINEAR_

