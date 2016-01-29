/* ============================================================================
 * Copyright (c) Texas Instruments Inc 2002, 2003, 2004, 2005
 *
 * Use of this software is controlled by the terms and conditions found in the
 * license agreement under which this software has been supplied.
 * ============================================================================
 */

/** =========================================================================== 
 * @mainpage PWRDOWN CSL 3.x
 *
 * @section Introduction
 *
 * @subsection xxx Purpose and Scope
 * The purpose of this document is to describe the common API's and data
 * structures for the power Down Module module.
 *
 * @subsection aaa Terms and Abbreviations
 *   -# CSL:  Chip Support Library
 *   -# API:  Application Programmer Interface
 *
 * @subsection References
 *    -# CSL-001-DES, CSL 3.x Design Specification DocumentVersion 1.02
 * ===========================================================================
 */

/** =========================================================================== 
 * @file csl_pwrdwn.h
 *
 * @brief Header file for functional layer of CSL
 *
 * @path $(CSLPATH)\inc
 *
 * Description
 *    - The different enumerations, structure definitions
 *      and function declarations
 *
 * Modification 1
 *    - modified on: 07/17/2004
 *    - reason: Created
 * @date 17th July, 2004
 * @author Ruchika Kharwar
 * ===========================================================================
 */             

#ifndef _CSL_PWRDWN_H_
#define _CSL_PWRDWN_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <csl.h>
#include <soc.h>
#include <cslr_pwrdwnPdc.h>
#include <cslr_pwrdwnL2.h>

/**************************************************************************\
* PWRDWN global macro declarations
\**************************************************************************/


/** @brief Enumeration for queries passed to @a CSL_pwrdwnGetHwStatus()
 *
 * This is used to get the status of different operations or to get the
 * existing setup of PWRDWN.
 */
typedef enum {
    /** Gets the page0 sleep status 
     *  @param (CSL_PwrdwnPortData *)
     */
    CSL_PWRDWN_QUERY_PAGE0_STATUS, 
    
    /** Gets the page1 sleep status 
     *  @param (CSL_PwrdwnPortData *) 
     */
    CSL_PWRDWN_QUERY_PAGE1_STATUS  
} CSL_PwrdwnHwStatusQuery;


/** @brief Enumeration for queries passed to @a CSL_pwrdwnHwControl()
 *
 * This is used to select the commands to control the operations
 * existing setup of PWRDWN. The arguments to be passed with each
 * enumeration if any are specified next to the enumeration.
 */
 typedef enum {
    /** manual power down, port0 or port1, page0 sleep 
     *  @param (CSL_PwrdwnPortData *)
     */
    CSL_PWRDWN_CMD_PAGE0_SLEEP,
    
    /** manual power down, port0 or port1, page1 sleep 
     * @param (CSL_PwrdwnPortData *)
     */
    CSL_PWRDWN_CMD_PAGE1_SLEEP,
    
    /** manual power down, port0 or port1, page0 wake
     * @param (CSL_PwrdwnPortData *)
     */
    CSL_PWRDWN_CMD_PAGE0_WAKE,
    
    /** manual power down, port0 or port1, page1 wake
     * @param (CSL_PwrdwnPortData *)
     */
    CSL_PWRDWN_CMD_PAGE1_WAKE
} CSL_PwrdwnHwControlCmd;



/**************************************************************************\
* PWRDWN global typedef declarations
\**************************************************************************/

/** @brief This object contains the reference to the instance of PWRDWN opened
 *  using the @a CSL_pwrdwnOpen().
 */
typedef struct {
    /** This is a pointer to the registers of the instance of PDC
     *  referred to by this object
     */
    CSL_PdcRegsOvly pdcRegs; 
    
    /** This is a pointer to the registers of the instance of L2 PWRDWN
     *  referred to by this object
     */
    CSL_L2pwrdwnRegsOvly l2pwrdwnRegs;
    
    /** This is the instance of PWRDWN being referred to by this object  */
    CSL_InstNum instNum;
} CSL_PwrdwnObj;


/** @brief Pointer to the powerdown  object. This handle contains the reference to the
 *  instance of PWRDWN opened @a CSL_pwrdwnOpen(). 
 *
 *  The pointer to this, is passed to all PWRDWN CSL APIs.
 */
typedef CSL_PwrdwnObj *CSL_PwrdwnHandle;


