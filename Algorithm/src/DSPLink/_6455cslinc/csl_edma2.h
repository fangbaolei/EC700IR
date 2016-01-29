/*  ===========================================================================
 *   Copyright (c) Texas Instruments Inc 2002, 2003, 2004, 2005
 *
 *   Use of this software is controlled by the terms and conditions found
 *   in the license agreement under which this software has been supplied.
 *   ==========================================================================
 */

/** @file  csl_edma2.h
 *
 *  @path $(CSLPATH)\inc
 *
 *  @brief  Inline functions for CSL of EDMA
 *
 */

/** @mainpage EDMA CSL 2.x
 *
 * @section Introduction
 *
 * @subsection xxx Purpose and Scope
 * The purpose of this document is to identify a set of common CSL APIs for
 * the EDMA module across various devices. The CSL developer is expected to
 * refer to this document while designing APIs for these modules. Some of the
 * listed APIs may not be applicable to a given EDMA module. While other cases
 * this list of APIs may not be sufficient to cover all the features of a
 * particular EDMA Module. The CSL developer should use his discretion
 * designing new APIs or extending the existing ones to cover these.
 *
 * @subsection aaa Terms and Abbreviations
 *  -# CSL:  Chip Support Library
 *  -# API:  Application Programmer Interface
 *
 * @subsection References
 *  -# EDMA 3.0 CHANNEL CONTROLLER REVISION 3.0.2, SPRU234.pdf and SPRU401J.pdf
 *
 */

/*****************************************************************************\
* FILENAME...... csl_edma2.h
* DATE CREATED.. 06/11/1999
* LAST MODIFIED. 09/28/2005 Modified the EDMA channel 32 definition
*                09/16/2005 Changes according to the review comments
*                08/04/2005 Interrupt related API are modified to remove the
*                           calls to 'CSL_edma3Open'
*                08/03/2005 Modified to work with the EDMA 3.0.2 and renamed
*                           to cls_edma2.h
*                08/13/2004 Modified tccAllocTable type from static to
*                           global fn.
*                08/02/2004 - Adding support for C6418
*                04/16/2004 Modified tccAllocTable[0] entry to 0.
*                02/09/2004 Removed volatile variable type from EDMA_chain
*                           inline fn.
*                06/17/2003 added support for 6712C
*                05/28/2003 added support for 6711C
*                02/15/2002 added EDMA channel events 6713/DM642 - EDMA_map()
*                04/16/2001
\*****************************************************************************/
#ifndef _CSL_EDMA_2X_H_
#define _CSL_EDMA_2X_H_

#include <csl.h>
#include <csl_edma3.h>
#include <cslr_edma3cc.h>
#include <soc.h>


/*****************************************************************************\
* scope and inline control macros
\*****************************************************************************/
#ifdef __cplusplus
#define CSLAPI extern "C" far
#else
#define CSLAPI extern far
#endif

#undef  USEDEFS
#undef  IDECL
#undef  IDEF

#ifdef  _EDMA_MOD_
#define IDECL CSLAPI
#define USEDEFS
#define IDEF
#else
#ifdef  _INLINE
#define IDECL static inline
#define USEDEFS
#define IDEF  static inline
#else
#define IDECL CSLAPI
#endif
#endif

#define _EDMA_CHA_CNT         (64)
#define _QEDMA_PARAM_CNT      (1)       /* NOT the QDMA channel count */
#define _EDMA_BASE_PRAM       0x02A00000u
#define _EDMA_PRAM_START      ((_EDMA_BASE_PRAM) + 0x4000)
#define _EDMA_PRAM_SIZE       0x00002000u
#define _EDMA_PRAM_ERASE       0x00000800u

#define _EDMA_ENTRY_SIZE      0x00000020u
#define _EDMA_QDMA_PARAM      (_EDMA_PRAM_START+ \
                               _EDMA_ENTRY_SIZE*(_EDMA_CHA_CNT))

#define _EDMA_NULL_PARAM      (_EDMA_PRAM_START+ \
                       _EDMA_ENTRY_SIZE*(_EDMA_CHA_CNT + _QEDMA_PARAM_CNT))

#define _EDMA_RSVD_PARAM      (_EDMA_NULL_PARAM+_EDMA_ENTRY_SIZE)
#define _EDMA_LINK_START      (_EDMA_RSVD_PARAM+_EDMA_ENTRY_SIZE)
#define _EDMA_LINK_CNT        ((_EDMA_PRAM_SIZE/_EDMA_ENTRY_SIZE)- \
                       (_EDMA_CHA_CNT + _QEDMA_PARAM_CNT + 2 + 1))

#define _EDMA_SCRATCH_START   (_EDMA_LINK_START+ \
                       _EDMA_LINK_CNT*_EDMA_ENTRY_SIZE)

#define _EDMA_SCRATCH_SIZE    ((_EDMA_PRAM_START+_EDMA_PRAM_SIZE- \
                       _EDMA_SCRATCH_START) - (2 * sizeof(Uint32)))
 /*
  * Decrement by 8 (2 * sizeof(Uint32)) to not to use the words of the
  * PaRAM, that have reserved
  * fields, for the scratch purposes
  */

/*****************************************************************************\
* global macro declarations
\*****************************************************************************/
/** Number of EDMA channels */
#define EDMA_CHA_CNT              (_EDMA_CHA_CNT)

/** Number of PaRAM tables available */
#define EDMA_TABLE_CNT            (_EDMA_LINK_CNT)

/**
 * Argument used to allocate a unspecific resource of a type
 * @b Example: EDMA_open(EDMA_ALLOC_ANY, EDMA_OPEN_RESET);
 */
#define EDMA_ALLOC_ANY            (-1)

/** Reset flag passed to EDMA_open */
#define EDMA_OPEN_RESET           (0x00000001)

/**
 * Enable flag passed to EDMA_open, enables the particular channel to
 *  service events
 */
#define EDMA_OPEN_ENABLE          (0x00000002)

#define _EDMACC_REGS  _EDMA_BASE_PRAM
                           /* The base address of EDMA registers */

/** Macros for EDMA channels */

/** Use this to open any EDMA channel */
#define EDMA_CHA_ANY      -1

/** EDMA channel  0 */
#define EDMA_CHA_DSPINT   0
/** EDMA channel 1 */
#define EDMA_CHA_TINT0L   1
/** EDMA channel 2 */
#define EDMA_CHA_TINT0H   2
/** EDMA channel 3 */
#define EDMA_CHA_3        3
/** EDMA channel 4 */
#define EDMA_CHA_4        4
/** EDMA channel 5 */
#define EDMA_CHA_5        5
/** EDMA channel 6 */
#define EDMA_CHA_6        6
/** EDMA channel 7 */
#define EDMA_CHA_7        7
/** EDMA channel 8 */
#define EDMA_CHA_8        8
/** EDMA channel 9 */
#define EDMA_CHA_9        9
/** EDMA channel 10 */
#define EDMA_CHA_10       10
/** EDMA channel 11*/
#define EDMA_CHA_11       11
/** EDMA channel 12*/
#define EDMA_CHA_XEVT0    12
/** EDMA channel 13*/
#define EDMA_CHA_REVT0    13
/** EDMA channel 14*/
#define EDMA_CHA_XEVT1    14
/** EDMA channel 15*/
#define EDMA_CHA_REVT1    15
/** EDMA channel 16*/
#define EDMA_CHA_TINT1L   16
/** EDMA channel 17*/
#define EDMA_CHA_TINT1H   17
/** EDMA channel 18*/
#define EDMA_CHA_18       18
/** EDMA channel 19*/
#define EDMA_CHA_19       19
/** EDMA channel 20*/
#define EDMA_CHA_20       20
/** EDMA channel 21*/
#define EDMA_CHA_21       21
/** EDMA channel 22*/
#define EDMA_CHA_22       22
/** EDMA channel 23*/
#define EDMA_CHA_23       23
/** EDMA channel 24*/
#define EDMA_CHA_24       24
/** EDMA channel 25*/
#define EDMA_CHA_25       25
/** EDMA channel 26*/
#define EDMA_CHA_26       26
/** EDMA channel 27*/
#define EDMA_CHA_27       27
/** EDMA channel 28*/
#define EDMA_CHA_VCPREVT0 28
/** EDMA channel 29*/
#define EDMA_CHA_VCPXEVT0 29
/** EDMA channel 30*/
#define EDMA_CHA_TCPREVT0 30
/** EDMA channel 31*/
#define EDMA_CHA_TCPXEVT0 31
/** EDMA channel 32*/
#define EDMA_CHA_UREVT    32
/** EDMA channel 33*/
#define EDMA_CHA_33       33
/** EDMA channel 34*/
#define EDMA_CHA_34       34
/** EDMA channel 35*/
#define EDMA_CHA_35       35
/** EDMA channel 36*/
#define EDMA_CHA_36       36
/** EDMA channel 37*/
#define EDMA_CHA_37       37
/** EDMA channel 38*/
#define EDMA_CHA_38       38
/** EDMA channel 39*/
#define EDMA_CHA_39       39
/** EDMA channel 40*/
#define EDMA_CHA_UXEVT    40
/** EDMA channel 41*/
#define EDMA_CHA_41       41
/** EDMA channel 42*/
#define EDMA_CHA_42       42
/** EDMA channel 43*/
#define EDMA_CHA_43       43
/** EDMA channel 44*/
#define EDMA_CHA_ICREVT   44
/** EDMA channel 45*/
#define EDMA_CHA_ICXEVT   45
/** EDMA channel 46*/
#define EDMA_CHA_46       46
/** EDMA channel 47*/
#define EDMA_CHA_47       47
/** EDMA channel 48*/
#define EDMA_CHA_GPINT0   48
/** EDMA channel 49*/
#define EDMA_CHA_GPINT1   49
/** EDMA channel 50*/
#define EDMA_CHA_GPINT2   50
/** EDMA channel 51*/
#define EDMA_CHA_GPINT3   51
/** EDMA channel 52*/
#define EDMA_CHA_GPINT4   52
/** EDMA channel 53*/
#define EDMA_CHA_GPINT5   53
/** EDMA channel 54*/
#define EDMA_CHA_GPINT6   54
/** EDMA channel 55*/
#define EDMA_CHA_GPINT7   55
/** EDMA channel 56*/
#define EDMA_CHA_GPINT8   56
/** EDMA channel 57*/
#define EDMA_CHA_GPINT9   57
/** EDMA channel 58*/
#define EDMA_CHA_GPINT10  58
/** EDMA channel 59*/
#define EDMA_CHA_GPINT11  59
/** EDMA channel 60*/
#define EDMA_CHA_GPINT12  60
/** EDMA channel 61*/
#define EDMA_CHA_GPINT13  61
/** EDMA channel 62*/
#define EDMA_CHA_GPINT14  62
/** EDMA channel 63*/
#define EDMA_CHA_GPINT15  63


