/*****************************************************\
 *  Copyright 2004, Texas Instruments Incorporated.  *
 *  All rights reserved.                             *
 *  Restricted rights to use, duplicate or disclose  *
 *  this   code   are  granted   through  contract.  *
 *                                                   *
 * "@(#) PSP/CSL  3.0.0.0  (2003-09-30)              *
\*****************************************************/
/* =========================================================
 *   @file  csl_edmaHwChannelSetup.c
 *
 *   @path  $(CSLPATH)\src\edma
 *
 *   @desc  EDMA module 
 *
 *
 */

#include <csl_edma.h>
#include <csl_edmaAux.h>

#pragma CODE_SECTION (CSL_edmaHwChannelSetup, ".text:csl_section:edma");

/** Does the setup for the channel.
 *
 * viz the parameter entry mapping, the trigger word setting (if QDMA channels) and 
 * the event que mapping of the channel
 * 
 * <b> Usage Constraints: </b>
 * All   CSL_edmaInit(),   CSL_edmaOpen() and   CSL_edmaChannelOpen() must be
 * called successfully in that order before   csl_edmaHwChannelSetup() 
 * can be called. 
 * 
 @b Example:
   \code

    CSL_EdmaHandle hModule;
	
	CSL_EdmaChanObj ChObj;
	CSL_EdmaChanHandle hChannel;
	CSL_EdmaChannelParam chParam;
    CSL_Status         edmaStatus;	
    
    // Initialization
	CSL_edmaInit();
	// Module Level Open	
	 hModule = CSL_edmaOpen(NULL,CSL_EDMA_0,NULL,&edmaStatus);
	
    ...
    // Channel 0 is opened which is allocated to Region Region 0
    chParam.regionNum = CSL_EDMA_REGION_GLOBAL;
    chParam.chaNum = CSL_EDMA_CHA0;
    hChannel = CSL_edmaChannelOpen(&edmaObj,
                            CSL_EDMA_0,
                            &chParam,
                            &edmaStatus);
    
	// Channel Setup
	setup.que =  CSL_EDMA_EVT_QUE0;
	setup.paramEntry = 0;
	setup.triggerWord = CSL_EDMA_TRIGWORD_NONE;
	CSL_edmaHwChannelSetup(edmaHndl,&setup);
   
   \endcode
 * 
 * @return Status (CSL_SOK/CSL_ESYS_INVPARAMS/CSL_ESYS_BADHANDLE)
 * 
 */

CSL_Status  CSL_edmaHwChannelSetup(
    /** pointer to the object that holds reference to the channel
    * instance of the Specified DMA */
    CSL_EdmaChanHandle        hCh,
    /** pointer to the setup structure */
    CSL_EdmaHwChannelSetup    *setup
)
{
    if (hCh==NULL)
    	return CSL_ESYS_BADHANDLE;
    if (setup==NULL)
    	return CSL_ESYS_INVPARAMS;
    CSL_edmaChannelSetEvtQue(hCh,setup->que);
   	if (hCh->chaNum  < CSL_EDMA_NUM_DMACH) {
#if CSL_EDMA_CHMAPEXIST		
		CSL_FINS(hCh->ccregs->DCHMAP[hCh->chaNum], EDMACC_DCHMAP_PAENTRY,setup->paramEntry);			
#endif		
	} else
		hCh->ccregs->QCHMAP[hCh->chaNum-CSL_EDMA_NUM_DMACH] = CSL_FMK(EDMACC_QCHMAP_PAENTRY,setup->paramEntry) |  CSL_FMK(EDMACC_QCHMAP_TRWORD,setup->triggerWord);	
	return CSL_SOK;	
}


