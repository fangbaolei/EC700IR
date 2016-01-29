/*  ============================================================================
 *   Copyright (c) Texas Instruments Inc 2002, 2003, 2004, 2005
 *
 *   Use of this software is controlled by the terms and conditions found
 *   in the license agreement under which this software has been supplied.
 *  ============================================================================
 */
/** ============================================================================
 *  @file    csl_emifaAux.h
 *
 *  @path    $(CSLPATH)\inc
 *
 *  @desc    Header file for functional layer of CSL
 *           - The defines inline function definitions 
 *
 */
/* =============================================================================
 *  Revision History
 *  ===============
 *  12-May-2005 RM  File Created.
 *  
 *  08-Jul-2005 RM  Changes made in accordance to the change in cslr_emifa.h            
 *                               
 *  09-Sep-2005 NG  Updation according to coding guidelines
 *  
 * =============================================================================
 */
#ifndef _CSL_EMIFAAUX_H_
#define _CSL_EMIFAAUX_H_

#include <csl_emifa.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Status query functions of the EMIFA EMIF */
 
/** ============================================================================
 *   @n@b CSL_emifaGetRevId
 *
 *   @b Description
 *   @n Gets the current value of Rev ID value of EMIFA external memory 
 *      interface
 *
 *   @b Arguments
 *   @verbatim
        hEmifa              Handle to the external memory interface instance
        
        status              Revision status structure  
 *   @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *    @n Assigns the current value of the revision status of EMIFA external 
 *    @n memory interface to the rev status structure parameter passed as 
 *       parameter.
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim
        CSL_EmifaHandle     hEmifa;
        CSL_EmifaModIdRev  *status;
        
        CSL_emifaGetRevId(hEmifa, status);
        ...
     @endverbatim
 * ===========================================================================
 */
CSL_IDEF_INLINE
void CSL_emifaGetRevId (
    CSL_EmifaHandle      hEmifa,
    CSL_EmifaModIdRev    *status    
)
{
    status->modId = (Uint16)CSL_FEXT(hEmifa->regs->MIDR, EMIFA_MIDR_MOD_ID);
    status->majRev = (Uint8)CSL_FEXT(hEmifa->regs->MIDR, EMIFA_MIDR_MJ_REV);
    status->minRev = (Uint8)CSL_FEXT(hEmifa->regs->MIDR, EMIFA_MIDR_MN_REV);
}

/** ============================================================================
 *   @n@b CSL_emifaGetEndian
 *
 *   @b Description
 *   @n Gets the the current endian of EMIFA emif from the Status register
 *
 *   @b Arguments
 *   @verbatim
        hEmifa              Handle to the external memory interface instance
        
        response            holds the edian of EMIFA after function call
 *   @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
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
        CSL_EmifaHandle     hEmifa;
        Uint8               *response;
        
        CSL_emifaGetEndian(hEmifa, response);
        ...
     @endverbatim
 * =============================================================================
 */
CSL_IDEF_INLINE
void CSL_emifaGetEndian (
    CSL_EmifaHandle    hEmifa,
    Uint8              *response 
)
{
    *(response) = (Uint8)CSL_FEXT(hEmifa->regs->STAT, EMIFA_STAT_BE);    
}

/** ============================================================================
 *   @n@b CSL_emifaGetAsyncEnStatus
 *
 *   @b Description
 *   @n Gets the Async Timeout interrupt enable status in Interrupt Masked Set 
 *      Register and Interrupt Masked Clear Register.
 *
 *   @b Arguments
 *   @verbatim
        hEmifa            Handle to the EMIFA external memory interface instance
         
 *   @endverbatim
 *
 *   <b> Return Value </b> Uint8
                    
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
        CSL_EmifaHandle      hEmifa;
        Uint8              status;
        
        status = CSL_emifaGetAsyncEnStatus(hEmifa);
        ...
     @endverbatim
 * =============================================================================
 */
CSL_IDEF_INLINE
Uint8 CSL_emifaGetAsyncEnStatus (
    CSL_EmifaHandle    hEmifa
)
{
    Uint8    status;
    status = ((Uint8) CSL_FEXT(hEmifa->regs->INTMSKCLR, 
                               EMIFA_INTMSKCLR_AT_MASK_CLR)) & 
             ((Uint8) CSL_FEXT(hEmifa->regs->INTMSKSET, 
                               EMIFA_INTMSKSET_AT_MASK_SET)) ;  
    return status;
}

/** ============================================================================
 *   @n@b CSL_emifaGetAsyncStatus
 *
 *   @b Description
 *   @n Gets the Async Timeout interrupt status in Interrupt Raw Register and
 *      Interrupt Masked register.
 *
 *   @b Arguments
 *   @verbatim
        hEmifa            Handle to the EMIFA external memory interface instance
         
 *   @endverbatim
 *
 *   <b> Return Value </b> Uint8
                    
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
        CSL_EmifaHandle      hEmifa;
        Uint8                status;
        
        status = CSL_emifaGetAsyncStatus(hEmifa);
        ...
     @endverbatim
 * =============================================================================
 */
