/** ===========================================================================
 *   @file  csl_mcaspHwSetupRaw.c
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
    
#include <csl_mcasp.h>
    
/** ===========================================================================
 *   @n@b CSL_mcaspHwSetupRaw
 *
 *   @b Description
 *   @n This function initializes the device registers with the register-values
 *      provided through the config data structure. This configures registers 
 *      based on a structure of register values, as compared to HwSetup, which 
 *      configures registers based on structure of bit field values and may 
 *      perform other functions (delays, etc.)
 *
 *   @b Arguments
 *   @verbatim
            hMcasp        Handle to the McASP instance

            config        Pointer to config structure
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK             - Configuration successful
 *   @li                    CSL_ESYS_BADHANDLE  - Invalid handle
 *   @li                    CSL_ESYS_INVPARAMS  - Configuration structure is
 *                                                not properly initialized
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  The registers of the specified McASP instance will be setup
 *       according to value passed
 *
 *   @b Modifies
 *   @n Hardware registers of the specified McASP instance
 *
 *   @b Example
 *   @verbatim
        CSL_McaspHandle       hMcasp;
        CSL_McaspConfig       config;
        CSL_Status            status;
        
        config.PWRDEMU =0x00000001;
        config.PFUNC   = 0x00000000;
        config.PDIR    = 0x00000000;
        config.PDOUT   = 0x00000000;
        config.PDIN_PDSET = 0x00000000;
        config.PDCLR  = 0x00000000;
        config.GBLCTL = 0x00000000;
        config.AMUTE  = 0x00000000;
        config.DLBCTL = 0x00000000;
        config.DITCTL = 0x00000000;
        config.RMASK  = 0x00000000;
        config.RFMT   = 0x00000000;
        config.AFSRCTL   = 0x00000000;
        config.ACLKRCTL  = 0x00000020;
        config.AHCLKRCTL = 0x00008000;
        config.RTDM    = 0x00000000;
        config.RINTCTL = 0x00000000;
        config.RSTAT   = 0x00000000;
        config.RCLKCHK = 0x00000000;
        config.REVTCTL = 0x00000000;
        config.XMASK   = 0x00000000;
        config.XFMT    = 0x00000000;
        config.AFSXCTL = 0x00000000;
        config.ACLKXCTL  = 0x00000060;
        config.AHCLKXCTL = 0x00008000;
        config.XTDM    = 0x00000000;
        config.XINTCTL = 0x00000000;
        config.XSTAT   = 0x00000000;
        config.XCLKCHK = 0x00000000;
        config.XEVTCTL = 0x00000000;
        config.SRCTL0  = 0x00000000;
        config.SRCTL1  = 0x00000000;
        config.SRCTL2  = 0x00000000;
        config.SRCTL3  = 0x00000000;
        config.SRCTL4  = 0x00000000;
        config.SRCTL5  = 0x00000000;

        status = CSL_mcaspHwSetupRaw (hMcasp, &config);

     @endverbatim
 * ============================================================================
 */ 
    
#pragma CODE_SECTION (CSL_mcaspHwSetupRaw, ".text:csl_section:mcasp");

