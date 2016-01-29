/*   ==========================================================================
 *   Copyright (c) Texas Instruments Inc , 2004
 *
 *   Use of this software is controlled by the terms and conditions found
 *   in the license agreement under which this software has been supplied
 *   provided
 *   ==========================================================================
*/
/** @file csl_ddrGetHwStatus.c
 *
 *  @brief    File for functional layer of CSL API @a CSL_ddrGetHwStatus()
 *
 *  @date   02 August, 2004
 *  @author Pratheesh Gangadhar
 
 */

#include <csl_ddr.h>


#pragma CODE_SECTION (CSL_ddrGetHwStatus, ".text:csl_section:ddr");
/**
    \brief Get DDR status

    \param hDdr  Pointer to the object that holds reference to the instance
                 of DDR requested
   
    \param query  The query to this API which indicates the status/setup to
                  be returned 

    \param response  Pointer to query response object     

    \param status  Placeholder to return the status; @a void* casted  

    \return if success, \c CSL_SOK, else error code

    \see 
*/
CSL_Status CSL_ddrGetHwStatus ( CSL_DdrHandle hDdr,
                                CSL_DdrHwStatusQuery query,
                                void *response ) {
  CSL_Status status = CSL_SOK;
  CSL_DdrRegsOvly ddrRegs = hDdr->regs;
  
  switch (query) {  
  case CSL_DDR_QUERY_PID:
    *(Uint32*)response = ddrRegs->ERCSR;
    break;
  case CSL_DDR_QUERY_DDRPHY_PID:
    *(Uint32*)response = ddrRegs->DDRPHYREV;
    break;
  case CSL_DDR_QUERY_SDRAM_STATUS:
    *(Uint32*)response = ddrRegs->SDRSTAT;
    break;
  case CSL_DDR_QUERY_DDRPHY_STATUS:
    *(Uint32*)response = ddrRegs->DDRPHYSR;
    break;
  case CSL_DDR_QUERY_INTR_STATUS:
    *(Uint32*)response = ddrRegs->IRR;
    break;
  case CSL_DDR_QUERY_INTMSKD_STATUS:
    *(Uint32*)response = ddrRegs->IMR;
    break; 
  case CSL_DDR_QUERY_PERFCOUNT1:
    *(Uint32*)response = ddrRegs->PERFC1R;
    break;
  case CSL_DDR_QUERY_PERFCOUNT2:
    *(Uint32*)response = ddrRegs->PERFC2R;
    break;
  case CSL_DDR_QUERY_VTPIO_STATUS:
    *(Uint32*)response = ddrRegs->VTPSTAT;
    break;    
  default:
    status = CSL_ESYS_INVQUERY ;	
    break;	
  }
  
  return status;

}
/* Rev.No.   Date/Time               ECN No.          Modifier      */
/* -------   ---------               -------          --------      */

/* 3          Aug 10:00:22 9         2271             xkeshavm      */
/*                                                                  */
/* To upload API CSL 0.51 Release                                   */
/********************************************************************/ 
