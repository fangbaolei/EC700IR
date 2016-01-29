/*  ============================================================================
 *   Copyright (c) Texas Instruments Inc 2002, 2003, 2004, 2005
 *
 *   Use of this software is controlled by the terms and conditions found
 *   in the license agreement under which this software has been supplied.
 *  ============================================================================
 */
/** ============================================================================
 *   @file    csl_utopia2.h
 *
 *   @path    $(CSLPATH)\inc
 *
 *   @desc    This files contains the macros and inline function for UTOPIA2.
 *
 */

/*  ============================================================================
 *  Revision History
 *  ================
 *  15-Feb-2006 ds     - Added #undef  IDECL and #undef  IDEF
 *
 *  ============================================================================
 */

#ifndef _CSL_UTOPIA2_H_
#define _CSL_UTOPIA2_H_

#include <cslr_utopia2.h>
#include <soc.h>

/******************************************************************************\
* scope and inline control macros
\******************************************************************************/
#ifdef __cplusplus
#define CSLAPI extern "C" far 
#else
#define CSLAPI extern far 
#endif

#undef  IDECL
#undef  IDEF

#define IDECL static inline
#define IDEF  static inline

/******************************************************************************\
* global macro declarations
\******************************************************************************/

/* utopia2 interrupt numbers */
/** Interrupt for Transmit queue */
#define UTOPIA2_INT_XQ     0
/** Interrupt for Receive queue */
#define UTOPIA2_INT_RQ     16

/* utopia2 error interrupt numbers */
/** Receive queue stall interrupt enable bit. */
#define UTOPIA2_ERR_RQS    0
/** Receive clock failed interrupt enable bit. */
#define UTOPIA2_ERR_RCF    1
/** Receive clock present interrupt enable bit. */
#define UTOPIA2_ERR_RCP    2
/** Transmit queue stall interrupt enable bit. */
#define UTOPIA2_ERR_XQS    16
/** Transmit clock failed interrupt enable bit. */
#define UTOPIA2_ERR_XCF    17
/** Transmit clock present interrupt enable bit. */
#define UTOPIA2_ERR_XCP    18

/** Base address of the UTOPIA2 receive queue */
#define UTOPIA2_RCVQ_ADDR  CSL_UTOPIA2_RX_EDMA_REGS 
/** Base address of the UTOPIA2 transmit queue */
#define UTOPIA2_XMTQ_ADDR  CSL_UTOPIA2_TX_EDMA_REGS 

/** Default value of UCR */
#define UTOPIA2_UCR_DEFAULT    (0x00000000u)
/** Default value of CDR */
#define UTOPIA2_CDR_DEFAULT    (0x00FF00FFu)

/******************************************************************************\
* global typedef declarations
\******************************************************************************/

/* device configuration structure */
/**
 * @brief   The Config structure
 *
 * Used to configure the UTOPIA2 using utopia2_config(ucr,cdr);
 */
typedef struct {
    /** UTOPIA2 Control Register */
    Uint32 ucr;
    /** Clock Detect Register of UTOPIA2 */
    Uint32 cdr;
} UTOPIA2_Config;

/******************************************************************************\
* global variable declarations
\******************************************************************************/
extern CSL_Utopia2Regs *utopia2Regs;

/******************************************************************************\
* global function declarations
\******************************************************************************/
CSLAPI void UTOPIA2_reset();

/******************************************************************************\
* inline function definitions
\******************************************************************************/
/** ============================================================================
 *   @n@b UTOPIA2_getXmtAddr
 *
 *   @b Description
 *   @n This function is to get the transmit address of UTOPIA2.  This address 
 *      is needed to write to the Transmit Port. 
 *
 *   @b Arguments
 *   @n None
 *
 *   <b> Return Value </b>  
 *   @li                        val - address of transmit queue
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
            Uint32 utopXmtAddr;
            utopXmtAddr = UTOPIA2_getXmtAddr();
     @endverbatim
 * =============================================================================
 */
IDEF 
Uint32 UTOPIA2_getXmtAddr (
    void
)
{
    return (Uint32)(UTOPIA2_XMTQ_ADDR);
}

/** ============================================================================
 *   @n@b UTOPIA2_getRcvAddr
 *
 *   @b Description
 *   @n This function is to get the receive address of UTOPIA2.  This address 
 *      is required to read from the Receiver Port.
 *
 *   @b Arguments
 *   @n None
 *
 *   <b> Return Value </b>  
 *   @li                        val - address of receive queue
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
            Uint32 utopRcvAddr;
            utopRcvAddr = UTOPIA2_getRcvAddr();
     @endverbatim
 * =============================================================================
 */
