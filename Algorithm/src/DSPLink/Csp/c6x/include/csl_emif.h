/** @mainpage EMIF CSL 3.x
 *
 * @section Introduction
 *
 * @subsection xxx Purpose and Scope
 * The purpose of this document is to identify a set of common CSL APIs for
 * the EMIF module across various devices. The CSL developer is expected to
 * refer to this document while designing APIs for these modules. Some of the
 * listed APIs may not be applicable to a given EMIF module. While other cases
 * this list of APIs may not be sufficient to cover all the features of a
 * particular EMIF Module. The CSL developer should use his discretion designing
 * new APIs or extending the existing ones to cover these.
 *
 * @subsection aaa Terms and Abbreviations
 *   -# CSL:  Chip Support Library
 *   -# API:  Application Programmer Interface
 *
 * @subsection References
 *    -# CSL-001-DES, CSL 3.x Design Specification DocumentVersion 1.02
 *    -# EMIF-SPEC, EMIF Module Specifications DocumentVersion 2.1.7
 *
 */

/** @file csl_emif.h
 *
 * @brief    Header file for functional layer of CSL
 *
 * Description
 *    - The different enumerations, structure definitions
 *      and function declarations
 *
 * Modification 1
 *    - modified on: 10/6/2004
 *    - reason: Created the sources
 *
 *
 * @date 10th June, 2004
 * @author Santosh Narayanan.
 *
 */
#ifndef _CSL_EMIF_H_
#define _CSL_EMIF_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <cslr.h>
#include <csl_error.h>
#include <csl_sysData.h>
#include <csl_types.h>
#include <cslr_emif.h>

/**************************************************************************\
* EMIF global macro declarations
\**************************************************************************/

/** Constants for passing parameters to the EMIF HwSetup function.
 */

/** For disabling the Self Refresh mode of SDRAM           */
#define CSL_EMIF_SELF_REFRESH_DISABLE                   (0)
/** For enabling the Self Refresh mode of SDRAM           */
#define CSL_EMIF_SELF_REFRESH_ENABLE                    (1)
/** For disabling the Power down mode of SDRAM           */
#define CSL_EMIF_POWER_DOWN_DISABLE                     (0)
/** For enabling the Power down mode of SDRAM           */
#define CSL_EMIF_POWER_DOWN_ENABLE                      (1)
/** For disabling the Refresh during Power down mode of SDRAM  */
#define CSL_EMIF_PDWR_DISABLE                           (0)
/** For enabling the Refresh during Power down mode of SDRAM  */
#define CSL_EMIF_PDWR_ENABLE                            (1)
/** For setting normal drive strength for DDR SDRAM drive */
#define CSL_EMIF_DDR_NORMAL_DRIVE                       (0)
/** For setting weak drive strength for DDR SDRAM drive */
#define CSL_EMIF_DDR_WEAK_DRIVE                         (1)
/** For disabling the SDRAM narrow mode */
#define CSL_EMIF_NM_DISABLE                             (0)
/** For enabling the SDRAM narrow mode */                   
#define CSL_EMIF_NM_ENABLE                              (1)
/** For disabling the DLL select for DDR1 SDRAM  */
#define CSL_EMIF_DDR_DLL_DISABLE                        (1)
/** For enabling the DLL select for DDR1 SDRAM  */
#define CSL_EMIF_DDR_DLL_ENABLE                         (0)
/** For de-selecting Strobe mode for Async banks */
#define CSL_EMIF_ASYNC_SS_DISABLE                       (0)
/** For selecting Strobe mode for Async banks */
#define CSL_EMIF_ASYNC_SS_ENABLE                        (1)
/** For disabling the Extended Wait mode for Async banks */
#define CSL_EMIF_ASYNC_EW_DISABLE                       (0)
/** For enabling the Extended Wait mode for Async banks */
#define CSL_EMIF_ASYNC_EW_ENABLE                        (1)
/** For disabling NAND FLASH on chip select 5 */
#define CSL_EMIF_CS5_NAND_DISABLE                       (0)
/** For enabling NAND FLASH on chip select 5 */
#define CSL_EMIF_CS5_NAND_ENABLE                        (1)
/** For disabling NAND FLASH on chip select 4 */
#define CSL_EMIF_CS4_NAND_DISABLE                       (0)
/** For enabling NAND FLASH on chip select 4 */
#define CSL_EMIF_CS4_NAND_ENABLE                        (1)
/** For disabling NAND FLASH on chip select 3 */
#define CSL_EMIF_CS3_NAND_DISABLE                       (0)
/** For enabling NAND FLASH on chip select 3 */
#define CSL_EMIF_CS3_NAND_ENABLE                        (1)
/** For disabling NAND FLASH on chip select 2 */
#define CSL_EMIF_CS2_NAND_DISABLE                       (0)
/** For enabling NAND FLASH on chip select 2 */
#define CSL_EMIF_CS2_NAND_ENABLE                        (1)


