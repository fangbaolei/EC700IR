/*****************************************************\
 *  Copyright 2004, Texas Instruments Incorporated.  *
 *  All rights reserved.                             *
 *  Restricted rights to use, duplicate or disclose  *
 *  this   code   are  granted   through  contract.  *
 *                                                   *
 * "@(#) PSP/CSL  3.0.0.0  (2003-09-30)              *
\*****************************************************/
/* =========================================================
 *   @file  csl_edmaOpen.c
 *
 *   @path  $(CSLPATH)\src\edma
 *
 *   @desc  EDMA module 
 *
 */
#include <csl_edma.h>

#pragma CODE_SECTION (CSL_edmaClose, ".text:csl_section:edma");

/** This is a module level close require to invalidate the module handle
 *  The module handle must not be used after this API call.
 *
 * <b> Usage Constraints: </b>
 * Module Init and open must have done prior to this call.
 *
 * @b Example:
   \code


   CSL_EdmaHandle     edmaHndl;          
   ...
   edmaHndl = CSL_edmaOpen(NULL,CSL_EDMA_0,NULL,&modStatus);
   ...
   CSL_edmaClose(edmaHndl);
   \endcode
 *
 * @return Status (CSL_SOK/CSL_ESYS_INVPARAMS)
 *
 */

CSL_Status CSL_edmaClose(
    CSL_EdmaHandle			 hEdma
)
{
	return CSL_SOK;
}

