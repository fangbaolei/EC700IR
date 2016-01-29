
#include <csl_sysData.h>

#pragma CODE_SECTION (CSL_sysInit, ".text:csl_section:init");

void
    CSL_sysInit(
        void
)
{
/*
   this is what should have been done for dynamic initialization
   of CSL_sysDataObj; in this case, it has been statically
   initialized with the required default values.

    register int i;

    CSL_sysDataHandle->altRouteHandler = NULL;
    CSL_sysDataHandle->p2vHandler = NULL;
    CSL_sysDataHandle->v2pHandler = NULL;
    CSL_sysDataHandle->versionId = CSL_VERSION_ID;
    CSL_sysDataHandle->chipId = CSL_CHIP_ID;
    CSL_sysDataHandle->chipRegs = CSL_CHIP_REGS;
    CSL_sysDataHandle->xioInUse = 0;

    for(i = 0; i < ((CSL_CHIP_NUM_RESOURCES + 31) >> 5); ++i)
        CSL_sysDataHandle->resInUse[i];

    for(i = 0; i < ((CSL_CHIP_NUM_MODULES + 31) >> 5); ++i)
        CSL_sysDataHandle->initDone[i] = 0;

    CSL_sysDataHandle->intc1VectMask = 0;
    CSL_sysDataHandle->irq2VectMask = 0;
    CSL_sysDataHandle->intcEventHandlerRecord = NULL;
    CSL_sysDataHandle->irq2EventHandlerRecord = NULL;
*/
}

/* Rev.No.   Date/Time               ECN No.          Modifier      */
/* -------   ---------               -------          --------      */

/* 1          Aug 11:10:59 9         2273             xkeshavm      */
/*                                                                  */
/* To upload API CSL 0.51 Release                                   */
/********************************************************************/ 
