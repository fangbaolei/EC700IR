/*  ============================================================================
 *   Copyright (c) Texas Instruments Inc 2002, 2003, 2004, 2005
 *
 *   Use of this software is controlled by the terms and conditions found
 *   in the license agreement under which this software has been supplied.
 *   ===========================================================================
 */

/** 
 *  @file    csl_hpi.h
 *
 *  @path    $(CSLPATH)\inc
 *
 *  @brief   HPI functional layer API header file
 *
 */

/** 
 * @mainpage HPI CSL 3.x
 *
 * @section Introduction
 *
 * @subsection xxx Purpose and Scope
 * The purpose of this document is to identify a set of common CSL APIs for
 * the HPI module across various devices. The CSL developer is expected to
 * refer to this document while designing APIs for these modules. Some of the
 * listed APIs may not be applicable to a given HPI module. While other cases
 * this list of APIs may not be sufficient to cover all the features of a
 * particular HPI Module. The CSL developer should use his discretion designing
 * new APIs or extending the existing ones to cover these.
 *
 * @subsection Terms and Abbreviations
 *   -# CSL:  Chip Support Library
 *   -# API:  Application Programmer Interface
 *
 * @subsection References
 *    -# CSL-001-DES, CSL 3.x Design Specification DocumentVersion 1.02
 *
 */

/* =============================================================================
 *  Revision History
 *  ===============
 *  06-Feb-2006 ds  Removed CSL_HPI_QUERY_PID_REV, CSL_HPI_QUERY_PID_TYPE and
 *                  CSL_HPI_QUERY_PID_CLASS from the CSL_HpiHwStatusQuery
 *
 * =============================================================================
 */

#ifndef _CSL_HPI_H_
#define _CSL_HPI_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <cslr.h>
#include <soc.h>
#include <csl_error.h>
#include <csl_types.h>
#include <csl.h>
#include <cslr_hpi.h>

/** 
 * @brief Enumeration for queries passed to @a CSL_hpiGetHwStatus()
 *
 * This is used to get the status of different operations 
 */
typedef enum {
    CSL_HPI_QUERY_HRDY         = 1,
    /**<
     * @brief   Query the current value of Host Fetch
     * @param   (Uint32 *)
     */    
    CSL_HPI_QUERY_FETCH        = 2,
    /**<
     * @brief   Query the current value of HPI Reset
     * @param   (Uint32 *)
     */    
    CSL_HPI_QUERY_HPI_RST      = 3,
    /**<
     * @brief   Query the current value of Half-word ordering status
     * @param   (Uint32 *)
     */    
    CSL_HPI_QUERY_HWOB_STAT    = 4
    
} CSL_HpiHwStatusQuery;

/** 
 * @brief Enumeration for commands passed to @a CSL_hpiHwControl()
 *
 * This is used to select the commands to control the operations
 * existing setup of HPI. The arguments to be passed with each
 * enumeration if any are specified next to the enumeration.
 */
typedef enum {
    /**<
     * @brief   Sets the HPIC Host-to-DSP Interrupt.
     * @param   (None)
     */    
    CSL_HPI_CMD_SET_DSP_INT      = 0,
    /**<
     * @brief   Reset the HPIC Host-to-DSP Interrupt.
     * @param   (None)
     */    
    CSL_HPI_CMD_RESET_DSP_INT    = 1,
    /**<
     * @brief   Set the HPIC DSP-to-Host Interrupt.
     * @param   (None)
     */    
    CSL_HPI_CMD_SET_HINT         = 2,
    /**<
     * @brief   Reset the HPIC DSP-to-Host Interrupt.
     * @param   (None)
     */    
    CSL_HPI_CMD_RESET_HINT       = 3
} CSL_HpiHwControlCmd;

/** @brief Structure configures Host Port Interface Write & Read Address */ 
typedef struct {
    /** Host Port Interface Write Address */
    Uint32    hpiaWrtAddr;
    /** Host Port Interface Read Address */
    Uint32    hpiaReadAddr;
} CSL_HpiAddrCfg;

