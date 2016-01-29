/*  ===========================================================================
 *   Copyright (c) Texas Instruments Inc 2002, 2003, 2004, 2005
 *
 *   Use of this software is controlled by the terms and conditions found in the
 *   license agreement under which this software has been supplied.
 *   ===========================================================================
 */
 
/** ============================================================================
 *  @file csl_i2cAux.h
 *
 *  @brief Header file for functional layer of CSL
 *
 *  @path $(CSLPATH)\inc
 *
 *  Description
 *    - The defines inline function definitions for control commands & status
 *       queris
 *  ===========================================================================
 */

/* =============================================================================
 *  Revision History
 *  ===============
 *  31-Aug-2004 Hs File Created from CSL_i2cHwControl.c.
 *  01-Sep-2004 Hs Added inline functions for query commands also.  
 *  11-oct-2004 Hs updated according to code review comments.
 *  28-jul-2005 sv removed gpio support 
 *  06-Feb-2006 ds Updated according to TCI6482/C6455 User Guide
 *  ===========================================================================
 */

#ifndef _CSL_I2CAUX_H_
#define _CSL_I2CAUX_H_

#include <csl_i2c.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  Control Functions of i2c.
 */
/** ============================================================================
 *   @n@b CSL_i2cEnable
 *
 *   @b Description
 *   @n This function enables the I2C module.
 *
 *   @b Arguments
 *   @verbatim

            hI2c            Handle to I2C instance

     @endverbatim
 *
 *   <b> Return Value </b>  None
 *
 *   <b> Pre Condition </b>
 *   @n  CSL_i2cInit(),CSL_i2cOpen()has to be called successfully before calling
 *       this function
 *
 *   <b> Post Condition </b>
 *    @n None
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim

        CSL_i2cEnable(hI2c);

     @endverbatim
 *  ============================================================================
 */
static inline
void CSL_i2cEnable (
    /*
     * Pointer to the handle of I2C 
     */
    CSL_I2cHandle hI2c
)
{
    CSL_FINS(hI2c->regs->ICMDR, I2C_ICMDR_IRS, FALSE);
    CSL_FINS(hI2c->regs->ICMDR, I2C_ICMDR_IRS, TRUE);
}


/** ============================================================================
 *   @n@b CSL_i2cReset
 *
 *   @b Description
 *   @n This function resets the I2C module. provides software reset
 *
 *   @b Arguments
 *   @verbatim

            hI2c            Handle to I2C instance

     @endverbatim
 *
 *   <b> Return Value </b>  None
 *
 *   <b> Pre Condition </b>
 *   @n  CSL_i2cInit(),CSL_i2cOpen()has to be called successfully before calling
 *       this function
 *
 *   <b> Post Condition </b>
 *    @n None
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim

        CSL_i2cReset(hI2c);

     @endverbatim
 *  ============================================================================
 */
static inline
void CSL_i2cReset (
    /*
     * Pointer to the handle of I2C 
     */
    CSL_I2cHandle hI2c
)
{
    CSL_FINS(hI2c->regs->ICMDR, I2C_ICMDR_IRS, FALSE);
}

/** ============================================================================
 *   @n@b CSL_i2cOutOfReset
 *
 *   @b Description
 *   @n This function pulls the I2C out of reset.
 *
 *   @b Arguments
 *   @verbatim

            hI2c            Handle to I2C instance

     @endverbatim
 *
 *   <b> Return Value </b>  None
 *
 *   <b> Pre Condition </b>
 *   @n  CSL_i2cInit(),CSL_i2cOpen()has to be called successfully before calling
 *       this function
 *
 *   <b> Post Condition </b>
 *    @n None
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim

        CSL_i2cOutOfReset(hI2c);

     @endverbatim
 *  ============================================================================
 */
static inline
void CSL_i2cOutOfReset (
    /*
     * Pointer to the handle of I2C 
     */
    CSL_I2cHandle hI2c
)
{
    CSL_FINS(hI2c->regs->ICMDR, I2C_ICMDR_IRS, TRUE);
}


/** ============================================================================
 *   @n@b CSL_i2cClearStatus
 *
 *   @b Description
 *   @n This function clears the status register of I2C.
 *
 *   @b Arguments
 *   @verbatim

            hI2c            Handle to I2C instance

     @endverbatim
 *
 *   <b> Return Value </b>  None
 *
 *   <b> Pre Condition </b>
 *   @n  CSL_i2cInit(),CSL_i2cOpen()has to be called successfully before calling
 *       this function
 *
 *   <b> Post Condition </b>
 *    @n None
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim

        CSL_i2cClearStatus(hI2c);

     @endverbatim
 *  ============================================================================
 */
