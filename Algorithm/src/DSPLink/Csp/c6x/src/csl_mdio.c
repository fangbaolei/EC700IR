/*  ============================================================================
 *   Copyright (c) Texas Instruments Inc 2002, 2003, 2004, 2005, 2006
 *
 *   Use of this software is controlled by the terms and conditions found in the
 *   license agreement under which this software has been supplied.
 *   ===========================================================================
 */

/** ============================================================================
 *   @file  csl_mdio.c
 *
 *   PATH:  \$(CSLPATH)\\src\\mdio
 *
 *   @brief  MDIO CSL Implementation on DSP side
 *
 */

/* =============================================================================
 *  Revision History
 *  ===============
 *  25-Aug-03 xxx Modified to introduce ~(100 to 200) ms delay in PHY reset.
 *                Also checks ACK value on reading back PHY reset status
 *  10-Mar-06 xxx Ported to TCI6488
 * =============================================================================
 */

#include <csl_mdio.h>

/**
 *  \brief  Tick counts for timeout of each state
 *
 *  Note that NWAYSTART falls through to NWAYWAIT which falls through
 *  to LINKWAIT. The timeout is not reset progressing from one state
 *  to the next, so the system has 5 seconds total to find a link.
 */
static Uint32 PhyStateTimeout[] = {
                                2,    /**< PHYSTATE_MDIOINIT   - min-delay  */
                                6,    /**< PHYSTATE_RESET      - 0.5 sec max*/
                                41,   /**< PHYSTATE_NWAYSTART  - 4 seconds  */
                                41,   /**< PHYSTATE_NWAYWAIT   - 4 seconds  */
                                51,   /**< PHYSTATE_LINKWAIT   - 5 seconds  */
                                0     /**< PHYSTATE_LINKED     - no timeout */
                                };

static void MDIO_initStateMachine( MDIO_Device *pd );
static Uint32 MDIO_initContinue( MDIO_Device *pd );

static Uint32		macsel = GMII;

/**< Curent selection of MAC                                                */

/* ============================================================================
 *  @n@b MDIO_initStateMachine()
 *
 *  @b Description
 *  @n Internal function to initialize the state machine. It is referred to
 *     often in the code as it is called in case of a PHY error
 *
 *  @b Arguments
 *  @verbatim
        pd  pointer to MDIO device object
    @endverbatim
 *
 *  <b> Return Value </b>  None
 *
 *  <b> Pre Condition </b>
 *  @n  None
 *
 *  <b> Post Condition </b>
 *  @n  MDIO_Device Structure members are initialized means that MDIO state
 *      machine default values are set.
 *
 *  @b  Example:
 *  @verbatim
        MDIO_Device localDev;

        MDIO_initStateMachine( &localDev );
    @endverbatim
 * ============================================================================
 */
static void MDIO_initStateMachine( MDIO_Device *pd )
{
    /* Setup the state machine defaults */
    pd->phyAddr       = 0;                  /* The next PHY to try  */
    pd->phyState      = PHYSTATE_MDIOINIT;  /* PHY State            */
    pd->phyStateTicks = 0;                  /* Ticks elapsed        */
    pd->LinkStatus    = MDIO_LINKSTATUS_NOLINK;
}

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
 *  <b> Pre Condition </b>
 *  @n  The MDIO module must be reset prior to calling this function.
 *
 *  <b> Post Condition </b>
 *  @n  Opens the MDIO peripheral and start searching for a PHY device.
 *
 *  @b  Example:
 *  @verbatim
        #define MDIO_MODEFLG_FD1000         0x0020
        #define MDIO_MODEFLG_EXTLOOPBACK    0x0100

        Uint32 mdioModeFlags = MDIO_MODEFLG_FD1000 | MDIO_MODEFLG_LOOPBACK;

        MDIO_open ( mdioModeFlags );
    @endverbatim
 * ============================================================================
 */
