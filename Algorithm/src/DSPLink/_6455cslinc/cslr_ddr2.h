/*********************************************************************
 * Copyright (C) 2003-2005 Texas Instruments Incorporated. 
 * All Rights Reserved 
 *********************************************************************/

/** \file cslr_ddr2.h
 * 
 * \brief This file contains the Register Desciptions for DDR2
 * 
 *********************************************************************/

#ifndef _CSLR_DDR2_H_
#define _CSLR_DDR2_H_

#include <cslr.h>

#include <tistdtypes.h>

/**************************************************************************\
* Register Overlay Structure
\**************************************************************************/
typedef struct  {
    volatile Uint32 MIDR;
    volatile Uint32 DMCSTAT;
    volatile Uint32 SDCFG;
    volatile Uint32 SDRFC;
    volatile Uint32 SDTIM1;
    volatile Uint32 SDTIM2;
    volatile Uint8 RSVD0[8];
    volatile Uint32 BPRIO;
    volatile Uint8 RSVD1[192];
    volatile Uint32 DMCCTL;
} CSL_Ddr2Regs;

/**************************************************************************\
* Overlay structure typedef definition
\**************************************************************************/
typedef volatile CSL_Ddr2Regs  *CSL_Ddr2RegsOvly;

/**************************************************************************\
* Field Definition Macros
\**************************************************************************/

/* MIDR */

#define CSL_DDR2_MIDR_MOD_ID_MASK        (0x3FFF0000u)
#define CSL_DDR2_MIDR_MOD_ID_SHIFT       (0x00000010u)
#define CSL_DDR2_MIDR_MOD_ID_RESETVAL    (0x00000031u)

#define CSL_DDR2_MIDR_MJ_REV_MASK        (0x0000FF00u)
#define CSL_DDR2_MIDR_MJ_REV_SHIFT       (0x00000008u)
#define CSL_DDR2_MIDR_MJ_REV_RESETVAL    (0x00000003u)

#define CSL_DDR2_MIDR_MN_REV_MASK        (0x000000FFu)
#define CSL_DDR2_MIDR_MN_REV_SHIFT       (0x00000000u)
#define CSL_DDR2_MIDR_MN_REV_RESETVAL    (0x0000000Fu)

#define CSL_DDR2_MIDR_RESETVAL           (0x0031030Fu)

/* DMCSTAT */

#define CSL_DDR2_DMCSTAT_BE_MASK         (0x80000000u)
#define CSL_DDR2_DMCSTAT_BE_SHIFT        (0x0000001Fu)
#define CSL_DDR2_DMCSTAT_BE_RESETVAL     (0x00000000u)

/*----BE Tokens----*/
#define CSL_DDR2_DMCSTAT_BE_B_ENDIAN     (0x00000001u)
#define CSL_DDR2_DMCSTAT_BE_L_ENDIAN     (0x00000000u)

#define CSL_DDR2_DMCSTAT_IFRDY_MASK      (0x00000004u)
#define CSL_DDR2_DMCSTAT_IFRDY_SHIFT     (0x00000002u)
#define CSL_DDR2_DMCSTAT_IFRDY_RESETVAL  (0x00000000u)

/*----IFRDY Tokens----*/
#define CSL_DDR2_DMCSTAT_IFRDY_NOTREADY  (0x00000000u)
#define CSL_DDR2_DMCSTAT_IFRDY_READY     (0x00000001u)

#define CSL_DDR2_DMCSTAT_RESETVAL        (0x40000000u)

/* SDCFG */

#define CSL_DDR2_SDCFG_BOOT_UNLOCK_MASK  (0x00800000u)
#define CSL_DDR2_SDCFG_BOOT_UNLOCK_SHIFT (0x00000017u)
#define CSL_DDR2_SDCFG_BOOT_UNLOCK_RESETVAL (0x00000000u)

/*----BOOT_UNLOCK Tokens----*/
#define CSL_DDR2_SDCFG_BOOT_UNLOCK_LOCKED (0x00000000u)
#define CSL_DDR2_SDCFG_BOOT_UNLOCK_UNLOCKED (0x00000001u)

#define CSL_DDR2_SDCFG_DDR_DRIVE_MASK    (0x00040000u)
#define CSL_DDR2_SDCFG_DDR_DRIVE_SHIFT   (0x00000012u)
#define CSL_DDR2_SDCFG_DDR_DRIVE_RESETVAL (0x00000000u)

/*----DDR_DRIVE Tokens----*/
#define CSL_DDR2_SDCFG_DDR_DRIVE_NORMAL  (0x00000000u)
#define CSL_DDR2_SDCFG_DDR_DRIVE_WEAK    (0x00000001u)

