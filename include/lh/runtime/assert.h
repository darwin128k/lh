/**
 * @file assert.h
 * @brief Runtime checks: a failed one reports its site
 *        (::lh_runtime_check_fail) and ends the program.
 *
 * Two families, split by what a failure means:
 *   - `lh_runtime_check_*` — the environment failed (allocation, a missing
 *     allocator callback): always compiled in, in every build;
 *   - `lh_runtime_assert_*` — the caller broke a contract (null pointer,
 *     index out of range, bad argument): compiled in only while
 *     ::LH_RUNTIME_ASSERT_ENABLED is 1 — by default in builds without
 *     `NDEBUG`, i.e. not in Release, the same rule as C's `assert`.
 *
 * An enabled assertion fails in one of two ways:
 *   - by default it reports its site to the host's handler
 *     (::lh_runtime_check_fail, `lh/runtime/check.h`) and then calls
 *     ::lh_runtime_terminate;
 *   - with ::LH_RUNTIME_ASSERT_TRAP it stops the process on the spot
 *     (::lh_compiler_trap) — no error object, no call, so the checked
 *     functions stay cheap enough to keep in a Release build.
 *
 * A disabled assertion does not evaluate its condition or its initializer,
 * and a trapping one does not evaluate its initializer, so neither may have
 * side effects.
 *
 * The second argument to all these macros is an @p initializer — an
 * ::lh_runtime_error_t naming what went wrong (e.g. ::lh_runtime_error_make_by_code).
 * It documents the check at the call site; it is not evaluated — nothing is
 * thrown, so there is no error object to build.
 */

#ifndef LH_RUNTIME_ASSERT_H
#define LH_RUNTIME_ASSERT_H

#include <lh/compiler/trap.h>
#include <lh/runtime/error.h>
#include <lh/null.h>
#include <lh/runtime/check.h>

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

/**
 * @def LH_RUNTIME_ASSERT_TRAP
 * @brief `1` to make a failed contract assertion stop the process on the
 *        spot (::lh_compiler_trap) instead of raising an error; `0`
 *        (default) to raise it.
 *
 * Only matters while ::LH_RUNTIME_ASSERT_ENABLED is `1` — e.g. a Release
 * build with `-DLH_RUNTIME_ASSERT_ENABLED=1 -DLH_RUNTIME_ASSERT_TRAP=1`
 * keeps the checks at a fraction of their raising cost. Always-on
 * `lh_runtime_check_*` are not affected: they always raise.
 */
#ifndef LH_RUNTIME_ASSERT_TRAP
#    define LH_RUNTIME_ASSERT_TRAP 0
#endif

/* ── failure reporting ─────────────────────────────────────────────────── */

/**
 * @def lh_runtime_check_fail_here(expr)
 * @brief Hand this check's static ::lh_runtime_check_site_t (filled per
 *        ::LH_LIBRARY_OPTION_RUNTIME_CHECK_REPORT) to ::lh_runtime_check_fail.
 */
#if (LH_LIBRARY_OPTION_RUNTIME_CHECK_REPORT == LH_RUNTIME_CHECK_REPORT_FULL)
#    define lh_runtime_check_fail_here(expr)                                                       \
        do                                                                                         \
        {                                                                                          \
            static const lh_runtime_check_site_t _lh_check_site = {__FILE__, __func__, #expr,      \
                                                                   __LINE__};                      \
            lh_runtime_check_fail(&_lh_check_site);                                                \
        } while (0)
#elif (LH_LIBRARY_OPTION_RUNTIME_CHECK_REPORT == LH_RUNTIME_CHECK_REPORT_LOCATION)
#    define lh_runtime_check_fail_here(expr)                                                       \
        do                                                                                         \
        {                                                                                          \
            static const lh_runtime_check_site_t _lh_check_site = {                                \
                __FILE__, (lh_str_cptr)lh_null, (lh_str_cptr)lh_null, __LINE__};                   \
            lh_runtime_check_fail(&_lh_check_site);                                                \
        } while (0)
#else
#    define lh_runtime_check_fail_here(expr)                                                       \
        lh_runtime_check_fail((const lh_runtime_check_site_t *)lh_null)
#endif

/* ── always-on checks ──────────────────────────────────────────────────── */

/**
 * @def lh_runtime_check_if(expr, initializer)
 * @brief Terminate if @p expr is *true* — in every build.
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
        (void)sizeof(initializer);                                                                 \
        if (expr)                                                                                  \
        {                                                                                          \
            lh_runtime_check_fail_here(expr);                                                      \
        }                                                                                          \
    } while (0)

/**
 * @def lh_runtime_check_ifn(expr, initializer)
 * @brief Terminate if @p expr is *false* — in every build. Inverse of
 *        ::lh_runtime_check_if.
 */
#define lh_runtime_check_ifn(expr, initializer) lh_runtime_check_if(!(expr), initializer)

/* ── contract assertions (not in Release) ──────────────────────────────── */

/**
 * @def lh_runtime_assert_if(expr, initializer)
 * @brief Terminate if @p expr is *true* — only while ::LH_RUNTIME_ASSERT_ENABLED.
 *
 * @param expr        Condition; if true, terminates.
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
#if LH_RUNTIME_ASSERT_ENABLED && LH_RUNTIME_ASSERT_TRAP
/* The initializer only names the error, which a trap does not report: keep
   it "used" through sizeof without building it. */
#    define lh_runtime_assert_if(expr, initializer)                                                \
        do                                                                                         \
        {                                                                                          \
            (void)sizeof(initializer);                                                             \
            if (expr)                                                                              \
            {                                                                                      \
                lh_compiler_trap();                                                                \
            }                                                                                      \
        } while (0)
#elif LH_RUNTIME_ASSERT_ENABLED
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
 * @see lh_runtime_terminate
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
    lh_runtime_assert(expr,                                                                        \
                      lh_runtime_error_make(lh_runtime_error_code_null_pointer,                    \
                                            lh_str_view_lit("invalid reference to null pointer")))

#endif /* LH_RUNTIME_ASSERT_H */
