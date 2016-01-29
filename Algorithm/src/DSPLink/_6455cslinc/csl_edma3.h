/*  ============================================================================
 *   Copyright (c) Texas Instruments Inc 2002, 2003, 2004, 2005, 2006
 *
 *   Use of this software is controlled by the terms and conditions found in the
 *   license agreement under which this software has been supplied.
 *   ===========================================================================
 */
 
/** ============================================================================
 *   @file  csl_edma3.h
 *
 *   @path  $(CSLPATH)\inc
 *
 *   @desc  Header file for functional layer CSL of EDMA
 *
 */
 
/** @mainpage EDMA3
 *
 * @section Introduction
 *
 * @subsection xxx Purpose and Scope
 * The purpose of this document is to detail the  CSL APIs for the EDMA3 
 * Module. The CSL developer is expected to refer to this document  
 * while designing APIs for the modules which use Edma module. Some of the
 * listed APIs may not be applicable to a given module. While other cases
 * this list of APIs may not be sufficient to cover all the features required
 * for a particular Module. The CSL developer should use his discretion 
 * designing new APIs or extending the existing ones to cover these.
 *
 * @subsection aaa Terms and Abbreviations
 *   -# CSL:  Chip Support Library
 *   -# API:  Application Programmer Interface
 *   -# EDMA: Enhanced Direct Memory Access 
 *
 * @subsection References
 *   -# CSL 3.x Technical Requirements Specifications Version 0.5, dated
 *      May 14th, 2003
 *   -# EDMA Channel Controller Specification (Revision 3.0.2)
 *   -# EDMA Transfer Controller Specification (Revision 3.0.1)
 *
 * @subsection Assumptions
 *    The abbreviations EDMA, edma and Edma have been used throughout this
 *    document to refer to Enhanced Direct Memory Access.
 */

/* =============================================================================
 *  Revision History
 *  ===============
 *  29-May-2004 Ruchika Kharwar File Created.
 *
 * =============================================================================
 */
 
#ifndef _CSL_EDMA3_H_
#define _CSL_EDMA3_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <csl.h>
#include <soc.h>
#include <cslr_edma3cc.h>

/* EDMA Symbols Defined */

/** Link to a Null Param set */
#define CSL_EDMA3_LINK_NULL                     0xFFFF 
/** Link to a Null Param set */
#define CSL_EDMA3_LINK_DEFAULT                  0xFFFF
/** A synchronized transfer  */
#define CSL_EDMA3_SYNC_A                        0
/** AB synchronized transfer */
#define CSL_EDMA3_SYNC_AB                       1
/** Normal Completion */
#define CSL_EDMA3_TCC_NORMAL                    0
/** Early  Completion */
#define CSL_EDMA3_TCC_EARLY                     1
/** Only for ease  */
#define CSL_EDMA3_FIFOWIDTH_NONE                0
/** 8 bit FIFO Width */
#define CSL_EDMA3_FIFOWIDTH_8BIT                0    
/** 16 bit FIFO Width */
#define CSL_EDMA3_FIFOWIDTH_16BIT               1    
/** 32 bit FIFO Width */
#define CSL_EDMA3_FIFOWIDTH_32BIT               2    
/** 64 bit FIFO Width */
#define CSL_EDMA3_FIFOWIDTH_64BIT               3    
/** 128 bit FIFO Width */
#define CSL_EDMA3_FIFOWIDTH_128BIT              4   
/** 256 bit FIFO Width */
#define CSL_EDMA3_FIFOWIDTH_256BIT              5    
/** Address Mode is incremental */
#define CSL_EDMA3_ADDRMODE_INCR                 0
/** Address Mode is such it wraps around after reaching FIFO width */
#define CSL_EDMA3_ADDRMODE_FIFO                 1



/* Bitwise OR of the below symbols are used for setting the Memory attributes 
   These are defined only if the Memory Protection feature exists */
#if CSL_EDMA3_MEMPROTECT
/** User Execute permission */
#define CSL_EDMA3_MEMACCESS_UX              0x0001  
/** User Write permission */  
#define CSL_EDMA3_MEMACCESS_UW              0x0002  
/** User Read permission */
#define CSL_EDMA3_MEMACCESS_UR              0x0004  
/** Supervisor Execute permission */  
#define CSL_EDMA3_MEMACCESS_SX              0x0008  
/** Supervisor Write permission */
#define CSL_EDMA3_MEMACCESS_SW              0x0010  
/** Supervisor Read permission */
#define CSL_EDMA3_MEMACCESS_SR              0x0020  
/** External Allowed ID. Requests with PrivID >= '6' are permitted 
 * if access type is allowed
 */
#define CSL_EDMA3_MEMACCESS_EXT            0x0200 
/** Allowed ID '0' */ 
#define CSL_EDMA3_MEMACCESS_AID0           0x0400  
/** Allowed ID '1' */
#define CSL_EDMA3_MEMACCESS_AID1           0x0800  
/** Allowed ID '2' */
#define CSL_EDMA3_MEMACCESS_AID2           0x1000  
/** Allowed ID '3' */
#define CSL_EDMA3_MEMACCESS_AID3           0x2000  
/** Allowed ID '4' */
#define CSL_EDMA3_MEMACCESS_AID4           0x4000  
/** Allowed ID '5' */
#define CSL_EDMA3_MEMACCESS_AID5           0x8000  
#endif

/** Intermediate transfer completion interrupt enable */  
#define CSL_EDMA3_ITCINT_EN             1
/** Intermediate transfer completion interrupt disable */ 
#define CSL_EDMA3_ITCINT_DIS            0
/** Intermediate transfer completion chaining enable */ 
#define CSL_EDMA3_ITCCH_EN              1
/** Intermediate transfer completion chaining disable */    
#define CSL_EDMA3_ITCCH_DIS             0
/** Transfer completion interrupt enable */  
#define CSL_EDMA3_TCINT_EN              1
/** Transfer completion interrupt disable */ 
#define CSL_EDMA3_TCINT_DIS             0
/** Transfer completion chaining enable */   
#define CSL_EDMA3_TCCH_EN               1
/** Transfer completion chaining disable */
#define CSL_EDMA3_TCCH_DIS              0
/** Enable Static */
#define CSL_EDMA3_STATIC_EN             1
/** Disable Static */
#define CSL_EDMA3_STATIC_DIS            0
/** Last trigger word in a QDMA parameter set */
#define CSL_EDMA3_TRIGWORD_DEFAULT      7   
    

/** Used for creating the options entry in the parameter ram */
#define CSL_EDMA3_OPT_MAKE(itcchEn, tcchEn, itcintEn, tcintEn, tcc, tccMode, \
                           fwid, stat, syncDim, dam, sam) \
(Uint32)(\
     CSL_FMKR(23,23,itcchEn) \
    |CSL_FMKR(22,22,tcchEn) \
    |CSL_FMKR(21,21,itcintEn) \
    |CSL_FMKR(20,20,tcintEn) \
    |CSL_FMKR(17,12,tcc) \
    |CSL_FMKR(11,11,tccMode) \
    |CSL_FMKR(10,8,fwid) \
    |CSL_FMKR(3,3,stat) \
    |CSL_FMKR(2,2,syncDim) \
    |CSL_FMKR(1,1,dam) \
    |CSL_FMKR(0,0,sam)) 
    
/** Used for creating the A,B Count entry in the parameter ram */
#define CSL_EDMA3_CNT_MAKE(aCnt,bCnt) \
(Uint32)(\
     CSL_FMK(EDMA3CC_A_B_CNT_ACNT,aCnt) \
    |CSL_FMK(EDMA3CC_A_B_CNT_BCNT,bCnt)\
    )

/** Used for creating the link and B count reload entry in the parameter ram */
#define CSL_EDMA3_LINKBCNTRLD_MAKE(link,bCntRld) \
(Uint32)(\
     CSL_FMK(EDMA3CC_LINK_BCNTRLD_LINK,(Uint32)link) \
    |CSL_FMK(EDMA3CC_LINK_BCNTRLD_BCNTRLD,bCntRld)\
    )

/** Used for creating the B index entry in the parameter ram */
#define CSL_EDMA3_BIDX_MAKE(src,dst) \
(Uint32)(\
     CSL_FMK(EDMA3CC_SRC_DST_BIDX_DSTBIDX,(Uint32)dst) \
    |CSL_FMK(EDMA3CC_SRC_DST_BIDX_SRCBIDX,(Uint32)src)\
    )

/** Used for creating the C index entry in the parameter ram */
#define CSL_EDMA3_CIDX_MAKE(src,dst) \
(Uint32)(\
     CSL_FMK(EDMA3CC_SRC_DST_CIDX_DSTCIDX,(Uint32)dst) \
    |CSL_FMK(EDMA3CC_SRC_DST_CIDX_SRCCIDX,(Uint32)src)\
    )

/** DMA Channel Setup  */
#define CSL_EDMA3_DMACHANNELSETUP_DEFAULT   {       \
   {CSL_EDMA3_QUE_0,0}, \
   {CSL_EDMA3_QUE_0,1}, \
   {CSL_EDMA3_QUE_0,2}, \
   {CSL_EDMA3_QUE_0,3}, \
   {CSL_EDMA3_QUE_0,4}, \
   {CSL_EDMA3_QUE_0,5}, \
   {CSL_EDMA3_QUE_0,6}, \
   {CSL_EDMA3_QUE_0,7}, \
   {CSL_EDMA3_QUE_0,8}, \
   {CSL_EDMA3_QUE_0,9}, \
   {CSL_EDMA3_QUE_0,10}, \
   {CSL_EDMA3_QUE_0,11}, \
   {CSL_EDMA3_QUE_0,12}, \
   {CSL_EDMA3_QUE_0,13}, \
   {CSL_EDMA3_QUE_0,14}, \
   {CSL_EDMA3_QUE_0,15}, \
   {CSL_EDMA3_QUE_0,16}, \
   {CSL_EDMA3_QUE_0,17}, \
   {CSL_EDMA3_QUE_0,18}, \
   {CSL_EDMA3_QUE_0,19}, \
   {CSL_EDMA3_QUE_0,20}, \
   {CSL_EDMA3_QUE_0,21}, \
   {CSL_EDMA3_QUE_0,22}, \
   {CSL_EDMA3_QUE_0,23}, \
   {CSL_EDMA3_QUE_0,24}, \
   {CSL_EDMA3_QUE_0,25}, \
   {CSL_EDMA3_QUE_0,26}, \
   {CSL_EDMA3_QUE_0,27}, \
   {CSL_EDMA3_QUE_0,28}, \
   {CSL_EDMA3_QUE_0,29}, \
   {CSL_EDMA3_QUE_0,30}, \
   {CSL_EDMA3_QUE_0,31}, \
   {CSL_EDMA3_QUE_0,32}, \
   {CSL_EDMA3_QUE_0,33}, \
   {CSL_EDMA3_QUE_0,34}, \
   {CSL_EDMA3_QUE_0,35}, \
   {CSL_EDMA3_QUE_0,36}, \
   {CSL_EDMA3_QUE_0,37}, \
   {CSL_EDMA3_QUE_0,38}, \
   {CSL_EDMA3_QUE_0,39}, \
   {CSL_EDMA3_QUE_0,40}, \
   {CSL_EDMA3_QUE_0,41}, \
   {CSL_EDMA3_QUE_0,42}, \
   {CSL_EDMA3_QUE_0,43}, \
   {CSL_EDMA3_QUE_0,44}, \
   {CSL_EDMA3_QUE_0,45}, \
   {CSL_EDMA3_QUE_0,46}, \
   {CSL_EDMA3_QUE_0,47}, \
   {CSL_EDMA3_QUE_0,48}, \
   {CSL_EDMA3_QUE_0,49}, \
   {CSL_EDMA3_QUE_0,50}, \
   {CSL_EDMA3_QUE_0,51}, \
   {CSL_EDMA3_QUE_0,52}, \
   {CSL_EDMA3_QUE_0,53}, \
   {CSL_EDMA3_QUE_0,54}, \
   {CSL_EDMA3_QUE_0,55}, \
   {CSL_EDMA3_QUE_0,56}, \
   {CSL_EDMA3_QUE_0,57}, \
   {CSL_EDMA3_QUE_0,58}, \
   {CSL_EDMA3_QUE_0,59}, \
   {CSL_EDMA3_QUE_0,60}, \
   {CSL_EDMA3_QUE_0,61}, \
   {CSL_EDMA3_QUE_0,62}, \
   {CSL_EDMA3_QUE_0,63} \
}

