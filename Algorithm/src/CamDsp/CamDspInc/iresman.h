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
 *  @file       ti/sdo/fc/ires/iresman.h
 *
 *  @brief      Device specific Resource Manager Interface Definitions 
 *              IRESMAN - Interface that needs to be implemented 
 *              and registered with the generic RMAN Resource Registry that 
 *              are called to manage corresponding IRES resources 
 */

/**
 *  @defgroup   ti_sdo_fc_ires_IRESMAN  Resource manager interface
 *
 *              The IRESMAN interface needs to be implemented and registered 
 *              with the generic RMAN Resource Registry and is called by the 
 *              Framework resource manager (RMAN) to manage corresponding 
 *              IRES resources 
 */

#ifndef IRESMAN_
#define IRESMAN_

/** @ingroup    ti_sdo_fc_ires_IRESMAN */
/*@{*/

#ifdef __cplusplus
extern "C" {
#endif

#include "ires_common.h"

/**
 *  @brief      Memory allocation function for all of ResMan and other 
 *              IRES protocol's memory requirements
 */
typedef Bool (IRESMAN_PersistentAllocFxn)(IALG_MemRec *memTab, Int numRecs);

/**
 *  @brief      Memory free functions for all of ResMan and other IRES 
 *              protocol's memory requirements
 */
typedef Void (IRESMAN_PersistentFreeFxn)(IALG_MemRec *memTab, Int numRecs);


/**
 * @brief       Configuration Parameters required by the Resource Manager 
 *              implementation. These can be extended by individual resource 
 *              managers if required.
 */
typedef struct IRESMAN_Params {

    /**
     *  @brief              Size of the structure
     */
    Int32 size;

    /**
     *  @brief              Function to allocate memory required by the resource
     *                      manager implementation and the protocol
     */
    IRESMAN_PersistentAllocFxn * allocFxn;

    /**
     *  @brief              Function to free memory released by the resource 
     *                      manager implementation and the protocol
     */
    IRESMAN_PersistentFreeFxn * freeFxn;    

} IRESMAN_Params;

typedef struct IRESMAN_Fxns {

    /**
     *  @brief              Function to query the name of the protocol  
     */
    String (*getProtocolName)();

    /**
     *  @brief              Function to query the revision of hte protocol 
     *  @retval             Pointer to the protocol revision structure 
     */
    IRES_ProtocolRevision * (* getProtocolRevision)();

    /**
     *  @brief              Function to create and initialize the device 
     *                      specific resource manager implementation
     *  @param[in]          initArgs            Initialization arguments maybe 
     *                                          interpreted by device specific 
     *                                          implementation
     *  @retval             IRESMAN_EOK         Success 
     *                      IRESMAN_EEXISTS     RESMAN already initialized
     *                      IRESMAN_EERROR      RESMAN could not be initialized
     *
     */
    IRES_Status (* init)(IRESMAN_Params * initArgs);

    /**
     *  @brief              Function called when RESMAN is no longer needed. 
     *                      Use it to free memory allocated etc
     *  @retval             IRESMAN_EOK         Success 
     *                      IRESMAN_ENOINIT     RESMAN not initialized
     *                      IRESMAN_EERROR      RESMAN could not be reset
     */
    IRES_Status (* exit)();

    /**
     *  @brief              Function that returns the IRES_Handle to resource 
     *                      requested using the IRES_ProtocolArgs
     *  @param[in]          algHandle           Handle of the algorithm that
     *                                          is requesting the resources 
     *  @param[in]          protocolArgs        Arguments to be interpreted by
     *                                          IRES_Manager implementation to 
     *                                          determine actual physical 
     *                                          resources to obtain
     *  @param[in]          scratchGroupId      Scratch Group Id where resources
     *                                          should be allocated
     *  @param[out]         status              IRESMAN_EOK         Success
     *                                          IRESMAN_ENORESOURCE Resource
     *                                                              unavailable
     *                                          IRESMAN_EERROR      Error 
     *                                                              obtaining
     *                                                              handles
     */
    IRES_Handle (* getHandle)(IALG_Handle algHandle, 
            IRES_ProtocolArgs * protocolArgs, Int scratchGroupId, 
            IRES_Status * status);

    /**
     *  @brief              Frees handles back to the resource pool     
     *  @param[in]          algHandle           Handle of the algorithm that
     *                                          is releasing the resources 
     *  @param[in]          resourceHandle      Handle that is to be released
     *  @param[in]          protocolArgs        Arguments that help determine 
     *                                          which resources to free
     *  @retval             IRESMAN_EOK         Success 
     *                      IRESMAN_EERROR      Handle could not be freed
     */
    IRES_Status (* freeHandle)(IALG_Handle algHandle, 
            IRES_Handle resourceHandle, IRES_ProtocolArgs * protocolArgs, 
            Int scratchGroupId);

} IRESMAN_Fxns;

#ifdef __cplusplus
}
#endif /* extern "C" */

/*@}*/

#endif  /* IRESMAN_ */
/*
 *  @(#) ti.sdo.fc.ires; 1, 0, 0,80; 5-15-2008 20:30:10; /db/atree/library/trees/fc-e19x/src/
 */

