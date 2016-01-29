#ifndef _CSLR_I2C_1_H_
#define _CSLR_I2C_1_H_
/*********************************************************************
 * Copyright (C) 2003-2004 Texas Instruments Incorporated.
 * All Rights Reserved
 *********************************************************************/
 /** \file cslr_i2c_1.h
 *
 * \brief This file contains the Register Desciptions for I2C
 *
 *********************************************************************/

#include <cslr.h>

#include <tistdtypes.h>

/**************************************************************************\
* Register Overlay Structure
\**************************************************************************/
typedef struct  {
    volatile Uint16 ICOAR;
    volatile Uint8 RSVD0[2];
    volatile Uint16 ICIMR;
    volatile Uint8 RSVD1[2];
    volatile Uint16 ICSTR;
    volatile Uint8 RSVD2[2];
    volatile Uint16 ICCLKL;
    volatile Uint8 RSVD3[2];
    volatile Uint16 ICCLKH;
    volatile Uint8 RSVD4[2];
    volatile Uint16 ICCNT;
    volatile Uint8 RSVD5[2];
    volatile Uint16 ICDRR;
    volatile Uint8 RSVD6[2];
    volatile Uint16 ICSAR;
    volatile Uint8 RSVD7[2];
    volatile Uint16 ICDXR;
    volatile Uint8 RSVD8[2];
    volatile Uint16 ICMDR;
    volatile Uint8 RSVD9[2];
    volatile Uint16 ICIVR;
    volatile Uint8 RSVD10[2];
    volatile Uint16 ICEMDR;
    volatile Uint8 RSVD11[2];
    volatile Uint16 ICPSC;
    volatile Uint8 RSVD12[2];
    volatile Uint16 ICPID1;
    volatile Uint8 RSVD13[2];
    volatile Uint16 ICPID2;
    volatile Uint8 RSVD14[2];
    volatile Uint16 ICDMAC;
    volatile Uint8 RSVD15[10];
    volatile Uint32 ICPFUNC;
    volatile Uint32 ICPDIR;
    volatile Uint32 ICPDIN;
    volatile Uint32 ICPDOUT;
    volatile Uint32 ICPDSET;
    volatile Uint32 ICPDCLR;
} CSL_I2cRegs;

/**************************************************************************\
* Field Definition Macros
\**************************************************************************/

/* ICOAR */

#define CSL_I2C_ICOAR_OADDR_MASK         (0x03FFu)
#define CSL_I2C_ICOAR_OADDR_SHIFT        (0x0000u)
#define CSL_I2C_ICOAR_OADDR_RESETVAL     (0x0000u)

#define CSL_I2C_ICOAR_RESETVAL           (0x0000u)

/* ICIMR */

#define CSL_I2C_ICIMR_AAS_MASK           (0x0040u)
#define CSL_I2C_ICIMR_AAS_SHIFT          (0x0006u)
#define CSL_I2C_ICIMR_AAS_RESETVAL       (0x0000u)

/*----AAS Tokens----*/
#define CSL_I2C_ICIMR_AAS_DISABLE        (0x0000u)
#define CSL_I2C_ICIMR_AAS_ENABLE         (0x0001u)

#define CSL_I2C_ICIMR_SCD_MASK           (0x0020u)
#define CSL_I2C_ICIMR_SCD_SHIFT          (0x0005u)
#define CSL_I2C_ICIMR_SCD_RESETVAL       (0x0000u)

/*----SCD Tokens----*/
#define CSL_I2C_ICIMR_SCD_DISABLE        (0x0000u)
#define CSL_I2C_ICIMR_SCD_ENABLE         (0x0001u)

#define CSL_I2C_ICIMR_ICXRDY_MASK        (0x0010u)
#define CSL_I2C_ICIMR_ICXRDY_SHIFT       (0x0004u)
#define CSL_I2C_ICIMR_ICXRDY_RESETVAL    (0x0000u)

/*----ICXRDY Tokens----*/
#define CSL_I2C_ICIMR_ICXRDY_DISABLE     (0x0000u)
#define CSL_I2C_ICIMR_ICXRDY_ENABLE      (0x0001u)

#define CSL_I2C_ICIMR_ICRRDY_MASK        (0x0008u)
#define CSL_I2C_ICIMR_ICRRDY_SHIFT       (0x0003u)
#define CSL_I2C_ICIMR_ICRRDY_RESETVAL    (0x0000u)

