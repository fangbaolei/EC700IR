/*  ============================================================================
 *   Copyright (c) Texas Instruments Inc 2002, 2003, 2004, 2005                 
 *                                                                              
 *   Use of this software is controlled by the terms and conditions found in the
 *   license agreement under which this software has been supplied.             
 *   ===========================================================================
 */

/*
 *  @file  _csl_int.h
 *
 *  @brief  File for functional layer of INTC CSL
 *
 *  PATH  $(CSLPATH)\inc
 */
 
/* =============================================================================
 *  Revision History
 *  ===============
 *  12-Jun-2004 Ruchika Kharwar File Created
 *  
 * =============================================================================
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
} CSL_IntcVect;
extern CSL_IntcVect _CSL_intcCpuIntrTable;

/* These declarations are meant for computing the ISR jump location. */
void _CSL_intcIvpSet();
interrupt void _CSL_intcDispatcher (void);
interrupt void _CSL_intcEvent0Dispatcher (void);
interrupt void _CSL_intcEvent1Dispatcher (void);
interrupt void _CSL_intcEvent2Dispatcher (void);
interrupt void _CSL_intcEvent3Dispatcher (void);

#endif /* __CSL_INTC_H_ */
