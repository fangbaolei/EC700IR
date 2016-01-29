/*   ==========================================================================
 *   Copyright (c) Texas Instruments Inc , 2004
 *
 *   Use of this software is controlled by the terms and conditions found
 *   in the license agreement under which this software has been supplied
 *   provided
 *   ==========================================================================
*/
#ifndef _CSLR_EMIF_H_
#define _CSLR_EMIF_H_

#include <cslr.h>
#include <cslr_aemif_001.h>
#include <davinci_hd.h>

/*****************************************************************************\
               Overlay structure typedef definition
\*****************************************************************************/

/** @brief Returns the pointer to the EMIF Register Overlay Structure
 *         for a given instance of the EMIF
 *
 */
CSL_IDEF_INLINE CSL_EmifRegsOvly _CSL_emifGetBaseAddr (Uint16 emifNum) {
  return (CSL_EmifRegsOvly)_CSL_emiflookup[emifNum];
}

#endif  /* CSLR_EMIF_H_ */
/* Rev.No.   Date/Time               ECN No.          Modifier      */
/* -------   ---------               -------          --------      */

/* 4         14 Jan 2005 13:32:27    5888             xkeshavm      */
/*                                                                  */
/* Uploaded the CSL0.57 JAN 2005 Release and built the library for ARM and DSP*/
/********************************************************************/ 
