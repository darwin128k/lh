/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macros for ::lh_memory_view_t.
 */

#ifndef LH_MEMORY_VIEW_INITIALIZER_H
#define LH_MEMORY_VIEW_INITIALIZER_H

#include <lh/memory/view/slice/initializer.h>

/**
 * @def lh_memory_view_initializer(begin, end)
 * @brief Produces a brace-enclosed initializer for ::lh_memory_view_t.
 *
 * Expands to ::lh_memory_view_slice_initializer with @p begin as @c first
 * and @p end as @c second. For ::lh_memory_view_t, the endpoints are
 * interpreted as a half-open interval <tt>[begin, end)</tt>; @p end is one
 * past the last byte.
 *
 * @param begin Value for @c first (inclusive begin pointer).
 * @param end   Value for @c second (exclusive end pointer).
 *
 * Example usage:
 * @code{.c}
 * static lh_memory_view_t view =
 *     lh_memory_view_initializer(ptr, ptr + n);
 * @endcode
 *
 * @see lh_memory_view_t
 * @see lh_memory_view_slice_initializer
 */
#define lh_memory_view_initializer(begin, end) lh_memory_view_slice_initializer(begin, end)

/**
 * @def lh_memory_view_empty_initializer()
 * @brief Initializer with both endpoints null (uninitialized pattern).
 *
 * Expands to ::lh_memory_view_initializer(::lh_null, ::lh_null).
 *
 * @see lh_memory_view_initializer
 * @see lh_memory_view_is_uninitialized
 */
#define lh_memory_view_empty_initializer() lh_memory_view_initializer(lh_null, lh_null)

#endif /* LH_MEMORY_VIEW_INITIALIZER_H */
