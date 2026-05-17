/**
 * @file assert.h
 * @brief Runtime assertion macros that raise an ::lh_runtime_error_t on failure.
 *
 * The second argument to all assertion macros is an @p initializer — any expression
 * that produces an ::lh_runtime_error_t value, e.g.
 * ::lh_runtime_error_make or a compound literal.
 * The value is stored in a local variable before its address is taken, so
 * rvalue results from function calls are fully supported.
 */

#ifndef LH_RUNTIME_ASSERT_H
#define LH_RUNTIME_ASSERT_H

#include <lh/runtime/raise.h>

/* ── public API ────────────────────────────────────────────────────────── */

/**
 * @def lh_runtime_assert_if(expr, initializer)
 * @brief Throw if @p expr is *true*.
 *
 * @param expr        Condition; if true, throws.
 * @param initializer An ::lh_runtime_error_t value — any expression
 *                    (function call, compound literal, or variable).
 *
 * Example usage:
 * @code{.c}
 * lh_runtime_assert_if(result < 0,
 *     lh_runtime_error_make(lh_runtime_error_code_invalid_argument, "bad result"));
 * @endcode
 *
 * @see lh_runtime_assert_ifn
 */
#define lh_runtime_assert_if(expr, initializer)                                                    \
    do                                                                                             \
    {                                                                                              \
        if (expr)                                                                                  \
        {                                                                                          \
            lh_runtime_error_t _err = (initializer);                                               \
            lh_runtime_raise(lh_addr_of(_err));                                                    \
        }                                                                                          \
    } while (0)

/**
 * @def lh_runtime_assert_ifn(expr, initializer)
 * @brief Throw if @p expr is *false* — inverse of ::lh_runtime_assert_if.
 *
 * @param expr        Condition; if false, throws.
 * @param initializer An ::lh_runtime_error_t value — same as ::lh_runtime_assert_if.
 *
 * Example usage:
 * @code{.c}
 * lh_runtime_assert_ifn(ptr != lh_null,
 *     lh_runtime_error_make(lh_runtime_error_code_null_pointer, lh_null));
 * @endcode
 *
 * @see lh_runtime_assert_if
 */
#define lh_runtime_assert_ifn(expr, initializer) lh_runtime_assert_if(!(expr), initializer)

/**
 * @def lh_runtime_assert(expr, initializer)
 * @brief Conditional runtime assertion: throw if @p expr is false.
 *
 * @param expr        Condition; if false, throws.
 * @param initializer An ::lh_runtime_error_t value — same as ::lh_runtime_assert_if.
 *
 * Example usage:
 * @code{.c}
 * lh_runtime_assert(ptr != lh_null,
 *     lh_runtime_error_make(lh_runtime_error_code_null_pointer, lh_null));
 * @endcode
 *
 * @see lh_runtime_assert_if
 * @see lh_runtime_raise
 */
#define lh_runtime_assert(expr, initializer) lh_runtime_assert_ifn(expr, initializer)

/**
 * @def lh_runtime_assert_ref(expr)
 * @brief Null-reference guard — throws ::lh_runtime_error_code_null_pointer if @p expr is false.
 *
 * @param expr Condition; if false (e.g. null pointer), throws.
 *
 * Example usage:
 * @code{.c}
 * lh_runtime_assert_ref(ptr);
 * @endcode
 *
 * @see lh_runtime_assert
 * @see lh_runtime_error_code_null_pointer
 */
#define lh_runtime_assert_ref(expr)                                                                \
    lh_runtime_assert(expr, lh_runtime_error_make(lh_runtime_error_code_null_pointer,              \
                                                  "invalid reference to null pointer"))

#endif // LH_RUNTIME_ASSERT_H
