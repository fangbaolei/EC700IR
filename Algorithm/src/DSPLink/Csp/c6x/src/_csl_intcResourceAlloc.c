/*****************************************************\
 *  Copyright 2004, Texas Instruments Incorporated.  *
 *  All rights reserved.                             *
 *  Restricted rights to use, duplicate or disclose  *
 *  this   code   are  granted   through  contract.  *
 *                                                   *
 * "@(#) PSP/CSL  3.0.0.0  (2003-09-30)              *
\*****************************************************/
/** @file _csl_intcResourceAlloc.c
 *
 *  @date 12th June, 2004
 *  @author Ruchika Kharwar
 */
#include <csl_intc.h>
#include <_csl_intc.h>

#pragma DATA_SECTION (_CSL_intcAllocMask, ".bss:csl_section:intc");
CSL_BitMask32*   _CSL_intcAllocMask = NULL;

#pragma DATA_SECTION (_CSL_intcCpuIntrTable, ".bss:csl_section:intc");
CSL_IntcVect _CSL_intcCpuIntrTable;

#pragma DATA_SECTION (_CSL_intcEventOffsetMap, ".bss:csl_section:intc");
Int8 *_CSL_intcEventOffsetMap = NULL;

#pragma DATA_SECTION (_CSL_intcNumEvents, ".bss:csl_section:intc");
Uint16 _CSL_intcNumEvents;

#pragma DATA_SECTION (_CSL_intcEventHandlerRecord, ".bss:csl_section:intc");
CSL_IntcEventHandlerRecord* _CSL_intcEventHandlerRecord;
/* Rev.No.   Date/Time               ECN No.          Modifier      */
/* -------   ---------               -------          --------      */

/* 1          Aug 11:10:49 9         2273             xkeshavm      */
/*                                                                  */
/* To upload API CSL 0.51 Release                                   */
/********************************************************************/ 
