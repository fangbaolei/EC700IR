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
 *  @file       ti/sdo/fc/edma3/edma3_config.h
 *
 *  @brief      EDMA3 Interface Definitions - Meta-config module to manage 
 *              EDMA3 resources for all Framework Component modules that   
 *              will use them.
 */
 
/**
 *  @defgroup   ti_sdo_fc_edma3_EDMA3 EDMA3
 *   
 *              EDMA3 module is a Meta-config module that manages all the EDMA3
 *              resources including DMA channels, QDMA channels, PaRams, Tccs
 *              required by various users of the EDMA3 device. 
 *              It also supports the notion of scratch sharing of the resources,
 *              and allows for distribution of these resources into various
 *              scratch groups for allocation to algorithms.  
 *              Internally it uses the EDMA3 Low Level Driver package for 
 *              managing the resources.  
 * 
 *  @remark     For additional information on the EDMA3 Low Level Driver, refer  *              to its documentation.
 */

#ifndef ti_sdo_fc_edma3_EDMA3_
#define ti_sdo_fc_edma3_EDMA3_

/** @ingroup    ti_sdo_fc_edma3_EDMA3 */
/*@{*/

#include <iresman.h>
#include <edma3_rm.h>

#ifdef __cplusplus
extern "C" {
#endif

#define EDMA3_GTNAME "ti.sdo.fc.edma3"   /**< Name to identify the GT module*/

#define EDMA3_MAXGROUPS     20           /**< Maximum number of groups for 
                                            sharing EDMA3 resources. */

#define EDMA3_PHYSICALID    0            /**< Physical ID identifying the EDMA3
                                            device being managed */

/**
 *  @brief      The module configuration structure for EDMA3 implementation.
 *              It is set at design time by the system integrator to allow
 *              setting up of the EDMA3 Low Level Resource Manager for acquiring
 *              EDMA3 resources.
 * 
 *  @remarks    Typically, system integrators configure these settings using
 *              XDC config scripts.  System integrators that use that method
 *              of configuration can ignore this structure definition.  It's
 *              auto-generated during the config step by the XDC Tools.
 */
typedef struct EDMA3_Params {

    /**
     *  @brief      Region Number of the region that is being configured by this
     *              Module
     */
    EDMA3_RM_RegionId   regionId;

    /**
     *  @brief      Boolean value indicating if initialization of EDMA3 Global 
     *              Registers should be handled by this module  
     */
    Bool globalInit;

    /**
     *  @brief      Function used to allocate Memory required by this module 
     */
    IRESMAN_PersistentAllocFxn *  allocFxn; 

    /**
     *  @brief      Function used to free Memory used by this module 
     */
    IRESMAN_PersistentFreeFxn *  freeFxn; 

    /**
     *  @brief      Optional configuration information used to initialize 
     *              the SOC specific EDMA3 resource information required by 
     *              the EDMA3 Low Level Resource Manager.
     */
    EDMA3_RM_GblConfigParams * globalConfig; 

    /**
     *  @brief      Optional configuration information used to initialize 
     *              the region specific EDMA3 resource information required by 
     *              the EDMA3 Low Level Resource Manager. 
     */
    EDMA3_RM_InstanceInitConfig * regionConfig; 

    /**
     *  @brief      Number of Scratch Groups into which the EDMA3 resources 
     *              will be distributed.
     */
    Int numGroups;

    /**
     *  @brief      Boolean value indicating in Contiguous paRams are required 
     *              to be allocated to the Scratch Groups 
     */
    Bool contigParams;

} EDMA3_Params;

/**
 *  @brief      Scratch Group specific configuration information for the EDMA3
 *              module. It is set at design time by the system integrator to 
 *              allow sharing of the EDMA3 resources among scratch groups.
 * 
 *  @remarks    Typically, system integrators configure these settings using
 *              XDC config scripts.  System integrators that use that method
 *              of configuration can ignore this structure definition.  It's
 *              auto-generated during the config step by the XDC Tools.
 */
/*@{*/

/**
 *  @brief      Maximum EDMA channel requirements of all scratch groups 
 */
extern far Uns EDMA3_edmaChanScratchSizes[EDMA3_MAXGROUPS];

/**
 *  @brief      Maximum QDMA channel requirements of all scratch groups 
 */
extern far Uns EDMA3_qdmaChanScratchSizes[EDMA3_MAXGROUPS];

/**
 *  @brief      Maximum PaRam requirements of all scratch groups 
 */
extern far Uns EDMA3_paRamScratchSizes[EDMA3_MAXGROUPS];

/**
 *  @brief      Maximum TCC requirements of all scratch groups 
 */
extern far Uns EDMA3_tccScratchSizes[EDMA3_MAXGROUPS];

/*@}*/

/**
 *  @brief      Initialization information to create EDMA3 Low Level Resource 
 *              Manager handles for each scratch group 
 */
extern far EDMA3_RM_InstanceInitConfig EDMA3_scratchResourceConfig[EDMA3_MAXGROUPS]; 

/**
 *  @brief      Initialization information to create the system-level EDMA3 Low  *              Level Resource Manager handle.
 */
extern far EDMA3_RM_InstanceInitConfig EDMA3_systemResourceConfig; 

/**
 *  @brief      Default module configuration structure 
 *              (defined in edma3_config.c).
 */
extern EDMA3_Params EDMA3_PARAMS;

/**
 *  @brief      Function to create the EDMA3 Low Level Resource Manager object 
 *
 *  @retval     EDMA3_RM_SOK or EDMA3_RM Error Code 
 *
 *  @remarks    Typically, system integrators configure these settings using
 *              XDC config scripts.  System integrators that use that method
 *              of configuration can ignore this structure definition.  It's
 *              auto-generated during the config step by the XDC Tools.
 */
extern EDMA3_RM_Result EDMA3_createResourceManagerObject();

/**
 *  @brief      Function to delete the EDMA3 Low Level Resource Manager object 
 *
 *  @retval     EDMA3_RM_SOK or EDMA3_RM Error Code 
 *
 *  @remarks    Typically, system integrators configure these settings using
 *              XDC config scripts.  System integrators that use that method
 *              of configuration can ignore this structure definition.  It's
 *              auto-generated during the config step by the XDC Tools.
 */
extern EDMA3_RM_Result EDMA3_deleteResourceManagerObject();
                                                            
/**
 *  @brief      Function to obtain a handle to the EDMA3 Low Level Resource 
 *              Manager instance corresponding to a particular algorithm in a  
 *              scratch group.
 *  @param[in]  algHandle       Algorithm Handle
 *  @param[in]  scratchGroupId  Scratch Group Id
 *
 *  @retval     NULL or valid handle    
 */
EDMA3_RM_Handle EDMA3_getResourceManager(IALG_Handle algHandle, 
        Int scratchGroupId);

/**
 *  @brief      Function to release the EDMA3 Low Level Resource Manager handle 
 *              corresponding to a particular algorithm in a scratch group.
 * 
 *  @param[in]  algHandle       Algorithm Handle
 *  @param[in]  scratchGroupId  Scratch Group Id
 *  @retval     EDMA3_RM_SOK or EDMA3_RM Error Code 
 */
EDMA3_RM_Result EDMA3_releaseResourceManager(IALG_Handle algHandle,
        Int scratchGroupId);

/**
 *  @brief      Function prototype for creation of Semaphore required to create 
 *              the EDMA3 Low level Resource Manager Handles
 *  @param[in]  key             Key on which to create the Semaphore.
 *  @param[in]  count           Count to initialize the Semaphore.
 *  @retval     NULL or Handle to a valid Semaphore
 */ 
extern Void * EDMA3_semCreate(Int key, Int count);

/**
 *  @brief      Function prototype for deletion of Semaphores created to acquire
 *              EDMA3 Low level Resource Manager Handles
 *  @param[in]  hSem            Handle to a valid semaphore
 */ 
extern Void EDMA3_semDelete(EDMA3_OS_Sem_Handle hSem);

#ifdef __cplusplus
}
#endif /* extern "C" */
 
/*@}*/
 
#endif
/*
 *  @(#) ti.sdo.fc.edma3; 2, 0, 0,56; 5-15-2008 20:29:58; /db/atree/library/trees/fc-e19x/src/
 */

