#ifndef _CSLR_AEMIF_1_H_
#define _CSLR_AEMIF_1_H_

#include <cslr.h>

#include "tistdtypes.h"


/* Minimum unit = 4 bytes */

/**************************************************************************\
* Register Overlay Structure
\**************************************************************************/
typedef struct  {
    volatile Uint32 RCSR;
    volatile Uint32 WAITCFG;
    volatile Uint32 RSVD0;
    volatile Uint32 RSVD1;
    volatile Uint32 ACFG2;
    volatile Uint32 ACFG3;
    volatile Uint32 ACFG4;
    volatile Uint32 ACFG5;
    volatile Uint32 RSVD2;
    volatile Uint32 RSVD3;
    volatile Uint32 RSVD4;
    volatile Uint32 RSVD5;
    volatile Uint32 RSVD6;
    volatile Uint32 RSVD7;
    volatile Uint32 RSVD8;
    volatile Uint32 RSVD9;
    volatile Uint32 INTRAW;
    volatile Uint32 INTMASK;
    volatile Uint32 INTMASKSET;
    volatile Uint32 INTMASKCLEAR;
    volatile Uint32 RSVD10;
    volatile Uint32 RSVD11;
    volatile Uint32 RSVD12;
    volatile Uint32 RSVD13;
    volatile Uint32 NANDCTL;
    volatile Uint32 NANDSTAT;
    volatile Uint32 RSVD14;
    volatile Uint32 RSVD15;
    volatile Uint32 NANDECC2;
    volatile Uint32 NANDECC3;
    volatile Uint32 NANDECC4;
    volatile Uint32 NANDECC5;
    volatile Uint32 RSVD16;
    volatile Uint32 IODFTECR;
    volatile Uint32 IODFTGCR;
    volatile Uint32 RSVD17;
    volatile Uint32 IODFTMRLR;
    volatile Uint32 IODFTMRMR;
    volatile Uint32 IODFTMRMSBR;
    volatile Uint32 RSVD18;
    volatile Uint32 RSVD19;
    volatile Uint32 RSVD20;
    volatile Uint32 RSVD21;
    volatile Uint32 RSVD22;
    volatile Uint32 MODRNR;
} CSL_AemifRegs;

/**************************************************************************\
* Field Definition Macros
\**************************************************************************/

/* RCSR */

#define CSL_AEMIF_RCSR_BE_MASK           (0x80000000u)
#define CSL_AEMIF_RCSR_BE_SHIFT          (0x0000001Fu)
#define CSL_AEMIF_RCSR_BE_RESETVAL       (0x00000000u)

#define CSL_AEMIF_RCSR_FR_MASK           (0x40000000u)
#define CSL_AEMIF_RCSR_FR_SHIFT          (0x0000001Eu)
#define CSL_AEMIF_RCSR_FR_RESETVAL       (0x00000000u)

#define CSL_AEMIF_RCSR_MODID_MASK        (0x3FFF0000u)
#define CSL_AEMIF_RCSR_MODID_SHIFT       (0x00000010u)
#define CSL_AEMIF_RCSR_MODID_RESETVAL    (0x0000000Fu)

#define CSL_AEMIF_RCSR_MAJREV_MASK       (0x0000FF00u)
#define CSL_AEMIF_RCSR_MAJREV_SHIFT      (0x00000008u)
#define CSL_AEMIF_RCSR_MAJREV_RESETVAL   (0x00000002u)

#define CSL_AEMIF_RCSR_MINREV_MASK       (0x000000FFu)
#define CSL_AEMIF_RCSR_MINREV_SHIFT      (0x00000000u)
#define CSL_AEMIF_RCSR_MINREV_RESETVAL   (0x00000002u)

#define CSL_AEMIF_RCSR_RESETVAL          (0x000F0202u)

/* WAITCFG */




#define CSL_AEMIF_WAITCFG_WP0_MASK       (0x10000000u)
#define CSL_AEMIF_WAITCFG_WP0_SHIFT      (0x0000001Cu)
#define CSL_AEMIF_WAITCFG_WP0_RESETVAL   (0x00000001u)


#define CSL_AEMIF_WAITCFG_CS3WAIT_MASK   (0x00C00000u)
#define CSL_AEMIF_WAITCFG_CS3WAIT_SHIFT  (0x00000016u)
#define CSL_AEMIF_WAITCFG_CS3WAIT_RESETVAL (0x00000000u)

#define CSL_AEMIF_WAITCFG_CS2WAIT_MASK   (0x00300000u)
#define CSL_AEMIF_WAITCFG_CS2WAIT_SHIFT  (0x00000014u)
#define CSL_AEMIF_WAITCFG_CS2WAIT_RESETVAL (0x00000000u)

#define CSL_AEMIF_WAITCFG_CS1WAIT_MASK   (0x000C0000u)
#define CSL_AEMIF_WAITCFG_CS1WAIT_SHIFT  (0x00000012u)
#define CSL_AEMIF_WAITCFG_CS1WAIT_RESETVAL (0x00000000u)

#define CSL_AEMIF_WAITCFG_CS0WAIT_MASK   (0x00030000u)
#define CSL_AEMIF_WAITCFG_CS0WAIT_SHIFT  (0x00000010u)
#define CSL_AEMIF_WAITCFG_CS0WAIT_RESETVAL (0x00000000u)


#define CSL_AEMIF_WAITCFG_MAXEXTWAIT_MASK (0x000000FFu)
#define CSL_AEMIF_WAITCFG_MAXEXTWAIT_SHIFT (0x00000000u)
#define CSL_AEMIF_WAITCFG_MAXEXTWAIT_RESETVAL (0x00000080u)

#define CSL_AEMIF_WAITCFG_RESETVAL       (0xF0000080u)

/* RSVD0 */


#define CSL_AEMIF_RSVD0_RESETVAL         (0x00000000u)

/* RSVD1 */


#define CSL_AEMIF_RSVD1_RESETVAL         (0x00000000u)

/* ACFG2 */

#define CSL_AEMIF_ACFG2_SS_MASK          (0x80000000u)
#define CSL_AEMIF_ACFG2_SS_SHIFT         (0x0000001Fu)
#define CSL_AEMIF_ACFG2_SS_RESETVAL      (0x00000000u)

#define CSL_AEMIF_ACFG2_EW_MASK          (0x40000000u)
#define CSL_AEMIF_ACFG2_EW_SHIFT         (0x0000001Eu)
#define CSL_AEMIF_ACFG2_EW_RESETVAL      (0x00000000u)

#define CSL_AEMIF_ACFG2_WSETUP_MASK      (0x3C000000u)
#define CSL_AEMIF_ACFG2_WSETUP_SHIFT     (0x0000001Au)
#define CSL_AEMIF_ACFG2_WSETUP_RESETVAL  (0x0000000Fu)

#define CSL_AEMIF_ACFG2_WSTROBE_MASK     (0x03F00000u)
#define CSL_AEMIF_ACFG2_WSTROBE_SHIFT    (0x00000014u)
#define CSL_AEMIF_ACFG2_WSTROBE_RESETVAL (0x0000003Fu)

#define CSL_AEMIF_ACFG2_WHOLD_MASK       (0x000E0000u)
#define CSL_AEMIF_ACFG2_WHOLD_SHIFT      (0x00000011u)
#define CSL_AEMIF_ACFG2_WHOLD_RESETVAL   (0x00000007u)

