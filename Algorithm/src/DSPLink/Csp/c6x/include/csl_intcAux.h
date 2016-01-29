/*****************************************************\
 *  Copyright 2004, Texas Instruments Incorporated.  *
 *  All rights reserved.                             *
 *  Restricted rights to use, duplicate or disclose  *
 *  this   code   are  granted   through  contract.  *
 *                                                   *
 * "@(#) PSP/CSL  3.0.0.0  (2003-09-30)              *
\*****************************************************/
#ifndef _CSL_INTCAUX_H
#define _CSL_INTCAUX_H
#define CSL_IDEF_INLINE static inline

extern Int8 *_CSL_intcEventOffsetMap;
extern CSL_BitMask32*    _CSL_intcAllocMask;
extern CSL_IntcEventHandlerRecord* _CSL_intcEventHandlerRecord;
extern Uint16 _CSL_intcNumEvents;
/** @addtogroup CSL_INTC_FUNCTION_INTERNAL
@{ */


/* Internal APIs - Interrupts and exceptions */
/*
 * ======================================================
 *   @func   CSL_intcMapEventVector
 * ======================================================
 */
/** @brief 
 *	   Maps the event to the given CPU vector
 */
 /* ======================================================
 *   @arg  eventId
 *		Event Id (4-127)
 *   @ret  CSL_IntcEventEnableState - Previous state
 *
 * ======================================================
 */
CSL_IDEF_INLINE
void CSL_intcMapEventVector(CSL_IntcEventId eventId,CSL_IntcVectId vectId)
{
	
	int bitLow;
	if (vectId < CSL_INTC_VECTID_8) {
		bitLow = (vectId - 4) * 8;
		CSL_FINSR(((CSL_IntcRegsOvly)CSL_INTC_REGS)->INTMUX1,bitLow+6,bitLow,eventId);
	} else {
		if (vectId < CSL_INTC_VECTID_12) {
			bitLow = (vectId - 8) * 8;
			CSL_FINSR(((CSL_IntcRegsOvly)CSL_INTC_REGS)->INTMUX2,bitLow+6,bitLow,eventId);
		} else {
			bitLow = (vectId - 12) * 8;
			CSL_FINSR(((CSL_IntcRegsOvly)CSL_INTC_REGS)->INTMUX3,bitLow+6,bitLow,eventId);
		}	
	}
}
/*
 * ======================================================
 *   @func   CSL_intcEventEnable
 * ======================================================
 */
/** @brief 
 *	   Event Enable (EVTMASK0/1/2/3 bit programmation)
 */
 /* ======================================================
 *   @arg  eventId
 *		Event Id (4-127)
 *   @ret  CSL_IntcEventEnableState - Previous state
 *
 * ======================================================
 */
CSL_IDEF_INLINE
CSL_IntcEventEnableState
	CSL_intcEventEnable(
   		CSL_IntcEventId		eventId          /**< Event Id to be Enabled */ 
)
{
    int _x,_y,_regVal;
	CSL_IntcEventEnableState _oldState;
	_y = eventId >> 5;
	_x = eventId & 0x1f;
	_regVal = ((CSL_IntcRegsOvly)CSL_INTC_REGS)->EVTMASK[_y];
	_oldState = CSL_FEXTR(_regVal,_x,_x);
	CSL_FINSR(_regVal,_x,_x,0);
	((CSL_IntcRegsOvly)CSL_INTC_REGS)->EVTMASK[_y] = _regVal;
	return _oldState;
}

/*
 * ======================================================
 *   @func   CSL_intcEventDisable
 * ======================================================
 */
/** @brief 
 *	   Event disable (EVTMASK0/1/2/3 bit programmation)
 */
 /* ======================================================
 *   @arg  eventId
 *		Event Id (4-127)
 *   @ret  CSL_IntcEventEnableState - Previous state
 *
 * ======================================================
 */