/**************************************************************************\
* EMIF global typedef declarations
\**************************************************************************/

/** @brief This object contains the reference to the instance of EMIF opened
 *  using the @a CSL_emifOpen().
 *
 *  The pointer to this, is passed to all EMIF CSL APIs.
 */
typedef struct CSL_EmifObj {
	/** This is the mode which the CSL instance is opened     */
	CSL_OpenMode openMode;
	/** This is a unique identifier to the instance of EMIF being
	 *  referred to by this object
	 */
	CSL_Uid uid;
	/** This is the variable that contains the current state of a
	 *  resource being shared by current instance of EMIF with
     *  other peripherals
     */
	CSL_Xio xio;
	/** This is a pointer to the registers of the instance of EMIF
     *  referred to by this object
     */
	CSL_EmifRegsOvly regs;
	/** This is the instance of EMIF being referred to by this object  */
	CSL_EmifNum perNum;
}CSL_EmifObj;

typedef struct CSL_EmifObj *CSL_EmifHandle;

/** @brief SDRAM Configuration structure.
*
* All fields needed for SDRAM Bank configuration are present in this structure.
 */
typedef struct CSL_EmifSdramConfig_{
       /** Self Refresh mode: 0==> No refresh, 1==> Self Refresh*/
	Uint16 selfRefresh;   
	/** Power down mode: 0==> No power down, 1==> Power down mode */
	Uint16 powerDown;
	/** Power down with Refresh: 0==> No Autorefresh, 1==> Exit power down and 
	      perform autorefresh */
	Uint16 pdwr;
	/** DDR SDRAM drive strength: 0==> Normal drive strength, 1==> Weak drive strength */
	Uint16 ddrDrSt;
	/** Bit 17 Enable: 0==> Disable Bit 17, 1==> Enable Bit 17 */
	Uint16 bit17Enable;
	/**  EMIF data bus width: 0==> 32 bit, 1==> 16 bit   */
	Uint16 narrowMode;
	/** Disable DLL for DDR SDRAM: 0==> Enable, 1==> Disable */
	Uint16 disableDdrDll;
	/** Bit 13 Enable: 0==> Disable Bit 13, 1==> Enable Bit 13 */
	Uint16 bit13Enable;
	/** CAS Latency */
	Uint16 casLatency;
	/** Bit 9-11 Enable: 0==> Disable, 1==> Enable */
	Uint16 bit911Enable;
	/** Number of Internal SDRAM banks */
	Uint16 intBank;
	/** External SDRAM bank Setup: 0==>CS0 used for SDRAM, 
	      1==>CS0 & CS1 used for SDRAM */
	Uint16 extBank;
	/** Page Size of the internal SDRAM devices: 0==>256-word, 1==> 512-word, 
	      2==>1024-word, 3==>2048-word */
	Uint16 pageSize;
}CSL_EmifSdramConfig;

/** @brief SDRAM Refresh Control structure.
*
* All fields needed for SDRAM Refresh control are present in this structure.
 */
typedef struct CSL_EmifSdramRefreshControl_{
	/** DDR Refresh Threshold */
	Uint16 ddrRefreshThresh;
	/** Refresh Rate */
	Uint16 refreshRate;
}CSL_EmifSdramRefreshControl;

/** @brief SDRAM Timing structure.
*
* All fields needed for SDRAM Timing are present in this structure.
 */
