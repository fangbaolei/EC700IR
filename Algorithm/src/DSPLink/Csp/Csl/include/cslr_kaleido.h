/*=============================================================================
 TI Proprietary Information

 Copyright (c) 2005 Texas Instruments, Inc.

    This is an unpublished work created in the year stated above.
    Texas Instruments owns all rights in and to this work and
    intends to maintain and protect it as an unpublished copyright.
    In the event of either inadvertent or deliberate publication,
    the above stated date shall be treated as the year of first
    publication.  In the event of such publication, Texas Instruments
    intends to enforce its rights in the work under the copyright
    laws as a published work.
===============================================================================

 cslr_kaleido.h

 Description :
     Kaleido CSLR top

===============================================================================

 Revision History:

  Revision
   0.0.0    02/16/06  kurihara   Initial version
   0.0.1    04/05/06  kurihara   Changed CLKC and BFSW registers common
   0.0.2    04/18/06  kurihara   Fixed from cslr_clkc11.h to csrl_intc11.h
   0.0.3    08/09/06  kurihara   Changed from cslr_intc.h to cslr_kldintc.h

*/

#ifndef _CSLR_KALEIDO_H_
#define _CSLR_KALEIDO_H_

#include "cslr.h"
#include "tistdtypes.h"
#include "kaleido_mem_map.h"

//#ifdef KALEIDO10
#include "cslr_top10.h"
//#else
#include "cslr_top11.h"
//#endif

#include "cslr_ecd.h"
#include "cslr_calc.h"
#include "cslr_bs.h"
#include "cslr_mc.h"
#include "cslr_lpf.h"
#include "cslr_me.h"
#include "cslr_ipe.h"
#include "cslr_clkc.h"
#include "cslr_bfswc.h"
#include "cslr_kldintc.h"
#include "cslr_icec.h"

/*****************************************************************************\
* Peripheral Instance count
\*****************************************************************************/
#define CSLR_TOP_PER_CNT        1
#define CSLR_ECD_PER_CNT        1
#define CSLR_CALC_PER_CNT       1 
#define CSLR_BS_PER_CNT         1 
#define CSLR_MC_PER_CNT         1 
#define CSLR_LPF_PER_CNT        1 
#define CSLR_ME_PER_CNT         1 
#define CSLR_IPE_PER_CNT        1 
#define CSLR_CLKC_PER_CNT       1 
#define CSLR_BFSWC_PER_CNT      1 
#define CSLR_KLDINTC_PER_CNT    1 
#define CSLR_ICEC_PER_CNT       1 

/*****************************************************************************\
* Overlay structure typedef definition
\*****************************************************************************/
#ifdef KALEIDO10
typedef volatile CSL_Top10Regs        * CSL_Top10RegsOverlay;
typedef volatile CSL_MeRegs           * CSL_MeRegsOverlay;
typedef volatile CSL_IpeRegs          * CSL_IpeRegsOverlay;
#else
typedef volatile CSL_Top11Regs        * CSL_Top11RegsOverlay;
#endif

typedef volatile CSL_EcdRegs          * CSL_EcdRegsOverlay;
typedef volatile CSL_CalcRegs         * CSL_CalcRegsOverlay;
typedef volatile CSL_BsRegs           * CSL_BsRegsOverlay;
typedef volatile CSL_McRegs           * CSL_McRegsOverlay;
typedef volatile CSL_LpfRegs          * CSL_LpfRegsOverlay;
typedef volatile CSL_ClkcRegs         * CSL_ClkcRegsOverlay;
typedef volatile CSL_BfswcRegs        * CSL_BfswcRegsOverlay;
typedef volatile CSL_KldintcRegs      * CSL_KldintcRegsOverlay;
typedef volatile CSL_IcecRegs         * CSL_IcecRegsOverlay;

/*****************************************************************************\
* Peripheral Base Address
\*****************************************************************************/
#define KLD_ARM_TOPREG          ( KLD_CFG_BASE_FR_ARM + KLD_CFG_TOPREG   )
#define KLD_ARM_ECDREG          ( KLD_CFG_BASE_FR_ARM + KLD_CFG_ECDREG   )
#define KLD_ARM_CALCREG         ( KLD_CFG_BASE_FR_ARM + KLD_CFG_CALCREG  )
#define KLD_ARM_BSREG           ( KLD_CFG_BASE_FR_ARM + KLD_CFG_BSREG    )
#define KLD_ARM_MCREG           ( KLD_CFG_BASE_FR_ARM + KLD_CFG_MCREG    )
#define KLD_ARM_LPFREG          ( KLD_CFG_BASE_FR_ARM + KLD_CFG_LPFREG   )
#define KLD_ARM_MEREG           ( KLD_CFG_BASE_FR_ARM + KLD_CFG_MEREG    )
#define KLD_ARM_IPEREG          ( KLD_CFG_BASE_FR_ARM + KLD_CFG_IPEREG   )
#define KLD_ARM_CLKCREG         ( KLD_CFG_BASE_FR_ARM + KLD_CFG_CLKCREG  )
#define KLD_ARM_BFSWCREG        ( KLD_CFG_BASE_FR_ARM + KLD_CFG_BFSWCREG )
#define KLD_ARM_INTCREG         ( KLD_CFG_BASE_FR_ARM + KLD_CFG_INTCREG  )
#define KLD_ARM_ICECREG         ( KLD_CFG_BASE_FR_ARM + KLD_CFG_ICECREG  )

#ifdef KALEIDO10
#define CSL_TOP10_0_REGS        ((CSL_Top10RegsOverlay)   KLD_ARM_TOPREG   )
#define CSL_ME_0_REGS           ((CSL_MeRegsOverlay)      KLD_ARM_MEREG    )
#define CSL_IPE_0_REGS          ((CSL_IpeRegsOverlay)     KLD_ARM_IPEREG   )
#else
#define CSL_TOP11_0_REGS        ((CSL_Top11RegsOverlay)   KLD_ARM_TOPREG   )
#endif

#define CSL_ECD_0_REGS          ((CSL_EcdRegsOverlay)     KLD_ARM_ECDREG   )
#define CSL_CALC_0_REGS         ((CSL_CalcRegsOverlay)    KLD_ARM_CALCREG  )
#define CSL_BS_0_REGS           ((CSL_BsRegsOverlay)      KLD_ARM_BSREG    )
#define CSL_MC_0_REGS           ((CSL_McRegsOverlay)      KLD_ARM_MCREG    )
#define CSL_LPF_0_REGS          ((CSL_LpfRegsOverlay)     KLD_ARM_LPFREG   )
#define CSL_CLKC_0_REGS         ((CSL_ClkcRegsOverlay)    KLD_ARM_CLKCREG  )
#define CSL_BFSWC_0_REGS        ((CSL_BfswcRegsOverlay)   KLD_ARM_BFSWCREG )
#define CSL_ICEC_0_REGS         ((CSL_IcecRegsOverlay)    KLD_ARM_ICECREG  )
#define CSL_INTC_0_REGS         ((CSL_KldintcRegsOverlay) KLD_ARM_INTCREG  )

#endif



