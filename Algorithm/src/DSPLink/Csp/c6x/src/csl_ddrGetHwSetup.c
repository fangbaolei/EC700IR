/*   ==========================================================================
*   Copyright (c) Texas Instruments Inc , 2004
 *
 *   Use of this software is controlled by the terms and conditions found
 *   in the license agreement under which this software has been supplied
 *   provided
 *   ==========================================================================
*/
/** @file csl_ddrGetHwSetup.c
*
*  @brief    File for functional layer of CSL API @a CSL_ddrGetHwSetup()
*
*  Description
*    - The @a CSL_ddrGetHwSetup() function definition & it's associated
*      functions
*  @date   29 June, 2004
*  @author Pratheesh Gangadhar
*/

#include <csl_ddr.h>

#pragma CODE_SECTION (CSL_ddrGetHwSetup, ".text:csl_section:ddr");
/**

    \brief Reads the currently set configuration from the DDR peripheral
           registers
    \param hDdr  Pointer to the object that holds reference to the instance
                 of DDR requested
    \param setup  Place holder to return the setup data		  
    \return if success, \c CSL_SOK, else error code 

    \see 
*/

CSL_Status CSL_ddrGetHwSetup ( CSL_DdrHandle hDdr,
                               CSL_DdrHwSetup *setup ) {
  CSL_Status status = CSL_SOK;
  CSL_DdrRegsOvly ddrRegs = hDdr->regs;
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
  tempReg = ddrRegs->SDBCR;
  bankConfig->bootUnlockConfig = ( CSL_FEXTR (tempReg, 23, 16) << 16); 
  bankConfig->timingControl =  (CSL_DdrTimingUnlockCtrl)
	                       CSL_FEXT (tempReg, DDR_SDBCR_TIMUNLOCK);
  bankConfig->narrowModeCtrl = (CSL_DdrNarrowModeCtrl)
                               CSL_FEXT (tempReg, DDR_SDBCR_NM);
  bankConfig->casLatency = (CSL_DdrCasLatency)
                            CSL_FEXT (tempReg, DDR_SDBCR_CL);
  bankConfig->internBank = (CSL_DdrInternSdramBank)
                            CSL_FEXT (tempReg, DDR_SDBCR_IBANK);
  bankConfig->externBank = (CSL_DdrExternSdramBank)
                            CSL_FEXT (tempReg, DDR_SDBCR_EBANK);
  bankConfig->pageSize = (CSL_DdrSdramPageSize)
                          CSL_FEXT (tempReg, DDR_SDBCR_PAGESIZE);
                   
  refreshCtrl   = &(setup->refreshCtrl);
  tempReg = ddrRegs->SDRCR;
  refreshCtrl->selfRefresh = (CSL_DdrSelfRefrCtrl)
                              CSL_FEXT (tempReg, DDR_SDRCR_SR);
  refreshCtrl->parityEn = (CSL_DdrParityCtrl)
                           CSL_FEXT (tempReg, DDR_SDRCR_PEN);
  refreshCtrl->refreshRate = CSL_FEXT (tempReg, DDR_SDRCR_RR);

  sdramTiming =  &(setup->sdramTiming);
  
  tempReg = ddrRegs->SDTIMR;
  sdramTiming->rfc = CSL_FEXT (tempReg, DDR_SDTIMR_T_RFC);
  sdramTiming->rp = CSL_FEXT (tempReg, DDR_SDTIMR_T_RP);
  sdramTiming->rcd = CSL_FEXT (tempReg, DDR_SDTIMR_T_RCD);
  sdramTiming->wr = CSL_FEXT (tempReg, DDR_SDTIMR_T_WR);
  sdramTiming->ras = CSL_FEXT (tempReg, DDR_SDTIMR_T_RAS);
  sdramTiming->rc = CSL_FEXT (tempReg, DDR_SDTIMR_T_RC);
  sdramTiming->rrd = CSL_FEXT (tempReg, DDR_SDTIMR_T_RRD);
  sdramTiming->wtr = CSL_FEXT (tempReg, DDR_SDTIMR_T_WTR);
  
  tempReg = ddrRegs->SDTIMR2;
  sdramTiming->odt = CSL_FEXT (tempReg, DDR_SDTIMR2_T_ODT);
  sdramTiming->sxnr = CSL_FEXT (tempReg, DDR_SDTIMR2_T_SXNR);
  sdramTiming->sxrd = CSL_FEXT (tempReg, DDR_SDTIMR2_T_SXRD);
  sdramTiming->rtp = CSL_FEXT (tempReg, DDR_SDTIMR2_T_RTP);
  sdramTiming->cke = CSL_FEXT (tempReg, DDR_SDTIMR2_T_CKE);
  
  asyncWait = & (setup->asyncWait);
  tempReg = ddrRegs->AWCCR;
  asyncWait->waitPolarity = (CSL_DdrWaitPolCtrl)
                             CSL_FEXT (tempReg, DDR_AWCCR_WP);
  asyncWait->turnAround = CSL_FEXT (tempReg, DDR_AWCCR_TA);
  asyncWait->maxExtWait = CSL_FEXT (tempReg, DDR_AWCCR_MEWC);

  vbusmConfig  = & (setup->vbusmConfig);
  
  tempReg = ddrRegs->VBCFG1;
  vbusmConfig->busWidth = (CSL_DdrVbusmWidth)
                           CSL_FEXT (tempReg, DDR_VBCFG1_BUS);
  vbusmConfig->statFifoDepth = CSL_FEXT (tempReg, DDR_VBCFG1_STATFIFO);
  vbusmConfig->writeFifoDepth = CSL_FEXT (tempReg, DDR_VBCFG1_WRFIFO);
  vbusmConfig->cmdFifoDepth = CSL_FEXT (tempReg, DDR_VBCFG1_CMDFIFO);
  
  tempReg = ddrRegs->VBCFG2;
  vbusmConfig->regReadFifoDepth = CSL_FEXT (tempReg, DDR_VBCFG2_RREGFIFO);
  vbusmConfig->sdramFifoDepth = CSL_FEXT (tempReg, DDR_VBCFG2_RDFIFO);
  vbusmConfig->readCmdFifoDepth = CSL_FEXT (tempReg, DDR_VBCFG2_RDCMDFIFO);
  
  
  
  burstPriority = & (setup->burstPriority);   
  
  burstPriority->priOldCount = CSL_FEXT ( ddrRegs->VBPR, DDR_VBPR_PROC); 
  

  perfConfig = & (setup->perfConfig);
  
  perfConfig->count1MidEn = (CSL_DdrMidFiltCtrl)
                             CSL_FEXT (ddrRegs->PCCR, DDR_PCCR_CNT1MSTEN);
  if (perfConfig->count1MidEn) {
    perfConfig->count1MasterId = CSL_FEXT (ddrRegs->PCMRSR, DDR_PCMRSR_MSTID1);
  }
  
  perfConfig->count2MidEn = (CSL_DdrMidFiltCtrl)
                             CSL_FEXT (ddrRegs->PCCR, DDR_PCCR_CNT2MSTEN);
  if (perfConfig->count2MidEn) {
    perfConfig->count2MasterId = CSL_FEXT (ddrRegs->PCMRSR, DDR_PCMRSR_MSTID2);
  }
  
  perfConfig->count1CsEn = (CSL_DdrCsFiltCtrl)
                             CSL_FEXT (ddrRegs->PCCR, DDR_PCCR_CNT1REGEN);
  if (perfConfig->count1CsEn) {
    perfConfig->count1ChipSel = CSL_FEXT (ddrRegs->PCMRSR, DDR_PCMRSR_REGID1);
  }
 
  perfConfig->count2CsEn = (CSL_DdrCsFiltCtrl)
                             CSL_FEXT (ddrRegs->PCCR, DDR_PCCR_CNT2REGEN);
  if (perfConfig->count2CsEn) {
    perfConfig->count2ChipSel = CSL_FEXT (ddrRegs->PCMRSR, DDR_PCMRSR_REGID2);
  }
  
  perfConfig->count1Filter = (CSL_DdrFiltConfig)
                              CSL_FEXT (ddrRegs->PCCR, DDR_PCCR_CNT1CFG);
  perfConfig->count2Filter = (CSL_DdrFiltConfig)
                              CSL_FEXT (ddrRegs->PCCR, DDR_PCCR_CNT2CFG); 
                                                                    
  setup->memSel2 = (CSL_DdrMemSelect)
                    CSL_FEXT (ddrRegs->ASYNCCS2CR, DDR_ASYNCCS2CR_SSEL);
  if (setup->memSel2 == CSL_DDR_ASYNC_MEM) {
    asyncMemConfig = setup->configCS2; 
    tempReg = ddrRegs->ASYNCCS2CR; 
    asyncMemConfig->strobeSel = (CSL_DdrStrobeCtrl)
                                 CSL_FEXT (tempReg, DDR_ASYNCCS2CR_SS); 
    asyncMemConfig->weStrbSel = (CSL_DdrWeStrobeCtrl)
                                 CSL_FEXT (tempReg, DDR_ASYNCCS2CR_BWEM);
    asyncMemConfig->extWait = (CSL_DdrExtWaitCtrl)
                               CSL_FEXT (tempReg, DDR_ASYNCCS2CR_EW);
    asyncMemConfig->wrSetup = CSL_FEXT (tempReg, DDR_ASYNCCS2CR_WSETUP);
    asyncMemConfig->wrStrobe = CSL_FEXT (tempReg, DDR_ASYNCCS2CR_WSTROBE);
    asyncMemConfig->wrHold = CSL_FEXT (tempReg, DDR_ASYNCCS2CR_WHOLD);
    asyncMemConfig->rdSetup = CSL_FEXT (tempReg, DDR_ASYNCCS2CR_RSETUP);
    asyncMemConfig->rdStrobe = CSL_FEXT (tempReg, DDR_ASYNCCS2CR_RSTROBE);
    asyncMemConfig->rdHold = CSL_FEXT (tempReg, DDR_ASYNCCS2CR_RHOLD);
    asyncMemConfig->asize = (CSL_DdrBuswidth)
                            CSL_FEXT (tempReg, DDR_ASYNCCS2CR_ASIZE);
                   
  } else {
    sbsramConfig =  setup->configCS2;  
    tempReg = ddrRegs->SBSCS2CR; 
    sbsramConfig->csExt = (CSL_DdrCsExtend)
                           CSL_FEXT (tempReg, DDR_SBSCS2CR_CSEXT);
    sbsramConfig->rdEnMode = (CSL_DdrRdEnCtrl) 
                              CSL_FEXT (tempReg, DDR_SBSCS2CR_READEN);
    sbsramConfig->wrLat = (CSL_DdrLatency)
                           CSL_FEXT (tempReg, DDR_SBSCS2CR_WRLAT);
    sbsramConfig->rdLat = (CSL_DdrLatency)
                           CSL_FEXT (tempReg, DDR_SBSCS2CR_RDLAT);
    sbsramConfig->sbsize = (CSL_DdrBuswidth)
                            CSL_FEXT (tempReg, DDR_SBSCS2CR_SBSIZE);
  }
  
  setup->memSel3 = (CSL_DdrMemSelect)
                    CSL_FEXT (ddrRegs->ASYNCCS3CR, DDR_ASYNCCS3CR_SSEL);                  
  if (setup->memSel3 == CSL_DDR_ASYNC_MEM) {
    asyncMemConfig = setup->configCS3; 
    tempReg = ddrRegs->ASYNCCS3CR; 
    asyncMemConfig->strobeSel = (CSL_DdrStrobeCtrl)
                                 CSL_FEXT (tempReg, DDR_ASYNCCS3CR_SS); 
    asyncMemConfig->weStrbSel = (CSL_DdrWeStrobeCtrl)
                                 CSL_FEXT (tempReg, DDR_ASYNCCS3CR_BWEM);
    asyncMemConfig->extWait = (CSL_DdrExtWaitCtrl)
                               CSL_FEXT (tempReg, DDR_ASYNCCS3CR_EW);
    asyncMemConfig->wrSetup = CSL_FEXT (tempReg, DDR_ASYNCCS3CR_WSETUP);
    asyncMemConfig->wrStrobe = CSL_FEXT (tempReg, DDR_ASYNCCS3CR_WSTROBE);
    asyncMemConfig->wrHold = CSL_FEXT (tempReg, DDR_ASYNCCS3CR_WHOLD);
    asyncMemConfig->rdSetup = CSL_FEXT (tempReg, DDR_ASYNCCS3CR_RSETUP);
    asyncMemConfig->rdStrobe = CSL_FEXT (tempReg, DDR_ASYNCCS3CR_RSTROBE);
    asyncMemConfig->rdHold = CSL_FEXT (tempReg, DDR_ASYNCCS3CR_RHOLD);
    asyncMemConfig->asize = (CSL_DdrBuswidth)
                             CSL_FEXT (tempReg, DDR_ASYNCCS3CR_ASIZE);
                   
  } else {
    sbsramConfig =  setup->configCS3;  
    tempReg = ddrRegs->SBSCS3CR; 
    sbsramConfig->csExt = (CSL_DdrCsExtend)
                           CSL_FEXT (tempReg, DDR_SBSCS3CR_CSEXT);
    sbsramConfig->rdEnMode = (CSL_DdrRdEnCtrl)
                              CSL_FEXT (tempReg, DDR_SBSCS3CR_READEN);
    sbsramConfig->wrLat = (CSL_DdrLatency)
                           CSL_FEXT (tempReg, DDR_SBSCS3CR_WRLAT);
    sbsramConfig->rdLat = (CSL_DdrLatency)
                           CSL_FEXT (tempReg, DDR_SBSCS3CR_RDLAT);
    sbsramConfig->sbsize = (CSL_DdrBuswidth)
                            CSL_FEXT (tempReg, DDR_SBSCS3CR_SBSIZE);
  }
  
  setup->memSel4 = (CSL_DdrMemSelect)
                    CSL_FEXT (ddrRegs->ASYNCCS4CR, DDR_ASYNCCS4CR_SSEL);                  
  if (setup->memSel4 == CSL_DDR_ASYNC_MEM) {
    asyncMemConfig = setup->configCS4; 
    tempReg = ddrRegs->ASYNCCS4CR; 
    asyncMemConfig->strobeSel = (CSL_DdrStrobeCtrl)
                                 CSL_FEXT (tempReg, DDR_ASYNCCS4CR_SS); 
    asyncMemConfig->weStrbSel = (CSL_DdrWeStrobeCtrl)
                                 CSL_FEXT (tempReg, DDR_ASYNCCS4CR_BWEM);
    asyncMemConfig->extWait = (CSL_DdrExtWaitCtrl)
                               CSL_FEXT (tempReg, DDR_ASYNCCS4CR_EW);
    asyncMemConfig->wrSetup = CSL_FEXT (tempReg, DDR_ASYNCCS4CR_WSETUP);
    asyncMemConfig->wrStrobe = CSL_FEXT (tempReg, DDR_ASYNCCS4CR_WSTROBE);
    asyncMemConfig->wrHold = CSL_FEXT (tempReg, DDR_ASYNCCS4CR_WHOLD);
    asyncMemConfig->rdSetup = CSL_FEXT (tempReg, DDR_ASYNCCS4CR_RSETUP);
    asyncMemConfig->rdStrobe = CSL_FEXT (tempReg, DDR_ASYNCCS4CR_RSTROBE);
    asyncMemConfig->rdHold = CSL_FEXT (tempReg, DDR_ASYNCCS4CR_RHOLD);
    asyncMemConfig->asize = (CSL_DdrBuswidth)
                             CSL_FEXT (tempReg, DDR_ASYNCCS4CR_ASIZE);
                   
  } else {
    sbsramConfig =  setup->configCS4;  
    tempReg = ddrRegs->SBSCS4CR; 
    sbsramConfig->csExt = (CSL_DdrCsExtend)
                           CSL_FEXT (tempReg, DDR_SBSCS4CR_CSEXT);
    sbsramConfig->rdEnMode = (CSL_DdrRdEnCtrl)
                              CSL_FEXT (tempReg, DDR_SBSCS4CR_READEN);
    sbsramConfig->wrLat = (CSL_DdrLatency)
                           CSL_FEXT (tempReg, DDR_SBSCS4CR_WRLAT);
    sbsramConfig->rdLat = (CSL_DdrLatency)
                           CSL_FEXT (tempReg, DDR_SBSCS4CR_RDLAT);
    sbsramConfig->sbsize = (CSL_DdrBuswidth)
                            CSL_FEXT (tempReg, DDR_SBSCS4CR_SBSIZE);
  }
  
  setup->memSel5 = (CSL_DdrMemSelect)
                    CSL_FEXT (ddrRegs->ASYNCCS5CR, DDR_ASYNCCS5CR_SSEL);                  
  if (setup->memSel5 == CSL_DDR_ASYNC_MEM) {
    asyncMemConfig = setup->configCS5; 
    tempReg = ddrRegs->ASYNCCS5CR; 
    asyncMemConfig->strobeSel = (CSL_DdrStrobeCtrl)
                                 CSL_FEXT (tempReg, DDR_ASYNCCS5CR_SS); 
    asyncMemConfig->weStrbSel = (CSL_DdrWeStrobeCtrl)
                                 CSL_FEXT (tempReg, DDR_ASYNCCS5CR_BWEM);
    asyncMemConfig->extWait = (CSL_DdrExtWaitCtrl)
                               CSL_FEXT (tempReg, DDR_ASYNCCS5CR_EW);
    asyncMemConfig->wrSetup = CSL_FEXT (tempReg, DDR_ASYNCCS5CR_WSETUP);
    asyncMemConfig->wrStrobe = CSL_FEXT (tempReg, DDR_ASYNCCS5CR_WSTROBE);
    asyncMemConfig->wrHold = CSL_FEXT (tempReg, DDR_ASYNCCS5CR_WHOLD);
    asyncMemConfig->rdSetup = CSL_FEXT (tempReg, DDR_ASYNCCS5CR_RSETUP);
    asyncMemConfig->rdStrobe = CSL_FEXT (tempReg, DDR_ASYNCCS5CR_RSTROBE);
    asyncMemConfig->rdHold = CSL_FEXT (tempReg, DDR_ASYNCCS5CR_RHOLD);
    asyncMemConfig->asize = (CSL_DdrBuswidth)
                             CSL_FEXT (tempReg, DDR_ASYNCCS5CR_ASIZE);
                   
  } else {
    sbsramConfig =  setup->configCS5;  
    tempReg = ddrRegs->SBSCS5CR; 
    sbsramConfig->csExt = (CSL_DdrCsExtend)
                           CSL_FEXT (tempReg, DDR_SBSCS5CR_CSEXT);
    sbsramConfig->rdEnMode = (CSL_DdrRdEnCtrl)
                              CSL_FEXT (tempReg, DDR_SBSCS5CR_READEN);
    sbsramConfig->wrLat = (CSL_DdrLatency)
                           CSL_FEXT (tempReg, DDR_SBSCS5CR_WRLAT);
    sbsramConfig->rdLat = (CSL_DdrLatency)
                           CSL_FEXT (tempReg, DDR_SBSCS5CR_RDLAT);
    sbsramConfig->sbsize = (CSL_DdrBuswidth)
                            CSL_FEXT (tempReg, DDR_SBSCS5CR_SBSIZE);
  }
  
  
  return status;
}

/* Rev.No.   Date/Time               ECN No.          Modifier      */
/* -------   ---------               -------          --------      */

/* 3          Aug 10:00:22 9         2271             xkeshavm      */
/*                                                                  */
/* To upload API CSL 0.51 Release                                   */
/********************************************************************/ 
