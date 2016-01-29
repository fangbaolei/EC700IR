/*  ============================================================================
 *   Copyright (c) Texas Instruments Inc 2002, 2003, 2004, 2005
 *
 *   Use of this software is controlled by the terms and conditions found in the
 *   license agreement under which this software has been supplied.
 *   ===========================================================================
 */
 
/* ============================================================================
 *   @file  _csl_mcbsp.h
 *
 *   @path  $(CSLPATH)\inc
 *
 *   @desc  Header file for private functional layer of McBSP CSL
 *
 */

#ifndef __CSL_MCBSP_H_
#define __CSL_MCBSP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <csl.h>
#include <csl_mcbsp.h>

/******************************************************************************\
* MCBSP Private function declarations
\******************************************************************************/

/*
 * =============================================================================
 *   @func _csl_mcbspBlockAssign
 *
 *   @desc
 *      The funtion is used to assign block to a particular partition This is an 
 *      Internal function and is used by the CSL_mcbspHwControl function
 *
 *   @arg   hMcbsp
 *          Handle of the McBSP
 *
 *   @arg   partition
 *          Partition to which the block is to be assigned to
 *
 *   @arg   block
 *          Identifies the block
 *
 *   @ret   CSL_Status
 *          CSL_SOK        - Successful completion
 *          CSL_ESYS_FAIL  - Operation could not the done
 *
 *   @eg
 *          CSL_McbspHandle     hMcbsp;
 
            _CSL_mcbspBlockAssign(hMcbsp, CSL_MCBSP_PARTITION_ATX, 
                                  CSL_MCBSP_BLOCK0);
            
            ...
 *
 * =============================================================================
 */
CSL_Status  _CSL_mcbspBlockAssign (
    CSL_McbspHandle         hMcbsp,
    CSL_McbspPartition      partition,
    CSL_McbspBlock          block
);

/*
 * =============================================================================
 *   @func   _csl_mcbspChannelControl.c
 *
 *   @desc   Eanble one Channel in Multi Channel Selection Mode
 *
 *
 *   @arg   hMcbsp
 *          Device Handle;
 *
 *   @arg   channelNo
 *          Channel Number
 *
 *   @arg   block
 *          Block to which the channel belongs to
 *
 *   @arg   operation
 *          The operation to be performed
 *
 *   @ret   CSL_Status
 *          CSL_SOK               - Successful completion
 *          CSL_EMCBSP_INVPARAMS  - Invalid Parameter
 *          CSL_EMCBSP_INVMODE    - Invalid Mode
 *
 *   @eg
 *          MCBSP_Handle hMcbsp;
 *          hMcbsp = CSL_mcbspOpen(hMcbsp, CSL_MCBSP_1, NULL, &status);
 *          _CSL_mcbspChannelControl( hMcbsp, 11, 1, CSL_MCBSP_CHAN_RX_ENABLE);
 *
 *
 * =============================================================================
 */

CSL_Status  _CSL_mcbspChannelControl (
    CSL_McbspHandle     hMcbsp,
    Uint16              channelNo,
    CSL_McbspBlock      block,
    CSL_McbspChCtrl     operation
);

/*
 * =============================================================================
 *   @func   _csl_mcbspClearFrmSyncErr.c
 *
 *   @desc   Clear Frame Sync Error for Receiver and Transmitter.
 *
 *   @arg   hMcbsp
 *          Device Handle;
 *
 *   @arg   chanType
 *          Selects receiver/transmitter.
 *
 *   @ret   void
 *          None
 *
 *   @eg
 *          CSL_McbspHandle hMcbsp;
 *          hMcbsp = CSL_mcbspOpen(hMcbsp, CSL_MCBSP_1,NULL, &status );
 *          _CSL_mcbspClearFrmSyncErr( hMcbsp, CSL_MCBSP_RX);
 *
 *
 * =============================================================================
 */

void  _CSL_mcbspClearFrmSyncErr (
    CSL_McbspHandle     hMcbsp,
    CSL_McbspChType     chanType
);

/*
 * =============================================================================
 *   @func   _csl_mcbspResetCtrl.c
 *
 *   @desc   Enable/Disable Frame Sync Logic/Sample Rate Generator and Receiver 
 *           and Transmitter Operation
 *
 *   @arg   hMcbsp
 *          Device Handle
 *
 *   @arg   selectMask
 *          Selects enable/disable of receiverCfg/transmitter. Bitwise OR 
 *          of MCBSP_CTRL_XXXX
 *
 *   @ret   void
 *          None
 *
 *   @eg
 *          CSL_McbspHandle hMcbsp;
 *          hMcbsp = CSL_mcbspOpen(hMcbsp, CSL_MCBSP_1,NULL, &status );
 *          _CSL_mcbspResetCtrl (hMcbsp, CSL_MCBSP_CTRL_SRG_ENABLE
 *                                  |CSL_MCBSP_CTRL_FSYNC_DISABLE
 *                                  |CSL_MCBSP_CTRL_RX_ENABLE
 *                                  |CSL_MCBSP_CTRL_TX_ENABLE );
 *
 * =============================================================================
 */

void  _CSL_mcbspResetCtrl (
    CSL_McbspHandle     hMcbsp,
    CSL_BitMask16       selectMask
);

/*
 * =============================================================================
 *   @func   _csl_mcbspRegReset.c
 *
 *   @desc   Function to reset all the registers values
 *
 *
 *   @arg   hMcbsp
 *          Device Handle;
 *
 *   @ret  void
 *         None
 *
 *   @eg
 *          CSL_McbspHandle hMcbsp;
 *          hMcbsp = CSL_mcbspOpen(hMcbsp, CSL_MCBSP_1,NULL, &status );
 *          _CSL_mcbspRegReset(hMcbsp);
 *
 *
 * =============================================================================
 */
void  _CSL_mcbspRegReset (
    CSL_McbspHandle     hMcbsp
);

#ifdef __cplusplus
}
#endif

#endif