CSL_IDEF_INLINE
CSL_IntcEventEnableState  CSL_intcEventDisable(
   CSL_IntcEventId            eventId
)
{
    int _x,_y,_regVal;
	CSL_IntcEventEnableState oldState;
	_y = eventId >> 5;
	_x = eventId & 0x1f;
	_regVal = ((CSL_IntcRegsOvly)CSL_INTC_REGS)->EVTMASK[_y];
	oldState = CSL_FEXTR(_regVal,_x,_x);
	CSL_FINSR(_regVal,_x,_x,1);
	((CSL_IntcRegsOvly)CSL_INTC_REGS)->EVTMASK[_y] = _regVal;
	return oldState;
}
/*
 * ======================================================
 *   @func   CSL_intcEventRestore
 * ======================================================
 */
/** @brief 
 *	   Event restore (EVTMASK0/1/2/3 bit programmation)
 */
 /* ======================================================
 *   @arg  eventId
 *		Event Id (4-127)
 *   @arg  prevState
 *		previous state
 *   @ret  None
 *
 * ======================================================
 */
CSL_IDEF_INLINE
void  CSL_intcEventRestore(
   CSL_IntcEventId               eventId,
   CSL_IntcEventEnableState  restoreVal
)
{
    int _x,_y;
	_y = eventId >> 5;
	_x = eventId & 0x1F;
	CSL_FINSR(((CSL_IntcRegsOvly)CSL_INTC_REGS)->EVTMASK[_y],_x,_x,restoreVal);
}
/*
 * ======================================================
 *   @func   CSL_intcEventSet
 * ======================================================
 */
/** @brief 
 *	   Event set (EVTSET0/1/2/3 bit programmation)
 */
 /* ======================================================
 *   @arg  eventId
 *		Event Id (4-127)
 *   @ret  None
 *
 * ======================================================
 */
CSL_IDEF_INLINE
void  CSL_intcEventSet(
   CSL_IntcEventId               eventId
)
{
	int _x,_y;
	_y = eventId >> 5;
	_x = eventId & 0x1F;
	CSL_FINSR(((CSL_IntcRegsOvly)CSL_INTC_REGS)->EVTSET[_y],_x,_x,1);
}
/*
 * ======================================================
 *   @func   CSL_intcEventClear
 * ======================================================
 */
/** @brief 
 *	   Event set (EVTCLR0/1/2/3 bit programmation)
 */
 /* ======================================================
 *   @arg  eventId
 *		Event Id (4-127)
 *   @ret  None
 *
 * ======================================================
 */
CSL_IDEF_INLINE
void  CSL_intcEventClear(
   CSL_IntcEventId               eventId
)
{
	int _x,_y;
	_y = eventId >> 5;
	_x = eventId & 0x1F;
	CSL_FINSR(((CSL_IntcRegsOvly)CSL_INTC_REGS)->EVTCLR[_y],_x,_x,1);
}
/*
 * ======================================================
 *   @func   CSL_intcCombinedEventClear
 * ======================================================
 */
/** @brief 
 *	   Event Clear (EVTCLR0/1/2/3 BitMask programmation)
 */
 /* ======================================================
 *   @arg  eventId
 *		Event Id (0-3)
 *   @arg  clearMask
 *		BitMask of events to be cleared 
 *   @ret  None
 *
 * ======================================================
 */
CSL_IDEF_INLINE
void  CSL_intcCombinedEventClear(
   CSL_IntcEventId               eventId,
   CSL_BitMask32                 clearMask
)
{
	((CSL_IntcRegsOvly)CSL_INTC_REGS)->EVTCLR[eventId] = clearMask;
}
/*
 * ======================================================
 *   @func   CSL_intcCombinedEventGet
 * ======================================================
 */
/** @brief 
 *	   Event query status (MEVTFLAG0/1/2/3 BitMask programmation)
 */
 /* ======================================================
 *   @arg  eventId
 *		Event Id (0-3)
 *   @ret  None
 *
 * ======================================================
 */

