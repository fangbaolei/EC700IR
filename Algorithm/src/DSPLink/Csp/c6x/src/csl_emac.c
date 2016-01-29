/*  ============================================================================
 *   Copyright (c) Texas Instruments Inc 2002, 2003, 2004, 2005, 2006
 *
 *   Use of this software is controlled by the terms and conditions found in the
 *   license agreement under which this software has been supplied.
 *   ===========================================================================
 */

/** ============================================================================
 *   @file  csl_emac.c
 *
 *   PATH:  \$(CSLPATH)\\src\\emac
 *
 *   @brief  EMAC CSL Implementation on DSP side
 *
 *  \par
 *  NOTE:
 *  When used in an multitasking environment, no EMAC function may be
 *  called while another EMAC function is operating on the same device
 *  handle in another thread. It is the responsibility of the application
 *  to assure adherence to this restriction.
 *
 *  \par
 *  Although the EMAC API is defined to support multiple device instances,
 *  this version supports a single device instance
 */

/* =============================================================================
 *  Revision History
 *  ===============
 *  16-Mar-06 xxx Ported to TCI6488
 * =============================================================================
 */

#include <cslr_ectl.h>
#include <csl_emac.h>

extern void   *memset(void *_mem, int _ch, Uint32 _n);
/**< Default Memset API                                                     */

/* Queue Helper Functions                                                   */
static EMAC_Pkt *pqPop( PKTQ *pq );
static void pqPush( PKTQ *pq, EMAC_Pkt *pPktHdr );
static void pqPushChain( PKTQ *pq, EMAC_Pkt *pPktHdrFirst,
                         EMAC_Pkt *pPktHdrLast, Uint32 Count );

static Uint32           openFlag = 0;
/**< Flag to indicate if the EMAC is already opened.                        */
static EMAC_Device      localDev;
/**< Local copy of the EMAC device instance                                 */

Uint32                  TotalTxChannels = 0;
/* Total number of Tx channels used by all the three cores                 */

Uint32                  TotalRxChannels = 0;
/* Total number of Rx channels used by all the three cores                 */


/*
 * Local Helper Functions
 */
static void emacUpdateStats(EMAC_Device *pd);
static void emacEnqueueTx(EMAC_DescCh *pdc);
static void emacDequeueTx(EMAC_DescCh *pdc, EMAC_Desc *pDescLast);
static void emacEnqueueRx(EMAC_DescCh *pdc, Uint32 fRestart);
static void emacDequeueRx(EMAC_DescCh *pdc, EMAC_Desc *pDescAck);


/** ============================================================================
 *  @n@b pqPop()
 *
 *  @b Description
 *  @n Pop a desc buffer off a queue
 *
 *  @b Arguments
 *  @verbatim
        pq      pointer to packet queue
    @endverbatim
 *
 *  <b> Return Value </b>  Pointer to EMAC packet
 *
 *  <b> Pre Condition </b>
 *  @n  None
 *
 *  <b> Post Condition </b>
 *  @n  Descriptor buffer from the Queue is removed
 *
 *  @b Example
 *  @verbatim
        PKTQ *pq;
        pqPop( pq );
    @endverbatim
 * ============================================================================
 */
static EMAC_Pkt *pqPop(PKTQ *pq)
{
    EMAC_Pkt *pPktHdr;

    pPktHdr = pq->pHead;

    if (pPktHdr)
    {
        pq->pHead = pPktHdr->pNext;
        pq->Count--;
        pPktHdr->pPrev = pPktHdr->pNext = 0;
    }

    return (pPktHdr);
}

/** ============================================================================
 *  @n@b pqPush()
 *
 *  @b Description
 *  @n Push a desc buffer onto a queue
 *
 *  @b Arguments
 *  @verbatim
        pq      pointer to packet queue
        pPktHdr pointer to the EMAC packet
    @endverbatim
 *
 *
 *  <b> Return Value </b>  None
 *
 *  <b> Pre Condition </b>
 *  @n  None
 *
 *  <b> Post Condition </b>
 *  @n  Descriptor buffer from the Queue is added
 *
 *  @b Example
 *  @verbatim
        PKTQ *pq;
        EMAC_Pkt *pPktHdr

        pqPush( pq, pPktHdr );
    @endverbatim
 * ============================================================================
 */
static void pqPush(PKTQ *pq, EMAC_Pkt *pPktHdr)
{
    pPktHdr->pNext = 0;

    if (!pq->pHead)
    {
        // Queue is empty - Initialize it with this one packet
        pq->pHead = pPktHdr;
        pq->pTail = pPktHdr;
    }
    else
    {
        // Queue is not empty - Push onto END
        pq->pTail->pNext = pPktHdr;
        pq->pTail        = pPktHdr;
    }
    pq->Count++;
}
/** ============================================================================
 *  @n@b pqPushChain()
 *
 *  @b Description
 *  @n Push a desc buffer chain onto a queue
 *
 *  @b Arguments
 *  @verbatim
        pq              pointer to packet queue
        pPktHdrFirst    pointer to the first element of the EMAC Packet.
        pPktHdrLast     pointer to the last element of the EMAC Packet.
        Count           count of elements to push
    @endverbatim
 *
 *
 *  <b> Return Value </b>  None
 *
 *  <b> Pre Condition </b>
 *  @n  None
 *
 *  <b> Post Condition </b>
 *  @n  A chain of descriptor buffers are added to the Queue
 *
 *  @b Example
 *  @verbatim
        PKTQ *pq;
        EMAC_Pkt *pPktHdrFirst, *pPktHdrLast;
        Uint32 Count;

        pqPushChain( pq, pPktHdrFirst, pPktHdrLast, Count );
    @endverbatim
 * ============================================================================
 */
static void pqPushChain(PKTQ *pq, EMAC_Pkt *pPktHdrFirst,
                         EMAC_Pkt *pPktHdrLast, Uint32 Count)
{
    pPktHdrLast->pNext = 0;

    if (!pq->pHead)
    {
        // Queue is empty - Initialize it with this packet chain
        pq->pHead = pPktHdrFirst;
        pq->pTail = pPktHdrLast;
    }
    else
    {
        // Queue is not empty - Push onto END
        pq->pTail->pNext = pPktHdrFirst;
        pq->pTail        = pPktHdrLast;
    }
    pq->Count += Count;
}

/** ============================================================================
 *  @n@b emacUpdateStats()
 *
 *  @b Description
 *  @n Update our local copy of the statistics
 *
 *  @b Arguments
 *  @verbatim
        pd  pointer to EMAC object
    @endverbatim
 *  <b> Return Value </b>  None
 *
 *  <b> Pre Condition </b>
 *  @n  EMAC peripheral instance must be opened
 *
 *  <b> Post Condition </b>
 *  @n  Update local copy of the statistics to the EMAC registers
 *
 *  @b Example
 *  @verbatim
        EMAC_Device *pd;

        emacUpdateStats( pd );
    @endverbatim
 * ============================================================================
 */
static void emacUpdateStats( EMAC_Device *pd )
{
    int             i;
    volatile Uint32 *pRegAddr;
    Uint32          *pStatAddr;
    Uint32          statval;

    pRegAddr = &EMAC_REGS->RXGOODFRAMES;
    pStatAddr = (Uint32 *)(&pd->Stats);

    /*
     * There are "EMAC_NUMSTATS" statistics registers
     * Note that when GMIIEN is set in MACCONTROL, these registers
     * are "write to decrement".
     */
    for (i = 0; i < EMAC_NUMSTATS; i++)
    {
        statval = *pRegAddr;
        *pRegAddr++ = statval;
        statval += *pStatAddr;
        *pStatAddr++ = statval;
    }
}
/** ============================================================================
 *  @n@b emacEnqueueTx()
 *
 *  @b Description
 *  @n Enqueue a TX packet and restart transmitter as needed
 *
 *  @b Arguments
 *  @verbatim
        pq  pointer to Channel descriptor
    @endverbatim
 *
 *  <b> Return Value </b>  None
 *
 *  <b> Pre Condition </b>
 *  @n  None
 *
 *  <b> Post Condition </b>
 *  @n  Enqueue a TX packet and restart transmitter as needed
 *
 *  @b Example
 *  @verbatim
        EMAC_DescCh *pdc;

        emacEnqueueTx ( pdc );
    @endverbatim
 * ============================================================================
 */
