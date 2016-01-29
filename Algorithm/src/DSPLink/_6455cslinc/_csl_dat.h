/*  ===========================================================================
 *   Copyright (c) Texas Instruments Inc 2002, 2003, 2004, 2005
 *
 *   Use of this software is controlled by the terms and conditions found in the
 *   license agreement under which this software has been supplied.
 *   ===========================================================================
 */
 
/*  ============================================================================
 * @file  _csl_dat.h
 *
 * @desc  Header file for DAT System Service APIs 
 * 
 *  @path  $(CSLPATH)\inc
 * 
 */

/* Dat State structure */
typedef struct _CSL_DatStateStruct {
    /* QDMA Channel number being requested */
    Int32     qchNum;
    /* Region of operation */         
    Int32     regionNum;
    /* Transfer completion code dedicated for DAT */      
    Int32     tccNum;
    /* Parameter Entry for this channel */         
    Int32     paramNum;
    /* Priority/Que number on which the transfer requests are submitted */        
    Int32     priority;       
    /* Pending/Not */                                                                                  
    Bool    pending;          
} CSL_DatStateStruct;

extern CSL_DatStateStruct  _CSL_datStateStruct;
