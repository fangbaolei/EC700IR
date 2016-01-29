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
 *  @file       ti/sdo/fc/ires/iresman_protocol.h
 *
 *  @brief      IRES Resource Protocol Definitions (C64P) - IRES Resource 
 *              specific protocol interface to construct logical handles.
 *              This interface is exposed only to IRESMAN 
 */

#ifndef IRESMAN_PROTOCOL
#define IRESMAN_PROTOCOL

/** @ingroup    ti_sdo_fc_ires_IRESMAN */
/*@{*/


#ifdef __cplusplus
extern "C" {
#endif

#include <ires_common.h>

/**
 *  @brief      Abstract Protocol Construct Arguments structure/pointer 
 *              definition. Arguments passed by the Resource manager to request
 *              a resource from a specific IRES Protocol will extend and supply
 *              the concrete  definition.
 */
typedef struct IRESMAN_ConstructArgs {

    Int32 size;   /* size of this structure */

} IRESMAN_ConstructArgs;


/**
 *  @brief      These functions are used to determine memory requirements for
 *              and initialize the IRES protocol's environment that will be
 *              allocated by the resource manager.
 *
 *  @remarks    These fxns are implemented by the IRES
 *              protocol that is used for a given channel (e.g, ACPY3), and
 *              are passed in the IRES_ProtocolArgs and 
 *              IRES_ResmanProtocolConstructArgs structure to request
 *              a logical resource. 
 *           
 *  @remarks    getName() - Called by the resource manager to query the name
 *              of the protocol to return to the generic Resource Manager RMAN  
 *
 *  @remarks    getRevision() - Called by the resource manager to query the  
 *              revision of the protocol to return to the generic Resource 
 *              Manager RMAN  
 *
 *  @remarks    getMemRecs() - Called by the resource manager when 
 *              creating a resource handle to query the specific resource 
 *              protocol's memory requirements.
 *              Since IRES is an abstract interface, the actual size of the 
 *              handle can be extended by the specific resource protocol.
 *
 *  @remarks    getNumMemRecs() - Called by the resource manager when 
 *              creating a logical resource handle to obtain the number of 
 *              IALG memory request descriptors the protocol uses to request 
 *              memory for the resource handle.
 *
 *  @remarks    constructHandle() - Called by the resource manager after 
 *              allocating memory for the handle, so that the resource specific
 *              custom protocol initializes the handle.
 *
 *  @remarks    destructHandle() - Called by the resource manager just before
 *              the resource is freed, to allow the custom IRES protocol to 
 *              perform any required de-initialization.
 */
typedef struct IRESMAN_ConstructFxns {

    /**
     *  Query for identification for protocol 
     */
    String (*getName)();

    /**
     *  Query for revision of the protocol 
     */
    IRES_ProtocolRevision * (* getRevision)();

    /**
     *  Obtain the memory requirements that must be allocated by the framework
     *  to construct a resource handle.
     *
     *  The 'protocol' determines the size and attributes of the memory that 
     *  represents the logical resource. The size must be at least the size of 
     *  IRES_Obj. The first sizeof(IRES_Obj) bytes are assigned by the 
     *  Resource Manager, the protocol may request additional 'environment'
     *  memory to follow  the IRES_Obj. 
     *
     *  The "IRES_Handle handle" argument may be NULL, in which case the 
     *  resource protocol fills in the memRecs with the attributes of the 
     *  memory that is needed to create a handle.
     *
     *  Resource Manager may call this function anytime after the 
     *  'constructHandle()' call with a valid IRES_Handle to find out 
     *  the memory assigned to the resource handle. 
     *  Prior to the 'constructHandle()' call, or when a NULL IRES_Handle 
     *  argument is supplied, the value(s) returned in the memRecs' 
     *  IALG_MemRec::base fields are undefined. 
     */
    IRES_Status (*getMemRecs)(IRES_Handle handle, 
            IRES_ProtocolArgs *protocolArgs, IALG_MemRec *memRecs);
    
    /**
     *  Obtain the number of IALG memory request descriptors the protocol
     *  uses to request memory for the resource handle.
     */
    Int (*getNumMemRecs)(IRES_ProtocolArgs *protocolArgs);
    
    
    /**
     *  Protocol specific initialization function to finalize the creation 
     *  a resource object (handle). 
     *  The protocol::constructHandle function can extend the IRES_Obj 
     *  structure and must store any additional allocated 'environment' memory
     *  within the resource handle and return the IALG_MemRec descriptors
     *  in protocol::getResourceMemRecs() function.
     *
     *  Two types of arguments are passed to this function, protocolArgs
     *  that are forwarded from the IRES_Protocol and the
     *  constructHandleArgs that consists of information from the Resource
     *  Manager required to construct a Resource handle.
     *  Return valid resource handle on success, and set status to IRES_OK.
     *  Return NULL  and indicate an IRES Error status otherwise.
     */
    IRES_Handle (*constructHandle)(IRES_ProtocolArgs *protocolArgs,
            IALG_MemRec *memRecs, 
            IRESMAN_ConstructArgs * constructHandleArgs, 
            IRES_Status *status);

    /**
     *  Perform any resource specific operations that may be required before
     *  the resource is reclaimed by the resource manager.
     */
    IRES_Status (*destructHandle)(IRES_Handle handle);

} IRESMAN_ConstructFxns;


#ifdef __cplusplus
}
#endif /* extern "C" */

/*@}*/

#endif  /* IRESMAN_PROTOCOL */
/*
 *  @(#) ti.sdo.fc.ires; 1, 0, 0,80; 5-15-2008 20:30:11; /db/atree/library/trees/fc-e19x/src/
 */

