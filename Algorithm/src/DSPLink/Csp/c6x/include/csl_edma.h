/*****************************************************\
 *  Copyright 2004, Texas Instruments Incorporated.  *
 *  All rights reserved.                             *
 *  Restricted rights to use, duplicate or disclose  *
 *  this   code   are  granted   through  contract.  *
 *                                                   *
 * "@(#) PSP/CSL  3.0.0.0  (2003-09-30)              *
\*****************************************************/

/** @mainpage EDMA 
*
* @section Introduction
*
* @subsection xxx Purpose and Scope
* The purpose of this document is to detail the  CSL APIs for the
* EDMA Module.
*
* @subsection aaa Terms and Abbreviations
*   -# CSL:  Chip Support Library
*   -# API:  Application Programmer Interface
*   -# EDMA: Enhanced Direct Memory Access 
*
* @subsection References
*    -# CSL 3.x Technical Requirements Specifications Version 0.5, dated
*       May 14th, 2003
*    -# EDMA Channel Controller Specification (Revision 0.25)
*    -# EDMA Transfer Controller Specification (Revision 0.14)
*
* @subsection Assumptions
*     The abbreviations EDMA, edma and Edma have been used throughout this
*     document to refer to Enhanced Direct Memory Access.
*/

/** @file csl_edma.h
 *
 *    @brief    Header file for functional layer CSL of EDMA
 *
 *  Description
 *    - The different enumerations, structure definitions
 *      and function declarations
 *  Last Modified : 29th June 2004
 */
#ifndef _CSL_EDMA_H_
#define _CSL_EDMA_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <csl.h>
#include <cslr_edmacc.h>

#include "davinci_hdEdma.h"
#include "davinci_hd.h"


/**
@defgroup CSL_EDMA_API EDMA
*/
/**
@defgroup CSL_EDMA_DATASTRUCT  EDMA Data Structures
@ingroup CSL_EDMA_API
*/
/**
@defgroup CSL_EDMA_SYMBOL  EDMA Symbols Defined
@ingroup CSL_EDMA_API
*/
/**
@defgroup CSL_EDMA_ENUM  EDMA Enumerated Data Types
@ingroup CSL_EDMA_API
*/

/**
@defgroup CSL_EDMA_FUNCTION  EDMA Functions
@ingroup CSL_EDMA_API
*/
/**
@defgroup CSL_EDMA_FUNCTION_MODULE  EDMA Module Functions
@ingroup CSL_EDMA_FUNCTION
*/
/**
@defgroup CSL_EDMA_FUNCTION_SHADOW EDMA Region Functions
@ingroup CSL_EDMA_FUNCTION
*/
/**
@defgroup CSL_EDMA_FUNCTION_CHANNEL EDMA Channel Functions
@ingroup CSL_EDMA_FUNCTION
*/
/**
@defgroup CSL_EDMA_FUNCTION_MODULE_INTERNAL  EDMA Module Internal Functions
@ingroup CSL_EDMA_FUNCTION_MODULE
*/
/**
@defgroup CSL_EDMA_FUNCTION_SHADOW_INTERNAL  EDMA Region Internal Functions
@ingroup CSL_EDMA_FUNCTION_SHADOW
*/
/**
@defgroup CSL_EDMA_FUNCTION_CHANNEL_INTERNAL  EDMA Channel Internal Functions
@ingroup CSL_EDMA_FUNCTION_CHANNEL
*/
/** @addtogroup CSL_EDMA_SYMBOL
* @{ */
#define CSL_EDMA_LINK_NULL                 0xFFFF 
/**< Link to a Null Param entry */
#define CSL_EDMA_LINK_DEFAULT              0xFFFF
/**< Link to a Null Param entry */
#define CSL_EDMA_SYNC_A                    0
/**< A sycnhronized transfer  */
#define CSL_EDMA_SYNC_AB                   1
/**< AB sycnhronized transfer */
#define CSL_EDMA_SYNC_ARRAY                CSL_EDMA_SYNC_A
/**< Array/A sycnhronized transfer */
#define CSL_EDMA_SYNC_FRAME                CSL_EDMA_SYNC_AB
/**< Frame/AB sycnhronized transfer */
#define	CSL_EDMA_TCC_NORMAL  		           0
/**< Normal Completion */
#define	CSL_EDMA_TCC_EARLY                     1
/**< Early  Completion */
#define	CSL_EDMA_FIFOWIDTH_NONE                0
/**< Only for ease  */
#define	CSL_EDMA_FIFOWIDTH_8BIT                0    
/**< 8 bit FIFO Width */
#define	CSL_EDMA_FIFOWIDTH_16BIT               1    
/**< 16 bit FIFO Width */
#define	CSL_EDMA_FIFOWIDTH_32BIT               2    
/**< 32 bit FIFO Width */
#define	CSL_EDMA_FIFOWIDTH_64BIT               3    
/**< 64 bit FIFO Width */
#define	CSL_EDMA_FIFOWIDTH_128BIT              4   
/**< 128 bit FIFO Width */
#define	CSL_EDMA_FIFOWIDTH_256BIT              5    
/**< 256 bit FIFO Width */
#define	CSL_EDMA_ADDRMODE_INCR                 0
/**< Address Mode is incremental */
#define	CSL_EDMA_ADDRMODE_FIFO                 1
/**< Address Mode is such it wraps around after reaching FIFO width */


/* Bitwise OR of the below symbols are used for setting the Memory attributes 
   These are defined only if the Memory Protection feature exists */
#if CSL_EDMA_MEMPROTECT

