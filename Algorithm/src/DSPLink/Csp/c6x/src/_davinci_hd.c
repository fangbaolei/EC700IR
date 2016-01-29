/*   ==========================================================================
 *   Copyright (c) Texas Instruments Inc , 2004
 *
 *   Use of this software is controlled by the terms and conditions found
 *   in the license agreement under which this software has been supplied
 *   provided
 *   ==========================================================================
*/
#include <davinci_hd.h>

void * _CSL_pwmlookup[]  = {(void *) CSL_PWM_0_REGS, 
                            (void *) CSL_PWM_1_REGS};
void * _CSL_uartlookup[] = {(void *) CSL_UART_0_REGS, 
                            (void *) CSL_UART_1_REGS,
                            (void *) CSL_UART_2_REGS};
void * _CSL_spilookup[]  = {(void *) CSL_SPI_0_REGS};
//void * _CSL_atalookup[]  = {(void *) CSL_ATA_0_REGS};
void * _CSL_vlynqlookup[]= {(void *) CSL_VLYNQ_0_REGS};
void * _CSL_i2clookup[]  = {(void *) CSL_I2C_0_REGS};
void * _CSL_gpiolookup[] = {(void *) CSL_GPIO_REGS};
void * _CSL_emiflookup[] = {(void *) CSL_EMIF_0_REGS};
void * _CSL_pllclookup[] = {(void *) CSL_PLLC_0_REGS,
                            (void *) CSL_PLLC_1_REGS};
void * _CSL_ddrlookup[]  = {(void *) CSL_DDR_0_REGS};                            

void * _CSL_gelookup[]         = {(void *) CSL_GE_0_REGS};
void * _CSL_spiolookup[]       = {(void *) CSL_SPIO_0_REGS, 
                                  (void *) CSL_SPIO_1_REGS};
void * _CSL_crgenlookup[]      = {(void *) CSL_CRGEN_0_REGS, 
                                  (void *) CSL_CRGEN_1_REGS};
void * _CSL_vpiflookup[]       = {(void *) CSL_VPIF_0_REGS};
void * _CSL_edmacclookup[]     = {(void *) CSL_EDMACC_0_REGS};
void * _CSL_edmatclookup[]     = {(void *) CSL_EDMATC_0_REGS,
                                  (void *) CSL_EDMATC_1_REGS,
                                  (void *) CSL_EDMATC_2_REGS,
                                  (void *) CSL_EDMATC_3_REGS};

/* Rev.No.   Date/Time               ECN No.          Modifier      */
/* -------   ---------               -------          --------      */

/* 8          Dec 14:17:15 7         4658             xkeshavm      */
/*                                                                  */
/* Uploaded the CSL Dec6 2004 Release                               */
/********************************************************************/ 
/* Rev.No.   Date/Time               ECN No.          Modifier      */
/* -------   ---------               -------          --------      */

/* 9          Dec 12:34:35 8         4698             xkeshavm      */
/*                                                                  */
/* Recompiled lib after adding the osd, vpbe and venc definitions   */
/********************************************************************/ 
