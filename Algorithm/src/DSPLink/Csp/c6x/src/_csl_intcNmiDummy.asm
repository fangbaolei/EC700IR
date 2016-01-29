;/*****************************************************\
; *  Copyright 2004, Texas Instruments Incorporated.  *
; *  All rights reserved.                             *
; *  Restricted rights to use, duplicate or disclose  *
; *  this   code   are  granted   through  contract.  *
; *                                                   *
; * "@(#) PSP/CSL  3.0.0.0  (2003-09-30)              *
;\*****************************************************/
;/** @file _csl_intcIsrDispatch.asm
; *
; *  @date 12th June, 2004
; *  @author Ruchika Kharwar
; */
    .global __CSL_intcNmiDummy
    .def __CSL_intcNmiDummy
    .sect  ".text:csl_section:intc"
__CSL_intcNmiDummy:
	b nrp
	nop 5    
		; Rev.No.   Date/Time               ECN No.          Modifier        ;
; -------   ---------               -------          --------        ;

; 1          Aug 11:10:45 9         2273             xkeshavm        ;
;                                                                    ;
; To upload API CSL 0.51 Release                                     ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 
