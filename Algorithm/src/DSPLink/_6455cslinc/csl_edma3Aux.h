/*  ============================================================================
 *   Copyright (c) Texas Instruments Inc 2002, 2003, 2004, 2005, 2006
 *
 *   Use of this software is controlled by the terms and conditions found in the
 *   license agreement under which this software has been supplied.
 *   ===========================================================================
 */
 
/* ============================================================================
 *   @file  csl_edmaAux.h
 *
 *   @path  $(CSLPATH)\inc
 *
 *   @desc  Auxillary Header file for EDMA
 *
 */
 
/* =============================================================================
 *  Revision History
 *  ===============
 *  29-May-2004 Ruchika Kharwar File Created.
 *
 * =============================================================================
 */

#ifndef _CSL_EDMA3AUX_H_
#define _CSL_EDMA3AUX_H_

#define CSL_IDEF_INLINE static inline

/*
 *  Status query functions of the Edma Module
 */

/* ============================================================================
 *   @n@b CSL_edma3GetEventMissed
 *
 *   @b Description
 *   @n Queries all the events missed.Since there may be upto 64 EDMA 
 *      channels + upto 8 QDMA channels,this points to an  array of 3, 
 *      32 bit elements.Gets the status of the missed events.
 *
 *   @b Arguments
 *   @verbatim
        hModule           Module Handle
        missedLo          missed [0] - holds status from EMR
        missedHi          missed [1] - holds status from EMRH
        missedQdma        missed [2] - holds status from QEMR
 *   @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @n                     Always returns CSL_SOK
 *
 *   <b> Pre Condition </b>
 *   @n  All CSL_edma3Init(), CSL_edma3Open(), must be called successfully in  
 *       that order before CSL_edma3GetEventsMissed() can be called.
 *
 *   <b> Post Condition </b>
 *    @n None
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim
            CSL_Edma3Handle             hModule;
            CSL_Edma3HwSetup            hwSetup,gethwSetup;
            CSL_Edma3Obj                edmaObj;
            CSL_Edma3ParamHandle        hParamPing,hParamPong,hParamBasic;
            CSL_Edma3ChannelObj         ChObj;
            CSL_Edma3QueryInfo          info;
            CSL_Edma3CmdIntr            regionIntr;
            CSL_Edma3CmdDrae            regionAccess;
            CSL_Edma3ChannelHandle      hChannel;
            CSL_Edma3ParamSetup         myParamSetup;
            CSL_Edma3Context            context;
            CSL_Edma3ChannelAttr        chAttr;
            CSL_Edma3HwChannelSetup     dmahwSetup[CSL_EDMA3_NUM_DMACH] = \
                                              CSL_EDMA3_DMACHANNELSETUP_DEFAULT;
            CSL_Edma3HwChannelSetup     getdmahwSetup[CSL_EDMA3_NUM_DMACH];
            CSL_BitMask32               missedEdmaHi,missedEdma, missedQdma;
            CSL_Status                  status;
            Uint32                      i, passStatus = 1;   
        
            // Module Initialization
            CSL_edma3Init(&context);
            
            // Module Level Open    
            hModule = CSL_edma3Open(&edmaObj,CSL_EDMA3,NULL,&status);
            
            // Module Setup
            hwSetup.dmaChaSetup  = &dmahwSetup[0];
            hwSetup.qdmaChaSetup = NULL;
            CSL_edma3HwSetup(hModule,&hwSetup);
         
            // Channel 0 Open in context of Shadow region 0
            chAttr.regionNum = CSL_EDMA3_REGION_0;
            chAttr.chaNum = CSL_EDMA3_CHA_DSP_EVT;
            hChannel = CSL_edma3ChannelOpen(&ChObj, CSL_EDMA3, &chAttr, &status);
               
            // Get missed events 
            status = CSL_edma3GetEventMissed(hModule, &missedEdma, &missedEdmaHi, 
                                             &missedQdma);
            ...
     @endverbatim
 * ===========================================================================
 */
CSL_IDEF_INLINE CSL_Status  CSL_edma3GetEventMissed (
    CSL_Edma3Handle         hModule,
    CSL_BitMask32           *missedLo,
    CSL_BitMask32           *missedHi,
    CSL_BitMask32           *missedQdma
)
{
    /* Reading the missed Status registers */
    *missedLo    = hModule->regs->EMR;
    *missedHi    = hModule->regs->EMRH;
    *missedQdma  = hModule->regs->QEMR;
    
    return CSL_SOK;     
}

/* ============================================================================
 *   @n@b CSL_edma3GetControllerError
 *
 *   @b Description
 *   @n Gets the status of the controller Error.
 *
 *   @b Arguments
 *   @verbatim
        hModule           Module Handle
        ccStat            This points to a structure allocated by the user 
                          program that can hold the queried status values.
 *   @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @n                     Always returns CSL_SOK
 *
 *   <b> Pre Condition </b>
 *   @n  All CSL_edma3Init(), CSL_edma3Open(), must be called successfully in  
 *       that order before CSL_edma3GetControllerError() can be called.
 *
 *   <b> Post Condition </b>
 *    @n None
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim
            CSL_Edma3Handle hModule;
            CSL_Edma3HwSetup hwSetup,gethwSetup;
            CSL_Edma3Obj    edmaObj;
            CSL_Edma3ParamHandle hParamPing,hParamPong,hParamBasic;
            CSL_Edma3ChannelObj ChObj;
            CSL_Edma3QueryInfo info;
            CSL_Edma3CmdIntr regionIntr;
            CSL_Edma3CmdDrae regionAccess;
            CSL_Edma3ChannelHandle hChannel;
            CSL_Edma3ParamSetup myParamSetup;
            CSL_Edma3Context    context;
            CSL_Edma3ChannelAttr  chAttr;
            CSL_Edma3HwChannelSetup dmahwSetup[CSL_EDMA3_NUM_DMACH] = \
                                              CSL_EDMA3_DMACHANNELSETUP_DEFAULT;
            CSL_Edma3HwChannelSetup getdmahwSetup[CSL_EDMA3_NUM_DMACH];
            
            CSL_Status status;
            Uint32 i,passStatus = 1;   
            
            // Module Initialization
            CSL_edma3Init(&context);
            
            // Module Level Open    
            hModule = CSL_edma3Open(&edmaObj,CSL_EDMA3,NULL,&status);
            
            // Module Setup
            hwSetup.dmaChaSetup  = &dmahwSetup[0];
            hwSetup.qdmaChaSetup = NULL;
            CSL_edma3HwSetup(hModule,&hwSetup);
            
            // Channel 0 Open in context of Shadow region 0
            chAttr.regionNum = CSL_EDMA3_REGION_0;
            chAttr.chaNum = CSL_EDMA3_CHA_DSP_EVT;
            hChannel = CSL_edma3ChannelOpen(&ChObj,
                                    CSL_EDMA3,
                                    &chAttr,                            
                                    &status);   
            // Get Controller Error
            status = CSL_edma3GetControllerError(hModule,&ccError);
            ...
     @endverbatim
 * ===========================================================================
 */
CSL_IDEF_INLINE CSL_Status  CSL_edma3GetControllerError (
    CSL_Edma3Handle         hModule,
    CSL_Edma3CtrlErrStat    *ccStat
)
{ 
    ccStat->error = hModule->regs->CCERR & 0xFF ; 
    ccStat->exceedTcc = (Bool)(CSL_FEXT(hModule->regs->CCERR, \
                                        EDMA3CC_CCERR_TCCERR)) ;
    
    return CSL_SOK;     
}

/* ============================================================================
 *   @n@b CSL_edma3GetIntrPendStatus
 *
 *   @b Description
 *   @n Queries the interrupt status of the interrupts.This is read from the 
 *      Global Channel Register.Gets the interrupt pend status.
 *
 *   @b Arguments
 *   @verbatim
        hModule           Module Handle
        region            Region
        intrLo            Status 0-31 of the interrupts 
        intrHi            Status 32-63 of the interrupts       
 *   @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @n                     Returns CSL_SOK
 *
 *   <b> Pre Condition </b>
 *   @n  All CSL_edma3Init(), CSL_edma3Open(), must be called successfully in  
 *       that order before CSL_edma3GetIntrPendStatus() can be called.
 *
 *   <b> Post Condition </b>
 *    @n None
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim
            CSL_Edma3Handle             hModule;
            CSL_Edma3HwSetup            hwSetup,gethwSetup;
            CSL_Edma3Obj                edmaObj;
            CSL_Edma3ParamHandle        hParamPing,hParamPong,hParamBasic;
            CSL_Edma3ChannelObj         ChObj;
            CSL_Edma3QueryInfo          info;
            CSL_Edma3CmdIntr            regionIntr;
            CSL_Edma3CmdDrae            regionAccess;
            CSL_Edma3ChannelHandle      hChannel;
            CSL_Edma3ParamSetup         myParamSetup;
            CSL_Edma3Context            context;
            CSL_Edma3ChannelAttr        chAttr;
            CSL_Edma3HwChannelSetup     dmahwSetup[CSL_EDMA3_NUM_DMACH] = \
                                             CSL_EDMA3_DMACHANNELSETUP_DEFAULT;
            CSL_Edma3HwChannelSetup     getdmahwSetup[CSL_EDMA3_NUM_DMACH];
            CSL_Status                  status;
            Uint32                      i, passStatus = 1;   
        
            // Module Initialization
            CSL_edma3Init(&context);
            
            // Module Level Open    
            hModule = CSL_edma3Open(&edmaObj,CSL_EDMA3,NULL,&status);
            
            // Module Setup
            hwSetup.dmaChaSetup  = &dmahwSetup[0];
            hwSetup.qdmaChaSetup = NULL;
            CSL_edma3HwSetup(hModule,&hwSetup);
         
            // Channel 0 Open in context of Shadow region 0
            chAttr.regionNum = CSL_EDMA3_REGION_0;
            chAttr.chaNum = CSL_EDMA3_CHA_DSP_EVT;
            hChannel = CSL_edma3ChannelOpen(&ChObj,
                                    CSL_EDMA3,
                                    &chAttr,                            
                                    &status);   
            
            // Get interrupt pend status 
            status = CSL_edma3GetIntrPendStatus(hModule,CSL_EDMA3_REGION_0, 
                                                &edmaIntrLo, &edmaIntrHi);
            ...
     @endverbatim
 * ===========================================================================
 */
CSL_IDEF_INLINE CSL_Status  CSL_edma3GetIntrPendStatus (
    CSL_Edma3Handle          hModule,
    Int                      region,
    CSL_BitMask32            *intrLo,
    CSL_BitMask32            *intrHi
)
{
    if (region != CSL_EDMA3_REGION_GLOBAL) {   
        *intrLo = hModule->regs->SHADOW[region].IPR  ;
        *intrHi = hModule->regs->SHADOW[region].IPRH ;
    } 
    else {
        *intrLo = hModule->regs->IPR  ;
        *intrHi = hModule->regs->IPRH ;
    }
    
    return CSL_SOK; 
}
   
#if CSL_EDMA3_MEMPROTECT 
/* ============================================================================
 *   @n@b CSL_edma3GetMemoryProtectionAttrib
 *
 *   @b Description
 *   @n Queries the Memory Access rights of the specified region. This 
 *      may be global/view.Gets the memory protection attributes.
 *
 *   @b Arguments
 *   @verbatim
        hModule           Module Handle
        region            Region queried. This may refer to the global channel
                          register space or a specific region/view.
 *   @endverbatim
 *
 *   <b> Return Value </b>  CSL_BitMask32
 *   @n                     Memory protection attributes - MPPAG/MPPA[0/1../n] 
 *                                                         value
 *
 *   <b> Pre Condition </b>
 *   @n  All CSL_edma3Init(), CSL_edma3Open(), must be called successfully in  
 *       that order before CSL_edma3GetMemoryProtectAttrib() can be called.
 *
 *   <b> Post Condition </b>
 *    @n None
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim
            CSL_Edma3Handle             hModule;
            CSL_Edma3HwSetup            hwSetup,gethwSetup;
            CSL_Edma3Obj                edmaObj;
            CSL_Edma3ParamHandle        hParamPing,hParamPong,hParamBasic;
            CSL_Edma3ChannelObj         ChObj;
            CSL_Edma3QueryInfo          info;
            CSL_Edma3CmdIntr            regionIntr;
            CSL_Edma3CmdDrae            regionAccess;
            CSL_Edma3ChannelHandle      hChannel;
            CSL_Edma3ParamSetup         myParamSetup;
            CSL_Edma3Context            context;
            CSL_Edma3ChannelAttr        chAttr;
            CSL_Edma3HwChannelSetup     dmahwSetup[CSL_EDMA3_NUM_DMACH] = \
                                              CSL_EDMA3_DMACHANNELSETUP_DEFAULT;
            CSL_Edma3HwChannelSetup     getdmahwSetup[CSL_EDMA3_NUM_DMACH];
            CSL_Status                  status;
            Uint32                      i, passStatus = 1,memprot;   
        
            // Module Initialization
            CSL_edma3Init(&context);
            
            // Module Level Open    
            hModule = CSL_edma3Open(&edmaObj,CSL_EDMA3,NULL,&status);
            
            // Module Setup
            hwSetup.dmaChaSetup  = &dmahwSetup[0];
            hwSetup.qdmaChaSetup = NULL;
            CSL_edma3HwSetup(hModule,&hwSetup);
         
            // Channel 0 Open in context of Shadow region 0
            chAttr.regionNum = CSL_EDMA3_REGION_0;
            chAttr.chaNum = CSL_EDMA3_CHA_DSP_EVT;
            hChannel = CSL_edma3ChannelOpen(&ChObj,
                                            CSL_EDMA3,
                                            &chAttr,                            
                                            &status);   
            // Get memory protection attributes 
            memProt = CSL_edma3GetMemoryProtectAttrib(hModule,CSL_EDMA3_REGION_2);
            ...
     @endverbatim
 * ===========================================================================
 */
