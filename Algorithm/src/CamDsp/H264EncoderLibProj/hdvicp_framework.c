/* ----------------------- SYSTEM AND PLATFORM FILES ------------------------*/
#include <tistdtypes.h>
#include <std.h>
#include <xdas.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <c6x.h>
#include <ividenc1.h>
#include <ires_hdvicp.h>
#define NUM_HDVICP_PROCESSORS 2

/* ------------------------------ PROGRAM FILES -----------------------------*/
 
#include "hdvicp_framework.h"

/*---------------------------------------------------------------------------*/
/*  CSL files to be included for programming GEM interrupt controller.       */ 
/*---------------------------------------------------------------------------*/
#include "csl_intc.h"
#include "cslr_intc_001.h"
#include "cslr_kldintc.h"

/*---------------------------------------------------------------------------*/
/*  Global variables to be used for initializing the GEM interrupt controller*/
/*---------------------------------------------------------------------------*/
CSL_IntcContext             GemIntcContext; 
CSL_IntcEventHandlerRecord  Record[sizeof(CSL_IntcEventHandlerRecord)];

/*---------------------------------------------------------------------------*/
/*  Global Algorithm handle which will be initialized with the argument to   */
/*  the algorithm's MB processing function.                                  */
/*---------------------------------------------------------------------------*/
IALG_Handle g_handle[NUM_HDVICP_PROCESSORS];

/*---------------------------------------------------------------------------*/
/*  Function used for programming the GEM interrupt controller.              */
/*---------------------------------------------------------------------------*/
static void GemIntcInit
(
      CSL_IntcEventHandler       pfGemIntrpHandler,
      void *                     pVIntrHandlerArgs, 
      CSL_IntcEventHandlerRecord *Record,
      XDAS_Int8                  imcop_sel
);

/* ---------------------------- FUNCTION PROTOTYPES -------------------------*/

/*---------------------------------------------------------------------------*/
/*  Global function pointer which will be initialized to the algorithm's MB  */
/*  processing function and will be called by the gem interrupt handler      */
/*  function.                                                                */
/*---------------------------------------------------------------------------*/
void (* g_fnPtr[NUM_HDVICP_PROCESSORS])(IALG_Handle handle);

/*---------------------------------------------------------------------------*/
/*  The gem interrupt handler function. The application ties down this func  */
/*  as service routine to the interrupts received by GEM from ARM968.        */
/*---------------------------------------------------------------------------*/
void gem_int_handler(void *x);

/*---------------------------------------------------------------------------*/
/*  This global flag is part of the current implementation of hdvicp_wait()  */
/*  to keep GEM waiting for an interrupt from ARM968. If this flag gets set  */
/*  to 1, GEM comes out to wait to do post encode settings.                  */
/*---------------------------------------------------------------------------*/

volatile XDAS_Int8 global_flag;
//extern volatile XDAS_Int8 frameSkipFlagGlobal;

/*===========================================================================*/
/*
 *@brief  This is the top level function on the gem side, which sets the 
 *        interrupt configuration for the interrupts received from ARM968.
 *        In effect, this function currently sets the global function ptr   
 *        to the actual MB processing decode function that needs to be called
 *        from the interrupt service routine.
 *
 *@param  handle                      Algorithm handle
 *
 *@param  hdvicpHandle                void pointer to the hdvicp handle.
 *
 *@param  (*ISRfunctionptr)(handle)   Function pointer to the MB pair 
 *                                    processing function of the algorithm.
 *
 *@return none
 *
 *@note   As per XDM, this function will be called from the algorithm but the
 *        implementation will lie in the application.
 */
/*===========================================================================*/

