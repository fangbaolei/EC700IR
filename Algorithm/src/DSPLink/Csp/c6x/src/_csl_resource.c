/* ---- File: <_csl_resource.c> ---- */
#include <csl.h>
#include <_csl_resource.h>
#include <csl_sysData.h>

/* Assuming resInUse array elements to be 32 bits long
 * Calculate index and shift for the bit that represents given resId
 */
#define _CSL_GETINDEX(resId)		(resId >> 5)
#define _CSL_GETSHIFT(resId, index)	(resId -  32*index)

#pragma CODE_SECTION (_CSL_certifyOpen, ".text:csl_section:resource");

CSL_ResHandle _CSL_certifyOpen(CSL_ResHandle rHandle, CSL_Status *st)
{

    Uint16 restoreVal;

    restoreVal = _CSL_beginCriticalSection();

    *st = CSL_SOK;

	/* Bug fix: Clearquest bug PSG00000025 */
	/* Check for CSL_FAIL is done instead of CSL_ESYS_FAIL */
    //if(_CSL_resourceTest(((CSL_ResAttrs *)rHandle)->uid) == CSL_ESYS_FAIL)
    if(_CSL_resourceTest(((CSL_ResAttrs *)rHandle)->uid) == CSL_FAIL)
    {
        /*  This specific resource is already in use */
        *st = CSL_ESYS_INUSE;
        _CSL_endCriticalSection(restoreVal);
        return((CSL_ResHandle)NULL);
    }

    /* If no need for pin-sharing, treat this as Exclusive Open */
    if(((CSL_ResAttrs *)rHandle)->xio == 0)
        ((CSL_ResAttrs *)rHandle)->openMode = CSL_EXCLUSIVE;

    if(((CSL_ResAttrs *)rHandle)->openMode == CSL_EXCLUSIVE)
        {
          /* Check for absolute access to XIOs */
            if(_CSL_xioTest(((CSL_ResAttrs *)rHandle)->xio) == CSL_ESYS_FAIL)
            {
                *st = CSL_ESYS_XIO;
            }
            else
            {
                _CSL_xioSet(((CSL_ResAttrs *)rHandle)->xio);
                *st = CSL_SOK;
            }
        }

    if(*st == CSL_SOK) _CSL_resourceSet(((CSL_ResAttrs *)rHandle)->uid);

    /* END OF CRITICAL SECTION, as no Global data is being modified after this. */
    _CSL_endCriticalSection(restoreVal);

    if(CSL_sysDataHandle->altRouteHandler != NULL)
        (CSL_sysDataHandle->altRouteHandler)(rHandle);

    if(CSL_sysDataHandle->p2vHandler != NULL)
        (CSL_sysDataHandle->p2vHandler)(rHandle);

    return(rHandle);
}

#pragma CODE_SECTION (_CSL_certifyClose, ".text:csl_section:resource");

/* CSL Private Function Definition: 'CSL_certifyClose' */
CSL_Status _CSL_certifyClose(CSL_ResHandle rHandle)
{

    Uint16 restoreVal;

    restoreVal = _CSL_beginCriticalSection();
    if (((CSL_ResAttrs *)rHandle)->openMode == CSL_EXCLUSIVE)
        _CSL_xioClear(((CSL_ResAttrs *)rHandle)->xio);

    _CSL_resourceClear(((CSL_ResAttrs *)rHandle)->uid);

    _CSL_endCriticalSection(restoreVal);
    return CSL_SOK;
}

#pragma CODE_SECTION (_CSL_resourceTest, ".text:csl_section:resource");

/* FUNCTIONS DEFINED BELOW MODIFIES GLOBAL DATA STRUCTURE,
 * BELOW FUNCTIONS MUST BE CALLED IN ATOMIC REGION. */
CSL_Test _CSL_resourceTest(CSL_Uid resId)
{
    Uint32 index, shift;

    index = _CSL_GETINDEX(resId);
    shift = _CSL_GETSHIFT(resId, index);

    if ((CSL_sysDataHandle->resInUse[index] >> shift) & 0x1)
        return CSL_FAIL;
    else
        return CSL_PASS;
}

#pragma CODE_SECTION (_CSL_xioTest, ".text:csl_section:resource");

CSL_Test _CSL_xioTest(CSL_Xio xio)
{
    if (xio == 0) return CSL_PASS;
    if (xio & CSL_sysDataHandle->xioInUse)
        return CSL_FAIL;
    else
        return CSL_PASS;
}

#pragma CODE_SECTION (_CSL_xioSet, ".text:csl_section:resource");

void _CSL_xioSet(CSL_Xio xio)
{
    CSL_sysDataHandle->xioInUse |= xio;
    return;
}

#pragma CODE_SECTION (_CSL_resourceSet, ".text:csl_section:resource");

void _CSL_resourceSet(CSL_Uid resId)
{
    Uint32 index, shift;

    index = _CSL_GETINDEX(resId);
    shift = _CSL_GETSHIFT(resId, index);

    CSL_sysDataHandle->resInUse[index] |= (1L << shift);
    return;
}

#pragma CODE_SECTION (_CSL_resourceClear, ".text:csl_section:resource");

void _CSL_resourceClear(CSL_Uid resId)
{
    Uint32 index, shift;

    index = _CSL_GETINDEX(resId);
    shift = _CSL_GETSHIFT(resId, index);

    CSL_sysDataHandle->resInUse[index] &= ~(1L << shift);
    return;
}

#pragma CODE_SECTION (_CSL_xioClear, ".text:csl_section:resource");

void _CSL_xioClear(CSL_Xio xio)
{
    CSL_sysDataHandle->xioInUse &= ~xio;
    return;
}

/* Rev.No.   Date/Time               ECN No.          Modifier      */
/* -------   ---------               -------          --------      */

/* 1          Aug 11:10:50 9         2273             xkeshavm      */
/*                                                                  */
/* To upload API CSL 0.51 Release                                   */
/********************************************************************/ 
