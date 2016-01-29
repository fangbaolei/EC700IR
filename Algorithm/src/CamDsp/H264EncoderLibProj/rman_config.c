#include <tistdtypes.h>
#include <std.h>
#include <alg.h>
#include <_alg.h>
#include <iresman.h>
#include <rman.h> 
#include <iresman_hdvicp.h>
#include <ires_hdvicp.h>

#include <edma3_rm.h>
#include <iresman_edma3Chan.h>
#include <ires_edma3Chan.h>
#include "_edma3lld_dm6467.h" 

#include <edma3_config.h>

#define RMAN_PARAMS_ALLOCFXN _ALG_allocMemory
#define RMAN_PARAMS_FREEFXN _ALG_freeMemory 
#define RMAN_PARAMS_NUMREGISTRIES 10 
#define RMAN_PARAMS_LOCKFXN _lockFxn
#define RMAN_PARAMS_UNLOCKFXN _unlockFxn
#define RMAN_PARAMS_GETCONTEXTFXN _getContextFxn
#define RMAN_PARAMS_SETCONTEXTFXN _setContextFxn
#define RMAN_PARAMS_YIELDSAMEPRI FALSE
#define RMAN_PARAMS_YIELDFXN NULL

static Int RMAN_SEMAPHORE = 0x4567; //Random NOT NULL value

/*
 * Array of EDMA3 LLD RM configuration structure for all the scratch groups 
 */
far EDMA3_RM_InstanceInitConfig EDMA3_ScratchResourceConfig[EDMA3_MAXGROUPS];

/*
 * EDMA3 LLD RM configuration structure for the system Resource Manager  
 */
far EDMA3_RM_InstanceInitConfig EDMA3_SystemResourceConfig; 
	
/*
 * Scratch group requirements of each group for Tccs, PaRams, Edma, 
 * and Qdma channels    
 */
static IRESMAN_HdVicpParams _hdvicpParams = {
    0,      /* Size of this structure */
    NULL,
    NULL,
    2,      /* Number of resources to manage */
    NULL,   /* Rsvd for future use */
};

EDMA3_RM_GblConfigParams H264DEC_DM6467_EDMA3_RM_GLOBALCONFIG =
{
    /** Total number of DMA Channels supported by the EDMA3 Controller */
    NUM_DMA_CHANNELS,
    /** Total number of QDMA Channels supported by the EDMA3 Controller */
    NUM_QDMA_CHANNELS,
    /** Total number of TCCs supported by the EDMA3 Controller */
    NUM_TCC,
    /** Total number of PaRAM Sets supported by the EDMA3 Controller */
    NUM_PARAM_SETS,
    /** Total number of Event Queues in the EDMA3 Controller */
    NUM_EVENT_QUEUE,
    /** Total number of Transfer Controllers (TCs) in the EDMA3 Controller */
    NUM_TC,
    /** Number of Regions on this EDMA3 controller */
    NUM_REGION,

    /**
     * \brief Channel mapping existence
     * A value of 0 (No channel mapping) implies that there is fixed association
     * for a channel number to a parameter entry number or, in other words,
     * PaRAM entry n corresponds to channel n.
     */
    CHANNEL_MAPPING_EXISTENCE,

    /** Existence of memory protection feature */
    MEM_PROTECTION_EXISTENCE,

    /** Global Register Region of CC Registers */
    (void *)(CC_BASE_ADDRESS),
    /** Transfer Controller (TC) Registers */
        {
        (void *)(TC0_BASE_ADDRESS),
        (void *)(TC1_BASE_ADDRESS),
        (void *)(TC2_BASE_ADDRESS),
        (void *)(TC3_BASE_ADDRESS),
        (void *)(TC4_BASE_ADDRESS),
        (void *)(TC5_BASE_ADDRESS),
        (void *)(TC6_BASE_ADDRESS),
        (void *)(TC7_BASE_ADDRESS)
        },
    /** Interrupt no. for Transfer Completion */
    XFER_COMPLETION_INT,
    /** Interrupt no. for CC Error */
    CC_ERROR_INT,
    /** Interrupt no. for TCs Error */
        {
        TC0_ERROR_INT,
        TC1_ERROR_INT,
        TC2_ERROR_INT,
        TC3_ERROR_INT,
        TC4_ERROR_INT,
        TC5_ERROR_INT,
        TC6_ERROR_INT,
        TC7_ERROR_INT
        },

    /**
     * \brief EDMA3 TC priority setting
     *
     * User can program the priority of the Event Queues
     * at a system-wide level.  This means that the user can set the
     * priority of an IO initiated by either of the TCs (Transfer Controllers)
     * relative to IO initiated by the other bus masters on the
     * device (ARM, DSP, USB, etc)
     */
        {
        0u,
        1u,
        2u,
        3u,
        0u,
        0u,
        0u,
        0u
        },
    /**
     * \brief To Configure the Threshold level of number of events
     * that can be queued up in the Event queues. EDMA3CC error register
     * (CCERR) will indicate whether or not at any instant of time the
     * number of events queued up in any of the event queues exceeds
     * or equals the threshold/watermark value that is set
     * in the queue watermark threshold register (QWMTHRA).
     */
        {
        16u,
        16u,
        16u,
        16u,
        0u,
        0u,
        0u,
        0u
        },

    /**
     * \brief To Configure the Default Burst Size (DBS) of TCs.
     * An optimally-sized command is defined by the transfer controller
     * default burst size (DBS). Different TCs can have different
     * DBS values. It is defined in Bytes.
     */
        {
        64u,
        64u,
        64u,
        64u,
        0u,
        0u,
        0u,
        0u
        },

    /**
     * \brief Mapping from each DMA channel to a Parameter RAM set,
     * if it exists, otherwise of no use.
     */
        {
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP
        },

     /**
      * \brief Mapping from each DMA channel to a TCC. This specific
      * TCC code will be returned when the transfer is completed
      * on the mapped channel.
      */
        {
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP
        },

    /**
     * \brief Mapping of DMA channels to Hardware Events from
     * various peripherals, which use EDMA for data transfer.
     * All channels need not be mapped, some can be free also.
     */
        {
        0,
        0
        }
};

