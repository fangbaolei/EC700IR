/*  ============================================================================
 *   Copyright (c) Texas Instruments Inc 2002, 2003, 2004, 2005
 *
 *   Use of this software is controlled by the terms and conditions found in the
 *   license agreement under which this software has been supplied.
 *   ===========================================================================
 */
 
/** ============================================================================
 *   @file  csl_mcbsp.h
 *
 *   @path  $(CSLPATH)\inc
 *
 *   @desc  Header file for functional layer of McBSP CSL
 *
 */

/** @mainpage MCBSP CSL 3.x
 *
 * @section Introduction
 *
 * @subsection xxx Purpose and Scope
 * The purpose of this document is to identify a set of common CSL APIs for
 * the MCBSP module across various devices. The CSL developer is expected to
 * refer to this document while designing APIs for these modules. Some of the
 * listed APIs may not be applicable to a given MCBSP module. While other cases
 * this list of APIs may not be sufficient to cover all the features of a
 * particular MCBSP Module. The CSL developer should use his discretion 
 * designing new APIs or extending the existing ones to cover these.
 *
 * @subsection aaa Terms and Abbreviations
 *   -# CSL:  Chip Support Library
 *   -# API:  Application Programmer Interface
 *
 * @subsection References
 *    -# CSL-001-DES, CSL 3.x Design Specification DocumentVersion 1.02
 *
 */

/* =============================================================================
 * Revision History
 * ================
 *  June 29,2004 Pratheesh Gangadhar - Created
 * 
 *  May 17,2005  Ramitha Mathew  - Removed support for 512 channel mode              
 *                               - Removed support for Super synchronization         
 *                               - IntEvent removed from CSL_McbspHwSetup and  
 *                                 added commands and queries for interrupt mode          
 *                               - CSL_MCBSP_CMD_REG_RESET changed to
 *                                 CSL_MCBSP_CMD_RESET 
 *
 *  July 04, 2005 ds             - Removed support for DX Mode.Hence removed
 *                                 dxState from CSL_McbspHwSetup
 *                               - Removed support for enhanced sample clock
 *                                 mode. 
 *                               - Register Naming convention used for 
 *                                 Multichannel registers are changed according 
 *                                 the changes in cslr_mcbsp.h
 *
 *  July 26, 2005 ds            - Removed control cmd 
 *                                CSL_MCBSP_CMD_IO_MODE_CONTROL. 
 *                                  
 *  Sept 21, 2005 ds            - Removed the DXR and DRR register from the 
 *                                config data structure
 *
 *  Oct  27, 2005 ds            - Removed CSL_MCBSP_QUERY_PID query from 
 *                                CSL_McbspHwStatusQuery
 *
 *  Feb 02 , 2006 ds            - IntEvent added to CSL_McbspHwSetup and 
 *                                removed interrupt mode commands and queries 
 * =============================================================================
 */

#ifndef _CSL_MCBSP_H_
#define _CSL_MCBSP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <soc.h>
#include <csl.h>
#include <cslr_mcbsp.h>

/**************************************************************************\
* MCBSP global macro declarations
\**************************************************************************/

/** 
 * Use this symbol as pin mask for @a CSL_mcbspIoRead() and
 * CSL_mcbspIoWrite() functions
 */ 
 
/** I/O Pin Input/Output configuration for CLKX Pin      */
#define CSL_MCBSP_IO_CLKX                             (1)

/** I/O Pin Input/Output configuration for FSX  Pin       */
#define CSL_MCBSP_IO_FSX                              (2)

/** Not Configurable. Always Output.                      */
#define CSL_MCBSP_IO_DX                               (4)

/** I/O Pin Input/Output configuration for CLKR Pin       */
#define CSL_MCBSP_IO_CLKR                             (8)

/** I/O Pin Input/Output configuration for FSR Pin        */
#define CSL_MCBSP_IO_FSR                              (16)

/** Not Configurable. Always Input.                        */
#define CSL_MCBSP_IO_DR                               (32)

/** Not Configurable. Always Input.                        */
#define CSL_MCBSP_IO_CLKS                             (64)


/** 
 * Use this symbol as enable/disable control bitmask for
 * CSL_mcbspHwControl() function
 */
 
/** To enable Receiver in resetControl Function             */
#define CSL_MCBSP_CTRL_RX_ENABLE                      (1)

/** To enable Transmitter in resetControl Function          */
#define CSL_MCBSP_CTRL_TX_ENABLE                      (2)

/** To disable Receiver in resetControl Function            */
#define CSL_MCBSP_CTRL_RX_DISABLE                     (4)

/** To disable Transmitter in resetControl Function         */
#define CSL_MCBSP_CTRL_TX_DISABLE                     (8)

/** To enable Frame Sync Generation in resetControl Function */
#define CSL_MCBSP_CTRL_FSYNC_ENABLE                   (16)

/** To enable Sample Rate Generator in resetControl Function */
#define CSL_MCBSP_CTRL_SRG_ENABLE                     (32)

/** To disable Frame Sync Generation in resetControl Function */
#define CSL_MCBSP_CTRL_FSYNC_DISABLE                  (64)

/** To disable Sample Rate Generator in resetControl Function */
#define CSL_MCBSP_CTRL_SRG_DISABLE                    (128)

/** 
 * Use this symbol to compare the return value of @a CSL_mcbspGetHwStatus()
 * function for @a CSL_MCBSP_QUERY_DEV_STATUS query
 * \n For eg:- On RFULL event, (response & CSL_MCBSP_RFULL) == 0x0004
 */
 
/** RCV ready status */
#define CSL_MCBSP_RRDY             0x0001
/** XMT ready status */
#define CSL_MCBSP_XRDY             0x0002
/** RCV full status */
#define CSL_MCBSP_RFULL            0x0004
/** XMT empty status */
#define CSL_MCBSP_XEMPTY           0x0008
/** RCV frame sync error status */
#define CSL_MCBSP_RSYNCERR         0x0010
/** XMT frame sync error status */
#define CSL_MCBSP_XSYNCERR         0x0020

/** CSL_EMCBSP_SYMBOL Error codes */
 
/** Invalid Control Command */
#define CSL_EMCBSP_INVCNTLCMD             (CSL_EMCBSP_FIRST - 0)

/** Invalid Query */
#define CSL_EMCBSP_INVQUERY               (CSL_EMCBSP_FIRST - 1)

/** Invalid Parameter */
#define CSL_EMCBSP_INVPARAMS              (CSL_EMCBSP_FIRST - 2)

/** Invalid Size */
#define CSL_EMCBSP_INVSIZE                (CSL_EMCBSP_FIRST - 3)

/** 'Does not exist' */
#define CSL_EMCBSP_NOTEXIST               (CSL_EMCBSP_FIRST - 4)

/** Invalid mode to conduct operation */
#define CSL_EMCBSP_INVMODE                (CSL_EMCBSP_FIRST - 5)

/** CSL_MCBSP_DEFAULT_SYMBOL MCBSP CSL Defaults  */

