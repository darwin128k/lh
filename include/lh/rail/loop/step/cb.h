/**
 * @file cb.h
 * @brief ::lh_rail_loop_step_cb — pointer to an ::lh_rail_loop_step_fn.
 */

#ifndef LH_RAIL_LOOP_STEP_CB_H
#define LH_RAIL_LOOP_STEP_CB_H

#include <lh/rail/loop/step/fn.h>
#include <lh/core/util/ptr.h>

/** @brief Pointer to an ::lh_rail_loop_step_fn. */
#define lh_rail_loop_step_cb lh_ptr_of(lh_rail_loop_step_fn)

#endif /* LH_RAIL_LOOP_STEP_CB_H */