static void emacEnqueueTx(EMAC_DescCh *pdc)
{
    EMAC_Desc       *pDescOrg;
    EMAC_Desc       *pDescThis;
    EMAC_Pkt        *pPkt;
    Uint32          PktFrags;
    Uint32          CountOrg;
    volatile Uint32 *pRegAddr;

    /*
     * We need to be careful that we don't post half a packet to
     * the list. Otherwise; we just fill in as much packet descriptor
     * stuff as we can.
     */
    pDescOrg = pdc->pDescWrite;
    CountOrg = pdc->DescCount;

    /* Try to post any waiting packets */
    while (pdc->WaitQueue.Count)
    {
        /* See if we have enough room for a new packet */
        pPkt = pdc->WaitQueue.pHead;
        PktFrags = pPkt->PktFrags;

        /* If we don't have room, break out */
        if ((PktFrags+pdc->DescCount) > pdc->DescMax)
            break;

        /* The next packet will fit, post it. */
        while (PktFrags)
        {
            /* Pop the next frag off the wait queue */
            pPkt = pqPop(&pdc->WaitQueue);

            /* Assign the pointer to "this" desc */
            pDescThis = pdc->pDescWrite;

            /* Move the write pointer and bump count */
            if (pdc->pDescWrite == pdc->pDescLast)
                pdc->pDescWrite = pdc->pDescFirst;
            else
                pdc->pDescWrite++;
            pdc->DescCount++;

            /*
             * If this is the last frag, the forward pointer is NULL
             * Otherwise; this desc points to the next frag's desc
             */
            if (PktFrags == 1)
                pDescThis->pNext = 0;
            else
                pDescThis->pNext = pdc->pDescWrite;

            pDescThis->pBuffer   = pPkt->pDataBuffer + pPkt->DataOffset;
            pDescThis->BufOffLen = pPkt->ValidLen;

            if (pPkt->Flags & EMAC_PKT_FLAGS_SOP)
                pDescThis->PktFlgLen = ((pPkt->Flags&
                                       (EMAC_PKT_FLAGS_SOP|EMAC_PKT_FLAGS_EOP))
                                       |pPkt->PktLength|EMAC_DSC_FLAG_OWNER);
            else
                pDescThis->PktFlgLen = (pPkt->Flags&EMAC_PKT_FLAGS_EOP)
                                       |EMAC_DSC_FLAG_OWNER;

            /* Enqueue this frag onto the desc queue */
            pqPush(&pdc->DescQueue, pPkt);
            PktFrags--;
        }
    }

    /* If we posted anything, chain on the list or start the transmitter */
    if (CountOrg != pdc->DescCount)
    {
        if (CountOrg)
        {
            /*
             * Transmitter is already running. Just tack this packet on
             * to the end of the list (we need to "back up" one descriptor)
             */
            if (pDescOrg == pdc->pDescFirst)
                pDescThis = pdc->pDescLast;
            else
                pDescThis = pDescOrg - 1;
            pDescThis->pNext = pDescOrg;
        }
        else
        {
            /* Transmitter is not running, start it up */
            pRegAddr = &EMAC_REGS->TX0HDP;
            *(pRegAddr + pdc->ChannelIndex) = (Uint32)pDescOrg;
        }
    }
}
/** ============================================================================
 *  @n@b emacDequeueTx()
 *
 *  @b Description
 *  @n Dequeue all completed TX packets and return buffers to application
 *
 *  @b Arguments
 *  @verbatim
        pdc         pointer to channel descriptor
        pDescAck    pointer to Descriptor object
    @endverbatim
 *
 *  <b> Return Value </b>  None
 *
 *  <b> Pre Condition </b>
 *  @n  None
 *
 *  <b> Post Condition </b>
 *  @n  Dequeue all completed TX packets and return buffers to application
 *
 *  @b Example
 *  @verbatim
        EMAC_DescCh *pdc;
        EMAC_Desc *pDescAck;

        emacDequeueTx ( pdc, pDescAck );
    @endverbatim
 * ============================================================================
 */
static void emacDequeueTx(EMAC_DescCh *pdc, EMAC_Desc *pDescAck)
{
    EMAC_Pkt        *pPkt;
    Uint32          PktFlgLen;
    register Uint32   i;
    register Uint32   j = (Uint32)pdc->pDescRead;
    volatile Uint32 *pRegAddr;


    /* Get the status of the ACK descriptor */
    PktFlgLen = pDescAck->PktFlgLen;

    /* Calc the new "Read" descriptor */
    if (pDescAck == pdc->pDescLast)
        pdc->pDescRead = pdc->pDescFirst;
    else
        pdc->pDescRead = pDescAck+1;

    i = (Uint32)pdc->pDescRead;

    /* Turn i into a descriptor count */
    if( j < i )
        i = (i-j)/sizeof(EMAC_Desc);
    else
        i = pdc->DescMax - ((j-i)/sizeof(EMAC_Desc));

    pdc->DescCount-=i;

    /* Pop & Free Buffers 'till the last Descriptor */
    while (i--)
    {
        /* Recover the buffer and free it */
        pPkt = pqPop(&pdc->DescQueue);
        if (pPkt)
            (*localDev.Config.pfcbFreePacket)(pdc->pd->hApplication,pPkt);
    }

    /* If the transmitter stopped and we have more descriptors, then restart */
    if ((PktFlgLen & EMAC_DSC_FLAG_EOQ) && pdc->DescCount)
    {
        pRegAddr = &EMAC_REGS->TX0HDP;
        *(pRegAddr + pdc->ChannelIndex) = (Uint32)pdc->pDescRead;
    }

    /* Try to post any waiting TX packets */
    if (pdc->WaitQueue.Count)
        emacEnqueueTx(pdc);
}

/** ============================================================================
 *  @n@b emacEnqueueRx()
 *
 *  @b Description
 *  @n Fill any empty RX descriptors with new buffers from the application
 *
 *  @b Arguments
 *  @verbatim
        pdc         pointer to Descriptor object
        fRestart    re-fill packet
    @endverbatim
 *
 *  <b> Return Value </b>  None
 *
 *  <b> Pre Condition </b>
 *  @n  None
 *
 *  <b> Post Condition </b>
 *  @n  Fill any empty RX descriptors with new buffers from the application.
 *
 *  @b Example
 *  @verbatim
        EMAC_DescCh  *pdc;
        Uint32         fRestart;

        emacEnqueueRx( pdc, fRestart );
    @endverbatim
 * ============================================================================
 */
static void emacEnqueueRx(EMAC_DescCh *pdc, Uint32 fRestart)
{
    EMAC_Pkt        *pPkt;
    EMAC_Desc       *pDesc;
    Uint32            CountOrg;
    volatile Uint32  *pRegAddr;

    /* Keep the old count around */
    CountOrg = pdc->DescCount;

    /* Fill RX Packets Until Full */
    while (pdc->DescCount < pdc->DescMax)
    {
        /* Get a buffer from the application */
        pPkt = (*localDev.Config.pfcbGetPacket)(pdc->pd->hApplication);

        /* If no more buffers are available, break out of loop */
        if (!pPkt)
            break;

        /* Fill in the descriptor for this buffer */
        pDesc = pdc->pDescWrite;

        /* Move the write pointer and bump count */
        if (pdc->pDescWrite == pdc->pDescLast)
            pdc->pDescWrite = pdc->pDescFirst;
        else
            pdc->pDescWrite++;
        pdc->DescCount++;

        /* Supply buffer pointer with application supplied offset */
        pDesc->pNext     = 0;
        pDesc->pBuffer   = pPkt->pDataBuffer + pPkt->DataOffset;
        pDesc->BufOffLen = localDev.PktMTU;
        pDesc->PktFlgLen = EMAC_DSC_FLAG_OWNER;

        /* Make the previous buffer point to us */
        if (pDesc == pdc->pDescFirst)
             pdc->pDescLast->pNext = pDesc;
        else
            (pDesc-1)->pNext = pDesc;

        /* Push the packet buffer on the local descriptor queue */
        pqPush(&pdc->DescQueue, pPkt);
    }

    /* Restart RX if we had ran out of descriptors and got some here */
    if (fRestart && !CountOrg && pdc->DescCount)
    {
        pRegAddr = &EMAC_REGS->RX0HDP;
        *(pRegAddr + pdc->ChannelIndex)= (Uint32)pdc->pDescRead;

    }
    
}
/** ============================================================================
 *  @n@b emacDequeueRx()
 *
 *  @b Description
 *  @n Dequeue all completed RX packets and give buffers to application
 *
 *  @b Arguments
 *  @verbatim
        pdc         pointer to descriptor channel object
        pDescAck    pointer to the acknowledge
    @endverbatim
 *
 *  <b> Return Value </b>  None
 *
 *  <b> Pre Condition </b>
 *  @n  None
 *
 *  <b> Post Condition </b>
 *  @n  Dequeue all completed RX packets and give buffers to application
 *
 *  @b Example
 *  @verbatim
        EMAC_DescCh *pdc;
        EMAC_Desc   *pDescAck;

        emacDequeueRx( pdc, pDescAck );
    @endverbatim
 * ============================================================================
 */
