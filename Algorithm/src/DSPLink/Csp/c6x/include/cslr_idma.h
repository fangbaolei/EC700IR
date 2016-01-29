#ifndef _CSLR_IDMA_1_H_

#define _CSLR_IDMA_1_H_

/*********************************************************************

 * Copyright (C) 2003-2004 Texas Instruments Incorporated. 

 * All Rights Reserved 

 *********************************************************************/

 /*

 *      This file contains the Register Desciptions for IDMA

 * 

 *********************************************************************/



#include <cslr.h>



#include <tistdtypes.h>



/**************************************************************************\

* Register Overlay Structure 

\**************************************************************************/

typedef struct  {

    volatile Uint32 IDMA0_STAT;

    volatile Uint32 IDMA0_MASK;

    volatile Uint32 IDMA0_SRC;

    volatile Uint32 IDMA0_DST;

    volatile Uint32 IDMA0_CNT;

    const char RSVD0[236];

    volatile Uint32 IDMA1_STAT;

    const char RSVD1[4];

    volatile Uint32 IDMA1_SRC;

    volatile Uint32 IDMA1_DST;

    volatile Uint32 IDMA1_CNT;

} CSL_IdmaRegs;



/**************************************************************************\

* Register Id's 

\**************************************************************************/

typedef enum  {

   CSL_IDMA_IDMA0_STAT = 0x0000u,

   CSL_IDMA_IDMA0_MASK = 0x0004u,

   CSL_IDMA_IDMA0_SRC = 0x0008u,

   CSL_IDMA_IDMA0_DST = 0x000cu,

   CSL_IDMA_IDMA0_CNT = 0x0010u,

   CSL_IDMA_IDMA1_STAT = 0x0100u,

   CSL_IDMA_IDMA1_SRC = 0x0108u,

   CSL_IDMA_IDMA1_DST = 0x010cu,

   CSL_IDMA_IDMA1_CNT = 0x0110u

} CSL_IdmaRegIds;



/**************************************************************************\

* Field Definition Macros 

\**************************************************************************/



/* IDMA0_STAT */



#define CSL_IDMA_IDMA0_STAT_ACTV_MASK    (0x00000001u)

#define CSL_IDMA_IDMA0_STAT_ACTV_SHIFT   (0x00000000u)

#define CSL_IDMA_IDMA0_STAT_ACTV_RESETVAL (0x00000000u)

#define CSL_IDMA_IDMA0_STAT_ACTV_ACTIVE  (0x00000001u)

#define CSL_IDMA_IDMA0_STAT_ACTV_INACTIVE (0x00000000u)



#define CSL_IDMA_IDMA0_STAT_PEND_MASK    (0x00000002u)

#define CSL_IDMA_IDMA0_STAT_PEND_SHIFT   (0x00000001u)

#define CSL_IDMA_IDMA0_STAT_PEND_RESETVAL (0x00000000u)

#define CSL_IDMA_IDMA0_STAT_PEND_PENDING (0x00000001u)

#define CSL_IDMA_IDMA0_STAT_PEND_NOPENDING (0x00000000u)



#define CSL_IDMA_IDMA0_STAT_RESETVAL     (0x00000000u)



/* IDMA0_MASK */



#define CSL_IDMA_IDMA0_MASK_BITMASK_MASK (0xFFFFFFFFu)

#define CSL_IDMA_IDMA0_MASK_BITMASK_SHIFT (0x00000000u)

#define CSL_IDMA_IDMA0_MASK_BITMASK_RESETVAL (0x00000000u)



#define CSL_IDMA_IDMA0_MASK_RESETVAL     (0x00000000u)



/* IDMA0_SRC */



#define CSL_IDMA_IDMA0_SRC_SRCADDR_MASK  (0xFFFFFFFCu)

#define CSL_IDMA_IDMA0_SRC_SRCADDR_SHIFT (0x00000002u)

#define CSL_IDMA_IDMA0_SRC_SRCADDR_RESETVAL (0x00000000u)



#define CSL_IDMA_IDMA0_SRC_RESETVAL      (0x00000000u)



/* IDMA0_DST */



#define CSL_IDMA_IDMA0_DST_DSTADDR_MASK  (0xFFFFFFFCu)

#define CSL_IDMA_IDMA0_DST_DSTADDR_SHIFT (0x00000002u)

#define CSL_IDMA_IDMA0_DST_DSTADDR_RESETVAL (0x00000000u)



#define CSL_IDMA_IDMA0_DST_RESETVAL      (0x00000000u)



/* IDMA0_CNT */



#define CSL_IDMA_IDMA0_CNT_COUNT_MASK    (0x0000000Fu)

#define CSL_IDMA_IDMA0_CNT_COUNT_SHIFT   (0x00000000u)

#define CSL_IDMA_IDMA0_CNT_COUNT_RESETVAL (0x00000000u)



