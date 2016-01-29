/*
 *  Copyright 2003 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *  
 */
/* "@(#) ReferenceFrameworks 2.10.00.11 04-30-03 (swat-d15)" */
/*
 *  ======== sscr.h ========
 *  This module enables users to manage shared scratch memory for
 *  XDAIS algorithms.  It uses the idea of buckets.  Each bucket 
 *  holds one buffer pointer, the size of the buffer and the number
 *  of current users of the buffer.  The number of buckets is determine
 *  by the user via SSCR_setup().  SSCR does not provide any protection
 *  of the scratch buffer.
 *
 *  The following is the calling sequence of the SSCR APIs:
 *
 *                                                -----------
 *                                                |         |
 *                                                v         |
 *  --------------     --------------     ----------------  |
 *  | SSCR_init  | --> | SSCR_setup | --> | SSCR_prime   |---
 *  --------------     --------------     ----------------
 *                                                |
 *                                                |  ----------
 *                                                |  |        |
 *                                                v  v        |
 *                                        ------------------   |
 *                                        | SSCR_createBuf@|----
 *                                        | SSCR_getBuf    |
 *                                        | SSCR_deleteBuf@|
 *                                        ----------------   
 *                                                |  
 *                                                |  
 *                                                v  
 *                                        ----------------
 *                                        | SSCR_exit    |
 *                                        ----------------   
 * Additional notes
 *   @ = must have created a scratch buffer before you can delete it. 
 */

#ifndef SSCR_
#define SSCR_

#include <ialg.h>

#ifdef __cplusplus
extern "C" {
#endif


/* Default Heap ID where the scratch buffers will be allocated from. */
#define _SSCR_INVALIDHEAP  (-1)

/* 
 *  Maximum number of memTab descriptors returned by an algorithm.
 *  This allows stack based instead of dynamically allocated
 *  memTab arrays.  Helps reduce fragmentation.
 */
#define _SSCR_MAXMEMRECS   (16)

/* Used to denote that the user supplied the memory for the overlay */
#define _SSCR_USERDEFINED  ((Uns)-1)

/* 
 *  Structure for storing the size and address of the scratch buffer. 
 *  buffer: Pointer to the scratch buffer.
 *  size:   The size of the scratch buffer.
 *  count:  Number of users of this bucket. This is used to determine
 *          when to free the scratch buffer in a dynamic system.  It 
 *          can also aid in debugging a static system.
 */ 
typedef struct SSCR_Bucket {    
    Ptr   buffer;   
    Uns   size;     
    Uns   count;    
} SSCR_Bucket;

/* Private global variables for the SSCR module. */
extern Int _SSCR_heapId;
extern Int _SSCR_bucketCnt;
extern SSCR_Bucket *_SSCR_bucketList;

/* Create the scratch buffer (or return it if already allocated) */
extern Ptr  SSCR_createBuf(Uns scrBucketIndex, Uns *scrSize);

/* Delete the scratch buffer. */
extern Bool SSCR_deleteBuf(Uns scrBucketIndex);

/* Exit the SSCR module. */
extern Void SSCR_exit(Void);

/* Get the size of and pointer to the scratch buffer. */
extern Ptr  SSCR_getBuf(Uns scrBucketIndex, Uns *scrSize);

/* Init the SSCR module. */
extern Void SSCR_init(Void);

/* Determine worst-case scratch usage for an algorithm instance */
extern Bool SSCR_prime(Uns          scrBucketIndex,
                       IALG_Fxns   *fxns,
                       IALG_Params *params);

/* Set-up the SSCR module. */
extern Bool SSCR_setup(Int heapId, Uns bucketCnt, 
                       Ptr bucketBuf[], Uns bucketSize[]);

#ifdef __cplusplus
}
#endif // extern "C" 

#endif // SSCR_

