/* 
 *  Copyright 2008
 *  Texas Instruments Incorporated
 *
 *  All rights reserved.  Property of Texas Instruments Incorporated
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 * 
 */
/**
 *  @file       ti/sdo/fc/acpy3/acpy3.h
 *
 *  @brief      ACPY3 Definitions (C64P) - 3rd Generation ACPY.
 *              Provides a comprehensive list of DMA operations an algorithm
 *              can perform on logical DMA channels it acquires through the
 *              IDMA3 protocol.
 */

/**
 *  @defgroup   DSPACPY3 ACPY3 (C64P)
 *
 *              The ACPY3 module provides a comprehensive list of DMA
 *              operations an algorithm can perform on logical DMA channels
 *              it acquires through the IDMA3 protocol. Example of ACPY3
 *              operations include channel configuration, DMA transfer
 *              scheduling, and DMA transfer synchronization.
 *
 *  @remark     For additional information, please refer to the
 *              "Using DMA with Framework Components for 'C64x+" document.
 */
#ifndef _ACPY3_
#define _ACPY3_


#ifdef __cplusplus
extern "C" {
#endif

/** @ingroup    DSPACPY3 */
/*@{*/

#include "idma3.h"

#define ACPY3_GTNAME "ti.sdo.fc.acpy3"


/**
 *  @brief  ACPY3 16-bit param field structure. These values are passed to
 *          ACPY3_fastConfigure16b() to indicate the field of the
 *          ACPY3_Params structure to be changed.
 */
typedef enum ACPY3_ParamField16b {

    ACPY3_PARAMFIELD_ELEMENTSIZE =  8,
    ACPY3_PARAMFIELD_NUMELEMENTS = 10 ,
    ACPY3_PARAMFIELD_ELEMENTINDEX_SRC = 16,
    ACPY3_PARAMFIELD_ELEMENTINDEX_DST = 18,
    ACPY3_PARAMFIELD_FRAMEINDEX_SRC =  24,
    ACPY3_PARAMFIELD_FRAMEINDEX_DST =  26,
    ACPY3_PARAMFIELD_NUMFRAMES = 28

} ACPY3_ParamField16b;

/**
 *  @brief      ACPY3 32-bit param field structure. These values are passed to
 *          ACPY3_fastConfigure32b() to indicate the field of the
 *          ACPY3_Params structure to be changed.
 */
typedef enum ACPY3_ParamField32b {

    ACPY3_PARAMFIELD_SRCADDR = 4,
    ACPY3_PARAMFIELD_DSTADDR = 12,
    ACPY3_PARAMFIELD_ELEMENTINDEXES = 16,
    ACPY3_PARAMFIELD_FRAMEINDEXES = 24

} ACPY3_ParamField32b;

/**
 *  @brief      ACPY3 DMA Transfer Types:
 *
 *  Note that all transfers can be specified using the ACPY3_2D2D transfer
 *  type, and providing element and frame indices for both source and
 *  destination. However, the other transfer types are provided to simplify
 *  configuration when applicable. For example, when using the ACPY3_1D2D
 *  transfer type, you do not need to set the source element and frame
 *  indices in the ACPY3_Params structure.
 */
typedef enum ACPY3_TransferType {

    /**
     *  Only one element is copied from source to destination.
     *  The size of the element is specified in the elementSize field of
     *  ACPY3_Params
     */
    ACPY3_1D1D,

    /**
     *  After each element is copied to destination, the source
     *  and destination of the next element to copy is updated
     *  as follows:
     *  - src = src + element size
     *  - dst = dst + destination element index
     *
     *  After an entire frame is copied (the frame size specified by the
     *  numElements field of ACPY3_Params), the source and destination of
     *  the next frame to copy are given by:
     *  - src = src + element size
     *  - dst = start of frame + destination frame index
     */
    ACPY3_1D2D,

    /**
     *  This is similar to ACPY3_1D2D, except that source and
     *  destination are updated after an element is copied,
     *  as follows:
     *  - src = src + source element index
     *  - dst = dst + element size
     *
     *  After an entire frame is copied by:
     *  - src = start of frame + source frame index
     *  - dst = dst + element size
     */
    ACPY3_2D1D,

    /**
     *  This transfer type combines ACPY3_1D2D and ACPY3_2D1D,
     *  so that source and destination are updated after an
     *  element is copied by:
     *  - src = src + source element index
     *  - dst = dst + destination element index
     *
     *  After a frame is copied by:
     *  - src = start of frame + source frame index
     *  - dst = start of frame + destination frame index
     */
    ACPY3_2D2D
} ACPY3_TransferType;


/**
 *  @brief  DMA transfer specific parameters.  Defines the configuration
 *          of a logical channel.
 */
typedef struct ACPY3_Params {

    /** 1D1D, 1D2D, 2D1D or 2D2D */
    ACPY3_TransferType transferType;

    /** Source Address of the DMA transfer */
    Void *         srcAddr;

    /** Destination Address of the DMA transfer */
    Void *         dstAddr;

    /** Number of consecutive bytes in each 1D transfer vector (ACNT)   */
    unsigned short elementSize;

    /** Number of 1D vectors in 2D transfers (BCNT) */
    unsigned short numElements;

    /** Number of 2D frames in 3D transfers (CCNT) */
    unsigned short numFrames ;

    /**
     *  Offset in number of bytes from beginning of each 1D vector to the
     *  beginning of the next 1D vector.  (SBIDX)
     */
    short srcElementIndex;

    /**
     *  Offset in number of bytes from beginning of each 1D vector to the
     *  beginning of the next 1D vector.  (DBIDX)
     */
    short dstElementIndex;

    /**
     *  Offset in number of bytes from beginning of 1D vector of current
     *  (source) frame to the beginning of next frame's first 1D vector.
     *  (SCIDX): signed value between -32768 and 32767.
     */
    short srcFrameIndex;

    /**
     *  Offset in number of bytes from beginning of 1D vector of current
     *  (destination) frame to the beginning of next frame's first 1D vector.
     *  (DCIDX): signed value between -32768 and 32767.
     */
    short dstFrameIndex;

    /**
     *  For a linked transfer entry:
     *  -  waitId = -1           : no intermediate wait on this transfer
     *  -  0 <= waitId < numWaits : this transfer can be waited on or polled
     *                            for completion
     *  - Ignored for single-transfers and for the last transfer in a
     *    sequence of linked transfers which are always synchronized with
     *    waitId == (numWaits - 1)
     */
    short waitId;

} ACPY3_Params;


extern IDMA3_ProtocolObj ACPY3_PROTOCOL;

/**
 *  @brief      Functions called by the framework for allocating and
 *              initializing the channel state.
 */


/**
 *  @brief              Configure all DMA transfer settings for the logical channel.
 *
 *  @param[in]  handle      IDMA3 channel handle.
 *  @param[in]  params      DMA transfer specific parameters used to
 *                          configure this logical DMA channel.
 *  @param[in]  transferNo  Indicates the individual transfer to be
 *                          configured using the passed 'params'
 *
 *  @pre        channel must be in active state
 *  @pre        params->waitId != (numWaits - 1), unless configuring the last
 *              transfer.
 *  @pre        valid IDMA_Obj handle
 *  @pre        handle->protocol == &ACPY3_PROTOCOL
 *  @pre        0 <= transferNo < originally requested number of transfers
 */
Void ACPY3_configure(IDMA3_Handle handle, ACPY3_Params *params,
    short transferNo);


/**
 *  @brief      Modify the 16-bit DMA transfer parameter, indicated by the
 *              parameter field id, fieldId, of the current channel settings.
 *
 *  @param[in]  handle      IDMA3 channel handle.
 *  @param[in]  fieldId     Indicates which of the parameters is to be
 *                          modified.
 *  @param[in]  value       New value of the parameter to be modified.
 *  @param[in]  transferNo  Indicates which transfer the parameters correspond
 *                          to (same value that would be passed to
 *                          ACPY3_configure()).
 *                          logical DMA handle and environment (env, envSize)
 *                          fields.
 *
 *  @pre        Algorithm instance must be in "active" state using IALG
 *              interface.
 *
 *  @pre        Channel must be in "active" state.
 *
 *  @remarks    Once a channel has been configured once with ACPY3_configure(),
 *              ACPY3_fastConfigure16b() can be used to update any of the
 *              16-bit parameter fields, for example, the number of elements
 *              in a frame to be transferred.
 *
 *  @remarks    Both an inlined version and a non-inlined version of this
 *              function are supported. To use the inlined version, add the
 *              line \#define ACPY3_FASTCONFIGURE16B_INLINE or
 *              \#define ACPY3_INLINE_ALL to the source file where this
 *              function is used. The \#define must go above the line,
 *              \#include <acpy3.h>. By default, this function will not be
 *              inlined. Another alternative is to define the macro in the
 *              build option of the file or of the project file.
 *
 *  @remarks
 *              - When _ACPY3_DIRTY_BITS is 0, this function requires 8 ticks.
 *              - When _ACPY3_DIRTY_BITS is 1, this function requires 14 ticks.
*/
#if defined(ACPY3_FASTCONFIGURE16B_INLINE) || defined(ACPY3_INLINE_ALL)
#include <ti/sdo/fc/acpy3/acpy3_fastconfigure16b.h>
#else
Void ACPY3_fastConfigure16b(IDMA3_Handle handle, ACPY3_ParamField16b fieldId,
    unsigned short value, short transferNo);
#endif

/**
 *  @brief      Modify the 32-bit DMA transfer parameter, indicated by the
 *              parameter field id, fieldId, of the current channel settings.
 *
 *  @param[in]  handle      IDMA3 channel handle.
 *  @param[in]  fieldId     Indicates which of the parameters is to be
 *                          modified.
 *  @param[in]  value       New value of the parameter to be modified.
 *  @param[in]  transferNo  Indicates which transfer the parameters correspond
 *                          to (same value that would be passed to
 *                          ACPY3_configure()).
 *                          logical DMA handle and environment (env, envSize)
 *                          fields.
 *
 *  @pre        @c handle must be a valid channel handle.
 *
 *  @pre        Algorithm instance must be in "active" state using IALG
 *              interface.
 *
 *  @pre        Channel must be in 'active' state.
 *
 *  @remarks    Once a channel has been configured once with ACPY3_configure(),
 *              ACPY3_fastConfigure32b() can be used to update any of the
 *              32-bit parameter fields, for example, the source address of
 *              the data to be transferred.
 *
 *  @remarks    Both an inlined version and a non-inlined version of this
 *              function are supported. To use the inlined version, add the
 *              line \#define ACPY3_FASTCONFIGURE32B_INLINE or
 *              \#define ACPY3_INLINE_ALL to the source file where this
 *              function is used. The \#define must go above the line,
 *              \#include <acpy3.h>. By default, this function will not be
 *              inlined. Another alternative is to define the macro in the
 *              build option of the file or of the project file.
 *
 *  @remarks
 *              - When _ACPY3_DIRTY_BITS is 0, this function requires 8 ticks.
 *              - When _ACPY3_DIRTY_BITS is 1, this function requires 14 ticks.
*/
#if defined(ACPY3_FASTCONFIGURE32B_INLINE) || defined(ACPY3_INLINE_ALL)
#include <ti/sdo/fc/acpy3/acpy3_fastconfigure32b.h>
#else
Void ACPY3_fastConfigure32b(IDMA3_Handle handle, ACPY3_ParamField32b fieldId,
    Uns value, short transferNo);
#endif

/**
 *  @brief      Indicate that a given transfer will be the last in a sequence
 *              of linked transfers.
 *
 *              This API can be used to dynamically change the number of
 *              transfers in a linked transfers. Any waitId previously
 *              associated with given transfer "transferNo" will be replaced
 *              with the waitId 'numWaits - 1' since ACPY3_wait() uses this
 *              waitId to check for transfer completion.
 *
 *  @param[in]  handle      IDMA3 channel handle.
 *  @param[in]  transferNo  Indicates which transfer that will be the last
 *                          in the set of linked transfers. (This is the same
 *                          value that was passed to ACPY3_configure()).
 *
 *  @remarks    This API can be used if a channel was created to
 *              transfer numTransfers linked transfers, but at
 *              some point, it may be that fewer transfers than
 *              numTransfers should be started.
 *  @remarks    Both an inlined version and a non-inlined version of this
 *              function are supported. To use the inlined version, add the
 *              line \#define ACPY3_SETFINAL_INLINE or
 *              \#define ACPY3_INLINE_ALL to the source file where this
 *              function is used. The \#define must go above the line,
 *              \#include <acpy3.h>. By default, this function will not be
 *              inlined. Another alternative is to define the macro in the
 *              build option of the file or of the project file.
 *
 *  @remarks
 *              - When _ACPY3_DIRTY_BITS is 0, this routine requires 66 ticks.
 *              - When _ACPY3_DIRTY_BITS is 1, this routine requires 66 ticks.
 *
 *  @pre        Valid channel handle.
 *  @pre        Channel must be in 'active' state.
 *
 *  @post       transferNo's waitId is set to 'numWaits - 1'
 *  @post       Any previously associated waitId with the old "final"
 *              transfer is cleared. As a consequence applications may
 *              need to call ACPY3_configure() to restore the
 *              appropriate waitId.
 */
#if defined(ACPY3_SETFINAL_INLINE) || defined(ACPY3_INLINE_ALL)
#include <ti/sdo/fc/acpy3/acpy3_setfinal.h>
#else
Void ACPY3_setFinal(IDMA3_Handle handle, short transferNo);
#endif

/**
 *  @brief       Indicate that a given transfer will be the last
 *              in a sequence of linked transfers.
 *
 *              This API can be used to dynamically change the number of
 *              transfers in a linked transfers. Any waitId previously
 *              This function is only usable when the handle only has
 *              a single waitId.  Otherwise is works exactly the same
 *              as ACPY3_setFinal.  It just runs much faster.
 *
 *  @param[in]  handle      IDMA3 channel handle.
 *  @param[in]  transferNo  Indicates which transfer that will be
 *                          the last in the set of linked transfers.
 *                          (This is the same value that was passed
 *                          to ACPY3_configure()).
 *
 *  @remarks    This API can be used if a channel was created to
 *              transfer numTransfers linked transfers, but at
 *              some point, it may be that fewer transfers than
 *              numTransfers should be started.
 *
 *  @remarks    Both an inlined version and a non-inlined version of this
 *              function are supported. To use the inlined version, add the
 *              line \#define ACPY3_FASTSETFINAL_INLINE or
 *              \#define ACPY3_INLINE_ALL to the source file where this
 *              function is used. The \#define must go above the line,
 *              \#include <acpy3.h>. By default, this function will not be
 *              inlined. Another alternative is to define the macro in the
 *              build option of the file or of the project file.
 *
 *  @remarks    ACPY3_fastSetFinal() does the work of ACPY3_setFinal() but
 *              assumes that there is only 1 TCC allocated to the handle.
 *              It is very much optimized compared to ACPY3_setFinal().
 *
 *  @remarks
 *              - When _ACPY3_DIRTY_BITS is 0, this routine requires 15 ticks.
 *              - When _ACPY3_DIRTY_BITS is 1, this routine requires 21 ticks.
 *
 *  @pre        Valid channel handle.
 *  @pre        Channel must be in 'active' state.
 *
 *  @post       transferNo's waitId is set to 'numWaits - 1'
 */
#if defined(ACPY3_FASTSETFINAL_INLINE) || defined(ACPY3_INLINE_ALL)
#include <ti/sdo/fc/acpy3/acpy3_fastsetfinal.h>
#else
Void ACPY3_fastSetFinal(IDMA3_Handle handle, short transferNo);
#endif

/**
 *  @brief      Submit a single or linked transfer using the most recently
 *              configured transfer parameter settings.
 *
 *  @param[in]  handle      IDMA3 channel handle.
 *
 *  @pre        Valid channel handle.
 *  @pre        Channel must be in 'active' state.
 *  @pre        Channel must be in 'configured' state.
 *
 *  @post       handle->transferPending = TRUE
 *
 *  @remarks    Both an inlined version and a non-inlined version of this
 *              function are supported. To use the inlined version, add the
 *              line \#define ACPY3_START_INLINE or
 *              \#define ACPY3_INLINE_ALL to the source file where this
 *              function is used. The \#define must go above the line,
 *              \#include <acpy3.h>. By default, this function will not be
 *              inlined. Another alternative is to define the macro in the
 *              build option of the file or of the project file.
 *
 *  @remarks    ACPY3_start() requires the following number of ticks:
 * <table align="center" border="1" cellpadding="3" cellspacing="1">
 * <tr>
 *   <th>Number of Linked Transfers</th>
 *   <th>ACPY3_DIRTY_BITS=0</th>
 *   <th>ACPY3_DIRTY_BITS=1</th>
 * </tr>
 * <tr>
 *   <td>1, 2, or 3</td>
 *   <td>89</td>
 *   <td>101</td>
 * </tr>
 * <tr>
 *   <td>4, 8, 12, or 16</td>
 *   <td>92</td>
 *   <td>110</td>
 * </tr>
 * <tr>
 *   <td>all other numbers</td>
 *   <td>149</td>
 *   <td>167</td>
 * </tr>
 * </table>
 */
#if defined(ACPY3_START_INLINE) || defined(ACPY3_INLINE_ALL)
#include <ti/sdo/fc/acpy3/acpy3_start.h>
#else
Void ACPY3_start(IDMA3_Handle handle);
#endif


/**
 *  @brief      Wait for all data transfers on a logical channel to complete.
 *
 *              ACPY3_wait() uses waitId 'numWaits - 1' to wait for the
 *              completion of all transfers.  Therefore, this waitId should
 *              not be used to configure any intermediate transfers.
 *
 *  @param[in]  handle      IDMA3 channel handle.
 *
 *  @pre        Valid channel handle
 *  @pre        Channel must be in 'active' state.
 *
 *  @post       handle->transferPending = FALSE
 *
 *  @remarks    This function will not return until all the data
 *              transfers on the given channel have completed.
 *
 *  @remarks    Both an inlined version and a non-inlined version of this
 *              function are supported. To use the inlined version, add the
 *              line \#define ACPY3_WAIT_INLINE or
 *              \#define ACPY3_INLINE_ALL to the source file where this
 *              function is used. The \#define must go above the line,
 *              \#include <acpy3.h>. By default, this function will not be
 *              inlined. Another alternative is to define the macro in the
 *              build option of the file or of the project file.
 */
#if defined(ACPY3_WAIT_INLINE) || defined(ACPY3_INLINE_ALL)
#include <ti/sdo/fc/acpy3/acpy3_wait.h>
#else
Void ACPY3_wait(IDMA3_Handle handle);
#endif


/**
 *      @brief  Wait for an individual transfer in a Linked transfer to
 *              finish.
 *
 *  @param[in]  handle      IDMA3 channel handle.
 *  @param[in]  waitId      The waitId for the transfer to wait on, that was
 *                          passed in the ACPY3_Params to ACPY3_configure().
 *
 *  @pre        Valid channel handle
 *  @pre        Channel must be in 'active' state.
 *  @pre        0 <= waitId < originally requested number of waitId
 *  @pre        Channel contains one transfer T, i-th transfer, such that
 *              i < handle->numPaRams and T configured with given waitId.
 *
 *  @remarks    This function will not return until the data
 *              transfer that was configured with 'waitId' has
 *              completed.
 */
Void ACPY3_waitLinked(IDMA3_Handle handle, unsigned short waitId);


/**
 *  @brief      Check to see if all dma transfers on logical channel are
 *              finished.
 *
 *  @param[in]  handle      IDMA3 channel handle.
 *
 *  @retval     TRUE        No transfers are pending (i.e., ACPY3_wait() has
 *                          been called since last ACPY3_start()), or all
 *                          transfers on this channel have completed.
 *  @retval     FALSE       One or more transfers on this channel have not
 *                          completed.
 *
 *  @pre        Valid channel handle
 *  @pre        Channel must be in 'active' state.
 *
 *  @remarks    This function will not wait for data transfers
 *              to complete, but will return immediately with
 *              completion status.
 *  @remarks    There is no need to call ACPY3_wait() once
 *              ACPY3_complete() returns TRUE.
 *
 *  @remarks    Both an inlined version and a non-inlined version of this
 *              function are supported. To use the inlined version, add the
 *              line \#define ACPY3_COMPLETE_INLINE or
 *              \#define ACPY3_INLINE_ALL to the source file where this
 *              function is used. The \#define must go above the line,
 *              \#include <acpy3.h>. By default, this function will not be
 *              inlined. Another alternative is to define the macro in the
 *              build option of the file or of the project file.
 */
#if defined(ACPY3_COMPLETE_INLINE) || defined(ACPY3_INLINE_ALL)
#include <ti/sdo/fc/acpy3/acpy3_complete.h>
#else
Bool ACPY3_complete(IDMA3_Handle handle);
#endif


/**
 *  @brief      Non-blocking query to check completion of an individual
 *              transfer in a Linked transfer.
 *
 *  @param[in]  handle      IDMA3 channel handle.
 *  @param[in]  waitId      The waitId for the transfer to check for
 *                          completion, that was passed in the ACPY3_Params
 *                          to ACPY3_configure().
 *
 *
 *  @retval     TRUE        No transfers are pending (i.e., ACPY3_wait() has
 *                          been called since last ACPY3_start()), or the
 *                          individual transfer that was configured with waitId
 *                          has completed.
 *  @retval     FALSE       The transfer configured with waitId has not
 *                          completed.
 *
 *  @pre        Valid channel handle.
 *  @pre        waitId < handle->numTccs
 *  @pre        Channel must be in 'active' state.
 *
 *  @remarks    This function will not wait for the data transfer
 *              to complete, but will return immediately with
 *              completion status.
 *  @remarks    There is no need to call ACPY3_waitLinked() once
 *              ACPY3_completeLinked() returns TRUE.
 */
Bool ACPY3_completeLinked(IDMA3_Handle handle, unsigned short waitId);


/**
 *  @brief      Activate the given channel.
 *
 *  @param[in]  handle      IDMA3 channel handle.
 *
 *  @pre        @c handle must be a valid channel handle.
 *
 *  @pre        Algorithm instance must be in "active" state using IALG
 *              interface.
 *
 *  @post       Channel is in 'active' state, any ACPY3 APIs can be called
 *              using this handle.
 *
 *  @post       handle->transferPending == FALSE
 */
Void ACPY3_activate(IDMA3_Handle handle);


/**
 *  @brief      Deactivates the given channel.
 *
 *  @param[in]  handle      IDMA3 channel handle.
 *
 *  @pre        @c handle must be a valid channel handle.
 *
 *  @pre        Channel must be in 'active' state.
 *
 *  @pre        Algorithm instance must be in 'active' state using IALG
 *              interface
 *
 *  @post       Channel is in 'deactivated' state.
 */
Void ACPY3_deactivate(IDMA3_Handle handle);


/**
 *  @brief              Initialize the ACPY3 module.  Called by framework.
 */
Void ACPY3_init(Void);

/**
 *  @brief              Finalization of the ACPY3 module.
 */
Void ACPY3_exit(Void);


/*@}*/

#ifdef __cplusplus
}
#endif /* extern "C" */


#endif /* _ACPY3_ */

/*
 *  @(#) ti.sdo.fc.acpy3; 1, 0, 2,205; 5-15-2008 20:28:57; /db/atree/library/trees/fc-e19x/src/
 */