/** Data Setup defaults */
#define CSL_MCBSP_DATASETUP_DEFAULTS   {            \
   (CSL_McbspPhase)CSL_MCBSP_PHASE_SINGLE,          \
   (CSL_McbspWordLen)CSL_MCBSP_WORDLEN_16,          \
   1,                                               \
   (CSL_McbspWordLen)0,                             \
   0,                                               \
   (CSL_McbspFrmSync)CSL_MCBSP_FRMSYNC_DETECT,      \
   (CSL_McbspCompand)CSL_MCBSP_COMPAND_OFF_MSB_FIRST, \
   (CSL_McbspDataDelay)CSL_MCBSP_DATADELAY_0_BIT,   \
   (CSL_McbspRjustDxena)0,                          \
   (CSL_McbspIntMode)CSL_MCBSP_INTMODE_ON_READY,    \
   (CSL_McbspBitReversal)CSL_MCBSP_32BIT_REVERS_DISABLE }\


/** Clock Setup defaults */
#define CSL_MCBSP_CLOCKSETUP_DEFAULTS  {                \
 (CSL_McbspFsClkMode)CSL_MCBSP_FSCLKMODE_EXTERNAL,      \
 (CSL_McbspFsClkMode)CSL_MCBSP_FSCLKMODE_EXTERNAL,      \
 (CSL_McbspTxRxClkMode)CSL_MCBSP_TXRXCLKMODE_INTERNAL,  \
 (CSL_McbspTxRxClkMode)CSL_MCBSP_TXRXCLKMODE_EXTERNAL,  \
 (CSL_McbspFsPol)0,                                     \
 (CSL_McbspFsPol)0,                                     \
 (CSL_McbspClkPol)0,                                    \
 (CSL_McbspClkPol)0,                                    \
 1,                                                     \
 0x40,                                                  \
 0xFF,                                                  \
 (CSL_McbspSrgClk)0,                                    \
 (CSL_McbspClkPol)0,                                    \
 (CSL_McbspTxFsMode)CSL_MCBSP_TXFSMODE_SRG,             \
 (CSL_McbspClkgSyncMode)CSL_MCBSP_CLKGSYNCMODE_OFF     }\


/** Multichannel Setup defaults */
#define CSL_MCBSP_MULTICHAN_DEFAULTS  {                 \
 (CSL_McbspPartMode)CSL_MCBSP_PARTMODE_2PARTITION,      \
 (CSL_McbspPartMode)CSL_MCBSP_PARTMODE_2PARTITION,      \
 (Uint16)0,                                             \
 (Uint16)0,                                             \
 (CSL_McbspPABlk)CSL_MCBSP_PABLK_0,                     \
 (CSL_McbspPBBlk)CSL_MCBSP_PBBLK_1,                     \
 (CSL_McbspPABlk)CSL_MCBSP_PABLK_0,                     \
 (CSL_McbspPBBlk)CSL_MCBSP_PBBLK_1,                     \
 }\


/** Global parameters Setup defaults */
#define CSL_MCBSP_GLOBALSETUP_DEFAULTS  {               \
 (CSL_McbspIOMode)CSL_MCBSP_IOMODE_TXDIS_RXDIS,         \
 (CSL_McbspDlbMode)CSL_MCBSP_DLBMODE_OFF,               \
 (CSL_McbspClkStp)CSL_MCBSP_CLKSTP_DISABLE }             \


/** Default Emulation mode - Stop */
#define CSL_MCBSP_EMUMODE_DEFAULT  CSL_MCBSP_EMU_STOP

/** Extend Setup default - NULL */
#define CSL_MCBSP_EXTENDSETUP_DEFAULT  NULL


/**************************************************************************\
* MCBSP global typedef declarations
\**************************************************************************/

/**
 * Word lengths supported on MCBSP.Use this symbol for setting Word 
 * Length in each Phase for every Frame
 */
 
typedef enum {
    /** Word Length for Frame is 8 */
    CSL_MCBSP_WORDLEN_8        =                  0,
    /** Word Length for Frame is 12 */
    CSL_MCBSP_WORDLEN_12       =                  1,
    /** Word Length for Frame is 16 */
    CSL_MCBSP_WORDLEN_16       =                  2,
    /** Word Length for Frame is 20 */
    CSL_MCBSP_WORDLEN_20       =                  3,
    /** Word Length for Frame is 24 */
    CSL_MCBSP_WORDLEN_24       =                  4,
    /** Word Length for Frame is 32 */
    CSL_MCBSP_WORDLEN_32       =                  5
} CSL_McbspWordLen;

/**
 * MCBSP companding options - Use this symbol to set Companding related options
 */
typedef enum {
    /** no companding for msb */
    CSL_MCBSP_COMPAND_OFF_MSB_FIRST =                 0,
    /** no companding for lsb */
    CSL_MCBSP_COMPAND_OFF_LSB_FIRST =                 1,
    /** mu-law comapanding enable for channel */
    CSL_MCBSP_COMPAND_MULAW         =                 2,
    /** A-law comapanding enable for channel */
    CSL_MCBSP_COMPAND_ALAW          =                 3
} CSL_McbspCompand;

/**
 * Data delay in bits - Use this symbol to set XMT/RCV Data Delay (in bits)
 */
typedef enum {
    /** sets XMT/RCV Data Delay is 0 */
    CSL_MCBSP_DATADELAY_0_BIT      =                 0,
    /** sets XMT/RCV Data Delay is 1 */
    CSL_MCBSP_DATADELAY_1_BIT      =                 1,
    /** sets XMT/RCV Data Delay is 2 */
    CSL_MCBSP_DATADELAY_2_BITS     =                 2
} CSL_McbspDataDelay;

/**
 * MCBSP Interrupt mode -
 * Use this symbol to set Interrupt mode (i.e. source of interrupt generation).
 * This symbol is used on both RCV and XMT for RINT and XINT generation mode.
 */ 
typedef enum {
    /** Interrupt generated on RRDY of RCV or XRDY of XMT */
    CSL_MCBSP_INTMODE_ON_READY         =                  0,
    /** Interrupt generated on end of 16-channel block transfer
     *  in multichannel mode */
    CSL_MCBSP_INTMODE_ON_EOB           =                  1,
    /** Interrupt generated on frame sync */
    CSL_MCBSP_INTMODE_ON_FSYNC         =                  2,
    /** Interrupt generated on synchronisation error */
    CSL_MCBSP_INTMODE_ON_SYNCERR       =                  3
} CSL_McbspIntMode;

/**
 * Frame sync clock source - Use this symbol to set the frame sync clock 
 * source as internal or external
 */
typedef enum {
    /** frame sync clock source as external */
    CSL_MCBSP_FSCLKMODE_EXTERNAL    =                  0,
    /** frame sync clock source as internal */
    CSL_MCBSP_FSCLKMODE_INTERNAL    =                  1
} CSL_McbspFsClkMode;

/**
 * Clock source - Use this symbol to set the clock source as 
 * internal or external
 */