static inline
void CSL_i2cClearStatus (
    /*
     *  Pointer to the handle of I2C 
     */
    CSL_I2cHandle hI2c
)
{
    Uint32 temp_stat = 0x0;
    Uint32 temp_stat_old = 0x0;

    /* Multiple status bits can be cleared. */

    temp_stat_old = hI2c->regs->ICSTR & 0x3F;

    if (temp_stat_old & CSL_I2C_CLEAR_AL) {
        temp_stat = CSL_FMK(I2C_ICSTR_AL, 1);         /* Writing 1 clears bit*/
    }
    if (temp_stat_old & CSL_I2C_CLEAR_NACK) {
        temp_stat = CSL_FMK(I2C_ICSTR_NACK, 1);       /* Writing 1 clears bit*/
    }
    if (temp_stat_old & CSL_I2C_CLEAR_ARDY) {
            temp_stat = CSL_FMK(I2C_ICSTR_ARDY, 1) ; /* Writing 1 clears bit*/
    }
    if (temp_stat_old & CSL_I2C_CLEAR_RRDY) {
        temp_stat = CSL_FMK(I2C_ICSTR_ICRRDY, 1) ;     /* Writing 1 clears bit*/
    }
    if (temp_stat_old & CSL_I2C_CLEAR_XRDY) {
        temp_stat = CSL_FMK(I2C_ICSTR_ICXRDY, 1) ;     /* Writing 1 clears bit*/
    }
    if (temp_stat_old & CSL_I2C_CLEAR_SCD) {
        temp_stat = CSL_FMK(I2C_ICSTR_SCD, 1) ;        /* Writing 1 clears bit*/
    }

    hI2c->regs->ICSTR = hI2c->regs->ICSTR | (temp_stat & 0x3F);
}


/** ============================================================================
 *   @n@b CSL_i2cSetSlaveAddr
 *
 *   @b Description
 *   @n This function sets the slave address of I2C module.
 *
 *   @b Arguments
 *   @verbatim

            hI2c            Handle to I2C instance
            
            arg             pointer to hold the slave address

     @endverbatim
 *
 *   <b> Return Value </b>  None
 *
 *   <b> Pre Condition </b>
 *   @n  CSL_i2cInit(),CSL_i2cOpen()has to be called successfully before calling
 *       this function
 *
 *   <b> Post Condition </b>
 *    @n None
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim

        CSL_i2cSetSlaveAddr(hI2c, &arg);

     @endverbatim
 *  ============================================================================
 */
static inline
void CSL_i2cSetSlaveAddr (
    /*
     * Pointer to the handle of I2C 
     */
    CSL_I2cHandle hI2c,

    /* Arguments if any for the command */
    void          *arg
)
{
    hI2c->regs->ICSAR = ((*(Uint32 *)arg) & 0x3FF);
}


/** ============================================================================
 *   @n@b CSL_i2cSetOwnAddr
 *
 *   @b Description
 *   @n This function sets its own address.
 *
 *   @b Arguments
 *   @verbatim

            hI2c            Handle to I2C instance
            
            arg             pointer to hold the own address

     @endverbatim
 *
 *   <b> Return Value </b>  None
 *
 *   <b> Pre Condition </b>
 *   @n  CSL_i2cInit(), CSL_i2cOpen()has to be called successfully before calling
 *       this function
 *
 *   <b> Post Condition </b>
 *    @n None
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim

        CSL_i2cSetOwnAddr(hI2c, &arg);

     @endverbatim
 *  ============================================================================
 */
static inline
void CSL_i2cSetOwnAddr (
    /*
     * Pointer to the handle of I2C 
     */
    CSL_I2cHandle hI2c,

    /* Arguments if any for the command */
    void          *arg
)
{
    hI2c->regs->ICOAR = (*(Uint32 *)arg);
}


/** ============================================================================
 *   @n@b CSL_i2cEnableIntr
 *
 *   @b Description
 *   @n This function enables selected interrupts of I2C module.
 *
 *   @b Arguments
 *   @verbatim

            hI2c            Handle to I2C instance
            
            arg             OR-ed value of interrupts

     @endverbatim
 *
 *   <b> Return Value </b>  None
 *
 *   <b> Pre Condition </b>
 *   @n  CSL_i2cInit(),CSL_i2cOpen()has to be called successfully before calling
 *       this function
 *
 *   <b> Post Condition </b>
 *    @n None
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim

        CSL_i2cEnableIntr(hI2c, arg);

     @endverbatim
 *  ============================================================================
 */
static inline
void CSL_i2cEnableIntr (
    /*
     * Pointer to the handle of I2C 
     */
    CSL_I2cHandle hI2c,

    /* Arguments if any for the command */
    Uint32        arg
)
{
    hI2c->regs->ICIMR = (hI2c->regs->ICIMR | (arg));
}


