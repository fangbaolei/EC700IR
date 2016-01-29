#ifndef _CSLR_BS_2_H_
#define _CSLR_BS_2_H_

#include <cslr.h>

#include "tistdtypes.h"


/* Minimum unit = 4 bytes */

/**************************************************************************\
* Register Overlay Structure
\**************************************************************************/
typedef struct  {
    volatile Uint32 PID;
    volatile Uint32 COUNT;
    volatile Uint32 CTRL;
    volatile Uint32 RSVD0;
    volatile Uint32 CURMBADDR;
    volatile Uint32 SLCMBCNT;
    volatile Uint32 MBPOSX;
    volatile Uint32 DP_SLC;
    volatile Uint32 DP_MB0;
    volatile Uint32 DP_MB1;
    volatile Uint32 DP_MBB0;
    volatile Uint32 DP_MBB1;
    volatile Uint32 BA_BS;
} CSL_BsRegs;

/**************************************************************************\
* Field Definition Macros
\**************************************************************************/

/* PID */

#define CSL_BS_PID_SCHEME_MASK           (0xC0000000u)
#define CSL_BS_PID_SCHEME_SHIFT          (0x0000001Eu)
#define CSL_BS_PID_SCHEME_RESETVAL       (0x00000001u)


#define CSL_BS_PID_FUNC_MASK             (0x0FFF0000u)
#define CSL_BS_PID_FUNC_SHIFT            (0x00000010u)
#define CSL_BS_PID_FUNC_RESETVAL         (0x00000C03u)

#define CSL_BS_PID_RTL_MASK              (0x0000F800u)
#define CSL_BS_PID_RTL_SHIFT             (0x0000000Bu)
#define CSL_BS_PID_RTL_RESETVAL          (0x00000000u)

#define CSL_BS_PID_MAJOR_MASK            (0x00000700u)
#define CSL_BS_PID_MAJOR_SHIFT           (0x00000008u)
#define CSL_BS_PID_MAJOR_RESETVAL        (0x00000001u)

#define CSL_BS_PID_CUSTOM_MASK           (0x000000C0u)
#define CSL_BS_PID_CUSTOM_SHIFT          (0x00000006u)
#define CSL_BS_PID_CUSTOM_RESETVAL       (0x00000000u)

#define CSL_BS_PID_MINOR_MASK            (0x0000003Fu)
#define CSL_BS_PID_MINOR_SHIFT           (0x00000000u)
#define CSL_BS_PID_MINOR_RESETVAL        (0x00000000u)

#define CSL_BS_PID_RESETVAL              (0x4C030100u)

/* COUNT */

#define CSL_BS_COUNT_EN_MASK             (0x80000000u)
#define CSL_BS_COUNT_EN_SHIFT            (0x0000001Fu)
#define CSL_BS_COUNT_EN_RESETVAL         (0x00000000u)
/*----EN Tokens----*/
#define CSL_BS_COUNT_EN_DISABLE          (0x00000000u)
#define CSL_BS_COUNT_EN_ENABLE           (0x00000001u)

#define CSL_BS_COUNT_RST_MASK            (0x40000000u)
#define CSL_BS_COUNT_RST_SHIFT           (0x0000001Eu)
#define CSL_BS_COUNT_RST_RESETVAL        (0x00000000u)
/*----RST Tokens----*/
#define CSL_BS_COUNT_RST_SET             (0x00000001u)


#define CSL_BS_COUNT_VALUE_MASK          (0x0000FFFFu)
#define CSL_BS_COUNT_VALUE_SHIFT         (0x00000000u)
#define CSL_BS_COUNT_VALUE_RESETVAL      (0x00000000u)

#define CSL_BS_COUNT_RESETVAL            (0x00000000u)

/* CTRL */


#define CSL_BS_CTRL_ADDR_MASK            (0x000FFC00u)
#define CSL_BS_CTRL_ADDR_SHIFT           (0x0000000Au)
#define CSL_BS_CTRL_ADDR_RESETVAL        (0x00000000u)


#define CSL_BS_CTRL_SSM_MASK             (0x00000004u)
#define CSL_BS_CTRL_SSM_SHIFT            (0x00000002u)
#define CSL_BS_CTRL_SSM_RESETVAL         (0x00000000u)
/*----SSM Tokens----*/
#define CSL_BS_CTRL_SSM_NORMAL           (0x00000000u)
#define CSL_BS_CTRL_SSM_SSP              (0x00000001u)

