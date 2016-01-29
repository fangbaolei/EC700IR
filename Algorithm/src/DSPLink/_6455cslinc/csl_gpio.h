/* ============================================================================
 * Copyright (c) Texas Instruments Inc 2002, 2003, 2004, 2005
 *
 * Use of this software is controlled by the terms and conditions found
 * in the license agreement under which this software has been supplied
 * provided
 * ============================================================================
*/

/** =========================================================================== 
 *  @file csl_gpio.h
 *
 *  @path $(CSLPATH)\inc
 *
 *  @desc GPIO functional layer APIs header file. The different enumerations, 
 *        structure definitions and function declarations
 * ============================================================================      
 * @mainpage GPIO CSL 3.x
 *
 * @section Introduction
 *
 * @subsection xxx Purpose and Scope
 * The purpose of this document is to identify a set of common CSL APIs for
 * the GPIO module across various devices. The CSL developer is expected to
 * refer to this document while designing APIs for these modules. Some of the
 * listed APIs may not be applicable to a given GPIO module. While other cases
 * this list of APIs may not be sufficient to cover all the features of a
 * particular GPIO Module.The CSL developer should use his discretion designing
 * new APIs or extending the existing ones to cover these.
 *
 * @subsection aaa Terms and Abbreviations
 *   -# CSL:  Chip Support Library
 *   -# API:  Application Programmer Interface
 *
 * @subsection References
 *    -# CSL-001-DES, CSL 3.x Design Specification DocumentVersion 1.02
 *=============================================================================
 */

/* ============================================================================
 * Revision History
 * ===============
 *  11-Jun-2004 PGR file created   
 *  04-sep-2004 Nsr - Updated CSL_GpioObj and added CSL_GpioBaseAddress,
 *                    CSL_GpioParam, SL_GpioContext,  CSL_GpioConfig structures.
 *                  - Updated comments for H/W control cmd and status query
 *                    enums.
 *                  - Added prototypes for CSL_gpioGetBaseAdddress and
 *                    CSL_gpioHwSetupRaw.
 *                  - Changed prototypes of CSL_gpioInit, CSL_gpioOpen.
 *                  - Updated respective comments along with that of
 *                    CSL_gpioClose.
 *  11-Oct-2004 Nsr - Removed the extern keyword before function declaration and
 *                  - Changed this file according to review.
 *  22-Feb-2005 Nsr - Added control command CSL_GPIO_CMD_GET_BIT according to
 *                     TI issue PSG00000310.
 *  28-Jul-2005 PSK - Updated the CSL source to support only one BANK
 *
 *  11-Jan-2006 NG  - Added CSL_GPIO_CMD_SET_OUT_BIT Control Command 
 *  06-Mar-2006 ds  - Rename CSL_GPIO_CMD_SET_OUT_BIT to 
 *                    CSL_GPIO_CMD_ENABLE_DISABLE_OUTBIT
 *                  - Moved CSL_GpioPinNum Enumeration from the cslr_gpio.h 
 * ============================================================================
 */

#ifndef _CSL_GPIO_H_
#define _CSL_GPIO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <soc.h>
#include <csl.h>
#include <cslr_gpio.h>

/**< Invalid argument */    
#define CSL_EGPIO_INVPARAM CSL_EGPIO_FIRST
    
/*****************************************************************************\
          GPIO global typedef declarations
\*****************************************************************************/

/**
 *  \brief  Base-address of the Configuration registers of GPIO.
 */
typedef struct {
    /** Base-address of the configuration registers of the peripheral
     */
    CSL_GpioRegsOvly regs;
} CSL_GpioBaseAddress;


/**
 *  \brief GPIO specific parameters. Present implementation doesn't have
 *  any specific parameters.
 */
typedef struct {
    /** Bit mask to be used for module specific parameters.
     *  The below declaration is just a place-holder for future
     *  implementation.
     */
    CSL_BitMask16 flags;
} CSL_GpioParam;


