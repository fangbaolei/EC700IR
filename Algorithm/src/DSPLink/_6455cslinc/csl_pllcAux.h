/*  ============================================================================
 *   Copyright (c) Texas Instruments Inc 2002, 2003, 2004, 2005
 *
 *   Use of this software is controlled by the terms and conditions found in the
 *   license agreement under which this software has been supplied.
 *  ============================================================================
 */

/** @file csl_pllcAux.h
 *
 *  @brief API Auxilary header file for PLLC CSL.
 *
 *  Path: \(CSLPATH)\inc
 */

/*  ============================================================================
 *  Revision History
 *  ===============
 *  10-Feb-2004 kpn  File Created. 
 *  25-Aug-2005 Tej File Modified.
 *  27-oct-2005 sd  changes for multiplier configuration
 *  18-Jan-2006 sd  Changes according to spec changes
 *  ============================================================================
 */

#ifndef _CSL_PPLCAUX_H_
#define _CSL_PLLCAUX_H_

#include <csl_pllc.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  Status command functions of the pllc
 */

/** ============================================================================
 *   @n@b CSL_pllcGetStatus
 *
 *   @b Description
 *   @n Gets the Status of the pll controller.
 *
 *   @b Arguments
 *   @verbatim
            hPllc           Handle to the PLLC instance
     @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  CSL_BitMask32
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
        CSL_PllcHandle    hPllc;
        CSL_BitMask32     response;
        ...

        response = CSL_pllcGetStatus (hPllc);
     @endverbatim
 *  ============================================================================
 */
CSL_IDEF_INLINE
CSL_BitMask32 CSL_pllcGetStatus (
    CSL_PllcHandle                hPllc
)
{
    CSL_BitMask32 response;
    response = (CSL_BitMask32) hPllc->regs->PLLSTAT;
    return response;
}

/*
 *  Status command functions of the pllc
 */
 
/** ============================================================================
 *   @n@b CSL_pllcGetSysClkStatus
 *
 *   @b Description
 *   @n Gets the System Clock ON/OFF Status of the pllc.
 *
 *   @b Arguments
 *   @verbatim
            hPllc           Handle to the PLLC instance
     @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  CSL_BitMask32
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
        CSL_PllcHandle    hPllc;
        CSL_BitMask32     response;
        ...

        response = CSL_pllcGetSysClkStatus (hPllc);
     @endverbatim
 *  ============================================================================
 */
CSL_IDEF_INLINE
CSL_BitMask32 CSL_pllcGetSysClkStatus (
    CSL_PllcHandle                      hPllc
)
{
    CSL_BitMask32 response;
    response = (CSL_BitMask32) hPllc->regs->SYSTAT;
    return response;
}

/*
 *  Status command functions of the pllc
 */
 
/** ============================================================================
 *   @n@b CSL_pllcGetResetStatus
 *
 *   @b Description
 *   @n Gets the Reset Type Status of the pllc.
 *
 *   @b Arguments
 *   @verbatim
            hPllc           Handle to the PLLC instance
     @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  CSL_BitMask32
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
        CSL_PllcHandle    hPllc;
        CSL_BitMask32     response;
        ...

        response = CSL_pllcGetResetStatus (hPllc);
     @endverbatim
 *  ============================================================================
 */
CSL_IDEF_INLINE
CSL_BitMask32 CSL_pllcGetResetStatus (
    CSL_PllcHandle         hPllc
)
{
    CSL_BitMask32 response;
   	response = (Uint32) hPllc->regs->RSTYPE;
    return response;
}

/*
 *  Control command functions of the pllc
 */

/** ============================================================================
 *   @n@b CSL_pllcCommandCtrl
 *
 *   @b Description
 *      Controls the pllc operation.
 *
 *   @b Arguments
 *   @verbatim
        hPllc        Handle to the PLLC instance
        loadVal      Value to be loaded to pllc PLLCMD register
		status 		 Status variable
     @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  pllc operation is controlled according to value set.
 *
 *   @b Modifies
 *   @n pllc PLLCMD register.
 *
 *   @b Example
 *   @verbatim
        CSL_PllcHandle      hPllc;
        CSL_BitMask32       loadVal;
	    CSL_Status          status
        ...

        CSL_pllcCommandCtrl (hPllc, loadVal, &status);
     @endverbatim
 *  ============================================================================
 */
CSL_IDEF_INLINE
void CSL_pllcCommandCtrl (
    CSL_PllcHandle         hPllc,
    CSL_BitMask32          loadVal,
    CSL_Status             *status
)
{
    if (hPllc->pllcNum == CSL_PLLC_1) {
	    hPllc->regs->PLLCTL = (loadVal & 0xFFFF);
	    loadVal = (loadVal & 0xFFFF0000)>>16;
	    if (loadVal)
	        hPllc->regs->PLLCMD = loadVal;
	}
    else
        *status = CSL_ESYS_INVPARAMS;
}

/** ============================================================================
 *   @n@b CSL_pllcMultiplierCtrl
 *
 *   @b Description
 *      Controls the pllc Multiplier.
 *
 *   @b Arguments
 *   @verbatim
        hPllc        Handle to the PLLC instance
        loadVal      Value to be loaded to pllc PLLM register
		status 	     Status variable		
     @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  pllc multiplier is controlled accordingly.
 *
 *   @b Modifies
 *   @n pllc PLLM register.
 *
 *   @b Example
 *   @verbatim
        CSL_PllcHandle      hPllc;
        Uint32              loadVal;
	    CSL_Status          status
        ...

        CSL_pllcMultiplierCtrl (hPllc, loadVal, &status);
     @endverbatim
 *  ============================================================================
 */