/** QDMA Channel Setup  */
#define CSL_EDMA3_QDMACHANNELSETUP_DEFAULT   {       \
   {CSL_EDMA3_QUE_0,64,CSL_EDMA3_TRIGWORD_DEFAULT}, \
   {CSL_EDMA3_QUE_0,65,CSL_EDMA3_TRIGWORD_DEFAULT}, \
   {CSL_EDMA3_QUE_0,66,CSL_EDMA3_TRIGWORD_DEFAULT}, \
   {CSL_EDMA3_QUE_0,67,CSL_EDMA3_TRIGWORD_DEFAULT}  \
}

/** @brief Enumeration for System priorities
 *
 * This is used for Setting up the Que Priority level
 */
typedef enum {
    /** System priority level 0 */
    CSL_EDMA3_QUE_PRI_0 = 0,
    /** System priority level 1 */
    CSL_EDMA3_QUE_PRI_1 = 1,
    /** System priority level 2 */         
    CSL_EDMA3_QUE_PRI_2 = 2,
    /** System priority level 3 */
    CSL_EDMA3_QUE_PRI_3 = 3,
    /** System priority level 4 */     
    CSL_EDMA3_QUE_PRI_4 = 4,   
    /** System priority level 5 */
    CSL_EDMA3_QUE_PRI_5 = 5,   
    /** System priority level 6 */
    CSL_EDMA3_QUE_PRI_6 = 6,   
    /** System priority level 7 */
    CSL_EDMA3_QUE_PRI_7 = 7    
}CSL_Edma3QuePri;

/** @brief Enumeration for EDMA Que Thresholds
 *
 * This is used for Setting up the Que thresholds
 */
typedef enum {
    /** EDMA Que Threshold 0 */
    CSL_EDMA3_QUE_THR_0 = 0,
    /** EDMA Que Threshold 1 */        
    CSL_EDMA3_QUE_THR_1 = 1,
    /** EDMA Que Threshold 2 */        
    CSL_EDMA3_QUE_THR_2 = 2,
    /** EDMA Que Threshold 3 */        
    CSL_EDMA3_QUE_THR_3 = 3, 
    /** EDMA Que Threshold 4 */       
    CSL_EDMA3_QUE_THR_4 = 4,
    /** EDMA Que Threshold 5 */
    CSL_EDMA3_QUE_THR_5 = 5,
    /** EDMA Que Threshold 6 */   
    CSL_EDMA3_QUE_THR_6 = 6, 
    /** EDMA Que Threshold 7 */
    CSL_EDMA3_QUE_THR_7 = 7,
    /** EDMA Que Threshold 8 */
    CSL_EDMA3_QUE_THR_8 = 8,
    /** EDMA Que Threshold 9 */
    CSL_EDMA3_QUE_THR_9 = 9,
    /** EDMA Que Threshold 10 */
    CSL_EDMA3_QUE_THR_10 = 10,
    /** EDMA Que Threshold 11 */
    CSL_EDMA3_QUE_THR_11 = 11,
    /** EDMA Que Threshold 12 */
    CSL_EDMA3_QUE_THR_12 = 12, 
    /** EDMA Que Threshold 13 */
    CSL_EDMA3_QUE_THR_13 = 13,
    /** EDMA Que Threshold 14 */
    CSL_EDMA3_QUE_THR_14 = 14,
    /** EDMA Que Threshold 15 */
    CSL_EDMA3_QUE_THR_15 = 15, 
    /** EDMA Que Threshold 16 */
    CSL_EDMA3_QUE_THR_16 = 16, 
    /* EDMA Que Threshold Disable Errors */
    CSL_EDMA3_QUE_THR_DISABLE = 17  
}CSL_Edma3QueThr;

/** MODULE Level Commands */
typedef enum {
#if CSL_EDMA3_MEMPROTECT
        /**
         * @brief   Programmation of MPPAG,MPPA[0-7] attributes
         *
         * @param   (CSL_Edma3CmdRegion *)
         */
    CSL_EDMA3_CMD_MEMPROTECT_SET,
        /**
         * @brief   Clear Memory Fault
         *
         * @param   (None)
         */
    CSL_EDMA3_CMD_MEMFAULT_CLEAR,
#endif  
        /**
         * @brief   Enables bits as specified in the argument passed in 
         *          DRAE/DRAEH. Please note:If bits are already set in 
         *          DRAE/DRAEH this Control command will cause additional bits
         *          (as specified by the bitmask) to be set and does   
         * @param   (CSL_Edma3CmdDrae *)
         */
    CSL_EDMA3_CMD_DMAREGION_ENABLE,
        /**
         * @brief   Disables bits as specified in the argument passed in 
         *          DRAE/DRAEH   
         * @param   (CSL_Edma3CmdDrae *)
         */     
    CSL_EDMA3_CMD_DMAREGION_DISABLE,
        /**
         * @brief   Enables bits as specified in the argument 
         *          passed in QRAE.Pleasenote:If bits are already set in 
         *          QRAE/QRAEH this Control command will cause additional bits 
         *          (as specified by the bitmask) to be set and does  
         * @param   (CSL_Edma3CmdQrae *)
         */
    CSL_EDMA3_CMD_QDMAREGION_ENABLE,
        /**
         * @brief   Disables bits as specified in the argument passed in QRAE
         *          DRAE/DRAEH   
         * @param   (CSL_Edma3CmdQrae *)
         */    
    CSL_EDMA3_CMD_QDMAREGION_DISABLE,
        /**
         * @brief   Programmation of QUEPRI register with the specified priority
         *          DRAE/DRAEH   
         * @param   (CSL_Edma3CmdQuePri *)
         */ 
    CSL_EDMA3_CMD_QUEPRIORITY_SET,
        /**
         * @brief   Programmation of QUE Threshold levels
         *
         * @param   (CSL_Edma3CmdQueThr *)
         */      
    CSL_EDMA3_CMD_QUETHRESHOLD_SET,
        /**
         * @brief   Sets the EVAL bit in the EEVAL register
         *
         * @param   (None)
         */
    CSL_EDMA3_CMD_ERROR_EVAL,
        /**
         * @brief   Clears specified (Bitmask)pending interrupt at Module/Region
         *          Level
         * @param   (CSL_Edma3CmdIntr *)
         */         
    CSL_EDMA3_CMD_INTRPEND_CLEAR,
        /**
         * @brief   Enables specified interrupts(BitMask) at Module/Region Level
         *          
         * @param   (CSL_Edma3CmdIntr *)
         */  
    CSL_EDMA3_CMD_INTR_ENABLE, 
        /**
         * @brief   Disables specified interrupts(BitMask) at Module/Region 
         *          Level
         * @param   (CSL_Edma3CmdIntr *)
         */
    CSL_EDMA3_CMD_INTR_DISABLE,
        /**
         * @brief   Interrupt Evaluation asserted for the Module/Region
         *
         * @param   (Int *)
         */    
    CSL_EDMA3_CMD_INTR_EVAL, 
        /**
         * @brief   Clear the EDMA Controller Erorr
         *
         * @param   (CSL_Edma3CtrlErrStat *)
         */
    CSL_EDMA3_CMD_CTRLERROR_CLEAR , 
        /**
         * @brief   Pointer to an array of 3 elements, where element0 refers to 
         *          the EMR register to be cleared, element1 refers to the EMRH 
         *          register to be cleared, element2 refers to the QEMR register
         *          to be cleared.
         * @param   (CSL_BitMask32 *)
         */
    CSL_EDMA3_CMD_EVENTMISSED_CLEAR     
} CSL_Edma3HwControlCmd;

/** @brief MODULE Level Queries */
typedef enum {
#if CSL_EDMA3_MEMPROTECT
        /**
         * @brief   Return the Memory fault details
         *
         * @param   (CSL_Edma3MemFaultStat *)
         */    
    CSL_EDMA3_QUERY_MEMFAULT,
        /**
         * @brief   Return memory attribute of the specified region
         *
         * @param   (CSL_Edma3CmdRegion *)
         */             
    CSL_EDMA3_QUERY_MEMPROTECT,           
#endif
        /**
         * @brief   Return Controller Error
         *
         * @param   (CSL_Edma3CtrlErrStat *)
         */ 
    CSL_EDMA3_QUERY_CTRLERROR,
        /**
         * @brief   Return pend status of specified interrupt
         *
         * @param   (CSL_Edma3CmdIntr *)
         */             
    CSL_EDMA3_QUERY_INTRPEND,
        /**
         * @brief   Returns Miss Status of all Channels
         *          Pointer to an array of 3 elements, where element0 refers to 
         *          the EMR  registr, element1 refers to the EMRH register,  
         *          element2 refers to the QEMR register
         * @param   (CSL_BitMask32 *)
         */             
    CSL_EDMA3_QUERY_EVENTMISSED,
        /**
         * @brief   Returns the Que status
         *
         * @param   (CSL_Edma3QueStat *)
         */          
    CSL_EDMA3_QUERY_QUESTATUS,            
        /**
         * @brief   Returns the Channel Controller Active Status 
         *
         * @param   (CSL_Edma3ActivityStat *)
         */
    CSL_EDMA3_QUERY_ACTIVITY, 
        /**
         * @brief   Returns the Channel Controller Information viz. 
         *          Configuration, Revision Id  
         * @param   (CSL_Edma3QueryInfo *)
         */            
    CSL_EDMA3_QUERY_INFO                 
} CSL_Edma3HwStatusQuery;

