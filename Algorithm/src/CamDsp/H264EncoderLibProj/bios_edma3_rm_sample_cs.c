#include <edma3_rm.h>
#include "bios_edma3_rm_sample.h"

int NON_BIOS_SEMAPHORE = 6467;

void edma3OsProtectEntry(int level, unsigned int *intState)
{
}

void edma3OsProtectExit (int level, unsigned int intState)
{
}

EDMA3_RM_Result edma3OsSemCreate(
    int initVal,
    const EDMA3_OS_SemAttrs *attrs,
    EDMA3_OS_Sem_Handle *hSem)
{
    EDMA3_RM_Result semCreateResult = EDMA3_RM_SOK;
    *hSem = (EDMA3_OS_Sem_Handle)(&NON_BIOS_SEMAPHORE);
    return semCreateResult;
}

EDMA3_RM_Result edma3OsSemDelete(EDMA3_OS_Sem_Handle hSem)
{
    EDMA3_RM_Result semDeleteResult = EDMA3_RM_SOK;
    return semDeleteResult;
}

EDMA3_RM_Result edma3OsSemTake(EDMA3_OS_Sem_Handle hSem, int mSecTimeout)
{
    EDMA3_RM_Result semTakeResult = EDMA3_RM_SOK;
    return semTakeResult;
}

EDMA3_RM_Result edma3OsSemGive(EDMA3_OS_Sem_Handle hSem)
{
    EDMA3_RM_Result semGiveResult = EDMA3_RM_SOK;
    return semGiveResult;
}
