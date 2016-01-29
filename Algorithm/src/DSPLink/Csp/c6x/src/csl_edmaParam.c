/*****************************************************\
 *  Copyright 2004, Texas Instruments Incorporated.  *
 *  All rights reserved.                             *
 *  Restricted rights to use, duplicate or disclose  *
 *  this   code   are  granted   through  contract.  *
 *                                                   *
 * "@(#) PSP/CSL  3.0.0.0  (2003-09-30)              *
\*****************************************************/
/* =========================================================
 *   @file  csl_edmaParamSetup.c
 *
 *   @path  $(CSLPATH)\src\edma
 *
 *   @desc  EDMA module
 *
 */
#include <csl_edma.h>

#pragma CODE_SECTION (CSL_edmaGetParamHandle, ".text:csl_section:edma");

/** Acquires the Param entry as specified by the argument.
 *
 * <b> Usage Constraints: </b>
 * CSL_edmaInit(), CSL_edmaOpen(),  CSL_edmaChannelOpen() must be called
 * successfully in that order before this function can be called. 
 * If the channel for which the paramentry is obtained is in the context of a region then
 * CSL_edmaRegionOpen() must also be invoked.
 * This API returns a handle to the requested parameter entry.
 *
 * @b Example:
 * \code
 	CSL_EdmaObj ModuleObj;
	CSL_EdmaHandle hModHndl;
	
	CSL_EdmaChanObj ChObj;
	CSL_EdmaChanHandle chHndl;
	CSL_EdmaChannelParam chParam;
    CSL_Status         edmaStatus;	
    CSL_EdmaParamHandle param;
    // Initialization
	CSL_edmaInit();
	// Module Level Open	
	hModHndl = CSL_edmaOpen(NULL,CSL_EDMA_0,NULL,&modStatus);
	
    ...
    // Channel 0 is opened which is allocated to Region Region 0
    chParam.regionNum = CSL_EDMA_REGION_GLOBAL;
    chParam.chaNum = CSL_EDMA_CHA0;
    chHndl = CSL_edmaChannelOpen(&ChObj,
                            CSL_EDMA_0,
                            &chParam,
                            &edmaStatus);
	// Acquire Param Entries in the Shadow Region itself, hence region is NULL
	param = CSL_edmaGetParamHandle(chHndl,1,&status);
	
	// Setup the acquired parameter entry
    ...  
   \endcode
 *
 * @return Parameter Entry Handle.
 *
 */
CSL_EdmaParamHandle  CSL_edmaGetParamHandle(
    /**  a handle to the channel instance of the specified EDMA
     *   obtained  through the @a CSL_edmaOpen() call */
    CSL_EdmaChanHandle            hEdma,
    /**  entry number to be acquired */
    Uint16                        entryNum,
    /**  status of operation */
    CSL_Status    				  *status	    
)
{	
	if (status!=NULL)
		*status = CSL_SOK;
	return (&hEdma->ccregs->PARAMENTRY[entryNum]);	

}


#pragma CODE_SECTION (CSL_edmaParamSetup, ".text:csl_section:edma");


