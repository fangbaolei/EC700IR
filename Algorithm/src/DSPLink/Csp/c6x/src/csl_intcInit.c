/*****************************************************\
 *  Copyright 2004, Texas Instruments Incorporated.  *
 *  All rights reserved.                             *
 *  Restricted rights to use, duplicate or disclose  *
 *  this   code   are  granted   through  contract.  *
 *                                                   *
 * "@(#) PSP/CSL  3.0.0.0  (2003-09-30)              *
\*****************************************************/
/** @file csl_intcInit.c
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
   
   \endcode
 *
 * @return CSL Status 
 */
#pragma CODE_SECTION (CSL_intcInit, ".text:csl_section:intc");
CSL_Status
    CSL_intcInit (
        CSL_IntcContext *   pContext
)
{
	Uint16   cs;
    Uint16   i;
    cs = _CSL_beginCriticalSection( );
    if (pContext != NULL) {
		_CSL_intcEventOffsetMap =  pContext->offsetResv;
		_CSL_intcAllocMask = pContext->eventAllocMask;
		_CSL_intcNumEvents =  pContext->numEvtEntries;
		
		for (i = 0; i < ((CSL_INTC_EVENTID_CNT + 31) / 32); i++)
	        _CSL_intcAllocMask[i] = 0;
	             
	    for (i = 0; i < CSL_INTC_EVENTID_CNT ; i++)
	        _CSL_intcEventOffsetMap[i] = CSL_INTC_MAPPED_NONE;
		if (pContext->eventhandlerRecord != NULL) 
		{
			_CSL_intcEventHandlerRecord  = pContext->eventhandlerRecord;	        
		    for (i = 0; i < _CSL_intcNumEvents; i++) 
		    	_CSL_intcEventHandlerRecord[i].handler = CSL_INTC_EVTHANDLER_NONE;
		}    	
    } 
	_CSL_intcIvpSet();
	_CSL_intcCpuIntrTable.nmiIsr  = _CSL_intcNmiDummy;
    _CSL_intcCpuIntrTable.isr4  = _CSL_intcDispatcher;
    _CSL_intcCpuIntrTable.isr5  = _CSL_intcDispatcher;
    _CSL_intcCpuIntrTable.isr6  = _CSL_intcDispatcher;
    _CSL_intcCpuIntrTable.isr7  = _CSL_intcDispatcher;
    _CSL_intcCpuIntrTable.isr8  = _CSL_intcDispatcher;
    _CSL_intcCpuIntrTable.isr9  = _CSL_intcDispatcher;
    _CSL_intcCpuIntrTable.isr10 = _CSL_intcDispatcher;
    _CSL_intcCpuIntrTable.isr11 = _CSL_intcDispatcher;
    _CSL_intcCpuIntrTable.isr12 = _CSL_intcDispatcher;
    _CSL_intcCpuIntrTable.isr13 = _CSL_intcDispatcher;
    _CSL_intcCpuIntrTable.isr14 = _CSL_intcDispatcher;
    _CSL_intcCpuIntrTable.isr15 = _CSL_intcDispatcher;
	_CSL_endCriticalSection(cs);
	return CSL_SOK;
}

/* Rev.No.   Date/Time               ECN No.          Modifier      */
/* -------   ---------               -------          --------      */

/* 1          Aug 11:10:55 9         2273             xkeshavm      */
/*                                                                  */
/* To upload API CSL 0.51 Release                                   */
/********************************************************************/ 
