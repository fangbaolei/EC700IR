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
 *  @file       ti/sdo/fc/dman3/dman3.h
 *
 *  @brief      DMAN3 Interface Definitions - 3rd Generation DMA Manager.
 *              Application interface to the DMA Manager.  Grants DMA
 *              resources to xDAIS algorithms.
 */

/**
 *  @defgroup   ti_sdo_fc_dman3_DMAN3 DMAN3
 *
 *              DMAN3 is an application interface to the 3rd generation DMA
 *              Manager. It provides routines for granting and reclaiming DMA
 *              resources used by algorithms or other non-algorithm users.
 *              This module is used by frameworks or applications which have
 *              XDAIS algorithms that implement the IDMA3 interface.
 *
 *  @remark     For additional information, please refer to the
 *              "C64+ DMA Framework Design and Reference Guide" document.
 */

#ifndef ti_sdo_fc_dman3_DMAN3_
#define ti_sdo_fc_dman3_DMAN3_

/** @ingroup    ti_sdo_fc_dman3_DMAN3 */
/*@{*/


#ifdef __cplusplus
extern "C" {
#endif

#include <ialg.h>
#include "idma3.h"

#define DMAN3_GTNAME "ti.sdo.fc.dman3"  /**< Name to identify the GT module */ 

#define DMAN3_MAXGROUPS      20  /**< Maximum number of groups for sharing
                                    TCCs and parameter RAM. */


/**
 *  @name Defines: DMAN3 Status Codes
 *
 */
/*@{*/
#define DMAN3_SOK             0  /**< DMAN3 Status Code: Successful. */
#define DMAN3_EOUTOFMEMORY   -1  /**< DMAN3 Status Code: Failed to allocate
                                  *   memory for the handle.
                                  */
#define DMAN3_EFAIL          -2  /**< DMAN3 Status Code: Unspecified error. */
#define DMAN3_EFREE          -3  /**< DMAN3 Status Code: Failed to free memory.
                                  */
#define DMAN3_EOUTOFTCCS     -4  /**< DMAN3 Status Code: Insufficient number of
                                  *   TCCs available to create channels.
                                  */
#define DMAN3_EOUTOFPARAMS   -5  /**< DMAN3 Status Code: Insufficient number of
                                  *   PaRams available to create channels.
                                  */
#define DMAN3_ETCCCONFIG     -6  /**< DMAN3 Status Code: The number of TCCs
                                  *   configured for the scratchId is not enough
                                  *   to create the channels. This error can
                                  *   only be returned if @c allowUnshared
                                  *   is set to FALSE.
                                  */
#define DMAN3_EPARAMCONFIG   -7  /**< DMAN3 Status Code: The number of PaRams
                                  *   configured for the scratchId is not enough
                                  *   to create the channels. This error can
                                  *   only be returned if @c allowUnshared
                                  *   is set to FALSE.
                                  */
/*@}*/

/**
 *  @brief      Function prototype for allocating IDMA3 Object's env from
 *              shared scratch memory. Algorithms might specify a particular
 *              IDMA3 protocol that provides custom DMA services. This protocol
 *              might require extra memory for the IDMA3 object's environment.
 *              DMAN3_ScratchAllocFxn might be used to allocate this memory.
 *              If this is NULL, then the env will be
 *              allocated from persistent memory.
 */
typedef Bool (*DMAN3_ScratchAllocFxn)(IALG_Handle alg, Int mutexId,
        IALG_MemRec *memTab, Int numRecs);

/**
 *  @brief      Function prototype for freeing IDMA3 Object's env from
 *              shared scratch memory. If this is NULL then env will be
 *              freed from persistent memory.
 */
typedef Void (*DMAN3_ScratchFreeFxn)(Int mutexId, Void *addr, Uns size);


/**
 *  @brief      The module configuration structure for DMAN3 implementation.
 *              It is set at design time by the system integrator to ensure
 *              optimal sharing of DMA resources for the execution environment.
 *
 *  @remarks    Typically, system integrators configure these settings using
 *              XDC config scripts.  System integrators that use that method
 *              of configuration can ignore this structure definition.  It's
 *              auto-generated during the config step by the XDC Tools.
 */
typedef struct DMAN3_Params {
    /**
     * @brief   Physical base address of the PARAM0 in the EDMA3/QDMA
     *          hardware whose resources are being managed by DMAN3.
     *
     * DMAN3_QDMA_PARAM_BASE
     */
    Uns    *qdmaPaRamBase;

    /**
     *  @brief  Total number of PARAM Table entries on the
     *          hardware (eg, for IVA2 this is 128, for Himalaya, this is 256).
     *
     * DMAN3_MAX_PARAM_ENTRIES
     */
    Uns     maxPaRamEntries;

    /**
     *  @brief  Represents the first PARAM TABLE ENTRY
     *          NUMBER that is assigned by configuration for exclusive DMAN3
     *          allocation.
     *
     *  @remarks        Must be between zero and 255.
     *
     *  DMAN3_PARAM_BASE_INDEX
     */
    Uns     paRamBaseIndex;

    /**
     *  @brief  Number of PARAM Table entries starting at
     *          DMAN3_PARAM_BASE_INDEX assigned by configuration for
     *          exclusive DMAN3 allocation.
     *
     *  DMAN3_NUM_CONTIGUOUS_PARAM_ENTRIES
     */
    Uns     numPaRamEntries;

    /**
     *  @brief  The total number of Physical QDMA channels
     *          available on the hardware.
     *
     *  DMAN3_MAX_QDMA_CHANNELS
     */
    Uns     maxQdmaChannels;

    /**
     *  @brief  The number of Physical QDMA channels
     *          that are assigned to DMAN3 via configuration.
     *
     *  DMAN3_NUM_QDMA_CHANNELS
     */
    Uns     numQdmaChannels;

    /**
     *  @brief  Array of size DMAN3_NUM_QDMA_CHANNELS that will contain
     *          the channel numbers of the Physical QDMA channels assigned
     *          to DMAN3 via configuration.
     *
     *  DMAN3_QDMA_CHANNELS[]
     */
    Uns    *qdmaChannels;

    /**
     *  @brief  32-bit "high" bitmask representing configuration provided list
     *          of TCCs for exclusive DMAN3 allocation.
     *
     *  @remarks        For TCC's in the range 32-63, the High Mask
     *                  (@c tccAllocationMaskH) is configured so that a '1'
     *                  in bit position 'i' indicates the TCC '32 + i' is
     *                  assigned to DMAN3.
     *
     *  DMAN3_TCC_FREEMASK_H
     *
     *  @sa     tccAllocationMaskL;
     */
    Uns     tccAllocationMaskH;

    /**
     *  @brief  32-bit "low" bitmask representing configuration provided list of
     *          TCCs for exclusive DMAN3 allocation.
     *
     *  @remarks        For TCC's in the range 0-31, the Low Mask
     *                  (@c tccAllocationMaskL) is configured so that a '1'
     *                  in bit position 'i' indicates the TCC 'i' is assigned
     *                  to DMAN3.
     *
     *  DMAN3_TCC_FREEMASK_L
     *
     *  @sa     tccAllocationMaskH;
     */
    Uns     tccAllocationMaskL;

    /**
     *  @brief  Memory Heap descriptor for dynamic allocation of DMAN3 objects
     *          that must be allocated in L1D Internal RAM.
     *
     *  @remarks    A value of -1 indicates that the heap is NOT DEFINED.
     *  @remarks    If @c heapInternal is not defined then any IDMA3 protocol
     *              that requests IDMA3_INTERNAL type 'env' memory will fail.
     *
     *  @sa     heapExternal
     */
    Int heapInternal;

    /**
     *  @brief  Memory Heap ID for dyn allocation of private DMAN3
     *          data structures that can be allocated in external memory.
     *
     *  @remarks    A value of -1 indicates that the heap is NOT DEFINED.
     *  @remarks    If @c heapExternal is not defined then DMAN3 attempts to
     *              use @c heapInternal.  As a consequence at least one of
     *              them must be defined.
     *
     *  @sa     heapInternal
     */
    Int heapExternal;

    /**
     *  @brief  Array containing the number of TCCs that will be assigned to
     *          the algorithm groups for sharing.
     *
     *  @remarks        Channels created with a given group ID will use TCCs
     *                  allocated for that group ID, and may share them with
     *                  other channels created with the same group ID.
     */
    unsigned char numTccGroup[DMAN3_MAXGROUPS];    /**< DMAN3_NUMTCC_GROUP  */

    /**
     *  @brief  Array containing the number of PaRams that will be assigned to
     *          the algorithm groups for sharing.
     *
     *  @remarks        Channels created with a given group ID will use PaRams
     *                  allocated for that group ID, and may share them with
     *                  other channels created with the same group ID.
     */
    unsigned short numPaRamGroup[DMAN3_MAXGROUPS];  /**< DMAN3_NUMPARAM_GROUP */

    /**
     *  @brief  Flag to indicate that the internal memory heap should be used
     *          for dynamic allocation of IDMA3 objects.
     *
     *  @remarks    A value of 0 means that IDMA3 objects will be allocated in
     *              the heap specified by @c heapExternal.
     *  @remarks    If the value of @c idma3Internal is non-zero, IDMA3 objects
     *              will be allocated in the heap specified by @c heapInternal.
     */
    Bool          idma3Internal;

    /**
     *  @brief  Function for dynamic allocation of IDMA3 objects 'env' from
     *          shared scratch memory.
     *
     *  @remarks    If @c scratchAllocFxn is NULL then the 'env' will be
     *              allocated from persistent memory.
     *  @remarks    If @c scratchFreeFxn != NULL, then this function
     *              must not be NULL.
     *
     *  @remarks    For example, to allocate 'env' in DSKT2 scratch memory, set
     *              @c scratchAllocFxn to DSKT2_allocScratch().
     */
    DMAN3_ScratchAllocFxn scratchAllocFxn;

    /**
     *  @brief  Function for freeing IDMA3 objects 'env' from shared scratch
     *          memory.
     *
     *  @remarks    If @c scratchAllocFxn is NULL then the 'env' will be
     *              freed from persistent memory.
     *  @remarks    If @c scratchAllocFxn != NULL, then this function must
     *              not be NULL.
     *  @remarks    For example, if 'env' is to be allocated in DSKT2 scratch
     *              memory, set @c scratchFreeFxn to DSKT2_freeScratch().
     */
    DMAN3_ScratchFreeFxn scratchFreeFxn;

    /**
     * @brief   Index of a reserved PaRam entry that will not be used for any
     *          DMA transfers.  This PaRam will be used to set QCHMAP
     *          register to, when there is no activity on the corresponding
     *          QDMA channel.
     */
    Uns nullPaRamIndex;

    /**
     *  @brief  The total number of Transfer Controllers available on the
     *          hardware (eg, 2 for DaVinci, 4 for Himalaya).
     */
    Uns     maxTCs;  

    /**
     *  @brief  Represents the mapping of the QDMA channels owned by  
     *          DMAN3 to the event queue. 
     *
     *  @remarks    For example, qdmaQueueMap[0] specifies queue number for
     *              qdmaChannel[0], etc.
     */
    Uns   *qdmaQueueMap; 

    /**
     *  @brief  Represents the mapping of individual hardware Event queues to
     *          transfer controllers.
     */
    Uns   *queueTCMap;

    /**
     *  @brief  Represents the priority assigned to each of the Event Queues
     *          (and hence the Transfer Controllers).
     */
    Uns   *queuePri;

    /**
     *  @brief  Flag indicating whether DMAN3 should allow algorithms to ask
     *          for more resources than configured into their scratch group.
     *
     *  @remarks    If this field is set to FALSE, the DMAN3 library will fail
     *              to grant DMA channels to an algorithm requesting more TCCs
     *              or PaRams then the number configured for its scratch group.
     *
     *  @remarks    If this field is TRUE, and an algorithm requests more TCCs
     *              or PaRams than the number configured for its scratch group,
     *              DMAN3 will do either of the following:
     *                 - If no algorithm is currently instantiated for the
     *                   given scratch group, DMAN3 will bump up the number of
     *                   TCCs and PaRams to the maximum of the number
     *                   configured and that requested by the algorithm, for
     *                   the scratch group. This will remain in effect until
     *                   all algorithms for that scratch group are deleted.
     *                 - If an algorithm has already been granted DMA channels
     *                   for the given scratch group (so the number of TCCs and
     *                   PaRams for that scratch group has already been
     *                   determined by (1) above), and the number of TCCs or
     *                   PaRams for the scratch group is insufficient for the
     *                   algorithm currently requesting DMA channels, DMAN3
     *                   will allocate new TCCs and PaRams if available. These
     *                   new TCCs and PaRams will not be shared by any other
     *                   algorithm.
     */
    Bool allowUnshared;
} DMAN3_Params;


/**
 *  @brief      Default module configuration structure (defined in dman3.c).
 */
extern DMAN3_Params DMAN3_PARAMS;


/**
 *  @brief      Add one or several algorithms to the DMA Manager.
 *              The DMA Manager will grant DMA resources to the algorithms as a
 *              result. This function is called when initializing XDAIS
 *              algorithm instances.
 *
 *  @param[in]  groupId         Group number for sharing TCCs and PaRAMs.
 *                              Channels created with the same group number
 *                              in a previous or subsequent call to this
 *                              function will share the same TCCs and PaRam,
 *                              as the channels created in this call, so they
 *                              must be used simultaneously. The channels
 *                              created in this function call, will not share
 *                              TCCs and PaRam with eachother, even though
 *                              they use the same group Id. When granting DMA
 *                              resources for several algorithms (i.e. when
 *                              numAlgs is greater than 1) these algorithms
 *                              do not share TCCs and PaRAMs. If two algorithms
 *                              can share TCCs and PaRAMs, two calls to
 *                              DMAN3_grantDmaChannels() are needed.
 *  @param[in]  algHandle[]     Array of algorithm handles.
 *  @param[in]  dmaFxns[]       Array of IDMA3 Interfaces associated with each
 *                              algorithm handle.
 *  @param[in]  numAlgs         Number of algorithms in algHandle array.
 *
 *  @retval     DMAN3_SOK            Success.
 *  @retval     DMAN3_EOUTOFMEMORY   Failed to allocate logical handle.
 *  @retval     DMAN3_EFAIL          Failed to initialize handle or dmaFxns
 *                                   dmaInit() failed.
 *  @retval     DMAN3_EOUTOFTCCS     Not enough TCCs available for channels.
 *  @retval     DMAN3_EOUTOFPARAMS   Not enough PaRams available for channels.
 *
 *  @pre        groupId < DMAN3_MAXGROUPS.
 *  @pre        Valid IALG_Handle.
 *  @pre        Valid IDMA3_Fxns pointer.
 *  @pre        <tt>IALG_Handle->fxns->implementationId ==
 *              IDMA3_Fxns->implementationId</tt>
 *  @pre        numberOfChannels returned by algorithm's dmaGetChannelCnt
 *              and dmaGetChannels is not greater than total number of Tccs
 *              allocated.
 *
 *  @post       DMAN3_SOK on successful allocation of IDMA3 logical handles.
 */
extern Int DMAN3_grantDmaChannels(Int groupId, IALG_Handle algHandle[],
        IDMA3_Fxns *dmaFxns[], Int numAlgs);

/**
 *  @brief      Finalization method of the DMAN module.
 */
extern Void DMAN3_exit(Void);

/**
 *  @brief      Allocate and initialize memory for one or several channel
 *              handles.
 *
 *  @param[in]  groupId     Group number for sharing TCCs and PaRAMs.
 *                          Channels created with the same group number
 *                          in a previous or subsequent call to this
 *                          function will share the same TCCs and PaRam,
 *                          as the channels created in this call, so they
 *                          must be used simultaneously. The channels
 *                          created in this function call, will not share
 *                          TCCs and PaRam with eachother, even though
 *                          they use the same group Id. If two channels can
 *                          share TCCs and PaRAMS then two calls to
 *                          DMAN3_createChannels()  are needed.
 *  @param[in]  dmaTab      Array of IDMA3 channel resource descriptors
 *                          containing the parameters of the requested
 *                          channels.
 *  @param[in]  numChans    Number of entries in dmaTab (ie, number of channels
 *                          to create).
 *  @param[out] dmaTab      On success assign valid and initialized IDMA3
 *                          logical DMA handle and environment (env, envSize)
 *                          fields.
 *
 *  @retval     DMAN3_SOK   Success.
 *  @retval     DMAN3_EOUTOFMEMORY   Failed to allocate logical handle.
 *  @retval     DMAN3_EFAIL          Failed to initialize handle or dmaFxns
 *                                   dmaInit() failed.
 *  @retval     DMAN3_EOUTOFTCCS     Not enough TCCs available for channels.
 *  @retval     DMAN3_EOUTOFPARAMS   Not enough PaRams available for channels.
 *
 *  @pre        Valid dmaTab structure.
 *  @pre        numChans >= 0.
 *  @pre        groupId < DMAN3_MAXGROUPS.
 *
 *  @post       On success, each handle in dmaTab[] will be set to a valid
 *              IDMA3_Handle. The 'env' field of the IDMA3_Handle will be
 *              set to newly allocated memory whose size is determined by
 *              the IDMA3_InitFxns passed in the dmaTab entry.
 *              On failure, the handle of each dmaTab[] entry will be NULL.
 */
extern Int DMAN3_createChannels(Int groupId, IDMA3_ChannelRec dmaTab[],
        Int numChans);


/**
 *  @brief      Initialization method of the DMAN3 module. DMAN3_init() uses
 *              externally configured and provided DMAN3_PARAMS to initialize
 *              DMAN3 resources (QDMA channel map, memory heap...)
 *
 *  @pre        DMAN3_PARAMS must be initialized
 */
extern Void DMAN3_init(Void);


/**
 *  @brief      Remove logical channel resources from one or several algorithm
 *              instances.
 *
 *  @param[in]  algHandle[]             Array of algorithm handles.
 *  @param[in]  dmaFxns[]       Array of IDMA3 Interfaces associated with the
 *                              algorithm handles.
 *  @param[in]  numAlgs                 Number of algorithm handles in algHandle[].
 *
 *  @pre        algHandle is an array of valid IALG_Handle
 *  @pre        dmaFxns is an array of valid IDMA3_Fxns pointer
 *  @pre        <tt>IALG_Handle->fxns->implementationId ==
 *              IDMA2_Fxns->implementationId</tt>
 *  @pre        numberOfChannels returned by algorithm's dmaGetChannelCnt
 *              and dmaGetChannels is >= 0 and not greater than
 *              number of Tccs allocated
 *
 *  @post       Handle and environment memory and allocated resources are
 *              freed/released.
 *
 */
extern Int  DMAN3_releaseDmaChannels(IALG_Handle algHandle[],
    IDMA3_Fxns *dmaFxns[], Int numAlgs);


/**
 *  @brief      Free memory for array of channel handles.
 *
 *  @param[in]  channelTab  Array of IDMA3 handles
 *                          representing logical dma channel state.
 *  @param[in]  numChans    Number of entries in channelTab (ie, number of
 *                          channels to free).
 *
 *  @retval     DMAN3_SOK   Success.
 *
 *  @pre        @c numChans >= 0.
 *  @pre        Valid IDMA3 logical channel handles or numChans == 0.
 *
 *  @post       'handle' and 'env' memory freed for each channel.
 */
extern Int DMAN3_freeChannels(IDMA3_Handle channelTab[], Int numChans);


#ifdef __cplusplus
}
#endif /* extern "C" */


/*@}*/

#endif
/*
 *  @(#) ti.sdo.fc.dman3; 1, 0, 3,203; 5-15-2008 20:29:22; /db/atree/library/trees/fc-e19x/src/
 */

