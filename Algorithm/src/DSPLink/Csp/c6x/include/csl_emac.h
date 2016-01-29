/*  ============================================================================
 *   Copyright (c) Texas Instruments Inc 2002, 2003, 2004, 2005, 2006
 *
 *   Use of this software is controlled by the terms and conditions found in the
 *   license agreement under which this software has been supplied.
 *   ===========================================================================
 */

/** ============================================================================
 *   @file  csl_emac.h
 *
 *   PATH:  \$(CSLPATH)\\inc
 *
 *   @brief  EMAC CSL Implementation on DSP side
 *
 *  NOTE:
 *   When used in an multitasking environment, no EMAC function may be
 *   called while another EMAC function is operating on the same device
 *   handle in another thread. It is the responsibility of the application
 *   to assure adherence to this restriction.
 *
 */

/* =============================================================================
 *  Revision History
 *  ===============
 *  07-Oct-05 XXX File created
 *  14-Mar-06 PSK Added multi core support
 * =============================================================================
 */

/** @mainpage EMAC Documentation
 *
 * @section Introduction
 *
 * @subsection xxx Purpose and Scope
 * The purpose of this document is to detail the  CSL APIs for the EMAC
 * Module. The CSL developer is expected to refer to this document
 * while designing APIs for the modules which use EMAC module. Some of the
 * listed APIs may not be applicable to a given module. While other cases
 * this list of APIs may not be sufficient to cover all the features required
 * for a particular Module. The CSL developer should use his discretion
 * designing new APIs or extending the existing ones to cover these.
 *
 * @subsection aaa Terms and Abbreviations
 *   -# CSL:  Chip Support Library
 *   -# API:  Application Programmer Interface
 *   -# EMAC: Ethernet Media Access Controller
 *   -# MDIO: Management Data Input/Output
 *
 * @subsection References
 *   -# CSL 3.x Technical Requirements Specifications Version 0.5, dated
 *      May 14th, 2003
 *   -# EMAC User's Guide EMAC_catalog_SPRU975.pdf (SPRU975 August 2005)
 *   -# MDIO User's Guide EMAC_catalog_SPRU975.pdf (SPRU975 August 2005)
 *
 * @subsection Assumptions
 *    The abbreviations EMAC, emac and Emac have been used throughout this
 *    document to refer to Ethernet Media Access Controller and MDIO or mdio
 *    or Mdio refer to Management Data Input/Output.
 */
#ifndef CSL_EMAC_H
#define CSL_EMAC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <csl.h>
#include <cslr_emac.h>
#include <csl_mdio.h>
#include <davinci_hd.h>

/**************************************************************************\
* Peripheral Base Address
\**************************************************************************/
/** EMAC Module registers */
#define EMAC_REGS                 ((CSL_EmacRegs *)CSL_EMAC_0_REGS)

/** EMAC Control Module registers */
#define ECTL_REGS                 ((CSL_EctlRegs *)CSL_ECTL_0_REGS)

/**
 *  \defgroup   EMACDevice  EMAC
 *
 *  Constants, objects and interfaces for EMAC
 *  @{
 */
/**
 *  \defgroup   EMACConstants   EMAC Constants
 *  @{
 */

/**
 * \defgroup EMACPktFlags Packet Buffer Flags set in Flags
 *
 *  @{
 */
#define EMAC_PKT_FLAGS_SOP              0x80000000u
/**< Start of packet                                                        */
#define EMAC_PKT_FLAGS_EOP              0x40000000u
/**< End of packet                                                          */
/* @} */

/**
 *  \defgroup   RecvPktFlags    Receive Packet flags
 *
 * The Following Packet flags are set in Flags on RX packets only
 *
 * @{
 */
#define EMAC_PKT_FLAGS_HASCRC           0x04000000u
/**< RxCrc: PKT has 4byte CRC                                               */
#define EMAC_PKT_FLAGS_JABBER           0x02000000u
/**< RxErr: Jabber                                                          */
#define EMAC_PKT_FLAGS_OVERSIZE         0x01000000u
/**< RxErr: Oversize                                                        */
#define EMAC_PKT_FLAGS_FRAGMENT         0x00800000u
/**< RxErr: Fragment                                                        */
#define EMAC_PKT_FLAGS_UNDERSIZED       0x00400000u
/**< RxErr: Undersized                                                      */
#define EMAC_PKT_FLAGS_CONTROL          0x00200000u
/**< RxCtl: Control Frame                                                   */
#define EMAC_PKT_FLAGS_OVERRUN          0x00100000u
/**< RxErr: Overrun                                                         */
#define EMAC_PKT_FLAGS_CODEERROR        0x00080000u
/**< RxErr: Code Error                                                      */
#define EMAC_PKT_FLAGS_ALIGNERROR       0x00040000u
/**< RxErr: Alignment Error                                                 */
#define EMAC_PKT_FLAGS_CRCERROR         0x00020000u
/**< RxErr: Bad CRC                                                         */
#define EMAC_PKT_FLAGS_NOMATCH          0x00010000u
/**< RxPrm: No Match                                                        */
/* @} */

/**
 *  \defgroup   CfgModeFlags Configuration Mode Flags
 *
 * @{
 */
