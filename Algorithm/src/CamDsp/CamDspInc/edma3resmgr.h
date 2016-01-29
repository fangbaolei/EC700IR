/*******************************************************************************
**+--------------------------------------------------------------------------+**
**|                            ****                                          |**
**|                            ****                                          |**
**|                            ******o***                                    |**
**|                      ********_///_****                                   |**
**|                      ***** /_//_/ ****                                   |**
**|                       ** ** (__/ ****                                    |**
**|                           *********                                      |**
**|                            ****                                          |**
**|                            ***                                           |**
**|                                                                          |**
**|         Copyright (c) 1998-2006 Texas Instruments Incorporated           |**
**|                        ALL RIGHTS RESERVED                               |**
**|                                                                          |**
**| Permission is hereby granted to licensees of Texas Instruments           |**
**| Incorporated (TI) products to use this computer program for the sole     |**
**| purpose of implementing a licensee product based on TI products.         |**
**| No other rights to reproduce, use, or disseminate this computer          |**
**| program, whether in part or in whole, are granted.                       |**
**|                                                                          |**
**| TI makes no representation or warranties with respect to the             |**
**| performance of this computer program, and specifically disclaims         |**
**| any responsibility for any damages, special or consequential,            |**
**| connected with the use of this program.                                  |**
**|                                                                          |**
**+--------------------------------------------------------------------------+**
*******************************************************************************/

/** \file   edma3resmgr.h
    \brief  EDMA3 Resource Manager Internal header file.

    This file contains implementation specific details used by the RM internally

    (C) Copyright 2006, Texas Instruments, Inc

    \version    0.1.0     Joseph Fernandez  - Created
                0.2.0     Anuj Aggarwal     - Modified it for EDMA3 package
                                            - Added multiple instances
                                            capability
                0.2.1     Anuj Aggarwal     - Modified it for more run time
                                            configuration.
                                            - Made EDMA3 package OS
                                            independent.
                0.2.2     Anuj Aggarwal     - Critical section handling code
                                            modification. Uses semaphore and
                                            interrupts disabling mechanism
                                            for resource sharing.
                0.3.0     Anuj Aggarwal     - Renamed EDMA3_DVR to EDMA3_DRV
                                            - IPR bit clearing in RM ISR
                                              issue fixed.
                                            - Sample application made generic
                0.3.1     Anuj Aggarwal     - Added DMA/QDMA Channel to TCC
                                              mapping, to fix QDMA missed
                                              event issue.
                0.3.2     Anuj Aggarwal     - Added support for POLL mode
                                            - Added a new API to modify the
                                              CC Register.
                1.0.0     Anuj Aggarwal     - Fixed resource allocation related
                                              bugs.
                1.0.0.1     Anuj Aggarwal   - Fixed spurious missed event
                                              generation related bug.
                1.0.0.2     Anuj Aggarwal   - Made the EDMA3 package RTSC
                                              compliant.
                1.0.0.3     Anuj Aggarwal   - Changed the directory structure
                                              as per RTSC standard.
                1.01.00.01  Anuj Aggarwal   - a) Added new APIs to allocate
                                              logical channels
                                              b) Created EDMA3 config files
                                              for different platforms
                                              c) Misc changes
                1.02.00.01  Anuj Aggarwal   - a) Added DM6467 support
                                              b) Fixed some MRs
                1.03.00.01  Anuj Aggarwal   - a) Added non-RTSC PJT files
                                              b) IOCTL Interface added.
                                              c) Fixed some MRs.
                1.04  Anuj Aggarwal         - a) Added new IOCTLs and APIs.
                                              b) Number of maximum Resource
                                                Manager Instances is configurable.
                                              c) Header files modified to have
                                                extern "C" declarations.


 */

#ifndef _EDMA3_RES_MGR_H_
#define _EDMA3_RES_MGR_H_


/** Include Resource Manager header file */
#include <edma3_rm.h>

/* For the EDMA3 Register Layer functionality. */
#include <edma3_rl_cc.h>
#include <edma3_rl_tc.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Number of PaRAM Sets actually present on the SoC. This will be updated
 * while creating the Resource Manager Object.
 */
extern unsigned int edma3NumPaRAMSets;


/** Define for setting all bits of the EDMA3 Controller Registers */
#define EDMA3_RM_SET_ALL_BITS              (0xFFFFFFFFu)

