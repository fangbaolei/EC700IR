/** @mainpage MCASP CSL 3.x
 *
 * @section Introduction
 *
 * @subsection xxx Purpose and Scope
 * The purpose of this document is to identify a set of common CSL APIs
 * for the McASP Module across variousdevices. The CSL developer is expected 
 * to refer to this document while designing APIs for these modules. Some of 
 * the listed APIs may not be applicable to a given McASP Module. While in 
 * other cases this list of APIs may not be sufficient to cover all the features 
 * of a particular McASP Module. The CSL developer should use his discretion in 
 * designing new APIs or extending the existing ones to cover these.
 *
 * @subsection aaa Terms and Abbreviations
 *  -#  CSL:  Chip Support Library
 *  -#  API:  Application Programmer Interface
 *  -# MCASP: Application Programmer Interface
 *
 *
 */

/** @file csl_mcasp.h
 *
 * @brief    Header file for functional layer of CSL
 *
 * Description
 *    - The different enumerations, structure definitions
 *      and function declarations
 *
 * Modification 
 *    - modified on: 10/1/2005
 *    - reason: Created the sources
 *
 * 
 * @author asr.
 *
 */
 
#ifndef _CSL_MCASP_H_
#define _CSL_MCASP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <csl_error.h>
#include <csl_types.h>
#include <davinci_hd.h>

/** @brief McASP0 Tx Buffer Address */
#define MCASP0_TXBUF_ADDR	0x01D01000
/** @brief McASP0 Rx Buffer Address */
#define MCASP0_RXBUF_ADDR	0x01D01000
/** @brief McASP1 Tx Buffer Address */
#define MCASP1_TXBUF_ADDR	0x01D01800
/** @brief McASP1 Rx Buffer Address */
#define MCASP1_RXBUF_ADDR   0x01D01800


/** 
 * @brief McASP Module specific object structure 
 */
typedef struct CSL_McaspObj {
	/** Pointer to the register overlay structure for the peripheral */
	CSL_McaspRegsOvly   regs;
	/** Specifies a particular instance of McASP */
	Int16               perNo;
	/** Number of serializers */
	Int32               numOfSerializers;
	/** Support for DIT mode */
	Bool                ditStatus;
} CSL_McaspObj;

/** 
 * @brief McASP Module specific Hardware setup global structure 
 */
typedef struct CSL_McaspHwSetupGbl {
	/** Pin function register */
	Uint32  pfunc;
	/** Pin direction register */
	Uint32  pdir;
	/** Global control register - GBLCTL*/
	Uint32  ctl;
	/** Decides whether McASP operates in DIT mode - DITCTL */
	Uint32  ditCtl;
	/** Digital loopback mode setup - DLBEN */
	Uint32  dlbMode;
	/** Mute control register - AMUTE */
	Uint32  amute;
	/** Setup serializer control register (SRCTL0-15) */
	Uint32  serSetup[16];
} CSL_McaspHwSetupGbl;

/** 
 * @brief Hardware setup data clock structure 
 */
typedef struct CSL_McaspHwSetupDataClk {
	/** Clock details ACLK(R/X)CTL */
	Uint32  clkSetupClk;
	/** High clock details AHCLK(R/X)CTL */
	Uint32  clkSetupHiClk;
	/** Configures receive/transmit clock failure detection R/XCLKCHK */
	Uint32  clkChk;
} CSL_McaspHwSetupDataClk;

/** 
* @brief Hardware setup data structure 
*/
typedef struct CSL_McaspHwSetupData {
	/** To mask or not to mask - R/XMASK */
	Uint32                      mask;
	/** Format details as per  - R/XFMT */
	Uint32                      fmt;
	/** Configure the rcv/xmt frame sync - AFSR/XCTL */
	Uint32                      frSyncCtl;
	/** Specifies which TDM slots are active - R/XTDM */
	Uint32                      tdm;
	/** Controls generation of McASP interrupts - R/XINTCTL */
	Uint32                      intCtl;
	/** Status register (controls writable fields of STAT register)-R/XSTAT */
	Uint32                      stat;
	/** Event control register - R/XEVTCTL */
	Uint32                      evtCtl;
	/** Clock settings for rcv/xmt */
	CSL_McaspHwSetupDataClk     clk;
} CSL_McaspHwSetupData;

/** 
 * @brief Hardware setup structure 
 */
typedef struct CSL_McaspHwSetup {
	/** Value to be loaded in global control register (GLBCTL) */
	CSL_McaspHwSetupGbl     glb;
	/** Receiver settings */
	CSL_McaspHwSetupData    rx;
	/** Transmitter settings */
	CSL_McaspHwSetupData    tx;
	/** Power down emulation mode params - PWRDEMU */
	Uint32                  emu;
} CSL_McaspHwSetup;

/**
 * @brief DIT channel/user Left/right data structure
 */ 
typedef enum {
	/** 1st DIT (channel/user data), (left/right) Register */
	DIT_REGISTER_0 = 0,
	/** 2nd DIT (channel/user data), (left/right) Register */
	DIT_REGISTER_1 = 1,
	/** 3rd DIT (channel/user data), (left/right) Register */
	DIT_REGISTER_2 = 2,
	/** 4th DIT (channel/user data), (left/right) Register */
	DIT_REGISTER_3 = 3,
	/** 5th DIT (channel/user data), (left/right) Register */
	DIT_REGISTER_4 = 4,
	/** 6th DIT (channel/user data), (left/right) Register */
	DIT_REGISTER_5 = 5
} CSL_McaspDITRegIndex;

/** 
 * @brief DIT channel status register structure 
 */
typedef struct {
	/** Left channel status registers (DITCSRA0-5) */
	Uint32 chStatusLeft[6];
	/** Right channel status register (DITCSRB0-5) */
	Uint32 chStatusRight[6];
} CSL_McaspChStatusRam;

