/*********************************************************************
 * Copyright (C) 2003-2005 Texas Instruments Incorporated. 
 * All Rights Reserved 
 *********************************************************************/
 /** \file cslr_chip.h

 * 
 * \brief This file contains the Register Desciptions for CHIP
 * 
 *********************************************************************/

#ifndef _CSLR_CHIP_H_
#define _CSLR_CHIP_H_

#include <cslr.h>

#include <tistdtypes.h>


/**************************************************************************\
* Field Definition Macros
\**************************************************************************/

/* AMR */

#define CSL_CHIP_AMR_BK1_MASK            (0x03E00000u)
#define CSL_CHIP_AMR_BK1_SHIFT           (0x00000015u)
#define CSL_CHIP_AMR_BK1_RESETVAL        (0x00000000u)

#define CSL_CHIP_AMR_BK0_MASK            (0x001F0000u)
#define CSL_CHIP_AMR_BK0_SHIFT           (0x00000010u)
#define CSL_CHIP_AMR_BK0_RESETVAL        (0x00000000u)

#define CSL_CHIP_AMR_B7MODE_MASK         (0x0000C000u)
#define CSL_CHIP_AMR_B7MODE_SHIFT        (0x0000000Eu)
#define CSL_CHIP_AMR_B7MODE_RESETVAL     (0x00000000u)

/*----B7MODE Tokens----*/
#define CSL_CHIP_AMR_B7MODE_LINEAR       (0x00000000u)
#define CSL_CHIP_AMR_B7MODE_CIRCULARBK0  (0x00000001u)
#define CSL_CHIP_AMR_B7MODE_CIRCULARBK1  (0x00000002u)
#define CSL_CHIP_AMR_B7MODE_RESV         (0x00000003u)

#define CSL_CHIP_AMR_B6MODE_MASK         (0x00003000u)
#define CSL_CHIP_AMR_B6MODE_SHIFT        (0x0000000Cu)
#define CSL_CHIP_AMR_B6MODE_RESETVAL     (0x00000000u)

/*----B6MODE Tokens----*/
#define CSL_CHIP_AMR_B6MODE_LINEAR       (0x00000000u)
#define CSL_CHIP_AMR_B6MODE_CIRCULARBK0  (0x00000001u)
#define CSL_CHIP_AMR_B6MODE_CIRCULARBK1  (0x00000002u)
#define CSL_CHIP_AMR_B6MODE_RESV         (0x00000003u)

#define CSL_CHIP_AMR_B5MODE_MASK         (0x00000C00u)
#define CSL_CHIP_AMR_B5MODE_SHIFT        (0x0000000Au)
#define CSL_CHIP_AMR_B5MODE_RESETVAL     (0x00000000u)

/*----B5MODE Tokens----*/
#define CSL_CHIP_AMR_B5MODE_LINEAR       (0x00000000u)
#define CSL_CHIP_AMR_B5MODE_CIRCULARBK1  (0x00000001u)
#define CSL_CHIP_AMR_B5MODE_CIRCULARBK2  (0x00000002u)
#define CSL_CHIP_AMR_B5MODE_RESV         (0x00000003u)

#define CSL_CHIP_AMR_B4MODE_MASK         (0x00000300u)
#define CSL_CHIP_AMR_B4MODE_SHIFT        (0x00000008u)
#define CSL_CHIP_AMR_B4MODE_RESETVAL     (0x00000000u)

/*----B4MODE Tokens----*/
#define CSL_CHIP_AMR_B4MODE_LINEAR       (0x00000000u)
#define CSL_CHIP_AMR_B4MODE_CIRCULARBK1  (0x00000001u)
#define CSL_CHIP_AMR_B4MODE_CIRCULARBK2  (0x00000002u)
#define CSL_CHIP_AMR_B4MODE_RESV         (0x00000003u)

#define CSL_CHIP_AMR_A7MODE_MASK         (0x000000C0u)
#define CSL_CHIP_AMR_A7MODE_SHIFT        (0x00000006u)
#define CSL_CHIP_AMR_A7MODE_RESETVAL     (0x00000000u)

/*----A7MODE Tokens----*/
#define CSL_CHIP_AMR_A7MODE_LINEAR       (0x00000000u)
#define CSL_CHIP_AMR_A7MODE_CIRCULARBK1  (0x00000001u)
#define CSL_CHIP_AMR_A7MODE_CIRCULARBK2  (0x00000002u)
#define CSL_CHIP_AMR_A7MODE_RESV         (0x00000003u)

#define CSL_CHIP_AMR_A6MODE_MASK         (0x00000030u)
#define CSL_CHIP_AMR_A6MODE_SHIFT        (0x00000004u)
#define CSL_CHIP_AMR_A6MODE_RESETVAL     (0x00000000u)

/*----A6MODE Tokens----*/
#define CSL_CHIP_AMR_A6MODE_LINEAR       (0x00000000u)
#define CSL_CHIP_AMR_A6MODE_CIRCULARBK2  (0x00000001u)
#define CSL_CHIP_AMR_A6MODE_CIRCULARBK3  (0x00000002u)
#define CSL_CHIP_AMR_A6MODE_RESV         (0x00000003u)

#define CSL_CHIP_AMR_A5MODE_MASK         (0x0000000Cu)
#define CSL_CHIP_AMR_A5MODE_SHIFT        (0x00000002u)
#define CSL_CHIP_AMR_A5MODE_RESETVAL     (0x00000000u)

/*----A5MODE Tokens----*/
#define CSL_CHIP_AMR_A5MODE_LINEAR       (0x00000000u)
#define CSL_CHIP_AMR_A5MODE_CIRCULARBK3  (0x00000001u)
#define CSL_CHIP_AMR_A5MODE_CIRCULARBK4  (0x00000002u)
#define CSL_CHIP_AMR_A5MODE_RESV         (0x00000003u)

#define CSL_CHIP_AMR_A4MODE_MASK         (0x00000003u)
#define CSL_CHIP_AMR_A4MODE_SHIFT        (0x00000000u)
#define CSL_CHIP_AMR_A4MODE_RESETVAL     (0x00000000u)

/*----A4MODE Tokens----*/
#define CSL_CHIP_AMR_A4MODE_LINEAR       (0x00000000u)
#define CSL_CHIP_AMR_A4MODE_CIRCULARBK4  (0x00000001u)
#define CSL_CHIP_AMR_A4MODE_CIRCULARBK5  (0x00000002u)
#define CSL_CHIP_AMR_A4MODE_RESV         (0x00000003u)

#define CSL_CHIP_AMR_RESETVAL            (0x00000000u)

/* CSR */

