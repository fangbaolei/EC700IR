/*
 *  Copyright 2006 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *
 * @(#) framework_components_1_0 1,0,0,1 1-14-2006 (fcp-b04)
 */

/*
 *! ============================================================
 *! Copyright 2005 by Texas Instruments Incorporated.
 *! All rights reserved. Property of Texas Instruments Incorporated.
 *! Restricted rights to use, duplicate or disclose this code are
 *! granted through contract.
 *! ============================================================
 */
/**
 *  @file       dman3.h
 *
 *  @brief      DMAN3 Interface Definitions (C64P) - 3rd Generation
 *              DMAN. Application interface to the DMA Manager. Grants DMA
 *              resources to XDAIS algorithms.
 */

/**
 *  @defgroup   DSPDMAN3 DMAN3 (C64P)
 *
 *              DMAN3 is an application interface to the 3rd generation DMA
 *              Manager. It provides routines for granting and reclaiming DMA
 *              resources used by algorithms.This module is used
 *              by frameworks or applications which have XDAIS algorithms that
 *              implement the IDMA3 interface.
 *
 *  @remark     For additional information, please refer to the
 *              "Joule DMA Framework Design and Reference Guide" document.
 */

#ifndef DMAN3_
#define DMAN3_

/** @ingroup    DSPDMAN3 */
/*@{*/


