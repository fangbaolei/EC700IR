/*  ============================================================================
 *   Copyright (c) Texas Instruments Inc 2002, 2003, 2004, 2005
 *
 *   Use of this software is controlled by the terms and conditions found in the
 *   license agreement under which this software has been supplied.
 *   ===========================================================================
 */
 
/* @file csl_intcAux.h
 *
 * @brief   Header file for functional layer of CSL
 *
 *  PATH  $(CSLPATH)\\inc
 */
 
/* =============================================================================
 *  Revision History
 *  ===============
 *  12-Jun-2004 Ruchika Kharwar File Created
 *  16-Mar-2005 brn modified for doxygen documentation and removed redundant 
                    CSL_intcInterruptEnable, CSL_intcInterruptDisable and 
                    CSL_intcInterruptRestore functions.
    27-Mar-2006 ds  Removed Read access to Write only EVTSET[3] and EVTCLR[3] 
                    registers 
 * =============================================================================
 */
 

#ifndef _CSL_INTCAUX_H
#define _CSL_INTCAUX_H

#include <csl_intc.h>

/* External definition for _CSL_intcEventOffsetMap */
extern Int8 *_CSL_intcEventOffsetMap;

/* External definition for _CSL_intcAllocMask */
extern CSL_BitMask32*    _CSL_intcAllocMask;

/* External definition for _CSL_intcEventHandlerRecord */
extern CSL_IntcEventHandlerRecord* _CSL_intcEventHandlerRecord;

/* External definition for _CSL_intcNumEvents */
extern Uint16 _CSL_intcNumEvents;

/*  ============================================================================
 *   @n@b CSL_intcMapEventVector
 *
 *   @b Description
 *      This API Maps the event to the given CPU vector
 *      
 *   @b Arguments
 *   @verbatim
        hInts    Handle to the intcs
        vectId   Intc vector identifier 
     @endverbatim
 *
 *   <b> Return Value </b> 
 *       None 
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  Maps the event to the given CPU vector 
 *
 *   @b  Modifies
 *   @n  None
 *
 *   @b  Example
 *   @verbatim
        CSL_IntcHandle     hIntc;
        CSL_IntcVectId     vectId;
        ...
        CSL_intcMapEventVector (hIntc, vectId);
        ...
     @endverbatim
 * ===========================================================================
 */
CSL_IDEF_INLINE void CSL_intcMapEventVector (
    CSL_IntcEventId    eventId,
    CSL_IntcVectId     vectId
)
{
    
    Int         bitLow;
    
    if (vectId < CSL_INTC_VECTID_8) {
        bitLow = (vectId - 4) * 8;
        CSL_FINSR(((CSL_IntcRegsOvly)CSL_INTC_0_REGS)->INTMUX1, \
                  bitLow+6,bitLow,eventId);
    } 
    else {
        if (vectId < CSL_INTC_VECTID_12) {
            bitLow = (vectId - 8) * 8;
            CSL_FINSR(((CSL_IntcRegsOvly)CSL_INTC_0_REGS)->INTMUX2, \
                     bitLow+6,bitLow,eventId);
        } 
        else {
            bitLow = (vectId - 12) * 8;
            CSL_FINSR(((CSL_IntcRegsOvly)CSL_INTC_0_REGS)->INTMUX3, \
                     bitLow+6,bitLow,eventId);
        }   
    }
}

/*  ============================================================================
 *   @n@b CSL_intcEventEnable
 *
 *   @b Description
 *      This API enables particular event (EVTMASK0/1/2/3 bit programmation)
 *      
 *   @b Arguments
 *   @verbatim
        eventId   event identifier
        
     @endverbatim
 *
 *   <b> Return Value </b> 
 *       CSL_IntcEventEnableState - Previous state 
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  particular event will be enabled
 *
 *   @b  Modifies
 *   @n  None
 *
 *   @b  Example
 *   @verbatim
        CSL_IntcVectId             eventId;
        CSL_IntcEventEnableState   eventStat;
        ...
        eventStat = CSL_intcEventEnable (eventId);
        ...
     @endverbatim
 * ===========================================================================
 */
