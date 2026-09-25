/**
 * @file assert.h
 * @brief Runtime checks that raise an ::lh_runtime_error_t on failure.
 *
 * Two families, split by what a failure means:
 *   - `lh_runtime_check_*` — the environment failed (allocation, a missing
 *     allocator callback): always compiled in, in every build;
 *   - `lh_runtime_assert_*` — the caller broke a contract (null pointer,
 *     index out of range, bad argument): compiled in only while
 *     ::LH_RUNTIME_ASSERT_ENABLED is 1 — by default in builds without
 *     `NDEBUG`, i.e. not in Release, the same rule as C's `assert`.
 *
 * A disabled assertion does not evaluate its condition or its initializer,
 * so they must have no side effects.
 *
 * The second argument to all these macros is an @p initializer — any expression
 * that produces an ::lh_runtime_error_t value, e.g.
 * ::lh_runtime_error_make or a compound literal.
 * The value is stored in a local variable before its address is taken, so
 * rvalue results from function calls are fully supported.
 */

#ifndef LH_RUNTIME_ASSERT_H
#define LH_RUNTIME_ASSERT_H

#include <lh/runtime/raise.h>

/**
 * @def LH_RUNTIME_ASSERT_ENABLED
 * @brief `1` when `lh_runtime_assert_*` (and `lh_assert_runtime_*`) are
 *        compiled in, `0` when they vanish.
 *
 * Defaults to `1` without `NDEBUG` and `0` with it. Define it yourself
 * (e.g. `-DLH_RUNTIME_ASSERT_ENABLED=1`) to keep assertions in a Release
 * build, or to drop them from a Debug one. The library and code that inlines
 * its headers should agree on it.
 */
#ifndef LH_RUNTIME_ASSERT_ENABLED
#    if defined(NDEBUG)
#        define LH_RUNTIME_ASSERT_ENABLED 0
#    else
#        define LH_RUNTIME_ASSERT_ENABLED 1
#    endif
#endif

/* ── always-on checks ──────────────────────────────────────────────────── */

/**
 * @def lh_runtime_check_if(expr, initializer)
 * @brief Throw if @p expr is *true* — in every build.
 *
 * For failures of the environment rather than of the caller (allocation
 * failed, allocator not configured). Contract violations use
 * ::lh_runtime_assert_if instead.
 *
 * @see lh_runtime_check_ifn
 */
#define lh_runtime_check_if(expr, initializer)                                                     \
    do                                                                                             \
    {                                                                                              \
        if (expr)                                                                                  \
        {                                                                                          \
            lh_runtime_error_t _err = (initializer);                                               \
            lh_runtime_raise(lh_addr_of(_err));                                                    \
        }                                                                                          \
    } while (0)

/**
 * @def lh_runtime_check_ifn(expr, initializer)
 * @brief Throw if @p expr is *false* — in every build. Inverse of
 *        ::lh_runtime_check_if.
 */
#define lh_runtime_check_ifn(expr, initializer) lh_runtime_check_if(!(expr), initializer)

/* ── contract assertions (not in Release) ──────────────────────────────── */

/**
 * @def lh_runtime_assert_if(expr, initializer)
 * @brief Throw if @p expr is *true* — only while ::LH_RUNTIME_ASSERT_ENABLED.
 *
 * @param expr        Condition; if true, throws.
 * @param initializer An ::lh_runtime_error_t value — any expression
 *                    (function call, compound literal, or variable).
 *
 * Example usage:
 * @code{.c}
 * lh_runtime_assert_if(result < 0,
 *     lh_runtime_error_make(lh_runtime_error_code_invalid_argument, lh_str_view_lit("bad result"));
 * @endcode
 *
 * @see lh_runtime_assert_ifn
 */
#if LH_RUNTIME_ASSERT_ENABLED
#    define lh_runtime_assert_if(expr, initializer) lh_runtime_check_if(expr, initializer)
#else
/* sizeof keeps the operands "used" (no unused-variable warnings) without
   evaluating them. */
#    define lh_runtime_assert_if(expr, initializer) ((void)sizeof(!(expr)))
#endif

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
 *     lh_runtime_error_make(lh_runtime_error_code_null_pointer, lh_str_view_empty()));
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
 *     lh_runtime_error_make(lh_runtime_error_code_null_pointer, lh_str_view_empty()));
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
                                                  lh_str_view_lit("invalid reference to null pointer")))

#endif /* LH_RUNTIME_ASSERT_H */