/** @brief This will have the base-address information for the module
 *  instance
 */
typedef struct {
    /** Base-address of the PDC registers */
    CSL_PdcRegsOvly regs;
    
    /** Base-address of the L2 Powerdown registers */
    CSL_L2pwrdwnRegsOvly l2pwrdwnRegs;
} CSL_PwrdwnBaseAddress;


/** @brief Module specific parameters. None in this implementation.
 */
typedef struct{
    /** Perhaps useful for future use */
    void *futureUse;
} CSL_PwrdwnParam;


/** @brief Module specific context information. Present implementation doesn't have
 *  any Context information.
 */
typedef struct {
    /** Context information of PWRDWN.
     *  The below declaration is just a place-holder for future
     *  implementation. This is a Dummy.
     */
    Uint16 contextInfo;
} CSL_PwrdwnContext;


typedef struct {
    /** port number */
    Bool  portNum;
    
    /** 8-bit mask */
    CSL_BitMask8 data;
} CSL_PwrdwnPortData;


/**
 * @brief   The manual powerdown setup structure 
 *
 */
typedef struct {
    /** Bitmask of the pages that need to be woken on UMAP0 */
    CSL_BitMask8 port0PageWake; 
    
    /** Bitmask of the pages that need to be woken on UMAP1 */
    CSL_BitMask8 port1PageWake;
    
    /** Bitmask of the pages that need to be put to sleep on UMAP0 */
    CSL_BitMask8 port0PageSleep; 
    
    /** Bitmask of the pages that need to be put to sleep on UMAP1 */
    CSL_BitMask8 port1PageSleep; 
} CSL_PwrdwnL2Manual;


/** @brief This has all the fields required to configure PWRDWN at Power Up
 *  (After a Hardware Reset) or a Soft Reset
 *
 *  This structure is used to setup or obtain existing setup of
 *  PWRDWN using @a CSL_pwrdwnHwSetup() & @a CSL_pwrdwnGetHwSetup() functions
 *  respectively.
 */
typedef struct {
    /** idle powerdown */
    Bool idlePwrdwn;
    
    /** Manual power down setup */
    CSL_PwrdwnL2Manual *manualPwrdwn;
} CSL_PwrdwnHwSetup;


/**
 * @brief   The config-structure
 *
 * Used to configure the PWRDWN using CSL_pwrdwnHwSetupRaw(..)
 */
typedef struct {
    /**< PDCCMD register */
    Uint32 PDCCMD;         
        
    /**< Per page manual awake port0 */
    Uint32 L2PDWAKE0; 
    
    /**< Per page manual awake for port1 */
    Uint32 L2PDWAKE1; 
    
    /**< Per page manual sleep for port0 */
    Uint32 L2PDSLEEP0;
    
    /**< Per page manual sleep for port1 */
    Uint32 L2PDSLEEP1;
} CSL_PwrdwnConfig;


/**************************************************************************\
* PWRDWN global function declarations
\**************************************************************************/

/**
 * ============================================================================
 *  @brief  Initializes the module
 *
 *  CSL_pwrdwnInit(..) initializes the PWRDWN module. 
 *  This function is idempotent -- calling it several times would
 *  have the same effect as calling it the first time.
 *  This function initializes the CSL data structures, and doesn't
 *  touches the hardware.
 * 
 *  @b Arguments
 *  @verbatim
       pContext    Pointer to module-context. As PWRDWN doesn't have any 
                   context based information user is expected to pass NULL.
    @endverbatim
 *
 *  <b> Return Value </b>  CSL_Status
 *  @li                        CSL_SOK - Always returns
 * 
 *  <b> Pre Condition </b>
 *  @n  None
 *
 *  <b> Post Condition </b>
 *  @n  The PWRDWN module is initialized
 *
 *  @b Modifies
 *  @n  None
 *  
 *  Note: As PWRDWN doesn't have any context based information, the function 
 *        just returns CSL_SOK. User is expected to pass NULL.
 *
 * @b Example
 * @verbatim
    ...
    if (CSL_SOK != CSL_pwrdwnInit(NULL)) {
       return;
    }
    @endverbatim
 * ============================================================================
 */
CSL_Status CSL_pwrdwnInit (
    /* PWRDWN specific context information
     */
    CSL_PwrdwnContext *pContext
);


