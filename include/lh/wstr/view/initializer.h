/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macros for ::lh_wstr_view_t.
 */

#ifndef LH_WSTR_VIEW_INITIALIZER_H
#define LH_WSTR_VIEW_INITIALIZER_H

#include <lh/memory/view/initializer.h>
#include <lh/util/wstr/ptr.h>

/**
 * @def lh_wstr_view_initializer(begin, size)
 * @brief Brace-enclosed initializer for ::lh_wstr_view_t as <tt>[begin, begin + size)</tt>.
 *
 * Delegates to ::lh_memory_view_initializer.
 *
 * @param begin First wide character (const lh_wstr_ptr).
 * @param size  Length in bytes, not in ::lh_wchar_t: the count times ::LH_WCHAR_T_SIZE.
 *
 * @see lh_memory_view_initializer
 */
#define lh_wstr_view_initializer(begin, size) lh_memory_view_initializer(begin, size)

/**
 * @def lh_wstr_view_empty_initializer()
 * @brief Uninitialized / empty view (::lh_null, 0).
 *
 * Delegates to ::lh_memory_view_empty_initializer.
 *
 * @see lh_memory_view_empty_initializer
 */
#define lh_wstr_view_empty_initializer() lh_memory_view_empty_initializer()

/**
 * @def lh_wstr_view_empty()
 * @brief Typed empty ::lh_wstr_view_t for function arguments.
 *
 * Same fields as ::lh_wstr_view_empty_initializer. No scan.
 *
 * Example usage:
 * @code{.c}
 * lh_wstr_view_t v = lh_wstr_view_empty();
 * @endcode
 *
 * @see lh_wstr_view_empty_initializer
 * @see lh_wstr_view_lit
 */
#define lh_wstr_view_empty() lh_initializer_of(lh_wstr_view_t, lh_wstr_view_empty_initializer)

/**
 * @def lh_wstr_view_initializer_lit(s)
 * @brief Brace-enclosed view over a wide string literal @p s, length at compile time.
 *
 * Begin is ::lh_wstr_ptr_get_begin(@p s), size is ::lh_wstr_ptr_get_size(@p s)
 * (array capacity minus the stored @c L'\\0') times ::LH_WCHAR_T_SIZE.
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
    lh_wstr_view_initializer(lh_wstr_ptr_get_begin(s), lh_wstr_ptr_get_size(s) * LH_WCHAR_T_SIZE)

/**
 * @def lh_wstr_view_lit(s)
 * @brief Compound literal ::lh_wstr_view_t over wide string literal @p s.
 *
 * Same fields as ::lh_wstr_view_initializer_lit, as a typed value for
 * function arguments. No runtime scan of @p s.
 *
 * Example usage:
 * @code{.c}
 * lh_wstr_view_t v = lh_wstr_view_lit(L"failed");
 * @endcode
 *
 * @see lh_wstr_view_initializer_lit
 */
#define lh_wstr_view_lit(s) lh_initializer_of(lh_wstr_view_t, lh_wstr_view_initializer_lit, s)

#endif /* LH_WSTR_VIEW_INITIALIZER_H */
