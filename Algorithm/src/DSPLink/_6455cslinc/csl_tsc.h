/*  ============================================================================
 *   Copyright (c) Texas Instruments Inc 2002, 2003, 2004,2005
 *
 *   Use of this software is controlled by the terms and conditions found
 *   in the license agreement under which this software has been supplied
 *   provided
 *  ============================================================================
*/

/** ============================================================================
 *   @file  csl_tsc.h
 *
 *   @path  $(CSLPATH)\inc
 *
 *   @desc  Header file for functional layer of CSL
 *
 */
 
/** @mainpage Time Stamp Counter Module
 *
 *  @section Introduction
 *  
 *  @subsection xxx Purpose and Scope
 *  The purpose of this document is to identify a set of common CSL APIs for 
 *  the Time Stamp Counter 
 *  
 *  @subsection aaa Terms and Abbreviations
 *    -# CSL     :  Chip Support Library
 *    -# API     :  Application Programmer Interface
 *    -# TSC     :  Time Stamp Counter
 *  
 *  @subsection Assumptions
 *      The abbreviations CSL, TSC have been used throughout this
 *      document to refer to the C64Plus Time Stamp Counter
 */
  
/* =============================================================================
 *  Revision History
 *  ===============
 *  09-Jun-2004 Chad Courtney File Created.
 *  
 *  14-Nov-2005 ds    - Removed CSL_ctimerStart() and CSL_ctimerRead ().
 *                    - Added CSL_tscEnable() and CSL_tscRead ()
 * =============================================================================
 */
 
#ifndef _CSL_TSC_H_
#define _CSL_TSC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <csl.h>

/** ===========================================================================
 *   @n@b CSL_tscEnable
 *
 *   @b Description
 *   @n This API enable the 64 bit time stamp counter. The Time Stamp Counter
 *      (TSC) stops only upon Reset or powerdown. When time stamp counter is 
 *      enabled (following a reset or powerdown of the CPU) it will initialize 
 *      to 0 and begin incrementing once per CPU cycle. You cannot reset the 
 *      time stamp counter. 
 *
 *   @b Arguments          
 *   @n None
 *
 *   <b> Return Value </b>  
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  Time Stamp Counter value starts incrementing
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
        
        CSL_tscEnable ();
        
 *   @endverbatim
 * ============================================================================
 */
extern void  CSL_tscEnable(void);

/** ============================================================================
 *   @n@b CSL_tscRead
 *
 *   @b Description
 *   @n Reads the 64 bit timer stamp conter and return the 64-bit counter value.
 *
 *   @b Arguments
 *   @verbatim
            None
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Uint64
 *   @li                    64 Bit Time Stamp Counter value
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies            None
 *
 *   @b Example
 *   @verbatim
        CSL_Uint64        counterVal;
        
        ...
        
        CSL_tscStart();
        counterVal = CSL_tscRead();
        
 *   @endverbatim
 * ============================================================================
 */
extern CSL_Uint64 CSL_tscRead (void);

#ifdef __cplusplus
}
#endif

#endif
