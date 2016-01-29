/* ============================================================================
 * Copyright (c) Texas Instruments Inc 2002, 2003, 2004, 2005
 *
 * Use of this software is controlled by the terms and conditions found in the
 * license agreement under which this software has been supplied.
 * ============================================================================
 */

/** ===========================================================================
 * @file csl_srio.h
 *
 * @brief API header file for serial rapid IO CSL
 * 
 * @Path $(CSLPATH)\srio\inc
 * 
 * @desc API header file for serial rapid IO CSL 
*/

/** ============================================================================
 * @mainpage SRIO
 *
 * @section Introduction
 *
 * @subsection xxx Purpose and Scope
 * The purpose of this document is to identify a set of common CSL APIs for
 * the SRIO Module across various devices. The CSL developer is expected
 * to refer to this document while designing APIs for these modules. Some
 * of the listed APIs may not be applicable to a given SRIO Module. While
 * in other cases this list of APIs may not be sufficient to cover all the
 * features of a particular SRIO Module. The CSL developer should use his
 * discretion in designing new APIs or extending the existing ones to cover
 * these.
 *
 *
 * @subsection aaa Terms and Abbreviations
 *   -# CSL:  Chip Support Library
 *   -# API:  Application Programmer Interface
 */

/* ============================================================================
 * Revision History
 * ===============
 * 09-Aug-2005 PSK File Created.
 * 15-Dec-2005 SD  Added structures and enums for the SERDES configuration
                   Updated the HwSetup structure for the same. 
 * ============================================================================
 */

#ifndef _CSL_SRIO_H_
#define _CSL_SRIO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <csl.h>
#include <cslr_srio.h>

/**
 * General purpose global typedef declarations  
 */

/**
 *  @brief This enum describes the commands used to control the SRIO through
 *  CSL_srioHwControl()
 */
typedef enum {
    /**
     * @brief   Enables/disables the peripheral   
     * @param   Bool
     */
    CSL_SRIO_CMD_PER_ENABLE = 0,

    /**
     * @brief   Enable/disable the SERDES PLLs. PLL enable macros can be 
     *          OR’ed to get the value
     * @param   Uint8 
     */
    CSL_SRIO_CMD_PLL_CONTROL = 1,

    /**
     * @brief   Clears doorbell    interrupts. Macros can be OR’ed to 
     *          get the value
     * @param   CSL_SrioPortData 
     */
    CSL_SRIO_CMD_DOORBELL_INTR_CLEAR = 2,

    /**
     * @brief   Clear load/store module interrupts. Macros can be OR’ed 
     *          to get the value
     * @param   Uint32
     */
    CSL_SRIO_CMD_LSU_INTR_CLEAR = 3,

    /**
     * @brief   Clears Error, Reset, and Special Event interrupts. 
     *          Macros can be OR’ed to get the value 
     * @param   Uint32
     */
    CSL_SRIO_CMD_ERR_RST_INTR_CLEAR = 4,

    /**
     * @brief   Sets 32-bit DSP byte source address
     * @param   CSL_SrioPortData
     */
    CSL_SRIO_CMD_DIRECTIO_SRC_NODE_ADDR_SET = 5,

    /**
     * @brief   Sets the rapid IO destination MSB address
     * @param   CSL_SrioPortData
     */
    CSL_SRIO_CMD_DIRECTIO_DST_ADDR_MSB_SET = 6,

    /**
     * @brief   Sets the rapid IO destination LSB address
     * @param   CSL_SrioPortData
     */
	CSL_SRIO_CMD_DIRECTIO_DST_ADDR_LSB_SET = 7,

    /**
     * @brief   Number of data bytes to Read/Write - up to 4KB
     * @param   CSL_SrioPortData
     */
    CSL_SRIO_CMD_DIRECTIO_XFR_BYTECNT_SET = 8,

    /**
     * @brief   Sets 4 MSBs to 4-bit ftype field for all packets and 4 LSBs 
     *          to 4-bit trans field for Packet types 2,5 and 8
     * @param   CSL_SrioPortData
     */
    CSL_SRIO_CMD_DIRECTIO_LSU_XFR_TYPE_SET = 9,

    /**
     * @brief   Sets RapidIO doorbell info field for type 10 packets 
     *          and sets the packet type to 10
     * @param   CSL_SrioPortData
     */
    CSL_SRIO_CMD_DOORBELL_XFR_SET = 10,

    /**
     * @brief   Sets LSU flow masks. Port number is passed as input.
     *          Macros can be OR’ed to get the value for argument
     * @param   CSL_SrioPortData
     */
    CSL_SRIO_CMD_DIRECTIO_LSU_FLOW_MASK_SET = 11,

    /**
     * @brief   Sets the command to be sent in the link-request control symbol
     * @param   CSL_SrioPortData
     */
    CSL_SRIO_CMD_PORT_COMMAND_SET = 12,

    /**
     * @brief	Clear fields’ status of SP_ERR_STAT register. Macros can be 
     *          OR’ed to get the value to pass the argument
     * @param	CSL_SrioPortData
     */
    CSL_SRIO_CMD_SP_ERR_STAT_CLEAR = 13,

    /**
     * @brief   Clear status of  Logical/Transport layer errors.
     *          Macros can be OR’ed to get the value to pass the argument 
     * @param	Uint32
     */
    CSL_SRIO_CMD_LGCL_TRANS_ERR_STAT_CLEAR = 14,

    /**
     * @brief   Clears status of port errors interrupts.
     *          Macros can be OR’ed to get the value to pass the argument
     *          
     * @param	CSL_SrioPortData
     */
    CSL_SRIO_CMD_SP_ERR_DET_STAT_CLEAR = 15, 

    /**
     * @brief   Clear the fields status of the SP_CTL_INDEP register
     *          
     * @param	CSL_SrioPortData 
     */
    CSL_SRIO_CMD_SP_CTL_INDEP_ERR_STAT_CLEAR = 16,

    /**
     * @brief   Set control symbols used for  packet acknowledgment
     *          
     * @param	CSL_SrioCntlSym
     */
    CSL_SRIO_CMD_CNTL_SYM_SET = 17,

    /**
     * @brief   Sets interrupt rate control counter
     *          
     * @param	Uint32
     */
    CSL_SRIO_CMD_INTDST_RATE_CNTL = 18
} CSL_SrioHwControlCmd;

/**
 *  @brief This enum describes the commands used to get status of various parameters of the
 *  SRIO. These values are used in CSL_srioGetHwStatus()
 */
typedef enum {
    /**
     * @brief   This query command returns the SRIO Peripheral 
     *          Identification number
     *          
     * @param   CSL_SrioPidNumber
     */
    CSL_SRIO_QUERY_PID_NUMBER = 0,

    /**
     * @brief   Gets global enable status
     * 
     * @param   Uint32 
     */
    CSL_SRIO_QUERY_GBL_EN_STAT = 1,
    
    /**
     * @brief   Gets block enable status for all the blocks
     *
     * @param   CSL_SrioBlkEn
     */
    CSL_SRIO_QUERY_BLK_EN_STAT = 2,

    /**
     * @brief   Get doorbell  interrupts status. The port number is 
     *          passed as input
     *
     * @param   CSL_SrioPortData
     */
    CSL_SRIO_QUERY_DOORBELL_INTR_STAT = 3,

    /**
     * @brief	Get the LSU interrupts status
     * 
     * @param	Uint32
     */
    CSL_SRIO_QUERY_LSU_INTR_STAT = 4,

    /**
     * @brief   Gets  Error, Reset, and Special Event interrupts status
     * 
     * @param	Uint32
     */
    CSL_SRIO_QUERY_ERR_RST_INTR_STAT = 5,
    
    /**
     * @brief	Get status of LSU interrupts decode for DST 0
     * 
     * @param	Bool
     */
    CSL_SRIO_QUERY_LSU_INTR_DECODE_STAT = 6,

    /**
     * @brief	Get Error, Reset, and Special Event interrupts decode 
     *          status for DST 0
     * 
     * @param	Bool
     */
    CSL_SRIO_QUERY_ERR_INTR_DECODE_STAT = 7,

    /**
     * @brief   Gets the status of the pending command of LSU 
     *          registers for a particular port   	
     * 
     * @param	CSL_SrioLsuCompStat
     */
    CSL_SRIO_QUERY_LSU_COMP_CODE_STAT = 8,

    /**
     * @brief	Gets status of the command registers of LSU module 
     *          for a particular port
     * 
     * @param	CSL_SrioPortData
     */
    CSL_SRIO_QUERY_LSU_BSY_STAT = 9,

    /**
     * @brief	Gets the type of device (Vendor specific)
     * 
     * @param	CSL_SrioDevInfo
     */
    CSL_SRIO_QUERY_DEV_ID_INFO = 10,

    /**
     * @brief	Gets vendor specific assembly information
     * 
     * @param	CSL_SrioAssyInfo
     */
    CSL_SRIO_QUERY_ASSY_ID_INFO = 11,

    /**
     * @brief	Gets processing element features 
     * 
     * @param	Uint32
     */
    CSL_SRIO_QUERY_PE_FEATURE = 12,

    /**
     * @brief	Get source operations CAR status
     * 
     * @param	Uint32
     */
    CSL_SRIO_QUERY_SRC_OPERN_SUPPORT = 13,

    /**
     * @brief	Get destination operations CAR status
     * 
     * @param	Uint32
     */
    CSL_SRIO_QUERY_DST_OPERN_SUPPORT = 14,

    /**
     * @brief  	Get local configuration space base addresses 
     * 
     * @param	CSL_SrioLongAddress
     */
    CSL_SRIO_QUERY_LCL_CFG_BAR = 15,

    /**
     * @brief	Get status of SP_LM_RESP register fields
     * 
     * @param	CSL_SrioPortData
     */
    CSL_SRIO_QUERY_SP_LM_RESP_STAT = 16,

    /**
     * @brief	Get status of SP_ACKID_STAT register fields
     * 
     * @param	CSL_SrioPortData
     */
    CSL_SRIO_QUERY_SP_ACKID_STAT = 17,

    /**
     * @brief	Get status of SP_ERR_STAT register fields
     * 
     * @param	CSL_SrioPortData
     */
    CSL_SRIO_QUERY_SP_ERR_STAT = 18,

    /**
     * @brief	Gets SP_CTL register status fields
     * 
     * @param	CSL_SrioPortData
     */
    CSL_SRIO_QUERY_SP_CTL = 19,

   /**
     * @brief	Get the status of logical/transport layer errors 
     * 
     * @param	Uint32
     */
    CSL_SRIO_QUERY_LGCL_TRNS_ERR_STAT = 20,

    /**
     * @brief	Get captured  error info of logical/transport layer 
     * 
     * @param	CSL_SrioLogTrErrInfoCapt
     */
    CSL_SRIO_QUERY_LGCL_TRNS_ERR_CAPT = 21,

    /**
     * @brief	Get status of port error detect CSR fields
     * 
     * @param	CSL_SrioPortData
     */
    CSL_SRIO_QUERY_SP_ERR_DET_STAT = 22,

    /**
     * @brief	Get the  port  error captured information 
     * 
     * @param	CSL_SrioPortErrCapt
     */
    CSL_SRIO_QUERY_PORT_ERR_CAPT = 23,
    
    /**
     * @brief	Get port control independent register fields status
     * 
     * @param	CSL_SrioPortData
     */
    CSL_SRIO_QUERY_SP_CTL_INDEP = 24,

    /**
     * @brief	Get the port write capture information
     * 
     * @param	CSL_SrioPortWriteCapt
     */
    CSL_SRIO_QUERY_PW_CAPTURE = 25,

	/**
     * @brief   Reads the count of the number of transmission 
     *          errors that have occurred
     *          
     * @param	CSL_SrioPortData
     */
    CSL_SRIO_QUERY_ERR_RATE_CNTR_READ = 26,
    
    /**
     * @brief   Reads the peak value of the error rate counter
     *          
     * @param	CSL_SrioPortData
     */
    CSL_SRIO_QUERY_PEAK_ERR_RATE_READ = 27
} CSL_SrioHwStatusQuery;

/**
 *  @brief This enum describes type of the captured information type at the 
 *         time of port error.
 */
typedef enum {
    /** Port captured packet data during error */
    CSL_SRIO_CAPT_TYPE_PKT = 0,

    /** Port captured control symbols during error */
    CSL_SRIO_CAPT_TYPE_CNTL_SYM = 1,

    /** Port captured implementation specific during error */
    CSL_SRIO_CAPT_TYPE_IMP_SPEC = 2
} CSL_SrioPortCaptType;

/**
 *  @brief This enum describes the port number configuration for SRIO.
 */
typedef enum {
    /** Port number 0 */
    CSL_SRIO_PORT_0 = 0,

    /** Port number 1 */
    CSL_SRIO_PORT_1 = 1,

    /** Port number 2 */
    CSL_SRIO_PORT_2 = 2,

    /** Port number 3 */
    CSL_SRIO_PORT_3 = 3
} CSL_SrioPortNum;

