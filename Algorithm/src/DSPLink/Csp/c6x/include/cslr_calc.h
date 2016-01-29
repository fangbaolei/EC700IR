/***************************************************************************\
* THE FOLLOWING REGISTERS or REGISTER'S bit fields ARE NOT VALID(RESERVED) * 
* IN KALEIDO11                                                             *
*                                                                          *  
* -> MODE Regsiter's Bit 16                                                * 
*                                                                          *  
\**************************************************************************/

#ifndef _CSLR_CALC_1_H_
#define _CSLR_CALC_1_H_

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
    volatile Uint32 RSVD0;
    volatile Uint32 MODE;
} CSL_CalcRegs;

/**************************************************************************\
* Field Definition Macros
\**************************************************************************/

/* PID */

#define CSL_CALC_PID_SCHEME_MASK         (0xC0000000u)
#define CSL_CALC_PID_SCHEME_SHIFT        (0x0000001Eu)
#define CSL_CALC_PID_SCHEME_RESETVAL     (0x00000001u)


#define CSL_CALC_PID_FUNC_MASK           (0x0FFF0000u)
#define CSL_CALC_PID_FUNC_SHIFT          (0x00000010u)
#define CSL_CALC_PID_FUNC_RESETVAL       (0x00000C02u)

#define CSL_CALC_PID_RTL_MASK            (0x0000F800u)
#define CSL_CALC_PID_RTL_SHIFT           (0x0000000Bu)
#define CSL_CALC_PID_RTL_RESETVAL        (0x00000000u)

#define CSL_CALC_PID_MAJOR_MASK          (0x00000700u)
#define CSL_CALC_PID_MAJOR_SHIFT         (0x00000008u)
#define CSL_CALC_PID_MAJOR_RESETVAL      (0x00000001u)

#define CSL_CALC_PID_CUSTOM_MASK         (0x000000C0u)
#define CSL_CALC_PID_CUSTOM_SHIFT        (0x00000006u)
#define CSL_CALC_PID_CUSTOM_RESETVAL     (0x00000000u)

#define CSL_CALC_PID_MINOR_MASK          (0x0000003Fu)
#define CSL_CALC_PID_MINOR_SHIFT         (0x00000000u)
#define CSL_CALC_PID_MINOR_RESETVAL      (0x00000000u)

#define CSL_CALC_PID_RESETVAL            (0x4C020100u)

/* COUNT */

#define CSL_CALC_COUNT_EN_MASK           (0x80000000u)
#define CSL_CALC_COUNT_EN_SHIFT          (0x0000001Fu)
#define CSL_CALC_COUNT_EN_RESETVAL       (0x00000000u)
/*----EN Tokens----*/
#define CSL_CALC_COUNT_EN_DISABLE        (0x00000000u)
#define CSL_CALC_COUNT_EN_ENABLE         (0x00000001u)

#define CSL_CALC_COUNT_RST_MASK          (0x40000000u)
#define CSL_CALC_COUNT_RST_SHIFT         (0x0000001Eu)
#define CSL_CALC_COUNT_RST_RESETVAL      (0x00000000u)
/*----RST Tokens----*/
#define CSL_CALC_COUNT_RST_SET           (0x00000001u)


#define CSL_CALC_COUNT_VALUE_MASK        (0x0000FFFFu)
#define CSL_CALC_COUNT_VALUE_SHIFT       (0x00000000u)
#define CSL_CALC_COUNT_VALUE_RESETVAL    (0x00000000u)

#define CSL_CALC_COUNT_RESETVAL          (0x00000000u)

/* CTRL */

#define CSL_CALC_CTRL_CMS_MASK           (0x80000000u)
#define CSL_CALC_CTRL_CMS_SHIFT          (0x0000001Fu)
#define CSL_CALC_CTRL_CMS_RESETVAL       (0x00000000u)


#define CSL_CALC_CTRL_ADDR_UP10_MASK     (0x001FF800u)
#define CSL_CALC_CTRL_ADDR_UP10_SHIFT    (0x0000000Bu)
#define CSL_CALC_CTRL_ADDR_UP10_RESETVAL (0x00000000u)

#define CSL_CALC_CTRL_ADDR_LOW_MASK      (0x00000700u)
#define CSL_CALC_CTRL_ADDR_LOW_SHIFT     (0x00000008u)
#define CSL_CALC_CTRL_ADDR_LOW_RESETVAL  (0x00000000u)


#define CSL_CALC_CTRL_SSM_MASK           (0x00000004u)
#define CSL_CALC_CTRL_SSM_SHIFT          (0x00000002u)
#define CSL_CALC_CTRL_SSM_RESETVAL       (0x00000000u)
/*----SSM Tokens----*/
#define CSL_CALC_CTRL_SSM_NORMAL         (0x00000000u)
#define CSL_CALC_CTRL_SSM_SSP            (0x00000001u)

#define CSL_CALC_CTRL_CSB_MASK           (0x00000002u)
#define CSL_CALC_CTRL_CSB_SHIFT          (0x00000001u)
#define CSL_CALC_CTRL_CSB_RESETVAL       (0x00000000u)
/*----CSB Tokens----*/
#define CSL_CALC_CTRL_CSB_DEFINE         (0x00000000u)

#define CSL_CALC_CTRL_EN_MASK            (0x00000001u)
#define CSL_CALC_CTRL_EN_SHIFT           (0x00000000u)
#define CSL_CALC_CTRL_EN_RESETVAL        (0x00000000u)
/*----EN Tokens----*/
#define CSL_CALC_CTRL_EN_SET             (0x00000001u)

#define CSL_CALC_CTRL_RESETVAL           (0x00000000u)

/* MODE */


#define CSL_CALC_MODE_SEQ_MASK           (0x00070000u)
#define CSL_CALC_MODE_SEQ_SHIFT          (0x00000010u)
#define CSL_CALC_MODE_SEQ_RESETVAL       (0x00000000u)
/*----SEQ Tokens----*/
#define CSL_CALC_MODE_SEQ_NSIP_DEC       (0x00000000u)
#define CSL_CALC_MODE_SEQ_NSIP_ENC       (0x00000001u)
#define CSL_CALC_MODE_SEQ_SIP_DEC        (0x00000002u)
#define CSL_CALC_MODE_SEQ_SIP_ENC        (0x00000003u)
#define CSL_CALC_MODE_SEQ_JPEG_DEC       (0x00000004u)
#define CSL_CALC_MODE_SEQ_JPEG_ENC       (0x00000005u)


#define CSL_CALC_MODE_RECON_MASK         (0x00000100u)
#define CSL_CALC_MODE_RECON_SHIFT        (0x00000008u)
#define CSL_CALC_MODE_RECON_RESETVAL     (0x00000000u)
/*----RECON Tokens----*/
#define CSL_CALC_MODE_RECON_8BIT         (0x00000000u)
#define CSL_CALC_MODE_RECON_16BIT        (0x00000001u)


#define CSL_CALC_MODE_PICTURE_MASK       (0x00000003u)
#define CSL_CALC_MODE_PICTURE_SHIFT      (0x00000000u)
#define CSL_CALC_MODE_PICTURE_RESETVAL   (0x00000000u)
/*----PICTURE Tokens----*/
#define CSL_CALC_MODE_PICTURE_FRAME      (0x00000000u)
#define CSL_CALC_MODE_PICTURE_FIELD      (0x00000001u)
#define CSL_CALC_MODE_PICTURE_AFF        (0x00000002u)

#define CSL_CALC_MODE_RESETVAL           (0x00000000u)

#endif