CSL_IDEF_INLINE CSL_BitMask32  CSL_edma3GetMemoryProtectionAttrib (
    CSL_Edma3Handle        hModule,
    Int                    region
)
{
    /* Memory protection page fault attribute */
    CSL_BitMask32         mppa;
    
    if (region < 0) {
        mppa = hModule->regs->MPPAG;
    }
    else {
        mppa = hModule->regs->MPPA[region];
    }
                 
    return  mppa;
}

/* ============================================================================
 *   @n@b CSL_edma3GetMemoryFaultError
 *
 *   @b Description
 *   @n Queries the Channel controllers memory fault error and the error 
 *      attributes.Gets the memory fault status details, clears the fault.
 *
 *   @b Arguments
 *   @verbatim
        hModule           Module Handle
        memFault          This points to a structure alloctaed by the user 
                          program that can hold the queried the memory fault
                          status values 
 *   @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @n                     Always returns CSL_SOK 
 *
 *   <b> Pre Condition </b>
 *   @n  All CSL_edma3Init(), CSL_edma3Open(), must be called successfully in  
 *       that order before CSL_edma3GetMemoryFaultError() can be called.
 *
 *   <b> Post Condition </b>
 *    @n None
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim
            CSL_Edma3Handle             hModule;
            CSL_Edma3HwSetup            hwSetup,gethwSetup;
            CSL_Edma3Obj                edmaObj;
            CSL_Edma3ParamHandle        hParamPing,hParamPong,hParamBasic;
            CSL_Edma3ChannelObj         ChObj;
            CSL_Edma3QueryInfo          info;
            CSL_Edma3CmdIntr            regionIntr;
            CSL_Edma3CmdDrae            regionAccess;
            CSL_Edma3ChannelHandle      hChannel;
            CSL_Edma3ParamSetup         myParamSetup;
            CSL_Edma3Context            context;
            CSL_Edma3ChannelAttr        chAttr;
            CSL_Edma3HwChannelSetup     dmahwSetup[CSL_EDMA3_NUM_DMACH] = \
                                              CSL_EDMA3_DMACHANNELSETUP_DEFAULT;
            CSL_Edma3HwChannelSetup     getdmahwSetup[CSL_EDMA3_NUM_DMACH];
            CSL_Status                  status;
            Uint32                      i, passStatus = 1;   
        
            // Module Initialization
            CSL_edma3Init(&context);
            
            // Module Level Open    
            hModule = CSL_edma3Open(&edmaObj,CSL_EDMA3,NULL,&status);
            
            // Module Setup
            hwSetup.dmaChaSetup  = &dmahwSetup[0];
            hwSetup.qdmaChaSetup = NULL;
            CSL_edma3HwSetup(hModule,&hwSetup);
         
            // Channel 0 Open in context of Shadow region 0
            chAttr.regionNum = CSL_EDMA3_REGION_0;
            chAttr.chaNum = CSL_EDMA3_CHA_DSP_EVT;
            hChannel = CSL_edma3ChannelOpen(&ChObj, CSL_EDMA3,&chAttr, &status);   
            
            // Get memory protection fault 
            status = CSL_edma3GetMemoryFaultError(hModule,&memFault);
            ...
     @endverbatim
 * ===========================================================================
 */
CSL_IDEF_INLINE CSL_Status  CSL_edma3GetMemoryFaultError (
    CSL_Edma3Handle             hModule,
    CSL_Edma3MemFaultStat       *memFault
)
{ 
    /* Reading the Memory Fault Status registers */
    memFault->addr = hModule->regs->MPFAR; 
    memFault->error =  CSL_FEXTR(hModule->regs->MPFSR, 8, 0);
    memFault->fid = CSL_FEXTR(hModule->regs->MPFSR, 12, 9);
    
    return CSL_SOK;     
}
#endif

/* =============================================================================
 *   @n@b CSL_edma3SetMemoryProtectionAttrib
 *
 *   @b Description
 *   @n This API sets the memory protection attributes for the specified region.
 *      Sets specified memory protection attributes for the specifed region.
 *
 *   @b Arguments
 *   @verbatim
        hModule           Module Handle
        
        edmaRegion        region - this may be specified as one of the many view 
                          supported by the chip or the global memory protection 
                          attributes themselves.
                          
        mpa               Value to be programmed into the MPPAG/MPPA[0/1/2/../n]
                          This is a Bitmask of the protection attributes.
 *   @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @n                     Always returns CSL_SOK
 *
 *   <b> Pre Condition </b>
 *   @n  All CSL_edma3Init(), CSL_edma3Open(), must be called successfully in  
 *       that order before CSL_edma3SetMemoryProtectionAttrib() can be called.
 *
 *   <b> Post Condition </b>
 *    @n None
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim
            CSL_Edma3Handle hModule;
            CSL_Edma3HwSetup hwSetup,gethwSetup;
            CSL_Edma3Obj    edmaObj;
            CSL_Edma3ParamHandle hParamPing,hParamPong,hParamBasic;
            CSL_Edma3ChannelObj ChObj;
            CSL_Edma3QueryInfo info;
            CSL_Edma3CmdIntr regionIntr;
            CSL_Edma3CmdDrae regionAccess;
            CSL_Edma3ChannelHandle hChannel;
            CSL_Edma3ParamSetup myParamSetup;
            CSL_Edma3Context    context;
            CSL_Edma3ChannelAttr  chAttr;
            CSL_Edma3HwChannelSetup dmahwSetup[CSL_EDMA3_NUM_DMACH] = \
                                             CSL_EDMA3_DMACHANNELSETUP_DEFAULT;
            CSL_Edma3HwChannelSetup getdmahwSetup[CSL_EDMA3_NUM_DMACH];
        
            CSL_Status status;
            Uint32 i,passStatus = 1;   
        
            // Module Initialization
            CSL_edma3Init(&context);
            
            // Module Level Open    
            hModule = CSL_edma3Open(&edmaObj,CSL_EDMA3,NULL,&status);
            
            // Module Setup
            hwSetup.dmaChaSetup  = &dmahwSetup[0];
            hwSetup.qdmaChaSetup = NULL;
            CSL_edma3HwSetup(hModule,&hwSetup);
         
            // Channel 0 Open in context of Shadow region 0
            chAttr.regionNum = CSL_EDMA3_REGION_0;
            chAttr.chaNum = CSL_EDMA3_CHA_DSP_EVT;
            hChannel = CSL_edma3ChannelOpen(&ChObj, CSL_EDMA3,&chAttr, &status);   
            
            // Module Control API 
            status = CSL_edma3SetMemoryProtectionAttrib (
                                                    hModule,CSL_EDMA3_REGION_0,
                                                    (CSL_EDMA3_MEMACCESS_UX \
                                                    |CSL_EDMA3_MEMACCESS_UW \
                                                    |CSL_EDMA3_MEMACCESS_UR \
                                                    |CSL_EDMA3_MEMACCESS_AID2));
            ...
     @endverbatim
 * ===========================================================================
 */
#if CSL_EDMA3_MEMPROTECT 
CSL_IDEF_INLINE CSL_Status  CSL_edma3SetMemoryProtectionAttrib(
    CSL_Edma3Handle        hModule,
    Int                    edmaRegion,
    CSL_BitMask32          mppa
)
{    
    if (edmaRegion == CSL_EDMA3_REGION_GLOBAL) {
        hModule->regs->MPPAG = mppa;
    }
    else {
        hModule->regs->MPPA[edmaRegion] = mppa;
    }
    
    return CSL_SOK;
}    
#endif

/* =============================================================================
 *   @n@b CSL_edma3DmaRegionAccessEnable
 *
 *   @b Description
 *   @n This API sets the DRAE/DRAEH BitMask.Enables shadow region access bits
 *      DRAE,DRAEH.
 *
 *   @b Arguments
 *   @verbatim
        hModule           Module Handle
        
        edmaRegion        Region - this may be specified as one of the many view 
                          supported by the chip or the global memory protection 
                          attributes themselves.
                          
        access            Region bits to be programmed
                          
        drae              Bitmask to be enabled in DRAE
                          
        draeh             Bitmask to be enabled in DRAEH
 *   @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status    
 *   @n                     CSL_SOK            - Region access enable success
 *   @n                     CSL_ESYS_INVPARAMS - Invalid parameter
 *
 *   <b> Pre Condition </b>
 *   @n  All CSL_edma3Init(), CSL_edma3Open(), must be called successfully in  
 *       that order before CSL_edma3DmaRegionAccessEnable() can be called.
 *
 *   <b> Post Condition </b>
 *    @n None
 *
 *   @b Modifies
 *   @n Enable shadow region access bits DRAE,DRAEH
 *
 *   @b Example
 *   @verbatim
            CSL_Edma3Handle         hModule;
            CSL_Edma3HwSetup        hwSetup,gethwSetup;
            CSL_Edma3Obj            edmaObj;
            CSL_Edma3ParamHandle    hParamPing,hParamPong,hParamBasic;
            CSL_Edma3ChannelObj     ChObj;
            CSL_Edma3QueryInfo      info;
            CSL_Edma3CmdIntr        regionIntr;
            CSL_Edma3CmdDrae        regionAccess;
            CSL_Edma3ChannelHandle  hChannel;
            CSL_Edma3ParamSetup     myParamSetup;
            CSL_Edma3Context        context;
            CSL_Edma3ChannelAttr    chAttr;
            CSL_Edma3HwChannelSetup dmahwSetup[CSL_EDMA3_NUM_DMACH] = \
                                             CSL_EDMA3_DMACHANNELSETUP_DEFAULT;
            CSL_Edma3HwChannelSetup getdmahwSetup[CSL_EDMA3_NUM_DMACH];
            CSL_Status              status;
            Uint32                  i, passStatus = 1;   
        
            // Module Initialization
            CSL_edma3Init(&context);
            
            // Module Level Open    
            hModule = CSL_edma3Open(&edmaObj,CSL_EDMA3,NULL,&status);
            
            // Module Setup
            hwSetup.dmaChaSetup  = &dmahwSetup[0];
            hwSetup.qdmaChaSetup = NULL;
            CSL_edma3HwSetup(hModule,&hwSetup);
         
            // Channel 0 Open in context of Shadow region 0
            chAttr.regionNum = CSL_EDMA3_REGION_0;
            chAttr.chaNum = CSL_EDMA3_CHA_DSP_EVT;
            hChannel = CSL_edma3ChannelOpen(&ChObj,
                                    CSL_EDMA3,
                                    &chAttr,                            
                                    &status);   
        
            // Module Control API 
            status = CSL_edma3DmaRegionAccessEnable(hModule, CSL_EDMA3_REGION_0,
                                                    0x000000FF, 0x000000FF);
            ...
     @endverbatim
 * ===========================================================================
 */
CSL_IDEF_INLINE CSL_Status  CSL_edma3DmaRegionAccessEnable (
    CSL_Edma3Handle       hModule,
    Int                   edmaRegion,
    CSL_BitMask32         drae,
    CSL_BitMask32         draeh
)
{

    if (edmaRegion == CSL_EDMA3_REGION_GLOBAL) {
        return CSL_ESYS_INVPARAMS;
    }
    
    hModule->regs->DRA[edmaRegion].DRAE |= drae;
    hModule->regs->DRA[edmaRegion].DRAEH |= draeh;
    
    return CSL_SOK;
}

