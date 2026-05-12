/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macros for ::lh_memory_bounds_t.
 */

#ifndef LH_MEMORY_BOUNDS_INITIALIZER_H
#define LH_MEMORY_BOUNDS_INITIALIZER_H

#include <lh/memory/bounds/slice/initializer.h>

/**
 * @def lh_memory_bounds_initializer(begin, end)
 * @brief Produces a brace-enclosed initializer for ::lh_memory_bounds_t.
 *
 * Expands to ::lh_memory_bounds_slice_initializer with @p begin as @c first
 * and @p end as @c second. For ::lh_memory_bounds_t, the endpoints are
 * interpreted as a half-open interval <tt>[begin, end)</tt>; @p end is one
 * past the last byte.
 *
 * @param begin Value for @c first (inclusive begin pointer).
 * @param end   Value for @c second (exclusive end pointer).
 *
 * Example usage:
 * @code{.c}
 * static lh_memory_bounds_t bounds =
 *     lh_memory_bounds_initializer(ptr, ptr + n);
 * @endcode
 *
 * @see lh_memory_bounds_t
 * @see lh_memory_bounds_slice_initializer
 */
#define lh_memory_bounds_initializer(begin, end) lh_memory_bounds_slice_initializer(begin, end)

/**
 * @def lh_memory_bounds_empty_initializer()
 * @brief Initializer with both endpoints null (uninitialized pattern).
 *
 * Expands to ::lh_memory_bounds_initializer(::lh_null, ::lh_null).
 *
 * @see lh_memory_bounds_initializer
 * @see lh_memory_bounds_is_uninitialized
 */
#define lh_memory_bounds_empty_initializer() lh_memory_bounds_initializer(lh_null, lh_null)

#endif // LH_MEMORY_BOUNDS_INITIALIZER_H
