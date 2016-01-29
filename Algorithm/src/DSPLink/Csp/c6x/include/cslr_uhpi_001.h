#ifndef _CSLR_UHPI_1_H_
#define _CSLR_UHPI_1_H_
/*********************************************************************
 * Copyright (C) 2003-2004 Texas Instruments Incorporated. 
 * All Rights Reserved 
 *********************************************************************/
 /** \file cslr_uhpi_1.h
 * 
 * \brief This file contains the Register Desciptions for UHPI
 * 
 *********************************************************************/

#include <cslr.h>

#include <tistdtypes.h>

/**************************************************************************\
* Register Overlay Structure
\**************************************************************************/
typedef struct  {
    volatile Uint32 PID;
    volatile Uint32 PWREMU_MGMT;
    volatile Uint32 GeneralPurposeInterruptControlregister;
    volatile Uint32 GPIO_EN;
    volatile Uint32 GPIO_DIR1;
    volatile Uint32 GPIO_DAT1;
    volatile Uint32 GPIO_DIR2;
    volatile Uint32 GPIO_DAT2;
    volatile Uint32 GPIO_DIR3;
    volatile Uint32 GPIO_DAT3;
    volatile Uint32 RESV1;
    volatile Uint32 RESV2;
    volatile Uint32 HPIC;
    volatile Uint32 HPIAW;
    volatile Uint32 HPIAR;
    volatile Uint32 XHPIAW;
    volatile Uint32 XHPIAR;
	volatile Uint32 XDK_STEP;
} CSL_UhpiRegs;

/**************************************************************************\
* Field Definition Macros
\**************************************************************************/

/* PID */

#define CSL_UHPI_PID_TID_MASK            (0x00FF0000u)
#define CSL_UHPI_PID_TID_SHIFT           (0x00000010u)
#define CSL_UHPI_PID_TID_RESETVAL        (0x00000001u)

#define CSL_UHPI_PID_CID_MASK            (0x0000FF00u)
#define CSL_UHPI_PID_CID_SHIFT           (0x00000008u)
#define CSL_UHPI_PID_CID_RESETVAL        (0x00000002u)

#define CSL_UHPI_PID_PREV_MASK           (0x000000FFu)
#define CSL_UHPI_PID_PREV_SHIFT          (0x00000000u)
#define CSL_UHPI_PID_PREV_RESETVAL       (0x00000000u)

#define CSL_UHPI_PID_RESETVAL            (0x00010200u)

/* PWREMU_MGMT */

#define CSL_UHPI_PWREMU_MGMT_RESERVED_MASK (0x0001FFFCu)
#define CSL_UHPI_PWREMU_MGMT_RESERVED_SHIFT (0x00000002u)
#define CSL_UHPI_PWREMU_MGMT_RESERVED_RESETVAL (0x00000000u)

#define CSL_UHPI_PWREMU_MGMT_FREE_MASK   (0x00000002u)
#define CSL_UHPI_PWREMU_MGMT_FREE_SHIFT  (0x00000001u)
#define CSL_UHPI_PWREMU_MGMT_FREE_RESETVAL (0x00000000u)

/*----FREE Tokens----*/
#define CSL_UHPI_PWREMU_MGMT_FREE_NOTFREE (0x00000000u)
#define CSL_UHPI_PWREMU_MGMT_FREE_FREE   (0x00000001u)

#define CSL_UHPI_PWREMU_MGMT_SOFT_MASK   (0x00000001u)
#define CSL_UHPI_PWREMU_MGMT_SOFT_SHIFT  (0x00000000u)
#define CSL_UHPI_PWREMU_MGMT_SOFT_RESETVAL (0x00000000u)

/*----SOFT Tokens----*/
#define CSL_UHPI_PWREMU_MGMT_SOFT_OFF    (0x00000000u)
#define CSL_UHPI_PWREMU_MGMT_SOFT_IDLE   (0x00000001u)