/**
 *  @brief This enum describes the discovery time for the link partner to 
 *         enter its discovery state.
 */
typedef enum {
    /** Discovery time is 102.4ps (for debug mode only) */
    CSL_SRIO_DISCOVERY_TIME_0 = 0,

    /**  Discovery time is 0.84ms */
    CSL_SRIO_DISCOVERY_TIME_1 = 1,

    /**  Discovery time is 0.84ms*2 */
    CSL_SRIO_DISCOVERY_TIME_2 = 2,

    /**  Discovery time is 0.84ms*3 */
    CSL_SRIO_DISCOVERY_TIME_3 = 3,

    /**  Discovery time is 0.84ms*4 */
    CSL_SRIO_DISCOVERY_TIME_4 = 4,

    /**  Discovery time is 0.84ms*5 */
    CSL_SRIO_DISCOVERY_TIME_5 = 5,

    /**  Discovery time is 0.84ms*6 */
    CSL_SRIO_DISCOVERY_TIME_6 = 6,

    /**  Discovery time is 0.84ms*7 */
    CSL_SRIO_DISCOVERY_TIME_7 = 7,

    /**  Discovery time is 0.84ms*8 */
    CSL_SRIO_DISCOVERY_TIME_8 = 8,

    /**  Discovery time is 0.84ms*9 */
    CSL_SRIO_DISCOVERY_TIME_9 = 9,

    /**  Discovery time is 0.84ms*10 */
    CSL_SRIO_DISCOVERY_TIME_10 = 10,

    /**  Discovery time is 0.84ms*11 */
    CSL_SRIO_DISCOVERY_TIME_11 = 11,

    /**  Discovery time is 0.84ms*12 */
    CSL_SRIO_DISCOVERY_TIME_12 = 12,

    /**  Discovery time is 0.84ms*13 */
    CSL_SRIO_DISCOVERY_TIME_13 = 13,

    /**  Discovery time is 0.84ms*14 */
    CSL_SRIO_DISCOVERY_TIME_14 = 14,

    /**  Discovery time is 0.84ms*15 */
    CSL_SRIO_DISCOVERY_TIME_15 = 15
} CSL_SrioDiscoveryTimer;

/**
 *  @brief This enum describes the port write time for request.
 */
typedef enum {
    /** Port write is sent only once (disabled) */
    CSL_SRIO_PW_TIME_0 = 0,

    /** Port write time is 107ms - 214ms */
    CSL_SRIO_PW_TIME_1 = 1,

    /** Port write time is 214ms - 321ms */
    CSL_SRIO_PW_TIME_2 = 2,

    /** Port write time is 428ms - 535ms */
    CSL_SRIO_PW_TIME_6 = 6,
    
    /** Port write time is 856ms - 963ms */
    CSL_SRIO_PW_TIME_8 = 8,
        
    /** Port write time is 0.82 - 1.64us */
    CSL_SRIO_PW_TIME_15 = 15
} CSL_SrioPwTimer;

/**
 *  @brief This enum describes the time values for the port in silent state.
 */
typedef enum {
    /** Port in silent state for 64ns (debug mode) */
    CSL_SRIO_SILENCE_TIME_0 = 0,

    /** Port in silent state for 13.1us*1 */
    CSL_SRIO_SILENCE_TIME_1 = 1,
    
    /** Port in silent state for 13.1us*2 */
    CSL_SRIO_SILENCE_TIME_2 = 2,
    
    /** Port in silent state for 13.1us*3 */
    CSL_SRIO_SILENCE_TIME_3 = 3,
    
    /** Port in silent state for 13.1us*4 */
    CSL_SRIO_SILENCE_TIME_4 = 4,
    
    /** Port in silent state for 13.1us*5 */
    CSL_SRIO_SILENCE_TIME_5 = 5,
    
    /** Port in silent state for 13.1us*6 */
    CSL_SRIO_SILENCE_TIME_6 = 6,
    
    /** Port in silent state for 13.1us*7 */
    CSL_SRIO_SILENCE_TIME_7 = 7,
    
    /** Port in silent state for 13.1us*8 */
    CSL_SRIO_SILENCE_TIME_8 =8,
    
    /** Port in silent state for 13.1us*9 */
    CSL_SRIO_SILENCE_TIME_9 = 9,
    
    /** Port in silent state for 13.1us*10 */
    CSL_SRIO_SILENCE_TIME_10 = 10,
    
    /** Port in silent state for 13.1us*11 */
    CSL_SRIO_SILENCE_TIME_11 = 11,
    
    /** Port in silent state for 13.1us*12 */
    CSL_SRIO_SILENCE_TIME_12 = 12,
    
    /** Port in silent state for 13.1us*13 */
    CSL_SRIO_SILENCE_TIME_13 = 13,
    
    /** Port in silent state for 13.1us*14 */
    CSL_SRIO_SILENCE_TIME_14 = 14,
    
    /** Port in silent state for 13.1us*15 */
    CSL_SRIO_SILENCE_TIME_15
} CSL_SrioSilenceTimer;

/**
 *  @brief This enum describes the bus transaction priority values for SRIO
 */
typedef enum {
    /** Sets internal bus priority to 0(highest) */
    CSL_SRIO_BUS_TRANS_PRIORITY_0 = 0,

    /** Sets internal bus priority to 1 */
    CSL_SRIO_BUS_TRANS_PRIORITY_1 = 1,
    
    /** Sets internal bus priority to 2 */
    CSL_SRIO_BUS_TRANS_PRIORITY_2 = 2,
    
    /** Sets internal bus priority to 3 */
    CSL_SRIO_BUS_TRANS_PRIORITY_3 = 3,
    
    /** Sets internal bus priority to 4 */
    CSL_SRIO_BUS_TRANS_PRIORITY_4 = 4,
    
    /** Sets internal bus priority to 5 */
    CSL_SRIO_BUS_TRANS_PRIORITY_5 = 5,
    
    /** Sets internal bus priority to 6 */
    CSL_SRIO_BUS_TRANS_PRIORITY_6 = 6,
    
    /** Sets internal bus priority to 7 */
    CSL_SRIO_BUS_TRANS_PRIORITY_7 = 7
} CSL_SrioBusTransPriority;

/**
 *  @brief This enum describes the internal clock prescale values for SRIO
 */
typedef enum {
    /** Sets the internal clock frequency Min 44.7 and Max 89.5 */
    CSL_SRIO_CLK_PRESCALE_0 = 0,

    /** Sets the internal clock frequency Min 89.5 and Max 179.0 */
    CSL_SRIO_CLK_PRESCALE_1 = 1,
    
    /*Sets the internal clock frequency Min 134.2 and Max 268.4 8 */
    CSL_SRIO_CLK_PRESCALE_2 = 2,
    
    /** Sets the internal clock frequency Min 180.0 and Max 360.0  */
    CSL_SRIO_CLK_PRESCALE_3 = 3,
    
    /** Sets the internal clock frequency Min 223.7 and Max 447.4 */
    CSL_SRIO_CLK_PRESCALE_4 = 4,
    
    /** Sets the internal clock frequency Min 268.4 and Max 536.8 */
    CSL_SRIO_CLK_PRESCALE_5 = 5,
    
    /** Sets the internal clock frequency Min 313.2 and Max 626.4 */
    CSL_SRIO_CLK_PRESCALE_6 = 6,
    
    /** Sets the internal clock frequency Min 357.9 and Max 715.8 */
    CSL_SRIO_CLK_PRESCALE_7 = 7,
    
    /** sets the internal clock frequency Min 402.6 and Max 805.4 */
    CSL_SRIO_CLK_PRESCALE_8 = 8,
    
    /** Sets the internal clock frequency Min 447.4 and Max 894.8 */
    CSL_SRIO_CLK_PRESCALE_9 = 9,
    
    /** Sets the internal clock frequency Min 492.1 and Max 984.2 */
    CSL_SRIO_CLK_PRESCALE_10 = 10,
    
    /** Sets the internal clock frequency Min 536.9 and Max 1073.8 */
    CSL_SRIO_CLK_PRESCALE_11 = 11,
    
    /** Sets the internal clock frequency Min 581.6 and Max 1163.2 */
    CSL_SRIO_CLK_PRESCALE_12 = 12,
    
    /** Sets the internal clock frequency Min 626.3 and Max 1252.6 */
    CSL_SRIO_CLK_PRESCALE_13 = 13,
    
    /** Sets the internal clock frequency Min 671.1 and Max 1342.2 */
    CSL_SRIO_CLK_PRESCALE_14 = 14,
    
    /** Sets the internal clock frequency Min 715.8 and Max 1431.6 */
    CSL_SRIO_CLK_PRESCALE_15 = 15
} CSL_SrioClkDiv;

/**
 *  @brief This enum describes required buffer count for packets to be sent 
 *         across the UDI interface.
 */
typedef enum {
    /** Transmit credit threshold 1 */
    CSL_SRIO_TX_PRIORITY_WM_0 = 0,

    /** Transmit credit threshold 2 */
    CSL_SRIO_TX_PRIORITY_WM_1 = 1,

    /** Transmit credit threshold 3 */
    CSL_SRIO_TX_PRIORITY_WM_2 = 2,

    /** Transmit credit threshold 4 */
    CSL_SRIO_TX_PRIORITY_WM_3 = 3,
        
    /** Transmit credit threshold 5 */
    CSL_SRIO_TX_PRIORITY_WM_4 = 4,
    
    /** Transmit credit threshold 6 */
    CSL_SRIO_TX_PRIORITY_WM_5 = 5,
    
    /** Transmit credit threshold 7 */
    CSL_SRIO_TX_PRIORITY_WM_6 = 6,
    
    /** Transmit credit threshold 8 */
    CSL_SRIO_TX_PRIORITY_WM_7 = 7
} CSL_SrioTxPriorityWm;

/**
 *  @brief This enum describes extended addressing control bits.
 */
typedef enum {
    /** PE supports 66 bit addresses */
    CSL_SRIO_ADDR_SELECT_66BIT = 0,

    /** PE supports 50 bit addresses */
    CSL_SRIO_ADDR_SELECT_50BIT = 1,

    /** PE supports 34 bit addresses (default) */
    CSL_SRIO_ADDR_SELECT_34BIT = 2
} CSL_SrioAddrSelect;

/**  @brief This enum describes UDI buffers setup.
 */
typedef enum {
    /** UDI buffers are port based */
    CSL_SRIO_1X_MODE_PORT = 1,

    /** UDI buffers are priority based */
    CSL_SRIO_1X_MODE_PRIORITY = 0
} CSL_SrioBufMode;

/** @brief This enum describes the port width override options 
 */
typedef enum {
	/** No override to the port width  */
	CSL_SRIO_PORT_WIDTH_NO_OVERRIDE = 0,

	/** Watchdog timeout occured */
	CSL_SRIO_PORT_WIDTH_LANE_0 = 1,

    /** Force single lane, lane 2 */
    CSL_SRIO_PORT_WIDTH_LANE_2
} CSL_SrioPortWidthOverride;

/**
 * @brief This enum describes the error rate bias values
 */
typedef enum {
    /** Error rate counter do not decrement */
    CSL_SRIO_ERR_RATE_BIAS_0 = 0x0,

    /** Error rate counter decrements every 1ms */
    CSL_SRIO_ERR_RATE_BIAS_1MS = 0x1,

    /** Error rate counter decrements every 10ms */
    CSL_SRIO_ERR_RATE_BIAS_10MS = 0x3,

    /** Error rate counter decrements every 100ms */
    CSL_SRIO_ERR_RATE_BIAS_100MS = 0x7,

    /** Error rate counter decrements every 1s */
    CSL_SRIO_ERR_RATE_BIAS_1S = 0xf,

    /** Error rate counter decrements every 10s */
    CSL_SRIO_ERR_RATE_BIAS_10S = 0x1f,

    /** Error rate counter decrements every 100s */
    CSL_SRIO_ERR_RATE_BIAS_100S = 0x3f,

    /** Error rate counter decrements every 1000s */
    CSL_SRIO_ERR_RATE_BIAS_1000S = 0x7f,

    /** Error rate counter decrements every 1000s */
    CSL_SRIO_ERR_RATE_BIAS_10000S = 0xff
} CSL_SrioErrRtBias;

/**
 * @brief This enum describes the port link timeout values
 */
typedef enum {
    /** Timer disabled */
    CSL_SRIO_PORT_LNK_TIMEOUT_0 = 0,
    
    /** Timeout value is 205ns */
    CSL_SRIO_PORT_LNK_TIMEOUT_1 = 1,
    
    /** Timeout value is 3.1us */
    CSL_SRIO_PORT_LNK_TIMEOUT_2 = 2,
    
    /** Timeout value is 52.4us */
    CSL_SRIO_PORT_LNK_TIMEOUT_3 = 3,
    
    /** Timeout value is 839.5us */
    CSL_SRIO_PORT_LNK_TIMEOUT_4 = 4,
    
    /** Timeout value is 13.4ms */
    CSL_SRIO_PORT_LNK_TIMEOUT_5 = 5,
    
    /** Timeout value is 215ms */
    CSL_SRIO_PORT_LNK_TIMEOUT_6 = 6,
    
    /** Timeout value is 3.4s */
    CSL_SRIO_PORT_LNK_TIMEOUT_7 = 7
} CSL_SrioPortLnkTimeout;

