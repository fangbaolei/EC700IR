/** @file csl_emifGetHwSetup.c
 *
 *    @brief    File for functional layer of CSL API @a CSL_emifGetHwSetup()
 *
 *  Description
 *    - The @a CSL_emifGetHwSetup() function definition & it's associated
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

#pragma CODE_SECTION (CSL_emifGetHwSetup, ".text:csl_section:emif")

/** @brief Gets the current setup of EMIF
 */
CSL_Status  CSL_emifGetHwSetup(
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

      /* Get the Async Bank 1 setup */
	  
	setup->asyncBank1Config->selectStrobe = CSL_FEXT(hEmif->regs->AB1CR, EMIF_AB1CR_SS);
    setup->asyncBank1Config->extWait = CSL_FEXT(hEmif->regs->AB1CR, EMIF_AB1CR_EW);
	setup->asyncBank1Config->writeSetup = CSL_FEXT(hEmif->regs->AB1CR, EMIF_AB1CR_W_SETUP);
	setup->asyncBank1Config->writeStrobe = CSL_FEXT(hEmif->regs->AB1CR, EMIF_AB1CR_W_STROBE);
	setup->asyncBank1Config->writeHold = CSL_FEXT(hEmif->regs->AB1CR, EMIF_AB1CR_W_HOLD);
    setup->asyncBank1Config->readSetup = CSL_FEXT(hEmif->regs->AB1CR, EMIF_AB1CR_R_SETUP);
	setup->asyncBank1Config->readStrobe = CSL_FEXT(hEmif->regs->AB1CR, EMIF_AB1CR_R_STROBE);
	setup->asyncBank1Config->readHold = CSL_FEXT(hEmif->regs->AB1CR, EMIF_AB1CR_R_HOLD);
       setup->asyncBank1Config->turnAround = CSL_FEXT(hEmif->regs->AB1CR, EMIF_AB1CR_TA);
	setup->asyncBank1Config->asyncSize = CSL_FEXT(hEmif->regs->AB1CR, EMIF_AB1CR_ASIZE);
	   
      /* Get the Async Bank 2 setup */
	  
	setup->asyncBank2Config->selectStrobe = CSL_FEXT(hEmif->regs->AB2CR, EMIF_AB2CR_SS);
       setup->asyncBank2Config->extWait = CSL_FEXT(hEmif->regs->AB2CR, EMIF_AB2CR_EW);
	setup->asyncBank2Config->writeSetup = CSL_FEXT(hEmif->regs->AB2CR, EMIF_AB2CR_W_SETUP);
	setup->asyncBank2Config->writeStrobe = CSL_FEXT(hEmif->regs->AB2CR, EMIF_AB2CR_W_STROBE);
	setup->asyncBank2Config->writeHold = CSL_FEXT(hEmif->regs->AB2CR, EMIF_AB2CR_W_HOLD);
       setup->asyncBank2Config->readSetup = CSL_FEXT(hEmif->regs->AB2CR, EMIF_AB2CR_R_SETUP);
	setup->asyncBank2Config->readStrobe = CSL_FEXT(hEmif->regs->AB2CR, EMIF_AB2CR_R_STROBE);
	setup->asyncBank2Config->readHold = CSL_FEXT(hEmif->regs->AB2CR, EMIF_AB2CR_R_HOLD);
       setup->asyncBank2Config->turnAround = CSL_FEXT(hEmif->regs->AB2CR, EMIF_AB2CR_TA);
	setup->asyncBank2Config->asyncSize = CSL_FEXT(hEmif->regs->AB2CR, EMIF_AB2CR_ASIZE);
	
      /* Get the Async Bank 3 setup */
	  
	setup->asyncBank3Config->selectStrobe = CSL_FEXT(hEmif->regs->AB3CR, EMIF_AB3CR_SS);
       setup->asyncBank3Config->extWait = CSL_FEXT(hEmif->regs->AB3CR, EMIF_AB3CR_EW);
	setup->asyncBank3Config->writeSetup = CSL_FEXT(hEmif->regs->AB3CR, EMIF_AB3CR_W_SETUP);
	setup->asyncBank3Config->writeStrobe = CSL_FEXT(hEmif->regs->AB3CR, EMIF_AB3CR_W_STROBE);
	setup->asyncBank3Config->writeHold = CSL_FEXT(hEmif->regs->AB3CR, EMIF_AB3CR_W_HOLD);
       setup->asyncBank3Config->readSetup = CSL_FEXT(hEmif->regs->AB3CR, EMIF_AB3CR_R_SETUP);
	setup->asyncBank3Config->readStrobe = CSL_FEXT(hEmif->regs->AB3CR, EMIF_AB3CR_R_STROBE);
	setup->asyncBank3Config->readHold = CSL_FEXT(hEmif->regs->AB3CR, EMIF_AB3CR_R_HOLD);
       setup->asyncBank3Config->turnAround = CSL_FEXT(hEmif->regs->AB3CR, EMIF_AB3CR_TA);
	setup->asyncBank3Config->asyncSize = CSL_FEXT(hEmif->regs->AB3CR, EMIF_AB3CR_ASIZE);
	   
      /* Get the Async Bank 4 setup */
	  
	setup->asyncBank4Config->selectStrobe = CSL_FEXT(hEmif->regs->AB4CR, EMIF_AB4CR_SS);
       setup->asyncBank4Config->extWait = CSL_FEXT(hEmif->regs->AB4CR, EMIF_AB4CR_EW);
	setup->asyncBank4Config->writeSetup = CSL_FEXT(hEmif->regs->AB4CR, EMIF_AB4CR_W_SETUP);
	setup->asyncBank4Config->writeStrobe = CSL_FEXT(hEmif->regs->AB4CR, EMIF_AB4CR_W_STROBE);
	setup->asyncBank4Config->writeHold = CSL_FEXT(hEmif->regs->AB4CR, EMIF_AB4CR_W_HOLD);
       setup->asyncBank4Config->readSetup = CSL_FEXT(hEmif->regs->AB4CR, EMIF_AB4CR_R_SETUP);
	setup->asyncBank4Config->readStrobe = CSL_FEXT(hEmif->regs->AB4CR, EMIF_AB4CR_R_STROBE);
	setup->asyncBank4Config->readHold = CSL_FEXT(hEmif->regs->AB4CR, EMIF_AB4CR_R_HOLD);
       setup->asyncBank4Config->turnAround = CSL_FEXT(hEmif->regs->AB4CR, EMIF_AB4CR_TA);
	setup->asyncBank4Config->asyncSize = CSL_FEXT(hEmif->regs->AB4CR, EMIF_AB4CR_ASIZE);
	
       /* Get the Async Wait Cycle Configuration */

       setup->asyncWaitCycleConfig->wp3 = CSL_FEXT(hEmif->regs->AWCCR, EMIF_AWCCR_WP3);
       setup->asyncWaitCycleConfig->wp2 = CSL_FEXT(hEmif->regs->AWCCR, EMIF_AWCCR_WP2);
       setup->asyncWaitCycleConfig->wp1 = CSL_FEXT(hEmif->regs->AWCCR, EMIF_AWCCR_WP1);
       setup->asyncWaitCycleConfig->wp0 = CSL_FEXT(hEmif->regs->AWCCR, EMIF_AWCCR_WP0);
       setup->asyncWaitCycleConfig->cs3Wait = CSL_FEXT(hEmif->regs->AWCCR, EMIF_AWCCR_CS3WAIT);
	setup->asyncWaitCycleConfig->cs2Wait = CSL_FEXT(hEmif->regs->AWCCR, EMIF_AWCCR_CS2WAIT);   
       setup->asyncWaitCycleConfig->cs1Wait = CSL_FEXT(hEmif->regs->AWCCR, EMIF_AWCCR_CS1WAIT);
	setup->asyncWaitCycleConfig->cs0Wait = CSL_FEXT(hEmif->regs->AWCCR, EMIF_AWCCR_CS0WAIT);   
       setup->asyncWaitCycleConfig->maxExtWait = CSL_FEXT(hEmif->regs->AWCCR, EMIF_AWCCR_MEWC);   

	/* Get the SDRAM Bank Configuration */

	setup->sdramBankConfig->selfRefresh = CSL_FEXT(hEmif->regs->SDBCR, EMIF_SDBCR_SR);   
	setup->sdramBankConfig->powerDown = CSL_FEXT(hEmif->regs->SDBCR, EMIF_SDBCR_PD);
       setup->sdramBankConfig->pdwr = CSL_FEXT(hEmif->regs->SDBCR, EMIF_SDBCR_PDWR);   
	setup->sdramBankConfig->ddrDrSt = CSL_FEXT(hEmif->regs->SDBCR, EMIF_SDBCR_DDRDRST);   
	setup->sdramBankConfig->bit17Enable = CSL_FEXT(hEmif->regs->SDBCR, EMIF_SDBCR_BIT17LOCK);   
	setup->sdramBankConfig->narrowMode = CSL_FEXT(hEmif->regs->SDBCR, EMIF_SDBCR_NM);
	setup->sdramBankConfig->disableDdrDll = CSL_FEXT(hEmif->regs->SDBCR, EMIF_SDBCR_DISDDRDLL);   
	setup->sdramBankConfig->bit13Enable = CSL_FEXT(hEmif->regs->SDBCR, EMIF_SDBCR_BIT13LOCK);
       setup->sdramBankConfig->casLatency = CSL_FEXT(hEmif->regs->SDBCR, EMIF_SDBCR_CL);   
	setup->sdramBankConfig->bit911Enable = CSL_FEXT(hEmif->regs->SDBCR, EMIF_SDBCR_BIT11_9LOCK);
	setup->sdramBankConfig->intBank = CSL_FEXT(hEmif->regs->SDBCR, EMIF_SDBCR_IBANK);
       setup->sdramBankConfig->extBank = CSL_FEXT(hEmif->regs->SDBCR, EMIF_SDBCR_EBANK);   
	setup->sdramBankConfig->pageSize = CSL_FEXT(hEmif->regs->SDBCR, EMIF_SDBCR_PAGESIZE);
	
	/* Get the SDRAM Refresh Control Configuration */

	setup->emifSdramRefreshControl->ddrRefreshThresh = CSL_FEXT(hEmif->regs->SDRCR, EMIF_SDRCR_DDRRT);
	setup->emifSdramRefreshControl->refreshRate = CSL_FEXT(hEmif->regs->SDRCR, EMIF_SDRCR_RR);

	/* Get the SDRAM Timing Configuration */

	setup->sdramTiming->trfc = CSL_FEXT(hEmif->regs->SDTIMR, EMIF_SDTIMR_T_RFC);
	setup->sdramTiming->trp = CSL_FEXT(hEmif->regs->SDTIMR, EMIF_SDTIMR_T_RP);
       setup->sdramTiming->trcd = CSL_FEXT(hEmif->regs->SDTIMR, EMIF_SDTIMR_T_RCD);
	setup->sdramTiming->twr = CSL_FEXT(hEmif->regs->SDTIMR, EMIF_SDTIMR_T_WR);
       setup->sdramTiming->tras = CSL_FEXT(hEmif->regs->SDTIMR, EMIF_SDTIMR_T_RAS);
       setup->sdramTiming->trc = CSL_FEXT(hEmif->regs->SDTIMR, EMIF_SDTIMR_T_RC);
	setup->sdramTiming->trrd = CSL_FEXT(hEmif->regs->SDTIMR, EMIF_SDTIMR_T_RRD);
	setup->sdramTiming->txs = CSL_FEXT(hEmif->regs->SDSRETR, EMIF_SDSRETR_T_XS);

       /* Get the DDR PHY Control Configuration */

	setup->ddrPhyControl = CSL_FEXT(hEmif->regs->DDRPHYCR, EMIF_DDRPHYCR_DDRPHYCTRL);

	/* Get the NAND FLASH Control Configuration */

	setup->nandFlashControl->cs5nand = CSL_FEXT(hEmif->regs->NANDFCR, EMIF_NANDFCR_CS5NAND);
      	setup->nandFlashControl->cs4nand = CSL_FEXT(hEmif->regs->NANDFCR, EMIF_NANDFCR_CS4NAND);
	setup->nandFlashControl->cs3nand = CSL_FEXT(hEmif->regs->NANDFCR, EMIF_NANDFCR_CS3NAND);	
	setup->nandFlashControl->cs2nand = CSL_FEXT(hEmif->regs->NANDFCR, EMIF_NANDFCR_CS2NAND);

	return status;
}

/* Rev.No.   Date/Time               ECN No.          Modifier      */
/* -------   ---------               -------          --------      */

/* 3          Aug 10:00:32 9         2271             xkeshavm      */
/*                                                                  */
/* To upload API CSL 0.51 Release                                   */
/********************************************************************/ 