static void emacDequeueRx(EMAC_DescCh *pdc, EMAC_Desc *pDescAck)
{
    EMAC_Pkt    *pPkt;
    EMAC_Pkt    *pPktNew;
    EMAC_Desc   *pDescNewRxFirst;
    EMAC_Desc   *pDescNewRxLast=0;
    EMAC_Desc   *pTemp;
    Uint32      tmp;
    Uint32      PktFlgLen;
    volatile Uint32 *pRegAddr;

    /* Remember the first new descriptor slot */
    pDescNewRxFirst = pdc->pDescWrite;

    /*
     * Pop & Free Buffers 'till the last Descriptor
     * One thing we know for sure is that all the decriptors from
     * the read pointer to pDescAsk are linked to each other via
     * their pNext field.
    */
    for (tmp = 1; tmp; )
    {
        /* Get the status of this descriptor */
        PktFlgLen = pdc->pDescRead->PktFlgLen;

        /* Recover the buffer and free it */
        pPkt = pqPop(&pdc->DescQueue);
        if (pPkt)
        {
            /* Fill in the necessary packet header fields */
            pPkt->Flags = PktFlgLen & 0xFFFF0000;
            pPkt->ValidLen = pPkt->PktLength = PktFlgLen & 0xFFFF;
            pPkt->PktChannel = pdc->ChannelIndex; 
            pPkt->PktFrags = 1;

            /* Pass the packet to the application */
            pPktNew = (*localDev.Config.pfcbRxPacket)
                                           (pdc->pd->hApplication,pPkt);
        }

        /* See if this was the last buffer */
        if (pdc->pDescRead == pDescAck)
            tmp = 0;

        /* Move the read pointer and decrement count */
        if (pdc->pDescRead == pdc->pDescLast)
            pdc->pDescRead = pdc->pDescFirst;
        else
            pdc->pDescRead++;
        pdc->DescCount--;

        /* See if we got a replacement packet */
        if(pPktNew)
        {
            /* We know we can immediately queue this packet */

            /* Fill in the descriptor for this buffer */
            pDescNewRxLast = pdc->pDescWrite;

            /* Move the write pointer and bump count */
            if( pdc->pDescWrite == pdc->pDescLast )
                pdc->pDescWrite = pdc->pDescFirst;
            else
                pdc->pDescWrite++;
            pdc->DescCount++;

            /* Supply buffer pointer with application supplied offset */
            pDescNewRxLast->pBuffer   = pPktNew->pDataBuffer + pPktNew->DataOffset;
            pDescNewRxLast->BufOffLen = localDev.PktMTU;
            pDescNewRxLast->PktFlgLen = EMAC_DSC_FLAG_OWNER;

            /* Push the packet buffer on the local descriptor queue */
            pqPush(&pdc->DescQueue, pPktNew);
        }
    }

    /*
     * If we added descriptors, make the pNext of the last NULL, and
     * make the previous descriptor point to the new list we added.
     */
    if (pDescNewRxLast)
    {
        pDescNewRxLast->pNext = 0;

        /* Make the previous buffer point to us */
        if (pDescNewRxFirst == pdc->pDescFirst)
            pTemp = pdc->pDescLast;
        else
            pTemp = pDescNewRxFirst-1;

        /*
         * If these pointers wrapped, the RX engine is stopped
         * Otherwise; tack the new list to the old
         */
        if (pTemp != pDescNewRxLast)
            pTemp->pNext = pDescNewRxFirst;
    }

    /* If the receiver stopped and we have more descriptors, then restart */
    if ((PktFlgLen & EMAC_DSC_FLAG_EOQ) && pdc->DescCount)
    {
        pRegAddr = &EMAC_REGS->RX0HDP;
        *(pRegAddr + pdc->ChannelIndex) = (Uint32)pdc->pDescRead;
    }   
}


