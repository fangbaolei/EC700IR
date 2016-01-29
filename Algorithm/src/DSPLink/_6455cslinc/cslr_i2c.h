#ifndef _CSLR_I2C_1_H_
#define _CSLR_I2C_1_H_
/*********************************************************************
 * Copyright (C) 2003-2006 Texas Instruments Incorporated. 
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
    volatile Uint32 ICOAR;
    volatile Uint32 ICIMR;
    volatile Uint32 ICSTR;
    volatile Uint32 ICCLKL;
    volatile Uint32 ICCLKH;
    volatile Uint32 ICCNT;
    volatile Uint32 ICDRR;
    volatile Uint32 ICSAR;
    volatile Uint32 ICDXR;
    volatile Uint32 ICMDR;
    volatile Uint32 ICIVR;
    volatile Uint32 ICEMDR;
    volatile Uint32 ICPSC;
} CSL_I2cRegs;

typedef volatile CSL_I2cRegs             *CSL_I2cRegsOvly;

/**************************************************************************\
* Field Definition Macros
\**************************************************************************/

/* ICOAR */

#define CSL_I2C_ICOAR_RESERVED_MASK      (0xFFFF0000u)
#define CSL_I2C_ICOAR_RESERVED_SHIFT     (0x00000010u)
#define CSL_I2C_ICOAR_RESERVED_RESETVAL  (0x00000000u)

#define CSL_I2C_ICOAR_OADDR_MASK         (0x000003FFu)
#define CSL_I2C_ICOAR_OADDR_SHIFT        (0x00000000u)
#define CSL_I2C_ICOAR_OADDR_RESETVAL     (0x00000000u)

#define CSL_I2C_ICOAR_RESETVAL           (0x00000000u)

/* ICIMR */

#define CSL_I2C_ICIMR_RESERVED_MASK      (0xFFFF0000u)
#define CSL_I2C_ICIMR_RESERVED_SHIFT     (0x00000010u)
#define CSL_I2C_ICIMR_RESERVED_RESETVAL  (0x00000000u)

#define CSL_I2C_ICIMR_AAS_MASK           (0x00000040u)
#define CSL_I2C_ICIMR_AAS_SHIFT          (0x00000006u)
#define CSL_I2C_ICIMR_AAS_RESETVAL       (0x00000000u)

/*----AAS Tokens----*/
#define CSL_I2C_ICIMR_AAS_DISABLE        (0x00000000u)
#define CSL_I2C_ICIMR_AAS_ENABLE         (0x00000001u)

#define CSL_I2C_ICIMR_SCD_MASK           (0x00000020u)
#define CSL_I2C_ICIMR_SCD_SHIFT          (0x00000005u)
#define CSL_I2C_ICIMR_SCD_RESETVAL       (0x00000000u)

/*----SCD Tokens----*/
#define CSL_I2C_ICIMR_SCD_DISABLE        (0x00000000u)
#define CSL_I2C_ICIMR_SCD_ENABLE         (0x00000001u)

#define CSL_I2C_ICIMR_ICXRDY_MASK        (0x00000010u)
#define CSL_I2C_ICIMR_ICXRDY_SHIFT       (0x00000004u)
#define CSL_I2C_ICIMR_ICXRDY_RESETVAL    (0x00000000u)

/*----ICXRDY Tokens----*/
#define CSL_I2C_ICIMR_ICXRDY_DISABLE     (0x00000000u)
#define CSL_I2C_ICIMR_ICXRDY_ENABLE      (0x00000001u)

#define CSL_I2C_ICIMR_ICRRDY_MASK        (0x00000008u)
#define CSL_I2C_ICIMR_ICRRDY_SHIFT       (0x00000003u)
#define CSL_I2C_ICIMR_ICRRDY_RESETVAL    (0x00000000u)

/*----ICRRDY Tokens----*/
#define CSL_I2C_ICIMR_ICRRDY_DISABLE     (0x00000000u)
#define CSL_I2C_ICIMR_ICRRDY_ENABLE      (0x00000001u)