#define CSL_AEMIF_ACFG2_RSETUP_MASK      (0x0001E000u)
#define CSL_AEMIF_ACFG2_RSETUP_SHIFT     (0x0000000Du)
#define CSL_AEMIF_ACFG2_RSETUP_RESETVAL  (0x0000000Fu)

#define CSL_AEMIF_ACFG2_RSTROBE_MASK     (0x00001F80u)
#define CSL_AEMIF_ACFG2_RSTROBE_SHIFT    (0x00000007u)
#define CSL_AEMIF_ACFG2_RSTROBE_RESETVAL (0x0000003Fu)

#define CSL_AEMIF_ACFG2_RHOLD_MASK       (0x00000070u)
#define CSL_AEMIF_ACFG2_RHOLD_SHIFT      (0x00000004u)
#define CSL_AEMIF_ACFG2_RHOLD_RESETVAL   (0x00000007u)

#define CSL_AEMIF_ACFG2_TA_MASK          (0x0000000Cu)
#define CSL_AEMIF_ACFG2_TA_SHIFT         (0x00000002u)
#define CSL_AEMIF_ACFG2_TA_RESETVAL      (0x00000003u)

#define CSL_AEMIF_ACFG2_ASIZE_MASK       (0x00000003u)
#define CSL_AEMIF_ACFG2_ASIZE_SHIFT      (0x00000000u)
#define CSL_AEMIF_ACFG2_ASIZE_RESETVAL   (u)

#define CSL_AEMIF_ACFG2_RESETVAL         (0x3FFFFFFCu)

/* ACFG3 */

#define CSL_AEMIF_ACFG3_SS_MASK          (0x80000000u)
#define CSL_AEMIF_ACFG3_SS_SHIFT         (0x0000001Fu)
#define CSL_AEMIF_ACFG3_SS_RESETVAL      (0x00000000u)

#define CSL_AEMIF_ACFG3_EW_MASK          (0x40000000u)
#define CSL_AEMIF_ACFG3_EW_SHIFT         (0x0000001Eu)
#define CSL_AEMIF_ACFG3_EW_RESETVAL      (0x00000000u)

#define CSL_AEMIF_ACFG3_WSETUP_MASK      (0x3C000000u)
#define CSL_AEMIF_ACFG3_WSETUP_SHIFT     (0x0000001Au)
#define CSL_AEMIF_ACFG3_WSETUP_RESETVAL  (0x0000000Fu)

#define CSL_AEMIF_ACFG3_WSTROBE_MASK     (0x03F00000u)
#define CSL_AEMIF_ACFG3_WSTROBE_SHIFT    (0x00000014u)
#define CSL_AEMIF_ACFG3_WSTROBE_RESETVAL (0x0000003Fu)

#define CSL_AEMIF_ACFG3_WHOLD_MASK       (0x000E0000u)
#define CSL_AEMIF_ACFG3_WHOLD_SHIFT      (0x00000011u)
#define CSL_AEMIF_ACFG3_WHOLD_RESETVAL   (0x00000007u)

#define CSL_AEMIF_ACFG3_RSETUP_MASK      (0x0001E000u)
#define CSL_AEMIF_ACFG3_RSETUP_SHIFT     (0x0000000Du)
#define CSL_AEMIF_ACFG3_RSETUP_RESETVAL  (0x0000000Fu)

#define CSL_AEMIF_ACFG3_RSTROBE_MASK     (0x00001F80u)
#define CSL_AEMIF_ACFG3_RSTROBE_SHIFT    (0x00000007u)
#define CSL_AEMIF_ACFG3_RSTROBE_RESETVAL (0x0000003Fu)

#define CSL_AEMIF_ACFG3_RHOLD_MASK       (0x00000070u)
#define CSL_AEMIF_ACFG3_RHOLD_SHIFT      (0x00000004u)
#define CSL_AEMIF_ACFG3_RHOLD_RESETVAL   (0x00000007u)

#define CSL_AEMIF_ACFG3_TA_MASK          (0x0000000Cu)
#define CSL_AEMIF_ACFG3_TA_SHIFT         (0x00000002u)
#define CSL_AEMIF_ACFG3_TA_RESETVAL      (0x00000003u)

#define CSL_AEMIF_ACFG3_ASIZE_MASK       (0x00000003u)
#define CSL_AEMIF_ACFG3_ASIZE_SHIFT      (0x00000000u)
#define CSL_AEMIF_ACFG3_ASIZE_RESETVAL   (0x00000000u)

#define CSL_AEMIF_ACFG3_RESETVAL         (0x3FFFFFFCu)

/* ACFG4 */

#define CSL_AEMIF_ACFG4_SS_MASK          (0x80000000u)
#define CSL_AEMIF_ACFG4_SS_SHIFT         (0x0000001Fu)
#define CSL_AEMIF_ACFG4_SS_RESETVAL      (0x00000000u)

#define CSL_AEMIF_ACFG4_EW_MASK          (0x40000000u)
#define CSL_AEMIF_ACFG4_EW_SHIFT         (0x0000001Eu)
#define CSL_AEMIF_ACFG4_EW_RESETVAL      (0x00000000u)

#define CSL_AEMIF_ACFG4_WSETUP_MASK      (0x3C000000u)
#define CSL_AEMIF_ACFG4_WSETUP_SHIFT     (0x0000001Au)
#define CSL_AEMIF_ACFG4_WSETUP_RESETVAL  (0x0000000Fu)

#define CSL_AEMIF_ACFG4_WSTROBE_MASK     (0x03F00000u)
#define CSL_AEMIF_ACFG4_WSTROBE_SHIFT    (0x00000014u)
#define CSL_AEMIF_ACFG4_WSTROBE_RESETVAL (0x0000003Fu)

#define CSL_AEMIF_ACFG4_WHOLD_MASK       (0x000E0000u)
#define CSL_AEMIF_ACFG4_WHOLD_SHIFT      (0x00000011u)
#define CSL_AEMIF_ACFG4_WHOLD_RESETVAL   (0x00000007u)

#define CSL_AEMIF_ACFG4_RSETUP_MASK      (0x0001E000u)
#define CSL_AEMIF_ACFG4_RSETUP_SHIFT     (0x0000000Du)
#define CSL_AEMIF_ACFG4_RSETUP_RESETVAL  (0x0000000Fu)

#define CSL_AEMIF_ACFG4_RSTROBE_MASK     (0x00001F80u)
#define CSL_AEMIF_ACFG4_RSTROBE_SHIFT    (0x00000007u)
#define CSL_AEMIF_ACFG4_RSTROBE_RESETVAL (0x0000003Fu)

#define CSL_AEMIF_ACFG4_RHOLD_MASK       (0x00000070u)
#define CSL_AEMIF_ACFG4_RHOLD_SHIFT      (0x00000004u)
#define CSL_AEMIF_ACFG4_RHOLD_RESETVAL   (0x00000007u)

#define CSL_AEMIF_ACFG4_TA_MASK          (0x0000000Cu)
#define CSL_AEMIF_ACFG4_TA_SHIFT         (0x00000002u)
#define CSL_AEMIF_ACFG4_TA_RESETVAL      (0x00000003u)

#define CSL_AEMIF_ACFG4_ASIZE_MASK       (0x00000003u)
#define CSL_AEMIF_ACFG4_ASIZE_SHIFT      (0x00000000u)
#define CSL_AEMIF_ACFG4_ASIZE_RESETVAL   (0x00000000u)