#define EMAC_CONFIG_MODEFLG_CHPRIORITY          0x00001
/**< Use Tx channel priority                                                */
#define EMAC_CONFIG_MODEFLG_MACLOOPBACK         0x00002
/**< MAC internal loopback                                                  */
#define EMAC_CONFIG_MODEFLG_RXCRC               0x00004
/**< Include CRC in RX frames                                               */
#define EMAC_CONFIG_MODEFLG_TXCRC               0x00008
/**< Tx frames include CRC                                                  */
#define EMAC_CONFIG_MODEFLG_PASSERROR           0x00010
/**< Pass error frames                                                      */
#define EMAC_CONFIG_MODEFLG_PASSCONTROL         0x00020
/**< Pass control frames                                                    */
#define EMAC_CONFIG_MODEFLG_PASSALL             0x00040
/**< pass all frames                                                          */
#define EMAC_CONFIG_MODEFLG_RXQOS               0x00080
/**< Enable QOS at receive side                                               */
#define EMAC_CONFIG_MODEFLG_RXNOCHAIN           0x00100
/**< Select no buffer chaining                                                */
#define EMAC_CONFIG_MODEFLG_RXOFFLENBLOCK       0x00200
/**< Enable offset/length blocking                                            */
#define EMAC_CONFIG_MODEFLG_RXOWNERSHIP         0x00400
/**< Use ownership bit as 1                                                   */
#define EMAC_CONFIG_MODEFLG_RXFIFOFLOWCNTL      0x00800
/**< Enable rx fifo flow control                                              */
#define EMAC_CONFIG_MODEFLG_CMDIDLE             0x01000
/**< Enable IDLE command                                                      */
#define EMAC_CONFIG_MODEFLG_TXSHORTGAPEN        0x02000
/**< Enable tx short gap                                                      */
#define EMAC_CONFIG_MODEFLG_TXPACE              0x04000
/**< Enable tx pacing                                                         */
#define EMAC_CONFIG_MODEFLG_TXFLOWCNTL          0x08000
/**< Enable tx flow control                                                   */
#define EMAC_CONFIG_MODEFLG_RXBUFFERFLOWCNTL    0x10000
/**< Enable rx buffer flow control                                            */
#define EMAC_CONFIG_MODEFLG_FULLDUPLEX          0x20000
/**< Set full duplex mode                                                   */
#define EMAC_CONFIG_MODEFLG_GIGABIT             0x40000
/**< Set gigabit                                                            */

/* @} */

#define EMAC_TEARDOWN_CHANNEL(x)    (1 << x)
/**< Macro to tear down selective Rx/Tx channels */

/**
 *  \defgroup   PktFiltering Packet Filtering
 *
 *  Packet Filtering Settings (cumulative)
 *
 *  @{
 */
#define EMAC_RXFILTER_NOTHING           0
/**< Receive filter set to Nothing                                          */
#define EMAC_RXFILTER_DIRECT            1
/**< Receive filter set to Direct                                           */
#define EMAC_RXFILTER_BROADCAST         2
/**< Receive filter set to Broadcast                                        */
#define EMAC_RXFILTER_MULTICAST         3
/**< Receive filter set to Multicast                                        */
#define EMAC_RXFILTER_ALLMULTICAST      4
/**< Receive filter set to All Mcast                                        */
#define EMAC_RXFILTER_ALL               5
/**< Receive filter set to All                                              */
/* @} */

/**
 *  \defgroup   ErrCodes    STANDARD ERROR CODES
 *  @{
 */
#define EMAC_ERROR_ALREADY              1
/**< Operation has already been started                                     */
#define EMAC_ERROR_NOTREADY             2
/**< Device is not open or not ready                                        */
#define EMAC_ERROR_DEVICE               3
/**< Device hardware error                                                  */
#define EMAC_ERROR_INVALID              4
/**< Function or calling parameter is invalid                               */
#define EMAC_ERROR_BADPACKET            5
/**< Supplied packet was invalid                                            */
#define EMAC_ERROR_MACFATAL             6
/**< Fatal Error - EMAC_close() required                                    */
/* @} */

#define EMAC_DEVMAGIC   0x0aceface
/**< Device Magic number                                                    */
#define EMAC_NUMSTATS   36
/**< Number of statistics regs                                              */

/* @} */

/**
 *  \defgroup   EMACObjects   EMAC Objects
 *  @{
 */

/**
 *  \brief  EMAC_Pkt
 *
 *  The packet structure defines the basic unit of memory used to hold data
 *  packets for the EMAC device.
 *
 *  A packet is comprised of one or more packet buffers. Each packet buffer
 *  contains a packet buffer header, and a pointer to the buffer data.
 *  The EMAC_Pkt structure defines the packet buffer header.
 *
 *  The pDataBuffer field points to the packet data. This is set when the
 *  buffer is allocated, and is not altered.
 *
 *  BufferLen holds the the total length of the data buffer that is used to
 *  store the packet (or packet fragment). This size is set by the entity
 *  that originally allocates the buffer, and is not altered.
 *
 *  The Flags field contains additional information about the packet
 *
 *  ValidLen holds the length of the valid data currently contained in the
 *  data buffer.
 *
 *  DataOffset is the byte offset from the start of the data buffer to the
 *  first byte of valid data. Thus (ValidLen+DataOffet)<=BufferLen.
 *
 *  Note that for receive buffer packets, the DataOffset field may be
 *  assigned before there is any valid data in the packet buffer. This allows
 *  the application to reserve space at the top of data buffer for private
 *  use. In all instances, the DataOffset field must be valid for all packets
 *  handled by EMAC.
 *
 *  The data portion of the packet buffer represents a packet or a fragment
 *  of a larger packet. This is determined by the Flags parameter. At the
 *  start of every packet, the SOP bit is set in Flags. If the EOP bit is
 *  also set, then the packet is not fragmented. Otherwise; the next packet
 *  structure pointed to by the pNext field will contain the next fragment in
 *  the packet. On either type of buffer, when the SOP bit is set in Flags,
 *  then the PktChannel, PktLength, and PktFrags fields must also be valid.
 *  These fields contain additional information about the packet.
 *
 *  The PktChannel field detetmines what channel the packet has arrived on,
 *  or what channel it should be transmitted on. The EMAC library supports
 *  only a single receive channel, but allows for up to eight transmit
 *  channels. Transmit channels can be treated as round-robin or priority
 *  queues.
 *
 *  The PktLength field holds the size of the entire packet. On single frag
 *  packets (both SOP and EOP set in BufFlags), PktLength and ValidLen will
 *  be equal.
 *
 *  The PktFrags field holds the number of fragments (EMAC_Pkt records) used
 *  to describe the packet. If more than 1 frag is present, the first record
 *  must have EMAC_PKT_FLAGS_SOP flag set, with corresponding fields validated.
 *  Each frag/record must be linked list using the pNext field, and the final
 *  frag/record must have EMAC_PKT_FLAGS_EOP flag set and pNext=0.
 *
 *  In systems where the packet resides in cacheable memory, the data buffer
 *  must start on a cache line boundary and be an even multiple of cache
 *  lines in size. The EMAC_Pkt header must not appear in the same cache line
 *  as the data portion of the packet. On multi-fragment packets, some packet
 *  fragments may reside in cacheable memory where others do not.
 *
 *  @verbatim
    <b> NOTE: It is up to the caller to assure that all packet buffers
    residing in cacheable memory are not currently stored in L1 or L2
    cache when passed to any EMAC function. </b>
    @endverbatim
 *
 *  Some of the packet Flags can only be set if the device is in the
 *  proper configuration to receive the corresponding frames. In order to
 *  enable these flags, the following modes must be set:
 *        RxCrc Flag  : RXCRC Mode in EMAC_Config
 *        RxErr Flags : PASSERROR Mode in EMAC_Config
 *        RxCtl Flags : PASSCONTROL Mode in EMAC_Config
 *        RxPrm Flag  : EMAC_RXFILTER_ALL in EMAC_setReceiveFilter()
 *
 */