/** @brief  Enumation for the control commands of HPI. */ 
typedef enum {
    /** Half-word Ordering Bit */
    CSL_HPI_HWOB           = 0x1, 
    /** Host-to-DSP Interrupt */
    CSL_HPI_DSP_INT        = 0x2, 
    /** DSP-to-Host Interrupt */      
    CSL_HPI_HINT           = 0x4, 
    /** Host Ready */
    CSL_HPI_HRDY           = 0x8,   
    /** Host Fetch */
    CSL_HPI_FETCH          = 0x10,  
    /** CPU Core Reset */
    CSL_HPI_RESET          = 0x40,  
    /** HPI Reset */
    CSL_HPI_HPI_RST        = 0x80,
    /** Half-word ordering bit status */
    CSL_HPI_HWOB_STAT      = 0x100,
    /** Dual HPIA mode configuration bit */
    CSL_HPI_DUAL_HPIA      = 0x200, 
    /** HPIA register select bit */    
    CSL_HPI_HPIA_RW_SEL    = 0x800
} CSL_HpiCtrl;            
                 
/** @brief The structure contains the HPI hardware setup */ 
typedef struct {
    /** Host port Interface control Register */
    CSL_HpiCtrl       hpiCtrl;
    /** Host port Interface Read & Write Address Register */
    CSL_HpiAddrCfg    hpiAddr;
    /** Emulation Mode parameter */
    Uint32             emu;
} CSL_HpiHwSetup;

/** @brief The configuration structure which is to configure HPI. */ 
typedef struct  {
    /** Power and Emulation Management Register */
    volatile Uint32    PWREMU_MGMT;
    /** Host Port Interface Control Register */    
    volatile Uint32    HPIC;
    /** Host Port Interface Write Address Register */
    volatile Uint32    HPIAW;
    /** Host Port Interface Read Address Register */
    volatile Uint32    HPIAR;
} CSL_HpiConfig;

/** Default Values for Config structure */
#define CSL_HPI_CONFIG_DEFAULTS { \
    CSL_HPI_PWREMU_MGMT_RESETVAL, \
    CSL_HPI_HPIC_RESETVAL, \
    CSL_HPI_HPIAW_RESETVAL, \
    CSL_HPI_HPIAR_RESETVAL \
}

/** @brief HPI specific context information. Present implementation of HPI 
 *         CSL doesn't have any context information.
 */
typedef struct {
    /** Context information of HPI CSL.
     *  The below declaration is just a place-holder for future implementation.
     */
    Uint32    contextInfo;
} CSL_HpiContext;

/** @brief HPI specific parameters.  Present implementation of HPI CSL 
 *         doesn't have any module specific parameters.
 */
typedef struct {
    /** Bit mask to be used for module specific parameters. The below
     *  declaration is just a place-holder for future implementation.
     */
    CSL_BitMask32    flags;
} CSL_HpiParam;

/** @brief This structure contains the base-address information for the 
 *  peripheral instance of the HPI
 */
typedef struct {
    /** Base-address of the configuration registers of the peripheral */
    CSL_HpiRegsOvly    regs;
} CSL_HpiBaseAddress;

/** @brief This structure/object holds the context of the instance of HPI 
  *        opened using CSL_hpiOpen() function. Pointer to this object is 
  *        passed as HPI Handle to all HPI CSL APIs. CSL_hpiOpen() function 
  *        initializes this structure based on the parameters passed 
  */
typedef struct CSL_HpiObj{
    /** Pointer to the register overlay structure of the hpi */
    CSL_HpiRegsOvly    regs;
    /** Instance of hpi being referred by this object  */
    CSL_InstNum         hpiNum;
} CSL_HpiObj;

/** @brief This data type is used to return the handle to the CSL of the HPI */
typedef struct CSL_HpiObj    *CSL_HpiHandle;

/*******************************************************************************
 * DSP HPI controller function declarations
 ******************************************************************************/