/*-----------------------------------------------------------------------*\
 * STANDARD API FUNCTIONS
 *
 * Note on Exclusion (Serialization):
 *   The application is charged with verifying that only one of the
 * following API calls may only be executing at a given time across
 * all threads and all interrupt functions.
 *
 *-----------------------------------------------------------------------*/


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
 *  <b> Return Value </b>  Uint32
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
Uint32 EMAC_enumerate(void)
{
    return (1);
}


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
Uint32 EMAC_open(int physicalIndex, Handle hApplication,
                 EMAC_Config *pEMACConfig, Handle *phEMAC)
{
    int              i, index, tmp, j, tmp1, tmp2;
    volatile Uint32  *pRegAddr;
    Uint32           tmpval;
    EMAC_Pkt         *pPkt;
    Uint32           utemp1;
    EMAC_Desc        *pDesc;

    /* We only handle a single instance */
    if (physicalIndex != 1)
        return (EMAC_ERROR_INVALID);

    /* If the device is alread open, return an error */
    if (openFlag)
        return( EMAC_ERROR_ALREADY );

    /*
     *  Validate the supplied configuration structure
     */
    if (!pEMACConfig || !phEMAC)
        return (EMAC_ERROR_INVALID);

    /* Rx pool must be at least 8 to 192 */
    if (pEMACConfig->RxMaxPktPool < 8 || pEMACConfig->RxMaxPktPool > 192)
        return( EMAC_ERROR_INVALID );

    /* MAC address must be supplied and not a multicast address */
	for (index=0; index<3; index++){
    	if( pEMACConfig->MacAddr[index][0] & 1 )
        	return( EMAC_ERROR_INVALID );
	    for (i = 0; (i < 6) && (!(pEMACConfig->MacAddr[index][i])); i++)
    	if (i == 6)
        	return (EMAC_ERROR_INVALID);
	}

    /* Tx Channels must be 1-8 */
    for (i = 0; i < 3; i++) {
        if (pEMACConfig->ChannelInfo[i].TxChannels == 0)
            continue;
        if (pEMACConfig->ChannelInfo[i].TxChannels < 1 || pEMACConfig->ChannelInfo[i].TxChannels > 8)
            return (EMAC_ERROR_INVALID);
    }

    /* Rx Channels must be 1-8 */
    for (i = 0; i < 3; i++) {
        if (pEMACConfig->ChannelInfo[i].RxChannels == 0)
            continue;
        if (pEMACConfig->ChannelInfo[i].RxChannels < 1 || pEMACConfig->ChannelInfo[i].RxChannels > 8)
            return (EMAC_ERROR_INVALID);
    }

    /* Callback functions must be supplied */
    if (!pEMACConfig->pfcbGetPacket || !pEMACConfig->pfcbFreePacket ||
            !pEMACConfig->pfcbRxPacket || !pEMACConfig->pfcbStatus ||
            !pEMACConfig->pfcbStatistics)
        return (EMAC_ERROR_INVALID);

    if (pEMACConfig->UseMdio)
    {
        /* MDIO settings must be valid - at least one bit must be set */
        if (!(pEMACConfig->MdioModeFlags & (MDIO_MODEFLG_AUTONEG |
                MDIO_MODEFLG_HD10  | MDIO_MODEFLG_FD10 |
                MDIO_MODEFLG_HD100 | MDIO_MODEFLG_FD100 |
                MDIO_MODEFLG_FD1000)))
            return (EMAC_ERROR_INVALID);
    }
    /*
     * Init the instance structure
     */

    /* Default everything in our instance structure to zero */
    memset(&localDev, 0, sizeof(EMAC_Device));

    /* Set the hApplication and RxFilter */
    localDev.hApplication = hApplication;
    localDev.RxFilter     = EMAC_RXFILTER_NOTHING;

    /* Setup the new configuration */
    localDev.Config = *pEMACConfig;


    /*
     *  Initialize the EMAC and MDIO devices
     */
    //ECTL_REGS->EWCTL = CSL_FMKT(ECTL_EWCTL_INTEN, DISABLE ); 
    /* Disable wrapper interrupt pacing */
	ECTL_REGS->INT_CONTROL = 0x00;
    /*Give soft reset to Wrapper*/
	ECTL_REGS->SOFT_RESET = 0x01; 
    while (ECTL_REGS->SOFT_RESET != 0x00000000);  /* Wait until reset has occured */ 

    /* Give soft reset to EMAC */
    EMAC_REGS->SOFTRESET = 0x00000001;
    while (EMAC_REGS->SOFTRESET != 0x00000000); /* Wait until reset has occured */ 

    if (localDev.Config.UseMdio)
        /* Start the MII Configuration */
        localDev.hMDIO = MDIO_open(pEMACConfig->MdioModeFlags);
    
    /*
     *  Setup the EMAC
     */

    /* Reset MAC Control */
    EMAC_REGS->MACCONTROL = 0 ;


    /* Must manually init HDPs to NULL */
    pRegAddr = &EMAC_REGS->TX0HDP;
    for (i = 0; i < 8; i++)
        *pRegAddr++ = 0;
    pRegAddr = &EMAC_REGS->RX0HDP;
    for (i = 0; i < 8; i++)
        *pRegAddr++ = 0;

    /*
     * While GMIIEN is clear in MACCONTROL, we can write directly to
     * the statistics registers (there are "EMAC_NUMSTATS" of them).
     */
    pRegAddr = &EMAC_REGS->RXGOODFRAMES;
    for (i=0; i<EMAC_NUMSTATS; i++)
        *pRegAddr++ = 0;

    /* Initialize the RAM locations */
    for (i = 0; i < 32; i++)
    {
        EMAC_REGS->MACINDEX = i;
        EMAC_REGS->MACADDRHI = 0;
        EMAC_REGS->MACADDRLO = 0;
    }

    /* Setup device MAC addresses */
    tmp=0;
    for (index = 0; index < 3; index++) {
        for (j = 0; j < localDev.Config.ChannelInfo[index].RxChannels; j++) 
        {
            EMAC_REGS->MACINDEX = (j + tmp); //0x0 ;

            tmpval = 0;
            for (i = 3; i >= 0; i--)
                tmpval = (tmpval << 8) | localDev.Config.MacAddr[index][i];

            EMAC_REGS->MACADDRHI = tmpval;

            tmpval = localDev.Config.MacAddr[index][5];// + (2 * index);
            EMAC_REGS->MACADDRLO = CSL_FMKT(EMAC_MACADDRLO_VALID, VALID) |
                                   CSL_FMKT(EMAC_MACADDRLO_MATCHFILT, MATCH) |
                                   CSL_FMK(EMAC_MACADDRLO_CHANNEL, (j + tmp)) |
                                   (tmpval << 8) |
                                   localDev.Config.MacAddr[index][4];
        }
        tmp += j;
    }
    /*
     * Setup Special Receive Conditions (loopback, error frames, etc)
     */

    /* For us buffer offset will always be zero */
    EMAC_REGS->RXBUFFEROFFSET = 0;

    /* Reset RX (M)ulticast (B)roadcast (P)romiscuous Enable register */
    EMAC_REGS->RXMBPENABLE = 0;
    EMAC_REGS->MACHASH1 = 0;
    EMAC_REGS->MACHASH2 = 0;

    /* Clear Unicast RX on channel 0-7 */
    EMAC_REGS->RXUNICASTCLEAR = 0xFF;

    /* Set the pass RX CRC mode and adjust max buffer accordingly */
    if (localDev.Config.ModeFlags & EMAC_CONFIG_MODEFLG_RXCRC)
    {
        CSL_FINST(EMAC_REGS->RXMBPENABLE, EMAC_RXMBPENABLE_RXPASSCRC, INCLUDE);
        localDev.PktMTU = 1518;
    }
    else
        localDev.PktMTU = 1514;

    /* If PASSERROR is set, enable both ERROR and short frames */
    if (localDev.Config.ModeFlags & EMAC_CONFIG_MODEFLG_PASSERROR){
        CSL_FINST(EMAC_REGS->RXMBPENABLE, EMAC_RXMBPENABLE_RXCEFEN, ENABLE);
        CSL_FINST(EMAC_REGS->RXMBPENABLE, EMAC_RXMBPENABLE_RXCSFEN, ENABLE);
    }
    /* If PASSCONTROL is set, enable control frames */
    if (localDev.Config.ModeFlags & EMAC_CONFIG_MODEFLG_PASSCONTROL)
        CSL_FINST(EMAC_REGS->RXMBPENABLE, EMAC_RXMBPENABLE_RXCMFEN, ENABLE);
    if (localDev.Config.ModeFlags & EMAC_CONFIG_MODEFLG_PASSALL)
        CSL_FINST(EMAC_REGS->RXMBPENABLE, EMAC_RXMBPENABLE_RXCAFEN, ENABLE);
    if (localDev.Config.ModeFlags & EMAC_CONFIG_MODEFLG_RXQOS)
        CSL_FINST(EMAC_REGS->RXMBPENABLE, EMAC_RXMBPENABLE_RXQOSEN, ENABLE);
    if (localDev.Config.ModeFlags & EMAC_CONFIG_MODEFLG_RXNOCHAIN)
        CSL_FINST(EMAC_REGS->RXMBPENABLE, EMAC_RXMBPENABLE_RXNOCHAIN, ENABLE);
    
    /* Set the channel configuration to priority if requested */
    if (localDev.Config.ModeFlags & EMAC_CONFIG_MODEFLG_CHPRIORITY)
        CSL_FINST(EMAC_REGS->MACCONTROL, EMAC_MACCONTROL_TXPTYPE, CHANNELPRI);

    /* Set MAC loopback if requested */
    if (localDev.Config.ModeFlags & EMAC_CONFIG_MODEFLG_MACLOOPBACK)
        CSL_FINST(EMAC_REGS->MACCONTROL, EMAC_MACCONTROL_LOOPBACK, ENABLE);
    /* Set full duplex mode if requested */
    if (localDev.Config.ModeFlags & EMAC_CONFIG_MODEFLG_FULLDUPLEX)
        CSL_FINST(EMAC_REGS->MACCONTROL, EMAC_MACCONTROL_FULLDUPLEX, ENABLE);
    /* Enable gigabit transfer if requested */
    if (localDev.Config.ModeFlags & EMAC_CONFIG_MODEFLG_GIGABIT)
        CSL_FINST(EMAC_REGS->MACCONTROL, EMAC_MACCONTROL_GIG, ENABLE); 
    /* Enable rx offset/length blocing if requested */
    if (localDev.Config.ModeFlags & EMAC_CONFIG_MODEFLG_RXOFFLENBLOCK)
        CSL_FINST(EMAC_REGS->MACCONTROL, EMAC_MACCONTROL_RXOFFLENBLOCK, BLOCK); 
    /* Use rx owership one if requested */
    if (localDev.Config.ModeFlags & EMAC_CONFIG_MODEFLG_RXOWNERSHIP)
        CSL_FINST(EMAC_REGS->MACCONTROL, EMAC_MACCONTROL_RXOWNERSHIP, ONE); 
    /* Enable rx fifo flow control if requested */
    if (localDev.Config.ModeFlags & EMAC_CONFIG_MODEFLG_RXFIFOFLOWCNTL)
        CSL_FINST(EMAC_REGS->MACCONTROL, EMAC_MACCONTROL_RXFIFOFLOWEN, ENABLE); 
    /* Enable IDLE command bit if requested */
    if (localDev.Config.ModeFlags & EMAC_CONFIG_MODEFLG_CMDIDLE)
        CSL_FINST(EMAC_REGS->MACCONTROL, EMAC_MACCONTROL_CMDIDLE, ENABLE); 
    /* Enable tx short gap if requested */
    if (localDev.Config.ModeFlags & EMAC_CONFIG_MODEFLG_TXSHORTGAPEN)
        CSL_FINST(EMAC_REGS->MACCONTROL, EMAC_MACCONTROL_TXSHORTGAPEN, ENABLE); 
    /* Enable tx pace if requested */
    if (localDev.Config.ModeFlags & EMAC_CONFIG_MODEFLG_TXPACE)
        CSL_FINST(EMAC_REGS->MACCONTROL, EMAC_MACCONTROL_TXPACE, ENABLE); 
    /* Enable tx flow control */
    if (localDev.Config.ModeFlags & EMAC_CONFIG_MODEFLG_TXFLOWCNTL)
        CSL_FINST(EMAC_REGS->MACCONTROL, EMAC_MACCONTROL_TXFLOWEN, ENABLE); 
    /* Enable rx  buffer flow control if requested */
    if (localDev.Config.ModeFlags & EMAC_CONFIG_MODEFLG_RXBUFFERFLOWCNTL)
        CSL_FINST(EMAC_REGS->MACCONTROL, EMAC_MACCONTROL_RXBUFFERFLOWEN, ENABLE); 
                                                                           


    /*
     * Enable TX and RX channel interrupts (set mask bits)
     * Enable Host interrupts
     */
    EMAC_REGS->RXINTMASKCLEAR = 0xFF;
    EMAC_REGS->TXINTMASKCLEAR = 0xFF;
    
    for (index = 0; index < 3; index++)
        TotalRxChannels += localDev.Config.ChannelInfo[index].RxChannels;

    EMAC_REGS->RXINTMASKSET = ((1 << TotalRxChannels) - 1);

    for (index = 0; index < 3; index++)
        TotalTxChannels += localDev.Config.ChannelInfo[index].TxChannels;

    EMAC_REGS->TXINTMASKSET = ((1 << TotalTxChannels) - 1);
   
    EMAC_REGS->MACINTMASKSET = CSL_FMK(EMAC_MACINTMASKSET_HOSTMASK, 1) |
                               CSL_FMK(EMAC_MACINTMASKSET_STATMASK, 1);
    
    /*
     * Setup Receive Buffers
     */

    /*
     * We give the first descriptors to RX. The rest of the descriptors
     * will be divided evenly among the TX channels. Odds are this
     * will leave TX with a very large number of TX descriptors, but
     * we'll only use what we need (driven from the application send
     * requests). The RX descriptors are always kept fully populated.
     */

    /* Pointer to first descriptor to use on RX */
    pDesc = (EMAC_Desc *)CSL_EMAC_DSC_BASE_ADDR;   

    /* Number of descriptors for RX channel */
    utemp1 = (localDev.Config.RxMaxPktPool);

    /* Init all Rx channels in use */
    for (i = 0; i < (int)TotalRxChannels; i++) {
        localDev.RxCh[i].pd         = &localDev;
        localDev.RxCh[i].DescMax    = utemp1;
        localDev.RxCh[i].pDescFirst = pDesc;
        localDev.RxCh[i].pDescLast  = pDesc + (utemp1 - 1);
        localDev.RxCh[i].pDescRead  = pDesc;
        localDev.RxCh[i].pDescWrite = pDesc;

        pDesc += utemp1;

        /* Fill the descriptor table */
        emacEnqueueRx(&localDev.RxCh[i], 0);
    }
    
    /*
     * If we didn't get the number of descriptor buffers that the
     * application said we should, then the app lied to us. This is
     * bad because we'll be constantly calling to the app to fill
     * up our buffers. So we'll close now to make the problem
     * obvious.
     */
    for (i = 0; i < (int)TotalRxChannels; i++) {
        if (localDev.RxCh[i].DescCount < utemp1)
        {
            /* Free all RX descriptors */
            while (pPkt = pqPop( &localDev.RxCh[i].DescQueue))
                (*localDev.Config.pfcbFreePacket)(localDev.hApplication, pPkt);

            if (localDev.Config.UseMdio)
                /* Close the MDIO Module */
                MDIO_close(localDev.hMDIO);    
            
            /* Return the error condition */
            return (EMAC_ERROR_INVALID);
        }
    }
    /*
     * Setup Transmit Buffers
     */

    /* Pointer to first descriptor to use on TX */
    pDesc += utemp1;

    /* Number of descriptors (max) per channel */
    utemp1 = (CSL_EDMA_DSC_ENTRY_COUNT-utemp1)/TotalTxChannels;

    /* Init all TX channels in use */
    for (i = 0; i < (int)TotalTxChannels; i++)
    {
        localDev.TxCh[i].pd         = &localDev;
        localDev.TxCh[i].DescMax    = utemp1;
        localDev.TxCh[i].pDescFirst = pDesc;
        localDev.TxCh[i].pDescLast  = pDesc + (utemp1 - 1);
        localDev.TxCh[i].pDescRead  = pDesc;
        localDev.TxCh[i].pDescWrite = pDesc;

        pDesc += utemp1;
    }


    /*
     * Enable RX, TX, and GMII
     *
     * Note in full duplex mode we also need to set the FULLDUPLEX
     * bit in MACCRONTROL. However, we don't know what to set until
     * we have a link. Also, we must be able to dynamically change
     * this bit if the cable is unplugged and re-linked with a different
     * duplex.
     */
    CSL_FINST(EMAC_REGS->TXCONTROL, EMAC_TXCONTROL_TXEN, ENABLE);
    CSL_FINST(EMAC_REGS->RXCONTROL, EMAC_RXCONTROL_RXEN, ENABLE);
    CSL_FINST(EMAC_REGS->MACCONTROL, EMAC_MACCONTROL_GMIIEN, ENABLE);

    /* Startup RX */
    for (i = 0; i < TotalRxChannels; i++)
    {
        pRegAddr = &EMAC_REGS->RX0HDP;
        *(pRegAddr + i) = (Uint32)localDev.RxCh[i].pDescRead;
    }

    /* Validate the device handle */
    localDev.DevMagic = EMAC_DEVMAGIC;

    /* Set the open flag */
    openFlag = 1;

    /* Give a handle back to the caller */
    *phEMAC = &localDev;

    /* Disable global interrupt pacing in wrapper */
    ECTL_REGS->INT_CONTROL = 0x00000000; /* Disable Interrupt Pacing*/
    
    i = localDev.Config.ChannelInfo[0].RxChannels;
    j = localDev.Config.ChannelInfo[0].TxChannels;
    ECTL_REGS->CONTROL[0].C_RX_EN = ((1 << i) - 1);/*Enable core 0, 1 and 2, RX channel 0, 1 and 2 respectively*/ 
    ECTL_REGS->CONTROL[0].C_TX_EN = ((1 << j) - 1);/*Enable core 0, 1 and 2, TX channels*/

    i = localDev.Config.ChannelInfo[1].RxChannels;
    j = localDev.Config.ChannelInfo[1].TxChannels;
    tmp1 = localDev.Config.ChannelInfo[0].RxChannels;
    tmp2 = localDev.Config.ChannelInfo[0].TxChannels;
    ECTL_REGS->CONTROL[1].C_RX_EN = (((1 << i) - 1) << tmp1);/*Enable core 0, 1 and 2, RX channel 0, 1 and 2 respectively*/ 
    ECTL_REGS->CONTROL[1].C_TX_EN = (((1 << j) - 1) << tmp2);/*Enable core 0, 1 and 2, TX channels*/

    i = localDev.Config.ChannelInfo[2].RxChannels;
    j = localDev.Config.ChannelInfo[2].TxChannels;
    tmp1 = TotalRxChannels - i;
    tmp2 = TotalTxChannels - j;
    ECTL_REGS->CONTROL[2].C_RX_EN = (((1 << i) - 1) << tmp1);/*Enable core 0, 1 and 2, RX channel 0, 1 and 2 respectively*/ 
    ECTL_REGS->CONTROL[2].C_TX_EN = (((1 << j) - 1) << tmp2);/*Enable core 0, 1 and 2, TX channels*/
    
    
    /* Return Success */
    return( 0 );
}


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
Uint32 EMAC_close(Handle hEMAC)
{
	EMAC_Device     *pd = (EMAC_Device *)hEMAC;
    Uint32          tmp, index;
    EMAC_Pkt        *pPkt;
    volatile Uint32 *pRegAddr;
    

    /* Validate our handle */
    if (!pd || pd->DevMagic != EMAC_DEVMAGIC)
        return (EMAC_ERROR_INVALID);

    /* Disable EMAC/MDIO interrupts pacing in wrapper */
	ECTL_REGS->INT_CONTROL = 0x00000000; /*Disable Wrapper interrupts pacing */
    for (index = 0; index < 3; index++) {
        ECTL_REGS->CONTROL[index].C_RX_EN = 0x00;/*Disable core 0, RX channel 0*/ 
        ECTL_REGS->CONTROL[index].C_TX_EN = 0x00;/*Disable core 0, TX channel 0*/
    }
    /*
     * The close process consists of tearing down all the active
     * channels (RX and TX) and then waiting for the teardown
     * complete indication from the MAC. Then, all queued packets
     * will be returned.
     */

        /* Teardown RX */
    for (index = 0; index < TotalRxChannels; index++) 
        EMAC_REGS->RXTEARDOWN = index ;
    
        /* Teardown TX channels in use */
    for (index = 0; index < TotalTxChannels; index++)
        EMAC_REGS->TXTEARDOWN = index ;

    /* Only check teardown status if there was no fatal error         */
    /* Otherwise; the EMAC is halted and can’t be shutdown gracefully */
    if (!pd->FatalError)
    {
        /* Wait for the teardown to complete */
        pRegAddr = &EMAC_REGS->RX0CP;
        for (index = 0; index < TotalRxChannels; index++) 
        {
            for (tmp = 0; tmp != 0xFFFFFFFC; tmp = *(pRegAddr + index));
            *(pRegAddr + index) = tmp;
        }

        pRegAddr = &EMAC_REGS->TX0CP;
        for (index=0; index<TotalTxChannels; index++)
        {
            for (tmp = 0; tmp != 0xFFFFFFFC; tmp = *(pRegAddr + index));
            *(pRegAddr + index) = tmp;
        }
    }

    /* Disable RX, TX, and Clear MACCONTROL */
    CSL_FINST(EMAC_REGS->TXCONTROL, EMAC_TXCONTROL_TXEN, DISABLE);
    CSL_FINST(EMAC_REGS->RXCONTROL, EMAC_RXCONTROL_RXEN, DISABLE);
    EMAC_REGS->MACCONTROL = 0;

    /* Free all RX buffers */
    for (index = 0; index < TotalRxChannels; index++)
        while (pPkt = pqPop( &pd->RxCh[index].DescQueue))
            (*pd->Config.pfcbFreePacket)(localDev.hApplication, pPkt);
    
    /* Free all TX buffers */
    for (index = 0; index < TotalTxChannels; index++)
    {
        while (pPkt = pqPop( &pd->TxCh[index].DescQueue))
            (*pd->Config.pfcbFreePacket)(localDev.hApplication, pPkt);
        while ( pPkt = pqPop(&pd->TxCh[index].WaitQueue))
            (*pd->Config.pfcbFreePacket)(localDev.hApplication, pPkt);
    }

    if (pd->Config.UseMdio)
        /* Close the MDIO Module */
        MDIO_close( pd->hMDIO );
    
    /* Invalidate the EMAC handle */
    pd->DevMagic = 0;

    /* Clear the open flag */
    openFlag = 0;

    /* Exit with interrupts still disabled in the wrapper */
    return (0);
}


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
        EMAC_Status pStatus;
        EMAC_Config  ecfg;
        Handle       hEMAC = 0;
        Uint8        coreNum = 0;

        //Open the EMAC peripheral
        EMAC_open(1, (Handle)0x12345678, &ecfg, &hEMAC);

        EMAC_getStatus( hEMAC, &status, coreNum);
    @endverbatim
 * ============================================================================
 */
