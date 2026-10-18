/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macros for ::lh_memory_view_t.
 */

#ifndef LH_MEMORY_VIEW_INITIALIZER_H
#define LH_MEMORY_VIEW_INITIALIZER_H

#include <lh/null.h>
#include <lh/initializer.h>

/**
 * @def lh_memory_view_initializer(begin, size)
 * @brief Produces a brace-enclosed initializer for ::lh_memory_view_t.
 *
 * Expands to ::lh_initializer with @p begin as @c begin and @p size as
 * @c size: the half-open view <tt>[begin, begin + size)</tt>.
 *
 * @param begin First byte.
 * @param size  Number of bytes.
 *
 * Example usage:
 * @code{.c}
 * static lh_memory_view_t view =
 *     lh_memory_view_initializer(ptr, n);
 * @endcode
 *
 * @see lh_memory_view_t
 * @see lh_initializer
 */
#define lh_memory_view_initializer(begin, size) lh_initializer(begin, size)

/**
 * @def lh_memory_view_empty_initializer()
 * @brief Initializer with a null @c begin and a zero @c size (uninitialized pattern).
 *
 * Expands to ::lh_memory_view_initializer(::lh_null, 0).
 *
 * @see lh_memory_view_initializer
 * @see lh_memory_view_is_uninitialized
 */
#define lh_memory_view_empty_initializer() lh_memory_view_initializer(lh_null, 0U)

#endif /* LH_MEMORY_VIEW_INITIALIZER_H */