#define CSL_CHIP_CSR_CPU_ID_MASK         (0xFF000000u)
#define CSL_CHIP_CSR_CPU_ID_SHIFT        (0x00000018u)
#define CSL_CHIP_CSR_CPU_ID_RESETVAL     (0x00000000u)

#define CSL_CHIP_CSR_REV_ID_MASK         (0x00FF0000u)
#define CSL_CHIP_CSR_REV_ID_SHIFT        (0x00000010u)
#define CSL_CHIP_CSR_REV_ID_RESETVAL     (0x00000801u)

#define CSL_CHIP_CSR_PWRD_MASK           (0x0000FC00u)
#define CSL_CHIP_CSR_PWRD_SHIFT          (0x0000000Au)
#define CSL_CHIP_CSR_PWRD_RESETVAL       (0x00000000u)

#define CSL_CHIP_CSR_SAT_MASK            (0x00000200u)
#define CSL_CHIP_CSR_SAT_SHIFT           (0x00000009u)
#define CSL_CHIP_CSR_SAT_RESETVAL        (0x00000000u)

/*----SAT Tokens----*/
#define CSL_CHIP_CSR_SAT_SATURATE        (0x00000001u)
#define CSL_CHIP_CSR_SAT_NONSATURATE     (0x00000000u)

#define CSL_CHIP_CSR_EN_MASK             (0x00000100u)
#define CSL_CHIP_CSR_EN_SHIFT            (0x00000008u)
#define CSL_CHIP_CSR_EN_RESETVAL         (0x00000000u)

/*----EN Tokens----*/
#define CSL_CHIP_CSR_EN_BIG              (0x00000000u)
#define CSL_CHIP_CSR_EN_LITTLE           (0x00000001u)

#define CSL_CHIP_CSR_PCC_MASK            (0x000000E0u)
#define CSL_CHIP_CSR_PCC_SHIFT           (0x00000005u)
#define CSL_CHIP_CSR_PCC_RESETVAL        (0x00000000u)

#define CSL_CHIP_CSR_DCC_MASK            (0x0000001Cu)
#define CSL_CHIP_CSR_DCC_SHIFT           (0x00000002u)
#define CSL_CHIP_CSR_DCC_RESETVAL        (0x00000000u)

#define CSL_CHIP_CSR_PGIE_MASK           (0x00000002u)
#define CSL_CHIP_CSR_PGIE_SHIFT          (0x00000001u)
#define CSL_CHIP_CSR_PGIE_RESETVAL       (0x00000000u)

#define CSL_CHIP_CSR_GIE_MASK            (0x00000001u)
#define CSL_CHIP_CSR_GIE_SHIFT           (0x00000000u)
#define CSL_CHIP_CSR_GIE_RESETVAL        (0x00000000u)

/*----GIE Tokens----*/
#define CSL_CHIP_CSR_GIE_ENABLE          (0x00000001u)
#define CSL_CHIP_CSR_GIE_DISABLE         (0x00000000u)

#define CSL_CHIP_CSR_RESETVAL            (0x08010000u)

/* IFR */

#define CSL_CHIP_IFR_IF15_MASK           (0x00008000u)
#define CSL_CHIP_IFR_IF15_SHIFT          (0x0000000Fu)
#define CSL_CHIP_IFR_IF15_RESETVAL       (0x00000000u)

/*----IF15 Tokens----*/
#define CSL_CHIP_IFR_IF15_ENABLE         (0x00000001u)

#define CSL_CHIP_IFR_IF14_MASK           (0x00004000u)
#define CSL_CHIP_IFR_IF14_SHIFT          (0x0000000Eu)
#define CSL_CHIP_IFR_IF14_RESETVAL       (0x00000000u)

#define CSL_CHIP_IFR_IF13_MASK           (0x00002000u)
#define CSL_CHIP_IFR_IF13_SHIFT          (0x0000000Du)
#define CSL_CHIP_IFR_IF13_RESETVAL       (0x00000000u)

#define CSL_CHIP_IFR_IF12_MASK           (0x00001000u)
#define CSL_CHIP_IFR_IF12_SHIFT          (0x0000000Cu)
#define CSL_CHIP_IFR_IF12_RESETVAL       (0x00000000u)

#define CSL_CHIP_IFR_IF11_MASK           (0x00000800u)
#define CSL_CHIP_IFR_IF11_SHIFT          (0x0000000Bu)
#define CSL_CHIP_IFR_IF11_RESETVAL       (0x00000000u)

#define CSL_CHIP_IFR_IF10_MASK           (0x00000400u)
#define CSL_CHIP_IFR_IF10_SHIFT          (0x0000000Au)
#define CSL_CHIP_IFR_IF10_RESETVAL       (0x00000000u)

#define CSL_CHIP_IFR_IF9_MASK            (0x00000200u)
#define CSL_CHIP_IFR_IF9_SHIFT           (0x00000009u)
#define CSL_CHIP_IFR_IF9_RESETVAL        (0x00000000u)

#define CSL_CHIP_IFR_IF8_MASK            (0x00000100u)
#define CSL_CHIP_IFR_IF8_SHIFT           (0x00000008u)
#define CSL_CHIP_IFR_IF8_RESETVAL        (0x00000000u)

#define CSL_CHIP_IFR_IF7_MASK            (0x00000080u)
#define CSL_CHIP_IFR_IF7_SHIFT           (0x00000007u)
#define CSL_CHIP_IFR_IF7_RESETVAL        (0x00000000u)

#define CSL_CHIP_IFR_IF6_MASK            (0x00000040u)
#define CSL_CHIP_IFR_IF6_SHIFT           (0x00000006u)
#define CSL_CHIP_IFR_IF6_RESETVAL        (0x00000000u)

#define CSL_CHIP_IFR_IF5_MASK            (0x00000020u)
#define CSL_CHIP_IFR_IF5_SHIFT           (0x00000005u)
#define CSL_CHIP_IFR_IF5_RESETVAL        (0x00000000u)

#define CSL_CHIP_IFR_IF4_MASK            (0x00000010u)
#define CSL_CHIP_IFR_IF4_SHIFT           (0x00000004u)
#define CSL_CHIP_IFR_IF4_RESETVAL        (0x00000000u)

#define CSL_CHIP_IFR_NMIF_MASK           (0x00000002u)
#define CSL_CHIP_IFR_NMIF_SHIFT          (0x00000001u)
#define CSL_CHIP_IFR_NMIF_RESETVAL       (0x00000000u)

#define CSL_CHIP_IFR_RESETVAL            (0x00000000u)

/* ISR */