Uint32 EMAC_getStatus(Handle hEMAC, EMAC_Status *pStatus, Uint8 coreNum)
{
    EMAC_Device  *pd = (EMAC_Device *)hEMAC;
    Uint32        i, tmp;

    /* Validate our handle */
    if (!pd || pd->DevMagic != EMAC_DEVMAGIC || !pStatus)
        return (EMAC_ERROR_INVALID);

    if (pd->Config.UseMdio) 
        /* Get the MDIO status */
        MDIO_getStatus(pd->hMDIO, &pStatus->PhyDev, &pStatus->MdioLinkStatus);

    /* Number of rx packets held */
    tmp = 0;
    for (i = 0; i < pd->Config.ChannelInfo[coreNum].RxChannels; i++) 
        tmp += pd->RxCh[i].DescCount;

    pStatus->RxPktHeld = tmp;

    /* Number of tx packets held */
    tmp = 0;
    for (i = 0; i < pd->Config.ChannelInfo[coreNum].TxChannels; i++)
    {
        tmp += pd->TxCh[i].DescCount;
        tmp += pd->TxCh[i].WaitQueue.Count;
    }
    pStatus->TxPktHeld = tmp;

    /* Fatal error value */
    pStatus->FatalError = pd->FatalError;

    return (0);
}


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
Uint32 EMAC_setReceiveFilter(Handle hEMAC, Uint32 ReceiveFilter)
{
    EMAC_Device  *pd = (EMAC_Device *)hEMAC;
//    Uint32  index;

    /* Validate our handle */
    if (!pd || pd->DevMagic != EMAC_DEVMAGIC || ReceiveFilter > EMAC_RXFILTER_ALL)
        return (EMAC_ERROR_INVALID);

    /*
     * The following code relies on the numeric relation of the filter
     * value such that the higher filter values receive more types of
     * packets.
     */

    /* Disable Section */
    if (ReceiveFilter < EMAC_RXFILTER_ALL)
        CSL_FINST(EMAC_REGS->RXMBPENABLE, EMAC_RXMBPENABLE_RXCAFEN, DISABLE);
    if (ReceiveFilter < EMAC_RXFILTER_ALLMULTICAST)
    {
        EMAC_REGS->MACHASH1 = pd->MacHash1;
        EMAC_REGS->MACHASH2 = pd->MacHash2;
    }
    if (ReceiveFilter < EMAC_RXFILTER_MULTICAST)
        CSL_FINST(EMAC_REGS->RXMBPENABLE, EMAC_RXMBPENABLE_RXMULTEN, DISABLE);
    if (ReceiveFilter < EMAC_RXFILTER_BROADCAST)
        CSL_FINST(EMAC_REGS->RXMBPENABLE, EMAC_RXMBPENABLE_RXBROADEN, DISABLE);
    if (ReceiveFilter < EMAC_RXFILTER_DIRECT)
    {
        EMAC_REGS->RXUNICASTCLEAR = (1 << TotalRxChannels) - 1;
    }

    /* Enable Section */
    if (ReceiveFilter >= EMAC_RXFILTER_DIRECT) 
    {
        EMAC_REGS->RXUNICASTSET = (1 << TotalRxChannels) - 1;
    }
        
    if (ReceiveFilter >= EMAC_RXFILTER_BROADCAST)
        CSL_FINST(EMAC_REGS->RXMBPENABLE, EMAC_RXMBPENABLE_RXBROADEN, ENABLE);
    if (ReceiveFilter >= EMAC_RXFILTER_MULTICAST)
        CSL_FINST(EMAC_REGS->RXMBPENABLE, EMAC_RXMBPENABLE_RXMULTEN, ENABLE);
    if (ReceiveFilter >= EMAC_RXFILTER_ALLMULTICAST)
    {
        EMAC_REGS->MACHASH1 = 0xffffffff;
        EMAC_REGS->MACHASH1 = 0xffffffff;
    }
    if (ReceiveFilter == EMAC_RXFILTER_ALL)
        CSL_FINST(EMAC_REGS->RXMBPENABLE, EMAC_RXMBPENABLE_RXCAFEN, ENABLE);

    pd->RxFilter = ReceiveFilter;
    return (0);
}


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
Uint32 EMAC_getReceiveFilter(Handle hEMAC, Uint32 *pReceiveFilter)
{
    EMAC_Device  *pd = (EMAC_Device *)hEMAC;

    /* Validate our handle */
    if (!pd || pd->DevMagic != EMAC_DEVMAGIC || !pReceiveFilter)
        return (EMAC_ERROR_INVALID);

    *pReceiveFilter = pd->RxFilter;
    return (0);
}


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
Uint32 EMAC_getStatistics(Handle hEMAC, EMAC_Statistics *pStatistics)
{
    EMAC_Device  *pd = (EMAC_Device *)hEMAC;

    /* Validate our handle */
    if(!pd || pd->DevMagic != EMAC_DEVMAGIC || !pStatistics)
        return (EMAC_ERROR_INVALID);

    /* Update the stats */
    emacUpdateStats(pd);

    /* Copy the updated stats to the application */
    *pStatistics = pd->Stats;

    /* Clear our copy */
    memset(&pd->Stats, 0, sizeof(EMAC_Statistics));

    return (0);
}


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
Uint32 EMAC_setMulticast(Handle hEMAC, Uint32 AddrCnt, Uint8 *pMCastList)
{
    EMAC_Device  *pd = (EMAC_Device *)hEMAC;
    Uint32        tmp1,tmp2;
    Uint8       HashVal,tmpval;

    /* Validate our handle */
    if (!pd || pd->DevMagic != EMAC_DEVMAGIC || (AddrCnt && !pMCastList))
        return (EMAC_ERROR_INVALID);

    /* Clear the hash bits */
    pd->MacHash1 = 0;
    pd->MacHash2 = 0;

    /* For each address in the list, hash and set the bit */
    for (tmp1=0; tmp1<AddrCnt; tmp1++)
    {
        HashVal=0;

        for (tmp2=0; tmp2 < 2; tmp2++)
        {
            tmpval = *pMCastList++;
            HashVal ^= (tmpval >> 2) ^ (tmpval << 4);
            tmpval = *pMCastList++;
            HashVal ^= (tmpval >> 4) ^ (tmpval << 2);
            tmpval = *pMCastList++;
            HashVal ^= (tmpval >> 6) ^ (tmpval);
        }

        if (HashVal & 0x20)
            pd->MacHash2 |= (1 << (HashVal & 0x1f));
        else
            pd->MacHash1 |= (1 << (HashVal & 0x1f));
    }

    /* We only write the hash table if the filter setting allows */
    if (pd->RxFilter < EMAC_RXFILTER_ALLMULTICAST)
    {
        EMAC_REGS->MACHASH1 = pd->MacHash1;
        EMAC_REGS->MACHASH2 = pd->MacHash2;
    }

    return (0);
}


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
Uint32 EMAC_sendPacket(Handle hEMAC, EMAC_Pkt *pPkt)
{
    EMAC_Device  *pd = (EMAC_Device *)hEMAC;
    Uint32        fragcnt,pktlen;
    EMAC_Pkt     *pPktLast;
    EMAC_DescCh  *pdc;

    /* Validate our handle */
    if (!pd || pd->DevMagic != EMAC_DEVMAGIC || !pPkt)
        return (EMAC_ERROR_INVALID);

    /* Do some packet validation */
    if (!(pPkt->Flags & EMAC_PKT_FLAGS_SOP))
        return (EMAC_ERROR_BADPACKET);

    if (pPkt->PktChannel >= TotalTxChannels)// pd->Config.ChannelInfo[coreNum].TxChannels)
        return (EMAC_ERROR_BADPACKET);

    if (pPkt->PktLength < 14 || pPkt->PktLength > pd->PktMTU)
        return (EMAC_ERROR_BADPACKET);

    /* Count the number of frags in this packet */
    fragcnt = pPkt->PktFrags;
    pktlen  = pPkt->PktLength;
   
    pPktLast = pPkt;
    while (!(pPktLast->Flags & EMAC_PKT_FLAGS_EOP))
    {
        if (!pPktLast->pNext)
            return (EMAC_ERROR_INVALID);
        pktlen -= pPktLast->ValidLen;
        pPktLast = pPktLast->pNext;
        fragcnt++;

        /* At this point we can't have another SOP */
        if (pPktLast->Flags & EMAC_PKT_FLAGS_SOP)
            return (EMAC_ERROR_INVALID);
    }

    /* Make sure PktLength and ValidLen agree */
    if (pktlen != pPkt->ValidLen)
        return (EMAC_ERROR_BADPACKET);

    /* The final packet frag must be the last in the list */
    if (pPktLast->pNext)
        return (EMAC_ERROR_BADPACKET);

    /* The frag count must be correct */
    if (fragcnt != pPkt->PktFrags)
        return (EMAC_ERROR_BADPACKET);

    /* Now pad for 60 byte min size. We only pad the last fragment */
    if (pPkt->PktLength < 60)
    {
        pktlen = 60 - pPkt->PktLength;
        pPkt->PktLength = 60;
        pPktLast->ValidLen += pktlen;
    }

    /* Get a local pointer to the descriptor channel */
    pdc = &(pd->TxCh[pPkt->PktChannel]);
    pdc->ChannelIndex = pPkt->PktChannel;

    /* Make sure this packet does not have too many frags to fit */
    if (fragcnt > pdc->DescMax)
        return (EMAC_ERROR_BADPACKET);

    /*
     * Queue and packet and service transmitter
     */
    pqPushChain(&pdc->WaitQueue, pPkt, pPktLast, fragcnt);
    emacEnqueueTx(pdc);

    return (0);
}


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
 *  @n  EMAC_open function must be called before calling this API.
 *
 *  <b> Post Condition </b>
 *  @n  None
 *
 *  @b Example
 *  @verbatim
        static CSL_IntcContext context;
        static CSL_IntcEventHandlerRecord Record[13];
        static CSL_IntcObj intcEMACRx;
        static CSL_IntcHandle hIntcEMACRx;

        //CSL_IntcParam vectId1;
        CSL_IntcParam vectId2;

        CSL_IntcGlobalEnableState state;
   
        // Setup the global Interrupt 
        context.numEvtEntries = 13;
        context.eventhandlerRecord = Record; 

        // VectorID for the Event 
        vectId2 = CSL_INTC_VECTID_6;
	
        CSL_intcInit(&context);
        // Enable NMIs  
        CSL_intcGlobalNmiEnable();
        // Enable Global Interrupts
        CSL_intcGlobalEnable(&state);
   
        // Opening a handle for EMAC Rx interrupt 
        hIntcEMACRx=CSL_intcOpen(&intcEMACRx,CSL_INTC_EVENTID_6,&vectId2,NULL);

        //Hook the ISRs 
        CSL_intcHookIsr(vectId2,&HwRxInt);

        CSL_intcHwControl(hIntcEMACRx, CSL_INTC_CMD_EVTENABLE, NULL);

        // This function is called when Rx interrupt occurs 
        Void HwRxInt (void)
        {
	        // Note : get the Emac Handle(hEMAC) by calling EMAC_open function
            EMAC_RxServiceCheck(hEMAC, 0);
        } 

    @endverbatim
 * ============================================================================
 */
