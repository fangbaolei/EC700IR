/*  ============================================================================
 *   Copyright (c) Texas Instruments Inc 2002, 2003, 2004, 2005, 2006
 *
 *   Use of this software is controlled by the terms and conditions found in the
 *   license agreement under which this software has been supplied.
 *   ===========================================================================
 */
 
/** ============================================================================
 *   @file  csl_ectl.c
 *
 *   @path  $(CSLPATH)\src\ectl
 *
 *   @desc  Header file for functional layer CSL of ECTL
 */

/* =============================================================================
 * Revision History
 * ===============
 *  01-May-2006 NS  updated the file for DOXYGEN compatibiliy
 *  12-Jan-2006 PSK file created 
 * =============================================================================
 */                                      


#include <csl_ectl.h>


/** ============================================================================
 *   @n@b ECTL_config
 *
 *   @b Description
 *   @n Module configuration is achieved by calling ECTL_config().
 *
 *   <b> Return Value </b>  Uint32
 *   @li                '0'                 - Configuration successful
 *   @li                ECTL_ERROR_INVALID  - Invalid config structure
 *
 *   <b> Pre Condition </b>
 *   @n  None.
 *
 *   <b> Post Condition </b>
 *   @n  ECTL module is configured
 *
 *   @b Modifies    
 *   @n  Memory mapped registers ECTL are modified.
 *
 *   @b Example
 *   @verbatim
        ECTL_Config pEctlConfig;  
        
        pEctlConfig.intrPaceEn = 0x1;
        pEctlConfig.intrPrescale = 0x100;
        ...
        
        ECTL_config ();

     @endverbatim
 * =============================================================================
 */
Uint32 ECTL_config (ECTL_Config *pEctlConfig)
{
    int i = 0;    
    /* Validate our handle */
    if (!pEctlConfig) {
        return(ECTL_ERROR_INVALID);
    }

    /*Soft resets the module, this is an immediate effect */

    CSL_FINST(ECTL_REGS->SOFT_RESET, ECTL_SOFT_RESET_SOFT_RESET, YES);

    /*Enales inteerupt pacing and lods prescale value */
    CSL_FINS(ECTL_REGS->INT_CONTROL, ECTL_INT_CONTROL_INT_PACE_EN, pEctlConfig->intrPaceEn);
    CSL_FINS(ECTL_REGS->INT_CONTROL, ECTL_INT_CONTROL_INT_PRESCALE, pEctlConfig->intrPrescale);

    /*Configure Rx thresh, Rx, Tx inteerupts for core (0, 1 and 2)*/
    for (i = 0; i < NUM_OF_CORES; i++) {
        CSL_FINS(ECTL_REGS->CONTROL[i].C_RX_THRESH_EN, ECTL_C_RX_THRESH_EN_C0_RX_THRESH_EN, pEctlConfig->rxThreshEn[i]);
        CSL_FINS(ECTL_REGS->CONTROL[i].C_RX_EN,  ECTL_C_RX_EN_C_RX_EN, pEctlConfig->rxEn[i]);
        CSL_FINS(ECTL_REGS->CONTROL[i].C_TX_EN,  ECTL_C_TX_EN_C_TX_EN, pEctlConfig->txEn[i]);
        CSL_FINS(ECTL_REGS->CONTROL[i].C_MISC_EN, ECTL_C_MISC_EN_C_MISC_EN, pEctlConfig->miscEn[i]);
    }

    /*Load imax value for Rx/Tx interrupts per millisecond */
    for (i = 0; i < NUM_OF_CORES; i++) {
        CSL_FINS(ECTL_REGS->INTR_COUNT[i].C_RX_IMAX, ECTL_C_RX_IMAX_C_RX_IMAX, pEctlConfig->rxImaxLoad[i]);
        CSL_FINS(ECTL_REGS->INTR_COUNT[i].C_TX_IMAX, ECTL_C_TX_IMAX_C_TX_IMAX, pEctlConfig->txImaxLoad[i]);
    }

    return 0;
}


/** ============================================================================
 *   @n@b ECTL_getStatus
 *
 *   @b Description
 *   @n Module configuration is achieved by calling ECTL_config().
 *
 *   <b> Return Value </b>  Uint32
 *   @li                '0'                 - Read status successful
 *   @li                ECTL_ERROR_INVALID  - Invalid status structure
 *
 *   <b> Pre Condition </b>
 *   @n  None.
 *
 *   <b> Post Condition </b>
 *   @n  ECTL module status is read
 *
 *   @b Modifies    
 *   @n  None
 *
 *   @b Example
 *   @verbatim
        ECTL_Config pEctlConfig;
        ECTL_Status pStatus;  
        
        pEctlConfig.intrPaceEn = 0x1;
        pEctlConfig.intrPrescale = 0x100;
        ...
        
        ECTL_config(&pEctlConfig);
        ECTL_getStatus(&pStatus);

     @endverbatim
 * =============================================================================
 */
Uint32 ECTL_getStatus (ECTL_Status *pStatus)
{  
    int i;

    /* Validate our handle */
    if (!pStatus) {
        return (ECTL_ERROR_INVALID);
    }

    /*Get masked status of Rx thresh, Rx, Tx inteerupts for core (0, 1 and 2)*/
    for (i = 0; i < NUM_OF_CORES; i++) {
        pStatus->rxThreshStat[i] = CSL_FEXT(ECTL_REGS->STATUS[i].C_RX_THRESH_STAT,\
                                       ECTL_C_RX_THRESH_STAT_C_RX_THRESH_STAT);
        pStatus->rxStat[i] = CSL_FEXT(ECTL_REGS->STATUS[i].C_RX_STAT, \
                                                     ECTL_C_RX_STAT_C_RX_STAT);
        pStatus->txStat[i] = CSL_FEXT(ECTL_REGS->STATUS[i].C_TX_STAT,\
                                                     ECTL_C_TX_STAT_C_TX_STAT);
    }

    /*Read Rx/Tx interrupts per millisecond */
    for (i = 0; i < NUM_OF_CORES; i++) {
        pStatus->rxImaxRead[i] = CSL_FEXT(ECTL_REGS->INTR_COUNT[i].C_RX_IMAX,\
                                        ECTL_C_RX_IMAX_C_RX_IMAX);
        pStatus->txImaxRead[i] = CSL_FEXT(ECTL_REGS->INTR_COUNT[i].C_TX_IMAX,\
                                        ECTL_C_TX_IMAX_C_TX_IMAX);
    }

    return 0;
}



