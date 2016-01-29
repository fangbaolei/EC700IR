/*  ============================================================================
 *   Copyright (c) Texas Instruments Inc 2002, 2003, 2004, 2005, 2006
 *
 *   Use of this software is controlled by the terms and conditions found in the
 *   license agreement under which this software has been supplied.
 *   ===========================================================================
 */

/** ============================================================================
 *   @file  csl_mdio.h
 *
 *   PATH:  \$(CSLPATH)\\inc
 *
 *   @brief  MDIO CSL Implementation on DSP side
 *
 */

/* =============================================================================
 *  Revision History
 *  ===============
 *  28-Jan-06  Ported to TCI6488
 * =============================================================================
 */
#ifndef CSL_MDIO_H
#define CSL_MDIO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <cslr.h>
#include <davinci_hd.h>
#include <csl_error.h>
#include <csl_types.h>
#include <cslr_mdio.h>

/**
 *  \defgroup   MDIO Device  MDIO
 *
 *  Constants, Objects and interfaces for MDIO
 *  @{
 */
/*-----------------------------------------------------------------------*\
* NEW TYPES
\*-----------------------------------------------------------------------*/
#ifndef _EMAC_TYPES
#define _EMAC_TYPES
typedef void * Handle;
/**< Void pointer type defined for MDIO                                     */
#endif

#define DEV_REGS    ((CSL_DevRegs *) CSL_DEV_REGS)
/**< Base address of device registers                                       */

#define MDIO_REGS   ((CSL_MdioRegs *) CSL_MDIO_0_REGS)
/**< Base address of MDIO registers                                         */

/*-----------------------------------------------------------------------*
 * PHY Control Register Macros
 *
 * These MACROS provide an easy way to read/write PHY registers
 *-----------------------------------------------------------------------*/
/** This macro reads from the PHY register through the USERACESS register 
 * provided in MDIO module
 */
#define PHYREG_read(regadr, phyadr)                             \
            MDIO_REGS->USERACCESS0 =                            \
                    CSL_FMK(MDIO_USERACCESS0_GO,1u)         |   \
                    CSL_FMK(MDIO_USERACCESS0_REGADR,regadr) |   \
                    CSL_FMK(MDIO_USERACCESS0_PHYADR,phyadr)

/** This macro writes to the PHY register through the USERACESS register 
 * provided in MDIO module
 */
#define PHYREG_write(regadr, phyadr, data)                      \
            MDIO_REGS->USERACCESS0 =                            \
                    CSL_FMK(MDIO_USERACCESS0_GO,1u)         |   \
                    CSL_FMK(MDIO_USERACCESS0_WRITE,1)       |   \
                    CSL_FMK(MDIO_USERACCESS0_REGADR,regadr) |   \
                    CSL_FMK(MDIO_USERACCESS0_PHYADR,phyadr) |   \
                    CSL_FMK(MDIO_USERACCESS0_DATA, data)

/** Waits for GO bit to set */
#define PHYREG_wait()                                           \
            while( CSL_FEXT(MDIO_REGS->USERACCESS0,MDIO_USERACCESS0_GO) )

/** Waits for GO bit to set and reads data from the PHY register */  
#define PHYREG_waitResults( results ) {                                                \
            while( CSL_FEXT(MDIO_REGS->USERACCESS0,MDIO_USERACCESS0_GO) );             \
            results = CSL_FEXT(MDIO_REGS->USERACCESS0,MDIO_USERACCESS0_DATA); }

/** Waits for GO bit to set, reads data from the PHY register and checks for ACK */  
#define PHYREG_waitResultsAck( results, ack ) {                                        \
            while( CSL_FEXT(MDIO_REGS->USERACCESS0,MDIO_USERACCESS0_GO) );             \
            results = CSL_FEXT( MDIO_REGS->USERACCESS0,MDIO_USERACCESS0_DATA );		   \
            ack = CSL_FEXT( MDIO_REGS->USERACCESS0, MDIO_USERACCESS0_ACK); }