/** 
 * @brief DIT channel user data register structure 
 */
typedef struct {
	/** Left channel user data registers (DITUDRA0-5) */
	Uint32 userDataLeft[6];
	/** Right channel user data registers (DITUDRB0-5) */
	Uint32 userDataRight[6];
} CSL_McaspUserDataRam;

/**
 * @brief Module specific Configuration structure.This is used to configure McASP
 * instance using CSL_mcaspHwSetupRaw function
 */
typedef struct {
	/** Power down and emulation management register */
	Uint32 PWRDEMU;
	/** Pin function register */
	Uint32 PFUNC;
	/** Pin direction register */
	Uint32 PDIR;
	/** Pin data output register */
	Uint32 PDOUT;
	/** Pin data set register */
	Uint32 PDIN_PDSET;
	/** Pin data clear register */
	Uint32 PDCLR;
	/** Global control register */
	Uint32 GBLCTL;
	/** Audio mute control register */
	Uint32 AMUTE;
	/** Digital loopback control register */
	Uint32 DLBCTL;
	/** DIT mode control register */
	Uint32 DITCTL;
	/** Receive format unit bit mask register */
	Uint32 RMASK;
	/** Receive bit stream format register */
	Uint32 RFMT;
	/** Receive frame sync control register */
	Uint32 AFSRCTL;
	/** Receive clock control register */
	Uint32 ACLKRCTL;
	/** Receive high-frequency clock control register */
	Uint32 AHCLKRCTL;
	/** Receive TDM time slot 0-31 register */
	Uint32 RTDM;
	/** Receiver interrupt control register */
	Uint32 RINTCTL;
	/** Receiver status register */
	Uint32 RSTAT;
	/** Receive clock check control register */
	Uint32 RCLKCHK;
	/** Receiver DMA event control register */
	Uint32 REVTCTL;
	/** Transmit format unit bit mask register */
	Uint32 XMASK;
	/** Transmit bit stream format register */
	Uint32 XFMT;
	/** Transmit frame sync control register */
	Uint32 AFSXCTL;
	/** Transmit clock control register */
	Uint32 ACLKXCTL;
	/** Transmit high-frequency clock control register */
	Uint32 AHCLKXCTL;
	/** Transmit TDM time slot 0-31 register */
	Uint32 XTDM;
	/** Transmitter interrupt control register */
	Uint32 XINTCTL;
	/** Transmitter status register */
	Uint32 XSTAT;
	/** Transmit clock check control register */
	Uint32 XCLKCHK;
	/** Transmitter DMA event control register */
	Uint32 XEVTCTL;
	/** Serializer control register 0 */
	Uint32 SRCTL0;
	/** Serializer control register 1 */
	Uint32 SRCTL1;
	/** Serializer control register 2 */
	Uint32 SRCTL2;
	/** Serializer control register 3 */
	Uint32 SRCTL3;
	/** Serializer control register 4 */
	Uint32 SRCTL4;
	/** Serializer control register 5 */
	Uint32 SRCTL5;
	/** Serializer control register 6 */
	Uint32 SRCTL6;
	/** Serializer control register 7 */
	Uint32 SRCTL7;
	/** Serializer control register 8 */
	Uint32 SRCTL8;
	/** Serializer control register 9 */
	Uint32 SRCTL9;
	/** Serializer control register 10 */
	Uint32 SRCTL10;
	/** Serializer control register 11 */
	Uint32 SRCTL11;
	/** Serializer control register 12 */
	Uint32 SRCTL12;
	/** Serializer control register 13 */
	Uint32 SRCTL13;
	/** Serializer control register 14 */
	Uint32 SRCTL14;
	/** Serializer control register 15 */
	Uint32 SRCTL15;
} CSL_McaspConfig;

/** 
 *  @brief Module specific parameters. Present implementation doesn't have
 *  any module specific parameters.
 */
typedef struct {
	/** 
	*  Bit mask to be used for module specific parameters.
	*  The below declaration is just a place-holder for future
	*  implementation.
	*/
	CSL_BitMask32   flags;
} CSL_McaspParam;

/** 
 * @brief Enumeration for the serializer numbers 
 */
typedef enum {
	/** SRCTL0 */
	SERIALIZER_1 = 0,
	/** SRCTL1 */
	SERIALIZER_2 = 1,
	/** SRCTL2 */
	SERIALIZER_3 = 2,
	/** SRCTL3 */
	SERIALIZER_4 = 3,
	/** SRCTL4 */
	SERIALIZER_5 = 4,
	/** SRCTL5 */
	SERIALIZER_6 = 5,
	/** SRCTL5 */
	SERIALIZER_7 = 6,
	/** SRCTL5 */
	SERIALIZER_8 = 7,
	/** SRCTL5 */
	SERIALIZER_9 = 8,
	/** SRCTL5 */
	SERIALIZER_10 = 9,
	/** SRCTL5 */
	SERIALIZER_11 = 10,
	/** SRCTL5 */
	SERIALIZER_12 = 11,
	/** SRCTL5 */
	SERIALIZER_13 = 12,
	/** SRCTL5 */
	SERIALIZER_14 = 13,
	/** SRCTL5 */
	SERIALIZER_15 = 14,
	/** SRCTL5 */
	SERIALIZER_16 = 15
} CSL_McaspSerializerNum;

/** 
 * @brief Enumeration for the serializer mode 
 */
typedef enum {
	/** Serializer is inactive */
	SERIALIZER_INACTIVE = 0,
	/** Serializer is transmitter */
	SERIALIZER_XMT = 1,
	/** Serializer is receiver */
	SERIALIZER_RCV = 2
} CSL_McaspSerMode;

