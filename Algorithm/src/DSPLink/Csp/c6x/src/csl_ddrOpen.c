/*   ==========================================================================
 *   Copyright (c) Texas Instruments Inc , 2004
 *
 *   Use of this software is controlled by the terms and conditions found
 *   in the license agreement under which this software has been supplied
 *   provided
 *   ==========================================================================
 */

/** @file csl_ddrOpen.c
 *
 *    @brief    File for functional layer of CSL API @a CSL_ddrOpen()
 *
 *  Description
 *    - The @a CSL_ddrOpen() function definition & it's associated functions
 *
 *
 *  @date 28 June, 2004
 *  @author Pratheesh Gangadhar
 */

#include <csl_ddr.h>
#include <_csl_resource.h>


#pragma CODE_SECTION (CSL_ddrOpen, ".text:csl_section:ddr");

extern void _CSL_ddrGetAttrs(CSL_DdrNum ddrNum , CSL_DdrHandle hDdr);

/** @brief Reserves the specified DDR for use. 
 */
CSL_DdrHandle CSL_ddrOpen (
    /** Pointer to the object that holds reference to the
     *  instance of DDR requested after the call 
	 */
    CSL_DdrObj              *hDdrObj,
    /** Instance of DDR to which a handle is requested 
	 */
    CSL_DdrNum              ddrNum,
    /** Specifies if DDR should be opened with exclusive or
     *  shared access to the associate pins 
	 */
    CSL_OpenMode            openMode,
    /** If a valid structure is passed (not @a NULL), then
     *  the @a CSL_ddrHwSetup() is called with this parameter 
	 */
    CSL_DdrHwSetup          *hwSetup,
    /** This returns the status (success/errors) of the call 
	 */
    CSL_Status              *status
	){

    CSL_DdrNum ddrInst;
    CSL_DdrHandle hDdr = (CSL_DdrHandle)NULL;

    hDdrObj->openMode = openMode;
    
    if (ddrNum >= 0 ) {
        _CSL_ddrGetAttrs(ddrNum, hDdrObj);
        hDdr = (CSL_DdrHandle)_CSL_certifyOpen((CSL_ResHandle)hDdrObj, status);
    }
    else {
		for(ddrInst=(CSL_DdrNum)0;ddrInst < (CSL_DdrNum)CSL_DDR_PER_CNT;++ddrInst){
			_CSL_ddrGetAttrs(ddrInst, hDdrObj);
            hDdr = (CSL_DdrHandle)_CSL_certifyOpen((CSL_ResHandle)hDdrObj, status);
            if (*status == CSL_SOK) 
                break;
        }
        if (ddrInst == CSL_DDR_PER_CNT) 
            *status = CSL_ESYS_OVFL;
    }

    if (*status == CSL_SOK && hwSetup != NULL) 
        CSL_ddrHwSetup(hDdr, hwSetup);

    return hDdr;
}
/* Rev.No.   Date/Time               ECN No.          Modifier      */
/* -------   ---------               -------          --------      */

/* 3          Aug 10:00:24 9         2271             xkeshavm      */
/*                                                                  */
/* To upload API CSL 0.51 Release                                   */
/********************************************************************/ 
