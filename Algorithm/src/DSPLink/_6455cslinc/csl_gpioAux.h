/* ============================================================================
 * Copyright (c) Texas Instruments Inc 2002, 2003, 2004
 *
 * Use of this software is controlled by the terms and conditions found in 
 * the license agreement under which this software has been supplied.
 * ============================================================================
 */

/** ===========================================================================
 * @file csl_gpioAux.h
 *
 * @brief APIs Auxilary header file for GPIO CSL
 * 
 * @Path $(CSLPATH)\inc
 *
 * @desc Control command and status query Functions of gpio
 * ============================================================================
 */

/* ============================================================================
 * Revision History
 * ===============
 *  04-Sep-2004 Nsr File Created from CSL_gpioHwControl.c And 
 *                  CSL_gpioGetHwStatus.c.
 *  04-Sep-2004 Nsr Added inline functions for query commands also.
 *  11-Oct-2004 Nsr Changed this file according to review commnets.
 *  22-Feb-2005 Nsr Added CSL_gpioGetBit API according to TI Issue PSG00000310.
 *  03-Mar-2005 Nsr Updated changes (PSG00000289) in CSL_gpioSetBit and 
 *                  CSL_gpioClearBit.
 *  28-Jul-2005 PSK updated CSL source to support only ONE BANK.
 *                  bank "index" is removed.
 *  11-Jan-2006 NG  Added CSL_GPIO_CMD_SET_OUT_BIT Control Command
 *  06-Mar-2006 ds  Changed CSL_GPIO_CMD_SET_OUT_BIT cmd to clear and set the
 *                  outbit and Renamed CSL_GPIO_CMD_SET_OUT_BIT to 
 *                  CSL_GPIO_CMD_ENABLE_DISABLE_OUTBIT
 * ============================================================================
 */

#ifndef _CSL_GPIOAUX_H_
#define _CSL_GPIOAUX_H_

#include <csl_gpio.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Control command and status query Functions of gpio. */

/** ===========================================================================
 *   @n@b CSL_gpioBankIntEnable
 *
 *   @b Description
 *   @n This function enables the GPIO bank interrupt .
 *
 *   @b Arguments
 *   @verbatim

            hGpio            Handle to GPIO instance.
            
     @endverbatim
 *
 *   <b> Return Value </b>  
 *    @n CSL_SOK
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *    @n Bank interrupt will be enabled
 *
 *   @b Modifies
 *   @n  BINTEN register
 *
 *   @b Example
 *   @verbatim

        CSL_gpioBankIntEnable(hGpio);

     @endverbatim
 * ============================================================================
 */
static inline
CSL_Status CSL_gpioBankIntEnable (
    CSL_GpioHandle hGpio
)
{   
    CSL_FINSR(hGpio->regs->BINTEN, 0, 0, TRUE);

    return CSL_SOK;
} 


    
/** ===========================================================================
 *   @n@b CSL_gpioBankIntDisable
 *
 *   @b Description
 *   @n This function disables the GPIO bank interrupt .
 *
 *   @b Arguments
 *   @verbatim

            hGpio            Handle to GPIO instance.
            
     @endverbatim
 *
 *   <b> Return Value </b>  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *    @n Bank interrupt will be disabled
 *
 *   @b Modifies
 *   @n  BINTEN register
 *
 *   @b Example
 *   @verbatim

        CSL_gpioBankIntDisable(hGpio);

     @endverbatim
 * ============================================================================
 */
static inline
CSL_Status CSL_gpioBankIntDisable (
    CSL_GpioHandle hGpio
)
{   
    CSL_FINSR(hGpio->regs->BINTEN, 0, 0, FALSE);

    return CSL_SOK;
}    

    
/** ===========================================================================
 *   @n@b CSL_gpioConfigBit
 *
 *   @b Description
 *   @n This function used to configure the direction and rising and falling 
        edge trigger registers.
 *
 *   @b Arguments
 *   @verbatim

            hGpio            Handle to GPIO instance
            config           Pointer to the CSL_GpioPinConfig structure.
            
     @endverbatim
 *
 *   <b> Return Value </b>  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *    @n Configures any of the 16 GPIO signals
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim

        CSL_gpioConfigBit (hGpio, &config);

     @endverbatim
 * ============================================================================
 */