/** \brief GPIO specific context information. Present implementation doesn't
 *  have any Context information.
 */
typedef struct {
    /** Context information of GPIO.
     *  The below declaration is just a place-holder for future
     *  implementation.
     */
    Uint16 contextInfo;
} CSL_GpioContext;


/**
 * @brief Config structure of GPIO. This is used to configure GPIO
 * using CSL_HwSetupRaw function
 */
typedef struct  {
    /** GPIO Interrupt Per-Bank Enable Register */
    volatile Uint32 BINTEN; 
    
    /** GPIO Direction Register */
    volatile Uint32 DIR;
    
    /** GPIO Output Data Register */
    volatile Uint32 OUT_DATA;
    
    /** GPIO Set Data Register */
    volatile Uint32 SET_DATA;
    
    /** GPIO Clear Data Register */
    volatile Uint32 CLR_DATA;
    
    /** GPIO Set Rising Edge Interrupt Register */
    volatile Uint32 SET_RIS_TRIG;
    
    /** GPIO Clear Rising Edge Interrupt Register */
    volatile Uint32 CLR_RIS_TRIG;
    
    /** GPIO Set Falling Edge Interrupt Register */
    volatile Uint32 SET_FAL_TRIG;
    
    /** GPIO Clear Falling Edge Interrupt Register */
    volatile Uint32 CLR_FAL_TRIG;    
} CSL_GpioConfig;


/** @brief Default Values for GPIO Config structure */
#define CSL_GPIO_CONFIG_DEFAULTS {      \
    CSL_GPIO_BINTEN_RESETVAL ,          \
    CSL_GPIO_DIR_RESETVAL,              \
    CSL_GPIO_OUT_DATA_RESETVAL,         \
    CSL_GPIO_SET_DATA_RESETVAL,         \
    CSL_GPIO_CLR_DATA_RESETVAL,         \
    CSL_GPIO_SET_RIS_TRIG_RESETVAL,     \
    CSL_GPIO_CLR_RIS_TRIG_RESETVAL,     \
    CSL_GPIO_SET_FAL_TRIG_RESETVAL,     \
    CSL_GPIO_CLR_FAL_TRIG_RESETVAL,     \
}

/** Enumeration used for specifying the GPIO pin numbers */
typedef enum {
    /** Gpio pin 0 */
    CSL_GPIO_PIN0,
    /** Gpio pin 1 */
    CSL_GPIO_PIN1,
    /** Gpio pin 2 */
    CSL_GPIO_PIN2,
    /** Gpio pin 3 */
    CSL_GPIO_PIN3,
    /** Gpio pin 4 */
    CSL_GPIO_PIN4,
    /** Gpio pin 5 */
    CSL_GPIO_PIN5,
    /** Gpio pin 6 */
    CSL_GPIO_PIN6,
    /** Gpio pin 7 */
    CSL_GPIO_PIN7,
    /** Gpio pin 8 */
    CSL_GPIO_PIN8,
    /** Gpio pin 0 */
    CSL_GPIO_PIN9,
    /** Gpio pin 10 */
    CSL_GPIO_PIN10,
    /** Gpio pin 11 */
    CSL_GPIO_PIN11,
    /** Gpio pin 12 */
    CSL_GPIO_PIN12,
    /** Gpio pin 13 */
    CSL_GPIO_PIN13,
    /** Gpio pin 14 */
    CSL_GPIO_PIN14,
    /** Gpio pin 15 */
    CSL_GPIO_PIN15
} CSL_GpioPinNum;

/**\brief  Enums for configuring GPIO pin direction
 *
 */
typedef enum {
    CSL_GPIO_DIR_OUTPUT,/**<<b>: Output pin</b>*/
    CSL_GPIO_DIR_INPUT  /**<<b>: Input pin</b>*/
} CSL_GpioDirection;


/** \brief  Enums for configuring GPIO pin edge detection
 *
 */
