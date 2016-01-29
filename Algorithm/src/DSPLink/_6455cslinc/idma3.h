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
 *  @file       idma3.h
 *
 *  @brief      IDMA3 Interface Definitions (C64P) - Allows algorithms to
 *              request and receive handles representing private logical DMA
 *              resources.
 */

/**
 *  @defgroup   DSPIDMA3 IDMA3 (C64P)
 *
 *              The IDMA3 interface enables algorithms to request and receive
 *              handles representing private logical DMA resources.
 *
 *  @remark     For additional information, please refer to the
 *              "Joule DMA Framework Design and Reference Guide" document.
 */
#ifndef IDMA3_
#define IDMA3_

/** @ingroup    DSPIDMA3 */
/*@{*/


#ifdef __cplusplus
extern "C" {
#endif

#ifndef IDMA3_USEFULLPACKAGEPATH
#include <ialg.h>
#else
#include <ti/xdais/ialg.h>
#endif

/**
 *  @brief      Memory types.
 */
typedef enum IDMA3_MemType {
    
    /** Internal data memory */
    IDMA3_INTERNAL,             

    /** External data memory */
    IDMA3_EXTERNAL 

} IDMA3_MemType;


/**
 *  @brief      Record containing attributes of memory to allocate.
 */
typedef struct IDMA3_MemRec {

    /** Base address of allocated buffer*/
    Void *base;

    /** Size of buffer to allocate (MAUs)  */
    Uns size;

    /** 
     *  Buffer alignment (0 or a power of 2). Use the values of 0 or 1 to 
     *  indicate no alignment requirements. 
 */
    Uns align;
    
    /** Type of memory to use for allocation */
    IDMA3_MemType memType; 

} IDMA3_MemRec;


/**
 *  @brief      Handle to "logical" DMA channel.
 */
typedef struct IDMA3_Obj *IDMA3_Handle;


/**
 *  @brief      Handle to IDMA3 protocol object
 */
typedef struct IDMA3_ProtocolObj *IDMA3_ProtocolHandle;


/**
 *  @brief      NULL protocol definition
 *
 *              The NULL protocol can be used when no protocol is needed, 
 *              e.g. when the algorithm directly accesses the eDMA PaRAM 
 *              registers, or when the DMA driver library used does not retain
 *              any state. When the NULL protocol is used 'env' buffer is not
 *              allocated            
 */
#define IDMA3_PROTOCOL_NULL ((IDMA3_ProtocolHandle)NULL)


/**
 *  @brief      IDMA3 Priority Levels.
 */
typedef enum IDMA3_Priority {

    IDMA3_PRIORITY_URGENT,
    IDMA3_PRIORITY_HIGH,
    IDMA3_PRIORITY_MEDIUM,
    IDMA3_PRIORITY_LOW

} IDMA3_Priority;


/**
 *  @brief      DMA Channel Descriptor to logical DMA channels.
 */
typedef struct IDMA3_ChannelRec {

    /** Handle to logical DMA channel */
    IDMA3_Handle handle;      

    /** 
     *  Number of DMA transfers that are submitted using this logical channel
     *  handle. <tt>Single (==1) or Linked ( >= 2).</tt> 
     */
    Int numTransfers;

    /** 
     *  Number of individual transfers that can be waited in a linked start.
     *  (1 for single transfers or for waiting all) 
 */
    Int numWaits;    

    /** Relative priority recommendation: High, Medium, Low. */
    IDMA3_Priority priority;   

    /** 
     *  When non-NULL, the protocol object provides interface for querying and
     *  initializing logical DMA channel for use by the given protocol. The
     *  protocol can be IDMA3_PROTOCOL_NULL in this case no 'env' is allocated
     *  The NULL protocol can be used to directly program eDMA registers
     * 
     *  For example, when requesting a logical channel to be used with ACPY3 
     *  APIs, the protocol needs to be set to &ACPY3_PROTOCOL.
     */ 
    IDMA3_ProtocolHandle protocol; 

    /**
     *  When persistent is set to TRUE, the PaRAMs and TCCs will be allocated
     *  exclusively for this channel. They cannot be shared with any other
     *  IDMA3 channel.
     */
    Bool persistent;

} IDMA3_ChannelRec;


/**
 *  @remarks    These fxns are used to query/grant the DMA resources requested
 *              by the algorithm at initialization time, and to change these
 *              resources at runtime. All these fxns are implemented by the
 *              algorithm, and called by the client of the algorithm.
 *  @remarks    implementationId - unique pointer that identifies the module
 *              implementing this interface.
 *  @remarks    dmaChangeChannels() - apps call this whenever the logical
 *              channels are moved at runtime.
 *  @remarks    dmaGetChannelCnt()  - apps call this to query algorithm about
 *              max number of logical dma channel requested.
 *  @remarks    dmaGetChannels() - apps call this to query algorithm about its
 *              dma channel requests at init time, or to get the current
 *              channel holdings.
 *  @remarks    dmaInit() - apps call this to grant dma handle(s) to the
 *              algorithm at initialization. Algorithm initializes the
 *              instance object.
 */
typedef struct IDMA3_Fxns {

    /** TODO: to be documented */
    Void *implementationId;

    /** TODO: to be documented */
    Void    (*dmaChangeChannels)(IALG_Handle, IDMA3_ChannelRec *);

    /** TODO: to be documented */
    Uns (*dmaGetChannelCnt)(Void);
                                  
    /** TODO: to be documented */
    Uns (*dmaGetChannels)(IALG_Handle, IDMA3_ChannelRec *);
    
    /** TODO: to be documented */
    Int  	(*dmaInit)(IALG_Handle, IDMA3_ChannelRec *);
                        
} IDMA3_Fxns;


/**
 *  @remarks    These functions are used to determine memory requirements for
 *              and initialize the IDMA3 protocol's environment that will be
 *              allocated by the DMA manager.
 *              These fxns are implemented by the IDMA3
 *              protocol that is used for a given channel (e.g, ACPY3), and
 *              are passed in the IDMA3_ChannelRec structure to request a
 *              logical DMA channel. If the IDMA3 protocol does not require
 *              any particular one of these functions, it may be set to NULL.
 *  @remarks    getEnvMemRec() - Called by the DMA manager when creating a
 *              logical DMA channel, to query the IDMA3 protocol's memory
 *              requirements for its environment.
 *  @remarks    initHandle() - Called by the DMA manager after allocation the
 *              environment, to allow the IDMA3 protocol to do any
 *              initialization of its environment.
 *  @remarks    deInitHandle() - Called by the DMA manager when a channel is
 *              freed, so that the IDMA3 protocol can do any required
 *              de-initialization or freeing any memory that may have been
 *              allocated in initHandle().
 */
typedef struct IDMA3_ProtocolObj {

    /**
     *  Name of the protocol
     */
    String name;

    /**
     *  Fill in the IDMA3_MemRec with the memory attributes needed for
     *  allocation of the IDMA3 protocol's environment buffer. 
     */
    Void (*getEnvMemRec)(IDMA3_ChannelRec *, IDMA3_MemRec *);

    /** 
     *  Initialize the env stored in the IDMA3 channel handle. Return TRUE on 
     *  success, FALSE otherwise. If FALSE is returned channel creation will 
     *  fail.
     */
    Bool (*initHandle)(IDMA3_Handle);

    /**
     *  De-Initialize the env stored in the IDMA3 channel handle, before it is
     *  freed by the DMA manager. 
     */
    Bool (*deInitHandle)(IDMA3_Handle);

} IDMA3_ProtocolObj;


/**
 *  @brief      IDMA3_Obj holds the private state associated with each
 *              logical DMA channel.
 */
typedef struct IDMA3_Obj {

    /** The number of TCCs allocated to this channel. */
    MdUns numTccs;    

    /** The number of PaRam entries allocated to this channel. */
    MdUns numPaRams;   

    /** TCCs assigned to channel */
    SmUns *tccTable;

    /** PaRAMs assigned to channel */
    Uns *paRamAddr;  

    /** Physical QDMA Channel assigned to handle */
    MdUns qdmaChan;   

    /** 
     *  Set to true when a new transfer is started on this channel. Set to 
     *  false when a wait/sync operation is performed on this channel.
     */                                  
    Bool transferPending;   

    /** 
     *  Implementation dependent private channel data area of size = 'sizeEnv'.
     *  The memory for the 'env'  is allocated and reclaimed by the framework.
     */
    Void *env;

    /**
     *  The channel protocol functions used by the DMA manager to determine
     *  memory requirements for the 'env' 
     */
    IDMA3_ProtocolHandle protocol;  

    /** Indicates if the channel has been allocated with persistent property.*/     
    Bool persistent;

} IDMA3_Obj;


#ifdef __cplusplus
}
#endif /* extern "C" */


/*@}*/ /* ingroup DSPIDMA3 */


#endif  /* IDMA3_ */

