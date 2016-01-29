/* ============================================================================
 * Copyright (c) Texas Instruments Inc 2002, 2003, 2004, 2005
 *
 * Use of this software is controlled by the terms and conditions found in the
 * license agreement under which this software has been supplied.
 * ============================================================================
 */

/** ===========================================================================
 * @file csl_tmr.h
 *
 * @brief Header file for functional layer of CSL
 * 
 * @Path $(CSLPATH)\inc
 * 
 * @desc The different enumerations, structure definitions
 *       and function declarations  
 * ============================================================================
 * @mainpage TIMER
 *
 * @section Introduction
 *
 * @subsection xxx Purpose and Scope
 * The purpose of this document is to identify a set of common CSL APIs for
 * the TMR Module across various devices. The CSL developer is expected
 * to refer to this document while designing APIs for these modules. Some
 * of the listed APIs may not be applicable to a given TMR Module. While
 * in other cases this list of APIs may not be sufficient to cover all the
 * features of a particular TMR Module. The CSL developer should use his
 * discretion in designing new APIs or extending the existing ones to cover
 * these.
 *
 *
 * @subsection aaa Terms and Abbreviations
 *   -# CSL:  Chip Support Library
 *   -# API:  Application Programmer Interface
 * ============================================================================
 */

/* ============================================================================
 * Revision History
 * ===============
 *  20-may-2005 PSK File Created.
 
 *  29-Jul-2005 PSK Updted changes acooriding to revised timer spec. the number 
 *                  convention TIM12, TIM34 are changed to TIMLO and TIMHI.
 
 *  01-Feb-2006 ds  Updated According to TCI6482/C6455 UserGuide
 *                  (Removed tmrClksrcHi, tmrIpGateHi and tmrInvInpHi fields 
 *                  from the hwSetup structure).
 *  03-Apr-2006 ds  Removed "EMUMGT_CLKSPD" from CSL_TmrConfig structure
 * ============================================================================
 */

#ifndef _CSL_TMR_H_
#define _CSL_TMR_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <csl.h>
#include <cslr_tmr.h>

/**
 * General purpose global typedef declarations  
 */

/**
 *  @brief This enum describes the commands used to control the GP timer through
 *  CSL_tmrHwControl()
 */
typedef enum {
    /**
     * @brief   Loads the GP Timer Period Register Low
     * @param   Uint32 *
     */
    CSL_TMR_CMD_LOAD_PRDLO = 0,

    /**
     * @brief   Loads the GP Timer Period Register High
     * @param   Uint32 *
     */
    CSL_TMR_CMD_LOAD_PRDHI = 1,

    /**
     * @brief   Loads the GP Timer Pre-scalar value for TIMHI
     * @param   Uint8 *
     */
    CSL_TMR_CMD_LOAD_PSCHI = 2,

    /**
     * @brief   Enable the GP timer Low
     * @param   CSL_TmrEnamode
     */
    CSL_TMR_CMD_START_TIMLO = 3,

    /**
     * @brief   Enable the GP timer High
     * @param   CSL_TmrEnamode
     */
    CSL_TMR_CMD_START_TIMHI = 4,

    /**
     * @brief   Stop the GP timer Low
     * @param   None
     */
    CSL_TMR_CMD_STOP_TIMLO = 5,

    /**
     * @brief   Stop the GP timer High
     * @param   None
     */
    CSL_TMR_CMD_STOP_TIMHI = 6,

    /**
     * @brief   Reset the GP timer Low
     * @param   None
     */
    CSL_TMR_CMD_RESET_TIMLO = 7,

    /**
     * @brief   Reset the GP timer High
     * @param   None
     */
    CSL_TMR_CMD_RESET_TIMHI = 8,

    /**
     * @brief   Start the timer in GPtimer64 OR Chained mode
     * @param   None
     */
    CSL_TMR_CMD_START64 = 9,

    /**
     * @brief   Stop the timer of GPtimer64 OR Chained
     * @param   CSL_TmrEnamode
     */
    CSL_TMR_CMD_STOP64 = 10,

    /**
     * @brief   Reset the timer of GPtimer64 OR Chained
     * @param   None
     */
    CSL_TMR_CMD_RESET64 = 11,

    /**
     * @brief	Enable the timer in watchdog mode
     * @param	CSL_TmrEnamode
     */
    CSL_TMR_CMD_START_WDT = 12,

    /**
     * @brief	Loads the watchdog key
     * @param	Uint16
     */
    CSL_TMR_CMD_LOAD_WDKEY = 13        
} CSL_TmrHwControlCmd;

