/** ===========================================================================
 *   @file  csl_mcaspAux.h
 *
 *   @path  $(CSLPATH)\mcasp\src
 *
 *   @desc  API auxilary header file for McASP CSL.
 *
 */

/*  ===========================================================================
 *   Copyright (c) Texas Instruments Inc 2002, 2003, 2004, 2005
 *
 *   Use of this software is controlled by the terms and conditions found in
 *   the license agreement under which this software has been supplied.
 *   ==========================================================================
 */

/*  @(#) PSP/CSL 3.00.01.00[5905] (2005-01-12)  */

/* ============================================================================
 *  Revision History
 *  ===============
 *  12-Jan-2005 asr File Created.
 * ============================================================================
 */

#ifndef _CSL_MCASPAUX_H_
#define _CSL_MCASPAUX_H_

#include <davinci_hd.h>
#include <csl_mcasp.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  Status Query Functions of McASP
 */

/** ===========================================================================
 *   @n@b CSL_mcaspGetCurrentXSlot
 *
 *   @b Description
 *   @n This function returns the current transmit time slot count
 *
 *   @b Arguments
 *   @verbatim
            hMcasp            Handle to the McASP instance
     @endverbatim
 *
 *   <b> Return Value </b>  Uint16
 *   @li                    Current transmit time slot count
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
        Uint16  currentXslot;
        
        currentXslot = CSL_mcaspGetCurrentXSlot (hMcasp);
        
     @endverbatim
 * ============================================================================
 */
static inline
Uint16 CSL_mcaspGetCurrentXSlot (
    CSL_McaspHandle    hMcasp    
)
{
	return((Uint16)CSL_FEXT(hMcasp->regs->XSLOT, MCASP_XSLOT_XSLOTCNT));
}

/** ===========================================================================
 *   @n@b CSL_mcaspGetCurrentRSlot
 *
 *   @b Description
 *   @n This function returns the current receive time slot count
 *
 *   @b Arguments
 *   @verbatim
            hMcasp            Handle to the McASP instance
     @endverbatim
 *
 *   <b> Return Value </b>  Uint16
 *   @li                    Current receive time slot count
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
        Uint16  currentRslot;
        
        currentRslot = CSL_mcaspGetCurrentRSlot (hMcasp);
        
     @endverbatim
 * ============================================================================
 */
static inline
Uint16 CSL_mcaspGetCurrentRSlot (
    CSL_McaspHandle    hMcasp    
)
{
	return((Uint16)CSL_FEXT(hMcasp->regs->RSLOT, MCASP_RSLOT_RSLOTCNT));
}

/** ===========================================================================
 *   @n@b CSL_mcaspGetXmtErr
 *
 *   @b Description
 *   @n This function checks whether transmitter error interrupt has occurred
 *      or not
 *
 *   @b Arguments
 *   @verbatim
            hMcasp            Handle to the McASP instance
     @endverbatim
 *
 *   <b> Return Value </b>  Bool
 *   @li                    TRUE  - Transmitter error interrupt has occurred
 *   
 *   @li                    FALSE - Transmitter error interrupt has not 
 *                                  occurred
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim
        Bool    xmtErr;
        
        xmtErr = CSL_mcaspGetXmtErr (hMcasp);
        
     @endverbatim
 * ============================================================================
 */
static inline
    Bool CSL_mcaspGetXmtErr (
    CSL_McaspHandle    hMcasp    
)
{
	return((Bool)CSL_FEXT(hMcasp->regs->XSTAT, MCASP_XSTAT_XERR));
}

/** ===========================================================================
 *   @n@b CSL_mcaspGetXmtClkFail
 *
 *   @b Description
 *   @n This function checks whether transmit clock failure flag is set or not
 *
 *   @b Arguments
 *   @verbatim
            hMcasp            Handle to the McASP instance
     @endverbatim
 *
 *   <b> Return Value </b>  Bool
 *   @li                    TRUE  - Transmit clock failure flag is set
 *   
 *   @li                    FALSE - Transmit clock failure flag is not set
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim
        Bool    xmtClkFail;
        
        xmtClkFail = CSL_mcaspGetXmtClkFail (hMcasp);
        
     @endverbatim
 * ============================================================================
 */
static inline
Bool CSL_mcaspGetXmtClkFail (
    CSL_McaspHandle    hMcasp    
)
{
	return((Bool)CSL_FEXT(hMcasp->regs->XSTAT, MCASP_XSTAT_XCKFAIL));
}

/** ===========================================================================
 *   @n@b CSL_mcaspGetXmtSyncErr
 *
 *   @b Description
 *   @n This function checks whether unexpected transmit frame sync flag is set
 *      or not
 *
 *   @b Arguments
 *   @verbatim
            hMcasp            Handle to the McASP instance
     @endverbatim
 *
 *   <b> Return Value </b>  Bool
 *   @li                    TRUE  - Unexpected transmit frame sync error has
 *                                  occurred
 *   
 *   @li                    FALSE - Unexpected transmit frame sync error has
 *                                  not occurred
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim
        Bool    xmtSyncErr;
        
        xmtSyncErr = CSL_mcaspGetXmtSyncErr (hMcasp);
        
     @endverbatim
 * ============================================================================
 */
static inline
Bool CSL_mcaspGetXmtSyncErr (
    CSL_McaspHandle    hMcasp    
)
{
	return((Bool)CSL_FEXT(hMcasp->regs->XSTAT, MCASP_XSTAT_XSYNCERR));
}

/** ===========================================================================
 *   @n@b CSL_mcaspGetXmtUnderrun
 *
 *   @b Description
 *   @n This function checks whether transmitter underrun flag is set or not
 *
 *   @b Arguments
 *   @verbatim
            hMcasp            Handle to the McASP instance
     @endverbatim
 *
 *   <b> Return Value </b>  Bool
 *   @li                    TRUE  - Transmitter underrun flag is set 
 *   
 *   @li                    FALSE - Transmitter underrun flag is not set
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim
        Bool    xmtUnderRun;
        
        xmtUnderRun = CSL_mcaspGetXmtUnderrun (hMcasp);
        
     @endverbatim
 * ============================================================================
 */
static inline
Bool CSL_mcaspGetXmtUnderrun (
    CSL_McaspHandle    hMcasp    
)
{
	return((Bool)CSL_FEXT(hMcasp->regs->XSTAT, MCASP_XSTAT_XUNDRN));
}

/** ===========================================================================
 *   @n@b CSL_mcaspGetXmtDataReady
 *
 *   @b Description
 *   @n This function checks whether transmit data ready flag is set or not
 *
 *   @b Arguments
 *   @verbatim
            hMcasp            Handle to the McASP instance
     @endverbatim
 *
 *   <b> Return Value </b>  Bool
 *   @li                    TRUE  - Transmit data ready flag is set
 *   
 *   @li                    FALSE - Transmit data ready flag is not set
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim
        Bool    xmtDataReady;
        
        xmtDataReady = CSL_mcaspGetXmtDataReady (hMcasp);
        
     @endverbatim
 * ============================================================================
 */
static inline
Bool CSL_mcaspGetXmtDataReady (
    CSL_McaspHandle    hMcasp    
)
{
	return((Bool)CSL_FEXT(hMcasp->regs->XSTAT, MCASP_XSTAT_XDATA));
}

/** ===========================================================================
 *   @n@b CSL_mcaspGetRcvErr
 *
 *   @b Description
 *   @n This function checks whether receiver error interrupt has occurred
 *      or not
 *
 *   @b Arguments
 *   @verbatim
            hMcasp            Handle to the McASP instance
     @endverbatim
 *
 *   <b> Return Value </b>  Bool
 *   @li                    TRUE  - Receiver error interrupt has occurred
 *   
 *   @li                    FALSE - Receiver error interrupt has not occurred
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
        Bool    rcvErr;
        
        rcvErr = CSL_mcaspGetRcvErr (hMcasp);
        
     @endverbatim
 * ============================================================================
 */
static inline
Bool CSL_mcaspGetRcvErr (
    CSL_McaspHandle    hMcasp    
)
{
	return((Bool)CSL_FEXT(hMcasp->regs->RSTAT, MCASP_RSTAT_RERR));
}

/** ===========================================================================
 *   @n@b CSL_mcaspGetRcvClkFail
 *
 *   @b Description
 *   @n This function checks whether receive clock failure flag is set or not
 *
 *   @b Arguments
 *   @verbatim
            hMcasp            Handle to the McASP instance
     @endverbatim
 *
 *   <b> Return Value </b>  Bool
 *   @li                    TRUE  - Receive clock failure flag is set
 *   
 *   @li                    FALSE - Receive clock failure flag is not set
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
        Bool    rcvClkFail;
        
        rcvClkFail = CSL_mcaspGetRcvClkFail (hMcasp);
        
     @endverbatim
 * ============================================================================
 */
static inline
Bool CSL_mcaspGetRcvClkFail (
    CSL_McaspHandle    hMcasp    
)
{
	return((Bool)CSL_FEXT(hMcasp->regs->RSTAT, MCASP_RSTAT_RCKFAIL));
}

/** ===========================================================================
 *   @n@b CSL_mcaspGetRcvSyncErr
 *
 *   @b Description
 *   @n This function checks whether unexpected receive frame sync flag is set
 *      or not
 *
 *   @b Arguments
 *   @verbatim
            hMcasp            Handle to the McASP instance
     @endverbatim
 *
 *   <b> Return Value </b>  Bool
 *   @li                    TRUE  - Unexpected receive frame sync error has
 *                                  occurred
 *   
 *   @li                    FALSE - Unexpected receive frame sync error has
 *                                  not occurred
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
        Bool    rcvSyncErr;
        
        rcvSyncErr = CSL_mcaspGetRcvSyncErr (hMcasp);
        
     @endverbatim
 * ============================================================================
 */
static inline
Bool CSL_mcaspGetRcvSyncErr (
    CSL_McaspHandle    hMcasp    
)
{
	return((Bool)CSL_FEXT(hMcasp->regs->RSTAT, MCASP_RSTAT_RSYNCERR));
}

/** ===========================================================================
 *   @n@b CSL_mcaspGetRcvOverrun
 *
 *   @b Description
 *   @n This function checks whether receiver overrun flag is set or not
 *
 *   @b Arguments
 *   @verbatim
            hMcasp            Handle to the McASP instance
     @endverbatim
 *
 *   <b> Return Value </b>  Bool
 *   @li                    TRUE  - Receiver overrun flag is set 
 *   
 *   @li                    FALSE - Receiver overrun flag is not set
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
        Bool    rcvOverRun;
        
        rcvOverRun = CSL_mcaspGetRcvOverrun (hMcasp);
        
     @endverbatim
 * ============================================================================
 */
static inline
Bool CSL_mcaspGetRcvOverrun (
    CSL_McaspHandle    hMcasp    
)
{
	return((Bool)CSL_FEXT(hMcasp->regs->RSTAT, MCASP_RSTAT_ROVRN));
}

/** ===========================================================================
 *   @n@b CSL_mcaspGetRcvDataReady
 *
 *   @b Description
 *   @n This function checks whether receive data ready flag is set or not
 *
 *   @b Arguments
 *   @verbatim
            hMcasp            Handle to the McASP instance
     @endverbatim
 *
 *   <b> Return Value </b>  Bool
 *   @li                    TRUE  - Receive data ready flag is set
 *   
 *   @li                    FALSE - Receive data ready flag is not set
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
        Bool    rcvDataReady;
        
        rcvDataReady = CSL_mcaspGetRcvDataReady (hMcasp);
        
     @endverbatim
 * ============================================================================
 */
static inline
Bool CSL_mcaspGetRcvDataReady (
    CSL_McaspHandle    hMcasp    
)
{
	return((Bool)CSL_FEXT(hMcasp->regs->RSTAT, MCASP_RSTAT_RDATA));
}

/** ===========================================================================
 *   @n@b CSL_mcaspGetSerRcvReady
 *
 *   @b Description
 *   @n This function checks whether receive buffer ready bit of serializer
 *      control register is set or not
 *
 *   @b Arguments
 *   @verbatim
            hMcasp            Handle to the McASP instance
            
            serRcvReady       Serailizer status will be stored here
            
            serNum            Serailizer register to be checked
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK - Command executed successfully 
 *   
 *   @li                    CSL_ESYS_FAIL - If serNum passed is invalid.
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
        CSL_Status st;
        Bool                    serRcvReady;
        CSL_McaspSerializerNum  serNum = SERIALIZER_1;
        
        st = CSL_mcaspGetSerRcvReady (hMcasp, &serRcvReady, serNum);
        
     @endverbatim
 * ============================================================================
 */
