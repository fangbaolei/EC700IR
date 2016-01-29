/*****************************************************\
 *  Copyright 2004, Texas Instruments Incorporated.  *
 *  All rights reserved.                             *
 *  Restricted rights to use, duplicate or disclose  *
 *  this   code   are  granted   through  contract.  *
 *                                                   *
 * "@(#) PSP/CSL  3.0.0.0  (2003-09-30)              *
\*****************************************************/
/** @file csl_intcPlugEventHandler.c
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
    
    context.numEvtEntries = 20;
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
	
	EventRecord.handler = &event20Handler;
	EventRecord.arg = hIntc20;
    CSL_intcPlugEventHandler(hIntc20,&EventRecord);	
    // Close handle
    CSL_IntcClose(hIntc20);
   }
   
  void event20Handler( CSL_IntcHandle hIntc)
  {
  
  }
   \endcode
 *
 * @return CSL Status 
 */
#pragma CODE_SECTION (CSL_intcPlugEventHandler, ".text:csl_section:intc");

CSL_Status
	CSL_intcPlugEventHandler (
		CSL_IntcHandle			hIntc,
		CSL_IntcEventHandlerRecord *	eventHandlerRecord
)
{
    Uint16  cs,i;
    CSL_Status status = CSL_ESYS_FAIL;

	cs = _CSL_beginCriticalSection( );

	/* Search for free entry and plug in handler */
	
	for (i = 0; i < _CSL_intcNumEvents; i++)
	{
		if (_CSL_intcEventHandlerRecord[i].handler == CSL_INTC_EVTHANDLER_NONE)
		{
			/* Plug in Handler */
			_CSL_intcEventHandlerRecord[i].handler =
				eventHandlerRecord->handler;
			_CSL_intcEventHandlerRecord[i].arg =
				eventHandlerRecord->arg;	
			/* Establish mapping */
			_CSL_intcEventOffsetMap[hIntc->eventId] = i;
			status = CSL_SOK;
			break;			
		}
	
	}
	_CSL_endCriticalSection(cs);
	return status;
}

/* Rev.No.   Date/Time               ECN No.          Modifier      */
/* -------   ---------               -------          --------      */

/* 1          Aug 11:10:57 9         2273             xkeshavm      */
/*                                                                  */
/* To upload API CSL 0.51 Release                                   */
/********************************************************************/ 
