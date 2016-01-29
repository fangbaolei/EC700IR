/** ===========================================================================
 *   @file  csl_mcaspResetCtrl.c
 *
 *   @path  $(CSLPATH)\mcasp\src
 *
 *   @desc  Multi channel audio serial port CSL implementation.
 *
 */  
    
/*  ===========================================================================
 *  Copyright (c) Texas Instruments Inc 2002, 2003, 2004, 2005
 *
 *  Use of this software is controlled by the terms and conditions found in the
 *  license agreement under which this software has been supplied.
 *  ===========================================================================
 */ 
    
/*  @(#) PSP/CSL 3.00.01.00[5905] (2005-01-13)  */ 
    
/* ============================================================================
 *  Revision History
 *  ===============
 *  13-Jan-2005 asr File Created.
 * ============================================================================
 */ 
    
#include <csl_mcasp.h>
    
/** ===========================================================================
 *   @n@b CSL_mcaspResetCtrl
 *
 *   @b Description
 *   @n This function enable bit fields of GBLCTL register
 *
 *   @b Arguments
 *   @verbatim
            hMcasp          Handle to the McASP instance
            
            selectMask      Selects the bits to enable
            
     @endverbatim
 *
 *   <b> Return Value </b>  
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  Some GBLCTL register bit fields are enabled.
 *
 *   @b Modifies
 *   @n   GBLCTL register.
 *
 *   @b Example
 *   @verbatim
            CSL_McaspHandle hMcasp;
            
            CSL_mcaspResetCtrl (hMcasp, CSL_MCASP_GBLCTL_XSMRST_ACTIVE);
    @endverbatim
 * ============================================================================
 */ 
#pragma CODE_SECTION (CSL_mcaspResetCtrl, ".text:csl_section:mcasp");

void CSL_mcaspResetCtrl (
    CSL_McaspHandle    hMcasp,
    CSL_BitMask32      selectMask 
) 
{
        /* Activate the RFRST bit field of GBLCTL */ 
        if (selectMask & CSL_MCASP_GBLCTL_RFRST_MASK) {
            CSL_FINST (hMcasp->regs->GBLCTL, MCASP_GBLCTL_RFRST, ACTIVE);
            while ((hMcasp->regs->GBLCTL & 0x0010) != 0x0010);
        }
    
        /* Activate the RSMRST bit field of GBLCTL */ 
        if (selectMask & CSL_MCASP_GBLCTL_RSMRST_MASK) {
            CSL_FINST (hMcasp->regs->GBLCTL, MCASP_GBLCTL_RSMRST, ACTIVE);
            while ((hMcasp->regs->GBLCTL & 0x0008) != 0x0008);
        }
    
        /* Activate the RSRCLR bit field of GBLCTL */ 
        if (selectMask & CSL_MCASP_GBLCTL_RSRCLR_MASK) {
            CSL_FINST (hMcasp->regs->GBLCTL, MCASP_GBLCTL_RSRCLR, ACTIVE);
            while ((hMcasp->regs->GBLCTL & 0x0004) != 0x0004);
        }
    
        /* Activate the RHCLKRST bit field of GBLCTL */ 
        if (selectMask & CSL_MCASP_GBLCTL_RHCLKRST_MASK) {
            CSL_FINST (hMcasp->regs->GBLCTL, MCASP_GBLCTL_RHCLKRST, ACTIVE);
            while ((hMcasp->regs->GBLCTL & 0x0002) != 0x0002);
        }
    
        /* Activate the RCLKRST bit field of GBLCTL */ 
        if (selectMask & CSL_MCASP_GBLCTL_RCLKRST_MASK) {
            CSL_FINST (hMcasp->regs->GBLCTL, MCASP_GBLCTL_RCLKRST, ACTIVE);
            while ((hMcasp->regs->GBLCTL & 0x0001) != 0x0001);
        }
    
        /* Activate the XFRST bit field of GBLCTL */ 
        if (selectMask & CSL_MCASP_GBLCTL_XFRST_MASK) {
            CSL_FINST (hMcasp->regs->GBLCTL, MCASP_GBLCTL_XFRST, ACTIVE);
            while ((hMcasp->regs->GBLCTL & 0x1000) != 0x1000);
        }
    
        /* Activate the XSMRST bit field of GBLCTL */ 
        if (selectMask & CSL_MCASP_GBLCTL_XSMRST_MASK) {
            CSL_FINST (hMcasp->regs->GBLCTL, MCASP_GBLCTL_XSMRST, ACTIVE);
            while ((hMcasp->regs->GBLCTL & 0x0800) != 0x0800);
        }
    
        /* Activate the XSRCLR bit field of GBLCTL */ 
        if (selectMask & CSL_MCASP_GBLCTL_XSRCLR_MASK) {
            CSL_FINST (hMcasp->regs->GBLCTL, MCASP_GBLCTL_XSRCLR, ACTIVE);
            while ((hMcasp->regs->GBLCTL & 0x0400) != 0x0400);
        }
    
        /* Activate the XHCLKRST bit field of GBLCTL */ 
        if (selectMask & CSL_MCASP_GBLCTL_XHCLKRST_MASK) {
            CSL_FINST (hMcasp->regs->GBLCTL, MCASP_GBLCTL_XHCLKRST, ACTIVE);
            while ((hMcasp->regs->GBLCTL & 0x0200) != 0x0200);
        }
    
        /* Activate the XCLKRST bit field of GBLCTL */ 
        if (selectMask & CSL_MCASP_GBLCTL_XCLKRST_MASK) {
            CSL_FINST (hMcasp->regs->GBLCTL, MCASP_GBLCTL_XCLKRST, ACTIVE);
            while ((hMcasp->regs->GBLCTL & 0x0100) != 0x0100);
        }
}