static inline
CSL_Status CSL_mcaspGetSerRcvReady (
    CSL_McaspHandle         hMcasp,
    Bool                    *serRcvReady,
    CSL_McaspSerializerNum  serNum
)
{
	CSL_Status st = CSL_SOK;

	switch (serNum) 
	{
		case SERIALIZER_1:
			*(Bool *)serRcvReady = ((Bool)CSL_FEXT(hMcasp->regs->SRCTL0,
			MCASP_SRCTL0_RRDY));
			break;
		case SERIALIZER_2:
			*(Bool *)serRcvReady = ((Bool)CSL_FEXT(hMcasp->regs->SRCTL1,
			MCASP_SRCTL1_RRDY));
			break;
		case SERIALIZER_3:
			*(Bool *)serRcvReady = ((Bool)CSL_FEXT(hMcasp->regs->SRCTL2,
			MCASP_SRCTL2_RRDY));
			break;
		case SERIALIZER_4:
			*(Bool *)serRcvReady = ((Bool)CSL_FEXT(hMcasp->regs->SRCTL3,
			MCASP_SRCTL3_RRDY));
			break;
		case SERIALIZER_5:
			*(Bool *)serRcvReady = ((Bool)CSL_FEXT(hMcasp->regs->SRCTL4,
			MCASP_SRCTL4_RRDY));
			break;
		case SERIALIZER_6:
			*(Bool *)serRcvReady = ((Bool)CSL_FEXT(hMcasp->regs->SRCTL5,
			MCASP_SRCTL5_RRDY));
			break;
		case SERIALIZER_7:
			*(Bool *)serRcvReady = ((Bool)CSL_FEXT(hMcasp->regs->SRCTL6,
			MCASP_SRCTL6_RRDY));
			break;
		case SERIALIZER_8:
			*(Bool *)serRcvReady = ((Bool)CSL_FEXT(hMcasp->regs->SRCTL7,
			MCASP_SRCTL7_RRDY));
			break;
		case SERIALIZER_9:
			*(Bool *)serRcvReady = ((Bool)CSL_FEXT(hMcasp->regs->SRCTL8,
			MCASP_SRCTL8_RRDY));
			break;
		case SERIALIZER_10:
			*(Bool *)serRcvReady = ((Bool)CSL_FEXT(hMcasp->regs->SRCTL9,
			MCASP_SRCTL9_RRDY));
			break;
		case SERIALIZER_11:
			*(Bool *)serRcvReady = ((Bool)CSL_FEXT(hMcasp->regs->SRCTL10,
			MCASP_SRCTL10_RRDY));
			break;
		case SERIALIZER_12:
			*(Bool *)serRcvReady = ((Bool)CSL_FEXT(hMcasp->regs->SRCTL11,
			MCASP_SRCTL11_RRDY));
			break;
		case SERIALIZER_13:
			*(Bool *)serRcvReady = ((Bool)CSL_FEXT(hMcasp->regs->SRCTL12,
			MCASP_SRCTL12_RRDY));
			break;
		case SERIALIZER_14:
			*(Bool *)serRcvReady = ((Bool)CSL_FEXT(hMcasp->regs->SRCTL13,
			MCASP_SRCTL13_RRDY));
			break;
		case SERIALIZER_15:
			*(Bool *)serRcvReady = ((Bool)CSL_FEXT(hMcasp->regs->SRCTL14,
			MCASP_SRCTL14_RRDY));
			break;
		case SERIALIZER_16:
			*(Bool *)serRcvReady = ((Bool)CSL_FEXT(hMcasp->regs->SRCTL15,
			MCASP_SRCTL15_RRDY));
			break;
		default:
			st = CSL_ESYS_FAIL;
	}
	return st;
}

/** ===========================================================================
 *   @n@b CSL_mcaspGetSerXmtReady
 *
 *   @b Description
 *   @n This function checks whether transmit buffer ready bit of serializer
 *      control register is set or not
 *
 *   @b Arguments
 *   @verbatim
            hMcasp            Handle to the McASP instance
            
            serXmtReady       Status of the serializer will be stored here
            
            serNum            Serailizer register to be checked
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK - Command executed successfully
 *   
 *   @li                    CSL_ESYS_FAIL - If serNum passed is invalid.
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
        CSL_Status st;
        Bool    serXmtReady;
        CSL_McaspSerializerNum   serNum = SERIALIZER_1;
        
        st = CSL_mcaspGetSerXmtReady (hMcasp, &serXmtReady, serNum);
        
     @endverbatim
 * ============================================================================
 */
static inline
CSL_Status CSL_mcaspGetSerXmtReady (
    CSL_McaspHandle         hMcasp,
    Bool                    *serXmtReady,
    CSL_McaspSerializerNum  serNum
)
{
	CSL_Status st = CSL_SOK;

	switch (serNum) 
	{
		case SERIALIZER_1:
			*(Bool *)serXmtReady = ((Bool)CSL_FEXT(hMcasp->regs->SRCTL0, 
			MCASP_SRCTL0_XRDY));
			break;

		case SERIALIZER_2:
			*(Bool *)serXmtReady = ((Bool)CSL_FEXT(hMcasp->regs->SRCTL1, 
			MCASP_SRCTL1_XRDY));
			break;

		case SERIALIZER_3:
			*(Bool *)serXmtReady = ((Bool)CSL_FEXT(hMcasp->regs->SRCTL2, 
			MCASP_SRCTL2_XRDY));
			break;

		case SERIALIZER_4:
			*(Bool *)serXmtReady = ((Bool)CSL_FEXT(hMcasp->regs->SRCTL3, 
			MCASP_SRCTL3_XRDY));
			break;

		case SERIALIZER_5:
			*(Bool *)serXmtReady = ((Bool)CSL_FEXT(hMcasp->regs->SRCTL4, 
			MCASP_SRCTL4_XRDY));
			break;

		case SERIALIZER_6:
			*(Bool *)serXmtReady = ((Bool)CSL_FEXT(hMcasp->regs->SRCTL5, 
			MCASP_SRCTL5_XRDY));
			break;

		case SERIALIZER_7:
			*(Bool *)serXmtReady = ((Bool)CSL_FEXT(hMcasp->regs->SRCTL6, 
			MCASP_SRCTL0_XRDY));
			break;

		case SERIALIZER_8:
			*(Bool *)serXmtReady = ((Bool)CSL_FEXT(hMcasp->regs->SRCTL7, 
			MCASP_SRCTL0_XRDY));
			break;

		case SERIALIZER_9:
			*(Bool *)serXmtReady = ((Bool)CSL_FEXT(hMcasp->regs->SRCTL8, 
			MCASP_SRCTL0_XRDY));
			break;

		case SERIALIZER_10:
			*(Bool *)serXmtReady = ((Bool)CSL_FEXT(hMcasp->regs->SRCTL9, 
			MCASP_SRCTL0_XRDY));
			break;

		case SERIALIZER_11:
			*(Bool *)serXmtReady = ((Bool)CSL_FEXT(hMcasp->regs->SRCTL10, 
			MCASP_SRCTL0_XRDY));
			break;

		case SERIALIZER_12:
			*(Bool *)serXmtReady = ((Bool)CSL_FEXT(hMcasp->regs->SRCTL11, 
			MCASP_SRCTL0_XRDY));
			break;

		case SERIALIZER_13:
			*(Bool *)serXmtReady = ((Bool)CSL_FEXT(hMcasp->regs->SRCTL12, 
			MCASP_SRCTL0_XRDY));
			break;

		case SERIALIZER_14:
			*(Bool *)serXmtReady = ((Bool)CSL_FEXT(hMcasp->regs->SRCTL13, 
			MCASP_SRCTL0_XRDY));
			break;

		case SERIALIZER_15:
			*(Bool *)serXmtReady = ((Bool)CSL_FEXT(hMcasp->regs->SRCTL14, 
			MCASP_SRCTL0_XRDY));
			break;

		case SERIALIZER_16:
			*(Bool *)serXmtReady = ((Bool)CSL_FEXT(hMcasp->regs->SRCTL15, 
			MCASP_SRCTL0_XRDY));
			break;

		default:
			st = CSL_ESYS_FAIL;
	}
	return st;
}

/** ===========================================================================
 *   @n@b CSL_mcaspGetSerMode
 *
 *   @b Description
 *   @n This function gets the serializer mode
 *
 *   @b Arguments
 *   @verbatim
            hMcasp            Handle to the McASP instance
            
            serMode           Serializer mode will be here
            
            serNum            Serailizer register to be checked
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK - Command executed successfully
 *   
 *   @li                    CSL_ESYS_FAIL - If serNum passed is invalid.
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim
        CSL_Status = st;
        CSL_McaspSerMode   serMode;
        CSL_McaspSerializerNum   serNum = SERIALIZER_1;
        
        st = CSL_mcaspGetSerMode (hMcasp, serMode, serNum);
        
     @endverbatim
 * ============================================================================
 */
static inline
CSL_Status CSL_mcaspGetSerMode (
    CSL_McaspHandle         hMcasp,
    CSL_McaspSerMode        *serMode,
    CSL_McaspSerializerNum  serNum
)
{
	CSL_Status st = CSL_SOK;

	switch (serNum) 
	{
		case SERIALIZER_1:
			*(CSL_McaspSerMode *)serMode = ((CSL_McaspSerMode)
			CSL_FEXT (hMcasp->regs->SRCTL0,
			MCASP_SRCTL0_SRMOD));
			break;
		case SERIALIZER_2:
			*(CSL_McaspSerMode *)serMode = ((CSL_McaspSerMode)
			CSL_FEXT(hMcasp->regs->SRCTL1,
			MCASP_SRCTL1_SRMOD));
			break;
		case SERIALIZER_3:
			*(CSL_McaspSerMode *)serMode = ((CSL_McaspSerMode)
			CSL_FEXT(hMcasp->regs->SRCTL2, 
			MCASP_SRCTL2_SRMOD));
			break;
		case SERIALIZER_4:
			*(CSL_McaspSerMode *)serMode = ((CSL_McaspSerMode)
			CSL_FEXT(hMcasp->regs->SRCTL3, 
			MCASP_SRCTL3_SRMOD));
			break;
		case SERIALIZER_5:
			*(CSL_McaspSerMode *)serMode = ((CSL_McaspSerMode)
			CSL_FEXT(hMcasp->regs->SRCTL4, 
			MCASP_SRCTL4_SRMOD));
			break;
		case SERIALIZER_6:
			*(CSL_McaspSerMode *)serMode = ((CSL_McaspSerMode)
			CSL_FEXT(hMcasp->regs->SRCTL5, 
			MCASP_SRCTL5_SRMOD));
			break;
		case SERIALIZER_7:
			*(CSL_McaspSerMode *)serMode = ((CSL_McaspSerMode)
			CSL_FEXT(hMcasp->regs->SRCTL6, 
			MCASP_SRCTL6_SRMOD));
			break;
		case SERIALIZER_8:
			*(CSL_McaspSerMode *)serMode = ((CSL_McaspSerMode)
			CSL_FEXT(hMcasp->regs->SRCTL7, 
			MCASP_SRCTL7_SRMOD));
			break;
		case SERIALIZER_9:
			*(CSL_McaspSerMode *)serMode = ((CSL_McaspSerMode)
			CSL_FEXT(hMcasp->regs->SRCTL8, 
			MCASP_SRCTL8_SRMOD));
			break;
		case SERIALIZER_10:
			*(CSL_McaspSerMode *)serMode = ((CSL_McaspSerMode)
			CSL_FEXT(hMcasp->regs->SRCTL9, 
			MCASP_SRCTL9_SRMOD));
			break;
		case SERIALIZER_11:
			*(CSL_McaspSerMode *)serMode = ((CSL_McaspSerMode)
			CSL_FEXT(hMcasp->regs->SRCTL10, 
			MCASP_SRCTL10_SRMOD));
			break;
		case SERIALIZER_12:
			*(CSL_McaspSerMode *)serMode = ((CSL_McaspSerMode)
			CSL_FEXT(hMcasp->regs->SRCTL11, 
			MCASP_SRCTL11_SRMOD));
			break;
		case SERIALIZER_13:
			*(CSL_McaspSerMode *)serMode = ((CSL_McaspSerMode)
			CSL_FEXT(hMcasp->regs->SRCTL12, 
			MCASP_SRCTL12_SRMOD));
			break;
		case SERIALIZER_14:
			*(CSL_McaspSerMode *)serMode = ((CSL_McaspSerMode)
			CSL_FEXT(hMcasp->regs->SRCTL13, 
			MCASP_SRCTL13_SRMOD));
			break;
		case SERIALIZER_15:
			*(CSL_McaspSerMode *)serMode = ((CSL_McaspSerMode)
			CSL_FEXT(hMcasp->regs->SRCTL14, 
			MCASP_SRCTL14_SRMOD));
			break;
		case SERIALIZER_16:
			*(CSL_McaspSerMode *)serMode = ((CSL_McaspSerMode)
			CSL_FEXT(hMcasp->regs->SRCTL15, 
			MCASP_SRCTL15_SRMOD));
			break;
		default:
			st = CSL_ESYS_FAIL;
	}
	return st;
}

/** ===========================================================================
 *   @n@b CSL_mcaspGetXmtStat
 *
 *   @b Description
 *   @n This function returns the value of XSTAT register.
 *
 *   @b Arguments
 *   @verbatim
            hMcasp            Handle to the McASP instance
     @endverbatim
 *
 *   <b> Return Value </b>  Uint32
 *   @li                    Value of XSTAT register.
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  Value of XSTAT register will be returned.
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim
        Uint32 xmtStat;
        
        xmtStat = CSL_mcaspGetXmtStat (hMcasp);
        
     @endverbatim
 * ============================================================================
 */