/*----ICRRDY Tokens----*/
#define CSL_I2C_ICIMR_ICRRDY_DISABLE     (0x0000u)
#define CSL_I2C_ICIMR_ICRRDY_ENABLE      (0x0001u)

#define CSL_I2C_ICIMR_ARDY_MASK          (0x0004u)
#define CSL_I2C_ICIMR_ARDY_SHIFT         (0x0002u)
#define CSL_I2C_ICIMR_ARDY_RESETVAL      (0x0000u)

/*----ARDY Tokens----*/
#define CSL_I2C_ICIMR_ARDY_DISABLE       (0x0000u)
#define CSL_I2C_ICIMR_ARDY_ENABLE        (0x0001u)

#define CSL_I2C_ICIMR_NACK_MASK          (0x0002u)
#define CSL_I2C_ICIMR_NACK_SHIFT         (0x0001u)
#define CSL_I2C_ICIMR_NACK_RESETVAL      (0x0000u)

/*----NACK Tokens----*/
#define CSL_I2C_ICIMR_NACK_DISABLE       (0x0000u)
#define CSL_I2C_ICIMR_NACK_ENABLE        (0x0001u)

#define CSL_I2C_ICIMR_AL_MASK            (0x0001u)
#define CSL_I2C_ICIMR_AL_SHIFT           (0x0000u)
#define CSL_I2C_ICIMR_AL_RESETVAL        (0x0000u)

/*----AL Tokens----*/
#define CSL_I2C_ICIMR_AL_DISABLE         (0x0000u)
#define CSL_I2C_ICIMR_AL_ENABLE          (0x0001u)

#define CSL_I2C_ICIMR_RESETVAL           (0x0000u)

/* ICSTR */

#define CSL_I2C_ICSTR_SDIR_MASK          (0x4000u)
#define CSL_I2C_ICSTR_SDIR_SHIFT         (0x000Eu)
#define CSL_I2C_ICSTR_SDIR_RESETVAL      (0x0000u)

#define CSL_I2C_ICSTR_NACKSNT_MASK       (0x2000u)
#define CSL_I2C_ICSTR_NACKSNT_SHIFT      (0x000Du)
#define CSL_I2C_ICSTR_NACKSNT_RESETVAL   (0x0000u)

#define CSL_I2C_ICSTR_BB_MASK            (0x1000u)
#define CSL_I2C_ICSTR_BB_SHIFT           (0x000Cu)
#define CSL_I2C_ICSTR_BB_RESETVAL        (0x0000u)

#define CSL_I2C_ICSTR_RSFULL_MASK        (0x0800u)
#define CSL_I2C_ICSTR_RSFULL_SHIFT       (0x000Bu)
#define CSL_I2C_ICSTR_RSFULL_RESETVAL    (0x0000u)

#define CSL_I2C_ICSTR_XSMT_MASK          (0x0400u)
#define CSL_I2C_ICSTR_XSMT_SHIFT         (0x000Au)
#define CSL_I2C_ICSTR_XSMT_RESETVAL      (0x0001u)

#define CSL_I2C_ICSTR_AAS_MASK           (0x0200u)
#define CSL_I2C_ICSTR_AAS_SHIFT          (0x0009u)
#define CSL_I2C_ICSTR_AAS_RESETVAL       (0x0000u)

#define CSL_I2C_ICSTR_AD0_MASK           (0x0100u)
#define CSL_I2C_ICSTR_AD0_SHIFT          (0x0008u)
#define CSL_I2C_ICSTR_AD0_RESETVAL       (0x0000u)

#define CSL_I2C_ICSTR_SCD_MASK           (0x0020u)
#define CSL_I2C_ICSTR_SCD_SHIFT          (0x0005u)
#define CSL_I2C_ICSTR_SCD_RESETVAL       (0x0000u)

#define CSL_I2C_ICSTR_ICXRDY_MASK        (0x0010u)
#define CSL_I2C_ICSTR_ICXRDY_SHIFT       (0x0004u)
#define CSL_I2C_ICSTR_ICXRDY_RESETVAL    (0x0001u)

#define CSL_I2C_ICSTR_ICRRDY_MASK        (0x0008u)
#define CSL_I2C_ICSTR_ICRRDY_SHIFT       (0x0003u)
#define CSL_I2C_ICSTR_ICRRDY_RESETVAL    (0x0000u)

