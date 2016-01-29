/*  ============================================================================
 *  Copyright (c) Texas Instruments Inc 2002, 2003, 2004, 2005 
 *
 *  Use of this software is controlled by the terms and conditions found in the
 *  license agreement under which this software has been supplied.
 *  ============================================================================
 */
 
/** ============================================================================
 *  @mainpage I2C CSL 3.x
 *
 *  @section Introduction
 *
 *  @subsection xxx Purpose and Scope
 *  The purpose of this document is to identify a set of common CSL APIs for
 *  the I2C module across various devices. The CSL developer is expected to
 *  refer to this document while designing APIs for this module. Some of the
 *  listed APIs may not be applicable to a given I2C module. While other cases
 *  this list of APIs may not be sufficient to cover all the features of a
 *  particular I2C Module. The CSL developer should use his discretion designing
 *  new APIs or extending the existing ones to cover these.
 *
 *  @subsection aaa Terms and Abbreviations
 *   -# CSL:  Chip Support Library
 *   -# API:  Application Programmer Interface
 *
 *  @subsection References
 *    -# CSL-001-DES, CSL 3.x Design Specification DocumentVersion 1.02
 *    -# i2c_fs_25.pdf, I2C Peripheral Module 
 *       Design Requirement/Specification V2.5 
 */

/** ============================================================================
 *  @file   csl_i2c.h
 *
 *  @brief  Header file for functional layer of CSL
 *
 *  @path  $(CSLPATH)\inc
 *
 *  Description
 *    - The different enumerations, structure definitions
 *      and function declarations
 *

 *
 *  Modification 1
 *    - modified on: 28/5/2004
 *    - reason: Created the sources
 *
 *  @date   28th May, 2004
 *  @author Santosh Narayanan.
 *  ===========================================================================
 */

/* =============================================================================
 *  Revision History
 *  ===============
 *  30-aug-2004 Hs updated CSL_I2cObj and added CSL_I2cBaseAddress, CSL_i2cParam,
 *                 CSL_i2cContext,  CSL_i2cConfig structures.
 *                 - Updated comments for H/W control cmd and status query enums.
 *                 - Added prototypes for CSL_i2cGetBaseAdddress and
 *                   CSL_I2cHwSetupRaw.
 *                 - Changed prototypes of CSL_i2cInit, CSL_i2cOpen.
 *                 - Updated respective comments along with that of CSL_i2cClose.
 *  11-oct-2004 Hs updated according to code review comments.
 *  28-jul-2005 sv removed gpio support 
 *  06-Feb-2006 ds removed CSL_I2C_QUERY_REV, CSL_I2C_QUERY_CLASS and 
 *                 CSL_I2C_QUERY_TYPE queries from the CSL_I2cHwStatusQuery
 *  ===========================================================================
 */

#ifndef _CSL_I2C_H_
#define _CSL_I2C_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <csl.h>
#include <soc.h>
#include <cslr_i2c.h>

/**************************************************************************\
* I2C global macro declarations
\**************************************************************************/

/** Constants for passing parameters to the I2C Hardware Setup function
 */
/** For setting the SLAVE Mode for I2C                                      */
#define CSL_I2C_MODE_SLAVE                            (0)
/** For setting the MASTER Mode for I2C                                     */
#define CSL_I2C_MODE_MASTER                           (1)                   
/** For setting the RECEIVER Mode for I2C                                   */
#define CSL_I2C_DIR_RECEIVE                           (0)                   
/** For setting the TRANSMITTER Mode for I2C                                */
#define CSL_I2C_DIR_TRANSMIT                          (1)                   
/** For setting the 7-bit Addressing Mode for I2C                           */
#define CSL_I2C_ADDRSZ_SEVEN                          (0)                   
/** For setting the 10-bit Addressing Mode                                  */
#define CSL_I2C_ADDRSZ_TEN                            (1)                   
/** For Disabling the Start Byte Mode for I2C(Normal Mode)                  */
#define CSL_I2C_STB_DISABLE                           (0)                   
/** For Enabling the Start Byte Mode for I2C                                */
#define CSL_I2C_STB_ENABLE                            (1)
/** For enabling the tx of a ACK to the TX-ER, while in the RECEIVER mode   */
#define CSL_I2C_ACK_ENABLE                            (0)
/** For enabling the tx of a NACK to the TX-ER, while in the RECEIVER mode  */
#define CSL_I2C_ACK_DISABLE                           (1)
/** For enabling the free run mode of the I2C                               */
#define CSL_I2C_FREE_MODE_ENABLE                      (1)                   
/** For disabling the free run mode of the I2C                              */
#define CSL_I2C_FREE_MODE_DISABLE                     (0)                   
/** For enabling the Repeat Mode of the I2C                                 */
#define CSL_I2C_REPEAT_MODE_ENABLE                    (1)                   
/** For disabling the Repeat Mode of the I2C                                */
#define CSL_I2C_REPEAT_MODE_DISABLE                   (0)
/** For enabling DLB mode of I2C (applicable only in case of MASTER TX-ER)  */
#define CSL_I2C_DLB_ENABLE                            (1)
/** For disabling DLB mode of I2C (applicable only in case of MASTER TX-ER) */
#define CSL_I2C_DLB_DISABLE                           (0)
/** For putting the I2C in Reset                                            */
#define CSL_I2C_IRS_ENABLE                            (0)                   
/** For taking the I2C out of Reset                                         */
#define CSL_I2C_IRS_DISABLE                           (1)                   
/** For enabling the Free Data Format of I2C                                */
#define CSL_I2C_FDF_ENABLE                            (1)                   
/** For disabling the Free Data Format of I2C                               */
#define CSL_I2C_FDF_DISABLE                           (0)                   
/** For enabling the Backward Compatibility mode of I2C                     */
#define CSL_I2C_BCM_ENABLE                            (1)                   
/** For disabling the Backward Compatibility mode of I2C                    */
#define CSL_I2C_BCM_DISABLE                           (0)