/** ============================================================================
 *   @n@b CSL_i2cDisableIntr
 *
 *   @b Description
 *   @n This function disables selected interrupts of I2C module.
 *
 *   @b Arguments
 *   @verbatim

            hI2c            Handle to I2C instance
            
            arg             OR-ed value of interrupts

     @endverbatim
 *
 *   <b> Return Value </b>  None
 *
 *   <b> Pre Condition </b>
 *   @n  CSL_i2cInit(),CSL_i2cOpen()has to be called successfully before calling
 *       this function
 *
 *   <b> Post Condition </b>
 *    @n None
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim

        CSL_i2cDisableIntr(hI2c, arg);

     @endverbatim
 *  ============================================================================
 */
static inline
void CSL_i2cDisableIntr (
    /*
     * Pointer to the handle of I2C 
     */
    CSL_I2cHandle hI2c,

    /* Arguments if any for the command */
    Uint32        arg
)
{
    hI2c->regs->ICIMR = (hI2c->regs->ICIMR & ~(arg));
}


/** ============================================================================
 *   @n@b CSL_i2cSetDataCount
 *
 *   @b Description
 *   @n This function sets the data count of the I2C module.
 *
 *   @b Arguments
 *   @verbatim

            hI2c            Handle to I2C instance
            
            arg             pointer to hold the data count

     @endverbatim
 *
 *   <b> Return Value </b>  None
 *
 *   <b> Pre Condition </b>
 *   @n  CSL_i2cInit(),CSL_i2cOpen()has to be called successfully before calling
 *       this function
 *
 *   <b> Post Condition </b>
 *    @n None
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim

        CSL_i2cSetDataCount(hI2c, &arg);

     @endverbatim
 *  ============================================================================
 */
static inline
void CSL_i2cSetDataCount (
    /*
     * Pointer to the handle of I2C 
     */
    CSL_I2cHandle hI2c,

    /* Arguments if any for the command */
    void          *arg
)
{
    hI2c->regs->ICCNT = (*(Uint32 *)arg) & 0xFFFF;
}


/** ============================================================================
 *   @n@b CSL_i2cSetClock
 *
 *   @b Description
 *   @n This function sets the I2C Clock.
 *
 *   @b Arguments
 *   @verbatim

            hI2c       Handle to I2C instance

            arg        address of ClkSetup structure.

     @endverbatim
 *
 *   <b> Return Value </b>  None
 *
 *   <b> Pre Condition </b>
 *   @n  CSL_i2cInit(),CSL_i2cOpen()has to be called successfully before calling
 *       this function
 *
 *   <b> Post Condition </b>
 *    @n None
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim

        CSL_i2cSetClockSetup(hI2c, &arg);

     @endverbatim
 *  ============================================================================
 */
static inline
void CSL_i2cSetClock (
    /*
     * Pointer to the handle of I2C 
     */
    CSL_I2cHandle   hI2c,

    /** Placeholder to return the status. @a void* casted */
    CSL_I2cClkSetup *arg
)
{
    CSL_FINS(hI2c->regs->ICPSC, I2C_ICPSC_IPSC, arg->prescalar);
    CSL_FINS(hI2c->regs->ICCLKL, I2C_ICCLKL_ICCL, arg->clklowdiv);
    CSL_FINS(hI2c->regs->ICCLKH, I2C_ICCLKH_ICCH, arg->clkhighdiv);
}


/** ============================================================================
 *   @n@b CSL_i2cStart
 *
 *   @b Description
 *   @n This function writes the start command to the I2C module.
 *
 *   @b Arguments
 *   @verbatim

            hI2c            Handle to I2C instance

     @endverbatim
 *
 *   <b> Return Value </b>  None
 *
 *   <b> Pre Condition </b>
 *   @n  CSL_i2cInit(),CSL_i2cOpen()has to be called successfully before calling
 *       this function
 *
 *   <b> Post Condition </b>
 *    @n None
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim

        CSL_i2cStart(hI2c);

     @endverbatim
 *  ============================================================================
 */
static inline
void CSL_i2cStart (
    /*
     * Pointer to the handle of I2C 
     */
    CSL_I2cHandle hI2c
)
{
    CSL_FINS(hI2c->regs->ICMDR, I2C_ICMDR_STT, TRUE);
}


/** ============================================================================
 *   @n@b CSL_i2cStop
 *
 *   @b Description
 *   @n This function writes the stop command to the I2C module.
 *
 *   @b Arguments
 *   @verbatim

            hI2c            Handle to I2C instance

     @endverbatim
 *
 *   <b> Return Value </b>  None
 *
 *   <b> Pre Condition </b>
 *   @n  CSL_i2cInit(),CSL_i2cOpen()has to be called successfully before calling
 *       this function
 *
 *   <b> Post Condition </b>
 *    @n None
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim

        CSL_i2cStop(hI2c);

     @endverbatim
 *  ============================================================================
 */