/** Configures the EDMA parameter Entry using the values passed in through the
 * Param setup structure. 
 *
 * <b> Usage Constraints: </b>
 * CSL_edmaInit(),@CSL_edmaOpen,@a CSL_edmaChannelOpen(),@a CSL_edmaGetParamHandle() must
 * be called successfully in that order before this function can be called.
 * If the channel for which the paramentry is obtained is in the context of a region then
 * CSL_edmaRegionOpen() must also be invoked.
 * The user must allocate space for & fill in the parameter setup structure
 *
 * @b Example:
 * \code
	CSL_EdmaObj ModuleObj;
	CSL_EdmaHandle hModHndl;
	
	CSL_EdmaChanObj ChObj1,ChObj2;
	CSL_EdmaChanHandle chHndl1, chHndl2;
	CSL_EdmaHwChannelSetup chSetup;
	CSL_EdmaChannelParam chParam;
    CSL_Status         edmaStatus;	
    CSL_EdmaParamHandle param1, param2;
    CSL_EdmaParamSetup myParamSetup;
    // Initialization
	CSL_edmaInit();
	// Module Level Open	
	 hModHndl = CSL_edmaOpen(NULL,CSL_EDMA_0,NULL,&modStatus);
	
    ...
    // Channel 1 is opened which is allocated to Region Region 0
    chParam.regionNum = CSL_EDMA_REGION_GLOBAL;
    chParam.chaNum = CSL_EDMA_CHA1;
    chHndl1 = CSL_edmaChannelOpen(&ChObj1,
                            CSL_EDMA_0,
                            &chParam,
                            &edmaStatus);
    // Channel Setup 
    chSetup.paramEntry = 1;
	chSetup.que  = CSL_EDMA_EVT_QUE0;
	CSL_edmaHwChannelSetup(chHndl1,&chSetup);
	                        
	// Acquire Param Entries in the Shadow Region itself, hence region is NULL
	param1 = CSL_edmaGetParamHandle(chHndl1,1,&status);
	
	// Setup the acquired parameter entry                          
    // Setup the parameter Entry parameters (Ping buffer)
    myParamSetup.option = CSL_EDMA_OPT_MAKE(FALSE,FALSE,FALSE,TRUE,tcc,CSL_EDMA_TCC_EARLY,CSL_EDMA_FIFOWIDTH_128BIT,TRUE,CSL_EDMA_SYNC_ARRAY,CSL_EDMA_ADDRMODE_INCR,CSL_EDMA_ADDRMODE_INCR);           
	myParamSetup.srcAddr = (Uint32)srcBuff1;         
	myParamSetup.elmArrCnt = CSL_EDMA_CNT_MAKE(256,1);       
	myParamSetup.dstAddr = (Uint32)dstBuff1;        
	myParamSetup.srcDstBidx = CSL_EDMA_BIDX_MAKE(1,1);     
	myParamSetup.BcntRld = CSL_EDMA_LINKBCNTRLD_MAKE(CSL_EDMA_LINK_NULL,0);
	myParamSetup.srcDstCidx = CSL_EDMA_CIDX_MAKE(0,1);     
	myParamSetup.cCnt = 1;
	myParamSetup.triggerWord = CSL_EDMA_TRIGWORD_NONE;
   	
	// Setup the acquired parameter Entry 
	CSL_edmaParamSetup(param1,&myParamSetup,CSL_EDMA_PARAM_BASIC);	
	
	myParamSetup.srcAddr = (Uint32)srcBuff2;         
	myParamSetup.elmArrCnt = CSL_EDMA_CNT_MAKE(256,1);       
	myParamSetup.dstAddr = (Uint32)dstBuff2;
	
	// Setup the acquired parameter Entry 
	CSL_edmaParamSetup(param2,&myParamSetup,CSL_EDMA_PARAM_BASIC);	
	
	// If Linking Desired for param1 to param2
	CSL_edmaParamSetup(param1,(void*)param2,CSL_EDMA_PARAM_LINK);
	
	chParam.regionNum = CSL_EDMA_REGION_GLOBAL;
    chParam.chaNum = CSL_EDMA_CHA2;
    chHndl2 = CSL_edmaChannelOpen(&ChObj2,
                            CSL_EDMA_0,
                            &chParam,
                            &edmaStatus);
    // Channel Setup 
    chSetup.paramEntry = 2;
	chSetup.que  = CSL_EDMA_EVT_QUE0;
	CSL_edmaHwChannelSetup(chHndl2,&chSetup);
	
	// If Chaining Desired from Channel 1 to Channel 2
	CSL_edmaParamSetup(param1,(void*)(chHndl2),CSL_EDMA_PARAM_CHAIN);
	}	     
   \endcode
 *
 * @return Status (CSL_ESYS_BADHANDLE/CSL_ESYS_INVPARAMS/CSL_SOK)
 *
 */
          
CSL_Status  CSL_edmaParamSetup(
    /**  a handle to the param entry acquired previously by the channel */
    CSL_EdmaParamHandle        hParamHndl,
    /** Pointer to setup structure which contains the
     *  information to program EDMA Channel to the startup state 
     *  OR the parameter entry to which linking needs to be done 
     *  OR the Channel to which chaining needs to be performed  */
    void       				   *setup,
    /** Specifies the stage of Setup i.e Could be initial, link Setup or Chain Setup. These 
        do not fall into the category of the Control Commands since 
        this does not fall into the category of the run time change of parameter 
        setups. The user is expected to follow tis flow of parameter entry setup hence
        3 diferent stages are required */
    CSL_EdmaParamSetupStage    stage      
)
{
	Uint32* paramHndl = (Uint32*)hParamHndl;
	int i;
	if (hParamHndl == NULL)
        return CSL_ESYS_BADHANDLE;
	if (setup == NULL)
	    return CSL_ESYS_INVPARAMS;
	if (stage == CSL_EDMA_PARAM_BASIC) {	
		
		for ( i = ((CSL_EdmaParamSetup*)setup)->triggerWord-1; i >=0; i--)
			paramHndl[i] = ((Uint32*)setup)[i];
		for ( i = 7; i > ((CSL_EdmaParamSetup*)setup)->triggerWord; i--)
			paramHndl[i] = ((Uint32*)setup)[i];
		paramHndl[((CSL_EdmaParamSetup*)setup)->triggerWord] = ((Uint32*)setup)[((CSL_EdmaParamSetup*)setup)->triggerWord];	
	    
	} else if (stage == CSL_EDMA_PARAM_LINK) {
		CSL_FINS(hParamHndl->LINK_BCNTRLD,EDMACC_LINK_BCNTRLD_LINK,(Uint32)setup);
	} else {
		CSL_FINS(hParamHndl->OPT,EDMACC_OPT_TCC,((CSL_EdmaChanHandle)(setup))->chaNum);		
 	}	
	return CSL_SOK;
}




