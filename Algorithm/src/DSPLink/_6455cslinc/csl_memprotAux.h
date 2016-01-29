/* ============================================================================
 * Copyright (c) Texas Instruments Inc 2002, 2003, 2004, 2005                 
 *                                                                              
 * Use of this software is controlled by the terms and conditions found in the
 * license agreement under which this software has been supplied.             
 * ===========================================================================
 */ 

/** =========================================================================== 
 * @file  csl_memprotAux.h
 *
 * @brief  API Auxilary header file for MEMPROT CSL.
 *
 * @path $(CSLPATH)\inc
 *
 * Description
 *    - The different control command and status query functions definations
 *    
 *   Modification : 30th Aug 2004
 *   Modified function
 *   Added documentation
 *
 *   @author Ruchika Kharwar
 * ===========================================================================
 */

/* =============================================================================
 *  Revision History
 *  ===============
 *  15-Dec-2005  ds Removed Read access to Write only MPLKCMD register 
 * =============================================================================
 */
 
#ifndef _CSL_MEMPROTAUX_H
#define _CSL_MEMPROTAUX_H


#ifdef __cplusplus
extern "C" {
#endif

#define CSL_IDEF_INLINE static inline
/** @addtogroup CSL_MEMPROT_FUNCTION_INTERNAL
@{ */

/** ===========================================================================
 *   @n@b CSL_memprotLock
 *
 *   @b Description
 *   @n Locks access to the memory protection registers
 *
 *   @b Arguments
 *   @verbatim
            hMemprot            Handle to the MEMPROT instance
            key                 Key used to lock the memory module registers
            
     @endverbatim
 *
 *   <b> Return Value </b>  Status(CSL_SOK/CSL_ESYS_FAIL)
 *
 *   <b> Pre Condition </b>
 *   @n  All @a CSL_memprotInit(), @a CSL_memprotOpen()  must be
 *       invoked successfully in that order before this API can 
 *       be invoked.                                            
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
 
        CSL_MemprotObj mpL2Obj;
        CSL_MemprotHandle hmpL2;
        CSL_Status status;
        Uint32 key[2] = {0x11223344,0x55667788};
        
        // Initializing the module
        CSL_memprotInit(NULL);
        
        // Opening the Handle for the L2
        hmpL2 = CSL_memprotOpen(&mpL2Obj,CSL_MEMPROT_L2,NULL,&status);
        
        CSL_memprotLock(hmpL2,key);

     @endverbatim
 * =============================================================================
 */ 
CSL_IDEF_INLINE
CSL_Status CSL_memprotLock (
    CSL_MemprotHandle hMemprot,
    Uint32*           key
)
{
    if (CSL_FEXT(hMemprot->regs->MPLKSTAT,MEMPROTL1D_MPLKSTAT_LK)) {
        return CSL_ESYS_FAIL;
    }
    
    hMemprot->regs->MPLKCMD = CSL_FMKT(MEMPROTL1D_MPLKCMD_KEYR, YES);
    
    /* Since the L2, L1D, LIP all implement only 64 bit 
       keys this is sufficient */
    hMemprot->regs->MPLK0 = key[0];
    hMemprot->regs->MPLK1 = key[1];
    hMemprot->regs->MPLK2 = 0;
    hMemprot->regs->MPLK3 = 0;
    
    hMemprot->regs->MPLKCMD = CSL_FMKT(MEMPROTL1D_MPLKCMD_LOCK, YES);
    
    return CSL_SOK;
}


/** ===========================================================================
 *   @n@b CSL_memprotUnLock
 *
 *   @b Description
 *   @n Unlocks access to the memory protection registers
 *
 *   @b Arguments
 *   @verbatim
            hMemprot            Handle to the MEMPROT instance
            key                 Key used to lock the memory module registers
            
     @endverbatim
 *
 *   <b> Return Value </b>  Status(CSL_SOK/CSL_ESYS_FAIL)
 *
 *   <b> Pre Condition </b>
 *   @n  All @a CSL_memprotInit(), @a CSL_memprotOpen()  must be
 *       invoked successfully in that order before this API can 
 *       be invoked.                                            
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
 
        CSL_MemprotObj mpL2Obj;
        CSL_MemprotHandle hmpL2;
        CSL_Status status;
        Uint32 key[2] = {0x11223344,0x55667788};
        
        // Initializing the module
        CSL_memprotInit(NULL);
        
        // Opening the Handle for the L2
        hmpL2 = CSL_memprotOpen(&mpL2Obj,CSL_MEMPROT_L2,NULL,&status);
        
        CSL_memprotUnLock(hmpL2,key);

     @endverbatim
 * =============================================================================
 */ 
CSL_IDEF_INLINE
CSL_Status CSL_memprotUnLock (
    CSL_MemprotHandle hMemprot,
    Uint32*           key
)
{
    if (!CSL_FEXT(hMemprot->regs->MPLKSTAT,MEMPROTL1D_MPLKSTAT_LK)) {
        return CSL_ESYS_FAIL;
    }
    
    hMemprot->regs->MPLKCMD = CSL_FMKT(MEMPROTL1D_MPLKCMD_KEYR, YES);
    
    /* Since the L2, L1D, LIP all implement only 64 bit 
       keys this is sufficient */
    hMemprot->regs->MPLK0 = key[0];
    hMemprot->regs->MPLK1 = key[1];
    hMemprot->regs->MPLK2 = 0;
    hMemprot->regs->MPLK3 = 0;
    
    hMemprot->regs->MPLKCMD = CSL_FMKT(MEMPROTL1D_MPLKCMD_UNLOCK, YES);
    
    return CSL_SOK;
}

/** ===========================================================================
 *   @n@b CSL_memprotSetPageAttr
 *
 *   @b Description
 *   @n Sets the page access attributes.
 *
 *   @b Arguments
 *   @verbatim
            hMemprot            Handle to the memory protection unit
            page                Page number
            attr                Attributes
            
     @endverbatim
 *
 *   <b> Return Value </b>  None
 *
 *   <b> Pre Condition </b>
 *   @n  All @a CSL_memprotInit(), @a CSL_memprotOpen()  must be
 *       invoked successfully in that order before this API can 
 *       be invoked.                                            
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
 
        CSL_MemprotObj mpL2Obj;
        CSL_MemprotHandle hmpL2;
        CSL_Status status;
        
        // Initializing the module
        CSL_memprotInit(NULL);
        
        // Opening the Handle for the L2
        hmpL2 = CSL_memprotOpen(&mpL2Obj,CSL_MEMPROT_L2,NULL,&status);
        
        CSL_memprotSetPageAttr(hmpL2,12,PAGE_ATTR|CSL_MEMPROT_MEMACCESS_UR);

     @endverbatim
 * =============================================================================
 */ 
CSL_IDEF_INLINE
void CSL_memprotSetPageAttr (
    CSL_MemprotHandle hMemprot,
    Uint32            page,
    Uint32            attr
)
{
    hMemprot->regs->MPPA[page] = attr;
}


/** ===========================================================================
 *   @n@b CSL_memprotGetPageAttr
 *
 *   @b Description
 *   @n Queries the page access attributes.
 *
 *   @b Arguments
 *   @verbatim
            hMemprot            Handle to the memory protection unit
            page                Page number
            attr                Attributes
            
     @endverbatim
 *
 *   <b> Return Value </b>  Page Attributes
 *
 *   <b> Pre Condition </b>
 *   @n  All @a CSL_memprotInit(), @a CSL_memprotOpen()  must be
 *       invoked successfully in that order before this API can 
 *       be invoked.                                            
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
 
        CSL_MemprotObj mpL2Obj;
        CSL_MemprotHandle hmpL2;
        CSL_Status status;
        CSL_BitMask32 pageAttr;
        
        // Initializing the module
        CSL_memprotInit(NULL);
        
        // Opening the Handle for the L2
        hmpL2 = CSL_memprotOpen(&mpL2Obj,CSL_MEMPROT_L2,NULL,&status);
        
        pageAttr = CSL_memprotGetPageAttr(hmpL2,12);

     @endverbatim
 * =============================================================================
 */                            
CSL_IDEF_INLINE
CSL_BitMask32 CSL_memprotGetPageAttr (
    CSL_MemprotHandle hMemprot,
    Uint16            page
)
{
    return hMemprot->regs->MPPA[page];
}


/** ===========================================================================
 *   @n@b CSL_memprotGetFaultStatus
 *
 *   @b Description
 *   @n Queries the fault id of the defaulter access
 *
 *   @b Arguments
 *   @verbatim
            hMemprot            Handle to the memory protection unit
            faultStatus         fault Status 
            
     @endverbatim
 *
 *   <b> Return Value </b>  None
 *
 *   <b> Pre Condition </b>
 *   @n  All @a CSL_memprotInit(), @a CSL_memprotOpen()  must be
 *       invoked successfully in that order before this API can 
 *       be invoked.                                            
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
        
        CSL_MemprotObj mpL2Obj;
        CSL_MemprotHandle hmpL2;
        CSL_Status status;
        CSL_BitMask32 pageAttr;
        CSL_MemprotFaultStatus faultStat;
        // Initializing the module
        CSL_memprotInit(NULL);
        
        // Opening the Handle for the L2
        hmpL2 = CSL_memprotOpen(&mpL2Obj,CSL_MEMPROT_L2,NULL,&status);
        
        CSL_memprotGetFaultStatus(hmpL2,&faultStat);

     @endverbatim
 * =============================================================================
 */                 
CSL_IDEF_INLINE
void CSL_memprotGetFaultStatus (
    CSL_MemprotHandle      hMemprot,
    CSL_MemprotFaultStatus *faultStatus
)
{
    Uint32 stat;
    
    faultStatus->addr = hMemprot->regs->MPFAR ;
    stat = hMemprot->regs->MPFSR ;
    
    faultStatus->errorMask = CSL_FEXTR(stat,8,0);
    faultStatus->fid = CSL_FEXTR(stat,15,9);
    
    /* Clearing the Memory Fault Error */
    hMemprot->regs->MPFCR = 1;
}

/** ===========================================================================
 *   @n@b CSL_memprotGetLockStat
 *
 *   @b Description
 *   @n Queries the lock status of the registers.
 *
 *   @b Arguments
 *   @verbatim
            hMemprot            Handle to the memory protection unit
            
     @endverbatim
 *
 *   <b> Return Value </b>  Lock Status
 *
 *   <b> Pre Condition </b>
 *   @n  All @a CSL_memprotInit(), @a CSL_memprotOpen()  must be
 *       invoked successfully in that order before this API can 
 *       be invoked.                                            
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
        
        CSL_MemprotObj mpL2Obj;
        CSL_MemprotHandle hmpL2;
        CSL_Status status;
        CSL_MemprotLockStatus lockAttr;
        
        // Initializing the module 
        CSL_memprotInit(NULL);
        
        // Opening the Handle for the L2 
        hmpL2 = CSL_memprotOpen(&mpL2Obj,CSL_MEMPROT_L2,NULL,&status);
        
        lockAttr = CSL_memprotGetLockStat(hmpL2);

     @endverbatim
 * =============================================================================
 */      
CSL_IDEF_INLINE
CSL_MemprotLockStatus CSL_memprotGetLockStat (
    CSL_MemprotHandle hMemprot
)
{
    return ((CSL_MemprotLockStatus)(CSL_FEXTR(hMemprot->regs->MPLKSTAT,0,0)));
}

#ifdef __cplusplus
}
#endif

#endif
/**
@}
*/
