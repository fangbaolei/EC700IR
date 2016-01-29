/*****************************************************\
 *  Copyright 2004, Texas Instruments Incorporated.  *
 *  All rights reserved.                             *
 *  Restricted rights to use, duplicate or disclose  *
 *  this   code   are  granted   through  contract.  *
 *                                                   *
 * "@(#) PSP/CSL  3.0.0.0  (2003-09-30)              *
\*****************************************************/
/** @file csl_intcExcep.c
 *
 *  @date 12th June, 2004
 *  @author Ruchika Kharwar
 */


#include <csl_intc.h>

#pragma CODE_SECTION (CSL_intcExcepAllEnable, ".text:csl_section:intc");
CSL_Status CSL_intcExcepAllEnable(
		CSL_IntcExcepEn excepMask,
		CSL_BitMask32   excVal,
		CSL_IntcGlobalEnableState *prevState
		
)
{
	if (prevState) 
		*prevState = ((CSL_IntcRegsOvly)CSL_INTC_REGS)->EXPMASK[excepMask];
	((CSL_IntcRegsOvly)CSL_INTC_REGS)->EXPMASK[excepMask] &= ~excVal;
	return CSL_SOK;
}

#pragma CODE_SECTION (CSL_intcExcepAllDisable, ".text:csl_section:intc");
CSL_Status CSL_intcExcepAllDisable(
		CSL_IntcExcepEn excepMask,
		CSL_BitMask32   excVal,
		CSL_IntcGlobalEnableState *prevState
)
{
	if (prevState)
		*prevState = ((CSL_IntcRegsOvly)CSL_INTC_REGS)->EXPMASK[excepMask];
	((CSL_IntcRegsOvly)CSL_INTC_REGS)->EXPMASK[excepMask] |= excVal;
	return CSL_SOK;
}

#pragma CODE_SECTION (CSL_intcExcepAllRestore, ".text:csl_section:intc");
CSL_Status CSL_intcExcepAllRestore(
		CSL_IntcExcepEn excepMask,
		CSL_IntcGlobalEnableState   restoreVal
)
{
	((CSL_IntcRegsOvly)CSL_INTC_REGS)->EXPMASK[excepMask] = restoreVal;
	return CSL_SOK;
}

#pragma CODE_SECTION (CSL_intcExcepAllClear, ".text:csl_section:intc");
CSL_Status CSL_intcExcepAllClear(
		CSL_IntcExcepEn excepMask,
		CSL_BitMask32   excVal
)
{
	((CSL_IntcRegsOvly)CSL_INTC_REGS)->EVTCLR[excepMask] = excVal;
	return CSL_SOK;
}

#pragma CODE_SECTION (CSL_intcExcepAllStatus, ".text:csl_section:intc");
CSL_Status CSL_intcExcepAllStatus(
		CSL_IntcExcepEn excepMask,
		CSL_BitMask32   *status		
)
{
	if (status)
		*status = ((CSL_IntcRegsOvly)CSL_INTC_REGS)->MEXPFLAG[excepMask] ;
	return CSL_SOK;	
}

/* Rev.No.   Date/Time               ECN No.          Modifier      */
/* -------   ---------               -------          --------      */

/* 1          Aug 11:10:53 9         2273             xkeshavm      */
/*                                                                  */
/* To upload API CSL 0.51 Release                                   */
/********************************************************************/ 
