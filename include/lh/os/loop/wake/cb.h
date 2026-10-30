/**
 * @file cb.h
 * @brief ::lh_os_loop_wake_cb — pointer to an ::lh_os_loop_wake_fn.
 */

#ifndef LH_OS_LOOP_WAKE_CB_H
#define LH_OS_LOOP_WAKE_CB_H

#include <lh/os/loop/wake/fn.h>
#include <lh/util/ptr.h>

/** @brief Pointer to an ::lh_os_loop_wake_fn. */
#define lh_os_loop_wake_cb lh_ptr_of(lh_os_loop_wake_fn)

#endif /* LH_OS_LOOP_WAKE_CB_H */
