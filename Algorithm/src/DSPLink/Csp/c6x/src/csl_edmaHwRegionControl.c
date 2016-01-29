/*****************************************************\
 *  Copyright 2004, Texas Instruments Incorporated.  *
 *  All rights reserved.                             *
 *  Restricted rights to use, duplicate or disclose  *
 *  this   code   are  granted   through  contract.  *
 *                                                   *
 * "@(#) PSP/CSL  3.0.0.0  (2003-09-30)              *
\*****************************************************/
/** @file csl_edmaHwRegionControl.c
 *
 *  @brief    File for functional layer of CSL API @a CSL_edmaHwControl()
 *
 *  Description
 *    - The @a CSL_edmaHwControl() function definition & it's associated
 *      functions
 *  @date 29th May, 2004
 *  @author Ruchika Kharwar
 */

#include <csl_edma.h>
#include <csl_edmaAux.h>

#pragma CODE_SECTION (CSL_edmaHwRegionControl, ".text:csl_section:edma");

/** Takes a command with an optional argument & implements it. This function is
 * used to carry out/set attributes of the Shadow Region registers.
 * 
 * <b> Usage Constraints: </b>
 *   CSL_edmaInit(),   CSL_edmaOpen(),   CSL_edmaRegionOpen() must have been called 
 * successfully before this function can be called.
 * The user must allocate space for this and this API fills in the system 
 * specified fields.
 * 
 *
 * @b Example:
 * \code
	
	CSL_EdmaRegionObj RegionObj;
	CSL_EdmaRegionHndl shHndl;
	CSL_EdmaRegionParam regParam;
	
	CSL_EdmaChanObj ChObj;
	CSL_EdmaChanHandle chHndl;
	
	CSL_BitMask32 intrEnable[2];
    CSL_EdmaHandle     hModHndl;          
    CSL_Status modStatus;
   
   // Initialize
   modStatus = CSL_edmaInit(NULL);
   ...
   
   hModHndl = CSL_edmaOpen(NULL,CSL_EDMA_0,NULL,&modStatus);
	
	// Hw Status Revision  
	modStatus = CSL_edmaGetHwStatus(hModHndl,CSL_EDMA_QUERY_REVISION,&revision);
	
	// Region 0 Open
	regParam.regionNum = CSL_EDMA_REGION_0;	
	shHndl = CSL_edmaRegionOpen(&RegionObj,CSL_EDMA_0,&regParam,&status);			

	// Enables the Shadow Region interrupts
	intrEnable[0] = 0x0000ffff;
	intrEnable[1] = 0xffff0000;
	
	CSL_edmaHwRegionControl(shHndl, CSL_EDMA_CMD_REGION_INTERRUPT_ENABLE, intrEnable);
	
   \endcode
 *
 * @return Status (CSL_SOK/CSL_ESYS_BADHANDLE/CSL_ESYS_INVCMD)
 *
 */

CSL_Status  CSL_edmaHwRegionControl
(
	/** Pointer to the object that holds reference to the
     * instance of EDMA requested after the call */
	CSL_EdmaRegionHandle           hSh,
	/** The command to this API which indicates the action to be taken */
	CSL_EdmaHwRegionControlCmd     cmd,
	/** Optional argument @a void* casted */
	void                           *cmdArg
)
{ 
    CSL_Status st;
    if (hSh == NULL) 
		return CSL_ESYS_BADHANDLE;
	switch(cmd) {
		case CSL_EDMA_CMD_REGION_INTERRUPT_CLEAR:
			st = CSL_edmaRegionInterruptClear(hSh,((CSL_BitMask32*)(cmdArg))[0],((CSL_BitMask32*)(cmdArg))[1]);
			break;			
		case CSL_EDMA_CMD_REGION_INTERRUPT_ENABLE:
			st = CSL_edmaRegionInterruptEnable(hSh,((CSL_BitMask32*)(cmdArg))[0],((CSL_BitMask32*)(cmdArg))[1]);
			break;			
		case CSL_EDMA_CMD_REGION_INTERRUPT_DISABLE:
			st = CSL_edmaRegionInterruptDisable(hSh,((CSL_BitMask32*)(cmdArg))[0],((CSL_BitMask32*)(cmdArg))[1]);
			break;	
		case CSL_EDMA_CMD_REGION_INTERRUPT_EVAL:
			st = CSL_edmaRegionInterruptEval(hSh);
			break;			
	    default:
			st = CSL_ESYS_INVCMD;
		break;
        }
    return st;
}

