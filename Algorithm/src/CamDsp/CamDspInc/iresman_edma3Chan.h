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
 *  @file       ti/sdo/fc/ires/edma3chan/iresman_edma3chan.h
 *
 *  @brief      IRESMAN EDMA3 specific Resource Manager Definitions - 
 *              EDMA3 resource manager implementation
 *
 */

/**
 *  @addtogroup ti_sdo_fc_ires_edma3chan_IRESMAN_EDMA3CHAN IRESMAN EDMA3 - 
 *              EDMA3 specific Resource Manager and corresponding
 *              IRES Protocol Implementation 
 */

#ifndef ti_sdo_fc_ires_edma3chan_IRESMAN_EDMA3CHAN_
#define ti_sdo_fc_ires_edma3chan_IRESMAN_EDMA3CHAN_

/** @ingroup    ti_sdo_fc_ires_edma3chan_IRESMAN_EDMA3CHAN */
/*@{*/


#ifdef __cplusplus
extern "C" {
#endif

#include <iresman.h>
#include <iresman_protocol.h>
#include <edma3_rm.h>

#include "iresman_protocol_edma3Chan.h"

/**
 * @brief        Configuration parameters for Edma3Chan resman 
 *               implementation
 */
typedef struct IRESMAN_Edma3ChanParams {

    /**
     * The base configuration information
     */
    IRESMAN_Params baseConfig;

} IRESMAN_Edma3ChanParams ; 

/**
 *  @brief        Function to return the name of the protocol 
 *  @retval       String name identifying the protocol
 */
String IRESMAN_EDMA3CHAN_getProtocolName();

/**
 *  @brief        Function to return the revision of the protocol 
 *  @retval       Revision identifying the protocol
 */
IRES_ProtocolRevision * IRESMAN_EDMA3CHAN_getProtocolRevision();

/**
 *  @brief        Function to initialize the device specific resource manager
 *                implementation
 *  @param[in]    initArgs           Initialization arguments that may be 
 *                                   interpreted by the device specific 
 *                                   implementation
 *  @retval       IRES_Status        IRES_OK         Success
 *                                   IRES_EEXISTS    Already initialized
 *                                   IRES_EFAIL      Error
 *
 */
IRES_Status IRESMAN_EDMA3CHAN_init( IRESMAN_Params * initArgs);

/**
 *  @brief        Function called when RESMAN is no longer needed. Use it to 
 *                free memory allocated etc
 *  @retval       IRES_Status        IRES_OK         Success
 *                                   IRES_ENOINIT    Not initialized
 *                                   IRES_EFAIL      Error
 */
IRES_Status IRESMAN_EDMA3CHAN_exit();

/**
 *  @brief        Function that returns the IRES_Handle to the resource 
 *                requested using the IRES_ResourceProtocolArgs.
 *
 *  @param[in]    algHandle          Handle of the algorithm that requests 
 *                                   the resources
 *  @param[in]    protocolArgs       Arguments to be interpreted by the
 *                                   IRES_Manager implementation to determine
 *                                   which actual physical resources to obtain
 *  @param[in]    scratchGroupId     Group Id required if scratch resources are
 *                                   requested, -1 otherwise
 *  @param[out]   status             Status of this call
 *
 *  @retval       IRES_Handle        Valid handle corresponding to the resource
 *                                   requested on success, or NULL on error.
 */
IRES_Handle IRESMAN_EDMA3CHAN_getHandles(IALG_Handle algHandle, 
        IRES_ProtocolArgs *protocolArgs, Int scratchGroupId, 
        IRES_Status *status);

/**
 *  @brief        Frees handles back to the resource pool
 *
 *  @param[in]    algHandle          Handle of the algorithm that frees 
 *                                   the resources
 *  @param[in]    algResourceHandle  Handle that is to be released
 *  @param[in]    protocolArgs       Arguments that help determine which 
 *                                   resources to free
 *  @param[in]    scratchGroupId     Group Id required if scratch resources are
 *                                   requested, -1 otherwise
 *
 *  @retval       IRES_Status        IRES_OK     Success
 *                                   IRES_EFAIL  Failure
 */
IRES_Status IRESMAN_EDMA3CHAN_freeHandle(IALG_Handle algHandle, 
        IRES_Handle algResourceHandle, IRES_ProtocolArgs *protocolArgs, 
        Int scratchGroupId);

/**
 *  @brief       Memory allocation functions for all of RMANs
 *               and other IRES rotocol's memory requirements
 */
extern far IRESMAN_PersistentAllocFxn _RMAN_allocPersistent;

/**
 *  @brief       Memory free functions for all of RMANs
 *               and other IRES protocol's memory requirements
 */
extern far IRESMAN_PersistentFreeFxn _RMAN_freePersistent;

/**
 * @brief        IRES RESMAN implementation for EDMA3 Linked transfers
 */
extern IRESMAN_Fxns IRESMAN_EDMA3CHAN;

/*
 * Interface to expose to the resource manager to be able to construct handles
 */
extern IRESMAN_ConstructFxns IRESMAN_EDMA3CHAN_CONSTRUCTFXNS;

#ifdef __cplusplus
}
#endif /* extern "C" */


/*@}*/ /** @ingroup */

#endif
/*
 *  @(#) ti.sdo.fc.ires.edma3chan; 1, 0, 0,48; 5-15-2008 20:30:21; /db/atree/library/trees/fc-e19x/src/
 */

