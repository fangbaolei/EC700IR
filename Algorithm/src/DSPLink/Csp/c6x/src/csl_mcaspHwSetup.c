/** ===========================================================================
 *   @file  csl_mcaspHwSetup.c
 *
 *   @path  $(CSLPATH)\mcasp\src
 *
 *   @desc  Multi channel audio serial port CSL implementation
 *
 */  
    
/*  ===========================================================================
 *   Copyright (c) Texas Instruments Inc 2002, 2003, 2004, 2005
 *
 *   Use of this software is controlled by the terms and conditions found in
 *   the license agreement under which this software has been supplied.
 *   ==========================================================================
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
 *   @n@b CSL_mcaspHwSetup
 *
 *   @b Description
 *   @n It configures the  McASP instance registers as per the values passed
 *      in the hardware setup structure.
 *
 *   @b Arguments
 *   @verbatim
            hMcasp      Handle to the McASP instance

            myHwSetup   Pointer to hardware setup structure
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK             - Hardware setup successful
 *   @li                    CSL_ESYS_BADHANDLE  - Invalid handle
 *   @li                    CSL_ESYS_INVPARAMS  - Hardware structure is not
 *                                                properly initialized
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  The specified instance will be setup according to value passed
 *
 *   @b Modifies
 *   @n Hardware registers for the specified instance
 *
 *   @b Example
 *   @verbatim
        CSL_status        status;
        CSL_McaspHwSetup  myHwSetup;
        CSL_McaspHandle   hMcasp;

        myHwSetup.glb.ctl = 0x00000000;
        myHwSetup.glb.ditCtl = 0x00000003;
        myHwSetup.glb.dlbMode = 0x00000002;
        myHwSetup.glb.amute = 0x00001234;
        myHwSetup.glb.serSetup[1] = 0x00000012;
        myHwSetup.glb.serSetup[2] = 0x00000012;
        myHwSetup.rx.mask = 0x11111111;
        myHwSetup.tx.mask = 0x11111111;
        myHwSetup.rx.fmt = 0x00001111;
        myHwSetup.tx.fmt = 0x00001111;
        myHwSetup.rx.frSyncCtl = 0x00000001;
        myHwSetup.tx.frSyncCtl = 0x00000001;
        myHwSetup.rx.tdm = 0xFFFFFFFF;
        myHwSetup.tx.tdm = 0xFFFFFFFF;
        myHwSetup.rx.intCtl = 0x000000BF;
        myHwSetup.tx.intCtl = 0x000000BF;
        myHwSetup.rx.stat = 0x000001F7;
        myHwSetup.tx.stat = 0x000001F7;
        myHwSetup.rx.evtCtl = 0x00000000;
        myHwSetup.tx.evtCtl = 0x00000000;
        myHwSetup.rx.clk.clkSetupClk = 0x00000001;
        myHwSetup.tx.clk.clkSetupClk = 0x00000001;
        myHwSetup.rx.clk.clkSetupHiClk = 0x00000101;
        myHwSetup.tx.clk.clkSetupHiClk = 0x00000101;
        myHwSetup.rx.clk.clkChk = 0x00432100;
        myHwSetup.tx.clk.clkChk = 0x00432100;
        myHwSetup.emu = CSL_MCASP_PWRDEMU_FREE_ON;

        status = CSL_mcaspHwsetup (hMcasp, &myHwSetup);

     @endverbatim
 * ===========================================================================
 */ 
#pragma CODE_SECTION (CSL_mcaspHwSetup, ".text:csl_section:mcasp");