void HDVICP_configure(IALG_Handle handle,
                      void *hdvicpHandle,
                      void (*ISRfunctionptr)(IALG_Handle handle))
{
    int processorId = ((IRES_HDVICP_Handle)hdvicpHandle)->id;

     /*-----------------------------------------------------------------------*/
	/* Initialize the global_flag to 1 initially                             */
	/*-----------------------------------------------------------------------*/
	global_flag = 1;

    /*-----------------------------------------------------------------------*/
    /*  Initialize the argument to the MB processing function that is        */
	/*  required to be called by the ISR.                                    */
    /*-----------------------------------------------------------------------*/
    g_handle[processorId] = handle;
    
    /*-----------------------------------------------------------------------*/
    /*  Initialize the global function ptr to the MB processing function     */
    /*  which will be called from the ISR.                                   */ 
    /*-----------------------------------------------------------------------*/
    g_fnPtr[processorId]  = ISRfunctionptr;

      /*---------------------------------------------------------------------*/
      /*  The following function initializes and ties down the GEM interrupt */
      /*  handler to the appropriate Imcop based on the values set above.    */
      /*  The ISR function is the gem_int_handler() and has the pointer to   */
      /*  the alg handle as its argument.                                    */ 
      /*---------------------------------------------------------------------*/
      GemIntcInit
      (
           gem_int_handler,
           (void *)&((IRES_HDVICP_Handle)hdvicpHandle)->id, //handle, 
           Record,
           processorId
      );
    return;
}

/*===========================================================================*/
/*
 *@brief  This is the top level function on the gem side, which corresponds
 *        to the actual interrupt service routine for interrupts received 
 *        by GEM. The bare minimum that the application needs to do in this 
 *        function is to make the call to the global function pointer with 
 *        its corresponding argument which were initialized by the algorithm's
 *        call to the hdvicp_configure() function. This ensures that the MB
 *        processing command setup of the decode gets executed. Currently, 
 *        only this bare minimum call is made in this function.                           
 *
 *@param  x         void pointer to the argument of ISR
 *
 *@return none
 */
/*===========================================================================*/
void gem_int_handler (void *processorId)
{
  (*g_fnPtr[* ((int *)processorId)])(g_handle[* ((int *)processorId)]);

  return;
}

/*===========================================================================*/
/*
 *@brief  This is the top level function on the gem side, which implements
 *        the hdvicp_wait(). Current implementation allows GEM to wait on a 
 *        global intc flag which will be 1 as long as one frame is not 
 *        completed.
 *
 *@param  hdvicpHandle      void pointer to the hdvicp handle.
 *
 *@return none
 *
 *@note   As per XDM, this function will be called from the algorithm but the
 *        implementation will lie in the application.
 */
/*===========================================================================*/
void HDVICP_wait(void *hdvicpHandle)
{
  /*-------------------------------------------------------------------------*/  
  /*  Set the task priority of GEM decode to low over here                   */
  /*-------------------------------------------------------------------------*/

  while (global_flag);//(global_flag == 0));// && (frameSkipFlagGlobal == 0)); 
}
/*===========================================================================*/
/*
 *@brief  This is the top level function on the gem side, which implements
 *        the hdvicp_done(). Current implementation allows GEM to set a 
 *        global intc flag to 0 to come out of hdvicp_wait() as soon as one 
 *        frame is completed.
 *
 *@param  hdvicpHandle      void pointer to the hdvicp handle.
 *
 *@return none
 *
 *@note   As per XDM, this function will be called from the algorithm but the
 *        implementation will lie in the application.
 */
/*===========================================================================*/

void HDVICP_done(void *hdvicpHandle)
{
  /*-------------------------------------------------------------------------*/
  /* This function will be called by the interrupt handler function when it  */
  /* detects an end-of-frame processing.                                     */
  /*-------------------------------------------------------------------------*/
    global_flag = 0;
}

