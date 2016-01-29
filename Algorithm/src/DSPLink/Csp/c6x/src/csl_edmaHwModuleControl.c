/*****************************************************\
 *  Copyright 2004, Texas Instruments Incorporated.  *
 *  All rights reserved.                             *
 *  Restricted rights to use, duplicate or disclose  *
 *  this   code   are  granted   through  contract.  *
 *                                                   *
 * "@(#) PSP/CSL  3.0.0.0  (2003-09-30)              *
\*****************************************************/
/** @file csl_edmaHwModuleControl.c
 *
 *    @brief    File for functional layer of CSL API 
 *    @a CSL_edmaHwControl()
 *
 *  Description
 *    - The @a CSL_edmaHwControl() function definition & 
 *      it's associated
 *      functions
 *  @date 29th May, 2004
 *  @author Ruchika Kharwar
 */

#include <csl_edma.h>
#include <csl_edmaAux.h> 

#pragma CODE_SECTION (CSL_edmaHwControl, ".text:csl_section:edma");

/** Takes a command with an optional argument & implements it. This function is
 * used to carry out the different operations performed by EDMA.
 * 
 * <b> Usage Constraints: </b>
 * Both   CSL_edmaInit() and   CSL_edmaOpen() must be called successfully
 * in that order before   CSL_edmaHwControl() can be called. For the
 * argument type that can be   void* casted & passed with a particular command
 * refer to   CSL_EdmaHwControlCmd
 *
 * @b Example:
    \code
 
   CSL_EdmaHandle     hModHndl;          
   CSL_Status modStatus;
   
   // Initialize
   modStatus = CSL_edmaInit(NULL);
   ...
   
   hModHndl = CSL_edmaOpen(NULL,CSL_EDMA_0,NULL,&modStatus);
	
    // Interrupt 5,6,7 enable  
    intrEn[0] = 0xE0;
    intrEn[1] = 0x00;
    status = CSL_edmaHwControl(hModHndl,CSL_EDMA_CMD_INTERRUPT_ENABLE, intrEn);
   
   \endcode
 * 
 * @return Status (CSL_SOK/CSL_ESYS_INVPARAMS/CSL_ESYS_BADHANDLE/CSL_ESYS_INVCMD)
 * 
 */

CSL_Status  CSL_edmaHwControl
(
	/** Pointer to the object that holds reference to the
     * instance of EDMA  */
	CSL_EdmaHandle           hMod,
	/** The command to this API which indicates the action to be taken */
	CSL_EdmaHwControlCmd     cmd,
	/** Optional argument @a void* casted */
	void                     *cmdArg
)
{ 
    CSL_Status st;
    if (hMod == NULL) 
		return CSL_ESYS_BADHANDLE;

	switch(cmd) {
#if CSL_EDMA_MEMPROTECT	
		case CSL_EDMA_CMD_MEMPROTECT_SET:
			st = CSL_edmaSetMemoryProtectionAttrib(hMod,((CSL_EdmaCmdRegion*)cmdArg)->region,((CSL_EdmaCmdRegion*)cmdArg)->regionVal);
			break;
#endif						
		case CSL_EDMA_CMD_DMAREGION_ENABLE:
			st = CSL_edmaDmaRegionAccessEnable(hMod,((CSL_EdmaCmdDmaRegion*)cmdArg)->region,((CSL_EdmaCmdDmaRegion*)cmdArg)->drae,((CSL_EdmaCmdDmaRegion*)cmdArg)->draeh);
			break;
			
		case CSL_EDMA_CMD_DMAREGION_DISABLE:
			st = CSL_edmaDmaRegionAccessDisable(hMod,((CSL_EdmaCmdDmaRegion*)cmdArg)->region,((CSL_EdmaCmdDmaRegion*)cmdArg)->drae,((CSL_EdmaCmdDmaRegion*)cmdArg)->draeh);
			break;
			
		case CSL_EDMA_CMD_QDMAREGION_ENABLE:
			st = CSL_edmaQdmaRegionAccessEnable(hMod,((CSL_EdmaCmdQdmaRegion*)cmdArg)->region,((CSL_EdmaCmdQdmaRegion*)cmdArg)->regionVal);
			break;
			
		case CSL_EDMA_CMD_QDMAREGION_DISABLE:
			st = CSL_edmaQdmaRegionAccessDisable(hMod,((CSL_EdmaCmdQdmaRegion*)cmdArg)->region,((CSL_EdmaCmdQdmaRegion*)cmdArg)->regionVal);
			break;	
			
		case CSL_EDMA_CMD_QUEPRIORITY_SET:
			st = CSL_edmaEventQueuePrioritySet(hMod,((CSL_EdmaCmdQuePriority*)cmdArg)->que,((CSL_EdmaCmdQuePriority*)cmdArg)->pri);
			break;
			
		case CSL_EDMA_CMD_QUETHRESHOLD_SET:
		    st = CSL_edmaEventQueueThresholdSet(hMod,((CSL_EdmaCmdQueThr*)cmdArg)->que,((CSL_EdmaCmdQueThr*)cmdArg)->threshold);
			break;			
			
		case CSL_EDMA_CMD_QUETC_MAP:
			st = CSL_edmaEventQueueTcMap(hMod,((CSL_EdmaCmdQueTc*)cmdArg)->que,((CSL_EdmaCmdQueTc*)cmdArg)->tc);
			break;			
			
		case CSL_EDMA_CMD_ERROR_EVAL:
			st = CSL_edmaErrorEval(hMod);
			break;			
			
		case CSL_EDMA_CMD_INTERRUPT_CLEAR:
			st = CSL_edmaInterruptClear(hMod,((CSL_BitMask32*)(cmdArg))[0],((CSL_BitMask32*)(cmdArg))[1]);
			break;			
			
		case CSL_EDMA_CMD_INTERRUPT_ENABLE:
			st = CSL_edmaInterruptEnable(hMod,((CSL_BitMask32*)(cmdArg))[0],((CSL_BitMask32*)(cmdArg))[1]);
			break;			
			
		case CSL_EDMA_CMD_INTERRUPT_DISABLE:
			st = CSL_edmaInterruptDisable(hMod,((CSL_BitMask32*)(cmdArg))[0],((CSL_BitMask32*)(cmdArg))[1]);
			break;
			
		case CSL_EDMA_CMD_INTERRUPT_EVAL:
			st = CSL_edmaInterruptEval(hMod);			
			break;			
			
	    default:
	         st = CSL_ESYS_INVCMD;
	         break;
        }
    return st;
}

