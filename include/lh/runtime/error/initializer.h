/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macros for ::lh_runtime_error_t.
 */

#ifndef LH_RUNTIME_ERROR_INITIALIZER_H
#define LH_RUNTIME_ERROR_INITIALIZER_H

#include <lh/initializer.h>
#include <lh/runtime/error/code.h>

/**
 * @def lh_runtime_error_initializer(code, ...)
 * @brief Produces a brace-enclosed initializer for ::lh_runtime_error_t.
 *
 * Expands to ::lh_initializer(@p code, __VA_ARGS__).
 * The first field is @c code; remaining tokens initialize
 * @c desc and any trailing members in struct order,
 * or you may use designated initializers in the variadic tail.
 *
 * @param code Error code (::lh_runtime_error_code_t).
 * @param ...  Optional remainder of the initializer list
 *             (e.g. description pointer or @c .desc = ...).
 *
 * Example usage:
 * @code{.c}
 * lh_runtime_error_t err = lh_runtime_error_initializer(lh_runtime_error_code_interrupt, lh_null);
 * lh_runtime_error_t e2  = lh_runtime_error_initializer(lh_runtime_error_code_null_pointer, "msg");
 * @endcode
 *
 * @see lh_initializer
 * @see lh_runtime_error_t
 * @see lh_runtime_error_empty_initializer
 */
#define lh_runtime_error_initializer(code, ...) lh_initializer(code, __VA_ARGS__)

/**
 * @def lh_runtime_error_empty_initializer(...)
 * @brief Initializer for a cleared / success runtime error (::lh_runtime_error_code_ok).
 *
 * Expands to ::lh_runtime_error_initializer(::lh_runtime_error_code_ok, __VA_ARGS__).
 * With no arguments, @c desc is value-initialized (typically null).
 *
 * @param ... Optional tail passed to ::lh_runtime_error_initializer after the code.
 *
 * Example usage:
 * @code{.c}
 * lh_runtime_error_t ok = lh_runtime_error_empty_initializer();
 * @endcode
 *
 * @see lh_runtime_error_initializer
 * @see lh_runtime_error_code_ok
 */
#define lh_runtime_error_empty_initializer(...) \
    lh_runtime_error_initializer(lh_runtime_error_code_ok, __VA_ARGS__)

#endif // LH_RUNTIME_ERROR_INITIALIZER_H