/**
 *  @brief The following stcruture will be used in CSL_MCASP_QUERY_SRCTL_RRDY,
 *  and CSL_MCASP_QUERY_SRCTL_XRDY
 */
typedef struct CSL_McaspSerQuery {
	/** Serializer number */
	CSL_McaspSerializerNum  serNum;
	/** Return value of the query */
	Bool                    serStatus;
} CSL_McaspSerQuery;

/**
 *  @brief The following stcruture will be used in CSL_MCASP_QUERY_SRCTL_SRMOD
 */
typedef struct CSL_McaspSerMmode {
	/** Serializer number */
	CSL_McaspSerializerNum  serNum;
	/** Serializer mode */
	CSL_McaspSerMode        serMode;
} CSL_McaspSerModeQuery;

/**
 *  @brief Enumeration for hardware control commands passed to  @a CSL_mcaspHwControl()
 *
 * This is used to select the commands to control the operations
 * existing setup of McASP. The arguments to be passed with each
 * enumeration if any are specified next to the enumeration. 
 */
typedef enum {
	/**
	* @brief   Configure transmitter global control register  with parameters
	*          passed
	* @param   (Uint32 *)
	*/
	CSL_MCASP_CMD_SET_XMT               = 1,

	/**
	* @brief   Configure receiver global control register  with parameters
	*          passed
	* @param   (Uint32 *)
	*/
	CSL_MCASP_CMD_SET_RCV               = 2,

	/**
	* @brief   Reset transmit frame sync generator
	* @param   (None)
	*/
	CSL_MCASP_CMD_RESET_FSYNC_XMT       = 3,

	/**
	* @brief   Reset receive frame sync generator
	* @param   (None)
	*/
	CSL_MCASP_CMD_RESET_FSYNC_RCV       = 4,

	/**
	* @brief   Reset all registers
	* @param   (None)
	*/
	CSL_MCASP_CMD_REG_RESET             = 5,

	/**
	* @brief   Mute enable
	* @param   (Uint32 *)
	*/
	CSL_MCASP_CMD_AMUTE_ON              = 6,

	/**
	* @brief   Enable digital loopback mode
	* @param   (Uint32 *)
	*/
	CSL_MCASP_CMD_DLB_ON                = 7,

	/**
	* @brief   Configures receive slots
	* @param   (Uint32 *)
	*/
	CSL_MCASP_CMD_CONFIG_RTDM_SLOT     = 8,

	/**
	* @brief   Configures transmit slots
	* @param   (Uint32 *)
	*/
	CSL_MCASP_CMD_CONFIG_XTDM_SLOT     = 9,

	/**
	* @brief   Configures the interrupts on the receive side
	* @param   (Uint32 *)
	*/
	CSL_MCASP_CMD_CONFIG_INTERRUPT_RCV  = 10,

	/**
	* @brief   Configures the interrupts on the transmit side
	* @param   (Uint32 *)
	*/
	CSL_MCASP_CMD_CONFIG_INTERRUPT_XMT  = 11,

	/**
	* @brief   Reset clock circuitry for receive
	* @param   (None)
	*/
	CSL_MCASP_CMD_CLK_RESET_RCV         = 12,

	/**
	* @brief   Reset clock circuitry for transmit
	* @param   (None)
	*/
	CSL_MCASP_CMD_CLK_RESET_XMT         = 13,

	/**
	* @brief   Set receive clock registers with value
	*          (CSL_McaspHwSetupDataClk*) passed
	* @param   (CSL_McaspHwSetupDataClk *)
	*/
	CSL_MCASP_CMD_CLK_SET_RCV           = 14,

	/**
	* @brief   Set transmit clock registers with value
	*          (CSL_McaspHwSetupDataClk*) passed
	* @param   (CSL_McaspHwSetupDataClk *)
	*/
	CSL_MCASP_CMD_CLK_SET_XMT           = 15,

	/**
	* @brief   Configure the format, frame sync, and other parameters related
	*          to the transmit section
	* @param   (CSL_McaspHwSetupData *)
	*/
	CSL_MCASP_CMD_CONFIG_XMT_SECTION = 16,

	/**
	* @brief   Configure the format, frame sync, and other parameters related
	*          to the receive section
	* @param   (CSL_McaspHwSetupData *)
	*/
	CSL_MCASP_CMD_CONFIG_RCV_SECTION = 17,

	/**
	* @brief   Sets a particular serializer to act as transmitter
	* @param   (CSL_McaspSerializerNum *)
	*/
	CSL_MCASP_CMD_SET_SER_XMT           = 18,

	/**
	* @brief   Sets a particular serializer to act as receiver
	* @param   (CSL_McaspSerializerNum *)
	*/
	CSL_MCASP_CMD_SET_SER_RCV           = 19,

	/**
	* @brief   Sets a particular serializer as inactivated
	* @param   (CSL_McaspSerializerNum *)
	*/
	CSL_MCASP_CMD_SET_SER_INA           = 20,

	/**
	* @brief   Writes to the channel status RAM
	* @param   (CSL_McaspChStatusRam *)
	*/
	CSL_MCASP_CMD_WRITE_CHAN_STAT_RAM   = 21,

	/**
	* @brief   Writes to the user data RAM
	* @param   (CSL_McaspUserDataRam *)
	*/
	CSL_MCASP_CMD_WRITE_USER_DATA_RAM   = 22,

	/**
	* @brief   Resets the bits related to transmit in transmitter global
	*          control register
	* @param   (None)
	*/
	CSL_MCASP_CMD_RESET_XMT             = 23,

	/**
	* @brief   Resets the bits related to receive in transmitter global
	*          control register
	* @param   (None)
	*/
	CSL_MCASP_CMD_RESET_RCV             = 24,

	/**
	* @brief   Resets transmit frame sync generator and transmit state
	*          machine in transmitter global control register
	* @param   (None)
	*/
	CSL_MCASP_CMD_RESET_SM_FS_XMT       = 25,

	/**
	* @brief   Resets receive frame sync generator and receive state
	*          machine in receiver global control register
	* @param   (None)
	*/
	CSL_MCASP_CMD_RESET_SM_FS_RCV       = 26,

	/**
	* @brief   Sets the bits related to transmit in transmitter global
	*          control register
	* @param   (None)
	*/
	CSL_MCASP_CMD_ACTIVATE_XMT_CLK_SER  = 27,

	/**
	* @brief   Sets the bits related to receive in receiver global control
	*          register
	* @param   (None)
	*/
	CSL_MCASP_CMD_ACTIVATE_RCV_CLK_SER  = 28,

	/**
	* @brief   Activates receive and transmit state
	*          machine in global control register
	* @param   (None)
	*/
	CSL_MCASP_CMD_ACTIVATE_SM_RCV_XMT   = 29,

	/**
	* @brief   Activates receive and transmit frame sync 
	*          generator in global control register
	* @param   (None)
	*/
	CSL_MCASP_CMD_ACTIVATE_FS_RCV_XMT   = 30,

	/**
	* @brief   Enables/disables the DIT mode
	* @param   (Bool *)
	*/
	CSL_MCASP_CMD_SET_DIT_MODE          = 31
} CSL_McaspHwControlCmd;