#define CSL_DDR2_SDCFG_TIMUNLOCK_MASK    (0x00008000u)
#define CSL_DDR2_SDCFG_TIMUNLOCK_SHIFT   (0x0000000Fu)
#define CSL_DDR2_SDCFG_TIMUNLOCK_RESETVAL (0x00000000u)

/*----TIMUNLOCK Tokens----*/
#define CSL_DDR2_SDCFG_TIMUNLOCK_CLEAR   (0x00000000u)
#define CSL_DDR2_SDCFG_TIMUNLOCK_SET     (0x00000001u)

#define CSL_DDR2_SDCFG_NM_MASK           (0x00004000u)
#define CSL_DDR2_SDCFG_NM_SHIFT          (0x0000000Eu)
#define CSL_DDR2_SDCFG_NM_RESETVAL       (0x00000000u)

/*----NM Tokens----*/
#define CSL_DDR2_SDCFG_NM_32BIT          (0x00000000u)
#define CSL_DDR2_SDCFG_NM_16BIT          (0x00000001u)

#define CSL_DDR2_SDCFG_CL_MASK           (0x00000E00u)
#define CSL_DDR2_SDCFG_CL_SHIFT          (0x00000009u)
#define CSL_DDR2_SDCFG_CL_RESETVAL       (0x00000005u)

/*----CL Tokens----*/
#define CSL_DDR2_SDCFG_CL_TWO            (0x00000002u)
#define CSL_DDR2_SDCFG_CL_THREE          (0x00000003u)
#define CSL_DDR2_SDCFG_CL_FOUR           (0x00000004u)
#define CSL_DDR2_SDCFG_CL_FIVE           (0x00000005u)

#define CSL_DDR2_SDCFG_IBANK_MASK        (0x00000070u)
#define CSL_DDR2_SDCFG_IBANK_SHIFT       (0x00000004u)
#define CSL_DDR2_SDCFG_IBANK_RESETVAL    (0x00000002u)

/*----IBANK Tokens----*/
#define CSL_DDR2_SDCFG_IBANK_ONE         (0x00000000u)
#define CSL_DDR2_SDCFG_IBANK_TWO         (0x00000001u)
#define CSL_DDR2_SDCFG_IBANK_FOUR        (0x00000002u)
#define CSL_DDR2_SDCFG_IBANK_EIGHT       (0x00000003u)

#define CSL_DDR2_SDCFG_PAGESIZE_MASK     (0x00000007u)
#define CSL_DDR2_SDCFG_PAGESIZE_SHIFT    (0x00000000u)
#define CSL_DDR2_SDCFG_PAGESIZE_RESETVAL (0x00000000u)

/*----PAGESIZE Tokens----*/
#define CSL_DDR2_SDCFG_PAGESIZE_256W_PAGE (0x00000000u)
#define CSL_DDR2_SDCFG_PAGESIZE_512W_PAGE (0x00000001u)
#define CSL_DDR2_SDCFG_PAGESIZE_1024W_PAGE (0x00000002u)
#define CSL_DDR2_SDCFG_PAGESIZE_2048W_PAGE (0x00000003u)

#define CSL_DDR2_SDCFG_RESETVAL          (0x00530A20u)

/* SDRFC */

#define CSL_DDR2_SDRFC_SR_MASK           (0x80000000u)
#define CSL_DDR2_SDRFC_SR_SHIFT          (0x0000001Fu)
#define CSL_DDR2_SDRFC_SR_RESETVAL       (0x00000000u)

/*----SR Tokens----*/
#define CSL_DDR2_SDRFC_SR_EXIT           (0x00000000u)
#define CSL_DDR2_SDRFC_SR_ENTER          (0x00000001u)

#define CSL_DDR2_SDRFC_REFRESH_RATE_MASK (0x0000FFFFu)
#define CSL_DDR2_SDRFC_REFRESH_RATE_SHIFT (0x00000000u)
#define CSL_DDR2_SDRFC_REFRESH_RATE_RESETVAL (0x00000753u)

#define CSL_DDR2_SDRFC_RESETVAL          (0x00000753u)

/* SDTIM1 */

#define CSL_DDR2_SDTIM1_T_RFC_MASK       (0xFE000000u)
#define CSL_DDR2_SDTIM1_T_RFC_SHIFT      (0x00000019u)
#define CSL_DDR2_SDTIM1_T_RFC_RESETVAL   (0x0000003Fu)

#define CSL_DDR2_SDTIM1_T_RP_MASK        (0x01C00000u)
#define CSL_DDR2_SDTIM1_T_RP_SHIFT       (0x00000016u)
#define CSL_DDR2_SDTIM1_T_RP_RESETVAL    (0x00000007u)

