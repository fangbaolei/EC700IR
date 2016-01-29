/* ============================================================================
 * Copyright (c) Texas Instruments Inc 2002, 2003, 2004, 2005                 
 *                                                                              
 * Use of this software is controlled by the terms and conditions found in the
 * license agreement under which this software has been supplied.             
 * ============================================================================
 */ 

/** ============================================================================ 
 * @file csl_memprot.h
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
 *    - modified on: 06/23/2004
 *    - reason: Created the sources
 *
 * @date 23rd Jun, 2004
 * @author Ruchika Kharwar
 * ============================================================================
 */

/* =============================================================================
 * Revision History
 * ===============
 *  16-Nov-2005 ds  updated the documentation
 * =============================================================================
 */
 
/** ============================================================================ 
 * @mainpage Memory Protection CSL 3.x
 *
 * @section Introduction
 *
 * @subsection xxx Purpose and Scope
 * The purpose of this document is to describe the common API's and data
 * structures for the Memory Protection module.
 *
 * @subsection aaa Terms and Abbré
 * eviations
 *   -# CSL:  Chip Support Library
 *   -# API:  Application Programmer Interface
 *
 * @subsection References
 *    -# CSL-001-DES, CSL 3.x Design Specification DocumentVersion 1.02
 * ============================================================================
 */ 

#ifndef _CSL_MEMPROT_H_
#define _CSL_MEMPROT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <csl.h>
#include <cslr_memprot_L2.h>
#include <cslr_memprot_L1D.h>
#include <cslr_memprot_L1P.h>
#include <soc.h>
    
/**************************************************************************\
* Memory Protection global macro declarations. These are used to setup the
* Memory protection attribute pages. The relevant symbols can also be used
* when after querying the fault status with the errorMask.
*
\**************************************************************************/

#define CSL_MEMPROT_MEMACCESS_UX              0x0001
 /**< User Execute permission           */
    
#define CSL_MEMPROT_MEMACCESS_UW              0x0002
/**< User Write permission              */
    
#define CSL_MEMPROT_MEMACCESS_UR              0x0004
/**< User Read permission               */
    
#define CSL_MEMPROT_MEMACCESS_SX              0x0008
/**< Supervisor Execute permission */
    
#define CSL_MEMPROT_MEMACCESS_SW              0x0010
/**< Supervisor Write permission */
    
#define CSL_MEMPROT_MEMACCESS_SR              0x0020
/**< Supervisor Read permission */
    
#define CSL_MEMPROT_MEMACCESS_LOCAL           0x0100
/**< Local Access */
    
#define CSL_MEMPROT_MEMACCESS_EXT             0x0200
/**< External Allowed ID. VBus  requests with PrivID >= '6' are
    * permitted if access type is allowed*/
    
#define CSL_MEMPROT_MEMACCESS_AID0            0x0400
/**< Allowed ID '0' */
    
#define CSL_MEMPROT_MEMACCESS_AID1            0x0800
/**< Allowed ID '1' */
    
#define CSL_MEMPROT_MEMACCESS_AID2            0x1000
/**< Allowed ID '2' */
    
#define CSL_MEMPROT_MEMACCESS_AID3            0x2000
/**< Allowed ID '3' */
    
#define CSL_MEMPROT_MEMACCESS_AID4            0x4000
/**< Allowed ID '4' */
    
#define CSL_MEMPROT_MEMACCESS_AID5            0x8000
/**< Allowed ID '5' */
    

/** Constants for passing parameters to the functions.
 */
/** @brief Pointer to the L2 memeory protection overlay registers */
typedef volatile CSL_Memprotl2RegsOvly CSL_MemprotRegsOvly;

/** @brief This object contains the reference to the instance of memory
 *  Protection Module opened using the @a CSL_memprotOpen().
 *
 *  A pointer to this object is passed to all Memory Protection CSL APIs.
 */

typedef struct CSL_MemprotObj {
    /** This is a pointer to the memory protection registers of the module
     *  for which memory protection is requested.
     */
    CSL_MemprotRegsOvly regs;
    
    /** This is the instance of module number i.e L2/L1D/L1P/CONFIG */
    CSL_InstNum modNum;
} CSL_MemprotObj;


/** @brief MEMPROT handle.
*/
typedef struct CSL_MemprotObj *CSL_MemprotHandle;

/** @brief Dummy structure */
typedef void CSL_MemprotConfig;

