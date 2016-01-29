/*  ============================================================================
 *   Copyright (c) Texas Instruments Inc 2002, 2003, 2004, 2005
 *
 *   Use of this software is controlled by the terms and conditions found
 *   in the license agreement under which this software has been supplied.
 *  ============================================================================
 */
/** ============================================================================
 *  @file    csl_ddr2.h
 *
 *  @Path    $(CSLPATH)\inc
 *
 *  @desc    Header file for functional layer of CSL 
 *    - The different enumerations, structure definitions
 *      and function declarations
 * 
 */
/* =============================================================================
 *  Revision History
 *  ===============
 *  11-Apr-2005 RM  File Created.
 *  
 *  05-Oct-2005 NG  Updation done according to new register layer
 *  
 *  03-Feb-2006 ds  ddrDrive field added to CSL_Ddr2Settings 
 * =============================================================================
 */

/** 
 * @mainpage DDR2 EMIF CSL 3.x
 *
 * @section Introduction
 *
 * @subsection xxx Purpose and Scope
 * The purpose of this document is to identify a set of common CSL APIs for
 * the DDR2 EMIF module across various devices. The CSL developer is expected 
 * to refer to this document while designing APIs for these modules. Some of 
 * the cases listed APIs may not be applicable to a given DDR2 EMIF module. 
 * While other in this list of APIs may not be sufficient to cover all the 
 * features of a particular DDR2 EMIF Module. The CSL developer should use his 
 * discretion designing new APIs or extending the existing ones to cover these.
 *
 * @subsection aaa Terms and Abbreviations
 *   -# CSL:  Chip Support Library
 *   -# API:  Application Programmer Interface
 *
 * @subsection References
 *    -# EMIF_SPEC, EMIF Module Specifications Document; Version 3.16.1  
 *                  Jan 18,2005
 *
 */
#ifndef _CSL_DDR2_H_
#define _CSL_DDR2_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <cslr.h>
#include <soc.h>
#include <csl_error.h>
#include <csl_types.h>
#include <cslr_ddr2.h>

/*
 * The defaults of DDR2 SDRAM Timing1 Control structure
 */
#define CSL_DDR2_TIMING1_TRFC_DEFAULT    0x7F
#define CSL_DDR2_TIMING1_TRP_DEFAULT     0x07
#define CSL_DDR2_TIMING1_TRCD_DEFAULT    0x07
#define CSL_DDR2_TIMING1_TWR_DEFAULT     0x07
#define CSL_DDR2_TIMING1_TRAS_DEFAULT    0x1F
#define CSL_DDR2_TIMING1_TRC_DEFAULT     0x1F
#define CSL_DDR2_TIMING1_TRRD_DEFAULT    0x07
#define CSL_DDR2_TIMING1_TWTR_DEFAULT    0x03

/*
 * The defaults of DDR2 SDRAM Timing2 Control structure.
 */

#define CSL_DDR2_TIMING2_T_ODT_DEFAULT    0x03
#define CSL_DDR2_TIMING2_TSXNR_DEFAULT    0x7F
#define CSL_DDR2_TIMING2_TSXRD_DEFAULT    0xFF
#define CSL_DDR2_TIMING2_TRTP_DEFAULT     0x07
#define CSL_DDR2_TIMING2_TCKE_DEFAULT     0x1F

/*
 * The default values of SDRAM config, refresh, timing1 and timing2 registers 
 * which are other than the reset values.
 */
#define CSL_DDR2_SDCFG_DEFAULT     (0x00008A20u)
#define CSL_DDR2_SDRFC_DEFAULT     (0x00000753u)
#define CSL_DDR2_SDTIM1_DEFAULT    (0xFFFFFFFBu)
#define CSL_DDR2_SDTIM2_DEFAULT    (0x007FFFFFu)

/******************************************************************************
 * DDR2 global typedef declarations
 ******************************************************************************
 */

/** @brief Enumeration for bit field narrow_mode of SDRAM Config Register */ 
typedef enum {
    /** DDR2 SDRAM data bus width is 32 bits  */
    CSL_DDR2_NORMAL_MODE      = 0,
    
    /** DDR2 SDRAM data bus width is 16 bits   */
    CSL_DDR2_NARROW_MODE     = 1
}CSL_Ddr2Mode; 

