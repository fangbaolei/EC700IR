/* ========================================================================== */
/*                                                                            */
/*  TEXAS INSTRUMENTS, INC.                                                   */
/*                                                                            */
/*  NAME                                                                      */
/*    jpegdec_ti.h                                                            */
/*                                                                            */
/*  DESCRIPTION                                                               */
/*     Interface for the JPEGDEC_TI module; TI's implementation 			  */
/*  of the IJPEGDEC interface												  */			
/*
 *  Copyright 2006 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *
 */

#ifndef JPEGDEC_TI_
#define JPEGDEC_TI_

#include <ijpegdec.h>

#ifndef EXT_MEM
#include <idma3.h>

/*
 *  ======== FCPY_TI_IDMA3 ========
 *  TI's implementation of JPEGDEC IDMA3 interface
 */
extern IDMA3_Fxns JPEGDEC_TI_IDMA3;

#endif

/*
 *  ======== JPEGDEC_TI_IALG ========
 *  TI's implementation of the IALG interface for JPEGDEC
 */
extern IALG_Fxns JPEGDEC_TI_IALG;

/*
 *  ======== JPEGDEC_TI_IJPEGDEC ========
 *  TI's implementation of the IJPEGDEC interface
 */
extern IJPEGDEC_Fxns JPEGDEC_TI_IJPEGDEC;

extern Void JPEGDEC_TI_init(Void);
extern Void JPEGDEC_TI_exit(Void);

#endif	/* JPEGDEC_TI_ */
