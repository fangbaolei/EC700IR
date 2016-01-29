/*****************************************************\
 *  Copyright 2004, Texas Instruments Incorporated.  *
 *  All rights reserved.                             *
 *  Restricted rights to use, duplicate or disclose  *
 *  this   code   are  granted   through  contract.  *
 *                                                   *
 * "@(#) PSP/CSL  3.0.0.0  (2003-09-30)              *
\*****************************************************/
/** @file csl_intcHookIsr.c
 *
 *  @date 12th June, 2004
 *  @author Ruchika Kharwar
 */
#include <csl_intc.h>
#include <_csl_intc.h>
/** 
 *
 * <b> Usage Constraints: </b>
 * The CSL intc module and a handle must be opened prior to this call
 * @b Example:
   \code

	
	CSL_IntcObj intcObj20;
	CSL_IntcDropStatus drop;
    CSL_IntcGlobalEnableState state;
    
    CSL_IntcContext context; 
    CSL_Status intStat;
    CSL_IntcParam vectId;
    Uint32 intrStat;
    
    context.numEvtEntries = 0;
    context.eventhandlerRecord = NULL;
    // Init Module
	CSL_intcInit(&context);
	
    // NMI Enable	
	CSL_intcGlobalNmiEnable();
	
	// Enable Global Interrupts 
	intStat = CSL_intcGlobalEnable(&state);
	
	// Hook Isr appropriately
    CSL_intcHookIsr(CSL_INTC_VECTID_4,&isrVect4);
...
 }
interrupt void isrVect4()
{

}
   \endcode
 *
 * @return CSL Status 
 */
#pragma CODE_SECTION (CSL_intcHookIsr, ".text:csl_section:intc");

CSL_Status  CSL_intcHookIsr(
    CSL_IntcVectId        evtId,
    void                  *isrAddr
)
{
	Uint32 *dispPtr ;
	dispPtr = (Uint32*)(&_CSL_intcCpuIntrTable);
	dispPtr[evtId + 1] = (Uint32)(isrAddr);
    return(CSL_SOK);
}

/* Rev.No.   Date/Time               ECN No.          Modifier      */
/* -------   ---------               -------          --------      */

/* 1          Aug 11:10:54 9         2273             xkeshavm      */
/*                                                                  */
/* To upload API CSL 0.51 Release                                   */
/********************************************************************/ 
