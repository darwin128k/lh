/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macros for ::lh_memory_allocator_t.
 */

#ifndef LH_MEMORY_ALLOCATOR_INITIALIZER_H
#define LH_MEMORY_ALLOCATOR_INITIALIZER_H

#include <lh/initializer.h>
#include <lh/memory/allocator/alloc/cb.h>
#include <lh/memory/allocator/dealloc/cb.h>
#include <lh/null.h>

/**
 * @def lh_memory_allocator_initializer(malloc_fn, dealloc_fn)
 * @brief Produces a brace-enclosed initializer for ::lh_memory_allocator_t.
 *
 * Expands to ::lh_initializer with each argument passed through ::lh_ptr_rcast
 * to `lh_memory_allocator_alloc_fn *` and `lh_memory_allocator_dealloc_fn *`
 * — the same types as @c alloc_cb and @c dealloc_cb (see ::lh_memory_allocator_fields).
 *
 * ::lh_ptr_rcast uses ::lh_cast_reinterpret,
 * so in **C++** this becomes `reinterpret_cast` to a function pointer.
 *
 * That is why object pointers such as ::lh_null (`void *` in this library)
 * are accepted where a plain implicit conversion to a function pointer would be ill-formed;
 * the same macro also accepts function designators and existing callback pointers.
 * **C** uses a C-style cast to the function-pointer type.
 *
 * @param malloc_fn  Value for @c alloc_cb (e.g. a function name, compatible pointer, or ::lh_null).
 * @param dealloc_fn Value for @c dealloc_cb (same).
 *
 * Example usage:
 * @code{.c}
 * static lh_memory_allocator_t alloc = lh_memory_allocator_initializer(malloc, free);
 * @endcode
 *
 * @see lh_initializer
 * @see lh_ptr_rcast
 * @see lh_cast_reinterpret
 * @see lh_memory_allocator_t
 * @see lh_memory_allocator_init
 */
#define lh_memory_allocator_initializer(malloc_fn, dealloc_fn)                                     \
    lh_initializer(lh_ptr_rcast(lh_memory_allocator_alloc_fn, malloc_fn),                          \
                   lh_ptr_rcast(lh_memory_allocator_dealloc_fn, dealloc_fn))

/**
 * @def lh_memory_allocator_empty_initializer()
 * @brief Initializer with both callbacks null (allocator not configured).
 *
 * Expands to ::lh_memory_allocator_initializer(::lh_null, ::lh_null).
 * Each ::lh_null is reinterpreted to a null function pointer via ::lh_ptr_rcast inside
 * ::lh_memory_allocator_initializer, matching the cleared state from ::lh_memory_allocator_deinit.
 *
 * Example usage:
 * @code{.c}
 * static lh_memory_allocator_t empty = lh_memory_allocator_empty_initializer();
 * @endcode
 *
 * @see lh_memory_allocator_initializer
 * @see lh_memory_allocator_deinit
 */
#define lh_memory_allocator_empty_initializer() lh_memory_allocator_initializer(lh_null, lh_null)

#endif /* LH_MEMORY_ALLOCATOR_INITIALIZER_H */
