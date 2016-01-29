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
 *  @file       ti/sdo/fc/ires/hdvicp/ires_hdvicp.h 
 *
 *  @brief      IRES PROTOCOL HDVICP Interface Definitions - 
 *              Allows algorithms to request and receive handles representing 
 *              the HDVICP resource.
 */

/**
 *  @addtogroup ti_sdo_fc_ires_hdvicp_IRES_HDVICP IRES HDVICP Protocol
 *
 *              Allows algorithms to request and receive handles representing 
 *              HDVICP resources.
 */

#ifndef ti_sdo_fc_ires_hdvicp_IRES_HDVICP_
#define ti_sdo_fc_ires_hdvicp_IRES_HDVICP_

/** @ingroup    ti_sdo_fc_ires_hdvicp_IRES_HDVICP */
/*@{*/


#ifdef __cplusplus
extern "C" {
#endif

#include "ires_common.h"

/**
 * @brief Name used to describe this protocol
 */
#define IRES_HDVICP_PROTOCOLNAME     "ti.sdo.fc.ires.hdvicp"


/*
 * Note, we wrap the PROTOCOLVERSION in an ifdef so the
 * resource managers and algs get this version data placed in their object
 * files.  Apps, which include rman.h, will have this 'NOPROTOCOLREV' defined.
 */
#ifndef ti_sdo_fc_ires_NOPROTOCOLREV

/**
 * @brief   Protocol Revision for this resource
 */
static IRES_ProtocolRevision IRES_HDVICP_PROTOCOLREVISION = {1, 0, 0}; 

#endif

/**
 *  @brief      Handle to "logical" resource
 */
typedef struct IRES_HDVICP_Obj *IRES_HDVICP_Handle;

/**
 * @brief       Type of resource that can be requested.
 */
typedef enum IRES_HDVICP_RequestType {

    IRES_HDVICP_ID_0 = 0,
    IRES_HDVICP_ID_1 = 1,
    IRES_HDVICP_ID_ANY = 2
} IRES_HDVICP_RequestType;

/**
 *  @brief      HDVICP Protocol Arguments definition.
 */
typedef struct IRES_HDVICP_ProtocolArgs {

   IRES_ProtocolArgs base;
   IRES_HDVICP_RequestType id;                  /**< ID of the HDVICP resource 
                                                     being requested, could be 
                                                     IRES_HDVICP_ID_0,
                                                     IRES_HDVICP_ID_1 or 
                                                     IRES_HDVICP_ID_ANY */ 
} IRES_HDVICP_ProtocolArgs;

/**
 *  @brief          IRES_HDVICP_Obj extends the generic IRES_Obj structure
 *                  that is returned back to the algorithm requesting the
 *                  resource.
 */
typedef struct  IRES_HDVICP_Obj {

    IRES_Obj ires;
    Void *info;                                 /**< RSVD for future use */
    Int id;                                     /**< ID of the HDVICP being , 
                                                     granted, could be 
                                                     IRES_HDVICP_ID_0 or  
                                                     IRES_HDVICP_ID_1 */
} IRES_HDVICP_Obj;


/**
 * @brief           HDVICP Register Layout 
 */
typedef struct IRES_HDVICP_RegisterLayer {

    int size;
} IRES_HDVICP_RegisterLayer;


/**
 * @brief       Static Resource Properties 
 */
typedef struct IRES_HDVICP_Properties {

    int size;
    IRES_HDVICP_RegisterLayer *globalRegs;      /* Register layer of HDVICP */ 
} IRES_HDVICP_Properties;

#ifdef __cplusplus
}
#endif /* extern "C" */

/*@}*/

#endif
/*
 *  @(#) ti.sdo.fc.ires.hdvicp; 1, 0, 0,80; 5-15-2008 20:30:49; /db/atree/library/trees/fc-e19x/src/
 */