/* =============================================================================
 *   @n@b CSL_edma3DmaRegionAccessDisable
 *
 *   @b Description
 *   @n This API clears the DRAE/DRAEH BitMask..Disable shadow region access bits
 *      DRAE,DRAEH.
 *
 *   @b Arguments
 *   @verbatim
        hModule           Module Handle
        
        edmaRegion        Region - this may be specified as one of the many view 
                          supported by the chip or the global memory protection 
                          attributes themselves.
                          
        access            Region bits to be programmed
                          
        drae              Bitmask to be enabled in DRAE
                          
        draeh             Bitmask to be enabled in DRAEH
 *   @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @n                     CSL_SOK            - Region access disable success
 *   @n                     CSL_ESYS_INVPARAMS - Invalid parameter
 *
 *   <b> Pre Condition </b>
 *   @n  All CSL_edma3Init(), CSL_edma3Open(), must be called successfully in  
 *       that order before CSL_edma3DmaRegionAccessEnable() can be called.
 *
 *   <b> Post Condition </b>
 *    @n None
 *
 *   @b Modifies
 *   @n Disables shadow region access bits DRAE,DRAEH
 *
 *   @b Example
 *   @verbatim
            CSL_Edma3Handle             hModule;
            CSL_Edma3HwSetup            hwSetup,gethwSetup;
            CSL_Edma3Obj                edmaObj;
            CSL_Edma3ParamHandle        hParamPing,hParamPong,hParamBasic;
            CSL_Edma3ChannelObj         ChObj;
            CSL_Edma3QueryInfo          info;
            CSL_Edma3CmdIntr            regionIntr;
            CSL_Edma3CmdDrae            regionAccess;
            CSL_Edma3ChannelHandle      hChannel;
            CSL_Edma3ParamSetup         myParamSetup;
            CSL_Edma3Context            context;
            CSL_Edma3ChannelAttr        chAttr;
            CSL_Edma3HwChannelSetup     dmahwSetup[CSL_EDMA3_NUM_DMACH] = \
                                             CSL_EDMA3_DMACHANNELSETUP_DEFAULT;
            CSL_Edma3HwChannelSetup     getdmahwSetup[CSL_EDMA3_NUM_DMACH];
            CSL_Status                  status;
            Uint32                      i, passStatus = 1;   
        
            // Module Initialization
            CSL_edma3Init(&context);
            
            // Module Level Open    
            hModule = CSL_edma3Open(&edmaObj,CSL_EDMA3,NULL,&status);
            
            // Module Setup
            hwSetup.dmaChaSetup  = &dmahwSetup[0];
            hwSetup.qdmaChaSetup = NULL;
            CSL_edma3HwSetup(hModule,&hwSetup);
         
            // Channel 0 Open in context of Shadow region 0
            chAttr.regionNum = CSL_EDMA3_REGION_0;
            chAttr.chaNum = CSL_EDMA3_CHA_DSP_EVT;
            hChannel = CSL_edma3ChannelOpen(&ChObj, CSL_EDMA3,&chAttr, &status);   
        
            // Module Control API 
            status = CSL_edma3DmaRegionAccessDisable(hModule,CSL_EDMA3_REGION_0,
                                                     0x000000FF,0x000000FF);
            ...
     @endverbatim
 * ===========================================================================
 */
CSL_IDEF_INLINE CSL_Status  CSL_edma3DmaRegionAccessDisable (
    CSL_Edma3Handle        hModule,
    Int                    edmaRegion,
    CSL_BitMask32          drae,
    CSL_BitMask32          draeh
)
{
    if (edmaRegion == CSL_EDMA3_REGION_GLOBAL) {
        return CSL_ESYS_INVPARAMS;
    }
    
    hModule->regs->DRA[edmaRegion].DRAE &= ~drae;
    hModule->regs->DRA[edmaRegion].DRAEH &= ~draeh;
    
    return CSL_SOK;
}

/* =============================================================================
 *   @n@b CSL_edma3QdmaRegionAccessEnable
 *
 *   @b Description
 *   @n This API sets the QRAE BitMask.Enables shadow region access bits QRAE
 *
 *   @b Arguments
 *   @verbatim
        hModule           Module Handle
        
        edmaRegion        Region - this may be specified as one of the many view 
                          supported by the chip or the global memory protection 
                          attributes themselves.
                          
        qrae              Bitmask to be enabled in QRAE
 *   @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status    
 *   @n                     CSL_SOK          - Qdma Region access enable success
 *   @n                     CSL_ESYS_INVPARAMS - Invalid parameter
 *
 *   <b> Pre Condition </b>
 *   @n  All CSL_edma3Init(), CSL_edma3Open(), must be called successfully in  
 *       that order before CSL_edma3QdmaRegionAccessEnable() can be called.
 *
 *   <b> Post Condition </b>
 *    @n None
 *
 *   @b Modifies
 *   @n Enable Qdma shadow region access bits QRAE
 *
 *   @b Example
 *   @verbatim
            CSL_Edma3Handle         hModule;
            CSL_Edma3HwSetup        hwSetup,gethwSetup;
            CSL_Edma3Obj            edmaObj;
            CSL_Edma3ParamHandle    hParamPing,hParamPong,hParamBasic;
            CSL_Edma3ChannelObj     ChObj;
            CSL_Edma3QueryInfo      info;
            CSL_Edma3CmdIntr        regionIntr;
            CSL_Edma3CmdDrae        regionAccess;
            CSL_Edma3ChannelHandle  hChannel;
            CSL_Edma3ParamSetup     myParamSetup;
            CSL_Edma3Context        context;
            CSL_Edma3ChannelAttr    chAttr;
            CSL_Edma3HwChannelSetup dmahwSetup[CSL_EDMA3_NUM_DMACH] = \
                                             CSL_EDMA3_DMACHANNELSETUP_DEFAULT;
            CSL_Edma3HwChannelSetup getdmahwSetup[CSL_EDMA3_NUM_DMACH];
            CSL_Status              status;
            Uint32                  i, passStatus = 1;   
        
            // Module Initialization
            CSL_edma3Init(&context);
            
            // Module Level Open    
            hModule = CSL_edma3Open(&edmaObj,CSL_EDMA3,NULL,&status);
            
            // Module Setup
            hwSetup.dmaChaSetup  = &dmahwSetup[0];
            hwSetup.qdmaChaSetup = NULL;
            CSL_edma3HwSetup(hModule,&hwSetup);
         
            // Channel 0 Open in context of Shadow region 0
            chAttr.regionNum = CSL_EDMA3_REGION_0;
            chAttr.chaNum = CSL_EDMA3_CHA_DSP_EVT;
            hChannel = CSL_edma3ChannelOpen(&ChObj, CSL_EDMA3, &chAttr, &status);   
        
            // Module Control API 
            status = CSL_edma3QdmaRegionAccessEnable(hModule,CSL_EDMA3_REGION_0, 
                                                     0x0000000F);
            ...
     @endverbatim
 * ===========================================================================
 */
CSL_IDEF_INLINE CSL_Status  CSL_edma3QdmaRegionAccessEnable (
    CSL_Edma3Handle        hModule,
    Int                    edmaRegion,
    CSL_BitMask32          qrae
)
{
    if (edmaRegion == CSL_EDMA3_REGION_GLOBAL) {
        return CSL_ESYS_INVPARAMS;
    }
    
    hModule->regs->QRAE[edmaRegion] |= qrae;
    
    return CSL_SOK;
}

/* =============================================================================
 *   @n@b CSL_edma3QdmaRegionAccessDisable
 *
 *   @b Description
 *   @n This API clears the QRAE BitMask.Disables shadow region access bits QRAE
 *
 *   @b Arguments
 *   @verbatim
        hModule           Module Handle
        
        edmaRegion        Region - this may be specified as one of the many view 
                          supported by the chip or the global memory protection 
                          attributes themselves.
                          
        access            Region bits to be programmed
                          
        qrae              Bitmask to be enabled in QRAE
 *   @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status    
 *   @n                     CSL_SOK          - Qdma Region access enable success
 *   @n                     CSL_ESYS_INVPARAMS - Invalid parameter
 *
 *   <b> Pre Condition </b>
 *   @n  All CSL_edma3Init(), CSL_edma3Open(), must be called successfully in  
 *       that order before CSL_edma3QdmaRegionAccessDisable() can be called.
 *
 *   <b> Post Condition </b>
 *    @n None
 *
 *   @b Modifies
 *   @n Disables Qdma shadow region access bits QRAE
 *
 *   @b Example
 *   @verbatim
            CSL_Edma3Handle         hModule;
            CSL_Edma3HwSetup        hwSetup,gethwSetup;
            CSL_Edma3Obj            edmaObj;
            CSL_Edma3ParamHandle    hParamPing,hParamPong,hParamBasic;
            CSL_Edma3ChannelObj     ChObj;
            CSL_Edma3QueryInfo      info;
            CSL_Edma3CmdIntr        regionIntr;
            CSL_Edma3CmdDrae        regionAccess;
            CSL_Edma3ChannelHandle  hChannel;
            CSL_Edma3ParamSetup     myParamSetup;
            CSL_Edma3Context        context;
            CSL_Edma3ChannelAttr    chAttr;
            CSL_Edma3HwChannelSetup dmahwSetup[CSL_EDMA3_NUM_DMACH] = \
                                             CSL_EDMA3_DMACHANNELSETUP_DEFAULT;
            CSL_Edma3HwChannelSetup getdmahwSetup[CSL_EDMA3_NUM_DMACH];
            CSL_Status              status;
            Uint32                  i, passStatus = 1;   
        
            // Module Initialization
            CSL_edma3Init(&context);
            
            // Module Level Open    
            hModule = CSL_edma3Open(&edmaObj,CSL_EDMA3,NULL,&status);
            
            // Module Setup
            hwSetup.dmaChaSetup  = &dmahwSetup[0];
            hwSetup.qdmaChaSetup = NULL;
            CSL_edma3HwSetup(hModule,&hwSetup);
         
            // Channel 0 Open in context of Shadow region 0
            chAttr.regionNum = CSL_EDMA3_REGION_0;
            chAttr.chaNum = CSL_EDMA3_CHA_DSP_EVT;
            hChannel = CSL_edma3ChannelOpen(&ChObj, CSL_EDMA3, &chAttr, &status);   
        
            // 4 channels disallowed
            status = CSL_edma3QdmaRegionAccessDisable(hModule,CSL_EDMA3_REGION_0,
                                                      0x000000FF);
            ...
     @endverbatim
 * ===========================================================================
 */
CSL_IDEF_INLINE CSL_Status  CSL_edma3QdmaRegionAccessDisable (
    CSL_Edma3Handle        hModule,
    Int                    edmaRegion,
    CSL_BitMask32          qrae
)
{ 
    if (edmaRegion == CSL_EDMA3_REGION_GLOBAL) {
        return CSL_ESYS_INVPARAMS;
    }
        
    hModule->regs->QRAE[edmaRegion] &= ~qrae;
    
    return CSL_SOK;
}

/* =============================================================================
 *   @n@b CSL_edma3EventQueuePrioritySet
 *
 *   @b Description
 *   @n This API sets the Queue priority level to the appropriate value.
 *
 *   @b Arguments
 *   @verbatim
        hModule           Module Handle
        
        que               Event queue
                          
        pri               Priority needed to be set for above Event queue
 *   @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status    
 *   @n                     Always return CSL_SOK
 *
 *   <b> Pre Condition </b>
 *   @n  All CSL_edma3Init(), CSL_edma3Open(), must be called successfully in  
 *       that order before CSL_edma3EventQueuePrioritySet() can be called.
 *
 *   <b> Post Condition </b>
 *    @n None
 *
 *   @b Modifies
 *   @n Set event queue priority
 *
 *   @b Example
 *   @verbatim
            CSL_Edma3Handle         hModule;
            CSL_Edma3HwSetup        hwSetup,gethwSetup;
            CSL_Edma3Obj            edmaObj;
            CSL_Edma3ParamHandle    hParamPing,hParamPong,hParamBasic;
            CSL_Edma3ChannelObj     ChObj;
            CSL_Edma3QueryInfo      info;
            CSL_Edma3CmdIntr        regionIntr;
            CSL_Edma3CmdDrae        regionAccess;
            CSL_Edma3ChannelHandle  hChannel;
            CSL_Edma3ParamSetup     myParamSetup;
            CSL_Edma3Context        context;
            CSL_Edma3ChannelAttr    chAttr;
            CSL_Edma3HwChannelSetup dmahwSetup[CSL_EDMA3_NUM_DMACH] = \
                                             CSL_EDMA3_DMACHANNELSETUP_DEFAULT;
            CSL_Edma3HwChannelSetup getdmahwSetup[CSL_EDMA3_NUM_DMACH];
            CSL_Status              status;
            Uint32                  i, passStatus = 1;   
        
            // Module Initialization
            CSL_edma3Init(&context);
            
            // Module Level Open    
            hModule = CSL_edma3Open(&edmaObj,CSL_EDMA3,NULL,&status);
            
            // Module Setup
            hwSetup.dmaChaSetup  = &dmahwSetup[0];
            hwSetup.qdmaChaSetup = NULL;
            CSL_edma3HwSetup(hModule,&hwSetup);
         
            // Channel 0 Open in context of Shadow region 0
            chAttr.regionNum = CSL_EDMA3_REGION_0;
            chAttr.chaNum = CSL_EDMA3_CHA_DSP_EVT;
            hChannel = CSL_edma3ChannelOpen(&ChObj, CSL_EDMA3, &chAttr, &status);   
        
            status = CSL_edma3EventQueuePrioritySet(hModule,CSL_EDMA3_QUE_0,
                                                    CSL_EDMA3_QUE_PRI2);
            ...
     @endverbatim
 * ===========================================================================
 */
CSL_IDEF_INLINE CSL_Status  CSL_edma3EventQueuePrioritySet (
    CSL_Edma3Handle      hModule,
    CSL_Edma3Que         que,
    CSL_Edma3QuePri      pri
)
{
    CSL_FINSR(hModule->regs->QUEPRI, (4 * que + 2), (4 * que), pri);
    
    return CSL_SOK;
} 

