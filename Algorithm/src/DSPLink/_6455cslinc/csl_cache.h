/*  ============================================================================
 *   Copyright (c) Texas Instruments Inc 2002, 2003, 2004, 2005
 *
 *   Use of this software is controlled by the terms and conditions found in the
 *   license agreement under which this software has been supplied.
 *   ===========================================================================
 */
 
/** ============================================================================
 *   @file  csl_cache.h
 *
 *   @path  $(CSLPATH)\inc
 *
 *   @desc  Header file for functional layer CSL of CACHE
 *
 */

/** @mainpage CACHE 
 *
 * @section Introduction
 *
 * @subsection xxx Purpose and Scope
 * The purpose of this document is to detail the  Cache APIs 
 *
 * @subsection aaa Terms and Abbreviations
 *   -# CSL:  Chip Support Library
 *   -# API:  Application Programmer Interface
 *
 * @subsection References
 *    -# Cache L2 
 *    -# Cache L1D
 *    -# Cache L1P 
 *
 */

/* =============================================================================
 *  Revision History
 *  ===============
 *  23-Mar-2004  Jamon Bowen File Created.
 *  21-Jun-2004  Ruchika Kharwar modified.
 *  15-Dec-2005  ds Removed EmifA CE0 ranges from the CE_MAR Enum
 * =============================================================================
 */
 
#ifndef _CSL_CACHE_H_
#define _CSL_CACHE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <csl.h>
#include <cslr_cache.h>
#include <soc.h>

/** L2 Line Size */
#define CACHE_L2_LINESIZE    128 
/** L1D Line Size */
#define CACHE_L1D_LINESIZE    64
/** L1P Line Size */
#define CACHE_L1P_LINESIZE    32


