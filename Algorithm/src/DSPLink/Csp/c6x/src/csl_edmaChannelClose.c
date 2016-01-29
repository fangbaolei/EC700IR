/*****************************************************\
 *  Copyright 2004, Texas Instruments Incorporated.  *
 *  All rights reserved.                             *
 *  Restricted rights to use, duplicate or disclose  *
 *  this   code   are  granted   through  contract.  *
 *                                                   *
 * "@(#) PSP/CSL  3.0.0.0  (2003-09-30)              *
\*****************************************************/
/* =========================================================
 *   @file  csl_edmaChannelClose.c
 *
 *   @path  $(CSLPATH)\src\edma
 *
 *   @desc  EDMA module 
 *
 */
#include <csl_edma.h>

#pragma CODE_SECTION (CSL_edmaChannelClose, ".text:csl_section:edma");

/** The channel cannot be accessed anymore using the handle.
 *
 * <b> Usage Constraints: </b>
 * All @a CSL_edmaInit(), @a CSL_edmaOpen(), @a CSL_edmaChannelOpen() must be 
 * invoked successfully in that order before this API can be invoked.
 * If the channel is opened in context of a Region, @a CSL_edmaRegionOpen()
 * may also need to be invoked before this API. If a Shadow region is used then care of the 
 * DRAE settings must be taken.
 *
 * @b Example:
 * \code
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
	...
	// EDMA Channel Close Call
    edmaStatus = CSL_edmaChannelClose(hChannel);

   \endcode
 *
 * @return Status (CSL_SOK/CSL_ESYS_BADHANDLE)
 *
 */
CSL_Status  CSL_edmaChannelClose(
   /**  a handle to the requested channel instance of the specified EDMA 
    *   obtained through the   CSL_edmaOpen() call */
    CSL_EdmaChanHandle             hEdma
)
{
    CSL_Status st = CSL_SOK;
    if (hEdma == NULL)
        return CSL_ESYS_BADHANDLE;
    return st;
}
