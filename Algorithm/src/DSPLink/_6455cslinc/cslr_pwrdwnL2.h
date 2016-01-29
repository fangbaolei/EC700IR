/*********************************************************************
 * Copyright (C) 2003-2005 Texas Instruments Incorporated. 
 * All Rights Reserved 
 *********************************************************************/
 /** \file cslr_pwrdwnL2.h
 * 
 * \brief This file contains the Register Desciptions for L2PWRDWN
 * 
 *********************************************************************/

#ifndef _CSLR_PWRDWNL2_H_
#define _CSLR_PWRDWNL2_H_

#include <cslr.h>
#include <tistdtypes.h>

/**************************************************************************\
* Register Overlay Structure
\**************************************************************************/
typedef struct  {
    volatile Uint8 RSVD0[64];
    volatile Uint32 L2PDWAKE[2];
    volatile Uint8 RSVD1[8];
    volatile Uint32 L2PDSLEEP[2];
    volatile Uint8 RSVD2[8];
    volatile Uint32 L2PDSTAT[2];
} CSL_L2pwrdwnRegs;

typedef volatile CSL_L2pwrdwnRegs *CSL_L2pwrdwnRegsOvly;
/**************************************************************************\
* Field Definition Macros
\**************************************************************************/

/* L2PDWAKE */

#define CSL_L2PWRDWN_L2PDWAKE_P1_MASK    (0x00000002u)
#define CSL_L2PWRDWN_L2PDWAKE_P1_SHIFT   (0x00000001u)
#define CSL_L2PWRDWN_L2PDWAKE_P1_RESETVAL (0x00000000u)

/*----P1 Tokens----*/
#define CSL_L2PWRDWN_L2PDWAKE_P1_DISABLE (0x00000000u)
#define CSL_L2PWRDWN_L2PDWAKE_P1_ENABLE  (0x00000001u)

#define CSL_L2PWRDWN_L2PDWAKE_P0_MASK    (0x00000001u)
#define CSL_L2PWRDWN_L2PDWAKE_P0_SHIFT   (0x00000000u)
#define CSL_L2PWRDWN_L2PDWAKE_P0_RESETVAL (0x00000000u)

/*----P0 Tokens----*/
#define CSL_L2PWRDWN_L2PDWAKE_P0_DISABLE (0x00000000u)
#define CSL_L2PWRDWN_L2PDWAKE_P0_ENABLE  (0x00000001u)

#define CSL_L2PWRDWN_L2PDWAKE_RESETVAL   (0x00000000u)

/* L2PDSLEEP */

#define CSL_L2PWRDWN_L2PDSLEEP_P1_MASK   (0x00000002u)
#define CSL_L2PWRDWN_L2PDSLEEP_P1_SHIFT  (0x00000001u)
#define CSL_L2PWRDWN_L2PDSLEEP_P1_RESETVAL (0x00000000u)

/*----P1 Tokens----*/
#define CSL_L2PWRDWN_L2PDSLEEP_P1_DISABLE (0x00000000u)
#define CSL_L2PWRDWN_L2PDSLEEP_P1_ENABLE (0x00000001u)

#define CSL_L2PWRDWN_L2PDSLEEP_P0_MASK   (0x00000001u)
#define CSL_L2PWRDWN_L2PDSLEEP_P0_SHIFT  (0x00000000u)
#define CSL_L2PWRDWN_L2PDSLEEP_P0_RESETVAL (0x00000000u)

/*----P0 Tokens----*/
#define CSL_L2PWRDWN_L2PDSLEEP_P0_DISABLE (0x00000000u)
#define CSL_L2PWRDWN_L2PDSLEEP_P0_ENABLE (0x00000001u)

#define CSL_L2PWRDWN_L2PDSLEEP_RESETVAL  (0x00000000u)

/* L2PDSTAT */

#define CSL_L2PWRDWN_L2PDSTAT_P1_MASK    (0x00000002u)
#define CSL_L2PWRDWN_L2PDSTAT_P1_SHIFT   (0x00000001u)
#define CSL_L2PWRDWN_L2PDSTAT_P1_RESETVAL (0x00000000u)

#define CSL_L2PWRDWN_L2PDSTAT_P0_MASK    (0x00000001u)
#define CSL_L2PWRDWN_L2PDSTAT_P0_SHIFT   (0x00000000u)
#define CSL_L2PWRDWN_L2PDSTAT_P0_RESETVAL (0x00000000u)

#define CSL_L2PWRDWN_L2PDSTAT_RESETVAL   (0x00000000u)

#endif