typedef enum {
    /**<<b>: No edge detection </b>*/
    CSL_GPIO_TRIG_CLEAR_EDGE,  
    
    /**<<b>: Rising edge detection </b>*/
    CSL_GPIO_TRIG_RISING_EDGE, 
    
    /**<<b>: Falling edge detection </b>*/
    CSL_GPIO_TRIG_FALLING_EDGE,
    
    /**<<b>: Dual edge detection </b>*/
    CSL_GPIO_TRIG_DUAL_EDGE    
} CSL_GpioTriggerType;


/** \brief Enumeration for control commands passed to \a CSL_gpioHwControl()
 *
 * This is the set of commands that are passed to the \a CSL_gpioHwControl()
 * with an optional argument type-casted to \a void* .
 * The arguments to be passed with each enumeration (if any) are specified
 * next to the enumeration
 */
typedef enum {
    /**
     * @brief Enables interrupt on bank
     * @param ( None )
     */
    CSL_GPIO_CMD_BANK_INT_ENABLE = 1,

    /**
     * @brief Disables interrupt on bank
     * @param ( None )
     */
    CSL_GPIO_CMD_BANK_INT_DISABLE = 2,

    /**
     * @brief Configures GPIO pin direction and edge detection properties
     * @param ( CSL_GpioPinConfig )
     */
    CSL_GPIO_CMD_CONFIG_BIT = 3,

    /**
     * @brief Changes output state of GPIO pin to logic-1
     * @param ( CSL_GpioPinNum )
     */
    CSL_GPIO_CMD_SET_BIT = 4,

    /**
     * @brief Changes output state of GPIO pin to logic-0
     * @param ( CSL_GpioPinNum )
     */
    CSL_GPIO_CMD_CLEAR_BIT = 5,

    /**
     * @brief Gets the state of input pins on bank
     *        The "data" field act as output parameter reporting
     *        the input state of the GPIO pins on the bank.
     * @param ( CSL_BitMask16*)
     */
    CSL_GPIO_CMD_GET_INPUTBIT = 6,

    /**
     * @brief Gets the state of output pins on bank.
     *        The "data" field act as output parameter reporting the
     *        output drive state of the GPIO pins on the bank
     * @param ( CSL_BitMask16* )
     */
     CSL_GPIO_CMD_GET_OUTDRVSTATE = 7,

    /**
     * @brief Gets the state of input pin on bank.
     * @param ( CSL_GpioPinData * )
     */
     CSL_GPIO_CMD_GET_BIT = 8,

    /**
     * @brief Changes output state of GPIO pin to logic-1
     * @param ( CSL_GpioPinData *)
     */     
     CSL_GPIO_CMD_ENABLE_DISABLE_OUTBIT = 9
} CSL_GpioHwControlCmd;


/** \brief Enumeration for queries passed to \a CSL_GpioGetHwStatus()
 *
 * This is used to get the status of different operations.The arguments
 * to be passed with each enumeration if any are specified next to
 * the enumeration 
 */
typedef enum {
    /**
     * @brief Queries GPIO bank interrupt enable status
     * @param ( CSL_BitMask16* )
     */
    CSL_GPIO_QUERY_BINTEN_STAT = 2
} CSL_GpioHwStatusQuery;


/** \brief Input parameters for configuring a GPIO pin
 *  This is used to configure the direction and edge detection
 */
typedef struct {
    /**< Pin number for GPIO bank */
    CSL_GpioPinNum pinNum; 
    
    /**< Direction for GPIO Pin */
    CSL_GpioDirection direction; 

    /**< GPIO pin edge detection */
    CSL_GpioTriggerType trigger; 
} CSL_GpioPinConfig;

/**\brief this is used for getting a specific pin staus */
typedef struct {
    /**< Pin number for GPIO bank */
    CSL_GpioPinNum pinNum;

    /** pin value */
    Int16 pinVal;
} CSL_GpioPinData;