static inline
CSL_Status CSL_gpioConfigBit (
    CSL_GpioHandle    hGpio,
    CSL_GpioPinConfig *config
)
{   
    CSL_Status status = CSL_SOK;
    CSL_GpioPinNum pinNum;
    CSL_GpioTriggerType trigger;
    CSL_GpioRegsOvly gpioRegs = hGpio->regs;
    
    pinNum = config->pinNum;
    
    if (pinNum < hGpio->numPins) {
        pinNum = (CSL_GpioPinNum)(pinNum & 0x1f);
        trigger = config->trigger;
        
        CSL_FINSR(gpioRegs->DIR, pinNum, pinNum, config->direction);
                 
        if (trigger & CSL_GPIO_TRIG_RISING_EDGE) { 
    	     CSL_FINSR(gpioRegs->SET_RIS_TRIG, pinNum, pinNum, TRUE); 
    	}
    	else {
    	    CSL_FINSR(gpioRegs->CLR_RIS_TRIG, pinNum, pinNum, TRUE); 
    	}
        
    	if (trigger & CSL_GPIO_TRIG_FALLING_EDGE) { 
    	    CSL_FINSR(gpioRegs->SET_FAL_TRIG, pinNum, pinNum, TRUE); 
    	}
        else {
            CSL_FINSR (gpioRegs->CLR_FAL_TRIG, pinNum, pinNum, TRUE); 
        }                          
    } 
    else {
        status = CSL_EGPIO_INVPARAM;     
    }
      
    return status;                
}    
    


/** ===========================================================================
 *   @n@b CSL_gpioSetBit
 *
 *   @b Description
 *   @n This function sets the bit value of SET_DATA register in the GPIO 
 *      module.
 * 
 *   @b Arguments
 *   @verbatim

            hGpio            Handle to GPIO instance
            pinNum           This variable holds the GPIO pin number.  
            
     @endverbatim
 *
 *   <b> Return Value </b>  None
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

        CSL_gpioSetBit(hGpio,pinNum);

     @endverbatim
 * ============================================================================
 */
static inline
CSL_Status CSL_gpioSetBit (
    CSL_GpioHandle hGpio,
    CSL_GpioPinNum pinNum
)
{   
    CSL_Status status = CSL_SOK;
    CSL_GpioRegsOvly gpioRegs = hGpio->regs;

    if (pinNum < hGpio->numPins) {
        pinNum = (CSL_GpioPinNum)(pinNum & 0x1f);    
        gpioRegs->SET_DATA = (1<<pinNum);
    } 
    else {
        status = CSL_EGPIO_INVPARAM;     
    }   
    
    return status;       
}    

/** ===========================================================================
 *   @n@b CSL_gpioEnDisOutBit
 *
 *   @b Description
 *   @n This function sets and clear the bit value of OUT_DATA register in the 
 *      GPIO 
 *      module.
 * 
 *   @b Arguments
 *   @verbatim

            hGpio            Handle to GPIO instance
            pinData          Pointer to Gpio PinData structure 
            
     @endverbatim
 *
 *   <b> Return Value </b>  None
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

        CSL_gpioSetOutBit(hGpio,pinNum);

     @endverbatim
 * ============================================================================
 */
static inline
CSL_Status CSL_gpioEnDisOutBit (
    CSL_GpioHandle  hGpio,
    CSL_GpioPinData *pinData
)
{   
    CSL_Status status = CSL_SOK;
    CSL_GpioRegsOvly gpioRegs = hGpio->regs;

    if (pinData->pinNum < hGpio->numPins) {
        pinData->pinNum = (CSL_GpioPinNum)(pinData->pinNum & 0x1f);    
        CSL_FINSR(gpioRegs->OUT_DATA, pinData->pinNum, pinData->pinNum, \
                  pinData->pinVal);
    } 
    else {
        status = CSL_EGPIO_INVPARAM;     
    }   
    
    return status; 
}    

    

/** ===========================================================================
 *   @n@b CSL_gpioClearBit
 *
 *   @b Description
 *   @n This function sets the bit value of CLR_DATA  register.
 *
 *   @b Arguments
 *   @verbatim

            hGpio            Handle to GPIO instance
            pinNum           This variable holds the GPIO pin number.  
            
     @endverbatim
 *
 *   <b> Return Value </b>  None
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

        CSL_gpioClearBit(hGpio,pinNum);

     @endverbatim
 * ============================================================================
 */