/**
 *  @brief Enumeration for hardware status query commands passed to @a CSL_mcaspGetHwStatus()
 *
 *  This is used to get the status of different operations or to get the
 *  existing setup of McASP. 
 */
typedef enum {
	/**
	* @brief   Return current transmit slot being transmitted
	* @param   (Uint16 *)
	*/
	CSL_MCASP_QUERY_CURRENT_XSLOT   = 1,

	/**
	* @brief   Return current receive slot being received
	* @param   (Uint16 *)
	*/
	CSL_MCASP_QUERY_CURRENT_RSLOT   = 2,

	/**
	* @brief   Return transmit error status bit
	* @param   (Bool *)
	*/
	CSL_MCASP_QUERY_XSTAT_XERR      = 3,

	/**
	* @brief   Return transmit clock failure flag status
	* @param   (Bool *)
	*/
	CSL_MCASP_QUERY_XSTAT_XCLKFAIL  = 4,

	/**
	* @brief   Return unexpected transmit frame sync flag status
	* @param   (Bool *)
	*/
	CSL_MCASP_QUERY_XSTAT_XSYNCERR  = 5,

	/**
	* @brief   Return transmit underrun flag status
	* @param   (Bool *)
	*/
	CSL_MCASP_QUERY_XSTAT_XUNDRN    = 6,

	/**
	* @brief   Return transmit data ready flag status
	* @param   (Bool *)
	*/
	CSL_MCASP_QUERY_XSTAT_XDATA     = 7,

	/**
	* @brief   Return receive error status bit
	* @param   (Bool *)
	*/
	CSL_MCASP_QUERY_RSTAT_RERR      = 8,

	/**
	* @brief   Return receive clock failure flag status
	* @param   (Bool *)
	*/
	CSL_MCASP_QUERY_RSTAT_RCLKFAIL  = 9,

	/**
	* @brief   Return unexpected receive frame sync flag status
	* @param   (Bool *)
	*/
	CSL_MCASP_QUERY_RSTAT_RSYNCERR  = 10,

	/**
	* @brief   Return receive overrun flag status
	* @param   (Bool *)
	*/
	CSL_MCASP_QUERY_RSTAT_ROVRN     = 11,

	/**
	* @brief   Return receive data ready flag status
	* @param   (Bool *)
	*/
	CSL_MCASP_QUERY_RSTAT_RDATA     = 12,

	/**
	* @brief   Return status whether the serializer is ready to receive or not
	* @param   (CSL_McaspSerQuery *)
	*/
	CSL_MCASP_QUERY_SRCTL_RRDY      = 13,

	/**
	* @brief   Return status whether the serializer is ready to transmit
	*          or not
	* @param   (CSL_McaspSerQuery *)
	*/
	CSL_MCASP_QUERY_SRCTL_XRDY      = 14,

	/**
	* @brief   Return status whether serializer is configured to
	*          transmit/receive/inactive
	* @param   (CSL_McaspSerQuery *)
	*/
	CSL_MCASP_QUERY_SRCTL_SRMOD     = 15,

	/**
	* @brief   Return the value of transmitter status register. 
	* @param   (Uint16 *)
	*/
	CSL_MCASP_QUERY_XSTAT           = 16,

	/**
	* @brief   Return the value of receiver status register. 
	* @param   (Uint16 *)
	*/
	CSL_MCASP_QUERY_RSTAT           = 17,

	/**
	* @brief   Return the transmit state machine and transmit frame sync
	*          generator values in transmitter global control register 
	* @param   (Uint8 *)
	* @li          0x00 - Both transmit frame generator sync and transmit
	*                     state machine are reset.
	* @li          0x1  - Only transmit state machine is active.
	* @li          0x10 - Only transmit frame sync generator is active.
	* @li          0x11 - Both transmit frame generator sync and transmit
	*                     state machine are active.
	*/
	CSL_MCASP_QUERY_SM_FS_XMT       = 18,

	/**
	* @brief   Return the receive state machine and receive frame sync
	*          generator values in receiver global control register 
	* @param   (Uint8 *)
	* @li          0x00 - Both receive frame generator sync and receive state
	*                     machine are reset.
	* @li          0x1  - Only receive state machine is active.
	* @li          0x10 - Only receive frame sync generator is active.
	* @li          0x11 - Both receive frame generator sync and receive state
	*                     machine are active.
	*/
	CSL_MCASP_QUERY_SM_FS_RCV       = 19,

	/**
	* @brief   Queries whether DIT mode is set or not
	* @param   (Bool *)
	*/
	CSL_MCASP_QUERY_DIT_MODE        = 20
} CSL_McaspHwStatusQuery;

