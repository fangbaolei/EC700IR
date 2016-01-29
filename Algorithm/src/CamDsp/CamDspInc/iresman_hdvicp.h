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
 *  @file       ti/sdo/fc/ires/hdvicp/iresman_hdvicp.h
 *
 *  @brief      IRESMAN
 *
 */

/**
 *  @addtogroup ti_sdo_fc_ires_hdvicp_IRESMAN_HDVICP IRESMAN PROTOCOL - HDVICP
 *
 *              HDVICP specific Resource Manager and corresponding
 *              IRES Protocol Implementation.
 */


#ifndef ti_sdo_fc_ires_hdvicp_IRESMAN_HDVICP_
#define ti_sdo_fc_ires_hdvicp_IRESMAN_HDVICP_

/** @ingroup    ti_sdo_fc_ires_hdvicp_IRESMAN_HDVICP */
/*@{*/


#ifdef __cplusplus
extern "C" {
#endif

#include <iresman.h>

/*
 *      @brief Maximum number of resources that can be managed  
 */
#define IRES_HDVICP_MAXRESOURCES 10

typedef struct IRESMAN_HdVicpParams {

    IRESMAN_Params baseConfig;

    int numResources;           /* Number of HDVICP resources to be managed */
    
    Void * info;

} IRESMAN_HdVicpParams; 

/**
 *  @brief        Function to return the name of the protocol 
 *  @retval       String name identifying the protocol
 */
String IRESMAN_HDVICP_getProtocolName();

/**
 *  @brief        Function to return the revision of the protocol 
 *  @retval       Revision identifying the protocol
 */
IRES_ProtocolRevision * IRESMAN_HDVICP_getProtocolRevision();

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
IRES_Status IRESMAN_HDVICP_init( IRESMAN_Params * initArgs);

/**
 *  @brief        Function called when RESMAN is no longer needed. Use it to 
 *                free memory allocated etc
 *  @retval       IRES_Status        IRES_OK         Success
 *                                   IRES_ENOINIT    Not initialized
 *                                   IRES_EFAIL      Error
 */
IRES_Status IRESMAN_HDVICP_exit();

/**
 *  @brief        Function that returns the IRES_Handle to the resource 
 *                requested using the IRES_ResourceProtocolArgs.
 *
 *  @param[in]    algHandle          Handle of the algorithm that is acquiring 
 *                                   resources 
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
IRES_Handle IRESMAN_HDVICP_getHandles(IALG_Handle algHandle,
        IRES_ProtocolArgs *protocolArgs, Int scratchGroupId, 
        IRES_Status *status);

/**
 *  @brief        Frees handles back to the resource pool
 *
 *  @param[in]    algHandle          Handle of the algorithm that is releasing 
 *                                   resources 
 *  @param[in]    algResourceHandle  Handle that is to be released
 *  @param[in]    protocolArgs       Arguments that help determine which 
 *                                   resources to free
 *  @param[in]    scratchGroupId     Group Id required if scratch resources are
 *                                   requested, -1 otherwise
 *
 *  @retval       IRES_Status        IRES_OK     Success
 *                                   IRES_EFAIL  Failure
 */
IRES_Status IRESMAN_HDVICP_freeHandles(IALG_Handle algHandle, 
        IRES_Handle algResourceHandle, IRES_ProtocolArgs *protocolArgs, 
        Int scratchGroupId);

/**
 *  @brief       Memory allocation functions for all of RMANs
 *               and other IRES protocol's memory requirements
 */
extern far IRESMAN_PersistentAllocFxn _RMAN_allocPersistent;

/**
 *  @brief       Memory free functions for all of RMANs
 *               and other IRES protocol's memory requirements
 */
extern far IRESMAN_PersistentFreeFxn _RMAN_freePersistent;

/**
 * @brief        IRES RESMAN implementation for HDVICP 
 */
extern IRESMAN_Fxns IRESMAN_HDVICP;


#ifdef __cplusplus
}
#endif /* extern "C" */


/*@}*/

#endif
/*
 *  @(#) ti.sdo.fc.ires.hdvicp; 1, 0, 0,80; 5-15-2008 20:30:49; /db/atree/library/trees/fc-e19x/src/
 */

