/* ============================================================================
 * Copyright (c) Texas Instruments Inc 2002, 2003, 2004, 2005
 *
 * Use of this software is controlled by the terms and conditions found in the
 * license agreement under which this software has been supplied.
 * =========================================================================
 */

/** ===========================================================================
 * @file csl_tmrAux.h
 *
 * @brief API Auxilary header file for TIMER CSL
 * 
 * @Path $(CSLPATH)\inc
 *  
 * @desc It gives the definitions of the status query & control functions.
 * ============================================================================
 */

/* ============================================================================
 * Revision History
 * ===============
 * 1-Sept-2004 HMM File Created.
 * 29-Jul-2005 PSK updted changes acooriding to revised timer spec. the number
 *                 convention TIM12, TIM34 are changed to TIMLO and TIMHI.  
 * ============================================================================
 */

#ifndef _CSL_TMRAUX_H_
#define _CSL_TMRAUX_H_

#include <csl_tmr.h>

#ifdef __cplusplus
extern "C" {
#endif

/*  Status Query Functions of General purpose timer */


/** ============================================================================
 *   @n@b CSL_tmrGetTimHiCount
 *
 *   @b Description
 *   @n This function gets the value of the TIMHI counter
 *
 *   @b Arguments
 *   @verbatim
         hTmr        Pointer to the object that holds reference to the 
                     instance of TIMER requested after the call 

         countHi     output parameter to get the TIMHI value
     @endverbatim
 *
 *   <b> Return Value </b>  None
 *
 *   <b> Pre Condition </b>
 *   @n  Timer should be set to GPTimer OR Unchained mode OR Chained mode
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b  Modifies
 *   @n  None
 *
 *   @b  Example
 *   @verbatim
        CSL_TmrHandle hTmr;
        Unit32        *countHi;
        ...
        CSL_tmrGetTimHiCount(hTmr, &countHi);
        ...
     @endverbatim
 * ============================================================================
 */
static inline
 void CSL_tmrGetTimHiCount (
    CSL_TmrHandle hTmr,
    Uint32        *countHi
)
{
    *countHi = hTmr->regs->TIMHI;
}



/** ===========================================================================
 *   @n@b CSL_tmrGetTimLoCount
 *
 *   @b Description
 *   @n This function gets the value of the TIMLO counter
 *
 *   @b Arguments
 *   @verbatim
         hTmr        Pointer to the object that holds reference to the 
                     instance of TIMER requested after the call 
                        
         countLo     output parameter to get the TIMHI value
     @endverbatim
 *
 *   <b> Return Value </b>  None
 *
 *   <b> Pre Condition </b>
 *   @n  Timer should be set to GPTimer OR Unchained mode OR Chained mode
 *
 *   <b> Post Condition </b>
 *   @n None
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim
        CSL_TmrHandle hTmr;
        Unit32        *countLo;
        ...
        CSL_tmrGetTimLoCount(hTmr, &countLo);
        ...
     @endverbatim
 * ============================================================================ 
 */
static inline
void CSL_tmrGetTimLoCount (
    CSL_TmrHandle hTmr,
    Uint32        *countLo
)
{
    *countLo = hTmr->regs->TIMLO;
}



/** ===========================================================================
 *   @n@b CSL_tmrGetTstatLoStatus
 *
 *   @b Description
 *   @n This function gets the status of the TINTLO
 *
 *   @b Arguments
 *   @verbatim
         hTmr        Pointer to the object that holds reference to the 
                     instance of TIMER requested after the call 
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_TmrTstatLo - TINTLO status value
 *
 *   @li        CSL_TMR_TSTAT_LO_LOW  - Data low
 *   @li        CSL_TMR_TSTAT_LO_HIGH - Data High
 *
 *   <b> Pre Condition </b>
 *   @n  Timer should be set to GPTimer OR Unchained mode OR Chained mode
 *
 *   <b> Post Condition </b>
 *    @n TINTLO status value
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim
        CSL_TmrHandle  hTmr;
        CSL_TmrTstatLo statusLo;
        ...
        statusLo = CSL_tmrGetTstatLoStatus(hTmr);
        ...
     @endverbatim
 * ===========================================================================
 */
static inline
CSL_TmrTstat CSL_tmrGetTstatLoStatus (
    CSL_TmrHandle hTmr
)
{
    return (CSL_TmrTstat)CSL_FEXT(hTmr->regs->TCR, TMR_TCR_TSTAT_LO);
}



/** ===========================================================================
 *   @n@b CSL_tmrGetTstatHiStatus
 *
 *   @b Description
 *   @n This function gets the status of the TINTHI
 *
 *   @b Arguments
 *   @verbatim
         hTmr        Pointer to the object that holds reference to the 
                     instance of TIMER requested after the call 
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_TmrTstatHi - TINTHI status value
 *
 *   @li        CSL_TMR_TSTATHI_LOW  - Data low
 *   @li        CSL_TMR_TSTATHI_HIGH - Data High
 *
 *   <b> Pre Condition </b>
 *   @n  Timer should be set to GPTimer OR Unchained mode OR Chained mode
 *
 *   <b> Post Condition </b>
 *    @n TINTHI status value
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim
        CSL_TmrHandle  hTmr;
        CSL_TmrTstatHi statusHi;
        ...
        statusHi = CSL_tmrGetTstatHiStatus(hTmr);
        ...
     @endverbatim
 * ============================================================================
 */
static inline
CSL_TmrTstat CSL_tmrGetTstatHiStatus (
    CSL_TmrHandle hTmr
)
{
    return (CSL_TmrTstat)CSL_FEXT(hTmr->regs->TCR, TMR_TCR_TSTAT_HI);
}



/** ===========================================================================
 *   @n@b CSL_tmrGetWdflagStatus
 *
 *   @b Description
 *   @n This function gets the status of the WDFLAG
 *
 *   @b Arguments
 *   @verbatim

         hTmr        Pointer to the object that holds reference to the 
                        instance of TIMER requested after the call 
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_TmrFlagBitStatus - WDFLAG status value
 *
 *   @li        CSL_TMR_WDFLAG_NOTIMEOUT  - no watchdog timeout occured
 *   @li        CSL_TMR_WDFLAG_TIMEOUT - watchdog timeout occured
 *
 *   <b> Pre Condition </b>
 *   @n  Timer should be set to  WATCHDOG mode
 *
 *   <b> Post Condition </b>
 *    @n WDFLAG  status value
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim
        CSL_TmrHandle          hTmr;
        CSL_TmrWdflagBitStatus status;
        ...
        status = CSL_tmrGetWdflagStatus(hTmr);
        ...
     @endverbatim
 * ===========================================================================
 */
static inline
CSL_TmrWdflagBitStatus CSL_tmrGetWdflagStatus (
    CSL_TmrHandle hTmr
)
{
    return (CSL_TmrWdflagBitStatus)CSL_FEXT(hTmr->regs->WDTCR, TMR_WDTCR_WDFLAG);
}



/** ============================================================================
 *   @n@b CSL_TmrLoadPrdLo
 *
 *   @b Description
 *      Loads the General purpose timer period register Low
 *
 *   @b Arguments
 *   @verbatim
         hTmr        Handle to the Gptimer instance

         loadVal     Value to be loaded to the Gptimer period register Low
     @endverbatim
 *
 *   <b> Return Value </b>
 *       None
 *
 *   <b> Pre Condition </b>
 *   @n  Timer should be set it to timer mode
 *
 *   <b> Post Condition </b>
 *   @n  Gptimer period register is loaded with the given value.
 *
 *
 *   @b Modifies
 *   @n Gptimer period register Low
 *
 *   @b Example
 *   @verbatim
        CSL_TmrHandle hTmr;
        Uint32        *loadVal;
        ...
        CSL_TmrLoadPrdLo(hWdt, &loadVal);
        ...
     @endverbatim
 * ===========================================================================
 */
static inline
void CSL_TmrLoadPrdLo (
    CSL_TmrHandle hTmr,
    Uint32        *loadVal
)
{
    hTmr->regs->PRDLO = *(loadVal);
}



/** ============================================================================
 *   @n@b CSL_TmrLoadPrdHi
 *
 *   @b Description
 *      Loads the General purpose timer period register High
 *
 *   @b Arguments
 *   @verbatim
         hTmr        Handle to the GPtimer instance

         loadVal     Value to be loaded to the Gptimer period register High
     @endverbatim
 *
 *   <b> Return Value </b>
 *       None
 *
 *   <b> Pre Condition </b>
 *   @n  Timer should be set it to timer mode
 *
 *   <b> Post Condition </b>
 *   @n  Gptimer period register is loaded with the given value.
 *
 *
 *   @b Modifies
 *   @n Gptimer period register High
 *
 *   @b Example
 *   @verbatim
        CSL_TmrHandle hTmr;
        Uint32        *loadVal;
        ...
        CSL_TmrLoadPrdHi(hWdt, &loadVal);
        ...
     @endverbatim
 * ===========================================================================
 */
static inline
void CSL_TmrLoadPrdHi (
    CSL_TmrHandle hTmr,
    Uint32        *loadVal
)
{
    hTmr->regs->PRDHI = *(loadVal);
}



/** ============================================================================
 *   @n@b CSL_TmrLoadPrescalarHi
 *
 *   @b Description
 *      Loads the General purpose timer prescalr register High
 *
 *   @b Arguments
 *   @verbatim
         hTmr        Handle to the Gptimer instance

         loadVal     Value to be loaded to the Gptimer prescalar register High
     @endverbatim
 *
 *   <b> Return Value </b>
 *       None
 *
 *   <b> Pre Condition </b>
 *   @n  Timer should be set it to unchained mode
 *
 *   <b> Post Condition </b>
 *   @n  Gptimer prescalar register is loaded with the given value.
 *
 *
 *   @b Modifies
 *   @n Gptimer prescalar register High
 *
 *   @b Example
 *   @verbatim
        CSL_TmrHandle hTmr;
        Uint8         *loadVal;
        ...
        CSL_TmrLoadPrescalarHi(hWdt, &loadVal);
        ...
     @endverbatim
 * ===========================================================================
 */
static inline
void CSL_TmrLoadPrescalarHi (
    CSL_TmrHandle hTmr,
    Uint8         *loadVal
)
{
    CSL_FINS(hTmr->regs->TGCR, TMR_TGCR_PSCHI, *((Uint8 *)loadVal));
}



/** ============================================================================
 *   @n@b CSL_TmrStartHi
 *
 *   @b Description
 *      sets the timer counting mode and timer reset
 *
 *   @b Arguments
 *   @verbatim
         hTmr        Handle to the Gptimer instance

         countMode   specifies the timer counting mode
     @endverbatim
 *
 *   <b> Return Value </b>
 *       None
 *
 *   <b> Pre Condition </b>
 *   @n  Timer should be set it to any GPtimer mode
 *
 *   <b> Post Condition </b>
 *   @n   Set the GP Timer counting mode and timer High reset.
 *
 *   @b Modifies
 *   @n Gptimer TCR and TGCR register of timer High
 *
 *   @b Example
 *   @verbatim
        CSL_TmrHandle  hTmr;
        CSL_TmrEnamode countMode;
        ...
        CSL_TmrStartHi(hWdt, countMode);
        ...
     @endverbatim
 * ===========================================================================
 */
static inline
void CSL_TmrStartHi (
    CSL_TmrHandle  hTmr,
    CSL_TmrEnamode countMode
)
{
    CSL_FINST(hTmr->regs->TGCR, TMR_TGCR_TIMHIRS, RESET_OFF);
    CSL_FINS(hTmr->regs->TCR, TMR_TCR_ENAMODE_HI, countMode);
}



/** ============================================================================
 *   @n@b CSL_TmrStartLo
 *
 *   @b Description
 *      sets the timer counting mode and timer reset
 *
 *   @b Arguments
 *   @verbatim
         hTmr        Handle to the Gptimer instance

         countMode   specifies the timer counting mode
     @endverbatim
 *
 *   <b> Return Value </b>
 *       None
 *
 *   <b> Pre Condition </b>
 *   @n  Timer should be set it to any GPtimer mode
 *
 *   <b> Post Condition </b>
 *   @n   Set the GP Timer counting mode and timer Low reset.
 *
 *
 *   @b Modifies
 *   @n Gptimer TCR and TGCR register of timer Low
 *
 *   @b Example
 *   @verbatim
        CSL_TmrHandle  hTmr;
        CSL_TmrEnamode countMode;
        ...
        CSL_TmrStartLo(hWdt, countMode);
        ...
     @endverbatim
 * ===========================================================================
 */
static inline
void CSL_TmrStartLo (
    CSL_TmrHandle  hTmr,
    CSL_TmrEnamode countMode
)
{
    CSL_FINST(hTmr->regs->TGCR, TMR_TGCR_TIMLORS, RESET_OFF);
    CSL_FINS(hTmr->regs->TCR, TMR_TCR_ENAMODE_LO, countMode);
}



/** ===========================================================================
 *   @n@b CSL_TmrStart64
 *
 *   @b Description
 *      sets the timer counting mode and timer reset of GP OR chained timer
 *
 *   @b Arguments
 *   @verbatim
         hTmr        Handle to the Gptimer instance

         countMode   specifies the timer counting mode
     @endverbatim
 *
 *   <b> Return Value </b>
 *       None
 *
 *   <b> Pre Condition </b>
 *   @n  Timer should be set it to any GPtimer mode
 *
 *   <b> Post Condition </b>
 *   @n   Set the GP OR chained timer counting mode and
 		  timer Low and High reset register.
 *
 *
 *   @b Modifies
 *   @n Gptimer TCR and TGCR register of timer Low AND High
 *
 *   @b Example
 *   @verbatim
        CSL_TmrHandle  hTmr;
        CSL_TmrEnamode countMode;
        ...
        CSL_TmrStart64(hWdt, countMode);
        ...
     @endverbatim
 * ===========================================================================
 */
static inline
void CSL_TmrStart64 (
    CSL_TmrHandle  hTmr,
    CSL_TmrEnamode countMode
)
{
    CSL_FINST(hTmr->regs->TGCR, TMR_TGCR_TIMLORS, RESET_OFF);
    CSL_FINST(hTmr->regs->TGCR, TMR_TGCR_TIMHIRS, RESET_OFF);
    CSL_FINS(hTmr->regs->TCR, TMR_TCR_ENAMODE_LO, countMode);
    CSL_FINS(hTmr->regs->TCR, TMR_TCR_ENAMODE_HI, countMode);
}



/** ===========================================================================
 *   @n@b CSL_TmrStopLo
 *
 *   @b Description
 *      sets the timer counting mode to stop the timer.
 *	    The timer maintains the current value
 *
 *   @b Arguments
 *   @verbatim
         hTmr        Handle to the Gptimer instance
     @endverbatim
 *
 *   <b> Return Value </b>
 *       None
 *
 *   <b> Pre Condition </b>
 *   @n  Timer should be set it to any GPtimer mode
 *
 *   <b> Post Condition </b>
 *   @n   Set the GP Timer counting mode to stop the counting.
 *
 *
 *   @b Modifies
 *   @n Gptimer TCR of timer Low
 *
 *   @b Example
 *   @verbatim
        CSL_TmrHandle hTmr;
        ...
        CSL_TmrStopLo(hWdt);
        ...
     @endverbatim
 * ===========================================================================
 */
static inline
void CSL_TmrStopLo (
    CSL_TmrHandle hTmr
)
{
    CSL_FINST(hTmr->regs->TCR, TMR_TCR_ENAMODE_LO, DISABLE);
}



/** ===========================================================================
 *   @n@b CSL_TmrStopHi
 *
 *   @b Description
 *      sets the timer counting mode to stop the timer.
 *	    The timer maintains the current value
 *
 *   @b Arguments
 *   @verbatim
         hTmr        Handle to the Gptimer instance
     @endverbatim
 *
 *   <b> Return Value </b>
 *       None
 *
 *   <b> Pre Condition </b>
 *   @n  Timer should be set it to any GPtimer mode
 *
 *   <b> Post Condition </b>
 *   @n   Set the GP Timer counting mode to stop the counting.
 *
 *
 *   @b Modifies
 *   @n Gptimer TCR of timer High
 *
 *   @b Example
 *   @verbatim
        CSL_TmrHandle hTmr;
        ...
        CSL_TmrStopHi(hWdt);
        ...
     @endverbatim
 * ===========================================================================
 */
static inline
void CSL_TmrStopHi (
    CSL_TmrHandle hTmr
)
{
    CSL_FINST(hTmr->regs->TCR, TMR_TCR_ENAMODE_HI, DISABLE);
}



/** ===========================================================================
 *   @n@b CSL_TmrStop64
 *
 *   @b Description
 *      sets the timer counting mode to stop the timer.
 *	    The timer maintains the current value
 *
 *   @b Arguments
 *   @verbatim
         hTmr        Handle to the Gptimer instance

     @endverbatim
 *
 *   <b> Return Value </b>
 *       None
 *
 *   <b> Pre Condition </b>
 *   @n  Timer should be set it to GPtimer or Chained mode
 *
 *   <b> Post Condition </b>
 *   @n   Set the GP Timer counting mode to stop the counting.
 *
 *
 *   @b Modifies
 *   @n Gptimer TCR of timer Low and High
 *
 *   @b Example
 *   @verbatim
        CSL_TmrHandle hTmr;
        ...
        CSL_TmrStop64(hWdt);
        ...
     @endverbatim
 * ===========================================================================
 */
static inline
void CSL_TmrStop64 (
    CSL_TmrHandle hTmr
)
{
    CSL_FINST(hTmr->regs->TCR, TMR_TCR_ENAMODE_LO, DISABLE);
    CSL_FINST(hTmr->regs->TCR, TMR_TCR_ENAMODE_HI, DISABLE);
}



/** ===========================================================================
 *   @n@b CSL_TmrResetHi
 *
 *   @b Description
 *      TSTAT_HI is set to reset state
 *
 *   @b Arguments
 *   @verbatim
         hTmr        Handle to the Gptimer instance
     @endverbatim
 *
 *   <b> Return Value </b>
 *       None
 *
 *   <b> Pre Condition </b>
 *   @n  Timer should be set it to any unchained mode
 *
 *   <b> Post Condition </b>
 *   @n   TTSTAT_HI is set to reset state
 *
 *
 *   @b Modifies
 *   @n Gptimer TCR of timer High
 *
 *   @b Example
 *   @verbatim
        CSL_TmrHandle hTmr;
        ...
        CSL_TmrResetHi(hWdt);
        ...
     @endverbatim
 * ===========================================================================
 */
static inline
void CSL_TmrResetHi (
    CSL_TmrHandle hTmr
)
{
    CSL_FINST(hTmr->regs->TGCR, TMR_TGCR_TIMHIRS, RESET_ON);
}



/** ===========================================================================
 *   @n@b CSL_TmrResetLo
 *
 *   @b Description
 *      TSTAT_LO is set to reset state
 *
 *   @b Arguments
 *   @verbatim
         hTmr        Handle to the Gptimer instance
     @endverbatim
 *
 *   <b> Return Value </b>
 *       None
 *
 *   <b> Pre Condition </b>
 *   @n  Timer should be set it to any unchained mode
 *
 *   <b> Post Condition </b>
 *   @n   TSTAT_LO is set to reset state
 *
 *
 *   @b Modifies
 *   @n Gptimer TCR of timer Low
 *
 *   @b Example
 *   @verbatim
        CSL_TmrHandle hTmr;
        ...
        CSL_TmrResetLo(hWdt);
        ...
     @endverbatim
 * ===========================================================================
 */
static inline
void CSL_TmrResetLo (
    CSL_TmrHandle hTmr
)
{
    CSL_FINST(hTmr->regs->TGCR, TMR_TGCR_TIMLORS, RESET_ON);
}



/** ===========================================================================
 *   @n@b CSL_TmrReset64
 *
 *   @b Description
 *      TSTAT_LO and TSTAT_HI is set to reset state.
 *
 *   @b Arguments
 *   @verbatim
         hTmr        Handle to the Gptimer instance
     @endverbatim
 *
 *   <b> Return Value </b>
 *       None
 *
 *   <b> Pre Condition </b>
 *   @n  Timer should be set it to GPtimer or chained mode
 *
 *   <b> Post Condition </b>
 *   @n   TSTAT_LO and TSTAT_HI is set to reset state
 *
 *
 *   @b Modifies
 *   @n Gptimer TCR of timer Low and High
 *
 *   @b Example
 *   @verbatim
        CSL_TmrHandle hTmr;
        ...
        CSL_TmrReset64(hWdt);
        ...
     @endverbatim
 * ===========================================================================
 */
static inline
void CSL_TmrReset64 (
	CSL_TmrHandle hTmr
)
{
    CSL_FINST(hTmr->regs->TGCR, TMR_TGCR_TIMLORS, RESET_ON);
    CSL_FINST(hTmr->regs->TGCR, TMR_TGCR_TIMHIRS, RESET_ON);
}



/** ===========================================================================
 *   @n@b CSL_TmrStartWdt
 *
 *   @b Description
 *      sets the timer in watchdog  mode and counting mode for it
 *
 *   @b Arguments
 *   @verbatim
         hTmr        Handle to the Gptimer instance

        countMode    specifies the timer counting mode
     @endverbatim
 *
 *   <b> Return Value </b>
 *       None
 *
 *   <b> Pre Condition </b>
 *   @n  Timer should be set it to any GPtimer mode
 *
 *   <b> Post Condition </b>
 *   @n   Set the watchdog timer counting mode and
 		  timer Low and High reset register and WDEN bit and WDKEY.
 *
 *
 *   @b Modifies
 *   @n Gptimer TCR and TGCR register of timer Low AND High and WDTCR rgister
 *
 *   @b Example
 *   @verbatim
        CSL_TmrHandle  hTmr;
        CSL_TmrEnamode countMode;
        ...
        CSL_TmrStartWdt(hWdt, countMode);
        ...
     @endverbatim
 * ===========================================================================
 */
static inline
void CSL_TmrStartWdt (
    CSL_TmrHandle  hTmr,
    CSL_TmrEnamode countMode
)
{
    CSL_FINST(hTmr->regs->TGCR, TMR_TGCR_TIMLORS, RESET_OFF);
    CSL_FINST(hTmr->regs->TGCR, TMR_TGCR_TIMHIRS, RESET_OFF);
    CSL_FINST(hTmr->regs->WDTCR, TMR_WDTCR_WDEN, ENABLE);
    CSL_FINS(hTmr->regs->TCR, TMR_TCR_ENAMODE_LO, countMode);
    CSL_FINS(hTmr->regs->TCR, TMR_TCR_ENAMODE_HI, countMode); 
}



/** ===========================================================================
 *   @n@b CSL_TmrLoadtWdkey
 *
 *   @b Description
 *      sets the watchdog key
 *
 *   @b Arguments
 *   @verbatim
         hTmr        Handle to the Gptimer instance

         cmd         specifies the key 
     @endverbatim
 *
 *   <b> Return Value </b>
 *       None
 *
 *   <b> Pre Condition </b>
 *   @n  Timer should be set it to any GPtimer mode
 *
 *   <b> Post Condition </b>
 *   @n   Loads the  WDKEY in WDTCR register.
 *
 *
 *   @b Modifies
 *   @n WDTCR rgister
 *
 *   @b Example
 *   @verbatim
        CSL_TmrHandle hTmr;
        Uint16        cmd;	
        ...
        CSL_TmrStartWdt(hTmr, cmd);
        ...
     @endverbatim
 * ===========================================================================
 */
static inline
void CSL_TmrLoadWdkey (
    CSL_TmrHandle hTmr,
    Uint16        cmd
)
{
    CSL_FINS(hTmr->regs->WDTCR, TMR_WDTCR_WDKEY, cmd);
}



/** ===========================================================================
 *   @n@b CSL_TmrDisableWdt
 *
 *   @b Description
 *      disables  the timer in watchdog  mode and counting mode for it
 *
 *   @b Arguments
 *   @verbatim
        hTmr         Handle to the Gptimer instance
     @endverbatim
 *
 *   <b> Return Value </b>
 *       None
 *
 *   <b> Pre Condition </b>
 *   @n  Timer should be set it to any GPtimer mode
 *
 *   <b> Post Condition </b>
 *   @n   Set the watchdog timer counting mode and
 		  timer Low and High reset register and WDEN bit and WDKEY.
 *
 *
 *   @b Modifies
 *   @n Gptimer TCR and TGCR register of timer Low AND High and WDTCR rgister
 *
 *   @b Example
 *   @verbatim
        CSL_TmrHandle hTmr;
        ...
        CSL_TmrStartWdt(hTmr);
        ...
     @endverbatim
 * ===========================================================================
 */
static inline
void CSL_TmrDisableWdt (
    CSL_TmrHandle hTmr
)
{
    CSL_FINST(hTmr->regs->WDTCR, TMR_WDTCR_WDEN, DISABLE);
}

#ifdef __cplusplus
extern "C" {
#endif

#endif /* CSL_TMRAUX_H_ */