#define CSL_AEMIF_ACFG4_RESETVAL         (0x3FFFFFFCu)

/* ACFG5 */

#define CSL_AEMIF_ACFG5_SS_MASK          (0x80000000u)
#define CSL_AEMIF_ACFG5_SS_SHIFT         (0x0000001Fu)
#define CSL_AEMIF_ACFG5_SS_RESETVAL      (0x00000000u)

#define CSL_AEMIF_ACFG5_EW_MASK          (0x40000000u)
#define CSL_AEMIF_ACFG5_EW_SHIFT         (0x0000001Eu)
#define CSL_AEMIF_ACFG5_EW_RESETVAL      (0x00000000u)

#define CSL_AEMIF_ACFG5_WSETUP_MASK      (0x3C000000u)
#define CSL_AEMIF_ACFG5_WSETUP_SHIFT     (0x0000001Au)
#define CSL_AEMIF_ACFG5_WSETUP_RESETVAL  (0x0000000Fu)

#define CSL_AEMIF_ACFG5_WSTROBE_MASK     (0x03F00000u)
#define CSL_AEMIF_ACFG5_WSTROBE_SHIFT    (0x00000014u)
#define CSL_AEMIF_ACFG5_WSTROBE_RESETVAL (0x0000003Fu)

#define CSL_AEMIF_ACFG5_WHOLD_MASK       (0x000E0000u)
#define CSL_AEMIF_ACFG5_WHOLD_SHIFT      (0x00000011u)
#define CSL_AEMIF_ACFG5_WHOLD_RESETVAL   (0x00000007u)

#define CSL_AEMIF_ACFG5_RSETUP_MASK      (0x0001E000u)
#define CSL_AEMIF_ACFG5_RSETUP_SHIFT     (0x0000000Du)
#define CSL_AEMIF_ACFG5_RSETUP_RESETVAL  (0x0000000Fu)

#define CSL_AEMIF_ACFG5_RSTROBE_MASK     (0x00001F80u)
#define CSL_AEMIF_ACFG5_RSTROBE_SHIFT    (0x00000007u)
#define CSL_AEMIF_ACFG5_RSTROBE_RESETVAL (0x0000003Fu)

#define CSL_AEMIF_ACFG5_RHOLD_MASK       (0x00000070u)
#define CSL_AEMIF_ACFG5_RHOLD_SHIFT      (0x00000004u)
#define CSL_AEMIF_ACFG5_RHOLD_RESETVAL   (0x00000007u)

#define CSL_AEMIF_ACFG5_TA_MASK          (0x0000000Cu)
#define CSL_AEMIF_ACFG5_TA_SHIFT         (0x00000002u)
#define CSL_AEMIF_ACFG5_TA_RESETVAL      (0x00000003u)

#define CSL_AEMIF_ACFG5_ASIZE_MASK       (0x00000003u)
#define CSL_AEMIF_ACFG5_ASIZE_SHIFT      (0x00000000u)
#define CSL_AEMIF_ACFG5_ASIZE_RESETVAL   (0x00000000u)

#define CSL_AEMIF_ACFG5_RESETVAL         (0x3FFFFFFCu)

/* RSVD2 */


#define CSL_AEMIF_RSVD2_RESETVAL         (0x00000000u)

/* RSVD3 */


#define CSL_AEMIF_RSVD3_RESETVAL         (0x00000000u)

/* RSVD4 */


#define CSL_AEMIF_RSVD4_RESETVAL         (0x00000000u)

/* RSVD5 */


#define CSL_AEMIF_RSVD5_RESETVAL         (0x00000000u)

/* RSVD6 */


#define CSL_AEMIF_RSVD6_RESETVAL         (0x00000000u)

/* RSVD7 */


#define CSL_AEMIF_RSVD7_RESETVAL         (0x00000000u)

/* RSVD8 */


#define CSL_AEMIF_RSVD8_RESETVAL         (0x00000000u)

/* RSVD9 */


#define CSL_AEMIF_RSVD9_RESETVAL         (0x00000000u)

/* INTRAW */


#define CSL_AEMIF_INTRAW_WR_MASK         (0x0000003Cu)
#define CSL_AEMIF_INTRAW_WR_SHIFT        (0x00000002u)
#define CSL_AEMIF_INTRAW_WR_RESETVAL     (0x00000000u)

#define CSL_AEMIF_INTRAW_LT_MASK         (0x00000002u)
#define CSL_AEMIF_INTRAW_LT_SHIFT        (0x00000001u)
#define CSL_AEMIF_INTRAW_LT_RESETVAL     (0x00000000u)

#define CSL_AEMIF_INTRAW_ATO_MASK        (0x00000001u)
#define CSL_AEMIF_INTRAW_ATO_SHIFT       (0x00000000u)
#define CSL_AEMIF_INTRAW_ATO_RESETVAL    (0x00000000u)

#define CSL_AEMIF_INTRAW_RESETVAL        (0x00000000u)

/* INTMASK */


#define CSL_AEMIF_INTMASK_WRMASK_MASK    (0x0000003Cu)
#define CSL_AEMIF_INTMASK_WRMASK_SHIFT   (0x00000002u)
#define CSL_AEMIF_INTMASK_WRMASK_RESETVAL (0x00000000u)

#define CSL_AEMIF_INTMASK_LTMASK_MASK    (0x00000002u)
#define CSL_AEMIF_INTMASK_LTMASK_SHIFT   (0x00000001u)
#define CSL_AEMIF_INTMASK_LTMASK_RESETVAL (0x00000000u)

#define CSL_AEMIF_INTMASK_ATOMASK_MASK   (0x00000001u)
#define CSL_AEMIF_INTMASK_ATOMASK_SHIFT  (0x00000000u)
#define CSL_AEMIF_INTMASK_ATOMASK_RESETVAL (0x00000000u)

#define CSL_AEMIF_INTMASK_RESETVAL       (0x00000000u)

/* INTMASKSET */


#define CSL_AEMIF_INTMASKSET_WRMASKSET_MASK (0x0000003Cu)
#define CSL_AEMIF_INTMASKSET_WRMASKSET_SHIFT (0x00000002u)
#define CSL_AEMIF_INTMASKSET_WRMASKSET_RESETVAL (0x00000000u)

#define CSL_AEMIF_INTMASKSET_LTMASKSET_MASK (0x00000002u)
#define CSL_AEMIF_INTMASKSET_LTMASKSET_SHIFT (0x00000001u)
#define CSL_AEMIF_INTMASKSET_LTMASKSET_RESETVAL (0x00000000u)

#define CSL_AEMIF_INTMASKSET_ATOMASKSET_MASK (0x00000001u)
#define CSL_AEMIF_INTMASKSET_ATOMASKSET_SHIFT (0x00000000u)
#define CSL_AEMIF_INTMASKSET_ATOMASKSET_RESETVAL (0x00000000u)

#define CSL_AEMIF_INTMASKSET_RESETVAL    (0x00000000u)

/* INTMASKCLEAR */


#define CSL_AEMIF_INTMASKCLEAR_WRMASKCLR_MASK (0x0000003Cu)
#define CSL_AEMIF_INTMASKCLEAR_WRMASKCLR_SHIFT (0x00000002u)
#define CSL_AEMIF_INTMASKCLEAR_WRMASKCLR_RESETVAL (0x00000000u)