CSL_IDEF_INLINE
CSL_BitMask32  CSL_intcCombinedEventGet(
   CSL_IntcEventId               eventId
)
{
	return (((CSL_IntcRegsOvly)CSL_INTC_REGS)->MEVTFLAG[eventId]);
}
/*
 * ======================================================
 *   @func   CSL_intcCombinedEventEnable
 * ======================================================
 */
/** @brief 
 *	   Event enable (EVTMASK0/1/2/3 BitMask programmation)
 */
 /* ======================================================
 *   @arg  eventId
 *		Event Id (0-3)
 *   @arg  enableMask
 *		BitMask of events to be enabled.
 *   @ret  previous state
 *
 * ======================================================
 */
CSL_IDEF_INLINE
CSL_BitMask32  CSL_intcCombinedEventEnable(
   CSL_IntcEventId               eventId,
   CSL_BitMask32                 enableMask
)
{
	CSL_BitMask32 evtMask = ((CSL_IntcRegsOvly)CSL_INTC_REGS)->EVTMASK[eventId];
	((CSL_IntcRegsOvly)CSL_INTC_REGS)->EVTMASK[eventId] = ~enableMask;
	return evtMask;
}
/*
 * ======================================================
 *   @func   CSL_intcCombinedEventDisable
 * ======================================================
 */
/** @brief 
 *	   Event disable (EVTMASK0/1/2/3 BitMask programmation)
 */
 /* ======================================================
 *   @arg  eventId
 *		Event Id (0-3)
 *   @arg  enableMask
 *		BitMask of events to be disabled.
 *   @ret  previous state
 *
 * ======================================================
 */
CSL_IDEF_INLINE
CSL_BitMask32  CSL_intcCombinedEventDisable(
   CSL_IntcEventId               eventId,
   CSL_BitMask32                 enableMask
)
{
	CSL_BitMask32 evtMask = ((CSL_IntcRegsOvly)CSL_INTC_REGS)->EVTMASK[eventId];
	((CSL_IntcRegsOvly)CSL_INTC_REGS)->EVTMASK[eventId] = enableMask;
	return evtMask ;	
}
/*
 * ======================================================
 *   @func   CSL_intcCombinedEventRestore
 * ======================================================
 */
/** @brief 
 *	   Event restore (EVTMASK0/1/2/3 BitMask programmation)
 */
 /* ======================================================
 *   @arg  eventId
 *		Event Id (0-3)
 *   @arg  enableMask
 *		BitMask of events to be set.
 *   @ret  previous state
 *
 * ======================================================
 */
CSL_IDEF_INLINE
void  CSL_intcCombinedEventRestore(
   CSL_IntcEventId               eventId,
   CSL_BitMask32                 restoreMask
)
{
	((CSL_IntcRegsOvly)CSL_INTC_REGS)->EVTMASK[eventId] = restoreMask;

}

/*
 * ======================================================
 *   @func   CSL_intcIntrDropEnable
 * ======================================================
 */
/** @brief 
 *	   Enables interrupts for which drop detection is enabled.
 */
 /* ======================================================
 *   @arg  dropMask
 *		Vector id mask
 *   @ret  CSL_Status - CSL_SOK
 *
 * ======================================================
 */
CSL_IDEF_INLINE
void  CSL_intcInterruptDropEnable(
    CSL_BitMask32      			 dropMask        
)
{
	((CSL_IntcRegsOvly)CSL_INTC_REGS)->INTDMASK &= ~dropMask;
	
}/*
 * ======================================================
 *   @func   CSL_intcIntrDropDisable
 * ======================================================
 */
/** @brief 
 *	   Disables interrupts for which drop detection is enabled.
 */
 /* ======================================================
 *   @arg  dropMask
 *		Vector id mask
 *   @ret  CSL_Status - CSL_SOK
 *
 * ======================================================
 */
CSL_IDEF_INLINE
void  CSL_intcInterruptDropDisable(
    CSL_BitMask32      			 dropMask        
)
{
	((CSL_IntcRegsOvly)CSL_INTC_REGS)->INTDMASK  |= dropMask;
	
}

/*
 * ======================================================
 *   @func   CSL_intcInvokeEventHandle
 * ======================================================
 */