typedef enum {
    /** clock source as external */
    CSL_MCBSP_TXRXCLKMODE_EXTERNAL    =                  0,
    /** clock source as internal */
    CSL_MCBSP_TXRXCLKMODE_INTERNAL    =                  1
} CSL_McbspTxRxClkMode;

/**
 * Frame sync polarity - Use this symbol to set frame sync polarity as 
 * active-high or active-low
 */
typedef enum {
    /** frame sync polarity is active-high */
    CSL_MCBSP_FSPOL_ACTIVE_HIGH      =                  0,
    /** frame sync polarity is active-low */
    CSL_MCBSP_FSPOL_ACTIVE_LOW       =                  1
} CSL_McbspFsPol;

/**
 * Clock polarity - Use this symbol to set XMT or RCV clock polarity as
 * rising or falling edge
 */
typedef enum {
    /** XMT clock polarity is rising edge */
    CSL_MCBSP_CLKPOL_TX_RISING_EDGE      =                 0,
    /** RCV clock polarity is falling edge */
    CSL_MCBSP_CLKPOL_RX_FALLING_EDGE     =                 0,
    /** SRG clock polarity is rising edge */
    CSL_MCBSP_CLKPOL_SRG_RISING_EDGE     =                 0,
    /** XMT clock polarity is falling edge */
    CSL_MCBSP_CLKPOL_TX_FALLING_EDGE     =                 1,
    /** RCV clock polarity is rising edge */
    CSL_MCBSP_CLKPOL_RX_RISING_EDGE      =                 1,
    /** SRG clock polarity Is falling edge */
    CSL_MCBSP_CLKPOL_SRG_FALLING_EDGE    =                 1
} CSL_McbspClkPol;

/**
 * SRG clock source - Use this symbol to select input clock source 
 * for Sample Rate Generator
 */
typedef enum {
    /** input clock source for Sample Rate Generator is CLKS pin */
    CSL_MCBSP_SRGCLK_CLKS                 =                  0,
    /** input clock source for Sample Rate Generator is CPU */
    CSL_MCBSP_SRGCLK_CLKCPU               =                  1
} CSL_McbspSrgClk;

/**
 * XMT Frame Sync generation mode -  Use this symbol to set XMT Frame Sync
 * generation mode
 */
typedef enum {
    /** Disables the frame sync generation mode */
    CSL_MCBSP_TXFSMODE_DXRCOPY  =                  0,
    /** Enables the frame sync generation mode */
    CSL_MCBSP_TXFSMODE_SRG      =                  1
} CSL_McbspTxFsMode;

/** 
 * XMT and RCV IO Mode - Use this symbol to Enable/Disable IO Mode
 * for XMT and RCV
 */
typedef enum {
    /** Disable the both XMT and RCV IO mode */
    CSL_MCBSP_IOMODE_TXDIS_RXDIS       =                  0,
    /** Disable XMT and enable RCV IO mode */
    CSL_MCBSP_IOMODE_TXDIS_RXEN        =                  1,
    /** Enable XMT and Disble RCV IO mode */
    CSL_MCBSP_IOMODE_TXEN_RXDIS        =                  2,
    /** Enable XMT and enable RCV IO mode */
    CSL_MCBSP_IOMODE_TXEN_RXEN         =                  3
} CSL_McbspIOMode;
  
/** 
 * Clock Stop Mode - Use this symbol to Enable/Disable Clock Stop Mode
 */
typedef enum {
    /** Disable the clock stop mode */
    CSL_MCBSP_CLKSTP_DISABLE       =                  0,
    /** Emable the clock stop mode  with out delay */
    CSL_MCBSP_CLKSTP_WITHOUT_DELAY =                  2,
    /** Emable the clock stop mode  with delay */
    CSL_MCBSP_CLKSTP_WITH_DELAY    =                  3
} CSL_McbspClkStp;

/**
 * Multichannel mode Partition type - Use this symbol to select the partition
 * type in multichannel mode
 */
typedef enum {
    /** two partition mode */
    CSL_MCBSP_PARTMODE_2PARTITION           =                  0,
    /** Eight partition multichannel mode */
    CSL_MCBSP_PARTMODE_8PARTITION           =                  1
} CSL_McbspPartMode;

/**
 * Multichannel mode PartitionA block - Use this symbol to assign Blocks to 
 * Partition-A in multichannel mode
 */
typedef enum {
    /** Block 0 for partition A */
    CSL_MCBSP_PABLK_0              =                  0,
    /** Block 2 for partition A */
    CSL_MCBSP_PABLK_2              =                  1,
    /** Block 4 for partition A */
    CSL_MCBSP_PABLK_4              =                  2,
    /** Block 6 for partition A */
    CSL_MCBSP_PABLK_6              =                  3
} CSL_McbspPABlk;

/**
 * Multichannel mode PartitionB block - Use this symbol to assign Blocks to 
 * Partition-B in multichannel mode
 */
typedef enum {
    /** Block 1 for partition B */
    CSL_MCBSP_PBBLK_1              =                  0,
    /** Block 3 for partition B */
    CSL_MCBSP_PBBLK_3              =                  1,
    /** Block 5 for partition B */
    CSL_MCBSP_PBBLK_5              =                  2,
    /** Block 7 for partition B */
    CSL_MCBSP_PBBLK_7              =                  3
} CSL_McbspPBBlk;

/**
 * Emulation mode setting - Use this symbol to set the Emulation Mode
 */
typedef enum {
    /** Emulation mode stop */
    CSL_MCBSP_EMU_STOP             =                  0,
    /** Emulation mode TX stop */
    CSL_MCBSP_EMU_TX_STOP          =                  1,
    /** Emulation free run mode */
    CSL_MCBSP_EMU_FREERUN          =                  2
} CSL_McbspEmu;

/**
 * Multichannel mode Partition select - Use this symbol in multichannel mode to 
 * select the Partition for assigning a block to
 */
typedef enum {
    /** TX partition for A */
    CSL_MCBSP_PARTITION_ATX        =                  0,
    /** RX partition for A */
    CSL_MCBSP_PARTITION_ARX        =                  1,
    /** TX partition for B */
    CSL_MCBSP_PARTITION_BTX        =                  2,
    /** RX partition for B */
    CSL_MCBSP_PARTITION_BRX        =                  3
} CSL_McbspPartition;

/**
 * Multichannel mode Block select - Use this symbol in multichannel mode to
 * select block on which the operation is to be performed
 */
typedef enum {
    /** Block 0 for multichannel mode */
    CSL_MCBSP_BLOCK_0               =                  0,
    /** Block 1 for multichannel mode */
    CSL_MCBSP_BLOCK_1               =                  1,
    /** Block 2 for multichannel mode */
    CSL_MCBSP_BLOCK_2               =                  2,
    /** Block 3 for multichannel mode */
    CSL_MCBSP_BLOCK_3               =                  3,
    /** Block 4 for multichannel mode */
    CSL_MCBSP_BLOCK_4               =                  4,
    /** Block 5 for multichannel mode */
    CSL_MCBSP_BLOCK_5               =                  5,
    /** Block 6 for multichannel mode */
    CSL_MCBSP_BLOCK_6               =                  6,
    /** Block 7 for multichannel mode */
    CSL_MCBSP_BLOCK_7               =                  7
} CSL_McbspBlock;

