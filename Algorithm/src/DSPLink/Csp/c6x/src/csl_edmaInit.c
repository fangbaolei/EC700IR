/*****************************************************\
 *  Copyright 2004, Texas Instruments Incorporated.  *
 *  All rights reserved.                             *
 *  Restricted rights to use, duplicate or disclose  *
 *  this   code   are  granted   through  contract.  *
 *                                                   *
 * "@(#) PSP/CSL  3.0.0.0  (2003-09-30)              *
\*****************************************************/
/* ==========================================================
 *   @file  csl_edmaInit.c
 *
 *   @path  $(CSLPATH)\src\edma
 *
 *   @desc    EDMA module 
 *
 */

#include <csl_edma.h>


#pragma CODE_SECTION (CSL_edmaInit, ".text:csl_section:edma");

/** This function is idempotent in that calling it many times is same as
 * calling it once. This function initializes the EDMA.
 *
 * <b> Usage Constraints: </b>
 * This function should be called before using any of the CSL
 * EDMA APIs.
 *
 * @b Example:
 * \code
 
   ...
   if (CSL_edmaInit(NULL) != CSL_SOK) {
       exit;
   }
   \endcode
 *
 * @return Status (CSL_SOK)
 *
 */
CSL_Status
    CSL_edmaInit (
        CSL_EdmaContext *pContext
)
{
    CSL_Status st = CSL_SOK;
	return st;
}

