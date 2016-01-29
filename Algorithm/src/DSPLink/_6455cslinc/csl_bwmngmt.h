/* ============================================================================
 * Copyright (c) Texas Instruments Inc 2002, 2003, 2004, 2005
 *
 * Use of this software is controlled by the terms and conditions found
 * in the license agreement under which this software has been supplied
 * provided
 * ============================================================================
 */
 
/** =========================================================================== 
 * @file csl_bwmngmt.h
 *
 * @brief Header file for functional layer of CSL
 *
 * @path $(CSLPATH)\bwmngmt\src
 * 
 * Description
 *    - The different enumerations, structure definitions
 *      and function declaration
 *
 *  @date 28th May, 2004
 *  @author Chad Courtney
 *  ===========================================================================
 */

/** =========================================================================== 
 * @mainpage BWMNGMT Module
 *
 * @section Introduction
 *
 * @subsection xxx Purpose and Scope
 * The purpose of this document is to identify a set of common CSL APIs for 
 * the BWMNGMT Module across various devices. The CSL developer
 * is expected to refer to this document while implementing APIs for these
 * modules . Some of the listed APIs may not be applicable to a given BWMNGMT  
 *
 *
 *
 * @subsection aaa Terms and Abbreviations
 *   -# CSL     :  Chip Support Library
 *   -# API     :  Application Programmer Interface
 *   -# BWMNGMT :  Bandwidth Management Module
 *
 * @subsection References
 *    -# CSL 3.x Technical Requirements Specifications Version 0.5, dated
 *       May 14th, 2003
 *    -# Bandwidth Management Specification (Revision 0.22)
 *
 * @subsection Assumptions
 *     The abbreviations CSL, BWMNGMT have been used throughout this
 *     document to refer to the C64Plus Bandwidth Management Module.
 * ============================================================================
 */
 
/* =============================================================================
 *  Revision History
 *  ===============
 *  11-Apr-2005 Brn updated the file for doxygen compatibiliy
 *
 *  16-Nov-2005 ds  updated the documentation
 * =============================================================================
 */

#ifndef _CSL_BWMNGMT_H_
#define _CSL_BWMNGMT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <csl.h>
#include <cslr_bwmngmt.h>
#include <soc.h>

    

/** Control Block Set for BWMNGMT 
 *  This is used to indicate which control blocks (L1D, L2, and/or EXT) are to 
 *  be set within BWMNGMT for the given requestor (CPU, IDMA, SLAP, MAP, UC) 
 *  arbitration settings.
 */
typedef enum {
    /** All controller blocks will be update with given requestors arbitration 
     *  setting 
     */
    CSL_BWMNGMT_BLOCK_ALL = 7,
    
    /** L1D controller block will be update with given requestors arbitration
     *  setting 
     */ 
    CSL_BWMNGMT_BLOCK_L1D = 1,
    
    /** L2 controller block will be update with given requestors arbitration 
     *  setting 
     */
    CSL_BWMNGMT_BLOCK_L2 = 2,

    /** EXT controller block will be update with given requestors arbitration 
     *  setting 
     */
    CSL_BWMNGMT_BLOCK_EXT = 4

} CSL_BwmngmtControlBlocks;


/** @brief Priority Settings for BWMNGMT 
 *
 * This is used to indicate to set the Priority arbitration settings for
 * the Requestors (CPU, IDMA, SLAP, MAP, UC)
 */
typedef enum {
    /** Priority arbitration setting 0 - Highest priority requestor */
    CSL_BWMNGMT_PRI_0 = CSL_BWMNGMT_CPUARBL1D_PRI_PRI0,
    
    /** Priority arbitration setting 1 - 2nd Highest priority requestor */
    CSL_BWMNGMT_PRI_1 = CSL_BWMNGMT_CPUARBL1D_PRI_PRI1,
    
    /** Priority arbitration setting 2 - 3rd Highest priority requestor */
    CSL_BWMNGMT_PRI_2 = CSL_BWMNGMT_CPUARBL1D_PRI_PRI2,
    
    /** Priority arbitration setting 3 - 4th Highest priority requestor */
    CSL_BWMNGMT_PRI_3 = CSL_BWMNGMT_CPUARBL1D_PRI_PRI3,
    
    /** Priority arbitration setting 4 - 5th Highest priority requestor */
    CSL_BWMNGMT_PRI_4 = CSL_BWMNGMT_CPUARBL1D_PRI_PRI4,
    
    /** Priority arbitration setting 5 - 6th Highest priority requestor */
    CSL_BWMNGMT_PRI_5 = CSL_BWMNGMT_CPUARBL1D_PRI_PRI5,
    
    /** Priority arbitration setting 6 - 7th Highest priority requestor */
    CSL_BWMNGMT_PRI_6 = CSL_BWMNGMT_CPUARBL1D_PRI_PRI6,
    
    /** Priority arbitration setting 7 - Lowest priority requestor */
    CSL_BWMNGMT_PRI_7 = CSL_BWMNGMT_CPUARBL1D_PRI_PRI7,

    /** Priority arbitration setting NULL - Due Not Program PRIORITY for this 
     *  requestor 
     */
    CSL_BWMNGMT_PRI_NULL = -1
} CSL_BwmngmtPriority;


