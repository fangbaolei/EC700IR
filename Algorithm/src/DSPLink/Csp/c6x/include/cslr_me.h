#ifndef _CSLR_ME_1_H_
#define _CSLR_ME_1_H_

#include <cslr.h>

#include "tistdtypes.h"


/* Minimum unit = 4 bytes */

/**************************************************************************\
* Register Overlay Structure
\**************************************************************************/
typedef struct  {
    volatile Uint32 PID;
    volatile Uint32 CNT;
    volatile Uint32 CTRL;
    volatile Uint32 MON_ADDR;
    volatile Uint32 MON_DATA;
} CSL_MeRegs;

/**************************************************************************\
* Field Definition Macros
\**************************************************************************/

/* PID */

#define CSL_ME_PID_PID_MASK              (0xFFFFFFFFu)
#define CSL_ME_PID_PID_SHIFT             (0x00000000u)
#define CSL_ME_PID_PID_RESETVAL          (0x4C060100u)

#define CSL_ME_PID_RESETVAL              (0x4C060100u)

/* CNT */

#define CSL_ME_CNT_EN_MASK               (0x80000000u)
#define CSL_ME_CNT_EN_SHIFT              (0x0000001Fu)
#define CSL_ME_CNT_EN_RESETVAL           (0x00000000u)
/*----EN Tokens----*/
#define CSL_ME_CNT_EN_DISABLE            (0x00000000u)
#define CSL_ME_CNT_EN_ENABLE             (0x00000001u)

#define CSL_ME_CNT_RST_MASK              (0x40000000u)
#define CSL_ME_CNT_RST_SHIFT             (0x0000001Eu)
#define CSL_ME_CNT_RST_RESETVAL          (0x00000000u)
/*----RST Tokens----*/
#define CSL_ME_CNT_RST_SET               (0x00000001u)


#define CSL_ME_CNT_VALUE_MASK            (0x0000FFFFu)
#define CSL_ME_CNT_VALUE_SHIFT           (0x00000000u)
#define CSL_ME_CNT_VALUE_RESETVAL        (0x00000000u)

#define CSL_ME_CNT_RESETVAL              (0x00000000u)

/* CTRL */


#define CSL_ME_CTRL_ADDR_MASK            (0x003FFF00u)
#define CSL_ME_CTRL_ADDR_SHIFT           (0x00000008u)
#define CSL_ME_CTRL_ADDR_RESETVAL        (0x00000000u)


#define CSL_ME_CTRL_SSM_MASK             (0x00000004u)
#define CSL_ME_CTRL_SSM_SHIFT            (0x00000002u)
#define CSL_ME_CTRL_SSM_RESETVAL         (0x00000000u)
/*----SSM Tokens----*/
#define CSL_ME_CTRL_SSM_NORMAL           (0x00000000u)
#define CSL_ME_CTRL_SSM_SSP              (0x00000001u)

#define CSL_ME_CTRL_CSB_MASK             (0x00000002u)
#define CSL_ME_CTRL_CSB_SHIFT            (0x00000001u)
#define CSL_ME_CTRL_CSB_RESETVAL         (0x00000000u)
/*----CSB Tokens----*/
#define CSL_ME_CTRL_CSB_DEFINE           (0x00000000u)

#define CSL_ME_CTRL_EN_MASK              (0x00000001u)
#define CSL_ME_CTRL_EN_SHIFT             (0x00000000u)
#define CSL_ME_CTRL_EN_RESETVAL          (0x00000000u)
/*----EN Tokens----*/
#define CSL_ME_CTRL_EN_SET               (0x00000001u)

#define CSL_ME_CTRL_RESETVAL             (0x00000000u)

/* MON_ADDR */


#define CSL_ME_MON_ADDR_SET_MASK         (0x0000007Fu)
#define CSL_ME_MON_ADDR_SET_SHIFT        (0x00000000u)
#define CSL_ME_MON_ADDR_SET_RESETVAL     (0x00000000u)

#define CSL_ME_MON_ADDR_RESETVAL         (0x00000000u)

/* MON_DATA */

#define CSL_ME_MON_DATA_VALUE_MASK       (0xFFFFFFFFu)
#define CSL_ME_MON_DATA_VALUE_SHIFT      (0x00000000u)
#define CSL_ME_MON_DATA_VALUE_RESETVAL   (0x00000000u)

#define CSL_ME_MON_DATA_RESETVAL         (0x00000000u)

#endif
