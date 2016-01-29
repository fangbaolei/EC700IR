/*****************************************************\
 *  Copyright 2004, Texas Instruments Incorporated.  *
 *  All rights reserved.                             *
 *  Restricted rights to use, duplicate or disclose  *
 *  this   code   are  granted   through  contract.  *
 *                                                   *
 * "@(#) PSP/CSL  3.0.0.0  (2003-09-30)              *
\*****************************************************/
/** @file csl_intcGetHwStatus.c
 *
 *  @date 12th June, 2004
 *  @author Ruchika Kharwar
 */
//#include <cslr_intc.h>
#include <csl_intc.h>
#include <_csl_intc.h>
#include <csl_intcAux.h>
/** 
 *
 * <b> Usage Constraints: </b>
 * The CSL intc module and a handle must be opened prior to this call
 * @b Example:
   \code

	
	CSL_IntcObj intcObj20;
	
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
	
	// Opening a handle for the Event 20 at vector id 4
	
	vectId = CSL_INTC_VECTID_4;
	hIntc20 = CSL_intcOpen (&intcObj20, CSL_INTC_EVENTID_20, &vectId , NULL);
    
    do {
		CSL_intcGetHwStatus(hIntc20,CSL_INTC_QUERY_PENDSTATUS,(void*)&intrStat);
	} while (!stat);
	// Close handle
    CSL_IntcClose(hIntc20);
   \endcode
 *
 * @return CSL Status 
 */
#pragma CODE_SECTION (CSL_intcGetHwStatus, ".text:csl_section:intc");

CSL_Status  CSL_intcGetHwStatus(
		CSL_IntcHandle            hIntc,
		CSL_IntcHwStatusQuery     myQuery,
		void                      *answer
		)
{


	CSL_Status status = CSL_SOK;
    if (hIntc && answer)
    {
		switch (myQuery)
		{
			case CSL_INTC_QUERY_PENDSTATUS:
				if (hIntc->vectId < CSL_INTC_VECTID_EXCEP) {
					if (hIntc->eventId < 4)
						*((Uint32*)answer) = (Uint32)(CSL_intcCombinedEventGet(hIntc->eventId));
					else	
						*((Bool*)answer) = (Bool)(CSL_intcQueryEventStatus(hIntc->eventId));
				}	
				break;	
			default:
				status = CSL_ESYS_INVCMD;
				break;
		}		
	} else
		status = CSL_ESYS_INVPARAMS;
	return status;
}

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
	
	// Opening a handle for the Event 20 at vector id 4
	
	vectId = CSL_INTC_VECTID_4;
	hIntc20 = CSL_intcOpen (&intcObj20, CSL_INTC_EVENTID_20, &vectId , NULL);
    CSL_intcQueryDropStatus(&drop);
    // Close handle
    CSL_IntcClose(hIntc20);
   \endcode
 *
 * @return CSL Status 
 */
#pragma CODE_SECTION (CSL_intcQueryDropStatus, ".text:csl_section:intc");
CSL_Status CSL_intcQueryDropStatus(
		CSL_IntcDropStatus *drop		
)
{
	Uint32 dropVal;
	CSL_Status st = CSL_ESYS_INVPARAMS ;
	if (drop) {
		dropVal = ((CSL_IntcRegsOvly)CSL_INTC_REGS)->INTXSTAT;
	 	drop->drop = (Bool)(CSL_FEXT(dropVal,INTC_INTXSTAT_DROP));
		drop->eventId = (CSL_IntcEventId)(CSL_FEXT(dropVal,INTC_INTXSTAT_SYSINT));
		drop->vectId = (CSL_IntcVectId)(CSL_FEXT(dropVal,INTC_INTXSTAT_CPUINT));
		/* Clear the drop condition */
		((CSL_IntcRegsOvly)CSL_INTC_REGS)->INTXCLR = 1;
		st = CSL_SOK;	 
	} 
	return st;
}
/* Rev.No.   Date/Time               ECN No.          Modifier      */
/* -------   ---------               -------          --------      */

/* 1          Aug 11:10:54 9         2273             xkeshavm      */
/*                                                                  */
/* To upload API CSL 0.51 Release                                   */
/********************************************************************/ 