/** @brief Enumeration for bit field cl of SDRAM Config Register */ 
typedef enum {
    /** Cas Latency is 2  */
    CSL_DDR2_CAS_LATENCY_2    = 2,
    
    /** Cas Latency is 3   */
    CSL_DDR2_CAS_LATENCY_3    = 3,
    
    /** Cas Latency is 4  */
    CSL_DDR2_CAS_LATENCY_4    = 4,
    
    /** Cas Latency is 5   */
    CSL_DDR2_CAS_LATENCY_5    = 5
} CSL_Ddr2CasLatency; 

/** @brief Enumeration for bit field ibank of SDRAM Config Register */ 
typedef enum {
    /** DDR2 SDRAM has one internal bank  */
    CSL_DDR2_1_SDRAM_BANKS    = 0,
    
    /** DDR2 SDRAM has two internal banks   */
    CSL_DDR2_2_SDRAM_BANKS    = 1,
    
    /** DDR2 SDRAM has four internal bank  */
    CSL_DDR2_4_SDRAM_BANKS    = 2,
    
    /** DDR2 SDRAM has eight internal banks   */
    CSL_DDR2_8_SDRAM_BANKS    = 3
} CSL_Ddr2IntBank; 

/** @brief Enumeration for bit field pagesize of SDRAM Config Register */ 
typedef enum {
    /** 256-word pages requiring 8 column address bits  */
    CSL_DDR2_256WORD_8COL_ADDR      = 0,
    
    /** 512-word pages requiring 9 column address bits  */
    CSL_DDR2_512WORD_9COL_ADDR      = 1,
    
    /** 1024-word pages requiring 10 column address bits */
    CSL_DDR2_1024WORD_10COL_ADDR    = 2,
    
    /** 2048-word pages requiring 11 column address bits */
    CSL_DDR2_2048WORD_11COL_ADDR    = 3
} CSL_Ddr2PageSize; 

/** @brief Enumeration for bit field SR of SDRAM Config Register */ 
typedef enum {
    /** Disables Self Refresh on DDR2  */
    CSL_DDR2_SELF_REFRESH_DISABLE    = 0,
    
    /** Connected DDR2 SDRAM device will enter Self Refresh Mode 
     *  and DDR2 EMIF enters Self Refresh State   
     */
    CSL_DDR2_SELF_REFRESH_ENABLE     = 1
} CSL_Ddr2SelfRefresh; 

/** @brief Enumeration for bit field ddr_drive of SDRAM Config Register */ 
typedef enum {
    /** Normal drive strength */
    CSL_DDR2_NORM_DRIVE      = 0,
    
    /** Weak drive strengths   */
    CSL_DDR2_WEAK_DRIVE     = 1
}CSL_Ddr2Drive; 

/** 
 *  @brief Module specific context information.  
 */
typedef struct {
    /** Context information of DDR2 external memory interface CSL passed as an
     *  argument to CSL_ddr2Init().Present implementation of DDR2 CSL doesn't 
     *  have any context information; hence assigned NULL.
     *  The below declaration is just a place-holder for future implementation.
     */
    Uint16    contextInfo;
    
} CSL_Ddr2Context;

/** 
 * @brief This structure contains the base-address information for the DDR2
 *        instance
 */
typedef struct {
    /** Base-address of the configuration registers of the peripheral */
    CSL_Ddr2RegsOvly    regs;
} CSL_Ddr2BaseAddress;


/** 
 * @brief This Object contains the reference to the instance of DDR2 opened
 *  using the @a CSL_ddr2Open().
 *
 *  The pointer to this, is passed to all DDR2 CSL APIs.
 */
typedef struct CSL_Ddr2Obj {
    /** This is a pointer to the registers of the instance of DDR2
     *  referred to by this object
     */
    CSL_Ddr2RegsOvly    regs;
    
    /** This is the instance of DDR2 being referred to by this object  */
    CSL_InstNum         perNum;    
} CSL_Ddr2Obj;

/** 
 * @brief this is a pointer to @a CSL_Ddr2Obj and is passed as the first
 *  parameter to all DDR2 CSL APIs
 */
typedef struct CSL_Ddr2Obj *CSL_Ddr2Handle;

/** 
 * @brief This is module specific parameters.  Present implementation of DDR2 
 *        CSL doesn't have any module specific parameters.
 */
typedef struct {
    /** Bit mask to be used for module specific parameters. The below
     *  declaration is just a place-holder for future implementation. Passed as 
     *  an argument to CSL_ddr2Open().
     */
    CSL_BitMask16    flags;
} CSL_Ddr2Param;