Handle MDIO_open( Uint32 mdioModeFlags )
{
    /*
     * Note: In a multi-instance environment, we'd have to allocate "localDev"
     */
    static MDIO_Device localDev;

	/* Find out what interface we are working with */
	//macsel = CSL_FEXT(DEV_REGS->DEVSTAT, DEV_DEVSTAT_MACSEL);

    /* Get the mode flags from the user - clear our reserved flag */
    localDev.ModeFlags = mdioModeFlags & ~MDIO_MODEFLG_NWAYACTIVE;

    /* Setup the MDIO state machine */
    MDIO_initStateMachine( &localDev );

    /* Enable MDIO and setup divider */
    MDIO_REGS->CONTROL = CSL_FMKT(MDIO_CONTROL_ENABLE,YES) |
                         CSL_FMK(MDIO_CONTROL_CLKDIV,VBUSCLK) ;

    /* We're done for now - all the rest is done via MDIO_event() */
    return( &localDev );
}


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
 *  <b> Pre Condition </b>
 *  @n  MDIO module must be reset and opened before calling this function.
 *
 *  <b> Post Condition </b>
 *  @n  MDIO module is closed. No further operations are possible.
 *
 *  @b  Example:
 *  @verbatim
        #define MDIO_MODEFLG_FD1000      0x0020
        #define MDIO_MODEFLG_EXTLOOPBACK    0x0100

        Uint32    mdioModeFlags = MDIO_MODEFLG_FD1000 | MDIO_MODEFLG_LOOPBACK;
        Handle  hMDIO;

        //Open the MDIO module
        hMDIO = MDIO_open ( mdioModeFlags );

        MDIO_close( hMDIO );
    @endverbatim
 * ============================================================================
 */
void MDIO_close( Handle hMDIO )
{
    Uint32         ltmp1;
    Uint32           i;

    (void)hMDIO;

    /*
     * We really don't care what state anything is in at this point,
     * but to be safe, we'll isolate all the PHY devices.
     */
    ltmp1 = MDIO_REGS->ALIVE;
    for( i=0; ltmp1; i++,ltmp1>>=1 )
    {
        if( ltmp1 & 1 )
        {
            PHYREG_write( PHYREG_CONTROL, i, PHYREG_CONTROL_ISOLATE |
                                             PHYREG_CONTROL_POWERDOWN );
            PHYREG_wait();
        }
    }
}


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
 *  <b> Pre Condition </b>
 *  @n  MDIO module must be reset and opened before calling this API
 *
 *  <b> Post Condition </b>
 *  @n  MDIO stauts is returned through the parameters paased in this API.
 *
 *  @b  Example:
 *  @verbatim
        #define MDIO_MODEFLG_FD1000         0x0020
        #define MDIO_MODEFLG_EXTLOOPBACK    0x0100

        Uint32    mdioModeFlags = MDIO_MODEFLG_FD1000 | MDIO_MODEFLG_LOOPBACK;
        Handle  hMDIO;
        Uint32    pPhy, pLinkStatus;

        //Open the MDIO module
        hMDIO = MDIO_open ( mdioModeFlags );

        MDIO_getStatus( hMDIO, &pPhy, &pLinkStatus );

    @endverbatim
 * ============================================================================
 */
void MDIO_getStatus( Handle hMDIO, Uint32 *pPhy, Uint32 *pLinkStatus )
{
    MDIO_Device *pd = (MDIO_Device *)hMDIO;

    if( pPhy )
        *pPhy = pd->phyAddr;
    if( pLinkStatus )
        *pLinkStatus = pd->LinkStatus;
}


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
 *  <b> Pre Condition </b>
 *  @n  MDIO module must be reset and opened before calling this API.
 *
 *  <b> Post Condition </b>
 *  @n  Gets approximately 100mS delay
 *
 *  @b  Example:
 *  @verbatim
        #define MDIO_MODEFLG_FD1000         0x0020
        #define MDIO_MODEFLG_EXTLOOPBACK    0x0100

        Uint32    mdioModeFlags = MDIO_MODEFLG_FD1000 | MDIO_MODEFLG_LOOPBACK;
        Handle  hMDIO;

        //Open the MDIO module
        hMDIO = MDIO_open ( mdioModeFlags );

        MDIO_timerTick( hMDIO );
    @endverbatim
 * ============================================================================
 */