/** @brief CHANNEL Commands */
typedef enum {
        /**
         * @brief   Enables specified Channel
         *
         * @param   (None)
         */ 
    CSL_EDMA3_CMD_CHANNEL_ENABLE,
        /**
         * @brief   Disables specified Channel
         *
         * @param   (None)
         */
    CSL_EDMA3_CMD_CHANNEL_DISABLE,
        /**
         * @brief   Manually sets the Channel Event,writes into ESR/ESRH 
         *          and not ER.NA for QDMA
         * @param   (None)
         */       
    CSL_EDMA3_CMD_CHANNEL_SET,
        /**
         * @brief   Manually clears the Channel Event, does not write into
         *          ESR/ESRH or ER/ERH but the ECR/ECRH. NA for QDMA
         * @param   (None)
         */            
    CSL_EDMA3_CMD_CHANNEL_CLEAR,
        /**
         * @brief   In case of DMA channels clears SER/SERH(by writing into 
         *          SECR/SECRH if "secEvt" and "missed" are both TRUE) and 
         *          EMR/EMRH(by writing into EMCR/EMCRH if "missed" is TRUE).
         *          In case of QDMA channels clears QSER(by writing into QSECR 
         *          if "ser" and "missed" are both TRUE) and QEMR(by writing 
         *          into QEMCR if "missed" is TRUE)
         * @param   (CSL_Edma3ChannelErr *)
         */ 
    CSL_EDMA3_CMD_CHANNEL_CLEARERR       
} CSL_Edma3HwChannelControlCmd;

/** @brief CHANNEL Queries */  
typedef enum  {
        /**
         * @brief   In case of DMA channels returns TRUE if ER/ERH is set,
         *          In case of QDMA channels returns TRUE if QER is set
         * @param   (Bool *)
         */ 
    CSL_EDMA3_QUERY_CHANNEL_STATUS,
        /**
         * @brief   In case of DMA channels,'missed' is set
         *          to TRUE if EMR/EMRH is set, 'secEvt' is set to TRUE if 
         *          SER/SERH is set.In case of QDMA channels,'missed' is set to 
         *          TRUE if QEMR is set, 'secEvt' is set to TRUE if QSER is set.
         *          It should be noted that if secEvt ONLY is set to TRUE it 
         *          may not be a valid error condition 
         * @param   (CSL_Edma3ChannelErr *)
         */  
    CSL_EDMA3_QUERY_CHANNEL_ERR      
} CSL_Edma3HwChannelStatusQuery;

/** @brief Module specific context information.
 *  This is a dummy handle. 
 */
typedef void *CSL_Edma3Context;      

/** @brief Module Attributes specific information.
 *  This is a dummy handle. 
 */
typedef void *CSL_Edma3ModuleAttr;

/** @brief This object contains the reference to the instance of Edma Module
 * opened using the @a CSL_edma3Open().
 *
 *  A pointer to this object is passed to all Edma Module level CSL APIs.
 */
 
typedef struct CSL_Edma3Obj {
    /** This is a pointer to the Edma Channel Controller registers of the module
     *   requested
     */
    CSL_Edma3ccRegsOvly regs;
    /** This is the instance of module number i.e CSL_EDMA3 */
    CSL_InstNum         instNum;
} CSL_Edma3Obj;

/** @brief EDMA handle */
typedef struct CSL_Edma3Obj *CSL_Edma3Handle;

/** CSL Parameter Set Handle */
typedef volatile CSL_Edma3ccParamsetRegs *CSL_Edma3ParamHandle;

/** @brief Edma ParamSetup Structure
 *
 *  An object of this type is allocated by the user and
 *  its address is passed as a parameter to the CSL_edma3ParamSetup().
 *  This structure is used to program the Param Set for EDMA/QDMA.
 *  The macros can be used to assign values to the fields of the structure.
 *  The setup structure should be setup using the macros provided OR
 *  as per the bit descriptions in the user guide..
 *  
 */
typedef struct CSL_Edma3ParamSetup {
    /** Options */
    Uint32          option;
    /** Specifies the source address */               
    Uint32          srcAddr;
    /** Lower 16 bits are A Count Upper 16 bits are B Count*/             
    Uint32          aCntbCnt;            
    /** Specifies the destination address */                                          
    Uint32          dstAddr;
    /** Lower 16 bits are source b index Upper 16 bits are
     * destination b index 
     */             
    Uint32          srcDstBidx;          
    /** Lower 16 bits are link of the next param entry Upper 16 bits are 
     * b count reload 
     */
    Uint32          linkBcntrld;         
    /** Lower 16 bits are source c index Upper 16 bits are destination 
     * c index 
     */             
    Uint32          srcDstCidx;          
    /** C count */                                           
    Uint32          cCnt;                
} CSL_Edma3ParamSetup;


/** @brief Edma Object Structure
 *
 *  An object of this type is allocated by the user and
 *  its address is passed as a parameter to the CSL_edma3ChannelOpen()
 *  The CSL_edma3ChannelOpen() updates all the members of the data structure
 *  and returns the objects address as a @a #CSL_Edma3ChannelHandle. The
 *  @a #CSL_Edma3ChannelHandle is used in all subsequent function calls.
 */
 
typedef struct CSL_Edma3ChannelObj {
    /** Pointer to the Edma Channel Controller module register 
     * Overlay structure 
     */
    CSL_Edma3ccRegsOvly      regs;               
    /** Region number to which the channel belongs to */ 
    Int                     region;              
    /** EDMA instance whose channel is being requested */
    Int                     edmaNum;             
    /** Channel Number being requested */
    Int                     chaNum;              
} CSL_Edma3ChannelObj;

/** CSL Channel Handle
 *  All channel level API calls must be made with this handle. 
 */
typedef struct CSL_Edma3ChannelObj *CSL_Edma3ChannelHandle;

#if CSL_EDMA3_MEMPROTECT    
/** @brief Edma Memory Protection Fault Error Status
 *
 *  An object of this type is allocated by the user and
 *  its address is passed as a parameter to the CSL_edma3GetMemoryFaultError()
 *  / CSL_edma3GetHwStatus() with the relevant command. This is relevant only is 
 *  MPEXIST is present for a given device.
 */
typedef struct CSL_Edma3MemFaultStat {
    /** Memory Protection Fault Address */
    Uint32           addr;
    /** Bit Mask of the Errors */               
    CSL_BitMask16    error;
    /** Faulted ID */              
    Uint16           fid;                
} CSL_Edma3MemFaultStat;
#endif
/** @brief Edma Controller Error Status. 
 *
 *  An object of this type is allocated by the user and
 *  its address is passed as a parameter to the CSL_edma3GetControllerError()
 *  /CSL_edma3GetHwStatus().
 */
typedef struct CSL_Edma3CtrlErrStat {
    /** Bit Mask of the Que Threshold Errors */
    CSL_BitMask16    error;
    /** Whether number of permissible outstanding Tcc's is exceeded */
    Bool             exceedTcc;          
                                                  
} CSL_Edma3CtrlErrStat;
/** @brief Edma Controller Information
 *
 *  An object of this type is allocated by the user and
 *  its address is passed as a parameter to the CSL_edma3GetInfo()
 *  /CSL_edma3GetHwStatus().
 */
typedef struct CSL_Edma3QueryInfo{
    /** Revision/Periperhal id of the EDMA3 Channel Controller */
    Uint32 revision;
    /** Channel Controller Configuration obtained from the CCCFG register */
    Uint32 config;                      
} CSL_Edma3QueryInfo;

/** @brief Edma Channel Controller Activity Status
 *
 *  An object of this type is allocated by the user and
 *  its address is passed as a parameter to the CSL_edma3GetActivityStatus()
 *  /CSL_edma3GetHwStatus().
 */
typedef struct CSL_Edma3ActivityStat {
    /** Number of outstanding completion requests */
    Uint16 outstandingTcc;
    /** BitMask of the que active in the Channel Controller */   
    CSL_BitMask16 queActive; 
    /** Indicates if the Channel Controller is active at all */
    Bool active; 
    /** Indicates whether any QDMA events are active */
    Bool qevtActive;
    /** Indicates whether any EDMA events are active */ 
    Bool evtActive;
    /** Indicates whether the TR processing/submission logic is active*/  
    Bool trActive;   
} CSL_Edma3ActivityStat;

/** @brief Edma Controller Que Status. 
 *
 *  An object of this type is allocated by the user and
 *  its address is passed as a parameter to the CSL_edma3GetQueStatus()
 *  /CSL_edma3GetHwStatus().
 */
typedef struct CSL_Edma3QueStat {
    /** Input field: Event Que. This needs to be specified by the user 
     *  before invocation of the above API 
     */
    CSL_Edma3Que que;                        
    /** Output field: The number of valid entries in a queue has exceeded the 
     * threshold specified in QWMTHRA has been exceeded 
     */    
    Bool             exceed;                 
    /** Output field: The most entries that have been in Que since reset/last 
     * time the watermark was cleared
     */
    Uint8            waterMark;              
    /** Output field: Number of valid entries in Que N*/                                                
    Uint8            numVal;                 
    /** Output field: Start pointer/Head of the queue */
    Uint8            startPtr;               
} CSL_Edma3QueStat;

/** @brief Edma Control/Query Command Structure for querying region specific 
 *  attributes. 
 *
 *  An object of this type is allocated by the user and
 *  its address is passed as a parameter to the 
 *   CSL_edma3GetHwStatus/CSL_edma3HwControl with the relevant command.   
 */
typedef struct CSL_Edma3CmdRegion {
    /** Input field:- this field needs to be initialized by the user before
     * issuing the query/command 
     */
    Int              region;                 
    /** Input/Output field:-this needs to be filled by the user in case 
     * of issuing a COMMAND or it will be filled in by the CSL when
     * used with a QUERY 
     */                                                
    CSL_BitMask32    regionVal;                                                            
} CSL_Edma3CmdRegion;

/** @brief Edma Control/Query Command Structure for querying qdma region access  
 *  enable attributes. 
 *
 *  An object of this type is allocated by the user and
 *  its address is passed as a parameter to the 
 *  CSL_edma3GetHwStatus/CSL_edma3HwControl with the relevant command.   
 */


typedef struct CSL_Edma3CmdQrae {
    /** this field needs to be initialized by the user before issuing 
     * the query/command
     */
    Int              region;             
    /** this needs to be filled by the user in case of issuing a 
     * COMMAND or it will be filled in by the CSL when  used with a QUERY 
     */
    CSL_BitMask32    qrae;               
                                                  
                                                 
} CSL_Edma3CmdQrae;

/** @brief Edma Control/Query Control Command structure for issuing commands 
 *  for Interrupt related APIs
 *  An object of this type is allocated by the user and
 *  its address is passed to the Control API. 
 */