IDEF 
Uint32 UTOPIA2_getRcvAddr (
    void
)
{
    return (Uint32)(UTOPIA2_RCVQ_ADDR);
}

/** ============================================================================
 *   @n@b UTOPIA2_getEventId
 *
 *   @b Description
 *   @n This function is to get the event Id associated to the 
 *      UTOPIA2 CPU-interrupt Id.
 *
 *   @b Arguments
 *   @n  None
 *
 *   <b> Return Value </b>  
 *   @li                        val - Event Id of UTOPIA2
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
            Uint32 utopEventId;
            utopEventId = UTOPIA2_getEventId();
     @endverbatim
 * =============================================================================
 */
IDEF 
Uint32 UTOPIA2_getEventId (
    void
)
{
    return (CSL_INTC_EVENTID_UINT);
}

/** ============================================================================
 *   @n@b UTOPIA2_read
 *
 *   @b Description
 *   @n Reads data from the receive queue of UTOPIA2.
 *
 *   @b Arguments
 *   @n  None
 *
 *   <b> Return Value </b>  
 *   @li                        val - Data from the receive queue.
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
            Uint32 utopRxData;
            utopRxData = UTOPIA2_read();
     @endverbatim
 * =============================================================================
 */
IDEF 
Uint32 UTOPIA2_read (
    void
)
{
    return (*(volatile Uint32*)(UTOPIA2_RCVQ_ADDR));
}

/** ============================================================================
 *   @n@b UTOPIA2_write
 *
 *   @b Description
 *   @n Writes data into the transmit queue of UTOPIA2.
 *
 *   @b Arguments
 *   @verbatim
        val - Value to be written into transmit queue
        

     @endverbatim
 *
 *
 *   <b> Return Value </b>  
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  Value passed is written at transmit address of UTOPIA2 i.e., 
 *       UTOPIA2_XMTQ_ADDR.
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim
            Uint32 utopTxData = 0x1111FFFF;
            UTOPIA2_write(utopTxData);
     @endverbatim
 * =============================================================================
 */
IDEF 
void UTOPIA2_write (
    Uint32    val
)
{
    (*(volatile Uint32*)(UTOPIA2_XMTQ_ADDR)) = val;
}

/** ============================================================================
 *   @n@b UTOPIA2_enableXmt
 *
 *   @b Description
 *   @n Enables transmitter port
 *
 *   @b Arguments
 *   @n None
 *
 *   <b> Return Value </b>  
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n Modifies the UXEN bit of UCR register
 *
 *   @b Example
 *   @verbatim
             // Configure UTOPIA2
             UTOPIA2_configArgs(0x00040004, // ucr
                             0x00FF00FF // cdr);
             .....
             .....
             //Enables Transmitter port
             UTOPIA2_enableXmt();
     @endverbatim
 * =============================================================================
 */
IDEF 
void UTOPIA2_enableXmt (
    void
)
{
    CSL_FINST(utopia2Regs->UCR, UTOPIA2_UCR_UXEN, TX_PORT_ENABLE);
}

/** ============================================================================
 *   @n@b UTOPIA2_enableRcv
 *
 *   @b Description
 *   @n Enables the reciever port
 *
 *   @b Arguments
 *   @n  None
 *
 *   <b> Return Value </b>  
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n  Modifies the UREN bit of UCR register
 *
 *   @b Example
 *   @verbatim
             // Configure UTOPIA2
             UTOPIA2_configArgs(0x00040004, // ucr
                             0x00FF00FF // cdr);
             .....
             .....
             //Enables Receiver port
             UTOPIA2_enableRcv();
     @endverbatim
 * =============================================================================
 */
IDEF 
void UTOPIA2_enableRcv (
    void
)
{
    CSL_FINST(utopia2Regs->UCR, UTOPIA2_UCR_UREN, RX_PORT_ENABLE);
}

/** ============================================================================
 *   @n@b UTOPIA2_errDisable
 *
 *   @b Description
 *   @n Disables the error interrupt event.
 *
 *   @b Arguments
 *   @verbatim
             errNum - Error condition ID 

             The following are the possible errors from EIPR
                 - UTOPIA2_ERR_RQS
                 - UTOPIA2_ERR_RCF
                 - UTOPIA2_ERR_RCP
                 - UTOPIA2_ERR_XQS
                 - UTOPIA2_ERR_XCF
                 - UTOPIA2_ERR_XCP
        
     @endverbatim
 *
 *   <b> Return Value </b>  
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n  Clears the transmit clock fail bit of EIER register.
 *
 *   @b Example
 *   @verbatim
             // disables the transmit clock fail error bit.
             UTOPIA2_errDisable(UTOPIA2_ERR_XCF);
     @endverbatim
 * =============================================================================
 */