CSL_IDEF_INLINE CSL_IntcEventEnableState CSL_intcEventEnable (
    CSL_IntcEventId     eventId
)
{
    Int                      _x;
    Int                      _y;
    Int                      _regVal;
    CSL_IntcEventEnableState _oldState;
    
    _y = eventId >> 5;
    _x = eventId & 0x1f;
    _regVal = ((CSL_IntcRegsOvly)CSL_INTC_0_REGS)->EVTMASK[_y];
    _oldState = CSL_FEXTR(_regVal,_x,_x);
    CSL_FINSR(_regVal,_x,_x,0);
    ((CSL_IntcRegsOvly)CSL_INTC_0_REGS)->EVTMASK[_y] = _regVal;
    
    return _oldState;
}

/*  ============================================================================
 *   @n@b CSL_intcEventDisable
 *
 *   @b Description
 *      This API disables particular event (EVTMASK0/1/2/3 bit programmation)
 *      
 *   @b Arguments
 *   @verbatim
        eventId   event identifier
        
     @endverbatim
 *
 *   <b> Return Value </b> 
 *       CSL_IntcEventEnableState - Previous state 
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  particular event will be disabled
 *
 *   @b  Modifies
 *   @n  None
 *
 *   @b  Example
 *   @verbatim
        CSL_IntcVectId             eventId;
        CSL_IntcEventEnableState   eventStat;
        ...
        eventStat = CSL_intcEventDisable (eventId);
        ...
     @endverbatim
 * ===========================================================================
 */
CSL_IDEF_INLINE CSL_IntcEventEnableState  CSL_intcEventDisable (
    CSL_IntcEventId            eventId
)
{
    Int                       _x;
    Int                       _y;
    Int                      _regVal;
    CSL_IntcEventEnableState oldState;
    
    _y = eventId >> 5;
    _x = eventId & 0x1f;
    _regVal = ((CSL_IntcRegsOvly)CSL_INTC_0_REGS)->EVTMASK[_y];
    oldState = CSL_FEXTR(_regVal,_x,_x);
    CSL_FINSR(_regVal,_x,_x,1);
    ((CSL_IntcRegsOvly)CSL_INTC_0_REGS)->EVTMASK[_y] = _regVal;
    
    return oldState;
}

/*  ============================================================================
 *   @n@b CSL_intcEventRestore
 *
 *   @b Description
 *      This API restores particular event (EVTMASK0/1/2/3 bit programmation)
 *      
 *   @b Arguments
 *   @verbatim
        eventId   event identifier
        
     @endverbatim
 *
 *   <b> Return Value </b> 
 *       CSL_IntcEventEnableState - Previous state 
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  particular event will be restored
 *
 *   @b  Modifies
 *   @n  None
 *
 *   @b  Example
 *   @verbatim
        CSL_IntcVectId             eventId;
        CSL_IntcEventEnableState   restoreVal;
        CSL_IntcEventEnableState   eventStat;
        ...
        eventStat = CSL_intcEventRestore (eventId, restoreVal);
        ...
     @endverbatim
 * ===========================================================================
 */
CSL_IDEF_INLINE void  CSL_intcEventRestore(
    CSL_IntcEventId               eventId,
    CSL_IntcEventEnableState      restoreVal
)
{
    Int     _x;
    Int     _y;
    
    _y = eventId >> 5;
    _x = eventId & 0x1F;
    
    CSL_FINSR(((CSL_IntcRegsOvly)CSL_INTC_0_REGS)->EVTMASK[_y], _x, _x, \
              restoreVal);
}

/*  ============================================================================
 *   @n@b CSL_intcEventSet
 *
 *   @b Description
 *      This API sets Event  (EVTMASK0/1/2/3 bit programmation)
 *      
 *   @b Arguments
 *   @verbatim
        eventId   event identifier
        
     @endverbatim
 *
 *   <b> Return Value </b> 
 *       None 
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  particular event will set
 *
 *   @b  Modifies
 *   @n  None
 *
 *   @b  Example
 *   @verbatim
        CSL_IntcVectId             eventId;
        ...
        CSL_intcEventSet (eventId);
        ...
     @endverbatim
 * ===========================================================================
 */