typedef struct CSL_Edma3CmdIntr{
    /** Input field:- this field needs to be initialized by the user before
     * issuing the query/command 
     */
    Int              region;             
    /** Input/Output field:- this needs to be filled by the user in case 
     * of issuing a COMMAND or it will be filled in by the CSL when used with 
     * a QUERY 
     */                
    CSL_BitMask32    intr;               
    /** Input/Output:- this needs to be filled by the user in case of issuing a                                               
     * COMMAND or it will be filled in by the CSL when  used with a QUERY 
     */                                             
    CSL_BitMask32    intrh;              
                                                  
                                                  
} CSL_Edma3CmdIntr;
/** @brief Edma Command Structure for setting region specific 
 *  attributes.
 *
 *  An object of this type is allocated by the user and
 *  its address is passed as a parameter to the CSL_edma3GetHwStatus
 *  when 
 */


typedef struct CSL_Edma3CmdDrae {
    /** this field needs to be initialiazed by the user before issuing 
     * the command specifying the region for which attributes need to be set 
     */
    Int   region;                
    /** DRAE Setting for the region  */
    CSL_BitMask32     drae;
    /** DRAEH Setting for the region */
    CSL_BitMask32     draeh;     
} CSL_Edma3CmdDrae;


/** @brief Edma Command Structure used for setting Event Que priority level
 *
 *  An object of this type is allocated by the user and
 *  its address is passed as a parameter to the CSL_edma3HwControl API.
 */


typedef struct CSL_Edma3CmdQuePri {
    /** Specifies the Que that needs a priority change */
    CSL_Edma3Que         que;
    /** Que priority */              
    CSL_Edma3QuePri      pri;              
} CSL_Edma3CmdQuePri;    

/** @brief Edma Command Structure used for setting Event Que threshold level
 *
 *  An object of this type is allocated by the user and
 *  its address is passed as a parameter to the CSL_edma3HwControl API.
 */
typedef struct CSL_Edma3CmdQueThr {
    /** Specifies the Que that needs a change in the threshold setting */
    CSL_Edma3Que         que;
    /** Que threshold setting */              
    CSL_Edma3QueThr      threshold;        
} CSL_Edma3CmdQueThr;    

/** @brief This will have the base-address information for the module
 *  instance
 */
typedef struct {
    /** Base-address of the peripheral registers */
    CSL_Edma3ccRegsOvly regs;                   
} CSL_Edma3ModuleBaseAddress;

/** @brief Edma Channel parameter structure used for opening a channel
 */
typedef struct { 
     /** Region Number */   
    Int regionNum;
    /** Channel number */                             
    Int chaNum;                                 
} CSL_Edma3ChannelAttr;

/** @brief Edma Channel Error . 
 *
 *  An object of this type is allocated by the user and
 *  its address is passed as a parameter to the CSL_edma3GetChannelError()
 *  /CSL_edma3GetHwStatus()/ CSL_edma3ChannelErrorClear()
 *  /CSL_edma3HwChannelControl().
 */
typedef struct CSL_Edma3ChannelErr {
    /** a TRUE indicates an event is missed on this channel.  */
    Bool    missed;      
    /** a TRUE indicates an event that no events on this channel will be
     * prioritized till this is cleared. This being TRUE does NOT necessarily
     * mean it is an error. ONLY if both missed and ser are set, this kind of 
     * error need  to be cleared.
     */
    Bool    secEvt;      
} CSL_Edma3ChannelErr;

/** @brief QDMA Edma Channel Setup
 *
 *  An array of such objects are allocated by the user and
 *  address initialized in the CSL_Edma3HwSetup structure which is passed
 *  CSL_edma3HwSetup()
 */

typedef struct CSL_Edma3HwQdmaChannelSetup {
    /** Que number for the channel */
    CSL_Edma3Que que;                            
    /** Parameter set mapping for the channel. */             
    Uint16  paramNum;                            
    /** Trigger word for the QDMA channels. */
    Uint8   triggerWord;                         
} CSL_Edma3HwQdmaChannelSetup;

/** @brief QDMA Edma Channel Setup
 *
 *  An array of such objects are allocated by the user and
 *  address initialized in the CSL_Edma3HwSetup structure which is passed
 *  CSL_edma3HwSetup()
 */

typedef struct CSL_Edma3HwDmaChannelSetup {
    /** Que number for the channel */
    CSL_Edma3Que que;                     
#ifdef CSL_EDMA3_CHMAPEXIST
    /** Parameter set mapping for the channel. This may not be initialized 
     * for Edma channels on devices that do not have CHMAPEXIST.  
     */
    Uint16  paramNum;                     
#endif
} CSL_Edma3HwDmaChannelSetup;

/** @brief Edma Hw Setup Structure 
 */
typedef struct {
    /** Edma Hw Channel setup */
    CSL_Edma3HwDmaChannelSetup *dmaChaSetup;
    /** QEdma Hw Channel setup */
    CSL_Edma3HwQdmaChannelSetup *qdmaChaSetup;
} CSL_Edma3HwSetup;

/**************************************************************************\
* EDMA global function declarations
\**************************************************************************/

/*
 * =============================================================================
 *   @func CSL_edma3Init
 *
 *   @desc
 *     This is EDMA module's init function. This initializes the context Object
 *     variables.Needs to be invoked before using EDMA module.
 *
 *   @arg pContext
 *        Context information for edma
 *
 *   @ret CSL_Status
 *        CSL_SOK - Always returns 
 *
 *   @eg
 *     CSL_edma3Init (NULL);
 * =============================================================================
 */
extern CSL_Status CSL_edma3Init (
    CSL_Edma3Context    *pContext
);

/*
 *============================================================================
 *   @func CSL_edma3Open
 *
 *   @desc
 *      Returns a handle to the EDMA Module. This handle is further used
 *      for invoking all module level control APIs.This call enables 
 *      the relocatability of code since the handle  can be subsequently used for
 *      Module level control commands rather than  the usage of  of the register
 *      layer base address directly.
 *
 *   @arg   edmaObj
 *          Pointer to the object to which a handle is returned.
 *
 *   @arg   edmaNum
 *          Instance Number of the EDMA
 *        
 *   @arg   param
 *          Pointer to module specific parameters
 *
 *   @arg   status
 *          pointer for returning status of the function call
 *
 *   @ret   CSL_Edma3Handle
 *          Valid edma handle will be returned if status value is equal to 
 *          CSL_SOK
 *
 *   @eg
 *          CSL_Edma3Handle     hModule;
            CSL_Edma3Obj        edmaObj;
            CSL_Edma3Context    context;
            CSL_Status          status;
             
            // Module Initialization
            CSL_edma3Init(&context);
             
            // Module Level Open    
            hModule = CSL_edma3Open(&edmaObj,CSL_EDMA3,NULL,&status);
 *
 * ===========================================================================
 */
extern CSL_Edma3Handle  CSL_edma3Open (
    CSL_Edma3Obj             *edmaObj,
    CSL_InstNum              edmaNum,
    CSL_Edma3ModuleAttr      *attr,
    CSL_Status               *status
);

/*
 *============================================================================
 *   @func CSL_edma3Close
 *
 *   @desc
 *      The EDMA module handle is invalidated and in order to use the EDMA module
 *      it needs to be opened again.
 *
 *   @arg   hEdma
 *          Handle to the EDMA module
 *
 *   @ret   CSL_Status
 *          CSL_SOK            - Close successful
 *          CSL_ESYS_BADHANDLE - Invalid handle
 *
 *   @eg
 *          CSL_Edma3Handle     hModule;
            CSL_Edma3Obj        edmaObj;
            CSL_Edma3Context    context;
            CSL_Status          status;
             
            // Module Initialization
            CSL_edma3Init(&context);
             
            // Module Level Open    
            hModule = CSL_edma3Open(&edmaObj,CSL_EDMA3,NULL,&status);
            
            // Close Module
            CSL_edma3Close(hModule);
 *
 * ===========================================================================
 */
extern CSL_Status CSL_edma3Close (
    CSL_Edma3Handle       hEdma
);

/*
 * =============================================================================
 *   @func CSL_edma3HwSetup
 *
 *   @desc
 *      Does a module level HW setup of EDMA. This programs the 
 *      Channel to Param mapping, Channel to Que map and the trigger 
 *      word (if applicable) of ALL channels (DMA, QDMA).
 *
 *   @arg   hMod
 *          Edma Module Handle 
 *
 *   @arg   setup
 *          Pointer to hardware setup structure containing the setup parameters 
 *          of all DMA, QDMA channels.
 *
 *   @ret   CSL_Status
 *          CSL_SOK             - Hardware setup successful
 *          CSL_ESYS_BADHANDLE  - Invalid handle
 *          CSL_ESYS_INVPARAMS  - Hardware structure is not
 *                              properly initialized
 *
 *   @eg
 *          CSL_Edma3Handle     hModule;
            CSL_Edma3Obj        edmaObj;
            CSL_Edma3Context    context;
            CSL_Edma3HwSetup    hwSetup;
            CSL_Status          status;
            CSL_Edma3HwDmaChannelSetup  dmahwSetup[CSL_EDMA3_NUM_DMACH] = 
                                              CSL_EDMA3_DMACHANNELSETUP_DEFAULT;
            CSL_Edma3HwQdmaChannelSetup qdmahwSetup[CSL_EDMA3_NUM_QDMACH] = 
                                              CSL_EDMA3_QDMACHANNELSETUP_DEFAULT; 
            // Module Initialization
            CSL_edma3Init(&context);
             
            // Module Level Open    
            hModule = CSL_edma3Open(&edmaObj,CSL_EDMA3,NULL,&status);
            
            // Module Setup
            hwSetup.dmaChaSetup  = &dmahwSetup[0];
            hwSetup.qdmaChaSetup = &qdmaSetup[0];
            CSL_edma3HwSetup(hModule,&hwSetup);
            
            ...
 *
 * ===========================================================================
 */
extern CSL_Status  CSL_edma3HwSetup (
    CSL_Edma3Handle           hMod,
    CSL_Edma3HwSetup          *setup
);

/*
 * =============================================================================
 *   @func CSL_edma3GetHwSetup
 *
 *   @desc
 *      Obtains the module level HW setup of EDMA. This reads the 
 *      Channel to Param mapping.Channel to Que map and the trigger word 
 *      (if applicable) of ALL channels (DMA, QDMA) into the placeholder 
 *      provided by the user.
 *
 *   @arg   hMod
 *          Edma Module Handle 
 *
 *   @arg   setup
 *          Pointer to the Module Setup structure
 *
 *   @ret   CSL_Status
 *          CSL_SOK             - Hardware setup successful
 *          CSL_ESYS_BADHANDLE  - Invalid handle
 *          CSL_ESYS_INVPARAMS  - Hardware structure is not
 *                              properly initialized
 *
 *   @eg
 *          CSL_Edma3Handle     hModule;
            CSL_Edma3Obj        edmaObj;
            CSL_Edma3Context    context;
            CSL_Status          status;
            CSL_Edma3HwSetup    hwSetup, gethwSetup;
            CSL_Edma3HwDmaChannelSetup  dmahwSetup[CSL_EDMA3_NUM_DMACH] = 
                                            CSL_EDMA3_DMACHANNELSETUP_DEFAULT;
            CSL_Edma3HwQdmaChannelSetup qdmahwSetup[CSL_EDMA3_NUM_QDMACH] = 
                                            CSL_EDMA3_QDMACHANNELSETUP_DEFAULT; 
            // Module Initialization
            CSL_edma3Init(&context);
             
            // Module Level Open    
            hModule = CSL_edma3Open(&edmaObj,CSL_EDMA3,NULL,&status);
            
            // Module Setup
            hwSetup.dmaChaSetup  = &dmahwSetup[0];
            hwSetup.qdmaChaSetup = &qdmaSetup[0];
            CSL_edma3HwSetup(hModule,&hwSetup);
            
            // Get Module Setup
            gethwSetup.dmaChaSetup  = &getdmahwSetup[0];
            gethwSetup.qdmaChaSetup = NULL;
            CSL_edma3GetHwSetup(hModule,&gethwSetup);
             
            ...
 *
 * ===========================================================================
 */