/** 
 * @brief DDR2 SDRAM Timing1 structure.
 * All fields needed for DDR2 SDRAM Timing1 are present in this structure.
 */
typedef struct {
    /** Specifies TRFC value: Minimum number of DDR2 EMIF cycles from Refresh 
     *  or Load command to Refresh or Activate command, minus one 
     */
    Uint8    trfc;
    /** Specifies TRP value: Minimum number of DDR2 EMIF cycles from Pre-charge
     *  to Active or Refresh command, minus one  
     */
    Uint8    trp;
    /** Specifies TRCD value: Minimum number of DDR2 EMIF cycles from Active to 
     *  Read or Write command, minus one  
     */
    Uint8    trcd;
    /** Specifies TWR value: Minimum number of DDR2 EMIF cycles from last write 
     *  transfer to Pre-charge command, minus one 
     */
    Uint8    twr;
    /** Specifies TRAS value: Minimum number of DDR2 EMIF cycles from Activate 
     *  to Pre-charge command, minus one  
     */
    Uint8    tras;
    /** Specifies TRC value: Minimum number of DDR2 EMIF cycles from Activate 
     *  command to Activate command, minus one 
     */
    Uint8    trc;
    /** Specifies TRRD value: Minimum number of DDR2 EMIF cycles from Activate 
     *  command to Activate command for a differnt bank, minus one 
     */
    Uint8    trrd;
    /** Specifies the minimum number of DDR2 EMIF clock cycles from last DDR 
     *  Write to DDR Read,  minus one        
     */
    Uint8    twtr;
} CSL_Ddr2Timing1;

/** @brief DDR2 SDRAM Timing2 structure.
 *
 * All fields needed for DDR2 SDRAM Timing2 are present in this structure.
 */
typedef struct {    
    /** Specifies the minimum number of DDR2 EMIF clock cycles from ODT enable 
     *  to write data driven for DDR2 SDRAM.          
     */
    Uint8    todt;
    /** Specifies the minimum number of DDR2 EMIF clock cycles from Self-Refresh
     *  exit to any command other than a Read command, minus one.
     */
    Uint8    tsxnr;
    /** Specifies the minimum number of DDR2 EMIF clock cycles from Self-Refresh
     *  exit to a Read command for DDR SDRAM, minus one.
     */
    Uint8    tsxrd;
    /** Specifies the minimum number of DDR2 EMIF clock cycles from the last 
     *  Read command to a Pre-charge command for DDR2 SDRAM, minus one.
     */
    Uint8    trtp;
    /** Specifies the minimum number of DDR2 EMIF clock cycles between 
     *  pado_mcke_o changes, minus one.
     */
    Uint8    tcke;
} CSL_Ddr2Timing2;

/** @brief DDR2 SDRAM Settings structure.
 *
 * All fields needed for DDR2 SDRAM settings are present in this structure.
 */
typedef struct {
    /** CAS Latency */
    CSL_Ddr2CasLatency    casLatncy;
    /** Defines number of banks inside connected SDRAM devices */
    CSL_Ddr2IntBank       ibank;
    /** Defines the internal page size of connected SDRAM devices */
    CSL_Ddr2PageSize      pageSize;     
    /** SDRAM data bus width */
    CSL_Ddr2Mode          narrowMode;
    /** DDR SDRAM drive strength */
    CSL_Ddr2Drive         ddrDrive;
} CSL_Ddr2Settings;

/** 
 * @brief This has all the fields required to configure DDR2 at Power Up
 *  (after a Hardware Reset) or a Soft Reset.  
 *
 *  This structure is used to setup or obtain existing setup of
 *  DDR2 using @a CSL_ddr2HwSetup() & @a CSL_ddr2GetHwSetup() functions
 *  respectively.
 */
typedef struct {
    /** Refresh Rate */
    Uint16               refreshRate;
    /** Structure for DDR2 SDRAM Timing1 */
    CSL_Ddr2Timing1    *timing1Param;
    /** Structure for DDR2 SDRAM Timing2 */
    CSL_Ddr2Timing2     *timing2Param;
    /** Structure for DDR2 SDRAM configuration */
    CSL_Ddr2Settings    *setParam;        
} CSL_Ddr2HwSetup;