#define _EDMA_TYPE_C   (0x80000000)
#define _EDMA_TYPE_T   (0x40000000)
#define _EDMA_TYPE_Q   (0x20000000)
#define _EDMA_TYPE_S   (0x10000000)


#define _EDMA_MK_HANDLE(base,index,flags)    (EDMA_Handle)(\
  ((base)&0x0000FFFF)|(((index)<<16)&0x00FF0000)|((flags)&0xFF000000)\
)

/** Invalid handle */
#define EDMA_HINV     _EDMA_MK_HANDLE(0x00000000,0,0)

#define EDMA_HNULL    _EDMA_MK_HANDLE(_EDMA_NULL_PARAM,0,_EDMA_TYPE_T)
/** NULL function */
#define NULL_FUNC     0

/* Chaining Flag */
/** Macro for EDMA tranfer completion code interrupt */
#define EDMA_TCC_SET       1

/** Macro to clear EDMA transfer completion code interrupt */
#define EDMA_TCC_CLEAR     0

/** Macro for EDMA alternate tranfer completion code interrupt */
#define EDMA_ATCC_SET      1

/** Macro to clear EDMA alternate transfer completion code interrupt */
#define EDMA_ATCC_CLEAR    0

/* Wrapper macros  */
#define _EDMA_CHANNEL_HANDLE_2X_TO_3X(handle_2x) \
 EDMA_Wrapper_Data_Objs[(((Uint32)handle_2x & 0x00FF0000)>>16)].hChannel

#define _EDMA_MODULE_HANDLE    ((CSL_Edma3ccRegsOvly)_EDMACC_REGS)

#define _EDMA_CHANNEL_HANDLE_2X_TO_3X_PARAM_HANDLE(handle_2x) \
  ((CSL_Edma3ParamHandle)(&(_EDMA_MODULE_HANDLE->PARAMSET[(((Uint32)handle_2x \
    & 0x00FF0000)>>16)])))

#define _EDMA_CHANNEL_NUMBER(handle)    ((handle & 0x00FF0000) >> 16)

/*****************************************************************************\
* global typedef declarations
\*****************************************************************************/

/** EDMA handle returned by EDMA_open and EDMA_allocTable */
typedef Uint32 EDMA_Handle;

typedef struct
{
    CSL_Edma3ChannelObj ChObj;       /* EDMA per channel 3x object */
    CSL_Edma3ParamHandle param;      /* Corresponding EDMA PaRAM handle */
    CSL_Edma3ChannelHandle hChannel; /* EDMA channel handled returned by 3x */
} EDMA_Wrapper_Data;             /* This is to maintain the opened channel
                                   data and its handle required for the 3x */

extern EDMA_Wrapper_Data EDMA_Wrapper_Data_Objs[EDMA_CHA_CNT];

/* A global structure to maintain the EDMA object instance */
extern CSL_Edma3Obj edmaObj;

/** EDMA PaRAM configuration structure */
typedef struct
{
/** Options word of the configuration */
    Uint32 opt;
/** Source address word */
    Uint32 src;
/** Transfer count word */
    Uint32 cnt;
/** Destination address word */
    Uint32 dst;
/** Index configuration word */
    Uint32 idx;
/** Reload address and Link offset */
    Uint32 rld;
} EDMA_Config;





/*****************************************************************************\
* global variable declarations
\*****************************************************************************/
#define EDMA_hNull    EDMA_HNULL

extern far Uint8 tccAllocTable[EDMA_CHA_CNT];

/*****************************************************************************\
* global function declarations
\*****************************************************************************/

/* 
 * ============================================================================
 *   @n@b EDMA_reset
 *
 *   @b Description
 *   @n Resets the given DMA channel.
 *
 *   @b Arguments
 *   @verbatim
       hEdma      Handle to the channel to be reset

     @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  Channel must have been opened, previously.
 *
 *   <b> Post Condition </b>
 *   @li 1. The corresponding PaRAM entry is cleared to 0.
 *   @li 2. The channel is disabled and event register bit is cleared.
 *
 *   @b Modifies
 *   @n The system data structures are modified.
 *
 *
 *   @b Example
 *   @verbatim
     EDMA_handle handle;
     Uint32 chan_no = 1;
     handle = EDMA_open(chan_no, EDMA_OPEN_RESET);
      ...
     EDMA_reset(handle);
      ...
     @endverbatim
 * ============================================================================
 */

CSLAPI void EDMA_reset (
    EDMA_Handle    hEdma
);
/* 
 * ============================================================================
 *   @n@b EDMA_resetAll
 *
 *   @b Description
 *   @n Resets all EDMA channels.
 *
 *   @b Arguments </b>
 *   @n  None
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @li 1. The PaRAM words corresponding to all of the DMA channels is are
 *          cleared to 0.
 *   @li 2. All channels are disabled and their interrupts reset.
 *
 *   @b Modifies
 *   @n The system data structures are modified.
 *
 *   @b Example
 *   @verbatim
      ...
     EDMA_resetAll();
      ...
     @endverbatim
 * ============================================================================
 */
CSLAPI void EDMA_resetAll (
    void
);
/*
 * ============================================================================
 *   @n@b EDMA_open
 *
 *   @b Description
 *   @n Opens a DMA channel for use by the application.
 *
 *   @b Arguments </b>
 *   @verbatim
     1. Channel number or EDMA_CHA_ANY (to open any channel)
     2. Flags - EDMA_OPEN_RESET or EDMA_OPEN_ENABLE or 0
        EDMA_OPEN_RESET  - resets the channel
        EDMA_OPEN_ENABLE - enables the transfers


     @endverbatim
 *   <b> Return Value </b>
 *   @li  A valid handle on success
 *   @li  EDMA_HINV on failure
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @li 1. The channel is enabled or reset (PaRAM entry is cleared,
 *          channel disabled and interrupts cleared) depending on the flags
 *          passed. 
 *
 *   @b Modifies
 *   @n The system data structures are modified.
 *
 *   @b Example
 *   @verbatim
     Uint32 chan_no = 4;
     EDMA_handle handle;
      ...
     handle = EDMA_open(chan_no, 0);
      ...
     @endverbatim
 * ============================================================================
 */
CSLAPI EDMA_Handle EDMA_open (
    int       chaNum,
    Uint32    flags
);

/*
 * ============================================================================
 *   @n@b EDMA_close
 *
 *   @b Description
 *   @n Closes a previously opened DMA channel, after its use
 *      by the application.
 *
 *   @b Arguments
 *   @verbatim
       hEdma      Handle to the channel to be closed

     @endverbatim
 *
 *   <b> Return Value </b>
 *   @li None
 *
 *   <b> Pre Condition </b>
 *   @n  The channel to be closed must have been opened previously.
 *
 *   <b> Post Condition </b>
 *   @li 1. The channel is closed and reset.
 *
 *   @b Modifies
 *   @n The system data structures are modified.
 *
 *
 *   @b Example
 *   @verbatim
     EDMA_handle handle;
     Uint32 chan_no = 1;
     handle = EDMA_open(chan_no, EDMA_OPEN_RESET);
      ...
     EDMA_close(handle);
      ...
     @endverbatim
 * ============================================================================
 */
CSLAPI void EDMA_close (
    EDMA_Handle    hEdma
);

/* 
 * ============================================================================
 *   @n@b EDMA_allocTable
 *
 *   @b Description
 *   @n Allocates a PaRAM table entry for use by the application.
 *
 *   @b Arguments
 *   @verbatim
       tableNum         - PaRAM table entry number (0 to EDMA_TABLE_CNT)
        or
       EDMA_ALLOC_ANY   - to allocate any available entry of PaRAM table

     @endverbatim
 *
 *   <b> Return Value </b>
 *   @li  A valid handle on success
 *   @li  EDMA_HINV on failure
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @li A PaRAM table entry is allocated from the free pool.
 *
 *   @b Modifies
 *   @n The system data structures are modified.
 *
 *
 *   @b Example
 *   @verbatim
     EDMA_handle handle;
     Uint32 tabNum = 1;
      ...
     handle = EDMA_allocTable(tabNum);
      ...
     @endverbatim
 * ============================================================================
 */
CSLAPI EDMA_Handle EDMA_allocTable (
    int    tableNum
);

/* 
 * ============================================================================
 *   @n@b EDMA_freeTable
 *
 *   @b Description
 *   @n Frees a previously allocated PaRAM table entry.
 *
 *   @b Arguments
 *   @verbatim
       hEdma      Handle to the PaRAM entry to be freed

     @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  The channel to be closed must have been allocated previously.
 *
 *   <b> Post Condition </b>
 *   @li One more entry in the free PaRAM table.
 *
 *   @b Modifies
 *   @n The system data structures are modified.
 *
 *
 *   @b Example
 *   @verbatim
     EDMA_handle handle;
     Uint32 tabNum = 1;
      ...
     handle = EDMA_allocTable(tabNum);
      ...
     EDMA_freeTable(handle);
      ...
     @endverbatim
 * ============================================================================
 */
CSLAPI void EDMA_freeTable (
    EDMA_Handle    hEdma
);

/* 
 * ============================================================================
 *   @n@b EDMA_allocTableEx
 *
 *   @b Description
 *   @n Allocates a number of PaRAM table entries from the free pool.
 *
 *   @b Arguments
 *   @verbatim
       cnt        number of channels to be allocaed
       array      pointer to the first element of array of EDMA_handles
                  to return handles for the allocated entries

     @endverbatim
 *
 *   <b> Return Value </b>
 *   @li  The number of allocated entries, if success
 *   @li  0, if failure
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @li 1. The number of entries in free PaRAM table are less by 'cnt'
 *
 *   @b Modifies
 *   @n The system data structures are modified.
 *
 *
 *   @b Example
 *   @verbatim
     EDMA_handle hArray[4];
     Uint32 cnt = 4, retCnt;
     retCnt = EDMA_allocTableEx(cnt, &hArray[0]);
      ...

     @endverbatim
 * ============================================================================
 */
