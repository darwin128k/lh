/**
 * @file fn.h
 * @brief Callable signature for allocator allocation routines.
 *
 * Defines the function type used by allocators to obtain memory.
 * The function accepts the allocator's context and a size request and returns
 * a pointer to allocated memory, or null on failure.
 *
 * @p context is the allocator's own state (::lh_memory_allocator_get_context):
 * a pool or arena over a static buffer, a per-module sandbox, ... A stateless
 * allocator such as a `malloc` wrapper ignores it.
 *
 * @see lh_memory_allocator_dealloc_fn
 */

#ifndef LH_MEMORY_ALLOCATOR_ALLOC_FN_H
#define LH_MEMORY_ALLOCATOR_ALLOC_FN_H

#include <lh/ptr.h>
#include <lh/size.h>

/**
 * @typedef lh_memory_allocator_alloc_fn
 * @brief Function type `lh_ptr(lh_ptr, lh_usize_t)` for allocator allocation.
 *
 * Not a pointer type by itself; use pointer wrappers where needed.
 */
typedef lh_ptr(lh_memory_allocator_alloc_fn)(lh_ptr context, lh_usize_t size);

#endif /* LH_MEMORY_ALLOCATOR_ALLOC_FN_H */