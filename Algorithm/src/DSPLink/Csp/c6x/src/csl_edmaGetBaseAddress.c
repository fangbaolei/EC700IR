/** @file csl_edmaGetBaseAddress.c
 *
 * @brief   Implementation of @a CSL_edmaGetBaseAddress()
 *
 * Description
 *  - The @a CSL_edmaGetBaseAddress() function returns the base-address of the
 *    specified Memory Protection instance.
 *
 * Note: This function is open for re-implementing if the user wants to modify
 *  the base address of the peripheral object to point to a different location
 *  and there by allow CSL initiated write/reads into peripheral MMR's go to an 
 *  alternate location. Please refer documentation for more details. 
 *      
 * Revision History
 *   2004/06/01 (Ruchika Kharwar)   Revision 1 
 */


#include <davinci_hd64plus.h>
#include <csl_edma.h>

#pragma CODE_SECTION (CSL_edmaccGetModuleBaseAddr, ".text:csl_section:edma");
 
/** This is an API which obtains the Module base address.
 *
 * <b> Usage Constraints: </b>
 *    None
 * 
 * @b Example:
   \code


   CSL_EdmaModuleBaseAddress  baseAddress;
   ...
   CSL_edmaccGetModuleBaseAddr(CSL_EDMA_0,NULL,&baseAddress);
   ...

   \endcode
 *
 * @return Status (CSL_SOK)
 *
 */
    	
CSL_Status
    CSL_edmaccGetModuleBaseAddr (
	/**  Peripheral instance number 
	 */ 	    
        CSL_InstNum 	              edmaNum,
	/** Module specific parameters.
	 */ 
        CSL_EdmaModuleParam *         pParam,
	/** Base address details.
	 */ 
        CSL_EdmaModuleBaseAddress *   pBaseAddress 
)
{
    pBaseAddress->regs = (CSL_EdmaccRegsOvly)CSL_EDMACC_0_REGS;
    return CSL_SOK;
}