/** 
 * @brief DDR2 Module ID and Revision structure
 *
 *  This structure is used for querying the DDR2 module ID and revision 
 */
typedef struct {
    /** DDR2 EMIF Module ID */
    Uint16    modId;
    /** DDR2 EMIF Major Revision */
    Uint8     majRev;
    /** DDR2 EMIF Minor Revision */
    Uint8     minRev;
} CSL_Ddr2ModIdRev;   

/** 
 * @brief DDR2 config structure which is used in CSL_ddr2HwSetupRaw function
 */
typedef struct {
    /** SDRAM Config Register */
    volatile Uint32 SDCFG;
    /** SDRAM Refresh Control Register */
    volatile Uint32 SDRFC;    
    /** SDRAM Timing1 Register*/

    volatile Uint32 SDTIM1;
    /** SDRAM Timing2 Register */
    volatile Uint32 SDTIM2;    
    /** VBUSM Burst Priority Register */
    volatile Uint32 BPRIO;     
} CSL_Ddr2Config;

/** 
 * @brief Enumeration for queries passed to @a CSL_ddr2GetHwStatus()
 *
 * This is used to get the status of different operations 
 */
typedef enum {
    /** Get the DDR2 EMIF module ID and revision numbers (response type: 
     *  @a (CSL_Ddr2ModIdRev*)) 
     */
    CSL_DDR2_QUERY_REV_ID,     
    /** Get the EMIF refresh rate information
     *  (response type: @a Uint16 *)  
     */
    CSL_DDR2_QUERY_REFRESH_RATE,
    /** Get self refresh bit value(response type: 
     *  @a (CSL_Ddr2SelfRefresh *))   
     */
    CSL_DDR2_QUERY_SELF_REFRESH,
    /** VBUSM Configuration values such as FIFO depths and Bus width
     *   (response type: @a CSL_Ddr2VbusmCfg*)   
     */
    CSL_DDR2_QUERY_ENDIAN,
    /** Reflects the value on the IFRDY_ready port (active high) that
     *  defines whether the DDR IFRDY is ready for normal operation.
     *   (response type: @a Uint8*)   
     */
    CSL_DDR2_QUERY_IFRDY
} CSL_Ddr2HwStatusQuery;

/** 
 * @brief Enumeration for commands passed to @a CSL_ddr2HwControl()
 *
 * This is used to select the commands to control the operations
 * existing setup of DDR2. The arguments to be passed with each
 * enumeration if any are specified next to the enumeration.
 */
typedef enum {    
    /** Self refresh enable or disable based on arg passed
     *  : argument @a (CSL_Ddr2SelfRefresh *)  
     */
    CSL_DDR2_CMD_SELF_REFRESH = 0,
    /** Enters the Refresh rate value : argument @a (Uint16 *) */     
    CSL_DDR2_CMD_REFRESH_RATE,     
    /** Number of memory transfers after which the DDR2 EMIF momentarily 
     *  raises the priority of old commands in the VBUSM Command FIFO. : 
     *  argument @a (Uint8 *) 
     */
    CSL_DDR2_CMD_PRIO_RAISE        
} CSL_Ddr2HwControlCmd;
 
/** @brief The defaults of DDR2 SDRAM Timing1 Control structure */
#define CSL_DDR2_TIMING1_DEFAULTS  {\
    (Uint8)CSL_DDR2_TIMING1_TRFC_DEFAULT, \
    (Uint8)CSL_DDR2_TIMING1_TRP_DEFAULT,  \
    (Uint8)CSL_DDR2_TIMING1_TRCD_DEFAULT, \
    (Uint8)CSL_DDR2_TIMING1_TWR_DEFAULT, \
    (Uint8)CSL_DDR2_TIMING1_TRAS_DEFAULT, \
    (Uint8)CSL_DDR2_TIMING1_TRC_DEFAULT, \
    (Uint8)CSL_DDR2_TIMING1_TRRD_DEFAULT, \
    (Uint8)CSL_DDR2_TIMING1_TWTR_DEFAULT \
}

/** @brief The defaults of DDR2 SDRAM Timing2 Control structure */
#define CSL_DDR2_TIMING2_DEFAULTS  { \
    (Uint8)CSL_DDR2_TIMING2_T_ODT_DEFAULT, \
    (Uint8)CSL_DDR2_TIMING2_TSXNR_DEFAULT, \
    (Uint8)CSL_DDR2_TIMING2_TSXRD_DEFAULT, \
    (Uint8)CSL_DDR2_TIMING2_TRTP_DEFAULT,  \
    (Uint8)CSL_DDR2_TIMING2_TCKE_DEFAULT   \
}

