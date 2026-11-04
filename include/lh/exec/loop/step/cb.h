/**
 * @file cb.h
 * @brief ::lh_exec_loop_step_cb — pointer to an ::lh_exec_loop_step_fn.
 */

#ifndef LH_EXEC_LOOP_STEP_CB_H
#define LH_EXEC_LOOP_STEP_CB_H

#include <lh/exec/loop/step/fn.h>
#include <lh/core/util/ptr.h>

/** @brief Pointer to an ::lh_exec_loop_step_fn. */
#define lh_exec_loop_step_cb lh_ptr_of(lh_exec_loop_step_fn)

#endif /* LH_EXEC_LOOP_STEP_CB_H */
