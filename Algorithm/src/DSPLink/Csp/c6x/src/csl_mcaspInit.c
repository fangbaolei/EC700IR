/** ===========================================================================
 *   @file  csl_mcaspInit.c
 *
 *   @path  $(CSLPATH)\mcasp\src
 *
 *   @desc  Multi channel audio serial port implementation.
 *
 */  
    
/*  ===========================================================================
 *  Copyright (c) Texas Instruments Inc 2002, 2003, 2004, 2005
 *
 *  Use of this software is controlled by the terms and conditions found in the
 *  license agreement under which this software has been supplied.
 *  ===========================================================================
 */ 
    
/*  @(#) PSP/CSL 3.00.01.00[5905] (2005-01-12)  */ 
    
/* ============================================================================
 *  Revision History
 *  ===============
 *  12-Jan-2005 asr File Created.
 * ============================================================================
 */ 
    
#include <csl_mcasp.h>
    
/** ===========================================================================
 *   @n@b CSL_mcaspInit
 *
 *   @b Description
 *   @n This is the initialization function for the multi channel audio serial
 *      port CSL. This function needs to be called before any other multi
 *      channel audio serial port CSL functions are to be called.  
 *      This function does not modify any registers or check status. It returns 
 *      status CSL_SOK. It has been kept for the future use.
 *
 *
 *   @b Arguments
     @verbatim
            pContext   Context information for the instance. Should be NULL
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK - Always returns
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim
            CSL_Status  status;
            
            status = CSL_mcaspInit (NULL);
     @endverbatim
 * ============================================================================
 */ 
#pragma CODE_SECTION (CSL_mcaspInit, ".text:csl_section:mcasp");
CSL_Status CSL_mcaspInit (
    CSL_McaspContext   *pContext
) 
{
    /* to remove warning */ 
    pContext = pContext;
        
    return CSL_SOK;
}