/** @brief Maxwait Settings for BWMNGMT 
 *
 *  This is used to indicate to set Maxwait arbitration settings for
 *  the Requestors (CPU, IDMA, SLAP, MAP, UC)
 */
typedef enum {
    /** Maxwait arbitration setting 0 - Always stall due to higher priority 
     *  requestor 
     */
    CSL_BWMNGMT_MAXWAIT_0 = CSL_BWMNGMT_CPUARBL1D_MAXWAIT_MAXWAIT0,
    
    /** Maxwait arbitration setting 1 - Stall max of 1 cycle due to higher 
     *  priority requestor 
     */
    CSL_BWMNGMT_MAXWAIT_1 = CSL_BWMNGMT_CPUARBL1D_MAXWAIT_MAXWAIT1,
    
    /** Maxwait arbitration setting 2 - Stall max of 2 cycle due to higher 
     *  priority requestor 
     */
    CSL_BWMNGMT_MAXWAIT_2 = CSL_BWMNGMT_CPUARBL1D_MAXWAIT_MAXWAIT2,
    
    
    /** Maxwait arbitration setting 4 - Stall max of 4 cycle due to higher 
     *  priority requestor 
     */
    CSL_BWMNGMT_MAXWAIT_4 = CSL_BWMNGMT_CPUARBL1D_MAXWAIT_MAXWAIT4,
    
    /** Maxwait arbitration setting 8 - Stall max of 8 cycle due to higher 
     *  priority requestor 
     */
    CSL_BWMNGMT_MAXWAIT_8 = CSL_BWMNGMT_CPUARBL1D_MAXWAIT_MAXWAIT8,
    
    /** Maxwait arbitration setting 16 - Stall max of 16 cycle due to higher 
     *  priority requestor 
     */
    CSL_BWMNGMT_MAXWAIT_16 = CSL_BWMNGMT_CPUARBL1D_MAXWAIT_MAXWAIT16,
    
    /** Maxwait arbitration setting 32 - Stall max of 32 cycle due to higher 
     *  priority requestor 
     */
    CSL_BWMNGMT_MAXWAIT_32 = CSL_BWMNGMT_CPUARBL1D_MAXWAIT_MAXWAIT32,
    
    /** Maxwait arbitration setting NULL - Due Not Program MAXWAIT for this 
     *  requestor 
     */
    CSL_BWMNGMT_MAXWAIT_NULL = -1
} CSL_BwmngmtMaxwait;


/** Enumaration for Hardware status query */
typedef enum {
    PLACEHOLDER0 = 0
} CSL_BwmngmtHwStatusQuery;


/** Enumaration for Hardware control command */
typedef enum {
    PLACEHOLDER2 = 0 
} CSL_BwmngmtHwControlCmd;


/** @brief CSL_BwmngmtHwSetup has all the fields required to configure BWMNGMT
 * 
 * This structure has the substructures required to configure BWMNGMT at 
 * Power-Up/Reset. 
 *
 */