CSLAPI Uint32 EDMA_allocTableEx (
    int            cnt,
    EDMA_Handle    * array
);

/*
 * ============================================================================
 *   @n@b EDMA_freeTableEx
 *
 *   @b Description
 *   @n Frees previously allocated PaRAM table entries.
 *
 *   @b Arguments
 *   @verbatim
       cnt        number of channels to be freed
       array      pointer to the first element of array of EDMA_handles
                  that are to be freed

     @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  Freed entries must have been allocated previously
 *
 *   <b> Post Condition </b>
 *   @li The number of entries in free PaRAM table are more by 'cnt' 
 *
 *   @b Modifies
 *   @n The system data structures are modified.
 *
 *
 *   @b Example
 *   @verbatim
     EDMA_handle hArray[4];
     Uint32 cnt = 4, retCnt;
     retCnt = EDMA_allocTableEx(cnt, &hArray[0]);
      ...
     EDMA_freeTableEx(cnt, &hArray[0]);
      ...
     @endverbatim
 * ============================================================================
 */
CSLAPI void EDMA_freeTableEx (
    int            cnt,
    EDMA_Handle    * array
);

/*
 * ============================================================================
 *   @n@b EDMA_clearPram
 *
 *   @b Description
 *   @n The PaRAM words corresponding to all of the DMA channels  are set to
 *      ‘val’.
 *
 *   @b Arguments
 *   @verbatim
       val      Value to be written into the PaRAM words

     @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @li All words of the PaRAM corresponding to the DMA
 *       channels, are set to the given value, 'val'.  Reserved fields of PaRAM
 *       do not reflect the written bit values.  They are read as 0.
 *
 *   @b Modifies
 *      None.
 *
 *
 *   @b Example
 *   @verbatim
     Uint32 val = 0;
      ...
     EDMA_clearPram(val);
      ...
     @endverbatim
 * ============================================================================
 */
CSLAPI void EDMA_clearPram (
    Uint32    val
);

/* 
 * ============================================================================
 *   @n@b EDMA_intAlloc
 *
 *   @b Description
 *   @n Allocates a DMA channel interrupt.  This interrupt is used in channel
 *      configuration to configure the interrupt to be generated after a
 *      transfer.
 *
 *   @b Arguments
 *   @verbatim
       tcc      interrupt number
       or
       -1       to allocate any avaliable interrupt

     @endverbatim
 *
 *   <b> Return Value </b>
 *   @li interrupt number allocated, if success
 *   @li -1, to indicate failure
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @li One interrupt less in the free pool of interrupts.
 *
 *   @b Modifies
 *   @n The system data structures are modified.
 *
 *
 *   @b Example
 *   @verbatim
     Uint32 tcc = 1, retTcc;
      ...
     retTcc = EDMA_intAlloc(tcc);
      ...

     @endverbatim
 * ============================================================================
 */
CSLAPI Int32 EDMA_intAlloc (
    int    tcc
);
/* 
 * ============================================================================
 *   @n@b EDMA_intFree
 *
 *   @b Description
 *   @n Frees a previously allocated interrupt.
 *
 *   @b Arguments
 *   @verbatim
       tcc      interrupt number to be freed

     @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  To be freed interrupt must have been allocated, previously.
 *
 *   <b> Post Condition </b>
 *   @li One interrupt more in the free pool.
 *
 *   @b Modifies
 *   @n The system data structures are modified.
 *
 *
 *   @b Example
 *   @verbatim
     Uint32 tcc = 1, retTcc;
      ...
     retTcc = EDMA_intAlloc(tcc);
      ...
     EDMA_intFree(retTcc);
      ...
     @endverbatim
 * ============================================================================
 */
CSLAPI void EDMA_intFree (
    int    tcc
);
/* 
 * ============================================================================
 *   @n@b EDMA_config
 *
 *   @b Description
 *   @n Configures an EDMA transfer.
 *
 *   @verbatim
     1. Following transfers specified in the document 'spru234.pdf' are
        NOT possible.  When these are configured in the EDMA_Config structure,
        the routine returns without cofiguring the PaRAM.

        The NOT possible transfers are:
        A-44, A-47, A-48, A-49, A-50, A-62, A-65, A-66, A-67,
        A-68, A-80, A-83 A-84, A-85 and A-86.

        All these "NOT POSSIBLE" are possible saying that Acnt = elmSize,
        BCnt = elmCnt, Cnt =arCnt+1, appropriate indexes and these chain to
        themselves. But TCC = channel Number should be free and this
        programmation should not contrast with user programmation of Interrupt
        enables/chain enables/tcc programmation.

     2. For the following transfers specfied in the document 'spru234.pdf',
        the source address must be alligned to 256-bits, otherwise the config
        API returns without configuring.
        A-42, A-43, A-60, A-61 and A-78

     3. For the following transfers specified in the document 'spru234.pdf',
        the destination address must be alligned to 256-bits, otherwise the
        config API returns without configuring.
        A-42, A-45, A-60, A-63, A-66, A78 and A-81
     @endverbatim

 *   @b Arguments
 *   @verbatim
       hEdma      Handle to the channel or PaRAM to be configured
       conf       Address of the cofiguration structure
     @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @verbatim
     1. Channel must have been opened or a PaRAM entry allocated, previously.
     2. A TCC must have been allocated, if TCINT bit is set.
     @endverbatim

 *   <b> Post Condition </b>
 *   @li The corresponding PaRAM entry is cofigured, if the configuration is
 *       valid
 *
 *   @b Modifies
 *   @n The PaRAM is modified if the configuration is valid.
 *
 *
 *   @b Example
 *   @verbatim
     EDMA_handle handle;
     Uint32 chan_no = 1, tcc;
     EDMA_Config conf;
     char dst[512];
     char src[512];
     handle = EDMA_open(chan_no, EDMA_OPEN_RESET);
     tcc = EDMA_intAlloc(4);
      ...
     conf.opt = 0x51340001;
     conf.cnt = 0x00000200;  // Transfer 512 bytes
     conf.idx = 0;
     conf.rld = 0x0000FFFF;
     conf.dst = (Uint32)&dst[0];
     conf.src = (Uint32)&src[0];

     EDMA_config(handle, &conf);
      ...
     @endverbatim
 * ============================================================================
 */
CSLAPI void EDMA_config (
    EDMA_Handle    hEdma,
    EDMA_Config    * config
);

/*
 * ============================================================================
 *   @n@b EDMA_configArgs
 *
 *   @b Description
 *   @n Configures an EDMA transfer.
 *
 *   @verbatim
     1. Following transfers specified in the document 'spru234.pdf' are
        NOT possible.  When these are configured in the EDMA_Config structure,
        the routine returns without cofiguring the PaRAM.

        The NOT possible transfers are:
        A-44, A-47, A-48, A-49, A-50, A-62, A-65, A-66, A-67,
        A-68, A-80, A-83 A-84, A-85 and A-86.

        All these "NOT POSSIBLE" are possible saying that Acnt = elmSize,
        BCnt = elmCnt, Cnt =arCnt+1, appropriate indexes and these chain to
        themselves. But TCC = channel Number should be free and this
        programmation should not contrast with user programmation of Interrupt
        enables/chain enables/tcc programmation.

     2. For the following transfers specfied in the document 'spru234.pdf',
        the source address must be alligned to 256-bits, otherwise the config
        API returns without configuring.
        A-42, A-43, A-60, A-61 and A-78

     3. For the following transfers specified in the document 'spru234.pdf',
        the destination address must be alligned to 256-bits, otherwise the
        config API returns without configuring.
        A-42, A-45, A-60, A-63, A-66, A78 and A-81
     @endverbatim

 *   @b Arguments
 *   @verbatim
       hEdma      Handle to the channel or PaRAM to be configured
       opt        Options word of the configuration
       src        From address used in the transfer
       cnt        Specify the number of arrays and number of elements
                  in each array
       dst        To address used in the transfer
       idx        Specify offsets used to calculate the addresses
       rld        Specify the link address and the reload value

     @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @verbatim
     1. Channel must have been opened or a PaRAM entry allocated, previously.
     2. A TCC must have been allocated, if TCINT bit is set.
     @endverbatim

 *
 *   <b> Post Condition </b>
 *   @li The corresponding PaRAM entry is cofigured, if the configuration is
 *       valid
 *
 *   @b Modifies
 *   @n The PaRAM is modified if the configuration is valid.
 *
 *
 *   @b Example
 *   @verbatim
     EDMA_handle handle;
     Uint32 chan_no = 1, tcc;
     Uint32 opt, cnt, idx, rld, src, dst;
     char dst[512];
     char src[512];
     handle = EDMA_open(chan_no, EDMA_OPEN_RESET);
     tcc = EDMA_intAlloc(4);
      ...
     opt = 0x51340001;
     cnt = 0x00000200;  // Transfer 512 bytes
     idx = 0;
     rld = 0x0000FFFF;
     dst = (Uint32)&dst[0];
     src = (Uint32)&src[0];

     EDMA_configArgs(handle, opt, src, cnt, dst, idx, rld);
      ...
     @endverbatim
 * ============================================================================
 */


CSLAPI void EDMA_configArgs (
    EDMA_Handle    hEdma,
    Uint32         opt,
    Uint32         src,
    Uint32         cnt,
    Uint32         dst,
    Uint32         idx,
    Uint32         rld
);

