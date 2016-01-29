/*****************************************************\
 *  Copyright 2004, Texas Instruments Incorporated.  *
 *  All rights reserved.                             *
 *  Restricted rights to use, duplicate or disclose  *
 *  this   code   are  granted   through  contract.  *
 *                                                   *
 * "@(#) PSP/CSL  3.0.0.0  (2003-09-30)              *
\*****************************************************/
/** @file csl_edmaGetHwModuleStatus.c
 *
 *  @brief    File for functional layer of CSL API @a CSL_edmaGetHwStatus()
 *
 *  @date 29th May, 2004
 *  @author Ruchika Kharwar
 */

#include <csl_edma.h>
#include <csl_edmaAux.h>

#pragma CODE_SECTION (CSL_edmaGetHwStatus, ".text:csl_section:edma");

/** Gets the status of the different operations or the current setup of EDMA 
 *	module.
 * 
 * <b> Usage Constraints: </b>
 * Both   CSL_edmaInit() and   CSL_edmaOpen() must be called successfully
 * in that order before   CSL_edmaGetHwStatus() can be called.  For the
 * argument type that can be   void* casted & passed with a particular command
 * refer to   CSL_EdmaHwStatusQuery
 * 
 * @b Example:
   \code
   CSL_EdmaHandle     hModHndl;          
   CSL_Status modStatus;
   
   // Initialize
   modStatus = CSL_edmaInit(NULL);
   ...
  
   hModHndl = CSL_edmaOpen(NULL,CSL_EDMA_0,NULL,&modStatus);
	
	// Hw Status Revision  
	modStatus = CSL_edmaGetHwStatus(hModHndl,CSL_EDMA_QUERY_REVISION,&revision);
                            
	\endcode
 * 
 * @return Status (CSL_SOK/CSL_ESYS_INVPARAMS/CSL_ESYS_INVQUERY/CSL_ESYS_BADHANDLE)
 * 
 */

CSL_Status  CSL_edmaGetHwStatus(
   /** Pointer to the object that holds reference to the
     * instance of EDMA */
	CSL_EdmaHandle                 hMod,
	/** The query to this API which indicates the status/setup
     * to be returned */
	CSL_EdmaHwStatusQuery          myQuery,
	/** Placeholder to return the status; @a void* casted */
	void                           *response
)
{
    CSL_Status st = CSL_SOK;
    if (hMod == NULL)
		return CSL_ESYS_BADHANDLE;
	if (response == NULL)
		return CSL_ESYS_INVPARAMS; 	
	switch(myQuery) {
	    case CSL_EDMA_QUERY_REVISION:	
			*((Uint32*)response) = CSL_edmaGetRevision(hMod);	    	
	    	break;
#if CSL_EDMA_MEMPROTECT    	
		case CSL_EDMA_QUERY_MEMFAULT:	
		    st = CSL_edmaGetMemoryFaultError(hMod,(CSL_EdmaMemFaultStatus*)response);
	    	break;	
	    case CSL_EDMA_QUERY_MEMPROTECT:	
			((CSL_EdmaQueryRegion*)response)->regionVal = \
			CSL_edmaGetMemoryProtectionAttrib(hMod,((CSL_EdmaQueryRegion*)response)->region);
	    	break;	    	    
#endif	    		
		case CSL_EDMA_QUERY_CTRLERROR:	
			st = CSL_edmaGetControllerError(hMod,(CSL_EdmaccStatus*)response);
	    	break;	    	
		case CSL_EDMA_QUERY_INTERRUPT_PENDSTATUS:	
			st = CSL_edmaGetIntrPendStatus(hMod,&((CSL_BitMask32*)response)[0],&((CSL_BitMask32*)response)[1]);
	    	break;	    	
		
		case CSL_EDMA_QUERY_INTERRUPT_MISSED:	
		    st = \
			CSL_edmaGetEventsMissed(hMod,&((CSL_BitMask32*)response)[0],&((CSL_BitMask32*)response)[1],&((CSL_BitMask32*)response)[2]);
	    	break;
		default:
	        st = CSL_ESYS_INVQUERY;
	        break;
    }
    return st;
}
