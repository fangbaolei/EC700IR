/*  ============================================================================
 *   Copyright (c) Texas Instruments Inc 2002, 2003, 2004, 2005
 *
 *   Use of this software is controlled by the terms and conditions found in the
 *   license agreement under which this software has been supplied.
 *  ============================================================================
 */

/** @file csl_pllc.h
 *
 *  @brief PLLC functional layer API header file
 *
 *  Path: \(CSLPATH)\inc
 */

/** @mainpage PLLC CSL 3.x
 *
 *  @section Introduction
 *
 *  @subsection xxx Purpose and Scope
 *  The purpose of this document is to identify a set of common CSL APIs for
 *  the PLLC module across various devices. The CSL developer is expected to
 *  refer to this document while designing APIs for these modules. Some of the
 *  listed APIs may not be applicable to a given PLLC module. While other cases
 *  this list of APIs may not be sufficient to cover all the features of a
 *  particular PLLC Module. The CSL developer should use his discretion
 *  designing new APIs or extending the existing ones to cover these.
 *
 *  @subsection aaa Terms and Abbreviations
 *    -# CSL:  Chip Support Library
 *    -# API:  Application Programmer Interface
 *
 *  @subsection References
 *    -# CSL-001-DES, CSL 3.x Design Specification Document Version 1.02
 *
 */

/*  ============================================================================
 *  Revision History
 *  ===============
 *  10-Feb-2004 kpn CSL3X Upgradation.
 *  24-Aug-2005 Tej Modified.
 *  27-Oct-2005 sd  adding defaults for 1GHz, 500MHz, 750MHz
 *  18-Jan-2006 sd  Changes according to spec changes
 *  ============================================================================
 */

#ifndef _CSL_PLLC_H_
#define _CSL_PLLC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <cslr.h>
#include <soc.h>
#include <csl_error.h>
#include <csl_types.h>
#include <cslr_pllc.h>

/**
\defgroup CSL_PLLC_API PLLC
\internal Based on <b>PLLC
*/
/**
\defgroup CSL_PLLC_DATASTRUCT Data Structures
\ingroup CSL_PLLC_API
*/
/**
\defgroup CSL_PLLC_DEFINE  Defines
\ingroup CSL_PLLC_API
*/
/**
\defgroup CSL_PLLC_ENUM  Enumerated Data Types
\ingroup CSL_PLLC_API
*/

/**
\defgroup CSL_PLLC_FUNCTION  Functions
\ingroup CSL_PLLC_API
*/

/*****************************************************************************\
          PLLC global macro declarations
\*****************************************************************************/
#define CSL_PLLC_DELAY_2000      2000

/** \defgroup CSL_PLLC_STATUS_DEFINE PLL Controller Status
 *  \ingroup CSL_PLLC_DEFINE
 *
 * @{ */

/** Set when GO operation (divide-ratio change and
 *   clock alignment) is in progress
 */
#define CSL_PLLC_STATUS_GO CSL_FMKT (PLLC_PLLSTAT_GOSTAT, INPROG)
/**
@} */

/** \defgroup CSL_PLLC_SYSCLKSTAT_DEFINE PLLC SYSCLK Status
 *  \ingroup CSL_PLLC_DEFINE
 *
 * @{ */

 /** SYSCLK5 is ON */
#define CSL_PLLC_SYSCLKSTAT_SYS5ON CSL_FMKT (PLLC_CKSTAT_SYS5ON, ON)
 /** SYSCLK4 is ON */
#define CSL_PLLC_SYSCLKSTAT_SYS4ON CSL_FMKT (PLLC_CKSTAT_SYS4ON, ON)
/** SYSCLK3 is ON */
#define CSL_PLLC_SYSCLKSTAT_SYS3ON CSL_FMKT (PLLC_CKSTAT_SYS3ON, ON)
/** SYSCLK2 is ON */
#define CSL_PLLC_SYSCLKSTAT_SYS2ON CSL_FMKT (PLLC_CKSTAT_SYS2ON, ON)
/** SYSCLK1 is ON */
#define CSL_PLLC_SYSCLKSTAT_SYS1ON CSL_FMKT (PLLC_CKSTAT_SYS1ON, ON)

