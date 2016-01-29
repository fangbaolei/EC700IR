/*   ==========================================================================
 *   Copyright (c) Texas Instruments Inc 2004, 2005
 *
 *   Use of this software is controlled by the terms and conditions found
 *   in the license agreement under which this software has been supplied
 *   provided
 *   ==========================================================================
*/

/* ---- File: <csl_error.h> ---- */
#ifndef _CSL_ERROR_H_
#define _CSL_ERROR_H_

/* Below Error codes are Global across all CSL Modules. */
#define CSL_SOK                 (1)         /* Success */
#define CSL_ESYS_FAIL           (-1)        /* Generic failure */
#define CSL_ESYS_INUSE          (-2)        /* Peripheral resource is already in use */
#define CSL_ESYS_XIO            (-3)        /* Encountered a shared I/O(XIO) pin conflict */
#define CSL_ESYS_OVFL           (-4)        /* Encoutered CSL system resource overflow */
#define CSL_ESYS_BADHANDLE      (-5)        /* Handle passed to CSL was invalid */
#define CSL_ESYS_INVPARAMS      (-6)        /* invalid parameters */
#define CSL_ESYS_INVCMD         (-7)        /* invalid command */
#define CSL_ESYS_INVQUERY       (-8)        /* invalid query */
#define CSL_ESYS_NOTSUPPORTED   (-9)        /* action not supported */

/* Peripheral IDs */
#define CSL_MCBSP_ID            (7)
#define CSL_GPIO_ID             (8)

/* Error codes individual to various modules. */

/* Error code for DMA, individual error would be assigned as
 * eg: #define CSL_E<Peripheral name>_<error code>  CSL_EDMA_FIRST - 1
 */


#define CSL_EPWM_FIRST      -( ((CSL_PWM_ID + 1) << 5 ) + 1 )
#define CSL_EPWM_LAST       -( (CSL_PWM_ID + 1) << 6 )

#define CSL_EUART_FIRST     -( ((CSL_UART_ID + 1) << 5 ) + 1 )
#define CSL_EUART_LAST      -( (CSL_UART_ID + 1) << 6 )

#define CSL_ESPI_FIRST      -( ((CSL_SPI_ID + 1) << 5 ) + 1 )
#define CSL_ESPI_LAST       -( (CSL_SPI_ID + 1) << 6 )

#define CSL_EATA_FIRST      -( ((CSL_ATA_ID + 1) << 5 ) + 1 )
#define CSL_EATA_LAST       -( (CSL_ATA_ID + 1) << 6 )

#define CSL_EMMCSD_FIRST    -( ((CSL_MMCSD_ID + 1) << 5 ) + 1 )
#define CSL_EMMCSD_LAST     -( (CSL_MMCSD_ID + 1) << 6 )

#define CSL_EVLYNQ_FIRST    -( ((CSL_VLYNQ_ID + 1) << 5 ) + 1 )
#define CSL_EVLYNQ_LAST     -( (CSL_VLYNQ_ID + 1) << 6 )

#define CSL_EMCBSP_FIRST    -( ((CSL_MCBSP_ID + 1) << 5 ) + 1 )
#define CSL_EMCBSP_LAST     -( (CSL_MCBSP_ID + 1) << 6 )

#define CSL_EI2C_FIRST      -( ((CSL_I2C_ID + 1) << 5 ) + 1 )
#define CSL_EI2C_LAST       -( (CSL_I2C_ID + 1) << 6 )

#define CSL_EGPIO_FIRST     -( ((CSL_GPIO_ID + 1) << 5 ) + 1 )
#define CSL_EGPIO_LAST      -( (CSL_GPIO_ID + 1) << 6 )

#define CSL_EMS_FIRST       -( ((CSL_MS_ID + 1) << 5 ) + 1 )
#define CSL_EMS_LAST        -( (CSL_MS_ID + 1) << 6 )

#define CSL_EINTC_FIRST     -( ((CSL_INTC_ID + 1) << 5 ) + 1 )
#define CSL_EINTC_LAST      -( (CSL_INTC_ID + 1) << 6 )

#define CSL_EEMIF_FIRST     -( ((CSL_EMIF_ID + 1) << 5 ) + 1 )
#define CSL_EEMIF_LAST      -( (CSL_EMIF_ID + 1) << 6 )

#define CSL_EPLLC_FIRST     -( ((CSL_PLLC_ID + 1) << 5 ) + 1 )
#define CSL_EPLLC_LAST      -( (CSL_PLLC_ID + 1) << 6 )

#define CSL_EDDR_FIRST      -( ((CSL_DDR_ID + 1) << 5 ) + 1 )
#define CSL_EDDR_LAST       -( (CSL_DDR_ID + 1) << 6 )

#define CSL_EEDMA_FIRST     -( ((CSL_EDMA_ID + 1) << 5 ) + 1 )
#define CSL_EEDMA_LAST      -( (CSL_EDMA_ID + 1) << 6 )

#endif /* _CSL_ERROR_H_ */