static inline
void CSL_i2cStop (
    /*
     * Pointer to the handle of I2C 
     */
    CSL_I2cHandle hI2c
)
{
    CSL_FINS(hI2c->regs->ICMDR, I2C_ICMDR_STP, TRUE);
}


/** ============================================================================
 *   @n@b CSL_i2cDirTransmit
 *
 *   @b Description
 *   @n This function sets the direction of data as transmit of the I2C module.
 *
 *   @b Arguments
 *   @verbatim

            hI2c            Handle to I2C instance

     @endverbatim
 *
 *   <b> Return Value </b>  None
 *
 *   <b> Pre Condition </b>
 *   @n  CSL_i2cInit(),CSL_i2cOpen()has to be called successfully before calling
 *       this function
 *
 *   <b> Post Condition </b>
 *    @n None
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim

        CSL_i2cDirTransmit(hI2c);

     @endverbatim
 *  ============================================================================
 */
static inline
void CSL_i2cDirTransmit (
    /*
     * Pointer to the handle of I2C 
     */
    CSL_I2cHandle        hI2c
)
{
    CSL_FINS(hI2c->regs->ICMDR, I2C_ICMDR_TRX, TRUE);
}


/** ============================================================================
 *   @n@b CSL_i2cDirReceive
 *
 *   @b Description
 *   @n This function sets the direction of data as receive of the I2C module.
 *
 *   @b Arguments
 *   @verbatim

            hI2c            Handle to I2C instance

     @endverbatim
 *
 *   <b> Return Value </b>  None
 *
 *   <b> Pre Condition </b>
 *   @n  CSL_i2cInit(),CSL_i2cOpen()has to be called successfully before calling
 *       this function
 *
 *   <b> Post Condition </b>
 *    @n None
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim

        CSL_i2cDirReceive(hI2c);

     @endverbatim
 *  ============================================================================
 */
static inline
void CSL_i2cDirReceive (
    /*
     * Pointer to the handle of I2C 
     */
    CSL_I2cHandle hI2c
)
{
    CSL_FINS(hI2c->regs->ICMDR, I2C_ICMDR_TRX, FALSE);
}


/** ============================================================================
 *   @n@b CSL_i2RmEnable
 *
 *   @b Description
 *   @n This function enables the repeat mode of the I2C module.
 *
 *   @b Arguments
 *   @verbatim

            hI2c            Handle to I2C instance

     @endverbatim
 *
 *   <b> Return Value </b>  None
 *
 *   <b> Pre Condition </b>
 *   @n  CSL_i2cInit(),CSL_i2cOpen()has to be called successfully before calling
 *       this function
 *
 *   <b> Post Condition </b>
 *    @n None
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim

        CSL_i2RmEnable(hI2c);

     @endverbatim
 *  ============================================================================
 */
static inline
void CSL_i2cRmEnable (
    /* 
     * Pointer to the handle of I2C 
     */
    CSL_I2cHandle hI2c
)
{
    CSL_FINS(hI2c->regs->ICMDR, I2C_ICMDR_RM, TRUE);
}


/** ============================================================================
 *   @n@b CSL_i2cRmDisable
 *
 *   @b Description
 *   @n This function disables the repeat mode of the I2C module.
 *
 *   @b Arguments
 *   @verbatim

            hI2c            Handle to I2C instance

     @endverbatim
 *
 *   <b> Return Value </b>  None
 *
 *   <b> Pre Condition </b>
 *   @n  CSL_i2cInit(),CSL_i2cOpen()has to be called successfully before calling
 *       this function
 *
 *   <b> Post Condition </b>
 *    @n None
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim

        CSL_i2cRmDisable(hI2c);

     @endverbatim
 *  ============================================================================
 */
static inline
void CSL_i2cRmDisable (
    /*
     * Pointer to the handle of I2C 
     */
    CSL_I2cHandle hI2c
)
{
    CSL_FINS(hI2c->regs->ICMDR, I2C_ICMDR_RM, FALSE);
}


/** ============================================================================
 *   @n@b CSL_i2cDlbEnable
 *
 *   @b Description
 *   @n This function enables the data loop back of the I2C module.
 *
 *   @b Arguments
 *   @verbatim

            hI2c            Handle to I2C instance

     @endverbatim
 *
 *   <b> Return Value </b>  None
 *
 *   <b> Pre Condition </b>
 *   @n  CSL_i2cInit(),CSL_i2cOpen()has to be called successfully before calling
 *       this function
 *
 *   <b> Post Condition </b>
 *    @n None
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim

        CSL_i2cDlbEnable(hI2c);

     @endverbatim
 *  ============================================================================
 */