/** @brief This is the setup structure used with the HwSetup API
 */
typedef struct {
    /** This should point to a table of memory page attributes */
    Uint16 *memPageAttr;
    
    /** This should point to an array of 2 32 bit elements
      * (constituting the key) 
      */
    Uint32 *key;
    
    /** This is the number of pages which need to be programmed
     *  starting from 0 
     */
    Uint16 numPages;
} CSL_MemprotHwSetup;


/** @brief This will have the base-address information for the module
 *  instance
 */
typedef struct {
    /** Base-address of the memory protection registers */
    CSL_MemprotRegsOvly regs;
} CSL_MemprotBaseAddress;


/** @brief Module specific context information. Present implementation
 *  doesn't have any Context information.
 */
typedef struct {
    /** Context information of Memory Protection.
     *  The below declaration is just a place-holder for future
     *  implementation.
     */
    Uint16  contextInfo;
} CSL_MemprotContext;

/**
 *  MEMPROT specific parameters. Present implementation doesn't have
 *  any specific parameters.
 */
typedef struct {
    /** Bit mask to be used for module specific parameters.
     *  The below declaration is just a place-holder for future
     *  implementation.
     */
    CSL_BitMask16   flags;
} CSL_MemprotParam;

/** @brief This will be used to query the memory fault status.
 */
typedef struct {
    Uint32        addr;             /**< Memory Protection Fault Address */
    CSL_BitMask16 errorMask;        /**< Bit Mask of the Errors */
    Uint16        fid;              /**< Faulted ID */
} CSL_MemprotFaultStatus;


/** @brief This will be used to set/query the memory page attributes
 */

typedef struct {
    CSL_BitMask16 attr;           /**< Memory Protection Page attributes */
    Uint16        page;           /**< Memory Protection Page number */
}CSL_MemprotPageAttr;


/** @brief Enumeration for queries passed to @a CSL_memprotGetHwStatus()
 *
 *  This is used to get the status of different operations or he current
 *  register settings.
 */
typedef enum {
    /** @brief Gets the fault status from the unit
     *  @param (CSL_MemprotFaultStatus *) 
     */
    CSL_MEMPROT_QUERY_FAULT,
  
    /** @brief Get the memory protection page attributes
     *  @param (CSL_MemprotPageAttr *)
     */
    CSL_MEMPROT_QUERY_PAGEATTR,
  
    /** @brief Memory protection Lock status
     *  @param (CSL_MemprotLockStatus *) 
     */
    CSL_MEMPROT_QUERY_LOCKSTAT
} CSL_MemprotHwStatusQuery;


/** @brief Enumeration for commands passed to @a CSL_memprotHwControl()
 *
 * This is used to select the commands to control the operations in the Module
 */
typedef enum {
    /** Locks the Memory Protecion Unit (command argument
     *  @param Uint32* (An array of 2 32 bits elements constituing the key)) 
     */
    CSL_MEMPROT_CMD_LOCK,
    
    /** Unlocks the Memory Protecion Unit (command argument
     *  @param Uint32* (An array of 2 32 bits elements constituing the key))
     */
    CSL_MEMPROT_CMD_UNLOCK,
    
    /** Sets the page attributes 
     *  @param (CSL_MemprotPageAttr*) 
     */
    CSL_MEMPROT_CMD_PAGEATTR
} CSL_MemprotHwControlCmd;


/** @brief Enumeration for queried lock status.
 */
typedef enum {
    /** Non secure Lock */
    CSL_MEMPROT_LOCKSTAT_LOCK   = 1,
    
    /** Non secure UnLock */
    CSL_MEMPROT_LOCKSTAT_UNLOCK = 0
} CSL_MemprotLockStatus;

/**************************************************************************\
* Memory Protection global function declarations
\**************************************************************************/

/*  ===========================================================================
 *   @n@b CSL_memprotInit
 *
 *   @b Description
 *   @n This is the initialization function for the MEMPROT. This function is
 *      idempotent in that calling it many times is same as calling it once.
 *      This function initializes the CSL data structures, and doesn't affect
 *      the H/W. The function must be called before calling any other API from 
 *      this CSL.
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
 *       This function should be called before using any of the CSL APIs
 *       in the Memory Protection module.
 *       
 *       Note: As Memory Protection doesn't have any context based information, 
 *       the function  just returns CSL_SOK. User is expected to pass NULL.
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
         CSL_memprotInit();
     @endverbatim
 * ============================================================================
 */
