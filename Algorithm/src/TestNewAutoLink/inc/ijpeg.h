/*
 *  Copyright 1999 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *  U.S. Patent Nos. 5,283,900  5,392,448
 */
/* "@(#) XDAS 2.00 11-12-99 (__imports)" */
/*
 *  ======== ijpeg.h ========
 *  This header defines all types and constants common to all JPEG
 *  encoder and decoder modules.
 */
#ifndef IJPEG_
#define IJPEG_

/*
 *  ======== IJPEG_Cmd ========
 *  This structure defines the control commands for the JPEG encoder
 *  and decoder instance objects.
 */
typedef enum IJPEG_Cmd {
    IJPEG_GETSTATUS,
    IJPEG_SETSTATUS
} IJPEG_Cmd;


#endif  /* IJPEG_ */