/**
@} */


/** \defgroup CSL_PLLC_RESETSTAT_DEFINE PLLC Last Reset Status
 *  \ingroup CSL_PLLC_DEFINE
 *
 * @{ */

/** Power On Reset */
#define CSL_PLLC_RESETSTAT_POR CSL_FMKT (PLLC_RSTYPE_POR, YES)
/** Warm Reset */
#define CSL_PLLC_RESETSTAT_WRST CSL_FMKT (PLLC_RSTYPE_WRST, YES)
/** Maximum Reset */
#define CSL_PLLC_RESETSTAT_MRST CSL_FMKT (PLLC_RSTYPE_MRST, YES)
/** System/Chip Reset */
#define CSL_PLLC_RESETSTAT_SRST CSL_FMKT (PLLC_RSTYPE_SRST, YES)

/**
@} */


/** \defgroup CSL_PLLC_CTRL_DEFINE PLLC Control Mask
 *  \ingroup CSL_PLLC_DEFINE
 *
 * @{ */

/** PreDiv, PLL, and PostDiv are bypassed. SYSCLK
 *   divided down directly from input reference
 *   clock refclk
 */
#define CSL_PLLC_CTRL_BYPASS CSL_FMKT (PLLC_PLLCTL_PLLEN, BYPASS)
/** PLL is used. SYSCLK divided down from PostDiv
 *   output
 */
#define CSL_PLLC_CTRL_ENABLE CSL_FMKT (PLLC_PLLCTL_PLLEN, PLL)
/** Selected PLL Operational */
#define CSL_PLLC_CTRL_OPERATIONAL CSL_FMKT (PLLC_PLLCTL_PLLPWRDN, NO)
/** Selected PLL Placed In Power Down State */
#define CSL_PLLC_CTRL_POWERDOWN CSL_FMKT (PLLC_PLLCTL_PLLPWRDN, YES)
/** PLL Reset Released */
#define CSL_PLLC_CTRL_RELEASE_RESET CSL_FMKT (PLLC_PLLCTL_PLLRST, NO)
/** PLL Reset Asserted */
#define CSL_PLLC_CTRL_RESET CSL_FMKT (PLLC_PLLCTL_PLLRST, YES)
/** PLLEN Mux is controlled by PLLCTL.PLLEN.
 *   pllen_pi is don’t care
 */
#define CSL_PLLC_CTRL_MUXCTRL_REGBIT CSL_FMKT (PLLC_PLLCTL_PLLENSRC, REGBIT)
/** PLLEN Mux is controlled by input pllen_pi.
 *   PLLCTL.PLLEN is don’t care
 */
#define CSL_PLLC_CTRL_MUXCTRL_PORT CSL_FMKT (PLLC_PLLCTL_PLLENSRC, NONREGBIT)
/**
@} */

/** \defgroup CSL_PLLC_DIVEN_DEFINE PLLC Divider Enable
 *  \ingroup CSL_PLLC_DEFINE
 *
 * @{ */

/** PREDIV enable */
#define CSL_PLLC_DIVEN_PREDIV    (1 << 0)
/** Enable divider D1 for SYSCLK1 */
#define CSL_PLLC_DIVEN_PLLDIV1   (1 << 1)
/** Enable divider D4 for SYSCLK4 */
#define CSL_PLLC_DIVEN_PLLDIV4   (1 << 2)
/** Enable divider D5 for SYSCLK5 */
#define CSL_PLLC_DIVEN_PLLDIV5   (1 << 3)

/**
@} */

/** \defgroup CSL_PLLC_DIVSEL_DEFINE Divider Select for SYSCLKs
 *  \ingroup CSL_PLLC_DEFINE
 *
 * @{ */

