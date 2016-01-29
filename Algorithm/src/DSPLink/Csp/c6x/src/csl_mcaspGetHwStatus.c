/** ===========================================================================
 *   @file  csl_mcaspGetHwStatus.c
 *
 *   @path  $(CSLPATH)\mcasp\src
 *
 *   @desc  Multi channel audio serial port CSL implementation.
 *
 */  
    
/*  ===========================================================================
 *  Copyright (c) Texas Instruments Inc 2002, 2003, 2004, 2005
 *
 *  Use of this software is controlled by the terms and conditions found in the
 *  license agreement under which this software has been supplied.
 *  ===========================================================================
 */ 
    
/*  @(#) PSP/CSL 3.00.01.00[5905] (2005-01-12)  */ 
    
/* ============================================================================
 *  Revision History
 *  ===============
 *  12-Jan-2005 asr File Created.
 * ============================================================================
 */ 
    
#include <csl_mcasp.h>
#include <csl_mcaspAux.h>
    
/** ===========================================================================
 *   @n@b CSL_mcaspGetHwStatus
 *
 *   @b Description
 *   @n This function is used to get the value of various parameters of the
 *      McASP instance. The value returned depends on the query passed.
 *
 *   @b Arguments
 *   @verbatim
            hMcasp      Handle to the McASP instance
 
            myQuery     Query to be performed 
 
            response    Pointer to buffer to return the data requested by
                        the query passed
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK            - Successful completion of the
 *                                               query
 *
 *   @li                    CSL_ESYS_BADHANDLE - Invalid handle
 *
 *   @li                    CSL_ESYS_INVQUERY  - Query command not supported
 *
 *   @li                    CSL_ESYS_FAIL      - Generic failure
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *    @n Data requested by query is returned through the variable "response"
 *
 *   @b Modifies
 *   @n The input argument "response" is modified
 *
 *   @b Example
 *   @verbatim

        CSL_McaspHandle hMcasp;
        CSL_status      status;
        Bool            xmtUnderRun;
        
        status = CSL_mcaspGetHwStatus(hMcasp, CSL_MCASP_QUERY_XSTAT_XUNDRN, 
                                      &xmtUnderRun);
     @endverbatim
 * ============================================================================
 */ 
#pragma CODE_SECTION (CSL_mcaspGetHwStatus, ".text:csl_section:mcasp");

