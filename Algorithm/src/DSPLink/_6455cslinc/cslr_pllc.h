/*********************************************************************
 * Copyright (C) 2003-2005 Texas Instruments Incorporated. 
 * All Rights Reserved 
 *********************************************************************/
 /** \file cslr_pllc.h
 * 
 * \brief This file contains the Register Desciptions for PLLC
 * 
 *********************************************************************/

#ifndef _CSLR_PLLC_H_
#define _CSLR_PLLC_H_

#include <cslr.h>

#include <tistdtypes.h>

/**************************************************************************\
* Register Overlay Structure
\**************************************************************************/
typedef struct  {
    volatile Uint8 RSVD0[228];
    volatile Uint32 RSTYPE;
    volatile Uint8 RSVD1[24];
    volatile Uint32 PLLCTL;
    volatile Uint8 RSVD2[12];
    volatile Uint32 PLLM;
    volatile Uint32 PREDIV;
    volatile Uint32 PLLDIV1;
    volatile Uint8 RSVD3[28];
    volatile Uint32 PLLCMD;
    volatile Uint32 PLLSTAT;
    volatile Uint32 ALNCTL;
    volatile Uint32 DCHANGE;
    volatile Uint8 RSVD4[8];
    volatile Uint32 SYSTAT;
    volatile Uint8 RSVD5[12];
    volatile Uint32 PLLDIV4;
    volatile Uint32 PLLDIV5;
} CSL_PllcRegs;

/**************************************************************************\
* Overlay structure typedef definition
\**************************************************************************/
typedef volatile CSL_PllcRegs             *CSL_PllcRegsOvly;

/**************************************************************************\
* Field Definition Macros
\**************************************************************************/

/* RSTYPE */

#define CSL_PLLC_RSTYPE_SRST_MASK        (0x00000008u)
#define CSL_PLLC_RSTYPE_SRST_SHIFT       (0x00000003u)
#define CSL_PLLC_RSTYPE_SRST_RESETVAL    (0x00000000u)

/*----SRST Tokens----*/
#define CSL_PLLC_RSTYPE_SRST_NO          (0x00000000u)
#define CSL_PLLC_RSTYPE_SRST_YES         (0x00000001u)

#define CSL_PLLC_RSTYPE_MRST_MASK        (0x00000004u)
#define CSL_PLLC_RSTYPE_MRST_SHIFT       (0x00000002u)
#define CSL_PLLC_RSTYPE_MRST_RESETVAL    (0x00000000u)

/*----MRST Tokens----*/
#define CSL_PLLC_RSTYPE_MRST_NO          (0x00000000u)
#define CSL_PLLC_RSTYPE_MRST_YES         (0x00000001u)

#define CSL_PLLC_RSTYPE_WRST_MASK        (0x00000002u)
#define CSL_PLLC_RSTYPE_WRST_SHIFT       (0x00000001u)
#define CSL_PLLC_RSTYPE_WRST_RESETVAL    (0x00000000u)

/*----WRST Tokens----*/
#define CSL_PLLC_RSTYPE_WRST_NO          (0x00000000u)
#define CSL_PLLC_RSTYPE_WRST_YES         (0x00000001u)

#define CSL_PLLC_RSTYPE_POR_MASK         (0x00000001u)
#define CSL_PLLC_RSTYPE_POR_SHIFT        (0x00000000u)
#define CSL_PLLC_RSTYPE_POR_RESETVAL     (0x00000000u)

/*----POR Tokens----*/
#define CSL_PLLC_RSTYPE_POR_NO           (0x00000000u)
#define CSL_PLLC_RSTYPE_POR_YES          (0x00000001u)

#define CSL_PLLC_RSTYPE_RESETVAL         (0x00000000u)

/* PLLCTL */

#define CSL_PLLC_PLLCTL_PLLENSRC_MASK    (0x00000020u)
#define CSL_PLLC_PLLCTL_PLLENSRC_SHIFT   (0x00000005u)
#define CSL_PLLC_PLLCTL_PLLENSRC_RESETVAL (0x00000000u)

/*----PLLENSRC Tokens----*/
#define CSL_PLLC_PLLCTL_PLLENSRC_REGBIT  (0x00000000u)
#define CSL_PLLC_PLLCTL_PLLENSRC_NONREGBIT (0x00000001u)

