/*  ============================================================================
 *   Copyright (c) Texas Instruments Inc 2002, 2003, 2004, 2005
 *
 *   Use of this software is controlled by the terms and conditions found in the
 *   license agreement under which this software has been supplied.
 *   ===========================================================================
 */
 
/** ============================================================================
 *   @file  csl_intc.h
 *
 *   @path  $(CSLPATH)\inc
 *
 *   @desc  Header file for functional layer CSL of INTC
 *
 */

/** @mainpage Interrupt Controller 
 *
 *  @section Introduction 
 *  
 *  @subsection xxx Purpose and Scope
 *  The purpose of this document is to detail the  CSL APIs for the
 *  INTC Module.
 *  
 *  @subsection aaa Terms and Abbreviations
 *    -# CSL:  Chip Support Library
 *    -# API:  Application Programmer Interface
 *    -# INTC: Interrupt Controller
 *  
 *  @subsection References
 *     -# CSL 3.x Technical Requirements Specifications Version 0.5, dated
 *        May 14th, 2003
 *     -# Inerrupt Controller Specification 
 *  
 *  @subsection Assumptions
 *     The abbreviations INTC, Intc and intc have been used throughout this
 *     document to refer to Interrupt Controller
 */

/* =============================================================================
 *  Revision History
 *  ===============
 *  12-Jun-2004 Ruchika Kharwar File Created
 *  14-Mar-2005 brn Moved the Event Ids to soc64plus.h
 *  16-Mar-2005 brn modified for doxygen documentation
 * =============================================================================
 */
#ifndef _CSL_INTC_H_
#define _CSL_INTC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <csl.h>
#include <cslr_intc.h>
#include <soc.h>

/** Number of Events in the System */
#define CSL_INTC_EVENTID_CNT        128

/** Indicates there is no associated event-handler */
#define CSL_INTC_EVTHANDLER_NONE    ((CSL_IntcEventHandler) 0) 

/** Invalid handle */
#define CSL_INTC_BADHANDLE          (0) 

/** None mapped */
#define CSL_INTC_MAPPED_NONE        (-1)

/**
 *  Interrupt Vector IDs
 */
typedef enum {
    /** Should be used only along with CSL_intcHookIsr() */
    CSL_INTC_VECTID_NMI      =   1,   
    /** CPU Vector 4 */
    CSL_INTC_VECTID_4        =   4,
    /** CPU Vector 5 */   
    CSL_INTC_VECTID_5        =   5,   
    /** CPU Vector 6 */
    CSL_INTC_VECTID_6        =   6,   
    /** CPU Vector 7 */
    CSL_INTC_VECTID_7        =   7,   
    /** CPU Vector 8 */
    CSL_INTC_VECTID_8        =   8,
    /** CPU Vector 9 */
    CSL_INTC_VECTID_9        =   9,
    /** CPU Vector 10 */   
    CSL_INTC_VECTID_10       =   10,
    /** CPU Vector 11 */   
    CSL_INTC_VECTID_11       =   11,
    /** CPU Vector 12 */  
    CSL_INTC_VECTID_12       =   12,
    /** CPU Vector 13 */  
    CSL_INTC_VECTID_13       =   13,  
    /** CPU Vector 14 */    
    CSL_INTC_VECTID_14       =   14,
    /** CPU Vector 15 */  
    CSL_INTC_VECTID_15       =   15,  
    /** Should be used at the time of opening an Event handle
     *  to specify that the event needs to go to the combiner
     */
    CSL_INTC_VECTID_COMBINE  =   16,  
    /** Should be used at the time of opening an Event handle
     * to specify that the event needs to go to the exception
     * combiner.
     */
    CSL_INTC_VECTID_EXCEP    =   17   
} CSL_IntcVectId; 

/**
 *  Interrupt Event IDs
 */
typedef Int     CSL_IntcEventId; 


/**
 *  Enumeration of the control commands
 *
 *  These are the control commands that could be used with
 *  CSL_intcHwControl(..). Some of the commands expect an
 *  argument as documented along-side the description of
 *  the command.
 */
