#include <std.h>
#include <alg.h>
#include <ialg.h>
#include <stdlib.h>

#include <_alg.h>

ALG_Handle ALG_create(IALG_Fxns *fxns, IALG_Handle p, IALG_Params *params)
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

            if (_ALG_allocMemory(memTab, n))
            {
                alg = (IALG_Handle)memTab[0].base;
                alg->fxns = fxns;

                if (fxns->algInit(alg, memTab, p, params) == IALG_EOK)
                {
                    free(memTab);
                    return (alg);
                }

                fxns->algFree(alg, memTab);
                _ALG_freeMemory(memTab, n);
            }

            free(memTab);
        }
    }

    return (NULL);
}

Void ALG_delete(ALG_Handle alg)
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
            _ALG_freeMemory(memTab, n);

            free(memTab);
        }
    }
}