#define CSL_PLLC_PLLCTL_PLLRST_MASK      (0x00000008u)
#define CSL_PLLC_PLLCTL_PLLRST_SHIFT     (0x00000003u)
#define CSL_PLLC_PLLCTL_PLLRST_RESETVAL  (0x00000001u)

/*----PLLRST Tokens----*/
#define CSL_PLLC_PLLCTL_PLLRST_NO        (0x00000000u)
#define CSL_PLLC_PLLCTL_PLLRST_YES       (0x00000001u)

#define CSL_PLLC_PLLCTL_PLLPWRDN_MASK    (0x00000002u)
#define CSL_PLLC_PLLCTL_PLLPWRDN_SHIFT   (0x00000001u)
#define CSL_PLLC_PLLCTL_PLLPWRDN_RESETVAL (0x00000000u)

/*----PLLPWRDN Tokens----*/
#define CSL_PLLC_PLLCTL_PLLPWRDN_NO      (0x00000000u)
#define CSL_PLLC_PLLCTL_PLLPWRDN_YES     (0x00000001u)

#define CSL_PLLC_PLLCTL_PLLEN_MASK       (0x00000001u)
#define CSL_PLLC_PLLCTL_PLLEN_SHIFT      (0x00000000u)
#define CSL_PLLC_PLLCTL_PLLEN_RESETVAL   (0x00000000u)

/*----PLLEN Tokens----*/
#define CSL_PLLC_PLLCTL_PLLEN_BYPASS     (0x00000000u)
#define CSL_PLLC_PLLCTL_PLLEN_PLL        (0x00000001u)

#define CSL_PLLC_PLLCTL_RESETVAL         (0x00000048u)

/* PLLM */

#define CSL_PLLC_PLLM_PLLM_MASK          (0x0000003Fu)
#define CSL_PLLC_PLLM_PLLM_SHIFT         (0x00000000u)
#define CSL_PLLC_PLLM_PLLM_RESETVAL      (0x00000000u)

#define CSL_PLLC_PLLM_RESETVAL           (0x00000000u)

/* PREDIV */

#define CSL_PLLC_PREDIV_PREDEN_MASK      (0x00008000u)
#define CSL_PLLC_PREDIV_PREDEN_SHIFT     (0x0000000Fu)
#define CSL_PLLC_PREDIV_PREDEN_RESETVAL  (0x00000001u)

/*----PREDEN Tokens----*/
#define CSL_PLLC_PREDIV_PREDEN_DISABLE   (0x00000000u)
#define CSL_PLLC_PREDIV_PREDEN_ENABLE    (0x00000001u)

#define CSL_PLLC_PREDIV_RATIO_MASK       (0x0000001Fu)
#define CSL_PLLC_PREDIV_RATIO_SHIFT      (0x00000000u)
#define CSL_PLLC_PREDIV_RATIO_RESETVAL   (0x00000002u)

#define CSL_PLLC_PREDIV_RESETVAL         (0x00008002u)

/* PLLDIV1 */

#define CSL_PLLC_PLLDIV1_D1EN_MASK       (0x00008000u)
#define CSL_PLLC_PLLDIV1_D1EN_SHIFT      (0x0000000Fu)
#define CSL_PLLC_PLLDIV1_D1EN_RESETVAL   (0x00000001u)

/*----D1EN Tokens----*/
#define CSL_PLLC_PLLDIV1_D1EN_DISABLE    (0x00000000u)
#define CSL_PLLC_PLLDIV1_D1EN_ENABLE     (0x00000001u)

#define CSL_PLLC_PLLDIV1_RATIO_MASK      (0x0000001Fu)
#define CSL_PLLC_PLLDIV1_RATIO_SHIFT     (0x00000000u)
#define CSL_PLLC_PLLDIV1_RATIO_RESETVAL  (0x00000001u)

/*----RATIO Tokens----*/
#define CSL_PLLC_PLLDIV1_RATIO_DIV2      (0x00000001u)
#define CSL_PLLC_PLLDIV1_RATIO_DIV5      (0x00000004u)

#define CSL_PLLC_PLLDIV1_RESETVAL        (0x00008001u)

/* PLLCMD */

#define CSL_PLLC_PLLCMD_GOSET_MASK       (0x00000001u)
#define CSL_PLLC_PLLCMD_GOSET_SHIFT      (0x00000000u)
#define CSL_PLLC_PLLCMD_GOSET_RESETVAL   (0x00000000u)