typedef enum {
        /**
         * @brief   Enables the event
         * @param   CSL_IntcEnableState
         */
    CSL_INTC_CMD_EVTENABLE      = 0,
        /**
         * @brief   Disables the event
         * @param   CSL_IntcEnableState
         */
    CSL_INTC_CMD_EVTDISABLE     = 1,
        /**
         * @brief   Sets the event manually
         * @param   None
         */   
    CSL_INTC_CMD_EVTSET         =2,
        /**
         * @brief   Clears the event (if pending)
         * @param   None
         */
    CSL_INTC_CMD_EVTCLEAR       =3,
        /**
         * @brief   Enables the Drop Event detection feature for this event
         * @param   None
         */    
    CSL_INTC_CMD_EVTDROPENABLE  =4,
        /**
         * @brief   Disables the Drop Event detection feature for this event
         * @param   None
         */       
    CSL_INTC_CMD_EVTDROPDISABLE =5,
        /**
         * @brief   To be used ONLY to invoke the associated Function handle 
         * with Event when the user is writing an exception handling routine. 
         * @param   None
         */             
    CSL_INTC_CMD_EVTINVOKEFUNCTION = 6
} CSL_IntcHwControlCmd;


/**
 *  Enumeration of the queries
 *
 *  These are the queries that could be used with CSL_intcGetHwStatus(..).
 *  The queries return a value through the object pointed to by the pointer
 *  that it takes as an argument. The argument supported by the query is
 *  documented along-side the description of the query.
 */
typedef enum {
        /**
         * @brief   The Pend Status of the Event is queried 
         * @param   Bool
         */  
    CSL_INTC_QUERY_PENDSTATUS
}CSL_IntcHwStatusQuery;


/**
 *  Enumeration of the exception mask registers
 *
 *  These are the symbols used along with the value to be programmed
 *  into the Exception mask register.
 */
typedef enum {
        /**
         * @brief   Symbol for EXPMASK[0]
         * @param   BitMask for EXPMASK0
         */
    CSL_INTC_EXCEP_0TO31 = 0,
        /**
         * @brief   Symbol for EXPMASK[1]
         * @param   BitMask for EXPMASK1
         */
    CSL_INTC_EXCEP_32TO63 = 1,
        /**
         * @brief   Symbol for EXPMASK[2]
         * @param   BitMask for EXPMASK2
         */   
    CSL_INTC_EXCEP_64TO95 = 2,
        /**
         * @brief   Symbol for EXPMASK[3]
         * @param   BitMask for EXPMASK3
         */
    CSL_INTC_EXCEP_96TO127 = 3
} CSL_IntcExcepEn;

/**
 *  Enumeration of the exception
 * These are the symbols used along with the Exception Clear API
 */
typedef enum {
        /**
         * @brief   Symbol for NMI
         * @param   None
         */ 
    CSL_INTC_EXCEPTION_NMI = 31,
        /**
         * @brief   Symbol for External Exception
         * @param   None
         */ 
    CSL_INTC_EXCEPTION_EXT = 30,
        /**
         * @brief   Symbol for Internal Exception
         * @param   None
         */ 
    CSL_INTC_EXCEPTION_INT = 1,
        /**
         * @brief   Symbol for Software Exception
         * @param   None
         */ 
    CSL_INTC_EXCEPTION_SW = 0
} CSL_IntcExcep;

/**
 *  Event Handler pointer
 *
 *  Event handlers ought to conform to this type
 */
typedef void (* CSL_IntcEventHandler)(void *);


/**
 *  Event Handler Record
 *
 *  Used to set-up the event-handler using CSL_intcPlugEventHandler(..)
 */
typedef struct  {
    /** pointer to the event handler */
    CSL_IntcEventHandler    handler;
    /** the argument to be passed to the handler when it is invoked */  
    void *                  arg;        
                                          
} CSL_IntcEventHandlerRecord;

/**
 *  INTC Module Context. 
 */
typedef struct {
    /** Pointer to the event handle record */
    CSL_IntcEventHandlerRecord* eventhandlerRecord;

    /** Event allocation mask */
    CSL_BitMask32   eventAllocMask[(CSL_INTC_EVENTID_CNT + 31) / 32];

    /** Number of event entries */
    Uint16          numEvtEntries;

    /** Reserved */
    Int8            offsetResv[128];
} CSL_IntcContext;


