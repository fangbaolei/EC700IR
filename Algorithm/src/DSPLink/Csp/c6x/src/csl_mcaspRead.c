/** ===========================================================================
 *   @file  csl_mcaspRead.c
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
    
/*  @(#) PSP/CSL 3.00.01.00[5905] (2005-1-11)  */ 
    
/* ============================================================================
 *  Revision History
 *  ===============
 *  11-Jan-2005 asr File Created.
 * ============================================================================
 */ 
    
#include <davinci_hd64plus.h>
#include <csl_mcasp.h>
    
/** ===========================================================================
 *   @n@b CSL_mcaspRead
 *
 *   @b Description
 *   @n This reads the data from McASP. 32 bits of data will be read in the 
 *      data object (variable); the pointer to which is passed as the third
 *      argument.
 *
 *   @b Arguments
 *   @verbatim
            hMcasp      Handle to the McASP instance
            
            data        Buffer to store read data    
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *    @li                   CSL_SOK             - Successful completion of read
 *
 *    @li                   CSL_ESYS_INVPARAMS  - Invalid serializer number
 *
 *    @li                   CSL_ESYS_BADHANDLE  - Invalid handle
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  Input argument "data" will be modified.
 *
 *   @b Modifies
 *   @n   "data" argument.
 *
 *   @b Example
 *   @verbatim
            CSL_Status        status;
            Uint32            inData;
            CSL_McaspHandle   hMcasp;
 
            status = CSL_mcaspRead (hMcasp, &inData);
    @endverbatim
 * ============================================================================
 */ 
#pragma CODE_SECTION (CSL_mcaspRead, ".text:csl_section:mcasp");
CSL_Status CSL_mcaspRead (
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
        *data = *(volatile Uint32 *) MCASP0_RXBUF_ADDR;
    } 
        /* For 2st instance of McASP */ 
    else if (hMcasp->perNo == CSL_MCASP_1) {
        *data = *(volatile Uint32 *) MCASP1_RXBUF_ADDR;
    }
    
        /* For 3st instance of McASP */ 
/*#ifndef C6722
    else if (hMcasp->perNo == CSL_MCASP_2) {
        *data = *(volatile Uint32 *) MCASP2_RXBUF_ADDR;
    }
    
#endif   For C762x Derivatives */
        return (status);
}


