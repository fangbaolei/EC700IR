/*****************************************************\
 *  Copyright 2004, Texas Instruments Incorporated.  *
 *  All rights reserved.                             *
 *  Restricted rights to use, duplicate or disclose  *
 *  this   code   are  granted   through  contract.  *
 *                                                   *
 * "@(#) PSP/CSL  3.0.0.0  (2003-09-30)              *
\*****************************************************/
/* =========================================================
 *   @file  csl_edmaChannelOpen.c
 *
 *   @path  $(CSLPATH)\src\edma
 *
 *   @desc  EDMA module 
 *
 *
 */

#include <csl_edma.h>

#pragma CODE_SECTION (CSL_edmaChannelOpen, ".text:csl_section:edma");

/** Reserves the specified EDMA Channel for use. The channel can be
 * re-opened anytime after it has been normally closed if so
 * required. The handle returned by this call is input as an
 * essential argument for many of the APIs described for this
 * module.
 *
 * <b> Usage Constraints: </b>
 * All @a CSL_edmaInit(), @a CSL_edmaOpen() must be 
 * invoked successfully in that order before this API can be invoked.
 * If the channel is opened in context of a Region, @a CSL_edmaRegionOpen()
 * may also need to be invoked before this API. If a Shadow region is used then care of the 
 * DRAE settings must be taken.
 *
 * Memory for the   CSL_EdmaChanObj must be allocated outside
 * this call.
 *
 * @b Example:
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
                            
	\endcode
 *
 * @return Handle   CSL_EdmaHandle to the requested channel instance
 * of the specified EDMA if the call is successful, else a NULL is
 * returned.
 *
 */
CSL_EdmaChanHandle  CSL_edmaChannelOpen(
    /** pointer to the object that holds reference to the channel
    * instance of the Specified DMA */
    CSL_EdmaChanObj            *edmaObj,
    /** instance of EDMA whose channel is requested */
    CSL_InstNum                edmaNum,
    /** instance of Channel requested */
    CSL_EdmaChannelParam       *param,
    /** This returns the status (success/errors) of the call */
    CSL_Status                 *status
)
{
    CSL_EdmaChanHandle   hEdma = (CSL_EdmaChanHandle)NULL;
    
    CSL_Status st = CSL_SOK;
    CSL_EdmaModuleBaseAddress  baseAddress;
    if (param != NULL) {
    	CSL_edmaccGetModuleBaseAddr(edmaNum,NULL,&baseAddress);
    	edmaObj->ccregs = baseAddress.regs;
    	edmaObj->edmaNum = edmaNum;
    	edmaObj->chaNum = param->chaNum;
    	edmaObj->region = param->regionNum;
    	hEdma = (CSL_EdmaChanHandle)edmaObj;
    } else {
    	st = CSL_ESYS_FAIL;    
    }
	if (status != NULL)
		*status = st;
	return(hEdma);
}


