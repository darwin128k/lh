/**
 * @file cb.h
 * @brief ::lh_exec_loop_wake_cb — pointer to an ::lh_exec_loop_wake_fn.
 */

#ifndef LH_EXEC_LOOP_WAKE_CB_H
#define LH_EXEC_LOOP_WAKE_CB_H

#include <lh/exec/loop/wake/fn.h>
#include <lh/core/util/ptr.h>

/** @brief Pointer to an ::lh_exec_loop_wake_fn. */
#define lh_exec_loop_wake_cb lh_ptr_of(lh_exec_loop_wake_fn)

#endif /* LH_EXEC_LOOP_WAKE_CB_H */