typedef struct {
    /** CPU - Requestor Arbitration Settings - PRI */   
    CSL_BwmngmtPriority cpuPriority;       
    
    /** CPU - Requestor Arbitration Settings - MAXWAIT */   
    CSL_BwmngmtMaxwait cpuMaxwait;
    
    /** IDMA (Internal DMA) Requestor Arbitration Settings - MAXWAIT */   
    CSL_BwmngmtMaxwait idmaMaxwait;          
    
    /** SLAP (Slave Port) Requestor Arbitration Settings - MAXWAIT */   
    CSL_BwmngmtMaxwait slapMaxwait;          
    
    /** MAP (Master Port) Requestor Arbitration Settings - PRI */   
    CSL_BwmngmtPriority mapPriority;          
    
    /** UC (User Coherence) Requestor Arbitration Settings - MAXWAIT */   
    CSL_BwmngmtMaxwait ucMaxwait;          
    
    /** Controller(s) to be set with Requestors Settings        
     *  L1D, L2 and/or EXT                                        
     */
    CSL_BwmngmtControlBlocks control;
} CSL_BwmngmtHwSetup;

/*
The following #define CSL_BWMNGMT_HWSETUP_DEFAULTS is meant to simplify the
implementation in C code by the customer.

CSL_BwmngmtHwSetup defaultSetup;
    defaultSetup.cpuPriority    = CSL_BWMNGMT_CPUARBL1D_PRI_RESETVAL;
    defaultSetup.cpuMaxwait     = CSL_BWMNGMT_CPUARBL1D_MAXWAIT_RESETVAL;
    defaultSetup.idmaMaxwait    = CSL_BWMNGMT_IDMAARBL2_MAXWAIT_RESETVAL;
    defaultSetup.slapMaxwait    = CSL_BWMNGMT_SLAPARBL1D_MAXWAIT_RESETVAL;
    defaultSetup.mapPriority    = CSL_BWMNGMT_MAPARBEXT_PRI_RESETVAL;
    defaultSetup.ucMaxwait      = CSL_BWMNGMT_UCARBL1D_MAXWAIT_RESETVAL;
    defaultSetup.control        = CSL_BWMNGMT_BLOCK_ALL;

*/ 
 
/* Default Setup Structure */
/** 
 * The following #define CSL_BWMNGMT_HWSETUP_DEFAULTS is meant to simplify the
 * implementation in C code by the customer
 */
#define CSL_BWMNGMT_HWSETUP_DEFAULTS {  \
           (CSL_BwmngmtPriority)CSL_BWMNGMT_CPUARBL1D_PRI_RESETVAL, \
           (CSL_BwmngmtMaxwait)CSL_BWMNGMT_CPUARBL1D_MAXWAIT_RESETVAL,  \
           (CSL_BwmngmtMaxwait)CSL_BWMNGMT_IDMAARBL2_MAXWAIT_RESETVAL,  \
           (CSL_BwmngmtMaxwait)CSL_BWMNGMT_SLAPARBL1D_MAXWAIT_RESETVAL,  \
           (CSL_BwmngmtPriority)CSL_BWMNGMT_MAPARBEXT_PRI_RESETVAL,  \
           (CSL_BwmngmtMaxwait)CSL_BWMNGMT_UCARBL1D_MAXWAIT_RESETVAL,  \
           (CSL_BwmngmtControlBlocks)CSL_BWMNGMT_BLOCK_ALL  \
           }


/**
 * This will have the base-address information for the peripheral instance
 */
typedef struct {
    /** Base-address of the Configuration registers of BWMNGMT */
    CSL_BwmngmtRegsOvly regs;
} CSL_BwmngmtBaseAddress;

/**
 *  BWMNGMT specific parameters. Present implementation doesn't have
 *  any specific parameters.
 */
typedef struct {
    /** Bit mask to be used for module specific parameters.
     *  The below declaration is just a place-holder for future
     *  implementation.
     */
    CSL_BitMask16   flags;
} CSL_BwmngmtParam;

/**
 *  BWMNGMT specific context information. Present implementation doesn't
 *  have any Context information.
 */
typedef struct {
    /** Context information of BWMNGMT.
     *  The below declaration is just a place-holder for future
     *  implementation.
     */
    Uint16  contextInfo;
} CSL_BwmngmtContext;

/** @brief This object contains the reference to the instance of BWMNGMT
 *  opened using the @a CSL_bwmngmtOpen(). The pointer to this object is passed 
 *  as BWMNGMT handle to all BWMNGMT CSL APIs. . CSL_bwmngmtOpen() function 
 *  initializes this structure based on the parameters passed. 
 */
typedef struct {
    /** This is a pointer to the registers of the instance of BWMNGMT
     * referred to by this object */
    CSL_BwmngmtRegsOvly regs;
    
    /** This is the instance of BWMNGMT being referred to by this object */
    CSL_InstNum bwmngmtNum;
} CSL_BwmngmtObj;