/** @brief The defaults of DDR2 SDRAM Config structure */
#define CSL_DDR2_SETTING_DEFAULTS  {  \
    (CSL_Ddr2CasLatency)CSL_DDR2_CAS_LATENCY_5, \
    (CSL_Ddr2IntBank)CSL_DDR2_4_SDRAM_BANKS, \
    (CSL_Ddr2PageSize)CSL_DDR2_256WORD_8COL_ADDR, \
    (CSL_Ddr2Mode)CSL_DDR2_NORMAL_MODE,  \
    (CSL_Ddr2Drive)CSL_DDR2_NORM_DRIVE \
}

/** @brief The default Config structure */
#define CSL_DDR2_CONFIG_DEFAULTS { \
    CSL_DDR2_SDCFG_DEFAULT,       \
    CSL_DDR2_SDRFC_DEFAULT,       \
    CSL_DDR2_SDTIM1_DEFAULT,       \
    CSL_DDR2_SDTIM2_DEFAULT,       \
    CSL_DDR2_BPRIO_RESETVAL \
}

/******************************************************************************
 * DDR2 global function declarations
 ******************************************************************************
 */
/** ===========================================================================
 *   @n@b   CSL_ddr2Init
 *
 *  @b Description
 *  @n This function is idempotent i.e. calling it many times is same as calling
 *     it once.  This function presently does nothing.
 *
 * <b> Usage Constraints: </b>
 *  This function should be called before using any of the CSL APIs
 *
 *   @b Arguments
     @verbatim
            pContext   Context information for the instance. Should be NULL
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK - Always returns
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 * @verbatim
   ...
   
   CSL_ddr2Init( NULL );
   ...    
   }
   @endverbatim
 *
 *
 * ============================================================================
 */
CSL_Status  CSL_ddr2Init (
    CSL_Ddr2Context    *pContext
);

/** =============================================================================
 *   @n@b   CSL_ddr2Open
 *
 *   @b Description
 *   @n This function returns the handle to the DDR2 instance. This
 *      handle is passed to all other CSL APIs.
 *
 *   @b Arguments
 *   @verbatim

            pDdr2Obj        Pointer to the object that holds reference to the
                            instance of DDR2 requested after the call
            
            ddr2Num         Instance of DDR2 to which a handle is requested
 
            pDdr2Param      Pointer to module specific parameters
 
            pStatus         pointer for returning status of the function call

     @endverbatim
 *
 *   <b> Return Value </b>  
 *      CSL_Ddr2Handle - Valid DDR2 instance handle will be returned if status
 *    @li                value is equal to CSL_SOK.
 * 
 *   <b> Pre Condition </b>
 *   @n  @a The DDR2 must be successfully initialized via CSL_ ddr2Init() 
 *          before calling this function.
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b  Modifies
 *   @n  None
 *
 *   @b Example:
 *   @verbatim
         CSL_Status         status;
         CSL_Ddr2Obj        ddr2Obj;
         CSL_Ddr2Handle     hDdr2;
 
         hDdr2 = CSL_Ddr2Open(&ddr2Obj,
                              CSL_DDR2,
                              NULL,
                              &status
                             );
     @endverbatim
 *
 * ===========================================================================
 */
CSL_Ddr2Handle CSL_ddr2Open (
    CSL_Ddr2Obj      *pDdr2Obj,
    CSL_InstNum      ddr2Num,
    CSL_Ddr2Param    *pDdr2Param,
    CSL_Status       *pStatus
);