typedef struct CSL_EmifSdramTiming_{

	/** Specifies TRFC value: Minimum number of EMIF cycles from Refresh or Load command 
	      to Refresh or Activate command, minus one */
	Uint16 trfc;
	/** Specifies TRP value: Minimum number of EMIF cycles from Pre-charge to Active or 
	      Refresh command, minus one  */
	Uint16 trp;
	/** Specifies TRCD value: Minimum number of EMIF cycles from Active to Read or 
	      Write command, minus one  */
	Uint16 trcd;
	/** Specifies TWR value: Minimum number of EMIF cycles from last write transfer
	      to Pre-charge command, minus one */
	Uint16 twr;
	/** Specifies TRAS value: Minimum number of EMIF cycles from Activate to 
	      Pre-charge command, minus one  */
	Uint16 tras;
	/** Specifies TRC value: Minimum number of EMIF cycles from Activate command 
	      to Activate command, minus one */
	Uint16 trc;
	/** Specifies TRRD value: Minimum number of EMIF cycles from Activate command 
	      to Activate command for a differnt bank, minus one */
	Uint16 trrd;
	/** Specifies the minimum number of EMIF clock cycles from Self refresh exit to any
	      command, minus one */
	Uint16 txs;
}CSL_EmifSdramTiming;

/** @brief Asynchronous Wait Cycle Configuration structure
*
* All fields needed for Async Wait Cycle configuration are present in this structure.
 */
typedef struct CSL_EmifAsyncWaitCycleConfig_{
     /** Wait polarity for pad_wait_i[3]*/
     Uint16 wp3;
     /** Wait polarity for pad_wait_i[2]*/
     Uint16 wp2;
     /** Wait polarity for pad_wait_i[1]*/
     Uint16 wp1;
     /** Wait polarity for pad_wait_i[0]*/
     Uint16 wp0;
     /** pad_wait_i map bits for chip select 5 */
     Uint16 cs3Wait;	 
     /** pad_wait_i map bits for chip select 4 */
     Uint16 cs2Wait;	 
      /** pad_wait_i map bits for chip select 3 */
     Uint16 cs1Wait;	 
      /** pad_wait_i map bits for chip select 2 */
     Uint16 cs0Wait;	 	  
     /** Maximum external wait cycles */
     Uint16 maxExtWait;	 
}CSL_EmifAsyncWaitCycleConfig;

/** @brief Asynchronous Bank Configuration structure
*
* All fields needed for Async Bank configuration are present in this structure.
 */
typedef struct CSL_EmifAsyncBankConfig_{
     /** Select strobe mode */
     Uint16 selectStrobe;
     /** Extend wait mode */
     Uint16 extWait;
     /** Write strobe setup cycles */
     Uint16 writeSetup;
     /** Write strobe duration cycles */
     Uint16 writeStrobe;
     /** Write strobe hold cycles */
     Uint16 writeHold;	 
     /** Read strobe setup cycles */
     Uint16 readSetup;	 
      /** Read strobe duration cycles */
     Uint16 readStrobe; 
      /** Read strobe hold cycles */
     Uint16 readHold;	 
      /** Turnaround cycles */
     Uint16 turnAround;	 	  
     /** Asyncronous Bank size */
     Uint16 asyncSize;	 
}CSL_EmifAsyncBankConfig;


/** @brief  NAND FLASH Control Register structure
*
*  All fields needed for NAND FLASH Control are present in this structure.
*/
typedef struct CSL_EmifNandFlashControl_{
     /** Chip select 5 Nand */
     Uint16 cs5nand;
     /** Chip select 4 Nand */
     Uint16 cs4nand;
     /** Chip select 3 Nand */
     Uint16 cs3nand;
     /** Chip select 2 Nand */
     Uint16 cs2nand;
}CSL_EmifNandFlashControl;


/** @brief This has all the fields required to configure EMIF at Power Up
 *  (After a Hardware Reset) or a Soft Reset
 *
 *  This structure is used to setup or obtain existing setup of
 *  EMIF using @a CSL_emifHwSetup() & @a CSL_emifGetHwSetup() functions
 *  respectively.
 */