/** Constants for passing parameters to the I2C Status Query function
*/
/** For indicating the non-completion of Reset                              */
#define CSL_I2C_RESET_NOT_DONE                        (0)                   
/** For indicating the completion of Reset                                  */
#define CSL_I2C_RESET_DONE                            (1)                   
/** For indicating that the bus is not busy                                 */
#define CSL_I2C_BUS_NOT_BUSY                          (0)                   
/** For indicating that the bus is busy                                     */
#define CSL_I2C_BUS_BUSY                              (1)                   
/** For indicating that the Receive ready signal is low                     */
#define CSL_I2C_RX_NOT_READY                          (0)                   
/** For indicating that the Receive ready signal is high                    */
#define CSL_I2C_RX_READY                              (1)                   
/** For indicating that the Transmit ready signal is low                    */
#define CSL_I2C_TX_NOT_READY                          (0)                   
/** For indicating that the Transmit ready signal is high                   */
#define CSL_I2C_TX_READY                              (1)                   
/** For indicating that the Access ready signal is low                      */
#define CSL_I2C_ACS_NOT_READY                         (0)                   
/** For indicating that the Access ready signal is high                     */
#define CSL_I2C_ACS_READY                             (1)                   
/** For indicating Single Byte Data signal is set                           */
#define CSL_I2C_SINGLE_BYTE_DATA                      (1)                   
/** For indicating Receive overflow signal is set                           */
#define CSL_I2C_RECEIVE_OVERFLOW                      (1)                   
/** For indicating Transmit underflow signal is set                         */
#define CSL_I2C_TRANSMIT_UNDERFLOW                    (1)                   
/** For indicating Arbitration Lost signal is set                           */
#define CSL_I2C_ARBITRATION_LOST                      (1)                   
                                                                            
/** Constants for status bit clear                                          
 */                                                                         
/** Clear the Arbitration Lost status bit                                   */
#define CSL_I2C_CLEAR_AL                              0x1                                                
/** Clear the No acknowledge status bit                                     */
#define CSL_I2C_CLEAR_NACK                            0x2                                              
/** Clear the Register access ready status bit                              */
#define CSL_I2C_CLEAR_ARDY                            0x4                                              
/** Clear the Receive ready status bit                                      */
#define CSL_I2C_CLEAR_RRDY                            0x8                                              
/** Clear the Transmit ready status bit                                     */
#define CSL_I2C_CLEAR_XRDY                            0x10                                             
/** Clear the Stop Condition Detect  status bit                             */
#define CSL_I2C_CLEAR_SCD                             0x20

/**************************************************************************\
* I2C global typedef declarations
\**************************************************************************/

/** @brief This object contains the reference to the instance of I2C opened
 *  using the @a CSL_i2cOpen().
 *
 *  The pointer to this, is passed to all I2C CSL APIs.
 */
typedef struct CSL_I2cObj {
    /** The register overlay structure of I2C.
    */
    CSL_I2cRegsOvly regs;
    /** This is the instance of I2C being referred to by this object  
    */
    CSL_InstNum perNum;
} CSL_I2cObj;

/** @brief Holds the base-address information for I2C peripheral
 *  instance
 */
