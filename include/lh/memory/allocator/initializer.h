/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macros for ::lh_memory_allocator_t.
 */

#ifndef LH_MEMORY_ALLOCATOR_INITIALIZER_H
#define LH_MEMORY_ALLOCATOR_INITIALIZER_H

#include <lh/cast/static.h>
#include <lh/initializer.h>
#include <lh/memory/allocator/alloc/cb.h>
#include <lh/memory/allocator/dealloc/cb.h>
#include <lh/memory/allocator/realloc/cb.h>
#include <lh/null.h>
#include <lh/util/ptr.h>

/**
 * @def lh_memory_allocator_initializer(malloc_fn, dealloc_fn)
 * @brief Produces a brace-enclosed initializer for ::lh_memory_allocator_t
 *        with no native realloc (`realloc_cb` null) and no context.
 *
 * Expands to ::lh_initializer with each callback argument passed through
 * ::lh_cast_static to `lh_memory_allocator_alloc_fn *` and
 * `lh_memory_allocator_dealloc_fn *` — the same types as @c alloc_cb and
 * @c dealloc_cb (see ::lh_memory_allocator_fields).
 *
 * The cast is **static** on purpose. A callback here is a function, the address
 * of one, or ::lh_null, and all three become a function pointer by a standard
 * conversion. A *reinterpretation* would only ever be needed to turn an object
 * pointer into a function pointer — a mistake rather than a feature, and with
 * `nullptr` (::lh_null in C++) not even expressible. **C** gets the C-style
 * cast, which is what ::lh_cast_static is everywhere in the library.
 *
 * Pass only functions of type ::lh_memory_allocator_alloc_fn /
 * ::lh_memory_allocator_dealloc_fn (they take the context first) — not
 * `malloc` / `free` themselves. In **C++** a function of the wrong signature
 * is now a compile error instead of a pointer to the wrong shape.
 *
 * @param malloc_fn  Value for @c alloc_cb (e.g. a function name, compatible pointer, or ::lh_null).
 * @param dealloc_fn Value for @c dealloc_cb (same).
 *
 * Example usage:
 * @code{.c}
 * static lh_ptr my_alloc(lh_ptr context, lh_usize_t size) { (void)context; return malloc(size); }
 * static lh_void my_free(lh_ptr context, lh_ptr ptr) { (void)context; free(ptr); }
 * static lh_memory_allocator_t alloc = lh_memory_allocator_initializer(my_alloc, my_free);
 * @endcode
 *
 * @see lh_initializer
 * @see lh_cast_static
 * @see lh_ptr_of
 * @see lh_memory_allocator_t
 * @see lh_memory_allocator_init
 */
#define lh_memory_allocator_initializer(malloc_fn, dealloc_fn)                                     \
    lh_memory_allocator_initializer_with_realloc(malloc_fn, dealloc_fn, lh_null)

/**
 * @def lh_memory_allocator_initializer_with_realloc(malloc_fn, dealloc_fn, realloc_fn)
 * @brief ::lh_memory_allocator_initializer plus a native reallocation callback.
 *
 * @p realloc_fn must belong to the same heap as @p malloc_fn / @p dealloc_fn
 * (wrappers of `malloc`, `free`, `realloc`, say), or be ::lh_null. The
 * context is ::lh_null; see ::lh_memory_allocator_initializer_with_context.
 */
#define lh_memory_allocator_initializer_with_realloc(malloc_fn, dealloc_fn, realloc_fn)            \
    lh_memory_allocator_initializer_with_context(malloc_fn, dealloc_fn, realloc_fn, lh_null)

/**
 * @def lh_memory_allocator_initializer_with_context(malloc_fn, dealloc_fn, realloc_fn, context)
 * @brief ::lh_memory_allocator_initializer_with_realloc plus the context every
 *        callback receives (a static pool's state, for instance).
 */
#define lh_memory_allocator_initializer_with_context(malloc_fn, dealloc_fn, realloc_fn, context)   \
    lh_initializer(lh_cast_static(lh_ptr_of(lh_memory_allocator_alloc_fn), malloc_fn),             \
                   lh_cast_static(lh_ptr_of(lh_memory_allocator_dealloc_fn), dealloc_fn),          \
                   lh_cast_static(lh_ptr_of(lh_memory_allocator_realloc_fn), realloc_fn), (context))

/**
 * @def lh_memory_allocator_empty_initializer()
 * @brief Initializer with both callbacks null (allocator not configured).
 *
 * Expands to ::lh_memory_allocator_initializer(::lh_null, ::lh_null).
 * Each ::lh_null becomes a null function pointer via ::lh_cast_static inside
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