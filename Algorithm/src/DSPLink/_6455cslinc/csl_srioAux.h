/* ============================================================================
 * Copyright (c) Texas Instruments Inc 2002, 2003, 2004, 2005
 *
 * Use of this software is controlled by the terms and conditions found in the
 * license agreement under which this software has been supplied.
 * =========================================================================
 */

/** ===========================================================================
 * @file csl_srioAux.h
 *
 * @brief API Auxilary header file for SRIO CSL
 * 
 * @path $(CSLPATH)\srio\inc
 *  
 * @desc It gives the definitions of the status query and control functions.
 * ============================================================================
 */

/* ============================================================================
 * Revision History
 * ===============
 * 26-Aug-2005 PSK File Created.
 * ============================================================================
 */

#ifndef _CSL_SRIOAUX_H_
#define _CSL_SRIOAUX_H_

#include <csl_srio.h>

#ifdef __cplusplus
extern "C" {
#endif

/** ============================================================================
 *   @n@b CSL_SrioDoorbellIntrClear
 *
 *   @b Description
 *   @n This function Clears doorbell interrupts. Macros can be OR'ed to get 
 *      the value
 *
 *   @b Arguments
 *   @verbatim
         hSrio        Pointer to the object that holds reference to the 
                     instance of SRIO   

         clearData   pointer to the clear value
     @endverbatim
 *
 *   <b> Return Value </b>  None
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
        CSL_SrioHandle   hSrio;
        CSL_SrioPortData clearData;

        clearData.data = CSL_SRIO_DOORBELL_INTR0 |
                         CSL_SRIO_DOORBELL_INTR1 |
                         CSL_SRIO_DOORBELL_INTR2;
        clearData.index = 1;
        ...
        CSL_SrioDoorbellIntrClear(hSrio, &clearData);
        ...
     @endverbatim
 * ============================================================================
 */
static inline
 void CSL_SrioDoorbellIntrClear (
    CSL_SrioHandle   hSrio,
    CSL_SrioPortData *clearData
)
{
    hSrio->regs->DOORBELL_INTR[clearData->index].DOORBELL_ICCR = 
                                                            clearData->data;
}



/** ===========================================================================
 *   @n@b CSL_SrioSrcAddrSet
 *
 *   @b Description
 *   @n This function Sets 32-bit DSP byte source address
 *
 *   @b Arguments
 *   @verbatim
         hSrio        Pointer to the object that holds reference to the 
                     instance of SRIO  
                        
         arg     contains the source address and index
     @endverbatim
 *
 *   <b> Return Value </b>  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n None
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim
        CSL_SrioHandle hSrio;
        CSL_SrioPortData   arg;
        
        arg.data = 0x050;
        arg.index = 1; //index to the LSU BLOCKs
        ...
        CSL_SrioSrcAddrSet(hSrio, &arg);
        ...
     @endverbatim
 * ============================================================================ 
 */
static inline
void CSL_SrioSrcAddrSet (
    CSL_SrioHandle   hSrio,
    CSL_SrioPortData *arg
)
{
    hSrio->regs->LSU[arg->index].LSU_REG2 = arg->data;
}



/** ===========================================================================
 *   @n@b CSL_SrioDstAddrMsbSet
 *
 *   @b Description
 *   @n This function Sets the rapid IO destination MSB address
 *
 *   @b Arguments
 *   @verbatim
         hSrio      Pointer to the object that holds reference to the 
                    instance of SRIO  
         arg        pointer to the structure that contains destination MSB 
                    address and index to LSU registers set  
     @endverbatim
 *
 *   <b> Return Value </b>  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *    @n None
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim
        CSL_SrioHandle  hSrio;
        CSL_SrioPortData arg;

        arg.index = 2; 
        arg.data = 0x02e00000; // destination address
        ...
        CSL_SrioDstAddrMsbSet(hSrio, &arg);
        ...
     @endverbatim
 * ===========================================================================
 */
static inline
void CSL_SrioDstAddrMsbSet (
    CSL_SrioHandle   hSrio, 
    CSL_SrioPortData *arg
)
{
   hSrio->regs->LSU[arg->index].LSU_REG0 = arg->data;
}



/** ===========================================================================
 *   @n@b CSL_SrioDstAddrLsbSet
 *
 *   @b Description
 *   @n This function Sets the rapid IO destination LSB address
 *
 *   @b Arguments
 *   @verbatim
        hSrio       Pointer to the object that holds reference to the 
                    instance of SRIO  
        arg         pointer to the structure that contains destination LSB 
                    address and index to LSU registers set
     @endverbatim
 *
 *   <b> Return Value </b>  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *    @n None
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim
        CSL_SrioHandle  hSrio;
        CSL_SrioPortData arg;
        arg.index = 2; 
        arg.data = 0x02e00000; // destination address
        ...
        CSL_SrioDstAddrLsbSet(hSrio, &arg);
        ...
     @endverbatim
 * ============================================================================
 */
static inline
void CSL_SrioDstAddrLsbSet (
    CSL_SrioHandle   hSrio, 
    CSL_SrioPortData *arg
)
{
   hSrio->regs->LSU[arg->index].LSU_REG1 = arg->data;
}



/** ===========================================================================
 *   @n@b CSL_SrioLsuXfrByteCntSet
 *
 *   @b Description
 *   @n This function sets the Number of data bytes to Read/Write - up to 4KB
 *
 *   @b Arguments
 *   @verbatim

         hSrio      Pointer to the object that holds reference to the 
                    instance of SRIO  
         arg        pointer to the structure that contains Number of data bytes 
                    to Read/Write and index to LSU registers set

     @endverbatim
 *
 *   <b> Return Value </b>  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *    @n None
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim
        CSL_SrioHandle          hSrio;
        CSL_SrioPortData        arg;

        arg.index = 2; 
        arg.data = 0x0f; 
        ...
        CSL_SrioLsuXfrByteCntSet(hSrio, &arg);
        ...
     @endverbatim
 * ===========================================================================
 */
static inline
void CSL_SrioLsuXfrByteCntSet (
    CSL_SrioHandle   hSrio, 
    CSL_SrioPortData *arg
)
{
    CSL_FINS(hSrio->regs->LSU[arg->index].LSU_REG3, 
                                        SRIO_LSU_REG3_BYTE_COUNT, arg->data);
}



/** ============================================================================
 *   @n@b CSL_SrioLsuXfrTypeSet
 *
 *   @b Description
 *      Sets 4 MSBs to 4-bit ftype field for all packets 
 *       and 4 LSBs to 4-bit trans field for Packet types 2,5 and 8
 *
 *   @b Arguments
 *   @verbatim
         hSrio       Pointer to the object that holds reference to the 
                     instance of SRIO  

         arg       pointer to the structure that contains transfer type
                    and index to LSU registers set
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
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
        CSL_SrioHandle hSrio;
        CSL_SrioPortData   arg;

        arg.index = 2; 
        arg.data = 2; //packet type
        ...
        CSL_SrioLsuXfrTypeSet(hSrio, &arg);
        ...
     @endverbatim
 * ===========================================================================
 */
static inline
void CSL_SrioLsuXfrTypeSet (
    CSL_SrioHandle   hSrio,
    CSL_SrioPortData *arg
)
{
    CSL_FINS(hSrio->regs->LSU[arg->index].LSU_REG5, 
                                        SRIO_LSU_REG5_PACKET_TYPE, arg->data);
}



/** ============================================================================
 *   @n@b CSL_SrioDoorbellXfrTypeSet
 *
 *   @b Description
 *      Sets RapidIO doorbell info field for type 10 packets and sets the 
 *      packet type to 10.
 *
 *   @b Arguments
 *   @verbatim
         hSrio       Pointer to the object that holds reference to the 
                     instance of SRIO  

         arg     pointer to the structure that contains doorbell info
                    and index to LSU registers set
     @endverbatim
 *
 *   <b> Return Value </b>
 *       None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None.
 *
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
        CSL_SrioHandle hSrio;
        CSL_SrioPortData   arg;

        arg.index = 2; 
        arg.data = 0; //doorbell info for type 10 packets
        ...
        CSL_SrioDoorbellXfrTypeSet(hSrio, &arg);
        ...
     @endverbatim
 * ===========================================================================
 */
static inline
void CSL_SrioDoorbellXfrTypeSet (
    CSL_SrioHandle   hSrio,
    CSL_SrioPortData *arg
)
{
    Uint32 val = hSrio->regs->LSU[arg->index].LSU_REG5;

    CSL_FINS (val, SRIO_LSU_REG5_DRBLL_INFO, arg->data);
    CSL_FINS (val, SRIO_LSU_REG5_PACKET_TYPE, 10); /* Doorbell packet type */ 
    
    hSrio->regs->LSU[arg->index].LSU_REG5 = val;
}



/** ============================================================================
 *   @n@b CSL_SrioLsuFlowMaskSet
 *
 *   @b Description
 *      Sets LSU flow masks.Port number is passed as input.
 *         Macros can be OR'ed to get the value for argument
 *
 *   @b Arguments
 *   @verbatim
         hSrio       Pointer to the object that holds reference to the 
                     instance of SRIO  

         arg         pointer to the structure that contains flowmask value
                    and index to LSU registers set
     @endverbatim
 *
 *   <b> Return Value </b>
 *       None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None.
 *
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
        CSL_SrioHandle   hSrio;
        CSL_SrioPortData arg;

        arg.index = 1;
        arg.data = 0x0;
        ...
        CSL_SrioLsuFlowMaskSet(hSrio, &arg);
        ...
     @endverbatim
 * ===========================================================================
 */
static inline
void CSL_SrioLsuFlowMaskSet (
    CSL_SrioHandle   hSrio,
    CSL_SrioPortData *arg
)
{
  hSrio->regs->LSU[arg->index].LSU_FLOW_MASKS = arg->data;
}



/** ============================================================================
 *   @n@b CSL_SrioPortCmdSet
 *
 *   @b Description
 *      Sets the command to be sent in the link-request control symbol
 *
 *   @b Arguments
 *   @verbatim
         hSrio       Pointer to the object that holds reference to the 
                     instance of SRIO  

         arg         pointer to structure that contains the command value and
                     index to ports
     @endverbatim
 *
 *   <b> Return Value </b>
 *       None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n   None
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
        CSL_SrioHandle  hSrio;
        CSL_SrioPortData arg;

        arg.index = 2; 
        arg.data = 0x0;
        ...
        CSL_SrioPortCmdSet(hSrio, &arg);
        ...
     @endverbatim
 * ===========================================================================
 */
static inline
void CSL_SrioPortCmdSet (
    CSL_SrioHandle   hSrio,
    CSL_SrioPortData *arg
)
{
    CSL_FINS(hSrio->regs->PORT[arg->index].SP_LM_REQ, 
                                        SRIO_SP_LM_REQ_COMMAND, arg->data);
}



/** ============================================================================
 *   @n@b CSL_SrioSpErrStatClear
 *
 *   @b Description
 *      Clear port errors status. Macros can be OR'ed to get the value to 
 *      pass the argument
 *
 *   @b Arguments
 *   @verbatim
         hSrio       Pointer to the object that holds reference to the 
                     instance of SRIO  

         arg         Pointer to a structure containing the error status 
                     clear value and index to the port
     @endverbatim
 *
 *   <b> Return Value </b>
 *       None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n   None
 *
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
        CSL_SrioHandle  hSrio;
        CSL_SrioPortData arg;

        arg.index = 0; 
        arg.data = CSL_SRIO_ERR_OUTPUT_PKT_DROP  |
                   CSL_SRIO_ERR_OUTPUT_FLD_ENC   |
                   CSL_SRIO_ERR_OUTPUT_DEGRD_ENC |
                   ...
                   CSL_SRIO_INPUT_ERROR_ENC;
        
        CSL_SrioSpErrStatClear(hSrio, &arg);
        ...
     @endverbatim
 * ===========================================================================
 */
static inline
void CSL_SrioSpErrStatClear (
    CSL_SrioHandle   hSrio,
    CSL_SrioPortData *arg
)
{
    hSrio->regs->PORT[arg->index].SP_ERR_STAT = arg->data;
}



/** ============================================================================
 *   @n@b CSL_SrioSpErrDetStatClear
 *
 *   @b Description
 *      Clear port error detect status. Macros can be OR'ed to get the value 
 *      to pass the argument
 *
 *   @b Arguments
 *   @verbatim
         hSrio       Pointer to the object that holds reference to the 
                     instance of SRIO  

         arg         Pointer to the structure that specify, port number and the 
                     argument
     @endverbatim
 *
 *   <b> Return Value </b>
 *       None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n   None
 *
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
        CSL_SrioHandle  hSrio;
        CSL_SrioPortData arg;

        arg.index = 3;
        arg.data = CSL_SRIO_ERR_IMP_SPECIFIC |
                   CSL_SRIO_CORRUPT_CNTL_SYM
                   ...
                   CSL_SRIO_RCVD_PKT_WITH_BAD_CRC;
                   
        CSL_SrioSpErrDetStatClear(hSrio, &arg);
        ...
     @endverbatim
 * ===========================================================================
 */
static inline
void CSL_SrioSpErrDetStatClear (
    CSL_SrioHandle    hSrio,
    CSL_SrioPortData *arg
)
{
    hSrio->regs->PORT_ERROR[arg->index].SP_ERR_DET = arg->data;
}


/** ============================================================================
 *   @n@b CSL_SrioCntlSymSet
 *
 *   @b Description
 *      Sets up the registers for sending a control symbol
 *
 *   @b Arguments
 *   @verbatim
         hSrio       Pointer to the object that holds reference to the 
                     instance of SRIO  

         arg         Pointer to the control symbol structure

     @endverbatim
 *
 *   <b> Return Value </b>
 *       None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n   None
 *
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
        CSL_SrioHandle  hSrio;
        CSL_SrioCntlSym arg;
        ...
        CSL_SrioCntlSymSet(hSrio, &arg);
        ...
     @endverbatim
 * ===========================================================================
 */
static inline
void CSL_SrioCntlSymSet (
    CSL_SrioHandle   hSrio,
    CSL_SrioCntlSym *arg
)
{
    hSrio->regs->PORT_OPTION[arg->portNum].SP_CS_TX = 
                            CSL_FMK(SRIO_SP_CS_TX_STYPE_0, arg->stype0) |
                            CSL_FMK(SRIO_SP_CS_TX_PAR_0, arg->par0) |
                            CSL_FMK(SRIO_SP_CS_TX_PAR_1, arg->par1) |
                            CSL_FMK(SRIO_SP_CS_TX_STYPE_1, arg->stype1) |
                            CSL_FMK(SRIO_SP_CS_TX_CMD, arg->cmd) |
                            CSL_FMK(SRIO_SP_CS_TX_CS_EMB, arg->emb);
}

/** ============================================================================
 *   @n@b CSL_SrioSpCtlIndepErrStatClear
 *
 *   @b Description
 *      Clears port control independent status register bits.
 *
 *   @b Arguments
 *   @verbatim
         hSrio       Pointer to the object that holds reference to the 
                     instance of SRIO  

         arg         Pointer to the structure that specify, port number and the 
                     argument
     @endverbatim
 *
 *   <b> Return Value </b>
 *       None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n   None
 *
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
        CSL_SrioHandle  hSrio;
        CSL_SrioPortData arg;
        
        arg.index = 1;
        arg.data = 0x10040; //clears corresponding error status
        ...
        CSL_SrioSpCtlIndepErrStatClear(hSrio, &arg);
        ...
     @endverbatim
 * ===========================================================================
 */
static inline
void CSL_SrioSpCtlIndepErrStatClear (
    CSL_SrioHandle   hSrio,
    CSL_SrioPortData *arg
)
{
    hSrio->regs->PORT_OPTION[arg->index].SP_CTL_INDEP = arg->data;
}

/** ============================================================================
 *   @n@b CSL_SrioPerEn
 *
 *   @b Description
 *      Enables/disables the peripheral.
 *
 *   @b Arguments
 *   @verbatim
         hSrio       Pointer to the object that holds reference to the 
                     instance of SRIO  

         arg         value to be configured
     @endverbatim
 *
 *   <b> Return Value </b>
 *       None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n   None
 *
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
        CSL_SrioHandle  hSrio;
        Bool			arg = 0x0;
        ...
        CSL_SrioPerEn(hSrio, &arg);
        ...
     @endverbatim
 * ===========================================================================
 */
static inline
void CSL_SrioPerEn (
	CSL_SrioHandle	hSrio, 
	Bool 			arg
)
{
	CSL_FINS(hSrio->regs->PCR, SRIO_PCR_PEREN, arg);
}
/** ============================================================================
 *   @n@b CSL_SrioPllCntlSet
 *
 *   @b Description
 *      Enables/disables the PLL.
 *
 *   @b Arguments
 *   @verbatim
         hSrio       Pointer to the object that holds reference to the 
                     instance of SRIO  

         arg         Value to enable/disable the 4 PLL
     @endverbatim
 *
 *   <b> Return Value </b>
 *       None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n   None
 *
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
        CSL_SrioHandle  hSrio;
        Uint8 arg = CSL_SRIO_PLL1_ENABLE |
                    CSL_SRIO_PLL2_ENABLE |
                    ...
                    CSL_SRIO_PLL4_ENABLE;
        ...
        CSL_SrioPllCntlSet(hSrio, arg);
        ...
     @endverbatim
 * ===========================================================================
 */
static inline
void CSL_SrioPllCntlSet (
	CSL_SrioHandle	hSrio, 
	Uint8			arg
)
{
	CSL_FINSR(hSrio->regs->PER_SET_CNTL, 3, 0, arg);
}
/** ============================================================================
 *   @n@b CSL_SrioLsuIntrClear
 *
 *   @b Description
 *      Clears LSU interrupt status bits corresponding to the bits set . 
 *
 *   @b Arguments
 *   @verbatim
         hSrio       Pointer to the object that holds reference to the 
                     instance of SRIO  

         arg         Mask cotaining the status bits to be cleared
     @endverbatim
 *
 *   <b> Return Value </b>
 *       None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n   None
 *
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
        CSL_SrioHandle  hSrio;
        Uint32	 arg;

        arg = CSL_SRIO_LSU_INTR0 |
              CSL_SRIO_LSU_INTR1 |
              CSL_SRIO_LSU_INTR2;
        ...
        CSL_SrioLsuIntrClear(hSrio, arg);
        ...
     @endverbatim
 * ===========================================================================
 */
static inline
void CSL_SrioLsuIntrClear (
	CSL_SrioHandle	hSrio, 
	Uint32 			arg
)
{
	hSrio->regs->LSU_ICCR = arg;
}
/** ============================================================================
 *   @n@b CSL_SrioErrRstIntrClear
 *
 *   @b Description
 *      Clears error interrupt status bits corresponding to the bits set . 
 *
 *   @b Arguments
 *   @verbatim
         hSrio       Pointer to the object that holds reference to the 
                     instance of SRIO  

         arg         Mask cotaining the status bits to be cleared
     @endverbatim
 *
 *   <b> Return Value </b>
 *       None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n   None
 *
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
        CSL_SrioHandle  hSrio;
        Uint32			 arg;

        arg = CSL_SRIO_ERR_DEV_RST_INTR |
              CSL_SRIO_ERR_PORT3_INTR |
              ...
              CSL_SRIO_ERR_LGCL_INTR;
        ...
        CSL_SrioErrRstIntrClear(hSrio, arg);
        ...
     @endverbatim
 * ===========================================================================
 */
static inline
void CSL_SrioErrRstIntrClear (
	CSL_SrioHandle	hSrio, 
	Uint32 			arg
)
{
	hSrio->regs->ERR_RST_EVNT_ICCR = arg;
}

/** ============================================================================
 *   @n@b CSL_SrioLgclTrnsErrStatClear
 *
 *   @b Description
 *      Clears logical ransport layer error status bits corresponding to 
 *		the bits set . 
 *
 *   @b Arguments
 *   @verbatim
         hSrio       Pointer to the object that holds reference to the 
                     instance of SRIO  

         arg         Mask cotaining the status bits to be cleared
     @endverbatim
 *
 *   <b> Return Value </b>
 *       None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n   None
 *
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
        CSL_SrioHandle  hSrio;
        Uint32			 arg;

        arg = CSL_SRIO_IO_ERR_RSPNS |
              CSL_SRIO_ILL_TRANS_DECODE |
              ...
              CSL_SRIO_UNSOLICITED_RSPNS;
        ...
        CSL_SrioLgclTrnsErrStatClear(hSrio, arg);
        ...
     @endverbatim
 * ===========================================================================
 */
static inline
void CSL_SrioLgclTrnsErrStatClear (
	CSL_SrioHandle	hSrio, 
	Uint32 			arg
)
{
	hSrio->regs->ERR_DET = arg;  
}


/** ============================================================================
 *   @n@b CSL_SrioSetIntdstRateCntl
 *
 *   @b Description
 *      Sets interrupt rate control counter
 *
 *   @b Arguments
 *   @verbatim
         hSrio       Pointer to the object that holds reference to the 
                     instance of SRIO  

         arg         Mask cotaining the status bits to be cleared
     @endverbatim
 *
 *   <b> Return Value </b>
 *       None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n   None
 *
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
        CSL_SrioHandle  hSrio;
        Uint32			 arg;

        arg = 0x100;
        ...
        CSL_SrioSetIntdstRateCntl(hSrio, arg);
        ...
     @endverbatim
 * ===========================================================================
 */
static inline 
void CSL_SrioSetIntdstRateCntl (
    CSL_SrioHandle hSrio, 
    Uint32         arg
)
{
    hSrio->regs->INTDST_RATE_CNTL[0] = arg;    
}

/** ============================================================================
 *   @n@b CSL_srioGetPid
 *
 *   @b Description
 *      Quries the peripheral identification details.
 *
 *   @b Arguments
 *   @verbatim
         hSrio       Pointer to the object that holds reference to the 
                     instance of SRIO  

         arg         Pointer to the structure to return the peripheral details
     @endverbatim
 *
 *   <b> Return Value </b>
 *       None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n   None
 *
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
        CSL_SrioHandle  hSrio;
        CSL_SrioPidNumber arg;
        ...
        CSL_srioGetPid(hSrio, &arg);
        ...
     @endverbatim
 * ===========================================================================
 */
static inline
void  CSL_srioGetPid (
    CSL_SrioHandle    hSrio,
    CSL_SrioPidNumber *response
)
{
     response->srioType = CSL_FEXT(hSrio->regs->PID, SRIO_PID_TYPE);
     response->srioClass = CSL_FEXT(hSrio->regs->PID, SRIO_PID_CLASS);
     response->srioRevision = CSL_FEXT(hSrio->regs->PID, SRIO_PID_REV);
}



/** ============================================================================
 *   @n@b CSL_srioGetBlkEnStat
 *
 *   @b Description
 *      Quries the enabled blocks of the peripheral.
 *
 *   @b Arguments
 *   @verbatim
         hSrio       Pointer to the object that holds reference to the 
                     instance of SRIO  

         arg         Pointer to the structure to return the status of different blocks
     @endverbatim
 *
 *   <b> Return Value </b>
 *       None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n   None
 *
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
        CSL_SrioHandle  hSrio;
        CSL_SrioBlkEn arg;
        ...
        CSL_srioGetBlkEnStat(hSrio, &arg);
        ...
     @endverbatim
 * ===========================================================================
 */
static inline
void CSL_srioGetBlkEnStat (
    CSL_SrioHandle hSrio, 
    CSL_SrioBlkEn *response
)
{
    response->block0 = (Bool)hSrio->regs->BLK_ENABLE[0].BLK_EN_STAT;
    response->block1 = (Bool)hSrio->regs->BLK_ENABLE[1].BLK_EN_STAT;
    response->block2 = (Bool)hSrio->regs->BLK_ENABLE[2].BLK_EN_STAT;
    response->block3 = (Bool)hSrio->regs->BLK_ENABLE[3].BLK_EN_STAT;
    response->block4 = (Bool)hSrio->regs->BLK_ENABLE[4].BLK_EN_STAT;
    response->block5 = (Bool)hSrio->regs->BLK_ENABLE[5].BLK_EN_STAT;
    response->block6 = (Bool)hSrio->regs->BLK_ENABLE[6].BLK_EN_STAT;
    response->block7 = (Bool)hSrio->regs->BLK_ENABLE[7].BLK_EN_STAT;
    response->block8 = (Bool)hSrio->regs->BLK_ENABLE[8].BLK_EN_STAT;
}



/** ============================================================================
 *   @n@b CSL_srioGetDoorbellIntrStat
 *
 *   @b Description
 *      Quries the doorbell interrupts status.
 *
 *   @b Arguments
 *   @verbatim
         hSrio       Pointer to the object that holds reference to the 
                     instance of SRIO  

         arg         Pointer to the structure that specify, port number and the 
                     argument
     @endverbatim
 *
 *   <b> Return Value </b>
 *       None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n   None
 *
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
        CSL_SrioHandle  hSrio;
        CSL_SrioPortData arg;
        ...
        CSL_srioGetDoorbellIntrStat(hSrio, &arg);
        ...
     @endverbatim
 * ===========================================================================
 */
static inline
void CSL_srioGetDoorbellIntrStat (
    CSL_SrioHandle   hSrio,
    CSL_SrioPortData *response
)
{
    response->data = hSrio->regs->DOORBELL_INTR[response->index].DOORBELL_ICSR;
}



/** ============================================================================
 *   @n@b CSL_srioLsuCompCodeStat
 *
 *   @b Description
 *      Quries the completion code status of LSU.
 *
 *   @b Arguments
 *   @verbatim
         hSrio       Pointer to the object that holds reference to the 
                     instance of SRIO  

         arg         Pointer to the structure that specify, port number and the 
                     LSU completion code
     @endverbatim
 *
 *   <b> Return Value </b>
 *       None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n   None
 *
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
        CSL_SrioHandle  hSrio;
        CSL_SrioLsuCompStat arg;
        ...
        CSL_srioLsuCompCodeStat(hSrio, &arg);
        ...
     @endverbatim
 * ===========================================================================
 */
static inline
void CSL_srioLsuCompCodeStat(
    CSL_SrioHandle      hSrio,
    CSL_SrioLsuCompStat *response
)
{
    response->lsuCompCode = (CSL_SrioCompCode)CSL_FEXT(
                                hSrio->regs->LSU[response->portNum].LSU_REG6, 
                                SRIO_LSU_REG6_COMPLETION_CODE);
}

/** ============================================================================
 *   @n@b CSL_srioLsuBsyStat
 *
 *   @b Description
 *      Quries the LSU busy status.
 *
 *   @b Arguments
 *   @verbatim
         hSrio       Pointer to the object that holds reference to the 
                     instance of SRIO  

         arg         Pointer to the structure that specify, port number and the 
                     argument
     @endverbatim
 *
 *   <b> Return Value </b>
 *       None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n   None
 *
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
        CSL_SrioHandle  hSrio;
        CSL_SrioPortData arg;
        ...
        CSL_srioLsuBsyStat(hSrio, &arg);
        ...
     @endverbatim
 * ===========================================================================
 */
static inline
void CSL_srioLsuBsyStat (
    CSL_SrioHandle   hSrio,
    CSL_SrioPortData *response
)
{
    response->data = CSL_FEXT(hSrio->regs->LSU[response->index].LSU_REG6, 
                                                        SRIO_LSU_REG6_BSY);
}



/** ============================================================================
 *   @n@b CSL_srioGetDevIdInfo
 *
 *   @b Description
 *      Quries the device identity.
 *
 *   @b Arguments
 *   @verbatim
         hSrio       Pointer to the object that holds reference to the 
                     instance of SRIO  

         arg         Pointer to the structure that specify, device info 

     @endverbatim
 *
 *   <b> Return Value </b>
 *       None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n   None
 *
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
        CSL_SrioHandle  hSrio;
        CSL_SrioDevInfo arg;
        ...
        CSL_srioGetDevIdInfo(hSrio, &arg);
        ...
     @endverbatim
 * ===========================================================================
 */
static inline
void CSL_srioGetDevIdInfo (
    CSL_SrioHandle   hSrio,
    CSL_SrioDevInfo *response
)
{
    response->devId = CSL_FEXT(hSrio->regs->DEV_ID, SRIO_DEV_ID_DEVICEIDENTITY);
    response->devVendorId = CSL_FEXT(hSrio->regs->DEV_ID, 
                                        SRIO_DEV_ID_DEVICE_VENDORIDENTITY);
    response->devRevision = CSL_FEXT(hSrio->regs->DEV_INFO, 
                                        SRIO_DEV_INFO_DEVICEREV);
}



/** ============================================================================
 *   @n@b CSL_srioGetAssyIdInfo
 *
 *   @b Description
 *      Quries the device assembly identity.
 *
 *   @b Arguments
 *   @verbatim
         hSrio       Pointer to the object that holds reference to the 
                     instance of SRIO  

         arg         Pointer to the structure that specify, assembly info of
                     the peripheral

     @endverbatim
 *
 *   <b> Return Value </b>
 *       None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n   None
 *
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
        CSL_SrioHandle  hSrio;
        CSL_SrioAssyInfo arg;
        ...
        CSL_srioGetAssyIdInfo(hSrio, &arg);
        ...
     @endverbatim
 * ===========================================================================
 */
static inline
void CSL_srioGetAssyIdInfo (
    CSL_SrioHandle    hSrio,
    CSL_SrioAssyInfo *response
)
{
    response->assyId = CSL_FEXT(hSrio->regs->ASBLY_ID, 
                                      SRIO_ASBLY_ID_ASSY_IDENTITY);
    response->assyVendorId = CSL_FEXT(hSrio->regs->ASBLY_ID, 
                                      SRIO_ASBLY_ID_ASSY_VENDORIDENTITY);
    response->assyRevision = CSL_FEXT(hSrio->regs->ASBLY_INFO, 
                                      SRIO_ASBLY_INFO_ASSYREV);
}
/** ============================================================================
 *   @n@b CSL_srioGetLclCfgBar
 *
 *   @b Description
 *      Quries long address of programmed for the LSU
 *
 *   @b Arguments
 *   @verbatim
         hSrio       Pointer to the object that holds reference to the 
                     instance of SRIO  

         arg         Pointer to the structure that specify the long address

     @endverbatim
 *
 *   <b> Return Value </b>
 *       None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n   None
 *
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
        CSL_SrioHandle  hSrio;
        CSL_SrioLongAddress arg;
        ...
        CSL_srioGetLclCfgBar(hSrio, &arg);
        ...
     @endverbatim
 * ===========================================================================
 */
static inline
void CSL_srioGetLclCfgBar(
    CSL_SrioHandle      hSrio,
    CSL_SrioLongAddress *response
)
{
    response->addressHi = hSrio->regs->LCL_CFG_HBAR;
    response->addressLo = hSrio->regs->LCL_CFG_BAR;
}

/** ============================================================================
 *   @n@b CSL_srioGetSpLmRespStat
 *
 *   @b Description
 *      Quries the link maintainance response status.
 *
 *   @b Arguments
 *   @verbatim
         hSrio       Pointer to the object that holds reference to the 
                     instance of SRIO  

         arg         Pointer to the structure that specify, port number and the 
                     argument
     @endverbatim
 *
 *   <b> Return Value </b>
 *       None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n   None
 *
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
        CSL_SrioHandle  hSrio;
        CSL_SrioPortData arg;
        ...
        CSL_srioGetSpLmRespStat(hSrio, &arg);
        ...
     @endverbatim
 * ===========================================================================
 */

static inline
void CSL_srioGetSpLmRespStat (
    CSL_SrioHandle   hSrio,
    CSL_SrioPortData *response
)
{
    response->data = hSrio->regs->PORT[response->index].SP_LM_RESP;
}
/** ============================================================================
 *   @n@b CSL_srioGetSpAckIdStat
 *
 *   @b Description
 *      Quries port ACK ID status.
 *
 *   @b Arguments
 *   @verbatim
         hSrio       Pointer to the object that holds reference to the 
                     instance of SRIO  

         arg         Pointer to the structure that specify, port number and the 
                     argument
     @endverbatim
 *
 *   <b> Return Value </b>
 *       None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n   None
 *
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
        CSL_SrioHandle  hSrio;
        CSL_SrioPortData arg;
        ...
        CSL_srioGetSpAckIdStat(hSrio, &arg);
        ...
     @endverbatim
 * ===========================================================================
 */
static inline
void CSL_srioGetSpAckIdStat (
    CSL_SrioHandle   hSrio,
    CSL_SrioPortData *response
)
{
    response->data = hSrio->regs->PORT[response->index].SP_ACKID_STAT;
}

/** ============================================================================
 *   @n@b CSL_srioGetSpErrStat
 *
 *   @b Description
 *      Quries the port error status.
 *
 *   @b Arguments
 *   @verbatim
         hSrio       Pointer to the object that holds reference to the 
                     instance of SRIO  

         arg         Pointer to the structure that specify, port number and the 
                     argument
     @endverbatim
 *
 *   <b> Return Value </b>
 *       None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n   None
 *
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
        CSL_SrioHandle  hSrio;
        CSL_SrioPortData arg;
        ...
        CSL_srioGetSpErrStat(hSrio, &arg);
        ...
     @endverbatim
 * ===========================================================================
 */
static inline
void CSL_srioGetSpErrStat (
    CSL_SrioHandle   hSrio,
    CSL_SrioPortData *response
)
{
    response->data = hSrio->regs->PORT[response->index].SP_ERR_STAT;
}

/** ============================================================================
 *   @n@b CSL_srioGetSpCtlStat
 *
 *   @b Description
 *      Quries the port control status.
 *
 *   @b Arguments
 *   @verbatim
         hSrio       Pointer to the object that holds reference to the 
                     instance of SRIO  

         arg         Pointer to the structure that specify, port number and the 
                     argument
     @endverbatim
 *
 *   <b> Return Value </b>
 *       None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n   None
 *
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
        CSL_SrioHandle  hSrio;
        CSL_SrioPortData arg;
        ...
        CSL_srioGetSpCtlStat(hSrio, &arg);
        ...
     @endverbatim
 * ===========================================================================
 */

static inline
void CSL_srioGetSpCtlStat (
    CSL_SrioHandle   hSrio,
    CSL_SrioPortData *response
)
{
    response->data = hSrio->regs->PORT[response->index].SP_CTL;
}

/** ============================================================================
 *   @n@b CSL_srioGetLgclTransErrCapt
 *
 *   @b Description
 *      Quries captured error information of 
 *        logical/transport layer status
 *
 *   @b Arguments
 *   @verbatim
         hSrio       Pointer to the object that holds reference to the 
                     instance of SRIO  

         arg         Pointer to the structure that specify, captured error
                     information of logical/transport layer
     @endverbatim
 *
 *   <b> Return Value </b>
 *       None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n   None
 *
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
        CSL_SrioHandle  hSrio;
        CSL_SrioLogTrErrInfo arg;
        ...
        CSL_srioGetLgclTransErrCapt(hSrio, &arg);
        ...
     @endverbatim
 * ===========================================================================
 */
static inline
void CSL_srioGetLgclTransErrCapt (
    CSL_SrioHandle       hSrio, 
    CSL_SrioLogTrErrInfo *response
)
{
    response->errAddrHi = hSrio->regs->H_ADDR_CAPT;
    response->errAddrLo = CSL_FEXT(hSrio->regs->ADDR_CAPT, 
                                        SRIO_ADDR_CAPT_ADDRESS_31_3); 
    response->xambs = CSL_FEXT(hSrio->regs->ADDR_CAPT, 
                                        SRIO_ADDR_CAPT_XAMSBS);
    response->destId = CSL_FEXTR(hSrio->regs->ID_CAPT, 31, 16);
    response->srcId = CSL_FEXTR(hSrio->regs->ID_CAPT, 15, 0);
    response->ftype = CSL_FEXT(hSrio->regs->CTRL_CAPT, SRIO_CTRL_CAPT_FTYPE); 
    response->tType = CSL_FEXT(hSrio->regs->CTRL_CAPT, SRIO_CTRL_CAPT_TTYPE);
    //response->msgInfo = CSL_FEXT(hSrio->regs->CTRL_CAPT, SRIO_CTRL_CAPT_MSGINFO);
    response->impSpecific = CSL_FEXT(hSrio->regs->CTRL_CAPT, 
                                                SRIO_CTRL_CAPT_IMP_SPECIFIC);
}

/** ============================================================================
 *   @n@b CSL_srioGetSpErrDetStat
 *
 *   @b Description
 *      Quries the port error detect status
 *
 *   @b Arguments
 *   @verbatim
         hSrio       Pointer to the object that holds reference to the 
                     instance of SRIO  

         arg         Pointer to the structure that specify, port number and the 
                     argument
     @endverbatim
 *
 *   <b> Return Value </b>
 *       None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n   None
 *
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
        CSL_SrioHandle  hSrio;
        CSL_SrioPortData arg;
        ...
        CSL_srioGetSpErrDetStat(hSrio, &arg);
        ...
     @endverbatim
 * ===========================================================================
 */
static inline
void CSL_srioGetSpErrDetStat (
    CSL_SrioHandle   hSrio, 
    CSL_SrioPortData *response
)
{
    response->data = hSrio->regs->PORT_ERROR[response->index].SP_ERR_DET;
}

/** ============================================================================
 *   @n@b CSL_srioGetPortErrCapt
 *
 *   @b Description
 *      Quries the port error capture status
 *
 *   @b Arguments
 *   @verbatim
         hSrio       Pointer to the object that holds reference to the 
                     instance of SRIO  

         arg         Pointer to the structure that specify, port error capture
                     details
     @endverbatim
 *
 *   <b> Return Value </b>
 *       None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n   None
 *
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
        CSL_SrioHandle  hSrio;
        CSL_SrioPortData arg;
        ...
        CSL_srioGetPortErrCapt(hSrio, &arg);
        ...
     @endverbatim
 * ===========================================================================
 */
static inline
void CSL_srioGetPortErrCapt (
    CSL_SrioHandle      hSrio, 
    CSL_SrioPortErrCapt *response
)
{
    response->portErrCaptType = (CSL_SrioPortCaptType)CSL_FEXT(
            hSrio->regs->PORT_ERROR[response->portNum].SP_ERR_ATTR_CAPT_DBG0, \
                                          SRIO_SP_ERR_ATTR_CAPT_DBG0_INFO_TYPE);  
    response->errorType = CSL_FEXT(
        hSrio->regs->PORT_ERROR[response->portNum].SP_ERR_ATTR_CAPT_DBG0, \
                                           SRIO_SP_ERR_ATTR_CAPT_DBG0_ERROR_TYPE);
    response->impSpecData = CSL_FEXT(
            hSrio->regs->PORT_ERROR[response->portNum].SP_ERR_ATTR_CAPT_DBG0, \
                                        SRIO_SP_ERR_ATTR_CAPT_DBG0_IMP_SPECIFIC);
    response->capture0 = 
        hSrio->regs->PORT_ERROR[response->portNum].SP_ERR_CAPT_DBG[0];
    response->capture1 = 
        hSrio->regs->PORT_ERROR[response->portNum].SP_ERR_CAPT_DBG[1];
    response->capture2 = 
        hSrio->regs->PORT_ERROR[response->portNum].SP_ERR_CAPT_DBG[2];
    response->capture3 = 
        hSrio->regs->PORT_ERROR[response->portNum].SP_ERR_CAPT_DBG[3];
        
}

/** ============================================================================
 *   @n@b CSL_srioGetSpCtlIndepStat
 *
 *   @b Description
 *      Quries the port control independent status.
 *
 *   @b Arguments
 *   @verbatim
         hSrio       Pointer to the object that holds reference to the 
                     instance of SRIO  

         arg         Pointer to the structure that specify, port number and the 
                     argument
     @endverbatim
 *
 *   <b> Return Value </b>
 *       None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n   None
 *
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
        CSL_SrioHandle  hSrio;
        CSL_SrioPortData arg;
        ...
        CSL_srioGetSpCtlIndepStat(hSrio, &arg);
        ...
     @endverbatim
 * ===========================================================================
 */

static inline
void CSL_srioGetSpCtlIndepStat (
    CSL_SrioHandle   hSrio, 
    CSL_SrioPortData *response
)
{
    response->data = hSrio->regs->PORT_OPTION[response->index].SP_CTL_INDEP;
}

/** ============================================================================
 *   @n@b CSL_srioGetPwCapt
 *
 *   @b Description
 *      Quries the port error capture status
 *
 *   @b Arguments
 *   @verbatim
         hSrio       Pointer to the object that holds reference to the 
                     instance of SRIO  

         arg         Pointer to the structure that specify, port write capture
                     details
     @endverbatim
 *
 *   <b> Return Value </b>
 *       None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n   None
 *
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
        CSL_SrioHandle  hSrio;
        CSL_SrioPortWriteCapt arg;
        ...
        CSL_srioGetPwCapt(hSrio, &arg);
        ...
     @endverbatim
 * ===========================================================================
 */

static inline
void CSL_srioGetPwCapt (
    CSL_SrioHandle        hSrio, 
    CSL_SrioPortWriteCapt *response
)
{
    response->capture0 = hSrio->regs->SP_IP_PW_IN_CAPT[0]; 
    response->capture1 = hSrio->regs->SP_IP_PW_IN_CAPT[1];
    response->capture2 = hSrio->regs->SP_IP_PW_IN_CAPT[2];
    response->capture3 = hSrio->regs->SP_IP_PW_IN_CAPT[3];
}


/** ============================================================================
 *   @n@b CSL_srioErrRateCounterRead
 *
 *   @b Description
 *      Quries the port error rate counter value
 *
 *   @b Arguments
 *   @verbatim
         hSrio       Pointer to the object that holds reference to the 
                     instance of SRIO  

         arg         Pointer to the structure that specify, output parameter 
		             for error rate counter and index to the ports
     @endverbatim
 *
 *   <b> Return Value </b>
 *       None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n   None
 *
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
        CSL_SrioHandle  hSrio;
        CSL_SrioPortData arg;
        ...
        CSL_srioErrRateCounterRead(hSrio, &arg);
        ...
     @endverbatim
 * ===========================================================================
 */
static inline
void CSL_srioErrRateCounterRead (
    CSL_SrioHandle  hSrio,
	CSL_SrioPortData *response
)
{
    response->data = 
    CSL_FEXT(hSrio->regs->PORT_ERROR[response->index].SP_ERR_RATE,\
                              SRIO_SP_ERR_RATE_ERROR_RATE_COUNTER); 
}


/** ============================================================================
 *   @n@b CSL_srioErrRatePeakRead
 *
 *   @b Description
 *      Quries the port Error Rate Peak value
 *
 *   @b Arguments
 *   @verbatim
         hSrio       Pointer to the object that holds reference to the 
                     instance of SRIO  

         arg         Pointer to the structure that specify, output parameter for
		              Error Rate Peak value and index
     @endverbatim
 *
 *   <b> Return Value </b>
 *       None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n   None
 *
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
        CSL_SrioHandle  hSrio;
        CSL_SrioPortData arg;
        ...
        CSL_srioErrRatePeakRead(hSrio, &arg);
        ...
     @endverbatim
 * ===========================================================================
 */
static inline
void CSL_srioErrRatePeakRead (
    CSL_SrioHandle   hSrio,
	CSL_SrioPortData *response
)
{
    response->data = 
    CSL_FEXT(hSrio->regs->PORT_ERROR[response->index].SP_ERR_RATE, \
                         SRIO_SP_ERR_RATE_PEAK_ERROR_RATE); 
}

/** ============================================================================
 *   @n@b CSL_srioGetGblEnStat
 *
 *   @b Description
 *      Queries the Global enable status
 *
 *   @b Arguments
 *   @verbatim
         hSrio       Pointer to the object that holds reference to the 
                     instance of SRIO  

         response    output parameter to return the status
     @endverbatim
 *
 *   <b> Return Value </b>
 *       None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n   None
 *
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
        CSL_SrioHandle  hSrio;
        Uint32			resp;
        ...
        CSL_srioGetGblEnStat(hSrio, &resp);
        ...
     @endverbatim
 * ===========================================================================
 */
static inline
void CSL_srioGetGblEnStat (
	CSL_SrioHandle 	hSrio, 
	Uint32*			response
)
{
	*(Uint32 *)response = hSrio->regs->GBL_EN_STAT;
}

/** ============================================================================
 *   @n@b CSL_srioGetLsuIntrStat
 *
 *   @b Description
 *      Queries the LSU interrupt status
 *
 *   @b Arguments
 *   @verbatim
         hSrio       Pointer to the object that holds reference to the 
                     instance of SRIO  

         response    output parameter to return the status
     @endverbatim
 *
 *   <b> Return Value </b>
 *       None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n   None
 *
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
        CSL_SrioHandle  hSrio;
        Uint32			resp;
        ...
        CSL_srioGetLsuIntrStat(hSrio, &resp);
        ...
     @endverbatim
 * ===========================================================================
 */
static inline
void CSL_srioGetLsuIntrStat (
	CSL_SrioHandle 	hSrio, 
	Uint32*			response
)
{
	*(Uint32 *)response = hSrio->regs->LSU_ICSR; 
}

/** ============================================================================
 *   @n@b CSL_srioGetErrRstIntrStat
 *
 *   @b Description
 *      Queries the error and reset interrupt status
 *
 *   @b Arguments
 *   @verbatim
         hSrio       Pointer to the object that holds reference to the 
                     instance of SRIO  

         response    output parameter to return the status
     @endverbatim
 *
 *   <b> Return Value </b>
 *       None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n   None
 *
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
        CSL_SrioHandle  hSrio;
        Uint32			resp;
        ...
        CSL_srioGetErrRstIntrStat (hSrio, &resp);
        ...
     @endverbatim
 * ===========================================================================
 */
static inline
void CSL_srioGetErrRstIntrStat (
	CSL_SrioHandle 		hSrio, 
	Uint32*			response
)
{	 
	*(Uint32 *)response = hSrio->regs->ERR_RST_EVNT_ICSR;
}

/** ============================================================================
 *   @n@b CSL_srioGetLsuIntrDecodeStat
 *
 *   @b Description
 *      Queries the LSU interrupt decode status
 *
 *   @b Arguments
 *   @verbatim
         hSrio       Pointer to the object that holds reference to the 
                     instance of SRIO  

         response    output parameter to return the status
     @endverbatim
 *
 *   <b> Return Value </b>
 *       None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n   None
 *
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
        CSL_SrioHandle  hSrio;
        Uint32			resp;
        ...
        CSL_srioGetLsuIntrDecodeStat (hSrio, &resp);
        ...
     @endverbatim
 * ===========================================================================
 */
static inline
void CSL_srioGetLsuIntrDecodeStat (
	CSL_SrioHandle		hSrio, 
	Uint32*				response
)
{
	*(Uint32 *)response = CSL_FEXT(hSrio->regs->INTDST_DECODE[0], 
										SRIO_INTDST_DECODE_ISDR31);
}
/** ============================================================================
 *   @n@b CSL_srioGetErrIntrDecodeStat
 *
 *   @b Description
 *      Queries the error interrupt decode status
 *
 *   @b Arguments
 *   @verbatim
         hSrio       Pointer to the object that holds reference to the 
                     instance of SRIO  

         response    output parameter to return the status
     @endverbatim
 *
 *   <b> Return Value </b>
 *       None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n   None
 *
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
        CSL_SrioHandle  hSrio;
        Uint32			resp;
        ...
        CSL_srioGetErrIntrDecodeStat(hSrio, &resp);
        ...
     @endverbatim
 * ===========================================================================
 */
static inline
void CSL_srioGetErrIntrDecodeStat (
	CSL_SrioHandle		hSrio, 
	Uint32*				response
)
{
	*(Uint32 *)response = CSL_FEXT(hSrio->regs->INTDST_DECODE[0], 
										SRIO_INTDST_DECODE_ISDR30);
}

/** ============================================================================
 *   @n@b CSL_srioGetPeFeature
 *
 *   @b Description
 *      Queries the PE features register
 *
 *   @b Arguments
 *   @verbatim
         hSrio       Pointer to the object that holds reference to the 
                     instance of SRIO  

         response    output parameter to return the value
     @endverbatim
 *
 *   <b> Return Value </b>
 *       None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n   None
 *
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
        CSL_SrioHandle  hSrio;
        Uint32			resp;
        ...
        CSL_srioGetPeFeature(hSrio, &resp);
        ...
     @endverbatim
 * ===========================================================================
 */
static inline
void CSL_srioGetPeFeature (
	CSL_SrioHandle 	hSrio, 
	Uint32*			response
) 
{
	*(Uint32 *)response = hSrio->regs->PE_FEAT;
}

/** ============================================================================
 *   @n@b CSL_srioGetSrcOpernSuppStat
 *
 *   @b Description
 *      Queries the sorce operations supported register
 *
 *   @b Arguments
 *   @verbatim
         hSrio       Pointer to the object that holds reference to the 
                     instance of SRIO  

         response    output parameter to return the value
     @endverbatim
 *
 *   <b> Return Value </b>
 *       None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n   None
 *
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
        CSL_SrioHandle  hSrio;
        Uint32			resp;
        ...
        CSL_srioGetSrcOpernSuppStat(hSrio, &resp);
        ...
     @endverbatim
 * ===========================================================================
 */
static inline
void CSL_srioGetSrcOpernSuppStat (
	CSL_SrioHandle		hSrio, 
	Uint32*				response
)
{
	*(Uint32 *)response = hSrio->regs->SRC_OP;
}

/** ============================================================================
 *   @n@b CSL_srioGetDstOpernSuppStat
 *
 *   @b Description
 *      Queries the destination operation support register
 *
 *   @b Arguments
 *   @verbatim
         hSrio       Pointer to the object that holds reference to the 
                     instance of SRIO  

         response    output parameter to return the value
     @endverbatim
 *
 *   <b> Return Value </b>
 *       None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n   None
 *
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
        CSL_SrioHandle  hSrio;
        Uint32			resp;
        ...
        CSL_srioGetDstOpernSuppStat(hSrio, &resp);
        ...
     @endverbatim
 * ===========================================================================
 */
static inline
void CSL_srioGetDstOpernSuppStat (
	CSL_SrioHandle 	hSrio, 
	Uint32*			response
)
{
	*(Uint32 *)response = hSrio->regs->DEST_OP;
}

/** ============================================================================
 *   @n@b CSL_SrioGetLgclTrnsErrStat
 *
 *   @b Description
 *      Queries the logical transport layer error status
 *
 *   @b Arguments
 *   @verbatim
         hSrio       Pointer to the object that holds reference to the 
                     instance of SRIO  

         response    output parameter to return the status
     @endverbatim
 *
 *   <b> Return Value </b>
 *       None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n   None
 *
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
        CSL_SrioHandle  hSrio;
        Uint32			resp;
        ...
        CSL_SrioGetLgclTrnsErrStat(hSrio, &resp);
        ...
     @endverbatim
 * ===========================================================================
 */
static inline
void CSL_SrioGetLgclTrnsErrStat (
	CSL_SrioHandle	hSrio, 
	Uint32* 		response
) 
{
	*(Uint32 *)response = hSrio->regs->ERR_DET;
}

#ifdef __cplusplus
extern "C" {
#endif

#endif /* CSL_SRIOAUX_H_ */