#define CSL_CHIP_ISR_IS15_MASK           (0x00008000u)
#define CSL_CHIP_ISR_IS15_SHIFT          (0x0000000Fu)
#define CSL_CHIP_ISR_IS15_RESETVAL       (0x00000000u)

/*----IS15 Tokens----*/
#define CSL_CHIP_ISR_IS15_SET            (0x00000001u)

#define CSL_CHIP_ISR_IS14_MASK           (0x00004000u)
#define CSL_CHIP_ISR_IS14_SHIFT          (0x0000000Eu)
#define CSL_CHIP_ISR_IS14_RESETVAL       (0x00000000u)

/*----IS14 Tokens----*/
#define CSL_CHIP_ISR_IS14_SET            (0x00000001u)

#define CSL_CHIP_ISR_IS13_MASK           (0x00002000u)
#define CSL_CHIP_ISR_IS13_SHIFT          (0x0000000Du)
#define CSL_CHIP_ISR_IS13_RESETVAL       (0x00000000u)

/*----IS13 Tokens----*/
#define CSL_CHIP_ISR_IS13_SET            (0x00000001u)

#define CSL_CHIP_ISR_IS12_MASK           (0x00001000u)
#define CSL_CHIP_ISR_IS12_SHIFT          (0x0000000Cu)
#define CSL_CHIP_ISR_IS12_RESETVAL       (0x00000000u)

/*----IS12 Tokens----*/
#define CSL_CHIP_ISR_IS12_SET            (0x00000001u)

#define CSL_CHIP_ISR_IS11_MASK           (0x00000800u)
#define CSL_CHIP_ISR_IS11_SHIFT          (0x0000000Bu)
#define CSL_CHIP_ISR_IS11_RESETVAL       (0x00000000u)

/*----IS11 Tokens----*/
#define CSL_CHIP_ISR_IS11_SET            (0x00000001u)

#define CSL_CHIP_ISR_IS10_MASK           (0x00000400u)
#define CSL_CHIP_ISR_IS10_SHIFT          (0x0000000Au)
#define CSL_CHIP_ISR_IS10_RESETVAL       (0x00000000u)

/*----IS10 Tokens----*/
#define CSL_CHIP_ISR_IS10_SET            (0x00000001u)

#define CSL_CHIP_ISR_IS9_MASK            (0x00000200u)
#define CSL_CHIP_ISR_IS9_SHIFT           (0x00000009u)
#define CSL_CHIP_ISR_IS9_RESETVAL        (0x00000000u)

/*----IS9 Tokens----*/
#define CSL_CHIP_ISR_IS9_SET             (0x00000001u)

#define CSL_CHIP_ISR_IS8_MASK            (0x00000100u)
#define CSL_CHIP_ISR_IS8_SHIFT           (0x00000008u)
#define CSL_CHIP_ISR_IS8_RESETVAL        (0x00000000u)

/*----IS8 Tokens----*/
#define CSL_CHIP_ISR_IS8_SET             (0x00000001u)

#define CSL_CHIP_ISR_IS7_MASK            (0x00000080u)
#define CSL_CHIP_ISR_IS7_SHIFT           (0x00000007u)
#define CSL_CHIP_ISR_IS7_RESETVAL        (0x00000000u)

/*----IS7 Tokens----*/
#define CSL_CHIP_ISR_IS7_SET             (0x00000001u)

#define CSL_CHIP_ISR_IS6_MASK            (0x00000040u)
#define CSL_CHIP_ISR_IS6_SHIFT           (0x00000006u)
#define CSL_CHIP_ISR_IS6_RESETVAL        (0x00000000u)

/*----IS6 Tokens----*/
#define CSL_CHIP_ISR_IS6_SET             (0x00000001u)

#define CSL_CHIP_ISR_IS5_MASK            (0x00000020u)
#define CSL_CHIP_ISR_IS5_SHIFT           (0x00000005u)
#define CSL_CHIP_ISR_IS5_RESETVAL        (0x00000000u)

/*----IS5 Tokens----*/
#define CSL_CHIP_ISR_IS5_SET             (0x00000001u)

#define CSL_CHIP_ISR_IS4_MASK            (0x00000010u)
#define CSL_CHIP_ISR_IS4_SHIFT           (0x00000004u)
#define CSL_CHIP_ISR_IS4_RESETVAL        (0x00000000u)

/*----IS4 Tokens----*/
#define CSL_CHIP_ISR_IS4_SET             (0x00000001u)

#define CSL_CHIP_ISR_RESETVAL            (0x00000000u)

/* ICR */

#define CSL_CHIP_ICR_IC15_MASK           (0x00008000u)
#define CSL_CHIP_ICR_IC15_SHIFT          (0x0000000Fu)
#define CSL_CHIP_ICR_IC15_RESETVAL       (0x00000000u)

/*----IC15 Tokens----*/
#define CSL_CHIP_ICR_IC15_CLR            (0x00000001u)

#define CSL_CHIP_ICR_IC14_MASK           (0x00004000u)
#define CSL_CHIP_ICR_IC14_SHIFT          (0x0000000Eu)
#define CSL_CHIP_ICR_IC14_RESETVAL       (0x00000000u)

/*----IC14 Tokens----*/
#define CSL_CHIP_ICR_IC14_CLR            (0x00000001u)

#define CSL_CHIP_ICR_IC13_MASK           (0x00002000u)
#define CSL_CHIP_ICR_IC13_SHIFT          (0x0000000Du)
#define CSL_CHIP_ICR_IC13_RESETVAL       (0x00000000u)

/*----IC13 Tokens----*/
#define CSL_CHIP_ICR_IC13_CLR            (0x00000001u)

#define CSL_CHIP_ICR_IC12_MASK           (0x00001000u)
#define CSL_CHIP_ICR_IC12_SHIFT          (0x0000000Cu)
#define CSL_CHIP_ICR_IC12_RESETVAL       (0x00000000u)

/*----IC12 Tokens----*/
#define CSL_CHIP_ICR_IC12_CLR            (0x00000001u)

#define CSL_CHIP_ICR_IC11_MASK           (0x00000800u)
#define CSL_CHIP_ICR_IC11_SHIFT          (0x0000000Bu)
#define CSL_CHIP_ICR_IC11_RESETVAL       (0x00000000u)

/*----IC11 Tokens----*/
#define CSL_CHIP_ICR_IC11_CLR            (0x00000001u)

#define CSL_CHIP_ICR_IC10_MASK           (0x00000400u)
#define CSL_CHIP_ICR_IC10_SHIFT          (0x0000000Au)
#define CSL_CHIP_ICR_IC10_RESETVAL       (0x00000000u)

/*----IC10 Tokens----*/
#define CSL_CHIP_ICR_IC10_CLR            (0x00000001u)