extern CSL_Status  CSL_edma3GetHwSetup (
    CSL_Edma3Handle      hMod,
    CSL_Edma3HwSetup    *setup
);

/*
 * =============================================================================
 *   @func CSL_edma3HwControl
 *
 *   @desc
 *     Module level control commands are handled by this API.
 *
 *   @arg   hMod
 *          Edma Module Handle 
 *
 *   @arg   cmd
 *          Module Command
 *
 *   @arg   cmdArg
 *          Additional command arguments are passed to the API using this. 
 *          The CSL function type casts to the appropriate arguments type 
 *          depending on the cmd. 
 *
 *   @ret   CSL_Status
 *          CSL_SOK            - Command execution successful.
 *          CSL_ESYS_BADHANDLE - Invalid handle
 *          CSL_ESYS_INVCMD    - Invalid command
 *
 *   @eg
 *          CSL_Edma3Handle     hModule;
            CSL_Edma3Obj        edmaObj;
            CSL_Edma3Context    context;
            CSL_Status          status;
            CSL_Edma3HwSetup    hwSetup, gethwSetup;
            CSL_Edma3CmdDrae    regionAccess;
            CSL_Edma3HwDmaChannelSetup  dmahwSetup[CSL_EDMA3_NUM_DMACH] = 
                                             CSL_EDMA3_DMACHANNELSETUP_DEFAULT;
            CSL_Edma3HwQdmaChannelSetup qdmahwSetup[CSL_EDMA3_NUM_QDMACH] = 
                                             CSL_EDMA3_QDMACHANNELSETUP_DEFAULT;
            // Module Initialization
            CSL_edma3Init(&context);
             
            // Module Level Open    
            hModule = CSL_edma3Open(&edmaObj,CSL_EDMA3,NULL,&status);
            
            // Module Setup
            hwSetup.dmaChaSetup  = &dmahwSetup[0];
            hwSetup.qdmaChaSetup = &qdmaSetup[0];
            CSL_edma3HwSetup(hModule,&hwSetup);
            
            // DRAE Enable(Bits 0-15) for the Shadow Region 0.          
            regionAccess.region = CSL_EDMA3_REGION_0 ;                  
            regionAccess.drae =   0xFFFF ;                              
            regionAccess.draeh =  0x0000 ;                              
            CSL_edma3HwControl(hModule,CSL_EDMA3_CMD_DMAREGION_ENABLE, \ 
                              &regionAccess);                           
            ...
 *
 * ===========================================================================
 */
extern CSL_Status  CSL_edma3HwControl (
    CSL_Edma3Handle           hMod,
    CSL_Edma3HwControlCmd     cmd,
    void                      *cmdArg
);

/** ============================================================================
 *   @n@b CSL_edma3ccGetModuleBaseAddr
 *
 *   @b Description
 *   @n  This function is used for getting the base-address of the peripheral
 *       instance. This function will be called inside the @ CSL_edma3Open()
 *       /CSL_edma3ChannelOpen() function call. 
 *
 *       Note: This function is open for re-implementing if the user wants to 
 *       modify the base address of the peripheral object to point to a different
 *       location and there by allow CSL initiated write/reads into peripheral
 *       MMR's go to an alternate location.
 *
 *   @b Arguments
 *   @verbatim      
            edmaNum         Specifies the instance of the edma to be opened.
 
            pAttr           Module specific parameters.
 
            pBaseAddress    Pointer to baseaddress structure containing base 
                            address details.
            
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK           Open call is successful
 *   @li                    CSL_ESYS_FAIL     The instance number is invalid.
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  Base Address structure is populated
 *
 *   @b Modifies
 *   @li    The status variable
 *   @li    Base address structure is modified.
 *
 *   @b Example
 *   @verbatim
        CSL_Status                   status;
        CSL_Edma3ModuleBaseAddress   baseAddress;

        ...
        status = CSL_edma3ccGetModuleBaseAddr(CSL_EDMA3, NULL, &baseAddress);

    @endverbatim
 * ===========================================================================
 */
extern CSL_Status CSL_edma3ccGetModuleBaseAddr (
    CSL_InstNum                 edmaNum,
    CSL_Edma3ModuleAttr         *pAttr,
    CSL_Edma3ModuleBaseAddress  *pBaseAddress
);

/*
 * =============================================================================
 *   @n@b CSL_edma3GetHwStatus
 *
 *   @desc
 *      Module level queries to the EDMA module are handled through this API
 *     
 *   @arg   hMod
 *          Edma Module Handle
 *
 *   @arg   myQuery
 *          Query to be performed. 
 *
 *   @arg   response
 *          Pointer to buffer to return the data requested by the query passed
 *
 *   @arg   CSL_Status
 *          CSL_SOK            - Successful completion of the query
 *          CSL_ESYS_BADHANDLE - Invalid handle
 *          CSL_ESYS_INVQUERY  - Query command not supported
 *
 *   @eg
 *          CSL_Edma3Handle             hModule;
            CSL_Edma3HwSetup            hwSetup,gethwSetup;
            CSL_Edma3Obj                edmaObj;
            CSL_Edma3QueryInfo          info;
            CSL_Edma3CmdIntr            regionIntr;
            CSL_Edma3CmdDrae            regionAccess;
            CSL_Edma3Context            context;
            CSL_Edma3HwDmaChannelSetup  dmahwSetup[CSL_EDMA3_NUM_DMACH] = \
                                             CSL_EDMA3_DMACHANNELSETUP_DEFAULT;
            CSL_Edma3HwDmaChannelSetup  getdmahwSetup[CSL_EDMA3_NUM_DMACH];
            CSL_Status                  status;
            Uint32                      i,passStatus = 1;
           
        
            // Module Initialization
            CSL_edma3Init(&context);
            
            // Module Level Open    
            hModule = CSL_edma3Open(&edmaObj,CSL_EDMA3,NULL,&status);
            
            // Module Setup
            hwSetup.dmaChaSetup  = &dmahwSetup[0];
            hwSetup.qdmaChaSetup = NULL;
            CSL_edma3HwSetup(hModule,&hwSetup);
        
            // Query Module Info 
            CSL_edma3GetHwStatus(hModule,CSL_EDMA3_QUERY_INFO,&info);
            
            ...
 *
 * ===========================================================================
 */
extern CSL_Status  CSL_edma3GetHwStatus (
    CSL_Edma3Handle              hMod,
    CSL_Edma3HwStatusQuery       myQuery,
    void                         *response
);

/*
 *============================================================================
 *   @func CSL_edma3ChannelOpen
 *
 *   @desc
 *      Opens an EDMA channel to get access to the resources for a
 *      particular channel.
 *
 *   @arg   edmaObj
 *          Pointer to the EDMA Handle Object - to be allocated by the user
 *
 *   @arg   edmaNum
 *          Instance Number of the EDMA
 *        
 *   @arg   param
 *          Channel specific parameters
 *
 *   @arg   status
 *          Pointer to CSL Status
 *
 *   @ret   CSL_Edma3ChannelHandle
 *          Valid edma channel handle will be returned if status value is 
 *          equal to CSL_SOK
 *
 *   @eg
 *          CSL_Edma3Handle         hModule;
            CSL_Edma3Obj            edmaObj;
            CSL_Edma3Context        context;
            CSL_Status              status;
            CSL_Edma3ChannelAttr    chAttr;
            CSL_Edma3ChannelHandle  hChannel;
             
            // Module Initialization
            CSL_edma3Init(&context);
             
            // Module Level Open    
            hModule = CSL_edma3Open(&edmaObj,CSL_EDMA3,NULL,&status);
            
            // Channel 0 Open in context of Shadow region 0
            chAttr.regionNum = CSL_EDMA3_REGION_0;
            chAttr.chaNum = CSL_EDMA3_CHA_DSPINT;
            hChannel = CSL_edma3ChannelOpen(&ChObj, CSL_EDMA3, &chAttr, &status);
                                           
            ...                           
 *
 * ===========================================================================
 */
extern CSL_Edma3ChannelHandle  CSL_edma3ChannelOpen (
    CSL_Edma3ChannelObj        *edmaObj,
    CSL_InstNum                edmaNum,
    CSL_Edma3ChannelAttr       *chAttr,
    CSL_Status                 *status
);

/*
 *============================================================================
 *   @func CSL_edma3ChannelClose
 *
 *   @desc
 *      Closes (Invalidates) an EDMA channel (passed as handle) after it has 
 *      finished operating. The channel cannot be accessed any more. 
 *
 *   @arg   hEdma
 *          Channel Handle
 *
 *   @ret   CSL_Status
 *          CSL_SOK            - Close successful
 *          CSL_ESYS_BADHANDLE - Invalid handle
 *
 *   @eg
 *          CSL_Edma3Handle     hModule;
            CSL_Edma3Obj        edmaObj;
            CSL_Edma3Context    context;
            CSL_Status          status;
             
            // Module Initialization
            CSL_edma3Init(&context);
             
            // Module Level Open    
            hModule = CSL_edma3Open(&edmaObj,CSL_EDMA3,NULL,&status);
            
            // Channel 0 Open in context of Shadow region 0
            chAttr.regionNum = CSL_EDMA3_REGION_0;
            chAttr.chaNum    = CSL_EDMA3_CHA_DSPINT;
            hChannel = CSL_edma3ChannelOpen(&ChObj, CSL_EDMA3, &chAttr, &status);
            
            // Close Module
            CSL_edma3Close(hModule);
            
            //Close channel
            CSL_edma3ChannelClose (hChannel);
 *
 * ===========================================================================
 */
extern CSL_Status  CSL_edma3ChannelClose (
    CSL_Edma3ChannelHandle      hEdma
);