/** Divider D1 for SYSCLK1 */
#define CSL_PLLC_DIVSEL_PLLDIV1     (1)
/** Divider D4 for SYSCLK4 */
#define CSL_PLLC_DIVSEL_PLLDIV4     (2)
/** Divider D5 for SYSCLK5 */
#define CSL_PLLC_DIVSEL_PLLDIV5     (3)

/** \brief  PLLC Bypass Mode
*/typedef enum {
    /** PLL Bypass Mode.                */
    CSL_PLLC_PLL_BYPASS_MODE   = CSL_PLLC_PLLCTL_PLLEN_BYPASS,
    /** PLL PLL Mode (not Bypass Mode). */
    CSL_PLLC_PLL_PLL_MODE      = CSL_PLLC_PLLCTL_PLLEN_PLL
} CSL_PllcPllBypassMode;

/**
@} */

/** \defgroup CSL_PLLC_PLLDIVCTL_ENUM PLL Divide Control
 *  \ingroup CSL_PLLC_ENUM
 @{*/
/** \brief  Enums for PLL divide enable/ disable */
typedef enum {
    /** <b>: PLL Divider Disable</b> */
    CSL_PLLC_PLLDIV_DISABLE = 0,
    /** <b>: PLL Divider Enable</b> */
    CSL_PLLC_PLLDIV_ENABLE = 1
} CSL_PllcDivCtrl;
/**
@} */

/** \defgroup CSL_PLLC_CONTROLCMD_ENUM  Control Commands
 *  \ingroup CSL_PLLC_CONTROL_API
@{*/
/** \brief Enumeration for control commands passed to \a CSL_pllcHwControl()
 *
 *  This is the set of commands that are passed to the \a CSL_pllcHwControl()
 *  with an optional argument type-casted to \a void* .
 *  The arguments to be passed with each enumeration (if any) are specified
 *  next to the enumeration
 */
typedef enum {
    /** \brief Control PLL based on the bits set in the input argument
     *  This is valid only for PLLC instance 1
     *  \param CSL_BitMask32
     */
    CSL_PLLC_CMD_PLLCONTROL,
    /** \brief Set PLL multiplier value
     *  This is valid only for PLLC instance 1
     *  \param Uint32
     */
    CSL_PLLC_CMD_SET_PLLM,
    /** \brief Set PLL divide ratio
     *  \param CSL_PllcDivRatio
     */
    CSL_PLLC_CMD_SET_PLLRATIO,
    /** \brief Enable/disable  PLL divider
     *  \param CSL_PllcDivideControl
     */
    CSL_PLLC_CMD_PLLDIV_CONTROL
} CSL_PllcHwControlCmd;
/**
@} */

/** \defgroup CSL_PLLC_QUERYCMD_ENUM Query Commands
 *  \ingroup CSL_PLLC_QUERY_API
 @{ */

/** \brief Enumeration for queries passed to \a CSL_PllcGetHwStatus()
 *
 *  This is used to get the status of different operations.The arguments
 *  to be passed with each enumeration if any are specified next to
 *  the enumeration
 */
typedef enum {
    /** \brief Queries PLL Controller Status
     *  \param (CSL_BitMask32*)
     */
    CSL_PLLC_QUERY_STATUS,
    /** \brief Queries PLL SYSCLK Status
     *  \param (CSL_BitMask32*)
     */
    CSL_PLLC_QUERY_SYSCLKSTAT,
    /** \brief Queries Reset Type Status
     *  \param (CSL_BitMask32*)
     */
    CSL_PLLC_QUERY_RESETSTAT
} CSL_PllcHwStatusQuery;

/**
@} */

/**
\addtogroup CSL_PLLC_DATASTRUCT
@{ */
/**
 *  \brief Input parameters for setting up PLL Divide ratio
 */
typedef struct CSL_PllcDivRatio {
    /** \brief Divider number  */
    Uint32 divNum;
    /** \brief Divider Ratio  */
    Uint32 divRatio;
} CSL_PllcDivRatio;
/**
@} */