static inline
Uint32 CSL_mcaspGetXmtStat (
    CSL_McaspHandle         hMcasp
)
{
	return((Uint32) hMcasp->regs->XSTAT);
}

/** ===========================================================================
 *   @n@b CSL_mcaspGetRcvStat
 *
 *   @b Description
 *   @n This function returns the value of RSTAT register.
 *
 *   @b Arguments
 *   @verbatim
            hMcasp            Handle to the McASP instance
     @endverbatim
 *
 *   <b> Return Value </b>  Uint32
 *   @li                    Value of RSTAT register.
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  Value of RSTAT register will be returned.
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim
        Uint32 rcvStat;
        
        rcvStat = CSL_mcaspGetRcvStat (hMcasp);
        
     @endverbatim
 * ============================================================================
 */
static inline
Uint32 CSL_mcaspGetRcvStat (
    CSL_McaspHandle         hMcasp
)
{
	return((Uint32) hMcasp->regs->RSTAT);
}

/** ===========================================================================
 *   @n@b CSL_mcaspGetSmFsXmt
 *
 *   @b Description
 *   @n This function returns the XSMRST and XFRST field values of XGBLCTL
 *      register.
 *
 *   @b Arguments
 *   @verbatim
            hMcasp            Handle to the McASP instance
     @endverbatim
 *
 *   <b> Return Value </b>  Uint8
 *   @li          0x00 - Both transmit frame generator sync and transmit state
 *                       machine are reset.
 *   @li          0x1  - Only transmit state machine is active.
 *   @li          0x10 - Only transmit frame sync generator is active.
 *   @li          0x11 - Both transmit frame generator sync and transmit state
 *                       machine are active.
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  Value of XSMRST and XFRST fields of XGBLCTL register will be returned.
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim
        Uint16 smFsXmt;
        
        smFsXmt = CSL_mcaspGetSmFsXmt (hMcasp);
        
     @endverbatim
 * ============================================================================
 */
static inline
Uint8 CSL_mcaspGetSmFsXmt (
    CSL_McaspHandle         hMcasp
)
{
	Uint8 smFsXmt = 0;

	smFsXmt = ((Uint8) CSL_FEXT (hMcasp->regs->XGBLCTL,
		MCASP_XGBLCTL_XFRST));
	smFsXmt <<= 4;
	smFsXmt |= ((Uint8) CSL_FEXT (hMcasp->regs->XGBLCTL,
		MCASP_XGBLCTL_XSMRST));

	return smFsXmt;
}

/** ===========================================================================
 *   @n@b CSL_mcaspGetSmFsRcv
 *
 *   @b Description
 *   @n This function returns the RSMRST and RFRST field values of RGBLCTL
 *      register.
 *
 *   @b Arguments
 *   @verbatim
            hMcasp            Handle to the McASP instance
     @endverbatim
 *
 *   <b> Return Value </b>  Uint8
 *   @li          0x00 - Both receive frame generator sync and receive state
 *                       machine are reset.
 *   @li          0x1  - Only receive state machine is active.
 *   @li          0x10 - Only receive frame sync generator is active.
 *   @li          0x11 - Both receive frame generator sync and receive state
 *                       machine are active.
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  Value of RSMRST and RFRST fields of RGBLCTL register will be returned.
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim
        Uint16 smFsRcv;
        
        smFsRcv = CSL_mcaspGetSmFsRcv (hMcasp);
        
     @endverbatim
 * ============================================================================
 */
static inline
Uint8 CSL_mcaspGetSmFsRcv (
    CSL_McaspHandle         hMcasp
)
{
	Uint8 smFsRcv = 0;

	smFsRcv = ((Uint8) CSL_FEXT (hMcasp->regs->RGBLCTL,
			MCASP_RGBLCTL_RFRST));
	smFsRcv <<= 4;
	smFsRcv |= ((Uint8) CSL_FEXT (hMcasp->regs->RGBLCTL,
			MCASP_RGBLCTL_RSMRST));

	return smFsRcv;
}

/** ===========================================================================
 *   @n@b CSL_mcaspGetDitMode
 *
 *   @b Description
 *   @n This function returns the status of DITEN bit in DITCTL register.
 *      register.
 *
 *   @b Arguments
 *   @verbatim
            hMcasp            Handle to the McASP instance
     @endverbatim
 *
 *   <b> Return Value </b>  Bool
 *   @li                    TRUE    DIT Mode is enabled
 *
 *   @li                    FALSE   DIT Mode is not enabled
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  Value of DITEN bit in DITCTL register will be returned.
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim
        Bool ditMode;
        
        ditMode = CSL_mcaspGetDitMode (hMcasp);
        
     @endverbatim
 * ============================================================================
 */
static inline
Bool CSL_mcaspGetDitMode (
    CSL_McaspHandle         hMcasp
)
{
	return((Bool) CSL_FEXT(hMcasp->regs->DITCTL, MCASP_DITCTL_DITEN));
}

/**
 *  Hardware Control Functions of McASP
 */

/** ===========================================================================
 *   @n@b CSL_mcaspSetXmtGbl
 *
 *   @b Description
 *   @n This function configures the transmitter global control register
 *
 *   @b Arguments
 *   @verbatim
            hMcasp          Handle to the McASP instance

            xmtGbl          Value to be loaded into the XGBLCTL register
     @endverbatim
 *
 *   <b> Return Value </b>  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  XGBLCTL register will be loaded with the specified value
 *
 *   @b  Modifies
 *   @n  XGBLCTL register
 *
 *   @b Example
 *   @verbatim
        Uint32 xmtGbl;  
        
        xmtGbl = 0x00000000;
        CSL_mcaspSetXmtGbl (hMcasp, xmtGbl);
        
     @endverbatim
 * ============================================================================
 */
static inline
void CSL_mcaspSetXmtGbl (
    CSL_McaspHandle     hMcasp,
    Uint32              xmtGbl
)
{
	/* Configure XGBLCTL with the value passed */
	hMcasp->regs->XGBLCTL = xmtGbl;
}

/** ===========================================================================
 *   @n@b CSL_mcaspSetRcvGbl
 *
 *   @b Description
 *   @n This function configures the receiver global control register
 *
 *   @b Arguments
 *   @verbatim
            hMcasp      Handle to the McASP instance

            rcvGbl      Value to be loaded into the RGBLCTL register
     @endverbatim
 *
 *   <b> Return Value </b>  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  RGBLCTL register will be loaded with the specified value
 *
 *   @b  Modifies
 *   @n  RGBLCTL register
 *
 *   @b Example
 *   @verbatim
        Uint32  rcvGbl;
        
        rcvGbl = 0x00000000;  
        CSL_mcaspSetRcvGbl (hMcasp, rcvGbl);
        
     @endverbatim
 * ============================================================================
 */
static inline
void CSL_mcaspSetRcvGbl (
    CSL_McaspHandle     hMcasp,
    Uint32              rcvGbl
)
{
	/* Configure RGBLCTL with the value passed */
	hMcasp->regs->RGBLCTL =  rcvGbl;
}

/** ===========================================================================
 *   @n@b CSL_mcaspResetXmtFSRst
 *
 *   @b Description
 *   @n This function resets the transmit frame sync generator reset enable bit
 *      of transmit global control register
 *
 *   @b Arguments
 *   @verbatim
            hMcasp      Handle to the McASP instance
     @endverbatim
 *
 *   <b> Return Value </b>  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  XGBLCTL register will be loaded with the specified value
 *
 *   @b  Modifies
 *   @n  XGBLCTL register
 *
 *   @b Example
 *   @verbatim
        CSL_mcaspResetXmtFSRst (hMcasp);
        
     @endverbatim
 * ============================================================================
 */
static inline
void CSL_mcaspResetXmtFSRst (
    CSL_McaspHandle     hMcasp
)
{
	CSL_FINST (hMcasp->regs->XGBLCTL, MCASP_XGBLCTL_XFRST, RESET);
}

/** ===========================================================================
 *   @n@b CSL_mcaspResetRcvFSRst
 *
 *   @b Description
 *   @n This function resets the receive frame sync generator reset enable bit
 *      of receive global control register
 *
 *   @b Arguments
 *   @verbatim
            hMcasp      Handle to the McASP instance
     @endverbatim
 *
 *   <b> Return Value </b>  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  RGBLCTL register will be loaded with the specified value
 *
 *   @b  Modifies
 *   @n  RGBLCTL register
 *
 *   @b Example
 *   @verbatim
        CSL_mcaspResetRcvFSRst (hMcasp);
        
     @endverbatim
 * ============================================================================
 */
static inline
void CSL_mcaspResetRcvFSRst (
    CSL_McaspHandle     hMcasp
)
{
	CSL_FINST (hMcasp->regs->RGBLCTL, MCASP_RGBLCTL_RFRST, RESET);
}

/** ===========================================================================
 *   @n@b CSL_mcaspConfigAudioMute
 *
 *   @b Description
 *   @n This function configures the AMUTE register with specified values
 *
 *   @b Arguments
 *   @verbatim
            hMcasp          Handle to the McASP instance

            audioMute       Value to be loaded to the register
     @endverbatim
 *
 *   <b> Return Value </b>  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  AMUTE register will be loaded with the specified value
 *
 *   @b  Modifies
 *   @n  AMUTE register
 *
 *   @b Example
 *   @verbatim
        Uint32  audioMute;
        
        audioMute = 0x00000010;
        CSL_mcaspConfigAudioMute (hMcasp, audioMute);
        
     @endverbatim
 * ============================================================================
 */
static inline
void CSL_mcaspConfigAudioMute (
    CSL_McaspHandle     hMcasp,
    Uint32              audioMute
)
{
	/* Configure AMUTE register */
	hMcasp->regs->AMUTE = (Uint16) audioMute;
}

/** ===========================================================================
 *   @n@b CSL_mcaspConfigLoopBack
 *
 *   @b Description
 *   @n This function sets the digital loopback mode
 *
 *   @b Arguments
 *   @verbatim
            hMcasp          Handle to the McASP instance

            loopBack        Value to be loaded into the bit filed
     @endverbatim
 *
 *   <b> Return Value </b>  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  GBLCTL and DLBCTL register will be loaded with the specified value
 *
 *   @b  Modifies
 *   @n  GBLCTL, DLBCTL register
 *
 *   @b Example
 *   @verbatim
        Bool   loopBack;   
        
        loopBack = TRUE;
        CSL_mcaspConfigLoopBack (hMcasp, loopBack);
        
     @endverbatim
 * ============================================================================
 */
