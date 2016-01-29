/*   ==========================================================================
 *   Copyright (c) Texas Instruments Inc , 2004
 *
 *   Use of this software is controlled by the terms and conditions found
 *   in the license agreement under which this software has been supplied
 *   provided
 *   ==========================================================================
*/
/** @file csl_ddrHwControl.c
 *
 *    @brief    File for functional layer of CSL API @a CSL_ddrHwControl()
 *
 *  Description
 *    - The @a CSL_ddrHwControl() function definition & it's associated
 *      functions
 *  @date   02 August, 2004
 *  @author Pratheesh Gangadhar
 */

#include <csl_ddr.h>

#pragma CODE_SECTION (CSL_ddrHwControl, ".text:csl_section:ddr");
/**
    \brief Control DDR module


    \param hDdr  Pointer to the object that holds reference to the instance
                 of DDR requested
    \param cmd   Command to perform an operation on DDR

    \param arg   Arguments if any for the command

    \return if success, \c CSL_SOK, else error code
    \see 
*/

CSL_Status CSL_ddrHwControl ( CSL_DdrHandle hDdr,
                              CSL_DdrHwControlCmd cmd,
                              void *arg ) {
  CSL_Status status = CSL_SOK;
  CSL_DdrRegsOvly ddrRegs = hDdr->regs;
  
  switch (cmd) {
  case CSL_DDR_CMD_INTR_ENABLE:
    ddrRegs->IMSR = *(CSL_BitMask32*)arg;
    break;
  case CSL_DDR_CMD_INTR_DISABLE:
    ddrRegs->IMCR = *(CSL_BitMask32*)arg;
    break;
  case CSL_DDR_CMD_CLEAR_INTRSTATUS:
    ddrRegs->IRR = *(CSL_BitMask32*)arg;
    break;   
  case CSL_DDR_CMD_DDRPHY_CONTROL:
    ddrRegs->DDRPHYCR = *(Uint32*)arg;
    break;
  case CSL_DDR_CMD_VTPIO_CONTROL:
    ddrRegs->VTPCTRL = *(Uint32*)arg;
    break;          
  default:
    status = CSL_ESYS_INVCMD;
    break;
  }

  return status;
}

/* Rev.No.   Date/Time               ECN No.          Modifier      */
/* -------   ---------               -------          --------      */

/* 3          Aug 10:00:23 9         2271             xkeshavm      */
/*                                                                  */
/* To upload API CSL 0.51 Release                                   */
/********************************************************************/ 
