/**
 * @file check.h
 * @brief Macros that throw a runtime exception when a boolean condition matches.
 */

#ifndef LH_RUNTIME_CHECK_H
#define LH_RUNTIME_CHECK_H

#include <lh/runtime/throw/by/code.h>

/**
 * @def lh_runtime_check_if(expr, code)
 * @brief If @p expr is true, throw with @p code.
 *
 * Expands to @c if (@p expr) ::lh_runtime_throw_with_code(@p code).
 * Use when the “bad” case is a true boolean (e.g. an error flag already set).
 *
 * @param expr Condition; if it evaluates to non-zero, an exception is thrown.
 * @param code Error code passed to ::lh_runtime_throw_with_code (::lh_error_code_t /
 *             ::lh_runtime_error_code_t).
 *
 * @see lh_runtime_check_ifn
 * @see lh_runtime_throw_with_code
 */
#define lh_runtime_check_if(expr, code)                                                            \
    if (expr)                                                                                      \
    lh_runtime_throw_with_code(code)

/**
 * @def lh_runtime_check_ifn(expr, code)
 * @brief If @p expr is false, throw with @p code.
 *
 * Equivalent to ::lh_runtime_check_if(!(@p expr), @p code).
 * Use when the condition must hold for execution to continue
 * (typical invariant / pointer-validity check).
 *
 * @param expr Condition; if it evaluates to zero, an exception is thrown.
 * @param code Error code passed to ::lh_runtime_throw_with_code.
 *
 * @see lh_runtime_check_if
 * @see lh_runtime_check
 * @see lh_runtime_check_ref
 */
#define lh_runtime_check_ifn(expr, code) lh_runtime_check_if(!(expr), code)

/**
 * @def lh_runtime_check(expr, code)
 * @brief Require @p expr to be true; otherwise throw with @p code.
 *
 * Same as ::lh_runtime_check_ifn(@p expr, @p code).
 *
 * @param expr Condition that must be non-zero to avoid throwing.
 * @param code Error code passed to ::lh_runtime_throw_with_code.
 *
 * Example usage:
 * @code{.c}
 * lh_runtime_check(p != lh_null, lh_runtime_error_code_null_pointer);
 * @endcode
 *
 * @see lh_runtime_check_ifn
 * @see lh_runtime_throw_with_code
 */
#define lh_runtime_check(expr, code) lh_runtime_check_ifn(expr, code)

#endif // LH_RUNTIME_CHECK_H