/**
   \brief Input parameters for setting up GPIO during startup
*
*  This is just a placeholder as GPIO is a simple module, which doesn't
*  require any setup
*/
typedef struct {
    /** The extendSetup is just a place-holder for future
     *  implementation.
     */
    void *extendSetup;
} CSL_GpioHwSetup;

/** \brief This object contains the reference to the instance of GPIO
*          opened using the @a CSL_gpioOpen()
*
* The pointer to this is passed to all GPIO CSL APIs
* This structure has the fields required to configure GPIO for any test
* case/application. It should be initialized as per requirements of a
* test case/application and passed on to the setup function
*/
typedef struct CSL_GpioObj {
    /**< This is a pointer to the registers of the
     *   instance of GPIO referred to by this object
     */
    CSL_GpioRegsOvly regs;

    /**< This is the instance of GPIO being referred to
     *   by this object 
     */
    CSL_InstNum gpioNum; 
    
    /**< This is the maximum number of pins supported by this
     *   instance of GPIO 
     */
    Uint8 numPins; 
} CSL_GpioObj;

/** \brief this is a pointer to @a CSL_GpioObj and is passed as the first
 * parameter to all GPIO CSL APIs 
 */
typedef CSL_GpioObj *CSL_GpioHandle;


/*****************************************************************************\
          CSL3.x mandatory function prototype definitions
\*****************************************************************************/

/** ===========================================================================
 *   @n@b CSL_gpioInit
 *
 *   @b Description
 *   @n This is the initialization function for the GPIO. This function is
 *      idempotent in that calling it many times is same as calling it once.
 *      This function initializes the CSL data structures, and doesn't affect
 *      the H/W.
 *
 *   @b Arguments
 *   @verbatim
           pContext   Context information for the instance. Should be NULL
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK - Always returns
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
      ...
        CSL_gpioInit();
     @endverbatim
 *  ===========================================================================
 */ 
CSL_Status CSL_gpioInit(
    CSL_GpioContext *pContext
);


/** ============================================================================
 *   @n@b CSL_gpioOpen
 *
 *   @b Description
 *   @n This function populates the peripheral dgpio object for the GPIO instance
 *      and returns a handle to the instance.
 *      The open call sets up the dgpio structures for the particular instance
 *      of GPIO device. The device can be re-opened anytime after it has been
 *      normally closed if so required. The handle returned by this call is
 *      input as an essential argument for rest of the APIs described
 *      for this module.
 *
 *   @b Arguments
 *   @verbatim
              pGpioObj      Pointer to the GPIO instance object

              gpioNum       Instance of the GPIO to be opened

              pGpioParam    Pointer to module specific parameters

              pStatus      pointer for returning status of the function call
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_GpioHandle
 *   @n                     Valid GPIO instance handle will be returned if status
                            value is equal to CSL_SOK.
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n   1.    GPIO object structure is populated
 *   @n   2.    The status is returned in the status variable. If status
 *              returned is
 *   @li            CSL_SOK             Valid gpio handle is returned
 *   @li            CSL_ESYS_FAIL       The gpio instance is invalid
 *   @li            CSL_ESYS_INVPARAMS  Invalid parameter
 *
 *   @b Modifies
 *   @n    1. The status variable
 *   @n    2. GPIO object structure
 *
 *   @b Example
 *   @verbatim
            CSL_status         status;
            CSL_GpioObj        gpioObj;
            CSL_GpioHandle     hGpio;
            ...

            hGpio = CSL_gpioOpen(&gpioObj, CSL_GPIO_PER_CNT, NULL, &status);
            ...
    @endverbatim
 * =============================================================================
 */ 
CSL_GpioHandle CSL_gpioOpen (
    CSL_GpioObj   *hGpioObj,
    CSL_InstNum   gpioNum,
    CSL_GpioParam *pGpioParam,
    CSL_Status    *status
);


