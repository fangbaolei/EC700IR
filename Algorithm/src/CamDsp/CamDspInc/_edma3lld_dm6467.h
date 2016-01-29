/* 
 *  Copyright 2008
 *  Texas Instruments Incorporated
 *
 *  All rights reserved.  Property of Texas Instruments Incorporated
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 * 
 */

/** Total number of DMA Channels supported by the EDMA3 Controller */
#define NUM_DMA_CHANNELS                        (64u)
/** Total number of QDMA Channels supported by the EDMA3 Controller */
#define NUM_QDMA_CHANNELS                       (8u)
/** Total number of TCCs supported by the EDMA3 Controller */
#define NUM_TCC                                 (64u)
/** Total number of PaRAM Sets supported by the EDMA3 Controller */
#define NUM_PARAM_SETS                          (512)
/** Total number of Event Queues in the EDMA3 Controller */
#define NUM_EVENT_QUEUE                         (4u)
/** Total number of Transfer Controllers (TCs) in the EDMA3 Controller */
#define NUM_TC                                  (4u)
/** Number of Regions on this EDMA3 controller */
#define NUM_REGION                              (8u)

/**
 * \brief Channel mapping existence
 * A value of 0 (No channel mapping) implies that there is fixed association
 * for a channel number to a parameter entry number or, in other words,
 * PaRAM entry n corresponds to channel n.
 */
#define CHANNEL_MAPPING_EXISTENCE               (1u)
/** Existence of memory protection feature */
#define MEM_PROTECTION_EXISTENCE                (0u)

/** Global Register Region of CC Registers */
#define CC_BASE_ADDRESS                         (0x01C00000u)
/** Transfer Controller 0 Registers */
#define TC0_BASE_ADDRESS                        (0x01C10000u)
/** Transfer Controller 1 Registers */
#define TC1_BASE_ADDRESS                        (0x01C10400u)
/** Transfer Controller 2 Registers */
#define TC2_BASE_ADDRESS                        (0x01C10800u)
/** Transfer Controller 3 Registers */
#define TC3_BASE_ADDRESS                        (0x01C10C00u)
/** Transfer Controller 4 Registers */
#define TC4_BASE_ADDRESS                        NULL
/** Transfer Controller 5 Registers */
#define TC5_BASE_ADDRESS                        NULL
/** Transfer Controller 6 Registers */
#define TC6_BASE_ADDRESS                        NULL
/** Transfer Controller 7 Registers */
#define TC7_BASE_ADDRESS                        NULL

/** Interrupt no. for Transfer Completion */
#define XFER_COMPLETION_INT                     (84u)
/** Interrupt no. for CC Error */
#define CC_ERROR_INT                            (85u)
/** Interrupt no. for TC 0 Error */
#define TC0_ERROR_INT                           (86u)
/** Interrupt no. for TC 1 Error */
#define TC1_ERROR_INT                           (87u)
/** Interrupt no. for TC 2 Error */
#define TC2_ERROR_INT                           (88u)
/** Interrupt no. for TC 3 Error */
#define TC3_ERROR_INT                           (89u)
/** Interrupt no. for TC 4 Error */
#define TC4_ERROR_INT                           (0u)
/** Interrupt no. for TC 5 Error */
#define TC5_ERROR_INT                           (0u)
/** Interrupt no. for TC 6 Error */
#define TC6_ERROR_INT                           (0u)
/** Interrupt no. for TC 7 Error */
#define TC7_ERROR_INT                           (0u)

/**
 * \brief Mapping of DMA channels 0-31 to Hardware Events from
 * various peripherals, which use EDMA for data transfer.
 * All channels need not be mapped, some can be free also.
 * 1: Mapped
 * 0: Not mapped
 *
 * This mapping will be used to allocate DMA channels when user passes
 * EDMA3_RM_DMA_CHANNEL_ANY as dma channel id (for eg to do memory-to-memory
 * copy). The same mapping is used to allocate the TCC when user passes
 * EDMA3_RM_TCC_ANY as tcc id (for eg to do memory-to-memory copy).
 *
 * To allocate more DMA channels or TCCs, one has to modify the event mapping.
 */
#define DMA_CHANNEL_TO_EVENT_MAPPING_0          (0x3DFF0FFCu)
/**
 * \brief Mapping of DMA channels 32-63 to Hardware Events from
 * various peripherals, which use EDMA for data transfer.
 * All channels need not be mapped, some can be free also.
 * 1: Mapped
 * 0: Not mapped
 *
 * This mapping will be used to allocate DMA channels when user passes
 * EDMA3_RM_DMA_CHANNEL_ANY as dma channel id (for eg to do memory-to-memory
 * copy). The same mapping is used to allocate the TCC when user passes
 * EDMA3_RM_TCC_ANY as tcc id (for eg to do memory-to-memory copy).
 *
 * To allocate more DMA channels or TCCs, one has to modify the event mapping.
 */
#define DMA_CHANNEL_TO_EVENT_MAPPING_1          (0x007F1FFFu)

/*
 *  @(#) ti.sdo.fc.rman.examples.dummyEdma3; 1, 0, 0,81; 5-15-2008 20:31:28; /db/atree/library/trees/fc-e19x/src/
 */

