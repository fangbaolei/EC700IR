/*
 *  Copyright 2003 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *  
 */
/* "@(#) ReferenceFrameworks 2.10.00.11 04-30-03 (swat-d15)" */
/*
 *  ======== chan.h ========
 *  This file is the main include file for the CHAN module.
 *
 *  The following is the calling sequence of the CHAN APIs:
 *
 *                                                -----------
 *                                                |         |
 *                                                v         |
 *  --------------     --------------     ----------------  |
 *  | CHAN_init  | --> | CHAN_setup | --> | CHAN_regCell |---
 *  --------------     --------------     ----------------
 *                                                |
 *                                                |  ----------
 *                                                |  |        |
 *                                                v  v        |
 *                                        -----------------   |
 *                                        | CHAN_create@  |----
 *                                        | CHAN_open#    |
 *                                        | CHAN_getAttrs |
 *                                        | CHAN_setAttrs | 
 *                                        | CHAN_execute  |
 *                                        | CHAN_close#   |
 *                                        | CHAN_delete@  |
 *                                        -----------------   
 *                                                |  
 *                                                |   --------   
 *                                                |   |      |
 *                                                v   v      |
 *                     --------------     ----------------   |
 *                     | CHAN_exit  | <-- |CHAN_unregCell|----
 *                     --------------     ----------------
 * Additional notes
 *   # = must have opened a channel before you can close it. 
 *   @ = must have created a channel before you can delete it. Create/delete
 *       are not required if channel statically defined.
 *
 */
#ifndef CHAN_
#define CHAN_

#include <std.h>

#include "icell.h"
#include "icc.h"

#ifdef __cplusplus
extern "C" {
#endif

/* States for a channel */
typedef enum CHAN_State { 
    CHAN_ACTIVE,
    CHAN_INACTIVE
} CHAN_State;

typedef struct CHAN_Obj *CHAN_Handle;

/* 
 *  CHAN_Obj
 *  --------
 *  cellSet: Set of cells in the channel.  
 *  cellCnt: number of cells in the cellSet. Must be at least 1.
 *  state: state of the channel.
 *  chanControlCB: Callback called within CHAN_execute. Can be NULL.
 */
typedef struct CHAN_Obj {
    ICELL_Obj  *cellSet;
    Uns         cellCnt;    
    CHAN_State  state;
    Bool        (*chanControlCB)(CHAN_Handle chanHandle);
} CHAN_Obj;

/* 
 *  CHAN_Attrs
 *  ----------
 *  state: state of the channel.
 *  chanControlCB: Callback called within CHAN_execute. Can be NULL.
 */
typedef struct CHAN_Attrs {    
    CHAN_State  state;
    Bool       (*chanControlCB)(CHAN_Handle chanHandle);
} CHAN_Attrs;

extern CHAN_Attrs CHAN_ATTRS;     /* default attributes, defined in chan.c */

/* Release the resources owned by the channel.  Does not free the channel. */
extern Bool CHAN_close(CHAN_Handle chanHandle);

/* Create the channel */
extern CHAN_Handle CHAN_create(Void);

/* Delete the channel */
extern Bool CHAN_delete(CHAN_Handle chanHandle);

/* The function that actually executes all the algorithms. */
extern Bool CHAN_execute(CHAN_Handle chanHandle, Arg arg);

/* Exit the CHAN module. */
extern Void CHAN_exit(Void);

/* Get the state of a channel */
extern Void CHAN_getAttrs(CHAN_Handle chanHandle, CHAN_Attrs *attrs);

/* Init the CHAN module. */
extern Void CHAN_init(Void);

/* Create the algorithms */
extern Bool CHAN_open(CHAN_Handle chanHandle, ICELL_Obj cellSet[], 
                      Uns cellCnt, CHAN_Attrs *chanAttrs);

/* Determine worst-case scratch requirements and assign ICCs */
extern Bool CHAN_regCell(ICELL_Handle cellHandle,
                         ICC_Handle iccIn[], Uns iccInCnt,
                         ICC_Handle iccOut[], Uns iccOutCnt);

/* Set the state of a channel */
extern Void CHAN_setAttrs(CHAN_Handle chanHandle, CHAN_Attrs *attrs); 

/* 
 *  Set-up the CHAN module. 
 *  algrfInternalHeap - heap from which algrf allocates internal non-scratch
 *                      memory requested by XDAIS algorithms.
 *  algrfExternalHeap - heap from which algrf allocates external memory
 *                      requested by XDAIS algorithms.
 *  sscrInternalHeap -  heap from which sscr allocates internal scratch
 *                      memory requested by XDAIS algorithms. 
 */
extern Bool CHAN_setup(Int algrfInternalHeap, Int algrfExternalHeap,
                       Int sscrInternalHeap, Uns bucketCnt, Ptr bucketBuf[],
                       Uns bucketSize[]);

/* release memory allocated for ICC arrays */
extern Void CHAN_unregCell(ICELL_Handle cellHandle);

#ifdef __cplusplus
}
#endif // extern "C" 

#endif // CHAN_


