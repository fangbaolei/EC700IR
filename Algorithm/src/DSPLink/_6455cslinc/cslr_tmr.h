/*  ============================================================================
 *   Copyright (c) Texas Instruments Inc 2002, 2003, 2004, 2005
 *
 *   Use of this software is controlled by the terms and conditions found in the
 *   license agreement under which this software has been supplied.
 *   ===========================================================================
 */
 
/** ============================================================================
 *   @file  cslr_tmr.h
 *
 *   @desc  API header file for TIMER CSL
 *
 */

#ifndef _CSLR_TMR_H_
#define _CSLR_TMR_H_

#include <cslr.h>
#include <tistdtypes.h>

/**************************************************************************\
* Register Overlay Structure
\**************************************************************************/
typedef struct  {
    volatile Uint8 RSVD0[4];
    volatile Uint32 EMUMGT_CLKSPD;
    volatile Uint8 RSVD1[8];
    volatile Uint32 TIMLO;
    volatile Uint32 TIMHI;
    volatile Uint32 PRDLO;
    volatile Uint32 PRDHI;
    volatile Uint32 TCR;
    volatile Uint32 TGCR;
    volatile Uint32 WDTCR;
} CSL_TmrRegs;


/**************************************************************************\
* Overlay structure typedef definition
\**************************************************************************/
typedef volatile CSL_TmrRegs             *CSL_TmrRegsOvly;


/**************************************************************************\
* Field Definition Macros
\**************************************************************************/

/* EMUMGT_CLKSPD */

#define CSL_TMR_EMUMGT_CLKSPD_CLKDIV_MASK (0x000F0000u)
#define CSL_TMR_EMUMGT_CLKSPD_CLKDIV_SHIFT (0x00000010u)
#define CSL_TMR_EMUMGT_CLKSPD_CLKDIV_RESETVAL (0x00000006u)

/*----CLKDIV Tokens----*/
#define CSL_TMR_EMUMGT_CLKSPD_CLKDIV_DIV0 (0x00000000u)
#define CSL_TMR_EMUMGT_CLKSPD_CLKDIV_DIV1 (0x00000001u)
#define CSL_TMR_EMUMGT_CLKSPD_CLKDIV_DIV2 (0x00000002u)
#define CSL_TMR_EMUMGT_CLKSPD_CLKDIV_DIV4 (0x00000004u)
#define CSL_TMR_EMUMGT_CLKSPD_CLKDIV_DIV8 (0x00000008u)

#define CSL_TMR_EMUMGT_CLKSPD_SOFT_MASK  (0x00000002u)
#define CSL_TMR_EMUMGT_CLKSPD_SOFT_SHIFT (0x00000001u)
#define CSL_TMR_EMUMGT_CLKSPD_SOFT_RESETVAL (0x00000000u)

#define CSL_TMR_EMUMGT_CLKSPD_FREE_MASK  (0x00000001u)
#define CSL_TMR_EMUMGT_CLKSPD_FREE_SHIFT (0x00000000u)
#define CSL_TMR_EMUMGT_CLKSPD_FREE_RESETVAL (0x00000000u)

#define CSL_TMR_EMUMGT_CLKSPD_RESETVAL   (0x00060000u)

/* TIMLO */

#define CSL_TMR_TIMLO_TIMLO_MASK         (0xFFFFFFFFu)
#define CSL_TMR_TIMLO_TIMLO_SHIFT        (0x00000000u)
#define CSL_TMR_TIMLO_TIMLO_RESETVAL     (0x00000000u)

#define CSL_TMR_TIMLO_RESETVAL           (0x00000000u)

/* TIMHI */

#define CSL_TMR_TIMHI_TIMHI_MASK         (0xFFFFFFFFu)
#define CSL_TMR_TIMHI_TIMHI_SHIFT        (0x00000000u)
#define CSL_TMR_TIMHI_TIMHI_RESETVAL     (0x00000000u)

#define CSL_TMR_TIMHI_RESETVAL           (0x00000000u)

/* PRDLO */

#define CSL_TMR_PRDLO_PRDLO_MASK         (0xFFFFFFFFu)
#define CSL_TMR_PRDLO_PRDLO_SHIFT        (0x00000000u)
#define CSL_TMR_PRDLO_PRDLO_RESETVAL     (0x00000000u)