static inline
void CSL_i2cDlbEnable (
    /*
     * Pointer to the handle of I2C 
     */
    CSL_I2cHandle hI2c
)
{
    CSL_FINS(hI2c->regs->ICMDR, I2C_ICMDR_DLB, TRUE);
}


/** ============================================================================
 *   @n@b CSL_i2cDlbDisable
 *
 *   @b Description
 *   @n This function disables the data loop back of I2C module.
 *
 *   @b Arguments
 *   @verbatim

            hI2c            Handle to I2C instance

     @endverbatim
 *
 *   <b> Return Value </b>  None
 *
 *   <b> Pre Condition </b>
 *   @n  CSL_i2cInit(),CSL_i2cOpen()has to be called successfully before calling
 *       this function
 *
 *   <b> Post Condition </b>
 *    @n None
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim

        CSL_i2cDlbDisable(hI2c);

     @endverbatim
 *  ============================================================================
 */
static inline
void CSL_i2cDlbDisable (
    /*
     * Pointer to the handle of I2C 
     */
    CSL_I2cHandle hI2c
)
{
    CSL_FINS(hI2c->regs->ICMDR, I2C_ICMDR_DLB, FALSE);
}


/**
 *  Status Functions of i2c.
 */

/** ============================================================================
 *   @n@b CSL_i2cGetClockSetup
 *
 *   @b Description
 *   @n This function gets the clock setup of the I2C module.
 *
 *   @b Arguments
 *   @verbatim

            hI2c            Handle to I2C instance

            response        Placeholder to return status.

     @endverbatim
 *
 *   <b> Return Value </b>  Return clock high,clock low and prescale value.
 *
 *   <b> Pre Condition </b>
 *   @n  CSL_i2cInit(),CSL_i2cOpen()has to be called successfully before calling
 *       this function
 *
 *   <b> Post Condition </b>
 *    @n None
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim

        CSL_i2cGetClockSetup(hI2c, &response);

     @endverbatim
 *  ============================================================================
 */
static inline
void CSL_i2cGetClockSetup (
    /*
     * Pointer to the handle of I2C 
     */
    CSL_I2cHandle hI2c,

    /** Placeholder to return the status. @a void* casted */
    void          *response
)
{
    ((CSL_I2cClkSetup *)response)->prescalar
                                = CSL_FEXT(hI2c->regs->ICPSC, I2C_ICPSC_IPSC);
    ((CSL_I2cClkSetup *)response)->clklowdiv
                                = CSL_FEXT(hI2c->regs->ICCLKL, I2C_ICCLKL_ICCL);
    ((CSL_I2cClkSetup *)response)->clkhighdiv
                                = CSL_FEXT(hI2c->regs->ICCLKH, I2C_ICCLKH_ICCH);
}


/** ============================================================================
 *   @n@b CSL_i2cGetBusBusy
 *
 *   @b Description
 *   @n This function gets the bus busy status of I2C module.
 *
 *   @b Arguments
 *   @verbatim

            hI2c            Handle to I2C instance

            response        Placeholder to return status.

     @endverbatim
 *
 *   <b> Return Value </b>  Return bus busy status.
 *
 *   <b> Pre Condition </b>
 *   @n  CSL_i2cInit(),CSL_i2cOpen()has to be called successfully before calling
 *       this function
 *
 *   <b> Post Condition </b>
 *    @n None
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim

        CSL_i2cGetBusBusy(hI2c, &response);

     @endverbatim
 *  ============================================================================
 */
static inline
void CSL_i2cGetBusBusy (
    /* 
     * Pointer to the handle of I2C 
     */
    CSL_I2cHandle hI2c,

    /* Placeholder to return the status. @a void* casted */
    void          *response
)
{
    *(Uint32 *)response = CSL_FEXT(hI2c->regs->ICSTR, I2C_ICSTR_BB);
}


/** ============================================================================
 *   @n@b CSL_i2cGetRxRdy
 *
 *   @b Description
 *   @n This function gets the receive ready status of the I2C module.
 *
 *   @b Arguments
 *   @verbatim

            hI2c            Handle to I2C instance

            response        Placeholder to return status.

     @endverbatim
 *
 *   <b> Return Value </b>  Return receive ready status.
 *
 *   <b> Pre Condition </b>
 *   @n  CSL_i2cInit(),CSL_i2cOpen()has to be called successfully before calling
 *       this function
 *
 *   <b> Post Condition </b>
 *    @n None
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim

        CSL_i2cGetRxRdy(hI2c, &response);

     @endverbatim
 *  ============================================================================
 */
static inline
void CSL_i2cGetRxRdy (
    /*
     * Pointer to the handle of I2C 
     */
    CSL_I2cHandle hI2c,

    /* Placeholder to return the status. @a void* casted */
    void          *response
)
{
    *(Uint32 *)response = CSL_FEXT(hI2c->regs->ICSTR, I2C_ICSTR_ICRRDY);
}