/**
 * @brief This will have the base-address information for the peripheral
 * instance
 */
typedef struct {
	/** Base-address of the configuration registers of the peripheral */
	CSL_McaspRegsOvly   regs;
	/** Number of serializers */
	Int32               numOfSerializers;
	/** Support for DIT mode */
	Bool                ditStatus;
} CSL_McaspChipContext;

/** 
 *  @brief Module specific context information. Present implementation doesn't
 *  have any context information.
 */
typedef struct {
	/** 
	*  Context information of McASP.
	*  The below declaration is just a place-holder for future
	*  implementation.
	*/
	Uint16  contextInfo;
} CSL_McaspContext;

/**
 * @brief This data type is used to return the handle to an
 * instance of McASP
 */
typedef struct CSL_McaspObj    *CSL_McaspHandle;

/** @brief Default values for hardware setup structure */
#define CSL_MCASP_HWSETUP_DEFAULTS {\
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},\
    {0,0,0,0,0,0,0,0x20,0x8000,0},\
	{0,0,0,0,0,0,0,0x20,0x8000,0},\
    0\
}


/** @brief Default values for config structure */
#define CSL_MCASP_CONFIG_DEFAULTS {\
	CSL_MCASP_PWRDEMU_RESETVAL,\
	CSL_MCASP_PFUNC_RESETVAL,\
	CSL_MCASP_PDIR_RESETVAL,\
	CSL_MCASP_PDOUT_RESETVAL,\
	CSL_MCASP_PDIN_PDSET_RESETVAL,\
	CSL_MCASP_PDCLR_RESETVAL,\
	CSL_MCASP_GBLCTL_RESETVAL,\
	CSL_MCASP_AMUTE_RESETVAL,\
	CSL_MCASP_DLBCTL_RESETVAL,\
	CSL_MCASP_DITCTL_RESETVAL,\
	CSL_MCASP_RMASK_RESETVAL,\
	CSL_MCASP_RFMT_RESETVAL,\
	CSL_MCASP_AFSRCTL_RESETVAL,\
	CSL_MCASP_ACLKRCTL_RESETVAL,\
	CSL_MCASP_AHCLKRCTL_RESETVAL,\
	CSL_MCASP_RTDM_RESETVAL,\
	CSL_MCASP_RINTCTL_RESETVAL,\
	CSL_MCASP_RSTAT_RESETVAL,\
	CSL_MCASP_RCLKCHK_RESETVAL,\
	CSL_MCASP_REVTCTL_RESETVAL,\
	CSL_MCASP_XMASK_RESETVAL,\
	CSL_MCASP_XFMT_RESETVAL,\
	CSL_MCASP_AFSXCTL_RESETVAL,\
	CSL_MCASP_ACLKXCTL_RESETVAL,\
	CSL_MCASP_AHCLKXCTL_RESETVAL,\
	CSL_MCASP_XTDM_RESETVAL,\
	CSL_MCASP_XINTCTL_RESETVAL,\
	CSL_MCASP_XSTAT_RESETVAL,\
	CSL_MCASP_XCLKCHK_RESETVAL,\
	CSL_MCASP_XEVTCTL_RESETVAL,\
	CSL_MCASP_SRCTL0_RESETVAL,\
	CSL_MCASP_SRCTL1_RESETVAL,\
	CSL_MCASP_SRCTL2_RESETVAL,\
	CSL_MCASP_SRCTL3_RESETVAL,\
	CSL_MCASP_SRCTL4_RESETVAL,\
	CSL_MCASP_SRCTL5_RESETVAL,\
	CSL_MCASP_SRCTL6_RESETVAL,\
	CSL_MCASP_SRCTL7_RESETVAL,\
	CSL_MCASP_SRCTL8_RESETVAL,\
	CSL_MCASP_SRCTL9_RESETVAL,\
	CSL_MCASP_SRCTL10_RESETVAL,\
	CSL_MCASP_SRCTL11_RESETVAL,\
	CSL_MCASP_SRCTL12_RESETVAL,\
	CSL_MCASP_SRCTL13_RESETVAL,\
	CSL_MCASP_SRCTL14_RESETVAL,\
	CSL_MCASP_SRCTL15_RESETVAL\
} 

/******************************************************************************
 * Multi channel audio serial port global function declarations
 *****************************************************************************/
/**
 * ============================================================================
 *   @func CSL_mcaspInit
 *
 *   @desc
 *   @n     This is the initialization function for McASP CSL. This function
 *          needs to be called before any McASP CSL functions are to be called.
 *          This function is idem-potent. Currently, this function does not
 *          perform anything.
 *
 *   @arg pContext
 *        Context information for McASP
 *
 *   @ret CSL_Status
 *        CSL_SOK - Always returns 
 *
 *   @eg
 *     CSL_mcaspInit (NULL);
 * ============================================================================
 */
extern CSL_Status  CSL_mcaspInit(
    CSL_McaspContext *pContext 
);

/**
 *=============================================================================
 *   @func CSL_mcaspOpen
 *
 *   @desc
 *   @n     This function populates the peripheral data object for the instance
 *          and returns a handle to the instance.
 *
 *   @arg pMcaspObj
 *        Pointer to the McASP instance object 
 *
 *   @arg mcaspNum
 *        Instance of the McASP to be opened. There is two instance of 
 *		  McASP on C672x.
 * 
 *   @arg pMcaspParam
 *        Pointer to module specific parameters
 *
 *   @arg pStatus
 *        pointer for returning status of the function call
 *
 *   @ret CSL_McaspHandle
 *        Valid McASP instance handle will be returned if status value is
 *        equal to CSL_SOK.
 *
 *   @eg
 *        CSL_status          status;
 *        CSL_McaspObj        mcaspObj;
 *        CSL_McaspHandle     hMcasp;
 *
 *        hMcasp = CSL_McaspOpen (&mcaspObj,          
 *                                CSL_MCASP_1, 
 *                                NULL,  
 *                                &status
 *                                );
 *
 * ============================================================================
 */