typedef struct CSL_EmifHwSetup_ {
	/** Structure for Async Wait Cycle configuration */
	CSL_EmifAsyncWaitCycleConfig  *asyncWaitCycleConfig;
	/** Structure for SDRAM Refresh control */
	CSL_EmifSdramRefreshControl *emifSdramRefreshControl;
	/** Structure for SDRAM Timing */
	CSL_EmifSdramTiming *sdramTiming;
	/** Structure for SDRAM Bank configuration */
	CSL_EmifSdramConfig *sdramBankConfig;
	/** Structure for Async Bank 1 Config Registers */
	CSL_EmifAsyncBankConfig *asyncBank1Config;
	/** Structure for Async Bank 2 Config Registers */
	CSL_EmifAsyncBankConfig *asyncBank2Config;
	/** Structure for Async Bank 3 Config Registers */
	CSL_EmifAsyncBankConfig *asyncBank3Config;
	/** Structure for Async Bank 4 Config Registers */
	CSL_EmifAsyncBankConfig *asyncBank4Config;
	/** DDR PHY control */
	Uint32 ddrPhyControl;
	/** Structure for NAND FLASH control */
	CSL_EmifNandFlashControl *nandFlashControl;
}CSL_EmifHwSetup;

/** @brief EMIF Module ID and Revision structure
*
*  This structure is used for querying the EMIF module ID and revision 
*/
typedef struct CSL_EmifRevStatus_{
     /** EMIF Module ID */
     Uint16 moduleID;
     /** EMIF Major Revision */
     Uint16 majorRev;
     /** EMIF Minor Revision */
     Uint16 minorRev;
}CSL_EmifRevStatus;	 

/** @brief EMIF DDR Status structure 
*
* This structure holds the DDR PHY Ready, Train CS1 & CS0 and DDR status fields.
*/
typedef struct CSL_EmifDdrStatus_{
      /** DDR PHY ready */
      Uint16 phyDllRdy;
      /** Double Rate */
      Uint16 ddr;
}CSL_EmifDdrStatus;	  


/** @brief NAND FLASH ECC structure
*
* This structure holds the fields in the Nand Flash ECC Register.
*/
typedef struct CSL_EmifNandFlashECC_{
        Uint16 p2048o;
	 Uint16 p1024o;
	 Uint16 p512o;
	 Uint16 p256o;
	 Uint16 p128o;
	 Uint16 p64o;
	 Uint16 p32o;
	 Uint16 p16o;
	 Uint16 p8o;
	 Uint16 p4o;
	 Uint16 p2o;
	 Uint16 p1o;
	 Uint16 p2048e;
	 Uint16 p1024e;
	 Uint16 p512e;
	 Uint16 p256e;
	 Uint16 p128e;
	 Uint16 p64e;
	 Uint16 p32e;
	 Uint16 p16e;
	 Uint16 p8e;
	 Uint16 p4e;
	 Uint16 p2e;
	 Uint16 p1e;
}CSL_EmifNandFlashECC;

/** @brief IODFT MISR structure
*
* This structure holds the three fields in the IODFT MISR Registers - IODFTMRLR, IODFTMRMR, IODFTMRMSBR.
*/
typedef struct CSL_EmifIodftMisr_{
     /** MISR LSB value */
     Uint32 misrLsb;	 
     /** MISR MID value */
     Uint32 misrMid;	 
     /** MISR MSB value */
     Uint32 misrMsb;	 
}CSL_EmifIodftMisr;


/** @brief Enumeration for queries passed to @a CSL_emifGetHwStatus()
 *
 * This is used to get the status of different operations or to get the
 * existing setup of EMIF.
 */