CSL_IDEF_INLINE
Uint8 CSL_emifaGetAsyncStatus (
    CSL_EmifaHandle    hEmifa
)
{
    Uint8    status;
    status = (Uint8) CSL_FEXT(hEmifa->regs->INTRAW, EMIFA_INTRAW_AT) ||
             (Uint8) CSL_FEXT(hEmifa->regs->INTMSK, EMIFA_INTMSK_AT_MASKED);  
     
    return status;
}

/** Control command functions external memory interface */
/** ============================================================================
 *   @n@b CSL_emifaAsyncClear
 *
 *   @b Description
 *      This API clears the Asynchronous Timeout interrupt
 *      
 *   @b Arguments
 *   @verbatim
        hEmifa    Handle to the EMIFA 
     @endverbatim
 *
 *   <b> Return Value </b> 
 *       None 
 *
 *   <b> Pre Condition </b>
 *   @n  CSL_emifaInit () and CSL_emifaOpen should be successfully called.
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b  Modifies
 *   @n  Interrupt Raw register and Interrupt Masked Register
 *
 *   @b  Example
 *   @verbatim
        CSL_EmifaHandle     hEmifa;
                ...
        CSL_emifaAsyncClear(hEmifa);
        ...
     @endverbatim
 * =============================================================================
 */
CSL_IDEF_INLINE
void CSL_emifaAsyncClear (
    CSL_EmifaHandle    hEmifa
)
{
    CSL_FINST(hEmifa->regs->INTRAW, EMIFA_INTRAW_AT, SET);
}

/** ============================================================================
 *   @n@b CSL_emifaAsyncDisable
 *
 *   @b Description
 *      This API disables the Asynchronous Timeout interrupt
 *      
 *   @b Arguments
 *   @verbatim
        hEmifa    Handle to the EMIFA 
     @endverbatim
 *
 *   <b> Return Value </b> 
 *       None 
 *
 *   <b> Pre Condition </b>
 *   @n  CSL_emifaInit () and CSL_emifaOpen should be successfully called.
 *
 *   <b> Post Condition </b>
 *   @n  Interrupt doesn't reach the cpu boundary
 *
 *   @b  Modifies
 *   @n  Interrupt Mask Clear register and Interrupt Mask Set register
 *
 *   @b  Example
 *   @verbatim
        CSL_EmifaHandle     hEmifa;
                ...
        CSL_emifaAsyncDisable(hEmifa);
        ...
     @endverbatim
 * =============================================================================
 */
CSL_IDEF_INLINE
void CSL_emifaAsyncDisable (
    CSL_EmifaHandle    hEmifa
)
{
    CSL_FINST(hEmifa->regs->INTMSKCLR, EMIFA_INTMSKCLR_AT_MASK_CLR, SET);
}

/** ============================================================================
 *   @n@b CSL_emifaAsyncEnable
 *
 *   @b Description
 *      This API enables the Asynchronous Timeout interrupt
 *      
 *   @b Arguments
 *   @verbatim
        hEmifa    Handle to the EMIFA 
     @endverbatim
 *
 *   <b> Return Value </b> 
 *       None 
 *
 *   <b> Pre Condition </b>
 *   @n  CSL_emifaInit () and CSL_emifaOpen should be successfully called.
 *
 *   <b> Post Condition </b>
 *   @n  Interrupt enabled
 *
 *   @b  Modifies
 *   @n  Interrupt Mask Clear register and Interrupt Mask Set register
 *
 *   @b  Example
 *   @verbatim
        CSL_EmifaHandle     hEmifa;
                ...
        CSL_emifaAsyncEnable(hEmifa);
        ...
     @endverbatim
 * =============================================================================
 */
CSL_IDEF_INLINE
void CSL_emifaAsyncEnable (
    CSL_EmifaHandle    hEmifa
)
{
    CSL_FINST(hEmifa->regs->INTMSKSET, EMIFA_INTMSKSET_AT_MASK_SET, SET);
}

/** ============================================================================
 *   @n@b CSL_emifaPrioRaise
 *
 *   @b Description
 *      This API writes the arg passed into the pr_old_count field of Burst 
 *      Priority register.
 *      
 *   @b Arguments
 *   @verbatim
        hEmifa    Handle to the EMIFA EMIF
        val       Value to set to raise the priority value

     @endverbatim
 *
 *   <b> Return Value </b> 
 *       None 
 *
 *   <b> Pre Condition </b>
 *   @n  CSL_emifaInit () and CSL_emifaOpen should be successfully called.   
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b  Modifies
 *   @n  Burst Priority register
 *
 *   @b  Example
 *   @verbatim
        CSL_EmifaHandle     hEmifa;
        void               *arg;
        (Uint8) *arg = 0x00010;
        ...
        CSL_emifaPrioRaise(hEmifa, arg); 
        ...
     @endverbatim
 * =============================================================================
 */
CSL_IDEF_INLINE
void CSL_emifaPrioRaise (
    CSL_EmifaHandle    hEmifa,
    Uint8              val
)
{
    CSL_FINS(hEmifa->regs->BPRIO, EMIFA_BPRIO_PRIO_RAISE, val);
}

#ifdef __cplusplus
}
#endif

#endif /* _CSL_EMIFAAUX_H_ */