#define CSL_UHPI_PWREMU_MGMT_RESETVAL    (0x00000000u)

/* GeneralPurposeInterruptControlregister */

#define CSL_UHPI_GENERALPURPOSEINTERRUPTCONTROLREGISTER_RESETVAL (0x00000000u)

/* GPIO_EN */

#define CSL_UHPI_GPIO_EN_RESETVAL        (0x00000000u)

/* GPIO_DIR1 */

#define CSL_UHPI_GPIO_DIR1_RESETVAL      (0x00000000u)

/* GPIO_DAT1 */

#define CSL_UHPI_GPIO_DAT1_RESETVAL      (0x00000000u)

/* GPIO_DIR2 */

#define CSL_UHPI_GPIO_DIR2_RESETVAL      (0x00000000u)

/* GPIO_DAT2 */

#define CSL_UHPI_GPIO_DAT2_RESETVAL      (0x00000000u)

/* GPIO_DIR3 */

#define CSL_UHPI_GPIO_DIR3_RESETVAL      (0x00000000u)

/* GPIO_DAT3 */

#define CSL_UHPI_GPIO_DAT3_RESETVAL      (0x00000000u)

/* RESV1 */

#define CSL_UHPI_RESV1_RESETVAL          (0x00000000u)

/* RESV2 */

#define CSL_UHPI_RESV2_RESETVAL          (0x00000000u)

/* HPIC */

#define CSL_UHPI_HPIC_HPIA_RW_SEL_MASK   (0x00000800u)
#define CSL_UHPI_HPIC_HPIA_RW_SEL_SHIFT  (0x0000000Bu)
#define CSL_UHPI_HPIC_HPIA_RW_SEL_RESETVAL (0x00000000u)

#define CSL_UHPI_HPIC_LB_MODE_MASK       (0x00000400u)
#define CSL_UHPI_HPIC_LB_MODE_SHIFT      (0x0000000Au)
#define CSL_UHPI_HPIC_LB_MODE_RESETVAL   (0x00000000u)

#define CSL_UHPI_HPIC_DUAL_HPIA_MASK     (0x00000200u)
#define CSL_UHPI_HPIC_DUAL_HPIA_SHIFT    (0x00000009u)
#define CSL_UHPI_HPIC_DUAL_HPIA_RESETVAL (0x00000000u)

#define CSL_UHPI_HPIC_HWOB_STAT_MASK     (0x00000100u)
#define CSL_UHPI_HPIC_HWOB_STAT_SHIFT    (0x00000008u)
#define CSL_UHPI_HPIC_HWOB_STAT_RESETVAL (0x00000000u)

#define CSL_UHPI_HPIC_HPI_RST_MASK       (0x00000080u)
#define CSL_UHPI_HPIC_HPI_RST_SHIFT      (0x00000007u)
#define CSL_UHPI_HPIC_HPI_RST_RESETVAL   (0x00000000u)

#define CSL_UHPI_HPIC_RESET_MASK         (0x00000040u)
#define CSL_UHPI_HPIC_RESET_SHIFT        (0x00000006u)
#define CSL_UHPI_HPIC_RESET_RESETVAL     (0x00000000u)

#define CSL_UHPI_HPIC_XHPIA_MASK         (0x00000020u)
#define CSL_UHPI_HPIC_XHPIA_SHIFT        (0x00000005u)
#define CSL_UHPI_HPIC_XHPIA_RESETVAL     (0x00000000u)

#define CSL_UHPI_HPIC_FETCH_MASK         (0x00000010u)
#define CSL_UHPI_HPIC_FETCH_SHIFT        (0x00000004u)
#define CSL_UHPI_HPIC_FETCH_RESETVAL     (0x00000000u)

#define CSL_UHPI_HPIC_HRDY_MASK          (0x00000008u)
#define CSL_UHPI_HPIC_HRDY_SHIFT         (0x00000003u)
#define CSL_UHPI_HPIC_HRDY_RESETVAL      (0x00000000u)