/** @brief this is a pointer to @a CSL_BwmngmtObj & is passed as the first
 * parameter to all BWMNGMT CSL APIs 
 */
typedef CSL_BwmngmtObj *CSL_BwmngmtHandle;

/** ===========================================================================
 *  @n@b CSL_bwmngmtOpen
 *
 *  @b Description
 *       This function populates the peripheral data object for the instance
 *       and returns a handle to the instance.
 *       The open call sets up the data structures for the particular instance
 *       of BWMNGMT device. The device can be re-opened anytime after it has
 *       been normally closed if so required. BWMNGMT Hardware setup will be
 *       performed at the end of the open call only if the HwSetup Pointer
 *       supplied was non- NULL. The handle returned by this call is input as
 *       an essential argument for rest of the APIs described for this module.

 *
 *  @b Arguments
 *  @verbatim
        pBwmngmtObj       Pointer to the BWMNGMT instance object

        bwmngmtNum        Instance of the BWMNGMT to be opened.

        pBwmngmtParam     Pointer to module specific parameters

        pStatus           pointer for returning status of the function call

    @endverbatim
 *
 *  <b> Return Value </b> CSL_BwmngmtHandle
 *      Valid BWMNGMT instance handle will be returned if status value is
 *      equal to CSL_SOK.
 *
 *  <b> Pre Condition </b>
 *  @n  The CSL system as well as BWMNGMT must be succesfully initialized
 *      via @a CSL_sysInit() and @a CSL_bwmngmtInit() before calling this
 *      function. Memory for the @a CSL_BwmngmtObj must be allocated outside
 *      this call. This object must be retained while usage of this peripheral.
 *
 *  <b> Post Condition </b>
 *  @n  None
 *
 *  @b  Modifies
 *  @n  None
 *
 *  @b Example
 *  @verbatim
        CSL_status            status;
        CSL_BwmngmtObj        bwmngmtObj;
        CSL_BwmngmtHandle     hBwmngmt;
        
        ...
        hBwmngmt = CSL_bwmngmtOpen(&bwmngmtObj, CSL_BWMNGMT_0, NULL, &status);
        ...
    @endverbatim
 * ============================================================================
 */
CSL_BwmngmtHandle CSL_bwmngmtOpen (
    /* Pointer to the handle object - to be allocated by the user */
    CSL_BwmngmtObj   *hBwmngmtObj,
    
    /* Specifies the instance of BWMNGMT to be opened */
    CSL_InstNum      bwmngmtNum,
    
    /* Module specific parameters;
     * Currently there are none; the user should pass 'NULL'
     */
    CSL_BwmngmtParam *pBwmParam,
    
    /* Pointer to the variable that holds the holds the status of the open
     * call 
     */
    CSL_Status       *status
);


/*  ===========================================================================
 *   @n@b CSL_bwmngmtOpen
 *
 *   @b Description
 *        This function populates the peripheral data object for the instance
 *        and returns a handle to the BWMNGMT instance.
 *        The open call sets up the data structures for the particular instance
 *        of BWMNGMT device. The device can be re-opened anytime after it has
 *        been normally closed, if so required.The handle returned by this call 
 *        is input as an essential argument for rest of the APIs described for 
 *        this module.
 *
 *   @b Arguments
 *   @verbatim
  
        pBwmngmtObj       Pointer to the BWMNGMT instance object

        bwmngmtNum        Instance of the BWMNGMT to be opened.

        pBwmngmtParam     Pointer to module specific parameters

        pStatus           pointer for returning status of the function call

     @endverbatim
 *
 *   <b> Return Value </b> CSL_BwmngmtHandle
 *       Valid BWMNGMT instance handle will be returned if status value is
 *       equal to CSL_SOK.
 *
 *   <b> Pre Condition </b>
 *   @n  The BWMNGMT module must be successfully initialized via 
 *       CSL_bwmngmtInit() before calling this function. 
 *
 *   <b> Post Condition </b>
 *   @n  1. The status is returned in the status variable. If status returned is 
 *			CSL_SOK - Valid BWMNGMT handle is returned. 
 *			CSL_ESYS_FAIL - The BWMNGMT instance is invalid. 
 *   	 2. BWMNGMT object structure is populated.
 *
 *
 *   @b  Modifies
 *   @n  1. The status variable 
 *		 2. BWMNGMT object structure 
 *
 *  @b Example
 *  @verbatim
        ...
        CSL_bwmngmtClose(hBwmngmt);
        ...
    @endverbatim
 * ============================================================================
 */