/** ============================================================================
 *   @n@b CSL_i2cGetTxRdy
 *
 *   @b Description
 *   @n This function gets the transmit ready status of the I2C module.
 *
 *   @b Arguments
 *   @verbatim

            hI2c            Handle to I2C instance

            response        Placeholder to return status.

     @endverbatim
 *
 *   <b> Return Value </b>  Return Transmit ready status.
 *
 *   <b> Pre Condition </b>
 *   @n  CSL_i2cInit(),CSL_i2cOpen()has to be called successfully before calling
 *       this function
 *
 *   <b> Post Condition </b>
 *    @n None
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim

        CSL_i2cGetTxRdy(hI2c, &response);

     @endverbatim
 *  ============================================================================
 */
static inline
void CSL_i2cGetTxRdy (
    /* 
     * Pointer to the handle of I2C 
     */
    CSL_I2cHandle hI2c,

    /* Placeholder to return the status. @a void* casted */
    void          *response
)
{
    *(Uint32 *)response = CSL_FEXT(hI2c->regs->ICSTR, I2C_ICSTR_ICXRDY);
}


/** ============================================================================
 *   @n@b CSL_i2cGetAcsRdy
 *
 *   @b Description
 *   @n This function gets the ACS ready status of the I2C module.
 *
 *   @b Arguments
 *   @verbatim

            hI2c            Handle to I2C instance

            response        Placeholder to return status.

     @endverbatim
 *
 *   <b> Return Value </b>  Return Register-access-ready status.
 *
 *   <b> Pre Condition </b>
 *   @n  CSL_i2cInit(),CSL_i2cOpen()has to be called successfully before calling
 *       this function
 *
 *   <b> Post Condition </b>
 *    @n None
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim

        CSL_i2cGetAcsRdy(hI2c, &response);

     @endverbatim
 *  ============================================================================
 */
static inline
void CSL_i2cGetAcsRdy (
    /*
     * Pointer to the handle of I2C 
     */
    CSL_I2cHandle hI2c,

    /* Placeholder to return the status. @a void* casted */
    void          *response
)
{
    *(Uint32 *)response = CSL_FEXT(hI2c->regs->ICSTR, I2C_ICSTR_ARDY);
}


/** ============================================================================
 *   @n@b CSL_i2cGetScd
 *
 *   @b Description
 *   @n This function gets the SCD status of the I2C module.
 *
 *   @b Arguments
 *   @verbatim

            hI2c            Handle to I2C instance

            response        Placeholder to return status.

     @endverbatim
 *
 *   <b> Return Value </b>  Return stop condition detection status.
 *
 *   <b> Pre Condition </b>
 *   @n  CSL_i2cInit(),CSL_i2cOpen()has to be called successfully before calling
 *       this function
 *
 *   <b> Post Condition </b>
 *    @n None
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim

        CSL_i2cGetScd(hI2c, &response);

     @endverbatim
 *  ============================================================================
 */
static inline
void CSL_i2cGetScd (
    /* 
     * Pointer to the handle of I2C 
     */
    CSL_I2cHandle hI2c,

    /* Placeholder to return the status. @a void* casted */
    void          *response
)
{
    *(Uint32 *)response = CSL_FEXT(hI2c->regs->ICSTR, I2C_ICSTR_SCD);
}


/** ============================================================================
 *   @n@b CSL_i2cGetAd0
 *
 *   @b Description
 *   @n This function gets the AD0 status of the I2C module.
 *
 *   @b Arguments
 *   @verbatim

            hI2c            Handle to I2C instance

            response        Placeholder to return status.

     @endverbatim
 *
 *   <b> Return Value </b> Return Address Zero Status.
 *
 *   <b> Pre Condition </b>
 *   @n  CSL_i2cInit(),CSL_i2cOpen()has to be called successfully before calling
 *       this function
 *
 *   <b> Post Condition </b>
 *    @n None
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim

        CSL_i2cGetAd0(hI2c, &response);

     @endverbatim
 *  ============================================================================
 */
static inline
void CSL_i2cGetAd0 (
    /*
     * Pointer to the handle of I2C 
     */
    CSL_I2cHandle hI2c,

    /* Placeholder to return the status. @a void* casted */
    void          *response
)
{
    *(Uint32 *)response = CSL_FEXT(hI2c->regs->ICSTR, I2C_ICSTR_AD0);
}


