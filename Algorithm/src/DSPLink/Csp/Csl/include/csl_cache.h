/*****************************************************\
 *  Copyright 2004, Texas Instruments Incorporated.  *
 *  All rights reserved.                             *
 *  Restricted rights to use, duplicate or disclose  *
 *  this   code   are  granted   through  contract.  *
 *                                                   *
 * "@(#) PSP/CSL  3.0.0.0  (2003-09-30)              *
\*****************************************************/

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
* @subsection Assumptions
*		None worthy of note	
*/

/** @file csl_cache.h
 *
 *  @brief    Header file for Cache System Service APIs 
 *
 *  Description
 *      The API declarations, enumerations, structure definitions of the necessary functionality
 *      to configure and control   (if avalible) L2, L1D, and L1P Cache.
 *  @date     23th March 2004
 *  @author   Jamon Bowen created
 *  @modification 
 *  @date     21st June 2004
 *  @author   Ruchika Kharwar
 */

/************************************************************************/
#ifndef _CSL_CACHE_H_
#define _CSL_CACHE_H_
#ifdef __cplusplus
extern "C" {
#endif
/**
@defgroup CSLSYS_CACHE_API CACHE
*/

/**
@defgroup CSLSYS_CACHE_SYMBOL  Cache defined Symbols/Macros
@ingroup CSLSYS_CACHE_API
*/
/**
@defgroup CSLSYS_CACHE_ENUM  Cache Enumerated Data Types
@ingroup CSLSYS_CACHE_API
*/

/**
@defgroup CSLSYS_CACHE_FUNCTION  Cache Functions
@ingroup CSLSYS_CACHE_API
*/

/**
@defgroup CSLSYS_CACHE_L1FUNCTION  Cache L1 Functions
@ingroup CSLSYS_CACHE_FUNCTION
*/

/**
@defgroup CSLSYS_CACHE_L2FUNCTION  Cache L2 Functions
@ingroup CSLSYS_CACHE_FUNCTION
*/


#include <csl.h>
#include <cslr_cache.h>
#include <cslsys_soc64plus.h>


/** @addtogroup CSLSYS_CACHE_SYMBOL
* @{ */
#define CACHE_L2_LINESIZE    128 
/**< L2 Line Size */
#define CACHE_L1D_LINESIZE    64
/**< L1D Line Size */
#define CACHE_L1P_LINESIZE    32
/**< L1P Line Size */

#define CACHE_ROUND_TO_LINESIZE(CACHE,ELCNT,ELSIZE)          \
        ((CACHE_##CACHE##_LINESIZE *                         \
        ((ELCNT)*(ELSIZE)/CACHE_##CACHE##_LINESIZE) + 1) /   \
        (ELSIZE))
/**
@}
*/
/******************************************************************************\
* global macro declarations
\******************************************************************************/
/** @addtogroup CSLSYS_CACHE_ENUM
* @{ */
/** @brief Enumeration for Emif ranges
*
* This is used for setting up the cacheability of the EMIF ranges. 
*/
typedef enum {
#ifdef HIMALAYA
	CACHE_EMIFA_CE00  =   128,
	CACHE_EMIFA_CE01  =   129,
	CACHE_EMIFA_CE02  =   130,
	CACHE_EMIFA_CE03  =   131,
	CACHE_EMIFA_CE04  =   132,
	CACHE_EMIFA_CE05  =   133,
	CACHE_EMIFA_CE06  =   134,
	CACHE_EMIFA_CE07  =   135,
	CACHE_EMIFA_CE08  =   136,
	CACHE_EMIFA_CE09  =   137,
	CACHE_EMIFA_CE010 =   138,
	CACHE_EMIFA_CE011 =   139,
	CACHE_EMIFA_CE012 =   140,
	CACHE_EMIFA_CE013 =   141,
	CACHE_EMIFA_CE014 =   142,
	CACHE_EMIFA_CE015 =   143,

	CACHE_EMIFA_CE20  =   160,
	CACHE_EMIFA_CE21  =   161,
	CACHE_EMIFA_CE22  =   162,
	CACHE_EMIFA_CE23  =   163,
	CACHE_EMIFA_CE24  =   164,
	CACHE_EMIFA_CE25  =   165,
	CACHE_EMIFA_CE26  =   166,
	CACHE_EMIFA_CE27  =   167,
	CACHE_EMIFA_CE28  =   168,
	CACHE_EMIFA_CE29  =   169,
	CACHE_EMIFA_CE210 =   170,
	CACHE_EMIFA_CE211 =   171,
	CACHE_EMIFA_CE212 =   172,
	CACHE_EMIFA_CE213 =   173,
	CACHE_EMIFA_CE214 =   174,
	CACHE_EMIFA_CE215 =   175,
	
	CACHE_EMIFA_CE30  =   176,
	CACHE_EMIFA_CE31  =   177,
	CACHE_EMIFA_CE32  =   178,
	CACHE_EMIFA_CE33  =   179,
	CACHE_EMIFA_CE34  =   180,
	CACHE_EMIFA_CE35  =   181,
	CACHE_EMIFA_CE36  =   182,
	CACHE_EMIFA_CE37  =   183,
	CACHE_EMIFA_CE38  =   184,
	CACHE_EMIFA_CE39  =   185,
	CACHE_EMIFA_CE310 =   186,
	CACHE_EMIFA_CE311 =   187,
	CACHE_EMIFA_CE312 =   188,
	CACHE_EMIFA_CE313 =   189,
	CACHE_EMIFA_CE314 =   190,
	CACHE_EMIFA_CE315 =   191,
	
	CACHE_EMIFA_CE40  =   191,
	CACHE_EMIFA_CE41  =   192,
	CACHE_EMIFA_CE42  =   193,
	CACHE_EMIFA_CE43  =   194,
	CACHE_EMIFA_CE44  =   195,
	CACHE_EMIFA_CE45  =   196,
	CACHE_EMIFA_CE46  =   197,
	CACHE_EMIFA_CE47  =   198,
	CACHE_EMIFA_CE48  =   199,
	CACHE_EMIFA_CE49  =   200,
	CACHE_EMIFA_CE410 =   201,
	CACHE_EMIFA_CE411 =   202,
	CACHE_EMIFA_CE412 =   203,
	CACHE_EMIFA_CE413 =   204,
	CACHE_EMIFA_CE414 =   205,
	CACHE_EMIFA_CE415 =   206,
	
    
	CACHE_EMIFA_CE50  =   207,
	CACHE_EMIFA_CE51  =   208,
	CACHE_EMIFA_CE52  =   209,
	CACHE_EMIFA_CE53  =   210,
	CACHE_EMIFA_CE54  =   211,
	CACHE_EMIFA_CE55  =   212,
	CACHE_EMIFA_CE56  =   213,
	CACHE_EMIFA_CE57  =   214,
	CACHE_EMIFA_CE58  =   215,
	CACHE_EMIFA_CE59  =   216,
	CACHE_EMIFA_CE510 =   217,
	CACHE_EMIFA_CE511 =   218,
	CACHE_EMIFA_CE512 =   219,
	CACHE_EMIFA_CE513 =   220,
	CACHE_EMIFA_CE514 =   221,
	CACHE_EMIFA_CE515 =   222,
	
	CACHE_EMIFB_CE00  =   223,
	CACHE_EMIFB_CE01  =   224,
	CACHE_EMIFB_CE02  =   225,
	CACHE_EMIFB_CE03  =   226,
	CACHE_EMIFB_CE04  =   227,
	CACHE_EMIFB_CE05  =   228,
	CACHE_EMIFB_CE06  =   229,
	CACHE_EMIFB_CE07  =   230,
	CACHE_EMIFB_CE08  =   231,
	CACHE_EMIFB_CE09  =   232,
	CACHE_EMIFB_CE010 =   233,
	CACHE_EMIFB_CE011 =   234,
	CACHE_EMIFB_CE012 =   235,
	CACHE_EMIFB_CE013 =   236,
	CACHE_EMIFB_CE014 =   237,
	CACHE_EMIFB_CE015 =   238
    
#else
	CACHE_EMIFB_CE00 =  96,
    CACHE_EMIFB_CE01 =  97,
    CACHE_EMIFB_CE02 =  98,
    CACHE_EMIFB_CE03 =  99,
    CACHE_EMIFB_CE10 =  100,
    CACHE_EMIFB_CE11 =  101,
    CACHE_EMIFB_CE12 =  102,
    CACHE_EMIFB_CE13 =  103,
    CACHE_EMIFB_CE20 =  104,
    CACHE_EMIFB_CE21 =  105,
    CACHE_EMIFB_CE22 =  106,
    CACHE_EMIFB_CE23 =  107,
    CACHE_EMIFB_CE30 =  108,
    CACHE_EMIFB_CE31 =  109,
    CACHE_EMIFB_CE32 =  110,
    CACHE_EMIFB_CE33 =  111,

    CACHE_EMIFA_CE00 =   128,
    CACHE_EMIFA_CE01 =   129,
    CACHE_EMIFA_CE02 =   130,
    CACHE_EMIFA_CE03 =   131,
    CACHE_EMIFA_CE04 =   132,
    CACHE_EMIFA_CE05 =   133,
    CACHE_EMIFA_CE06 =   134,
    CACHE_EMIFA_CE07 =   135,
    CACHE_EMIFA_CE08 =   136,
    CACHE_EMIFA_CE09 =   137,
    CACHE_EMIFA_CE010 =  138,
    CACHE_EMIFA_CE011 =  139,
    CACHE_EMIFA_CE012 =  140,
    CACHE_EMIFA_CE013 =  141,
    CACHE_EMIFA_CE014 =  142,
    CACHE_EMIFA_CE015 =  143,

    CACHE_EMIFA_CE10  =  144,
    CACHE_EMIFA_CE11  =  145,
    CACHE_EMIFA_CE12  =  146,
    CACHE_EMIFA_CE13  =  147,
    CACHE_EMIFA_CE14  =  148,
    CACHE_EMIFA_CE15  =  149,
    CACHE_EMIFA_CE16  =  150,
    CACHE_EMIFA_CE17  =  151,
    CACHE_EMIFA_CE18  =  152,
    CACHE_EMIFA_CE19  =  153,
    CACHE_EMIFA_CE110 =  154,
    CACHE_EMIFA_CE111 =  155,
    CACHE_EMIFA_CE112 =  156,
    CACHE_EMIFA_CE113 =  157,
    CACHE_EMIFA_CE114 =  158,
    CACHE_EMIFA_CE115 =  159,

    CACHE_EMIFA_CE20  =  160,
    CACHE_EMIFA_CE21  =  161,
    CACHE_EMIFA_CE22  =  162,
    CACHE_EMIFA_CE23  =  163,
    CACHE_EMIFA_CE24  =  164,
    CACHE_EMIFA_CE25  =  165,
    CACHE_EMIFA_CE26  =  166,
    CACHE_EMIFA_CE27  =  167,
    CACHE_EMIFA_CE28  =  168,
    CACHE_EMIFA_CE29  =  169,
    CACHE_EMIFA_CE210 =  170,
    CACHE_EMIFA_CE211 =  171,
    CACHE_EMIFA_CE212 =  172,
    CACHE_EMIFA_CE213 =  173,
    CACHE_EMIFA_CE214 =  174,
    CACHE_EMIFA_CE215 =  175,


    CACHE_EMIFA_CE30  =  176,
    CACHE_EMIFA_CE31  =  177,
    CACHE_EMIFA_CE32  =  178,
    CACHE_EMIFA_CE33  =  179,
    CACHE_EMIFA_CE34  =  180,
    CACHE_EMIFA_CE35  =  181,
    CACHE_EMIFA_CE36  =  182,
    CACHE_EMIFA_CE37  =  183,
    CACHE_EMIFA_CE38  =  184,
    CACHE_EMIFA_CE39  =  185,
    CACHE_EMIFA_CE310 =  186,
    CACHE_EMIFA_CE311 =  187,
    CACHE_EMIFA_CE312 =  188,
    CACHE_EMIFA_CE313 =  189,
    CACHE_EMIFA_CE314 =  190,
    CACHE_EMIFA_CE315 =  191
#endif
   
} CE_MAR;

	
/** @brief Enumeration for Cache wait flags
*
* This is used for specifying whether the cache operations should block till the 
* desired operation is complete.
*/

typedef enum {
  CACHE_NOWAIT = 0,					   	/**< No blocking, the call exits after programmation of the control registers */
  CACHE_WAITINTERNAL = 1,			    /**< Blocking Call, the call exits after the relevant cache status registers indicate completion*/
  CACHE_WAIT = 2	                    /**< Blocking Call, the call waits not only till the cache status registers indicate completion,
                                             but also till a write read is issued to the EMIF registers (if required) */
} CACHE_Wait;

/** @brief Enumeration for Cache Freeze flags
*
* This is used for reporting back the current state of the L1.
*/

typedef enum {
  CACHE_L1D_NORMAL = 0,					/**< L1D is in Normal State */ 
  CACHE_L1D_FREEZE = 1,					/**< L1D is in Freeze State */
  CACHE_L1P_NORMAL = 3,					/**< L1P is in Normal State */
  CACHE_L1P_FREEZE = 4,					/**< L1P is in Freeze State */
  CACHE_L1_NORMAL = 5,					/**< L1D, L1P is in Normal State */
  CACHE_L1_FREEZE = 6					/**< L1D, L1P is in Freeze State */
} CACHE_L1_Freeze;

/** @brief Enumeration for L1 (P or D) Sizes */

typedef enum { 
  CACHE_L1_0KCACHE   = 0,				/**< No Cache    */					
  CACHE_L1_4KCACHE  = 1,				/**< 4KB Cache   */					
  CACHE_L1_8KCACHE  = 2,				/**< 8KB Cache   */
  CACHE_L1_16KCACHE = 3,				/**< 16KB Cache  */					
  CACHE_L1_32KCACHE = 4				    /**< 32KB Cache  */
} CACHE_L1Size; 

/** @brief Enumeration for L2 Sizes */
typedef enum { 
  CACHE_0KCACHE   = CSL_CACHE_L2CFG_MODE_0K,  /**< No Cache     */					
  CACHE_32KCACHE  = CSL_CACHE_L2CFG_MODE_32K, /**< 32KB Cache   */					
  CACHE_64KCACHE  = CSL_CACHE_L2CFG_MODE_64K, /**< 64KB Cache   */					
  CACHE_128KCACHE = CSL_CACHE_L2CFG_MODE_128K,/**< 128KB Cache  */					
  CACHE_256KCACHE = CSL_CACHE_L2CFG_MODE_256K /**< 256KB Cache  */					
} CACHE_L2Size; 

/** @brief Enumeration for L2 Modes */
typedef enum {
  CACHE_L2_NORMAL = CSL_CACHE_L2CFG_L2CC_ENABLED, /**< Enabled/Normal Mode */					
  CACHE_L2_FREEZE = CSL_CACHE_L2CFG_L2CC_FREEZE,  /**< Freeze Mode */					
  CACHE_L2_BYPASS = CSL_CACHE_L2CFG_L2CC_BYPASS   /**< Bypass Mode */					
} CACHE_L2Mode;

/**
@}
*/
/** @addtogroup CSLSYS_CACHE_FUNCTION
@{
*/
/*
 * ======================================================
 *   @func   CACHE_enableCaching
 * ======================================================
 */
/** @brief 
 *	   Enables caching for the specified EMIF range.
 */
void CACHE_enableCaching(
CE_MAR mar       /**<EMIF range */
);
/*
 * ======================================================
 *   @func   CACHE_wait
 * ======================================================
 */
/** @brief 
 *	Waits till any previously issued block/global cache command is 
 *  completed.
 *
 *  This does a complete wait, i.e waits till WC = 0 and does a read write to the 
 *  Emif registers if required. 
 *  
 */
void CACHE_wait(
	void /**< None */
);

/*
 * ======================================================
 *   @func   CACHE_waitInternal
 * ======================================================
 */
/** @brief 
 *  Waits till any previosuly issued block/global cache command is 
 *  completed to the extent of the status register/count registers to 
 *  reflect a 'done' status.
 */
void CACHE_waitInternal(
	void /**< None */
);
/**
@}
*/

/** @addtogroup CSLSYS_CACHE_L1FUNCTION
@{
*/
/*
 * ======================================================
 *   @func   CACHE_freezeL1
 * ======================================================
 */
/** @brief 
 *	   Freezes the L1P and L1D
 */
CACHE_L1_Freeze CACHE_freezeL1(
	void	/**< None */
);

/*
 * ======================================================
 *   @func   CACHE_unfreezeL1
 * ======================================================
 */
/** @brief 
 *	   Unfreezes the L1P and L1D
 */
CACHE_L1_Freeze CACHE_unfreezeL1(
	void	/**< None */
);


/*------------------------------------------------*\
		
		L1P

\*------------------------------------------------*/
/*
 * ======================================================
 *   @func   CACHE_setL1pSize
 * ======================================================
 */
/** @brief 
 *	   Sets the L1P size
 */
CACHE_L1Size CACHE_setL1pSize(
	CACHE_L1Size newSize	/**< New size to be programmed */
);
/*
 * ======================================================
 *   @func   CACHE_freezeL1p
 * ======================================================
 */
/** @brief 
 *	   Freezes L1P 
 */

CACHE_L1_Freeze CACHE_freezeL1p(
	void                  /**< Previous L1P state */
);
/*
 * ======================================================
 *   @func   CACHE_unfreezeL1p
 * ======================================================
 */
/** @brief 
 *	   Unfreezes L1P 
 */

CACHE_L1_Freeze CACHE_unfreezeL1p(
	void                  /**< Previous L1P state */
);

/*
 * ======================================================
 *   @func   CACHE_invL1p
 * ======================================================
 */
/** @brief 
 *	   Invalidates range specified in L1P 
 */
/* Block Operations */
void CACHE_invL1p(
	void *blockPtr,                  /**< Start address of range to be invalidated*/
	Uint32 byteCnt,                  /**< Number of bytes to be invalidated*/
	CACHE_Wait wait                  /**< Whether the call is blocking(and the extent of wait) till the issue operation is 
										  completed. */
);
/*
 * ======================================================
 *   @func   CACHE_invAllL1p
 * ======================================================
 */
/** @brief 
 *	   Invalidates all of L1P 
 */

/*global Operations*/
void CACHE_invAllL1p(
	CACHE_Wait wait                  /**< Whether the call is blocking(and the extent of wait)  till the issue operation is completed or not*/
);

/*------------------------------------------------*\
		
		L1D

\*------------------------------------------------*/
/*
 * ======================================================
 *   @func   CACHE_setL1dSize
 * ======================================================
 */
/** @brief 
 *	   Sets the L1D size
 */
CACHE_L1Size CACHE_setL1dSize(
	CACHE_L1Size newSize                  /**< New size to be programmed*/
);
/*
 * ======================================================
 *   @func   CACHE_freezeL1d(
 * ======================================================
 */
/** @brief 
 *	   Freezes L1D 
 */

CACHE_L1_Freeze CACHE_freezeL1d(
	void                  /**< None*/
);
/*
 * ======================================================
 *   @func   CACHE_unfreezeL1d(
 * ======================================================
 */
/** @brief 
 *	   Unfreezes L1D 
 */
CACHE_L1_Freeze CACHE_unfreezeL1d(
	void                  /**< None*/
);

/* Block Operations */
/*
 * ======================================================
 *   @func   CACHE_wbL1d(
 * ======================================================
 */
/** @brief 
 *	   Writes back range specified in L1D 
 */
void CACHE_wbL1d(
	void *blockPtr,                  /**< Start address of range to be written back*/
	Uint32 byteCnt,                  /**< Number of bytes to be written back*/
	CACHE_Wait wait                  /**< Whether the call is blocking(and the extent of wait)  till the issue operation is completed or not*/
);
/*
 * ======================================================
 *   @func   CACHE_invL1d(
 * ======================================================
 */
/** @brief 
 *	   Invalidates range specified in L1D 
 */
void CACHE_invL1d(
	void *blockPtr,                  /**< Start address of range to be invalidated*/
	Uint32 byteCnt,                  /**< Number of bytes to be invalidated*/
	CACHE_Wait wait                  /**< Whether the call is blocking(and the extent of wait) till the issue operation is completed or not*/
);
/*
 * ======================================================
 *   @func   CACHE_wbInvL1d(
 * ======================================================
 */
/** @brief 
 *	   Writeback invalidates range specified in L1D 
 */

void CACHE_wbInvL1d(
	void *blockPtr,                  /**< Start address of range to be written back invalidated.*/
	Uint32 byteCnt,                  /**< Number of bytes to be written back invalidated*/
	CACHE_Wait wait                  /**< Whether the call is blocking(and the extent of wait)  till the issue operation is completed or not*/
);

/*global Operations*/
/*
 * ======================================================
 *   @func   CACHE_wbAllL1d(
 * ======================================================
 */
/** @brief 
 *	   Writeback All of L1D 
 */

void CACHE_wbAllL1d(
	CACHE_Wait wait                  /**< Whether the call is blocking (and the extent of wait)  till the issue operation is completed or not*/
);
/*
 * ======================================================
 *   @func   CACHE_invAllL1d(
 * ======================================================
 */
/** @brief 
 *	   Invalidates All of L1D 
 */

void CACHE_invAllL1d(
	CACHE_Wait wait                  /**< Whether the call is blocking(and the extent of wait)  till the issue operation is completed or not*/
);
/*
 * ======================================================
 *   @func   CACHE_wbInvAllL1d(
 * ======================================================
 */
/** @brief 
 *	   Writeback invalidates All of L1D 
 */

void CACHE_wbInvAllL1d(
	CACHE_Wait wait                  /**< Whether the call is blocking (and the extent of wait)  till the issue operation is completed or not*/
);

/**
@}
*/
/*------------------------------------------------*\
		
		L2

\*------------------------------------------------*/
/** @addtogroup CSLSYS_CACHE_L2FUNCTION
@{
*/
/*
 * ======================================================
 *   @func   CACHE_setL2Size
 * ======================================================
 */
/** @brief 
 *	   Sets the L2 size
 */
CACHE_L2Size CACHE_setL2Size(
	CACHE_L2Size newSize                  /**< New size to be programmed*/
);
/*
 * ======================================================
 *   @func   CACHE_setL2Mode
 * ======================================================
 */
/** @brief 
 *	   Sets the L2 mode
 */
CACHE_L2Mode CACHE_setL2Mode(
	CACHE_L2Mode newMode                  /**< New mode to be programmed*/
);

/* Block Operations */
/*
 * ======================================================
 *   @func   CACHE_wbL2
 * ======================================================
 */
/** @brief 
 *	   Writes back range specified in L2 
 */
void CACHE_wbL2(
	void *blockPtr,                  /**< Start address of range to be written back.*/
	Uint32 byteCnt,                  /**< Number of bytes to be written back*/
	CACHE_Wait wait                  /**< Whether the call is blocking(and the extent of wait)  till the issue operation is completed or not*/
);
/*
 * ======================================================
 *   @func   CACHE_invL2
 * ======================================================
 */
/** @brief 
 *	   Invalidates range specified in L2 
 */
void CACHE_invL2(
	void *blockPtr,                  /**< Start address of range to be invalidated*/
	Uint32 byteCnt,                  /**< Number of bytes to be invalidated*/
	CACHE_Wait wait                  /**< Whether the call is blocking(and the extent of wait)  till the issue operation is completed or not*/
);
/*
 * ======================================================
 *   @func   CACHE_wbInvL2
 * ======================================================
 */
/** @brief 
 *	   Writeback invalidates range specified in L2 
 */
void CACHE_wbInvL2(
	void *blockPtr,                  /**< Start address of range to be written back invalidated*/
	Uint32 byteCnt,                  /**< Number of bytes to be written back invalidated*/
	CACHE_Wait wait                  /**< Whether the call is blocking(and the extent of wait) till the issue operation is completed or not*/
);

/*global Operations*/
/*
 * ======================================================
 *   @func   CACHE_wbAllL2
 * ======================================================
 */
/** @brief 
 *	   Writes back all of L2 
 */

void CACHE_wbAllL2(
	CACHE_Wait wait                  /**<  Whether the call is blocking(and the extent of wait)  till the issue operation is completed or not*/
);
/*
 * ======================================================
 *   @func   CACHE_invAllL2
 * ======================================================
 */
/** @brief 
 *	   Invalidates All of L2 
 */
void CACHE_invAllL2(
	CACHE_Wait wait                  /**< Whether the call is blocking(and the extent of wait) till the issue operation is completed or not */
);
/*
 * ======================================================
 *   @func   CACHE_wbInvAllL2
 * ======================================================
 */
/** @brief 
 *	   Writeback invalidates All of L2 
 */

void CACHE_wbInvAllL2(
	CACHE_Wait wait                  /**< Whether the call is blocking(and the extent of wait)  till the issue operation is completed or not*/
);
/**
@}
*/
#ifdef __cplusplus
}
#endif
#endif /*_CSL_CACHE_H_*/
