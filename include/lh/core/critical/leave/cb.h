/**
 * @file cb.h
 * @brief ::lh_critical_leave_cb — pointer to an ::lh_critical_leave_fn.
 */

#ifndef LH_CRITICAL_LEAVE_CB_H
#define LH_CRITICAL_LEAVE_CB_H

#include <lh/core/critical/leave/fn.h>
#include <lh/core/util/ptr.h>

/** @brief Pointer to an ::lh_critical_leave_fn. */
#define lh_critical_leave_cb lh_ptr_of(lh_critical_leave_fn)

#endif /* LH_CRITICAL_LEAVE_CB_H */