/**
\addtogroup CSL_PLLC_DATASTRUCT
@{ */
/**
 *  \brief Input parameters for enabling\disabling PLL Divide ratio
 *  \sa CSL_PLLC_DIVSEL_DEFINE
 */
typedef struct CSL_PllcDivideControl {
    /** \brief Divider Number  */
    Uint32 divNum;
    /** \brief Divider Control (Enable/Disable) */
    CSL_PllcDivCtrl divCtrl;
} CSL_PllcDivideControl;
/**
@} */

/**
\addtogroup CSL_PLLC_DATASTRUCT
@{ */
/**
 *  \brief Input parameters for setting up PLL Controller
 *
 *  Used to put PLLC known useful state
 */
typedef struct CSL_PllcHwSetup {
    /** \brief PLL Mode PLL/BYPASS 
      * This is valid only for PLLC instance 1
      */
    Uint32        pllMode;
    /** \brief Divider Enable/Disable */
    CSL_BitMask32 divEnable;
    /** \brief Pre-Divider 
      * This is valid only for PLLC instance 1
      */
    Uint32        preDiv;
    /** \brief PLL Multiplier 
      * This is valid only for PLLC instance 1
      */
    Uint32        pllM;
    /** \brief PLL Divider 1  
      * This is valid only for PLLC instance 2
      */
    Uint32        pllDiv1;
    /** \brief PLL Divider 4  
      * This is valid only for PLLC instance 1
      */
    Uint32        pllDiv4;
    /** \brief PLL Divider 5 
      * This is valid only for PLLC instance 1
      */
    Uint32        pllDiv5;
    /** \brief Setup that can be used for future implementation */
    void*         extendSetup;
} CSL_PllcHwSetup;
/**
@} */

/** \defgroup CSL_PLLC_DEFAULT_HWSETUP_DEFINE PLLC Default HwSetup Structure
 *  \ingroup CSL_PLLC_DEFINE
 *
 * @{ */
/** \brief Default hardware setup parameters  for PLL1 */
#define CSL_PLLC_HWSETUP_DEFAULTS_PLL1 {  \
    CSL_PLLC_PLL_BYPASS_MODE,        \
    (CSL_PLLC_DIVEN_PREDIV | \
    CSL_PLLC_DIVEN_PLLDIV4 | \
    CSL_PLLC_DIVEN_PLLDIV5) ,\
    CSL_PLLC_PREDIV_RATIO_RESETVAL + 1,  \
    CSL_PLLC_PLLM_PLLM_RESETVAL  + 1,     \
    0, \
    CSL_PLLC_PLLDIV4_RATIO_RESETVAL + 1, \
    CSL_PLLC_PLLDIV5_RATIO_RESETVAL + 1, \
    NULL                             \
}

/** \brief Default hardware setup parameters for PLL2 */
#define CSL_PLLC_HWSETUP_DEFAULTS_PLL2 {  \
    0,                               \
    CSL_PLLC_DIVEN_PLLDIV1,  \
    0,                               \
    0,                               \
    CSL_PLLC_PLLDIV1_RATIO_RESETVAL + 1, \
    0,                               \
    0,                               \
    NULL                             \
}
/** \brief Default hardware setup parameters for output clock frequency of 750 MHz , 
  * CLKIN = 50MHz
 */
#define CSL_PLLC_HWSETUP_DEFAULTS_750MHZ {  \
    CSL_PLLC_PLL_PLL_MODE,           \
    0,                               \
    0,                               \
    15,                              \
    0,                               \
    0,                               \
    0,                               \
    NULL                             \
}


/** \brief Default hardware setup parameters for output clock frequency of 1GHz , 
  * CLKIN = 50MHz
 */
#define CSL_PLLC_HWSETUP_DEFAULTS_1GHZ {  \
    CSL_PLLC_PLL_PLL_MODE,           \
    0,                               \
    0,                               \
    20,                              \
    0,                               \
    0,                               \
    0,                               \
    NULL                             \
}
/**
@} */

/**
\addtogroup CSL_PLLC_DATASTRUCT
@{ */