#define CSL_CHIP_ICR_IC9_MASK            (0x00000200u)
#define CSL_CHIP_ICR_IC9_SHIFT           (0x00000009u)
#define CSL_CHIP_ICR_IC9_RESETVAL        (0x00000000u)

/*----IC9 Tokens----*/
#define CSL_CHIP_ICR_IC9_CLR             (0x00000001u)

#define CSL_CHIP_ICR_IC8_MASK            (0x00000100u)
#define CSL_CHIP_ICR_IC8_SHIFT           (0x00000008u)
#define CSL_CHIP_ICR_IC8_RESETVAL        (0x00000000u)

/*----IC8 Tokens----*/
#define CSL_CHIP_ICR_IC8_CLR             (0x00000001u)

#define CSL_CHIP_ICR_IC7_MASK            (0x00000080u)
#define CSL_CHIP_ICR_IC7_SHIFT           (0x00000007u)
#define CSL_CHIP_ICR_IC7_RESETVAL        (0x00000000u)

/*----IC7 Tokens----*/
#define CSL_CHIP_ICR_IC7_CLR             (0x00000001u)

#define CSL_CHIP_ICR_IC6_MASK            (0x00000040u)
#define CSL_CHIP_ICR_IC6_SHIFT           (0x00000006u)
#define CSL_CHIP_ICR_IC6_RESETVAL        (0x00000000u)

/*----IC6 Tokens----*/
#define CSL_CHIP_ICR_IC6_CLR             (0x00000001u)

#define CSL_CHIP_ICR_IC5_MASK            (0x00000020u)
#define CSL_CHIP_ICR_IC5_SHIFT           (0x00000005u)
#define CSL_CHIP_ICR_IC5_RESETVAL        (0x00000000u)

/*----IC5 Tokens----*/
#define CSL_CHIP_ICR_IC5_CLR             (0x00000001u)

#define CSL_CHIP_ICR_IC4_MASK            (0x00000010u)
#define CSL_CHIP_ICR_IC4_SHIFT           (0x00000004u)
#define CSL_CHIP_ICR_IC4_RESETVAL        (0x00000000u)

/*----IC4 Tokens----*/
#define CSL_CHIP_ICR_IC4_CLR             (0x00000001u)

#define CSL_CHIP_ICR_RESETVAL            (0x00000000u)

/* IER */

#define CSL_CHIP_IER_IE15_MASK           (0x00008000u)
#define CSL_CHIP_IER_IE15_SHIFT          (0x0000000Fu)
#define CSL_CHIP_IER_IE15_RESETVAL       (0x00000000u)

/*----IE15 Tokens----*/
#define CSL_CHIP_IER_IE15_ENABLE         (0x00000001u)
#define CSL_CHIP_IER_IE15_DISABLE        (0x00000000u)

#define CSL_CHIP_IER_IE14_MASK           (0x00004000u)
#define CSL_CHIP_IER_IE14_SHIFT          (0x0000000Eu)
#define CSL_CHIP_IER_IE14_RESETVAL       (0x00000000u)

/*----IE14 Tokens----*/
#define CSL_CHIP_IER_IE14_ENABLE         (0x00000001u)
#define CSL_CHIP_IER_IE14_DISABLE        (0x00000000u)

#define CSL_CHIP_IER_IE13_MASK           (0x00002000u)
#define CSL_CHIP_IER_IE13_SHIFT          (0x0000000Du)
#define CSL_CHIP_IER_IE13_RESETVAL       (0x00000000u)

/*----IE13 Tokens----*/
#define CSL_CHIP_IER_IE13_ENABLE         (0x00000001u)
#define CSL_CHIP_IER_IE13_DISABLE        (0x00000000u)

#define CSL_CHIP_IER_IE12_MASK           (0x00001000u)
#define CSL_CHIP_IER_IE12_SHIFT          (0x0000000Cu)
#define CSL_CHIP_IER_IE12_RESETVAL       (0x00000000u)

/*----IE12 Tokens----*/
#define CSL_CHIP_IER_IE12_ENABLE         (0x00000001u)
#define CSL_CHIP_IER_IE12_DISABLE        (0x00000000u)

#define CSL_CHIP_IER_IE11_MASK           (0x00000800u)
#define CSL_CHIP_IER_IE11_SHIFT          (0x0000000Bu)
#define CSL_CHIP_IER_IE11_RESETVAL       (0x00000000u)

/*----IE11 Tokens----*/
#define CSL_CHIP_IER_IE11_ENABLE         (0x00000001u)
#define CSL_CHIP_IER_IE11_DISABLE        (0x00000000u)

#define CSL_CHIP_IER_IE10_MASK           (0x00000400u)
#define CSL_CHIP_IER_IE10_SHIFT          (0x0000000Au)
#define CSL_CHIP_IER_IE10_RESETVAL       (0x00000000u)

/*----IE10 Tokens----*/
#define CSL_CHIP_IER_IE10_ENABLE         (0x00000001u)
#define CSL_CHIP_IER_IE10_DISABLE        (0x00000000u)

#define CSL_CHIP_IER_IE09_MASK           (0x00000200u)
#define CSL_CHIP_IER_IE09_SHIFT          (0x00000009u)
#define CSL_CHIP_IER_IE09_RESETVAL       (0x00000000u)

/*----IE09 Tokens----*/
#define CSL_CHIP_IER_IE09_ENABLE         (0x00000001u)
#define CSL_CHIP_IER_IE09_DISABLE        (0x00000000u)

#define CSL_CHIP_IER_IE08_MASK           (0x00000100u)
#define CSL_CHIP_IER_IE08_SHIFT          (0x00000008u)
#define CSL_CHIP_IER_IE08_RESETVAL       (0x00000000u)

/*----IE08 Tokens----*/
#define CSL_CHIP_IER_IE08_ENABLE         (0x00000001u)
#define CSL_CHIP_IER_IE08_DISABLE        (0x00000000u)

#define CSL_CHIP_IER_IE07_MASK           (0x00000080u)
#define CSL_CHIP_IER_IE07_SHIFT          (0x00000007u)
#define CSL_CHIP_IER_IE07_RESETVAL       (0x00000000u)

/*----IE07 Tokens----*/
#define CSL_CHIP_IER_IE07_ENABLE         (0x00000001u)
#define CSL_CHIP_IER_IE07_DISABLE        (0x00000000u)

#define CSL_CHIP_IER_IE06_MASK           (0x00000040u)
#define CSL_CHIP_IER_IE06_SHIFT          (0x00000006u)
#define CSL_CHIP_IER_IE06_RESETVAL       (0x00000000u)