#define CSL_AEMIF_INTMASKCLEAR_LTMASKCLR_MASK (0x00000002u)
#define CSL_AEMIF_INTMASKCLEAR_LTMASKCLR_SHIFT (0x00000001u)
#define CSL_AEMIF_INTMASKCLEAR_LTMASKCLR_RESETVAL (0x00000000u)

#define CSL_AEMIF_INTMASKCLEAR_ATOMASKCLR_MASK (0x00000001u)
#define CSL_AEMIF_INTMASKCLEAR_ATOMASKCLR_SHIFT (0x00000000u)
#define CSL_AEMIF_INTMASKCLEAR_ATOMASKCLR_RESETVAL (0x00000000u)

#define CSL_AEMIF_INTMASKCLEAR_RESETVAL  (0x00000000u)

/* RSVD10 */


#define CSL_AEMIF_RSVD10_RESETVAL        (0x00000000u)

/* RSVD11 */


#define CSL_AEMIF_RSVD11_RESETVAL        (0x00000000u)

/* RSVD12 */


#define CSL_AEMIF_RSVD12_RESETVAL        (0x00000000u)

/* RSVD13 */


#define CSL_AEMIF_RSVD13_RESETVAL        (0x00000000u)

/* NANDCTL */


#define CSL_AEMIF_NANDCTL_ECCSTART5_MASK (0x00000800u)
#define CSL_AEMIF_NANDCTL_ECCSTART5_SHIFT (0x0000000Bu)
#define CSL_AEMIF_NANDCTL_ECCSTART5_RESETVAL (0x00000000u)

#define CSL_AEMIF_NANDCTL_ECCSTART4_MASK (0x00000400u)
#define CSL_AEMIF_NANDCTL_ECCSTART4_SHIFT (0x0000000Au)
#define CSL_AEMIF_NANDCTL_ECCSTART4_RESETVAL (0x00000000u)

#define CSL_AEMIF_NANDCTL_ECCSTART3_MASK (0x00000200u)
#define CSL_AEMIF_NANDCTL_ECCSTART3_SHIFT (0x00000009u)
#define CSL_AEMIF_NANDCTL_ECCSTART3_RESETVAL (0x00000000u)

#define CSL_AEMIF_NANDCTL_ECCSTART2_MASK (0x00000100u)
#define CSL_AEMIF_NANDCTL_ECCSTART2_SHIFT (0x00000008u)
#define CSL_AEMIF_NANDCTL_ECCSTART2_RESETVAL (0x00000000u)


#define CSL_AEMIF_NANDCTL_USENAND5_MASK  (0x00000008u)
#define CSL_AEMIF_NANDCTL_USENAND5_SHIFT (0x00000003u)
#define CSL_AEMIF_NANDCTL_USENAND5_RESETVAL (0x00000000u)

#define CSL_AEMIF_NANDCTL_USENAND4_MASK  (0x00000004u)
#define CSL_AEMIF_NANDCTL_USENAND4_SHIFT (0x00000002u)
#define CSL_AEMIF_NANDCTL_USENAND4_RESETVAL (0x00000000u)

#define CSL_AEMIF_NANDCTL_USENAND3_MASK  (0x00000002u)
#define CSL_AEMIF_NANDCTL_USENAND3_SHIFT (0x00000001u)
#define CSL_AEMIF_NANDCTL_USENAND3_RESETVAL (0x00000000u)

#define CSL_AEMIF_NANDCTL_USENAND2_MASK  (0x00000001u)
#define CSL_AEMIF_NANDCTL_USENAND2_SHIFT (0x00000000u)
#define CSL_AEMIF_NANDCTL_USENAND2_RESETVAL (u)

#define CSL_AEMIF_NANDCTL_RESETVAL       (0x00000000u)

/* NANDSTAT */


#define CSL_AEMIF_NANDSTAT_WAITSTAT_MASK (0x0000000Fu)
#define CSL_AEMIF_NANDSTAT_WAITSTAT_SHIFT (0x00000000u)
#define CSL_AEMIF_NANDSTAT_WAITSTAT_RESETVAL (u)

#define CSL_AEMIF_NANDSTAT_RESETVAL      (0x00000000u)

/* RSVD14 */


#define CSL_AEMIF_RSVD14_RESETVAL        (0x00000000u)

/* RSVD15 */


#define CSL_AEMIF_RSVD15_RESETVAL        (0x00000000u)

/* NANDECC2 */


#define CSL_AEMIF_NANDECC2_P2048O_MASK   (0x08000000u)
#define CSL_AEMIF_NANDECC2_P2048O_SHIFT  (0x0000001Bu)
#define CSL_AEMIF_NANDECC2_P2048O_RESETVAL (0x00000000u)

#define CSL_AEMIF_NANDECC2_P1024O_MASK   (0x04000000u)
#define CSL_AEMIF_NANDECC2_P1024O_SHIFT  (0x0000001Au)
#define CSL_AEMIF_NANDECC2_P1024O_RESETVAL (0x00000000u)

#define CSL_AEMIF_NANDECC2_P512O_MASK    (0x02000000u)
#define CSL_AEMIF_NANDECC2_P512O_SHIFT   (0x00000019u)
#define CSL_AEMIF_NANDECC2_P512O_RESETVAL (0x00000000u)

#define CSL_AEMIF_NANDECC2_P256O_MASK    (0x01000000u)
#define CSL_AEMIF_NANDECC2_P256O_SHIFT   (0x00000018u)
#define CSL_AEMIF_NANDECC2_P256O_RESETVAL (0x00000000u)

#define CSL_AEMIF_NANDECC2_P128O_MASK    (0x00800000u)
#define CSL_AEMIF_NANDECC2_P128O_SHIFT   (0x00000017u)
#define CSL_AEMIF_NANDECC2_P128O_RESETVAL (0x00000000u)

#define CSL_AEMIF_NANDECC2_P64O_MASK     (0x00400000u)
#define CSL_AEMIF_NANDECC2_P64O_SHIFT    (0x00000016u)
#define CSL_AEMIF_NANDECC2_P64O_RESETVAL (0x00000000u)

#define CSL_AEMIF_NANDECC2_P32O_MASK     (0x00200000u)
#define CSL_AEMIF_NANDECC2_P32O_SHIFT    (0x00000015u)
#define CSL_AEMIF_NANDECC2_P32O_RESETVAL (0x00000000u)

#define CSL_AEMIF_NANDECC2_P16O_MASK     (0x00100000u)
#define CSL_AEMIF_NANDECC2_P16O_SHIFT    (0x00000014u)
#define CSL_AEMIF_NANDECC2_P16O_RESETVAL (0x00000000u)

#define CSL_AEMIF_NANDECC2_P8O_MASK      (0x00080000u)
#define CSL_AEMIF_NANDECC2_P8O_SHIFT     (0x00000013u)
#define CSL_AEMIF_NANDECC2_P8O_RESETVAL  (0x00000000u)

#define CSL_AEMIF_NANDECC2_P4O_MASK      (0x00040000u)
#define CSL_AEMIF_NANDECC2_P4O_SHIFT     (0x00000012u)
#define CSL_AEMIF_NANDECC2_P4O_RESETVAL  (0x00000000u)

#define CSL_AEMIF_NANDECC2_P2O_MASK      (0x00020000u)
#define CSL_AEMIF_NANDECC2_P2O_SHIFT     (0x00000011u)
#define CSL_AEMIF_NANDECC2_P2O_RESETVAL  (0x00000000u)