/** \brief Config-structure
 *
 *  Used to configure the pllc using CSL_pllcHwSetupRaw()
 */
typedef struct {
    /** PLL Control register 
      * This should be configured only for PLLC instance 1
      */
    Uint32 PLLCTL;

    /** PLL Multiplier Control register 
      * This should be configured only for PLLC instance 1
      */
    Uint32 PLLM;

    /** PLL Pre-Divider Control register 
      * This should be configured only for PLLC instance 1
      */
    Uint32 PREDIV;

    /** PLL Controller Divider 1 register 
      * This should be configured only for PLLC instance 2
      */
    Uint32 PLLDIV1;

    /** PLL Controller Divider 4 register 
      * This should be configured only for PLLC instance 1
      */
    Uint32 PLLDIV4;

    /** PLL Controller Divider 5 register 
      * This should be configured only for PLLC instance 1
      */
    Uint32 PLLDIV5;
} CSL_PllcConfig;
/**
@} */

/** \defgroup CSL_PLLC_DEFAULT_CONFIG_DEFINE PLLC Default Config Structure
 *  \ingroup CSL_PLLC_DEFINE
 *
 * @{ */
/** \brief Default values for config structure for PLL1
 */
#define CSL_PLLC_CONFIG_DEFAULTS_PLL1 {     \
    CSL_PLLC_PLLCTL_RESETVAL,               \
    CSL_PLLC_PLLM_RESETVAL,                 \
    CSL_PLLC_PREDIV_RESETVAL,               \
    0,                                      \
    CSL_PLLC_PLLDIV4_RESETVAL,              \
    CSL_PLLC_PLLDIV5_RESETVAL               \
}


/** \brief Default values for config structure for PLL2
 */
#define CSL_PLLC_CONFIG_DEFAULTS_PLL2 {     \
    0,                                      \
    0,                                      \
    0,                                      \
    CSL_PLLC_PLLDIV1_RESETVAL,              \
    0,                                      \
    0                                       \
}
/**
@} */

/**
\addtogroup CSL_PLLC_DATASTRUCT
@{ */
/** \brief Module specific context information. Present implementation of pllc
 *   CSL doesn't have any context information.
 */
typedef struct {
    /** Context information of Pllc CSL.
     *  The below declaration is just a place-holder for future implementation.
     */
    Uint16  contextInfo;
} CSL_PllcContext;
/**
@} */

/**
\addtogroup CSL_PLLC_DATASTRUCT
@{ */
/** \brief Module specific parameters. Present implementation of pllc CSL
 *         doesn't have any module specific parameters.
 */
typedef struct {
    /** Bit mask to be used for module specific parameters. The below
     *  declaration is just a place-holder for future implementation.
     */
    CSL_BitMask16   flags;
} CSL_PllcParam;
/**
@} */

/**
\addtogroup CSL_PLLC_DATASTRUCT
@{ */
/** \brief This structure contains the base-address information for the
 *         peripheral instance of the PLLC
 */
typedef struct {
    /** Base-address of the configuration registers of the peripheral
     */
    CSL_PllcRegsOvly  regs;
} CSL_PllcBaseAddress;
/**
@} */

/**
\addtogroup CSL_PLLC_DATASTRUCT
@{ */
/** \brief This object contains the reference to the instance of PLLC
 *         opened using the @a CSL_pllcOpen()
 *
 *  The pointer to this is passed to all PLLC CSL APIs
 *  This structure has the fields required to configure PLLC for any test
 *  case/application. It should be initialized as per requirements of a
 *  test case/application and passed on to the setup function
 */
typedef struct CSL_PllcObj {
    /** This is a pointer to the registers of the instance of PLLC
     *   referred to by this object
     */
    CSL_PllcRegsOvly regs;
    /** This is the instance of PLLC being referred to by this object */
    CSL_InstNum  pllcNum;
} CSL_PllcObj;
/**
@} */

/**
\addtogroup CSL_PLLC_DATASTRUCT
@{ */

