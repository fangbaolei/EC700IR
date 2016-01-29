			.sect	".text"	
				
			.global _PRECISE_DelayUS
			.global _PRECISE_DelayTNS
			.global _PRECISE_InitDEV
			
_PRECISE_DelayUS:
			MV		.S1		A4, A0
	[!A0]	BNOP	.S2 	delayEnd, 5
			SUB		.S1		A4, 2, A4
tar_loop:
			MVK		.S2		170, B4			
tar_loop1:
			BDEC	.S2		tar_loop1, B4
			NOP				5
			BDEC	.S1		tar_loop, A4
			NOP				5
delayEnd:			
			RETNOP  .S2     B3,5
			

_PRECISE_DelayTNS:
loop1:		BDEC	.S1		loop1, A4
			NOP		5
			
			RETNOP	.S2	B3,5

_PRECISE_InitDEV:
			MVKL	.S2		4, B4
||			MVKL	.S1		0550h, A8

			MVKH	.S2		2ac0000h, B4
||			MVKH	.S1		0C0100000h, A8
			
			MVKL	.S2		0b00h, B6
||			MV		.D1X	B4, A6
			
			MVKH	.S2		0f0a0000h, B6
||			MVKL	.S1		0014h, A10
||			ADD		.D1		A6, 4, A6
			
			STW		.D2T2	B6, *B4[0]		
			STW		.D1T1	A8, *A6[0]			
		

			MVKH	.S1		02AC0000h, A10			
			LDW		.D1T1	*A10[0], A8
			NOP				5

			RETNOP	.S2		B3, 5
			                       
			.end