static inline
CSL_Status CSL_gpioClearBit (
    CSL_GpioHandle hGpio,
    CSL_GpioPinNum pinNum
)
{   
    CSL_Status status = CSL_SOK;
    CSL_GpioRegsOvly gpioRegs = hGpio->regs;

    if (pinNum < hGpio->numPins) {
        pinNum = (CSL_GpioPinNum)(pinNum & 0x1f);
        gpioRegs->CLR_DATA = (1<<pinNum);
    }
    else {
        status = CSL_EGPIO_INVPARAM;     
    }   
    
    return status;       
}    



/** ===========================================================================
 *   @n@b CSL_gpioGetInputBit
 *
 *   @b Description
 *   @n This function gets the IN_DATA register value of the GPIO bank.
 *
 *   @b Arguments
 *   @verbatim

            hGpio            Handle to GPIO instance
            value            Read IN_DATA register value.
            
     @endverbatim
 *
 *   <b> Return Value </b>  None
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

        CSL_gpioGetInputBit(hGpio,&value);

     @endverbatim
 * ============================================================================
 */
static inline
CSL_Status CSL_gpioGetInputBit (
    CSL_GpioHandle hGpio,
    CSL_BitMask16  *value
)
{   
    *(CSL_BitMask16 *)value = hGpio->regs->IN_DATA;    

    return CSL_SOK;
}    


/** ===========================================================================
 *   @n@b CSL_gpioGetBit
 *
 *   @b Description
 *   @n This function gets the Pin number status of GPIO module.
 *   @b Arguments
 *   @verbatim

            hGpio           Handle to GPIO instance

            pinNum          pin number to get the status.

     @endverbatim
 *
 *   <b> Return Value </b>  
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

        CSL_gpioGetBit(hGpio,pinNum);

     @endverbatim
 * ===========================================================================
 */
static inline
CSL_Status CSL_gpioGetBit (
    CSL_GpioHandle   hGpio,
    CSL_GpioPinData  *pinData
)
{ 
    CSL_Status status = CSL_SOK;
    CSL_GpioRegsOvly gpioRegs = hGpio->regs;

    if (pinData->pinNum < hGpio->numPins) {
        pinData->pinNum = (CSL_GpioPinNum)(pinData->pinNum & 0x1f);
        pinData->pinVal = CSL_FEXTR(gpioRegs->IN_DATA, pinData->pinNum, 
                                                        pinData->pinNum);
    }
    else {
        pinData->pinVal = CSL_ESYS_FAIL;     
        status = CSL_EGPIO_INVPARAM;
    }   
    
    return status;
}



/** ===========================================================================
 *   @n@b CSL_gpioGetOutDrvState
 *
 *   @b Description
 *   @n This function gets the OUT_DATA value of the GPIO bank.
 *
 *   @b Arguments
 *   @verbatim

            hGpio            Handle to GPIO instance
            value		     Read OUT_DATA register value.
            
     @endverbatim
 *
 *   <b> Return Value </b>  None
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

        CSL_gpioGetOutDrvState(hGpio,&value);

     @endverbatim
 * ===========================================================================
 */
static inline
CSL_Status CSL_gpioGetOutDrvState (
    CSL_GpioHandle hGpio,
    void           *value
)
{   
    *(CSL_BitMask16 *)value = hGpio->regs->OUT_DATA;    

    return CSL_SOK;
}    



/** ===========================================================================
 *   @n@b CSL_gpioGetBintenStat
 *
 *   @b Description
 *   @n This function gets the interrupt capabilities of all banks in GPIO.
 *
 *   @b Arguments
 *   @verbatim

            hGpio           Handle to GPIO instance

            response        Placeholder to return status.

     @endverbatim
 *
 *   <b> Return Value </b>  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n None
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim

        CSL_gpioGetBintenStat(hGpio, &response);

     @endverbatim
 * ===========================================================================
 */
static inline
void CSL_gpioGetBintenStat (
    CSL_GpioHandle hGpio,
    void           *response
)
{
    *(CSL_BitMask16 *)response = hGpio->regs->BINTEN;
}


#ifdef __cplusplus
extern "C" {
#endif

#endif /* CSL_GPIOAUX_H_ */