#define CSL_BS_CTRL_CSB_MASK             (0x00000002u)
#define CSL_BS_CTRL_CSB_SHIFT            (0x00000001u)
#define CSL_BS_CTRL_CSB_RESETVAL         (0x00000000u)
/*----CSB Tokens----*/
#define CSL_BS_CTRL_CSB_DEFINE           (0x00000000u)

#define CSL_BS_CTRL_EN_MASK              (0x00000001u)
#define CSL_BS_CTRL_EN_SHIFT             (0x00000000u)
#define CSL_BS_CTRL_EN_RESETVAL          (0x00000000u)
/*----EN Tokens----*/
#define CSL_BS_CTRL_EN_SET               (0x00000001u)

#define CSL_BS_CTRL_RESETVAL             (0x00000000u)

/* CURMBADDR */


#define CSL_BS_CURMBADDR_CURMBADDR_MASK  (0x0000FFFFu)
#define CSL_BS_CURMBADDR_CURMBADDR_SHIFT (0x00000000u)
#define CSL_BS_CURMBADDR_CURMBADDR_RESETVAL (0x00000000u)

#define CSL_BS_CURMBADDR_RESETVAL        (0x00000000u)

/* SLCMBCNT */


#define CSL_BS_SLCMBCNT_SLCMBCNT_MASK    (0x0000FFFFu)
#define CSL_BS_SLCMBCNT_SLCMBCNT_SHIFT   (0x00000000u)
#define CSL_BS_SLCMBCNT_SLCMBCNT_RESETVAL (0x00000000u)

#define CSL_BS_SLCMBCNT_RESETVAL         (0x00000000u)

/* MBPOSX */


#define CSL_BS_MBPOSX_MBPOSX_MASK        (0x000000FFu)
#define CSL_BS_MBPOSX_MBPOSX_SHIFT       (0x00000000u)
#define CSL_BS_MBPOSX_MBPOSX_RESETVAL    (0x00000000u)

#define CSL_BS_MBPOSX_RESETVAL           (0x00000000u)

/* DP_SLC */


#define CSL_BS_DP_SLC_DPSLC_MASK         (0x00000FF8u)
#define CSL_BS_DP_SLC_DPSLC_SHIFT        (0x00000003u)
#define CSL_BS_DP_SLC_DPSLC_RESETVAL     (0x00000000u)


#define CSL_BS_DP_SLC_RESETVAL           (0x00000000u)

/* DP_MB0 */


#define CSL_BS_DP_MB0_DPMB0_MASK         (0x000007F8u)
#define CSL_BS_DP_MB0_DPMB0_SHIFT        (0x00000003u)
#define CSL_BS_DP_MB0_DPMB0_RESETVAL     (0x00000000u)


#define CSL_BS_DP_MB0_RESETVAL           (0x00000000u)

/* DP_MB1 */


#define CSL_BS_DP_MB1_DPMB1_MASK         (0x000007F8u)
#define CSL_BS_DP_MB1_DPMB1_SHIFT        (0x00000003u)
#define CSL_BS_DP_MB1_DPMB1_RESETVAL     (0x00000000u)


#define CSL_BS_DP_MB1_RESETVAL           (0x00000000u)

/* DP_MBB0 */


#define CSL_BS_DP_MBB0_DPMBB0_MASK       (0x00000FF8u)
#define CSL_BS_DP_MBB0_DPMBB0_SHIFT      (0x00000003u)
#define CSL_BS_DP_MBB0_DPMBB0_RESETVAL   (0x00000000u)


#define CSL_BS_DP_MBB0_RESETVAL          (0x00000000u)

/* DP_MBB1 */


#define CSL_BS_DP_MBB1_DPMBB1_MASK       (0x00000FF8u)
#define CSL_BS_DP_MBB1_DPMBB1_SHIFT      (0x00000003u)
#define CSL_BS_DP_MBB1_DPMBB1_RESETVAL   (0x00000000u)


#define CSL_BS_DP_MBB1_RESETVAL          (0x00000000u)

/* BA_BS */


#define CSL_BS_BA_BS_BABS_MASK           (0x00000FF8u)
#define CSL_BS_BA_BS_BABS_SHIFT          (0x00000003u)
#define CSL_BS_BA_BS_BABS_RESETVAL       (0x00000000u)


#define CSL_BS_BA_BS_RESETVAL            (0x00000000u)

#endif