#define CSL_AEMIF_NANDECC2_P1O_MASK      (0x00010000u)
#define CSL_AEMIF_NANDECC2_P1O_SHIFT     (0x00000010u)
#define CSL_AEMIF_NANDECC2_P1O_RESETVAL  (0x00000000u)


#define CSL_AEMIF_NANDECC2_P2048E_MASK   (0x00000800u)
#define CSL_AEMIF_NANDECC2_P2048E_SHIFT  (0x0000000Bu)
#define CSL_AEMIF_NANDECC2_P2048E_RESETVAL (0x00000000u)

#define CSL_AEMIF_NANDECC2_P1024E_MASK   (0x00000400u)
#define CSL_AEMIF_NANDECC2_P1024E_SHIFT  (0x0000000Au)
#define CSL_AEMIF_NANDECC2_P1024E_RESETVAL (0x00000000u)

#define CSL_AEMIF_NANDECC2_P512E_MASK    (0x00000200u)
#define CSL_AEMIF_NANDECC2_P512E_SHIFT   (0x00000009u)
#define CSL_AEMIF_NANDECC2_P512E_RESETVAL (0x00000000u)

#define CSL_AEMIF_NANDECC2_P256E_MASK    (0x00000100u)
#define CSL_AEMIF_NANDECC2_P256E_SHIFT   (0x00000008u)
#define CSL_AEMIF_NANDECC2_P256E_RESETVAL (0x00000000u)

#define CSL_AEMIF_NANDECC2_P128E_MASK    (0x00000080u)
#define CSL_AEMIF_NANDECC2_P128E_SHIFT   (0x00000007u)
#define CSL_AEMIF_NANDECC2_P128E_RESETVAL (0x00000000u)

#define CSL_AEMIF_NANDECC2_P64E_MASK     (0x00000040u)
#define CSL_AEMIF_NANDECC2_P64E_SHIFT    (0x00000006u)
#define CSL_AEMIF_NANDECC2_P64E_RESETVAL (0x00000000u)

#define CSL_AEMIF_NANDECC2_P32E_MASK     (0x00000020u)
#define CSL_AEMIF_NANDECC2_P32E_SHIFT    (0x00000005u)
#define CSL_AEMIF_NANDECC2_P32E_RESETVAL (0x00000000u)

#define CSL_AEMIF_NANDECC2_P16E_MASK     (0x00000010u)
#define CSL_AEMIF_NANDECC2_P16E_SHIFT    (0x00000004u)
#define CSL_AEMIF_NANDECC2_P16E_RESETVAL (0x00000000u)

#define CSL_AEMIF_NANDECC2_P8E_MASK      (0x00000008u)
#define CSL_AEMIF_NANDECC2_P8E_SHIFT     (0x00000003u)
#define CSL_AEMIF_NANDECC2_P8E_RESETVAL  (0x00000000u)

#define CSL_AEMIF_NANDECC2_P4E_MASK      (0x00000004u)
#define CSL_AEMIF_NANDECC2_P4E_SHIFT     (0x00000002u)
#define CSL_AEMIF_NANDECC2_P4E_RESETVAL  (0x00000000u)

#define CSL_AEMIF_NANDECC2_P2E_MASK      (0x00000002u)
#define CSL_AEMIF_NANDECC2_P2E_SHIFT     (0x00000001u)
#define CSL_AEMIF_NANDECC2_P2E_RESETVAL  (0x00000000u)

#define CSL_AEMIF_NANDECC2_P1E_MASK      (0x00000001u)
#define CSL_AEMIF_NANDECC2_P1E_SHIFT     (0x00000000u)
#define CSL_AEMIF_NANDECC2_P1E_RESETVAL  (0x00000000u)

#define CSL_AEMIF_NANDECC2_RESETVAL      (0x00000000u)

/* NANDECC3 */


#define CSL_AEMIF_NANDECC3_P2048O_MASK   (0x08000000u)
#define CSL_AEMIF_NANDECC3_P2048O_SHIFT  (0x0000001Bu)
#define CSL_AEMIF_NANDECC3_P2048O_RESETVAL (0x00000000u)

#define CSL_AEMIF_NANDECC3_P1024O_MASK   (0x04000000u)
#define CSL_AEMIF_NANDECC3_P1024O_SHIFT  (0x0000001Au)
#define CSL_AEMIF_NANDECC3_P1024O_RESETVAL (0x00000000u)

#define CSL_AEMIF_NANDECC3_P512O_MASK    (0x02000000u)
#define CSL_AEMIF_NANDECC3_P512O_SHIFT   (0x00000019u)
#define CSL_AEMIF_NANDECC3_P512O_RESETVAL (0x00000000u)

#define CSL_AEMIF_NANDECC3_P256O_MASK    (0x01000000u)
#define CSL_AEMIF_NANDECC3_P256O_SHIFT   (0x00000018u)
#define CSL_AEMIF_NANDECC3_P256O_RESETVAL (0x00000000u)

#define CSL_AEMIF_NANDECC3_P128O_MASK    (0x00800000u)
#define CSL_AEMIF_NANDECC3_P128O_SHIFT   (0x00000017u)
#define CSL_AEMIF_NANDECC3_P128O_RESETVAL (0x00000000u)

#define CSL_AEMIF_NANDECC3_P64O_MASK     (0x00400000u)
#define CSL_AEMIF_NANDECC3_P64O_SHIFT    (0x00000016u)
#define CSL_AEMIF_NANDECC3_P64O_RESETVAL (0x00000000u)

#define CSL_AEMIF_NANDECC3_P32O_MASK     (0x00200000u)
#define CSL_AEMIF_NANDECC3_P32O_SHIFT    (0x00000015u)
#define CSL_AEMIF_NANDECC3_P32O_RESETVAL (0x00000000u)

#define CSL_AEMIF_NANDECC3_P16O_MASK     (0x00100000u)
#define CSL_AEMIF_NANDECC3_P16O_SHIFT    (0x00000014u)
#define CSL_AEMIF_NANDECC3_P16O_RESETVAL (0x00000000u)

#define CSL_AEMIF_NANDECC3_P8O_MASK      (0x00080000u)
#define CSL_AEMIF_NANDECC3_P8O_SHIFT     (0x00000013u)
#define CSL_AEMIF_NANDECC3_P8O_RESETVAL  (0x00000000u)

#define CSL_AEMIF_NANDECC3_P4O_MASK      (0x00040000u)
#define CSL_AEMIF_NANDECC3_P4O_SHIFT     (0x00000012u)
#define CSL_AEMIF_NANDECC3_P4O_RESETVAL  (0x00000000u)

#define CSL_AEMIF_NANDECC3_P2O_MASK      (0x00020000u)
#define CSL_AEMIF_NANDECC3_P2O_SHIFT     (0x00000011u)
#define CSL_AEMIF_NANDECC3_P2O_RESETVAL  (0x00000000u)

#define CSL_AEMIF_NANDECC3_P1O_MASK      (0x00010000u)
#define CSL_AEMIF_NANDECC3_P1O_SHIFT     (0x00000010u)
#define CSL_AEMIF_NANDECC3_P1O_RESETVAL  (0x00000000u)


#define CSL_AEMIF_NANDECC3_P2048E_MASK   (0x00000800u)
#define CSL_AEMIF_NANDECC3_P2048E_SHIFT  (0x0000000Bu)
#define CSL_AEMIF_NANDECC3_P2048E_RESETVAL (0x00000000u)

