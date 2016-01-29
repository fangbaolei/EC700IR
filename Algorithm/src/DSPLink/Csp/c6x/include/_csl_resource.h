/*****************************************************\
 *  Copyright 2003, Texas Instruments Incorporated.  *
 *  All rights reserved.                             *
 *  Restricted rights to use, duplicate or disclose  *
 *  this   code   are  granted   through  contract.  *
 *                                                   *
 * "@(#) PSP/CSL  3.0.0.0  (2003-09-30)              *
\*****************************************************/

#ifndef __CSL_RESOURCE_H_
#define __CSL_RESOURCE_H_

#include <csl_types.h>
#include <csl.h>

typedef Uint16      CSL_CriticalSectionState;

CSL_CriticalSectionState
    _CSL_beginCriticalSection (
        void
);

void
    _CSL_endCriticalSection (
        CSL_CriticalSectionState
);

CSL_ResHandle
    _CSL_certifyOpen (
        CSL_ResHandle   rHandle,
        CSL_Status *    st
);

CSL_Status
    _CSL_certifyClose (
        CSL_ResHandle   rHandle
);

CSL_Test
    _CSL_resourceTest (
        CSL_Uid         resId
);

CSL_Test
    _CSL_xioTest (
        CSL_Xio         xio
);

void
    _CSL_xioSet (
        CSL_Xio         xio
);

void
    _CSL_resourceSet (
        CSL_Uid         resId
);

void
    _CSL_resourceClear (
        CSL_Uid         resId
);

void
    _CSL_xioClear (
        CSL_Xio         xio
);

#endif /* __CSL_RESOURCE_H_ */

/* Rev.No.   Date/Time               ECN No.          Modifier      */
/* -------   ---------               -------          --------      */

/* 4          Aug 10:08:52 9         2272             xkeshavm      */
/*                                                                  */
/* To upload API CSL 0.51 Release                                   */
/********************************************************************/ 