/*
 * =============================================================================
 *   @func CSL_edma3ChannelSetupParam
 *
 *   @desc
 *      Does the Channel setup i.e the channel to param set mapping(if relevant)
 *
 *   @arg   hEdma
 *          Channel Handle 
 *
 *   @arg   paramNum
 *          parameter set to be mapped to the channel
 *
 *   @ret   CSL_Status
 *          CSL_SOK             - Setup Param successful
 *          CSL_ESYS_BADHANDLE  - Invalid handle
 *          CSL_ESYS_INVPARAMS  - Invalid parameter
 *
 *   @eg
 *          CSL_Edma3Handle         hModule;
            CSL_Edma3Obj            edmaObj;
            CSL_Edma3Context        context;
            CSL_Edma3HwSetup        hwSetup;
            CSL_Status              status;
            Uint16                  paramNum;
            CSL_Edma3ChannelHandle  hChannel;
            CSL_Edma3HwDmaChannelSetup  dmahwSetup[CSL_EDMA3_NUM_DMACH] = 
                                              CSL_EDMA3_DMACHANNELSETUP_DEFAULT;
            CSL_Edma3HwQdmaChannelSetup qdmahwSetup[CSL_EDMA3_NUM_QDMACH] = 
                                              CSL_EDMA3_QDMACHANNELSETUP_DEFAULT; 
            // Module Initialization
            CSL_edma3Init(&context);
             
            // Module Level Open    
            hModule = CSL_edma3Open(&edmaObj,CSL_EDMA3,NULL,&status);
            
            // Module Setup
            hwSetup.dmaChaSetup  = &dmahwSetup[0];
            hwSetup.qdmaChaSetup = &qdmaSetup[0];
            CSL_edma3HwSetup(hModule,&hwSetup);
            
            // Channel 0 Open in context of Shadow region 0
            chAttr.regionNum = CSL_EDMA3_REGION_0;
            chAttr.chaNum = CSL_EDMA3_CHA_DSPINT;
            hChannel = CSL_edma3ChannelOpen(&ChObj,
                                    CSL_EDMA3,
                                    &chAttr,                            
                                    &status);   
        
            // Set the parameter entry number to channel
            paramNum = 100;
            CSL_edma3HwChannelSetupParam(hChannel,paramNum);
            ...
 *
 * ===========================================================================
 */
extern CSL_Status  CSL_edma3HwChannelSetupParam (
    CSL_Edma3ChannelHandle      hEdma,
    Uint16                      paramNum 
);

/*
 * =============================================================================
 *   @func CSL_edma3ChannelSetupTriggerWord
 *
 *   @desc
 *      Sets up the channel trigger word.This is applicable only 
 *      for QDMA channels.
 *
 *   @arg   hEdma
 *          Channel Handle 
 *
 *   @arg   triggerWord
 *          trigger word (0-7) for the channel
 *
 *   @ret   CSL_Status
 *          CSL_SOK             - Setup Param successful
 *          CSL_ESYS_BADHANDLE  - Invalid handle
 *          CSL_ESYS_INVPARAMS  - Invalid parameter
 *
 *   @eg
 *          CSL_Edma3Handle         hModule;
            CSL_Edma3Obj            edmaObj;
            CSL_Edma3Context        context;
            CSL_Edma3HwSetup        hwSetup;
            CSL_Status              status;
            CSL_Edma3ChannelHandle  hChannel;
            CSL_Edma3HwDmaChannelSetup  dmahwSetup[CSL_EDMA3_NUM_DMACH] = 
                                              CSL_EDMA3_DMACHANNELSETUP_DEFAULT;
            CSL_Edma3HwQdmaChannelSetup qdmahwSetup[CSL_EDMA3_NUM_QDMACH] = 
                                              CSL_EDMA3_QDMACHANNELSETUP_DEFAULT; 
            // Module Initialization
            CSL_edma3Init(&context);
             
            // Module Level Open    
            hModule = CSL_edma3Open(&edmaObj,CSL_EDMA3,NULL,&status);
            
            // Module Setup
            hwSetup.dmaChaSetup  = &dmahwSetup[0];
            hwSetup.qdmaChaSetup = &qdmaSetup[0];
            CSL_edma3HwSetup(hModule,&hwSetup);
            
            // Channel 0 Open in context of Shadow region 0
            chAttr.regionNum = CSL_EDMA3_REGION_0;
            chAttr.chaNum = CSL_EDMA3_CHA_DSPINT;
            hChannel = CSL_edma3ChannelOpen(&ChObj,
                                    CSL_EDMA3,
                                    &chAttr,                            
                                    &status);   
        
            // Sets up the QDMA Channel 0 trigger Word to the 3rd trigger word
            CSL_edma3HwChannelSetupTriggerWord(hChannel,3);
            ...
 *
 * ===========================================================================
 */
extern CSL_Status  CSL_edma3HwChannelSetupTriggerWord (
    CSL_Edma3ChannelHandle      hEdma,
    Uint8                       triggerWord   
);

/*
 * =============================================================================
 *   @func CSL_edma3ChannelSetupQue
 *
 *   @desc
 *      Does the Channel setup i.e the channel to que mapping
 *
 *   @arg   hEdma
 *          Channel Handle 
 *
 *   @arg   que
 *          Queue to be mapped to the channel
 *
 *   @ret   CSL_Status
 *          CSL_SOK             - Setup Param successful
 *          CSL_ESYS_BADHANDLE  - Invalid handle
 *          CSL_ESYS_INVPARAMS  - Invalid parameter
 *
 *   @eg
 *          CSL_Edma3Handle         hModule;
            CSL_Edma3Obj            edmaObj;
            CSL_Edma3Context        context;
            CSL_Edma3HwSetup        hwSetup;
            CSL_Status              status;
            CSL_Edma3Que            evtQue;
            CSL_Edma3ChannelHandle  hChannel;
            CSL_Edma3HwDmaChannelSetup  dmahwSetup[CSL_EDMA3_NUM_DMACH] = 
                                              CSL_EDMA3_DMACHANNELSETUP_DEFAULT;
            CSL_Edma3HwQdmaChannelSetup qdmahwSetup[CSL_EDMA3_NUM_QDMACH] = 
                                              CSL_EDMA3_QDMACHANNELSETUP_DEFAULT; 
            // Module Initialization
            CSL_edma3Init(&context);
             
            // Module Level Open    
            hModule = CSL_edma3Open(&edmaObj,CSL_EDMA3,NULL,&status);
            
            // Module Setup
            hwSetup.dmaChaSetup  = &dmahwSetup[0];
            hwSetup.qdmaChaSetup = &qdmaSetup[0];
            CSL_edma3HwSetup(hModule,&hwSetup);
            
            // Channel 0 Open in context of Shadow region 0
            chAttr.regionNum = CSL_EDMA3_REGION_0;
            chAttr.chaNum = CSL_EDMA3_CHA_DSPINT;
            hChannel = CSL_edma3ChannelOpen(&ChObj,
                                    CSL_EDMA3,
                                    &chAttr,                            
                                    &status);   
        
            // Set up the channel to que mapping
            CSL_edma3HwChannelSetupQue(hChannel,CSL_EDMA3_QUE_3);
            ...
 *
 * ===========================================================================
 */
extern CSL_Status  CSL_edma3HwChannelSetupQue (
    CSL_Edma3ChannelHandle       hEdma,
    CSL_Edma3Que                 que    
);

/*
 * =============================================================================
 *   @func CSL_edma3GetHwChannelSetupParam
 *
 *   @desc
 *      Obtains the current channel to parameter set mapping.
 *
 *   @arg   hEdma
 *          Channel Handle 
 *
 *   @arg   paramNum
 *          parameter set currently mapped to the channel
 *
 *   @ret   CSL_Status
 *          CSL_SOK             - Setup Param successful
 *          CSL_ESYS_BADHANDLE  - Invalid handle
 *          CSL_ESYS_INVPARAMS  - Invalid parameter
 *
 *   @eg
 *          CSL_Edma3Handle         hModule;
            CSL_Edma3Obj            edmaObj;
            CSL_Edma3Context        context;
            CSL_Edma3HwSetup        hwSetup;
            CSL_Status              status;
            Uint16                  paramNum;
            CSL_Edma3ChannelHandle  hChannel;
            CSL_Edma3HwDmaChannelSetup  dmahwSetup[CSL_EDMA3_NUM_DMACH] = 
                                              CSL_EDMA3_DMACHANNELSETUP_DEFAULT;
            CSL_Edma3HwQdmaChannelSetup qdmahwSetup[CSL_EDMA3_NUM_QDMACH] = 
                                              CSL_EDMA3_QDMACHANNELSETUP_DEFAULT; 
            // Module Initialization
            CSL_edma3Init(&context);
             
            // Module Level Open    
            hModule = CSL_edma3Open(&edmaObj,CSL_EDMA3,NULL,&status);
            
            // Module Setup
            hwSetup.dmaChaSetup  = &dmahwSetup[0];
            hwSetup.qdmaChaSetup = &qdmaSetup[0];
            CSL_edma3HwSetup(hModule,&hwSetup);
            
            // Channel 0 Open in context of Shadow region 0
            chAttr.regionNum = CSL_EDMA3_REGION_0;
            chAttr.chaNum = CSL_EDMA3_CHA_DSPINT;
            hChannel = CSL_edma3ChannelOpen(&ChObj,
                                    CSL_EDMA3,
                                    &chAttr,                            
                                    &status);   
        
            // Get the parameter entry number to which a channel is mapped to 
            CSL_edma3GetHwChannelSetupParam(hChannel,&paramNum);
            ...
 *
 * ===========================================================================
 */
extern CSL_Status  CSL_edma3GetHwChannelSetupParam (
    CSL_Edma3ChannelHandle       hEdma,
    Uint16                      *paramNum    
);

/*
 * =============================================================================
 *   @func CSL_edma3GetHwChannelSetupTriggerWord
 *
 *   @desc
 *      Gets channel trigger word.This is applicable only for QDMA channels.
 *      
 *   @arg   hEdma
 *          Channel Handle 
 *
 *   @arg   triggerWord
 *          Trigger word setup in the (QDMA)channel
 *
 *   @ret   CSL_Status
 *          CSL_SOK             - Setup Param successful
 *          CSL_ESYS_BADHANDLE  - Invalid handle
 *          CSL_ESYS_INVPARAMS  - Invalid parameter
 *
 *   @eg
 *          CSL_Edma3Handle         hModule;
            CSL_Edma3Obj            edmaObj;
            CSL_Edma3Context        context;
            CSL_Edma3HwSetup        hwSetup;
            CSL_Status              status;
            CSL_Edma3ChannelHandle  hChannel;
            Uint8                   triggerWord;
            CSL_Edma3HwDmaChannelSetup  dmahwSetup[CSL_EDMA3_NUM_DMACH] = 
                                              CSL_EDMA3_DMACHANNELSETUP_DEFAULT;
            CSL_Edma3HwQdmaChannelSetup qdmahwSetup[CSL_EDMA3_NUM_QDMACH] = 
                                              CSL_EDMA3_QDMACHANNELSETUP_DEFAULT; 
            // Module Initialization
            CSL_edma3Init(&context);
             
            // Module Level Open    
            hModule = CSL_edma3Open(&edmaObj,CSL_EDMA3,NULL,&status);
            
            // Module Setup
            hwSetup.dmaChaSetup  = &dmahwSetup[0];
            hwSetup.qdmaChaSetup = &qdmaSetup[0];
            CSL_edma3HwSetup(hModule,&hwSetup);
            
            // Channel 0 Open in context of Shadow region 0
            chAttr.regionNum = CSL_EDMA3_REGION_0;
            chAttr.chaNum = CSL_EDMA3_CHA_DSPINT;
            hChannel = CSL_edma3ChannelOpen(&ChObj,
                                    CSL_EDMA3,
                                    &chAttr,                            
                                    &status);   
        
            // Get the trigger word programmed for a channel
            CSL_edma3GetHwChannelSetupTriggerWord(hChannel,&triggerWord);
            ...
 *
 * ===========================================================================
 */