CSL_IDEF_INLINE
void CSL_pllcMultiplierCtrl (
    CSL_PllcHandle            hPllc,
    Uint32                    loadVal,
    CSL_Status                *status
)
{
    if (hPllc->pllcNum == CSL_PLLC_1) 
	    CSL_FINS (hPllc->regs->PLLM, PLLC_PLLM_PLLM, loadVal - 1);
    else
        *status = CSL_ESYS_INVPARAMS;
}

/** ============================================================================
 *   @n@b CSL_pllcSetPLLDivRatio
 *
 *   @b Description
 *      Sets the pllc Dividers ratios.
 *
 *   @b Arguments
 *   @verbatim
        hPllc        Handle to the PLLC instance
        loadVal      Value to be loaded to pllc divider registers
        status       Status variable
     @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  pllc dividers ratios are set.
 *
 *   @b Modifies
 *   @n pllc divider registers.
 *
 *   @b Example
 *   @verbatim
        CSL_PllcHandle      hPllc;
        CSL_PllcDivRatio    loadVal;
        CSL_Status          status;
        ...

        CSL_pllcSetPLLDivRatio (hPllc, loadVal, &status);
     @endverbatim
 *  ============================================================================
 */
CSL_IDEF_INLINE
void CSL_pllcSetPLLDivRatio (
    CSL_PllcHandle             hPllc,
    CSL_PllcDivRatio           loadVal,
    CSL_Status                *status
)
{
    switch (loadVal.divNum) {
        case CSL_PLLC_DIVSEL_PLLDIV1:
            if (hPllc->pllcNum == CSL_PLLC_2) {
                CSL_FINS (hPllc->regs->PLLDIV1, PLLC_PLLDIV1_RATIO,
                          (Uint32)loadVal.divRatio - 1);
            }
            else
                *status = CSL_ESYS_INVPARAMS;
            break;
        case CSL_PLLC_DIVSEL_PLLDIV4:
            if (hPllc->pllcNum == CSL_PLLC_1) {
                CSL_FINS (hPllc->regs->PLLDIV4, PLLC_PLLDIV4_RATIO,
                          (Uint32)loadVal.divRatio - 1);
            }
            else
                *status = CSL_ESYS_INVPARAMS;
            break;
        case CSL_PLLC_DIVSEL_PLLDIV5:
            if (hPllc->pllcNum == CSL_PLLC_1) {
                CSL_FINS (hPllc->regs->PLLDIV5, PLLC_PLLDIV5_RATIO,
                          (Uint32)loadVal.divRatio - 1);
            }
            else
                *status = CSL_ESYS_INVPARAMS;
            break;
        default:
            *status = CSL_ESYS_INVPARAMS;
            break;
    }
}

/** ============================================================================
 *   @n@b CSL_pllcPLLDivCtrl
 *
 *   @b Description
 *      Controls the pllc dividers.
 *
 *   @b Arguments
 *   @verbatim
        hPllc        Handle to the PLLC instance
        loadVal      Value to be loaded to pllc dividers register.
        status       Status variable
     @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  pllc dividers are controlled.
 *
 *   @b Modifies
 *   @n pllc dividers register.
 *
 *   @b Example
 *   @verbatim
        CSL_PllcHandle              hPllc;
        CSL_PllcDivideControl       loadVal;
        CSL_Status                  status;
        ...

        CSL_pllcPLLDivCtrl (hPllc, loadVal, &status);
     @endverbatim
 *  ============================================================================
 */
CSL_IDEF_INLINE
void CSL_pllcPLLDivCtrl (
    CSL_PllcHandle         hPllc,
    CSL_PllcDivideControl  loadVal,
    CSL_Status            *status
)
{
    switch (loadVal.divNum) {
        case CSL_PLLC_DIVSEL_PLLDIV1:
            if (hPllc->pllcNum == CSL_PLLC_2) {
                CSL_FINS (hPllc->regs->PLLDIV1, PLLC_PLLDIV1_D1EN,
                          (CSL_PllcDivCtrl)loadVal.divCtrl);
            }
            else
                *status = CSL_ESYS_INVPARAMS;
            break;
        case CSL_PLLC_DIVSEL_PLLDIV4:
            if (hPllc->pllcNum == CSL_PLLC_1) {
                CSL_FINS (hPllc->regs->PLLDIV4, PLLC_PLLDIV4_D4EN,
                          (CSL_PllcDivCtrl)loadVal.divCtrl);
            }
            else
                *status = CSL_ESYS_INVPARAMS;
            break;
        case CSL_PLLC_DIVSEL_PLLDIV5:
            if (hPllc->pllcNum == CSL_PLLC_1) {
                CSL_FINS (hPllc->regs->PLLDIV5, PLLC_PLLDIV5_D5EN,
                          (CSL_PllcDivCtrl)loadVal.divCtrl);
            }
            else
                *status = CSL_ESYS_INVPARAMS;
            break;
        default:
            *status = CSL_ESYS_INVPARAMS;
            break;
    }
}

#ifdef __cplusplus
}
#endif

#endif /* _CSL_PLLCAUX_H_ */