/**
 * Channel control in multichannel mode
 * Use this symbol to enable/disable a channel in multichannel mode.
 * This is a member of CSL_McbspChanControl structure, which is input to
 * CSL_mcbspHwControl() function for CSL_MCBSP_CMD_CHANNEL_CONTROL command
 */
typedef enum {
    /** TX enable for multichannel mode */
    CSL_MCBSP_CHCTRL_TX_ENABLE       =                  0,
    /** TX disable for multichannel mode */
    CSL_MCBSP_CHCTRL_TX_DISABLE      =                  1,
    /** RX enable for multichannel mode */
    CSL_MCBSP_CHCTRL_RX_ENABLE       =                  2,
    /** RX disable for multichannel mode */
    CSL_MCBSP_CHCTRL_RX_DISABLE      =                  3
} CSL_McbspChCtrl;

/**
 * Channel type: TX, RX or both - Use this symbol to select the channel type for 
 * CSL_mcbspHwControl()
 */
typedef enum {
    /** Channel type is RX */
    CSL_MCBSP_CHTYPE_RX                   =             1,
    /** Channel type is TX */
    CSL_MCBSP_CHTYPE_TX                   =             2,
    /** Channel type is TXRX */
    CSL_MCBSP_CHTYPE_TXRX                 =             4
} CSL_McbspChType;

/**
 * Digital Loopback mode selection - Use this symbol to enable/disable digital
 * loopback mode
 */
typedef enum {
    /** disable digital loopback mode */
    CSL_MCBSP_DLBMODE_OFF                  =            0,
    /** enable digital loopback mode */
    CSL_MCBSP_DLBMODE_ON                   =            1
} CSL_McbspDlbMode;

/**
 * Phase count selection - Use this symbol to select number of phases per frame
 */
typedef enum {
    /** Single phase for frame */
    CSL_MCBSP_PHASE_SINGLE                =             0,
    /** Dual phase for frame */
    CSL_MCBSP_PHASE_DUAL                  =             1
} CSL_McbspPhase;

/**
 * Frame sync ignore status - Use this symbol to detect or ignore 
 * frame synchronisation
 */
typedef enum {
    /** detect frame synchronisation */
    CSL_MCBSP_FRMSYNC_DETECT                  =         0,
    /** ignore frame synchronisation */
    CSL_MCBSP_FRMSYNC_IGNORE                  =         1
} CSL_McbspFrmSync;

/**
 * RJUST or DXENA settings - Use this symbol for setting up RCV sign-extension
 * and justification mode or enabling/disabling XMT DX pin delay
 */
typedef enum {
    /** RCV setting - right justify, fill MSBs with zeros */
    CSL_MCBSP_RJUSTDXENA_RJUST_RZF                 =    0,
    /** XMT setting - Delay at DX pin disabled */
    CSL_MCBSP_RJUSTDXENA_DXENA_OFF                 =    0,
    /** RCV setting - right justify, sign-extend the data into MSBs */
    CSL_MCBSP_RJUSTDXENA_RJUST_RSE                 =    1,
    /** XMT setting - Delay at DX pin enabled */
    CSL_MCBSP_RJUSTDXENA_DXENA_ON                  =    1,
    /** RCV setting - left justify, fill LSBs with zeros */
    CSL_MCBSP_RJUSTDXENA_RJUST_LZF                 =    2
} CSL_McbspRjustDxena;

/**
 * CLKG sync mode selection - Use this symbol to enable/disable CLKG 
 * synchronisation when input CLK source for SRGR is external
 */
typedef enum {
    /** disable CLKG synchronisation */
    CSL_MCBSP_CLKGSYNCMODE_OFF                 =        0,
    /** enable CLKG synchronisation */
    CSL_MCBSP_CLKGSYNCMODE_ON                  =        1
} CSL_McbspClkgSyncMode;

/**
 * Tx/Rx reset status - Use this symbol to compare the output of 
 * CSL_mcbspGetHwStatus() for CSL_MCBSP_QUERY_TX_RST_STAT and 
 * CSL_MCBSP_QUERY_RX_RST_STAT queries
 */
typedef enum {
    /** disable the XRST bit */
    CSL_MCBSP_RSTSTAT_TX_IN_RESET             =         0,
    /** disable the RRST bit */
    CSL_MCBSP_RSTSTAT_RX_IN_RESET             =         0,
    /** enable the XRST bit */
    CSL_MCBSP_RSTSTAT_TX_OUTOF_RESET          =         1,
    /** enable the RRST bit */
    CSL_MCBSP_RSTSTAT_RX_OUTOF_RESET          =         1
} CSL_McbspRstStat;

/** McBSP 32-bit reversal feature */
typedef enum {
    /** 32-bit reversal disabled */
    CSL_MCBSP_32BIT_REVERS_DISABLE  = 0,
    /** 32-bit reversal enabled. 32-bit data is received LSB first. Word length
     *  should be set for 32-bit operation; else operation undefined
     */
    CSL_MCBSP_32BIT_REVERS_ENABLE    = 1 
} CSL_McbspBitReversal;

/**
 * This is the set of control commands that are passed to
 * CSL_mcbspHwControl(), with an optional argument type-casted to void*
 * The arguments, if any, to be passed with each command are specified
 * next to that command.
 */
typedef enum {
        /**
         * @brief   Assigns a block to a particular partition in multichannel 
         *          mode
         * @param   (CSL_McbspBlkAssign *)
         */
    CSL_MCBSP_CMD_ASSIGN_BLOCK     =                  0,
        /**
         * @brief   Enables or disables a channel in multichannel mode
         * @param   (CSL_McbspChanControl *)
         */
    CSL_MCBSP_CMD_CHANNEL_CONTROL  =                  1,
        /**
         * @brief   Clears frame sync error for XMT or RCV
         * @param   (CSL_McbspChType *)
         */
    CSL_MCBSP_CMD_CLEAR_FRAME_SYNC =                  2,
        /**
         * @brief   Resets all the registers to their power-on default values
         * @param   None
         */
    CSL_MCBSP_CMD_RESET        =                      3,
        /**
         * @brief   Enable/Disable - Frame Sync, Sample Rate Generator and 
         *          XMT/RCV Operation
         * @param   (CSL_BitMask16 *)
         */
    CSL_MCBSP_CMD_RESET_CONTROL    =                  4
} CSL_McbspControlCmd;

/**
 * This is the set of query commands to get the status of various
 * operations in MCBSP
 * The arguments, if any, to be passed with each command are specified
 * next to that command
 */