CSL_Status CSL_bwmngmtClose (
    /* Pointer to the object that holds reference to the
     * instance of BWMNGMT requested after the call 
     */
    CSL_BwmngmtHandle hBwmngmt
);


/*  ============================================================================
 *  @n@b CSL_bwmngmtHwSetup
 *
 *   @b Description
 *   @n Configures the BWMNGMT using the values passed in through the
 *      setup structure.
 *
 *   @b Arguments
 *   @verbatim

         hBwmngmt            Handle to the BWMNGMT instance

         setup               Setup structure for BWMNGMT

     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *                          CSL_SOK            - Close successful
 *                          CSL_ESYS_BADHANDLE - Invalid handle
 *                          CSL_ESYS_INVPARAMS - If setup is NULL
 *
 *   <b> Pre Condition </b>
 *   @n  Both @a CSL_bwmngmtInit() and @a CSL_bwmngmtOpen() must be called
 *       successfully in that order before this function can be called. The
 *       main setup structure consists of fields used for the configuration at
 *       start up. The user must allocate space for it and fill in the main
 *       setup structure fields appropriately before a call to this function
 *       is made. \n
 *
 *   <b> Post Condition </b>
 *   @n  BWMNGMT registers are configured according to the hardware setup 
 *       parameters
 *
 *   @b  Modifies
 *   @n  The following registers and fields are programmed by this API \n
 *   1.  CPU Arbitration Parameters \n
 *       -   PRI field set in L1D, L2 and/or EXT \n
 *       -   MAXWAIT field set in L1D, L2 and/or EXT \n
 *
 *   2.  IDMA Arbitration Parameter \n
 *       -   MAXWAIT field set in L1D, L2 and/or EXT \n
 *
 *   3.  SLAP Arbitration Parameter \n
 *       -   MAXWAIT field set in L1D, L2 and/or EXT \n
 *
 *   4.  MAP Arbitration Parameter \n
 *       -   PRI field set in EXT \n
 *
 *   5.  UC Arbitration Parameter \n
 *       -   MAXWAIT field set in L1D and/or L2 \n
 *
 *  The @b control: bitmask indicates which of the three control blocks
 *  (L1D, L2 and EXT) will be set with the associated PRI and MAXWAIT values 
 *  Note: That if associated control block is not programmable for given 
 *  requestor then it will not ignored but no error will be provide.  This 
 *  allows the user to set control to CSL_BWMNGMT_BLOCK_ALL which is the 
 *  default value. This will set all programmed arbitration values for a given 
 *  requestor to the same value across \n the blocks which is recommended.
 *  If PRI is set to CSL_BWMNGMT_PRI_NULL (-1) then no change will be made 
 *  for the corresponding requestors priority level.
 *  If MAXWAIT is set to CSL_BWMNGMT_MAXWAIT_NULL (-1) then no change will be 
 *  made for the corresponding requestors maxwait setting.

 *  @b Examples:
 *  @verbatim

    Example 1: Sets Priorities and Maxwaits to default values

        CSL_BwmngmtHandle hBwmngmt;
        CSL_BwmngmtHwSetup hwSetup;
        hwSetup = CSL_BWMNGMT_HWSETUP_DEFAULTS;
        ...

        // Init Successfully done
        ...
        // Open Successfully done
        ...
        CSL_bwmngmtHwSetup(hBwmngmt, &hwSetup);


    Example 2: Sets CPU Priority to 1, CPU Maxwait to 8, MAP Priority to 6 for 
    all blocks (L1D, L2 and EXT)

        CSL_BwmngmtHandle hBwmngmt;
        CSL_BwmngmtHwSetup hwSetup;
        hwSetup.cpuPriority = CSL_BWMNGMT_PRI_1;
        hwSetup.cpuMaxwait  = CSL_BWMNGMT_MAXWAIT_8;
        hwSetup.idmaMaxwait = CSL_BWMNGMT_MAXWAIT_NULL;
        hwSetup.slapMaxwait = CSL_BWMNGMT_MAXWAIT_NULL;
        hwSetup.mapPriority = CSL_BWMNGMT_PRI_6;
        hwSetup.ucMaxwait   = CSL_BWMNGMT_MAXWAIT_NULL;
        hwSetup.control     = CSL_BWMNGMT_BLOCK_ALL;
        ...

        // Init Successfully done
        ...
        // Open Successfully done
        ...
        CSL_bwmngmtHwSetup(hBwmngmt, &hwSetup);
     @endverbatim
 *
 *  ===========================================================================
 */
