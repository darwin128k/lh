/**
 * @file allocator.h
 * @brief Runtime allocator accessor and convenience allocation API.
 *
 * Same shape as ::lh_runtime_writer / ::lh_runtime_terminate: one slot the host
 * wires, then thin wrappers for everyday use. With
 * ::LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_USE_STDLIB on it starts as malloc/free;
 * off (bare-metal), empty until the application installs callbacks:
 *
 * @code
 * lh_runtime_allocator_init(pool_alloc, pool_free);
 * lh_ptr p = lh_runtime_allocator_alloc(64);
 * @endcode
 *
 * ::lh_runtime_allocator() remains for hosts that need the underlying
 * ::lh_memory_allocator_t (context, realloc, …).
 */

#ifndef LH_RUNTIME_ALLOCATOR_H
#define LH_RUNTIME_ALLOCATOR_H

#include <lh/memory/allocator.h>

LH_COMPILER_EXTERN_C_BEGIN

/** The default allocation callback: ::LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_DEFAULT_ALLOC. */
lh_ptr
lh_runtime_allocator_default_alloc(lh_self_ptr self, lh_usize_t size);

/** The default release callback: ::LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_DEFAULT_DEALLOC. */
lh_void
lh_runtime_allocator_default_dealloc(lh_self_ptr self, lh_ptr ptr);

/** The default resize callback: ::LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_DEFAULT_REALLOC; @p old_size
 * is unused. */
lh_ptr
lh_runtime_allocator_default_realloc(lh_self_ptr self, lh_ptr ptr, lh_usize_t old_size,
                                     lh_usize_t new_size);

/**
 * @brief Return address of the internal runtime allocator instance.
 *
 * The returned object stores current alloc/dealloc callbacks for runtime
 * memory operations. When thread-local mode is enabled, each thread gets
 * its own independent instance.
 *
 * @return Pointer to internal ::lh_memory_allocator_t object (never null).
 */
lh_memory_allocator_t *
lh_runtime_allocator(void);

/**
 * @brief Install alloc/dealloc on ::lh_runtime_allocator().
 *
 * Same as ::lh_memory_allocator_init(::lh_runtime_allocator(), …). Use this on
 * freestanding builds; with the stdlib option on, the slot is already wired.
 *
 * @param alloc_cb   Allocation callback (may be ::lh_null).
 * @param dealloc_cb Deallocation callback (may be ::lh_null).
 */
lh_void
lh_runtime_allocator_init(lh_memory_allocator_alloc_cb alloc_cb,
                          lh_memory_allocator_dealloc_cb dealloc_cb);

/**
 * @brief Allocate @p size bytes using the runtime allocator.
 *
 * Same semantics as ::lh_memory_allocator_alloc(::lh_runtime_allocator(), @p size).
 *
 * @param size Requested size in bytes.
 * @return Pointer to allocated memory.
 */
lh_ptr
lh_runtime_allocator_alloc(lh_usize_t size);

/**
 * @brief Free @p ptr using the runtime allocator.
 *
 * Same semantics as ::lh_memory_allocator_dealloc(::lh_runtime_allocator(), @p ptr).
 *
 * @param ptr Block to release (null is a no-op).
 */
lh_void
lh_runtime_allocator_free(lh_ptr ptr);

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
lh_ptr
lh_runtime_allocator_realloc(lh_ptr old_ptr, lh_usize_t old_size, lh_usize_t new_size);

LH_COMPILER_EXTERN_C_END

#endif /* LH_RUNTIME_ALLOCATOR_H */