typedef enum {
        /**
         * @brief   Queries the current XMT block
         * @param   (CSL_McbspBlock *)
         */
        CSL_MCBSP_QUERY_CUR_TX_BLK =                 1,
        /**
         * @brief   Queries the current RCV block
         * @param   (CSL_McbspBlock *)
         */
    CSL_MCBSP_QUERY_CUR_RX_BLK =                     2,
        /**
         * @brief   Queries the status of RRDY, XRDY, RFULL, XEMPTY, RSYNCERR
         *          and XSYNCERR events and returns them in supplied 
         *          CSL_BitMask16 argument
         * @param  (CSL_BitMask16 *)
         */
    CSL_MCBSP_QUERY_DEV_STATUS =                     3,
        /**
         * @brief   Queries XMT reset status
         * @param   (CSL_McbspRstStat *)
         * @return CSL_SOK
         */
    CSL_MCBSP_QUERY_TX_RST_STAT =                    4,
        /**
         * @brief   Queries RCV reset status
         * @param  (CSL_McbspRstStat *)
         */
    CSL_MCBSP_QUERY_RX_RST_STAT =                    5
} CSL_McbspHwStatusQuery;

/**
 * This will have the base-address information for the peripheral instance
 */
typedef struct {
    /** Base-address of the Configuration registers of MCBSP  */
    CSL_McbspRegsOvly regs;
} CSL_McbspBaseAddress;

/**
 *  MCBSP specific parameters. Present implementation doesn't have
 *  any specific parameters.
 */
typedef struct {
    /** Bit mask to be used for module specific parameters.
     *  The below declaration is just a place-holder for future
     *  implementation.
     */
    CSL_BitMask16   flags;
} CSL_McbspParam;

/**
 *  MCBSP specific context information. Present implementation doesn't
 *  have any Context information.
 */

typedef struct {
    /** Context information of MCBSP.
     *  The below declaration is just a place-holder for future
     *  implementation.
     */
    Uint16  contextInfo;
} CSL_McbspContext;

/**
 * Pointer to this structure is used as the third argument in
 * CSL_mcbspHwControl() for block assignment in multichannel mode
 */
typedef struct CSL_McbspBlkAssign {
    /** Partition to choose */
    CSL_McbspPartition      partition;
    /** Block to choose */
    CSL_McbspBlock          block;
} CSL_McbspBlkAssign;

/**
 * Pointer to this structure is used as the third argument in
 * CSL_mcbspHwControl() for channel control operations (Enable/Disable TX/RX)
 * in multichannel mode.
 */
typedef struct CSL_McbspChanControl {
    /** Channel number to control */
    Uint16              channelNo;
    /** Control operation */
    CSL_McbspChCtrl     operation;
} CSL_McbspChanControl;

/**
 * This is a sub-structure in @ CSL_McbspHwSetup. This structure is used for
 * configuring input/output data related parameters.
 */
typedef struct CSL_McbspDataSetup {
    /** Number of phases in a frame */
    CSL_McbspPhase          numPhases;
   
    /** Number of bits per word in phase 1 */
    CSL_McbspWordLen        wordLength1;

    /** Number of words per frame in phase 1 */
    Uint16                  frmLength1;

    /** Number of bits per word in phase 2 */
    CSL_McbspWordLen        wordLength2;

    /** Number of words per frame in phase 2 */
    Uint16                  frmLength2;

    /** Frame Sync ignore */
    CSL_McbspFrmSync        frmSyncIgn;

    /** Companding options */
    CSL_McbspCompand        compand;

    /** Data delay in number of bits */
    CSL_McbspDataDelay      dataDelay;

    /** Controls DX delay for XMT or sign-extension and justification for RCV */
    CSL_McbspRjustDxena     rjust_dxenable;
    
    /** Interrupt event mask */
    CSL_McbspIntMode       intEvent;
    
    /** 32-bit reversal feature */
    CSL_McbspBitReversal    wordReverse;
} CSL_McbspDataSetup;

/**
 * This is a sub-structure in @a CSL_McbspHwSetup. This structure is used for
 * configuring Clock and Frame Sync generation parameters.
 */
typedef struct CSL_McbspClkSetup {
    /** XMT frame sync mode */
    CSL_McbspFsClkMode        frmSyncTxMode;

    /** RCV frame sync mode */
    CSL_McbspFsClkMode        frmSyncRxMode;

    /** XMT clock mode */
    CSL_McbspTxRxClkMode      clkTxMode;

    /** RCV clock mode */
    CSL_McbspTxRxClkMode      clkRxMode;

    /** XMT frame sync polarity */
    CSL_McbspFsPol            frmSyncTxPolarity;

    /** RCV frame sync polarty */
    CSL_McbspFsPol            frmSyncRxPolarity;

    /** XMT clock polarity */
    CSL_McbspClkPol           clkTxPolarity;

    /** RCV clock polarity */
    CSL_McbspClkPol           clkRxPolarity;

    /** SRG frame sync pulse width */
    Uint16                    srgFrmPulseWidth;

    /** SRG frame sync period */
    Uint16                    srgFrmPeriod;

    /** SRG divide-down ratio */
    Uint16                    srgClkDivide;

    /** SRG input clock mode */
    CSL_McbspSrgClk           srgInputClkMode;

    /** SRG clock polarity */
    CSL_McbspClkPol           srgClkPolarity;

    /** SRG XMT frame-synchronisatoin mode */
    CSL_McbspTxFsMode         srgTxFrmSyncMode;

    /** SRG clock synchronisation mode */
    CSL_McbspClkgSyncMode     srgClkSync;
} CSL_McbspClkSetup;

/**
 * This is a sub-structure in @a CSL_McbspHwSetup. This structure is used 
 * for configuring the parameters global to MCBSP
 */
typedef struct CSL_McbspGlobalSetup {
    /** XMT and RCV IO enable bit */
    CSL_McbspIOMode       ioEnableMode;

    /** Digital Loopback mode */
    CSL_McbspDlbMode      dlbMode;

    /** Clock stop mode */
    CSL_McbspClkStp       clkStopMode;
} CSL_McbspGlobalSetup;

/**
 * This is a sub-structure in @a CSL_McbspHwSetup. This structure is used 
 * for configuring Multichannel mode parameters
 */
typedef struct CSL_McbspMulChSetup {
    /** RCV partition */
    CSL_McbspPartMode  rxPartition;

    /** XMT partition */
    CSL_McbspPartMode  txPartition;

    /** RCV multichannel selection mode */
    Uint16             rxMulChSel;

    /** XMT multichannel selection mode */
    Uint16             txMulChSel;

    /** RCV partition A block */
    CSL_McbspPABlk     rxPartABlk;

    /** RCV partition B block */
    CSL_McbspPBBlk     rxPartBBlk;

    /** XMT partition A block */
    CSL_McbspPABlk     txPartABlk;

    /** XMT partition B block */
    CSL_McbspPBBlk     txPartBBlk;    

} CSL_McbspMulChSetup;

/**
 * This is the Setup structure for configuring MCBSP using CSL_mcbspHwSetup() 
 * function.
 */