/* ==========================================================================
 *@func   GemIntcInit
 *
 *@brief  This function enables/ties down a interrupt regs/handler to the 
 *        interrupts from the Kaleido HDIMCOP0/1, based on the input 
 *        parameter supplied by the user.
 *
 *@param  pfGemIntrpHandler  : Function ptr to the ISR
 *
 *@param  pVIntrHandlerArgs  : Contains the ISR args. 
 *
 *@param  Record             : To store the argument of interrupt handler
 *
 *@param  imcop_sel          : imcop value for which the interrupt programming
 *                             needs to be done
 *@return None
 *
 *@note   None
 *==========================================================================
*/
void GemIntcInit(
                  CSL_IntcEventHandler       pfGemIntrpHandler,
                  void *                     pVIntrHandlerArgs, 
                  CSL_IntcEventHandlerRecord *Record,
                  XDAS_Int8                  imcop_sel
                )
{
    /*-------------------------------------------------------------------*/
    /*  Enable the GEM INTC. This is required as in the interrupt mode   */
    /*  the ARM shall signal the GEM by using interrupt.                 */
    /*                                                                   */
    /*  Here we do the following:                                        */
    /*  1. Enable interrupts in GEM.                                     */
    /*  2. Enable the GEM INTC event to recieve appropriate message from */  
    /*     ARM and also enable GEM to interrupt ARM.                     */
    /*                                                                   */
    /*-------------------------------------------------------------------*/
    CSL_IntcObj                 tIntcKldIntcObj;
    CSL_IntcHandle              tIntcKldHandle;
    CSL_IntcEventHandlerRecord  EventRecord;

    CSL_Status  tStatus;  
    CSL_IntcParam tVectId     = CSL_INTC_VECTID_4;
    CSL_IntcEventId tEventId  = (imcop_sel ?
                          CSL_INTC_EVENTID_CP_ARM1 : CSL_INTC_EVENTID_CP_ARM0);

    /*-----------------------------------------------------------------------*/
    /*                             ENABLE INTERRUPTS                         */
    /*-----------------------------------------------------------------------*/
    /*                                                                       */
    /*  The GEM interrupts at the reset are disabled. For the GEM to be      */
    /*  able to recieve interrupts from Interrupt controller, the interrupts */
    /*  need to be enabled. For this we need to:                             */
    /*  1. Enable GIE bit in CSR                                             */
    /*  2. Enable NMIE bit in IER.                                           */
    /*                                                                       */
    /*-----------------------------------------------------------------------*/
    tStatus  = CSL_intcGlobalEnable(0);
    tStatus |= CSL_intcGlobalNmiEnable(); 

    /*-----------------------------------------------------------------------*/
    /*  Check for status to return flag to detect errors.                    */
    /*-----------------------------------------------------------------------*/
    if(tStatus != CSL_SOK)
    {
        printf("GEM Interrupts Enable Failure.....\n");     
    }
      
    /*-----------------------------------------------------------------------*/
    /*  Intialize the GEM INTC context                                       */
    /*-----------------------------------------------------------------------*/
    GemIntcContext.numEvtEntries = 1;
    GemIntcContext.eventhandlerRecord = Record;

    tStatus = CSL_intcInit(&GemIntcContext); 

    /*-----------------------------------------------------------------------*/
    /*  Check for status to return flag to detect errors.                    */
    /*-----------------------------------------------------------------------*/
    if(tStatus != CSL_SOK)
    {
       printf("GEM INTC CSL Intialization Failed......\n");    
    }

    /*-----------------------------------------------------------------------*/
    /*  Open the INTC event for the ARM968 to send interrupts to GEM.        */
    /*-----------------------------------------------------------------------*/
    tIntcKldHandle = CSL_intcOpen(
                                   &tIntcKldIntcObj, 
                                   tEventId, 
                                   &tVectId,
                                   NULL
                                 ); 

    /*-----------------------------------------------------------------------*/
    /*  Plug the Event handler to handle the Kaleido interrupt event         */
    /*-----------------------------------------------------------------------*/
    EventRecord.arg    = (void *)pVIntrHandlerArgs;
    EventRecord.handler = pfGemIntrpHandler;
  
    tStatus = CSL_intcPlugEventHandler(
                                         tIntcKldHandle, 
                                         &EventRecord
                                      );

    /*-----------------------------------------------------------------------*/
    /*  Check for status to return flag to detect errors.                    */
    /*-----------------------------------------------------------------------*/
    if(tStatus != CSL_SOK)
    {
        printf("Failed to plug event handler.......\n");  
    }

    /*-----------------------------------------------------------------------*/
    /*  Enable the EVENT                                                     */
    /*-----------------------------------------------------------------------*/
    tStatus = CSL_intcHwControl(tIntcKldHandle, CSL_INTC_CMD_EVTENABLE, NULL);

    if(tStatus != CSL_SOK)
    {
        printf("Hardware event enable failed......\n");         
    }

}/* GemIntcInit */
