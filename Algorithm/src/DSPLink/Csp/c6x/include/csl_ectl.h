/*  ============================================================================
 *   Copyright (c) Texas Instruments Inc 2002, 2003, 2004, 2005, 2006
 *
 *   Use of this software is controlled by the terms and conditions found in the
 *   license agreement under which this software has been supplied.
 *   ===========================================================================
 */
 
/** ============================================================================
 *   @file  csl_ectl.h
 *
 *   @path  $(CSLPATH)\inc
 *
 *   @desc  Header file for functional layer CSL of ECTL
 *
 *   @date    17/01/2006 
*/

/** @mainpage EMAC interrupt control
 *
 *  @section Introduction 
 *  
 *  @subsection xxx Purpose and Scope
 *  The purpose of this document is to detail the  CSL APIs for the
 *  ECTL Module.
 *  
 *  @subsection aaa Terms and Abbreviations
 *    -# CSL:   Chip Support Library
 *    -# API:   Application Programmer Interface
 *    -# ECTL:  EMAC interrupt control
 *  @subsection References
 *     -# CSL 3.x Technical Requirements Specifications Version 0.5, dated
 *        May 14th, 2003
 *     -# ECTL Specification 
 *  
 *  @subsection Assumptions
 *     The abbreviations ECTL, Ectl & ectl have been used throughout this
 *     document to refer to EMAC interrupt control
 */


#ifndef _CSL_ECTL_H_
#define _CSL_ECTL_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Include the register layer ECTL header file */
#include <cslr_ectl.h>
#include <davinci_hd.h>

/** Function or calling parameter is invalid  */
#define ECTL_ERROR_INVALID   4   
/** ECTL module base address */
#define ECTL_REGS         ((CSL_EctlRegs *)CSL_ECTL_0_REGS)

/** number of DSP cores on chip */
#define NUM_OF_CORES 3

/**core(0, 1 and 2) miscellaneous interrupts enable/status macros*/
#define ECTL_MISC_STAT_PEND     0x00000008
#define ECTL_MISC_HOST_PEND     0x00000004
#define ECTL_MISC_MDIO_LINKINT  0x00000002
#define ECTL_MISC_MDIO_USERINT  0x00000001

/**core(0, 1 and 2) Rx thresh interrupts enable/status macros*/
#define ECTL_RX_THRESH_CHA7   0x00000080
#define ECTL_RX_THRESH_CHA6   0x00000040
#define ECTL_RX_THRESH_CHA5   0x00000020
#define ECTL_RX_THRESH_CHA4   0x00000010
#define ECTL_RX_THRESH_CHA3   0x00000008 
#define ECTL_RX_THRESH_CHA2   0x00000004 
#define ECTL_RX_THRESH_CHA1   0x00000002 
#define ECTL_RX_THRESH_CHA0   0x00000001 

/**core(0, 1 and 2) Rx interrupts enable/status macros*/
#define ECTL_RX_CHA7    0x00000080
#define ECTL_RX_CHA6    0x00000040
#define ECTL_RX_CHA5    0x00000020
#define ECTL_RX_CHA4    0x00000010
#define ECTL_RX_CHA3    0x00000008
#define ECTL_RX_CHA2    0x00000004
#define ECTL_RX_CHA1    0x00000002
#define ECTL_RX_CHA0    0x00000001

/**core(0, 1 and 2) Tx interrupts enable/status macros*/
#define ECTL_TX_CHA7    0x00000080
#define ECTL_TX_CHA6    0x00000040
#define ECTL_TX_CHA5    0x00000020
#define ECTL_TX_CHA4    0x00000010
#define ECTL_TX_CHA3    0x00000008
#define ECTL_TX_CHA2    0x00000004
#define ECTL_TX_CHA1    0x00000002
#define ECTL_TX_CHA0    0x00000001

/**core(0, 1 and 2) interrupts pace enable macros*/
#define ECTL_INTR_PACE_CORE2_TX 0x00000020
#define ECTL_INTR_PACE_CORE2_RX 0x00000010
#define ECTL_INTR_PACE_CORE1_TX 0x00000008
#define ECTL_INTR_PACE_CORE1_RX 0x00000004
#define ECTL_INTR_PACE_CORE0_TX 0x00000002
#define ECTL_INTR_PACE_CORE0_RX 0x00000001


/*-----------------------------------------------------------------------*\
* STANDARD DATA STRUCTURES
\*-----------------------------------------------------------------------*/

/** This structure is used for configuring the ECTL module */
typedef struct {
   /** Enables interrupt pacing for core(0, 1 and 2)*/
   Uint8   intrPaceEn; 
   /** sets the the no of VBUSP_CLK periods in 4us */
   Uint16  intrPrescale;  
   /** array to enable Rx threshold interrupts for  core(0, 1 and 2)*/
   Uint8   rxThreshEn[NUM_OF_CORES];  
   /** array to enable Rx interrupts for  core(0, 1 and 2) */
   Uint8   rxEn[NUM_OF_CORES];        
   /** array to enable Tx interrupts for  core(0, 1 and 2) */
   Uint8   txEn[NUM_OF_CORES];        
   /** array to enable misc interrupts for core(0, 1 and 2) */
   Uint8   miscEn[NUM_OF_CORES];      
   /** Loads imax for Rx interrupts */
   Uint8   rxImaxLoad[NUM_OF_CORES];   
   /** Loads imax for Tx interrupts */
   Uint8   txImaxLoad[NUM_OF_CORES];   
} ECTL_Config;

/** This structure is used to get the status values of ECTL */
typedef struct {
    /** Rx thresh masked intrrupt status */
    Uint8 rxThreshStat[NUM_OF_CORES];   
    /** Rx masked intrrupt status */
    Uint8 rxStat[NUM_OF_CORES];         
    /** Tx masked intrrupt status */
    Uint8 txStat[NUM_OF_CORES];         
    /** Misc(STAT_PEND, HOST_PEND, MDIO_LINKINT and MDIO_USERINT) masked intrrupt status */
    Uint8 miscStat[NUM_OF_CORES];       
    /** read imax count for Rx interrupts*/
    Uint8 rxImaxRead[NUM_OF_CORES];     
    /** read imax count for Tx interrupts*/
    Uint8 txImaxRead[NUM_OF_CORES];     
} ECTL_Status;


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
Uint32 ECTL_config( ECTL_Config *ectlConfig );

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
Uint32 ECTL_getStatus( ECTL_Status *pStatus );


#ifdef __cplusplus
}
#endif

#endif  /* _CSL_ECTL_H_ */