/**
 *  \defgroup   MdioCfgModeFlags  MDIO Configuration Mode Flags
 *
 *  These flags determine how the MDIO module behaves
 *  @{
 */
#define MDIO_MODEFLG_AUTONEG        0x0001
/**< Use Autonegotiate                                                      */

/* The following bits are used for manual and fallback configuration        */
#define MDIO_MODEFLG_HD10           0x0002
/**< Use 10Mb/s Half Duplex                                                 */
#define MDIO_MODEFLG_FD10           0x0004
/**< Use 10Mb/s Full Duplex                                                 */
#define MDIO_MODEFLG_HD100          0x0008
/**< Use 100Mb/s Half Duplex                                                */
#define MDIO_MODEFLG_FD100          0x0010
/**< Use 100Mb/s Full Duplex                                                */
#define MDIO_MODEFLG_FD1000         0x0020
/**< Use 1000Mb/s Full Duplex                                               */
#define MDIO_MODEFLG_LOOPBACK       0x0040
/**< Use PHY Loopback                                                       */

/* The following bits are reserved for use by the MDIO module               */
#define MDIO_MODEFLG_NWAYACTIVE     0x0080
/**< NWAY currently active                                                  */
#define MDIO_MODEFLG_EXTLOOPBACK    0x0100
/**< Use external PHY Loopback, with plug                                   */
/* @} */

/**
 *  \defgroup  LinkStatus   MDIO Link Status Values
 *
 *  These values indicate current PHY link status
 *  @{
 */
#define MDIO_LINKSTATUS_NOLINK      0
/**< Link Status: No Link                                                   */
#define MDIO_LINKSTATUS_HD10        1
/**< Link Status: HD10                                                      */
#define MDIO_LINKSTATUS_FD10        2
/**< Link Status: FD10                                                      */
#define MDIO_LINKSTATUS_HD100       3
/**< Link Status: HD100                                                     */
#define MDIO_LINKSTATUS_FD100       4
/**< Link Status: FD100                                                     */
#define MDIO_LINKSTATUS_FD1000      5
/**< Link Status: FD1000                                                    */
/* @} */

/**
 *  \defgroup   MdioEvents  MDIO Events
 *
 *  These events are returned by MDIO_timerTick() to allow the application
 *  (or EMAC) to track MDIO status.
 *  @{
 */
#define MDIO_EVENT_NOCHANGE         0
/**< No change from previous status                                         */
#define MDIO_EVENT_LINKDOWN         1
/**< Link down event                                                        */
#define MDIO_EVENT_LINKUP           2
/**< Link (or re-link) event                                                */
#define MDIO_EVENT_PHYERROR         3
/**< No PHY connected                                                       */
/* @} */

#define VBUSCLK     165
/**< Standard defines/assumptions for MDIO interface                        */

/**
 *  \defgroup  PhyRegs MDIO PHY Registers
 *
 *  Used by MDIO to configure a MII compliant PHY
 *  @{
 */

/**
 *  \defgroup  PhyCtrlRegs MDIO PHY Control Registers
 *
 *  Used by MDIO to configure a MII compliant PHY
 *  @{
 */
#define PHYREG_CONTROL              0           /**< Control register       */
#define PHYREG_CONTROL_RESET        (1<<15)     /**< Set Reset bit          */
#define PHYREG_CONTROL_LOOPBACK     (1<<14)     /**< Set Loop back bit      */
#define PHYREG_CONTROL_SPEEDLSB     (1<<13)     /**< Set Speed LSB bit      */
#define PHYREG_CONTROL_AUTONEGEN    (1<<12)     /**< Auto Negate Enable bit */
#define PHYREG_CONTROL_POWERDOWN    (1<<11)     /**< Set Power Down bit     */
#define PHYREG_CONTROL_ISOLATE      (1<<10)     /**< Set Isolate bit        */
#define PHYREG_CONTROL_AUTORESTART  (1<<9)      /**< Set Auto restart bit   */
#define PHYREG_CONTROL_DUPLEXFULL   (1<<8)      /**< Set Full Duplex bit    */
#define PHYREG_CONTROL_SPEEDMSB     (1<<6)      /**< Set Speed MSB bit      */
/* @} */