/*----IE06 Tokens----*/
#define CSL_CHIP_IER_IE06_ENABLE         (0x00000001u)
#define CSL_CHIP_IER_IE06_DISABLE        (0x00000000u)

#define CSL_CHIP_IER_IE05_MASK           (0x00000020u)
#define CSL_CHIP_IER_IE05_SHIFT          (0x00000005u)
#define CSL_CHIP_IER_IE05_RESETVAL       (0x00000000u)

/*----IE05 Tokens----*/
#define CSL_CHIP_IER_IE05_ENABLE         (0x00000001u)
#define CSL_CHIP_IER_IE05_DISABLE        (0x00000000u)

#define CSL_CHIP_IER_IE04_MASK           (0x00000010u)
#define CSL_CHIP_IER_IE04_SHIFT          (0x00000004u)
#define CSL_CHIP_IER_IE04_RESETVAL       (0x00000000u)

/*----IE04 Tokens----*/
#define CSL_CHIP_IER_IE04_ENABLE         (0x00000001u)
#define CSL_CHIP_IER_IE04_DISABLE        (0x00000000u)

#define CSL_CHIP_IER_NMI_MASK            (0x00000002u)
#define CSL_CHIP_IER_NMI_SHIFT           (0x00000001u)
#define CSL_CHIP_IER_NMI_RESETVAL        (0x00000000u)

/*----NMI Tokens----*/
#define CSL_CHIP_IER_NMI_ENABLE          (0x00000001u)

#define CSL_CHIP_IER_RESET_MASK          (0x00000001u)
#define CSL_CHIP_IER_RESET_SHIFT         (0x00000000u)
#define CSL_CHIP_IER_RESET_RESETVAL      (0x00000001u)

#define CSL_CHIP_IER_RESETVAL            (0x00000001u)

/* ISTP */

#define CSL_CHIP_ISTP_ISTB_MASK          (0xFFFFFC00u)
#define CSL_CHIP_ISTP_ISTB_SHIFT         (0x0000000Au)
#define CSL_CHIP_ISTP_ISTB_RESETVAL      (0x00000000u)

#define CSL_CHIP_ISTP_HPEINT_MASK        (0x000003E0u)
#define CSL_CHIP_ISTP_HPEINT_SHIFT       (0x00000005u)
#define CSL_CHIP_ISTP_HPEINT_RESETVAL    (0x00000000u)

#define CSL_CHIP_ISTP_RESETVAL           (0x00000000u)

/* IRP */

#define CSL_CHIP_IRP_IRP_MASK            (0xFFFFFFFFu)
#define CSL_CHIP_IRP_IRP_SHIFT           (0x00000000u)
#define CSL_CHIP_IRP_IRP_RESETVAL        (0x00000000u)

#define CSL_CHIP_IRP_RESETVAL            (0x00000000u)

/* NRP */

#define CSL_CHIP_NRP_NRP_MASK            (0xFFFFFFFFu)
#define CSL_CHIP_NRP_NRP_SHIFT           (0x00000000u)
#define CSL_CHIP_NRP_NRP_RESETVAL        (0x00000000u)

#define CSL_CHIP_NRP_RESETVAL            (0x00000000u)

/* ERP */

#define CSL_CHIP_ERP_ERP_MASK            (0xFFFFFFFFu)
#define CSL_CHIP_ERP_ERP_SHIFT           (0x00000000u)
#define CSL_CHIP_ERP_ERP_RESETVAL        (0x00000000u)

#define CSL_CHIP_ERP_RESETVAL            (0x00000000u)

/* TSCL */

#define CSL_CHIP_TSCL_TSCL_MASK          (0xFFFFFFFFu)
#define CSL_CHIP_TSCL_TSCL_SHIFT         (0x00000000u)
#define CSL_CHIP_TSCL_TSCL_RESETVAL      (0x00000000u)

#define CSL_CHIP_TSCL_RESETVAL           (0x00000000u)

/* TSCH */

#define CSL_CHIP_TSCH_TSCH_MASK          (0xFFFFFFFFu)
#define CSL_CHIP_TSCH_TSCH_SHIFT         (0x00000000u)
#define CSL_CHIP_TSCH_TSCH_RESETVAL      (0x00000000u)

#define CSL_CHIP_TSCH_RESETVAL           (0x00000000u)

/* ILC */

#define CSL_CHIP_ILC_ILC_MASK            (0xFFFFFFFFu)
#define CSL_CHIP_ILC_ILC_SHIFT           (0x00000000u)
#define CSL_CHIP_ILC_ILC_RESETVAL        (0x00000000u)

#define CSL_CHIP_ILC_RESETVAL            (0x00000000u)

/* RILC */

#define CSL_CHIP_RILC_RILC_MASK          (0xFFFFFFFFu)
#define CSL_CHIP_RILC_RILC_SHIFT         (0x00000000u)
#define CSL_CHIP_RILC_RILC_RESETVAL      (0x00000000u)

#define CSL_CHIP_RILC_RESETVAL           (0x00000000u)

/* PCE1 */

#define CSL_CHIP_PCE1_PCE1_MASK          (0xFFFFFFFFu)
#define CSL_CHIP_PCE1_PCE1_SHIFT         (0x00000000u)
#define CSL_CHIP_PCE1_PCE1_RESETVAL      (0x00000000u)

#define CSL_CHIP_PCE1_RESETVAL           (0x00000000u)

/* DNUM */

#define CSL_CHIP_DNUM_DSPNUM_MASK        (0x000000FFu)
#define CSL_CHIP_DNUM_DSPNUM_SHIFT       (0x00000000u)
#define CSL_CHIP_DNUM_DSPNUM_RESETVAL    (0x00000000u)

#define CSL_CHIP_DNUM_RESETVAL           (0x00000000u)

/* SSR */

#define CSL_CHIP_SSR_RESV_MASK           (0xFFFFFFC0u)
#define CSL_CHIP_SSR_RESV_SHIFT          (0x00000006u)
#define CSL_CHIP_SSR_RESV_RESETVAL       (0x00000000u)

#define CSL_CHIP_SSR_M2_MASK             (0x00000020u)
#define CSL_CHIP_SSR_M2_SHIFT            (0x00000005u)
#define CSL_CHIP_SSR_M2_RESETVAL         (0x00000000u)

#define CSL_CHIP_SSR_M1_MASK             (0x00000010u)
#define CSL_CHIP_SSR_M1_SHIFT            (0x00000004u)
#define CSL_CHIP_SSR_M1_RESETVAL         (0x00000000u)