/* 
 * ============================================================================
 *   @n@b EDMA_getConfig
 *
 *   @b Description
 *   @n Returns the configuration of an EDMA transfer, with the following
 *          limitations.
 *   @verbatim
     Fields - 2DS, SUM, 2DD, DUM, PDTS, PDTD, FS, FRMIDX, ELEIDX and ELERLD 
              are not returned (not modified in the argument structure passed
              to the API).
     Fields - ATCINT, ATCC, LINK valid if the programed values are valid.
     Other fields contain valid configuration.
 
     @endverbatim
 *   @b Arguments
 *   @verbatim
       handle       Handle of the channel
       config       Pointer to the configuration structure of type
                    'EDMA_Config'

     @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @   None
 *
 *   @b Modifies
 *   @n None
 *
 *
 *   @b Example
 *   @verbatim
     EDMA_handle handle;
     Uint32 chan_no = 1;
     EDMA_Config conf;
     handle = EDMA_open(chan_no, EDMA_OPEN_RESET);
      ...
     EDMA_getConfig(handle, &conf);
      ...
     @endverbatim
 * ============================================================================
 */

CSLAPI void EDMA_getConfig (
    EDMA_Handle    hEdma,
    EDMA_Config    * config
);
/* 
 * ============================================================================
 *   @n@b EDMA_qdmaConfig
 *
 *   @b Description
 *   @n Configures a QDMA transfer, returns after initiating the transfer.
 *
 *   @verbatim
     1. Following transfers specified in the document 'spru234.pdf' are
        NOT possible.  When these are configured in the EDMA_Config structure,
        the routine returns without a data transfer.

        The NOT possible transfers are:
        A-44, A-47, A-48, A-49, A-50, A-62, A-65, A-66, A-67,
        A-68, A-80, A-83 A-84, A-85 and A-86.

     2. For the following transfers specfied in the document 'spru234.pdf',
        the source address must be alligned to 256-bits, otherwise the config
        API returns without a data transfer.
        A-42, A-43, A-60, A-61 and A-78

     3. For the following transfers specified in the document 'spru234.pdf',
        the destination address must be alligned to 256-bits, otherwise the
        config API returns without a data transfer.
        A-42, A-45, A-60, A-63, A-66, A78 and A-81

     4. No need to enable the QDMA channel seperately, this API takes care of
        enabling the QDMA channel.

     5. All transfers with QDMA are frame synchronized transfers.

     @endverbatim

 *   @b Arguments
 *   @verbatim
       hEdma      Handle to the channel or PaRAM to be configured

     @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @verbatim
     1. A TCC must have been allocated, if TCINT bit is set.
     @endverbatim
 *
 *   <b> Post Condition </b>
 *   @li The corresponding PaRAM entry is cofigured, if the configuration is
 *       valid
 *
 *   @b Modifies
 *   @n The PaRAM is modified if the configuration is valid.
 *
 *
 *   @b Example
 *   @verbatim

     EDMA_Config conf;
     Uint32 tcc;
     char dst[512];
     char src[512];
     handle = EDMA_open(chan_no, EDMA_OPEN_RESET);
     tcc = EDMA_intAlloc(4);
      ...
     conf.opt = 0x51340001;
     conf.cnt = 0x00000200;  // Transfer 512 bytes
     conf.idx = 0;
     conf.dst = (Uint32)&dst[0];
     conf.src = (Uint32)&src[0];

     EDMA_qdmaConfig(&conf);
      ...
     @endverbatim
 * ============================================================================
 */
CSLAPI void EDMA_qdmaConfig (
    EDMA_Config    * config
);
/* 
 * ============================================================================
 *   @n@b EDMA_qdmaConfigArgs
 *
 *   @b Description
 *   @n Configures a QDMA transfer, returns after initiating the transfer.
 *
 *   @verbatim
     1. Following transfers specified in the document 'spru234.pdf' are
        NOT possible.  When these are configured in the EDMA_Config structure,
        the routine returns without a data transfer.

        The NOT possible transfers are:
        A-44, A-47, A-48, A-49, A-50, A-62, A-65, A-66, A-67,
        A-68, A-80, A-83 A-84, A-85 and A-86.

     2. For the following transfers specfied in the document 'spru234.pdf',
        the source address must be alligned to 256-bits, otherwise the config
        API returns without a data transfer.
        A-42, A-43, A-60, A-61 and A-78

     3. For the following transfers specified in the document 'spru234.pdf',
        the destination address must be alligned to 256-bits, otherwise the
        config API returns without a data transfer.
        A-42, A-45, A-60, A-63, A-66, A78 and A-81

     4. No need to enable the QDMA channel seperately, this API takes care of
        enabling the QDMA channel.

     5. All transfers with QDMA are frame synchronized transfers.

     @endverbatim

 *   @b Arguments
 *   @verbatim
       opt        Options word of the configuration
       src        From address used in the transfer
       cnt        Specify the number of arrays and number of elements
                  in each array
       dst        To address used in the transfer
       idx        Specify offsets used to calculate the addresses

     @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @verbatim
     1. A TCC must have been allocated, if TCINT bit is set.
     @endverbatim
 *
 *   <b> Post Condition </b>
 *   @li The corresponding PaRAM entry is cofigured, if the configuration is
 *       valid
 *
 *   @b Modifies
 *   @n The PaRAM is modified if the configuration is valid.
 *
 *
 *   @b Example
 *   @verbatim

     Uint32 opt, cnt, idx, src, dst, tcc;
     char dst[512];
     char src[512];

      ...
     tcc = EDMA_intAlloc(4);
     opt = 0x51340001;
     cnt = 0x00000200;  // Transfer 512 bytes
     idx = 0;
     rld = 0x0000FFFF;
     dst = (Uint32)&dst[0];
     src = (Uint32)&src[0];

     EDMA_qdmaConfigArgs(opt, src, cnt, dst, idx);
      ...
     @endverbatim
 * ============================================================================
 */

CSLAPI void EDMA_qdmaConfigArgs (
    Uint32    opt,
    Uint32    src,
    Uint32    cnt,
    Uint32    dst,
    Uint32    idx
);
/* 
 * ============================================================================
 *   @n@b EDMA_qdmaGetConfig
 *
 *   @b Description
 *   @n Returns the configuration of an QEDMA transfer, with the following
 *      limitations.
 *   @verbatim
     Fields - ESIZE, 2DS, SUM, 2DD, DUM, PDTS, PDTD, FRMCNT, ELECNT, 
              FRMIDX and ELEIDX are not returned (not modified in the
              argument structure passed to the API).
     Fields - FS returned as 1, reserved fields are DO NOT CARE.
              Other fields contain valid configuration.
     
     @endverbatim
 *   @b Arguments
 *   @verbatim
       config       A pointer to EDMA_Config structure to return the
                    configuration

     @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n   None
 *
 *   @b Modifies
 *   @n None
 *
 *
 *   @b Example
 *   @verbatim
     EDMA_Config conf;
     handle = EDMA_open(chan_no, EDMA_OPEN_RESET);
      ...
     EDMA_qdmaGetConfig(&conf);
      ...
     @endverbatim
 * ============================================================================
 */
CSLAPI void EDMA_qdmaGetConfig (
    EDMA_Config    * config
);
/*****************************************************************************\
* inline function declarations
\*****************************************************************************/

/* 
 * ============================================================================
 *   @n@b EDMA_getScratchAddr
 *
 *   @b Description
 *   @n Returns the address of the scratch area.  Some portion of PaRAM area
 *      is reserved for scratch purposes.
 *
 *   @b Arguments
 *   @n None
 *
 *   <b> Return Value </b>
 *   @li Address of the scratch area
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n None
 *
 *
 *   @b Example
 *   @verbatim

     Uint32 *addr;
      ...
     addr = (Uint32 *)EDMA_getScratchAddr();
      ...

     @endverbatim
 * ============================================================================
 */

IDECL Uint32 EDMA_getScratchAddr (
    void
);

 /* 
 * ============================================================================
 *   @n@b EDMA_getScratchSize
 *
 *   @b Description
 *   @n Returns the size of scratch area in bytes.
 *
 *   @b Arguments
 *   @n None
 *
 *   <b> Return Value </b>
 *   @n  Size in bytes
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n None
 *
 *
 *   @b Example
 *   @verbatim
     Uint32 scratchSize;
      ...
     scratchSize = EDMA_getScratchSize();
      ...

     @endverbatim
 * ============================================================================
 */
IDECL Uint32 EDMA_getScratchSize (
    void
);

/* 
 * ============================================================================
 *   @n@b EDMA_enableChannel
 *
 *   @b Description
 *   @n Enables a channel for use by a peripheral / host.
 *
 *   @b Arguments
 *   @verbatim
       hEdma      Handle to the channel to be enabled

     @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  Channel must have been opened, previously.
 *
 *   <b> Post Condition </b>
 *   @li 1. The corresponding channel is ready for use by the peripheral.
 *
 *   @b Modifies
 *   @n Sets a bit in EER or EERH.
 *
 *
 *   @b Example
 *   @verbatim
     EDMA_handle handle;
     Uint32 chan_no = 1;
     handle = EDMA_open(chan_no, EDMA_OPEN_RESET);
      ...
     EDMA_enableChannel(handle);
      ...
     @endverbatim
 * ============================================================================
 */
IDECL void EDMA_enableChannel (
    EDMA_Handle    hEdma
);
/* 
 * ============================================================================
 *   @n@b EDMA_disableChannel
 *
 *   @b Description
 *   @n Disables a channel after its use by a peripheral / host.
 *
 *   @b Arguments
 *   @verbatim
       hEdma      Handle to the channel to be disabled

     @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  Channel must have been enabled, previously.
 *
 *   <b> Post Condition </b>
 *   @li The corresponding channel is no longer usable by the periheral.
 *
 *   @b Modifies
 *   @n Clears a bit in EER or EERH.
 *
 *
 *   @b Example
 *   @verbatim
     EDMA_handle handle;
     Uint32 chan_no = 1;
     handle = EDMA_open(chan_no, EDMA_OPEN_RESET);
      ...
     EDMA_enableChannel(handle);
      ...
     EDMA_disableChannel(handle);
      ...
     @endverbatim
 * ============================================================================
 */