#define CSL_I2C_ICSTR_ARDY_MASK          (0x0004u)
#define CSL_I2C_ICSTR_ARDY_SHIFT         (0x0002u)
#define CSL_I2C_ICSTR_ARDY_RESETVAL      (0x0000u)

#define CSL_I2C_ICSTR_NACK_MASK          (0x0002u)
#define CSL_I2C_ICSTR_NACK_SHIFT         (0x0001u)
#define CSL_I2C_ICSTR_NACK_RESETVAL      (0x0000u)

#define CSL_I2C_ICSTR_AL_MASK            (0x0001u)
#define CSL_I2C_ICSTR_AL_SHIFT           (0x0000u)
#define CSL_I2C_ICSTR_AL_RESETVAL        (0x0000u)

#define CSL_I2C_ICSTR_RESETVAL           (0x0410u)

/* ICCLKL */

#define CSL_I2C_ICCLKL_ICCL_MASK         (0xFFFFu)
#define CSL_I2C_ICCLKL_ICCL_SHIFT        (0x0000u)
#define CSL_I2C_ICCLKL_ICCL_RESETVAL     (0x0000u)

#define CSL_I2C_ICCLKL_RESETVAL          (0x0000u)

/* ICCLKH */

#define CSL_I2C_ICCLKH_ICCH_MASK         (0xFFFFu)
#define CSL_I2C_ICCLKH_ICCH_SHIFT        (0x0000u)
#define CSL_I2C_ICCLKH_ICCH_RESETVAL     (0x0000u)

#define CSL_I2C_ICCLKH_RESETVAL          (0x0000u)

/* ICCNT */

#define CSL_I2C_ICCNT_ICDC_MASK          (0xFFFFu)
#define CSL_I2C_ICCNT_ICDC_SHIFT         (0x0000u)
#define CSL_I2C_ICCNT_ICDC_RESETVAL      (0x0000u)

#define CSL_I2C_ICCNT_RESETVAL           (0x0000u)

/* ICDRR */

#define CSL_I2C_ICDRR_D_MASK             (0x00FFu)
#define CSL_I2C_ICDRR_D_SHIFT            (0x0000u)
#define CSL_I2C_ICDRR_D_RESETVAL         (0x0000u)

#define CSL_I2C_ICDRR_RESETVAL           (0x0000u)

/* ICSAR */

#define CSL_I2C_ICSAR_SADDR_MASK         (0x03FFu)
#define CSL_I2C_ICSAR_SADDR_SHIFT        (0x0000u)
#define CSL_I2C_ICSAR_SADDR_RESETVAL     (0x03FFu)

#define CSL_I2C_ICSAR_RESETVAL           (0x03FFu)

/* ICDXR */

#define CSL_I2C_ICDXR_D_MASK             (0x00FFu)
#define CSL_I2C_ICDXR_D_SHIFT            (0x0000u)
#define CSL_I2C_ICDXR_D_RESETVAL         (0x0000u)

#define CSL_I2C_ICDXR_RESETVAL           (0x0000u)

/* ICMDR */

#define CSL_I2C_ICMDR_NACKMOD_MASK       (0x8000u)
#define CSL_I2C_ICMDR_NACKMOD_SHIFT      (0x000Fu)
#define CSL_I2C_ICMDR_NACKMOD_RESETVAL   (0x0000u)

#define CSL_I2C_ICMDR_FREE_MASK          (0x4000u)
#define CSL_I2C_ICMDR_FREE_SHIFT         (0x000Eu)
#define CSL_I2C_ICMDR_FREE_RESETVAL      (0x0000u)

#define CSL_I2C_ICMDR_STT_MASK           (0x2000u)
#define CSL_I2C_ICMDR_STT_SHIFT          (0x000Du)
#define CSL_I2C_ICMDR_STT_RESETVAL       (0x0000u)

#define CSL_I2C_ICMDR_STP_MASK           (0x0800u)
#define CSL_I2C_ICMDR_STP_SHIFT          (0x000Bu)
#define CSL_I2C_ICMDR_STP_RESETVAL       (0x0000u)

#define CSL_I2C_ICMDR_MST_MASK           (0x0400u)
#define CSL_I2C_ICMDR_MST_SHIFT          (0x000Au)
#define CSL_I2C_ICMDR_MST_RESETVAL       (0x0000u)