#define CSL_I2C_ICIMR_ARDY_MASK          (0x00000004u)
#define CSL_I2C_ICIMR_ARDY_SHIFT         (0x00000002u)
#define CSL_I2C_ICIMR_ARDY_RESETVAL      (0x00000000u)

/*----ARDY Tokens----*/
#define CSL_I2C_ICIMR_ARDY_DISABLE       (0x00000000u)
#define CSL_I2C_ICIMR_ARDY_ENABLE        (0x00000001u)

#define CSL_I2C_ICIMR_NACK_MASK          (0x00000002u)
#define CSL_I2C_ICIMR_NACK_SHIFT         (0x00000001u)
#define CSL_I2C_ICIMR_NACK_RESETVAL      (0x00000000u)

/*----NACK Tokens----*/
#define CSL_I2C_ICIMR_NACK_DISABLE       (0x00000000u)
#define CSL_I2C_ICIMR_NACK_ENABLE        (0x00000001u)

#define CSL_I2C_ICIMR_AL_MASK            (0x00000001u)
#define CSL_I2C_ICIMR_AL_SHIFT           (0x00000000u)
#define CSL_I2C_ICIMR_AL_RESETVAL        (0x00000000u)

/*----AL Tokens----*/
#define CSL_I2C_ICIMR_AL_DISABLE         (0x00000000u)
#define CSL_I2C_ICIMR_AL_ENABLE          (0x00000001u)

#define CSL_I2C_ICIMR_RESETVAL           (0x00000000u)

/* ICSTR */

#define CSL_I2C_ICSTR_RESERVED_MASK      (0xFFFF0000u)
#define CSL_I2C_ICSTR_RESERVED_SHIFT     (0x00000010u)
#define CSL_I2C_ICSTR_RESERVED_RESETVAL  (0x00000000u)

#define CSL_I2C_ICSTR_SDIR_MASK          (0x00004000u)
#define CSL_I2C_ICSTR_SDIR_SHIFT         (0x0000000Eu)
#define CSL_I2C_ICSTR_SDIR_RESETVAL      (0x00000000u)

#define CSL_I2C_ICSTR_NACKSNT_MASK       (0x00002000u)
#define CSL_I2C_ICSTR_NACKSNT_SHIFT      (0x0000000Du)
#define CSL_I2C_ICSTR_NACKSNT_RESETVAL   (0x00000000u)

#define CSL_I2C_ICSTR_BB_MASK            (0x00001000u)
#define CSL_I2C_ICSTR_BB_SHIFT           (0x0000000Cu)
#define CSL_I2C_ICSTR_BB_RESETVAL        (0x00000000u)

#define CSL_I2C_ICSTR_RSFULL_MASK        (0x00000800u)
#define CSL_I2C_ICSTR_RSFULL_SHIFT       (0x0000000Bu)
#define CSL_I2C_ICSTR_RSFULL_RESETVAL    (0x00000000u)

#define CSL_I2C_ICSTR_XSMT_MASK          (0x00000400u)
#define CSL_I2C_ICSTR_XSMT_SHIFT         (0x0000000Au)
#define CSL_I2C_ICSTR_XSMT_RESETVAL      (0x00000001u)

#define CSL_I2C_ICSTR_AAS_MASK           (0x00000200u)
#define CSL_I2C_ICSTR_AAS_SHIFT          (0x00000009u)
#define CSL_I2C_ICSTR_AAS_RESETVAL       (0x00000000u)

#define CSL_I2C_ICSTR_AD0_MASK           (0x00000100u)
#define CSL_I2C_ICSTR_AD0_SHIFT          (0x00000008u)
#define CSL_I2C_ICSTR_AD0_RESETVAL       (0x00000000u)

#define CSL_I2C_ICSTR_SCD_MASK           (0x00000020u)
#define CSL_I2C_ICSTR_SCD_SHIFT          (0x00000005u)
#define CSL_I2C_ICSTR_SCD_RESETVAL       (0x00000000u)

