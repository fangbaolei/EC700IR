/* =============================================================================
 *  Copyright (c) Texas Instruments Inc 2002, 2003, 2004, 2005, 2006
 *
 *  Use of this software is controlled by the terms and conditions found in the
 *  license agreement under which this software has been supplied.
 *  ============================================================================
 */

/** ============================================================================
 *  @file  cslr_ectl.h
 *
 *   @path  $(CSLPATH)\inc
 *
 *  @desc  This file contains the Register Descriptions for ECTL
 *  ===========================================================================
 */
#ifndef _CSLR_ECTL_H_
#define _CSLR_ECTL_H_

#include <cslr.h>

#include <tistdtypes.h>

/**************************************************************************\
* Register Overlay Structure for CONTROL 
\**************************************************************************/
typedef struct  {
    volatile Uint32 C_RX_THRESH_EN;
    volatile Uint32 C_RX_EN;
    volatile Uint32 C_TX_EN;
    volatile Uint32 C_MISC_EN;
} CSL_EctlControlRegs;

/**************************************************************************\
* Register Overlay Structure for STATUS 
\**************************************************************************/
typedef struct  {
    volatile Uint32 C_RX_THRESH_STAT;
    volatile Uint32 C_RX_STAT;
    volatile Uint32 C_TX_STAT;
    volatile Uint32 C_MISC_STAT;
} CSL_EctlStatusRegs;

/**************************************************************************\
* Register Overlay Structure for INTR_COUNT 
\**************************************************************************/
typedef struct  {
    volatile Uint32 C_RX_IMAX;
    volatile Uint32 C_TX_IMAX;
} CSL_EctlIntr_countRegs;

/**************************************************************************\
* Register Overlay Structure
\**************************************************************************/
typedef struct  {
    volatile Uint32 IDVER;
    volatile Uint32 SOFT_RESET;
    volatile Uint32 EM_CONTROL;
    volatile Uint32 INT_CONTROL;
    CSL_EctlControlRegs CONTROL[3];
    CSL_EctlStatusRegs STATUS[3];
    CSL_EctlIntr_countRegs INTR_COUNT[3];
} CSL_EctlRegs;

/**************************************************************************\
* Field Definition Macros
\**************************************************************************/

/* IDVER */

#define CSL_ECTL_IDVER_CPGMACSS_S_IDENT_MASK (0xFFFF0000u)
#define CSL_ECTL_IDVER_CPGMACSS_S_IDENT_SHIFT (0x00000010u)
#define CSL_ECTL_IDVER_CPGMACSS_S_IDENT_RESETVAL (0x0000002Du)

#define CSL_ECTL_IDVER_CPGMACSS_S_RTL_VER_MASK (0x0000F800u)
#define CSL_ECTL_IDVER_CPGMACSS_S_RTL_VER_SHIFT (0x0000000Bu)
#define CSL_ECTL_IDVER_CPGMACSS_S_RTL_VER_RESETVAL (0x00000000u)

#define CSL_ECTL_IDVER_CPGMACSS_S_MAJ_VER_MASK (0x00000700u)
#define CSL_ECTL_IDVER_CPGMACSS_S_MAJ_VER_SHIFT (0x00000008u)
#define CSL_ECTL_IDVER_CPGMACSS_S_MAJ_VER_RESETVAL (0x00000001u)

#define CSL_ECTL_IDVER_CPGMACSS_S_MINOR_VER_MASK (0x000000FFu)
#define CSL_ECTL_IDVER_CPGMACSS_S_MINOR_VER_SHIFT (0x00000000u)
#define CSL_ECTL_IDVER_CPGMACSS_S_MINOR_VER_RESETVAL (0x00000002u)

#define CSL_ECTL_IDVER_RESETVAL          (0x002D0102u)

/* SOFT_RESET */