Uint32 MDIO_timerTick( Handle hMDIO )
{
    MDIO_Device *pd = (MDIO_Device *)hMDIO;
    Uint16      tmp1,tmp2,tmp1gig = 0, tmp2gig = 0, ack;
    Uint32      ltmp1;
    Uint32        RetVal = MDIO_EVENT_NOCHANGE;

    /*
     * If we are linked, we just check to see if we lost link. Otherwise;
     * we keep treking through our state machine.
     */
    if( pd->phyState == PHYSTATE_LINKED )
    {
        /*
         * Here we check for a "link-change" status indication or a link
         * down indication.
         */
        ltmp1 = MDIO_REGS->LINKINTRAW & 1;
        MDIO_REGS->LINKINTRAW = ltmp1;
        if( ltmp1 || !(MDIO_REGS->LINK)&(1<<pd->phyAddr) )
        {
            /*
             * There has been a change in link (or it is down)
             * If we do not auto-neg, then we just wait for a new link
             * Otherwise, we enter NWAYSTART or NWAYWAIT
             */

            pd->LinkStatus = MDIO_LINKSTATUS_NOLINK;
            RetVal = MDIO_EVENT_LINKDOWN;
            pd->phyStateTicks = 0;  /* Reset timeout */

            /* If not NWAY, just wait for link */
            if( !(pd->ModeFlags & MDIO_MODEFLG_NWAYACTIVE) )
                pd->phyState = PHYSTATE_LINKWAIT;
            else
            {
                /* Handle NWAY condition */

                /* First see if link is really down */
                PHYREG_read( PHYREG_STATUS, pd->phyAddr );
                PHYREG_wait();
                PHYREG_read( PHYREG_STATUS, pd->phyAddr );
                PHYREG_waitResultsAck( tmp1, ack );
                if( !ack )
                {
                    /* No PHY response, maybe it was unplugged */
                    MDIO_initStateMachine( pd );
                }
                else if( !(tmp1 & PHYREG_STATUS_LINKSTATUS) )
                {
                    /* No Link - restart NWAY */
                    pd->phyState = PHYSTATE_NWAYSTART;

                    PHYREG_write( PHYREG_CONTROL, pd->phyAddr,
                                  PHYREG_CONTROL_AUTONEGEN |
                                  PHYREG_CONTROL_AUTORESTART );
                    PHYREG_wait();
                }
                else
                {
                    /* We have a Link - re-read NWAY params  */
                    pd->phyState = PHYSTATE_NWAYWAIT;
                }
            }
        }
    }

    /*
     * If running in a non-linked state, execute the next
     * state of the state machine.
     */
    if( pd->phyState != PHYSTATE_LINKED )
    {
        /* Bump the time counter */
        pd->phyStateTicks++;

        /* Process differently based on state */
        switch( pd->phyState )
        {
        case PHYSTATE_RESET:
            /* Don't try to read reset status for the first 100 to 200 ms */
            if( pd->phyStateTicks < 2 )
                break;

            /* See if the PHY has come out of reset */
            PHYREG_read( PHYREG_CONTROL, pd->phyAddr );
            PHYREG_waitResultsAck( tmp1, ack );
            if( ack && !(tmp1 & PHYREG_CONTROL_RESET) )
            {
                /* PHY is not reset. If the PHY init is going well, break out */
                if( MDIO_initContinue( pd ) )
                    break;
                /* Else, this PHY is toast. Manually trigger a timeout */
                pd->phyStateTicks = PhyStateTimeout[pd->phyState];
            }

            /* Fall through to timeout check */

        case PHYSTATE_MDIOINIT:
CheckTimeout:
            /* Here we just check timeout and try to find a PHY */
            if( pd->phyStateTicks >= PhyStateTimeout[pd->phyState] )
            {
                // Try the next PHY if anything but a MDIOINIT condition
                if( pd->phyState != PHYSTATE_MDIOINIT )
                    if( ++pd->phyAddr == 32 )
                        pd->phyAddr = 0;
                ltmp1 = MDIO_REGS->ALIVE;
                for( tmp1=0; tmp1<32; tmp1++ )
                {
                    if( ltmp1 & (1<<pd->phyAddr) )
                    {
                        if( MDIO_initPHY( pd, pd->phyAddr ) )
                            break;
                    }

                    if( ++pd->phyAddr == 32 )
                        pd->phyAddr = 0;
                }

                // If we didn't find a PHY, try again
                if( tmp1 == 32 )
                {
                    pd->phyAddr       = 0;
                    pd->phyState      = PHYSTATE_MDIOINIT;
                    pd->phyStateTicks = 0;
                    RetVal = MDIO_EVENT_PHYERROR;
                }
            }
            break;

        case PHYSTATE_NWAYSTART:
            /*
             * Here we started NWAY. We check to see if NWAY is done.
             * If not done and timeout occured, we find another PHY.
             */

            /* Read the CONTROL reg to verify "restart" is not set */
            PHYREG_read( PHYREG_CONTROL, pd->phyAddr );
            PHYREG_waitResultsAck( tmp1, ack );
            if( !ack )
            {
                MDIO_initStateMachine( pd );
                break;
            }
            if( tmp1 & PHYREG_CONTROL_AUTORESTART )
                goto CheckTimeout;

            /* Flush latched "link status" from the STATUS reg */
            PHYREG_read( PHYREG_STATUS, pd->phyAddr );
            PHYREG_wait();

            pd->phyState = PHYSTATE_NWAYWAIT;

            /* Fallthrough */

        case PHYSTATE_NWAYWAIT:
            /*
             * Here we are waiting for NWAY to complete.
             */

            /* Read the STATUS reg to check for "complete" */
            PHYREG_read( PHYREG_STATUS, pd->phyAddr );
            PHYREG_waitResultsAck( tmp1, ack );
            if( !ack )
            {
                MDIO_initStateMachine( pd );
                break;
            }
            if( !(tmp1 & PHYREG_STATUS_AUTOCOMPLETE) )
                goto CheckTimeout;

            /* We can now check the negotiation results */

            if ( (macsel == GMII) || (macsel == RGMII) )
            {
                PHYREG_read( PHYREG_1000CONTROL, pd->phyAddr );
                PHYREG_waitResults( tmp1gig );
                PHYREG_read( PHYREG_1000STATUS, pd->phyAddr );
                PHYREG_waitResults( tmp2gig );
            }

            PHYREG_read( PHYREG_ADVERTISE, pd->phyAddr );
            PHYREG_waitResults( tmp1 );
            PHYREG_read( PHYREG_PARTNER, pd->phyAddr );
            PHYREG_waitResults( tmp2 );
            /*
             * Use the "best" results
             */
            tmp2 &= tmp1;

            /* Check first for 1 Gigabit */
            if( (tmp1gig & PHYREG_ADVERTISE_FD1000) && (tmp2gig & PHYREG_PARTNER_FD1000) )
                pd->PendingStatus = MDIO_LINKSTATUS_FD1000;
            else if( tmp2 & PHYREG_ADVERTISE_FD100 )
                pd->PendingStatus = MDIO_LINKSTATUS_FD100;
            else if( tmp2 & PHYREG_ADVERTISE_HD100 )
                pd->PendingStatus = MDIO_LINKSTATUS_HD100;
            else if( tmp2 & PHYREG_ADVERTISE_FD10 )
                pd->PendingStatus = MDIO_LINKSTATUS_FD10;
            else if( tmp2 & PHYREG_ADVERTISE_HD10 )
                pd->PendingStatus = MDIO_LINKSTATUS_HD10;
            /*
             * If we get here the negotiation failed
             * We just use HD 100 or 10 - the best we think we can do
             */
            else if( tmp1 & PHYREG_ADVERTISE_HD100 )
                pd->PendingStatus = MDIO_LINKSTATUS_HD100;
            else
                pd->PendingStatus = MDIO_LINKSTATUS_HD10;

            pd->phyState = PHYSTATE_LINKWAIT;

            /* Fallthrough */

        case PHYSTATE_LINKWAIT:
            /*
             * Here we are waiting for LINK
             */

            /* Read the STATUS reg to check for "link" */
            PHYREG_read( PHYREG_STATUS, pd->phyAddr );
            PHYREG_waitResultsAck( tmp1, ack );
            if( !ack )
            {
                MDIO_initStateMachine( pd );
                break;
            }
            if( !(tmp1 & PHYREG_STATUS_LINKSTATUS) )
                goto CheckTimeout;

            /* Make sure we're linked in the MDIO module as well */
            ltmp1 = MDIO_REGS->LINK;
            if( !(ltmp1&(1<<pd->phyAddr)) )
                goto CheckTimeout;

            /* Start monitoring this PHY */
            MDIO_REGS->USERPHYSEL0 = pd->phyAddr;

            /* Clear the link change flag so we can detect a "re-link" later */
            MDIO_REGS->LINKINTRAW = 1;

            /* Setup our linked state */
            pd->phyState   = PHYSTATE_LINKED;
            pd->LinkStatus = pd->PendingStatus;
            RetVal = MDIO_EVENT_LINKUP;

            break;
        }
    }

    return( RetVal );
}


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
 *  <b> Pre Condition </b>
 *  @n  MDIO module must be reset.
 *
 *  <b> Post Condition </b>
 *  @n  Initializes the specific PHY device.
 *
 *  @b  Example:
 *  @verbatim
        #define MDIO_MODEFLG_FD1000         0x0020
        #define MDIO_MODEFLG_EXTLOOPBACK    0x0100

        Uint32    mdioModeFlags = MDIO_MODEFLG_FD1000 | MDIO_MODEFLG_LOOPBACK;
        Handle         hMDIO;
        volatile Uint32  phyAddr;

        //Open the MDIO module
        hMDIO = MDIO_open ( mdioModeFlags );

        MDIO_initPHY( hMDIO, phyAddr );
    @endverbatim
 * ============================================================================
 */