/**
 * @brief This enumeration indicates the status of the pending command
 */
typedef enum {
    /** Transaction complete, no errors (Posted/Non-posted) */
    CSL_SRIO_TRANS_NO_ERR = 0,
        
    /** Transaction timeout occurred on non-posted transaction */
    CSL_SRIO_TRANS_TIMEOUT = 1,
    
    /** Transaction complete, packet not sent due to flow control blockade 
     *  (Xoff)
     */
    CSL_SRIO_FLOW_CNTL_BLOCKADE = 2,
    
    /** Transaction complete, non-posted response packet (type 8 and 13) 
     *  contained ERROR status, or response payload length was in error
     */
    CSL_SRIO_RESP_PKT_ERR = 3,
    
    /** Transaction complete, packet not sent due to unsupported transaction 
     *  type or invalid programming encoding for one or more LSU register fields
     */
    CSL_SRIO_INV_PROG_ENCODING = 4,
    
    /** DMA data transfer error */
    CSL_SRIO_DMA_TRANS_ERR = 5,
    
    /** "Retry" DOORBELL response received, or atomic test-and-swap was not 
     *  allowed (semaphore in use) 
     */
    CSL_SRIO_RETRY_DRBL_RESP_RCVD = 6,
    
    /** Transaction complete, packet not sent due to unavailable outbound 
     *  credit at given priority 
     */
    CSL_SRIO_UNAVAILABLE_OUTBOUND_CR = 7
} CSL_SrioCompCode;

/**
 * @brief This enum describes error rate counter threshold values
 */
typedef enum {
    /** Only count 2 errors and above */
    CSL_SRIO_ERR_RATE_COUNT_2 = 0,
        
    /** Only count 4 errors and above */
    CSL_SRIO_ERR_RATE_COUNT_4 = 1,
    
    /** Only count 16 errors and above */
    CSL_SRIO_ERR_RATE_COUNT_16 = 2,
    
    /** No limit in incrementing the error rate count */
    CSL_SRIO_ERR_RATE_COUNT_N0_LIMIT = 3
} CSL_SrioErrRtNum;

/** @brief Enum for SERDES Loop bandwidth */
typedef enum {
    /** Frequency dependant loop bandwidth */
    CSL_SRIO_SERDES_LOOP_BANDWIDTH_FREQ_DEP = 
                            CSL_SRIO_SERDES_CFG_CNTL_LB_FREQ_DEP,

    /** Low loop bandwidth */
    CSL_SRIO_SERDES_LOOP_BANDWIDTH_LOW =  
                            CSL_SRIO_SERDES_CFG_CNTL_LB_LOW,

    /** High loop bandwidth */
    CSL_SRIO_SERDES_LOOP_BANDWIDTH_HIGH = 
                            CSL_SRIO_SERDES_CFG_CNTL_LB_HIGH
} CSL_SrioSerDesLoopBandwidth;

/** @brief Enum for SERDES PLL multiplication factor values */
typedef enum {
    /** SERDES PLL multiplication factor 4 */
    CSL_SRIO_SERDES_PLL_MPLY_BY_4 = CSL_SRIO_SERDES_CFG_CNTL_MPY_4,

    /** SERDES PLL multiplication factor 5 */
    CSL_SRIO_SERDES_PLL_MPLY_BY_5 = CSL_SRIO_SERDES_CFG_CNTL_MPY_5,

    /** SERDES PLL multiplication factor 6 */
    CSL_SRIO_SERDES_PLL_MPLY_BY_6 = CSL_SRIO_SERDES_CFG_CNTL_MPY_6,

    /** SERDES PLL multiplication factor 8 */
    CSL_SRIO_SERDES_PLL_MPLY_BY_8 = CSL_SRIO_SERDES_CFG_CNTL_MPY_8,

    /** SERDES PLL multiplication factor 10 */
    CSL_SRIO_SERDES_PLL_MPLY_BY_10 = CSL_SRIO_SERDES_CFG_CNTL_MPY_10,

    /** SERDES PLL multiplication factor 12 */
    CSL_SRIO_SERDES_PLL_MPLY_BY_12 = CSL_SRIO_SERDES_CFG_CNTL_MPY_12,

    /** SERDES PLL multiplication factor 12.5 */
    CSL_SRIO_SERDES_PLL_MPLY_BY_12_5 = CSL_SRIO_SERDES_CFG_CNTL_MPY_12_5,

    /** SERDES PLL multiplication factor 15 */
    CSL_SRIO_SERDES_PLL_MPLY_BY_15 = CSL_SRIO_SERDES_CFG_CNTL_MPY_15,

    /** SERDES PLL multiplication factor 20 */
    CSL_SRIO_SERDES_PLL_MPLY_BY_20 = CSL_SRIO_SERDES_CFG_CNTL_MPY_20,

    /** SERDES PLL multiplication factor 25 */
    CSL_SRIO_SERDES_PLL_MPLY_BY_25 = CSL_SRIO_SERDES_CFG_CNTL_MPY_25,

    /** SERDES PLL multiplication factor 50 */
    CSL_SRIO_SERDES_PLL_MPLY_BY_50 = CSL_SRIO_SERDES_CFG_CNTL_MPY_50,

    /** SERDES PLL multiplication factor 60 */
    CSL_SRIO_SERDES_PLL_MPLY_BY_60 = CSL_SRIO_SERDES_CFG_CNTL_MPY_60
} CSL_SrioSerDesPllMply;


/** @brief Enum for SERDES loss of signal detection configuration */
typedef enum {
    /** Loss of signal detection disabled */
     CSL_SRIO_SERDES_LOS_DET_DISABLE = 0,

    /** Loss of signal detection threshold in the range 85 to 195mVdfpp.*/
     CSL_SRIO_SERDES_LOS_DET_HIGH_THRESHOLD = 1,

    /** Loss of signal detection threshold in the range 65 to 175mVdfpp.*/
     CSL_SRIO_SERDES_LOS_DET_LOW_THRESHOLD = 2
} CSL_SrioSerDesLos;


/** @brief Enum for SERDES symbol alignment configuration */
typedef enum {
    /** Symbol alignment disbaled */
    CSL_SRIO_SERDES_SYM_ALIGN_DISABLE = 0,

    /** Comma alignment: Symbol alignment will be performed whenever a
      * misaligned comma symbol is received.
      */
    CSL_SRIO_SERDES_SYM_ALIGN_COMMA = 1,

    /** Alignment Jog. The symbol alignment will be adjusted by one bit 
      * position 
      */
    CSL_SRIO_SERDES_SYM_ALIGN_JOG =  2
} CSL_SrioSerDesSymAlignment;
    
/** @brief Enum for SERDES input termination */
typedef enum {
    /** Input termination is to VDDT */
    CSL_SRIO_SERDES_TERMINATION_VDDT = CSL_SRIO_SERDES_CFGRX_CNTL_TERM_VDDT,

    /** Input termination is to 0.8 VDDT */
    CSL_SRIO_SERDES_TERMINATION_0_8_VDDT = 
                                    CSL_SRIO_SERDES_CFGRX_CNTL_TERM_0_8_VDDT,

    /** Input termination is floating */
    CSL_SRIO_SERDES_TERMINATION_FLOATING = 
                                    CSL_SRIO_SERDES_CFGRX_CNTL_TERM_FLOATING
} CSL_SrioSerDesTermination;

/** @brief Enum for the SERDES operating rate configuration */
typedef enum {
    /** Full rate operation */
    CSL_SRIO_SERDES_RATE_FULL = 0,

    /** Half rate operation */
    CSL_SRIO_SERDES_RATE_HALF = 1,

    /** Quarter rate operation */
    CSL_SRIO_SERDES_RATE_QUARTER = 2
} CSL_SrioSerDesRate;                                     

/** @brief Enum for the SERDES bus width configuration */
typedef enum {
    /** 10 bit bus width */
    CSL_SRIO_SERDES_BUS_WIDTH_10_BIT = 0,

    /** 8 bit bus width */
    CSL_SRIO_SERDES_BUS_WIDTH_8_BIT = 1
} CSL_SrioSerDesBusWidth;

/** @brief Enum for SERDES output swing configuration */
typedef enum {
    /** output swing amplitude 125 */
    CSL_SRIO_SERDES_SWING_AMPLITUDE_125 = 0,

    /** output swing amplitude 250 */
    CSL_SRIO_SERDES_SWING_AMPLITUDE_250 = 1,

    /** output swing amplitude 500 */
    CSL_SRIO_SERDES_SWING_AMPLITUDE_500 = 2,

    /** output swing amplitude 625 */
    CSL_SRIO_SERDES_SWING_AMPLITUDE_625 = 3,

    /** output swing amplitude 750 */
    CSL_SRIO_SERDES_SWING_AMPLITUDE_750 = 4,

    /** output swing amplitude 1000 */
    CSL_SRIO_SERDES_SWING_AMPLITUDE_1000 = 5,

    /** output swing amplitude 1125 */
    CSL_SRIO_SERDES_SWING_AMPLITUDE_1125 = 6,

    /** output swing amplitude 1250 */
    CSL_SRIO_SERDES_SWING_AMPLITUDE_1250 = 7
} CSL_SrioSerDesSwingCfg;

/** @brief Enum for SERDES TX common mode configuration */
typedef enum {
    /** Normal: Common mode not adjusted */
    CSL_SRIO_SERDES_COMMON_MODE_NORMAL = 0,

    /** Raised: Common mode raised by 5% of e54 */
    CSL_SRIO_SERDES_COMMON_MODE_RAISED = 1
} CSL_SrioSerDesCommonMode;

/**
 * @brief This structure contains the control and congestion flow mask 
 *        registers for the configuration of Load/Store module in SRIO
 */
typedef struct {
    /** LSU control register 0 */
    Uint32 LSU_REG0;
    
    /** LSU control register 1 */
    Uint32 LSU_REG1;
    
    /** LSU control register 2 */
    Uint32 LSU_REG2;
    
    /** LSU control register 3 */
    Uint32 LSU_REG3;
    
    /** LSU control register 4 */
    Uint32 LSU_REG4;
    
    /** core n LSU congestion control flow mask register */
    Uint32 LSU_FLOW_MASKS;
} CSL_SrioCfgLsuRegs; 

/**
 * @brief This structure contains port configuration CSR registers
 */
typedef struct {
    /** Port link maintenance request CSR */
    Uint32 SP_LM_REQ;
        
    /** Port local ACK ID status CSR */
    Uint32 SP_ACKID_STAT;
    
    /** Port error and status CSR */
    Uint32 SP_ERR_STAT;
    
    /** Port control CSR */
    Uint32 SP_CTL;
} CSL_SrioCfgPortRegs;

/**
 * @brief This structure contains port error configuration CSR registers
 */
typedef struct {
    /** Port error detect CSR */
    Uint32 SP_ERR_DET;
    
    /** Port error enable CSR */
    Uint32 SP_RATE_EN;
    
    /** Port error rate CSR */
    Uint32 SP_ERR_RATE;
    
    /** Port error rate threshold CSR */
    Uint32 SP_ERR_THRESH;
} CSL_SrioCfgPortErrorRegs;

/**
 * @brief This structure contains port error configuration CSR registers.
 */
typedef struct {
    /** Port reset option CSR */
    Uint32 SP_RST_OPT;
    /** Port control independent register */
    Uint32 SP_CTL_INDEP;
    /** Port silence timer register */
    Uint32 SP_SILENCE_TIMER;
    /** Port multicast-event control symbol request register */
    Uint32 SP_MULT_EVNT_CS;
    /** Port control symbol transmit register */
    Uint32 SP_CS_TX;
} CSL_SrioCfgPortOptionRegs;

/**
 * @brief This structure contains the control parameters of SRIO
 */
typedef struct {
    /** Puts the memories in either in sleep mode or in awake mode, while in 
     *  shutdown 
     */
    Bool swMemSleepOverride;
    
    /** 0 - Normal operation, 1 - Loop back. Transmit data to receive on the 
     *  same port. Packet data is looped back in the digital domain before 
     *  the SerDes macros
     */
    Bool loopback;
    
    /** Controls ability to write any register during initialization. It also
     *  includes read only registers during normal mode of operation, that have 
     *  application defined reset value. 0 - write enabled, 1 - write to read 
     *  only registers disabled. Usually the boot_complete is asserted once 
     *  after reset to define power on configuration
     */
    Bool bootComplete;
    
    /** Sets the required number of logical layer TX buffers needed to send 
     *  priority 2 packets across the UDI interface
     */
    CSL_SrioTxPriorityWm txPriority2Wm;
    
    /** Sets the required number of logical layer TX buffers needed to send 
     *  priority 1 packets across the UDI interface
     */
    CSL_SrioTxPriorityWm txPriority1Wm;
    
    /** Sets the required number of logical layer TX buffers needed to send 
     *  priority 0 packets across the UDI interface  
     */
    CSL_SrioTxPriorityWm txPriority0Wm;
    
    /** Internal BUS transaction priority */
    CSL_SrioBusTransPriority busTransPriority;
    
    /** UDI buffering setup (priority versus port) */
    CSL_SrioBufMode bufferMode;
    
    /** Internal clock frequency pre-scalar, used to drive the request to 
     *  response timers 
     */
    CSL_SrioClkDiv prescalar;
    
    /** SERDES macros PLL enable/disable. Enable/disable macros are
     *  OR’ ed to get the value  
     */
    Uint8 pllEn;
} CSL_SrioControlSetup;


