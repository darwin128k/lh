/**
 * @file cb.h
 * @brief Callable wrapper for allocator reallocation callbacks.
 *
 * Provides a convenient type alias
 * for function pointer to ::lh_memory_allocator_realloc_fn.
 *
 * @see lh_memory_allocator_alloc_cb
 */

#ifndef LH_MEMORY_ALLOCATOR_REALLOC_CB_H
#define LH_MEMORY_ALLOCATOR_REALLOC_CB_H

#include <lh/memory/allocator/realloc/fn.h>
#include <lh/util/ptr.h>

/**
 * @def lh_memory_allocator_realloc_cb
 * @brief Pointer to ::lh_memory_allocator_realloc_fn.
 *
 * Expands to a function pointer type for allocator reallocation callbacks.
 */
#define lh_memory_allocator_realloc_cb lh_ptr_of(lh_memory_allocator_realloc_fn)

#endif /* LH_MEMORY_ALLOCATOR_REALLOC_CB_H */
