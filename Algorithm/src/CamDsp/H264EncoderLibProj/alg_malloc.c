#include "tistdtypes.h"
#include <std.h>
#include <alg.h>
#include <ialg.h>

#include <stdlib.h>     /* malloc/free declarations */
#include <string.h>     /* memset declaration */

void *memAlignVc(size_t alignment, size_t size);
#define myMemalign  memAlignVc
#define myFree      free

Bool _ALG_allocMemory(IALG_MemRec memTab[], Int n);
Void _ALG_freeMemory(IALG_MemRec memTab[], Int n);
static int freeMemTabRequest(IALG_MemRec  *memTab);

Void ALG_activate(ALG_Handle alg)
{
    if (alg->fxns->algActivate != NULL)
    {
        alg->fxns->algActivate(alg);
    }
}

Void ALG_deactivate(ALG_Handle alg)
{
    if (alg->fxns->algDeactivate != NULL)
    {
        alg->fxns->algDeactivate(alg);
    }
}

Void ALG_exit(Void)
{
}

Void ALG_init(Void)
{
}

int allocateMemTabRequest(IALG_MemRec *memTab);

Bool _ALG_allocMemory(IALG_MemRec memTab[], Int n)
{
    Int i;

    for (i = 0; i < n; i++)
    {
        /* XXX changing the code here, to change the memory allocator for
         * different requirements. */
        allocateMemTabRequest(&memTab[i]);
        if (memTab[i].base == NULL)
        {
            _ALG_freeMemory(memTab, i);
            return (FALSE);
        }
        memset(memTab[i].base, 0, memTab[i].size);
    }

    return (TRUE);
}

Void _ALG_freeMemory(IALG_MemRec memTab[], Int n)
{
    Int i;

    for (i = 0; i < n; i++)
    {
        if (memTab[i].base != NULL)
        {
            /* XXX changing code here too. to take care of internal memory
             * allocatiuons */
            freeMemTabRequest(&memTab[i]);
            /* myFree(memTab[i].base); */
        }
    }
}

void *memAlignVc(size_t alignment, size_t size)
{
    return malloc(size);
} /* memAlignVc */

/* Trying to have an internal heap without BIOS support. */
/* Assumption is that, freeing and allocation in the internal memory wouldn't
 * be discontinuous. That is the allocation and freeing will happen in the same
 * orders. */

#define INTERNAL_DATA_MEM_SIZE (64*1024)

#ifdef _TMS320C6400
#pragma DATA_SECTION(internalDataMemory, ".intDataMem");
#endif /* #ifdef _TMS320C6400 */

unsigned char  internalDataMemory[INTERNAL_DATA_MEM_SIZE];
unsigned char* pInternalDataMemory    = internalDataMemory;
unsigned int   internalDataMemorySize = INTERNAL_DATA_MEM_SIZE;

void InitH264OnChipRAM()
{
	pInternalDataMemory    = internalDataMemory;
	internalDataMemorySize = INTERNAL_DATA_MEM_SIZE;
}

int allocateMemTabRequest(IALG_MemRec *memTab)
{
    if( memTab->space == IALG_EXTERNAL )
    {
        /* external memory request >-> do the normal way */
        memTab->base = (void *)myMemalign(memTab->alignment, memTab->size);
    }
    else
    {
        /* internal memory request */
        unsigned int  alignBytes;
        alignBytes =(((unsigned int) pInternalDataMemory +(memTab->alignment - 1))\
                     & ~ (memTab->alignment - 1));
        alignBytes -= (unsigned int) pInternalDataMemory;
        pInternalDataMemory += alignBytes;
        internalDataMemorySize -= alignBytes;
        if(internalDataMemorySize >= memTab->size)
        {
            /* allocate memory */
            memTab->base = pInternalDataMemory;
            pInternalDataMemory += memTab->size;
            internalDataMemorySize -= memTab->size;
        }
        else
        {
            memTab->base = 0;
        }
    }
    return 0;
} /* allocateMemTabRequest */

static int freeMemTabRequest(IALG_MemRec* memTab)
{
    if ( memTab->space == IALG_EXTERNAL )
    {
        /* external memory request >-> do the normal way */
        myFree(memTab->base);
    }
    else
    {
        /* internal memory  free request. XXX see the code  below
         * for the dangers of calling them as normal mallocs. Free is faked!!!  */
        memTab->base = 0;
        pInternalDataMemory = internalDataMemory;
        internalDataMemorySize = INTERNAL_DATA_MEM_SIZE;
    }

    return 0;
} /* freeMemTabRequest */
