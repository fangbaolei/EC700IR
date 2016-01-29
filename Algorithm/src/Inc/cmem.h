/*
 *  Copyright 2008 by Texas Instruments Incorporated.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; version 2 of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

/**
 * @file    cmem.h
 * @brief   Describes the interface to the contiguous memory allocator.
 *
 * The cmem user interface library wraps file system calls to an associated
 * kernel module (cmemk.ko), which needs to be loaded in order for calls to
 * this library to succeed.
 *
 * The following is an example of installing the cmem kernel module:
 * 
 * @verbatim /sbin/insmod cmemk.ko pools=4x30000,2x500000 phys_start=0x0 phys_end=0x3000000 @endverbatim
 *     - phys_start and phys_end must be specified in hexadecimal format
 *     - pools must be specified using decimal format (for both number and
 *       size), since using hexadecimal format would visually clutter the
 *       specification due to the use of "x" as a token separator
 *
 * This particular command creates 2 pools. The first pool is created with 4
 * buffers of size 30000 bytes and the second pool is created with 2 buffers
 * of size 500000 bytes. The CMEM pool buffers start at 0x0 and end at
 * 0x3000000 (max). 
 *
 * Pool buffers are aligned on a module-dependent boundary, and their sizes are
 * rounded up to this same boundary.  This applies to each buffer within a
 * pool.  The total space used by an individual pool will therefore be greater
 * than (or equal to) the exact amount requested in the installation of the
 * module.
 *
 * The poolid used in the driver calls would be 0 for the first pool and 1 for
 * the second pool.
 *
 * Pool allocations can be requested explicitly by pool number, or more
 * generally by just a size.  For size-based allocations, the pool which best
 * fits the requested size is automatically chosen.
 *
 * There is also support for a general purpose heap.  In addition to the 2
 * pools described above, a general purpose heap block is created from which
 * allocations of any size can be requested.  Internally, allocation sizes are
 * rounded up to a module-dependent boundary and allocation addresses are
 * aligned either to this same boundary or to the requested alignment
 * (whichever is greater).
 *
 * The size of the heap block is the amount of CMEM memory remaining after all
 * pool allocations.  If more heap space is needed than is available after pool
 * allocations, you must reduce the amount of CMEM memory granted to the pools.
 *
 * Since the CMEM interface library doesn't use the GT tracing facility, there
 * is one configuration option available for the CMEM module to control
 * whether the debug or release interface library is used for building the
 * application.  This config parameter is named 'debug' and is of type bool,
 * and the default value is 'false'.
 *
 * The following line is an example of enabling usage of the debug interface
 * library:
 *     var cmem = xdc.useModule('ti.sdo.linuxutils.cmem.CMEM');
 *     cmem.debug = true;
 * This will enable "CMEM Debug" statements to be printed to stdout.
 */

#ifndef _CMEM_H
#define _CMEM_H

