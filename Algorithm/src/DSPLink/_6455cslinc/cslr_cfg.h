/* =============================================================================
 *   Copyright (c) Texas Instruments Inc 2002, 2003, 2004, 2005
 *
 *   Use of this software is controlled by the terms and conditions found in the
 *   license agreement under which this software has been supplied.
 *  ============================================================================
 */

/** ============================================================================
 *   @file  cslr_cfg.h
 *
 *   @desc  This file contains the Register Desciptions for CFG
 * ============================================================================
 */
#ifndef _CSLR_CFG_H_
#define _CSLR_CFG_H_

#include <cslr.h>
#include <tistdtypes.h>

/**************************************************************************\
* Register Overlay Structure
\**************************************************************************/
typedef struct  {
    volatile Uint32 MPFAR;
    volatile Uint32 MPFSR;
    volatile Uint32 MPFCR;
} CSL_CfgRegs;

typedef volatile CSL_CfgRegs  *CSL_CfgRegsOvly;
/**************************************************************************\
* Field Definition Macros
\**************************************************************************/

/* MPFAR */

#define CSL_CFG_MPFAR_ADDR_MASK          (0xFFFFFFFFu)
#define CSL_CFG_MPFAR_ADDR_SHIFT         (0x00000000u)
#define CSL_CFG_MPFAR_ADDR_RESETVAL      (0x00000000u)

#define CSL_CFG_MPFAR_RESETVAL           (0x00000000u)

/* MPFSR */

#define CSL_CFG_MPFSR_FID_MASK           (0x0000FE00u)
#define CSL_CFG_MPFSR_FID_SHIFT          (0x00000009u)
#define CSL_CFG_MPFSR_FID_RESETVAL       (0x00000000u)

#define CSL_CFG_MPFSR_LOCAL_MASK         (0x00000100u)
#define CSL_CFG_MPFSR_LOCAL_SHIFT        (0x00000008u)
#define CSL_CFG_MPFSR_LOCAL_RESETVAL     (0x00000000u)

#define CSL_CFG_MPFSR_SR_MASK            (0x00000020u)
#define CSL_CFG_MPFSR_SR_SHIFT           (0x00000005u)
#define CSL_CFG_MPFSR_SR_RESETVAL        (0x00000000u)

#define CSL_CFG_MPFSR_SW_MASK            (0x00000010u)
#define CSL_CFG_MPFSR_SW_SHIFT           (0x00000004u)
#define CSL_CFG_MPFSR_SW_RESETVAL        (0x00000000u)

#define CSL_CFG_MPFSR_SX_MASK            (0x00000008u)
#define CSL_CFG_MPFSR_SX_SHIFT           (0x00000003u)
#define CSL_CFG_MPFSR_SX_RESETVAL        (0x00000000u)

#define CSL_CFG_MPFSR_UR_MASK            (0x00000004u)
#define CSL_CFG_MPFSR_UR_SHIFT           (0x00000002u)
#define CSL_CFG_MPFSR_UR_RESETVAL        (0x00000000u)

#define CSL_CFG_MPFSR_UW_MASK            (0x00000002u)
#define CSL_CFG_MPFSR_UW_SHIFT           (0x00000001u)
#define CSL_CFG_MPFSR_UW_RESETVAL        (0x00000000u)

#define CSL_CFG_MPFSR_UX_MASK            (0x00000001u)
#define CSL_CFG_MPFSR_UX_SHIFT           (0x00000000u)
#define CSL_CFG_MPFSR_UX_RESETVAL        (0x00000000u)

#define CSL_CFG_MPFSR_RESETVAL           (0x00000000u)

/* MPFCR */

#define CSL_CFG_MPFCR_MPFCLR_MASK        (0x00000001u)
#define CSL_CFG_MPFCR_MPFCLR_SHIFT       (0x00000000u)
#define CSL_CFG_MPFCR_MPFCLR_RESETVAL    (0x00000000u)

/*----MPFCLR Tokens----*/
#define CSL_CFG_MPFCR_MPFCLR_CLEAR       (0x00000001u)

#define CSL_CFG_MPFCR_RESETVAL           (0x00000000u)

#endif