typedef struct _EMAC_Pkt {
    Uint32           AppPrivate;
    /**< For use by the application                                         */
    struct _EMAC_Pkt *pPrev;
    /**< Previous record                                                    */
    struct _EMAC_Pkt *pNext;
    /**< Next record                                                        */
    Uint8            *pDataBuffer;
    /**< Pointer to Data Buffer (read only)                                 */
    Uint32           BufferLen;
    /**< Physical Length of buffer (read only)                              */
    Uint32           Flags;
    /**< Packet Flags                                                       */
    Uint32           ValidLen;
    /**< Length of valid data in buffer                                     */
    Uint32           DataOffset;
    /**< Byte offset to valid data                                          */
    Uint32           PktChannel;
    /**< Tx/Rx Channel/Priority 0-7 (SOP only)                              */
    Uint32           PktLength;
    /**< Length of Packet (SOP only) (same as ValidLen on single frag Pkt)  */
    Uint32           PktFrags;
    /**< No of frags in packet (SOP only) frag is EMAC_Pkt record-normally 1*/
} EMAC_Pkt;

/**
 *  \brief Packet Queue
 *
 * We keep a local packet queue for transmit and receive packets.
 * The queue structure is OS independent.
 */
typedef struct _pktq {
  Uint32             Count;
  /**< Number of packets in queue                                           */
  EMAC_Pkt          *pHead;
  /**< Pointer to first packet                                              */
  EMAC_Pkt          *pTail;
  /**< Pointer to last packet                                               */
} PKTQ;

/**
 *  \brief Transmit/Receive Descriptor Channel Structure
 *
 *  (One receive and up to 8 transmit in this example)
 */
typedef struct _EMAC_DescCh {
    struct _EMAC_Device *pd;
    /**< Pointer to parent structure                                        */
    PKTQ            DescQueue;
    /**< Packets queued as desc                                             */
    PKTQ            WaitQueue;
    /**< Packets waiting for TX desc                                        */
    Uint32          ChannelIndex;
    /**< Channel index 0-7                                                  */
    Uint32          DescMax;
    /**< Max number of desc (buffs)                                         */
    Uint32          DescCount;
    /**< Current number of desc                                             */
    EMAC_Desc       *pDescFirst;
    /**< First desc location                                                */
    EMAC_Desc       *pDescLast;
    /**< Last desc location                                                 */
    EMAC_Desc       *pDescRead;
    /**< Location to read next desc                                         */
    EMAC_Desc       *pDescWrite;
    /**< Location to write next desc                                        */
} EMAC_DescCh;


/**
 *  \brief Transmit/Receive Channel info Structure
 *
 *  (one receive and up to 8 transmit per core in this example)
 */
typedef struct _EMAC_ChannelInfo {
    Uint32    TxChannels;
    /**< Number of Tx Channels to use (1-8)
      * if TxChannels = 0 does not allocate the Tx channels for the core  
      * if TxChannels = 1, 2, ... 8 allocates that many numer of TxChannels
                                              for the core. 
      * User has to take care of allocating a portion from total allocation
      * for the cores */
    Uint32    RxChannels;
    /**< Number of Rx Channels  to use (1-8)                   
      *  if RxChannel = 0 does not allocate the Rx channel for the core
      *  if RxChannel = 1, 2, ...8 uses that many no of channels for the core        */  
} EMAC_ChannelInfo;

/**
 *  \brief  EMAC_Config
 *
 *  The config structure defines how the EMAC device should operate. It is
 *  passed to the device when the device is opened, and remains in effect
 *  until the device is closed.
 *
 *  The following is a short description of the configuration fields:
 *
 *  UseMdio      - Uses MDIO configuration if required. In case of SGMII
 *                 MAC to MAC communication MDIO is not required. If this 
 *                 field is one (1) configures MDIO
 *                          zero (0) does not configure MDIO
 *
 *  ModeFlags    - Specify the Fixed Operating Mode of the Device:
 *      - EMAC_CONFIG_MODEFLG_CHPRIORITY  - Treat TX channels as Priority Levels
 *                                   (Channel 7 is highest, 0 is lowest)
 *      - EMAC_CONFIG_MODEFLG_MACLOOPBACK - Set MAC in Internal Loopback for
 *                                          Testing
 *      - EMAC_CONFIG_MODEFLG_RXCRC       - Include the 4 byte EtherCRC in RX
 *                                          frames
 *      - EMAC_CONFIG_MODEFLG_TXCRC       - Assume TX Frames Include 4 byte
 *                                          EtherCRC
 *      - EMAC_CONFIG_MODEFLG_PASSERROR   - Receive Error Frames for Testing
 *      - EMAC_CONFIG_MODEFLG_PASSCONTROL - Receive Control Frames for
 *                                          Testing
 *
 *  MdioModeFlags - Specify the MDIO/PHY Operation (See csl_MDIO.H)
 *
 *  TxChannels    - Number of TX Channels to use (1-8, usually 1)
 *
 *  MacAddr       - Device MAC address
 *
 *  RxMaxPktPool  - Max Rx packet buffers to get from pool
 *                  (Must be in the range of 8 to 192)
 *
 *  A list of callback functions is used to register callback functions with
 *  a particular instance of the EMAC peripheral. Callback functions are
 *  used by EMAC to communicate with the application. These functions are
 *  REQUIRED for operation. The same callback table can be used for multiple
 *  driver instances.
 *
 *  The callback functions can be used by EMAC during any EMAC function, but
 *  mostly occur during calls to EMAC_statusIsr() and EMAC_statusPoll().
 *
 *  <b>pfcbGetPacket </b> -  Called by EMAC to get a free packet buffer from
 *                   the application layer for receive data. This function
 *                   should return NULL is no free packets are available.
 *                   The size of the packet buffer must be large enough
 *                   to accommodate a full sized packet (1514 or 1518
 *                   depending on the EMAC_CONFIG_MODEFLG_RXCRC flag), plus
 *                   any application buffer padding (DataOffset).
 *
 *  <b>pfcbFreePacket </b> - Called by EMAC to give a free packet buffer back
 *                   to the application layer. This function is used to
 *                   return transmit packets. Note that at the time of the
 *                   call, structure fields other than pDataBuffer and
 *                   BufferLen are in an undefined state.
 *
 *  <b>pfcbRxPacket </b>   - Called to give a received data packet to the
 *                   application layer. The applicaiton must accept the packet.
 *                   When the application is finished with the packet, it
 *                   can return it to its own free queue. This function also
 *                   returns a pointer to a free packet to replace the received
 *                   packet on the EMAC free list. It returns NULL when no free
 *                   packets are available. The return packet is the same as
 *                   would be returned by pfcbGetPacket. Thus if a newly
 *                   received packet is not desired, it can simply be returned
 *                   to EMAC via the return value.
 *
 *  <b>pfcbStatus </b>     - Called to indicate to the application that it
 *                   should call EMAC_getStatus() to read the current
 *                   device status. This call is made when device status
 *                   changes.
 *
 *  <b>pfcbStatistics </b> - Called to indicate to the application that it
 *                   should call EMAC_getStatistics() to read the
 *                   current Ethernet statistics. Called when the
 *                   statistic counters are to the point of overflow.
 *
 *  The hApplication calling calling argument is the application's handle
 *  as supplied to the EMAC device in the EMAC_open() function.
 */
