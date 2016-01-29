/*******************************************************************************
**+--------------------------------------------------------------------------+**
**|                            ****                                          |**
**|                            ****                                          |**
**|                            ******o***                                    |**
**|                      ********_///_****                                   |**
**|                      ***** /_//_/ ****                                   |**
**|                       ** ** (__/ ****                                    |**
**|                           *********                                      |**
**|                            ****                                          |**
**|                            ***                                           |**
**|                                                                          |**
**|         Copyright (c) 1998-2006 Texas Instruments Incorporated           |**
**|                        ALL RIGHTS RESERVED                               |**
**|                                                                          |**
**| Permission is hereby granted to licensees of Texas Instruments           |**
**| Incorporated (TI) products to use this computer program for the sole     |**
**| purpose of implementing a licensee product based on TI products.         |**
**| No other rights to reproduce, use, or disseminate this computer          |**
**| program, whether in part or in whole, are granted.                       |**
**|                                                                          |**
**| TI makes no representation or warranties with respect to the             |**
**| performance of this computer program, and specifically disclaims         |**
**| any responsibility for any damages, special or consequential,            |**
**| connected with the use of this program.                                  |**
**|                                                                          |**
**+--------------------------------------------------------------------------+**
*******************************************************************************/

/** \file   bios_edma3_rm_sample.h

    \brief Header file for the Demo application for the EDMA3 Resource Manager.

    (C) Copyright 2006, Texas Instruments, Inc

    \version    1.0   Anuj Aggarwal         - Created
                1.1   Anuj Aggarwal         - Removed EDMA3 driver related
                                              #defines

 */

#ifndef _BIOS_EDMA3_RM_SAMPLE_H_
#define _BIOS_EDMA3_RM_SAMPLE_H_


#include <edma3_rm.h>


/**
 * \brief SoC specific TC related information. Specified in the sample
 * configuration file (bios_edma3_sample_cfg.c).
 */
extern unsigned int numEdma3Tc;
extern unsigned int ccXferCompInt;
extern unsigned int ccErrorInt;
extern unsigned int tcErrorInt[8];


/**
 * \brief Semaphore Types
 *
 * Enums for different semaphore types
 */
typedef enum
{
  EDMA3_OS_SEMTYPE_FIFO         = 0,
  EDMA3_OS_SEMTYPE_PRIORITY     = 1
} EDMA3_OS_SemType;


/**
 * \brief Semaphore Attributes Data Structure
 *
 * Data structure for Semaphore Attributes
 */
typedef struct
{
  EDMA3_OS_SemType          type;
  unsigned int              memSegId;
}EDMA3_OS_SemAttrs;




/**
  * Counting Semaphore related functions (OS dependent) should be
  * called/implemented by the application. A handle to the semaphore
  * is required while opening the driver/resource manager instance.
  */

/**
 * \brief   EDMA3 OS Semaphore Create
 *
 *      This function creates a counting semaphore with specified
 *      attributes and initial value. It should be used to create a semaphore
 *      with initial value as '1'. The semaphore is then passed by the user
 *      to the EDMA3 RM for proper sharing of resources.
 * \param   initVal [IN] is initial value for semaphore
 * \param   attrs [IN] is the semaphore attributes ex: Fifo type
 * \param   hSem [OUT] is location to recieve the handle to just created
 *      semaphore
 * \return  EDMA3_RM_SOK if succesful, else a suitable error code.
 */
EDMA3_RM_Result edma3OsSemCreate(int initVal,
                           const EDMA3_OS_SemAttrs *attrs,
                           EDMA3_OS_Sem_Handle *hSem);



/**
 * \brief   EDMA3 OS Semaphore Delete
 *
 *      This function deletes or removes the specified semaphore
 *      from the system. Associated dynamically allocated memory
 *      if any is also freed up.
 * \warning OsSEM services run in client context and not in a thread
 *      of their own. If there exist threads pended on a semaphore
 *      that is being deleted, results are undefined.
 * \param   hSem [IN] handle to the semaphore to be deleted
 * \return  EDMA3_RM_SOK if succesful else a suitable error code
 */
EDMA3_RM_Result edma3OsSemDelete(EDMA3_OS_Sem_Handle hSem);

#endif

