/**
 * @file allocator.h
 * @brief Runtime allocator accessor and convenience allocation API.
 *
 * Exposes an accessor for the internal ::lh_memory_allocator_t used by runtime
 * helpers, plus ::lh_runtime_allocator_alloc / ::lh_runtime_allocator_free /
 * ::lh_runtime_allocator_realloc as thin wrappers over ::lh_memory_allocator_*
 * on that instance.
 */

#ifndef LH_RUNTIME_ALLOCATOR_H
#define LH_RUNTIME_ALLOCATOR_H

#include <lh/memory/allocator.h>

LH_COMPILER(EXTERN_C_BEGIN)

/**
 * @brief Return address of the internal runtime allocator instance.
 *
 * The returned object stores current alloc/dealloc callbacks for runtime
 * memory operations. When thread-local mode is enabled, each thread gets
 * its own independent instance.
 *
 * @return Pointer to internal ::lh_memory_allocator_t object (never null).
 */
LH_ATTRIBUTE(SYMBOL)
lh_memory_allocator_t *lh_runtime_allocator(void);

/**
 * @brief Allocate @p size bytes using the runtime allocator.
 *
 * Same semantics as ::lh_memory_allocator_alloc(::lh_runtime_allocator(), @p size).
 *
 * @param size Requested size in bytes.
 * @return Pointer to allocated memory.
 */
LH_ATTRIBUTE(SYMBOL)
lh_ptr lh_runtime_allocator_alloc(lh_usize_t size);

/**
 * @brief Free @p ptr using the runtime allocator.
 *
 * Same semantics as ::lh_memory_allocator_dealloc(::lh_runtime_allocator(), @p ptr).
 *
 * @param ptr Block to release (null is a no-op).
 */
LH_ATTRIBUTE(SYMBOL)
lh_void lh_runtime_allocator_free(lh_ptr ptr);

/**
 * @brief Resize or (re)allocate using the runtime allocator.
 *
 * Same semantics as ::lh_memory_allocator_realloc(::lh_runtime_allocator(), ...).
 *
 * @param old_ptr Previous block (or null).
 * @param old_size Previous size in bytes.
 * @param new_size New size in bytes.
 * @return Resized pointer, @p old_ptr, or null per ::lh_memory_allocator_realloc.
 */
LH_ATTRIBUTE(SYMBOL)
lh_ptr lh_runtime_allocator_realloc(lh_ptr old_ptr, lh_usize_t old_size, lh_usize_t new_size);

LH_COMPILER(EXTERN_C_END)

#endif // LH_RUNTIME_ALLOCATOR_H
