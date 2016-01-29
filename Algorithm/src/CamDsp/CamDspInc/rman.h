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
 *  @file       ti/sdo/fc/rman/rman.h
 *
 *  @brief      IRES generic Resource Manager RMAN Definitions - Generic
 *              Resource Manager that manages resource allocation for algorithms
 *              that implement the IRES interface
 *
 */

/**
 *  @defgroup   ti_sdo_fc_rman_RMAN Resource Manager for IRES-based resources
 *
 *              IRES generic Resource Manager RMAN Definitions - Generic
 *              Resource Manager that manages resource allocation for algorithms
 *              that implement the IRES interface
 */

#ifndef ti_sdo_fc_rman_RMAN_
#define ti_sdo_fc_rman_RMAN_

#define ti_sdo_fc_ires_NOPROTOCOLREV

#ifdef __cplusplus
extern "C" {
#endif

/** @ingroup    ti_sdo_fc_rman_RMAN */
/*@{*/

#include "ires.h"
#include "iresman.h"

/**
 *  @brief      Name used to identify the GT module 
 */
#define RMAN_GTNAME  "ti.sdo.fc.rman"

/**
 *  @brief      Size of the Protocol Name in the Resource Registry Table
 */
#define RMAN_PROTOCOLNAMESIZE 200

/**
 *  @brief      Declare abstract handle for Semaphore object 
 */
typedef struct Sem_Obj * RMAN_SemHandle;


/**
 *  @brief      The module configuration structure for RMAN. 
 *              It is set at design time by the system integrator to register 
 *              the specific Resource Managers(IRESMAN implementations). 
 *
 *  @remarks    Typically, system integrators configure these settings using
 *              XDC config scripts.  System integrators that use that method
 *              of configuration can ignore this structure definition.  It's
 *              auto-generated during the config step by the XDC Tools. 
 */
typedef struct RMAN_Params {

    /*
     *  @brief      Number of registry entries in the RMAN table
     */
    short numRegistries;

    /*
     *  @brief      Memory allocation function for all RMAN memory requirements
     */
    IRESMAN_PersistentAllocFxn * allocFxn;

    /*
     *  @brief      Memory free function for all RMAN memory requirements
     */
    IRESMAN_PersistentFreeFxn * freeFxn;

    /*
     *  @brief      RMAN_yield if yielding is supported, else NULL. 
     */
    IRES_YieldFxn yieldFxn;  

    /*
     *  @brief      Flag indicating if yield to same priority is okay 
     */
    Bool yieldSamePriority;

} RMAN_Params;

/**
 *  @brief      Yield Arguments passed to the algorithm if RMAN's useDSKT2 
 *              flag is set. These arguments are used by the yield function 
 *              set/get context and release/acquire locks for a particular 
 *              scratch group.
 */
typedef struct RMAN_YieldArgs {

    /*
     *  @brief      groupId of the resource  
     */
    Int groupId;

    /*
     *  @brief      Flag indicating if same-priority yields should occur. 
     */
    Bool yieldFlag;

} RMAN_YieldArgs;


/*
 *  @brief      Initialization parameters for the generic Resource Manager
 */
extern far RMAN_Params RMAN_PARAMS;

/*
 *  @brief      RMAN Table to hold IRESMAN registrations
 */
extern far IRESMAN_Fxns * RMAN_TABLE[];

/*
 *  @brief      Free Table to hold entries freed from RMAN Table
 */
extern far short RMAN_FREE_ENTRIES[];

/*
 *  @brief      Number of entries statically configured for the Resource 
 *              Registry table. Set this variable to zero if no static 
 *              configuration is required
 */
extern far short RMAN_numRegistryEntries;

/*
 *  @brief      To allow for static configuration of the Resource Registry 
 *              table this can be set to a valid configuration table or to NULL.
 *              Set this variable to NULL if no static configuration is required
 */
extern far IRESMAN_Fxns ** RMAN_registryEntries;

/*
 *  @brief      Initialization arguments for the entries to be registered
 *              Set this variable to NULL if no static configuration is required
 */
extern far IRESMAN_Params ** RMAN_registryResmanArgs;


/**
 *  @brief       Initialize the RMAN object with static information from the
 *               headers/configuration etc.  This function has to return
 *               successfully before the other APIs are called.
 *
 *  @retval      IRES_OK                IRES RMAN created successfully
 *  @retval      IRES_ENORESOURCE       One of the calls to the device specific
 *                                      Resource Manager failed
 */
IRES_Status RMAN_init(Void);


/**
 *  @brief       Delete the generic IRES RMAN and release memory back
 *
 *  @retval      IRES_OK                IRES RMAN exited successfully
 *  @retval      IRES_EFAIL             Error closing device specific RESMAN
 */
IRES_Status RMAN_exit(Void);


/**
 *  @brief       Register the protocol/protocol revision and the device
 *               specific resource manager implementation with the Resource
 *               Registry of the RMAN.
 *
 *  @param[in]   resmanFxns             Device-specific implementation of
 *                                      the RESMAN interfaces
 *  @param[in]   initArgs               Arguments to the initialization
 *                                      interface of the RESMAN interfaces
 *  @retval      IRES_OK                Registered successfully
 *  @retval      IRES_EEXISTS           Protocol already exists
 *  @retval      IRES_ENORESOURCE       Device specific Resman failed to
 *                                      initialize
 *                                      Resource Manager failed
 *  @retval      IRES_EFAIL             Error closing device specific RESMAN
 */
IRES_Status RMAN_register(IRESMAN_Fxns * resmanFxns, IRESMAN_Params * initArgs);


/**
 *  @brief       Unregister the protocol and the corresponding resource
 *               manager implementation from the Resource Registry.
 *
 *  @param[in]   resmanFxns             Device-specific implementation of
 *                                      the RESMAN interfaces
 *  @retval      IRES_OK                Unregistered successfully
 *  @retval      IRES_ENOTFOUND         Entry not found in the RMAN Table
 *  @retval      IRES_EFAIL             Error closing device specific RESMAN
 */
IRES_Status RMAN_unregister(IRESMAN_Fxns * resmanFxns);

/**
 * @brief        Assign resources to the algorithm indicated by algHandle.
 *               The resource requirements can be determined by using the
 *               IRES_Fxns implementation
 *  @param[in]   algHandle              Handle to the algorithm
 *  @param[in]   resFxns                Handle to the IRES implementation
 *  @param[in]   scratchGroupId         Id of the scratch group in which
 *                                      resources are requested from
 *                                      -1 if persistent resources are
 *                                      requested.
 *                                      Note: If performing cooperative 
 *                                      preemption, use the same scratchGroupId  *                                      to request resources and to create 
 *                                      algorithm. Failure to do so, will lead
 *                                      to unexpected behaviour.
 *  @retval      IRES_OK                Resources assigned successfully
 *  @retval      IRES_EALG              One of the calls to the IRES interfaces
 *                                      of the algorithm failed
 *  @retval      IRES_ENOMEM            Not enough memory
 *  @retval      IRES_ENOTFOUND         Entry not found
 *  @retval      IRES_ENORESOURCE       Resources unavailable
 *  @retval      IRES_EFAIL             Error freeing handles etc
 */
IRES_Status RMAN_assignResources(IALG_Handle algHandle, IRES_Fxns * resFxns,
    Int scratchGroupId);


/**
 *  @brief      Free resources held by the algorithm resource handle
 *
 *  @param[in]  algHandle           Handle to the algorithm
 *  @param[in]  resFxns             Handle to the IRES implementation
 *  @param[in]  scratchGroupId      Id of the scratch group in which
 *                                  resources were requested
 *                                  -1 if persistent resources are
 *                                  requested
 *  @retval     IRES_OK             Resources freed successfully
 *  @retval     IRES_EALG           One of the calls to the IRES interfaces
 *                                  of the algorithm failed
 *  @retval     IRES_ENOMEM         Not enough memory
 *  @retval     IRES_ENOTFOUND      Entry not found
 *  @retval     IRES_ENORESOURCE    Error obtaining resource information
 *  @retval     IRES_EFAIL          Error freeing handles etc
 *  @retval     IRES_ENOMEM         Out of memory
 */
IRES_Status RMAN_freeResources(IALG_Handle algHandle, IRES_Fxns * resFxns, Int
    scratchGroupId);

/**
 *  @brief      Activate resource identified by the @c resourceHandle held by
 *              the algorithm.
 *
 *  @param[in]  algHandle           Handle to the algorithm
 *  @param[in]  resourceHandle      Handle to the resource
 *  @param[in]  resFxns             Handle to the IRES implementation
 *  @param[in]  scratchGroupId      Id of the scratch group in which
 *                                  resources are requested from
 *
 *  @retval     IRES_OK             Resource activated successfully
 */
IRES_Status RMAN_activateResource(IALG_Handle algHandle,
    IRES_Handle resourceHandle, IRES_Fxns * resFxns, Int scratchGroupId);

/**
 *  @brief      Activate all resources held by the algorithm
 *  @param[in]  algHandle           Handle to the algorithm
 *  @param[in]  resFxns             Handle to the IRES implementation
 *  @param[in]  scratchGroupId      Id of the scratch group in which
 *                                  resources are requested from
 *
 *  @retval     IRES_OK             All resources activated successfully
 */
IRES_Status RMAN_activateAllResources(IALG_Handle algHandle,
    IRES_Fxns * resFxns, Int scratchGroupId);

/**
 *  @brief      Deactivate resource identified by the @c resourceHandle held by
 *              the algorithm.
 *
 *  @param[in]  algHandle           Handle to the algorithm
 *  @param[in]  resourceHandle      Handle to the resource
 *  @param[in]  resFxns             Handle to the IRES implementation
 *  @param[in]  scratchGroupId      Id of the scratch group in which
 *                                  resources are requested from
 *
 *  @retval     IRES_OK             Resource deactivated successfully
 */
IRES_Status RMAN_deactivateResource(IALG_Handle algHandle,
    IRES_Handle resourceHandle, IRES_Fxns * resFxns, Int scratchGroupId);

/**
 *  @brief      Deactivate all resources held by the algorithm
 *
 *  @param[in]  algHandle           Handle to the algorithm
 *  @param[in]  resFxns             Handle to the IRES implementation
 *  @param[in]  scratchGroupId      Id of the scratch group in which
 *                                  resources are requested from
 *
 *  @retval     IRES_OK             Resources deactivated successfully
 */
IRES_Status RMAN_deactivateAllResources(IALG_Handle algHandle,
    IRES_Fxns * resFxns, Int scratchGroupId);


/**
 *  @brief      Yield function specifically for use when DSKT2 based  
 *              algorithms need cooperative preemption and multi-tasking support
 *              This function is called by the algorithm at pre-determined 
 *              points during its execution, to allow another algorithm of same,
 *              or higher prioirty to preempt its execution.
 *
 *  @param[in]  resource            Enum identifying type of resource (ALL,  
 *                                  Scratch Memory, Scratch DMA, IRES Resource)
 *  @param[in]  algYieldContext     Handle to the yield context that contains  
 *                                  information supplied by the algorithm 
 *                                  including the algorithm handle, 
 *                                  the corresponding context save/restore fxns
 *                                  and arguments with which to call these fxns.
 *  @param[in]  yieldArgs           Framework supplied yield arguments that are
 *                                  passed to the algorithm at the time of 
 *                                  granting resource handles. These should be
 *                                  passed back unmodified to this function.
 */
void RMAN_yield(IRES_YieldResourceType resource,
    IRES_YieldContextHandle algYieldContext, IRES_YieldArgs yieldArgs);


/*@}*/ /** @ingroup    DSPRMAN */

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif  /* RMAN_ */
/*
 *  @(#) ti.sdo.fc.rman; 2, 0, 0,110; 5-15-2008 20:31:09; /db/atree/library/trees/fc-e19x/src/
 */

