;/*****************************************************\
; *  Copyright 2004, Texas Instruments Incorporated.  *
; *  All rights reserved.                             *
; *  Restricted rights to use, duplicate or disclose  *
; *  this   code   are  granted   through  contract.  *
; *                                                   *
; * "@(#) PSP/CSL  3.0.0.0  (2003-09-30)              *
;\*****************************************************/
;/** @file _csl_intcIntrEnDisRes.asm
; *
; *  @date 12th June, 2004
; *  @author Ruchika Kharwar
; */
	; A0 contains the Register to be read 
_CSL_INTC_GIE_MASK .equ 0x00000001
_CSL_INTC_GIE_MASK_DISABLE .equ 0xfffffffe
_CSL_INTC_GEE .equ 4h	
_CSL_INTC_XEN .equ 8h	
	
    .ref __CSL_intcVectorTable
    .sect  ".text:csl_section:intc"
	.global __CSL_intcIvpSet
__CSL_intcIvpSet:
	bnop b3,2
	mvkl __CSL_intcVectorTable, b0
	mvkh __CSL_intcVectorTable, b0
	mvc b0, istp

	.global _CSL_intcInterruptEnable
_CSL_intcInterruptEnable:
	;input argument is in a4
	;return value is in a4
	bnop b3, 1
	mvk   1, a5
	mvc ier, b0
	||shl  a5, a4, a5 ; Or mask obtained
	shru b0, a5, a4 ; return value obtained
	||or   a5, b0, b0
	mvc  b0, ier
	||and a4,1,a4
   
	.global _CSL_intcInterruptDisable
_CSL_intcInterruptDisable:
	;input argument is in a4
	;return value is in a4
	bnop b3, 1
	mvk   1, a5
	mvc ier, b0
	||shl  a5, a4, a5 ; Or mask obtained
	shru b0, a4, a4 ; return value obtained
	||andn  b0, a5, b0
	mvc  b0, ier
	||and a4,1,a4
  
	.global _CSL_intcInterruptClear
_CSL_intcInterruptClear:
	;input argument is in a4
	bnop b3, 1
	mv   a4,b2
	mvk   1, b1
	shl  b1, b2, b1 ; Or mask obtained
	mvc  b1, icr
   
   .global _CSL_intcInterruptSet
_CSL_intcInterruptSet:
	bnop b3, 1
	mv   a4,b2
	mvk   1, b1
	shl  b1, b2, b1 ; Or mask obtained
	mvc  b1, isr   
                       
   .global _CSL_intcInterruptRestore
_CSL_intcInterruptRestore:
	;a4 contains the vectorId
	;b4 contains the value to be restored
	b b3
	mv a4,b1
	shl .s2 b4,b1,b4
	mvc ier,b0
	or b0,b4,b0
	mvc b0,ier
	
	
	.global _CSL_intcGlobalEnable
_CSL_intcGlobalEnable:
	bnop b3
	mv a4,a0
	||mvc csr, b0
    and b0 , _CSL_INTC_GIE_MASK, a3
 [a0]  stnw a3 , *a0
    ||or  b0 , _CSL_INTC_GIE_MASK, b0	
	mvc b0 , csr
	mvk 1, a4

	; CSL_Status = CSL_SOK
	.global _CSL_intcGlobalDisable
_CSL_intcGlobalDisable:	
	bnop b3
	mv a4,a0
    ||mvc csr, b0
    and b0 , 1, a3
 [a0]stnw a3 , *a0
    ||and  b0 , _CSL_INTC_GIE_MASK_DISABLE, b0	
	mvc b0 , csr
	mvk 1, a4
	
	.global _CSL_intcGlobalRestore
_CSL_intcGlobalRestore:	
	bnop b3
    mvc csr, b0
    and a4, _CSL_INTC_GIE_MASK, b1
    or b1, b0,b0
    mvk 1, a4 ; CSL_Status 
	mvc b0 , csr	
		
	.global _CSL_intcGlobalNmiEnable		   
_CSL_intcGlobalNmiEnable:
	bnop b3, 1
	mvc ier, b0
	or 2, b0, b0
	mvc b0, ier
	mvk 1, a4
	
	.global _CSL_intcGlobalExcepEnable
_CSL_intcGlobalExcepEnable:
	bnop b3, 1
	mvc tsr, b0
	or _CSL_INTC_GEE, b0, b0
	mvc b0, tsr
	mvk 1, a4


	.global _CSL_intcGlobalExtExcepEnable
_CSL_intcGlobalExtExcepEnable:
	bnop b3, 1
	mvc tsr, b0
	or _CSL_INTC_XEN, b0, b0
	mvc b0, tsr
	mvk 1, a4
	
	.global _CSL_intcGlobalExcepClear
_CSL_intcGlobalExcepClear:
	b b3
	mvk 1, a3
	shl a3, a4, a3
	mv a3, b2
	mvc b2, ecr
	mvk 1, a4
	
	.global _CSL_intcQueryInterruptStatus			
_CSL_intcQueryInterruptStatus:
	b b3
	mvk 1, b1
	mvc ifr, b0
	shl b1, a4,a5
	and a5,b0,a5
	shru a5,a4,a4
	
	
	
	
	
		
				
	
 ; Rev.No.   Date/Time               ECN No.          Modifier        ;
; -------   ---------               -------          --------        ;

; 1          Aug 11:10:44 9         2273             xkeshavm        ;
;                                                                    ;
; To upload API CSL 0.51 Release                                     ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 
