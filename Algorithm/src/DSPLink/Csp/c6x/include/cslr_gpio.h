/*   ==========================================================================
 *   Copyright (c) Texas Instruments Inc , 2004
 *
 *   Use of this software is controlled by the terms and conditions found
 *   in the license agreement under which this software has been supplied
 *   provided
 *   ==========================================================================
*/
#ifndef _CSLR_GPIO_H_
#define _CSLR_GPIO_H_

#include <cslr.h>
#include <cslr_gpio_002.h>
#include <davinci_hd.h>

#define CSL_GPIO_NUM_PINS          (48)

/* #define CSL_GPIO_NUM_BANKS       (CSL_GPIO_NUM_PINS + 15)/16 */

#define CSL_GPIO_NUM_BANKS         (3)

           
typedef enum {
  CSL_GPIO_PIN0,
  CSL_GPIO_PIN1,
  CSL_GPIO_PIN2,
  CSL_GPIO_PIN3,
  CSL_GPIO_PIN4,
  CSL_GPIO_PIN5,
  CSL_GPIO_PIN6,
  CSL_GPIO_PIN7,
  CSL_GPIO_PIN8,
  CSL_GPIO_PIN9,
  CSL_GPIO_PIN10,
  CSL_GPIO_PIN11,
  CSL_GPIO_PIN12,
  CSL_GPIO_PIN13,
  CSL_GPIO_PIN14,
  CSL_GPIO_PIN15,
  CSL_GPIO_PIN16,
  CSL_GPIO_PIN17,
  CSL_GPIO_PIN18,
  CSL_GPIO_PIN19,
  CSL_GPIO_PIN20,
  CSL_GPIO_PIN21,
  CSL_GPIO_PIN22,
  CSL_GPIO_PIN23,
  CSL_GPIO_PIN24,
  CSL_GPIO_PIN25,
  CSL_GPIO_PIN26,
  CSL_GPIO_PIN27,
  CSL_GPIO_PIN28,
  CSL_GPIO_PIN29,
  CSL_GPIO_PIN30,
  CSL_GPIO_PIN31,
  CSL_GPIO_PIN32,
  CSL_GPIO_PIN33,
  CSL_GPIO_PIN34,
  CSL_GPIO_PIN35,
  CSL_GPIO_PIN36,
  CSL_GPIO_PIN37,
  CSL_GPIO_PIN38,
  CSL_GPIO_PIN39,
  CSL_GPIO_PIN40,
  CSL_GPIO_PIN41,
  CSL_GPIO_PIN42,
  CSL_GPIO_PIN43,
  CSL_GPIO_PIN44,
  CSL_GPIO_PIN45,
  CSL_GPIO_PIN46,
  CSL_GPIO_PIN47
} CSL_GpioPinNum;

typedef enum {
  CSL_GPIO_BANK0,
  CSL_GPIO_BANK1,
  CSL_GPIO_BANK2
} CSL_GpioBankNum;

/*****************************************************************************\
             Overlay structure typedef definition
\*****************************************************************************/

CSL_IDEF_INLINE CSL_GpioRegsOvly _CSL_gpioGetBaseAddr (Uint16 gpioNum) {
  return (CSL_GpioRegsOvly)_CSL_gpiolookup[gpioNum];
}

#endif  /* CSLR_GPIO_H_ */
/* Rev.No.   Date/Time               ECN No.          Modifier      */
/* -------   ---------               -------          --------      */

/* 5         14 Jan 2005 13:32:28    5888             xkeshavm      */
/*                                                                  */
/* Uploaded the CSL0.57 JAN 2005 Release and built the library for ARM and DSP*/
/********************************************************************/ 