#define CSL_ECTL_SOFT_RESET_SOFT_RESET_MASK (0x00000001u)
#define CSL_ECTL_SOFT_RESET_SOFT_RESET_SHIFT (0x00000000u)
#define CSL_ECTL_SOFT_RESET_SOFT_RESET_RESETVAL (0x00000000u)

/*----SOFT_RESET Tokens----*/
#define CSL_ECTL_SOFT_RESET_SOFT_RESET_YES (0x00000001u)
#define CSL_ECTL_SOFT_RESET_SOFT_RESET_NO (0x00000000u)

#define CSL_ECTL_SOFT_RESET_RESETVAL     (0x00000000u)

/* EM_CONTROL */

#define CSL_ECTL_EM_CONTROL_SOFT_MASK    (0x00000002u)
#define CSL_ECTL_EM_CONTROL_SOFT_SHIFT   (0x00000001u)
#define CSL_ECTL_EM_CONTROL_SOFT_RESETVAL (0x00000000u)

#define CSL_ECTL_EM_CONTROL_FREE_MASK    (0x00000001u)
#define CSL_ECTL_EM_CONTROL_FREE_SHIFT   (0x00000000u)
#define CSL_ECTL_EM_CONTROL_FREE_RESETVAL (0x00000000u)

#define CSL_ECTL_EM_CONTROL_RESETVAL     (0x00000000u)

/* INT_CONTROL */

#define CSL_ECTL_INT_CONTROL_INT_TEST_MASK (0x80000000u)
#define CSL_ECTL_INT_CONTROL_INT_TEST_SHIFT (0x0000001Fu)
#define CSL_ECTL_INT_CONTROL_INT_TEST_RESETVAL (0x00000000u)

#define CSL_ECTL_INT_CONTROL_INT_PACE_EN_MASK (0x003F0000u)
#define CSL_ECTL_INT_CONTROL_INT_PACE_EN_SHIFT (0x00000010u)
#define CSL_ECTL_INT_CONTROL_INT_PACE_EN_RESETVAL (0x00000000u)

/*----INT_PACE_EN Tokens----*/
#define CSL_ECTL_INT_CONTROL_INT_PACE_EN_C0_RX (0x00000001u)
#define CSL_ECTL_INT_CONTROL_INT_PACE_EN_C0_TX (0x00000002u)
#define CSL_ECTL_INT_CONTROL_INT_PACE_EN_C1_RX (0x00000004u)
#define CSL_ECTL_INT_CONTROL_INT_PACE_EN_C1_TX (0x00000008u)
#define CSL_ECTL_INT_CONTROL_INT_PACE_EN_C2_RX (0x00000010u)
#define CSL_ECTL_INT_CONTROL_INT_PACE_EN_C2_TX (0x00000020u)

#define CSL_ECTL_INT_CONTROL_INT_PRESCALE_MASK (0x00000FFFu)
#define CSL_ECTL_INT_CONTROL_INT_PRESCALE_SHIFT (0x00000000u)
#define CSL_ECTL_INT_CONTROL_INT_PRESCALE_RESETVAL (0x00000000u)

#define CSL_ECTL_INT_CONTROL_RESETVAL    (0x00000000u)

/* C_RX_THRESH_EN */

#define CSL_ECTL_C_RX_THRESH_EN_C0_RX_THRESH_EN_MASK (0x000000FFu)
#define CSL_ECTL_C_RX_THRESH_EN_C0_RX_THRESH_EN_SHIFT (0x00000000u)
#define CSL_ECTL_C_RX_THRESH_EN_C0_RX_THRESH_EN_RESETVAL (0x00000000u)

#define CSL_ECTL_C_RX_THRESH_EN_RESETVAL (0x00000000u)

/* C_RX_EN */

#define CSL_ECTL_C_RX_EN_C_RX_EN_MASK    (0x000000FFu)
#define CSL_ECTL_C_RX_EN_C_RX_EN_SHIFT   (0x00000000u)
#define CSL_ECTL_C_RX_EN_C_RX_EN_RESETVAL (0x00000000u)

