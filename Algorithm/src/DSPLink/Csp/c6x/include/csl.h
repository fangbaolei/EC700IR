/*****************************************************\
 *  Copyright 2003, Texas Instruments Incorporated.  *
 *  All rights reserved.                             *
 *  Restricted rights to use, duplicate or disclose  *
 *  this   code   are  granted   through  contract.  *
 \*****************************************************/

#ifndef _CSL_H_
#define _CSL_H_

#include <csl_types.h>
#include <csl_resId.h>
#include <csl_error.h>
#include <cslr.h>

typedef void *  CSL_ResHandle;

typedef struct {
    CSL_OpenMode openMode;
    CSL_Uid uid;
    CSL_Xio xio;
} CSL_ResAttrs;

typedef void (* CSL_Phy2VirtHandler)(CSL_ResHandle);
typedef void (* CSL_Virt2PhyHandler)(CSL_ResHandle);
typedef void (* CSL_AltRouteHandler)(CSL_ResHandle);

void
    CSL_sysInit (
        void
);

#endif /* _CSL_H_ */


/* Rev.No.   Date/Time               ECN No.          Modifier      */
/* -------   ---------               -------          --------      */

/* 4          Aug 10:08:55 9         2272             xkeshavm      */
/*                                                                  */
/* To upload API CSL 0.51 Release                                   */
/********************************************************************/ 