#define CSL_EDMA_MEMACCESS_UX              0x0001  
/**< User Execute permission */  
#define CSL_EDMA_MEMACCESS_UW              0x0002  
/**< User Write permission */
#define CSL_EDMA_MEMACCESS_UR              0x0004  
/**< User Read permission */
#define CSL_EDMA_MEMACCESS_SX              0x0008  
/**< Supervisor Execute permission */  
#define CSL_EDMA_MEMACCESS_SW              0x0010  
/**< Supervisor Write permission */
#define CSL_EDMA_MEMACCESS_SR              0x0020  
/**< Supervisor Read permission */
#define CSL_EDMA_MEMACCESS_EMU             0x0040  
/**< Emulation security permission */  
#define CSL_EDMA_MEMACCESS_NS              0x0080  
/**< Non secure Access permission */  
#define CSL_EDMA_MEMACCESS_EXT             0x0200  
/**< External Allowed ID. VBus requests with PrivID >= '6' are permitted if access type is allowed*/

#define CSL_EDMA_MEMACCESS_AID0		   0x0400  
/**< Allowed ID '0' */
#define CSL_EDMA_MEMACCESS_AID1		   0x0800  
/**< Allowed ID '1' */
#define CSL_EDMA_MEMACCESS_AID2		   0x1000  
/**< Allowed ID '2' */
#define CSL_EDMA_MEMACCESS_AID3		   0x2000  
/**< Allowed ID '3' */
#define CSL_EDMA_MEMACCESS_AID4		   0x4000  
/**< Allowed ID '4' */
#define CSL_EDMA_MEMACCESS_AID5		   0x8000  
/**< Allowed ID '5' */

#endif
#define CSL_EDMA_TRIGWORD_NONE         7       
/**< No trigger word. Useful while doing setup for EDMA channels */
#define CSL_EDMA_TRIGWORD_DEFAULT      7       
/**< Last trigger word in a QDMA parameter entry */


