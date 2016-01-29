/*
 *  Copyright 2003 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *  
 */
/* "@(#) ReferenceFrameworks 2.10.00.11 04-30-03 (swat-d15)" */
/*
 *  ======== algrf.h ========
 */
#ifndef ALGRF_
#define ALGRF_

#include <std.h>

#include <ialg.h>

#ifdef __cplusplus
extern "C" {
#endif

 
/* Maximum number of memTab descriptors returned by an algorithm */
#define ALGRF_MAXMEMRECS                16

typedef IALG_Handle ALGRF_Handle;

typedef struct ALGRF_Config {
    Int INTHEAP;
    Int EXTHEAP;
} ALGRF_Config;

extern ALGRF_Config* ALGRF;  /* declared in algrf_setup.c */

/*
 *  ======== ALGRF_memSpace ========
 */
static inline Int ALGRF_memSpace(IALG_MemSpace space)
{
    switch (space) {
       case IALG_DARAM0: 
       case IALG_DARAM1: 
       case IALG_SARAM:    /* IALG_SARAM0 same as IALG_SARAM */
       case IALG_SARAM1:
       case IALG_DARAM2:
       case IALG_SARAM2: {
           return (ALGRF->INTHEAP);
       }
       
       case IALG_ESDATA:
       case IALG_EXTERNAL: {
           return (ALGRF->EXTHEAP);
       }
              
       default: {
           return (ALGRF->EXTHEAP);
       }
    }
}

/* Initialize scratch memory buffers before processing */
extern Void ALGRF_activate(ALGRF_Handle alg);

/* Algorithm specific control and status */ 
extern Int ALGRF_control(ALGRF_Handle alg, IALG_Cmd cmd,
    IALG_Status *statusPtr);  

/*
 *  Algorithm instance creation API for applications not supporting
 *  shared scratch memory buffers
 */
extern ALGRF_Handle ALGRF_create(IALG_Fxns *fxns, IALG_Handle parent,
    IALG_Params *params);

/*
 *  Algorithm instance creation API for applications supporting shared
 *  scratch memory buffers
 */    
extern ALGRF_Handle ALGRF_createScratchSupport(IALG_Fxns *fxns,
    IALG_Handle parent, IALG_Params *params, Void *scratchBuf,
    Uns scratchSize);

/* Save all persistent data to non-scratch memory */    
extern Void ALGRF_deactivate(ALGRF_Handle alg);
  
/*
 *  Algorithm instance deletion API for applications not supporting
 *  shared scratch memory buffers
 */
extern Bool ALGRF_delete(ALGRF_Handle alg);

/*
 *  Algorithm instance deletion API for applications supporting
 *  shared scratch memory buffers
 */
extern Bool ALGRF_deleteScratchSupport(ALGRF_Handle alg);

/* Free all memory allocated to an algorithm */
extern Void ALGRF_memFree(IALG_MemRec memTab[], Int numRecs);

/* Free all memory allocated to an algorithm, except internal scratch */
extern Void ALGRF_memFreeScratchSupport(IALG_MemRec memTab[], Int numRecs);

/* Configure the system to use the selected heaps */
extern Void ALGRF_setup(Int internalHeap, Int externalHeap);

/* ALGRF Module Initialization */
extern Void ALGRF_init(Void);

/* ALGRF Module Finalization */
extern Void ALGRF_exit(Void);


#ifdef __cplusplus
}
#endif /* extern "C" */

#endif  /* ALGRF_ */