#define CSL_I2C_ICMDR_TRX_MASK           (0x0200u)
#define CSL_I2C_ICMDR_TRX_SHIFT          (0x0009u)
#define CSL_I2C_ICMDR_TRX_RESETVAL       (0x0000u)

#define CSL_I2C_ICMDR_XA_MASK            (0x0100u)
#define CSL_I2C_ICMDR_XA_SHIFT           (0x0008u)
#define CSL_I2C_ICMDR_XA_RESETVAL        (0x0000u)

#define CSL_I2C_ICMDR_RM_MASK            (0x0080u)
#define CSL_I2C_ICMDR_RM_SHIFT           (0x0007u)
#define CSL_I2C_ICMDR_RM_RESETVAL        (0x0000u)

#define CSL_I2C_ICMDR_DLB_MASK           (0x0040u)
#define CSL_I2C_ICMDR_DLB_SHIFT          (0x0006u)
#define CSL_I2C_ICMDR_DLB_RESETVAL       (0x0000u)

#define CSL_I2C_ICMDR_IRS_MASK           (0x0020u)
#define CSL_I2C_ICMDR_IRS_SHIFT          (0x0005u)
#define CSL_I2C_ICMDR_IRS_RESETVAL       (0x0000u)

#define CSL_I2C_ICMDR_STB_MASK           (0x0010u)
#define CSL_I2C_ICMDR_STB_SHIFT          (0x0004u)
#define CSL_I2C_ICMDR_STB_RESETVAL       (0x0000u)

#define CSL_I2C_ICMDR_FDF_MASK           (0x0008u)
#define CSL_I2C_ICMDR_FDF_SHIFT          (0x0003u)
#define CSL_I2C_ICMDR_FDF_RESETVAL       (0x0000u)

#define CSL_I2C_ICMDR_BC_MASK            (0x0007u)
#define CSL_I2C_ICMDR_BC_SHIFT           (0x0000u)
#define CSL_I2C_ICMDR_BC_RESETVAL        (0x0000u)

#define CSL_I2C_ICMDR_RESETVAL           (0x0000u)

/* ICIVR */

#define CSL_I2C_ICIVR_TESTMD_MASK        (0x0F00u)
#define CSL_I2C_ICIVR_TESTMD_SHIFT       (0x0008u)
#define CSL_I2C_ICIVR_TESTMD_RESETVAL    (0x0000u)

#define CSL_I2C_ICIVR_INTCODE_MASK       (0x0007u)
#define CSL_I2C_ICIVR_INTCODE_SHIFT      (0x0000u)
#define CSL_I2C_ICIVR_INTCODE_RESETVAL   (0x0000u)

/*----INTCODE Tokens----*/
#define CSL_I2C_ICIVR_INTCODE_NONE       (0x0000u)
#define CSL_I2C_ICIVR_INTCODE_AL         (0x0001u)
#define CSL_I2C_ICIVR_INTCODE_NACK       (0x0002u)
#define CSL_I2C_ICIVR_INTCODE_RAR        (0x0003u)
#define CSL_I2C_ICIVR_INTCODE_RDR        (0x0004u)
#define CSL_I2C_ICIVR_INTCODE_TDR        (0x0005u)
#define CSL_I2C_ICIVR_INTCODE_SCD        (0x0006u)
#define CSL_I2C_ICIVR_INTCODE_AAS        (0x0007u)

#define CSL_I2C_ICIVR_RESETVAL           (0x0000u)

/* ICEMDR */

#define CSL_I2C_ICEMDR_BCM_MASK          (0x0001u)
#define CSL_I2C_ICEMDR_BCM_SHIFT         (0x0000u)
#define CSL_I2C_ICEMDR_BCM_RESETVAL      (0x0001u)

#define CSL_I2C_ICEMDR_RESETVAL          (0x0001u)

/* ICPSC */

#define CSL_I2C_ICPSC_IPSC_MASK          (0x00FFu)
#define CSL_I2C_ICPSC_IPSC_SHIFT         (0x0000u)
#define CSL_I2C_ICPSC_IPSC_RESETVAL      (0x0000u)

#define CSL_I2C_ICPSC_RESETVAL           (0x0000u)

/* ICPID1 */

