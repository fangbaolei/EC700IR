/*********************************************************************
 * Copyright (C) 2003-2005 Texas Instruments Incorporated. 
 * All Rights Reserved 
 *********************************************************************/
 /** \file cslr_ectl.h
 * 
 * \brief This file contains the Register Desciptions for ECTL
 * 
 *********************************************************************/

#ifndef _CSLR_ECTL_H_
#define _CSLR_ECTL_H_
#include <cslr.h>

#include <tistdtypes.h>

/**************************************************************************\
* Register Overlay Structure
\**************************************************************************/
typedef struct  {
    volatile Uint8 RSVD0[4];
    volatile Uint32 EWCTL;
    volatile Uint32 EWINTTCNT;
} CSL_EctlRegs;

/**************************************************************************\
* Field Definition Macros
\**************************************************************************/

/* EWCTL */

#define CSL_ECTL_EWCTL_INTEN_MASK        (0x00000001u)
#define CSL_ECTL_EWCTL_INTEN_SHIFT       (0x00000000u)
#define CSL_ECTL_EWCTL_INTEN_RESETVAL    (0x00000000u)

/*----INTEN Tokens----*/
#define CSL_ECTL_EWCTL_INTEN_DISABLE     (0x00000000u)
#define CSL_ECTL_EWCTL_INTEN_ENABLE      (0x00000001u)

#define CSL_ECTL_EWCTL_RESETVAL          (0x00000000u)

/* EWINTTCNT */

#define CSL_ECTL_EWINTTCNT_EWINTTCNT_MASK (0x0001FFFFu)
#define CSL_ECTL_EWINTTCNT_EWINTTCNT_SHIFT (0x00000000u)
#define CSL_ECTL_EWINTTCNT_EWINTTCNT_RESETVAL (0x00000000u)

#define CSL_ECTL_EWINTTCNT_RESETVAL      (0x00000000u)

#endif