/**
 * @brief This structure contains SRIO vendor related information
 */
typedef struct {
    /** Identifies the vendor specific type of device */
    Uint16 devId; 
    
    /** Device vendor ID assigned by RapidIO TA */
    Uint16 devVendorId;

    /** Vendor supplied device revision */
    Uint32 devRevision; 
} CSL_SrioDevInfo;

/**
 * @brief This structure contains the information about SRIO assembly
 */
typedef struct {
    /** Identifies the vendor specific type of assembly */
    Uint16 assyId;
    
    /** Assembly vendor ID assigned by RapidIO TA */
    Uint16 assyVendorId;

    /** Vendor supplied assembly revision */
    Uint16 assyRevision;
} CSL_SrioAssyInfo;
    
/**
 * @brief This structure contains control symbols used for packet acknowledgment
 */
typedef struct {
    /** Encoding for control symbol that make use of parameters PAR_0 and 
     *  PAR_1  
     */
    Uint8 stype0;
    
    /** Used in conjunction with stype0 encoding  */
    Uint8 par0;
    
    /** Used in conjunction with stype0 encoding  */
    Uint8 par1;
    
    /** Encoding for control symbol that make use of parameter CMD  */
    Uint8 stype1;
    
    /** Used in conjunction with stype1 encoding to define the link maintenance
     *  commands
     */
    Uint8 cmd;
    
    /** When set, force the outbound flow to insert control symbol into packet.
     *  Used in debug mode
     */
    Bool emb;    

	CSL_SrioPortNum portNum;
} CSL_SrioCntlSym;

/**
 * @brief This structure contains captured error information of 
 *        logical/transport layer
 */
typedef struct {
    /** The address associated with the error(only valid for devices supporting
     *  66 and 50 bit addresses)
     */
    Uint32 errAddrHi;
    
    /** The address associated with the error(only valid for devices supporting
     *  66 and 50 bit addresses)  
     */
    Uint32 errAddrLo;
    
    /** Extended address bits of the address associated with the error */
    Uint8 xambs; 
    
    /** The destination ID associated with the error */
    Uint16 destId;
    
    /** The source ID associated with the error */
    Uint16 srcId; 
    
    /** Format type associated with the error */
    Uint8 ftype; 
    
    /** Transaction type associated with the error */
    Uint8 tType;
    
    /** Implementation specific information associated with the error */
    Uint16 impSpecific;  
} CSL_SrioLogTrErrInfo;

/** 
 * @brief This structure is used to hold the configuration/status information
 *        of different SRIO ports
 */
typedef struct {
    /** Port selection */
    Uint32 index;
    
    /** Desired information in the registers */
    CSL_BitMask32 data;
} CSL_SrioPortData;

/**
 * @brief This structure contains information to configure port
 */
typedef struct {
    /** Timeout value for all ports on the device. This timeout is for link
     *  events such as sending a packet to receiving the corresponding ACK
     */
    Uint32 portLinkTimeout;
    
    /** Timeout value for all ports on the device. This timeout is for sending
     *  a packet to receiving the corresponding response packet
     */
    Uint32 portRespTimeout;
    
    /** A Host device enable
     *  0b0 - agent or slave device 
     *  0b1 - host device 
     */
    Bool hostEn;
       
    /** It controls whether or not a device is allowed to issue requests into 
     *  the system. If the Master Enable is not set, the device may only 
     *  respond to requests
     */
    Bool masterEn;
} CSL_SrioPortGenConfig;

/**
 * @brief This structure contains information to configure port parameters
 */ 
typedef struct {
    /** Controls port receivers/drivers to receive/transmit to any packets or 
     * control symbols 
     */
    Bool portDis;
    
    /** Controls output port to issue any packets and control symbols */
    Bool outPortEn;
    
    /** Input port receive enable. Controls input port to respond to any packet */
    Bool inPortEn; 
    
    /** Soft port configuration to override the hardware size */
    CSL_SrioPortWidthOverride portWidthOverride;
    
    /** Disables/Enables all RapidIO transmission error checking */
    Bool errCheckDis;
    
    /** Disables/Enables the multicast event reception on this port */
    Bool multicastRcvEn;
    
    /** Enabling this bit causes the port to stop attempting to send packets
     *  to the connected device when the output failed-encountered bit is set
     */
    Bool stopOnPortFailEn;
        
    /** Enabling this bit causes the port to drop packets that are acknowledged
     *  with a packet-not-accepted control symbol when the error failed 
     *  threshold is exceeded
     */
    Bool dropPktEn;
    
    /** When the bit is set the port is stopped and is not enabled to issue
     *  or receive any packets
     */
    Bool portLockoutEn;
} CSL_SrioPortCntlConfig;

/**
 * @brief This structure contains information to configure port error enable and
 * error rate thresholds
 */
typedef struct {
    /** Enable/disable port error interrupts. Macros can be OR’ed to get the 
     *  value to pass the argument 
     */
    Uint32 portErrRateEn;
    
    /** The error rate bias value */
    CSL_SrioErrRtBias prtErrRtBias;
    
    /** Limit value  to the error rate counter above the failed threshold
     *  trigger 
     */
    CSL_SrioErrRtNum portErrRtRec;
    
    /** The threshold value for reporting an error condition due to a possibly
     *  broken link 
     */
    Uint8 portErrRtFldThresh;
    
    /** The threshold value for reporting an error condition due to a degrading
     *  link 
     */
    Uint8 portErrRtDegrdThresh;
} CSL_SrioPortErrConfig;


/**
 * @brief This structure is used to return the contents of the Peripheral 
 *        Identification register, which has the versioning information, 
 *        used to identify the specific SRIO peripheral
 */
typedef struct {
    /** Identifies the type of peripheral  */
    Uint8 srioType;

    /** Identifies the class of peripheral */
    Uint8 srioClass;
    
    /** Identifies the revision of SRIO   */
    Uint8 srioRevision;
} CSL_SrioPidNumber;


/**
 * @brief This structure contains base device configuration parameters
 */
typedef struct {
    /** This is the base ID of the device in small common transport system 
     *  (End points only) 
     */
    Uint8 smallTrBaseDevId;
    
    /** This is the base ID of the device in a large common transport system 
     *  (Only valid for end points, and if bit 4 of the PEFTR register is set)
     */
    Uint16 largeTrBaseDevId;
    
    /** This is the base ID for the Host PE that is initializing this PE
     *  (processing element)
     */
    Uint16 hostBaseDevId;
} CSL_SrioDevIdConfig;

/**
 * @brief This structure is used to enable/disable the blocks within the SRIO
 *        peripheral
 */
typedef struct {
    /** Enable/disable MMR non-Reset/PD control Registers (Logical Block 0) */
    Bool block0;
    
    /** Enable/disable LSU (Direct I/O Initiator) */
    Bool block1;
    
    /** Enable/disable  MAU (Direct I/O Target) */
    Bool block2;
    
    /** Enable/disable TXU (Message Passing Initiator) */
    Bool block3;
    
    /** Enable/disable RXU (Message Passing Target) */
    Bool block4;
    
    /** Enable/disable Port 0 Data path */
    Bool block5;
    
    /** Enable/disable Port 1 Data path */
    Bool block6;
    
    /** Enable/disable port 2 Data  path */
    Bool block7;
    
    /** Enable/disable Port 3 Data path */
    Bool block8;
} CSL_SrioBlkEn;


/**
 * @brief This structure is used to configure hardware packet forwarding
 */
typedef struct {
    /** Upper 16-bit Device ID boundary. Destination ID above this range cannot
     *  use the table entry
     */
    Uint16 largeUpBoundDevId;
    
    /** Lower 16-bit Device ID boundary. Destination ID lower than this number
     *  cannot use the table entry
     */
    Uint16 largeLowBoundDevId;
    
    /** Output port number for packet’s whose destination  ID falls within the
     *  8b or 16b range for this table entry 
     */
    CSL_SrioPortNum outBoundPort;
    
    /** Upper 8-bit Device ID boundary. Destination ID above this range cannot
     *  use the table entry
     */
    Uint8 smallUpBoundDevId;
    
    /** Lower 8-bit Device ID boundary. Destination ID lower than this number
     *  cannot use the table entry
     */
    Uint8 smallLowBoundDevId; 
} CSL_SrioPktFwdCntl;

/**
 * @brief This structure is used to return the completion status of the LSU command
 */
typedef struct {
    /** Port number */
    CSL_SrioPortNum portNum;
    
    /** This is used to return the LSU command completion code */
    CSL_SrioCompCode lsuCompCode;
} CSL_SrioLsuCompStat;

/**
 * @brief This structure contains local configuration base address
 */ 
typedef struct {
    /** Configuration address high */
    Uint32 addressHi;
    
    /** Configuration address low */
    Uint32 addressLo;
} CSL_SrioLongAddress;

/**
 * @brief This structure is sued to return the error capture information
 *        for the specified port
 */
typedef struct {
    /** Port number for which the error data is to be captured */
    CSL_SrioPortNum portNum;
    
    /** Type of information logged */
    CSL_SrioPortCaptType portErrCaptType;
    
    /** Encoded error type */
    Uint8 errorType;
    
    /** Implementation specific data */
    Uint32 impSpecData;
    
    /** This contains the control symbol information or 0-3 bytes of packet 
     *  header  
     */
    Uint32 capture0;
    
    /** This contains the control symbol information or 4-7 bytes of packet 
     *  header
     */
    Uint32 capture1;
    
    /** This contains the control symbol information or 8-11 bytes of packet 
     *  header
     */
    Uint32 capture2;
    
    /** This contains the control symbol information or 12-15 bytes of packet
     *  header  
     */
    Uint32 capture3;
} CSL_SrioPortErrCapt;
    
/**
 * @brief This structure is used to return the port write capture information
 */
typedef struct {
    /** Port-Write payload, word 0 */
    Uint32 capture0;
    
    /** Port-Write payload, word 1 */
    Uint32 capture1;
    
    /** Port-Write payload, word 2 */
    Uint32 capture2;
    
    /** Port-Write payload, word 3 */
    Uint32 capture3;
} CSL_SrioPortWriteCapt;


/**
 * @brief This structure is used to configure LSU module for Transfer enable
 */
typedef struct {
    /** Source node address */
    Uint32 srcNodeAddr;
    
    /** Destination node address */
    CSL_SrioLongAddress dstNodeAddr;
    
    /** Number of data bytes to Read/Write - up to 4KB. (Used in conjunction
     *  with RapidIO address to create WRSIZE/RDSIZE and WDPTR in RapidIO
     *  packet header)
     */
    Uint16 byteCnt; 
        
    /** RapidIO tt field specifying 8 or 16bit Device IDs */
    Uint8 idSize;
    
    /** This field specifies packet priority */
    Uint8 priority;
    
    /** RapidIO xambs field specifying extended address MSB */
    Uint8 xambs;
    
    /** RapidIO destination ID field specifying target device */
    Uint16 dstId;

    /** RapidIO Lsu module interrupt request */
	Bool intrReq;

    /** packet type */
	Uint8 pktType;

    /** RapidIO hop count */
	Uint8 hopCount;

    /** doorbell info */
	Uint16 doorbellInfo;
    
    /** out port ID */
	Uint8 outPortId;
} CSL_SrioDirectIO_ConfigXfr;

/** @brief Structure to configure SERDES PLL */
typedef struct {
    /** Enables the internal PLL of the SERDES */
    Bool                        pllEnable;

    /** PLL multiplication factor */
    CSL_SrioSerDesPllMply       pllMplyFactor;

    /** Loop bandwidth */
    CSL_SrioSerDesLoopBandwidth loopBandwidth;
} CSL_SrioSerDesPllCfg;

/** @brief Structure to configure the SERDES receiver */
typedef struct {
    /** Enable receiver */
    Bool                        enRx;

    /** Bus width */
    CSL_SrioSerDesBusWidth      busWidth;

    /** Operating rate */
    CSL_SrioSerDesRate          rate;

    /** Inverted polarity */
    Bool                        invertedPolarity;

    /** Selects input termination options suitable for a variety of 
      * AC or DC coupled scenarios.
      */
    CSL_SrioSerDesTermination   termination;

    /** Enables internal or external symbol alignment. */
    CSL_SrioSerDesSymAlignment  symAlign;

    /** Loss of signal detection, with selectable thresholds */
    CSL_SrioSerDesLos           los;

    /** Clock/data recovery configuration */
    Uint8                       clockDataRecovery;

    /** Configure the adaptive equalizer */
    Uint8                       equalizer;
} CSL_SrioSerDesRxCfg;