/**
 *  @brief This enum describes the commands used to get status of various parameters of the
 *  GP timer. These values are used in CSL_tmrGetHwStatus()
 */
typedef enum {
    /**
     * @brief   Gets the current value of the GP timer TIMLO register
     * @param   Uint32 *
     */
    CSL_TMR_QUERY_COUNT_LO = 0,

    /**
     * @brief   Gets the current value of the GP timer TIMHI register
     * @param   Uint32 *
     */
    CSL_TMR_QUERY_COUNT_HI = 1,
    /**
     * @brief   This query command returns the status
     *          about whether the TIMLO is running or stopped
     *
     * @param   CSL_TmrTstat
     */
    CSL_TMR_QUERY_TSTAT_LO = 2,

    /**
     * @brief   This query command returns the status
     *          about whether the TIMHI is running or stopped
     *
     * @param   CSL_TmrTstat
     */
    CSL_TMR_QUERY_TSTAT_HI = 3,

    /**
     * @brief	This query command returns the status about whether the timer 
     *		is in watchdog mode or not
     * @param	CSL_WdflagBitStatus
     */
    CSL_TMR_QUERY_WDFLAG_STATUS = 4
} CSL_TmrHwStatusQuery;

/**
 *  @brief This enum describes whether the Timer Clock input is gated or not gated.
 */
typedef enum {
    /** timer input not gated */
    CSL_TMR_CLOCK_INP_NOGATE = 0,

    /** timer input gated */
    CSL_TMR_CLOCK_INP_GATE = 1
} CSL_TmrIpGate;

/**
 *  @brief This enum describes the Timer Clock source selection.
 */
typedef enum {
    /** timer clock INTERNAL source selection */
    CSL_TMR_CLKSRC_INTERNAL = 0,

    /** timer clock Timer input pin source selection */
    CSL_TMR_CLKSRC_TMRINP = 1
} CSL_TmrClksrc;

/**
 *  @brief This enum describes the enabling/disabling of Timer.
 */
typedef enum {
    /** The timer is disabled and maintains current value */
    CSL_TMR_ENAMODE_DISABLE = 0,

    /**  The timer is enabled one time */
    CSL_TMR_ENAMODE_ENABLE = 1,

    /**  The timer is enabled continuously */
    CSL_TMR_ENAMODE_CONT = 2
} CSL_TmrEnamode;

/**
 *  @brief This enum describes the Timer Clock cycles (1/2/3/4).
 */
typedef enum {
    /** One timer clock cycle */
    CSL_TMR_PWID_ONECLK = 0,

    /** Two timer clock cycle */
    CSL_TMR_PWID_TWOCLKS = 1,

    /** Three timer clock cycle */
    CSL_TMR_PWID_THREECLKS = 2,

    /** Four timer clock cycle */
    CSL_TMR_PWID_FOURCLKS = 3
} CSL_TmrPulseWidth;

/**
 *  @brief This enum describes the mode of Timer Clock (Pulse/Clock).
 */
typedef enum {
    /** Pulse mode */
    CSL_TMR_CP_PULSE = 0,

    /** Clock mode */
    CSL_TMR_CP_CLOCK = 1
} CSL_TmrClockPulse;

/**
 *  @brief This enum describes the Timer input inverter control
 */
typedef enum {
    /** Uninverted timer input drives timer */
    CSL_TMR_INVINP_UNINVERTED = 0,

    /** Inverted timer input drives timer */
    CSL_TMR_INVINP_INVERTED = 1
} CSL_TmrInvInp;

/**
 *  @brief This enum describes the Timer output inverter control
 */
typedef enum {
    /** Uninverted timer output */
    CSL_TMR_INVOUTP_UNINVERTED = 0,

    /** Inverted timer output */
    CSL_TMR_INVOUTP_INVERTED = 1
} CSL_TmrInvOutp;

/**
 *  @brief This enum describes the mode of Timer (GPT/WDT/Chained/Unchained).
 */
typedef enum {
    /** The timer is in 64-bit GP timer mode */
    CSL_TMR_TIMMODE_GPT = 0,

    /** The timer is in dual 32-bit timer, unchained mode */
    CSL_TMR_TIMMODE_DUAL_UNCHAINED = 1,

    /** The timer is in 64-bit Watchdog timer mode */
    CSL_TMR_TIMMODE_WDT = 2,

    /** The timer is in dual 32-bit timer, chained mode */
    CSL_TMR_TIMMODE_DUAL_CHAINED = 3
} CSL_TmrMode;