static inline
void CSL_mcaspConfigLoopBack (
    CSL_McaspHandle     hMcasp,
    Bool                loopBack
)
{
	Bool    loopBackEnable;
	Bool    orderBit;
	Uint32  serNum = 0;

	/* Reset the RSRCLR and XSRCLR registers in GBLCTL */
	CSL_FINST (hMcasp->regs->GBLCTL, MCASP_GBLCTL_RSRCLR, CLEAR);
	CSL_FINST (hMcasp->regs->GBLCTL, MCASP_GBLCTL_XSRCLR, CLEAR);

	/* Reset the RSMRST and XSMRST registers in GBLCTL */
	CSL_FINST (hMcasp->regs->GBLCTL, MCASP_GBLCTL_RSMRST, RESET);
	CSL_FINST (hMcasp->regs->GBLCTL, MCASP_GBLCTL_XSMRST, RESET);

	/* Reset the RFRST and XFRST registers in GBLCTL */
	CSL_FINST (hMcasp->regs->GBLCTL, MCASP_GBLCTL_RFRST, RESET);
	CSL_FINST (hMcasp->regs->GBLCTL, MCASP_GBLCTL_XFRST, RESET);

	/* configure loop back mode */
	CSL_FINS (hMcasp->regs->DLBCTL, MCASP_DLBCTL_DLBEN, loopBack);

	loopBackEnable = CSL_FEXT (hMcasp->regs->DLBCTL, MCASP_DLBCTL_DLBEN);

	if (loopBackEnable == TRUE) {
		CSL_FINST (hMcasp->regs->DLBCTL, MCASP_DLBCTL_MODE, XMTCLK);
	}

	orderBit = CSL_FEXT (hMcasp->regs->DLBCTL, MCASP_DLBCTL_ORD);

	if (orderBit == TRUE) {
		while (serNum < hMcasp->numOfSerializers) {
			CSL_FINST (hMcasp->regs->SRCTL0, MCASP_SRCTL0_SRMOD, XMT);
			CSL_FINST (hMcasp->regs->PDIR, MCASP_PDIR_AXR0, OUTPUT);
			if (++serNum >= hMcasp->numOfSerializers) break;

			CSL_FINST (hMcasp->regs->SRCTL1, MCASP_SRCTL1_SRMOD, RCV);
			CSL_FINST (hMcasp->regs->PDIR, MCASP_PDIR_AXR1, INPUT);
			if (++serNum >= hMcasp->numOfSerializers) break;

			CSL_FINST (hMcasp->regs->SRCTL2, MCASP_SRCTL2_SRMOD, XMT);
			CSL_FINST (hMcasp->regs->PDIR, MCASP_PDIR_AXR2, OUTPUT);
			if (++serNum >= hMcasp->numOfSerializers) break;

			CSL_FINST (hMcasp->regs->SRCTL3, MCASP_SRCTL3_SRMOD, RCV);
			CSL_FINST (hMcasp->regs->PDIR, MCASP_PDIR_AXR3, INPUT);
			if (++serNum >= hMcasp->numOfSerializers) break;

			CSL_FINST (hMcasp->regs->SRCTL4, MCASP_SRCTL4_SRMOD, XMT);
			CSL_FINST (hMcasp->regs->PDIR, MCASP_PDIR_AXR4, OUTPUT);
			if (++serNum >= hMcasp->numOfSerializers) break;

			CSL_FINST (hMcasp->regs->SRCTL5, MCASP_SRCTL5_SRMOD, RCV);
			CSL_FINST (hMcasp->regs->PDIR, MCASP_PDIR_AXR5, INPUT);
			if (++serNum >= hMcasp->numOfSerializers) break;

			CSL_FINST (hMcasp->regs->SRCTL6, MCASP_SRCTL6_SRMOD, XMT);
			CSL_FINST (hMcasp->regs->PDIR, MCASP_PDIR_AXR6, OUTPUT);
			if (++serNum >= hMcasp->numOfSerializers) break;

			CSL_FINST (hMcasp->regs->SRCTL7, MCASP_SRCTL7_SRMOD, RCV);
			CSL_FINST (hMcasp->regs->PDIR, MCASP_PDIR_AXR7, INPUT);
			if (++serNum >= hMcasp->numOfSerializers) break;

			CSL_FINST (hMcasp->regs->SRCTL8, MCASP_SRCTL8_SRMOD, XMT);
			CSL_FINST (hMcasp->regs->PDIR, MCASP_PDIR_AXR8, OUTPUT);
			if (++serNum >= hMcasp->numOfSerializers) break;

			CSL_FINST (hMcasp->regs->SRCTL9, MCASP_SRCTL9_SRMOD, RCV);
			CSL_FINST (hMcasp->regs->PDIR, MCASP_PDIR_AXR9, INPUT);
			if (++serNum >= hMcasp->numOfSerializers) break;

			CSL_FINST (hMcasp->regs->SRCTL10, MCASP_SRCTL10_SRMOD, XMT);
			CSL_FINST (hMcasp->regs->PDIR, MCASP_PDIR_AXR10, OUTPUT);
			if (++serNum >= hMcasp->numOfSerializers) break;

			CSL_FINST (hMcasp->regs->SRCTL11, MCASP_SRCTL11_SRMOD, RCV);
			CSL_FINST (hMcasp->regs->PDIR, MCASP_PDIR_AXR11, INPUT);
			if (++serNum >= hMcasp->numOfSerializers) break;

			CSL_FINST (hMcasp->regs->SRCTL12, MCASP_SRCTL12_SRMOD, XMT);
			CSL_FINST (hMcasp->regs->PDIR, MCASP_PDIR_AXR12, OUTPUT);
			if (++serNum >= hMcasp->numOfSerializers) break;

			CSL_FINST (hMcasp->regs->SRCTL13, MCASP_SRCTL13_SRMOD, RCV);
			CSL_FINST (hMcasp->regs->PDIR, MCASP_PDIR_AXR13, INPUT);
			if (++serNum >= hMcasp->numOfSerializers) break;

			CSL_FINST (hMcasp->regs->SRCTL14, MCASP_SRCTL14_SRMOD, XMT);
			CSL_FINST (hMcasp->regs->PDIR, MCASP_PDIR_AXR14, OUTPUT);
			if (++serNum >= hMcasp->numOfSerializers) break;

			CSL_FINST (hMcasp->regs->SRCTL15, MCASP_SRCTL15_SRMOD, RCV);
			CSL_FINST (hMcasp->regs->PDIR, MCASP_PDIR_AXR15, INPUT);
			if (++serNum >= hMcasp->numOfSerializers) break;
		}
	} else {
		while (serNum < hMcasp->numOfSerializers) {
			CSL_FINST (hMcasp->regs->SRCTL0, MCASP_SRCTL0_SRMOD, RCV);
			CSL_FINST (hMcasp->regs->PDIR, MCASP_PDIR_AXR0, INPUT);
			if (++serNum >= hMcasp->numOfSerializers) break;

			CSL_FINST (hMcasp->regs->SRCTL1, MCASP_SRCTL1_SRMOD, XMT);
			CSL_FINST (hMcasp->regs->PDIR, MCASP_PDIR_AXR1, OUTPUT);
			if (++serNum >= hMcasp->numOfSerializers) break;

			CSL_FINST (hMcasp->regs->SRCTL2, MCASP_SRCTL2_SRMOD, RCV);
			CSL_FINST (hMcasp->regs->PDIR, MCASP_PDIR_AXR2, INPUT);
			if (++serNum >= hMcasp->numOfSerializers) break;

			CSL_FINST (hMcasp->regs->SRCTL3, MCASP_SRCTL3_SRMOD, XMT);
			CSL_FINST (hMcasp->regs->PDIR, MCASP_PDIR_AXR3, OUTPUT);
			if (++serNum >= hMcasp->numOfSerializers) break;

			CSL_FINST (hMcasp->regs->SRCTL4, MCASP_SRCTL4_SRMOD, RCV);
			CSL_FINST (hMcasp->regs->PDIR, MCASP_PDIR_AXR4, INPUT);
			if (++serNum >= hMcasp->numOfSerializers) break;

			CSL_FINST (hMcasp->regs->SRCTL5, MCASP_SRCTL5_SRMOD, XMT);
			CSL_FINST (hMcasp->regs->PDIR, MCASP_PDIR_AXR5, OUTPUT);
			if (++serNum >= hMcasp->numOfSerializers) break;

			CSL_FINST (hMcasp->regs->SRCTL6, MCASP_SRCTL6_SRMOD, RCV);
			CSL_FINST (hMcasp->regs->PDIR, MCASP_PDIR_AXR6, INPUT);
			if (++serNum >= hMcasp->numOfSerializers) break;

			CSL_FINST (hMcasp->regs->SRCTL7, MCASP_SRCTL7_SRMOD, XMT);
			CSL_FINST (hMcasp->regs->PDIR, MCASP_PDIR_AXR7, OUTPUT);
			if (++serNum >= hMcasp->numOfSerializers) break;

			CSL_FINST (hMcasp->regs->SRCTL8, MCASP_SRCTL8_SRMOD, RCV);
			CSL_FINST (hMcasp->regs->PDIR, MCASP_PDIR_AXR8, INPUT);
			if (++serNum >= hMcasp->numOfSerializers) break;

			CSL_FINST (hMcasp->regs->SRCTL9, MCASP_SRCTL9_SRMOD, XMT);
			CSL_FINST (hMcasp->regs->PDIR, MCASP_PDIR_AXR9, OUTPUT);
			if (++serNum >= hMcasp->numOfSerializers) break;

			CSL_FINST (hMcasp->regs->SRCTL10, MCASP_SRCTL10_SRMOD, RCV);
			CSL_FINST (hMcasp->regs->PDIR, MCASP_PDIR_AXR10, INPUT);
			if (++serNum >= hMcasp->numOfSerializers) break;

			CSL_FINST (hMcasp->regs->SRCTL11, MCASP_SRCTL11_SRMOD, XMT);
			CSL_FINST (hMcasp->regs->PDIR, MCASP_PDIR_AXR11, OUTPUT);
			if (++serNum >= hMcasp->numOfSerializers) break;

			CSL_FINST (hMcasp->regs->SRCTL12, MCASP_SRCTL12_SRMOD, RCV);
			CSL_FINST (hMcasp->regs->PDIR, MCASP_PDIR_AXR12, INPUT);
			if (++serNum >= hMcasp->numOfSerializers) break;

			CSL_FINST (hMcasp->regs->SRCTL13, MCASP_SRCTL13_SRMOD, XMT);
			CSL_FINST (hMcasp->regs->PDIR, MCASP_PDIR_AXR13, OUTPUT);
			if (++serNum >= hMcasp->numOfSerializers) break;

			CSL_FINST (hMcasp->regs->SRCTL14, MCASP_SRCTL14_SRMOD, RCV);
			CSL_FINST (hMcasp->regs->PDIR, MCASP_PDIR_AXR14, INPUT);
			if (++serNum >= hMcasp->numOfSerializers) break;

			CSL_FINST (hMcasp->regs->SRCTL15, MCASP_SRCTL15_SRMOD, XMT);
			CSL_FINST (hMcasp->regs->PDIR, MCASP_PDIR_AXR15, OUTPUT);
			if (++serNum >= hMcasp->numOfSerializers) break;
		}
	}
}

/** ===========================================================================
 *   @n@b CSL_mcaspConfigRcvSlot
 *
 *   @b Description
 *   @n This function configures receive slot with value passed.
 *
 *   @b Arguments
 *   @verbatim
            hMcasp      Handle to the McASP instance

            rcvSlot     Value to be loaded into the bit filed
     @endverbatim
 *
 *   <b> Return Value </b>  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  RTDM register will be loaded with the specified value
 *
 *   @b  Modifies
 *   @n  RTDM register
 *
 *   @b Example
 *   @verbatim
        Uint32  rcvSlot;
        
        rcvSlot = 0x00000034;
        CSL_mcaspConfigRcvSlot (hMcasp, rcvSlot);
        
     @endverbatim
 * ============================================================================
 */
static inline
void CSL_mcaspConfigRcvSlot (
    CSL_McaspHandle     hMcasp,
    Uint32              rcvSlot
)
{
	/* configure the RTDM register */
	hMcasp->regs->RTDM = (Uint32) rcvSlot;
}

/** ===========================================================================
 *   @n@b CSL_mcaspConfigXmtSlot
 *
 *   @b Description
 *   @n This function configures transmit slots with value passed.  
 *
 *   @b Arguments
 *   @verbatim
            hMcasp      Handle to the McASP instance

            xmtSlot     Value to be loaded into the bit filed
     @endverbatim
 *
 *   <b> Return Value </b>  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  XTDM register will be loaded with the specified value
 *
 *   @b  Modifies
 *   @n  XTDM register
 *
 *   @b Example
 *   @verbatim
        Uint32  xmtSlot;
        
        xmtSlot = 0x00000034;
        CSL_mcaspConfigXmtSlot (hMcasp, xmtSlot);
        
     @endverbatim
 * ============================================================================
 */
static inline
void CSL_mcaspConfigXmtSlot (
    CSL_McaspHandle     hMcasp,
    Uint32              xmtSlot
)
{
	/* configure the XTDM register */
	hMcasp->regs->XTDM = (Uint32) xmtSlot;
}

/** ===========================================================================
 *   @n@b CSL_mcaspConfigRcvInt
 *
 *   @b Description
 *   @n This function configures the receiver interrupt control register with
 *      specified value.
 *
 *   @b Arguments
 *   @verbatim
            hMcasp      Handle to the McASP instance

            rcvInt      Value to be loaded into the RINTCTL register
     @endverbatim
 *
 *   <b> Return Value </b>  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  RINTCTL register will be loaded with the specified value
 *
 *   @b  Modifies
 *   @n  RINTCTL register
 *
 *   @b Example
 *   @verbatim
        Uint32  rcvInt;
        
        rcvInt = 0x00001234;
        CSL_mcaspConfigRcvInt (hMcasp, rcvInt);
        
     @endverbatim
 * ============================================================================
 */
static inline
void CSL_mcaspConfigRcvInt (
    CSL_McaspHandle     hMcasp,
    Uint32              rcvInt
)
{
	/* configure the RINTCTL register */
	hMcasp->regs->RINTCTL = (Uint32) rcvInt;
}

/** ===========================================================================
 *   @n@b CSL_mcaspConfigXmtInt
 *
 *   @b Description
 *   @n This function configures the transmitter interrupt control register
 *      with specified value.
 *
 *   @b Arguments
 *   @verbatim
            hMcasp      Handle to the McASP instance

            xmtInt      Value to be loaded into the XINTCTL register
     @endverbatim
 *
 *   <b> Return Value </b>  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  XINTCTL register will be loaded with the specified value
 *
 *   @b  Modifies
 *   @n  XINTCTL register
 *
 *   @b Example
 *   @verbatim
        Uint32  xmtInt;
        
        xmtInt = 0x00001234;
        CSL_mcaspConfigXmtInt (hMcasp, xmtInt);
        
     @endverbatim
 * ============================================================================
 */
static inline
void CSL_mcaspConfigXmtInt (
    CSL_McaspHandle     hMcasp,
    Uint32              xmtInt             
)
{
	/* configure the XINTCTL register */
	hMcasp->regs->XINTCTL = (Uint32) xmtInt;
}

