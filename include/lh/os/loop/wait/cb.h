/**
 * @file cb.h
 * @brief ::lh_os_loop_wait_cb — pointer to an ::lh_os_loop_wait_fn.
 */

#ifndef LH_OS_LOOP_WAIT_CB_H
#define LH_OS_LOOP_WAIT_CB_H

#include <lh/os/loop/wait/fn.h>
#include <lh/util/ptr.h>

/** @brief Pointer to an ::lh_os_loop_wait_fn. */
#define lh_os_loop_wait_cb lh_ptr_of(lh_os_loop_wait_fn)

#endif /* LH_OS_LOOP_WAIT_CB_H */