#define CSL_I2C_ICPID1_CLASS_MASK        (0xFF00u)
#define CSL_I2C_ICPID1_CLASS_SHIFT       (0x0008u)
#define CSL_I2C_ICPID1_CLASS_RESETVAL    (0x0001u)

#define CSL_I2C_ICPID1_REVISION_MASK     (0x00FFu)
#define CSL_I2C_ICPID1_REVISION_SHIFT    (0x0000u)
#define CSL_I2C_ICPID1_REVISION_RESETVAL (0x0006u) // for DaVinci=0x05, for HD=0x06

#define CSL_I2C_ICPID1_RESETVAL          (0x0106u)

/* ICPID2 */

#define CSL_I2C_ICPID2_TYPE_MASK         (0x00FFu)
#define CSL_I2C_ICPID2_TYPE_SHIFT        (0x0000u)
#define CSL_I2C_ICPID2_TYPE_RESETVAL     (0x0005u)

#define CSL_I2C_ICPID2_RESETVAL          (0x0005u)

/* ICDMAC */

#define CSL_I2C_ICDMAC_TXDMAEN_MASK      (0x0002u)
#define CSL_I2C_ICDMAC_TXDMAEN_SHIFT     (0x0001u)
#define CSL_I2C_ICDMAC_TXDMAEN_RESETVAL  (0x0001u)
#define CSL_I2C_ICDMAC_TXDMAEN_TXDMA_EVT_DISABLED (0x0000u)
#define CSL_I2C_ICDMAC_TXDMAEN_TXDMA_EVT_ENABLED (0x0001u)

#define CSL_I2C_ICDMAC_RXDMAEN_MASK      (0x0001u)
#define CSL_I2C_ICDMAC_RXDMAEN_SHIFT     (0x0000u)
#define CSL_I2C_ICDMAC_RXDMAEN_RESETVAL  (0x0001u)
#define CSL_I2C_ICDMAC_RXDMAEN_RXDMA_EVT_DISABLED (0x0000u)
#define CSL_I2C_ICDMAC_RXDMAEN_RXDMA_EVT_ENABLED (0x0001u)

#define CSL_I2C_ICDMAC_RESETVAL          (0x0003u)

/* ICPFUNC */

#define CSL_I2C_ICPFUNC_PFUNC_MASK       (0x00000001u)
#define CSL_I2C_ICPFUNC_PFUNC_SHIFT      (0x00000000u)
#define CSL_I2C_ICPFUNC_PFUNC_RESETVAL   (0x00000000u)

#define CSL_I2C_ICPFUNC_RESETVAL         (0x00000000u)

/* ICPDIR */

#define CSL_I2C_ICPDIR_PDIR1_MASK        (0x00000002u)
#define CSL_I2C_ICPDIR_PDIR1_SHIFT       (0x00000001u)
#define CSL_I2C_ICPDIR_PDIR1_RESETVAL    (0x00000000u)

/*----PDIR1 Tokens----*/
#define CSL_I2C_ICPDIR_PDIR1_IN          (0x00000000u)
#define CSL_I2C_ICPDIR_PDIR1_OUT         (0x00000001u)

#define CSL_I2C_ICPDIR_PDIR0_MASK        (0x00000001u)
#define CSL_I2C_ICPDIR_PDIR0_SHIFT       (0x00000000u)
#define CSL_I2C_ICPDIR_PDIR0_RESETVAL    (0x00000000u)

/*----PDIR0 Tokens----*/
#define CSL_I2C_ICPDIR_PDIR0_IN          (0x00000000u)
#define CSL_I2C_ICPDIR_PDIR0_OUT         (0x00000001u)

#define CSL_I2C_ICPDIR_RESETVAL          (0x00000000u)

/* ICPDIN */

#define CSL_I2C_ICPDIN_PDIN1_MASK        (0x00000002u)
#define CSL_I2C_ICPDIN_PDIN1_SHIFT       (0x00000001u)
#define CSL_I2C_ICPDIN_PDIN1_RESETVAL    (0x00000000u)

/*----PDIN1 Tokens----*/
#define CSL_I2C_ICPDIN_PDIN1_LOW         (0x00000000u)
#define CSL_I2C_ICPDIN_PDIN1_HIGH        (0x00000001u)

