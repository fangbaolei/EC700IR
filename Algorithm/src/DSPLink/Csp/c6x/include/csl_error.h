/*   ==========================================================================
 *   Copyright (c) Texas Instruments Inc , 2004
 *
 *   Use of this software is controlled by the terms and conditions found
 *   in the license agreement under which this software has been supplied
 *   provided
 *   ==========================================================================
*/

/* ---- File: <csl_error.h> ---- */
/* Error file for Davinci: ARM side */
#ifndef _CSL_ERROR_H_
#define _CSL_ERROR_H_

#include <csl_resId.h>

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

#define CSL_EVLYNQ_FIRST    -( ((CSL_VLYNQ_ID + 1) << 5 ) + 1 )
#define CSL_EVLYNQ_LAST     -( (CSL_VLYNQ_ID + 1) << 6 )

#define CSL_EI2C_FIRST      -( ((CSL_I2C_ID + 1) << 5 ) + 1 )
#define CSL_EI2C_LAST       -( (CSL_I2C_ID + 1) << 6 )

#define CSL_EGPIO_FIRST     -( ((CSL_GPIO_ID + 1) << 5 ) + 1 )
#define CSL_EGPIO_LAST      -( (CSL_GPIO_ID + 1) << 6 )

#define CSL_EEMIF_FIRST     -( ((CSL_EMIF_ID + 1) << 5 ) + 1 )
#define CSL_EEMIF_LAST      -( (CSL_EMIF_ID + 1) << 6 )

#define CSL_EPLLC_FIRST     -( ((CSL_PLLC_ID + 1) << 5 ) + 1 )
#define CSL_EPLLC_LAST      -( (CSL_PLLC_ID + 1) << 6 )

#define CSL_EDDR_FIRST      -( ((CSL_DDR_ID + 1) << 5 ) + 1 )
#define CSL_EDDR_LAST       -( (CSL_DDR_ID + 1) << 6 )

#define CSL_ETMR_FIRST     -( ((CSL_TMR_ID + 1) << 5 ) + 1 )
#define CSL_ETMR_LAST      -( (CSL_TMR_ID + 1) << 6 )

#define CSL_EUHPI_FIRST     -( ((CSL_UHPI_ID + 1) << 5 ) + 1 )
#define CSL_EUHPI_LAST      -( (CSL_UHPI_ID + 1) << 6 )

#define CSL_EMCASP_FIRST    -( ((CSL_MCASP_ID + 1) << 5 ) + 1 )
#define CSL_EMCASP_LAST     -( (CSL_MCASP_ID + 1) << 6 )

#define CSL_EEDMA_FIRST     -( ((CSL_EDMA_ID + 1) << 5 ) + 1 )
#define CSL_EEDMA_LAST      -( (CSL_EDMA_ID + 1) << 6 )

#define CSL_EUSB_FIRST     -( ((CSL_USB_ID + 1) << 5 ) + 1 )
#define CSL_EUSB_LAST      -( (CSL_USB_ID + 1) << 6 )

#define CSL_EGE_FIRST     -( ((CSL_GE_ID + 1) << 5 ) + 1 )
#define CSL_EGE_LAST      -( (CSL_GE_ID + 1) << 6 )

#define CSL_ESPIO_FIRST     -( ((CSL_SPIO_ID + 1) << 5 ) + 1 )
#define CSL_ESPIO_LAST      -( (CSL_SPIO_ID + 1) << 6 )

#define CSL_ECRGEN_FIRST     -( ((CSL_CRGEN_ID + 1) << 5 ) + 1 )
#define CSL_ECRGEN_LAST      -( (CSL_CRGEN_ID + 1) << 6 )

#define CSL_EVPIF_FIRST     -( ((CSL_VPIF_ID + 1) << 5 ) + 1 )
#define CSL_EVPIF_LAST      -( (CSL_VPIF_ID + 1) << 6 )

#define CSL_ETOP_FIRST     -( ((CSL_TOP_ID + 1) << 5 ) + 1 )
#define CSL_ETOP_LAST      -( (CSL_TOP_ID + 1) << 6 )

#define CSL_EECD_FIRST     -( ((CSL_ECD_ID + 1) << 5 ) + 1 )
#define CSL_EECD_LAST      -( (CSL_ECD_ID + 1) << 6 )

#define CSL_ECALC_FIRST     -( ((CSL_CALC_ID + 1) << 5 ) + 1 )
#define CSL_ECALC_LAST      -( (CSL_CALC_ID + 1) << 6 )

#define CSL_EBS_FIRST     -( ((CSL_BS_ID + 1) << 5 ) + 1 )
#define CSL_EBS_LAST      -( (CSL_BS_ID + 1) << 6 )

#define CSL_EMC_FIRST     -( ((CSL_MC_ID + 1) << 5 ) + 1 )
#define CSL_EMC_LAST      -( (CSL_MC_ID + 1) << 6 )

#define CSL_ELPF_FIRST     -( ((CSL_LPF_ID + 1) << 5 ) + 1 )
#define CSL_ELPF_LAST      -( (CSL_LPF_ID + 1) << 6 )

#define CSL_EME_FIRST     -( ((CSL_ME_ID + 1) << 5 ) + 1 )
#define CSL_EME_LAST      -( (CSL_ME_ID + 1) << 6 )

#define CSL_EIPE_FIRST     -( ((CSL_IPE_ID + 1) << 5 ) + 1 )
#define CSL_EIPE_LAST      -( (CSL_IPE_ID + 1) << 6 )

#define CSL_ECLKC_FIRST     -( ((CSL_CLKC_ID + 1) << 5 ) + 1 )
#define CSL_ECLKC_LAST      -( (CSL_CLKC_ID + 1) << 6 )

#define CSL_EBFSWC_FIRST     -( ((CSL_BFSWC_ID + 1) << 5 ) + 1 )
#define CSL_EBFSWC_LAST      -( (CSL_BFSWC_ID + 1) << 6 )

#define CSL_EINTC_FIRST     -( ((CSL_INTC_ID + 1) << 5 ) + 1 )
#define CSL_EINTC_LAST      -( (CSL_INTC_ID + 1) << 6 )

#define CSL_EICEC_FIRST     -( ((CSL_ICEC_ID + 1) << 5 ) + 1 )
#define CSL_EICEC_LAST      -( (CSL_ICEC_ID + 1) << 6 )

#endif /* _CSL_ERROR_H_ */

/* Rev.No.   Date/Time               ECN No.          Modifier      */
/* -------   ---------               -------          --------      */

/* 6          Dec 14:17:09 7         4658             xkeshavm      */
/*                                                                  */
/* Uploaded the CSL Dec6 2004 Release                               */
/********************************************************************/ 
