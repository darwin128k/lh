/**
 * @file fn.h
 * @brief Callable signature for the handler of a failed runtime check.
 */

#ifndef LH_RUNTIME_CHECK_FAIL_FN_H
#define LH_RUNTIME_CHECK_FAIL_FN_H

#include <lh/exception.h>

/**
 * @typedef lh_runtime_check_fail_fn
 * @brief Function type `void(const lh_exception_t *)` called when a runtime
 *        check fails.
 *
 * @p exception holds the error (code at every report level, message unless
 * ::LH_RUNTIME_CHECK_REPORT_NONE) and the origin (::lh_null at
 * ::LH_RUNTIME_CHECK_REPORT_NONE); it lives only for the call, so copy what
 * you keep. The handler may log, reset the board, or stop; if it returns,
 * ::lh_runtime_terminate runs next.
 */
typedef void(lh_runtime_check_fail_fn)(const lh_exception_t *exception);

#endif /* LH_RUNTIME_CHECK_FAIL_FN_H */
