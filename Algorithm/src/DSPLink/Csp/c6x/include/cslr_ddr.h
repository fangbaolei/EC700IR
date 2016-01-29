/*   ==========================================================================
 *   Copyright (c) Texas Instruments Inc , 2004
 *
 *   Use of this software is controlled by the terms and conditions found
 *   in the license agreement under which this software has been supplied
 *   provided
 *   ==========================================================================
*/
#ifndef _CSLR_DDR_H_
#define _CSLR_DDR_H_

#include <davinci_hd.h>
#include <cslr_ddr_001.h>

/*****************************************************************************\
              Overlay structure typedef definition
\*****************************************************************************/

CSL_IDEF_INLINE CSL_DdrRegsOvly _CSL_ddrGetBaseAddr (Uint16 ddrNum) {
  return (CSL_DdrRegsOvly)_CSL_ddrlookup[ddrNum];
}

#endif  /* CSLR_DDR_H_ */
/* Rev.No.   Date/Time               ECN No.          Modifier      */
/* -------   ---------               -------          --------      */

/* 4         14 Jan 2005 13:32:26    5888             xkeshavm      */
/*                                                                  */
/* Uploaded the CSL0.57 JAN 2005 Release and built the library for ARM and DSP*/
/********************************************************************/ 
