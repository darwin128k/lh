/**
 * @file ref.h
 * @brief Runtime check macro that throws ::lh_runtime_error_code_null_pointer on failure.
 */

#ifndef LH_RUNTIME_CHECK_REF_H
#define LH_RUNTIME_CHECK_REF_H

#include <lh/runtime/check.h>
#include <lh/runtime/error/code.h>

/**
 * @def lh_runtime_check_ref(...)
 * @brief Null-reference guard macro.
 *
 * Expands to `::lh_runtime_check(__VA_ARGS__, ::lh_runtime_error_code_null_pointer)`.
 *
 * Because ::lh_runtime_check throws when the expression is **false**, a typical
 * use is to pass a non-null pointer test (e.g. a bare pointer) so a
 * null reference triggers the throw.
 *
 * @param ... Single boolean expression (often a pointer) that must hold for execution
 *            to continue. Do not pass a comma-separated argument list unless the whole
 *            condition is parenthesized so it forms one macro argument.
 *
 * Example usage:
 * @code{.c}
 * lh_runtime_check_ref(p != lh_null);
 * @endcode
 *
 * @see lh_runtime_check
 * @see lh_runtime_error_code_null_pointer
 * @see lh_runtime_throw_with_code
 */
#define lh_runtime_check_ref(...)                                                                  \
    lh_runtime_check(__VA_ARGS__, lh_runtime_error_code_null_pointer)

#endif // LH_RUNTIME_CHECK_REF_H