typedef struct _EMAC_Config {
    Uint8       UseMdio;
    /**< MDIO Configuation select. User has to pass one (1) if MDIO 
         Configuration  is needed, if not should pass zero (0)              */
    Uint32      ModeFlags;
    /**< Configuation Mode Flags                                            */
    Uint32      MdioModeFlags;
    /**< CSL_MDIO Mode Flags (see CSL_MDIO.H)                               */
    EMAC_ChannelInfo ChannelInfo[3];
    /**< Tx and Rx Channels information for the three (3) cores to use      */
    Uint8       MacAddr[3][6];
    /**< Mac Address                                                        */
    Uint32      RxMaxPktPool;
    /**< Max Rx packet buffers to get from pool                             */
    EMAC_Pkt *  (*pfcbGetPacket)(Handle hApplication);
    /**< Get packet call back                                               */
    void        (*pfcbFreePacket)(Handle hApplication, EMAC_Pkt *pPacket);
    /**< Free packet call back                                              */
    EMAC_Pkt *  (*pfcbRxPacket)(Handle hApplication, EMAC_Pkt *pPacket);
    /**< Receive packet call back                                           */
    void        (*pfcbStatus)(Handle hApplication);
    /**< Get status call back                                               */
    void        (*pfcbStatistics)(Handle hApplication);
    /**< Get statistics call back                                           */
} EMAC_Config;

/**
 *  \brief  EMAC_Status
 *
 *  The status structure contains information about the MAC's run-time
 *  status.
 *
 *  The following is a short description of the configuration fields:
 *
 *  MdioLinkStatus - Current link stat (non-zero on link; see CSL_MDIO.H)
 *
 *  PhyDev         - Current PHY device in use (0-31)
 *
 *  RxPktHeld      - Current number of Rx packets held by the EMAC device
 *
 *  TxPktHeld      - Current number of Tx packets held by the EMAC device
 *
 *  FatalError     - Fatal Error Code (TBD)
 */
typedef struct _EMAC_Status {
    Uint32      MdioLinkStatus;
    /**< CSL_MDIO Link status (see csl_mdio.h)                              */
    Uint32      PhyDev;
    /**< Current PHY device in use (0-31)                                   */
    Uint32      RxPktHeld;
    /**< Number of packets held for Rx                                      */
    Uint32      TxPktHeld;
    /**< Number of packets held for Tx                                      */
    Uint32      FatalError;
    /**< Fatal Error when non-zero                                          */
} EMAC_Status;


/**
 *  \brief  EMAC_Statistics
 *
 *  The statistics structure is the used to retrieve the current count
 *  of various packet events in the system. These values represent the
 *  delta values from the last time the statistics were read.
 */
typedef struct _EMAC_Statistics {
    Uint32 RxGoodFrames;     /**< Good Frames Received                      */
    Uint32 RxBCastFrames;    /**< Good Broadcast Frames Received            */
    Uint32 RxMCastFrames;    /**< Good Multicast Frames Received            */
    Uint32 RxPauseFrames;    /**< PauseRx Frames Received                   */
    Uint32 RxCRCErrors;      /**< Frames Received with CRC Errors           */
    Uint32 RxAlignCodeErrors;/**< Frames Received with Alignment/Code Errors*/
    Uint32 RxOversized;      /**< Oversized Frames Received                 */
    Uint32 RxJabber;         /**< Jabber Frames Received                    */
    Uint32 RxUndersized;     /**< Undersized Frames Received                */
    Uint32 RxFragments;      /**< Rx Frame Fragments Received               */
    Uint32 RxFiltered;       /**< Rx Frames Filtered Based on Address       */
    Uint32 RxQOSFiltered;    /**< Rx Frames Filtered Based on QoS Filtering */
    Uint32 RxOctets;         /**< Total Received Bytes in Good Frames       */
    Uint32 TxGoodFrames;     /**< Good Frames Sent                          */
    Uint32 TxBCastFrames;    /**< Good Broadcast Frames Sent                */
    Uint32 TxMCastFrames;    /**< Good Multicast Frames Sent                */
    Uint32 TxPauseFrames;    /**< PauseTx Frames Sent                       */
    Uint32 TxDeferred;       /**< Frames Where Transmission was Deferred    */
    Uint32 TxCollision;      /**< Total Frames Sent With Collision          */
    Uint32 TxSingleColl;     /**< Frames Sent with Exactly One Collision    */
    Uint32 TxMultiColl;      /**< Frames Sent with Multiple Colisions       */
    Uint32 TxExcessiveColl;  /**< Tx Frames Lost Due to Excessive Collisions*/
    Uint32 TxLateColl;       /**< Tx Frames Lost Due to a Late Collision    */
    Uint32 TxUnderrun;       /**< Tx Frames Lost with Tx Underrun Error     */
    Uint32 TxCarrierSLoss;   /**< Tx Frames Lost Due to Carrier Sense Loss  */
    Uint32 TxOctets;         /**< Total Transmitted Bytes in Good Frames    */
    Uint32 Frame64;          /**< Total Tx&Rx with Octet Size of 64         */
    Uint32 Frame65t127;      /**< Total Tx&Rx with Octet Size of 65 to 127  */
    Uint32 Frame128t255;     /**< Total Tx&Rx with Octet Size of 128 to 255 */
    Uint32 Frame256t511;     /**< Total Tx&Rx with Octet Size of 256 to 511 */
    Uint32 Frame512t1023;    /**< Total Tx&Rx with Octet Size of 512 to 1023*/
    Uint32 Frame1024tUp;     /**< Total Tx&Rx with Octet Size of >=1024     */
    Uint32 NetOctets;        /**< Sum of all Octets Tx or Rx on the Network */
    Uint32 RxSOFOverruns;    /**< Total Rx Start of Frame Overruns          */
    Uint32 RxMOFOverruns;    /**< Total Rx Middle of Frame Overruns         */
    Uint32 RxDMAOverruns;    /**< Total Rx DMA Overruns                     */
} EMAC_Statistics;