/** @brief Structure to configure the SERDES transmitter */
typedef struct {
    /** Enable transmitter */
    Bool enTx;

    /** Bus width */
    CSL_SrioSerDesBusWidth  busWidth;

    /** Operating rate */
    CSL_SrioSerDesRate      rate;

    /** Inverted polarity */
    Bool                    invertedPolarity;

    /** Common mode configuration */
    CSL_SrioSerDesCommonMode      commonMode;

    /** Output swing configuration  */
    CSL_SrioSerDesSwingCfg        outputSwing;

    /** Output de-emphasis select */
    Uint8                         outputDeEmphasis;

    /** Enable fixed TXBCLKIN[i] phase with TXBCLK [i] */
    Bool                    enableFixedPhase;
} CSL_SrioSerDesTxCfg;

/**
 * @brief Hardware setup structure.
 */
typedef struct {
    /** Peripheral enable. Controls the flow of data in the logical layer of 
     *  the peripheral 
     */
    Bool perEn;

    /** This is used to hold the information for local SRIO’s control setup */
    CSL_SrioControlSetup periCntlSetup;
    
    /** Controls reset to all clock domains within the peripheral */
    Bool gblEn;

    /** Controls reset to Logical block n */
    Uint32 blkEn[9];

    /** This value is equal to the value of the RapidIO Base Device ID CSR.
     *  The CPU must read the CSR value and set this register, so that out-going
     *  packets contain the correct SOURCEID value. This field contains both 
     *  16bit and 8bit IDs  
     */
    Uint32 deviceId1;

    /** This is a secondary supported DeviceID checked against an in-coming 
     *  packet’s DestID field.  Typically used for Multi-cast support. This 
     *  field contains both 16bit and 8bit IDs
     */
    Uint32 deviceId2;

    /** Sets the boundaries for device IDs that are part of the chain and the 
     *  packet can be forwarded to
     */
    CSL_SrioPktFwdCntl pktFwdCntl[4];

    /** SERDES PLL configure */ 
    CSL_SrioSerDesPllCfg serDesPllCfg[4];
    
    /** SERDES RX channel configure */
    CSL_SrioSerDesRxCfg serDesRxChannelCfg[4];

    /** SERDES TX channel configure */
    CSL_SrioSerDesTxCfg serDesTxChannelCfg[4];

    /** Selects flow control ID length */
    Uint8 flowCntlIdLen[16];

    /** Destination ID of flow n */
    Uint16 flowCntlId[16];

    /** Sets the number of address bits generated by the PE as a source and 
     *  processed by the PE as the target of an operation
     */
    CSL_SrioAddrSelect peLlAddrCtrl;

    /** Base device configuration */
    CSL_SrioDevIdConfig devIdSetup ;

    /** Software defined component Tag for PE (processing element). Useful for 
     *  devices without device IDs 
     */
    Uint32 componentTag;

    /** Port General configuration */
    CSL_SrioPortGenConfig  portGenSetup ;

    /** Port control configuration */
    CSL_SrioPortCntlConfig  portCntlSetup[4];

    /** Enable/disable logical/transport layer errors. Macros can be OR’ed 
     *  to get the value to pass the argument */
    Uint32 lgclTransErrEn;

    /** Port error configuration */
    CSL_SrioPortErrConfig portErrSetup[4];

    /** Discovery Timer in 4x mode. The discovery-timer allows time for the 
     *  link partner to enter its DISCOVERY state and if the link partner is
     *  supporting 4x mode, for all 4 lanes to be aligned
     */
    CSL_SrioDiscoveryTimer discoveryTimer;

    /** This configures the SP_IP_MODE register */
    Uint32 portIpModeSet;

    /** This configures the SP_IP_PRESCALE register */
    Uint32 portIpPrescalar;

    /** Port-Write Timer. The timer defines a period to repeat sending an error
     *  reporting Port-Write request for software assistance. The timer stopped 
     *  by software writing to the error detect registers
     */
    CSL_SrioPwTimer pwTimer;

    /** Silence timer. Defines the time of the port in the SILENT state */
    CSL_SrioSilenceTimer silenceTimer[4];

    /** Port control independent error reporting enable. Macros can be OR’ed
     *  to get the value
     */
    Uint32 portCntlIndpEn[4];
} CSL_SrioHwSetup;

/**
 *  @brief Config-structure Used to configure the SRIO using CSL_srioHwSetupRaw()
 */
typedef struct {
    /** Peripheral control register */
    Uint32 PCR;
    
    /** Peripheral settings control register */
    Uint32 PER_SET_CNTL;
    
    /** Peripheral global enable register */
    Uint32 GBL_EN;
    
    /** Block enable registers */
    Uint32 BLK_EN[9];
    
    /** Device ID register 1 */
    Uint32 DEVICEID_REG1;
    
    /** Device ID register 2 */
    Uint32 DEVICEID_REG2;
    
    /** Packet forwarding registers for 16-bit and 8bbit device IDs */
    CSL_SrioHw_pkt_fwdRegs HW_PKT_FWD[4];
    
    /** SerDes RX channels configuration control registers */
    Uint32 SERDES_CFGRX_CNTL[4];
    
    /** SerDes TX channels configuration control registers */
    Uint32 SERDES_CFGTX_CNTL[4];
    
    /** SerDes macros configuration control registers */
    Uint32 SERDES_CFG_CNTL[4];
    
    /** Doorbell interrupt clear registers */
    Uint32 DOORBELL_ICCR[4];
    
    /** LSU interrupt clear registers */
    Uint32 LSU_ICCR; 
    
    /** Error, Reset, and Special event interrupt clear registers */
    Uint32 ERR_RST_EVNT_ICCR;
    
    /** INTDST interrupt rate control register for DST 0 */
    Uint32 INTDST_RATE_CNTL;
    
    /** LSU registers */
    CSL_SrioCfgLsuRegs LSU[4];
    
    /** Flow control table entry registers */
    Uint32 FLOW_CNTL[16];
    
    /** Processing element logical layer control CSR register */
    Uint32 PE_LL_CTL;
    
    /** Base device ID CSR register */
    Uint32 BASE_ID;
    
    /** Host base device ID lock CSR */
    Uint32 HOST_BASE_ID_LOCK;
    
    /** Component tag CSR */
    Uint32 COMP_TAG;
    
    /** Port link time-out control CSR */
    Uint32 SP_LT_CTL;
    
    /** Port link response time-out control CSR */
    Uint32 SP_RT_CTL;
    
    /** Port general control CSR */
    Uint32 SP_GEN_CTL;
    
    /** Port registers */
    CSL_SrioCfgPortRegs PORT[4];
    
    /** Logical/Transport layer error detect CSR */
    Uint32 ERR_DET;
    
    /** Logical/Transport layer error enable CSR */
    Uint32 ERR_EN;
    
    /** Port-write target device ID CSR */
    Uint32 PW_TGT_ID;
    
    /** Port error CSR */
    CSL_SrioCfgPortErrorRegs PORT_ERROR[4];
    
    /** Port IP discovery timer in 4x mode */
    Uint32 SP_IP_DISCOVERY_TIMER;
    
    /** Port IP mode CSR */
    Uint32 SP_IP_MODE;
    
    /** Serial port IP prescalar */
    Uint32 IP_PRESCALAR;
    
    /** Port options CSR */
    CSL_SrioCfgPortOptionRegs PORT_OPTION[4];
} CSL_SrioConfig;

/**
 * @brief Module specific context information. Present implementation of SRIO
 *        CSL doesn't have any context information.
 */
typedef struct {
    /** Context information of SRIO CSL.
     *  The below declaration is just a place-holder for future implementation.
     */
    Uint16 contextInfo;
} CSL_SrioContext;

/** @brief Module specific parameters. Present implementation of SRIO CSL
 *         doesn't have any module specific parameters.
 */
typedef struct {
    /** Bit mask to be used for module specific parameters. The below
     *  declaration is just a place-holder for future implementation.
     */
    CSL_BitMask16 flags;
} CSL_SrioParam;

/** @brief This structure contains the base-address information for the
 *         peripheral instance
 */
typedef struct {
    /** Base-address of the configuration registers of the peripheral */
    CSL_SrioRegsOvly regs;
} CSL_SrioBaseAddress;

/**
 * @brief serial rapid IO object structure.
 */
typedef struct {
    /** Pointer to the register overlay structure of the SRIO */
    CSL_SrioRegsOvly regs;

    /** Instance of SRIO being referred by this object  */
    CSL_InstNum perNum;
} CSL_SrioObj;