/** @brief 
 *	   This API is for the purpose of excption handler which will need to be
 * written by the user. This API invokes the event handler regisered by 
 * the user at the time of event Open and event handler registration.
 */
 /* ======================================================
 *   @arg  eventId
 *		Event Id of the function to be invoked.
 *   @ret  CSL_Status - CSL_SOK
 *
 * ======================================================
 */
CSL_IDEF_INLINE
CSL_Status CSL_intcInvokeEventHandle (
        CSL_IntcEventId  evtId    /**< Event Id Handle */
)
{

     if (_CSL_intcEventOffsetMap[evtId] != CSL_INTC_MAPPED_NONE) 
			        _CSL_intcEventHandlerRecord[_CSL_intcEventOffsetMap[evtId]].handler (
			        _CSL_intcEventHandlerRecord[_CSL_intcEventOffsetMap[evtId]].arg
	         );  
	 return CSL_SOK;
}
/*
 * ======================================================
 *   @func   CSL_intcQueryEventStatus
 * ======================================================
 */
/** @brief 
 *	   Whether a specified event is enabled.
 */
 /* ======================================================
 *   @arg  eventId
 *		Event Id of the query event.
 *   @ret  Bool
 *
 * ======================================================
 */

CSL_IDEF_INLINE
Bool CSL_intcQueryEventStatus(CSL_IntcEventId eventId)
{
	int _x,_y;
	_y = eventId >> 5;
	_x = eventId & 0x1F;
	return ((Bool)(CSL_FEXTR(((CSL_IntcRegsOvly)CSL_INTC_REGS)->EVTFLAG[_y],_x,_x)));
}
/* CPU Interrupt Handling routines */
/*
 * ======================================================
 *   @func   CSL_intcInterruptEnable
 * ======================================================
 */
/** @brief 
 *	   Interrupt enable.
 */
 /* ======================================================
 *   @arg  vectId
 *		Vector id to enabled
 *   @ret  previous state
 *
 * ======================================================
 */

Uint32
	CSL_intcInterruptEnable (
		CSL_IntcVectId vectId
);
/*
 * ======================================================
 *   @func   CSL_intcInterruptDisable
 * ======================================================
 */
/** @brief 
 *	   Interrupt disable.
 */
 /* ======================================================
 *   @arg  vectId
 *		Vector id to disabled
 *   @ret  previous state
 *
 * ======================================================
 */

Uint32 
	CSL_intcInterruptDisable (
		CSL_IntcVectId vectId
);
/*
 * ======================================================
 *   @func   CSL_intcInterruptRestore
 * ======================================================
 */
/** @brief 
 *	   Interrupt restore.
 */
 /* ======================================================
 *   @arg  vectId
 *		Vector id to disabled
 *   @arg  restoreVal
 *		Value to be restored
 *   @ret  None
 *
 * ======================================================
 */
CSL_IDEF_INLINE
void 
	CSL_intcInterruptRestore ( 
		CSL_IntcVectId vectId,
		Uint32 restoreVal
);

/* Exception handling routines */

/*
 * ======================================================
 *   @func   CSL_intcExcepEnable
 * ======================================================
 */
/** @brief 
 *	   Enables the specific exception event.
 */
 /* ======================================================
 *   @arg  eventId
 *		event id to be enabled
 *   @ret  None
 *
 * ======================================================
 */
CSL_IDEF_INLINE
CSL_IntcEventEnableState  CSL_intcExcepEnable(
   CSL_IntcEventId		eventId
)
{
    int _x,_y,_regVal;
	CSL_IntcEventEnableState _oldState;
	_y = eventId >> 5;
	_x = eventId & 0x1f;
	_regVal = ((CSL_IntcRegsOvly)CSL_INTC_REGS)->EXPMASK[_y];
	_oldState = CSL_FEXTR(_regVal,_x,_x);
	CSL_FINSR(_regVal,_x,_x,0);
	((CSL_IntcRegsOvly)CSL_INTC_REGS)->EXPMASK[_y] = _regVal;
	return _oldState;
}