/** ============================================================================
 *   @n@b CSL_gpioClose
 *
 *   @b Description
 *   @n This function closes the specified instance of GPIO.
 *
 *   @b Arguments
 *   @verbatim
            hGpio            Handle to the GPIO instance
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK             - Close successful
 *   @li                    CSL_ESYS_BADHANDLE  - Invalid handle
 *
 *   @b Example
 *   @verbatim
            CSL_GpioHandle       hGpio;
            CSL_status          status;

            ...

            status = CSL_gpioClose(hGpio);
     @endverbatim
 * =============================================================================
 */
CSL_Status CSL_gpioClose (
    CSL_GpioHandle hGpio
);


/** ============================================================================
 *   @n@b CSL_gpioHwSetup
 *
 *   @b Description
 *   @n It configures the gpio registers as per the values passed
 *      in the hardware setup structure.
 *
 *   @b Arguments
 *   @verbatim
            hGpio            Handle to the GPIO instance
            hwSetup          Pointer to harware setup structure
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK             - Hardware setup successful.
 *   @li                    CSL_ESYS_BADHANDLE  - Invalid handle
 *   @li                    CSL_ESYS_INVPARAMS  - Hardware structure is not
 *                                                 properly initialized
 *
 *   @b Modifies
 *   @n The hardware registers of GPIO.
 *
 *   @b Example
 *   @verbatim
        CSL_GpioHandle    hGpio;
        CSL_GpioObj       gpioObj;
        CSL_GpioHwSetup   hwSetup;
        CSL_status       status;

        ...

        hGpio = CSL_gpioOpen(&gpioObj, CSL_GPIO_PRIMARY, NULL, &status);

        status = CSL_gpioHwSetup(hGpio, &hwSetup);
    @endverbatim
 * =============================================================================
 */ 
CSL_Status CSL_gpioHwSetup (
    CSL_GpioHandle  hGpio,
    CSL_GpioHwSetup *setup
);


/** ===========================================================================
 *   @n@b CSL_gpioHwSetupRaw
 *
 *   @b Description
 *   @n This function initializes the device registers with the register-values
 *      provided through the Config Data structure.
 *
 *   @b Arguments
 *   @verbatim
            hGpio        Handle to the Gpio instance

            config       Pointer to config structure   
            
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK             - Configuration successful
 *   @li                    CSL_ESYS_BADHANDLE  - Invalid handle
 *   @li                    CSL_ESYS_INVPARAMS  - Configuration is not
 *                                                properly initialized
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  The registers of the specified GPIO instance will be setup
 *       according to value passed.
 *
 *   @b Modifies
 *   @n Hardware registers of the specified GPIO instance.
 *
 *   @b Example
 *   @verbatim
        CSL_GpioHandle       hGpio;
        CSL_GpioConfig       config = CSL_GPIO_CONFIG_DEFAULTS;
        CSL_Status          status;

        status = CSL_gpioHwSetupRaw(hGpio, &config);

     @endverbatim
 * ===========================================================================
 */ 
CSL_Status CSL_gpioHwSetupRaw (
    CSL_GpioHandle hGpio,
    CSL_GpioConfig *config    
);

/** ============================================================================
 *   @n@b CSL_gpioGetHwSetup
 *
 *   @b Description
 *   @n Gets the current setup of GPIO.
 *
 *   @b Arguments
 *   @verbatim
            hGpio            Handle to the GPIO instance

            setup           Pointer to setup structure which contains the
                            setup information of GPIO.
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK             - Setup info load successful.
 *   @li                    CSL_ESYS_BADHANDLE  - Invalid handle
 *   @li                    CSL_ESYS_INVPARAMS  - Invalid parameter
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  The registers of the specified GPIO instance will be setup.
 *
 *   @b Modifies
 *   @n Hardware registers of the specified GPIO instance.
 *
 *   @b Example
 *   @verbatim
        CSL_GpioHandle       hGpio;
        CSL_GpioHwSetup      setup;
        CSL_Status           status;

        status = CSL_gpioGetHwSetup(hGpio, &setup);
     @endverbatim
 * =============================================================================
 */
