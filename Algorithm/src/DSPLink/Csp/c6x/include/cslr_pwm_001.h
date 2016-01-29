#ifndef _CSLR_PWM_1_H_
#define _CSLR_PWM_1_H_
/*********************************************************************
 * Copyright (C) 2003-2004 Texas Instruments Incorporated. 
 * All Rights Reserved 
 *********************************************************************/
 /** \file cslr_pwm_1.h
 * 
 * \brief This file contains the Register Desciptions for PWM
 * 
 *********************************************************************/

#include <cslr.h>

#include <tistdtypes.h>

/**************************************************************************\
* Register Overlay Structure
\**************************************************************************/
typedef struct  {
    volatile Uint32 PID;
    volatile Uint32 PCR;
    volatile Uint32 CFG;
    volatile Uint32 START;
    volatile Uint32 RPT;
    volatile Uint32 PER;
    volatile Uint32 PH1D;
} CSL_PwmRegs;

/**************************************************************************\
* Field Definition Macros
\**************************************************************************/

/* PID */

#define CSL_PWM_PID_TID_MASK             (0x00FF0000u)
#define CSL_PWM_PID_TID_SHIFT            (0x00000010u)
#define CSL_PWM_PID_TID_RESETVAL         (0x00000000u)

#define CSL_PWM_PID_CID_MASK             (0x0000FF00u)
#define CSL_PWM_PID_CID_SHIFT            (0x00000008u)
#define CSL_PWM_PID_CID_RESETVAL         (0x00000000u)

#define CSL_PWM_PID_REV_MASK             (0x000000FFu)
#define CSL_PWM_PID_REV_SHIFT            (0x00000000u)
#define CSL_PWM_PID_REV_RESETVAL         (0x00000000u)

#define CSL_PWM_PID_RESETVAL             (0x00000000u)

/* PCR */

#define CSL_PWM_PCR_SOFT_MASK            (0x00000002u)
#define CSL_PWM_PCR_SOFT_SHIFT           (0x00000001u)
#define CSL_PWM_PCR_SOFT_RESETVAL        (0x00000000u)

#define CSL_PWM_PCR_FREE_MASK            (0x00000001u)
#define CSL_PWM_PCR_FREE_SHIFT           (0x00000000u)
#define CSL_PWM_PCR_FREE_RESETVAL        (0x00000000u)

/*----FREE Tokens----*/
#define CSL_PWM_PCR_FREE_STOP            (0x00000000u)
#define CSL_PWM_PCR_FREE_RUN             (0x00000001u)

#define CSL_PWM_PCR_RESETVAL             (0x00000000u)

/* CFG */

#define CSL_PWM_CFG_OPST_MASK            (0x00020000u)
#define CSL_PWM_CFG_OPST_SHIFT           (0x00000011u)
#define CSL_PWM_CFG_OPST_RESETVAL        (0x00000000u)

/*----OPST Tokens----*/
#define CSL_PWM_CFG_OPST_IDLE            (0x00000000u)
#define CSL_PWM_CFG_OPST_RUN             (0x00000001u)

#define CSL_PWM_CFG_CURLEV_MASK          (0x00010000u)
#define CSL_PWM_CFG_CURLEV_SHIFT         (0x00000010u)
#define CSL_PWM_CFG_CURLEV_RESETVAL      (0x00000000u)

/*----CURLEV Tokens----*/
#define CSL_PWM_CFG_CURLEV_LOW           (0x00000000u)
#define CSL_PWM_CFG_CURLEV_HIGH          (0x00000001u)

#define CSL_PWM_CFG_INTEN_MASK           (0x00000040u)
#define CSL_PWM_CFG_INTEN_SHIFT          (0x00000006u)
#define CSL_PWM_CFG_INTEN_RESETVAL       (0x00000000u)

/*----INTEN Tokens----*/
#define CSL_PWM_CFG_INTEN_DISABLE        (0x00000000u)
#define CSL_PWM_CFG_INTEN_ENABLE         (0x00000001u)