/*----GOSET Tokens----*/
#define CSL_PLLC_PLLCMD_GOSET_CLRBIT     (0x00000000u)
#define CSL_PLLC_PLLCMD_GOSET_SET        (0x00000001u)

#define CSL_PLLC_PLLCMD_RESETVAL         (0x00000000u)

/* PLLSTAT */

#define CSL_PLLC_PLLSTAT_GOSTAT_MASK     (0x00000001u)
#define CSL_PLLC_PLLSTAT_GOSTAT_SHIFT    (0x00000000u)
#define CSL_PLLC_PLLSTAT_GOSTAT_RESETVAL (0x00000000u)

/*----GOSTAT Tokens----*/
#define CSL_PLLC_PLLSTAT_GOSTAT_NONE     (0x00000000u)
#define CSL_PLLC_PLLSTAT_GOSTAT_INPROG   (0x00000001u)

#define CSL_PLLC_PLLSTAT_RESETVAL        (0x00000000u)

/* ALNCTL */

#define CSL_PLLC_ALNCTL_ALN5_MASK        (0x00000010u)
#define CSL_PLLC_ALNCTL_ALN5_SHIFT       (0x00000004u)
#define CSL_PLLC_ALNCTL_ALN5_RESETVAL    (0x00000001u)

/*----ALN5 Tokens----*/
#define CSL_PLLC_ALNCTL_ALN5_NO          (0x00000000u)
#define CSL_PLLC_ALNCTL_ALN5_YES         (0x00000001u)

#define CSL_PLLC_ALNCTL_ALN4_MASK        (0x00000008u)
#define CSL_PLLC_ALNCTL_ALN4_SHIFT       (0x00000003u)
#define CSL_PLLC_ALNCTL_ALN4_RESETVAL    (0x00000001u)

/*----ALN4 Tokens----*/
#define CSL_PLLC_ALNCTL_ALN4_NO          (0x00000000u)
#define CSL_PLLC_ALNCTL_ALN4_YES         (0x00000001u)

#define CSL_PLLC_ALNCTL_ALN1_MASK        (0x00000001u)
#define CSL_PLLC_ALNCTL_ALN1_SHIFT       (0x00000000u)
#define CSL_PLLC_ALNCTL_ALN1_RESETVAL    (0x00000001u)

/*----ALN1 Tokens----*/
#define CSL_PLLC_ALNCTL_ALN1_NO          (0x00000000u)
#define CSL_PLLC_ALNCTL_ALN1_YES         (0x00000001u)

#define CSL_PLLC_ALNCTL_RESETVAL         (0x00000019u)

/* DCHANGE */

#define CSL_PLLC_DCHANGE_SYS5_MASK       (0x00000010u)
#define CSL_PLLC_DCHANGE_SYS5_SHIFT      (0x00000004u)
#define CSL_PLLC_DCHANGE_SYS5_RESETVAL   (0x00000000u)

/*----SYS5 Tokens----*/
#define CSL_PLLC_DCHANGE_SYS5_NO         (0x00000000u)
#define CSL_PLLC_DCHANGE_SYS5_YES        (0x00000001u)

#define CSL_PLLC_DCHANGE_SYS4_MASK       (0x00000008u)
#define CSL_PLLC_DCHANGE_SYS4_SHIFT      (0x00000003u)
#define CSL_PLLC_DCHANGE_SYS4_RESETVAL   (0x00000000u)

/*----SYS4 Tokens----*/
#define CSL_PLLC_DCHANGE_SYS4_NO         (0x00000000u)
#define CSL_PLLC_DCHANGE_SYS4_YES        (0x00000001u)

#define CSL_PLLC_DCHANGE_SYS1_MASK       (0x00000001u)
#define CSL_PLLC_DCHANGE_SYS1_SHIFT      (0x00000000u)
#define CSL_PLLC_DCHANGE_SYS1_RESETVAL   (0x00000000u)

/*----SYS1 Tokens----*/
#define CSL_PLLC_DCHANGE_SYS1_NO         (0x00000000u)
#define CSL_PLLC_DCHANGE_SYS1_YES        (0x00000001u)

#define CSL_PLLC_DCHANGE_RESETVAL        (0x00000000u)

/* SYSTAT */

#define CSL_PLLC_SYSTAT_SYS5ON_MASK      (0x00000010u)
#define CSL_PLLC_SYSTAT_SYS5ON_SHIFT     (0x00000004u)
#define CSL_PLLC_SYSTAT_SYS5ON_RESETVAL  (0x00000001u)

