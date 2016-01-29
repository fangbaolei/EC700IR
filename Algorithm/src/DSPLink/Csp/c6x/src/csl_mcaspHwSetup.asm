;******************************************************************************
;* TMS320C6x C/C++ Codegen                                          PC v6.0.1 *
;* Date/Time created: Tue Mar 07 22:08:44 2006                                *
;******************************************************************************
	.compiler_opts --endian=big --hll_source=on --mem_model:code=near --mem_model:data=far_aggregates --quiet --silicon_version=6500 --symdebug:dwarf 

;******************************************************************************
;* GLOBAL FILE PARAMETERS                                                     *
;*                                                                            *
;*   Architecture      : TMS320C64x+                                          *
;*   Optimization      : Disabled                                             *
;*   Optimizing for    : Compile time, Ease of Development                    *
;*                       Based on options: no -o, no -ms                      *
;*   Endian            : Big                                                  *
;*   Interrupt Thrshld : Disabled                                             *
;*   Data Access Model : Far Aggregate Data                                   *
;*   Pipelining        : Disabled                                             *
;*   Memory Aliases    : Presume are aliases (pessimistic)                    *
;*   Debug Info        : DWARF Debug                                          *
;*                                                                            *
;******************************************************************************

	.asg	A15, FP
	.asg	B14, DP
	.asg	B15, SP
	.global	$bss


$C$DW$CU	.dwtag  DW_TAG_compile_unit
	.dwattr $C$DW$CU, DW_AT_name("csl_mcaspHwSetup.c")
	.dwattr $C$DW$CU, DW_AT_producer("TMS320C6x C/C++ Codegen PC v6.0.1 Copyright (c) 1996-2005 Texas Instruments Incorporated")
	.dwattr $C$DW$CU, DW_AT_TI_version(0x01)
;	C:\CCStudio_v3.2\C6000\cgtools\bin\acp6x.exe -@C:\DOCUME~1\a0322216.ENT\LOCALS~1\Temp\0446414 
	.sect	".text:csl_section:mcasp:_CSL_mcaspHwSetup"
	.clink
	.global	_CSL_mcaspHwSetup

$C$DW$1	.dwtag  DW_TAG_subprogram, DW_AT_name("CSL_mcaspHwSetup")
	.dwattr $C$DW$1, DW_AT_low_pc(_CSL_mcaspHwSetup)
	.dwattr $C$DW$1, DW_AT_high_pc(0x00)
	.dwattr $C$DW$1, DW_AT_TI_symbol_name("_CSL_mcaspHwSetup")
	.dwattr $C$DW$1, DW_AT_external
	.dwattr $C$DW$1, DW_AT_type(*$C$DW$T$164)
	.dwattr $C$DW$1, DW_AT_TI_begin_file("csl_mcaspHwSetup.c")
	.dwattr $C$DW$1, DW_AT_TI_begin_line(0x64)
	.dwattr $C$DW$1, DW_AT_TI_begin_column(0x0c)
	.dwpsn	file "csl_mcaspHwSetup.c",line 104,column 1,is_stmt,address _CSL_mcaspHwSetup

	.dwfde $C$DW$CIE, _CSL_mcaspHwSetup

;******************************************************************************
;* FUNCTION NAME: CSL_mcaspHwSetup                                            *
;*                                                                            *
;*   Regs Modified     : A3,A4,A5,B0,B4,B5,B6,SP,A24,A25,A26,A27,A28,A29,A30, *
;*                           A31,B31                                          *
;*   Regs Used         : A3,A4,A5,B0,B3,B4,B5,B6,SP,A24,A25,A26,A27,A28,A29,  *
;*                           A30,A31,B31                                      *
;*   Local Frame Size  : 0 Args + 12 Auto + 0 Save = 12 byte                  *
;******************************************************************************
_CSL_mcaspHwSetup:
;** --------------------------------------------------------------------------*
	.dwcfi	cfa_offset, 0
	.dwcfi	save_reg_to_reg, 228, 19
           SUB     .D2     SP,16,SP          ; |104| 
	.dwcfi	cfa_offset, 16
$C$DW$2	.dwtag  DW_TAG_formal_parameter, DW_AT_name("hMcasp")
	.dwattr $C$DW$2, DW_AT_TI_symbol_name("_hMcasp")
	.dwattr $C$DW$2, DW_AT_type(*$C$DW$T$166)
	.dwattr $C$DW$2, DW_AT_location[DW_OP_reg4]
$C$DW$3	.dwtag  DW_TAG_formal_parameter, DW_AT_name("myHwSetup")
	.dwattr $C$DW$3, DW_AT_TI_symbol_name("_myHwSetup")
	.dwattr $C$DW$3, DW_AT_type(*$C$DW$T$168)
	.dwattr $C$DW$3, DW_AT_location[DW_OP_reg20]
$C$DW$4	.dwtag  DW_TAG_variable, DW_AT_name("hMcasp")
	.dwattr $C$DW$4, DW_AT_TI_symbol_name("_hMcasp")
	.dwattr $C$DW$4, DW_AT_type(*$C$DW$T$166)
	.dwattr $C$DW$4, DW_AT_location[DW_OP_breg31 4]
$C$DW$5	.dwtag  DW_TAG_variable, DW_AT_name("myHwSetup")
	.dwattr $C$DW$5, DW_AT_TI_symbol_name("_myHwSetup")
	.dwattr $C$DW$5, DW_AT_type(*$C$DW$T$168)
	.dwattr $C$DW$5, DW_AT_location[DW_OP_breg31 8]
$C$DW$6	.dwtag  DW_TAG_variable, DW_AT_name("serNum")
	.dwattr $C$DW$6, DW_AT_TI_symbol_name("_serNum")
	.dwattr $C$DW$6, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$6, DW_AT_location[DW_OP_breg31 12]
           STW     .D2T2   B4,*+SP(8)        ; |104| 
           STW     .D2T1   A4,*+SP(4)        ; |104| 
	.dwpsn	file "csl_mcaspHwSetup.c",line 105,column 12,is_stmt
           NOP             2
           ZERO    .L2     B4                ; |105| 
           STW     .D2T2   B4,*+SP(12)       ; |105| 
	.dwpsn	file "csl_mcaspHwSetup.c",line 107,column 5,is_stmt
           NOP             2
           MV      .L2X    A4,B0
   [ B0]   BNOP    .S1     $C$L1,5           ; |107| 
           ; BRANCHCC OCCURS {$C$L1}         ; |107| 
;** --------------------------------------------------------------------------*
	.dwpsn	file "csl_mcaspHwSetup.c",line 108,column 9,is_stmt
           BNOP    .S1     $C$L6,4           ; |108| 
           MVK     .L1     0xfffffffb,A4     ; |108| 
           ; BRANCH OCCURS {$C$L6}           ; |108| 
;** --------------------------------------------------------------------------*
$C$L1:    
	.dwpsn	file "csl_mcaspHwSetup.c",line 111,column 5,is_stmt
           LDW     .D2T2   *+SP(8),B0        ; |111| 
           NOP             4
   [ B0]   BNOP    .S1     $C$L2,5           ; |111| 
           ; BRANCHCC OCCURS {$C$L2}         ; |111| 
;** --------------------------------------------------------------------------*
	.dwpsn	file "csl_mcaspHwSetup.c",line 112,column 9,is_stmt
           BNOP    .S1     $C$L6,4           ; |112| 
           MVK     .L1     0xfffffffa,A4     ; |112| 
           ; BRANCH OCCURS {$C$L6}           ; |112| 
;** --------------------------------------------------------------------------*
$C$L2:    
	.dwpsn	file "csl_mcaspHwSetup.c",line 116,column 5,is_stmt
           MV      .L1     A4,A3
           LDW     .D1T1   *A3,A3            ; |116| 
           MV      .L1X    B4,A4             ; |116| 
           NOP             3
           STW     .D1T1   A4,*+A3(68)       ; |116| 
	.dwpsn	file "csl_mcaspHwSetup.c",line 119,column 5,is_stmt
           NOP             2
           LDW     .D2T2   *+SP(4),B4        ; |119| 
           LDW     .D2T1   *+SP(8),A3        ; |119| 
           LDW     .D2T1   *+SP(4),A4        ; |119| 
           MVK     .S1     42,A5             ; |119| 
           NOP             1
           LDW     .D2T2   *B4,B4            ; |119| 
           LDW     .D1T1   *+A3[A5],A3       ; |119| 
           LDW     .D1T1   *A4,A4            ; |119| 
           NOP             2
           LDW     .D2T2   *+B4(4),B4        ; |119| 
           AND     .L1     1,A3,A3           ; |119| 
           NOP             3
           AND     .L2     -2,B4,B4          ; |119| 
           NOP             1
           OR      .L1X    A3,B4,A3          ; |119| 
           STW     .D1T1   A3,*+A4(4)        ; |119| 
	.dwpsn	file "csl_mcaspHwSetup.c",line 122,column 5,is_stmt
           NOP             2
           LDW     .D2T1   *+SP(8),A3        ; |122| 
           LDW     .D2T1   *+SP(4),A4        ; |122| 
           NOP             3
           LDW     .D1T1   *+A3(88),A3       ; |122| 
           LDW     .D1T1   *A4,A4            ; |122| 
           NOP             4
           STW     .D1T1   A3,*+A4(100)      ; |122| 
	.dwpsn	file "csl_mcaspHwSetup.c",line 125,column 5,is_stmt
           NOP             2
           LDW     .D2T1   *+SP(4),A4        ; |125| 
           LDW     .D2T1   *+SP(8),A3        ; |125| 
           NOP             3
           LDW     .D1T1   *A4,A4            ; |125| 
           LDW     .D1T1   *+A3(92),A3       ; |125| 
           NOP             4
           STW     .D1T1   A3,*+A4(104)      ; |125| 
	.dwpsn	file "csl_mcaspHwSetup.c",line 128,column 5,is_stmt
           NOP             2
           LDW     .D2T1   *+SP(4),A4        ; |128| 
           LDW     .D2T1   *+SP(8),A3        ; |128| 
           NOP             3
           LDW     .D1T1   *A4,A4            ; |128| 
           LDW     .D1T1   *+A3(96),A3       ; |128| 
           NOP             4
           STW     .D1T1   A3,*+A4(108)      ; |128| 
	.dwpsn	file "csl_mcaspHwSetup.c",line 131,column 5,is_stmt
           NOP             2
           LDW     .D2T1   *+SP(4),A4        ; |131| 
           LDW     .D2T1   *+SP(8),A3        ; |131| 
           NOP             3
           LDW     .D1T1   *A4,A4            ; |131| 
           LDW     .D1T1   *+A3(116),A3      ; |131| 
           NOP             4
           STW     .D1T1   A3,*+A4(112)      ; |131| 
	.dwpsn	file "csl_mcaspHwSetup.c",line 134,column 5,is_stmt
           NOP             2
           LDW     .D2T1   *+SP(4),A4        ; |134| 
           LDW     .D2T1   *+SP(8),A3        ; |134| 
           NOP             3
           LDW     .D1T1   *A4,A4            ; |134| 
           LDW     .D1T1   *+A3(120),A3      ; |134| 
           NOP             4
           STW     .D1T1   A3,*+A4(116)      ; |134| 
	.dwpsn	file "csl_mcaspHwSetup.c",line 137,column 5,is_stmt
           NOP             2
           LDW     .D2T1   *+SP(4),A4        ; |137| 
           LDW     .D2T1   *+SP(8),A3        ; |137| 
           NOP             3
           LDW     .D1T1   *A4,A4            ; |137| 
           LDW     .D1T1   *+A3(100),A3      ; |137| 
           NOP             4
           STW     .D1T1   A3,*+A4(120)      ; |137| 
	.dwpsn	file "csl_mcaspHwSetup.c",line 140,column 5,is_stmt
           NOP             2
           LDW     .D2T1   *+SP(4),A4        ; |140| 
           LDW     .D2T1   *+SP(8),A3        ; |140| 
           NOP             3
           LDW     .D1T1   *A4,A4            ; |140| 
           LDW     .D1T1   *+A3(104),A3      ; |140| 
           NOP             4
           STW     .D1T1   A3,*+A4(124)      ; |140| 
	.dwpsn	file "csl_mcaspHwSetup.c",line 143,column 5,is_stmt
           NOP             2
           LDW     .D2T1   *+SP(4),A4        ; |143| 
           LDW     .D2T1   *+SP(8),A3        ; |143| 
           MVK     .S1     34,A5             ; |143| 
           NOP             2
           LDW     .D1T1   *A4,A4            ; |143| 
           LDW     .D1T1   *+A3(124),A3      ; |143| 
           NOP             4
           STW     .D1T1   A3,*+A4[A5]       ; |143| 
;** --------------------------------------------------------------------------*
	.dwpsn	file "csl_mcaspHwSetup.c",line 146,column 5,is_stmt
           LDW     .D2T1   *+SP(4),A4        ; |146| 
           LDW     .D2T1   *+SP(8),A3        ; |146| 
           MVK     .S1     32,A5             ; |146| 
           MVK     .S1     41,A31            ; |146| 
           NOP             1
           LDW     .D1T1   *A4,A4            ; |146| 
           LDW     .D1T1   *+A3[A5],A3       ; |146| 
           NOP             4
           STW     .D1T1   A3,*+A4[A31]      ; |146| 
	.dwpsn	file "csl_mcaspHwSetup.c",line 149,column 5,is_stmt
           NOP             2
           LDW     .D2T1   *+SP(4),A4        ; |149| 
           LDW     .D2T1   *+SP(8),A3        ; |149| 
           MVK     .S1     33,A5             ; |149| 
           MVK     .S1     42,A30            ; |149| 
           NOP             1
           LDW     .D1T1   *A4,A4            ; |149| 
           LDW     .D1T1   *+A3[A5],A3       ; |149| 
           NOP             4
           STW     .D1T1   A3,*+A4[A30]      ; |149| 
	.dwpsn	file "csl_mcaspHwSetup.c",line 152,column 5,is_stmt
           NOP             2
           LDW     .D2T1   *+SP(4),A4        ; |152| 
           LDW     .D2T1   *+SP(8),A3        ; |152| 
           MVK     .S1     69,A5             ; |152| 
           MVK     .S1     43,A29            ; |152| 
           NOP             1
           LDW     .D1T1   *A4,A4            ; |152| 
           LDHU    .D1T1   *+A3[A5],A3       ; |152| 
           NOP             4
           STW     .D1T1   A3,*+A4[A29]      ; |152| 
	.dwpsn	file "csl_mcaspHwSetup.c",line 155,column 5,is_stmt
           NOP             2
           LDW     .D2T1   *+SP(4),A4        ; |155| 
           LDW     .D2T1   *+SP(8),A3        ; |155| 
           MVK     .S1     79,A5             ; |155| 
           MVK     .S1     44,A28            ; |155| 
           NOP             1
           LDW     .D1T1   *A4,A4            ; |155| 
           LDHU    .D1T1   *+A3[A5],A3       ; |155| 
           NOP             4
           STW     .D1T1   A3,*+A4[A28]      ; |155| 
	.dwpsn	file "csl_mcaspHwSetup.c",line 158,column 5,is_stmt
           NOP             2
           LDW     .D2T1   *+SP(4),A4        ; |158| 
           LDW     .D2T1   *+SP(8),A3        ; |158| 
           MVK     .S1     81,A5             ; |158| 
           MVK     .S1     45,A27            ; |158| 
           NOP             1
           LDW     .D1T1   *A4,A4            ; |158| 
           LDHU    .D1T1   *+A3[A5],A3       ; |158| 
           NOP             4
           STW     .D1T1   A3,*+A4[A27]      ; |158| 
	.dwpsn	file "csl_mcaspHwSetup.c",line 161,column 5,is_stmt
           NOP             2
           LDW     .D2T1   *+SP(4),A4        ; |161| 
           LDW     .D2T1   *+SP(8),A3        ; |161| 
           MVK     .S1     35,A5             ; |161| 
           MVK     .S1     46,A26            ; |161| 
           NOP             1
           LDW     .D1T1   *A4,A4            ; |161| 
           LDW     .D1T1   *+A3[A5],A3       ; |161| 
           NOP             4
           STW     .D1T1   A3,*+A4[A26]      ; |161| 
	.dwpsn	file "csl_mcaspHwSetup.c",line 164,column 5,is_stmt
           NOP             2
           LDW     .D2T1   *+SP(4),A4        ; |164| 
           LDW     .D2T1   *+SP(8),A3        ; |164| 
           MVK     .S1     73,A5             ; |164| 
           MVK     .S1     47,A25            ; |164| 
           NOP             1
           LDW     .D1T1   *A4,A4            ; |164| 
           LDHU    .D1T1   *+A3[A5],A3       ; |164| 
           NOP             4
           STW     .D1T1   A3,*+A4[A25]      ; |164| 
	.dwpsn	file "csl_mcaspHwSetup.c",line 167,column 5,is_stmt
           NOP             2
           LDW     .D2T1   *+SP(4),A4        ; |167| 
           LDW     .D2T1   *+SP(8),A3        ; |167| 
           MVK     .S1     41,A5             ; |167| 
           MVK     .S1     50,A24            ; |167| 
           NOP             1
           LDW     .D1T1   *A4,A4            ; |167| 
           LDW     .D1T1   *+A3[A5],A3       ; |167| 
           NOP             4
           STW     .D1T1   A3,*+A4[A24]      ; |167| 
	.dwpsn	file "csl_mcaspHwSetup.c",line 170,column 12,is_stmt
           NOP             2
           LDW     .D2T2   *+SP(4),B4        ; |170| 
           LDW     .D2T2   *+SP(12),B5       ; |170| 
           NOP             3
           LDW     .D2T2   *+B4(8),B4        ; |170| 
           NOP             4
           CMPLTU  .L2     B5,B4,B0          ; |170| 
   [!B0]   BNOP    .S1     $C$L4,5           ; |170| 
           ; BRANCHCC OCCURS {$C$L4}         ; |170| 
;*----------------------------------------------------------------------------*
;*   SOFTWARE PIPELINE INFORMATION
;*      Disqualified loop: Software pipelining disabled
;*----------------------------------------------------------------------------*
$C$L3:    
$C$DW$L$_CSL_mcaspHwSetup$7$B:
	.dwpsn	file "csl_mcaspHwSetup.c",line 171,column 9,is_stmt
           LDW     .D2T2   *+SP(12),B5       ; |171| 
           LDW     .D2T2   *+SP(8),B6        ; |171| 
           LDW     .D2T2   *+SP(4),B4        ; |171| 
           MVK     .S2     96,B31            ; |171| 
           NOP             2
           ADDAW   .D2     B6,B5,B6          ; |171| 
           LDW     .D2T2   *B4,B5            ; |171| 
           LDHU    .D2T2   *+B6(26),B4       ; |171| 
           NOP             4
           STW     .D2T2   B4,*+B5[B31]      ; |171| 
	.dwpsn	file "csl_mcaspHwSetup.c",line 172,column 9,is_stmt
           NOP             2
           LDW     .D2T2   *+SP(12),B4       ; |172| 
           LDW     .D2T2   *+SP(4),B5        ; |172| 
           NOP             3
           ADD     .L2     1,B4,B4           ; |172| 
           STW     .D2T2   B4,*+SP(12)       ; |172| 
           LDW     .D2T2   *+B5(8),B5        ; |172| 
           NOP             4
           CMPLTU  .L2     B4,B5,B0          ; |172| 
   [!B0]   BNOP    .S1     $C$L4,5           ; |172| 
           ; BRANCHCC OCCURS {$C$L4}         ; |172| 
$C$DW$L$_CSL_mcaspHwSetup$7$E:
;** --------------------------------------------------------------------------*
$C$DW$L$_CSL_mcaspHwSetup$8$B:
	.dwpsn	file "csl_mcaspHwSetup.c",line 173,column 13,is_stmt
	.dwpsn	file "csl_mcaspHwSetup.c",line 174,column 9,is_stmt
           LDW     .D2T2   *+SP(12),B4       ; |174| 
           LDW     .D2T2   *+SP(8),B5        ; |174| 
           LDW     .D2T2   *+SP(4),B6        ; |174| 
           MVK     .S2     97,B31            ; |174| 
           NOP             2
           ADDAW   .D2     B5,B4,B4          ; |174| 
           LDHU    .D2T2   *+B4(26),B4       ; |174| 
           LDW     .D2T2   *B6,B5            ; |174| 
           NOP             4
           STW     .D2T2   B4,*+B5[B31]      ; |174| 
	.dwpsn	file "csl_mcaspHwSetup.c",line 175,column 9,is_stmt
           NOP             2
           LDW     .D2T2   *+SP(12),B4       ; |175| 
           LDW     .D2T2   *+SP(4),B5        ; |175| 
           NOP             3
           ADD     .L2     1,B4,B4           ; |175| 
           STW     .D2T2   B4,*+SP(12)       ; |175| 
           LDW     .D2T2   *+B5(8),B5        ; |175| 
           NOP             4
           CMPLTU  .L2     B4,B5,B0          ; |175| 
   [!B0]   BNOP    .S1     $C$L4,5           ; |175| 
           ; BRANCHCC OCCURS {$C$L4}         ; |175| 
$C$DW$L$_CSL_mcaspHwSetup$8$E:
;** --------------------------------------------------------------------------*
$C$DW$L$_CSL_mcaspHwSetup$9$B:
	.dwpsn	file "csl_mcaspHwSetup.c",line 176,column 13,is_stmt
	.dwpsn	file "csl_mcaspHwSetup.c",line 177,column 9,is_stmt
           LDW     .D2T2   *+SP(12),B4       ; |177| 
           LDW     .D2T2   *+SP(8),B5        ; |177| 
           LDW     .D2T2   *+SP(4),B6        ; |177| 
           MVK     .S2     98,B31            ; |177| 
           NOP             2
           ADDAW   .D2     B5,B4,B4          ; |177| 
           LDHU    .D2T2   *+B4(26),B4       ; |177| 
           LDW     .D2T2   *B6,B5            ; |177| 
           NOP             4
           STW     .D2T2   B4,*+B5[B31]      ; |177| 
	.dwpsn	file "csl_mcaspHwSetup.c",line 178,column 9,is_stmt
           NOP             2
           LDW     .D2T2   *+SP(12),B4       ; |178| 
           LDW     .D2T2   *+SP(4),B5        ; |178| 
           NOP             3
           ADD     .L2     1,B4,B4           ; |178| 
           STW     .D2T2   B4,*+SP(12)       ; |178| 
           LDW     .D2T2   *+B5(8),B5        ; |178| 
           NOP             4
           CMPLTU  .L2     B4,B5,B0          ; |178| 
   [!B0]   BNOP    .S1     $C$L4,5           ; |178| 
           ; BRANCHCC OCCURS {$C$L4}         ; |178| 
$C$DW$L$_CSL_mcaspHwSetup$9$E:
;** --------------------------------------------------------------------------*
$C$DW$L$_CSL_mcaspHwSetup$10$B:
	.dwpsn	file "csl_mcaspHwSetup.c",line 179,column 13,is_stmt
	.dwpsn	file "csl_mcaspHwSetup.c",line 180,column 9,is_stmt
           LDW     .D2T2   *+SP(12),B4       ; |180| 
           LDW     .D2T2   *+SP(8),B5        ; |180| 
           LDW     .D2T2   *+SP(4),B6        ; |180| 
           MVK     .S2     99,B31            ; |180| 
           NOP             2
           ADDAW   .D2     B5,B4,B4          ; |180| 
           LDHU    .D2T2   *+B4(26),B4       ; |180| 
           LDW     .D2T2   *B6,B5            ; |180| 
           NOP             4
           STW     .D2T2   B4,*+B5[B31]      ; |180| 
	.dwpsn	file "csl_mcaspHwSetup.c",line 181,column 9,is_stmt
           NOP             2
           LDW     .D2T2   *+SP(12),B4       ; |181| 
           LDW     .D2T2   *+SP(4),B5        ; |181| 
           NOP             3
           ADD     .L2     1,B4,B4           ; |181| 
           STW     .D2T2   B4,*+SP(12)       ; |181| 
           LDW     .D2T2   *+B5(8),B5        ; |181| 
           NOP             4
           CMPLTU  .L2     B4,B5,B0          ; |181| 
   [!B0]   BNOP    .S1     $C$L4,5           ; |181| 
           ; BRANCHCC OCCURS {$C$L4}         ; |181| 
$C$DW$L$_CSL_mcaspHwSetup$10$E:
;** --------------------------------------------------------------------------*
$C$DW$L$_CSL_mcaspHwSetup$11$B:
	.dwpsn	file "csl_mcaspHwSetup.c",line 182,column 13,is_stmt
	.dwpsn	file "csl_mcaspHwSetup.c",line 183,column 9,is_stmt
           LDW     .D2T2   *+SP(12),B4       ; |183| 
           LDW     .D2T2   *+SP(8),B5        ; |183| 
           LDW     .D2T2   *+SP(4),B6        ; |183| 
           MVK     .S2     100,B31           ; |183| 
           NOP             2
           ADDAW   .D2     B5,B4,B4          ; |183| 
           LDHU    .D2T2   *+B4(26),B4       ; |183| 
           LDW     .D2T2   *B6,B5            ; |183| 
           NOP             4
           STW     .D2T2   B4,*+B5[B31]      ; |183| 
	.dwpsn	file "csl_mcaspHwSetup.c",line 184,column 9,is_stmt
           NOP             2
           LDW     .D2T2   *+SP(12),B4       ; |184| 
           LDW     .D2T2   *+SP(4),B5        ; |184| 
           NOP             3
           ADD     .L2     1,B4,B4           ; |184| 
           STW     .D2T2   B4,*+SP(12)       ; |184| 
           LDW     .D2T2   *+B5(8),B5        ; |184| 
           NOP             4
           CMPLTU  .L2     B4,B5,B0          ; |184| 
   [!B0]   BNOP    .S1     $C$L4,5           ; |184| 
           ; BRANCHCC OCCURS {$C$L4}         ; |184| 
$C$DW$L$_CSL_mcaspHwSetup$11$E:
;** --------------------------------------------------------------------------*
$C$DW$L$_CSL_mcaspHwSetup$12$B:
	.dwpsn	file "csl_mcaspHwSetup.c",line 185,column 13,is_stmt
	.dwpsn	file "csl_mcaspHwSetup.c",line 186,column 9,is_stmt
           LDW     .D2T2   *+SP(12),B4       ; |186| 
           LDW     .D2T2   *+SP(8),B5        ; |186| 
           LDW     .D2T2   *+SP(4),B6        ; |186| 
           MVK     .S2     101,B31           ; |186| 
           NOP             2
           ADDAW   .D2     B5,B4,B4          ; |186| 
           LDHU    .D2T2   *+B4(26),B4       ; |186| 
           LDW     .D2T2   *B6,B5            ; |186| 
           NOP             4
           STW     .D2T2   B4,*+B5[B31]      ; |186| 
	.dwpsn	file "csl_mcaspHwSetup.c",line 187,column 9,is_stmt
           NOP             2
           LDW     .D2T2   *+SP(12),B4       ; |187| 
           LDW     .D2T2   *+SP(4),B5        ; |187| 
           NOP             3
           ADD     .L2     1,B4,B4           ; |187| 
           STW     .D2T2   B4,*+SP(12)       ; |187| 
           LDW     .D2T2   *+B5(8),B5        ; |187| 
           NOP             4
           CMPLTU  .L2     B4,B5,B0          ; |187| 
   [!B0]   BNOP    .S1     $C$L4,5           ; |187| 
           ; BRANCHCC OCCURS {$C$L4}         ; |187| 
$C$DW$L$_CSL_mcaspHwSetup$12$E:
;** --------------------------------------------------------------------------*
$C$DW$L$_CSL_mcaspHwSetup$13$B:
	.dwpsn	file "csl_mcaspHwSetup.c",line 188,column 13,is_stmt
	.dwpsn	file "csl_mcaspHwSetup.c",line 189,column 9,is_stmt
           LDW     .D2T2   *+SP(12),B4       ; |189| 
           LDW     .D2T2   *+SP(8),B5        ; |189| 
           LDW     .D2T2   *+SP(4),B6        ; |189| 
           MVK     .S2     102,B31           ; |189| 
           NOP             2
           ADDAW   .D2     B5,B4,B4          ; |189| 
           LDHU    .D2T2   *+B4(26),B4       ; |189| 
           LDW     .D2T2   *B6,B5            ; |189| 
           NOP             4
           STW     .D2T2   B4,*+B5[B31]      ; |189| 
	.dwpsn	file "csl_mcaspHwSetup.c",line 190,column 9,is_stmt
           NOP             2
           LDW     .D2T2   *+SP(12),B4       ; |190| 
           LDW     .D2T2   *+SP(4),B5        ; |190| 
           NOP             3
           ADD     .L2     1,B4,B4           ; |190| 
           STW     .D2T2   B4,*+SP(12)       ; |190| 
           LDW     .D2T2   *+B5(8),B5        ; |190| 
           NOP             4
           CMPLTU  .L2     B4,B5,B0          ; |190| 
   [!B0]   BNOP    .S1     $C$L4,5           ; |190| 
           ; BRANCHCC OCCURS {$C$L4}         ; |190| 
$C$DW$L$_CSL_mcaspHwSetup$13$E:
;** --------------------------------------------------------------------------*
$C$DW$L$_CSL_mcaspHwSetup$14$B:
	.dwpsn	file "csl_mcaspHwSetup.c",line 191,column 13,is_stmt
	.dwpsn	file "csl_mcaspHwSetup.c",line 192,column 9,is_stmt
           LDW     .D2T2   *+SP(12),B4       ; |192| 
           LDW     .D2T2   *+SP(8),B5        ; |192| 
           LDW     .D2T2   *+SP(4),B6        ; |192| 
           MVK     .S2     103,B31           ; |192| 
           NOP             2
           ADDAW   .D2     B5,B4,B4          ; |192| 
           LDHU    .D2T2   *+B4(26),B4       ; |192| 
           LDW     .D2T2   *B6,B5            ; |192| 
           NOP             4
           STW     .D2T2   B4,*+B5[B31]      ; |192| 
	.dwpsn	file "csl_mcaspHwSetup.c",line 193,column 9,is_stmt
           NOP             2
           LDW     .D2T2   *+SP(12),B4       ; |193| 
           LDW     .D2T2   *+SP(4),B5        ; |193| 
           NOP             3
           ADD     .L2     1,B4,B4           ; |193| 
           STW     .D2T2   B4,*+SP(12)       ; |193| 
           LDW     .D2T2   *+B5(8),B5        ; |193| 
           NOP             4
           CMPLTU  .L2     B4,B5,B0          ; |193| 
   [!B0]   BNOP    .S1     $C$L4,5           ; |193| 
           ; BRANCHCC OCCURS {$C$L4}         ; |193| 
$C$DW$L$_CSL_mcaspHwSetup$14$E:
;** --------------------------------------------------------------------------*
$C$DW$L$_CSL_mcaspHwSetup$15$B:
	.dwpsn	file "csl_mcaspHwSetup.c",line 194,column 13,is_stmt
	.dwpsn	file "csl_mcaspHwSetup.c",line 195,column 9,is_stmt
           LDW     .D2T2   *+SP(12),B4       ; |195| 
           LDW     .D2T2   *+SP(8),B5        ; |195| 
           LDW     .D2T2   *+SP(4),B6        ; |195| 
           MVK     .S2     104,B31           ; |195| 
           NOP             2
           ADDAW   .D2     B5,B4,B4          ; |195| 
           LDHU    .D2T2   *+B4(26),B4       ; |195| 
           LDW     .D2T2   *B6,B5            ; |195| 
           NOP             4
           STW     .D2T2   B4,*+B5[B31]      ; |195| 
	.dwpsn	file "csl_mcaspHwSetup.c",line 196,column 9,is_stmt
           NOP             2
           LDW     .D2T2   *+SP(12),B4       ; |196| 
           LDW     .D2T2   *+SP(4),B5        ; |196| 
           NOP             3
           ADD     .L2     1,B4,B4           ; |196| 
           STW     .D2T2   B4,*+SP(12)       ; |196| 
           LDW     .D2T2   *+B5(8),B5        ; |196| 
           NOP             4
           CMPLTU  .L2     B4,B5,B0          ; |196| 
   [!B0]   BNOP    .S1     $C$L4,5           ; |196| 
           ; BRANCHCC OCCURS {$C$L4}         ; |196| 
$C$DW$L$_CSL_mcaspHwSetup$15$E:
;** --------------------------------------------------------------------------*
$C$DW$L$_CSL_mcaspHwSetup$16$B:
	.dwpsn	file "csl_mcaspHwSetup.c",line 197,column 13,is_stmt
	.dwpsn	file "csl_mcaspHwSetup.c",line 198,column 9,is_stmt
           LDW     .D2T2   *+SP(12),B4       ; |198| 
           LDW     .D2T2   *+SP(8),B5        ; |198| 
           LDW     .D2T2   *+SP(4),B6        ; |198| 
           MVK     .S2     105,B31           ; |198| 
           NOP             2
           ADDAW   .D2     B5,B4,B4          ; |198| 
           LDHU    .D2T2   *+B4(26),B4       ; |198| 
           LDW     .D2T2   *B6,B5            ; |198| 
           NOP             4
           STW     .D2T2   B4,*+B5[B31]      ; |198| 
	.dwpsn	file "csl_mcaspHwSetup.c",line 199,column 9,is_stmt
           NOP             2
           LDW     .D2T2   *+SP(12),B4       ; |199| 
           LDW     .D2T2   *+SP(4),B5        ; |199| 
           NOP             3
           ADD     .L2     1,B4,B4           ; |199| 
           STW     .D2T2   B4,*+SP(12)       ; |199| 
           LDW     .D2T2   *+B5(8),B5        ; |199| 
           NOP             4
           CMPLTU  .L2     B4,B5,B0          ; |199| 
   [!B0]   BNOP    .S1     $C$L4,5           ; |199| 
           ; BRANCHCC OCCURS {$C$L4}         ; |199| 
$C$DW$L$_CSL_mcaspHwSetup$16$E:
;** --------------------------------------------------------------------------*
$C$DW$L$_CSL_mcaspHwSetup$17$B:
	.dwpsn	file "csl_mcaspHwSetup.c",line 200,column 13,is_stmt
	.dwpsn	file "csl_mcaspHwSetup.c",line 201,column 9,is_stmt
           LDW     .D2T2   *+SP(12),B4       ; |201| 
           LDW     .D2T2   *+SP(8),B5        ; |201| 
           LDW     .D2T2   *+SP(4),B6        ; |201| 
           MVK     .S2     106,B31           ; |201| 
           NOP             2
           ADDAW   .D2     B5,B4,B4          ; |201| 
           LDHU    .D2T2   *+B4(26),B4       ; |201| 
           LDW     .D2T2   *B6,B5            ; |201| 
           NOP             4
           STW     .D2T2   B4,*+B5[B31]      ; |201| 
	.dwpsn	file "csl_mcaspHwSetup.c",line 202,column 9,is_stmt
           NOP             2
           LDW     .D2T2   *+SP(12),B4       ; |202| 
           LDW     .D2T2   *+SP(4),B5        ; |202| 
           NOP             3
           ADD     .L2     1,B4,B4           ; |202| 
           STW     .D2T2   B4,*+SP(12)       ; |202| 
           LDW     .D2T2   *+B5(8),B5        ; |202| 
           NOP             4
           CMPLTU  .L2     B4,B5,B0          ; |202| 
   [!B0]   BNOP    .S1     $C$L4,5           ; |202| 
           ; BRANCHCC OCCURS {$C$L4}         ; |202| 
$C$DW$L$_CSL_mcaspHwSetup$17$E:
;** --------------------------------------------------------------------------*
$C$DW$L$_CSL_mcaspHwSetup$18$B:
	.dwpsn	file "csl_mcaspHwSetup.c",line 203,column 13,is_stmt
	.dwpsn	file "csl_mcaspHwSetup.c",line 204,column 9,is_stmt
           LDW     .D2T2   *+SP(12),B4       ; |204| 
           LDW     .D2T2   *+SP(8),B5        ; |204| 
           LDW     .D2T2   *+SP(4),B6        ; |204| 
           MVK     .S2     107,B31           ; |204| 
           NOP             2
           ADDAW   .D2     B5,B4,B4          ; |204| 
           LDHU    .D2T2   *+B4(26),B4       ; |204| 
           LDW     .D2T2   *B6,B5            ; |204| 
           NOP             4
           STW     .D2T2   B4,*+B5[B31]      ; |204| 
	.dwpsn	file "csl_mcaspHwSetup.c",line 205,column 9,is_stmt
           NOP             2
           LDW     .D2T2   *+SP(12),B4       ; |205| 
           LDW     .D2T2   *+SP(4),B5        ; |205| 
           NOP             3
           ADD     .L2     1,B4,B4           ; |205| 
           STW     .D2T2   B4,*+SP(12)       ; |205| 
           LDW     .D2T2   *+B5(8),B5        ; |205| 
           NOP             4
           CMPLTU  .L2     B4,B5,B0          ; |205| 
   [!B0]   BNOP    .S1     $C$L4,5           ; |205| 
           ; BRANCHCC OCCURS {$C$L4}         ; |205| 
$C$DW$L$_CSL_mcaspHwSetup$18$E:
;** --------------------------------------------------------------------------*
$C$DW$L$_CSL_mcaspHwSetup$19$B:
	.dwpsn	file "csl_mcaspHwSetup.c",line 206,column 13,is_stmt
	.dwpsn	file "csl_mcaspHwSetup.c",line 207,column 9,is_stmt
           LDW     .D2T2   *+SP(12),B4       ; |207| 
           LDW     .D2T2   *+SP(8),B5        ; |207| 
           LDW     .D2T2   *+SP(4),B6        ; |207| 
           MVK     .S2     108,B31           ; |207| 
           NOP             2
           ADDAW   .D2     B5,B4,B4          ; |207| 
           LDHU    .D2T2   *+B4(26),B4       ; |207| 
           LDW     .D2T2   *B6,B5            ; |207| 
           NOP             4
           STW     .D2T2   B4,*+B5[B31]      ; |207| 
	.dwpsn	file "csl_mcaspHwSetup.c",line 208,column 9,is_stmt
           NOP             2
           LDW     .D2T2   *+SP(12),B4       ; |208| 
           LDW     .D2T2   *+SP(4),B5        ; |208| 
           NOP             3
           ADD     .L2     1,B4,B4           ; |208| 
           STW     .D2T2   B4,*+SP(12)       ; |208| 
           LDW     .D2T2   *+B5(8),B5        ; |208| 
           NOP             4
           CMPLTU  .L2     B4,B5,B0          ; |208| 
   [!B0]   BNOP    .S1     $C$L4,5           ; |208| 
           ; BRANCHCC OCCURS {$C$L4}         ; |208| 
$C$DW$L$_CSL_mcaspHwSetup$19$E:
;** --------------------------------------------------------------------------*
$C$DW$L$_CSL_mcaspHwSetup$20$B:
	.dwpsn	file "csl_mcaspHwSetup.c",line 209,column 13,is_stmt
	.dwpsn	file "csl_mcaspHwSetup.c",line 210,column 9,is_stmt
           LDW     .D2T2   *+SP(12),B4       ; |210| 
           LDW     .D2T2   *+SP(8),B5        ; |210| 
           LDW     .D2T2   *+SP(4),B6        ; |210| 
           MVK     .S2     109,B31           ; |210| 
           NOP             2
           ADDAW   .D2     B5,B4,B4          ; |210| 
           LDHU    .D2T2   *+B4(26),B4       ; |210| 
           LDW     .D2T2   *B6,B5            ; |210| 
           NOP             4
           STW     .D2T2   B4,*+B5[B31]      ; |210| 
	.dwpsn	file "csl_mcaspHwSetup.c",line 211,column 9,is_stmt
           NOP             2
           LDW     .D2T2   *+SP(12),B4       ; |211| 
           LDW     .D2T2   *+SP(4),B5        ; |211| 
           NOP             3
           ADD     .L2     1,B4,B4           ; |211| 
           STW     .D2T2   B4,*+SP(12)       ; |211| 
           LDW     .D2T2   *+B5(8),B5        ; |211| 
           NOP             4
           CMPLTU  .L2     B4,B5,B0          ; |211| 
   [!B0]   BNOP    .S1     $C$L4,5           ; |211| 
           ; BRANCHCC OCCURS {$C$L4}         ; |211| 
$C$DW$L$_CSL_mcaspHwSetup$20$E:
;** --------------------------------------------------------------------------*
$C$DW$L$_CSL_mcaspHwSetup$21$B:
	.dwpsn	file "csl_mcaspHwSetup.c",line 212,column 13,is_stmt
	.dwpsn	file "csl_mcaspHwSetup.c",line 213,column 9,is_stmt
           LDW     .D2T2   *+SP(12),B4       ; |213| 
           LDW     .D2T2   *+SP(8),B5        ; |213| 
           LDW     .D2T2   *+SP(4),B6        ; |213| 
           MVK     .S2     110,B31           ; |213| 
           NOP             2
           ADDAW   .D2     B5,B4,B4          ; |213| 
           LDHU    .D2T2   *+B4(26),B4       ; |213| 
           LDW     .D2T2   *B6,B5            ; |213| 
           NOP             4
           STW     .D2T2   B4,*+B5[B31]      ; |213| 
	.dwpsn	file "csl_mcaspHwSetup.c",line 214,column 9,is_stmt
           NOP             2
           LDW     .D2T2   *+SP(12),B4       ; |214| 
           LDW     .D2T2   *+SP(4),B5        ; |214| 
           NOP             3
           ADD     .L2     1,B4,B4           ; |214| 
           STW     .D2T2   B4,*+SP(12)       ; |214| 
           LDW     .D2T2   *+B5(8),B5        ; |214| 
           NOP             4
           CMPLTU  .L2     B4,B5,B0          ; |214| 
   [!B0]   BNOP    .S1     $C$L4,5           ; |214| 
           ; BRANCHCC OCCURS {$C$L4}         ; |214| 
$C$DW$L$_CSL_mcaspHwSetup$21$E:
;** --------------------------------------------------------------------------*
$C$DW$L$_CSL_mcaspHwSetup$22$B:
	.dwpsn	file "csl_mcaspHwSetup.c",line 215,column 13,is_stmt
	.dwpsn	file "csl_mcaspHwSetup.c",line 216,column 9,is_stmt
           LDW     .D2T2   *+SP(12),B4       ; |216| 
           LDW     .D2T2   *+SP(8),B5        ; |216| 
           LDW     .D2T2   *+SP(4),B6        ; |216| 
           MVK     .S2     111,B31           ; |216| 
           NOP             2
           ADDAW   .D2     B5,B4,B4          ; |216| 
           LDHU    .D2T2   *+B4(26),B4       ; |216| 
           LDW     .D2T2   *B6,B5            ; |216| 
           NOP             4
           STW     .D2T2   B4,*+B5[B31]      ; |216| 
	.dwpsn	file "csl_mcaspHwSetup.c",line 217,column 9,is_stmt
           NOP             2
           LDW     .D2T2   *+SP(12),B4       ; |217| 
           LDW     .D2T2   *+SP(4),B5        ; |217| 
           NOP             3
           ADD     .L2     1,B4,B4           ; |217| 
           STW     .D2T2   B4,*+SP(12)       ; |217| 
           LDW     .D2T2   *+B5(8),B5        ; |217| 
           NOP             4
           CMPLTU  .L2     B4,B5,B0          ; |217| 
   [!B0]   BNOP    .S1     $C$L4,5           ; |217| 
           ; BRANCHCC OCCURS {$C$L4}         ; |217| 
$C$DW$L$_CSL_mcaspHwSetup$22$E:
;** --------------------------------------------------------------------------*
$C$DW$L$_CSL_mcaspHwSetup$23$B:
	.dwpsn	file "csl_mcaspHwSetup.c",line 218,column 13,is_stmt
	.dwpsn	file "csl_mcaspHwSetup.c",line 170,column 12,is_stmt
           LDW     .D2T2   *+SP(4),B4        ; |170| 
           LDW     .D2T2   *+SP(12),B5       ; |170| 
           NOP             3
           LDW     .D2T2   *+B4(8),B4        ; |170| 
           NOP             4
           CMPLTU  .L2     B5,B4,B0          ; |170| 
   [ B0]   BNOP    .S1     $C$L3,5           ; |170| 
           ; BRANCHCC OCCURS {$C$L3}         ; |170| 
$C$DW$L$_CSL_mcaspHwSetup$23$E:
;** --------------------------------------------------------------------------*
$C$L4:    
	.dwpsn	file "csl_mcaspHwSetup.c",line 222,column 5,is_stmt
           LDW     .D2T1   *+SP(8),A3        ; |222| 
           LDW     .D2T1   *+SP(4),A4        ; |222| 
           NOP             3
           LDW     .D1T1   *A3,A3            ; |222| 
           LDW     .D1T1   *A4,A4            ; |222| 
           NOP             4
           STW     .D1T1   A3,*+A4(16)       ; |222| 
	.dwpsn	file "csl_mcaspHwSetup.c",line 225,column 5,is_stmt
           NOP             2
           LDW     .D2T1   *+SP(4),A4        ; |225| 
           LDW     .D2T1   *+SP(8),A3        ; |225| 
           NOP             3
           LDW     .D1T1   *A4,A4            ; |225| 
           LDW     .D1T1   *+A3(4),A3        ; |225| 
           NOP             4
           STW     .D1T1   A3,*+A4(20)       ; |225| 
	.dwpsn	file "csl_mcaspHwSetup.c",line 228,column 5,is_stmt
           NOP             2
           LDW     .D2T2   *+SP(4),B4        ; |228| 
           NOP             4
           LDHU    .D2T2   *+B4(12),B4       ; |228| 
           NOP             4
           CMPEQ   .L2     B4,1,B0           ; |228| 
   [!B0]   BNOP    .S1     $C$L5,5           ; |228| 
           ; BRANCHCC OCCURS {$C$L5}         ; |228| 
;** --------------------------------------------------------------------------*
	.dwpsn	file "csl_mcaspHwSetup.c",line 229,column 9,is_stmt
           LDW     .D2T1   *+SP(8),A3        ; |229| 
           LDW     .D2T2   *+SP(4),B4        ; |229| 
           NOP             4

           LDW     .D1T1   *+A3(12),A3       ; |229| 
||         LDW     .D2T2   *B4,B4            ; |229| 

           NOP             4
           STW     .D2T1   A3,*+B4(80)       ; |229| 
;** --------------------------------------------------------------------------*
$C$L5:    
	.dwpsn	file "csl_mcaspHwSetup.c",line 233,column 5,is_stmt
           LDW     .D2T1   *+SP(8),A3        ; |233| 
           LDW     .D2T1   *+SP(4),A4        ; |233| 
           NOP             3
           LDW     .D1T1   *+A3(16),A3       ; |233| 
           LDW     .D1T1   *A4,A4            ; |233| 
           NOP             4
           STW     .D1T1   A3,*+A4(76)       ; |233| 
	.dwpsn	file "csl_mcaspHwSetup.c",line 236,column 5,is_stmt
           NOP             2
           LDW     .D2T1   *+SP(4),A4        ; |236| 
           LDW     .D2T1   *+SP(8),A3        ; |236| 
           NOP             3
           LDW     .D1T1   *A4,A4            ; |236| 
           LDW     .D1T1   *+A3(20),A3       ; |236| 
           NOP             4
           STW     .D1T1   A3,*+A4(72)       ; |236| 
	.dwpsn	file "csl_mcaspHwSetup.c",line 239,column 5,is_stmt
           NOP             2
           LDW     .D2T1   *+SP(8),A4        ; |239| 
           LDW     .D2T1   *+SP(4),A3        ; |239| 
           MVK     .S1     32,A5             ; |239| 
           NOP             2
           LDW     .D1T1   *+A4(108),A4      ; |239| 
           LDW     .D1T1   *A3,A3            ; |239| 
           NOP             4
           STW     .D1T1   A4,*+A3[A5]       ; |239| 
	.dwpsn	file "csl_mcaspHwSetup.c",line 240,column 5,is_stmt
           NOP             2
           LDW     .D2T1   *+SP(4),A4        ; |240| 
           LDW     .D2T1   *+SP(8),A3        ; |240| 
           MVK     .S1     37,A5             ; |240| 
           MVK     .S1     48,A31            ; |240| 
           NOP             1
           LDW     .D1T1   *A4,A4            ; |240| 
           LDW     .D1T1   *+A3[A5],A3       ; |240| 
           NOP             4
           STW     .D1T1   A3,*+A4[A31]      ; |240| 
	.dwpsn	file "csl_mcaspHwSetup.c",line 243,column 5,is_stmt
           NOP             2
           LDW     .D2T1   *+SP(4),A4        ; |243| 
           LDW     .D2T1   *+SP(8),A3        ; |243| 
           MVK     .S1     35,A5             ; |243| 
           NOP             2
           LDW     .D1T1   *A4,A4            ; |243| 
           LDW     .D1T1   *+A3(112),A3      ; |243| 
           NOP             4
           STW     .D1T1   A3,*+A4[A5]       ; |243| 
	.dwpsn	file "csl_mcaspHwSetup.c",line 244,column 5,is_stmt
           NOP             2
           LDW     .D2T1   *+SP(4),A4        ; |244| 
           LDW     .D2T1   *+SP(8),A3        ; |244| 
           MVK     .S1     38,A5             ; |244| 
           MVK     .S1     51,A30            ; |244| 
           NOP             1
           LDW     .D1T1   *A4,A4            ; |244| 
           LDW     .D1T1   *+A3[A5],A3       ; |244| 
           NOP             4
           STW     .D1T1   A3,*+A4[A30]      ; |244| 
	.dwpsn	file "csl_mcaspHwSetup.c",line 247,column 5,is_stmt
           NOP             2
           LDW     .D2T2   *+SP(4),B4        ; |247| 
           LDW     .D2T1   *+SP(8),A3        ; |247| 
           NOP             4

           LDW     .D2T2   *B4,B4            ; |247| 
||         LDW     .D1T1   *+A3(8),A3        ; |247| 

           NOP             4
           STW     .D2T1   A3,*+B4(68)       ; |247| 
	.dwpsn	file "csl_mcaspHwSetup.c",line 249,column 5,is_stmt
           NOP             2
           MVK     .L1     0x1,A4            ; |249| 
;** --------------------------------------------------------------------------*
$C$L6:    
	.dwpsn	file "csl_mcaspHwSetup.c",line 250,column 1,is_stmt
           ADDK    .S2     16,SP             ; |250| 
	.dwcfi	cfa_offset, 0
	.dwcfi	cfa_offset, 0
           RETNOP  .S2     B3,5              ; |250| 
           ; BRANCH OCCURS {B3}              ; |250| 

$C$DW$7	.dwtag  DW_TAG_TI_loop
	.dwattr $C$DW$7, DW_AT_name("V:\c64lc_test_a0322216\cdb_dsp_c64lc\test\csl\c64lc_csl\src\csl_mcaspHwSetup.asm:$C$L3:1:1141790925")
	.dwattr $C$DW$7, DW_AT_TI_begin_file("csl_mcaspHwSetup.c")
	.dwattr $C$DW$7, DW_AT_TI_begin_line(0xaa)
	.dwattr $C$DW$7, DW_AT_TI_end_line(0xdb)
$C$DW$8	.dwtag  DW_TAG_TI_loop_range
	.dwattr $C$DW$8, DW_AT_low_pc($C$DW$L$_CSL_mcaspHwSetup$7$B)
	.dwattr $C$DW$8, DW_AT_high_pc($C$DW$L$_CSL_mcaspHwSetup$7$E)
$C$DW$9	.dwtag  DW_TAG_TI_loop_range
	.dwattr $C$DW$9, DW_AT_low_pc($C$DW$L$_CSL_mcaspHwSetup$8$B)
	.dwattr $C$DW$9, DW_AT_high_pc($C$DW$L$_CSL_mcaspHwSetup$8$E)
$C$DW$10	.dwtag  DW_TAG_TI_loop_range
	.dwattr $C$DW$10, DW_AT_low_pc($C$DW$L$_CSL_mcaspHwSetup$9$B)
	.dwattr $C$DW$10, DW_AT_high_pc($C$DW$L$_CSL_mcaspHwSetup$9$E)
$C$DW$11	.dwtag  DW_TAG_TI_loop_range
	.dwattr $C$DW$11, DW_AT_low_pc($C$DW$L$_CSL_mcaspHwSetup$10$B)
	.dwattr $C$DW$11, DW_AT_high_pc($C$DW$L$_CSL_mcaspHwSetup$10$E)
$C$DW$12	.dwtag  DW_TAG_TI_loop_range
	.dwattr $C$DW$12, DW_AT_low_pc($C$DW$L$_CSL_mcaspHwSetup$11$B)
	.dwattr $C$DW$12, DW_AT_high_pc($C$DW$L$_CSL_mcaspHwSetup$11$E)
$C$DW$13	.dwtag  DW_TAG_TI_loop_range
	.dwattr $C$DW$13, DW_AT_low_pc($C$DW$L$_CSL_mcaspHwSetup$12$B)
	.dwattr $C$DW$13, DW_AT_high_pc($C$DW$L$_CSL_mcaspHwSetup$12$E)
$C$DW$14	.dwtag  DW_TAG_TI_loop_range
	.dwattr $C$DW$14, DW_AT_low_pc($C$DW$L$_CSL_mcaspHwSetup$13$B)
	.dwattr $C$DW$14, DW_AT_high_pc($C$DW$L$_CSL_mcaspHwSetup$13$E)
$C$DW$15	.dwtag  DW_TAG_TI_loop_range
	.dwattr $C$DW$15, DW_AT_low_pc($C$DW$L$_CSL_mcaspHwSetup$14$B)
	.dwattr $C$DW$15, DW_AT_high_pc($C$DW$L$_CSL_mcaspHwSetup$14$E)
$C$DW$16	.dwtag  DW_TAG_TI_loop_range
	.dwattr $C$DW$16, DW_AT_low_pc($C$DW$L$_CSL_mcaspHwSetup$15$B)
	.dwattr $C$DW$16, DW_AT_high_pc($C$DW$L$_CSL_mcaspHwSetup$15$E)
$C$DW$17	.dwtag  DW_TAG_TI_loop_range
	.dwattr $C$DW$17, DW_AT_low_pc($C$DW$L$_CSL_mcaspHwSetup$16$B)
	.dwattr $C$DW$17, DW_AT_high_pc($C$DW$L$_CSL_mcaspHwSetup$16$E)
$C$DW$18	.dwtag  DW_TAG_TI_loop_range
	.dwattr $C$DW$18, DW_AT_low_pc($C$DW$L$_CSL_mcaspHwSetup$17$B)
	.dwattr $C$DW$18, DW_AT_high_pc($C$DW$L$_CSL_mcaspHwSetup$17$E)
$C$DW$19	.dwtag  DW_TAG_TI_loop_range
	.dwattr $C$DW$19, DW_AT_low_pc($C$DW$L$_CSL_mcaspHwSetup$18$B)
	.dwattr $C$DW$19, DW_AT_high_pc($C$DW$L$_CSL_mcaspHwSetup$18$E)
$C$DW$20	.dwtag  DW_TAG_TI_loop_range
	.dwattr $C$DW$20, DW_AT_low_pc($C$DW$L$_CSL_mcaspHwSetup$19$B)
	.dwattr $C$DW$20, DW_AT_high_pc($C$DW$L$_CSL_mcaspHwSetup$19$E)
$C$DW$21	.dwtag  DW_TAG_TI_loop_range
	.dwattr $C$DW$21, DW_AT_low_pc($C$DW$L$_CSL_mcaspHwSetup$20$B)
	.dwattr $C$DW$21, DW_AT_high_pc($C$DW$L$_CSL_mcaspHwSetup$20$E)
$C$DW$22	.dwtag  DW_TAG_TI_loop_range
	.dwattr $C$DW$22, DW_AT_low_pc($C$DW$L$_CSL_mcaspHwSetup$21$B)
	.dwattr $C$DW$22, DW_AT_high_pc($C$DW$L$_CSL_mcaspHwSetup$21$E)
$C$DW$23	.dwtag  DW_TAG_TI_loop_range
	.dwattr $C$DW$23, DW_AT_low_pc($C$DW$L$_CSL_mcaspHwSetup$22$B)
	.dwattr $C$DW$23, DW_AT_high_pc($C$DW$L$_CSL_mcaspHwSetup$22$E)
$C$DW$24	.dwtag  DW_TAG_TI_loop_range
	.dwattr $C$DW$24, DW_AT_low_pc($C$DW$L$_CSL_mcaspHwSetup$23$B)
	.dwattr $C$DW$24, DW_AT_high_pc($C$DW$L$_CSL_mcaspHwSetup$23$E)
	.dwendtag $C$DW$7

	.dwattr $C$DW$1, DW_AT_TI_end_file("csl_mcaspHwSetup.c")
	.dwattr $C$DW$1, DW_AT_TI_end_line(0xfa)
	.dwattr $C$DW$1, DW_AT_TI_end_column(0x01)
	.dwendentry
	.dwendtag $C$DW$1


;******************************************************************************
;* TYPE INFORMATION                                                           *
;******************************************************************************
$C$DW$T$3	.dwtag  DW_TAG_pointer_type
	.dwattr $C$DW$T$3, DW_AT_address_class(0x20)
$C$DW$T$162	.dwtag  DW_TAG_typedef, DW_AT_name("Ptr")
	.dwattr $C$DW$T$162, DW_AT_type(*$C$DW$T$3)
	.dwattr $C$DW$T$162, DW_AT_language(DW_LANG_C)
$C$DW$T$4	.dwtag  DW_TAG_base_type, DW_AT_name("bool")
	.dwattr $C$DW$T$4, DW_AT_encoding(DW_ATE_boolean)
	.dwattr $C$DW$T$4, DW_AT_byte_size(0x01)
$C$DW$T$5	.dwtag  DW_TAG_base_type, DW_AT_name("signed char")
	.dwattr $C$DW$T$5, DW_AT_encoding(DW_ATE_signed_char)
	.dwattr $C$DW$T$5, DW_AT_byte_size(0x01)
$C$DW$T$6	.dwtag  DW_TAG_base_type, DW_AT_name("unsigned char")
	.dwattr $C$DW$T$6, DW_AT_encoding(DW_ATE_unsigned_char)
	.dwattr $C$DW$T$6, DW_AT_byte_size(0x01)
$C$DW$T$47	.dwtag  DW_TAG_typedef, DW_AT_name("Uint8")
	.dwattr $C$DW$T$47, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$T$47, DW_AT_language(DW_LANG_C)
$C$DW$T$48	.dwtag  DW_TAG_volatile_type
	.dwattr $C$DW$T$48, DW_AT_type(*$C$DW$T$47)

$C$DW$T$49	.dwtag  DW_TAG_array_type, DW_AT_type(*$C$DW$T$48)
	.dwattr $C$DW$T$49, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$49, DW_AT_byte_size(0x02)
$C$DW$25	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$25, DW_AT_upper_bound(0x01)
	.dwendtag $C$DW$T$49


$C$DW$T$50	.dwtag  DW_TAG_array_type, DW_AT_type(*$C$DW$T$48)
	.dwattr $C$DW$T$50, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$50, DW_AT_byte_size(0x0e)
$C$DW$26	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$26, DW_AT_upper_bound(0x0d)
	.dwendtag $C$DW$T$50


$C$DW$T$53	.dwtag  DW_TAG_array_type, DW_AT_type(*$C$DW$T$48)
	.dwattr $C$DW$T$53, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$53, DW_AT_byte_size(0xdc)
$C$DW$27	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$27, DW_AT_upper_bound(0xdb)
	.dwendtag $C$DW$T$53


$C$DW$T$54	.dwtag  DW_TAG_array_type, DW_AT_type(*$C$DW$T$48)
	.dwattr $C$DW$T$54, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$54, DW_AT_byte_size(0x14)
$C$DW$28	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$28, DW_AT_upper_bound(0x13)
	.dwendtag $C$DW$T$54


$C$DW$T$55	.dwtag  DW_TAG_array_type, DW_AT_type(*$C$DW$T$48)
	.dwattr $C$DW$T$55, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$55, DW_AT_byte_size(0x04)
$C$DW$29	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$29, DW_AT_upper_bound(0x03)
	.dwendtag $C$DW$T$55


$C$DW$T$56	.dwtag  DW_TAG_array_type, DW_AT_type(*$C$DW$T$48)
	.dwattr $C$DW$T$56, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$56, DW_AT_byte_size(0x0c)
$C$DW$30	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$30, DW_AT_upper_bound(0x0b)
	.dwendtag $C$DW$T$56


$C$DW$T$62	.dwtag  DW_TAG_array_type, DW_AT_type(*$C$DW$T$48)
	.dwattr $C$DW$T$62, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$62, DW_AT_byte_size(0x08)
$C$DW$31	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$31, DW_AT_upper_bound(0x07)
	.dwendtag $C$DW$T$62


$C$DW$T$64	.dwtag  DW_TAG_array_type, DW_AT_type(*$C$DW$T$48)
	.dwattr $C$DW$T$64, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$64, DW_AT_byte_size(0x10)
$C$DW$32	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$32, DW_AT_upper_bound(0x0f)
	.dwendtag $C$DW$T$64


$C$DW$T$65	.dwtag  DW_TAG_array_type, DW_AT_type(*$C$DW$T$48)
	.dwattr $C$DW$T$65, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$65, DW_AT_byte_size(0x18)
$C$DW$33	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$33, DW_AT_upper_bound(0x17)
	.dwendtag $C$DW$T$65


$C$DW$T$68	.dwtag  DW_TAG_array_type, DW_AT_type(*$C$DW$T$48)
	.dwattr $C$DW$T$68, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$68, DW_AT_byte_size(0x30)
$C$DW$34	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$34, DW_AT_upper_bound(0x2f)
	.dwendtag $C$DW$T$68


$C$DW$T$69	.dwtag  DW_TAG_array_type, DW_AT_type(*$C$DW$T$48)
	.dwattr $C$DW$T$69, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$69, DW_AT_byte_size(0x1c)
$C$DW$35	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$35, DW_AT_upper_bound(0x1b)
	.dwendtag $C$DW$T$69


$C$DW$T$73	.dwtag  DW_TAG_array_type, DW_AT_type(*$C$DW$T$48)
	.dwattr $C$DW$T$73, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$73, DW_AT_byte_size(0x168)
$C$DW$36	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$36, DW_AT_upper_bound(0x167)
	.dwendtag $C$DW$T$73


$C$DW$T$76	.dwtag  DW_TAG_array_type, DW_AT_type(*$C$DW$T$48)
	.dwattr $C$DW$T$76, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$76, DW_AT_byte_size(0xf8)
$C$DW$37	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$37, DW_AT_upper_bound(0xf7)
	.dwendtag $C$DW$T$76


$C$DW$T$78	.dwtag  DW_TAG_array_type, DW_AT_type(*$C$DW$T$48)
	.dwattr $C$DW$T$78, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$78, DW_AT_byte_size(0x20)
$C$DW$38	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$38, DW_AT_upper_bound(0x1f)
	.dwendtag $C$DW$T$78


$C$DW$T$79	.dwtag  DW_TAG_array_type, DW_AT_type(*$C$DW$T$48)
	.dwattr $C$DW$T$79, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$79, DW_AT_byte_size(0x78)
$C$DW$39	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$39, DW_AT_upper_bound(0x77)
	.dwendtag $C$DW$T$79


$C$DW$T$82	.dwtag  DW_TAG_array_type, DW_AT_type(*$C$DW$T$48)
	.dwattr $C$DW$T$82, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$82, DW_AT_byte_size(0x60)
$C$DW$40	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$40, DW_AT_upper_bound(0x5f)
	.dwendtag $C$DW$T$82


$C$DW$T$86	.dwtag  DW_TAG_array_type, DW_AT_type(*$C$DW$T$48)
	.dwattr $C$DW$T$86, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$86, DW_AT_byte_size(0xbc)
$C$DW$41	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$41, DW_AT_upper_bound(0xbb)
	.dwendtag $C$DW$T$86


$C$DW$T$87	.dwtag  DW_TAG_array_type, DW_AT_type(*$C$DW$T$48)
	.dwattr $C$DW$T$87, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$87, DW_AT_byte_size(0xf4)
$C$DW$42	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$42, DW_AT_upper_bound(0xf3)
	.dwendtag $C$DW$T$87


$C$DW$T$88	.dwtag  DW_TAG_array_type, DW_AT_type(*$C$DW$T$48)
	.dwattr $C$DW$T$88, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$88, DW_AT_byte_size(0x7d0)
$C$DW$43	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$43, DW_AT_upper_bound(0x7cf)
	.dwendtag $C$DW$T$88


$C$DW$T$89	.dwtag  DW_TAG_array_type, DW_AT_type(*$C$DW$T$48)
	.dwattr $C$DW$T$89, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$89, DW_AT_byte_size(0xf68)
$C$DW$44	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$44, DW_AT_upper_bound(0xf67)
	.dwendtag $C$DW$T$89


$C$DW$T$92	.dwtag  DW_TAG_array_type, DW_AT_type(*$C$DW$T$48)
	.dwattr $C$DW$T$92, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$92, DW_AT_byte_size(0x1000)
$C$DW$45	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$45, DW_AT_upper_bound(0xfff)
	.dwendtag $C$DW$T$92


$C$DW$T$96	.dwtag  DW_TAG_array_type, DW_AT_type(*$C$DW$T$48)
	.dwattr $C$DW$T$96, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$96, DW_AT_byte_size(0x28)
$C$DW$46	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$46, DW_AT_upper_bound(0x27)
	.dwendtag $C$DW$T$96


$C$DW$T$98	.dwtag  DW_TAG_array_type, DW_AT_type(*$C$DW$T$48)
	.dwattr $C$DW$T$98, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$98, DW_AT_byte_size(0x74)
$C$DW$47	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$47, DW_AT_upper_bound(0x73)
	.dwendtag $C$DW$T$98


$C$DW$T$104	.dwtag  DW_TAG_array_type, DW_AT_type(*$C$DW$T$48)
	.dwattr $C$DW$T$104, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$104, DW_AT_byte_size(0x64)
$C$DW$48	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$48, DW_AT_upper_bound(0x63)
	.dwendtag $C$DW$T$104


$C$DW$T$105	.dwtag  DW_TAG_array_type, DW_AT_type(*$C$DW$T$48)
	.dwattr $C$DW$T$105, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$105, DW_AT_byte_size(0x40)
$C$DW$49	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$49, DW_AT_upper_bound(0x3f)
	.dwendtag $C$DW$T$105


$C$DW$T$106	.dwtag  DW_TAG_array_type, DW_AT_type(*$C$DW$T$48)
	.dwattr $C$DW$T$106, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$106, DW_AT_byte_size(0x58)
$C$DW$50	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$50, DW_AT_upper_bound(0x57)
	.dwendtag $C$DW$T$106


$C$DW$T$107	.dwtag  DW_TAG_array_type, DW_AT_type(*$C$DW$T$48)
	.dwattr $C$DW$T$107, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$107, DW_AT_byte_size(0x270)
$C$DW$51	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$51, DW_AT_upper_bound(0x26f)
	.dwendtag $C$DW$T$107


$C$DW$T$119	.dwtag  DW_TAG_array_type, DW_AT_type(*$C$DW$T$48)
	.dwattr $C$DW$T$119, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$119, DW_AT_byte_size(0x50)
$C$DW$52	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$52, DW_AT_upper_bound(0x4f)
	.dwendtag $C$DW$T$119


$C$DW$T$121	.dwtag  DW_TAG_array_type, DW_AT_type(*$C$DW$T$48)
	.dwattr $C$DW$T$121, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$121, DW_AT_byte_size(0x24)
$C$DW$53	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$53, DW_AT_upper_bound(0x23)
	.dwendtag $C$DW$T$121


$C$DW$T$122	.dwtag  DW_TAG_array_type, DW_AT_type(*$C$DW$T$48)
	.dwattr $C$DW$T$122, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$122, DW_AT_byte_size(0x84)
$C$DW$54	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$54, DW_AT_upper_bound(0x83)
	.dwendtag $C$DW$T$122


$C$DW$T$123	.dwtag  DW_TAG_array_type, DW_AT_type(*$C$DW$T$48)
	.dwattr $C$DW$T$123, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$123, DW_AT_byte_size(0xd4)
$C$DW$55	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$55, DW_AT_upper_bound(0xd3)
	.dwendtag $C$DW$T$123


$C$DW$T$125	.dwtag  DW_TAG_array_type, DW_AT_type(*$C$DW$T$48)
	.dwattr $C$DW$T$125, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$125, DW_AT_byte_size(0x5c)
$C$DW$56	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$56, DW_AT_upper_bound(0x5b)
	.dwendtag $C$DW$T$125


$C$DW$T$126	.dwtag  DW_TAG_array_type, DW_AT_type(*$C$DW$T$48)
	.dwattr $C$DW$T$126, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$126, DW_AT_byte_size(0x15c)
$C$DW$57	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$57, DW_AT_upper_bound(0x15b)
	.dwendtag $C$DW$T$126


$C$DW$T$127	.dwtag  DW_TAG_array_type, DW_AT_type(*$C$DW$T$48)
	.dwattr $C$DW$T$127, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$127, DW_AT_byte_size(0xe8)
$C$DW$58	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$58, DW_AT_upper_bound(0xe7)
	.dwendtag $C$DW$T$127


$C$DW$T$139	.dwtag  DW_TAG_array_type, DW_AT_type(*$C$DW$T$48)
	.dwattr $C$DW$T$139, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$139, DW_AT_byte_size(0x38)
$C$DW$59	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$59, DW_AT_upper_bound(0x37)
	.dwendtag $C$DW$T$139


$C$DW$T$140	.dwtag  DW_TAG_array_type, DW_AT_type(*$C$DW$T$48)
	.dwattr $C$DW$T$140, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$140, DW_AT_byte_size(0x34)
$C$DW$60	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$60, DW_AT_upper_bound(0x33)
	.dwendtag $C$DW$T$140


$C$DW$T$143	.dwtag  DW_TAG_array_type, DW_AT_type(*$C$DW$T$48)
	.dwattr $C$DW$T$143, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$143, DW_AT_byte_size(0xc4)
$C$DW$61	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$61, DW_AT_upper_bound(0xc3)
	.dwendtag $C$DW$T$143


$C$DW$T$144	.dwtag  DW_TAG_array_type, DW_AT_type(*$C$DW$T$48)
	.dwattr $C$DW$T$144, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$144, DW_AT_byte_size(0x3c)
$C$DW$62	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$62, DW_AT_upper_bound(0x3b)
	.dwendtag $C$DW$T$144


$C$DW$T$146	.dwtag  DW_TAG_array_type, DW_AT_type(*$C$DW$T$48)
	.dwattr $C$DW$T$146, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$146, DW_AT_byte_size(0x108)
$C$DW$63	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$63, DW_AT_upper_bound(0x107)
	.dwendtag $C$DW$T$146

$C$DW$T$163	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_Reg8")
	.dwattr $C$DW$T$163, DW_AT_type(*$C$DW$T$48)
	.dwattr $C$DW$T$163, DW_AT_language(DW_LANG_C)
$C$DW$T$7	.dwtag  DW_TAG_base_type, DW_AT_name("wchar_t")
	.dwattr $C$DW$T$7, DW_AT_encoding(DW_ATE_signed_char)
	.dwattr $C$DW$T$7, DW_AT_byte_size(0x02)
$C$DW$T$8	.dwtag  DW_TAG_base_type, DW_AT_name("short")
	.dwattr $C$DW$T$8, DW_AT_encoding(DW_ATE_signed)
	.dwattr $C$DW$T$8, DW_AT_byte_size(0x02)
$C$DW$T$32	.dwtag  DW_TAG_typedef, DW_AT_name("Int16")
	.dwattr $C$DW$T$32, DW_AT_type(*$C$DW$T$8)
	.dwattr $C$DW$T$32, DW_AT_language(DW_LANG_C)
$C$DW$T$164	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_Status")
	.dwattr $C$DW$T$164, DW_AT_type(*$C$DW$T$32)
	.dwattr $C$DW$T$164, DW_AT_language(DW_LANG_C)

$C$DW$T$169	.dwtag  DW_TAG_subroutine_type, DW_AT_type(*$C$DW$T$164)
	.dwattr $C$DW$T$169, DW_AT_language(DW_LANG_C)
$C$DW$64	.dwtag  DW_TAG_formal_parameter, DW_AT_type(*$C$DW$T$166)
$C$DW$65	.dwtag  DW_TAG_formal_parameter, DW_AT_type(*$C$DW$T$168)
	.dwendtag $C$DW$T$169

$C$DW$T$170	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_Uid")
	.dwattr $C$DW$T$170, DW_AT_type(*$C$DW$T$32)
	.dwattr $C$DW$T$170, DW_AT_language(DW_LANG_C)
$C$DW$T$171	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_ModuleId")
	.dwattr $C$DW$T$171, DW_AT_type(*$C$DW$T$32)
	.dwattr $C$DW$T$171, DW_AT_language(DW_LANG_C)
$C$DW$T$172	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_InstNum")
	.dwattr $C$DW$T$172, DW_AT_type(*$C$DW$T$32)
	.dwattr $C$DW$T$172, DW_AT_language(DW_LANG_C)
$C$DW$T$173	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_ChaNum")
	.dwattr $C$DW$T$173, DW_AT_type(*$C$DW$T$32)
	.dwattr $C$DW$T$173, DW_AT_language(DW_LANG_C)
$C$DW$T$9	.dwtag  DW_TAG_base_type, DW_AT_name("unsigned short")
	.dwattr $C$DW$T$9, DW_AT_encoding(DW_ATE_unsigned)
	.dwattr $C$DW$T$9, DW_AT_byte_size(0x02)
$C$DW$T$34	.dwtag  DW_TAG_typedef, DW_AT_name("Bool")
	.dwattr $C$DW$T$34, DW_AT_type(*$C$DW$T$9)
	.dwattr $C$DW$T$34, DW_AT_language(DW_LANG_C)
$C$DW$T$45	.dwtag  DW_TAG_typedef, DW_AT_name("Uint16")
	.dwattr $C$DW$T$45, DW_AT_type(*$C$DW$T$9)
	.dwattr $C$DW$T$45, DW_AT_language(DW_LANG_C)
$C$DW$T$46	.dwtag  DW_TAG_volatile_type
	.dwattr $C$DW$T$46, DW_AT_type(*$C$DW$T$45)
$C$DW$T$175	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_Reg16")
	.dwattr $C$DW$T$175, DW_AT_type(*$C$DW$T$46)
	.dwattr $C$DW$T$175, DW_AT_language(DW_LANG_C)
$C$DW$T$174	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_BitMask16")
	.dwattr $C$DW$T$174, DW_AT_type(*$C$DW$T$45)
	.dwattr $C$DW$T$174, DW_AT_language(DW_LANG_C)
$C$DW$T$10	.dwtag  DW_TAG_base_type, DW_AT_name("int")
	.dwattr $C$DW$T$10, DW_AT_encoding(DW_ATE_signed)
	.dwattr $C$DW$T$10, DW_AT_byte_size(0x04)
$C$DW$T$33	.dwtag  DW_TAG_typedef, DW_AT_name("Int32")
	.dwattr $C$DW$T$33, DW_AT_type(*$C$DW$T$10)
	.dwattr $C$DW$T$33, DW_AT_language(DW_LANG_C)
$C$DW$T$176	.dwtag  DW_TAG_typedef, DW_AT_name("Int")
	.dwattr $C$DW$T$176, DW_AT_type(*$C$DW$T$10)
	.dwattr $C$DW$T$176, DW_AT_language(DW_LANG_C)
$C$DW$T$11	.dwtag  DW_TAG_base_type, DW_AT_name("unsigned int")
	.dwattr $C$DW$T$11, DW_AT_encoding(DW_ATE_unsigned)
	.dwattr $C$DW$T$11, DW_AT_byte_size(0x04)
$C$DW$T$19	.dwtag  DW_TAG_typedef, DW_AT_name("Uint32")
	.dwattr $C$DW$T$19, DW_AT_type(*$C$DW$T$11)
	.dwattr $C$DW$T$19, DW_AT_language(DW_LANG_C)
$C$DW$T$20	.dwtag  DW_TAG_volatile_type
	.dwattr $C$DW$T$20, DW_AT_type(*$C$DW$T$19)

$C$DW$T$21	.dwtag  DW_TAG_array_type, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$T$21, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$21, DW_AT_byte_size(0x08)
$C$DW$66	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$66, DW_AT_upper_bound(0x01)
	.dwendtag $C$DW$T$21


$C$DW$T$22	.dwtag  DW_TAG_array_type, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$T$22, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$22, DW_AT_byte_size(0x20)
$C$DW$67	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$67, DW_AT_upper_bound(0x07)
	.dwendtag $C$DW$T$22


$C$DW$T$23	.dwtag  DW_TAG_array_type, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$T$23, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$23, DW_AT_byte_size(0x0c)
$C$DW$68	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$68, DW_AT_upper_bound(0x02)
	.dwendtag $C$DW$T$23


$C$DW$T$24	.dwtag  DW_TAG_array_type, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$T$24, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$24, DW_AT_byte_size(0x10)
$C$DW$69	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$69, DW_AT_upper_bound(0x03)
	.dwendtag $C$DW$T$24


$C$DW$T$25	.dwtag  DW_TAG_array_type, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$T$25, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$25, DW_AT_byte_size(0x30)
$C$DW$70	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$70, DW_AT_upper_bound(0x0b)
	.dwendtag $C$DW$T$25


$C$DW$T$26	.dwtag  DW_TAG_array_type, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$T$26, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$26, DW_AT_byte_size(0x40)
$C$DW$71	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$71, DW_AT_upper_bound(0x0f)
	.dwendtag $C$DW$T$26


$C$DW$T$77	.dwtag  DW_TAG_array_type, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$T$77, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$77, DW_AT_byte_size(0x100)
$C$DW$72	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$72, DW_AT_upper_bound(0x3f)
	.dwendtag $C$DW$T$77


$C$DW$T$124	.dwtag  DW_TAG_array_type, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$T$124, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$124, DW_AT_byte_size(0xa4)
$C$DW$73	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$73, DW_AT_upper_bound(0x28)
	.dwendtag $C$DW$T$124


$C$DW$T$135	.dwtag  DW_TAG_array_type, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$T$135, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$135, DW_AT_byte_size(0x80)
$C$DW$74	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$74, DW_AT_upper_bound(0x1f)
	.dwendtag $C$DW$T$135


$C$DW$T$145	.dwtag  DW_TAG_array_type, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$T$145, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$145, DW_AT_byte_size(0x18)
$C$DW$75	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$75, DW_AT_upper_bound(0x05)
	.dwendtag $C$DW$T$145

$C$DW$T$177	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_Reg32")
	.dwattr $C$DW$T$177, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$T$177, DW_AT_language(DW_LANG_C)

$C$DW$T$36	.dwtag  DW_TAG_array_type, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$T$36, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$36, DW_AT_byte_size(0x40)
$C$DW$76	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$76, DW_AT_upper_bound(0x0f)
	.dwendtag $C$DW$T$36


$C$DW$T$148	.dwtag  DW_TAG_array_type, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$T$148, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$148, DW_AT_byte_size(0x18)
$C$DW$77	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$77, DW_AT_upper_bound(0x05)
	.dwendtag $C$DW$T$148

$C$DW$T$152	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_BitMask32")
	.dwattr $C$DW$T$152, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$T$152, DW_AT_language(DW_LANG_C)
$C$DW$T$179	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_Xio")
	.dwattr $C$DW$T$179, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$T$179, DW_AT_language(DW_LANG_C)
$C$DW$T$181	.dwtag  DW_TAG_typedef, DW_AT_name("Uns")
	.dwattr $C$DW$T$181, DW_AT_type(*$C$DW$T$11)
	.dwattr $C$DW$T$181, DW_AT_language(DW_LANG_C)
$C$DW$T$12	.dwtag  DW_TAG_base_type, DW_AT_name("long")
	.dwattr $C$DW$T$12, DW_AT_encoding(DW_ATE_signed)
	.dwattr $C$DW$T$12, DW_AT_byte_size(0x08)
	.dwattr $C$DW$T$12, DW_AT_bit_size(0x28)
	.dwattr $C$DW$T$12, DW_AT_bit_offset(0x18)
$C$DW$T$13	.dwtag  DW_TAG_base_type, DW_AT_name("unsigned long")
	.dwattr $C$DW$T$13, DW_AT_encoding(DW_ATE_unsigned)
	.dwattr $C$DW$T$13, DW_AT_byte_size(0x08)
	.dwattr $C$DW$T$13, DW_AT_bit_size(0x28)
	.dwattr $C$DW$T$13, DW_AT_bit_offset(0x18)
$C$DW$T$14	.dwtag  DW_TAG_base_type, DW_AT_name("long long")
	.dwattr $C$DW$T$14, DW_AT_encoding(DW_ATE_signed)
	.dwattr $C$DW$T$14, DW_AT_byte_size(0x08)
$C$DW$T$15	.dwtag  DW_TAG_base_type, DW_AT_name("unsigned long long")
	.dwattr $C$DW$T$15, DW_AT_encoding(DW_ATE_unsigned)
	.dwattr $C$DW$T$15, DW_AT_byte_size(0x08)
$C$DW$T$16	.dwtag  DW_TAG_base_type, DW_AT_name("float")
	.dwattr $C$DW$T$16, DW_AT_encoding(DW_ATE_float)
	.dwattr $C$DW$T$16, DW_AT_byte_size(0x04)
$C$DW$T$17	.dwtag  DW_TAG_base_type, DW_AT_name("double")
	.dwattr $C$DW$T$17, DW_AT_encoding(DW_ATE_float)
	.dwattr $C$DW$T$17, DW_AT_byte_size(0x08)
$C$DW$T$18	.dwtag  DW_TAG_base_type, DW_AT_name("long double")
	.dwattr $C$DW$T$18, DW_AT_encoding(DW_ATE_float)
	.dwattr $C$DW$T$18, DW_AT_byte_size(0x08)

$C$DW$T$27	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$27, DW_AT_byte_size(0x2c0)
$C$DW$78	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$78, DW_AT_name("PID")
	.dwattr $C$DW$78, DW_AT_TI_symbol_name("_PID")
	.dwattr $C$DW$78, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$78, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$79	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$79, DW_AT_name("PWRDEMU")
	.dwattr $C$DW$79, DW_AT_TI_symbol_name("_PWRDEMU")
	.dwattr $C$DW$79, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$79, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$80	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$21)
	.dwattr $C$DW$80, DW_AT_name("RSVD0")
	.dwattr $C$DW$80, DW_AT_TI_symbol_name("_RSVD0")
	.dwattr $C$DW$80, DW_AT_data_member_location[DW_OP_plus_uconst 0x8]
	.dwattr $C$DW$80, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$81	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$81, DW_AT_name("PFUNC")
	.dwattr $C$DW$81, DW_AT_TI_symbol_name("_PFUNC")
	.dwattr $C$DW$81, DW_AT_data_member_location[DW_OP_plus_uconst 0x10]
	.dwattr $C$DW$81, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$82	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$82, DW_AT_name("PDIR")
	.dwattr $C$DW$82, DW_AT_TI_symbol_name("_PDIR")
	.dwattr $C$DW$82, DW_AT_data_member_location[DW_OP_plus_uconst 0x14]
	.dwattr $C$DW$82, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$83	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$83, DW_AT_name("PDOUT")
	.dwattr $C$DW$83, DW_AT_TI_symbol_name("_PDOUT")
	.dwattr $C$DW$83, DW_AT_data_member_location[DW_OP_plus_uconst 0x18]
	.dwattr $C$DW$83, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$84	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$84, DW_AT_name("PDIN_PDSET")
	.dwattr $C$DW$84, DW_AT_TI_symbol_name("_PDIN_PDSET")
	.dwattr $C$DW$84, DW_AT_data_member_location[DW_OP_plus_uconst 0x1c]
	.dwattr $C$DW$84, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$85	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$85, DW_AT_name("PDCLR")
	.dwattr $C$DW$85, DW_AT_TI_symbol_name("_PDCLR")
	.dwattr $C$DW$85, DW_AT_data_member_location[DW_OP_plus_uconst 0x20]
	.dwattr $C$DW$85, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$86	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$22)
	.dwattr $C$DW$86, DW_AT_name("RSVD1")
	.dwattr $C$DW$86, DW_AT_TI_symbol_name("_RSVD1")
	.dwattr $C$DW$86, DW_AT_data_member_location[DW_OP_plus_uconst 0x24]
	.dwattr $C$DW$86, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$87	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$87, DW_AT_name("GBLCTL")
	.dwattr $C$DW$87, DW_AT_TI_symbol_name("_GBLCTL")
	.dwattr $C$DW$87, DW_AT_data_member_location[DW_OP_plus_uconst 0x44]
	.dwattr $C$DW$87, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$88	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$88, DW_AT_name("AMUTE")
	.dwattr $C$DW$88, DW_AT_TI_symbol_name("_AMUTE")
	.dwattr $C$DW$88, DW_AT_data_member_location[DW_OP_plus_uconst 0x48]
	.dwattr $C$DW$88, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$89	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$89, DW_AT_name("DLBCTL")
	.dwattr $C$DW$89, DW_AT_TI_symbol_name("_DLBCTL")
	.dwattr $C$DW$89, DW_AT_data_member_location[DW_OP_plus_uconst 0x4c]
	.dwattr $C$DW$89, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$90	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$90, DW_AT_name("DITCTL")
	.dwattr $C$DW$90, DW_AT_TI_symbol_name("_DITCTL")
	.dwattr $C$DW$90, DW_AT_data_member_location[DW_OP_plus_uconst 0x50]
	.dwattr $C$DW$90, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$91	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$23)
	.dwattr $C$DW$91, DW_AT_name("RSVD2")
	.dwattr $C$DW$91, DW_AT_TI_symbol_name("_RSVD2")
	.dwattr $C$DW$91, DW_AT_data_member_location[DW_OP_plus_uconst 0x54]
	.dwattr $C$DW$91, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$92	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$92, DW_AT_name("RGBLCTL")
	.dwattr $C$DW$92, DW_AT_TI_symbol_name("_RGBLCTL")
	.dwattr $C$DW$92, DW_AT_data_member_location[DW_OP_plus_uconst 0x60]
	.dwattr $C$DW$92, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$93	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$93, DW_AT_name("RMASK")
	.dwattr $C$DW$93, DW_AT_TI_symbol_name("_RMASK")
	.dwattr $C$DW$93, DW_AT_data_member_location[DW_OP_plus_uconst 0x64]
	.dwattr $C$DW$93, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$94	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$94, DW_AT_name("RFMT")
	.dwattr $C$DW$94, DW_AT_TI_symbol_name("_RFMT")
	.dwattr $C$DW$94, DW_AT_data_member_location[DW_OP_plus_uconst 0x68]
	.dwattr $C$DW$94, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$95	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$95, DW_AT_name("AFSRCTL")
	.dwattr $C$DW$95, DW_AT_TI_symbol_name("_AFSRCTL")
	.dwattr $C$DW$95, DW_AT_data_member_location[DW_OP_plus_uconst 0x6c]
	.dwattr $C$DW$95, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$96	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$96, DW_AT_name("ACLKRCTL")
	.dwattr $C$DW$96, DW_AT_TI_symbol_name("_ACLKRCTL")
	.dwattr $C$DW$96, DW_AT_data_member_location[DW_OP_plus_uconst 0x70]
	.dwattr $C$DW$96, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$97	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$97, DW_AT_name("AHCLKRCTL")
	.dwattr $C$DW$97, DW_AT_TI_symbol_name("_AHCLKRCTL")
	.dwattr $C$DW$97, DW_AT_data_member_location[DW_OP_plus_uconst 0x74]
	.dwattr $C$DW$97, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$98	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$98, DW_AT_name("RTDM")
	.dwattr $C$DW$98, DW_AT_TI_symbol_name("_RTDM")
	.dwattr $C$DW$98, DW_AT_data_member_location[DW_OP_plus_uconst 0x78]
	.dwattr $C$DW$98, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$99	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$99, DW_AT_name("RINTCTL")
	.dwattr $C$DW$99, DW_AT_TI_symbol_name("_RINTCTL")
	.dwattr $C$DW$99, DW_AT_data_member_location[DW_OP_plus_uconst 0x7c]
	.dwattr $C$DW$99, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$100	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$100, DW_AT_name("RSTAT")
	.dwattr $C$DW$100, DW_AT_TI_symbol_name("_RSTAT")
	.dwattr $C$DW$100, DW_AT_data_member_location[DW_OP_plus_uconst 0x80]
	.dwattr $C$DW$100, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$101	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$101, DW_AT_name("RSLOT")
	.dwattr $C$DW$101, DW_AT_TI_symbol_name("_RSLOT")
	.dwattr $C$DW$101, DW_AT_data_member_location[DW_OP_plus_uconst 0x84]
	.dwattr $C$DW$101, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$102	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$102, DW_AT_name("RCLKCHK")
	.dwattr $C$DW$102, DW_AT_TI_symbol_name("_RCLKCHK")
	.dwattr $C$DW$102, DW_AT_data_member_location[DW_OP_plus_uconst 0x88]
	.dwattr $C$DW$102, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$103	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$103, DW_AT_name("REVTCTL")
	.dwattr $C$DW$103, DW_AT_TI_symbol_name("_REVTCTL")
	.dwattr $C$DW$103, DW_AT_data_member_location[DW_OP_plus_uconst 0x8c]
	.dwattr $C$DW$103, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$104	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$24)
	.dwattr $C$DW$104, DW_AT_name("RSVD3")
	.dwattr $C$DW$104, DW_AT_TI_symbol_name("_RSVD3")
	.dwattr $C$DW$104, DW_AT_data_member_location[DW_OP_plus_uconst 0x90]
	.dwattr $C$DW$104, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$105	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$105, DW_AT_name("XGBLCTL")
	.dwattr $C$DW$105, DW_AT_TI_symbol_name("_XGBLCTL")
	.dwattr $C$DW$105, DW_AT_data_member_location[DW_OP_plus_uconst 0xa0]
	.dwattr $C$DW$105, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$106	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$106, DW_AT_name("XMASK")
	.dwattr $C$DW$106, DW_AT_TI_symbol_name("_XMASK")
	.dwattr $C$DW$106, DW_AT_data_member_location[DW_OP_plus_uconst 0xa4]
	.dwattr $C$DW$106, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$107	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$107, DW_AT_name("XFMT")
	.dwattr $C$DW$107, DW_AT_TI_symbol_name("_XFMT")
	.dwattr $C$DW$107, DW_AT_data_member_location[DW_OP_plus_uconst 0xa8]
	.dwattr $C$DW$107, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$108	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$108, DW_AT_name("AFSXCTL")
	.dwattr $C$DW$108, DW_AT_TI_symbol_name("_AFSXCTL")
	.dwattr $C$DW$108, DW_AT_data_member_location[DW_OP_plus_uconst 0xac]
	.dwattr $C$DW$108, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$109	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$109, DW_AT_name("ACLKXCTL")
	.dwattr $C$DW$109, DW_AT_TI_symbol_name("_ACLKXCTL")
	.dwattr $C$DW$109, DW_AT_data_member_location[DW_OP_plus_uconst 0xb0]
	.dwattr $C$DW$109, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$110	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$110, DW_AT_name("AHCLKXCTL")
	.dwattr $C$DW$110, DW_AT_TI_symbol_name("_AHCLKXCTL")
	.dwattr $C$DW$110, DW_AT_data_member_location[DW_OP_plus_uconst 0xb4]
	.dwattr $C$DW$110, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$111	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$111, DW_AT_name("XTDM")
	.dwattr $C$DW$111, DW_AT_TI_symbol_name("_XTDM")
	.dwattr $C$DW$111, DW_AT_data_member_location[DW_OP_plus_uconst 0xb8]
	.dwattr $C$DW$111, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$112	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$112, DW_AT_name("XINTCTL")
	.dwattr $C$DW$112, DW_AT_TI_symbol_name("_XINTCTL")
	.dwattr $C$DW$112, DW_AT_data_member_location[DW_OP_plus_uconst 0xbc]
	.dwattr $C$DW$112, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$113	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$113, DW_AT_name("XSTAT")
	.dwattr $C$DW$113, DW_AT_TI_symbol_name("_XSTAT")
	.dwattr $C$DW$113, DW_AT_data_member_location[DW_OP_plus_uconst 0xc0]
	.dwattr $C$DW$113, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$114	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$114, DW_AT_name("XSLOT")
	.dwattr $C$DW$114, DW_AT_TI_symbol_name("_XSLOT")
	.dwattr $C$DW$114, DW_AT_data_member_location[DW_OP_plus_uconst 0xc4]
	.dwattr $C$DW$114, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$115	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$115, DW_AT_name("XCLKCHK")
	.dwattr $C$DW$115, DW_AT_TI_symbol_name("_XCLKCHK")
	.dwattr $C$DW$115, DW_AT_data_member_location[DW_OP_plus_uconst 0xc8]
	.dwattr $C$DW$115, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$116	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$116, DW_AT_name("XEVTCTL")
	.dwattr $C$DW$116, DW_AT_TI_symbol_name("_XEVTCTL")
	.dwattr $C$DW$116, DW_AT_data_member_location[DW_OP_plus_uconst 0xcc]
	.dwattr $C$DW$116, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$117	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$25)
	.dwattr $C$DW$117, DW_AT_name("RSVD4")
	.dwattr $C$DW$117, DW_AT_TI_symbol_name("_RSVD4")
	.dwattr $C$DW$117, DW_AT_data_member_location[DW_OP_plus_uconst 0xd0]
	.dwattr $C$DW$117, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$118	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$118, DW_AT_name("DITCSRA0")
	.dwattr $C$DW$118, DW_AT_TI_symbol_name("_DITCSRA0")
	.dwattr $C$DW$118, DW_AT_data_member_location[DW_OP_plus_uconst 0x100]
	.dwattr $C$DW$118, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$119	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$119, DW_AT_name("DITCSRA1")
	.dwattr $C$DW$119, DW_AT_TI_symbol_name("_DITCSRA1")
	.dwattr $C$DW$119, DW_AT_data_member_location[DW_OP_plus_uconst 0x104]
	.dwattr $C$DW$119, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$120	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$120, DW_AT_name("DITCSRA2")
	.dwattr $C$DW$120, DW_AT_TI_symbol_name("_DITCSRA2")
	.dwattr $C$DW$120, DW_AT_data_member_location[DW_OP_plus_uconst 0x108]
	.dwattr $C$DW$120, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$121	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$121, DW_AT_name("DITCSRA3")
	.dwattr $C$DW$121, DW_AT_TI_symbol_name("_DITCSRA3")
	.dwattr $C$DW$121, DW_AT_data_member_location[DW_OP_plus_uconst 0x10c]
	.dwattr $C$DW$121, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$122	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$122, DW_AT_name("DITCSRA4")
	.dwattr $C$DW$122, DW_AT_TI_symbol_name("_DITCSRA4")
	.dwattr $C$DW$122, DW_AT_data_member_location[DW_OP_plus_uconst 0x110]
	.dwattr $C$DW$122, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$123	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$123, DW_AT_name("DITCSRA5")
	.dwattr $C$DW$123, DW_AT_TI_symbol_name("_DITCSRA5")
	.dwattr $C$DW$123, DW_AT_data_member_location[DW_OP_plus_uconst 0x114]
	.dwattr $C$DW$123, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$124	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$124, DW_AT_name("DITCSRB0")
	.dwattr $C$DW$124, DW_AT_TI_symbol_name("_DITCSRB0")
	.dwattr $C$DW$124, DW_AT_data_member_location[DW_OP_plus_uconst 0x118]
	.dwattr $C$DW$124, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$125	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$125, DW_AT_name("DITCSRB1")
	.dwattr $C$DW$125, DW_AT_TI_symbol_name("_DITCSRB1")
	.dwattr $C$DW$125, DW_AT_data_member_location[DW_OP_plus_uconst 0x11c]
	.dwattr $C$DW$125, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$126	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$126, DW_AT_name("DITCSRB2")
	.dwattr $C$DW$126, DW_AT_TI_symbol_name("_DITCSRB2")
	.dwattr $C$DW$126, DW_AT_data_member_location[DW_OP_plus_uconst 0x120]
	.dwattr $C$DW$126, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$127	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$127, DW_AT_name("DITCSRB3")
	.dwattr $C$DW$127, DW_AT_TI_symbol_name("_DITCSRB3")
	.dwattr $C$DW$127, DW_AT_data_member_location[DW_OP_plus_uconst 0x124]
	.dwattr $C$DW$127, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$128	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$128, DW_AT_name("DITCSRB4")
	.dwattr $C$DW$128, DW_AT_TI_symbol_name("_DITCSRB4")
	.dwattr $C$DW$128, DW_AT_data_member_location[DW_OP_plus_uconst 0x128]
	.dwattr $C$DW$128, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$129	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$129, DW_AT_name("DITCSRB5")
	.dwattr $C$DW$129, DW_AT_TI_symbol_name("_DITCSRB5")
	.dwattr $C$DW$129, DW_AT_data_member_location[DW_OP_plus_uconst 0x12c]
	.dwattr $C$DW$129, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$130	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$130, DW_AT_name("DITUDRA0")
	.dwattr $C$DW$130, DW_AT_TI_symbol_name("_DITUDRA0")
	.dwattr $C$DW$130, DW_AT_data_member_location[DW_OP_plus_uconst 0x130]
	.dwattr $C$DW$130, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$131	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$131, DW_AT_name("DITUDRA1")
	.dwattr $C$DW$131, DW_AT_TI_symbol_name("_DITUDRA1")
	.dwattr $C$DW$131, DW_AT_data_member_location[DW_OP_plus_uconst 0x134]
	.dwattr $C$DW$131, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$132	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$132, DW_AT_name("DITUDRA2")
	.dwattr $C$DW$132, DW_AT_TI_symbol_name("_DITUDRA2")
	.dwattr $C$DW$132, DW_AT_data_member_location[DW_OP_plus_uconst 0x138]
	.dwattr $C$DW$132, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$133	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$133, DW_AT_name("DITUDRA3")
	.dwattr $C$DW$133, DW_AT_TI_symbol_name("_DITUDRA3")
	.dwattr $C$DW$133, DW_AT_data_member_location[DW_OP_plus_uconst 0x13c]
	.dwattr $C$DW$133, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$134	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$134, DW_AT_name("DITUDRA4")
	.dwattr $C$DW$134, DW_AT_TI_symbol_name("_DITUDRA4")
	.dwattr $C$DW$134, DW_AT_data_member_location[DW_OP_plus_uconst 0x140]
	.dwattr $C$DW$134, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$135	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$135, DW_AT_name("DITUDRA5")
	.dwattr $C$DW$135, DW_AT_TI_symbol_name("_DITUDRA5")
	.dwattr $C$DW$135, DW_AT_data_member_location[DW_OP_plus_uconst 0x144]
	.dwattr $C$DW$135, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$136	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$136, DW_AT_name("DITUDRB0")
	.dwattr $C$DW$136, DW_AT_TI_symbol_name("_DITUDRB0")
	.dwattr $C$DW$136, DW_AT_data_member_location[DW_OP_plus_uconst 0x148]
	.dwattr $C$DW$136, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$137	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$137, DW_AT_name("DITUDRB1")
	.dwattr $C$DW$137, DW_AT_TI_symbol_name("_DITUDRB1")
	.dwattr $C$DW$137, DW_AT_data_member_location[DW_OP_plus_uconst 0x14c]
	.dwattr $C$DW$137, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$138	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$138, DW_AT_name("DITUDRB2")
	.dwattr $C$DW$138, DW_AT_TI_symbol_name("_DITUDRB2")
	.dwattr $C$DW$138, DW_AT_data_member_location[DW_OP_plus_uconst 0x150]
	.dwattr $C$DW$138, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$139	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$139, DW_AT_name("DITUDRB3")
	.dwattr $C$DW$139, DW_AT_TI_symbol_name("_DITUDRB3")
	.dwattr $C$DW$139, DW_AT_data_member_location[DW_OP_plus_uconst 0x154]
	.dwattr $C$DW$139, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$140	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$140, DW_AT_name("DITUDRB4")
	.dwattr $C$DW$140, DW_AT_TI_symbol_name("_DITUDRB4")
	.dwattr $C$DW$140, DW_AT_data_member_location[DW_OP_plus_uconst 0x158]
	.dwattr $C$DW$140, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$141	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$141, DW_AT_name("DITUDRB5")
	.dwattr $C$DW$141, DW_AT_TI_symbol_name("_DITUDRB5")
	.dwattr $C$DW$141, DW_AT_data_member_location[DW_OP_plus_uconst 0x15c]
	.dwattr $C$DW$141, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$142	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$22)
	.dwattr $C$DW$142, DW_AT_name("RSVD5")
	.dwattr $C$DW$142, DW_AT_TI_symbol_name("_RSVD5")
	.dwattr $C$DW$142, DW_AT_data_member_location[DW_OP_plus_uconst 0x160]
	.dwattr $C$DW$142, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$143	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$143, DW_AT_name("SRCTL0")
	.dwattr $C$DW$143, DW_AT_TI_symbol_name("_SRCTL0")
	.dwattr $C$DW$143, DW_AT_data_member_location[DW_OP_plus_uconst 0x180]
	.dwattr $C$DW$143, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$144	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$144, DW_AT_name("SRCTL1")
	.dwattr $C$DW$144, DW_AT_TI_symbol_name("_SRCTL1")
	.dwattr $C$DW$144, DW_AT_data_member_location[DW_OP_plus_uconst 0x184]
	.dwattr $C$DW$144, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$145	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$145, DW_AT_name("SRCTL2")
	.dwattr $C$DW$145, DW_AT_TI_symbol_name("_SRCTL2")
	.dwattr $C$DW$145, DW_AT_data_member_location[DW_OP_plus_uconst 0x188]
	.dwattr $C$DW$145, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$146	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$146, DW_AT_name("SRCTL3")
	.dwattr $C$DW$146, DW_AT_TI_symbol_name("_SRCTL3")
	.dwattr $C$DW$146, DW_AT_data_member_location[DW_OP_plus_uconst 0x18c]
	.dwattr $C$DW$146, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$147	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$147, DW_AT_name("SRCTL4")
	.dwattr $C$DW$147, DW_AT_TI_symbol_name("_SRCTL4")
	.dwattr $C$DW$147, DW_AT_data_member_location[DW_OP_plus_uconst 0x190]
	.dwattr $C$DW$147, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$148	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$148, DW_AT_name("SRCTL5")
	.dwattr $C$DW$148, DW_AT_TI_symbol_name("_SRCTL5")
	.dwattr $C$DW$148, DW_AT_data_member_location[DW_OP_plus_uconst 0x194]
	.dwattr $C$DW$148, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$149	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$149, DW_AT_name("SRCTL6")
	.dwattr $C$DW$149, DW_AT_TI_symbol_name("_SRCTL6")
	.dwattr $C$DW$149, DW_AT_data_member_location[DW_OP_plus_uconst 0x198]
	.dwattr $C$DW$149, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$150	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$150, DW_AT_name("SRCTL7")
	.dwattr $C$DW$150, DW_AT_TI_symbol_name("_SRCTL7")
	.dwattr $C$DW$150, DW_AT_data_member_location[DW_OP_plus_uconst 0x19c]
	.dwattr $C$DW$150, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$151	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$151, DW_AT_name("SRCTL8")
	.dwattr $C$DW$151, DW_AT_TI_symbol_name("_SRCTL8")
	.dwattr $C$DW$151, DW_AT_data_member_location[DW_OP_plus_uconst 0x1a0]
	.dwattr $C$DW$151, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$152	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$152, DW_AT_name("SRCTL9")
	.dwattr $C$DW$152, DW_AT_TI_symbol_name("_SRCTL9")
	.dwattr $C$DW$152, DW_AT_data_member_location[DW_OP_plus_uconst 0x1a4]
	.dwattr $C$DW$152, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$153	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$153, DW_AT_name("SRCTL10")
	.dwattr $C$DW$153, DW_AT_TI_symbol_name("_SRCTL10")
	.dwattr $C$DW$153, DW_AT_data_member_location[DW_OP_plus_uconst 0x1a8]
	.dwattr $C$DW$153, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$154	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$154, DW_AT_name("SRCTL11")
	.dwattr $C$DW$154, DW_AT_TI_symbol_name("_SRCTL11")
	.dwattr $C$DW$154, DW_AT_data_member_location[DW_OP_plus_uconst 0x1ac]
	.dwattr $C$DW$154, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$155	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$155, DW_AT_name("SRCTL12")
	.dwattr $C$DW$155, DW_AT_TI_symbol_name("_SRCTL12")
	.dwattr $C$DW$155, DW_AT_data_member_location[DW_OP_plus_uconst 0x1b0]
	.dwattr $C$DW$155, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$156	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$156, DW_AT_name("SRCTL13")
	.dwattr $C$DW$156, DW_AT_TI_symbol_name("_SRCTL13")
	.dwattr $C$DW$156, DW_AT_data_member_location[DW_OP_plus_uconst 0x1b4]
	.dwattr $C$DW$156, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$157	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$157, DW_AT_name("SRCTL14")
	.dwattr $C$DW$157, DW_AT_TI_symbol_name("_SRCTL14")
	.dwattr $C$DW$157, DW_AT_data_member_location[DW_OP_plus_uconst 0x1b8]
	.dwattr $C$DW$157, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$158	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$158, DW_AT_name("SRCTL15")
	.dwattr $C$DW$158, DW_AT_TI_symbol_name("_SRCTL15")
	.dwattr $C$DW$158, DW_AT_data_member_location[DW_OP_plus_uconst 0x1bc]
	.dwattr $C$DW$158, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$159	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$26)
	.dwattr $C$DW$159, DW_AT_name("RSVD6")
	.dwattr $C$DW$159, DW_AT_TI_symbol_name("_RSVD6")
	.dwattr $C$DW$159, DW_AT_data_member_location[DW_OP_plus_uconst 0x1c0]
	.dwattr $C$DW$159, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$160	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$160, DW_AT_name("XBUF0")
	.dwattr $C$DW$160, DW_AT_TI_symbol_name("_XBUF0")
	.dwattr $C$DW$160, DW_AT_data_member_location[DW_OP_plus_uconst 0x200]
	.dwattr $C$DW$160, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$161	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$161, DW_AT_name("XBUF1")
	.dwattr $C$DW$161, DW_AT_TI_symbol_name("_XBUF1")
	.dwattr $C$DW$161, DW_AT_data_member_location[DW_OP_plus_uconst 0x204]
	.dwattr $C$DW$161, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$162	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$162, DW_AT_name("XBUF2")
	.dwattr $C$DW$162, DW_AT_TI_symbol_name("_XBUF2")
	.dwattr $C$DW$162, DW_AT_data_member_location[DW_OP_plus_uconst 0x208]
	.dwattr $C$DW$162, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$163	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$163, DW_AT_name("XBUF3")
	.dwattr $C$DW$163, DW_AT_TI_symbol_name("_XBUF3")
	.dwattr $C$DW$163, DW_AT_data_member_location[DW_OP_plus_uconst 0x20c]
	.dwattr $C$DW$163, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$164	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$164, DW_AT_name("XBUF4")
	.dwattr $C$DW$164, DW_AT_TI_symbol_name("_XBUF4")
	.dwattr $C$DW$164, DW_AT_data_member_location[DW_OP_plus_uconst 0x210]
	.dwattr $C$DW$164, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$165	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$165, DW_AT_name("XBUF5")
	.dwattr $C$DW$165, DW_AT_TI_symbol_name("_XBUF5")
	.dwattr $C$DW$165, DW_AT_data_member_location[DW_OP_plus_uconst 0x214]
	.dwattr $C$DW$165, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$166	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$166, DW_AT_name("XBUF6")
	.dwattr $C$DW$166, DW_AT_TI_symbol_name("_XBUF6")
	.dwattr $C$DW$166, DW_AT_data_member_location[DW_OP_plus_uconst 0x218]
	.dwattr $C$DW$166, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$167	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$167, DW_AT_name("XBUF7")
	.dwattr $C$DW$167, DW_AT_TI_symbol_name("_XBUF7")
	.dwattr $C$DW$167, DW_AT_data_member_location[DW_OP_plus_uconst 0x21c]
	.dwattr $C$DW$167, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$168	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$168, DW_AT_name("XBUF8")
	.dwattr $C$DW$168, DW_AT_TI_symbol_name("_XBUF8")
	.dwattr $C$DW$168, DW_AT_data_member_location[DW_OP_plus_uconst 0x220]
	.dwattr $C$DW$168, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$169	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$169, DW_AT_name("XBUF9")
	.dwattr $C$DW$169, DW_AT_TI_symbol_name("_XBUF9")
	.dwattr $C$DW$169, DW_AT_data_member_location[DW_OP_plus_uconst 0x224]
	.dwattr $C$DW$169, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$170	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$170, DW_AT_name("XBUF10")
	.dwattr $C$DW$170, DW_AT_TI_symbol_name("_XBUF10")
	.dwattr $C$DW$170, DW_AT_data_member_location[DW_OP_plus_uconst 0x228]
	.dwattr $C$DW$170, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$171	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$171, DW_AT_name("XBUF11")
	.dwattr $C$DW$171, DW_AT_TI_symbol_name("_XBUF11")
	.dwattr $C$DW$171, DW_AT_data_member_location[DW_OP_plus_uconst 0x22c]
	.dwattr $C$DW$171, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$172	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$172, DW_AT_name("XBUF12")
	.dwattr $C$DW$172, DW_AT_TI_symbol_name("_XBUF12")
	.dwattr $C$DW$172, DW_AT_data_member_location[DW_OP_plus_uconst 0x230]
	.dwattr $C$DW$172, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$173	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$173, DW_AT_name("XBUF13")
	.dwattr $C$DW$173, DW_AT_TI_symbol_name("_XBUF13")
	.dwattr $C$DW$173, DW_AT_data_member_location[DW_OP_plus_uconst 0x234]
	.dwattr $C$DW$173, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$174	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$174, DW_AT_name("XBUF14")
	.dwattr $C$DW$174, DW_AT_TI_symbol_name("_XBUF14")
	.dwattr $C$DW$174, DW_AT_data_member_location[DW_OP_plus_uconst 0x238]
	.dwattr $C$DW$174, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$175	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$175, DW_AT_name("XBUF15")
	.dwattr $C$DW$175, DW_AT_TI_symbol_name("_XBUF15")
	.dwattr $C$DW$175, DW_AT_data_member_location[DW_OP_plus_uconst 0x23c]
	.dwattr $C$DW$175, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$176	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$26)
	.dwattr $C$DW$176, DW_AT_name("RSVD7")
	.dwattr $C$DW$176, DW_AT_TI_symbol_name("_RSVD7")
	.dwattr $C$DW$176, DW_AT_data_member_location[DW_OP_plus_uconst 0x240]
	.dwattr $C$DW$176, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$177	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$177, DW_AT_name("RBUF0")
	.dwattr $C$DW$177, DW_AT_TI_symbol_name("_RBUF0")
	.dwattr $C$DW$177, DW_AT_data_member_location[DW_OP_plus_uconst 0x280]
	.dwattr $C$DW$177, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$178	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$178, DW_AT_name("RBUF1")
	.dwattr $C$DW$178, DW_AT_TI_symbol_name("_RBUF1")
	.dwattr $C$DW$178, DW_AT_data_member_location[DW_OP_plus_uconst 0x284]
	.dwattr $C$DW$178, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$179	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$179, DW_AT_name("RBUF2")
	.dwattr $C$DW$179, DW_AT_TI_symbol_name("_RBUF2")
	.dwattr $C$DW$179, DW_AT_data_member_location[DW_OP_plus_uconst 0x288]
	.dwattr $C$DW$179, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$180	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$180, DW_AT_name("RBUF3")
	.dwattr $C$DW$180, DW_AT_TI_symbol_name("_RBUF3")
	.dwattr $C$DW$180, DW_AT_data_member_location[DW_OP_plus_uconst 0x28c]
	.dwattr $C$DW$180, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$181	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$181, DW_AT_name("RBUF4")
	.dwattr $C$DW$181, DW_AT_TI_symbol_name("_RBUF4")
	.dwattr $C$DW$181, DW_AT_data_member_location[DW_OP_plus_uconst 0x290]
	.dwattr $C$DW$181, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$182	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$182, DW_AT_name("RBUF5")
	.dwattr $C$DW$182, DW_AT_TI_symbol_name("_RBUF5")
	.dwattr $C$DW$182, DW_AT_data_member_location[DW_OP_plus_uconst 0x294]
	.dwattr $C$DW$182, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$183	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$183, DW_AT_name("RBUF6")
	.dwattr $C$DW$183, DW_AT_TI_symbol_name("_RBUF6")
	.dwattr $C$DW$183, DW_AT_data_member_location[DW_OP_plus_uconst 0x298]
	.dwattr $C$DW$183, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$184	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$184, DW_AT_name("RBUF7")
	.dwattr $C$DW$184, DW_AT_TI_symbol_name("_RBUF7")
	.dwattr $C$DW$184, DW_AT_data_member_location[DW_OP_plus_uconst 0x29c]
	.dwattr $C$DW$184, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$185	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$185, DW_AT_name("RBUF8")
	.dwattr $C$DW$185, DW_AT_TI_symbol_name("_RBUF8")
	.dwattr $C$DW$185, DW_AT_data_member_location[DW_OP_plus_uconst 0x2a0]
	.dwattr $C$DW$185, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$186	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$186, DW_AT_name("RBUF9")
	.dwattr $C$DW$186, DW_AT_TI_symbol_name("_RBUF9")
	.dwattr $C$DW$186, DW_AT_data_member_location[DW_OP_plus_uconst 0x2a4]
	.dwattr $C$DW$186, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$187	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$187, DW_AT_name("RBUF10")
	.dwattr $C$DW$187, DW_AT_TI_symbol_name("_RBUF10")
	.dwattr $C$DW$187, DW_AT_data_member_location[DW_OP_plus_uconst 0x2a8]
	.dwattr $C$DW$187, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$188	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$188, DW_AT_name("RBUF11")
	.dwattr $C$DW$188, DW_AT_TI_symbol_name("_RBUF11")
	.dwattr $C$DW$188, DW_AT_data_member_location[DW_OP_plus_uconst 0x2ac]
	.dwattr $C$DW$188, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$189	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$189, DW_AT_name("RBUF12")
	.dwattr $C$DW$189, DW_AT_TI_symbol_name("_RBUF12")
	.dwattr $C$DW$189, DW_AT_data_member_location[DW_OP_plus_uconst 0x2b0]
	.dwattr $C$DW$189, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$190	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$190, DW_AT_name("RBUF13")
	.dwattr $C$DW$190, DW_AT_TI_symbol_name("_RBUF13")
	.dwattr $C$DW$190, DW_AT_data_member_location[DW_OP_plus_uconst 0x2b4]
	.dwattr $C$DW$190, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$191	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$191, DW_AT_name("RBUF14")
	.dwattr $C$DW$191, DW_AT_TI_symbol_name("_RBUF14")
	.dwattr $C$DW$191, DW_AT_data_member_location[DW_OP_plus_uconst 0x2b8]
	.dwattr $C$DW$191, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$192	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$192, DW_AT_name("RBUF15")
	.dwattr $C$DW$192, DW_AT_TI_symbol_name("_RBUF15")
	.dwattr $C$DW$192, DW_AT_data_member_location[DW_OP_plus_uconst 0x2bc]
	.dwattr $C$DW$192, DW_AT_accessibility(DW_ACCESS_public)
	.dwendtag $C$DW$T$27

$C$DW$T$28	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_McaspRegs")
	.dwattr $C$DW$T$28, DW_AT_type(*$C$DW$T$27)
	.dwattr $C$DW$T$28, DW_AT_language(DW_LANG_C)
$C$DW$T$29	.dwtag  DW_TAG_volatile_type
	.dwattr $C$DW$T$29, DW_AT_type(*$C$DW$T$28)
$C$DW$T$30	.dwtag  DW_TAG_pointer_type, DW_AT_type(*$C$DW$T$29)
	.dwattr $C$DW$T$30, DW_AT_address_class(0x20)
$C$DW$T$31	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_McaspRegsOvly")
	.dwattr $C$DW$T$31, DW_AT_type(*$C$DW$T$30)
	.dwattr $C$DW$T$31, DW_AT_language(DW_LANG_C)

$C$DW$T$35	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$35, DW_AT_name("CSL_McaspObj")
	.dwattr $C$DW$T$35, DW_AT_byte_size(0x10)
$C$DW$193	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$31)
	.dwattr $C$DW$193, DW_AT_name("regs")
	.dwattr $C$DW$193, DW_AT_TI_symbol_name("_regs")
	.dwattr $C$DW$193, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$193, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$194	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$32)
	.dwattr $C$DW$194, DW_AT_name("perNo")
	.dwattr $C$DW$194, DW_AT_TI_symbol_name("_perNo")
	.dwattr $C$DW$194, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$194, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$195	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$33)
	.dwattr $C$DW$195, DW_AT_name("numOfSerializers")
	.dwattr $C$DW$195, DW_AT_TI_symbol_name("_numOfSerializers")
	.dwattr $C$DW$195, DW_AT_data_member_location[DW_OP_plus_uconst 0x8]
	.dwattr $C$DW$195, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$196	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$34)
	.dwattr $C$DW$196, DW_AT_name("ditStatus")
	.dwattr $C$DW$196, DW_AT_TI_symbol_name("_ditStatus")
	.dwattr $C$DW$196, DW_AT_data_member_location[DW_OP_plus_uconst 0xc]
	.dwattr $C$DW$196, DW_AT_accessibility(DW_ACCESS_public)
	.dwendtag $C$DW$T$35

$C$DW$T$165	.dwtag  DW_TAG_pointer_type, DW_AT_type(*$C$DW$T$35)
	.dwattr $C$DW$T$165, DW_AT_address_class(0x20)
$C$DW$T$166	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_McaspHandle")
	.dwattr $C$DW$T$166, DW_AT_type(*$C$DW$T$165)
	.dwattr $C$DW$T$166, DW_AT_language(DW_LANG_C)
$C$DW$T$182	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_McaspObj")
	.dwattr $C$DW$T$182, DW_AT_type(*$C$DW$T$35)
	.dwattr $C$DW$T$182, DW_AT_language(DW_LANG_C)

$C$DW$T$37	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$37, DW_AT_name("CSL_McaspHwSetupGbl")
	.dwattr $C$DW$T$37, DW_AT_byte_size(0x58)
$C$DW$197	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$197, DW_AT_name("pfunc")
	.dwattr $C$DW$197, DW_AT_TI_symbol_name("_pfunc")
	.dwattr $C$DW$197, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$197, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$198	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$198, DW_AT_name("pdir")
	.dwattr $C$DW$198, DW_AT_TI_symbol_name("_pdir")
	.dwattr $C$DW$198, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$198, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$199	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$199, DW_AT_name("ctl")
	.dwattr $C$DW$199, DW_AT_TI_symbol_name("_ctl")
	.dwattr $C$DW$199, DW_AT_data_member_location[DW_OP_plus_uconst 0x8]
	.dwattr $C$DW$199, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$200	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$200, DW_AT_name("ditCtl")
	.dwattr $C$DW$200, DW_AT_TI_symbol_name("_ditCtl")
	.dwattr $C$DW$200, DW_AT_data_member_location[DW_OP_plus_uconst 0xc]
	.dwattr $C$DW$200, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$201	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$201, DW_AT_name("dlbMode")
	.dwattr $C$DW$201, DW_AT_TI_symbol_name("_dlbMode")
	.dwattr $C$DW$201, DW_AT_data_member_location[DW_OP_plus_uconst 0x10]
	.dwattr $C$DW$201, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$202	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$202, DW_AT_name("amute")
	.dwattr $C$DW$202, DW_AT_TI_symbol_name("_amute")
	.dwattr $C$DW$202, DW_AT_data_member_location[DW_OP_plus_uconst 0x14]
	.dwattr $C$DW$202, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$203	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$36)
	.dwattr $C$DW$203, DW_AT_name("serSetup")
	.dwattr $C$DW$203, DW_AT_TI_symbol_name("_serSetup")
	.dwattr $C$DW$203, DW_AT_data_member_location[DW_OP_plus_uconst 0x18]
	.dwattr $C$DW$203, DW_AT_accessibility(DW_ACCESS_public)
	.dwendtag $C$DW$T$37

$C$DW$T$41	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_McaspHwSetupGbl")
	.dwattr $C$DW$T$41, DW_AT_type(*$C$DW$T$37)
	.dwattr $C$DW$T$41, DW_AT_language(DW_LANG_C)

$C$DW$T$38	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$38, DW_AT_name("CSL_McaspHwSetupDataClk")
	.dwattr $C$DW$T$38, DW_AT_byte_size(0x0c)
$C$DW$204	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$204, DW_AT_name("clkSetupClk")
	.dwattr $C$DW$204, DW_AT_TI_symbol_name("_clkSetupClk")
	.dwattr $C$DW$204, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$204, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$205	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$205, DW_AT_name("clkSetupHiClk")
	.dwattr $C$DW$205, DW_AT_TI_symbol_name("_clkSetupHiClk")
	.dwattr $C$DW$205, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$205, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$206	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$206, DW_AT_name("clkChk")
	.dwattr $C$DW$206, DW_AT_TI_symbol_name("_clkChk")
	.dwattr $C$DW$206, DW_AT_data_member_location[DW_OP_plus_uconst 0x8]
	.dwattr $C$DW$206, DW_AT_accessibility(DW_ACCESS_public)
	.dwendtag $C$DW$T$38

$C$DW$T$39	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_McaspHwSetupDataClk")
	.dwattr $C$DW$T$39, DW_AT_type(*$C$DW$T$38)
	.dwattr $C$DW$T$39, DW_AT_language(DW_LANG_C)

$C$DW$T$40	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$40, DW_AT_name("CSL_McaspHwSetupData")
	.dwattr $C$DW$T$40, DW_AT_byte_size(0x28)
$C$DW$207	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$207, DW_AT_name("mask")
	.dwattr $C$DW$207, DW_AT_TI_symbol_name("_mask")
	.dwattr $C$DW$207, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$207, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$208	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$208, DW_AT_name("fmt")
	.dwattr $C$DW$208, DW_AT_TI_symbol_name("_fmt")
	.dwattr $C$DW$208, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$208, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$209	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$209, DW_AT_name("frSyncCtl")
	.dwattr $C$DW$209, DW_AT_TI_symbol_name("_frSyncCtl")
	.dwattr $C$DW$209, DW_AT_data_member_location[DW_OP_plus_uconst 0x8]
	.dwattr $C$DW$209, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$210	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$210, DW_AT_name("tdm")
	.dwattr $C$DW$210, DW_AT_TI_symbol_name("_tdm")
	.dwattr $C$DW$210, DW_AT_data_member_location[DW_OP_plus_uconst 0xc]
	.dwattr $C$DW$210, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$211	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$211, DW_AT_name("intCtl")
	.dwattr $C$DW$211, DW_AT_TI_symbol_name("_intCtl")
	.dwattr $C$DW$211, DW_AT_data_member_location[DW_OP_plus_uconst 0x10]
	.dwattr $C$DW$211, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$212	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$212, DW_AT_name("stat")
	.dwattr $C$DW$212, DW_AT_TI_symbol_name("_stat")
	.dwattr $C$DW$212, DW_AT_data_member_location[DW_OP_plus_uconst 0x14]
	.dwattr $C$DW$212, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$213	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$213, DW_AT_name("evtCtl")
	.dwattr $C$DW$213, DW_AT_TI_symbol_name("_evtCtl")
	.dwattr $C$DW$213, DW_AT_data_member_location[DW_OP_plus_uconst 0x18]
	.dwattr $C$DW$213, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$214	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$39)
	.dwattr $C$DW$214, DW_AT_name("clk")
	.dwattr $C$DW$214, DW_AT_TI_symbol_name("_clk")
	.dwattr $C$DW$214, DW_AT_data_member_location[DW_OP_plus_uconst 0x1c]
	.dwattr $C$DW$214, DW_AT_accessibility(DW_ACCESS_public)
	.dwendtag $C$DW$T$40

$C$DW$T$42	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_McaspHwSetupData")
	.dwattr $C$DW$T$42, DW_AT_type(*$C$DW$T$40)
	.dwattr $C$DW$T$42, DW_AT_language(DW_LANG_C)

$C$DW$T$43	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$43, DW_AT_name("CSL_McaspHwSetup")
	.dwattr $C$DW$T$43, DW_AT_byte_size(0xac)
$C$DW$215	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$41)
	.dwattr $C$DW$215, DW_AT_name("glb")
	.dwattr $C$DW$215, DW_AT_TI_symbol_name("_glb")
	.dwattr $C$DW$215, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$215, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$216	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$42)
	.dwattr $C$DW$216, DW_AT_name("rx")
	.dwattr $C$DW$216, DW_AT_TI_symbol_name("_rx")
	.dwattr $C$DW$216, DW_AT_data_member_location[DW_OP_plus_uconst 0x58]
	.dwattr $C$DW$216, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$217	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$42)
	.dwattr $C$DW$217, DW_AT_name("tx")
	.dwattr $C$DW$217, DW_AT_TI_symbol_name("_tx")
	.dwattr $C$DW$217, DW_AT_data_member_location[DW_OP_plus_uconst 0x80]
	.dwattr $C$DW$217, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$218	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$218, DW_AT_name("emu")
	.dwattr $C$DW$218, DW_AT_TI_symbol_name("_emu")
	.dwattr $C$DW$218, DW_AT_data_member_location[DW_OP_plus_uconst 0xa8]
	.dwattr $C$DW$218, DW_AT_accessibility(DW_ACCESS_public)
	.dwendtag $C$DW$T$43

$C$DW$T$167	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_McaspHwSetup")
	.dwattr $C$DW$T$167, DW_AT_type(*$C$DW$T$43)
	.dwattr $C$DW$T$167, DW_AT_language(DW_LANG_C)
$C$DW$T$168	.dwtag  DW_TAG_pointer_type, DW_AT_type(*$C$DW$T$167)
	.dwattr $C$DW$T$168, DW_AT_address_class(0x20)

$C$DW$T$44	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$44, DW_AT_byte_size(0x34)
$C$DW$219	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$219, DW_AT_name("RBR")
	.dwattr $C$DW$219, DW_AT_TI_symbol_name("_RBR")
	.dwattr $C$DW$219, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$219, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$220	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$220, DW_AT_name("IER")
	.dwattr $C$DW$220, DW_AT_TI_symbol_name("_IER")
	.dwattr $C$DW$220, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$220, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$221	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$221, DW_AT_name("IIR")
	.dwattr $C$DW$221, DW_AT_TI_symbol_name("_IIR")
	.dwattr $C$DW$221, DW_AT_data_member_location[DW_OP_plus_uconst 0x8]
	.dwattr $C$DW$221, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$222	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$222, DW_AT_name("LCR")
	.dwattr $C$DW$222, DW_AT_TI_symbol_name("_LCR")
	.dwattr $C$DW$222, DW_AT_data_member_location[DW_OP_plus_uconst 0xc]
	.dwattr $C$DW$222, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$223	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$223, DW_AT_name("MCR")
	.dwattr $C$DW$223, DW_AT_TI_symbol_name("_MCR")
	.dwattr $C$DW$223, DW_AT_data_member_location[DW_OP_plus_uconst 0x10]
	.dwattr $C$DW$223, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$224	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$224, DW_AT_name("LSR")
	.dwattr $C$DW$224, DW_AT_TI_symbol_name("_LSR")
	.dwattr $C$DW$224, DW_AT_data_member_location[DW_OP_plus_uconst 0x14]
	.dwattr $C$DW$224, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$225	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$225, DW_AT_name("MSR")
	.dwattr $C$DW$225, DW_AT_TI_symbol_name("_MSR")
	.dwattr $C$DW$225, DW_AT_data_member_location[DW_OP_plus_uconst 0x18]
	.dwattr $C$DW$225, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$226	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$226, DW_AT_name("SCR")
	.dwattr $C$DW$226, DW_AT_TI_symbol_name("_SCR")
	.dwattr $C$DW$226, DW_AT_data_member_location[DW_OP_plus_uconst 0x1c]
	.dwattr $C$DW$226, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$227	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$227, DW_AT_name("DLL")
	.dwattr $C$DW$227, DW_AT_TI_symbol_name("_DLL")
	.dwattr $C$DW$227, DW_AT_data_member_location[DW_OP_plus_uconst 0x20]
	.dwattr $C$DW$227, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$228	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$228, DW_AT_name("DLH")
	.dwattr $C$DW$228, DW_AT_TI_symbol_name("_DLH")
	.dwattr $C$DW$228, DW_AT_data_member_location[DW_OP_plus_uconst 0x24]
	.dwattr $C$DW$228, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$229	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$229, DW_AT_name("PID1")
	.dwattr $C$DW$229, DW_AT_TI_symbol_name("_PID1")
	.dwattr $C$DW$229, DW_AT_data_member_location[DW_OP_plus_uconst 0x28]
	.dwattr $C$DW$229, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$230	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$230, DW_AT_name("PID2")
	.dwattr $C$DW$230, DW_AT_TI_symbol_name("_PID2")
	.dwattr $C$DW$230, DW_AT_data_member_location[DW_OP_plus_uconst 0x2c]
	.dwattr $C$DW$230, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$231	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$231, DW_AT_name("PWREMU_MGMT")
	.dwattr $C$DW$231, DW_AT_TI_symbol_name("_PWREMU_MGMT")
	.dwattr $C$DW$231, DW_AT_data_member_location[DW_OP_plus_uconst 0x30]
	.dwattr $C$DW$231, DW_AT_accessibility(DW_ACCESS_public)
	.dwendtag $C$DW$T$44

$C$DW$T$193	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_UartRegs")
	.dwattr $C$DW$T$193, DW_AT_type(*$C$DW$T$44)
	.dwattr $C$DW$T$193, DW_AT_language(DW_LANG_C)
$C$DW$T$194	.dwtag  DW_TAG_volatile_type
	.dwattr $C$DW$T$194, DW_AT_type(*$C$DW$T$193)
$C$DW$T$195	.dwtag  DW_TAG_pointer_type, DW_AT_type(*$C$DW$T$194)
	.dwattr $C$DW$T$195, DW_AT_address_class(0x20)
$C$DW$T$196	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_UartRegsOvly")
	.dwattr $C$DW$T$196, DW_AT_type(*$C$DW$T$195)
	.dwattr $C$DW$T$196, DW_AT_language(DW_LANG_C)

$C$DW$T$51	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$51, DW_AT_byte_size(0x60)
$C$DW$232	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$46)
	.dwattr $C$DW$232, DW_AT_name("ICOAR")
	.dwattr $C$DW$232, DW_AT_TI_symbol_name("_ICOAR")
	.dwattr $C$DW$232, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$232, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$233	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$49)
	.dwattr $C$DW$233, DW_AT_name("RSVD0")
	.dwattr $C$DW$233, DW_AT_TI_symbol_name("_RSVD0")
	.dwattr $C$DW$233, DW_AT_data_member_location[DW_OP_plus_uconst 0x2]
	.dwattr $C$DW$233, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$234	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$46)
	.dwattr $C$DW$234, DW_AT_name("ICIMR")
	.dwattr $C$DW$234, DW_AT_TI_symbol_name("_ICIMR")
	.dwattr $C$DW$234, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$234, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$235	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$49)
	.dwattr $C$DW$235, DW_AT_name("RSVD1")
	.dwattr $C$DW$235, DW_AT_TI_symbol_name("_RSVD1")
	.dwattr $C$DW$235, DW_AT_data_member_location[DW_OP_plus_uconst 0x6]
	.dwattr $C$DW$235, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$236	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$46)
	.dwattr $C$DW$236, DW_AT_name("ICSTR")
	.dwattr $C$DW$236, DW_AT_TI_symbol_name("_ICSTR")
	.dwattr $C$DW$236, DW_AT_data_member_location[DW_OP_plus_uconst 0x8]
	.dwattr $C$DW$236, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$237	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$49)
	.dwattr $C$DW$237, DW_AT_name("RSVD2")
	.dwattr $C$DW$237, DW_AT_TI_symbol_name("_RSVD2")
	.dwattr $C$DW$237, DW_AT_data_member_location[DW_OP_plus_uconst 0xa]
	.dwattr $C$DW$237, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$238	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$46)
	.dwattr $C$DW$238, DW_AT_name("ICCLKL")
	.dwattr $C$DW$238, DW_AT_TI_symbol_name("_ICCLKL")
	.dwattr $C$DW$238, DW_AT_data_member_location[DW_OP_plus_uconst 0xc]
	.dwattr $C$DW$238, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$239	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$49)
	.dwattr $C$DW$239, DW_AT_name("RSVD3")
	.dwattr $C$DW$239, DW_AT_TI_symbol_name("_RSVD3")
	.dwattr $C$DW$239, DW_AT_data_member_location[DW_OP_plus_uconst 0xe]
	.dwattr $C$DW$239, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$240	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$46)
	.dwattr $C$DW$240, DW_AT_name("ICCLKH")
	.dwattr $C$DW$240, DW_AT_TI_symbol_name("_ICCLKH")
	.dwattr $C$DW$240, DW_AT_data_member_location[DW_OP_plus_uconst 0x10]
	.dwattr $C$DW$240, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$241	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$49)
	.dwattr $C$DW$241, DW_AT_name("RSVD4")
	.dwattr $C$DW$241, DW_AT_TI_symbol_name("_RSVD4")
	.dwattr $C$DW$241, DW_AT_data_member_location[DW_OP_plus_uconst 0x12]
	.dwattr $C$DW$241, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$242	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$46)
	.dwattr $C$DW$242, DW_AT_name("ICCNT")
	.dwattr $C$DW$242, DW_AT_TI_symbol_name("_ICCNT")
	.dwattr $C$DW$242, DW_AT_data_member_location[DW_OP_plus_uconst 0x14]
	.dwattr $C$DW$242, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$243	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$49)
	.dwattr $C$DW$243, DW_AT_name("RSVD5")
	.dwattr $C$DW$243, DW_AT_TI_symbol_name("_RSVD5")
	.dwattr $C$DW$243, DW_AT_data_member_location[DW_OP_plus_uconst 0x16]
	.dwattr $C$DW$243, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$244	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$46)
	.dwattr $C$DW$244, DW_AT_name("ICDRR")
	.dwattr $C$DW$244, DW_AT_TI_symbol_name("_ICDRR")
	.dwattr $C$DW$244, DW_AT_data_member_location[DW_OP_plus_uconst 0x18]
	.dwattr $C$DW$244, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$245	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$49)
	.dwattr $C$DW$245, DW_AT_name("RSVD6")
	.dwattr $C$DW$245, DW_AT_TI_symbol_name("_RSVD6")
	.dwattr $C$DW$245, DW_AT_data_member_location[DW_OP_plus_uconst 0x1a]
	.dwattr $C$DW$245, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$246	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$46)
	.dwattr $C$DW$246, DW_AT_name("ICSAR")
	.dwattr $C$DW$246, DW_AT_TI_symbol_name("_ICSAR")
	.dwattr $C$DW$246, DW_AT_data_member_location[DW_OP_plus_uconst 0x1c]
	.dwattr $C$DW$246, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$247	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$49)
	.dwattr $C$DW$247, DW_AT_name("RSVD7")
	.dwattr $C$DW$247, DW_AT_TI_symbol_name("_RSVD7")
	.dwattr $C$DW$247, DW_AT_data_member_location[DW_OP_plus_uconst 0x1e]
	.dwattr $C$DW$247, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$248	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$46)
	.dwattr $C$DW$248, DW_AT_name("ICDXR")
	.dwattr $C$DW$248, DW_AT_TI_symbol_name("_ICDXR")
	.dwattr $C$DW$248, DW_AT_data_member_location[DW_OP_plus_uconst 0x20]
	.dwattr $C$DW$248, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$249	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$49)
	.dwattr $C$DW$249, DW_AT_name("RSVD8")
	.dwattr $C$DW$249, DW_AT_TI_symbol_name("_RSVD8")
	.dwattr $C$DW$249, DW_AT_data_member_location[DW_OP_plus_uconst 0x22]
	.dwattr $C$DW$249, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$250	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$46)
	.dwattr $C$DW$250, DW_AT_name("ICMDR")
	.dwattr $C$DW$250, DW_AT_TI_symbol_name("_ICMDR")
	.dwattr $C$DW$250, DW_AT_data_member_location[DW_OP_plus_uconst 0x24]
	.dwattr $C$DW$250, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$251	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$49)
	.dwattr $C$DW$251, DW_AT_name("RSVD9")
	.dwattr $C$DW$251, DW_AT_TI_symbol_name("_RSVD9")
	.dwattr $C$DW$251, DW_AT_data_member_location[DW_OP_plus_uconst 0x26]
	.dwattr $C$DW$251, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$252	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$46)
	.dwattr $C$DW$252, DW_AT_name("ICIVR")
	.dwattr $C$DW$252, DW_AT_TI_symbol_name("_ICIVR")
	.dwattr $C$DW$252, DW_AT_data_member_location[DW_OP_plus_uconst 0x28]
	.dwattr $C$DW$252, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$253	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$49)
	.dwattr $C$DW$253, DW_AT_name("RSVD10")
	.dwattr $C$DW$253, DW_AT_TI_symbol_name("_RSVD10")
	.dwattr $C$DW$253, DW_AT_data_member_location[DW_OP_plus_uconst 0x2a]
	.dwattr $C$DW$253, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$254	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$46)
	.dwattr $C$DW$254, DW_AT_name("ICEMDR")
	.dwattr $C$DW$254, DW_AT_TI_symbol_name("_ICEMDR")
	.dwattr $C$DW$254, DW_AT_data_member_location[DW_OP_plus_uconst 0x2c]
	.dwattr $C$DW$254, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$255	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$49)
	.dwattr $C$DW$255, DW_AT_name("RSVD11")
	.dwattr $C$DW$255, DW_AT_TI_symbol_name("_RSVD11")
	.dwattr $C$DW$255, DW_AT_data_member_location[DW_OP_plus_uconst 0x2e]
	.dwattr $C$DW$255, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$256	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$46)
	.dwattr $C$DW$256, DW_AT_name("ICPSC")
	.dwattr $C$DW$256, DW_AT_TI_symbol_name("_ICPSC")
	.dwattr $C$DW$256, DW_AT_data_member_location[DW_OP_plus_uconst 0x30]
	.dwattr $C$DW$256, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$257	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$49)
	.dwattr $C$DW$257, DW_AT_name("RSVD12")
	.dwattr $C$DW$257, DW_AT_TI_symbol_name("_RSVD12")
	.dwattr $C$DW$257, DW_AT_data_member_location[DW_OP_plus_uconst 0x32]
	.dwattr $C$DW$257, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$258	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$46)
	.dwattr $C$DW$258, DW_AT_name("ICPID1")
	.dwattr $C$DW$258, DW_AT_TI_symbol_name("_ICPID1")
	.dwattr $C$DW$258, DW_AT_data_member_location[DW_OP_plus_uconst 0x34]
	.dwattr $C$DW$258, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$259	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$49)
	.dwattr $C$DW$259, DW_AT_name("RSVD13")
	.dwattr $C$DW$259, DW_AT_TI_symbol_name("_RSVD13")
	.dwattr $C$DW$259, DW_AT_data_member_location[DW_OP_plus_uconst 0x36]
	.dwattr $C$DW$259, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$260	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$46)
	.dwattr $C$DW$260, DW_AT_name("ICPID2")
	.dwattr $C$DW$260, DW_AT_TI_symbol_name("_ICPID2")
	.dwattr $C$DW$260, DW_AT_data_member_location[DW_OP_plus_uconst 0x38]
	.dwattr $C$DW$260, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$261	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$50)
	.dwattr $C$DW$261, DW_AT_name("RSVD14")
	.dwattr $C$DW$261, DW_AT_TI_symbol_name("_RSVD14")
	.dwattr $C$DW$261, DW_AT_data_member_location[DW_OP_plus_uconst 0x3a]
	.dwattr $C$DW$261, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$262	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$262, DW_AT_name("ICPFUNC")
	.dwattr $C$DW$262, DW_AT_TI_symbol_name("_ICPFUNC")
	.dwattr $C$DW$262, DW_AT_data_member_location[DW_OP_plus_uconst 0x48]
	.dwattr $C$DW$262, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$263	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$263, DW_AT_name("ICPDIR")
	.dwattr $C$DW$263, DW_AT_TI_symbol_name("_ICPDIR")
	.dwattr $C$DW$263, DW_AT_data_member_location[DW_OP_plus_uconst 0x4c]
	.dwattr $C$DW$263, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$264	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$264, DW_AT_name("ICPDIN")
	.dwattr $C$DW$264, DW_AT_TI_symbol_name("_ICPDIN")
	.dwattr $C$DW$264, DW_AT_data_member_location[DW_OP_plus_uconst 0x50]
	.dwattr $C$DW$264, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$265	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$265, DW_AT_name("ICPDOUT")
	.dwattr $C$DW$265, DW_AT_TI_symbol_name("_ICPDOUT")
	.dwattr $C$DW$265, DW_AT_data_member_location[DW_OP_plus_uconst 0x54]
	.dwattr $C$DW$265, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$266	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$266, DW_AT_name("ICPDSET")
	.dwattr $C$DW$266, DW_AT_TI_symbol_name("_ICPDSET")
	.dwattr $C$DW$266, DW_AT_data_member_location[DW_OP_plus_uconst 0x58]
	.dwattr $C$DW$266, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$267	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$267, DW_AT_name("ICPDCLR")
	.dwattr $C$DW$267, DW_AT_TI_symbol_name("_ICPDCLR")
	.dwattr $C$DW$267, DW_AT_data_member_location[DW_OP_plus_uconst 0x5c]
	.dwattr $C$DW$267, DW_AT_accessibility(DW_ACCESS_public)
	.dwendtag $C$DW$T$51

$C$DW$T$197	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_I2cRegs")
	.dwattr $C$DW$T$197, DW_AT_type(*$C$DW$T$51)
	.dwattr $C$DW$T$197, DW_AT_language(DW_LANG_C)
$C$DW$T$198	.dwtag  DW_TAG_volatile_type
	.dwattr $C$DW$T$198, DW_AT_type(*$C$DW$T$197)
$C$DW$T$199	.dwtag  DW_TAG_pointer_type, DW_AT_type(*$C$DW$T$198)
	.dwattr $C$DW$T$199, DW_AT_address_class(0x20)
$C$DW$T$200	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_I2cRegsOvly")
	.dwattr $C$DW$T$200, DW_AT_type(*$C$DW$T$199)
	.dwattr $C$DW$T$200, DW_AT_language(DW_LANG_C)

$C$DW$T$52	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$52, DW_AT_byte_size(0x1c)
$C$DW$268	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$268, DW_AT_name("PID")
	.dwattr $C$DW$268, DW_AT_TI_symbol_name("_PID")
	.dwattr $C$DW$268, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$268, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$269	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$269, DW_AT_name("PCR")
	.dwattr $C$DW$269, DW_AT_TI_symbol_name("_PCR")
	.dwattr $C$DW$269, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$269, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$270	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$270, DW_AT_name("CFG")
	.dwattr $C$DW$270, DW_AT_TI_symbol_name("_CFG")
	.dwattr $C$DW$270, DW_AT_data_member_location[DW_OP_plus_uconst 0x8]
	.dwattr $C$DW$270, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$271	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$271, DW_AT_name("START")
	.dwattr $C$DW$271, DW_AT_TI_symbol_name("_START")
	.dwattr $C$DW$271, DW_AT_data_member_location[DW_OP_plus_uconst 0xc]
	.dwattr $C$DW$271, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$272	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$272, DW_AT_name("RPT")
	.dwattr $C$DW$272, DW_AT_TI_symbol_name("_RPT")
	.dwattr $C$DW$272, DW_AT_data_member_location[DW_OP_plus_uconst 0x10]
	.dwattr $C$DW$272, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$273	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$273, DW_AT_name("PER")
	.dwattr $C$DW$273, DW_AT_TI_symbol_name("_PER")
	.dwattr $C$DW$273, DW_AT_data_member_location[DW_OP_plus_uconst 0x14]
	.dwattr $C$DW$273, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$274	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$274, DW_AT_name("PH1D")
	.dwattr $C$DW$274, DW_AT_TI_symbol_name("_PH1D")
	.dwattr $C$DW$274, DW_AT_data_member_location[DW_OP_plus_uconst 0x18]
	.dwattr $C$DW$274, DW_AT_accessibility(DW_ACCESS_public)
	.dwendtag $C$DW$T$52

$C$DW$T$201	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_PwmRegs")
	.dwattr $C$DW$T$201, DW_AT_type(*$C$DW$T$52)
	.dwattr $C$DW$T$201, DW_AT_language(DW_LANG_C)
$C$DW$T$202	.dwtag  DW_TAG_volatile_type
	.dwattr $C$DW$T$202, DW_AT_type(*$C$DW$T$201)
$C$DW$T$203	.dwtag  DW_TAG_pointer_type, DW_AT_type(*$C$DW$T$202)
	.dwattr $C$DW$T$203, DW_AT_address_class(0x20)
$C$DW$T$204	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_PwmRegsOvly")
	.dwattr $C$DW$T$204, DW_AT_type(*$C$DW$T$203)
	.dwattr $C$DW$T$204, DW_AT_language(DW_LANG_C)

$C$DW$T$57	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$57, DW_AT_byte_size(0x174)
$C$DW$275	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$275, DW_AT_name("PID")
	.dwattr $C$DW$275, DW_AT_TI_symbol_name("_PID")
	.dwattr $C$DW$275, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$275, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$276	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$53)
	.dwattr $C$DW$276, DW_AT_name("RSVD0")
	.dwattr $C$DW$276, DW_AT_TI_symbol_name("_RSVD0")
	.dwattr $C$DW$276, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$276, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$277	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$277, DW_AT_name("FUSERR")
	.dwattr $C$DW$277, DW_AT_TI_symbol_name("_FUSERR")
	.dwattr $C$DW$277, DW_AT_data_member_location[DW_OP_plus_uconst 0xe0]
	.dwattr $C$DW$277, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$278	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$278, DW_AT_name("RSTYPE")
	.dwattr $C$DW$278, DW_AT_TI_symbol_name("_RSTYPE")
	.dwattr $C$DW$278, DW_AT_data_member_location[DW_OP_plus_uconst 0xe4]
	.dwattr $C$DW$278, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$279	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$279, DW_AT_name("RSTDEF")
	.dwattr $C$DW$279, DW_AT_TI_symbol_name("_RSTDEF")
	.dwattr $C$DW$279, DW_AT_data_member_location[DW_OP_plus_uconst 0xe8]
	.dwattr $C$DW$279, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$280	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$54)
	.dwattr $C$DW$280, DW_AT_name("RSVD1")
	.dwattr $C$DW$280, DW_AT_TI_symbol_name("_RSVD1")
	.dwattr $C$DW$280, DW_AT_data_member_location[DW_OP_plus_uconst 0xec]
	.dwattr $C$DW$280, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$281	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$281, DW_AT_name("PLLCTL")
	.dwattr $C$DW$281, DW_AT_TI_symbol_name("_PLLCTL")
	.dwattr $C$DW$281, DW_AT_data_member_location[DW_OP_plus_uconst 0x100]
	.dwattr $C$DW$281, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$282	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$282, DW_AT_name("OCSEL")
	.dwattr $C$DW$282, DW_AT_TI_symbol_name("_OCSEL")
	.dwattr $C$DW$282, DW_AT_data_member_location[DW_OP_plus_uconst 0x104]
	.dwattr $C$DW$282, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$283	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$283, DW_AT_name("SECCTL")
	.dwattr $C$DW$283, DW_AT_TI_symbol_name("_SECCTL")
	.dwattr $C$DW$283, DW_AT_data_member_location[DW_OP_plus_uconst 0x108]
	.dwattr $C$DW$283, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$284	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$55)
	.dwattr $C$DW$284, DW_AT_name("RSVD2")
	.dwattr $C$DW$284, DW_AT_TI_symbol_name("_RSVD2")
	.dwattr $C$DW$284, DW_AT_data_member_location[DW_OP_plus_uconst 0x10c]
	.dwattr $C$DW$284, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$285	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$285, DW_AT_name("PLLM")
	.dwattr $C$DW$285, DW_AT_TI_symbol_name("_PLLM")
	.dwattr $C$DW$285, DW_AT_data_member_location[DW_OP_plus_uconst 0x110]
	.dwattr $C$DW$285, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$286	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$286, DW_AT_name("PREDIV")
	.dwattr $C$DW$286, DW_AT_TI_symbol_name("_PREDIV")
	.dwattr $C$DW$286, DW_AT_data_member_location[DW_OP_plus_uconst 0x114]
	.dwattr $C$DW$286, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$287	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$287, DW_AT_name("PLLDIV1")
	.dwattr $C$DW$287, DW_AT_TI_symbol_name("_PLLDIV1")
	.dwattr $C$DW$287, DW_AT_data_member_location[DW_OP_plus_uconst 0x118]
	.dwattr $C$DW$287, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$288	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$288, DW_AT_name("PLLDIV2")
	.dwattr $C$DW$288, DW_AT_TI_symbol_name("_PLLDIV2")
	.dwattr $C$DW$288, DW_AT_data_member_location[DW_OP_plus_uconst 0x11c]
	.dwattr $C$DW$288, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$289	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$289, DW_AT_name("PLLDIV3")
	.dwattr $C$DW$289, DW_AT_TI_symbol_name("_PLLDIV3")
	.dwattr $C$DW$289, DW_AT_data_member_location[DW_OP_plus_uconst 0x120]
	.dwattr $C$DW$289, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$290	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$290, DW_AT_name("OSCDIV1")
	.dwattr $C$DW$290, DW_AT_TI_symbol_name("_OSCDIV1")
	.dwattr $C$DW$290, DW_AT_data_member_location[DW_OP_plus_uconst 0x124]
	.dwattr $C$DW$290, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$291	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$291, DW_AT_name("POSTDIV")
	.dwattr $C$DW$291, DW_AT_TI_symbol_name("_POSTDIV")
	.dwattr $C$DW$291, DW_AT_data_member_location[DW_OP_plus_uconst 0x128]
	.dwattr $C$DW$291, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$292	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$292, DW_AT_name("BPDIV")
	.dwattr $C$DW$292, DW_AT_TI_symbol_name("_BPDIV")
	.dwattr $C$DW$292, DW_AT_data_member_location[DW_OP_plus_uconst 0x12c]
	.dwattr $C$DW$292, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$293	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$293, DW_AT_name("WAKEUP")
	.dwattr $C$DW$293, DW_AT_TI_symbol_name("_WAKEUP")
	.dwattr $C$DW$293, DW_AT_data_member_location[DW_OP_plus_uconst 0x130]
	.dwattr $C$DW$293, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$294	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$55)
	.dwattr $C$DW$294, DW_AT_name("RSVD3")
	.dwattr $C$DW$294, DW_AT_TI_symbol_name("_RSVD3")
	.dwattr $C$DW$294, DW_AT_data_member_location[DW_OP_plus_uconst 0x134]
	.dwattr $C$DW$294, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$295	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$295, DW_AT_name("PLLCMD")
	.dwattr $C$DW$295, DW_AT_TI_symbol_name("_PLLCMD")
	.dwattr $C$DW$295, DW_AT_data_member_location[DW_OP_plus_uconst 0x138]
	.dwattr $C$DW$295, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$296	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$296, DW_AT_name("PLLSTAT")
	.dwattr $C$DW$296, DW_AT_TI_symbol_name("_PLLSTAT")
	.dwattr $C$DW$296, DW_AT_data_member_location[DW_OP_plus_uconst 0x13c]
	.dwattr $C$DW$296, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$297	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$297, DW_AT_name("ALNCTL")
	.dwattr $C$DW$297, DW_AT_TI_symbol_name("_ALNCTL")
	.dwattr $C$DW$297, DW_AT_data_member_location[DW_OP_plus_uconst 0x140]
	.dwattr $C$DW$297, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$298	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$298, DW_AT_name("DCHANGE")
	.dwattr $C$DW$298, DW_AT_TI_symbol_name("_DCHANGE")
	.dwattr $C$DW$298, DW_AT_data_member_location[DW_OP_plus_uconst 0x144]
	.dwattr $C$DW$298, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$299	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$299, DW_AT_name("CKEN")
	.dwattr $C$DW$299, DW_AT_TI_symbol_name("_CKEN")
	.dwattr $C$DW$299, DW_AT_data_member_location[DW_OP_plus_uconst 0x148]
	.dwattr $C$DW$299, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$300	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$300, DW_AT_name("CKSTAT")
	.dwattr $C$DW$300, DW_AT_TI_symbol_name("_CKSTAT")
	.dwattr $C$DW$300, DW_AT_data_member_location[DW_OP_plus_uconst 0x14c]
	.dwattr $C$DW$300, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$301	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$301, DW_AT_name("SYSTAT")
	.dwattr $C$DW$301, DW_AT_TI_symbol_name("_SYSTAT")
	.dwattr $C$DW$301, DW_AT_data_member_location[DW_OP_plus_uconst 0x150]
	.dwattr $C$DW$301, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$302	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$56)
	.dwattr $C$DW$302, DW_AT_name("RSVD4")
	.dwattr $C$DW$302, DW_AT_TI_symbol_name("_RSVD4")
	.dwattr $C$DW$302, DW_AT_data_member_location[DW_OP_plus_uconst 0x154]
	.dwattr $C$DW$302, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$303	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$303, DW_AT_name("PLLDIV4")
	.dwattr $C$DW$303, DW_AT_TI_symbol_name("_PLLDIV4")
	.dwattr $C$DW$303, DW_AT_data_member_location[DW_OP_plus_uconst 0x160]
	.dwattr $C$DW$303, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$304	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$304, DW_AT_name("PLLDIV5")
	.dwattr $C$DW$304, DW_AT_TI_symbol_name("_PLLDIV5")
	.dwattr $C$DW$304, DW_AT_data_member_location[DW_OP_plus_uconst 0x164]
	.dwattr $C$DW$304, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$305	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$305, DW_AT_name("PLLDIV6")
	.dwattr $C$DW$305, DW_AT_TI_symbol_name("_PLLDIV6")
	.dwattr $C$DW$305, DW_AT_data_member_location[DW_OP_plus_uconst 0x168]
	.dwattr $C$DW$305, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$306	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$306, DW_AT_name("PLLDIV7")
	.dwattr $C$DW$306, DW_AT_TI_symbol_name("_PLLDIV7")
	.dwattr $C$DW$306, DW_AT_data_member_location[DW_OP_plus_uconst 0x16c]
	.dwattr $C$DW$306, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$307	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$307, DW_AT_name("PLLDIV8")
	.dwattr $C$DW$307, DW_AT_TI_symbol_name("_PLLDIV8")
	.dwattr $C$DW$307, DW_AT_data_member_location[DW_OP_plus_uconst 0x170]
	.dwattr $C$DW$307, DW_AT_accessibility(DW_ACCESS_public)
	.dwendtag $C$DW$T$57

$C$DW$T$205	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_PllcRegs")
	.dwattr $C$DW$T$205, DW_AT_type(*$C$DW$T$57)
	.dwattr $C$DW$T$205, DW_AT_language(DW_LANG_C)
$C$DW$T$206	.dwtag  DW_TAG_volatile_type
	.dwattr $C$DW$T$206, DW_AT_type(*$C$DW$T$205)
$C$DW$T$207	.dwtag  DW_TAG_pointer_type, DW_AT_type(*$C$DW$T$206)
	.dwattr $C$DW$T$207, DW_AT_address_class(0x20)
$C$DW$T$208	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_PllcRegsOvly")
	.dwattr $C$DW$T$208, DW_AT_type(*$C$DW$T$207)
	.dwattr $C$DW$T$208, DW_AT_language(DW_LANG_C)

$C$DW$T$58	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$58, DW_AT_byte_size(0x28)
$C$DW$308	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$308, DW_AT_name("DIR")
	.dwattr $C$DW$308, DW_AT_TI_symbol_name("_DIR")
	.dwattr $C$DW$308, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$308, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$309	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$309, DW_AT_name("OUT_DATA")
	.dwattr $C$DW$309, DW_AT_TI_symbol_name("_OUT_DATA")
	.dwattr $C$DW$309, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$309, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$310	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$310, DW_AT_name("SET_DATA")
	.dwattr $C$DW$310, DW_AT_TI_symbol_name("_SET_DATA")
	.dwattr $C$DW$310, DW_AT_data_member_location[DW_OP_plus_uconst 0x8]
	.dwattr $C$DW$310, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$311	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$311, DW_AT_name("CLR_DATA")
	.dwattr $C$DW$311, DW_AT_TI_symbol_name("_CLR_DATA")
	.dwattr $C$DW$311, DW_AT_data_member_location[DW_OP_plus_uconst 0xc]
	.dwattr $C$DW$311, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$312	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$312, DW_AT_name("IN_DATA")
	.dwattr $C$DW$312, DW_AT_TI_symbol_name("_IN_DATA")
	.dwattr $C$DW$312, DW_AT_data_member_location[DW_OP_plus_uconst 0x10]
	.dwattr $C$DW$312, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$313	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$313, DW_AT_name("SET_RIS_TRIG")
	.dwattr $C$DW$313, DW_AT_TI_symbol_name("_SET_RIS_TRIG")
	.dwattr $C$DW$313, DW_AT_data_member_location[DW_OP_plus_uconst 0x14]
	.dwattr $C$DW$313, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$314	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$314, DW_AT_name("CLR_RIS_TRIG")
	.dwattr $C$DW$314, DW_AT_TI_symbol_name("_CLR_RIS_TRIG")
	.dwattr $C$DW$314, DW_AT_data_member_location[DW_OP_plus_uconst 0x18]
	.dwattr $C$DW$314, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$315	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$315, DW_AT_name("SET_FAL_TRIG")
	.dwattr $C$DW$315, DW_AT_TI_symbol_name("_SET_FAL_TRIG")
	.dwattr $C$DW$315, DW_AT_data_member_location[DW_OP_plus_uconst 0x1c]
	.dwattr $C$DW$315, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$316	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$316, DW_AT_name("CLR_FAL_TRIG")
	.dwattr $C$DW$316, DW_AT_TI_symbol_name("_CLR_FAL_TRIG")
	.dwattr $C$DW$316, DW_AT_data_member_location[DW_OP_plus_uconst 0x20]
	.dwattr $C$DW$316, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$317	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$317, DW_AT_name("INTSTAT")
	.dwattr $C$DW$317, DW_AT_TI_symbol_name("_INTSTAT")
	.dwattr $C$DW$317, DW_AT_data_member_location[DW_OP_plus_uconst 0x24]
	.dwattr $C$DW$317, DW_AT_accessibility(DW_ACCESS_public)
	.dwendtag $C$DW$T$58

$C$DW$T$59	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_GpioBankRegs")
	.dwattr $C$DW$T$59, DW_AT_type(*$C$DW$T$58)
	.dwattr $C$DW$T$59, DW_AT_language(DW_LANG_C)

$C$DW$T$60	.dwtag  DW_TAG_array_type, DW_AT_type(*$C$DW$T$59)
	.dwattr $C$DW$T$60, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$60, DW_AT_byte_size(0x78)
$C$DW$318	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$318, DW_AT_upper_bound(0x02)
	.dwendtag $C$DW$T$60


$C$DW$T$61	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$61, DW_AT_byte_size(0x88)
$C$DW$319	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$319, DW_AT_name("PID")
	.dwattr $C$DW$319, DW_AT_TI_symbol_name("_PID")
	.dwattr $C$DW$319, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$319, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$320	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$320, DW_AT_name("PCR")
	.dwattr $C$DW$320, DW_AT_TI_symbol_name("_PCR")
	.dwattr $C$DW$320, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$320, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$321	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$321, DW_AT_name("BINTEN")
	.dwattr $C$DW$321, DW_AT_TI_symbol_name("_BINTEN")
	.dwattr $C$DW$321, DW_AT_data_member_location[DW_OP_plus_uconst 0x8]
	.dwattr $C$DW$321, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$322	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$55)
	.dwattr $C$DW$322, DW_AT_name("RSVD0")
	.dwattr $C$DW$322, DW_AT_TI_symbol_name("_RSVD0")
	.dwattr $C$DW$322, DW_AT_data_member_location[DW_OP_plus_uconst 0xc]
	.dwattr $C$DW$322, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$323	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$60)
	.dwattr $C$DW$323, DW_AT_name("BANK")
	.dwattr $C$DW$323, DW_AT_TI_symbol_name("_BANK")
	.dwattr $C$DW$323, DW_AT_data_member_location[DW_OP_plus_uconst 0x10]
	.dwattr $C$DW$323, DW_AT_accessibility(DW_ACCESS_public)
	.dwendtag $C$DW$T$61

$C$DW$T$209	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_GpioRegs")
	.dwattr $C$DW$T$209, DW_AT_type(*$C$DW$T$61)
	.dwattr $C$DW$T$209, DW_AT_language(DW_LANG_C)
$C$DW$T$210	.dwtag  DW_TAG_volatile_type
	.dwattr $C$DW$T$210, DW_AT_type(*$C$DW$T$209)
$C$DW$T$211	.dwtag  DW_TAG_pointer_type, DW_AT_type(*$C$DW$T$210)
	.dwattr $C$DW$T$211, DW_AT_address_class(0x20)
$C$DW$T$212	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_GpioRegsOvly")
	.dwattr $C$DW$T$212, DW_AT_type(*$C$DW$T$211)
	.dwattr $C$DW$T$212, DW_AT_language(DW_LANG_C)

$C$DW$T$63	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$63, DW_AT_byte_size(0x90)
$C$DW$324	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$324, DW_AT_name("ERCSR")
	.dwattr $C$DW$324, DW_AT_TI_symbol_name("_ERCSR")
	.dwattr $C$DW$324, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$324, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$325	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$325, DW_AT_name("AWCCR")
	.dwattr $C$DW$325, DW_AT_TI_symbol_name("_AWCCR")
	.dwattr $C$DW$325, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$325, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$326	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$326, DW_AT_name("SDBCR")
	.dwattr $C$DW$326, DW_AT_TI_symbol_name("_SDBCR")
	.dwattr $C$DW$326, DW_AT_data_member_location[DW_OP_plus_uconst 0x8]
	.dwattr $C$DW$326, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$327	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$327, DW_AT_name("SDRCR")
	.dwattr $C$DW$327, DW_AT_TI_symbol_name("_SDRCR")
	.dwattr $C$DW$327, DW_AT_data_member_location[DW_OP_plus_uconst 0xc]
	.dwattr $C$DW$327, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$328	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$328, DW_AT_name("AB1CR")
	.dwattr $C$DW$328, DW_AT_TI_symbol_name("_AB1CR")
	.dwattr $C$DW$328, DW_AT_data_member_location[DW_OP_plus_uconst 0x10]
	.dwattr $C$DW$328, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$329	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$329, DW_AT_name("AB2CR")
	.dwattr $C$DW$329, DW_AT_TI_symbol_name("_AB2CR")
	.dwattr $C$DW$329, DW_AT_data_member_location[DW_OP_plus_uconst 0x14]
	.dwattr $C$DW$329, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$330	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$330, DW_AT_name("AB3CR")
	.dwattr $C$DW$330, DW_AT_TI_symbol_name("_AB3CR")
	.dwattr $C$DW$330, DW_AT_data_member_location[DW_OP_plus_uconst 0x18]
	.dwattr $C$DW$330, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$331	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$331, DW_AT_name("AB4CR")
	.dwattr $C$DW$331, DW_AT_TI_symbol_name("_AB4CR")
	.dwattr $C$DW$331, DW_AT_data_member_location[DW_OP_plus_uconst 0x1c]
	.dwattr $C$DW$331, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$332	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$332, DW_AT_name("SDTIMR")
	.dwattr $C$DW$332, DW_AT_TI_symbol_name("_SDTIMR")
	.dwattr $C$DW$332, DW_AT_data_member_location[DW_OP_plus_uconst 0x20]
	.dwattr $C$DW$332, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$333	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$333, DW_AT_name("DDRSR")
	.dwattr $C$DW$333, DW_AT_TI_symbol_name("_DDRSR")
	.dwattr $C$DW$333, DW_AT_data_member_location[DW_OP_plus_uconst 0x24]
	.dwattr $C$DW$333, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$334	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$334, DW_AT_name("DDRPHYCR")
	.dwattr $C$DW$334, DW_AT_TI_symbol_name("_DDRPHYCR")
	.dwattr $C$DW$334, DW_AT_data_member_location[DW_OP_plus_uconst 0x28]
	.dwattr $C$DW$334, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$335	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$335, DW_AT_name("DDRPHYSR")
	.dwattr $C$DW$335, DW_AT_TI_symbol_name("_DDRPHYSR")
	.dwattr $C$DW$335, DW_AT_data_member_location[DW_OP_plus_uconst 0x2c]
	.dwattr $C$DW$335, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$336	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$336, DW_AT_name("TOTAR")
	.dwattr $C$DW$336, DW_AT_TI_symbol_name("_TOTAR")
	.dwattr $C$DW$336, DW_AT_data_member_location[DW_OP_plus_uconst 0x30]
	.dwattr $C$DW$336, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$337	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$337, DW_AT_name("TOTACTR")
	.dwattr $C$DW$337, DW_AT_TI_symbol_name("_TOTACTR")
	.dwattr $C$DW$337, DW_AT_data_member_location[DW_OP_plus_uconst 0x34]
	.dwattr $C$DW$337, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$338	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$338, DW_AT_name("DDRPHYID_REV")
	.dwattr $C$DW$338, DW_AT_TI_symbol_name("_DDRPHYID_REV")
	.dwattr $C$DW$338, DW_AT_data_member_location[DW_OP_plus_uconst 0x38]
	.dwattr $C$DW$338, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$339	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$339, DW_AT_name("SDSRETR")
	.dwattr $C$DW$339, DW_AT_TI_symbol_name("_SDSRETR")
	.dwattr $C$DW$339, DW_AT_data_member_location[DW_OP_plus_uconst 0x3c]
	.dwattr $C$DW$339, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$340	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$340, DW_AT_name("EIRR")
	.dwattr $C$DW$340, DW_AT_TI_symbol_name("_EIRR")
	.dwattr $C$DW$340, DW_AT_data_member_location[DW_OP_plus_uconst 0x40]
	.dwattr $C$DW$340, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$341	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$341, DW_AT_name("EIMR")
	.dwattr $C$DW$341, DW_AT_TI_symbol_name("_EIMR")
	.dwattr $C$DW$341, DW_AT_data_member_location[DW_OP_plus_uconst 0x44]
	.dwattr $C$DW$341, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$342	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$342, DW_AT_name("EIMSR")
	.dwattr $C$DW$342, DW_AT_TI_symbol_name("_EIMSR")
	.dwattr $C$DW$342, DW_AT_data_member_location[DW_OP_plus_uconst 0x48]
	.dwattr $C$DW$342, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$343	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$343, DW_AT_name("EIMCR")
	.dwattr $C$DW$343, DW_AT_TI_symbol_name("_EIMCR")
	.dwattr $C$DW$343, DW_AT_data_member_location[DW_OP_plus_uconst 0x4c]
	.dwattr $C$DW$343, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$344	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$344, DW_AT_name("IOCTRLR")
	.dwattr $C$DW$344, DW_AT_TI_symbol_name("_IOCTRLR")
	.dwattr $C$DW$344, DW_AT_data_member_location[DW_OP_plus_uconst 0x50]
	.dwattr $C$DW$344, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$345	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$345, DW_AT_name("IOSTATR")
	.dwattr $C$DW$345, DW_AT_TI_symbol_name("_IOSTATR")
	.dwattr $C$DW$345, DW_AT_data_member_location[DW_OP_plus_uconst 0x54]
	.dwattr $C$DW$345, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$346	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$346, DW_AT_name("NANDFCR")
	.dwattr $C$DW$346, DW_AT_TI_symbol_name("_NANDFCR")
	.dwattr $C$DW$346, DW_AT_data_member_location[DW_OP_plus_uconst 0x58]
	.dwattr $C$DW$346, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$347	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$347, DW_AT_name("NANDFSR")
	.dwattr $C$DW$347, DW_AT_TI_symbol_name("_NANDFSR")
	.dwattr $C$DW$347, DW_AT_data_member_location[DW_OP_plus_uconst 0x5c]
	.dwattr $C$DW$347, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$348	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$62)
	.dwattr $C$DW$348, DW_AT_name("RSVD1")
	.dwattr $C$DW$348, DW_AT_TI_symbol_name("_RSVD1")
	.dwattr $C$DW$348, DW_AT_data_member_location[DW_OP_plus_uconst 0x60]
	.dwattr $C$DW$348, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$349	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$349, DW_AT_name("NANDF1ECC")
	.dwattr $C$DW$349, DW_AT_TI_symbol_name("_NANDF1ECC")
	.dwattr $C$DW$349, DW_AT_data_member_location[DW_OP_plus_uconst 0x68]
	.dwattr $C$DW$349, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$350	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$350, DW_AT_name("NANDF2ECC")
	.dwattr $C$DW$350, DW_AT_TI_symbol_name("_NANDF2ECC")
	.dwattr $C$DW$350, DW_AT_data_member_location[DW_OP_plus_uconst 0x6c]
	.dwattr $C$DW$350, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$351	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$351, DW_AT_name("NANDF3ECC")
	.dwattr $C$DW$351, DW_AT_TI_symbol_name("_NANDF3ECC")
	.dwattr $C$DW$351, DW_AT_data_member_location[DW_OP_plus_uconst 0x70]
	.dwattr $C$DW$351, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$352	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$352, DW_AT_name("NANDF4ECC")
	.dwattr $C$DW$352, DW_AT_TI_symbol_name("_NANDF4ECC")
	.dwattr $C$DW$352, DW_AT_data_member_location[DW_OP_plus_uconst 0x74]
	.dwattr $C$DW$352, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$353	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$353, DW_AT_name("IODFTECR")
	.dwattr $C$DW$353, DW_AT_TI_symbol_name("_IODFTECR")
	.dwattr $C$DW$353, DW_AT_data_member_location[DW_OP_plus_uconst 0x78]
	.dwattr $C$DW$353, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$354	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$354, DW_AT_name("IODFTGCR")
	.dwattr $C$DW$354, DW_AT_TI_symbol_name("_IODFTGCR")
	.dwattr $C$DW$354, DW_AT_data_member_location[DW_OP_plus_uconst 0x7c]
	.dwattr $C$DW$354, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$355	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$355, DW_AT_name("IODFTMRLR")
	.dwattr $C$DW$355, DW_AT_TI_symbol_name("_IODFTMRLR")
	.dwattr $C$DW$355, DW_AT_data_member_location[DW_OP_plus_uconst 0x80]
	.dwattr $C$DW$355, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$356	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$356, DW_AT_name("IODFTMRMR")
	.dwattr $C$DW$356, DW_AT_TI_symbol_name("_IODFTMRMR")
	.dwattr $C$DW$356, DW_AT_data_member_location[DW_OP_plus_uconst 0x84]
	.dwattr $C$DW$356, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$357	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$357, DW_AT_name("IODFTMRMSBR")
	.dwattr $C$DW$357, DW_AT_TI_symbol_name("_IODFTMRMSBR")
	.dwattr $C$DW$357, DW_AT_data_member_location[DW_OP_plus_uconst 0x88]
	.dwattr $C$DW$357, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$358	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$358, DW_AT_name("MODRNR")
	.dwattr $C$DW$358, DW_AT_TI_symbol_name("_MODRNR")
	.dwattr $C$DW$358, DW_AT_data_member_location[DW_OP_plus_uconst 0x8c]
	.dwattr $C$DW$358, DW_AT_accessibility(DW_ACCESS_public)
	.dwendtag $C$DW$T$63

$C$DW$T$213	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_EmifRegs")
	.dwattr $C$DW$T$213, DW_AT_type(*$C$DW$T$63)
	.dwattr $C$DW$T$213, DW_AT_language(DW_LANG_C)
$C$DW$T$214	.dwtag  DW_TAG_volatile_type
	.dwattr $C$DW$T$214, DW_AT_type(*$C$DW$T$213)
$C$DW$T$215	.dwtag  DW_TAG_pointer_type, DW_AT_type(*$C$DW$T$214)
	.dwattr $C$DW$T$215, DW_AT_address_class(0x20)
$C$DW$T$216	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_EmifRegsOvly")
	.dwattr $C$DW$T$216, DW_AT_type(*$C$DW$T$215)
	.dwattr $C$DW$T$216, DW_AT_language(DW_LANG_C)

$C$DW$T$66	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$66, DW_AT_byte_size(0xe8)
$C$DW$359	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$359, DW_AT_name("REVID")
	.dwattr $C$DW$359, DW_AT_TI_symbol_name("_REVID")
	.dwattr $C$DW$359, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$359, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$360	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$360, DW_AT_name("CTRL")
	.dwattr $C$DW$360, DW_AT_TI_symbol_name("_CTRL")
	.dwattr $C$DW$360, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$360, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$361	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$361, DW_AT_name("STAT")
	.dwattr $C$DW$361, DW_AT_TI_symbol_name("_STAT")
	.dwattr $C$DW$361, DW_AT_data_member_location[DW_OP_plus_uconst 0x8]
	.dwattr $C$DW$361, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$362	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$362, DW_AT_name("INTPRI")
	.dwattr $C$DW$362, DW_AT_TI_symbol_name("_INTPRI")
	.dwattr $C$DW$362, DW_AT_data_member_location[DW_OP_plus_uconst 0xc]
	.dwattr $C$DW$362, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$363	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$363, DW_AT_name("INTSTATCLR")
	.dwattr $C$DW$363, DW_AT_TI_symbol_name("_INTSTATCLR")
	.dwattr $C$DW$363, DW_AT_data_member_location[DW_OP_plus_uconst 0x10]
	.dwattr $C$DW$363, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$364	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$364, DW_AT_name("INTPENDSET")
	.dwattr $C$DW$364, DW_AT_TI_symbol_name("_INTPENDSET")
	.dwattr $C$DW$364, DW_AT_data_member_location[DW_OP_plus_uconst 0x14]
	.dwattr $C$DW$364, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$365	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$365, DW_AT_name("INTPTR")
	.dwattr $C$DW$365, DW_AT_TI_symbol_name("_INTPTR")
	.dwattr $C$DW$365, DW_AT_data_member_location[DW_OP_plus_uconst 0x18]
	.dwattr $C$DW$365, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$366	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$366, DW_AT_name("XAM")
	.dwattr $C$DW$366, DW_AT_TI_symbol_name("_XAM")
	.dwattr $C$DW$366, DW_AT_data_member_location[DW_OP_plus_uconst 0x1c]
	.dwattr $C$DW$366, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$367	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$367, DW_AT_name("RAMS1")
	.dwattr $C$DW$367, DW_AT_TI_symbol_name("_RAMS1")
	.dwattr $C$DW$367, DW_AT_data_member_location[DW_OP_plus_uconst 0x20]
	.dwattr $C$DW$367, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$368	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$368, DW_AT_name("RAMO1")
	.dwattr $C$DW$368, DW_AT_TI_symbol_name("_RAMO1")
	.dwattr $C$DW$368, DW_AT_data_member_location[DW_OP_plus_uconst 0x24]
	.dwattr $C$DW$368, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$369	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$369, DW_AT_name("RAMS2")
	.dwattr $C$DW$369, DW_AT_TI_symbol_name("_RAMS2")
	.dwattr $C$DW$369, DW_AT_data_member_location[DW_OP_plus_uconst 0x28]
	.dwattr $C$DW$369, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$370	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$370, DW_AT_name("RAMO2")
	.dwattr $C$DW$370, DW_AT_TI_symbol_name("_RAMO2")
	.dwattr $C$DW$370, DW_AT_data_member_location[DW_OP_plus_uconst 0x2c]
	.dwattr $C$DW$370, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$371	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$371, DW_AT_name("RAMS3")
	.dwattr $C$DW$371, DW_AT_TI_symbol_name("_RAMS3")
	.dwattr $C$DW$371, DW_AT_data_member_location[DW_OP_plus_uconst 0x30]
	.dwattr $C$DW$371, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$372	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$372, DW_AT_name("RAMO3")
	.dwattr $C$DW$372, DW_AT_TI_symbol_name("_RAMO3")
	.dwattr $C$DW$372, DW_AT_data_member_location[DW_OP_plus_uconst 0x34]
	.dwattr $C$DW$372, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$373	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$373, DW_AT_name("RAMS4")
	.dwattr $C$DW$373, DW_AT_TI_symbol_name("_RAMS4")
	.dwattr $C$DW$373, DW_AT_data_member_location[DW_OP_plus_uconst 0x38]
	.dwattr $C$DW$373, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$374	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$374, DW_AT_name("RAMO4")
	.dwattr $C$DW$374, DW_AT_TI_symbol_name("_RAMO4")
	.dwattr $C$DW$374, DW_AT_data_member_location[DW_OP_plus_uconst 0x3c]
	.dwattr $C$DW$374, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$375	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$375, DW_AT_name("CHIPVER")
	.dwattr $C$DW$375, DW_AT_TI_symbol_name("_CHIPVER")
	.dwattr $C$DW$375, DW_AT_data_member_location[DW_OP_plus_uconst 0x40]
	.dwattr $C$DW$375, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$376	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$376, DW_AT_name("AUTNGO")
	.dwattr $C$DW$376, DW_AT_TI_symbol_name("_AUTNGO")
	.dwattr $C$DW$376, DW_AT_data_member_location[DW_OP_plus_uconst 0x44]
	.dwattr $C$DW$376, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$377	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$377, DW_AT_name("MANNGO")
	.dwattr $C$DW$377, DW_AT_TI_symbol_name("_MANNGO")
	.dwattr $C$DW$377, DW_AT_data_member_location[DW_OP_plus_uconst 0x48]
	.dwattr $C$DW$377, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$378	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$378, DW_AT_name("NGOSTAT")
	.dwattr $C$DW$378, DW_AT_TI_symbol_name("_NGOSTAT")
	.dwattr $C$DW$378, DW_AT_data_member_location[DW_OP_plus_uconst 0x4c]
	.dwattr $C$DW$378, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$379	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$64)
	.dwattr $C$DW$379, DW_AT_name("RSVD0")
	.dwattr $C$DW$379, DW_AT_TI_symbol_name("_RSVD0")
	.dwattr $C$DW$379, DW_AT_data_member_location[DW_OP_plus_uconst 0x50]
	.dwattr $C$DW$379, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$380	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$380, DW_AT_name("INTVEC0")
	.dwattr $C$DW$380, DW_AT_TI_symbol_name("_INTVEC0")
	.dwattr $C$DW$380, DW_AT_data_member_location[DW_OP_plus_uconst 0x60]
	.dwattr $C$DW$380, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$381	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$381, DW_AT_name("INTVEC1")
	.dwattr $C$DW$381, DW_AT_TI_symbol_name("_INTVEC1")
	.dwattr $C$DW$381, DW_AT_data_member_location[DW_OP_plus_uconst 0x64]
	.dwattr $C$DW$381, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$382	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$65)
	.dwattr $C$DW$382, DW_AT_name("RSVD1")
	.dwattr $C$DW$382, DW_AT_TI_symbol_name("_RSVD1")
	.dwattr $C$DW$382, DW_AT_data_member_location[DW_OP_plus_uconst 0x68]
	.dwattr $C$DW$382, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$383	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$383, DW_AT_name("RREVID")
	.dwattr $C$DW$383, DW_AT_TI_symbol_name("_RREVID")
	.dwattr $C$DW$383, DW_AT_data_member_location[DW_OP_plus_uconst 0x80]
	.dwattr $C$DW$383, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$384	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$384, DW_AT_name("RCTRL")
	.dwattr $C$DW$384, DW_AT_TI_symbol_name("_RCTRL")
	.dwattr $C$DW$384, DW_AT_data_member_location[DW_OP_plus_uconst 0x84]
	.dwattr $C$DW$384, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$385	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$385, DW_AT_name("RSTAT")
	.dwattr $C$DW$385, DW_AT_TI_symbol_name("_RSTAT")
	.dwattr $C$DW$385, DW_AT_data_member_location[DW_OP_plus_uconst 0x88]
	.dwattr $C$DW$385, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$386	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$386, DW_AT_name("RINTPRI")
	.dwattr $C$DW$386, DW_AT_TI_symbol_name("_RINTPRI")
	.dwattr $C$DW$386, DW_AT_data_member_location[DW_OP_plus_uconst 0x8c]
	.dwattr $C$DW$386, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$387	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$387, DW_AT_name("RINTSTATCLR")
	.dwattr $C$DW$387, DW_AT_TI_symbol_name("_RINTSTATCLR")
	.dwattr $C$DW$387, DW_AT_data_member_location[DW_OP_plus_uconst 0x90]
	.dwattr $C$DW$387, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$388	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$388, DW_AT_name("RINTPENDSET")
	.dwattr $C$DW$388, DW_AT_TI_symbol_name("_RINTPENDSET")
	.dwattr $C$DW$388, DW_AT_data_member_location[DW_OP_plus_uconst 0x94]
	.dwattr $C$DW$388, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$389	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$389, DW_AT_name("RINTPTR")
	.dwattr $C$DW$389, DW_AT_TI_symbol_name("_RINTPTR")
	.dwattr $C$DW$389, DW_AT_data_member_location[DW_OP_plus_uconst 0x98]
	.dwattr $C$DW$389, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$390	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$390, DW_AT_name("RXAM")
	.dwattr $C$DW$390, DW_AT_TI_symbol_name("_RXAM")
	.dwattr $C$DW$390, DW_AT_data_member_location[DW_OP_plus_uconst 0x9c]
	.dwattr $C$DW$390, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$391	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$391, DW_AT_name("RRAMS1")
	.dwattr $C$DW$391, DW_AT_TI_symbol_name("_RRAMS1")
	.dwattr $C$DW$391, DW_AT_data_member_location[DW_OP_plus_uconst 0xa0]
	.dwattr $C$DW$391, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$392	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$392, DW_AT_name("RRAMO1")
	.dwattr $C$DW$392, DW_AT_TI_symbol_name("_RRAMO1")
	.dwattr $C$DW$392, DW_AT_data_member_location[DW_OP_plus_uconst 0xa4]
	.dwattr $C$DW$392, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$393	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$393, DW_AT_name("RRAMS2")
	.dwattr $C$DW$393, DW_AT_TI_symbol_name("_RRAMS2")
	.dwattr $C$DW$393, DW_AT_data_member_location[DW_OP_plus_uconst 0xa8]
	.dwattr $C$DW$393, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$394	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$394, DW_AT_name("RRAMO2")
	.dwattr $C$DW$394, DW_AT_TI_symbol_name("_RRAMO2")
	.dwattr $C$DW$394, DW_AT_data_member_location[DW_OP_plus_uconst 0xac]
	.dwattr $C$DW$394, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$395	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$395, DW_AT_name("RRAMS3")
	.dwattr $C$DW$395, DW_AT_TI_symbol_name("_RRAMS3")
	.dwattr $C$DW$395, DW_AT_data_member_location[DW_OP_plus_uconst 0xb0]
	.dwattr $C$DW$395, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$396	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$396, DW_AT_name("RRAMO3")
	.dwattr $C$DW$396, DW_AT_TI_symbol_name("_RRAMO3")
	.dwattr $C$DW$396, DW_AT_data_member_location[DW_OP_plus_uconst 0xb4]
	.dwattr $C$DW$396, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$397	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$397, DW_AT_name("RRAMS4")
	.dwattr $C$DW$397, DW_AT_TI_symbol_name("_RRAMS4")
	.dwattr $C$DW$397, DW_AT_data_member_location[DW_OP_plus_uconst 0xb8]
	.dwattr $C$DW$397, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$398	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$398, DW_AT_name("RRAMO4")
	.dwattr $C$DW$398, DW_AT_TI_symbol_name("_RRAMO4")
	.dwattr $C$DW$398, DW_AT_data_member_location[DW_OP_plus_uconst 0xbc]
	.dwattr $C$DW$398, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$399	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$399, DW_AT_name("RCHIPVER")
	.dwattr $C$DW$399, DW_AT_TI_symbol_name("_RCHIPVER")
	.dwattr $C$DW$399, DW_AT_data_member_location[DW_OP_plus_uconst 0xc0]
	.dwattr $C$DW$399, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$400	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$400, DW_AT_name("RAUTNGO")
	.dwattr $C$DW$400, DW_AT_TI_symbol_name("_RAUTNGO")
	.dwattr $C$DW$400, DW_AT_data_member_location[DW_OP_plus_uconst 0xc4]
	.dwattr $C$DW$400, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$401	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$401, DW_AT_name("RMANNGO")
	.dwattr $C$DW$401, DW_AT_TI_symbol_name("_RMANNGO")
	.dwattr $C$DW$401, DW_AT_data_member_location[DW_OP_plus_uconst 0xc8]
	.dwattr $C$DW$401, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$402	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$402, DW_AT_name("RNGOSTAT")
	.dwattr $C$DW$402, DW_AT_TI_symbol_name("_RNGOSTAT")
	.dwattr $C$DW$402, DW_AT_data_member_location[DW_OP_plus_uconst 0xcc]
	.dwattr $C$DW$402, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$403	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$64)
	.dwattr $C$DW$403, DW_AT_name("RSVD2")
	.dwattr $C$DW$403, DW_AT_TI_symbol_name("_RSVD2")
	.dwattr $C$DW$403, DW_AT_data_member_location[DW_OP_plus_uconst 0xd0]
	.dwattr $C$DW$403, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$404	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$404, DW_AT_name("RINTVEC0")
	.dwattr $C$DW$404, DW_AT_TI_symbol_name("_RINTVEC0")
	.dwattr $C$DW$404, DW_AT_data_member_location[DW_OP_plus_uconst 0xe0]
	.dwattr $C$DW$404, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$405	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$405, DW_AT_name("RINTVEC1")
	.dwattr $C$DW$405, DW_AT_TI_symbol_name("_RINTVEC1")
	.dwattr $C$DW$405, DW_AT_data_member_location[DW_OP_plus_uconst 0xe4]
	.dwattr $C$DW$405, DW_AT_accessibility(DW_ACCESS_public)
	.dwendtag $C$DW$T$66

$C$DW$T$217	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_VlynqRegs")
	.dwattr $C$DW$T$217, DW_AT_type(*$C$DW$T$66)
	.dwattr $C$DW$T$217, DW_AT_language(DW_LANG_C)
$C$DW$T$218	.dwtag  DW_TAG_volatile_type
	.dwattr $C$DW$T$218, DW_AT_type(*$C$DW$T$217)
$C$DW$T$219	.dwtag  DW_TAG_pointer_type, DW_AT_type(*$C$DW$T$218)
	.dwattr $C$DW$T$219, DW_AT_address_class(0x20)
$C$DW$T$220	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_VlynqRegsOvly")
	.dwattr $C$DW$T$220, DW_AT_type(*$C$DW$T$219)
	.dwattr $C$DW$T$220, DW_AT_language(DW_LANG_C)

$C$DW$T$67	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$67, DW_AT_byte_size(0x5c)
$C$DW$406	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$406, DW_AT_name("DRR")
	.dwattr $C$DW$406, DW_AT_TI_symbol_name("_DRR")
	.dwattr $C$DW$406, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$406, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$407	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$407, DW_AT_name("DXR")
	.dwattr $C$DW$407, DW_AT_TI_symbol_name("_DXR")
	.dwattr $C$DW$407, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$407, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$408	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$408, DW_AT_name("SPCR")
	.dwattr $C$DW$408, DW_AT_TI_symbol_name("_SPCR")
	.dwattr $C$DW$408, DW_AT_data_member_location[DW_OP_plus_uconst 0x8]
	.dwattr $C$DW$408, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$409	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$409, DW_AT_name("RCR")
	.dwattr $C$DW$409, DW_AT_TI_symbol_name("_RCR")
	.dwattr $C$DW$409, DW_AT_data_member_location[DW_OP_plus_uconst 0xc]
	.dwattr $C$DW$409, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$410	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$410, DW_AT_name("XCR")
	.dwattr $C$DW$410, DW_AT_TI_symbol_name("_XCR")
	.dwattr $C$DW$410, DW_AT_data_member_location[DW_OP_plus_uconst 0x10]
	.dwattr $C$DW$410, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$411	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$411, DW_AT_name("SRGR")
	.dwattr $C$DW$411, DW_AT_TI_symbol_name("_SRGR")
	.dwattr $C$DW$411, DW_AT_data_member_location[DW_OP_plus_uconst 0x14]
	.dwattr $C$DW$411, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$412	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$412, DW_AT_name("MCR")
	.dwattr $C$DW$412, DW_AT_TI_symbol_name("_MCR")
	.dwattr $C$DW$412, DW_AT_data_member_location[DW_OP_plus_uconst 0x18]
	.dwattr $C$DW$412, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$413	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$413, DW_AT_name("RCERAB")
	.dwattr $C$DW$413, DW_AT_TI_symbol_name("_RCERAB")
	.dwattr $C$DW$413, DW_AT_data_member_location[DW_OP_plus_uconst 0x1c]
	.dwattr $C$DW$413, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$414	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$414, DW_AT_name("XCERAB")
	.dwattr $C$DW$414, DW_AT_TI_symbol_name("_XCERAB")
	.dwattr $C$DW$414, DW_AT_data_member_location[DW_OP_plus_uconst 0x20]
	.dwattr $C$DW$414, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$415	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$415, DW_AT_name("PCR")
	.dwattr $C$DW$415, DW_AT_TI_symbol_name("_PCR")
	.dwattr $C$DW$415, DW_AT_data_member_location[DW_OP_plus_uconst 0x24]
	.dwattr $C$DW$415, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$416	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$416, DW_AT_name("RCERCD")
	.dwattr $C$DW$416, DW_AT_TI_symbol_name("_RCERCD")
	.dwattr $C$DW$416, DW_AT_data_member_location[DW_OP_plus_uconst 0x28]
	.dwattr $C$DW$416, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$417	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$417, DW_AT_name("XCERCD")
	.dwattr $C$DW$417, DW_AT_TI_symbol_name("_XCERCD")
	.dwattr $C$DW$417, DW_AT_data_member_location[DW_OP_plus_uconst 0x2c]
	.dwattr $C$DW$417, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$418	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$418, DW_AT_name("RCEREF")
	.dwattr $C$DW$418, DW_AT_TI_symbol_name("_RCEREF")
	.dwattr $C$DW$418, DW_AT_data_member_location[DW_OP_plus_uconst 0x30]
	.dwattr $C$DW$418, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$419	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$419, DW_AT_name("XCEREF")
	.dwattr $C$DW$419, DW_AT_TI_symbol_name("_XCEREF")
	.dwattr $C$DW$419, DW_AT_data_member_location[DW_OP_plus_uconst 0x34]
	.dwattr $C$DW$419, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$420	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$420, DW_AT_name("RCERGH")
	.dwattr $C$DW$420, DW_AT_TI_symbol_name("_RCERGH")
	.dwattr $C$DW$420, DW_AT_data_member_location[DW_OP_plus_uconst 0x38]
	.dwattr $C$DW$420, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$421	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$421, DW_AT_name("XCERGH")
	.dwattr $C$DW$421, DW_AT_TI_symbol_name("_XCERGH")
	.dwattr $C$DW$421, DW_AT_data_member_location[DW_OP_plus_uconst 0x3c]
	.dwattr $C$DW$421, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$422	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$422, DW_AT_name("REVTCR")
	.dwattr $C$DW$422, DW_AT_TI_symbol_name("_REVTCR")
	.dwattr $C$DW$422, DW_AT_data_member_location[DW_OP_plus_uconst 0x40]
	.dwattr $C$DW$422, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$423	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$423, DW_AT_name("XEVTCR")
	.dwattr $C$DW$423, DW_AT_TI_symbol_name("_XEVTCR")
	.dwattr $C$DW$423, DW_AT_data_member_location[DW_OP_plus_uconst 0x44]
	.dwattr $C$DW$423, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$424	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$424, DW_AT_name("RFLR")
	.dwattr $C$DW$424, DW_AT_TI_symbol_name("_RFLR")
	.dwattr $C$DW$424, DW_AT_data_member_location[DW_OP_plus_uconst 0x48]
	.dwattr $C$DW$424, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$425	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$425, DW_AT_name("XFLR")
	.dwattr $C$DW$425, DW_AT_TI_symbol_name("_XFLR")
	.dwattr $C$DW$425, DW_AT_data_member_location[DW_OP_plus_uconst 0x4c]
	.dwattr $C$DW$425, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$426	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$426, DW_AT_name("RSYNCCNT")
	.dwattr $C$DW$426, DW_AT_TI_symbol_name("_RSYNCCNT")
	.dwattr $C$DW$426, DW_AT_data_member_location[DW_OP_plus_uconst 0x50]
	.dwattr $C$DW$426, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$427	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$427, DW_AT_name("XSYNCCNT")
	.dwattr $C$DW$427, DW_AT_TI_symbol_name("_XSYNCCNT")
	.dwattr $C$DW$427, DW_AT_data_member_location[DW_OP_plus_uconst 0x54]
	.dwattr $C$DW$427, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$428	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$428, DW_AT_name("PID")
	.dwattr $C$DW$428, DW_AT_TI_symbol_name("_PID")
	.dwattr $C$DW$428, DW_AT_data_member_location[DW_OP_plus_uconst 0x58]
	.dwattr $C$DW$428, DW_AT_accessibility(DW_ACCESS_public)
	.dwendtag $C$DW$T$67

$C$DW$T$221	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_McbspRegs")
	.dwattr $C$DW$T$221, DW_AT_type(*$C$DW$T$67)
	.dwattr $C$DW$T$221, DW_AT_language(DW_LANG_C)
$C$DW$T$222	.dwtag  DW_TAG_volatile_type
	.dwattr $C$DW$T$222, DW_AT_type(*$C$DW$T$221)
$C$DW$T$223	.dwtag  DW_TAG_pointer_type, DW_AT_type(*$C$DW$T$222)
	.dwattr $C$DW$T$223, DW_AT_address_class(0x20)
$C$DW$T$224	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_McbspRegsOvly")
	.dwattr $C$DW$T$224, DW_AT_type(*$C$DW$T$223)
	.dwattr $C$DW$T$224, DW_AT_language(DW_LANG_C)

$C$DW$T$70	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$70, DW_AT_byte_size(0xf8)
$C$DW$429	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$429, DW_AT_name("ERCSR")
	.dwattr $C$DW$429, DW_AT_TI_symbol_name("_ERCSR")
	.dwattr $C$DW$429, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$429, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$430	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$430, DW_AT_name("SDRSTAT")
	.dwattr $C$DW$430, DW_AT_TI_symbol_name("_SDRSTAT")
	.dwattr $C$DW$430, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$430, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$431	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$431, DW_AT_name("SDBCR")
	.dwattr $C$DW$431, DW_AT_TI_symbol_name("_SDBCR")
	.dwattr $C$DW$431, DW_AT_data_member_location[DW_OP_plus_uconst 0x8]
	.dwattr $C$DW$431, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$432	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$432, DW_AT_name("SDRCR")
	.dwattr $C$DW$432, DW_AT_TI_symbol_name("_SDRCR")
	.dwattr $C$DW$432, DW_AT_data_member_location[DW_OP_plus_uconst 0xc]
	.dwattr $C$DW$432, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$433	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$433, DW_AT_name("SDTIMR")
	.dwattr $C$DW$433, DW_AT_TI_symbol_name("_SDTIMR")
	.dwattr $C$DW$433, DW_AT_data_member_location[DW_OP_plus_uconst 0x10]
	.dwattr $C$DW$433, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$434	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$434, DW_AT_name("SDTIMR2")
	.dwattr $C$DW$434, DW_AT_TI_symbol_name("_SDTIMR2")
	.dwattr $C$DW$434, DW_AT_data_member_location[DW_OP_plus_uconst 0x14]
	.dwattr $C$DW$434, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$435	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$62)
	.dwattr $C$DW$435, DW_AT_name("RSVD0")
	.dwattr $C$DW$435, DW_AT_TI_symbol_name("_RSVD0")
	.dwattr $C$DW$435, DW_AT_data_member_location[DW_OP_plus_uconst 0x18]
	.dwattr $C$DW$435, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$436	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$436, DW_AT_name("VBPR")
	.dwattr $C$DW$436, DW_AT_TI_symbol_name("_VBPR")
	.dwattr $C$DW$436, DW_AT_data_member_location[DW_OP_plus_uconst 0x20]
	.dwattr $C$DW$436, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$437	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$55)
	.dwattr $C$DW$437, DW_AT_name("RSVD1")
	.dwattr $C$DW$437, DW_AT_TI_symbol_name("_RSVD1")
	.dwattr $C$DW$437, DW_AT_data_member_location[DW_OP_plus_uconst 0x24]
	.dwattr $C$DW$437, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$438	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$438, DW_AT_name("VBCFG1")
	.dwattr $C$DW$438, DW_AT_TI_symbol_name("_VBCFG1")
	.dwattr $C$DW$438, DW_AT_data_member_location[DW_OP_plus_uconst 0x28]
	.dwattr $C$DW$438, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$439	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$439, DW_AT_name("VBCFG2")
	.dwattr $C$DW$439, DW_AT_TI_symbol_name("_VBCFG2")
	.dwattr $C$DW$439, DW_AT_data_member_location[DW_OP_plus_uconst 0x2c]
	.dwattr $C$DW$439, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$440	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$64)
	.dwattr $C$DW$440, DW_AT_name("RSVD2")
	.dwattr $C$DW$440, DW_AT_TI_symbol_name("_RSVD2")
	.dwattr $C$DW$440, DW_AT_data_member_location[DW_OP_plus_uconst 0x30]
	.dwattr $C$DW$440, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$441	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$441, DW_AT_name("PERFC1R")
	.dwattr $C$DW$441, DW_AT_TI_symbol_name("_PERFC1R")
	.dwattr $C$DW$441, DW_AT_data_member_location[DW_OP_plus_uconst 0x40]
	.dwattr $C$DW$441, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$442	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$442, DW_AT_name("PERFC2R")
	.dwattr $C$DW$442, DW_AT_TI_symbol_name("_PERFC2R")
	.dwattr $C$DW$442, DW_AT_data_member_location[DW_OP_plus_uconst 0x44]
	.dwattr $C$DW$442, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$443	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$443, DW_AT_name("PCCR")
	.dwattr $C$DW$443, DW_AT_TI_symbol_name("_PCCR")
	.dwattr $C$DW$443, DW_AT_data_member_location[DW_OP_plus_uconst 0x48]
	.dwattr $C$DW$443, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$444	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$444, DW_AT_name("PCMRSR")
	.dwattr $C$DW$444, DW_AT_TI_symbol_name("_PCMRSR")
	.dwattr $C$DW$444, DW_AT_data_member_location[DW_OP_plus_uconst 0x4c]
	.dwattr $C$DW$444, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$445	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$68)
	.dwattr $C$DW$445, DW_AT_name("RSVD3")
	.dwattr $C$DW$445, DW_AT_TI_symbol_name("_RSVD3")
	.dwattr $C$DW$445, DW_AT_data_member_location[DW_OP_plus_uconst 0x50]
	.dwattr $C$DW$445, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$446	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$446, DW_AT_name("ASYNCCS2CR")
	.dwattr $C$DW$446, DW_AT_TI_symbol_name("_ASYNCCS2CR")
	.dwattr $C$DW$446, DW_AT_data_member_location[DW_OP_plus_uconst 0x80]
	.dwattr $C$DW$446, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$447	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$447, DW_AT_name("ASYNCCS3CR")
	.dwattr $C$DW$447, DW_AT_TI_symbol_name("_ASYNCCS3CR")
	.dwattr $C$DW$447, DW_AT_data_member_location[DW_OP_plus_uconst 0x84]
	.dwattr $C$DW$447, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$448	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$448, DW_AT_name("ASYNCCS4CR")
	.dwattr $C$DW$448, DW_AT_TI_symbol_name("_ASYNCCS4CR")
	.dwattr $C$DW$448, DW_AT_data_member_location[DW_OP_plus_uconst 0x88]
	.dwattr $C$DW$448, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$449	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$449, DW_AT_name("ASYNCCS5CR")
	.dwattr $C$DW$449, DW_AT_TI_symbol_name("_ASYNCCS5CR")
	.dwattr $C$DW$449, DW_AT_data_member_location[DW_OP_plus_uconst 0x8c]
	.dwattr $C$DW$449, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$450	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$64)
	.dwattr $C$DW$450, DW_AT_name("RSVD4")
	.dwattr $C$DW$450, DW_AT_TI_symbol_name("_RSVD4")
	.dwattr $C$DW$450, DW_AT_data_member_location[DW_OP_plus_uconst 0x90]
	.dwattr $C$DW$450, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$451	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$451, DW_AT_name("AWCCR")
	.dwattr $C$DW$451, DW_AT_TI_symbol_name("_AWCCR")
	.dwattr $C$DW$451, DW_AT_data_member_location[DW_OP_plus_uconst 0xa0]
	.dwattr $C$DW$451, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$452	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$69)
	.dwattr $C$DW$452, DW_AT_name("RSVD5")
	.dwattr $C$DW$452, DW_AT_TI_symbol_name("_RSVD5")
	.dwattr $C$DW$452, DW_AT_data_member_location[DW_OP_plus_uconst 0xa4]
	.dwattr $C$DW$452, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$453	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$453, DW_AT_name("IRR")
	.dwattr $C$DW$453, DW_AT_TI_symbol_name("_IRR")
	.dwattr $C$DW$453, DW_AT_data_member_location[DW_OP_plus_uconst 0xc0]
	.dwattr $C$DW$453, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$454	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$454, DW_AT_name("IMR")
	.dwattr $C$DW$454, DW_AT_TI_symbol_name("_IMR")
	.dwattr $C$DW$454, DW_AT_data_member_location[DW_OP_plus_uconst 0xc4]
	.dwattr $C$DW$454, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$455	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$455, DW_AT_name("IMSR")
	.dwattr $C$DW$455, DW_AT_TI_symbol_name("_IMSR")
	.dwattr $C$DW$455, DW_AT_data_member_location[DW_OP_plus_uconst 0xc8]
	.dwattr $C$DW$455, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$456	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$456, DW_AT_name("IMCR")
	.dwattr $C$DW$456, DW_AT_TI_symbol_name("_IMCR")
	.dwattr $C$DW$456, DW_AT_data_member_location[DW_OP_plus_uconst 0xcc]
	.dwattr $C$DW$456, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$457	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$64)
	.dwattr $C$DW$457, DW_AT_name("RSVD6")
	.dwattr $C$DW$457, DW_AT_TI_symbol_name("_RSVD6")
	.dwattr $C$DW$457, DW_AT_data_member_location[DW_OP_plus_uconst 0xd0]
	.dwattr $C$DW$457, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$458	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$458, DW_AT_name("DDRPHYREV")
	.dwattr $C$DW$458, DW_AT_TI_symbol_name("_DDRPHYREV")
	.dwattr $C$DW$458, DW_AT_data_member_location[DW_OP_plus_uconst 0xe0]
	.dwattr $C$DW$458, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$459	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$459, DW_AT_name("DDRPHYCR")
	.dwattr $C$DW$459, DW_AT_TI_symbol_name("_DDRPHYCR")
	.dwattr $C$DW$459, DW_AT_data_member_location[DW_OP_plus_uconst 0xe4]
	.dwattr $C$DW$459, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$460	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$460, DW_AT_name("DDRPHYSR")
	.dwattr $C$DW$460, DW_AT_TI_symbol_name("_DDRPHYSR")
	.dwattr $C$DW$460, DW_AT_data_member_location[DW_OP_plus_uconst 0xe8]
	.dwattr $C$DW$460, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$461	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$55)
	.dwattr $C$DW$461, DW_AT_name("RSVD7")
	.dwattr $C$DW$461, DW_AT_TI_symbol_name("_RSVD7")
	.dwattr $C$DW$461, DW_AT_data_member_location[DW_OP_plus_uconst 0xec]
	.dwattr $C$DW$461, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$462	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$462, DW_AT_name("VTPCTRL")
	.dwattr $C$DW$462, DW_AT_TI_symbol_name("_VTPCTRL")
	.dwattr $C$DW$462, DW_AT_data_member_location[DW_OP_plus_uconst 0xf0]
	.dwattr $C$DW$462, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$463	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$463, DW_AT_name("VTPSTAT")
	.dwattr $C$DW$463, DW_AT_TI_symbol_name("_VTPSTAT")
	.dwattr $C$DW$463, DW_AT_data_member_location[DW_OP_plus_uconst 0xf4]
	.dwattr $C$DW$463, DW_AT_accessibility(DW_ACCESS_public)
	.dwendtag $C$DW$T$70

$C$DW$T$225	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_DdrRegs")
	.dwattr $C$DW$T$225, DW_AT_type(*$C$DW$T$70)
	.dwattr $C$DW$T$225, DW_AT_language(DW_LANG_C)
$C$DW$T$226	.dwtag  DW_TAG_volatile_type
	.dwattr $C$DW$T$226, DW_AT_type(*$C$DW$T$225)
$C$DW$T$227	.dwtag  DW_TAG_pointer_type, DW_AT_type(*$C$DW$T$226)
	.dwattr $C$DW$T$227, DW_AT_address_class(0x20)
$C$DW$T$228	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_DdrRegsOvly")
	.dwattr $C$DW$T$228, DW_AT_type(*$C$DW$T$227)
	.dwattr $C$DW$T$228, DW_AT_language(DW_LANG_C)

$C$DW$T$71	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$71, DW_AT_byte_size(0x08)
$C$DW$464	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$464, DW_AT_name("DRAE")
	.dwattr $C$DW$464, DW_AT_TI_symbol_name("_DRAE")
	.dwattr $C$DW$464, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$464, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$465	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$465, DW_AT_name("DRAEH")
	.dwattr $C$DW$465, DW_AT_TI_symbol_name("_DRAEH")
	.dwattr $C$DW$465, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$465, DW_AT_accessibility(DW_ACCESS_public)
	.dwendtag $C$DW$T$71

$C$DW$T$80	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_EdmaccDraRegs")
	.dwattr $C$DW$T$80, DW_AT_type(*$C$DW$T$71)
	.dwattr $C$DW$T$80, DW_AT_language(DW_LANG_C)

$C$DW$T$81	.dwtag  DW_TAG_array_type, DW_AT_type(*$C$DW$T$80)
	.dwattr $C$DW$T$81, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$81, DW_AT_byte_size(0x40)
$C$DW$466	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$466, DW_AT_upper_bound(0x07)
	.dwendtag $C$DW$T$81


$C$DW$T$72	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$72, DW_AT_byte_size(0x04)
$C$DW$467	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$467, DW_AT_name("QUEEVT_ENTRY")
	.dwattr $C$DW$467, DW_AT_TI_symbol_name("_QUEEVT_ENTRY")
	.dwattr $C$DW$467, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$467, DW_AT_accessibility(DW_ACCESS_public)
	.dwendtag $C$DW$T$72

$C$DW$T$83	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_EdmaccQueevtentryRegs")
	.dwattr $C$DW$T$83, DW_AT_type(*$C$DW$T$72)
	.dwattr $C$DW$T$83, DW_AT_language(DW_LANG_C)

$C$DW$T$84	.dwtag  DW_TAG_array_type, DW_AT_type(*$C$DW$T$83)
	.dwattr $C$DW$T$84, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$84, DW_AT_byte_size(0x40)
$C$DW$468	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$468, DW_AT_upper_bound(0x0f)
	.dwendtag $C$DW$T$84


$C$DW$T$85	.dwtag  DW_TAG_array_type, DW_AT_type(*$C$DW$T$83)
	.dwattr $C$DW$T$85, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$85, DW_AT_byte_size(0x200)
$C$DW$469	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$469, DW_AT_upper_bound(0x07)
$C$DW$470	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$470, DW_AT_upper_bound(0x0f)
	.dwendtag $C$DW$T$85


$C$DW$T$74	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$74, DW_AT_byte_size(0x200)
$C$DW$471	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$471, DW_AT_name("ER")
	.dwattr $C$DW$471, DW_AT_TI_symbol_name("_ER")
	.dwattr $C$DW$471, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$471, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$472	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$472, DW_AT_name("ERH")
	.dwattr $C$DW$472, DW_AT_TI_symbol_name("_ERH")
	.dwattr $C$DW$472, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$472, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$473	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$473, DW_AT_name("ECR")
	.dwattr $C$DW$473, DW_AT_TI_symbol_name("_ECR")
	.dwattr $C$DW$473, DW_AT_data_member_location[DW_OP_plus_uconst 0x8]
	.dwattr $C$DW$473, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$474	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$474, DW_AT_name("ECRH")
	.dwattr $C$DW$474, DW_AT_TI_symbol_name("_ECRH")
	.dwattr $C$DW$474, DW_AT_data_member_location[DW_OP_plus_uconst 0xc]
	.dwattr $C$DW$474, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$475	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$475, DW_AT_name("ESR")
	.dwattr $C$DW$475, DW_AT_TI_symbol_name("_ESR")
	.dwattr $C$DW$475, DW_AT_data_member_location[DW_OP_plus_uconst 0x10]
	.dwattr $C$DW$475, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$476	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$476, DW_AT_name("ESRH")
	.dwattr $C$DW$476, DW_AT_TI_symbol_name("_ESRH")
	.dwattr $C$DW$476, DW_AT_data_member_location[DW_OP_plus_uconst 0x14]
	.dwattr $C$DW$476, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$477	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$477, DW_AT_name("CER")
	.dwattr $C$DW$477, DW_AT_TI_symbol_name("_CER")
	.dwattr $C$DW$477, DW_AT_data_member_location[DW_OP_plus_uconst 0x18]
	.dwattr $C$DW$477, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$478	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$478, DW_AT_name("CERH")
	.dwattr $C$DW$478, DW_AT_TI_symbol_name("_CERH")
	.dwattr $C$DW$478, DW_AT_data_member_location[DW_OP_plus_uconst 0x1c]
	.dwattr $C$DW$478, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$479	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$479, DW_AT_name("EER")
	.dwattr $C$DW$479, DW_AT_TI_symbol_name("_EER")
	.dwattr $C$DW$479, DW_AT_data_member_location[DW_OP_plus_uconst 0x20]
	.dwattr $C$DW$479, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$480	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$480, DW_AT_name("EERH")
	.dwattr $C$DW$480, DW_AT_TI_symbol_name("_EERH")
	.dwattr $C$DW$480, DW_AT_data_member_location[DW_OP_plus_uconst 0x24]
	.dwattr $C$DW$480, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$481	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$481, DW_AT_name("EECR")
	.dwattr $C$DW$481, DW_AT_TI_symbol_name("_EECR")
	.dwattr $C$DW$481, DW_AT_data_member_location[DW_OP_plus_uconst 0x28]
	.dwattr $C$DW$481, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$482	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$482, DW_AT_name("EECRH")
	.dwattr $C$DW$482, DW_AT_TI_symbol_name("_EECRH")
	.dwattr $C$DW$482, DW_AT_data_member_location[DW_OP_plus_uconst 0x2c]
	.dwattr $C$DW$482, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$483	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$483, DW_AT_name("EESR")
	.dwattr $C$DW$483, DW_AT_TI_symbol_name("_EESR")
	.dwattr $C$DW$483, DW_AT_data_member_location[DW_OP_plus_uconst 0x30]
	.dwattr $C$DW$483, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$484	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$484, DW_AT_name("EESRH")
	.dwattr $C$DW$484, DW_AT_TI_symbol_name("_EESRH")
	.dwattr $C$DW$484, DW_AT_data_member_location[DW_OP_plus_uconst 0x34]
	.dwattr $C$DW$484, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$485	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$485, DW_AT_name("SER")
	.dwattr $C$DW$485, DW_AT_TI_symbol_name("_SER")
	.dwattr $C$DW$485, DW_AT_data_member_location[DW_OP_plus_uconst 0x38]
	.dwattr $C$DW$485, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$486	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$486, DW_AT_name("SERH")
	.dwattr $C$DW$486, DW_AT_TI_symbol_name("_SERH")
	.dwattr $C$DW$486, DW_AT_data_member_location[DW_OP_plus_uconst 0x3c]
	.dwattr $C$DW$486, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$487	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$487, DW_AT_name("SECR")
	.dwattr $C$DW$487, DW_AT_TI_symbol_name("_SECR")
	.dwattr $C$DW$487, DW_AT_data_member_location[DW_OP_plus_uconst 0x40]
	.dwattr $C$DW$487, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$488	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$488, DW_AT_name("SECRH")
	.dwattr $C$DW$488, DW_AT_TI_symbol_name("_SECRH")
	.dwattr $C$DW$488, DW_AT_data_member_location[DW_OP_plus_uconst 0x44]
	.dwattr $C$DW$488, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$489	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$62)
	.dwattr $C$DW$489, DW_AT_name("RSVD0")
	.dwattr $C$DW$489, DW_AT_TI_symbol_name("_RSVD0")
	.dwattr $C$DW$489, DW_AT_data_member_location[DW_OP_plus_uconst 0x48]
	.dwattr $C$DW$489, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$490	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$490, DW_AT_name("IER")
	.dwattr $C$DW$490, DW_AT_TI_symbol_name("_IER")
	.dwattr $C$DW$490, DW_AT_data_member_location[DW_OP_plus_uconst 0x50]
	.dwattr $C$DW$490, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$491	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$491, DW_AT_name("IERH")
	.dwattr $C$DW$491, DW_AT_TI_symbol_name("_IERH")
	.dwattr $C$DW$491, DW_AT_data_member_location[DW_OP_plus_uconst 0x54]
	.dwattr $C$DW$491, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$492	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$492, DW_AT_name("IECR")
	.dwattr $C$DW$492, DW_AT_TI_symbol_name("_IECR")
	.dwattr $C$DW$492, DW_AT_data_member_location[DW_OP_plus_uconst 0x58]
	.dwattr $C$DW$492, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$493	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$493, DW_AT_name("IECRH")
	.dwattr $C$DW$493, DW_AT_TI_symbol_name("_IECRH")
	.dwattr $C$DW$493, DW_AT_data_member_location[DW_OP_plus_uconst 0x5c]
	.dwattr $C$DW$493, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$494	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$494, DW_AT_name("IESR")
	.dwattr $C$DW$494, DW_AT_TI_symbol_name("_IESR")
	.dwattr $C$DW$494, DW_AT_data_member_location[DW_OP_plus_uconst 0x60]
	.dwattr $C$DW$494, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$495	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$495, DW_AT_name("IESRH")
	.dwattr $C$DW$495, DW_AT_TI_symbol_name("_IESRH")
	.dwattr $C$DW$495, DW_AT_data_member_location[DW_OP_plus_uconst 0x64]
	.dwattr $C$DW$495, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$496	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$496, DW_AT_name("IPR")
	.dwattr $C$DW$496, DW_AT_TI_symbol_name("_IPR")
	.dwattr $C$DW$496, DW_AT_data_member_location[DW_OP_plus_uconst 0x68]
	.dwattr $C$DW$496, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$497	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$497, DW_AT_name("IPRH")
	.dwattr $C$DW$497, DW_AT_TI_symbol_name("_IPRH")
	.dwattr $C$DW$497, DW_AT_data_member_location[DW_OP_plus_uconst 0x6c]
	.dwattr $C$DW$497, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$498	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$498, DW_AT_name("ICR")
	.dwattr $C$DW$498, DW_AT_TI_symbol_name("_ICR")
	.dwattr $C$DW$498, DW_AT_data_member_location[DW_OP_plus_uconst 0x70]
	.dwattr $C$DW$498, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$499	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$499, DW_AT_name("ICRH")
	.dwattr $C$DW$499, DW_AT_TI_symbol_name("_ICRH")
	.dwattr $C$DW$499, DW_AT_data_member_location[DW_OP_plus_uconst 0x74]
	.dwattr $C$DW$499, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$500	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$500, DW_AT_name("IEVAL")
	.dwattr $C$DW$500, DW_AT_TI_symbol_name("_IEVAL")
	.dwattr $C$DW$500, DW_AT_data_member_location[DW_OP_plus_uconst 0x78]
	.dwattr $C$DW$500, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$501	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$55)
	.dwattr $C$DW$501, DW_AT_name("RSVD1")
	.dwattr $C$DW$501, DW_AT_TI_symbol_name("_RSVD1")
	.dwattr $C$DW$501, DW_AT_data_member_location[DW_OP_plus_uconst 0x7c]
	.dwattr $C$DW$501, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$502	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$502, DW_AT_name("QER")
	.dwattr $C$DW$502, DW_AT_TI_symbol_name("_QER")
	.dwattr $C$DW$502, DW_AT_data_member_location[DW_OP_plus_uconst 0x80]
	.dwattr $C$DW$502, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$503	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$503, DW_AT_name("QEER")
	.dwattr $C$DW$503, DW_AT_TI_symbol_name("_QEER")
	.dwattr $C$DW$503, DW_AT_data_member_location[DW_OP_plus_uconst 0x84]
	.dwattr $C$DW$503, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$504	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$504, DW_AT_name("QEECR")
	.dwattr $C$DW$504, DW_AT_TI_symbol_name("_QEECR")
	.dwattr $C$DW$504, DW_AT_data_member_location[DW_OP_plus_uconst 0x88]
	.dwattr $C$DW$504, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$505	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$505, DW_AT_name("QEESR")
	.dwattr $C$DW$505, DW_AT_TI_symbol_name("_QEESR")
	.dwattr $C$DW$505, DW_AT_data_member_location[DW_OP_plus_uconst 0x8c]
	.dwattr $C$DW$505, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$506	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$506, DW_AT_name("QSER")
	.dwattr $C$DW$506, DW_AT_TI_symbol_name("_QSER")
	.dwattr $C$DW$506, DW_AT_data_member_location[DW_OP_plus_uconst 0x90]
	.dwattr $C$DW$506, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$507	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$507, DW_AT_name("QSECR")
	.dwattr $C$DW$507, DW_AT_TI_symbol_name("_QSECR")
	.dwattr $C$DW$507, DW_AT_data_member_location[DW_OP_plus_uconst 0x94]
	.dwattr $C$DW$507, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$508	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$73)
	.dwattr $C$DW$508, DW_AT_name("RSVD2")
	.dwattr $C$DW$508, DW_AT_TI_symbol_name("_RSVD2")
	.dwattr $C$DW$508, DW_AT_data_member_location[DW_OP_plus_uconst 0x98]
	.dwattr $C$DW$508, DW_AT_accessibility(DW_ACCESS_public)
	.dwendtag $C$DW$T$74

$C$DW$T$90	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_EdmaccShadowRegs")
	.dwattr $C$DW$T$90, DW_AT_type(*$C$DW$T$74)
	.dwattr $C$DW$T$90, DW_AT_language(DW_LANG_C)

$C$DW$T$91	.dwtag  DW_TAG_array_type, DW_AT_type(*$C$DW$T$90)
	.dwattr $C$DW$T$91, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$91, DW_AT_byte_size(0x1000)
$C$DW$509	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$509, DW_AT_upper_bound(0x07)
	.dwendtag $C$DW$T$91

$C$DW$T$229	.dwtag  DW_TAG_volatile_type
	.dwattr $C$DW$T$229, DW_AT_type(*$C$DW$T$90)
$C$DW$T$230	.dwtag  DW_TAG_pointer_type, DW_AT_type(*$C$DW$T$229)
	.dwattr $C$DW$T$230, DW_AT_address_class(0x20)
$C$DW$T$231	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_EdmaccShadowRegsOvly")
	.dwattr $C$DW$T$231, DW_AT_type(*$C$DW$T$230)
	.dwattr $C$DW$T$231, DW_AT_language(DW_LANG_C)

$C$DW$T$75	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$75, DW_AT_byte_size(0x20)
$C$DW$510	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$510, DW_AT_name("OPT")
	.dwattr $C$DW$510, DW_AT_TI_symbol_name("_OPT")
	.dwattr $C$DW$510, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$510, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$511	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$511, DW_AT_name("SRC")
	.dwattr $C$DW$511, DW_AT_TI_symbol_name("_SRC")
	.dwattr $C$DW$511, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$511, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$512	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$512, DW_AT_name("A_B_CNT")
	.dwattr $C$DW$512, DW_AT_TI_symbol_name("_A_B_CNT")
	.dwattr $C$DW$512, DW_AT_data_member_location[DW_OP_plus_uconst 0x8]
	.dwattr $C$DW$512, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$513	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$513, DW_AT_name("DST")
	.dwattr $C$DW$513, DW_AT_TI_symbol_name("_DST")
	.dwattr $C$DW$513, DW_AT_data_member_location[DW_OP_plus_uconst 0xc]
	.dwattr $C$DW$513, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$514	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$514, DW_AT_name("SRC_DST_BIDX")
	.dwattr $C$DW$514, DW_AT_TI_symbol_name("_SRC_DST_BIDX")
	.dwattr $C$DW$514, DW_AT_data_member_location[DW_OP_plus_uconst 0x10]
	.dwattr $C$DW$514, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$515	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$515, DW_AT_name("LINK_BCNTRLD")
	.dwattr $C$DW$515, DW_AT_TI_symbol_name("_LINK_BCNTRLD")
	.dwattr $C$DW$515, DW_AT_data_member_location[DW_OP_plus_uconst 0x14]
	.dwattr $C$DW$515, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$516	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$516, DW_AT_name("SRC_DST_CIDX")
	.dwattr $C$DW$516, DW_AT_TI_symbol_name("_SRC_DST_CIDX")
	.dwattr $C$DW$516, DW_AT_data_member_location[DW_OP_plus_uconst 0x18]
	.dwattr $C$DW$516, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$517	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$517, DW_AT_name("CCNT")
	.dwattr $C$DW$517, DW_AT_TI_symbol_name("_CCNT")
	.dwattr $C$DW$517, DW_AT_data_member_location[DW_OP_plus_uconst 0x1c]
	.dwattr $C$DW$517, DW_AT_accessibility(DW_ACCESS_public)
	.dwendtag $C$DW$T$75

$C$DW$T$93	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_EdmaccParamentryRegs")
	.dwattr $C$DW$T$93, DW_AT_type(*$C$DW$T$75)
	.dwattr $C$DW$T$93, DW_AT_language(DW_LANG_C)

$C$DW$T$94	.dwtag  DW_TAG_array_type, DW_AT_type(*$C$DW$T$93)
	.dwattr $C$DW$T$94, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$94, DW_AT_byte_size(0x4000)
$C$DW$518	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$518, DW_AT_upper_bound(0x1ff)
	.dwendtag $C$DW$T$94

$C$DW$T$232	.dwtag  DW_TAG_volatile_type
	.dwattr $C$DW$T$232, DW_AT_type(*$C$DW$T$93)
$C$DW$T$233	.dwtag  DW_TAG_pointer_type, DW_AT_type(*$C$DW$T$232)
	.dwattr $C$DW$T$233, DW_AT_address_class(0x20)
$C$DW$T$234	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_EdmaccParamentryRegsOvly")
	.dwattr $C$DW$T$234, DW_AT_type(*$C$DW$T$233)
	.dwattr $C$DW$T$234, DW_AT_language(DW_LANG_C)

$C$DW$T$95	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$95, DW_AT_byte_size(0x8000)
$C$DW$519	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$519, DW_AT_name("REV")
	.dwattr $C$DW$519, DW_AT_TI_symbol_name("_REV")
	.dwattr $C$DW$519, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$519, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$520	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$520, DW_AT_name("CCCFG")
	.dwattr $C$DW$520, DW_AT_TI_symbol_name("_CCCFG")
	.dwattr $C$DW$520, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$520, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$521	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$76)
	.dwattr $C$DW$521, DW_AT_name("RSVD0")
	.dwattr $C$DW$521, DW_AT_TI_symbol_name("_RSVD0")
	.dwattr $C$DW$521, DW_AT_data_member_location[DW_OP_plus_uconst 0x8]
	.dwattr $C$DW$521, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$522	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$77)
	.dwattr $C$DW$522, DW_AT_name("DCHMAP")
	.dwattr $C$DW$522, DW_AT_TI_symbol_name("_DCHMAP")
	.dwattr $C$DW$522, DW_AT_data_member_location[DW_OP_plus_uconst 0x100]
	.dwattr $C$DW$522, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$523	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$22)
	.dwattr $C$DW$523, DW_AT_name("QCHMAP")
	.dwattr $C$DW$523, DW_AT_TI_symbol_name("_QCHMAP")
	.dwattr $C$DW$523, DW_AT_data_member_location[DW_OP_plus_uconst 0x200]
	.dwattr $C$DW$523, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$524	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$78)
	.dwattr $C$DW$524, DW_AT_name("RSVD1")
	.dwattr $C$DW$524, DW_AT_TI_symbol_name("_RSVD1")
	.dwattr $C$DW$524, DW_AT_data_member_location[DW_OP_plus_uconst 0x220]
	.dwattr $C$DW$524, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$525	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$22)
	.dwattr $C$DW$525, DW_AT_name("DMAQNUM")
	.dwattr $C$DW$525, DW_AT_TI_symbol_name("_DMAQNUM")
	.dwattr $C$DW$525, DW_AT_data_member_location[DW_OP_plus_uconst 0x240]
	.dwattr $C$DW$525, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$526	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$526, DW_AT_name("QDMAQNUM")
	.dwattr $C$DW$526, DW_AT_TI_symbol_name("_QDMAQNUM")
	.dwattr $C$DW$526, DW_AT_data_member_location[DW_OP_plus_uconst 0x260]
	.dwattr $C$DW$526, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$527	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$69)
	.dwattr $C$DW$527, DW_AT_name("RSVD2")
	.dwattr $C$DW$527, DW_AT_TI_symbol_name("_RSVD2")
	.dwattr $C$DW$527, DW_AT_data_member_location[DW_OP_plus_uconst 0x264]
	.dwattr $C$DW$527, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$528	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$528, DW_AT_name("QUETCMAP")
	.dwattr $C$DW$528, DW_AT_TI_symbol_name("_QUETCMAP")
	.dwattr $C$DW$528, DW_AT_data_member_location[DW_OP_plus_uconst 0x280]
	.dwattr $C$DW$528, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$529	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$529, DW_AT_name("QUEPRI")
	.dwattr $C$DW$529, DW_AT_TI_symbol_name("_QUEPRI")
	.dwattr $C$DW$529, DW_AT_data_member_location[DW_OP_plus_uconst 0x284]
	.dwattr $C$DW$529, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$530	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$79)
	.dwattr $C$DW$530, DW_AT_name("RSVD3")
	.dwattr $C$DW$530, DW_AT_TI_symbol_name("_RSVD3")
	.dwattr $C$DW$530, DW_AT_data_member_location[DW_OP_plus_uconst 0x288]
	.dwattr $C$DW$530, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$531	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$531, DW_AT_name("EMR")
	.dwattr $C$DW$531, DW_AT_TI_symbol_name("_EMR")
	.dwattr $C$DW$531, DW_AT_data_member_location[DW_OP_plus_uconst 0x300]
	.dwattr $C$DW$531, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$532	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$532, DW_AT_name("EMRH")
	.dwattr $C$DW$532, DW_AT_TI_symbol_name("_EMRH")
	.dwattr $C$DW$532, DW_AT_data_member_location[DW_OP_plus_uconst 0x304]
	.dwattr $C$DW$532, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$533	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$533, DW_AT_name("EMCR")
	.dwattr $C$DW$533, DW_AT_TI_symbol_name("_EMCR")
	.dwattr $C$DW$533, DW_AT_data_member_location[DW_OP_plus_uconst 0x308]
	.dwattr $C$DW$533, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$534	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$534, DW_AT_name("EMCRH")
	.dwattr $C$DW$534, DW_AT_TI_symbol_name("_EMCRH")
	.dwattr $C$DW$534, DW_AT_data_member_location[DW_OP_plus_uconst 0x30c]
	.dwattr $C$DW$534, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$535	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$535, DW_AT_name("QEMR")
	.dwattr $C$DW$535, DW_AT_TI_symbol_name("_QEMR")
	.dwattr $C$DW$535, DW_AT_data_member_location[DW_OP_plus_uconst 0x310]
	.dwattr $C$DW$535, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$536	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$536, DW_AT_name("QEMCR")
	.dwattr $C$DW$536, DW_AT_TI_symbol_name("_QEMCR")
	.dwattr $C$DW$536, DW_AT_data_member_location[DW_OP_plus_uconst 0x314]
	.dwattr $C$DW$536, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$537	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$537, DW_AT_name("CCERR")
	.dwattr $C$DW$537, DW_AT_TI_symbol_name("_CCERR")
	.dwattr $C$DW$537, DW_AT_data_member_location[DW_OP_plus_uconst 0x318]
	.dwattr $C$DW$537, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$538	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$538, DW_AT_name("CCERRCLR")
	.dwattr $C$DW$538, DW_AT_TI_symbol_name("_CCERRCLR")
	.dwattr $C$DW$538, DW_AT_data_member_location[DW_OP_plus_uconst 0x31c]
	.dwattr $C$DW$538, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$539	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$539, DW_AT_name("EEVAL")
	.dwattr $C$DW$539, DW_AT_TI_symbol_name("_EEVAL")
	.dwattr $C$DW$539, DW_AT_data_member_location[DW_OP_plus_uconst 0x320]
	.dwattr $C$DW$539, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$540	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$69)
	.dwattr $C$DW$540, DW_AT_name("RSVD4")
	.dwattr $C$DW$540, DW_AT_TI_symbol_name("_RSVD4")
	.dwattr $C$DW$540, DW_AT_data_member_location[DW_OP_plus_uconst 0x324]
	.dwattr $C$DW$540, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$541	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$81)
	.dwattr $C$DW$541, DW_AT_name("DRA")
	.dwattr $C$DW$541, DW_AT_TI_symbol_name("_DRA")
	.dwattr $C$DW$541, DW_AT_data_member_location[DW_OP_plus_uconst 0x340]
	.dwattr $C$DW$541, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$542	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$22)
	.dwattr $C$DW$542, DW_AT_name("QRAE")
	.dwattr $C$DW$542, DW_AT_TI_symbol_name("_QRAE")
	.dwattr $C$DW$542, DW_AT_data_member_location[DW_OP_plus_uconst 0x380]
	.dwattr $C$DW$542, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$543	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$82)
	.dwattr $C$DW$543, DW_AT_name("RSVD5")
	.dwattr $C$DW$543, DW_AT_TI_symbol_name("_RSVD5")
	.dwattr $C$DW$543, DW_AT_data_member_location[DW_OP_plus_uconst 0x3a0]
	.dwattr $C$DW$543, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$544	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$85)
	.dwattr $C$DW$544, DW_AT_name("QUEEVTENTRY")
	.dwattr $C$DW$544, DW_AT_TI_symbol_name("_QUEEVTENTRY")
	.dwattr $C$DW$544, DW_AT_data_member_location[DW_OP_plus_uconst 0x400]
	.dwattr $C$DW$544, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$545	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$22)
	.dwattr $C$DW$545, DW_AT_name("QSTAT")
	.dwattr $C$DW$545, DW_AT_TI_symbol_name("_QSTAT")
	.dwattr $C$DW$545, DW_AT_data_member_location[DW_OP_plus_uconst 0x600]
	.dwattr $C$DW$545, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$546	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$546, DW_AT_name("QWMTHRA")
	.dwattr $C$DW$546, DW_AT_TI_symbol_name("_QWMTHRA")
	.dwattr $C$DW$546, DW_AT_data_member_location[DW_OP_plus_uconst 0x620]
	.dwattr $C$DW$546, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$547	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$547, DW_AT_name("QWMTHRB")
	.dwattr $C$DW$547, DW_AT_TI_symbol_name("_QWMTHRB")
	.dwattr $C$DW$547, DW_AT_data_member_location[DW_OP_plus_uconst 0x624]
	.dwattr $C$DW$547, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$548	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$65)
	.dwattr $C$DW$548, DW_AT_name("RSVD6")
	.dwattr $C$DW$548, DW_AT_TI_symbol_name("_RSVD6")
	.dwattr $C$DW$548, DW_AT_data_member_location[DW_OP_plus_uconst 0x628]
	.dwattr $C$DW$548, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$549	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$549, DW_AT_name("CCSTAT")
	.dwattr $C$DW$549, DW_AT_TI_symbol_name("_CCSTAT")
	.dwattr $C$DW$549, DW_AT_data_member_location[DW_OP_plus_uconst 0x640]
	.dwattr $C$DW$549, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$550	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$86)
	.dwattr $C$DW$550, DW_AT_name("RSVD7")
	.dwattr $C$DW$550, DW_AT_TI_symbol_name("_RSVD7")
	.dwattr $C$DW$550, DW_AT_data_member_location[DW_OP_plus_uconst 0x644]
	.dwattr $C$DW$550, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$551	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$551, DW_AT_name("AETCTL")
	.dwattr $C$DW$551, DW_AT_TI_symbol_name("_AETCTL")
	.dwattr $C$DW$551, DW_AT_data_member_location[DW_OP_plus_uconst 0x700]
	.dwattr $C$DW$551, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$552	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$552, DW_AT_name("AETSTAT")
	.dwattr $C$DW$552, DW_AT_TI_symbol_name("_AETSTAT")
	.dwattr $C$DW$552, DW_AT_data_member_location[DW_OP_plus_uconst 0x704]
	.dwattr $C$DW$552, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$553	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$553, DW_AT_name("AETCMD")
	.dwattr $C$DW$553, DW_AT_TI_symbol_name("_AETCMD")
	.dwattr $C$DW$553, DW_AT_data_member_location[DW_OP_plus_uconst 0x708]
	.dwattr $C$DW$553, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$554	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$87)
	.dwattr $C$DW$554, DW_AT_name("RSVD8")
	.dwattr $C$DW$554, DW_AT_TI_symbol_name("_RSVD8")
	.dwattr $C$DW$554, DW_AT_data_member_location[DW_OP_plus_uconst 0x70c]
	.dwattr $C$DW$554, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$555	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$555, DW_AT_name("MPFAR")
	.dwattr $C$DW$555, DW_AT_TI_symbol_name("_MPFAR")
	.dwattr $C$DW$555, DW_AT_data_member_location[DW_OP_plus_uconst 0x800]
	.dwattr $C$DW$555, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$556	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$556, DW_AT_name("MPFSR")
	.dwattr $C$DW$556, DW_AT_TI_symbol_name("_MPFSR")
	.dwattr $C$DW$556, DW_AT_data_member_location[DW_OP_plus_uconst 0x804]
	.dwattr $C$DW$556, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$557	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$557, DW_AT_name("MPFCR")
	.dwattr $C$DW$557, DW_AT_TI_symbol_name("_MPFCR")
	.dwattr $C$DW$557, DW_AT_data_member_location[DW_OP_plus_uconst 0x808]
	.dwattr $C$DW$557, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$558	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$558, DW_AT_name("MPPAG")
	.dwattr $C$DW$558, DW_AT_TI_symbol_name("_MPPAG")
	.dwattr $C$DW$558, DW_AT_data_member_location[DW_OP_plus_uconst 0x80c]
	.dwattr $C$DW$558, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$559	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$22)
	.dwattr $C$DW$559, DW_AT_name("MPPA")
	.dwattr $C$DW$559, DW_AT_TI_symbol_name("_MPPA")
	.dwattr $C$DW$559, DW_AT_data_member_location[DW_OP_plus_uconst 0x810]
	.dwattr $C$DW$559, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$560	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$88)
	.dwattr $C$DW$560, DW_AT_name("RSVD9")
	.dwattr $C$DW$560, DW_AT_TI_symbol_name("_RSVD9")
	.dwattr $C$DW$560, DW_AT_data_member_location[DW_OP_plus_uconst 0x830]
	.dwattr $C$DW$560, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$561	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$561, DW_AT_name("ER")
	.dwattr $C$DW$561, DW_AT_TI_symbol_name("_ER")
	.dwattr $C$DW$561, DW_AT_data_member_location[DW_OP_plus_uconst 0x1000]
	.dwattr $C$DW$561, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$562	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$562, DW_AT_name("ERH")
	.dwattr $C$DW$562, DW_AT_TI_symbol_name("_ERH")
	.dwattr $C$DW$562, DW_AT_data_member_location[DW_OP_plus_uconst 0x1004]
	.dwattr $C$DW$562, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$563	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$563, DW_AT_name("ECR")
	.dwattr $C$DW$563, DW_AT_TI_symbol_name("_ECR")
	.dwattr $C$DW$563, DW_AT_data_member_location[DW_OP_plus_uconst 0x1008]
	.dwattr $C$DW$563, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$564	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$564, DW_AT_name("ECRH")
	.dwattr $C$DW$564, DW_AT_TI_symbol_name("_ECRH")
	.dwattr $C$DW$564, DW_AT_data_member_location[DW_OP_plus_uconst 0x100c]
	.dwattr $C$DW$564, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$565	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$565, DW_AT_name("ESR")
	.dwattr $C$DW$565, DW_AT_TI_symbol_name("_ESR")
	.dwattr $C$DW$565, DW_AT_data_member_location[DW_OP_plus_uconst 0x1010]
	.dwattr $C$DW$565, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$566	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$566, DW_AT_name("ESRH")
	.dwattr $C$DW$566, DW_AT_TI_symbol_name("_ESRH")
	.dwattr $C$DW$566, DW_AT_data_member_location[DW_OP_plus_uconst 0x1014]
	.dwattr $C$DW$566, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$567	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$567, DW_AT_name("CER")
	.dwattr $C$DW$567, DW_AT_TI_symbol_name("_CER")
	.dwattr $C$DW$567, DW_AT_data_member_location[DW_OP_plus_uconst 0x1018]
	.dwattr $C$DW$567, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$568	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$568, DW_AT_name("CERH")
	.dwattr $C$DW$568, DW_AT_TI_symbol_name("_CERH")
	.dwattr $C$DW$568, DW_AT_data_member_location[DW_OP_plus_uconst 0x101c]
	.dwattr $C$DW$568, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$569	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$569, DW_AT_name("EER")
	.dwattr $C$DW$569, DW_AT_TI_symbol_name("_EER")
	.dwattr $C$DW$569, DW_AT_data_member_location[DW_OP_plus_uconst 0x1020]
	.dwattr $C$DW$569, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$570	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$570, DW_AT_name("EERH")
	.dwattr $C$DW$570, DW_AT_TI_symbol_name("_EERH")
	.dwattr $C$DW$570, DW_AT_data_member_location[DW_OP_plus_uconst 0x1024]
	.dwattr $C$DW$570, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$571	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$571, DW_AT_name("EECR")
	.dwattr $C$DW$571, DW_AT_TI_symbol_name("_EECR")
	.dwattr $C$DW$571, DW_AT_data_member_location[DW_OP_plus_uconst 0x1028]
	.dwattr $C$DW$571, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$572	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$572, DW_AT_name("EECRH")
	.dwattr $C$DW$572, DW_AT_TI_symbol_name("_EECRH")
	.dwattr $C$DW$572, DW_AT_data_member_location[DW_OP_plus_uconst 0x102c]
	.dwattr $C$DW$572, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$573	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$573, DW_AT_name("EESR")
	.dwattr $C$DW$573, DW_AT_TI_symbol_name("_EESR")
	.dwattr $C$DW$573, DW_AT_data_member_location[DW_OP_plus_uconst 0x1030]
	.dwattr $C$DW$573, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$574	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$574, DW_AT_name("EESRH")
	.dwattr $C$DW$574, DW_AT_TI_symbol_name("_EESRH")
	.dwattr $C$DW$574, DW_AT_data_member_location[DW_OP_plus_uconst 0x1034]
	.dwattr $C$DW$574, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$575	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$575, DW_AT_name("SER")
	.dwattr $C$DW$575, DW_AT_TI_symbol_name("_SER")
	.dwattr $C$DW$575, DW_AT_data_member_location[DW_OP_plus_uconst 0x1038]
	.dwattr $C$DW$575, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$576	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$576, DW_AT_name("SERH")
	.dwattr $C$DW$576, DW_AT_TI_symbol_name("_SERH")
	.dwattr $C$DW$576, DW_AT_data_member_location[DW_OP_plus_uconst 0x103c]
	.dwattr $C$DW$576, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$577	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$577, DW_AT_name("SECR")
	.dwattr $C$DW$577, DW_AT_TI_symbol_name("_SECR")
	.dwattr $C$DW$577, DW_AT_data_member_location[DW_OP_plus_uconst 0x1040]
	.dwattr $C$DW$577, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$578	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$578, DW_AT_name("SECRH")
	.dwattr $C$DW$578, DW_AT_TI_symbol_name("_SECRH")
	.dwattr $C$DW$578, DW_AT_data_member_location[DW_OP_plus_uconst 0x1044]
	.dwattr $C$DW$578, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$579	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$62)
	.dwattr $C$DW$579, DW_AT_name("RSVD10")
	.dwattr $C$DW$579, DW_AT_TI_symbol_name("_RSVD10")
	.dwattr $C$DW$579, DW_AT_data_member_location[DW_OP_plus_uconst 0x1048]
	.dwattr $C$DW$579, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$580	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$580, DW_AT_name("IER")
	.dwattr $C$DW$580, DW_AT_TI_symbol_name("_IER")
	.dwattr $C$DW$580, DW_AT_data_member_location[DW_OP_plus_uconst 0x1050]
	.dwattr $C$DW$580, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$581	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$581, DW_AT_name("IERH")
	.dwattr $C$DW$581, DW_AT_TI_symbol_name("_IERH")
	.dwattr $C$DW$581, DW_AT_data_member_location[DW_OP_plus_uconst 0x1054]
	.dwattr $C$DW$581, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$582	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$582, DW_AT_name("IECR")
	.dwattr $C$DW$582, DW_AT_TI_symbol_name("_IECR")
	.dwattr $C$DW$582, DW_AT_data_member_location[DW_OP_plus_uconst 0x1058]
	.dwattr $C$DW$582, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$583	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$583, DW_AT_name("IECRH")
	.dwattr $C$DW$583, DW_AT_TI_symbol_name("_IECRH")
	.dwattr $C$DW$583, DW_AT_data_member_location[DW_OP_plus_uconst 0x105c]
	.dwattr $C$DW$583, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$584	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$584, DW_AT_name("IESR")
	.dwattr $C$DW$584, DW_AT_TI_symbol_name("_IESR")
	.dwattr $C$DW$584, DW_AT_data_member_location[DW_OP_plus_uconst 0x1060]
	.dwattr $C$DW$584, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$585	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$585, DW_AT_name("IESRH")
	.dwattr $C$DW$585, DW_AT_TI_symbol_name("_IESRH")
	.dwattr $C$DW$585, DW_AT_data_member_location[DW_OP_plus_uconst 0x1064]
	.dwattr $C$DW$585, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$586	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$586, DW_AT_name("IPR")
	.dwattr $C$DW$586, DW_AT_TI_symbol_name("_IPR")
	.dwattr $C$DW$586, DW_AT_data_member_location[DW_OP_plus_uconst 0x1068]
	.dwattr $C$DW$586, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$587	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$587, DW_AT_name("IPRH")
	.dwattr $C$DW$587, DW_AT_TI_symbol_name("_IPRH")
	.dwattr $C$DW$587, DW_AT_data_member_location[DW_OP_plus_uconst 0x106c]
	.dwattr $C$DW$587, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$588	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$588, DW_AT_name("ICR")
	.dwattr $C$DW$588, DW_AT_TI_symbol_name("_ICR")
	.dwattr $C$DW$588, DW_AT_data_member_location[DW_OP_plus_uconst 0x1070]
	.dwattr $C$DW$588, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$589	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$589, DW_AT_name("ICRH")
	.dwattr $C$DW$589, DW_AT_TI_symbol_name("_ICRH")
	.dwattr $C$DW$589, DW_AT_data_member_location[DW_OP_plus_uconst 0x1074]
	.dwattr $C$DW$589, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$590	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$590, DW_AT_name("IEVAL")
	.dwattr $C$DW$590, DW_AT_TI_symbol_name("_IEVAL")
	.dwattr $C$DW$590, DW_AT_data_member_location[DW_OP_plus_uconst 0x1078]
	.dwattr $C$DW$590, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$591	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$55)
	.dwattr $C$DW$591, DW_AT_name("RSVD11")
	.dwattr $C$DW$591, DW_AT_TI_symbol_name("_RSVD11")
	.dwattr $C$DW$591, DW_AT_data_member_location[DW_OP_plus_uconst 0x107c]
	.dwattr $C$DW$591, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$592	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$592, DW_AT_name("QER")
	.dwattr $C$DW$592, DW_AT_TI_symbol_name("_QER")
	.dwattr $C$DW$592, DW_AT_data_member_location[DW_OP_plus_uconst 0x1080]
	.dwattr $C$DW$592, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$593	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$593, DW_AT_name("QEER")
	.dwattr $C$DW$593, DW_AT_TI_symbol_name("_QEER")
	.dwattr $C$DW$593, DW_AT_data_member_location[DW_OP_plus_uconst 0x1084]
	.dwattr $C$DW$593, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$594	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$594, DW_AT_name("QEECR")
	.dwattr $C$DW$594, DW_AT_TI_symbol_name("_QEECR")
	.dwattr $C$DW$594, DW_AT_data_member_location[DW_OP_plus_uconst 0x1088]
	.dwattr $C$DW$594, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$595	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$595, DW_AT_name("QEESR")
	.dwattr $C$DW$595, DW_AT_TI_symbol_name("_QEESR")
	.dwattr $C$DW$595, DW_AT_data_member_location[DW_OP_plus_uconst 0x108c]
	.dwattr $C$DW$595, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$596	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$596, DW_AT_name("QSER")
	.dwattr $C$DW$596, DW_AT_TI_symbol_name("_QSER")
	.dwattr $C$DW$596, DW_AT_data_member_location[DW_OP_plus_uconst 0x1090]
	.dwattr $C$DW$596, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$597	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$597, DW_AT_name("QSECR")
	.dwattr $C$DW$597, DW_AT_TI_symbol_name("_QSECR")
	.dwattr $C$DW$597, DW_AT_data_member_location[DW_OP_plus_uconst 0x1094]
	.dwattr $C$DW$597, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$598	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$89)
	.dwattr $C$DW$598, DW_AT_name("RSVD12")
	.dwattr $C$DW$598, DW_AT_TI_symbol_name("_RSVD12")
	.dwattr $C$DW$598, DW_AT_data_member_location[DW_OP_plus_uconst 0x1098]
	.dwattr $C$DW$598, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$599	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$91)
	.dwattr $C$DW$599, DW_AT_name("SHADOW")
	.dwattr $C$DW$599, DW_AT_TI_symbol_name("_SHADOW")
	.dwattr $C$DW$599, DW_AT_data_member_location[DW_OP_plus_uconst 0x2000]
	.dwattr $C$DW$599, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$600	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$92)
	.dwattr $C$DW$600, DW_AT_name("RSVD13")
	.dwattr $C$DW$600, DW_AT_TI_symbol_name("_RSVD13")
	.dwattr $C$DW$600, DW_AT_data_member_location[DW_OP_plus_uconst 0x3000]
	.dwattr $C$DW$600, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$601	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$94)
	.dwattr $C$DW$601, DW_AT_name("PARAMENTRY")
	.dwattr $C$DW$601, DW_AT_TI_symbol_name("_PARAMENTRY")
	.dwattr $C$DW$601, DW_AT_data_member_location[DW_OP_plus_uconst 0x4000]
	.dwattr $C$DW$601, DW_AT_accessibility(DW_ACCESS_public)
	.dwendtag $C$DW$T$95

$C$DW$T$235	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_EdmaccRegs")
	.dwattr $C$DW$T$235, DW_AT_type(*$C$DW$T$95)
	.dwattr $C$DW$T$235, DW_AT_language(DW_LANG_C)
$C$DW$T$236	.dwtag  DW_TAG_volatile_type
	.dwattr $C$DW$T$236, DW_AT_type(*$C$DW$T$235)
$C$DW$T$237	.dwtag  DW_TAG_pointer_type, DW_AT_type(*$C$DW$T$236)
	.dwattr $C$DW$T$237, DW_AT_address_class(0x20)
$C$DW$T$238	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_EdmaccRegsOvly")
	.dwattr $C$DW$T$238, DW_AT_type(*$C$DW$T$237)
	.dwattr $C$DW$T$238, DW_AT_language(DW_LANG_C)

$C$DW$T$97	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$97, DW_AT_byte_size(0x40)
$C$DW$602	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$602, DW_AT_name("DFOPT")
	.dwattr $C$DW$602, DW_AT_TI_symbol_name("_DFOPT")
	.dwattr $C$DW$602, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$602, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$603	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$603, DW_AT_name("DFSRC")
	.dwattr $C$DW$603, DW_AT_TI_symbol_name("_DFSRC")
	.dwattr $C$DW$603, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$603, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$604	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$604, DW_AT_name("DFCNT")
	.dwattr $C$DW$604, DW_AT_TI_symbol_name("_DFCNT")
	.dwattr $C$DW$604, DW_AT_data_member_location[DW_OP_plus_uconst 0x8]
	.dwattr $C$DW$604, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$605	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$605, DW_AT_name("DFDST")
	.dwattr $C$DW$605, DW_AT_TI_symbol_name("_DFDST")
	.dwattr $C$DW$605, DW_AT_data_member_location[DW_OP_plus_uconst 0xc]
	.dwattr $C$DW$605, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$606	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$606, DW_AT_name("DFBIDX")
	.dwattr $C$DW$606, DW_AT_TI_symbol_name("_DFBIDX")
	.dwattr $C$DW$606, DW_AT_data_member_location[DW_OP_plus_uconst 0x10]
	.dwattr $C$DW$606, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$607	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$607, DW_AT_name("DFMPPRXY")
	.dwattr $C$DW$607, DW_AT_TI_symbol_name("_DFMPPRXY")
	.dwattr $C$DW$607, DW_AT_data_member_location[DW_OP_plus_uconst 0x14]
	.dwattr $C$DW$607, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$608	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$96)
	.dwattr $C$DW$608, DW_AT_name("RSVD0")
	.dwattr $C$DW$608, DW_AT_TI_symbol_name("_RSVD0")
	.dwattr $C$DW$608, DW_AT_data_member_location[DW_OP_plus_uconst 0x18]
	.dwattr $C$DW$608, DW_AT_accessibility(DW_ACCESS_public)
	.dwendtag $C$DW$T$97

$C$DW$T$99	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_EdmatcDfiregRegs")
	.dwattr $C$DW$T$99, DW_AT_type(*$C$DW$T$97)
	.dwattr $C$DW$T$99, DW_AT_language(DW_LANG_C)

$C$DW$T$100	.dwtag  DW_TAG_array_type, DW_AT_type(*$C$DW$T$99)
	.dwattr $C$DW$T$100, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$100, DW_AT_byte_size(0x100)
$C$DW$609	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$609, DW_AT_upper_bound(0x03)
	.dwendtag $C$DW$T$100


$C$DW$T$101	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$101, DW_AT_byte_size(0x400)
$C$DW$610	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$610, DW_AT_name("REV")
	.dwattr $C$DW$610, DW_AT_TI_symbol_name("_REV")
	.dwattr $C$DW$610, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$610, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$611	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$611, DW_AT_name("TCCFG")
	.dwattr $C$DW$611, DW_AT_TI_symbol_name("_TCCFG")
	.dwattr $C$DW$611, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$611, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$612	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$76)
	.dwattr $C$DW$612, DW_AT_name("RSVD0")
	.dwattr $C$DW$612, DW_AT_TI_symbol_name("_RSVD0")
	.dwattr $C$DW$612, DW_AT_data_member_location[DW_OP_plus_uconst 0x8]
	.dwattr $C$DW$612, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$613	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$613, DW_AT_name("TPTCSTAT")
	.dwattr $C$DW$613, DW_AT_TI_symbol_name("_TPTCSTAT")
	.dwattr $C$DW$613, DW_AT_data_member_location[DW_OP_plus_uconst 0x100]
	.dwattr $C$DW$613, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$614	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$614, DW_AT_name("INTSTAT")
	.dwattr $C$DW$614, DW_AT_TI_symbol_name("_INTSTAT")
	.dwattr $C$DW$614, DW_AT_data_member_location[DW_OP_plus_uconst 0x104]
	.dwattr $C$DW$614, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$615	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$615, DW_AT_name("INTEN")
	.dwattr $C$DW$615, DW_AT_TI_symbol_name("_INTEN")
	.dwattr $C$DW$615, DW_AT_data_member_location[DW_OP_plus_uconst 0x108]
	.dwattr $C$DW$615, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$616	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$616, DW_AT_name("INTCLR")
	.dwattr $C$DW$616, DW_AT_TI_symbol_name("_INTCLR")
	.dwattr $C$DW$616, DW_AT_data_member_location[DW_OP_plus_uconst 0x10c]
	.dwattr $C$DW$616, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$617	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$617, DW_AT_name("INTCMD")
	.dwattr $C$DW$617, DW_AT_TI_symbol_name("_INTCMD")
	.dwattr $C$DW$617, DW_AT_data_member_location[DW_OP_plus_uconst 0x110]
	.dwattr $C$DW$617, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$618	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$56)
	.dwattr $C$DW$618, DW_AT_name("RSVD1")
	.dwattr $C$DW$618, DW_AT_TI_symbol_name("_RSVD1")
	.dwattr $C$DW$618, DW_AT_data_member_location[DW_OP_plus_uconst 0x114]
	.dwattr $C$DW$618, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$619	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$619, DW_AT_name("ERRSTAT")
	.dwattr $C$DW$619, DW_AT_TI_symbol_name("_ERRSTAT")
	.dwattr $C$DW$619, DW_AT_data_member_location[DW_OP_plus_uconst 0x120]
	.dwattr $C$DW$619, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$620	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$620, DW_AT_name("ERREN")
	.dwattr $C$DW$620, DW_AT_TI_symbol_name("_ERREN")
	.dwattr $C$DW$620, DW_AT_data_member_location[DW_OP_plus_uconst 0x124]
	.dwattr $C$DW$620, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$621	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$621, DW_AT_name("ERRCLR")
	.dwattr $C$DW$621, DW_AT_TI_symbol_name("_ERRCLR")
	.dwattr $C$DW$621, DW_AT_data_member_location[DW_OP_plus_uconst 0x128]
	.dwattr $C$DW$621, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$622	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$622, DW_AT_name("ERRDET")
	.dwattr $C$DW$622, DW_AT_TI_symbol_name("_ERRDET")
	.dwattr $C$DW$622, DW_AT_data_member_location[DW_OP_plus_uconst 0x12c]
	.dwattr $C$DW$622, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$623	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$623, DW_AT_name("ERRCMD")
	.dwattr $C$DW$623, DW_AT_TI_symbol_name("_ERRCMD")
	.dwattr $C$DW$623, DW_AT_data_member_location[DW_OP_plus_uconst 0x130]
	.dwattr $C$DW$623, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$624	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$56)
	.dwattr $C$DW$624, DW_AT_name("RSVD2")
	.dwattr $C$DW$624, DW_AT_TI_symbol_name("_RSVD2")
	.dwattr $C$DW$624, DW_AT_data_member_location[DW_OP_plus_uconst 0x134]
	.dwattr $C$DW$624, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$625	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$625, DW_AT_name("RDRATE")
	.dwattr $C$DW$625, DW_AT_TI_symbol_name("_RDRATE")
	.dwattr $C$DW$625, DW_AT_data_member_location[DW_OP_plus_uconst 0x140]
	.dwattr $C$DW$625, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$626	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$86)
	.dwattr $C$DW$626, DW_AT_name("RSVD3")
	.dwattr $C$DW$626, DW_AT_TI_symbol_name("_RSVD3")
	.dwattr $C$DW$626, DW_AT_data_member_location[DW_OP_plus_uconst 0x144]
	.dwattr $C$DW$626, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$627	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$627, DW_AT_name("POPT")
	.dwattr $C$DW$627, DW_AT_TI_symbol_name("_POPT")
	.dwattr $C$DW$627, DW_AT_data_member_location[DW_OP_plus_uconst 0x200]
	.dwattr $C$DW$627, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$628	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$628, DW_AT_name("PSRC")
	.dwattr $C$DW$628, DW_AT_TI_symbol_name("_PSRC")
	.dwattr $C$DW$628, DW_AT_data_member_location[DW_OP_plus_uconst 0x204]
	.dwattr $C$DW$628, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$629	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$629, DW_AT_name("PCNT")
	.dwattr $C$DW$629, DW_AT_TI_symbol_name("_PCNT")
	.dwattr $C$DW$629, DW_AT_data_member_location[DW_OP_plus_uconst 0x208]
	.dwattr $C$DW$629, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$630	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$630, DW_AT_name("PDST")
	.dwattr $C$DW$630, DW_AT_TI_symbol_name("_PDST")
	.dwattr $C$DW$630, DW_AT_data_member_location[DW_OP_plus_uconst 0x20c]
	.dwattr $C$DW$630, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$631	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$631, DW_AT_name("PBIDX")
	.dwattr $C$DW$631, DW_AT_TI_symbol_name("_PBIDX")
	.dwattr $C$DW$631, DW_AT_data_member_location[DW_OP_plus_uconst 0x210]
	.dwattr $C$DW$631, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$632	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$632, DW_AT_name("PMPPRXY")
	.dwattr $C$DW$632, DW_AT_TI_symbol_name("_PMPPRXY")
	.dwattr $C$DW$632, DW_AT_data_member_location[DW_OP_plus_uconst 0x214]
	.dwattr $C$DW$632, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$633	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$96)
	.dwattr $C$DW$633, DW_AT_name("RSVD4")
	.dwattr $C$DW$633, DW_AT_TI_symbol_name("_RSVD4")
	.dwattr $C$DW$633, DW_AT_data_member_location[DW_OP_plus_uconst 0x218]
	.dwattr $C$DW$633, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$634	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$634, DW_AT_name("SAOPT")
	.dwattr $C$DW$634, DW_AT_TI_symbol_name("_SAOPT")
	.dwattr $C$DW$634, DW_AT_data_member_location[DW_OP_plus_uconst 0x240]
	.dwattr $C$DW$634, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$635	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$635, DW_AT_name("SASRC")
	.dwattr $C$DW$635, DW_AT_TI_symbol_name("_SASRC")
	.dwattr $C$DW$635, DW_AT_data_member_location[DW_OP_plus_uconst 0x244]
	.dwattr $C$DW$635, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$636	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$636, DW_AT_name("SACNT")
	.dwattr $C$DW$636, DW_AT_TI_symbol_name("_SACNT")
	.dwattr $C$DW$636, DW_AT_data_member_location[DW_OP_plus_uconst 0x248]
	.dwattr $C$DW$636, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$637	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$637, DW_AT_name("SADST")
	.dwattr $C$DW$637, DW_AT_TI_symbol_name("_SADST")
	.dwattr $C$DW$637, DW_AT_data_member_location[DW_OP_plus_uconst 0x24c]
	.dwattr $C$DW$637, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$638	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$638, DW_AT_name("SABIDX")
	.dwattr $C$DW$638, DW_AT_TI_symbol_name("_SABIDX")
	.dwattr $C$DW$638, DW_AT_data_member_location[DW_OP_plus_uconst 0x250]
	.dwattr $C$DW$638, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$639	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$639, DW_AT_name("SAMPPRXY")
	.dwattr $C$DW$639, DW_AT_TI_symbol_name("_SAMPPRXY")
	.dwattr $C$DW$639, DW_AT_data_member_location[DW_OP_plus_uconst 0x254]
	.dwattr $C$DW$639, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$640	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$640, DW_AT_name("SACNTRLD")
	.dwattr $C$DW$640, DW_AT_TI_symbol_name("_SACNTRLD")
	.dwattr $C$DW$640, DW_AT_data_member_location[DW_OP_plus_uconst 0x258]
	.dwattr $C$DW$640, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$641	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$641, DW_AT_name("SASRCBREF")
	.dwattr $C$DW$641, DW_AT_TI_symbol_name("_SASRCBREF")
	.dwattr $C$DW$641, DW_AT_data_member_location[DW_OP_plus_uconst 0x25c]
	.dwattr $C$DW$641, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$642	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$642, DW_AT_name("SADSTBREF")
	.dwattr $C$DW$642, DW_AT_TI_symbol_name("_SADSTBREF")
	.dwattr $C$DW$642, DW_AT_data_member_location[DW_OP_plus_uconst 0x260]
	.dwattr $C$DW$642, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$643	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$69)
	.dwattr $C$DW$643, DW_AT_name("RSVD5")
	.dwattr $C$DW$643, DW_AT_TI_symbol_name("_RSVD5")
	.dwattr $C$DW$643, DW_AT_data_member_location[DW_OP_plus_uconst 0x264]
	.dwattr $C$DW$643, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$644	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$644, DW_AT_name("DFCNTRLD")
	.dwattr $C$DW$644, DW_AT_TI_symbol_name("_DFCNTRLD")
	.dwattr $C$DW$644, DW_AT_data_member_location[DW_OP_plus_uconst 0x280]
	.dwattr $C$DW$644, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$645	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$645, DW_AT_name("DFSRCBREF")
	.dwattr $C$DW$645, DW_AT_TI_symbol_name("_DFSRCBREF")
	.dwattr $C$DW$645, DW_AT_data_member_location[DW_OP_plus_uconst 0x284]
	.dwattr $C$DW$645, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$646	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$646, DW_AT_name("DFDSTBREF")
	.dwattr $C$DW$646, DW_AT_TI_symbol_name("_DFDSTBREF")
	.dwattr $C$DW$646, DW_AT_data_member_location[DW_OP_plus_uconst 0x288]
	.dwattr $C$DW$646, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$647	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$98)
	.dwattr $C$DW$647, DW_AT_name("RSVD6")
	.dwattr $C$DW$647, DW_AT_TI_symbol_name("_RSVD6")
	.dwattr $C$DW$647, DW_AT_data_member_location[DW_OP_plus_uconst 0x28c]
	.dwattr $C$DW$647, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$648	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$100)
	.dwattr $C$DW$648, DW_AT_name("DFIREG")
	.dwattr $C$DW$648, DW_AT_TI_symbol_name("_DFIREG")
	.dwattr $C$DW$648, DW_AT_data_member_location[DW_OP_plus_uconst 0x300]
	.dwattr $C$DW$648, DW_AT_accessibility(DW_ACCESS_public)
	.dwendtag $C$DW$T$101

$C$DW$T$239	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_EdmatcRegs")
	.dwattr $C$DW$T$239, DW_AT_type(*$C$DW$T$101)
	.dwattr $C$DW$T$239, DW_AT_language(DW_LANG_C)
$C$DW$T$240	.dwtag  DW_TAG_volatile_type
	.dwattr $C$DW$T$240, DW_AT_type(*$C$DW$T$239)
$C$DW$T$241	.dwtag  DW_TAG_pointer_type, DW_AT_type(*$C$DW$T$240)
	.dwattr $C$DW$T$241, DW_AT_address_class(0x20)
$C$DW$T$242	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_EdmatcRegsOvly")
	.dwattr $C$DW$T$242, DW_AT_type(*$C$DW$T$241)
	.dwattr $C$DW$T$242, DW_AT_language(DW_LANG_C)

$C$DW$T$102	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$102, DW_AT_byte_size(0x2c)
$C$DW$649	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$649, DW_AT_name("PID12")
	.dwattr $C$DW$649, DW_AT_TI_symbol_name("_PID12")
	.dwattr $C$DW$649, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$649, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$650	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$650, DW_AT_name("EMUMGT_CLKSPD")
	.dwattr $C$DW$650, DW_AT_TI_symbol_name("_EMUMGT_CLKSPD")
	.dwattr $C$DW$650, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$650, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$651	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$651, DW_AT_name("GPINT_GPEN")
	.dwattr $C$DW$651, DW_AT_TI_symbol_name("_GPINT_GPEN")
	.dwattr $C$DW$651, DW_AT_data_member_location[DW_OP_plus_uconst 0x8]
	.dwattr $C$DW$651, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$652	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$652, DW_AT_name("GPTDAT_GPDIR")
	.dwattr $C$DW$652, DW_AT_TI_symbol_name("_GPTDAT_GPDIR")
	.dwattr $C$DW$652, DW_AT_data_member_location[DW_OP_plus_uconst 0xc]
	.dwattr $C$DW$652, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$653	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$653, DW_AT_name("TIM12")
	.dwattr $C$DW$653, DW_AT_TI_symbol_name("_TIM12")
	.dwattr $C$DW$653, DW_AT_data_member_location[DW_OP_plus_uconst 0x10]
	.dwattr $C$DW$653, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$654	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$654, DW_AT_name("TIM34")
	.dwattr $C$DW$654, DW_AT_TI_symbol_name("_TIM34")
	.dwattr $C$DW$654, DW_AT_data_member_location[DW_OP_plus_uconst 0x14]
	.dwattr $C$DW$654, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$655	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$655, DW_AT_name("PRD12")
	.dwattr $C$DW$655, DW_AT_TI_symbol_name("_PRD12")
	.dwattr $C$DW$655, DW_AT_data_member_location[DW_OP_plus_uconst 0x18]
	.dwattr $C$DW$655, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$656	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$656, DW_AT_name("PRD34")
	.dwattr $C$DW$656, DW_AT_TI_symbol_name("_PRD34")
	.dwattr $C$DW$656, DW_AT_data_member_location[DW_OP_plus_uconst 0x1c]
	.dwattr $C$DW$656, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$657	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$657, DW_AT_name("TCR")
	.dwattr $C$DW$657, DW_AT_TI_symbol_name("_TCR")
	.dwattr $C$DW$657, DW_AT_data_member_location[DW_OP_plus_uconst 0x20]
	.dwattr $C$DW$657, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$658	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$658, DW_AT_name("TGCR")
	.dwattr $C$DW$658, DW_AT_TI_symbol_name("_TGCR")
	.dwattr $C$DW$658, DW_AT_data_member_location[DW_OP_plus_uconst 0x24]
	.dwattr $C$DW$658, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$659	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$659, DW_AT_name("WDTCR")
	.dwattr $C$DW$659, DW_AT_TI_symbol_name("_WDTCR")
	.dwattr $C$DW$659, DW_AT_data_member_location[DW_OP_plus_uconst 0x28]
	.dwattr $C$DW$659, DW_AT_accessibility(DW_ACCESS_public)
	.dwendtag $C$DW$T$102

$C$DW$T$243	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_TmrRegs")
	.dwattr $C$DW$T$243, DW_AT_type(*$C$DW$T$102)
	.dwattr $C$DW$T$243, DW_AT_language(DW_LANG_C)
$C$DW$T$244	.dwtag  DW_TAG_volatile_type
	.dwattr $C$DW$T$244, DW_AT_type(*$C$DW$T$243)
$C$DW$T$245	.dwtag  DW_TAG_pointer_type, DW_AT_type(*$C$DW$T$244)
	.dwattr $C$DW$T$245, DW_AT_address_class(0x20)
$C$DW$T$246	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_TmrRegsOvly")
	.dwattr $C$DW$T$246, DW_AT_type(*$C$DW$T$245)
	.dwattr $C$DW$T$246, DW_AT_language(DW_LANG_C)

$C$DW$T$103	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$103, DW_AT_byte_size(0x44)
$C$DW$660	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$660, DW_AT_name("PID")
	.dwattr $C$DW$660, DW_AT_TI_symbol_name("_PID")
	.dwattr $C$DW$660, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$660, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$661	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$661, DW_AT_name("PWREMU_MGMT")
	.dwattr $C$DW$661, DW_AT_TI_symbol_name("_PWREMU_MGMT")
	.dwattr $C$DW$661, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$661, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$662	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$662, DW_AT_name("GeneralPurposeInterruptControlregister")
	.dwattr $C$DW$662, DW_AT_TI_symbol_name("_GeneralPurposeInterruptControlregister")
	.dwattr $C$DW$662, DW_AT_data_member_location[DW_OP_plus_uconst 0x8]
	.dwattr $C$DW$662, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$663	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$663, DW_AT_name("GPIO_EN")
	.dwattr $C$DW$663, DW_AT_TI_symbol_name("_GPIO_EN")
	.dwattr $C$DW$663, DW_AT_data_member_location[DW_OP_plus_uconst 0xc]
	.dwattr $C$DW$663, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$664	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$664, DW_AT_name("GPIO_DIR1")
	.dwattr $C$DW$664, DW_AT_TI_symbol_name("_GPIO_DIR1")
	.dwattr $C$DW$664, DW_AT_data_member_location[DW_OP_plus_uconst 0x10]
	.dwattr $C$DW$664, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$665	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$665, DW_AT_name("GPIO_DAT1")
	.dwattr $C$DW$665, DW_AT_TI_symbol_name("_GPIO_DAT1")
	.dwattr $C$DW$665, DW_AT_data_member_location[DW_OP_plus_uconst 0x14]
	.dwattr $C$DW$665, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$666	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$666, DW_AT_name("GPIO_DIR2")
	.dwattr $C$DW$666, DW_AT_TI_symbol_name("_GPIO_DIR2")
	.dwattr $C$DW$666, DW_AT_data_member_location[DW_OP_plus_uconst 0x18]
	.dwattr $C$DW$666, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$667	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$667, DW_AT_name("GPIO_DAT2")
	.dwattr $C$DW$667, DW_AT_TI_symbol_name("_GPIO_DAT2")
	.dwattr $C$DW$667, DW_AT_data_member_location[DW_OP_plus_uconst 0x1c]
	.dwattr $C$DW$667, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$668	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$668, DW_AT_name("GPIO_DIR3")
	.dwattr $C$DW$668, DW_AT_TI_symbol_name("_GPIO_DIR3")
	.dwattr $C$DW$668, DW_AT_data_member_location[DW_OP_plus_uconst 0x20]
	.dwattr $C$DW$668, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$669	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$669, DW_AT_name("GPIO_DAT3")
	.dwattr $C$DW$669, DW_AT_TI_symbol_name("_GPIO_DAT3")
	.dwattr $C$DW$669, DW_AT_data_member_location[DW_OP_plus_uconst 0x24]
	.dwattr $C$DW$669, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$670	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$670, DW_AT_name("RESV1")
	.dwattr $C$DW$670, DW_AT_TI_symbol_name("_RESV1")
	.dwattr $C$DW$670, DW_AT_data_member_location[DW_OP_plus_uconst 0x28]
	.dwattr $C$DW$670, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$671	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$671, DW_AT_name("RESV2")
	.dwattr $C$DW$671, DW_AT_TI_symbol_name("_RESV2")
	.dwattr $C$DW$671, DW_AT_data_member_location[DW_OP_plus_uconst 0x2c]
	.dwattr $C$DW$671, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$672	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$672, DW_AT_name("HPIC")
	.dwattr $C$DW$672, DW_AT_TI_symbol_name("_HPIC")
	.dwattr $C$DW$672, DW_AT_data_member_location[DW_OP_plus_uconst 0x30]
	.dwattr $C$DW$672, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$673	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$673, DW_AT_name("HPIAW")
	.dwattr $C$DW$673, DW_AT_TI_symbol_name("_HPIAW")
	.dwattr $C$DW$673, DW_AT_data_member_location[DW_OP_plus_uconst 0x34]
	.dwattr $C$DW$673, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$674	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$674, DW_AT_name("HPIAR")
	.dwattr $C$DW$674, DW_AT_TI_symbol_name("_HPIAR")
	.dwattr $C$DW$674, DW_AT_data_member_location[DW_OP_plus_uconst 0x38]
	.dwattr $C$DW$674, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$675	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$675, DW_AT_name("XHPIAW")
	.dwattr $C$DW$675, DW_AT_TI_symbol_name("_XHPIAW")
	.dwattr $C$DW$675, DW_AT_data_member_location[DW_OP_plus_uconst 0x3c]
	.dwattr $C$DW$675, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$676	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$676, DW_AT_name("XHPIAR")
	.dwattr $C$DW$676, DW_AT_TI_symbol_name("_XHPIAR")
	.dwattr $C$DW$676, DW_AT_data_member_location[DW_OP_plus_uconst 0x40]
	.dwattr $C$DW$676, DW_AT_accessibility(DW_ACCESS_public)
	.dwendtag $C$DW$T$103

$C$DW$T$247	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_UhpiRegs")
	.dwattr $C$DW$T$247, DW_AT_type(*$C$DW$T$103)
	.dwattr $C$DW$T$247, DW_AT_language(DW_LANG_C)
$C$DW$T$248	.dwtag  DW_TAG_volatile_type
	.dwattr $C$DW$T$248, DW_AT_type(*$C$DW$T$247)
$C$DW$T$249	.dwtag  DW_TAG_pointer_type, DW_AT_type(*$C$DW$T$248)
	.dwattr $C$DW$T$249, DW_AT_address_class(0x20)
$C$DW$T$250	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_UhpiRegsOvly")
	.dwattr $C$DW$T$250, DW_AT_type(*$C$DW$T$249)
	.dwattr $C$DW$T$250, DW_AT_language(DW_LANG_C)

$C$DW$T$108	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$108, DW_AT_byte_size(0x680)
$C$DW$677	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$677, DW_AT_name("TXIDVER")
	.dwattr $C$DW$677, DW_AT_TI_symbol_name("_TXIDVER")
	.dwattr $C$DW$677, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$677, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$678	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$678, DW_AT_name("TXCONTROL")
	.dwattr $C$DW$678, DW_AT_TI_symbol_name("_TXCONTROL")
	.dwattr $C$DW$678, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$678, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$679	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$679, DW_AT_name("TXTEARDOWN")
	.dwattr $C$DW$679, DW_AT_TI_symbol_name("_TXTEARDOWN")
	.dwattr $C$DW$679, DW_AT_data_member_location[DW_OP_plus_uconst 0x8]
	.dwattr $C$DW$679, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$680	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$55)
	.dwattr $C$DW$680, DW_AT_name("RSVD0")
	.dwattr $C$DW$680, DW_AT_TI_symbol_name("_RSVD0")
	.dwattr $C$DW$680, DW_AT_data_member_location[DW_OP_plus_uconst 0xc]
	.dwattr $C$DW$680, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$681	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$681, DW_AT_name("RXIDVER")
	.dwattr $C$DW$681, DW_AT_TI_symbol_name("_RXIDVER")
	.dwattr $C$DW$681, DW_AT_data_member_location[DW_OP_plus_uconst 0x10]
	.dwattr $C$DW$681, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$682	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$682, DW_AT_name("RXCONTROL")
	.dwattr $C$DW$682, DW_AT_TI_symbol_name("_RXCONTROL")
	.dwattr $C$DW$682, DW_AT_data_member_location[DW_OP_plus_uconst 0x14]
	.dwattr $C$DW$682, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$683	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$683, DW_AT_name("RXTEARDOWN")
	.dwattr $C$DW$683, DW_AT_TI_symbol_name("_RXTEARDOWN")
	.dwattr $C$DW$683, DW_AT_data_member_location[DW_OP_plus_uconst 0x18]
	.dwattr $C$DW$683, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$684	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$104)
	.dwattr $C$DW$684, DW_AT_name("RSVD1")
	.dwattr $C$DW$684, DW_AT_TI_symbol_name("_RSVD1")
	.dwattr $C$DW$684, DW_AT_data_member_location[DW_OP_plus_uconst 0x1c]
	.dwattr $C$DW$684, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$685	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$685, DW_AT_name("TXINTSTATRAW")
	.dwattr $C$DW$685, DW_AT_TI_symbol_name("_TXINTSTATRAW")
	.dwattr $C$DW$685, DW_AT_data_member_location[DW_OP_plus_uconst 0x80]
	.dwattr $C$DW$685, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$686	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$686, DW_AT_name("TXINTSTATMASKED")
	.dwattr $C$DW$686, DW_AT_TI_symbol_name("_TXINTSTATMASKED")
	.dwattr $C$DW$686, DW_AT_data_member_location[DW_OP_plus_uconst 0x84]
	.dwattr $C$DW$686, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$687	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$687, DW_AT_name("TXINTMASKSET")
	.dwattr $C$DW$687, DW_AT_TI_symbol_name("_TXINTMASKSET")
	.dwattr $C$DW$687, DW_AT_data_member_location[DW_OP_plus_uconst 0x88]
	.dwattr $C$DW$687, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$688	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$688, DW_AT_name("TXINTMASKCLEAR")
	.dwattr $C$DW$688, DW_AT_TI_symbol_name("_TXINTMASKCLEAR")
	.dwattr $C$DW$688, DW_AT_data_member_location[DW_OP_plus_uconst 0x8c]
	.dwattr $C$DW$688, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$689	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$689, DW_AT_name("MACINVECTOR")
	.dwattr $C$DW$689, DW_AT_TI_symbol_name("_MACINVECTOR")
	.dwattr $C$DW$689, DW_AT_data_member_location[DW_OP_plus_uconst 0x90]
	.dwattr $C$DW$689, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$690	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$56)
	.dwattr $C$DW$690, DW_AT_name("RSVD2")
	.dwattr $C$DW$690, DW_AT_TI_symbol_name("_RSVD2")
	.dwattr $C$DW$690, DW_AT_data_member_location[DW_OP_plus_uconst 0x94]
	.dwattr $C$DW$690, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$691	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$691, DW_AT_name("RXINTSTATRAW")
	.dwattr $C$DW$691, DW_AT_TI_symbol_name("_RXINTSTATRAW")
	.dwattr $C$DW$691, DW_AT_data_member_location[DW_OP_plus_uconst 0xa0]
	.dwattr $C$DW$691, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$692	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$692, DW_AT_name("RXINTSTATMASKED")
	.dwattr $C$DW$692, DW_AT_TI_symbol_name("_RXINTSTATMASKED")
	.dwattr $C$DW$692, DW_AT_data_member_location[DW_OP_plus_uconst 0xa4]
	.dwattr $C$DW$692, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$693	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$693, DW_AT_name("RXINTMASKSET")
	.dwattr $C$DW$693, DW_AT_TI_symbol_name("_RXINTMASKSET")
	.dwattr $C$DW$693, DW_AT_data_member_location[DW_OP_plus_uconst 0xa8]
	.dwattr $C$DW$693, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$694	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$694, DW_AT_name("RXINTMASKCLEAR")
	.dwattr $C$DW$694, DW_AT_TI_symbol_name("_RXINTMASKCLEAR")
	.dwattr $C$DW$694, DW_AT_data_member_location[DW_OP_plus_uconst 0xac]
	.dwattr $C$DW$694, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$695	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$695, DW_AT_name("MACINTSTATRAW")
	.dwattr $C$DW$695, DW_AT_TI_symbol_name("_MACINTSTATRAW")
	.dwattr $C$DW$695, DW_AT_data_member_location[DW_OP_plus_uconst 0xb0]
	.dwattr $C$DW$695, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$696	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$696, DW_AT_name("MACINTSTATMASKED")
	.dwattr $C$DW$696, DW_AT_TI_symbol_name("_MACINTSTATMASKED")
	.dwattr $C$DW$696, DW_AT_data_member_location[DW_OP_plus_uconst 0xb4]
	.dwattr $C$DW$696, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$697	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$697, DW_AT_name("MACINTMASKSET")
	.dwattr $C$DW$697, DW_AT_TI_symbol_name("_MACINTMASKSET")
	.dwattr $C$DW$697, DW_AT_data_member_location[DW_OP_plus_uconst 0xb8]
	.dwattr $C$DW$697, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$698	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$698, DW_AT_name("MACINTMASKCLEAR")
	.dwattr $C$DW$698, DW_AT_TI_symbol_name("_MACINTMASKCLEAR")
	.dwattr $C$DW$698, DW_AT_data_member_location[DW_OP_plus_uconst 0xbc]
	.dwattr $C$DW$698, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$699	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$105)
	.dwattr $C$DW$699, DW_AT_name("RSVD3")
	.dwattr $C$DW$699, DW_AT_TI_symbol_name("_RSVD3")
	.dwattr $C$DW$699, DW_AT_data_member_location[DW_OP_plus_uconst 0xc0]
	.dwattr $C$DW$699, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$700	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$700, DW_AT_name("RXMBPENABLE")
	.dwattr $C$DW$700, DW_AT_TI_symbol_name("_RXMBPENABLE")
	.dwattr $C$DW$700, DW_AT_data_member_location[DW_OP_plus_uconst 0x100]
	.dwattr $C$DW$700, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$701	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$701, DW_AT_name("RXUNICASTSET")
	.dwattr $C$DW$701, DW_AT_TI_symbol_name("_RXUNICASTSET")
	.dwattr $C$DW$701, DW_AT_data_member_location[DW_OP_plus_uconst 0x104]
	.dwattr $C$DW$701, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$702	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$702, DW_AT_name("RXUNICASTCLEAR")
	.dwattr $C$DW$702, DW_AT_TI_symbol_name("_RXUNICASTCLEAR")
	.dwattr $C$DW$702, DW_AT_data_member_location[DW_OP_plus_uconst 0x108]
	.dwattr $C$DW$702, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$703	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$703, DW_AT_name("RXMAXLEN")
	.dwattr $C$DW$703, DW_AT_TI_symbol_name("_RXMAXLEN")
	.dwattr $C$DW$703, DW_AT_data_member_location[DW_OP_plus_uconst 0x10c]
	.dwattr $C$DW$703, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$704	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$704, DW_AT_name("RXBUFFEROFFSET")
	.dwattr $C$DW$704, DW_AT_TI_symbol_name("_RXBUFFEROFFSET")
	.dwattr $C$DW$704, DW_AT_data_member_location[DW_OP_plus_uconst 0x110]
	.dwattr $C$DW$704, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$705	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$705, DW_AT_name("RXFILTERLOWTHRESH")
	.dwattr $C$DW$705, DW_AT_TI_symbol_name("_RXFILTERLOWTHRESH")
	.dwattr $C$DW$705, DW_AT_data_member_location[DW_OP_plus_uconst 0x114]
	.dwattr $C$DW$705, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$706	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$62)
	.dwattr $C$DW$706, DW_AT_name("RSVD4")
	.dwattr $C$DW$706, DW_AT_TI_symbol_name("_RSVD4")
	.dwattr $C$DW$706, DW_AT_data_member_location[DW_OP_plus_uconst 0x118]
	.dwattr $C$DW$706, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$707	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$707, DW_AT_name("RX0FLOWTHRESH")
	.dwattr $C$DW$707, DW_AT_TI_symbol_name("_RX0FLOWTHRESH")
	.dwattr $C$DW$707, DW_AT_data_member_location[DW_OP_plus_uconst 0x120]
	.dwattr $C$DW$707, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$708	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$708, DW_AT_name("RX1FLOWTHRESH")
	.dwattr $C$DW$708, DW_AT_TI_symbol_name("_RX1FLOWTHRESH")
	.dwattr $C$DW$708, DW_AT_data_member_location[DW_OP_plus_uconst 0x124]
	.dwattr $C$DW$708, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$709	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$709, DW_AT_name("RX2FLOWTHRESH")
	.dwattr $C$DW$709, DW_AT_TI_symbol_name("_RX2FLOWTHRESH")
	.dwattr $C$DW$709, DW_AT_data_member_location[DW_OP_plus_uconst 0x128]
	.dwattr $C$DW$709, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$710	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$710, DW_AT_name("RX3FLOWTHRESH")
	.dwattr $C$DW$710, DW_AT_TI_symbol_name("_RX3FLOWTHRESH")
	.dwattr $C$DW$710, DW_AT_data_member_location[DW_OP_plus_uconst 0x12c]
	.dwattr $C$DW$710, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$711	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$711, DW_AT_name("RX4FLOWTHRESH")
	.dwattr $C$DW$711, DW_AT_TI_symbol_name("_RX4FLOWTHRESH")
	.dwattr $C$DW$711, DW_AT_data_member_location[DW_OP_plus_uconst 0x130]
	.dwattr $C$DW$711, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$712	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$712, DW_AT_name("RX5FLOWTHRESH")
	.dwattr $C$DW$712, DW_AT_TI_symbol_name("_RX5FLOWTHRESH")
	.dwattr $C$DW$712, DW_AT_data_member_location[DW_OP_plus_uconst 0x134]
	.dwattr $C$DW$712, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$713	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$713, DW_AT_name("RX6FLOWTHRESH")
	.dwattr $C$DW$713, DW_AT_TI_symbol_name("_RX6FLOWTHRESH")
	.dwattr $C$DW$713, DW_AT_data_member_location[DW_OP_plus_uconst 0x138]
	.dwattr $C$DW$713, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$714	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$714, DW_AT_name("RX7FLOWTHRESH")
	.dwattr $C$DW$714, DW_AT_TI_symbol_name("_RX7FLOWTHRESH")
	.dwattr $C$DW$714, DW_AT_data_member_location[DW_OP_plus_uconst 0x13c]
	.dwattr $C$DW$714, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$715	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$715, DW_AT_name("RX0FREEBUFFER")
	.dwattr $C$DW$715, DW_AT_TI_symbol_name("_RX0FREEBUFFER")
	.dwattr $C$DW$715, DW_AT_data_member_location[DW_OP_plus_uconst 0x140]
	.dwattr $C$DW$715, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$716	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$716, DW_AT_name("RX1FREEBUFFER")
	.dwattr $C$DW$716, DW_AT_TI_symbol_name("_RX1FREEBUFFER")
	.dwattr $C$DW$716, DW_AT_data_member_location[DW_OP_plus_uconst 0x144]
	.dwattr $C$DW$716, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$717	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$717, DW_AT_name("RX2FREEBUFFER")
	.dwattr $C$DW$717, DW_AT_TI_symbol_name("_RX2FREEBUFFER")
	.dwattr $C$DW$717, DW_AT_data_member_location[DW_OP_plus_uconst 0x148]
	.dwattr $C$DW$717, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$718	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$718, DW_AT_name("RX3FREEBUFFER")
	.dwattr $C$DW$718, DW_AT_TI_symbol_name("_RX3FREEBUFFER")
	.dwattr $C$DW$718, DW_AT_data_member_location[DW_OP_plus_uconst 0x14c]
	.dwattr $C$DW$718, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$719	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$719, DW_AT_name("RX4FREEBUFFER")
	.dwattr $C$DW$719, DW_AT_TI_symbol_name("_RX4FREEBUFFER")
	.dwattr $C$DW$719, DW_AT_data_member_location[DW_OP_plus_uconst 0x150]
	.dwattr $C$DW$719, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$720	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$720, DW_AT_name("RX5FREEBUFFER")
	.dwattr $C$DW$720, DW_AT_TI_symbol_name("_RX5FREEBUFFER")
	.dwattr $C$DW$720, DW_AT_data_member_location[DW_OP_plus_uconst 0x154]
	.dwattr $C$DW$720, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$721	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$721, DW_AT_name("RX6FREEBUFFER")
	.dwattr $C$DW$721, DW_AT_TI_symbol_name("_RX6FREEBUFFER")
	.dwattr $C$DW$721, DW_AT_data_member_location[DW_OP_plus_uconst 0x158]
	.dwattr $C$DW$721, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$722	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$722, DW_AT_name("RX7FREEBUFFER")
	.dwattr $C$DW$722, DW_AT_TI_symbol_name("_RX7FREEBUFFER")
	.dwattr $C$DW$722, DW_AT_data_member_location[DW_OP_plus_uconst 0x15c]
	.dwattr $C$DW$722, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$723	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$723, DW_AT_name("MACCONTROL")
	.dwattr $C$DW$723, DW_AT_TI_symbol_name("_MACCONTROL")
	.dwattr $C$DW$723, DW_AT_data_member_location[DW_OP_plus_uconst 0x160]
	.dwattr $C$DW$723, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$724	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$724, DW_AT_name("MACSTATUS")
	.dwattr $C$DW$724, DW_AT_TI_symbol_name("_MACSTATUS")
	.dwattr $C$DW$724, DW_AT_data_member_location[DW_OP_plus_uconst 0x164]
	.dwattr $C$DW$724, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$725	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$725, DW_AT_name("EMCONTROL")
	.dwattr $C$DW$725, DW_AT_TI_symbol_name("_EMCONTROL")
	.dwattr $C$DW$725, DW_AT_data_member_location[DW_OP_plus_uconst 0x168]
	.dwattr $C$DW$725, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$726	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$726, DW_AT_name("FIFOCONTROL")
	.dwattr $C$DW$726, DW_AT_TI_symbol_name("_FIFOCONTROL")
	.dwattr $C$DW$726, DW_AT_data_member_location[DW_OP_plus_uconst 0x16c]
	.dwattr $C$DW$726, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$727	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$727, DW_AT_name("MACCONFIG")
	.dwattr $C$DW$727, DW_AT_TI_symbol_name("_MACCONFIG")
	.dwattr $C$DW$727, DW_AT_data_member_location[DW_OP_plus_uconst 0x170]
	.dwattr $C$DW$727, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$728	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$728, DW_AT_name("SOFTRESET")
	.dwattr $C$DW$728, DW_AT_TI_symbol_name("_SOFTRESET")
	.dwattr $C$DW$728, DW_AT_data_member_location[DW_OP_plus_uconst 0x174]
	.dwattr $C$DW$728, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$729	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$106)
	.dwattr $C$DW$729, DW_AT_name("RSVD5")
	.dwattr $C$DW$729, DW_AT_TI_symbol_name("_RSVD5")
	.dwattr $C$DW$729, DW_AT_data_member_location[DW_OP_plus_uconst 0x178]
	.dwattr $C$DW$729, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$730	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$730, DW_AT_name("MACSRCADDRLO")
	.dwattr $C$DW$730, DW_AT_TI_symbol_name("_MACSRCADDRLO")
	.dwattr $C$DW$730, DW_AT_data_member_location[DW_OP_plus_uconst 0x1d0]
	.dwattr $C$DW$730, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$731	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$731, DW_AT_name("MACSRCADDRHI")
	.dwattr $C$DW$731, DW_AT_TI_symbol_name("_MACSRCADDRHI")
	.dwattr $C$DW$731, DW_AT_data_member_location[DW_OP_plus_uconst 0x1d4]
	.dwattr $C$DW$731, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$732	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$732, DW_AT_name("MACHASH1")
	.dwattr $C$DW$732, DW_AT_TI_symbol_name("_MACHASH1")
	.dwattr $C$DW$732, DW_AT_data_member_location[DW_OP_plus_uconst 0x1d8]
	.dwattr $C$DW$732, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$733	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$733, DW_AT_name("MACHASH2")
	.dwattr $C$DW$733, DW_AT_TI_symbol_name("_MACHASH2")
	.dwattr $C$DW$733, DW_AT_data_member_location[DW_OP_plus_uconst 0x1dc]
	.dwattr $C$DW$733, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$734	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$734, DW_AT_name("BOFFTEST")
	.dwattr $C$DW$734, DW_AT_TI_symbol_name("_BOFFTEST")
	.dwattr $C$DW$734, DW_AT_data_member_location[DW_OP_plus_uconst 0x1e0]
	.dwattr $C$DW$734, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$735	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$735, DW_AT_name("TPACETEST")
	.dwattr $C$DW$735, DW_AT_TI_symbol_name("_TPACETEST")
	.dwattr $C$DW$735, DW_AT_data_member_location[DW_OP_plus_uconst 0x1e4]
	.dwattr $C$DW$735, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$736	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$736, DW_AT_name("RXPAUSE")
	.dwattr $C$DW$736, DW_AT_TI_symbol_name("_RXPAUSE")
	.dwattr $C$DW$736, DW_AT_data_member_location[DW_OP_plus_uconst 0x1e8]
	.dwattr $C$DW$736, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$737	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$737, DW_AT_name("TXPAUSE")
	.dwattr $C$DW$737, DW_AT_TI_symbol_name("_TXPAUSE")
	.dwattr $C$DW$737, DW_AT_data_member_location[DW_OP_plus_uconst 0x1ec]
	.dwattr $C$DW$737, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$738	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$64)
	.dwattr $C$DW$738, DW_AT_name("RSVD6")
	.dwattr $C$DW$738, DW_AT_TI_symbol_name("_RSVD6")
	.dwattr $C$DW$738, DW_AT_data_member_location[DW_OP_plus_uconst 0x1f0]
	.dwattr $C$DW$738, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$739	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$739, DW_AT_name("RXGOODFRAMES")
	.dwattr $C$DW$739, DW_AT_TI_symbol_name("_RXGOODFRAMES")
	.dwattr $C$DW$739, DW_AT_data_member_location[DW_OP_plus_uconst 0x200]
	.dwattr $C$DW$739, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$740	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$740, DW_AT_name("RXBCASTFRAMES")
	.dwattr $C$DW$740, DW_AT_TI_symbol_name("_RXBCASTFRAMES")
	.dwattr $C$DW$740, DW_AT_data_member_location[DW_OP_plus_uconst 0x204]
	.dwattr $C$DW$740, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$741	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$741, DW_AT_name("RXMCASTFRAMES")
	.dwattr $C$DW$741, DW_AT_TI_symbol_name("_RXMCASTFRAMES")
	.dwattr $C$DW$741, DW_AT_data_member_location[DW_OP_plus_uconst 0x208]
	.dwattr $C$DW$741, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$742	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$742, DW_AT_name("RXPAUSEFRAMES")
	.dwattr $C$DW$742, DW_AT_TI_symbol_name("_RXPAUSEFRAMES")
	.dwattr $C$DW$742, DW_AT_data_member_location[DW_OP_plus_uconst 0x20c]
	.dwattr $C$DW$742, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$743	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$743, DW_AT_name("RXCRCERRORS")
	.dwattr $C$DW$743, DW_AT_TI_symbol_name("_RXCRCERRORS")
	.dwattr $C$DW$743, DW_AT_data_member_location[DW_OP_plus_uconst 0x210]
	.dwattr $C$DW$743, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$744	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$744, DW_AT_name("RXALIGNCODEERRORS")
	.dwattr $C$DW$744, DW_AT_TI_symbol_name("_RXALIGNCODEERRORS")
	.dwattr $C$DW$744, DW_AT_data_member_location[DW_OP_plus_uconst 0x214]
	.dwattr $C$DW$744, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$745	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$745, DW_AT_name("RXOVERSIZED")
	.dwattr $C$DW$745, DW_AT_TI_symbol_name("_RXOVERSIZED")
	.dwattr $C$DW$745, DW_AT_data_member_location[DW_OP_plus_uconst 0x218]
	.dwattr $C$DW$745, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$746	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$746, DW_AT_name("RXJABBER")
	.dwattr $C$DW$746, DW_AT_TI_symbol_name("_RXJABBER")
	.dwattr $C$DW$746, DW_AT_data_member_location[DW_OP_plus_uconst 0x21c]
	.dwattr $C$DW$746, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$747	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$747, DW_AT_name("RXUNDERSIZED")
	.dwattr $C$DW$747, DW_AT_TI_symbol_name("_RXUNDERSIZED")
	.dwattr $C$DW$747, DW_AT_data_member_location[DW_OP_plus_uconst 0x220]
	.dwattr $C$DW$747, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$748	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$748, DW_AT_name("RXFRAGMENTS")
	.dwattr $C$DW$748, DW_AT_TI_symbol_name("_RXFRAGMENTS")
	.dwattr $C$DW$748, DW_AT_data_member_location[DW_OP_plus_uconst 0x224]
	.dwattr $C$DW$748, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$749	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$749, DW_AT_name("RXFILTERED")
	.dwattr $C$DW$749, DW_AT_TI_symbol_name("_RXFILTERED")
	.dwattr $C$DW$749, DW_AT_data_member_location[DW_OP_plus_uconst 0x228]
	.dwattr $C$DW$749, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$750	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$750, DW_AT_name("RXQOSFILTERED")
	.dwattr $C$DW$750, DW_AT_TI_symbol_name("_RXQOSFILTERED")
	.dwattr $C$DW$750, DW_AT_data_member_location[DW_OP_plus_uconst 0x22c]
	.dwattr $C$DW$750, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$751	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$751, DW_AT_name("RXOCTETS")
	.dwattr $C$DW$751, DW_AT_TI_symbol_name("_RXOCTETS")
	.dwattr $C$DW$751, DW_AT_data_member_location[DW_OP_plus_uconst 0x230]
	.dwattr $C$DW$751, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$752	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$752, DW_AT_name("TXGOODFRAMES")
	.dwattr $C$DW$752, DW_AT_TI_symbol_name("_TXGOODFRAMES")
	.dwattr $C$DW$752, DW_AT_data_member_location[DW_OP_plus_uconst 0x234]
	.dwattr $C$DW$752, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$753	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$753, DW_AT_name("TXBCASTFRAMES")
	.dwattr $C$DW$753, DW_AT_TI_symbol_name("_TXBCASTFRAMES")
	.dwattr $C$DW$753, DW_AT_data_member_location[DW_OP_plus_uconst 0x238]
	.dwattr $C$DW$753, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$754	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$754, DW_AT_name("TXMCASTFRAMES")
	.dwattr $C$DW$754, DW_AT_TI_symbol_name("_TXMCASTFRAMES")
	.dwattr $C$DW$754, DW_AT_data_member_location[DW_OP_plus_uconst 0x23c]
	.dwattr $C$DW$754, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$755	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$755, DW_AT_name("TXPAUSEFRAMES")
	.dwattr $C$DW$755, DW_AT_TI_symbol_name("_TXPAUSEFRAMES")
	.dwattr $C$DW$755, DW_AT_data_member_location[DW_OP_plus_uconst 0x240]
	.dwattr $C$DW$755, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$756	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$756, DW_AT_name("TXDEFERRED")
	.dwattr $C$DW$756, DW_AT_TI_symbol_name("_TXDEFERRED")
	.dwattr $C$DW$756, DW_AT_data_member_location[DW_OP_plus_uconst 0x244]
	.dwattr $C$DW$756, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$757	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$757, DW_AT_name("TXCOLLISION")
	.dwattr $C$DW$757, DW_AT_TI_symbol_name("_TXCOLLISION")
	.dwattr $C$DW$757, DW_AT_data_member_location[DW_OP_plus_uconst 0x248]
	.dwattr $C$DW$757, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$758	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$758, DW_AT_name("TXSINGLECOLL")
	.dwattr $C$DW$758, DW_AT_TI_symbol_name("_TXSINGLECOLL")
	.dwattr $C$DW$758, DW_AT_data_member_location[DW_OP_plus_uconst 0x24c]
	.dwattr $C$DW$758, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$759	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$759, DW_AT_name("TXMULTICOLL")
	.dwattr $C$DW$759, DW_AT_TI_symbol_name("_TXMULTICOLL")
	.dwattr $C$DW$759, DW_AT_data_member_location[DW_OP_plus_uconst 0x250]
	.dwattr $C$DW$759, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$760	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$760, DW_AT_name("TXEXCESSIVECOLL")
	.dwattr $C$DW$760, DW_AT_TI_symbol_name("_TXEXCESSIVECOLL")
	.dwattr $C$DW$760, DW_AT_data_member_location[DW_OP_plus_uconst 0x254]
	.dwattr $C$DW$760, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$761	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$761, DW_AT_name("TXLATECOLL")
	.dwattr $C$DW$761, DW_AT_TI_symbol_name("_TXLATECOLL")
	.dwattr $C$DW$761, DW_AT_data_member_location[DW_OP_plus_uconst 0x258]
	.dwattr $C$DW$761, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$762	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$762, DW_AT_name("TXUNDERRUN")
	.dwattr $C$DW$762, DW_AT_TI_symbol_name("_TXUNDERRUN")
	.dwattr $C$DW$762, DW_AT_data_member_location[DW_OP_plus_uconst 0x25c]
	.dwattr $C$DW$762, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$763	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$763, DW_AT_name("TXCARRIERSENSE")
	.dwattr $C$DW$763, DW_AT_TI_symbol_name("_TXCARRIERSENSE")
	.dwattr $C$DW$763, DW_AT_data_member_location[DW_OP_plus_uconst 0x260]
	.dwattr $C$DW$763, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$764	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$764, DW_AT_name("TXOCTETS")
	.dwattr $C$DW$764, DW_AT_TI_symbol_name("_TXOCTETS")
	.dwattr $C$DW$764, DW_AT_data_member_location[DW_OP_plus_uconst 0x264]
	.dwattr $C$DW$764, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$765	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$765, DW_AT_name("FRAME64")
	.dwattr $C$DW$765, DW_AT_TI_symbol_name("_FRAME64")
	.dwattr $C$DW$765, DW_AT_data_member_location[DW_OP_plus_uconst 0x268]
	.dwattr $C$DW$765, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$766	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$766, DW_AT_name("FRAME65T127")
	.dwattr $C$DW$766, DW_AT_TI_symbol_name("_FRAME65T127")
	.dwattr $C$DW$766, DW_AT_data_member_location[DW_OP_plus_uconst 0x26c]
	.dwattr $C$DW$766, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$767	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$767, DW_AT_name("FRAME128T255")
	.dwattr $C$DW$767, DW_AT_TI_symbol_name("_FRAME128T255")
	.dwattr $C$DW$767, DW_AT_data_member_location[DW_OP_plus_uconst 0x270]
	.dwattr $C$DW$767, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$768	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$768, DW_AT_name("FRAME256T511")
	.dwattr $C$DW$768, DW_AT_TI_symbol_name("_FRAME256T511")
	.dwattr $C$DW$768, DW_AT_data_member_location[DW_OP_plus_uconst 0x274]
	.dwattr $C$DW$768, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$769	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$769, DW_AT_name("FRAME512T1023")
	.dwattr $C$DW$769, DW_AT_TI_symbol_name("_FRAME512T1023")
	.dwattr $C$DW$769, DW_AT_data_member_location[DW_OP_plus_uconst 0x278]
	.dwattr $C$DW$769, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$770	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$770, DW_AT_name("FRAME1024TUP")
	.dwattr $C$DW$770, DW_AT_TI_symbol_name("_FRAME1024TUP")
	.dwattr $C$DW$770, DW_AT_data_member_location[DW_OP_plus_uconst 0x27c]
	.dwattr $C$DW$770, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$771	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$771, DW_AT_name("NETOCTETS")
	.dwattr $C$DW$771, DW_AT_TI_symbol_name("_NETOCTETS")
	.dwattr $C$DW$771, DW_AT_data_member_location[DW_OP_plus_uconst 0x280]
	.dwattr $C$DW$771, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$772	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$772, DW_AT_name("RXSOFOVERRUNS")
	.dwattr $C$DW$772, DW_AT_TI_symbol_name("_RXSOFOVERRUNS")
	.dwattr $C$DW$772, DW_AT_data_member_location[DW_OP_plus_uconst 0x284]
	.dwattr $C$DW$772, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$773	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$773, DW_AT_name("RXMOFOVERRUNS")
	.dwattr $C$DW$773, DW_AT_TI_symbol_name("_RXMOFOVERRUNS")
	.dwattr $C$DW$773, DW_AT_data_member_location[DW_OP_plus_uconst 0x288]
	.dwattr $C$DW$773, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$774	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$774, DW_AT_name("RXDMAOVERRUNS")
	.dwattr $C$DW$774, DW_AT_TI_symbol_name("_RXDMAOVERRUNS")
	.dwattr $C$DW$774, DW_AT_data_member_location[DW_OP_plus_uconst 0x28c]
	.dwattr $C$DW$774, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$775	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$107)
	.dwattr $C$DW$775, DW_AT_name("RSVD7")
	.dwattr $C$DW$775, DW_AT_TI_symbol_name("_RSVD7")
	.dwattr $C$DW$775, DW_AT_data_member_location[DW_OP_plus_uconst 0x290]
	.dwattr $C$DW$775, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$776	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$776, DW_AT_name("MACADDRLO")
	.dwattr $C$DW$776, DW_AT_TI_symbol_name("_MACADDRLO")
	.dwattr $C$DW$776, DW_AT_data_member_location[DW_OP_plus_uconst 0x500]
	.dwattr $C$DW$776, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$777	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$777, DW_AT_name("MACADDRHI")
	.dwattr $C$DW$777, DW_AT_TI_symbol_name("_MACADDRHI")
	.dwattr $C$DW$777, DW_AT_data_member_location[DW_OP_plus_uconst 0x504]
	.dwattr $C$DW$777, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$778	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$778, DW_AT_name("MACINDEX")
	.dwattr $C$DW$778, DW_AT_TI_symbol_name("_MACINDEX")
	.dwattr $C$DW$778, DW_AT_data_member_location[DW_OP_plus_uconst 0x508]
	.dwattr $C$DW$778, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$779	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$87)
	.dwattr $C$DW$779, DW_AT_name("RSVD8")
	.dwattr $C$DW$779, DW_AT_TI_symbol_name("_RSVD8")
	.dwattr $C$DW$779, DW_AT_data_member_location[DW_OP_plus_uconst 0x50c]
	.dwattr $C$DW$779, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$780	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$780, DW_AT_name("TX0HDP")
	.dwattr $C$DW$780, DW_AT_TI_symbol_name("_TX0HDP")
	.dwattr $C$DW$780, DW_AT_data_member_location[DW_OP_plus_uconst 0x600]
	.dwattr $C$DW$780, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$781	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$781, DW_AT_name("TX1HDP")
	.dwattr $C$DW$781, DW_AT_TI_symbol_name("_TX1HDP")
	.dwattr $C$DW$781, DW_AT_data_member_location[DW_OP_plus_uconst 0x604]
	.dwattr $C$DW$781, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$782	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$782, DW_AT_name("TX2HDP")
	.dwattr $C$DW$782, DW_AT_TI_symbol_name("_TX2HDP")
	.dwattr $C$DW$782, DW_AT_data_member_location[DW_OP_plus_uconst 0x608]
	.dwattr $C$DW$782, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$783	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$783, DW_AT_name("TX3HDP")
	.dwattr $C$DW$783, DW_AT_TI_symbol_name("_TX3HDP")
	.dwattr $C$DW$783, DW_AT_data_member_location[DW_OP_plus_uconst 0x60c]
	.dwattr $C$DW$783, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$784	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$784, DW_AT_name("TX4HDP")
	.dwattr $C$DW$784, DW_AT_TI_symbol_name("_TX4HDP")
	.dwattr $C$DW$784, DW_AT_data_member_location[DW_OP_plus_uconst 0x610]
	.dwattr $C$DW$784, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$785	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$785, DW_AT_name("TX5HDP")
	.dwattr $C$DW$785, DW_AT_TI_symbol_name("_TX5HDP")
	.dwattr $C$DW$785, DW_AT_data_member_location[DW_OP_plus_uconst 0x614]
	.dwattr $C$DW$785, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$786	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$786, DW_AT_name("TX6HDP")
	.dwattr $C$DW$786, DW_AT_TI_symbol_name("_TX6HDP")
	.dwattr $C$DW$786, DW_AT_data_member_location[DW_OP_plus_uconst 0x618]
	.dwattr $C$DW$786, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$787	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$787, DW_AT_name("TX7HDP")
	.dwattr $C$DW$787, DW_AT_TI_symbol_name("_TX7HDP")
	.dwattr $C$DW$787, DW_AT_data_member_location[DW_OP_plus_uconst 0x61c]
	.dwattr $C$DW$787, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$788	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$788, DW_AT_name("RX0HDP")
	.dwattr $C$DW$788, DW_AT_TI_symbol_name("_RX0HDP")
	.dwattr $C$DW$788, DW_AT_data_member_location[DW_OP_plus_uconst 0x620]
	.dwattr $C$DW$788, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$789	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$789, DW_AT_name("RX1HDP")
	.dwattr $C$DW$789, DW_AT_TI_symbol_name("_RX1HDP")
	.dwattr $C$DW$789, DW_AT_data_member_location[DW_OP_plus_uconst 0x624]
	.dwattr $C$DW$789, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$790	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$790, DW_AT_name("RX2HDP")
	.dwattr $C$DW$790, DW_AT_TI_symbol_name("_RX2HDP")
	.dwattr $C$DW$790, DW_AT_data_member_location[DW_OP_plus_uconst 0x628]
	.dwattr $C$DW$790, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$791	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$791, DW_AT_name("RX3HDP")
	.dwattr $C$DW$791, DW_AT_TI_symbol_name("_RX3HDP")
	.dwattr $C$DW$791, DW_AT_data_member_location[DW_OP_plus_uconst 0x62c]
	.dwattr $C$DW$791, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$792	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$792, DW_AT_name("RX4HDP")
	.dwattr $C$DW$792, DW_AT_TI_symbol_name("_RX4HDP")
	.dwattr $C$DW$792, DW_AT_data_member_location[DW_OP_plus_uconst 0x630]
	.dwattr $C$DW$792, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$793	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$793, DW_AT_name("RX5HDP")
	.dwattr $C$DW$793, DW_AT_TI_symbol_name("_RX5HDP")
	.dwattr $C$DW$793, DW_AT_data_member_location[DW_OP_plus_uconst 0x634]
	.dwattr $C$DW$793, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$794	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$794, DW_AT_name("RX6HDP")
	.dwattr $C$DW$794, DW_AT_TI_symbol_name("_RX6HDP")
	.dwattr $C$DW$794, DW_AT_data_member_location[DW_OP_plus_uconst 0x638]
	.dwattr $C$DW$794, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$795	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$795, DW_AT_name("RX7HDP")
	.dwattr $C$DW$795, DW_AT_TI_symbol_name("_RX7HDP")
	.dwattr $C$DW$795, DW_AT_data_member_location[DW_OP_plus_uconst 0x63c]
	.dwattr $C$DW$795, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$796	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$796, DW_AT_name("TX0CP")
	.dwattr $C$DW$796, DW_AT_TI_symbol_name("_TX0CP")
	.dwattr $C$DW$796, DW_AT_data_member_location[DW_OP_plus_uconst 0x640]
	.dwattr $C$DW$796, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$797	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$797, DW_AT_name("TX1CP")
	.dwattr $C$DW$797, DW_AT_TI_symbol_name("_TX1CP")
	.dwattr $C$DW$797, DW_AT_data_member_location[DW_OP_plus_uconst 0x644]
	.dwattr $C$DW$797, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$798	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$798, DW_AT_name("TX2CP")
	.dwattr $C$DW$798, DW_AT_TI_symbol_name("_TX2CP")
	.dwattr $C$DW$798, DW_AT_data_member_location[DW_OP_plus_uconst 0x648]
	.dwattr $C$DW$798, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$799	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$799, DW_AT_name("TX3CP")
	.dwattr $C$DW$799, DW_AT_TI_symbol_name("_TX3CP")
	.dwattr $C$DW$799, DW_AT_data_member_location[DW_OP_plus_uconst 0x64c]
	.dwattr $C$DW$799, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$800	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$800, DW_AT_name("TX4CP")
	.dwattr $C$DW$800, DW_AT_TI_symbol_name("_TX4CP")
	.dwattr $C$DW$800, DW_AT_data_member_location[DW_OP_plus_uconst 0x650]
	.dwattr $C$DW$800, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$801	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$801, DW_AT_name("TX5CP")
	.dwattr $C$DW$801, DW_AT_TI_symbol_name("_TX5CP")
	.dwattr $C$DW$801, DW_AT_data_member_location[DW_OP_plus_uconst 0x654]
	.dwattr $C$DW$801, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$802	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$802, DW_AT_name("TX6CP")
	.dwattr $C$DW$802, DW_AT_TI_symbol_name("_TX6CP")
	.dwattr $C$DW$802, DW_AT_data_member_location[DW_OP_plus_uconst 0x658]
	.dwattr $C$DW$802, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$803	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$803, DW_AT_name("TX7CP")
	.dwattr $C$DW$803, DW_AT_TI_symbol_name("_TX7CP")
	.dwattr $C$DW$803, DW_AT_data_member_location[DW_OP_plus_uconst 0x65c]
	.dwattr $C$DW$803, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$804	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$804, DW_AT_name("RX0CP")
	.dwattr $C$DW$804, DW_AT_TI_symbol_name("_RX0CP")
	.dwattr $C$DW$804, DW_AT_data_member_location[DW_OP_plus_uconst 0x660]
	.dwattr $C$DW$804, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$805	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$805, DW_AT_name("RX1CP")
	.dwattr $C$DW$805, DW_AT_TI_symbol_name("_RX1CP")
	.dwattr $C$DW$805, DW_AT_data_member_location[DW_OP_plus_uconst 0x664]
	.dwattr $C$DW$805, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$806	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$806, DW_AT_name("RX2CP")
	.dwattr $C$DW$806, DW_AT_TI_symbol_name("_RX2CP")
	.dwattr $C$DW$806, DW_AT_data_member_location[DW_OP_plus_uconst 0x668]
	.dwattr $C$DW$806, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$807	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$807, DW_AT_name("RX3CP")
	.dwattr $C$DW$807, DW_AT_TI_symbol_name("_RX3CP")
	.dwattr $C$DW$807, DW_AT_data_member_location[DW_OP_plus_uconst 0x66c]
	.dwattr $C$DW$807, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$808	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$808, DW_AT_name("RX4CP")
	.dwattr $C$DW$808, DW_AT_TI_symbol_name("_RX4CP")
	.dwattr $C$DW$808, DW_AT_data_member_location[DW_OP_plus_uconst 0x670]
	.dwattr $C$DW$808, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$809	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$809, DW_AT_name("RX5CP")
	.dwattr $C$DW$809, DW_AT_TI_symbol_name("_RX5CP")
	.dwattr $C$DW$809, DW_AT_data_member_location[DW_OP_plus_uconst 0x674]
	.dwattr $C$DW$809, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$810	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$810, DW_AT_name("RX6CP")
	.dwattr $C$DW$810, DW_AT_TI_symbol_name("_RX6CP")
	.dwattr $C$DW$810, DW_AT_data_member_location[DW_OP_plus_uconst 0x678]
	.dwattr $C$DW$810, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$811	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$811, DW_AT_name("RX7CP")
	.dwattr $C$DW$811, DW_AT_TI_symbol_name("_RX7CP")
	.dwattr $C$DW$811, DW_AT_data_member_location[DW_OP_plus_uconst 0x67c]
	.dwattr $C$DW$811, DW_AT_accessibility(DW_ACCESS_public)
	.dwendtag $C$DW$T$108

$C$DW$T$251	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_EmacRegs")
	.dwattr $C$DW$T$251, DW_AT_type(*$C$DW$T$108)
	.dwattr $C$DW$T$251, DW_AT_language(DW_LANG_C)
$C$DW$T$252	.dwtag  DW_TAG_volatile_type
	.dwattr $C$DW$T$252, DW_AT_type(*$C$DW$T$251)
$C$DW$T$253	.dwtag  DW_TAG_pointer_type, DW_AT_type(*$C$DW$T$252)
	.dwattr $C$DW$T$253, DW_AT_address_class(0x20)
$C$DW$T$254	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_EmacRegsOvly")
	.dwattr $C$DW$T$254, DW_AT_type(*$C$DW$T$253)
	.dwattr $C$DW$T$254, DW_AT_language(DW_LANG_C)

$C$DW$T$109	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$109, DW_AT_byte_size(0x114)
$C$DW$812	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$812, DW_AT_name("PID")
	.dwattr $C$DW$812, DW_AT_TI_symbol_name("_PID")
	.dwattr $C$DW$812, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$812, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$813	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$813, DW_AT_name("PCR")
	.dwattr $C$DW$813, DW_AT_TI_symbol_name("_PCR")
	.dwattr $C$DW$813, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$813, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$814	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$814, DW_AT_name("GLB_REG_0")
	.dwattr $C$DW$814, DW_AT_TI_symbol_name("_GLB_REG_0")
	.dwattr $C$DW$814, DW_AT_data_member_location[DW_OP_plus_uconst 0x8]
	.dwattr $C$DW$814, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$815	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$815, DW_AT_name("GLB_REG_1")
	.dwattr $C$DW$815, DW_AT_TI_symbol_name("_GLB_REG_1")
	.dwattr $C$DW$815, DW_AT_data_member_location[DW_OP_plus_uconst 0xc]
	.dwattr $C$DW$815, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$816	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$816, DW_AT_name("GLB_REG_2")
	.dwattr $C$DW$816, DW_AT_TI_symbol_name("_GLB_REG_2")
	.dwattr $C$DW$816, DW_AT_data_member_location[DW_OP_plus_uconst 0x10]
	.dwattr $C$DW$816, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$817	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$817, DW_AT_name("GLB_REG_3")
	.dwattr $C$DW$817, DW_AT_TI_symbol_name("_GLB_REG_3")
	.dwattr $C$DW$817, DW_AT_data_member_location[DW_OP_plus_uconst 0x14]
	.dwattr $C$DW$817, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$818	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$818, DW_AT_name("GLB_REG_4")
	.dwattr $C$DW$818, DW_AT_TI_symbol_name("_GLB_REG_4")
	.dwattr $C$DW$818, DW_AT_data_member_location[DW_OP_plus_uconst 0x18]
	.dwattr $C$DW$818, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$819	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$819, DW_AT_name("GLB_REG_5")
	.dwattr $C$DW$819, DW_AT_TI_symbol_name("_GLB_REG_5")
	.dwattr $C$DW$819, DW_AT_data_member_location[DW_OP_plus_uconst 0x1c]
	.dwattr $C$DW$819, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$820	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$820, DW_AT_name("GLB_REG_6")
	.dwattr $C$DW$820, DW_AT_TI_symbol_name("_GLB_REG_6")
	.dwattr $C$DW$820, DW_AT_data_member_location[DW_OP_plus_uconst 0x20]
	.dwattr $C$DW$820, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$821	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$821, DW_AT_name("GLB_REG_7")
	.dwattr $C$DW$821, DW_AT_TI_symbol_name("_GLB_REG_7")
	.dwattr $C$DW$821, DW_AT_data_member_location[DW_OP_plus_uconst 0x24]
	.dwattr $C$DW$821, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$822	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$822, DW_AT_name("CCDC_WR_0")
	.dwattr $C$DW$822, DW_AT_TI_symbol_name("_CCDC_WR_0")
	.dwattr $C$DW$822, DW_AT_data_member_location[DW_OP_plus_uconst 0x28]
	.dwattr $C$DW$822, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$823	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$823, DW_AT_name("CCDC_WR_1")
	.dwattr $C$DW$823, DW_AT_TI_symbol_name("_CCDC_WR_1")
	.dwattr $C$DW$823, DW_AT_data_member_location[DW_OP_plus_uconst 0x2c]
	.dwattr $C$DW$823, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$824	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$824, DW_AT_name("CCDC_WR_2")
	.dwattr $C$DW$824, DW_AT_TI_symbol_name("_CCDC_WR_2")
	.dwattr $C$DW$824, DW_AT_data_member_location[DW_OP_plus_uconst 0x30]
	.dwattr $C$DW$824, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$825	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$825, DW_AT_name("CCDC_WR_3")
	.dwattr $C$DW$825, DW_AT_TI_symbol_name("_CCDC_WR_3")
	.dwattr $C$DW$825, DW_AT_data_member_location[DW_OP_plus_uconst 0x34]
	.dwattr $C$DW$825, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$826	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$826, DW_AT_name("CCDC_FP_RD_0")
	.dwattr $C$DW$826, DW_AT_TI_symbol_name("_CCDC_FP_RD_0")
	.dwattr $C$DW$826, DW_AT_data_member_location[DW_OP_plus_uconst 0x38]
	.dwattr $C$DW$826, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$827	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$827, DW_AT_name("CCDC_FP_RD_1")
	.dwattr $C$DW$827, DW_AT_TI_symbol_name("_CCDC_FP_RD_1")
	.dwattr $C$DW$827, DW_AT_data_member_location[DW_OP_plus_uconst 0x3c]
	.dwattr $C$DW$827, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$828	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$828, DW_AT_name("PRV_RD_0")
	.dwattr $C$DW$828, DW_AT_TI_symbol_name("_PRV_RD_0")
	.dwattr $C$DW$828, DW_AT_data_member_location[DW_OP_plus_uconst 0x40]
	.dwattr $C$DW$828, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$829	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$829, DW_AT_name("PRV_RD_1")
	.dwattr $C$DW$829, DW_AT_TI_symbol_name("_PRV_RD_1")
	.dwattr $C$DW$829, DW_AT_data_member_location[DW_OP_plus_uconst 0x44]
	.dwattr $C$DW$829, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$830	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$830, DW_AT_name("PRV_RD_2")
	.dwattr $C$DW$830, DW_AT_TI_symbol_name("_PRV_RD_2")
	.dwattr $C$DW$830, DW_AT_data_member_location[DW_OP_plus_uconst 0x48]
	.dwattr $C$DW$830, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$831	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$831, DW_AT_name("PRV_RD_3")
	.dwattr $C$DW$831, DW_AT_TI_symbol_name("_PRV_RD_3")
	.dwattr $C$DW$831, DW_AT_data_member_location[DW_OP_plus_uconst 0x4c]
	.dwattr $C$DW$831, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$832	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$832, DW_AT_name("PRV_WR_0")
	.dwattr $C$DW$832, DW_AT_TI_symbol_name("_PRV_WR_0")
	.dwattr $C$DW$832, DW_AT_data_member_location[DW_OP_plus_uconst 0x50]
	.dwattr $C$DW$832, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$833	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$833, DW_AT_name("PRV_WR_1")
	.dwattr $C$DW$833, DW_AT_TI_symbol_name("_PRV_WR_1")
	.dwattr $C$DW$833, DW_AT_data_member_location[DW_OP_plus_uconst 0x54]
	.dwattr $C$DW$833, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$834	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$834, DW_AT_name("PRV_WR_2")
	.dwattr $C$DW$834, DW_AT_TI_symbol_name("_PRV_WR_2")
	.dwattr $C$DW$834, DW_AT_data_member_location[DW_OP_plus_uconst 0x58]
	.dwattr $C$DW$834, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$835	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$835, DW_AT_name("PRV_WR_3")
	.dwattr $C$DW$835, DW_AT_TI_symbol_name("_PRV_WR_3")
	.dwattr $C$DW$835, DW_AT_data_member_location[DW_OP_plus_uconst 0x5c]
	.dwattr $C$DW$835, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$836	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$836, DW_AT_name("PRV_DK_RD_0")
	.dwattr $C$DW$836, DW_AT_TI_symbol_name("_PRV_DK_RD_0")
	.dwattr $C$DW$836, DW_AT_data_member_location[DW_OP_plus_uconst 0x60]
	.dwattr $C$DW$836, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$837	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$837, DW_AT_name("PRV_DK_RD_1")
	.dwattr $C$DW$837, DW_AT_TI_symbol_name("_PRV_DK_RD_1")
	.dwattr $C$DW$837, DW_AT_data_member_location[DW_OP_plus_uconst 0x64]
	.dwattr $C$DW$837, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$838	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$838, DW_AT_name("PRV_DK_RD_2")
	.dwattr $C$DW$838, DW_AT_TI_symbol_name("_PRV_DK_RD_2")
	.dwattr $C$DW$838, DW_AT_data_member_location[DW_OP_plus_uconst 0x68]
	.dwattr $C$DW$838, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$839	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$839, DW_AT_name("PRV_DK_RD_3")
	.dwattr $C$DW$839, DW_AT_TI_symbol_name("_PRV_DK_RD_3")
	.dwattr $C$DW$839, DW_AT_data_member_location[DW_OP_plus_uconst 0x6c]
	.dwattr $C$DW$839, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$840	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$840, DW_AT_name("RESZ_RD_0")
	.dwattr $C$DW$840, DW_AT_TI_symbol_name("_RESZ_RD_0")
	.dwattr $C$DW$840, DW_AT_data_member_location[DW_OP_plus_uconst 0x70]
	.dwattr $C$DW$840, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$841	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$841, DW_AT_name("RESZ_RD_1")
	.dwattr $C$DW$841, DW_AT_TI_symbol_name("_RESZ_RD_1")
	.dwattr $C$DW$841, DW_AT_data_member_location[DW_OP_plus_uconst 0x74]
	.dwattr $C$DW$841, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$842	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$842, DW_AT_name("RESZ_RD_2")
	.dwattr $C$DW$842, DW_AT_TI_symbol_name("_RESZ_RD_2")
	.dwattr $C$DW$842, DW_AT_data_member_location[DW_OP_plus_uconst 0x78]
	.dwattr $C$DW$842, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$843	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$843, DW_AT_name("RESZ_RD_3")
	.dwattr $C$DW$843, DW_AT_TI_symbol_name("_RESZ_RD_3")
	.dwattr $C$DW$843, DW_AT_data_member_location[DW_OP_plus_uconst 0x7c]
	.dwattr $C$DW$843, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$844	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$844, DW_AT_name("RESZ1_WR_0")
	.dwattr $C$DW$844, DW_AT_TI_symbol_name("_RESZ1_WR_0")
	.dwattr $C$DW$844, DW_AT_data_member_location[DW_OP_plus_uconst 0x80]
	.dwattr $C$DW$844, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$845	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$845, DW_AT_name("RESZ1_WR_1")
	.dwattr $C$DW$845, DW_AT_TI_symbol_name("_RESZ1_WR_1")
	.dwattr $C$DW$845, DW_AT_data_member_location[DW_OP_plus_uconst 0x84]
	.dwattr $C$DW$845, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$846	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$846, DW_AT_name("RESZ1_WR_2")
	.dwattr $C$DW$846, DW_AT_TI_symbol_name("_RESZ1_WR_2")
	.dwattr $C$DW$846, DW_AT_data_member_location[DW_OP_plus_uconst 0x88]
	.dwattr $C$DW$846, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$847	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$847, DW_AT_name("RESZ1_WR_3")
	.dwattr $C$DW$847, DW_AT_TI_symbol_name("_RESZ1_WR_3")
	.dwattr $C$DW$847, DW_AT_data_member_location[DW_OP_plus_uconst 0x8c]
	.dwattr $C$DW$847, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$848	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$848, DW_AT_name("RESZ2_WR_0")
	.dwattr $C$DW$848, DW_AT_TI_symbol_name("_RESZ2_WR_0")
	.dwattr $C$DW$848, DW_AT_data_member_location[DW_OP_plus_uconst 0x90]
	.dwattr $C$DW$848, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$849	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$849, DW_AT_name("RESZ2_WR_1")
	.dwattr $C$DW$849, DW_AT_TI_symbol_name("_RESZ2_WR_1")
	.dwattr $C$DW$849, DW_AT_data_member_location[DW_OP_plus_uconst 0x94]
	.dwattr $C$DW$849, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$850	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$850, DW_AT_name("RESZ2_WR_2")
	.dwattr $C$DW$850, DW_AT_TI_symbol_name("_RESZ2_WR_2")
	.dwattr $C$DW$850, DW_AT_data_member_location[DW_OP_plus_uconst 0x98]
	.dwattr $C$DW$850, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$851	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$851, DW_AT_name("RESZ2_WR_3")
	.dwattr $C$DW$851, DW_AT_TI_symbol_name("_RESZ2_WR_3")
	.dwattr $C$DW$851, DW_AT_data_member_location[DW_OP_plus_uconst 0x9c]
	.dwattr $C$DW$851, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$852	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$852, DW_AT_name("RESZ3_WR_0")
	.dwattr $C$DW$852, DW_AT_TI_symbol_name("_RESZ3_WR_0")
	.dwattr $C$DW$852, DW_AT_data_member_location[DW_OP_plus_uconst 0xa0]
	.dwattr $C$DW$852, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$853	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$853, DW_AT_name("RESZ3_WR_1")
	.dwattr $C$DW$853, DW_AT_TI_symbol_name("_RESZ3_WR_1")
	.dwattr $C$DW$853, DW_AT_data_member_location[DW_OP_plus_uconst 0xa4]
	.dwattr $C$DW$853, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$854	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$854, DW_AT_name("RESZ3_WR_2")
	.dwattr $C$DW$854, DW_AT_TI_symbol_name("_RESZ3_WR_2")
	.dwattr $C$DW$854, DW_AT_data_member_location[DW_OP_plus_uconst 0xa8]
	.dwattr $C$DW$854, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$855	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$855, DW_AT_name("RESZ3_WR_3")
	.dwattr $C$DW$855, DW_AT_TI_symbol_name("_RESZ3_WR_3")
	.dwattr $C$DW$855, DW_AT_data_member_location[DW_OP_plus_uconst 0xac]
	.dwattr $C$DW$855, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$856	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$856, DW_AT_name("RESZ4_WR_0")
	.dwattr $C$DW$856, DW_AT_TI_symbol_name("_RESZ4_WR_0")
	.dwattr $C$DW$856, DW_AT_data_member_location[DW_OP_plus_uconst 0xb0]
	.dwattr $C$DW$856, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$857	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$857, DW_AT_name("RESZ4_WR_1")
	.dwattr $C$DW$857, DW_AT_TI_symbol_name("_RESZ4_WR_1")
	.dwattr $C$DW$857, DW_AT_data_member_location[DW_OP_plus_uconst 0xb4]
	.dwattr $C$DW$857, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$858	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$858, DW_AT_name("RESZ4_WR_2")
	.dwattr $C$DW$858, DW_AT_TI_symbol_name("_RESZ4_WR_2")
	.dwattr $C$DW$858, DW_AT_data_member_location[DW_OP_plus_uconst 0xb8]
	.dwattr $C$DW$858, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$859	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$859, DW_AT_name("RESZ4_WR_3")
	.dwattr $C$DW$859, DW_AT_TI_symbol_name("_RESZ4_WR_3")
	.dwattr $C$DW$859, DW_AT_data_member_location[DW_OP_plus_uconst 0xbc]
	.dwattr $C$DW$859, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$860	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$860, DW_AT_name("HIST_RD_0")
	.dwattr $C$DW$860, DW_AT_TI_symbol_name("_HIST_RD_0")
	.dwattr $C$DW$860, DW_AT_data_member_location[DW_OP_plus_uconst 0xc0]
	.dwattr $C$DW$860, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$861	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$861, DW_AT_name("HIST_RD_1")
	.dwattr $C$DW$861, DW_AT_TI_symbol_name("_HIST_RD_1")
	.dwattr $C$DW$861, DW_AT_data_member_location[DW_OP_plus_uconst 0xc4]
	.dwattr $C$DW$861, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$862	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$862, DW_AT_name("AF_WR_0")
	.dwattr $C$DW$862, DW_AT_TI_symbol_name("_AF_WR_0")
	.dwattr $C$DW$862, DW_AT_data_member_location[DW_OP_plus_uconst 0xc8]
	.dwattr $C$DW$862, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$863	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$863, DW_AT_name("AF_WR_1")
	.dwattr $C$DW$863, DW_AT_TI_symbol_name("_AF_WR_1")
	.dwattr $C$DW$863, DW_AT_data_member_location[DW_OP_plus_uconst 0xcc]
	.dwattr $C$DW$863, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$864	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$864, DW_AT_name("AEAWB_WR_0")
	.dwattr $C$DW$864, DW_AT_TI_symbol_name("_AEAWB_WR_0")
	.dwattr $C$DW$864, DW_AT_data_member_location[DW_OP_plus_uconst 0xd0]
	.dwattr $C$DW$864, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$865	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$865, DW_AT_name("AEAWB_WR_1")
	.dwattr $C$DW$865, DW_AT_TI_symbol_name("_AEAWB_WR_1")
	.dwattr $C$DW$865, DW_AT_data_member_location[DW_OP_plus_uconst 0xd4]
	.dwattr $C$DW$865, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$866	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$866, DW_AT_name("OSD_V0_RD_0")
	.dwattr $C$DW$866, DW_AT_TI_symbol_name("_OSD_V0_RD_0")
	.dwattr $C$DW$866, DW_AT_data_member_location[DW_OP_plus_uconst 0xd8]
	.dwattr $C$DW$866, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$867	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$867, DW_AT_name("OSD_V0_RD_1")
	.dwattr $C$DW$867, DW_AT_TI_symbol_name("_OSD_V0_RD_1")
	.dwattr $C$DW$867, DW_AT_data_member_location[DW_OP_plus_uconst 0xdc]
	.dwattr $C$DW$867, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$868	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$868, DW_AT_name("OSD_V0_RD_2")
	.dwattr $C$DW$868, DW_AT_TI_symbol_name("_OSD_V0_RD_2")
	.dwattr $C$DW$868, DW_AT_data_member_location[DW_OP_plus_uconst 0xe0]
	.dwattr $C$DW$868, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$869	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$869, DW_AT_name("OSD_V0_RD_3")
	.dwattr $C$DW$869, DW_AT_TI_symbol_name("_OSD_V0_RD_3")
	.dwattr $C$DW$869, DW_AT_data_member_location[DW_OP_plus_uconst 0xe4]
	.dwattr $C$DW$869, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$870	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$870, DW_AT_name("OSD_V1_RD_0")
	.dwattr $C$DW$870, DW_AT_TI_symbol_name("_OSD_V1_RD_0")
	.dwattr $C$DW$870, DW_AT_data_member_location[DW_OP_plus_uconst 0xe8]
	.dwattr $C$DW$870, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$871	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$871, DW_AT_name("OSD_V1_RD_1")
	.dwattr $C$DW$871, DW_AT_TI_symbol_name("_OSD_V1_RD_1")
	.dwattr $C$DW$871, DW_AT_data_member_location[DW_OP_plus_uconst 0xec]
	.dwattr $C$DW$871, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$872	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$872, DW_AT_name("OSD_V1_RD_2")
	.dwattr $C$DW$872, DW_AT_TI_symbol_name("_OSD_V1_RD_2")
	.dwattr $C$DW$872, DW_AT_data_member_location[DW_OP_plus_uconst 0xf0]
	.dwattr $C$DW$872, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$873	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$873, DW_AT_name("OSD_V1_RD_3")
	.dwattr $C$DW$873, DW_AT_TI_symbol_name("_OSD_V1_RD_3")
	.dwattr $C$DW$873, DW_AT_data_member_location[DW_OP_plus_uconst 0xf4]
	.dwattr $C$DW$873, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$874	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$874, DW_AT_name("OSD_O0_RD_0")
	.dwattr $C$DW$874, DW_AT_TI_symbol_name("_OSD_O0_RD_0")
	.dwattr $C$DW$874, DW_AT_data_member_location[DW_OP_plus_uconst 0xf8]
	.dwattr $C$DW$874, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$875	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$875, DW_AT_name("OSD_O0_RD_1")
	.dwattr $C$DW$875, DW_AT_TI_symbol_name("_OSD_O0_RD_1")
	.dwattr $C$DW$875, DW_AT_data_member_location[DW_OP_plus_uconst 0xfc]
	.dwattr $C$DW$875, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$876	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$876, DW_AT_name("OSD_O1_RD_2")
	.dwattr $C$DW$876, DW_AT_TI_symbol_name("_OSD_O1_RD_2")
	.dwattr $C$DW$876, DW_AT_data_member_location[DW_OP_plus_uconst 0x100]
	.dwattr $C$DW$876, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$877	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$877, DW_AT_name("OSD_O1_RD_3")
	.dwattr $C$DW$877, DW_AT_TI_symbol_name("_OSD_O1_RD_3")
	.dwattr $C$DW$877, DW_AT_data_member_location[DW_OP_plus_uconst 0x104]
	.dwattr $C$DW$877, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$878	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$878, DW_AT_name("SDR_REQ_EXP")
	.dwattr $C$DW$878, DW_AT_TI_symbol_name("_SDR_REQ_EXP")
	.dwattr $C$DW$878, DW_AT_data_member_location[DW_OP_plus_uconst 0x108]
	.dwattr $C$DW$878, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$879	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$879, DW_AT_name("TEST_CNT")
	.dwattr $C$DW$879, DW_AT_TI_symbol_name("_TEST_CNT")
	.dwattr $C$DW$879, DW_AT_data_member_location[DW_OP_plus_uconst 0x10c]
	.dwattr $C$DW$879, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$880	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$880, DW_AT_name("MISR")
	.dwattr $C$DW$880, DW_AT_TI_symbol_name("_MISR")
	.dwattr $C$DW$880, DW_AT_data_member_location[DW_OP_plus_uconst 0x110]
	.dwattr $C$DW$880, DW_AT_accessibility(DW_ACCESS_public)
	.dwendtag $C$DW$T$109

$C$DW$T$255	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_VpssRegs")
	.dwattr $C$DW$T$255, DW_AT_type(*$C$DW$T$109)
	.dwattr $C$DW$T$255, DW_AT_language(DW_LANG_C)
$C$DW$T$256	.dwtag  DW_TAG_volatile_type
	.dwattr $C$DW$T$256, DW_AT_type(*$C$DW$T$255)
$C$DW$T$257	.dwtag  DW_TAG_pointer_type, DW_AT_type(*$C$DW$T$256)
	.dwattr $C$DW$T$257, DW_AT_address_class(0x20)
$C$DW$T$258	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_VpssRegsOvly")
	.dwattr $C$DW$T$258, DW_AT_type(*$C$DW$T$257)
	.dwattr $C$DW$T$258, DW_AT_language(DW_LANG_C)

$C$DW$T$110	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$110, DW_AT_byte_size(0x0c)
$C$DW$881	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$55)
	.dwattr $C$DW$881, DW_AT_name("RSVD0")
	.dwattr $C$DW$881, DW_AT_TI_symbol_name("_RSVD0")
	.dwattr $C$DW$881, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$881, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$882	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$882, DW_AT_name("EWCTL")
	.dwattr $C$DW$882, DW_AT_TI_symbol_name("_EWCTL")
	.dwattr $C$DW$882, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$882, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$883	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$883, DW_AT_name("EWINTTCNT")
	.dwattr $C$DW$883, DW_AT_TI_symbol_name("_EWINTTCNT")
	.dwattr $C$DW$883, DW_AT_data_member_location[DW_OP_plus_uconst 0x8]
	.dwattr $C$DW$883, DW_AT_accessibility(DW_ACCESS_public)
	.dwendtag $C$DW$T$110

$C$DW$T$259	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_EwrapRegs")
	.dwattr $C$DW$T$259, DW_AT_type(*$C$DW$T$110)
	.dwattr $C$DW$T$259, DW_AT_language(DW_LANG_C)
$C$DW$T$260	.dwtag  DW_TAG_volatile_type
	.dwattr $C$DW$T$260, DW_AT_type(*$C$DW$T$259)
$C$DW$T$261	.dwtag  DW_TAG_pointer_type, DW_AT_type(*$C$DW$T$260)
	.dwattr $C$DW$T$261, DW_AT_address_class(0x20)
$C$DW$T$262	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_EwrapRegsOvly")
	.dwattr $C$DW$T$262, DW_AT_type(*$C$DW$T$261)
	.dwattr $C$DW$T$262, DW_AT_language(DW_LANG_C)

$C$DW$T$111	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$111, DW_AT_byte_size(0x98)
$C$DW$884	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$884, DW_AT_name("PID")
	.dwattr $C$DW$884, DW_AT_TI_symbol_name("_PID")
	.dwattr $C$DW$884, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$884, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$885	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$885, DW_AT_name("PCR")
	.dwattr $C$DW$885, DW_AT_TI_symbol_name("_PCR")
	.dwattr $C$DW$885, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$885, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$886	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$886, DW_AT_name("SYN_MODE")
	.dwattr $C$DW$886, DW_AT_TI_symbol_name("_SYN_MODE")
	.dwattr $C$DW$886, DW_AT_data_member_location[DW_OP_plus_uconst 0x8]
	.dwattr $C$DW$886, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$887	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$887, DW_AT_name("HD_VD_WID")
	.dwattr $C$DW$887, DW_AT_TI_symbol_name("_HD_VD_WID")
	.dwattr $C$DW$887, DW_AT_data_member_location[DW_OP_plus_uconst 0xc]
	.dwattr $C$DW$887, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$888	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$888, DW_AT_name("PIX_LINES")
	.dwattr $C$DW$888, DW_AT_TI_symbol_name("_PIX_LINES")
	.dwattr $C$DW$888, DW_AT_data_member_location[DW_OP_plus_uconst 0x10]
	.dwattr $C$DW$888, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$889	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$889, DW_AT_name("HORZ_INFO")
	.dwattr $C$DW$889, DW_AT_TI_symbol_name("_HORZ_INFO")
	.dwattr $C$DW$889, DW_AT_data_member_location[DW_OP_plus_uconst 0x14]
	.dwattr $C$DW$889, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$890	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$890, DW_AT_name("VERT_START")
	.dwattr $C$DW$890, DW_AT_TI_symbol_name("_VERT_START")
	.dwattr $C$DW$890, DW_AT_data_member_location[DW_OP_plus_uconst 0x18]
	.dwattr $C$DW$890, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$891	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$891, DW_AT_name("VERT_LINES")
	.dwattr $C$DW$891, DW_AT_TI_symbol_name("_VERT_LINES")
	.dwattr $C$DW$891, DW_AT_data_member_location[DW_OP_plus_uconst 0x1c]
	.dwattr $C$DW$891, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$892	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$892, DW_AT_name("CULLING")
	.dwattr $C$DW$892, DW_AT_TI_symbol_name("_CULLING")
	.dwattr $C$DW$892, DW_AT_data_member_location[DW_OP_plus_uconst 0x20]
	.dwattr $C$DW$892, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$893	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$893, DW_AT_name("HSIZE_OFF")
	.dwattr $C$DW$893, DW_AT_TI_symbol_name("_HSIZE_OFF")
	.dwattr $C$DW$893, DW_AT_data_member_location[DW_OP_plus_uconst 0x24]
	.dwattr $C$DW$893, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$894	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$894, DW_AT_name("SDOFST")
	.dwattr $C$DW$894, DW_AT_TI_symbol_name("_SDOFST")
	.dwattr $C$DW$894, DW_AT_data_member_location[DW_OP_plus_uconst 0x28]
	.dwattr $C$DW$894, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$895	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$895, DW_AT_name("SDR_ADDR")
	.dwattr $C$DW$895, DW_AT_TI_symbol_name("_SDR_ADDR")
	.dwattr $C$DW$895, DW_AT_data_member_location[DW_OP_plus_uconst 0x2c]
	.dwattr $C$DW$895, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$896	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$896, DW_AT_name("CLAMP")
	.dwattr $C$DW$896, DW_AT_TI_symbol_name("_CLAMP")
	.dwattr $C$DW$896, DW_AT_data_member_location[DW_OP_plus_uconst 0x30]
	.dwattr $C$DW$896, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$897	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$897, DW_AT_name("DCSUB")
	.dwattr $C$DW$897, DW_AT_TI_symbol_name("_DCSUB")
	.dwattr $C$DW$897, DW_AT_data_member_location[DW_OP_plus_uconst 0x34]
	.dwattr $C$DW$897, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$898	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$898, DW_AT_name("COLPTN")
	.dwattr $C$DW$898, DW_AT_TI_symbol_name("_COLPTN")
	.dwattr $C$DW$898, DW_AT_data_member_location[DW_OP_plus_uconst 0x38]
	.dwattr $C$DW$898, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$899	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$899, DW_AT_name("BLKCMP")
	.dwattr $C$DW$899, DW_AT_TI_symbol_name("_BLKCMP")
	.dwattr $C$DW$899, DW_AT_data_member_location[DW_OP_plus_uconst 0x3c]
	.dwattr $C$DW$899, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$900	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$900, DW_AT_name("FPC")
	.dwattr $C$DW$900, DW_AT_TI_symbol_name("_FPC")
	.dwattr $C$DW$900, DW_AT_data_member_location[DW_OP_plus_uconst 0x40]
	.dwattr $C$DW$900, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$901	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$901, DW_AT_name("FPC_ADDR")
	.dwattr $C$DW$901, DW_AT_TI_symbol_name("_FPC_ADDR")
	.dwattr $C$DW$901, DW_AT_data_member_location[DW_OP_plus_uconst 0x44]
	.dwattr $C$DW$901, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$902	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$902, DW_AT_name("VDINT")
	.dwattr $C$DW$902, DW_AT_TI_symbol_name("_VDINT")
	.dwattr $C$DW$902, DW_AT_data_member_location[DW_OP_plus_uconst 0x48]
	.dwattr $C$DW$902, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$903	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$903, DW_AT_name("ALAW")
	.dwattr $C$DW$903, DW_AT_TI_symbol_name("_ALAW")
	.dwattr $C$DW$903, DW_AT_data_member_location[DW_OP_plus_uconst 0x4c]
	.dwattr $C$DW$903, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$904	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$904, DW_AT_name("REC656IF")
	.dwattr $C$DW$904, DW_AT_TI_symbol_name("_REC656IF")
	.dwattr $C$DW$904, DW_AT_data_member_location[DW_OP_plus_uconst 0x50]
	.dwattr $C$DW$904, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$905	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$905, DW_AT_name("CCDCFG")
	.dwattr $C$DW$905, DW_AT_TI_symbol_name("_CCDCFG")
	.dwattr $C$DW$905, DW_AT_data_member_location[DW_OP_plus_uconst 0x54]
	.dwattr $C$DW$905, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$906	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$906, DW_AT_name("FMTCFG")
	.dwattr $C$DW$906, DW_AT_TI_symbol_name("_FMTCFG")
	.dwattr $C$DW$906, DW_AT_data_member_location[DW_OP_plus_uconst 0x58]
	.dwattr $C$DW$906, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$907	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$907, DW_AT_name("FMT_HORZ")
	.dwattr $C$DW$907, DW_AT_TI_symbol_name("_FMT_HORZ")
	.dwattr $C$DW$907, DW_AT_data_member_location[DW_OP_plus_uconst 0x5c]
	.dwattr $C$DW$907, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$908	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$908, DW_AT_name("FMT_VERT")
	.dwattr $C$DW$908, DW_AT_TI_symbol_name("_FMT_VERT")
	.dwattr $C$DW$908, DW_AT_data_member_location[DW_OP_plus_uconst 0x60]
	.dwattr $C$DW$908, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$909	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$909, DW_AT_name("FMT_ADDR0")
	.dwattr $C$DW$909, DW_AT_TI_symbol_name("_FMT_ADDR0")
	.dwattr $C$DW$909, DW_AT_data_member_location[DW_OP_plus_uconst 0x64]
	.dwattr $C$DW$909, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$910	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$910, DW_AT_name("FMT_ADDR1")
	.dwattr $C$DW$910, DW_AT_TI_symbol_name("_FMT_ADDR1")
	.dwattr $C$DW$910, DW_AT_data_member_location[DW_OP_plus_uconst 0x68]
	.dwattr $C$DW$910, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$911	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$911, DW_AT_name("FMT_ADDR2")
	.dwattr $C$DW$911, DW_AT_TI_symbol_name("_FMT_ADDR2")
	.dwattr $C$DW$911, DW_AT_data_member_location[DW_OP_plus_uconst 0x6c]
	.dwattr $C$DW$911, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$912	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$912, DW_AT_name("FMT_ADDR3")
	.dwattr $C$DW$912, DW_AT_TI_symbol_name("_FMT_ADDR3")
	.dwattr $C$DW$912, DW_AT_data_member_location[DW_OP_plus_uconst 0x70]
	.dwattr $C$DW$912, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$913	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$913, DW_AT_name("FMT_ADDR4")
	.dwattr $C$DW$913, DW_AT_TI_symbol_name("_FMT_ADDR4")
	.dwattr $C$DW$913, DW_AT_data_member_location[DW_OP_plus_uconst 0x74]
	.dwattr $C$DW$913, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$914	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$914, DW_AT_name("FMT_ADDR5")
	.dwattr $C$DW$914, DW_AT_TI_symbol_name("_FMT_ADDR5")
	.dwattr $C$DW$914, DW_AT_data_member_location[DW_OP_plus_uconst 0x78]
	.dwattr $C$DW$914, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$915	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$915, DW_AT_name("FMT_ADDR6")
	.dwattr $C$DW$915, DW_AT_TI_symbol_name("_FMT_ADDR6")
	.dwattr $C$DW$915, DW_AT_data_member_location[DW_OP_plus_uconst 0x7c]
	.dwattr $C$DW$915, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$916	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$916, DW_AT_name("FMT_ADDR7")
	.dwattr $C$DW$916, DW_AT_TI_symbol_name("_FMT_ADDR7")
	.dwattr $C$DW$916, DW_AT_data_member_location[DW_OP_plus_uconst 0x80]
	.dwattr $C$DW$916, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$917	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$917, DW_AT_name("PRGEVEN_0")
	.dwattr $C$DW$917, DW_AT_TI_symbol_name("_PRGEVEN_0")
	.dwattr $C$DW$917, DW_AT_data_member_location[DW_OP_plus_uconst 0x84]
	.dwattr $C$DW$917, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$918	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$918, DW_AT_name("PRGEVEN_1")
	.dwattr $C$DW$918, DW_AT_TI_symbol_name("_PRGEVEN_1")
	.dwattr $C$DW$918, DW_AT_data_member_location[DW_OP_plus_uconst 0x88]
	.dwattr $C$DW$918, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$919	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$919, DW_AT_name("PRGODD_0")
	.dwattr $C$DW$919, DW_AT_TI_symbol_name("_PRGODD_0")
	.dwattr $C$DW$919, DW_AT_data_member_location[DW_OP_plus_uconst 0x8c]
	.dwattr $C$DW$919, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$920	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$920, DW_AT_name("PRGODD_1")
	.dwattr $C$DW$920, DW_AT_TI_symbol_name("_PRGODD_1")
	.dwattr $C$DW$920, DW_AT_data_member_location[DW_OP_plus_uconst 0x90]
	.dwattr $C$DW$920, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$921	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$921, DW_AT_name("VP_OUT")
	.dwattr $C$DW$921, DW_AT_TI_symbol_name("_VP_OUT")
	.dwattr $C$DW$921, DW_AT_data_member_location[DW_OP_plus_uconst 0x94]
	.dwattr $C$DW$921, DW_AT_accessibility(DW_ACCESS_public)
	.dwendtag $C$DW$T$111

$C$DW$T$263	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_CcdcRegs")
	.dwattr $C$DW$T$263, DW_AT_type(*$C$DW$T$111)
	.dwattr $C$DW$T$263, DW_AT_language(DW_LANG_C)
$C$DW$T$264	.dwtag  DW_TAG_volatile_type
	.dwattr $C$DW$T$264, DW_AT_type(*$C$DW$T$263)
$C$DW$T$265	.dwtag  DW_TAG_pointer_type, DW_AT_type(*$C$DW$T$264)
	.dwattr $C$DW$T$265, DW_AT_address_class(0x20)
$C$DW$T$266	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_CcdcRegsOvly")
	.dwattr $C$DW$T$266, DW_AT_type(*$C$DW$T$265)
	.dwattr $C$DW$T$266, DW_AT_language(DW_LANG_C)

$C$DW$T$112	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$112, DW_AT_byte_size(0x60)
$C$DW$922	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$922, DW_AT_name("PID")
	.dwattr $C$DW$922, DW_AT_TI_symbol_name("_PID")
	.dwattr $C$DW$922, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$922, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$923	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$923, DW_AT_name("PCR")
	.dwattr $C$DW$923, DW_AT_TI_symbol_name("_PCR")
	.dwattr $C$DW$923, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$923, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$924	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$924, DW_AT_name("AFPAX1")
	.dwattr $C$DW$924, DW_AT_TI_symbol_name("_AFPAX1")
	.dwattr $C$DW$924, DW_AT_data_member_location[DW_OP_plus_uconst 0x8]
	.dwattr $C$DW$924, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$925	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$925, DW_AT_name("AFPAX2")
	.dwattr $C$DW$925, DW_AT_TI_symbol_name("_AFPAX2")
	.dwattr $C$DW$925, DW_AT_data_member_location[DW_OP_plus_uconst 0xc]
	.dwattr $C$DW$925, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$926	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$926, DW_AT_name("AFPAXSTART")
	.dwattr $C$DW$926, DW_AT_TI_symbol_name("_AFPAXSTART")
	.dwattr $C$DW$926, DW_AT_data_member_location[DW_OP_plus_uconst 0x10]
	.dwattr $C$DW$926, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$927	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$927, DW_AT_name("AFIIRSH")
	.dwattr $C$DW$927, DW_AT_TI_symbol_name("_AFIIRSH")
	.dwattr $C$DW$927, DW_AT_data_member_location[DW_OP_plus_uconst 0x14]
	.dwattr $C$DW$927, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$928	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$928, DW_AT_name("AFBUFST")
	.dwattr $C$DW$928, DW_AT_TI_symbol_name("_AFBUFST")
	.dwattr $C$DW$928, DW_AT_data_member_location[DW_OP_plus_uconst 0x18]
	.dwattr $C$DW$928, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$929	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$929, DW_AT_name("AFCOEF010")
	.dwattr $C$DW$929, DW_AT_TI_symbol_name("_AFCOEF010")
	.dwattr $C$DW$929, DW_AT_data_member_location[DW_OP_plus_uconst 0x1c]
	.dwattr $C$DW$929, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$930	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$930, DW_AT_name("AFCOEF032")
	.dwattr $C$DW$930, DW_AT_TI_symbol_name("_AFCOEF032")
	.dwattr $C$DW$930, DW_AT_data_member_location[DW_OP_plus_uconst 0x20]
	.dwattr $C$DW$930, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$931	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$931, DW_AT_name("AFCOEFF054")
	.dwattr $C$DW$931, DW_AT_TI_symbol_name("_AFCOEFF054")
	.dwattr $C$DW$931, DW_AT_data_member_location[DW_OP_plus_uconst 0x24]
	.dwattr $C$DW$931, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$932	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$932, DW_AT_name("AFCOEFF076")
	.dwattr $C$DW$932, DW_AT_TI_symbol_name("_AFCOEFF076")
	.dwattr $C$DW$932, DW_AT_data_member_location[DW_OP_plus_uconst 0x28]
	.dwattr $C$DW$932, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$933	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$933, DW_AT_name("AFCOEFF098")
	.dwattr $C$DW$933, DW_AT_TI_symbol_name("_AFCOEFF098")
	.dwattr $C$DW$933, DW_AT_data_member_location[DW_OP_plus_uconst 0x2c]
	.dwattr $C$DW$933, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$934	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$934, DW_AT_name("AFCOEFF0010")
	.dwattr $C$DW$934, DW_AT_TI_symbol_name("_AFCOEFF0010")
	.dwattr $C$DW$934, DW_AT_data_member_location[DW_OP_plus_uconst 0x30]
	.dwattr $C$DW$934, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$935	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$935, DW_AT_name("AFCOEF110")
	.dwattr $C$DW$935, DW_AT_TI_symbol_name("_AFCOEF110")
	.dwattr $C$DW$935, DW_AT_data_member_location[DW_OP_plus_uconst 0x34]
	.dwattr $C$DW$935, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$936	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$936, DW_AT_name("AFCOEF132")
	.dwattr $C$DW$936, DW_AT_TI_symbol_name("_AFCOEF132")
	.dwattr $C$DW$936, DW_AT_data_member_location[DW_OP_plus_uconst 0x38]
	.dwattr $C$DW$936, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$937	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$937, DW_AT_name("AFCOEFF154")
	.dwattr $C$DW$937, DW_AT_TI_symbol_name("_AFCOEFF154")
	.dwattr $C$DW$937, DW_AT_data_member_location[DW_OP_plus_uconst 0x3c]
	.dwattr $C$DW$937, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$938	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$938, DW_AT_name("AFCOEFF176")
	.dwattr $C$DW$938, DW_AT_TI_symbol_name("_AFCOEFF176")
	.dwattr $C$DW$938, DW_AT_data_member_location[DW_OP_plus_uconst 0x40]
	.dwattr $C$DW$938, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$939	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$939, DW_AT_name("AFCOEFF198")
	.dwattr $C$DW$939, DW_AT_TI_symbol_name("_AFCOEFF198")
	.dwattr $C$DW$939, DW_AT_data_member_location[DW_OP_plus_uconst 0x44]
	.dwattr $C$DW$939, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$940	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$940, DW_AT_name("AFCOEFF1010")
	.dwattr $C$DW$940, DW_AT_TI_symbol_name("_AFCOEFF1010")
	.dwattr $C$DW$940, DW_AT_data_member_location[DW_OP_plus_uconst 0x48]
	.dwattr $C$DW$940, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$941	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$941, DW_AT_name("AEWWIN1")
	.dwattr $C$DW$941, DW_AT_TI_symbol_name("_AEWWIN1")
	.dwattr $C$DW$941, DW_AT_data_member_location[DW_OP_plus_uconst 0x4c]
	.dwattr $C$DW$941, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$942	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$942, DW_AT_name("AEWINSTART")
	.dwattr $C$DW$942, DW_AT_TI_symbol_name("_AEWINSTART")
	.dwattr $C$DW$942, DW_AT_data_member_location[DW_OP_plus_uconst 0x50]
	.dwattr $C$DW$942, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$943	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$943, DW_AT_name("AEWINBLK")
	.dwattr $C$DW$943, DW_AT_TI_symbol_name("_AEWINBLK")
	.dwattr $C$DW$943, DW_AT_data_member_location[DW_OP_plus_uconst 0x54]
	.dwattr $C$DW$943, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$944	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$944, DW_AT_name("AEWSUBWIN")
	.dwattr $C$DW$944, DW_AT_TI_symbol_name("_AEWSUBWIN")
	.dwattr $C$DW$944, DW_AT_data_member_location[DW_OP_plus_uconst 0x58]
	.dwattr $C$DW$944, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$945	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$945, DW_AT_name("AEWBUFST")
	.dwattr $C$DW$945, DW_AT_TI_symbol_name("_AEWBUFST")
	.dwattr $C$DW$945, DW_AT_data_member_location[DW_OP_plus_uconst 0x5c]
	.dwattr $C$DW$945, DW_AT_accessibility(DW_ACCESS_public)
	.dwendtag $C$DW$T$112

$C$DW$T$267	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_H3aRegs")
	.dwattr $C$DW$T$267, DW_AT_type(*$C$DW$T$112)
	.dwattr $C$DW$T$267, DW_AT_language(DW_LANG_C)
$C$DW$T$268	.dwtag  DW_TAG_volatile_type
	.dwattr $C$DW$T$268, DW_AT_type(*$C$DW$T$267)
$C$DW$T$269	.dwtag  DW_TAG_pointer_type, DW_AT_type(*$C$DW$T$268)
	.dwattr $C$DW$T$269, DW_AT_address_class(0x20)
$C$DW$T$270	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_H3aRegsOvly")
	.dwattr $C$DW$T$270, DW_AT_type(*$C$DW$T$269)
	.dwattr $C$DW$T$270, DW_AT_language(DW_LANG_C)

$C$DW$T$113	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$113, DW_AT_byte_size(0x44)
$C$DW$946	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$946, DW_AT_name("PID")
	.dwattr $C$DW$946, DW_AT_TI_symbol_name("_PID")
	.dwattr $C$DW$946, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$946, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$947	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$947, DW_AT_name("PCR")
	.dwattr $C$DW$947, DW_AT_TI_symbol_name("_PCR")
	.dwattr $C$DW$947, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$947, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$948	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$948, DW_AT_name("HIST_CNT")
	.dwattr $C$DW$948, DW_AT_TI_symbol_name("_HIST_CNT")
	.dwattr $C$DW$948, DW_AT_data_member_location[DW_OP_plus_uconst 0x8]
	.dwattr $C$DW$948, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$949	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$949, DW_AT_name("WB_GAIN")
	.dwattr $C$DW$949, DW_AT_TI_symbol_name("_WB_GAIN")
	.dwattr $C$DW$949, DW_AT_data_member_location[DW_OP_plus_uconst 0xc]
	.dwattr $C$DW$949, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$950	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$950, DW_AT_name("R0_HORZ")
	.dwattr $C$DW$950, DW_AT_TI_symbol_name("_R0_HORZ")
	.dwattr $C$DW$950, DW_AT_data_member_location[DW_OP_plus_uconst 0x10]
	.dwattr $C$DW$950, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$951	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$951, DW_AT_name("R0_VERT")
	.dwattr $C$DW$951, DW_AT_TI_symbol_name("_R0_VERT")
	.dwattr $C$DW$951, DW_AT_data_member_location[DW_OP_plus_uconst 0x14]
	.dwattr $C$DW$951, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$952	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$952, DW_AT_name("R1_HORZ")
	.dwattr $C$DW$952, DW_AT_TI_symbol_name("_R1_HORZ")
	.dwattr $C$DW$952, DW_AT_data_member_location[DW_OP_plus_uconst 0x18]
	.dwattr $C$DW$952, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$953	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$953, DW_AT_name("R1_VERT")
	.dwattr $C$DW$953, DW_AT_TI_symbol_name("_R1_VERT")
	.dwattr $C$DW$953, DW_AT_data_member_location[DW_OP_plus_uconst 0x1c]
	.dwattr $C$DW$953, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$954	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$954, DW_AT_name("R2_HORZ")
	.dwattr $C$DW$954, DW_AT_TI_symbol_name("_R2_HORZ")
	.dwattr $C$DW$954, DW_AT_data_member_location[DW_OP_plus_uconst 0x20]
	.dwattr $C$DW$954, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$955	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$955, DW_AT_name("R2_VERT")
	.dwattr $C$DW$955, DW_AT_TI_symbol_name("_R2_VERT")
	.dwattr $C$DW$955, DW_AT_data_member_location[DW_OP_plus_uconst 0x24]
	.dwattr $C$DW$955, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$956	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$956, DW_AT_name("R3_HORZ")
	.dwattr $C$DW$956, DW_AT_TI_symbol_name("_R3_HORZ")
	.dwattr $C$DW$956, DW_AT_data_member_location[DW_OP_plus_uconst 0x28]
	.dwattr $C$DW$956, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$957	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$957, DW_AT_name("R3_VERT")
	.dwattr $C$DW$957, DW_AT_TI_symbol_name("_R3_VERT")
	.dwattr $C$DW$957, DW_AT_data_member_location[DW_OP_plus_uconst 0x2c]
	.dwattr $C$DW$957, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$958	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$958, DW_AT_name("HIST_ADDR")
	.dwattr $C$DW$958, DW_AT_TI_symbol_name("_HIST_ADDR")
	.dwattr $C$DW$958, DW_AT_data_member_location[DW_OP_plus_uconst 0x30]
	.dwattr $C$DW$958, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$959	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$959, DW_AT_name("HIST_DATA")
	.dwattr $C$DW$959, DW_AT_TI_symbol_name("_HIST_DATA")
	.dwattr $C$DW$959, DW_AT_data_member_location[DW_OP_plus_uconst 0x34]
	.dwattr $C$DW$959, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$960	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$960, DW_AT_name("RADD")
	.dwattr $C$DW$960, DW_AT_TI_symbol_name("_RADD")
	.dwattr $C$DW$960, DW_AT_data_member_location[DW_OP_plus_uconst 0x38]
	.dwattr $C$DW$960, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$961	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$961, DW_AT_name("RADD_OFF")
	.dwattr $C$DW$961, DW_AT_TI_symbol_name("_RADD_OFF")
	.dwattr $C$DW$961, DW_AT_data_member_location[DW_OP_plus_uconst 0x3c]
	.dwattr $C$DW$961, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$962	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$962, DW_AT_name("H_V_INFO")
	.dwattr $C$DW$962, DW_AT_TI_symbol_name("_H_V_INFO")
	.dwattr $C$DW$962, DW_AT_data_member_location[DW_OP_plus_uconst 0x40]
	.dwattr $C$DW$962, DW_AT_accessibility(DW_ACCESS_public)
	.dwendtag $C$DW$T$113

$C$DW$T$271	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_HistRegs")
	.dwattr $C$DW$T$271, DW_AT_type(*$C$DW$T$113)
	.dwattr $C$DW$T$271, DW_AT_language(DW_LANG_C)
$C$DW$T$272	.dwtag  DW_TAG_volatile_type
	.dwattr $C$DW$T$272, DW_AT_type(*$C$DW$T$271)
$C$DW$T$273	.dwtag  DW_TAG_pointer_type, DW_AT_type(*$C$DW$T$272)
	.dwattr $C$DW$T$273, DW_AT_address_class(0x20)
$C$DW$T$274	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_HistRegsOvly")
	.dwattr $C$DW$T$274, DW_AT_type(*$C$DW$T$273)
	.dwattr $C$DW$T$274, DW_AT_language(DW_LANG_C)

$C$DW$T$114	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$114, DW_AT_byte_size(0x100)
$C$DW$963	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$963, DW_AT_name("MODE")
	.dwattr $C$DW$963, DW_AT_TI_symbol_name("_MODE")
	.dwattr $C$DW$963, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$963, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$964	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$964, DW_AT_name("VIDWINMD")
	.dwattr $C$DW$964, DW_AT_TI_symbol_name("_VIDWINMD")
	.dwattr $C$DW$964, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$964, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$965	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$965, DW_AT_name("OSDWIN0MD")
	.dwattr $C$DW$965, DW_AT_TI_symbol_name("_OSDWIN0MD")
	.dwattr $C$DW$965, DW_AT_data_member_location[DW_OP_plus_uconst 0x8]
	.dwattr $C$DW$965, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$966	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$966, DW_AT_name("OSDWIN1MD")
	.dwattr $C$DW$966, DW_AT_TI_symbol_name("_OSDWIN1MD")
	.dwattr $C$DW$966, DW_AT_data_member_location[DW_OP_plus_uconst 0xc]
	.dwattr $C$DW$966, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$967	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$967, DW_AT_name("RECTCUR")
	.dwattr $C$DW$967, DW_AT_TI_symbol_name("_RECTCUR")
	.dwattr $C$DW$967, DW_AT_data_member_location[DW_OP_plus_uconst 0x10]
	.dwattr $C$DW$967, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$968	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$968, DW_AT_name("RSV0")
	.dwattr $C$DW$968, DW_AT_TI_symbol_name("_RSV0")
	.dwattr $C$DW$968, DW_AT_data_member_location[DW_OP_plus_uconst 0x14]
	.dwattr $C$DW$968, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$969	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$969, DW_AT_name("VIDWIN0OFST")
	.dwattr $C$DW$969, DW_AT_TI_symbol_name("_VIDWIN0OFST")
	.dwattr $C$DW$969, DW_AT_data_member_location[DW_OP_plus_uconst 0x18]
	.dwattr $C$DW$969, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$970	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$970, DW_AT_name("VIDWIN1OFST")
	.dwattr $C$DW$970, DW_AT_TI_symbol_name("_VIDWIN1OFST")
	.dwattr $C$DW$970, DW_AT_data_member_location[DW_OP_plus_uconst 0x1c]
	.dwattr $C$DW$970, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$971	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$971, DW_AT_name("OSDWIN0OFST")
	.dwattr $C$DW$971, DW_AT_TI_symbol_name("_OSDWIN0OFST")
	.dwattr $C$DW$971, DW_AT_data_member_location[DW_OP_plus_uconst 0x20]
	.dwattr $C$DW$971, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$972	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$972, DW_AT_name("OSDWIN1OFST")
	.dwattr $C$DW$972, DW_AT_TI_symbol_name("_OSDWIN1OFST")
	.dwattr $C$DW$972, DW_AT_data_member_location[DW_OP_plus_uconst 0x24]
	.dwattr $C$DW$972, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$973	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$973, DW_AT_name("RSV1")
	.dwattr $C$DW$973, DW_AT_TI_symbol_name("_RSV1")
	.dwattr $C$DW$973, DW_AT_data_member_location[DW_OP_plus_uconst 0x28]
	.dwattr $C$DW$973, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$974	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$974, DW_AT_name("VIDWIN0ADR")
	.dwattr $C$DW$974, DW_AT_TI_symbol_name("_VIDWIN0ADR")
	.dwattr $C$DW$974, DW_AT_data_member_location[DW_OP_plus_uconst 0x2c]
	.dwattr $C$DW$974, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$975	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$975, DW_AT_name("VIDWIN1ADR")
	.dwattr $C$DW$975, DW_AT_TI_symbol_name("_VIDWIN1ADR")
	.dwattr $C$DW$975, DW_AT_data_member_location[DW_OP_plus_uconst 0x30]
	.dwattr $C$DW$975, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$976	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$976, DW_AT_name("RSV2")
	.dwattr $C$DW$976, DW_AT_TI_symbol_name("_RSV2")
	.dwattr $C$DW$976, DW_AT_data_member_location[DW_OP_plus_uconst 0x34]
	.dwattr $C$DW$976, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$977	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$977, DW_AT_name("OSDWIN0ADR")
	.dwattr $C$DW$977, DW_AT_TI_symbol_name("_OSDWIN0ADR")
	.dwattr $C$DW$977, DW_AT_data_member_location[DW_OP_plus_uconst 0x38]
	.dwattr $C$DW$977, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$978	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$978, DW_AT_name("OSDWIN1ADR")
	.dwattr $C$DW$978, DW_AT_TI_symbol_name("_OSDWIN1ADR")
	.dwattr $C$DW$978, DW_AT_data_member_location[DW_OP_plus_uconst 0x3c]
	.dwattr $C$DW$978, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$979	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$979, DW_AT_name("BASEPX")
	.dwattr $C$DW$979, DW_AT_TI_symbol_name("_BASEPX")
	.dwattr $C$DW$979, DW_AT_data_member_location[DW_OP_plus_uconst 0x40]
	.dwattr $C$DW$979, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$980	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$980, DW_AT_name("BASEPY")
	.dwattr $C$DW$980, DW_AT_TI_symbol_name("_BASEPY")
	.dwattr $C$DW$980, DW_AT_data_member_location[DW_OP_plus_uconst 0x44]
	.dwattr $C$DW$980, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$981	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$981, DW_AT_name("VIDWIN0XP")
	.dwattr $C$DW$981, DW_AT_TI_symbol_name("_VIDWIN0XP")
	.dwattr $C$DW$981, DW_AT_data_member_location[DW_OP_plus_uconst 0x48]
	.dwattr $C$DW$981, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$982	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$982, DW_AT_name("VIDWIN0YP")
	.dwattr $C$DW$982, DW_AT_TI_symbol_name("_VIDWIN0YP")
	.dwattr $C$DW$982, DW_AT_data_member_location[DW_OP_plus_uconst 0x4c]
	.dwattr $C$DW$982, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$983	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$983, DW_AT_name("VIDWIN0XL")
	.dwattr $C$DW$983, DW_AT_TI_symbol_name("_VIDWIN0XL")
	.dwattr $C$DW$983, DW_AT_data_member_location[DW_OP_plus_uconst 0x50]
	.dwattr $C$DW$983, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$984	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$984, DW_AT_name("VIDWIN0YL")
	.dwattr $C$DW$984, DW_AT_TI_symbol_name("_VIDWIN0YL")
	.dwattr $C$DW$984, DW_AT_data_member_location[DW_OP_plus_uconst 0x54]
	.dwattr $C$DW$984, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$985	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$985, DW_AT_name("VIDWIN1XP")
	.dwattr $C$DW$985, DW_AT_TI_symbol_name("_VIDWIN1XP")
	.dwattr $C$DW$985, DW_AT_data_member_location[DW_OP_plus_uconst 0x58]
	.dwattr $C$DW$985, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$986	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$986, DW_AT_name("VIDWIN1YP")
	.dwattr $C$DW$986, DW_AT_TI_symbol_name("_VIDWIN1YP")
	.dwattr $C$DW$986, DW_AT_data_member_location[DW_OP_plus_uconst 0x5c]
	.dwattr $C$DW$986, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$987	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$987, DW_AT_name("VIDWIN1XL")
	.dwattr $C$DW$987, DW_AT_TI_symbol_name("_VIDWIN1XL")
	.dwattr $C$DW$987, DW_AT_data_member_location[DW_OP_plus_uconst 0x60]
	.dwattr $C$DW$987, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$988	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$988, DW_AT_name("VIDWIN1YL")
	.dwattr $C$DW$988, DW_AT_TI_symbol_name("_VIDWIN1YL")
	.dwattr $C$DW$988, DW_AT_data_member_location[DW_OP_plus_uconst 0x64]
	.dwattr $C$DW$988, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$989	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$989, DW_AT_name("OSDWIN0XP")
	.dwattr $C$DW$989, DW_AT_TI_symbol_name("_OSDWIN0XP")
	.dwattr $C$DW$989, DW_AT_data_member_location[DW_OP_plus_uconst 0x68]
	.dwattr $C$DW$989, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$990	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$990, DW_AT_name("OSDWIN0YP")
	.dwattr $C$DW$990, DW_AT_TI_symbol_name("_OSDWIN0YP")
	.dwattr $C$DW$990, DW_AT_data_member_location[DW_OP_plus_uconst 0x6c]
	.dwattr $C$DW$990, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$991	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$991, DW_AT_name("OSDWIN0XL")
	.dwattr $C$DW$991, DW_AT_TI_symbol_name("_OSDWIN0XL")
	.dwattr $C$DW$991, DW_AT_data_member_location[DW_OP_plus_uconst 0x70]
	.dwattr $C$DW$991, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$992	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$992, DW_AT_name("OSDWIN0YL")
	.dwattr $C$DW$992, DW_AT_TI_symbol_name("_OSDWIN0YL")
	.dwattr $C$DW$992, DW_AT_data_member_location[DW_OP_plus_uconst 0x74]
	.dwattr $C$DW$992, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$993	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$993, DW_AT_name("OSDWIN1XP")
	.dwattr $C$DW$993, DW_AT_TI_symbol_name("_OSDWIN1XP")
	.dwattr $C$DW$993, DW_AT_data_member_location[DW_OP_plus_uconst 0x78]
	.dwattr $C$DW$993, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$994	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$994, DW_AT_name("OSDWIN1YP")
	.dwattr $C$DW$994, DW_AT_TI_symbol_name("_OSDWIN1YP")
	.dwattr $C$DW$994, DW_AT_data_member_location[DW_OP_plus_uconst 0x7c]
	.dwattr $C$DW$994, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$995	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$995, DW_AT_name("OSDWIN1XL")
	.dwattr $C$DW$995, DW_AT_TI_symbol_name("_OSDWIN1XL")
	.dwattr $C$DW$995, DW_AT_data_member_location[DW_OP_plus_uconst 0x80]
	.dwattr $C$DW$995, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$996	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$996, DW_AT_name("OSDWIN1YL")
	.dwattr $C$DW$996, DW_AT_TI_symbol_name("_OSDWIN1YL")
	.dwattr $C$DW$996, DW_AT_data_member_location[DW_OP_plus_uconst 0x84]
	.dwattr $C$DW$996, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$997	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$997, DW_AT_name("CURXP")
	.dwattr $C$DW$997, DW_AT_TI_symbol_name("_CURXP")
	.dwattr $C$DW$997, DW_AT_data_member_location[DW_OP_plus_uconst 0x88]
	.dwattr $C$DW$997, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$998	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$998, DW_AT_name("CURYP")
	.dwattr $C$DW$998, DW_AT_TI_symbol_name("_CURYP")
	.dwattr $C$DW$998, DW_AT_data_member_location[DW_OP_plus_uconst 0x8c]
	.dwattr $C$DW$998, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$999	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$999, DW_AT_name("CURXL")
	.dwattr $C$DW$999, DW_AT_TI_symbol_name("_CURXL")
	.dwattr $C$DW$999, DW_AT_data_member_location[DW_OP_plus_uconst 0x90]
	.dwattr $C$DW$999, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1000	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1000, DW_AT_name("CURYL")
	.dwattr $C$DW$1000, DW_AT_TI_symbol_name("_CURYL")
	.dwattr $C$DW$1000, DW_AT_data_member_location[DW_OP_plus_uconst 0x94]
	.dwattr $C$DW$1000, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1001	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1001, DW_AT_name("RSV3")
	.dwattr $C$DW$1001, DW_AT_TI_symbol_name("_RSV3")
	.dwattr $C$DW$1001, DW_AT_data_member_location[DW_OP_plus_uconst 0x98]
	.dwattr $C$DW$1001, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1002	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1002, DW_AT_name("RSV4")
	.dwattr $C$DW$1002, DW_AT_TI_symbol_name("_RSV4")
	.dwattr $C$DW$1002, DW_AT_data_member_location[DW_OP_plus_uconst 0x9c]
	.dwattr $C$DW$1002, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1003	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1003, DW_AT_name("W0BMP01")
	.dwattr $C$DW$1003, DW_AT_TI_symbol_name("_W0BMP01")
	.dwattr $C$DW$1003, DW_AT_data_member_location[DW_OP_plus_uconst 0xa0]
	.dwattr $C$DW$1003, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1004	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1004, DW_AT_name("W0BMP23")
	.dwattr $C$DW$1004, DW_AT_TI_symbol_name("_W0BMP23")
	.dwattr $C$DW$1004, DW_AT_data_member_location[DW_OP_plus_uconst 0xa4]
	.dwattr $C$DW$1004, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1005	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1005, DW_AT_name("W0BMP45")
	.dwattr $C$DW$1005, DW_AT_TI_symbol_name("_W0BMP45")
	.dwattr $C$DW$1005, DW_AT_data_member_location[DW_OP_plus_uconst 0xa8]
	.dwattr $C$DW$1005, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1006	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1006, DW_AT_name("W0BMP67")
	.dwattr $C$DW$1006, DW_AT_TI_symbol_name("_W0BMP67")
	.dwattr $C$DW$1006, DW_AT_data_member_location[DW_OP_plus_uconst 0xac]
	.dwattr $C$DW$1006, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1007	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1007, DW_AT_name("W0BMP89")
	.dwattr $C$DW$1007, DW_AT_TI_symbol_name("_W0BMP89")
	.dwattr $C$DW$1007, DW_AT_data_member_location[DW_OP_plus_uconst 0xb0]
	.dwattr $C$DW$1007, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1008	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1008, DW_AT_name("W0BMPAB")
	.dwattr $C$DW$1008, DW_AT_TI_symbol_name("_W0BMPAB")
	.dwattr $C$DW$1008, DW_AT_data_member_location[DW_OP_plus_uconst 0xb4]
	.dwattr $C$DW$1008, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1009	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1009, DW_AT_name("W0BMPCD")
	.dwattr $C$DW$1009, DW_AT_TI_symbol_name("_W0BMPCD")
	.dwattr $C$DW$1009, DW_AT_data_member_location[DW_OP_plus_uconst 0xb8]
	.dwattr $C$DW$1009, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1010	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1010, DW_AT_name("W0BMPEF")
	.dwattr $C$DW$1010, DW_AT_TI_symbol_name("_W0BMPEF")
	.dwattr $C$DW$1010, DW_AT_data_member_location[DW_OP_plus_uconst 0xbc]
	.dwattr $C$DW$1010, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1011	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1011, DW_AT_name("W1BMP01")
	.dwattr $C$DW$1011, DW_AT_TI_symbol_name("_W1BMP01")
	.dwattr $C$DW$1011, DW_AT_data_member_location[DW_OP_plus_uconst 0xc0]
	.dwattr $C$DW$1011, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1012	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1012, DW_AT_name("W1BMP23")
	.dwattr $C$DW$1012, DW_AT_TI_symbol_name("_W1BMP23")
	.dwattr $C$DW$1012, DW_AT_data_member_location[DW_OP_plus_uconst 0xc4]
	.dwattr $C$DW$1012, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1013	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1013, DW_AT_name("W1BMP45")
	.dwattr $C$DW$1013, DW_AT_TI_symbol_name("_W1BMP45")
	.dwattr $C$DW$1013, DW_AT_data_member_location[DW_OP_plus_uconst 0xc8]
	.dwattr $C$DW$1013, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1014	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1014, DW_AT_name("W1BMP67")
	.dwattr $C$DW$1014, DW_AT_TI_symbol_name("_W1BMP67")
	.dwattr $C$DW$1014, DW_AT_data_member_location[DW_OP_plus_uconst 0xcc]
	.dwattr $C$DW$1014, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1015	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1015, DW_AT_name("W1BMP89")
	.dwattr $C$DW$1015, DW_AT_TI_symbol_name("_W1BMP89")
	.dwattr $C$DW$1015, DW_AT_data_member_location[DW_OP_plus_uconst 0xd0]
	.dwattr $C$DW$1015, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1016	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1016, DW_AT_name("W1BMPAB")
	.dwattr $C$DW$1016, DW_AT_TI_symbol_name("_W1BMPAB")
	.dwattr $C$DW$1016, DW_AT_data_member_location[DW_OP_plus_uconst 0xd4]
	.dwattr $C$DW$1016, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1017	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1017, DW_AT_name("W1BMPCD")
	.dwattr $C$DW$1017, DW_AT_TI_symbol_name("_W1BMPCD")
	.dwattr $C$DW$1017, DW_AT_data_member_location[DW_OP_plus_uconst 0xd8]
	.dwattr $C$DW$1017, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1018	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1018, DW_AT_name("W1BMPEF")
	.dwattr $C$DW$1018, DW_AT_TI_symbol_name("_W1BMPEF")
	.dwattr $C$DW$1018, DW_AT_data_member_location[DW_OP_plus_uconst 0xdc]
	.dwattr $C$DW$1018, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1019	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1019, DW_AT_name("TI_TEST")
	.dwattr $C$DW$1019, DW_AT_TI_symbol_name("_TI_TEST")
	.dwattr $C$DW$1019, DW_AT_data_member_location[DW_OP_plus_uconst 0xe0]
	.dwattr $C$DW$1019, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1020	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1020, DW_AT_name("MISCCTL")
	.dwattr $C$DW$1020, DW_AT_TI_symbol_name("_MISCCTL")
	.dwattr $C$DW$1020, DW_AT_data_member_location[DW_OP_plus_uconst 0xe4]
	.dwattr $C$DW$1020, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1021	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1021, DW_AT_name("RSV5")
	.dwattr $C$DW$1021, DW_AT_TI_symbol_name("_RSV5")
	.dwattr $C$DW$1021, DW_AT_data_member_location[DW_OP_plus_uconst 0xe8]
	.dwattr $C$DW$1021, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1022	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1022, DW_AT_name("CLUTRAMYCB")
	.dwattr $C$DW$1022, DW_AT_TI_symbol_name("_CLUTRAMYCB")
	.dwattr $C$DW$1022, DW_AT_data_member_location[DW_OP_plus_uconst 0xec]
	.dwattr $C$DW$1022, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1023	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1023, DW_AT_name("CLUTRAMCR")
	.dwattr $C$DW$1023, DW_AT_TI_symbol_name("_CLUTRAMCR")
	.dwattr $C$DW$1023, DW_AT_data_member_location[DW_OP_plus_uconst 0xf0]
	.dwattr $C$DW$1023, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1024	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1024, DW_AT_name("TRANSPVAL")
	.dwattr $C$DW$1024, DW_AT_TI_symbol_name("_TRANSPVAL")
	.dwattr $C$DW$1024, DW_AT_data_member_location[DW_OP_plus_uconst 0xf4]
	.dwattr $C$DW$1024, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1025	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1025, DW_AT_name("RSV6")
	.dwattr $C$DW$1025, DW_AT_TI_symbol_name("_RSV6")
	.dwattr $C$DW$1025, DW_AT_data_member_location[DW_OP_plus_uconst 0xf8]
	.dwattr $C$DW$1025, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1026	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1026, DW_AT_name("PPVWIN0ADR")
	.dwattr $C$DW$1026, DW_AT_TI_symbol_name("_PPVWIN0ADR")
	.dwattr $C$DW$1026, DW_AT_data_member_location[DW_OP_plus_uconst 0xfc]
	.dwattr $C$DW$1026, DW_AT_accessibility(DW_ACCESS_public)
	.dwendtag $C$DW$T$114

$C$DW$T$275	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_OsdRegs")
	.dwattr $C$DW$T$275, DW_AT_type(*$C$DW$T$114)
	.dwattr $C$DW$T$275, DW_AT_language(DW_LANG_C)
$C$DW$T$276	.dwtag  DW_TAG_volatile_type
	.dwattr $C$DW$T$276, DW_AT_type(*$C$DW$T$275)
$C$DW$T$277	.dwtag  DW_TAG_pointer_type, DW_AT_type(*$C$DW$T$276)
	.dwattr $C$DW$T$277, DW_AT_address_class(0x20)
$C$DW$T$278	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_OsdRegsOvly")
	.dwattr $C$DW$T$278, DW_AT_type(*$C$DW$T$277)
	.dwattr $C$DW$T$278, DW_AT_language(DW_LANG_C)

$C$DW$T$115	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$115, DW_AT_byte_size(0x88)
$C$DW$1027	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1027, DW_AT_name("PID")
	.dwattr $C$DW$1027, DW_AT_TI_symbol_name("_PID")
	.dwattr $C$DW$1027, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1027, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1028	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1028, DW_AT_name("PCR")
	.dwattr $C$DW$1028, DW_AT_TI_symbol_name("_PCR")
	.dwattr $C$DW$1028, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$1028, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1029	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1029, DW_AT_name("HORZ_INFO")
	.dwattr $C$DW$1029, DW_AT_TI_symbol_name("_HORZ_INFO")
	.dwattr $C$DW$1029, DW_AT_data_member_location[DW_OP_plus_uconst 0x8]
	.dwattr $C$DW$1029, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1030	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1030, DW_AT_name("VERT_INFO")
	.dwattr $C$DW$1030, DW_AT_TI_symbol_name("_VERT_INFO")
	.dwattr $C$DW$1030, DW_AT_data_member_location[DW_OP_plus_uconst 0xc]
	.dwattr $C$DW$1030, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1031	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1031, DW_AT_name("RSDR_ADDR")
	.dwattr $C$DW$1031, DW_AT_TI_symbol_name("_RSDR_ADDR")
	.dwattr $C$DW$1031, DW_AT_data_member_location[DW_OP_plus_uconst 0x10]
	.dwattr $C$DW$1031, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1032	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1032, DW_AT_name("RADR_OFFSET")
	.dwattr $C$DW$1032, DW_AT_TI_symbol_name("_RADR_OFFSET")
	.dwattr $C$DW$1032, DW_AT_data_member_location[DW_OP_plus_uconst 0x14]
	.dwattr $C$DW$1032, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1033	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1033, DW_AT_name("DSDR_ADDR")
	.dwattr $C$DW$1033, DW_AT_TI_symbol_name("_DSDR_ADDR")
	.dwattr $C$DW$1033, DW_AT_data_member_location[DW_OP_plus_uconst 0x18]
	.dwattr $C$DW$1033, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1034	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1034, DW_AT_name("DRKF_OFFSET")
	.dwattr $C$DW$1034, DW_AT_TI_symbol_name("_DRKF_OFFSET")
	.dwattr $C$DW$1034, DW_AT_data_member_location[DW_OP_plus_uconst 0x1c]
	.dwattr $C$DW$1034, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1035	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1035, DW_AT_name("WSDR_ADDR")
	.dwattr $C$DW$1035, DW_AT_TI_symbol_name("_WSDR_ADDR")
	.dwattr $C$DW$1035, DW_AT_data_member_location[DW_OP_plus_uconst 0x20]
	.dwattr $C$DW$1035, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1036	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1036, DW_AT_name("WADD_OFFSET")
	.dwattr $C$DW$1036, DW_AT_TI_symbol_name("_WADD_OFFSET")
	.dwattr $C$DW$1036, DW_AT_data_member_location[DW_OP_plus_uconst 0x24]
	.dwattr $C$DW$1036, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1037	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1037, DW_AT_name("AVE")
	.dwattr $C$DW$1037, DW_AT_TI_symbol_name("_AVE")
	.dwattr $C$DW$1037, DW_AT_data_member_location[DW_OP_plus_uconst 0x28]
	.dwattr $C$DW$1037, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1038	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1038, DW_AT_name("HMED")
	.dwattr $C$DW$1038, DW_AT_TI_symbol_name("_HMED")
	.dwattr $C$DW$1038, DW_AT_data_member_location[DW_OP_plus_uconst 0x2c]
	.dwattr $C$DW$1038, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1039	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1039, DW_AT_name("NF")
	.dwattr $C$DW$1039, DW_AT_TI_symbol_name("_NF")
	.dwattr $C$DW$1039, DW_AT_data_member_location[DW_OP_plus_uconst 0x30]
	.dwattr $C$DW$1039, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1040	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1040, DW_AT_name("WB_DGAIN")
	.dwattr $C$DW$1040, DW_AT_TI_symbol_name("_WB_DGAIN")
	.dwattr $C$DW$1040, DW_AT_data_member_location[DW_OP_plus_uconst 0x34]
	.dwattr $C$DW$1040, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1041	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1041, DW_AT_name("WBGAIN")
	.dwattr $C$DW$1041, DW_AT_TI_symbol_name("_WBGAIN")
	.dwattr $C$DW$1041, DW_AT_data_member_location[DW_OP_plus_uconst 0x38]
	.dwattr $C$DW$1041, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1042	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1042, DW_AT_name("WBSEL")
	.dwattr $C$DW$1042, DW_AT_TI_symbol_name("_WBSEL")
	.dwattr $C$DW$1042, DW_AT_data_member_location[DW_OP_plus_uconst 0x3c]
	.dwattr $C$DW$1042, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1043	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1043, DW_AT_name("CFA")
	.dwattr $C$DW$1043, DW_AT_TI_symbol_name("_CFA")
	.dwattr $C$DW$1043, DW_AT_data_member_location[DW_OP_plus_uconst 0x40]
	.dwattr $C$DW$1043, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1044	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1044, DW_AT_name("BLKADJOFF")
	.dwattr $C$DW$1044, DW_AT_TI_symbol_name("_BLKADJOFF")
	.dwattr $C$DW$1044, DW_AT_data_member_location[DW_OP_plus_uconst 0x44]
	.dwattr $C$DW$1044, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1045	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1045, DW_AT_name("RGB_MAT1")
	.dwattr $C$DW$1045, DW_AT_TI_symbol_name("_RGB_MAT1")
	.dwattr $C$DW$1045, DW_AT_data_member_location[DW_OP_plus_uconst 0x48]
	.dwattr $C$DW$1045, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1046	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1046, DW_AT_name("RGB_MAT2")
	.dwattr $C$DW$1046, DW_AT_TI_symbol_name("_RGB_MAT2")
	.dwattr $C$DW$1046, DW_AT_data_member_location[DW_OP_plus_uconst 0x4c]
	.dwattr $C$DW$1046, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1047	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1047, DW_AT_name("RGB_MAT3")
	.dwattr $C$DW$1047, DW_AT_TI_symbol_name("_RGB_MAT3")
	.dwattr $C$DW$1047, DW_AT_data_member_location[DW_OP_plus_uconst 0x50]
	.dwattr $C$DW$1047, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1048	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1048, DW_AT_name("RGB_MAT4")
	.dwattr $C$DW$1048, DW_AT_TI_symbol_name("_RGB_MAT4")
	.dwattr $C$DW$1048, DW_AT_data_member_location[DW_OP_plus_uconst 0x54]
	.dwattr $C$DW$1048, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1049	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1049, DW_AT_name("RGB_MAT5")
	.dwattr $C$DW$1049, DW_AT_TI_symbol_name("_RGB_MAT5")
	.dwattr $C$DW$1049, DW_AT_data_member_location[DW_OP_plus_uconst 0x58]
	.dwattr $C$DW$1049, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1050	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1050, DW_AT_name("RGB_OFF1")
	.dwattr $C$DW$1050, DW_AT_TI_symbol_name("_RGB_OFF1")
	.dwattr $C$DW$1050, DW_AT_data_member_location[DW_OP_plus_uconst 0x5c]
	.dwattr $C$DW$1050, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1051	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1051, DW_AT_name("RGB_OFF2")
	.dwattr $C$DW$1051, DW_AT_TI_symbol_name("_RGB_OFF2")
	.dwattr $C$DW$1051, DW_AT_data_member_location[DW_OP_plus_uconst 0x60]
	.dwattr $C$DW$1051, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1052	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1052, DW_AT_name("CSC0")
	.dwattr $C$DW$1052, DW_AT_TI_symbol_name("_CSC0")
	.dwattr $C$DW$1052, DW_AT_data_member_location[DW_OP_plus_uconst 0x64]
	.dwattr $C$DW$1052, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1053	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1053, DW_AT_name("CSC1")
	.dwattr $C$DW$1053, DW_AT_TI_symbol_name("_CSC1")
	.dwattr $C$DW$1053, DW_AT_data_member_location[DW_OP_plus_uconst 0x68]
	.dwattr $C$DW$1053, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1054	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1054, DW_AT_name("CSC2")
	.dwattr $C$DW$1054, DW_AT_TI_symbol_name("_CSC2")
	.dwattr $C$DW$1054, DW_AT_data_member_location[DW_OP_plus_uconst 0x6c]
	.dwattr $C$DW$1054, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1055	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1055, DW_AT_name("CSC_OFFSET")
	.dwattr $C$DW$1055, DW_AT_TI_symbol_name("_CSC_OFFSET")
	.dwattr $C$DW$1055, DW_AT_data_member_location[DW_OP_plus_uconst 0x70]
	.dwattr $C$DW$1055, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1056	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1056, DW_AT_name("CNT_BRT")
	.dwattr $C$DW$1056, DW_AT_TI_symbol_name("_CNT_BRT")
	.dwattr $C$DW$1056, DW_AT_data_member_location[DW_OP_plus_uconst 0x74]
	.dwattr $C$DW$1056, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1057	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1057, DW_AT_name("CSUP")
	.dwattr $C$DW$1057, DW_AT_TI_symbol_name("_CSUP")
	.dwattr $C$DW$1057, DW_AT_data_member_location[DW_OP_plus_uconst 0x78]
	.dwattr $C$DW$1057, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1058	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1058, DW_AT_name("SETUP_YC")
	.dwattr $C$DW$1058, DW_AT_TI_symbol_name("_SETUP_YC")
	.dwattr $C$DW$1058, DW_AT_data_member_location[DW_OP_plus_uconst 0x7c]
	.dwattr $C$DW$1058, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1059	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1059, DW_AT_name("SET_TBL_ADDRESS")
	.dwattr $C$DW$1059, DW_AT_TI_symbol_name("_SET_TBL_ADDRESS")
	.dwattr $C$DW$1059, DW_AT_data_member_location[DW_OP_plus_uconst 0x80]
	.dwattr $C$DW$1059, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1060	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1060, DW_AT_name("SET_TBL_DATA")
	.dwattr $C$DW$1060, DW_AT_TI_symbol_name("_SET_TBL_DATA")
	.dwattr $C$DW$1060, DW_AT_data_member_location[DW_OP_plus_uconst 0x84]
	.dwattr $C$DW$1060, DW_AT_accessibility(DW_ACCESS_public)
	.dwendtag $C$DW$T$115

$C$DW$T$279	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_PrevRegs")
	.dwattr $C$DW$T$279, DW_AT_type(*$C$DW$T$115)
	.dwattr $C$DW$T$279, DW_AT_language(DW_LANG_C)
$C$DW$T$280	.dwtag  DW_TAG_volatile_type
	.dwattr $C$DW$T$280, DW_AT_type(*$C$DW$T$279)
$C$DW$T$281	.dwtag  DW_TAG_pointer_type, DW_AT_type(*$C$DW$T$280)
	.dwattr $C$DW$T$281, DW_AT_address_class(0x20)
$C$DW$T$282	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_PrevRegsOvly")
	.dwattr $C$DW$T$282, DW_AT_type(*$C$DW$T$281)
	.dwattr $C$DW$T$282, DW_AT_language(DW_LANG_C)

$C$DW$T$116	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$116, DW_AT_byte_size(0xac)
$C$DW$1061	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1061, DW_AT_name("PID")
	.dwattr $C$DW$1061, DW_AT_TI_symbol_name("_PID")
	.dwattr $C$DW$1061, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1061, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1062	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1062, DW_AT_name("PCR")
	.dwattr $C$DW$1062, DW_AT_TI_symbol_name("_PCR")
	.dwattr $C$DW$1062, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$1062, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1063	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1063, DW_AT_name("RSZ_CNT")
	.dwattr $C$DW$1063, DW_AT_TI_symbol_name("_RSZ_CNT")
	.dwattr $C$DW$1063, DW_AT_data_member_location[DW_OP_plus_uconst 0x8]
	.dwattr $C$DW$1063, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1064	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1064, DW_AT_name("OUT_SIZE")
	.dwattr $C$DW$1064, DW_AT_TI_symbol_name("_OUT_SIZE")
	.dwattr $C$DW$1064, DW_AT_data_member_location[DW_OP_plus_uconst 0xc]
	.dwattr $C$DW$1064, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1065	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1065, DW_AT_name("IN_START")
	.dwattr $C$DW$1065, DW_AT_TI_symbol_name("_IN_START")
	.dwattr $C$DW$1065, DW_AT_data_member_location[DW_OP_plus_uconst 0x10]
	.dwattr $C$DW$1065, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1066	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1066, DW_AT_name("IN_SIZE")
	.dwattr $C$DW$1066, DW_AT_TI_symbol_name("_IN_SIZE")
	.dwattr $C$DW$1066, DW_AT_data_member_location[DW_OP_plus_uconst 0x14]
	.dwattr $C$DW$1066, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1067	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1067, DW_AT_name("SDR_INADD")
	.dwattr $C$DW$1067, DW_AT_TI_symbol_name("_SDR_INADD")
	.dwattr $C$DW$1067, DW_AT_data_member_location[DW_OP_plus_uconst 0x18]
	.dwattr $C$DW$1067, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1068	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1068, DW_AT_name("SDR_INOFF")
	.dwattr $C$DW$1068, DW_AT_TI_symbol_name("_SDR_INOFF")
	.dwattr $C$DW$1068, DW_AT_data_member_location[DW_OP_plus_uconst 0x1c]
	.dwattr $C$DW$1068, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1069	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1069, DW_AT_name("SDR_OUTADD")
	.dwattr $C$DW$1069, DW_AT_TI_symbol_name("_SDR_OUTADD")
	.dwattr $C$DW$1069, DW_AT_data_member_location[DW_OP_plus_uconst 0x20]
	.dwattr $C$DW$1069, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1070	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1070, DW_AT_name("SDR_OUTOFF")
	.dwattr $C$DW$1070, DW_AT_TI_symbol_name("_SDR_OUTOFF")
	.dwattr $C$DW$1070, DW_AT_data_member_location[DW_OP_plus_uconst 0x24]
	.dwattr $C$DW$1070, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1071	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1071, DW_AT_name("HFILT10")
	.dwattr $C$DW$1071, DW_AT_TI_symbol_name("_HFILT10")
	.dwattr $C$DW$1071, DW_AT_data_member_location[DW_OP_plus_uconst 0x28]
	.dwattr $C$DW$1071, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1072	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1072, DW_AT_name("HFILT32")
	.dwattr $C$DW$1072, DW_AT_TI_symbol_name("_HFILT32")
	.dwattr $C$DW$1072, DW_AT_data_member_location[DW_OP_plus_uconst 0x2c]
	.dwattr $C$DW$1072, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1073	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1073, DW_AT_name("HFILT54")
	.dwattr $C$DW$1073, DW_AT_TI_symbol_name("_HFILT54")
	.dwattr $C$DW$1073, DW_AT_data_member_location[DW_OP_plus_uconst 0x30]
	.dwattr $C$DW$1073, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1074	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1074, DW_AT_name("HFILT76")
	.dwattr $C$DW$1074, DW_AT_TI_symbol_name("_HFILT76")
	.dwattr $C$DW$1074, DW_AT_data_member_location[DW_OP_plus_uconst 0x34]
	.dwattr $C$DW$1074, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1075	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1075, DW_AT_name("HFILT98")
	.dwattr $C$DW$1075, DW_AT_TI_symbol_name("_HFILT98")
	.dwattr $C$DW$1075, DW_AT_data_member_location[DW_OP_plus_uconst 0x38]
	.dwattr $C$DW$1075, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1076	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1076, DW_AT_name("HFILT1110")
	.dwattr $C$DW$1076, DW_AT_TI_symbol_name("_HFILT1110")
	.dwattr $C$DW$1076, DW_AT_data_member_location[DW_OP_plus_uconst 0x3c]
	.dwattr $C$DW$1076, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1077	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1077, DW_AT_name("HFILT1312")
	.dwattr $C$DW$1077, DW_AT_TI_symbol_name("_HFILT1312")
	.dwattr $C$DW$1077, DW_AT_data_member_location[DW_OP_plus_uconst 0x40]
	.dwattr $C$DW$1077, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1078	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1078, DW_AT_name("HFILT1514")
	.dwattr $C$DW$1078, DW_AT_TI_symbol_name("_HFILT1514")
	.dwattr $C$DW$1078, DW_AT_data_member_location[DW_OP_plus_uconst 0x44]
	.dwattr $C$DW$1078, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1079	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1079, DW_AT_name("HFILT1716")
	.dwattr $C$DW$1079, DW_AT_TI_symbol_name("_HFILT1716")
	.dwattr $C$DW$1079, DW_AT_data_member_location[DW_OP_plus_uconst 0x48]
	.dwattr $C$DW$1079, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1080	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1080, DW_AT_name("HFILT1918")
	.dwattr $C$DW$1080, DW_AT_TI_symbol_name("_HFILT1918")
	.dwattr $C$DW$1080, DW_AT_data_member_location[DW_OP_plus_uconst 0x4c]
	.dwattr $C$DW$1080, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1081	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1081, DW_AT_name("HFILT2120")
	.dwattr $C$DW$1081, DW_AT_TI_symbol_name("_HFILT2120")
	.dwattr $C$DW$1081, DW_AT_data_member_location[DW_OP_plus_uconst 0x50]
	.dwattr $C$DW$1081, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1082	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1082, DW_AT_name("HFILT2322")
	.dwattr $C$DW$1082, DW_AT_TI_symbol_name("_HFILT2322")
	.dwattr $C$DW$1082, DW_AT_data_member_location[DW_OP_plus_uconst 0x54]
	.dwattr $C$DW$1082, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1083	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1083, DW_AT_name("HFILT2524")
	.dwattr $C$DW$1083, DW_AT_TI_symbol_name("_HFILT2524")
	.dwattr $C$DW$1083, DW_AT_data_member_location[DW_OP_plus_uconst 0x58]
	.dwattr $C$DW$1083, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1084	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1084, DW_AT_name("HFILT2726")
	.dwattr $C$DW$1084, DW_AT_TI_symbol_name("_HFILT2726")
	.dwattr $C$DW$1084, DW_AT_data_member_location[DW_OP_plus_uconst 0x5c]
	.dwattr $C$DW$1084, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1085	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1085, DW_AT_name("HFILT2928")
	.dwattr $C$DW$1085, DW_AT_TI_symbol_name("_HFILT2928")
	.dwattr $C$DW$1085, DW_AT_data_member_location[DW_OP_plus_uconst 0x60]
	.dwattr $C$DW$1085, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1086	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1086, DW_AT_name("HFILT3130")
	.dwattr $C$DW$1086, DW_AT_TI_symbol_name("_HFILT3130")
	.dwattr $C$DW$1086, DW_AT_data_member_location[DW_OP_plus_uconst 0x64]
	.dwattr $C$DW$1086, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1087	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1087, DW_AT_name("VFILT10")
	.dwattr $C$DW$1087, DW_AT_TI_symbol_name("_VFILT10")
	.dwattr $C$DW$1087, DW_AT_data_member_location[DW_OP_plus_uconst 0x68]
	.dwattr $C$DW$1087, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1088	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1088, DW_AT_name("VFILT32")
	.dwattr $C$DW$1088, DW_AT_TI_symbol_name("_VFILT32")
	.dwattr $C$DW$1088, DW_AT_data_member_location[DW_OP_plus_uconst 0x6c]
	.dwattr $C$DW$1088, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1089	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1089, DW_AT_name("VFILT54")
	.dwattr $C$DW$1089, DW_AT_TI_symbol_name("_VFILT54")
	.dwattr $C$DW$1089, DW_AT_data_member_location[DW_OP_plus_uconst 0x70]
	.dwattr $C$DW$1089, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1090	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1090, DW_AT_name("VFILT76")
	.dwattr $C$DW$1090, DW_AT_TI_symbol_name("_VFILT76")
	.dwattr $C$DW$1090, DW_AT_data_member_location[DW_OP_plus_uconst 0x74]
	.dwattr $C$DW$1090, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1091	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1091, DW_AT_name("VFILT98")
	.dwattr $C$DW$1091, DW_AT_TI_symbol_name("_VFILT98")
	.dwattr $C$DW$1091, DW_AT_data_member_location[DW_OP_plus_uconst 0x78]
	.dwattr $C$DW$1091, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1092	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1092, DW_AT_name("VFILT1110")
	.dwattr $C$DW$1092, DW_AT_TI_symbol_name("_VFILT1110")
	.dwattr $C$DW$1092, DW_AT_data_member_location[DW_OP_plus_uconst 0x7c]
	.dwattr $C$DW$1092, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1093	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1093, DW_AT_name("VFILT1312")
	.dwattr $C$DW$1093, DW_AT_TI_symbol_name("_VFILT1312")
	.dwattr $C$DW$1093, DW_AT_data_member_location[DW_OP_plus_uconst 0x80]
	.dwattr $C$DW$1093, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1094	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1094, DW_AT_name("VFILT1514")
	.dwattr $C$DW$1094, DW_AT_TI_symbol_name("_VFILT1514")
	.dwattr $C$DW$1094, DW_AT_data_member_location[DW_OP_plus_uconst 0x84]
	.dwattr $C$DW$1094, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1095	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1095, DW_AT_name("VFILT1716")
	.dwattr $C$DW$1095, DW_AT_TI_symbol_name("_VFILT1716")
	.dwattr $C$DW$1095, DW_AT_data_member_location[DW_OP_plus_uconst 0x88]
	.dwattr $C$DW$1095, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1096	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1096, DW_AT_name("VFILT1918")
	.dwattr $C$DW$1096, DW_AT_TI_symbol_name("_VFILT1918")
	.dwattr $C$DW$1096, DW_AT_data_member_location[DW_OP_plus_uconst 0x8c]
	.dwattr $C$DW$1096, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1097	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1097, DW_AT_name("VFILT2120")
	.dwattr $C$DW$1097, DW_AT_TI_symbol_name("_VFILT2120")
	.dwattr $C$DW$1097, DW_AT_data_member_location[DW_OP_plus_uconst 0x90]
	.dwattr $C$DW$1097, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1098	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1098, DW_AT_name("VFILT2322")
	.dwattr $C$DW$1098, DW_AT_TI_symbol_name("_VFILT2322")
	.dwattr $C$DW$1098, DW_AT_data_member_location[DW_OP_plus_uconst 0x94]
	.dwattr $C$DW$1098, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1099	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1099, DW_AT_name("VFILT2524")
	.dwattr $C$DW$1099, DW_AT_TI_symbol_name("_VFILT2524")
	.dwattr $C$DW$1099, DW_AT_data_member_location[DW_OP_plus_uconst 0x98]
	.dwattr $C$DW$1099, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1100	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1100, DW_AT_name("VFILT2726")
	.dwattr $C$DW$1100, DW_AT_TI_symbol_name("_VFILT2726")
	.dwattr $C$DW$1100, DW_AT_data_member_location[DW_OP_plus_uconst 0x9c]
	.dwattr $C$DW$1100, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1101	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1101, DW_AT_name("VFILT2928")
	.dwattr $C$DW$1101, DW_AT_TI_symbol_name("_VFILT2928")
	.dwattr $C$DW$1101, DW_AT_data_member_location[DW_OP_plus_uconst 0xa0]
	.dwattr $C$DW$1101, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1102	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1102, DW_AT_name("VFILT3130")
	.dwattr $C$DW$1102, DW_AT_TI_symbol_name("_VFILT3130")
	.dwattr $C$DW$1102, DW_AT_data_member_location[DW_OP_plus_uconst 0xa4]
	.dwattr $C$DW$1102, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1103	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1103, DW_AT_name("YENH")
	.dwattr $C$DW$1103, DW_AT_TI_symbol_name("_YENH")
	.dwattr $C$DW$1103, DW_AT_data_member_location[DW_OP_plus_uconst 0xa8]
	.dwattr $C$DW$1103, DW_AT_accessibility(DW_ACCESS_public)
	.dwendtag $C$DW$T$116

$C$DW$T$283	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_ReszRegs")
	.dwattr $C$DW$T$283, DW_AT_type(*$C$DW$T$116)
	.dwattr $C$DW$T$283, DW_AT_language(DW_LANG_C)
$C$DW$T$284	.dwtag  DW_TAG_volatile_type
	.dwattr $C$DW$T$284, DW_AT_type(*$C$DW$T$283)
$C$DW$T$285	.dwtag  DW_TAG_pointer_type, DW_AT_type(*$C$DW$T$284)
	.dwattr $C$DW$T$285, DW_AT_address_class(0x20)
$C$DW$T$286	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_ReszRegsOvly")
	.dwattr $C$DW$T$286, DW_AT_type(*$C$DW$T$285)
	.dwattr $C$DW$T$286, DW_AT_language(DW_LANG_C)

$C$DW$T$117	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$117, DW_AT_byte_size(0x158)
$C$DW$1104	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1104, DW_AT_name("VMOD")
	.dwattr $C$DW$1104, DW_AT_TI_symbol_name("_VMOD")
	.dwattr $C$DW$1104, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1104, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1105	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1105, DW_AT_name("VIDCTL")
	.dwattr $C$DW$1105, DW_AT_TI_symbol_name("_VIDCTL")
	.dwattr $C$DW$1105, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$1105, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1106	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1106, DW_AT_name("VDPRO")
	.dwattr $C$DW$1106, DW_AT_TI_symbol_name("_VDPRO")
	.dwattr $C$DW$1106, DW_AT_data_member_location[DW_OP_plus_uconst 0x8]
	.dwattr $C$DW$1106, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1107	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1107, DW_AT_name("SYNCCTL")
	.dwattr $C$DW$1107, DW_AT_TI_symbol_name("_SYNCCTL")
	.dwattr $C$DW$1107, DW_AT_data_member_location[DW_OP_plus_uconst 0xc]
	.dwattr $C$DW$1107, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1108	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1108, DW_AT_name("HSPLS")
	.dwattr $C$DW$1108, DW_AT_TI_symbol_name("_HSPLS")
	.dwattr $C$DW$1108, DW_AT_data_member_location[DW_OP_plus_uconst 0x10]
	.dwattr $C$DW$1108, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1109	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1109, DW_AT_name("VSPLS")
	.dwattr $C$DW$1109, DW_AT_TI_symbol_name("_VSPLS")
	.dwattr $C$DW$1109, DW_AT_data_member_location[DW_OP_plus_uconst 0x14]
	.dwattr $C$DW$1109, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1110	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1110, DW_AT_name("HINT")
	.dwattr $C$DW$1110, DW_AT_TI_symbol_name("_HINT")
	.dwattr $C$DW$1110, DW_AT_data_member_location[DW_OP_plus_uconst 0x18]
	.dwattr $C$DW$1110, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1111	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1111, DW_AT_name("HSTART")
	.dwattr $C$DW$1111, DW_AT_TI_symbol_name("_HSTART")
	.dwattr $C$DW$1111, DW_AT_data_member_location[DW_OP_plus_uconst 0x1c]
	.dwattr $C$DW$1111, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1112	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1112, DW_AT_name("HVALID")
	.dwattr $C$DW$1112, DW_AT_TI_symbol_name("_HVALID")
	.dwattr $C$DW$1112, DW_AT_data_member_location[DW_OP_plus_uconst 0x20]
	.dwattr $C$DW$1112, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1113	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1113, DW_AT_name("VINT")
	.dwattr $C$DW$1113, DW_AT_TI_symbol_name("_VINT")
	.dwattr $C$DW$1113, DW_AT_data_member_location[DW_OP_plus_uconst 0x24]
	.dwattr $C$DW$1113, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1114	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1114, DW_AT_name("VSTART")
	.dwattr $C$DW$1114, DW_AT_TI_symbol_name("_VSTART")
	.dwattr $C$DW$1114, DW_AT_data_member_location[DW_OP_plus_uconst 0x28]
	.dwattr $C$DW$1114, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1115	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1115, DW_AT_name("VVALID")
	.dwattr $C$DW$1115, DW_AT_TI_symbol_name("_VVALID")
	.dwattr $C$DW$1115, DW_AT_data_member_location[DW_OP_plus_uconst 0x2c]
	.dwattr $C$DW$1115, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1116	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1116, DW_AT_name("HSDLY")
	.dwattr $C$DW$1116, DW_AT_TI_symbol_name("_HSDLY")
	.dwattr $C$DW$1116, DW_AT_data_member_location[DW_OP_plus_uconst 0x30]
	.dwattr $C$DW$1116, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1117	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1117, DW_AT_name("VSDLY")
	.dwattr $C$DW$1117, DW_AT_TI_symbol_name("_VSDLY")
	.dwattr $C$DW$1117, DW_AT_data_member_location[DW_OP_plus_uconst 0x34]
	.dwattr $C$DW$1117, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1118	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1118, DW_AT_name("YCCCTL")
	.dwattr $C$DW$1118, DW_AT_TI_symbol_name("_YCCCTL")
	.dwattr $C$DW$1118, DW_AT_data_member_location[DW_OP_plus_uconst 0x38]
	.dwattr $C$DW$1118, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1119	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1119, DW_AT_name("RGBCTL")
	.dwattr $C$DW$1119, DW_AT_TI_symbol_name("_RGBCTL")
	.dwattr $C$DW$1119, DW_AT_data_member_location[DW_OP_plus_uconst 0x3c]
	.dwattr $C$DW$1119, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1120	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1120, DW_AT_name("RGBCLP")
	.dwattr $C$DW$1120, DW_AT_TI_symbol_name("_RGBCLP")
	.dwattr $C$DW$1120, DW_AT_data_member_location[DW_OP_plus_uconst 0x40]
	.dwattr $C$DW$1120, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1121	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1121, DW_AT_name("LINECTL")
	.dwattr $C$DW$1121, DW_AT_TI_symbol_name("_LINECTL")
	.dwattr $C$DW$1121, DW_AT_data_member_location[DW_OP_plus_uconst 0x44]
	.dwattr $C$DW$1121, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1122	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1122, DW_AT_name("CULLLINE")
	.dwattr $C$DW$1122, DW_AT_TI_symbol_name("_CULLLINE")
	.dwattr $C$DW$1122, DW_AT_data_member_location[DW_OP_plus_uconst 0x48]
	.dwattr $C$DW$1122, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1123	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1123, DW_AT_name("LCDOUT")
	.dwattr $C$DW$1123, DW_AT_TI_symbol_name("_LCDOUT")
	.dwattr $C$DW$1123, DW_AT_data_member_location[DW_OP_plus_uconst 0x4c]
	.dwattr $C$DW$1123, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1124	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1124, DW_AT_name("BRTS")
	.dwattr $C$DW$1124, DW_AT_TI_symbol_name("_BRTS")
	.dwattr $C$DW$1124, DW_AT_data_member_location[DW_OP_plus_uconst 0x50]
	.dwattr $C$DW$1124, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1125	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1125, DW_AT_name("BRTW")
	.dwattr $C$DW$1125, DW_AT_TI_symbol_name("_BRTW")
	.dwattr $C$DW$1125, DW_AT_data_member_location[DW_OP_plus_uconst 0x54]
	.dwattr $C$DW$1125, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1126	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1126, DW_AT_name("ACCTL")
	.dwattr $C$DW$1126, DW_AT_TI_symbol_name("_ACCTL")
	.dwattr $C$DW$1126, DW_AT_data_member_location[DW_OP_plus_uconst 0x58]
	.dwattr $C$DW$1126, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1127	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1127, DW_AT_name("PWMP")
	.dwattr $C$DW$1127, DW_AT_TI_symbol_name("_PWMP")
	.dwattr $C$DW$1127, DW_AT_data_member_location[DW_OP_plus_uconst 0x5c]
	.dwattr $C$DW$1127, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1128	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1128, DW_AT_name("PWMW")
	.dwattr $C$DW$1128, DW_AT_TI_symbol_name("_PWMW")
	.dwattr $C$DW$1128, DW_AT_data_member_location[DW_OP_plus_uconst 0x60]
	.dwattr $C$DW$1128, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1129	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1129, DW_AT_name("DCLKCTL")
	.dwattr $C$DW$1129, DW_AT_TI_symbol_name("_DCLKCTL")
	.dwattr $C$DW$1129, DW_AT_data_member_location[DW_OP_plus_uconst 0x64]
	.dwattr $C$DW$1129, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1130	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1130, DW_AT_name("DCLKPTN0")
	.dwattr $C$DW$1130, DW_AT_TI_symbol_name("_DCLKPTN0")
	.dwattr $C$DW$1130, DW_AT_data_member_location[DW_OP_plus_uconst 0x68]
	.dwattr $C$DW$1130, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1131	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1131, DW_AT_name("DCLKPTN1")
	.dwattr $C$DW$1131, DW_AT_TI_symbol_name("_DCLKPTN1")
	.dwattr $C$DW$1131, DW_AT_data_member_location[DW_OP_plus_uconst 0x6c]
	.dwattr $C$DW$1131, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1132	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1132, DW_AT_name("DCLKPTN2")
	.dwattr $C$DW$1132, DW_AT_TI_symbol_name("_DCLKPTN2")
	.dwattr $C$DW$1132, DW_AT_data_member_location[DW_OP_plus_uconst 0x70]
	.dwattr $C$DW$1132, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1133	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1133, DW_AT_name("DCLKPTN3")
	.dwattr $C$DW$1133, DW_AT_TI_symbol_name("_DCLKPTN3")
	.dwattr $C$DW$1133, DW_AT_data_member_location[DW_OP_plus_uconst 0x74]
	.dwattr $C$DW$1133, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1134	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1134, DW_AT_name("DCLKPTN0A")
	.dwattr $C$DW$1134, DW_AT_TI_symbol_name("_DCLKPTN0A")
	.dwattr $C$DW$1134, DW_AT_data_member_location[DW_OP_plus_uconst 0x78]
	.dwattr $C$DW$1134, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1135	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1135, DW_AT_name("DCLKPTN1A")
	.dwattr $C$DW$1135, DW_AT_TI_symbol_name("_DCLKPTN1A")
	.dwattr $C$DW$1135, DW_AT_data_member_location[DW_OP_plus_uconst 0x7c]
	.dwattr $C$DW$1135, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1136	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1136, DW_AT_name("DCLKPTN2A")
	.dwattr $C$DW$1136, DW_AT_TI_symbol_name("_DCLKPTN2A")
	.dwattr $C$DW$1136, DW_AT_data_member_location[DW_OP_plus_uconst 0x80]
	.dwattr $C$DW$1136, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1137	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1137, DW_AT_name("DCLKPTN3A")
	.dwattr $C$DW$1137, DW_AT_TI_symbol_name("_DCLKPTN3A")
	.dwattr $C$DW$1137, DW_AT_data_member_location[DW_OP_plus_uconst 0x84]
	.dwattr $C$DW$1137, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1138	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1138, DW_AT_name("DCLKHS")
	.dwattr $C$DW$1138, DW_AT_TI_symbol_name("_DCLKHS")
	.dwattr $C$DW$1138, DW_AT_data_member_location[DW_OP_plus_uconst 0x88]
	.dwattr $C$DW$1138, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1139	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1139, DW_AT_name("DCLKHSA")
	.dwattr $C$DW$1139, DW_AT_TI_symbol_name("_DCLKHSA")
	.dwattr $C$DW$1139, DW_AT_data_member_location[DW_OP_plus_uconst 0x8c]
	.dwattr $C$DW$1139, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1140	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1140, DW_AT_name("DCLKHR")
	.dwattr $C$DW$1140, DW_AT_TI_symbol_name("_DCLKHR")
	.dwattr $C$DW$1140, DW_AT_data_member_location[DW_OP_plus_uconst 0x90]
	.dwattr $C$DW$1140, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1141	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1141, DW_AT_name("DCLKVS")
	.dwattr $C$DW$1141, DW_AT_TI_symbol_name("_DCLKVS")
	.dwattr $C$DW$1141, DW_AT_data_member_location[DW_OP_plus_uconst 0x94]
	.dwattr $C$DW$1141, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1142	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1142, DW_AT_name("DCLKVR")
	.dwattr $C$DW$1142, DW_AT_TI_symbol_name("_DCLKVR")
	.dwattr $C$DW$1142, DW_AT_data_member_location[DW_OP_plus_uconst 0x98]
	.dwattr $C$DW$1142, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1143	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1143, DW_AT_name("CAPCTL")
	.dwattr $C$DW$1143, DW_AT_TI_symbol_name("_CAPCTL")
	.dwattr $C$DW$1143, DW_AT_data_member_location[DW_OP_plus_uconst 0x9c]
	.dwattr $C$DW$1143, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1144	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1144, DW_AT_name("CAPDO")
	.dwattr $C$DW$1144, DW_AT_TI_symbol_name("_CAPDO")
	.dwattr $C$DW$1144, DW_AT_data_member_location[DW_OP_plus_uconst 0xa0]
	.dwattr $C$DW$1144, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1145	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1145, DW_AT_name("CAPDE")
	.dwattr $C$DW$1145, DW_AT_TI_symbol_name("_CAPDE")
	.dwattr $C$DW$1145, DW_AT_data_member_location[DW_OP_plus_uconst 0xa4]
	.dwattr $C$DW$1145, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1146	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1146, DW_AT_name("ATR0")
	.dwattr $C$DW$1146, DW_AT_TI_symbol_name("_ATR0")
	.dwattr $C$DW$1146, DW_AT_data_member_location[DW_OP_plus_uconst 0xa8]
	.dwattr $C$DW$1146, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1147	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1147, DW_AT_name("ATR1")
	.dwattr $C$DW$1147, DW_AT_TI_symbol_name("_ATR1")
	.dwattr $C$DW$1147, DW_AT_data_member_location[DW_OP_plus_uconst 0xac]
	.dwattr $C$DW$1147, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1148	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1148, DW_AT_name("ATR2")
	.dwattr $C$DW$1148, DW_AT_TI_symbol_name("_ATR2")
	.dwattr $C$DW$1148, DW_AT_data_member_location[DW_OP_plus_uconst 0xb0]
	.dwattr $C$DW$1148, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1149	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1149, DW_AT_name("EPSON_LCDCTL")
	.dwattr $C$DW$1149, DW_AT_TI_symbol_name("_EPSON_LCDCTL")
	.dwattr $C$DW$1149, DW_AT_data_member_location[DW_OP_plus_uconst 0xb4]
	.dwattr $C$DW$1149, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1150	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1150, DW_AT_name("VSTAT")
	.dwattr $C$DW$1150, DW_AT_TI_symbol_name("_VSTAT")
	.dwattr $C$DW$1150, DW_AT_data_member_location[DW_OP_plus_uconst 0xb8]
	.dwattr $C$DW$1150, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1151	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1151, DW_AT_name("RAMADR")
	.dwattr $C$DW$1151, DW_AT_TI_symbol_name("_RAMADR")
	.dwattr $C$DW$1151, DW_AT_data_member_location[DW_OP_plus_uconst 0xbc]
	.dwattr $C$DW$1151, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1152	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1152, DW_AT_name("RAMPORT")
	.dwattr $C$DW$1152, DW_AT_TI_symbol_name("_RAMPORT")
	.dwattr $C$DW$1152, DW_AT_data_member_location[DW_OP_plus_uconst 0xc0]
	.dwattr $C$DW$1152, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1153	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1153, DW_AT_name("DACTST")
	.dwattr $C$DW$1153, DW_AT_TI_symbol_name("_DACTST")
	.dwattr $C$DW$1153, DW_AT_data_member_location[DW_OP_plus_uconst 0xc4]
	.dwattr $C$DW$1153, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1154	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1154, DW_AT_name("YCOLVL")
	.dwattr $C$DW$1154, DW_AT_TI_symbol_name("_YCOLVL")
	.dwattr $C$DW$1154, DW_AT_data_member_location[DW_OP_plus_uconst 0xc8]
	.dwattr $C$DW$1154, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1155	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1155, DW_AT_name("SCPROG")
	.dwattr $C$DW$1155, DW_AT_TI_symbol_name("_SCPROG")
	.dwattr $C$DW$1155, DW_AT_data_member_location[DW_OP_plus_uconst 0xcc]
	.dwattr $C$DW$1155, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1156	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1156, DW_AT_name("RSV1")
	.dwattr $C$DW$1156, DW_AT_TI_symbol_name("_RSV1")
	.dwattr $C$DW$1156, DW_AT_data_member_location[DW_OP_plus_uconst 0xd0]
	.dwattr $C$DW$1156, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1157	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1157, DW_AT_name("RSV2")
	.dwattr $C$DW$1157, DW_AT_TI_symbol_name("_RSV2")
	.dwattr $C$DW$1157, DW_AT_data_member_location[DW_OP_plus_uconst 0xd4]
	.dwattr $C$DW$1157, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1158	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1158, DW_AT_name("RSV3")
	.dwattr $C$DW$1158, DW_AT_TI_symbol_name("_RSV3")
	.dwattr $C$DW$1158, DW_AT_data_member_location[DW_OP_plus_uconst 0xd8]
	.dwattr $C$DW$1158, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1159	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1159, DW_AT_name("CVBS")
	.dwattr $C$DW$1159, DW_AT_TI_symbol_name("_CVBS")
	.dwattr $C$DW$1159, DW_AT_data_member_location[DW_OP_plus_uconst 0xdc]
	.dwattr $C$DW$1159, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1160	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1160, DW_AT_name("CMPNT")
	.dwattr $C$DW$1160, DW_AT_TI_symbol_name("_CMPNT")
	.dwattr $C$DW$1160, DW_AT_data_member_location[DW_OP_plus_uconst 0xe0]
	.dwattr $C$DW$1160, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1161	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1161, DW_AT_name("ETMG0")
	.dwattr $C$DW$1161, DW_AT_TI_symbol_name("_ETMG0")
	.dwattr $C$DW$1161, DW_AT_data_member_location[DW_OP_plus_uconst 0xe4]
	.dwattr $C$DW$1161, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1162	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1162, DW_AT_name("ETMG1")
	.dwattr $C$DW$1162, DW_AT_TI_symbol_name("_ETMG1")
	.dwattr $C$DW$1162, DW_AT_data_member_location[DW_OP_plus_uconst 0xe8]
	.dwattr $C$DW$1162, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1163	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1163, DW_AT_name("ETMG2")
	.dwattr $C$DW$1163, DW_AT_TI_symbol_name("_ETMG2")
	.dwattr $C$DW$1163, DW_AT_data_member_location[DW_OP_plus_uconst 0xec]
	.dwattr $C$DW$1163, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1164	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1164, DW_AT_name("ETMG3")
	.dwattr $C$DW$1164, DW_AT_TI_symbol_name("_ETMG3")
	.dwattr $C$DW$1164, DW_AT_data_member_location[DW_OP_plus_uconst 0xf0]
	.dwattr $C$DW$1164, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1165	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1165, DW_AT_name("DACSEL")
	.dwattr $C$DW$1165, DW_AT_TI_symbol_name("_DACSEL")
	.dwattr $C$DW$1165, DW_AT_data_member_location[DW_OP_plus_uconst 0xf4]
	.dwattr $C$DW$1165, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1166	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1166, DW_AT_name("RSV4")
	.dwattr $C$DW$1166, DW_AT_TI_symbol_name("_RSV4")
	.dwattr $C$DW$1166, DW_AT_data_member_location[DW_OP_plus_uconst 0xf8]
	.dwattr $C$DW$1166, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1167	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1167, DW_AT_name("RSV5")
	.dwattr $C$DW$1167, DW_AT_TI_symbol_name("_RSV5")
	.dwattr $C$DW$1167, DW_AT_data_member_location[DW_OP_plus_uconst 0xfc]
	.dwattr $C$DW$1167, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1168	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1168, DW_AT_name("ARGBX0")
	.dwattr $C$DW$1168, DW_AT_TI_symbol_name("_ARGBX0")
	.dwattr $C$DW$1168, DW_AT_data_member_location[DW_OP_plus_uconst 0x100]
	.dwattr $C$DW$1168, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1169	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1169, DW_AT_name("ARGBX1")
	.dwattr $C$DW$1169, DW_AT_TI_symbol_name("_ARGBX1")
	.dwattr $C$DW$1169, DW_AT_data_member_location[DW_OP_plus_uconst 0x104]
	.dwattr $C$DW$1169, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1170	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1170, DW_AT_name("ARGBX2")
	.dwattr $C$DW$1170, DW_AT_TI_symbol_name("_ARGBX2")
	.dwattr $C$DW$1170, DW_AT_data_member_location[DW_OP_plus_uconst 0x108]
	.dwattr $C$DW$1170, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1171	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1171, DW_AT_name("ARGBX3")
	.dwattr $C$DW$1171, DW_AT_TI_symbol_name("_ARGBX3")
	.dwattr $C$DW$1171, DW_AT_data_member_location[DW_OP_plus_uconst 0x10c]
	.dwattr $C$DW$1171, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1172	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1172, DW_AT_name("ARGBX4")
	.dwattr $C$DW$1172, DW_AT_TI_symbol_name("_ARGBX4")
	.dwattr $C$DW$1172, DW_AT_data_member_location[DW_OP_plus_uconst 0x110]
	.dwattr $C$DW$1172, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1173	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1173, DW_AT_name("DRGBX0")
	.dwattr $C$DW$1173, DW_AT_TI_symbol_name("_DRGBX0")
	.dwattr $C$DW$1173, DW_AT_data_member_location[DW_OP_plus_uconst 0x114]
	.dwattr $C$DW$1173, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1174	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1174, DW_AT_name("DRGBX1")
	.dwattr $C$DW$1174, DW_AT_TI_symbol_name("_DRGBX1")
	.dwattr $C$DW$1174, DW_AT_data_member_location[DW_OP_plus_uconst 0x118]
	.dwattr $C$DW$1174, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1175	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1175, DW_AT_name("DRGBX2")
	.dwattr $C$DW$1175, DW_AT_TI_symbol_name("_DRGBX2")
	.dwattr $C$DW$1175, DW_AT_data_member_location[DW_OP_plus_uconst 0x11c]
	.dwattr $C$DW$1175, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1176	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1176, DW_AT_name("DRGBX3")
	.dwattr $C$DW$1176, DW_AT_TI_symbol_name("_DRGBX3")
	.dwattr $C$DW$1176, DW_AT_data_member_location[DW_OP_plus_uconst 0x120]
	.dwattr $C$DW$1176, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1177	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1177, DW_AT_name("DRGBX4")
	.dwattr $C$DW$1177, DW_AT_TI_symbol_name("_DRGBX4")
	.dwattr $C$DW$1177, DW_AT_data_member_location[DW_OP_plus_uconst 0x124]
	.dwattr $C$DW$1177, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1178	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1178, DW_AT_name("VSTARTA")
	.dwattr $C$DW$1178, DW_AT_TI_symbol_name("_VSTARTA")
	.dwattr $C$DW$1178, DW_AT_data_member_location[DW_OP_plus_uconst 0x128]
	.dwattr $C$DW$1178, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1179	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1179, DW_AT_name("OSDCLK0")
	.dwattr $C$DW$1179, DW_AT_TI_symbol_name("_OSDCLK0")
	.dwattr $C$DW$1179, DW_AT_data_member_location[DW_OP_plus_uconst 0x12c]
	.dwattr $C$DW$1179, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1180	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1180, DW_AT_name("OSDCLK1")
	.dwattr $C$DW$1180, DW_AT_TI_symbol_name("_OSDCLK1")
	.dwattr $C$DW$1180, DW_AT_data_member_location[DW_OP_plus_uconst 0x130]
	.dwattr $C$DW$1180, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1181	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1181, DW_AT_name("HVLDCL0")
	.dwattr $C$DW$1181, DW_AT_TI_symbol_name("_HVLDCL0")
	.dwattr $C$DW$1181, DW_AT_data_member_location[DW_OP_plus_uconst 0x134]
	.dwattr $C$DW$1181, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1182	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1182, DW_AT_name("HVLDCL1")
	.dwattr $C$DW$1182, DW_AT_TI_symbol_name("_HVLDCL1")
	.dwattr $C$DW$1182, DW_AT_data_member_location[DW_OP_plus_uconst 0x138]
	.dwattr $C$DW$1182, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1183	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1183, DW_AT_name("OSDHADV")
	.dwattr $C$DW$1183, DW_AT_TI_symbol_name("_OSDHADV")
	.dwattr $C$DW$1183, DW_AT_data_member_location[DW_OP_plus_uconst 0x13c]
	.dwattr $C$DW$1183, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1184	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1184, DW_AT_name("SCTEST0")
	.dwattr $C$DW$1184, DW_AT_TI_symbol_name("_SCTEST0")
	.dwattr $C$DW$1184, DW_AT_data_member_location[DW_OP_plus_uconst 0x140]
	.dwattr $C$DW$1184, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1185	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1185, DW_AT_name("SCTEST1")
	.dwattr $C$DW$1185, DW_AT_TI_symbol_name("_SCTEST1")
	.dwattr $C$DW$1185, DW_AT_data_member_location[DW_OP_plus_uconst 0x144]
	.dwattr $C$DW$1185, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1186	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1186, DW_AT_name("SCTEST2")
	.dwattr $C$DW$1186, DW_AT_TI_symbol_name("_SCTEST2")
	.dwattr $C$DW$1186, DW_AT_data_member_location[DW_OP_plus_uconst 0x148]
	.dwattr $C$DW$1186, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1187	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1187, DW_AT_name("VTEST0")
	.dwattr $C$DW$1187, DW_AT_TI_symbol_name("_VTEST0")
	.dwattr $C$DW$1187, DW_AT_data_member_location[DW_OP_plus_uconst 0x14c]
	.dwattr $C$DW$1187, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1188	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1188, DW_AT_name("VTEST1")
	.dwattr $C$DW$1188, DW_AT_TI_symbol_name("_VTEST1")
	.dwattr $C$DW$1188, DW_AT_data_member_location[DW_OP_plus_uconst 0x150]
	.dwattr $C$DW$1188, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1189	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1189, DW_AT_name("VTEST2")
	.dwattr $C$DW$1189, DW_AT_TI_symbol_name("_VTEST2")
	.dwattr $C$DW$1189, DW_AT_data_member_location[DW_OP_plus_uconst 0x154]
	.dwattr $C$DW$1189, DW_AT_accessibility(DW_ACCESS_public)
	.dwendtag $C$DW$T$117

$C$DW$T$287	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_VencRegs")
	.dwattr $C$DW$T$287, DW_AT_type(*$C$DW$T$117)
	.dwattr $C$DW$T$287, DW_AT_language(DW_LANG_C)
$C$DW$T$288	.dwtag  DW_TAG_volatile_type
	.dwattr $C$DW$T$288, DW_AT_type(*$C$DW$T$287)
$C$DW$T$289	.dwtag  DW_TAG_pointer_type, DW_AT_type(*$C$DW$T$288)
	.dwattr $C$DW$T$289, DW_AT_address_class(0x20)
$C$DW$T$290	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_VencRegsOvly")
	.dwattr $C$DW$T$290, DW_AT_type(*$C$DW$T$289)
	.dwattr $C$DW$T$290, DW_AT_language(DW_LANG_C)

$C$DW$T$118	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$118, DW_AT_byte_size(0x08)
$C$DW$1190	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1190, DW_AT_name("PID")
	.dwattr $C$DW$1190, DW_AT_TI_symbol_name("_PID")
	.dwattr $C$DW$1190, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1190, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1191	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1191, DW_AT_name("PCR")
	.dwattr $C$DW$1191, DW_AT_TI_symbol_name("_PCR")
	.dwattr $C$DW$1191, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$1191, DW_AT_accessibility(DW_ACCESS_public)
	.dwendtag $C$DW$T$118

$C$DW$T$291	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_VpbeRegs")
	.dwattr $C$DW$T$291, DW_AT_type(*$C$DW$T$118)
	.dwattr $C$DW$T$291, DW_AT_language(DW_LANG_C)
$C$DW$T$292	.dwtag  DW_TAG_volatile_type
	.dwattr $C$DW$T$292, DW_AT_type(*$C$DW$T$291)
$C$DW$T$293	.dwtag  DW_TAG_pointer_type, DW_AT_type(*$C$DW$T$292)
	.dwattr $C$DW$T$293, DW_AT_address_class(0x20)
$C$DW$T$294	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_VpbeRegsOvly")
	.dwattr $C$DW$T$294, DW_AT_type(*$C$DW$T$293)
	.dwattr $C$DW$T$294, DW_AT_language(DW_LANG_C)

$C$DW$T$120	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$120, DW_AT_byte_size(0x88)
$C$DW$1192	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1192, DW_AT_name("VERSION")
	.dwattr $C$DW$1192, DW_AT_TI_symbol_name("_VERSION")
	.dwattr $C$DW$1192, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1192, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1193	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1193, DW_AT_name("CONTROL")
	.dwattr $C$DW$1193, DW_AT_TI_symbol_name("_CONTROL")
	.dwattr $C$DW$1193, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$1193, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1194	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1194, DW_AT_name("ALIVE")
	.dwattr $C$DW$1194, DW_AT_TI_symbol_name("_ALIVE")
	.dwattr $C$DW$1194, DW_AT_data_member_location[DW_OP_plus_uconst 0x8]
	.dwattr $C$DW$1194, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1195	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1195, DW_AT_name("LINK")
	.dwattr $C$DW$1195, DW_AT_TI_symbol_name("_LINK")
	.dwattr $C$DW$1195, DW_AT_data_member_location[DW_OP_plus_uconst 0xc]
	.dwattr $C$DW$1195, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1196	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1196, DW_AT_name("LINKINTRAW")
	.dwattr $C$DW$1196, DW_AT_TI_symbol_name("_LINKINTRAW")
	.dwattr $C$DW$1196, DW_AT_data_member_location[DW_OP_plus_uconst 0x10]
	.dwattr $C$DW$1196, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1197	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1197, DW_AT_name("LINKINTMASKED")
	.dwattr $C$DW$1197, DW_AT_TI_symbol_name("_LINKINTMASKED")
	.dwattr $C$DW$1197, DW_AT_data_member_location[DW_OP_plus_uconst 0x14]
	.dwattr $C$DW$1197, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1198	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$62)
	.dwattr $C$DW$1198, DW_AT_name("RSVD0")
	.dwattr $C$DW$1198, DW_AT_TI_symbol_name("_RSVD0")
	.dwattr $C$DW$1198, DW_AT_data_member_location[DW_OP_plus_uconst 0x18]
	.dwattr $C$DW$1198, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1199	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1199, DW_AT_name("USERINTRAW")
	.dwattr $C$DW$1199, DW_AT_TI_symbol_name("_USERINTRAW")
	.dwattr $C$DW$1199, DW_AT_data_member_location[DW_OP_plus_uconst 0x20]
	.dwattr $C$DW$1199, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1200	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1200, DW_AT_name("USERINTMASKED")
	.dwattr $C$DW$1200, DW_AT_TI_symbol_name("_USERINTMASKED")
	.dwattr $C$DW$1200, DW_AT_data_member_location[DW_OP_plus_uconst 0x24]
	.dwattr $C$DW$1200, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1201	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1201, DW_AT_name("USERINTMASKSET")
	.dwattr $C$DW$1201, DW_AT_TI_symbol_name("_USERINTMASKSET")
	.dwattr $C$DW$1201, DW_AT_data_member_location[DW_OP_plus_uconst 0x28]
	.dwattr $C$DW$1201, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1202	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1202, DW_AT_name("USERINTMASKCLEAR")
	.dwattr $C$DW$1202, DW_AT_TI_symbol_name("_USERINTMASKCLEAR")
	.dwattr $C$DW$1202, DW_AT_data_member_location[DW_OP_plus_uconst 0x2c]
	.dwattr $C$DW$1202, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1203	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$119)
	.dwattr $C$DW$1203, DW_AT_name("RSVD1")
	.dwattr $C$DW$1203, DW_AT_TI_symbol_name("_RSVD1")
	.dwattr $C$DW$1203, DW_AT_data_member_location[DW_OP_plus_uconst 0x30]
	.dwattr $C$DW$1203, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1204	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1204, DW_AT_name("USERACCESS0")
	.dwattr $C$DW$1204, DW_AT_TI_symbol_name("_USERACCESS0")
	.dwattr $C$DW$1204, DW_AT_data_member_location[DW_OP_plus_uconst 0x80]
	.dwattr $C$DW$1204, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1205	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1205, DW_AT_name("USERPHYSEL0")
	.dwattr $C$DW$1205, DW_AT_TI_symbol_name("_USERPHYSEL0")
	.dwattr $C$DW$1205, DW_AT_data_member_location[DW_OP_plus_uconst 0x84]
	.dwattr $C$DW$1205, DW_AT_accessibility(DW_ACCESS_public)
	.dwendtag $C$DW$T$120

$C$DW$T$295	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_MdioRegs")
	.dwattr $C$DW$T$295, DW_AT_type(*$C$DW$T$120)
	.dwattr $C$DW$T$295, DW_AT_language(DW_LANG_C)
$C$DW$T$296	.dwtag  DW_TAG_volatile_type
	.dwattr $C$DW$T$296, DW_AT_type(*$C$DW$T$295)
$C$DW$T$297	.dwtag  DW_TAG_pointer_type, DW_AT_type(*$C$DW$T$296)
	.dwattr $C$DW$T$297, DW_AT_address_class(0x20)
$C$DW$T$298	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_MdioRegsOvly")
	.dwattr $C$DW$T$298, DW_AT_type(*$C$DW$T$297)
	.dwattr $C$DW$T$298, DW_AT_language(DW_LANG_C)

$C$DW$T$128	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$128, DW_AT_byte_size(0xaa4)
$C$DW$1206	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1206, DW_AT_name("PID")
	.dwattr $C$DW$1206, DW_AT_TI_symbol_name("_PID")
	.dwattr $C$DW$1206, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1206, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1207	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$56)
	.dwattr $C$DW$1207, DW_AT_name("RSVD0")
	.dwattr $C$DW$1207, DW_AT_TI_symbol_name("_RSVD0")
	.dwattr $C$DW$1207, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$1207, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1208	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1208, DW_AT_name("GBLCTL")
	.dwattr $C$DW$1208, DW_AT_TI_symbol_name("_GBLCTL")
	.dwattr $C$DW$1208, DW_AT_data_member_location[DW_OP_plus_uconst 0x10]
	.dwattr $C$DW$1208, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1209	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$55)
	.dwattr $C$DW$1209, DW_AT_name("RSVD1")
	.dwattr $C$DW$1209, DW_AT_TI_symbol_name("_RSVD1")
	.dwattr $C$DW$1209, DW_AT_data_member_location[DW_OP_plus_uconst 0x14]
	.dwattr $C$DW$1209, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1210	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1210, DW_AT_name("INTEVAL")
	.dwattr $C$DW$1210, DW_AT_TI_symbol_name("_INTEVAL")
	.dwattr $C$DW$1210, DW_AT_data_member_location[DW_OP_plus_uconst 0x18]
	.dwattr $C$DW$1210, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1211	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$121)
	.dwattr $C$DW$1211, DW_AT_name("RSVD2")
	.dwattr $C$DW$1211, DW_AT_TI_symbol_name("_RSVD2")
	.dwattr $C$DW$1211, DW_AT_data_member_location[DW_OP_plus_uconst 0x1c]
	.dwattr $C$DW$1211, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1212	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1212, DW_AT_name("ERRPR0")
	.dwattr $C$DW$1212, DW_AT_TI_symbol_name("_ERRPR0")
	.dwattr $C$DW$1212, DW_AT_data_member_location[DW_OP_plus_uconst 0x40]
	.dwattr $C$DW$1212, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1213	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1213, DW_AT_name("ERRPR1")
	.dwattr $C$DW$1213, DW_AT_TI_symbol_name("_ERRPR1")
	.dwattr $C$DW$1213, DW_AT_data_member_location[DW_OP_plus_uconst 0x44]
	.dwattr $C$DW$1213, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1214	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$62)
	.dwattr $C$DW$1214, DW_AT_name("RSVD3")
	.dwattr $C$DW$1214, DW_AT_TI_symbol_name("_RSVD3")
	.dwattr $C$DW$1214, DW_AT_data_member_location[DW_OP_plus_uconst 0x48]
	.dwattr $C$DW$1214, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1215	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1215, DW_AT_name("ERRCR0")
	.dwattr $C$DW$1215, DW_AT_TI_symbol_name("_ERRCR0")
	.dwattr $C$DW$1215, DW_AT_data_member_location[DW_OP_plus_uconst 0x50]
	.dwattr $C$DW$1215, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1216	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1216, DW_AT_name("ERRCR1")
	.dwattr $C$DW$1216, DW_AT_TI_symbol_name("_ERRCR1")
	.dwattr $C$DW$1216, DW_AT_data_member_location[DW_OP_plus_uconst 0x54]
	.dwattr $C$DW$1216, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1217	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$62)
	.dwattr $C$DW$1217, DW_AT_name("RSVD4")
	.dwattr $C$DW$1217, DW_AT_TI_symbol_name("_RSVD4")
	.dwattr $C$DW$1217, DW_AT_data_member_location[DW_OP_plus_uconst 0x58]
	.dwattr $C$DW$1217, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1218	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1218, DW_AT_name("PERRPR")
	.dwattr $C$DW$1218, DW_AT_TI_symbol_name("_PERRPR")
	.dwattr $C$DW$1218, DW_AT_data_member_location[DW_OP_plus_uconst 0x60]
	.dwattr $C$DW$1218, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1219	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$55)
	.dwattr $C$DW$1219, DW_AT_name("RSVD5")
	.dwattr $C$DW$1219, DW_AT_TI_symbol_name("_RSVD5")
	.dwattr $C$DW$1219, DW_AT_data_member_location[DW_OP_plus_uconst 0x64]
	.dwattr $C$DW$1219, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1220	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1220, DW_AT_name("PERRCR")
	.dwattr $C$DW$1220, DW_AT_TI_symbol_name("_PERRCR")
	.dwattr $C$DW$1220, DW_AT_data_member_location[DW_OP_plus_uconst 0x68]
	.dwattr $C$DW$1220, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1221	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$55)
	.dwattr $C$DW$1221, DW_AT_name("RSVD6")
	.dwattr $C$DW$1221, DW_AT_TI_symbol_name("_RSVD6")
	.dwattr $C$DW$1221, DW_AT_data_member_location[DW_OP_plus_uconst 0x6c]
	.dwattr $C$DW$1221, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1222	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1222, DW_AT_name("EPCPR")
	.dwattr $C$DW$1222, DW_AT_TI_symbol_name("_EPCPR")
	.dwattr $C$DW$1222, DW_AT_data_member_location[DW_OP_plus_uconst 0x70]
	.dwattr $C$DW$1222, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1223	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$55)
	.dwattr $C$DW$1223, DW_AT_name("RSVD7")
	.dwattr $C$DW$1223, DW_AT_TI_symbol_name("_RSVD7")
	.dwattr $C$DW$1223, DW_AT_data_member_location[DW_OP_plus_uconst 0x74]
	.dwattr $C$DW$1223, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1224	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1224, DW_AT_name("EPCR")
	.dwattr $C$DW$1224, DW_AT_TI_symbol_name("_EPCR")
	.dwattr $C$DW$1224, DW_AT_data_member_location[DW_OP_plus_uconst 0x78]
	.dwattr $C$DW$1224, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1225	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$122)
	.dwattr $C$DW$1225, DW_AT_name("RSVD8")
	.dwattr $C$DW$1225, DW_AT_TI_symbol_name("_RSVD8")
	.dwattr $C$DW$1225, DW_AT_data_member_location[DW_OP_plus_uconst 0x7c]
	.dwattr $C$DW$1225, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1226	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1226, DW_AT_name("RAILSTAT")
	.dwattr $C$DW$1226, DW_AT_TI_symbol_name("_RAILSTAT")
	.dwattr $C$DW$1226, DW_AT_data_member_location[DW_OP_plus_uconst 0x100]
	.dwattr $C$DW$1226, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1227	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1227, DW_AT_name("RAILCTL")
	.dwattr $C$DW$1227, DW_AT_TI_symbol_name("_RAILCTL")
	.dwattr $C$DW$1227, DW_AT_data_member_location[DW_OP_plus_uconst 0x104]
	.dwattr $C$DW$1227, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1228	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1228, DW_AT_name("RAILSEL")
	.dwattr $C$DW$1228, DW_AT_TI_symbol_name("_RAILSEL")
	.dwattr $C$DW$1228, DW_AT_data_member_location[DW_OP_plus_uconst 0x108]
	.dwattr $C$DW$1228, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1229	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$54)
	.dwattr $C$DW$1229, DW_AT_name("RSVD9")
	.dwattr $C$DW$1229, DW_AT_TI_symbol_name("_RSVD9")
	.dwattr $C$DW$1229, DW_AT_data_member_location[DW_OP_plus_uconst 0x10c]
	.dwattr $C$DW$1229, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1230	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1230, DW_AT_name("PTCMD")
	.dwattr $C$DW$1230, DW_AT_TI_symbol_name("_PTCMD")
	.dwattr $C$DW$1230, DW_AT_data_member_location[DW_OP_plus_uconst 0x120]
	.dwattr $C$DW$1230, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1231	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$55)
	.dwattr $C$DW$1231, DW_AT_name("RSVD10")
	.dwattr $C$DW$1231, DW_AT_TI_symbol_name("_RSVD10")
	.dwattr $C$DW$1231, DW_AT_data_member_location[DW_OP_plus_uconst 0x124]
	.dwattr $C$DW$1231, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1232	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1232, DW_AT_name("PTSTAT")
	.dwattr $C$DW$1232, DW_AT_TI_symbol_name("_PTSTAT")
	.dwattr $C$DW$1232, DW_AT_data_member_location[DW_OP_plus_uconst 0x128]
	.dwattr $C$DW$1232, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1233	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$123)
	.dwattr $C$DW$1233, DW_AT_name("RSVD11")
	.dwattr $C$DW$1233, DW_AT_TI_symbol_name("_RSVD11")
	.dwattr $C$DW$1233, DW_AT_data_member_location[DW_OP_plus_uconst 0x12c]
	.dwattr $C$DW$1233, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1234	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$124)
	.dwattr $C$DW$1234, DW_AT_name("PDSTAT")
	.dwattr $C$DW$1234, DW_AT_TI_symbol_name("_PDSTAT")
	.dwattr $C$DW$1234, DW_AT_data_member_location[DW_OP_plus_uconst 0x200]
	.dwattr $C$DW$1234, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1235	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$125)
	.dwattr $C$DW$1235, DW_AT_name("RSVD12")
	.dwattr $C$DW$1235, DW_AT_TI_symbol_name("_RSVD12")
	.dwattr $C$DW$1235, DW_AT_data_member_location[DW_OP_plus_uconst 0x2a4]
	.dwattr $C$DW$1235, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1236	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$124)
	.dwattr $C$DW$1236, DW_AT_name("PDCTL")
	.dwattr $C$DW$1236, DW_AT_TI_symbol_name("_PDCTL")
	.dwattr $C$DW$1236, DW_AT_data_member_location[DW_OP_plus_uconst 0x300]
	.dwattr $C$DW$1236, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1237	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$126)
	.dwattr $C$DW$1237, DW_AT_name("RSVD13")
	.dwattr $C$DW$1237, DW_AT_TI_symbol_name("_RSVD13")
	.dwattr $C$DW$1237, DW_AT_data_member_location[DW_OP_plus_uconst 0x3a4]
	.dwattr $C$DW$1237, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1238	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$21)
	.dwattr $C$DW$1238, DW_AT_name("MRSTOUT")
	.dwattr $C$DW$1238, DW_AT_TI_symbol_name("_MRSTOUT")
	.dwattr $C$DW$1238, DW_AT_data_member_location[DW_OP_plus_uconst 0x500]
	.dwattr $C$DW$1238, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1239	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$62)
	.dwattr $C$DW$1239, DW_AT_name("RSVD14")
	.dwattr $C$DW$1239, DW_AT_TI_symbol_name("_RSVD14")
	.dwattr $C$DW$1239, DW_AT_data_member_location[DW_OP_plus_uconst 0x508]
	.dwattr $C$DW$1239, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1240	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$21)
	.dwattr $C$DW$1240, DW_AT_name("MCKOUT")
	.dwattr $C$DW$1240, DW_AT_TI_symbol_name("_MCKOUT")
	.dwattr $C$DW$1240, DW_AT_data_member_location[DW_OP_plus_uconst 0x510]
	.dwattr $C$DW$1240, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1241	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$127)
	.dwattr $C$DW$1241, DW_AT_name("RSVD15")
	.dwattr $C$DW$1241, DW_AT_TI_symbol_name("_RSVD15")
	.dwattr $C$DW$1241, DW_AT_data_member_location[DW_OP_plus_uconst 0x518]
	.dwattr $C$DW$1241, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1242	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$124)
	.dwattr $C$DW$1242, DW_AT_name("MDCFG")
	.dwattr $C$DW$1242, DW_AT_TI_symbol_name("_MDCFG")
	.dwattr $C$DW$1242, DW_AT_data_member_location[DW_OP_plus_uconst 0x600]
	.dwattr $C$DW$1242, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1243	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$126)
	.dwattr $C$DW$1243, DW_AT_name("RSVD16")
	.dwattr $C$DW$1243, DW_AT_TI_symbol_name("_RSVD16")
	.dwattr $C$DW$1243, DW_AT_data_member_location[DW_OP_plus_uconst 0x6a4]
	.dwattr $C$DW$1243, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1244	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$124)
	.dwattr $C$DW$1244, DW_AT_name("MDSTAT")
	.dwattr $C$DW$1244, DW_AT_TI_symbol_name("_MDSTAT")
	.dwattr $C$DW$1244, DW_AT_data_member_location[DW_OP_plus_uconst 0x800]
	.dwattr $C$DW$1244, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1245	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$126)
	.dwattr $C$DW$1245, DW_AT_name("RSVD17")
	.dwattr $C$DW$1245, DW_AT_TI_symbol_name("_RSVD17")
	.dwattr $C$DW$1245, DW_AT_data_member_location[DW_OP_plus_uconst 0x8a4]
	.dwattr $C$DW$1245, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1246	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$124)
	.dwattr $C$DW$1246, DW_AT_name("MDCTL")
	.dwattr $C$DW$1246, DW_AT_TI_symbol_name("_MDCTL")
	.dwattr $C$DW$1246, DW_AT_data_member_location[DW_OP_plus_uconst 0xa00]
	.dwattr $C$DW$1246, DW_AT_accessibility(DW_ACCESS_public)
	.dwendtag $C$DW$T$128

$C$DW$T$299	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_PscRegs")
	.dwattr $C$DW$T$299, DW_AT_type(*$C$DW$T$128)
	.dwattr $C$DW$T$299, DW_AT_language(DW_LANG_C)
$C$DW$T$300	.dwtag  DW_TAG_volatile_type
	.dwattr $C$DW$T$300, DW_AT_type(*$C$DW$T$299)
$C$DW$T$301	.dwtag  DW_TAG_pointer_type, DW_AT_type(*$C$DW$T$300)
	.dwattr $C$DW$T$301, DW_AT_address_class(0x20)
$C$DW$T$302	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_PscRegsOvly")
	.dwattr $C$DW$T$302, DW_AT_type(*$C$DW$T$301)
	.dwattr $C$DW$T$302, DW_AT_language(DW_LANG_C)

$C$DW$T$129	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$129, DW_AT_byte_size(0x90)
$C$DW$1247	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1247, DW_AT_name("PINMUX0")
	.dwattr $C$DW$1247, DW_AT_TI_symbol_name("_PINMUX0")
	.dwattr $C$DW$1247, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1247, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1248	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1248, DW_AT_name("PINMUX1")
	.dwattr $C$DW$1248, DW_AT_TI_symbol_name("_PINMUX1")
	.dwattr $C$DW$1248, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$1248, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1249	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1249, DW_AT_name("DSPBOOTADDR")
	.dwattr $C$DW$1249, DW_AT_TI_symbol_name("_DSPBOOTADDR")
	.dwattr $C$DW$1249, DW_AT_data_member_location[DW_OP_plus_uconst 0x8]
	.dwattr $C$DW$1249, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1250	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1250, DW_AT_name("BOOTCMPLT")
	.dwattr $C$DW$1250, DW_AT_TI_symbol_name("_BOOTCMPLT")
	.dwattr $C$DW$1250, DW_AT_data_member_location[DW_OP_plus_uconst 0xc]
	.dwattr $C$DW$1250, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1251	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1251, DW_AT_name("RSVD0")
	.dwattr $C$DW$1251, DW_AT_TI_symbol_name("_RSVD0")
	.dwattr $C$DW$1251, DW_AT_data_member_location[DW_OP_plus_uconst 0x10]
	.dwattr $C$DW$1251, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1252	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1252, DW_AT_name("BOOTCFG")
	.dwattr $C$DW$1252, DW_AT_TI_symbol_name("_BOOTCFG")
	.dwattr $C$DW$1252, DW_AT_data_member_location[DW_OP_plus_uconst 0x14]
	.dwattr $C$DW$1252, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1253	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1253, DW_AT_name("VDD1P0V_ADJ")
	.dwattr $C$DW$1253, DW_AT_TI_symbol_name("_VDD1P0V_ADJ")
	.dwattr $C$DW$1253, DW_AT_data_member_location[DW_OP_plus_uconst 0x18]
	.dwattr $C$DW$1253, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1254	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1254, DW_AT_name("VDD1P2V_ADJ")
	.dwattr $C$DW$1254, DW_AT_TI_symbol_name("_VDD1P2V_ADJ")
	.dwattr $C$DW$1254, DW_AT_data_member_location[DW_OP_plus_uconst 0x1c]
	.dwattr $C$DW$1254, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1255	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1255, DW_AT_name("DDR_SLEW")
	.dwattr $C$DW$1255, DW_AT_TI_symbol_name("_DDR_SLEW")
	.dwattr $C$DW$1255, DW_AT_data_member_location[DW_OP_plus_uconst 0x20]
	.dwattr $C$DW$1255, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1256	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1256, DW_AT_name("PERIPHEN")
	.dwattr $C$DW$1256, DW_AT_TI_symbol_name("_PERIPHEN")
	.dwattr $C$DW$1256, DW_AT_data_member_location[DW_OP_plus_uconst 0x24]
	.dwattr $C$DW$1256, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1257	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1257, DW_AT_name("DEVICE_ID")
	.dwattr $C$DW$1257, DW_AT_TI_symbol_name("_DEVICE_ID")
	.dwattr $C$DW$1257, DW_AT_data_member_location[DW_OP_plus_uconst 0x28]
	.dwattr $C$DW$1257, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1258	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1258, DW_AT_name("DAC_DEMEN")
	.dwattr $C$DW$1258, DW_AT_TI_symbol_name("_DAC_DEMEN")
	.dwattr $C$DW$1258, DW_AT_data_member_location[DW_OP_plus_uconst 0x2c]
	.dwattr $C$DW$1258, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1259	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1259, DW_AT_name("UHPICTL")
	.dwattr $C$DW$1259, DW_AT_TI_symbol_name("_UHPICTL")
	.dwattr $C$DW$1259, DW_AT_data_member_location[DW_OP_plus_uconst 0x30]
	.dwattr $C$DW$1259, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1260	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$21)
	.dwattr $C$DW$1260, DW_AT_name("RSVD1")
	.dwattr $C$DW$1260, DW_AT_TI_symbol_name("_RSVD1")
	.dwattr $C$DW$1260, DW_AT_data_member_location[DW_OP_plus_uconst 0x34]
	.dwattr $C$DW$1260, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1261	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1261, DW_AT_name("MSTPRI0")
	.dwattr $C$DW$1261, DW_AT_TI_symbol_name("_MSTPRI0")
	.dwattr $C$DW$1261, DW_AT_data_member_location[DW_OP_plus_uconst 0x3c]
	.dwattr $C$DW$1261, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1262	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1262, DW_AT_name("MSTPRI1")
	.dwattr $C$DW$1262, DW_AT_TI_symbol_name("_MSTPRI1")
	.dwattr $C$DW$1262, DW_AT_data_member_location[DW_OP_plus_uconst 0x40]
	.dwattr $C$DW$1262, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1263	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1263, DW_AT_name("VPSS_CLK_CTRL")
	.dwattr $C$DW$1263, DW_AT_TI_symbol_name("_VPSS_CLK_CTRL")
	.dwattr $C$DW$1263, DW_AT_data_member_location[DW_OP_plus_uconst 0x44]
	.dwattr $C$DW$1263, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1264	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1264, DW_AT_name("VDD3P3V_PWRDN")
	.dwattr $C$DW$1264, DW_AT_TI_symbol_name("_VDD3P3V_PWRDN")
	.dwattr $C$DW$1264, DW_AT_data_member_location[DW_OP_plus_uconst 0x48]
	.dwattr $C$DW$1264, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1265	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1265, DW_AT_name("DFT_ENABLE")
	.dwattr $C$DW$1265, DW_AT_TI_symbol_name("_DFT_ENABLE")
	.dwattr $C$DW$1265, DW_AT_data_member_location[DW_OP_plus_uconst 0x4c]
	.dwattr $C$DW$1265, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1266	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1266, DW_AT_name("SEC_SCAN_REG")
	.dwattr $C$DW$1266, DW_AT_TI_symbol_name("_SEC_SCAN_REG")
	.dwattr $C$DW$1266, DW_AT_data_member_location[DW_OP_plus_uconst 0x50]
	.dwattr $C$DW$1266, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1267	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1267, DW_AT_name("SEC_TEST_REG")
	.dwattr $C$DW$1267, DW_AT_TI_symbol_name("_SEC_TEST_REG")
	.dwattr $C$DW$1267, DW_AT_data_member_location[DW_OP_plus_uconst 0x54]
	.dwattr $C$DW$1267, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1268	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1268, DW_AT_name("SEC_TAP_CTL")
	.dwattr $C$DW$1268, DW_AT_TI_symbol_name("_SEC_TAP_CTL")
	.dwattr $C$DW$1268, DW_AT_data_member_location[DW_OP_plus_uconst 0x58]
	.dwattr $C$DW$1268, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1269	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1269, DW_AT_name("PUBLIC_KEY0")
	.dwattr $C$DW$1269, DW_AT_TI_symbol_name("_PUBLIC_KEY0")
	.dwattr $C$DW$1269, DW_AT_data_member_location[DW_OP_plus_uconst 0x5c]
	.dwattr $C$DW$1269, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1270	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1270, DW_AT_name("PUBLIC_KEY1")
	.dwattr $C$DW$1270, DW_AT_TI_symbol_name("_PUBLIC_KEY1")
	.dwattr $C$DW$1270, DW_AT_data_member_location[DW_OP_plus_uconst 0x60]
	.dwattr $C$DW$1270, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1271	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1271, DW_AT_name("PUBLIC_KEY2")
	.dwattr $C$DW$1271, DW_AT_TI_symbol_name("_PUBLIC_KEY2")
	.dwattr $C$DW$1271, DW_AT_data_member_location[DW_OP_plus_uconst 0x64]
	.dwattr $C$DW$1271, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1272	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1272, DW_AT_name("PUBLIC_KEY3")
	.dwattr $C$DW$1272, DW_AT_TI_symbol_name("_PUBLIC_KEY3")
	.dwattr $C$DW$1272, DW_AT_data_member_location[DW_OP_plus_uconst 0x68]
	.dwattr $C$DW$1272, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1273	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1273, DW_AT_name("VLYNQ_DELAY")
	.dwattr $C$DW$1273, DW_AT_TI_symbol_name("_VLYNQ_DELAY")
	.dwattr $C$DW$1273, DW_AT_data_member_location[DW_OP_plus_uconst 0x6c]
	.dwattr $C$DW$1273, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1274	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1274, DW_AT_name("SEC_PSEUDO0")
	.dwattr $C$DW$1274, DW_AT_TI_symbol_name("_SEC_PSEUDO0")
	.dwattr $C$DW$1274, DW_AT_data_member_location[DW_OP_plus_uconst 0x70]
	.dwattr $C$DW$1274, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1275	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1275, DW_AT_name("SEC_PSEUDO1")
	.dwattr $C$DW$1275, DW_AT_TI_symbol_name("_SEC_PSEUDO1")
	.dwattr $C$DW$1275, DW_AT_data_member_location[DW_OP_plus_uconst 0x74]
	.dwattr $C$DW$1275, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1276	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$23)
	.dwattr $C$DW$1276, DW_AT_name("RSVD2")
	.dwattr $C$DW$1276, DW_AT_TI_symbol_name("_RSVD2")
	.dwattr $C$DW$1276, DW_AT_data_member_location[DW_OP_plus_uconst 0x78]
	.dwattr $C$DW$1276, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1277	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1277, DW_AT_name("TIMERCTL")
	.dwattr $C$DW$1277, DW_AT_TI_symbol_name("_TIMERCTL")
	.dwattr $C$DW$1277, DW_AT_data_member_location[DW_OP_plus_uconst 0x84]
	.dwattr $C$DW$1277, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1278	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1278, DW_AT_name("TPTCCCFG")
	.dwattr $C$DW$1278, DW_AT_TI_symbol_name("_TPTCCCFG")
	.dwattr $C$DW$1278, DW_AT_data_member_location[DW_OP_plus_uconst 0x88]
	.dwattr $C$DW$1278, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1279	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1279, DW_AT_name("RSVD3")
	.dwattr $C$DW$1279, DW_AT_TI_symbol_name("_RSVD3")
	.dwattr $C$DW$1279, DW_AT_data_member_location[DW_OP_plus_uconst 0x8c]
	.dwattr $C$DW$1279, DW_AT_accessibility(DW_ACCESS_public)
	.dwendtag $C$DW$T$129

$C$DW$T$303	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_SysRegs")
	.dwattr $C$DW$T$303, DW_AT_type(*$C$DW$T$129)
	.dwattr $C$DW$T$303, DW_AT_language(DW_LANG_C)
$C$DW$T$304	.dwtag  DW_TAG_volatile_type
	.dwattr $C$DW$T$304, DW_AT_type(*$C$DW$T$303)
$C$DW$T$305	.dwtag  DW_TAG_pointer_type, DW_AT_type(*$C$DW$T$304)
	.dwattr $C$DW$T$305, DW_AT_address_class(0x20)
$C$DW$T$306	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_SysRegsOvly")
	.dwattr $C$DW$T$306, DW_AT_type(*$C$DW$T$305)
	.dwattr $C$DW$T$306, DW_AT_language(DW_LANG_C)

$C$DW$T$130	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$130, DW_AT_byte_size(0x80)
$C$DW$1280	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1280, DW_AT_name("PINMUX0")
	.dwattr $C$DW$1280, DW_AT_TI_symbol_name("_PINMUX0")
	.dwattr $C$DW$1280, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1280, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1281	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1281, DW_AT_name("PINMUX1")
	.dwattr $C$DW$1281, DW_AT_TI_symbol_name("_PINMUX1")
	.dwattr $C$DW$1281, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$1281, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1282	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1282, DW_AT_name("DSPBOOTADDR")
	.dwattr $C$DW$1282, DW_AT_TI_symbol_name("_DSPBOOTADDR")
	.dwattr $C$DW$1282, DW_AT_data_member_location[DW_OP_plus_uconst 0x8]
	.dwattr $C$DW$1282, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1283	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1283, DW_AT_name("BOOTCMPLT")
	.dwattr $C$DW$1283, DW_AT_TI_symbol_name("_BOOTCMPLT")
	.dwattr $C$DW$1283, DW_AT_data_member_location[DW_OP_plus_uconst 0xc]
	.dwattr $C$DW$1283, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1284	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1284, DW_AT_name("RSVD0")
	.dwattr $C$DW$1284, DW_AT_TI_symbol_name("_RSVD0")
	.dwattr $C$DW$1284, DW_AT_data_member_location[DW_OP_plus_uconst 0x10]
	.dwattr $C$DW$1284, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1285	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1285, DW_AT_name("BOOTCFG")
	.dwattr $C$DW$1285, DW_AT_TI_symbol_name("_BOOTCFG")
	.dwattr $C$DW$1285, DW_AT_data_member_location[DW_OP_plus_uconst 0x14]
	.dwattr $C$DW$1285, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1286	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1286, DW_AT_name("VDD1P0V_ADJ")
	.dwattr $C$DW$1286, DW_AT_TI_symbol_name("_VDD1P0V_ADJ")
	.dwattr $C$DW$1286, DW_AT_data_member_location[DW_OP_plus_uconst 0x18]
	.dwattr $C$DW$1286, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1287	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1287, DW_AT_name("VDD1P2V_ADJ")
	.dwattr $C$DW$1287, DW_AT_TI_symbol_name("_VDD1P2V_ADJ")
	.dwattr $C$DW$1287, DW_AT_data_member_location[DW_OP_plus_uconst 0x1c]
	.dwattr $C$DW$1287, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1288	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1288, DW_AT_name("DDR_SLEW")
	.dwattr $C$DW$1288, DW_AT_TI_symbol_name("_DDR_SLEW")
	.dwattr $C$DW$1288, DW_AT_data_member_location[DW_OP_plus_uconst 0x20]
	.dwattr $C$DW$1288, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1289	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1289, DW_AT_name("PERIPHEN")
	.dwattr $C$DW$1289, DW_AT_TI_symbol_name("_PERIPHEN")
	.dwattr $C$DW$1289, DW_AT_data_member_location[DW_OP_plus_uconst 0x24]
	.dwattr $C$DW$1289, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1290	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1290, DW_AT_name("DEVICE_ID")
	.dwattr $C$DW$1290, DW_AT_TI_symbol_name("_DEVICE_ID")
	.dwattr $C$DW$1290, DW_AT_data_member_location[DW_OP_plus_uconst 0x28]
	.dwattr $C$DW$1290, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1291	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1291, DW_AT_name("DAC_DEMEN")
	.dwattr $C$DW$1291, DW_AT_TI_symbol_name("_DAC_DEMEN")
	.dwattr $C$DW$1291, DW_AT_data_member_location[DW_OP_plus_uconst 0x2c]
	.dwattr $C$DW$1291, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1292	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1292, DW_AT_name("UHPICTL")
	.dwattr $C$DW$1292, DW_AT_TI_symbol_name("_UHPICTL")
	.dwattr $C$DW$1292, DW_AT_data_member_location[DW_OP_plus_uconst 0x30]
	.dwattr $C$DW$1292, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1293	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$21)
	.dwattr $C$DW$1293, DW_AT_name("RSVD1")
	.dwattr $C$DW$1293, DW_AT_TI_symbol_name("_RSVD1")
	.dwattr $C$DW$1293, DW_AT_data_member_location[DW_OP_plus_uconst 0x34]
	.dwattr $C$DW$1293, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1294	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1294, DW_AT_name("MSTPRI0")
	.dwattr $C$DW$1294, DW_AT_TI_symbol_name("_MSTPRI0")
	.dwattr $C$DW$1294, DW_AT_data_member_location[DW_OP_plus_uconst 0x3c]
	.dwattr $C$DW$1294, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1295	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1295, DW_AT_name("MSTPRI1")
	.dwattr $C$DW$1295, DW_AT_TI_symbol_name("_MSTPRI1")
	.dwattr $C$DW$1295, DW_AT_data_member_location[DW_OP_plus_uconst 0x40]
	.dwattr $C$DW$1295, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1296	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1296, DW_AT_name("VPSS_CLK_CTRL")
	.dwattr $C$DW$1296, DW_AT_TI_symbol_name("_VPSS_CLK_CTRL")
	.dwattr $C$DW$1296, DW_AT_data_member_location[DW_OP_plus_uconst 0x44]
	.dwattr $C$DW$1296, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1297	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1297, DW_AT_name("VDD3P3V_PWRDN")
	.dwattr $C$DW$1297, DW_AT_TI_symbol_name("_VDD3P3V_PWRDN")
	.dwattr $C$DW$1297, DW_AT_data_member_location[DW_OP_plus_uconst 0x48]
	.dwattr $C$DW$1297, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1298	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1298, DW_AT_name("RSVD2")
	.dwattr $C$DW$1298, DW_AT_TI_symbol_name("_RSVD2")
	.dwattr $C$DW$1298, DW_AT_data_member_location[DW_OP_plus_uconst 0x4c]
	.dwattr $C$DW$1298, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1299	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1299, DW_AT_name("SEC_SCAN_REG")
	.dwattr $C$DW$1299, DW_AT_TI_symbol_name("_SEC_SCAN_REG")
	.dwattr $C$DW$1299, DW_AT_data_member_location[DW_OP_plus_uconst 0x50]
	.dwattr $C$DW$1299, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1300	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1300, DW_AT_name("SEC_TEST_REG")
	.dwattr $C$DW$1300, DW_AT_TI_symbol_name("_SEC_TEST_REG")
	.dwattr $C$DW$1300, DW_AT_data_member_location[DW_OP_plus_uconst 0x54]
	.dwattr $C$DW$1300, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1301	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1301, DW_AT_name("SEC_TAP_CTL")
	.dwattr $C$DW$1301, DW_AT_TI_symbol_name("_SEC_TAP_CTL")
	.dwattr $C$DW$1301, DW_AT_data_member_location[DW_OP_plus_uconst 0x58]
	.dwattr $C$DW$1301, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1302	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1302, DW_AT_name("PUBLIC_KEY0")
	.dwattr $C$DW$1302, DW_AT_TI_symbol_name("_PUBLIC_KEY0")
	.dwattr $C$DW$1302, DW_AT_data_member_location[DW_OP_plus_uconst 0x5c]
	.dwattr $C$DW$1302, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1303	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1303, DW_AT_name("PUBLIC_KEY1")
	.dwattr $C$DW$1303, DW_AT_TI_symbol_name("_PUBLIC_KEY1")
	.dwattr $C$DW$1303, DW_AT_data_member_location[DW_OP_plus_uconst 0x60]
	.dwattr $C$DW$1303, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1304	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1304, DW_AT_name("PUBLIC_KEY2")
	.dwattr $C$DW$1304, DW_AT_TI_symbol_name("_PUBLIC_KEY2")
	.dwattr $C$DW$1304, DW_AT_data_member_location[DW_OP_plus_uconst 0x64]
	.dwattr $C$DW$1304, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1305	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1305, DW_AT_name("PUBLIC_KEY3")
	.dwattr $C$DW$1305, DW_AT_TI_symbol_name("_PUBLIC_KEY3")
	.dwattr $C$DW$1305, DW_AT_data_member_location[DW_OP_plus_uconst 0x68]
	.dwattr $C$DW$1305, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1306	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1306, DW_AT_name("VLYNQ_DELAY")
	.dwattr $C$DW$1306, DW_AT_TI_symbol_name("_VLYNQ_DELAY")
	.dwattr $C$DW$1306, DW_AT_data_member_location[DW_OP_plus_uconst 0x6c]
	.dwattr $C$DW$1306, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1307	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1307, DW_AT_name("RSVD3")
	.dwattr $C$DW$1307, DW_AT_TI_symbol_name("_RSVD3")
	.dwattr $C$DW$1307, DW_AT_data_member_location[DW_OP_plus_uconst 0x70]
	.dwattr $C$DW$1307, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1308	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1308, DW_AT_name("TIMERCTL")
	.dwattr $C$DW$1308, DW_AT_TI_symbol_name("_TIMERCTL")
	.dwattr $C$DW$1308, DW_AT_data_member_location[DW_OP_plus_uconst 0x74]
	.dwattr $C$DW$1308, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1309	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1309, DW_AT_name("TPTCCCFG")
	.dwattr $C$DW$1309, DW_AT_TI_symbol_name("_TPTCCCFG")
	.dwattr $C$DW$1309, DW_AT_data_member_location[DW_OP_plus_uconst 0x78]
	.dwattr $C$DW$1309, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1310	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1310, DW_AT_name("EMACADDR")
	.dwattr $C$DW$1310, DW_AT_TI_symbol_name("_EMACADDR")
	.dwattr $C$DW$1310, DW_AT_data_member_location[DW_OP_plus_uconst 0x7c]
	.dwattr $C$DW$1310, DW_AT_accessibility(DW_ACCESS_public)
	.dwendtag $C$DW$T$130

$C$DW$T$307	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_DeviceconfigRegs")
	.dwattr $C$DW$T$307, DW_AT_type(*$C$DW$T$130)
	.dwattr $C$DW$T$307, DW_AT_language(DW_LANG_C)
$C$DW$T$308	.dwtag  DW_TAG_volatile_type
	.dwattr $C$DW$T$308, DW_AT_type(*$C$DW$T$307)
$C$DW$T$309	.dwtag  DW_TAG_pointer_type, DW_AT_type(*$C$DW$T$308)
	.dwattr $C$DW$T$309, DW_AT_address_class(0x20)
$C$DW$T$310	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_DeviceconfigRegsOvly")
	.dwattr $C$DW$T$310, DW_AT_type(*$C$DW$T$309)
	.dwattr $C$DW$T$310, DW_AT_language(DW_LANG_C)

$C$DW$T$131	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$131, DW_AT_name("MBX")
	.dwattr $C$DW$T$131, DW_AT_byte_size(0x10)
$C$DW$1311	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1311, DW_AT_name("MID")
	.dwattr $C$DW$1311, DW_AT_TI_symbol_name("_MID")
	.dwattr $C$DW$1311, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1311, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1312	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1312, DW_AT_name("MCF")
	.dwattr $C$DW$1312, DW_AT_TI_symbol_name("_MCF")
	.dwattr $C$DW$1312, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$1312, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1313	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1313, DW_AT_name("MDL")
	.dwattr $C$DW$1313, DW_AT_TI_symbol_name("_MDL")
	.dwattr $C$DW$1313, DW_AT_data_member_location[DW_OP_plus_uconst 0x8]
	.dwattr $C$DW$1313, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1314	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1314, DW_AT_name("MDH")
	.dwattr $C$DW$1314, DW_AT_TI_symbol_name("_MDH")
	.dwattr $C$DW$1314, DW_AT_data_member_location[DW_OP_plus_uconst 0xc]
	.dwattr $C$DW$1314, DW_AT_accessibility(DW_ACCESS_public)
	.dwendtag $C$DW$T$131


$C$DW$T$137	.dwtag  DW_TAG_array_type, DW_AT_type(*$C$DW$T$131)
	.dwattr $C$DW$T$137, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$137, DW_AT_byte_size(0x200)
$C$DW$1315	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$1315, DW_AT_upper_bound(0x1f)
	.dwendtag $C$DW$T$137

$C$DW$T$132	.dwtag  DW_TAG_base_type, DW_AT_name("signed char")
	.dwattr $C$DW$T$132, DW_AT_encoding(DW_ATE_signed_char)
	.dwattr $C$DW$T$132, DW_AT_byte_size(0x01)
$C$DW$T$133	.dwtag  DW_TAG_const_type
	.dwattr $C$DW$T$133, DW_AT_type(*$C$DW$T$132)

$C$DW$T$134	.dwtag  DW_TAG_array_type, DW_AT_type(*$C$DW$T$133)
	.dwattr $C$DW$T$134, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$134, DW_AT_byte_size(0x18)
$C$DW$1316	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$1316, DW_AT_upper_bound(0x17)
	.dwendtag $C$DW$T$134


$C$DW$T$136	.dwtag  DW_TAG_array_type, DW_AT_type(*$C$DW$T$133)
	.dwattr $C$DW$T$136, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$136, DW_AT_byte_size(0x200)
$C$DW$1317	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$1317, DW_AT_upper_bound(0x1ff)
	.dwendtag $C$DW$T$136

$C$DW$T$183	.dwtag  DW_TAG_typedef, DW_AT_name("Char")
	.dwattr $C$DW$T$183, DW_AT_type(*$C$DW$T$132)
	.dwattr $C$DW$T$183, DW_AT_language(DW_LANG_C)
$C$DW$T$184	.dwtag  DW_TAG_pointer_type, DW_AT_type(*$C$DW$T$132)
	.dwattr $C$DW$T$184, DW_AT_address_class(0x20)
$C$DW$T$185	.dwtag  DW_TAG_typedef, DW_AT_name("String")
	.dwattr $C$DW$T$185, DW_AT_type(*$C$DW$T$184)
	.dwattr $C$DW$T$185, DW_AT_language(DW_LANG_C)
$C$DW$T$186	.dwtag  DW_TAG_typedef, DW_AT_name("Int8")
	.dwattr $C$DW$T$186, DW_AT_type(*$C$DW$T$132)
	.dwattr $C$DW$T$186, DW_AT_language(DW_LANG_C)

$C$DW$T$138	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$138, DW_AT_byte_size(0x600)
$C$DW$1318	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1318, DW_AT_name("ME")
	.dwattr $C$DW$1318, DW_AT_TI_symbol_name("_ME")
	.dwattr $C$DW$1318, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1318, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1319	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1319, DW_AT_name("MD")
	.dwattr $C$DW$1319, DW_AT_TI_symbol_name("_MD")
	.dwattr $C$DW$1319, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$1319, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1320	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1320, DW_AT_name("TRS")
	.dwattr $C$DW$1320, DW_AT_TI_symbol_name("_TRS")
	.dwattr $C$DW$1320, DW_AT_data_member_location[DW_OP_plus_uconst 0x8]
	.dwattr $C$DW$1320, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1321	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1321, DW_AT_name("TRR")
	.dwattr $C$DW$1321, DW_AT_TI_symbol_name("_TRR")
	.dwattr $C$DW$1321, DW_AT_data_member_location[DW_OP_plus_uconst 0xc]
	.dwattr $C$DW$1321, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1322	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1322, DW_AT_name("TA")
	.dwattr $C$DW$1322, DW_AT_TI_symbol_name("_TA")
	.dwattr $C$DW$1322, DW_AT_data_member_location[DW_OP_plus_uconst 0x10]
	.dwattr $C$DW$1322, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1323	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1323, DW_AT_name("AA")
	.dwattr $C$DW$1323, DW_AT_TI_symbol_name("_AA")
	.dwattr $C$DW$1323, DW_AT_data_member_location[DW_OP_plus_uconst 0x14]
	.dwattr $C$DW$1323, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1324	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1324, DW_AT_name("RMP")
	.dwattr $C$DW$1324, DW_AT_TI_symbol_name("_RMP")
	.dwattr $C$DW$1324, DW_AT_data_member_location[DW_OP_plus_uconst 0x18]
	.dwattr $C$DW$1324, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1325	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1325, DW_AT_name("RML")
	.dwattr $C$DW$1325, DW_AT_TI_symbol_name("_RML")
	.dwattr $C$DW$1325, DW_AT_data_member_location[DW_OP_plus_uconst 0x1c]
	.dwattr $C$DW$1325, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1326	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1326, DW_AT_name("RFP")
	.dwattr $C$DW$1326, DW_AT_TI_symbol_name("_RFP")
	.dwattr $C$DW$1326, DW_AT_data_member_location[DW_OP_plus_uconst 0x20]
	.dwattr $C$DW$1326, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1327	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1327, DW_AT_name("GAM")
	.dwattr $C$DW$1327, DW_AT_TI_symbol_name("_GAM")
	.dwattr $C$DW$1327, DW_AT_data_member_location[DW_OP_plus_uconst 0x24]
	.dwattr $C$DW$1327, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1328	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1328, DW_AT_name("MC")
	.dwattr $C$DW$1328, DW_AT_TI_symbol_name("_MC")
	.dwattr $C$DW$1328, DW_AT_data_member_location[DW_OP_plus_uconst 0x28]
	.dwattr $C$DW$1328, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1329	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1329, DW_AT_name("BTC")
	.dwattr $C$DW$1329, DW_AT_TI_symbol_name("_BTC")
	.dwattr $C$DW$1329, DW_AT_data_member_location[DW_OP_plus_uconst 0x2c]
	.dwattr $C$DW$1329, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1330	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1330, DW_AT_name("ES")
	.dwattr $C$DW$1330, DW_AT_TI_symbol_name("_ES")
	.dwattr $C$DW$1330, DW_AT_data_member_location[DW_OP_plus_uconst 0x30]
	.dwattr $C$DW$1330, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1331	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1331, DW_AT_name("TEC")
	.dwattr $C$DW$1331, DW_AT_TI_symbol_name("_TEC")
	.dwattr $C$DW$1331, DW_AT_data_member_location[DW_OP_plus_uconst 0x34]
	.dwattr $C$DW$1331, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1332	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1332, DW_AT_name("REC")
	.dwattr $C$DW$1332, DW_AT_TI_symbol_name("_REC")
	.dwattr $C$DW$1332, DW_AT_data_member_location[DW_OP_plus_uconst 0x38]
	.dwattr $C$DW$1332, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1333	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1333, DW_AT_name("GIF0")
	.dwattr $C$DW$1333, DW_AT_TI_symbol_name("_GIF0")
	.dwattr $C$DW$1333, DW_AT_data_member_location[DW_OP_plus_uconst 0x3c]
	.dwattr $C$DW$1333, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1334	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1334, DW_AT_name("GIM")
	.dwattr $C$DW$1334, DW_AT_TI_symbol_name("_GIM")
	.dwattr $C$DW$1334, DW_AT_data_member_location[DW_OP_plus_uconst 0x40]
	.dwattr $C$DW$1334, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1335	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1335, DW_AT_name("GIF1")
	.dwattr $C$DW$1335, DW_AT_TI_symbol_name("_GIF1")
	.dwattr $C$DW$1335, DW_AT_data_member_location[DW_OP_plus_uconst 0x44]
	.dwattr $C$DW$1335, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1336	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1336, DW_AT_name("MIM")
	.dwattr $C$DW$1336, DW_AT_TI_symbol_name("_MIM")
	.dwattr $C$DW$1336, DW_AT_data_member_location[DW_OP_plus_uconst 0x48]
	.dwattr $C$DW$1336, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1337	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1337, DW_AT_name("MIL")
	.dwattr $C$DW$1337, DW_AT_TI_symbol_name("_MIL")
	.dwattr $C$DW$1337, DW_AT_data_member_location[DW_OP_plus_uconst 0x4c]
	.dwattr $C$DW$1337, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1338	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1338, DW_AT_name("OPC")
	.dwattr $C$DW$1338, DW_AT_TI_symbol_name("_OPC")
	.dwattr $C$DW$1338, DW_AT_data_member_location[DW_OP_plus_uconst 0x50]
	.dwattr $C$DW$1338, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1339	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1339, DW_AT_name("TIOC")
	.dwattr $C$DW$1339, DW_AT_TI_symbol_name("_TIOC")
	.dwattr $C$DW$1339, DW_AT_data_member_location[DW_OP_plus_uconst 0x54]
	.dwattr $C$DW$1339, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1340	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1340, DW_AT_name("RIOC")
	.dwattr $C$DW$1340, DW_AT_TI_symbol_name("_RIOC")
	.dwattr $C$DW$1340, DW_AT_data_member_location[DW_OP_plus_uconst 0x58]
	.dwattr $C$DW$1340, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1341	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1341, DW_AT_name("LNT")
	.dwattr $C$DW$1341, DW_AT_TI_symbol_name("_LNT")
	.dwattr $C$DW$1341, DW_AT_data_member_location[DW_OP_plus_uconst 0x5c]
	.dwattr $C$DW$1341, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1342	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1342, DW_AT_name("TOC")
	.dwattr $C$DW$1342, DW_AT_TI_symbol_name("_TOC")
	.dwattr $C$DW$1342, DW_AT_data_member_location[DW_OP_plus_uconst 0x60]
	.dwattr $C$DW$1342, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1343	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1343, DW_AT_name("TOS")
	.dwattr $C$DW$1343, DW_AT_TI_symbol_name("_TOS")
	.dwattr $C$DW$1343, DW_AT_data_member_location[DW_OP_plus_uconst 0x64]
	.dwattr $C$DW$1343, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1344	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$134)
	.dwattr $C$DW$1344, DW_AT_name("RSVD0")
	.dwattr $C$DW$1344, DW_AT_TI_symbol_name("_RSVD0")
	.dwattr $C$DW$1344, DW_AT_data_member_location[DW_OP_plus_uconst 0x68]
	.dwattr $C$DW$1344, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1345	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$135)
	.dwattr $C$DW$1345, DW_AT_name("LAM")
	.dwattr $C$DW$1345, DW_AT_TI_symbol_name("_LAM")
	.dwattr $C$DW$1345, DW_AT_data_member_location[DW_OP_plus_uconst 0x80]
	.dwattr $C$DW$1345, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1346	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$135)
	.dwattr $C$DW$1346, DW_AT_name("MOTS")
	.dwattr $C$DW$1346, DW_AT_TI_symbol_name("_MOTS")
	.dwattr $C$DW$1346, DW_AT_data_member_location[DW_OP_plus_uconst 0x100]
	.dwattr $C$DW$1346, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1347	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$135)
	.dwattr $C$DW$1347, DW_AT_name("MOTO")
	.dwattr $C$DW$1347, DW_AT_TI_symbol_name("_MOTO")
	.dwattr $C$DW$1347, DW_AT_data_member_location[DW_OP_plus_uconst 0x180]
	.dwattr $C$DW$1347, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1348	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$136)
	.dwattr $C$DW$1348, DW_AT_name("RSVD1")
	.dwattr $C$DW$1348, DW_AT_TI_symbol_name("_RSVD1")
	.dwattr $C$DW$1348, DW_AT_data_member_location[DW_OP_plus_uconst 0x200]
	.dwattr $C$DW$1348, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1349	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$137)
	.dwattr $C$DW$1349, DW_AT_name("MBX")
	.dwattr $C$DW$1349, DW_AT_TI_symbol_name("_MBX")
	.dwattr $C$DW$1349, DW_AT_data_member_location[DW_OP_plus_uconst 0x400]
	.dwattr $C$DW$1349, DW_AT_accessibility(DW_ACCESS_public)
	.dwendtag $C$DW$T$138

$C$DW$T$311	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_CanRegs")
	.dwattr $C$DW$T$311, DW_AT_type(*$C$DW$T$138)
	.dwattr $C$DW$T$311, DW_AT_language(DW_LANG_C)
$C$DW$T$312	.dwtag  DW_TAG_typedef, DW_AT_name("CAN_RegsOverlay")
	.dwattr $C$DW$T$312, DW_AT_type(*$C$DW$T$311)
	.dwattr $C$DW$T$312, DW_AT_language(DW_LANG_C)
$C$DW$T$313	.dwtag  DW_TAG_volatile_type
	.dwattr $C$DW$T$313, DW_AT_type(*$C$DW$T$311)
$C$DW$T$314	.dwtag  DW_TAG_pointer_type, DW_AT_type(*$C$DW$T$313)
	.dwattr $C$DW$T$314, DW_AT_address_class(0x20)
$C$DW$T$315	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_CanRegsOvly")
	.dwattr $C$DW$T$315, DW_AT_type(*$C$DW$T$314)
	.dwattr $C$DW$T$315, DW_AT_language(DW_LANG_C)

$C$DW$T$141	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$141, DW_AT_byte_size(0x1c4)
$C$DW$1350	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$24)
	.dwattr $C$DW$1350, DW_AT_name("EVTFLAG")
	.dwattr $C$DW$1350, DW_AT_TI_symbol_name("_EVTFLAG")
	.dwattr $C$DW$1350, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1350, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1351	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$64)
	.dwattr $C$DW$1351, DW_AT_name("RSVD0")
	.dwattr $C$DW$1351, DW_AT_TI_symbol_name("_RSVD0")
	.dwattr $C$DW$1351, DW_AT_data_member_location[DW_OP_plus_uconst 0x10]
	.dwattr $C$DW$1351, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1352	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$24)
	.dwattr $C$DW$1352, DW_AT_name("EVTSET")
	.dwattr $C$DW$1352, DW_AT_TI_symbol_name("_EVTSET")
	.dwattr $C$DW$1352, DW_AT_data_member_location[DW_OP_plus_uconst 0x20]
	.dwattr $C$DW$1352, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1353	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$64)
	.dwattr $C$DW$1353, DW_AT_name("RSVD1")
	.dwattr $C$DW$1353, DW_AT_TI_symbol_name("_RSVD1")
	.dwattr $C$DW$1353, DW_AT_data_member_location[DW_OP_plus_uconst 0x30]
	.dwattr $C$DW$1353, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1354	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$24)
	.dwattr $C$DW$1354, DW_AT_name("EVTCLR")
	.dwattr $C$DW$1354, DW_AT_TI_symbol_name("_EVTCLR")
	.dwattr $C$DW$1354, DW_AT_data_member_location[DW_OP_plus_uconst 0x40]
	.dwattr $C$DW$1354, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1355	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$68)
	.dwattr $C$DW$1355, DW_AT_name("RSVD2")
	.dwattr $C$DW$1355, DW_AT_TI_symbol_name("_RSVD2")
	.dwattr $C$DW$1355, DW_AT_data_member_location[DW_OP_plus_uconst 0x50]
	.dwattr $C$DW$1355, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1356	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$24)
	.dwattr $C$DW$1356, DW_AT_name("EVTMASK")
	.dwattr $C$DW$1356, DW_AT_TI_symbol_name("_EVTMASK")
	.dwattr $C$DW$1356, DW_AT_data_member_location[DW_OP_plus_uconst 0x80]
	.dwattr $C$DW$1356, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1357	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$64)
	.dwattr $C$DW$1357, DW_AT_name("RSVD3")
	.dwattr $C$DW$1357, DW_AT_TI_symbol_name("_RSVD3")
	.dwattr $C$DW$1357, DW_AT_data_member_location[DW_OP_plus_uconst 0x90]
	.dwattr $C$DW$1357, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1358	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$24)
	.dwattr $C$DW$1358, DW_AT_name("MEVTFLAG")
	.dwattr $C$DW$1358, DW_AT_TI_symbol_name("_MEVTFLAG")
	.dwattr $C$DW$1358, DW_AT_data_member_location[DW_OP_plus_uconst 0xa0]
	.dwattr $C$DW$1358, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1359	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$64)
	.dwattr $C$DW$1359, DW_AT_name("RSVD4")
	.dwattr $C$DW$1359, DW_AT_TI_symbol_name("_RSVD4")
	.dwattr $C$DW$1359, DW_AT_data_member_location[DW_OP_plus_uconst 0xb0]
	.dwattr $C$DW$1359, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1360	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$24)
	.dwattr $C$DW$1360, DW_AT_name("EXPMASK")
	.dwattr $C$DW$1360, DW_AT_TI_symbol_name("_EXPMASK")
	.dwattr $C$DW$1360, DW_AT_data_member_location[DW_OP_plus_uconst 0xc0]
	.dwattr $C$DW$1360, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1361	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$64)
	.dwattr $C$DW$1361, DW_AT_name("RSVD5")
	.dwattr $C$DW$1361, DW_AT_TI_symbol_name("_RSVD5")
	.dwattr $C$DW$1361, DW_AT_data_member_location[DW_OP_plus_uconst 0xd0]
	.dwattr $C$DW$1361, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1362	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$24)
	.dwattr $C$DW$1362, DW_AT_name("MEXPFLAG")
	.dwattr $C$DW$1362, DW_AT_TI_symbol_name("_MEXPFLAG")
	.dwattr $C$DW$1362, DW_AT_data_member_location[DW_OP_plus_uconst 0xe0]
	.dwattr $C$DW$1362, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1363	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$54)
	.dwattr $C$DW$1363, DW_AT_name("RSVD6")
	.dwattr $C$DW$1363, DW_AT_TI_symbol_name("_RSVD6")
	.dwattr $C$DW$1363, DW_AT_data_member_location[DW_OP_plus_uconst 0xf0]
	.dwattr $C$DW$1363, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1364	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1364, DW_AT_name("INTMUX1")
	.dwattr $C$DW$1364, DW_AT_TI_symbol_name("_INTMUX1")
	.dwattr $C$DW$1364, DW_AT_data_member_location[DW_OP_plus_uconst 0x104]
	.dwattr $C$DW$1364, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1365	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1365, DW_AT_name("INTMUX2")
	.dwattr $C$DW$1365, DW_AT_TI_symbol_name("_INTMUX2")
	.dwattr $C$DW$1365, DW_AT_data_member_location[DW_OP_plus_uconst 0x108]
	.dwattr $C$DW$1365, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1366	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1366, DW_AT_name("INTMUX3")
	.dwattr $C$DW$1366, DW_AT_TI_symbol_name("_INTMUX3")
	.dwattr $C$DW$1366, DW_AT_data_member_location[DW_OP_plus_uconst 0x10c]
	.dwattr $C$DW$1366, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1367	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$68)
	.dwattr $C$DW$1367, DW_AT_name("RSVD7")
	.dwattr $C$DW$1367, DW_AT_TI_symbol_name("_RSVD7")
	.dwattr $C$DW$1367, DW_AT_data_member_location[DW_OP_plus_uconst 0x110]
	.dwattr $C$DW$1367, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1368	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1368, DW_AT_name("AEGMUX0")
	.dwattr $C$DW$1368, DW_AT_TI_symbol_name("_AEGMUX0")
	.dwattr $C$DW$1368, DW_AT_data_member_location[DW_OP_plus_uconst 0x140]
	.dwattr $C$DW$1368, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1369	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1369, DW_AT_name("AEGMUX1")
	.dwattr $C$DW$1369, DW_AT_TI_symbol_name("_AEGMUX1")
	.dwattr $C$DW$1369, DW_AT_data_member_location[DW_OP_plus_uconst 0x144]
	.dwattr $C$DW$1369, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1370	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$139)
	.dwattr $C$DW$1370, DW_AT_name("RSVD8")
	.dwattr $C$DW$1370, DW_AT_TI_symbol_name("_RSVD8")
	.dwattr $C$DW$1370, DW_AT_data_member_location[DW_OP_plus_uconst 0x148]
	.dwattr $C$DW$1370, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1371	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1371, DW_AT_name("INTXSTAT")
	.dwattr $C$DW$1371, DW_AT_TI_symbol_name("_INTXSTAT")
	.dwattr $C$DW$1371, DW_AT_data_member_location[DW_OP_plus_uconst 0x180]
	.dwattr $C$DW$1371, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1372	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1372, DW_AT_name("INTXCLR")
	.dwattr $C$DW$1372, DW_AT_TI_symbol_name("_INTXCLR")
	.dwattr $C$DW$1372, DW_AT_data_member_location[DW_OP_plus_uconst 0x184]
	.dwattr $C$DW$1372, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1373	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1373, DW_AT_name("INTDMASK")
	.dwattr $C$DW$1373, DW_AT_TI_symbol_name("_INTDMASK")
	.dwattr $C$DW$1373, DW_AT_data_member_location[DW_OP_plus_uconst 0x188]
	.dwattr $C$DW$1373, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1374	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$140)
	.dwattr $C$DW$1374, DW_AT_name("RSVD9")
	.dwattr $C$DW$1374, DW_AT_TI_symbol_name("_RSVD9")
	.dwattr $C$DW$1374, DW_AT_data_member_location[DW_OP_plus_uconst 0x18c]
	.dwattr $C$DW$1374, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1375	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1375, DW_AT_name("EVTASRT")
	.dwattr $C$DW$1375, DW_AT_TI_symbol_name("_EVTASRT")
	.dwattr $C$DW$1375, DW_AT_data_member_location[DW_OP_plus_uconst 0x1c0]
	.dwattr $C$DW$1375, DW_AT_accessibility(DW_ACCESS_public)
	.dwendtag $C$DW$T$141

$C$DW$T$318	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_IntcRegs")
	.dwattr $C$DW$T$318, DW_AT_type(*$C$DW$T$141)
	.dwattr $C$DW$T$318, DW_AT_language(DW_LANG_C)
$C$DW$T$319	.dwtag  DW_TAG_volatile_type
	.dwattr $C$DW$T$319, DW_AT_type(*$C$DW$T$318)
$C$DW$T$320	.dwtag  DW_TAG_pointer_type, DW_AT_type(*$C$DW$T$319)
	.dwattr $C$DW$T$320, DW_AT_address_class(0x20)
$C$DW$T$321	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_IntcRegsOvly")
	.dwattr $C$DW$T$321, DW_AT_type(*$C$DW$T$320)
	.dwattr $C$DW$T$321, DW_AT_language(DW_LANG_C)

$C$DW$T$142	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$142, DW_AT_byte_size(0x1cc)
$C$DW$1376	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1376, DW_AT_name("PID")
	.dwattr $C$DW$1376, DW_AT_TI_symbol_name("_PID")
	.dwattr $C$DW$1376, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1376, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1377	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1377, DW_AT_name("PCR")
	.dwattr $C$DW$1377, DW_AT_TI_symbol_name("_PCR")
	.dwattr $C$DW$1377, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$1377, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1378	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1378, DW_AT_name("VFOCUS_CNT")
	.dwattr $C$DW$1378, DW_AT_TI_symbol_name("_VFOCUS_CNT")
	.dwattr $C$DW$1378, DW_AT_data_member_location[DW_OP_plus_uconst 0x8]
	.dwattr $C$DW$1378, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1379	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1379, DW_AT_name("W0_START")
	.dwattr $C$DW$1379, DW_AT_TI_symbol_name("_W0_START")
	.dwattr $C$DW$1379, DW_AT_data_member_location[DW_OP_plus_uconst 0xc]
	.dwattr $C$DW$1379, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1380	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1380, DW_AT_name("W0_END")
	.dwattr $C$DW$1380, DW_AT_TI_symbol_name("_W0_END")
	.dwattr $C$DW$1380, DW_AT_data_member_location[DW_OP_plus_uconst 0x10]
	.dwattr $C$DW$1380, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1381	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1381, DW_AT_name("W1_START")
	.dwattr $C$DW$1381, DW_AT_TI_symbol_name("_W1_START")
	.dwattr $C$DW$1381, DW_AT_data_member_location[DW_OP_plus_uconst 0x14]
	.dwattr $C$DW$1381, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1382	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1382, DW_AT_name("W1_END")
	.dwattr $C$DW$1382, DW_AT_TI_symbol_name("_W1_END")
	.dwattr $C$DW$1382, DW_AT_data_member_location[DW_OP_plus_uconst 0x18]
	.dwattr $C$DW$1382, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1383	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1383, DW_AT_name("W2_START")
	.dwattr $C$DW$1383, DW_AT_TI_symbol_name("_W2_START")
	.dwattr $C$DW$1383, DW_AT_data_member_location[DW_OP_plus_uconst 0x1c]
	.dwattr $C$DW$1383, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1384	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1384, DW_AT_name("W2_END")
	.dwattr $C$DW$1384, DW_AT_TI_symbol_name("_W2_END")
	.dwattr $C$DW$1384, DW_AT_data_member_location[DW_OP_plus_uconst 0x20]
	.dwattr $C$DW$1384, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1385	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1385, DW_AT_name("W3_START")
	.dwattr $C$DW$1385, DW_AT_TI_symbol_name("_W3_START")
	.dwattr $C$DW$1385, DW_AT_data_member_location[DW_OP_plus_uconst 0x24]
	.dwattr $C$DW$1385, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1386	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1386, DW_AT_name("W3_END")
	.dwattr $C$DW$1386, DW_AT_TI_symbol_name("_W3_END")
	.dwattr $C$DW$1386, DW_AT_data_member_location[DW_OP_plus_uconst 0x28]
	.dwattr $C$DW$1386, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1387	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1387, DW_AT_name("W4_START")
	.dwattr $C$DW$1387, DW_AT_TI_symbol_name("_W4_START")
	.dwattr $C$DW$1387, DW_AT_data_member_location[DW_OP_plus_uconst 0x2c]
	.dwattr $C$DW$1387, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1388	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1388, DW_AT_name("W4_END")
	.dwattr $C$DW$1388, DW_AT_TI_symbol_name("_W4_END")
	.dwattr $C$DW$1388, DW_AT_data_member_location[DW_OP_plus_uconst 0x30]
	.dwattr $C$DW$1388, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1389	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1389, DW_AT_name("W5_START")
	.dwattr $C$DW$1389, DW_AT_TI_symbol_name("_W5_START")
	.dwattr $C$DW$1389, DW_AT_data_member_location[DW_OP_plus_uconst 0x34]
	.dwattr $C$DW$1389, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1390	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1390, DW_AT_name("W5_END")
	.dwattr $C$DW$1390, DW_AT_TI_symbol_name("_W5_END")
	.dwattr $C$DW$1390, DW_AT_data_member_location[DW_OP_plus_uconst 0x38]
	.dwattr $C$DW$1390, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1391	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1391, DW_AT_name("W0_ACCL_R")
	.dwattr $C$DW$1391, DW_AT_TI_symbol_name("_W0_ACCL_R")
	.dwattr $C$DW$1391, DW_AT_data_member_location[DW_OP_plus_uconst 0x3c]
	.dwattr $C$DW$1391, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1392	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1392, DW_AT_name("W0_ACCU_R")
	.dwattr $C$DW$1392, DW_AT_TI_symbol_name("_W0_ACCU_R")
	.dwattr $C$DW$1392, DW_AT_data_member_location[DW_OP_plus_uconst 0x40]
	.dwattr $C$DW$1392, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1393	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1393, DW_AT_name("W0_ACCL_G")
	.dwattr $C$DW$1393, DW_AT_TI_symbol_name("_W0_ACCL_G")
	.dwattr $C$DW$1393, DW_AT_data_member_location[DW_OP_plus_uconst 0x44]
	.dwattr $C$DW$1393, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1394	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1394, DW_AT_name("W0_ACCU_G")
	.dwattr $C$DW$1394, DW_AT_TI_symbol_name("_W0_ACCU_G")
	.dwattr $C$DW$1394, DW_AT_data_member_location[DW_OP_plus_uconst 0x48]
	.dwattr $C$DW$1394, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1395	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1395, DW_AT_name("W0_ACCL_B")
	.dwattr $C$DW$1395, DW_AT_TI_symbol_name("_W0_ACCL_B")
	.dwattr $C$DW$1395, DW_AT_data_member_location[DW_OP_plus_uconst 0x4c]
	.dwattr $C$DW$1395, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1396	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1396, DW_AT_name("W0_ACCU_B")
	.dwattr $C$DW$1396, DW_AT_TI_symbol_name("_W0_ACCU_B")
	.dwattr $C$DW$1396, DW_AT_data_member_location[DW_OP_plus_uconst 0x50]
	.dwattr $C$DW$1396, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1397	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1397, DW_AT_name("W1_ACCL_R")
	.dwattr $C$DW$1397, DW_AT_TI_symbol_name("_W1_ACCL_R")
	.dwattr $C$DW$1397, DW_AT_data_member_location[DW_OP_plus_uconst 0x54]
	.dwattr $C$DW$1397, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1398	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1398, DW_AT_name("W1_ACCU_R")
	.dwattr $C$DW$1398, DW_AT_TI_symbol_name("_W1_ACCU_R")
	.dwattr $C$DW$1398, DW_AT_data_member_location[DW_OP_plus_uconst 0x58]
	.dwattr $C$DW$1398, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1399	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1399, DW_AT_name("W1_ACCL_G")
	.dwattr $C$DW$1399, DW_AT_TI_symbol_name("_W1_ACCL_G")
	.dwattr $C$DW$1399, DW_AT_data_member_location[DW_OP_plus_uconst 0x5c]
	.dwattr $C$DW$1399, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1400	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1400, DW_AT_name("W1_ACCU_G")
	.dwattr $C$DW$1400, DW_AT_TI_symbol_name("_W1_ACCU_G")
	.dwattr $C$DW$1400, DW_AT_data_member_location[DW_OP_plus_uconst 0x60]
	.dwattr $C$DW$1400, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1401	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1401, DW_AT_name("W1_ACCL_B")
	.dwattr $C$DW$1401, DW_AT_TI_symbol_name("_W1_ACCL_B")
	.dwattr $C$DW$1401, DW_AT_data_member_location[DW_OP_plus_uconst 0x64]
	.dwattr $C$DW$1401, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1402	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1402, DW_AT_name("W1_ACCU_B")
	.dwattr $C$DW$1402, DW_AT_TI_symbol_name("_W1_ACCU_B")
	.dwattr $C$DW$1402, DW_AT_data_member_location[DW_OP_plus_uconst 0x68]
	.dwattr $C$DW$1402, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1403	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1403, DW_AT_name("W2_ACCL_R")
	.dwattr $C$DW$1403, DW_AT_TI_symbol_name("_W2_ACCL_R")
	.dwattr $C$DW$1403, DW_AT_data_member_location[DW_OP_plus_uconst 0x6c]
	.dwattr $C$DW$1403, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1404	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1404, DW_AT_name("W2_ACCU_R")
	.dwattr $C$DW$1404, DW_AT_TI_symbol_name("_W2_ACCU_R")
	.dwattr $C$DW$1404, DW_AT_data_member_location[DW_OP_plus_uconst 0x70]
	.dwattr $C$DW$1404, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1405	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1405, DW_AT_name("W2_ACCL_G")
	.dwattr $C$DW$1405, DW_AT_TI_symbol_name("_W2_ACCL_G")
	.dwattr $C$DW$1405, DW_AT_data_member_location[DW_OP_plus_uconst 0x74]
	.dwattr $C$DW$1405, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1406	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1406, DW_AT_name("W2_ACCU_G")
	.dwattr $C$DW$1406, DW_AT_TI_symbol_name("_W2_ACCU_G")
	.dwattr $C$DW$1406, DW_AT_data_member_location[DW_OP_plus_uconst 0x78]
	.dwattr $C$DW$1406, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1407	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1407, DW_AT_name("W2_ACCL_B")
	.dwattr $C$DW$1407, DW_AT_TI_symbol_name("_W2_ACCL_B")
	.dwattr $C$DW$1407, DW_AT_data_member_location[DW_OP_plus_uconst 0x7c]
	.dwattr $C$DW$1407, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1408	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1408, DW_AT_name("W2_ACCU_B")
	.dwattr $C$DW$1408, DW_AT_TI_symbol_name("_W2_ACCU_B")
	.dwattr $C$DW$1408, DW_AT_data_member_location[DW_OP_plus_uconst 0x80]
	.dwattr $C$DW$1408, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1409	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1409, DW_AT_name("W3_ACCL_R")
	.dwattr $C$DW$1409, DW_AT_TI_symbol_name("_W3_ACCL_R")
	.dwattr $C$DW$1409, DW_AT_data_member_location[DW_OP_plus_uconst 0x84]
	.dwattr $C$DW$1409, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1410	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1410, DW_AT_name("W3_ACCU_R")
	.dwattr $C$DW$1410, DW_AT_TI_symbol_name("_W3_ACCU_R")
	.dwattr $C$DW$1410, DW_AT_data_member_location[DW_OP_plus_uconst 0x88]
	.dwattr $C$DW$1410, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1411	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1411, DW_AT_name("W3_ACCL_G")
	.dwattr $C$DW$1411, DW_AT_TI_symbol_name("_W3_ACCL_G")
	.dwattr $C$DW$1411, DW_AT_data_member_location[DW_OP_plus_uconst 0x8c]
	.dwattr $C$DW$1411, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1412	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1412, DW_AT_name("W3_ACCU_G")
	.dwattr $C$DW$1412, DW_AT_TI_symbol_name("_W3_ACCU_G")
	.dwattr $C$DW$1412, DW_AT_data_member_location[DW_OP_plus_uconst 0x90]
	.dwattr $C$DW$1412, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1413	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1413, DW_AT_name("W3_ACCL_B")
	.dwattr $C$DW$1413, DW_AT_TI_symbol_name("_W3_ACCL_B")
	.dwattr $C$DW$1413, DW_AT_data_member_location[DW_OP_plus_uconst 0x94]
	.dwattr $C$DW$1413, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1414	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1414, DW_AT_name("W3_ACCU_B")
	.dwattr $C$DW$1414, DW_AT_TI_symbol_name("_W3_ACCU_B")
	.dwattr $C$DW$1414, DW_AT_data_member_location[DW_OP_plus_uconst 0x98]
	.dwattr $C$DW$1414, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1415	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1415, DW_AT_name("W4_ACCL_R")
	.dwattr $C$DW$1415, DW_AT_TI_symbol_name("_W4_ACCL_R")
	.dwattr $C$DW$1415, DW_AT_data_member_location[DW_OP_plus_uconst 0x9c]
	.dwattr $C$DW$1415, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1416	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1416, DW_AT_name("W4_ACCU_R")
	.dwattr $C$DW$1416, DW_AT_TI_symbol_name("_W4_ACCU_R")
	.dwattr $C$DW$1416, DW_AT_data_member_location[DW_OP_plus_uconst 0xa0]
	.dwattr $C$DW$1416, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1417	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1417, DW_AT_name("W4_ACCL_G")
	.dwattr $C$DW$1417, DW_AT_TI_symbol_name("_W4_ACCL_G")
	.dwattr $C$DW$1417, DW_AT_data_member_location[DW_OP_plus_uconst 0xa4]
	.dwattr $C$DW$1417, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1418	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1418, DW_AT_name("W4_ACCU_G")
	.dwattr $C$DW$1418, DW_AT_TI_symbol_name("_W4_ACCU_G")
	.dwattr $C$DW$1418, DW_AT_data_member_location[DW_OP_plus_uconst 0xa8]
	.dwattr $C$DW$1418, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1419	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1419, DW_AT_name("W4_ACCL_B")
	.dwattr $C$DW$1419, DW_AT_TI_symbol_name("_W4_ACCL_B")
	.dwattr $C$DW$1419, DW_AT_data_member_location[DW_OP_plus_uconst 0xac]
	.dwattr $C$DW$1419, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1420	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1420, DW_AT_name("W4_ACCU_B")
	.dwattr $C$DW$1420, DW_AT_TI_symbol_name("_W4_ACCU_B")
	.dwattr $C$DW$1420, DW_AT_data_member_location[DW_OP_plus_uconst 0xb0]
	.dwattr $C$DW$1420, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1421	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1421, DW_AT_name("W5_ACCL_R")
	.dwattr $C$DW$1421, DW_AT_TI_symbol_name("_W5_ACCL_R")
	.dwattr $C$DW$1421, DW_AT_data_member_location[DW_OP_plus_uconst 0xb4]
	.dwattr $C$DW$1421, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1422	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1422, DW_AT_name("W5_ACCU_R")
	.dwattr $C$DW$1422, DW_AT_TI_symbol_name("_W5_ACCU_R")
	.dwattr $C$DW$1422, DW_AT_data_member_location[DW_OP_plus_uconst 0xb8]
	.dwattr $C$DW$1422, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1423	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1423, DW_AT_name("W5_ACCL_G")
	.dwattr $C$DW$1423, DW_AT_TI_symbol_name("_W5_ACCL_G")
	.dwattr $C$DW$1423, DW_AT_data_member_location[DW_OP_plus_uconst 0xbc]
	.dwattr $C$DW$1423, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1424	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1424, DW_AT_name("W5_ACCU_G")
	.dwattr $C$DW$1424, DW_AT_TI_symbol_name("_W5_ACCU_G")
	.dwattr $C$DW$1424, DW_AT_data_member_location[DW_OP_plus_uconst 0xc0]
	.dwattr $C$DW$1424, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1425	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1425, DW_AT_name("W5_ACCL_B")
	.dwattr $C$DW$1425, DW_AT_TI_symbol_name("_W5_ACCL_B")
	.dwattr $C$DW$1425, DW_AT_data_member_location[DW_OP_plus_uconst 0xc4]
	.dwattr $C$DW$1425, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1426	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1426, DW_AT_name("W5_ACCU_B")
	.dwattr $C$DW$1426, DW_AT_TI_symbol_name("_W5_ACCU_B")
	.dwattr $C$DW$1426, DW_AT_data_member_location[DW_OP_plus_uconst 0xc8]
	.dwattr $C$DW$1426, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1427	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1427, DW_AT_name("LUT0")
	.dwattr $C$DW$1427, DW_AT_TI_symbol_name("_LUT0")
	.dwattr $C$DW$1427, DW_AT_data_member_location[DW_OP_plus_uconst 0xcc]
	.dwattr $C$DW$1427, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1428	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1428, DW_AT_name("LUT1")
	.dwattr $C$DW$1428, DW_AT_TI_symbol_name("_LUT1")
	.dwattr $C$DW$1428, DW_AT_data_member_location[DW_OP_plus_uconst 0xd0]
	.dwattr $C$DW$1428, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1429	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1429, DW_AT_name("LUT2")
	.dwattr $C$DW$1429, DW_AT_TI_symbol_name("_LUT2")
	.dwattr $C$DW$1429, DW_AT_data_member_location[DW_OP_plus_uconst 0xd4]
	.dwattr $C$DW$1429, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1430	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1430, DW_AT_name("LUT3")
	.dwattr $C$DW$1430, DW_AT_TI_symbol_name("_LUT3")
	.dwattr $C$DW$1430, DW_AT_data_member_location[DW_OP_plus_uconst 0xd8]
	.dwattr $C$DW$1430, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1431	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1431, DW_AT_name("LUT4")
	.dwattr $C$DW$1431, DW_AT_TI_symbol_name("_LUT4")
	.dwattr $C$DW$1431, DW_AT_data_member_location[DW_OP_plus_uconst 0xdc]
	.dwattr $C$DW$1431, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1432	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1432, DW_AT_name("LUT5")
	.dwattr $C$DW$1432, DW_AT_TI_symbol_name("_LUT5")
	.dwattr $C$DW$1432, DW_AT_data_member_location[DW_OP_plus_uconst 0xe0]
	.dwattr $C$DW$1432, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1433	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1433, DW_AT_name("LUT6")
	.dwattr $C$DW$1433, DW_AT_TI_symbol_name("_LUT6")
	.dwattr $C$DW$1433, DW_AT_data_member_location[DW_OP_plus_uconst 0xe4]
	.dwattr $C$DW$1433, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1434	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1434, DW_AT_name("LUT7")
	.dwattr $C$DW$1434, DW_AT_TI_symbol_name("_LUT7")
	.dwattr $C$DW$1434, DW_AT_data_member_location[DW_OP_plus_uconst 0xe8]
	.dwattr $C$DW$1434, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1435	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1435, DW_AT_name("LUT8")
	.dwattr $C$DW$1435, DW_AT_TI_symbol_name("_LUT8")
	.dwattr $C$DW$1435, DW_AT_data_member_location[DW_OP_plus_uconst 0xec]
	.dwattr $C$DW$1435, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1436	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1436, DW_AT_name("LUT9")
	.dwattr $C$DW$1436, DW_AT_TI_symbol_name("_LUT9")
	.dwattr $C$DW$1436, DW_AT_data_member_location[DW_OP_plus_uconst 0xf0]
	.dwattr $C$DW$1436, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1437	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1437, DW_AT_name("LUT10")
	.dwattr $C$DW$1437, DW_AT_TI_symbol_name("_LUT10")
	.dwattr $C$DW$1437, DW_AT_data_member_location[DW_OP_plus_uconst 0xf4]
	.dwattr $C$DW$1437, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1438	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1438, DW_AT_name("LUT11")
	.dwattr $C$DW$1438, DW_AT_TI_symbol_name("_LUT11")
	.dwattr $C$DW$1438, DW_AT_data_member_location[DW_OP_plus_uconst 0xf8]
	.dwattr $C$DW$1438, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1439	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1439, DW_AT_name("LUT12")
	.dwattr $C$DW$1439, DW_AT_TI_symbol_name("_LUT12")
	.dwattr $C$DW$1439, DW_AT_data_member_location[DW_OP_plus_uconst 0xfc]
	.dwattr $C$DW$1439, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1440	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1440, DW_AT_name("LUT13")
	.dwattr $C$DW$1440, DW_AT_TI_symbol_name("_LUT13")
	.dwattr $C$DW$1440, DW_AT_data_member_location[DW_OP_plus_uconst 0x100]
	.dwattr $C$DW$1440, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1441	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1441, DW_AT_name("LUT14")
	.dwattr $C$DW$1441, DW_AT_TI_symbol_name("_LUT14")
	.dwattr $C$DW$1441, DW_AT_data_member_location[DW_OP_plus_uconst 0x104]
	.dwattr $C$DW$1441, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1442	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1442, DW_AT_name("LUT15")
	.dwattr $C$DW$1442, DW_AT_TI_symbol_name("_LUT15")
	.dwattr $C$DW$1442, DW_AT_data_member_location[DW_OP_plus_uconst 0x108]
	.dwattr $C$DW$1442, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1443	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1443, DW_AT_name("LUT16")
	.dwattr $C$DW$1443, DW_AT_TI_symbol_name("_LUT16")
	.dwattr $C$DW$1443, DW_AT_data_member_location[DW_OP_plus_uconst 0x10c]
	.dwattr $C$DW$1443, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1444	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1444, DW_AT_name("LUT17")
	.dwattr $C$DW$1444, DW_AT_TI_symbol_name("_LUT17")
	.dwattr $C$DW$1444, DW_AT_data_member_location[DW_OP_plus_uconst 0x110]
	.dwattr $C$DW$1444, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1445	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1445, DW_AT_name("LUT18")
	.dwattr $C$DW$1445, DW_AT_TI_symbol_name("_LUT18")
	.dwattr $C$DW$1445, DW_AT_data_member_location[DW_OP_plus_uconst 0x114]
	.dwattr $C$DW$1445, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1446	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1446, DW_AT_name("LUT19")
	.dwattr $C$DW$1446, DW_AT_TI_symbol_name("_LUT19")
	.dwattr $C$DW$1446, DW_AT_data_member_location[DW_OP_plus_uconst 0x118]
	.dwattr $C$DW$1446, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1447	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1447, DW_AT_name("LUT20")
	.dwattr $C$DW$1447, DW_AT_TI_symbol_name("_LUT20")
	.dwattr $C$DW$1447, DW_AT_data_member_location[DW_OP_plus_uconst 0x11c]
	.dwattr $C$DW$1447, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1448	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1448, DW_AT_name("LUT21")
	.dwattr $C$DW$1448, DW_AT_TI_symbol_name("_LUT21")
	.dwattr $C$DW$1448, DW_AT_data_member_location[DW_OP_plus_uconst 0x120]
	.dwattr $C$DW$1448, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1449	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1449, DW_AT_name("LUT22")
	.dwattr $C$DW$1449, DW_AT_TI_symbol_name("_LUT22")
	.dwattr $C$DW$1449, DW_AT_data_member_location[DW_OP_plus_uconst 0x124]
	.dwattr $C$DW$1449, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1450	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1450, DW_AT_name("LUT23")
	.dwattr $C$DW$1450, DW_AT_TI_symbol_name("_LUT23")
	.dwattr $C$DW$1450, DW_AT_data_member_location[DW_OP_plus_uconst 0x128]
	.dwattr $C$DW$1450, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1451	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1451, DW_AT_name("LUT24")
	.dwattr $C$DW$1451, DW_AT_TI_symbol_name("_LUT24")
	.dwattr $C$DW$1451, DW_AT_data_member_location[DW_OP_plus_uconst 0x12c]
	.dwattr $C$DW$1451, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1452	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1452, DW_AT_name("LUT25")
	.dwattr $C$DW$1452, DW_AT_TI_symbol_name("_LUT25")
	.dwattr $C$DW$1452, DW_AT_data_member_location[DW_OP_plus_uconst 0x130]
	.dwattr $C$DW$1452, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1453	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1453, DW_AT_name("LUT26")
	.dwattr $C$DW$1453, DW_AT_TI_symbol_name("_LUT26")
	.dwattr $C$DW$1453, DW_AT_data_member_location[DW_OP_plus_uconst 0x134]
	.dwattr $C$DW$1453, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1454	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1454, DW_AT_name("LUT27")
	.dwattr $C$DW$1454, DW_AT_TI_symbol_name("_LUT27")
	.dwattr $C$DW$1454, DW_AT_data_member_location[DW_OP_plus_uconst 0x138]
	.dwattr $C$DW$1454, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1455	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1455, DW_AT_name("LUT28")
	.dwattr $C$DW$1455, DW_AT_TI_symbol_name("_LUT28")
	.dwattr $C$DW$1455, DW_AT_data_member_location[DW_OP_plus_uconst 0x13c]
	.dwattr $C$DW$1455, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1456	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1456, DW_AT_name("LUT29")
	.dwattr $C$DW$1456, DW_AT_TI_symbol_name("_LUT29")
	.dwattr $C$DW$1456, DW_AT_data_member_location[DW_OP_plus_uconst 0x140]
	.dwattr $C$DW$1456, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1457	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1457, DW_AT_name("LUT30")
	.dwattr $C$DW$1457, DW_AT_TI_symbol_name("_LUT30")
	.dwattr $C$DW$1457, DW_AT_data_member_location[DW_OP_plus_uconst 0x144]
	.dwattr $C$DW$1457, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1458	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1458, DW_AT_name("LUT31")
	.dwattr $C$DW$1458, DW_AT_TI_symbol_name("_LUT31")
	.dwattr $C$DW$1458, DW_AT_data_member_location[DW_OP_plus_uconst 0x148]
	.dwattr $C$DW$1458, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1459	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1459, DW_AT_name("LUT32")
	.dwattr $C$DW$1459, DW_AT_TI_symbol_name("_LUT32")
	.dwattr $C$DW$1459, DW_AT_data_member_location[DW_OP_plus_uconst 0x14c]
	.dwattr $C$DW$1459, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1460	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1460, DW_AT_name("LUT33")
	.dwattr $C$DW$1460, DW_AT_TI_symbol_name("_LUT33")
	.dwattr $C$DW$1460, DW_AT_data_member_location[DW_OP_plus_uconst 0x150]
	.dwattr $C$DW$1460, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1461	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1461, DW_AT_name("LUT34")
	.dwattr $C$DW$1461, DW_AT_TI_symbol_name("_LUT34")
	.dwattr $C$DW$1461, DW_AT_data_member_location[DW_OP_plus_uconst 0x154]
	.dwattr $C$DW$1461, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1462	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1462, DW_AT_name("LUT35")
	.dwattr $C$DW$1462, DW_AT_TI_symbol_name("_LUT35")
	.dwattr $C$DW$1462, DW_AT_data_member_location[DW_OP_plus_uconst 0x158]
	.dwattr $C$DW$1462, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1463	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1463, DW_AT_name("LUT36")
	.dwattr $C$DW$1463, DW_AT_TI_symbol_name("_LUT36")
	.dwattr $C$DW$1463, DW_AT_data_member_location[DW_OP_plus_uconst 0x15c]
	.dwattr $C$DW$1463, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1464	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1464, DW_AT_name("LUT37")
	.dwattr $C$DW$1464, DW_AT_TI_symbol_name("_LUT37")
	.dwattr $C$DW$1464, DW_AT_data_member_location[DW_OP_plus_uconst 0x160]
	.dwattr $C$DW$1464, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1465	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1465, DW_AT_name("LUT38")
	.dwattr $C$DW$1465, DW_AT_TI_symbol_name("_LUT38")
	.dwattr $C$DW$1465, DW_AT_data_member_location[DW_OP_plus_uconst 0x164]
	.dwattr $C$DW$1465, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1466	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1466, DW_AT_name("LUT39")
	.dwattr $C$DW$1466, DW_AT_TI_symbol_name("_LUT39")
	.dwattr $C$DW$1466, DW_AT_data_member_location[DW_OP_plus_uconst 0x168]
	.dwattr $C$DW$1466, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1467	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1467, DW_AT_name("LUT40")
	.dwattr $C$DW$1467, DW_AT_TI_symbol_name("_LUT40")
	.dwattr $C$DW$1467, DW_AT_data_member_location[DW_OP_plus_uconst 0x16c]
	.dwattr $C$DW$1467, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1468	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1468, DW_AT_name("LUT41")
	.dwattr $C$DW$1468, DW_AT_TI_symbol_name("_LUT41")
	.dwattr $C$DW$1468, DW_AT_data_member_location[DW_OP_plus_uconst 0x170]
	.dwattr $C$DW$1468, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1469	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1469, DW_AT_name("LUT42")
	.dwattr $C$DW$1469, DW_AT_TI_symbol_name("_LUT42")
	.dwattr $C$DW$1469, DW_AT_data_member_location[DW_OP_plus_uconst 0x174]
	.dwattr $C$DW$1469, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1470	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1470, DW_AT_name("LUT43")
	.dwattr $C$DW$1470, DW_AT_TI_symbol_name("_LUT43")
	.dwattr $C$DW$1470, DW_AT_data_member_location[DW_OP_plus_uconst 0x178]
	.dwattr $C$DW$1470, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1471	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1471, DW_AT_name("LUT44")
	.dwattr $C$DW$1471, DW_AT_TI_symbol_name("_LUT44")
	.dwattr $C$DW$1471, DW_AT_data_member_location[DW_OP_plus_uconst 0x17c]
	.dwattr $C$DW$1471, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1472	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1472, DW_AT_name("LUT45")
	.dwattr $C$DW$1472, DW_AT_TI_symbol_name("_LUT45")
	.dwattr $C$DW$1472, DW_AT_data_member_location[DW_OP_plus_uconst 0x180]
	.dwattr $C$DW$1472, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1473	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1473, DW_AT_name("LUT46")
	.dwattr $C$DW$1473, DW_AT_TI_symbol_name("_LUT46")
	.dwattr $C$DW$1473, DW_AT_data_member_location[DW_OP_plus_uconst 0x184]
	.dwattr $C$DW$1473, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1474	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1474, DW_AT_name("LUT47")
	.dwattr $C$DW$1474, DW_AT_TI_symbol_name("_LUT47")
	.dwattr $C$DW$1474, DW_AT_data_member_location[DW_OP_plus_uconst 0x188]
	.dwattr $C$DW$1474, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1475	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1475, DW_AT_name("LUT48")
	.dwattr $C$DW$1475, DW_AT_TI_symbol_name("_LUT48")
	.dwattr $C$DW$1475, DW_AT_data_member_location[DW_OP_plus_uconst 0x18c]
	.dwattr $C$DW$1475, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1476	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1476, DW_AT_name("LUT49")
	.dwattr $C$DW$1476, DW_AT_TI_symbol_name("_LUT49")
	.dwattr $C$DW$1476, DW_AT_data_member_location[DW_OP_plus_uconst 0x190]
	.dwattr $C$DW$1476, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1477	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1477, DW_AT_name("LUT50")
	.dwattr $C$DW$1477, DW_AT_TI_symbol_name("_LUT50")
	.dwattr $C$DW$1477, DW_AT_data_member_location[DW_OP_plus_uconst 0x194]
	.dwattr $C$DW$1477, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1478	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1478, DW_AT_name("LUT51")
	.dwattr $C$DW$1478, DW_AT_TI_symbol_name("_LUT51")
	.dwattr $C$DW$1478, DW_AT_data_member_location[DW_OP_plus_uconst 0x198]
	.dwattr $C$DW$1478, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1479	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1479, DW_AT_name("LUT52")
	.dwattr $C$DW$1479, DW_AT_TI_symbol_name("_LUT52")
	.dwattr $C$DW$1479, DW_AT_data_member_location[DW_OP_plus_uconst 0x19c]
	.dwattr $C$DW$1479, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1480	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1480, DW_AT_name("LUT53")
	.dwattr $C$DW$1480, DW_AT_TI_symbol_name("_LUT53")
	.dwattr $C$DW$1480, DW_AT_data_member_location[DW_OP_plus_uconst 0x1a0]
	.dwattr $C$DW$1480, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1481	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1481, DW_AT_name("LUT54")
	.dwattr $C$DW$1481, DW_AT_TI_symbol_name("_LUT54")
	.dwattr $C$DW$1481, DW_AT_data_member_location[DW_OP_plus_uconst 0x1a4]
	.dwattr $C$DW$1481, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1482	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1482, DW_AT_name("LUT55")
	.dwattr $C$DW$1482, DW_AT_TI_symbol_name("_LUT55")
	.dwattr $C$DW$1482, DW_AT_data_member_location[DW_OP_plus_uconst 0x1a8]
	.dwattr $C$DW$1482, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1483	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1483, DW_AT_name("LUT56")
	.dwattr $C$DW$1483, DW_AT_TI_symbol_name("_LUT56")
	.dwattr $C$DW$1483, DW_AT_data_member_location[DW_OP_plus_uconst 0x1ac]
	.dwattr $C$DW$1483, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1484	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1484, DW_AT_name("LUT57")
	.dwattr $C$DW$1484, DW_AT_TI_symbol_name("_LUT57")
	.dwattr $C$DW$1484, DW_AT_data_member_location[DW_OP_plus_uconst 0x1b0]
	.dwattr $C$DW$1484, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1485	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1485, DW_AT_name("LUT58")
	.dwattr $C$DW$1485, DW_AT_TI_symbol_name("_LUT58")
	.dwattr $C$DW$1485, DW_AT_data_member_location[DW_OP_plus_uconst 0x1b4]
	.dwattr $C$DW$1485, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1486	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1486, DW_AT_name("LUT59")
	.dwattr $C$DW$1486, DW_AT_TI_symbol_name("_LUT59")
	.dwattr $C$DW$1486, DW_AT_data_member_location[DW_OP_plus_uconst 0x1b8]
	.dwattr $C$DW$1486, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1487	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1487, DW_AT_name("LUT60")
	.dwattr $C$DW$1487, DW_AT_TI_symbol_name("_LUT60")
	.dwattr $C$DW$1487, DW_AT_data_member_location[DW_OP_plus_uconst 0x1bc]
	.dwattr $C$DW$1487, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1488	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1488, DW_AT_name("LUT61")
	.dwattr $C$DW$1488, DW_AT_TI_symbol_name("_LUT61")
	.dwattr $C$DW$1488, DW_AT_data_member_location[DW_OP_plus_uconst 0x1c0]
	.dwattr $C$DW$1488, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1489	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1489, DW_AT_name("LUT62")
	.dwattr $C$DW$1489, DW_AT_TI_symbol_name("_LUT62")
	.dwattr $C$DW$1489, DW_AT_data_member_location[DW_OP_plus_uconst 0x1c4]
	.dwattr $C$DW$1489, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1490	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1490, DW_AT_name("LUT63")
	.dwattr $C$DW$1490, DW_AT_TI_symbol_name("_LUT63")
	.dwattr $C$DW$1490, DW_AT_data_member_location[DW_OP_plus_uconst 0x1c8]
	.dwattr $C$DW$1490, DW_AT_accessibility(DW_ACCESS_public)
	.dwendtag $C$DW$T$142

$C$DW$T$322	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_VfocRegs")
	.dwattr $C$DW$T$322, DW_AT_type(*$C$DW$T$142)
	.dwattr $C$DW$T$322, DW_AT_language(DW_LANG_C)
$C$DW$T$323	.dwtag  DW_TAG_volatile_type
	.dwattr $C$DW$T$323, DW_AT_type(*$C$DW$T$322)
$C$DW$T$324	.dwtag  DW_TAG_pointer_type, DW_AT_type(*$C$DW$T$323)
	.dwattr $C$DW$T$324, DW_AT_address_class(0x20)
$C$DW$T$325	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_VfocRegsOvly")
	.dwattr $C$DW$T$325, DW_AT_type(*$C$DW$T$324)
	.dwattr $C$DW$T$325, DW_AT_language(DW_LANG_C)

$C$DW$T$147	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$147, DW_AT_byte_size(0x3fc)
$C$DW$1491	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$64)
	.dwattr $C$DW$1491, DW_AT_name("RSVD0")
	.dwattr $C$DW$1491, DW_AT_TI_symbol_name("_RSVD0")
	.dwattr $C$DW$1491, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1491, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1492	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1492, DW_AT_name("PCISTATSET")
	.dwattr $C$DW$1492, DW_AT_TI_symbol_name("_PCISTATSET")
	.dwattr $C$DW$1492, DW_AT_data_member_location[DW_OP_plus_uconst 0x10]
	.dwattr $C$DW$1492, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1493	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1493, DW_AT_name("PCISTATCLR")
	.dwattr $C$DW$1493, DW_AT_TI_symbol_name("_PCISTATCLR")
	.dwattr $C$DW$1493, DW_AT_data_member_location[DW_OP_plus_uconst 0x14]
	.dwattr $C$DW$1493, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1494	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$62)
	.dwattr $C$DW$1494, DW_AT_name("RSVD1")
	.dwattr $C$DW$1494, DW_AT_TI_symbol_name("_RSVD1")
	.dwattr $C$DW$1494, DW_AT_data_member_location[DW_OP_plus_uconst 0x18]
	.dwattr $C$DW$1494, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1495	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1495, DW_AT_name("PCIHINTSET")
	.dwattr $C$DW$1495, DW_AT_TI_symbol_name("_PCIHINTSET")
	.dwattr $C$DW$1495, DW_AT_data_member_location[DW_OP_plus_uconst 0x20]
	.dwattr $C$DW$1495, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1496	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1496, DW_AT_name("PCIHINTCLR")
	.dwattr $C$DW$1496, DW_AT_TI_symbol_name("_PCIHINTCLR")
	.dwattr $C$DW$1496, DW_AT_data_member_location[DW_OP_plus_uconst 0x24]
	.dwattr $C$DW$1496, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1497	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$62)
	.dwattr $C$DW$1497, DW_AT_name("RSVD2")
	.dwattr $C$DW$1497, DW_AT_TI_symbol_name("_RSVD2")
	.dwattr $C$DW$1497, DW_AT_data_member_location[DW_OP_plus_uconst 0x28]
	.dwattr $C$DW$1497, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1498	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1498, DW_AT_name("PCIBINTSET")
	.dwattr $C$DW$1498, DW_AT_TI_symbol_name("_PCIBINTSET")
	.dwattr $C$DW$1498, DW_AT_data_member_location[DW_OP_plus_uconst 0x30]
	.dwattr $C$DW$1498, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1499	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1499, DW_AT_name("PCIBINTCLR")
	.dwattr $C$DW$1499, DW_AT_TI_symbol_name("_PCIBINTCLR")
	.dwattr $C$DW$1499, DW_AT_data_member_location[DW_OP_plus_uconst 0x34]
	.dwattr $C$DW$1499, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1500	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1500, DW_AT_name("PCIBCLKMGT")
	.dwattr $C$DW$1500, DW_AT_TI_symbol_name("_PCIBCLKMGT")
	.dwattr $C$DW$1500, DW_AT_data_member_location[DW_OP_plus_uconst 0x38]
	.dwattr $C$DW$1500, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1501	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$143)
	.dwattr $C$DW$1501, DW_AT_name("RSVD3")
	.dwattr $C$DW$1501, DW_AT_TI_symbol_name("_RSVD3")
	.dwattr $C$DW$1501, DW_AT_data_member_location[DW_OP_plus_uconst 0x3c]
	.dwattr $C$DW$1501, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1502	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1502, DW_AT_name("PCIVENDEVMIR")
	.dwattr $C$DW$1502, DW_AT_TI_symbol_name("_PCIVENDEVMIR")
	.dwattr $C$DW$1502, DW_AT_data_member_location[DW_OP_plus_uconst 0x100]
	.dwattr $C$DW$1502, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1503	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1503, DW_AT_name("PCICSRMIR")
	.dwattr $C$DW$1503, DW_AT_TI_symbol_name("_PCICSRMIR")
	.dwattr $C$DW$1503, DW_AT_data_member_location[DW_OP_plus_uconst 0x104]
	.dwattr $C$DW$1503, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1504	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1504, DW_AT_name("PCICLREVMIR")
	.dwattr $C$DW$1504, DW_AT_TI_symbol_name("_PCICLREVMIR")
	.dwattr $C$DW$1504, DW_AT_data_member_location[DW_OP_plus_uconst 0x108]
	.dwattr $C$DW$1504, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1505	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1505, DW_AT_name("PCICLINEMIR")
	.dwattr $C$DW$1505, DW_AT_TI_symbol_name("_PCICLINEMIR")
	.dwattr $C$DW$1505, DW_AT_data_member_location[DW_OP_plus_uconst 0x10c]
	.dwattr $C$DW$1505, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1506	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1506, DW_AT_name("PCIBAR0MSK")
	.dwattr $C$DW$1506, DW_AT_TI_symbol_name("_PCIBAR0MSK")
	.dwattr $C$DW$1506, DW_AT_data_member_location[DW_OP_plus_uconst 0x110]
	.dwattr $C$DW$1506, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1507	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1507, DW_AT_name("PCIBAR1MSK")
	.dwattr $C$DW$1507, DW_AT_TI_symbol_name("_PCIBAR1MSK")
	.dwattr $C$DW$1507, DW_AT_data_member_location[DW_OP_plus_uconst 0x114]
	.dwattr $C$DW$1507, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1508	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1508, DW_AT_name("PCIBAR2MSK")
	.dwattr $C$DW$1508, DW_AT_TI_symbol_name("_PCIBAR2MSK")
	.dwattr $C$DW$1508, DW_AT_data_member_location[DW_OP_plus_uconst 0x118]
	.dwattr $C$DW$1508, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1509	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1509, DW_AT_name("PCIBAR3MSK")
	.dwattr $C$DW$1509, DW_AT_TI_symbol_name("_PCIBAR3MSK")
	.dwattr $C$DW$1509, DW_AT_data_member_location[DW_OP_plus_uconst 0x11c]
	.dwattr $C$DW$1509, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1510	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1510, DW_AT_name("PCIBAR4MSK")
	.dwattr $C$DW$1510, DW_AT_TI_symbol_name("_PCIBAR4MSK")
	.dwattr $C$DW$1510, DW_AT_data_member_location[DW_OP_plus_uconst 0x120]
	.dwattr $C$DW$1510, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1511	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1511, DW_AT_name("PCIBAR5MSK")
	.dwattr $C$DW$1511, DW_AT_TI_symbol_name("_PCIBAR5MSK")
	.dwattr $C$DW$1511, DW_AT_data_member_location[DW_OP_plus_uconst 0x124]
	.dwattr $C$DW$1511, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1512	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$55)
	.dwattr $C$DW$1512, DW_AT_name("RSVD4")
	.dwattr $C$DW$1512, DW_AT_TI_symbol_name("_RSVD4")
	.dwattr $C$DW$1512, DW_AT_data_member_location[DW_OP_plus_uconst 0x128]
	.dwattr $C$DW$1512, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1513	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1513, DW_AT_name("PCISUBIDMIR")
	.dwattr $C$DW$1513, DW_AT_TI_symbol_name("_PCISUBIDMIR")
	.dwattr $C$DW$1513, DW_AT_data_member_location[DW_OP_plus_uconst 0x12c]
	.dwattr $C$DW$1513, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1514	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$55)
	.dwattr $C$DW$1514, DW_AT_name("RSVD5")
	.dwattr $C$DW$1514, DW_AT_TI_symbol_name("_RSVD5")
	.dwattr $C$DW$1514, DW_AT_data_member_location[DW_OP_plus_uconst 0x130]
	.dwattr $C$DW$1514, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1515	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1515, DW_AT_name("PCICPBPTRMIR")
	.dwattr $C$DW$1515, DW_AT_TI_symbol_name("_PCICPBPTRMIR")
	.dwattr $C$DW$1515, DW_AT_data_member_location[DW_OP_plus_uconst 0x134]
	.dwattr $C$DW$1515, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1516	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$55)
	.dwattr $C$DW$1516, DW_AT_name("RSVD6")
	.dwattr $C$DW$1516, DW_AT_TI_symbol_name("_RSVD6")
	.dwattr $C$DW$1516, DW_AT_data_member_location[DW_OP_plus_uconst 0x138]
	.dwattr $C$DW$1516, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1517	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1517, DW_AT_name("PCILGINTMIR")
	.dwattr $C$DW$1517, DW_AT_TI_symbol_name("_PCILGINTMIR")
	.dwattr $C$DW$1517, DW_AT_data_member_location[DW_OP_plus_uconst 0x13c]
	.dwattr $C$DW$1517, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1518	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$105)
	.dwattr $C$DW$1518, DW_AT_name("RSVD7")
	.dwattr $C$DW$1518, DW_AT_TI_symbol_name("_RSVD7")
	.dwattr $C$DW$1518, DW_AT_data_member_location[DW_OP_plus_uconst 0x140]
	.dwattr $C$DW$1518, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1519	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1519, DW_AT_name("PCISLVCNTL")
	.dwattr $C$DW$1519, DW_AT_TI_symbol_name("_PCISLVCNTL")
	.dwattr $C$DW$1519, DW_AT_data_member_location[DW_OP_plus_uconst 0x180]
	.dwattr $C$DW$1519, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1520	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$144)
	.dwattr $C$DW$1520, DW_AT_name("RSVD8")
	.dwattr $C$DW$1520, DW_AT_TI_symbol_name("_RSVD8")
	.dwattr $C$DW$1520, DW_AT_data_member_location[DW_OP_plus_uconst 0x184]
	.dwattr $C$DW$1520, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1521	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1521, DW_AT_name("PCIBAR0TRL")
	.dwattr $C$DW$1521, DW_AT_TI_symbol_name("_PCIBAR0TRL")
	.dwattr $C$DW$1521, DW_AT_data_member_location[DW_OP_plus_uconst 0x1c0]
	.dwattr $C$DW$1521, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1522	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1522, DW_AT_name("PCIBAR1TRL")
	.dwattr $C$DW$1522, DW_AT_TI_symbol_name("_PCIBAR1TRL")
	.dwattr $C$DW$1522, DW_AT_data_member_location[DW_OP_plus_uconst 0x1c4]
	.dwattr $C$DW$1522, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1523	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1523, DW_AT_name("PCIBAR2TRL")
	.dwattr $C$DW$1523, DW_AT_TI_symbol_name("_PCIBAR2TRL")
	.dwattr $C$DW$1523, DW_AT_data_member_location[DW_OP_plus_uconst 0x1c8]
	.dwattr $C$DW$1523, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1524	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1524, DW_AT_name("PCIBAR3TRL")
	.dwattr $C$DW$1524, DW_AT_TI_symbol_name("_PCIBAR3TRL")
	.dwattr $C$DW$1524, DW_AT_data_member_location[DW_OP_plus_uconst 0x1cc]
	.dwattr $C$DW$1524, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1525	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1525, DW_AT_name("PCIBAR4TRL")
	.dwattr $C$DW$1525, DW_AT_TI_symbol_name("_PCIBAR4TRL")
	.dwattr $C$DW$1525, DW_AT_data_member_location[DW_OP_plus_uconst 0x1d0]
	.dwattr $C$DW$1525, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1526	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1526, DW_AT_name("PCIBAR5TRL")
	.dwattr $C$DW$1526, DW_AT_TI_symbol_name("_PCIBAR5TRL")
	.dwattr $C$DW$1526, DW_AT_data_member_location[DW_OP_plus_uconst 0x1d4]
	.dwattr $C$DW$1526, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1527	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$62)
	.dwattr $C$DW$1527, DW_AT_name("RSVD9")
	.dwattr $C$DW$1527, DW_AT_TI_symbol_name("_RSVD9")
	.dwattr $C$DW$1527, DW_AT_data_member_location[DW_OP_plus_uconst 0x1d8]
	.dwattr $C$DW$1527, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1528	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$145)
	.dwattr $C$DW$1528, DW_AT_name("PCIBARMIR")
	.dwattr $C$DW$1528, DW_AT_TI_symbol_name("_PCIBARMIR")
	.dwattr $C$DW$1528, DW_AT_data_member_location[DW_OP_plus_uconst 0x1e0]
	.dwattr $C$DW$1528, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1529	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$146)
	.dwattr $C$DW$1529, DW_AT_name("RSVD10")
	.dwattr $C$DW$1529, DW_AT_TI_symbol_name("_RSVD10")
	.dwattr $C$DW$1529, DW_AT_data_member_location[DW_OP_plus_uconst 0x1f8]
	.dwattr $C$DW$1529, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1530	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1530, DW_AT_name("PCIMCFGDAT")
	.dwattr $C$DW$1530, DW_AT_TI_symbol_name("_PCIMCFGDAT")
	.dwattr $C$DW$1530, DW_AT_data_member_location[DW_OP_plus_uconst 0x300]
	.dwattr $C$DW$1530, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1531	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1531, DW_AT_name("PCIMCFGADR")
	.dwattr $C$DW$1531, DW_AT_TI_symbol_name("_PCIMCFGADR")
	.dwattr $C$DW$1531, DW_AT_data_member_location[DW_OP_plus_uconst 0x304]
	.dwattr $C$DW$1531, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1532	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1532, DW_AT_name("PCIMCFGCMD")
	.dwattr $C$DW$1532, DW_AT_TI_symbol_name("_PCIMCFGCMD")
	.dwattr $C$DW$1532, DW_AT_data_member_location[DW_OP_plus_uconst 0x308]
	.dwattr $C$DW$1532, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1533	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$55)
	.dwattr $C$DW$1533, DW_AT_name("RSVD11")
	.dwattr $C$DW$1533, DW_AT_TI_symbol_name("_RSVD11")
	.dwattr $C$DW$1533, DW_AT_data_member_location[DW_OP_plus_uconst 0x30c]
	.dwattr $C$DW$1533, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1534	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1534, DW_AT_name("PCIMSTCFG")
	.dwattr $C$DW$1534, DW_AT_TI_symbol_name("_PCIMSTCFG")
	.dwattr $C$DW$1534, DW_AT_data_member_location[DW_OP_plus_uconst 0x310]
	.dwattr $C$DW$1534, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1535	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$135)
	.dwattr $C$DW$1535, DW_AT_name("PCIADDSUB")
	.dwattr $C$DW$1535, DW_AT_TI_symbol_name("_PCIADDSUB")
	.dwattr $C$DW$1535, DW_AT_data_member_location[DW_OP_plus_uconst 0x314]
	.dwattr $C$DW$1535, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1536	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1536, DW_AT_name("PCIVENDEVPRG")
	.dwattr $C$DW$1536, DW_AT_TI_symbol_name("_PCIVENDEVPRG")
	.dwattr $C$DW$1536, DW_AT_data_member_location[DW_OP_plus_uconst 0x394]
	.dwattr $C$DW$1536, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1537	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1537, DW_AT_name("PCICMDSTATPRG")
	.dwattr $C$DW$1537, DW_AT_TI_symbol_name("_PCICMDSTATPRG")
	.dwattr $C$DW$1537, DW_AT_data_member_location[DW_OP_plus_uconst 0x398]
	.dwattr $C$DW$1537, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1538	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1538, DW_AT_name("PCICLREVPRG")
	.dwattr $C$DW$1538, DW_AT_TI_symbol_name("_PCICLREVPRG")
	.dwattr $C$DW$1538, DW_AT_data_member_location[DW_OP_plus_uconst 0x39c]
	.dwattr $C$DW$1538, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1539	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1539, DW_AT_name("PCISUBIDPRG")
	.dwattr $C$DW$1539, DW_AT_TI_symbol_name("_PCISUBIDPRG")
	.dwattr $C$DW$1539, DW_AT_data_member_location[DW_OP_plus_uconst 0x3a0]
	.dwattr $C$DW$1539, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1540	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1540, DW_AT_name("PCIMAXLGPRG")
	.dwattr $C$DW$1540, DW_AT_TI_symbol_name("_PCIMAXLGPRG")
	.dwattr $C$DW$1540, DW_AT_data_member_location[DW_OP_plus_uconst 0x3a4]
	.dwattr $C$DW$1540, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1541	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1541, DW_AT_name("PCILRSTREG")
	.dwattr $C$DW$1541, DW_AT_TI_symbol_name("_PCILRSTREG")
	.dwattr $C$DW$1541, DW_AT_data_member_location[DW_OP_plus_uconst 0x3a8]
	.dwattr $C$DW$1541, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1542	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1542, DW_AT_name("PCICFGDONE")
	.dwattr $C$DW$1542, DW_AT_TI_symbol_name("_PCICFGDONE")
	.dwattr $C$DW$1542, DW_AT_data_member_location[DW_OP_plus_uconst 0x3ac]
	.dwattr $C$DW$1542, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1543	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1543, DW_AT_name("PCIBAR0MPRG")
	.dwattr $C$DW$1543, DW_AT_TI_symbol_name("_PCIBAR0MPRG")
	.dwattr $C$DW$1543, DW_AT_data_member_location[DW_OP_plus_uconst 0x3b0]
	.dwattr $C$DW$1543, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1544	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1544, DW_AT_name("PCIBAR1MPRG")
	.dwattr $C$DW$1544, DW_AT_TI_symbol_name("_PCIBAR1MPRG")
	.dwattr $C$DW$1544, DW_AT_data_member_location[DW_OP_plus_uconst 0x3b4]
	.dwattr $C$DW$1544, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1545	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1545, DW_AT_name("PCIBAR2MPRG")
	.dwattr $C$DW$1545, DW_AT_TI_symbol_name("_PCIBAR2MPRG")
	.dwattr $C$DW$1545, DW_AT_data_member_location[DW_OP_plus_uconst 0x3b8]
	.dwattr $C$DW$1545, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1546	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1546, DW_AT_name("PCIBAR3MPRG")
	.dwattr $C$DW$1546, DW_AT_TI_symbol_name("_PCIBAR3MPRG")
	.dwattr $C$DW$1546, DW_AT_data_member_location[DW_OP_plus_uconst 0x3bc]
	.dwattr $C$DW$1546, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1547	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1547, DW_AT_name("PCIBAR4MPRG")
	.dwattr $C$DW$1547, DW_AT_TI_symbol_name("_PCIBAR4MPRG")
	.dwattr $C$DW$1547, DW_AT_data_member_location[DW_OP_plus_uconst 0x3c0]
	.dwattr $C$DW$1547, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1548	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1548, DW_AT_name("PCIBAR5MPRG")
	.dwattr $C$DW$1548, DW_AT_TI_symbol_name("_PCIBAR5MPRG")
	.dwattr $C$DW$1548, DW_AT_data_member_location[DW_OP_plus_uconst 0x3c4]
	.dwattr $C$DW$1548, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1549	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1549, DW_AT_name("PCIBAR0PRG")
	.dwattr $C$DW$1549, DW_AT_TI_symbol_name("_PCIBAR0PRG")
	.dwattr $C$DW$1549, DW_AT_data_member_location[DW_OP_plus_uconst 0x3c8]
	.dwattr $C$DW$1549, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1550	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1550, DW_AT_name("PCIBAR1PRG")
	.dwattr $C$DW$1550, DW_AT_TI_symbol_name("_PCIBAR1PRG")
	.dwattr $C$DW$1550, DW_AT_data_member_location[DW_OP_plus_uconst 0x3cc]
	.dwattr $C$DW$1550, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1551	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1551, DW_AT_name("PCIBAR2PRG")
	.dwattr $C$DW$1551, DW_AT_TI_symbol_name("_PCIBAR2PRG")
	.dwattr $C$DW$1551, DW_AT_data_member_location[DW_OP_plus_uconst 0x3d0]
	.dwattr $C$DW$1551, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1552	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1552, DW_AT_name("PCIBAR3PRG")
	.dwattr $C$DW$1552, DW_AT_TI_symbol_name("_PCIBAR3PRG")
	.dwattr $C$DW$1552, DW_AT_data_member_location[DW_OP_plus_uconst 0x3d4]
	.dwattr $C$DW$1552, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1553	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1553, DW_AT_name("PCIBAR4PRG")
	.dwattr $C$DW$1553, DW_AT_TI_symbol_name("_PCIBAR4PRG")
	.dwattr $C$DW$1553, DW_AT_data_member_location[DW_OP_plus_uconst 0x3d8]
	.dwattr $C$DW$1553, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1554	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1554, DW_AT_name("PCIBAR5PRG")
	.dwattr $C$DW$1554, DW_AT_TI_symbol_name("_PCIBAR5PRG")
	.dwattr $C$DW$1554, DW_AT_data_member_location[DW_OP_plus_uconst 0x3dc]
	.dwattr $C$DW$1554, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1555	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1555, DW_AT_name("PCIBAR0TRLPRG")
	.dwattr $C$DW$1555, DW_AT_TI_symbol_name("_PCIBAR0TRLPRG")
	.dwattr $C$DW$1555, DW_AT_data_member_location[DW_OP_plus_uconst 0x3e0]
	.dwattr $C$DW$1555, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1556	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1556, DW_AT_name("PCIBAR1TRLPRG")
	.dwattr $C$DW$1556, DW_AT_TI_symbol_name("_PCIBAR1TRLPRG")
	.dwattr $C$DW$1556, DW_AT_data_member_location[DW_OP_plus_uconst 0x3e4]
	.dwattr $C$DW$1556, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1557	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1557, DW_AT_name("PCIBAR2TRLPRG")
	.dwattr $C$DW$1557, DW_AT_TI_symbol_name("_PCIBAR2TRLPRG")
	.dwattr $C$DW$1557, DW_AT_data_member_location[DW_OP_plus_uconst 0x3e8]
	.dwattr $C$DW$1557, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1558	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1558, DW_AT_name("PCIBAR3TRLPRG")
	.dwattr $C$DW$1558, DW_AT_TI_symbol_name("_PCIBAR3TRLPRG")
	.dwattr $C$DW$1558, DW_AT_data_member_location[DW_OP_plus_uconst 0x3ec]
	.dwattr $C$DW$1558, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1559	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1559, DW_AT_name("PCIBAR4TRLPRG")
	.dwattr $C$DW$1559, DW_AT_TI_symbol_name("_PCIBAR4TRLPRG")
	.dwattr $C$DW$1559, DW_AT_data_member_location[DW_OP_plus_uconst 0x3f0]
	.dwattr $C$DW$1559, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1560	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1560, DW_AT_name("PCIBAR5TRLPRG")
	.dwattr $C$DW$1560, DW_AT_TI_symbol_name("_PCIBAR5TRLPRG")
	.dwattr $C$DW$1560, DW_AT_data_member_location[DW_OP_plus_uconst 0x3f4]
	.dwattr $C$DW$1560, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1561	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$1561, DW_AT_name("PCIBASENPRG")
	.dwattr $C$DW$1561, DW_AT_TI_symbol_name("_PCIBASENPRG")
	.dwattr $C$DW$1561, DW_AT_data_member_location[DW_OP_plus_uconst 0x3f8]
	.dwattr $C$DW$1561, DW_AT_accessibility(DW_ACCESS_public)
	.dwendtag $C$DW$T$147

$C$DW$T$326	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_PciifRegs")
	.dwattr $C$DW$T$326, DW_AT_type(*$C$DW$T$147)
	.dwattr $C$DW$T$326, DW_AT_language(DW_LANG_C)
$C$DW$T$327	.dwtag  DW_TAG_volatile_type
	.dwattr $C$DW$T$327, DW_AT_type(*$C$DW$T$326)
$C$DW$T$328	.dwtag  DW_TAG_pointer_type, DW_AT_type(*$C$DW$T$327)
	.dwattr $C$DW$T$328, DW_AT_address_class(0x20)
$C$DW$T$329	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_PcictlRegsOvly")
	.dwattr $C$DW$T$329, DW_AT_type(*$C$DW$T$328)
	.dwattr $C$DW$T$329, DW_AT_language(DW_LANG_C)

$C$DW$T$149	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$149, DW_AT_byte_size(0x30)
$C$DW$1562	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$148)
	.dwattr $C$DW$1562, DW_AT_name("chStatusLeft")
	.dwattr $C$DW$1562, DW_AT_TI_symbol_name("_chStatusLeft")
	.dwattr $C$DW$1562, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1562, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1563	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$148)
	.dwattr $C$DW$1563, DW_AT_name("chStatusRight")
	.dwattr $C$DW$1563, DW_AT_TI_symbol_name("_chStatusRight")
	.dwattr $C$DW$1563, DW_AT_data_member_location[DW_OP_plus_uconst 0x18]
	.dwattr $C$DW$1563, DW_AT_accessibility(DW_ACCESS_public)
	.dwendtag $C$DW$T$149

$C$DW$T$394	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_McaspChStatusRam")
	.dwattr $C$DW$T$394, DW_AT_type(*$C$DW$T$149)
	.dwattr $C$DW$T$394, DW_AT_language(DW_LANG_C)

$C$DW$T$150	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$150, DW_AT_byte_size(0x30)
$C$DW$1564	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$148)
	.dwattr $C$DW$1564, DW_AT_name("userDataLeft")
	.dwattr $C$DW$1564, DW_AT_TI_symbol_name("_userDataLeft")
	.dwattr $C$DW$1564, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1564, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1565	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$148)
	.dwattr $C$DW$1565, DW_AT_name("userDataRight")
	.dwattr $C$DW$1565, DW_AT_TI_symbol_name("_userDataRight")
	.dwattr $C$DW$1565, DW_AT_data_member_location[DW_OP_plus_uconst 0x18]
	.dwattr $C$DW$1565, DW_AT_accessibility(DW_ACCESS_public)
	.dwendtag $C$DW$T$150

$C$DW$T$395	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_McaspUserDataRam")
	.dwattr $C$DW$T$395, DW_AT_type(*$C$DW$T$150)
	.dwattr $C$DW$T$395, DW_AT_language(DW_LANG_C)

$C$DW$T$151	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$151, DW_AT_byte_size(0xb8)
$C$DW$1566	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$1566, DW_AT_name("PWRDEMU")
	.dwattr $C$DW$1566, DW_AT_TI_symbol_name("_PWRDEMU")
	.dwattr $C$DW$1566, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1566, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1567	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$1567, DW_AT_name("PFUNC")
	.dwattr $C$DW$1567, DW_AT_TI_symbol_name("_PFUNC")
	.dwattr $C$DW$1567, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$1567, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1568	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$1568, DW_AT_name("PDIR")
	.dwattr $C$DW$1568, DW_AT_TI_symbol_name("_PDIR")
	.dwattr $C$DW$1568, DW_AT_data_member_location[DW_OP_plus_uconst 0x8]
	.dwattr $C$DW$1568, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1569	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$1569, DW_AT_name("PDOUT")
	.dwattr $C$DW$1569, DW_AT_TI_symbol_name("_PDOUT")
	.dwattr $C$DW$1569, DW_AT_data_member_location[DW_OP_plus_uconst 0xc]
	.dwattr $C$DW$1569, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1570	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$1570, DW_AT_name("PDIN_PDSET")
	.dwattr $C$DW$1570, DW_AT_TI_symbol_name("_PDIN_PDSET")
	.dwattr $C$DW$1570, DW_AT_data_member_location[DW_OP_plus_uconst 0x10]
	.dwattr $C$DW$1570, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1571	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$1571, DW_AT_name("PDCLR")
	.dwattr $C$DW$1571, DW_AT_TI_symbol_name("_PDCLR")
	.dwattr $C$DW$1571, DW_AT_data_member_location[DW_OP_plus_uconst 0x14]
	.dwattr $C$DW$1571, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1572	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$1572, DW_AT_name("GBLCTL")
	.dwattr $C$DW$1572, DW_AT_TI_symbol_name("_GBLCTL")
	.dwattr $C$DW$1572, DW_AT_data_member_location[DW_OP_plus_uconst 0x18]
	.dwattr $C$DW$1572, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1573	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$1573, DW_AT_name("AMUTE")
	.dwattr $C$DW$1573, DW_AT_TI_symbol_name("_AMUTE")
	.dwattr $C$DW$1573, DW_AT_data_member_location[DW_OP_plus_uconst 0x1c]
	.dwattr $C$DW$1573, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1574	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$1574, DW_AT_name("DLBCTL")
	.dwattr $C$DW$1574, DW_AT_TI_symbol_name("_DLBCTL")
	.dwattr $C$DW$1574, DW_AT_data_member_location[DW_OP_plus_uconst 0x20]
	.dwattr $C$DW$1574, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1575	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$1575, DW_AT_name("DITCTL")
	.dwattr $C$DW$1575, DW_AT_TI_symbol_name("_DITCTL")
	.dwattr $C$DW$1575, DW_AT_data_member_location[DW_OP_plus_uconst 0x24]
	.dwattr $C$DW$1575, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1576	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$1576, DW_AT_name("RMASK")
	.dwattr $C$DW$1576, DW_AT_TI_symbol_name("_RMASK")
	.dwattr $C$DW$1576, DW_AT_data_member_location[DW_OP_plus_uconst 0x28]
	.dwattr $C$DW$1576, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1577	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$1577, DW_AT_name("RFMT")
	.dwattr $C$DW$1577, DW_AT_TI_symbol_name("_RFMT")
	.dwattr $C$DW$1577, DW_AT_data_member_location[DW_OP_plus_uconst 0x2c]
	.dwattr $C$DW$1577, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1578	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$1578, DW_AT_name("AFSRCTL")
	.dwattr $C$DW$1578, DW_AT_TI_symbol_name("_AFSRCTL")
	.dwattr $C$DW$1578, DW_AT_data_member_location[DW_OP_plus_uconst 0x30]
	.dwattr $C$DW$1578, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1579	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$1579, DW_AT_name("ACLKRCTL")
	.dwattr $C$DW$1579, DW_AT_TI_symbol_name("_ACLKRCTL")
	.dwattr $C$DW$1579, DW_AT_data_member_location[DW_OP_plus_uconst 0x34]
	.dwattr $C$DW$1579, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1580	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$1580, DW_AT_name("AHCLKRCTL")
	.dwattr $C$DW$1580, DW_AT_TI_symbol_name("_AHCLKRCTL")
	.dwattr $C$DW$1580, DW_AT_data_member_location[DW_OP_plus_uconst 0x38]
	.dwattr $C$DW$1580, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1581	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$1581, DW_AT_name("RTDM")
	.dwattr $C$DW$1581, DW_AT_TI_symbol_name("_RTDM")
	.dwattr $C$DW$1581, DW_AT_data_member_location[DW_OP_plus_uconst 0x3c]
	.dwattr $C$DW$1581, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1582	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$1582, DW_AT_name("RINTCTL")
	.dwattr $C$DW$1582, DW_AT_TI_symbol_name("_RINTCTL")
	.dwattr $C$DW$1582, DW_AT_data_member_location[DW_OP_plus_uconst 0x40]
	.dwattr $C$DW$1582, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1583	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$1583, DW_AT_name("RSTAT")
	.dwattr $C$DW$1583, DW_AT_TI_symbol_name("_RSTAT")
	.dwattr $C$DW$1583, DW_AT_data_member_location[DW_OP_plus_uconst 0x44]
	.dwattr $C$DW$1583, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1584	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$1584, DW_AT_name("RCLKCHK")
	.dwattr $C$DW$1584, DW_AT_TI_symbol_name("_RCLKCHK")
	.dwattr $C$DW$1584, DW_AT_data_member_location[DW_OP_plus_uconst 0x48]
	.dwattr $C$DW$1584, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1585	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$1585, DW_AT_name("REVTCTL")
	.dwattr $C$DW$1585, DW_AT_TI_symbol_name("_REVTCTL")
	.dwattr $C$DW$1585, DW_AT_data_member_location[DW_OP_plus_uconst 0x4c]
	.dwattr $C$DW$1585, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1586	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$1586, DW_AT_name("XMASK")
	.dwattr $C$DW$1586, DW_AT_TI_symbol_name("_XMASK")
	.dwattr $C$DW$1586, DW_AT_data_member_location[DW_OP_plus_uconst 0x50]
	.dwattr $C$DW$1586, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1587	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$1587, DW_AT_name("XFMT")
	.dwattr $C$DW$1587, DW_AT_TI_symbol_name("_XFMT")
	.dwattr $C$DW$1587, DW_AT_data_member_location[DW_OP_plus_uconst 0x54]
	.dwattr $C$DW$1587, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1588	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$1588, DW_AT_name("AFSXCTL")
	.dwattr $C$DW$1588, DW_AT_TI_symbol_name("_AFSXCTL")
	.dwattr $C$DW$1588, DW_AT_data_member_location[DW_OP_plus_uconst 0x58]
	.dwattr $C$DW$1588, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1589	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$1589, DW_AT_name("ACLKXCTL")
	.dwattr $C$DW$1589, DW_AT_TI_symbol_name("_ACLKXCTL")
	.dwattr $C$DW$1589, DW_AT_data_member_location[DW_OP_plus_uconst 0x5c]
	.dwattr $C$DW$1589, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1590	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$1590, DW_AT_name("AHCLKXCTL")
	.dwattr $C$DW$1590, DW_AT_TI_symbol_name("_AHCLKXCTL")
	.dwattr $C$DW$1590, DW_AT_data_member_location[DW_OP_plus_uconst 0x60]
	.dwattr $C$DW$1590, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1591	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$1591, DW_AT_name("XTDM")
	.dwattr $C$DW$1591, DW_AT_TI_symbol_name("_XTDM")
	.dwattr $C$DW$1591, DW_AT_data_member_location[DW_OP_plus_uconst 0x64]
	.dwattr $C$DW$1591, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1592	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$1592, DW_AT_name("XINTCTL")
	.dwattr $C$DW$1592, DW_AT_TI_symbol_name("_XINTCTL")
	.dwattr $C$DW$1592, DW_AT_data_member_location[DW_OP_plus_uconst 0x68]
	.dwattr $C$DW$1592, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1593	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$1593, DW_AT_name("XSTAT")
	.dwattr $C$DW$1593, DW_AT_TI_symbol_name("_XSTAT")
	.dwattr $C$DW$1593, DW_AT_data_member_location[DW_OP_plus_uconst 0x6c]
	.dwattr $C$DW$1593, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1594	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$1594, DW_AT_name("XCLKCHK")
	.dwattr $C$DW$1594, DW_AT_TI_symbol_name("_XCLKCHK")
	.dwattr $C$DW$1594, DW_AT_data_member_location[DW_OP_plus_uconst 0x70]
	.dwattr $C$DW$1594, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1595	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$1595, DW_AT_name("XEVTCTL")
	.dwattr $C$DW$1595, DW_AT_TI_symbol_name("_XEVTCTL")
	.dwattr $C$DW$1595, DW_AT_data_member_location[DW_OP_plus_uconst 0x74]
	.dwattr $C$DW$1595, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1596	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$1596, DW_AT_name("SRCTL0")
	.dwattr $C$DW$1596, DW_AT_TI_symbol_name("_SRCTL0")
	.dwattr $C$DW$1596, DW_AT_data_member_location[DW_OP_plus_uconst 0x78]
	.dwattr $C$DW$1596, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1597	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$1597, DW_AT_name("SRCTL1")
	.dwattr $C$DW$1597, DW_AT_TI_symbol_name("_SRCTL1")
	.dwattr $C$DW$1597, DW_AT_data_member_location[DW_OP_plus_uconst 0x7c]
	.dwattr $C$DW$1597, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1598	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$1598, DW_AT_name("SRCTL2")
	.dwattr $C$DW$1598, DW_AT_TI_symbol_name("_SRCTL2")
	.dwattr $C$DW$1598, DW_AT_data_member_location[DW_OP_plus_uconst 0x80]
	.dwattr $C$DW$1598, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1599	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$1599, DW_AT_name("SRCTL3")
	.dwattr $C$DW$1599, DW_AT_TI_symbol_name("_SRCTL3")
	.dwattr $C$DW$1599, DW_AT_data_member_location[DW_OP_plus_uconst 0x84]
	.dwattr $C$DW$1599, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1600	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$1600, DW_AT_name("SRCTL4")
	.dwattr $C$DW$1600, DW_AT_TI_symbol_name("_SRCTL4")
	.dwattr $C$DW$1600, DW_AT_data_member_location[DW_OP_plus_uconst 0x88]
	.dwattr $C$DW$1600, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1601	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$1601, DW_AT_name("SRCTL5")
	.dwattr $C$DW$1601, DW_AT_TI_symbol_name("_SRCTL5")
	.dwattr $C$DW$1601, DW_AT_data_member_location[DW_OP_plus_uconst 0x8c]
	.dwattr $C$DW$1601, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1602	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$1602, DW_AT_name("SRCTL6")
	.dwattr $C$DW$1602, DW_AT_TI_symbol_name("_SRCTL6")
	.dwattr $C$DW$1602, DW_AT_data_member_location[DW_OP_plus_uconst 0x90]
	.dwattr $C$DW$1602, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1603	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$1603, DW_AT_name("SRCTL7")
	.dwattr $C$DW$1603, DW_AT_TI_symbol_name("_SRCTL7")
	.dwattr $C$DW$1603, DW_AT_data_member_location[DW_OP_plus_uconst 0x94]
	.dwattr $C$DW$1603, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1604	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$1604, DW_AT_name("SRCTL8")
	.dwattr $C$DW$1604, DW_AT_TI_symbol_name("_SRCTL8")
	.dwattr $C$DW$1604, DW_AT_data_member_location[DW_OP_plus_uconst 0x98]
	.dwattr $C$DW$1604, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1605	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$1605, DW_AT_name("SRCTL9")
	.dwattr $C$DW$1605, DW_AT_TI_symbol_name("_SRCTL9")
	.dwattr $C$DW$1605, DW_AT_data_member_location[DW_OP_plus_uconst 0x9c]
	.dwattr $C$DW$1605, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1606	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$1606, DW_AT_name("SRCTL10")
	.dwattr $C$DW$1606, DW_AT_TI_symbol_name("_SRCTL10")
	.dwattr $C$DW$1606, DW_AT_data_member_location[DW_OP_plus_uconst 0xa0]
	.dwattr $C$DW$1606, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1607	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$1607, DW_AT_name("SRCTL11")
	.dwattr $C$DW$1607, DW_AT_TI_symbol_name("_SRCTL11")
	.dwattr $C$DW$1607, DW_AT_data_member_location[DW_OP_plus_uconst 0xa4]
	.dwattr $C$DW$1607, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1608	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$1608, DW_AT_name("SRCTL12")
	.dwattr $C$DW$1608, DW_AT_TI_symbol_name("_SRCTL12")
	.dwattr $C$DW$1608, DW_AT_data_member_location[DW_OP_plus_uconst 0xa8]
	.dwattr $C$DW$1608, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1609	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$1609, DW_AT_name("SRCTL13")
	.dwattr $C$DW$1609, DW_AT_TI_symbol_name("_SRCTL13")
	.dwattr $C$DW$1609, DW_AT_data_member_location[DW_OP_plus_uconst 0xac]
	.dwattr $C$DW$1609, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1610	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$1610, DW_AT_name("SRCTL14")
	.dwattr $C$DW$1610, DW_AT_TI_symbol_name("_SRCTL14")
	.dwattr $C$DW$1610, DW_AT_data_member_location[DW_OP_plus_uconst 0xb0]
	.dwattr $C$DW$1610, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1611	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$1611, DW_AT_name("SRCTL15")
	.dwattr $C$DW$1611, DW_AT_TI_symbol_name("_SRCTL15")
	.dwattr $C$DW$1611, DW_AT_data_member_location[DW_OP_plus_uconst 0xb4]
	.dwattr $C$DW$1611, DW_AT_accessibility(DW_ACCESS_public)
	.dwendtag $C$DW$T$151

$C$DW$T$396	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_McaspConfig")
	.dwattr $C$DW$T$396, DW_AT_type(*$C$DW$T$151)
	.dwattr $C$DW$T$396, DW_AT_language(DW_LANG_C)

$C$DW$T$153	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$153, DW_AT_byte_size(0x04)
$C$DW$1612	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$152)
	.dwattr $C$DW$1612, DW_AT_name("flags")
	.dwattr $C$DW$1612, DW_AT_TI_symbol_name("_flags")
	.dwattr $C$DW$1612, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1612, DW_AT_accessibility(DW_ACCESS_public)
	.dwendtag $C$DW$T$153

$C$DW$T$397	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_McaspParam")
	.dwattr $C$DW$T$397, DW_AT_type(*$C$DW$T$153)
	.dwattr $C$DW$T$397, DW_AT_language(DW_LANG_C)

$C$DW$T$154	.dwtag  DW_TAG_enumeration_type
	.dwattr $C$DW$T$154, DW_AT_byte_size(0x04)
$C$DW$1613	.dwtag  DW_TAG_enumerator, DW_AT_name("SERIALIZER_1"), DW_AT_const_value(0x00)
$C$DW$1614	.dwtag  DW_TAG_enumerator, DW_AT_name("SERIALIZER_2"), DW_AT_const_value(0x01)
$C$DW$1615	.dwtag  DW_TAG_enumerator, DW_AT_name("SERIALIZER_3"), DW_AT_const_value(0x02)
$C$DW$1616	.dwtag  DW_TAG_enumerator, DW_AT_name("SERIALIZER_4"), DW_AT_const_value(0x03)
$C$DW$1617	.dwtag  DW_TAG_enumerator, DW_AT_name("SERIALIZER_5"), DW_AT_const_value(0x04)
$C$DW$1618	.dwtag  DW_TAG_enumerator, DW_AT_name("SERIALIZER_6"), DW_AT_const_value(0x05)
$C$DW$1619	.dwtag  DW_TAG_enumerator, DW_AT_name("SERIALIZER_7"), DW_AT_const_value(0x06)
$C$DW$1620	.dwtag  DW_TAG_enumerator, DW_AT_name("SERIALIZER_8"), DW_AT_const_value(0x07)
$C$DW$1621	.dwtag  DW_TAG_enumerator, DW_AT_name("SERIALIZER_9"), DW_AT_const_value(0x08)
$C$DW$1622	.dwtag  DW_TAG_enumerator, DW_AT_name("SERIALIZER_10"), DW_AT_const_value(0x09)
$C$DW$1623	.dwtag  DW_TAG_enumerator, DW_AT_name("SERIALIZER_11"), DW_AT_const_value(0x0a)
$C$DW$1624	.dwtag  DW_TAG_enumerator, DW_AT_name("SERIALIZER_12"), DW_AT_const_value(0x0b)
$C$DW$1625	.dwtag  DW_TAG_enumerator, DW_AT_name("SERIALIZER_13"), DW_AT_const_value(0x0c)
$C$DW$1626	.dwtag  DW_TAG_enumerator, DW_AT_name("SERIALIZER_14"), DW_AT_const_value(0x0d)
$C$DW$1627	.dwtag  DW_TAG_enumerator, DW_AT_name("SERIALIZER_15"), DW_AT_const_value(0x0e)
$C$DW$1628	.dwtag  DW_TAG_enumerator, DW_AT_name("SERIALIZER_16"), DW_AT_const_value(0x0f)
	.dwendtag $C$DW$T$154

$C$DW$T$155	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_McaspSerializerNum")
	.dwattr $C$DW$T$155, DW_AT_type(*$C$DW$T$154)
	.dwattr $C$DW$T$155, DW_AT_language(DW_LANG_C)

$C$DW$T$156	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$156, DW_AT_name("CSL_McaspSerQuery")
	.dwattr $C$DW$T$156, DW_AT_byte_size(0x08)
$C$DW$1629	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$155)
	.dwattr $C$DW$1629, DW_AT_name("serNum")
	.dwattr $C$DW$1629, DW_AT_TI_symbol_name("_serNum")
	.dwattr $C$DW$1629, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1629, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1630	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$34)
	.dwattr $C$DW$1630, DW_AT_name("serStatus")
	.dwattr $C$DW$1630, DW_AT_TI_symbol_name("_serStatus")
	.dwattr $C$DW$1630, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$1630, DW_AT_accessibility(DW_ACCESS_public)
	.dwendtag $C$DW$T$156

$C$DW$T$398	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_McaspSerQuery")
	.dwattr $C$DW$T$398, DW_AT_type(*$C$DW$T$156)
	.dwattr $C$DW$T$398, DW_AT_language(DW_LANG_C)

$C$DW$T$157	.dwtag  DW_TAG_enumeration_type
	.dwattr $C$DW$T$157, DW_AT_byte_size(0x04)
$C$DW$1631	.dwtag  DW_TAG_enumerator, DW_AT_name("SERIALIZER_INACTIVE"), DW_AT_const_value(0x00)
$C$DW$1632	.dwtag  DW_TAG_enumerator, DW_AT_name("SERIALIZER_XMT"), DW_AT_const_value(0x01)
$C$DW$1633	.dwtag  DW_TAG_enumerator, DW_AT_name("SERIALIZER_RCV"), DW_AT_const_value(0x02)
	.dwendtag $C$DW$T$157

$C$DW$T$158	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_McaspSerMode")
	.dwattr $C$DW$T$158, DW_AT_type(*$C$DW$T$157)
	.dwattr $C$DW$T$158, DW_AT_language(DW_LANG_C)

$C$DW$T$159	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$159, DW_AT_name("CSL_McaspSerMmode")
	.dwattr $C$DW$T$159, DW_AT_byte_size(0x08)
$C$DW$1634	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$155)
	.dwattr $C$DW$1634, DW_AT_name("serNum")
	.dwattr $C$DW$1634, DW_AT_TI_symbol_name("_serNum")
	.dwattr $C$DW$1634, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1634, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1635	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$158)
	.dwattr $C$DW$1635, DW_AT_name("serMode")
	.dwattr $C$DW$1635, DW_AT_TI_symbol_name("_serMode")
	.dwattr $C$DW$1635, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$1635, DW_AT_accessibility(DW_ACCESS_public)
	.dwendtag $C$DW$T$159

$C$DW$T$399	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_McaspSerModeQuery")
	.dwattr $C$DW$T$399, DW_AT_type(*$C$DW$T$159)
	.dwattr $C$DW$T$399, DW_AT_language(DW_LANG_C)

$C$DW$T$160	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$160, DW_AT_byte_size(0x0c)
$C$DW$1636	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$31)
	.dwattr $C$DW$1636, DW_AT_name("regs")
	.dwattr $C$DW$1636, DW_AT_TI_symbol_name("_regs")
	.dwattr $C$DW$1636, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1636, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1637	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$33)
	.dwattr $C$DW$1637, DW_AT_name("numOfSerializers")
	.dwattr $C$DW$1637, DW_AT_TI_symbol_name("_numOfSerializers")
	.dwattr $C$DW$1637, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$1637, DW_AT_accessibility(DW_ACCESS_public)
$C$DW$1638	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$34)
	.dwattr $C$DW$1638, DW_AT_name("ditStatus")
	.dwattr $C$DW$1638, DW_AT_TI_symbol_name("_ditStatus")
	.dwattr $C$DW$1638, DW_AT_data_member_location[DW_OP_plus_uconst 0x8]
	.dwattr $C$DW$1638, DW_AT_accessibility(DW_ACCESS_public)
	.dwendtag $C$DW$T$160

$C$DW$T$404	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_McaspChipContext")
	.dwattr $C$DW$T$404, DW_AT_type(*$C$DW$T$160)
	.dwattr $C$DW$T$404, DW_AT_language(DW_LANG_C)

$C$DW$T$161	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$161, DW_AT_byte_size(0x02)
$C$DW$1639	.dwtag  DW_TAG_member, DW_AT_type(*$C$DW$T$45)
	.dwattr $C$DW$1639, DW_AT_name("contextInfo")
	.dwattr $C$DW$1639, DW_AT_TI_symbol_name("_contextInfo")
	.dwattr $C$DW$1639, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1639, DW_AT_accessibility(DW_ACCESS_public)
	.dwendtag $C$DW$T$161

$C$DW$T$405	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_McaspContext")
	.dwattr $C$DW$T$405, DW_AT_type(*$C$DW$T$161)
	.dwattr $C$DW$T$405, DW_AT_language(DW_LANG_C)

$C$DW$T$187	.dwtag  DW_TAG_enumeration_type
	.dwattr $C$DW$T$187, DW_AT_byte_size(0x04)
$C$DW$1640	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_EXCLUSIVE"), DW_AT_const_value(0x00)
$C$DW$1641	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_SHARED"), DW_AT_const_value(0x01)
	.dwendtag $C$DW$T$187

$C$DW$T$188	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_OpenMode")
	.dwattr $C$DW$T$188, DW_AT_type(*$C$DW$T$187)
	.dwattr $C$DW$T$188, DW_AT_language(DW_LANG_C)

$C$DW$T$189	.dwtag  DW_TAG_enumeration_type
	.dwattr $C$DW$T$189, DW_AT_byte_size(0x04)
$C$DW$1642	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_FAIL"), DW_AT_const_value(0x00)
$C$DW$1643	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_PASS"), DW_AT_const_value(0x01)
	.dwendtag $C$DW$T$189

$C$DW$T$190	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_Test")
	.dwattr $C$DW$T$190, DW_AT_type(*$C$DW$T$189)
	.dwattr $C$DW$T$190, DW_AT_language(DW_LANG_C)

$C$DW$T$191	.dwtag  DW_TAG_enumeration_type
	.dwattr $C$DW$T$191, DW_AT_byte_size(0x04)
$C$DW$1644	.dwtag  DW_TAG_enumerator, DW_AT_name("pinMuxReg0"), DW_AT_const_value(0x00)
$C$DW$1645	.dwtag  DW_TAG_enumerator, DW_AT_name("pinMuxReg1"), DW_AT_const_value(0x01)
	.dwendtag $C$DW$T$191

$C$DW$T$192	.dwtag  DW_TAG_typedef, DW_AT_name("pinMuxReg")
	.dwattr $C$DW$T$192, DW_AT_type(*$C$DW$T$191)
	.dwattr $C$DW$T$192, DW_AT_language(DW_LANG_C)

$C$DW$T$316	.dwtag  DW_TAG_enumeration_type
	.dwattr $C$DW$T$316, DW_AT_byte_size(0x04)
$C$DW$1646	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_ME"), DW_AT_const_value(0x00)
$C$DW$1647	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_MD"), DW_AT_const_value(0x10)
$C$DW$1648	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_TRS"), DW_AT_const_value(0x20)
$C$DW$1649	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_TRR"), DW_AT_const_value(0x30)
$C$DW$1650	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_TA"), DW_AT_const_value(0x40)
$C$DW$1651	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_AA"), DW_AT_const_value(0x50)
$C$DW$1652	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_RMP"), DW_AT_const_value(0x60)
$C$DW$1653	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_RML"), DW_AT_const_value(0x70)
$C$DW$1654	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_RFP"), DW_AT_const_value(0x80)
$C$DW$1655	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_GAM"), DW_AT_const_value(0x90)
$C$DW$1656	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_MC"), DW_AT_const_value(0xa0)
$C$DW$1657	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_BTC"), DW_AT_const_value(0xb0)
$C$DW$1658	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_ES"), DW_AT_const_value(0xc0)
$C$DW$1659	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_TEC"), DW_AT_const_value(0xd0)
$C$DW$1660	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_REC"), DW_AT_const_value(0xe0)
$C$DW$1661	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_GIF0"), DW_AT_const_value(0xf0)
$C$DW$1662	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_GIM"), DW_AT_const_value(0x100)
$C$DW$1663	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_GIF1"), DW_AT_const_value(0x110)
$C$DW$1664	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_MIM"), DW_AT_const_value(0x120)
$C$DW$1665	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_MIL"), DW_AT_const_value(0x130)
$C$DW$1666	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_OPC"), DW_AT_const_value(0x140)
$C$DW$1667	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_TIOC"), DW_AT_const_value(0x150)
$C$DW$1668	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_RIOC"), DW_AT_const_value(0x160)
$C$DW$1669	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_LNT"), DW_AT_const_value(0x170)
$C$DW$1670	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_TOC"), DW_AT_const_value(0x180)
$C$DW$1671	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_TOS"), DW_AT_const_value(0x190)
$C$DW$1672	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_LAM0"), DW_AT_const_value(0x200)
$C$DW$1673	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_LAM1"), DW_AT_const_value(0x200)
$C$DW$1674	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_LAM2"), DW_AT_const_value(0x200)
$C$DW$1675	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_LAM3"), DW_AT_const_value(0x200)
$C$DW$1676	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_LAM4"), DW_AT_const_value(0x200)
$C$DW$1677	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_LAM5"), DW_AT_const_value(0x200)
$C$DW$1678	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_LAM6"), DW_AT_const_value(0x200)
$C$DW$1679	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_LAM7"), DW_AT_const_value(0x200)
$C$DW$1680	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_LAM8"), DW_AT_const_value(0x200)
$C$DW$1681	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_LAM9"), DW_AT_const_value(0x200)
$C$DW$1682	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_LAM10"), DW_AT_const_value(0x200)
$C$DW$1683	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_LAM11"), DW_AT_const_value(0x200)
$C$DW$1684	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_LAM12"), DW_AT_const_value(0x200)
$C$DW$1685	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_LAM13"), DW_AT_const_value(0x200)
$C$DW$1686	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_LAM14"), DW_AT_const_value(0x200)
$C$DW$1687	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_LAM15"), DW_AT_const_value(0x200)
$C$DW$1688	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_LAM16"), DW_AT_const_value(0x200)
$C$DW$1689	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_LAM17"), DW_AT_const_value(0x200)
$C$DW$1690	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_LAM18"), DW_AT_const_value(0x200)
$C$DW$1691	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_LAM19"), DW_AT_const_value(0x200)
$C$DW$1692	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_LAM20"), DW_AT_const_value(0x200)
$C$DW$1693	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_LAM21"), DW_AT_const_value(0x200)
$C$DW$1694	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_LAM22"), DW_AT_const_value(0x200)
$C$DW$1695	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_LAM23"), DW_AT_const_value(0x200)
$C$DW$1696	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_LAM24"), DW_AT_const_value(0x200)
$C$DW$1697	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_LAM25"), DW_AT_const_value(0x200)
$C$DW$1698	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_LAM26"), DW_AT_const_value(0x200)
$C$DW$1699	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_LAM27"), DW_AT_const_value(0x200)
$C$DW$1700	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_LAM28"), DW_AT_const_value(0x200)
$C$DW$1701	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_LAM29"), DW_AT_const_value(0x200)
$C$DW$1702	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_LAM30"), DW_AT_const_value(0x200)
$C$DW$1703	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_LAM31"), DW_AT_const_value(0x200)
$C$DW$1704	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_MOTS0"), DW_AT_const_value(0x400)
$C$DW$1705	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_MOTS1"), DW_AT_const_value(0x400)
$C$DW$1706	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_MOTS2"), DW_AT_const_value(0x400)
$C$DW$1707	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_MOTS3"), DW_AT_const_value(0x400)
$C$DW$1708	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_MOTS4"), DW_AT_const_value(0x400)
$C$DW$1709	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_MOTS5"), DW_AT_const_value(0x400)
$C$DW$1710	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_MOTS6"), DW_AT_const_value(0x400)
$C$DW$1711	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_MOTS7"), DW_AT_const_value(0x400)
$C$DW$1712	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_MOTS8"), DW_AT_const_value(0x400)
$C$DW$1713	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_MOTS9"), DW_AT_const_value(0x400)
$C$DW$1714	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_MOTS10"), DW_AT_const_value(0x400)
$C$DW$1715	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_MOTS11"), DW_AT_const_value(0x400)
$C$DW$1716	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_MOTS12"), DW_AT_const_value(0x400)
$C$DW$1717	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_MOTS13"), DW_AT_const_value(0x400)
$C$DW$1718	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_MOTS14"), DW_AT_const_value(0x400)
$C$DW$1719	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_MOTS15"), DW_AT_const_value(0x400)
$C$DW$1720	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_MOTS16"), DW_AT_const_value(0x400)
$C$DW$1721	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_MOTS17"), DW_AT_const_value(0x400)
$C$DW$1722	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_MOTS18"), DW_AT_const_value(0x400)
$C$DW$1723	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_MOTS19"), DW_AT_const_value(0x400)
$C$DW$1724	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_MOTS20"), DW_AT_const_value(0x400)
$C$DW$1725	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_MOTS21"), DW_AT_const_value(0x400)
$C$DW$1726	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_MOTS22"), DW_AT_const_value(0x400)
$C$DW$1727	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_MOTS23"), DW_AT_const_value(0x400)
$C$DW$1728	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_MOTS24"), DW_AT_const_value(0x400)
$C$DW$1729	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_MOTS25"), DW_AT_const_value(0x400)
$C$DW$1730	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_MOTS26"), DW_AT_const_value(0x400)
$C$DW$1731	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_MOTS27"), DW_AT_const_value(0x400)
$C$DW$1732	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_MOTS28"), DW_AT_const_value(0x400)
$C$DW$1733	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_MOTS29"), DW_AT_const_value(0x400)
$C$DW$1734	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_MOTS30"), DW_AT_const_value(0x400)
$C$DW$1735	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_MOTS31"), DW_AT_const_value(0x400)
$C$DW$1736	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_MOTO0"), DW_AT_const_value(0x600)
$C$DW$1737	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_MOTO1"), DW_AT_const_value(0x600)
$C$DW$1738	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_MOTO2"), DW_AT_const_value(0x600)
$C$DW$1739	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_MOTO3"), DW_AT_const_value(0x600)
$C$DW$1740	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_MOTO4"), DW_AT_const_value(0x600)
$C$DW$1741	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_MOTO5"), DW_AT_const_value(0x600)
$C$DW$1742	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_MOTO6"), DW_AT_const_value(0x600)
$C$DW$1743	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_MOTO7"), DW_AT_const_value(0x600)
$C$DW$1744	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_MOTO8"), DW_AT_const_value(0x600)
$C$DW$1745	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_MOTO9"), DW_AT_const_value(0x600)
$C$DW$1746	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_MOTO10"), DW_AT_const_value(0x600)
$C$DW$1747	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_MOTO11"), DW_AT_const_value(0x600)
$C$DW$1748	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_MOTO12"), DW_AT_const_value(0x600)
$C$DW$1749	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_MOTO13"), DW_AT_const_value(0x600)
$C$DW$1750	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_MOTO14"), DW_AT_const_value(0x600)
$C$DW$1751	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_MOTO15"), DW_AT_const_value(0x600)
$C$DW$1752	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_MOTO16"), DW_AT_const_value(0x600)
$C$DW$1753	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_MOTO17"), DW_AT_const_value(0x600)
$C$DW$1754	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_MOTO18"), DW_AT_const_value(0x600)
$C$DW$1755	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_MOTO19"), DW_AT_const_value(0x600)
$C$DW$1756	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_MOTO20"), DW_AT_const_value(0x600)
$C$DW$1757	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_MOTO21"), DW_AT_const_value(0x600)
$C$DW$1758	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_MOTO22"), DW_AT_const_value(0x600)
$C$DW$1759	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_MOTO23"), DW_AT_const_value(0x600)
$C$DW$1760	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_MOTO24"), DW_AT_const_value(0x600)
$C$DW$1761	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_MOTO25"), DW_AT_const_value(0x600)
$C$DW$1762	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_MOTO26"), DW_AT_const_value(0x600)
$C$DW$1763	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_MOTO27"), DW_AT_const_value(0x600)
$C$DW$1764	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_MOTO28"), DW_AT_const_value(0x600)
$C$DW$1765	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_MOTO29"), DW_AT_const_value(0x600)
$C$DW$1766	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_MOTO30"), DW_AT_const_value(0x600)
$C$DW$1767	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_MOTO31"), DW_AT_const_value(0x600)
$C$DW$1768	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_MID"), DW_AT_const_value(0x00)
$C$DW$1769	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_MCF"), DW_AT_const_value(0x10)
$C$DW$1770	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_MDL"), DW_AT_const_value(0x20)
$C$DW$1771	.dwtag  DW_TAG_enumerator, DW_AT_name("CAN_MDH"), DW_AT_const_value(0x30)
	.dwendtag $C$DW$T$316

$C$DW$T$317	.dwtag  DW_TAG_typedef, DW_AT_name("CAN_RegIds")
	.dwattr $C$DW$T$317, DW_AT_type(*$C$DW$T$316)
	.dwattr $C$DW$T$317, DW_AT_language(DW_LANG_C)

$C$DW$T$330	.dwtag  DW_TAG_enumeration_type
	.dwattr $C$DW$T$330, DW_AT_byte_size(0x04)
$C$DW$1772	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_UART_ANY"), DW_AT_const_value(-1)
$C$DW$1773	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_UART_0"), DW_AT_const_value(0x00)
$C$DW$1774	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_UART_1"), DW_AT_const_value(0x01)
	.dwendtag $C$DW$T$330

$C$DW$T$331	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_UartNum")
	.dwattr $C$DW$T$331, DW_AT_type(*$C$DW$T$330)
	.dwattr $C$DW$T$331, DW_AT_language(DW_LANG_C)

$C$DW$T$332	.dwtag  DW_TAG_enumeration_type
	.dwattr $C$DW$T$332, DW_AT_byte_size(0x04)
$C$DW$1775	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_I2C_ANY"), DW_AT_const_value(-1)
$C$DW$1776	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_I2C_0"), DW_AT_const_value(0x00)
	.dwendtag $C$DW$T$332

$C$DW$T$333	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_I2cNum")
	.dwattr $C$DW$T$333, DW_AT_type(*$C$DW$T$332)
	.dwattr $C$DW$T$333, DW_AT_language(DW_LANG_C)

$C$DW$T$334	.dwtag  DW_TAG_enumeration_type
	.dwattr $C$DW$T$334, DW_AT_byte_size(0x04)
$C$DW$1777	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_PWM_ANY"), DW_AT_const_value(-1)
$C$DW$1778	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_PWM_0"), DW_AT_const_value(0x00)
$C$DW$1779	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_PWM_1"), DW_AT_const_value(0x01)
$C$DW$1780	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_PWM_2"), DW_AT_const_value(0x02)
	.dwendtag $C$DW$T$334

$C$DW$T$335	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_PwmNum")
	.dwattr $C$DW$T$335, DW_AT_type(*$C$DW$T$334)
	.dwattr $C$DW$T$335, DW_AT_language(DW_LANG_C)

$C$DW$T$336	.dwtag  DW_TAG_enumeration_type
	.dwattr $C$DW$T$336, DW_AT_byte_size(0x04)
$C$DW$1781	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_PLLC_ANY"), DW_AT_const_value(-1)
$C$DW$1782	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_PLLC_0"), DW_AT_const_value(0x00)
$C$DW$1783	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_PLLC_1"), DW_AT_const_value(0x01)
	.dwendtag $C$DW$T$336

$C$DW$T$337	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_PllcNum")
	.dwattr $C$DW$T$337, DW_AT_type(*$C$DW$T$336)
	.dwattr $C$DW$T$337, DW_AT_language(DW_LANG_C)

$C$DW$T$338	.dwtag  DW_TAG_enumeration_type
	.dwattr $C$DW$T$338, DW_AT_byte_size(0x04)
$C$DW$1784	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_GPIO_ANY"), DW_AT_const_value(-1)
$C$DW$1785	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_GPIO"), DW_AT_const_value(0x00)
	.dwendtag $C$DW$T$338

$C$DW$T$339	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_GpioNum")
	.dwattr $C$DW$T$339, DW_AT_type(*$C$DW$T$338)
	.dwattr $C$DW$T$339, DW_AT_language(DW_LANG_C)

$C$DW$T$340	.dwtag  DW_TAG_enumeration_type
	.dwattr $C$DW$T$340, DW_AT_byte_size(0x04)
$C$DW$1786	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_EMIF_ANY"), DW_AT_const_value(-1)
$C$DW$1787	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_EMIF_0"), DW_AT_const_value(0x00)
	.dwendtag $C$DW$T$340

$C$DW$T$341	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_EmifNum")
	.dwattr $C$DW$T$341, DW_AT_type(*$C$DW$T$340)
	.dwattr $C$DW$T$341, DW_AT_language(DW_LANG_C)

$C$DW$T$342	.dwtag  DW_TAG_enumeration_type
	.dwattr $C$DW$T$342, DW_AT_byte_size(0x04)
$C$DW$1788	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_VLYNQ_ANY"), DW_AT_const_value(-1)
$C$DW$1789	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_VLYNQ_0"), DW_AT_const_value(0x00)
	.dwendtag $C$DW$T$342

$C$DW$T$343	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_VlynqNum")
	.dwattr $C$DW$T$343, DW_AT_type(*$C$DW$T$342)
	.dwattr $C$DW$T$343, DW_AT_language(DW_LANG_C)

$C$DW$T$344	.dwtag  DW_TAG_enumeration_type
	.dwattr $C$DW$T$344, DW_AT_byte_size(0x04)
$C$DW$1790	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_MCBSP_ANY"), DW_AT_const_value(-1)
$C$DW$1791	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_MCBSP_0"), DW_AT_const_value(0x00)
$C$DW$1792	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_MCBSP_1"), DW_AT_const_value(0x02)
	.dwendtag $C$DW$T$344

$C$DW$T$345	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_McbspNum")
	.dwattr $C$DW$T$345, DW_AT_type(*$C$DW$T$344)
	.dwattr $C$DW$T$345, DW_AT_language(DW_LANG_C)

$C$DW$T$346	.dwtag  DW_TAG_enumeration_type
	.dwattr $C$DW$T$346, DW_AT_byte_size(0x04)
$C$DW$1793	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_DDR_ANY"), DW_AT_const_value(-1)
$C$DW$1794	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_DDR_0"), DW_AT_const_value(0x00)
	.dwendtag $C$DW$T$346

$C$DW$T$347	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_DdrNum")
	.dwattr $C$DW$T$347, DW_AT_type(*$C$DW$T$346)
	.dwattr $C$DW$T$347, DW_AT_language(DW_LANG_C)

$C$DW$T$348	.dwtag  DW_TAG_enumeration_type
	.dwattr $C$DW$T$348, DW_AT_byte_size(0x04)
$C$DW$1795	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_TMR_ANY"), DW_AT_const_value(-1)
$C$DW$1796	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_TMR_0"), DW_AT_const_value(0x00)
$C$DW$1797	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_TMR_1"), DW_AT_const_value(0x01)
$C$DW$1798	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_TMR_2"), DW_AT_const_value(0x02)
	.dwendtag $C$DW$T$348

$C$DW$T$349	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_TmrNum")
	.dwattr $C$DW$T$349, DW_AT_type(*$C$DW$T$348)
	.dwattr $C$DW$T$349, DW_AT_language(DW_LANG_C)

$C$DW$T$350	.dwtag  DW_TAG_enumeration_type
	.dwattr $C$DW$T$350, DW_AT_byte_size(0x04)
$C$DW$1799	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_UHPI_ANY"), DW_AT_const_value(-1)
$C$DW$1800	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_UHPI_0"), DW_AT_const_value(0x00)
	.dwendtag $C$DW$T$350

$C$DW$T$351	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_UhpiNum")
	.dwattr $C$DW$T$351, DW_AT_type(*$C$DW$T$350)
	.dwattr $C$DW$T$351, DW_AT_language(DW_LANG_C)

$C$DW$T$352	.dwtag  DW_TAG_enumeration_type
	.dwattr $C$DW$T$352, DW_AT_byte_size(0x04)
$C$DW$1801	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_EMAC_ANY"), DW_AT_const_value(-1)
$C$DW$1802	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_EMAC_0"), DW_AT_const_value(0x00)
	.dwendtag $C$DW$T$352

$C$DW$T$353	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_EmacNum")
	.dwattr $C$DW$T$353, DW_AT_type(*$C$DW$T$352)
	.dwattr $C$DW$T$353, DW_AT_language(DW_LANG_C)

$C$DW$T$354	.dwtag  DW_TAG_enumeration_type
	.dwattr $C$DW$T$354, DW_AT_byte_size(0x04)
$C$DW$1803	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_VPSS_ANY"), DW_AT_const_value(-1)
$C$DW$1804	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_VPSS_0"), DW_AT_const_value(0x00)
	.dwendtag $C$DW$T$354

$C$DW$T$355	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_VpssNum")
	.dwattr $C$DW$T$355, DW_AT_type(*$C$DW$T$354)
	.dwattr $C$DW$T$355, DW_AT_language(DW_LANG_C)

$C$DW$T$356	.dwtag  DW_TAG_enumeration_type
	.dwattr $C$DW$T$356, DW_AT_byte_size(0x04)
$C$DW$1805	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_EWRAP_ANY"), DW_AT_const_value(-1)
$C$DW$1806	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_EWRAP_0"), DW_AT_const_value(0x00)
	.dwendtag $C$DW$T$356

$C$DW$T$357	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_EwrapNum")
	.dwattr $C$DW$T$357, DW_AT_type(*$C$DW$T$356)
	.dwattr $C$DW$T$357, DW_AT_language(DW_LANG_C)

$C$DW$T$358	.dwtag  DW_TAG_enumeration_type
	.dwattr $C$DW$T$358, DW_AT_byte_size(0x04)
$C$DW$1807	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_CCDC_ANY"), DW_AT_const_value(-1)
$C$DW$1808	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_CCDC_0"), DW_AT_const_value(0x00)
	.dwendtag $C$DW$T$358

$C$DW$T$359	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_CcdcNum")
	.dwattr $C$DW$T$359, DW_AT_type(*$C$DW$T$358)
	.dwattr $C$DW$T$359, DW_AT_language(DW_LANG_C)

$C$DW$T$360	.dwtag  DW_TAG_enumeration_type
	.dwattr $C$DW$T$360, DW_AT_byte_size(0x04)
$C$DW$1809	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_H3A_ANY"), DW_AT_const_value(-1)
$C$DW$1810	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_H3A_0"), DW_AT_const_value(0x00)
	.dwendtag $C$DW$T$360

$C$DW$T$361	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_H3aNum")
	.dwattr $C$DW$T$361, DW_AT_type(*$C$DW$T$360)
	.dwattr $C$DW$T$361, DW_AT_language(DW_LANG_C)

$C$DW$T$362	.dwtag  DW_TAG_enumeration_type
	.dwattr $C$DW$T$362, DW_AT_byte_size(0x04)
$C$DW$1811	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_HIST_ANY"), DW_AT_const_value(-1)
$C$DW$1812	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_HIST_0"), DW_AT_const_value(0x00)
	.dwendtag $C$DW$T$362

$C$DW$T$363	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_HistNum")
	.dwattr $C$DW$T$363, DW_AT_type(*$C$DW$T$362)
	.dwattr $C$DW$T$363, DW_AT_language(DW_LANG_C)

$C$DW$T$364	.dwtag  DW_TAG_enumeration_type
	.dwattr $C$DW$T$364, DW_AT_byte_size(0x04)
$C$DW$1813	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_OSD_ANY"), DW_AT_const_value(-1)
$C$DW$1814	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_OSD_0"), DW_AT_const_value(0x00)
	.dwendtag $C$DW$T$364

$C$DW$T$365	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_OsdNum")
	.dwattr $C$DW$T$365, DW_AT_type(*$C$DW$T$364)
	.dwattr $C$DW$T$365, DW_AT_language(DW_LANG_C)

$C$DW$T$366	.dwtag  DW_TAG_enumeration_type
	.dwattr $C$DW$T$366, DW_AT_byte_size(0x04)
$C$DW$1815	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_PREV_ANY"), DW_AT_const_value(-1)
$C$DW$1816	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_PREV_0"), DW_AT_const_value(0x00)
	.dwendtag $C$DW$T$366

$C$DW$T$367	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_PrevNum")
	.dwattr $C$DW$T$367, DW_AT_type(*$C$DW$T$366)
	.dwattr $C$DW$T$367, DW_AT_language(DW_LANG_C)

$C$DW$T$368	.dwtag  DW_TAG_enumeration_type
	.dwattr $C$DW$T$368, DW_AT_byte_size(0x04)
$C$DW$1817	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_RESZ_ANY"), DW_AT_const_value(-1)
$C$DW$1818	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_RESZ_0"), DW_AT_const_value(0x00)
	.dwendtag $C$DW$T$368

$C$DW$T$369	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_ReszNum")
	.dwattr $C$DW$T$369, DW_AT_type(*$C$DW$T$368)
	.dwattr $C$DW$T$369, DW_AT_language(DW_LANG_C)

$C$DW$T$370	.dwtag  DW_TAG_enumeration_type
	.dwattr $C$DW$T$370, DW_AT_byte_size(0x04)
$C$DW$1819	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_VENC_ANY"), DW_AT_const_value(-1)
$C$DW$1820	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_VENC_0"), DW_AT_const_value(0x00)
	.dwendtag $C$DW$T$370

$C$DW$T$371	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_VencNum")
	.dwattr $C$DW$T$371, DW_AT_type(*$C$DW$T$370)
	.dwattr $C$DW$T$371, DW_AT_language(DW_LANG_C)

$C$DW$T$372	.dwtag  DW_TAG_enumeration_type
	.dwattr $C$DW$T$372, DW_AT_byte_size(0x04)
$C$DW$1821	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_VPBE_ANY"), DW_AT_const_value(-1)
$C$DW$1822	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_VPBE_0"), DW_AT_const_value(0x00)
	.dwendtag $C$DW$T$372

$C$DW$T$373	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_VpbeNum")
	.dwattr $C$DW$T$373, DW_AT_type(*$C$DW$T$372)
	.dwattr $C$DW$T$373, DW_AT_language(DW_LANG_C)

$C$DW$T$374	.dwtag  DW_TAG_enumeration_type
	.dwattr $C$DW$T$374, DW_AT_byte_size(0x04)
$C$DW$1823	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_MDIO_ANY"), DW_AT_const_value(-1)
$C$DW$1824	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_MDIO_0"), DW_AT_const_value(0x00)
	.dwendtag $C$DW$T$374

$C$DW$T$375	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_MdioNum")
	.dwattr $C$DW$T$375, DW_AT_type(*$C$DW$T$374)
	.dwattr $C$DW$T$375, DW_AT_language(DW_LANG_C)

$C$DW$T$376	.dwtag  DW_TAG_enumeration_type
	.dwattr $C$DW$T$376, DW_AT_byte_size(0x04)
$C$DW$1825	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_PSC_ANY"), DW_AT_const_value(-1)
$C$DW$1826	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_PSC_0"), DW_AT_const_value(0x00)
	.dwendtag $C$DW$T$376

$C$DW$T$377	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_PscNum")
	.dwattr $C$DW$T$377, DW_AT_type(*$C$DW$T$376)
	.dwattr $C$DW$T$377, DW_AT_language(DW_LANG_C)

$C$DW$T$378	.dwtag  DW_TAG_enumeration_type
	.dwattr $C$DW$T$378, DW_AT_byte_size(0x04)
$C$DW$1827	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_SYS_ANY"), DW_AT_const_value(-1)
$C$DW$1828	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_SYS_0"), DW_AT_const_value(0x00)
	.dwendtag $C$DW$T$378

$C$DW$T$379	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_SysNum")
	.dwattr $C$DW$T$379, DW_AT_type(*$C$DW$T$378)
	.dwattr $C$DW$T$379, DW_AT_language(DW_LANG_C)

$C$DW$T$380	.dwtag  DW_TAG_enumeration_type
	.dwattr $C$DW$T$380, DW_AT_byte_size(0x04)
$C$DW$1829	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_DEVICECONFIG_ANY"), DW_AT_const_value(-1)
$C$DW$1830	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_DEVICECONFIG_0"), DW_AT_const_value(0x00)
	.dwendtag $C$DW$T$380

$C$DW$T$381	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_DeviceconfigNum")
	.dwattr $C$DW$T$381, DW_AT_type(*$C$DW$T$380)
	.dwattr $C$DW$T$381, DW_AT_language(DW_LANG_C)

$C$DW$T$382	.dwtag  DW_TAG_enumeration_type
	.dwattr $C$DW$T$382, DW_AT_byte_size(0x04)
$C$DW$1831	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_MCASP_ANY"), DW_AT_const_value(-1)
$C$DW$1832	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_MCASP_0"), DW_AT_const_value(0x00)
$C$DW$1833	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_MCASP_1"), DW_AT_const_value(0x01)
	.dwendtag $C$DW$T$382

$C$DW$T$383	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_McaspNum")
	.dwattr $C$DW$T$383, DW_AT_type(*$C$DW$T$382)
	.dwattr $C$DW$T$383, DW_AT_language(DW_LANG_C)

$C$DW$T$384	.dwtag  DW_TAG_enumeration_type
	.dwattr $C$DW$T$384, DW_AT_byte_size(0x04)
$C$DW$1834	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_CAN_ANY"), DW_AT_const_value(-1)
$C$DW$1835	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_CAN_0"), DW_AT_const_value(0x00)
	.dwendtag $C$DW$T$384

$C$DW$T$385	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_CanNum")
	.dwattr $C$DW$T$385, DW_AT_type(*$C$DW$T$384)
	.dwattr $C$DW$T$385, DW_AT_language(DW_LANG_C)

$C$DW$T$386	.dwtag  DW_TAG_enumeration_type
	.dwattr $C$DW$T$386, DW_AT_byte_size(0x04)
$C$DW$1836	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_INTC_ANY"), DW_AT_const_value(-1)
$C$DW$1837	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_INTC_0"), DW_AT_const_value(0x00)
	.dwendtag $C$DW$T$386

$C$DW$T$387	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_IntcNum")
	.dwattr $C$DW$T$387, DW_AT_type(*$C$DW$T$386)
	.dwattr $C$DW$T$387, DW_AT_language(DW_LANG_C)

$C$DW$T$388	.dwtag  DW_TAG_enumeration_type
	.dwattr $C$DW$T$388, DW_AT_byte_size(0x04)
$C$DW$1838	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_VFOC_ANY"), DW_AT_const_value(-1)
$C$DW$1839	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_VFOC_0"), DW_AT_const_value(0x00)
	.dwendtag $C$DW$T$388

$C$DW$T$389	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_VfocNum")
	.dwattr $C$DW$T$389, DW_AT_type(*$C$DW$T$388)
	.dwattr $C$DW$T$389, DW_AT_language(DW_LANG_C)

$C$DW$T$390	.dwtag  DW_TAG_enumeration_type
	.dwattr $C$DW$T$390, DW_AT_byte_size(0x04)
$C$DW$1840	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_PCIIF_ANY"), DW_AT_const_value(-1)
$C$DW$1841	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_PCIIF_0"), DW_AT_const_value(0x00)
	.dwendtag $C$DW$T$390

$C$DW$T$391	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_PciifNum")
	.dwattr $C$DW$T$391, DW_AT_type(*$C$DW$T$390)
	.dwattr $C$DW$T$391, DW_AT_language(DW_LANG_C)

$C$DW$T$392	.dwtag  DW_TAG_enumeration_type
	.dwattr $C$DW$T$392, DW_AT_byte_size(0x04)
$C$DW$1842	.dwtag  DW_TAG_enumerator, DW_AT_name("DIT_REGISTER_0"), DW_AT_const_value(0x00)
$C$DW$1843	.dwtag  DW_TAG_enumerator, DW_AT_name("DIT_REGISTER_1"), DW_AT_const_value(0x01)
$C$DW$1844	.dwtag  DW_TAG_enumerator, DW_AT_name("DIT_REGISTER_2"), DW_AT_const_value(0x02)
$C$DW$1845	.dwtag  DW_TAG_enumerator, DW_AT_name("DIT_REGISTER_3"), DW_AT_const_value(0x03)
$C$DW$1846	.dwtag  DW_TAG_enumerator, DW_AT_name("DIT_REGISTER_4"), DW_AT_const_value(0x04)
$C$DW$1847	.dwtag  DW_TAG_enumerator, DW_AT_name("DIT_REGISTER_5"), DW_AT_const_value(0x05)
	.dwendtag $C$DW$T$392

$C$DW$T$393	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_McaspDITRegIndex")
	.dwattr $C$DW$T$393, DW_AT_type(*$C$DW$T$392)
	.dwattr $C$DW$T$393, DW_AT_language(DW_LANG_C)

$C$DW$T$400	.dwtag  DW_TAG_enumeration_type
	.dwattr $C$DW$T$400, DW_AT_byte_size(0x04)
$C$DW$1848	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_MCASP_CMD_SET_XMT"), DW_AT_const_value(0x01)
$C$DW$1849	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_MCASP_CMD_SET_RCV"), DW_AT_const_value(0x02)
$C$DW$1850	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_MCASP_CMD_RESET_FSYNC_XMT"), DW_AT_const_value(0x03)
$C$DW$1851	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_MCASP_CMD_RESET_FSYNC_RCV"), DW_AT_const_value(0x04)
$C$DW$1852	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_MCASP_CMD_REG_RESET"), DW_AT_const_value(0x05)
$C$DW$1853	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_MCASP_CMD_AMUTE_ON"), DW_AT_const_value(0x06)
$C$DW$1854	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_MCASP_CMD_DLB_ON"), DW_AT_const_value(0x07)
$C$DW$1855	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_MCASP_CMD_CONFIG_RTDM_SLOT"), DW_AT_const_value(0x08)
$C$DW$1856	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_MCASP_CMD_CONFIG_XTDM_SLOT"), DW_AT_const_value(0x09)
$C$DW$1857	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_MCASP_CMD_CONFIG_INTERRUPT_RCV"), DW_AT_const_value(0x0a)
$C$DW$1858	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_MCASP_CMD_CONFIG_INTERRUPT_XMT"), DW_AT_const_value(0x0b)
$C$DW$1859	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_MCASP_CMD_CLK_RESET_RCV"), DW_AT_const_value(0x0c)
$C$DW$1860	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_MCASP_CMD_CLK_RESET_XMT"), DW_AT_const_value(0x0d)
$C$DW$1861	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_MCASP_CMD_CLK_SET_RCV"), DW_AT_const_value(0x0e)
$C$DW$1862	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_MCASP_CMD_CLK_SET_XMT"), DW_AT_const_value(0x0f)
$C$DW$1863	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_MCASP_CMD_CONFIG_XMT_SECTION"), DW_AT_const_value(0x10)
$C$DW$1864	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_MCASP_CMD_CONFIG_RCV_SECTION"), DW_AT_const_value(0x11)
$C$DW$1865	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_MCASP_CMD_SET_SER_XMT"), DW_AT_const_value(0x12)
$C$DW$1866	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_MCASP_CMD_SET_SER_RCV"), DW_AT_const_value(0x13)
$C$DW$1867	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_MCASP_CMD_SET_SER_INA"), DW_AT_const_value(0x14)
$C$DW$1868	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_MCASP_CMD_WRITE_CHAN_STAT_RAM"), DW_AT_const_value(0x15)
$C$DW$1869	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_MCASP_CMD_WRITE_USER_DATA_RAM"), DW_AT_const_value(0x16)
$C$DW$1870	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_MCASP_CMD_RESET_XMT"), DW_AT_const_value(0x17)
$C$DW$1871	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_MCASP_CMD_RESET_RCV"), DW_AT_const_value(0x18)
$C$DW$1872	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_MCASP_CMD_RESET_SM_FS_XMT"), DW_AT_const_value(0x19)
$C$DW$1873	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_MCASP_CMD_RESET_SM_FS_RCV"), DW_AT_const_value(0x1a)
$C$DW$1874	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_MCASP_CMD_ACTIVATE_XMT_CLK_SER"), DW_AT_const_value(0x1b)
$C$DW$1875	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_MCASP_CMD_ACTIVATE_RCV_CLK_SER"), DW_AT_const_value(0x1c)
$C$DW$1876	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_MCASP_CMD_ACTIVATE_SM_RCV_XMT"), DW_AT_const_value(0x1d)
$C$DW$1877	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_MCASP_CMD_ACTIVATE_FS_RCV_XMT"), DW_AT_const_value(0x1e)
$C$DW$1878	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_MCASP_CMD_SET_DIT_MODE"), DW_AT_const_value(0x1f)
	.dwendtag $C$DW$T$400

$C$DW$T$401	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_McaspHwControlCmd")
	.dwattr $C$DW$T$401, DW_AT_type(*$C$DW$T$400)
	.dwattr $C$DW$T$401, DW_AT_language(DW_LANG_C)

$C$DW$T$402	.dwtag  DW_TAG_enumeration_type
	.dwattr $C$DW$T$402, DW_AT_byte_size(0x04)
$C$DW$1879	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_MCASP_QUERY_CURRENT_XSLOT"), DW_AT_const_value(0x01)
$C$DW$1880	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_MCASP_QUERY_CURRENT_RSLOT"), DW_AT_const_value(0x02)
$C$DW$1881	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_MCASP_QUERY_XSTAT_XERR"), DW_AT_const_value(0x03)
$C$DW$1882	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_MCASP_QUERY_XSTAT_XCLKFAIL"), DW_AT_const_value(0x04)
$C$DW$1883	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_MCASP_QUERY_XSTAT_XSYNCERR"), DW_AT_const_value(0x05)
$C$DW$1884	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_MCASP_QUERY_XSTAT_XUNDRN"), DW_AT_const_value(0x06)
$C$DW$1885	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_MCASP_QUERY_XSTAT_XDATA"), DW_AT_const_value(0x07)
$C$DW$1886	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_MCASP_QUERY_RSTAT_RERR"), DW_AT_const_value(0x08)
$C$DW$1887	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_MCASP_QUERY_RSTAT_RCLKFAIL"), DW_AT_const_value(0x09)
$C$DW$1888	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_MCASP_QUERY_RSTAT_RSYNCERR"), DW_AT_const_value(0x0a)
$C$DW$1889	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_MCASP_QUERY_RSTAT_ROVRN"), DW_AT_const_value(0x0b)
$C$DW$1890	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_MCASP_QUERY_RSTAT_RDATA"), DW_AT_const_value(0x0c)
$C$DW$1891	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_MCASP_QUERY_SRCTL_RRDY"), DW_AT_const_value(0x0d)
$C$DW$1892	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_MCASP_QUERY_SRCTL_XRDY"), DW_AT_const_value(0x0e)
$C$DW$1893	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_MCASP_QUERY_SRCTL_SRMOD"), DW_AT_const_value(0x0f)
$C$DW$1894	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_MCASP_QUERY_XSTAT"), DW_AT_const_value(0x10)
$C$DW$1895	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_MCASP_QUERY_RSTAT"), DW_AT_const_value(0x11)
$C$DW$1896	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_MCASP_QUERY_SM_FS_XMT"), DW_AT_const_value(0x12)
$C$DW$1897	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_MCASP_QUERY_SM_FS_RCV"), DW_AT_const_value(0x13)
$C$DW$1898	.dwtag  DW_TAG_enumerator, DW_AT_name("CSL_MCASP_QUERY_DIT_MODE"), DW_AT_const_value(0x14)
	.dwendtag $C$DW$T$402

$C$DW$T$403	.dwtag  DW_TAG_typedef, DW_AT_name("CSL_McaspHwStatusQuery")
	.dwattr $C$DW$T$403, DW_AT_type(*$C$DW$T$402)
	.dwattr $C$DW$T$403, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$CU, DW_AT_language(DW_LANG_C)

;***************************************************************
;* DWARF CIE ENTRIES                                           *
;***************************************************************

$C$DW$CIE	.dwcie 228
	.dwcfi	cfa_register, 31
	.dwcfi	cfa_offset, 0
	.dwcfi	undefined, 0
	.dwcfi	undefined, 1
	.dwcfi	undefined, 2
	.dwcfi	undefined, 3
	.dwcfi	undefined, 4
	.dwcfi	undefined, 5
	.dwcfi	undefined, 6
	.dwcfi	undefined, 7
	.dwcfi	undefined, 8
	.dwcfi	undefined, 9
	.dwcfi	same_value, 10
	.dwcfi	same_value, 11
	.dwcfi	same_value, 12
	.dwcfi	same_value, 13
	.dwcfi	same_value, 14
	.dwcfi	same_value, 15
	.dwcfi	undefined, 16
	.dwcfi	undefined, 17
	.dwcfi	undefined, 18
	.dwcfi	undefined, 19
	.dwcfi	undefined, 20
	.dwcfi	undefined, 21
	.dwcfi	undefined, 22
	.dwcfi	undefined, 23
	.dwcfi	undefined, 24
	.dwcfi	undefined, 25
	.dwcfi	same_value, 26
	.dwcfi	same_value, 27
	.dwcfi	same_value, 28
	.dwcfi	same_value, 29
	.dwcfi	same_value, 30
	.dwcfi	same_value, 31
	.dwcfi	same_value, 32
	.dwcfi	undefined, 33
	.dwcfi	undefined, 34
	.dwcfi	undefined, 35
	.dwcfi	undefined, 36
	.dwcfi	undefined, 37
	.dwcfi	undefined, 38
	.dwcfi	undefined, 39
	.dwcfi	undefined, 40
	.dwcfi	undefined, 41
	.dwcfi	undefined, 42
	.dwcfi	undefined, 43
	.dwcfi	undefined, 44
	.dwcfi	undefined, 45
	.dwcfi	undefined, 46
	.dwcfi	undefined, 47
	.dwcfi	undefined, 48
	.dwcfi	undefined, 49
	.dwcfi	undefined, 50
	.dwcfi	undefined, 51
	.dwcfi	undefined, 52
	.dwcfi	undefined, 53
	.dwcfi	undefined, 54
	.dwcfi	undefined, 55
	.dwcfi	undefined, 56
	.dwcfi	undefined, 57
	.dwcfi	undefined, 58
	.dwcfi	undefined, 59
	.dwcfi	undefined, 60
	.dwcfi	undefined, 61
	.dwcfi	undefined, 62
	.dwcfi	undefined, 63
	.dwcfi	undefined, 64
	.dwcfi	undefined, 65
	.dwcfi	undefined, 66
	.dwcfi	undefined, 67
	.dwcfi	undefined, 68
	.dwcfi	undefined, 69
	.dwcfi	undefined, 70
	.dwcfi	undefined, 71
	.dwcfi	undefined, 72
	.dwcfi	undefined, 73
	.dwcfi	undefined, 74
	.dwcfi	undefined, 75
	.dwcfi	undefined, 76
	.dwcfi	undefined, 77
	.dwcfi	undefined, 78
	.dwcfi	undefined, 79
	.dwcfi	undefined, 80
	.dwcfi	undefined, 81
	.dwcfi	undefined, 82
	.dwcfi	undefined, 83
	.dwcfi	undefined, 84
	.dwcfi	undefined, 85
	.dwcfi	undefined, 86
	.dwcfi	undefined, 87
	.dwcfi	undefined, 88
	.dwcfi	undefined, 89
	.dwcfi	undefined, 90
	.dwcfi	undefined, 91
	.dwcfi	undefined, 92
	.dwcfi	undefined, 93
	.dwcfi	undefined, 94
	.dwcfi	undefined, 95
	.dwcfi	undefined, 96
	.dwcfi	undefined, 97
	.dwcfi	undefined, 98
	.dwcfi	undefined, 99
	.dwcfi	undefined, 100
	.dwcfi	undefined, 101
	.dwcfi	undefined, 102
	.dwcfi	undefined, 103
	.dwcfi	undefined, 104
	.dwcfi	undefined, 105
	.dwcfi	undefined, 106
	.dwcfi	undefined, 107
	.dwcfi	undefined, 108
	.dwcfi	undefined, 109
	.dwcfi	undefined, 110
	.dwcfi	undefined, 111
	.dwcfi	undefined, 112
	.dwcfi	undefined, 113
	.dwcfi	undefined, 114
	.dwcfi	undefined, 115
	.dwcfi	undefined, 116
	.dwcfi	undefined, 117
	.dwcfi	undefined, 118
	.dwcfi	undefined, 119
	.dwcfi	undefined, 120
	.dwcfi	undefined, 121
	.dwcfi	undefined, 122
	.dwcfi	undefined, 123
	.dwcfi	undefined, 124
	.dwcfi	undefined, 125
	.dwcfi	undefined, 126
	.dwcfi	undefined, 127
	.dwcfi	undefined, 128
	.dwcfi	undefined, 129
	.dwcfi	undefined, 130
	.dwcfi	undefined, 131
	.dwcfi	undefined, 132
	.dwcfi	undefined, 133
	.dwcfi	undefined, 134
	.dwcfi	undefined, 135
	.dwcfi	undefined, 136
	.dwcfi	undefined, 137
	.dwcfi	undefined, 138
	.dwcfi	undefined, 139
	.dwcfi	undefined, 140
	.dwcfi	undefined, 141
	.dwcfi	undefined, 142
	.dwcfi	undefined, 143
	.dwcfi	undefined, 144
	.dwcfi	undefined, 145
	.dwcfi	undefined, 146
	.dwcfi	undefined, 147
	.dwcfi	undefined, 148
	.dwcfi	undefined, 149
	.dwcfi	undefined, 150
	.dwcfi	undefined, 151
	.dwcfi	undefined, 152
	.dwcfi	undefined, 153
	.dwcfi	undefined, 154
	.dwcfi	undefined, 155
	.dwcfi	undefined, 156
	.dwcfi	undefined, 157
	.dwcfi	undefined, 158
	.dwcfi	undefined, 159
	.dwcfi	undefined, 160
	.dwcfi	undefined, 161
	.dwcfi	undefined, 162
	.dwcfi	undefined, 163
	.dwcfi	undefined, 164
	.dwcfi	undefined, 165
	.dwcfi	undefined, 166
	.dwcfi	undefined, 167
	.dwcfi	undefined, 168
	.dwcfi	undefined, 169
	.dwcfi	undefined, 170
	.dwcfi	undefined, 171
	.dwcfi	undefined, 172
	.dwcfi	undefined, 173
	.dwcfi	undefined, 174
	.dwcfi	undefined, 175
	.dwcfi	undefined, 176
	.dwcfi	undefined, 177
	.dwcfi	undefined, 178
	.dwcfi	undefined, 179
	.dwcfi	undefined, 180
	.dwcfi	undefined, 181
	.dwcfi	undefined, 182
	.dwcfi	undefined, 183
	.dwcfi	undefined, 184
	.dwcfi	undefined, 185
	.dwcfi	undefined, 186
	.dwcfi	undefined, 187
	.dwcfi	undefined, 188
	.dwcfi	undefined, 189
	.dwcfi	undefined, 190
	.dwcfi	undefined, 191
	.dwcfi	undefined, 192
	.dwcfi	undefined, 193
	.dwcfi	undefined, 194
	.dwcfi	undefined, 195
	.dwcfi	undefined, 196
	.dwcfi	undefined, 197
	.dwcfi	undefined, 198
	.dwcfi	undefined, 199
	.dwcfi	undefined, 200
	.dwcfi	undefined, 201
	.dwcfi	undefined, 202
	.dwcfi	undefined, 203
	.dwcfi	undefined, 204
	.dwcfi	undefined, 205
	.dwcfi	undefined, 206
	.dwcfi	undefined, 207
	.dwcfi	undefined, 208
	.dwcfi	undefined, 209
	.dwcfi	undefined, 210
	.dwcfi	undefined, 211
	.dwcfi	undefined, 212
	.dwcfi	undefined, 213
	.dwcfi	undefined, 214
	.dwcfi	undefined, 215
	.dwcfi	undefined, 216
	.dwcfi	undefined, 217
	.dwcfi	undefined, 218
	.dwcfi	undefined, 219
	.dwcfi	undefined, 220
	.dwcfi	undefined, 221
	.dwcfi	undefined, 222
	.dwcfi	undefined, 223
	.dwcfi	undefined, 224
	.dwcfi	undefined, 225
	.dwcfi	undefined, 226
	.dwcfi	undefined, 227
	.dwcfi	undefined, 228
	.dwendentry

;***************************************************************
;* DWARF REGISTER MAP                                          *
;***************************************************************

$C$DW$1899	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("A0")
	.dwattr $C$DW$1899, DW_AT_location[DW_OP_reg0]
$C$DW$1900	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("A1")
	.dwattr $C$DW$1900, DW_AT_location[DW_OP_reg1]
$C$DW$1901	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("A2")
	.dwattr $C$DW$1901, DW_AT_location[DW_OP_reg2]
$C$DW$1902	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("A3")
	.dwattr $C$DW$1902, DW_AT_location[DW_OP_reg3]
$C$DW$1903	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("A4")
	.dwattr $C$DW$1903, DW_AT_location[DW_OP_reg4]
$C$DW$1904	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("A5")
	.dwattr $C$DW$1904, DW_AT_location[DW_OP_reg5]
$C$DW$1905	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("A6")
	.dwattr $C$DW$1905, DW_AT_location[DW_OP_reg6]
$C$DW$1906	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("A7")
	.dwattr $C$DW$1906, DW_AT_location[DW_OP_reg7]
$C$DW$1907	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("A8")
	.dwattr $C$DW$1907, DW_AT_location[DW_OP_reg8]
$C$DW$1908	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("A9")
	.dwattr $C$DW$1908, DW_AT_location[DW_OP_reg9]
$C$DW$1909	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("A10")
	.dwattr $C$DW$1909, DW_AT_location[DW_OP_reg10]
$C$DW$1910	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("A11")
	.dwattr $C$DW$1910, DW_AT_location[DW_OP_reg11]
$C$DW$1911	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("A12")
	.dwattr $C$DW$1911, DW_AT_location[DW_OP_reg12]
$C$DW$1912	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("A13")
	.dwattr $C$DW$1912, DW_AT_location[DW_OP_reg13]
$C$DW$1913	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("A14")
	.dwattr $C$DW$1913, DW_AT_location[DW_OP_reg14]
$C$DW$1914	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("A15")
	.dwattr $C$DW$1914, DW_AT_location[DW_OP_reg15]
$C$DW$1915	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("B0")
	.dwattr $C$DW$1915, DW_AT_location[DW_OP_reg16]
$C$DW$1916	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("B1")
	.dwattr $C$DW$1916, DW_AT_location[DW_OP_reg17]
$C$DW$1917	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("B2")
	.dwattr $C$DW$1917, DW_AT_location[DW_OP_reg18]
$C$DW$1918	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("B3")
	.dwattr $C$DW$1918, DW_AT_location[DW_OP_reg19]
$C$DW$1919	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("B4")
	.dwattr $C$DW$1919, DW_AT_location[DW_OP_reg20]
$C$DW$1920	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("B5")
	.dwattr $C$DW$1920, DW_AT_location[DW_OP_reg21]
$C$DW$1921	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("B6")
	.dwattr $C$DW$1921, DW_AT_location[DW_OP_reg22]
$C$DW$1922	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("B7")
	.dwattr $C$DW$1922, DW_AT_location[DW_OP_reg23]
$C$DW$1923	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("B8")
	.dwattr $C$DW$1923, DW_AT_location[DW_OP_reg24]
$C$DW$1924	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("B9")
	.dwattr $C$DW$1924, DW_AT_location[DW_OP_reg25]
$C$DW$1925	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("B10")
	.dwattr $C$DW$1925, DW_AT_location[DW_OP_reg26]
$C$DW$1926	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("B11")
	.dwattr $C$DW$1926, DW_AT_location[DW_OP_reg27]
$C$DW$1927	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("B12")
	.dwattr $C$DW$1927, DW_AT_location[DW_OP_reg28]
$C$DW$1928	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("B13")
	.dwattr $C$DW$1928, DW_AT_location[DW_OP_reg29]
$C$DW$1929	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("DP")
	.dwattr $C$DW$1929, DW_AT_location[DW_OP_reg30]
$C$DW$1930	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("SP")
	.dwattr $C$DW$1930, DW_AT_location[DW_OP_reg31]
$C$DW$1931	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("FP")
	.dwattr $C$DW$1931, DW_AT_location[DW_OP_regx 0x20]
$C$DW$1932	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("PC")
	.dwattr $C$DW$1932, DW_AT_location[DW_OP_regx 0x21]
$C$DW$1933	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("IRP")
	.dwattr $C$DW$1933, DW_AT_location[DW_OP_regx 0x22]
$C$DW$1934	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("IFR")
	.dwattr $C$DW$1934, DW_AT_location[DW_OP_regx 0x23]
$C$DW$1935	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("NRP")
	.dwattr $C$DW$1935, DW_AT_location[DW_OP_regx 0x24]
$C$DW$1936	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("A16")
	.dwattr $C$DW$1936, DW_AT_location[DW_OP_regx 0x25]
$C$DW$1937	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("A17")
	.dwattr $C$DW$1937, DW_AT_location[DW_OP_regx 0x26]
$C$DW$1938	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("A18")
	.dwattr $C$DW$1938, DW_AT_location[DW_OP_regx 0x27]
$C$DW$1939	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("A19")
	.dwattr $C$DW$1939, DW_AT_location[DW_OP_regx 0x28]
$C$DW$1940	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("A20")
	.dwattr $C$DW$1940, DW_AT_location[DW_OP_regx 0x29]
$C$DW$1941	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("A21")
	.dwattr $C$DW$1941, DW_AT_location[DW_OP_regx 0x2a]
$C$DW$1942	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("A22")
	.dwattr $C$DW$1942, DW_AT_location[DW_OP_regx 0x2b]
$C$DW$1943	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("A23")
	.dwattr $C$DW$1943, DW_AT_location[DW_OP_regx 0x2c]
$C$DW$1944	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("A24")
	.dwattr $C$DW$1944, DW_AT_location[DW_OP_regx 0x2d]
$C$DW$1945	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("A25")
	.dwattr $C$DW$1945, DW_AT_location[DW_OP_regx 0x2e]
$C$DW$1946	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("A26")
	.dwattr $C$DW$1946, DW_AT_location[DW_OP_regx 0x2f]
$C$DW$1947	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("A27")
	.dwattr $C$DW$1947, DW_AT_location[DW_OP_regx 0x30]
$C$DW$1948	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("A28")
	.dwattr $C$DW$1948, DW_AT_location[DW_OP_regx 0x31]
$C$DW$1949	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("A29")
	.dwattr $C$DW$1949, DW_AT_location[DW_OP_regx 0x32]
$C$DW$1950	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("A30")
	.dwattr $C$DW$1950, DW_AT_location[DW_OP_regx 0x33]
$C$DW$1951	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("A31")
	.dwattr $C$DW$1951, DW_AT_location[DW_OP_regx 0x34]
$C$DW$1952	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("B16")
	.dwattr $C$DW$1952, DW_AT_location[DW_OP_regx 0x35]
$C$DW$1953	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("B17")
	.dwattr $C$DW$1953, DW_AT_location[DW_OP_regx 0x36]
$C$DW$1954	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("B18")
	.dwattr $C$DW$1954, DW_AT_location[DW_OP_regx 0x37]
$C$DW$1955	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("B19")
	.dwattr $C$DW$1955, DW_AT_location[DW_OP_regx 0x38]
$C$DW$1956	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("B20")
	.dwattr $C$DW$1956, DW_AT_location[DW_OP_regx 0x39]
$C$DW$1957	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("B21")
	.dwattr $C$DW$1957, DW_AT_location[DW_OP_regx 0x3a]
$C$DW$1958	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("B22")
	.dwattr $C$DW$1958, DW_AT_location[DW_OP_regx 0x3b]
$C$DW$1959	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("B23")
	.dwattr $C$DW$1959, DW_AT_location[DW_OP_regx 0x3c]
$C$DW$1960	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("B24")
	.dwattr $C$DW$1960, DW_AT_location[DW_OP_regx 0x3d]
$C$DW$1961	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("B25")
	.dwattr $C$DW$1961, DW_AT_location[DW_OP_regx 0x3e]
$C$DW$1962	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("B26")
	.dwattr $C$DW$1962, DW_AT_location[DW_OP_regx 0x3f]
$C$DW$1963	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("B27")
	.dwattr $C$DW$1963, DW_AT_location[DW_OP_regx 0x40]
$C$DW$1964	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("B28")
	.dwattr $C$DW$1964, DW_AT_location[DW_OP_regx 0x41]
$C$DW$1965	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("B29")
	.dwattr $C$DW$1965, DW_AT_location[DW_OP_regx 0x42]
$C$DW$1966	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("B30")
	.dwattr $C$DW$1966, DW_AT_location[DW_OP_regx 0x43]
$C$DW$1967	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("B31")
	.dwattr $C$DW$1967, DW_AT_location[DW_OP_regx 0x44]
$C$DW$1968	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("AMR")
	.dwattr $C$DW$1968, DW_AT_location[DW_OP_regx 0x45]
$C$DW$1969	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("CSR")
	.dwattr $C$DW$1969, DW_AT_location[DW_OP_regx 0x46]
$C$DW$1970	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("ISR")
	.dwattr $C$DW$1970, DW_AT_location[DW_OP_regx 0x47]
$C$DW$1971	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("ICR")
	.dwattr $C$DW$1971, DW_AT_location[DW_OP_regx 0x48]
$C$DW$1972	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("IER")
	.dwattr $C$DW$1972, DW_AT_location[DW_OP_regx 0x49]
$C$DW$1973	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("ISTP")
	.dwattr $C$DW$1973, DW_AT_location[DW_OP_regx 0x4a]
$C$DW$1974	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("IN")
	.dwattr $C$DW$1974, DW_AT_location[DW_OP_regx 0x4b]
$C$DW$1975	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("OUT")
	.dwattr $C$DW$1975, DW_AT_location[DW_OP_regx 0x4c]
$C$DW$1976	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("ACR")
	.dwattr $C$DW$1976, DW_AT_location[DW_OP_regx 0x4d]
$C$DW$1977	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("ADR")
	.dwattr $C$DW$1977, DW_AT_location[DW_OP_regx 0x4e]
$C$DW$1978	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("FADCR")
	.dwattr $C$DW$1978, DW_AT_location[DW_OP_regx 0x4f]
$C$DW$1979	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("FAUCR")
	.dwattr $C$DW$1979, DW_AT_location[DW_OP_regx 0x50]
$C$DW$1980	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("FMCR")
	.dwattr $C$DW$1980, DW_AT_location[DW_OP_regx 0x51]
$C$DW$1981	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("GFPGFR")
	.dwattr $C$DW$1981, DW_AT_location[DW_OP_regx 0x52]
$C$DW$1982	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("DIER")
	.dwattr $C$DW$1982, DW_AT_location[DW_OP_regx 0x53]
$C$DW$1983	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("REP")
	.dwattr $C$DW$1983, DW_AT_location[DW_OP_regx 0x54]
$C$DW$1984	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("TSCL")
	.dwattr $C$DW$1984, DW_AT_location[DW_OP_regx 0x55]
$C$DW$1985	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("TSCH")
	.dwattr $C$DW$1985, DW_AT_location[DW_OP_regx 0x56]
$C$DW$1986	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("ARP")
	.dwattr $C$DW$1986, DW_AT_location[DW_OP_regx 0x57]
$C$DW$1987	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("ILC")
	.dwattr $C$DW$1987, DW_AT_location[DW_OP_regx 0x58]
$C$DW$1988	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("RILC")
	.dwattr $C$DW$1988, DW_AT_location[DW_OP_regx 0x59]
$C$DW$1989	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("DNUM")
	.dwattr $C$DW$1989, DW_AT_location[DW_OP_regx 0x5a]
$C$DW$1990	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("SSR")
	.dwattr $C$DW$1990, DW_AT_location[DW_OP_regx 0x5b]
$C$DW$1991	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("GPLYA")
	.dwattr $C$DW$1991, DW_AT_location[DW_OP_regx 0x5c]
$C$DW$1992	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("GPLYB")
	.dwattr $C$DW$1992, DW_AT_location[DW_OP_regx 0x5d]
$C$DW$1993	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("TSR")
	.dwattr $C$DW$1993, DW_AT_location[DW_OP_regx 0x5e]
$C$DW$1994	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("ITSR")
	.dwattr $C$DW$1994, DW_AT_location[DW_OP_regx 0x5f]
$C$DW$1995	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("NTSR")
	.dwattr $C$DW$1995, DW_AT_location[DW_OP_regx 0x60]
$C$DW$1996	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("EFR")
	.dwattr $C$DW$1996, DW_AT_location[DW_OP_regx 0x61]
$C$DW$1997	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("ECR")
	.dwattr $C$DW$1997, DW_AT_location[DW_OP_regx 0x62]
$C$DW$1998	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("IERR")
	.dwattr $C$DW$1998, DW_AT_location[DW_OP_regx 0x63]
$C$DW$1999	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("DMSG")
	.dwattr $C$DW$1999, DW_AT_location[DW_OP_regx 0x64]
$C$DW$2000	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("CMSG")
	.dwattr $C$DW$2000, DW_AT_location[DW_OP_regx 0x65]
$C$DW$2001	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("DT_DMA_ADDR")
	.dwattr $C$DW$2001, DW_AT_location[DW_OP_regx 0x66]
$C$DW$2002	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("DT_DMA_DATA")
	.dwattr $C$DW$2002, DW_AT_location[DW_OP_regx 0x67]
$C$DW$2003	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("DT_DMA_CNTL")
	.dwattr $C$DW$2003, DW_AT_location[DW_OP_regx 0x68]
$C$DW$2004	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("TCU_CNTL")
	.dwattr $C$DW$2004, DW_AT_location[DW_OP_regx 0x69]
$C$DW$2005	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("RTDX_REC_CNTL")
	.dwattr $C$DW$2005, DW_AT_location[DW_OP_regx 0x6a]
$C$DW$2006	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("RTDX_XMT_CNTL")
	.dwattr $C$DW$2006, DW_AT_location[DW_OP_regx 0x6b]
$C$DW$2007	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("RTDX_CFG")
	.dwattr $C$DW$2007, DW_AT_location[DW_OP_regx 0x6c]
$C$DW$2008	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("RTDX_RDATA")
	.dwattr $C$DW$2008, DW_AT_location[DW_OP_regx 0x6d]
$C$DW$2009	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("RTDX_WDATA")
	.dwattr $C$DW$2009, DW_AT_location[DW_OP_regx 0x6e]
$C$DW$2010	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("RTDX_RADDR")
	.dwattr $C$DW$2010, DW_AT_location[DW_OP_regx 0x6f]
$C$DW$2011	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("RTDX_WADDR")
	.dwattr $C$DW$2011, DW_AT_location[DW_OP_regx 0x70]
$C$DW$2012	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("MFREG0")
	.dwattr $C$DW$2012, DW_AT_location[DW_OP_regx 0x71]
$C$DW$2013	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("DBG_STAT")
	.dwattr $C$DW$2013, DW_AT_location[DW_OP_regx 0x72]
$C$DW$2014	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("BRK_EN")
	.dwattr $C$DW$2014, DW_AT_location[DW_OP_regx 0x73]
$C$DW$2015	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("HWBP0_CNT")
	.dwattr $C$DW$2015, DW_AT_location[DW_OP_regx 0x74]
$C$DW$2016	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("HWBP0")
	.dwattr $C$DW$2016, DW_AT_location[DW_OP_regx 0x75]
$C$DW$2017	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("HWBP1")
	.dwattr $C$DW$2017, DW_AT_location[DW_OP_regx 0x76]
$C$DW$2018	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("HWBP2")
	.dwattr $C$DW$2018, DW_AT_location[DW_OP_regx 0x77]
$C$DW$2019	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("HWBP3")
	.dwattr $C$DW$2019, DW_AT_location[DW_OP_regx 0x78]
$C$DW$2020	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("OVERLAY")
	.dwattr $C$DW$2020, DW_AT_location[DW_OP_regx 0x79]
$C$DW$2021	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("PC_PROF")
	.dwattr $C$DW$2021, DW_AT_location[DW_OP_regx 0x7a]
$C$DW$2022	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("ATSR")
	.dwattr $C$DW$2022, DW_AT_location[DW_OP_regx 0x7b]
$C$DW$2023	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("TRR")
	.dwattr $C$DW$2023, DW_AT_location[DW_OP_regx 0x7c]
$C$DW$2024	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("TCRR")
	.dwattr $C$DW$2024, DW_AT_location[DW_OP_regx 0x7d]
$C$DW$2025	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("DESR")
	.dwattr $C$DW$2025, DW_AT_location[DW_OP_regx 0x7e]
$C$DW$2026	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("DETR")
	.dwattr $C$DW$2026, DW_AT_location[DW_OP_regx 0x7f]
$C$DW$2027	.dwtag  DW_TAG_TI_assign_register, DW_AT_name("CIE_RETA")
	.dwattr $C$DW$2027, DW_AT_location[DW_OP_regx 0xe4]
	.dwendtag $C$DW$CU

