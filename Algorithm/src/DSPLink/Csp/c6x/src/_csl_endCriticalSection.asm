
	.asg	A15, FP
	.asg	B14, DP
	.asg	B15, SP
	.global	$bss


	.sect	".text"
	.global	__CSL_endCriticalSection

;******************************************************************************
;* FUNCTION NAME: __CSL_endCriticalSection                                    *
;*                                                                            *
;*   Regs Modified     : B4                                                   *
;*   Regs Used         : B3,B4,DP,SP                                          *
;*   Local Frame Size  : 0 Args + 0 Auto + 0 Save = 0 byte                    *
;******************************************************************************
__CSL_endCriticalSection:
;** --------------------------------------------------------------------------*
           MVC     .S2     CSR,B4            ; |5| 
           OR      .D2     1,B4,B4           ; |5| 
           MVC     .S2     B4,CSR            ; |5| 
           NOP             1
           RETNOP  .S2     B3,5              ; |6| 
           ; BRANCH OCCURS                   ; |6| 
; Rev.No.   Date/Time               ECN No.          Modifier        ;
; -------   ---------               -------          --------        ;

; 1          Aug 11:10:43 9         2273             xkeshavm        ;
;                                                                    ;
; To upload API CSL 0.51 Release                                     ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 