/** Default hardware setup parameters */
#define CSL_SRIO_HWSETUP_DEFAULTS   { \
    CSL_SRIO_PCR_PEREN_RESETVAL, \
    {\
        CSL_SRIO_PER_SET_CNTL_SW_MEM_SLEEP_OVERRIDE_RESETVAL, \
        CSL_SRIO_PER_SET_CNTL_LOOPBACK_RESETVAL, \
        CSL_SRIO_PER_SET_CNTL_BOOT_COMPLETE_RESETVAL, \
        CSL_SRIO_TX_PRIORITY_WM_3, \
        CSL_SRIO_TX_PRIORITY_WM_2, \
        CSL_SRIO_TX_PRIORITY_WM_1, \
        CSL_SRIO_BUS_TRANS_PRIORITY_0, \
        CSL_SRIO_1X_MODE_PRIORITY, \
        CSL_SRIO_CLK_PRESCALE_0, \
        0x0 \
    }, \
    CSL_SRIO_GBL_EN_EN_RESETVAL, \
    {\
        0x0, \
        0x0, \
        0x0, \
        0x0, \
        0x0, \
        0x0, \
        0x0, \
        0x0, \
        0x0 \
    }, \
	CSL_SRIO_DEVICEID_REG1_RESETVAL, \
	CSL_SRIO_DEVICEID_REG2_RESETVAL, \
    { \
        {0x0000FFFF, 0x0000FFFF, CSL_SRIO_PORT_3, 0x000000FF, 0x000000FF}, \
        {0x0000FFFF, 0x0000FFFF, CSL_SRIO_PORT_3, 0x000000FF, 0x000000FF}, \
        {0x0000FFFF, 0x0000FFFF, CSL_SRIO_PORT_3, 0x000000FF, 0x000000FF}, \
        {0x0000FFFF, 0x0000FFFF, CSL_SRIO_PORT_3, 0x000000FF, 0x000000FF} \
    }, \
    { \
    	{ \
    	FALSE, \
    	CSL_SRIO_SERDES_PLL_MPLY_BY_4, \
    	CSL_SRIO_SERDES_LOOP_BANDWIDTH_FREQ_DEP \
		}, \
    	{ \
    	FALSE, \
    	CSL_SRIO_SERDES_PLL_MPLY_BY_4, \
    	CSL_SRIO_SERDES_LOOP_BANDWIDTH_FREQ_DEP \
		}, \
    	{ \
    	FALSE, \
    	CSL_SRIO_SERDES_PLL_MPLY_BY_4, \
    	CSL_SRIO_SERDES_LOOP_BANDWIDTH_FREQ_DEP \
		}, \
    	{ \
    	FALSE, \
    	CSL_SRIO_SERDES_PLL_MPLY_BY_4, \
    	CSL_SRIO_SERDES_LOOP_BANDWIDTH_FREQ_DEP \
		} \
	}, \
    { \
    	{ \
		FALSE, \
		CSL_SRIO_SERDES_BUS_WIDTH_10_BIT, \
		CSL_SRIO_SERDES_RATE_FULL, \
		FALSE, \
		CSL_SRIO_SERDES_TERMINATION_VDDT, \
		CSL_SRIO_SERDES_SYM_ALIGN_DISABLE, \
		CSL_SRIO_SERDES_LOS_DET_DISABLE, \
		0x0, \
		0x0 \
		}, \
    	{ \
		FALSE, \
		CSL_SRIO_SERDES_BUS_WIDTH_10_BIT, \
		CSL_SRIO_SERDES_RATE_FULL, \
		FALSE, \
		CSL_SRIO_SERDES_TERMINATION_VDDT, \
		CSL_SRIO_SERDES_SYM_ALIGN_DISABLE, \
		CSL_SRIO_SERDES_LOS_DET_DISABLE, \
		0x0, \
		0x0 \
		}, \
    	{ \
		FALSE, \
		CSL_SRIO_SERDES_BUS_WIDTH_10_BIT, \
		CSL_SRIO_SERDES_RATE_FULL, \
		FALSE, \
		CSL_SRIO_SERDES_TERMINATION_VDDT, \
		CSL_SRIO_SERDES_SYM_ALIGN_DISABLE, \
		CSL_SRIO_SERDES_LOS_DET_DISABLE, \
		0x0, \
		0x0 \
		}, \
    	{ \
		FALSE, \
		CSL_SRIO_SERDES_BUS_WIDTH_10_BIT, \
		CSL_SRIO_SERDES_RATE_FULL, \
		FALSE, \
		CSL_SRIO_SERDES_TERMINATION_VDDT, \
		CSL_SRIO_SERDES_SYM_ALIGN_DISABLE, \
		CSL_SRIO_SERDES_LOS_DET_DISABLE, \
		0x0, \
		0x0 \
		} \
	}, \
    { \
    	{ \
		FALSE, \
		CSL_SRIO_SERDES_BUS_WIDTH_10_BIT, \
		CSL_SRIO_SERDES_RATE_FULL, \
		FALSE, \
		CSL_SRIO_SERDES_COMMON_MODE_NORMAL, \
		CSL_SRIO_SERDES_SWING_AMPLITUDE_125, \
		0x0, \
		FALSE \
		}, \
    	{ \
		FALSE, \
		CSL_SRIO_SERDES_BUS_WIDTH_10_BIT, \
		CSL_SRIO_SERDES_RATE_FULL, \
		FALSE, \
		CSL_SRIO_SERDES_COMMON_MODE_NORMAL, \
		CSL_SRIO_SERDES_SWING_AMPLITUDE_125, \
		0x0, \
		FALSE \
		}, \
    	{ \
		FALSE, \
		CSL_SRIO_SERDES_BUS_WIDTH_10_BIT, \
		CSL_SRIO_SERDES_RATE_FULL, \
		FALSE, \
		CSL_SRIO_SERDES_COMMON_MODE_NORMAL, \
		CSL_SRIO_SERDES_SWING_AMPLITUDE_125, \
		0x0, \
		FALSE \
		}, \
    	{ \
		FALSE, \
		CSL_SRIO_SERDES_BUS_WIDTH_10_BIT, \
		CSL_SRIO_SERDES_RATE_FULL, \
		FALSE, \
		CSL_SRIO_SERDES_COMMON_MODE_NORMAL, \
		CSL_SRIO_SERDES_SWING_AMPLITUDE_125, \
		0x0, \
		FALSE \
		} \
    }, \
    {0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, \
                                                              0x1, 0x1 }, \
    {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, \
                  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 }, \
    (CSL_SrioAddrSelect)CSL_SRIO_PE_LL_CTL_EXTENDED_ADDRESSING_CONTROL_RESETVAL, \
    {\
        CSL_SRIO_BASE_ID_BASE_DEVICEID_RESETVAL, \
        CSL_SRIO_BASE_ID_LARGE_BASE_DEVICEID_RESETVAL, \
        CSL_SRIO_HOST_BASE_ID_LOCK_HOST_BASE_DEVICEID_RESETVAL \
    }, \
    CSL_SRIO_COMP_TAG_COMPONENT_TAG_RESETVAL, \
    {\
        CSL_SRIO_SP_LT_CTL_TIMEOUT_VALUE_RESETVAL, \
        CSL_SRIO_SP_RT_CTL_TIMEOUT_VALUE_RESETVAL, \
        0x0, \
        0x0 \
    }, \
    { \
        {\
            FALSE, \
            FALSE, \
            FALSE, \
            (CSL_SrioPortWidthOverride)CSL_SRIO_SP_CTL_PORT_WIDTH_OVERRIDE_RESETVAL, \
            FALSE, \
            FALSE, \
            FALSE, \
            FALSE, \
            FALSE \
        },\
        {\
            FALSE, \
            FALSE, \
            FALSE, \
            (CSL_SrioPortWidthOverride)CSL_SRIO_SP_CTL_PORT_WIDTH_OVERRIDE_RESETVAL, \
            FALSE, \
            FALSE, \
            FALSE, \
            FALSE, \
            FALSE \
        }, \
        {\
            FALSE, \
            FALSE, \
            FALSE, \
            (CSL_SrioPortWidthOverride)CSL_SRIO_SP_CTL_PORT_WIDTH_OVERRIDE_RESETVAL, \
            FALSE, \
            FALSE, \
            FALSE, \
            FALSE, \
            FALSE \
        }, \
        {\
            FALSE, \
            FALSE, \
            FALSE, \
            (CSL_SrioPortWidthOverride)CSL_SRIO_SP_CTL_PORT_WIDTH_OVERRIDE_RESETVAL, \
            FALSE, \
            FALSE, \
            FALSE, \
            FALSE, \
            FALSE \
        } \
    }, \
    CSL_SRIO_ERR_EN_RESETVAL, \
    {\
        {\
            CSL_SRIO_SP_RATE_EN_RESETVAL, \
            (CSL_SrioErrRtBias)CSL_SRIO_SP_ERR_RATE_ERROR_RATE_BIAS_RESETVAL, \
            (CSL_SrioErrRtNum)CSL_SRIO_SP_ERR_RATE_ERROR_RATE_RECOVERY_RESETVAL, \
            CSL_SRIO_SP_ERR_THRESH_ERROR_RATE_FAILED_THRESHOLD_RESETVAL, \
            CSL_SRIO_SP_ERR_THRESH_ERROR_RATE_DEGRADED_THRES_RESETVAL \
        }, \
        {\
            CSL_SRIO_SP_RATE_EN_RESETVAL, \
            (CSL_SrioErrRtBias)CSL_SRIO_SP_ERR_RATE_ERROR_RATE_BIAS_RESETVAL, \
            (CSL_SrioErrRtNum)CSL_SRIO_SP_ERR_RATE_ERROR_RATE_RECOVERY_RESETVAL, \
            CSL_SRIO_SP_ERR_THRESH_ERROR_RATE_FAILED_THRESHOLD_RESETVAL, \
            CSL_SRIO_SP_ERR_THRESH_ERROR_RATE_DEGRADED_THRES_RESETVAL \
        }, \
        {\
            CSL_SRIO_SP_RATE_EN_RESETVAL, \
            (CSL_SrioErrRtBias)CSL_SRIO_SP_ERR_RATE_ERROR_RATE_BIAS_RESETVAL, \
            (CSL_SrioErrRtNum)CSL_SRIO_SP_ERR_RATE_ERROR_RATE_RECOVERY_RESETVAL, \
            CSL_SRIO_SP_ERR_THRESH_ERROR_RATE_FAILED_THRESHOLD_RESETVAL, \
            CSL_SRIO_SP_ERR_THRESH_ERROR_RATE_DEGRADED_THRES_RESETVAL \
        }, \
        {\
            CSL_SRIO_SP_RATE_EN_RESETVAL, \
            (CSL_SrioErrRtBias)CSL_SRIO_SP_ERR_RATE_ERROR_RATE_BIAS_RESETVAL, \
            (CSL_SrioErrRtNum)CSL_SRIO_SP_ERR_RATE_ERROR_RATE_RECOVERY_RESETVAL, \
            CSL_SRIO_SP_ERR_THRESH_ERROR_RATE_FAILED_THRESHOLD_RESETVAL, \
            CSL_SRIO_SP_ERR_THRESH_ERROR_RATE_DEGRADED_THRES_RESETVAL \
        } \
    }, \
    (CSL_SrioDiscoveryTimer)CSL_SRIO_SP_IP_DISCOVERY_TIMER_DISCOVERY_TIMER_RESETVAL, \
    CSL_SRIO_SP_IP_MODE_RESETVAL, \
    CSL_SRIO_IP_PRESCAL_RESETVAL, \
    (CSL_SrioPwTimer)CSL_SRIO_SP_IP_DISCOVERY_TIMER_PW_TIMER_RESETVAL, \
    { \
        (CSL_SrioSilenceTimer)CSL_SRIO_SP_SILENCE_TIMER_SILENCE_TIMER_RESETVAL, \
        (CSL_SrioSilenceTimer)CSL_SRIO_SP_SILENCE_TIMER_SILENCE_TIMER_RESETVAL, \
        (CSL_SrioSilenceTimer)CSL_SRIO_SP_SILENCE_TIMER_SILENCE_TIMER_RESETVAL, \
        (CSL_SrioSilenceTimer)CSL_SRIO_SP_SILENCE_TIMER_SILENCE_TIMER_RESETVAL \
    }, \
    { \
        CSL_SRIO_SP_CTL_INDEP_RESETVAL, \
        CSL_SRIO_SP_CTL_INDEP_RESETVAL, \
        CSL_SRIO_SP_CTL_INDEP_RESETVAL, \
        CSL_SRIO_SP_CTL_INDEP_RESETVAL  \
	}\
}