extern CSL_Status  CSL_edma3GetHwChannelSetupTriggerWord (
    CSL_Edma3ChannelHandle      hEdma,
    Uint8                       *triggerWord  
);

/*
 * =============================================================================
 *   @func CSL_edma3GetHwChannelSetupQue
 *
 *   @desc
 *      Obtains the Channel setup i.e the channel to que mapping
 *
 *   @arg   hEdma
 *          Channel Handle 
 *
 *   @arg   que
 *          Queue currently mapped to the channel
 *
 *   @ret   CSL_Status
 *          CSL_SOK             - Setup Param successful
 *          CSL_ESYS_BADHANDLE  - Invalid handle
 *          CSL_ESYS_INVPARAMS  - Invalid parameter
 *
 *   @eg
 *          CSL_Edma3Handle         hModule;
            CSL_Edma3Obj            edmaObj;
            CSL_Edma3Context        context;
            CSL_Edma3HwSetup        hwSetup;
            CSL_Status              status;
            CSL_Edma3Que            evtQue;
            CSL_Edma3ChannelHandle  hChannel;
            CSL_Edma3HwDmaChannelSetup  dmahwSetup[CSL_EDMA3_NUM_DMACH] = 
                                              CSL_EDMA3_DMACHANNELSETUP_DEFAULT;
            CSL_Edma3HwQdmaChannelSetup qdmahwSetup[CSL_EDMA3_NUM_QDMACH] = 
                                              CSL_EDMA3_QDMACHANNELSETUP_DEFAULT; 
            // Module Initialization
            CSL_edma3Init(&context);
             
            // Module Level Open    
            hModule = CSL_edma3Open(&edmaObj,CSL_EDMA3,NULL,&status);
            
            // Module Setup
            hwSetup.dmaChaSetup  = &dmahwSetup[0];
            hwSetup.qdmaChaSetup = &qdmaSetup[0];
            CSL_edma3HwSetup(hModule,&hwSetup);
            
            // Channel 0 Open in context of Shadow region 0
            chAttr.regionNum = CSL_EDMA3_REGION_0;
            chAttr.chaNum = CSL_EDMA3_CHA_DSPINT;
            hChannel = CSL_edma3ChannelOpen(&ChObj,
                                    CSL_EDMA3,
                                    &chAttr,                            
                                    &status);   
        
            // Get the que to which a channel is mapped
            CSL_edma3GetHwChannelSetupQue(hChannel,&evtQue);
            ...
 *
 * ===========================================================================
 */
extern CSL_Status  CSL_edma3GetHwChannelSetupQue (
    CSL_Edma3ChannelHandle      hEdma,
    CSL_Edma3Que                *que   
);

/*
 * =============================================================================
 *   @func CSL_edma3HwChannelControl
 *
 *   @desc
 *     Channel level control commands are handled by this API.
 *
 *   @arg   hCh
 *          Channel Handle
 *
 *   @arg   cmd
 *          Channel Command
 *
 *   @arg   cmdArg
 *          Additional command arguments are passed to the API using this. 
 *          The CSL function type casts to the appropriate arguments type 
 *          depending on the cmd. 
 *
 *   @ret   CSL_Status
 *          CSL_SOK            - Command execution successful.
 *          CSL_ESYS_BADHANDLE - Invalid handle
 *          CSL_ESYS_INVCMD    - Invalid command
 *
 *   @eg
 *          CSL_Edma3Handle             hModule;
            CSL_Edma3Obj                edmaObj;
            CSL_Edma3Context            context;
            CSL_Status                  status;
            CSL_Edma3HwSetup            hwSetup;
            CSL_Edma3ChannelObj         ChObj;
            CSL_Edma3ChannelHandle      hChannel;
            CSL_Edma3ChannelAttr        chAttr;
            CSL_Edma3HwDmaChannelSetup  dmahwSetup[CSL_EDMA3_NUM_DMACH] = 
                                             CSL_EDMA3_DMACHANNELSETUP_DEFAULT;
            CSL_Edma3HwQdmaChannelSetup qdmahwSetup[CSL_EDMA3_NUM_QDMACH] = 
                                             CSL_EDMA3_QDMACHANNELSETUP_DEFAULT;
            // Module Initialization
            CSL_edma3Init(&context);
             
            // Module Level Open    
            hModule = CSL_edma3Open(&edmaObj,CSL_EDMA3,NULL,&status);
            
            // Module Setup
            hwSetup.dmaChaSetup  = &dmahwSetup[0];
            hwSetup.qdmaChaSetup = &qdmaSetup[0];
            CSL_edma3HwSetup(hModule,&hwSetup);
            
            // Channel 0 Open in context of Shadow region 0
            chAttr.regionNum = CSL_EDMA3_REGION_0;
            chAttr.chaNum = CSL_EDMA3_CHA_DSPINT;
            hChannel = CSL_edma3ChannelOpen(&ChObj,
                                    CSL_EDMA3,
                                    &chAttr,                            
                                    &status);   
                                    
            CSL_edma3HwChannelControl(hChannel,CSL_EDMA3_CMD_CHANNEL_ENABLE,NULL);                         
            ...
 *
 * ===========================================================================
 */
extern CSL_Status  CSL_edma3HwChannelControl (
    CSL_Edma3ChannelHandle         hCh,
    CSL_Edma3HwChannelControlCmd   cmd,
    void                           *cmdArg
);

/*
 * =============================================================================
 *   @n@b CSL_edma3GetHwChannelStatus
 *
 *   @desc
 *      Channel level queries to the EDMA module are handled through this API.
 *     
 *   @arg   hCh
 *          Channel Handle
 *
 *   @arg   myQuery
 *          Channel query to be performed. 
 *
 *   @arg   response
 *          Pointer to buffer to return the data requested by the query passed
 *
 *   @ret   CSL_Status
 *          CSL_SOK            - Successful completion of the query
 *          CSL_ESYS_BADHANDLE - Invalid handle
 *          CSL_ESYS_INVQUERY  - Query command not supported
 *
 *   @eg
 *          CSL_Edma3Handle             hModule;
            CSL_Edma3Obj                edmaObj;
            CSL_Edma3Context            context;
            CSL_Status                  status;
            CSL_Edma3HwSetup            hwSetup;
            CSL_Edma3ChannelObj         ChObj;
            CSL_Edma3ChannelHandle      hChannel;
            CSL_Edma3ChannelAttr        chAttr;
            Bool                        errStat;
            CSL_Edma3HwDmaChannelSetup  dmahwSetup[CSL_EDMA3_NUM_DMACH] = 
                                             CSL_EDMA3_DMACHANNELSETUP_DEFAULT;
            CSL_Edma3HwQdmaChannelSetup qdmahwSetup[CSL_EDMA3_NUM_QDMACH] = 
                                             CSL_EDMA3_QDMACHANNELSETUP_DEFAULT;
            // Module Initialization
            CSL_edma3Init(&context);
             
            // Module Level Open    
            hModule = CSL_edma3Open(&edmaObj,CSL_EDMA3,NULL,&status);
            
            // Module Setup
            hwSetup.dmaChaSetup  = &dmahwSetup[0];
            hwSetup.qdmaChaSetup = &qdmaSetup[0];
            CSL_edma3HwSetup(hModule,&hwSetup);
            
            // Channel 0 Open in context of Shadow region 0
            chAttr.regionNum = CSL_EDMA3_REGION_0;
            chAttr.chaNum = CSL_EDMA3_CHA_DSPINT;
            hChannel = CSL_edma3ChannelOpen(&ChObj,
                                    CSL_EDMA3,
                                    &chAttr,                            
                                    &status);
            
            // Obtain Channel Error Status
            CSL_edma3GetHwChannelStatus(hChannel,CSL_EDMA3_QUERY_CHANNEL_ERR, \
                                        errStat);
            ...
 *
 * ===========================================================================
 */
extern CSL_Status  CSL_edma3GetHwChannelStatus (
    CSL_Edma3ChannelHandle          hCh,
    CSL_Edma3HwChannelStatusQuery   myQuery,
    void                            *response
);

/*
 * =============================================================================
 *   @n@b CSL_edma3GetParamHandle
 *
 *   @desc
 *      Returns the handle to the requested parameter set.
 *     
 *   @arg   hEdma
 *          Channel Handle
 *
 *   @arg   paramNum
 *          Param set number desired
 *
 *   @arg   status
 *          Pointer to the placeholder for the status
 *
 *   @ret   CSL_Edma3ParamHandle
 *          Valid param handle will be returned if status value
 *          is equal to CSL_SOK. 
 *
 *   @eg
 *          CSL_Edma3Handle             hModule;
            CSL_Edma3Obj                edmaObj;
            CSL_Edma3Context            context;
            CSL_Status                  status;
            CSL_Edma3HwSetup            hwSetup;
            CSL_Edma3ChannelObj         ChObj;
            CSL_Edma3ChannelHandle      hChannel;
            CSL_Edma3ParamHandle        hParamBasic;
            CSL_Edma3ChannelAttr        chAttr;
            CSL_Edma3HwDmaChannelSetup  dmahwSetup[CSL_EDMA3_NUM_DMACH] = 
                                             CSL_EDMA3_DMACHANNELSETUP_DEFAULT;
            CSL_Edma3HwQdmaChannelSetup qdmahwSetup[CSL_EDMA3_NUM_QDMACH] = 
                                             CSL_EDMA3_QDMACHANNELSETUP_DEFAULT;
            // Module Initialization
            CSL_edma3Init(&context);
             
            // Module Level Open    
            hModule = CSL_edma3Open(&edmaObj,CSL_EDMA3,NULL,&status);
            
            // Module Setup
            hwSetup.dmaChaSetup  = &dmahwSetup[0];
            hwSetup.qdmaChaSetup = &qdmaSetup[0];
            CSL_edma3HwSetup(hModule,&hwSetup);
            
            // Channel 0 Open in context of Shadow region 0
            chAttr.regionNum = CSL_EDMA3_REGION_0;
            chAttr.chaNum = CSL_EDMA3_CHA_DSPINT;
            hChannel = CSL_edma3ChannelOpen(&ChObj,
                                    CSL_EDMA3,
                                    &chAttr,                            
                                    &status);
            
            // Obtain a handle to parameter entry 0
            hParamBasic = CSL_edma3GetParamHandle(hChannel,0,NULL);
            ...
 *
 * ===========================================================================
 */