CSL_IDEF_INLINE void  CSL_intcEventSet(
    CSL_IntcEventId               eventId
)
{
    Int     _x;
    Int     _y;
    
    _y = eventId >> 5;
    _x = eventId & 0x1F;
    
    ((CSL_IntcRegsOvly)CSL_INTC_0_REGS)->EVTSET[_y] = CSL_FMKR(_x,_x,1);
}

/*  ============================================================================
 *   @n@b CSL_intcEventClear
 *
 *   @b Description
 *      This API clears particular event (EVTMASK0/1/2/3 bit programmation)
 *      
 *   @b Arguments
 *   @verbatim
        eventId   event identifier
        
     @endverbatim
 *
 *   <b> Return Value </b> 
 *       None 
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  particular event will be cleared
 *
 *   @b  Modifies
 *   @n  None
 *
 *   @b  Example
 *   @verbatim
        CSL_IntcVectId             eventId;
        ...
        CSL_intcEventClear (eventId);
        ...
     @endverbatim
 * ===========================================================================
 */
CSL_IDEF_INLINE void  CSL_intcEventClear (
    CSL_IntcEventId               eventId
)
{
    Int     _x;
    Int     _y;
    
    _y = eventId >> 5;
    _x = eventId & 0x1F;
    
    ((CSL_IntcRegsOvly)CSL_INTC_0_REGS)->EVTCLR[_y] = CSL_FMKR(_x,_x,1);
}

/*  ============================================================================
 *   @n@b CSL_intcCombinedEventClear
 *
 *   @b Description
 *      This API clears particular combined events 
 *      (EVTMASK0/1/2/3 bit programmation)
 *      
 *   @b Arguments
 *   @verbatim
        eventId     event identifier
        clearMask   BitMask of events to be cleared
        
     @endverbatim
 *
 *   <b> Return Value </b> 
 *       None 
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  particular combined event will be cleared
 *
 *   @b  Modifies
 *   @n  None
 *
 *   @b  Example
 *   @verbatim
        CSL_IntcVectId             eventId;
        ...
        CSL_intcCombinedEventClear (eventId);
        ...
     @endverbatim
 * ===========================================================================
 */

CSL_IDEF_INLINE void  CSL_intcCombinedEventClear(
    CSL_IntcEventId               eventId,
    CSL_BitMask32                 clearMask
)
{
    ((CSL_IntcRegsOvly)CSL_INTC_0_REGS)->EVTCLR[eventId] = clearMask;
}

/*  ============================================================================
 *   @n@b CSL_intcCombinedEventGet
 *
 *   @b Description
 *      This API gets particular combined events 
 *      (EVTMASK0/1/2/3 bit programmation)
 *      
 *   @b Arguments
 *   @verbatim
        eventId     event identifier
                
     @endverbatim
 *
 *   <b> Return Value </b> 
 *       CSL_BitMask32 - The combined events information 
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b  Modifies
 *   @n  None
 *
 *   @b  Example
 *   @verbatim
        CSL_IntcVectId             eventId;
        CSL_BitMask32              combEvntStat;
        ...
        combEvntStat = CSL_intcCombinedEventGet (eventId);
        ...
     @endverbatim
 * ===========================================================================
 */
CSL_IDEF_INLINE CSL_BitMask32  CSL_intcCombinedEventGet(
   CSL_IntcEventId               eventId
)
{
    return (((CSL_IntcRegsOvly)CSL_INTC_0_REGS)->MEVTFLAG[eventId]);
}

/*  ============================================================================
 *   @n@b CSL_intcCombinedEventEnable
 *
 *   @b Description
 *      This API enables particular combined events 
 *      
 *   @b Arguments
 *   @verbatim
        eventId     event identifier
        enableMask  BitMask of events to be enabled     
     @endverbatim
 *
 *   <b> Return Value </b> 
 *       CSL_BitMask32 - previous state 
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b  Modifies
 *   @n  None
 *
 *   @b  Example
 *   @verbatim
        CSL_IntcVectId             eventId;
        CSL_BitMask32              enableMask;
        CSL_BitMask32              combEvntStat;
        ...
        combEvntStat = CSL_intcCombinedEventEnable (eventId, enableMask);
        ...
     @endverbatim
 * ===========================================================================
 */