#define CSL_I2C_ICSTR_ICXRDY_MASK        (0x00000010u)
#define CSL_I2C_ICSTR_ICXRDY_SHIFT       (0x00000004u)
#define CSL_I2C_ICSTR_ICXRDY_RESETVAL    (0x00000001u)

#define CSL_I2C_ICSTR_ICRRDY_MASK        (0x00000008u)
#define CSL_I2C_ICSTR_ICRRDY_SHIFT       (0x00000003u)
#define CSL_I2C_ICSTR_ICRRDY_RESETVAL    (0x00000000u)

#define CSL_I2C_ICSTR_ARDY_MASK          (0x00000004u)
#define CSL_I2C_ICSTR_ARDY_SHIFT         (0x00000002u)
#define CSL_I2C_ICSTR_ARDY_RESETVAL      (0x00000000u)

#define CSL_I2C_ICSTR_NACK_MASK          (0x00000002u)
#define CSL_I2C_ICSTR_NACK_SHIFT         (0x00000001u)
#define CSL_I2C_ICSTR_NACK_RESETVAL      (0x00000000u)

#define CSL_I2C_ICSTR_AL_MASK            (0x00000001u)
#define CSL_I2C_ICSTR_AL_SHIFT           (0x00000000u)
#define CSL_I2C_ICSTR_AL_RESETVAL        (0x00000000u)

#define CSL_I2C_ICSTR_RESETVAL           (0x00000410u)

/* ICCLKL */

#define CSL_I2C_ICCLKL_RESERVED_MASK     (0xFFFF0000u)
#define CSL_I2C_ICCLKL_RESERVED_SHIFT    (0x00000010u)
#define CSL_I2C_ICCLKL_RESERVED_RESETVAL (0x00000000u)

#define CSL_I2C_ICCLKL_ICCL_MASK         (0x0000FFFFu)
#define CSL_I2C_ICCLKL_ICCL_SHIFT        (0x00000000u)
#define CSL_I2C_ICCLKL_ICCL_RESETVAL     (0x00000000u)

#define CSL_I2C_ICCLKL_RESETVAL          (0x00000000u)

/* ICCLKH */

#define CSL_I2C_ICCLKH_RESERVED_MASK     (0xFFFF0000u)
#define CSL_I2C_ICCLKH_RESERVED_SHIFT    (0x00000010u)
#define CSL_I2C_ICCLKH_RESERVED_RESETVAL (0x00000000u)

#define CSL_I2C_ICCLKH_ICCH_MASK         (0x0000FFFFu)
#define CSL_I2C_ICCLKH_ICCH_SHIFT        (0x00000000u)
#define CSL_I2C_ICCLKH_ICCH_RESETVAL     (0x00000000u)

#define CSL_I2C_ICCLKH_RESETVAL          (0x00000000u)

/* ICCNT */

#define CSL_I2C_ICCNT_RESERVED_MASK      (0xFFFF0000u)
#define CSL_I2C_ICCNT_RESERVED_SHIFT     (0x00000010u)
#define CSL_I2C_ICCNT_RESERVED_RESETVAL  (0x00000000u)

#define CSL_I2C_ICCNT_ICDC_MASK          (0x0000FFFFu)
#define CSL_I2C_ICCNT_ICDC_SHIFT         (0x00000000u)
#define CSL_I2C_ICCNT_ICDC_RESETVAL      (0x00000000u)

#define CSL_I2C_ICCNT_RESETVAL           (0x00000000u)

/* ICDRR */

#define CSL_I2C_ICDRR_RESERVED_MASK      (0xFFFF0000u)
#define CSL_I2C_ICDRR_RESERVED_SHIFT     (0x00000010u)
#define CSL_I2C_ICDRR_RESERVED_RESETVAL  (0x00000000u)

#define CSL_I2C_ICDRR_D_MASK             (0x000000FFu)
#define CSL_I2C_ICDRR_D_SHIFT            (0x00000000u)
#define CSL_I2C_ICDRR_D_RESETVAL         (0x00000000u)

#define CSL_I2C_ICDRR_RESETVAL           (0x00000000u)

/* ICSAR */

