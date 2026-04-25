/**
 * @file ref.h
 * @brief Runtime check macro that throws ::lh_runtime_error_code_null_pointer on failure.
 */

#ifndef LH_RUNTIME_CHECK_REF_H
#define LH_RUNTIME_CHECK_REF_H

#include <lh/library/fallback.h>
#include <lh/runtime/check.h>
#include <lh/runtime/error/code.h>

/**
 * @def lh_runtime_check_ref(...)
 * @brief Null-reference guard macro controlled by
 *        ::LH_LIBRARY_OPTION_RUNTIME_CHECK_REF.
 *
 * Expansion depends on ::LH_LIBRARY_OPTION_RUNTIME_CHECK_REF:
 * - ::LH_LIBRARY_OPTION_ON: expands to
 *   `::lh_runtime_check(__VA_ARGS__, ::lh_runtime_error_code_null_pointer)`.
 * - ::LH_LIBRARY_OPTION_OFF: expands to a no-op.
 *
 * Because ::lh_runtime_check throws when the expression is **false**, a typical
 * ON-mode use is to pass a non-null pointer test (e.g. a bare pointer) so a
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
 * @see LH_LIBRARY_OPTION_RUNTIME_CHECK_REF
 */
#if (LH_LIBRARY_OPTION_RUNTIME_CHECK_REF == LH_LIBRARY_OPTION_ON)
#    define lh_runtime_check_ref(...)                                                              \
        lh_runtime_check(__VA_ARGS__, lh_runtime_error_code_null_pointer)
#elif (LH_LIBRARY_OPTION_RUNTIME_CHECK_REF == LH_LIBRARY_OPTION_OFF)
#    define lh_runtime_check_ref(...)
#endif

#endif // LH_RUNTIME_CHECK_REF_H
