/* ========================================================================== */
/*                                                                            */
/*  TEXAS INSTRUMENTS, INC.                                                   */
/*                                                                            */
/*  NAME                                                                      */
/*     alg_create.c                                                           */
/*                                                                            */
/*  DESCRIPTION                                                               */
/*    This file contains a simple implementation of the ALG_create API        */
/*    operation.                                                              */
/*                                                                            */
/*  COPYRIGHT NOTICES                                                         */
/*   Copyright (C) 1996, MPEG Software Simulation Group. All Rights           */
/*   Reserved.                                                                */
/*                                                                            */
/*   Copyright (c) 2006 Texas Instruments Inc.  All rights reserved.     */
/*   Exclusive property of the Video & Imaging Products, Emerging End         */
/*   Equipment group of Texas Instruments India Limited. Any handling,        */
/*   use, disclosure, reproduction, duplication, transmission, or storage     */
/*   of any part of this work by any means is subject to restrictions and     */
/*   prior written permission set forth in TI's program license agreements    */
/*   and associated software documentation.                                   */
/*                                                                            */
/*   This copyright notice, restricted rights legend, or other proprietary    */
/*   markings must be reproduced without modification in any authorized       */
/*   copies of any part of this work.  Removal or modification of any part    */
/*   of this notice is prohibited.                                            */
/*                                                                            */
/*   U.S. Patent Nos. 5,283,900  5,392,448                                    */
/* -------------------------------------------------------------------------- */
/*            Copyright (c) 2006 Texas Instruments, Incorporated.             */
/*                           All Rights Reserved.                             */
/* ========================================================================== */
/* "@(#) XDAS 2.12 05-21-01 (__imports)" */
//static const char Copyright[] = "Copyright (C) 2003 Texas Instruments "
//                                "Incorporated. All rights Reserved."; 
#ifdef _TMS320C6400
/* -------------------------------------------------------------------------- */
/* Assigning text section to allow better control on placing function in      */
/* memory of our choice and our alignment. The details about placement of     */
/* these section can be obtained from the linker command file "mpeg2enc.cmd". */
/* -------------------------------------------------------------------------- */
//#pragma CODE_SECTION(ALG_create, ".text:create")
//#pragma CODE_SECTION(ALG_delete, ".text:delete")
#endif

#include <std.h>
#include <alg.h>
#include <ialg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <_alg.h>


#define JPEGDEC_MEM_NUMBER		10

/*
*  ======== ALG_create ========
*/
ALG_Handle ALG_create(IALG_Fxns *fxns, IALG_Handle p, IALG_Params *pParams)
{
	IALG_MemRec memTab[ JPEGDEC_MEM_NUMBER ];
	Int n;
	ALG_Handle alg;
	IALG_Fxns *fxnsPtr;

	if (fxns != NULL) 
	{
		n = fxns->algNumAlloc != NULL ? fxns->algNumAlloc() : IALG_DEFMEMRECS;
		memset( memTab, 0, JPEGDEC_MEM_NUMBER * sizeof( IALG_MemRec ) );

		n = fxns->algAlloc(pParams, &fxnsPtr, memTab);
		if (( n <= 0 ) ) 
		{
			return (NULL);
		}
		if (_ALG_allocMemory(memTab, n)) 
		{
			alg = (IALG_Handle)memTab[0].base;
			alg->fxns = fxns;
			if (fxns->algInit(alg, memTab, p, pParams) == IALG_EOK) 
			{
				return (alg);
			}
			fxns->algFree(alg, memTab);
			_ALG_freeMemory(memTab, n);
		}
	}

	return (NULL);
}

/*
*  ======== ALG_delete ========
*/
Void ALG_delete(ALG_Handle alg)
{
	IALG_MemRec memTab[ JPEGDEC_MEM_NUMBER ];
	Int n;
	IALG_Fxns *fxns;

	if (alg != NULL && alg->fxns != NULL) {
		fxns = alg->fxns;
		n = fxns->algNumAlloc != NULL ? fxns->algNumAlloc() : IALG_DEFMEMRECS;
		memset( memTab, 0, JPEGDEC_MEM_NUMBER * sizeof( IALG_MemRec ) );
		memTab[0].base = alg;
		n = fxns->algFree(alg, memTab);
		_ALG_freeMemory(memTab, n);
	}
}
/* ========================================================================== */
/* End of file : alg_create.c                                                 */
/* -------------------------------------------------------------------------- */
/*            Copyright (c) 2006 Texas Instruments, Incorporated.             */
/*                           All Rights Reserved.                             */
/* ========================================================================== */