CSL_Status CSL_mcaspGetHwStatus (
    CSL_McaspHandle        hMcasp,
    CSL_McaspHwStatusQuery myQuery, 
    void                   *response 
) 
{
    CSL_Status status = CSL_SOK;
    if (hMcasp == NULL)
        return CSL_ESYS_BADHANDLE;
    switch (myQuery) {
        
                /* Return current transmit slot being transmitted */ 
        case CSL_MCASP_QUERY_CURRENT_XSLOT:
            *((Uint16 *) response) = CSL_mcaspGetCurrentXSlot (hMcasp);
                break;
            
                /* Return current receive slot being received */ 
        case CSL_MCASP_QUERY_CURRENT_RSLOT:
            *((Uint16 *) response) = CSL_mcaspGetCurrentRSlot (hMcasp);
                break;
            
                /* Return transmit error status bit */ 
        case CSL_MCASP_QUERY_XSTAT_XERR:
            *(Bool *) response = CSL_mcaspGetXmtErr (hMcasp);
                break;
            
                /* Return transmit clock failure flag status */ 
        case CSL_MCASP_QUERY_XSTAT_XCLKFAIL:
            *(Bool *) response = CSL_mcaspGetXmtClkFail (hMcasp);
                break;
            
                /* Return unexpected transmit frame sync flag status */ 
        case CSL_MCASP_QUERY_XSTAT_XSYNCERR:
            *(Bool *) response = CSL_mcaspGetXmtSyncErr (hMcasp);
                break;
            
                /* Return transmit underrun flag status */ 
        case CSL_MCASP_QUERY_XSTAT_XUNDRN:
            *((Bool *) response) = CSL_mcaspGetXmtUnderrun (hMcasp);
                break;
            
                /* Return transmit data ready flag status */ 
        case CSL_MCASP_QUERY_XSTAT_XDATA:
            (*(Bool *) response) = CSL_mcaspGetXmtDataReady (hMcasp);
                break;
            
                /* Return receive error status bit */ 
        case CSL_MCASP_QUERY_RSTAT_RERR:
            *((Bool *) response) = CSL_mcaspGetRcvErr (hMcasp);
                break;
            
                /* Return receive clk failure flag status */ 
        case CSL_MCASP_QUERY_RSTAT_RCLKFAIL:
            *((Bool *) response) = CSL_mcaspGetRcvClkFail (hMcasp);
                break;
            
                /* Return unexpected receive frame sync flag status */ 
        case CSL_MCASP_QUERY_RSTAT_RSYNCERR:
            *((Bool *) response) = CSL_mcaspGetRcvSyncErr (hMcasp);
                break;
            
                /* Return receive overrun flag status */ 
        case CSL_MCASP_QUERY_RSTAT_ROVRN:
            *((Bool *) response) = CSL_mcaspGetRcvOverrun (hMcasp);
                break;
            
                /* Return receive data ready flag status */ 
        case CSL_MCASP_QUERY_RSTAT_RDATA:
            *((Bool *) response) = CSL_mcaspGetRcvDataReady (hMcasp);
                break;
            
                /* Return status whether rrdy is set or not */ 
        case CSL_MCASP_QUERY_SRCTL_RRDY:{
                CSL_McaspSerQuery * serQuery = (CSL_McaspSerQuery *) response;
                status =
                    CSL_mcaspGetSerRcvReady (hMcasp, &(serQuery->serStatus),
                    serQuery->serNum);
                break;
            }
            
                /* Return status whether xrdy is set or not */ 
        case CSL_MCASP_QUERY_SRCTL_XRDY:
            {
                CSL_McaspSerQuery * serQuery = (CSL_McaspSerQuery *) response;
                status =
                    CSL_mcaspGetSerXmtReady (hMcasp, &(serQuery->serStatus),
                    serQuery->serNum);
                break;
            }
            
                /* 
                 * Return status whether serializer is configured as 
                 * transmit/receive/inactive
                 */ 
        case CSL_MCASP_QUERY_SRCTL_SRMOD:
            {
                CSL_McaspSerModeQuery * serQuery = 
                    (CSL_McaspSerModeQuery *) response;
                status =
                    CSL_mcaspGetSerMode (hMcasp, &(serQuery->serMode),
                    serQuery->serNum);
                break;
            }
            
                /* Return the value of XSTAT register */ 
        case CSL_MCASP_QUERY_XSTAT:
            *((Uint16 *) response) = CSL_mcaspGetXmtStat (hMcasp);
                break;
            
                /* Return the value of RSTAT register */ 
        case CSL_MCASP_QUERY_RSTAT:
            *((Uint16 *) response) = CSL_mcaspGetRcvStat (hMcasp);
                break;
            
                /* Return the XSMRST and XFRST field values GBLCTL register */ 
        case CSL_MCASP_QUERY_SM_FS_XMT:
            *((Uint8 *) response) = CSL_mcaspGetSmFsXmt (hMcasp);
                break;
            
                /* Return the RSMRST and RFRST field values GBLCTL register */ 
        case CSL_MCASP_QUERY_SM_FS_RCV:
            *((Uint8 *) response) = CSL_mcaspGetSmFsRcv (hMcasp);
                break;
            
                /* Return status of DITEN bit in DITCTL register */ 
        case CSL_MCASP_QUERY_DIT_MODE:
            *((Bool *) response) = CSL_mcaspGetDitMode (hMcasp);
                break;
        default:
            status = CSL_ESYS_INVQUERY;
     }
    return status;
}


