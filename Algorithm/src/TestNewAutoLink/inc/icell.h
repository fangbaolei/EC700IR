/*
 *  Copyright 2003 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *  
 */
/* "@(#) ReferenceFrameworks 2.10.00.11 04-30-03 (swat-d15)" */
/*
 *  ======== icell.h ========
 *  This file defines the cell interface. A cell is an encapsulation 
 *  of an algorithm.  It allows a standard interface to the algorithms 
 *  runtime execution functions (e.g. apply(), decode(), etc.).
 */ 
#ifndef _ICELL_
#define _ICELL_

#include <ialg.h>
#include "icc.h"

#ifdef __cplusplus
extern "C" {
#endif    

typedef struct ICELL_Obj *ICELL_Handle;

/*
 *  ICELL_Fxns
 *  ----------
 *  This structure defines the functions that must be supplied for 
 *  all cells.
 *
 *      cellClose()   - Used to clean-up or de-allocate resources used by
 *                      the cell (e.g. DMA resources, statistics, etc.). 
 *                      This function does not free the algorithm. This 
 *                      function should be called before the algorithm is
 *                      freed. This is an optional function.
 *
 *      cellControl() - Used to call the control functions of the
 *                      algorithm. This is an optional function.
 * 
 *      cellExecute() - Used to call the runtime execution functions of the
 *                      algorithm. This is an required function.
 *
 *      cellOpen()    - Used to initialize or allocate resources used by
 *                      the cell (e.g. DMA resources, statistics, etc.). 
 *                      This function does not allocate the algorithm. This 
 *                      function should be called after the algorithm is 
 *                      allocated. This is an optional function.
 */
typedef struct ICELL_Fxns {   
   Bool (*cellClose  )(ICELL_Handle handle);
   Int  (*cellControl)(ICELL_Handle handle, IALG_Cmd cmd, IALG_Status *status);
   Bool (*cellExecute)(ICELL_Handle handle, Arg arg);
   Bool (*cellOpen   )(ICELL_Handle handle);
} ICELL_Fxns;


/*  
 *  ICELL_Obj
 *  ---------
 *  This structure is used to define a cell.
 */
typedef struct ICELL_Obj {
    Int          size;           /* Number of MAU in the structure        */    
    String       name;           /* User chosen name.                     */
    ICELL_Fxns  *cellFxns;       /* Ptr to cell v-table function.         */
    Ptr          cellEnv;        /* Ptr to user defined cell env. struct  */
    IALG_Fxns   *algFxns;        /* Ptr to alg v-table functions.         */
    IALG_Params *algParams;      /* Ptr to alg parameters.                */
    IALG_Handle  algHandle;      /* Handle of alg managed by cell.        */
    Uns          scrBucketIndex; /* Scratch bucket for XDAIS scratch mem. */
    ICC_Handle  *inputIcc;       /* Array of input ICC objects            */
    Uns          inputIccCnt;    /* # of ICC objects in the input array   */
    ICC_Handle  *outputIcc;      /* Array of output ICC objects           */
    Uns          outputIccCnt;   /* # of ICC objects in the output array  */
} ICELL_Obj;

#define ICELL_DEFAULT {                                                      \
    sizeof(ICELL_Obj),           /* Number of MAU in the structure        */ \
    "",                          /* User chosen name.                     */ \
    NULL,                        /* Ptr to cell v-table function.         */ \
    NULL,                        /* Ptr to user defined cell env. struct  */ \
    NULL,                        /* Ptr to alg v-table functions.         */ \
    NULL,                        /* Ptr to alg parameters.                */ \
    NULL,                        /* Handle of alg managed by cell.        */ \
    0,                           /* Scratch bucket for XDAIS scratch mem. */ \
    NULL,                        /* Array of input ICC objects            */ \
    0,                           /* # of ICC objects in the input array   */ \
    NULL,                        /* Array of output ICC objects           */ \
    0                            /* # of ICC objects in the output array  */ \
}

    


#ifdef __cplusplus
}
#endif // extern "C" 

#endif // _ICELL_

