/**
 * @file raise.h
 * @brief Smart throw dispatch — auto-selects variant
 *        from argument count and type.
 *
 * Wraps ::lh_runtime_throw with three convenient forms:
 * - `(code)`       — error code only
 * - `(msg)`        — message only, uses ::lh_runtime_error_code_interrupt
 * - `(code, msg)`  — full code + message
 *
 * @note The single-argument form uses @c _Generic
 *       to distinguish a code from a message pointer.
 *
 *       String literals have type @c char[N] in C and do notmatch
 *       ::lh_str_cptr / ::lh_str_ptr; cast them explicitly:
 *       @code{.c}
 *       lh_runtime_raise((lh_str_cptr)"open failed");
 *       @endcode
 *
 * @see lh_runtime_throw
 * @see lh_runtime_assert
 */

#ifndef LH_RUNTIME_RAISE_H
#define LH_RUNTIME_RAISE_H

#include <lh/runtime/error/code.h>
#include <lh/runtime/throw.h>
#include <lh/util/arg.h>
#include <lh/util/str/ptr.h>

/* ── internal dispatch ─────────────────────────────────────────────────── */

/**
 * @internal
 * @def lh_runtime_raise_impl_1(arg)
 * @brief Internal: throw from single argument — detects if code or message.
 */
#define lh_runtime_raise_impl_1(arg)                                                               \
    _Generic((arg),                                                                                \
        lh_str_cptr: lh_runtime_throw(lh_runtime_error_code_interrupt, arg),                       \
        lh_str_ptr: lh_runtime_throw(lh_runtime_error_code_interrupt, arg),                        \
        default: lh_runtime_throw(arg))

/**
 * @internal
 * @def lh_runtime_raise_impl_2(code, msg)
 * @brief Internal: throw from code + message.
 */
#define lh_runtime_raise_impl_2(code, msg) lh_runtime_throw(code, msg)

/* ── public API ────────────────────────────────────────────────────────── */

/**
 * @def lh_runtime_raise(...)
 * @brief Throw a runtime exception, auto-detecting argument pattern.
 *
 * Three forms:
 * - `(code)`      — throws with the given error code, no description
 * - `(msg)`       — throws with ::lh_runtime_error_code_interrupt and @p msg;
 *                   @p msg must be ::lh_str_cptr or ::lh_str_ptr
 * - `(code, msg)` — throws with the given code and description
 *
 * @param ... Error code, message, or both — see forms above.
 *
 * Example usage:
 * @code{.c}
 * lh_runtime_raise(lh_runtime_error_code_null_pointer);
 * lh_runtime_raise((lh_str_cptr)"open failed");
 * lh_runtime_raise(lh_runtime_error_code_null_pointer, "null pointer");
 * @endcode
 *
 * @see lh_runtime_throw
 * @see lh_runtime_error_code_interrupt
 * @see lh_runtime_assert
 */
#define lh_runtime_raise(...)                                                                      \
    lh_arg_concat(lh_runtime_raise_impl_, lh_arg_get_count(__VA_ARGS__))(__VA_ARGS__)

#endif // LH_RUNTIME_RAISE_H