/**
 *  Event enable state
 */
typedef Uint32 CSL_IntcEventEnableState;


/**
 *  Global Interrupt enable state
 */
typedef Uint32 CSL_IntcGlobalEnableState;

/**
 *  The interrupt handle object
 *  This object is used by the handle to identify the event.
 */
typedef struct CSL_IntcObj {
    /** The event-id */
    CSL_IntcEventId eventId;
    /** The vector-id */    
    CSL_IntcVectId   vectId;    
} CSL_IntcObj;

/**
 *  The drop status structure
 *
 *  This object is used along with the CSL_intcQueryDropStatus()
 *  API.
 */
typedef struct {
    /** whether dropped/not */
    Bool drop;
    /** The event-id */                  
    CSL_IntcEventId eventId;
    /** The vect-id */    
    CSL_IntcVectId  vectId;     
}CSL_IntcDropStatus;

/**
 *  INTC module parameters for open
 *
 *  This is equivalent to the Vector Id for the event number.
 */
typedef CSL_IntcVectId CSL_IntcParam;

/**
 *  The interrupt handle
 *
 *  This is returned by the CSL_intcOpen(..) API. The handle is used
 *  to identify the event of interest in all INTC calls.
 */
typedef struct CSL_IntcObj*   CSL_IntcHandle;

/*  ============================================================================
 *   @n@b CSL_intcInit
 *
 *   @b Description
 *   @n This is the initialization function for the INTC. This function is
 *      idempotent in that calling it many times is same as calling it once.
 *      This function initializes the CSL data structures, and doesn't affect
 *      the H/W.
 *
 *   @b Arguments
 *   @verbatim
              pContext     Pointer to module-context structure
             
    @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK - Always returns
 *
 *   <b> Pre Condition </b>
 *   @n  This function should be called before using any of the CSL INTC APIs. 
 *       The context should be initialized such that numEvtEntries is equal to 
 *       the number of records capable of being held in the eventhandlerRecord
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim
    CSL_IntcContext context; 
    CSL_Status intStat;
    CSL_IntcParam vectId;
    CSL_IntcEventHandlerRecord recordTable[10];
    
    context.numEvtEntries = 10;
    context.eventhandlerRecord = &recordTable;

    // Init Module
    ...
    if (CSL_intcInit(&context) != CSL_SOK) {
       exit;
    }
     @endverbatim
 * =============================================================================
 */
CSL_Status CSL_intcInit (
    CSL_IntcContext     *pContext 
);

/*  ============================================================================
 *   @n@b CSL_intcOpen
 *
 *   @b Description
 *   @n The API would reserve an interrupt-event for use. It returns
 *      a valid handle to the event only if the event is not currently
 *      allocated. The user could release the event after use by calling
 *      CSL_intcClose(..). The CSL-object ('intcObj') that the user
 *      passes would be used to store information pertaining handle.
 *
 *   @b Arguments
 *   @verbatim
              pIntcObj     pointer to the CSL-object allocated by the user

              eventId      the event-id of the interrupt

              param        pointer to the Intc specific parameter

              pStatus      (optional) pointer for returning status of the
                           function call
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_IntcHandle
 *   @n                     Valid INTC handle identifying the event
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n   1.    INTC object structure is populated
 *   @n   2.    The status is returned in the status variable. If status
 *              returned is
 *   @li            CSL_SOK             Valid intc handle is returned
 *   @li            CSL_ESYS_FAIL       The open command failed
 *
 *   @b Modifies
 *   @n    1. The status variable
 *   @n    2. INTC object structure
 *
 * @b Example:
 * @verbatim

        CSL_IntcObj                 intcObj20;
        CSL_IntcGlobalEnableState   state;
        CSL_IntcContext             context;
        CSL_Status                  intStat;
        CSL_IntcParam               vectId;

        context.numEvtEntries = 0;
        context.eventhandlerRecord = NULL;
        // Init Module
        CSL_intcInit(&context);

        // NMI Enable
        CSL_intcGlobalNmiEnable();

        // Enable Global Interrupts
        intStat = CSL_intcGlobalEnable(&state);

        // Opening a handle for the Event 20 at vector id 4

        vectId = CSL_INTC_VECTID_4;
        hIntc20 = CSL_intcOpen (&intcObj20, CSL_INTC_EVENTID_20, &vectId , NULL);

        // Close handle
        CSL_IntcClose(hIntc20);
   @endverbatim
 * =============================================================================
 */
