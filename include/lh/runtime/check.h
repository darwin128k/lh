/**
 * @file check.h
 * @brief What happens when a runtime check fails: the host's handler, then
 *        ::lh_runtime_terminate.
 *
 * A failed `lh_runtime_check_*` / `lh_runtime_assert_*` (see
 * `lh/runtime/assert.h`) calls ::lh_runtime_check_fail with the static
 * ::lh_exception_origin_t of that check and its error code; that builds
 * an ::lh_exception_t and hands it to the failure handler. The handler is the
 * host's to choose — print to a console, log to a server, blink an LED,
 * reset the board — installed with ::lh_runtime_check_set, the same way
 * ::lh_runtime_terminate_set installs the terminate handler. Nothing is
 * thrown and nothing unwinds.
 *
 * The default handler writes the exception to `stderr` when the library may use
 * the C library (::LH_LIBRARY_OPTION_RUNTIME_TERMINATE_USE_STDLIB) and does
 * nothing otherwise.
 */

#ifndef LH_RUNTIME_CHECK_H
#define LH_RUNTIME_CHECK_H

#include <lh/attribute/noreturn.h>
#include <lh/compiler/extern/c.h>
#include <lh/library/fallback.h>
#include <lh/exception.h>
#include <lh/runtime/check/fail/cb.h>
#include <lh/runtime/check/report.h>
#include <lh/exception/origin.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Current handler for failed runtime checks (never ::lh_null).
 */
lh_runtime_check_fail_cb
lh_runtime_check_get(void);

/**
 * @brief Install the handler for failed runtime checks.
 * @param fn New handler, or ::lh_null to restore the default one.
 */
void
lh_runtime_check_set(lh_runtime_check_fail_cb fn);

/**
 * @brief Build the ::lh_exception_t of a check failed with @p code at
 *        @p origin, report it to the handler, then ::lh_runtime_terminate.
 *        Does not return.
 *
 * Called by the check macros; @p origin is ::lh_null at
 * ::LH_RUNTIME_CHECK_REPORT_NONE (the exception then has the code only).
 */
LH_ATTRIBUTE_NORETURN
void
lh_runtime_check_fail(const lh_exception_origin_t *origin, lh_runtime_error_code_t code);

LH_COMPILER_EXTERN_C_END

#endif /* LH_RUNTIME_CHECK_H */