typedef struct CSL_McbspHwSetup {
    /** Global configuration parameters */
    CSL_McbspGlobalSetup    *global;

    /** RCV data setup related parameters */
    CSL_McbspDataSetup      *rxdataset;

    /** XMT data setup related parameters */
    CSL_McbspDataSetup      *txdataset;

    /** Clock configuration parameters */
    CSL_McbspClkSetup       *clkset;

    /** Multichannel mode configuration parameters */
    CSL_McbspMulChSetup     *mulCh;

    /** Emulation mode parameters */
    CSL_McbspEmu            emumode ;

    /** Any extra parameters, for future use */
    void                    *extendSetup;
} CSL_McbspHwSetup;

/**
 * This structure/object holds the context of the instance of MCBSP
 * opened using CSL_mcbspOpen() function.
 *
 * Pointer to this object is passed as MCBSP Handle to all MCBSP CSL APIs.
 * CSL_mcbspOpen() function intializes this structure based on the parameters
 * passed 
 */
typedef struct CSL_McbspObj {
    /** Pointer to the register overlay structure of the MCBSP */
    CSL_McbspRegsOvly    regs;

    /** Instance of MCBSP being referred by this object  */
    CSL_InstNum          perNum;
} CSL_McbspObj;

/** This is a pointer to @a CSL_McbspObj and is passed as the first
 *  parameter to all MCBSP CSL APIs
 */
typedef struct CSL_McbspObj *CSL_McbspHandle;

/**************************************************************************\
* Register Configuration Structure of MCBSP
\**************************************************************************/

/** @brief Config structure of MCBSP.
 *
 *  This is used to configure MCBSP using CSL_HwSetupRaw function
 */
typedef struct  {
    /** Config structure of MCBSP. This is used to configure MCBSP
     * using CSL_HwSetupRaw function
     */
    /** Serial Port Control Register */
    volatile Uint32 SPCR;
    /** Receive Control Register */
    volatile Uint32 RCR;
    /** Transmit Control Register */
    volatile Uint32 XCR;
    /** Sample Rate Generator Register */
    volatile Uint32 SRGR;
    /** Multichannel Control Register */
    volatile Uint32 MCR;
    /** Receive Channel Enable Register for Partition A and B */
    volatile Uint32 RCERE0;
    /** Transmit Channel Enable Register for Partition A and B */
    volatile Uint32 XCERE0;
    /** Pin Control Register */
    volatile Uint32 PCR;
    /** Receive Channel Enable Register for Partition C and D */
    volatile Uint32 RCERE1;
    /** Transmit Channel Enable Register for Partition C and D */
    volatile Uint32 XCERE1;
    /** Receive Channel Enable Register for Partition E and F */
    volatile Uint32 RCERE2;
    /** Transmit Channel Enable Register for Partition E and F */
    volatile Uint32 XCERE2;
    /** Receive Channel Enable Register for Partition G and H */
    volatile Uint32 RCERE3;
    /** Transmit Channel Enable Register for Partition G and H */    
    volatile Uint32 XCERE3;
} CSL_McbspConfig;

/** Default Values for Config structure */
#define CSL_MCBSP_CONFIG_DEFAULTS { \
    CSL_MCBSP_SPCR_RESETVAL,    \
    CSL_MCBSP_RCR_RESETVAL,     \
    CSL_MCBSP_XCR_RESETVAL,     \
    CSL_MCBSP_SRGR_RESETVAL,    \
    CSL_MCBSP_MCR_RESETVAL,     \
    CSL_MCBSP_RCERE0_RESETVAL,  \
    CSL_MCBSP_XCERE0_RESETVAL,  \
    CSL_MCBSP_PCR_RESETVAL,     \
    CSL_MCBSP_RCERE1_RESETVAL,  \
    CSL_MCBSP_XCERE1_RESETVAL,  \
    CSL_MCBSP_RCERE2_RESETVAL,  \
    CSL_MCBSP_XCERE2_RESETVAL,  \
    CSL_MCBSP_RCERE3_RESETVAL,  \
    CSL_MCBSP_XCERE3_RESETVAL   \
}

/* ************************************************************************\
 * MCBSP global function declarations
\* ************************************************************************/

/*
 * =============================================================================
 *   @func CSL_mcbspInit
 *
 *   @desc
 *      This function is idempotent i.e. calling it many times is same as
 *      calling it once. This function is only for book-keeping purpose
 *      and it doesn't touch the hardware (read/write registers) in any manner.
 *
 *   @arg pContext
 *        Context information for mcbsp
 *
 *   @ret CSL_Status
 *        CSL_SOK - Always returns 
 *
 *   @eg
 *     CSL_mcbspInit (NULL);
 * =============================================================================
 */
extern CSL_Status  CSL_mcbspInit (
    CSL_McbspContext   *pContext
);

/*
 *============================================================================
 *   @func CSL_mcbspOpen
 *
 *   @desc
 *      Reserves the specified MCBSP for use. The device can be re-opened 
 *      anytime after it has been normally closed, if so required. The MCBSP  
 *      handle returned  by this call is input as an essential argument for
 *      the rest of the APIs in MCBSP module. 
 *
 *   @arg pMcbspObj
 *        Pointer to the object that holds reference to the instance of MCBSP 
 *        requested after the call
 *
 *   @arg mcbspNum
 *        Instance of mcbsp CSL to be opened.
 *        
 *   @arg pMcbspParam
 *        Pointer to module specific parameters
 *
 *   @arg pStatus
 *        pointer for returning status of the function call
 *
 *   @ret CSL_McbspHandle
 *        Valid mcbsp instance handle will be returned 
 *        if status value is equal to CSL_SOK.
 *
 *   @eg
 *      CSL_McbspHandle     hMcbsp;
 *      CSL_McbspObj        mcbspObj;
 *      CSL_McbspHwSetup    mcbspSetup;
 *      CSL_Status          status;
 *      ...
 *      hMcbsp = CSL_mcbspOpen(&mcbspObj, CSL_MCBSP_0, NULL, &status);
 *      ...
 * ===========================================================================
 */
extern CSL_McbspHandle CSL_mcbspOpen (
    CSL_McbspObj              *pMcbspObj,
    CSL_InstNum               mcbspNum,
    CSL_McbspParam            *pMcbspParam,
    CSL_Status                *pStatus
);


/*
 *============================================================================
 *   @func CSL_mcbspClose
 *
 *   @desc
 *      Unreserves the MCBSP identified by the handle passed.
 *
 *   @arg hMcbsp
 *        Mcbsp handle
 *
 *   @ret CSL_Status
 *        CSL_SOK            - Close successful
 *        CSL_ESYS_BADHANDLE - Invalid handle
 *
 *   @eg
 *      CSL_McbspHandle     hMcbsp;
 *      CSL_McbspObj        mcbspObj;
 *      CSL_McbspHwSetup    mcbspSetup;
 *      CSL_Status          status;
 *      ...
 *      hMcbsp = CSL_mcbspOpen(&mcbspObj, CSL_MCBSP_0, NULL, &status);
 *      ...
 *      CSL_mcbspClose(hMcbsp);
 * ===========================================================================
 */
extern CSL_Status  CSL_mcbspClose (
    CSL_McbspHandle        hMcbsp
);