/** Default values for config structure */
#define CSL_SRIO_CONFIG_DEFAULTS { \
    CSL_SRIO_PCR_RESETVAL, \
    CSL_SRIO_PER_SET_CNTL_RESETVAL, \
    CSL_SRIO_GBL_EN_RESETVAL, \
    {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}, \
    CSL_SRIO_DEVICEID_REG1_RESETVAL, \
    CSL_SRIO_DEVICEID_REG2_16BNODEID_RESETVAL, \
    {\
        {0xFFFFFFFF, 0x0003FFFF},\
        {0xFFFFFFFF, 0x0003FFFF}, \
        {0xFFFFFFFF, 0x0003FFFF}, \
        {0xFFFFFFFF, 0x0003FFFF} \
    }, \
    {0x00000000, 0x00000000, 0x00000000, 0x00000000}, \
    {0x00000000, 0x00000000, 0x00000000, 0x00000000}, \
    {0x00000000, 0x00000000, 0x00000000, 0x00000000}, \
    {0x00000000, 0x00000000, 0x00000000, 0x00000000}, \
    CSL_SRIO_LSU_ICCR_RESETVAL, \
    CSL_SRIO_ERR_RST_EVNT_ICCR_RESETVAL, \
    CSL_SRIO_INTDST_RATE_CNTL_RESETVAL, \
    {\
        {\
            CSL_SRIO_LSU_REG0_RESETVAL, \
            CSL_SRIO_LSU_REG1_RESETVAL, \
            CSL_SRIO_LSU_REG2_RESETVAL, \
            CSL_SRIO_LSU_REG3_RESETVAL, \
            CSL_SRIO_LSU_REG4_RESETVAL, \
            CSL_SRIO_LSU_FLOW_MASKS_RESETVAL \
        }, \
        {\
            CSL_SRIO_LSU_REG0_RESETVAL, \
            CSL_SRIO_LSU_REG1_RESETVAL, \
            CSL_SRIO_LSU_REG2_RESETVAL, \
            CSL_SRIO_LSU_REG3_RESETVAL, \
            CSL_SRIO_LSU_REG4_RESETVAL, \
            CSL_SRIO_LSU_FLOW_MASKS_RESETVAL \
        }, \
        {\
            CSL_SRIO_LSU_REG0_RESETVAL, \
            CSL_SRIO_LSU_REG1_RESETVAL, \
            CSL_SRIO_LSU_REG2_RESETVAL, \
            CSL_SRIO_LSU_REG3_RESETVAL, \
            CSL_SRIO_LSU_REG4_RESETVAL, \
            CSL_SRIO_LSU_FLOW_MASKS_RESETVAL \
        }, \
        {\
            CSL_SRIO_LSU_REG0_RESETVAL, \
            CSL_SRIO_LSU_REG1_RESETVAL, \
            CSL_SRIO_LSU_REG2_RESETVAL, \
            CSL_SRIO_LSU_REG3_RESETVAL, \
            CSL_SRIO_LSU_REG4_RESETVAL, \
            CSL_SRIO_LSU_FLOW_MASKS_RESETVAL \
        } \
    }, \
    {\
        CSL_SRIO_FLOW_CNTL_RESETVAL, \
        CSL_SRIO_FLOW_CNTL_RESETVAL, \
        CSL_SRIO_FLOW_CNTL_RESETVAL, \
        CSL_SRIO_FLOW_CNTL_RESETVAL, \
        CSL_SRIO_FLOW_CNTL_RESETVAL, \
        CSL_SRIO_FLOW_CNTL_RESETVAL, \
        CSL_SRIO_FLOW_CNTL_RESETVAL, \
        CSL_SRIO_FLOW_CNTL_RESETVAL, \
        CSL_SRIO_FLOW_CNTL_RESETVAL, \
        CSL_SRIO_FLOW_CNTL_RESETVAL, \
        CSL_SRIO_FLOW_CNTL_RESETVAL, \
        CSL_SRIO_FLOW_CNTL_RESETVAL, \
        CSL_SRIO_FLOW_CNTL_RESETVAL, \
        CSL_SRIO_FLOW_CNTL_RESETVAL, \
        CSL_SRIO_FLOW_CNTL_RESETVAL, \
        CSL_SRIO_FLOW_CNTL_RESETVAL \
    }, \
    CSL_SRIO_PE_LL_CTL_RESETVAL, \
    CSL_SRIO_BASE_ID_RESETVAL, \
    CSL_SRIO_HOST_BASE_ID_LOCK_RESETVAL, \
    CSL_SRIO_COMP_TAG_RESETVAL, \
    CSL_SRIO_SP_LT_CTL_RESETVAL, \
    CSL_SRIO_SP_RT_CTL_RESETVAL, \
    CSL_SRIO_SP_GEN_CTL_RESETVAL, \
    {\
        {\
            CSL_SRIO_SP_LM_REQ_RESETVAL, \
            CSL_SRIO_SP_ACKID_STAT_RESETVAL, \
            CSL_SRIO_SP_ERR_STAT_RESETVAL, \
            CSL_SRIO_SP_CTL_RESETVAL \
        }, \
        {\
            CSL_SRIO_SP_LM_REQ_RESETVAL, \
            CSL_SRIO_SP_ACKID_STAT_RESETVAL, \
            CSL_SRIO_SP_ERR_STAT_RESETVAL, \
            CSL_SRIO_SP_CTL_RESETVAL \
        }, \
        {\
            CSL_SRIO_SP_LM_REQ_RESETVAL, \
            CSL_SRIO_SP_ACKID_STAT_RESETVAL, \
            CSL_SRIO_SP_ERR_STAT_RESETVAL, \
            CSL_SRIO_SP_CTL_RESETVAL \
        }, \
        {\
            CSL_SRIO_SP_LM_REQ_RESETVAL, \
            CSL_SRIO_SP_ACKID_STAT_RESETVAL, \
            CSL_SRIO_SP_ERR_STAT_RESETVAL, \
            CSL_SRIO_SP_CTL_RESETVAL \
        } \
    }, \
    CSL_SRIO_ERR_DET_RESETVAL, \
    CSL_SRIO_ERR_EN_RESETVAL, \
    CSL_SRIO_PW_TGT_ID_RESETVAL, \
    {\
        {\
            CSL_SRIO_SP_ERR_DET_RESETVAL, \
            CSL_SRIO_SP_RATE_EN_RESETVAL, \
            CSL_SRIO_SP_ERR_RATE_RESETVAL, \
            CSL_SRIO_SP_ERR_THRESH_RESETVAL \
        }, \
        {\
            CSL_SRIO_SP_ERR_DET_RESETVAL, \
            CSL_SRIO_SP_RATE_EN_RESETVAL, \
            CSL_SRIO_SP_ERR_RATE_RESETVAL, \
            CSL_SRIO_SP_ERR_THRESH_RESETVAL \
        }, \
        {\
            CSL_SRIO_SP_ERR_DET_RESETVAL, \
            CSL_SRIO_SP_RATE_EN_RESETVAL, \
            CSL_SRIO_SP_ERR_RATE_RESETVAL, \
            CSL_SRIO_SP_ERR_THRESH_RESETVAL \
        }, \
        {\
            CSL_SRIO_SP_ERR_DET_RESETVAL, \
            CSL_SRIO_SP_RATE_EN_RESETVAL, \
            CSL_SRIO_SP_ERR_RATE_RESETVAL, \
            CSL_SRIO_SP_ERR_THRESH_RESETVAL \
        } \
    }, \
    CSL_SRIO_SP_IP_DISCOVERY_TIMER_RESETVAL, \
    CSL_SRIO_SP_IP_MODE_RESETVAL, \
    CSL_SRIO_IP_PRESCAL_RESETVAL, \
    {\
        {\
            CSL_SRIO_SP_RST_OPT_RESETVAL, \
            CSL_SRIO_SP_CTL_INDEP_RESETVAL, \
            CSL_SRIO_SP_SILENCE_TIMER_RESETVAL, \
            CSL_SRIO_SP_MULT_EVNT_CS_RESETVAL, \
            CSL_SRIO_SP_CS_TX_RESETVAL \
        }, \
        {\
            CSL_SRIO_SP_RST_OPT_RESETVAL, \
            CSL_SRIO_SP_CTL_INDEP_RESETVAL, \
            CSL_SRIO_SP_SILENCE_TIMER_RESETVAL, \
            CSL_SRIO_SP_MULT_EVNT_CS_RESETVAL, \
            CSL_SRIO_SP_CS_TX_RESETVAL \
        }, \
        {\
            CSL_SRIO_SP_RST_OPT_RESETVAL, \
            CSL_SRIO_SP_CTL_INDEP_RESETVAL, \
            CSL_SRIO_SP_SILENCE_TIMER_RESETVAL, \
            CSL_SRIO_SP_MULT_EVNT_CS_RESETVAL, \
            CSL_SRIO_SP_CS_TX_RESETVAL \
        }, \
        {\
            CSL_SRIO_SP_RST_OPT_RESETVAL, \
            CSL_SRIO_SP_CTL_INDEP_RESETVAL, \
            CSL_SRIO_SP_SILENCE_TIMER_RESETVAL, \
            CSL_SRIO_SP_MULT_EVNT_CS_RESETVAL, \
            CSL_SRIO_SP_CS_TX_RESETVAL \
        } \
    }\
}


/** PLL enable macros */
#define	CSL_SRIO_PLL1_ENABLE (0x00000001) 
#define	CSL_SRIO_PLL2_ENABLE (0x00000002) 
#define	CSL_SRIO_PLL3_ENABLE (0x00000004) 
#define	CSL_SRIO_PLL4_ENABLE (0x00000008) 


/** Doorbell interrupts clear macros */
#define	CSL_SRIO_DOORBELL_INTR0  (0x00000001)
#define	CSL_SRIO_DOORBELL_INTR1  (0x00000002)
#define	CSL_SRIO_DOORBELL_INTR2  (0x00000004)
#define	CSL_SRIO_DOORBELL_INTR3  (0x00000008)
#define	CSL_SRIO_DOORBELL_INTR4  (0x00000010)
#define	CSL_SRIO_DOORBELL_INTR5  (0x00000020)
#define	CSL_SRIO_DOORBELL_INTR6	 (0x00000040)
#define	CSL_SRIO_DOORBELL_INTR7	 (0x00000080)
#define	CSL_SRIO_DOORBELL_INTR8  (0x00000100)
#define	CSL_SRIO_DOORBELL_INTR9  (0x00000200)
#define	CSL_SRIO_DOORBELL_INTR10 (0x00000400)
#define	CSL_SRIO_DOORBELL_INTR11 (0x00000800)
#define	CSL_SRIO_DOORBELL_INTR12 (0x00001000)
#define	CSL_SRIO_DOORBELL_INTR13 (0x00002000)
#define	CSL_SRIO_DOORBELL_INTR14 (0x00004000)
#define	CSL_SRIO_DOORBELL_INTR15 (0x00008000)


/** LSU interrupts clear macros */
#define	CSL_SRIO_LSU_INTR0  (0x00000001)
#define	CSL_SRIO_LSU_INTR1  (0x00000002)
#define	CSL_SRIO_LSU_INTR2  (0x00000004)
#define	CSL_SRIO_LSU_INTR3  (0x00000008)
#define	CSL_SRIO_LSU_INTR4  (0x00000010)
#define	CSL_SRIO_LSU_INTR5  (0x00000020)
#define	CSL_SRIO_LSU_INTR6  (0x00000040)
#define	CSL_SRIO_LSU_INTR7  (0x00000080)
#define	CSL_SRIO_LSU_INTR8  (0x00000100)
#define	CSL_SRIO_LSU_INTR9  (0x00000200)
#define	CSL_SRIO_LSU_INTR10 (0x00000400)
#define	CSL_SRIO_LSU_INTR11 (0x00000800)
#define	CSL_SRIO_LSU_INTR12	(0x00001000)
#define	CSL_SRIO_LSU_INTR13 (0x00002000)
#define	CSL_SRIO_LSU_INTR14 (0x00004000)
#define	CSL_SRIO_LSU_INTR15 (0x00008000)
#define	CSL_SRIO_LSU_INTR16 (0x00010000)
#define	CSL_SRIO_LSU_INTR17	(0x00020000)
#define	CSL_SRIO_LSU_INTR18	(0x00040000)
#define	CSL_SRIO_LSU_INTR19	(0x00080000)
#define	CSL_SRIO_LSU_INTR20	(0x00100000)
#define	CSL_SRIO_LSU_INTR21	(0x00200000)
#define	CSL_SRIO_LSU_INTR22	(0x00400000)
#define	CSL_SRIO_LSU_INTR23	(0x00800000)
#define	CSL_SRIO_LSU_INTR24	(0x01000000)
#define	CSL_SRIO_LSU_INTR25	(0x02000000)
#define	CSL_SRIO_LSU_INTR26	(0x04000000)
#define	CSL_SRIO_LSU_INTR27	(0x08000000)
#define	CSL_SRIO_LSU_INTR28 (0x10000000)
#define	CSL_SRIO_LSU_INTR29 (0x20000000)
#define	CSL_SRIO_LSU_INTR30 (0x40000000)
#define	CSL_SRIO_LSU_INTR31 (0x80000000)


/** Error, Reset, and Special Event Status Interrupt clear macros */
#define	CSL_SRIO_ERR_DEV_RST_INTR   (0x00010000)
#define	CSL_SRIO_ERR_PORT3_INTR     (0x00000800)
#define	CSL_SRIO_ERR_PORT2_INTR	    (0x00000400)
#define	CSL_SRIO_ERR_PORT1_INTR     (0x00000200)
#define	CSL_SRIO_ERR_PORT0_INTR     (0x00000100)
#define	CSL_SRIO_ERR_LGCL_INTR      (0x00000004)
#define	CSL_SRIO_ERR_PW_INTR        (0x00000002)
#define	CSL_SRIO_ERR_MULTICAST_INTR	(0x00000001)


/** Port error Status clear macros */
#define	CSL_SRIO_ERR_OUTPUT_PKT_DROP  (0x04000000)
#define	CSL_SRIO_ERR_OUTPUT_FLD_ENC   (0x02000000)
#define	CSL_SRIO_ERR_OUTPUT_DEGRD_ENC (0x01000000)
#define	CSL_SRIO_ERR_OUTPUT_RETRY_ENC (0x00100000)
#define CSL_SRIO_OUTPUT_ERROR_ENC     (0x00020000)
#define CSL_SRIO_INPUT_ERROR_ENC      (0x00000200)
#define	CSL_SRIO_PORT_WRITE_PND       (0x00000010)
#define	CSL_SRIO_PORT_ERROR           (0x00000004)


/** Logical/transport layer error status clear */
#define	CSL_SRIO_IO_ERR_RSPNS      ~(0x80000000)
#define	CSL_SRIO_ILL_TRANS_DECODE  ~(0x08000000)
#define	CSL_SRIO_PKT_RSPNS_TIMEOUT ~(0x01000000)
#define	CSL_SRIO_UNSOLICITED_RSPNS ~(0x00800000)
#define	CSL_SRIO_UNSUPPORTED_TRANS ~(0x00400000)


/** Port error detect clear macros */
#define	CSL_SRIO_ERR_IMP_SPECIFIC          ~(0x80000000)
#define	CSL_SRIO_CORRUPT_CNTL_SYM          ~(0x00400000)
#define	CSL_SRIO_CNTL_SYM_UNEXPECTED_ACKID ~(0x00200000)
#define	CSL_SRIO_RCVD_PKT_NOT_ACCPT        ~(0x00100000)
#define	CSL_SRIO_PKT_UNEXPECTED_ACKID      ~(0x00080000)
#define	CSL_SRIO_RCVD_PKT_WITH_BAD_CRC     ~(0x00040000)
#define	CSL_SRIO_RCVD_PKT_OVER_276B        ~(0x00020000)	
#define	CSL_SRIO_NON_OUTSTANDING_ACKID     ~(0x00000020)
#define	CSL_SRIO_PROTOCOL_ERROR            ~(0x00000010)
#define	CSL_SRIO_UNSOLICITED_ACK_CNTL_SYM  ~(0x00000002)	
#define	CSL_SRIO_LINK_TIMEOUT              ~(0x00000001)

/** Port error detect enable macros */
#define	CSL_SRIO_ERR_IMP_SPECIFIC_ENABLE          (0x80000000)
#define	CSL_SRIO_CORRUPT_CNTL_SYM_ENABLE          (0x00400000)
#define	CSL_SRIO_CNTL_SYM_UNEXPECTED_ACKID_ENABLE (0x00200000)
#define	CSL_SRIO_RCVD_PKT_NOT_ACCPT_ENABLE        (0x00100000)
#define	CSL_SRIO_PKT_UNEXPECTED_ACKID_ENABLE      (0x00080000)
#define	CSL_SRIO_RCVD_PKT_WITH_BAD_CRC_ENABLE     (0x00040000)
#define	CSL_SRIO_RCVD_PKT_OVER_276B_ENABLE        (0x00020000)	
#define	CSL_SRIO_NON_OUTSTANDING_ACKID_ENABLE     (0x00000020)
#define	CSL_SRIO_PROTOCOL_ERROR_ENABLE            (0x00000010)
#define	CSL_SRIO_UNSOLICITED_ACK_CNTL_SYM_ENABLE  (0x00000002)	
#define	CSL_SRIO_LINK_TIMEOUT_ENABLE              (0x00000001)