/**
 *  \brief  EMAC Main Device Instance Structure
 *
 */
typedef struct _EMAC_Device {
    Uint32          DevMagic;
    /**< Magic ID for this instance                                         */
    Handle          hApplication;
    /**< Calling Application's Handle                                       */
    Handle          hMDIO;
    /**< Handle to MDIO Module                                              */
    Uint32          RxFilter;
    /**< Current RX filter value                                            */
    Uint32          PktMTU;
    /**< Max physical packet size                                           */
    Uint32          MacHash1;
    /**< Hash value cache                                                   */
    Uint32          MacHash2;
    /**< Hash value cache                                                   */
    Uint32          FatalError;
    /**< Fatal Error Code                                                   */
    EMAC_Config     Config;
    /**< Original User Configuration                                        */
    EMAC_Statistics Stats;
    /**< Current running statistics                                         */
    EMAC_DescCh     RxCh[8];
    /**< Receive channel status                                             */
    EMAC_DescCh     TxCh[8];
    /**< Transmit channel status                                            */
} EMAC_Device;

/* @} */

/**
 *  \defgroup   EMACInterfaces   EMAC Interfaces
 *
 *  Interfaces for EMAC module.
 *
 *  \par
 *  <b> Note: </b>The application is charged with verifying that only
 *  one of the following API calls may be executing at a given time across all
 *  threads and all interrupt functions.
 *  \par
 *  Hence, when used in an multitasking environment, no EMAC function may be
 *  called while another EMAC function is operating on the same device
 *  handle in another thread. It is the responsibility of the application
 *  to assure adherence to this restriction.
 *
 *  @{
 */

/** ============================================================================
 *  @n@b  EMAC_enumerate()
 *
 *  @b Description
 *  @n Enumerates the EMAC peripherals installed in the system and returns an
 *     integer count. The EMAC devices are enumerated in a consistent
 *     fashion so that each device can be later referenced by its physical
 *     index value ranging from "1" to "n" where "n" is the count returned
 *     by this function.
 *
 *  <b> Return Value </b>  None
 *
 *  <b> Pre Condition </b>
 *  @n  None
 *
 *  <b> Post Condition </b>
 *  @n  None
 *
 *  @b Example
 *  @verbatim
        EMAC_enumerate( );
    @endverbatim
 * ============================================================================
 */
Uint32 EMAC_enumerate( void );

/** ============================================================================
 *  @n@b  EMAC_open()
 *
 *  @b Description
 *  @n Opens the EMAC peripheral at the given physical index and initializes
 *     it to an embryonic state.
 *
 *     The calling application must supply a operating configuration that
 *     includes a callback function table. Data from this config structure is
 *     copied into the device's internal instance structure so the structure
 *     may be discarded after EMAC_open() returns. In order to change an item
 *     in the configuration, the EMAC device must be closed and then
 *     re-opened with the new configuration.
 *
 *     The application layer may pass in an hApplication callback handle,
 *     that will be supplied by the EMAC device when making calls to the
 *     application callback functions.
 *
 *     An EMAC device handle is written to phEMAC. This handle must be saved
 *     by the caller and then passed to other EMAC device functions.
 *
 *     The default receive filter prevents normal packets from being received
 *     until the receive filter is specified by calling EMAC_receiveFilter().
 *
 *     A device reset is achieved by calling EMAC_close() followed by EMAC_open().
 *
 *     The function returns zero on success, or an error code on failure.
 *
 *     Possible error codes include:
 *       EMAC_ERROR_ALREADY   - The device is already open
 *       EMAC_ERROR_INVALID   - A calling parameter is invalid
 *
 *  @b Arguments
 *  @verbatim
        physicalIndex   physical index
        hApplication    application handle
        pEMACConfig     EMAC's configuration structure
        phEMAC          handle to the EMAC device
    @endverbatim
 *
 *
 *  <b> Return Value </b>  Success (0)
 *  @n     EMAC_ERROR_INVALID   - A calling parameter is invalid
 *  @n     EMAC_ERROR_ALREADY   - The device is already open
 *
 *  <b> Pre Condition </b>
 *  @n  None
 *
 *  <b> Post Condition </b>
 *  @n  Opens the EMAC peripheral at the given physical index and initializes it.
 *
 *  @b Example
 *  @verbatim
        #define  EMAC_CONFIG_MODEFLG_MACLOOPBACK  0x0002
        #define  MDIO_MODEFLG_FD1000              0x0020
        #define  MDIO_MODEFLG_EXTLOOPBACK         0x0100

        EMAC_Config  ecfg;
        Handle       hEMAC = 0;
        // Setup the EMAC local loopback
        ecfg.ModeFlags      = EMAC_CONFIG_MODEFLG_MACLOOPBACK;
        ecfg.MdioModeFlags  = MDIO_MODEFLG_FD1000 | MDIO_MODEFLG_EXTLOOPBACK;
        ecfg.TxChannels     = 1;
        ecfg.RxMaxPktPool   = 8;
        //EMAC address from where it is stored in hardware.
        //For this example set EMAC address to be 00:01:02:03:04:05
    	for (j=0; j<3; j++){
        	for (i=0; i<6; i++){
		        if (j==0)
        	        ecfg.MacAddr[j][i] = i;
			    else
		            ecfg.MacAddr[j][i] = 0;
		    }
	    }

        EMAC_open(1, (Handle)0x12345678, &ecfg, &hEMAC);
    @endverbatim
 * ============================================================================
 */
