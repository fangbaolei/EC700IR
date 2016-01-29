/** ===========================================================================
 *   @file  csl_mcaspGetChipCtxt.c
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
    
/*  @(#) PSP/CSL 3.00.01.00[5905] (2005-1-10)  */ 
    
/* ============================================================================
 *  Revision History
 *  ===============
 *  10-Jan-2005 sr File Created.
 * ============================================================================
 */ 
    
#include <davinci_hd64plus.h>
#include <csl_mcasp.h>
    
/** ===========================================================================
 *   @n@b CSL_mcaspGetChipCtxt
 *
 *   @b Description
 *   @n  This function is used for getting the base address of the peripheral
 *       instance and to configure the number of serializers for a particular
 *       instance of McASP on a chip. This function will be called inside the
 *       CSL_mcaspOpen() function.This function is open for re-implementing 
 *       if the user wants to modify the base address of the peripheral object
 *       to point to a different location and there by allow CSL initiated
 *       write/reads into peripheral MMR's go to an alternate location.
 *
 *   @b Arguments
 *   @verbatim
            mcaspNum      Specifies the instance of the McASP to be opened.
  
            pMcaspParam   Module specific parameters.
  
            pChipContext  Pointer to hold the chip context details
  
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK           Successful completion.
 *   @li                    CSL_ESYS_FAIL     The instance number is invalid.
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  Base address structure is populated
 *
 *   @b Modifies
 *   @n Base address structure is modified.
 *
 *   @b Example
 *   @verbatim
        CSL_Status            status;
        CSL_McaspChipContext  chipContext;

       ...
      status = CSL_mcaspGetChipCtxt (CSL_MCASP, NULL, &chipContext);

    @endverbatim
 * ============================================================================
 */ 
    
#pragma CODE_SECTION (CSL_mcaspGetChipCtxt, ".text:csl_section:mcasp");

CSL_Status CSL_mcaspGetChipCtxt (
    CSL_InstNum            mcaspNum,
    CSL_McaspParam         *pMcaspParam, 
    CSL_McaspChipContext   *pChipContext 
) 
{
    CSL_Status st = CSL_SOK;
    
    switch (mcaspNum) {
        case CSL_MCASP_0:
            pChipContext->regs = (CSL_McaspRegsOvly) CSL_MCASP0_REGS;
            pChipContext->numOfSerializers = 16;
            pChipContext->ditStatus = FALSE;
            break;
        case CSL_MCASP_1:
            pChipContext->regs = (CSL_McaspRegsOvly) CSL_MCASP1_REGS;
            pChipContext->numOfSerializers = 6;
            pChipContext->ditStatus = FALSE;
            break;
            
 /*   #ifndef C6722
        case CSL_MCASP_2:
            pChipContext->regs = (CSL_McaspRegsOvly) CSL_MCASP_2_REGS;
            pChipContext->numOfSerializers = 2;
            pChipContext->ditStatus = TRUE;
            break;
            
    #endif   For C672x Derivatives */
        default:
            pChipContext->regs = (CSL_McaspRegsOvly) NULL;
            pChipContext->numOfSerializers = -1;
            pChipContext->ditStatus = FALSE;
            pMcaspParam = pMcaspParam;
            st = CSL_ESYS_FAIL;
            break;
     }
    return st;
}


