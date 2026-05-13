/**
 * @file assert.h
 * @brief Runtime assertion macros that throw by description on failure.
 */

#ifndef LH_RUNTIME_ASSERT_H
#define LH_RUNTIME_ASSERT_H

#include <lh/runtime/raise.h>
#include <lh/util/arg.h>

/* ── internal dispatch ─────────────────────────────────────────────────── */

/**
 * @internal
 * @def lh_runtime_assert_impl_1(expr)
 * @brief Internal: throw from single argument — throws interrupt code.
 */
#define lh_runtime_assert_impl_1(expr)                                                             \
    do                                                                                             \
    {                                                                                              \
        if (!(expr))                                                                               \
            lh_runtime_throw(lh_runtime_error_code_interrupt);                                     \
    } while (0)

/**
 * @internal
 * @def lh_runtime_assert_impl_2(expr, arg)
 * @brief Internal: throw from expression + arg — detects if code or message.
 */
#define lh_runtime_assert_impl_2(expr, arg)                                                        \
    do                                                                                             \
    {                                                                                              \
        if (!(expr))                                                                               \
            lh_runtime_raise(arg);                                                                 \
    } while (0)

/**
 * @internal
 * @def lh_runtime_assert_impl_3(expr, code, msg)
 * @brief Internal: throw from expression + code + message.
 */
#define lh_runtime_assert_impl_3(expr, code, msg)                                                  \
    do                                                                                             \
    {                                                                                              \
        if (!(expr))                                                                               \
            lh_runtime_raise(code, msg);                                                           \
    } while (0)

/* ── public API ────────────────────────────────────────────────────────── */

/**
 * @def lh_runtime_assert(...)
 * @brief Conditional runtime assertion: throw if @p expr is false.
 *
 * Three forms:
 * - `(expr)`            — throws with ::lh_runtime_error_code_interrupt, no message
 * - `(expr, arg)`       — throws via ::lh_runtime_raise(@p arg): code or message
 * - `(expr, code, msg)` — throws with @p code and @p msg
 *
 * @param expr Condition; if false, throws.
 * @param ... Optional: code, message, or both — see forms above.
 *
 * Example usage:
 * @code{.c}
 * lh_runtime_assert(ptr != lh_null);
 * lh_runtime_assert(ptr != lh_null, lh_runtime_error_code_null_pointer);
 * lh_runtime_assert(ptr != lh_null, (lh_str_cptr)"null pointer");
 * lh_runtime_assert(ptr != lh_null, lh_runtime_error_code_null_pointer, "null pointer");
 * @endcode
 *
 * @see lh_runtime_raise
 * @see lh_runtime_throw
 */
#define lh_runtime_assert(...)                                                                     \
    lh_arg_concat(lh_runtime_assert_impl_, lh_arg_get_count(__VA_ARGS__))(__VA_ARGS__)

/**
 * @def lh_runtime_assert_if(expr, arg)
 * @brief Throw if @p expr is *true* (inverse of ::lh_runtime_assert).
 *
 * Useful for early-exit on error conditions without negating the predicate
 * at the call site.
 *
 * @param expr Condition; if true, throws via ::lh_runtime_throw(@p arg).
 * @param arg  Error code or message — same rules as the two-argument form of
 *             ::lh_runtime_assert.
 *
 * Example usage:
 * @code{.c}
 * lh_runtime_assert_if(result < 0, lh_runtime_error_code_invalid_argument);
 * @endcode
 *
 * @see lh_runtime_assert
 * @see lh_runtime_throw
 */
#define lh_runtime_assert_if(expr, arg)                                                            \
    do                                                                                             \
    {                                                                                              \
        if (expr)                                                                                  \
            lh_runtime_throw(arg);                                                                 \
    } while (0)

/**
 * @def lh_runtime_assert_ifn(expr, arg)
 * @brief Throw if @p expr is *false* — shorthand for the two-argument form of
 *        ::lh_runtime_assert.
 *
 * @param expr Condition; if false, throws via ::lh_runtime_raise(@p arg).
 * @param arg  Error code or message — same rules as the two-argument form of
 *             ::lh_runtime_assert.
 *
 * Example usage:
 * @code{.c}
 * lh_runtime_assert_ifn(ptr != lh_null, lh_runtime_error_code_null_pointer);
 * @endcode
 *
 * @see lh_runtime_assert
 * @see lh_runtime_assert_if
 */
#define lh_runtime_assert_ifn(expr, arg) lh_runtime_assert_if(!(expr), arg)

/**
 * @def lh_runtime_assert_ref(expr)
 * @brief Null-reference guard — throws ::lh_runtime_error_code_null_pointer if @p expr is false.
 *
 * Shorthand for `::lh_runtime_assert_ifn(@p expr, ::lh_runtime_error_code_null_pointer)`.
 *
 * @param expr Condition; if false (e.g. null pointer), throws.
 *
 * Example usage:
 * @code{.c}
 * lh_runtime_assert_ref(ptr != lh_null);
 * @endcode
 *
 * @see lh_runtime_assert_ifn
 * @see lh_runtime_error_code_null_pointer
 */
#define lh_runtime_assert_ref(expr) lh_runtime_assert_ifn(expr, lh_runtime_error_code_null_pointer)

#endif // LH_RUNTIME_ASSERT_H