extern CSL_McaspHandle CSL_mcaspOpen (
    CSL_McaspObj      *pMcaspObj,
    CSL_InstNum       mcaspNum,
    CSL_McaspParam    *pMcaspParam,
    CSL_Status        *pStatus
);

/**
 * ============================================================================
 *   @func CSL_mcaspClose
 *
 *   @desc
 *   @n     This function closes the specified instance of McASP.      
 *      
 *   @arg  hMcasp
           Handle to the McASP instance 
 *
 *   @ret CSL_Status
 *         CSL_SOK            - Close successful
 *         CSL_ESYS_BADHANDLE - Invalid handle
 *
 *   @eg
 *     CSL_McaspHandle     hMcasp;
 * 
 *     CSL_mcaspClose (hMcasp);
 * ============================================================================
 */
extern CSL_Status CSL_mcaspClose (
    CSL_McaspHandle    hMcasp
);

/**
 * ============================================================================
 *   @func CSL_mcaspHwSetup
 *
 *   @desc
 *   @n     Configures the McASP instance with the specified setup parameters.  
 *
 *   @arg hMcasp
 *        Handle to the McASP instance
 *
 *   @arg myHwSetup
 *        Pointer to hardware setup structure
 *
 *   @ret CSL_Status
 *        CSL_SOK             - Hardware setup successful
 *        CSL_ESYS_BADHANDLE  - Invalid handle
 *        CSL_ESYS_INVPARAMS  - Invalid parameters
 *
 *   @eg
 *    
 *       CSL_status          status;
 *       CSL_McaspHwSetup    myHwSetup;
 *       CSL_McaspHandle     hMcasp;
 *
 *      myHwSetup.glb.ctl = 0x00000000;
 *      myHwSetup.glb.ditCtl = 0x00000003;
 *      myHwSetup.glb.dlbMode = 0x00000002;
 *      myHwSetup.glb.amute = 0x00001234;
 *      myHwSetup.glb.serSetup[1] = 0x00000012;
 *      myHwSetup.glb.serSetup[2] = 0x00000012;
 *      myHwSetup.glb.serSetup[3] = 0x00000012;
 *      myHwSetup.glb.serSetup[4] = 0x00000012;
 *      myHwSetup.glb.serSetup[5] = 0x00000012;
 *      myHwSetup.glb.serSetup[6] = 0x00000012;
 *      myHwSetup.glb.serSetup[7] = 0x00000012;
 *      myHwSetup.glb.serSetup[8] = 0x00000012;
 *      myHwSetup.glb.serSetup[9] = 0x00000012;
 *      myHwSetup.glb.serSetup[10] = 0x00000012;
 *      myHwSetup.glb.serSetup[11] = 0x00000012;
 *      myHwSetup.glb.serSetup[12] = 0x00000012;
 *      myHwSetup.glb.serSetup[13] = 0x00000012;
 *      myHwSetup.glb.serSetup[14] = 0x00000012;
 *      myHwSetup.glb.serSetup[15] = 0x00000012;
 *      myHwSetup.rx.mask = 0x11111111;
 *      myHwSetup.tx.mask = 0x11111111;
 *      myHwSetup.rx.fmt = 0x00001111;
 *      myHwSetup.tx.fmt = 0x00001111;
 *      myHwSetup.rx.frSyncCtl = 0x00000001;
 *      myHwSetup.tx.frSyncCtl = 0x00000001;
 *      myHwSetup.rx.tdm = 0x11111111;
 *      myHwSetup.tx.tdm = 0x11111111;
 *      myHwSetup.rx.intCtl = 0x00000111;
 *      myHwSetup.tx.intCtl = 0x00000111;
 *      myHwSetup.rx.stat = 0x00000110;
 *      myHwSetup.tx.stat = 0x00000110;
 *      myHwSetup.rx.evtCtl = 0x00000001;
 *      myHwSetup.tx.evtCtl = 0x00000001;
 *      myHwSetup.rx.clk.clkSetupClk = 0x00000001;
 *      myHwSetup.tx.clk.clkSetupClk = 0x00000001;
 *      myHwSetup.rx.clk.clkSetupHiClk = 0x00000101;
 *      myHwSetup.tx.clk.clkSetupHiClk = 0x00000101;
 *      myHwSetup.rx.clk.clkChk = 0x00432100;
 *      myHwSetup.tx.clk.clkChk = 0x00432100;
 *      myHwSetup.emu = CSL_MCASP_PWRDEMU_FREE_ON;
 *
 *      status = CSL_mcaspHwsetup (hMcasp, &myHwSetup);
 *
 * ============================================================================
 */
extern CSL_Status CSL_mcaspHwSetup (
    CSL_McaspHandle   hMcasp,
    CSL_McaspHwSetup  *myHwSetup
);