IDECL void EDMA_disableChannel (
    EDMA_Handle    hEdma
);
/* 
 * ============================================================================
 *   @n@b EDMA_setChannel
 *
 *   @b Description
 *   @n Initiates a transfer on a channel.
 *
 *   @b Arguments
 *   @verbatim
       hEdma      Handle to the channel to be triggered

     @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  Channel must have been opened and configured, previously.
 *
 *   <b> Post Condition </b>
 *   @li 1. Starts the transfer configured for the channel.
 *
 *   @b Modifies
 *   @n Sets a bit in ER or ERH.
 *
 *
 *   @b Example
 *   @verbatim
     EDMA_handle handle;
     Uint32 chan_no = 1;
     EDMA_Config conf;
     char dst[512];
     char src[512];
     handle = EDMA_open(chan_no, EDMA_OPEN_RESET);
      ...
     conf.opt = 0x51340001;
     conf.cnt = 0x00000200;  // Transfer 512 bytes
     conf.idx = 0;
     conf.rld = 0x0000FFFF;
     conf.dst = (Uint32)&dst[0];
     conf.src = (Uint32)&src[0];

     EDMA_config(handle, &conf);
      ...
     EDMA_setChannel(handle);
      ...
     @endverbatim
 * ============================================================================
 */
IDECL void EDMA_setChannel (
    EDMA_Handle    hEdma
);

/* 
 * ============================================================================
 *   @n@b EDMA_getChannel
 *
 *   @b Description
 *   @n Returns the current state of the channel event by reading
 *      the event flag from the EDMA channel event register (ER).
 *
 *   @b Arguments
 *   @verbatim
       hEdma      Handle to the channel to be tested

     @endverbatim
 *
 *   <b> Return Value </b>
 *   @li       0 - event not detected
 *   @li       1 - event detected
 *
 *   <b> Pre Condition </b>
 *   @n  Channel must have been enabled, previously.
 *
 *   <b> Post Condition </b>
 *   @li None
 *
 *   @b Modifies
 *   @n None
 *
 *
 *   @b Example
 *   @verbatim
     EDMA_handle handle;
     Uint32 chan_no = 1, status;
     handle = EDMA_open(chan_no, EDMA_OPEN_RESET);
      ...
     EDMA_enableChannel(handle);
      ...
     status = EDMA_getChannel(handle);
      ...
     @endverbatim
 * ============================================================================
 */
IDECL Uint32 EDMA_getChannel (
    EDMA_Handle    hEdma
);

 /* 
 * ============================================================================
 *   @n@b EDMA_clearChannel
 *
 *   @b Description
 *   @n Clears a peripheral transfer request on a channel.
 *
 *   @b Arguments
 *   @verbatim
       hEdma      Handle to the channel to be cleared

     @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  Channel must have been opened, previously.
 *
 *   <b> Post Condition </b>
 *   @li A bit in the event register is cleared.  This stops EDMA from
 *   transfering data if transfer has not been started.
 *
 *   @b Modifies
 *   @n Clears a bit in ER or ERH.
 *
 *
 *   @b Example
 *   @verbatim
     EDMA_handle handle;
     Uint32 chan_no = 1;
     handle = EDMA_open(chan_no, EDMA_OPEN_RESET);
      ...
     EDMA_clearChannel(handle);
      ...
     @endverbatim
 * ============================================================================
 */
IDECL void EDMA_clearChannel (
    EDMA_Handle    hEdma
);

 /* 
 * ============================================================================
 *   @n@b EDMA_getTableAddress
 *
 *   @b Description
 *   @n Returns address of PaRAM corresponding to the given EDMA handle
 *
 *   @b Arguments
 *   @verbatim
       hEdma      Handle to the channel or table entry

     @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  Address of the corresponding PaRAM entry
 *
 *   <b> Pre Condition </b>
 *   @n  Channel must have been opened or a table entry allocated, previously.
 *
 *   <b> Post Condition </b>
 *   @li None
 *
 *   @b Modifies
 *   @n None
 *
 *
 *   @b Example
 *   @verbatim
     EDMA_handle handle;
     Uint32 chan_no = 1, *addr;
     handle = EDMA_open(chan_no, EDMA_OPEN_RESET);
      ...
     addr = (Uint32 *)EDMA_getTableAddress(handle);
      ...
     @endverbatim
 * ============================================================================
 */
IDECL Uint32 EDMA_getTableAddress (
    EDMA_Handle    hEdma
);

 /* 
 * ============================================================================
 *   @n@b EDMA_intEnable
 *
 *   @b Description
 *   @n Enables a tranfer completion interrupt.
 *
 *   @b Arguments
 *   @verbatim
       tccIntNum      interrupt number to be enabled

     @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None.
 *
 *   <b> Post Condition </b>
 *   @li The events of this number are recognized.
 *
 *   @b Modifies
 *   @n A bit in IER or IERH  is set
 *
 *
 *   @b Example
 *   @verbatim
     EDMA_handle handle;
     Uint32 tccIntNum = 1;
      ...
     EDMA_intEnable(tccIntNum);
      ...
     @endverbatim
 * ============================================================================
 */
IDECL void EDMA_intEnable (
    Uint32    tccIntNum
);
 /* 
 * ============================================================================
 *   @n@b EDMA_intDisable
 *
 *   @b Description
 *   @n Disables a tranfer completion interrupt.
 *
 *   @b Arguments
 *   @verbatim
       tccIntNum      interrupt number to be disabled

     @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None.
 *
 *   <b> Post Condition </b>
 *   @li The events of this number are not recognized.
 *
 *   @b Modifies
 *   @n A bit in IER IERH is cleared
 *
 *
 *   @b Example
 *   @verbatim
     EDMA_handle handle;
     Uint32 tccIntNum = 1;
      ...
     EDMA_intDisable(tccIntNum);
      ...
     @endverbatim
 * ============================================================================
 */

IDECL void EDMA_intDisable (
    Uint32    tccIntNum
);
 /* 
 * ============================================================================
 *   @n@b EDMA_intClear
 *
 *   @b Description
 *   @n Clears a tranfer completion interrupt.
 *
 *   @b Arguments
 *   @verbatim
       tccIntNum      interrupt number to be cleared

     @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None.
 *
 *   <b> Post Condition </b>
 *   @li None.
 *
 *   @b Modifies
 *   @n A bit in IPR or IPRH is cleared
 *
 *
 *   @b Example
 *   @verbatim
     EDMA_handle handle;
     Uint32 tccIntNum = 1;
      ...
     EDMA_intClear(tccIntNum);
      ...
     @endverbatim
 * ============================================================================
 */
IDECL void EDMA_intClear (
    Uint32    tccIntNum
);
/* 
 * ============================================================================
 *   @n@b EDMA_intTest
 *
 *   @b Description
 *   @n Returns the status of a tranfer completion interrupt.
 *
 *   @b Arguments
 *   @verbatim
       tccIntNum      interrupt number to be tested

     @endverbatim
 *
 *   <b> Return Value </b>
 *   @li       0 = flag not set
 *   @li       1 = flag set
 *
 *   <b> Pre Condition </b>
 *   @n  None.
 *
 *   <b> Post Condition </b>
 *   @li None.
 *
 *   @b Modifies
 *   @n None
 *
 *
 *   @b Example
 *   @verbatim
     EDMA_handle handle;
     Uint32 tccIntNum = 1, status;
      ...
     status = EDMA_intTest(tccIntNum);
      ...
     @endverbatim
 * ============================================================================
 */
IDECL Uint32 EDMA_intTest (
    Uint32    tccIntNum
);

/* 
 * ============================================================================
 *   @n@b EDMA_intReset
 *
 *   @b Description
 *   @n Clears a pending tranfer completion interrupt and disables the
 *      interrupt.
 *
 *   @b Arguments
 *   @verbatim
       tccIntNum      interrupt number to be reset

     @endverbatim
 *
 *   <b> Return Value </b>
 *   @n None
 *
 *   <b> Pre Condition </b>
 *   @n  None.
 *
 *   <b> Post Condition </b>
 *   @li Interrupts are not recognized.
 *
 *   @b Modifies
 *   @n A bit in IPR or IPRH  and IER or IERH cleared
 *
 *
 *   @b Example
 *   @verbatim
     EDMA_handle handle;
     Uint32 tccIntNum = 1, status;
      ...
     EDMA_intReset(tccIntNum);
      ...
     @endverbatim
 * ============================================================================
 */

IDECL void EDMA_intReset (
    Uint32    tccIntNum
);
/* 
 * ============================================================================
 *   @n@b EDMA_intResetAll
 *
 *   @b Description
 *   @n Clears all pending tranfer completion interrupts and disables the all
 *      interrupts.
 *
 *   @b Arguments
 *   @n None
 *
 *   <b> Return Value </b>
 *   @n None
 *
 *   <b> Pre Condition </b>
 *   @n  None.
 *
 *   <b> Post Condition </b>
 *   @li Interrupts are not recognized.
 *
 *   @b Modifies
 *   @n All bits in IPR, IPRH, IER and IERH cleared
 *
 *
 *   @b Example
 *   @verbatim
     EDMA_handle handle;
     Uint32 tccIntNum = 1, status;
      ...
     EDMA_intResetAll();
      ...
     @endverbatim
 * ============================================================================
 */
IDECL void EDMA_intResetAll (
    void
);

/* 
 * ============================================================================
 *   @n@b EDMA_link
 *
 *   @b Description
 *   @n Links a child PaRAM entry to the parent.
 *
 *   @b Arguments
 *   @verbatim
       parent      handle to the parent PaRAM (channel)
       child         handle to the child PaRAM

     @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  Parent and child must have been configured.
 *
 *   <b> Post Condition </b>
 *   @li 1. The parent's RLD word of PaRAM is set to the offset of chaild
 *          PaRAM.
 *
 *   @b Modifies
 *   @n Parent PaRAM is modified.
 *
 *
 *   @b Example
 *   @verbatim
     EDMA_handle par_handle, ch_handle;
     EDMA_config par_conf, ch_conf;
     Uint32 chan_no = 1, tab = 4;
     par_handle = EDMA_open(chan_no, EDMA_OPEN_RESET);
     ch_handle = EDMA_allocTable(tab);

     // Configure parent, using EDMA_config
      ...
     // Configure child, using EDMA_config
      ...
      ...
     EDMA_link(par_handle, ch_handle);
      ...
     @endverbatim
 * ============================================================================
 */
IDECL void EDMA_link (
    EDMA_Handle    parent,
    EDMA_Handle    child
);

