/*   ==========================================================================
 *   Copyright (c) Texas Instruments Inc , 2004
 *
 *   Use of this software is controlled by the terms and conditions found
 *   in the license agreement under which this software has been supplied
 *   provided
 *   ==========================================================================
*/
#ifndef CSL_HDVICP_CONFIG_H
#define CSL_HDVICP_CONFIG_H

/*****************************************************************************/
 /** \file csl_hdvicp_config.h
 * 
 * \brief This file contains chip-specific HDVICP configuration, including 
 *  base addresses for all accelerators, INTC, ICEC 
 * 
 *****************************************************************************/


/*****************************************************************************\
* Include files for data types
\*****************************************************************************/
#include "cslr.h"
#include "tistdtypes.h"

/*****************************************************************************\
* Kaleido Peripheral Instance Count
\*****************************************************************************/

#define CSL_IDEF_INLINE static inline

/* Kaleido Peripheral Instance Counts */
#define CSLR_TOP_PER_CNT          2
#define CSLR_ECD_PER_CNT          2
#define CSLR_CALC_PER_CNT         2
#define CSLR_BS_PER_CNT           2
#define CSLR_MC_PER_CNT           2
#define CSLR_LPF_PER_CNT          2
#define CSLR_ME_PER_CNT           2
#define CSLR_IPE_PER_CNT          2
#define CSLR_CLKC_PER_CNT         2
#define CSLR_BFSWC_PER_CNT        2
#define CSLR_KLDINTC_PER_CNT      2
#define CSLR_ICEC_PER_CNT         2

/*****************************************************************************\
* Config Base Addresses 
\*****************************************************************************/

/* Kaliedo Reg Base Addresses */
#define KLD0_CFG_BASE   0x02000000
#define KLD1_CFG_BASE   0x02200000


#endif /* CSL_HDVICP_CONFIG_H */