/** Logical/transport layer error enable */
#define	CSL_SRIO_IO_ERR_RESP_ENABLE          (0x80000000)
#define	CSL_SRIO_ILL_TRANS_DECODE_ENABLE     (0x08000000)
#define	CSL_SRIO_ILL_TRANS_TARGET_ERR_ENABLE (0x04000000)
#define	CSL_SRIO_PKT_RESP_TIMEOUT_ENABLE     (0x01000000)
#define	CSL_SRIO_UNSOLICITED_RESP_ENABLE     (0x00800000)
#define	CSL_SRIO_UNSUPPORTED_TRANS_ENABLE    (0x00400000)




/**
 * @brief This data type is used to return the handle to the CSL of the SRIO
 */
typedef CSL_SrioObj  *CSL_SrioHandle;

/*******************************************************************************
 * SRIO global function declarations
 ******************************************************************************/

/** ============================================================================
 *   @n@b CSL_srioInit
 *
 *   @b Description
 *   @n This is the initialization function for the SRIO CSL.
 *      The function must be called before calling any other API from this CSL.
 *      This function is idem-potent. Currently, the function just return
 *      status CSL_SOK, without doing anything.
 *
 *   @b Arguments
 *   @verbatim
        pContext    Pointer to module-context. As SRIO doesn't
                    have any context based information user is expected to pass
                    NULL.
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                        CSL_SOK - Always returns
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  The CSL for SRIO is initialized
 *
 *   @b Modifies    
 *   @n  None
 *
 *   @b Example
 *   @verbatim
            CSL_srioInit();
     @endverbatim
 * =============================================================================
 */
CSL_Status CSL_srioInit (
    CSL_SrioContext *pContext
);



/** ============================================================================
 *   @n@b CSL_srioOpen
 *
 *   @b Description
 *   @n This function populates the peripheral data object for the SRIO instance
 *      and returns a handle to the instance. This handle is passed to all other
 *      CSL APIs.
 *
 *   @b Arguments
 *   @verbatim
            srioObj          Pointer to SRIO object.

            srioNum          Instance of SRIO CSL to be opened.
                             There is one instance of the SRIO
                             available. So, the value for this parameter will be
                             CSL_SRIO always.
 
            pSrioParam       Module specific parameters.

            status           Status of the function call
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_SrioHandle
 *   @n                         Valid SRIO handle will be returned if
 *                              status value is equal to CSL_SOK.
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n   1.    The status is returned in the status variable. If status
 *              returned is
 *   @li            CSL_SOK             Valid SRIO handle is returned
 *   @li            CSL_ESYS_FAIL       The SRIO instance is invalid
 *
 *        2.    SRIO object structure is populated
 *
 *   @b Modifies
 *   @n    1. The status variable
 *
 *         2. SRIO object structure
 *
 *   @b Example
 *   @verbatim
            CSL_status     status;
            CSL_SrioObj    srioObj;
            CSL_SrioHandle hSrio;
            
            ...   
            
            hSrio = CSL_srioOpen(&srioObj, CSL_SRIO, NULL, &status);
            
            ...
    @endverbatim
 * =============================================================================
 */
CSL_SrioHandle CSL_srioOpen (
    CSL_SrioObj   *srioObj, 
    CSL_InstNum   srioNum,
    CSL_SrioParam *pSrioParam, 
    CSL_Status    *status
);



/** ============================================================================
 *   @n@b CSL_SrioGetBaseAddress
 *
 *   @b Description
 *   @n  This function gets the base address of the given SRIOinstance.
         This function is used for getting the base address of the peripheral
 *       instance. This function will be called inside the CSL_srioOpen()
 *       function call. This function is open for re-implementing if the user
 *       wants to modify the base address of the peripheral object to point to
 *       a different location and there by allow CSL initiated write/reads into
 *       peripheral. MMR's go to an alternate location.
 *   
 *   @b Arguments
 *   @verbatim
            srioNum        Specifies the instance of the SRIO to be opened

            pSrioParam     SRIO module specific parameters

            pBaseAddress   Pointer to base address structure containing base
                           address details
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li					CSL_OK             Open call is successful
 *   @li					CSL_ESYS_FAIL      SRIO instance is not
 *                                             available.
 *   @li                    CSL_ESYS_INVPARAMS Invalid Parameters
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
        CSL_Status          status;
        CSL_SrioBaseAddress baseAddress;

        ...
        
        status = CSL_SrioGetBaseAddress(CSL_SRIO, NULL, &baseAddress);

        ...
    @endverbatim
 * ===========================================================================
 */
CSL_Status CSL_srioGetBaseAddress (
        CSL_InstNum         srioNum,
        CSL_SrioParam       *pSrioParam,
        CSL_SrioBaseAddress *pBaseAddress
);



/** ===========================================================================
 *   @n@b csl_srioClose.c
 *
 *   @b Description
 *   @n This function closes the specified instance of SRIO.
 *
 *   @b Arguments
 *   @verbatim
            hSrio         handle to the SRIO
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK            - SRIO is closed
 *                                               successfully
 *
 *   @li                    CSL_ESYS_BADHANDLE - The handle passed is invalid
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
            CSL_SrioHandle hSrio;
            CSL_Status     status;
            
            ...
            
            status = CSL_srioClose(hSrio);
     @endverbatim
 * ============================================================================
 */
CSL_Status CSL_srioClose (
    CSL_SrioHandle hSrio
);



/** ============================================================================
 *   @n@b CSL_srioHwSetup
 *
 *   @b Description
 *   @n It configures the  SRIO instance registers as per the values passed
 *      in the hardware setup structure.
 *
 *   @b Arguments
 *   @verbatim
            hSrio            Handle to the SRIO instance

            setup            Pointer to hardware setup structure
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK             - Hardware setup successful
 *   @li                    CSL_ESYS_BADHANDLE  - Invalid handle
 *   @li                    CSL_ESYS_INVPARAMS  - Hardware structure is not
 *                                                properly initialized
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  The specified instance will be setup according to value passed
 *
 *   @b Modifies
 *   @n Hardware registers for the specified instance
 *
 *   @b Example
 *   @verbatim
         CSL_SrioHandle 		hSrio;
         CSL_SrioObj			srioObj;
         CSL_SrioHwSetup		hwSetup;
	     CSL_Status				status;
         CSL_SrioControlSetup periSetup;     
         CSL_SrioBlkEn 	        blockSetup;
         CSL_SrioPktFwdCntl  	pktFwdSetup;

         hwSetup.perEn = TRUE;
         periSetup.swMemSleepOverride = FALSE;
    	 periSetup.loopback	= FALSE;
 	     . . .
    	 periSetup.prescalar = CSL_SRIO_CLK_PRESCALE_0;
         hwSetup.periCntlSetup = &periSetup;
         hwSetup.blkEn = &blockSetup;
         hwSetup.pktFwdCntl = &pktfwdSetup;
         ...
         
         hSrio = CSL_srioOpen (&srioObj, CSL_SRIO, NULL, &status);
         
         ...
         
         status = CSL_srioHwSetup(hSrio, &hwSetup);     

         ...
     @endverbatim
 * ===========================================================================
 */
CSL_Status  CSL_srioHwSetup (
    CSL_SrioHandle  hSrio,
    CSL_SrioHwSetup *hwSetup
);



/** ============================================================================
 *   @n@b CSL_srioHwControl
 *
 *   @b Description
 *   @n This function performs various control operations on the SRIO instance,
 *      based on the command passed.
 *
 *   @b Arguments
 *   @verbatim
            hSrio        Handle to the SRIO instance

            cmd          Operation to be performed on the SRIO

            cmdArg       Arguement specific to the command

     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK            - Command execution successful.
 *   @li                    CSL_ESYS_BADHANDLE - Invalid handle
 *   @li                    CSL_ESYS_INVCMD    - Invalid command
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  Registers of the SRIO instance are configured according to the command
 *       and the command arguments. The command determines which registers are
 *       modified.
 *
 *   @b Modifies
 *   @n Registers determined by the command
 *
 *   @b Example
 *   @verbatim
        CSL_SrioHandle   hSrio;
    	CSL_SrioPortData clearData;
        CSL_Status       status;
        Uint32           mask;
    	Uint8            index;
        ... 
        // for clearing LSU interrupts status [0..3]
    	index = 1;
        mask = CSL_SRIO_LSU_INTR3 | CSL_SRIO_LSU_INTR2 | 
               CSL_SRIO_LSU_INTR1 | CSL_SRIO_LSU_INTR0;
	    clearData.index = index;
    	clearData.data = mask;

        ...
        
        CSL_srioHwControl(hSrio, CSL_SRIO_CMD_LSU_INTR_CLEAR, &clearData);

        ...
     @endverbatim
 * ============================================================================
 */
CSL_Status  CSL_srioHwControl(
    CSL_SrioHandle       hSrio,
    CSL_SrioHwControlCmd cmd,
    void                 *cmdArg
);



/** ===========================================================================
 *   @n@b CSL_srioGetHwStatus
 *
 *   @b Description
 *   @n This function is used to get the value of various parameters of the
 *      SRIO instance. The value returned depends on the query passed.
 *
 *   @b Arguments
 *   @verbatim
            hSrio           Handle to the SRIO instance

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
 *   @n  None
 *
 *   <b> Post Condition </b>
 *       Data requested by the query is returned through the variable "response"
 *
 *   @b Modifies
 *   @n  The input arguement "response" is modified
 *
 *   @b Example
 *   @verbatim
         CSL_Status        status;
         CSL_SrioHandle    hSrio;
         CSL_SrioPidNumber response; 
         ...
         
         Status=CSL_srioGetHwStatus(hSrio, CSL_SRIO_QUERY_PID_NUMBER, &response);
         
         ...
     @endverbatim
 * ===========================================================================
 */
CSL_Status  CSL_srioGetHwStatus (
    CSL_SrioHandle        hSrio,
    CSL_SrioHwStatusQuery query,
    void                  *response
);



/** ============================================================================
 *   @n@b CSL_srioHwSetupRaw
 *
 *   @b Description
 *   @n This function initializes the device registers with the register-values
 *  	provided through the config data structure.
 *
 *   @b Arguments
 *   @verbatim
            hSrio           Handle to the SRIO instance

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
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  The registers of the specified SRIO instance will be setup
 *       according to the values passed through the config structure
 *
 *   @b Modifies
 *   @n Hardware registers of the specified SRIO
 *
 *   @b Example
 *   @verbatim
        CSL_SrioHandle hSrio;
        CSL_SrioConfig config = CSL_SRIO_CONFIG_DEFAULTS;
        CSL_Status     status;
        ...
        
        status = CSL_srioHwSetupRaw(hSrio, &config);

        ...
     @endverbatim
 * ===========================================================================
 */
CSL_Status  CSL_srioHwSetupRaw (
    CSL_SrioHandle hSrio,
    CSL_SrioConfig *config
);



/** ============================================================================
 *   @n@b CSL_srioGetHwSetup
 *
 *   @b Description
 *   @n It retrives the hardware setup parameters
 *
 *   @b Arguments
 *   @verbatim
            hSrio           Handle to the SRIO instance

            hwSetup         Pointer to hardware setup structure
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK             - Hardware setup retrived
 *   @li                    CSL_ESYS_BADHANDLE  - Invalid handle
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  The hardware set up structure will be populated with values from
 *       the registers
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
        CSL_status          status;
        CSL_SrioHwSetup     hwSetup;
        
        ...
        
        status = CSL_srioGetHwsetup(hSrio, &hwSetup);
        
        ...
     @endverbatim
 * ===========================================================================
 */
CSL_Status CSL_srioGetHwSetup (
    CSL_SrioHandle  hSrio,
    CSL_SrioHwSetup *hwSetup
);



/** ============================================================================
 *   @n@b CSL_srioLsuSetup
 *
 *   @b Description
 *   @n Function to configure the LSU module for Direct IO transfer
 *
 *   @b Arguments
 *   @verbatim
            hSrio           Handle to the SRIO instance

            lsuConfig       Pointer to the direct IO configuration structure

            index           index to the LSU block number
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK             - Successfully configured the 
                                                  LSU module
 *   @li                    CSL_ESYS_BADHANDLE  - Invalid handle is passed
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  The LSU module registers are configured with the passed parameters 
 *       and the data transfer starts.
 *
 *   @b Modifies
 *   @n LSU module registers
 *
 *   @b Example
 *   @verbatim
         CSL_status					status;
         CSL_SrioDirectIO_ConfigXfr lsuConfig;
         Uint8 					    index;

         index = 1;
         lsuConfig.srcNodeAdd = 0x2ffe550;
         lsuConfig.priority   = 2;

         ...
            
         Status = CSL_srioLsuSetup(hSrio, &lsuConfig, index);        

         ...
     @endverbatim
 * ===========================================================================
 */
CSL_Status CSL_srioLsuSetup (
    CSL_SrioHandle             hSrio,
    CSL_SrioDirectIO_ConfigXfr *lsuConfig,
    Uint8                      index
);

#ifdef __cplusplus
}
#endif

#endif  /* _CSL_SRIO_H_ */