/**
 *  @brief This enum describes the reset condition of Timer (ON/OFF).
 */
typedef enum {
    /** timer TIMxx is in reset */
    CSL_TMR_TIMxxRS_RESET_ON = 0,

    /** timer TIMHI is not in reset. TIMHI can be used as a 32-bit timer */
    CSL_TMR_TIMxxRS_RESET_OFF = 1
} CSL_TmrState;

/**  @brief This enum describes the status of Timer.
 */
typedef enum {
    /** Timer status drives High */
    CSL_TMR_TSTAT_HIGH = 1,

    /** Timer status drives Low */
    CSL_TMR_TSTAT_LOW = 0
} CSL_TmrTstat;

/** @brief This enmeration describes the flag bit status of the timer in watchdog mode 
 */
typedef enum {
	/** No watchdog timeout occured */
	CSL_TMR_WDFLAG_NOTIMEOUT = 0,

	/** Watchdog timeout occured */
	CSL_TMR_WDFLAG_TIMEOUT = 1
} CSL_TmrWdflagBitStatus;

/**
 * @brief Hardware setup structure.
 */
typedef struct {
    /** 32 bit load value to be loaded to Timer Period Register low */
    Uint32 tmrTimerPeriodLo;

    /** 32 bit load value to be loaded to Timer Period Register High */
    Uint32 tmrTimerPeriodHi;

    /** 32 bit load value to be loaded to Timer Counter Register Low */
    Uint32 tmrTimerCounterLo;

    /** 32 bit load value to be loaded to Timer Counter Register High */
    Uint32 tmrTimerCounterHi;

    /** Pulse width. used in pulse mode (C/P_=0) by the timer */
    CSL_TmrPulseWidth tmrPulseWidthHi;

    /** Clock/Pulse mode for timerHigh output */
    CSL_TmrClockPulse tmrClockPulseHi;

    /** Timer output inverter control */
    CSL_TmrInvOutp tmrInvOutpHi;

    /** TIEN determines if the timer clock is gated by the timer input.
     *  Applicable only when CLKSRC=0
     */
    CSL_TmrIpGate tmrIpGateLo;

    /** CLKSRC determines the selected clock source for the timer */
    CSL_TmrClksrc tmrClksrcLo;

    /** Pulse width. used in pulse mode (C/P_=0) by the timer */
    CSL_TmrPulseWidth tmrPulseWidthLo;

    /** Clock/Pulse mode for timerLow output */
    CSL_TmrClockPulse tmrClockPulseLo;

    /** Timer input inverter control. Only affects operation
     *  if CLKSRC=1, Timer Input pin
     */
    CSL_TmrInvInp tmrInvInpLo;

    /** Timer output inverter control */
    CSL_TmrInvOutp tmrInvOutpLo;

    /** TIMHI pre-scalar counter specifies the count for TIMHI */
    Uint8 tmrPreScalarCounterHi;

    /** Configures the GP timer in GP mode or in
     *  general purpose timer mode or Dual 32 bit timer mode
     */
    CSL_TmrMode tmrTimerMode;
} CSL_TmrHwSetup;

/**
 *  @brief Config-structure Used to configure the GP timer using CSL_tmrHwSetupRaw()
 */
typedef struct {
    /** Timer Counter Register Low */
    Uint32 TIMLO;
    /** Timer Counter Register High */
    Uint32 TIMHI;
    /** Timer Period Register Low */
    Uint32 PRDLO;
    /** Timer Period Register High */
    Uint32 PRDHI;
    /** Timer Control Register */
    Uint32 TCR;
    /** Timer Global Control Register */
    Uint32 TGCR;
    /** Watchdog Timer Control Register */
    Uint32 WDTCR; 
} CSL_TmrConfig;

/**
 * @brief Module specific context information. Present implementation of 
 *        Timer CSL doesn't have any context information.
 */
typedef struct {
    /** Context information of Timer CSL.
     *  The below declaration is just a place-holder for future implementation.
     */
    Uint16 contextInfo;
} CSL_TmrContext;

/** @brief Module specific parameters. Present implementation of Timer CSL
 *         doesn't have any module specific parameters.
 */
