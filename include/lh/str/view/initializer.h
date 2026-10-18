/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macros for ::lh_str_view_t.
 */

#ifndef LH_STR_VIEW_INITIALIZER_H
#define LH_STR_VIEW_INITIALIZER_H

#include <lh/memory/view/initializer.h>
#include <lh/util/str/ptr.h>

/**
 * @def lh_str_view_initializer(begin, size)
 * @brief Brace-enclosed initializer for ::lh_str_view_t as <tt>[begin, begin + size)</tt>.
 *
 * Delegates to ::lh_memory_view_initializer.
 *
 * @param begin First character.
 * @param size  Number of characters (bytes).
 */
#define lh_str_view_initializer(begin, size) lh_memory_view_initializer(begin, size)

/**
 * @def lh_str_view_empty_initializer()
 * @brief Uninitialized / empty view (::lh_null, 0).
 *
 * Delegates to ::lh_memory_view_empty_initializer.
 */
#define lh_str_view_empty_initializer() lh_memory_view_empty_initializer()

/**
 * @def lh_str_view_empty()
 * @brief Typed empty ::lh_str_view_t for function arguments.
 *
 * Same fields as ::lh_str_view_empty_initializer. No scan.
 *
 * Example usage:
 * @code{.c}
 * lh_str_view_t v = lh_str_view_empty();
 * @endcode
 *
 * @see lh_str_view_empty_initializer
 * @see lh_str_view_lit
 */
#define lh_str_view_empty() lh_initializer_of(lh_str_view_t, lh_str_view_empty_initializer)

/**
 * @def lh_str_view_initializer_lit(s)
 * @brief Brace-enclosed view over a string literal @p s, length at compile time.
 *
 * Begin is ::lh_str_ptr_get_begin(@p s), size is ::lh_str_ptr_get_size(@p s)
 * (array capacity minus the stored NUL).
 * @p s must be a string literal or a `char` array object, not a decayed pointer.
 *
 * Example usage:
 * @code{.c}
 * static const lh_str_view_t v = lh_str_view_initializer_lit("msg");
 * @endcode
 *
 * @see lh_str_view_lit
 * @see lh_str_ptr_get_size
 */
#define lh_str_view_initializer_lit(s)                                                             \
    lh_str_view_initializer(lh_str_ptr_get_begin(s), lh_str_ptr_get_size(s))

/**
 * @def lh_str_view_lit(s)
 * @brief Compound literal ::lh_str_view_t over string literal @p s.
 *
 * Same fields as ::lh_str_view_initializer_lit, as a typed value for
 * function arguments. No runtime scan of @p s.
 *
 * Example usage:
 * @code{.c}
 * lh_str_view_t v = lh_str_view_lit("failed");
 * @endcode
 *
 * @see lh_str_view_initializer_lit
 */
#define lh_str_view_lit(s) lh_initializer_of(lh_str_view_t, lh_str_view_initializer_lit, s)

#endif /* LH_STR_VIEW_INITIALIZER_H */