CSL_IntcHandle   CSL_intcOpen (
    CSL_IntcObj        *intcObj,    
    CSL_IntcEventId     eventId,    
    CSL_IntcParam      *params,     
    CSL_Status         *status
);

/* =============================================================================
 *   @n@b CSL_intcClose
 *
 *   @b Description
 *   @n This intc Handle can no longer be used to access the event. The event is 
 *    de-allocated and further access to the event resources are possible only after
 *    opening the event object again.
 *
 *   @b Arguments
 *   @verbatim
            hIntc            Handle identifying the event
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK             - Close successful
 *   @li                    CSL_ESYS_FAIL       - Close failed
 *
 *   @b Example
 *   @verbatim
        CSL_IntcContext             context; 
        CSL_Status                  intStat;
        CSL_IntcParam               vectId;
        CSL_IntcObj                 intcObj20;
        CSL_IntcHandle              hIntc20;
        CSL_IntcEventHandlerRecord  recordTable[10];

        context.numEvtEntries = 10;
        context.eventhandlerRecord = &recordTable;

        // Init Module
        ...
        if (CSL_intcInit(&context) != CSL_SOK) {
           exit;
        // Opening a handle for the Event 20 at vector id 4

        vectId = CSL_INTC_VECTID_4;
        hIntc20 = CSL_intcOpen (&intcObj20, CSL_INTC_EVENTID_20, &vectId , NULL);

        // Close handle
        CSL_IntcClose(hIntc20);    
               
     @endverbatim
 * =============================================================================
 */
CSL_Status  CSL_intcClose (
    CSL_IntcHandle      hIntc   
);

/*  ============================================================================
 *  @n@b  
 *
 *  @b Description
 *  @n Associate an event-handler with an event
 *     CSL_intcPlugEventHandler(..) ties an event-handler to an event; so
 *     that the occurence of the event, would result in the event-handler
 *     being invoked.
 *
 *  @b Arguments
 *  @verbatim
        hIntc                Handle identying the interrupt-event

        eventHandlerRecord   Provides the details of the event-handler
    @endverbatim
 *
 *  <b> Return Value </b>
 *  @n  Returns the address of the previous handler
 *
 *  @b Example:
 *  @verbatim
        CSL_IntcObj                 intcObj20;
        CSL_IntcGlobalEnableState   state;
        CSL_IntcContext             context; 
        CSL_Status                  intStat;
        CSL_IntcParam               vectId;

        context.numEvtEntries = 0;
        context.eventhandlerRecord = NULL;
        // Init Module
        CSL_intcInit(&context);

        // NMI Enable   
        CSL_intcGlobalNmiEnable();

        // Enable Global Interrupts 
        intStat = CSL_intcGlobalEnable(&state);

        // Opening a handle for the Event 20 at vector id 4

        vectId = CSL_INTC_VECTID_4;
        hIntc20 = CSL_intcOpen (&intcObj20, CSL_INTC_EVENTID_20, &vectId , NULL);

        EventRecord.handler = &event20Handler;
        EventRecord.arg = hIntc20;
        CSL_intcPlugEventHandler(hIntc20,&EventRecord); 
        // Close handle
        CSL_IntcClose(hIntc20);
        }

        void event20Handler( CSL_IntcHandle hIntc)
        {

        }
    @endverbatim
 * =============================================================================
 */
CSL_Status CSL_intcPlugEventHandler (
    CSL_IntcHandle                  hIntc,
    CSL_IntcEventHandlerRecord      *eventHandlerRecord
);