#define CSL_UHPI_HPIC_HINT_MASK          (0x00000004u)
#define CSL_UHPI_HPIC_HINT_SHIFT         (0x00000002u)
#define CSL_UHPI_HPIC_HINT_RESETVAL      (0x00000000u)

#define CSL_UHPI_HPIC_DSP_INT_MASK       (0x00000002u)
#define CSL_UHPI_HPIC_DSP_INT_SHIFT      (0x00000001u)
#define CSL_UHPI_HPIC_DSP_INT_RESETVAL   (0x00000000u)

#define CSL_UHPI_HPIC_HWOB_MASK          (0x00000001u)
#define CSL_UHPI_HPIC_HWOB_SHIFT         (0x00000000u)
#define CSL_UHPI_HPIC_HWOB_RESETVAL      (0x00000000u)

/*----HWOB Tokens----*/
#define CSL_UHPI_HPIC_HWOB_FIRST_MSB     (0x00000000u)
#define CSL_UHPI_HPIC_HWOB_FIRST_LSB     (0x00000001u)

#define CSL_UHPI_HPIC_RESETVAL           (0x00000000u)

/* HPIAW */

#define CSL_UHPI_HPIAW_HPIAW_MASK        (0xFFFFFFFFu)
#define CSL_UHPI_HPIAW_HPIAW_SHIFT       (0x00000000u)
#define CSL_UHPI_HPIAW_HPIAW_RESETVAL    (0x00000000u)

#define CSL_UHPI_HPIAW_RESETVAL          (0x00000000u)

/* HPIAR */

#define CSL_UHPI_HPIAR_HPIAR_MASK        (0xFFFFFFFFu)
#define CSL_UHPI_HPIAR_HPIAR_SHIFT       (0x00000000u)
#define CSL_UHPI_HPIAR_HPIAR_RESETVAL    (0x00000000u)

#define CSL_UHPI_HPIAR_RESETVAL          (0x00000000u)

/* XHPIAW */

#define CSL_UHPI_XHPIAW_XHPIAW_MASK      (0xFFFFFFFFu)
#define CSL_UHPI_XHPIAW_XHPIAW_SHIFT     (0x00000000u)
#define CSL_UHPI_XHPIAW_XHPIAW_RESETVAL  (0x00000000u)

#define CSL_UHPI_XHPIAW_RESETVAL         (0x00000000u)

/* XHPIAR */

#define CSL_UHPI_XHPIAR_XHPIAR_MASK      (0xFFFFFFFFu)
#define CSL_UHPI_XHPIAR_XHPIAR_SHIFT     (0x00000000u)
#define CSL_UHPI_XHPIAR_XHPIAR_RESETVAL  (0x00000000u)

#define CSL_UHPI_XHPIAR_RESETVAL         (0x00000000u)

/* XDK_STEP */

#define CSL_UHPI_XDK_STEP_XDK_EN_MASK				(0x00000100u)
#define CSL_UHPI_XDK_STEP_XDK_EN_SHIFT				(0x00000008u)
#define CSL_UHPI_XDK_STEP_XDK_EN_RESETVAL			(0x00000000u)

#define CSL_UHPI_XDK_STEP_PTN_A_STEP_MASK			(0x000000F0u)	
#define CSL_UHPI_XDK_STEP_PTN_A_STEP_SHIFT			(0x00000004u)
#define CSL_UHPI_XDK_STEP_PTN_A_STEP_RESETVAL		(0x00000000u)

#define CSL_UHPI_XDK_STEP_PTN_B_STEP_MASK			(0x0000000Fu)
#define CSL_UHPI_XDK_STEP_PTN_B_STEP_SHIFT			(0x00000000u)
#define CSL_UHPI_XDK_STEP_PTN_B_STEP_RESETVAL		(0x00000000u)

#define CSL_UHPI_XDK_STEP_RESETVAL					(0x00000000u) /* comment by Paul: unknown reset value for bits, default to 0) */

#endif