/** ===========================================================================
 *   @n@b CSL_mcaspResetRcvClk
 *
 *   @b Description
 *   @n This function resets the receive clock circuitry
 *
 *   @b Arguments
 *   @verbatim
            hMcasp          Handle to the McASP instance
     @endverbatim
 *
 *   <b> Return Value </b>  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  RHCLKRST and RCLKRST bits will be reset in GBLCTL.
 *
 *   @b  Modifies
 *   @n  GBLCTL register.
 *
 *   @b Example
 *   @verbatim
                
        CSL_mcaspResetRcvClk (hMcasp);
        
     @endverbatim
 * ============================================================================
 */
static inline
void CSL_mcaspResetRcvClk (
    CSL_McaspHandle hMcasp
)
{
	/* Reset AHCLKRCTL */
	CSL_FINST (hMcasp->regs->AHCLKRCTL, MCASP_GBLCTL_RHCLKRST, RESET);
	/* Reset ACLKRCTL */
	CSL_FINST (hMcasp->regs->ACLKRCTL, MCASP_GBLCTL_RCLKRST, RESET);
}

/** ===========================================================================
 *   @n@b CSL_mcaspResetXmtClk
 *
 *   @b Description
 *   @n This function resets the transmit clock circuitry
 *
 *   @b Arguments
 *   @verbatim
            hMcasp          Handle to the McASP instance
     @endverbatim
 *
 *   <b> Return Value </b>  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  XHCLKRST and XCLKRST bits in GBLCTL will be reset.
 *
 *   @b  Modifies
 *   @n  GBLCTL register.
 *
 *   @b Example
 *   @verbatim
                
        CSL_mcaspResetXmtClk (hMcasp);
        
     @endverbatim
 * ============================================================================
 */
static inline
void CSL_mcaspResetXmtClk (
    CSL_McaspHandle         hMcasp
)
{
	/* Reset AHCLKXCTL */
	CSL_FINST (hMcasp->regs->AHCLKXCTL, MCASP_GBLCTL_XHCLKRST, RESET);
	/* Reset ACLKXCTL */
	CSL_FINST (hMcasp->regs->ACLKXCTL, MCASP_GBLCTL_XCLKRST, RESET);
}

/** ===========================================================================
 *   @n@b CSL_mcaspSetRcvClk
 *
 *   @b Description
 *   @n This function configures the receive clock circuitry with specified
 *      values
 *
 *   @b Arguments
 *   @verbatim
            hMcasp          Handle to the McASP instance
            
            rcvClkSet       value to be loaded into receive clock circuitry
     @endverbatim
 *
 *   <b> Return Value </b>  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  ACLKRCTL, AHCLKRCTL, RCLKCHK register will be loaded with the
 *       specified values.
 *
 *   @b  Modifies
 *   @n  GBLCTL, ACLKRCTL, AHCLKRCTL, RCLKCHK registers
 *
 *   @b Example
 *   @verbatim
        CSL_McaspHwSetupDataClk *rcvClkSet;
        
        rcvClkSet->clkSetupClk = 0x00001234;
        rcvClkSet->clkSetupHiClk = 0x004321;
        rcvClkSet->clkChk = 0x0000abcd;
        
        CSL_mcaspSetRcvClk (hMcasp, rcvClkSet);
        
     @endverbatim
 * ============================================================================
 */
static inline
void CSL_mcaspSetRcvClk (
    CSL_McaspHandle             hMcasp,
    CSL_McaspHwSetupDataClk     *rcvClkSet             
)
{
	Uint32 bitValue = 0;

	/* Reset the bits in GBLCTL */
	CSL_FINST (hMcasp->regs->GBLCTL, MCASP_GBLCTL_RHCLKRST, RESET);
	CSL_FINST (hMcasp->regs->GBLCTL, MCASP_GBLCTL_RCLKRST, RESET);
	CSL_FINST (hMcasp->regs->GBLCTL, MCASP_GBLCTL_RSRCLR, CLEAR);
	CSL_FINST (hMcasp->regs->GBLCTL, MCASP_GBLCTL_RSMRST, RESET);
	CSL_FINST (hMcasp->regs->GBLCTL, MCASP_GBLCTL_RFRST, RESET);

	/* Set the High frequency serial clock */
	hMcasp->regs->AHCLKRCTL = (Uint16) rcvClkSet->clkSetupHiClk;

	if ((CSL_FEXT (hMcasp->regs->AHCLKRCTL, MCASP_AHCLKRCTL_HCLKRM) == TRUE)) {
		CSL_FINST (hMcasp->regs->PDIR, MCASP_PDIR_AHCLKR, OUTPUT);
	}

	/* Set the serial clock */
	hMcasp->regs->ACLKRCTL = (Uint32) rcvClkSet->clkSetupClk;

	if ((CSL_FEXT (hMcasp->regs->ACLKRCTL, MCASP_ACLKRCTL_CLKRM) == TRUE)) {
		CSL_FINST (hMcasp->regs->PDIR, MCASP_PDIR_ACLKR, OUTPUT);
	}

	/* Start the serial clock */
	CSL_FINST (hMcasp->regs->GBLCTL, MCASP_GBLCTL_RCLKRST, ACTIVE);
	{
		while (bitValue != CSL_MCASP_GBLCTL_RCLKRST_ACTIVE) {
			bitValue = CSL_FEXT (hMcasp->regs->GBLCTL, MCASP_GBLCTL_RCLKRST);
		}
	}

	/* Start the high frequency clock */
	CSL_FINST (hMcasp->regs->GBLCTL, MCASP_GBLCTL_RHCLKRST, ACTIVE);
	{   
		bitValue = 0;
		while (bitValue != CSL_MCASP_GBLCTL_RHCLKRST_ACTIVE) {
			bitValue = CSL_FEXT (hMcasp->regs->GBLCTL, MCASP_GBLCTL_RHCLKRST);
		}
	}

	/* Set up the receive clock check control register */
	hMcasp->regs->RCLKCHK   = (Uint32) rcvClkSet->clkChk;
}

/** ===========================================================================
 *   @n@b CSL_mcaspSetXmtClk
 *
 *   @b Description
 *   @n This function configures the transmit clock circuitry with specified
 *      values
 *
 *   @b Arguments
 *   @verbatim
            hMcasp          Handle to the McASP instance
            
            xmtClkSet       value to be loaded into transmit clock circuitry
     @endverbatim
 *
 *   <b> Return Value </b>  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  ACLKXCTL, AHCLKXCTL, XCLKCHK register will be loaded with the
 *       specified values
 *
 *   @b  Modifies
 *   @n  GBLCTL, ACLKXCTL, AHCLKXCTL, XCLKCHK registers
 *
 *   @b Example
 *   @verbatim
        CSL_McaspHwSetupDataClk *xmtClkSet;
        
        xmtClkSet->clkSetupClk = 0x00001234;
        xmtClkSet->clkSetupHiClk = 0x004321;
        xmtClkSet->clkChk = 0x0000abcd;
        
        CSL_mcaspSetXmtClk (hMcasp, xmtClkSet);
        
     @endverbatim
 * ============================================================================
 */
static inline
void CSL_mcaspSetXmtClk (
    CSL_McaspHandle             hMcasp,
    CSL_McaspHwSetupDataClk     *xmtClkSet             
)
{
	Uint32 bitValue = 0;

	/* Reset the bits in GBLCTL */
	CSL_FINST (hMcasp->regs->GBLCTL, MCASP_GBLCTL_XHCLKRST, RESET);
	CSL_FINST (hMcasp->regs->GBLCTL, MCASP_GBLCTL_XCLKRST, RESET);
	CSL_FINST (hMcasp->regs->GBLCTL, MCASP_GBLCTL_XSRCLR, CLEAR);
	CSL_FINST (hMcasp->regs->GBLCTL, MCASP_GBLCTL_XSMRST, RESET);
	CSL_FINST (hMcasp->regs->GBLCTL, MCASP_GBLCTL_XFRST, RESET);

	/* Set the High frequency serial clock */
	hMcasp->regs->AHCLKXCTL = (Uint16) xmtClkSet->clkSetupHiClk;

	if ((CSL_FEXT (hMcasp->regs->AHCLKXCTL, MCASP_AHCLKXCTL_HCLKXM) == TRUE)) {
	CSL_FINST (hMcasp->regs->PDIR, MCASP_PDIR_AHCLKX, OUTPUT);
	}

	/* Set the serial clock */
	hMcasp->regs->ACLKXCTL = (Uint16) xmtClkSet->clkSetupClk;

	if ((CSL_FEXT (hMcasp->regs->ACLKXCTL, MCASP_ACLKXCTL_CLKXM) == TRUE)) {
	CSL_FINST (hMcasp->regs->PDIR, MCASP_PDIR_ACLKX, OUTPUT);
	}

	/* Start the serial clock */
	CSL_FINST (hMcasp->regs->GBLCTL, MCASP_GBLCTL_XCLKRST, ACTIVE);
	{
		while (bitValue != CSL_MCASP_GBLCTL_XCLKRST_ACTIVE) {
			bitValue = CSL_FEXT (hMcasp->regs->GBLCTL, MCASP_GBLCTL_XCLKRST);
		}
	}

	/* Start the high frequency clock */
	CSL_FINST (hMcasp->regs->GBLCTL, MCASP_GBLCTL_XHCLKRST, ACTIVE);

	{
		bitValue = 0;
		while (bitValue != CSL_MCASP_GBLCTL_XHCLKRST_ACTIVE) {
			bitValue = CSL_FEXT (hMcasp->regs->GBLCTL, MCASP_GBLCTL_XHCLKRST);
		}
	}

	/* Set up the transmit clock check control register */
	hMcasp->regs->XCLKCHK   = (Uint32) xmtClkSet->clkChk;
}

/** ===========================================================================
 *   @n@b CSL_mcaspConfigXmtSection
 *
 *   @b Description
 *   @n This function configures format, frame sync, and other parameters
 *      related to the xmt section. Also configures the xmt clk section.
 *
 *   @b Arguments
 *   @verbatim
            hMcasp          Handle to the McASP instance
            
            xmtData         transmit related parameters
            
            glbData         global hardware setup configuration
     @endverbatim
 *
 *   <b> Return Value </b>  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  Format, frame sync, and other parameters related to the xmt section
 *       are configured
 *
 *   @b  Modifies
 *   @n  XFMT, AFSXCTL and other transmit related registers
 *
 *   @b Example
 *   @verbatim
        CSL_McaspHandle             hMcasp;
        CSL_McaspHwSetupData        *xmtData;
        CSL_McaspHwSetupDataGbl     *glbData;
        
        xmtData->fmt = 0x0000abcd;
        xmtData->frSyncCtl = 0x00001234;
        
        CSL_mcaspConfigXmtSection (hMcasp, xmtData, glbData);
        
     @endverbatim
 * ============================================================================
 */
static inline
void CSL_mcaspConfigXmtSection (
    CSL_McaspHandle             hMcasp,
    CSL_McaspHwSetupData        *xmtData
)
{
	Uint32 bitValue = 0;

	/* Configure XMASK register */
	hMcasp->regs->XMASK     = xmtData->mask;

	/* Reset the XSMRST bit in GBLCTL register */
	CSL_FINST (hMcasp->regs->GBLCTL, MCASP_GBLCTL_XSMRST, RESET);

	/* Reset the RSMRST bit in GBLCTL register */
	CSL_FINST (hMcasp->regs->GBLCTL, MCASP_GBLCTL_RSMRST, RESET);

	/* Configure XFMT register */
	hMcasp->regs->XFMT      = (Uint32) xmtData->fmt;

	/*Reset the XFRST register in GBLCTL */
	CSL_FINST (hMcasp->regs->GBLCTL, MCASP_GBLCTL_XFRST, RESET);

	/* Configure AFSXCTL register */
	hMcasp->regs->AFSXCTL = (Uint32) xmtData->frSyncCtl;

	if ((CSL_FEXT (hMcasp->regs->AFSXCTL, MCASP_AFSXCTL_FSXM) == TRUE)) {
	CSL_FINST (hMcasp->regs->PDIR, MCASP_PDIR_AFSX, OUTPUT);
	}

	/* Reset XHCLKRST, XCLKRST, XSRCLR  in GBLCTL */
	CSL_FINST (hMcasp->regs->GBLCTL, MCASP_GBLCTL_XHCLKRST, RESET);
	CSL_FINST (hMcasp->regs->GBLCTL, MCASP_GBLCTL_XCLKRST, RESET);
	CSL_FINST (hMcasp->regs->GBLCTL, MCASP_GBLCTL_XSRCLR, CLEAR);

	/* Configure ACLKXCTL register */
	hMcasp->regs->ACLKXCTL  = (Uint32) xmtData->clk.clkSetupClk;

	if ((CSL_FEXT (hMcasp->regs->ACLKXCTL, MCASP_ACLKXCTL_CLKXM) == TRUE)) {
	CSL_FINST (hMcasp->regs->PDIR, MCASP_PDIR_ACLKX, OUTPUT);
	}

	/* Configure AHCLKXCTL register */
	hMcasp->regs->AHCLKXCTL = (Uint32) xmtData->clk.clkSetupHiClk;

	if ((CSL_FEXT (hMcasp->regs->AHCLKXCTL, MCASP_AHCLKXCTL_HCLKXM) == TRUE)) {
	CSL_FINST (hMcasp->regs->PDIR, MCASP_PDIR_AHCLKX, OUTPUT);
	}

	/* start AHCLKX */
	CSL_FINST (hMcasp->regs->GBLCTL, MCASP_GBLCTL_XHCLKRST, ACTIVE);
	{
		bitValue = 0;
		while (bitValue != CSL_MCASP_GBLCTL_XHCLKRST_ACTIVE) {
			bitValue = CSL_FEXT (hMcasp->regs->GBLCTL,
			MCASP_GBLCTL_XHCLKRST);
		}
	}

	/* start ACLKX */
	CSL_FINST (hMcasp->regs->GBLCTL, MCASP_GBLCTL_XCLKRST, ACTIVE);
	{
		bitValue = 0;
		while (bitValue != CSL_MCASP_GBLCTL_XCLKRST_ACTIVE) {
			bitValue = CSL_FEXT (hMcasp->regs->GBLCTL,
			MCASP_GBLCTL_XCLKRST);
		}
	}

	/* Configure XTDM register */
	hMcasp->regs->XTDM = (Uint32) xmtData->tdm;

	/* Configure XINTCTL register */
	hMcasp->regs->XINTCTL = (Uint32) xmtData->intCtl;

	/* Configure XCLKCHK register */
	hMcasp->regs->XCLKCHK   = (Uint32) xmtData->clk.clkChk;

	/* Configure XSTAT register */
	hMcasp->regs->XSTAT = (Uint32) xmtData->stat;

	/* Configure XEVTCTL register */
	hMcasp->regs->XEVTCTL = (Uint32) xmtData->evtCtl;
}