#if defined (__cplusplus)
extern "C" {
#endif

#define CMEM_VERSION    0x02000000

/* ioctl cmd "flavors" */
#define CMEM_WB                         0x0100
#define CMEM_INV                        0x0200
#define CMEM_HEAP                       0x0400  /**< operation applies to heap */
#define CMEM_POOL                       0x0000  /**< operation applies to a pool */
#define CMEM_CACHED                     0x0800  /**< allocated buffer is cached */
#define CMEM_NONCACHED                  0x0000  /**< allocated buffer is not cached */
#define CMEM_PHYS                       0x1000

/* supported "base" ioctl cmds for the driver. */
#define CMEM_IOCALLOC                   1
#define CMEM_IOCALLOCHEAP               2
#define CMEM_IOCFREE                    3
#define CMEM_IOCGETPHYS                 4
#define CMEM_IOCGETSIZE                 5
#define CMEM_IOCGETPOOL                 6
#define CMEM_IOCCACHE                   7
#define CMEM_IOCGETVERSION              8
#define CMEM_IOCGETBLOCK                9

/* supported "flavors" to "base" ioctl cmds for the driver. */
#define CMEM_IOCCACHEWBINV              CMEM_IOCCACHE | CMEM_WB | CMEM_INV
#define CMEM_IOCCACHEWB                 CMEM_IOCCACHE | CMEM_WB
#define CMEM_IOCCACHEINV                CMEM_IOCCACHE | CMEM_INV
#define CMEM_IOCALLOCCACHED             CMEM_IOCALLOC | CMEM_CACHED
#define CMEM_IOCALLOCHEAPCACHED         CMEM_IOCALLOCHEAP | CMEM_CACHED
#define CMEM_IOCFREEHEAP                CMEM_IOCFREE | CMEM_HEAP
#define CMEM_IOCFREEPHYS                CMEM_IOCFREE | CMEM_PHYS
#define CMEM_IOCFREEHEAPPHYS            CMEM_IOCFREE | CMEM_HEAP | CMEM_PHYS

#define CMEM_IOCCMDMASK                 0x000000ff

/**
 * @brief Parameters for CMEM_alloc(), CMEM_free(), and CMEM_allocPool().
 */
typedef struct CMEM_AllocParams {
    int type;           /**< either CMEM_HEAP or CMEM_POOL */
    int flags;          /**< either CMEM_CACHED or CMEM_NONCACHED */
    size_t alignment;   /**<
                         * only used for heap allocations, must be power of 2
                         */
} CMEM_AllocParams;

extern CMEM_AllocParams CMEM_DEFAULTPARAMS;

/** @cond INTERNAL */

/**
 */
union CMEM_AllocUnion {
    struct {                    /**< */
        size_t size;
        size_t align;
    } alloc_heap_inparams;      /**< */
    struct {                    /**< */
        unsigned long physp;
        size_t size;
    } alloc_pool_outparams;     /**< */
    struct {                    /**< */
        unsigned long physp;
        size_t size;
    } get_block_outparams;      /**< */
    struct {                    /**< */
        int poolid;
        size_t size;
    } free_outparams;           /**< */
    unsigned long physp;
    unsigned long virtp;
    int size;
    int poolid;
};

/** @endcond */

/**
 * @brief Initialize the CMEM module. Must be called before other API calls.
 *
 * @return 0 for success or -1 for failure.
 *
 * @sa CMEM_exit
 */
int CMEM_init(void);

/**
 * @brief Find the pool that best fits a given buffer size and has a buffer
 * available.
 *
 * @param   size    The buffer size for which a pool is needed.
 *
 * @return A poolid that can be passed to CMEM_allocPool(), or -1 for error.
 *
 * @pre Must have called CMEM_init()
 *
 * @sa CMEM_allocPool()
 * @sa CMEM_free()
 */
int CMEM_getPool(size_t size);

/**
 * @brief Allocate memory from a specified pool.
 *
 * @param   poolid  The pool from which to allocate memory.
 * @param   params  Allocation parameters.
 *
 * @remarks @c params->type is ignored - a pool will always be used.
 * @remarks @c params->alignment is unused, since pool buffers are already
 *          aligned to specific boundaries.
 *
 * @return A pointer to the allocated buffer, or NULL for failure.
 *
 * @pre Must have called CMEM_init()
 *
 * @sa CMEM_alloc()
 * @sa CMEM_free()
 */
void *CMEM_allocPool(int poolid, CMEM_AllocParams *params);

/**
 * @brief Allocate memory of a specified size
 *
 * @param   size    The size of the buffer to allocate.
 * @param   params  Allocation parameters.
 *
 * @remarks Used to allocate memory from either a pool or the heap.
 *          If doing a pool allocation, the pool that best fits the requested
 *          size will be selected.  Use CMEM_allocPool() to allocate from a
 *          specific pool.
 *          Allocation will be cached or noncached, as specified by params.
 *          params->alignment valid only for heap allocation.
 *
 * @return A pointer to the allocated buffer, or NULL for failure.
 *
 * @pre Must have called CMEM_init()
 *
 * @sa CMEM_allocPool()
 * @sa CMEM_free()
 */
void *CMEM_alloc(size_t size, CMEM_AllocParams *params);

/**
 * @brief Free a buffer previously allocated with
 *        CMEM_alloc()/ CMEM_allocPool().
 *
 * @param   ptr     The pointer to the buffer.
 * @param   params  Allocation parameters.
 *
 * @remarks Use the same CMEM_AllocParams as was used for the allocation.
 *          params->flags is "don't care".  params->alignment is "don't
 *          care".
 *
 * @return 0 for success or -1 for failure.
 *
 * @pre Must have called CMEM_init()
 *
 * @sa CMEM_alloc()
 * @sa CMEM_allocPool()
 */
int CMEM_free(void *ptr, CMEM_AllocParams *params);

/**
 * @brief Get the physical address of a contiguous buffer.
 *
 * @param   ptr     The pointer to the buffer.
 *
 * @return The physical address of the buffer or 0 for failure.
 *
 * @pre Must have called CMEM_init()
 */
unsigned long CMEM_getPhys(void *ptr);

/**
 * @brief Do a cache writeback of the block pointed to by @c ptr/@c size
 *
 * @param   ptr     Pointer to block to writeback
 * @param   size    Size in bytes of block to writeback.
 *
 * @return Success/failure boolean value
 *
 * @pre Must have called CMEM_init()
 *
 * @sa CMEM_cacheInv()
 * @sa CMEM_cacheWbInv()
 */
int CMEM_cacheWb(void *ptr, size_t size);

/**
 * @brief Do a cache invalidate of the block pointed to by @c ptr/@c size
 *
 * @param   ptr     Pointer to block to invalidate
 * @param   size    Size in bytes of block to invalidate
 *
 * @return Success/failure boolean value
 *
 * @pre Must have called CMEM_init()
 *
 * @sa CMEM_cacheWb()
 * @sa CMEM_cacheWbInv()
 */
int CMEM_cacheInv(void *ptr, size_t size);

/**
 * @brief Do a cache writeback/invalidate of the block pointed to by
 *        @c ptr/@c size
 *
 * @param   ptr     Pointer to block to writeback/invalidate
 * @param   size    Size in bytes of block to writeback/invalidate
 *
 * @return Success/failure boolean value
 *
 * @pre Must have called CMEM_init()
 *
 * @sa CMEM_cacheInv()
 * @sa CMEM_cacheWb()
 */
int CMEM_cacheWbInv(void *ptr, size_t size);

/**
 * @brief Retrieve version from CMEM driver.
 *
 * @return Installed CMEM driver's version number.
 *
 * @pre Must have called CMEM_init()
 */
int CMEM_getVersion(void);

/**
 * @brief Retrieve memory block bounds from CMEM driver
 *
 * @param   pphys_base   Pointer to storage for base physical address of
 *                      CMEM's memory block
 * @param   psize        Pointer to storage for size of CMEM's memory block
 *
 * @return Success (0) or failure (-1).
 *
 * @pre Must have called CMEM_init()
 */
int CMEM_getBlock(unsigned long *pphys_base, size_t *psize);

/**
 * @brief Finalize the CMEM module.
 *
 * @return 0 for success or -1 for failure.
 *
 * @remarks After this function has been called, no other CMEM function may be
 *          called (unless CMEM is reinitialized).
 *
 * @pre Must have called CMEM_init()
 *
 * @sa CMEM_init()
 */
int CMEM_exit(void);

#if defined (__cplusplus)
}
#endif

#endif // _CMEM_H
/*
 *  @(#) ti.sdo.linuxutils.cmem; 2, 0, 1,43; 3-7-2008 14:39:26; /db/atree/library/trees/cmem-b04x/src/
 */

