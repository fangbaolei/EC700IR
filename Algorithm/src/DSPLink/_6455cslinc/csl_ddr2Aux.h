/*  ============================================================================
 *   Copyright (c) Texas Instruments Inc 2002, 2003, 2004, 2005
 *
 *   Use of this software is controlled by the terms and conditions found
 *   in the license agreement under which this software has been supplied.
 *  ============================================================================
 */
/** ============================================================================
 * @file    csl_ddr2Aux.h
 *
 * @path    $(CSLPATH)\inc
 *
 * @desc    Header file for functional layer of CSL
 *          - The defines inline function definitions 
 *
 */
/* =============================================================================
 *  Revision History
 *  ===============
 *  11-Apr-2005 RM  File Created.
 *  
 *  05-Oct-2005 NG  Updation done according to new register layer
 *  
 * =============================================================================
 */
#ifndef _CSL_DDR2AUX_H_
#define _CSL_DDR2AUX_H_

#include <csl_ddr2.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  Status query functions of the DDR2 EMIF
 */
/** ============================================================================
 *   @n@b CSL_ddr2GetRevId
 *
 *   @b Description
 *   @n Gets the the current value of Rev ID value of DDR2 external memory
 *      interface
 *
 *   @b Arguments
 *   @verbatim
        hDdr2              Handle to the external memory interface instance
        
        status             Revision status structure  
 *   @endverbatim
 *
 *   <b> Return Value </b>
 *       None 
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *    @n assigns the current value of the revision status of DDR2 external 
 *    @n memory interface to the rev status structure parameter passed as 
 *       parameter
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim
        CSL_Ddr2Handle    hDdr2;
        CSL_Ddr2ModIdRev  status;
        
        CSL_ddr2GetRevId(hDdr2, status);
        ...
     @endverbatim
 * ===========================================================================
 */
CSL_IDEF_INLINE
void CSL_ddr2GetRevId (
    CSL_Ddr2Handle      hDdr2,
    CSL_Ddr2ModIdRev    *status    
)
{
    status->modId = (Uint16)CSL_FEXT(hDdr2->regs->MIDR, DDR2_MIDR_MOD_ID);
    status->majRev = (Uint8)CSL_FEXT(hDdr2->regs->MIDR, DDR2_MIDR_MJ_REV);
    status->minRev = (Uint8)CSL_FEXT(hDdr2->regs->MIDR, DDR2_MIDR_MN_REV);
}

/** ============================================================================
 *  @n@b CSL_ddr2GetEndian
 *
 *  @b Description
 *  @n Gets the the current endian of DDR2 emif from the SDRAM Status register
 *
 *  @b Arguments
 *  @verbatim
       hDdr2              Handle to the external memory interface instance
       
       response           holds the endian of DDR2 after function call
 *  @endverbatim
 *
 *  <b> Return Value </b>
 *       None 
 *
 *  <b> Pre Condition </b>
 *  @n  None
 *
 *  <b> Post Condition </b>
 *  @n  None
 *
 *  @b Modifies
 *  @n None
 *
 *  @b Example
 *  @verbatim
        CSL_Ddr2Handle     hDdr2;
        Bool               *response;
        
        CSL_ddr2GetEndian(hDdr2, response);
        ...
    @endverbatim
 * ===========================================================================
 */
CSL_IDEF_INLINE
void CSL_ddr2GetEndian (
    CSL_Ddr2Handle    hDdr2,
    Uint8             *response 
)
{
    *(response) = (Uint8)CSL_FEXT(hDdr2->regs->DMCSTAT, DDR2_DMCSTAT_BE);    
}

/** ============================================================================
 *   @n@b CSL_ddr2GetIFRDY
 *
 *   @b Description
 *   @n Gets the the current IFRDY status of DDR2 emif from the SDRAM 
 *      Status register
 *
 *   @b Arguments
 *   @verbatim
        hDdr2              Handle to the external memory interface instance
        
        response           holds IFRDY status after function call
 *   @endverbatim
 *
 *   <b> Return Value </b>
 *  @n  None
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
        CSL_Ddr2Handle     hDdr2;
        Bool               *response;
        
        CSL_ddr2GetIFRDY(hDdr2, response);
        ...
     @endverbatim
 * ============================================================================
 */
CSL_IDEF_INLINE
void CSL_ddr2GetIFRDY (
    CSL_Ddr2Handle    hDdr2,
    Uint8             *response 
)
{    
    *(response) = (Uint8)CSL_FEXT(hDdr2->regs->DMCSTAT, DDR2_DMCSTAT_IFRDY);
}

/** ============================================================================
 *   @n@b CSL_ddr2GetRefreshRate
 *
 *   @b Description
 *   @n Gets the refresh rate information of the DDR2 external memory interface 
 *
 *   @b Arguments
 *   @verbatim
        hDdr2        Handle to the DDR2 external memory interface instance
 *   @endverbatim
 *
 *   <b> Return Value </b> Uint16
                    
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
        CSL_Ddr2Handle     hDdr2;
        Uint16   rate;
        rate = CSL_ddr2GetRefreshRate(hDdr2);
        ...
     @endverbatim
 * ============================================================================
 */
CSL_IDEF_INLINE
Uint16 CSL_ddr2GetRefreshRate (
    CSL_Ddr2Handle    hDdr2
)
{
    Uint16    rate;
    rate = (Uint16) CSL_FEXT(hDdr2->regs->SDRFC, DDR2_SDRFC_REFRESH_RATE);  
     
    return rate;
}

