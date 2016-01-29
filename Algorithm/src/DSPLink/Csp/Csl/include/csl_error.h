/*****************************************************\
 *  Copyright 2003, Texas Instruments Incorporated.  *
 *  All rights reserved.                             *
 *  Restricted rights to use, duplicate or disclose  *
 *  this   code   are  granted   through  contract.  *
 \*****************************************************/

#ifndef _CSL_ERROR_H_
#define _CSL_ERROR_H_

#include <csl_resId.h>

/* Below Error codes are Global across all CSL Modules. */
#define CSL_SOK			        (1)		/* Success */
#define CSL_ESYS_FAIL		    (-1)		/* Generic failure */
#define CSL_ESYS_INUSE		    (-2) 		/* Peripheral resource is already in use */
#define CSL_ESYS_XIO		    (-3)		/* Encountered a shared I/O(XIO) pin conflict */
#define CSL_ESYS_OVFL		    (-4)		/* Encoutered CSL system resource overflow */
#define CSL_ESYS_BADHANDLE	    (-5)		/* Handle passed to CSL was invalid */
#define CSL_ESYS_INVPARAMS      (-6)        /* invalid parameters */
#define CSL_ESYS_INVCMD         (-7)        /* invalid command */
#define CSL_ESYS_INVQUERY       (-8)        /* invalid query */
#define CSL_ESYS_NOTSUPPORTED   (-9)        /* action not supported */


/* Error codes individual to various modules. */

/* Error code for UART, individual error would be assigned as
 * eg: #define CSL_E<Peripheral name>i_<error code>	CSL_EUART_FIRST - 0
 */
#define CSL_EMCBSP_FIRST    -( ((CSL_MCBSP_ID + 1) << 5 ) + 1 )
#define CSL_EMCBSP_LAST	    -( (CSL_MCBSP_ID + 1) << 6 )

#define CSL_EEDMA_FIRST	    -( ((CSL_EDMA_ID + 1) << 5 ) + 1 )
#define CSL_EEDMA_LAST	    -( (CSL_EDMA_ID + 1) << 6 )

#endif /* _CSL_ERROR_H_ */

