/*  ============================================================================
 *   Copyright (c) Texas Instruments Inc 2002, 2003, 2004, 2005
 *
 *   Use of this software is controlled by the terms and conditions found in the
 *   license agreement under which this software has been supplied.
 *   ===========================================================================
 */
 
/*  ============================================================================
 *   @file  _csl_cache.h
 *
 *   @path  $(CSLPATH)\inc
 *
 *   @desc  This file defines the functions in the Cache
 *
 */
 
/* =============================================================================
 *  Revision History
 *  ===============
 *  23-Mar-2004 Jamon Bowen File Created.
 *
 * =============================================================================
 */
  
#ifndef __CSL_CACHE_H_
#define __CSL_CACHE_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Enum for CACHE Wait State */
typedef enum {
    /* Cache no wait */
    CACHE_WAIT_NONE       = 0,
    /* Cache wait L1D invalidate all */
    CACHE_WAIT_L1DINVALL  = 1,
    /* Cache wait LID invalidate */
    CACHE_WAIT_L1DINV     = 2,
    /* Cache wait L1D Write back all */
    CACHE_WAIT_L1DWBALL   = 3,
    /* Cache wait L1D Write back */
    CACHE_WAIT_L1DWB      = 4,
    /* Cache wait Write back invalidate all */
    CACHE_WAIT_L1DWBINVALL= 5,
    /* Cache wait L1D Write back invalidate */
    CACHE_WAIT_L1DWBINV   = 6,
    /* Cache wait L1P invaliate all */
    CACHE_WAIT_L1PINVALL  = 7,
    /* Cache wait L1P invalidate */
    CACHE_WAIT_L1PINV     = 8,
    /* Cache wait L2 invalidate all */
    CACHE_WAIT_L2INVALL   = 9,
    /* Cache wait L2 invalidate */
    CACHE_WAIT_L2INV      = 10,
    /* Cache wait L2 Write back all */
    CACHE_WAIT_L2WBALL    = 11,
    /* Cache wait L2 Write back */
    CACHE_WAIT_L2WB       = 12,
    /* Cache wait L2 Write back invalidate all */
    CACHE_WAIT_L2WBINVALL = 13,
    /* Cache wait L2 Write back invalidate */
    CACHE_WAIT_L2WBINV    = 14
} CACHE_waitState;


extern volatile CACHE_waitState _CSL_cachebusyState;

/*  Enum for CACHE Emif State */
typedef enum {
    /* No CACHE EMIFA */
    CACHE_EMIF_NONE = 0,
    /* CACHE EMIFA */
    CACHE_EMIF_A    = 1,
    /* CACHE EMIFB */
    CACHE_EMIF_B    = 2,
    /* CACHE EMIFAB */
    CACHE_EMIF_AB   = 3
}CACHE_emifState;


extern volatile CACHE_emifState _CSL_cacheEmifState;

#define CSL_CACHE_EMIF_ISRANGE(addr)  \
                        ((CSL_CACHE_EMIF_MASK  >> ((addr >> 27) & 0x1f)) & 0x1)
#define CSL_CACHE_EMIF_ISEMIFBRANGE(addr)  \
                        ((CSL_CACHE_EMIFB_MASK >> ((addr >> 27) & 0x1f)) & 0x1)

void _CSL_cacheApplyPatch(); 
void _CACHE_wait(CACHE_Wait);

#define CSL_CACHE_EMIF_MASK  0x3ff30000
#define CSL_CACHE_EMIFB_MASK 0x30000000

/* Emif A Config */
#define CSL_CACHE_EMIFA_REG     *(volatile unsigned int*) 0x70000000 
/* Emif B Config */
#define CSL_CACHE_EMIFB_REG     *(volatile unsigned int*) 0x78000000 
#define CSL_CACHE_EMIFA_PATCH() { CSL_CACHE_EMIFA_REG = 0; CSL_CACHE_EMIFA_REG;}
#define CSL_CACHE_EMIFB_PATCH() { CSL_CACHE_EMIFB_REG = 0; CSL_CACHE_EMIFB_REG;}

#ifdef __cplusplus
}
#endif

#endif /*__CSL_CACHE_H_ */