#define CSL_ECTL_C_RX_EN_RESETVAL        (0x00000000u)

/* C_TX_EN */

#define CSL_ECTL_C_TX_EN_C_TX_EN_MASK    (0x000000FFu)
#define CSL_ECTL_C_TX_EN_C_TX_EN_SHIFT   (0x00000000u)
#define CSL_ECTL_C_TX_EN_C_TX_EN_RESETVAL (0x00000000u)

#define CSL_ECTL_C_TX_EN_RESETVAL        (0x00000000u)

/* C_MISC_EN */

#define CSL_ECTL_C_MISC_EN_C_MISC_EN_MASK (0x0000000Fu)
#define CSL_ECTL_C_MISC_EN_C_MISC_EN_SHIFT (0x00000000u)
#define CSL_ECTL_C_MISC_EN_C_MISC_EN_RESETVAL (0x00000000u)

#define CSL_ECTL_C_MISC_EN_RESETVAL      (0x00000000u)

/* C_RX_THRESH_STAT */

#define CSL_ECTL_C_RX_THRESH_STAT_C_RX_THRESH_STAT_MASK (0x000000FFu)
#define CSL_ECTL_C_RX_THRESH_STAT_C_RX_THRESH_STAT_SHIFT (0x00000000u)
#define CSL_ECTL_C_RX_THRESH_STAT_C_RX_THRESH_STAT_RESETVAL (0x00000000u)

#define CSL_ECTL_C_RX_THRESH_STAT_RESETVAL (0x00000000u)

/* C_RX_STAT */

#define CSL_ECTL_C_RX_STAT_C_RX_STAT_MASK (0x000000FFu)
#define CSL_ECTL_C_RX_STAT_C_RX_STAT_SHIFT (0x00000000u)
#define CSL_ECTL_C_RX_STAT_C_RX_STAT_RESETVAL (0x00000000u)

#define CSL_ECTL_C_RX_STAT_RESETVAL      (0x00000000u)

/* C_TX_STAT */

#define CSL_ECTL_C_TX_STAT_C_TX_STAT_MASK (0x000000FFu)
#define CSL_ECTL_C_TX_STAT_C_TX_STAT_SHIFT (0x00000000u)
#define CSL_ECTL_C_TX_STAT_C_TX_STAT_RESETVAL (0x00000000u)

#define CSL_ECTL_C_TX_STAT_RESETVAL      (0x00000000u)

/* C_MISC_STAT */

#define CSL_ECTL_C_MISC_STAT_C_MISC_STAT_MASK (0x0000000Fu)
#define CSL_ECTL_C_MISC_STAT_C_MISC_STAT_SHIFT (0x00000000u)
#define CSL_ECTL_C_MISC_STAT_C_MISC_STAT_RESETVAL (0x00000000u)

#define CSL_ECTL_C_MISC_STAT_RESETVAL    (0x00000000u)

/* C_RX_IMAX */

#define CSL_ECTL_C_RX_IMAX_C_RX_IMAX_MASK (0x0000003Fu)
#define CSL_ECTL_C_RX_IMAX_C_RX_IMAX_SHIFT (0x00000000u)
#define CSL_ECTL_C_RX_IMAX_C_RX_IMAX_RESETVAL (0x00000000u)

#define CSL_ECTL_C_RX_IMAX_RESETVAL      (0x00000000u)

/* C_TX_IMAX */

#define CSL_ECTL_C_TX_IMAX_C_TX_IMAX_MASK (0x0000003Fu)
#define CSL_ECTL_C_TX_IMAX_C_TX_IMAX_SHIFT (0x00000000u)
#define CSL_ECTL_C_TX_IMAX_C_TX_IMAX_RESETVAL (0x00000000u)

#define CSL_ECTL_C_TX_IMAX_RESETVAL      (0x00000000u)

#endif
