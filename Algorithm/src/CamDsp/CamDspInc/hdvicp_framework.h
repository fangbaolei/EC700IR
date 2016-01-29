/** ==========================================================================
* @file  hdvicp_framework.h
*
* @path  $\\(PROJDIR)\\..\\include\\  (update this)
*
* @brief  Header file for hdvicp_framework.c
*
* ============================================================================
*
* Copyright (c) Texas Instruments Inc 2007
*
* Use of this software is controlled by the terms and conditions found
* in the license agreement under which this software has been supplied 
*
* ===========================================================================
*
*  <b> Revision History </b> 
*  @n  ================  @n
*     - \c Created : 8 May, 2007
*     .
*/

#ifndef __HDVICP_FRAMEWORK__
#define __HDVICP_FRAMEWORK__

#include <xdas.h>
#include <ialg.h>

typedef struct hdvicpAppHandle
{
    /*! handle size useful when extended */
    XDAS_UInt32    handleSize;
    /*! process-Id */
    XDAS_UInt32    processId;
    /*! application specific information */
    void*       info;
} HDVICP_Obj;

typedef HDVICP_Obj* HDVICP_Handle;

void HDVICP_configure(IALG_Handle handle, 
                      void * hdvicpHandle, 
                      void (*ISRfunctionptr)(IALG_Handle handle));
void HDVICP_wait( void * hdvicpHandle );
void HDVICP_done( void * hdvicpHandle );


#endif