/** ============================================================================
 *   @n@b CSL_hpiInit
 *
 *   @b Description
 *   @n This is the initialization function for the hpi CSL.  The function 
 *      must be called before calling any other API from this CSL.  This
 *      function is idem-potent.  Currently, the function just return status
 *      CSL_SOK, without doing anything.
 *
 *   @b Arguments
 *   @verbatim
        pContext    Pointer to module-context.  As HPI doesn't have
                    any context based information user is expected to pass NULL.
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
            CSL_hpiInit(NULL);
     @endverbatim
 * =============================================================================
 */
extern CSL_Status  CSL_hpiInit (
    CSL_HpiContext    *pContext
);

/** ============================================================================
 *   @n@b CSL_hpiOpen
 *
 *   @b Description
 *   @n This function returns the handle to the HPI controller
 *      instance. This handle is passed to all other CSL APIs.
 *
 *   @b Arguments
 *   @verbatim
            hpiObj        Pointer to the object that holds reference to the
                           instance of HPI requested after the call.
 
            hpiNum        Instance of HPI to which a handle is requested. 
                           There is only one instance of the hpi available.
                           So, the value for this parameter will be 
                           CSL_HPI always.
 
            pHpiParam  Module specific parameters.
 
            status         Status of the function call
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_HpiHandle
 *   @n                     Valid HPI handle will be returned if
 *                          status value is equal to CSL_SOK.
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n   1.    The status is returned in the status variable. If status
 *              returned is
 *   @li            CSL_SOK            - Valid HPI handle is returned
 *   @li            CSL_ESYS_FAIL      - The HPI instance is invalid
 *   @li            CSL_ESYS_INVPARAMS - Invalid parameter
 *
 *        2.    HPI object structure is populated
 *
 *   @b Modifies
 *   @n    1. The status variable
 *
 *         2. HPI object structure
 *
 *   @b Example
 *   @verbatim
            CSL_status          status;
            CSL_HpiObj         hpiObj;
            CSL_HpiHandle      hHpi;

            ...
                
            hHpi = CSL_hpiOpen(&hpiObj, CSL_HPI, NULL, &status);
            
            ...
    @endverbatim
 * =============================================================================
 */
extern CSL_HpiHandle CSL_hpiOpen (
    CSL_HpiObj      *hpiObj,
    CSL_InstNum      hpiNum,
    CSL_HpiParam    *pHpiParam,
    CSL_Status       *status
);

/** ============================================================================
 *   @n@b CSL_hpiGetBaseAddress
 *
 *   @b Description
 *   @n  Function to get the base address of the peripheral instance.
 *       This function is used for getting the base address of the peripheral
 *       instance. This function will be called inside the CSL_hpiOpen()
 *       function call. This function is open for re-implementing if the user
 *       wants to modify the base address of the peripheral object to point to
 *       a different location and there by allow CSL initiated write/reads into
 *       peripheral. MMR's go to an alternate location.
 *
 *   @b Arguments
 *   @verbatim
            hpiNum      Specifies the instance of the hpi to be opened.

            pHpiParam   Module specific parameters.

            pBaseAddress    Pointer to base address structure containing base
                            address details.

     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_OK              Open call is successful
 *   @li                    CSL_ESYS_FAIL       The instance number is invalid.
 *   @li                    CSL_ESYS_INVPARAMS  Invalid parameter 
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
        CSL_Status              status;
        CSL_HpiBaseAddress  baseAddress;

        ...

        status = CSL_hpiGetBaseAddress(CSL_HPI_PER_CNT, NULL,
                                           &baseAddress);
        ...

    @endverbatim
 * ===========================================================================
 */
extern CSL_Status CSL_hpiGetBaseAddress (
    CSL_InstNum            hpiNum,
    CSL_HpiParam          *pHpiParam,
    CSL_HpiBaseAddress    *pBaseAddress
);

