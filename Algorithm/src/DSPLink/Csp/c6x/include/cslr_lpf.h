#ifndef _CSLR_LPF_1_H_
#define _CSLR_LPF_1_H_

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
    volatile Uint32 MAXEDGE_Y_H;
    volatile Uint32 MAXEDGE_Y_V;
    volatile Uint32 MAXEDGE_C;
} CSL_LpfRegs;

/**************************************************************************\
* Field Definition Macros
\**************************************************************************/

/* PID */

#define CSL_LPF_PID_PID_MASK             (0xFFFFFFFFu)
#define CSL_LPF_PID_PID_SHIFT            (0x00000000u)
#define CSL_LPF_PID_PID_RESETVAL         (0x4C050100u)

#define CSL_LPF_PID_RESETVAL             (0x4C050100u)

/* CNT */

#define CSL_LPF_CNT_EN_MASK              (0x80000000u)
#define CSL_LPF_CNT_EN_SHIFT             (0x0000001Fu)
#define CSL_LPF_CNT_EN_RESETVAL          (0x00000000u)
/*----EN Tokens----*/
#define CSL_LPF_CNT_EN_DISABLE           (0x00000000u)
#define CSL_LPF_CNT_EN_ENABLE            (0x00000001u)

#define CSL_LPF_CNT_RST_MASK             (0x40000000u)
#define CSL_LPF_CNT_RST_SHIFT            (0x0000001Eu)
#define CSL_LPF_CNT_RST_RESETVAL         (0x00000000u)
/*----RST Tokens----*/
#define CSL_LPF_CNT_RST_SET              (0x00000001u)


#define CSL_LPF_CNT_VALUE_MASK           (0x0000FFFFu)
#define CSL_LPF_CNT_VALUE_SHIFT          (0x00000000u)
#define CSL_LPF_CNT_VALUE_RESETVAL       (0x00000000u)

#define CSL_LPF_CNT_RESETVAL             (0x00000000u)

/* CTRL */


#define CSL_LPF_CTRL_ADDR_MASK           (0x003FF000u)
#define CSL_LPF_CTRL_ADDR_SHIFT          (0x0000000Cu)
#define CSL_LPF_CTRL_ADDR_RESETVAL       (0x00000000u)


#define CSL_LPF_CTRL_SSM_MASK            (0x00000004u)
#define CSL_LPF_CTRL_SSM_SHIFT           (0x00000002u)
#define CSL_LPF_CTRL_SSM_RESETVAL        (0x00000000u)
/*----SSM Tokens----*/
#define CSL_LPF_CTRL_SSM_NORMAL          (0x00000000u)
#define CSL_LPF_CTRL_SSM_SSP             (0x00000001u)

#define CSL_LPF_CTRL_CSB_MASK            (0x00000002u)
#define CSL_LPF_CTRL_CSB_SHIFT           (0x00000001u)
#define CSL_LPF_CTRL_CSB_RESETVAL        (0x00000000u)
/*----CSB Tokens----*/
#define CSL_LPF_CTRL_CSB_DEFINE          (0x00000000u)

#define CSL_LPF_CTRL_EN_MASK             (0x00000001u)
#define CSL_LPF_CTRL_EN_SHIFT            (0x00000000u)
#define CSL_LPF_CTRL_EN_RESETVAL         (0x00000000u)
/*----EN Tokens----*/
#define CSL_LPF_CTRL_EN_SET              (0x00000001u)

#define CSL_LPF_CTRL_RESETVAL            (0x00000000u)

/* MAXEDGE_Y_H */

#define CSL_LPF_MAXEDGE_Y_H_LUMA_4_MASK  (0xFF000000u)
#define CSL_LPF_MAXEDGE_Y_H_LUMA_4_SHIFT (0x00000018u)
#define CSL_LPF_MAXEDGE_Y_H_LUMA_4_RESETVAL (0x00000000u)

