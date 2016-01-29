/* ========================================================================== */
/*                                                                            */
/*  TEXAS INSTRUMENTS, INC.                                                   */
/*                                                                            */
/*  NAME                                                                      */
/*     alg_malloc.c                                                           */
/*                                                                            */
/*  DESCRIPTION                                                               */
/*    This module implements an algorithm memory management "policy" in which */
/*  no memory is shared among algorithm objects.  Memory is, however          */
/*  reclaimed when objects are deleted.                                       */
/*                                                                            */
/*  preemption      sharing             object deletion                       */
/*  ----------      -------             ---------------                       */
/*  yes(*)          none                yes                                   */
/*                                                                            */
/*  Note 1: this module uses run-time support functions malloc() and free()   */
/*  to allocate and free memory.  Since these functions are *not* reentrant,  */
/*  either all object creation and deletion must be performed by a single     */
/*  thread or reentrant versions or these functions must be created.          */
/*                                                                            */
/*  COPYRIGHT NOTICES                                                         */
/*   Copyright (C) 1996, MPEG Software Simulation Group. All Rights           */
/*   Reserved.                                                                */
/*                                                                            */
/*   Copyright (c) 2006 Texas Instruments Inc.  All rights reserved.     */
/*   Exclusive property of the Video & Imaging Products, Emerging End         */
/*   Equipment group of Texas Instruments India Limited. Any handling,        */
/*   use, disclosure, reproduction, duplication, transmission, or storage     */
/*   of any part of this work by any means is subject to restrictions and     */
/*   prior written permission set forth in TI's program license agreements    */
/*   and associated software documentation.                                   */
/*                                                                            */
/*   This copyright notice, restricted rights legend, or other proprietary    */
/*   markings must be reproduced without modification in any authorized       */
/*   copies of any part of this work.  Removal or modification of any part    */
/*   of this notice is prohibited.                                            */
/*                                                                            */
/*   U.S. Patent Nos. 5,283,900  5,392,448                                    */
/* -------------------------------------------------------------------------- */
/*            Copyright (c) 2006 Texas Instruments, Incorporated.             */
/*                           All Rights Reserved.                             */
/* ========================================================================== */
/* "@(#) XDAS 2.12 05-21-01 (__imports)" */
//static const char Copyright[] = "Copyright (C) 2003 Texas Instruments "
//                                "Incorporated. All rights Reserved."; 

/* -------------------------------------------------------------------------- */
/* Assigning text section to allow better control on placing function in      */
/* memory of our choice and our alignment. The details about placement of     */
/* these section can be obtained from the linker command file "mpeg2enc.cmd". */
/* -------------------------------------------------------------------------- */



#include <std.h>
#include <alg.h>
#include <ialg.h>

#include <stdlib.h>     /* malloc/free declarations */
#include <string.h>     /* memset declaration */
#include <stdio.h>

#include "mem.h"

extern int intHeap;
extern int extHeap;

// TODO:内存申请调整到片内
__inline void *HV_AllocFastMem2(int iMemSize)
{
	void *pTemp = MEM_alloc(extHeap, iMemSize, 128);
	if (pTemp == MEM_ILLEGAL)
	{
		return NULL;
	}
	return pTemp;
}

// TODO:内存释放调整到片内
__inline void HV_FreeFastMem2(void *pMem, int iMemSize)
{
	MEM_free(extHeap, pMem, iMemSize);
}

Bool _ALG_allocMemory(IALG_MemRec memTab[], Int n);
Void _ALG_freeMemory(IALG_MemRec memTab[], Int n);

/*Specify prototype for TI JPEG decoder   */
#include "jpegdec.h"
#include "jpegdec_ti.h" 

/*
 *  ======== ALG_activate ========
 */
Void ALG_activate(ALG_Handle alg)
{
    /* restore all persistant shared memory */
        ;   /* nothing to do since memory allocation never shares any data */
    
    /* do app specific initialization of scratch memory */
    if (alg->fxns->algActivate != NULL) {
        alg->fxns->algActivate(alg);
    }
}

/*
 *  ======== ALG_deactivate ========
 */
Void ALG_deactivate(ALG_Handle alg)
{
    /* do app specific store of persistent data */
    if (alg->fxns->algDeactivate != NULL) {
        alg->fxns->algDeactivate(alg);
    }

    /* save all persistant shared memory */
        ;   /* nothing to do since memory allocation never shares any data */
    
}

/*
 *  ======== ALG_exit ========
 */
Void ALG_exit(Void)
{
}

/*
 *  ======== ALG_init ========
 */
Void ALG_init(Void)
{
}


/*
 *  ======== _ALG_allocMemory ========
 */

Bool _ALG_allocMemory(IALG_MemRec memTab[], Int n)
{
    Int i;

    for (i = 0; i < n; i++) {
	  memTab[i].base = NULL;
      allocateMemTabRequest( &memTab[i]);
      if (memTab[i].base == NULL) {
		_ALG_freeMemory( memTab, n );
		return (FALSE);
      }
    }
    return (TRUE);
}


/*
 *  ======== _ALG_freeMemory ========
 */
Void _ALG_freeMemory(IALG_MemRec memTab[], Int n)
{
    Int i;
    
    for (i = 0; i < n; i++) {
        if (memTab[i].base != NULL) {
            freeMemTabRequest( &memTab[i]);
        }
    }
}



int allocateMemTabRequest( IALG_MemRec *memTab)
{
	memTab->base = HV_AllocFastMem2( memTab->size );
	if ( memTab->base == NULL )
	{
		printf( "Internal Memory is not enough.\n" );
		return -1;
	}
	memTab->space = IALG_SARAM;
    return 0;
} /* allocateMemTabRequest */



int freeMemTabRequest( IALG_MemRec  *memTab)
{
  	HV_FreeFastMem2( memTab->base, memTab->size );
    memTab->base = NULL;
  	return 0;
} /* freeMemTabRequest */




/* ========================================================================== */
/* End of file : alg_malloc.c                                                 */
/* -------------------------------------------------------------------------- */
/*            Copyright (c) 2006 Texas Instruments, Incorporated.             */
/*                           All Rights Reserved.                             */
/* ========================================================================== */