/*  ============================================================================
 *  @n@b CSL_intcHookIsr
 *
 *  @b Description
 *  @n Hook up an exception handler
 *     This API hooks up the handler to the specified exception.
 *     Note: In this case, it is done by inserting a B(ranch) instruction
 *     to the handler. Because of the restriction in the instruction
 *     th handler must be within 32MB of the exception vector.
 *     Also, the function assumes that the exception vector table is
 *     located at its default ("low") address.
 *
 *  @b Arguments
 *  @verbatim
        vectId       Interrupt Vector identifier

        isrAddr      Pointer to the handler
    @endverbatim
 *
 *  @b Example:
 *  @verbatim
        CSL_IntcContext             context; 
        CSL_Status                  intStat;
        CSL_IntcParam               vectId;
        CSL_IntcObj                 intcObj20;
        CSL_IntcHandle              hIntc20;
        CSL_IntcDropStatus          drop;
        CSL_IntcEventHandlerRecord  recordTable[10];
        CSL_IntcGlobalEnableState   state;
        Uint32 intrStat;

        context.numEvtEntries = 10;
        context.eventhandlerRecord = &recordTable;

        // Init Module
        ...
        if (CSL_intcInit(&context) != CSL_SOK)
           exit;
        // Opening a handle for the Event 20 at vector id 4

        vectId = CSL_INTC_VECTID_4;
        hIntc20 = CSL_intcOpen (&intcObj20, CSL_INTC_EVENTID_20, &vectId , NULL);

        CSL_intcNmiEnable();
        // Enable Global Interrupts 
        intStat = CSL_intcGlobalEnable(&state);

        // Hook Isr appropriately
        CSL_intcHookIsr(CSL_INTC_VECTID_4,&isrVect4);
        ...
         }
        interrupt void isrVect4()
        {

        }
        @endverbatim
 * =============================================================================
 */
CSL_Status CSL_intcHookIsr (
    CSL_IntcVectId  vectId,
    void            *isrAddr
);

/*  ============================================================================
 *   @n@b CSL_intcHwControl
 *
 *   @b Description
 *   @n Perform a control-operation. This API is used to invoke any of the
 *      supported control-operations supported by the module.
 *
 *   @b Arguments
 *   @verbatim
            hIntc           Handle identifying the event

            command         The command to this API indicates the action to be
                            taken on INTC.

            commandArg      An optional argument.
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK             - HwControl successful.
 *   @li                    CSL_ESYS_BADHANDLE  - Invalid handle
 *   @li                    CSL_ESYS_INVCMD     - Invalid command
 *
 *   <b> Pre Condition </b>
 *   @n  CSL_intcOpen() must be invoked before this call.
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n The hardware registers of INTC.
 *
 *   @b Example
 *   @verbatim
        CSL_IntcObj                 intcObj20;
        CSL_IntcGlobalEnableState   state;
        CSL_IntcContext             context; 
        CSL_Status                  intStat;
        CSL_IntcParam               vectId;
        
        context.numEvtEntries = 0;
        context.eventhandlerRecord = NULL;
        
        // Init Module
        CSL_intcInit(&context);

        // NMI Enable   
        CSL_intcGlobalNmiEnable();

        // Enable Global Interrupts 
        intStat = CSL_intcGlobalEnable(&state);

        // Opening a handle for the Event 20 at vector id 4

        vectId = CSL_INTC_VECTID_4;
        hIntc20 = CSL_intcOpen (&intcObj20, CSL_INTC_EVENTID_20, &vectId , NULL);

        CSL_intcHwControl(hIntc20,CSL_INTC_CMD_EVTENABLE,NULL);
     @endverbatim
 * =============================================================================
 */

CSL_Status CSL_intcHwControl (
    CSL_IntcHandle          hIntc,      
    CSL_IntcHwControlCmd    command,    
    void                   *commandArg  
);

