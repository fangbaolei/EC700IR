/* ============================================================================
 * Copyright (c) Texas Instruments Inc 2002, 2003, 2004, 2005                 
 *                                                                              
 * Use of this software is controlled by the terms and conditions found in the
 * license agreement under which this software has been supplied.             
 * ============================================================================
 */ 

 /** ============================================================================
 *   @file  csl_cfg.h
 *
 *   @path  $(CSLPATH)\inc
 *
 *   @desc  Header file for functional layer of CFG CSL
 *
 */

/** ===========================================================================
 * @mainpage CFG Module
 *
 * @section Introduction
 *
 * @subsection xxx Purpose and Scope
 * The purpose of this document is to identify a set of common CSL APIs for
 * the CFG Module across various devices. The CSL developer
 * is expected to refer to this document while implementing APIs for these
 * modules. Some of the listed APIs may not be applicable to a given CFG
 *
 *
 *
 * @subsection aaa Terms and Abbreviations
 *   -# CSL     :  Chip Support Library
 *   -# API     :  Application Programmer Interface
 *   -# CFG     :  Configuration
 * ============================================================================
 */

/* ============================================================================
 * Revision History
 * ===============
 *  14-Apr-2005 Brn file created
 *  16-Nov-2005 ds  updated the documentation
 *  31-Jan-2006 ds  added  CSL_cfgGetbaseaddress () API and CSL_CfgBaseAddress
 *                  structure   
 * ============================================================================
 */

#ifndef _CSL_CFG_H_
#define _CSL_CFG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <csl.h>
#include <cslr_cfg.h>
#include <soc.h>

/** Mask value of Fault ID */
#define CSL_CFG_FAULT_STAT_FID (0x0000F700u)

/** Mask value to get the status of Local memory (L1/L2) */
#define CSL_CFG_FAULT_STAT_LOCAL (0x00000080u)

/** Mask value for Supervisor Read */
#define CSL_CFG_FAULT_STAT_SR (0x00000020u)

/** Mask value for Supervisor Write */
#define CSL_CFG_FAULT_STAT_SW (0x00000010u)

/** Mask value for Supervisor Execute */
#define CSL_CFG_FAULT_STAT_SX (0x00000008u)

/** Mask value for User Read */
#define CSL_CFG_FAULT_STAT_UR (0x00000004u)

/** Mask value for User Write */
#define CSL_CFG_FAULT_STAT_UW (0x00000002u)

/** Mask value for User Execute */
#define CSL_CFG_FAULT_STAT_UX (0x00000001u)


/** @brief Enumeration for queries passed to @a CSL_cfgHwControl()
 *
 * This is used to select the commands to control the operations
 * existing setup of CFG. The arguments to be passed with each
 * enumeration if any are specified next to the enumeration.
 */
typedef enum {
    /** @brief CFG Hardware control command to clears the error conditions 
     *         stored in MPFAR and MPFSR
     *  @param None
     */
    CSL_CFG_CMD_CLEAR = 1
} CSL_CfgHwControlCmd;


/** @brief Enumeration for queries passed to @a CSL_cfgGetHwStatus()
 *
 * This is used to get the status of different operations or to get the
 * existing setup of CFG.
 */
typedef enum {
    /**
     * @brief Status query command to get the Fault Address 
     * @param (Uint32 *)
     */
    CSL_CFG_QUERY_FAULT_ADDR = 1,
        
    /** 
     * @brief Status query command to get the Status information of 
     *        CSL_CfgStatus
     * @param (CSL_CfgFaultStatus *)
     */
    CSL_CFG_QUERY_FAULT_STATUS     = 2
} CSL_CfgHwStatusQuery;



/** @brief CSL_CfgStatus has all the fields required for the status information
 *         of CFG module.
 */
typedef struct {
    /** Bit Mask of the Errors */
    CSL_BitMask16  errorMask;
    
    /** Fault Id. The IDof the originator of the faulting access */
    Uint16  faultId;
} CSL_CfgFaultStatus;

/**
 * This will have the base-address information for the peripheral instance
 */
typedef struct {
    /** Base-address of the Configuration registers of CFG */
    CSL_CfgRegsOvly regs;
} CSL_CfgBaseAddress;

/**
 *  CFG specific parameters. Present implementation doesn't have
 *  any specific parameters.
 */
