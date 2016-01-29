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


/** Closes the shadow region requested. 
 * The shadow region is returned to the available pool of shadow regions.
 *
 * <b> Usage Constraints: </b>
 *   CSL_edmaInit(),  CSL_edmaOpen(),  CSL_edmaRegionOpen() must have been called successfully before this function can 
 * be called.
 *
 * @b Example:
 * \code
 *  CSL_EdmaObj ModuleObj;
 *	CSL_EdmaHandle hModHndl;
 *	
 *	CSL_EdmaRegionObj RegionObj;
 *	CSL_EdmaRegionHndl shHndl;
 *	
 *	CSL_EdmaChanObj ChObj;
 *	CSL_EdmaChanHandle chHndl;
 *	
 * // Initialization
	CSL_edmaInit(NULL);
	
	// Open	Module
	edmaHndl = CSL_edmaOpen(NULL,CSL_EDMA_0,NULL,&modStatus);
	
	// Region Open 
	regParam.regionNum = CSL_EDMA_REGION_0;
	shHndl = CSL_edmaRegionOpen(&RegionObj,CSL_EDMA_0,&regParam,&status);	
 *	
 *  // Close the Region Region
 *  CSL_edmaRegionClose(shHndl);	
 * \endcode
 *
 * @return Status (CSL_SOK/CSL_ESYS_INVPARAMS)
 */
#pragma CODE_SECTION (CSL_edmaRegionClose, ".text:csl_section:edma"); 
CSL_Status  CSL_edmaRegionClose(
	/** pointer to the object that holds reference to the shadow region opened */
    CSL_EdmaRegionHandle        hShHndl
) 
{
	CSL_Status st = CSL_SOK;
	if (hShHndl == NULL)
		st = CSL_ESYS_INVPARAMS;
	return st;
}
