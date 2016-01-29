#ifndef _CSLR_IPE_1_H_
#define _CSLR_IPE_1_H_

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
    volatile Uint32 NS;
    volatile Uint32 PRED;
} CSL_IpeRegs;

/**************************************************************************\
* Field Definition Macros
\**************************************************************************/

/* PID */

#define CSL_IPE_PID_SCHEME_MASK          (0xC0000000u)
#define CSL_IPE_PID_SCHEME_SHIFT         (0x0000001Eu)
#define CSL_IPE_PID_SCHEME_RESETVAL      (0x00000001u)


#define CSL_IPE_PID_FUNC_MASK            (0x0FFF0000u)
#define CSL_IPE_PID_FUNC_SHIFT           (0x00000010u)
#define CSL_IPE_PID_FUNC_RESETVAL        (0x00000C07u)

#define CSL_IPE_PID_RTL_MASK             (0x0000F800u)
#define CSL_IPE_PID_RTL_SHIFT            (0x0000000Bu)
#define CSL_IPE_PID_RTL_RESETVAL         (0x00000000u)

#define CSL_IPE_PID_MAJOR_MASK           (0x00000700u)
#define CSL_IPE_PID_MAJOR_SHIFT          (0x00000008u)
#define CSL_IPE_PID_MAJOR_RESETVAL       (0x00000001u)

#define CSL_IPE_PID_CUSTOM_MASK          (0x000000C0u)
#define CSL_IPE_PID_CUSTOM_SHIFT         (0x00000006u)
#define CSL_IPE_PID_CUSTOM_RESETVAL      (0x00000000u)

#define CSL_IPE_PID_MINOR_MASK           (0x0000003Fu)
#define CSL_IPE_PID_MINOR_SHIFT          (0x00000000u)
#define CSL_IPE_PID_MINOR_RESETVAL       (0x00000000u)

#define CSL_IPE_PID_RESETVAL             (0x4C070100u)

/* COUNT */

#define CSL_IPE_COUNT_EN_MASK            (0x80000000u)
#define CSL_IPE_COUNT_EN_SHIFT           (0x0000001Fu)
#define CSL_IPE_COUNT_EN_RESETVAL        (0x00000000u)
/*----EN Tokens----*/
#define CSL_IPE_COUNT_EN_DISABLE         (0x00000000u)
#define CSL_IPE_COUNT_EN_ENABLE          (0x00000001u)

#define CSL_IPE_COUNT_RST_MASK           (0x40000000u)
#define CSL_IPE_COUNT_RST_SHIFT          (0x0000001Eu)
#define CSL_IPE_COUNT_RST_RESETVAL       (0x00000000u)
/*----RST Tokens----*/
#define CSL_IPE_COUNT_RST_SET            (0x00000001u)


#define CSL_IPE_COUNT_VALUE_MASK         (0x0000FFFFu)
#define CSL_IPE_COUNT_VALUE_SHIFT        (0x00000000u)
#define CSL_IPE_COUNT_VALUE_RESETVAL     (0x00000000u)

#define CSL_IPE_COUNT_RESETVAL           (0x00000000u)

/* CTRL */


#define CSL_IPE_CTRL_ADDR_MASK           (0x000FF800u)
#define CSL_IPE_CTRL_ADDR_SHIFT          (0x0000000Bu)
#define CSL_IPE_CTRL_ADDR_RESETVAL       (0x00000000u)


#define CSL_IPE_CTRL_SSM_MASK            (0x00000004u)
#define CSL_IPE_CTRL_SSM_SHIFT           (0x00000002u)
#define CSL_IPE_CTRL_SSM_RESETVAL        (0x00000000u)
/*----SSM Tokens----*/
#define CSL_IPE_CTRL_SSM_NORMAL          (0x00000000u)
#define CSL_IPE_CTRL_SSM_SSP             (0x00000001u)


#define CSL_IPE_CTRL_EN_MASK             (0x00000001u)
#define CSL_IPE_CTRL_EN_SHIFT            (0x00000000u)
#define CSL_IPE_CTRL_EN_RESETVAL         (0x00000000u)
/*----EN Tokens----*/
#define CSL_IPE_CTRL_EN_SET              (0x00000001u)

#define CSL_IPE_CTRL_RESETVAL            (0x00000000u)

/* NS */


#define CSL_IPE_NS_BOTTOM_MASK           (0x00000002u)
#define CSL_IPE_NS_BOTTOM_SHIFT          (0x00000001u)
#define CSL_IPE_NS_BOTTOM_RESETVAL       (0x00000000u)
/*----BOTTOM Tokens----*/
#define CSL_IPE_NS_BOTTOM_CLR            (0x00000000u)
#define CSL_IPE_NS_BOTTOM_SET            (0x00000001u)

#define CSL_IPE_NS_TOP_MASK              (0x00000001u)
#define CSL_IPE_NS_TOP_SHIFT             (0x00000000u)
#define CSL_IPE_NS_TOP_RESETVAL          (0x00000000u)
/*----TOP Tokens----*/
#define CSL_IPE_NS_TOP_CLR               (0x00000000u)
#define CSL_IPE_NS_TOP_SET               (0x00000001u)

#define CSL_IPE_NS_RESETVAL              (0x00000000u)

/* PRED */

#define CSL_IPE_PRED_A7_MASK             (0xF0000000u)
#define CSL_IPE_PRED_A7_SHIFT            (0x0000001Cu)
#define CSL_IPE_PRED_A7_RESETVAL         (0x00000000u)

#define CSL_IPE_PRED_A6_MASK             (0x0F000000u)
#define CSL_IPE_PRED_A6_SHIFT            (0x00000018u)
#define CSL_IPE_PRED_A6_RESETVAL         (0x00000000u)

#define CSL_IPE_PRED_A5_MASK             (0x00F00000u)
#define CSL_IPE_PRED_A5_SHIFT            (0x00000014u)
#define CSL_IPE_PRED_A5_RESETVAL         (0x00000000u)

#define CSL_IPE_PRED_A4_MASK             (0x000F0000u)
#define CSL_IPE_PRED_A4_SHIFT            (0x00000010u)
#define CSL_IPE_PRED_A4_RESETVAL         (0x00000000u)

#define CSL_IPE_PRED_A3_MASK             (0x0000F000u)
#define CSL_IPE_PRED_A3_SHIFT            (0x0000000Cu)
#define CSL_IPE_PRED_A3_RESETVAL         (0x00000000u)

#define CSL_IPE_PRED_A2_MASK             (0x00000F00u)
#define CSL_IPE_PRED_A2_SHIFT            (0x00000008u)
#define CSL_IPE_PRED_A2_RESETVAL         (0x00000000u)

#define CSL_IPE_PRED_A1_MASK             (0x000000F0u)
#define CSL_IPE_PRED_A1_SHIFT            (0x00000004u)
#define CSL_IPE_PRED_A1_RESETVAL         (0x00000000u)

#define CSL_IPE_PRED_A0_MASK             (0x0000000Fu)
#define CSL_IPE_PRED_A0_SHIFT            (0x00000000u)
#define CSL_IPE_PRED_A0_RESETVAL         (0x00000000u)

#define CSL_IPE_PRED_RESETVAL            (0x00000000u)

#endif