/** Cache Round to Linesize */
#define CACHE_ROUND_TO_LINESIZE(CACHE,ELCNT,ELSIZE)          \
        ((CACHE_##CACHE##_LINESIZE *                         \
        ((ELCNT)*(ELSIZE)/CACHE_##CACHE##_LINESIZE) + 1) /   \
        (ELSIZE))

/******************************************************************************\
* global macro declarations
\******************************************************************************/

/** Enumeration for Emif ranges
 *  This is used for setting up the cacheability of the EMIF ranges. 
 */
typedef enum {
    
    /** EMIFA CE2 Range A0000000 - A0FFFFFF */
    CACHE_EMIFA_CE20  =   160,
    /** EMIFA CE2 Range A1000000 - A1FFFFFF */
    CACHE_EMIFA_CE21  =   161,
    /** EMIFA CE2 Range A2000000 - A2FFFFFF */
    CACHE_EMIFA_CE22  =   162,
    /** EMIFA CE2 Range A3000000 - A3FFFFFF */
    CACHE_EMIFA_CE23  =   163,
    /** EMIFA CE2 Range A4000000 - A4FFFFFF */
    CACHE_EMIFA_CE24  =   164,
    /** EMIFA CE2 Range A5000000 - A5FFFFFF */
    CACHE_EMIFA_CE25  =   165,
    /** EMIFA CE2 Range A6000000 - A6FFFFFF */
    CACHE_EMIFA_CE26  =   166,
    /** EMIFA CE2 Range A7000000 - A7FFFFFF */
    CACHE_EMIFA_CE27  =   167,
    /** EMIFA CE2 Range A8000000 - A8FFFFFF */
    CACHE_EMIFA_CE28  =   168,
    /** EMIFA CE2 Range A9000000 - A9FFFFFF */
    CACHE_EMIFA_CE29  =   169,
    /** EMIFA CE2 Range AA000000 - AAFFFFFF */
    CACHE_EMIFA_CE210 =   170,
    /** EMIFA CE2 Range AB000000 - ABFFFFFF */
    CACHE_EMIFA_CE211 =   171,
    /** EMIFA CE2 Range AC000000 - ACFFFFFF */
    CACHE_EMIFA_CE212 =   172,
    /** EMIFA CE2 Range AD000000 - ADFFFFFF */
    CACHE_EMIFA_CE213 =   173,
    /** EMIFA CE2 Range AE000000 - AEFFFFFF */
    CACHE_EMIFA_CE214 =   174,
    /** EMIFA CE2 Range AF000000 - AFFFFFFF */
    CACHE_EMIFA_CE215 =   175,
    
    /** EMIFA CE3 Range B0000000 - B0FFFFFF */
    CACHE_EMIFA_CE30  =   176,
    /** EMIFA CE3 Range B1000000 - B1FFFFFF */
    CACHE_EMIFA_CE31  =   177,
    /** EMIFA CE3 Range B2000000 - B2FFFFFF */
    CACHE_EMIFA_CE32  =   178,
    /** EMIFA CE3 Range B3000000 - B3FFFFFF */
    CACHE_EMIFA_CE33  =   179,
    /** EMIFA CE3 Range B4000000 - B4FFFFFF */
    CACHE_EMIFA_CE34  =   180,
    /** EMIFA CE3 Range B5000000 - B5FFFFFF */
    CACHE_EMIFA_CE35  =   181,
    /** EMIFA CE3 Range B6000000 - B6FFFFFF */
    CACHE_EMIFA_CE36  =   182,
    /** EMIFA CE3 Range B7000000 - B7FFFFFF */
    CACHE_EMIFA_CE37  =   183,
    /** EMIFA CE3 Range B8000000 - B8FFFFFF */
    CACHE_EMIFA_CE38  =   184,
    /** EMIFA CE3 Range B9000000 - B9FFFFFF */
    CACHE_EMIFA_CE39  =   185,
    /** EMIFA CE3 Range BA000000 - BAFFFFFF */
    CACHE_EMIFA_CE310 =   186,
    /** EMIFA CE3 Range BB000000 - BBFFFFFF */
    CACHE_EMIFA_CE311 =   187,
    /** EMIFA CE3 Range BC000000 - BCFFFFFF */
    CACHE_EMIFA_CE312 =   188,
    /** EMIFA CE3 Range BD000000 - BDFFFFFF */
    CACHE_EMIFA_CE313 =   189,
    /** EMIFA CE3 Range BE000000 - BEFFFFFF */
    CACHE_EMIFA_CE314 =   190,
    /** EMIFA CE3 Range BF000000 - BFFFFFFF */
    CACHE_EMIFA_CE315 =   191,
    
    /** EMIFA CE4 Range C0000000 - C0FFFFFF */
    CACHE_EMIFA_CE40  =   192,
    /** EMIFA CE4 Range C1000000 - C1FFFFFF */
    CACHE_EMIFA_CE41  =   193,
    /** EMIFA CE4 Range C2000000 - C2FFFFFF */
    CACHE_EMIFA_CE42  =   194,
    /** EMIFA CE4 Range C3000000 - C3FFFFFF */
    CACHE_EMIFA_CE43  =   195,
    /** EMIFA CE4 Range C4000000 - C4FFFFFF */
    CACHE_EMIFA_CE44  =   196,
    /** EMIFA CE4 Range C5000000 - C5FFFFFF */
    CACHE_EMIFA_CE45  =   197,
    /** EMIFA CE4 Range C6000000 - C6FFFFFF */
    CACHE_EMIFA_CE46  =   198,
    /** EMIFA CE4 Range C7000000 - C7FFFFFF */
    CACHE_EMIFA_CE47  =   199,
    /** EMIFA CE4 Range C8000000 - C8FFFFFF */
    CACHE_EMIFA_CE48  =   200,
    /** EMIFA CE4 Range C9000000 - C9FFFFFF */
    CACHE_EMIFA_CE49  =   201,
    /** EMIFA CE4 Range CA000000 - CAFFFFFF */
    CACHE_EMIFA_CE410 =   202,
    /** EMIFA CE4 Range CB000000 - CBFFFFFF */
    CACHE_EMIFA_CE411 =   203,
    /** EMIFA CE4 Range CC000000 - CCFFFFFF */
    CACHE_EMIFA_CE412 =   204,
    /** EMIFA CE4 Range CD000000 - CDFFFFFF */
    CACHE_EMIFA_CE413 =   205,
    /** EMIFA CE4 Range CE000000 - CEFFFFFF */
    CACHE_EMIFA_CE414 =   206,
    /** EMIFA CE4 Range CF000000 - CFFFFFFF */
    CACHE_EMIFA_CE415 =   207,
    
    
    /** EMIFA CE5 Range D0000000 - D0FFFFFF */
    CACHE_EMIFA_CE50  =   208,
    /** EMIFA CE5 Range D1000000 - D1FFFFFF */
    CACHE_EMIFA_CE51  =   209,
    /** EMIFA CE5 Range D2000000 - D2FFFFFF */
    CACHE_EMIFA_CE52  =   210,
    /** EMIFA CE5 Range D3000000 - D3FFFFFF */
    CACHE_EMIFA_CE53  =   211,
    /** EMIFA CE5 Range D4000000 - D4FFFFFF */
    CACHE_EMIFA_CE54  =   212,
    /** EMIFA CE5 Range D5000000 - D5FFFFFF */
    CACHE_EMIFA_CE55  =   213,
    /** EMIFA CE5 Range D6000000 - D6FFFFFF */
    CACHE_EMIFA_CE56  =   214,
    /** EMIFA CE5 Range D7000000 - D7FFFFFF */
    CACHE_EMIFA_CE57  =   215,
    /** EMIFA CE5 Range D8000000 - D8FFFFFF */
    CACHE_EMIFA_CE58  =   216,
    /** EMIFA CE5 Range D9000000 - D9FFFFFF */
    CACHE_EMIFA_CE59  =   217,
    /** EMIFA CE5 Range DA000000 - DAFFFFFF */
    CACHE_EMIFA_CE510 =   218,
    /** EMIFA CE5 Range DB000000 - DBFFFFFF */
    CACHE_EMIFA_CE511 =   219,
    /** EMIFA CE5 Range DC000000 - DCFFFFFF */
    CACHE_EMIFA_CE512 =   220,
    /** EMIFA CE5 Range DD000000 - DDFFFFFF */
    CACHE_EMIFA_CE513 =   221,
    /** EMIFA CE5 Range DE000000 - DEFFFFFF */
    CACHE_EMIFA_CE514 =   222,
    /** EMIFA CE5 Range DF000000 - DFFFFFFF */
    CACHE_EMIFA_CE515 =   223,
    
    /** DDR2 CE0 Range E0000000 - E0FFFFFF */
    CACHE_EMIFB_CE00  =   224,
    /** DDR2 CE0 Range E1000000 - E1FFFFFF */
    CACHE_EMIFB_CE01  =   225,
    /** DDR2 CE0 Range E2000000 - E2FFFFFF */
    CACHE_EMIFB_CE02  =   226,
    /** DDR2 CE0 Range E3000000 - E3FFFFFF */
    CACHE_EMIFB_CE03  =   227,
    /** DDR2 CE0 Range E4000000 - E4FFFFFF */
    CACHE_EMIFB_CE04  =   228,
    /** DDR2 CE0 Range E5000000 - E5FFFFFF */
    CACHE_EMIFB_CE05  =   229,
    /** DDR2 CE0 Range E6000000 - E6FFFFFF */
    CACHE_EMIFB_CE06  =   230,
    /** DDR2 CE0 Range E7000000 - E7FFFFFF */
    CACHE_EMIFB_CE07  =   231,
    /** DDR2 CE0 Range E8000000 - E8FFFFFF */
    CACHE_EMIFB_CE08  =   232,
    /** DDR2 CE0 Range E9000000 - E9FFFFFF */
    CACHE_EMIFB_CE09  =   233,
    /** DDR2 CE0 Range EA000000 - EAFFFFFF */
    CACHE_EMIFB_CE010 =   234,
    /** DDR2 CE0 Range EB000000 - EBFFFFFF */
    CACHE_EMIFB_CE011 =   235,
    /** DDR2 CE0 Range EC000000 - ECFFFFFF */
    CACHE_EMIFB_CE012 =   236,
    /** DDR2 CE0 Range ED000000 - EDFFFFFF */
    CACHE_EMIFB_CE013 =   237,
    /** DDR2 CE0 Range EE000000 - EEFFFFFF */
    CACHE_EMIFB_CE014 =   238,
    /** DDR2 CE0 Range EF000000 - EFFFFFFF */
    CACHE_EMIFB_CE015 =   239
} CE_MAR;

    
/** @brief Enumeration for Cache wait flags
 *
 *  This is used for specifying whether the cache operations should block till
 *  the desired operation is complete.
 */

typedef enum {
    /** No blocking, the call exits after programmation of the 
     *  control registers 
     */
    CACHE_NOWAIT = 0,                     
    /** Blocking Call, the call exits after the relevant cache 
     *  status registers indicate completion
     */
    CACHE_WAITINTERNAL = 1,               
    /** Blocking Call, the call waits not only till the cache status registers 
     *  indicate completion, but also till a write read is issued to the 
     *  EMIF registers (if required)
     */
    CACHE_WAIT = 2                        
                                               
}CACHE_Wait;

/** @brief Enumeration for Cache Freeze flags
 *  This is used for reporting back the current state of the L1.
 */
typedef enum {
    /** L1D is in Normal State */ 
    CACHE_L1D_NORMAL = 0,
    /** L1D is in Freeze State */                 
    CACHE_L1D_FREEZE = 1,
    /** L1P is in Normal State */                 
    CACHE_L1P_NORMAL = 3,
    /** L1P is in Freeze State */                 
    CACHE_L1P_FREEZE = 4,
    /** L1D, L1P is in Normal State */                 
    CACHE_L1_NORMAL = 5,
    /** L1D, L1P is in Freeze State */                  
    CACHE_L1_FREEZE = 6                   
} CACHE_L1_Freeze;

/** @brief Enumeration for L1 (P or D) Sizes */

typedef enum { 
    /** No Cache    */                 
    CACHE_L1_0KCACHE  = 0,
    /** 4KB Cache   */                               
    CACHE_L1_4KCACHE  = 1,
    /** 8KB Cache   */                 
    CACHE_L1_8KCACHE  = 2,
    /** 16KB Cache  */                                 
    CACHE_L1_16KCACHE = 3,
    /** 32KB Cache  */                
    CACHE_L1_32KCACHE = 4 
} CACHE_L1Size; 

/** @brief Enumeration for L2 Sizes */
typedef enum { 
    /** No Cache     */
    CACHE_0KCACHE   = CSL_CACHE_L2CFG_MODE_0K,  
    /** 32KB Cache   */
    CACHE_32KCACHE  = CSL_CACHE_L2CFG_MODE_32K, 
    /** 64KB Cache   */
    CACHE_64KCACHE  = CSL_CACHE_L2CFG_MODE_64K, 
    /** 128KB Cache  */
    CACHE_128KCACHE = CSL_CACHE_L2CFG_MODE_128K,
    /** 256KB Cache  */
    CACHE_256KCACHE = CSL_CACHE_L2CFG_MODE_256K 
} CACHE_L2Size; 

/** @brief Enumeration for L2 Modes */
typedef enum {
    /** Enabled/Normal Mode */
    CACHE_L2_NORMAL = CSL_CACHE_L2CFG_L2CC_ENABLED,
    /** Freeze Mode */                  
    CACHE_L2_FREEZE = CSL_CACHE_L2CFG_L2CC_FREEZE
} CACHE_L2Mode;

/*
 * =============================================================================
 *   @n@b CACHE_enableCaching
 *
 *   @desc
 *      @n Enables caching for the specified memory range
 *
 *   @arg   mar 
 *          EMIF range
 *
 *   @ret   None
 *          
 *   @eg
 *      ...
        CACHE_enableCaching (CACHE_EMIFB_CE00);
        ...
 *
 * ===========================================================================
 */
extern void CACHE_enableCaching (
    CE_MAR      mar      
);

/*
 * =============================================================================
 *   @n@b CACHE_wait
 *
 *   @desc
 *      @n Waits for previously issued block operations to complete. This does a
 *      partial wait. i.e waits for the cache status register to read back as
 *      done.
 *
 *   @arg   None
 *
 *   @ret   None
 *          
 *   @eg
 *      ...
        CACHE_wait();
        ...
 *
 * ===========================================================================
 */
extern void CACHE_wait(void);

/*
 * =============================================================================
 *   @n@b CACHE_waitInternal
 *
 *   @desc
 *      @n Waits for previously issued block operations to complete. This does a
 *      partial wait. i.e waits for the cache status register to read back as
 *      done. (if required)
 *
 *   @arg   None
 *
 *   @ret   None
 *          
 *   @eg
 *      ...
        CACHE_waitInternal();
        ...
 *
 * ===========================================================================
 */
extern void CACHE_waitInternal(void);

/*
 * =============================================================================
 *   @n@b CACHE_freezeL1
 *
 *   @desc
 *      @n Freezes the L1P and L1D.
 *      @n As per the specification,
 *      @n a. The new freeze state is programmed in L1DCC, L1PCC.
 *      @n b. The old state is read from the L1DCC, L1PCC from the POPER field.
 *      @n This latter read accomplishes 2 things, viz. Ensuring the new state
 *         is programmed as well as reading the old programmed value.
 *
 *   @arg   None
 *
 *   @ret   CACHE_L1_Freeze
 *          Old Freeze State
 *   @eg
 *      ...
        CACHE_L1_Freeze oldFreezeState ;

        oldFreezeState = CACHE_freezeL1();
        ...
 *
 * ===========================================================================
 */
extern CACHE_L1_Freeze CACHE_freezeL1(void);

/*
 * =============================================================================
 *   @n@b CACHE_unfreezeL1
 *
 *   @desc
 *      @n Unfreezes the L1P and L1D.
 *      @n As per the specification,
 *      @n a. The new unfreeze state is programmed in L1DCC, L1PCC.
 *      @n b. The old state is read from the L1DCC, L1PCC from the POPER field.
 *      @n    This latter read accomplishes 2 things, viz. Ensuring the new 
 *            state is programmed as well as reading the old programmed value.
 *
 *   @arg   None
 *
 *   @ret   CACHE_L1_Freeze
 *          Old Freeze State
 *   @eg
 *      ...
        CACHE_L1_Freeze oldFreezeState ;

        oldFreezeState = CACHE_unfreezeL1();
        ...
 *
 * ===========================================================================
 */
extern CACHE_L1_Freeze CACHE_unfreezeL1(void);

/*
 * =============================================================================
 *   @n@b CACHE_setL1pSize
 *
 *   @desc
 *      @n Sets the L1P size.
 *      @n As per the specification,
 *      @n a. The new size is programmed in L1PCFG.
 *      @n b. L1PCFG is read back to ensure it is set.
 *
 *   @arg   newSize
 *          New size to be programmed
 *
 *   @ret   CACHE_L1Size
 *          Old Size set for L1P
 *   @eg
 *      ...
        CACHE_L1Size oldSize ;

        oldSize = CACHE_setL1pSize(CACHE_L1_32KCACHE);
        ...
 *
 * ===========================================================================
 */
extern CACHE_L1Size CACHE_setL1pSize (
    CACHE_L1Size    newSize   
);

/*
 * =============================================================================
 *   @n@b CACHE_freezeL1p
 *
 *   @desc
 *      @n Freezes L1P.
 *      @n As per the specification,
 *      @n a. The new freeze state is programmed in L1PCC.
 *      @n b. The old state is read from the L1PCC from the POPER field.
 *      @n    This latter read accomplishes 2 things, viz. Ensuring the new 
 *            state is programmed as well as reading the old programmed value.
 *
 *   @arg   None
 *
 *   @ret   CACHE_L1_Freeze
 *          Old Freeze State
 *   @eg
 *      ...
        CACHE_L1_Freeze oldFreezeState ;

        oldFreezeState = CACHE_freezeL1p();
        ...
 *
 * ===========================================================================
 */
extern CACHE_L1_Freeze CACHE_freezeL1p(void);

/*
 * =============================================================================
 *   @n@b CACHE_unfreezeL1p
 *
 *   @desc
 *      @n Unfreezes L1P.
 *      @n As per the specification,
 *      @n a. The normal state is programmed in L1PCC
 *      @n b. The old state is read from the L1PCC from the POPER field.
 *      @n    This latter read accomplishes 2 things, viz. Ensuring the new 
 *            state is programmed as well as reading the old programmed value.
 *
 *   @arg   None
 *
 *   @ret   CACHE_L1_Freeze
 *          Old Freeze State
 *   @eg
 *       ...
        CACHE_L1_Freeze oldFreezeState ;

        oldFreezeState = CACHE_unfreezeL1p();
        ...
 *
 * ============================================================================
 */
extern CACHE_L1_Freeze CACHE_unfreezeL1p(void);

/*
 * =============================================================================
 *   @n@b CACHE_invL1p
 *
 *   @desc
 *      @n Invalidates range specified in L1P.
 *      @n As per the specification,
 *      @n a. The start of the range that needs to be invalidated is written
 *         into L1PIBAR
 *      @n b. The byte count is programmed in L1PIWC.
 *   
 *   @arg   blockPtr
 *          Pointer to start address of range to be invalidated
 *
 *   @arg   byteCnt
 *          Number of bytes to be invalidated
 *  
 *   @arg   wait
 *          Whether the call is blocking (and the extent of wait) till the 
 *          issued operation is completed or not.
 *
 *   @ret   None
 *
 *   @eg
 *      ...
        CACHE_invL1p ((Uint32*)(0x1000), 200, CACHE_NOWAIT);
        ...
 *
 * ===========================================================================
 */    
extern void CACHE_invL1p (
    void          *blockPtr,           
    Uint32        byteCnt,           
    CACHE_Wait    wait          
);

/*
 * =============================================================================
 *   @n@b CACHE_invAllL1p
 *
 *   @desc
 *      Sets the L1D size. As per the specification,
 *       - The new size is programmed in L1DCFG
 *       - L1DCFG is read back to ensure it is set
 *
 *   @arg   wait
 *          Whether the call is blocking(and the extent of wait)  till the 
 *          issued operation is completed or not 
 *
 *   @ret   None
 *
 *   @eg
 *      ... 
        oldSize = CACHE_invAllL1p(CACHE_L1_32KCACHE);
        ...
 *
 * ===========================================================================
 */
extern void CACHE_invAllL1p (
    CACHE_Wait  wait                 
);

/*
 * =============================================================================
 *   @n@b CACHE_setL1dSize
 *
 *   @desc
 *      Sets the L1D size. As per the specification,
 *       - The new size is programmed in L1DCFG
 *       - L1DCFG is read back to ensure it is set
 *
 *   @arg   newSize
 *          New size to be programmed
 *
 *   @ret   CACHE_L1Size
 *          Old Size set for L1D 
 *   @eg
 *      CACHE_L1Size oldSize;
        ... 
        oldSize = CACHE_setL1dSize(CACHE_L1_32KCACHE);
        ...
 *
 * ===========================================================================
 */
extern CACHE_L1Size CACHE_setL1dSize (
    CACHE_L1Size    newSize                 
);

/*
 * =============================================================================
 *   @n@b CACHE_freezeL1d
 *
 *   @desc
 *      Freezes L1D. As per the specification,
 *       - The normal state is programmed in L1DCC
 *       - The old state is read from the L1DCC from the POPER field.This latter
 *         read accomplishes 2 things, viz. Ensuring the new state is programmed 
 *         as well as reading the old programmed value
 *
 *   @arg   None
 *
 *   @ret   CACHE_L1_Freeze
 *          Old Freeze State
 *   @eg
 *      CACHE_L1_Freeze oldFreezeState ;
        ... 
        CACHE_freezeL1d ();
        ...
 *
 * ===========================================================================
 */
extern CACHE_L1_Freeze CACHE_freezeL1d(void);

/*
 * =============================================================================
 *   @n@b CACHE_unfreezeL1d
 *
 *   @desc
 *      Unfreezes L1D. As per the specification,
 *       - The normal state is programmed in L1DCC
 *       - The old state is read from the L1DCC from the POPER field.This latter
 *         read accomplishes 2 things, viz. Ensuring the new state is programmed 
 *         as well as reading the old programmed value
 *
 *   @arg   None
 *
 *   @ret   CACHE_L1_Freeze
 *          Old Freeze State
 *   @eg
 *      CACHE_L1_Freeze oldFreezeState ;
        ... 
        CACHE_unfreezeL1d ();
        ...
 *
 * ===========================================================================
 */
extern CACHE_L1_Freeze CACHE_unfreezeL1d (void);

/*
 * =============================================================================
 *   @n@b CACHE_wbL1d
 *
 *   @desc
 *      Writes back range specified in L1D.As per the specification,
 *       - The start of the range that needs to be is written back
 *         into L1DWBAR
 *       - The byte count is programmed in L1DWWC
 *
 *   @arg   blockPtr
 *          Start address of range to be written back
 *
 *   @arg   byteCnt
 *          Number of bytes to be written back
 *
 *   @arg   wait
 *          Whether the call is blocking (and the extent of wait) till the 
 *          issued operation is completed or not.
 *
 *   @ret   None
 *
 *   @eg
 *      ... 
        CACHE_wbL1d ((Uint32*)(0x1000),200,CACHE_NOWAIT);
        ...
 *
 * ===========================================================================
 */
extern void CACHE_wbL1d (
    void         *blockPtr,
    Uint32       byteCnt,                  
    CACHE_Wait   wait                  
);

/*
 * =============================================================================
 *   @n@b CACHE_invL1d
 *
 *   @desc
 *      Invalidates range specified in L1D.As per the specification,
 *       - The start of the range that needs to be invalidated is written
 *         into L1DIBAR
 *       - The byte count is programmed in L1DIWC
 *
 *   @arg   blockPtr
 *          Start address of range to be invalidated
 *
 *   @arg   byteCnt
 *          Number of bytes to be invalidated
 *
 *   @arg   wait
 *          Whether the call is blocking (and the extent of wait) till the 
 *          issued operation is completed or not.
 *
 *   @ret   None
 *
 *   @eg
 *      ... 
        CACHE_invL1d ((Uint32*)(0x1000),200,CACHE_NOWAIT);
        ...
 *
 * ===========================================================================
 */
extern void CACHE_invL1d (
    void         *blockPtr,
    Uint32       byteCnt,                  
    CACHE_Wait   wait                  
);

/*
 * =============================================================================
 *   @n@b CACHE_wbInvL1d
 *
 *   @desc
 *      Writeback invalidates range specified in L1D.As per the specification,
 *       - The start of the range that needs to be writeback invalidated is
 *         programmed into L1DWIBAR
 *       - The byte count is programmed in L1DWIWC
 *
 *   @arg   blockPtr
 *          Start address of range to be written back invalidated
 *
 *   @arg   byteCnt
 *          Number of bytes to be written back invalidated
 *
 *   @arg   wait
 *          Whether the call is blocking (and the extent of wait) till the 
 *          issued operation is completed or not.
 *
 *   @ret   None
 *
 *   @eg
 *      ... 
        CACHE_wbInvL1d ((Uint32*)(0x1000),200,CACHE_NOWAIT);
        ...
 *
 * ===========================================================================
 */
extern void CACHE_wbInvL1d (
    void         *blockPtr,
    Uint32       byteCnt,                  
    CACHE_Wait   wait                  
);

/*
 * =============================================================================
 *   @n@b CACHE_wbAllL1d
 *
 *   @desc
 *      Invalidates All of L1D.As per the specification,
 *       - The L1DINV is programmed
 *
 *   @arg   wait
 *          Whether the call is blocking (and the extent of wait) till the 
 *          issued operation is completed or not
 *
 *   @ret   None
 *
 *   @eg
 *      ... 
        CACHE_wbAllL1d (CACHE_NOWAIT);
        ...
 *
 * ===========================================================================
 */
extern void CACHE_wbAllL1d (
    CACHE_Wait      wait                  
);

/*
 * =============================================================================
 *   @n@b CACHE_wbInvAllL1d
 *
 *   @desc
 *      Writeback invalidates All of L1D.As per the specification,
 *       - The L1DWBINV is programmed
 *
 *   @arg   wait
 *          Whether the call is blocking (and the extent of wait) till the 
 *          issued operation is completed or not
 *
 *   @ret   None
 *
 *   @eg
 *      ... 
        CACHE_wbInvAllL1d (CACHE_NOWAIT);
        ...
 *
 * ===========================================================================
 */
extern void CACHE_invAllL1d (
    CACHE_Wait      wait                  
);

/*
 * =============================================================================
 *   @n@b CACHE_wbInvAllL1d
 *
 *   @desc
 *      Writeback invalidates All of L1D.As per the specification,
 *       - The L1DWBINV is programmed
 *
 *   @arg   wait
 *          Whether the call is blocking (and the extent of wait) till the 
 *          issued operation is completed or not
 *
 *   @ret   None
 *
 *   @eg
 *      ... 
        CACHE_wbInvAllL1d (CACHE_NOWAIT);
        ...
 *
 * ===========================================================================
 */
extern void CACHE_wbInvAllL1d (
    CACHE_Wait      wait                  
);

/*
 * =============================================================================
 *   @n@b CACHE_setL2Size
 *
 *   @desc
 *      Sets the L2 size.As per the specification,
 *       - The old mode is read from the L2CFG
 *       - The new mode is programmed in L2CFG
 *       - L2CFG is read back to ensure it is set
 *
 *   @arg   newSize
 *          New size to be programmed
 *
 *   @ret   CACHE_L2Size
 *          Old Size set for L2
 *
 *   @eg
 *      CACHE_L2Mode oldMode;

        oldSize = CACHE_setL2Size(CACHE_L2_32KCACHE);
        ...
 *
 * ===========================================================================
 */
extern CACHE_L2Size CACHE_setL2Size (
    CACHE_L2Size    newSize                  
);

/*
 * =============================================================================
 *   @n@b CACHE_setL2Mode
 *
 *   @desc
 *      Sets the L2 mode.As per the specification,
 *       - The old mode is read from the L2CFG
 *       - The new mode is programmed in L2CFG
 *       - L2CFG is read back to ensure it is set
 *
 *   @arg   newMode
 *          New mode to be programmed
 *
 *   @ret   CACHE_L2Mode
 *          Old Mode set for L2
 *
 *   @eg
 *      CACHE_L2Mode oldMode;

        oldMode = CACHE_setL2Mode(CACHE_L2_NORMAL);
        ...
 *
 * ===========================================================================
 */
extern CACHE_L2Mode CACHE_setL2Mode (
    CACHE_L2Mode    newMode                  
);

/*
 * =============================================================================
 *   @n@b CACHE_wbL2
 *
 *   @desc
 *      Invalidates range specified in L2. As per the specification,
 *       - The start of the range that needs to be written back is programmed
 *         into L2WBAR
 *       - The byte count is programmed in L2WWC
 *
 *   @arg   blockPtr
 *          Start address of range to be written back
 *
 *   @arg   byteCnt
 *          Number of bytes to be written back
 *
 *   @arg   wait
 *          Whether the call is blocking (and the extent of wait) till the 
 *          issued operation is completed or not
 *
 *   @ret   None
 *
 *   @eg
 *      ...
        CACHE_wbL2((Uint32*)(0x1000), 200, CACHE_NOWAIT);
        ...
 *
 * ===========================================================================
 */
extern void CACHE_wbL2 (
    void            *blockPtr,
    Uint32          byteCnt,
    CACHE_Wait      wait                  
);

/*
 * =============================================================================
 *   @n@b CACHE_invL2
 *
 *   @desc
 *      Invalidates range specified in L2. As per the specification,
 *       - The start of the range that needs to be invalidate is programmed
 *         into L2IBAR
 *       - The byte count is programmed in L2IWC
 *
 *   @arg   blockPtr
 *          Start address of range to be invalidated
 *
 *   @arg   byteCnt
 *          Number of bytes to be invalidated
 *
 *   @arg   wait
 *          Whether the call is blocking (and the extent of wait) till the 
 *          issued operation is completed or not.
 *
 *   @ret   None
 *
 *   @eg
 *      ...
        CACHE_invL2((Uint32*)(0x1000), 200, CACHE_NOWAIT);
        ...
 *
 * ===========================================================================
 */
extern void CACHE_invL2 (
    void            *blockPtr,
    Uint32          byteCnt,
    CACHE_Wait      wait                  
);

/*
 * =============================================================================
 *   @n@b CACHE_wbInvL2
 *
 *   @desc
 *      Writeback invalidated range specified in L2.As per the specification,
 *       - The start of the range that needs to be written back is programmed
 *         into L2WIBAR
 *       - The byte count is programmed in L2WIWC
 *
 *   @arg   blockPtr
 *          Start address of range to be written back invalidated
 *
 *   @arg   byteCnt
 *          Number of bytes to be written back invalidated
 *
 *   @arg   wait
 *          Whether the call is blocking (and the extent of wait) till the 
 *          issued operation is completed or not.
 *
 *   @ret   None
 *
 *   @eg
 *      ...
        CACHE_wbInvL2((Uint32*)(0x1000), 200, CACHE_NOWAIT);
        ...
 *
 * ===========================================================================
 */
extern void CACHE_wbInvL2 (
    void         *blockPtr, 
    Uint32       byteCnt,                  
    CACHE_Wait   wait                  
);

/*
 * =============================================================================
 *   @n@b CACHE_wbAllL2
 *
 *   @desc
 *      Writes back all of L2.As per the specification,The L2WB needs to be 
 *      programmed
 *     
 *   @arg   wait
 *          Whether the call is blocking (and the extent of wait) till the 
 *          issued operation is completed or not.
 *
 *   @ret   None
 *
 *   @eg
 *      ...
        CACHE_wbAllL2(CACHE_NOWAIT);
        ...
 *
 * ===========================================================================
 */
extern void CACHE_wbAllL2 (
    CACHE_Wait  wait                  
);

/*
 * =============================================================================
 *   @n@b CACHE_invAllL2
 *
 *   @desc
 *      Invalidates All of L2.As per the specification,The L2INV needs to be 
 *      programmed
 *     
 *   @arg   wait
 *          Whether the call is blocking (and the extent of wait) till the 
 *          issued operation is completed or not.
 *
 *   @ret   None
 *
 *   @eg
 *      ...
        CACHE_invAllL2(CACHE_NOWAIT);
        ...
 *
 * ===========================================================================
 */
extern void CACHE_invAllL2 (
    CACHE_Wait  wait                  
);

/*
 * =============================================================================
 *   @n@b CACHE_wbInvAllL2
 *
 *   @desc
 *      Writeback invalidates All of L2.As per the specification,The L2WBINV
 *      needs to be programmed
 *     
 *   @arg   wait
 *          Whether the call is blocking (and the extent of wait) till the 
 *          issued operation is completed or not.
 *
 *   @ret   None
 *
 *   @eg
 *      ...
        CACHE_wbInvAllL2(CACHE_NOWAIT);
        ...
 *
 * ===========================================================================
 */
extern void CACHE_wbInvAllL2 (
    CACHE_Wait  wait                  
);

#ifdef __cplusplus
}
#endif

#endif /*_CSL_CACHE_H_*/
