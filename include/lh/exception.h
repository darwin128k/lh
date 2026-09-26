/**
 * @file exception.h
 * @brief What a failed runtime check reports (::lh_exception_t): the error
 *        and where it happened.
 *
 * A failed `lh_runtime_check_*` / `lh_runtime_assert_*` (see
 * `lh/runtime/assert.h`) ends up in ::lh_runtime_check_fail, which fills one
 * ::lh_exception_t and hands the host's handler a pointer to it
 * (`lh/runtime/check.h`). Nothing is thrown or caught: after the handler
 * the program terminates.
 *
 * The error carries the code and, when the check gives one, the message;
 * the origin is the check's `static const` location.
 */

#ifndef LH_EXCEPTION_H
#define LH_EXCEPTION_H

#include <lh/compiler/extern/c.h>
#include <lh/exception/origin.h>
#include <lh/runtime/error.h>
#include <lh/size.h>
#include <lh/str/ptr.h>

/**
 * @struct lh_exception
 * @typedef lh_exception_t
 * @brief Error of a failed runtime check plus its origin.
 */
typedef struct lh_exception
{
    lh_runtime_error_t error;            /**< Code and message (empty when none is given). */
    const lh_exception_origin_t *origin; /**< Where the check sits; ::lh_null at REPORT_NONE. */
} lh_exception_t;                        /**< Typedef for struct ::lh_exception. */

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The error (code and message) of @p self.
 */
const lh_runtime_error_t *
lh_exception_get_error(const lh_exception_t *self);

/**
 * @brief The error code of @p self — shorthand for
 *        ::lh_runtime_error_get_code on ::lh_exception_get_error.
 */
lh_runtime_error_code_t
lh_exception_get_code(const lh_exception_t *self);

/**
 * @brief Where the failed check sits, or ::lh_null when checks report no
 *        origin (::LH_RUNTIME_CHECK_REPORT_NONE).
 */
const lh_exception_origin_t *
lh_exception_get_origin(const lh_exception_t *self);

/* ── text ────────────────────────────────────────────────────────────────── */

/**
 * @brief Write @p self as the report the default check handler prints.
 *
 * One line per part the exception carries — lines whose data the report
 * level dropped are left out:
 * @code{.unparsed}
 * lh: runtime check failed: error <code>[, <message>]
 *   condition: <condition>
 *   at <file>:<line>[ in <function>]
 * @endcode
 * Every line ends in `\n`. Needs no C library, so a host can route the
 * report anywhere — a server log, a UART, a debugger — from its own handler
 * (::lh_runtime_check_set).
 *
 * All-or-nothing, like ::lh_str_ptr_format_text: when @p str_size cannot
 * hold the whole report nothing is written and 0 is returned. No NUL
 * terminator is written; ::lh_exception_format_size gives the exact size.
 *
 * Example usage:
 * @code{.c}
 * static void
 * host_handler(const lh_exception_t *exception)
 * {
 *     char text[512];
 *     lh_usize_t n = lh_exception_format(exception, text, sizeof(text) - 1);
 *     text[n] = '\0';
 *     server_log(text);
 * }
 * @endcode
 *
 * @param self     Exception to describe.
 * @param str      Destination buffer.
 * @param str_size Capacity of @p str in characters.
 * @return Characters written, or 0 if @p str_size was too small.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self or @p str is ::lh_null.
 */
lh_usize_t
lh_exception_format(const lh_exception_t *self, lh_str_ptr str, lh_usize_t str_size);

/**
 * @brief Characters ::lh_exception_format writes for @p self (no terminator).
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
lh_usize_t
lh_exception_format_size(const lh_exception_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_EXCEPTION_H */