#define CSL_I2C_ICPDIN_PDIN0_MASK        (0x00000001u)
#define CSL_I2C_ICPDIN_PDIN0_SHIFT       (0x00000000u)
#define CSL_I2C_ICPDIN_PDIN0_RESETVAL    (0x00000000u)

/*----PDIN0 Tokens----*/
#define CSL_I2C_ICPDIN_PDIN0_LOW         (0x00000000u)
#define CSL_I2C_ICPDIN_PDIN0_HIGH        (0x00000001u)

#define CSL_I2C_ICPDIN_RESETVAL          (0x00000000u)

/* ICPDOUT */

#define CSL_I2C_ICPDOUT_PDOUT1_MASK      (0x00000002u)
#define CSL_I2C_ICPDOUT_PDOUT1_SHIFT     (0x00000001u)
#define CSL_I2C_ICPDOUT_PDOUT1_RESETVAL  (0x00000000u)

/*----PDOUT1 Tokens----*/
#define CSL_I2C_ICPDOUT_PDOUT1_LOW       (0x00000000u)
#define CSL_I2C_ICPDOUT_PDOUT1_HIGH      (0x00000001u)

#define CSL_I2C_ICPDOUT_PDOUT0_MASK      (0x00000001u)
#define CSL_I2C_ICPDOUT_PDOUT0_SHIFT     (0x00000000u)
#define CSL_I2C_ICPDOUT_PDOUT0_RESETVAL  (0x00000000u)

/*----PDOUT0 Tokens----*/
#define CSL_I2C_ICPDOUT_PDOUT0_LOW       (0x00000000u)
#define CSL_I2C_ICPDOUT_PDOUT0_HIGH      (0x00000001u)

#define CSL_I2C_ICPDOUT_RESETVAL         (0x00000000u)

/* ICPDSET */

#define CSL_I2C_ICPDSET_PDSET1_MASK      (0x00000002u)
#define CSL_I2C_ICPDSET_PDSET1_SHIFT     (0x00000001u)
#define CSL_I2C_ICPDSET_PDSET1_RESETVAL  (0x00000000u)

/*----PDSET1 Tokens----*/
#define CSL_I2C_ICPDSET_PDSET1_NONE      (0x00000000u)
#define CSL_I2C_ICPDSET_PDSET1_SET       (0x00000001u)

#define CSL_I2C_ICPDSET_PDSET0_MASK      (0x00000001u)
#define CSL_I2C_ICPDSET_PDSET0_SHIFT     (0x00000000u)
#define CSL_I2C_ICPDSET_PDSET0_RESETVAL  (0x00000000u)

/*----PDSET0 Tokens----*/
#define CSL_I2C_ICPDSET_PDSET0_NONE      (0x00000000u)
#define CSL_I2C_ICPDSET_PDSET0_SET       (0x00000001u)

#define CSL_I2C_ICPDSET_RESETVAL         (0x00000000u)

/* ICPDCLR */

#define CSL_I2C_ICPDCLR_PDCLR1_MASK      (0x00000002u)
#define CSL_I2C_ICPDCLR_PDCLR1_SHIFT     (0x00000001u)
#define CSL_I2C_ICPDCLR_PDCLR1_RESETVAL  (0x00000000u)

/*----PDCLR1 Tokens----*/
#define CSL_I2C_ICPDCLR_PDCLR1_NONE      (0x00000000u)
#define CSL_I2C_ICPDCLR_PDCLR1_RESET     (0x00000001u)

#define CSL_I2C_ICPDCLR_PDCLR0_MASK      (0x00000001u)
#define CSL_I2C_ICPDCLR_PDCLR0_SHIFT     (0x00000000u)
#define CSL_I2C_ICPDCLR_PDCLR0_RESETVAL  (0x00000000u)

/*----PDCLR0 Tokens----*/
#define CSL_I2C_ICPDCLR_PDCLR0_NONE      (0x00000000u)
#define CSL_I2C_ICPDCLR_PDCLR0_RESET     (0x00000001u)

#define CSL_I2C_ICPDCLR_RESETVAL         (0x00000000u)

#endif
/* Rev.No.   Date/Time               ECN No.          Modifier      */
/* -------   ---------               -------          --------      */

/* 5         14 Jan 2005 13:32:30    5888             xkeshavm      */
/*                                                                  */
/* Uploaded the CSL0.57 JAN 2005 Release and built the library for ARM and DSP*/
/********************************************************************/
