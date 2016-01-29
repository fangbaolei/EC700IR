/*   ==========================================================================
 *   Copyright (c) Texas Instruments Inc , 2004
 *
 *   Use of this software is controlled by the terms and conditions found
 *   in the license agreement under which this software has been supplied
 *   provided
 *   ==========================================================================
*/
/** @file _csl_ddrGetAttrs.c
 *
 *  @brief File for internal CSL API @a _CSL_ddrGetAttrs()
 *
 *  Description
 *    - The @a _CSL_ddrGetAttrs() function definition
 *
 *
 *  @date 29 June, 2004
 *  @author Pratheesh Gangadhar
 */
#include <csl_ddr.h>

#pragma CODE_SECTION (_CSL_ddrGetAttrs, ".text:csl_section:init");

/** @brief Gets the unique identifier and  resource allocation mask
 *  specified for the DDR module. This is a CSL internal function.
 *
 */
void _CSL_ddrGetAttrs(
    /* instance number of DDR */
    CSL_DdrNum                            ddrNum,
    /*Pointer to the object that holds reference to the
     * instance of DDR requested after the call */
    CSL_DdrHandle                         hDdr
)
{
    /* get the DDR base address into the handle */
    hDdr->regs = (CSL_DdrRegsOvly) _CSL_ddrGetBaseAddr(ddrNum);

    switch (ddrNum) {
    case CSL_DDR_0:
      /* get the DDR unique identifier & resource
       * allocation mask into the handle */
      hDdr->xio = CSL_DDR_0_XIO;
      hDdr->uid = CSL_DDR_0_UID;
      break;
    }

    /* get the ddr instance number into the handle */
    hDdr->ddrNum = ddrNum;
    
}
/* Rev.No.   Date/Time               ECN No.          Modifier      */
/* -------   ---------               -------          --------      */

/* 3          Aug 09:59:59 9         2271             xkeshavm      */
/*                                                                  */
/* To upload API CSL 0.51 Release                                   */
/********************************************************************/ 