/*  ============================================================================
 *   @n@b CSL_intcGetHwStatus
 *
 *   @b Description
 *   @n Queries the peripheral for status. The CSL_intcGetHwStatus(..) API
 *      could be used to retrieve status or configuration information from
 *      the peripheral. The user must allocate an object that would hold
 *      the retrieved information and pass a pointer to it to the function.
 *      The type of the object is specific to the query-command.
 *
 *   @b Arguments
 *   @verbatim
            hIntc           Handle identifying the event

            query           The query to this API of INTC which indicates the
                            status to be returned.

            answer          Placeholder to return the status.
     @endverbatim
 *
 *   <b> Return Value </b> CSL_Status
 *   @li                   CSL_SOK               - Status info return successful
 *   @li                   CSL_ESYS_BADHANDLE    - Invalid handle
 *   @li                   CSL_ESYS_INVQUERY     - Invalid query
 *   @li                   CSL_ESYS_NOTSUPPORTED - Action not supported
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example:
 *   @verbatim
        CSL_IntcContext             context; 
        CSL_Status                  intStat;
        CSL_IntcParam               vectId;
        CSL_IntcObj                 intcObj20;
        CSL_IntcHandle              hIntc20;
        CSL_IntcEventHandlerRecord  recordTable[10];
        CSL_IntcGlobalEnableState   state;
        Uint32                      intrStat;

        context.numEvtEntries = 10;
        context.eventhandlerRecord = &recordTable;

        // Init Module
        ...
        if (CSL_intcInit(&context) != CSL_SOK)
           exit;
        // Opening a handle for the Event 20 at vector id 4

        vectId = CSL_INTC_VECTID_4;
        hIntc20 = CSL_intcOpen (&intcObj20, CSL_INTC_EVENTID_20, &vectId , NULL);

        // NMI Enable   
        CSL_intcGlobalNmiEnable();

        // Enable Global Interrupts 
        intStat = CSL_intcGlobalEnable(&state);

        do {
           CSL_intcGetHwStatus(hIntc20,CSL_INTC_QUERY_PENDSTATUS, \
                              (void*)&intrStat);
        } while (!stat);

        // Close handle
        CSL_IntcClose(hIntc20);

    @endverbatim
 * =============================================================================
 */
CSL_Status CSL_intcGetHwStatus (
    CSL_IntcHandle          hIntc,      
    CSL_IntcHwStatusQuery   query,      
    void                    *response    
);

/**  ============================================================================
 *  @n@b CSL_intcGlobalEnable
 *
 *  @b Description
 *  @n Globally enable interrupts.
 *     The API enables the global interrupt by manipulating the processor's
 *     global interrupt enable/disable flag. If the user wishes to restore
 *     the enable-state at a later point, they may store the current state
 *     using the parameter, which could be used with CSL_intcGlobalRestore(..).
 *     CSL_intcGlobalEnable(..) must be called from a privileged mode.
 *
 *  @b Arguments
 *  @verbatim
        prevState   (Optional) Pointer to object that would store current
                    stateObject that contains information about previous state
    @endverbatim
 *
 *  <b> Return Value </b> CSL_Status
 *  @li                   CSL_SOK on success
 *
 *  @b Example:
 *  @verbatim
        CSL_intcGlobalEnable(NULL);
    @endverbatim
 * =============================================================================
 */
CSL_Status CSL_intcGlobalEnable (
    CSL_IntcGlobalEnableState * prevState   

);

/** ============================================================================
 *  @n@b CSL_intcGlobalDisable
 *
 *  @b Description
 *  @n Globally disable interrupts.
 *     The API disables the global interrupt by manipulating the processor's
 *     global interrupt enable/disable flag. If the user wishes to restore
 *     the enable-state at a later point, they may store the current state
 *     using the parameter, which could be used with CSL_intcGlobalRestore(..).
 *     CSL_intcGlobalDisable(..) must be called from a privileged mode.
 *
 *  @b Arguments
 *  @verbatim
        prevState   (Optional) Pointer to object that would store current
                    stateObject that contains information about previous state
    @endverbatim
 *
 *  <b> Return Value </b> CSL_Status
 *  @li                   CSL_SOK on success
 *
 *  @b Example:
 *  @verbatim
        
        CSL_intcGlobalDisable(NULL);
    @endverbatim
 * =============================================================================
 */
CSL_Status CSL_intcGlobalDisable (
    CSL_IntcGlobalEnableState   *prevState   
);

