/*  ===========================================================================
 *   Copyright (c) Texas Instruments Inc 2002, 2003, 2004, 2005
 *
 *   Use of this software is controlled by the terms and conditions found in the
 *   license agreement under which this software has been supplied.
 *   ===========================================================================
 */

/** ============================================================================
 *   @mainpage DAT 
 *
 *  @section Introduction
 *
 *  @subsection xxx Purpose and Scope
 *  The purpose of this document is to detail the DAT Module APIs
 *
 *  @subsection aaa Terms and Abbreviations
 *   -# CSL:  Chip Support Library
 *   -# API:  Application Programmer Interface
 *
 *  @subsection References
 *  -# EDMA: Enhanced Direct Memory Access 
 *  @subsection Assumptions
 *    The user must allocate a free QDMA channel, free parameter set number,
 *    free transfer completion interrupt number and must also specify the shadow
 *    region of the DAT modules operation. 
 *    
 *   A second transfer cannot be submitted before the previous one is complete.
 */
 
/** ============================================================================
 * @file  csl_dat.h
 *
 * @brief  Header file for DAT System Service APIs 
 * 
 * @path  $(CSLPATH)\inc
 *
 */ 
 
#ifndef _CSL_DAT_H_
#define _CSL_DAT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <csl.h>
#include <soc.h>
#include <cslr_edma3cc.h>

/* global defines */
#define DAT_1D2D      0x1  
/**< Transfer type is 1D2D */
#define DAT_2D1D      0x2  
/**< Transfer type is 2D1D */
#define DAT_2D2D      0x3  
/**< Transfer type is 2D2D */

/* global typedef declarations */

/* global variable declarations */

/**
 * @brief   DAT Setup structure
 */
typedef struct DAT_Setup {
	/** QDMA Channel number being requested */
    Int    qchNum;
    /** Region of operation */           
    Int    regionNum;
    /** Transfer completion code dedicated for DAT */        
    Int    tccNum;           
    /** Parameter Set number for this channel */
    Int    paramNum;
    /** Priority/Queue number on which the transfer requests  are submitted */         
    Int    priority;         
} DAT_Setup;

/* global function declarations */
/* ============================================================================
 *   @n@b DAT_open 
 *
 *   @b Description
 *   @n This API 
 *        a. Sets up the channel to Parameter set mapping \n
 *        b. Sets up the priority. This is essentially done by specifying the  
 *           queue to which the channel is submitted to viz Queue0- Queue7. 
 *           Queue 0 being the highest priority. \n
 *        c. Enables the region access bit for the channel if a region is 
 *       specified.
 *
 *   @b Arguments
     @verbatim
            setup         Pointer to the DAT setup structure
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_SOK
 *
 *   <b> Pre Condition </b> 
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  The EDMA registers are configured with the setup values passed. 
 *
 *   @b Modifies
 *   @n None   
 *
 *   @b Example
 *   @verbatim
        DAT_Setup datSetup;
        datSetup.qchNum = CSL_DAT_QCHA0;
        datSetup.regionNum = CSL_DAT_REGION_GLOBAL ;
        datSetup.tccNum = 1;
        datSetup.paramNum = 0 ;
        datSetup.priority = CSL_DAT_PRI_0;
    
        DAT_open(&datSetup);
     @endverbatim
 * ===========================================================================
 */

Int16 DAT_open (
    DAT_Setup    *setup 
);
/* ============================================================================
 *   @n@b DAT_close 
 *
 *   @b Description
 *   @n This API 
 *         a. Disables the region access bit if specified.
 *
 *   @b Arguments
 *   @n None
 *
 *   <b> Return Value </b> 
 *   @n None
 *
 *   <b> Pre Condition </b> 
 *   @n  DAT_open() must be successfully invoked prior to this call.
 *
 *   <b> Post Condition </b>
 *   @n  None 
 *
 *   @b Modifies
 *   @n None   
 *
 *   @b Example
 *   @verbatim
        DAT_Setup datSetup;
        datSetup.qchNum = CSL_DAT_QCHA0;
        datSetup.regionNum = CSL_DAT_REGION_GLOBAL ;
        datSetup.tccNum = 1;
        datSetup.paramNum = 0 ;
        datSetup.priority = CSL_DAT_PRI_0;
    
        DAT_open(&datSetup);
        ...
        DAT_close();    
     @endverbatim
 * ===========================================================================
 */