CSL_Status CSL_bwmngmtHwSetup (
    /* Pointer to the object that holds reference to the
     * instance of BWMNGMT requested after the call
     */
    CSL_BwmngmtHandle  hBwmngmt,
    
    /* Pointer to setup structure which contains the
     * information to program BWMNGMT to a useful state
     */
    CSL_BwmngmtHwSetup *setup
);


/*  ============================================================================
 *  @n@b CSL_bwmngmtGetHwSetup
 *
 *   @b Description
 *   @n Gets the current set up of BWMNGMT
 *
 *  @b Arguments
 *  @verbatim
          hBwmngmt            Handle to the BWMNGMT instance

          setup               Setup structure for BWMNGMT

      @endverbatim
 *
 *  <b> Return Value </b>  CSL_Status
 *  					   CSL_SOK            - Close successful
 *  					   CSL_ESYS_BADHANDLE - Invalid handle
 *  					   CSL_ESYS_INVPARAMS - IF setup is NULL
 *
 *  <b> Pre Condition </b>
 *  @n Both CSL_bwmngmtInit() and CSL_bwmngmtOpen() must be called
 *     successfully in that order before this function can be called.
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b  Modifies
 *   @n 
 *   1.  CPU Arbitration Parameters 
 *       -   PRI field set in Control Block Specified by "control" \n
 *       -   MAXWAIT field set in Control Block Specified by "control"  \n
 *
 *   2.  IDMA Arbitration Parameter \n
 *       -   MAXWAIT field set in Control Block Specified by "control"  \n
 *
 *   3.  SLAP Arbitration Parameter \n
 *       -   MAXWAIT field set in Control Block Specified by "control"  \n
 *
 *   4.  MAP Arbitration Parameter \n
 *       -   PRI field set in Control Block Specified by "control"  \n
 *       if not EXT then returns CSL_BWMNGMT_PRI_NULL \n
 *
 *   5.  UC Arbitration Parameter \n
 *       -   MAXWAIT field set in Control Block Specified by "control" \n
 *       if not L1D or L2 then returns CSL_BWMNGMT_MAXWAIT_NULL \n
 *
 *  @b Example:
 *  @verbatim

        CSL_BwmngmtHandle hBwmngmt;
        CSL_BwmngmtHwSetup hwSetup;
        hwSetup.control = CSL_BWMNGMT_BLOCK_L1D;
        // only CSL_BWMNGMT_BLOCK_L1D, CSL_BWMNGMT_BLOCK_L2, or
        // CSL_BWMNGMT_BLOCK_EXT are valid
        ...

        // Init Successfully done
        ...
        // Open Successfully done
        ...
        CSL_bwmngmtGetHwSetup(hBwmngmt, &hwSetup);

    @endverbatim
 *  ===========================================================================
 */
CSL_Status CSL_bwmngmtGetHwSetup (
    /* Pointer to the object that holds reference to the
     * instance of BWMNGMT requested after the call
     */
    CSL_BwmngmtHandle  hBwmngmt,

    /* Placeholder to return the status; @a CSL_BwmngmtHwSetup* */
    CSL_BwmngmtHwSetup *mySetup
);


/*  ============================================================================
 *  @n@b CSL_bwmngmtHwControl
 *
 *  @b Description
 *  @n Takes a command of BWMNGMT with an optional argument & implements it.
 *     Not Implemented. For future use
 *
 *  @b Arguments
 *  @verbatim
           hBwmngmt        Handle to the BWMNGMT instance

           cmd             The command to this API indicates the action to be
                            taken on BWMNGMT.

           arg             An optional argument.

    @endverbatim
 *
 *  <b> Return Value </b>  CSL_Status
 *  @li         CSL_SOK               - Control info return successful.
 *  @li         CSL_ESYS_BADHANDLE    - Invalid handle
 *  @li         CSL_ESYS_INVCMD       - Invalid command
 *
 *  <b> Pre Condition </b>
 *   @n  None
 *
 *  <b> Post Condition </b>
 *  @n  None
 *
 *  @b Modifies
 *  @n The hardware registers of BWMNGMT.
 *
 *  @b Example
 *  @verbatim
        CSL_BwmngmtHandle         hBwmngmt;
        CSL_BwmngmtHwControlCmd   cmd;
        void                      arg;
        
        ...
        status = CSL_bwmngmtHwControl (hBwmngmt, cmd, &arg);
        ...
     @endverbatim
 * =============================================================================
 */