/** ===========================================================================
 *   @n@b CSL_mcaspConfigRcvSection
 *
 *   @b Description
 *   @n Configure the format, frame sync, and other parameters related to the
 *      rcv section. Also configures the rcv clk section.
 *
 *   @b Arguments
 *   @verbatim
            hMcasp          Handle to the McASP instance
            
            rcvData         transmit related parameters
     @endverbatim
 *
 *   <b> Return Value </b>  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  Format, frame sync, and other parameters related to the rcv section
 *       are configured
 *
 *   @b  Modifies
 *   @n  RFMT, AFSRCTL and other receive related registers
 *
 *   @b Example
 *   @verbatim
        CSL_McaspHandle             hMcasp;
        CSL_McaspHwSetupData        *rcvData;
        
        rcvData->fmt = 0x0000abcd;
        rcvData->frSyncCtl = 0x00001234;
        
        CSL_mcaspConfigXmtSection (hMcasp, rcvData);
        
     @endverbatim
 * ============================================================================
 */
static inline
void CSL_mcaspConfigRcvSection (
    CSL_McaspHandle             hMcasp,
    CSL_McaspHwSetupData        *rcvData
)
{
	Uint32 bitValue = 0;

	/* Configure RMASK register */
	hMcasp->regs->RMASK = (Uint32) rcvData->mask;

	/* Reset the RSMRST bit in GBLCTL register */
	CSL_FINST (hMcasp->regs->GBLCTL, MCASP_GBLCTL_RSMRST, RESET);

	/* Configure RFMT register */
	hMcasp->regs->RFMT = (Uint32) rcvData->fmt;

	/*Reset the RFRST register in GBLCTL */
	CSL_FINST (hMcasp->regs->GBLCTL, MCASP_GBLCTL_RFRST, RESET);

	/* Configure AFSRCTL register */
	hMcasp->regs->AFSRCTL = (Uint32) rcvData->frSyncCtl;

	if ((CSL_FEXT (hMcasp->regs->AFSRCTL, MCASP_AFSRCTL_FSRM) == TRUE)) {
		CSL_FINST (hMcasp->regs->PDIR, MCASP_PDIR_AFSR, OUTPUT);
	}

	/* Reset RHCLKRST, RCLKRST, RSRCLR in GBLCTL */
	CSL_FINST (hMcasp->regs->GBLCTL, MCASP_GBLCTL_RHCLKRST, RESET);
	CSL_FINST (hMcasp->regs->GBLCTL, MCASP_GBLCTL_RCLKRST, RESET);
	CSL_FINST (hMcasp->regs->GBLCTL, MCASP_GBLCTL_RSRCLR, CLEAR);

	/* Configure ACLKRCTL register */
	hMcasp->regs->ACLKRCTL  = (Uint32) rcvData->clk.clkSetupClk;

	if ((CSL_FEXT (hMcasp->regs->ACLKRCTL, MCASP_ACLKRCTL_CLKRM) == TRUE)) {
		CSL_FINST (hMcasp->regs->PDIR, MCASP_PDIR_ACLKR, OUTPUT);
	}

	/* Configure AHCLKRCTL register */
	hMcasp->regs->AHCLKRCTL = (Uint32) rcvData->clk.clkSetupHiClk;

	if ((CSL_FEXT (hMcasp->regs->AHCLKRCTL, MCASP_AHCLKRCTL_HCLKRM) == TRUE)) {
		CSL_FINST (hMcasp->regs->PDIR, MCASP_PDIR_AHCLKR, OUTPUT);
	}

	/* start ACLKR */
	CSL_FINST (hMcasp->regs->GBLCTL, MCASP_GBLCTL_RCLKRST, ACTIVE);
	{
		bitValue = 0;
		while (bitValue != CSL_MCASP_GBLCTL_RCLKRST_ACTIVE) {
			bitValue = CSL_FEXT (hMcasp->regs->GBLCTL, MCASP_GBLCTL_RCLKRST);
		}
	}

	/* start AHCLKR */
	CSL_FINST (hMcasp->regs->GBLCTL, MCASP_GBLCTL_RHCLKRST, ACTIVE);
	{
		bitValue = 0;
		while (bitValue != CSL_MCASP_GBLCTL_RHCLKRST_ACTIVE) {
			bitValue = CSL_FEXT (hMcasp->regs->GBLCTL,
			MCASP_GBLCTL_RHCLKRST);
		}
	}

	/* Configure RTDM register */
	hMcasp->regs->RTDM = (Uint32) rcvData->tdm;

	/* Configure RINTCTL register */
	hMcasp->regs->RINTCTL = (Uint32) rcvData->intCtl;

	/* Configure RCLKCHK register */
	hMcasp->regs->RCLKCHK   = (Uint32) rcvData->clk.clkChk;

	/* Configure RSTAT register */
	hMcasp->regs->RSTAT = (Uint32) rcvData->stat;

	/* Configure REVTCTL register */
	hMcasp->regs->REVTCTL = (Uint32) rcvData->evtCtl;
}

/** ===========================================================================
 *   @n@b CSL_mcaspSetSerXmt
 *
 *   @b Description
 *   @n This function sets a particular serializer to act as transmitter
 *
 *   @b Arguments
 *   @verbatim
            hMcasp          Handle to the McASP instance
            
            serNum          serializer number
     @endverbatim
 *
 *   <b> Return Value </b>  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  Particular serializer will be configured as transmitter
 *
 *   @b  Modifies
 *   @n  SRCTL register
 *
 *   @b Example
 *   @verbatim
        CSL_McaspHandle         hMcasp;
        CSL_McaspSerializerNum  serNum = SERIALIZER_2;
        
        CSL_mcaspSetSerXmt (hMcasp, serNum);
        
     @endverbatim
 * ============================================================================
 */
static inline
void CSL_mcaspSetSerXmt (
    CSL_McaspHandle         hMcasp,
    CSL_McaspSerializerNum  serNum
)
{
	switch (serNum) 
	{
		case SERIALIZER_1:
			CSL_FINST (hMcasp->regs->SRCTL0, MCASP_SRCTL0_SRMOD, XMT);
			CSL_FINST (hMcasp->regs->PDIR, MCASP_PDIR_AXR0, OUTPUT);
			break;

		case SERIALIZER_2:
			CSL_FINST (hMcasp->regs->SRCTL1, MCASP_SRCTL1_SRMOD, XMT);
			CSL_FINST (hMcasp->regs->PDIR, MCASP_PDIR_AXR1, OUTPUT);
			break;

		case SERIALIZER_3:
			CSL_FINST (hMcasp->regs->SRCTL2, MCASP_SRCTL2_SRMOD, XMT);
			CSL_FINST (hMcasp->regs->PDIR, MCASP_PDIR_AXR2, OUTPUT);
			break;

		case SERIALIZER_4:
			CSL_FINST (hMcasp->regs->SRCTL3, MCASP_SRCTL3_SRMOD, XMT);
			CSL_FINST (hMcasp->regs->PDIR, MCASP_PDIR_AXR3, OUTPUT);
			break;

		case SERIALIZER_5:
			CSL_FINST (hMcasp->regs->SRCTL4, MCASP_SRCTL4_SRMOD, XMT);
			CSL_FINST (hMcasp->regs->PDIR, MCASP_PDIR_AXR4, OUTPUT);
			break;

		case SERIALIZER_6:
			CSL_FINST (hMcasp->regs->SRCTL5, MCASP_SRCTL5_SRMOD, XMT);
			CSL_FINST (hMcasp->regs->PDIR, MCASP_PDIR_AXR5, OUTPUT);
			break;

		case SERIALIZER_7:
			CSL_FINST (hMcasp->regs->SRCTL6, MCASP_SRCTL6_SRMOD, XMT);
			CSL_FINST (hMcasp->regs->PDIR, MCASP_PDIR_AXR6, OUTPUT);
			break;

		case SERIALIZER_8:
			CSL_FINST (hMcasp->regs->SRCTL7, MCASP_SRCTL7_SRMOD, XMT);
			CSL_FINST (hMcasp->regs->PDIR, MCASP_PDIR_AXR7, OUTPUT);
			break;

		case SERIALIZER_9:
			CSL_FINST (hMcasp->regs->SRCTL8, MCASP_SRCTL8_SRMOD, XMT);
			CSL_FINST (hMcasp->regs->PDIR, MCASP_PDIR_AXR8, OUTPUT);
			break;

		case SERIALIZER_10:
			CSL_FINST (hMcasp->regs->SRCTL9, MCASP_SRCTL9_SRMOD, XMT);
			CSL_FINST (hMcasp->regs->PDIR, MCASP_PDIR_AXR9, OUTPUT);
			break;

		case SERIALIZER_11:
			CSL_FINST (hMcasp->regs->SRCTL10, MCASP_SRCTL10_SRMOD, XMT);
			CSL_FINST (hMcasp->regs->PDIR, MCASP_PDIR_AXR10, OUTPUT);
			break;

		case SERIALIZER_12:
			CSL_FINST (hMcasp->regs->SRCTL11, MCASP_SRCTL11_SRMOD, XMT);
			CSL_FINST (hMcasp->regs->PDIR, MCASP_PDIR_AXR11, OUTPUT);
			break;

		case SERIALIZER_13:
			CSL_FINST (hMcasp->regs->SRCTL12, MCASP_SRCTL12_SRMOD, XMT);
			CSL_FINST (hMcasp->regs->PDIR, MCASP_PDIR_AXR12, OUTPUT);
			break;

		case SERIALIZER_14:
			CSL_FINST (hMcasp->regs->SRCTL13, MCASP_SRCTL13_SRMOD, XMT);
			CSL_FINST (hMcasp->regs->PDIR, MCASP_PDIR_AXR13, OUTPUT);
			break;

		case SERIALIZER_15:
			CSL_FINST (hMcasp->regs->SRCTL14, MCASP_SRCTL14_SRMOD, XMT);
			CSL_FINST (hMcasp->regs->PDIR, MCASP_PDIR_AXR14, OUTPUT);
			break;

		case SERIALIZER_16:
			CSL_FINST (hMcasp->regs->SRCTL15, MCASP_SRCTL15_SRMOD, XMT);
			CSL_FINST (hMcasp->regs->PDIR, MCASP_PDIR_AXR15, OUTPUT);
			break;
	}
}

/** ===========================================================================
 *   @n@b CSL_mcaspSetSerRcv
 *
 *   @b Description
 *   @n This function sets a particular serializer to act as receiver
 *
 *   @b Arguments
 *   @verbatim
            hMcasp          Handle to the McASP instance
            
            serNum          serializer number
     @endverbatim
 *
 *   <b> Return Value </b>  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  Particular serializer will be configured as receiver
 *
 *   @b  Modifies
 *   @n  SRCTL register
 *
 *   @b Example
 *   @verbatim
        CSL_McaspHandle         hMcasp;
        CSL_McaspSerializerNum  serNum = SERIALIZER_2;
        
        CSL_mcaspSetSerRcv (hMcasp, serNum);
        
     @endverbatim
 * ============================================================================
 */
