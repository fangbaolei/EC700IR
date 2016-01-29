/****************************************************************************/

/*  lnk.cmd   v1.01                                                         */

/*  Copyright (c) 1996-1997  Texas Instruments Incorporated                 */

/****************************************************************************/

/* -cr */
-l DSPBIOS/DSPLinkMercurycfg.cmd

-l ../Csp/Csl/lib/dspcsl_davinci.lib
-l ../Csp/Csl/lib/csl_sysservicesdavinci.lib

-l ../../CamDsp/lib/DmaCopy/dman3.a64P
-l ../../CamDsp/lib/DmaCopy/acpy3.a64P

-c

SECTIONS
{	
	.sysReset > RESET_VECTOR
	.csl_vect > IRAM
	.template 	  > IRAM
	.jpd_table				> DDR2
	.jpd_code_external		> DDR2
	fastdata > IRAM
	.global_tbl > IRAM
}
