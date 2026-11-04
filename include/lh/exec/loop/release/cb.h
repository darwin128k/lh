/**
 * @file cb.h
 * @brief ::lh_exec_loop_release_cb — pointer to an ::lh_exec_loop_release_fn.
 */

#ifndef LH_EXEC_LOOP_RELEASE_CB_H
#define LH_EXEC_LOOP_RELEASE_CB_H

#include <lh/exec/loop/release/fn.h>
#include <lh/core/util/ptr.h>

/** @brief Pointer to an ::lh_exec_loop_release_fn. */
#define lh_exec_loop_release_cb lh_ptr_of(lh_exec_loop_release_fn)

#endif /* LH_EXEC_LOOP_RELEASE_CB_H */
