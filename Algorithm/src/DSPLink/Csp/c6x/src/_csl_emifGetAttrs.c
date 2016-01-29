/*   ==========================================================================
 *   Copyright (c) Texas Instruments Inc , 2004
 *
 *   Use of this software is controlled by the terms and conditions found
 *   in the license agreement under which this software has been supplied
 *   provided
 *   ==========================================================================
*/
/** @file _csl_emifGetAttrs.c
 *
 *  @brief File for internal CSL API @a _CSL_emifGetAttrs()
 *
 *  Description
 *    - The @a _CSL_emifGetAttrs() function definition
 *
 *  Modification 1
 *    - Created on: 6/10/2004
 *    - Reason: created the sources
 *
 *  @date 10 June, 2004
 *  @author Santosh Narayanan
 */
#include <csl_emif.h>

#pragma CODE_SECTION (_CSL_emifGetAttrs, ".text:csl_section:init");

/** @brief Gets the unique identifier and  resource allocation mask
 *  specified for the EMIF module. This is a CSL internal function.
 *
 */
void _CSL_emifGetAttrs(
    /* instance number of EMIF */
    CSL_EmifNum                            emifNum,
    /*Pointer to the object that holds reference to the
     * instance of MS requested after the call */
    CSL_EmifHandle                         hEmif
)
{
    /* get the EMIF base address into the handle */
    hEmif->regs = (CSL_EmifRegsOvly) _CSL_emifGetBaseAddr(emifNum);

    switch (emifNum) {
    case CSL_EMIF_0:
      /* get the EMIF unique identifier & resource
       * allocation mask into the handle */
      hEmif->xio = CSL_EMIF_0_XIO;
      hEmif->uid = CSL_EMIF_0_UID;
      break;
    }

    /* get the EMIF instance number into the handle */
    hEmif->perNum = emifNum;
    
}
/* Rev.No.   Date/Time               ECN No.          Modifier      */
/* -------   ---------               -------          --------      */

/* 3          Aug 09:59:59 9         2271             xkeshavm      */
/*                                                                  */
/* To upload API CSL 0.51 Release                                   */
/********************************************************************/ 
