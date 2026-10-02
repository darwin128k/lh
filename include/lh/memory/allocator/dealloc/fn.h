/**
 * @file fn.h
 * @brief Callable signature for allocator deallocation routines.
 *
 * Defines the function type used by allocators to release memory previously
 * returned to the caller. The function accepts the allocator's state @p self (see
 * ::lh_memory_allocator_alloc_fn) and the pointer to release, and returns
 * nothing.
 *
 * @see lh_memory_allocator_alloc_fn
 */

#ifndef LH_MEMORY_ALLOCATOR_DEALLOC_FN_H
#define LH_MEMORY_ALLOCATOR_DEALLOC_FN_H

#include <lh/ptr.h>
#include <lh/self.h>

/**
 * @typedef lh_memory_allocator_dealloc_fn
 * @brief Function type `lh_void (lh_self_ptr, lh_ptr)` for allocator deallocation.
 *
 * Not a pointer type by itself; use pointer wrappers where needed.
 */
typedef lh_void(lh_memory_allocator_dealloc_fn)(lh_self_ptr self, lh_ptr ptr);

#endif /* LH_MEMORY_ALLOCATOR_DEALLOC_FN_H */