/**
 *  \defgroup  PhyStatusRegs MDIO PHY Status Registers
 *
 *  Used by MDIO to configure a MII compliant PHY
 *  @{
 */
#define PHYREG_STATUS               1           /**< Status register        */
#define PHYREG_STATUS_FD100         (1<<14)     /**< Set FD100 bit          */
#define PHYREG_STATUS_HD100         (1<<13)     /**< Set HD100 bit          */
#define PHYREG_STATUS_FD10          (1<<12)     /**< Set FD10 bit           */
#define PHYREG_STATUS_HD10          (1<<11)     /**< Set HD10bit            */
#define PHYREG_STATUS_EXTSTATUS     (1<<8)      /**< Set External Status bit*/
#define PHYREG_STATUS_NOPREAMBLE    (1<<6)      /**< Set No preamble bit    */
#define PHYREG_STATUS_AUTOCOMPLETE  (1<<5)      /**< Set Auto complete bit  */
#define PHYREG_STATUS_REMOTEFAULT   (1<<4)      /**< Set Reomte default bit */
#define PHYREG_STATUS_AUTOCAPABLE   (1<<3)      /**< Set Auto Capable bit   */
#define PHYREG_STATUS_LINKSTATUS    (1<<2)      /**< Set Link status bit    */
#define PHYREG_STATUS_JABBER        (1<<1)      /**< Set Jabber bit         */
#define PHYREG_STATUS_EXTENDED      (1<<0)      /**< Set Extended bit       */
/* @} */

/**
 *  \defgroup  PhyIDRegs MDIO PHY ID Registers
 *
 *  Used by MDIO to configure a MII compliant PHY
 *  @{
 */
#define PHYREG_ID1                  2           /**< Physical ID 1 register */

#define PHYREG_ID2                  3           /**< Physical ID 1 register */
/* @} */

/**
 *  \defgroup  PhyAdvRegs MDIO PHY Advertise Registers
 *
 *  Used by MDIO to configure a MII compliant PHY
 *  @{
 */
#define PHYREG_ADVERTISE            4           /**< Physical Advertise reg */
#define PHYREG_ADVERTISE_NEXTPAGE   (1<<15)     /**< Set next page bit      */
#define PHYREG_ADVERTISE_FAULT      (1<<13)     /**< Set Fault bit          */
#define PHYREG_ADVERTISE_PAUSE      (1<<10)     /**< Set Pause bit          */
#define PHYREG_ADVERTISE_FD100      (1<<8)      /**< Set FD100 bit          */
#define PHYREG_ADVERTISE_HD100      (1<<7)      /**< Set HD100 bit          */
#define PHYREG_ADVERTISE_FD10       (1<<6)      /**< Set FD10 bit           */
#define PHYREG_ADVERTISE_HD10       (1<<5)      /**< Set HD10 bit           */
#define PHYREG_ADVERTISE_MSGMASK    (0x1F)      /**< Set Message mask bit   */
#define PHYREG_ADVERTISE_MSG        (1)         /**< Set Message bit        */
/* @} */

/**
 *  \defgroup  PhyPartnerRegs MDIO PHY Partner Registers
 *
 *  Used by MDIO to configure a MII compliant PHY
 *  @{
 */
#define PHYREG_PARTNER              5           /**< Physical Partner reg   */
#define PHYREG_PARTNER_NEXTPAGE     (1<<15)     /**< Set next page bit      */
#define PHYREG_PARTNER_ACK          (1<<14)     /**< Set Acknowledge bit    */
#define PHYREG_PARTNER_FAULT        (1<<13)     /**< Set Fault bit          */
#define PHYREG_PARTNER_PAUSE        (1<<10)     /**< Set Pause bit          */
#define PHYREG_PARTNER_FD100        (1<<8)      /**< Set FD100 bit          */
#define PHYREG_PARTNER_HD100        (1<<7)      /**< Set HD100 bit          */
#define PHYREG_PARTNER_FD10         (1<<6)      /**< Set FD10 bit           */
#define PHYREG_PARTNER_HD10         (1<<5)      /**< Set HD10 bit           */
#define PHYREG_PARTNER_MSGMASK      (0x1F)      /**< Set Message mask bit   */