#define CSL_AEMIF_NANDECC3_P1024E_MASK   (0x00000400u)
#define CSL_AEMIF_NANDECC3_P1024E_SHIFT  (0x0000000Au)
#define CSL_AEMIF_NANDECC3_P1024E_RESETVAL (0x00000000u)

#define CSL_AEMIF_NANDECC3_P512E_MASK    (0x00000200u)
#define CSL_AEMIF_NANDECC3_P512E_SHIFT   (0x00000009u)
#define CSL_AEMIF_NANDECC3_P512E_RESETVAL (0x00000000u)

#define CSL_AEMIF_NANDECC3_P256E_MASK    (0x00000100u)
#define CSL_AEMIF_NANDECC3_P256E_SHIFT   (0x00000008u)
#define CSL_AEMIF_NANDECC3_P256E_RESETVAL (0x00000000u)

#define CSL_AEMIF_NANDECC3_P128E_MASK    (0x00000080u)
#define CSL_AEMIF_NANDECC3_P128E_SHIFT   (0x00000007u)
#define CSL_AEMIF_NANDECC3_P128E_RESETVAL (0x00000000u)

#define CSL_AEMIF_NANDECC3_P64E_MASK     (0x00000040u)
#define CSL_AEMIF_NANDECC3_P64E_SHIFT    (0x00000006u)
#define CSL_AEMIF_NANDECC3_P64E_RESETVAL (0x00000000u)

#define CSL_AEMIF_NANDECC3_P32E_MASK     (0x00000020u)
#define CSL_AEMIF_NANDECC3_P32E_SHIFT    (0x00000005u)
#define CSL_AEMIF_NANDECC3_P32E_RESETVAL (0x00000000u)

#define CSL_AEMIF_NANDECC3_P16E_MASK     (0x00000010u)
#define CSL_AEMIF_NANDECC3_P16E_SHIFT    (0x00000004u)
#define CSL_AEMIF_NANDECC3_P16E_RESETVAL (0x00000000u)

#define CSL_AEMIF_NANDECC3_P8E_MASK      (0x00000008u)
#define CSL_AEMIF_NANDECC3_P8E_SHIFT     (0x00000003u)
#define CSL_AEMIF_NANDECC3_P8E_RESETVAL  (0x00000000u)

#define CSL_AEMIF_NANDECC3_P4E_MASK      (0x00000004u)
#define CSL_AEMIF_NANDECC3_P4E_SHIFT     (0x00000002u)
#define CSL_AEMIF_NANDECC3_P4E_RESETVAL  (0x00000000u)

#define CSL_AEMIF_NANDECC3_P2E_MASK      (0x00000002u)
#define CSL_AEMIF_NANDECC3_P2E_SHIFT     (0x00000001u)
#define CSL_AEMIF_NANDECC3_P2E_RESETVAL  (0x00000000u)

#define CSL_AEMIF_NANDECC3_P1E_MASK      (0x00000001u)
#define CSL_AEMIF_NANDECC3_P1E_SHIFT     (0x00000000u)
#define CSL_AEMIF_NANDECC3_P1E_RESETVAL  (0x00000000u)

#define CSL_AEMIF_NANDECC3_RESETVAL      (0x00000000u)

/* NANDECC4 */


#define CSL_AEMIF_NANDECC4_P2048O_MASK   (0x08000000u)
#define CSL_AEMIF_NANDECC4_P2048O_SHIFT  (0x0000001Bu)
#define CSL_AEMIF_NANDECC4_P2048O_RESETVAL (0x00000000u)

#define CSL_AEMIF_NANDECC4_P1024O_MASK   (0x04000000u)
#define CSL_AEMIF_NANDECC4_P1024O_SHIFT  (0x0000001Au)
#define CSL_AEMIF_NANDECC4_P1024O_RESETVAL (0x00000000u)

#define CSL_AEMIF_NANDECC4_P512O_MASK    (0x02000000u)
#define CSL_AEMIF_NANDECC4_P512O_SHIFT   (0x00000019u)
#define CSL_AEMIF_NANDECC4_P512O_RESETVAL (0x00000000u)

#define CSL_AEMIF_NANDECC4_P256O_MASK    (0x01000000u)
#define CSL_AEMIF_NANDECC4_P256O_SHIFT   (0x00000018u)
#define CSL_AEMIF_NANDECC4_P256O_RESETVAL (0x00000000u)

#define CSL_AEMIF_NANDECC4_P128O_MASK    (0x00800000u)
#define CSL_AEMIF_NANDECC4_P128O_SHIFT   (0x00000017u)
#define CSL_AEMIF_NANDECC4_P128O_RESETVAL (0x00000000u)

#define CSL_AEMIF_NANDECC4_P64O_MASK     (0x00400000u)
#define CSL_AEMIF_NANDECC4_P64O_SHIFT    (0x00000016u)
#define CSL_AEMIF_NANDECC4_P64O_RESETVAL (0x00000000u)

#define CSL_AEMIF_NANDECC4_P32O_MASK     (0x00200000u)
#define CSL_AEMIF_NANDECC4_P32O_SHIFT    (0x00000015u)
#define CSL_AEMIF_NANDECC4_P32O_RESETVAL (0x00000000u)

#define CSL_AEMIF_NANDECC4_P16O_MASK     (0x00100000u)
#define CSL_AEMIF_NANDECC4_P16O_SHIFT    (0x00000014u)
#define CSL_AEMIF_NANDECC4_P16O_RESETVAL (0x00000000u)

#define CSL_AEMIF_NANDECC4_P8O_MASK      (0x00080000u)
#define CSL_AEMIF_NANDECC4_P8O_SHIFT     (0x00000013u)
#define CSL_AEMIF_NANDECC4_P8O_RESETVAL  (0x00000000u)

#define CSL_AEMIF_NANDECC4_P4O_MASK      (0x00040000u)
#define CSL_AEMIF_NANDECC4_P4O_SHIFT     (0x00000012u)
#define CSL_AEMIF_NANDECC4_P4O_RESETVAL  (0x00000000u)

#define CSL_AEMIF_NANDECC4_P2O_MASK      (0x00020000u)
#define CSL_AEMIF_NANDECC4_P2O_SHIFT     (0x00000011u)
#define CSL_AEMIF_NANDECC4_P2O_RESETVAL  (0x00000000u)

#define CSL_AEMIF_NANDECC4_P1O_MASK      (0x00010000u)
#define CSL_AEMIF_NANDECC4_P1O_SHIFT     (0x00000010u)
#define CSL_AEMIF_NANDECC4_P1O_RESETVAL  (0x00000000u)


#define CSL_AEMIF_NANDECC4_P2048E_MASK   (0x00000800u)
#define CSL_AEMIF_NANDECC4_P2048E_SHIFT  (0x0000000Bu)
#define CSL_AEMIF_NANDECC4_P2048E_RESETVAL (0x00000000u)

#define CSL_AEMIF_NANDECC4_P1024E_MASK   (0x00000400u)
#define CSL_AEMIF_NANDECC4_P1024E_SHIFT  (0x0000000Au)
#define CSL_AEMIF_NANDECC4_P1024E_RESETVAL (0x00000000u)

#define CSL_AEMIF_NANDECC4_P512E_MASK    (0x00000200u)
#define CSL_AEMIF_NANDECC4_P512E_SHIFT   (0x00000009u)
#define CSL_AEMIF_NANDECC4_P512E_RESETVAL (0x00000000u)