/* Other Mask defines */
/** DCHMAP-PaRAMEntry bitfield Clear */
#define EDMA3_RM_DCH_PARAM_CLR_MASK                (~EDMA3_CCRL_DCHMAP_PAENTRY_MASK)
/** DCHMAP-PaRAMEntry bitfield Set */
#define EDMA3_RM_DCH_PARAM_SET_MASK(paRAMId)       (((EDMA3_CCRL_DCHMAP_PAENTRY_MASK >> EDMA3_CCRL_DCHMAP_PAENTRY_SHIFT) & (paRAMId)) << EDMA3_CCRL_DCHMAP_PAENTRY_SHIFT)
/** QCHMAP-PaRAMEntry bitfield Clear */
#define EDMA3_RM_QCH_PARAM_CLR_MASK                (~EDMA3_CCRL_QCHMAP_PAENTRY_MASK)
/** QCHMAP-PaRAMEntry bitfield Set */
#define EDMA3_RM_QCH_PARAM_SET_MASK(paRAMId)       (((EDMA3_CCRL_QCHMAP_PAENTRY_MASK >> EDMA3_CCRL_QCHMAP_PAENTRY_SHIFT) & (paRAMId)) << EDMA3_CCRL_QCHMAP_PAENTRY_SHIFT)
/** QCHMAP-TrigWord bitfield Clear */
#define EDMA3_RM_QCH_TRWORD_CLR_MASK               (~EDMA3_CCRL_QCHMAP_TRWORD_MASK)
/** QCHMAP-TrigWord bitfield Set */
#define EDMA3_RM_QCH_TRWORD_SET_MASK(paRAMId)      (((EDMA3_CCRL_QCHMAP_TRWORD_MASK >> EDMA3_CCRL_QCHMAP_TRWORD_SHIFT) & (paRAMId)) << EDMA3_CCRL_QCHMAP_TRWORD_SHIFT)
/** QUEPRI bits Clear */
#define EDMA3_RM_QUEPRI_CLR_MASK(queNum)           (~(EDMA3_CCRL_QUEPRI_PRIQ0_MASK << ((queNum) * EDMA3_CCRL_QUEPRI_PRIQ1_SHIFT)))
/** QUEPRI bits Set */
#define EDMA3_RM_QUEPRI_SET_MASK(queNum,quePri)    ((EDMA3_CCRL_QUEPRI_PRIQ0_MASK & (quePri)) << ((queNum) * EDMA3_CCRL_QUEPRI_PRIQ1_SHIFT))
/** QUEWMTHR bits Clear */
#define EDMA3_RM_QUEWMTHR_CLR_MASK(queNum)         (~(EDMA3_CCRL_QWMTHRA_Q0_MASK << ((queNum) * EDMA3_CCRL_QWMTHRA_Q1_SHIFT)))
/** QUEWMTHR bits Set */
#define EDMA3_RM_QUEWMTHR_SET_MASK(queNum,queThr)  ((EDMA3_CCRL_QWMTHRA_Q0_MASK & (queThr)) << ((queNum) * EDMA3_CCRL_QWMTHRA_Q1_SHIFT))

/** OPT-TCC bitfield Clear */
#define EDMA3_RM_OPT_TCC_CLR_MASK                  (~EDMA3_CCRL_OPT_TCC_MASK)
/** OPT-TCC bitfield Set */
#define EDMA3_RM_OPT_TCC_SET_MASK(tcc)             (((EDMA3_CCRL_OPT_TCC_MASK >> EDMA3_CCRL_OPT_TCC_SHIFT) & (tcc)) << EDMA3_CCRL_OPT_TCC_SHIFT)

/** PaRAM Set Entry for Link and B count Reload field */
#define EDMA3_RM_PARAM_ENTRY_LINK_BCNTRLD   (5u)


/**
 * \defgroup Edma3ResMgrInt Internal Interface Definition for Resource Manager
 *
 * Documentation of the Internal Interface of Resource Manager
 *
 * @{
 */


/**
 * \defgroup Edma3ResMgrIntObjMaint Object Maintenance
 *
 * Maintenance of the EDMA3 Resource Manager Object
 *
 * @{
 */


/** To maintain the state of the EDMA3 Resource Manager Object */
typedef enum {
    /** Object deleted */
    EDMA3_RM_DELETED   = 0,
    /** Obect Created */
    EDMA3_RM_CREATED   = 1,
    /** Object Opened */
    EDMA3_RM_OPENED    = 2,
    /** Object Closed */
    EDMA3_RM_CLOSED    = 3
} EDMA3_RM_ObjState;



/**
 * \defgroup Edma3RMIntBoundVals Boundary Values
 *
 * Boundary Values for Logical Channel Ranges
 *
 * @{
 */
/** Max of DMA Channels */
#define EDMA3_RM_DMA_CH_MAX_VAL        (EDMA3_MAX_DMA_CH - 1u)

/** Min of Link Channels */
#define EDMA3_RM_LINK_CH_MIN_VAL       (EDMA3_MAX_DMA_CH)

/** Max of Link Channels */
#define EDMA3_RM_LINK_CH_MAX_VAL       (EDMA3_MAX_DMA_CH + EDMA3_MAX_PARAM_SETS - 1u)

/** Min of QDMA Channels */
#define EDMA3_RM_QDMA_CH_MIN_VAL       (EDMA3_MAX_DMA_CH + EDMA3_MAX_PARAM_SETS)

/** Max of QDMA Channels */
#define EDMA3_RM_QDMA_CH_MAX_VAL       (EDMA3_MAX_DMA_CH + EDMA3_MAX_PARAM_SETS + EDMA3_MAX_QDMA_CH - 1u)

/** Max of Logical Channels */
#define EDMA3_RM_LOG_CH_MAX_VAL       (EDMA3_RM_QDMA_CH_MAX_VAL)



/* @} Edma3RMIntBoundVals */