#define CSL_CHIP_SSR_S2_MASK             (0x00000008u)
#define CSL_CHIP_SSR_S2_SHIFT            (0x00000003u)
#define CSL_CHIP_SSR_S2_RESETVAL         (0x00000000u)

#define CSL_CHIP_SSR_S1_MASK             (0x00000004u)
#define CSL_CHIP_SSR_S1_SHIFT            (0x00000002u)
#define CSL_CHIP_SSR_S1_RESETVAL         (0x00000000u)

#define CSL_CHIP_SSR_L2_MASK             (0x00000002u)
#define CSL_CHIP_SSR_L2_SHIFT            (0x00000001u)
#define CSL_CHIP_SSR_L2_RESETVAL         (0x00000000u)

#define CSL_CHIP_SSR_L1_MASK             (0x00000001u)
#define CSL_CHIP_SSR_L1_SHIFT            (0x00000000u)
#define CSL_CHIP_SSR_L1_RESETVAL         (0x00000000u)

#define CSL_CHIP_SSR_RESETVAL            (0x00000000u)

/* GPLYA */

#define CSL_CHIP_GPLYA_GPLYA_MASK        (0xFFFFFFFFu)
#define CSL_CHIP_GPLYA_GPLYA_SHIFT       (0x00000000u)
#define CSL_CHIP_GPLYA_GPLYA_RESETVAL    (0x00000000u)

#define CSL_CHIP_GPLYA_RESETVAL          (0x00000000u)

/* GPLYB */

#define CSL_CHIP_GPLYB_GPLYB_MASK        (0xFFFFFFFFu)
#define CSL_CHIP_GPLYB_GPLYB_SHIFT       (0x00000000u)
#define CSL_CHIP_GPLYB_GPLYB_RESETVAL    (0x00000000u)

#define CSL_CHIP_GPLYB_RESETVAL          (0x00000000u)

/* GFPGFR */

#define CSL_CHIP_GFPGFR_SIZE_MASK        (0x07000000u)
#define CSL_CHIP_GFPGFR_SIZE_SHIFT       (0x00000018u)
#define CSL_CHIP_GFPGFR_SIZE_RESETVAL    (0x00000007u)

#define CSL_CHIP_GFPGFR_POLY_MASK        (0x000000FFu)
#define CSL_CHIP_GFPGFR_POLY_SHIFT       (0x00000000u)
#define CSL_CHIP_GFPGFR_POLY_RESETVAL    (0x0000001Du)

#define CSL_CHIP_GFPGFR_RESETVAL         (0x0700001Du)

/* TSR */

#define CSL_CHIP_TSR_IB_MASK             (0x00008000u)
#define CSL_CHIP_TSR_IB_SHIFT            (0x0000000Fu)
#define CSL_CHIP_TSR_IB_RESETVAL         (0x00000000u)

/*----IB Tokens----*/
#define CSL_CHIP_TSR_IB_UNBLOCKED        (0x00000000u)
#define CSL_CHIP_TSR_IB_BLOCKED          (0x00000001u)

#define CSL_CHIP_TSR_SPLX_MASK           (0x00004000u)
#define CSL_CHIP_TSR_SPLX_SHIFT          (0x0000000Eu)
#define CSL_CHIP_TSR_SPLX_RESETVAL       (0x00000000u)

/*----SPLX Tokens----*/
#define CSL_CHIP_TSR_SPLX_NOTEXEC        (0x00000000u)
#define CSL_CHIP_TSR_SPLX_EXEC           (0x00000001u)

#define CSL_CHIP_TSR_EXC_MASK            (0x00000400u)
#define CSL_CHIP_TSR_EXC_SHIFT           (0x0000000Au)
#define CSL_CHIP_TSR_EXC_RESETVAL        (0x00000000u)

/*----EXC Tokens----*/
#define CSL_CHIP_TSR_EXC_NOTEXEC         (0x00000000u)
#define CSL_CHIP_TSR_EXC_EXEC            (0x00000001u)

#define CSL_CHIP_TSR_INT_MASK            (0x00000200u)
#define CSL_CHIP_TSR_INT_SHIFT           (0x00000009u)
#define CSL_CHIP_TSR_INT_RESETVAL        (0x00000000u)

/*----INT Tokens----*/
#define CSL_CHIP_TSR_INT_NOTEXEC         (0x00000000u)
#define CSL_CHIP_TSR_INT_EXEC            (0x00000001u)

#define CSL_CHIP_TSR_CXM_MASK            (0x000000C0u)
#define CSL_CHIP_TSR_CXM_SHIFT           (0x00000006u)
#define CSL_CHIP_TSR_CXM_RESETVAL        (0x00000000u)

/*----CXM Tokens----*/
#define CSL_CHIP_TSR_CXM_SUPERVISOR      (0x00000000u)
#define CSL_CHIP_TSR_CXM_USER            (0x00000001u)

#define CSL_CHIP_TSR_XEN_MASK            (0x00000008u)
#define CSL_CHIP_TSR_XEN_SHIFT           (0x00000003u)
#define CSL_CHIP_TSR_XEN_RESETVAL        (0x00000000u)

/*----XEN Tokens----*/
#define CSL_CHIP_TSR_XEN_ENABLE          (0x00000001u)
#define CSL_CHIP_TSR_XEN_DISABLE         (0x00000000u)

#define CSL_CHIP_TSR_GEE_MASK            (0x00000004u)
#define CSL_CHIP_TSR_GEE_SHIFT           (0x00000002u)
#define CSL_CHIP_TSR_GEE_RESETVAL        (0x00000000u)

/*----GEE Tokens----*/
#define CSL_CHIP_TSR_GEE_ENABLE          (0x00000001u)
#define CSL_CHIP_TSR_GEE_DISABLE         (0x00000000u)

#define CSL_CHIP_TSR_SGIE_MASK           (0x00000002u)
#define CSL_CHIP_TSR_SGIE_SHIFT          (0x00000001u)
#define CSL_CHIP_TSR_SGIE_RESETVAL       (0x00000000u)

#define CSL_CHIP_TSR_GIE_MASK            (0x00000001u)
#define CSL_CHIP_TSR_GIE_SHIFT           (0x00000000u)
#define CSL_CHIP_TSR_GIE_RESETVAL        (0x00000000u)

/*----GIE Tokens----*/
#define CSL_CHIP_TSR_GIE_ENABLE          (0x00000001u)
#define CSL_CHIP_TSR_GIE_DISABLE         (0x00000000u)

#define CSL_CHIP_TSR_RESETVAL            (0x00000000u)

/* ITSR */

#define CSL_CHIP_ITSR_IB_MASK            (0x00008000u)
#define CSL_CHIP_ITSR_IB_SHIFT           (0x0000000Fu)
#define CSL_CHIP_ITSR_IB_RESETVAL        (0x00000000u)