/** ============================================================================
 *   @n@b CSL_ddr2GetSlfRfrsh
 *
 *   @b Description
 *   @n Gets the DDR2 EMIF self refresh status
 *
 *   @b Arguments
 *   @verbatim
        hDdr2        Handle to the DDR2 external memory interface instance
         
 *   @endverbatim
 *
 *   <b> Return Value </b> CSL_Ddr2SelfRefresh
                    
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
        CSL_Ddr2Handle      hDdr2;
        CSL_Ddr2SelfRefresh ddrStatus;
        
        ddrStatus = CSL_ddr2GetSlfRfrsh(hDdr2);
        ...
     @endverbatim
 * ===========================================================================
 */
CSL_IDEF_INLINE
CSL_Ddr2SelfRefresh CSL_ddr2GetSlfRfrsh (
    CSL_Ddr2Handle    hDdr2
)
{
    CSL_Ddr2SelfRefresh    status;
    status = (CSL_Ddr2SelfRefresh)CSL_FEXT(hDdr2->regs->SDRFC, DDR2_SDRFC_SR);  
     
    return status;
}

/** 
 * Control command functions external memory interface
 */
/** ============================================================================
 *   @n@b CSL_ddr2Selfrefresh
 *
 *   @b Description
 *      This API sets or clears the sr bit in Refresh Control register according
 *      to the arg passed
 *      
 *   @b Arguments
 *   @verbatim
        hDdr2    Handle to the DDR2 EMIF instance
        
        val      Value to refresh 
     @endverbatim
 *
 *   <b> Return Value </b> 
 *       None 
 *
 *   <b> Pre Condition </b>
 *   @n  1) CSL_ddr2Init () and CSL_ddr2Open should be successfully called.
 *
 *   <b> Post Condition </b>
 *   @n  1) Setting the bit places DDR2 EMIF into self refresh state
 *       2) Clearing the bit takes it out of self refresh state
 *
 *   @b  Modifies
 *   @n  sr bit in SDRAM Refresh Control register
 *
 *   @b  Example
 *   @verbatim
        CSL_Ddr2Handle     hDdr2;
        void               *arg;
        (CSL_Ddr2SelfRefresh) *arg = CSL_DDR2_SELF_REFRESH_ENABLE;
        ...
        CSL_ddr2Selfrefresh(hDdr2, arg);
        ...
     @endverbatim
 * ===========================================================================
 */
CSL_IDEF_INLINE
void CSL_ddr2Selfrefresh (
    CSL_Ddr2Handle         hDdr2,
    CSL_Ddr2SelfRefresh    val
)
{
    CSL_FINS(hDdr2->regs->SDRFC, DDR2_SDRFC_SR, val);
}

/** ============================================================================
 *   @n@b CSL_ddr2RefreshRate
 *
 *   @b Description
 *      This API sets the refresh rate value with the value of arg passed as 
 *      parameter
 *      
 *   @b Arguments
 *   @verbatim
        hDdr2        Handle to the DDR2 EMIF instance
        
        val          refresh rate value

     @endverbatim
 *
 *   <b> Return Value </b> 
 *       None 
 *
 *   <b> Pre Condition </b>
 *   @n  CSL_ddr2Init () and CSL_ddr2Open should be successfully called.
 *
 *   <b> Post Condition </b>
 *   @n  DDR2 refresh rate will be set with the argument passed
 *
 *   @b  Modifies
 *   @n  SDRAM Refresh control register
 *
 *   @b  Example
 *   @verbatim
        CSL_Ddr2Handle     hDdr2;
        void               *arg;
        (Uint16) *arg = 0x00010;
        ...
        CSL_ddr2RefreshRate(hDdr2, arg);
        ...
     @endverbatim
 * ===========================================================================
 */
CSL_IDEF_INLINE
void CSL_ddr2RefreshRate (
    CSL_Ddr2Handle    hDdr2,
    Uint16            refRate
)
{
    CSL_FINS(hDdr2->regs->SDRFC, DDR2_SDRFC_REFRESH_RATE, refRate);
}

/** ============================================================================
 *   @n@b CSL_ddr2PrioRaise
 *
 *   @b Description
 *      This API writes the arg passed into the pr_old_count field of VBUSM 
 *      Burst Priority register.
 *      
 *   @b Arguments
 *   @verbatim
        hDdr2        Handle to the DDR2 EMIF
        
        val          Value to raise priority value

     @endverbatim
 *
 *   <b> Return Value </b> 
 *       None 
 *
 *   <b> Pre Condition </b>
 *   @n  CSL_ddr2Init () and CSL_ddr2Open should be successfully called.   
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b  Modifies
 *   @n  VBUSM Burst Priority register
 *
 *   @b  Example
 *   @verbatim
        CSL_Ddr2Handle     hDdr2;
        void               *arg;
        (Uint8) *arg = 0x00010;
        ...
        CSL_ddr2PrioRaise(hDdr2, arg); 
        ...
     @endverbatim
 * ===========================================================================
 */
CSL_IDEF_INLINE
void CSL_ddr2PrioRaise (
    CSL_Ddr2Handle    hDdr2,
    Uint8             val
)
{
    CSL_FINS(hDdr2->regs->BPRIO, DDR2_BPRIO_PRIO_RAISE, val);
}

#ifdef __cplusplus
}
#endif

#endif /* _CSL_DDR2AUX_H_ */

