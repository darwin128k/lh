/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macros for ::lh_memory_bounds_t.
 */

#ifndef LH_MEMORY_BOUNDS_INITIALIZER_H
#define LH_MEMORY_BOUNDS_INITIALIZER_H

#include <lh/null.h>

/**
 * @def lh_memory_bounds_initializer(begin, size)
 * @brief Produces a brace-enclosed initializer for ::lh_memory_bounds_t.
 *
 * Expands to ::lh_initializer with @p begin as @c begin and @p size as
 * @c size: the half-open bounds <tt>[begin, begin + size)</tt>.
 *
 * @param begin First byte.
 * @param size  Number of bytes.
 *
 * Example usage:
 * @code{.c}
 * static lh_memory_bounds_t bounds =
 *     lh_memory_bounds_initializer(ptr, n);
 * @endcode
 *
 * @see lh_memory_bounds_t
 * @see lh_initializer
 */
#define lh_memory_bounds_initializer(begin, size) lh_initializer(begin, size)

/**
 * @def lh_memory_bounds_empty_initializer()
 * @brief Initializer with a null @c begin and a zero @c size (uninitialized pattern).
 *
 * Expands to ::lh_memory_bounds_initializer(::lh_null, 0).
 *
 * @see lh_memory_bounds_initializer
 * @see lh_memory_bounds_is_uninitialized
 */
#define lh_memory_bounds_empty_initializer() lh_memory_bounds_initializer(lh_null, 0U)

#endif /* LH_MEMORY_BOUNDS_INITIALIZER_H */
