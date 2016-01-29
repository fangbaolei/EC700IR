#ifndef _DAVINCI_HD64PLUS_H_
#define _DAVINCI_HD64PLUS_H_

#include <cslr.h>
/**************************************************************************\
* 64 soc file
\**************************************************************************/

#define CSL_EDMA_0                         0
/*  Memory Protection specific details */
/** @brief Base address of UMC Memory protection registers */
#define CSL_MEMPROT_L2_REGS               (0x184A000u)
 
/** @brief Base address of PMC memory Protection registers */
#define CSL_MEMPROT_L1P_REGS              (0x184A400u)

/** @brief Base address of DMC memory protection registers */
#define CSL_MEMPROT_L1D_REGS              (0x184AC00u)

/** @brief Base address of CONFIG memory protection registers */
#define CSL_MEMPROT_CONFIG_REGS           (0x1820300u)

/** @brief Base address of INTC memory mapped registers */
//#define CSL_INTC_REGS                     (0x01800000u)

/** @brief Base address of Channel controller  memory mapped registers */
//#define CSL_EDMACC_0_REGS                 (0x01C00000u)

/** @brief Base address of Transfer controller  memory mapped registers */
//#define CSL_EDMATC_0_REGS                ( 0x1C10000u) /* Needs to change as per soc */
//#define CSL_EDMATC_1_REGS                ( 0x1C10400u) /* Needs to change as per soc */

#define CSL_MEMPROT_L2    (0) 
#define CSL_MEMPROT_L1P    (1) 
#define CSL_MEMPROT_L1D    (2) 
#define CSL_MEMPROT_CONFIG (3) 
#endif  /* _DAVINCI_HD64PLUS_H_ */

/* Rev.No.   Date/Time               ECN No.          Modifier      */
/* -------   ---------               -------          --------      */

/* 2          Aug 10:09:39 9         2272             xkeshavm      */
/*                                                                  */
/* To upload API CSL 0.51 Release                                   */
/********************************************************************/ 
