/**
 * @file assert.h
 * @brief Runtime checks: a failed one reports an ::lh_exception_t
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
 * Every macro takes the condition, then the error: an
 * ::lh_runtime_error_code_t and, optionally, a message string literal.
 * @code{.c}
 * lh_runtime_assert_ifn(lh_memory_bounds_is_valid(self), lh_runtime_error_code_invalid_range);
 * lh_runtime_assert(ptr != lh_null, lh_runtime_error_code_null_pointer, "no buffer");
 * @endcode
 *
 * Nothing is built at the check: the origin and message are one `static const`
 * ::lh_exception_origin_t per check, and the failing branch passes its
 * address and the code to ::lh_runtime_check_fail, which fills the
 * ::lh_exception_t the handler gets.
 *
 * An enabled assertion fails in one of two ways:
 *   - by default it reports to the host's handler (::lh_runtime_check_fail,
 *     `lh/runtime/check.h`) and then calls ::lh_runtime_terminate;
 *   - with ::LH_RUNTIME_ASSERT_TRAP it stops the process on the spot
 *     (::lh_compiler_trap) — no report, no call, so the checked functions
 *     stay cheap enough to keep in a Release build.
 *
 * A disabled assertion does not evaluate its condition or its error, and a
 * trapping one does not evaluate its error, so neither may have side effects.
 */

#ifndef LH_RUNTIME_ASSERT_H
#define LH_RUNTIME_ASSERT_H

#include <lh/compiler/trap.h>
#include <lh/null.h>
#include <lh/runtime/check.h>
#include <lh/str/view/initializer.h>
#include <lh/util/arg.h>

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
 *        spot (::lh_compiler_trap) instead of reporting it; `0` (default)
 *        to report it.
 *
 * Only matters while ::LH_RUNTIME_ASSERT_ENABLED is `1` — e.g. a Release
 * build with `-DLH_RUNTIME_ASSERT_ENABLED=1 -DLH_RUNTIME_ASSERT_TRAP=1`
 * keeps the checks at a fraction of their reporting cost. Always-on
 * `lh_runtime_check_*` are not affected: they always report.
 */
#ifndef LH_RUNTIME_ASSERT_TRAP
#    define LH_RUNTIME_ASSERT_TRAP 0
#endif

/* ── the error argument: code [, message] ─────────────────────────────── */

#define lh_runtime_check_code_1(code) (code)
#define lh_runtime_check_code_2(code, message) (code)

/**
 * @def lh_runtime_check_code(...)
 * @brief The ::lh_runtime_error_code_t of a check's `code [, message]` argument.
 */
#define lh_runtime_check_code(...)                                                                 \
    lh_arg_expand(lh_arg_concat(lh_runtime_check_code_, lh_arg_get_count(__VA_ARGS__))(__VA_ARGS__))

#define lh_runtime_check_desc_1(code) lh_str_view_empty_initializer()
#define lh_runtime_check_desc_2(code, message) lh_str_view_initializer_lit(message)

/**
 * @def lh_runtime_check_desc(...)
 * @brief Initializer of the ::lh_str_view_t message of a check's
 *        `code [, message]` argument — empty when there is none. The message
 *        must be a string literal.
 */
#define lh_runtime_check_desc(...)                                                                 \
    lh_arg_expand(lh_arg_concat(lh_runtime_check_desc_, lh_arg_get_count(__VA_ARGS__))(__VA_ARGS__))

/* ── failure reporting ─────────────────────────────────────────────────── */

/**
 * @def lh_runtime_check_fail_here(expr, ...)
 * @brief Hand this check's static ::lh_exception_origin_t (filled per
 *        ::LH_LIBRARY_OPTION_RUNTIME_CHECK_REPORT, message included) and its
 *        error code to ::lh_runtime_check_fail, which builds the
 *        ::lh_exception_t.
 *
 * Only a pointer and a code leave the failing branch: an exception object
 * here would give every checked function a stack frame for it.
 */