/**
 * \brief EDMA3 Hardware Instance Configuration Structure.
 *
 * Used to maintain information of the EDMA3 HW configuration.
 * One such storage exists for each instance of the EDMA 3 HW.
 */
typedef struct
    {
    /** HW Instance Id of the EDMA3 Controller */
    unsigned int        phyCtrllerInstId;

    /** State information of the Resource Manager object */
    EDMA3_RM_ObjState   state;

    /** Number of active opens of RM Instances */
    unsigned int        numOpens;

    /**
     * \brief Init-time Configuration structure for EDMA3
     * controller, to provide Global SoC specific Information.
     *
     * This configuration will can be provided by the user at run-time,
     * while calling EDMA3_RM_create().
     */
    EDMA3_RM_GblConfigParams gblCfgParams;

    } EDMA3_RM_Obj;


/**
 * \brief EDMA3 RM Instance Specific Configuration Structure.
 *
 * Used to maintain information of the EDMA3 Res Mgr instances.
 * One such storage exists for each instance of the EDMA3 Res Mgr.
 *
 * Maximum EDMA3_MAX_RM_INSTANCES instances are allowed for
 * each EDMA3 hardware instance, for same or different shadow regions.
 */
typedef struct
    {
    /**
     * Configuration such as region id, IsMaster, Callback function
     * This configuration is passed to the "Open" API.
     * For a single EDMA3 HW controller, there can be EDMA3_MAX_REGIONS
     * different instances tied to different regions.
     */
    EDMA3_RM_Param                  initParam;

    /** Pointer to appropriate Shadow Register region of CC Registers */
    EDMA3_CCRL_ShadowRegs           *shadowRegs;

    /**
     * Pointer to the EDMA3 RM Object (HW specific)
     * opened by RM instance.
     */
    EDMA3_RM_Obj                    *pResMgrObjHandle;

    /** Available DMA Channels to the RM Instance */
    unsigned int        avlblDmaChannels[EDMA3_MAX_DMA_CHAN_DWRDS];

    /** Available QDMA Channels to the RM Instance */
    unsigned int        avlblQdmaChannels[EDMA3_MAX_QDMA_CHAN_DWRDS];

    /** Available PaRAM Sets to the RM Instance */
    unsigned int        avlblPaRAMSets[EDMA3_MAX_PARAM_DWRDS];

    /** Available TCCs to the RM Instance */
    unsigned int        avlblTccs[EDMA3_MAX_TCC_DWRDS];

    /**
     * Sometimes, PaRAM clearing is not required for some particular RM
     * Instances. In that case, PaRAM Sets allocated will NOT be cleared before
     * allocating to any particular user. It is the responsibility of user
     * to program it accordingly, without assuming anything for a specific
     * field because the PaRAM Set might contain junk values. Not programming
     * it fully might result in erroneous behavior.
     * On the other hand, RM instances can also use this variable to get the
     * PaRAM Sets cleared before allocating them to the specific user.
     * User can program only the selected fields in this case.
     *
     * Value '0' : PaRAM Sets will NOT be cleared during their allocation.
     * Value '1' : PaRAM Sets will be cleared during their allocation.
     *
     * This value can be modified using the IOCTL commands.
     */
    unsigned int          paramInitRequired;

    /**
     * Sometimes, global EDMA3 registers (DCHMAP/QCHMAP) and PaRAM Sets should
     * not be modified during EDMA3_RM_allocLogicalChannel (), for some particular RM
     * Instances. In that case, it is the responsibility of user
     * to program them accordingly, when needed, without assuming anything because
     * they might contain junk values. Not programming
     * the registers/PaRAMs fully might result in erroneous behavior.
     * On the other hand, RM instances can also use this variable to get the
     * global registers and PaRAM Sets minimally programmed before allocating them to
     * the specific user.
     * User can program only the remaining fields in this case.
     *
     * Value '0' : EDMA3 registers (DCHMAP/QCHMAP) and PaRAM Sets will NOT be programmed during their allocation.
     * Value '1' : EDMA3 registers (DCHMAP/QCHMAP) and PaRAM Sets will be programmed during their allocation.
     *
     * This value can be modified using the IOCTL commands.
     */
    unsigned int          regModificationRequired;

    }EDMA3_RM_Instance;

/* @} Edma3ResMgrIntObjMaint */


/**
 * \brief EDMA3 Channel-Bound resources.
 *
 * Used to maintain information of the EDMA3 resources
 * (specifically Parameter RAM set and TCC), bound to the
 * particular channel within EDMA3_RM_allocLogicalChannel ().
 */
typedef struct {
    /** PaRAM Set number associated with the particular channel */
    int paRAMId;

    /** TCC associated with the particular channel */
    unsigned int tcc;
} EDMA3_RM_ChBoundResources;


/**
 * \brief TCC Callback - Caters to channel specific status reporting.
 */
typedef struct {
    /** Callback function */
    EDMA3_RM_TccCallback tccCb;

    /** Callback data, passed to the Callback function */
    void *cbData;
} EDMA3_RM_TccCallbackParams;


/* @} Edma3ResMgrInt */

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif    /* _EDMA3_RES_MGR_H_ */