#define CSL_PWM_CFG_INACTOUT_MASK        (0x00000020u)
#define CSL_PWM_CFG_INACTOUT_SHIFT       (0x00000005u)
#define CSL_PWM_CFG_INACTOUT_RESETVAL    (0x00000000u)

/*----INACTOUT Tokens----*/
#define CSL_PWM_CFG_INACTOUT_LOW         (0x00000000u)
#define CSL_PWM_CFG_INACTOUT_HIGH        (0x00000001u)

#define CSL_PWM_CFG_P1OUT_MASK           (0x00000010u)
#define CSL_PWM_CFG_P1OUT_SHIFT          (0x00000004u)
#define CSL_PWM_CFG_P1OUT_RESETVAL       (0x00000000u)

/*----P1OUT Tokens----*/
#define CSL_PWM_CFG_P1OUT_LOW            (0x00000000u)
#define CSL_PWM_CFG_P1OUT_HIGH           (0x00000001u)

#define CSL_PWM_CFG_EVTRIG_MASK          (0x0000000Cu)
#define CSL_PWM_CFG_EVTRIG_SHIFT         (0x00000002u)
#define CSL_PWM_CFG_EVTRIG_RESETVAL      (0x00000000u)

/*----EVTRIG Tokens----*/
#define CSL_PWM_CFG_EVTRIG_DISABLE       (0x00000000u)
#define CSL_PWM_CFG_EVTRIG_PEDGE         (0x00000001u)
#define CSL_PWM_CFG_EVTRIG_NEDGE         (0x00000002u)
#define CSL_PWM_CFG_EVTRIG_RSV           (0x00000003u)

#define CSL_PWM_CFG_MODE_MASK            (0x00000003u)
#define CSL_PWM_CFG_MODE_SHIFT           (0x00000000u)
#define CSL_PWM_CFG_MODE_RESETVAL        (0x00000000u)

/*----MODE Tokens----*/
#define CSL_PWM_CFG_MODE_DISABLE         (0x00000000u)
#define CSL_PWM_CFG_MODE_OSHOT           (0x00000001u)
#define CSL_PWM_CFG_MODE_CONT            (0x00000002u)
#define CSL_PWM_CFG_MODE_RSV             (0x00000003u)

#define CSL_PWM_CFG_RESETVAL             (0x00000000u)

/* START */

#define CSL_PWM_START_START_MASK         (0x00000001u)
#define CSL_PWM_START_START_SHIFT        (0x00000000u)
#define CSL_PWM_START_START_RESETVAL     (0x00000000u)

/*----START Tokens----*/
#define CSL_PWM_START_START_START        (0x00000001u)

#define CSL_PWM_START_RESETVAL           (0x00000000u)

/* RPT */

#define CSL_PWM_RPT_RPT_MASK             (0x0000FFFFu)
#define CSL_PWM_RPT_RPT_SHIFT            (0x00000000u)
#define CSL_PWM_RPT_RPT_RESETVAL         (0x00000000u)

#define CSL_PWM_RPT_RESETVAL             (0x00000000u)

/* PER */

#define CSL_PWM_PER_PER_MASK             (0x0000FFFFu)
#define CSL_PWM_PER_PER_SHIFT            (0x00000000u)
#define CSL_PWM_PER_PER_RESETVAL         (0x00000000u)

#define CSL_PWM_PER_RESETVAL             (0x00000000u)

/* PH1D */

#define CSL_PWM_PH1D_PH1D_MASK           (0x0000FFFFu)
#define CSL_PWM_PH1D_PH1D_SHIFT          (0x00000000u)
#define CSL_PWM_PH1D_PH1D_RESETVAL       (0x00000000u)

#define CSL_PWM_PH1D_RESETVAL            (0x00000000u)

#endif
/* Rev.No.   Date/Time               ECN No.          Modifier      */
/* -------   ---------               -------          --------      */

/* 5         14 Jan 2005 13:32:35    5888             xkeshavm      */
/*                                                                  */
/* Uploaded the CSL0.57 JAN 2005 Release and built the library for ARM and DSP*/
/********************************************************************/ 
