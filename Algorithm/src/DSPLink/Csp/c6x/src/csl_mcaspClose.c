/** ===========================================================================
 *   @file  csl_mcaspClose.c
 *
 *   @path  $(CSLPATH)\mcasp\src
 *
 *   @desc  Multi channel audio serial port CSL implementation.
 *
 */  
    
/*  ===========================================================================
 *   Copyright (c) Texas Instruments Inc 2002, 2003, 2004, 2005
 *
 *   Use of this software is controlled by the terms and conditions found in
 *   the license agreement under which this software has been supplied.
 *   ==========================================================================
 */ 
    
/*  @(#) PSP/CSL 3.00.01.00[5905] (2005-01-11)  */ 
    
/* ============================================================================
 *  Revision History
 *  ===============
 *  11-Jan-2005 asr File Created.
 * ============================================================================
 */ 
    
#include <csl_mcasp.h>
    
/** ===========================================================================
 *   @n@b CSL_mcaspClose
 *
 *   @b Description
 *   @n This function closes the McASP instance
 *
 *   @b Arguments
 *   @verbatim
            hMcasp         Handle to the McASP instance
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK            - McASP is close successful
 *   @li                    CSL_ESYS_BADHANDLE - The handle passed is Invalid
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  The McASP instance is closed.
 *
 *   @b  Modifies
 *   @n  Peripheral data object
 *
 *   @b Example
 *   @verbatim
            CSL_McaspHandle hMcasp;
            CSL_Status      status;
            
            status = CSL_mcaspClose (hMcasp);
    @endverbatim
 * ============================================================================
 */ 
#pragma CODE_SECTION (CSL_mcaspClose, ".text:csl_section:mcasp");

CSL_Status CSL_mcaspClose (
    CSL_McaspHandle hMcasp
) 
{
    CSL_Status st;
    
    if(hMcasp != NULL) {
        hMcasp->regs = (CSL_McaspRegsOvly) NULL;
        hMcasp->perNo = (CSL_InstNum) - 1;
        hMcasp->numOfSerializers = -1;
        hMcasp->ditStatus = FALSE;
        st = CSL_SOK;
    }
    else {
        st = CSL_ESYS_BADHANDLE;
    }
    return st;
}