#define PHYREG_1000CONTROL          9           /**< Physical 1000 Ctrl reg */
#define PHYREG_ADVERTISE_FD1000     (1<<9)      /**< Advertise FD1000 bit   */

#define PHYREG_1000STATUS           0xA         /**< Phy 1000 Status reg    */
#define PHYREG_PARTNER_FD1000       (1<<11)     /**< Partner FD1000 bit     */

#define PHYREG_EXTSTATUS            0x0F        /**< Physical Ext status reg*/
#define PHYREG_EXTSTATUS_FD1000     (1<<13)     /**< Ext Status FD1000 bit  */
/* @} */

/**
 *  \defgroup  PhyShadowRegs MDIO PHY Shadow Registers
 *
 *  Used by MDIO to configure a MII compliant PHY
 *  @{
 */
#define PHYREG_SHADOW               0x18        /**< Physical shadow reg    */
#define PHYREG_SHADOW_EXTLOOPBACK   0x8400      /**< Shadow Ext Loopback bit*/
#define PHYREG_SHADOW_RGMIIMODE     0xF080      /**< Shadow RGMII mode bit  */
#define PHYREG_SHADOW_INBAND        0xF1C7      /**< Shadow In band bit     */

#define PHYREG_ACCESS               0x1C        /**< Physical Access        */
#define PHYREG_ACCESS_COPPER        0xFC00      /**< Access Copper          */
/* @} */
/* @} */

/**
 *  \defgroup   PhyStateMachine MDIO PHY State Machine
 *
 *  When using auto-negotiation, the software must keep the MAC in
 *  sync with the PHY (for duplex). This module will also attempt to
 *  "auto-select" the PHY from a potential list of 32 based on which is
 *  first to get a link.
 *
 *  On detection of a good link, the link speed and duplex settings will be
 *  used to update the EMAC configuration (done external to this module).
 *
 *  States in the PHY State Machine:
 *  @{
 */
#define PHYSTATE_MDIOINIT           0   /**< MDIO Initialization state      */
#define PHYSTATE_RESET              1   /**< MDIO Reset State               */
#define PHYSTATE_NWAYSTART          2   /**< MDIO N Way start               */
#define PHYSTATE_NWAYWAIT           3   /**< MDIO N Way wait                */
#define PHYSTATE_LINKWAIT           4   /**< MDIO Wait for link             */
#define PHYSTATE_LINKED             5   /**< MDIO Linked                    */
/* @} */


/**
 *  \defgroup MDIODevObj    MDIO device Object
 *
 *  @{
 */

/**
 *  \brief  Interface
 *
 *  MDIO Interface enum
 */
enum Interface
{
    MII=0,  /**< */
    RMII,   /**< */
    GMII,   /**< */
    RGMII   /**< */
};

/**
 *  \brief MDIO_Device
 *  This is the MDIO object that contains the MDIO device object
 *  characteristics
 *
 */
typedef struct _MDIO_Device {
    Uint32            ModeFlags;
    /**< User specified configuration flags                                 */
    Uint32            phyAddr;
    /**< Current (or next) PHY addr (0-31)                                  */
    Uint32            phyState;
    /**< PHY State                                                          */
    Uint32            phyStateTicks;
    /**< Ticks elapsed in this PHY state                                    */
    Uint32            PendingStatus;
    /**< Pending Link Status                                                */
    Uint32            LinkStatus;
    /**< Link State PHYREG_STATUS_LINKSTATUS                                */
} MDIO_Device;
/* @} */

/**
 *  \defgroup  MDIOInterfaces MDIO Interfaces
 *
 *  Interfaces for the MDIO.
 *
 *  \par
 *  <b> NOTE: </b>
 *  When used in an multitasking environment, no MDIO function may be
 *  called while another MDIO function is operating on the same device
 *  handle in another thread. It is the responsibility of the application
 *  to assure adherence to this restriction.
 *
 *  @{
 */

