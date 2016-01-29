/*****************************************************\
 *  Copyright 2004, Texas Instruments Incorporated.  *
 *  All rights reserved.                             *
 *  Restricted rights to use, duplicate or disclose  *
 *  this   code   are  granted   through  contract.  *
 *                                                   *
 * "@(#) PSP/CSL  3.0.0.0  (2003-09-30)              *
\*****************************************************/
/* =========================================================
 *   @file  csl_intcClose.c
 *
 *   @path  $(CSLPATH)\src\intc
 *
 *   @desc  EDMA module 
 *
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
	
    // Close handle
    CSL_IntcClose(hIntc20);
   
   \endcode
 *
 * @return CSL Status 
 */
#pragma CODE_SECTION (CSL_intcClose, ".text:csl_section:intc");

CSL_Status  CSL_intcClose(
    CSL_IntcHandle                          hIntc
)
{
    
    CSL_Status  closeStatus = CSL_ESYS_BADHANDLE;
    Uint32 evt,x,y,cs;
    
    if (hIntc != (CSL_IntcHandle) CSL_INTC_BADHANDLE) {
        evt = (Uint32)(hIntc->eventId);
	    y = evt >> 5;
	    x = 1 << (evt & 0x1f);
	    
	    cs = _CSL_beginCriticalSection();
		_CSL_intcAllocMask[y] &= ~x; /* clear bit -> unused */
		if (_CSL_intcEventOffsetMap[hIntc->eventId] != CSL_INTC_MAPPED_NONE){
				_CSL_intcEventHandlerRecord[_CSL_intcEventOffsetMap[hIntc->eventId]].handler = CSL_INTC_EVTHANDLER_NONE;
				_CSL_intcEventOffsetMap[hIntc->eventId] = CSL_INTC_MAPPED_NONE;
		}	
		hIntc = (CSL_IntcHandle) CSL_INTC_BADHANDLE;
		_CSL_endCriticalSection(cs);
		closeStatus = CSL_SOK;		
    }
    return closeStatus;
}
/* Rev.No.   Date/Time               ECN No.          Modifier      */
/* -------   ---------               -------          --------      */

/* 1          Aug 11:10:52 9         2273             xkeshavm      */
/*                                                                  */
/* To upload API CSL 0.51 Release                                   */
/********************************************************************/ 