Uint32 EMAC_RxServiceCheck(Handle hEMAC, Uint8 coreNum)
{
    EMAC_Device     *pd = (EMAC_Device *)hEMAC;
	Uint32          intflags,Desc; //,mask;
    Uint32          tmp;
    volatile Uint32 *pRegAddr;
    int             i;

    /* Validate our handle */
    if (!pd || pd->DevMagic != EMAC_DEVMAGIC)
        return (EMAC_ERROR_INVALID);
  
    /* Read the interrupt cause */
    intflags = EMAC_REGS->MACINVECTOR;

    /* Look for fatal errors first */
    if (intflags & CSL_FMK(EMAC_MACINVECTOR_HOSTPEND, 1))
    {
        /* Read the error status - we'll decode it by hand */
        pd->FatalError = EMAC_REGS->MACSTATUS;

        /* Tell the application */
        (*localDev.Config.pfcbStatus)(pd->hApplication);

        /* return with interrupt disabled in the wrapper */
        return (EMAC_ERROR_MACFATAL);
    }

    /* Look for statistics interrupt */
    if (intflags & CSL_FMK(EMAC_MACINVECTOR_STATPEND, 1))
    {
        /* Read the stats and reset to zero         */
        /* This is necessary to clear the interrupt */
        emacUpdateStats(pd);

        /* Tell the application */
        (*localDev.Config.pfcbStatistics)(pd->hApplication);
    }

    pRegAddr = &EMAC_REGS->RX0CP;
    
    /* Look for RX interrupt (channel 0) */
    for (tmp=0, i=0; tmp<pd->Config.ChannelInfo[coreNum].RxChannels; tmp++)
    {
        if (coreNum == 1)
            i = pd->Config.ChannelInfo[coreNum - 1].RxChannels;
        if (coreNum == 2)
            i = pd->Config.ChannelInfo[coreNum - 2].RxChannels +
                pd->Config.ChannelInfo[coreNum - 1].RxChannels;

        if (intflags & CSL_FMK(EMAC_MACINVECTOR_RXPEND, 1 << (tmp + i)))   // Rx only support channel 0
        {
            /*
            * For multichannel operation, we need to apply the
            * same errata workaround as TX. However this driver
            * example only supports one RX channel.
            */
            Desc = *(pRegAddr + (tmp + i));
            *(pRegAddr + (tmp + i)) = Desc;

            pd->RxCh[tmp + i].ChannelIndex = tmp + i;
            emacDequeueRx(&pd->RxCh[tmp + i], (EMAC_Desc *)Desc);
        }
        /* Re-fill Rx buffer queue if needed */
        if (pd->RxCh[tmp + i].DescCount != pd->RxCh[tmp + i].DescMax)
            emacEnqueueRx(&pd->RxCh[tmp + i], 1);
    }
    
	EMAC_REGS->MACEOIVECTOR = 0x01 + (4 * coreNum);     // core 0, C0_RX_PULSE = 0x01

    return(0);
}




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
        coreNum     core number
    @endverbatim
 *  <b> Return Value </b>  Success (0)
 *  @n     EMAC_ERROR_INVALID   - A calling parameter is invalid
 *  @n     EMAC_ERROR_MACFATAL  - Fatal error in the MAC - Call EMAC_close()
 *
 *  <b> Pre Condition </b>
 *  @n EMAC_open function must be called before calling this API.
 *
 *  <b> Post Condition </b>
 *  @n  None
 *
 *  @b Example
 *  @verbatim
        static CSL_IntcContext context;
        static CSL_IntcEventHandlerRecord Record[13];
        static CSL_IntcObj intcEMACTx;
        static CSL_IntcHandle hIntcEMACTx;

        //CSL_IntcParam vectId1;
        CSL_IntcParam vectId2;

        CSL_IntcGlobalEnableState state;
   
        // Setup the global Interrupt 
        context.numEvtEntries = 13;
        context.eventhandlerRecord = Record; 

        // VectorID for the Event 
        vectId2 = CSL_INTC_VECTID_6;
	
        CSL_intcInit(&context);
        // Enable NMIs  
        CSL_intcGlobalNmiEnable();
        // Enable Global Interrupts
        CSL_intcGlobalEnable(&state);
   
        // Opening a handle for EMAC Tx interrupt 
        hIntcEMACTx=CSL_intcOpen(&intcEMACTx,CSL_INTC_EVENTID_6,&vectId2,NULL);

        //Hook the ISRs 
        CSL_intcHookIsr(vectId2,&HwTxInt);

        CSL_intcHwControl(hIntcEMACTx, CSL_INTC_CMD_EVTENABLE, NULL);

        // This function is called when Rx interrupt occurs 
        Void HwTxInt (void)
        {
	        // Note : get the Emac Handle(hEMAC) by calling EMAC_open function
            EMAC_TxServiceCheck(hEMAC, 0);
        } 

    @endverbatim
 * ============================================================================
 */
