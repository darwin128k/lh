/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macros for ::lh_memory_view_t.
 */

#ifndef LH_MEMORY_VIEW_INITIALIZER_H
#define LH_MEMORY_VIEW_INITIALIZER_H

#include <lh/memory/bounds/initializer.h>

/**
 * @def lh_memory_view_initializer(begin, end)
 * @brief Produces a brace-enclosed initializer for ::lh_memory_view_t.
 *
 * Expands to ::lh_memory_bounds_initializer with half-open bounds semantics
 * <tt>[begin, end)</tt>.
 *
 * @param begin Value for @c first (inclusive begin pointer).
 * @param end   Value for @c second (exclusive end pointer).
 *
 * @see lh_memory_view_t
 * @see lh_memory_bounds_initializer
 */
#define lh_memory_view_initializer(begin, end) lh_memory_bounds_initializer(begin, end)

/**
 * @def lh_memory_view_empty_initializer()
 * @brief Initializer with both endpoints null (uninitialized pattern).
 *
 * Expands to ::lh_memory_bounds_empty_initializer.
 *
 * @see lh_memory_view_initializer
 * @see lh_memory_bounds_empty_initializer
 */
#define lh_memory_view_empty_initializer() lh_memory_bounds_empty_initializer()

#endif // LH_MEMORY_VIEW_INITIALIZER_H