/** ============================================================================
 *  @n@b CSL_intcGlobalRestore
 *
 *  @b Description
 *  @n Restore global interrupt enable/disable to a previous state.
 *     The API restores the global interrupt enable/disable state to a previous
 *     state as recorded by the global-event-enable state passed as an argument.
 *     CSL_intcGlobalRestore(..) must be called from a privileged mode.
 *
 *  @b Arguments
 *  @verbatim
        prevState   Object containing information about previous state
    @endverbatim
 *
 *  <b> Return Value </b> CSL_Status
 *  @li                   CSL_SOK on success
 *
 *  @b Example:
 *  @verbatim
        CSL_IntcGlobalEnableState   prevState;
 
        CSL_intcGlobalRestore(prevState);
    @endverbatim
 * =============================================================================
 */
CSL_Status CSL_intcGlobalRestore (
    CSL_IntcGlobalEnableState   prevState
);

/** ============================================================================
 *  @n@b CSL_intcGlobalNmiEnable
 *
 *  @b Description
 *  @n This API enables global NMI
 *     
 *
 *  @b Arguments
 *  @verbatim
        None
    @endverbatim
 *
 *  <b> Return Value </b> CSL_Status
 *  @li                   CSL_SOK on success
 *
 *  @b Example:
 *  @verbatim
        
        CSL_intcGlobalNmiEnable();
    @endverbatim
 * =============================================================================
 */
CSL_Status CSL_intcGlobalNmiEnable (void);

/** ============================================================================
 *  @n@b CSL_intcGlobalExcepEnable
 *
 *  @b Description
 *  @n This API enables global exception
 *     
 *
 *  @b Arguments
 *  @verbatim
        None
    @endverbatim
 *
 *  <b> Return Value </b> CSL_Status
 *  @li                   CSL_SOK on success
 *
 *  @b Example:
 *  @verbatim
       CSL_intcGlobalExcepEnable();
    @endverbatim
 * =============================================================================
 */
CSL_Status CSL_intcGlobalExcepEnable (void);  

/** ============================================================================
 *  @n@b CSL_intcGlobalExtExcepEnable
 *
 *  @b Description
 *  @n This API enables external exception
 *     
 *
 *  @b Arguments
 *  @verbatim
        None
    @endverbatim
 *
 *  <b> Return Value </b> CSL_Status
 *  @li                   CSL_SOK on success
 *
 *  @b Example:
 *  @verbatim
        
        CSL_intcGlobalExtExcepEnable();
    @endverbatim
 * =============================================================================
 */
CSL_Status CSL_intcGlobalExtExcepEnable (void);  

/** ============================================================================
 *  @n@b CSL_intcGlobalExcepClear
 *
 *  @b Description
 *  @n This API clears Global Exceptions
 *     
 *
 *  @b Arguments
 *  @verbatim
        exc     Exception to be cleared NMI/SW/EXT/INT 
    @endverbatim
 *
 *  <b> Return Value </b> CSL_Status
 *  @li                   CSL_SOK on success
 *
 *  @b Example:
 *  @verbatim
        CSL_intcGlobalExcepClear(exc);
    @endverbatim
 * =============================================================================
 */
CSL_Status CSL_intcGlobalExcepClear (
    CSL_IntcExcep       exc
);

/*  ============================================================================
 *  @n@b CSL_intcExcepAllEnable
 *
 *  @b Description
 *  @n This API enables all exceptions
 *     
 *
 *  @b Arguments
 *  @verbatim
         excepMask    Exception Mask
         excVal       Event Value
         prevState    Pre state information 
    @endverbatim
 *
 *  <b> Return Value </b> CSL_Status
 *  @li                   CSL_SOK on success
 *
 *  @b Example:
 *  @verbatim
        CSL_IntcExcepEn   excepMask;                  
        CSL_BitMask32         excVal;            
        CSL_IntcEventEnableState *prevState;
     
        CSL_intcExcepAllEnable(excepMask, excVal, prevState);
    @endverbatim
 * =============================================================================
 */
CSL_Status CSL_intcExcepAllEnable (
    CSL_IntcExcepEn     excepMask,
    CSL_BitMask32       excVal,
    CSL_BitMask32       *prevState    
);

