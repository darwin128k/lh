/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macros for ::lh_str_view_t.
 */

#ifndef LH_STR_VIEW_INITIALIZER_H
#define LH_STR_VIEW_INITIALIZER_H

#include <lh/memory/view/initializer.h>

/**
 * @def lh_str_view_initializer(begin, end)
 * @brief Brace-enclosed initializer for ::lh_str_view_t as <tt>[begin, end)</tt>.
 *
 * Delegates to ::lh_memory_view_initializer.
 */
#define lh_str_view_initializer(begin, end) lh_memory_view_initializer(begin, end)

/**
 * @def lh_str_view_empty_initializer()
 * @brief Uninitialized / empty view (::lh_null, ::lh_null).
 *
 * Delegates to ::lh_memory_view_empty_initializer.
 */
#define lh_str_view_empty_initializer() lh_memory_view_empty_initializer()

#endif /* LH_STR_VIEW_INITIALIZER_H */
