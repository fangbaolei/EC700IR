/** ===========================================================================
 *   @file  csl_mcaspWrite.c
 *
 *   @path  $(CSLPATH)\mcasp\src
 *
 *   @desc  Multi channel audio serial port CSL implementation.
 *
 */  
    
/*  ===========================================================================
 *  Copyright (c) Texas Instruments Inc 2002, 2003, 2004, 2005
 *
 *  Use of this software is controlled by the terms and conditions found in the
 *  license agreement under which this software has been supplied.
 *  ===========================================================================
 */ 
    
/*  @(#) PSP/CSL 3.00.01.00[5905] (2005-10-12)  */ 
    
/* ============================================================================
 *  Revision History
 *  ===============
 *  12-Jan-2005 asr File Created.
 * ============================================================================
 */ 
    
#include <davinci_hd64plus.h>
#include <csl_mcasp.h>
    
/** ===========================================================================
 *   @n@b CSL_mcaspWrite
 *
 *   @b Description
 *   @n This transmits the data from McASP. 32 bits of data will be transmitted
 *      in the data object (variable); the pointer to which is passed as the 
 *      third argument.
 *
 *   @b Arguments
 *   @verbatim
            hMcasp      Handle to the McASP instance
            
            data        data to be written    
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *    @li                   CSL_SOK             - Successful completion of read
 *
 *    @li                   CSL_ESYS_INVPARAMS  - Invalid parameters
 *
 *    @li                   CSL_ESYS_BADHANDLE  - Invalid handle
 *
 *   <b> Pre Condition </b>
 *   @n  "data" should have the data to be written
 *
 *   <b> Post Condition </b>
 *   @n  XBUF Register will be written.
 *
 *   @b Modifies
 *   @n XBUF Register.
 *
 *   @b Example
 *   @verbatim
            CSL_Status        status;
            Uint32            outData;
            CSL_McaspHandle   hMcasp;
 
            status = CSL_mcaspWrite (hMcasp, &outData);
    @endverbatim
 * ============================================================================
 */ 
#pragma CODE_SECTION (CSL_mcaspWrite, ".text:csl_section:mcasp");
CSL_Status CSL_mcaspWrite (
    CSL_McaspHandle hMcasp, 
    Uint32         *data
) 
{
    CSL_Status status = CSL_SOK;
    
	if (hMcasp == NULL) {
		return CSL_ESYS_BADHANDLE;
	}
    
	/* For 1st instance of McASP */ 
	if (hMcasp->perNo == CSL_MCASP_0) {
        *(volatile Uint32 *) MCASP0_TXBUF_ADDR = *data;
	}
    
	/* For 2nd instance of McASP */ 
	else if (hMcasp->perNo == CSL_MCASP_1) {
		*(volatile Uint32 *) MCASP1_TXBUF_ADDR = *data;
	}
    
	/* For 3rd instance of McASP */ /*
#ifndef C6722
	else if (hMcasp->perNo == CSL_MCASP_2) {
		*(volatile Uint32 *) MCASP2_TXBUF_ADDR = *data;
    }
    
#endif  for C672x Derivatives */
	return (status);
}