/** ============================================================================
 *  @b CSL_ pwrdwnOpen
 *  
 *  @b Description
 *  @n This function populates the peripheral data object for the PWRDWN 
 *    instance and returns a handle to the instance.
 *    The open call sets up the data structures for the particular instance
 *    of PWRDWN device. The device can be re-opened anytime after it has been
 *    normally closed if so required. The handle returned by this call is
 *    input as an essential argument for rest of the APIs described
 *    for this module.
 *
 * @b Arguments
 * @verbatim
          pwrdwnObj      Pointer to PWRDWN object.

          pwrdwnNum      Instance of pwrdwn CSL to be opened.
                         There are three instance of the PWRDWN
                         available. So, the value for this parameter will be
                         based on the instance.
                         
          pPwrdwnParam   Module specific parameters.
          status         Status of the function call
   @endverbatim
 *
 * <b> Return Value </b>  CSL_pwrdwnHandle
 * @n                     Valid pwrdwn handle will be returned if
 *                        status value is equal to CSL_SOK.
 *
 * <b> Pre Condition </b>
 * @n  CSL_pwrdwnInit(), CSL_pwrdwnOpen()must be opened prior to this call
 *
 * <b> Post Condition </b>
 * @n   1. The status is returned in the status variable. If status
 *         returned is
 * @li     CSL_SOK             Valid pwrdwn handle is returned
 * @li     CSL_ESYS_FAIL       The pwrdwn instance is invalid
 *
 *      2. pwrdwn object structure is populated
 *
 * @b Modifies
 * @n   1. The status variable
 *
 *      2. pwrdwn object structure
 *      
 * @b Example
   @verbatim
       CSL_PwrdwnObj pwrObj;
       CSL_PwrdwnConfig pwrConfig;
       CSL_PwrdwnHandle hPwr;
       // Init Module
       ...
       if (CSL_pwrdwnInit(NULL) != CSL_SOK)
           exit;
       // Opening a handle for the Module   
       hPwr = CSL_pwrdwnOpen (&pwrObj, CSL_PWRDWN, NULL, NULL);
    
        // Setup the arguments fof the Config structure
        ...
   @endverbatim
 * ============================================================================
 */
CSL_PwrdwnHandle CSL_pwrdwnOpen (
    /* Pointer to the object that holds reference to the
     * instance of PWRDWN requested after the call
     */
    CSL_PwrdwnObj   *hPwrdwnObj,
    
    /* Instance of PWRDWN to which a handle is requested
     */
    CSL_InstNum     pwrdwnNum,
    
    /* Module specific parameters;
     * Currently there are none; the user should pass 'NULL'
     */
    CSL_PwrdwnParam *hPwrdwnParam,
    
    /* This returns the status (success/errors) of the call.
     * Could be 'NULL' if the user does not want status information.
     */
    CSL_Status      *hStatus
);