/** \brief This data type is used to return the handle to the pllc functions */
typedef struct CSL_PllcObj *CSL_PllcHandle;
/**
@} */

/*******************************************************************************
 * PLLC function declarations
 ******************************************************************************/

/**
\defgroup CSL_PLLC_INIT_API PLLC Init API
\ingroup CSL_PLLC_FUNCTION
@{*/
/** ============================================================================
 *   @n@b CSL_pllcInit
 *
 *   @b Description
 *   @n This is the initialization function for the pllc CSL. The function
 *      must be called before calling any other API from this CSL. This
 *      function is idem-potent. Currently, the function just return status
 *      CSL_SOK, without doing anything.
 *
 *   @b Arguments
 *   @verbatim
        pContext    Pointer to module-context. As pllc doesn't have
                    any context based information user is expected to pass NULL.
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                        CSL_SOK - Always returns
 *
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
            CSL_pllcInit(NULL);
     @endverbatim
 *  ============================================================================
 */
CSL_Status CSL_pllcInit (
    CSL_PllcContext       *pContext
);
/**
@} */

/**
\defgroup CSL_PLLC_OPEN_API PLLC Open API
\ingroup CSL_PLLC_FUNCTION
@{*/
/** ============================================================================
 *   @n@b CSL_pllcOpen
 *
 *   @b Description
 *   @n This function returns the handle to the PLLC
 *      instance. This handle is passed to all other CSL APIs.
 *
 *   @b Arguments
 *   @verbatim
            pllcObj     Pointer to pllc object.

            pllcNum     Instance of pllc CSL to be opened.
                        There is only one instance of the pllc available.
                        So, the value for this parameter will be
                        CSL_PLLC always.

            pPllcParam  Module specific parameters.

            status      Status of the function call
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_PllcHandle
 *   @n                     Valid pllc handle will be returned if
 *                          status value is equal to CSL_SOK.
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n   1.    The status is returned in the status variable. If status
 *              returned is
 *   @li            CSL_SOK            - Valid pllc handle is returned
 *   @li            CSL_ESYS_FAIL      - The pllc instance is invalid
 *   @li            CSL_ESYS_INVPARAMS - Invalid parameter
 *   @n   2.    PLLC object structure is populated
 *
 *   @b Modifies
 *   @n    1. The status variable
 *   @n    2. PLLC object structure
 *
 *   @b Example
 *   @verbatim
            CSL_status           status;
            CSL_PllcObj          pllcObj;
            CSL_PllcHandle       hPllc;
            ...

            hPllc = CSL_pllcOpen(&pllcObj, CSL_PLLC, NULL, &status);
            ...

    @endverbatim
 *  ============================================================================
 */
CSL_PllcHandle CSL_pllcOpen (
    CSL_PllcObj               *pllcObj,
    CSL_InstNum                pllcNum,
    CSL_PllcParam             *pPllcParam,
    CSL_Status                *status
);
/**
@} */

/**
\defgroup CSL_PLLC_GETBASEADDRESS_API PLLC GetBaseAddress API
\ingroup CSL_PLLC_FUNCTION
@{*/
/** ============================================================================
 *   @n@b CSL_pllcGetBaseAddress
 *
 *   @b Description
 *   @n  Function to get the base address of the peripheral instance.
 *       This function is used for getting the base address of the peripheral
 *       instance. This function will be called inside the CSL_pllcOpen()
 *       function call. This function is open for re-implementing if the user
 *       wants to modify the base address of the peripheral object to point to
 *       a different location and there by allow CSL initiated write/reads into
 *       peripheral. MMR's go to an alternate location.
 *
 *   @b Arguments
 *   @verbatim
            pllcNum         Specifies the instance of the pllc to be opened.

            pPllcParam      Module specific parameters.

            pBaseAddress    Pointer to base address structure containing base
                            address details.
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_OK             - Open call is successful
 *   @li                    CSL_ESYS_FAIL      - The instance number is invalid.
 *   @li                    CSL_ESYS_INVPARAMS - Invalid parameter
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  Base address structure is populated
 *
 *   @b Modifies
 *   @n    1. The status variable
 *   @n    2. Base address structure is modified.
 *
 *   @b Example
 *   @verbatim
         CSL_Status              status;
         CSL_PllcBaseAddress  baseAddress;

         ...

         status = CSL_pllcGetBaseAddress(CSL_PLLC_PER_CNT, NULL,
                  &baseAddress);
         ...
    @endverbatim
 *  ============================================================================
 */