IDEF 
void UTOPIA2_errDisable (
    Uint32    errNum
)
{
    (utopia2Regs->EIER) = ((utopia2Regs->EIER) & (~(1<<errNum)));
}

/** ============================================================================
 *   @n@b UTOPIA2_errEnable
 *
 *   @b Description
 *   @n Enables the bit of given error condition ID of EIPR.
 *
 *   @b Arguments
 *   @verbatim
             errNum - Error condition ID 

             The following are the possible errors from EIPR
                 - UTOPIA2_ERR_RQS
                 - UTOPIA2_ERR_RCF
                 - UTOPIA2_ERR_RCP
                 - UTOPIA2_ERR_XQS
                 - UTOPIA2_ERR_XCF
                 - UTOPIA2_ERR_XCP
        
     @endverbatim
 *
 *   <b> Return Value </b>  
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n  Sets the transmit clock fail bit of EIER register
 *
 *   @b Example
 *   @verbatim
             // Enables the transmit clock fail error bit.
             UTOPIA2_errEnable(UTOPIA2_ERR_XCF);
     @endverbatim
 * =============================================================================
 */
IDEF 
void UTOPIA2_errEnable (
    Uint32    errNum
)
{
    (utopia2Regs->EIER) = ((utopia2Regs->EIER) | (1<<errNum));
}

/** ============================================================================
 *   @n@b UTOPIA2_errClear
 *
 *   @b Description
 *   @n Clears the bit of given error condition ID of EIPR.
 *
 *   @b Arguments
 *   @verbatim
             errNum - Error condition ID 

             The following are the possible errors from EIPR
                 - UTOPIA2_ERR_RQS
                 - UTOPIA2_ERR_RCF
                 - UTOPIA2_ERR_RCP
                 - UTOPIA2_ERR_XQS
                 - UTOPIA2_ERR_XCF
                 - UTOPIA2_ERR_XCP
        
     @endverbatim
 *
 *   <b> Return Value </b>  
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n Clears the transmit clock fail bit of EIPR register
 *
 *   @b Example
 *   @verbatim
             // clears the transmit clock fail error bit.
             UTOPIA2_errClear(UTOPIA2_ERR_XCF);
     @endverbatim
 * =============================================================================
 */
IDEF 
void UTOPIA2_errClear (
    Uint32    errNum
)
{
    (utopia2Regs->EIPR) = ((utopia2Regs->EIPR) & (~(1<<errNum)));
}

/** ============================================================================
 *   @n@b UTOPIA2_errTest
 *
 *   @b Description
 *   @n Checks the error status of given error number.
 *
 *   @b Arguments
 *   @verbatim
             errNum - Error condition ID 

             The following are the possible errors from EIPR
                 - UTOPIA2_ERR_RQS
                 - UTOPIA2_ERR_RCF
                 - UTOPIA2_ERR_RCP
                 - UTOPIA2_ERR_XQS
                 - UTOPIA2_ERR_XCF
                 - UTOPIA2_ERR_XCP
        
     @endverbatim
 *
 *   <b> Return Value </b>  
 *   @n  None
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
             // checking for the transmit clock fail error bit.
             Uint32 errDetect;
             UTOPIA2_errEnable(UTOPIA2_ERR_RCF);
             errNum = UTOPIA2_errTest(UTOPIA2_ERR_RCF)
     @endverbatim
 * =============================================================================
 */
IDEF 
Uint32 UTOPIA2_errTest (
    Uint32    errNum
)
{
	return ((utopia2Regs->EIPR & (1<<errNum)) ? 1 : 0);
}

/** ============================================================================
 *   @n@b UTOPIA2_errReset
 *
 *   @b Description
 *   @n Disables and clears the error interrupt bit associated to the 
 *           given error number.
 *
 *   @b Arguments
 *   @verbatim
             errNum - Error condition ID 

             The following are the possible errors from EIPR
                 - UTOPIA2_ERR_RQS
                 - UTOPIA2_ERR_RCF
                 - UTOPIA2_ERR_RCP
                 - UTOPIA2_ERR_XQS
                 - UTOPIA2_ERR_XCF
                 - UTOPIA2_ERR_XCP
        
     @endverbatim
 *
 *   <b> Return Value </b>  
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n Clears the specified bit of EIPR register.
 *
 *   @b Example
 *   @verbatim
             // disables & clears the transmit clock fail error bit.
             UTOPIA2_errReset(UTOPIA2_ERR_XCF);
     @endverbatim
 * =============================================================================
 */
