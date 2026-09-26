/**
 * @file cb.h
 * @brief Pointer type for the handler of a failed runtime check.
 */

#ifndef LH_RUNTIME_CHECK_FAIL_CB_H
#define LH_RUNTIME_CHECK_FAIL_CB_H

#include <lh/runtime/check/fail/fn.h>
#include <lh/util/ptr.h>

/**
 * @def lh_runtime_check_fail_cb
 * @brief Pointer to ::lh_runtime_check_fail_fn.
 */
#define lh_runtime_check_fail_cb lh_ptr_of(lh_runtime_check_fail_fn)

#endif /* LH_RUNTIME_CHECK_FAIL_CB_H */