/* =============================================================================
 *   @n@b CSL_Edma3QueThresholdSet
 *
 *   @b Description
 *   @n Sets the specified threshold for the specified queue.This API sets the 
 *      Queue threshold to the appropriate level.
 * 
 *   @b Arguments
 *   @verbatim
        hModule           Module Handle
        
        que               Event queue
                          
        threshold         Target threshold value for the above queue.
 *   @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status    
 *   @n                     Always return CSL_SOK
 *
 *   <b> Pre Condition </b>
 *   @n  All CSL_edma3Init(), CSL_edma3Open(), must be called successfully in  
 *       that order before CSL_Edma3QueThresholdSet() can be called.
 *
 *   <b> Post Condition </b>
 *    @n None
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
            CSL_Edma3Handle         hModule;
            CSL_Edma3HwSetup        hwSetup,gethwSetup;
            CSL_Edma3Obj            edmaObj;
            CSL_Edma3ParamHandle    hParamPing,hParamPong,hParamBasic;
            CSL_Edma3ChannelObj     ChObj;
            CSL_Edma3QueryInfo      info;
            CSL_Edma3CmdIntr        regionIntr;
            CSL_Edma3CmdDrae        regionAccess;
            CSL_Edma3ChannelHandle  hChannel;
            CSL_Edma3ParamSetup     myParamSetup;
            CSL_Edma3Context        context;
            CSL_Edma3ChannelAttr    chAttr;
            CSL_Edma3HwChannelSetup dmahwSetup[CSL_EDMA3_NUM_DMACH] = \
                                             CSL_EDMA3_DMACHANNELSETUP_DEFAULT;
            CSL_Edma3HwChannelSetup getdmahwSetup[CSL_EDMA3_NUM_DMACH];
            CSL_Status              status;
            Uint32                  i, passStatus = 1;   
        
            // Module Initialization
            CSL_edma3Init(&context);
            
            // Module Level Open    
            hModule = CSL_edma3Open(&edmaObj,CSL_EDMA3,NULL,&status);
            
            // Module Setup
            hwSetup.dmaChaSetup  = &dmahwSetup[0];
            hwSetup.qdmaChaSetup = NULL;
            CSL_edma3HwSetup(hModule,&hwSetup);
         
            // Channel 0 Open in context of Shadow region 0
            chAttr.regionNum = CSL_EDMA3_REGION_0;
            chAttr.chaNum = CSL_EDMA3_CHA_DSP_EVT;
            hChannel = CSL_edma3ChannelOpen(&ChObj, CSL_EDMA3, &chAttr, &status);   
        
            status = CSL_edma3EventQueueThresholdSet(hModule,CSL_EDMA3_QUE_0,
                                                     CSL_EDMA3_QUE_THR9);
            ...
     @endverbatim
 * ===========================================================================
 */
CSL_IDEF_INLINE CSL_Status  CSL_edma3EventQueueThresholdSet (
    CSL_Edma3Handle      hModule,
    CSL_Edma3Que         que,
    CSL_Edma3QueThr      threshold
)
{
    CSL_FINSR(hModule->regs->QWMTHRA, (8 * que + 4),(8 * que),threshold);
    
    return CSL_SOK;
}

/* =============================================================================
 *   @n@b CSL_edma3ErrorEval
 *
 *   @b Description
 *   @n This API enables enables evaluation of errros for the specified 
 *      view/shadow region.Sets EVAL bit of the EEVAL register in the Global 
 *      register space
 * 
 *   @b Arguments
 *   @verbatim
        hModule           Module Handle
    @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status    
 *   @n                     Always return CSL_SOK
 *
 *   <b> Pre Condition </b>
 *   @n  All CSL_edma3Init(), CSL_edma3Open(), must be called successfully in  
 *       that order before CSL_edma3ErrorEval() can be called.
 *
 *   <b> Post Condition </b>
 *    @n None
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
            CSL_Edma3Handle         hModule;
            CSL_Edma3HwSetup        hwSetup,gethwSetup;
            CSL_Edma3Obj            edmaObj;
            CSL_Edma3ParamHandle    hParamPing,hParamPong,hParamBasic;
            CSL_Edma3ChannelObj     ChObj;
            CSL_Edma3QueryInfo      info;
            CSL_Edma3CmdIntr        regionIntr;
            CSL_Edma3CmdDrae        regionAccess;
            CSL_Edma3ChannelHandle  hChannel;
            CSL_Edma3ParamSetup     myParamSetup;
            CSL_Edma3Context        context;
            CSL_Edma3ChannelAttr    chAttr;
            CSL_Edma3HwChannelSetup dmahwSetup[CSL_EDMA3_NUM_DMACH] = \
                                             CSL_EDMA3_DMACHANNELSETUP_DEFAULT;
            CSL_Edma3HwChannelSetup getdmahwSetup[CSL_EDMA3_NUM_DMACH];
            CSL_Status              status;
            Uint32                  i, passStatus = 1;   
        
            // Module Initialization
            CSL_edma3Init(&context);
            
            // Module Level Open    
            hModule = CSL_edma3Open(&edmaObj,CSL_EDMA3,NULL,&status);
            
            // Module Setup
            hwSetup.dmaChaSetup  = &dmahwSetup[0];
            hwSetup.qdmaChaSetup = NULL;
            CSL_edma3HwSetup(hModule,&hwSetup);
         
            // Channel 0 Open in context of Shadow region 0
            chAttr.regionNum = CSL_EDMA3_REGION_0;
            chAttr.chaNum = CSL_EDMA3_CHA_DSP_EVT;
            hChannel = CSL_edma3ChannelOpen(&ChObj, CSL_EDMA3, &chAttr, &status);   
        
             status = CSL_edma3ErrorEval(hModule);
            ...
     @endverbatim
 * ===========================================================================
 */    
CSL_IDEF_INLINE CSL_Status  CSL_edma3ErrorEval (
    CSL_Edma3Handle        hModule
)
{
    hModule->regs->EEVAL = 1; 
    
    return CSL_SOK;
}

/* =============================================================================
 *   @n@b CSL_edma3InterruptEnable
 *
 *   @b Description
 *   @n Enables the specified interrupt in the Global Channel register space.
 *      This API enables the interrupts as specified in the BitMask.
 * 
 *   @b Arguments
 *   @verbatim
        hModule           Module Handle
        
        region            Region Number
        
        intrLo            Interrupt 0-31 (BitMask32) to be enabled
        
        intrHi            Interrupt 32-63 (BitMask32) to be enabled
    @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status    
 *   @n                     Always return CSL_SOK
 *
 *   <b> Pre Condition </b>
 *   @n  All CSL_edma3Init(), CSL_edma3Open(), must be called successfully in  
 *       that order before CSL_edma3InterruptEnable() can be called.
 *
 *   <b> Post Condition </b>
 *    @n None
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
            CSL_Edma3Handle         hModule;
            CSL_Edma3HwSetup        hwSetup,gethwSetup;
            CSL_Edma3Obj            edmaObj;
            CSL_Edma3ParamHandle    hParamPing,hParamPong,hParamBasic;
            CSL_Edma3ChannelObj     ChObj;
            CSL_Edma3QueryInfo      info;
            CSL_Edma3CmdIntr        regionIntr;
            CSL_Edma3CmdDrae        regionAccess;
            CSL_Edma3ChannelHandle  hChannel;
            CSL_Edma3ParamSetup     myParamSetup;
            CSL_Edma3Context        context;
            CSL_Edma3ChannelAttr    chAttr;
            CSL_Edma3HwChannelSetup dmahwSetup[CSL_EDMA3_NUM_DMACH] = \
                                             CSL_EDMA3_DMACHANNELSETUP_DEFAULT;
            CSL_Edma3HwChannelSetup getdmahwSetup[CSL_EDMA3_NUM_DMACH];
            CSL_Status              status;
            Uint32                  i, passStatus = 1;   
        
            // Module Initialization
            CSL_edma3Init(&context);
            
            // Module Level Open    
            hModule = CSL_edma3Open(&edmaObj,CSL_EDMA3,NULL,&status);
            
            // Module Setup
            hwSetup.dmaChaSetup  = &dmahwSetup[0];
            hwSetup.qdmaChaSetup = NULL;
            CSL_edma3HwSetup(hModule,&hwSetup);
         
            // Channel 0 Open in context of Shadow region 0
            chAttr.regionNum = CSL_EDMA3_REGION_0;
            chAttr.chaNum = CSL_EDMA3_CHA_DSP_EVT;
            hChannel = CSL_edma3ChannelOpen(&ChObj, CSL_EDMA3, &chAttr, &status);   
        
            // Interrupts 5-7 enabled
            status = CSL_edma3InterruptEnable(hModule,0x000000E0, 0x0);
            ...
     @endverbatim
 * ===========================================================================
 */    
CSL_IDEF_INLINE CSL_Status  CSL_edma3InterruptEnable ( 
    CSL_Edma3Handle        hModule,
    Int                    region,
    CSL_BitMask32          intrLo,
    CSL_BitMask32          intrHi
)
{
    
    if (region != CSL_EDMA3_REGION_GLOBAL) {   
        hModule->regs->SHADOW[region].IESR = intrLo;
        hModule->regs->SHADOW[region].IESRH = intrHi;
    } 
    else {
        hModule->regs->IESR = intrLo;
        hModule->regs->IESRH = intrHi;
    }
    
    return CSL_SOK; 
}

/* =============================================================================
 *   @n@b CSL_edma3InterruptDisable
 *
 *   @b Description
 *   @n Disables the specified interrupt in the Global Channel register space.
 *      This API disables the interrupts as specified in the BitMask.
 * 
 *   @b Arguments
 *   @verbatim
        hModule           Module Handle
        
        region            Region Number
        
        intrLo            Interrupt 0-31 (BitMask32) to be disabled
        
        intrHi            Interrupt 32-63 (BitMask32) to be disabled
    @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status    
 *   @n                     Always return CSL_SOK
 *
 *   <b> Pre Condition </b>
 *   @n  All CSL_edma3Init(), CSL_edma3Open(), must be called successfully in  
 *       that order before CSL_edma3InterruptEnable() can be called.
 *
 *   <b> Post Condition </b>
 *    @n None
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
            CSL_Edma3Handle         hModule;
            CSL_Edma3HwSetup        hwSetup,gethwSetup;
            CSL_Edma3Obj            edmaObj;
            CSL_Edma3ParamHandle    hParamPing,hParamPong,hParamBasic;
            CSL_Edma3ChannelObj     ChObj;
            CSL_Edma3QueryInfo      info;
            CSL_Edma3CmdIntr        regionIntr;
            CSL_Edma3CmdDrae        regionAccess;
            CSL_Edma3ChannelHandle  hChannel;
            CSL_Edma3ParamSetup     myParamSetup;
            CSL_Edma3Context        context;
            CSL_Edma3ChannelAttr    chAttr;
            CSL_Edma3HwChannelSetup dmahwSetup[CSL_EDMA3_NUM_DMACH] = \
                                             CSL_EDMA3_DMACHANNELSETUP_DEFAULT;
            CSL_Edma3HwChannelSetup getdmahwSetup[CSL_EDMA3_NUM_DMACH];
            CSL_Status              status;
            Uint32                  i, passStatus = 1;   
        
            // Module Initialization
            CSL_edma3Init(&context);
            
            // Module Level Open    
            hModule = CSL_edma3Open(&edmaObj,CSL_EDMA3,NULL,&status);
            
            // Module Setup
            hwSetup.dmaChaSetup  = &dmahwSetup[0];
            hwSetup.qdmaChaSetup = NULL;
            CSL_edma3HwSetup(hModule,&hwSetup);
         
            // Channel 0 Open in context of Shadow region 0
            chAttr.regionNum = CSL_EDMA3_REGION_0;
            chAttr.chaNum = CSL_EDMA3_CHA_DSP_EVT;
            hChannel = CSL_edma3ChannelOpen(&ChObj, CSL_EDMA3, &chAttr, &status);   
        
            // Interrupts 5-7 disabled
            status = CSL_edma3InterruptDisable(hModule,0x000000E0, 0x0);
            ...
     @endverbatim
 * ===========================================================================
 */    

CSL_IDEF_INLINE CSL_Status  CSL_edma3InterruptDisable (
    CSL_Edma3Handle        hModule,
    Int                    region,
    CSL_BitMask32          intrLo,
    CSL_BitMask32          intrHi
)
{
    if (region !=CSL_EDMA3_REGION_GLOBAL) {   
        hModule->regs->SHADOW[region].IECR = intrLo;
        hModule->regs->SHADOW[region].IECRH = intrHi;
    } 
    else {
        hModule->regs->IECR  = intrLo;
        hModule->regs->IECRH = intrHi;
    }
    
    return CSL_SOK; 
}