typedef enum {
  /** Get the EMIF module ID and revision numbers (response type: @a (CSL_EmifRevStatus*)) */
  CSL_EMIF_QUERY_REV_ID_STATUS = 1,
  /** Get the EMIF endianness (response type: @a Uint16 *)        */
  CSL_EMIF_QUERY_ENDIANNESS ,
  /** Get the EMIF rate information(response type: @a Uint16 *)    */
  CSL_EMIF_QUERY_RATE ,
  /** Get the EMIF DDR status information(response type: @a (CSL_EmifDdrStatus *))   */
  CSL_EMIF_QUERY_DDR_STATUS ,
  /** Get the EMIF DDR PHY status information(response type: @a Uint16 *)   */
  CSL_EMIF_QUERY_DDR_PHY_STATUS ,
  /** Get the total SDRAM accesses (response type: @a Uint16 *)   */
  CSL_EMIF_QUERY_SDRAM_ACCESS,
  /** Get the total SDRAM accesses that needed an activate command (response type: @a Uint16 *)   */
  CSL_EMIF_QUERY_SDRAM_ACTIVATE,
  /** Get the DDR PHY ID and Revision information (response type: @a Uint32 *)   */
  CSL_EMIF_QUERY_DDR_ID_REV,
  /** Get the Wait Rise Interrupt Status (response type: @a Uint16 *)   */
  CSL_EMIF_QUERY_WR_INT_STATUS,
  /** Get the Line Trap Interrupt Status (response type: @a Uint16 *)   */
  CSL_EMIF_QUERY_LT_INT_STATUS,
  /** Get the Asynchronous Timeout Interrupt Status (response type: @a Uint16 *)   */
  CSL_EMIF_QUERY_AT_INT_STATUS,
  /** Get the IO status (response type: @a Uint16 *)   */
  CSL_EMIF_QUERY_IO_STATUS,
  /** Get the NAND Flash Status (response type: @a Uint16 *) */
  CSL_EMIF_QUERY_NAND_FLASH_STATUS,
  /** Read the NAND FLASH ECC for Chip select 5 (response type: @a (CSL_EmifNandFlashECC *)) */
  CSL_EMIF_QUERY_CS5_NAND_FLASH_ECC,
   /** Read the NAND FLASH ECC for Chip select 4 (response type: @a (CSL_EmifNandFlashECC *)) */
  CSL_EMIF_QUERY_CS4_NAND_FLASH_ECC,
   /** Read the NAND FLASH ECC for Chip select 3 (response type: @a (CSL_EmifNandFlashECC *)) */
  CSL_EMIF_QUERY_CS3_NAND_FLASH_ECC,
   /** Read the NAND FLASH ECC for Chip select 2 (response type: @a (CSL_EmifNandFlashECC *)) */
  CSL_EMIF_QUERY_CS2_NAND_FLASH_ECC,
  /** Read the number of cycles MISR count will be accumulated in IODFT Execution Control Register
        (response type : @a Uint16 *) */
  CSL_EMIF_QUERY_IODFT_TLEC,
  /** Read the MISR result signature of a given test after the download function is executed 
         (response type : @a (CSL_EmifIodftMisr *)) */
  CSL_EMIF_QUERY_IODFT_MISR,
  /** Read the Release number from Module Release Number Register (response type: @a Uint16 *) */
  CSL_EMIF_QUERY_RELEASE_NUM
  } CSL_EmifHwStatusQuery;