/*
 * =============================================================================
 *   @func CSL_mcbspHwSetup
 *
 *   @desc
 *     Configures the MCBSP using the values passed in the setup structure.
 *
 *   @arg hMcbsp
 *        Handle to the mcbsp instance
 *
 *   @arg hwSetup
 *        Pointer to hardware setup structure
 *
 *   @ret CSL_Status
 *        CSL_SOK             - Hardware setup successful
 *        CSL_ESYS_BADHANDLE  - Invalid handle
 *        CSL_ESYS_INVPARAMS  - Hardware structure is not
 *                              properly initialized
 *
 *   @eg
 *      CSL_mcbspHandle hMcbsp;
 *      CSL_McbspHwSetup hwSetup = CSL_MCBSP_HWSETUP_DEFAULTS;
 *      ...
 *
 *      // Init Successfully done
 *      ...
 *      // Open Successfully done
 *      ...
 *      CSL_mcbspHwSetup(hMcbsp, &hwSetup);
 *      ...
 * ===========================================================================
 */
extern CSL_Status  CSL_mcbspHwSetup (
    CSL_McbspHandle       hMcbsp,
    CSL_McbspHwSetup      *setup
);

/*
 * =============================================================================
 *   @func CSL_mcbspHwSetupRaw
 *
 *   @desc
 *     This function initializes the device registers with the register-values
 *     provided through the Config Data structure.
 *
 *   @arg hMcbsp
 *        Handle to the mcbsp instance
 *
 *   @arg config
 *        Pointer to config structure
 *
 *   @ret CSL_Status
 *        CSL_SOK             - Configuration successful
 *        CSL_ESYS_BADHANDLE  - Invalid handle
 *        CSL_ESYS_INVPARAMS  - Config structure is not
 *                              properly initialized
 *
 *   @eg
 *      CSL_mcbspHandle     hMcbsp;
 *      CSL_McbspConfig     config = CSL_MCBSP_CONFIG_DEFAULTS;
 *      CSL_Status          status;
 *      ...
 *      status = CSL_mcbspHwSetupRaw (hMcbsp, &config);
 *      ...
 * ===========================================================================
 */
extern CSL_Status  CSL_mcbspHwSetupRaw (
    CSL_McbspHandle          hMcbsp,
    CSL_McbspConfig          *config
);

/*
 * =============================================================================
 *   @func CSL_mcbspRead
 *
 *   @desc
 *      Reads the data from MCBSP. The word length for the read operation is
 *      specefied using wordLen argument. According to this word length, 
 *      appropriate amount of data will read in the data object (variable);
 *      the pointer to which is passed as the third argument.
 *
 *   @arg hMcbsp
 *        Handle to the mcbsp instance
 *
 *   @arg wordLen
 *        Word length of data to be read in
 *
 *   @arg data
 *        Pointer to data object (variable) that will hold the input data
 *
 *   @ret CSL_Status
 *        CSL_SOK             - Read successful
 *        CSL_EMCBSP_INVSIZE  - Invalid word length
 *
 *   @eg
 *      CSL_mcbspHandle     hMcbsp;
 *      Uint16              inData;
 *      CSL_Status          status;
 *      ...
 *      // MCBSP object defined and HwSetup structure defined and initialized
 *      ...
 *      
 *      // Init, Open, HwSetup successfully done in that order
 *      ...
 *      // MCBSP SRG, Frame sync, RCV taken out of reset in that order
 *      ...
 *      status = CSL_mcbspRead(hMcbsp, CSL_MCBSP_WORDLEN_16, &inData);
 *      ...
 * ===========================================================================
 */
extern CSL_Status  CSL_mcbspRead (
    CSL_McbspHandle         hMcbsp,
    CSL_McbspWordLen        wordLen,
    void                    *data
);

/*
 * =============================================================================
 *   @func CSL_mcbspWrite
 *
 *   @desc
 *      Transmits the data from MCBSP. The word length for the write operation is
 *      specefied using wordLen argument. According to this word length, the
 *      appropriate amount of data will transmitted from the data object (variable);
 *      the pointer to which is passed as the third argument.
 *
 *   @arg hMcbsp
 *        Handle to the mcbsp instance
 *
 *   @arg wordLen
 *        Word length of data to be write
 *
 *   @arg data
 *        Pointer to data object (variable) that holds the data to be sent out
 *
 *   @ret CSL_Status
 *        CSL_SOK             - Read successful
 *        CSL_EMCBSP_INVSIZE  - Invalid word length
 *
 *   @eg
 *      CSL_mcbspHandle     hMcbsp;
 *      Uint16              outData;
 *      CSL_Status          status;
 *      ...
 *      // MCBSP object defined and HwSetup structure defined and initialized
 *      ...
 *      
 *      // Init, Open, HwSetup successfully done in that order
 *      ...
 *      // MCBSP SRG, Frame sync, RCV taken out of reset in that order
 *      ...
 *      outData = 0x1234;
 *      status = CSL_mcbspWrite(hMcbsp,CSL_MCBSP_WORDLEN_16,&outData);
 *      ...
 * ===========================================================================
 */
extern CSL_Status  CSL_mcbspWrite (
    CSL_McbspHandle         hMcbsp,
    CSL_McbspWordLen        wordLen,
    void                    *data
);

/*
 * =============================================================================
 *   @func CSL_mcbspIoWrite
 *
 *   @desc
 *      Sends the data using MCBSP pin which is configured as general purpose 
 *      output.The 16-bit data trasnmitted is specified by 'outputData' argument.
 *      MCBSP pin to use in this write operation is identified by the second
 *      argument.
 *
 *   @arg hMcbsp
 *        Handle to the mcbsp instance
 *
 *   @arg outputSel
 *        MCBSP pin to be used as general purpose output
 *
 *   @arg outputData
 *        1 bit output data to be transmitted 
 *
 *   @ret 
 *      None     
 *
 *   @eg
 *      Uint16          outData;
 *      CSL_Status      status;
 *      CSL_McbspHandle hMcbsp;
 *      ...
 *      // MCBSP object defined and HwSetup structure defined and initialized
 *      ...
 *      
 *      // Init, Open, HwSetup successfully done in that order
 *      ...
 *      outData = 1;
 *      inData = CSL_mcbspIoWrite(hMcbsp, CSL_MCBSP_IO_CLKX, outData);
 *      ...
 * ===========================================================================
 */
extern void  CSL_mcbspIoWrite (
    CSL_McbspHandle         hMcbsp,
    CSL_BitMask16           outputSel,
    Uint16                  outputData
);

