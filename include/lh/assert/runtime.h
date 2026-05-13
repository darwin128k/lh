/**
 * @file runtime.h
 * @brief Runtime assertions — check a condition and raise if false.
 *
 * Builds on ::lh_runtime_raise to provide conditional throw in one step.
 *
 * Primary macro: ::lh_runtime_assert
 * - `(expr)`            — raise with ::lh_runtime_error_code_interrupt, no message
 * - `(expr, arg)`       — raise via ::lh_runtime_raise (code or message)
 * - `(expr, code, msg)` — raise with code + message
 *
 * @see lh_runtime_raise
 * @see lh_runtime_throw
 */

#ifndef LH_ASSERT_RUNTIME_H
#define LH_ASSERT_RUNTIME_H

#endif // LH_ASSERT_RUNTIME_H
