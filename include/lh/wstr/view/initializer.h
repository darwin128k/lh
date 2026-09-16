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
 *
 * Delegates to ::lh_memory_view_initializer.
 *
 * @param begin Inclusive begin pointer (::lh_wstr_cptr).
 * @param end   Exclusive end pointer (one past the last payload ::lh_wchar_t).
 *
 * @see lh_memory_view_initializer
 */
#define lh_wstr_view_initializer(begin, end) lh_memory_view_initializer(begin, end)

/**
 * @def lh_wstr_view_empty_initializer()
 * @brief Uninitialized / empty view (::lh_null, ::lh_null).
 *
 * Delegates to ::lh_memory_view_empty_initializer.
 *
 * @see lh_memory_view_empty_initializer
 */
#define lh_wstr_view_empty_initializer() lh_memory_view_empty_initializer()

/**
 * @def lh_wstr_view_initializer_lit(s)
 * @brief Brace-enclosed view over a wide string literal @p s, length at compile time.
 *
 * Endpoints are ::lh_wstr_ptr_get_begin(@p s) and ::lh_wstr_ptr_get_end(@p s).
 * Payload length is ::lh_wstr_ptr_get_size (array capacity minus the stored @c L'\\0').
 * @p s must be a wide string literal or a `wchar_t` array object, not a decayed pointer.
 *
 * Example usage:
 * @code{.c}
 * static const lh_wstr_view_t v = lh_wstr_view_initializer_lit(L"msg");
 * @endcode
 *
 * @see lh_wstr_view_lit
 * @see lh_wstr_ptr_get_size
 */
#define lh_wstr_view_initializer_lit(s)                                                            \
    lh_wstr_view_initializer(lh_wstr_ptr_get_begin(s), lh_wstr_ptr_get_end(s))

/**
 * @def lh_wstr_view_lit(s)
 * @brief Compound literal ::lh_wstr_view_t over wide string literal @p s.
 *
 * Same endpoints as ::lh_wstr_view_initializer_lit, as a typed value for
 * function arguments. No runtime scan of @p s.
 *
 * Example usage:
 * @code{.c}
 * lh_werror_set(&err, 1, lh_wstr_view_lit(L"failed"));
 * @endcode
 *
 * @see lh_wstr_view_initializer_lit
 * @see lh_wstr_view_make
 */
#define lh_wstr_view_lit(s) lh_initializer_of(lh_wstr_view_t, lh_wstr_view_initializer_lit, s)

#endif /* LH_WSTR_VIEW_INITIALIZER_H */
