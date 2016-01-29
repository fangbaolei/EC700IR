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
 *  @file       ti/sdo/fc/ires/edma3chan/ires_edma3chan.h 
 *
 *  @brief      IRES EDMA3CHAN PROTOCOL Interface Definitions - 
 *              Allows algorithms to request and receive handles representing 
 *              EDMA3 resources associated with a single EDMA3 channel. 
 */

/**
 *  @addtogroup ti_sdo_fc_ires_edma3chan_IRES_EDMA3CHAN IRES EDMA3 Protocol
 *
 *              Allows algorithms to request and receive handles representing 
 *              EDMA3 resources associated with a single EDMA3 channel. 
 */

#ifndef ti_sdo_fc_ires_edma3chan_IRES_EDMA3CHAN_
#define ti_sdo_fc_ires_edma3chan_IRES_EDMA3CHAN_

/** @ingroup    ti_sdo_fc_ires_edma3Chan_IRES_EDMA3CHAN */
/*@{*/

#ifdef __cplusplus
extern "C" {
#endif

#include <ires_common.h>

/**
 * @brief Name used to describe this protocol
 */
#define IRES_EDMA3CHAN_PROTOCOLNAME     "ti.sdo.fc.ires.edma3chan"

/**
 * @brief Name used to describe the GT module 
 */
#define EDMA3CHAN_GTNAME "ti.sdo.fc.ires.edma3chan"

/*
 * Note, we wrap the PROTOCOLVERSION in an ifdef so the
 * resource managers and algs get this version data placed in their object
 * files.  Apps, which include rman.h, will have this 'NOPROTOCOLREV' defined.
 */
#ifndef ti_sdo_fc_ires_NOPROTOCOLREV

/**
 * @brief   Protocol Revision for this resource
 */
static IRES_ProtocolRevision IRES_EDMA3CHAN_PROTOCOLREVISION = {1, 0, 0}; 

#endif

/**
 * @brief       Maximum number of params and tccs that can be requested in one 
 *              handle 
 */
#define IRES_EDMA3CHAN_MAXPARAMS        512 
#define IRES_EDMA3CHAN_MAXTCCS          32 

/**
 * @brief       Macros to represent different PaRam, Channel and tcc types
 */
#define IRES_EDMA3CHAN_PARAM_ANY        512 
#define IRES_EDMA3CHAN_PARAM_NONE       513
#define IRES_EDMA3CHAN_TCC_ANY          514 
#define IRES_EDMA3CHAN_TCC_NONE         515
#define IRES_EDMA3CHAN_EDMACHAN_ANY     516    
#define IRES_EDMA3CHAN_QDMACHAN_ANY     516    
#define IRES_EDMA3CHAN_CHAN_NONE        518

/**
 *  @brief      Handle to "logical" resource
 */
typedef struct IRES_EDMA3CHAN_Obj *IRES_EDMA3CHAN_Handle;

/**
 * @brief       Representation of actual PaRam structure 
 */
typedef struct IRES_EDMA3CHAN_PaRamStruct {
    unsigned int opt;
    unsigned int src;
    unsigned short acnt;
    unsigned short bcnt;
    unsigned int dst;
    unsigned short srcElementIndex;
    unsigned short dstElementIndex;
    unsigned short link;
    unsigned short bCntrld;
    unsigned short srcFrameIndex;
    unsigned short dstFrameIndex;
    unsigned short ccnt;
    unsigned short rsvd;
} IRES_EDMA3CHAN_PaRamStruct;

/**
 *  @brief      EDMA3 Linked Transfer Protocol Arguments definition.
 */
typedef struct IRES_EDMA3CHAN_ProtocolArgs {
    int     size;                 /**< Size of this structure. */
    IRES_RequestMode mode;        /**< Resource request type, scratch or 
                                   *   persistent.
                                   */
    short   numPaRams;            /**< Number of params requested 
                                   * 0 to IRES_EDMA3CHAN_MAXPARAMS
                                   */
    short   paRamIndex;           /**< Describe type of paRams required. 
                                   * Can specify the start Index of the PaRam 
                                   * block (if numPaRams > 1 and requesting
                                   * contiguous PaRams) or request as 
                                   * IRES_EDMA3CHAN_PARAM_ANY
                                   */
    short   numTccs;              /**< Number of Tcc(s) requested
                                   * 0 to IRES_EDMA3CHAN_MAXTCCS
                                   */
    short   tccIndex;             /**< Describe type of tccs required.
                                   * Can specify the start Index of the Tcc
                                   * block (if numPaRams > 1 and requesting
                                   * contiguous Tccs) or request as
                                   * IRES_EDMA3CHAN_TCC_ANY
                                   */
    short   qdmaChan;             /**< Qdma channel required if any */
    short   edmaChan;             /**< Edma channel required if any
                                   * Note:- If a single Edma channel is 
                                   * requested with a single Tcc in one 
                                   * Resource descriptor, then resource 
                                   * allocator will attempt to allocate 
                                   * EDMA Channel # N = Tcc # N.     
                                   * Will fail if the above is unavailable.
                                   */ 

                                  /*
                                   * If either QDMA or EDMA channel is not 
                                   * required , set (qdmaChan or edmaChan)
                                   * to IRES_EDMA3CHAN_CHAN_NONE while 
                                   * requesting an EDMA3CHAN handle. 
                                   */

    short contiguousAllocation;   /**< Flag indicating if contiguous allocation
                                   * of PaRams and Tccs is required
                                   */

    short shadowPaRamsAllocation;  /**< Flag indicating if shadow PaRams 
                                    * need to be allocated 
                                    */
} IRES_EDMA3CHAN_ProtocolArgs;

/**
 *  @brief          IRES_EDMA3CHAN_Obj extends the generic IRES_Obj structure
 *                  that is returned back to the algorithm requesting the
 *                  resource
 */
typedef struct  IRES_EDMA3CHAN_Obj {

    IRES_Obj ires;
    IRES_EDMA3CHAN_PaRamStruct * shadowPaRams;  /**< Actual programmed shadow
                                                     paRam entries */
    unsigned int * assignedPaRamAddresses;      /**< Physical Addresses of 
                                                     assigned paRams */
    short * assignedPaRamIndices;               /**< Assigned paRam Indices */ 
    short * assignedTccIndices;                 /**< Assigned Tcc Indices */
    short assignedNumPaRams;                    /**< Number of assigned paRams*/
    short assignedNumTccs;                      /**< Number of assigned tccs */
    short assignedQdmaChannelIndex;             /**< Assigned qdma channel. 
                                                     Could be 
                                                     IRES_EDMA3CHAN_CHAN_NONE 
                                                     if no QDMA channel was 
                                                     requested */ 
    short assignedEdmaChannelIndex;             /**< Assigned edma channel. 
                                                     Could be 
                                                     IRES_EDMA3CHAN_CHAN_NONE 
                                                     if no EDMA channel was 
                                                     requested */ 
    unsigned int esrBitMaskL;                  
    unsigned int esrBitMaskH;                   /**< Event set register bit mask
                                                     to trigger edma channel if 
                                                     requested.  */
    unsigned int iprBitMaskL;                  
    unsigned int iprBitMaskH;                   /**< Interrupt pending register 
                                                     bit mask to pend on 
                                                     transfer completion. */
} IRES_EDMA3CHAN_Obj;

/**
 *  @brief          IRES_EDMA3CHAN_EDMA3ShadowRegister defines the shadow  
 *                  register part of the EDMA3 CC Register Layer defined below 
 */
typedef struct IRES_EDMA3CHAN_EDMA3ShadowRegister {
    volatile unsigned int ER;
    volatile unsigned int ERH;
    volatile unsigned int ECR;
    volatile unsigned int ECRH;
    volatile unsigned int ESR;
    volatile unsigned int ESRH;
    volatile unsigned int CER;
    volatile unsigned int CERH;
    volatile unsigned int EER;
    volatile unsigned int EERH;
    volatile unsigned int EECR;
    volatile unsigned int EECRH;
    volatile unsigned int EESR;
    volatile unsigned int EESRH;
    volatile unsigned int SER;
    volatile unsigned int SERH;
    volatile unsigned int SECR;
    volatile unsigned int SECRH;
    volatile unsigned char RSVD0[8];
    volatile unsigned int IER;
    volatile unsigned int IERH;
    volatile unsigned int IECR;
    volatile unsigned int IECRH;
    volatile unsigned int IESR;
    volatile unsigned int IESRH;
    volatile unsigned int IPR;
    volatile unsigned int IPRH;
    volatile unsigned int ICR;
    volatile unsigned int ICRH;
    volatile unsigned int IEVAL;
    volatile unsigned char RSVD1[4];
    volatile unsigned int QER;
    volatile unsigned int QEER;
    volatile unsigned int QEECR;
    volatile unsigned int QEESR;
    volatile unsigned int QSER;
    volatile unsigned int QSECR;
    volatile unsigned char RSVD2[360];

} IRES_EDMA3CHAN_EDMA3ShadowRegister;

/**
 *  @brief          IRES_EDMA3CHAN_EDMA3DraeRegister defines the Region Enable  
 *                  register part of the EDMA3 CC Register Layer defined below 
 */
typedef struct IRES_EDMA3CHAN_EDMA3DraeRegister {
    volatile unsigned int DRAE;
    volatile unsigned int DRAEH;
} IRES_EDMA3CHAN_EDMA3DraeRegister;

/*
 * @brief           IRES_EDMA3CHAN_EDMA3RegisterLayer defines the EDMA3 CC 
 *                  Register Layout 
 */
typedef struct IRES_EDMA3CHAN_EDMA3RegisterLayer {
    volatile unsigned int REV;
    volatile unsigned int CCCFG;
    volatile unsigned char RSVD0[248];
    volatile unsigned int DCHMAP[64];
    volatile unsigned int QCHMAP[8];
    volatile unsigned char RSVD1[32];
    volatile unsigned int DMAQNUM[8];
    volatile unsigned int QDMAQNUM;
    volatile unsigned char RSVD2[28];
    volatile unsigned int QUETCMAP;
    volatile unsigned int QUEPRI;
    volatile unsigned char RSVD3[120];
    volatile unsigned int EMR;
    volatile unsigned int EMRH;
    volatile unsigned int EMCR;
    volatile unsigned int EMCRH;
    volatile unsigned int QEMR;
    volatile unsigned int QEMCR;
    volatile unsigned int CCERR;
    volatile unsigned int CCERRCLR;
    volatile unsigned int EEVAL;
    volatile unsigned char RSVD4[28];
    IRES_EDMA3CHAN_EDMA3DraeRegister DRA[8];
    volatile unsigned int QRAE[8];
    volatile unsigned char RSVD5[96];
    volatile unsigned int QUEEVTENTRY[8][16];
    volatile unsigned int QSTAT[8];
    volatile unsigned int QWMTHRA;
    volatile unsigned int QWMTHRB;
    volatile unsigned char RSVD6[24];
    volatile unsigned int CCSTAT;
    volatile unsigned char RSVD7[188];
    volatile unsigned int AETCTL;
    volatile unsigned int AETSTAT;
    volatile unsigned int AETCMD;
    volatile unsigned char RSVD8[244];
    volatile unsigned int MPFAR;
    volatile unsigned int MPFSR;
    volatile unsigned int MPFCR;
    volatile unsigned int MPPAG;
    volatile unsigned int MPPA[8];
    volatile unsigned char RSVD9[2000];
    volatile unsigned int ER;
    volatile unsigned int ERH;
    volatile unsigned int ECR;
    volatile unsigned int ECRH;
    volatile unsigned int ESR;
    volatile unsigned int ESRH;
    volatile unsigned int CER;
    volatile unsigned int CERH;
    volatile unsigned int EER;
    volatile unsigned int EERH;
    volatile unsigned int EECR;
    volatile unsigned int EECRH;
    volatile unsigned int EESR;
    volatile unsigned int EESRH;
    volatile unsigned int SER;
    volatile unsigned int SERH;
    volatile unsigned int SECR;
    volatile unsigned int SECRH;
    volatile unsigned char RSVD10[8];
    volatile unsigned int IER;
    volatile unsigned int IERH;
    volatile unsigned int IECR;
    volatile unsigned int IECRH;
    volatile unsigned int IESR;
    volatile unsigned int IESRH;
    volatile unsigned int IPR;
    volatile unsigned int IPRH;
    volatile unsigned int ICR;
    volatile unsigned int ICRH;
    volatile unsigned int IEVAL;
    volatile unsigned char RSVD11[4];
    volatile unsigned int QER;
    volatile unsigned int QEER;
    volatile unsigned int QEECR;
    volatile unsigned int QEESR;
    volatile unsigned int QSER;
    volatile unsigned int QSECR;
    volatile unsigned char RSVD12[3944];
    IRES_EDMA3CHAN_EDMA3ShadowRegister SHADOW[8];
    volatile unsigned char RSVD13[4096];
    IRES_EDMA3CHAN_PaRamStruct PARAMENTRY[512];
} IRES_EDMA3CHAN_EDMA3RegisterLayer;

/**
 * @brief       Static Resource Properties 
 */
typedef struct IRES_EDMA3CHAN_Properties {

    unsigned int        numDmaChannels;         /**< Total number of DMA Channel
                                                     supported by the EDMA3 */ 
    unsigned int        numQdmaChannels;        /**< Total number of QDMA 
                                                    Channels supported by the 
                                                    EDMA3 Controller */
    unsigned int        numTccs;                /**< Total number of Tccs
                                                     supported by the EDMA3 */ 
    unsigned int        numPaRAMSets;           /**< Total number of PaRAM Sets                                                      supported by the EDMA3 */
    unsigned int        numEvtQueue;            /**< Total number of Event 
                                                     Queues in the EDMA3 */ 
    unsigned int        numTcs;                 /**< Total number of Transfer 
                                                     Controllers (TCs) in the 
                                                     EDMA3 Controller */
    unsigned int        numRegions;             /**< Number of Regions on the 
                                                     EDMA3 controller */

    /**
     * @brief  Channel mapping existence
     *
     * A value of 0 (No channel mapping) implies that there is fixed association
     * for a channel number to a parameter entry number or, in other words,
     * PaRAM entry n corresponds to channel n.
     */
    unsigned short dmaChPaRAMMapExists;

    unsigned short memProtectionExists;         /**< Existence of memory 
                                                     protection feature */

    IRES_EDMA3CHAN_EDMA3RegisterLayer *globalRegs; 
                                                /**< Pointer to Global Register                                                      Region of CC Registers */

} IRES_EDMA3CHAN_Properties;

#ifdef __cplusplus
}
#endif /* extern "C" */

/*@}*/

#endif
/*
 *  @(#) ti.sdo.fc.ires.edma3chan; 1, 0, 0,48; 5-15-2008 20:30:21; /db/atree/library/trees/fc-e19x/src/
 */