/** ============================================================================
 *   @n@b CSL_i2cGetAas
 *
 *   @b Description
 *   @n This function gets the AAS status of the I2C module.
 *
 *   @b Arguments
 *   @verbatim

            hI2c            Handle to I2C instance

            response        Placeholder to return status.

     @endverbatim
 *
 *   <b> Return Value </b>  Return address as slave status.
 *
 *   <b> Pre Condition </b>
 *   @n  CSL_i2cInit(),CSL_i2cOpen()has to be called successfully before calling
 *       this function
 *
 *   <b> Post Condition </b>
 *    @n None
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim

        CSL_i2cGetAas(hI2c, &response);

     @endverbatim
 *  ============================================================================
 */
static inline
void CSL_i2cGetAas (
    /*
     * Pointer to the handle of I2C 
     */
    CSL_I2cHandle hI2c,

    /* Placeholder to return the status. @a void* casted */
    void          *response
)
{
    *(Uint32 *)response = CSL_FEXT(hI2c->regs->ICSTR, I2C_ICSTR_AAS);
}


/** ============================================================================
 *   @n@b CSL_i2cGetRsFull
 *
 *   @b Description
 *   @n This function gets the RS Full status of the I2C module.
 *
 *   @b Arguments
 *   @verbatim

            hI2c            Handle to I2C instance

            response        Placeholder to return status.

     @endverbatim
 *
 *   <b> Return Value </b>  Return receive full status.
 *
 *   <b> Pre Condition </b>
 *   @n  CSL_i2cInit(),CSL_i2cOpen()has to be called successfully before calling
 *       this function
 *
 *   <b> Post Condition </b>
 *    @n None
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim

        CSL_i2cGetRsFull(hI2c, &response);

     @endverbatim
 *  ============================================================================
 */
static inline
void CSL_i2cGetRsFull (
    /*
     * Pointer to the handle of I2C 
     */
    CSL_I2cHandle hI2c,

    /* Placeholder to return the status. @a void* casted */
    void          *response
)
{
    *(Uint32 *)response = CSL_FEXT(hI2c->regs->ICSTR, I2C_ICSTR_RSFULL);
}


/** ============================================================================
 *   @n@b CSL_i2cGetXsmt
 *
 *   @b Description
 *   @n This function gets the transmit status of the I2C module.
 *
 *   @b Arguments
 *   @verbatim

            hI2c            Handle to I2C instance

            response        Placeholder to return status.

     @endverbatim
 *
 *   <b> Return Value </b>  Return transmit status.
 *
 *   <b> Pre Condition </b>
 *   @n  CSL_i2cInit(),CSL_i2cOpen()has to be called successfully before calling
 *       this function
 *
 *   <b> Post Condition </b>
 *    @n None
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim

        CSL_i2cGetXsmt(hI2c, &response);

     @endverbatim
 *  ============================================================================
 */
static inline
void CSL_i2cGetXsmt (
    /*
     * Pointer to the handle of I2C 
     */
    CSL_I2cHandle hI2c,

    /* Placeholder to return the status. @a void* casted */
    void          *response
)
{
    *(Uint32 *)response = CSL_FEXT(hI2c->regs->ICSTR, I2C_ICSTR_XSMT);
}


/** ============================================================================
 *   @n@b CSL_i2cGetAl
 *
 *   @b Description
 *   @n This function gets the AL status of the I2C module.
 *
 *   @b Arguments
 *   @verbatim

            hI2c            Handle to I2C instance

            response        Placeholder to return status.

     @endverbatim
 *
 *   <b> Return Value </b>  Return Arbitration-Lost status.
 *
 *   <b> Pre Condition </b>
 *   @n  CSL_i2cInit(),CSL_i2cOpen()has to be called successfully before calling
 *       this function
 *
 *   <b> Post Condition </b>
 *    @n None
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim

        CSL_i2cGetAl(hI2c, &response);

     @endverbatim
 *  ============================================================================
 */
static inline
void CSL_i2cGetAl (
    /*
     * Pointer to the handle of I2C 
     */
    CSL_I2cHandle hI2c,

    /* Placeholder to return the status. @a void* casted */
    void          *response
)
{
    *(Uint32 *)response = CSL_FEXT(hI2c->regs->ICSTR, I2C_ICSTR_AL);
}


/** ============================================================================
 *   @n@b CSL_i2cGetSdir
 *
 *   @b Description
 *   @n This function gets the SDIR status of the I2C module.
 *
 *   @b Arguments
 *   @verbatim

            hI2c            Handle to I2C instance

            response        Placeholder to return status.

     @endverbatim
 *
 *   <b> Return Value </b>  Return Slave Direction status.
 *
 *   <b> Pre Condition </b>
 *   @n  CSL_i2cInit(),CSL_i2cOpen()has to be called successfully before calling
 *       this function
 *
 *   <b> Post Condition </b>
 *    @n None
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim

        CSL_i2cGetSdir(hI2c, &response);

     @endverbatim
 *  ============================================================================
 */
