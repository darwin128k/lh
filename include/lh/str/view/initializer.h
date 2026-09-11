/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macros for ::lh_str_view_t.
 */

#ifndef LH_STR_VIEW_INITIALIZER_H
#define LH_STR_VIEW_INITIALIZER_H

#include <lh/memory/view/initializer.h>

/**
 * @def lh_str_view_initializer(begin, end)
 * @brief Produces a brace-enclosed initializer for ::lh_str_view_t.
 *
 * Wraps ::lh_memory_view_initializer. Endpoints are interpreted as a
 * half-open interval <tt>[begin, end)</tt> over @c const @c lh_char_t storage.
 *
 * @param begin Value for @c first (inclusive begin pointer).
 * @param end   Value for @c second (exclusive end pointer).
 *
 * Example usage:
 * @code{.c}
 * static lh_str_view_t view = lh_str_view_initializer(s, s + n);
 * @endcode
 *
 * @see lh_str_view_t
 * @see lh_memory_view_initializer
 */
#define lh_str_view_initializer(begin, end) lh_memory_view_initializer(begin, end)

/**
 * @def lh_str_view_empty_initializer()
 * @brief Initializer with both endpoints null (uninitialized pattern).
 *
 * @see lh_str_view_initializer
 * @see lh_memory_view_empty_initializer
 */
#define lh_str_view_empty_initializer() lh_str_view_initializer(lh_null, lh_null)

#endif /* LH_STR_VIEW_INITIALIZER_H */