CSL_Status CSL_memprotInit (
    CSL_MemprotContext *pContext
);


/*  ===========================================================================
 *   @n@b CSL_memprotOpen
 *
 *   @b Description
 *   @n This function populates the peripheral data object for the instance
 *      and returns a handle to the instance.
 *      The open call sets up the data structures for the particular instance
 *      of MEMPROT device.The device can be re-opened anytime after it has been
 *      normally closed, if so required. The handle returned by this call is 
 *      input as an essential argument for rest of the APIs described 
 *      for this module.
 *
 *   @b Arguments
 *   @verbatim 
              pMemprotObj     Pointer to the MEMPROT instance object
              
              memprotNum      Instance of the MEMPROT to be opened
              
              pMemprotParam   Pointer to module specific parameters
              
              pStatus         pointer for returning status of the function call
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_MemprotHandle
 *   @n                     Valid MEMPROT instance handle will be returned if 
                            status value is equal to CSL_SOK.
 *
 *   <b> Pre Condition </b>
 *  Memory protection must be successfully initialized via @a CSL_memprotInit()
 *  before calling this function. Memory for the @a CSL_MemoprotObj must be
 *  allocated outside this call. This object must be retained while usage of
 *  this module.Depending on the module opened some inherant constraints need
 *  to be kept in mind. When a handle for the Config block is opened the only
 *  operation  possible is a query for the fault Status. No other control
 *  command/ query/ setup must be used.
 *  When a handle for L1D/L1P is opened then too constraints wrt the number of
 *  Memory pages must be kept in mind.
 *
 *   <b> Post Condition </b>
 *   @n   1.    MEMPROT object structure is populated
 *   @n   2.    The status is returned in the status variable. If status
 *              returned is
 *   @li            CSL_SOK             Valid MEMPORT handle is returned
 *   @li            CSL_ESYS_FAIL       The MEMPORT instance is invalid
 *   @li            CSL_ESYS_INVPARAMS  Invalid parameter
 *
 *   @b Modifies
 *   @n    1. The status variable
 *   @n    2. MEMPROT object structure
 *
 *   @b Example
 *  @verbatim

            CSL_MemprotObj mpL2Obj;
            CSL_MemprotHandle hmpL2;
            CSL_Status status;
            // Initializing the module 
            CSL_memprotInit(NULL);
            
            // Opening the Handle for the L2 
            hmpL2 = CSL_memprotOpen(&mpL2Obj,
                                   CSL_MEMPROT_L2,
                                   NULL,
                                   &status);
   @endverbatim
 * ============================================================================
 */  

CSL_MemprotHandle CSL_memprotOpen (
    CSL_MemprotObj   *pMemprotObj,
    CSL_InstNum      memNum,
    CSL_MemprotParam *pMemParam,
    CSL_Status       *pStatus
);


/** ===========================================================================
 *   @n@b CSL_memprotClose
 *
 *   @b Description
 *   @n This function closes the specified instance of MEMPROT.
 *
 *   @b Arguments
 *   @verbatim
            hMem            Handle to the MEMPROT instance
     @endverbatim
 *
 *   <b> Usage Constraints: </b>
 *   CSL_memprotInit(), CSL_memprotOpen() must be opened prior to this call.
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK             - Close successful
 *   @li                    CSL_ESYS_BADHANDLE  - Invalid handle
 *
 *   @b Example
 *   @verbatim
            CSL_MemprotHandle   hMem;
            CSL_status          status;

            ...
            status = CSL_memprotClose(hMem);
     @endverbatim
 * ============================================================================
 */
CSL_Status CSL_memprotClose (
    CSL_MemprotHandle hMemprot
);


/** This function is a dummy in this Module CSL 
 *
 */
CSL_Status CSL_memprotHwSetupRaw (
    CSL_MemprotHandle hMemprot,
    CSL_MemprotConfig *config
);


