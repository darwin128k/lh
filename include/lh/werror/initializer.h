/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macros for ::lh_werror_t.
 */

#ifndef LH_WERROR_INITIALIZER_H
#define LH_WERROR_INITIALIZER_H

#include <lh/error/code.h>
#include <lh/initializer.h>

/**
 * @def lh_werror_initializer(code, ...)
 * @brief Produces a brace-enclosed initializer for ::lh_werror_t.
 *
 * Expands to ::lh_initializer(@p code, __VA_ARGS__).
 * The first field is @c code; remaining tokens initialize
 * @c desc and any trailing members in struct order,
 * or you may use designated initializers in the variadic tail.
 *
 * @param code Error code (::lh_error_code_t).
 * @param ...  Optional remainder of the initializer list
 *             (e.g. a ::lh_wstr_view_t initializer or @c .desc = ...).
 *
 * Example usage:
 * @code{.c}
 * lh_werror_t err = lh_werror_initializer(lh_error_code_ok, lh_wstr_view_empty_initializer());
 * lh_werror_t e2 = lh_werror_initializer(42, lh_wstr_view_lit(L"message"));
 * @endcode
 *
 * @see lh_initializer
 * @see lh_werror_t
 */
#define lh_werror_initializer(code, ...) lh_initializer(code, __VA_ARGS__)

/**
 * @def lh_werror_empty_initializer(...)
 * @brief Initializer for a cleared / success wide error (::lh_error_code_ok).
 *
 * Expands to ::lh_werror_initializer(::lh_error_code_ok, __VA_ARGS__).
 * With no arguments, @c desc is value-initialized (empty view).
 * Extra tokens are forwarded as the tail of the initializer list.
 *
 * @param ... Optional tail passed to ::lh_werror_initializer after the code.
 *
 * Example usage:
 * @code{.c}
 * lh_werror_t ok = lh_werror_empty_initializer();
 * @endcode
 *
 * @see lh_werror_initializer
 * @see lh_error_code_ok
 */
#define lh_werror_empty_initializer(...) lh_werror_initializer(lh_error_code_ok, __VA_ARGS__)

#endif /* LH_WERROR_INITIALIZER_H */