static EDMA3_RM_InstanceInitConfig H264DEC_DM6467_EDMA3_RM_INSTCONFIG =  {
            /* Resources owned by Region 1 */
            /* ownPaRAMSets */
           {0xFFFFFFFEu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu,
            0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu,
            0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu,
            0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu},

            /* ownDmaChannels */
            {0xFFFFFFFFu, 0xFFFFFFFFu},

            /* ownQdmaChannels */
            {0x0000007Fu},

            /* ownTccs */
            {0xFFFFFFFFu, 0xFFFFFFFFu},

            /* Resources reserved by Region 1 */
            /* resvdPaRAMSets */
            {0x0u, 0x0u, 0x0u, 0x0u, 0x0u, 0x0u, 0x0u, 0x0u,
            0x0u, 0x0u, 0x0u, 0x0u, 0x0u, 0x0u, 0x0u, 0x0u},

            /* resvdDmaChannels */
            {0x0, 0x0},

            /* resvdQdmaChannels */
            {0x0u},

            /* resvdTccs */
            {0x0, 0x0},
};

Uns EDMA3_PaRamScratchSizes[EDMA3_MAXGROUPS] = {
    384,
    20,
    20,
    20,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0
};
 
Uns EDMA3_EdmaChanScratchSizes[EDMA3_MAXGROUPS] = {
    30,
    4,
    4,
    5,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0
};

Uns EDMA3_QdmaChanScratchSizes[EDMA3_MAXGROUPS] = {
    4,
    1,
    1,
    1,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0
};

Uns EDMA3_TccScratchSizes[EDMA3_MAXGROUPS] = {
    30,
    10,
    10,
    10,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0
};

static IRESMAN_Edma3ChanParams _edma3Params = {
    0,
    NULL,
    NULL,
};

static int g_iRmanAlreadySetup = 0;

int H264FHDVENC_RMAN_setup() 
{
	IRES_Status status;

	if ( 1 == g_iRmanAlreadySetup )
	{
		return 0;
	}

	RMAN_numRegistryEntries = 0;
	RMAN_registryEntries = NULL;
	RMAN_registryResmanArgs =  NULL;

	RMAN_PARAMS.numRegistries = RMAN_PARAMS_NUMREGISTRIES;
	RMAN_PARAMS.allocFxn = RMAN_PARAMS_ALLOCFXN;
	RMAN_PARAMS.freeFxn = RMAN_PARAMS_FREEFXN;
	RMAN_PARAMS.yieldFxn = RMAN_PARAMS_YIELDFXN;
	RMAN_PARAMS.yieldSamePriority = RMAN_PARAMS_YIELDSAMEPRI;

	status = RMAN_init();
    if (IRES_OK != status) {
        return -1;
    }

    _hdvicpParams.baseConfig.size = sizeof(IRESMAN_HdVicpParams);
    _hdvicpParams.baseConfig.allocFxn = RMAN_PARAMS.allocFxn;
    _hdvicpParams.baseConfig.freeFxn = RMAN_PARAMS.freeFxn;
	_hdvicpParams.numResources = 2;
	_hdvicpParams.info = NULL;
    status = RMAN_register(&IRESMAN_HDVICP, (IRESMAN_Params *)&_hdvicpParams);
    if (IRES_OK != status) {
        return -1;
    }

	_edma3Params.baseConfig.size = sizeof(IRESMAN_Edma3ChanParams);
    _edma3Params.baseConfig.allocFxn = RMAN_PARAMS.allocFxn;
    _edma3Params.baseConfig.freeFxn = RMAN_PARAMS.freeFxn;
    status = RMAN_register(&IRESMAN_EDMA3CHAN, (IRESMAN_Params *)&_edma3Params);
    if (IRES_OK != status) {
        return -1;
    }

	g_iRmanAlreadySetup = 1;
	return 0;
}

