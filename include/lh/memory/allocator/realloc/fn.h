/**
 * @file fn.h
 * @brief Callable signature for allocator reallocation routines.
 *
 * Same contract as C `realloc`: grow or shrink the block at @p ptr to
 * @p size bytes, possibly in place, keeping its leading contents; return
 * the (possibly moved) block, or null on failure with @p ptr untouched.
 * Must belong to the same heap as the allocator's alloc/dealloc pair.
 *
 * @see lh_memory_allocator_alloc_fn
 */

#ifndef LH_MEMORY_ALLOCATOR_REALLOC_FN_H
#define LH_MEMORY_ALLOCATOR_REALLOC_FN_H

#include <lh/ptr.h>
#include <lh/size.h>

/**
 * @typedef lh_memory_allocator_realloc_fn
 * @brief Function type `lh_ptr(lh_ptr, lh_usize_t)` for allocator
 *        reallocation.
 *
 * Not a pointer type by itself; use pointer wrappers where needed.
 */
typedef lh_ptr(lh_memory_allocator_realloc_fn)(lh_ptr ptr, lh_usize_t size);

#endif /* LH_MEMORY_ALLOCATOR_REALLOC_FN_H */