typedef struct {
    /** Bit mask to be used for module specific parameters. The below
     *  declaration is just a place-holder for future implementation.
     */
    CSL_BitMask16 flags;
} CSL_TmrParam;

/** @brief This structure contains the base-address information for the
 *         peripheral instance
 */
typedef struct {
    /** Base-address of the configuration registers of the peripheral
     */
    CSL_TmrRegsOvly regs;
} CSL_TmrBaseAddress;

/**
 * @brief  Timer object structure.
 */
typedef struct {
    /** Pointer to the register overlay structure of the Timer */
    CSL_TmrRegsOvly regs;

    /** Instance of timer being referred by this object  */
    CSL_InstNum perNum;
} CSL_TmrObj;

/** @brief Default hardware setup parameters */
#define CSL_TMR_HWSETUP_DEFAULTS { \
    CSL_TMR_PRDLO_RESETVAL, \
    CSL_TMR_PRDHI_RESETVAL, \
    CSL_TMR_TIMLO_RESETVAL, \
    CSL_TMR_TIMHI_RESETVAL, \
    (CSL_TmrPulseWidth)CSL_TMR_TCR_PWID_HI_RESETVAL, \
    (CSL_TmrClockPulse)CSL_TMR_TCR_CP_HI_RESETVAL, \
    (CSL_TmrInvOutp)CSL_TMR_TCR_INVOUTP_HI_RESETVAL, \
    (CSL_TmrIpGate)CSL_TMR_TCR_TIEN_LO_RESETVAL, \
    (CSL_TmrClksrc)CSL_TMR_TCR_CLKSRC_LO_RESETVAL, \
    (CSL_TmrPulseWidth)CSL_TMR_TCR_PWID_LO_RESETVAL, \
    (CSL_TmrClockPulse)CSL_TMR_TCR_CP_LO_RESETVAL, \
    (CSL_TmrInvInp)CSL_TMR_TCR_INVINP_LO_RESETVAL, \
    (CSL_TmrInvOutp)CSL_TMR_TCR_INVOUTP_LO_RESETVAL, \
    CSL_TMR_TGCR_PSCHI_RESETVAL, \
    (CSL_TmrMode)CSL_TMR_TGCR_TIMMODE_RESETVAL \
}

/** @brief Default values for config structure */
#define CSL_TMR_CONFIG_DEFAULTS { \
    CSL_TMR_TIMLO_RESETVAL, \
    CSL_TMR_TIMHI_RESETVAL, \
    CSL_TMR_PRDLO_RESETVAL, \
    CSL_TMR_PRDHI_RESETVAL, \
    CSL_TMR_TCR_RESETVAL, \
    CSL_TMR_TGCR_RESETVAL, \
    CSL_TMR_WDTCR_RESETVAL \
}

/**
 * @brief This data type is used to return the handle to the CSL of the GP timer
 */
typedef CSL_TmrObj *CSL_TmrHandle;

/*******************************************************************************
 * Timer global function declarations
 ******************************************************************************/

/** ============================================================================
 *   @n@b CSL_tmrInit
 *
 *   @b Description
 *   @n This is the initialization function for the General purpose timer CSL.
 *      The function must be called before calling any other API from this CSL.
 *      This function is idem-potent. Currently, the function just return
 *      status CSL_SOK, without doing anything.
 *
 *   @b Arguments
 *   @verbatim
        pContext    Pointer to module-context. As General purpose timer doesn't
                    have any context based information user is expected to pass
                    NULL.
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                        CSL_SOK - Always returns
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  The CSL for gptimer is initialized
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim
            CSL_tmrInit();
     @endverbatim
 * =============================================================================
 */
CSL_Status CSL_tmrInit (
    CSL_TmrContext *pContext
);