/* =============================================================================
 *   @n@b CSL_edma3InterruptClear
 *
 *   @b Description
 *   @n This API clears the interrupt as specified.Clears the specified 
 *      interrupt in the Global Channel register space.
 * 
 *   @b Arguments
 *   @verbatim
        hModule           Module Handle
        
        region            Region Number
        
        intrLo            Interrupt 0-31 (BitMask32) to be cleared
        
        intrHi            Interrupt 32-63 (BitMask32) to be cleared
    @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status    
 *   @n                     Always return CSL_SOK
 *
 *   <b> Pre Condition </b>
 *   @n  All CSL_edma3Init(), CSL_edma3Open(), must be called successfully in  
 *       that order before CSL_edma3InterruptClear() can be called.
 *
 *   <b> Post Condition </b>
 *    @n None
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
            CSL_Edma3Handle         hModule;
            CSL_Edma3HwSetup        hwSetup,gethwSetup;
            CSL_Edma3Obj            edmaObj;
            CSL_Edma3ParamHandle    hParamPing,hParamPong,hParamBasic;
            CSL_Edma3ChannelObj     ChObj;
            CSL_Edma3QueryInfo      info;
            CSL_Edma3CmdIntr        regionIntr;
            CSL_Edma3CmdDrae        regionAccess;
            CSL_Edma3ChannelHandle  hChannel;
            CSL_Edma3ParamSetup     myParamSetup;
            CSL_Edma3Context        context;
            CSL_Edma3ChannelAttr    chAttr;
            CSL_Edma3HwChannelSetup dmahwSetup[CSL_EDMA3_NUM_DMACH] = \
                                             CSL_EDMA3_DMACHANNELSETUP_DEFAULT;
            CSL_Edma3HwChannelSetup getdmahwSetup[CSL_EDMA3_NUM_DMACH];
            CSL_Status              status;
            Uint32                  i, passStatus = 1;   
        
            // Module Initialization
            CSL_edma3Init(&context);
            
            // Module Level Open    
            hModule = CSL_edma3Open(&edmaObj,CSL_EDMA3,NULL,&status);
            
            // Module Setup
            hwSetup.dmaChaSetup  = &dmahwSetup[0];
            hwSetup.qdmaChaSetup = NULL;
            CSL_edma3HwSetup(hModule,&hwSetup);
         
            // Channel 0 Open in context of Shadow region 0
            chAttr.regionNum = CSL_EDMA3_REGION_0;
            chAttr.chaNum = CSL_EDMA3_CHA_DSP_EVT;
            hChannel = CSL_edma3ChannelOpen(&ChObj, CSL_EDMA3, &chAttr, &status);
        
            // 5,6,7 cleared 
            status = CSL_edma3InterruptClear(hModule,0x000000E0,0);
            ...
     @endverbatim
 * ===========================================================================
 */    
CSL_IDEF_INLINE CSL_Status  CSL_edma3InterruptClear (
    CSL_Edma3Handle        hModule,
    Int                    region,
    CSL_BitMask32          intrLo,
    CSL_BitMask32          intrHi
)
{
    if (region != CSL_EDMA3_REGION_GLOBAL) {   
        hModule->regs->SHADOW[region].ICR = intrLo;
        hModule->regs->SHADOW[region].ICRH = intrHi;
    } 
    else {
        hModule->regs->ICR = intrLo;
        hModule->regs->ICRH = intrHi;
    }   
    return CSL_SOK; 
}

/* =============================================================================
 *   @n@b CSL_edma3InterruptEval
 *
 *   @b Description
 *   @n If any enabled interrutps are pending, the tpcc_intN output signal 
 *      to be pulsed.
 * 
 *   @b Arguments
 *   @verbatim
        hModule           Module Handle
        
        region            Region Number
    @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status    
 *   @n                     Always return CSL_SOK
 *
 *   <b> Pre Condition </b>
 *   @n  All CSL_edma3Init(), CSL_edma3Open(), must be called successfully in  
 *       that order before CSL_edma3InterruptEval() can be called.
 *
 *   <b> Post Condition </b>
 *    @n None
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
            CSL_Edma3Handle         hModule;
            CSL_Edma3HwSetup        hwSetup,gethwSetup;
            CSL_Edma3Obj            edmaObj;
            CSL_Edma3ParamHandle    hParamPing,hParamPong,hParamBasic;
            CSL_Edma3ChannelObj     ChObj;
            CSL_Edma3QueryInfo      info;
            CSL_Edma3CmdIntr        regionIntr;
            CSL_Edma3CmdDrae        regionAccess;
            CSL_Edma3ChannelHandle  hChannel;
            CSL_Edma3ParamSetup     myParamSetup;
            CSL_Edma3Context        context;
            CSL_Edma3ChannelAttr    chAttr;
            CSL_Edma3HwChannelSetup dmahwSetup[CSL_EDMA3_NUM_DMACH] = \
                                             CSL_EDMA3_DMACHANNELSETUP_DEFAULT;
            CSL_Edma3HwChannelSetup getdmahwSetup[CSL_EDMA3_NUM_DMACH];
            CSL_Status              status;
            Uint32                  i, passStatus = 1;   
        
            // Module Initialization
            CSL_edma3Init(&context);
            
            // Module Level Open    
            hModule = CSL_edma3Open(&edmaObj,CSL_EDMA3,NULL,&status);
            
            // Module Setup
            hwSetup.dmaChaSetup  = &dmahwSetup[0];
            hwSetup.qdmaChaSetup = NULL;
            CSL_edma3HwSetup(hModule,&hwSetup);
         
            // Channel 0 Open in context of Shadow region 0
            chAttr.regionNum = CSL_EDMA3_REGION_0;
            chAttr.chaNum = CSL_EDMA3_CHA_DSP_EVT;
            hChannel = CSL_edma3ChannelOpen(&ChObj,CSL_EDMA3, &chAttr, &status);   
        
            // Interrupt Evaluate
            status = CSL_edma3InterruptEval(hModule);
            ...
     @endverbatim
 * ===========================================================================
 */    
CSL_IDEF_INLINE CSL_Status  CSL_edma3InterruptEval (
    CSL_Edma3Handle         hModule,
    Int                     region
)
{
    if (region != CSL_EDMA3_REGION_GLOBAL) {
        hModule->regs->SHADOW[region].IEVAL = 1;
    }
    else {
        hModule->regs->IEVAL = 1; 
    }
          
    return CSL_SOK; 
}

/* =============================================================================
 *   @n@b CSL_edma3MemFaultClear
 *
 *   @b Description
 *   @n Clear Memory Fault.
 * 
 *   @b Arguments
 *   @verbatim
        hModule           Module Handle
        
    @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status    
 *   @n                     Always return CSL_SOK
 *
 *   <b> Pre Condition </b>
 *   @n  All CSL_edma3Init(), CSL_edma3Open(), must be called successfully in  
 *       that order before CSL_edma3MemFaultClear() can be called.
 *
 *   <b> Post Condition </b>
 *    @n None
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
            CSL_Edma3Handle         hModule;
            CSL_Edma3HwSetup        hwSetup,gethwSetup;
            CSL_Edma3Obj            edmaObj;
            CSL_Edma3ParamHandle    hParamPing,hParamPong,hParamBasic;
            CSL_Edma3ChannelObj     ChObj;
            CSL_Edma3QueryInfo      info;
            CSL_Edma3CmdIntr        regionIntr;
            CSL_Edma3CmdDrae        regionAccess;
            CSL_Edma3ChannelHandle  hChannel;
            CSL_Edma3ParamSetup     myParamSetup;
            CSL_Edma3Context        context;
            CSL_Edma3ChannelAttr    chAttr;
            CSL_Edma3HwChannelSetup dmahwSetup[CSL_EDMA3_NUM_DMACH] = \
                                             CSL_EDMA3_DMACHANNELSETUP_DEFAULT;
            CSL_Edma3HwChannelSetup getdmahwSetup[CSL_EDMA3_NUM_DMACH];
            CSL_Status              status;
            Uint32                  i, passStatus = 1;   
        
            // Module Initialization
            CSL_edma3Init(&context);
            
            // Module Level Open    
            hModule = CSL_edma3Open(&edmaObj,CSL_EDMA3,NULL,&status);
            
            // Module Setup
            hwSetup.dmaChaSetup  = &dmahwSetup[0];
            hwSetup.qdmaChaSetup = NULL;
            CSL_edma3HwSetup(hModule,&hwSetup);
         
            // Channel 0 Open in context of Shadow region 0
            chAttr.regionNum = CSL_EDMA3_REGION_0;
            chAttr.chaNum = CSL_EDMA3_CHA_DSP_EVT;
            hChannel = CSL_edma3ChannelOpen(&ChObj, CSL_EDMA3, &chAttr, &status);   
        
            //... Channel Setup, trigger

            status = CSL_edma3MemFaultClear(hModule);
            ...
     @endverbatim
 * ===========================================================================
 */    
CSL_IDEF_INLINE CSL_Status  CSL_edma3MemFaultClear (
    CSL_Edma3Handle         hModule
)
{
    hModule->regs->MPFCR = 1;
    
    return CSL_SOK; 
}

/* =============================================================================
 *   @n@b CSL_edma3ControllerErrorClear
 *
 *   @b Description
 *   @n Channel Controller Error Fault.
 * 
 *   @b Arguments
 *   @verbatim
        hModule           Module Handle
        
        ccStat            Error Status Handle
    @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status    
 *   @n                     Always return CSL_SOK
 *
 *   <b> Pre Condition </b>
 *   @n  All CSL_edma3Init(), CSL_edma3Open(), must be called successfully in  
 *       that order before CSL_edma3ControllerErrorClear() can be called.
 *
 *   <b> Post Condition </b>
 *    @n None
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
            CSL_Edma3Handle         hModule;
            CSL_Edma3HwSetup        hwSetup,gethwSetup;
            CSL_Edma3Obj            edmaObj;
            CSL_Edma3ParamHandle    hParamPing,hParamPong,hParamBasic;
            CSL_Edma3ChannelObj     ChObj;
            CSL_Edma3QueryInfo      info;
            CSL_Edma3CmdIntr        regionIntr;
            CSL_Edma3CmdDrae        regionAccess;
            CSL_Edma3ChannelHandle  hChannel;
            CSL_Edma3ParamSetup     myParamSetup;
            CSL_Edma3Context        context;
            CSL_Edma3ChannelAttr    chAttr;
            CSL_Edma3HwChannelSetup dmahwSetup[CSL_EDMA3_NUM_DMACH] = \
                                             CSL_EDMA3_DMACHANNELSETUP_DEFAULT;
            CSL_Edma3HwChannelSetup getdmahwSetup[CSL_EDMA3_NUM_DMACH];
            CSL_Status              status;
            Uint32                  i, passStatus = 1;   
            CSL_Edma3CtrlErrStat    ccstatErr;
            
            // Module Initialization
            CSL_edma3Init(&context);
            
            // Module Level Open    
            hModule = CSL_edma3Open(&edmaObj,CSL_EDMA3,NULL,&status);
            
            // Module Setup
            hwSetup.dmaChaSetup  = &dmahwSetup[0];
            hwSetup.qdmaChaSetup = NULL;
            CSL_edma3HwSetup(hModule,&hwSetup);
         
            // Channel 0 Open in context of Shadow region 0
            chAttr.regionNum = CSL_EDMA3_REGION_0;
            chAttr.chaNum = CSL_EDMA3_CHA_DSP_EVT;
            hChannel = CSL_edma3ChannelOpen(&ChObj, CSL_EDMA3, &chAttr, &status);   
        
            //... Channel Setup, trigger

            // Obtain the status error
            status = CSL_edma3GetControllerError(hModule,&ccstatErr);

            // Clear the error 
            CSL_edma3ControllerErrorClear(hModule,&ccstatErr);
            ...
     @endverbatim
 * ===========================================================================
 */    
CSL_IDEF_INLINE CSL_Status  CSL_edma3ControllerErrorClear (
    CSL_Edma3Handle         hModule,
    CSL_Edma3CtrlErrStat   *ccStat
)
{
    /* Clears the errors */
    hModule->regs->CCERRCLR = CSL_FMK(EDMA3CC_CCERRCLR_TCCERR, \
                                      ccStat->exceedTcc) | ccStat->error;
                             
    return CSL_SOK;
}

/* =============================================================================
 *   @n@b CSL_edma3EventMissedClear
 *
 *   @b Description
 *   @n Clear the Event missed errors
 * 
 *   @b Arguments
 *   @verbatim
        hModule           Module Handle
        
        missedLo          Lower 32 of of the Event Missed register needing to 
                          be cleared
        
        missedHi          Upper 32 of of the Event Missed register needing to 
                          be cleared
        
        missedQdma        Bit mask of Qdma events missed needing to be cleared
    @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status    
 *   @n                     Always return CSL_SOK
 *
 *   <b> Pre Condition </b>
 *   @n  All CSL_edma3Init(), CSL_edma3Open(), must be called successfully in  
 *       that order before CSL_edma3EventMissedClear() can be called.
 *
 *   <b> Post Condition </b>
 *    @n None
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
            CSL_Edma3Handle         hModule;
            CSL_Edma3HwSetup        hwSetup,gethwSetup;
            CSL_Edma3Obj            edmaObj;
            CSL_Edma3ParamHandle    hParamPing,hParamPong,hParamBasic;
            CSL_Edma3ChannelObj     ChObj;
            CSL_Edma3QueryInfo      info;
            CSL_Edma3CmdIntr        regionIntr;
            CSL_Edma3CmdDrae        regionAccess;
            CSL_Edma3ChannelHandle  hChannel;
            CSL_Edma3ParamSetup     myParamSetup;
            CSL_Edma3Context        context;
            CSL_Edma3ChannelAttr    chAttr;
            CSL_Edma3HwChannelSetup dmahwSetup[CSL_EDMA3_NUM_DMACH] = \
                                             CSL_EDMA3_DMACHANNELSETUP_DEFAULT;
            CSL_Edma3HwChannelSetup getdmahwSetup[CSL_EDMA3_NUM_DMACH];
            CSL_Status              status;
            Uint32                  i, passStatus = 1;   
            CSL_BitMask32           missedLo, missedHi,qdmamissed;
            
            // Module Initialization
            CSL_edma3Init(&context);
            
            // Module Level Open    
            hModule = CSL_edma3Open(&edmaObj,CSL_EDMA3,NULL,&status);
            
            // Module Setup
            hwSetup.dmaChaSetup  = &dmahwSetup[0];
            hwSetup.qdmaChaSetup = NULL;
            CSL_edma3HwSetup(hModule,&hwSetup);
         
            // Channel 0 Open in context of Shadow region 0
            chAttr.regionNum = CSL_EDMA3_REGION_0;
            chAttr.chaNum = CSL_EDMA3_CHA_DSP_EVT;
            hChannel = CSL_edma3ChannelOpen(&ChObj, CSL_EDMA3, &chAttr, &status);   
        
            //... Channel Setup, trigger

            // Obtain the events missed errors
            status = CSL_edma3GetEventMissed(hModule,&missedLo, &missedHi, \
                                             &qdmamissed);

            // Clear the error 
            CSL_edma3EventMissedClear(hModule,missedLo, missedHi,qdmamissed);
            ...
     @endverbatim
 * ===========================================================================
 */   