typedef struct {
    /** Bit mask to be used for module specific parameters.
     *  The below declaration is just a place-holder for future
     *  implementation.
     */
    CSL_BitMask16   flags;
} CSL_CfgParam;

/**
 *  CFG specific context information. Present implementation doesn't
 *  have any Context information.
 */
typedef struct {
    /** Context information of CFG.
     *  The below declaration is just a place-holder for future
     *  implementation.
     */
    Uint16  contextInfo;
} CSL_CfgContext;

/** @brief This object contains the reference to the instance of CFG
 *  opened using the @a CSL_cfgOpen()
 *
 *  The pointer to this is passed as CFG Handle to all CFG CSL APIs. 
 *  CSL_cfgOpen() function initializes this structure based on the parameters 
 *  passed 
 */
typedef struct CSL_CfgObj {
    /** This is a pointer to the registers of the instance of CFG
     *  referred to by this object
     */
    CSL_CfgRegsOvly regs;
    
    /** This is the instance of CFG being referred to by this object */
    CSL_InstNum cfgNum;
} CSL_CfgObj;

/** @brief this is a pointer to @a CSL_CfgObj & is passed as the first
 * parameter to all CFG CSL APIs
 */
typedef CSL_CfgObj *CSL_CfgHandle;


/*  ===========================================================================
 *   @n@b CSL_cfgOpen
 *
 *   @b Description
 *   @n  This function populates the peripheral data object for the instance
 *       and returns a handle to the instance.
 *       The open call sets up the data structures for the particular instance
 *       of CFG device. The device can be re-opened anytime after it has
 *       been normally closed, if so required. The handle returned by this call 
 *       is input as an essential argument for rest of the APIs described for
 *       this module. 
 *
 *   @b Arguments
 *   @verbatim
        pCfgObj         Pointer to the CFG instance object

        cfgNum          Instance of the CFG to be opened.

        pCfgParam       Pointer to module specific parameters

        pStatus         pointer for returning status of the function call
     @endverbatim
 *
 *   <b> Return Value </b> CSL_CfgHandle
 *   @n  Valid CFG instance handle will be returned if status value is
 *       equal to CSL_SOK.
 *
 *   <b> Pre Condition </b>
 *   @n  CSL_cfgInit has to be called defore this function get called
 *
 *   <b> Post Condition </b>
 *   @n  CSL_cfgInit has to be called before calling this function.
 *       Post Condition 
 *       1. The status is returned in the status variable. If status returned is 
 *              - CSL_SOK       - Valid CFG handle is returned. 
 *              - CSL_ESYS_FAIL - The CFG instance is invalid. 
 *       2. CFG object structure is populated.
 *
 *
 *   @b Modifies
 *   @n  1. The status variable 
 *       2. CFG object structure 
 *
 *   @b Example
 *   @verbatim
          CSL_status        status;
          CSL_CfgObj        cfgObj;
          CSL_CfgHandle     hCfg;
  
          hCfg = CSL_cfgOpen (&cfgObj, CSL_MEMPROT_CONFIG, NULL, &status);
     @endverbatim
 * ===========================================================================
 */
CSL_CfgHandle CSL_cfgOpen (
    /* Pointer to the handle object - to be allocated by the user */
    CSL_CfgObj   *hCfgObj,
    
    /* Specifies the instance of CFG to be opened */
    CSL_InstNum  cfgNum,
    
    /* Module specific parameters;
     * Currently there are none; the user should pass 'NULL'
     */
    CSL_CfgParam *pCfgParam,
    
    /* Pointer to the variable that holds the holds the status of the open
     * call
     */
    CSL_Status   *status
);


/*  ============================================================================
 *   @n@b CSL_cfgClose
 *
 *   @b Description
 *   @n This function closes the specified instance of CFG.
 *
 *  <b> Usage Constraints: </b>
 *      Both @a CSL_cfgInit() and @a CSL_cfgOpen() must be called successfully
 *      in that order before @a CSL_cfgClose() can be called.
 *
 *   @arg  hCfg             Handle to the CFG instance
 *
 *   <b> Return Value </b>  CSL_Status
 *                          CSL_SOK            - Close successful
 *                          CSL_ESYS_BADHANDLE - Invalid handle
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
       ...
       CSL_cfgClose(hCfg);
       ...  
    @endverbatim
 * =============================================================================
 */