Uint32 EMAC_TxServiceCheck(Handle hEMAC, Uint8 coreNum)
{
    EMAC_Device     *pd = (EMAC_Device *)hEMAC;
    Uint32          intflags,Desc; //,mask;
	Uint32          i, tmp;
    volatile Uint32 *pRegAddr;

    /* Validate our handle */
    if (!pd || pd->DevMagic != EMAC_DEVMAGIC)
        return (EMAC_ERROR_INVALID);

    /* Read the interrupt cause */
    intflags = EMAC_REGS->MACINVECTOR;

    /* Look for fatal errors first */
    if (intflags & CSL_FMK(EMAC_MACINVECTOR_HOSTPEND, 1))
    {
        /* Read the error status - we'll decode it by hand */
        pd->FatalError = EMAC_REGS->MACSTATUS;

        /* Tell the application */
        (*localDev.Config.pfcbStatus)(pd->hApplication);

        /* return with interrupt disabled in the wrapper */
        return (EMAC_ERROR_MACFATAL);
    }

    /* Look for statistics interrupt */
    if (intflags & CSL_FMK(EMAC_MACINVECTOR_STATPEND, 1))
    {
        /* Read the stats and reset to zero         */
        /* This is necessary to clear the interrupt */
        emacUpdateStats(pd);

        /* Tell the application */
        (*localDev.Config.pfcbStatistics)(pd->hApplication);
    }

    pRegAddr = &EMAC_REGS->TX0CP;

    /* Look for TX interrupt (channel 0-max) */
    for (tmp = 0, i = 0; tmp < pd->Config.ChannelInfo[coreNum].TxChannels; tmp++)
    {   
        if (coreNum == 1)
            i = pd->Config.ChannelInfo[coreNum - 1].TxChannels;
        if (coreNum == 2)
            i = pd->Config.ChannelInfo[coreNum - 2].TxChannels +
                pd->Config.ChannelInfo[coreNum - 1].TxChannels;
        
	    if (intflags & CSL_FMK(EMAC_MACINVECTOR_TXPEND, 1 << (tmp + i)))
        {
            Desc = *(pRegAddr + (tmp + i));
            *(pRegAddr + (tmp + i)) = Desc;
            
            pd->TxCh[tmp + i].ChannelIndex = tmp + i;
            emacDequeueTx(&pd->TxCh[tmp + i], (EMAC_Desc *)Desc);
        }
    }

	EMAC_REGS->MACEOIVECTOR = 0x02 + (4 * coreNum);     // core 0, C0_TX_PULSE = 0x02

    return (0);
}



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
 *  @n  Re-fill Rx buffer queue if needed and modifies  EMAC CONTROL register.
 *
 *  @b Example
 *  @verbatim
        EMAC_Config ecfg;
        Handle      hEMAC = 0;

        //open the EMAC device
        EMAC_open( 1, (Handle)0x12345678, &ecfg, &hEMAC );

        EMAC_timerTick( hEMAC, 0 );
    @endverbatim
 * ============================================================================
 */