/** =========================================================================== 
 * @n@b CSL_pwrdwnClose
 *
 *  @b Description
 *  @n This function closes the specified instance of pwrdwn.
 *
 *  @b Arguments
 *  @verbatim
           hPwrdwn            Handle to the PWRDWN instance
    @endverbatim
 *
 *  <b> Return Value </b>  CSL_Status
 *  @li                    CSL_SOK             - Close successful
 *  @li                    CSL_ESYS_BADHANDLE  - Invalid handle
 *   
 *  <b> Pre Condition </b>
 *  @n  CSL_pwrdwnInit(), CSL_pwrdwnOpen() must be opened prior to this call.
 *
 *  <b> Post Condition </b>
 *  @n  1. The PWRDWN CSL APIs can not be called until the PWRDWN
 *         CSL is reopened again using CSL_pwrdwnOpen()
 *
 *  @b Modifies
 *  @n  None
 *
 *  @b Example:
 *  @verbatim 
       CSL_PwrdwnObj pwrObj;
       CSL_PwrdwnConfig pwrConfig;
       CSL_PwrdwnHandle hPwr;
       // Init Module
       ...
       if (CSL_pwrdwnInit(NULL) != CSL_SOK)
       exit;
       // Opening a handle for the Module   
       hPwr = CSL_pwrdwnOpen(&pwrObj, CSL_PWRDWN, NULL, NULL);
    
       // Setup the arguments fof the Config structure
       ...
       // Close
       CSL_pwrdwnClose(hPwr);  
  @endverbatim
 * ===========================================================================
 */
 CSL_Status CSL_pwrdwnClose (
    /* Pointer to the object that holds reference to the
     * instance of PWRDWN requested after the call
     */
    CSL_PwrdwnHandle hPwrdwn
);

 
/** ===========================================================================
 *   @n@b CSL_pwrdwnHwSetup
 *
 *   @b Description
 *   @n It configures the  PWRDWN instance registers as per the values passed
 *      in the hardware setup structure.
 *
 *   @b Arguments
 *   @verbatim
            hPwrdwn         Handle to the pwrdwn instance

            setup           Pointer to hardware setup structure
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK             - Hardware setup successful
 *   @li                    CSL_ESYS_BADHANDLE  - Invalid handle
 *   @li                    CSL_ESYS_INVPARAMS  - Hardware structure is not
 *                                                properly initialized
 *
 *   <b> Pre Condition </b>
 *   @n  CSL_pwrdwnInit(), CSL_pwrdwnOpen() must be opened prior to this call
 *
 *   <b> Post Condition </b>
 *   @n  The specified instance will be setup according to value passed
 *
 *   @b Modifies
 *   @n Hardware registers for the specified instance
 * 
 *   @b Example:
     @verbatim
        CSL_PwrdwnObj pwrObj;
        CSL_PwrdwnHwSetup pwrSetup;
        CSL_PwrdwnHandle hPwr;
        // Init Module
        ...
        if (CSL_pwrdwnInit(NULL) != CSL_SOK)
            exit;
        // Opening a handle for the Module  
        hPwr = CSL_pwrdwnOpen (&pwrObj, CSL_PWRDWN, NULL, NULL);
    
        // Setup the arguments for the Setup structure
        ...
    
        // Setup
        CSL_pwrdwnHwSetup(hPwr,&pwrSetup);
    
        // Close handle
        CSL_pwrdwnClose(hPwr);
    @endverbatim
 * ============================================================================  
 */
CSL_Status CSL_pwrdwnHwSetup (
    /* Pointer to the object that holds reference to the
     * instance of PWRDWN.   */
    CSL_PwrdwnHandle hPwrdwn,
    
    /* Pointer to setup structure which contains the
     * information to program PWRDWN to a useful state
     */
    CSL_PwrdwnHwSetup *setup
);


/** ===========================================================================
 *   @n@b CSL_pwrdwnGetHwSetup
 *
 *   @b Description
 *   @n It retrives the hardware setup parameters
 *
 *   @b Arguments
 *   @verbatim
         hPwr        Handle to the PWRDWN instance

         hwSetup     Pointer to hardware setup structure
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK             - Hardware setup retrived
 *   @li                    CSL_ESYS_BADHANDLE  - Invalid handle
 *
 *   <b> Pre Condition </b>
 *   @b CSL_pwrdwnInit(), CSL_pwrdwnOpen() must be opened prior to this call.
 *
 *   <b> Post Condition </b>
 *   @n  The hardware set up structure will be populated with values from
 *       the registers
 *
 *   @b Modifies
 *   @n None
 *   
 *   @b Example
     @verbatim
       CSL_PwrdwnObj pwrObj;
       CSL_PwrdwnHwSetup pwrSetup,querySetup;
       CSL_PwrdwnHandle hPwr;
       CSL_BitMask32 pageSleep;
       // Init Module
       ...
       if (CSL_pwrdwnInit(NULL) != CSL_SOK)
           exit;
           // Opening a handle for the Module   
           hPwr = CSL_pwrdwnOpen (&pwrObj, CSL_PWRDWN, NULL, NULL);
    
       // Setup the arguments for the Setup structure
       ...
    
       // Setup
       CSL_pwrdwnHwSetup(hPwr,&pwrSetup);
    
       // Query Setup
       CSL_pwrdwnGetHwSetup(hPwr,&querySetup);
    
       // Close handle
       CSL_pwrdwnClose(hPwr);

     @endverbatim
 * ============================================================================
 */
CSL_Status CSL_pwrdwnGetHwSetup (
    /* Pointer to the object that holds reference to the
     * instance of PWRDWN requested after the call
     */
    CSL_PwrdwnHandle  hPwrdwn,
    /* Pointer to setup structure which contains the
     * information to program PWRDWN to a useful state
     */
    CSL_PwrdwnHwSetup *setup
);


