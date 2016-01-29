/*  ============================================================================
 *   Copyright (c) Texas Instruments Inc 2002, 2003, 2004, 2005
 *
 *   Use of this software is controlled by the terms and conditions found
 *   in the license agreement under which this software has been supplied.
 *   ===========================================================================
 */

/** 
 *  @file    csl_hpiAux.h
 * 
 *  @path    $(CSLPATH)\inc
 *
 *  @brief   HPI CSL implementation on DSP side. 
 *
 */
 
/* =============================================================================
 *  Revision History
 *  ===============
 *  06-Feb-2006 ds Updated according to TCI6482/C6455 User Guide
 *
 * =============================================================================
 */

#ifndef _CSL_HPIAUX_H_
#define _CSL_HPIAUX_H_

#include <csl_hpi.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Hardware Control functions of the hpi */

/** ============================================================================
 *   @n@b CSL_hpiSetDspInt
 *
 *   @b Description
 *   @n Gets the HPIC Host-to-DSP Interrupt status of the hpi.
 *
 *   @b Arguments
 *   @verbatim
            hHpi           Handle to the HPI instance
     @endverbatim
 *
 *   <b> Return Value </b>
 *	 @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n Sets the DSP_INT bit of HPIC register
 *
 *   @b Example
 *   @verbatim
        CSL_HpiHandle          hHpi;

        CSL_hpiSetDspInt(hHpi);

     @endverbatim
 * =============================================================================
 */
static inline
void CSL_hpiSetDspInt (
    CSL_HpiHandle    hHpi
)
{
    CSL_FINS(hHpi->regs->HPIC, HPI_HPIC_DSPINT, TRUE); 
}   
    
/** ============================================================================
 *   @n@b CSL_hpiResetDspInt
 *
 *   @b Description
 *   @n Gets the Reset HPIC Host-to-DSP Interrupt status of the hpi.
 *
 *   @b Arguments
 *   @verbatim
            hHpi           Handle to the HPI instance
     @endverbatim
 *
 *   <b> Return Value </b>
 *	 @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n Resets the DSP_INT bit of HPIC register
 *
 *   @b Example
 *   @verbatim
        CSL_HpiHandle          hHpi;

        CSL_hpiResetDspInt(hHpi);

     @endverbatim
 * =============================================================================
 */
static inline
void CSL_hpiResetDspInt (
    CSL_HpiHandle    hHpi
)
{
    CSL_FINS(hHpi->regs->HPIC, HPI_HPIC_DSPINT, FALSE);
}       

/** ============================================================================
 *   @n@b CSL_hpiSetHint
 *
 *   @b Description
 *   @n Gets the HPIC DSP-to-Host Interrupt status of the hpi.
 *
 *   @b Arguments
 *   @verbatim
            hHpi           Handle to the HPI instance
     @endverbatim
 *
 *   <b> Return Value </b>
 *	 @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n Sets the HINT bit of HPIC register
 *
 *   @b Example
 *   @verbatim
        CSL_HpiHandle          hHpi;

        CSL_hpiSetHint(hHpi);

     @endverbatim
 * =============================================================================
 */
static inline
void CSL_hpiSetHint (
    CSL_HpiHandle    hHpi
)
{
    CSL_FINS(hHpi->regs->HPIC, HPI_HPIC_HINT, TRUE);    
}       

/** ============================================================================
 *   @n@b CSL_hpiResetHint
 *
 *   @b Description
 *   @n Gets the Reset HPIC DSP-to-Host Interrupt status of the hpi.
 *
 *   @b Arguments
 *   @verbatim
            hHpi           Handle to the HPI instance
     @endverbatim
 *
 *   <b> Return Value </b>
 *	 @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n Resets the HINT bit of HPIC register
 *
 *   @b Example
 *   @verbatim
        CSL_HpiHandle          hHpi;

        CSL_hpiSetHint(hHpi);

     @endverbatim
 * =============================================================================
 */
static inline
void CSL_hpiResetHint (
    CSL_HpiHandle    hHpi
)
{
    CSL_FINS(hHpi->regs->HPIC, HPI_HPIC_HINT, FALSE);   
}       

/** ===========================================================================
 *   @n@b CSL_hpiGetHpicHrdy
 *
 *   @b Description
 *   @n This function  gets HRDY bit of the HPIC.
 *
 *   @b Arguments
 *   @verbatim

            hHpi           Handle to HPI instance

            response        Placeholder to return status.

     @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *    @n None
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim

        CSL_hpiGetHpicHrdy(hHpi, &response);

     @endverbatim
 * ===========================================================================
 */
static inline
void CSL_hpiGetHpicHrdy (
    CSL_HpiHandle    hHpi,
    void              *response
)
{
    *((Uint32 *)response) = CSL_FEXT(hHpi->regs->HPIC, HPI_HPIC_HRDY);
}               
        
/** ===========================================================================
 *   @n@b CSL_hpiGetHpicFetch
 *
 *   @b Description
 *   @n This function  gets FETCH bit of the HPIC Register.
 *
 *   @b Arguments
 *   @verbatim

            hHpi           Handle to HPI instance

            response        Placeholder to return status.

     @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *    @n None
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim

        CSL_hpiGetHpicFetch(hHpi, &response);

     @endverbatim
 * ===========================================================================
 */
static inline
void CSL_hpiGetHpicFetch (
    CSL_HpiHandle    hHpi,
    void              *response
)
{
    *((Uint32 *)response) = CSL_FEXT(hHpi->regs->HPIC, HPI_HPIC_FETCH);
}               

/** ===========================================================================
 *   @n@b CSL_hpiGetHpicHpiRst
 *
 *   @b Description
 *   @n This function  gets HPI_RST bit of the HPIC Register.
 *
 *   @b Arguments
 *   @verbatim

            hHpi           Handle to HPI instance

            response        Placeholder to return status.

     @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *    @n None
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim

        CSL_hpiGetHpicHpiRst(hHpi, &response);

     @endverbatim
 * ===========================================================================
 */
static inline
void CSL_hpiGetHpicHpiRst (
    CSL_HpiHandle    hHpi,
    void              *response
)
{
    *((Uint32 *)response) = CSL_FEXT(hHpi->regs->HPIC, HPI_HPIC_HPIRST);
}

/** ===========================================================================
 *   @n@b CSL_hpiGetHpicHwobStat
 *
 *   @b Description
 *   @n This function  gets HWOB_STAT bit of the HPIC Register.
 *
 *   @b Arguments
 *   @verbatim

            hHpi           Handle to HPI instance

            response        Placeholder to return status.

     @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *    @n None
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim

        CSL_hpiGetHpicHwobStat(hHpi, &response);

     @endverbatim
 * ===========================================================================
 */
static inline
void CSL_hpiGetHpicHwobStat (
    CSL_HpiHandle    hHpi,
    void              *response
)
{
    *((Uint32 *)response) = CSL_FEXT(hHpi->regs->HPIC, HPI_HPIC_HWOBSTAT);
}


#ifdef __cplusplus
}
#endif

#endif /* _CSL_HPIAUX_H_ */