/** ============================================================================
 *   @n@b CSL_hpiClose
 *
 *   @b Description
 *   @n This function closes the specified instance of HPI.
 *
 *   @b Arguments
 *   @verbatim
            hHpi            Handle to the HPI

     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK             - Close successful
 *   @li                    CSL_ESYS_BADHANDLE  - Invalid handle
 *
 *   <b> Pre Condition </b>
 *   @n  The HPI instance should be opened before this close operation.
 *
 *   <b> Post Condition </b>
 *   @n  The HPI CSL APIs can not be called until the HPI CSL is reopened 
 *       again using CSL_hpiOpen().
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
            CSL_HpiHandle      hHpi;
            CSL_Status          status;

            ...
            
            status = CSL_hpiClose(hHpi, &hwSetup);
     @endverbatim
 * ===========================================================================
 */ 
extern CSL_Status  CSL_hpiClose (
    CSL_HpiHandle    hHpi
);

/** ============================================================================
 *   @n@b CSL_hpiHwSetup
 *
 *   @b Description
 *   @n It configures the hpi registers as per the values passed
 *      in the hardware setup structure.
 *
 *   @b Arguments
 *   @verbatim
            hHpi               Handle to the hpi
 
            hwSetup             Pointer to harware setup structure
 
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK             - Hardware setup successful
 *   @li                    CSL_ESYS_BADHANDLE  - The handle passed is invalid
 *   @li                    CSL_ESYS_INVPARAMS  - The parameter passed is 
 *                                                invalid
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  HPI registers are configured according to the hardware setup 
 *       parameters.
 *
 *   @b Modifies
 *   @n HPI registers
 *
 *   @b Example
 *   @verbatim
            CSL_status          status;
            CSL_HpiHwSetup     myHwSetup;            
            CSL_HpiHandle      hHpi;

            myHwSetup.HPIC = hwSetup->hpiCtrl;
             .....
            
            status = CSL_hpiHwSetup(hHpi, &hwSetup);
     @endverbatim
 * ===========================================================================
 */
extern CSL_Status  CSL_hpiHwSetup (
    CSL_HpiHandle     hHpi,
    CSL_HpiHwSetup    *hwSetup
);

/** ============================================================================
 *   @n@b CSL_hpiHwControl
 *
 *   @b Description
 *   @n This function takes an input control command with an optional argument 
 *      and accordingly controls the operation/configuration of HPI.
 *
 *   @b Arguments
 *   @verbatim
            hHpi           Handle to the HPI instance

            cmd             The command to this API indicates the action to be
                            taken on HPI.

            arg             An optional argument.

     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li         CSL_SOK               - Command successful.
 *   @li         CSL_ESYS_BADHANDLE    - The handle passed is invalid
 *   @li         CSL_ESYS_INVCMD       - Invalid command
 *   @li         CSL_ESYS_INVPARAMS    - Invalid parameter
 *
 *   <b> Pre Condition </b>
 *   @n  CSL_hpiInit() and CSL_hpiOpen() must be called successfully in order 
 *       before calling CSL_hpiHwControl().
 *
 *   <b> Post Condition </b>
 *   @n  HPI registers are configured according to the command passed.
 *
 *   @b Modifies
 *   @n The hardware registers of HPI.
 *
 *   @b Example
 *   @verbatim
        CSL_HpiHandle        hHpi;
        CSL_HpiHwControlCmd   cmd;
        void                  arg;

        status = CSL_hpiHwControl(hHpi, cmd, &arg);

     @endverbatim
 * =============================================================================
 */
extern CSL_Status  CSL_hpiHwControl (
    CSL_HpiHandle          hHpi,
    CSL_HpiHwControlCmd    cmd,
    void                    *arg
);

