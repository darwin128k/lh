/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macros for ::lh_memory_bounds_slice_t.
 */

#ifndef LH_MEMORY_BOUNDS_SLICE_INITIALIZER_H
#define LH_MEMORY_BOUNDS_SLICE_INITIALIZER_H

#include <lh/interval/bounds/initializer.h>
#include <lh/null.h>

/**
 * @def lh_memory_bounds_slice_initializer(begin, end)
 * @brief Produces a brace-enclosed initializer for ::lh_memory_bounds_slice_t.
 *
 * Expands to ::lh_interval_bounds_initializer with @p begin as @c first and
 * @p end as @c second (closed <tt>[begin, end]</tt> slice endpoints).
 *
 * @param begin Value for @c first (inclusive begin pointer).
 * @param end   Value for @c second (inclusive end pointer).
 *
 * Example usage:
 * @code{.c}
 * static lh_memory_bounds_slice_t slice =
 *     lh_memory_bounds_slice_initializer(ptr, ptr + n - 1);
 * @endcode
 *
 * @see lh_interval_bounds_initializer
 * @see lh_memory_bounds_slice_t
 */
#define lh_memory_bounds_slice_initializer(begin, end) lh_interval_bounds_initializer(begin, end)

/**
 * @def lh_memory_bounds_slice_initializer_empty()
 * @brief Initializer with both endpoints null (uninitialized pattern).
 *
 * Expands to
 * ::lh_memory_bounds_slice_initializer(::lh_null, ::lh_null).
 *
 * @see lh_memory_bounds_slice_initializer
 * @see lh_memory_bounds_slice_is_uninitialized
 */
#define lh_memory_bounds_slice_initializer_empty()                                                 \
    lh_memory_bounds_slice_initializer(lh_null, lh_null)

#endif /* LH_MEMORY_BOUNDS_SLICE_INITIALIZER_H */