/** ===========================================================================
 *   @n@b csl_ddr2Close.c                                                       
 *                                                                             
 *   @b Description                                                            
 *   @n This function marks that CSL for the external memory interface instance 
 *      is CSL for the external memory interface instance need to be reopened 
 *      before external memory interface CSL API.                                        
 *                                                                             
 *   @b Arguments                                                              
 *   @verbatim                                                                 
            hDdr2         Pointer to the object that holds reference to the
                          instance of DDR2 requested after the call        
     @endverbatim                                                              
 *                                                                             
 *   <b> Return Value </b>  CSL_Status                                         
 *   @li                    CSL_SOK            - external memory interface is 
 *                                               close successfully                  
 *                                                                             
 *   @li                    CSL_ESYS_BADHANDLE - The handle passed is invalid  
 *                                                                             
 *   <b> Pre Condition </b>                                                    
 *   @n  Both @a CSL_ddr2Init() and @a CSL_ddr2Open() must be called 
 *     successfully in that order before @a CSL_ddr2Close() can be called.
 *                                                                             
 *   <b> Post Condition </b>                                                   
 *   @n  1. The external memory interface CSL APIs can not be called until the    
 *          external memory interface CSL is reopened again using CSL_ddr2Open()   
 *                                                                             
 *   @b Modifies                                                               
 *   @n  hDdr2 structure                                                                 
 *                                                                             
 *   @b Example                                                                
 *   @verbatim                                                                 
            CSL_Ddr2Handle   hDdr2;                                              
                                                                               
            ...                                                                
                                                                               
            CSL_ddr2Close (hDdr2);                                               
                                                                               
            ...                                                                
     @endverbatim     
 *
 * ============================================================================
 */               
CSL_Status  CSL_ddr2Close (
    CSL_Ddr2Handle    hDdr2
);

/** ===========================================================================
 *   @n@b CSL_ddr2GetBaseAddress
 *
 * @b Description
 * @n Function to get the base address of the peripheral instance.
 *    This function is used for getting the base address of the peripheral
 *    instance. This function will be called inside the CSL_mcbspOpen()
 *    function call.
 *
 *   @b Arguments
 *   @verbatim      
            ddr2Num         Specifies the instance of the DDR2 external memory
                            interface for which the base address is requested
 
            pDdr2Param      Module specific parameters.
 
            pBaseAddress    Pointer to the base address structure to return the
                            base address details.
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_OK            Open call is successful
 *   @li                    CSL_ESYS_FAIL     The external memory interface 
 *                                            instance is not available.
 *
 *   <b> Pre Condition </b>
 *   @n  @a CSL_ddr2Init() and CSL_ddr2Open () must be called successfully.
 *
 *   <b> Post Condition </b>
 *   @n  Base address structure is populated
 *
 *   @b Modifies
 *   @n    1. The status variable
 *
 *         2. Base address structure.
 *
 *   @b Example
 *   @verbatim
        CSL_Status          status;
        CSL_Ddr2BaseAddress  baseAddress;

       ...
       status = CSL_ddr2GetBaseAddress(CSL_DDR2, NULL, &baseAddress);

    @endverbatim
 *  
 *
 * ============================================================================
 */
CSL_Status CSL_ddr2GetBaseAddress (
    CSL_InstNum            ddr2Num,
    CSL_Ddr2Param          *pDdr2Param,
    CSL_Ddr2BaseAddress    *pBaseAddress
);

/** ============================================================================
 *   @n@b CSL_ddr2HwSetupRaw
 *
 *   @b Description
 *   @n This function initializes the device registers with the register-values
 *      provided through the config data structure.  
 *
 *   @b Arguments
 *   @verbatim
            hDdr2           Handle to the DDR2 external memory interface 
                            instance
            
            config          Pointer to the config structure containing the
                            device register values
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK             - Configuration successful
 *   @li                    CSL_ESYS_BADHANDLE  - Invalid handle
 *   @li                    CSL_ESYS_INVPARAMS  - Configuration structure
 *                                                pointer is not properly
 *                                                initialized
 *
 *   <b> Pre Condition </b>
 *   @n  @a CSL_ddr2Init() and @a CSL_ddr2Open () must be called successfully.        
 *       before calling this function.          
 *
 *   <b> Post Condition </b>
 *   @n  The registers of the specified DDR2 EMIF instance will be 
 *       setup according to the values passed through the config structure
 *
 *   @b Modifies
 *   @n Hardware registers of the DDR2 EMIF
 *    
 *   @b Example
 *   @verbatim
        CSL_Ddr2Handle           hDdr2;
        CSL_Ddr2Config           config = CSL_DDR2_CONFIG_DEFAULTS;
        CSL_Status               status;

        ...       
       
        status = CSL_ddr2HwSetupRaw (hDdr2, &config);
        ..
        
            
     @endverbatim
 *
 * ===========================================================================
 */
CSL_Status  CSL_ddr2HwSetupRaw (
    CSL_Ddr2Handle    hDdr2,
    CSL_Ddr2Config    *config
);