/*  ============================================================================
 *   @n@b CSL_memprotHwSetup
 *
 *   @b Description
 *   @n This function initializes the module registers with the appropriate
 *      values provided through the HwSetup Data structure.
 *      For information passed through the HwSetup Data structure refer
 *      @a CSL_memprotHwSetup.
 *
 *   @b Arguments
 *   @verbatim
            hMemprot      Handle to the memprot instance

            setup         Pointer to harware setup structure
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK             - Hardware setup successful.
 *   @li                    CSL_ESYS_BADHANDLE  - Invalid handle
 *   @li                    CSL_ESYS_INVPARAMS  - Hardware structure is not
                                                  properly initialized
 *
 *   <b> Pre Condition </b>
 *  Both @a CSL_memprotInit() and @a CSL_memprotOpen() must be called
 *  successfully in that order before this function can be called. The user
 *  has to allocate space for & fill in the main setup structure appropriately
 *  before calling this function. Ensure numpages is not set to > 32 for handles
 *  for L1D/L1P. Ensure numpages is not > 64 for L2.
 *
 *   <b> Post Condition </b>
 *   @n   1.    MEMPROT object structure is populated
 *   @n   2.    The status is returned in the status variable. If status
 *              returned is
 *   @li            CSL_SOK             Valid MEMPORT handle is returned
 *   @li            CSL_ESYS_FAIL       The MEMPORT instance is invalid
 *   @li            CSL_ESYS_INVPARAMS  Invalid parameter
 *
 *
 *   @b Modifies
 *   @n The hardware registers of MEMPORT.
 *
 *   @b Example
 *   @verbatim
        #define PAGE_ATTR 0xFFF0
        
        CSL_MemprotObj mpL2Obj;
        CSL_MemprotHandle hmpL2;
        CSL_Status status;
        CSL_MemprotHwSetup L2MpSetup;
        Uint16 pageAttrTable[10] = {PAGE_ATTR,PAGE_ATTR,PAGE_ATTR,PAGE_ATTR,
                PAGE_ATTR,PAGE_ATTR,PAGE_ATTR,PAGE_ATTR,PAGE_ATTR,PAGE_ATTR};
        Uint32 key[2] = {0x11223344,0x55667788};
        // Initializing the module
        CSL_memprotInit(NULL);
        
        // Opening the Handle for the L2
        hmpL2 = CSL_memprotOpen(&mpL2Obj, CSL_MEMPROT_L2, NULL, &status);
        L2MpSetup. memPageAttr = pageAttrTable;
        L2MpSetup.numPages = 10;
        L2MpSetup.key = key;
        
        // Do Setup for the L2 Memory protection/
        CSL_memprotHwSetup  (hmpL2, &L2MpSetup);
     @endverbatim
 * =============================================================================
 */
CSL_Status CSL_memprotHwSetup (
    CSL_MemprotHandle  hMemprot,
    CSL_MemprotHwSetup *setup
);

/*  ============================================================================
 *   @n@b CSL_memprotGetHwSetup
 *
 *   @b Description
 *   @n This function gets the current setup of the Memory Protection registers.
 *      The status is returned through @a CSL_MemprotHwSetup. The obtaining of
 *      status is the reverse operation of @a CSL_MemprotHwSetup() function.
 *      Only the Memory Page attributes are read and filled into the HwSetup
 *      structure
 *
 *   @b Arguments
 *   @verbatim

            hMemprot        Handle to the MEMPROT instance

            setup           Pointer to setup structure which contains the
                            setup information of MEMPROT.
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK             - Setup info load successful.
 *   @li                    CSL_ESYS_BADHANDLE  - Invalid handle
 *   @li                    CSL_ESYS_INVPARAMS  - Invalid parameter
 *
 *   <b> Usage Constraints: </b>
 *  Both @a CSL_memprotInit(), @a CSL_memprotOpen() and @a CSL_memprotHwSetup()
 *  must be called successfully in that order before @a CSL_memprotGetHwSetup()
 *  can be called. Ensure numpages is initialized depending on the number of
 *  desired attributes in the setup.Make sure to set numpages <= 32 for handles
 *  for L1D/L1P. Ensure numpages <= 64 for L2.
 *
 *   <b> Post Condition </b>
 *   @n  The registers of the specified MEMPROT instance will be setup.
 *
 *   @b Modifies
 *   @n Hardware registers of the specified MEMPROT instance.
 *
 *   @b Example
 *   @verbatim
        #define PAGE_ATTR 0xFFF0
        
        CSL_MemprotObj mpL2Obj;
        CSL_MemprotHandle hmpL2;
        CSL_Status status;
        CSL_MemprotHwSetup L2MpSetup,L2MpGetSetup;
        Uint16 pageAttrTable[10] = {PAGE_ATTR,PAGE_ATTR,PAGE_ATTR,PAGE_ATTR,
                 PAGE_ATTR,PAGE_ATTR,PAGE_ATTR,PAGE_ATTR,PAGE_ATTR,PAGE_ATTR};
        Uint32 key[2] = {0x11223344,0x55667788};

        // Initializing the module
        CSL_memprotInit(NULL);

        // Opening the Handle for the L2
        hmpL2 = CSL_memprotOpen(&mpL2Obj,CSL_MEMPROT_L2,NULL,&status);
        L2MpSetup. memPageAttr = pageAttrTable;
        L2MpSetup.numPages = 10;
        L2MpSetup.key = key;

        // Do Setup for the L2 Memory protection/
        CSL_memprotHwSetup  (hmpL2,&L2MpSetup);
        CSL_memprotGetHwSetup(hmpL2,&L2MpGetSetup);

     @endverbatim
 * =============================================================================
 */
