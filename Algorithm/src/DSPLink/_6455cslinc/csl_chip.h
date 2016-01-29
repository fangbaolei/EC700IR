/*  ============================================================================
 *   Copyright (c) Texas Instruments Inc 2002, 2003, 2004, 2005
 *
 *   Use of this software is controlled by the terms and conditions found in the
 *   license agreement under which this software has been supplied.
 *   ===========================================================================
 */

/** ============================================================================
 *   @file  csl_chip.h
 *
 *   @path  $(CSLPATH)\inc
 *
 *   @desc  API header file for CHIP module CSL
 *
 * =============================================================================
 */
 
/* =============================================================================
 *  Revision History
 *  ===============
 *  07-Mar-2006 ds  To fix bug PSG00001004, Added a dummy code chipReadNone2, 
 *                  before ILC read in CSL_chipReadReg () API
 *                  To fix bug PSG00001012, Added dummy code chipWriteNone2, 
 *                  before ILC write in CSL_chipWriteReg () API
 * =============================================================================
 */

#ifndef _CSL_CHIP_H_
#define _CSL_CHIP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <csl.h>
#include <cslr_chip.h>


/* CHIP global macro declarations */

/* CHIP global typedef declarations */

/** Enum for the CHIP registers */
typedef enum  {
    /** Addressing Mode Register */
    AMR = 0,
    /** Control Status Register */
    CSR = 1,
    /** Interrupt Flag Register */
    IFR = 2,
    /** Interrupt Set Register */
    ISR = 2,
    /** Interrupt Clear Register */
    ICR = 3,
    /** Interrupt Enable Register */
    IER = 4,
    /** Interrupt Service Table Pointer Register */
    ISTP= 5,
    /** Interrupt Return Pointer Register */
    IRP= 6,
    /** Nonmaskable Interrupt (NMI) Return Pointer Register */
    NRP= 7,
    /** Exception Return Pointer Register */
    ERP= 7,
    /** Time Stamp Counter Register - Low */
    TSCL= 10,
    /** Time Stamp Counter Registers - High */
    TSCH= 11,
    /** SPLOOP Inner Loop Count Register */
    ILC= 13,
    /** SPLOOP Reload Inner Loop Count Register */
    RILC= 14,
    /** E1 Phase Program Counter */
    PCE1= 16,
    /** DSP Core Number Register */
    DNUM= 17,
    /** Saturation Status Register */
    SSR= 21,
    /** GMPY Polynomial.A Side Register */
    GPLYA= 22,
    /** GMPY Polynomial.B Side Register */
    GPLYB= 23,
    /** Galois Field Polynomial Generator Function Register */
    GFPGFR= 24,
    /** Task State Register */
    TSR= 26,
    /** Interrupt Task State Register */
    ITSR= 27,
    /** NMI/Exception Task State Register */
    NTSR= 28,
    /** Exception Flag Register */
    EFR= 29,
    /** Exception Clear Register */
    ECR= 29,
    /** Internal Exception Report Register */
    IERR= 31,
    /** Restricted Entry Point Address Register */
    REP= 32
}CSL_ChipReg;

/* CHIP global function declarations   */

/** ============================================================================
 *   @n@b CSL_chipWriteReg
 *
 *   @b Description
 *   @n This API writes specified control register with the specified 
 *      value 'val'. 
 *
 *   @b Arguments
 *   @verbatim      
            reg      Specifies the control register to be written to
 
            val      Value to be written.
 
     @endverbatim
 *
 *
 *   <b> Return Value   Uint32
 *   @li                    Old programmed value
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  The reg control register is written with the value passed
 *
 *   @b Modifies
 *   @n None
 *
 * <b> Usage Constraints: </b>
 *  @n Please refer to the C64x+ user guide for constraints while accessing 
 *     registers in different privilege levels.
 *
 *   @b Example
 *   @verbatim
 
        Uint32 oldamr;
        oldamr = CSL_chipWriteReg (AMR, 56);


    @endverbatim
 * ===========================================================================
 */
Uint32  CSL_chipWriteReg (
    CSL_ChipReg               reg,
    CSL_Reg32                 val
);

/** ============================================================================
 *   @n@b CSL_chipReadReg
 *
 *   @b Description
 *   @n This API reads the specified control register. 
 *
 *   @b Arguments
 *   @verbatim      
            reg      Specifies the control register to be read
 
     @endverbatim
 *
 *
 *   <b> Return Value   Uint32
 *   @li                    The control register value read
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
 *  <b> Usage Constraints: </b>
 *  @n Please refer to the C64x+ user guide for constraints while accessing 
 *     registers in different privilege levels.
 *
 *   @b Example
 *   @verbatim
 
        Uint32 amr;
        amr = CSL_chipReadReg (AMR);


    @endverbatim
 * ===========================================================================
 */
Uint32  CSL_chipReadReg(
    CSL_ChipReg               reg
);

#ifdef __cplusplus
}
#endif

#endif