/** @brief Enumeration for queries passed to @a CSL_emifHwControl()
 *
 * This is used to select the commands to control the operations
 * existing setup of EMIF. The arguments to be passed with each
 * enumeration if any are specified next to the enumeration.
 */
 typedef enum {
	/** Set the DDR Refresh Threshold : argument @a (Uint16 *)	 */
	CSL_EMIF_CMD_SET_DDR_REF_THRESH = 1,
	/** Set the Refresh Rate : argument @a (Uint16 *)	 */
	CSL_EMIF_CMD_REF_RATE ,
	/** Enable the Wait Rise Interrupt : no argument */
	CSL_EMIF_CMD_WR_ENABLE,
	/** Disable the Wait Rise Interrupt : no argument */
	CSL_EMIF_CMD_WR_DISABLE,
	/** Enable the Line Trap Interrupt : no argument */
	CSL_EMIF_CMD_LT_ENABLE,
	/** Disable the Line Trap Interrupt : no argument */
	CSL_EMIF_CMD_LT_DISABLE,
	/** Enable the Asynchronous Timeout Interrupt : no argument */
	CSL_EMIF_CMD_AT_ENABLE,
	/** Disable the Asynchronous Timeout Interrupt : no argument */
	CSL_EMIF_CMD_AT_DISABLE,
	/** Control word for IO Control : argument @a (Uint16*) */
	CSL_EMIF_CMD_IO_CONTROL,
	/** Start ECC calculation for Chip Select 5 : no argument*/
	CSL_EMIF_CMD_CS5_ECC_START,
       /** Start ECC calculation for Chip Select 4 : no argument */
	CSL_EMIF_CMD_CS4_ECC_START,
	/** Start ECC calculation for Chip Select 3 : no argument */
	CSL_EMIF_CMD_CS3_ECC_START,
	/** Start ECC calculation for Chip Select 2 : no argument */
	CSL_EMIF_CMD_CS2_ECC_START,
	/** Enable the CS5 NAND FLASH : no argument */
	CSL_EMIF_CMD_CS5_NAND_ENABLE,
	/** Disable the CS5 NAND FLASH : no argument */
	CSL_EMIF_CMD_CS5_NAND_DISABLE,
	/** Enable the CS4 NAND FLASH : no argument */
	CSL_EMIF_CMD_CS4_NAND_ENABLE,
	/** Disable the CS4 NAND FLASH : no argument */
	CSL_EMIF_CMD_CS4_NAND_DISABLE,
	/** Enable the CS3 NAND FLASH : no argument */
	CSL_EMIF_CMD_CS3_NAND_ENABLE,
	/** Disable the CS3 NAND FLASH : no argument */
	CSL_EMIF_CMD_CS3_NAND_DISABLE,
	/** Enable the CS2 NAND FLASH : no argument */
	CSL_EMIF_CMD_CS2_NAND_ENABLE,
	/** Disable the CS2 NAND FLASH : no argument */
	CSL_EMIF_CMD_CS2_NAND_DISABLE,
	/** Set the MISR counter : argument @a (Uint16*) */
	CSL_EMIF_CMD_IODFT_SET_TLEC,
	/** Set the MISR trigger on : no argument */
	CSL_EMIF_CMD_IODFT_MISR_ON,
	/** Set the MISR trigger off : no argument */
	CSL_EMIF_CMD_IODFT_MISR_OFF,
	/** Load the pattern generators initial value : no argument */
	CSL_EMIF_CMD_IODFT_OPGLD,
	/** Set the MISR input source as output register : no argument */
	CSL_EMIF_CMD_IODFT_MMS_OP_REG,
	/** Set the MISR input source as input capture : no argument */
	CSL_EMIF_CMD_IODFT_MMS_INP_CAP,
	/** Set the test mode for Output enable select */
	CSL_EMIF_CMD_IODFT_ESEL_TEST,
	/** Set the normal mode for Output enable select */
	CSL_EMIF_CMD_IODFT_ESEL_NORMAL,
	/** Enable the output for Test Output Enable Control: no argument */
	CSL_EMIF_CMD_IODFT_TOEN_ENABLE,
	/** Disable the output for Test Output Enable Control: no argument */
	CSL_EMIF_CMD_IODFT_TOEN_DISABLE,
	/** Set the MISR state: argument @a (Uint16*) */
	CSL_EMIF_CMD_IODFT_SET_MISR_STATE,
	/** Set the Pattern code: argument @a (Uint16*) */
	CSL_EMIF_CMD_IODFT_SET_PATTERN_CODE,
	/** Set the IODFT to functional mode : no argument */
	CSL_EMIF_CMD_IODFT_FUNC_MODE,
	/** Set to IODFT mode : no argument */
	CSL_EMIF_CMD_IODFT_MODE
		
} CSL_EmifHwControlCmd;



/**************************************************************************\
* EMIF global function declarations
\**************************************************************************/

/**  This function is idempotent in that calling it many times is same as
 *   calling it once. This function initializes the EMIF CSL data structures.
 *
 * <b> Usage Constraints: </b>
 * CSL system initialization must be successfully completed by invoking
 * @a CSL_sysInit() before calling this function. This function should be
 * called before using any of the CSL APIs
 *
 * @b Example:
 * @verbatim


   ...
   CSL_sysInit();
   if (CSL_SOK != CSL_emifInit()) {
       return;
   }
   @endverbatim
 *
 * @return returns the status of the operation
 *
 */
CSL_Status  CSL_emifInit(
    void
);