/*
 * ======================================================
 *   @func   CSL_intcExcepDisable
 * ======================================================
 */
/** @brief 
 *	   Disabled the specific exception event.
 */
 /* ======================================================
 *   @arg  eventId
 *		event id to be disabled
 *   @ret  None
 *
 * ======================================================
 */
CSL_IDEF_INLINE
CSL_IntcEventEnableState  CSL_intcExcepDisable(
   CSL_IntcEventId            eventId
)
{
    int _x,_y,_regVal;
	CSL_IntcEventEnableState oldState;
	_y = eventId >> 5;
	_x = eventId & 0x1f;
	_regVal = ((CSL_IntcRegsOvly)CSL_INTC_REGS)->EXPMASK[_y];
	oldState = CSL_FEXTR(_regVal,_x,_x);
	CSL_FINSR(_regVal,_x,_x,1);
	((CSL_IntcRegsOvly)CSL_INTC_REGS)->EXPMASK[_y] = _regVal;
	return oldState;
}

/*
 * ======================================================
 *   @func   CSL_intcExcepRestore
 * ======================================================
 */
/** @brief 
 *	   Restores the specific exception event.
 */
 /* ======================================================
 *   @arg  eventId
 *		event id to be resored
 *   @arg  restoreVal
 *		restore value
 *   @ret  None
 *
 * ======================================================
 */
CSL_IDEF_INLINE
void  CSL_intcExcepRestore(
   CSL_IntcEventId               eventId,
   Uint32                        restoreVal
)
{
    int _x,_y;
	_y = eventId >> 5;
	_x = eventId & 0x1F;
	CSL_FINSR(((CSL_IntcRegsOvly)CSL_INTC_REGS)->EXPMASK[_y],_x,_x,restoreVal);
}
/*
 * ======================================================
 *   @func   CSL_intcExcepClear
 * ======================================================
 */
/** @brief 
 *	   Clears the specific exception event.
 */
 /* ======================================================
 *   @arg  eventId
 *		event id to be resored
 *   @ret  None
 *
 * ======================================================
 */
CSL_IDEF_INLINE
void  CSL_intcExcepClear(
   CSL_IntcEventId               eventId
)
{
    int _x,_y;
	_y = eventId >> 5;
	_x = eventId & 0x1F;
	CSL_FINSR(((CSL_IntcRegsOvly)CSL_INTC_REGS)->EVTCLR[_y],_x,_x,1);
}

/*
 * ======================================================
 *   @func   CSL_intcInterruptSet
 * ======================================================
 */
/** @brief 
 *	   Interrupt set (ISR).
 */
 /* ======================================================
 *   @arg  vectId
 *		Vector id to set
 *   @ret  None
 *
 * ======================================================
 */

void
	CSL_intcInterruptSet ( 
		CSL_IntcVectId vectId
);
/*
 * ======================================================
 *   @func   CSL_intcInterruptClear
 * ======================================================
 */
/** @brief 
 *	   Interrupt to be cleared (ICR).
 */
 /* ======================================================
 *   @arg  vectId
 *		Vector id to cleared
 *   @ret  None
 *
 * ======================================================
 */

void
	CSL_intcInterruptClear ( 
		CSL_IntcVectId vectId
);

/*
 * ======================================================
 *   @func   CSL_intcQueryInterruptStatus
 * ======================================================
 */
/** @brief 
 *	   Whether a specified CPU interrupt is pending
 */
 /* ======================================================
 *   @arg  vectId
 *		Vector Id queried.
 *   @ret  Bool
 *
 * ======================================================
 */

Bool  CSL_intcQueryInterruptStatus (
		CSL_IntcVectId vectId
);
/**
@}
*/

#endif
/* Rev.No.   Date/Time               ECN No.          Modifier      */
/* -------   ---------               -------          --------      */

/* 1          Aug 11:11:57 9         2274             xkeshavm      */
/*                                                                  */
/* To upload API CSL 0.51 Release                                   */
/********************************************************************/ 
