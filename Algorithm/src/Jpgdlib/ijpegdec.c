/* ========================================================================== */
/*                                                                            */
/*  TEXAS INSTRUMENTS, INC.                                                   */
/*                                                                            */
/*  NAME                                                                      */
/*     ijpegdec.c                                                             */
/*                                                                            */
/*  DESCRIPTION                                                               */
/*    This file defines the default parameter structure for ijpegdec.h        */
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


#include <stdio.h>
#include <ijpegdech.h>

/*
 *  ======== JPEGDEC_PARAMS ========
 *  This constant structure defines the default parameters for JPEGDEC
 *  objects
 */
 


IJPEGDEC_Status IJPEGDEC_STATUS = {
    sizeof(IJPEGDEC_Status),
	 
};

IIMGDEC_InArgs IJPEGDEC_INARGS = {
    sizeof(IJPEGDEC_Status),
	 
}; 
