/*****************************************************\
 *  Copyright 2004, Texas Instruments Incorporated.  *
 *  All rights reserved.                             *
 *  Restricted rights to use, duplicate or disclose  *
 *  this   code   are  granted   through  contract.  *
 *                                                   *
 * "@(#) PSP/CSL  3.0.0.0  (2003-09-30)              *
\*****************************************************/
/** @file csl_intcOpen.c
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
	
    // Close handle
    CSL_IntcClose(hIntc20);
   
   \endcode
 *
 * @return CSL Status 
 */
#pragma CODE_SECTION (CSL_intcOpen, ".text:csl_section:intc");

CSL_IntcHandle  CSL_intcOpen(
    CSL_IntcObj                             *intcObj,
    CSL_IntcEventId                         eventId,
    CSL_IntcParam                           *param,
    CSL_Status                              *status
)
{
    CSL_IntcHandle  h = (CSL_IntcHandle)CSL_INTC_BADHANDLE;
    CSL_Status      openStatus;
    Uint16          cs;
    Uint32          evtId;      
    Uint32  _yCo,_xCo;
    evtId = (Uint32)(eventId);
    _yCo = evtId >> 5;          /* _yCo co-ordinate */
	_xCo = 1 << (evtId & 0x1f); /* _xCo co-ordinate */
    cs = _CSL_beginCriticalSection();

    if (!(_CSL_intcAllocMask[_yCo] & _xCo)) {
		_CSL_intcAllocMask[_yCo] |= _xCo; /* set bit -> used */
		h = intcObj;     
	}
    _CSL_endCriticalSection(cs);
    if (h != (CSL_IntcHandle)CSL_INTC_BADHANDLE) {
        intcObj->eventId    = eventId;
        intcObj->vectId     = *((CSL_IntcVectId*)param);
        openStatus          = CSL_SOK;
        /* Do the Mapping for the Event -> CPU Interrupt */
        if (*((CSL_IntcVectId*)param) < ((Uint32)(CSL_INTC_VECTID_COMBINE))) {
        	CSL_intcMapEventVector(eventId,*param);	
        	if (eventId < 4)
        	{
        		switch (eventId) {
        			case 0:
        				CSL_intcHookIsr(*((CSL_IntcVectId*)param),_CSL_intcEvent0Dispatcher);
        				break;
        			case 1:
        				CSL_intcHookIsr(*((CSL_IntcVectId*)param),_CSL_intcEvent1Dispatcher);
        				break;
        			case 2:
        				CSL_intcHookIsr(*((CSL_IntcVectId*)param),_CSL_intcEvent2Dispatcher);
        				break;
        			case 3:
        				CSL_intcHookIsr(*((CSL_IntcVectId*)param),_CSL_intcEvent3Dispatcher);
        				break;        			
        		}        	
        	}
        	
        }        
	} else 
	    openStatus          = CSL_ESYS_FAIL;

    if (status) {
        *status = openStatus;
    }
    return h;
}
/* Rev.No.   Date/Time               ECN No.          Modifier      */
/* -------   ---------               -------          --------      */

/* 1          Aug 11:10:56 9         2273             xkeshavm      */
/*                                                                  */
/* To upload API CSL 0.51 Release                                   */
/********************************************************************/ 
