/*********************************************************************
 * Copyright (C) 2003-2005 Texas Instruments Incorporated. 
 * All Rights Reserved 
 *********************************************************************/
 /** \file cslr_hpi.h
 * 
 * \brief This file contains the Register Desciptions for HPI
 * 
 *********************************************************************/

#ifndef _CSLR_HPI_H_
#define _CSLR_HPI_H_

#include <cslr.h>

#include <tistdtypes.h>

/**************************************************************************\
* Register Overlay Structure
\**************************************************************************/
typedef struct  {
    volatile Uint8 RSVD0[4];
    volatile Uint32 PWREMU_MGMT;
    volatile Uint8 RSVD1[40];
    volatile Uint32 HPIC;
    volatile Uint32 HPIAW;
    volatile Uint32 HPIAR;
} CSL_HpiRegs;

/**************************************************************************\
* Overlay structure typedef definition
\**************************************************************************/
typedef volatile CSL_HpiRegs         *CSL_HpiRegsOvly;


/**************************************************************************\
* Field Definition Macros
\**************************************************************************/

/* PWREMU_MGMT */

#define CSL_HPI_PWREMU_MGMT_SOFT_MASK    (0x00000002u)
#define CSL_HPI_PWREMU_MGMT_SOFT_SHIFT   (0x00000001u)
#define CSL_HPI_PWREMU_MGMT_SOFT_RESETVAL (0x00000000u)

/*----SOFT Tokens----*/
#define CSL_HPI_PWREMU_MGMT_SOFT_OFF     (0x00000000u)
#define CSL_HPI_PWREMU_MGMT_SOFT_ON      (0x00000001u)

#define CSL_HPI_PWREMU_MGMT_FREE_MASK    (0x00000001u)
#define CSL_HPI_PWREMU_MGMT_FREE_SHIFT   (0x00000000u)
#define CSL_HPI_PWREMU_MGMT_FREE_RESETVAL (0x00000000u)

/*----FREE Tokens----*/
#define CSL_HPI_PWREMU_MGMT_FREE_OFF     (0x00000000u)
#define CSL_HPI_PWREMU_MGMT_FREE_ON      (0x00000001u)

#define CSL_HPI_PWREMU_MGMT_RESETVAL     (0x00000000u)

/* HPIC */

#define CSL_HPI_HPIC_HPIARWSEL_MASK      (0x00000800u)
#define CSL_HPI_HPIC_HPIARWSEL_SHIFT     (0x0000000Bu)
#define CSL_HPI_HPIC_HPIARWSEL_RESETVAL  (0x00000000u)

/*----HPIARWSEL Tokens----*/
#define CSL_HPI_HPIC_HPIARWSEL_HPIA_R    (0x00000000u)
#define CSL_HPI_HPIC_HPIARWSEL_HPIA_W    (0x00000001u)

#define CSL_HPI_HPIC_DUALHPIA_MASK       (0x00000200u)
#define CSL_HPI_HPIC_DUALHPIA_SHIFT      (0x00000009u)
#define CSL_HPI_HPIC_DUALHPIA_RESETVAL   (0x00000000u)

/*----DUALHPIA Tokens----*/
#define CSL_HPI_HPIC_DUALHPIA_DISABLE    (0x00000000u)
#define CSL_HPI_HPIC_DUALHPIA_ENABLE     (0x00000001u)

#define CSL_HPI_HPIC_HWOBSTAT_MASK       (0x00000100u)
#define CSL_HPI_HPIC_HWOBSTAT_SHIFT      (0x00000008u)
#define CSL_HPI_HPIC_HWOBSTAT_RESETVAL   (0x00000000u)

#define CSL_HPI_HPIC_HPIRST_MASK         (0x00000080u)
#define CSL_HPI_HPIC_HPIRST_SHIFT        (0x00000007u)
#define CSL_HPI_HPIC_HPIRST_RESETVAL     (0x00000001u)

#define CSL_HPI_HPIC_FETCH_MASK          (0x00000010u)
#define CSL_HPI_HPIC_FETCH_SHIFT         (0x00000004u)
#define CSL_HPI_HPIC_FETCH_RESETVAL      (0x00000000u)

#define CSL_HPI_HPIC_HRDY_MASK           (0x00000008u)
#define CSL_HPI_HPIC_HRDY_SHIFT          (0x00000003u)
#define CSL_HPI_HPIC_HRDY_RESETVAL       (0x00000000u)

#define CSL_HPI_HPIC_HINT_MASK           (0x00000004u)
#define CSL_HPI_HPIC_HINT_SHIFT          (0x00000002u)
#define CSL_HPI_HPIC_HINT_RESETVAL       (0x00000000u)

/*----HINT Tokens----*/
#define CSL_HPI_HPIC_HINT_DISABLE        (0x00000001u)
#define CSL_HPI_HPIC_HINT_ENABLE         (0x00000001u)

#define CSL_HPI_HPIC_DSPINT_MASK         (0x00000002u)
#define CSL_HPI_HPIC_DSPINT_SHIFT        (0x00000001u)
#define CSL_HPI_HPIC_DSPINT_RESETVAL     (0x00000000u)

/*----DSPINT Tokens----*/
#define CSL_HPI_HPIC_DSPINT_DISABLE      (0x00000000u)
#define CSL_HPI_HPIC_DSPINT_ENABLE       (0x00000001u)

#define CSL_HPI_HPIC_HWOB_MASK           (0x00000001u)
#define CSL_HPI_HPIC_HWOB_SHIFT          (0x00000000u)
#define CSL_HPI_HPIC_HWOB_RESETVAL       (0x00000000u)

/*----HWOB Tokens----*/
#define CSL_HPI_HPIC_HWOB_DISABLE        (0x00000000u)
#define CSL_HPI_HPIC_HWOB_ENABLE         (0x00000001u)

#define CSL_HPI_HPIC_RESETVAL            (0x00000080u)

/* HPIAW */

#define CSL_HPI_HPIAW_HPIAW_MASK         (0xFFFFFFFFu)
#define CSL_HPI_HPIAW_HPIAW_SHIFT        (0x00000000u)
#define CSL_HPI_HPIAW_HPIAW_RESETVAL     (0x00000000u)

#define CSL_HPI_HPIAW_RESETVAL           (0x00000000u)

/* HPIAR */

#define CSL_HPI_HPIAR_HPIAR_MASK         (0xFFFFFFFFu)
#define CSL_HPI_HPIAR_HPIAR_SHIFT        (0x00000000u)
#define CSL_HPI_HPIAR_HPIAR_RESETVAL     (0x00000000u)

#define CSL_HPI_HPIAR_RESETVAL           (0x00000000u)

#endif