#define CSL_AEMIF_NANDECC4_P256E_MASK    (0x00000100u)
#define CSL_AEMIF_NANDECC4_P256E_SHIFT   (0x00000008u)
#define CSL_AEMIF_NANDECC4_P256E_RESETVAL (0x00000000u)

#define CSL_AEMIF_NANDECC4_P128E_MASK    (0x00000080u)
#define CSL_AEMIF_NANDECC4_P128E_SHIFT   (0x00000007u)
#define CSL_AEMIF_NANDECC4_P128E_RESETVAL (0x00000000u)

#define CSL_AEMIF_NANDECC4_P64E_MASK     (0x00000040u)
#define CSL_AEMIF_NANDECC4_P64E_SHIFT    (0x00000006u)
#define CSL_AEMIF_NANDECC4_P64E_RESETVAL (0x00000000u)

#define CSL_AEMIF_NANDECC4_P32E_MASK     (0x00000020u)
#define CSL_AEMIF_NANDECC4_P32E_SHIFT    (0x00000005u)
#define CSL_AEMIF_NANDECC4_P32E_RESETVAL (0x00000000u)

#define CSL_AEMIF_NANDECC4_P16E_MASK     (0x00000010u)
#define CSL_AEMIF_NANDECC4_P16E_SHIFT    (0x00000004u)
#define CSL_AEMIF_NANDECC4_P16E_RESETVAL (0x00000000u)

#define CSL_AEMIF_NANDECC4_P8E_MASK      (0x00000008u)
#define CSL_AEMIF_NANDECC4_P8E_SHIFT     (0x00000003u)
#define CSL_AEMIF_NANDECC4_P8E_RESETVAL  (0x00000000u)

#define CSL_AEMIF_NANDECC4_P4E_MASK      (0x00000004u)
#define CSL_AEMIF_NANDECC4_P4E_SHIFT     (0x00000002u)
#define CSL_AEMIF_NANDECC4_P4E_RESETVAL  (0x00000000u)

#define CSL_AEMIF_NANDECC4_P2E_MASK      (0x00000002u)
#define CSL_AEMIF_NANDECC4_P2E_SHIFT     (0x00000001u)
#define CSL_AEMIF_NANDECC4_P2E_RESETVAL  (0x00000000u)

#define CSL_AEMIF_NANDECC4_P1E_MASK      (0x00000001u)
#define CSL_AEMIF_NANDECC4_P1E_SHIFT     (0x00000000u)
#define CSL_AEMIF_NANDECC4_P1E_RESETVAL  (0x00000000u)

#define CSL_AEMIF_NANDECC4_RESETVAL      (0x00000000u)

/* NANDECC5 */


#define CSL_AEMIF_NANDECC5_P2048O_MASK   (0x08000000u)
#define CSL_AEMIF_NANDECC5_P2048O_SHIFT  (0x0000001Bu)
#define CSL_AEMIF_NANDECC5_P2048O_RESETVAL (0x00000000u)

#define CSL_AEMIF_NANDECC5_P1024O_MASK   (0x04000000u)
#define CSL_AEMIF_NANDECC5_P1024O_SHIFT  (0x0000001Au)
#define CSL_AEMIF_NANDECC5_P1024O_RESETVAL (0x00000000u)

#define CSL_AEMIF_NANDECC5_P512O_MASK    (0x02000000u)
#define CSL_AEMIF_NANDECC5_P512O_SHIFT   (0x00000019u)
#define CSL_AEMIF_NANDECC5_P512O_RESETVAL (0x00000000u)

#define CSL_AEMIF_NANDECC5_P256O_MASK    (0x01000000u)
#define CSL_AEMIF_NANDECC5_P256O_SHIFT   (0x00000018u)
#define CSL_AEMIF_NANDECC5_P256O_RESETVAL (0x00000000u)

#define CSL_AEMIF_NANDECC5_P128O_MASK    (0x00800000u)
#define CSL_AEMIF_NANDECC5_P128O_SHIFT   (0x00000017u)
#define CSL_AEMIF_NANDECC5_P128O_RESETVAL (0x00000000u)

#define CSL_AEMIF_NANDECC5_P64O_MASK     (0x00400000u)
#define CSL_AEMIF_NANDECC5_P64O_SHIFT    (0x00000016u)
#define CSL_AEMIF_NANDECC5_P64O_RESETVAL (0x00000000u)

#define CSL_AEMIF_NANDECC5_P32O_MASK     (0x00200000u)
#define CSL_AEMIF_NANDECC5_P32O_SHIFT    (0x00000015u)
#define CSL_AEMIF_NANDECC5_P32O_RESETVAL (0x00000000u)

#define CSL_AEMIF_NANDECC5_P16O_MASK     (0x00100000u)
#define CSL_AEMIF_NANDECC5_P16O_SHIFT    (0x00000014u)
#define CSL_AEMIF_NANDECC5_P16O_RESETVAL (0x00000000u)

#define CSL_AEMIF_NANDECC5_P8O_MASK      (0x00080000u)
#define CSL_AEMIF_NANDECC5_P8O_SHIFT     (0x00000013u)
#define CSL_AEMIF_NANDECC5_P8O_RESETVAL  (0x00000000u)

#define CSL_AEMIF_NANDECC5_P4O_MASK      (0x00040000u)
#define CSL_AEMIF_NANDECC5_P4O_SHIFT     (0x00000012u)
#define CSL_AEMIF_NANDECC5_P4O_RESETVAL  (0x00000000u)

#define CSL_AEMIF_NANDECC5_P2O_MASK      (0x00020000u)
#define CSL_AEMIF_NANDECC5_P2O_SHIFT     (0x00000011u)
#define CSL_AEMIF_NANDECC5_P2O_RESETVAL  (0x00000000u)

#define CSL_AEMIF_NANDECC5_P1O_MASK      (0x00010000u)
#define CSL_AEMIF_NANDECC5_P1O_SHIFT     (0x00000010u)
#define CSL_AEMIF_NANDECC5_P1O_RESETVAL  (0x00000000u)


#define CSL_AEMIF_NANDECC5_P2048E_MASK   (0x00000800u)
#define CSL_AEMIF_NANDECC5_P2048E_SHIFT  (0x0000000Bu)
#define CSL_AEMIF_NANDECC5_P2048E_RESETVAL (0x00000000u)

#define CSL_AEMIF_NANDECC5_P1024E_MASK   (0x00000400u)
#define CSL_AEMIF_NANDECC5_P1024E_SHIFT  (0x0000000Au)
#define CSL_AEMIF_NANDECC5_P1024E_RESETVAL (0x00000000u)

#define CSL_AEMIF_NANDECC5_P512E_MASK    (0x00000200u)
#define CSL_AEMIF_NANDECC5_P512E_SHIFT   (0x00000009u)
#define CSL_AEMIF_NANDECC5_P512E_RESETVAL (0x00000000u)

#define CSL_AEMIF_NANDECC5_P256E_MASK    (0x00000100u)
#define CSL_AEMIF_NANDECC5_P256E_SHIFT   (0x00000008u)
#define CSL_AEMIF_NANDECC5_P256E_RESETVAL (0x00000000u)

#define CSL_AEMIF_NANDECC5_P128E_MASK    (0x00000080u)
#define CSL_AEMIF_NANDECC5_P128E_SHIFT   (0x00000007u)
#define CSL_AEMIF_NANDECC5_P128E_RESETVAL (0x00000000u)