/*----SYS5ON Tokens----*/
#define CSL_PLLC_SYSTAT_SYS5ON_OFF       (0x00000000u)
#define CSL_PLLC_SYSTAT_SYS5ON_ON        (0x00000001u)

#define CSL_PLLC_SYSTAT_SYS4ON_MASK      (0x00000008u)
#define CSL_PLLC_SYSTAT_SYS4ON_SHIFT     (0x00000003u)
#define CSL_PLLC_SYSTAT_SYS4ON_RESETVAL  (0x00000001u)

/*----SYS4ON Tokens----*/
#define CSL_PLLC_SYSTAT_SYS4ON_OFF       (0x00000000u)
#define CSL_PLLC_SYSTAT_SYS4ON_ON        (0x00000001u)

#define CSL_PLLC_SYSTAT_SYS3ON_MASK      (0x00000004u)
#define CSL_PLLC_SYSTAT_SYS3ON_SHIFT     (0x00000002u)
#define CSL_PLLC_SYSTAT_SYS3ON_RESETVAL  (0x00000001u)

/*----SYS3ON Tokens----*/
#define CSL_PLLC_SYSTAT_SYS3ON_OFF       (0x00000000u)
#define CSL_PLLC_SYSTAT_SYS3ON_ON        (0x00000001u)

#define CSL_PLLC_SYSTAT_SYS2ON_MASK      (0x00000002u)
#define CSL_PLLC_SYSTAT_SYS2ON_SHIFT     (0x00000001u)
#define CSL_PLLC_SYSTAT_SYS2ON_RESETVAL  (0x00000001u)

/*----SYS2ON Tokens----*/
#define CSL_PLLC_SYSTAT_SYS2ON_OFF       (0x00000000u)
#define CSL_PLLC_SYSTAT_SYS2ON_ON        (0x00000001u)

#define CSL_PLLC_SYSTAT_SYS1ON_MASK      (0x00000001u)
#define CSL_PLLC_SYSTAT_SYS1ON_SHIFT     (0x00000000u)
#define CSL_PLLC_SYSTAT_SYS1ON_RESETVAL  (0x00000001u)

/*----SYS1ON Tokens----*/
#define CSL_PLLC_SYSTAT_SYS1ON_OFF       (0x00000000u)
#define CSL_PLLC_SYSTAT_SYS1ON_ON        (0x00000001u)

#define CSL_PLLC_SYSTAT_RESETVAL         (0x0000001Fu)

/* PLLDIV4 */

#define CSL_PLLC_PLLDIV4_D4EN_MASK       (0x00008000u)
#define CSL_PLLC_PLLDIV4_D4EN_SHIFT      (0x0000000Fu)
#define CSL_PLLC_PLLDIV4_D4EN_RESETVAL   (0x00000001u)

/*----D4EN Tokens----*/
#define CSL_PLLC_PLLDIV4_D4EN_DISABLE    (0x00000000u)
#define CSL_PLLC_PLLDIV4_D4EN_ENABLE     (0x00000001u)

#define CSL_PLLC_PLLDIV4_RATIO_MASK      (0x0000001Fu)
#define CSL_PLLC_PLLDIV4_RATIO_SHIFT     (0x00000000u)
#define CSL_PLLC_PLLDIV4_RATIO_RESETVAL  (0x00000007u)

#define CSL_PLLC_PLLDIV4_RESETVAL        (0x00008007u)

/* PLLDIV5 */

#define CSL_PLLC_PLLDIV5_D5EN_MASK       (0x00008000u)
#define CSL_PLLC_PLLDIV5_D5EN_SHIFT      (0x0000000Fu)
#define CSL_PLLC_PLLDIV5_D5EN_RESETVAL   (0x00000001u)

/*----D5EN Tokens----*/
#define CSL_PLLC_PLLDIV5_D5EN_DISABLE    (0x00000000u)
#define CSL_PLLC_PLLDIV5_D5EN_ENABLE     (0x00000001u)

#define CSL_PLLC_PLLDIV5_RATIO_MASK      (0x0000001Fu)
#define CSL_PLLC_PLLDIV5_RATIO_SHIFT     (0x00000000u)
#define CSL_PLLC_PLLDIV5_RATIO_RESETVAL  (0x00000002u)

#define CSL_PLLC_PLLDIV5_RESETVAL        (0x00008002u)


#endif