/** The open call sets up the data structures for the particular instance of
 *  EMIF device. The device can be re-opened anytime after it has been normally
 *  closed if so required. EMIF Hardware setup will be performed at the end of
 *  the open call only if the HwSetup Pointer supplied was non- NULL. The handle
 *  returned by this call is input as an essential argument for rest of the APIs
 *  described for this module.
 *
 *  <b> Usage Constraints: </b>
 *  The CSL system as well as EMIF must be successfully initialized
 *  via @a CSL_sysInit() and @a CSL_emifInit() before calling this
 *  function. Memory for the @a CSL_emifObj must be allocated outside
 *  this call. This object must be retained while usage of this peripheral.
 *
 *  @b Example:
 *  @verbatim

	  CSL_EmifObj     emifObj;
	  CSL_EmifHwSetup emifSetup;
	  CSL_Status       status;
 		...
	  hEmif = CSL_emifOpen(&emifObj,
                          CSL_EMIF_0,
                          CSL_EXCLUSIVE,
                          &emifSetup,
                          &status);
   @endverbatim
 *
 * @return returns a handle @a CSL_EmifHandle to the requested instance of
 * EMIF if the call is successful, otherwise, a @a NULL is returned.
 *
 */
CSL_EmifHandle CSL_emifOpen (
    /** Pointer to the object that holds reference to the
     *  instance of EMIF requested after the call
	 */
    CSL_EmifObj              *hEmifObj,
    /** Instance of EMIF to which a handle is requested
	 */
    CSL_EmifNum              emifNum,
    /** Specifies if EMIF should be opened with exclusive or
     *  shared access to the associate pins
	 */
    CSL_OpenMode            openMode,
    /** If a valid structure is passed (not @a NULL), then
     *  the @a CSL_emifHwSetup() is called with this parameter
	 */
    CSL_EmifHwSetup          *hwSetup,
    /** This returns the status (success/errors) of the call
	 */
    CSL_Status              *status
	);

/**  The Close call releases the resource and appropriate shared pins.
 *
 * <b> Usage Constraints: </b>
 * Both @a CSL_emifInit() and @a CSL_emifOpen() must be called successfully
 * in that order before @a CSL_emifClose() can be called.
 *
 * @b Example:
 * @verbatim


   CSL_EmifHandle hEmif;
   ...
   CSL_emifClose(hEmif);
   @endverbatim
 *
 * @return returns the status of the operation (see @a CSL_Status)
 *
 */
CSL_Status  CSL_emifClose(
    /** Pointer to the object that holds reference to the
     *  instance of EMIF requested after the call
	 */
    CSL_EmifHandle                         hEmif
);


/** This function initializes the device registers with the appropriate values
 *  provided through the HwSetup Data structure. This function needs to be called
 *  only if the HwSetup Structure was not previously passed through the Open call.
 *  After the Setup is completed, the serial device is ready for data transfer.
 *  For information passed through the HwSetup Data structure refer
 *  @a CSL_EmifHwSetup.
 *
 *  <b> Usage Constraints: </b>
 *  Both @a CSL_emifInit() and @a CSL_emifOpen() must be called
 *  successfully in that order before this function can be called. The user
 *  has to allocate space for & fill in the main setup structure appropriately
 *  before calling this function
 *
 * @b Example:
 * @verbatim
     CSL_EmifHandle hEmif;
     CSL_EmifHwSetup hwSetup = CSL_EMIF_HWSETUP_DEFAULTS;
     CSL_emifHwSetup(hEmif, &hwSetup);
  @endverbatim
 *
 * @return Returns the status of the setup operation
 *
 */
CSL_Status  CSL_emifHwSetup(
    /** Pointer to the object that holds reference to the
     *  instance of EMIF requested after the call
	 */
    CSL_EmifHandle                         hEmif,
    /** Pointer to setup structure which contains the
     *  information to program EMIF to a useful state
	 */
    CSL_EmifHwSetup                        *setup
	);