Uint32 EMAC_open( int physicalIndex, Handle hApplication,
                EMAC_Config *pEMACConfig, Handle *phEMAC );

/** ============================================================================
 *  @n@b  EMAC_close()
 *
 *  @b Description
 *  @n Closed the EMAC peripheral indicated by the supplied instance handle.
 *     When called, the EMAC device will shutdown both send and receive
 *     operations, and free all pending transmit and receive packets.
 *
 *     The function returns zero on success, or an error code on failure.
 *
 *     Possible error code include:
 *         EMAC_ERROR_INVALID   - A calling parameter is invalid
 *
 *  @b Arguments
 *  @verbatim
        hEMAC   handle to opened the EMAC device
    @endverbatim
 *
 *  <b> Return Value </b>  Success (0)
 *  @n      EMAC_ERROR_INVALID   - A calling parameter is invalid
 *
 *  <b> Pre Condition </b>
 *  @n  EMAC_open function must be called before calling this API.
 *
 *  <b> Post Condition </b>
 *  @n  The EMAC device will shutdown both send and receive
 *      operations, and free all pending transmit and receive packets.
 *
 *  @b Example
 *  @verbatim
        EMAC_Config  ecfg;
        Handle       hEMAC = 0;
        //Open the EMAC peripheral
        EMAC_open(1, (Handle)0x12345678, &ecfg, &hEMAC);

        //Close the EMAC peripheral
        EMAC_close( hEMAC );
    @endverbatim
 * ============================================================================
 */
Uint32 EMAC_close( Handle hEMAC );

/** ============================================================================
 *  @n@b  EMAC_getStatus()
 *
 *  @b Description
 *  @n Called to get the current status of the device. The device status
 *     is copied into the supplied data structure.
 *
 *     The function returns zero on success, or an error code on failure.
 *
 *     Possible error code include:
 *      EMAC_ERROR_INVALID   - A calling parameter is invalid
 *
 *  @b Arguments
 *  @verbatim
        hEMAC   handle to the opened EMAC device
        pStatus Status of the EMAC
        coreNum core number
    @endverbatim
 *
 *  <b> Return Value </b>  Success (0)
 *  @n      EMAC_ERROR_INVALID   - A calling parameter is invalid
 *
 *  <b> Pre Condition </b>
 *  @n  EMAC peripheral instance must be opened before calling this API.
 *
 *  <b> Post Condition </b>
 *  @n  The current status of the device is copied into the supplied data
 *      structure.
 *
 *  @b Example
 *  @verbatim
        EMAC_Status status;
        EMAC_Config  ecfg;
        Handle       hEMAC = 0;
        Uint8        coreNum = 0;

        //Open the EMAC peripheral
        EMAC_open(1, (Handle)0x12345678, &ecfg, &hEMAC);

        EMAC_getStatus( hEMAC, &status, coreNum);
    @endverbatim
 * ============================================================================
 */
Uint32 EMAC_getStatus( Handle hEMAC, EMAC_Status *pStatus, Uint8 coreNum);

/** ============================================================================
 *  @n@b  EMAC_setReceiveFilter()
 *
 *  @b Description
 *  @n Called to set the packet filter for received packets. The filtering
 *     level is inclusive, so BROADCAST would include both BROADCAST and
 *     DIRECTED (UNICAST) packets.
 *
 *     Available filtering modes include the following:
 *         - EMAC_RXFILTER_NOTHING      - Receive nothing
 *         - EMAC_RXFILTER_DIRECT       - Receive only Unicast to local MAC addr
 *         - EMAC_RXFILTER_BROADCAST    - Receive direct and Broadcast
 *         - EMAC_RXFILTER_MULTICAST    - Receive above plus multicast in mcast list
 *         - EMAC_RXFILTER_ALLMULTICAST - Receive above plus all multicast
 *         - EMAC_RXFILTER_ALL          - Receive all packets
 *
 *     Note that if error frames and control frames are desired, reception of
 *     these must be specified in the device configuration.
 *
 *     The function returns zero on success, or an error code on failure.
 *
 *     Possible error code include:
 *         EMAC_ERROR_INVALID   - A calling parameter is invalid
 *
 *  @b Arguments
 *  @verbatim
         hEMAC           handle to the opened EMAC device
         ReceiveFilter   Filtering modes
    @endverbatim
 *
 *  <b> Return Value </b>  Success (0)
 *  @n      EMAC_ERROR_INVALID   - A calling parameter is invalid
 *
 *  <b> Pre Condition </b>
 *  @n  EMAC peripheral instance must be opened before calling this API
 *
 *  <b> Post Condition </b>
 *  @n  Sets the packet filter for received packets
 *
 *  @b Example
 *  @verbatim
        #define EMAC_RXFILTER_DIRECT       1
        EMAC_Config  ecfg;
        Handle       hEMAC = 0;

        EMAC_open(1, (Handle)0x12345678, &ecfg, &hEMAC);

        EMAC_setReceiveFilter(hEMAC, EMAC_RXFILTER_DIRECT );

    @endverbatim
 * ============================================================================
 */
Uint32 EMAC_setReceiveFilter( Handle hEMAC, Uint32 ReceiveFilter );

