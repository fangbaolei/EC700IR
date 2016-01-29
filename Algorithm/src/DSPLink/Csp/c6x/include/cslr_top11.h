#ifndef _CSLR_TOP11_1_H_
#define _CSLR_TOP11_1_H_

#include <cslr.h>

#include "tistdtypes.h"


/* Minimum unit = 4 bytes */

/**************************************************************************\
* Register Overlay Structure
\**************************************************************************/
typedef struct  {
    volatile Uint32 PID;
} CSL_Top11Regs;

/**************************************************************************\
* Field Definition Macros
\**************************************************************************/

/* PID */

#define CSL_TOP11_PID_PID_MASK           (0xFFFFFFFFu)
#define CSL_TOP11_PID_PID_SHIFT          (0x00000000u)
#define CSL_TOP11_PID_PID_RESETVAL       (0x4C000101u)

#define CSL_TOP11_PID_RESETVAL           (0x4C000101u)

#endif
