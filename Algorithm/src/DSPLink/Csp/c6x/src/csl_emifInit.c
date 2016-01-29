/** @file csl_emifInit.c
 *
 *    @brief    File for functional layer of CSL API @a CSL_emifInit()
 *
 *  Description
 *    - The @a CSL_emifInit() function definition & it's associated functions
 *
 *  Modification 1
 *    - Modified on: 10/6/2004
 *    - Reason: created the sources
 *
 *  @date 10th June, 2004
 *  @author Santosh Narayanan.
 */

#include <csl_emif.h>

#pragma CODE_SECTION (CSL_emifInit, ".text:csl_section:emif");

/** @brief This function is idempotent in that calling it many times is same as
 *  calling it once. This function initializes the CSL data structures, and doesn't
 *  affect the H/W.
 *
 *  <b> Usage Constraints: </b>
 *  CSL system initialization must be successfully completed by invoking
 *  @a CSL_sysInit() before calling this function. This function should be
 *  called before using any of the CSL APIs
 *
 *  @b Example:
 *  @verbatim
 *   ...
 *   if (CSL_sysInit() == CSL_SOK) {
 *      CSL_emifInit();
 *   }
 *  @endverbatim
 *
 *  @return returns the status of the operation
 *
 */
CSL_Status  CSL_emifInit(
    void
	){

    return CSL_SOK;
}

/* Rev.No.   Date/Time               ECN No.          Modifier      */
/* -------   ---------               -------          --------      */

/* 3          Aug 10:00:34 9         2271             xkeshavm      */
/*                                                                  */
/* To upload API CSL 0.51 Release                                   */
/********************************************************************/ 