Uint32 MDIO_initPHY( Handle hMDIO, volatile Uint32 phyAddr )
{
    MDIO_Device *pd = (MDIO_Device *)hMDIO;
    Uint32         ltmp1;
    Uint32           i,ack;

    /* Switch the PHY */
    pd->phyAddr = phyAddr;

    /* There will be no link when we're done with this PHY */
    pd->LinkStatus = MDIO_LINKSTATUS_NOLINK;

    /* Shutdown all other PHYs */
    ltmp1 = MDIO_REGS->ALIVE ;
    for( i=0; ltmp1; i++,ltmp1>>=1 )
    {
        if( (ltmp1 & 1) && (i != phyAddr) )
        {
            PHYREG_write( PHYREG_CONTROL, i, PHYREG_CONTROL_ISOLATE |
                                             PHYREG_CONTROL_POWERDOWN );
            PHYREG_wait();
        }
    }

    /* Reset the PHY we plan to use */
    PHYREG_write( PHYREG_CONTROL, phyAddr, PHYREG_CONTROL_RESET );
    PHYREG_waitResultsAck( i, ack );

    /* If the PHY did not ACK the write, return zero */
    if( !ack )
        return(0);


/*
// This is for Broadcom phys 
    if ( macsel == RGMII )
    {
        //Put phy in copper mode
        PHYREG_write( PHYREG_ACCESS, phyAddr, PHYREG_ACCESS_COPPER );
        PHYREG_wait();

        PHYREG_write( 0x10, phyAddr, 0x0000 );  //GMII Interface
        PHYREG_wait();

        // Put phy in RGMII mode/in-band status data
        PHYREG_write(PHYREG_SHADOW, phyAddr, PHYREG_SHADOW_INBAND);
        PHYREG_waitResultsAck( i, ack );

        // If the PHY did not ACK the write, return zero 
        if( !ack )
            return(0);

		// Override gtxcdly so it's low - it's still needed on EVM
		PHYREG_write( PHYREG_ACCESS, phyAddr, 0x8C00 );
		PHYREG_waitResultsAck( i, ack );

		// If the PHY did not ACK the write, return zero
		if( !ack )
			return(0);

    }
*/

    /* Setup for our next state */
    pd->phyState = PHYSTATE_RESET;
    pd->phyStateTicks = 0;  /* Reset timeout */

    return(1);
}