#define CSL_AEMIF_NANDECC5_P64E_MASK     (0x00000040u)
#define CSL_AEMIF_NANDECC5_P64E_SHIFT    (0x00000006u)
#define CSL_AEMIF_NANDECC5_P64E_RESETVAL (0x00000000u)

#define CSL_AEMIF_NANDECC5_P32E_MASK     (0x00000020u)
#define CSL_AEMIF_NANDECC5_P32E_SHIFT    (0x00000005u)
#define CSL_AEMIF_NANDECC5_P32E_RESETVAL (0x00000000u)

#define CSL_AEMIF_NANDECC5_P16E_MASK     (0x00000010u)
#define CSL_AEMIF_NANDECC5_P16E_SHIFT    (0x00000004u)
#define CSL_AEMIF_NANDECC5_P16E_RESETVAL (0x00000000u)

#define CSL_AEMIF_NANDECC5_P8E_MASK      (0x00000008u)
#define CSL_AEMIF_NANDECC5_P8E_SHIFT     (0x00000003u)
#define CSL_AEMIF_NANDECC5_P8E_RESETVAL  (0x00000000u)

#define CSL_AEMIF_NANDECC5_P4E_MASK      (0x00000004u)
#define CSL_AEMIF_NANDECC5_P4E_SHIFT     (0x00000002u)
#define CSL_AEMIF_NANDECC5_P4E_RESETVAL  (0x00000000u)

#define CSL_AEMIF_NANDECC5_P2E_MASK      (0x00000002u)
#define CSL_AEMIF_NANDECC5_P2E_SHIFT     (0x00000001u)
#define CSL_AEMIF_NANDECC5_P2E_RESETVAL  (0x00000000u)

#define CSL_AEMIF_NANDECC5_P1E_MASK      (0x00000001u)
#define CSL_AEMIF_NANDECC5_P1E_SHIFT     (0x00000000u)
#define CSL_AEMIF_NANDECC5_P1E_RESETVAL  (0x00000000u)

#define CSL_AEMIF_NANDECC5_RESETVAL      (0x00000000u)

/* RSVD16 */


#define CSL_AEMIF_RSVD16_RESETVAL        (0x00000000u)

/* IODFTECR */


#define CSL_AEMIF_IODFTECR_TLEC_MASK     (0x0000FFFFu)
#define CSL_AEMIF_IODFTECR_TLEC_SHIFT    (0x00000000u)
#define CSL_AEMIF_IODFTECR_TLEC_RESETVAL (0x00000000u)

#define CSL_AEMIF_IODFTECR_RESETVAL      (0x00000000u)

/* IODFTGCR */


#define CSL_AEMIF_IODFTGCR_MT_MASK       (0x00004000u)
#define CSL_AEMIF_IODFTGCR_MT_SHIFT      (0x0000000Eu)
#define CSL_AEMIF_IODFTGCR_MT_RESETVAL   (0x00000000u)


#define CSL_AEMIF_IODFTGCR_OPG_LD_MASK   (0x00001000u)
#define CSL_AEMIF_IODFTGCR_OPG_LD_SHIFT  (0x0000000Cu)
#define CSL_AEMIF_IODFTGCR_OPG_LD_RESETVAL (0x00000000u)


#define CSL_AEMIF_IODFTGCR_MMS_MASK      (0x00000100u)
#define CSL_AEMIF_IODFTGCR_MMS_SHIFT     (0x00000008u)
#define CSL_AEMIF_IODFTGCR_MMS_RESETVAL  (0x00000000u)

#define CSL_AEMIF_IODFTGCR_ESEL_MASK     (0x00000080u)
#define CSL_AEMIF_IODFTGCR_ESEL_SHIFT    (0x00000007u)
#define CSL_AEMIF_IODFTGCR_ESEL_RESETVAL (0x00000001u)

#define CSL_AEMIF_IODFTGCR_TOEN_MASK     (0x00000040u)
#define CSL_AEMIF_IODFTGCR_TOEN_SHIFT    (0x00000006u)
#define CSL_AEMIF_IODFTGCR_TOEN_RESETVAL (0x00000000u)

#define CSL_AEMIF_IODFTGCR_MC_MASK       (0x00000030u)
#define CSL_AEMIF_IODFTGCR_MC_SHIFT      (0x00000004u)
#define CSL_AEMIF_IODFTGCR_MC_RESETVAL   (0x00000001u)

#define CSL_AEMIF_IODFTGCR_PC_MASK       (0x0000000Eu)
#define CSL_AEMIF_IODFTGCR_PC_SHIFT      (0x00000001u)
#define CSL_AEMIF_IODFTGCR_PC_RESETVAL   (0x00000000u)

#define CSL_AEMIF_IODFTGCR_TM_MASK       (0x00000001u)
#define CSL_AEMIF_IODFTGCR_TM_SHIFT      (0x00000000u)
#define CSL_AEMIF_IODFTGCR_TM_RESETVAL   (0x00000001u)

#define CSL_AEMIF_IODFTGCR_RESETVAL      (0x00000091u)

/* RSVD17 */


#define CSL_AEMIF_RSVD17_RESETVAL        (0x00000000u)

/* IODFTMRLR */

#define CSL_AEMIF_IODFTMRLR_TLMR_MASK    (0xFFFFFFFFu)
#define CSL_AEMIF_IODFTMRLR_TLMR_SHIFT   (0x00000000u)
#define CSL_AEMIF_IODFTMRLR_TLMR_RESETVAL (0x00000000u)

#define CSL_AEMIF_IODFTMRLR_RESETVAL     (0x00000000u)

/* IODFTMRMR */

#define CSL_AEMIF_IODFTMRMR_TLMR_MASK    (0xFFFFFFFFu)
#define CSL_AEMIF_IODFTMRMR_TLMR_SHIFT   (0x00000000u)
#define CSL_AEMIF_IODFTMRMR_TLMR_RESETVAL (0x00000000u)

#define CSL_AEMIF_IODFTMRMR_RESETVAL     (0x00000000u)

/* IODFTMRMSBR */


#define CSL_AEMIF_IODFTMRMSBR_TLMR_MASK  (0x007FFFFFu)
#define CSL_AEMIF_IODFTMRMSBR_TLMR_SHIFT (0x00000000u)
#define CSL_AEMIF_IODFTMRMSBR_TLMR_RESETVAL (0x00000000u)

#define CSL_AEMIF_IODFTMRMSBR_RESETVAL   (0x00000000u)

/* RSVD18 */


#define CSL_AEMIF_RSVD18_RESETVAL        (0x00000000u)

/* RSVD19 */


#define CSL_AEMIF_RSVD19_RESETVAL        (0x00000000u)

/* RSVD20 */


#define CSL_AEMIF_RSVD20_RESETVAL        (0x00000000u)

/* RSVD21 */


#define CSL_AEMIF_RSVD21_RESETVAL        (0x00000000u)

/* RSVD22 */


#define CSL_AEMIF_RSVD22_RESETVAL        (0x00000000u)

/* MODRNR */


#define CSL_AEMIF_MODRNR_TLMR_MASK       (0x000000FFu)
#define CSL_AEMIF_MODRNR_TLMR_SHIFT      (0x00000000u)
#define CSL_AEMIF_MODRNR_TLMR_RESETVAL   (0x00000000u)

#define CSL_AEMIF_MODRNR_RESETVAL        (0x00000000u)

#endif