/**
 * ============================================================================
 *   @func CSL_mcaspHwSetupRaw
 *
 *   @desc
 *   @n     This function configures the registers of McASP instance as per the 
 *          values given in the config structure.  
 *
 *   @arg hMcasp
 *        Handle to the McASP instance
 *
 *   @arg config
 *        Pointer to McASP config structure
 *
 *   @ret CSL_Status
 *        CSL_SOK             - Configuration successful
 *        CSL_ESYS_BADHANDLE  - Invalid handle
 *        CSL_ESYS_INVPARAMS  - Invalid parameters
 *
 *   @eg
 *        CSL_Status          status;
 *        CSL_McaspConfig     hwConfig;
 *        CSL_McaspHandle     hMcasp;
 *
 *        hwConfig.PWRDEMU = 0x00000001;
 *        hwConfig.PFUNC = 0x00000000;
 *        hwConfig.PDIR = 0x00000000;
 *        hwConfig.PDOUT = 0x00000000;
 *        hwConfig.PDIN_PDSET = 0x00000000;
 *        hwConfig.PDCLR = 0x00000000;
 *        hwConfig.GBLCTL = 0x00000000;
 *        hwConfig.AMUTE = 0x00000000;
 *        hwConfig.DLBCTL = 0x00000000;
 *        hwConfig.DITCTL = 0x00000000;
 *        hwConfig.RMASK = 0x00000000;
 *        hwConfig.RFMT = 0x00000000;
 *        hwConfig.AFSRCTL = 0x00000000;
 *        hwConfig.ACLKRCTL = 0x00000020;
 *        hwConfig.AHCLKRCTL = 0x00008000;
 *        hwConfig.RTDM = 0x00000000;
 *        hwConfig.RINTCTL = 0x00000000;
 *        hwConfig.RSTAT = 0x00000000;
 *        hwConfig.RCLKCHK = 0x00000000;
 *        hwConfig.REVTCTL = 0x00000000;
 *        hwConfig.XMASK = 0x00000000;
 *        hwConfig.XFMT = 0x00000000;
 *        hwConfig.AFSXCTL = 0x00000000;
 *        hwConfig.ACLKXCTL = 0x00000060;
 *        hwConfig.AHCLKXCTL = 0x00008000;
 *        hwConfig.XTDM = 0x00000000;
 *        hwConfig.XINTCTL = 0x00000000;
 *        hwConfig.XSTAT = 0x00000000;
 *        hwConfig.XCLKCHK = 0x00000000;
 *        hwConfig.XEVTCTL = 0x00000000;
 *        hwConfig.SRCTL0 = 0x00000000;
 *        hwConfig.SRCTL1 = 0x00000000;
 *        hwConfig.SRCTL2 = 0x00000000;
 *        hwConfig.SRCTL3 = 0x00000000;
 *        hwConfig.SRCTL4 = 0x00000000;
 *        hwConfig.SRCTL5 = 0x00000000;
 *        hwConfig.SRCTL6 = 0x00000000;
 *        hwConfig.SRCTL7 = 0x00000000;
 *        hwConfig.SRCTL8 = 0x00000000;
 *        hwConfig.SRCTL9 = 0x00000000;
 *        hwConfig.SRCTL10 = 0x00000000;
 *        hwConfig.SRCTL11 = 0x00000000;
 *        hwConfig.SRCTL12 = 0x00000000;
 *        hwConfig.SRCTL13 = 0x00000000;
 *        hwConfig.SRCTL14 = 0x00000000;
 *        hwConfig.SRCTL15 = 0x00000000;
 *
 *        status = CSL_mcaspHwsetupRaw (hMcasp, &hwConfig);
 *
 * ============================================================================
 */

extern CSL_Status  CSL_mcaspHwSetupRaw (
    CSL_McaspHandle         hMcasp,
    CSL_McaspConfig         *config
);

/**
 * ============================================================================
 *   @func CSL_mcaspGetHwSetup
 *
 *   @desc
 *   @n     It retrieves the hardware setup parameters .
 *
 *   @arg hMcasp
 *        Handle to the McASP instance
 *
 *   @arg myHwSetup
 *        Pointer to hardware setup structure
 *
 *   @ret CSL_Status
 *        CSL_SOK             - Hardware setup successfully retrieved
 *        CSL_ESYS_BADHANDLE  - Invalid handle
 *
 *   @eg
 *        CSL_status        status;
 *        CSL_McaspHwSetup  hwSetup;
 *        CSL_McaspHandle   hMcasp;
 *        
 *        status = CSL_mcaspGetHwsetup (hMcasp, &hwSetup);
 *
 * ============================================================================
 */
extern CSL_Status CSL_mcaspGetHwSetup (
    CSL_McaspHandle   hMcasp,
    CSL_McaspHwSetup  *myHwSetup
);

/** 
 * ============================================================================
 *   @func CSL_mcaspHwControl
 *
 *   @desc
 *   @n     This function performs various control operations on the McASP 
 *          instance, based on the command passed.
 *
 *   @arg hMcasp
 *        Handle to the McASP instance
 *
 *   @arg cmd
 *        Operation to be performed on the McASP instance
 *
 *   @arg arg
 *        Argument specific to the command 
 *
 *   @ret CSL_Status
 *        CSL_SOK            - Command execution successful.
 *        CSL_ESYS_BADHANDLE - Invalid handle
 *        CSL_ESYS_INVCMD    - Invalid command
 *
 *   @eg
 *        CSL_Status        status;
 *        CSL_McaspHandle   hMcasp;
 *
 *        status  = CSL_mcaspHwControl (hMcasp,  
 *              CSL_MCASP_CMD_CLK_RESET_XMT, NULL);
 *
 * ============================================================================
 */
extern CSL_Status CSL_mcaspHwControl (
    CSL_McaspHandle           hMcasp,
    CSL_McaspHwControlCmd     cmd,
    void                      *arg
);