CSL_IDEF_INLINE CSL_Status CSL_edma3EventsMissedClear (
    CSL_Edma3Handle        hModule,
    CSL_BitMask32          missedLo,
    CSL_BitMask32          missedHi,
    CSL_BitMask32          missedQdma
)
{
    hModule->regs->EMCR   = missedLo;
    hModule->regs->EMCRH  = missedHi;
    hModule->regs->QEMCR  = missedQdma;
    
    return CSL_SOK;
}

/* =============================================================================
 *   @n@b CSL_edma3GetQueStatus
 *
 *   @b Description
 *   @n Obtains the queue status
 * 
 *   @b Arguments
 *   @verbatim
        hModule           Module Handle
        
        que               queue for which status needs to be obtained
        
        queStat           Structure containing fields for the watermark,  
                          threshold,number of valid entries, start pointer etc.
    @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status    
 *   @n                     Always return CSL_SOK
 *
 *   <b> Pre Condition </b>
 *   @n  All CSL_edma3Init(), CSL_edma3Open(), must be called successfully in  
 *       that order before CSL_edma3GetQueStatus() can be called.
 *
 *   <b> Post Condition </b>
 *    @n None
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
            CSL_Edma3Handle         hModule;
            CSL_Edma3HwSetup        hwSetup,gethwSetup;
            CSL_Edma3Obj            edmaObj;
            CSL_Edma3ParamHandle    hParamPing,hParamPong,hParamBasic;
            CSL_Edma3ChannelObj     ChObj;
            CSL_Edma3QueryInfo      info;
            CSL_Edma3CmdIntr        regionIntr;
            CSL_Edma3CmdDrae        regionAccess;
            CSL_Edma3ChannelHandle  hChannel;
            CSL_Edma3ParamSetup     myParamSetup;
            CSL_Edma3Context        context;
            CSL_Edma3ChannelAttr    chAttr;
            CSL_Edma3HwChannelSetup dmahwSetup[CSL_EDMA3_NUM_DMACH] = \
                                             CSL_EDMA3_DMACHANNELSETUP_DEFAULT;
            CSL_Edma3HwChannelSetup getdmahwSetup[CSL_EDMA3_NUM_DMACH];
            CSL_Status              status;
            Uint32                  i, passStatus = 1;   
            CSL_Edma3QueStat        queStat;
            
            // Module Initialization
            CSL_edma3Init(&context);
            
            // Module Level Open    
            hModule = CSL_edma3Open(&edmaObj,CSL_EDMA3,NULL,&status);
            
            // Module Setup
            hwSetup.dmaChaSetup  = &dmahwSetup[0];
            hwSetup.qdmaChaSetup = NULL;
            CSL_edma3HwSetup(hModule,&hwSetup);
         
            // Channel 0 Open in context of Shadow region 0
            chAttr.regionNum = CSL_EDMA3_REGION_0;
            chAttr.chaNum = CSL_EDMA3_CHA_DSP_EVT;
            hChannel = CSL_edma3ChannelOpen(&ChObj, CSL_EDMA3, &chAttr, &status);   
        
            // Get queue status
            CSL_edma3GetQueStatus(hModule,CSL_EDMA3_QUE_0, &queStat);
            ...
     @endverbatim
 * ===========================================================================
 */   
CSL_IDEF_INLINE CSL_Status  CSL_edma3GetQueStatus (
    CSL_Edma3Handle      hModule,
    CSL_Edma3Que         que,
    CSL_Edma3QueStat    *queStat
)
{
    Uint32 qstat = hModule->regs->QSTAT[que];
    
    queStat->waterMark = (CSL_BitMask16)CSL_FEXT(qstat,EDMA3CC_QSTAT_WM);
    queStat->numVal = (CSL_BitMask16)CSL_FEXT(qstat,EDMA3CC_QSTAT_NUMVAL);
    queStat->startPtr = (Uint8)CSL_FEXT(qstat,EDMA3CC_QSTAT_STRTPTR);
    queStat->exceed = (Bool)CSL_FEXT(qstat,EDMA3CC_QSTAT_THRXD);
    
    return CSL_SOK;
}

/* =============================================================================
 *   @n@b CSL_edma3GetActivityStatus
 *
 *   @b Description
 *   @n Obtains the Channel Controller Activity Status
 * 
 *   @b Arguments
 *   @verbatim
        hModule           Module Handle
        
        activityStat      Activity Status 
    @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status    
 *   @n                     Always return CSL_SOK
 *
 *   <b> Pre Condition </b>
 *   @n  All CSL_edma3Init(), CSL_edma3Open(), must be called successfully in  
 *       that order before CSL_edma3GetActivityStatus() can be called.
 *
 *   <b> Post Condition </b>
 *    @n None
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
            CSL_Edma3Handle         hModule;
            CSL_Edma3HwSetup        hwSetup,gethwSetup;
            CSL_Edma3Obj            edmaObj;
            CSL_Edma3ParamHandle    hParamPing,hParamPong,hParamBasic;
            CSL_Edma3ChannelObj     ChObj;
            CSL_Edma3QueryInfo      info;
            CSL_Edma3CmdIntr        regionIntr;
            CSL_Edma3CmdDrae        regionAccess;
            CSL_Edma3ChannelHandle  hChannel;
            CSL_Edma3ParamSetup     myParamSetup;
            CSL_Edma3Context        context;
            CSL_Edma3ChannelAttr    chAttr;
            CSL_Edma3HwChannelSetup dmahwSetup[CSL_EDMA3_NUM_DMACH] = \
                                             CSL_EDMA3_DMACHANNELSETUP_DEFAULT;
            CSL_Edma3HwChannelSetup getdmahwSetup[CSL_EDMA3_NUM_DMACH];
            CSL_Status              status;
            Uint32                  i, passStatus = 1;   
            CSL_Edma3ActivityStat   activityStat;
            
            // Module Initialization
            CSL_edma3Init(&context);
            
            // Module Level Open    
            hModule = CSL_edma3Open(&edmaObj,CSL_EDMA3,NULL,&status);
            
            // Module Setup
            hwSetup.dmaChaSetup  = &dmahwSetup[0];
            hwSetup.qdmaChaSetup = NULL;
            CSL_edma3HwSetup(hModule,&hwSetup);
         
            // Channel 0 Open in context of Shadow region 0
            chAttr.regionNum = CSL_EDMA3_REGION_0;
            chAttr.chaNum = CSL_EDMA3_CHA_DSP_EVT;
            hChannel = CSL_edma3ChannelOpen(&ChObj, CSL_EDMA3, &chAttr, &status);   
        
            // Obtain the Controller activity
            status = CSL_edma3GetActivityStatus(hModule,&activityStat);
            ...
     @endverbatim
 * ===========================================================================
 */   
CSL_IDEF_INLINE CSL_Status CSL_edma3GetActivityStatus (
    CSL_Edma3Handle          hModule,
    CSL_Edma3ActivityStat    *activityStat
)
{
    Uint32 ccstat = hModule->regs->CCSTAT;
    
    activityStat->active = (Bool)CSL_FEXT(ccstat,EDMA3CC_CCSTAT_ACTV);
    activityStat->evtActive = (Bool)CSL_FEXT(ccstat,EDMA3CC_CCSTAT_EVTACTV);
    activityStat->outstandingTcc = CSL_FEXT(ccstat,EDMA3CC_CCSTAT_COMPACTV);
    activityStat->qevtActive = (Bool)CSL_FEXT(ccstat,EDMA3CC_CCSTAT_QEVTACTV);
    activityStat->queActive = CSL_FEXTR(ccstat,23,16);
    activityStat->trActive = (Bool)CSL_FEXT(ccstat,EDMA3CC_CCSTAT_TRACTV); 
         
    return CSL_SOK;
}

/* =============================================================================
 *   @n@b CSL_edma3GetInfo
 *
 *   @b Description
 *   @n Obtains the Channel Controller's Information
 * 
 *   @b Arguments
 *   @verbatim
        hModule           Module Handle
        
        response          Placeholder for the configuration and revision
    @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status    
 *   @n                     Always return CSL_SOK
 *
 *   <b> Pre Condition </b>
 *   @n  All CSL_edma3Init(), CSL_edma3Open(), must be called successfully in  
 *       that order before CSL_edma3GetInfo() can be called.
 *
 *   <b> Post Condition </b>
 *    @n None
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
            CSL_Edma3Handle         hModule;
            CSL_Edma3HwSetup        hwSetup,gethwSetup;
            CSL_Edma3Obj            edmaObj;
            CSL_Edma3ParamHandle    hParamPing,hParamPong,hParamBasic;
            CSL_Edma3ChannelObj     ChObj;
            CSL_Edma3QueryInfo      info;
            CSL_Edma3CmdIntr        regionIntr;
            CSL_Edma3CmdDrae        regionAccess;
            CSL_Edma3ChannelHandle  hChannel;
            CSL_Edma3ParamSetup     myParamSetup;
            CSL_Edma3Context        context;
            CSL_Edma3ChannelAttr    chAttr;
            CSL_Edma3HwChannelSetup dmahwSetup[CSL_EDMA3_NUM_DMACH] = \
                                             CSL_EDMA3_DMACHANNELSETUP_DEFAULT;
            CSL_Edma3HwChannelSetup getdmahwSetup[CSL_EDMA3_NUM_DMACH];
            CSL_Status              status;
            Uint32                  i, passStatus = 1;   
            CSL_Edma3QueryInfo      info;
            
            // Module Initialization
            CSL_edma3Init(&context);
            
            // Module Level Open    
            hModule = CSL_edma3Open(&edmaObj,CSL_EDMA3,NULL,&status);
            
            // Get Module Info
            status = CSL_edma3GetInfo(hModule,&info);
            ...
     @endverbatim
 * ===========================================================================
 */   
CSL_IDEF_INLINE CSL_Status CSL_edma3GetInfo (
    CSL_Edma3Handle         hModule,
    CSL_Edma3QueryInfo      *response
)
{
    response->config   = hModule->regs->CCCFG; 
    response->revision = hModule->regs->PID; 
    
    return CSL_SOK;
}

/* =============================================================================
 *   @n@b CSL_edma3HwChannelEnable
 *
 *   @b Description
 *   @n This API enables the event associated with the Channel. Enables
 *      the channel
 * 
 *   @b Arguments
 *   @verbatim
        hEdma             Channel Handle
    @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status    
 *   @n                     Always return CSL_SOK
 *
 *   <b> Pre Condition </b>
 *   @n  All CSL_edma3Init(), CSL_edma3Open() and CSL_edma3ChannelOpen() must be
 *       called successfully in that order before CSL_edma3HwChannelEnable() 
 *       can be called.If the channel is opened in context of a Region,may also
 *       need to be invoked before this API. If a Shadow region is used it 
 *       should be ensured the DRAE/DRAEH is setup properly.
 *
 *   <b> Post Condition </b>
 *    @n None
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
            CSL_Edma3Handle             hModule;
            CSL_Edma3HwSetup            hwSetup,gethwSetup;
            CSL_Edma3Obj                edmaObj;
            CSL_Edma3ParamHandle        hParamPing,hParamPong,hParamBasic;
            CSL_Edma3ChannelObj         ChObj;
            CSL_Edma3QueryInfo          info;
            CSL_Edma3CmdIntr            regionIntr;
            CSL_Edma3CmdDrae            regionAccess;
            CSL_Edma3ChannelHandle      hChannel;
            CSL_Edma3ParamSetup         myParamSetup;
            CSL_Edma3Context            context;
            CSL_Edma3ChannelAttr        chAttr;
            CSL_Edma3HwChannelSetup     dmahwSetup[CSL_EDMA3_NUM_DMACH] = \
                                             CSL_EDMA3_DMACHANNELSETUP_DEFAULT;
            CSL_Edma3HwChannelSetup     getdmahwSetup[CSL_EDMA3_NUM_DMACH];
            CSL_Status                  status;
            Uint32                      i, passStatus = 1;
            
        
            // Module Initialization
            CSL_edma3Init(&context);
            
            // Module Level Open    
            hModule = CSL_edma3Open(&edmaObj,CSL_EDMA3,NULL,&status);
            
            // Module Setup
            hwSetup.dmaChaSetup  = &dmahwSetup[0];
            hwSetup.qdmaChaSetup = NULL;
            CSL_edma3HwSetup(hModule,&hwSetup);
         
            // Channel 0 Open in context of Shadow region 0
            chAttr.regionNum = CSL_EDMA3_REGION_0;
            chAttr.chaNum = CSL_EDMA3_CHA_DSP_EVT;
            hChannel = CSL_edma3ChannelOpen(&ChObj, CSL_EDMA3, &chAttr,&status);   
          
           // Channel Enable    
           status = CSL_edma3ChannelEnable(hChannel);
            ...
     @endverbatim
 * ===========================================================================
 */   
