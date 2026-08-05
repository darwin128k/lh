/**
 * @file catch/initializer.h
 * @brief Brace-enclosed initializer helpers for ::lh_exception_catch_t.
 *
 * These macros expand to a brace list suitable for the first struct member,
 * ::lh_exception_catch_t::exception. The ::lh_exception_catch_t::env member
 * (the jump buffer) is implicitly zero-initialized; it is armed later by
 * ::lh_runtime_exception_catch_stack_capture (via `setjmp`).
 */

#ifndef LH_EXCEPTION_CATCH_INITIALIZER_H
#define LH_EXCEPTION_CATCH_INITIALIZER_H

#include <lh/exception/initializer.h>

/**
 * @def lh_exception_catch_initializer(...)
 * @brief Brace-enclosed initializer for ::lh_exception_catch_t::exception from exception arguments.
 *
 * Forwards @p ... to ::lh_exception_initializer. Remaining members of ::lh_exception_catch_t
 * (notably `env`) get default/zero initialization.
 *
 * @param ... Arguments passed to ::lh_exception_initializer (error code and optional tail).
 *
 * @see lh_exception_initializer
 * @see lh_exception_catch_t
 * @see lh_runtime_exception_catch_stack_capture
 */
#define lh_exception_catch_initializer(...) lh_initializer(lh_exception_initializer(__VA_ARGS__))

/**
 * @def lh_exception_catch_empty_initializer()
 * @brief Brace-enclosed initializer for a cleared catch frame (success / empty exception).
 *
 * Uses ::lh_exception_empty_initializer for `exception`. `env` is zero-initialized until capture.
 *
 * @see lh_exception_empty_initializer
 * @see lh_exception_catch_t
 * @see lh_runtime_exception_catch_stack_capture
 */
#define lh_exception_catch_empty_initializer() lh_initializer(lh_exception_empty_initializer())

#endif /* LH_EXCEPTION_CATCH_INITIALIZER_H */