#define CSL_I2C_ICSAR_RESERVED_MASK      (0xFFFF0000u)
#define CSL_I2C_ICSAR_RESERVED_SHIFT     (0x00000010u)
#define CSL_I2C_ICSAR_RESERVED_RESETVAL  (0x00000000u)

#define CSL_I2C_ICSAR_SADDR_MASK         (0x000003FFu)
#define CSL_I2C_ICSAR_SADDR_SHIFT        (0x00000000u)
#define CSL_I2C_ICSAR_SADDR_RESETVAL     (0x000003FFu)

#define CSL_I2C_ICSAR_RESETVAL           (0x000003FFu)

/* ICDXR */

#define CSL_I2C_ICDXR_RESERVED_MASK      (0xFFFF0000u)
#define CSL_I2C_ICDXR_RESERVED_SHIFT     (0x00000010u)
#define CSL_I2C_ICDXR_RESERVED_RESETVAL  (0x00000000u)

#define CSL_I2C_ICDXR_D_MASK             (0x000000FFu)
#define CSL_I2C_ICDXR_D_SHIFT            (0x00000000u)
#define CSL_I2C_ICDXR_D_RESETVAL         (0x00000000u)

#define CSL_I2C_ICDXR_RESETVAL           (0x00000000u)

/* ICMDR */

#define CSL_I2C_ICMDR_RESERVED_MASK      (0xFFFF0000u)
#define CSL_I2C_ICMDR_RESERVED_SHIFT     (0x00000010u)
#define CSL_I2C_ICMDR_RESERVED_RESETVAL  (0x00000000u)

#define CSL_I2C_ICMDR_NACKMOD_MASK       (0x00008000u)
#define CSL_I2C_ICMDR_NACKMOD_SHIFT      (0x0000000Fu)
#define CSL_I2C_ICMDR_NACKMOD_RESETVAL   (0x00000000u)

#define CSL_I2C_ICMDR_FREE_MASK          (0x00004000u)
#define CSL_I2C_ICMDR_FREE_SHIFT         (0x0000000Eu)
#define CSL_I2C_ICMDR_FREE_RESETVAL      (0x00000000u)

#define CSL_I2C_ICMDR_STT_MASK           (0x00002000u)
#define CSL_I2C_ICMDR_STT_SHIFT          (0x0000000Du)
#define CSL_I2C_ICMDR_STT_RESETVAL       (0x00000000u)

#define CSL_I2C_ICMDR_STP_MASK           (0x00000800u)
#define CSL_I2C_ICMDR_STP_SHIFT          (0x0000000Bu)
#define CSL_I2C_ICMDR_STP_RESETVAL       (0x00000000u)

#define CSL_I2C_ICMDR_MST_MASK           (0x00000400u)
#define CSL_I2C_ICMDR_MST_SHIFT          (0x0000000Au)
#define CSL_I2C_ICMDR_MST_RESETVAL       (0x00000000u)

#define CSL_I2C_ICMDR_TRX_MASK           (0x00000200u)
#define CSL_I2C_ICMDR_TRX_SHIFT          (0x00000009u)
#define CSL_I2C_ICMDR_TRX_RESETVAL       (0x00000000u)

#define CSL_I2C_ICMDR_XA_MASK            (0x00000100u)
#define CSL_I2C_ICMDR_XA_SHIFT           (0x00000008u)
#define CSL_I2C_ICMDR_XA_RESETVAL        (0x00000000u)

#define CSL_I2C_ICMDR_RM_MASK            (0x00000080u)
#define CSL_I2C_ICMDR_RM_SHIFT           (0x00000007u)
#define CSL_I2C_ICMDR_RM_RESETVAL        (0x00000000u)

#define CSL_I2C_ICMDR_DLB_MASK           (0x00000040u)
#define CSL_I2C_ICMDR_DLB_SHIFT          (0x00000006u)
#define CSL_I2C_ICMDR_DLB_RESETVAL       (0x00000000u)

