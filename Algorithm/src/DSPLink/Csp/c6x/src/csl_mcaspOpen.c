/** ===========================================================================
 *   @file  csl_mcaspOpen.c
 *
 *   @path  $(CSLPATH)\mcasp\src
 *
 *   @desc  Multi channel audio serial port implementation.
 */  
    
/*  ===========================================================================
 *  Copyright (c) Texas Instruments Inc 2002, 2003, 2004, 2005
 *
 *  Use of this software is controlled by the terms and conditions found in the
 *  license agreement under which this software has been supplied.
 *  ===========================================================================
 */ 
    
/*  @(#) PSP/CSL 3.00.01.00[5905] (2005-01-10)  */ 
    
/* ============================================================================
 *  Revision History
 *  ===============
 *  10-Jan-2005 asr File Created.
 * ============================================================================
 */ 
    
#include <csl_mcasp.h>
    
/** ===========================================================================
 *   @n@b CSL_mcaspOpen
 *
 *   @b Description
 *   @n This Function populates the peripheral data object for the instance and
 *      returns handle to it.
 *
 *   @b Arguments
 *   @verbatim
            pMcaspObj     Pointer to the data object for McASP instance

            mcaspNum      Specifies the instance of the McASP to be opened.
                          There are 3 instances of the McASP.

            pMcaspParam   Module specific parameter for McASP instance

            pStatus        Status of the function call
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_McaspHandle
 *   @n                     Valid McASP instance handle will be returned if
 *                          status value is equal to CSL_SOK.
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n   1.    The status is returned in the status variable. If status
 *              returned is
 *   @li            CSL_SOK           Open call is successful
 *   @li            CSL_ESYS_FAIL     The McASP instance is invalid
 *
 *        2.    Multi channel Audio serial Port object structure is populated
 *
 *   @b Modifies
 *   @n    1. The status variable
 *
 *         2. The data object for the instance
 *
 *   @b Example
 *   @verbatim
           CSL_status          status;
           CSL_McaspObj        mcaspObj;
           CSL_McaspHandle     hMcasp;

          hMcasp = CSL_McaspOpen (&mcaspObj,
                           CSL_MCASP_1, NULL
                           &status);

    @endverbatim
 * ============================================================================
 */ 
    
#pragma CODE_SECTION (CSL_mcaspOpen, ".text:csl_section:mcasp");
CSL_McaspHandle CSL_mcaspOpen (
        CSL_McaspObj   *pMcaspObj,
        CSL_InstNum    mcaspNum, 
        CSL_McaspParam *pMcaspParam,
        CSL_Status     *pStatus
) 
{
    CSL_Status st;
    CSL_McaspHandle hMcasp;
    CSL_McaspChipContext chipContext;
    
    st = CSL_mcaspGetChipCtxt (mcaspNum, pMcaspParam, &chipContext);
    
    if (st == CSL_SOK) {
        pMcaspObj->regs = chipContext.regs;
        pMcaspObj->perNo = (CSL_InstNum) mcaspNum;
        pMcaspObj->numOfSerializers = chipContext.numOfSerializers;
        pMcaspObj->ditStatus = chipContext.ditStatus;
        hMcasp = (CSL_McaspHandle) pMcaspObj;
    }
     else {
        pMcaspObj->regs = (CSL_McaspRegsOvly) NULL;
        pMcaspObj->perNo = (CSL_InstNum) - 1;
        pMcaspObj->numOfSerializers = -1;
        pMcaspObj->ditStatus = FALSE;
        hMcasp = (CSL_McaspHandle) NULL;
    }
    
    *pStatus = st;
    return hMcasp;
}