/** ============================================================================
 *  @n@b MDIO_initContinue()
 *
 *  @b Description
 *  @n Continues the initialization process started in MDIO_initPHY()
 *
 *  @b Arguments
 *  @verbatim
        pd  pointer to MDIO device object
    @endverbatim
 *
 *  <b> Return Value </b>
 *  @n Returns 0 on an error,
 *  @n 1 on success
 *
 *  <b> Pre Condition </b>
 *  @n  MDIO_initPHY function must be called before calling this API
 *
 *  <b> Post Condition </b>
 *  @n  Continues and completes the initialization process started in
 *      MDIO_initPHY()
 *
 *  @b  Example:
 *  @verbatim
        #define MDIO_MODEFLG_FD1000         0x0020
        #define MDIO_MODEFLG_EXTLOOPBACK    0x0100

        Uint32    mdioModeFlags = MDIO_MODEFLG_FD1000 | MDIO_MODEFLG_LOOPBACK;
        Handle         hMDIO;
        volatile Uint32  phyAddr;

        //Open the MDIO module
        hMDIO = MDIO_open ( mdioModeFlags );

        MDIO_initPHY( hMDIO, phyAddr );

        MDIO_initContinue( (MDIO_Device *)hMDIO );
    @endverbatim
 * ============================================================================
 */