/** ============================================================================
 *   @n@b CSL_tmrOpen
 *
 *   @b Description
 *   @n This function populates the peripheral data object for the TIMER instance
 *      and returns a handle to the instance.
 *      The open call sets up the data structures for the particular instance
 *      of TIMER device. The device can be re-opened anytime after it has been
 *      normally closed if so required. The handle returned by this call is
 *      input as an essential argument for rest of the APIs described
 *      for this module.
 *
 *   @b Arguments
 *   @verbatim
            tmrObj          Pointer to gptimer object.

            tmrNum          Instance of gptimer CSL to be opened.
                            There are three instance of the gptimer
                            available. So, the value for this parameter will be
                            based on the instance.

            pTmrParam       Module specific parameters.

            status          Status of the function call
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_TmrHandle
 *   @n                         Valid gptimer handle will be returned if
 *                              status value is equal to CSL_SOK.
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n   1.    The status is returned in the status variable. If status
 *              returned is
 *   @li            CSL_SOK             Valid gptimer handle is returned
 *   @li            CSL_ESYS_FAIL       The gptimer instance is invalid
 *
 *        2.    Gptimer object structure is populated
 *
 *   @b Modifies
 *   @n    1. The status variable
 *
 *         2. Gptimer object structure
 *
 *   @b Example
 *   @verbatim
            CSL_status              status;
            CSL_TmrObj              tmrObj;
            CSL_TmrHandle           hTmr;

            hTmr = CSL_tmrOpen(&tmrObj, CSL_TMR_1, NULL, &status);
            ...
    @endverbatim
 * =============================================================================
 */
CSL_TmrHandle CSL_tmrOpen (
    CSL_TmrObj   *tmrObj, 
    CSL_InstNum  tmrNum,
    CSL_TmrParam *pTmrParam, 
    CSL_Status   *status
);



/** ============================================================================
 *   @n@b CSL_tmrGetBaseAddress
 *
 *   @b Description
 *   @n  This function gets the base address of the given gptimer
 *       instance.
 *
 *   @b Arguments
 *   @verbatim
            tmrNum        Specifies the instance of the gptimer to be opened

            pTmrParam     Gptimer module specific parameters

            pBaseAddress  Pointer to base address structure containing base
                          address details
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li					CSL_OK             Open call is successful
 *   @li					CSL_ESYS_FAIL      gptimer instance is not
 *                                             available.
 *   @li                    CSL_ESYS_INVPARAMS Invalid Parameters
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  Base address structure is populated
 *
 *   @b Modifies
 *   @n    1. The status variable
 *
 *         2. Base address structure is modified.
 *
 *   @b Example
 *   @verbatim
        CSL_Status            status;
        CSL_TmrBaseAddress    baseAddress;

        ...
        status = CSL_tmrGetBaseAddress(CSL_TMR_1, NULL, &baseAddress);
        ...

    @endverbatim
 * ===========================================================================
 */
CSL_Status CSL_tmrGetBaseAddress (
    CSL_InstNum        tmrNum,
    CSL_TmrParam       *pTmrParam,
    CSL_TmrBaseAddress *pBaseAddress
);



/** ============================================================================
 *   @n@b CSL_tmrClose
 *
 *   @b Description
 *   @n This function marks that CSL for the GP timer instance is closed.
 *      CSL for the GP timer instance need to be reopened before using any
 *      GP timer CSL API.
 *
 *   @b Arguments
 *   @verbatim
            hTmr         Handle to the GP timer instance
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK            - GP timer is closed
 *                                               successfully
 *
 *   @li                    CSL_ESYS_BADHANDLE - The handle passed is invalid
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  1. The GP timer CSL APIs can not be called until the GP timer
 *          CSL is reopened again using CSL_tmrOpen()
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim
            CSL_tmrClose(hTmr);
    @endverbatim
 * ===========================================================================
 */
CSL_Status CSL_tmrClose (
    CSL_TmrHandle hTmr
);



/** ============================================================================
 *   @n@b CSL_tmrHwSetup
 *
 *   @b Description
 *   @n It configures the  timer instance registers as per the values passed
 *      in the hardware setup structure.
 *
 *   @b Arguments
 *   @verbatim
            htmr            Handle to the timer instance

            setup           Pointer to hardware setup structure
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK             - Hardware setup successful
 *   @li                    CSL_ESYS_BADHANDLE  - Invalid handle
 *   @li                    CSL_ESYS_INVPARAMS  - Hardware structure is not
 *                                                properly initialized
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  The specified instance will be setup according to value passed
 *
 *   @b Modifies
 *   @n Hardware registers for the specified instance
 *
 *   @b Example
 *   @verbatim
        CSL_status          status;
        CSL_tmrHwSetup      hwSetup;

        status = CSL_tmrHwSetup(htmr, &hwSetup);

     @endverbatim
 * ===========================================================================
 */
CSL_Status CSL_tmrHwSetup (
    CSL_TmrHandle  hTmr,
    CSL_TmrHwSetup *hwSetup
);



