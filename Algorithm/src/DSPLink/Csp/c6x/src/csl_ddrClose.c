/*   ==========================================================================
 *   Copyright (c) Texas Instruments Inc , 2004
 *
 *   Use of this software is controlled by the terms and conditions found
 *   in the license agreement under which this software has been supplied
 *   provided
 *   ==========================================================================
 */
/** @file csl_ddrClose.c
 *
 *  @brief File for functional layer of CSL API @a CSL_ddrClose()
 *
 *  Description
 *    - The @a CSL_ddrClose() function definition & it's associated functions
 *
 *  @date 28 June, 2004
 *  @author Pratheesh Gangadhar
 */
#include <csl_ddr.h>
#include <_csl_resource.h>

#pragma CODE_SECTION (CSL_ddrClose, ".text:csl_section:ddr");


/** @brief Unreserves the DDR identified by the handle.
 */
CSL_Status  CSL_ddrClose(
    /** Pointer to the object that holds reference to the
     *  instance of DDR requested after the call 
	 */
    CSL_DdrHandle                         hDdr
	){
    /* Indicate in the CSL global data structure that the peripheral
     * has been unreserved 	 */
	return(_CSL_certifyClose((CSL_ResHandle)hDdr));
}

/* Rev.No.   Date/Time               ECN No.          Modifier      */
/* -------   ---------               -------          --------      */

/* 3          Aug 10:00:21 9         2271             xkeshavm      */
/*                                                                  */
/* To upload API CSL 0.51 Release                                   */
/********************************************************************/ 
