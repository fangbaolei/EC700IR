/** @file csl_emifHwSetup.c
 *
 *  @brief    File for functional layer of CSL API @a CSL_emifHwSetup()
 *
 *  Description
 *    - The @a CSL_emifHwSetup() function definition & it's associated
 *      functions
 *
 *  Modification 1
 *    - Modified on: 23/6/2004
 *    - Reason: created the sources
 *
 *  @date 23rd June, 2004
 *  @author Santosh Narayanan.
 */

#include <csl_emif.h>

#pragma CODE_SECTION (CSL_emifHwSetup, ".text:csl_section:emif");

/** @brief Configures the EMIF using the values passed in through the
 *  setup structure. 
 */
CSL_Status  CSL_emifHwSetup(
    /** Pointer to the object that holds reference to the
     *  instance of EMIF requested after the call 
	 */
    CSL_EmifHandle                         hEmif,
    /** Pointer to setup structure which contains the
     *  information to program EMIF to a useful state 
	 */
    CSL_EmifHwSetup                        *setup
	){

    CSL_Status status = CSL_SOK;

	/*  Setup the configuration parameters in the Async Wait Cycle Config Register */

      hEmif->regs->AWCCR = (hEmif->regs->AWCCR & 0x0F00FF00) | (CSL_FMK(EMIF_AWCCR_WP3, setup->asyncWaitCycleConfig->wp3)
                                              |CSL_FMK(EMIF_AWCCR_WP2, setup->asyncWaitCycleConfig->wp2)
                                              |CSL_FMK(EMIF_AWCCR_WP1, setup->asyncWaitCycleConfig->wp1)
                                              |CSL_FMK(EMIF_AWCCR_WP0, setup->asyncWaitCycleConfig->wp0)
                                              |CSL_FMK(EMIF_AWCCR_CS3WAIT, setup->asyncWaitCycleConfig->cs3Wait)
                                              |CSL_FMK(EMIF_AWCCR_CS2WAIT, setup->asyncWaitCycleConfig->cs2Wait)
                                              |CSL_FMK(EMIF_AWCCR_CS1WAIT, setup->asyncWaitCycleConfig->cs1Wait)
                                              |CSL_FMK(EMIF_AWCCR_CS0WAIT, setup->asyncWaitCycleConfig->cs0Wait)
                                              |CSL_FMK(EMIF_AWCCR_MEWC, setup->asyncWaitCycleConfig->maxExtWait));

	  /* Setup the SDRAM Refresh Control Register */


	hEmif->regs->SDRCR = (hEmif->regs->SDRCR & 0xFFF8E000) |( CSL_FMK(EMIF_SDRCR_DDRRT, setup->emifSdramRefreshControl->ddrRefreshThresh)
	                                      |CSL_FMK(EMIF_SDRCR_RR, setup->emifSdramRefreshControl->refreshRate));

	/* Setup the SDRAM Timing Register */

	 hEmif->regs->SDTIMR = (hEmif->regs->SDTIMR & 0x00880087) | (CSL_FMK(EMIF_SDTIMR_T_RFC, setup->sdramTiming->trfc)
                                              |CSL_FMK(EMIF_SDTIMR_T_RP, setup->sdramTiming->trp)
                                              |CSL_FMK(EMIF_SDTIMR_T_RCD, setup->sdramTiming->trcd)
                                              |CSL_FMK(EMIF_SDTIMR_T_WR, setup->sdramTiming->twr)
                                              |CSL_FMK(EMIF_SDTIMR_T_RAS, setup->sdramTiming->tras)
                                              |CSL_FMK(EMIF_SDTIMR_T_RC, setup->sdramTiming->trc)
                                              |CSL_FMK(EMIF_SDTIMR_T_RRD, setup->sdramTiming->trrd));

       hEmif->regs->SDSRETR = (hEmif->regs->SDSRETR & 0xFFFFFFE0) |(CSL_FMK(EMIF_SDSRETR_T_XS,  setup->sdramTiming->txs));

        /* Setup the SDRAM Bank Configuration */
    	  
         hEmif->regs->SDBCR = (hEmif->regs->SDBCR & 0x1FFC8080) | (CSL_FMK(EMIF_SDBCR_SR, setup->sdramBankConfig->selfRefresh)
                                              |CSL_FMK(EMIF_SDBCR_PD,  setup->sdramBankConfig->powerDown)
                                              |CSL_FMK(EMIF_SDBCR_PDWR,  setup->sdramBankConfig->pdwr)
                                              |CSL_FMK(EMIF_SDBCR_DDRDRST,  setup->sdramBankConfig->ddrDrSt)
                                              |CSL_FMK(EMIF_SDBCR_BIT17LOCK,  setup->sdramBankConfig->bit17Enable)
                                              |CSL_FMK(EMIF_SDBCR_NM,  setup->sdramBankConfig->narrowMode)
                                              |CSL_FMK(EMIF_SDBCR_DISDDRDLL,  setup->sdramBankConfig->disableDdrDll)
                                              |CSL_FMK(EMIF_SDBCR_BIT13LOCK,  setup->sdramBankConfig->bit13Enable)
                                              |CSL_FMK(EMIF_SDBCR_CL,  setup->sdramBankConfig->casLatency)
                                              |CSL_FMK(EMIF_SDBCR_BIT11_9LOCK,  setup->sdramBankConfig->bit911Enable)
                                              |CSL_FMK(EMIF_SDBCR_IBANK,  setup->sdramBankConfig->intBank)
                                              |CSL_FMK(EMIF_SDBCR_EBANK,  setup->sdramBankConfig->extBank)
                                              |CSL_FMK(EMIF_SDBCR_PAGESIZE,  setup->sdramBankConfig->pageSize));

       /* Setup the Async Bank Registers */

	  hEmif->regs->AB1CR =  (CSL_FMK(EMIF_AB1CR_SS, setup->asyncBank1Config->selectStrobe)
                                              |CSL_FMK(EMIF_AB1CR_EW, setup->asyncBank1Config->extWait)
                                              |CSL_FMK(EMIF_AB1CR_W_SETUP, setup->asyncBank1Config->writeSetup)
                                              |CSL_FMK(EMIF_AB1CR_W_STROBE, setup->asyncBank1Config->writeStrobe)
                                              |CSL_FMK(EMIF_AB1CR_W_HOLD, setup->asyncBank1Config->writeHold)
                                              |CSL_FMK(EMIF_AB1CR_R_SETUP, setup->asyncBank1Config->readSetup)
                                              |CSL_FMK(EMIF_AB1CR_R_STROBE, setup->asyncBank1Config->readStrobe)
                                              |CSL_FMK(EMIF_AB1CR_R_HOLD, setup->asyncBank1Config->readHold)
                                              |CSL_FMK(EMIF_AB1CR_TA, setup->asyncBank1Config->turnAround)
                                              |CSL_FMK(EMIF_AB1CR_ASIZE, setup->asyncBank1Config->asyncSize));

        hEmif->regs->AB2CR =  (CSL_FMK(EMIF_AB2CR_SS, setup->asyncBank2Config->selectStrobe)
                                              |CSL_FMK(EMIF_AB2CR_EW, setup->asyncBank2Config->extWait)
                                              |CSL_FMK(EMIF_AB2CR_W_SETUP, setup->asyncBank2Config->writeSetup)
                                              |CSL_FMK(EMIF_AB2CR_W_STROBE, setup->asyncBank2Config->writeStrobe)
                                              |CSL_FMK(EMIF_AB2CR_W_HOLD, setup->asyncBank2Config->writeHold)
                                              |CSL_FMK(EMIF_AB2CR_R_SETUP, setup->asyncBank2Config->readSetup)
                                              |CSL_FMK(EMIF_AB2CR_R_STROBE, setup->asyncBank2Config->readStrobe)
                                              |CSL_FMK(EMIF_AB2CR_R_HOLD, setup->asyncBank2Config->readHold)
                                              |CSL_FMK(EMIF_AB2CR_TA, setup->asyncBank2Config->turnAround)
                                              |CSL_FMK(EMIF_AB2CR_ASIZE, setup->asyncBank2Config->asyncSize));
  
      	 hEmif->regs->AB3CR =  (CSL_FMK(EMIF_AB3CR_SS, setup->asyncBank3Config->selectStrobe)
                                              |CSL_FMK(EMIF_AB3CR_EW, setup->asyncBank3Config->extWait)
                                              |CSL_FMK(EMIF_AB3CR_W_SETUP, setup->asyncBank3Config->writeSetup)
                                              |CSL_FMK(EMIF_AB3CR_W_STROBE, setup->asyncBank3Config->writeStrobe)
                                              |CSL_FMK(EMIF_AB3CR_W_HOLD, setup->asyncBank3Config->writeHold)
                                              |CSL_FMK(EMIF_AB3CR_R_SETUP, setup->asyncBank3Config->readSetup)
                                              |CSL_FMK(EMIF_AB3CR_R_STROBE, setup->asyncBank3Config->readStrobe)
                                              |CSL_FMK(EMIF_AB3CR_R_HOLD, setup->asyncBank3Config->readHold)
                                              |CSL_FMK(EMIF_AB3CR_TA, setup->asyncBank3Config->turnAround)
                                              |CSL_FMK(EMIF_AB3CR_ASIZE, setup->asyncBank3Config->asyncSize));

        hEmif->regs->AB4CR =  (CSL_FMK(EMIF_AB4CR_SS, setup->asyncBank4Config->selectStrobe)
                                              |CSL_FMK(EMIF_AB4CR_EW, setup->asyncBank4Config->extWait)
                                              |CSL_FMK(EMIF_AB4CR_W_SETUP, setup->asyncBank4Config->writeSetup)
                                              |CSL_FMK(EMIF_AB4CR_W_STROBE, setup->asyncBank4Config->writeStrobe)
                                              |CSL_FMK(EMIF_AB4CR_W_HOLD, setup->asyncBank4Config->writeHold)
                                              |CSL_FMK(EMIF_AB4CR_R_SETUP, setup->asyncBank4Config->readSetup)
                                              |CSL_FMK(EMIF_AB4CR_R_STROBE, setup->asyncBank4Config->readStrobe)
                                              |CSL_FMK(EMIF_AB4CR_R_HOLD, setup->asyncBank4Config->readHold)
                                              |CSL_FMK(EMIF_AB4CR_TA, setup->asyncBank4Config->turnAround)
                                              |CSL_FMK(EMIF_AB4CR_ASIZE, setup->asyncBank4Config->asyncSize));


	  /* Setup the DDR PHY Control Register */

	  hEmif->regs->DDRPHYCR =  setup->ddrPhyControl;

	  /* Setup the NAND FLASH Control configuration */

	  hEmif->regs->NANDFCR = (hEmif->regs->NANDFCR & 0xFFFFFFF0) | (CSL_FMK(EMIF_NANDFCR_CS5NAND, setup ->nandFlashControl->cs5nand)
	                                            |CSL_FMK(EMIF_NANDFCR_CS4NAND, setup ->nandFlashControl->cs4nand)
	                                            |CSL_FMK(EMIF_NANDFCR_CS3NAND, setup ->nandFlashControl->cs3nand)
	                                            |CSL_FMK(EMIF_NANDFCR_CS2NAND, setup ->nandFlashControl->cs2nand));
	
    return status;
}

/* Rev.No.   Date/Time               ECN No.          Modifier      */
/* -------   ---------               -------          --------      */

/* 3          Aug 10:00:34 9         2271             xkeshavm      */
/*                                                                  */
/* To upload API CSL 0.51 Release                                   */
/********************************************************************/ 