CSL_Status CSL_gpioGetHwSetup (
    CSL_GpioHandle  hGpio,
    CSL_GpioHwSetup *setup
);


/** ============================================================================
 *   @n@b CSL_gpioHwControl
 *
 *   @b Description
 *   @n Takes a command of GPIO with an optional argument & implements it.
 *
 *   @b Arguments
 *   @verbatim
            hGpio            Handle to the GPIO instance

            cmd             The command to this API indicates the action to be
                            taken on GPIO.

            arg             An optional argument.
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK             - Status info return successful.
 *   @li                    CSL_ESYS_BADHANDLE  - Invalid handle
 *   @li                    CSL_ESYS_INVCMD     - Invalid command
 *   @li                    CSL_ESYS_FAIL       - Invalid instance number
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n The hardware registers of GPIO.
 *
 *   @b Example
 *   @verbatim
        CSL_GpioHandle         hGpio;
        CSL_GpioHwControlCmd   cmd;
        void                   arg;

        status = CSL_gpioHwControl(hGpio, cmd, &arg);

     @endverbatim
 * =============================================================================
 */
CSL_Status CSL_gpioHwControl (
    CSL_GpioHandle       hGpio,
    CSL_GpioHwControlCmd cmd,
    void                 *arg
);


/** ============================================================================
 *   @n@b CSL_gpioGetHwStatus
 *
 *   @b Description
 *   @n Gets the status of the different operations of GPIO.
 *
 *   @b Arguments
 *   @verbatim
            hGpio            Handle to the GPIO instance

            query           The query to this API of GPIO which indicates the
                            status to be returned.

            response        Placeholder to return the status.
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK             - Status info return successful.
 *   @li                    CSL_ESYS_BADHANDLE  - Invalid handle
 *   @li                    CSL_ESYS_INVPARAMS  - Invalid parameter
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
        CSL_GpioHandle          hGpio;
        CSL_GpioHwStatusQuery   query;
        void                    reponse;

        status = CSL_gpioGetHwStatus(hGpio, query, &response);

     @endverbatim
 * =============================================================================
 */
CSL_Status CSL_gpioGetHwStatus(
    CSL_GpioHandle        hGpio,
    CSL_GpioHwStatusQuery query,
    void                  *response
);


/** ===========================================================================
 *   @n@b CSL_gpioGetBaseAddress
 *
 *   @b Description
 *   @n  Function to get the base address of the peripheral instance.
 *       This function is used for getting the base address of the peripheral
 *       instance. This function will be called inside the CSL_gpioOpen()
 *       function call. This function is open for re-implementing if the user
 *       wants to modify the base address of the peripheral object to point to
 *       a different location and there by allow CSL initiated write/reads into
 *       peripheral MMR's go to an alternate location.
 *
 *   @b Arguments
 *   @verbatim
            gpioNum          Specifies the instance of GPIO to be opened.

            pGpioParam       Module specific parameters.

            pBaseAddress    Pointer to baseaddress structure containing base
                            address details.

     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_OK            Open call is successful
 *   @li                    CSL_ESYS_FAIL     The instance number is invalid.
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  Base Address structure is populated
 *
 *   @b Modifies
 *   @n    1. The status variable
 *
 *         2. Base address structure is modified.
 *
 *   @b Example
 *   @verbatim
        CSL_Status           status;
        CSL_GpioBaseAddress  baseAddress;

       ...
      status = CSL_gpioGetBaseAddress(CSL_GPIO_PER_CNT, NULL, &baseAddress);

    @endverbatim
 * ===========================================================================
 */
CSL_Status CSL_gpioGetBaseAddress (
    CSL_InstNum         gpioNum,
    CSL_GpioParam       *pGpioParam,
    CSL_GpioBaseAddress *pBaseAddress
);


#ifdef __cplusplus
}
#endif

#endif /*_CSL_GPIO_H_*/