/*----IB Tokens----*/
#define CSL_CHIP_ITSR_IB_UNBLOCKED       (0x00000000u)
#define CSL_CHIP_ITSR_IB_BLOCKED         (0x00000001u)

#define CSL_CHIP_ITSR_SPLX_MASK          (0x00004000u)
#define CSL_CHIP_ITSR_SPLX_SHIFT         (0x0000000Eu)
#define CSL_CHIP_ITSR_SPLX_RESETVAL      (0x00000000u)

/*----SPLX Tokens----*/
#define CSL_CHIP_ITSR_SPLX_NOTEXEC       (0x00000000u)
#define CSL_CHIP_ITSR_SPLX_EXEC          (0x00000001u)

#define CSL_CHIP_ITSR_EXC_MASK           (0x00000400u)
#define CSL_CHIP_ITSR_EXC_SHIFT          (0x0000000Au)
#define CSL_CHIP_ITSR_EXC_RESETVAL       (0x00000000u)

/*----EXC Tokens----*/
#define CSL_CHIP_ITSR_EXC_NOTEXEC        (0x00000000u)
#define CSL_CHIP_ITSR_EXC_EXEC           (0x00000001u)

#define CSL_CHIP_ITSR_INT_MASK           (0x00000200u)
#define CSL_CHIP_ITSR_INT_SHIFT          (0x00000009u)
#define CSL_CHIP_ITSR_INT_RESETVAL       (0x00000000u)

/*----INT Tokens----*/
#define CSL_CHIP_ITSR_INT_NOTEXEC        (0x00000000u)
#define CSL_CHIP_ITSR_INT_EXEC           (0x00000001u)

#define CSL_CHIP_ITSR_CXM_MASK           (0x000000C0u)
#define CSL_CHIP_ITSR_CXM_SHIFT          (0x00000006u)
#define CSL_CHIP_ITSR_CXM_RESETVAL       (0x00000000u)

/*----CXM Tokens----*/
#define CSL_CHIP_ITSR_CXM_SUPERVISOR     (0x00000000u)
#define CSL_CHIP_ITSR_CXM_USER           (0x00000001u)

#define CSL_CHIP_ITSR_XEN_MASK           (0x00000008u)
#define CSL_CHIP_ITSR_XEN_SHIFT          (0x00000003u)
#define CSL_CHIP_ITSR_XEN_RESETVAL       (0x00000000u)

/*----XEN Tokens----*/
#define CSL_CHIP_ITSR_XEN_ENABLE         (0x00000001u)
#define CSL_CHIP_ITSR_XEN_DISABLE        (0x00000000u)

#define CSL_CHIP_ITSR_GEE_MASK           (0x00000004u)
#define CSL_CHIP_ITSR_GEE_SHIFT          (0x00000002u)
#define CSL_CHIP_ITSR_GEE_RESETVAL       (0x00000000u)

/*----GEE Tokens----*/
#define CSL_CHIP_ITSR_GEE_ENABLE         (0x00000001u)
#define CSL_CHIP_ITSR_GEE_DISABLE        (0x00000000u)

#define CSL_CHIP_ITSR_SGIE_MASK          (0x00000002u)
#define CSL_CHIP_ITSR_SGIE_SHIFT         (0x00000001u)
#define CSL_CHIP_ITSR_SGIE_RESETVAL      (0x00000000u)

#define CSL_CHIP_ITSR_GIE_MASK           (0x00000001u)
#define CSL_CHIP_ITSR_GIE_SHIFT          (0x00000000u)
#define CSL_CHIP_ITSR_GIE_RESETVAL       (0x00000000u)

/*----GIE Tokens----*/
#define CSL_CHIP_ITSR_GIE_ENABLE         (0x00000001u)
#define CSL_CHIP_ITSR_GIE_DISABLE        (0x00000000u)

#define CSL_CHIP_ITSR_RESETVAL           (0x00000000u)

/* NTSR */

#define CSL_CHIP_NTSR_IB_MASK            (0x00008000u)
#define CSL_CHIP_NTSR_IB_SHIFT           (0x0000000Fu)
#define CSL_CHIP_NTSR_IB_RESETVAL        (0x00000000u)

/*----IB Tokens----*/
#define CSL_CHIP_NTSR_IB_UNBLOCKED       (0x00000000u)
#define CSL_CHIP_NTSR_IB_BLOCKED         (0x00000001u)

#define CSL_CHIP_NTSR_SPLX_MASK          (0x00004000u)
#define CSL_CHIP_NTSR_SPLX_SHIFT         (0x0000000Eu)
#define CSL_CHIP_NTSR_SPLX_RESETVAL      (0x00000000u)

/*----SPLX Tokens----*/
#define CSL_CHIP_NTSR_SPLX_NOTEXEC       (0x00000000u)
#define CSL_CHIP_NTSR_SPLX_EXEC          (0x00000001u)

#define CSL_CHIP_NTSR_EXC_MASK           (0x00000400u)
#define CSL_CHIP_NTSR_EXC_SHIFT          (0x0000000Au)
#define CSL_CHIP_NTSR_EXC_RESETVAL       (0x00000000u)

/*----EXC Tokens----*/
#define CSL_CHIP_NTSR_EXC_NOTEXEC        (0x00000000u)
#define CSL_CHIP_NTSR_EXC_EXEC           (0x00000001u)

#define CSL_CHIP_NTSR_INT_MASK           (0x00000200u)
#define CSL_CHIP_NTSR_INT_SHIFT          (0x00000009u)
#define CSL_CHIP_NTSR_INT_RESETVAL       (0x00000000u)

/*----INT Tokens----*/
#define CSL_CHIP_NTSR_INT_NOTEXEC        (0x00000000u)
#define CSL_CHIP_NTSR_INT_EXEC           (0x00000001u)

#define CSL_CHIP_NTSR_CXM_MASK           (0x000000C0u)
#define CSL_CHIP_NTSR_CXM_SHIFT          (0x00000006u)
#define CSL_CHIP_NTSR_CXM_RESETVAL       (0x00000000u)

/*----CXM Tokens----*/
#define CSL_CHIP_NTSR_CXM_SUPERVISOR     (0x00000000u)
#define CSL_CHIP_NTSR_CXM_USER           (0x00000001u)

#define CSL_CHIP_NTSR_XEN_MASK           (0x00000008u)
#define CSL_CHIP_NTSR_XEN_SHIFT          (0x00000003u)
#define CSL_CHIP_NTSR_XEN_RESETVAL       (0x00000000u)

