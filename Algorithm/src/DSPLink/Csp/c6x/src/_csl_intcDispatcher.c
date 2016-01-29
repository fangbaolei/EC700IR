/*****************************************************\
 *  Copyright 2004, Texas Instruments Incorporated.  *
 *  All rights reserved.                             *
 *  Restricted rights to use, duplicate or disclose  *
 *  this   code   are  granted   through  contract.  *
 *                                                   *
 * "@(#) PSP/CSL  3.0.0.0  (2003-09-30)              *
\*****************************************************/
/** @file _csl_intcDispatcher.c
 *
 *  @date 12th June, 2004
 *  @author Ruchika Kharwar
 */
#include <csl_intc.h>
#include <_csl_intc.h>
#include <csl_intcAux.h>


#pragma CODE_SECTION (_CSL_intcDispatcher, ".text:csl_section:intc");

interrupt
    void
        _CSL_intcDispatcher(
            void
)
{
    Uint32 intrId = (_CSL_intcCpuIntrTable.currentVectId - (Uint32)(&_CSL_intcCpuIntrTable) - 4)/4;
    Uint32 evtId;
    
    /* Get the Event Source of the Interrupt */
    if (intrId < 8) 
    	evtId = CSL_FEXTR(((CSL_IntcRegsOvly)CSL_INTC_REGS)->INTMUX1,(intrId-4)*8+6,(intrId-4)*8);
    else {
    	if (intrId < 12) 
    		evtId = CSL_FEXTR(((CSL_IntcRegsOvly)CSL_INTC_REGS)->INTMUX2,(intrId-8)*8+6,(intrId-8)*8);	
    	else 
    		evtId = CSL_FEXTR(((CSL_IntcRegsOvly)CSL_INTC_REGS)->INTMUX3,(intrId-12)*8+6,(intrId-12)*8);	
    	}
   		if (_CSL_intcEventOffsetMap[evtId] != CSL_INTC_MAPPED_NONE) 
			        _CSL_intcEventHandlerRecord[_CSL_intcEventOffsetMap[evtId]].handler (
			            _CSL_intcEventHandlerRecord[_CSL_intcEventOffsetMap[evtId]].arg
	         );    
}
/* Rev.No.   Date/Time               ECN No.          Modifier      */
/* -------   ---------               -------          --------      */

/* 1          Aug 11:10:48 9         2273             xkeshavm      */
/*                                                                  */
/* To upload API CSL 0.51 Release                                   */
/********************************************************************/ 