/** ============================================================================
 * @n@b   CSL_ddr2HwSetup
 *
 * @b Description
 * @n This function initializes the device registers with the appropriate values
 *  provided through the HwSetup data structure. 
 *
 *  @b Arguments
 *  @verbatim      
            hDdr2           Pointer to the peripheral data object of the
                            DDR2 external memory interface instance
 
            setup           Pointer to setup structure which contains the
                            information to program DDR2 to a useful state
 
    @endverbatim
 *
 *  <b> Return Value </b>  CSL_Status
 *  @li                    CSL_OK               Hwsetup successful
 *  @li                    CSL_ESYS_BADHANDLE   The handle passed is invalid
 *  @li                    CSL_ESYS_INVPARAMS   The param passed is invalid
 *
 *  <b> Pre Condition </b>
 *  Both @a CSL_ddr2Init() and @a CSL_ddr2Open() must be called
 *  successfully in that order before this function can be called.  The user
 *  has to allocate space for & fill in the main setup structure appropriately
 *  before calling this function
 *
 *  <b> Post Condition </b>
 *  @n  DDR2 registers are configured according to the hardware setup parameters
 *
 *  @b Modifies
 *  @n DDR2 registers
 *
 *  @b Example:
 *  @verbatim
     CSL_Ddr2Handle hDdr2;     
     CSL_Ddr2Timing1 tim1 = CSL_DDR2_TIMING1_DEFAULTS;
     CSL_Ddr2Timing2 tim2 = CSL_DDR2_TIMING2_DEFAULTS;
     CSL_Ddr2Settings set = CSL_DDR2_SETTING_DEFAULTS;
     CSL_Ddr2HwSetup hwSetup ;
     
     hwSetup.refreshRate = (Uint16)0x753;
     hwSetup.timing1Param = &tim1;
     hwSetup.timing2Param = &tim2;
     hwSetup.setParam = &set;
     
     CSL_ddr2HwSetup(hDdr2, &hwSetup);
    @endverbatim
 *
 *  @return Returns the status of the setup operation
 *
 * ============================================================================
 */
CSL_Status  CSL_ddr2HwSetup (
    CSL_Ddr2Handle     hDdr2,
    CSL_Ddr2HwSetup    *setup
);

/** ============================================================================
 * @n@b   CSL_ddr2GetHwSetup
 *
 * @b Description
 * @n This function gets the current setup of the DDR2.  The status is
 *    returned through @a CSL_Ddr2HwSetup.  The obtaining of status
 *    is the reverse operation of @a CSL_ddr2HwSetup() function.
 * 
 * @b Arguments                                                              
 * @verbatim                                                                 
            hDdr2        Handle to the external memory interface instance
            setup        Pointer to setup structure which contains the
                         information to program DDR2 to a useful state
   @endverbatim                                                              
 *                                                                             
 * <b> Return Value </b>  CSL_Status
 * @li                    CSL_OK             - Hardware setup successful
 * @li                    CSL_ESYS_INVPARAMS - The param passed is invalid    
 * @li                    CSL_ESYS_BADHANDLE - Handle is not valid    
 *
 * <b> Pre Condition </b>
 *     Both @a CSL_ddr2Init() and @a CSL_ddr2Open() must be called successfully
 *     in order before calling @a CSL_ddr2GetHwSetup().
 *
 * <b> Post Condition </b>
 * @n  None  
 *
 * @b Modifies
 * @n Second parameter setup value
 *
 * @b Example:
 * @verbatim
      CSL_Ddr2Handle hDdr2;
      CSL_Status status;      
      CSL_Ddr2Timing1 tim1;
      CSL_Ddr2Timing2 tim2;
      CSL_Ddr2Settings set;
      CSL_Ddr2HwSetup hwSetup ;
     
     
     hwSetup.timing1Param = &tim1;
     hwSetup.timing2Param = &tim2;
     hwSetup.setParam = &set;
       ...
      status = CSL_ddr2GetHwSetup(hDdr2, &hwSetup);
   @endverbatim
 *
 * @return returns the status of the operation (see @a CSL_Status)
 *
 * ============================================================================
 */
CSL_Status  CSL_ddr2GetHwSetup (
    CSL_Ddr2Handle     hDdr2,
    CSL_Ddr2HwSetup    *setup
);