#define CSL_I2C_ICMDR_IRS_MASK           (0x00000020u)
#define CSL_I2C_ICMDR_IRS_SHIFT          (0x00000005u)
#define CSL_I2C_ICMDR_IRS_RESETVAL       (0x00000000u)

#define CSL_I2C_ICMDR_STB_MASK           (0x00000010u)
#define CSL_I2C_ICMDR_STB_SHIFT          (0x00000004u)
#define CSL_I2C_ICMDR_STB_RESETVAL       (0x00000000u)

#define CSL_I2C_ICMDR_FDF_MASK           (0x00000008u)
#define CSL_I2C_ICMDR_FDF_SHIFT          (0x00000003u)
#define CSL_I2C_ICMDR_FDF_RESETVAL       (0x00000000u)

#define CSL_I2C_ICMDR_BC_MASK            (0x00000007u)
#define CSL_I2C_ICMDR_BC_SHIFT           (0x00000000u)
#define CSL_I2C_ICMDR_BC_RESETVAL        (0x00000000u)

#define CSL_I2C_ICMDR_RESETVAL           (0x00000000u)

/* ICIVR */

#define CSL_I2C_ICIVR_RESERVED_MASK      (0xFFFF0000u)
#define CSL_I2C_ICIVR_RESERVED_SHIFT     (0x00000010u)
#define CSL_I2C_ICIVR_RESERVED_RESETVAL  (0x00000000u)

#define CSL_I2C_ICIVR_TESTMD_MASK        (0x00000F00u)
#define CSL_I2C_ICIVR_TESTMD_SHIFT       (0x00000008u)
#define CSL_I2C_ICIVR_TESTMD_RESETVAL    (0x00000000u)

#define CSL_I2C_ICIVR_INTCODE_MASK       (0x00000007u)
#define CSL_I2C_ICIVR_INTCODE_SHIFT      (0x00000000u)
#define CSL_I2C_ICIVR_INTCODE_RESETVAL   (0x00000000u)

/*----INTCODE Tokens----*/
#define CSL_I2C_ICIVR_INTCODE_NONE       (0x00000000u)
#define CSL_I2C_ICIVR_INTCODE_AL         (0x00000001u)
#define CSL_I2C_ICIVR_INTCODE_NACK       (0x00000002u)
#define CSL_I2C_ICIVR_INTCODE_RAR        (0x00000003u)
#define CSL_I2C_ICIVR_INTCODE_RDR        (0x00000004u)
#define CSL_I2C_ICIVR_INTCODE_TDR        (0x00000005u)
#define CSL_I2C_ICIVR_INTCODE_SCD        (0x00000006u)
#define CSL_I2C_ICIVR_INTCODE_AAS        (0x00000007u)

#define CSL_I2C_ICIVR_RESETVAL           (0x00000000u)

/* ICEMDR */

#define CSL_I2C_ICEMDR_RESERVED_MASK     (0xFFFF0000u)
#define CSL_I2C_ICEMDR_RESERVED_SHIFT    (0x00000010u)
#define CSL_I2C_ICEMDR_RESERVED_RESETVAL (0x00000000u)

#define CSL_I2C_ICEMDR_BCM_MASK          (0x00000001u)
#define CSL_I2C_ICEMDR_BCM_SHIFT         (0x00000000u)
#define CSL_I2C_ICEMDR_BCM_RESETVAL      (0x00000001u)

#define CSL_I2C_ICEMDR_RESETVAL          (0x00000001u)

/* ICPSC */

#define CSL_I2C_ICPSC_RESERVED_MASK      (0xFFFF0000u)
#define CSL_I2C_ICPSC_RESERVED_SHIFT     (0x00000010u)
#define CSL_I2C_ICPSC_RESERVED_RESETVAL  (0x00000000u)

#define CSL_I2C_ICPSC_IPSC_MASK          (0x000000FFu)
#define CSL_I2C_ICPSC_IPSC_SHIFT         (0x00000000u)
#define CSL_I2C_ICPSC_IPSC_RESETVAL      (0x00000000u)

#define CSL_I2C_ICPSC_RESETVAL           (0x00000000u)

#endif