/** ============================================================================
 *   @n@b CSL_hpiGetHwStatus
 *
 *   @b Description
 *   @n Gets the status of the different operations of HPI.
 *
 *   @b Arguments
 *   @verbatim
            hHpi         Handle to the HPI instance

            query         The query to this API of HPI which indicates the
                          status to be returned.

            response      Placeholder to return the status.

     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK             - Query successful
 *   @li                    CSL_ESYS_BADHANDLE  - Invalid handle
 *   @li                    CSL_ESYS_INVQUERY   - The Query passed is invalid
 *   @li                    CSL_ESYS_INVPARAMS  - Invalid parameter
 *  
 *   <b> Pre Condition </b>
 *   @n  CSL_hpiInit() and CSL_hpiOpen() must be called successfully in order 
 *       before calling CSL_hpiGetHwStatus().
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n Third parameter response vlaue
 *
 *   @b Example
 *   @verbatim
        CSL_HpiHandle          hHpi;
        CSL_HpiHwStatusQuery   query;
        void                    reponse;

        status = CSL_hpiGetHwStatus(hHpi, query, &response);

     @endverbatim
 * =============================================================================
 */
extern CSL_Status  CSL_hpiGetHwStatus (
    CSL_HpiHandle           hHpi,
    CSL_HpiHwStatusQuery    query,
    void                     *response
);

/** ============================================================================
 *   @n@b CSL_hpiHwSetupRaw
 *
 *   @b Description
 *   @n This function initializes the device registers with the register-values
 *      provided through the Config Data structure.
 *
 *   @b Arguments
 *   @verbatim
            hHpi        Handle to the HPI instance

            config          Pointer to Config structure
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li            CSL_SOK               - Configuration successful
 *   @li            CSL_ESYS_BADHANDLE    - Invalid handle
 *   @li            CSL_ESYS_INVPARAMS    - Configuration is not
                                            properly initialized
 *   <b> Pre Condition </b>
 *   @n  CSL_hpiInit() and CSL_hpiOpen() must be called successfully in 
 *       order before calling CSL_hpiGetHwSetupRaw().
 *
 *   <b> Post Condition </b>
 *   @n  The registers of the specified HPI instance will be setup
 *       according to input configuration structure values.
 *
 *   @b Modifies
 *   @n Hardware registers of the specified HPI instance.
 *
 *   @b Example
 *   @verbatim
        CSL_HpiHandle       hHpi;
        CSL_HpiConfig       config = CSL_HPI_CONFIG_DEFAULTS;
        CSL_Status           status;

        status = CSL_hpiHwSetupRaw(hHpi, &config);

     @endverbatim
 * =============================================================================
 */
extern CSL_Status  CSL_hpiHwSetupRaw (
    CSL_HpiHandle    hHpi,
    CSL_HpiConfig    *config
);

/** ============================================================================
 *   @n@b CSL_hpiGetHwSetup
 *
 *   @b Description
 *   @n It retrives the hardware setup parameters of the hpi
 *      specified by the given handle.
 *
 *   @b Arguments
 *   @verbatim
            hHpi        Handle to the hpi
 
            hwSetup         Pointer to the harware setup structure
 
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK             - Retrieving the hardware setup
 *                                                parameters is successful
 *   @li                    CSL_ESYS_BADHANDLE  - The handle is passed is
 *                                                invalid
 *   @li                    CSL_ESYS_INVPARAMS  - Invalid parameter
 *
 *   <b> Pre Condition </b>
 *   @n  CSL_hpiInit() and CSL_hpiOpen() must be called successfully in order
 *       before calling CSL_hpiGetHwSetup().
 *
 *   <b> Post Condition </b>
 *   @n  The hardware setup structure is populated with the hardware setup
 *       parameters
 *
 *   @b Modifies
 *   @n hwSetup variable
 *
 *   @b Example
 *   @verbatim
            CSL_HpiHandle   hHpi;
            CSL_HpiHwSetup  hwSetup;

            ...
            
            status = CSL_hpiGetHwSetup(hHpi, &hwSetup);
            
            ...
            
     @endverbatim
 * ===========================================================================
 */
extern CSL_Status  CSL_hpiGetHwSetup(
    CSL_HpiHandle     hHpi,
    CSL_HpiHwSetup    *hwSetup
);

#ifdef __cplusplus
}
#endif

#endif  /* _CSL_HPI_H_ */