/** ===========================================================================
 *   @n@b CSL_pwrdwnHwControl
 *
 *   @b Description
 *   @n This function performs various control operations on the PWRDWN instance,
 *      based on the command passed.
 *
 *   @b Arguments
 *   @verbatim
            hPwrdwn  Handle to the PWRDWN instance

            cmd      Operation to be performed on the PWRDWN

            cmdArg  Arguement specific to the command
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK            - Command execution successful.
 *   @li                    CSL_ESYS_BADHANDLE - Invalid handle
 *   @li                    CSL_ESYS_INVCMD    - Invalid command
 *
 *   <b> Pre Condition </b>
 *   CSL_pwrdwnInit(), CSL_pwrdwnOpen() must be opened prior to this call
 *
 *   <b> Post Condition </b>
 *   @n  Registers of the PWRDWN instance are configured according to the command
 *       and the command arguments. The command determines which registers are
 *       modified.
 *
 *   @b Modifies
 *   @n Registers determined by the command
 *
 *   @b Example:
     @verbatim
        CSL_PwrdwnObj pwrObj;
        CSL_PwrdwnHwSetup pwrSetup;
        CSL_PwrdwnHandle hPwr;
        CSL_PwrdwnPortData pageSleep;
        // Init Module
        ...
        if (CSL_pwrdwnInit(NULL) != CSL_SOK)
            exit;
        // Opening a handle for the Module  
        hPwr = CSL_pwrdwnOpen (&pwrObj, CSL_PWRDWN, NULL, NULL);
    
        // Setup the arguments for the Setup structure
        ...
    
        // Setup
        CSL_pwrdwnHwSetup(hPwr,&pwrSetup);
    
        // Hw Control
        pageSleep.portNum = 0x0;
        pageSleep.data = 0x0;
        
        CSL_pwrdwnHwControl(hPwr,CSL_PWRDWN_CMD_PAGE0_SLEEP,&pageSleep);
    
        // Close handle
        CSL_pwrdwnClose(hPwr);
     @endverbtim
 * ============================================================================
 */
CSL_Status CSL_pwrdwnHwControl (
    /* Pointer to the object that holds reference to the
     * instance of PWRDWN.
     */
    CSL_PwrdwnHandle       hPwrdwn,
    
    /* The command to this API indicates the action to be taken
     */
    CSL_PwrdwnHwControlCmd cmd,
    
    /* An optional argument @a void* casted
     */
    void                   *arg
);


/** ===========================================================================
 *   @n@b CSL_pwrdwnGetHwStatus
 *
 *   @b Description
 *   @n This function is used to get the value of various parameters of the
 *      PWRDWN instance. The value returned depends on the query passed.
 *
 *   @b Arguments
 *   @verbatim
            hPwr            Handle to the PWRDWN instance

            query           Query to be performed

            response        Pointer to buffer to return the data requested by
                            the query passed
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK            - Successful completion of the
 *                                               query
 *
 *   @li                    CSL_ESYS_BADHANDLE - Invalid handle
 *
 *   @li                    CSL_ESYS_INVQUERY  - Query command not supported
 *
 *   <b> Pre Condition </b>
 *   @n  CSL_pwrdwnInit(), CSL_pwrdwnOpen() must be opened prior to this call
 *
 *   <b> Post Condition </b>
 *       Data requested by the query is returned through the variable "response"
 *
 *   @b Modifies
 *   @n The input arguement "response" is modified
 *
 *   @b Example:
     @verbatim
            CSL_PwrdwnObj pwrObj;
            CSL_PwrdwnHwSetup pwrSetup;
            CSL_PwrdwnHandle hPwr;
            CSL_PwrdwnPortData pageSleep;
            // Init Module
            ...
            if (CSL_pwrdwnInit(NULL) != CSL_SOK)
                exit;
            // Opening a handle for the Module  
            hPwr = CSL_pwrdwnOpen (&pwrObj, CSL_PWRDWN, NULL, NULL);
    
            // Setup the arguments for the Setup structure
            ...
    
            // Setup
            //
            CSL_pwrdwnHwSetup(hPwr,&pwrSetup);
    
            // Hw Status Query
            pageSleep.portNum = 0x0;
            CSL_pwrdwnGetHwStatus(hPwr,CSL_PWRDWN_QUERY_PAGE0_STATUS,&pageSleep);
    
            // Close handle
            CSL_pwrdwnClose(hPwr);
   @endverbatim
 * ============================================================================
 */
