/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macros for ::lh_wstr_view_t.
 */

#ifndef LH_WSTR_VIEW_INITIALIZER_H
#define LH_WSTR_VIEW_INITIALIZER_H

#include <lh/initializer.h>
#include <lh/memory/view/initializer.h>
#include <lh/util/wstr/ptr.h>

/**
 * @def lh_wstr_view_initializer(begin, end)
 * @brief Brace-enclosed initializer for ::lh_wstr_view_t as <tt>[begin, end)</tt>.
 */
#define lh_wstr_view_initializer(begin, end) lh_memory_view_initializer(begin, end)

/**
 * @def lh_wstr_view_empty_initializer()
 * @brief Uninitialized / empty view (::lh_null, ::lh_null).
 */
#define lh_wstr_view_empty_initializer() lh_memory_view_empty_initializer()

/**
 * @def lh_wstr_view_initializer_lit(s)
 * @brief Brace-enclosed view over a wide string literal @p s, length at compile time.
 *
 * @p s must be a wide string literal or a `wchar_t` array object, not a decayed pointer.
 *
 * Example usage:
 * @code{.c}
 * static const lh_wstr_view_t v = lh_wstr_view_initializer_lit(L"msg");
 * @endcode
 */
#define lh_wstr_view_initializer_lit(s)                                                            \
    lh_wstr_view_initializer(lh_wstr_ptr_get_begin(s), lh_wstr_ptr_get_end(s))

/**
 * @def lh_wstr_view_lit(s)
 * @brief Compound literal ::lh_wstr_view_t over wide string literal @p s.
 */
#define lh_wstr_view_lit(s) lh_initializer_of(lh_wstr_view_t, lh_wstr_view_initializer_lit, s)

#endif /* LH_WSTR_VIEW_INITIALIZER_H */
