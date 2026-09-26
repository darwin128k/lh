/**
 * @file fn.h
 * @brief Callable signature for the handler of a failed runtime check.
 */

#ifndef LH_RUNTIME_CHECK_FAIL_FN_H
#define LH_RUNTIME_CHECK_FAIL_FN_H

#include <lh/runtime/check/site.h>

/**
 * @typedef lh_runtime_check_fail_fn
 * @brief Function type `void(const lh_runtime_check_site_t *)` called when
 *        a runtime check fails.
 *
 * @p site is ::lh_null when checks report nothing
 * (::LH_RUNTIME_CHECK_REPORT_NONE). The handler may log, reset the board,
 * or stop; if it returns, ::lh_runtime_terminate runs next.
 */
typedef void(lh_runtime_check_fail_fn)(const lh_runtime_check_site_t *site);

#endif /* LH_RUNTIME_CHECK_FAIL_FN_H */
