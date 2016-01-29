/** @file csl_emifClose.c
 *
 *    @brief File for functional layer of CSL API @a CSL_emifClose()
 *
 *  Description
 *    - The @a CSL_emifClose() function definition & it's associated functions
 *
 *  Modification 1
 *    - Created on: 10/6/2004
 *    - Reason: created the sources
 *
 *  @date 10th June, 2004
 *  @author Santosh Narayanan.
 */
#include <csl_emif.h>
#include <_csl_resource.h>

#pragma CODE_SECTION (CSL_emifClose, ".text:csl_section:emif");


/** @brief Unreserves the EMIF identified by the handle.
 */
CSL_Status  CSL_emifClose(
    /** Pointer to the object that holds reference to the
     *  instance of EMIF requested after the call 
	 */
    CSL_EmifHandle                         hEmif
	){
    /* Indicate in the CSL global data structure that the peripheral
     * has been unreserved 	 */
	return(_CSL_certifyClose((CSL_ResHandle)hEmif));
}

/* Rev.No.   Date/Time               ECN No.          Modifier      */
/* -------   ---------               -------          --------      */

/* 3          Aug 10:00:32 9         2271             xkeshavm      */
/*                                                                  */
/* To upload API CSL 0.51 Release                                   */
/********************************************************************/ 