static inline
void CSL_mcaspSetSerRcv (
    CSL_McaspHandle         hMcasp,
    CSL_McaspSerializerNum  serNum
)
{
	switch (serNum) 
	{
		case SERIALIZER_1:
			CSL_FINST (hMcasp->regs->SRCTL0, MCASP_SRCTL0_SRMOD, RCV);
			CSL_FINST (hMcasp->regs->PDIR, MCASP_PDIR_AXR0, INPUT);
			break;

		case SERIALIZER_2:
			CSL_FINST (hMcasp->regs->SRCTL1, MCASP_SRCTL1_SRMOD, RCV);
			CSL_FINST (hMcasp->regs->PDIR, MCASP_PDIR_AXR1, INPUT);
			break;

		case SERIALIZER_3:
			CSL_FINST (hMcasp->regs->SRCTL2, MCASP_SRCTL2_SRMOD, RCV);
			CSL_FINST (hMcasp->regs->PDIR, MCASP_PDIR_AXR2, INPUT);
			break;

		case SERIALIZER_4:
			CSL_FINST (hMcasp->regs->SRCTL3, MCASP_SRCTL3_SRMOD, RCV);
			CSL_FINST (hMcasp->regs->PDIR, MCASP_PDIR_AXR3, INPUT);
			break;

		case SERIALIZER_5:
			CSL_FINST (hMcasp->regs->SRCTL4, MCASP_SRCTL4_SRMOD, RCV);
			CSL_FINST (hMcasp->regs->PDIR, MCASP_PDIR_AXR4, INPUT);
			break;

		case SERIALIZER_6:
			CSL_FINST (hMcasp->regs->SRCTL5, MCASP_SRCTL5_SRMOD, RCV);
			CSL_FINST (hMcasp->regs->PDIR, MCASP_PDIR_AXR5, INPUT);
			break;

		case SERIALIZER_7:
			CSL_FINST (hMcasp->regs->SRCTL6, MCASP_SRCTL6_SRMOD, RCV);
			CSL_FINST (hMcasp->regs->PDIR, MCASP_PDIR_AXR6, INPUT);
			break;

		case SERIALIZER_8:
			CSL_FINST (hMcasp->regs->SRCTL7, MCASP_SRCTL7_SRMOD, RCV);
			CSL_FINST (hMcasp->regs->PDIR, MCASP_PDIR_AXR7, INPUT);
			break;

		case SERIALIZER_9:
			CSL_FINST (hMcasp->regs->SRCTL8, MCASP_SRCTL8_SRMOD, RCV);
			CSL_FINST (hMcasp->regs->PDIR, MCASP_PDIR_AXR8, INPUT);
			break;

		case SERIALIZER_10:
			CSL_FINST (hMcasp->regs->SRCTL9, MCASP_SRCTL9_SRMOD, RCV);
			CSL_FINST (hMcasp->regs->PDIR, MCASP_PDIR_AXR9, INPUT);
			break;

		case SERIALIZER_11:
			CSL_FINST (hMcasp->regs->SRCTL10, MCASP_SRCTL10_SRMOD, RCV);
			CSL_FINST (hMcasp->regs->PDIR, MCASP_PDIR_AXR10, INPUT);
			break;

		case SERIALIZER_12:
			CSL_FINST (hMcasp->regs->SRCTL11, MCASP_SRCTL11_SRMOD, RCV);
			CSL_FINST (hMcasp->regs->PDIR, MCASP_PDIR_AXR11, INPUT);
			break;

		case SERIALIZER_13:
			CSL_FINST (hMcasp->regs->SRCTL12, MCASP_SRCTL12_SRMOD, RCV);
			CSL_FINST (hMcasp->regs->PDIR, MCASP_PDIR_AXR12, INPUT);
			break;

		case SERIALIZER_14:
			CSL_FINST (hMcasp->regs->SRCTL13, MCASP_SRCTL13_SRMOD, RCV);
			CSL_FINST (hMcasp->regs->PDIR, MCASP_PDIR_AXR13, INPUT);
			break;

		case SERIALIZER_15:
			CSL_FINST (hMcasp->regs->SRCTL14, MCASP_SRCTL14_SRMOD, RCV);
			CSL_FINST (hMcasp->regs->PDIR, MCASP_PDIR_AXR14, INPUT);
			break;

		case SERIALIZER_16:
			CSL_FINST (hMcasp->regs->SRCTL15, MCASP_SRCTL15_SRMOD, RCV);
			CSL_FINST (hMcasp->regs->PDIR, MCASP_PDIR_AXR15, INPUT);
			break;
	}
}

/** ===========================================================================
 *   @n@b CSL_mcaspSetSerIna
 *
 *   @b Description
 *   @n This function sets a particular serializer as inactivated
 *
 *   @b Arguments
 *   @verbatim
            hMcasp          Handle to the McASP instance
            
            serNum          serializer number
     @endverbatim
 *
 *   <b> Return Value </b>  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  Particular serializer will be configured as inactivated
 *
 *   @b  Modifies
 *   @n  SRCTL register
 *
 *   @b Example
 *   @verbatim
        CSL_McaspHandle         hMcasp;
        CSL_McaspSerializerNum  serNum = SERIALIZER_2;
        
        CSL_mcaspSetSerIna (hMcasp, serNum);
        
     @endverbatim
 * ============================================================================
 */
static inline
void CSL_mcaspSetSerIna (
    CSL_McaspHandle         hMcasp,
    CSL_McaspSerializerNum  serNum
)
{
	switch (serNum) 
	{
		case SERIALIZER_1:
			CSL_FINST (hMcasp->regs->SRCTL0, MCASP_SRCTL0_SRMOD, INACTIVE);
			break;

		case SERIALIZER_2:
			CSL_FINST (hMcasp->regs->SRCTL1, MCASP_SRCTL1_SRMOD, INACTIVE);
			break;

		case SERIALIZER_3:
			CSL_FINST (hMcasp->regs->SRCTL2, MCASP_SRCTL2_SRMOD, INACTIVE);
			break;

		case SERIALIZER_4:
			CSL_FINST (hMcasp->regs->SRCTL3, MCASP_SRCTL3_SRMOD, INACTIVE);
			break;

		case SERIALIZER_5:
			CSL_FINST (hMcasp->regs->SRCTL4, MCASP_SRCTL4_SRMOD, INACTIVE);
			break;

		case SERIALIZER_6:
			CSL_FINST (hMcasp->regs->SRCTL5, MCASP_SRCTL5_SRMOD, INACTIVE);
			break;

		case SERIALIZER_7:
			CSL_FINST (hMcasp->regs->SRCTL6, MCASP_SRCTL6_SRMOD, INACTIVE);
			break;

		case SERIALIZER_8:
			CSL_FINST (hMcasp->regs->SRCTL7, MCASP_SRCTL7_SRMOD, INACTIVE);
			break;

		case SERIALIZER_9:
			CSL_FINST (hMcasp->regs->SRCTL8, MCASP_SRCTL8_SRMOD, INACTIVE);
			break;

		case SERIALIZER_10:
			CSL_FINST (hMcasp->regs->SRCTL9, MCASP_SRCTL9_SRMOD, INACTIVE);
			break;

		case SERIALIZER_11:
			CSL_FINST (hMcasp->regs->SRCTL10, MCASP_SRCTL10_SRMOD, INACTIVE);
			break;

		case SERIALIZER_12:
			CSL_FINST (hMcasp->regs->SRCTL11, MCASP_SRCTL11_SRMOD, INACTIVE);
			break;

		case SERIALIZER_13:
			CSL_FINST (hMcasp->regs->SRCTL12, MCASP_SRCTL12_SRMOD, INACTIVE);
			break;

		case SERIALIZER_14:
			CSL_FINST (hMcasp->regs->SRCTL13, MCASP_SRCTL13_SRMOD, INACTIVE);
			break;

		case SERIALIZER_15:
			CSL_FINST (hMcasp->regs->SRCTL14, MCASP_SRCTL14_SRMOD, INACTIVE);
			break;

		case SERIALIZER_16:
			CSL_FINST (hMcasp->regs->SRCTL15, MCASP_SRCTL15_SRMOD, INACTIVE);
			break;
	}
}

/** ===========================================================================
 *   @n@b CSL_mcaspWriteChanStatRam
 *
 *   @b Description
 *   @n This function writes to the Channel status RAM (DITCSRA/B0-5)
 *
 *   @b Arguments
 *   @verbatim
            hMcasp          Handle to the McASP instance
            
            chanStatRam     status to be written
     @endverbatim
 *
 *   <b> Return Value </b>  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  DIT channel status registers 0-5 will be written
 *
 *   @b  Modifies
 *   @n  DITCSRA/B[0-5] register
 *
 *   @b Example
 *   @verbatim
        CSL_McaspHandle         hMcasp;
        CSL_McaspChStatusRam   chanStatRam;
        
        chanStatRam.chStatusLeft[0] = 0x0012345678;
        chanStatRam.chStatusRight[0] = 0x0087654321;
                                                     
        CSL_mcaspWriteChanStatRam (hMcasp, &chanStatRam);
        
     @endverbatim
 * ============================================================================
 */
static inline
void CSL_mcaspWriteChanStatRam (
    CSL_McaspHandle             hMcasp,
    CSL_McaspChStatusRam       *chanStatRam
)
{
	if (hMcasp->ditStatus == TRUE) {
		/* Configure the DIT left channel status registers */
		hMcasp->regs->DITCSRA0 = 
		(Uint32) chanStatRam->chStatusLeft[DIT_REGISTER_0];

		hMcasp->regs->DITCSRA1 = 
		(Uint32) chanStatRam->chStatusLeft[DIT_REGISTER_1];

		hMcasp->regs->DITCSRA2 = 
		(Uint32) chanStatRam->chStatusLeft[DIT_REGISTER_2];

		hMcasp->regs->DITCSRA3 = 
		(Uint32) chanStatRam->chStatusLeft[DIT_REGISTER_3];

		hMcasp->regs->DITCSRA4 = 
		(Uint32) chanStatRam->chStatusLeft[DIT_REGISTER_4];

		hMcasp->regs->DITCSRA5 = 
		(Uint32) chanStatRam->chStatusLeft[DIT_REGISTER_5];

		/* Configure the DIT right channel status registers */
		hMcasp->regs->DITCSRB0 = 
		(Uint32) chanStatRam->chStatusRight[DIT_REGISTER_0];

		hMcasp->regs->DITCSRB1 = 
		(Uint32) chanStatRam->chStatusRight[DIT_REGISTER_1];

		hMcasp->regs->DITCSRB2 = 
		(Uint32) chanStatRam->chStatusRight[DIT_REGISTER_2];

		hMcasp->regs->DITCSRB3 = 
		(Uint32) chanStatRam->chStatusRight[DIT_REGISTER_3];

		hMcasp->regs->DITCSRB4 = 
		(Uint32) chanStatRam->chStatusRight[DIT_REGISTER_4];

		hMcasp->regs->DITCSRB5 = 
		(Uint32) chanStatRam->chStatusRight[DIT_REGISTER_5];
	}
}

/** ===========================================================================
 *   @n@b CSL_mcaspWriteUserDataRam
 *
 *   @b Description
 *   @n This function writes to the User Data RAM (DITUDRA/B0-5)
 *
 *   @b Arguments
 *   @verbatim
            hMcasp          Handle to the McASP instance
            
            userDataRam     data to be written
     @endverbatim
 *
 *   <b> Return Value </b>  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  DIT user data registers 0-5 will be written
 *
 *   @b  Modifies
 *   @n  DITUDRA/B[0-5] register
 *
 *   @b Example
 *   @verbatim
        CSL_McaspHandle        hMcasp;
        CSL_McaspUserDataRam   userStatRam;
        
        userDataRam.userDataLeft[0] = 0x0012345678;
        userDataRam.userDataRight[0] = 0x0087654321;
                                                     
        CSL_mcaspWriteUserDataRam (hMcasp, &userDataRam);
        
     @endverbatim
 * ============================================================================
 */
static inline
void CSL_mcaspWriteUserDataRam (
    CSL_McaspHandle             hMcasp,
    CSL_McaspUserDataRam       *userDataRam
)
{
	if (hMcasp->ditStatus == TRUE) {
		/* Configure the DIT left user data registers */
		hMcasp->regs->DITUDRA0 = 
		(Uint32) userDataRam->userDataLeft[DIT_REGISTER_0];

		hMcasp->regs->DITUDRA1 = 
		(Uint32) userDataRam->userDataLeft[DIT_REGISTER_1];

		hMcasp->regs->DITUDRA2 = 
		(Uint32) userDataRam->userDataLeft[DIT_REGISTER_2];

		hMcasp->regs->DITUDRA3 = 
		(Uint32) userDataRam->userDataLeft[DIT_REGISTER_3];

		hMcasp->regs->DITUDRA4 = 
		(Uint32) userDataRam->userDataLeft[DIT_REGISTER_4];

		hMcasp->regs->DITUDRA5 = 
		(Uint32) userDataRam->userDataLeft[DIT_REGISTER_5];

		/* Configure the DIT right user data registers */
		hMcasp->regs->DITUDRB0 = 
		(Uint32) userDataRam->userDataRight[DIT_REGISTER_0];

		hMcasp->regs->DITUDRB1 = 
		(Uint32) userDataRam->userDataRight[DIT_REGISTER_1];

		hMcasp->regs->DITUDRB2 = 
		(Uint32) userDataRam->userDataRight[DIT_REGISTER_2];

		hMcasp->regs->DITUDRB3 = 
		(Uint32) userDataRam->userDataRight[DIT_REGISTER_3];

		hMcasp->regs->DITUDRB4 = 
		(Uint32) userDataRam->userDataRight[DIT_REGISTER_4];

		hMcasp->regs->DITUDRB5 = 
		(Uint32) userDataRam->userDataRight[DIT_REGISTER_5];
	}
}