static Uint32 MDIO_initContinue( MDIO_Device *pd )
{
    Uint16      tmp1,tmp2;
    Uint16      tmp1gig = 0;

    /* Read the STATUS reg to check autonegotiation capability */
    PHYREG_read( PHYREG_STATUS, pd->phyAddr );
    PHYREG_waitResults( tmp1 );

    if ( (macsel == GMII) || (macsel == RGMII) )
    {
        PHYREG_read( PHYREG_EXTSTATUS, pd->phyAddr );
        PHYREG_waitResults( tmp1gig );
    }

    /* See if we auto-neg or not */
    if( (pd->ModeFlags & MDIO_MODEFLG_AUTONEG) &&
                                     (tmp1 & PHYREG_STATUS_AUTOCAPABLE) )
    {
        /* We will use NWAY */

        /* Advertise 1000 for supported interfaces */
        if ( (macsel == GMII) || (macsel == RGMII) )
        {
            tmp1gig >>= 4;
            tmp1gig &= PHYREG_ADVERTISE_FD1000;

            PHYREG_write( PHYREG_1000CONTROL, pd->phyAddr, tmp1gig );
        }

        /* Shift down the capability bits */
        tmp1 >>= 6;

        /* Mask with the capabilities */
        tmp1 &= ( PHYREG_ADVERTISE_FD100 | PHYREG_ADVERTISE_HD100 |
                  PHYREG_ADVERTISE_FD10 | PHYREG_ADVERTISE_HD10 );

        /* Set Ethernet message bit */
        tmp1 |= PHYREG_ADVERTISE_MSG;

        /* Write out advertisement */
        PHYREG_write( PHYREG_ADVERTISE, pd->phyAddr, tmp1 );
        PHYREG_wait();

        /* Start NWAY */
        PHYREG_write( PHYREG_CONTROL, pd->phyAddr, PHYREG_CONTROL_AUTONEGEN );
        PHYREG_wait();

        PHYREG_write( PHYREG_CONTROL, pd->phyAddr,
                      PHYREG_CONTROL_AUTONEGEN|PHYREG_CONTROL_AUTORESTART );
        PHYREG_wait();

        /* Setup current state */
        pd->ModeFlags |= MDIO_MODEFLG_NWAYACTIVE;
        pd->phyState = PHYSTATE_NWAYSTART;
        pd->phyStateTicks = 0;  /* Reset timeout */
    }
    else
    {
        /* We will use a fixed configuration */

        /* Shift down the capability bits */
        tmp1 >>= 10;

        /* Mask with possible modes */
        tmp1 &= ( MDIO_MODEFLG_HD10 | MDIO_MODEFLG_FD10 |
                  MDIO_MODEFLG_HD100 | MDIO_MODEFLG_FD100 );

        if ( (macsel == GMII) || (macsel == RGMII) )
        {
            tmp1gig >>= 8;
            tmp1gig&= MDIO_MODEFLG_FD1000;

            /* Mask with what the User wants to allow */
            tmp1gig &= pd->ModeFlags;

        }

        /* Mask with what the User wants to allow */
        tmp1 &= pd->ModeFlags;

        /* If nothing if left, move on */
        if( (!tmp1) && (!tmp1gig) )
            return(0);

        /* Setup Control word and pending status */
        if( tmp1gig ) {
                tmp2 = PHYREG_CONTROL_SPEEDMSB | PHYREG_CONTROL_DUPLEXFULL;
                pd->PendingStatus = MDIO_LINKSTATUS_FD1000;
        }
        else if( tmp1 & MDIO_MODEFLG_FD100 )
        {
            tmp2 = PHYREG_CONTROL_SPEEDLSB | PHYREG_CONTROL_DUPLEXFULL;
            pd->PendingStatus = MDIO_LINKSTATUS_FD100;
        }
        else if( tmp1 & MDIO_MODEFLG_HD100 )
        {
            tmp2 = PHYREG_CONTROL_SPEEDLSB;
            pd->PendingStatus = MDIO_LINKSTATUS_HD100;
        }
        else if( tmp1 & MDIO_MODEFLG_FD10 )
        {
            tmp2 = PHYREG_CONTROL_DUPLEXFULL;
            pd->PendingStatus = MDIO_LINKSTATUS_FD10;
        }
        else
        {
            tmp2 = 0;
            pd->PendingStatus = MDIO_LINKSTATUS_HD10;
        }

        /* Add in internal phy loopback if user wanted it */
        if( pd->ModeFlags & MDIO_MODEFLG_LOOPBACK )
            tmp2 |= PHYREG_CONTROL_LOOPBACK;

        /* Configure PHY */
        PHYREG_write( PHYREG_CONTROL, pd->phyAddr, tmp2 );
        PHYREG_wait();

        /* Add in external phy loopback with plug if user wanted it */
        if( pd->ModeFlags & MDIO_MODEFLG_EXTLOOPBACK ) {
            PHYREG_write( PHYREG_SHADOW, pd->phyAddr, PHYREG_SHADOW_EXTLOOPBACK );
            PHYREG_wait();
        }

        /* Setup current state */
        pd->ModeFlags &= ~MDIO_MODEFLG_NWAYACTIVE;
        pd->phyState = PHYSTATE_LINKWAIT;
        pd->phyStateTicks = 0;  /* Reset timeout */
    }

    return(1);
}


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
 *  <b> Pre Condition </b>
 *  @n  MDIO module must be reset and opened. PHY device must be initialized.
 *
 *  <b> Post Condition </b>
 *  @n  Raw data is read from a PHY register.
 *
 *  @b  Example:
 *  @verbatim
        #define MDIO_MODEFLG_FD1000         0x0020
        #define MDIO_MODEFLG_EXTLOOPBACK    0x0100

        volatile Uint32 phyIdx = PHYREG_CONTROL;
        volatile Uint32 phyReg;
        Uint16 pData;

        Uint32    mdioModeFlags = MDIO_MODEFLG_FD1000 | MDIO_MODEFLG_LOOPBACK;
        Handle         hMDIO;
        volatile Uint32  phyAddr;

        //Open the MDIO module
        hMDIO = MDIO_open ( mdioModeFlags );

        MDIO_initPHY( hMDIO, phyAddr );

        MDIO_phyRegRead( phyIdx, ((MDIO_Device *)hMDIO)->phyReg, pData );
    @endverbatim
 * ============================================================================
 */
