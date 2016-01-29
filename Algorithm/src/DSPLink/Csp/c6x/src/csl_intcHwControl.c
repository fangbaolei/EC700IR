/*****************************************************\
 *  Copyright 2004, Texas Instruments Incorporated.  *
 *  All rights reserved.                             *
 *  Restricted rights to use, duplicate or disclose  *
 *  this   code   are  granted   through  contract.  *
 *                                                   *
 * "@(#) PSP/CSL  3.0.0.0  (2003-09-30)              *
\*****************************************************/
/** @file csl_intcHwControl.c
 *
 *  @date 12th June, 2004
 *  @author Ruchika Kharwar
 */
#include <csl_intc.h>
#include <_csl_intc.h>
#include <csl_intcAux.h>

/** 
 *
 * <b> Usage Constraints: </b>
 * The CSL intc module and a handle must be opened prior to this API
 * @b Example:
   \code

	
	CSL_IntcObj intcObj20;
	
    CSL_IntcGlobalEnableState state;
    
    CSL_IntcContext context; 
    CSL_Status intStat;
    CSL_IntcParam vectId;
    
    context.numEvtEntries = 0;
    context.eventhandlerRecord = NULL;
    // Init Module
	CSL_intcInit(&context);
	
    // NMI Enable	
	CSL_intcGlobalNmiEnable();
	
	// Enable Global Interrupts 
	intStat = CSL_intcGlobalEnable(&state);
	
	// Opening a handle for the Event 20 at vector id 4
	
	vectId = CSL_INTC_VECTID_4;
	hIntc20 = CSL_intcOpen (&intcObj20, CSL_INTC_EVENTID_20, &vectId , NULL);
	
	CSL_intcHwControl(hIntc20,CSL_INTC_CMD_EVTENABLE,NULL);
    // Close handle
    CSL_IntcClose(hIntc20);
   
   \endcode
 *
 * @return CSL Status 
 */
#pragma CODE_SECTION (CSL_intcHwControl, ".text:csl_section:intc");

CSL_Status  CSL_intcHwControl(
		CSL_IntcHandle            hIntc,
		CSL_IntcHwControlCmd      controlCommand,
		void*                     commandArg
		)

{
	CSL_Status status = CSL_SOK;
	switch(controlCommand)
	{
		
		case CSL_INTC_CMD_EVTENABLE:                   
		    if (hIntc->vectId < CSL_INTC_VECTID_EXCEP)
		    {
		    	if (hIntc->vectId < CSL_INTC_VECTID_COMBINE)
		    	{
		    		if (hIntc->eventId < 4) {
			    		if (commandArg)
			    			CSL_intcCombinedEventEnable(hIntc->eventId,*(CSL_BitMask32*)commandArg);
		    		}
		    		CSL_intcInterruptEnable(hIntc->vectId);		    	
		    		CSL_intcEventDisable(hIntc->eventId);				 	
			    }		
		    	else 
				 	CSL_intcEventEnable(hIntc->eventId);				 	
		    } else 		    
		    	CSL_intcExcepEnable(hIntc->eventId);
			break;
		
		case CSL_INTC_CMD_EVTDISABLE:
		    if (hIntc->vectId < CSL_INTC_VECTID_EXCEP)
		    {
		    	if (hIntc->vectId < CSL_INTC_VECTID_COMBINE)
		    	{
		    		if (hIntc->eventId < 4) {
			    		if (commandArg)
			    			CSL_intcCombinedEventDisable(hIntc->eventId,*(CSL_BitMask32*)commandArg);
		    		}
		    		CSL_intcInterruptDisable(hIntc->vectId);		    	
			    }		
		    	else 
				 	CSL_intcEventDisable(hIntc->eventId);
		    } else 		    
		    	CSL_intcExcepDisable(hIntc->eventId);
			break;
				
		case CSL_INTC_CMD_EVTCLEAR:		
			if (hIntc->vectId < CSL_INTC_VECTID_EXCEP)
		    {
	    		//CSL_intcInterruptClear(hIntc->vectId);	
	    		if (hIntc->eventId < 4) 
	    		{
		    		if (commandArg)
			   			CSL_intcCombinedEventClear(hIntc->eventId,*(CSL_BitMask32*)commandArg);
		    	} 
		    	else		    		
		    		CSL_intcEventClear(hIntc->eventId);
		    } else 		    
		    	CSL_intcExcepClear(hIntc->eventId);

			break;
		
		case CSL_INTC_CMD_EVTSET:
			CSL_intcEventSet(hIntc->eventId);		    	
		    break;					
		case CSL_INTC_CMD_EVTDROPENABLE:	
			if (hIntc->vectId < CSL_INTC_VECTID_COMBINE)
				CSL_intcInterruptDropEnable(1 << hIntc->vectId); 
			else 
				status = CSL_ESYS_INVCMD;
			break;
		
		case CSL_INTC_CMD_EVTDROPDISABLE:	
			if (hIntc->vectId < CSL_INTC_VECTID_COMBINE)
				CSL_intcInterruptDropDisable(1 << hIntc->vectId); 
			else 
				status = CSL_ESYS_INVCMD;	
			break;
		case CSL_INTC_CMD_EVTINVOKEFUNCTION:	
 			CSL_intcInvokeEventHandle (hIntc->eventId);
 			break;
		default:
			status = CSL_ESYS_INVCMD;
			break;

		}
	return status;
}
/* Rev.No.   Date/Time               ECN No.          Modifier      */
/* -------   ---------               -------          --------      */

/* 1          Aug 11:10:55 9         2273             xkeshavm      */
/*                                                                  */
/* To upload API CSL 0.51 Release                                   */
/********************************************************************/ 
