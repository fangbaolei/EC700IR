/*
 *  Copyright 2002 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *  
 */
/* "@(#) XDAS 2.51.00 02-20-03 (xdas-d20)" */
/*
 *  ======== alg_malloc.c ========
 *  This module implements an algorithm memory management "policy" in which
 *  no memory is shared among algorithm objects.  Memory is, however
 *  reclaimed when objects are deleted.
 *
 *  preemption      sharing             object deletion
 *  ----------      -------             ---------------
 *  yes(*)          none                yes
 *
 *  Note 1: this module uses run-time support functions malloc() and free()
 *  to allocate and free memory.  Since these functions are *not* reentrant,
 *  either all object creation and deletion must be performed by a single
 *  thread or reentrant versions or these functions must be created.
 *
 *  Note 2: Memory alignment is supported for the c5000 targets with the
 *  memalign5000() function which allocates 'size + align' memory.  This
 *  is necessary since the compiler's run-time library does not
 *  support memalign().  This wastes 'align' extra memory.
 *
 */

#ifdef _TMS320C6400
#pragma CODE_SECTION(ALG_init, ".text:init")
#pragma CODE_SECTION(ALG_exit, ".text:exit")
#pragma CODE_SECTION(_ALG_allocMemory, ".text:create")
#pragma CODE_SECTION(_ALG_freeMemory, ".text:create")
#endif /* _TMS320C6400 */

#include <std.h>
#include <alg.h>
#include <ialg.h>

#include <stdlib.h>     /* malloc/free declarations */
#include <string.h>     /* memset declaration */

#if defined (_54_) || (_55_)
void *memalign5000(size_t alignment, size_t size);
void free5000(void *ptr);
#define myMemalign  memalign5000
#define myFree      free5000
#else
#ifndef _TMS320C6400
void *memAlignVc(size_t alignment, size_t size);
#define myMemalign  memAlignVc
#else /* _TMS320C6400 */
#define myMemalign  memalign
#endif /* _TMS320C6400 */
#define myFree      free
#endif


Bool _ALG_allocMemory(IALG_MemRec memTab[], Int n);
Void _ALG_freeMemory(IALG_MemRec memTab[], Int n);
static int freeMemTabRequest( IALG_MemRec  *memTab);
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

int allocateMemTabRequest( IALG_MemRec *memTab);

/*
 *  ======== _ALG_allocMemory ========
 */
Bool _ALG_allocMemory(IALG_MemRec memTab[], Int n)
{
    Int i;
    
    for (i = 0; i < n; i++) {
      /* XXX changing the code here, to change the memory allocator for
       * different requirements. */
      allocateMemTabRequest( &memTab[i]);
     //    memTab[i].base = (void *)myMemalign(memTab[i].alignment, memTab[i].size); 

        if (memTab[i].base == NULL) {
            _ALG_freeMemory(memTab, i);
            return (FALSE);
        }
        memset(memTab[i].base, 0, memTab[i].size);
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
          /* XXX changing code here too. to take care of internal memory
           * allocatiuons */
            freeMemTabRequest( &memTab[i]);
            /* myFree(memTab[i].base); */
        }
    }
}

#if defined (_54_) || (_55_)
/*
 *  ======== memalign5000 ========
 */
void *memalign5000(size_t alignment, size_t size)
{
    void     **mallocPtr;
    void     **retPtr;

    /* return if invalid size value */ 
    if (size <= 0) {
       return (0);
    }

    /*
     * If alignment is not a power of two, return what malloc returns. This is
     * how memalign behaves on the c6x.
     */
    if ((alignment & (alignment - 1)) || (alignment <= 1)) {
        if( (mallocPtr = malloc(size + sizeof(mallocPtr))) != NULL ) {
            *mallocPtr = mallocPtr;
            mallocPtr++;
        }
        return ((void *)mallocPtr);
    }

    /* allocate block of memory */
    if ( !(mallocPtr = malloc(alignment + size)) ) { 
        return (0);
    }

    /* Calculate aligned memory address */ 
    retPtr = (void *)(((Uns)mallocPtr + alignment) & ~(alignment - 1));

    /* Set pointer to be used in the free5000() fxn */
    retPtr[-1] = mallocPtr;

    /* return aligned memory */
    return ((void *)retPtr);
}

/*
 *  ======== free5000 ========
 */
Void free5000(void *ptr)
{
    free((void *)((void **)ptr)[-1]);
}
#endif

void *memAlignVc(size_t alignment, size_t size)
{
  return malloc( size);
} /* memAlignVc */

/* Trying to have an internal heap without BIOS support. */
/* Assumption is that, freeing and allocation in the internal memory wouldn't
 * be discontinuous. That is the allocation and freeing will happen in the same
 * orders. */

#define INTERNAL_DATA_MEM_SIZE    (0x8000)

#ifdef _TMS320C6400
#pragma DATA_SECTION( internalDataMemory, ".intDataMem");
#endif /* #ifdef _TMS320C6400 */

unsigned char internalDataMemory[ INTERNAL_DATA_MEM_SIZE];
unsigned char *pInternalDataMemory = internalDataMemory;
unsigned int  internalDataMemorySize = INTERNAL_DATA_MEM_SIZE;

int allocateMemTabRequest( IALG_MemRec *memTab)
{
  if( memTab->space == IALG_EXTERNAL ) {
    /* external memory request >-> do the normal way */
    memTab->base = (void *)myMemalign(memTab->alignment, memTab->size);
  } else {
    /* internal memory request */
    unsigned int  alignBytes;
    alignBytes = (((unsigned int) pInternalDataMemory + (memTab->alignment - 1)) & ~ (memTab->alignment - 1));
    alignBytes -= (unsigned int) pInternalDataMemory;
    pInternalDataMemory += alignBytes;
    internalDataMemorySize -= alignBytes;
    if(internalDataMemorySize >= memTab->size) {
      /* allocate memory */
      memTab->base = pInternalDataMemory;
      pInternalDataMemory += memTab->size;
      internalDataMemorySize -= memTab->size;
    } else {
      memTab->base = 0;
    }
  }
  return 0;
} /* allocateMemTabRequest */

static int freeMemTabRequest( IALG_MemRec  *memTab)
{
  if( memTab->space == IALG_EXTERNAL ) {
    /* external memory request >-> do the normal way */
    myFree( memTab->base);
  } else {
    /* internal memory  free request. XXX see the code  below 
     * for the dangers of calling them as normal mallocs. Free is faked!!!  */
    memTab->base = 0;
    pInternalDataMemory = internalDataMemory;
    internalDataMemorySize = INTERNAL_DATA_MEM_SIZE;
  }
  return 0;
} /* freeMemTabRequest */