#define CSL_IDMA_IDMA0_CNT_RESETVAL      (0x00000000u)



/* IDMA1_STAT */



#define CSL_IDMA_IDMA1_STAT_ACTV_MASK    (0x00000001u)

#define CSL_IDMA_IDMA1_STAT_ACTV_SHIFT   (0x00000000u)

#define CSL_IDMA_IDMA1_STAT_ACTV_RESETVAL (0x00000000u)

#define CSL_IDMA_IDMA1_STAT_ACTV_ACTIVE  (0x00000001u)

#define CSL_IDMA_IDMA1_STAT_ACTV_INACTIVE (0x00000000u)



#define CSL_IDMA_IDMA1_STAT_PEND_MASK    (0x00000002u)

#define CSL_IDMA_IDMA1_STAT_PEND_SHIFT   (0x00000001u)

#define CSL_IDMA_IDMA1_STAT_PEND_RESETVAL (0x00000000u)

#define CSL_IDMA_IDMA1_STAT_PEND_PENDING (0x00000001u)

#define CSL_IDMA_IDMA1_STAT_PEND_NOPENDING (0x00000000u)



#define CSL_IDMA_IDMA1_STAT_RESETVAL     (0x00000000u)



/* IDMA1_SRC */



#define CSL_IDMA_IDMA1_SRC_SRCADDR_MASK  (0xFFFFFFFFu)

#define CSL_IDMA_IDMA1_SRC_SRCADDR_SHIFT (0x00000000u)

#define CSL_IDMA_IDMA1_SRC_SRCADDR_RESETVAL (0x00000000u) #define CSL_IDMA_IDMA1_SRC_SRCADDR_ZEROFILL (0x00000000u) #define CSL_IDMA_IDMA1_SRC_SRCADDR_ONESFILL (0xFFFFFFFFu)



#define CSL_IDMA_IDMA1_SRC_RESETVAL      (0x00000000u)



/* IDMA1_DST */



#define CSL_IDMA_IDMA1_DST_DSTADDR_MASK  (0xFFFFFFFFu)

#define CSL_IDMA_IDMA1_DST_DSTADDR_SHIFT (0x00000000u)

#define CSL_IDMA_IDMA1_DST_DSTADDR_RESETVAL (0x00000000u)



#define CSL_IDMA_IDMA1_DST_RESETVAL      (0x00000000u)



/* IDMA1_CNT */



#define CSL_IDMA_IDMA1_CNT_COUNT_MASK    (0x0000FFFFu)

#define CSL_IDMA_IDMA1_CNT_COUNT_SHIFT   (0x00000000u)

#define CSL_IDMA_IDMA1_CNT_COUNT_RESETVAL (0x00000000u)



#define CSL_IDMA_IDMA1_CNT_FILL_MASK     (0x00010000u)

#define CSL_IDMA_IDMA1_CNT_FILL_SHIFT    (0x00000010u)

#define CSL_IDMA_IDMA1_CNT_FILL_RESETVAL (0x00000000u)

#define CSL_IDMA_IDMA1_CNT_FILL_FILL     (0x00000001u)

#define CSL_IDMA_IDMA1_CNT_FILL_XFER     (0x00000000u)



#define CSL_IDMA_IDMA1_CNT_INT_MASK      (0x10000000u)

#define CSL_IDMA_IDMA1_CNT_INT_SHIFT     (0x0000001Cu)

#define CSL_IDMA_IDMA1_CNT_INT_RESETVAL  (0x00000000u)

#define CSL_IDMA_IDMA1_CNT_INT_YES       (0x00000001u)

#define CSL_IDMA_IDMA1_CNT_INT_NO        (0x00000000u)



#define CSL_IDMA_IDMA1_CNT_PRI_MASK      (0xE0000000u)

#define CSL_IDMA_IDMA1_CNT_PRI_SHIFT     (0x0000001Du)

#define CSL_IDMA_IDMA1_CNT_PRI_RESETVAL  (0x00000000u)

#define CSL_IDMA_IDMA1_CNT_PRI_PRI0      (0x00000000u)

#define CSL_IDMA_IDMA1_CNT_PRI_PRI1      (0x00000001u)

#define CSL_IDMA_IDMA1_CNT_PRI_PRI2      (0x00000002u)

#define CSL_IDMA_IDMA1_CNT_PRI_PRI3      (0x00000003u)

#define CSL_IDMA_IDMA1_CNT_PRI_PRI4      (0x00000004u)

#define CSL_IDMA_IDMA1_CNT_PRI_PRI5      (0x00000005u)

#define CSL_IDMA_IDMA1_CNT_PRI_PRI6      (0x00000006u)

#define CSL_IDMA_IDMA1_CNT_PRI_PRI7      (0x00000007u)



#define CSL_IDMA_IDMA1_CNT_RESETVAL     (0x00000000u)



#endif