/** This function gets the current setup of the EMIF. The status is
 *  returned through @a CSL_EmifHwSetup. The obtaining of status
 *  is the reverse operation of @a CSL_emifHwSetup() function.
 *
 *  <b> Usage Constraints: </b>
 *  Both @a CSL_emifInit() and @a CSL_emifOpen() must be called successfully
 *  in that order before @a CSL_emifGetHwSetup() can be called.
 *
 * @b Example:
 * @verbatim
	  CSL_EmifHandle hEmif;
	  CSL_Status status;
	  CSL_EmifHwSetup *mysetup;
	   ...
	  status = CSL_emifGetHwSetup(hEmif, &mysetup);
   @endverbatim
 *
 * @return returns the status of the operation (see @a CSL_Status)
 *
 */
CSL_Status  CSL_emifGetHwSetup(
    /** Pointer to the object that holds reference to the
     *  instance of EMIF requested after the call
     */
    CSL_EmifHandle                         hEmif,
    /** Pointer to setup structure which contains the
     *  information to program EMIF to a useful state
     */
    CSL_EmifHwSetup                        *setup
);


/** Control operations for the EMIF.  For a particular control operation, the
 *  pointer to the corresponding data type needs to be passed as argument HwControl
 *  function Call. All the arguments (Structure elements included) passed to  the
 *  HwControl function are inputs. For the list of commands supported and argument
 *  type that can be @a void* casted & passed with a particular command refer to
 *  @a CSL_EmifHwControlCmd.
 *
 *  <b> Usage Constraints: </b>
 *  Both @a CSL_emifInit() and @a CSL_emifOpen() must be called successfully
 *  in that order before @a CSL_emifHwControl() can be called. For the
 *  argument type that can be @a void* casted & passed with a particular command
 *  refer to @a CSL_EmifHwControlCmd
 *
 * @b Example:
 * @verbatim
       CSL_EmifHandle hEmif;
       CSL_Status status;
       ...
       status = CSL_emifHwControl(hEmif,
                                    CSL_EMIF_CMD_START,
                                   &command);
   @endverbatim
 *
 *  @return returns the status of the operation (see @a CSL_Status)
 *
 */
CSL_Status  CSL_emifHwControl(
    /** Pointer to the object that holds reference to the
     *  instance of EMIF requested after the call
	 */
    CSL_EmifHandle                         hEmif,
    /** The command to this API indicates the action to be taken
	 */
    CSL_EmifHwControlCmd                   cmd,
    /** An optional argument @a void* casted
	 */
    void                                     *arg
	);

/** This function is used to read the current device configuration, status flags
 *  and the value present associated registers. Following table details the various
 *  status queries supported and the associated data structure to record the response.
 *  User should allocate memory for the said data type and pass its pointer as an
 *  unadorned void* argument to the status query call. For details about the various
 *  status queries supported and the associated data structure to record the response,
 *  refer to @a CSL_EmifHwStatusQuery
 *
 *  <b> Usage Constraints: </b>
 *  Both @a CSL_emifInit() and @a CSL_emifOpen() must be called successfully
 *  in that order before @a CSL_emifGetHwStatus() can be called. For the
 *  argument type that can be @a void* casted & passed with a particular command
 *  refer to @a CSL_EmifHwStatusQuery
 *
 * @b Example:
 * @verbatim
	  CSL_EmifHandle hEmif;
	  CSL_Status status;
	  Uint16  *response;
	   ...
	  status = CSL_emifGetHwStatus(hEmif,
                           CSL_EMIF_QUERY_BUS_BUSY,
                                  &response);
   @endverbatim
 *
 * @return Returns the status of the operation (see @a CSL_Status)
 *
 */
CSL_Status  CSL_emifGetHwStatus(
    /** Pointer to the object that holds reference to the
     *  instance of EMIF requested after the call
	 */
    CSL_EmifHandle                         hEmif,
    /** The query to this API which indicates the status
     *  to be returned
	 */
    CSL_EmifHwStatusQuery                  query,
    /** Placeholder to return the status. @a void* casted */
    void                                    *response
	);

#ifdef __cplusplus
}
#endif


#endif
/* Rev.No.   Date/Time               ECN No.          Modifier      */
/* -------   ---------               -------          --------      */

/* 3          Aug 10:08:59 9         2272             xkeshavm      */
/*                                                                  */
/* To upload API CSL 0.51 Release                                   */
/********************************************************************/ 