IDEF 
void UTOPIA2_errReset (
    Uint32    errNum
)
{
    UTOPIA2_errDisable(errNum);
    UTOPIA2_errClear(errNum);
}

/** ============================================================================
 *   @n@b UTOPIA2_config
 *
 *   @b Description
 *   @n Sets up configuration to use the UTOPIA2.  The values are set
 *           to the UTOPIA2 register (UCR, CDR).
 *
 *   @b Arguments
 *   @verbatim
             config - Pointer to an initialized configuration structure

     @endverbatim
 *
 *   <b> Return Value </b>  
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n UCR and CDR registers of UTOPIA2
 *
 *   @b Example
 *   @verbatim
             UTOPIA2_Config utopConfig = {
                                     0x00000000, // ucr 
                                     0x00FF00FF  // cdr };
             ........
             ........
             UTOPIA2_config(&utopConfig);
     @endverbatim
 * =============================================================================
 */
IDEF 
void UTOPIA2_config (
    UTOPIA2_Config    *config
)
{

    Uint32 cs;
    register int ucrVal, cdrVal;

    CSL_Utopia2Regs *base = (CSL_Utopia2Regs *) CSL_UTOPIA2_0_REGS;

    cs = _disable_interrupts();

    /* the compiler generates more efficient code if the loads */
    /* and stores are grouped together rather than intermixed  */
    ucrVal = config->ucr;
    cdrVal = config->cdr;

    base->UCR = UTOPIA2_UCR_DEFAULT;
    base->CDR = cdrVal;
    base->UCR = ucrVal; 
    /* Enable interface after everything is set up */
    _restore_interrupts(cs);
}

/** ============================================================================
 *   @n@b UTOPIA2_configArgs
 *
 *   @b Description
 *   @n Sets up the UTOPIA2 mode by writing the registers which is passed in.
 *
 *   @b Arguments
 *   @verbatim
             ucr - Utopia2 Control Register value
             cdr - Clock Detect Register value

     @endverbatim
 *
 *   <b> Return Value </b>  
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n UCR and CDR registers of UTOPIA2
 *
 *   @b Example
 *   @verbatim
             UTOPIA2_configArgs(0x00000000, // ucr 
                             0x00FF00FF  // cdr);
     @endverbatim
 * =============================================================================
 */
IDEF 
void UTOPIA2_configArgs (
    Uint32    ucr, 
    Uint32    cdr
)
{

    Uint32 cs;
    CSL_Utopia2Regs *base = (CSL_Utopia2Regs *) CSL_UTOPIA2_0_REGS;

    cs = _disable_interrupts();

    base->UCR = UTOPIA2_UCR_DEFAULT;
    base->CDR = cdr;
    base->UCR = ucr; 
    /* Enable interface after everything is set up */
    _restore_interrupts(cs);
}

/** ============================================================================
 *   @n@b UTOPIA2_getConfig
 *
 *   @b Description
 *   @n Reads the configuration values into the config structure.
 *
 *   @b Arguments
 *   @verbatim
             config - Pointer to a configuration structure.

     @endverbatim
 *
 *   <b> Return Value </b>  
 *   @n  None
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
             UTOPIA2_config utopConfig;
             UTOPIA2_getConfig(&utopConfig);
     @endverbatim
 * =============================================================================
 */
IDEF 
void UTOPIA2_getConfig (
    UTOPIA2_Config    *config
)
{
    Uint32 cs;
    CSL_Utopia2Regs *base = (CSL_Utopia2Regs *) CSL_UTOPIA2_0_REGS;
    register int ucrVal, cdrVal;

    cs = _disable_interrupts();

    /* the compiler generates more efficient code if the loads */
    /* and stores are grouped together rather than intermixed  */
    ucrVal = base->UCR;
    cdrVal = base->CDR;
  
    config->ucr = ucrVal;
    config->cdr = cdrVal;

    _restore_interrupts(cs);
}

#endif /* _CSL_UTOPIA2_H_ */
/******************************************************************************\
* End of csl_utopia2.h
\******************************************************************************/

