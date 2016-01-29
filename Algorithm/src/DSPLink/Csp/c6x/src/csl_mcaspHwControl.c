/** ===========================================================================
 *   @file  csl_mcaspHwControl.c
 *
 *   @path  $(CSLPATH)\mcasp\src
 *
 *   @desc  Multi channel audio serial port CSL implementation
 *
 */  
    
/*  ===========================================================================
 *  Copyright (c) Texas Instruments Inc 2002, 2003, 2004, 2005
 *
 *  Use of this software is controlled by the terms and conditions found in the
 *  license agreement under which this software has been supplied.
 *  ===========================================================================
 */ 
    
/*  @(#) PSP/CSL 3.00.01.00[5905] (2004-10-12)  */ 
    
/* ============================================================================
 *  Revision History
 *  ===============
 *  12-Oct-2004 sr created
 * ============================================================================
 */ 
#include <csl_mcasp.h>
#include <csl_mcaspAux.h>
    
/** ===========================================================================
 *   @n@b CSL_mcaspHwControl
 *
 *   @b Description
 *   @n This function performs various control operations on McASP instance,
 *      based on the command passed.
 *
 *   @b Arguments
 *   @verbatim
            hMcasp      Handle to the McASP instance
 
            cmd         Operation to be performed on the McASP
 
            arg         Argument specific to the command 
 
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK            - Command execution successful.
 *   @li                    CSL_ESYS_BADHANDLE - Invalid handle
 *   @li                    CSL_ESYS_INVCMD    - Invalid command
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  Registers of McASP instance are configured according to the command
 *       and the command arguments. The command determines which registers are
 *       modified.
 *
 *   @b Modifies
 *   @n Registers determined by the command
 *
 *   @b Example
 *   @verbatim
        CSL_Status      status;
        CSL_McaspHandle hMcasp;

        status  = CSL_mcaspHwControl (hMcasp,  
                  CSL_MCASP_CMD_CLK_RESET_XMT, NULL);
     @endverbatim
 * ============================================================================
 */ 