typedef struct {
    /** Base-address of the Configuration registers of I2C.
     */
    CSL_I2cRegsOvly regs;
} CSL_I2cBaseAddress;

/** @brief I2C specific parameters. Present implementation doesn't have
 *  any specific parameters.
 */
typedef struct {
    /** Bit mask to be used for module specific parameters.
     *  The below declaration is just a place-holder for future
     *  implementation.
     */
    CSL_BitMask16   flags;
} CSL_I2cParam;

/** @brief I2C specific context information. Present implementation doesn't
 *  have any Context information.
 */
typedef struct {
    /** Context information of I2C.
     *  The below declaration is just a place-holder for future
     *  implementation.
     */
    Uint16  contextInfo;
} CSL_I2cContext;

/** @brief I2C Configuration Structure, is used to configure I2C
 * using CSL_HwSetupRaw function.
 */
typedef struct {
    volatile Uint32 ICOAR;      /**< I2C Own address register       */
    volatile Uint32 ICIMR;      /**< I2C Interrupt Mask register    */
    volatile Uint32 ICSTR;      /**< I2C Status register            */
    volatile Uint32 ICCLKL;     /**< I2C Clock low register         */
    volatile Uint32 ICCLKH;     /**< I2C clock high register        */
    volatile Uint32 ICCNT;      /**< I2C Data Count register        */
    volatile Uint32 ICSAR;      /**< I2C Slave address register     */
    volatile Uint32 ICDXR;      /**< I2C Data Transmit register     */
    volatile Uint32 ICMDR;      /**< I2C Data Receive register      */
    volatile Uint32 ICIVR;      /**< I2C Interrupt vector register  */
    volatile Uint32 ICEMDR;     /**< I2C Extended mode register     */
    volatile Uint32 ICPSC;      /**< I2C Prescalar register         */
} CSL_I2cConfig;

/** Default Values for Config structure */
#define CSL_I2C_CONFIG_DEFAULTS {   \
        CSL_I2C_ICOAR_RESETVAL,     \
        CSL_I2C_ICIMR_RESETVAL,     \
        CSL_I2C_ICSTR_RESETVAL,     \
        CSL_I2C_ICCLKL_RESETVAL,    \
        CSL_I2C_ICCLKH_RESETVAL,    \
        CSL_I2C_ICCNT_RESETVAL,     \
        CSL_I2C_ICSAR_RESETVAL,     \
        CSL_I2C_ICDXR_RESETVAL,     \
        CSL_I2C_ICMDR_RESETVAL,     \
        CSL_I2C_ICIVR_RESETVAL,     \
        CSL_I2C_ICEMDR_RESETVAL,    \
        CSL_I2C_ICPSC_RESETVAL,     \
}

/** @brief Handle to the I2C object
 *  Handle is used in all accesses to the device parameters.
 */
typedef struct CSL_I2cObj *CSL_I2cHandle;

/** @brief The clock setup structure has all the fields required to configure
 *  the I2C clock.
 */
typedef struct {
    /** Prescalar to the input clock     */
    Uint32 prescalar;
    /** Low time period of the clock     */
    Uint32 clklowdiv;
    /** High time period of the clock    */
    Uint32 clkhighdiv;
} CSL_I2cClkSetup;


/** @brief This has all the fields required to configure I2C at Power Up
 *  (After a Hardware Reset) or a Soft Reset
 *
 *  This structure is used to setup or obtain existing setup of
 *  I2C using @a CSL_i2cHwSetup() & @a CSL_i2cGetHwSetup() functions
 *  respectively.
 */
typedef struct {
    /** Master or Slave Mode : 1==> Master Mode, 0==> Slave Mode    */
    Uint32 mode;
    /** Transmitter Mode or Receiver Mode: 1==> Transmitter Mode,
    *  0 ==> Receiver Mode
    */
    Uint32 dir;
    /** Addressing Mode :0==> 7-bit Mode, 1==> 10-bit Mode          */
    Uint32 addrMode;
    /** Start Byte Mode : 1 ==> Start Byte Mode, 0 ==> Normal Mode  */
    Uint32 sttbyteen;
    /** Address of the own device                                   */
    Uint32 ownaddr;
    /** ACK mode while receiver: 0==> ACK Mode, 1==> NACK Mode      */
    Uint32 ackMode;
    /** Run mode of I2C: 0==> No Free Run, 1==> Free Run mode       */
    Uint32 runMode;
    /** Repeat Mode of I2C: 0==> No repeat mode 1==> Repeat mode    */
    Uint32 repeatMode;
    /** DLBack mode of I2C (master tx-er only):
    *   0 ==> No loopback,
    *   1 ==> Loopback Mode
    */
    Uint32 loopBackMode;
    /** Free Data Format of I2C:
    *   0 ==>Free data format disable,
    *   1 ==> Free data format enable
    */
    Uint32 freeDataFormat;
    /** I2C Reset Mode: 0==> Reset, 1==> Out of reset               */
    Uint32 resetMode;
    /** I2C Backward Compatibility Mode :
    *   0 ==> Not compatible,
    *   1 ==> Compatible
    */
    Uint32 bcm;
    /** Interrupt Enable mask The mask can be for one interrupt or
    *  OR of multiple interrupts.
    */
    Uint32 inten;
    /** Prescalar, Clock Low and Clock High for Clock Setup         */
    CSL_I2cClkSetup *clksetup;
} CSL_I2cHwSetup;


