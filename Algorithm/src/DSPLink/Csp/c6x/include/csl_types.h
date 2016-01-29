/*****************************************************\
 *  Copyright 2003, Texas Instruments Incorporated.  *
 *  All rights reserved.                             *
 *  Restricted rights to use, duplicate or disclose  *
 *  this   code   are  granted   through  contract.  *
 \*****************************************************/


#ifndef _CSL_TYPES_H_
#define _CSL_TYPES_H_

#include <tistdtypes.h>

typedef Int16		CSL_Uid;
typedef Int16		CSL_ModuleId;
typedef Uint32 		CSL_Xio;

typedef Uint16      CSL_BitMask16;
typedef Uint32      CSL_BitMask32;

typedef volatile Uint16 CSL_Reg16;
typedef volatile Uint32 CSL_Reg32;

typedef Int16		CSL_Status;
typedef Int16		CSL_InstNum;
typedef Int16 		CSL_ChaNum;

typedef enum {
  CSL_EXCLUSIVE = 0,
  CSL_SHARED    = 1
} CSL_OpenMode;

typedef enum {
  CSL_FAIL      = 0,
  CSL_PASS      = 1
} CSL_Test;

#ifndef NULL
#define NULL    (0)
#endif


#endif /* _CSL_TYPES_H_ */

/* Rev.No.   Date/Time               ECN No.          Modifier      */
/* -------   ---------               -------          --------      */

/* 1          Aug 11:11:59 9         2274             xkeshavm      */
/*                                                                  */
/* To upload API CSL 0.51 Release                                   */
/********************************************************************/ 