#define CSL_LPF_MAXEDGE_Y_H_LUMA_3_MASK  (0x00FF0000u)
#define CSL_LPF_MAXEDGE_Y_H_LUMA_3_SHIFT (0x00000010u)
#define CSL_LPF_MAXEDGE_Y_H_LUMA_3_RESETVAL (0x00000000u)

#define CSL_LPF_MAXEDGE_Y_H_LUMA_2_MASK  (0x0000FF00u)
#define CSL_LPF_MAXEDGE_Y_H_LUMA_2_SHIFT (0x00000008u)
#define CSL_LPF_MAXEDGE_Y_H_LUMA_2_RESETVAL (0x00000000u)

#define CSL_LPF_MAXEDGE_Y_H_LUMA_1_MASK  (0x000000FFu)
#define CSL_LPF_MAXEDGE_Y_H_LUMA_1_SHIFT (0x00000000u)
#define CSL_LPF_MAXEDGE_Y_H_LUMA_1_RESETVAL (0x00000000u)

#define CSL_LPF_MAXEDGE_Y_H_RESETVAL     (0x00000000u)

/* MAXEDGE_Y_V */

#define CSL_LPF_MAXEDGE_Y_V_LUMA_4_MASK  (0xFF000000u)
#define CSL_LPF_MAXEDGE_Y_V_LUMA_4_SHIFT (0x00000018u)
#define CSL_LPF_MAXEDGE_Y_V_LUMA_4_RESETVAL (0x00000000u)

#define CSL_LPF_MAXEDGE_Y_V_LUMA_3_MASK  (0x00FF0000u)
#define CSL_LPF_MAXEDGE_Y_V_LUMA_3_SHIFT (0x00000010u)
#define CSL_LPF_MAXEDGE_Y_V_LUMA_3_RESETVAL (0x00000000u)

#define CSL_LPF_MAXEDGE_Y_V_LUMA_2_MASK  (0x0000FF00u)
#define CSL_LPF_MAXEDGE_Y_V_LUMA_2_SHIFT (0x00000008u)
#define CSL_LPF_MAXEDGE_Y_V_LUMA_2_RESETVAL (0x00000000u)

#define CSL_LPF_MAXEDGE_Y_V_LUMA_1_MASK  (0x000000FFu)
#define CSL_LPF_MAXEDGE_Y_V_LUMA_1_SHIFT (0x00000000u)
#define CSL_LPF_MAXEDGE_Y_V_LUMA_1_RESETVAL (0x00000000u)

#define CSL_LPF_MAXEDGE_Y_V_RESETVAL     (0x00000000u)

/* MAXEDGE_C */

#define CSL_LPF_MAXEDGE_C_EHCB_MASK      (0xFF000000u)
#define CSL_LPF_MAXEDGE_C_EHCB_SHIFT     (0x00000018u)
#define CSL_LPF_MAXEDGE_C_EHCB_RESETVAL  (0x00000000u)

#define CSL_LPF_MAXEDGE_C_EVCB_MASK      (0x00FF0000u)
#define CSL_LPF_MAXEDGE_C_EVCB_SHIFT     (0x00000010u)
#define CSL_LPF_MAXEDGE_C_EVCB_RESETVAL  (0x00000000u)

#define CSL_LPF_MAXEDGE_C_EHCR_MASK      (0x0000FF00u)
#define CSL_LPF_MAXEDGE_C_EHCR_SHIFT     (0x00000008u)
#define CSL_LPF_MAXEDGE_C_EHCR_RESETVAL  (0x00000000u)

#define CSL_LPF_MAXEDGE_C_EVCR_MASK      (0x000000FFu)
#define CSL_LPF_MAXEDGE_C_EVCR_SHIFT     (0x00000000u)
#define CSL_LPF_MAXEDGE_C_EVCR_RESETVAL  (0x00000000u)

#define CSL_LPF_MAXEDGE_C_RESETVAL       (0x00000000u)

#endif
