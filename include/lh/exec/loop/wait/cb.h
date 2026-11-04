/**
 * @file cb.h
 * @brief ::lh_exec_loop_wait_cb — pointer to an ::lh_exec_loop_wait_fn.
 */

#ifndef LH_EXEC_LOOP_WAIT_CB_H
#define LH_EXEC_LOOP_WAIT_CB_H

#include <lh/exec/loop/wait/fn.h>
#include <lh/core/util/ptr.h>

/** @brief Pointer to an ::lh_exec_loop_wait_fn. */
#define lh_exec_loop_wait_cb lh_ptr_of(lh_exec_loop_wait_fn)

#endif /* LH_EXEC_LOOP_WAIT_CB_H */