/* 
 * ============================================================================
 *   @n@b EDMA_chain
 *
 *   @b Description
 *   @n Enables chainning of parent and child, after transfer gets
 *      finished/submitted based on the flags supplied.
 *          
 *
 *   @b Arguments
 *   @verbatim
      parent        EDMA handle of the channel, after which next channel gets
                    chained.

      nextChannel   EDMA handle associated with the channel to be chained.

      flag_tcc      Flag to enable/disable chaining the child after completion
                    of the parent transfer.
                    Following are the values:
                    0   -   Disable
                    1   -   Enable

      flag_atcc     Flag to enable/disable chaining the child after submission
                    of the parent transfer.
                    Following are the values:
                    0   -   Disable
                    1   -   Enable

     @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  Channel must have been opened, previously.
 *
 *   <b> Post Condition </b>
 *   @li 1. Chaild channel gets chained to parent.
 *   @li 2. Enables chaining of parent (child transfer gets triggered,
 *          after parent transfer gets over), if TCC flag is set.
 *   @li 3. Enables alternate chaining of parent (child transfer gets
 *          triggered, after parent transfer gets submitted), if ATCC flag is
 *          set.
 *
 *   @b Modifies
 *   @n The OPT word of the parent PaRAM gets modified.
 *
 *   @b Example
 *   @verbatim
     EDMA_handle par_handle, next_handle;
     EDMA_config par_conf, next_conf;
     Uint32 par_chan_no = 1, next_chan_no = 4;
     par_handle = EDMA_open(par_chan_no, EDMA_OPEN_RESET);
     par_handle = EDMA_open(next_chan_no, EDMA_OPEN_RESET);

     // Configure parent using EDMA_config
      ...
     // Configure next using EDMA_config
      ...
     EDMA_chain(par_handle, next_handle, 0, atccflag);
      ...


     @endverbatim
 * ============================================================================
 */
IDECL void EDMA_chain (
    EDMA_Handle    parent,
    EDMA_Handle    nextChannel,
    Uint32         tccflag,
    Uint32         atccflag
);

/* 
 * ============================================================================
 *   @n@b EDMA_enableChaining
 *
 *   @b Description
 *   @n Enables chaining on the given channel.
 *
 *   @b Arguments
 *   @verbatim
       hEdma      Handle to the channel to be chained

     @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  Channel must have been opened, previously.
 *
 *   <b> Post Condition </b>
 *   @li 1. The channel initiates a transfer on the channel specified in
 *          its TCC and TCCM fields, after transfer of the current
 *          channel is over.
 *
 *   @b Modifies
 *   @n Associated PaRAM.
 *
 *
 *   @b Example
 *   @verbatim
     EDMA_handle handle;
     Uint32 chan_no1 = 1;
     handle = EDMA_open(chan_no, EDMA_OPEN_RESET);

     ...
     EDMA_enableChaining(handle);
      ...
     @endverbatim
 * ============================================================================
 */
IDECL void EDMA_enableChaining (
    EDMA_Handle    hEdma
);
/* 
 * ============================================================================
 *   @n@b EDMA_disableChaining
 *
 *   @b Description
 *   @n Disables chaining on the given channel.
 *
 *   @b Arguments
 *   @verbatim
       hEdma      Handle to the channel whose chaining is to be disabled

     @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  Channel must have been opened, previously.
 *
 *   <b> Post Condition </b>
 *   @li 1. The channel does NOT initiate a transfer on the channel
 *          specified in its TCC and TCCM fields, after transfer of
 *          the current channel is over.
 *
 *   @b Modifies
 *   @n Associated PaRAM.
 *
 *
 *   @b Example
 *   @verbatim
     EDMA_handle par_handle, next_handle;
     EDMA_config par_conf, next_conf;
     Uint32 par_chan_no = 1, next_chan_no = 4;
     par_handle = EDMA_open(par_chan_no, EDMA_OPEN_RESET);
     par_handle = EDMA_open(next_chan_no, EDMA_OPEN_RESET);

     // Configure parent using EDMA_config, with tcc field set as next
      ...
     // Configure next using EDMA_config
      ...
     EDMA_enableChaining(par_handle);
      ...
     // Program/peripheral initates a transfer on parent
     // Wait for the completion of transfer on next_chan_no
      ...
     EDMA_disableChaining(par_handle);
      ...
     @endverbatim
 * ============================================================================
 */
IDECL void EDMA_disableChaining (
    EDMA_Handle    hEdma
);

/*****************************************************************************\
* inline function definitions
\*****************************************************************************/
#ifdef USEDEFS
/*---------------------------------------------------------------------------*/
/** 
 * ============================================================================
 *   @n@b EDMA_getScratchAddr
 *
 *   @b Description
 *   @n Returns the address of the scratch area.  Some portion of PaRAM area
 *      is reserved for scratch purposes.
 *
 *   @b Arguments
 *   @n None
 *
 *   <b> Return Value </b>
 *   @li Address of the scratch area
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n None
 *
 *
 *   @b Example
 *   @verbatim

     Uint32 *addr;
      ...
     addr = (Uint 32 *)EDMA_getScratchAddr();
      ...

     @endverbatim
 * ============================================================================
 */
IDEF Uint32 EDMA_getScratchAddr (
    void
)
{
    return (Uint32) ((_EDMA_SCRATCH_START) + sizeof (Uint32));
}

/*---------------------------------------------------------------------------*/
 /** 
 * ============================================================================
 *   @n@b EDMA_getScratchSize
 *
 *   @b Description
 *   @n Returns the size of scratch area in bytes.
 *
 *   @b Arguments
 *   @n None
 *
 *   <b> Return Value </b>
 *   @n  Size in bytes
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n None
 *
 *
 *   @b Example
 *   @verbatim
     Uint32 scratchSize;
      ...
     scratchSize = EDMA_getScratchSize();
      ...

     @endverbatim
 * ============================================================================
 */
IDEF Uint32 EDMA_getScratchSize (
    void
)
{
    return (Uint32) _EDMA_SCRATCH_SIZE;
}

/*---------------------------------------------------------------------------*/
/** 
 * ============================================================================
 *   @n@b EDMA_enableChannel
 *
 *   @b Description
 *   @n Enables a channel for use by a peripheral / host.
 *
 *   @b Arguments
 *   @verbatim
       hEdma      Handle to the channel to be enabled

     @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  Channel must have been opened, previously.
 *
 *   <b> Post Condition </b>
 *   @li 1. The corresponding channel is ready for use by the peripheral.
 *
 *   @b Modifies
 *   @n Sets a bit in EER or EERH.
 *
 *
 *   @b Example
 *   @verbatim
     EDMA_handle handle;
     Uint32 chan_no = 1;
     handle = EDMA_open(chan_no, EDMA_OPEN_RESET);
      ...
     EDMA_enableChannel(handle);
      ...
     @endverbatim
 * ============================================================================
 */
IDEF void EDMA_enableChannel (
    EDMA_Handle    hEdma
)
{

    CSL_edma3HwChannelControl (_EDMA_CHANNEL_HANDLE_2X_TO_3X (hEdma),
        CSL_EDMA3_CMD_CHANNEL_ENABLE, NULL);
}

/*---------------------------------------------------------------------------*/
/** 
 * ============================================================================
 *   @n@b EDMA_chain
 *
 *   @b Description
 *   @n Enables chainning of parent and child, after transfer gets 
 *      finished/submitted based on the flags supplied.
 *          
 *
 *   @b Arguments
 *   @verbatim
      parent        EDMA handle of the channel, after which next channel gets
                    chained.

      nextChannel   EDMA handle associated with the channel to be chained.

      flag_tcc      Flag to enable/disable chaining the child after completion
                    of the parent transfer.
                    Following are the values:
                    0   -   Disable
                    1   -   Enable

      flag_atcc     Flag to enable/disable chaining the child after submission
                    of the parent transfer.
                    Following are the values:
                    0   -   Disable
                    1   -   Enable

     @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  Channel must have been opened, previously.
 *
 *   <b> Post Condition </b>
 *   @li 1. Chaild channel gets chained to parent.
 *   @li 2. Enables chaining of parent (child transfer  gets triggered,
 *          after parent transfer gets over), if TCC flag is set.
 *   @li 3. Enables alternate chaining of parent (child transfer gets
 *          triggered, after parent transfer gets submitted), if ATCC flag
 *          is set.
 *
 *   @b Modifies
 *   @n The OPT word of the parent PaRAM gets modified.
 *
 *   @b Example
 *   @verbatim
     EDMA_handle par_handle, next_handle;
     EDMA_config par_conf, next_conf;
     Uint32 par_chan_no = 1, next_chan_no = 4, atccflag = 1;
     par_handle = EDMA_open(par_chan_no, EDMA_OPEN_RESET);
     par_handle = EDMA_open(next_chan_no, EDMA_OPEN_RESET);

     // Configure parent using EDMA_config
      ...
     // Configure next using EDMA_config
      ...
     EDMA_chain(par_handle, next_handle, 0, atccflag);
      ...


     @endverbatim
 * ============================================================================
 */
IDEF void EDMA_chain (
    EDMA_Handle    parent,
    EDMA_Handle    nextChannel,
    Uint32         tccflag,
    Uint32         atccflag
)
{
    Uint32 gie;
    Uint32 TccNum = _EDMA_CHANNEL_NUMBER (nextChannel), parent_num =
        _EDMA_CHANNEL_NUMBER (parent);
    gie = _disable_interrupts ();
    CSL_FINS (EDMA_Wrapper_Data_Objs[parent_num].param->OPT, EDMA3CC_OPT_TCC,
        TccNum);

    if (tccflag == 1) {
        CSL_FINS (EDMA_Wrapper_Data_Objs[parent_num].param->OPT,
            EDMA3CC_OPT_TCCHEN, 1);
    }
    else {
        CSL_FINS (EDMA_Wrapper_Data_Objs[parent_num].param->OPT,
            EDMA3CC_OPT_TCCHEN, 0);
    }
    if (atccflag == 1) {
        CSL_FINS (EDMA_Wrapper_Data_Objs[parent_num].param->OPT,
            EDMA3CC_OPT_ITCCHEN, 1);
    }
    else {
        CSL_FINS (EDMA_Wrapper_Data_Objs[parent_num].param->OPT,
            EDMA3CC_OPT_ITCCHEN, 0);
    }
    _restore_interrupts (gie);
}