CSL_Status CSL_memprotGetHwSetup (
    CSL_MemprotHandle  hMemprot,
    CSL_MemprotHwSetup *setup
);


/*  ===========================================================================
 *   @n@b CSL_memprotHwControl
 *
 *   @b Description
 *   @n Control operations for the Memory protection registers.
 *      For a particular control operation, the pointer to the corresponding
 *      data type needs to be passed as argument HwControl function Call.
 *      All the arguments (Structure elements included) passed to  the
 *      HwControl function are inputs. For the list of commands supported and
 *      argument type that can be @a void* casted & passed with a particular
 *      command refer to @a CSL_MemprotHwControlCmd.
 *
 *   @b Arguments
 *   @verbatim
            hMemprot        Handle to the MEMPROT instance

            cmd             The command to this API indicates the action to be
                            taken on MEMPROT.

            arg             An optional argument.
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK             - Status info return successful.
 *   @li                    CSL_ESYS_BADHANDLE  - Invalid handle
 *   @li                    CSL_ESYS_INVCMD     - Invalid command
 *   @li                    CSL_ESYS_FAIL       - Invalid instance number
 *
 *   <b> Pre Condition </b>
 *   @n  Both @a CSL_memprotInit() and @a CSL_memprotOpen() must be called
 *       successfully in that order before @a CSL_memprotHwControl() can be
 *       called. For the argument type that can be @a void* casted & passed
 *       with a particular command refer to @a CSL_MemprotHwControlCmd
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n The hardware registers of MEMPROT.
 *
 *   @b Example
 *   @verbatim
        #define PAGE_ATTR 0xFFF0
         
        Uint16 pageAttrTable[10] = {PAGE_ATTR,PAGE_ATTR,PAGE_ATTR,PAGE_ATTR,
                 PAGE_ATTR,PAGE_ATTR,PAGE_ATTR,PAGE_ATTR,PAGE_ATTR,PAGE_ATTR};
        Uint32 key[2] = {0x11223344,0x55667788};

        CSL_MemprotObj mpL2Obj;
        CSL_MemprotHandle hmpL2;
        CSL_Status status;
        CSL_MemprotHwSetup L2MpSetup,L2MpGetSetup;
        CSL_MemprotLockStatus lockStat;
        CSL_MemprotPageAttr pageAttr;
        CSL_MemprotFaultStatus queryFaultStatus;

        // Initializing the module
        CSL_memprotInit(NULL);

        // Opening the Handle for the L2
        hmpL2 = CSL_memprotOpen(&mpL2Obj,CSL_MEMPROT_L2,NULL,&status);
        L2MpSetup. memPageAttr = pageAttrTable;
        L2MpSetup.numPages = 10;
        L2MpSetup.key = key;

        // Do Setup for the L2 Memory protection/
        CSL_memprotHwSetup  (hmpL2,&L2MpSetup);

        // Query Lock Status
        CSL_memprotGetHwStatus(hmpL2,CSL_MEMPROT_QUERY_LOCKSTAT,&lockStat);
        // Unlock the Unit if Locked
        if ((lockStat == CSL_MEMPROT_LOCKSTAT_SUNLOCK)
                || (lockStat == CSL_MEMPROT_LOCKSTAT_NSUNLOCK)) {
            CSL_memprotHwControl(hmpL2,CSL_MEMPROT_CMD_UNLOCK,key);
        }

     @endverbatim
 * ============================================================================
 */