/** ===========================================================================
 *   @n@b CSL_mcaspResetXmt
 *
 *   @b Description
 *   @n This function resets the bits related to transmit in XGBLCTL.
 *
 *   @b Arguments
 *   @verbatim
            hMcasp          Handle to the McASP instance
     @endverbatim
 *
 *   <b> Return Value </b>  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  Transmit related bits in XGBLCTL will be reset.
 *
 *   @b  Modifies
 *   @n  XGBLCTL register.
 *
 *   @b Example
 *   @verbatim
        CSL_McaspHandle        hMcasp;
        
        CSL_mcaspResetXmt (hMcasp);
        
     @endverbatim
 * ============================================================================
 */
static inline
void CSL_mcaspResetXmt (
    CSL_McaspHandle     hMcasp
)
{
	CSL_FINST (hMcasp->regs->XGBLCTL, MCASP_XGBLCTL_XCLKRST, RESET);
	CSL_FINST (hMcasp->regs->XGBLCTL, MCASP_XGBLCTL_XHCLKRST, RESET);
	CSL_FINST (hMcasp->regs->XGBLCTL, MCASP_XGBLCTL_XSRCLR, CLEAR);
	CSL_FINST (hMcasp->regs->XGBLCTL, MCASP_XGBLCTL_XSMRST, RESET);
	CSL_FINST (hMcasp->regs->XGBLCTL, MCASP_XGBLCTL_XFRST, RESET);
}

/** ===========================================================================
 *   @n@b CSL_mcaspResetRcv
 *
 *   @b Description
 *   @n This function resets the bits related to receive in RGBLCTL.
 *
 *   @b Arguments
 *   @verbatim
            hMcasp          Handle to the McASP instance
     @endverbatim
 *
 *   <b> Return Value </b>  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  Receive related bits in RGBLCTL will be reset.
 *
 *   @b  Modifies
 *   @n  RGBLCTL register.
 *
 *   @b Example
 *   @verbatim
        CSL_McaspHandle        hMcasp;
        
        CSL_mcaspResetRcv (hMcasp);
        
     @endverbatim
 * ============================================================================
 */
static inline
void CSL_mcaspResetRcv (
    CSL_McaspHandle     hMcasp
)
{
	CSL_FINST (hMcasp->regs->RGBLCTL, MCASP_RGBLCTL_RCLKRST, RESET);
	CSL_FINST (hMcasp->regs->RGBLCTL, MCASP_RGBLCTL_RHCLKRST, RESET);
	CSL_FINST (hMcasp->regs->RGBLCTL, MCASP_RGBLCTL_RSRCLR, CLEAR);
	CSL_FINST (hMcasp->regs->RGBLCTL, MCASP_RGBLCTL_RSMRST, RESET);
	CSL_FINST (hMcasp->regs->RGBLCTL, MCASP_RGBLCTL_RFRST, RESET);
}

/** ===========================================================================
 *   @n@b CSL_mcaspResetSmFsXmt
 *
 *   @b Description
 *   @n This function resets the XFRST and XSMRST bits in XGBLCTL.
 *
 *   @b Arguments
 *   @verbatim
            hMcasp          Handle to the McASP instance
     @endverbatim
 *
 *   <b> Return Value </b>  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  XFRST and XSMRST will be reset in XGBLCTL.
 *
 *   @b  Modifies
 *   @n  XGBLCTL register.
 *
 *   @b Example
 *   @verbatim
        CSL_McaspHandle        hMcasp;
        
        CSL_mcaspResetSmFsXmt (hMcasp);
        
     @endverbatim
 * ============================================================================
 */
static inline
void CSL_mcaspResetSmFsXmt (
    CSL_McaspHandle     hMcasp
)
{
	CSL_FINST (hMcasp->regs->XGBLCTL, MCASP_XGBLCTL_XSMRST, RESET);
	CSL_FINST (hMcasp->regs->XGBLCTL, MCASP_XGBLCTL_XFRST, RESET);
}

/** ===========================================================================
 *   @n@b CSL_mcaspResetSmFsRcv
 *
 *   @b Description
 *   @n This function resets the RFRST and RSMRST bits in RGBLCTL.
 *
 *   @b Arguments
 *   @verbatim
            hMcasp          Handle to the McASP instance
     @endverbatim
 *
 *   <b> Return Value </b>  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  RFRST and RSMRST will be reset in RGBLCTL.
 *
 *   @b  Modifies
 *   @n  RGBLCTL register.
 *
 *   @b Example
 *   @verbatim
        CSL_McaspHandle        hMcasp;
        
        CSL_mcaspResetSmFsRcv (hMcasp);
        
     @endverbatim
 * ============================================================================
 */
static inline
void CSL_mcaspResetSmFsRcv (
    CSL_McaspHandle     hMcasp
)
{
	CSL_FINST (hMcasp->regs->RGBLCTL, MCASP_RGBLCTL_RSMRST, RESET);
	CSL_FINST (hMcasp->regs->RGBLCTL, MCASP_RGBLCTL_RFRST, RESET);
}

/** ===========================================================================
 *   @n@b CSL_mcaspActivateXmtClkSer
 *
 *   @b Description
 *   @n This function sets the bits related to transmit in XGBLCTL.
 *
 *   @b Arguments
 *   @verbatim
            hMcasp          Handle to the McASP instance
     @endverbatim
 *
 *   <b> Return Value </b>  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  Transmit related bits in XGBLCTL will be set.
 *
 *   @b  Modifies
 *   @n  XGBLCTL register.
 *
 *   @b Example
 *   @verbatim
        CSL_McaspHandle        hMcasp;
        
        CSL_mcaspActivateXmtClkSer (hMcasp);
        
     @endverbatim
 * ============================================================================
 */
static inline
void CSL_mcaspActivateXmtClkSer (
    CSL_McaspHandle     hMcasp
)
{
	Uint32 bitValue = 0;

	CSL_FINST (hMcasp->regs->XGBLCTL, MCASP_XGBLCTL_XHCLKRST, ACTIVE);
	while (bitValue != CSL_MCASP_GBLCTL_XHCLKRST_ACTIVE) {
		bitValue = CSL_FEXT (hMcasp->regs->GBLCTL, MCASP_GBLCTL_XHCLKRST);
	}

	CSL_FINST (hMcasp->regs->XGBLCTL, MCASP_XGBLCTL_XCLKRST, ACTIVE);
	{
		bitValue = 0;
		while (bitValue != CSL_MCASP_GBLCTL_XCLKRST_ACTIVE) {
			bitValue = CSL_FEXT (hMcasp->regs->GBLCTL, MCASP_GBLCTL_XCLKRST);
		}
	}

	CSL_FINST (hMcasp->regs->XGBLCTL, MCASP_XGBLCTL_XSRCLR, ACTIVE);
	{
		bitValue = 0;
		while (bitValue != CSL_MCASP_GBLCTL_XSRCLR_ACTIVE) {
			bitValue = CSL_FEXT (hMcasp->regs->GBLCTL, MCASP_GBLCTL_XSRCLR);
		}
	}
}

/** ===========================================================================
 *   @n@b CSL_mcaspActivateRcvClkSer
 *
 *   @b Description
 *   @n This function sets the bits related to receive in RGBLCTL.
 *
 *   @b Arguments
 *   @verbatim
            hMcasp          Handle to the McASP instance
     @endverbatim
 *
 *   <b> Return Value </b>  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  Receive related bits in RGBLCTL will be set.
 *
 *   @b  Modifies
 *   @n  RGBLCTL register.
 *
 *   @b Example
 *   @verbatim
        CSL_McaspHandle        hMcasp;
        
        CSL_mcaspActivateRcvClkSer (hMcasp);
        
     @endverbatim
 * ============================================================================
 */
static inline
void CSL_mcaspActivateRcvClkSer (
    CSL_McaspHandle     hMcasp
)
{
	Uint32 bitValue = 0;

	CSL_FINST (hMcasp->regs->RGBLCTL, MCASP_RGBLCTL_RHCLKRST, ACTIVE);
	while (bitValue != CSL_MCASP_GBLCTL_RHCLKRST_ACTIVE) {
		bitValue = CSL_FEXT (hMcasp->regs->GBLCTL, MCASP_GBLCTL_RHCLKRST);
	}

	CSL_FINST (hMcasp->regs->RGBLCTL, MCASP_RGBLCTL_RCLKRST, ACTIVE);
	{
		bitValue = 0;
		while (bitValue != CSL_MCASP_GBLCTL_RCLKRST_ACTIVE) {
			bitValue = CSL_FEXT (hMcasp->regs->GBLCTL, MCASP_GBLCTL_RCLKRST);
		}
	}

	CSL_FINST (hMcasp->regs->RGBLCTL, MCASP_RGBLCTL_RSRCLR, ACTIVE);
	{
		bitValue = 0;
		while (bitValue != CSL_MCASP_GBLCTL_RSRCLR_ACTIVE) {
			bitValue = CSL_FEXT (hMcasp->regs->GBLCTL, MCASP_GBLCTL_RSRCLR);
		}
	}
}

/** ===========================================================================
 *   @n@b CSL_mcaspActivateSmRcvXmt
 *
 *   @b Description
 *   @n This function sets the RSMRST and XSMRST bits in GBLCTL.
 *
 *   @b Arguments
 *   @verbatim
            hMcasp          Handle to the McASP instance
     @endverbatim
 *
 *   <b> Return Value </b>  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  RSMRST and XSMRST will be set in GBLCTL.
 *
 *   @b  Modifies
 *   @n  GBLCTL register.
 *
 *   @b Example
 *   @verbatim
        CSL_McaspHandle        hMcasp;
        
        CSL_mcaspActivateSmRcvXmt (hMcasp);
        
     @endverbatim
 * ============================================================================
 */
static inline
void CSL_mcaspActivateSmRcvXmt (
    CSL_McaspHandle     hMcasp
)
{
	CSL_BitMask16   selectMask;

	selectMask = CSL_MCASP_GBLCTL_RSMRST_MASK |
		CSL_MCASP_GBLCTL_XSMRST_MASK;

	hMcasp->regs->GBLCTL = (hMcasp->regs->GBLCTL | selectMask);
	while ((hMcasp->regs->GBLCTL & selectMask) != selectMask);
}

/** ===========================================================================
 *   @n@b CSL_mcaspActivateFsRcvXmt
 *
 *   @b Description
 *   @n This function resets the RFRST and XFRST bits in GBLCTL.
 *
 *   @b Arguments
 *   @verbatim
            hMcasp          Handle to the McASP instance
     @endverbatim
 *
 *   <b> Return Value </b>  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  RFRST and XFRST will be reset in GBLCTL.
 *
 *   @b  Modifies
 *   @n  GBLCTL register.
 *
 *   @b Example
 *   @verbatim
        CSL_McaspHandle        hMcasp;
        
        CSL_mcaspActivateFsRcvXmt (hMcasp);
        
     @endverbatim
 * ============================================================================
 */
static inline
void CSL_mcaspActivateFsRcvXmt (
    CSL_McaspHandle     hMcasp
)
{
	CSL_BitMask16   selectMask;

	selectMask = CSL_MCASP_GBLCTL_RFRST_MASK |
		CSL_MCASP_GBLCTL_XFRST_MASK;

	hMcasp->regs->GBLCTL = (hMcasp->regs->GBLCTL | selectMask);
	while ((hMcasp->regs->GBLCTL & selectMask) != selectMask);
}

/** ===========================================================================
 *   @n@b CSL_mcaspSetDitMode
 *
 *   @b Description
 *   @n This function enables/disables the DIT mode.
 *
 *   @b Arguments
 *   @verbatim
            hMcasp          Handle to the McASP instance
            
            ditFlag         Value to loaded into the DITCTL register
     @endverbatim
 *
 *   <b> Return Value </b>  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  DIT mode ill be either enabled/disabled.
 *
 *   @b  Modifies
 *   @n  DITCTL register.
 *
 *   @b Example
 *   @verbatim
        CSL_McaspHandle     hMcasp;
        Bool                ditFlag = TRUE;
        
        CSL_mcaspSetDitMode (hMcasp, ditMode);
        
     @endverbatim
 * ============================================================================
 */
static inline
void CSL_mcaspSetDitMode (
    CSL_McaspHandle     hMcasp,
    Bool                ditFlag
)
{
	if (hMcasp->ditStatus == TRUE) {
		/* Reset XSMRST and XFRST in GBLCTL */
		CSL_FINST (hMcasp->regs->GBLCTL, MCASP_GBLCTL_XSMRST, RESET);
		CSL_FINST (hMcasp->regs->GBLCTL, MCASP_GBLCTL_XFRST, RESET);

		/* Configure DITEN bit of DITCTL */
		CSL_FINS (hMcasp->regs->DITCTL, MCASP_DITCTL_DITEN, ditFlag);
	}
}

#ifdef __cplusplus
extern "C" {
#endif

#endif /* CSL_MCASPAUX_H_ */
