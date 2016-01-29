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
#pragma CODE_SECTION (CSL_edmaOpen, ".text:csl_section:edma");

/** This is a module level open required for the Module level control commands.
 *
 * <b> Usage Constraints: </b>
 * The EDMA must be succesfully initialized
 * via CSL_edmaInit() before calling this
 * function.
 *
 * @b Example:
   \code


   CSL_EdmaHandle     edmaHndl;          
   CSL_Status modStatus;
   // Initialize
   
   modStatus = CSL_edmaInit(NULL);
   ...
   
   edmaHndl = CSL_edmaOpen(NULL,CSL_EDMA_0,NULL,&modStatus);
   
   \endcode
 *
 * @return Handle @a CSL_EdmaHandle to the requested module  instance
 * of the specified EDMA Instance number. 
 */
CSL_EdmaHandle  CSL_edmaOpen(
    /* EDMA Module Object pointer. This is NULL. */    
    CSL_EdmaObj              *edmaObj,
    /** instance of EDMA */
    CSL_InstNum              edmaNum,
    /** EDMA Param pointer i.e NULL in this case */
    CSL_EdmaModuleParam      *param,
    /** CSL Status */
    CSL_Status               *status
)
{
    CSL_EdmaModuleBaseAddress  baseAddress;
    CSL_edmaccGetModuleBaseAddr(edmaNum,NULL,&baseAddress);
    if (status != NULL)
    	*status = CSL_SOK;
    return(baseAddress.regs);
}