/** @brief Enumeration for queries passed to @a CSL_i2cGetHwStatus()
 *
 * This is used to get the status of different operations or to get the
 * existing setup of I2C.
 */
typedef enum {
    /**
    * @brief   Get current clock setup parameters
    * @param   (CSL_I2cClkSetup *)
    */
    CSL_I2C_QUERY_CLOCK_SETUP   = 1,
    /**
    * @brief   Get the Bus Busy status information
    * @param   (Uint16*)
    */
    CSL_I2C_QUERY_BUS_BUSY      = 2,
    /**
    * @brief   Get the Receive Ready status information
    * @param   (Uint16*)
    */
    CSL_I2C_QUERY_RX_RDY        = 3,
    /**
    * @brief   Get the Transmit Ready status information
    * @param   (Uint16*)
    */
    CSL_I2C_QUERY_TX_RDY        = 4,
    /**
    * @brief   Get the Register Ready status information
    * @param   (Uint16*)
    */
    CSL_I2C_QUERY_ACS_RDY       = 5,
    /**
    * @brief   Get the Stop Condition Data bit information
    * @param   (Uint16*)
    */
    CSL_I2C_QUERY_SCD           = 6,
    /**
    * @brief   Get the Address Zero (General Call) detection status
    * @param   (Uint16*)
    */
    CSL_I2C_QUERY_AD0           = 7,
    /**
    * @brief   Get the Receive overflow status information
    * @param   (Uint16*)
    */
    CSL_I2C_QUERY_RSFULL        = 8,
    /**
    * @brief   Get the Transmit underflow status information
    * @param   (Uint16*)
    */
    CSL_I2C_QUERY_XSMT          = 9,
    /**
    * @brief   Get the Address as Slave bit information
    * @param   (Uint16*)
    */
    CSL_I2C_QUERY_AAS           = 10,
    /**
    * @brief   Get the Arbitration Lost status information
    * @param   (Uint16*)
    */
    CSL_I2C_QUERY_AL            = 11,
    /**
    * @brief   Get the Reset Done status bit information
    * @param   (Uint16*)
    */
    CSL_I2C_QUERY_RDONE         = 12,
    /**
    * @brief   Get no of bits of next byte to be received or transmitted
    * @param   (Uint16*)
    */
    CSL_I2C_QUERY_BITCOUNT      = 13,
    /**
    * @brief   Get the interrupt code for the interrupt that occured
    * @param   (Uint16*)
    */
    CSL_I2C_QUERY_INTCODE       = 14,
    /**
    * @brief   Get the slave direction
    * @param   (Uint16*)
    */
    CSL_I2C_QUERY_SDIR          = 15,
    /**
    * @brief   Get the acknowledgement status
    * @param   (Uint16*)
    */
    CSL_I2C_QUERY_NACKSNT       = 16
} CSL_I2cHwStatusQuery;