CSL_IDEF_INLINE CSL_BitMask32  CSL_intcCombinedEventEnable(
    CSL_IntcEventId               eventId,
    CSL_BitMask32                 enableMask
)
{
    CSL_BitMask32 evtMask = 
            ((CSL_IntcRegsOvly)CSL_INTC_0_REGS)->EVTMASK[eventId];
    ((CSL_IntcRegsOvly)CSL_INTC_0_REGS)->EVTMASK[eventId] = ~enableMask;
    
    return evtMask;
}

/** ============================================================================
 *   @n@b CSL_intcCombinedEventDisable
 *
 *   @b Description
 *      This API disables particular combined events 
 *      
 *   @b Arguments
 *   @verbatim
        eventId     event identifier
        enableMask  BitMask of events to be disabled     
     @endverbatim
 *
 *   <b> Return Value </b> 
 *       CSL_BitMask32 - previous state 
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b  Modifies
 *   @n  None
 *
 *   @b  Example
 *   @verbatim
        CSL_IntcVectId             eventId;
        CSL_BitMask32              enableMask;
        CSL_BitMask32              combEvntStat;
        
        ...
        combEvntStat = CSL_intcCombinedEventDisable (eventId, enableMask);
        ...
     @endverbatim
 * ===========================================================================
 */
CSL_IDEF_INLINE CSL_BitMask32  CSL_intcCombinedEventDisable (
    CSL_IntcEventId               eventId,
    CSL_BitMask32                 enableMask
)
{
    CSL_BitMask32 evtMask = 
            ((CSL_IntcRegsOvly)CSL_INTC_0_REGS)->EVTMASK[eventId];
    ((CSL_IntcRegsOvly)CSL_INTC_0_REGS)->EVTMASK[eventId] = enableMask;
    
    return evtMask ;    
}

/*  ============================================================================
 *   @n@b CSL_intcCombinedEventRestore
 *
 *   @b Description
 *      This API restores particular combined events 
 *      
 *   @b Arguments
 *   @verbatim
        eventId      event identifier
        restoreMask  BitMask of events to be restored     
     @endverbatim
 *
 *   <b> Return Value </b> 
 *       None 
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b  Modifies
 *   @n  None
 *
 *   @b  Example
 *   @verbatim
        CSL_IntcVectId             eventId;
        CSL_BitMask32              restoreMask
        ...
        CSL_intcCombinedEventRestore (eventId, restoreMask);
        ...
     @endverbatim
 * ===========================================================================
 */
CSL_IDEF_INLINE void  CSL_intcCombinedEventRestore(
    CSL_IntcEventId               eventId,
    CSL_BitMask32                 restoreMask
)
{
    ((CSL_IntcRegsOvly)CSL_INTC_0_REGS)->EVTMASK[eventId] = restoreMask;

}

/*  ============================================================================
 *   @n@b CSL_intcIntrDropEnable
 *
 *   @b Description
 *      This API enables interrupts for which drop detection 
 *      
 *   @b Arguments
 *   @verbatim
        dropMask  Vector id mask
     @endverbatim
 *
 *   <b> Return Value </b> 
 *       None 
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b  Modifies
 *   @n  None
 *
 *   @b  Example
 *   @verbatim
        CSL_BitMask32              dropMask
        ...
        CSL_intcIntrDropEnable (dropMask);
        ...
     @endverbatim
 * ===========================================================================
 */
CSL_IDEF_INLINE void  CSL_intcInterruptDropEnable (
    CSL_BitMask32                dropMask        
)
{
    ((CSL_IntcRegsOvly)CSL_INTC_0_REGS)->INTDMASK &= ~dropMask;
    
}