/*---------------------------------------------------------------------------*/
/** 
 * ============================================================================
 *   @n@b EDMA_enableChaining
 *
 *   @b Description
 *   @n Enables chaining on the given channel.
 *
 *   @b Arguments
 *   @verbatim
       hEdma      Handle to the channel to be chained

     @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  Channel must have been opened, previously.
 *
 *   <b> Post Condition </b>
 *   @li 1. The channel initiates a transfer on the channel specified in
 *          its TCC and TCCM fields, after transfer of the current
 *          channel is over.
 *
 *   @b Modifies
 *   @n Associated PaRAM.
 *
 *
 *   @b Example
 *   @verbatim
     EDMA_handle handle;
     Uint32 chan_no1 = 1;
     handle = EDMA_open(chan_no, EDMA_OPEN_RESET);

     ...
     EDMA_enableChaining(handle);
      ...
     @endverbatim
 * ============================================================================
 */
IDEF void EDMA_enableChaining (
    EDMA_Handle    hEdma
)
{

    Uint32 gie;
    Uint32 chaNum = _EDMA_CHANNEL_NUMBER (hEdma);
    gie = _disable_interrupts ();
    CSL_FINS (EDMA_Wrapper_Data_Objs[chaNum].param->OPT,
        EDMA3CC_OPT_TCCHEN, 1);
    _restore_interrupts (gie);
}

/** 
 * ============================================================================
 *   @n@b EDMA_disableChaining
 *
 *   @b Description
 *   @n Disables chaining on the given channel.
 *
 *   @b Arguments
 *   @verbatim
       hEdma      Handle to the channel whose chaining is to be disabled

     @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  Channel must have been opened, previously.
 *
 *   <b> Post Condition </b>
 *   @li 1. The channel does NOT initiate a transfer on the channel
 *          specified in its TCC and TCCM fields, after transfer of
 *          the current channel is over.
 *
 *   @b Modifies
 *   @n Associated PaRAM.
 *
 *
 *   @b Example
 *   @verbatim
     EDMA_handle par_handle, next_handle;
     EDMA_config par_conf, next_conf;
     Uint32 par_chan_no = 1, next_chan_no = 4;
     par_handle = EDMA_open(par_chan_no, EDMA_OPEN_RESET);
     par_handle = EDMA_open(next_chan_no, EDMA_OPEN_RESET);

     // Configure parent using EDMA_config, with tcc field set as next
      ...
     // Configure next using EDMA_config
      ...
     EDMA_enableChaining(par_handle);
      ...
     // Program/peripheral initates a transfer on parent
     // Wait for the completion of transfer on next_chan_no
      ...
     EDMA_disableChaining(par_handle);
      ...
     @endverbatim
 * ============================================================================
 */
IDEF void EDMA_disableChaining (
    EDMA_Handle    hEdma
)
{

    Uint32 gie;
    Uint32 chaNum = _EDMA_CHANNEL_NUMBER (hEdma);
    gie = _disable_interrupts ();
    CSL_FINS (EDMA_Wrapper_Data_Objs[chaNum].param->OPT,
        EDMA3CC_OPT_TCCHEN, 0);
    _restore_interrupts (gie);
}

/** 
 * ============================================================================
 *   @n@b EDMA_disableChannel
 *
 *   @b Description
 *   @n Disables a channel after its use by a peripheral / host.
 *
 *   @b Arguments
 *   @verbatim
       hEdma      Handle to the channel to be disabled

     @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  Channel must have been enabled, previously.
 *
 *   <b> Post Condition </b>
 *   @li The corresponding channel is no longer usable by the periheral.
 *
 *   @b Modifies
 *   @n Clears a bit in EER or EERH.
 *
 *
 *   @b Example
 *   @verbatim
     EDMA_handle handle;
     Uint32 chan_no = 1;
     handle = EDMA_open(chan_no, EDMA_OPEN_RESET);
      ...
     EDMA_enableChannel(handle);
      ...
     EDMA_disableChannel(handle);
     ...
     @endverbatim
 * ============================================================================
 */

IDEF void EDMA_disableChannel (
    EDMA_Handle    hEdma
)
{
    CSL_edma3HwChannelControl (_EDMA_CHANNEL_HANDLE_2X_TO_3X (hEdma),
        CSL_EDMA3_CMD_CHANNEL_DISABLE, NULL);
}

/*---------------------------------------------------------------------------*/
/** 
 * ============================================================================
 *   @n@b EDMA_setChannel
 *
 *   @b Description
 *   @n Initiates a transfer on a channel.
 *
 *   @b Arguments
 *   @verbatim
       hEdma      Handle to the channel to be triggered

     @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  Channel must have been opened and configured, previously.
 *
 *   <b> Post Condition </b>
 *   @li 1. Starts the transfer configured for the channel.
 *
 *   @b Modifies
 *   @n Sets a bit in ER or ERH.
 *
 *
 *   @b Example
 *   @verbatim
     EDMA_handle handle;
     Uint32 chan_no = 1;
     EDMA_Config conf;
     char dst[512];
     char src[512];
     handle = EDMA_open(chan_no, EDMA_OPEN_RESET);
      ...
     conf.opt = 0x51340001;
     conf.cnt = 0x00000200;  // Transfer 512 bytes
     conf.idx = 0;
     conf.rld = 0x0000FFFF;
     conf.dst = (Uint32)&dst[0];
     conf.src = (Uint32)&src[0];

     EDMA_config(handle, &conf);
      ...
     EDMA_setChannel(handle);
      ...
     @endverbatim
 * ============================================================================
 */
IDEF void EDMA_setChannel (
    EDMA_Handle    hEdma
)
{

    CSL_edma3HwChannelControl (_EDMA_CHANNEL_HANDLE_2X_TO_3X (hEdma),
        CSL_EDMA3_CMD_CHANNEL_SET, NULL);
}

/*---------------------------------------------------------------------------*/
/** 
 * ============================================================================
 *   @n@b EDMA_getChannel
 *
 *   @b Description
 *   @n Returns the current state of the channel event by reading
 *      the event flag from the EDMA channel event register (ER).
 *
 *   @b Arguments
 *   @verbatim
       hEdma      Handle to the channel to be tested

     @endverbatim
 *
 *   <b> Return Value </b>
 *   @li       0 - event not detected
 *   @li       1 - event detected
 *
 *   <b> Pre Condition </b>
 *   @n  Channel must have been enabled, previously.
 *
 *   <b> Post Condition </b>
 *   @li None
 *
 *   @b Modifies
 *   @n None
 *
 *
 *   @b Example
 *   @verbatim
     EDMA_handle handle;
     Uint32 chan_no = 1, status;
     handle = EDMA_open(chan_no, EDMA_OPEN_RESET);
      ...
     EDMA_enableChannel(handle);
      ...
     status = EDMA_getChannel(handle);
      ...
     @endverbatim
 * ============================================================================
 */
IDEF Uint32 EDMA_getChannel (
    EDMA_Handle    hEdma
)
{

    Uint32 chanNum = _EDMA_CHANNEL_NUMBER (hEdma);
    if (chanNum < 32) { /* There are 64 channels; One register can hold
                           bits corresponding to 32 channels */
        return ((_EDMA_MODULE_HANDLE->ER) >> (chanNum)) & (1);
    }
    else if (chanNum < 64) {
        return ((_EDMA_MODULE_HANDLE->ERH) >> (chanNum - 32)) & (1);
    }
    else {
        return 0;
    }
}

/*---------------------------------------------------------------------------*/
 /** 
 * ============================================================================
 *   @n@b EDMA_clearChannel
 *
 *   @b Description
 *   @n Clears a peripheral transfer request on a channel.
 *
 *   @b Arguments
 *   @verbatim
       hEdma      Handle to the channel to be cleared

     @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  Channel must have been opened, previously.
 *
 *   <b> Post Condition </b>
 *   @li A bit in the event register is cleared.  This stops EDMA from
 *   transfering data if transfer has not been started.
 *
 *   @b Modifies
 *   @n Clears a bit in ER or ERH.
 *
 *
 *   @b Example
 *   @verbatim
     EDMA_handle handle;
     Uint32 chan_no = 1;
     handle = EDMA_open(chan_no, EDMA_OPEN_RESET);
      ...
     EDMA_clearChannel(handle);
      ...
     @endverbatim
 * ============================================================================
 */
IDEF void EDMA_clearChannel (
    EDMA_Handle    hEdma
)
{

    /* Clear event register */
    CSL_edma3HwChannelControl (_EDMA_CHANNEL_HANDLE_2X_TO_3X (hEdma),
        CSL_EDMA3_CMD_CHANNEL_CLEAR, NULL);
}

/*---------------------------------------------------------------------------*/
 /** 
 * ============================================================================
 *   @n@b EDMA_getTableAddress
 *
 *   @b Description
 *   @n Returns address of PaRAM corresponding to the given EDMA handle
 *
 *   @b Arguments
 *   @verbatim
       hEdma      Handle to the channel or table entry

     @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  Address of the corresponding PaRAM entry
 *
 *   <b> Pre Condition </b>
 *   @n  Channel must have been opened or a table entry allocated,
 *       previously.
 *
 *   <b> Post Condition </b>
 *   @li None
 *
 *   @b Modifies
 *   @n None
 *
 *
 *   @b Example
 *   @verbatim
     EDMA_handle handle;
     Uint32 chan_no = 1, *addr;
     handle = EDMA_open(chan_no, EDMA_OPEN_RESET);
      ...
     addr = (Uint32 *)EDMA_getTableAddress(handle);
      ...
     @endverbatim
 * ============================================================================
 */
IDEF Uint32 EDMA_getTableAddress (
    EDMA_Handle    hEdma
)
{
    return (Uint32) (
        &_EDMA_MODULE_HANDLE->PARAMSET[_EDMA_CHANNEL_NUMBER (hEdma)]);
}