CSL_Status CSL_pwrdwnGetHwStatus (
    /* Pointer to the object that holds reference to the
     * instance of PWRDWN requested after the call
     */
    CSL_PwrdwnHandle        hPwrdwn,
    /* The query to this API which indicates the status
     * to be returned
     */
    CSL_PwrdwnHwStatusQuery query,
    /* Placeholder to return the status. @a void* casted */
    void                    *response
);


/** ============================================================================
 *   @n@b CSL_pwrdwnGetBaseAddress
 *
 *   @b Description
 *   @n  This function gets the base address of the given pwrdwn
 *       instance. 
 *
 *   @b Arguments
 *   @verbatim      
            hPwrdwn         Pointer to the peripheral data object for pwrdwn
 
            pwrdwnNum       Specifies the instance of the pwrdwn to be
                            opened.
 
            pPwrdwnParam    pwrdwn module specific parameters.
 
            pBaseAddress    Pointer to base address structure containing base 
                            address details.
            
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_OK            Open call is successful
 *   @li                    CSL_ESYS_FAIL     pwrdwn instance is not
 *                                            available.
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  Base address structure is populated
 *
 *   @b Modifies
 *   @n    1. The status variable
 *
 *         2. Base address structure is modified.
 *
 *   @b Example
 *   @verbatim
       CSL_PwrdwnHandle       hPwrdwn;
       CSL_PwrdwnBaseAddress  baseAddress;
       CSL_PwrdwnParam        params; 

       CSL_pwrdwnGetBaseAddress(CSL_PWRDWN, &params, &baseAddress) ;
        
    @endverbatim
 * ===========================================================================
 */
CSL_Status CSL_pwrdwnGetBaseAddress (
    /* Instance number */
    CSL_InstNum           pwrdwnNum,
        
    /* Module specific parameters */
    CSL_PwrdwnParam       *hPwrdwnParam,
        
    /* Base address details */
    CSL_PwrdwnBaseAddress *pBaseAddress
);


/** ===========================================================================
 *   @n@b CSL_pwrdwnHwSetupRaw
 *
 *   @b Description
 *   @n This function initializes the device registers with the register-values
 *      provided through the config data structure.
 *
 *   @b Arguments
 *   @verbatim
            hPwrdwn         Pointer to the object that holds reference to the
                            instance of PWRDWN requested after the call 

            config          Pointer to the config structure containing the
                            device register values
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK             - Configuration successful
 *   @li                    CSL_ESYS_BADHANDLE  - Invalid handle
 *   @li                    CSL_ESYS_INVPARAMS  - Configuration structure
 *                                                pointer is not properly
 *                                                 initialized
 *
 *   <b> Pre Condition </b>
 *   @n  CSL_pwrdwnInit(), CSL_pwrdwnOpen() must be opened prior to this call
 *
 *   <b> Post Condition </b>
 *   @n  The registers of the specified PWRDWN instance will be setup
 *       according to the values passed through the config structure
 *
 *   @b Modifies
 *   @n Hardware registers of the specified PWRDWN instance
 *
 * @b Example
   @verbatim 
        CSL_PwrdwnObj pwrObj;
        CSL_PwrdwnConfig pwrConfig;
        CSL_PwrdwnHandle hPwr;
        // Init Module
        ...
        if (CSL_pwrdwnInit(NULL) != CSL_SOK)
            exit;
        // Opening a handle for the Module  
        hPwr = CSL_pwrdwnOpen (&pwrObj, CSL_PWRDWN, NULL, NULL);
    
        // Setup the arguments for the Config structure
        ...
    
        // Setup
        CSL_pwrdwnHwSetupRaw(hPwr,&pwrConfig);
    
        // Close handle
        CSL_pwrdwnClose(hPwr);
   @endverbatim
 * ============================================================================
 */
CSL_Status CSL_pwrdwnHwSetupRaw (
    /* Pointer to the object that holds reference to the
     * instance of PWRDWN requested after the call
     */
    CSL_PwrdwnHandle hPwrdwn,

    /* Pointer to config structure which contains the
     * register values to program PWRDWN to a useful state
     */
    CSL_PwrdwnConfig *config
);


#ifdef __cplusplus
}
#endif

#endif /* _CSL_PWRDWN_H_ */