/*  ============================================================================
 *  @n@b CSL_intcExcepAllDisable
 *
 *  @b Description
 *  @n This API disables all exceptions
 *     
 *
 *  @b Arguments
 *  @verbatim
         excepMask    Exception Mask
         excVal       Event Value
         prevState    Pre state informationT 
    @endverbatim
 *
 *  <b> Return Value </b> CSL_Status
 *  @li                   CSL_SOK on success
 *
 *  @b Example:
 *  @verbatim
    CSL_IntcExcepEn   excepMask;                  
    CSL_BitMask32     excVal;            
    CSL_IntcEventEnableState *prevState;
     
        CSL_intcExcepAllDisable(excepMask, excVal, prevState);
    @endverbatim
 * =============================================================================
 */
CSL_Status CSL_intcExcepAllDisable (
    CSL_IntcExcepEn             excepMask,
    CSL_BitMask32               excVal,
    CSL_BitMask32               *prevState
);

/*  ============================================================================
 *  @n@b CSL_intcExcepAllRestore
 *
 *  @b Description
 *  @n This API restores all exceptions 
 *     
 *
 *  @b Arguments
 *  @verbatim
         excepMask    Exception Mask 0/1/2/3
         
         prevState    BitMask to be restored
    @endverbatim
 *
 *  <b> Return Value </b> CSL_Status
 *  @li                   CSL_SOK on success
 *
 *  @b Example:
 *  @verbatim
    CSL_IntcExcepEn   excepMask;                  
    CSL_IntcEventEnableState *prevState;
     
        CSL_intcExcepAllRestore(excepMask, prevState);
    @endverbatim
 * =============================================================================
 */
CSL_Status CSL_intcExcepAllRestore (
    CSL_IntcExcepEn             excepMask,
    CSL_IntcGlobalEnableState   restoreVal
);

/*  ============================================================================
 *  @n@b CSL_intcExcepAllClear
 *
 *  @b Description
 *  @n This API clears all exceptions
 *     
 *
 *  @b Arguments
 *  @verbatim
         excepMask    Exception Mask
         
         excVal       Holder for the event bitmask to be cleared
    @endverbatim
 *
 *  <b> Return Value </b> CSL_Status
 *  @li                   CSL_SOK on success
 *
 *  @b Example:
 *  @verbatim
    CSL_IntcExcepEn   excepMask;                  
    CSL_BitMask32     excVal;
     
        CSL_intcExcepAllClear(excepMask, excVal);
    @endverbatim
 * =============================================================================
 */
CSL_Status CSL_intcExcepAllClear (
    CSL_IntcExcepEn excepMask,  
    CSL_BitMask32   excVal      
);

/*  ============================================================================
 *  @n@b CSL_intcExcepAllStatus
 *
 *  @b Description
 *  @n This API is to get the exception status query
 *     
 *
 *  @b Arguments
 *  @verbatim
         excepMask    Exception Mask
         
         status       status of exception
    @endverbatim
 *
 *  <b> Return Value </b> CSL_Status
 *  @li                   CSL_SOK on success
 *
 *  @b Example:
 *  @verbatim
    CSL_IntcExcepEn   excepMask;                  
    CSL_BitMask32     *status;
     
        CSL_intcExcepAllStatus(excepMask, status);
    @endverbatim
 * =============================================================================
 */
CSL_Status CSL_intcExcepAllStatus (
    CSL_IntcExcepEn     excepMask,
    CSL_BitMask32       *status   
);      

/*  ============================================================================
 *  @n@b CSL_intcQueryDropStatus
 *
 *  @b Description
 *  @n This API is to get the exception drop status query
 *     
 *
 *  @b Arguments
 *  @verbatim
         dropStat     place holder for the drop status
               
    @endverbatim
 *
 *  <b> Return Value </b> CSL_Status
 *  @li                   CSL_SOK on success
 *
 *  @b Example:
 *  @verbatim
        CSL_IntcDropStatus   *dropStat;
     
        CSL_intcQueryDropStatus(dropStat);
    @endverbatim
 * =============================================================================
 */
CSL_Status CSL_intcQueryDropStatus (
    CSL_IntcDropStatus   *dropStat
);

#ifdef __cplusplus
}
#endif

#endif