/*----XEN Tokens----*/
#define CSL_CHIP_NTSR_XEN_ENABLE         (0x00000001u)
#define CSL_CHIP_NTSR_XEN_DISABLE        (0x00000000u)

#define CSL_CHIP_NTSR_GEE_MASK           (0x00000004u)
#define CSL_CHIP_NTSR_GEE_SHIFT          (0x00000002u)
#define CSL_CHIP_NTSR_GEE_RESETVAL       (0x00000000u)

/*----GEE Tokens----*/
#define CSL_CHIP_NTSR_GEE_ENABLE         (0x00000001u)
#define CSL_CHIP_NTSR_GEE_DISABLE        (0x00000000u)

#define CSL_CHIP_NTSR_SGIE_MASK          (0x00000002u)
#define CSL_CHIP_NTSR_SGIE_SHIFT         (0x00000001u)
#define CSL_CHIP_NTSR_SGIE_RESETVAL      (0x00000000u)

#define CSL_CHIP_NTSR_GIE_MASK           (0x00000001u)
#define CSL_CHIP_NTSR_GIE_SHIFT          (0x00000000u)
#define CSL_CHIP_NTSR_GIE_RESETVAL       (0x00000000u)

/*----GIE Tokens----*/
#define CSL_CHIP_NTSR_GIE_ENABLE         (0x00000001u)
#define CSL_CHIP_NTSR_GIE_DISABLE        (0x00000000u)

#define CSL_CHIP_NTSR_RESETVAL           (0x00000000u)

/* EFR */

#define CSL_CHIP_EFR_NXF_MASK            (0x80000000u)
#define CSL_CHIP_EFR_NXF_SHIFT           (0x0000001Fu)
#define CSL_CHIP_EFR_NXF_RESETVAL        (0x00000000u)

#define CSL_CHIP_EFR_EXF_MASK            (0x40000000u)
#define CSL_CHIP_EFR_EXF_SHIFT           (0x0000001Eu)
#define CSL_CHIP_EFR_EXF_RESETVAL        (0x00000000u)

#define CSL_CHIP_EFR_IXF_MASK            (0x00000002u)
#define CSL_CHIP_EFR_IXF_SHIFT           (0x00000001u)
#define CSL_CHIP_EFR_IXF_RESETVAL        (0x00000000u)

#define CSL_CHIP_EFR_OXF_MASK            (0x00000001u)
#define CSL_CHIP_EFR_OXF_SHIFT           (0x00000000u)
#define CSL_CHIP_EFR_OXF_RESETVAL        (0x00000000u)

#define CSL_CHIP_EFR_RESETVAL            (0x00000000u)

/* ECR */

#define CSL_CHIP_ECR_NXC_MASK            (0x80000000u)
#define CSL_CHIP_ECR_NXC_SHIFT           (0x0000001Fu)
#define CSL_CHIP_ECR_NXC_RESETVAL        (0x00000000u)

/*----NXC Tokens----*/
#define CSL_CHIP_ECR_NXC_CLEAR           (0x00000001u)

#define CSL_CHIP_ECR_EXC_MASK            (0x40000000u)
#define CSL_CHIP_ECR_EXC_SHIFT           (0x0000001Eu)
#define CSL_CHIP_ECR_EXC_RESETVAL        (0x00000000u)

/*----EXC Tokens----*/
#define CSL_CHIP_ECR_EXC_CLEAR           (0x00000001u)

#define CSL_CHIP_ECR_IXC_MASK            (0x00000002u)
#define CSL_CHIP_ECR_IXC_SHIFT           (0x00000001u)
#define CSL_CHIP_ECR_IXC_RESETVAL        (0x00000000u)

/*----IXC Tokens----*/
#define CSL_CHIP_ECR_IXC_CLEAR           (0x00000001u)

#define CSL_CHIP_ECR_OXC_MASK            (0x00000001u)
#define CSL_CHIP_ECR_OXC_SHIFT           (0x00000000u)
#define CSL_CHIP_ECR_OXC_RESETVAL        (0x00000000u)

/*----OXC Tokens----*/
#define CSL_CHIP_ECR_OXC_CLEAR           (0x00000001u)

#define CSL_CHIP_ECR_RESETVAL            (0x00000000u)

/* IERR */

#define CSL_CHIP_IERR_LBX_MASK           (0x00000080u)
#define CSL_CHIP_IERR_LBX_SHIFT          (0x00000007u)
#define CSL_CHIP_IERR_LBX_RESETVAL       (0x00000000u)

#define CSL_CHIP_IERR_PRX_MASK           (0x00000040u)
#define CSL_CHIP_IERR_PRX_SHIFT          (0x00000006u)
#define CSL_CHIP_IERR_PRX_RESETVAL       (0x00000000u)

#define CSL_CHIP_IERR_RAX_MASK           (0x00000020u)
#define CSL_CHIP_IERR_RAX_SHIFT          (0x00000005u)
#define CSL_CHIP_IERR_RAX_RESETVAL       (0x00000000u)

#define CSL_CHIP_IERR_RCX_MASK           (0x00000010u)
#define CSL_CHIP_IERR_RCX_SHIFT          (0x00000004u)
#define CSL_CHIP_IERR_RCX_RESETVAL       (0x00000000u)

#define CSL_CHIP_IERR_OPX_MASK           (0x00000008u)
#define CSL_CHIP_IERR_OPX_SHIFT          (0x00000003u)
#define CSL_CHIP_IERR_OPX_RESETVAL       (0x00000000u)

#define CSL_CHIP_IERR_EPX_MASK           (0x00000004u)
#define CSL_CHIP_IERR_EPX_SHIFT          (0x00000002u)
#define CSL_CHIP_IERR_EPX_RESETVAL       (0x00000000u)

#define CSL_CHIP_IERR_FPX_MASK           (0x00000002u)
#define CSL_CHIP_IERR_FPX_SHIFT          (0x00000001u)
#define CSL_CHIP_IERR_FPX_RESETVAL       (0x00000000u)

#define CSL_CHIP_IERR_IFX_MASK           (0x00000001u)
#define CSL_CHIP_IERR_IFX_SHIFT          (0x00000000u)
#define CSL_CHIP_IERR_IFX_RESETVAL       (0x00000000u)

#define CSL_CHIP_IERR_RESETVAL           (0x00000000u)

/* REP */

#define CSL_CHIP_REP_REP_MASK            (0xFFFFFFFFu)
#define CSL_CHIP_REP_REP_SHIFT           (0x00000000u)
#define CSL_CHIP_REP_REP_RESETVAL        (0x00000000u)

#define CSL_CHIP_REP_RESETVAL            (0x00000000u)

#endif
