/*****************************************************\
 *  Copyright 2004, Texas Instruments Incorporated.  *
 *  All rights reserved.                             *
 *  Restricted rights to use, duplicate or disclose  *
 *  this   code   are  granted   through  contract.  *
 *                                                   *
 * "@(#) PSP/CSL  3.0.0.0  (2003-09-30)              *
\*****************************************************/
/** @file csl_edmaGetHwRegionStatus.c
 *
 *  @brief    File for functional layer of CSL API @a CSL_edmaGetHwRegionStatus()
 *
 *  @date 
 *  @author Ruchika Kharwar
 */

#include <csl_edma.h>
#include <csl_edmaAux.h>

#pragma CODE_SECTION (CSL_edmaGetHwRegionStatus, ".text:csl_section:edma");

/** Queries the status paramaters of the shadow region.
 *
 * <b> Usage Constraints: </b>
 *   CSL_edmaInit(),   CSL_edmaOpen(),  CSL_edmaRegionOpen() must have been called successfully before this function can 
 * be called.The user must allocate space for this and this API fills in the system 
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
	
	// Region 0 Open
	regParam.regionNum = CSL_EDMA_REGION_0;	
	shHndl = CSL_edmaRegionOpen(&RegionObj,CSL_EDMA_0,&regParam,&status);				

	// Query the shadow region 
	CSL_edmaGetHwRegionStatus(shHndl, CSL_EDMA_QUERY_REGION_INTERRUPT_PENDSTATUS, intrPend);
	
   \endcode
 *
 * @return Status (CSL_SOK/CSL_ESYS_BADHANDLE/CSL_ESYS_INVPARAMS)
 *
 */
CSL_Status  CSL_edmaGetHwRegionStatus(
   /** Pointer to the object that holds reference to the
     * instance of EDMA */
	CSL_EdmaRegionHandle                 hSh,
	/** The query to this API which indicates the status/setup
     * to be returned */
	CSL_EdmaHwRegionStatusQuery    myQuery,
	/** Placeholder to return the status; @a void* casted */
	void                           *response
)
{
    CSL_Status st = CSL_SOK;
    if (hSh == NULL)
		return CSL_ESYS_BADHANDLE;
    if (response == NULL)
    	return CSL_ESYS_INVPARAMS;
	switch(myQuery) {
  		case CSL_EDMA_QUERY_REGION_INTERRUPT_PENDSTATUS:
			st = CSL_edmaGetRegionIntrPendStatus(hSh,&((CSL_BitMask32*)response)[0],&((CSL_BitMask32*)response)[1]);
			break;
		default:
	        st = CSL_ESYS_INVQUERY;
	        break;
    }
    return st;
}