extern CSL_Edma3ParamHandle  CSL_edma3GetParamHandle (
    CSL_Edma3ChannelHandle       hEdma,
    Int16                        paramNum,
    CSL_Status                   *status       
);

/*
 * =============================================================================
 *   @n@b CSL_edma3ParamSetup
 *
 *   @desc
 *      Configures a EDMA Parameter Set.
 *     
 *   @arg   hParam
 *          Handle to the Parameter Set
 *
 *   @arg   pSetup
 *          Pointer to the CSL_Edma3ParamSetup 
 *
 *   @ret   CSL_Status
 *          CSL_SOK             - Command execution successful
 *          CSL_ESYS_BADHANDLE  - The handle passed is
 *                                invalid
 *          CSL_ESYS_INVPARAMS  - The parameter passed is invalid
 *
 *   @eg
 *           CSL_Edma3Handle             hModule;
            CSL_Edma3HwSetup            hwSetup,gethwSetup;
            CSL_Edma3Obj                edmaObj;
            CSL_Edma3ParamHandle        hParamPing;
            CSL_Edma3ChannelObj         ChObj;
            CSL_Edma3QueryInfo          info;
            CSL_Edma3CmdIntr            regionIntr;
            CSL_Edma3CmdDrae            regionAccess;
            CSL_Edma3ChannelHandle      hChannel;
            CSL_Edma3ParamSetup         myParamSetup;
            CSL_Edma3Context            context;
            CSL_Edma3ChannelAttr        chAttr;
            CSL_Edma3HwDmaChannelSetup  dmahwSetup[CSL_EDMA3_NUM_DMACH] = 
                                              CSL_EDMA3_DMACHANNELSETUP_DEFAULT;
            CSL_Edma3HwDmaChannelSetup  getdmahwSetup[CSL_EDMA3_NUM_DMACH];
            CSL_Status                  status;
            Uint32                      i, passStatus = 1;
           
        
            // Module Initialization
            CSL_edma3Init(&context);
            
            // Module Level Open    
            hModule = CSL_edma3Open(&edmaObj,CSL_EDMA3,NULL,&status);
            
            // Module Setup
            hwSetup.dmaChaSetup  = &dmahwSetup[0];
            hwSetup.qdmaChaSetup = NULL;
            CSL_edma3HwSetup(hModule,&hwSetup);
            
            // DRAE Enable(Bits 0-15) for the Shadow Region 0.
            regionAccess.region = CSL_EDMA3_REGION_0 ;
            regionAccess.drae =   0xFFFF ;   
            regionAccess.draeh =  0x0000 ;
            CSL_edma3HwControl(hModule,CSL_EDMA3_CMD_DMAREGION_ENABLE, \
                               &regionAccess); 
        
            // Interrupt Enable (Bits 0-11)  for the Shadow Region 0. 
            regionIntr.region =  CSL_EDMA3_REGION_0  ;   
            regionIntr.intr  =   0x0FFF ;   
            regionIntr.intrh  =  0x0000 ;
            CSL_edma3HwControl(hModule,CSL_EDMA3_CMD_INTR_ENABLE,&regionIntr);
        
            // Channel 0 Open in context of Shadow region 0
            chAttr.regionNum = CSL_EDMA3_REGION_0;
            chAttr.chaNum = CSL_EDMA3_CHA_DSPINT;
            hChannel = CSL_edma3ChannelOpen(&ChObj,
                                    CSL_EDMA3,
                                    &chAttr,                            
                                    &status);   
        
            // Obtain a handle to parameter entry 0
            hParamBasic = CSL_edma3GetParamHandle(hChannel,0,NULL);
        
            // Setup the first param Entry (Ping buffer) 
            myParamSetup.option = CSL_EDMA3_OPT_MAKE(CSL_EDMA3_ITCCHEN_DIS, \
                                                     CSL_EDMA3_TCCHEN_DIS, \
                                                     CSL_EDMA3_ITCINT_DIS, \
                                                     CSL_EDMA3_TCINT_EN,\
                                                     0,CSL_EDMA3_TCC_NORMAL,\
                                                     CSL_EDMA3_FIFOWIDTH_NONE, \
                                                     CSL_EDMA3_STATIC_DIS, \
                                                     CSL_EDMA3_SYNC_A, \
                                                     CSL_EDMA3_ADDRMODE_INCR, \
                                                     CSL_EDMA3_ADDRMODE_INCR);           
            myParamSetup.srcAddr = (Uint32)srcBuff1;         
            myParamSetup.aCntbCnt = CSL_EDMA3_CNT_MAKE(256,1);       
            myParamSetup.dstAddr = (Uint32)dstBuff1;        
            myParamSetup.srcDstBidx = CSL_EDMA3_BIDX_MAKE(1,1);     
            myParamSetup.linkBcntrld = 
                             CSL_EDMA3_LINKBCNTRLD_MAKE(CSL_EDMA3_LINK_NULL,0);     
            myParamSetup.srcDstCidx = CSL_EDMA3_CIDX_MAKE(0,1);     
            myParamSetup.cCnt = 1;
            CSL_edma3ParamSetup(hParamBasic,&myParamSetup); 
            ...
 *
 * ===========================================================================
 */
extern CSL_Status  CSL_edma3ParamSetup (
    CSL_Edma3ParamHandle        hParam,
    CSL_Edma3ParamSetup         *pSetup  
);

/*
 * =============================================================================
 *   @n@b CSL_edma3ParamWriteWord
 *
 *   @desc
 *      Writes specified word at the specified parameter set offset. 
 *     
 *   @arg   hParam
 *          Handle to the Parameter Set
 *
 *   @arg   wordOffset
 *          Word Offset at which the specified word is to be written
 *
 *   @arg   word
 *          Word itself that needs to be written
 *
 *   @ret   CSL_Status
 *          CSL_SOK            - Param Write Word successful
 *
 *   @eg
 *          CSL_Edma3Handle             hModule;
            CSL_Edma3HwSetup            hwSetup,gethwSetup;
            CSL_Edma3Obj                edmaObj;
            CSL_Edma3ParamHandle        hParamPing;
            CSL_Edma3ChannelObj         ChObj;
            CSL_Edma3QueryInfo          info;
            CSL_Edma3CmdIntr            regionIntr;
            CSL_Edma3CmdQrae            regionAccess;
            CSL_Edma3ChannelHandle      hChannel;
            CSL_Edma3ParamSetup         myParamSetup;
            CSL_Edma3Context            context;
            CSL_Edma3ChannelAttr        chAttr;
            CSL_Edma3HwDmaChannelSetup  dmahwSetup[CSL_EDMA3_NUM_DMACH] = 
                                             CSL_EDMA3_DMACHANNELSETUP_DEFAULT;
            CSL_Edma3HwQdmaChannelSetup qdmahwSetup[CSL_EDMA3_NUM_QDMACH] = 
                                             CSL_EDMA3_QDMACHANNELSETUP_DEFAULT;
            CSL_Edma3HwDmaChannelSetup  getdmahwSetup[CSL_EDMA3_NUM_DMACH];
            
            // Module Initialization
            CSL_edma3Init(&context);
            
            // Module Level Open    
            hModule = CSL_edma3Open(&edmaObj,CSL_EDMA3,NULL,&status);
            
            // Module Setup
            hwSetup.dmaChaSetup  = &dmahwSetup[0];
            hwSetup.qdmaChaSetup = &qdmahwSetup[0];;
            CSL_edma3HwSetup(hModule,&hwSetup);
            
            // DRAE Enable(Bits 0-15) for the Shadow Region 0.
            regionAccess.region = CSL_EDMA3_REGION_0 ;
            regionAccess.qrae =   0x000F ;   
            CSL_edma3HwControl(hModule,CSL_EDMA3_CMD_QDMAREGION_ENABLE, \
                               &regionAccess); 
            
            // Interrupt Enable (Bits 0-11)  for the Shadow Region 0. 
            regionIntr.region =  CSL_EDMA3_REGION_0  ;   
            regionIntr.intr  =   0x0FFF ;   
            regionIntr.intrh  =  0x0000 ;
            CSL_edma3HwControl(hModule,CSL_EDMA3_CMD_INTR_ENABLE, &regionIntr);
            
            // Channel 0 Open in context of Shadow region 0
            chAttr.regionNum = CSL_EDMA3_REGION_0;
            chAttr.chaNum = CSL_EDMA3_CHA_DSPINT;
            hChannel = CSL_edma3ChannelOpen(&ChObj,
                                    CSL_EDMA3,
                                    &chAttr,                            
                                    &status);   
            
            // Obtain a handle to parameter entry 0
            hParamBasic = CSL_edma3GetParamHandle(hChannel,0,NULL);
            
            
            // Setup the first param Entry (Ping buffer) 
            myParamSetup.option = CSL_EDMA3_OPT_MAKE(CSL_EDMA3_ITCCHEN_DIS, \
                                                     CSL_EDMA3_TCCHEN_DIS, \
                                                     CSL_EDMA3_ITCINT_DIS, \
                                                     CSL_EDMA3_TCINT_EN,\
                                                     0,CSL_EDMA3_TCC_NORMAL,\
                                                     CSL_EDMA3_FIFOWIDTH_NONE, \
                                                     CSL_EDMA3_STATIC_EN, \
                                                     CSL_EDMA3_SYNC_A, \
                                                     CSL_EDMA3_ADDRMODE_INCR, \
                                                     CSL_EDMA3_ADDRMODE_INCR);           
            myParamSetup.srcAddr = (Uint32)srcBuff1;         
            myParamSetup.aCntbCnt = CSL_EDMA3_CNT_MAKE(256,1);       
            myParamSetup.dstAddr = (Uint32)dstBuff1;        
            myParamSetup.srcDstBidx = CSL_EDMA3_BIDX_MAKE(1,1);     
            myParamSetup.linkBcntrld = 
                            CSL_EDMA3_LINKBCNTRLD_MAKE(CSL_EDMA3_LINK_NULL,0);     
            myParamSetup.srcDstCidx = CSL_EDMA3_CIDX_MAKE(0,1);     
            myParamSetup.cCnt = 1;
            CSL_edma3ParamSetup(hParamBasic,&myParamSetup); 
            
            // Enable Channel
            CSL_edma3HwChannelControl(hChannel,CSL_EDMA3_CMD_CHANNEL_ENABLE,NULL);
            
            // Write trigger word
            CSL_edma3ParamWriteWord(hParamBasic,7,myParamSetup.cCnt);    
            ...
 *
 * ===========================================================================
 */
extern CSL_Status  CSL_edma3ParamWriteWord (
    CSL_Edma3ParamHandle       hParamHndl,
    Uint16                     wordOffset,
    Uint32                     word 
);

#ifdef __cplusplus
}
#endif

#endif

