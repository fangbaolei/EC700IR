/*   ==========================================================================
 *   Copyright (c) Texas Instruments Inc , 2004
 *
 *   Use of this software is controlled by the terms and conditions found
 *   in the license agreement under which this software has been supplied
 *   provided
 *   ==========================================================================
*/
/** @file CSL_ddrHwSetup.c
 *
 *  @brief    File for functional layer of CSL API @a CSL_ddrHwSetup()
 *
 *  Description
 *   - The @a CSL_ddrHwSetup() function definition & it's associated functions
 *
 *  @date   29 June, 2004
 *  @author Pratheesh Gangadhar
 */

#include <csl_ddr.h>

#pragma CODE_SECTION (CSL_ddrHwSetup, ".text:csl_section:ddr");
/**

    \brief Setup DDR module for operation

    \param hDdr  Pointer to the object that holds reference to the instance
                 of DDR requested
    \param setup  Pointer to setup structure which contains the information 
                  to program DDR to a useful state		  
    \return if success, \c CSL_SOK, else error code 

    \see 
*/

CSL_Status CSL_ddrHwSetup ( CSL_DdrHandle hDdr,
                            CSL_DdrHwSetup *setup ) {
  CSL_DdrRegsOvly ddrRegs = hDdr->regs;   
  CSL_Status status = CSL_SOK; 	
  CSL_DdrSdramBankConfig *bankConfig;
  CSL_DdrSdramRefreshControl *refreshCtrl;
  CSL_DdrSdramTiming *sdramTiming;
  CSL_DdrAsyncWaitConfig *asyncWait;
  CSL_DdrAsyncMemConfig   *asyncMemConfig;
  CSL_DdrSyncBurstSramConfig   *sbsramConfig;
  CSL_DdrVbusmBurstPriority *burstPriority;
  CSL_DdrPerfCountConfig    *perfConfig;
  CSL_DdrVbusmConfig        *vbusmConfig;
  Uint32 tempReg;
  if (setup == NULL) {
    return CSL_ESYS_INVPARAMS;
  }
  bankConfig = &(setup->bankConfig);
  tempReg = bankConfig->bootUnlockConfig;
  if (tempReg & CSL_DDR_BANKCONFIG_BOOTUNLOCK ) {
    tempReg &= ~CSL_DDR_BANKCONFIG_BOOTUNLOCK;
    CSL_FINS (ddrRegs->SDBCR, DDR_SDBCR_BOOTUNLOCK, 1); 
    ddrRegs->SDBCR = tempReg;
	  
  }	  
  if (bankConfig->timingControl == CSL_DDR_TIMUNLOCKCTL_ENABLE) {
    tempReg =  CSL_FMK (DDR_SDBCR_TIMUNLOCK, 1) |
               CSL_FMK (DDR_SDBCR_NM, bankConfig->narrowModeCtrl) |
               CSL_FMK (DDR_SDBCR_CL, bankConfig->casLatency);
    CSL_FINSR (ddrRegs->SDBCR, 15, 9, tempReg >> 9);

    sdramTiming =  &(setup->sdramTiming);
  
    ddrRegs->SDTIMR = CSL_FMK (DDR_SDTIMR_T_RFC, sdramTiming->rfc) |
                      CSL_FMK (DDR_SDTIMR_T_RP, sdramTiming->rp) |
                      CSL_FMK (DDR_SDTIMR_T_RCD, sdramTiming->rcd) |
                      CSL_FMK (DDR_SDTIMR_T_WR, sdramTiming->wr) |
		      CSL_FMK (DDR_SDTIMR_T_RAS, sdramTiming->ras) |
                      CSL_FMK (DDR_SDTIMR_T_RC, sdramTiming->rc) |
                      CSL_FMK (DDR_SDTIMR_T_RRD, sdramTiming->rrd) | 
                      CSL_FMK (DDR_SDTIMR_T_WTR, sdramTiming->wtr);
                     
    ddrRegs->SDTIMR2 = CSL_FMK (DDR_SDTIMR2_T_ODT, sdramTiming->odt) |
                       CSL_FMK (DDR_SDTIMR2_T_SXNR, sdramTiming->sxnr) |
                     CSL_FMK (DDR_SDTIMR2_T_SXRD, sdramTiming->sxrd) |
                     CSL_FMK (DDR_SDTIMR2_T_RTP, sdramTiming->rtp) |
                     CSL_FMK (DDR_SDTIMR2_T_CKE, sdramTiming->cke);
    CSL_FINS (ddrRegs->SDBCR, DDR_SDBCR_TIMUNLOCK, 0);

  } else {
    CSL_FINS (ddrRegs->SDBCR, DDR_SDBCR_NM, bankConfig->narrowModeCtrl);
  }	  
             
  
  tempReg = CSL_FMK (DDR_SDBCR_IBANK, bankConfig->internBank) |
            CSL_FMK (DDR_SDBCR_EBANK, bankConfig->externBank) |
            CSL_FMK (DDR_SDBCR_PAGESIZE, bankConfig->pageSize);

  CSL_FINSR (ddrRegs->SDBCR, 6, 0, tempReg);

       
  refreshCtrl   = &(setup->refreshCtrl);
  ddrRegs->SDRCR = CSL_FMK (DDR_SDRCR_SR, refreshCtrl->selfRefresh) |
                   CSL_FMK (DDR_SDRCR_PEN, refreshCtrl->parityEn) |
                   CSL_FMK (DDR_SDRCR_RR, refreshCtrl->refreshRate);
                       
                    
  asyncWait = & (setup->asyncWait);
  
  ddrRegs->AWCCR = CSL_FMK (DDR_AWCCR_WP, asyncWait->waitPolarity) |
                   CSL_FMK (DDR_AWCCR_TA, asyncWait->turnAround) |
                   CSL_FMK (DDR_AWCCR_MEWC, asyncWait->maxExtWait);
  vbusmConfig   = & (setup->vbusmConfig);
  
  ddrRegs->VBCFG1 = CSL_FMK (DDR_VBCFG1_BUS, vbusmConfig->busWidth) |
                    CSL_FMK (DDR_VBCFG1_STATFIFO, vbusmConfig->statFifoDepth) |
                    CSL_FMK (DDR_VBCFG1_WRFIFO, vbusmConfig->writeFifoDepth) |
                    CSL_FMK (DDR_VBCFG1_CMDFIFO, vbusmConfig->cmdFifoDepth);
  
  ddrRegs->VBCFG2 = CSL_FMK (DDR_VBCFG2_RREGFIFO, 
                                              vbusmConfig->regReadFifoDepth) |
                    CSL_FMK (DDR_VBCFG2_RDFIFO, vbusmConfig->sdramFifoDepth) |
                    CSL_FMK (DDR_VBCFG2_RDCMDFIFO, 
                                                vbusmConfig->readCmdFifoDepth);
                
  burstPriority = & (setup->burstPriority);   
  ddrRegs->VBPR = CSL_FMK (DDR_VBPR_PROC, burstPriority->priOldCount);
                                  
  perfConfig = & (setup->perfConfig);
  if (perfConfig->count1MidEn) {
    CSL_FINS (ddrRegs->PCCR, DDR_PCCR_CNT1MSTEN, CSL_DDR_MIDFILT_ENABLE);
    CSL_FINS (ddrRegs->PCMRSR, DDR_PCMRSR_MSTID1, perfConfig->count1MasterId);
  }
  if (perfConfig->count2MidEn) {
    CSL_FINS (ddrRegs->PCCR, DDR_PCCR_CNT2MSTEN, (Uint32)CSL_DDR_MIDFILT_ENABLE);
    CSL_FINS (ddrRegs->PCMRSR, DDR_PCMRSR_MSTID2, perfConfig->count2MasterId);
  }
  
  if (perfConfig->count1CsEn) {
    CSL_FINS (ddrRegs->PCCR, DDR_PCCR_CNT1REGEN, CSL_DDR_CSFILT_ENABLE);
    CSL_FINS (ddrRegs->PCMRSR, DDR_PCMRSR_REGID1, perfConfig->count1ChipSel);
  }
  if (perfConfig->count2CsEn) {
    CSL_FINS (ddrRegs->PCCR, DDR_PCCR_CNT2REGEN, CSL_DDR_CSFILT_ENABLE);
    CSL_FINS (ddrRegs->PCMRSR, DDR_PCMRSR_REGID2, perfConfig->count2ChipSel);
  }
  
  CSL_FINS (ddrRegs->PCCR, DDR_PCCR_CNT1CFG, perfConfig->count1Filter);
  CSL_FINS (ddrRegs->PCCR, DDR_PCCR_CNT2CFG, perfConfig->count2Filter);
  
  if (setup->memSel2 == CSL_DDR_ASYNC_MEM) {
    asyncMemConfig = setup->configCS2; 
    ddrRegs->ASYNCCS2CR = CSL_FMK (DDR_ASYNCCS2CR_SSEL, CSL_DDR_ASYNC_MEM) |
                   CSL_FMK (DDR_ASYNCCS2CR_SS, asyncMemConfig->strobeSel) |
                   CSL_FMK (DDR_ASYNCCS2CR_BWEM, asyncMemConfig->weStrbSel) |
                   CSL_FMK (DDR_ASYNCCS2CR_EW, asyncMemConfig->extWait) |
                   CSL_FMK (DDR_ASYNCCS2CR_WSETUP, asyncMemConfig->wrSetup) |
                   CSL_FMK (DDR_ASYNCCS2CR_WSTROBE, asyncMemConfig->wrStrobe) |
                   CSL_FMK (DDR_ASYNCCS2CR_WHOLD, asyncMemConfig->wrHold) |
                   CSL_FMK (DDR_ASYNCCS2CR_RSETUP, asyncMemConfig->rdSetup) |
                   CSL_FMK (DDR_ASYNCCS2CR_RSTROBE, asyncMemConfig->rdStrobe) |
                   CSL_FMK (DDR_ASYNCCS2CR_RHOLD, asyncMemConfig->rdHold) |
                   CSL_FMK (DDR_ASYNCCS2CR_ASIZE, asyncMemConfig->asize);
                   
  } else {
    sbsramConfig =  setup->configCS2;  
    ddrRegs->SBSCS2CR = CSL_FMK (DDR_SBSCS2CR_SSEL, (Uint32)CSL_DDR_SYNC_SBSRAM) |
                   CSL_FMK (DDR_SBSCS2CR_CSEXT, sbsramConfig->csExt) |
                   CSL_FMK (DDR_SBSCS2CR_READEN, sbsramConfig->rdEnMode) |
                   CSL_FMK (DDR_SBSCS2CR_WRLAT, sbsramConfig->wrLat) |
                   CSL_FMK (DDR_SBSCS2CR_RDLAT, sbsramConfig->rdLat) | 
                   CSL_FMK (DDR_SBSCS2CR_SBSIZE, sbsramConfig->sbsize);
  }
  
  if (setup->memSel3 == CSL_DDR_ASYNC_MEM) {
    asyncMemConfig = setup->configCS3; 
    ddrRegs->ASYNCCS3CR = CSL_FMK (DDR_ASYNCCS3CR_SSEL, CSL_DDR_ASYNC_MEM) |
                   CSL_FMK (DDR_ASYNCCS3CR_SS, asyncMemConfig->strobeSel) |
                   CSL_FMK (DDR_ASYNCCS3CR_BWEM, asyncMemConfig->weStrbSel) |
                   CSL_FMK (DDR_ASYNCCS3CR_EW, asyncMemConfig->extWait) |
                   CSL_FMK (DDR_ASYNCCS3CR_WSETUP, asyncMemConfig->wrSetup) |
                   CSL_FMK (DDR_ASYNCCS3CR_WSTROBE, asyncMemConfig->wrStrobe) |
                   CSL_FMK (DDR_ASYNCCS3CR_WHOLD, asyncMemConfig->wrHold) |
                   CSL_FMK (DDR_ASYNCCS3CR_RSETUP, asyncMemConfig->rdSetup) |
                   CSL_FMK (DDR_ASYNCCS3CR_RSTROBE, asyncMemConfig->rdStrobe) |
                   CSL_FMK (DDR_ASYNCCS3CR_RHOLD, asyncMemConfig->rdHold) |
                   CSL_FMK (DDR_ASYNCCS3CR_ASIZE, asyncMemConfig->asize);
                   
  } else {
    sbsramConfig =  setup->configCS3;  
    ddrRegs->SBSCS3CR = CSL_FMK (DDR_SBSCS3CR_SSEL, (Uint32)CSL_DDR_SYNC_SBSRAM) |
                   CSL_FMK (DDR_SBSCS3CR_CSEXT, sbsramConfig->csExt) |
                   CSL_FMK (DDR_SBSCS3CR_READEN, sbsramConfig->rdEnMode) |
                   CSL_FMK (DDR_SBSCS3CR_WRLAT, sbsramConfig->wrLat) |
                   CSL_FMK (DDR_SBSCS3CR_RDLAT, sbsramConfig->rdLat) | 
                   CSL_FMK (DDR_SBSCS3CR_SBSIZE, sbsramConfig->sbsize);
  }
  
  if (setup->memSel4 == CSL_DDR_ASYNC_MEM) {
    asyncMemConfig = setup->configCS4; 
    ddrRegs->ASYNCCS4CR = CSL_FMK (DDR_ASYNCCS4CR_SSEL, CSL_DDR_ASYNC_MEM) |
                   CSL_FMK (DDR_ASYNCCS4CR_SS, asyncMemConfig->strobeSel) |
                   CSL_FMK (DDR_ASYNCCS4CR_BWEM, asyncMemConfig->weStrbSel) |
                   CSL_FMK (DDR_ASYNCCS4CR_EW, asyncMemConfig->extWait) |
                   CSL_FMK (DDR_ASYNCCS4CR_WSETUP, asyncMemConfig->wrSetup) |
                   CSL_FMK (DDR_ASYNCCS4CR_WSTROBE, asyncMemConfig->wrStrobe) |
                   CSL_FMK (DDR_ASYNCCS4CR_WHOLD, asyncMemConfig->wrHold) |
                   CSL_FMK (DDR_ASYNCCS4CR_RSETUP, asyncMemConfig->rdSetup) |
                   CSL_FMK (DDR_ASYNCCS4CR_RSTROBE, asyncMemConfig->rdStrobe) |
                   CSL_FMK (DDR_ASYNCCS4CR_RHOLD, asyncMemConfig->rdHold) |
                   CSL_FMK (DDR_ASYNCCS4CR_ASIZE, asyncMemConfig->asize);
                   
  } else {
    sbsramConfig =  setup->configCS4;  
    ddrRegs->SBSCS4CR = CSL_FMK (DDR_SBSCS4CR_SSEL, (Uint32)CSL_DDR_SYNC_SBSRAM) |
                   CSL_FMK (DDR_SBSCS4CR_CSEXT, sbsramConfig->csExt) |
                   CSL_FMK (DDR_SBSCS4CR_READEN, sbsramConfig->rdEnMode) |
                   CSL_FMK (DDR_SBSCS4CR_WRLAT, sbsramConfig->wrLat) |
                   CSL_FMK (DDR_SBSCS4CR_RDLAT, sbsramConfig->rdLat) | 
                   CSL_FMK (DDR_SBSCS4CR_SBSIZE, sbsramConfig->sbsize);
  }
  
  if (setup->memSel5 == CSL_DDR_ASYNC_MEM) {
    asyncMemConfig = setup->configCS5; 
    ddrRegs->ASYNCCS5CR = CSL_FMK (DDR_ASYNCCS5CR_SSEL, CSL_DDR_ASYNC_MEM) |
                   CSL_FMK (DDR_ASYNCCS5CR_SS, asyncMemConfig->strobeSel) |
                   CSL_FMK (DDR_ASYNCCS5CR_BWEM, asyncMemConfig->weStrbSel) |
                   CSL_FMK (DDR_ASYNCCS5CR_EW, asyncMemConfig->extWait) |
                   CSL_FMK (DDR_ASYNCCS5CR_WSETUP, asyncMemConfig->wrSetup) |
                   CSL_FMK (DDR_ASYNCCS5CR_WSTROBE, asyncMemConfig->wrStrobe) |
                   CSL_FMK (DDR_ASYNCCS5CR_WHOLD, asyncMemConfig->wrHold) |
                   CSL_FMK (DDR_ASYNCCS5CR_RSETUP, asyncMemConfig->rdSetup) |
                   CSL_FMK (DDR_ASYNCCS5CR_RSTROBE, asyncMemConfig->rdStrobe) |
                   CSL_FMK (DDR_ASYNCCS5CR_RHOLD, asyncMemConfig->rdHold) |
                   CSL_FMK (DDR_ASYNCCS5CR_ASIZE, asyncMemConfig->asize);
                   
  } else {
    sbsramConfig = setup->configCS5;  
    ddrRegs->SBSCS5CR = CSL_FMK (DDR_SBSCS5CR_SSEL, (Uint32)CSL_DDR_SYNC_SBSRAM) |
                   CSL_FMK (DDR_SBSCS5CR_CSEXT, sbsramConfig->csExt) |
                   CSL_FMK (DDR_SBSCS5CR_READEN, sbsramConfig->rdEnMode) |
                   CSL_FMK (DDR_SBSCS5CR_WRLAT, sbsramConfig->wrLat) |
                   CSL_FMK (DDR_SBSCS5CR_RDLAT, sbsramConfig->rdLat) | 
                   CSL_FMK (DDR_SBSCS5CR_SBSIZE, sbsramConfig->sbsize);
  }
  
  return status;
}

/* Rev.No.   Date/Time               ECN No.          Modifier      */
/* -------   ---------               -------          --------      */

/* 3          Aug 10:00:23 9         2271             xkeshavm      */
/*                                                                  */
/* To upload API CSL 0.51 Release                                   */
/********************************************************************/ 