/** ============================================================================
 *  @n@b MDIO_open()
 *
 *  @b Description
 *  @n Opens the MDIO peripheral and start searching for a PHY device.
 *
 *     It is assumed that the MDIO module is reset prior to calling this
 *     function.
 *
 *  @b Arguments
 *  @verbatim
        mdioModeFlags   mode flags pof MII
    @endverbatim
 *
 *  <b> Return Value </b>  Handle to the opened MDIO instance
 *
 * ============================================================================
 */
Handle MDIO_open( Uint32 mdioModeFlags );

/** ============================================================================
 *  @n@b MDIO_close()
 *
 *  @b Description
 *  @n Close the  MDIO peripheral and disable further operation.
 *
 *  @b Arguments
 *  @verbatim
        hMDIO   handle to the opened MDIO instance
    @endverbatim
 *
 *  <b> Return Value </b>  None
 *
 * ============================================================================
 */
void MDIO_close( Handle hMDIO );

/** ============================================================================
 *  @n@b MDIO_getStatus()
 *
 *  @b Description
 *  @n Called to get the status of the MDIO/PHY
 *
 *  @b Arguments
 *  @verbatim
        hMDIO       handle to the opened MDIO instance
        pPhy        pointer to the physical address
        pLinkStatus pointer to the link status
    @endverbatim
 *
 *  <b> Return Value </b>  None
 *
 * ============================================================================
 */
void MDIO_getStatus( Handle hMDIO, Uint32 *pPhy, Uint32 *pLinkStatus );

/** ============================================================================
 *  @n@b MDIO_timerTick()
 *
 *  @b Description
 *  @n Called to signify that approx 100mS have elapsed
 *
 *  @b Arguments
 *  @verbatim
        hMDIO   Handle to the opened MDIO instance
    @endverbatim
 *
 *  <b> Return Value </b>
 *  @n MDIO event code (see MDIO Events).
 *
 * ============================================================================
 */
Uint32 MDIO_timerTick( Handle hMDIO );

/** ============================================================================
 *  @n@b MDIO_initPHY()
 *
 *  @b Description
 *  @n Force a switch to the specified PHY, and start the negotiation process.

 *  @b Arguments
 *  @verbatim
        hMDIO   handle to the opened MDIO instance
        phyAddr Physical address
    @endverbatim
 *
 *  <b> Return Value </b>
 *  @n Returns 1 if the PHY selection completed OK,
 *  @n else 0
 *
 * ============================================================================
 */
Uint32 MDIO_initPHY( Handle hMDIO, Uint32 phyAddr );

/** ============================================================================
 *  @n@b MDIO_phyRegRead()
 *
 *  @b Description
 *  @n Raw data read of a PHY register.
 *
 *  @b Arguments
 *  @verbatim
        phyIdx  Physical Index
        phyReg  Physical register
        pData   Data read
    @endverbatim
 *
 *  <b> Return Value </b>
 *  @n Returns 1 if the PHY ACK'd the read,
 *  @n else 0
 *
 * ============================================================================
 */
Uint32 MDIO_phyRegRead( Uint32 phyIdx, Uint32 phyReg, Uint16 *pData );

/** ============================================================================
 *  @n@b MDIO_phyRegWrite()
 *
 *  @b Description
 *  @n Raw data write of a PHY register.
 *
 *  @b Arguments
 *  @verbatim
        phyIdx  Physical Index
        phyReg  Physical register
        pData   Data written
    @endverbatim
 *
 *  <b> Return Value </b>
 *  @n Returns 1 if the PHY ACK'd the write,
 *  @n else 0
 *
 * ============================================================================
 */
Uint32 MDIO_phyRegWrite( Uint32 phyIdx, Uint32 phyReg, Uint16 data );
/* @} */
/* @} */
#ifdef __cplusplus
}
#endif
#endif /* CSL_MDIO_H */
