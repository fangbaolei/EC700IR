;/*   ==========================================================
; *   Copyright (c) Texas Instruments Inc , 2004
; *
; *   Use of this software is controlled by the terms and conditions found
; *   in the license agreement under which this software has been supplied
; *   priovided
; *   ==========================================================
;*/
;/** @file csl_chipReadReg.asm
; *
; *  @brief CSL chip APIs
; *
; *  Description
; *	- API for reading of the control registers is defined here 
; *  @date 15th Apr, 2004
; */
	; A0 contains the Register to be read 
	.global _CSL_chipReadReg
_CSL_chipReadReg:
    mvk 12, a1
  ||mvkl chipRegTable,b0 ; The offset of the case is attempted to be obtained here
    mvkh chipRegTable,b0
    shl a4,3,a4
    add .s2 a4,b0,b0
	bnop b0,4
	nop
  ||nop
  ||nop
  ||nop
  ||nop
  ||nop
  nop	
  ||nop
  ||nop	
  ||nop
  mvk 1, a4
 
chipRegTable:
chipReadAMR:
	bnop __end, 4
	mvc .s2 amr,b5
	

chipReadCSR:
	bnop __end, 4
	mvc .s2 csr,b5
	

chipReadIFR:
chipReadISR:
	bnop __end, 4
	mvc .s2 ifr,b5
	
chipReadICR:
	bnop __end, 4
	mvc .s2 ifr,b5

chipReadIER:
	bnop __end, 4
	mvc .s2 ier,b5

chipReadISTP:
	bnop __end, 4
	mvc .s2 istp,b5
	

chipReadIRP:
	bnop __end, 4
	mvc .s2 irp,b5

chipReadNRP:
chipReadERP:
	bnop __end, 4
	mvc .s2 nrp,b5

chipReadNone0:
	bnop __end, 4
 	mpy 1,a4,a4

chipReadNone1:
	bnop __end, 4
	mpy 1,a4,a4
	
chipReadTSCL:
	bnop __end, 4
	mvc  tscl,b5

chipReadTSCH:
	bnop __end, 4
	mvc  tsch,b5
	
chipReadARP:
	bnop __end, 4
	mvc .s2 arp,b5

chipReadILC:
	bnop __end, 4
	mvc .s2 ilc,b5

chipReadRILC:
	bnop __end, 4
	mvc .s2 rilc,b5

chipReadNone2:
	bnop __end, 4
	mpy 1,a4,a4
	
chipReadPCE1:	
	bnop __end, 4
	mvc .s2 pce1,b5
	
chipReadDNUM:
	bnop __end, 4
	mvc .s2 dnum,b5
	
chipReadNone3:
	bnop __end, 4
	mpy 1,a4,a4
	
chipReadNone4:
	
	bnop __end, 4
	mpy 1,a4,a4
	

chipReadNone5:
	bnop __end, 4
	mpy 1,a4,a4

chipReadSSR:
	bnop __end, 4
	mvc .s2 ssr,b5


chipReadGPLYA:	
	bnop __end, 4
	mvc .s2 gplya,b5
	

chipReadGPLYB:
	bnop __end, 4	
	mvc .s2 gplyb,b5
	

chipReadGFPGFR:
	bnop __end, 4
	mvc .s2 gfpgfr,b5
	

chipReadDIER:
	bnop __end, 4
	mvc .s2 dier,b5
	

chipReadTSR:	
	bnop __end, 4
	mvc .s2 tsr,b5
	

chipReadITSR:
	bnop __end, 4	
	mvc .s2 itsr,b5
	

chipReadNTSR:
chipReadETSR:
	bnop __end, 4
	mvc .s2 ntsr,b5

chipReadEFR:
chipReadECR:
	bnop __end, 4
	mvc .s2 efr,b5
	
chipReadNone6:
	bnop __end, 4
	mpy 1,a4,a4

chipReadIERR:
	bnop __end, 4
	mvc .s2 ierr,b5

__end:
	b b3    
	mv .s1 b5,a4
	nop 4
; Rev.No.   Date/Time               ECN No.          Modifier        ;
; -------   ---------               -------          --------        ;

; 1          Aug 11:10:46 9         2273             xkeshavm        ;
;                                                                    ;
; To upload API CSL 0.51 Release                                     ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 
