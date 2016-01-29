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
 *  @file       iresman_protocol_edma3Chan.h 
 *
 *  @brief      IRESMAN PROTOCOL EDMA3CHAN Interface Definitions (C64P) - 
 *              Interface of the protocol implementation to the device  
 *              specific resman, for the construction/deletion of 
 *              EDMA3CHAN resource handles
 */

#ifndef IRESMAN_PROTOCOL_EDMA3CHAN_
#define IRESMAN_PROTOCOL_EDMA3CHAN_

/** @ingroup    ti_sdo_fc_ires_edma3Chan_IRESMAN_EDMA3CHAN */
/*@{*/


#ifdef __cplusplus
extern "C" {
#endif

#include <iresman_protocol.h>

#define IRESMAN_PROTOCOL_EDMA3CHAN_MAX_MEMRECS  1
/*
 * @brief           Arguments that are passed from the IRES Resource manager
 *                  to intialize the handle
 */
typedef struct IRESMAN_EDMA3CHAN_ConstructHandleArgs {
    short edma3Chan;                            /* 
                                                 * EDMA channel allocated. 
                                                 * May be NONE if no EDMA 
                                                 * channel was requested. 
                                                 */
                                                
    short qdmaChan;                             /*
                                                 * QDMA channel allocated. 
                                                 * May be NONE if no QDMA 
                                                 * channel was requested. 
                                                 */
    short numTccs;                              /* Number of Tccs requested */
    short numPaRams;                            /* Number of params allocated */
    short * tccIndex;                           /*
                                                 * Actual indices of tcc(s)
                                                 * allocated for this handle
                                                 * by the EDMA3 RM
                                                 */
    short * paRamIndex;                         /*
                                                 * Actual indices of params
                                                 * allocated for this handle
                                                 * by the EDMA3 RM
                                                 */
    int persistent;                             /*
                                                 * Resource property, 
                                                 * persistent or non-persistent
                                                 */
    unsigned int edma3CCBaseAddress;            /*
                                                 * Base address of the EDMA3 CC
                                                 */
} IRESMAN_EDMA3CHAN_ConstructHandleArgs;


/*
 * Interface to expose to the resource manager to be able to construct handles
 */
extern IRESMAN_ConstructFxns IRESMAN_EDMA3CHAN_CONSTRUCTFXNS;

/*@}*/ /** @ingroup    DSPIRESPROTEDMA3CHAN */


#ifdef __cplusplus
}
#endif /* extern "C" */


#endif  /* IRESMAN_PROTOCOL_EDMA3CHAN_ */

/*
 *  @(#) ti.sdo.fc.ires.edma3chan; 1, 0, 0,48; 5-15-2008 20:30:21; /db/atree/library/trees/fc-e19x/src/
 */

