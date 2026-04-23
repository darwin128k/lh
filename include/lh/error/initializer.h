/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macros for ::lh_error_t.
 */

#ifndef LH_ERROR_INITIALIZER_H
#define LH_ERROR_INITIALIZER_H

#include <lh/error/code.h>
#include <lh/initializer.h>

/**
 * @def lh_error_initializer(code, ...)
 * @brief Produces a brace-enclosed initializer for ::lh_error_t.
 *
 * Expands to ::lh_initializer(@p code, __VA_ARGS__).
 * The first field is @c code; remaining tokens initialize
 * @c desc  and any trailing members in struct order,
 * or you may use designated initializers in the variadic tail.
 *
 * @param code Error code (::lh_error_code_t).
 * @param ...  Optional remainder of the initializer list
 *             (e.g. description pointer or @c .desc = ...).
 *
 * Example usage:
 * @code{.c}
 * lh_error_t err = lh_error_initializer(lh_error_code_ok, lh_null);
 * lh_error_t e2 = lh_error_initializer(42, "message");
 * @endcode
 *
 * @see lh_initializer
 * @see lh_error_t
 */
#define lh_error_initializer(code, ...) lh_initializer(code, __VA_ARGS__)

/**
 * @def lh_error_empty_initializer(...)
 * @brief Initializer for a cleared / success error (::lh_error_code_ok).
 *
 * Expands to ::lh_error_initializer(::lh_error_code_ok, __VA_ARGS__).
 * With no arguments, @c desc is value-initialized (typically null).
 * Extra tokens are forwarded as the tail of the initializer list.
 *
 * @param ... Optional tail passed to ::lh_error_initializer after the code.
 *
 * Example usage:
 * @code{.c}
 * lh_error_t ok = lh_error_empty_initializer();
 * @endcode
 *
 * @see lh_error_initializer
 * @see lh_error_code_ok
 */
#define lh_error_empty_initializer(...) lh_error_initializer(lh_error_code_ok, __VA_ARGS__)

#endif // LH_ERROR_INITIALIZER_H