CSL_Status CSL_memprotHwControl (
    CSL_MemprotHandle       hMemprot,
    CSL_MemprotHwControlCmd cmd,
    void                    *arg
);


/*  ===========================================================================
 *   @n@b CSL_memprotGetHwStatus
 *
 *   @b Description
 *   @n This function is used to read the current module configuration, status
 *      flags and the value present associated registers. User should allocate
 *      memory for the said data type and pass its pointer as an unadorned
 *      void*  argument to the status query call. For details about the various
 *      status queries supported and the associated data structure to record
 *      the response refer to @a CSL_MemprotHwStatusQuery
 *
 *   @b Arguments
 *   @verbatim
            hMemprot        Handle to the MEMPROT instance

            query           The query to this API of MEMPROT which indicates
                            the status to be returned.

            response        Placeholder to return the status.
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK             - Status info return successful.
 *   @li                    CSL_ESYS_BADHANDLE  - Invalid handle
 *   @li                    CSL_ESYS_INVPARAMS  - Invalid parameter
 *
 *   <b> Usage Constraints: </b>
 *       Both @a CSL_memprotInit() and @a CSL_memprotOpen() must be called
 *       successfully in that order before @a CSL_memprotGetHwStatus() can be
 *       called. For the argument type that can be @a void* casted & passed
 *       with a particular command refer to @a CSL_MemprotHwStatusQuery
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
        #define PAGE_ATTR 0xFFF0
        
        Uint16 pageAttrTable[10] = {PAGE_ATTR,PAGE_ATTR,PAGE_ATTR,PAGE_ATTR,
                 PAGE_ATTR,PAGE_ATTR,PAGE_ATTR,PAGE_ATTR,PAGE_ATTR,PAGE_ATTR};
        Uint32 key[2] = {0x11223344,0x55667788};
        CSL_MemprotObj mpL2Obj;
        CSL_MemprotHandle hmpL2;
        CSL_Status status;
        CSL_MemprotHwSetup L2MpSetup,L2MpGetSetup;
        CSL_MemprotLockStatus lockStat;
        CSL_MemprotPageAttr pageAttr;
        CSL_MemprotFaultStatus queryFaultStatus;

        // Initializing the module
        CSL_memprotInit(NULL);

        // Opening the Handle for the L2
        hmpL2 = CSL_memprotOpen(&mpL2Obj,CSL_MEMPROT_L2,NULL,&status);
        L2MpSetup. memPageAttr = pageAttrTable;
        L2MpSetup.numPages = 10;
        L2MpSetup.key = key;

        // Do Setup for the L2 Memory protection/
        CSL_memprotHwSetup  (hmpL2,&L2MpSetup);

        // Query Lock Status
        CSL_memprotGetHwStatus(hmpL2,CSL_MEMPROT_QUERY_LOCKSTAT,&lockStat);

     @endverbatim
 * ============================================================================
 */
CSL_Status CSL_memprotGetHwStatus (
    CSL_MemprotHandle        hMemprot,
    CSL_MemprotHwStatusQuery query,
    void                     *response
);


/** ============================================================================
 *   @n@b CSL_memprotGetBaseAddress
 *
 *   @b Description
 *   @n  Function to get the base address of the peripheral instance.
 *       This function is used for getting the base address of the peripheral
 *       instance. This function will be called inside the CSL_memprotOpen()
 *       function call. This function is open for re-implementing if the user
 *       wants to modify the base address of the peripheral object to point to
 *       a different location and there by allow CSL initiated write/reads into
 *       peripheral. MMR's go to an alternate location.
 *
 *   @b Arguments
 *   @verbatim
            memprotNum      Specifies the instance of the memprot to be opened.

            pMemprotParam   Module specific parameters.

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

      CSL_Status status;
      CSL_MemprotBaseAddress   baseAddress;

       ...
      status = CSL_memprotGetBaseAddress(CSL_MEMPROT_L2, NULL, &baseAddress);

    @endverbatim
 * ===========================================================================
 */
CSL_Status CSL_memprotGetBaseAddress (
    CSL_InstNum            memprotNum,
    CSL_MemprotParam       *pMemprotParam,
    CSL_MemprotBaseAddress *pBaseAddress
);

#ifdef __cplusplus
}
#endif

#endif /* _CSL_MEMPROT_H */
