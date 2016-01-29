#ifndef _CSLR_TOP10_1_H_
#define _CSLR_TOP10_1_H_

#include <cslr.h>

#include "tistdtypes.h"


/* Minimum unit = 4 bytes */

/**************************************************************************\
* Register Overlay Structure
\**************************************************************************/
typedef struct  {
    volatile Uint32 PID;
} CSL_Top10Regs;

/**************************************************************************\
* Field Definition Macros
\**************************************************************************/

/* PID */

#define CSL_TOP10_PID_PID_MASK           (0xFFFFFFFFu)
#define CSL_TOP10_PID_PID_SHIFT          (0x00000000u)
#define CSL_TOP10_PID_PID_RESETVAL       (0x4C000100u)

#define CSL_TOP10_PID_RESETVAL           (0x4C000100u)

#endif