Uint32 EMAC_timerTick(Handle hEMAC, Uint8 coreNum)
{
    EMAC_Device  *pd = (EMAC_Device *)hEMAC;
    Uint32       mdioStatus,linkStatus;
	Uint32       i;

    /* Validate our handle */
    if (!pd || pd->DevMagic != EMAC_DEVMAGIC)
        return( EMAC_ERROR_INVALID );

    if (pd->Config.UseMdio)
    {
        /* Signal the MDIO */
        mdioStatus = MDIO_timerTick( pd->hMDIO );
 
	    /* On a new link, set the EMAC duplex */
        if (mdioStatus == MDIO_EVENT_LINKUP)
        {
            MDIO_getStatus(pd->hMDIO, 0, &linkStatus);

            if (linkStatus == MDIO_LINKSTATUS_FD10  ||
                linkStatus == MDIO_LINKSTATUS_FD100 ||
                linkStatus == MDIO_LINKSTATUS_FD1000 )
            {
                CSL_FINST(EMAC_REGS->MACCONTROL, EMAC_MACCONTROL_FULLDUPLEX, ENABLE );
		    }
            else
            {
                CSL_FINST(EMAC_REGS->MACCONTROL, EMAC_MACCONTROL_FULLDUPLEX, DISABLE );
            }

		    if (linkStatus == MDIO_LINKSTATUS_FD1000)
			    CSL_FINST(EMAC_REGS->MACCONTROL, EMAC_MACCONTROL_GIG, ENABLE );
	    }

	    if (mdioStatus != MDIO_EVENT_NOCHANGE)
	    (*localDev.Config.pfcbStatus)(pd->hApplication);
    } 

    /* Re-fill Rx buffer queue if needed */
    for (i = 0; i < pd->Config.ChannelInfo[coreNum].RxChannels; i++) 
    {
        pd->RxCh[i].ChannelIndex = i;
        if (pd->RxCh[i].DescCount != pd->RxCh[i].DescMax)
        emacEnqueueRx(&pd->RxCh[i], 1);
    }
    
    return (0);
}

