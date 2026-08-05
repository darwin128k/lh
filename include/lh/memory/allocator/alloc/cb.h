/**
 * @file cb.h
 * @brief Callable wrapper for allocator allocation callbacks.
 *
 * Provides a convenient type alias
 * for function pointer to ::lh_memory_allocator_alloc_fn.
 *
 * @see lh_memory_allocator_dealloc_cb
 */

#ifndef LH_MEMORY_ALLOCATOR_ALLOC_CB_H
#define LH_MEMORY_ALLOCATOR_ALLOC_CB_H

#include <lh/memory/allocator/alloc/fn.h>
#include <lh/util/ptr.h>

/**
 * @def lh_memory_allocator_alloc_cb
 * @brief Pointer to ::lh_memory_allocator_alloc_fn.
 *
 * Expands to a function pointer type for allocator allocation callbacks.
 */
#define lh_memory_allocator_alloc_cb lh_ptr_of(lh_memory_allocator_alloc_fn)

#endif /* LH_MEMORY_ALLOCATOR_ALLOC_CB_H */