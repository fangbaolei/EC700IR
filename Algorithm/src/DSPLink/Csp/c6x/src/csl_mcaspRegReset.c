/** ===========================================================================
 *   @file  csl_mcaspRegReset.c
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
    
/*  @(#) PSP/CSL 3.00.01.00[5905] (2005-01-12)  */ 
    
/* ============================================================================
 *  Revision History
 *  ===============
 *  12-Jan-2005 asr File Created.
 * ============================================================================
 */ 
    
#include <davinci_hd64plus.h>
#include <csl_mcasp.h>
    
/** ===========================================================================
 *   @n@b CSL_mcaspRegReset
 *
 *   @b Description
 *   @n This function resets all the register values
 *
 *   @b Arguments
 *   @verbatim
            hMcasp         Handle to the McASP instance
     @endverbatim
 *
 *   <b> Return Value </b>  
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  All register values will be reset.
 *
 *   @b Modifies
 *   @n All McASP registers.
 *
 *   @b Example
 *   @verbatim
            CSL_McaspHandle hMcasp;
            
            CSL_mcaspRegReset (hMcasp);
    @endverbatim
 * ============================================================================
 */ 
#pragma CODE_SECTION (CSL_mcaspRegReset, ".text:csl_section:mcasp");
void CSL_mcaspRegReset (
    CSL_McaspHandle hMcasp 
) 
{
    Uint32 serNum = 0;
    
    hMcasp->regs->GBLCTL = CSL_MCASP_GBLCTL_RESETVAL;
    hMcasp->regs->PWRDEMU = CSL_MCASP_PWRDEMU_RESETVAL;
    hMcasp->regs->RMASK = CSL_MCASP_RMASK_RESETVAL;
    hMcasp->regs->RFMT = CSL_MCASP_RFMT_RESETVAL;
    hMcasp->regs->AFSRCTL = CSL_MCASP_AFSRCTL_RESETVAL;
    hMcasp->regs->ACLKRCTL = CSL_MCASP_ACLKRCTL_RESETVAL;
    hMcasp->regs->AHCLKRCTL = CSL_MCASP_AHCLKRCTL_RESETVAL;
    hMcasp->regs->RTDM = CSL_MCASP_RTDM_RESETVAL;
    hMcasp->regs->RINTCTL = CSL_MCASP_RINTCTL_RESETVAL;
    hMcasp->regs->RCLKCHK = CSL_MCASP_RCLKCHK_RESETVAL;
    hMcasp->regs->XMASK = CSL_MCASP_XMASK_RESETVAL;
    hMcasp->regs->XFMT = CSL_MCASP_XFMT_RESETVAL;
    hMcasp->regs->AFSXCTL = CSL_MCASP_AFSXCTL_RESETVAL;
    hMcasp->regs->ACLKXCTL = CSL_MCASP_ACLKXCTL_RESETVAL;
    hMcasp->regs->AHCLKXCTL = CSL_MCASP_AHCLKXCTL_RESETVAL;
    hMcasp->regs->XTDM = CSL_MCASP_XTDM_RESETVAL;
    hMcasp->regs->XINTCTL = CSL_MCASP_XINTCTL_RESETVAL;
    hMcasp->regs->XCLKCHK = CSL_MCASP_XCLKCHK_RESETVAL;
    while (serNum < hMcasp->numOfSerializers) {
        hMcasp->regs->SRCTL0 = CSL_MCASP_SRCTL0_RESETVAL;
        if (++serNum >= hMcasp->numOfSerializers)
            break;
        hMcasp->regs->SRCTL1 = CSL_MCASP_SRCTL1_RESETVAL;
        if (++serNum >= hMcasp->numOfSerializers)
            break;
        hMcasp->regs->SRCTL2 = CSL_MCASP_SRCTL2_RESETVAL;
        if (++serNum >= hMcasp->numOfSerializers)
            break;
        hMcasp->regs->SRCTL3 = CSL_MCASP_SRCTL3_RESETVAL;
        if (++serNum >= hMcasp->numOfSerializers)
            break;
        hMcasp->regs->SRCTL4 = CSL_MCASP_SRCTL4_RESETVAL;
        if (++serNum >= hMcasp->numOfSerializers)
            break;
        hMcasp->regs->SRCTL5 = CSL_MCASP_SRCTL5_RESETVAL;
        if (++serNum >= hMcasp->numOfSerializers)
            break;
        hMcasp->regs->SRCTL6 = CSL_MCASP_SRCTL6_RESETVAL;
        if (++serNum >= hMcasp->numOfSerializers)
            break;
        hMcasp->regs->SRCTL7 = CSL_MCASP_SRCTL7_RESETVAL;
        if (++serNum >= hMcasp->numOfSerializers)
            break;
        hMcasp->regs->SRCTL8 = CSL_MCASP_SRCTL8_RESETVAL;
        if (++serNum >= hMcasp->numOfSerializers)
            break;
        hMcasp->regs->SRCTL9 = CSL_MCASP_SRCTL9_RESETVAL;
        if (++serNum >= hMcasp->numOfSerializers)
            break;
        hMcasp->regs->SRCTL10 = CSL_MCASP_SRCTL10_RESETVAL;
        if (++serNum >= hMcasp->numOfSerializers)
            break;
        hMcasp->regs->SRCTL11 = CSL_MCASP_SRCTL11_RESETVAL;
        if (++serNum >= hMcasp->numOfSerializers)
            break;
        hMcasp->regs->SRCTL12 = CSL_MCASP_SRCTL12_RESETVAL;
        if (++serNum >= hMcasp->numOfSerializers)
            break;
        hMcasp->regs->SRCTL13 = CSL_MCASP_SRCTL13_RESETVAL;
        if (++serNum >= hMcasp->numOfSerializers)
            break;
        hMcasp->regs->SRCTL14 = CSL_MCASP_SRCTL14_RESETVAL;
        if (++serNum >= hMcasp->numOfSerializers)
            break;
        hMcasp->regs->SRCTL15 = CSL_MCASP_SRCTL15_RESETVAL;
        if (++serNum >= hMcasp->numOfSerializers)
            break;
    }
    
    hMcasp->regs->PFUNC = CSL_MCASP_PFUNC_RESETVAL;
    hMcasp->regs->PDIR = CSL_MCASP_PDIR_RESETVAL;
    hMcasp->regs->DITCTL = CSL_MCASP_DITCTL_RESETVAL;
    hMcasp->regs->DLBCTL = CSL_MCASP_DLBCTL_RESETVAL;
    hMcasp->regs->AMUTE = CSL_MCASP_AMUTE_RESETVAL;
    hMcasp->regs->RSTAT = CSL_MCASP_RSTAT_RESETVAL;
    hMcasp->regs->REVTCTL = CSL_MCASP_REVTCTL_RESETVAL;
    hMcasp->regs->XSTAT = CSL_MCASP_XSTAT_RESETVAL;
    hMcasp->regs->XEVTCTL = CSL_MCASP_XEVTCTL_RESETVAL;
    hMcasp->regs->PDOUT = CSL_MCASP_PDOUT_RESETVAL;
    hMcasp->regs->PDIN_PDSET = CSL_MCASP_PDIN_PDSET_RESETVAL;
    hMcasp->regs->PDCLR = CSL_MCASP_PDCLR_RESETVAL;
}