CSL_Status CSL_cfgClose (
    /* Handle to the CFG instance */
    CSL_CfgHandle hCfg
);


/*  ============================================================================
 *   @n@b CSL_cfgHwControl
 *
 *   @b Description
 *   @n Takes a command of CFG with an optional argument & implements it.
 *
 *   @b Arguments
 *   @verbatim
            hCfg            Handle to the CFG instance

            cmd             The command to this API indicates the action to be
                            taken on CFG.

            arg             An optional argument.

     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li         CSL_SOK               - Status info return successful.
 *   @li         CSL_ESYS_INVCMD       - Invalid command
 *   @li         CSL_ESYS_INVPARAMS    - Invalid parameter
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n The registers of CFG.
 *
 *   @b Example
 *   @verbatim
        CSL_CfgHandle         hCfg;
        CSL_CfgHwControlCmd   cmd;
        void                  arg;
        
        ...
        status = CSL_cfgHwControl(hCfg, cmd, &arg);
        ...
     @endverbatim
 * =============================================================================
 */
CSL_Status CSL_cfgHwControl (
    /* Handle to the CFG instance */
    CSL_CfgHandle       hCfg,
    
    /* The command to this API which indicates the action to be taken */
    CSL_CfgHwControlCmd cmd,
    
    /* Optional argument @a void* casted */
    void                *cmdArg
);


/*  ============================================================================
 *   @n@b CSL_cfgInit
 *
 *   @b Description
 *   @n This is the initialization function for the CFG. This function is
 *      idempotent in that calling it many times is same as calling it once.
 *      This function initializes the CSL data structures, and doesn't affect
 *      the H/W.
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
        CSL_cfgInit();
        ...
    @endverbatim
 *  ===========================================================================
 */
CSL_Status CSL_cfgInit (
    /* CFG Context */
    CSL_CfgContext *pContext
);


/*  ============================================================================
 *   @n@b CSL_cfgGetHwStatus
 *
 *   @b Description
 *   @n Gets the status of the different operations of CFG.
 *
 *   @b Arguments
 *   @verbatim
            hCfg          Handle to the CFG instance

            query         The query to this API of CFG which indicates the
                          status to be returned.

            response      Placeholder to return the status.

     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK             - Status info return successful
 *   @li                    CSL_ESYS_INVQUERY   - Invalid query command
 *   @li                    CSL_ESYS_INVPARAMS  - Invalid parameter
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *      None
 *
 *   @b Example
 *   @verbatim
        CSL_CfgHandle          hCfg;
        CSL_CfgHwStatusQuery   query;
        void                   reponse;

        status = CSL_GetcfgHwStatus (hCfg, query, &response);

     @endverbatim
 * =============================================================================
 */
CSL_Status CSL_cfgGetHwStatus (
    /* Handle to the CFG instance */
    CSL_CfgHandle        hCfg,
    
    /* The query to this API which indicates the status/setup
     * to be returned
     */
    CSL_CfgHwStatusQuery myQuery,

    /* Placeholder to return the status; @a void* casted */
    void                 *response
);

/** ============================================================================
 *   @n@b CSL_cfgGetBaseAddress
 *
 *   @b Description
 *   @n  Function to get the base address of the peripheral instance.
 *       This function is used for getting the base address of the peripheral
 *       instance. This function will be called inside the CSL_cfgOpen()
 *       function call. This function is open for re-implementing if the user
 *       wants to modify the base address of the peripheral object to point to
 *       a different location and there by allow CSL initiated write/reads into
 *       peripheral. MMR's go to an alternate location.
 *
 *   @b Arguments
 *   @verbatim
            cfgNum          Specifies the instance of the CFG to be opened.

            pCfgParam       Module specific parameters.

            pBaseAddress    Pointer to baseaddress structure.

     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_OK            Successful on getting the base 
 *                                            address of CFG
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
        CSL_Status          status;
        CSL_CfgBaseAddress  baseAddress;

       ...
      status = CSL_cfgGetBaseAddress(CSL_MEMPROT_CONFIG, NULL, &baseAddress);

    @endverbatim
 * ===========================================================================
 */
CSL_Status   CSL_cfgGetBaseAddress (
        CSL_InstNum             cfgNum,
        CSL_CfgParam           *pCfgParam,
        CSL_CfgBaseAddress     *pBaseAddress
);

#ifdef __cplusplus
}
#endif

#endif