CSL_Status CSL_mcaspHwSetup (
        CSL_McaspHandle    hMcasp,
        CSL_McaspHwSetup   *myHwSetup
) 
{
    Uint32 serNum = 0;
    
    if (hMcasp == NULL) {
        return CSL_ESYS_BADHANDLE;
    }
            
    if (myHwSetup == NULL) {
        return CSL_ESYS_INVPARAMS;
    }        
    
    /* Reset McASP to default values by setting GBLCTL = 0 */ 
    hMcasp->regs->GBLCTL = 0x0000;

    /* Initialize the powerdown and emulation management register */ 
    CSL_FINS (hMcasp->regs->PWRDEMU, MCASP_PWRDEMU_FREE, myHwSetup->emu);

    /* Configure the RMASK register */ 
    hMcasp->regs->RMASK = (Uint32) myHwSetup->rx.mask;

    /* Configure RFMT */ 
    hMcasp->regs->RFMT = (Uint32) myHwSetup->rx.fmt;

    /* Configure AFSRCTL */ 
    hMcasp->regs->AFSRCTL = (Uint32) myHwSetup->rx.frSyncCtl;

    /* Configure ACLKRCTL */ 
    hMcasp->regs->ACLKRCTL = (Uint32) myHwSetup->rx.clk.clkSetupClk;

    /* Configure AHCLKRCTL */ 
    hMcasp->regs->AHCLKRCTL = (Uint32) myHwSetup->rx.clk.clkSetupHiClk;

    /* Configure RTDM */ 
    hMcasp->regs->RTDM = (Uint32) myHwSetup->rx.tdm;

    /* Configure RINTCTL */ 
    hMcasp->regs->RINTCTL = (Uint32) myHwSetup->rx.intCtl;

    /* Configure RCLKCHK */ 
    hMcasp->regs->RCLKCHK = (Uint32) myHwSetup->rx.clk.clkChk;

    /* Configure XMASK */ 
    hMcasp->regs->XMASK = (Uint32) myHwSetup->tx.mask;

    /* Configure XFMT */ 
    hMcasp->regs->XFMT = (Uint32) myHwSetup->tx.fmt;

    /* Configure AFSXCTL */ 
    hMcasp->regs->AFSXCTL = (Uint16) myHwSetup->tx.frSyncCtl;

    /* Configure ACLKXCTL */ 
    hMcasp->regs->ACLKXCTL = (Uint16) myHwSetup->tx.clk.clkSetupClk;

    /* Configure AHCLKXCTL */ 
    hMcasp->regs->AHCLKXCTL = (Uint16) myHwSetup->tx.clk.clkSetupHiClk;

    /* Configure XTDM */ 
    hMcasp->regs->XTDM = (Uint32) myHwSetup->tx.tdm;

    /* Configure XINTCTL */ 
    hMcasp->regs->XINTCTL = (Uint16) myHwSetup->tx.intCtl;

    /* Configure XCLKCHK */ 
    hMcasp->regs->XCLKCHK = (Uint32) myHwSetup->tx.clk.clkChk;
    
    /* Configure serializer control registers 0-5 for McASP 1 */ 
    while (serNum < hMcasp->numOfSerializers) {
        hMcasp->regs->SRCTL0 = (Uint16) myHwSetup->glb.serSetup[serNum];
        if (++serNum >= hMcasp->numOfSerializers)
            break;
        hMcasp->regs->SRCTL1 = (Uint16) myHwSetup->glb.serSetup[serNum];
        if (++serNum >= hMcasp->numOfSerializers)
            break;
        hMcasp->regs->SRCTL2 = (Uint16) myHwSetup->glb.serSetup[serNum];
        if (++serNum >= hMcasp->numOfSerializers)
            break;
        hMcasp->regs->SRCTL3 = (Uint16) myHwSetup->glb.serSetup[serNum];
        if (++serNum >= hMcasp->numOfSerializers)
            break;
        hMcasp->regs->SRCTL4 = (Uint16) myHwSetup->glb.serSetup[serNum];
        if (++serNum >= hMcasp->numOfSerializers)
            break;
        hMcasp->regs->SRCTL5 = (Uint16) myHwSetup->glb.serSetup[serNum];
        if (++serNum >= hMcasp->numOfSerializers)
            break;
        hMcasp->regs->SRCTL6 = (Uint16) myHwSetup->glb.serSetup[serNum];
        if (++serNum >= hMcasp->numOfSerializers)
            break;
        hMcasp->regs->SRCTL7 = (Uint16) myHwSetup->glb.serSetup[serNum];
        if (++serNum >= hMcasp->numOfSerializers)
            break;
        hMcasp->regs->SRCTL8 = (Uint16) myHwSetup->glb.serSetup[serNum];
        if (++serNum >= hMcasp->numOfSerializers)
            break;
        hMcasp->regs->SRCTL9 = (Uint16) myHwSetup->glb.serSetup[serNum];
        if (++serNum >= hMcasp->numOfSerializers)
            break;
        hMcasp->regs->SRCTL10 = (Uint16) myHwSetup->glb.serSetup[serNum];
        if (++serNum >= hMcasp->numOfSerializers)
            break;
        hMcasp->regs->SRCTL11 = (Uint16) myHwSetup->glb.serSetup[serNum];
        if (++serNum >= hMcasp->numOfSerializers)
            break;
        hMcasp->regs->SRCTL12 = (Uint16) myHwSetup->glb.serSetup[serNum];
        if (++serNum >= hMcasp->numOfSerializers)
            break;
        hMcasp->regs->SRCTL13 = (Uint16) myHwSetup->glb.serSetup[serNum];
        if (++serNum >= hMcasp->numOfSerializers)
            break;
        hMcasp->regs->SRCTL14 = (Uint16) myHwSetup->glb.serSetup[serNum];
        if (++serNum >= hMcasp->numOfSerializers)
            break;
        hMcasp->regs->SRCTL15 = (Uint16) myHwSetup->glb.serSetup[serNum];
        if (++serNum >= hMcasp->numOfSerializers)
            break;
    }

    /* Configure pin function register */ 
    hMcasp->regs->PFUNC = (Uint32) myHwSetup->glb.pfunc;

    /* Configure pin direction register */ 
    hMcasp->regs->PDIR = (Uint32) myHwSetup->glb.pdir;

    /* Configure DITCTL */ 
    if (hMcasp->ditStatus == TRUE) {
        hMcasp->regs->DITCTL = (Uint32) myHwSetup->glb.ditCtl;
    }

    /* Configure DLBCTL */ 
    hMcasp->regs->DLBCTL = (Uint32) myHwSetup->glb.dlbMode;

    /* Configure AMUTE */ 
    hMcasp->regs->AMUTE = (Uint32) myHwSetup->glb.amute;

    /* Configure RSTAT and XSTAT */ 
    hMcasp->regs->RSTAT = (Uint32) myHwSetup->rx.stat;
    hMcasp->regs->XSTAT = (Uint32) myHwSetup->tx.stat;

    /* Configure REVTCTL and XEVTCTL */ 
    hMcasp->regs->REVTCTL = (Uint32) myHwSetup->rx.evtCtl;
    hMcasp->regs->XEVTCTL = (Uint32) myHwSetup->tx.evtCtl;

    /* Initialize the global control register */ 
    hMcasp->regs->GBLCTL = (Uint32) myHwSetup->glb.ctl;
    
    return CSL_SOK;
}