/**
 * ============================================================================
 *   @func CSL_mcaspGetHwStatus
 *
 *   @desc
 *   @n     This function is used to get the value of various parameters of an 
 *          instance of McASP. The value returned depends on the query passed.
 *
 *   @arg hMcasp
 *        Handle to the McASP
 *
 *   @arg myQuery
 *        Query to be performed. 
 *
 *   @arg response
 *        Pointer to buffer to return the data requested by the query passed
 *
 *   @ret CSL_Status
 *        CSL_SOK            - Successful completion of the query
 *        CSL_ESYS_BADHANDLE - Invalid handle
 *        CSL_ESYS_INVQUERY  - Query command not supported
 *        CSL_ESYS_FAIL      - Generic failure
 *
 *   @eg
 *        CSL_Status        status;
 *        Bool              xmtUnderRun;
 *        CSL_McaspHandle   hMcasp;
 *
 *        status = CSL_mcaspGetHwStatus (hMcasp,  
 *                        CSL_MCASP_QUERY_XSTAT_XUNDRN,  
 *                        &xmtUnderRun);
 *
 * ============================================================================
 */
extern CSL_Status CSL_mcaspGetHwStatus (
    CSL_McaspHandle           hMcasp,
    CSL_McaspHwStatusQuery    myQuery,
    void                      *response
);

/**
 * ============================================================================
 *   @func CSL_mcaspRead
 *
 *   @desc
 *   @n     This reads the data from McASP. 32 bits of data will be read
 *          in the data object (variable); the pointer to which is passed
 *          as the third argument.
 *
 *   @arg hMcasp
 *        Handle to the McASP
 *
 *   @arg serNum
 *        Serializer Number
 *
 *   @arg data
 *        Buffer to store read data.
 *
 *   @ret CSL_Status
 *        CSL_SOK               - Successful completion of read
 *        CSL_ESYS_BADHANDLE    - Invalid handle
 *        CSL_ESYS_INVPARAMS    - Invalid serializer
 *
 *   @eg
 *        CSL_Status        status;
 *        Uint32            inData;
 *        CSL_McaspHandle   hMcasp;
 *
 *        status = CSL_mcaspRead (hMcasp, &inData);
 *
 * ============================================================================
 */
extern CSL_Status CSL_mcaspRead (
    CSL_McaspHandle         hMcasp,
    Uint32                  *data
);

/**
 * ============================================================================
 *   @func CSL_mcaspWrite
 *
 *   @desc
 *   @n     This transmits the data from McASP. 32 bits of data will be
 *          transmitted in the data object (variable); the pointer to which
 *          is passed as the third argument.
 *
 *   @arg hMcasp
 *        Handle to the McASP
 *
 *   @arg serNum
 *        Serializer Number
 *
 *   @arg data
 *        Data to be written.
 *
 *   @ret CSL_Status
 *        CSL_SOK              - Successful completion of the query
 *        CSL_ESYS_BADHANDLE   - Invalid handle
 *        CSL_ESYS_INVPARAMS   - Invalid parameters
 *
 *   @eg
 *        CSL_Status        status;
 *        Uint32            outData;
 *        CSL_McaspHandle   hMcasp;
 *        
 *        outData = 0x1234;
 *        status = CSL_mcaspWrite (hMcasp, &outData);
 *
 * ============================================================================
 */
extern CSL_Status CSL_mcaspWrite (
    CSL_McaspHandle         hMcasp,
    Uint32                  *data
);

/**
 * ============================================================================
 *   @func CSL_mcaspRegReset
 *
 *   @desc
 *   @n     Function to reset all the registers values.
 *
 *   @arg hMcasp
 *          Handle to the McASP
 *
 *   @ret void
 *          None
 * 
 *   @eg
 *          CSL_McaspHandle     hMcasp;
 *
 *          CSL_mcaspRegReset (hMcasp);
 *
 * ============================================================================
 */
extern void CSL_mcaspRegReset (
    CSL_McaspHandle     hMcasp
);

/**
 * ============================================================================
 *   @func CSL_mcaspResetCtrl
 *
 *   @desc
 *   @n     This function enables bit fields of GBLCTL register.
 *
 *   @arg hMcasp
 *          Handle to the McASP
 *
 *   @arg selectMask
 *          Selects bits to enable
 *
 *   @ret void
 *          None
 * 
 *   @eg
 *          CSL_McaspHandle     hMcasp;
 *
 *          CSL_mcaspResetCtrl (hMcasp, CSL_MCASP_GBLCTL_XSMRST_MASK
 *                              | CSL_MCASP_GBLCTL_RSMRST_MASK);
 *
 * ============================================================================
 */
extern void CSL_mcaspResetCtrl (
    CSL_McaspHandle     hMcasp,
    CSL_BitMask32       selectMask
);


/** 
 * ============================================================================
 *   @func CSL_mcaspGetChipCtxt
 *
 *   @b Description
 *   @n  This function is used for getting the base address of the peripheral
 *       instance and to configure the number of serializers for a particular
 *       instance of McASP on a chip. This function will be called inside the
 *       CSL_mcaspOpen() function.This function is open for re-implementing 
 *       if the user wants to modify the base address of the peripheral object
 *       to point to a different location and there by allow CSL initiated
 *       write/reads into peripheral MMR's go to an alternate location.
 *
 *   @b Arguments
 *   @verbatim      
            mcaspNum        Specifies the instance of the McASP to be opened.
 
            pMcaspParam     Module specific parameters.
 
            pChipContext    Pointer to chip context structure containing base 
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
        CSL_Status            status;
        CSL_McaspChipContext  chipContext;

       ...
      status = CSL_mcaspGetChipCtxt(CSL_MCASP_1, NULL, &chipContext);

    @endverbatim
 * ============================================================================
 */
extern CSL_Status   CSL_mcaspGetChipCtxt (
    CSL_InstNum             mcaspNum,
    CSL_McaspParam          *pMcaspParam,
    CSL_McaspChipContext    *pChipContext 
);

#ifdef __cplusplus
}
#endif

#endif  /* _CSL_MCASP_H_ */