#define CSL_TMR_PRDLO_RESETVAL           (0x00000000u)

/* PRDHI */

#define CSL_TMR_PRDHI_PRDHI_MASK         (0xFFFFFFFFu)
#define CSL_TMR_PRDHI_PRDHI_SHIFT        (0x00000000u)
#define CSL_TMR_PRDHI_PRDHI_RESETVAL     (0x00000000u)

#define CSL_TMR_PRDHI_RESETVAL           (0x00000000u)

/* TCR */

#define CSL_TMR_TCR_ENAMODE_HI_MASK      (0x00C00000u)
#define CSL_TMR_TCR_ENAMODE_HI_SHIFT     (0x00000016u)
#define CSL_TMR_TCR_ENAMODE_HI_RESETVAL  (0x00000000u)

/*----ENAMODE_HI Tokens----*/
#define CSL_TMR_TCR_ENAMODE_HI_DISABLE   (0x00000000u)

#define CSL_TMR_TCR_PWID_HI_MASK         (0x00300000u)
#define CSL_TMR_TCR_PWID_HI_SHIFT        (0x00000014u)
#define CSL_TMR_TCR_PWID_HI_RESETVAL     (0x00000000u)

#define CSL_TMR_TCR_CP_HI_MASK           (0x00080000u)
#define CSL_TMR_TCR_CP_HI_SHIFT          (0x00000013u)
#define CSL_TMR_TCR_CP_HI_RESETVAL       (0x00000000u)

#define CSL_TMR_TCR_INVOUTP_HI_MASK      (0x00020000u)
#define CSL_TMR_TCR_INVOUTP_HI_SHIFT     (0x00000011u)
#define CSL_TMR_TCR_INVOUTP_HI_RESETVAL  (0x00000000u)

#define CSL_TMR_TCR_TSTAT_HI_MASK        (0x00010000u)
#define CSL_TMR_TCR_TSTAT_HI_SHIFT       (0x00000010u)
#define CSL_TMR_TCR_TSTAT_HI_RESETVAL    (0x00000000u)

#define CSL_TMR_TCR_TIEN_LO_MASK         (0x00000200u)
#define CSL_TMR_TCR_TIEN_LO_SHIFT        (0x00000009u)
#define CSL_TMR_TCR_TIEN_LO_RESETVAL     (0x00000000u)

#define CSL_TMR_TCR_CLKSRC_LO_MASK       (0x00000100u)
#define CSL_TMR_TCR_CLKSRC_LO_SHIFT      (0x00000008u)
#define CSL_TMR_TCR_CLKSRC_LO_RESETVAL   (0x00000000u)

#define CSL_TMR_TCR_ENAMODE_LO_MASK      (0x000000C0u)
#define CSL_TMR_TCR_ENAMODE_LO_SHIFT     (0x00000006u)
#define CSL_TMR_TCR_ENAMODE_LO_RESETVAL  (0x00000000u)

/*----ENAMODE_LO Tokens----*/
#define CSL_TMR_TCR_ENAMODE_LO_DISABLE   (0x00000000u)

#define CSL_TMR_TCR_PWID_LO_MASK         (0x00000030u)
#define CSL_TMR_TCR_PWID_LO_SHIFT        (0x00000004u)
#define CSL_TMR_TCR_PWID_LO_RESETVAL     (0x00000000u)

#define CSL_TMR_TCR_CP_LO_MASK           (0x00000008u)
#define CSL_TMR_TCR_CP_LO_SHIFT          (0x00000003u)
#define CSL_TMR_TCR_CP_LO_RESETVAL       (0x00000000u)

#define CSL_TMR_TCR_INVINP_LO_MASK       (0x00000004u)
#define CSL_TMR_TCR_INVINP_LO_SHIFT      (0x00000002u)
#define CSL_TMR_TCR_INVINP_LO_RESETVAL   (0x00000000u)

#define CSL_TMR_TCR_INVOUTP_LO_MASK      (0x00000002u)
#define CSL_TMR_TCR_INVOUTP_LO_SHIFT     (0x00000001u)
#define CSL_TMR_TCR_INVOUTP_LO_RESETVAL  (0x00000000u)

