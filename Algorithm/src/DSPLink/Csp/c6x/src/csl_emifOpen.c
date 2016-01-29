/** @file csl_emifOpen.c
 *
 *    @brief    File for functional layer of CSL API @a CSL_emifOpen()
 *
 *  Description
 *    - The @a CSL_emifOpen() function definition & it's associated functions
 *
 *  Modification 1
 *    - Created on: 10/06/2004
 *    - Reason:       Created the sources
 *
 *  @date 10th June, 2004
 *  @author Santosh Narayanan.
 */

#include <csl_emif.h>
#include <_csl_resource.h>


#pragma CODE_SECTION (CSL_emifOpen, ".text:csl_section:emif");

extern void _CSL_emifGetAttrs(CSL_EmifNum emifNum , CSL_EmifHandle hEmif);

/** @brief Reserves the specified EMIF for use. 
 */
CSL_EmifHandle CSL_emifOpen (
    /** Pointer to the object that holds reference to the
     *  instance of EMIF requested after the call 
	 */
    CSL_EmifObj              *hEmifObj,
    /** Instance of EMIF to which a handle is requested 
	 */
    CSL_EmifNum              emifNum,
    /** Specifies if EMIF should be opened with exclusive or
     *  shared access to the associate pins 
	 */
    CSL_OpenMode            openMode,
    /** If a valid structure is passed (not @a NULL), then
     *  the @a CSL_emifHwSetup() is called with this parameter 
	 */
    CSL_EmifHwSetup          *hwSetup,
    /** This returns the status (success/errors) of the call 
	 */
    CSL_Status              *status
	){

    CSL_EmifNum emifInst;
    CSL_EmifHandle hEmif = (CSL_EmifHandle)NULL;

    hEmifObj->openMode = openMode;
    
    if (emifNum >= 0 ) {
        _CSL_emifGetAttrs(emifNum, hEmifObj);
        hEmif = (CSL_EmifHandle)_CSL_certifyOpen((CSL_ResHandle)hEmifObj, status);
    }
    else {
		for(emifInst=(CSL_EmifNum)0;emifInst < (CSL_EmifNum)CSL_EMIF_PER_CNT;++emifInst){
			_CSL_emifGetAttrs(emifInst, hEmifObj);
            hEmif = (CSL_EmifHandle)_CSL_certifyOpen((CSL_ResHandle)hEmifObj, status);
            if (*status == CSL_SOK) 
                break;
        }
        if (emifInst == CSL_EMIF_PER_CNT) 
            *status = CSL_ESYS_OVFL;
    }

    if (*status == CSL_SOK && hwSetup != NULL) 
        CSL_emifHwSetup(hEmif, hwSetup);

    return hEmif;
}
/* Rev.No.   Date/Time               ECN No.          Modifier      */
/* -------   ---------               -------          --------      */

/* 3          Aug 10:00:35 9         2271             xkeshavm      */
/*                                                                  */
/* To upload API CSL 0.51 Release                                   */
/********************************************************************/ 