#define CSL_DDR2_SDTIM1_T_RCD_MASK       (0x00380000u)
#define CSL_DDR2_SDTIM1_T_RCD_SHIFT      (0x00000013u)
#define CSL_DDR2_SDTIM1_T_RCD_RESETVAL   (0x00000007u)

#define CSL_DDR2_SDTIM1_T_WR_MASK        (0x00070000u)
#define CSL_DDR2_SDTIM1_T_WR_SHIFT       (0x00000010u)
#define CSL_DDR2_SDTIM1_T_WR_RESETVAL    (0x00000007u)

#define CSL_DDR2_SDTIM1_T_RAS_MASK       (0x0000F800u)
#define CSL_DDR2_SDTIM1_T_RAS_SHIFT      (0x0000000Bu)
#define CSL_DDR2_SDTIM1_T_RAS_RESETVAL   (0x0000001Fu)

#define CSL_DDR2_SDTIM1_T_RC_MASK        (0x000007C0u)
#define CSL_DDR2_SDTIM1_T_RC_SHIFT       (0x00000006u)
#define CSL_DDR2_SDTIM1_T_RC_RESETVAL    (0x0000001Fu)

#define CSL_DDR2_SDTIM1_T_RRD_MASK       (0x00000038u)
#define CSL_DDR2_SDTIM1_T_RRD_SHIFT      (0x00000003u)
#define CSL_DDR2_SDTIM1_T_RRD_RESETVAL   (0x00000007u)

#define CSL_DDR2_SDTIM1_T_WTR_MASK       (0x00000003u)
#define CSL_DDR2_SDTIM1_T_WTR_SHIFT      (0x00000000u)
#define CSL_DDR2_SDTIM1_T_WTR_RESETVAL   (0x00000003u)

#define CSL_DDR2_SDTIM1_RESETVAL         (0x7FFFFFFBu)

/* SDTIM2 */

#define CSL_DDR2_SDTIM2_T_ODT_MASK       (0x01800000u)
#define CSL_DDR2_SDTIM2_T_ODT_SHIFT      (0x00000017u)
#define CSL_DDR2_SDTIM2_T_ODT_RESETVAL   (0x00000003u)

#define CSL_DDR2_SDTIM2_T_SXNR_MASK      (0x007F0000u)
#define CSL_DDR2_SDTIM2_T_SXNR_SHIFT     (0x00000010u)
#define CSL_DDR2_SDTIM2_T_SXNR_RESETVAL  (0x0000007Fu)

#define CSL_DDR2_SDTIM2_T_SXRD_MASK      (0x0000FF00u)
#define CSL_DDR2_SDTIM2_T_SXRD_SHIFT     (0x00000008u)
#define CSL_DDR2_SDTIM2_T_SXRD_RESETVAL  (0x000000FFu)

#define CSL_DDR2_SDTIM2_T_RTP_MASK       (0x000000E0u)
#define CSL_DDR2_SDTIM2_T_RTP_SHIFT      (0x00000005u)
#define CSL_DDR2_SDTIM2_T_RTP_RESETVAL   (0x00000007u)

#define CSL_DDR2_SDTIM2_T_CKE_MASK       (0x0000001Fu)
#define CSL_DDR2_SDTIM2_T_CKE_SHIFT      (0x00000000u)
#define CSL_DDR2_SDTIM2_T_CKE_RESETVAL   (0x0000001Fu)

#define CSL_DDR2_SDTIM2_RESETVAL         (0x01FFFFFFu)

/* BPRIO */

#define CSL_DDR2_BPRIO_PRIO_RAISE_MASK   (0x000000FFu)
#define CSL_DDR2_BPRIO_PRIO_RAISE_SHIFT  (0x00000000u)
#define CSL_DDR2_BPRIO_PRIO_RAISE_RESETVAL (0x000000FFu)

#define CSL_DDR2_BPRIO_RESETVAL          (0x000000FFu)

/* DMCCTL */

#define CSL_DDR2_DMCCTL_IFRESET_MASK     (0x00000020u)
#define CSL_DDR2_DMCCTL_IFRESET_SHIFT    (0x00000005u)
#define CSL_DDR2_DMCCTL_IFRESET_RESETVAL (0x00000001u)

/*----IFRESET Tokens----*/
#define CSL_DDR2_DMCCTL_IFRESET_RELEASE  (0x00000000u)
#define CSL_DDR2_DMCCTL_IFRESET_ASSERT   (0x00000001u)

#define CSL_DDR2_DMCCTL_RL_MASK          (0x00000007u)
#define CSL_DDR2_DMCCTL_RL_SHIFT         (0x00000000u)
#define CSL_DDR2_DMCCTL_RL_RESETVAL      (0x00000007u)

#define CSL_DDR2_DMCCTL_RESETVAL         (0x50006427u)

#endif
