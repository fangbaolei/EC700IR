/** ===========================================================================
 *   @file  csl_mcaspGetHwSetup.c
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
 *   @n@b CSL_mcaspGetHwSetup
 *
 *   @b Description
 *   @n This function retrieves the hardware setup parameters. 
 *
 *   @b Arguments
 *   @verbatim
            hMcasp          Handle to the McASP instance
 
            myHwSetup       Pointer to hardware setup structure 
 
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK             - Hardware setup retrieved
 *                                                successfully
 *   @li                    CSL_ESYS_BADHANDLE  - Invalid handle
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  The hardware set up structure will be populated with values from 
 *       the registers
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
        CSL_status        status;
        CSL_McaspHwSetup  myHwSetup;
        CSL_McaspHandle   hMcasp;
        
        status = CSL_mcaspGetHwsetup (hMcasp, &myHwSetup);
            
     @endverbatim
 * ============================================================================
 */ 
#pragma CODE_SECTION (CSL_mcaspGetHwSetup, ".text:csl_section:mcasp");
CSL_Status CSL_mcaspGetHwSetup (
    CSL_McaspHandle    hMcasp,
    CSL_McaspHwSetup   *myHwSetup
) 
{
    Uint32 serNum = 0;
    
    if (hMcasp == NULL) {
        return CSL_ESYS_BADHANDLE;
    }
    
    /* Read pin function register configuration */ 
    myHwSetup->glb.pfunc = hMcasp->regs->PFUNC;;

    /* Read pin direction register configuration */ 
    myHwSetup->glb.pdir = hMcasp->regs->PDIR;

    /* Read global control register configuration */ 
    myHwSetup->glb.ctl = hMcasp->regs->GBLCTL;

    /* Read DIT mode control register configuration */ 
    myHwSetup->glb.ditCtl = hMcasp->regs->DITCTL;

    /* Read digital loopback control register configuration */ 
    myHwSetup->glb.dlbMode = hMcasp->regs->DLBCTL;

    /* Read audio mute control register configuration */ 
    myHwSetup->glb.amute = hMcasp->regs->AMUTE;
    
        /* Read Serializer control register configuration */ 
    while (serNum < hMcasp->numOfSerializers) {
        myHwSetup->glb.serSetup[serNum] = hMcasp->regs->SRCTL0;
        if (++serNum >= hMcasp->numOfSerializers)
            break;
        myHwSetup->glb.serSetup[serNum] = hMcasp->regs->SRCTL1;
        if (++serNum >= hMcasp->numOfSerializers)
            break;
        myHwSetup->glb.serSetup[serNum] = hMcasp->regs->SRCTL2;
        if (++serNum >= hMcasp->numOfSerializers)
            break;
        myHwSetup->glb.serSetup[serNum] = hMcasp->regs->SRCTL3;
        if (++serNum >= hMcasp->numOfSerializers)
            break;
        myHwSetup->glb.serSetup[serNum] = hMcasp->regs->SRCTL4;
        if (++serNum >= hMcasp->numOfSerializers)
            break;
        myHwSetup->glb.serSetup[serNum] = hMcasp->regs->SRCTL5;
        if (++serNum >= hMcasp->numOfSerializers)
            break;
        myHwSetup->glb.serSetup[serNum] = hMcasp->regs->SRCTL6;
        if (++serNum >= hMcasp->numOfSerializers)
            break;
        myHwSetup->glb.serSetup[serNum] = hMcasp->regs->SRCTL7;
        if (++serNum >= hMcasp->numOfSerializers)
            break;
        myHwSetup->glb.serSetup[serNum] = hMcasp->regs->SRCTL8;
        if (++serNum >= hMcasp->numOfSerializers)
            break;
        myHwSetup->glb.serSetup[serNum] = hMcasp->regs->SRCTL9;
        if (++serNum >= hMcasp->numOfSerializers)
            break;
        myHwSetup->glb.serSetup[serNum] = hMcasp->regs->SRCTL10;
        if (++serNum >= hMcasp->numOfSerializers)
            break;
        myHwSetup->glb.serSetup[serNum] = hMcasp->regs->SRCTL11;
        if (++serNum >= hMcasp->numOfSerializers)
            break;
        myHwSetup->glb.serSetup[serNum] = hMcasp->regs->SRCTL12;
        if (++serNum >= hMcasp->numOfSerializers)
            break;
        myHwSetup->glb.serSetup[serNum] = hMcasp->regs->SRCTL13;
        if (++serNum >= hMcasp->numOfSerializers)
            break;
        myHwSetup->glb.serSetup[serNum] = hMcasp->regs->SRCTL14;
        if (++serNum >= hMcasp->numOfSerializers)
            break;
        myHwSetup->glb.serSetup[serNum] = hMcasp->regs->SRCTL15;
        if (++serNum >= hMcasp->numOfSerializers)
            break;
    }
    
    /* Read receive/transmit format unit bit mask register configuration */ 
    myHwSetup->rx.mask = hMcasp->regs->RMASK;
    myHwSetup->tx.mask = hMcasp->regs->XMASK;

    /* Read receive/transmit bit stream format register configuration */ 
    myHwSetup->rx.fmt = hMcasp->regs->RFMT;
    myHwSetup->tx.fmt = hMcasp->regs->XFMT;

    /* Read receive/transmit frame sync control register configuration */ 
    myHwSetup->rx.frSyncCtl = hMcasp->regs->AFSRCTL;
    myHwSetup->tx.frSyncCtl = hMcasp->regs->AFSXCTL;

    /* Read receive/transmit TDM time slot 0-31 register configuration */ 
    myHwSetup->rx.tdm = hMcasp->regs->RTDM;
    myHwSetup->tx.tdm = hMcasp->regs->XTDM;

    /* Read receiver/transmitter interrupt control register configuration */ 
    myHwSetup->rx.intCtl = hMcasp->regs->RINTCTL;
    myHwSetup->tx.intCtl = hMcasp->regs->XINTCTL;

    /* Read receiver/transmitter status register configuration */ 
    myHwSetup->rx.stat = hMcasp->regs->RSTAT;
    myHwSetup->tx.stat = hMcasp->regs->XSTAT;

    /* Read receiver/transmitter DMA event control register configuration */ 
    myHwSetup->rx.evtCtl = hMcasp->regs->REVTCTL;
    myHwSetup->tx.evtCtl = hMcasp->regs->XEVTCTL;

    /* Read receive/transmit clock control register configuration */ 
    myHwSetup->rx.clk.clkSetupClk = hMcasp->regs->ACLKRCTL;
    myHwSetup->tx.clk.clkSetupClk = hMcasp->regs->ACLKXCTL;

    /* 
     * Read receive/transmit high-frequency clock control register
     * configuration
     */ 
    myHwSetup->rx.clk.clkSetupHiClk = hMcasp->regs->AHCLKRCTL;
    myHwSetup->tx.clk.clkSetupHiClk = hMcasp->regs->AHCLKXCTL;

    /* Read receive/transmit clock check control register configuration */ 
    myHwSetup->rx.clk.clkChk = hMcasp->regs->RCLKCHK;
    myHwSetup->tx.clk.clkChk = hMcasp->regs->XCLKCHK;

    /* Read power down and emulation management register configuration */ 
    myHwSetup->emu = CSL_FEXT (hMcasp->regs->PWRDEMU, MCASP_PWRDEMU_FREE);
        
    return CSL_SOK;
}