#define CSL_TMR_TCR_TSTAT_LO_MASK        (0x00000001u)
#define CSL_TMR_TCR_TSTAT_LO_SHIFT       (0x00000000u)
#define CSL_TMR_TCR_TSTAT_LO_RESETVAL    (0x00000000u)

#define CSL_TMR_TCR_RESETVAL             (0x00000000u)

/* TGCR */

#define CSL_TMR_TGCR_TDDRHI_MASK         (0x0000F000u)
#define CSL_TMR_TGCR_TDDRHI_SHIFT        (0x0000000Cu)
#define CSL_TMR_TGCR_TDDRHI_RESETVAL     (0x00000000u)

#define CSL_TMR_TGCR_PSCHI_MASK          (0x00000F00u)
#define CSL_TMR_TGCR_PSCHI_SHIFT         (0x00000008u)
#define CSL_TMR_TGCR_PSCHI_RESETVAL      (0x00000000u)

#define CSL_TMR_TGCR_TIMMODE_MASK        (0x0000000Cu)
#define CSL_TMR_TGCR_TIMMODE_SHIFT       (0x00000002u)
#define CSL_TMR_TGCR_TIMMODE_RESETVAL    (0x00000000u)

#define CSL_TMR_TGCR_TIMHIRS_MASK        (0x00000002u)
#define CSL_TMR_TGCR_TIMHIRS_SHIFT       (0x00000001u)
#define CSL_TMR_TGCR_TIMHIRS_RESETVAL    (0x00000000u)

/*----TIMHIRS Tokens----*/
#define CSL_TMR_TGCR_TIMHIRS_RESET_ON    (0x00000000u)
#define CSL_TMR_TGCR_TIMHIRS_RESET_OFF   (0x00000001u)

#define CSL_TMR_TGCR_TIMLORS_MASK        (0x00000001u)
#define CSL_TMR_TGCR_TIMLORS_SHIFT       (0x00000000u)
#define CSL_TMR_TGCR_TIMLORS_RESETVAL    (0x00000000u)

/*----TIMLORS Tokens----*/
#define CSL_TMR_TGCR_TIMLORS_RESET_ON    (0x00000000u)
#define CSL_TMR_TGCR_TIMLORS_RESET_OFF   (0x00000001u)

#define CSL_TMR_TGCR_RESETVAL            (0x00000000u)

/* WDTCR */

#define CSL_TMR_WDTCR_WDKEY_MASK         (0xFFFF0000u)
#define CSL_TMR_WDTCR_WDKEY_SHIFT        (0x00000010u)
#define CSL_TMR_WDTCR_WDKEY_RESETVAL     (0x00000000u)

/*----WDKEY Tokens----*/
#define CSL_TMR_WDTCR_WDKEY_CMD1         (0x0000A5C6u)
#define CSL_TMR_WDTCR_WDKEY_CMD2         (0x0000DA7Eu)

#define CSL_TMR_WDTCR_WDFLAG_MASK        (0x00008000u)
#define CSL_TMR_WDTCR_WDFLAG_SHIFT       (0x0000000Fu)
#define CSL_TMR_WDTCR_WDFLAG_RESETVAL    (0x00000000u)

#define CSL_TMR_WDTCR_WDEN_MASK          (0x00004000u)
#define CSL_TMR_WDTCR_WDEN_SHIFT         (0x0000000Eu)
#define CSL_TMR_WDTCR_WDEN_RESETVAL      (0x00000000u)

/*----WDEN Tokens----*/
#define CSL_TMR_WDTCR_WDEN_DISABLE       (0x00000000u)
#define CSL_TMR_WDTCR_WDEN_ENABLE        (0x00000001u)

#define CSL_TMR_WDTCR_WDIKEY_MASK        (0x00003000u)
#define CSL_TMR_WDTCR_WDIKEY_SHIFT       (0x0000000Cu)
#define CSL_TMR_WDTCR_WDIKEY_RESETVAL    (0x00000000u)

#define CSL_TMR_WDTCR_RESETVAL           (0x00000000u)

#endif
