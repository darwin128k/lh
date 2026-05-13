/**
 * @file runtime.h
 * @brief Aliases for runtime assertion macros.
 */

#ifndef LH_ASSERT_RUNTIME_H
#define LH_ASSERT_RUNTIME_H

#include <lh/runtime/assert.h>

/**
 * @def lh_assert_runtime(...)
 * @brief Alias for ::lh_runtime_assert.
 *
 * Example usage:
 * @code{.c}
 * lh_assert_runtime(ptr != lh_null);
 * lh_assert_runtime(ptr != lh_null, lh_runtime_error_code_null_pointer);
 * @endcode
 *
 * @see lh_runtime_assert
 */
#define lh_assert_runtime(...) lh_runtime_assert(__VA_ARGS__)

/**
 * @def lh_assert_runtime_if(condition, ...)
 * @brief Alias for ::lh_runtime_assert_if.
 *
 * Example usage:
 * @code{.c}
 * lh_assert_runtime_if(result < 0, lh_runtime_error_code_invalid_argument);
 * @endcode
 *
 * @see lh_runtime_assert_if
 */
#define lh_assert_runtime_if(condition, ...) lh_runtime_assert_if(condition, __VA_ARGS__)

/**
 * @def lh_assert_runtime_if_not(condition, ...)
 * @brief Alias for ::lh_runtime_assert_ifn.
 *
 * Example usage:
 * @code{.c}
 * lh_assert_runtime_if_not(ptr != lh_null, lh_runtime_error_code_null_pointer);
 * @endcode
 *
 * @see lh_runtime_assert_ifn
 */
#define lh_assert_runtime_if_not(condition, ...) lh_runtime_assert_ifn(condition, __VA_ARGS__)

#endif // LH_ASSERT_RUNTIME_H