CSL_Status CSL_bwmngmtHwControl (
    /* Pointer to the object that holds reference to the
     * instance of BWMNGMT requested after the call
     */
    CSL_BwmngmtHandle       hBwmngmt,
    
    /* The command to this API which indicates the action to be taken */
    CSL_BwmngmtHwControlCmd cmd,
    
    /* Optional argument @a void* casted */
    void                    *cmdArg
);


/*  ============================================================================
 *   @n@b CSL_bwmngmtInit
 *
 *   @b Description
 *   @n This is the initialization function for the BWMNGMT. This function must
 *      be called before calling any other API from this CSL.This function is
 *		idem-potent. Currently, the function just returns status CSL_SOK, 
 *      without doing anything.
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
 *   @verbatim
       ...
          CSL_bwmngmtInit();
     @endverbatim
 *  ===========================================================================
 */
CSL_Status CSL_bwmngmtInit (
    /* BWMNGMT Context */
    CSL_BwmngmtContext *pContext
);


/*  ============================================================================
 *  @n@b CSL_bwmngmtGetHwStatus
 *
 *  @b Description
 *  @n Gets the status of the different operations of BWMNGMT. Not Implemented.
 *     For future use
 *
 *  @b Arguments
 *  @verbatim
           hBwmngmt      Handle to the BWMNGMT instance

           query         The query to this API of BWMNGMT which indicates the
                         status to be returned.

           response      Placeholder to return the status.
    @endverbatim
 *
 *  <b> Return Value </b>  CSL_Status
 *  @li                    CSL_SOK             - Status info return successful
 *  @li         			CSL_ESYS_INVQUERY   - Invalid query
 *
 *  <b> Pre Condition </b>
 *  @n  None
 *
 *  <b> Post Condition </b>
 *  @n  None
 *
 *  @b Modifies
 *  @n None
 *
 *  @b Example
 *  @verbatim
       CSL_BwmngmtHandle          hBwmngmt;
       CSL_BwmngmtHwStatusQuery   query;
       void                       reponse;
       
       ...
       status = CSL_GetbwmngmtHwStatus(hBwmngmt, query, &response);
       ...
     @endverbatim
 * =============================================================================
 */
CSL_Status CSL_bwmngmtGetHwStatus (
    /* Pointer to the object that holds reference to the
     * instance of BWMNGMT requested after the call
     */
    CSL_BwmngmtHandle        hBwmngmt,
    
    /* The query to this API which indicates the status/setup
     * to be returned
     */
    CSL_BwmngmtHwStatusQuery myQuery,
    
    /* Placeholder to return the status; @a void* casted */
    void                     *response
);

/** ============================================================================
 *   @n@b CSL_bwmngmtGetBaseAddress
 *
 *   @b Description
 *   @n  Function to get the base address of the peripheral instance.
 *       This function is used for getting the base address of the peripheral
 *       instance. This function will be called inside the CSL_bwmngmtOpen()
 *       function call. This function is open for re-implementing if the user
 *       wants to modify the base address of the peripheral object to point to
 *       a different location and there by allow CSL initiated write/reads into
 *       peripheral. MMR's go to an alternate location.
 *
 *   @b Arguments
 *   @verbatim
            bwmngmtNum        Specifies the instance of the bwmngmt to be opened.

            pBwmngmtParam     Module specific parameters.

            pBaseAddress      Pointer to baseaddress structure containing base
                              address details.

     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_OK            Successful on getting the base 
 *                                            address of bwmngmt
 *   @li                    CSL_ESYS_FAIL     The instance number is invalid.
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  Base Address structure is populated
 *
 *   @b Modifies
 *   @n    1. The status variable
 *
 *         2. Base address structure is modified.
 *
 *   @b Example
 *   @verbatim
        CSL_Status              status;
        CSL_BwmngmtBaseAddress  baseAddress;

       ...
      status = CSL_bwmngmtGetBaseAddress(CSL_BWMNGMT, NULL, &baseAddress);

     @endverbatim
 * ===========================================================================
 */

#ifdef __cplusplus
}
#endif

#endif


