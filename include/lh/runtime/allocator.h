/**
 * @file allocator.h
 * @brief Runtime allocator accessor and convenience allocation API.
 *
 * Same shape as ::lh_runtime_writer_get_mut / ::lh_runtime_terminate: a slot the host
 * wires, then thin wrappers for everyday use. It starts empty: the core has no
 * C library, so nothing allocates until the host installs an allocator (a pool,
 * an RTOS heap, `malloc` wrapped by a desktop host; see the README).
 *
 * The slot is one object for the whole program, empty until the host fills it
 * through ::lh_runtime_allocator_get_mut — before the first use and before it starts
 * other threads (a thread sees everything written before it was created).
 * Every thread, the ones LH starts and the host's own, uses the same object.
 *
 * @code
 * lh_memory_allocator_init(lh_runtime_allocator_get_mut(), pool_alloc, pool_free); // at start-up
 * lh_ptr p = lh_runtime_allocator_alloc(64);                                       // any thread
 * @endcode
 *
 * The callbacks are shared by every thread, so they must be thread-safe when
 * more than one thread allocates.
 */

#ifndef LH_RUNTIME_ALLOCATOR_H
#define LH_RUNTIME_ALLOCATOR_H

#include <lh/memory/allocator.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Return the runtime allocator, for change.
 *
 * One object for every thread. The host fills it before the first use and
 * before it starts other threads; a change while other threads use it is a
 * data race.
 *
 * @return The runtime ::lh_memory_allocator_t (never null).
 */
lh_memory_allocator_t *
lh_runtime_allocator_get_mut(void);

/**
 * @brief Return the runtime allocator.
 *
 * Same object as ::lh_runtime_allocator_get_mut, read-only.
 *
 * @return The runtime ::lh_memory_allocator_t (never null).
 */
const lh_memory_allocator_t *
lh_runtime_allocator_get(void);

/**
 * @brief Allocate @p size bytes using the runtime allocator.
 *
 * Same semantics as ::lh_memory_allocator_alloc(::lh_runtime_allocator_get_mut(), @p size).
 *
 * @param size Requested size in bytes.
 * @return Pointer to allocated memory.
 */
lh_ptr
lh_runtime_allocator_alloc(lh_usize_t size);

/**
 * @brief Free @p ptr using the runtime allocator.
 *
 * Same semantics as ::lh_memory_allocator_dealloc(::lh_runtime_allocator_get_mut(), @p ptr).
 *
 * @param ptr Block to release (null is a no-op).
 */
lh_void
lh_runtime_allocator_free(lh_ptr ptr);

/**
 * @brief Resize or (re)allocate using the runtime allocator.
 *
 * Same semantics as ::lh_memory_allocator_realloc(::lh_runtime_allocator_get_mut(), ...).
 *
 * @param old_ptr Previous block (or null).
 * @param old_size Previous size in bytes.
 * @param new_size New size in bytes.
 * @return Resized pointer, @p old_ptr, or null per ::lh_memory_allocator_realloc.
 */
lh_ptr
lh_runtime_allocator_realloc(lh_ptr old_ptr, lh_usize_t old_size, lh_usize_t new_size);

/** Same semantics as ::lh_memory_allocator_alloc_zero(::lh_runtime_allocator_get_mut(), ...). */
lh_ptr
lh_runtime_allocator_alloc_zero(lh_usize_t count, lh_usize_t item_size);

/** Same semantics as ::lh_memory_allocator_try_alloc(::lh_runtime_allocator_get_mut(), @p size). */
lh_ptr
lh_runtime_allocator_try_alloc(lh_usize_t size);

/** Same semantics as ::lh_memory_allocator_try_alloc_zero(::lh_runtime_allocator_get_mut(), ...).
 */
lh_ptr
lh_runtime_allocator_try_alloc_zero(lh_usize_t count, lh_usize_t item_size);

/** Same semantics as ::lh_memory_allocator_try_dealloc(::lh_runtime_allocator_get_mut(), @p ptr).
 */
lh_bool_t
lh_runtime_allocator_try_free(lh_ptr ptr);

/** Same semantics as ::lh_memory_allocator_try_realloc(::lh_runtime_allocator_get_mut(), ...). */
lh_ptr
lh_runtime_allocator_try_realloc(lh_ptr old_ptr, lh_usize_t old_size, lh_usize_t new_size);

LH_COMPILER_EXTERN_C_END

#endif /* LH_RUNTIME_ALLOCATOR_H */