CSL_IDEF_INLINE CSL_Status  CSL_edma3ChannelEnable(
    CSL_Edma3ChannelHandle         hEdma
)
{
    /* Ensure this is a normal channel and not a QDMA channel */
    if (hEdma->region == CSL_EDMA3_REGION_GLOBAL) {
        if (hEdma->chaNum >= CSL_EDMA3_NUM_DMACH)
            hEdma->regs->QEESR = (1 << (hEdma->chaNum - CSL_EDMA3_NUM_DMACH));
        else if (hEdma->chaNum < 32)
            hEdma->regs->EESR = (1 << hEdma->chaNum);
        else 
            hEdma->regs->EESRH = (1 << (hEdma->chaNum - 32));
    } 
    else {
        if (hEdma->chaNum >= CSL_EDMA3_NUM_DMACH)
            hEdma->regs->SHADOW[hEdma->region].QEESR = \
                                   (1 << (hEdma->chaNum - CSL_EDMA3_NUM_DMACH));
        else if (hEdma->chaNum < 32)
            hEdma->regs->SHADOW[hEdma->region].EESR = (1 << hEdma->chaNum);
        else 
            hEdma->regs->SHADOW[hEdma->region].EESRH = 
                                                 (1 << (hEdma->chaNum - 32));
    }
    
    return CSL_SOK;     
}

/* =============================================================================
 *   @n@b CSL_edma3HwChannelDisable
 *
 *   @b Description
 *   @n This API disables the event associated with the Channel. Disables 
 *      the channel
 * 
 *   @b Arguments
 *   @verbatim
        hEdma             Channel Handle
    @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status    
 *   @n                     Always return CSL_SOK
 *
 *   <b> Pre Condition </b>
 *   @n  All CSL_edma3Init(), CSL_edma3Open() and CSL_edma3ChannelOpen() must be
 *       called successfully in that order before CSL_edma3HwChannelDisable() 
 *       can be called.If the channel is opened in context of a Region, 
 *       CSL_edma3RegionOpen() may also need to be invoked before this API. 
 *       If a Shadow region is used then care of the  DRAE settings must be 
 *       taken.
 *
 *   <b> Post Condition </b>
 *    @n None
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
            CSL_Edma3Handle             hModule;
            CSL_Edma3HwSetup            hwSetup,gethwSetup;
            CSL_Edma3Obj                edmaObj;
            CSL_Edma3ParamHandle        hParamPing,hParamPong,hParamBasic;
            CSL_Edma3ChannelObj         ChObj;
            CSL_Edma3QueryInfo          info;
            CSL_Edma3CmdIntr            regionIntr;
            CSL_Edma3CmdDrae            regionAccess;
            CSL_Edma3ChannelHandle      hChannel;
            CSL_Edma3ParamSetup         myParamSetup;
            CSL_Edma3Context            context;
            CSL_Edma3ChannelAttr        chAttr;
            CSL_Edma3HwChannelSetup     dmahwSetup[CSL_EDMA3_NUM_DMACH] = \
                                             CSL_EDMA3_DMACHANNELSETUP_DEFAULT;
            CSL_Edma3HwChannelSetup     getdmahwSetup[CSL_EDMA3_NUM_DMACH];
            CSL_Status                  status;
            Uint32                      i, passStatus = 1;
            
        
            // Module Initialization
            CSL_edma3Init(&context);
            
            // Module Level Open    
            hModule = CSL_edma3Open(&edmaObj,CSL_EDMA3,NULL,&status);
            
            // Module Setup
            hwSetup.dmaChaSetup  = &dmahwSetup[0];
            hwSetup.qdmaChaSetup = NULL;
            CSL_edma3HwSetup(hModule,&hwSetup);
         
            // Channel 0 Open in context of Shadow region 0
            chAttr.regionNum = CSL_EDMA3_REGION_0;
            chAttr.chaNum = CSL_EDMA3_CHA_DSP_EVT;
            hChannel = CSL_edma3ChannelOpen(&ChObj, CSL_EDMA3, &chAttr,&status);   
          
           // Channel Disable    
           status = CSL_edma3ChannelDisable(hChannel);
            ...
     @endverbatim
 * ===========================================================================
 */   
CSL_IDEF_INLINE CSL_Status  CSL_edma3ChannelDisable(
    CSL_Edma3ChannelHandle         hEdma
)
{
    /* Ensure this is a normal channel and not a QDMA channel */
    if (hEdma->region ==CSL_EDMA3_REGION_GLOBAL) {
        if (hEdma->chaNum >= CSL_EDMA3_NUM_DMACH)
            hEdma->regs->QEECR = (1 << (hEdma->chaNum - CSL_EDMA3_NUM_DMACH));
        else if (hEdma->chaNum < 32)
            hEdma->regs->EECR = (1 << hEdma->chaNum);
        else 
            hEdma->regs->EECRH = (1 << (hEdma->chaNum - 32));
    } else {
        if (hEdma->chaNum >= CSL_EDMA3_NUM_DMACH)
            hEdma->regs->SHADOW[hEdma->region].QEECR = 
                                  (1 << (hEdma->chaNum - CSL_EDMA3_NUM_DMACH));
        else if (hEdma->chaNum < 32)
            hEdma->regs->SHADOW[hEdma->region].EECR = (1 << hEdma->chaNum);
        else 
            hEdma->regs->SHADOW[hEdma->region].EECRH = 
                                                    (1 << (hEdma->chaNum - 32));
    }
    return CSL_SOK;     
}

/* =============================================================================
 *   @n@b CSL_edma3ChannelEventSet
 *
 *   @b Description
 *   @n Manually triggers an EDMA event by setting the ER register
 * 
 *   @b Arguments
 *   @verbatim
        hEdma             Channel Handle
    @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status    
 *   @n                     CSL_SOK            - Channel event set cmd success
 *   @n                     CSL_ESYS_INVPARAMS - Invalid Parameter 
 *
 *   <b> Pre Condition </b>
 *   @n  All CSL_edma3Init(), CSL_edma3Open() and CSL_edma3ChannelOpen() must be
 *       called successfully in that order before CSL_edma3ChannelEventSet() 
 *       can be called.If the channel is opened in context of a Region, If a  
 *       Shadow region is used then care of the  @a CSL_edma3RegionOpen() may
 *       also need to be invoked before this API.  DRAE settings must be taken.
 *
 *   <b> Post Condition </b>
 *    @n None
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
            CSL_Edma3Handle             hModule;
            CSL_Edma3HwSetup            hwSetup,gethwSetup;
            CSL_Edma3Obj                edmaObj;
            CSL_Edma3ParamHandle        hParamPing,hParamPong,hParamBasic;
            CSL_Edma3ChannelObj         ChObj;
            CSL_Edma3QueryInfo          info;
            CSL_Edma3CmdIntr            regionIntr;
            CSL_Edma3CmdDrae            regionAccess;
            CSL_Edma3ChannelHandle      hChannel;
            CSL_Edma3ParamSetup         myParamSetup;
            CSL_Edma3Context            context;
            CSL_Edma3ChannelAttr        chAttr;
            CSL_Edma3HwChannelSetup     dmahwSetup[CSL_EDMA3_NUM_DMACH] = \
                                             CSL_EDMA3_DMACHANNELSETUP_DEFAULT;
            CSL_Edma3HwChannelSetup     getdmahwSetup[CSL_EDMA3_NUM_DMACH];
            CSL_Status                  status;
            Uint32                      i, passStatus = 1;
            
        
            // Module Initialization
            CSL_edma3Init(&context);
            
            // Module Level Open    
            hModule = CSL_edma3Open(&edmaObj,CSL_EDMA3,NULL,&status);
            
            // Module Setup
            hwSetup.dmaChaSetup  = &dmahwSetup[0];
            hwSetup.qdmaChaSetup = NULL;
            CSL_edma3HwSetup(hModule,&hwSetup);
         
            // Channel 0 Open in context of Shadow region 0
            chAttr.regionNum = CSL_EDMA3_REGION_0;
            chAttr.chaNum = CSL_EDMA3_CHA_DSP_EVT;
            hChannel = CSL_edma3ChannelOpen(&ChObj, CSL_EDMA3, &chAttr,&status);   
          
           // Channel Event set    
           status = CSL_edma3ChannelEventSet(hChannel);
            ...
     @endverbatim
 * ===========================================================================
 */   
CSL_IDEF_INLINE CSL_Status  CSL_edma3ChannelEventSet (
    CSL_Edma3ChannelHandle         hEdma
)
{       
    if (hEdma->region == CSL_EDMA3_REGION_GLOBAL) {
        if (hEdma->chaNum < 32)
            hEdma->regs->ESR = (1 << hEdma->chaNum);
        else if (hEdma->chaNum < CSL_EDMA3_NUM_DMACH)
            hEdma->regs->ESRH = (1 << (hEdma->chaNum - 32));
        else    
            return CSL_ESYS_INVPARAMS;  
    } 
    else {
        if (hEdma->chaNum < 32)
            hEdma->regs->SHADOW[hEdma->region].ESR = (1 << hEdma->chaNum);
        else if (hEdma->chaNum < CSL_EDMA3_NUM_DMACH)
            hEdma->regs->SHADOW[hEdma->region].ESRH = 
                                                (1 << (hEdma->chaNum - 32));
        else    
            return CSL_ESYS_INVPARAMS;  
    }
               
    return CSL_SOK;
}

/* =============================================================================
 *   @n@b CSL_edma3ChannelEventClear
 *
 *   @b Description
 *   @n This API clears the event (if set) for the channel .Clears the EDMA 
 *      event by writing the ECR register for normal channels for QDMA channels
 * 
 *   @b Arguments
 *   @verbatim
        hEdma             Channel Handle
    @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status    
 *   @n                     CSL_SOK            - Channel event clear cmd success
 *   @n                     CSL_ESYS_INVPARAMS - Invalid Parameter
 *
 *   <b> Pre Condition </b>
 *   @n  All CSL_edma3Init(), CSL_edma3Open() and CSL_edma3ChannelOpen() must be
 *       called successfully in that order before CSL_edma3ChannelEventClear() 
 *       can be called.If the channel is opened in context of a Region, If a  
 *       Shadow region is used then care of the  @a CSL_edma3RegionOpen() may
 *       also need to be invoked before this API.  DRAE settings must be taken.
 *
 *   <b> Post Condition </b>
 *    @n None
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
            CSL_Edma3Handle             hModule;
            CSL_Edma3HwSetup            hwSetup,gethwSetup;
            CSL_Edma3Obj                edmaObj;
            CSL_Edma3ParamHandle        hParamPing,hParamPong,hParamBasic;
            CSL_Edma3ChannelObj         ChObj;
            CSL_Edma3QueryInfo          info;
            CSL_Edma3CmdIntr            regionIntr;
            CSL_Edma3CmdDrae            regionAccess;
            CSL_Edma3ChannelHandle      hChannel;
            CSL_Edma3ParamSetup         myParamSetup;
            CSL_Edma3Context            context;
            CSL_Edma3ChannelAttr        chAttr;
            CSL_Edma3HwChannelSetup     dmahwSetup[CSL_EDMA3_NUM_DMACH] = \
                                             CSL_EDMA3_DMACHANNELSETUP_DEFAULT;
            CSL_Edma3HwChannelSetup     getdmahwSetup[CSL_EDMA3_NUM_DMACH];
            CSL_Status                  status;
            Uint32                      i, passStatus = 1;
            
        
            // Module Initialization
            CSL_edma3Init(&context);
            
            // Module Level Open    
            hModule = CSL_edma3Open(&edmaObj,CSL_EDMA3,NULL,&status);
            
            // Module Setup
            hwSetup.dmaChaSetup  = &dmahwSetup[0];
            hwSetup.qdmaChaSetup = NULL;
            CSL_edma3HwSetup(hModule,&hwSetup);
         
            // Channel 0 Open in context of Shadow region 0
            chAttr.regionNum = CSL_EDMA3_REGION_0;
            chAttr.chaNum = CSL_EDMA3_CHA_DSP_EVT;
            hChannel = CSL_edma3ChannelOpen(&ChObj, CSL_EDMA3, &chAttr,&status);   
          
           // Channel Event clear    
           status = CSL_edma3ChannelEventClear(hChannel);
            ...
     @endverbatim
 * ===========================================================================
 */   