#ifdef __cplusplus
extern "C" {
#endif

#ifndef DMAN3_USEFULLPACKAGEPATH
#include <ialg.h>
#include <idma3.h>
#else
#include <ti/xdais/ialg.h>
#include <ti/sdo/fc/acpy3/idma3.h>
#endif

#define DMAN3_MAXDMARECS     32  /**< Maximum number of DMA handles allowed
                                    for a XDAIS algorithm. */

#define DMAN3_MAXGROUPS      20  /**< Maximum number of groups for sharing
                                    TCCs and parameter RAM. */


/**
 *  @name Defines: DMAN3 Status Codes
 *
 */
/*@{*/
#define DMAN3_SOK             0  /**< DMAN3 Status Code: Successful. */
#define DMAN3_EOUTOFMEMORY   -1  /**< DMAN3 Status Code: Failed to allocate
                                    memory for the handle. */
#define DMAN3_EFAIL          -2  /**< DMAN3 Status Code: Unspecified error. */
#define DMAN3_EFREE          -3  /**< DMAN3 Status Code: Failed to free memory.
                                  */
#define DMAN3_EOUTOFTCCS     -4  /**< DMAN3 Status Code: Insufficient number of
                                  * TCCs available to create channels.
                                  */
#define DMAN3_EOUTOFPARAMS   -5  /**< DMAN3 Status Code: Insufficient number of
                                  * PaRams available to create channels.
                                  */
/*@}*/


/**
 *  @brief      The module configuration structure for DMAN3 implementation.
 *              It is set at design time by the system integrator to adjust
 *              the behaviour of the module to be optimal for its execution
 *              environment.
 */
typedef struct DMAN3_Params {
    /*
     * qdmaPaRamBase: Physical base address of the PARAM0 in the EDMA3/QDMA
     * hardware whose resources are being managed by DMAN3.
     *
     * paRamBaseIndex: (0>value>255) represents the first PARAM TABLE ENTRY
     * NUMBER that is assigned by configuration for exclusive DMAN3 allocation.
     *
     * The numPaRamEntries:  number of PARAM Table entries starting at
     * DMAN3_PARAM_BASE_INDEX assigned by configuration for exclusive DMAN3
     * allocation.
     */
    Uns    *qdmaPaRamBase;     /**< DMAN3_QDMA_PARAM_BASE */
    Uns     paRamBaseIndex;    /**< DMAN3_PARAM_BASE_INDEX */
    Uns     numPaRamEntries;   /**< DMAN3_NUM_CONTIGUOUS_PARAM_ENTRIES */

    /*
     * numQdmaChannels: contains the number of Physical QDMA channels
     * that are assigned to DMAN3 via configuration.
     *
     * qdmaChannels: array containing list of DMAN3_NUM_QDMA_CHANNELS
     * many Physical QDMA channels assigned to DMAN3 via configuration.
     */
    Uns     numQdmaChannels;  /**< DMAN3_NUM_QDMA_CHANNELS */
    Uns    *qdmaChannels;     /**< DMAN3_QDMA_CHANNELS[]   */

    /*
     * tccAllocationMaskH and tccAllocationMaskL:  are 32-bit bitmasks
     * representing configuration provided list of TCCs for exclusive DMAN3
     * allocation.
     *
     * For example, for TCC's in the range 0-31 the Low Mask
     * (tccAllocationMaskL) is configured so that a '1' in bit position 'i'
     * indicates the TCC 'i' is assigned to DMAN3.
     */
    Uns     tccAllocationMaskH;    /**< DMAN3_TCC_FREEMASK_H  */
    Uns     tccAllocationMaskL;    /**< DMAN3_TCC_FREEMASK_L  */

    /**
     * Memory Heap ID for dynamic allocation of DMAN3 objects that
     * must be allocated in L1D Internal RAM.
     *
     * A value of -1 indicates that the heap is NOT DEFINED. If heapInternal
     * is not defined then any IDMA3 protocol that requests IDMA3_INTERNAL
     * type 'env' memory will fail
     */
    Int heapInternal;

    /**
     * Memory Heap ID for dyn allocation of private DMAN3
     * data structures that can be allocated in external memory
     *
     * A value of -1 indicates that the heap is NOT DEFINED. If heapExternal
     * is not defined then DMAN3 attempts to use heapInternal. As a consequence
     * at least one of them must be defined.
     */
    Int heapExternal;

    /*
     *  These are arrays containing the number of TCCs and
     *  PaRam entries that will be assigned to groups for sharing.
     *  Channels created with a given group ID will use TCCs and PaRams
     *  allocated for that group ID, and may share them with other channels
     *  created with the same group ID.
     */
    SmUns         numTccGroup[DMAN3_MAXGROUPS];    /**< DMAN3_NUMTCC_GROUP   */
    MdUns         numPaRamGroup[DMAN3_MAXGROUPS];  /**< DMAN3_NUMPARAM_GROUP */

} DMAN3_Params;


/**
 *  @brief      Default module configuration structure (defined in dman3.c).
 */
#ifdef _TMS320C6X
extern far DMAN3_Params DMAN3_PARAMS;
#else
extern DMAN3_Params DMAN3_PARAMS;
#endif



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
 *  @returnval     DMAN3_SOK            Success.
 *  @returnval     DMAN3_EOUTOFMEMORY   Failed to allocate logical handle.
 *  @returnval     DMAN3_EFAIL          Failed to initialize handle or dmaFxns
 *                                   dmaInit() failed.
 *  @returnval     DMAN3_EOUTOFTCCS     Not enough TCCs available for channels.
 *  @returnval     DMAN3_EOUTOFPARAMS   Not enough PaRams available for channels.
 *
 *  @pre        groupId < DMAN3_MAXGROUPS.
 *  @pre        Valid IALG_Handle.
 *  @pre        Valid IDMA3_Fxns pointer.
 *  @pre        <tt>IALG_HandlefxnsimplementationId ==
 *              IDMA3_FxnsimplementationId</tt>
 *  @pre        numberOfChannels returned by algorithm's dmaGetChannelCnt
 *              and dmaGetChannels is not greater than DMAN3_MAXDMARECS.
 *              DMAN3 supports only DMAN3_MAXDMARECS channels.
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
 *  @returnval     DMAN3_SOK   Success.
 *  @returnval     DMAN3_EOUTOFMEMORY   Failed to allocate logical handle.
 *  @returnval     DMAN3_EFAIL          Failed to initialize handle or dmaFxns
 *                                   dmaInit() failed.
 *  @returnval     DMAN3_EOUTOFTCCS     Not enough TCCs available for channels.
 *  @returnval     DMAN3_EOUTOFPARAMS   Not enough PaRams available for channels.
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
 *  @returnval     DMAN3_SOK           Success.
 *  @returnval     DMAN3_EOUTOFMEMORY  Failed to initialize due to lack of memory
 *                                  resources.
 *
 *  @pre        DMAN3_PARAMS must be initialized
 */
extern Void DMAN3_init(Void);


/**
 *  @brief      Remove logical channel resources from one or several algorithm
 *              instances.
 *
 *  @param[in]  algHandle[]		Array of algorithm handles.
 *  @param[in]  dmaFxns[]       Array of IDMA3 Interfaces associated with the
 *                              algorithm handles.
 *  @param[in]  numAlgs 		Number of algorithm handles in algHandle[].
 *
 *  @pre        algHandle is an array of valid IALG_Handle
 *  @pre        dmaFxns is an array of valid IDMA3_Fxns pointer
 *  @pre        <tt>IALG_HandlefxnsimplementationId ==
 *              IDMA2_FxnsimplementationId</tt>
 *  @pre        numberOfChannels returned by algorithm's dmaGetChannelCnt
 *              and dmaGetChannels is >= 0 and not greater than
 *              DMAN3_MAXDMARECS.
 *              In other words, DMAN3 supports only DMAN3_MAXDMARECS
 *              channels.
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
 *  @returnval     DMAN3_SOK   Success.
 *
 *  @pre        numChans >= 0.
 *  @pre        Valid IDMA3 logical channel handles or numChans == 0.
 *
 *  @post       'handle' and 'env' memory freed for each channel.
 */
extern Int DMAN3_freeChannels(IDMA3_Handle channelTab[], Int numChans);


#ifdef __cplusplus
}
#endif /* extern "C" */


/*@}*/ /* ingroup DSPDMAN3 */

#endif /*DMAN3_*/
