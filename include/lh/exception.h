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

LH_COMPILER_EXTERN_C_END

#endif /* LH_EXCEPTION_H */
