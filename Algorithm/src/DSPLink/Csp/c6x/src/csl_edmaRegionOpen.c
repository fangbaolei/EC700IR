/*****************************************************\
 *  Copyright 2004, Texas Instruments Incorporated.  *
 *  All rights reserved.                             *
 *  Restricted rights to use, duplicate or disclose  *
 *  this   code   are  granted   through  contract.  *
 *                                                   *
 * "@(#) PSP/CSL  3.0.0.0  (2003-09-30)              *
\*****************************************************/
/* =========================================================
 *   @file  csl_edmaRegion.c
 *
 *   @path  $(CSLPATH)\src\edma
 *
 *   @desc  EDMA module
 *
 */
/* File contains all the related functionality to do with Region region acquisitions */

#include <csl_edma.h>


/** Acquires the shadow region requested. If the shadow region can be allocated 
 *  a valid handle to the shadow region is returned else a NULL handle is returned. 
 *
 * <b> Usage Constraints: </b>
 * CSL_edmaInit(), CSL_edmaOpen() must have been called successfully before this function can 
 * be called.The user must allocate space for this and this API fills in the system 
 * specified fields.
 * 
 *
 * @b Example:
 * \code
	CSL_EdmaObj ModuleObj;
	CSL_EdmaHandle hModHndl;
	CSL_EdmaRegionParam regParam;
	CSL_EdmaRegionObj RegionObj;
	CSL_EdmaRegionHndl shHndl;
	CSL_Status status;
	CSL_EdmaChanObj ChObj;
	CSL_EdmaChanHandle chHndl;
	
    // Initialization
	CSL_edmaInit(NULL);
	
	// Open	Module
	edmaHndl = CSL_edmaOpen(NULL,CSL_EDMA_0,NULL,&modStatus);
	
	// Region Open 
	regParam.regionNum = CSL_EDMA_REGION_0;
	shHndl = CSL_edmaRegionOpen(&RegionObj,CSL_EDMA_0,&regParam,&status);			

   \endcode
 *
 * @return Handle to the shadow region.
 *
 */

#pragma CODE_SECTION (CSL_edmaRegionOpen, ".text:csl_section:edma"); 
CSL_EdmaRegionHandle  CSL_edmaRegionOpen(
	/** pointer to the object that holds reference to the shadow region opened */
    CSL_EdmaRegionObj           *edmaShObj,
	/** instance of EDMA whose shadow region is requested for open */
    CSL_InstNum                 edmaNum,
	/** region requested */
    CSL_EdmaRegionParam         *param,
	/** points to the placeholder for the status (success/errors) of the call */
    CSL_Status                  *status
) 
{
	CSL_EdmaRegionHandle pShHndl = NULL;
	
	CSL_Status st = CSL_SOK;
	CSL_EdmaModuleBaseAddress  baseAddress;
    if (param != NULL) {
    	CSL_edmaccGetModuleBaseAddr(edmaNum,NULL,&baseAddress);
		edmaShObj->shregs = &baseAddress.regs->SHADOW[param->regionNum];
		edmaShObj->region = param->regionNum;
		edmaShObj->edmaNum = edmaNum;
		pShHndl = edmaShObj;
	} else {
    	st = CSL_ESYS_FAIL;
    }
    if (status != NULL)
    	*status = st;
	return pShHndl;		
}