#if (LH_LIBRARY_OPTION_RUNTIME_CHECK_REPORT == LH_RUNTIME_CHECK_REPORT_FULL)
#    define lh_runtime_check_fail_here(expr, ...)                                                  \
        do                                                                                         \
        {                                                                                          \
            static const lh_exception_origin_t _lh_exception_origin = {                            \
                __FILE__, __func__, #expr, lh_runtime_check_desc(__VA_ARGS__), __LINE__};          \
            lh_runtime_check_fail(&_lh_exception_origin, lh_runtime_check_code(__VA_ARGS__));      \
        } while (0)
#elif (LH_LIBRARY_OPTION_RUNTIME_CHECK_REPORT == LH_RUNTIME_CHECK_REPORT_LOCATION)
#    define lh_runtime_check_fail_here(expr, ...)                                                  \
        do                                                                                         \
        {                                                                                          \
            static const lh_exception_origin_t _lh_exception_origin = {                            \
                __FILE__, (lh_str_cptr)lh_null, (lh_str_cptr)lh_null,                              \
                lh_runtime_check_desc(__VA_ARGS__), __LINE__};                                     \
            lh_runtime_check_fail(&_lh_exception_origin, lh_runtime_check_code(__VA_ARGS__));      \
        } while (0)
#else
#    define lh_runtime_check_fail_here(expr, ...)                                                  \
        lh_runtime_check_fail((const lh_exception_origin_t *)lh_null,                              \
                              lh_runtime_check_code(__VA_ARGS__))
#endif

/* ── always-on checks ──────────────────────────────────────────────────── */

/**
 * @def lh_runtime_check_if(expr, ...)
 * @brief Fail if @p expr is *true* — in every build.
 *
 * For failures of the environment rather than of the caller (allocation
 * failed, allocator not configured). Contract violations use
 * ::lh_runtime_assert_if instead.
 *
 * @param expr Condition; if true, fails.
 * @param ...  `code [, message]` — an ::lh_runtime_error_code_t and an
 *             optional string literal.
 *
 * @see lh_runtime_check_ifn
 */
#define lh_runtime_check_if(expr, ...)                                                             \
    do                                                                                             \
    {                                                                                              \
        if (expr)                                                                                  \
        {                                                                                          \
            lh_runtime_check_fail_here(expr, __VA_ARGS__);                                         \
        }                                                                                          \
    } while (0)

/**
 * @def lh_runtime_check_ifn(expr, ...)
 * @brief Fail if @p expr is *false* — in every build. Inverse of
 *        ::lh_runtime_check_if.
 */
#define lh_runtime_check_ifn(expr, ...) lh_runtime_check_if(!(expr), __VA_ARGS__)

/* ── contract assertions (not in Release) ──────────────────────────────── */

/**
 * @def lh_runtime_assert_if(expr, ...)
 * @brief Fail if @p expr is *true* — only while ::LH_RUNTIME_ASSERT_ENABLED.
 *
 * @param expr Condition; if true, fails.
 * @param ...  `code [, message]` — same as ::lh_runtime_check_if.
 *
 * Example usage:
 * @code{.c}
 * lh_runtime_assert_if(result < 0, lh_runtime_error_code_invalid_argument, "bad result");
 * @endcode
 *
 * @see lh_runtime_assert_ifn
 */
#if LH_RUNTIME_ASSERT_ENABLED && LH_RUNTIME_ASSERT_TRAP
/* A trap reports nothing, so the error argument is dropped unevaluated. */
#    define lh_runtime_assert_if(expr, ...)                                                        \
        do                                                                                         \
        {                                                                                          \
            if (expr)                                                                              \
            {                                                                                      \
                lh_compiler_trap();                                                                \
            }                                                                                      \
        } while (0)
#elif LH_RUNTIME_ASSERT_ENABLED
#    define lh_runtime_assert_if(expr, ...) lh_runtime_check_if(expr, __VA_ARGS__)
#else
/* sizeof keeps the operands "used" (no unused-variable warnings) without
   evaluating them. */
#    define lh_runtime_assert_if(expr, ...) ((void)sizeof(!(expr)))
#endif

/**
 * @def lh_runtime_assert_ifn(expr, ...)
 * @brief Fail if @p expr is *false* — inverse of ::lh_runtime_assert_if.
 *
 * @param expr Condition; if false, fails.
 * @param ...  `code [, message]` — same as ::lh_runtime_check_if.
 *
 * Example usage:
 * @code{.c}
 * lh_runtime_assert_ifn(ptr != lh_null, lh_runtime_error_code_null_pointer);
 * @endcode
 *
 * @see lh_runtime_assert_if
 */
#define lh_runtime_assert_ifn(expr, ...) lh_runtime_assert_if(!(expr), __VA_ARGS__)

/**
 * @def lh_runtime_assert(expr, ...)
 * @brief Conditional runtime assertion: fail if @p expr is false.
 *
 * @param expr Condition; if false, fails.
 * @param ...  `code [, message]` — same as ::lh_runtime_check_if.
 *
 * Example usage:
 * @code{.c}
 * lh_runtime_assert(ptr != lh_null, lh_runtime_error_code_null_pointer);
 * @endcode
 *
 * @see lh_runtime_assert_if
 * @see lh_runtime_terminate
 */
#define lh_runtime_assert(expr, ...) lh_runtime_assert_ifn(expr, __VA_ARGS__)

/**
 * @def lh_runtime_assert_ref(expr)
 * @brief Null-reference guard — fails with ::lh_runtime_error_code_null_pointer
 *        if @p expr is false.
 *
 * @param expr Condition; if false (e.g. null pointer), fails.
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
    lh_runtime_assert(expr, lh_runtime_error_code_null_pointer, "invalid reference to null pointer")

#endif /* LH_RUNTIME_ASSERT_H */