/** ============================================================================
 *  @n@b  EMAC_getReceiveFilter()
 *
 *  @b Description
 *  @n Called to get the current packet filter setting for received packets.
 *     The filter values are the same as those used in EMAC_setReceiveFilter().
 *
 *     The current filter value is written to the pointer supplied in
 *     pReceiveFilter.
 *
 *     The function returns zero on success, or an error code on failure.
 *
 *     Possible error code include:
 *       EMAC_ERROR_INVALID   - A calling parameter is invalid
 *
 *  @b Arguments
 *  @verbatim
        hEMAC           handle to the opened EMAC device
        pReceiveFilter  Current receive packet filter
    @endverbatim
 *
 *  <b> Return Value </b>  Success (0)
 *  @n      EMAC_ERROR_INVALID   - A calling parameter is invalid
 *
 *  <b> Pre Condition </b>
 *  @n  EMAC peripheral instance must be opened before calling this API and
 *      must be set the packet filter value.
 *
 *  <b> Post Condition </b>
 *  @n  The current filter value is written to the pointer supplied
 *
 *  @b Example
 *  @verbatim
        #define EMAC_RXFILTER_DIRECT       1
        EMAC_Config  ecfg;
        Handle       hEMAC = 0;
        Uint32         pReceiveFilter;

        EMAC_open(1, (Handle)0x12345678, &ecfg, &hEMAC);

        EMAC_setReceiveFilter(hEMAC, EMAC_RXFILTER_DIRECT );

        EMAC_getReceiveFilter(hEMAC, &pReceiveFilter );
    @endverbatim
 * ============================================================================
 */
Uint32 EMAC_getReceiveFilter( Handle hEMAC, Uint32 *pReceiveFilter );

/** ============================================================================
 *  @n@b  EMAC_getStatistics()
 *
 *  @b Description
 *  @n Called to get the current device statistics. The statistics structure
 *     contains a collection of event counts for various packet sent and
 *     receive properties. Reading the statistics also clears the current
 *     statistic counters, so the values read represent a delta from the last
 *     call.
 *
 *     The statistics information is copied into the structure pointed to
 *     by the pStatistics argument.
 *
 *     The function returns zero on success, or an error code on failure.
 *
 *     Possible error code include:
 *      EMAC_ERROR_INVALID   - A calling parameter is invalid
 *
 *  @b Arguments
 *  @verbatim
        hEMAC       handle to the opened EMAC device
        pStatistics Get the device statistics
    @endverbatim
 *
 *  <b> Return Value </b>  Success (0)
 *  @n      EMAC_ERROR_INVALID   - A calling parameter is invalid
 *
 *  <b> Pre Condition </b>
 *  @n  EMAC peripheral instance must be opened before calling this API
 *
 *  <b> Post Condition </b>
 *      -# Statistics are read for various packects sent and received.
        -# Reading the statistics also clears the current
           statistic counters, so the values read represent a delta from the
           last call.
 *
 *  @b Example
 *  @verbatim
        EMAC_Config      ecfg;
        Handle           hEMAC = 0;
        EMAC_Statistics  pStatistics;

        EMAC_open(1, (Handle)0x12345678, &ecfg, &hEMAC);

        EMAC_getStatistics(hEMAC, &pStatistics );
    @endverbatim
 * ============================================================================
 */
Uint32 EMAC_getStatistics( Handle hEMAC, EMAC_Statistics *pStatistics );

/** ============================================================================
 *  @n@b  EMAC_setMulticast()
 *
 *  @b Description
 *  @n This function is called to install a list of multicast addresses for
 *     use in multicast address filtering. Each time this function is called,
 *     any current multicast configuration is discarded in favor of the new
 *     list. Thus a set with a list size of zero will remove all multicast
 *     addresses from the device.
 *
 *     Note that the multicast list configuration is stateless in that the
 *     list of multicast addresses used to build the configuration is not
 *     retained. Thus it is impossible to examine a list of currently installed
 *     addresses.
 *
 *     The addresses to install are pointed to by pMCastList. The length of
 *     this list in bytes is 6 times the value of AddrCnt. When AddrCnt is
 *     zero, the pMCastList parameter can be NULL.
 *
 *     The function returns zero on success, or an error code on failure.
 *     The multicast list settings are not altered in the event of a failure
 *     code.
 *
 *     Possible error code include:
 *       EMAC_ERROR_INVALID   - A calling parameter is invalid
 *
 *  @b Arguments
 *  @verbatim
        hEMAC       handle to the opened EMAC device
        AddrCount   number of addresses to multicast
        pMCastList  pointer to the multi cast list
    @endverbatim
 *
 *
 *  <b> Return Value </b>  Success (0)
 *  @n      EMAC_ERROR_INVALID   - A calling parameter is invalid
 *
 *  <b> Pre Condition </b>
 *  @n  EMAC peripheral instance must be opened and set multicast filter.
 *
 *  <b> Post Condition </b>
        -# Install a list of multicast addresses for use in multicast
           address filtering.
        -# A set with a list size of zero will remove all multicast addresses
           from the device.
 *
 *  @b Example
 *  @verbatim
        #define EMAC_RXFILTER_ALLMULTICAST 4

        Handle       hEMAC = 0;
        Uint32         AddrCnt;
        Uint8        pMCastList;
        EMAC_Config  ecfg;

        EMAC_open(1, (Handle)0x12345678, &ecfg, &hEMAC);

        EMAC_setReceiveFilter( hEMAC, EMAC_RXFILTER_ALLMULTICAST );

        EMAC_setMulticast( hEMAC, AddrCnt, &pMCastList );
    @endverbatim
 * ============================================================================
 */
Uint32 EMAC_setMulticast( Handle hEMAC, Uint32 AddrCnt, Uint8 *pMCastList );