/** @brief Enumeration for queries passed to @a CSL_i2cHwControl()
 *
 * This is used to select the commands to control the operations
 * existing setup of I2C. The arguments to be passed with each
 * enumeration if any are specified next to the enumeration.
 */
 typedef enum {
    /**
    * @brief   Enable the I2C
    * @param   (None)
    */
    CSL_I2C_CMD_ENABLE           = 1,
    /**
    * @brief   Reset command to the I2C
    * @param   (None)
    */
    CSL_I2C_CMD_RESET            = 2,

    /**
    * @brief   Bring the I2C out of reset
    * @param   (None)
    */
    CSL_I2C_CMD_OUTOFRESET       = 3,

    /**
    * @brief   Clear the status bits. The argument next to the command
    *          specifies the status bit to be cleared. The status bit
    *          can be :
    *            CSL_I2C_CLEAR_AL,
    *            CSL_I2C_CLEAR_NACK,
    *            CSL_I2C_CLEAR_ARDY,
    *            CSL_I2C_CLEAR_RRDY,
    *            CSL_I2C_CLEAR_XRDY,
    *            CSL_I2C_CLEAR_GC
    * @param   (None)
    */
    CSL_I2C_CMD_CLEAR_STATUS     = 4,

    /**
    * @brief   Set the address of the Slave device
    * @param   (Uint16 *)
    */
    CSL_I2C_CMD_SET_SLAVE_ADDR   = 5,

    /**
    * @brief   Set the Data Count
    * @param   (Uint16 *)
    */
    CSL_I2C_CMD_SET_DATA_COUNT   = 6,

    /**
    * @brief   Set the start condition
    * @param   (None)
    */
    CSL_I2C_CMD_START            = 7,

    /**
    * @brief   Set the stop condition
    * @param   (None)
    */
    CSL_I2C_CMD_STOP             = 8,

    /**
    * @brief   Set the transmission mode
    * @param   (None)
    */
    CSL_I2C_CMD_DIR_TRANSMIT     = 9,

    /**
    * @brief   Set the receiver mode
    * @param   (None)
    */
    CSL_I2C_CMD_DIR_RECEIVE      = 10,

    /**
    * @brief   Set the Repeat Mode
    * @param   (None)
    */
    CSL_I2C_CMD_RM_ENABLE        = 11,

    /**
    * @brief   Disable the Repeat Mode
    * @param   (None)
    */
    CSL_I2C_CMD_RM_DISABLE       = 12,

    /**
    * @brief   Set the loop back mode
    * @param   (None)
    */
    CSL_I2C_CMD_DLB_ENABLE       = 13,

    /**
    * @brief   Disable the loop back mode
    * @param   (None)
    */
    CSL_I2C_CMD_DLB_DISABLE      = 14
} CSL_I2cHwControlCmd;



/**************************************************************************\
* I2C global function declarations
\**************************************************************************/

/** ============================================================================
 *   @n@b CSL_i2cInit
 *
 *   @b Description
 *   @n This is the initialization function for the I2C. This function is
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
     if (CSL_sysInit() == CSL_SOK) {
        CSL_i2cInit();
     }
    @endverbatim
 *  ===========================================================================
 */
CSL_Status CSL_i2cInit (
    CSL_I2cContext    *pContext
);



/** ============================================================================
 *   @n@b CSL_i2cOpen
 *
 *   @b Description
 *   @n This function populates the peripheral data object for the instance
 *        and returns a handle to the instance.
 *        The open call sets up the data structures for the particular instance
 *        of I2C device. The device can be re-opened anytime after it has been
 *        normally closed if so required. The handle returned by this call is
 *        input as an essential argument for rest of the APIs described for
 *        this module.
 *
 *   @b Arguments
 *   @verbatim

            hI2c            Handle to I2C instance
            
            pI2cObj         Pointer to the I2C instance object
 
            i2cNum          Instance of the I2C to be opened.
 
            pI2cParam       Pointer to module specific parameters
 
            pStatus         pointer for returning status of the function call

     @endverbatim
 *
 *   <b> Return Value </b>  
 *      CSL_I2cHandle
 *        Valid I2C instance handle will be returned if status value is
 *        equal to CSL_SOK.
 *
 *   <b> Pre Condition </b>
 *   @n  @a CSL_i2cInit() must be called successfully.
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b  Modifies
 *   @n  None
 *
 *   @b Example:
 *   @verbatim
         CSL_status        status;
         CSL_I2cObj        i2cObj;
         CSL_I2cHandle     hI2c;
 
         hI2c = CSL_I2cOpen (&i2cObj,
                             CSL_I2C,
                             NULL,
                             &status
                             );
     @endverbatim
 *
 * ===========================================================================
 */
CSL_I2cHandle CSL_i2cOpen (
    /*
     * Pointer to the object that holds reference to the
     * instance of I2C requested after the call
     */
    CSL_I2cObj      *hI2cObj,
    /*
     * Instance of I2C to which a handle is requested
     */
    CSL_InstNum     i2cNum,
    /*
     * Specifies if I2C should be opened with exclusive or
     * shared access to the associate pins
     */
    CSL_I2cParam    *pI2cParam,
    /*
     * This returns the status (success/errors) of the call
     */
    CSL_Status      *status
);



