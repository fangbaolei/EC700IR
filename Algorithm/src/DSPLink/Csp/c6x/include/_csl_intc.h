/*****************************************************\
 *  Copyright 2004, Texas Instruments Incorporated.  *
 *  All rights reserved.                             *
 *  Restricted rights to use, duplicate or disclose  *
 *  this   code   are  granted   through  contract.  *
 *                                                   *
 * "@(#) PSP/CSL  3.0.0.0  (2003-09-30)              *
\*****************************************************/
/** @file _csl_intc.h
 *
 *  @date 12th June, 2004
 *  @author Ruchika Kharwar
 */
#ifndef __CSL_INTC_H_
#define __CSL_INTC_H_

#include <csl_intc.h>

void _CSL_intcNmiDummy();

typedef struct CSL_IntcVect {
	Uint32 currentVectId;
	void (*resv0)();
	void (*nmiIsr)();
	void (*resv2)();
	void (*resv3)();
	void (*isr4)();
	void (*isr5)();
	void (*isr6)();
	void (*isr7)();
	void (*isr8)();
	void (*isr9)();
	void (*isr10)();
	void (*isr11)();
	void (*isr12)();
	void (*isr13)();
	void (*isr14)();
	void (*isr15)();
}CSL_IntcVect;
extern CSL_IntcVect _CSL_intcCpuIntrTable;
/* These declarations are meant for computing the ISR jump location. */
void _CSL_intcIvpSet();
interrupt
	void
		_CSL_intcDispatcher (
			void
);
interrupt
    void
        _CSL_intcEvent0Dispatcher(
            void
);
interrupt
    void
        _CSL_intcEvent1Dispatcher(
            void
);
interrupt
    void
        _CSL_intcEvent2Dispatcher(
            void
);
interrupt
    void
        _CSL_intcEvent3Dispatcher(
            void
);
#endif /* __CSL_INTC_H_ */
/* Rev.No.   Date/Time               ECN No.          Modifier      */
/* -------   ---------               -------          --------      */

/* 1          Aug 11:11:52 9         2274             xkeshavm      */
/*                                                                  */
/* To upload API CSL 0.51 Release                                   */
/********************************************************************/ 