void DAT_close (
    void    
);
/* ============================================================================
 *   @n@b DAT_copy 
 *
 *   @b Description
 *   @n This API copies data from source to destination for one dimension 
 *    transfer. 
 *
 *   @b Arguments
     @verbatim
            src         Source memory address for the data transfer

            dst         Destination memory address of the data transfer

            byteCnt     Number of bytes to be transferred
     @endverbatim
 *
 *   <b> Return Value </b>  tccNum
 *
 *   <b> Pre Condition </b> 
 *   @n  DAT_open() must be successfully invoked prior to this call.
 *
 *   <b> Post Condition </b>
 *   @n  The EDMA registers are configured to transfer byteCnt bytes from the 
 *       source memory address to the destination memory address 
 *
 *   @b Modifies
 *   @n EDMA registers   
 *
 *   @b Example
 *   @verbatim
        DAT_Setup datSetup;
        Uint8 dst1d[8*16];
        Uint8 src1d[8*16];
        datSetup.qchNum = CSL_DAT_QCHA0;
        datSetup.regionNum = CSL_DAT_REGION_GLOBAL ;
        datSetup.tccNum = 1;
        datSetup.paramNum = 0 ;
        datSetup.priority = CSL_DAT_PRI_0;
    
        DAT_open(&datSetup);
        ...
        DAT_copy(&src1d,&dst1d,256);
        ...
        DAT_close();    
     @endverbatim
 * ===========================================================================
 */
Uint32 DAT_copy (
    void        *src ,       
    void        *dst ,       
    Uint16    byteCnt        
);
/* ============================================================================
 *   @n@b DAT_fill 
 *
 *   @b Description
 *   @n This API fill destination by given fill value.
 *
 *   @b Arguments
     @verbatim
            dst         Destination memory address to be filled

            byteCnt Number of bytes to be filled

            value       Value to be filled  
     @endverbatim
 *
 *   <b> Return Value </b>  tccNum
 *
 *   <b> Pre Condition </b> 
 *   @n  DAT_open() must be successfully invoked prior to this call.
 *
 *   <b> Post Condition </b>
 *   @n  The EDMA registers are configured to transfer a value to byteCnt bytes 
 *       of the destination memory address 
 *
 *   @b Modifies
 *   @n EDMA registers   
 *
 *   @b Example
 *   @verbatim
        DAT_Setup datSetup;
        Uint8 dst[8*16];
        Uint8 fillVal;
    
        datSetup.qchNum = CSL_DAT_QCHA0;
        datSetup.regionNum = CSL_DAT_REGION_GLOBAL ;
        datSetup.tccNum = 1;
        datSetup.paramNum = 0 ;
        datSetup.priority = CSL_DAT_PRI_0;
    
        DAT_open(&datSetup);
        ...
        fillVal = 0x5a;
        DAT_fill(&dst,256,&fillval);
        ...
        DAT_close();    

     @endverbatim
 * ===========================================================================
 */
Uint32 DAT_fill(
    void      *dst,          
    Uint16    byteCnt,       
    Uint32    *value         
);
/* ============================================================================
 *   @n@b DAT_wait 
 *
 *   @b Description
 *   @n This API Waits for completion of the ongoing transfer. 
 *
 *   @b Arguments
     @verbatim
            id         Channel number of the previous transfer
     @endverbatim
 *
 *   <b> Return Value </b>  
 *   @n  None
 *
 *   <b> Pre Condition </b> 
 *   @n  DAT_copy()/DAT_fill must be successfully invoked prior to this call.
 *
 *   <b> Post Condition </b>
 *   @n  Indicates that the transfer ongoing is complete
 *
 *   @b Modifies
 *   @n None   
 *
 *   @b Example
 *   @verbatim
        DAT_Setup datSetup;
        Uint8 dst1d[8*16];
        Uint8 src1d[8*16];
        datSetup.qchNum = CSL_DAT_QCHA0;
        datSetup.regionNum = CSL_DAT_REGION_GLOBAL ;
        datSetup.tccNum = 1;
        datSetup.paramNum = 0 ;
        datSetup.priority = CSL_DAT_PRI_0;
    
        DAT_open(&datSetup);
        ...
        id = DAT_copy(&src1d,&dst1d,256);

        DAT_wait(id);
        ...
        DAT_close();

     @endverbatim
 * ===========================================================================
 */
void DAT_wait (
    Uint32    id             
);