/** ============================================================================
 *   @n@b CSL_i2cClose
 *
 *   @b Description
 *   @n This function closes the specified instance of I2C.
 *
 *   @b Arguments
 *   @verbatim
            hI2c            Handle to the I2C

     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK             - Close Successful
 *   @li                    CSL_ESYS_BADHANDLE  - Invalid handle
 *
 *   <b> Pre Condition </b>
 *   @n  The device should be successfully opened.
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n I2C Handle
 *
 *   @b Example
 *   @verbatim
            CSL_I2cHandle   hI2c;

            ...

            status = CSL_i2cClose(hI2c);

            ...

 *    @endverbatim
 * ===========================================================================
 */
CSL_Status CSL_i2cClose (
    /*
     * Pointer to the object that holds reference to the
     * instance of I2C requested after the call
     */
    CSL_I2cHandle    hI2c
);



/** ============================================================================
 *   @n@b CSL_i2cHwSetup
 *
 *   @b Description
 *   @n This function initializes the device registers with the appropriate 
        values provided through the HwSetup Data structure. This function needs 
        to be called only if the HwSetup Structure was not previously passed 
        through the Open call. After the Setup is completed, the device is ready
        for  operation.For information passed through the HwSetup Data structure 
        refer @a CSL_i2cHwSetup.
 *   @b Arguments
 *   @verbatim
            hI2c            Handle to the I2C

     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK             - Close Successful
 *   @li                    CSL_ESYS_BADHANDLE  - Invalid handle
 *
 *   <b> Pre Condition </b>
 *   @n  Both @a CSL_i2cInit() and @a CSL_i2cOpen() must be called successfully
 *       in that order before this function can be called. The user has to 
 *       allocate space for & fill in the main setup structure appropriately 
 *       before calling this function.
 *
 *   <b> Post Condition </b>
 *   @n  Both @a CSL_i2cInit() and @a CSL_i2cOpen() must be called successfully
 *       in that order before @a CSL_i2cHwSetup() can be called.
 *
 *   @b Modifies
 *   @n The registers of the specified I2C instance will be setup
 *      according to value passed.
 *
 *   @b Modifies
 *   @n Hardware registers of the specified I2C instance.
 *
 *   @b Example
 * @verbatim
     CSL_i2cHandle hI2c;
     CSL_i2cHwSetup hwSetup = CSL_I2C_HWSETUP_DEFAULTS;
        
        ...
        
     CSL_i2cHwSetup(hI2c, &hwSetup);
        
        ...
  @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK             - HW setup is successful
 *   @li                    CSL_ESYS_BADHANDLE  - Invalid handle
 *   @li                    CSL_ESYS_INVPARAMS  - Invalid parameter
 * ===========================================================================
 */
CSL_Status CSL_i2cHwSetup (
    /*
     * Pointer to the object that holds reference to the
     *  instance of I2C requested after the call
     */
    CSL_I2cHandle     hI2c,
    /*
     * Pointer to setup structure which contains the
     * information to program I2C to a useful state
     */
    CSL_I2cHwSetup    *setup
);



/** ============================================================================
 *   @n@b CSL_i2cGetHwSetup
 *
 *   @b Description
 *   @n This function gets the current setup of the I2C. The status is
 *      returned through @a CSL_I2cHwSetup. The obtaining of status
 *      is the reverse operation of @a CSL_i2cHwSetup() function.
 *
 *   @b Arguments
 *   @verbatim
            hI2c            Handle to the I2C

            hwSetup         Pointer to the harware setup structure

     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK             - Retrieving the hardware setup
 *                                                parameters is successful
 *   @li                    CSL_ESYS_BADHANDLE  - Invalid handle
 *   @li                    CSL_ESYS_INVPARAMS  - Invalid parameter
 *
 *   <b> Pre Condition </b>
 *   @n  Both @a CSL_i2cInit() and @a CSL_i2cOpen() must be called successfully
 *       in that order before @a CSL_i2cGetHwSetup() can be called.
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
            CSL_I2cHandle   hI2c;
            CSL_I2cHwSetup  hwSetup;

            ...

            status = CSL_i2cGetHwSetup(hI2c, &hwSetup);

            ...

     @endverbatim
 * ===========================================================================
 */
CSL_Status  CSL_i2cGetHwSetup (
    /*
     * Pointer to the object that holds reference to the
     * instance of I2C requested after the call
     */
    CSL_I2cHandle     hI2c,
    /*
     * Pointer to setup structure which contains the
     * information to program I2C to a useful state
     */
    CSL_I2cHwSetup    *setup
);