/*
 * =============================================================================
 *   @func CSL_mcbspIoWrite
 *
 *   @desc
 *      Reads the data from MCBSP pin which is configured as general purpose 
 *      input.The 16-bit data read from this pin is returned by this API. MCBSP
 *      pin to use in this read operation is identified by the second argument.
 *
 *   @arg hMcbsp
 *        Handle to the mcbsp instance
 *
 *   @arg inputSel
 *        MCBSP pin to be used as general purpose input
 *
 *   @ret Uint16
 *        data read from the pin(s)    
 *
 *   @eg
 *      Uint16              inData, clkx_data, clkr_data;
 *      CSL_Status          status;
 *      CSL_BitMask16       inMask;
 *      CSL_McbspHandle     hMcbsp;
 *      ...
 *      // MCBSP object defined and HwSetup structure defined and initialized
 *      ...
 *      
 *      // Init, Open, HwSetup successfully done in that order
 *      ...
 *      inMask = CSL_MCBSP_IO_CLKX | CSL_MCBSP_IO_CLKR;
 *      inData = CSL_mcbspIoRead(hMcbsp, inMask);
 *      
 *      if ((inData & CSL_MCBSP_IO_CLKX) != 0) 
 *          clkx_data = 1;
 *      else 
 *          clkx_data = 0;
 *      if ((inData & CSL_MCBSP_IO_CLKR) != 0) 
 *          clkr_data = 1;
 *      else 
 *          clkr_data = 0;
 *      ...
 * ===========================================================================
 */
extern Uint16  CSL_mcbspIoRead (
    CSL_McbspHandle         hMcbsp,
    CSL_BitMask16           inputSel
);

/** ============================================================================
 *   @func CSL_mcbspHwControl
 *
 *   @desc
 *      This function takes an input control command with an optional argument
 *      and accordingly controls the operation/configuration of MCBSP.
 *
 *   @arg hMcbsp
 *        Handle to the Mcbsp instance
 *
 *   @arg cmd
 *        Operation to be performed on the mcbsp instance
 *
 *   @arg cmdArg
 *        Arguement specific to the command 
 *
 *   @ret CSL_Status
 *        CSL_SOK            - Command execution successful.
 *        CSL_ESYS_BADHANDLE - Invalid handle
 *        CSL_ESYS_INVCMD    - Invalid command
 *
 *   @eg
 *        CSL_Status            status;
 *        CSL_BitMask16         ctrlMask;
 *        CSL_McbspHandle       hMcbsp;
 *        ...
 *        // MCBSP object defined and HwSetup structure defined and initialized
 *        ...
 *      
 *        // Init successfully done
 *        ...
 *        // Open successfully done
 *        ...
 *        // HwSetup sucessfully done
 *        ...
 *        // MCBSP SRG and Frame sync taken out of reset
 *        ...
 *     
 *        ctrlMask = CSL_MCBSP_CTRL_RX_ENABLE | CSL_MCBSP_CTRL_TX_ENABLE; 
 *        status  = CSL_mcbspHwControl (hMcbsp, CSL_MCBSP_CMD_RESET_CONTROL,  
 *                                      &ctrlMask);
 *        ...
 * ===========================================================================
 */
extern CSL_Status  CSL_mcbspHwControl (
    CSL_McbspHandle             hMcbsp,
    CSL_McbspControlCmd         cmd,
    void                        *arg
);

/** ============================================================================
 *   @func CSL_mcbspGetHwStatus
 *
 *   @desc
 *      Gets the status of different operations or some setup-parameters of MCBSP.
 *      The status is returned through the third parameter.
 *
 *   @arg hMcbsp
 *        Handle to the Mcbsp instance
 *
 *   @arg myQuery
 *        Query to be performed
 *
 *   @arg response
 *        Pointer to buffer to return the data requested by the query passed
 *
 *   @ret CSL_Status
 *        CSL_SOK              - Query execution successful.
 *        CSL_ESYS_BADHANDLE   - Invalid handle
 *        CSL_ESYS_INVQUERY    - Invalid Query
 *
 *   @eg
 *      CSL_Status            status;
 *      CSL_BitMask16         ctrlMask;
 *      CSL_McbspHandle       hMcbsp;
 *      CSL_McbspHandle hMcbsp;
 *      Uint16 response;
 *      ...
 *      status = CSL_mcbspGetHwStatus(hMcbsp,CSL_MCBSP_QUERY_DEV_STATUS,
 *                                    &response);
 *
 *      if (response & CSL_MCBSP_RRDY) {
 *          // Receiver is ready to with new data
 *          ...
 *      }
 *      ...
 * ===========================================================================
 */
extern CSL_Status  CSL_mcbspGetHwStatus (
    CSL_McbspHandle                 hMcbsp,
    CSL_McbspHwStatusQuery          myQuery,
    void                            *response
);

/*
 * ============================================================================
 *   @func CSL_mcbspGetHwSetup
 *
 *   @desc
 *      Gets the status of some or all of the setup-parameters of MCBSP.
 *      To get the status of complete MCBSP h/w setup, all the sub-structure
 *      pointers inside the main HwSetup structure, should be non-NULL. 
 *
 *   @arg hMcbsp
 *        Handle to the mcbsp instance
 *
 *   @arg hwSetup
 *        Pointer to hardware setup structure
 *
 *   @ret CSL_Status
 *        CSL_SOK             - Hardware setup successful
 *        CSL_ESYS_BADHANDLE  - Invalid handle
 *        CSL_ESYS_INVPARAMS  - Hardware structure is not
 *                              properly initialized
 *
 *   @eg
 *        CSL_status                status;
 *        CSL_McbspGlobalSetup      gblSetup;
 *        CSL_McbspClkSetup         clkSetup; 
 *        CSL_McbspEmu              emuMode;                       
 *
 *        CSL_McbspHwSetup readSetup = {
                        &gblSetup,
                        NULL,    // RX Data-setup structure if not required
                        NULL,    // TX Data-setup structure if not required
                        &clkSetup,
                        NULL,    // Multichannel-setup structure if not required
                        emuMode
                    };
 *        
 *        status = CSL_mcbspGetHwSetup (hMcbs, &hwSetup);
 *
 * ===========================================================================
 */
extern CSL_Status  CSL_mcbspGetHwSetup (
    CSL_McbspHandle     hMcbsp,
    CSL_McbspHwSetup    *myHwSetup
);


/** ============================================================================
 *   @n@b CSL_mcbspGetBaseAddress
 *
 *   @b Description
 *   @n  Function to get the base address of the peripheral instance.
 *       This function is used for getting the base address of the peripheral
 *       instance. This function will be called inside the CSL_mcbspOpen()
 *       function call. This function is open for re-implementing if the user
 *       wants to modify the base address of the peripheral object to point to
 *       a different location and there by allow CSL initiated write/reads into
 *       peripheral. MMR's go to an alternate location.
 *
 *   @b Arguments
 *   @verbatim
            mcbspNum        Specifies the instance of the MCBSP to be opened.

            pMcbspParam     Module specific parameters.

            pBaseAddress    Pointer to baseaddress structure.

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
        CSL_Status          status;
        CSL_McbspBaseAddress  baseAddress;

       ...
      status = CSL_mcbspGetBaseAddress(CSL_MCBSP_PER_CNT, NULL, &baseAddress);

    @endverbatim
 * ===========================================================================
 */
CSL_Status   CSL_mcbspGetBaseAddress (
        CSL_InstNum              mcbspNum,
        CSL_McbspParam           *pMcbspParam,
        CSL_McbspBaseAddress     *pBaseAddress
);


#ifdef __cplusplus
}
#endif

#endif

