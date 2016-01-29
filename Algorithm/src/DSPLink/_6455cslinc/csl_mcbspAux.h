/*  ============================================================================
 *   Copyright (c) Texas Instruments Inc 2002, 2003, 2004, 2005
 *
 *   Use of this software is controlled by the terms and conditions found in the
 *   license agreement under which this software has been supplied.
 *   ===========================================================================
 */
 
/*  ============================================================================
 *   @file  csl_mcbspAux.h
 *
 *   @path  $(CSLPATH)\inc
 *
 *   @desc  Header file for functional layer of CSL_mcbspAux
 *
 */

/* ============================================================================
 *  Revision History
 *  ===============
 *  15-Feb-2005 NSR File Created from CSL_mcbspHwControl.c 
 *                                And CSL_mcbspGetHwStatus.c.
 *  17-May-2005 RMathew  - Removed CSL_mcbspIdleControl() and added inlines
 *                         for Transmit and receive interrupt mode commands and 
 *                         queries
 *  27-Oct-2005 ds       - Removed CSL_mcbspGetPid ()
 *
 *  01-Feb-2006 ds       - Removed CSL_mcbspGetTxIntMode (), 
 *                         CSL_mcbspGetRxIntMode (), CSL_mcbspTxIntMode () and
 *                         CSL_mcbspRxIntMode () APIs
 * ============================================================================
 */

#ifndef _CSL_MCBSPAUX_H_
#define _CSL_MCBSPAUX_H_

#include<csl_mcbsp.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  Status Query Functions of mcbsp.
 */

/* ============================================================================
 *   @n@b CSL_mcbspGetCurRxBlk
 *
 *   @b Description
 *   @n This function gets the current Receive Block. 
 *
 *   @b Arguments
 *   @verbatim

            hMcbsp          Handle to MCBSP instance

            response        Placeholder to return status.

     @endverbatim
 *
 *   <b> Return Value </b>  None
 *
 *   <b> Pre Condition </b>
 *   @n  Mcbsp must be initialized and opened properly
 *
 *   <b> Post Condition </b>
 *    @n None
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim

        CSL_mcbspGetCurRxBlk (hMcbsp, response);

     @endverbatim
 * ===========================================================================
 */
CSL_IDEF_INLINE void CSL_mcbspGetCurRxBlk (
    CSL_McbspHandle        hMcbsp,
    void                   *response
)
{
    *(CSL_McbspBlock *)response = 
        (CSL_McbspBlock) (CSL_FEXT(hMcbsp->regs->MCR, MCBSP_MCR_RCBLK));
}


/*  ============================================================================
 *   @n@b CSL_mcbspGetCurTxBlk
 *
 *   @b Description
 *   @n This function gets the current Transmit block. 
 *
 *   @b Arguments
 *   @verbatim

            hMcbsp          Handle to MCBSP instance

            response        Placeholder to return status.

     @endverbatim
 *
 *   <b> Return Value </b>  None
 *
 *   <b> Pre Condition </b>
 *   @n  Mcbsp must be initialized and opened properly 
 *
 *   <b> Post Condition </b>
 *    @n None
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim

        CSL_mcbspGetCurTxBlk (hMcbsp, response);

     @endverbatim
 * ===========================================================================
 */
CSL_IDEF_INLINE void CSL_mcbspGetCurTxBlk (
    CSL_McbspHandle        hMcbsp,
    void                   *response
)
{
    *(CSL_McbspBlock *)response = 
        (CSL_McbspBlock)(CSL_FEXT(hMcbsp->regs->MCR, MCBSP_MCR_XCBLK));
}


/*  ============================================================================
 *   @n@b CSL_mcbspGetDevStatus
 *
 *   @b Description
 *   @n This function gets the transmit and receive status conditions.
 *
 *   @b Arguments
 *   @verbatim

            hMcbsp          Handle to MCBSP instance

            response        Placeholder to return status.

     @endverbatim
 *
 *   <b> Return Value </b>  None
 *
 *   <b> Pre Condition </b>
 *   @n  Mcbsp must be initialized and opened properly 
 *
 *   <b> Post Condition </b>
 *    @n None
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim

        CSL_mcbspGetDevStatus (hMcbsp, response);

     @endverbatim
 * ===========================================================================
 */
CSL_IDEF_INLINE void CSL_mcbspGetDevStatus (
    CSL_McbspHandle        hMcbsp,
    void                   *response
)
{
    *(CSL_BitMask16*)response = 
        (CSL_BitMask32) ((CSL_FEXT(hMcbsp->regs->SPCR, MCBSP_SPCR_RRDY)) 
                | (CSL_FEXT(hMcbsp->regs->SPCR, MCBSP_SPCR_XRDY) << (1))
                | (CSL_FEXT(hMcbsp->regs->SPCR, MCBSP_SPCR_RFULL) << (2))
                | (CSL_FEXT(hMcbsp->regs->SPCR, MCBSP_SPCR_XEMPTY) << (3)) 
                | (CSL_FEXT(hMcbsp->regs->SPCR, MCBSP_SPCR_RSYNCERR) << (4)) 
                | (CSL_FEXT(hMcbsp->regs->SPCR, MCBSP_SPCR_XSYNCERR) << (5)));
}


/*  ============================================================================
 *   @n@b CSL_mcbspGetTxRstStat
 *
 *   @b Description
 *   @n This function gets the transmit reset state.
 *
 *   @b Arguments
 *   @verbatim

            hMcbsp          Handle to MCBSP instance

            response        Placeholder to return status.

     @endverbatim
 *
 *   <b> Return Value </b>  None
 *
 *   <b> Pre Condition </b>
 *   @n  Mcbsp must be initialized and opened properly 
 *
 *   <b> Post Condition </b>
 *    @n None
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim

        CSL_mcbspGetTxRstStat (hMcbsp, response);

     @endverbatim
 * ===========================================================================
 */
CSL_IDEF_INLINE void CSL_mcbspGetTxRstStat (
    CSL_McbspHandle        hMcbsp,
    void                   *response
)
{
    *(CSL_McbspRstStat *)response = 
        (CSL_McbspRstStat)(CSL_FEXT(hMcbsp->regs->SPCR, MCBSP_SPCR_XRST));
}


/*  ============================================================================
 *   @n@b CSL_mcbspGetRxRstStat
 *
 *   @b Description
 *   @n This function gets the receive reset state.
 *
 *   @b Arguments
 *   @verbatim

            hMcbsp          Handle to MCBSP instance

            response        Placeholder to return status.

     @endverbatim
 *
 *   <b> Return Value </b>  None
 *
 *   <b> Pre Condition </b>
 *   @n  Mcbsp must be initialized and opened properly 
 *
 *   <b> Post Condition </b>
 *    @n None
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim

       CSL_mcbspGetRxRstStat (hMcbsp, response);

     @endverbatim
 * ===========================================================================
 */
CSL_IDEF_INLINE void CSL_mcbspGetRxRstStat (
    CSL_McbspHandle        hMcbsp,
    void                   *response
)
{
    *(CSL_McbspRstStat *)response = 
        (CSL_McbspRstStat) (CSL_FEXT(hMcbsp->regs->SPCR, MCBSP_SPCR_RRST));
}

#ifdef __cplusplus
}
#endif

#endif /* CSL_MCBSPAUX_H_ */