CSL_Status CSL_mcaspHwSetupRaw (
    CSL_McaspHandle    hMcasp,
    CSL_McaspConfig    *config
) 
{
    Uint32 serNum = 0;
    
    if (hMcasp == NULL) {
        return CSL_ESYS_BADHANDLE;
    }
            
    if (config == NULL) {
        return CSL_ESYS_INVPARAMS;
    }
            
    hMcasp->regs->GBLCTL = config->GBLCTL;
    hMcasp->regs->PWRDEMU = config->PWRDEMU;
    hMcasp->regs->RMASK = config->RMASK;
    hMcasp->regs->RFMT = config->RFMT;
    hMcasp->regs->AFSRCTL = config->AFSRCTL;
    hMcasp->regs->ACLKRCTL = config->ACLKRCTL;
    hMcasp->regs->AHCLKRCTL = config->AHCLKRCTL;
    hMcasp->regs->RTDM = config->RTDM;
    hMcasp->regs->RINTCTL = config->RINTCTL;
    hMcasp->regs->RCLKCHK = config->RCLKCHK;
    hMcasp->regs->XMASK = config->XMASK;
    hMcasp->regs->XFMT = config->XFMT;
    hMcasp->regs->AFSXCTL = config->AFSXCTL;
    hMcasp->regs->ACLKXCTL = config->ACLKXCTL;
    hMcasp->regs->AHCLKXCTL = config->AHCLKXCTL;
    hMcasp->regs->XTDM = config->XTDM;
    hMcasp->regs->XINTCTL = config->XINTCTL;
    hMcasp->regs->XCLKCHK = config->XCLKCHK;
    
    while (serNum < hMcasp->numOfSerializers) {
        hMcasp->regs->SRCTL0 = config->SRCTL0;
        if (++serNum >= hMcasp->numOfSerializers)
            break;
        hMcasp->regs->SRCTL1 = config->SRCTL1;
        if (++serNum >= hMcasp->numOfSerializers)
            break;
        hMcasp->regs->SRCTL2 = config->SRCTL2;
        if (++serNum >= hMcasp->numOfSerializers)
            break;
        hMcasp->regs->SRCTL3 = config->SRCTL3;
        if (++serNum >= hMcasp->numOfSerializers)
            break;
        hMcasp->regs->SRCTL4 = config->SRCTL4;
        if (++serNum >= hMcasp->numOfSerializers)
            break;
        hMcasp->regs->SRCTL5 = config->SRCTL5;
        if (++serNum >= hMcasp->numOfSerializers)
            break;
        hMcasp->regs->SRCTL6 = config->SRCTL6;
        if (++serNum >= hMcasp->numOfSerializers)
            break;
        hMcasp->regs->SRCTL7 = config->SRCTL7;
        if (++serNum >= hMcasp->numOfSerializers)
            break;
        hMcasp->regs->SRCTL8 = config->SRCTL8;
        if (++serNum >= hMcasp->numOfSerializers)
            break;
        hMcasp->regs->SRCTL9 = config->SRCTL9;
        if (++serNum >= hMcasp->numOfSerializers)
            break;
        hMcasp->regs->SRCTL10 = config->SRCTL10;
        if (++serNum >= hMcasp->numOfSerializers)
            break;
        hMcasp->regs->SRCTL11 = config->SRCTL11;
        if (++serNum >= hMcasp->numOfSerializers)
            break;
        hMcasp->regs->SRCTL12 = config->SRCTL12;
        if (++serNum >= hMcasp->numOfSerializers)
            break;
        hMcasp->regs->SRCTL13 = config->SRCTL13;
        if (++serNum >= hMcasp->numOfSerializers)
            break;
        hMcasp->regs->SRCTL14 = config->SRCTL14;
        if (++serNum >= hMcasp->numOfSerializers)
            break;
        hMcasp->regs->SRCTL15 = config->SRCTL15;
        if (++serNum >= hMcasp->numOfSerializers)
            break;
    }
    
    hMcasp->regs->PFUNC = config->PFUNC;
    hMcasp->regs->PDIR = config->PDIR;
    
    if (hMcasp->ditStatus == TRUE) {
        hMcasp->regs->DITCTL = config->DITCTL;
    }
    
    hMcasp->regs->DLBCTL = config->DLBCTL;
    hMcasp->regs->AMUTE = config->AMUTE;
    hMcasp->regs->RSTAT = config->RSTAT;
    hMcasp->regs->XSTAT = config->XSTAT;
    hMcasp->regs->PDOUT = config->PDOUT;
    hMcasp->regs->PDIN_PDSET = config->PDIN_PDSET;
    hMcasp->regs->PDCLR = config->PDCLR;
    hMcasp->regs->REVTCTL = config->REVTCTL;
    hMcasp->regs->XEVTCTL = config->XEVTCTL;
    
    return CSL_SOK;
}