/** Used for creating the options entry in the parameter ram */
#define CSL_EDMA_OPT_MAKE(itccEn,tccEn,itcintEn,tcintEn,tcc,tccMode,fwid,stat,syncDim,dam,sam) \
(Uint32)(\
     CSL_FMKR(23,23,itccEn) \
    |CSL_FMKR(22,22,tccEn) \
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
#define CSL_EDMA_CNT_MAKE(aCnt,bCnt) \
(Uint32)(\
     CSL_FMK(EDMACC_A_B_CNT_ACNT,aCnt) \
    |CSL_FMK(EDMACC_A_B_CNT_BCNT,bCnt)\
    )
/** Used for creating the link and B count reload entry in the parameter ram */
#define CSL_EDMA_LINKBCNTRLD_MAKE(link,bCntRld) \
(Uint32)(\
     CSL_FMK(EDMACC_LINK_BCNTRLD_LINK,link) \
    |CSL_FMK(EDMACC_LINK_BCNTRLD_BCNTRLD,bCntRld)\
    )   
/** Used for creating the B index entry in the parameter ram */
#define CSL_EDMA_BIDX_MAKE(src,dst) \
(Uint32)(\
     CSL_FMK(EDMACC_SRC_DST_BIDX_DSTBIDX,dst) \
    |CSL_FMK(EDMACC_SRC_DST_BIDX_SRCBIDX,src)\
    )

/** Used for creating the C index entry in the parameter ram */
#define CSL_EDMA_CIDX_MAKE(src,dst) \
(Uint32)(\
     CSL_FMK(EDMACC_SRC_DST_CIDX_DSTCIDX,dst) \
    |CSL_FMK(EDMACC_SRC_DST_CIDX_SRCCIDX,src)\
    )
/**
@}
*/


/**
@addtogroup CSL_EDMA_ENUM
@{
*/
/** @brief Enumeration for VBusM priority 
*
* This is used for Setting up the Queue Priority level
*/
typedef enum {
	CSL_EDMA_VBUSM_PRI0 = 0,         /**< VBusM priority level 0 */
	CSL_EDMA_VBUSM_PRI1 = 1,         /**< VBusM priority level 1 */
	CSL_EDMA_VBUSM_PRI2 = 2,         /**< VBusM priority level 2 */
	CSL_EDMA_VBUSM_PRI3 = 3,         /**< VBusM priority level 3 */
	CSL_EDMA_VBUSM_PRI4 = 4,         /**< VBusM priority level 4 */
	CSL_EDMA_VBUSM_PRI5 = 5,         /**< VBusM priority level 5 */
	CSL_EDMA_VBUSM_PRI6 = 6,         /**< VBusM priority level 6 */
	CSL_EDMA_VBUSM_PRI7 = 7          /**< VBusM priority level 7 */
}CSL_EdmaEventQueuePri;

/** @brief Enumeration for EDMA Queue Thresholds
*
* This is used for Setting up the Queue thresholds
*/
typedef enum {
	CSL_EDMA_QUETHRESH_0 = 0,        /**< EDMA Queue Threshold 0 */
	CSL_EDMA_QUETHRESH_1 = 1,        /**< EDMA Queue Threshold 1 */
	CSL_EDMA_QUETHRESH_2 = 2,        /**< EDMA Queue Threshold 2 */
	CSL_EDMA_QUETHRESH_3 = 3,        /**< EDMA Queue Threshold 3 */
	CSL_EDMA_QUETHRESH_4 = 4,        /**< EDMA Queue Threshold 4 */
	CSL_EDMA_QUETHRESH_5 = 5,        /**< EDMA Queue Threshold 5 */
	CSL_EDMA_QUETHRESH_6 = 6,        /**< EDMA Queue Threshold 6 */
	CSL_EDMA_QUETHRESH_7 = 7,        /**< EDMA Queue Threshold 7 */
	CSL_EDMA_QUETHRESH_8 = 8,        /**< EDMA Queue Threshold 8 */
	CSL_EDMA_QUETHRESH_9 = 9,        /**< EDMA Queue Threshold 9 */
	CSL_EDMA_QUETHRESH_10 = 10,      /**< EDMA Queue Threshold 10 */
	CSL_EDMA_QUETHRESH_11 = 11,      /**< EDMA Queue Threshold 11 */
	CSL_EDMA_QUETHRESH_12 = 12,      /**< EDMA Queue Threshold 12 */
	CSL_EDMA_QUETHRESH_13 = 13,      /**< EDMA Queue Threshold 13 */
	CSL_EDMA_QUETHRESH_14 = 14,      /**< EDMA Queue Threshold 14 */
	CSL_EDMA_QUETHRESH_15 = 15,      /**< EDMA Queue Threshold 15 */
	CSL_EDMA_QUETHRESH_16 = 16,      /**< EDMA Queue Threshold 15 */
	CSL_EDMA_QUETHRESH_DISABLE = 17  /**< EDMA Queue Threshold Disable Errors */
}CSL_EdmaEventQueueThr;

/** @brief MODULE Level Commands
*
* 
*/
typedef enum {
#if CSL_EDMA_MEMPROTECT
	CSL_EDMA_CMD_MEMPROTECT_SET,       /**< (Arg: @a #CSL_EdmaCmdRegion*)Programmation of MPPAG,MPPA[0-7] attributes */
#endif	
	CSL_EDMA_CMD_DMAREGION_ENABLE,     /**< (Arg: @a #CSL_EdmaCmdDmaRegion*  )Programmation of DRAE,DRAEH region Enable attributes */
	CSL_EDMA_CMD_DMAREGION_DISABLE,    /**< (Arg: @a #CSL_EdmaCmdDmaRegion*  )Programmation of DRAE,DRAEH region Disable attributes */
	CSL_EDMA_CMD_QDMAREGION_ENABLE,    /**< (Arg: @a #CSL_EdmaCmdQdmaRegion*)Programmation of QRAE region Enable attributes */
	CSL_EDMA_CMD_QDMAREGION_DISABLE,   /**< (Arg: @a #CSL_EdmaCmdQdmaRegion*)Programmation of QRAE region Disable attributes */	
	CSL_EDMA_CMD_QUEPRIORITY_SET, 	   /**< (Arg: @a #CSL_EdmaCmdQuePriority*)Programmation of QUEPRI register with the specified priority */	
	CSL_EDMA_CMD_QUETHRESHOLD_SET,     /**< (Arg: @a #CSL_EdmaCmdQueThr*     )Programmation of QUE Threshold levels */
	CSL_EDMA_CMD_QUETC_MAP,            /**< (Arg: @a #CSL_EdmaCmdQueTc*      )Programmation of QUE to TC Mapping  */   
	CSL_EDMA_CMD_ERROR_EVAL,	  	   /**< (Arg: #None)Programmation of Error Evaluation */
	CSL_EDMA_CMD_INTERRUPT_CLEAR,      /**< (Arg: @a #CSL_BitMask32* )Clears specified (Bitmask)pending interrupt at Module Level */
	CSL_EDMA_CMD_INTERRUPT_ENABLE,     /**< (Arg: @a #CSL_BitMask32* )Enables specified interrupts(BitMask) at Module Level */
	CSL_EDMA_CMD_INTERRUPT_DISABLE,    /**< (Arg: @a #CSL_BitMask32* )Disables specified interrupts(BitMask) at Module Level */
	CSL_EDMA_CMD_INTERRUPT_EVAL        /**< (Arg: #None )Interrupt Evaluation asserted for the Module */
}CSL_EdmaHwControlCmd;

/** @brief MODULE Level Queries
*
* 
*/
typedef enum {
	CSL_EDMA_QUERY_REVISION,	   		/**< (Arg: @a #Uint32*)Returns the revision of the module */
#if CSL_EDMA_MEMPROTECT	
	CSL_EDMA_QUERY_MEMFAULT,	   		/**< (Arg: @a #CSL_EdmaMemFaultStatus*)Return the Memory fault attributes \n
      										 Also Clears the Memory Fault */		
    CSL_EDMA_QUERY_MEMPROTECT,          /**< (Arg: @a #CSL_EdmaQueryRegion*)Return memory attribute of the specified region */      										 
#endif    
	CSL_EDMA_QUERY_CTRLERROR,		    /**< (Arg: @a #CSL_EdmaccStatus*)Return Controller Error and \n
										     Also Clears the Controller Error */
	CSL_EDMA_QUERY_INTERRUPT_PENDSTATUS,/**< (Arg: @a #CSL_BitMask32*)Return pend status of specified interrupt */
	
	CSL_EDMA_QUERY_INTERRUPT_MISSED   	/**< (Arg: @a #CSL_BitMask32*)Returns Miss Status of all interrupts \n
					     	                 Also clears the missed status */
}CSL_EdmaHwStatusQuery;

/** @brief SHADOW REGION Level Commands
*
* 
*/
typedef enum {
	CSL_EDMA_CMD_REGION_INTERRUPT_CLEAR,	 /**< (Arg: @a #CSL_BitMask32*)Clears the specified pending interrupt(BitMask) at shadow Level */
	CSL_EDMA_CMD_REGION_INTERRUPT_ENABLE,    /**< (Arg: @a #CSL_BitMask32*)Enables specified interrupt(BitMask) at Shadow region Level */
	CSL_EDMA_CMD_REGION_INTERRUPT_DISABLE,   /**< (Arg: @a #CSL_BitMask32*)Disables specified interrupt(BitMask) at Shadow region Level */
	CSL_EDMA_CMD_REGION_INTERRUPT_EVAL       /**< (Arg: #None)Interrupt Evaluation asserted at Shadow Region level */
}CSL_EdmaHwRegionControlCmd;

/** @brief SHADOW REGION Level Queries 
*
* 
*/

typedef enum {
	CSL_EDMA_QUERY_REGION_INTERRUPT_PENDSTATUS /**< (Arg: @a #CSL_BitMask32*)Return pend status(BitMask) of specified interrupt */
}CSL_EdmaHwRegionStatusQuery;

/** @brief CHANNEL Commands
*
* 
*/
typedef enum {
	CSL_EDMA_CMD_CHANNEL_ENABLE,		 /**< (Arg: #None)Enables specified Channel */
	CSL_EDMA_CMD_CHANNEL_DISABLE,		 /**< (Arg: #None)Disables specified Channel */
	CSL_EDMA_CMD_CHANNEL_SET,		 	 /**< (Arg: #None)Manually sets the Channel Event, NA for QDMA   */
	CSL_EDMA_CMD_CHANNEL_CLEAR		 	 /**< (Arg: #None)Manually clears the Channel Event, NA for QDMA */
}CSL_EdmaHwChannelControlCmd;

/** @brief CHANNEL Queries
*
* 
*/	
typedef enum  {
	CSL_EDMA_QUERY_CHANNEL_SETUP 		 /**< (Arg: @a #CSL_EdmaHwChannelSetup*)Returns the Channel Setup */
}CSL_EdmaHwChannelStatusQuery;

/** @brief Enumeration for Param Setup Stage.
*
* 
*/	
typedef enum  {	
	CSL_EDMA_PARAM_BASIC,          		 /**< It is mandatory for all parameter entries to be setup with
				 	          				  this argument passed to the Param Setup API */
	CSL_EDMA_PARAM_LINK,		  	 	 /**< When the Param Setup API is invoked with this argument
				      		  				  the Link field of the specified parameter entry is updated */		  
	CSL_EDMA_PARAM_CHAIN				 /**< When the Param Setup API is invoked with this argument
						  					  the Tcc field of the specified parameter entry is updated 
					      	  				  with the Channel number of specified Channel*/		  
}CSL_EdmaParamSetupStage;

/**
@}
*/
/** @addtogroup CSL_EDMA_DATASTRUCT
* @{ */


/** @brief Module specific context information.
    This is a dummy handle. 
 */
typedef void *CSL_EdmaContext;      

/** @brief Module Parameter specific context information.
    This is a dummy handle. 
 */
typedef void *CSL_EdmaModuleParam;

/** @brief Module Object
    This is a dummy structure. 
 */
typedef void *CSL_EdmaObj;

/** @brief Edma Region Object Structure
 *
 *  An object of this type is allocated by the user and
 *  its address is passed as a parameter to the CSL_edmaRegionOpen()
 *  The CSL_edmaRegionOpen().
 */
 
typedef struct CSL_EdmaRegionObj {
    CSL_EdmaccShadowRegsOvly shregs;	 /**< Pointer to the Shadow Region Registers */
    Int                region;			 /**< Region number of the handle */
	Int                edmaNum;          /**< Edma Module number */  
}CSL_EdmaRegionObj;

typedef struct CSL_EdmaRegionObj *CSL_EdmaRegionHandle;

/** CSL Parameter Entry Handle
 *
 */
typedef volatile CSL_EdmaccParamentryRegs *CSL_EdmaParamHandle;
/** @brief Edma ParamSetup Structure
 *
 *  An object of this type is allocated by the user and
 *  its address is passed as a parameter to the CSL_edmaParamSetup().
 *  This structure is used to program the Param Entry for EDMA/QDMA.
 *  The macros can be used to assign values to the fields of the structure.
 */
typedef struct CSL_EdmaParamSetup {
    Uint32          option;              /**< Options */
    Uint32 	   	    srcAddr;      		 /**< Specifies the source address */ 
    Uint32          aCntbCnt;       	 /**< Lower 16 bits are A Count 
		  						      		  Upper 16 bits are B Count*/
    Uint32 		    dstAddr;         	 /**< Specifies the destination address */ 	                                       
    Uint32          srcDstBidx;      	 /**< Lower 16 bits are source b index 
					      					  Upper 16 bits are destination b index */
    Uint32          linkBcntrld;      	 /**< Lower 16 bits are link of the next param entry
					 	     			      Upper 16 bits are b count reload */
    Uint32          srcDstCidx;          /**< Lower 16 bits are source c index
					      					  Upper 16 bits are destination c index */
    Uint32          cCnt;	         	 /**< C count */
    Uint8           triggerWord;         /**< Trigger Word. For normal EDMA Channel parameter entries 
                                              this should be set to CSL_EDMA_TRIGWORD_NONE else
                                              for QDMA parameter entries this should be 
                                              set to CSL_EDMA_TRIGWORD_DEFAULT/Number of the trigger word i.e 0-6
                                              */
} CSL_EdmaParamSetup;


/** @brief Edma Object Structure
 *
 *  An object of this type is allocated by the user and
 *  its address is passed as a parameter to the CSL_edmaChannelOpen()
 *  The CSL_edmaChannelOpen() updates all the members of the data structure
 *  and returns the objects address as a @a #CSL_EdmaChanHandle. The
 *  @a #CSL_EdmaChanHandle is used in all subsequent function calls.
 */
 
typedef struct CSL_EdmaChanObj {
    CSL_EdmaccRegsOvly      ccregs;          	 /**< Pointer to the Edma Channel Controller module register Overlay structure */
	Int                     region;              /**< Region number to which the channel belongs to */ 
    Int                     edmaNum;         	 /**< EDMA instance whose channel is being requested */
    Int                     chaNum;          	 /**< Channel Number being requested */
} CSL_EdmaChanObj;
/** @brief Edma Channel Setup
 *
 *  An object of this type is allocated by the user and
 *  its address is passed as a parameter to the CSL_edmaHwChannelSetup()
 */

typedef struct CSL_EdmaHwChannelSetup {
	CSL_EdmaEventQueue que;                      /**< Que number for the channel */
	Uint16  paramEntry;							 /**< Parameter entry mapping for the channel. This may not be ini
													  initialized for Edma channels on devices that do not have
													  CHMAPEXIST. But must always be initialised for Qdma channels on every device
													  */				
	Uint8   triggerWord;						 /**< Trigger word for the QDMA channels. This is not required to be initialised for 
	                                                  EDMA channels */
} CSL_EdmaHwChannelSetup;

/** EDMA Module Handle 
 *  All module level API calls must be made with this handle. 
 */
typedef volatile CSL_EdmaccRegs *CSL_EdmaHandle;
/** CSL Channel Handle
 *  All channel level API calls must be made with this handle. 
 */
typedef struct CSL_EdmaChanObj *CSL_EdmaChanHandle;

#if CSL_EDMA_MEMPROTECT	
/** @brief Edma Memory Protection Fault Error Status
 *
 *  An object of this type is allocated by the user and
 *  its address is passed as a parameter to the CSL_edmaGetMemoryFaultError()
 *  / CSL_edmaGetHwStatus() with the relevant command. This is relevant only is 
 *  MPEXIST is present for a given device.
 */
typedef struct CSL_EdmaMemFaultStatus {
    Uint32           addr;           		 /**< Memory Protection Fault Address */
    CSL_BitMask16    errorMask;       		 /**< Bit Mask of the Errors */
    Uint16           fid;             		 /**< Faulted ID */
}CSL_EdmaMemFaultStatus;
#endif
/** @brief Edma Controller Error Status. 
 *
 *  An object of this type is allocated by the user and
 *  its address is passed as a parameter to the CSL_edmaGetControllerError()
 *  /CSL_edmaGetHwStatus().
 */
typedef struct CSL_EdmaccStatus {
    CSL_BitMask16    errorMask;       		 /**< Bit Mask of the Queue Threshold Errors */
    Bool             exceedTcc;       		 /**< Whether number of permissible outstanding Tcc's 
    						      				  is exceeded */
}CSL_EdmaccStatus;
/** @brief Edma Query Structure for querying region specific 
 *  attributes. 
 *
 *  An object of this type is allocated by the user and
 *  its address is passed as a parameter to the CSL_edmaGetHwStatus
 *  with the relevant command.   
 */


typedef struct CSL_EdmaCmdQueryRegion {
    Int              region;          		 /**< This field needs to be initialized 
          				    	      			  by the user before issuing the query/command */
	CSL_BitMask32    regionVal;	   		         /**< This needs to be filled by the user in case of issuing a 
    						      				  COMMAND or it will be filled in by the CSL when 
						      					  used with a QUERY */
}CSL_EdmaCmdQueryRegion,CSL_EdmaCmdQdmaRegion,CSL_EdmaQueryRegion,CSL_EdmaCmdRegion;
/** @brief Edma Command Structure for setting region specific 
 *  attributes.
 *
 *  An object of this type is allocated by the user and
 *  its address is passed as a parameter to the CSL_edmaGetHwStatus
 *  when 
 */


typedef struct CSL_EdmaCmdDmaRegion {
    Int   region;                /**< This field needs to be initialiazed 
						      		  by the user before issuing the command specifying 
						      		  the region for which attributes need to be set */
    CSL_BitMask32     drae;	     /**< DRAE Setting for the region  */
    CSL_BitMask32     draeh;	 /**< DRAEH Setting for the region */
}CSL_EdmaCmdDmaRegion;


/** @brief Edma Command Structure used for setting Event Queue priority level
 *
 *  An object of this type is allocated by the user and
 *  its address is passed as a parameter to the CSL_edmaHwControl API.
 */


typedef struct CSL_EdmaCmdQuePriority {
    CSL_EdmaEventQueue         que;              /**< Specifies the Queue that needs a priority change */
    CSL_EdmaEventQueuePri      pri;              /**< Queue priority */
}CSL_EdmaCmdQuePriority;    

/** @brief Edma Command Structure used for setting Event Queue to Transfer Controller mapping
 *
 *  An object of this type is allocated by the user and
 *  its address is passed as a parameter to the CSL_edmaHwControl API.
 */


typedef struct CSL_EdmaCmdQueTc {
    CSL_EdmaEventQueue         que;              /**< Specifies the Queue that needs a change to the Tc mapped */
    CSL_EdmaTc                 tc;               /**< Transfer controller number mapped */
}CSL_EdmaCmdQueTc;    


/** @brief Edma Command Structure used for setting Event Queue threshold level
 *
 *  An object of this type is allocated by the user and
 *  its address is passed as a parameter to the CSL_edmaHwControl API.
 */
typedef struct CSL_EdmaCmdQueueThr {
    CSL_EdmaEventQueue         que;              /**< Specifies the Queue that needs a change in the threshold setting */
    CSL_EdmaEventQueueThr      threshold;        /**< Queue threshold setting */
}CSL_EdmaCmdQueThr;    

/** @brief This will have the base-address information for the module
 *  instance
 */
typedef struct {
	CSL_EdmaccRegsOvly	regs;					/**< Base-address of the peripheral registers */
} CSL_EdmaModuleBaseAddress;

/** @brief Edma Region parameter structure used for opening a shadow handle
 */
typedef struct {	
	Int16 regionNum;							/**< Specifies the region number to be opened*/
} CSL_EdmaRegionParam;

/** @brief Edma Channel parameter structure used for opening a channel
 */
typedef struct {	
	Int regionNum;								/**< Region Number */
	Int chaNum;									/**< Channel number */
} CSL_EdmaChannelParam;
/**
@}
*/

/**
@addtogroup CSL_EDMA_FUNCTION_MODULE
@{
*/


/**************************************************************************\
* EDMA global function declarations
\**************************************************************************/
/*
 * ======================================================
 *   @func   CSL_edmaInit
 * ======================================================
 */
/** @brief 
 *     This is EDMA module's init function. 
 *     This initializes the context Object
 *     variables. 
 *     
 *     Needs to be called before using EDMA module. 
 *
 */

CSL_Status
    CSL_edmaInit (
        CSL_EdmaContext *   pContext
);
/*
 * ======================================================
 *   @func   CSL_edmaOpen
 * ======================================================
 */
/** @brief 
 *     Returns a handle to the EDMA Module. This handle is further used
 *     for invoking all module level control APIs. 
 *
 *     This function merely returns a handle 
 *     to the register overlay structure. 
 *     This call enables the relocatability of code since the handle 
 *     can be subsequently used for Module level control commands rather than the usage of 
 *     of the register layer base address directly.
 *
 */
 /* ======================================================
 *   @arg  edmaObj
 *		Dummy argument 
 *   @arg  edmaNum
 *		Instance Number of the EDMA
 *   @arg  param
 *		Dummy argument 
 *   @arg  status
 *		Status of the API 
 *
 * ======================================================
 */

CSL_EdmaHandle  CSL_edmaOpen(
    /* EDMA Module Object pointer. This is NULL. */    
    CSL_EdmaObj              *edmaObj,
    /** instance of EDMA */
    CSL_InstNum              edmaNum,
    /** EDMA Param pointer i.e NULL in this case */
    CSL_EdmaModuleParam      *param,
    /** CSL Status */
    CSL_Status               *status
);
/*
 * ======================================================
 *   @func   CSL_edmaClose
 * ======================================================
 */
/** @brief 
 *	   The EDMA module handlde is invalidated and in order to
 *	   use the EDMA module it needs to be opened again.
 */
 /* ======================================================
 *   @arg  Module Handle
 *		Handle to the EDMA module
 *
 * ======================================================
 */

CSL_Status CSL_edmaClose(
    CSL_EdmaHandle			 hEdma
);
/* Module Level Control APIs */
/*
 * ======================================================
 *   @func   CSL_edmaHwControl
 * ======================================================
 */
/** @brief 
 *	   Module level control commands are handled by this API.
 */
 /* ======================================================
 *   @arg  hMod
 *		Module Handle
 *   @arg  cmd
 *		Module Command
 *   @arg  cmdArg
 *	  Additional command arguments are passed to the API using this. 
 *    The CSL function type casts to the appropriate arguments type depending on
 *    the cmd.  
 *
 * ======================================================
 */
CSL_Status  CSL_edmaHwControl
(
	/** Pointer to the object that holds reference to the
     * instance of EDMA requested after the call */
	CSL_EdmaHandle           hMod,
	/** The command to this API which indicates the action to be taken */
	CSL_EdmaHwControlCmd     cmd,
	/** Optional argument @a void* casted */
	void                     *cmdArg
);
/** @brief Function to get the Base-address of the peripheral instance.
 *
 *  This function is used for getting the base-address of the peripheral
 *  instance. This function will be called inside the @ CSL_edmaOpen()/CSL_edmaChannelOpen()
 *  /CSL_edmaRegionOpen()
 *  function call.
 *
 *  Note: This function is open for re-implementing if the user wants to modify
 *        the base address of the peripheral object to point to a different
 *        location and there by allow CSL initiated write/reads into peripheral
 *        MMR's go to an alternate location. Please refer the documentation for
 *        more details.
 *
 * @b Example:
 * @verbatim

      CSL_Status status;
      CSL_EdmaModuleBaseAddress   baseAddress;

       ...
      status = CSL_edmaccGetModuleBaseAddr(CSL_EDMACC_0, NULL, &baseAddress);
   @endverbatim
 *
 */

CSL_Status CSL_edmaccGetModuleBaseAddr(
        /** Instance number
         */
        CSL_InstNum  		edmaNum,
        /** Module specific parameters
         */
        CSL_EdmaModuleParam       *pParam,
        /** Base address details
         */
        CSL_EdmaModuleBaseAddress *pBaseAddress
);
/*
 * ======================================================
 *   @func   CSL_edmaGetHwStatus
 * ======================================================
 */
/** @brief 
 *	   Module level queries to the EDMA module are handled through this API.
 */
 /* ======================================================
 *   @arg  hMod
 *		Module Handle
 *   @arg  query
 *		Module myQuery 
 *   @arg  response
 *	  Additional query arguments are passed to the API using the response structure. 
 *    The query response is passed back to the user program using this pointer.
 *    Depending on the 'myQuery' this argument is type casted appropriately.
 *
 * ======================================================
 */
CSL_Status  CSL_edmaGetHwStatus(
   /** Pointer to the object that holds reference to the
     * instance of EDMA */
	CSL_EdmaHandle                 hMod,
	/** The query to this API which indicates the status/setup
     * to be returned */
	CSL_EdmaHwStatusQuery          myQuery,
	/** Placeholder to return the status; @a void* casted */
	void                           *response
);
/**
@}
*/
/**
@addtogroup CSL_EDMA_FUNCTION_SHADOW
@{
*/          
/*
 * ======================================================
 *   @func   CSL_edmaRegionOpen
 * ======================================================
 */
/** @brief 
 *     Opens an EDMA Shadow region to get access to the shadow region 
 *     resources. On success, returns a valid handle which can be
 *     used to access the shadow resources.
 */
 /* ======================================================
 *   @arg  edmaShObj
 *		Pointer to the EDMA Region Object - to be allocated by the user.
 *   @arg  edmaNum
 *		Instance Number of the EDMA.
 *   @arg  param
 *		Region specific parameters
 *   @arg  status
 *		Pointer to CSL Status.
 *
 *
 * ======================================================
 */

CSL_EdmaRegionHandle  CSL_edmaRegionOpen(
	/** pointer to the object that holds reference to the shadow region opened */
    CSL_EdmaRegionObj           *edmaShObj,
	/** instance of EDMA whose shadow region is requested for open */
    CSL_InstNum                 edmaNum,
	/** parameters associated */
    CSL_EdmaRegionParam         *param,
	/** points to the placeholder for the status (success/errors) of the call */
    CSL_Status                  *status
);
/*
 * ======================================================
 *   @func   CSL_EdmaRegionClose
 * ======================================================
 */
/** @brief 
 *	   Closes the specified EDMA Shadow region. Henceforth access
 *     to the shadow region resources can be obtained only 
 *     after a successful CSL_edmaRegionOpen Call.
 *
 */
 /* ======================================================
 *   @arg  hRegion
 *		 Shadow region handle
 *
 * ======================================================
 */
CSL_Status  CSL_edmaRegionClose(
	/**  Shadow region handle */
    CSL_EdmaRegionHandle        hRegions
);

/*
 * ======================================================
 *   @func   CSL_edmaHwRegionControl
 * ======================================================
 */
/** @brief 
 *	   Shadow Region level control commands are handled by this API.
 */
 /* ======================================================
 *   @arg  hRegion
 *		 Shadow region handle
 *   @arg  cmd
 *		Shadow region command
 *   @arg  cmdArg
 *	  Additional command arguments are passed to the API using this. 
 *    The CSL function type casts to the appropriate arguments type depending on
 *    the cmd.  
 *
 * ======================================================
 */
CSL_Status  CSL_edmaHwRegionControl
(
    /**  Shadow region handle */
    CSL_EdmaRegionHandle           hRegion,
    /** The command to this API which indicates the action to be taken */
    CSL_EdmaHwRegionControlCmd     cmd,
    /** Optional argument @a void* casted */
    void                           *cmdArg
);
/* Top level Shadow Query API */
/*
 * ======================================================
 *   @func   CSL_edmaGetHwRegionStatus
 * ======================================================
 */
/** @brief 
 *	   Shadow region level queries to the EDMA module are handled through this API.
 */
 /* ======================================================
 *   @arg  hRegion
 *		Shadow region handle
 *   @arg  myQuery
 *		Shadow region query 
 *   @arg  response
 *	  Additional query arguments are passed to the API using the response structure. 
 *    The query response is passed back to the user program using this pointer.
 *    Depending on 'myQuery' this argument is type casted appropriately.
 *
 * ======================================================
 */
CSL_Status  CSL_edmaGetHwRegionStatus(
    /** Shadow region handle */
    CSL_EdmaRegionHandle                 hRegion,
    /** The query to this API which indicates the status/setup
     * to be returned */
    CSL_EdmaHwRegionStatusQuery     	 myQuery,
    /** Placeholder to return the status; @a void* casted */
    void                                 *response
);
/**
@}
*/  

/**
@addtogroup CSL_EDMA_FUNCTION_CHANNEL
@{
*/        
/*
 * ======================================================
 *   @func   CSL_edmaChannelOpen
 * ======================================================
 */
/** @brief 
 *  Opens an EDMA channel to get access to the resources for a particular channel.
 */
 /* ======================================================
 *   @arg  edmaObj
 *		Pointer to the EDMA Handle Object - to be allocated by the user.
 *   @arg  edmaNum
 *		Instance Number of the EDMA.
 *   @arg  param
 *		Channel specific parameters
 *   @arg  status
 *		Pointer to CSL Status.
 *
 *
 * ======================================================
 */

 CSL_EdmaChanHandle  CSL_edmaChannelOpen(
    /** Pointer to the channel object*/
    CSL_EdmaChanObj            *edmaObj,
    /** Module instance number */
    CSL_InstNum                edmaNum,
    /** Channel parameters  */
    CSL_EdmaChannelParam       *param,
    /** place holder for the status */
    CSL_Status                 *status
);

/*
 * ======================================================
 *   @func   CSL_edmaChannelClose
 * ======================================================
 */
/** @brief
 *     Closes (Invalidates) an EDMA channel (passed as handle) after it has finished
 *	   operating. The channel cannot be accessed any more. 
 *
 */
/* ======================================================
 *   @arg  hEdma
 *		Channel Handle
 *
 * ======================================================
*/

 CSL_Status  CSL_edmaChannelClose(
    /** Channel Handle*/
    CSL_EdmaChanHandle	      hEdma
);

/*
 * ======================================================
 *   @func   CSL_edmaChannelSetup
 * ======================================================
 */
/** @brief 
 *     Does the Channel setup i.e the channel to param entry mapping(if relevant)
 *     trigger word programmation (if relevant) and the 
 *     Que to which the channel is mapped.
 *
 */
 /* ======================================================
 *   @arg  hEdma
 *		Channel handle
 *   @arg  setup
 *		Setup structure
 *
 * ======================================================
 */

CSL_Status  CSL_edmaChannelSetup(
    /** Channel Handle*/   
    CSL_EdmaChanHandle	      hEdma,
    /** Setup Parameters */
    CSL_EdmaHwChannelSetup      *setup
);

/* Higher level control API */
/*
 * ======================================================
 *   @func   CSL_edmaHwChannelControl
 * ======================================================
 */
/** @brief 
 *	   Channel level control commands are handled by this API.
 */
 /* ======================================================
 *   @arg  hCh
 *		Channel Handle
 *   @arg  cmd
 *		Channel Command
 *   @arg  cmdArg
 *	  Additional command arguments are passed to the API using this. 
 *    The CSL function type casts to the appropriate arguments type depending on
 *    the cmd.  
 *
 * ======================================================
 */
CSL_Status  CSL_edmaHwChannelControl
(
	/** Channel Handle*/
    CSL_EdmaChanHandle              hCh,
    /** The command to this API which indicates the action to be taken */
    CSL_EdmaHwChannelControlCmd     cmd,
    /** Optional argument @a void* casted */
    void                            *cmdArg
);

/* top level query API */
/*
 * ======================================================
 *   @func   CSL_edmaGetHwChannelStatus
 * ======================================================
 */
/** @brief 
 *	   Channel level queries to the EDMA module are handled through this API.
 */
 /* ======================================================
 *   @arg  hCh
 *		Channel Handle
 *   @arg  myQuery
 *		Channel query 
 *   @arg  response
 *	 Additional query arguments are passed to the API using the response structure. 
 *   The query response is passed back to the user program using this pointer.
 *   Depending on 'myQuery' this argument is type casted appropriately.
 *
 * ======================================================
 */

CSL_Status  CSL_edmaGetHwChannelStatus(
	/** Channel Handle */
    CSL_EdmaChanHandle                 hCh,
    /** The query command */
    CSL_EdmaHwChannelStatusQuery    myQuery,
    /** Placeholder to return the status @a void* casted */
    void                           *response
);

/*
 * ======================================================
 *   @func   CSL_edmaGetParamHandle
 * ======================================================
 */
/** @brief 
 *	  Returns the handle to the requested parameter entry.
 *
 */
/* ======================================================
 *   @arg  hEdma
 *		Channel Handle
 *   @arg  entryNum
 *		Entry number desired.
 *   @arg  status
 *		Pointer to the placeholder for the status.
 * ======================================================
*/
CSL_EdmaParamHandle  CSL_edmaGetParamHandle(
    /**  a handle to the channel instance of the specified EDMA
     *   obtained  through the @a CSL_edmaChannelOpen() call */
    CSL_EdmaChanHandle            hEdma,
    /**  entry number to be acquired */
    Uint16                        entryNum,
    /**  placeholder for the status of operation */
    CSL_Status			      	  *status	    
);
  
/*
 * ======================================================
 *   @func   CSL_edmaParamSetup
 * ======================================================
 */
/** @brief 
 *	   Configures a EDMA Parameter Entry.
 *
 */
/* ======================================================
 *   @arg  hParam
 *		Handle to the Parameter Handle
 *   @arg  void
 *	    	Pointer to the CSL_EdmaParamSetup (in case of BASIC parameter ram setup)OR
 *						   CSL_EdmaParamHandle(in case of LINK parameter ram setup) OR
 *      				   CSL_EdmaChanHandle (in case of CHAIN parameter ram setup)
 *   		depending on the 3rd argument.	
 *   @arg  stage	
 *		Stage of the Setup i.e BASIC/LINK/CHAIN
 * 
 * ======================================================
*/
CSL_Status  CSL_edmaParamSetup(
    /**  a handle to the param entry acquired previously by the channel */
    CSL_EdmaParamHandle        hParam,
    /** Pointer to setup structure which contains the
     *  information to program EDMA Channel to the startup state */
    void                       *pSetup,
     /**
        Specifies the stage of Setup i.e Could be initial(basic), link setup or chain Setup. These 
        do not fall into the category of the Control Commands since 
        this does not fall into the category of the run time change of parameter 
        setups. The user is expected to follow this flow of parameter entry setup hence
        3 diferent stages are required */
    CSL_EdmaParamSetupStage    stage  
);

/*
 * ======================================================
 *   @func   CSL_edmaParamWriteWord
 * ======================================================
 */
/** @brief 
 *		Writes specified word at the specified parameter entry offset. 
 *
 */
/* ======================================================
 *   @arg  hParamHndl
 *		Handle to the Parameter Entry
 *   @arg  wordOffset
 *	    Word Offset at which the specified word is to be written
 *   @arg  word
 *		Word itself that needs to be written
 *
 * ======================================================
*/

CSL_Status  CSL_edmaParamWriteWord(
    /** a handle to the param entry acquired previously by the QDMA channel */
    CSL_EdmaParamHandle        hParamHndl,
    /** word offset in the 8 word paramater entry */
    Uint16                     wordOffset,
    /** word to be written */
    Uint32                     word 
);
#endif