#pragma CODE_SECTION (CSL_mcaspHwControl, ".text:csl_section:mcasp");
CSL_Status CSL_mcaspHwControl (
            CSL_McaspHandle        hMcasp,
            CSL_McaspHwControlCmd  cmd, 
            void                   *arg 
) 
{
    CSL_Status status = CSL_SOK;

    if (hMcasp == NULL){
            return CSL_ESYS_BADHANDLE;
    }            
    
    switch (cmd) {
                /* Set XGBLCTL */ 
        case CSL_MCASP_CMD_SET_XMT:
            CSL_mcaspSetXmtGbl(hMcasp, *(Uint32 *) arg);
            break;
            
                /* Set RGBLCTL */ 
        case CSL_MCASP_CMD_SET_RCV:
            CSL_mcaspSetRcvGbl(hMcasp, *(Uint32 *) arg);
            break;
            
                /* Reset XFRST in XGBLCTL */ 
        case CSL_MCASP_CMD_RESET_FSYNC_XMT:
            CSL_mcaspResetXmtFSRst(hMcasp);
            break;
            
                /* Reset RFRST in RGBLCTL */ 
        case CSL_MCASP_CMD_RESET_FSYNC_RCV:
            CSL_mcaspResetRcvFSRst(hMcasp);
            break;
            
                /* Reset all registers */ 
        case CSL_MCASP_CMD_REG_RESET:
            CSL_mcaspRegReset(hMcasp);
            break;
            
                /* Mute enable */ 
        case CSL_MCASP_CMD_AMUTE_ON:
            CSL_mcaspConfigAudioMute(hMcasp, *(Uint32 *) arg);
            break;
            
                /* Set DLB mode */ 
        case CSL_MCASP_CMD_DLB_ON:
            CSL_mcaspConfigLoopBack(hMcasp, *(Bool *) arg);
            break;
            
                /* Configure receive slots */ 
        case CSL_MCASP_CMD_CONFIG_RTDM_SLOT:
            CSL_mcaspConfigRcvSlot(hMcasp, *(Uint32 *) arg);
            break;
            
                /* Configure transmit slots */ 
        case CSL_MCASP_CMD_CONFIG_XTDM_SLOT:
            CSL_mcaspConfigXmtSlot(hMcasp, *(Uint32 *) arg);
            break;
            
                /* Set RINTCTL with value passed */ 
        case CSL_MCASP_CMD_CONFIG_INTERRUPT_RCV:
            CSL_mcaspConfigRcvInt(hMcasp, *(Uint32 *) arg);
            break;
            
                /* Set XINTCTL with value passed */ 
        case CSL_MCASP_CMD_CONFIG_INTERRUPT_XMT:
            CSL_mcaspConfigXmtInt(hMcasp, *(Uint32 *) arg);
            break;
            
                /* Reset clk circuitry for receive */ 
        case CSL_MCASP_CMD_CLK_RESET_RCV:
            CSL_mcaspResetRcvClk(hMcasp);
            break;
            
                /* Reset clk circuitry for transmit */ 
        case CSL_MCASP_CMD_CLK_RESET_XMT:
            CSL_mcaspResetXmtClk(hMcasp);
            break;
            
       /* Set rcv clk registers with value (CSL_McaspHwSetupDataClk*)passed */
        case CSL_MCASP_CMD_CLK_SET_RCV:
            CSL_mcaspSetRcvClk(hMcasp, (CSL_McaspHwSetupDataClk *) arg);
            break;
            
       /* Set xmt clk registers with value (CSL_McaspHwSetupDataClk*)passed */ 
        case CSL_MCASP_CMD_CLK_SET_XMT:
            CSL_mcaspSetXmtClk(hMcasp, (CSL_McaspHwSetupDataClk *) arg);
            break;
            
          /*
           *  Configure the format, frame sync, and other parameters related
           *  to the xmt section
           */ 
        case CSL_MCASP_CMD_CONFIG_XMT_SECTION:
            {
                CSL_mcaspConfigXmtSection(hMcasp, (CSL_McaspHwSetupData *) arg);
                break;
            }
            
         /*
          *  Configure the format, frame sync, and other parameters related
          *  to the rcv section
          */ 
        case CSL_MCASP_CMD_CONFIG_RCV_SECTION:
            CSL_mcaspConfigRcvSection(hMcasp, (CSL_McaspHwSetupData *) arg);
            break;
            
                /* Sets a particular serializer to act as transmitter */ 
        case CSL_MCASP_CMD_SET_SER_XMT:
            CSL_mcaspSetSerXmt(hMcasp, *(CSL_McaspSerializerNum *) arg);
            break;
            
                /* Sets a particular serializer to act as receiver */ 
        case CSL_MCASP_CMD_SET_SER_RCV:
            CSL_mcaspSetSerRcv(hMcasp, *(CSL_McaspSerializerNum *) arg);
            break;
            
                /* Sets a particular serializer as inactivated */ 
        case CSL_MCASP_CMD_SET_SER_INA:
            CSL_mcaspSetSerIna(hMcasp, *(CSL_McaspSerializerNum *) arg);
            break;
            
                /* Writes to the channel status RAM (DITCSRA/B0-5) */ 
        case CSL_MCASP_CMD_WRITE_CHAN_STAT_RAM:
            CSL_mcaspWriteChanStatRam(hMcasp, (CSL_McaspChStatusRam *) arg);
            break;
            
                /* Writes to the user data RAM (DITUDRA/B0-5) */ 
        case CSL_MCASP_CMD_WRITE_USER_DATA_RAM:
            CSL_mcaspWriteUserDataRam(hMcasp, (CSL_McaspUserDataRam *) arg);
            break;
            
                /* Resets the bits related to transmit in XGBLCTL */ 
        case CSL_MCASP_CMD_RESET_XMT:
            CSL_mcaspResetXmt(hMcasp);
            break;
            
                /* Resets the bits related to receive in RGBLCTL */ 
        case CSL_MCASP_CMD_RESET_RCV:
            CSL_mcaspResetRcv(hMcasp);
            break;
            
                /* Resets the XFRST and XSMRST bits in XGBLCTL */ 
        case CSL_MCASP_CMD_RESET_SM_FS_XMT:
            CSL_mcaspResetSmFsXmt(hMcasp);
            break;
            
                /* Resets the RFRST and RSMRST bits in RGBLCTL */ 
        case CSL_MCASP_CMD_RESET_SM_FS_RCV:
            CSL_mcaspResetSmFsRcv(hMcasp);
            break;
            
                /* Sets the bits related to transmit in XGBLCTL */ 
        case CSL_MCASP_CMD_ACTIVATE_XMT_CLK_SER:
            CSL_mcaspActivateXmtClkSer(hMcasp);
            break;
            
                /* Sets the bits related to receive in RGBLCTL */ 
        case CSL_MCASP_CMD_ACTIVATE_RCV_CLK_SER:
            CSL_mcaspActivateRcvClkSer(hMcasp);
            break;
            
                /* Sets the RSMRST and XSMRST bits in GBLCTL */ 
        case CSL_MCASP_CMD_ACTIVATE_SM_RCV_XMT:
            CSL_mcaspActivateSmRcvXmt(hMcasp);
            break;
            
                /* Sets the RFRST and XFRST bits in GBLCTL */ 
        case CSL_MCASP_CMD_ACTIVATE_FS_RCV_XMT:
            CSL_mcaspActivateFsRcvXmt(hMcasp);
            break;
            
                /* Enables/disables the DIT mode */ 
        case CSL_MCASP_CMD_SET_DIT_MODE:
            CSL_mcaspSetDitMode(hMcasp, *(Bool *) arg);
            break;
        default:
            status = CSL_ESYS_INVCMD;
        }
    return status;
}