/*  ============================================================================
 *   @n@b CSL_intcIntrDropDisable
 *
 *   @b Description
 *      This API disables interrupts for which drop detection 
 *      
 *   @b Arguments
 *   @verbatim
        dropMask  Vector id mask
     @endverbatim
 *
 *   <b> Return Value </b> 
 *       None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b  Modifies
 *   @n  None
 *
 *   @b  Example
 *   @verbatim
        CSL_BitMask32              dropMask
        ...
        CSL_intcIntrDropDisable (dropMask);
        ...
     @endverbatim
 * ===========================================================================
 */
CSL_IDEF_INLINE void  CSL_intcInterruptDropDisable (
    CSL_BitMask32                dropMask        
)
{
    ((CSL_IntcRegsOvly)CSL_INTC_0_REGS)->INTDMASK  |= dropMask;
    
}

/*  ============================================================================
 *   @n@b CSL_intcInvokeEventHandle
 *
 *   @b Description
 *     This API is for the purpose of excption handler which will need to be
 * written by the user. This API invokes the event handler regisered by 
 *      the user at the time of event Open and event handler registration 
 *      
 *   @b Arguments
 *   @verbatim
        evtId  Event Id of the function to be invoked
     @endverbatim
 *
 *   <b> Return Value </b> 
 *       CSL_SOK
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b  Modifies
 *   @n  None
 *
 *   @b  Example
 *   @verbatim
        CSL_IntcEventId              evtId;
        CSL_Status                   status;
        ...
        status = CSL_intcInvokeEventHandle (evtId);
        ...
     @endverbatim
 * ===========================================================================
 */
CSL_IDEF_INLINE CSL_Status CSL_intcInvokeEventHandle (
    CSL_IntcEventId     evtId
)
{

    if (_CSL_intcEventOffsetMap[evtId] != CSL_INTC_MAPPED_NONE) 
        _CSL_intcEventHandlerRecord[_CSL_intcEventOffsetMap[evtId]].handler (
            _CSL_intcEventHandlerRecord[_CSL_intcEventOffsetMap[evtId]].arg );
          
     return CSL_SOK;
}

/*  ============================================================================
 *   @n@b CSL_intcQueryEventStatus
 *
 *   @b Description
 *      This API is to check whether a specified event is enabled or not 
 *
 *   @b Arguments
 *   @verbatim
        evtId      Event Id of the query event
     @endverbatim
 *
 *   <b> Return Value </b> 
 *       Bool
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b  Modifies
 *   @n  None
 *
 *   @b  Example
 *   @verbatim
        CSL_IntcEventId              evtId;
        Bool                         return;
        ...
        return = CSL_intcQueryEventStatus (evtId);
        ...
     @endverbatim
 * ===========================================================================
 */
CSL_IDEF_INLINE Bool CSL_intcQueryEventStatus(CSL_IntcEventId eventId)
{
    Int     _x;
    Int     _y;
    
    _y = eventId >> 5;
    _x = eventId & 0x1F;
    
    return ((Bool)(CSL_FEXTR(((CSL_IntcRegsOvly)CSL_INTC_0_REGS)->EVTFLAG[_y], \
                            _x,_x)));
}

/* CPU Interrupt Handling routines */

/*  ============================================================================
 *   @n@b CSL_intcInterruptEnable
 *
 *   @b Description
 *      This API is enables Interrupt 
 *      
 *   @b Arguments
 *   @verbatim
        vectId      Vector Id of the query event
     @endverbatim
 *
 *   <b> Return Value </b> 
 *       Uint32     - previous state
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b  Modifies
 *   @n  None
 *
 *   @b  Example
 *   @verbatim
        CSL_IntcVectId              vectId;
        Uint32                      return;
        ...
        return = CSL_intcInterruptEnable (vectId);
        ...
     @endverbatim
 * ===========================================================================
 */
Uint32 CSL_intcInterruptEnable (
    CSL_IntcVectId vectId
);