/** ============================================================================
 * @n@b   CSL_ddr2HwControl
 *
 * @b Description
 * @n Control operations for the DDR2.  For a particular control operation, the
 *  pointer to the corresponding data type needs to be passed as argument 
 *  HwControl function Call.  All the arguments (Structure elements included) 
 *  passed to  the HwControl function are inputs. For the list of commands 
 *  supported and argument type that can be @a void* casted & passed with a 
 *  particular command refer to @a CSL_Ddr2HwControlCmd.
 *
 *  @b Arguments
 *  @verbatim      
            hDdr2           Pointer to the peripheral data object of the
                            DDR2 external memory interface instance
 
            cmd             The command to this API indicates the action to be 
                            taken 
 
            arg             An optional argument @a void* casted
 
    @endverbatim
 *
 *  <b> Return Value </b>  CSL_Status
 *  @li                    CSL_OK             -  Command successful
 *  @li                    CSL_ESYS_BADHANDLE -  The handle passed is invalid
 *  @li                    CSL_ESYS_INVCMD    -  The Command passed is invalid      
 *
 *  <b> Pre Condition </b>
 *  Both @a CSL_ddr2Init() and @a CSL_ddr2Open() must be called successfully
 *  in order before calling @a CSL_ddr2HwControl(). For the argument type that 
 *  can be @a void* casted & passed with a particular command refer to 
 *  @a CSL_Ddr2HwControlCmd.
 *
 *  <b> Post Condition </b>
 *  @n  DDR2 registers are configured according to the command passed.
 *
 * @b Modifies
 * @n DDR2 registers
 *
 *  @b Example:
 *  @verbatim
       CSL_Ddr2Handle hDdr2;
       CSL_Status status;
       CSL_Ddr2SelfRefresh command;
       
       command = CSL_DDR2_SELF_REFRESH_DISABLE;
       ...
       status = CSL_ddr2HwControl(hDdr2,
                                  CCSL_DDR2_CMD_SELF_REFRESH,
                                  &command);
    @endverbatim
 *
 *
 * ============================================================================
 */
CSL_Status  CSL_ddr2HwControl (
    CSL_Ddr2Handle          hDdr2,
    CSL_Ddr2HwControlCmd    cmd,
    void                    *arg
);

/** ============================================================================
 * @n@b   CSL_ddr2GetHwStatus
 *
 * @b Description
 * @n This function is used to read the current device configuration, status 
 *  flags and the value present associated registers.  Following table details 
 *  the various status queries supported and the associated data structure to 
 *  record the response.  User should allocate memory for the said data type and
 *  pass its pointer as an unadorned void* argument to the status query call. 
 *  For details about the various status queries supported and the associated 
 *  data structure to record the response, refer to @a CSL_Ddr2HwStatusQuery
 *
 *   @b Arguments
 *   @verbatim      
            hDdr2           Pointer to the peripheral data object of the
                            DDR2 external memory interface instance
 
            query           The query to this API which indicates the status 
                            to be returned 
 
            response        Placeholder to return the status. @a void* casted
 
     @endverbatim
 *
 *  <b> Return Value </b>  CSL_Status
 *  @li                    CSL_OK             - Hardware status call is 
 *                                              successful
 *  @li                    CSL_ESYS_BADHANDLE - Not a valid Handle  
 *  @li                    CSL_ESYS_INVQUERY  - Invalid Query
 *
 * <b> Pre Condition </b>
 *  Both @a CSL_ddr2Init() and @a CSL_ddr2Open() must be called successfully
 *  in order before callinbg @a CSL_ddr2GetHwStatus(). For the argument type 
 *  that can be @a void* casted & passed with a particular command refer to 
 *  @a CSL_Ddr2HwStatusQuery
 *
 * <b> Post Condition </b>
 * @n  None
 *
 * @b Modifies
 * @n Third parameter, response vlaue 
 *
 * @b Example:
 * @verbatim
      CSL_Ddr2Handle hDdr2;
      CSL_Status status;
      Uint16  response;
       ...
      status = CSL_ddr2GetHwStatus(hDdr2,
                                   CSL_DDR2_QUERY_REFRESH_RATE,
                                   &response);
   @endverbatim
 *
 *
 * ============================================================================
 */
CSL_Status  CSL_ddr2GetHwStatus (
    CSL_Ddr2Handle           hDdr2,
    CSL_Ddr2HwStatusQuery    query,
    void                     *response
);


#ifdef __cplusplus
}
#endif

#endif

