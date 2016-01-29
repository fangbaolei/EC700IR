/* ---- File: <csl_sysData.c> ---- */

#include <csl_sysData.h>
#include <csl_version.h>
#include <_csl_intc.h>

#pragma DATA_SECTION (CSL_sysDataObj, ".bss:csl_section:sys");

CSL_SysDataObj CSL_sysDataObj = {
    NULL,               /* alternate base address router */
    NULL,               /* physical to virtual address mapping routine */
    NULL,               /* virtual to physical address mapping routine */
    CSL_VERSION_ID,     /* defined in csl_version.h */
    CSL_CHIP_ID,        /* defined in _csl_device.h */
#if 0    
    NULL,               /* CSL chip Overlay structure, to be initialized
                                 * in CSL_chipInit() */
#endif                           
    0,                  /* XIO shared pins in use mask */
    {0},                /* CSL peripheral resources in-use mask-matrix */
    {0},                /* CSL modules' init Done bit flag matrix */
    0,                  /* intcAllocMask */
    NULL,               /* intcEventHandlerRecord */
    NULL,               /* edmaParamAllocMask[ ] */
	NULL,               /* edmaTccAllocMask[ ] */
	NULL                /* edmaShadowAllocMask[ ] */
};

CSL_SysDataHandle CSL_sysDataHandle = &CSL_sysDataObj;

/* Rev.No.   Date/Time               ECN No.          Modifier      */
/* -------   ---------               -------          --------      */

/* 1          Aug 11:10:58 9         2273             xkeshavm      */
/*                                                                  */
/* To upload API CSL 0.51 Release                                   */
/********************************************************************/ 
