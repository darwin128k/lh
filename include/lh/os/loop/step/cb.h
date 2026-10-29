/**
 * @file cb.h
 * @brief ::lh_os_loop_step_cb — pointer to an ::lh_os_loop_step_fn.
 */

#ifndef LH_OS_LOOP_STEP_CB_H
#define LH_OS_LOOP_STEP_CB_H

#include <lh/os/loop/step/fn.h>
#include <lh/util/ptr.h>

/** @brief Pointer to an ::lh_os_loop_step_fn. */
#define lh_os_loop_step_cb lh_ptr_of(lh_os_loop_step_fn)

#endif /* LH_OS_LOOP_STEP_CB_H */
