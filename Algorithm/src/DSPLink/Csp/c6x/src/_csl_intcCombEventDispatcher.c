/*****************************************************\
 *  Copyright 2004, Texas Instruments Incorporated.  *
 *  All rights reserved.                             *
 *  Restricted rights to use, duplicate or disclose  *
 *  this   code   are  granted   through  contract.  *
 *                                                   *
 * "@(#) PSP/CSL  3.0.0.0  (2003-09-30)              *
\*****************************************************/
/** @file _csl_intcCombEventDispatcher.c
 *
 *  @date 12th June, 2004
 *  @author Ruchika Kharwar
 */
#include <csl_intc.h>
#include <_csl_intc.h>
#include <csl_intcAux.h>
#pragma CODE_SECTION (_CSL_intcEvent0Dispatcher, ".text:csl_section:intc");

interrupt
    void
        _CSL_intcEvent0Dispatcher(
            void
)
{
	volatile CSL_BitMask32 evtRcv;
	Uint32 evtId, evtMask ;
	evtRcv = ((CSL_IntcRegsOvly)CSL_INTC_REGS)->MEVTFLAG[0];
	while (evtRcv) {
		/* Clear the events */
		((CSL_IntcRegsOvly)CSL_INTC_REGS)->EVTCLR[0] = evtRcv ;
		evtMask = 1<<4;
		evtId = 4;
        evtRcv &= ~(0xF);
        do {
        	if (evtRcv & evtMask) 
			{
			    if (_CSL_intcEventOffsetMap[evtId] != CSL_INTC_MAPPED_NONE) 
			        _CSL_intcEventHandlerRecord[_CSL_intcEventOffsetMap[evtId]].handler (
			            _CSL_intcEventHandlerRecord[_CSL_intcEventOffsetMap[evtId]].arg
			       );
				evtRcv &= ~evtMask;
			}	 
		 	evtMask = evtMask << 1;	     	
		 	evtId++;
	    } while (evtRcv); 	
		/* Read the MEVTFLAG[0] register */
		evtRcv = ((CSL_IntcRegsOvly)CSL_INTC_REGS)->MEVTFLAG[0];	
	}
}

#pragma CODE_SECTION (_CSL_intcEvent1Dispatcher, ".text:csl_section:intc");

interrupt
    void
        _CSL_intcEvent1Dispatcher(
            void
)
{
	volatile CSL_BitMask32 evtRcv;
	Uint32 evtId, evtMask ;
	
	/* Read the MEVTFLAG[0] register */
	evtRcv = ((CSL_IntcRegsOvly)CSL_INTC_REGS)->MEVTFLAG[1];
	while (evtRcv) {
		/* Clear the events */
		((CSL_IntcRegsOvly)CSL_INTC_REGS)->EVTCLR[1] = evtRcv ;
		evtId = 32;
		evtMask = 1;
        do {
    	    if (evtRcv & evtMask) 
			{
			    if (_CSL_intcEventOffsetMap[evtId] != CSL_INTC_MAPPED_NONE) 
			        _CSL_intcEventHandlerRecord[_CSL_intcEventOffsetMap[evtId]].handler (
			            _CSL_intcEventHandlerRecord[_CSL_intcEventOffsetMap[evtId]].arg
			        );
				evtRcv &= ~evtMask;
			}	 
			evtMask = evtMask << 1;	     	
		 	evtId++;
	    } while (evtRcv); 	
		/* Read the MEVTFLAG[1] register */
		evtRcv = ((CSL_IntcRegsOvly)CSL_INTC_REGS)->MEVTFLAG[1];
	}
}

#pragma CODE_SECTION (_CSL_intcEvent2Dispatcher, ".text:csl_section:intc");

interrupt
    void
        _CSL_intcEvent2Dispatcher(
            void
)
{
	volatile CSL_BitMask32 evtRcv;
	Uint32 evtId, evtMask ;
	/* Read the MEVTFLAG[2] register */
	evtRcv = ((CSL_IntcRegsOvly)CSL_INTC_REGS)->MEVTFLAG[2];
	while (evtRcv) {
		/* Clear the events */
		((CSL_IntcRegsOvly)CSL_INTC_REGS)->EVTCLR[2] = evtRcv ;
		evtId = 64;
		evtMask = 1;
        do {
			if (evtRcv & evtMask) 
			{
			    if (_CSL_intcEventOffsetMap[evtId] != CSL_INTC_MAPPED_NONE) 
			        _CSL_intcEventHandlerRecord[_CSL_intcEventOffsetMap[evtId]].handler (
			            _CSL_intcEventHandlerRecord[_CSL_intcEventOffsetMap[evtId]].arg
			        );
				evtRcv &= ~evtMask;
			}	 
		 	evtMask = evtMask << 1;	     	
		 	evtId++;
	    } while (evtRcv); 	
	    
		/* Read the MEVTFLAG[2] register */
		evtRcv = ((CSL_IntcRegsOvly)CSL_INTC_REGS)->MEVTFLAG[2];
	}
}

#pragma CODE_SECTION (_CSL_intcEvent3Dispatcher, ".text:csl_section:intc");

interrupt
    void
        _CSL_intcEvent3Dispatcher(
            void
)
{
	volatile CSL_BitMask32 evtRcv;
	Uint32 evtId, evtMask ;
	/* Read the MEVTFLAG[3] register */
	evtRcv = ((CSL_IntcRegsOvly)CSL_INTC_REGS)->MEVTFLAG[3];
	while (evtRcv) {
		/* Clear the events */
		((CSL_IntcRegsOvly)CSL_INTC_REGS)->EVTCLR[3] = evtRcv ;
		evtId = 96;
		evtMask = 1;
        do {
			if (evtRcv & evtMask) 
			{
			    if (_CSL_intcEventOffsetMap[evtId] != CSL_INTC_MAPPED_NONE) 
			        _CSL_intcEventHandlerRecord[_CSL_intcEventOffsetMap[evtId]].handler (
			            _CSL_intcEventHandlerRecord[_CSL_intcEventOffsetMap[evtId]].arg
			        );
				evtRcv &= ~evtMask;
			}	 
		    evtMask = evtMask << 1;	  
		    evtId++;   	
	    } while (evtRcv); 	
       /* Read the MEVTFLAG[3] register */
		evtRcv = ((CSL_IntcRegsOvly)CSL_INTC_REGS)->MEVTFLAG[3];
	}
}
/* Rev.No.   Date/Time               ECN No.          Modifier      */
/* -------   ---------               -------          --------      */

/* 1          Aug 11:10:47 9         2273             xkeshavm      */
/*                                                                  */
/* To upload API CSL 0.51 Release                                   */
/********************************************************************/ 
