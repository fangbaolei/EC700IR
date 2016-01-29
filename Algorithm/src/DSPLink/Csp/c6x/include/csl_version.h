/*****************************************************\
 *  Copyright 2003, Texas Instruments Incorporated.  *
 *  All rights reserved.                             *
 *  Restricted rights to use, duplicate or disclose  *
 *  this   code   are  granted   through  contract.  *
\*****************************************************/

/*  @(#) PSP/CSL 3.00.02.01[5912PG1_0] (2004-04-25)  */

#ifndef _CSL_VERSION_H_
#define _CSL_VERSION_H_

#define CSL_VERSION_ID          (0x03000201)    /* 0xAABBCCDD -> Arch (AA); API Changes (BB); Major (CC); Minor (DD) */
#define CSL_VERSION_STR         "@# CSL Revision: 3.00.02.01;"

extern void CSL_version3_00_02_01( );


#define CSL_CHIP_ID             (0x300)
#define CSL_CHIP_STR            "Chip: HIBARI, PG 1.0"

extern void CSL_chipHibariPG1_0(void);


static void CSL_versionCheck(void)
{
    CSL_version3_00_02_01();
    CSL_chipHibariPG1_0();
}

#endif /* _CSL_VERSION_H_ */

/* Rev.No.   Date/Time               ECN No.          Modifier      */
/* -------   ---------               -------          --------      */

/* 1          Aug 11:12:00 9         2274             xkeshavm      */
/*                                                                  */
/* To upload API CSL 0.51 Release                                   */
/********************************************************************/ 