/** ============================================================================
 *   @n@b CSL_tmrHwControl
 *
 *   @b Description
 *   @n This function performs various control operations on the timer instance,
 *      based on the command passed.
 *
 *   @b Arguments
 *   @verbatim
            hTmr         Handle to the timer instance

            cmd          Operation to be performed on the timer

            cmdArg       Arguement specific to the command

     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK            - Command execution successful.
 *   @li                    CSL_ESYS_BADHANDLE - Invalid handle
 *   @li                    CSL_ESYS_INVCMD    - Invalid command
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  Registers of the timer instance are configured according to the command
 *       and the command arguments. The command determines which registers are
 *       modified.
 *
 *   @b Modifies
 *   @n Registers determined by the command
 *
 *   @b Example
 *   @verbatim
        CSL_Status status;

        status  = CSL_tmrHwControl(hTmr, CSL_TMR_CMD_START_TIMLO, NULL);

     @endverbatim
 * ============================================================================
 */
CSL_Status CSL_tmrHwControl(
    CSL_TmrHandle       hTmr,
    CSL_TmrHwControlCmd cmd,
    void                *cmdArg
);



/** ===========================================================================
 *   @n@b CSL_tmrGetHwStatus
 *
 *   @b Description
 *   @n This function is used to get the value of various parameters of the
 *      timer instance. The value returned depends on the query passed.
 *
 *   @b Arguments
 *   @verbatim
            hTmr            Handle to the timer instance

            query           Query to be performed

            response        Pointer to buffer to return the data requested by
                            the query passed
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK            - Successful completion of the
 *                                               query
 *
 *   @li                    CSL_ESYS_BADHANDLE - Invalid handle
 *
 *   @li                    CSL_ESYS_INVQUERY  - Query command not supported
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *       Data requested by the query is returned through the variable "response"
 *
 *   @b Modifies
 *   @n  The input arguement "response" is modified
 *
 *   @b Example
 *   @verbatim

        CSL_status    status;
        Uint8         response;

        status = CSL_tmrGetHwStatus(hGptimer, CSL_TMR_QUERY_COUNT_LO,
                                    &response);
     @endverbatim
 * ===========================================================================
 */
CSL_Status CSL_tmrGetHwStatus (
    CSL_TmrHandle        hTmr,
    CSL_TmrHwStatusQuery query,
    void                 *response
);



/** ============================================================================
 *   @n@b CSL_tmrHwSetupRaw
 *
 *   @b Description
 *   @n This function initializes the device registers with the register-values
 *  	provided through the config data structure.
 *
 *   @b Arguments
 *   @verbatim
            hTmr            Pointer to the object that holds reference to the 
                            instance of TIMER requested after the call 

            config          Pointer to the config structure containing the
                            device register values
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK             - Configuration successful
 *   @li                    CSL_ESYS_BADHANDLE  - Invalid handle
 *   @li                    CSL_ESYS_INVPARAMS  - Configuration structure
 *                                                pointer is not properly
 *                                                 initialized
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  The registers of the specified General purpose timer instance will be setup
 *       according to the values passed through the config structure
 *
 *   @b Modifies
 *   @n Hardware registers of the specified General purpose timer instance
 *
 *   @b Example
 *   @verbatim
        CSL_TmrHandle           hTmr;
        CSL_TmrConfig           config = CSL_TMR_CONFIG_DEFAULTS;
        CSL_Status              status;

        status = CSL_tmrHwSetupRaw (hTmr, &config);

     @endverbatim
 * ===========================================================================
 */
CSL_Status CSL_tmrHwSetupRaw (
    CSL_TmrHandle hTmr,
    CSL_TmrConfig *config
);



/** ============================================================================
 *   @n@b CSL_tmrGetHwSetup
 *
 *   @b Description
 *   @n It retrives the hardware setup parameters
 *
 *   @b Arguments
 *   @verbatim
            hTmr            Handle to the timer instance

            hwSetup         Pointer to hardware setup structure

     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK             - Hardware setup retrived
 *   @li                    CSL_ESYS_BADHANDLE  - Invalid handle
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  The hardware set up structure will be populated with values from
 *       the registers
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
        CSL_status          status;
        CSL_TmrHwSetup      hwSetup;

        status = CSL_tmrGetHwsetup (hTmr, &hwSetup);

     @endverbatim
 * ===========================================================================
 */
CSL_Status CSL_tmrGetHwSetup (
    CSL_TmrHandle  hTmr,
    CSL_TmrHwSetup *hwSetup
);

#ifdef __cplusplus
}
#endif

#endif  /* _CSL_TMR_H_ */