/*  ============================================================================
 *   @n@b CSL_intcInterruptDisable
 *
 *   @b Description
 *      This API is disables Interrupt 
 *      
 *   @b Arguments
 *   @verbatim
        vectId      Vector Id of the query event
     @endverbatim
 *
 *   <b> Return Value </b> 
 *       Uint32     - previous state
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b  Modifies
 *   @n  None
 *
 *   @b  Example
 *   @verbatim
        CSL_IntcVectId              vectId;
        Uint32                      return;
        ...
        return = CSL_intcInterruptDisable (vectId);
        ...
     @endverbatim
 * ===========================================================================
 */
Uint32 CSL_intcInterruptDisable (
    CSL_IntcVectId vectId
);

/*  ============================================================================
 *   @n@b CSL_intcInterruptRestore
 *
 *   @b Description
 *      This API is restores Interrupt 
 *      
 *   @b Arguments
 *   @verbatim
        vectId      Vector Id of the query event
        restoreVal  Value to be restored
     @endverbatim
 *
 *   <b> Return Value </b> 
 *       None
 *       
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b  Modifies
 *   @n  None
 *
 *   @b  Example
 *   @verbatim
        CSL_IntcVectId              vectId;
        Uint32                      restore;
        ...
        CSL_intcInterruptRestore (vectId, restore);
        ...
     @endverbatim
 * ===========================================================================
 */
CSL_IDEF_INLINE void CSL_intcInterruptRestore ( 
    CSL_IntcVectId      vectId,
    Uint32              restoreVal
);

/*  ============================================================================
 *   @n@b CSL_intcInterruptSet
 *
 *   @b Description
 *      This API is sets interrupt(ISR) 
 *
 *   @b Arguments
 *   @verbatim
        vectId       Vector id to set

     @endverbatim
 *
 *   <b> Return Value </b> 
 *       None
 *       
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b  Modifies
 *   @n  None
 *
 *   @b  Example
 *   @verbatim
        CSL_IntcVectId              vectId;
        ...
        CSL_intcInterruptSet (vectId);
        ...
     @endverbatim
 * ===========================================================================
 */
void CSL_intcInterruptSet ( 
    CSL_IntcVectId      vectId
);

/*  ============================================================================
 *   @n@b CSL_intcInterruptClear
 *
 *   @b Description
 *      This API is clears specified interrupt  
 *      
 *   @b Arguments
 *   @verbatim
        vectId       Vector id to cleared
        
     @endverbatim
 *
 *   <b> Return Value </b> 
 *       None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b  Modifies
 *   @n  None
 *
 *   @b  Example
 *   @verbatim
        CSL_IntcVectId              vectId;
        ...
        CSL_intcInterruptClear (vectId);
        ...
     @endverbatim
 * ===========================================================================
 */
void CSL_intcInterruptClear ( 
    CSL_IntcVectId      vectId
);

/*  ============================================================================
 *   @n@b CSL_intcQueryInterruptStatus
 *
 *   @b Description
 *      This API is to check whether a specified CPU interrupt is pending or not  
 *
 *   @b Arguments
 *   @verbatim
        vectId       Vector id to cleared

     @endverbatim
 *
 *   <b> Return Value </b> 
 *       Bool
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b  Modifies
 *   @n  None
 *
 *   @b  Example
 *   @verbatim
        CSL_IntcVectId              vectId;
        Bool                        return;
        ...
        return = CSL_intcQueryInterruptStatus (vectId);
        ...
     @endverbatim
 * ===========================================================================
 */
Bool  CSL_intcQueryInterruptStatus (
    CSL_IntcVectId      vectId
);


/* Exception handling routines */

/*  ============================================================================
 *   @n@b CSL_intcExcepEnable
 *
 *   @b Description
 *      This API is enables the specific exception event  
 *      
 *   @b Arguments
 *   @verbatim
        eventId       event id to be enabled
        
     @endverbatim
 *
 *   <b> Return Value </b> 
 *       CSL_IntcEventEnableState
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b  Modifies
 *   @n  None
 *
 *   @b  Example
 *   @verbatim
        CSL_IntcEventId             eventId;
        CSL_IntcEventEnableState    return;
        ...
        return = CSL_intcExcepEnable (eventId);
        ...
     @endverbatim
 * ===========================================================================
 */
