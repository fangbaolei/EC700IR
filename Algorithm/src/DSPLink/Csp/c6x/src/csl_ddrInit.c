/*   ==========================================================================
 *   Copyright (c) Texas Instruments Inc , 2004
 *
 *   Use of this software is controlled by the terms and conditions found
 *   in the license agreement under which this software has been supplied
 *   provided
 *   ==========================================================================
 */

/** @file csl_ddrInit.c
 *
 *    @brief    File for functional layer of CSL API @a CSL_ddrInit()
 *
 *  Description
 *    - The @a CSL_ddrInit() function definition & it's associated functions
 *
 *
 *  @date 28 June, 2004
 *  @author Pratheesh Gangadhar
 */

#include <csl_ddr.h>

#pragma CODE_SECTION (CSL_ddrInit, ".text:csl_section:ddr");

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
 *      CSL_ddrInit();
 *   }
 *  @endverbatim
 *
 *  @return returns the status of the operation
 *
 */
CSL_Status  CSL_ddrInit (
    void
	){

    return CSL_SOK;
}

/* Rev.No.   Date/Time               ECN No.          Modifier      */
/* -------   ---------               -------          --------      */

/* 3          Aug 10:00:24 9         2271             xkeshavm      */
/*                                                                  */
/* To upload API CSL 0.51 Release                                   */
/********************************************************************/ 