/** ============================================================================
 *  @n@b  EMAC_sendPacket()
 *
 *  @b Description
 *  @n Sends a Ethernet data packet out the EMAC device. On a non-error return,
 *     the EMAC device takes ownership of the packet. The packet is returned
 *     to the application's free pool once it has been transmitted.
 *
 *     The function returns zero on success, or an error code on failure.
 *     When an error code is returned, the EMAC device has not taken ownership
 *     of the packet.
 *
 *     Possible error codes include:
 *       EMAC_ERROR_INVALID   - A calling parameter is invalid
 *       EMAC_ERROR_BADPACKET - The packet structure is invalid
 *
 *  @b Arguments
 *  @verbatim
        hEMAC       handle to the opened EMAC device
        pPkt        EMAC packet structure
    @endverbatim
 *  <b> Return Value </b>  Success (0)
 *  @n      EMAC_ERROR_INVALID   - A calling parameter is invalid
 *  @n      EMAC_ERROR_BADPACKET - The packet structure is invalid
 *
 *  <b> Pre Condition </b>
 *  @n  EMAC peripheral instance must be opened and get a packet
 *      buffer from private queue
 *
 *  <b> Post Condition </b>
 *  @n  Sends a ethernet data packet out the EMAC device and is returned to the
 *      application,s free pool once it has been transmitted.
 *
 *  @b Example
 *  @verbatim
        #define EMAC_RXFILTER_DIRECT       1
        #define EMAC_PKT_FLAGS_SOP         0x80000000u
        #define EMAC_PKT_FLAGS_EOP         0x40000000u

        EMAC_Config ecfg;
        EMAC_Pkt    *pPkt;
        Handle      hEMAC = 0;
        Uint32      size, TxCount = 0;

        //open the EMAC device
        EMAC_open( 1, (Handle)0x12345678, &ecfg, &hEMAC );

        //set the receive filter
        EMAC_setReceiveFilter( hEMAC, EMAC_RXFILTER_DIRECT );

        //Fill the packet options fields
        size = TxCount + 60;
        pPkt->Flags      = EMAC_PKT_FLAGS_SOP | EMAC_PKT_FLAGS_EOP;
        pPkt->ValidLen   = size;
        pPkt->DataOffset = 0;
        pPkt->PktChannel = 0;
        pPkt->PktLength  = size;
        pPkt->PktFrags   = 1;

        EMAC_sendPacket( hEMAC, pPkt );

    @endverbatim
 * ============================================================================
 */
Uint32 EMAC_sendPacket( Handle hEMAC, EMAC_Pkt *pPkt);

/** ============================================================================
 *  @n@b  EMAC_RxServiceCheck()
 *
 *  @b Description
 *  @n This function should be called every time there is an EMAC device Rx
 *     interrupt. It maintains the status the EMAC.
 *
 *     Note that the application has the responsibility for mapping the
 *     physical device index to the correct EMAC_serviceCheck() function. If
 *     more than one EMAC device is on the same interrupt, the function must be
 *     called for each device.
 *
 *     Possible error codes include:
 *       EMAC_ERROR_INVALID   - A calling parameter is invalid
 *       EMAC_ERROR_MACFATAL  - Fatal error in the MAC - Call EMAC_close()
 *
 *  @b Arguments
 *  @verbatim
        hEMAC       handle to the opened EMAC device
        coreNum     core number
    @endverbatim
 *  <b> Return Value </b>  Success (0)
 *  @n     EMAC_ERROR_INVALID   - A calling parameter is invalid
 *  @n     EMAC_ERROR_MACFATAL  - Fatal error in the MAC - Call EMAC_close()
 *
 *  <b> Pre Condition </b>
 *  @n
 *
 *  <b> Post Condition </b>
 *  @n
 *
 *  @b Example
 *  @verbatim

    @endverbatim
 * ============================================================================
 */
Uint32 EMAC_RxServiceCheck( Handle hEMAC, Uint8 coreNum );


/** ============================================================================
 *  @n@b  EMAC_TxServiceCheck()
 *
 *  @b Description
 *  @n This function should be called every time there is an EMAC device Tx
 *     interrupt. It maintains the status the EMAC.
 *
 *     Note that the application has the responsibility for mapping the
 *     physical device index to the correct EMAC_serviceCheck() function. If
 *     more than one EMAC device is on the same interrupt, the function must be
 *     called for each device.
 *
 *     Possible error codes include:
 *       EMAC_ERROR_INVALID   - A calling parameter is invalid
 *       EMAC_ERROR_MACFATAL  - Fatal error in the MAC - Call EMAC_close()
 *
 *  @b Arguments
 *  @verbatim
        hEMAC       handle to the opened EMAC device
        coreNUm     core number
    @endverbatim
 *  <b> Return Value </b>  Success (0)
 *  @n     EMAC_ERROR_INVALID   - A calling parameter is invalid
 *  @n     EMAC_ERROR_MACFATAL  - Fatal error in the MAC - Call EMAC_close()
 *
 *  <b> Pre Condition </b>
 *  @n
 *
 *  <b> Post Condition </b>
 *  @n
 *
 *  @b Example
 *  @verbatim

    @endverbatim
 * ============================================================================
 */
Uint32 EMAC_TxServiceCheck( Handle hEMAC, Uint8 coreNum );


/** ============================================================================
 *  @n@b  EMAC_timerTick()
 *
 *  @b Description
 *  @n This function should be called for each device in the system on a
 *     periodic basis of 100mS (10 times a second). It is used to check the
 *     status of the EMAC and MDIO device, and to potentially recover from
 *     low Rx buffer conditions.
 *
 *     Strict timing is not required, but the application should make a
 *     reasonable attempt to adhere to the 100mS mark. A missed call should
 *     not be "made up" by making multiple sequential calls.
 *
 *     A "polling" driver (one that calls EMAC_serviceCheck() in a tight loop),
 *     must also adhere to the 100mS timing on this function.
 *
 *     Possible error codes include:
 *       EMAC_ERROR_INVALID   - A calling parameter is invalid

 *  @b Arguments
 *  @verbatim
        hEMAC       handle to the opened EMAC device
        coreNum     core number
    @endverbatim
 *
 *  <b> Return Value </b>  Success (0)
 *  @n      EMAC_ERROR_INVALID   - A calling parameter is invalid
 *
 *  <b> Pre Condition </b>
 *  @n  EMAC peripheral instance must be opened
 *
 *  <b> Post Condition </b>
 *  @n  None
 *
 *  @b Example
 *  @verbatim
        EMAC_Config ecfg;
        Handle      hEMAC = 0;

        //open the EMAC device
        EMAC_open( 1, (Handle)0x12345678, &ecfg, &hEMAC );

        EMAC_timerTick( hEMAC );
    @endverbatim
 * ============================================================================
 */
Uint32 EMAC_timerTick( Handle hEMAC, Uint8 coreNum );
/* @} */
/* @} */
#ifdef __cplusplus
}
#endif
#endif /* CSL_EMAC_H */