CSL_Status CSL_pllcGetBaseAddress (
    CSL_InstNum                      pllcNum,
    CSL_PllcParam                   *pPllcParam,
    CSL_PllcBaseAddress             *pBaseAddress
);
/**
@} */

/**
\defgroup CSL_PLLC_CLOSE_API PLLC Close API
\ingroup CSL_PLLC_FUNCTION
@{*/
/** ============================================================================
 *   @n@b CSL_pllcClose
 *
 *   @b Description
 *   @n This function closes the specified instance of PLLC.
 *
 *   @b Arguments
 *   @verbatim
            hPllc            Handle to the pllc
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK             - Close successful
 *   @li                    CSL_ESYS_BADHANDLE  - Invalid handle
 *
 *   @b Example
 *   @verbatim
            CSL_PllcHandle   hPllc;
            CSL_status       status;
            ...

            status = CSL_pllcClose(hPllc, &hwSetup);
     @endverbatim
 *  ============================================================================
 */
CSL_Status CSL_pllcClose (
    CSL_PllcHandle         hPllc
);
/**
@} */

/**
\defgroup CSL_PLLC_SETUP_API PLLC Setup API
\ingroup CSL_PLLC_FUNCTION
@{*/
/** ============================================================================
 *   @n@b CSL_pllcHwSetup
 *
 *   @b Description
 *   @n It configures the pllc registers as per the values passed
 *      in the hardware setup structure.
 *
 *   @b Arguments
 *   @verbatim
            hPllc            Handle to the pllc

            hwSetup          Pointer to harware setup structure
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK             - Hardware setup successful
 *   @li                    CSL_ESYS_BADHANDLE  - Invalid handle
 *   @li                    CSL_ESYS_INVPARAMS  - Hardware structure is not
                                                  properly initialized
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  PLLC registers are configured according
 *       to the hardware setup parameters
 *
 *   @b Modifies
 *   @n PLLC registers
 *
 *   @b Example
 *   @verbatim
            CSL_PllcHandle   hPllc;
            CSL_PllcObj      pllcObj;
            CSL_PllcHwSetup  hwSetup;
            CSL_status       status;
            ...

            hPllc = CSL_pllcOpen(&pllcObj, CSL_PLLC, NULL, &status);
            ...

            status = CSL_pllcHwSetup(hPllc, &hwSetup);
     @endverbatim
 *  ============================================================================
 */
CSL_Status CSL_pllcHwSetup (
    CSL_PllcHandle            hPllc,
    CSL_PllcHwSetup          *hwSetup
);
/**
@} */

/**
\defgroup CSL_PLLC_CONTROL_API PLLC Control API
\ingroup CSL_PLLC_FUNCTION
@{*/
/** ============================================================================
 *   @n@b CSL_pllcHwControl
 *
 *   @b Description
 *   @n Takes a command of PLLC with an optional argument & implements it.
 *
 *   @b Arguments
 *   @verbatim
            hPllc           Handle to the PLLC instance

            cmd             The command to this API indicates the action to be
                            taken on PLLC.

            arg             An optional argument.
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li            CSL_SOK               - Status info return successful.
 *   @li            CSL_ESYS_BADHANDLE    - Invalid handle
 *   @li            CSL_ESYS_INVCMD       - Invalid command
 *   @li            CSL_ESYS_INVPARAMS    - Invalid parameter
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n The hardware registers of PLLC.
 *
 *   @b Example
 *   @verbatim
        CSL_PllcHandle         hPllc;
        CSL_PllcHwControlCmd   cmd;
        void                      arg;
        ...

        status = CSL_pllcHwControl (hPllc, cmd, &arg);
     @endverbatim
 *  ============================================================================
 */
