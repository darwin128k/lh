/**
 * @file runtime.h
 * @brief Aliases for runtime assertion macros.
 */

#ifndef LH_ASSERT_RUNTIME_H
#define LH_ASSERT_RUNTIME_H

#include <lh/runtime/assert.h>

/**
 * @def lh_assert_runtime(expr, initializer)
 * @brief Alias for ::lh_runtime_assert.
 *
 * Example usage:
 * @code{.c}
 * lh_assert_runtime(ptr != lh_null,
 *     lh_runtime_error_make(lh_runtime_error_code_null_pointer, lh_null));
 * @endcode
 *
 * @see lh_runtime_assert
 */
#define lh_assert_runtime(expr, initializer) lh_runtime_assert(expr, initializer)

/**
 * @def lh_assert_runtime_if(expr, initializer)
 * @brief Alias for ::lh_runtime_assert_if.
 *
 * Example usage:
 * @code{.c}
 * lh_assert_runtime_if(result < 0,
 *     lh_runtime_error_make(lh_runtime_error_code_invalid_argument, "bad result"));
 * @endcode
 *
 * @see lh_runtime_assert_if
 */
#define lh_assert_runtime_if(expr, initializer) lh_runtime_assert_if(expr, initializer)

/**
 * @def lh_assert_runtime_ifn(expr, initializer)
 * @brief Alias for ::lh_runtime_assert_ifn.
 *
 * Example usage:
 * @code{.c}
 * lh_assert_runtime_ifn(ptr != lh_null,
 *     lh_runtime_error_make(lh_runtime_error_code_null_pointer, lh_null));
 * @endcode
 *
 * @see lh_runtime_assert_ifn
 */
#define lh_assert_runtime_ifn(expr, initializer) lh_runtime_assert_ifn(expr, initializer)

/**
 * @def lh_assert_runtime_ref(expr)
 * @brief Alias for ::lh_runtime_assert_ref.
 *
 * Example usage:
 * @code{.c}
 * lh_assert_runtime_ref(ptr);
 * @endcode
 *
 * @see lh_runtime_assert_ref
 */
#define lh_assert_runtime_ref(expr) lh_runtime_assert_ref(expr)

#endif // LH_ASSERT_RUNTIME_H