/** This is for the ease of QDMA channels. Once the QDMA channel transfer is triggered,
 * subsequent triggers may be done with only writing the modified words in
 * the parameter entry along with the trigger word. This API is expected to achieve this 
 * this purpose. Most usage scenarios, the user should not be writing more than the 
 * trigger word entry.
 *  
 *
 * <b> Usage Constraints: </b>
 * CSL_edmaInit(),@CSL_edmaOpen,@a CSL_edmaChannelOpen(),@a CSL_edmaGetParamHandle(),
 * CSL_edmaParamSetup(), must be called successfully in that order before 
 * this function can be called. The main setup structure consists of pointers to
 * sub-structures. The user has to allocate space for & fill in the parameter 
 * setup structure.
 * If the channel for which the paramentry is obtained is in the context of a region then
 * CSL_edmaRegionOpen() must also be invoked before this API.
 
 *
* @b Example:
 * \code
	CSL_EdmaObj ModuleObj;
	CSL_EdmaHandle hModHndl;
	
	CSL_EdmaChanObj ChObj;
	CSL_EdmaChanHandle chHndl;
	CSL_EdmaChannelParam chParam;
    CSL_Status         edmaStatus;	
    CSL_EdmaParamHandle param;
    CSL_EdmaParamSetup myParamSetup;
    // Initialization
	CSL_edmaInit();
	// Module Level Open	
	 hModHndl = CSL_edmaOpen(NULL,CSL_EDMA_0,NULL,&modStatus);
	
    ...
    // Channel 0 is opened which is allocated to Region Region 0
    chParam.regionNum = CSL_EDMA_REGION_GLOBAL;
    chParam.chaNum = CSL_EDMA_QCHA0;
    chHndl = CSL_edmaChannelOpen(&ChObj,
                            CSL_EDMA_0,
                            &chParam,
                            &edmaStatus);
	// Acquire Param Entries in the Shadow Region itself, hence region is NULL
	param = CSL_edmaGetParamHandle(chHndl,1,&status);
	
	// Setup the acquired parameter entry                          
    // Setup the parameter Entry parameters (Ping buffer)
    myParamSetup.option = CSL_EDMA_OPT_MAKE(FALSE,FALSE,FALSE,TRUE,tcc,CSL_EDMA_TCC_EARLY,CSL_EDMA_FIFOWIDTH_128BIT,TRUE,CSL_EDMA_SYNC_ARRAY,CSL_EDMA_ADDRMODE_INCR,CSL_EDMA_ADDRMODE_INCR);           
	myParamSetup.srcAddr = (Uint32)srcBuff1;         
	myParamSetup.elmArrCnt = CSL_EDMA_CNT_MAKE(256,1);       
	myParamSetup.dstAddr = (Uint32)dstBuff1;        
	myParamSetup.srcDstBidx = CSL_EDMA_BIDX_MAKE(1,1);     
	myParamSetup.BcntRld = CSL_EDMA_LINKBCNTRLD_MAKE(CSL_EDMA_LINK_NULL,0);
	myParamSetup.srcDstCidx = CSL_EDMA_CIDX_MAKE(0,1);     
	myParamSetup.cCnt = 1;
	myParamSetup.triggerWord = CSL_EDMA_TRIGWORD_DEFAULT;
   
	
	// Setup the acquired parameter Entry 
	CSL_edmaParamSetup(param,&myParamSetup,CSL_EDMA_PARAM_BASIC);	
	
	// Enable Channel 
	...
	// Write the trigger Word to trigger transfer
	CSL_edmaParamWriteWord(myParamSetup,7,myParamSetup.cCnt);
	}	   
   \endcode
 *
  * @return Status (CSL_SOK)
 *
 */
#pragma CODE_SECTION (CSL_edmaParamWriteWord, ".text:csl_section:edma");
CSL_Status  CSL_edmaParamWriteWord(
	/** a handle to the param entry acquired previously by the QDMA channel */
    CSL_EdmaParamHandle        hParamHndl,
    /** word offset in the 8 word paramater entry */
    Uint16                     wordOffset,
    /** word to be written */
    Uint32                     word 
)
{
	Uint32* hParam = (Uint32*)(hParamHndl);
	hParam[wordOffset] = word;	
	return CSL_SOK;
}