CSL_Status CSL_pllcHwControl (
    CSL_PllcHandle              hPllc,
    CSL_PllcHwControlCmd        cmd,
    void                       *cmdArg
);
/**
@} */

/**
\defgroup CSL_PLLC_QUERY_API PLLC Query API
\ingroup CSL_PLLC_FUNCTION
@{*/
/** ============================================================================
 *   @n@b CSL_pllcGetHwStatus
 *
 *   @b Description
 *   @n Gets the status of the different operations of PLLC.
 *
 *   @b Arguments
 *   @verbatim
            hPllc         Handle to the PLLC instance

            query         The query to this API of PLLC which indicates the
                          status to be returned.

            response      Placeholder to return the status.
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK             - Status info return successful
 *   @li                    CSL_ESYS_BADHANDLE  - Invalid handle
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
 *   @n None
 *
 *   @b Example
 *   @verbatim
        CSL_PllcHandle          hPllc;
        CSL_PllcHwStatusQuery   query;
        void                    reponse;
        ...

        status = CSL_pllcGetHwStatus (hPllc, query, &response);
     @endverbatim
 *  ============================================================================
 */
CSL_Status CSL_pllcGetHwStatus (
    CSL_PllcHandle                hPllc,
    CSL_PllcHwStatusQuery         query,
    void                         *response
);
/**
@} */

/**
\defgroup CSL_PLLC_HWSETUPRAW_API PLLC HwSetupRaw API
\ingroup CSL_PLLC_FUNCTION
@{*/
/** ============================================================================
 *   @n@b CSL_pllcHwSetupRaw
 *
 *   @b Description
 *   @n This function initializes the device registers with the register-values
 *      provided through the Config Data structure.
 *
 *   @b Arguments
 *   @verbatim
            hPllc        Handle to the PLLC instance

            config       Pointer to config structure
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li            CSL_SOK               - Configuration successful
 *   @li            CSL_ESYS_BADHANDLE    - Invalid handle
 *   @li            CSL_ESYS_INVPARAMS    - Configuration is not
                                            properly initialized
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  The registers of the specified PLLC instance will be setup
 *       according to input configuration structure values.
 *
 *   @b Modifies
 *   @n Hardware registers of the specified PLLC instance.
 *
 *   @b Example
 *   @verbatim
        CSL_PllcHandle       hPllc;
        CSL_PllcConfig       config = CSL_PLLC_CONFIG_DEFAULTS;
        CSL_Status           status;
        ...

        status = CSL_pllcHwSetupRaw (hPllc, &config);
     @endverbatim
 *  ============================================================================
 */
CSL_Status CSL_pllcHwSetupRaw (
    CSL_PllcHandle               hPllc,
    CSL_PllcConfig              *config
);
/**
@} */

/**
\defgroup CSL_PLLC_GETSETUP_API PLLC GetSetup API
\ingroup CSL_PLLC_FUNCTION
@{*/
/** ============================================================================
 *   @n@b CSL_pllcGetHwSetup
 *
 *   @b Description
 *   @n It retrives the hardware setup parameters of the pllc
 *      specified by the given handle.
 *
 *   @b Arguments
 *   @verbatim
            hPllc        Handle to the pllc

            hwSetup      Pointer to the harware setup structure
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
 *   @n  None
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
            CSL_PllcHandle   hPllc;
            CSL_PllcHwSetup  hwSetup;
            ...

            status = CSL_pllcGetHwSetup(hPllc, &hwSetup);
     @endverbatim
 *  ============================================================================
 */
CSL_Status CSL_pllcGetHwSetup (
    CSL_PllcHandle               hPllc,
    CSL_PllcHwSetup             *hwSetup
);
/**
@} */

#ifdef __cplusplus
}
#endif

#endif /* _CSL_PLLC_H_ */

