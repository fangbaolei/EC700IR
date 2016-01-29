/*****************************************************\
 *  Copyright 2004, Texas Instruments Incorporated.  *
 *  All rights reserved.                             *
 *  Restricted rights to use, duplicate or disclose  *
 *  this   code   are  granted   through  contract.  *
 *                                                   *
 * "@(#) PSP/CSL  3.0.0.0  (2003-09-30)              *
\*****************************************************/
/** @file csl_edmaHwChannelControl.c
 *
 *    @brief    File for functional layer of CSL API 
 *    @a CSL_edmaHwChannelControl()
 *
 *  Description
 *    - The @a csl_edmaHwChannelControl() function definition &
 *     it's associated functions
 *  @date 29thMay, 2004
 *  @author Ruchika Kharwar
 */

#include <csl_edma.h>
#include <csl_edmaAux.h>

#pragma CODE_SECTION (CSL_edmaHwChannelControl, ".text:csl_section:edma");

/** Takes a command with an optional argument & implements it. This function is
 * used to carry out the different operations performed by EDMA.
 * 
 * <b> Usage Constraints: </b>
 * All @a CSL_edmaInit(), @a CSL_edmaOpen(), @a CSL_edmaChannelOpen() must be 
 * invoked successfully in that order before this API can be invoked.
 * If the channel is opened in context of a Region, @a CSL_edmaRegionOpen()
 * may also need to be invoked before this API. If a Shadow region is used then care of the 
 * DRAE settings must be taken.
 * 
 @b Example:
   \code

    // Initialization

	CSL_EdmaChanObj ChObj;
	CSL_EdmaChanHandle chHndl;
	
    CSL_EdmaHandle     hModHndl;          
    CSL_Status modStatus;
   
    // Initialize
    modStatus = CSL_edmaInit(NULL);
    ...
    // Open Module    
    hModHndl = CSL_edmaOpen(NULL,CSL_EDMA_0,NULL,&modStatus);
	
	// Open Channel 0
	chParam.regionNum = CSL_EDMA_REGION_GLOBAL;
    chParam.chaNum = CSL_EDMA_CHA0;
    edmaHndl = CSL_edmaChannelOpen(&edmaObj,
                            CSL_EDMA_0,
                            &chParam,
                            &edmaStatus);
   status = CSL_edmaHwChannelControl(edmaHndl,CSL_EDMA_CMD_CHANNEL_ENABLE,NULL);
   
   \endcode
 * 
 * @return Status (CSL_SOK/CSL_ESYS_INVPARAMS/CSL_ESYS_BADHANDLE)
 * 
 */

CSL_Status  CSL_edmaHwChannelControl
(
	/** Channel handle */
	CSL_EdmaChanHandle           hCh,
	/** The command to this API which indicates the action to be taken */
	CSL_EdmaHwChannelControlCmd     cmd,
	/** Optional argument @a void* casted */
	void                           *cmdArg
)
{ 
    CSL_Status st;
    if (hCh == NULL) 
		return CSL_ESYS_BADHANDLE;
	switch(cmd) {
		case CSL_EDMA_CMD_CHANNEL_ENABLE:
		    st = CSL_edmaChannelEnable(hCh);
			break;
		case CSL_EDMA_CMD_CHANNEL_DISABLE:
			st = CSL_edmaChannelDisable(hCh);
			break;
		case CSL_EDMA_CMD_CHANNEL_SET:
			st = CSL_edmaChannelEventSet(hCh);
			break;
		case CSL_EDMA_CMD_CHANNEL_CLEAR:
			st = CSL_edmaChannelEventClear(hCh);
			break;			
		default:
			st = CSL_ESYS_INVPARAMS;
	}		
    return st;
}

