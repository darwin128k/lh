/**
 * @file cb.h
 * @brief ::lh_critical_enter_cb — pointer to an ::lh_critical_enter_fn.
 */

#ifndef LH_CRITICAL_ENTER_CB_H
#define LH_CRITICAL_ENTER_CB_H

#include <lh/core/critical/enter/fn.h>
#include <lh/core/util/ptr.h>

/** @brief Pointer to an ::lh_critical_enter_fn. */
#define lh_critical_enter_cb lh_ptr_of(lh_critical_enter_fn)

#endif /* LH_CRITICAL_ENTER_CB_H */