CSL_IDEF_INLINE CSL_IntcEventEnableState  CSL_intcExcepEnable (
    CSL_IntcEventId      eventId
)
{
    Int                         _x;
    Int                         _y;
    Int                         _regVal;
    CSL_IntcEventEnableState    _oldState;
    
    _y = eventId >> 5;
    _x = eventId & 0x1f;
    _regVal = ((CSL_IntcRegsOvly)CSL_INTC_0_REGS)->EXPMASK[_y];
    _oldState = CSL_FEXTR(_regVal,_x,_x);
    CSL_FINSR(_regVal,_x,_x,0);
    ((CSL_IntcRegsOvly)CSL_INTC_0_REGS)->EXPMASK[_y] = _regVal;
    
    return _oldState;
}

/*  ============================================================================
 *   @n@b CSL_intcExcepDisable
 *
 *   @b Description
 *      This API is disables the specific exception event  
 *      
 *   @b Arguments
 *   @verbatim
        eventId       event id to be disabled
        
     @endverbatim
 *
 *   <b> Return Value </b> 
 *       CSL_IntcEventEnableState
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b  Modifies
 *   @n  None
 *
 *   @b  Example
 *   @verbatim
        CSL_IntcEventId             eventId;
        CSL_IntcEventEnableState    return;
        ...
        return = CSL_intcExcepDisable (eventId);
        ...
     @endverbatim
 * ===========================================================================
 */
CSL_IDEF_INLINE CSL_IntcEventEnableState  CSL_intcExcepDisable (
    CSL_IntcEventId            eventId
)
{
    Int                         _x;
    Int                         _y;
    Int                         _regVal;
    CSL_IntcEventEnableState    oldState;
    
    _y = eventId >> 5;
    _x = eventId & 0x1f;
    _regVal = ((CSL_IntcRegsOvly)CSL_INTC_0_REGS)->EXPMASK[_y];
    oldState = CSL_FEXTR(_regVal,_x,_x);
    CSL_FINSR(_regVal,_x,_x,1);
    ((CSL_IntcRegsOvly)CSL_INTC_0_REGS)->EXPMASK[_y] = _regVal;
    
    return oldState;
}

/*  ============================================================================
 *   @n@b CSL_intcExcepRestore
 *
 *   @b Description
 *      This API restores the specific exception event 
 *      
 *   @b Arguments
 *   @verbatim
        eventId       event id to be restored 
        restoreVal    restore value
     @endverbatim
 *
 *   <b> Return Value </b> 
 *       None 
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b  Modifies
 *   @n  None
 *
 *   @b  Example
 *   @verbatim
        CSL_IntcEventId             eventId;
        
        ...
        CSL_intcExcepRestore (eventId);
        ...
     @endverbatim
 * ===========================================================================
 */
CSL_IDEF_INLINE void  CSL_intcExcepRestore (
    CSL_IntcEventId               eventId,
    Uint32                        restoreVal
)
{
    Int         _x;
    Int         _y;
    
    _y = eventId >> 5;
    _x = eventId & 0x1F;
    CSL_FINSR(((CSL_IntcRegsOvly)CSL_INTC_0_REGS)->EXPMASK[_y],_x,_x, \
             restoreVal);
}

/*  ============================================================================
 *   @n@b CSL_intcExcepClear
 *
 *   @b Description
 *      This API clears the specific exception event 
 *      
 *   @b Arguments
 *   @verbatim
        eventId       event id to be cleared 
        
     @endverbatim
 *
 *   <b> Return Value </b> 
 *       None 
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b  Modifies
 *   @n  None
 *
 *   @b  Example
 *   @verbatim
        CSL_IntcEventId             eventId;
        
        ...
        CSL_intcExcepClear (eventId);
        ...
     @endverbatim
 * ===========================================================================
 */
CSL_IDEF_INLINE void  CSL_intcExcepClear (
    CSL_IntcEventId         eventId
)
{
    Int         _x;
    Int         _y;
    
    _y = eventId >> 5;
    _x = eventId & 0x1F;
    CSL_FINSR(((CSL_IntcRegsOvly)CSL_INTC_0_REGS)->EVTCLR[_y],_x,_x,1);
}

#endif
