#include <std.h>
#include <stdlib.h>     /* malloc/free declarations */
#include <string.h>     /* memset declaration */

#include <ialg.h>
typedef IALG_Handle ALG_Handle;

Void _ALG_freeMemory_JpegEnc(IALG_MemRec* memTab, Int n)
{
    Int i;
    for (i = 0; i < n; i++)
    {
        if( memTab[i].space == IALG_EXTERNAL )
        {
            free(memTab[i].base);
        }
    }
}

Bool _ALG_allocMemory_JpegEnc(IALG_MemRec* memTab, Int n)
{
    Int i;
    for (i = 0; i < n; i++)
    {
        if (memTab[i].space == IALG_EXTERNAL)
        {
            memTab[i].base = (void *)malloc(memTab[i].size);
        }

        if (memTab[i].base == NULL)
        {
            _ALG_freeMemory_JpegEnc(memTab, i);
            return (FALSE);
        }
        memset(memTab[i].base, 0, memTab[i].size);
    }
    return (TRUE);
}

//------------------------------------------------------------------------------------

ALG_Handle ALG_create_JpegEnc(IALG_Fxns *fxns, IALG_Handle p, IALG_Params *params)
{
    IALG_MemRec *memTab;
    Int n;
    ALG_Handle alg;
    IALG_Fxns *fxnsPtr;

    if (fxns != NULL)
    {
        n = fxns->algNumAlloc != NULL ? fxns->algNumAlloc() : IALG_DEFMEMRECS;

        if ((memTab = (IALG_MemRec *)malloc(n * sizeof (IALG_MemRec))))
        {
            n = fxns->algAlloc(params, &fxnsPtr, memTab);
            if (n <= 0)
            {
                return (NULL);
            }

            if (_ALG_allocMemory_JpegEnc(memTab, n))
            {
                alg = (IALG_Handle)memTab[0].base;
                alg->fxns = fxns;
                if (fxns->algInit(alg, memTab, p, params) == IALG_EOK)
                {
                    free(memTab);
                    return (alg);
                }
                fxns->algFree(alg, memTab);
                _ALG_freeMemory_JpegEnc(memTab, n);
            }

            free(memTab);
        }
    }

    return (NULL);
}

Void ALG_delete_JpegEnc(ALG_Handle alg)
{
    IALG_MemRec *memTab;
    Int n;
    IALG_Fxns *fxns;

    if (alg != NULL && alg->fxns != NULL)
    {
        fxns = alg->fxns;
        n = fxns->algNumAlloc != NULL ? fxns->algNumAlloc() : IALG_DEFMEMRECS;

        if ((memTab = (IALG_MemRec *)malloc(n * sizeof (IALG_MemRec))))
        {
            memTab[0].base = alg;
            n = fxns->algFree(alg, memTab);
            _ALG_freeMemory_JpegEnc(memTab, n);

            free(memTab);
        }
    }
}