/* ============================================================================
 *   @n@b DAT_busy 
 *
 *   @b Description
 *   @n This API polls for transfer completion. 
 *
 *   @b Arguments
     @verbatim
            id         Channel number of the previous transfer
     @endverbatim
 *
 *   <b> Return Value </b>  Int16
 *                              TRUE/FALSE
 *
 *   <b> Pre Condition </b> 
 *   @n  DAT_copy()/DAT_fill must be successfully invoked prior to this call.
 *
 *   <b> Post Condition </b>
 *   @n  Indicates that the transfer ongoing is complete
 *
 *   @b Modifies
 *   @n None   
 *
 *   @b Example
 *   @verbatim
        DAT_Setup datSetup;
        Uint8 dst1d[8*16];
        Uint8 src1d[8*16];
        datSetup.qchNum = CSL_DAT_QCHA0;
        datSetup.regionNum = CSL_DAT_REGION_GLOBAL ;
        datSetup.tccNum = 1;
        datSetup.paramNum = 0 ;
        datSetup.priority = CSL_DAT_PRI_0;
    
        DAT_open(&datSetup);
        ...
        id = DAT_copy(&src1d,&dst1d,256);

        do {
            ... 
           }while (DAT_busy(id));
        ...
        DAT_close();

     @endverbatim
 *
 */
Int16 DAT_busy (
    Uint32 id                
);

/* ============================================================================
 *   @n@b DAT_copy2d 
 *
 *   @b Description
 *   @n This API copies data from source to destination for two dimension 
 *    transfer. 
 *
 *   @b Arguments
     @verbatim
            type        Indicates the type of the transfer
                            DAT_1D2D - 1 dimension to 2 dimension
                            DAT_2D1D - 2 dimension to 1 dimension
                            DAT_2D2D - 2 dimension to 2 dimension
                        
            src         Source memory address for the data transfer

            dst         Destination memory address of the data transfer

            lineLen     Number of bytes per line
            
            lineCnt     Number of lines
            
            linePitch   Number of bytes between start of one line to start 
                        of next line
                        
     @endverbatim
 *
 *   <b> Return Value </b>  tccNum
 *
 *   <b> Pre Condition </b> 
 *   @n  DAT_open() must be successfully invoked prior to this call.
 *
 *   <b> Post Condition </b>
 *   @n  The EDMA registers are configured for the transfer  
 *
 *   @b Modifies
 *   @n EDMA registers   
 *
 *   @b Example
 *   @verbatim
        DAT_Setup datSetup;
        Uint8 dst2d[8][20];
        Uint8 src1d[8*16];
    
        datSetup.qchNum = CSL_DAT_QCHA0;
        datSetup.regionNum = CSL_DAT_REGION_GLOBAL ;
        datSetup.tccNum = 1;
        datSetup.paramNum = 0 ;
        datSetup.priority = CSL_DAT_PRI_0;
    
        DAT_open(&datSetup);
        ...
        id = DAT_copy2d(DAT_1D2D,src1d,dst2d,16,8,20);  

        do {
        ... 
        }while (DAT_busy(id));
        ...
        DAT_close();
     @endverbatim
 * ===========================================================================
 */
Uint32 DAT_copy2d (
    Uint32    type,          
    void      *src,          
    void      *dst,          
    Uint16    lineLen,       
    Uint16    lineCnt,       
    Uint16    linePitch           
);

/* ============================================================================
 *   @n@b DAT_setPriority 
 *
 *   @b Description
 *   @n Sets the priority bit value PRI of OPT register. The priority value 
 *      can be set by using the type CSL_DatPriority. 
 *
 *   @b Arguments
     @verbatim
            priority         priority value
     @endverbatim
 *
 *   <b> Return Value </b>  
 *   @n None
 *
 *   <b> Pre Condition </b> 
 *   @n  DAT_open must be successfully invoked prior to this call.
 *
 *   <b> Post Condition </b>
 *   @n  OPT register is set for the priority value
 *
 *   @b Modifies
 *   @n OPT register   
 *
 *   @b Example
 *   @verbatim
        DAT_Setup datSetup;
        Uint8 dst2d[8][20];
        Uint8 src1d[8*16];
    
        datSetup.qchNum = CSL_DAT_QCHA0;
        datSetup.regionNum = CSL_DAT_REGION_GLOBAL ;
        datSetup.tccNum = 1;
        datSetup.paramNum = 0 ;
        datSetup.priority = CSL_DAT_PRI_0;
    
        DAT_open(&datSetup);
        ...
        DAT_setPriority(CSL_DAT_PRI_3); 

     @endverbatim
 *
 */
void DAT_setPriority (
    Int    priority         
);

#ifdef __cplusplus
}
#endif
#endif /*_CSL_DAT_H_*/