/** ============================================================================
 *   @n@b CSL_i2cHwControl
 *
 *   @b Description
 *   @n Control operations for the I2C.  For a particular control operation, 
 *      the pointer to the corresponding data type need to be passed as argument 
 *      to HwControlfunction Call.All the arguments(Structure elements included) 
 *      passed to the HwControl function are inputs. For the list of commands 
 *      supported and argument type that can be @a void* casted & passed with a 
 *      particular command refer to @a CSL_I2cHwControlCmd.
 *   @b Arguments
 *   @verbatim
            hI2c        Handle to the I2C instance

            cmd         The command to this API indicates the action to be
                        taken on I2C.

            arg         An optional argument.

     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li         CSL_SOK               - Status info return successful.
 *   @li         CSL_ESYS_BADHANDLE    - Invalid handle
 *   @li         CSL_ESYS_INVCMD       - Invalid command
 *   @li         CSL_ESYS_INVPARAMS    - Invalid parameter
 *
 *   <b> Pre Condition </b>
 *   @n  Both @a CSL_i2cInit() and @a CSL_i2cOpen() must be called successfully
 *       in that order before @a CSL_i2cHwControl() can be called.
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n The hardware registers of I2C.
 *
 *   @b Example
 *   @verbatim
        CSL_I2cHandle         hI2c;
        CSL_I2cHwControlCmd   cmd;
        void                  arg;

        status = CSL_i2cHwControl (hI2c, cmd, &arg);

     @endverbatim
 * =============================================================================
 */
CSL_Status CSL_i2cHwControl (
    /* 
     * Pointer to the object that holds reference to the
     * instance of I2C requested after the call
     */
    CSL_I2cHandle          hI2c,
    /*
     * The command to this API indicates the action to be taken
     */
    CSL_I2cHwControlCmd    cmd,
    /*
     * An optional argument @a void* casted
     */
    void                   *arg
);



/** ============================================================================
 *   @n@b CSL_i2cRead
 *
 *   @b Description
 *   @n This function reads I2C data.
 *
 *   @b Arguments
 *   @verbatim

            hI2c            Handle to I2C instance
            buf             Buffer to store the data read

     @endverbatim
 *
 *   <b> Return Value </b>  None
 *
 *   <b> Pre Condition </b>
 *   @n  Both @a CSL_i2cInit() and @a CSL_i2cOpen() must be called successfully
 *       in that order before @a CSL_i2cRead() can be called.
 *
 *   <b> Post Condition </b>
 *   @n None
 *
 *   @b Modifies
 *   @n None
 *
 * @b Example:
 * @verbatim

        Uint16        outData;
      CSL_Status status;
      CSL_I2cHandle hI2c;
      ...
      // I2C object defined and HwSetup structure defined and initialized
      ...

      // Init, Open, HwSetup successfully done in that order
      ...

        status = CSL_i2cRead (hI2c, &outData);
   @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK             - Operation Successful
 *   @li                    CSL_ESYS_BADHANDLE  - Invalid handle
 *   @li                    CSL_ESYS_INVPARAMS  - Invalid parameter
 * ===========================================================================
 */
CSL_Status CSL_i2cRead (
     /*
      *  Handle of already opened peripheral
      */
    CSL_I2cHandle    hI2c,
     /*
      * Pointer to memory where data will be read and stored
      */
    void             *buf
);



/** ============================================================================
 *   @n@b CSL_i2cWrite
 *
 *   @b Description
 *   @n This function writes the specified data into I2C data register.
 *
 *   @b Arguments
 *   @verbatim

            hI2c            Handle to I2C instance
            buf             data to be written

     @endverbatim
 *
 *   <b> Return Value </b>  None
 *
 *   <b> Pre Condition </b>
 *   @n  Both @a CSL_i2cInit() and @a CSL_i2cOpen() must be called successfully
 *       in that order before @a CSL_i2cWrite() can be called.
 *
 *   <b> Post Condition </b>
 *    @n None
 *
 *   @b Modifies
 *   @n  None
 *
 * @b Example:
 * @verbatim

      Uint16     inData;
      CSL_Status status;
      CSL_I2cHandle hI2c;
      ...
      // I2C object defined and HwSetup structure defined and initialized
      ...

      // Init, Open, HwSetup successfully done in that order
      ...

        status = CSL_i2cWrite(hi2c, &inData);
   @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK             - Success (doesnot verify
 *                                                         written data)
 *   @li                    CSL_ESYS_BADHANDLE  - Invalid handle
 *   @li                    CSL_ESYS_INVPARAMS  - Invalid parameter
 * ===========================================================================
 */
CSL_Status CSL_i2cWrite (
     /*
      * Handle of already opened peripheral
      */
    CSL_I2cHandle    hI2c,
     /*
      * Pointer to data to be written
      */
    void             *buf
);