int H264FHDVENC_RMAN_cleanup() 
{
    IRES_Status status ;

	if ( 0 == g_iRmanAlreadySetup )
	{
		return 0;
	}

	if (IRES_OK != RMAN_unregister(&IRESMAN_HDVICP)) {
        return -1;
    }

	if (IRES_OK != RMAN_unregister(&IRESMAN_EDMA3CHAN)) {
        return -1;
    }

    status = RMAN_exit();
    if (IRES_OK != status) {
        return -1;
    }

	g_iRmanAlreadySetup = 0;
    return 0;
}

void TSK_yield() 
{
}

/*
 * EDMA3_PARAMS structure is created using the configured values  
 */
extern EDMA3_Params EDMA3_PARAMS;

/*
 * ======== EDMA3_createResourceManagerObject ========
 * Creates a resource manager object based on the underlying SOC 
 */
EDMA3_RM_Result EDMA3_createResourceManagerObject() 
{
    EDMA3_RM_Result status;
	EDMA3_RM_MiscParam param;

	EDMA3_PARAMS.regionId       = 1;
	EDMA3_PARAMS.globalInit     = 1;
	EDMA3_PARAMS.allocFxn       = RMAN_PARAMS_ALLOCFXN;
	EDMA3_PARAMS.freeFxn        = RMAN_PARAMS_FREEFXN;
	EDMA3_PARAMS.globalConfig	= &H264DEC_DM6467_EDMA3_RM_GLOBALCONFIG;
	EDMA3_PARAMS.regionConfig	= &H264DEC_DM6467_EDMA3_RM_INSTCONFIG;
	EDMA3_PARAMS.numGroups	    = 20;
	EDMA3_PARAMS.contigParams   = 1;

	param.isSlave = EDMA3_PARAMS.globalInit;
	param.param = 0;

    status = EDMA3_RM_create(
				EDMA3_PHYSICALID, 
				(EDMA3_RM_GblConfigParams*)(EDMA3_PARAMS.globalConfig),
				&param
			 );

    return status;
}

EDMA3_RM_Result EDMA3_deleteResourceManagerObject()
{
    EDMA3_RM_Result status;
    status = EDMA3_RM_delete(EDMA3_PHYSICALID, NULL); 
    return status;
}

void * EDMA3_semCreate(Int key, Int count)
{
/*
 * Call RMAN's configured semCreateFxn, cast the returned semaphore value
 * to the Semaphore type expected by EDMA3 RM.   
 */
    return (void *)RMAN_SEMAPHORE;
}

void EDMA3_semDelete(EDMA3_OS_Sem_Handle hSem)
{
}

//-----------------------------------------------------------------------

IRES_Status RMAN_autoRegister()
{
    /* Statically register entries with RMAN */
    RMAN_numRegistryEntries  = 0;
    RMAN_registryEntries = NULL; 
    RMAN_registryResmanArgs = NULL;

    return (IRES_OK);
}

typedef unsigned char UInt8;

typedef struct {
    String      modName;    /**< Name of this module instance. */
    UInt8      *flags;      /**< The current state of this instance */
} GT_Mask;

typedef struct {
	void* PRINTFXN;
	void* PIDFXN;
	void* TIDFXN;
	void* ERRORFXN;
	void* MALLOCFXN;
	void* FREEFXN;
	void* TIMEFXN;
	void* LOCKFXN;
	void* UNLOCKFXN;
	void* INITFXN;
} GT_Config;

GT_Config GT = {NULL};

Int _GT_trace(GT_Mask *mask, Int classId, String format, ...)
{
	return 0;
}

Void GT_init(Void)
{
}

Void _GT_create(GT_Mask *mask, String modName)
{
}
/*
void SYS_abort(char *s, ...)
{
	exit(1);
}
*/
