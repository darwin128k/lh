/**
 * @file check.h
 * @brief What happens when a runtime check fails: the host's handler, then
 *        ::lh_runtime_terminate.
 *
 * A failed `lh_runtime_check_*` / `lh_runtime_assert_*` (see
 * `lh/core/runtime/assert.h`) calls ::lh_runtime_check_fail with the static
 * ::lh_exception_origin_t of that check and its error code; that builds
 * an ::lh_exception_t and hands it to the calling thread's
 * ::lh_runtime_check_handler_t. The handler is the host's to choose — print to
 * a console, log to a server, blink an LED, reset the board. Nothing is thrown
 * and nothing unwinds; ::lh_runtime_terminate runs after the handler.
 *
 * The default handler (::lh_runtime_check_fail_default) logs the exception at
 * ::lh_logger_level_critical on the runtime logger (::lh_runtime_logger_get_mut):
 * it reaches whatever sink the host installed there (`stderr`, a UART, a
 * server) and is dropped while there is none.
 *
 * The slot is one object for the whole program, empty until the host fills it
 * through ::lh_runtime_check_get_mut — before the first use and before it starts
 * other threads (a thread sees everything written before it was created).
 * Every thread, the ones LH starts and the host's own, uses the same object. */

#ifndef LH_RUNTIME_CHECK_H
#define LH_RUNTIME_CHECK_H

#include <lh/core/attribute/noreturn.h>
#include <lh/core/library/fallback.h>
#include <lh/core/runtime/check/handler.h>
#include <lh/core/runtime/check/report.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The default report call: the exception on `stderr` when the C library
 *        is there to print it, nothing otherwise; @p self is unused.
 */
lh_void
lh_runtime_check_fail_default(lh_self_ptr self, const lh_exception_t *exception);

/**
 * @brief Return the runtime failed-check handler, for change.
 *
 * One object for every thread. The host fills it before the first use and
 * before it starts other threads; a change while other threads use it is a
 * data race.
 *
 * @return The runtime ::lh_runtime_check_handler_t (never null).
 */
lh_runtime_check_handler_t *
lh_runtime_check_get_mut(void);

/**
 * @brief Return the runtime failed-check handler.
 *
 * Same object as ::lh_runtime_check_get_mut, read-only.
 *
 * @return The runtime ::lh_runtime_check_handler_t (never null).
 */
const lh_runtime_check_handler_t *
lh_runtime_check_get(void);

/**
 * @brief Build the ::lh_exception_t of a check failed with @p code at
 *        @p origin, report it to the handler, then ::lh_runtime_terminate.
 *        Does not return.
 *
 * Called by the check macros; @p origin is ::lh_null at
 * ::LH_RUNTIME_CHECK_REPORT_NONE (the exception then has the code only).
 */
LH_ATTRIBUTE_NORETURN
lh_void
lh_runtime_check_fail(const lh_exception_origin_t *origin, lh_runtime_error_code_t code);

LH_COMPILER_EXTERN_C_END

#endif /* LH_RUNTIME_CHECK_H */