static inline
void CSL_i2cGetSdir (
    /* 
     * Pointer to the handle of I2C 
     */
    CSL_I2cHandle hI2c,

    /* Placeholder to return the status. @a void* casted */
    void          *response
)
{
    *(Uint32 *)response = CSL_FEXT(hI2c->regs->ICSTR, I2C_ICSTR_SDIR);
}


/** ============================================================================
 *   @n@b CSL_i2cGetNacksnt
 *
 *   @b Description
 *   @n This function gets the No Ack Sent status of the I2C module.
 *
 *   @b Arguments
 *   @verbatim

            hI2c            Handle to I2C instance

            response        Placeholder to return status.

     @endverbatim
 *
 *   <b> Return Value </b>  Return No Acknowledge status.
 *
 *   <b> Pre Condition </b>
 *   @n  CSL_i2cInit(),CSL_i2cOpen()has to be called successfully before calling
 *       this function
 *
 *   <b> Post Condition </b>
 *    @n None
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim

        CSL_i2cGetNacksnt(hI2c, &response);

     @endverbatim
 *  ============================================================================
 */
static inline
void CSL_i2cGetNacksnt (
    /*
     * Pointer to the handle of I2C 
     */
    CSL_I2cHandle hI2c,

    /* Placeholder to return the status. @a void* casted */
    void          *response
)
{
    *(Uint32 *)response = CSL_FEXT(hI2c->regs->ICSTR, I2C_ICSTR_NACKSNT);
}


/** ============================================================================
 *   @n@b CSL_i2cGetRdone
 *
 *   @b Description
 *   @n This function gets the Reset Done status of the I2C module.
 *
 *   @b Arguments
 *   @verbatim

            hI2c            Handle to I2C instance

            response        Placeholder to return status.

     @endverbatim
 *
 *   <b> Return Value </b>  Return receive done status.
 *
 *   <b> Pre Condition </b>
 *   @n  CSL_i2cInit(),CSL_i2cOpen()has to be called successfully before calling
 *       this function
 *
 *   <b> Post Condition </b>
 *    @n None
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim

        CSL_i2cGetRdone(hI2c, &response);

     @endverbatim
 *  ============================================================================
 */
static inline
void CSL_i2cGetRdone (
    /*
     * Pointer to the handle of I2C 
     */
    CSL_I2cHandle hI2c,

    /* Placeholder to return the status. @a void* casted */
    void          *response
)
{
    *(Uint32 *)response = CSL_FEXT(hI2c->regs->ICMDR, I2C_ICMDR_IRS);
}


/** ============================================================================
 *   @n@b CSL_i2cGetBitcount
 *
 *   @b Description
 *   @n This function gets the bit count number of the I2C module.
 *
 *   @b Arguments
 *   @verbatim

            hI2c            Handle to I2C instance

            response        Placeholder to return status.

     @endverbatim
 *
 *   <b> Return Value </b>  Return bit count value.
 *
 *   <b> Pre Condition </b>
 *   @n  CSL_i2cInit(),CSL_i2cOpen()has to be called successfully before calling
 *       this function
 *
 *   <b> Post Condition </b>
 *    @n None
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim

        CSL_i2cGetBitcount(hI2c, &response);

     @endverbatim
 *  ============================================================================
 */
static inline
void CSL_i2cGetBitcount (
    /*
     * Pointer to the handle of I2C 
     */
    CSL_I2cHandle hI2c,

    /* Placeholder to return the status. @a void* casted */
    void          *response
)
{
    *(Uint32 *)response = CSL_FEXT(hI2c->regs->ICMDR, I2C_ICMDR_BC);
}


/** ============================================================================
 *   @n@b CSL_i2cGetIntcode
 *
 *   @b Description
 *   @n This function gets the interrupt code of the I2C module.
 *
 *   @b Arguments
 *   @verbatim

            hI2c            Handle to I2C instance

            response        Placeholder to return status.

     @endverbatim
 *
 *   <b> Return Value </b>  None
 *
 *   <b> Pre Condition </b>
 *   @n  CSL_i2cInit(),CSL_i2cOpen()has to be called successfully before calling
 *       this function
 *
 *   <b> Post Condition </b>
 *    @n None
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim

        CSL_i2cGetIntcode(hI2c, &response);

     @endverbatim
 *  ============================================================================
 */
static inline
void CSL_i2cGetIntcode (
    /*
     * Pointer to the handle of I2C 
     */
    CSL_I2cHandle hI2c,

    /* Placeholder to return the status. @a void* casted */
    void          *response
)
{
    *(Uint32 *)response = CSL_FEXT(hI2c->regs->ICIVR, I2C_ICIVR_INTCODE);
}

#ifdef __cplusplus
}
#endif

#endif /* CSL_I2CAUX_H_ */