Uint32 MDIO_phyRegRead( volatile Uint32 phyIdx, volatile Uint32 phyReg, Uint16 *pData )
{
    Uint32 data,ack;

    PHYREG_read( phyReg, phyIdx );
    PHYREG_waitResultsAck( data, ack );
    if( !ack )
        return(0);
    if( pData )
        *pData = data;
    return(1);
}


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
 *  <b> Pre Condition </b>
 *  @n  MDIO module must be reset and opened. PHY device must be initialized.
 *
 *  <b> Post Condition </b>
 *  @n  Raw data is written to a PHY register.
 *
 *  @b  Example:
 *  @verbatim
        #define MDIO_MODEFLG_FD1000         0x0020
        #define MDIO_MODEFLG_EXTLOOPBACK    0x0100
        #define PHYREG_SHADOW_EXTLOOPBACK   0x8400

        volatile Uint32 phyIdx = PHYREG_CONTROL;
        volatile Uint32 phyReg;
        Uint16 pData = PHYREG_SHADOW_EXTLOOPBACK;

        Uint32    mdioModeFlags = MDIO_MODEFLG_FD1000 | MDIO_MODEFLG_LOOPBACK;
        Handle         hMDIO;
        volatile Uint32  phyAddr;

        //Open the MDIO module
        hMDIO = MDIO_open ( mdioModeFlags );

        MDIO_initPHY( hMDIO, phyAddr );

        MDIO_phyRegWrite( phyIdx, ((MDIO_Device *)hMDIO)->phyReg, pData );
    @endverbatim
 * ============================================================================
 */
Uint32 MDIO_phyRegWrite( volatile Uint32 phyIdx, volatile Uint32 phyReg, Uint16 data )
{
    Uint32 ack;

    PHYREG_write( phyReg, phyIdx, data );
    PHYREG_waitResultsAck( data, ack );
    if( !ack )
        return(0);
    return(1);
}

