/* ============================================================================
 * Copyright (c) Texas Instruments Inc 2002, 2003, 2004, 2005                 
 *                                                                              
 * Use of this software is controlled by the terms and conditions found in the
 * license agreement under which this software has been supplied.             
 * ============================================================================
 */ 

/** =========================================================================== 
 * @file csl_cfgAux.h
 *
 * @brief Header file for functional layer of CSL
 *  
 * @path $(CSLPATH)\inc
 *  
 * Description
 *    - The defines inline function definitions
 * ============================================================================
 */


/* ============================================================================
 * Revision History
 * ===============
 * 14-Apr-2005 Brn File Created
 * ============================================================================
 */

#ifndef _CSL_CFGAUX_H_
#define _CSL_CFGAUX_H_

#include <csl_cfg.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  Control Functions of cfg.
 */
/** ============================================================================
 *   @n@b CSL_cfgClear
 *
 *   @b Description
 *   @n This function clears the CFG module.
 *
 *   @b Arguments
 *   @verbatim

            hCfg            Handle to CFG instance

     @endverbatim
 *
 *   <b> Return Value </b>  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *    @n None
 *
 *   @b Modifies
 *   @n  Clears the status and flag values
 *
 *   @b Example
 *   @verbatim

        CSL_cfgEnable(hCfg);

     @endverbatim
 * ===========================================================================
 */
CSL_IDEF_INLINE
void CSL_cfgCmdClear (
    /* Handle to CFG instance */
    CSL_CfgHandle hCfg
)
{
    CSL_FINS(((CSL_CfgRegs *)CSL_MEMPROT_CONFIG_REGS)->
                                                   MPFCR, CFG_MPFCR_MPFCLR, 1);
}

/** Status query functions */

/** ============================================================================
 *   @n@b CSL_cfgGetFaultAddr
 *
 *   @b Description
 *   @n This function returns the Fault Adrress.
 *
 *   @b Arguments
 *   @verbatim
            hCfg            Handle to CFG instance
            
            faultAddr       Place holder to get the Fault Address
     @endverbatim
 *
 *   <b> Return Value </b>  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *    @n None
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim

        CSL_cfgGetFaultAddr(hCfg);

     @endverbatim
 * ===========================================================================
 */
CSL_IDEF_INLINE
void CSL_cfgGetFaultAddr (
    /* Handle to CFG instance */
    CSL_CfgHandle hCfg,
    
    /* Pointer which holds the fault address */
    Uint32        *faultAddr
)
{
    *faultAddr = ((CSL_CfgRegs*)CSL_MEMPROT_CONFIG_REGS)->MPFAR;
}

/** ============================================================================
 *   @n@b CSL_cfgGetCfgStat
 *
 *   @b Description
 *   @n This function gets the CFG status information.
 *
 *   @b Arguments
 *   @verbatim

            hCfg            Handle to CFG instance

            cfgStatus       Placeholder to return status.

     @endverbatim
 *
 *   <b> Return Value </b>  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *    @n None
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim

        CSL_cfgGetCfgStat(hCfg, &cfgStatus);

     @endverbatim
 * ===========================================================================
 */
CSL_IDEF_INLINE
void CSL_cfgGetFaultStat (
    /* Handle to CFG instance */
    CSL_CfgHandle      hCfg,

    /* Placeholder to return the status. @a void* casted */
    CSL_CfgFaultStatus *faultStatus
)
{
    faultStatus->errorMask = 
               CSL_FEXTR(((CSL_CfgRegs*)CSL_MEMPROT_CONFIG_REGS)->MPFSR, 8, 0);
    faultStatus->faultId = 
               CSL_FEXTR(((CSL_CfgRegs*)CSL_MEMPROT_CONFIG_REGS)->MPFSR, 15, 9);
    
}


#ifdef __cplusplus
}
#endif


#endif /* CSL_CFGAUX_H_ */