CSL_IDEF_INLINE CSL_Status  CSL_edma3ChannelEventClear (
    CSL_Edma3ChannelHandle         hEdma
)
{       
    if (hEdma->region ==CSL_EDMA3_REGION_GLOBAL) {
        if (hEdma->chaNum >= CSL_EDMA3_NUM_DMACH)
            return CSL_ESYS_INVPARAMS;
        else if (hEdma->chaNum < 32)
            hEdma->regs->ECR = (1 << hEdma->chaNum);
        else 
            hEdma->regs->ECRH = (1 << (hEdma->chaNum - 32));
    } 
    else {
        if (hEdma->chaNum >= CSL_EDMA3_NUM_DMACH)
            return CSL_ESYS_INVPARAMS;
            
        else {
            if (hEdma->chaNum < 32)
                hEdma->regs->SHADOW[hEdma->region].ECR = (1 << hEdma->chaNum);
            else 
                hEdma->regs->SHADOW[hEdma->region].ECRH = 
                                                (1 << (hEdma->chaNum - 32));
        }
    }
    
    return CSL_SOK;     
}

/* =============================================================================
 *   @n@b CSL_Edma3GetChannelStatus
 *
 *   @b Description
 *   @n Obtains the Channel Status i.e whether an even is set.
 * 
 *   @b Arguments
 *   @verbatim
        hChannel          Channel Handle
        
        response          Place holder for whether an event is set or not
    @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status    
 *   @n                     Always return CSL_SOK
 *
 *   <b> Pre Condition </b>
 *   @n  All CSL_edma3Init(), CSL_edma3Open() and CSL_edma3ChannelOpen() must be
 *       called successfully in that order before CSL_Edma3GetChannelStatus() 
 *       can be called.
 *
 *   <b> Post Condition </b>
 *    @n None
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
            CSL_Edma3Handle             hModule;
            CSL_Edma3HwSetup            hwSetup,gethwSetup;
            CSL_Edma3Obj                edmaObj;
            CSL_Edma3ParamHandle        hParamPing,hParamPong,hParamBasic;
            CSL_Edma3ChannelObj         ChObj;
            CSL_Edma3QueryInfo          info;
            CSL_Edma3CmdIntr            regionIntr;
            CSL_Edma3CmdDrae            regionAccess;
            CSL_Edma3ChannelHandle      hChannel;
            CSL_Edma3ParamSetup         myParamSetup;
            CSL_Edma3Context            context;
            CSL_Edma3ChannelAttr        chAttr;
            CSL_Edma3HwChannelSetup     dmahwSetup[CSL_EDMA3_NUM_DMACH] = \
                                             CSL_EDMA3_DMACHANNELSETUP_DEFAULT;
            CSL_Edma3HwChannelSetup     getdmahwSetup[CSL_EDMA3_NUM_DMACH];
            CSL_Status                  status;
            Uint32                      i, passStatus = 1;
            
        
            // Module Initialization
            CSL_edma3Init(&context);
            
            // Module Level Open    
            hModule = CSL_edma3Open(&edmaObj,CSL_EDMA3,NULL,&status);
            
            // Module Setup
            hwSetup.dmaChaSetup  = &dmahwSetup[0];
            hwSetup.qdmaChaSetup = NULL;
            CSL_edma3HwSetup(hModule,&hwSetup);
         
            // Channel 0 Open in context of Shadow region 0
            chAttr.regionNum = CSL_EDMA3_REGION_0;
            chAttr.chaNum = CSL_EDMA3_CHA_DSP_EVT;
            hChannel = CSL_edma3ChannelOpen(&ChObj, CSL_EDMA3, &chAttr,&status);   
          
            // Get Channel Status 
            status = CSL_Edma3GetChannelStatus(hChannel,&event);
            ...
     @endverbatim
 * ===========================================================================
 */   
CSL_IDEF_INLINE CSL_Status CSL_Edma3GetChannelStatus(
    CSL_Edma3ChannelHandle      hChannel,
    Bool                        *response
)
{
    if (hChannel->chaNum < CSL_EDMA3_NUM_DMACH) {
        if (hChannel->chaNum < 32) {
            if (CSL_FEXTR(hChannel->regs->ER,hChannel->chaNum,hChannel->chaNum))
                *response = TRUE;
            else
                *response = FALSE;
        } 
        else {
            if (CSL_FEXTR(hChannel->regs->ERH,hChannel->chaNum-32, \
                          hChannel->chaNum-32))
                *response = TRUE;
            else
                *response = FALSE;
        }
    }
    else {
        if (CSL_FEXTR(hChannel->regs->QER,hChannel->chaNum-CSL_EDMA3_NUM_DMACH,\
                      hChannel->chaNum-CSL_EDMA3_NUM_DMACH))
                *response = TRUE;
            else
                *response = FALSE;
    }
    return CSL_SOK;
}

/* =============================================================================
 *   @n@b CSL_Edma3GetChannelErrStatus
 *
 *   @b Description
 *   @n Obtains the Channel Error Status.
 * 
 *   @b Arguments
 *   @verbatim
        hChannel          Channel Handle
        
        response          Place holder for whether an event error is set or not
    @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status    
 *   @n                     Always return CSL_SOK
 *
 *   <b> Pre Condition </b>
 *   @n  All CSL_edma3Init(), CSL_edma3Open() and CSL_edma3ChannelOpen() must be
 *       called successfully in that order before CSL_Edma3GetChannelErrStatus() 
 *       can be called.
 *
 *   <b> Post Condition </b>
 *    @n None
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
            CSL_Edma3Handle             hModule;
            CSL_Edma3HwSetup            hwSetup,gethwSetup;
            CSL_Edma3Obj                edmaObj;
            CSL_Edma3ParamHandle        hParamPing,hParamPong,hParamBasic;
            CSL_Edma3ChannelObj         ChObj;
            CSL_Edma3QueryInfo          info;
            CSL_Edma3CmdIntr            regionIntr;
            CSL_Edma3CmdDrae            regionAccess;
            CSL_Edma3ChannelHandle      hChannel;
            CSL_Edma3ParamSetup         myParamSetup;
            CSL_Edma3Context            context;
            CSL_Edma3ChannelAttr        chAttr;
            CSL_Edma3HwChannelSetup     dmahwSetup[CSL_EDMA3_NUM_DMACH] = \
                                             CSL_EDMA3_DMACHANNELSETUP_DEFAULT;
            CSL_Edma3HwChannelSetup     getdmahwSetup[CSL_EDMA3_NUM_DMACH];
            CSL_Status                  status;
            Uint32                      i, passStatus = 1;
            
        
            // Module Initialization
            CSL_edma3Init(&context);
            
            // Module Level Open    
            hModule = CSL_edma3Open(&edmaObj,CSL_EDMA3,NULL,&status);
            
            // Module Setup
            hwSetup.dmaChaSetup  = &dmahwSetup[0];
            hwSetup.qdmaChaSetup = NULL;
            CSL_edma3HwSetup(hModule,&hwSetup);
         
            // Channel 0 Open in context of Shadow region 0
            chAttr.regionNum = CSL_EDMA3_REGION_0;
            chAttr.chaNum = CSL_EDMA3_CHA_DSP_EVT;
            hChannel = CSL_edma3ChannelOpen(&ChObj, CSL_EDMA3, &chAttr,&status);   
          
            // Get Channel Error Status 
            status = CSL_Edma3GetChannelErrStatus(hChannel,&event);
            ...
     @endverbatim
 * ===========================================================================
 */   
CSL_IDEF_INLINE CSL_Status CSL_Edma3GetChannelErrStatus (
    CSL_Edma3ChannelHandle      hChannel,
    CSL_Edma3ChannelErr         *errClr
)
{
    
    errClr->missed = FALSE;
    errClr->secEvt = FALSE;
    
    if (hChannel->chaNum < CSL_EDMA3_NUM_DMACH) {
        if (hChannel->chaNum < 32) {
            if (CSL_FEXTR(hChannel->regs->EMR,hChannel->chaNum, 
                hChannel->chaNum))
                    errClr->missed = TRUE;
            if (CSL_FEXTR(hChannel->regs->SER,hChannel->chaNum, 
                hChannel->chaNum))
                    errClr->secEvt = TRUE;
        } 
        else {
            if (CSL_FEXTR(hChannel->regs->EMRH,hChannel->chaNum-32, \
                          hChannel->chaNum-32))
                errClr->missed = TRUE;
            if (CSL_FEXTR(hChannel->regs->SERH,hChannel->chaNum-32, \
                          hChannel->chaNum-32))
                errClr->secEvt = TRUE;
        }
    }
    else {
        if (CSL_FEXTR(hChannel->regs->QEMR,hChannel->chaNum-CSL_EDMA3_NUM_DMACH, 
                      hChannel->chaNum-CSL_EDMA3_NUM_DMACH))
                errClr->missed = TRUE;
        if (CSL_FEXTR(hChannel->regs->QSER,hChannel->chaNum-CSL_EDMA3_NUM_DMACH,
                      hChannel->chaNum-CSL_EDMA3_NUM_DMACH))
                errClr->secEvt = TRUE;
    }
    
    return CSL_SOK;
}
/* =============================================================================
 *   @n@b CSL_Edma3ChannelErrorClear
 *
 *   @b Description
 *   @n Clears the channel and associated error conditions:- Clears 
 *      EMR/EMRH/QEMR and  SER/SERH/QSER. 
 *
 *   @b Arguments
 *   @verbatim
        hChannel            Channel Handle
        
        errClr              Channel clear error bit
    @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status    
 *   @n                     Always return CSL_SOK
 *
 *   <b> Pre Condition </b>
 *   @n  All CSL_edma3Init(), CSL_edma3Open() and CSL_edma3ChannelOpen() must be
 *       called successfully in that order before CSL_Edma3ChannelErrorClear() 
 *       can be called.
 *
 *   <b> Post Condition </b>
 *    @n None
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
            CSL_Edma3Handle             hModule;
            CSL_Edma3HwSetup            hwSetup,gethwSetup;
            CSL_Edma3Obj                edmaObj;
            CSL_Edma3ParamHandle        hParamPing,hParamPong,hParamBasic;
            CSL_Edma3ChannelObj         ChObj;
            CSL_Edma3QueryInfo          info;
            CSL_Edma3CmdIntr            regionIntr;
            CSL_Edma3CmdDrae            regionAccess;
            CSL_Edma3ChannelHandle      hChannel;
            CSL_Edma3ParamSetup         myParamSetup;
            CSL_Edma3Context            context;
            CSL_Edma3ChannelAttr        chAttr;
            CSL_Edma3HwChannelSetup     dmahwSetup[CSL_EDMA3_NUM_DMACH] = \
                                             CSL_EDMA3_DMACHANNELSETUP_DEFAULT;
            CSL_Edma3HwChannelSetup     getdmahwSetup[CSL_EDMA3_NUM_DMACH];
            CSL_Status                  status;
            Uint32                      i, passStatus = 1;
            CSL_Edma3ChannelErr         errClr;
        
            // Module Initialization
            CSL_edma3Init(&context);
            
            // Module Level Open    
            hModule = CSL_edma3Open(&edmaObj,CSL_EDMA3,NULL,&status);
            
            // Module Setup
            hwSetup.dmaChaSetup  = &dmahwSetup[0];
            hwSetup.qdmaChaSetup = NULL;
            CSL_edma3HwSetup(hModule,&hwSetup);
         
            // Channel 0 Open in context of Shadow region 0
            chAttr.regionNum = CSL_EDMA3_REGION_0;
            chAttr.chaNum = CSL_EDMA3_CHA_DSP_EVT;
            hChannel = CSL_edma3ChannelOpen(&ChObj, CSL_EDMA3, &chAttr,&status);   
          
            // Clear Channel Error, clears SER/SERH/QSER and EMR/EMRH/QEMR.
            CSL_Edma3ChannelErrorClear(hChannel, &errClr);
            ...
     @endverbatim
 * ===========================================================================
 */   
CSL_IDEF_INLINE CSL_Status CSL_Edma3ChannelErrorClear (
    CSL_Edma3ChannelHandle      hChannel,
    CSL_Edma3ChannelErr         *errClr
)
{
    if (errClr->missed == TRUE) {
        if (hChannel->chaNum < CSL_EDMA3_NUM_DMACH) {
            if (hChannel->chaNum < 32) {
                hChannel->regs->EMCR = 1 << hChannel->chaNum;
                if (errClr->secEvt == TRUE)
                    hChannel->regs->SECR = 1 << hChannel->chaNum;
            } 
            else {
                hChannel->regs->EMCRH = 1 << (hChannel->chaNum - 32);
                if (errClr->secEvt == TRUE)
                    hChannel->regs->SECRH = 1 << (hChannel->chaNum - 32);
            }
        }
        else {
           hChannel->regs->QEMR = 1 << (hChannel->chaNum - CSL_EDMA3_NUM_DMACH);
           if (errClr->secEvt == TRUE)
               hChannel->regs->QSECR = 1 << \
                                      (hChannel->chaNum - CSL_EDMA3_NUM_DMACH);

        }
    }
    
    return CSL_SOK;
}

#endif