/*---------------------------------------------------------------------------*/
 /** 
 * ============================================================================
 *   @n@b EDMA_intEnable
 *
 *   @b Description
 *   @n Enables a tranfer completion interrupt.
 *
 *   @b Arguments
 *   @verbatim
       tccIntNum      interrupt number to be enabled

     @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None.
 *
 *   <b> Post Condition </b>
 *   @li The events of this number are recognized.
 *
 *   @b Modifies
 *   @n A bit in IER or IERH  is set
 *
 *
 *   @b Example
 *   @verbatim
     EDMA_handle handle;
     Uint32 tccIntNum = 1;
      ...
     EDMA_intEnable(tccIntNum);
      ...
     @endverbatim
 * ============================================================================
 */
IDEF void EDMA_intEnable (
    Uint32    tccIntNum
)
{

    CSL_Edma3CmdIntr arg = { 0 };

    arg.region = CSL_EDMA3_REGION_GLOBAL;

    if (tccIntNum < 32) {  /* There are 64 channels; One register can hold
                              bits corresponding to 32 channels */
        arg.intr = 1 << tccIntNum;
    }
    else {
        arg.intrh = (1 << (tccIntNum - 32));
    }

    CSL_edma3HwControl ((CSL_Edma3Handle)&edmaObj,
        CSL_EDMA3_CMD_INTR_ENABLE, &arg);
}

/*---------------------------------------------------------------------------*/
 /** 
 * ============================================================================
 *   @n@b EDMA_intDisable
 *
 *   @b Description
 *   @n Disables a tranfer completion interrupt.
 *
 *   @b Arguments
 *   @verbatim
       tccIntNum      interrupt number to be disabled

     @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None.
 *
 *   <b> Post Condition </b>
 *   @li The events of this number are not recognized.
 *
 *   @b Modifies
 *   @n A bit in IER IERH is cleared
 *
 *
 *   @b Example
 *   @verbatim
     EDMA_handle handle;
     Uint32 tccIntNum = 1;
      ...
     EDMA_intDisable(tccIntNum);
      ...
     @endverbatim
 * ============================================================================
 */
IDEF void EDMA_intDisable (
    Uint32    tccIntNum
)
{

    CSL_Edma3CmdIntr arg = { 0 };

    arg.region = CSL_EDMA3_REGION_GLOBAL;

    if (tccIntNum < 32) {  /* There are 64 channels; One register can hold
                              bits corresponding to 32 channels */
        arg.intr = 1 << tccIntNum;
    }
    else {
        arg.intrh = (1 << (tccIntNum - 32));
    }

    CSL_edma3HwControl ((CSL_Edma3Handle)&edmaObj,
        CSL_EDMA3_CMD_INTR_DISABLE, &arg);
}

 /** 
 * ============================================================================
 *   @n@b EDMA_intClear
 *
 *   @b Description
 *   @n Clears a tranfer completion interrupt.
 *
 *   @b Arguments
 *   @verbatim
       tccIntNum      interrupt number to be cleared

     @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None.
 *
 *   <b> Post Condition </b>
 *   @li None.
 *
 *   @b Modifies
 *   @n A bit in IPR or IPRH is cleared
 *
 *
 *   @b Example
 *   @verbatim
     EDMA_handle handle;
     Uint32 tccIntNum = 1;
      ...
     EDMA_intClear(tccIntNum);
      ...
     @endverbatim
 * ============================================================================
 */
/*---------------------------------------------------------------------------*/
IDEF void EDMA_intClear (
    Uint32    tccIntNum
)
{

    CSL_Edma3CmdIntr arg = { 0 };

    arg.region = CSL_EDMA3_REGION_GLOBAL;

    if (tccIntNum < 32) {  /* There are 64 channels; One register can hold
                              bits corresponding to 32 channels */
        arg.intr = 1 << tccIntNum;
    }
    else {
        arg.intrh = (1 << (tccIntNum - 32));
    }

    CSL_edma3HwControl ((CSL_Edma3Handle)&edmaObj,
        CSL_EDMA3_CMD_INTRPEND_CLEAR, &arg);
}

/*---------------------------------------------------------------------------*/
/** 
 * ============================================================================
 *   @n@b EDMA_intTest
 *
 *   @b Description
 *   @n Returns the status of a tranfer completion interrupt.
 *
 *   @b Arguments
 *   @verbatim
       tccIntNum      interrupt number to be tested

     @endverbatim
 *
 *   <b> Return Value </b>
 *   @li       0 = flag not set
 *   @li       1 = flag set
 *
 *   <b> Pre Condition </b>
 *   @n  None.
 *
 *   <b> Post Condition </b>
 *   @li None.
 *
 *   @b Modifies
 *   @n None
 *
 *
 *   @b Example
 *   @verbatim
     EDMA_handle handle;
     Uint32 tccIntNum = 1, status;
      ...
     status = EDMA_intTest(tccIntNum);
      ...
     @endverbatim
 * ============================================================================
 */
IDEF Uint32 EDMA_intTest (
    Uint32    tccIntNum
)
{
    Uint32 tmp;
    CSL_Edma3CmdIntr resp = { 0 };

    resp.region = CSL_EDMA3_REGION_GLOBAL;
    CSL_edma3GetHwStatus ((CSL_Edma3Handle)&edmaObj,
        CSL_EDMA3_QUERY_INTRPEND, &resp);

    if (tccIntNum < 32) {   /* There are 64 channels; One register can hold
                               bits corresponding to 32 channels */
        tmp = resp.intr & (1 << tccIntNum);
    }
    else {
        tmp = resp.intrh & (1 << (tccIntNum - 32));
    }

    if (tmp) {
        return 1;
    }
    else {
        return 0;
    }
}

/*---------------------------------------------------------------------------*/
/** 
 * ============================================================================
 *   @n@b EDMA_intReset
 *
 *   @b Description
 *   @n Clears a pending tranfer completion interrupt and disables the
 *      interrupt.
 *
 *   @b Arguments
 *   @verbatim
       tccIntNum      interrupt number to be reset

     @endverbatim
 *
 *   <b> Return Value </b>
 *   @n None
 *
 *   <b> Pre Condition </b>
 *   @n  None.
 *
 *   <b> Post Condition </b>
 *   @li Interrupts are not recognized.
 *
 *   @b Modifies
 *   @n A bit in IPR or IPRH  and IER or IERH cleared
 *
 *
 *   @b Example
 *   @verbatim
     EDMA_handle handle;
     Uint32 tccIntNum = 1, status;
      ...
     EDMA_intReset(tccIntNum);
      ...
     @endverbatim
 * ============================================================================
 */
IDEF void EDMA_intReset (
    Uint32    tccIntNum
)
{

    CSL_Edma3CmdIntr arg = { 0 };

    arg.region = CSL_EDMA3_REGION_GLOBAL;

    if (tccIntNum < 32) {   /* There are 64 channels; One register can hold
                               bits corresponding to 32 channels */
        arg.intr = 1 << tccIntNum;
    }
    else {
        arg.intrh = (1 << (tccIntNum - 32));
    }

    CSL_edma3HwControl ((CSL_Edma3Handle)&edmaObj,
        CSL_EDMA3_CMD_INTR_DISABLE, &arg);
    CSL_edma3HwControl ((CSL_Edma3Handle)&edmaObj,
        CSL_EDMA3_CMD_INTRPEND_CLEAR, &arg);
}

/*---------------------------------------------------------------------------*/
/** 
 * ============================================================================
 *   @n@b EDMA_intResetAll
 *
 *   @b Description
 *   @n Clears all pending tranfer completion interrupts and disables the all
 *      interrupts.
 *
 *   @b Arguments
 *   @n None
 *
 *   <b> Return Value </b>
 *   @n None
 *
 *   <b> Pre Condition </b>
 *   @n  None.
 *
 *   <b> Post Condition </b>
 *   @li Interrupts are not recognized.
 *
 *   @b Modifies
 *   @n All bits in IPR, IPRH, IER and IERH cleared
 *
 *
 *   @b Example
 *   @verbatim
     EDMA_handle handle;
     Uint32 tccIntNum = 1, status;
      ...
     EDMA_intResetAll();
      ...
     @endverbatim
 * ============================================================================
 */
IDEF void EDMA_intResetAll (
    void
)
{

    CSL_Edma3CmdIntr arg = { 0 };

    arg.region = CSL_EDMA3_REGION_GLOBAL;
    arg.intr = 0xFFFFFFFF;
    arg.intrh = 0xFFFFFFFF;

    CSL_edma3HwControl ((CSL_Edma3Handle)&edmaObj,
        CSL_EDMA3_CMD_INTR_DISABLE, &arg);
    CSL_edma3HwControl ((CSL_Edma3Handle)&edmaObj,
        CSL_EDMA3_CMD_INTRPEND_CLEAR, &arg);
}

/*---------------------------------------------------------------------------*/
/** 
 * ============================================================================
 *   @n@b EDMA_link
 *
 *   @b Description
 *   @n Links a child PaRAM entry to the parent.
 *
 *   @b Arguments
 *   @verbatim
       parent      handle to the parent PaRAM (channel)
       child       handle to the child PaRAM

     @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  Parent and child must have been configured.
 *
 *   <b> Post Condition </b>
 *   @li 1. The parent's RLD word of PaRAM is set to the offset of chaild
 *          PaRAM.
 *
 *   @b Modifies
 *   @n Parent PaRAM is modified.
 *
 *
 *   @b Example
 *   @verbatim
     EDMA_handle par_handle, ch_handle;
     EDMA_config par_conf, ch_conf;
     Uint32 chan_no = 1, tab = 4;
     par_handle = EDMA_open(chan_no, EDMA_OPEN_RESET);
     ch_handle = EDMA_allocTable(tab);

     // Configure parent, using EDMA_config
      ...
     // Configure child, using EDMA_config
      ...
      ...
     EDMA_link(par_handle, ch_handle);
      ...
     @endverbatim
 * ============================================================================
 */
IDEF void EDMA_link (
    EDMA_Handle    parent,
    EDMA_Handle    child
)
{

    Uint32 link_offset = (child & 0x0000FFFF);

    CSL_FINS (
       _EDMA_CHANNEL_HANDLE_2X_TO_3X_PARAM_HANDLE (parent)->LINK_BCNTRLD,
        EDMA3CC_LINK_BCNTRLD_LINK, link_offset);
}

/*---------------------------------------------------------------------------*/

#endif /* USEDEFS */
#endif /*_CSL_EDMA_2X_H_ */
/*****************************************************************************\
* End of csl_edma.h
\*****************************************************************************/

