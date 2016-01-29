/*****************************************************\
 *  Copyright 2004, Texas Instruments Incorporated.  *
 *  All rights reserved.                             *
 *  Restricted rights to use, duplicate or disclose  *
 *  this   code   are  granted   through  contract.  *
 *                                                   *
 * "@(#) PSP/CSL  3.0.0.0  (2003-09-30)              *
\*****************************************************/
/** @file csl_edmaGetHwChannelStatus.c
 *
 *  @brief    File for functional layer of CSL API @a CSL_edmaGetHwChannelStatus()
 *
 *  @date 
 *  @author Ruchika Kharwar
 */

#include <csl_edma.h>
#include <csl_edmaAux.h>

#pragma CODE_SECTION (CSL_edmaGetHwChannelStatus, ".text:csl_section:edma");

/** Gets the status of the different operations or the current setup of EDMA 
 *	module.
 * 
 * <b> Usage Constraints: </b>
 * All @a CSL_edmaInit(), @a CSL_edmaOpen(), @a CSL_edmaChannelOpen() must be 
 * invoked successfully in that order before this API can be invoked.
 * If the channel is opened in context of a Region, @a CSL_edmaRegionOpen()
 * may also need to be invoked before this API. If a Shadow region is used then care of the 
 * DRAE settings must be taken.
 *
 * @b Example:
   \code

    // Initialization

	CSL_EdmaHandle hModule;
	
	CSL_EdmaChanObj ChObj;
	CSL_EdmaChanHandle hChannel;
	CSL_EdmaChannelParam chParam;
    CSL_Status         edmaStatus;	
    
    // Initialization
	CSL_edmaInit();
	// Module Level Open	
	 hModule = CSL_edmaOpen(NULL,CSL_EDMA_0,NULL,&edmaStatus);
	
    ...
    // Channel 0 is opened which is allocated to Region Region 0
    chParam.regionNum = CSL_EDMA_REGION_GLOBAL;
    chParam.chaNum = CSL_EDMA_CHA0;
    hChannel = CSL_edmaChannelOpen(&edmaObj,
                            CSL_EDMA_0,
                            &chParam,
                            &edmaStatus);

    // Channel 0 is opened which is in the global region, hence the shadow Handle
    // is NULL    
    edmaStatus = CSL_edmaGetHwChannelStatus(hChannel,
                            CSL_EDMA_QUERY_CHANNEL_SETUP,
                            &querySetup);
	\endcode
 * 
 * @return Status(CSL_SOK/CSL_ESYS_BADHANDLE/CSL_ESYS_INVPARAMS/CSL_ESYS_INVQUERY)
 * 
 */

CSL_Status  CSL_edmaGetHwChannelStatus(
   /** Pointer to the object that holds reference to the
     * instance of EDMA Channel*/
	CSL_EdmaChanHandle                 hEdma,
	/** The query to this API which indicates the status/setup
     * to be returned */
	CSL_EdmaHwChannelStatusQuery       myQuery,
	/** Placeholder to return the status; @a void* casted */
	void                               *response
)
{
    CSL_Status st = CSL_SOK;
    int _qNumIndex,_qchMap;
    Uint32 temp;
    if (hEdma == NULL)
		return CSL_ESYS_BADHANDLE;
	if (response == NULL)
		return CSL_ESYS_INVPARAMS; 
	switch(myQuery) {
	    case CSL_EDMA_QUERY_CHANNEL_SETUP:
	    	if (hEdma->chaNum  < CSL_EDMA_NUM_DMACH) 
	    	{
	    		_qNumIndex = hEdma->chaNum >> 3;
				_qchMap = hEdma->chaNum - (_qNumIndex * 8);
				
		   		((CSL_EdmaHwChannelSetup*)response)->que = (CSL_EdmaEventQueue)(CSL_FEXTR(hEdma->ccregs->DMAQNUM[_qNumIndex],_qchMap*4+2,_qchMap*4));
#if CSL_EDMA_CHMAPEXIST		   		
		   		((CSL_EdmaHwChannelSetup*)response)->paramEntry = CSL_FEXT(hEdma->ccregs->DCHMAP[hEdma->chaNum],EDMACC_DCHMAP_PAENTRY);
#else
				((CSL_EdmaHwChannelSetup*)response)->paramEntry = hEdma->chaNum;		   		
#endif		   		
		   		((CSL_EdmaHwChannelSetup*)response)->triggerWord = CSL_EDMA_TRIGWORD_NONE;
	    	} else 
	    	{
	    		_qNumIndex = hEdma->chaNum - CSL_EDMA_NUM_DMACH;
	    		((CSL_EdmaHwChannelSetup*)response)->que = (CSL_EdmaEventQueue)(CSL_FEXTR(hEdma->ccregs->QDMAQNUM,_qNumIndex*4 + 2, _qNumIndex*4));
				temp = hEdma->ccregs->QCHMAP[_qNumIndex];
	    		((CSL_EdmaHwChannelSetup*)response)->triggerWord = CSL_FEXT(temp,EDMACC_QCHMAP_TRWORD);
	    		((CSL_EdmaHwChannelSetup*)response)->paramEntry = CSL_FEXT(temp,EDMACC_QCHMAP_PAENTRY);
	    	}
	    	break;
		default:
	        st = CSL_ESYS_INVQUERY;
	        break;
    }
    return st;
}
