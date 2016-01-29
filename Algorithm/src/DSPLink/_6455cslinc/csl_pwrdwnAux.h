/* ============================================================================
 * Copyright (c) Texas Instruments Inc 2002, 2003, 2004, 2005
 *
 * Use of this software is controlled by the terms and conditions found in the
 * license agreement under which this software has been supplied.
 * ============================================================================
 */

/** ===========================================================================
 * @file csl_pwrdwnAux.h
 *   
 * @brief API Auxilary header file for PWRDWN CSL 
 *   
 * @path $(CSLPATH)\inc
 *
 *   @desc  PWRDWN module
 *   Modification : 30th Aug 2004
 *   Modified function 
 *   Added documentation
 * ============================================================================
 */
/* =============================================================================
 *  Revision History
 *  ===============
 *  20-Dec-2005   Modified CSL_pwrdwnGetPage1SleepStatus () and 
 *                CSL_pwrdwnGetPage0SleepStatus () to fix issue in release 
 *                mode
 *  01-Mar-2006   Modified CSL_pwrdwnPage1Wake () to fix bug PSG00000880
 * =============================================================================
 */
#ifndef _CSL_PWRDWNAUX_H
#define _CSL_PWRDWNAUX_H

#ifdef __cplusplus
extern "C" {
#endif
    
#define CSL_IDEF_INLINE static inline

/** @addtogroup CSL_PWRDWN_FUNCTION_INTERNAL
@{ */

/** ============================================================================
 *   @n@b  CSL_pwrdwnPage0Sleep
 *
 *   @b Description
 *   @n Manual power down. Puts page0 on specified port to sleep
 *
 *   @b Arguments
 *   @verbatim

            hPwr       Handle to the power down module

            arg        consists of port number and  page0 on the port that need
                       to be put to sleep   

     @endverbatim
 *
 *   <b> Return Value </b>  Status (CSL_SOK)
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *    @n Puts page0 on specified port to sleep
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim

       CSL_PwrdwnObj    pwrObj;
       CSL_PwrdwnConfig    pwrConfig;
       CSL_PwrdwnHandle    hPwr;
       CSL_PwrdwnPortData  *arg

       arg.portNum = 0x0;
       arg.data = 0x1;
    
       // Init Module
       ...
       if (CSL_pwrdwnInit(NULL) != CSL_SOK)
           exit;
       // Opening a handle for the Module   
       hPwr = CSL_pwrdwnOpen (&pwrObj, CSL_PWRDWN, NULL, NULL);
    
       CSL_pwrdwnPage0Sleep(hPwr, (CSL_PwrdwnPortData *)&arg);

     @endverbatim
 * ===========================================================================
 */
CSL_IDEF_INLINE 
CSL_Status CSL_pwrdwnPage0Sleep (
    CSL_PwrdwnHandle   hPwr,
    CSL_PwrdwnPortData *arg
)
{
    CSL_FINS(hPwr->l2pwrdwnRegs->L2PDSLEEP[arg->portNum], 
            L2PWRDWN_L2PDSLEEP_P0, arg->data);
    
    return CSL_SOK;
}


/** ============================================================================
 *   @n@b  CSL_pwrdwnPage1Sleep
 *
 *   @b Description
 *   @n Manual power down. Puts page1 on specified port to sleep
 *
 *   @b Arguments
 *   @verbatim

            hPwr       Handle to the power down module

            arg        consists of port number and  page1 on the port that need
                       to be put to sleep   

     @endverbatim
 *
 *   <b> Return Value </b>  Status (CSL_SOK)
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *    @n Puts page1 on specified port to sleep
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim

       CSL_PwrdwnObj       pwrObj;
       CSL_PwrdwnConfig    pwrConfig;
       CSL_PwrdwnHandle    hPwr;
       CSL_PwrdwnPortData  *arg

       arg.portNum = 0x0;
       arg.data = 0x1;
    
       // Init Module
       ...
       if (CSL_pwrdwnInit(NULL) != CSL_SOK)
           exit;
       // Opening a handle for the Module   
       hPwr = CSL_pwrdwnOpen (&pwrObj, CSL_PWRDWN, NULL, NULL);
    
       CSL_pwrdwnPage1Sleep(hPwr, (CSL_PwrdwnPortData *)&arg);

     @endverbatim
 * ===========================================================================
 */
CSL_IDEF_INLINE 
CSL_Status CSL_pwrdwnPage1Sleep (
    CSL_PwrdwnHandle   hPwr,
    CSL_PwrdwnPortData *arg
)
{
    CSL_FINS(hPwr->l2pwrdwnRegs->L2PDSLEEP[arg->portNum], 
                       L2PWRDWN_L2PDSLEEP_P1, arg->data);
    
    return CSL_SOK;
}


/** ============================================================================
 *   @n@b  CSL_pwrdwnPage0Wake
 *
 *   @b Description
 *   @n Manual page wake. Awakes page0 on specified port.
 *
 *   @b Arguments
 *   @verbatim

            hPwr       Handle to the power down module

            wake       consists of port number and  page0 on the port that need
                       to be put to woken   

     @endverbatim
 *
 *   <b> Return Value </b>  Status (CSL_SOK)
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *    @n Awakes page0 on specified port
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim

        CSL_PwrdwnObj pwrObj;
        CSL_PwrdwnConfig pwrConfig;
        CSL_PwrdwnHandle hPwr;

        arg.portNum = 0x0;
        arg.data = 0x1;

        // Init Module
        ...
        if (CSL_pwrdwnInit(NULL) != CSL_SOK)
          exit;
        // Opening a handle for the Module  
        hPwr = CSL_pwrdwnOpen (&pwrObj, CSL_PWRDWN, NULL, NULL);
    
        CSL_pwrdwnPage0Wake(hPwr, (CSL_PwrdwnPortData *)&arg);
     @endverbatim
 * ===========================================================================
 */
CSL_IDEF_INLINE 
CSL_Status CSL_pwrdwnPage0Wake (
    CSL_PwrdwnHandle   hPwr,
    CSL_PwrdwnPortData *arg
)
{
    CSL_FINS(hPwr->l2pwrdwnRegs->L2PDWAKE[arg->portNum],
                       L2PWRDWN_L2PDWAKE_P0, arg->data);
    
    return CSL_SOK;
}


/** ============================================================================
 *   @n@b  CSL_pwrdwnPage1Wake
 *
 *   @b Description
 *   @n Manual page wake. Awakes page0 on specified port.
 *
 *   @b Arguments
 *   @verbatim

            hPwr       Handle to the power down module

            wake       consists of port number and  page1 on the port that need
                       to be put to woken   

     @endverbatim
 *
 *   <b> Return Value </b>  Status (CSL_SOK)
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *    @n Awakes page1 on specified port
 *
 *   @b Modifies
 *   @n  None
 *
 *   @b Example
 *   @verbatim

        CSL_PwrdwnObj pwrObj;
        CSL_PwrdwnConfig pwrConfig;
        CSL_PwrdwnHandle hPwr;

        arg.portNum = 0x0;
        arg.data = 0x1;

        // Init Module
        ...
        if (CSL_pwrdwnInit(NULL) != CSL_SOK)
          exit;
        // Opening a handle for the Module  
        hPwr = CSL_pwrdwnOpen (&pwrObj, CSL_PWRDWN, NULL, NULL);
    
        CSL_pwrdwnPage1Wake(hPwr, (CSL_PwrdwnPortData *)&arg);
     @endverbatim
 * ===========================================================================
 */
CSL_IDEF_INLINE
CSL_Status CSL_pwrdwnPage1Wake (
    CSL_PwrdwnHandle   hPwr,
    CSL_PwrdwnPortData *arg
)
{
    CSL_FINS(hPwr->l2pwrdwnRegs->L2PDWAKE[arg->portNum],
                  L2PWRDWN_L2PDWAKE_P1, arg->data);
    
    return CSL_SOK;
}


/** ============================================================================
 *   @n@b  CSL_pwrdwnGetPage0SleepStatus
 *
 *   @b Description
 *   @n  Queries page0 status.
 *
 *   @b Arguments
 *   @verbatim

         hPwr      Handle to the power down module

         response  Place holder for sleep status of the page0 on specified port

     @endverbatim
 *
 *   <b> Return Value </b>  Status (CSL_SOK)
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

      CSL_PwrdwnObj pwrObj;
      CSL_PwrdwnConfig pwrConfig;
      CSL_PwrdwnHandle hPwr;
      CSL_PwrdwnPortData response;

      response.portNum = 0x0;
      
      // Init Module
      ...
      if (CSL_pwrdwnInit(NULL) != CSL_SOK)
         exit;
      // Opening a handle for the Module    
      hPwr = CSL_pwrdwnOpen (&pwrObj, CSL_PWRDWN, NULL, NULL);
    
      CSL_pwrdwnGetPage0SleepStatus(hPwr, (CSL_PwrdwnPortData *)&response);
      
     @endverbatim
 * ===========================================================================
 */
CSL_IDEF_INLINE 
void CSL_pwrdwnGetPage0SleepStatus (
    CSL_PwrdwnHandle   hPwr,
    CSL_PwrdwnPortData *response
)
{   
    volatile Uint8 data;
    
    data = CSL_FEXT(hPwr->l2pwrdwnRegs->L2PDSTAT[response->portNum],
                                                        L2PWRDWN_L2PDSTAT_P0);
    response->data = data;
}



/** ============================================================================
 *   @n@b  CSL_pwrdwnGetPage1SleepStatus
 *
 *   @b Description
 *   @n  Queries page1 status.
 *
 *   @b Arguments
 *   @verbatim

         hPwr      Handle to the power down module

         response  Place holder for sleep status of the page1 on specified port

     @endverbatim
 *
 *   <b> Return Value </b>  Status (CSL_SOK)
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

      CSL_PwrdwnObj pwrObj;
      CSL_PwrdwnConfig pwrConfig;
      CSL_PwrdwnHandle hPwr;
      CSL_PwrdwnPortData response;

      response.portNum = 0x0;
      
      // Init Module
      ...
      if (CSL_pwrdwnInit(NULL) != CSL_SOK)
         exit;
      // Opening a handle for the Module    
      hPwr = CSL_pwrdwnOpen (&pwrObj, CSL_PWRDWN, NULL, NULL);
    
      CSL_pwrdwnGetPage1SleepStatus(hPwr, (CSL_PwrdwnPortData *)&response);
      
     @endverbatim
 * ===========================================================================
 */
CSL_IDEF_INLINE
void CSL_pwrdwnGetPage1SleepStatus (
    CSL_PwrdwnHandle   hPwr,
    CSL_PwrdwnPortData *response
)
{   
    volatile Uint8 data;
    
    data = CSL_FEXT(hPwr->l2pwrdwnRegs->L2PDSTAT[response->portNum],
                                                        L2PWRDWN_L2PDSTAT_P1);
    response->data = data;
}

#ifdef __cplusplus
extern "C" {
#endif

/**
@}
*/
#endif /* _CSL_PWRDWNAUX_H_ */