/** ============================================================================
 *   @n@b CSL_i2cHwSetupRaw
 *
 *   @b Description
 *   @n This function initializes the device registers with the register-values
 *      provided through the Config Data structure.
 *
 *   @b Arguments
 *   @verbatim
            hI2c        Handle to the I2C

            config          Pointer to config structure
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK             - Configuration successful
 *   @li                    CSL_ESYS_BADHANDLE  - Invalid handle
 *   @li                    CSL_ESYS_INVPARAMS  - Configuration is not
 *                                                properly initialized
 *
 *   @n Both @a CSL_i2cInit() and @a CSL_i2cOpen() must be called successfully
 *      in that order before @a CSL_i2cHwSetupRaw() can be called.
 *
 *   <b> Post Condition </b>
 *   @n  The registers of the specified I2C instance will be setup
 *       according to value passed.
 *
 *   @b Modifies
 *   @n Hardware registers of the specified I2C instance.
 *
 *   @b Example
 *   @verbatim
        CSL_I2cHandle       hI2c;
        CSL_I2cConfig       config = CSL_I2C_CONFIG_DEFAULTS;
        CSL_Status          status;

        status = CSL_i2cHwSetupRaw (hI2c, &config);

     @endverbatim
 * ===========================================================================
 */
CSL_Status CSL_i2cHwSetupRaw (
    CSL_I2cHandle    hI2c,
    CSL_I2cConfig    *config
);



/** ============================================================================
 *   @n@b CSL_i2cGetHwStatus
 *
 *   @b Description
 *   @n This function is used to read the current device configuration, status
 *      flags and the value present associated registers.Following table details
 *      the various status queries supported and the associated data structure 
 *      to record the response. User should allocate memory for the said data 
 *      type and pass its pointer as an unadorned void* argument to the status 
 *      query call.For details about the various status queries supported and 
 *      the associated data structure to record the response, 
 *      refer to @a CSL_I2cHwStatusQuery
 *
 *   @b Arguments
 *   @verbatim
            hI2c      Handle to the I2C instance

            query     The query to this API of I2C which indicates the
                      status to be returned.

            response  Placeholder to return the status.

     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK             - Status info return successful
 *   @li                    CSL_ESYS_BADHANDLE  - Invalid handle
 *   @li                    CSL_ESYS_INVQUERY   - Invalid query command
 *   @li                    CSL_ESYS_INVPARAMS  - Invalid parameter
 *
 *   <b> Pre Condition </b>
 *   @n  Both @a CSL_i2cInit() and @a CSL_i2cOpen() must be called successfully
 *       in that order before @a CSL_i2cGetHwStatus() can be called.
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
        CSL_I2cHandle          hI2c;
        CSL_I2cHwStatusQuery   query;
        void                       reponse;

        status = CSL_Geti2cHwStatus (hI2c, query, &response);

     @endverbatim
 * =============================================================================
 */
CSL_Status CSL_i2cGetHwStatus (
    /* 
     * Pointer to the object that holds reference to the
     * instance of I2C requested after the call
     */
    CSL_I2cHandle           hI2c,
    /*
     * The query to this API which indicates the status
     * to be returned
     */
    CSL_I2cHwStatusQuery    query,
    /* Placeholder to return the status. @a void* casted */
    void                    *response
);



/** ============================================================================
 *   @n@b CSL_i2cGetBaseAddress
 *
 *   @b Description
 *   @n  Function to get the base address of the peripheral instance.
 *       This function is used for getting the base address of the peripheral
 *       instance. This function will be called inside the CSL_i2cOpen()
 *       function call. This function is open for re-implementing if the user
 *       wants to modify the base address of the peripheral object to point to
 *       a different location and there by allow CSL initiated write/reads into
 *       peripheral MMR's go to an alternate location.
 *
 *   @b Arguments
 *   @verbatim
            i2cNum          Specifies the instance of I2C to be opened.

            pI2cParam       Module specific parameters.

            pBaseAddress    Pointer to baseaddress structure containing base
                            address details.

     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_OK            Open call is successful
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
        CSL_I2cBaseAddress  baseAddress;

       ...
      status = CSL_i2cGetBaseAddress(CSL_I2C_PER_CNT, NULL, &baseAddress);

    @endverbatim
 * ===========================================================================
 */
CSL_Status CSL_i2cGetBaseAddress (
    CSL_InstNum           i2cNum,
    CSL_I2cParam          *pI2cParam,
    CSL_I2cBaseAddress    *pBaseAddress
);

#ifdef __cplusplus
}
#endif


#endif
