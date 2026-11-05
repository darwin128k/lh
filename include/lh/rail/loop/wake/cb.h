/**
 * @file cb.h
 * @brief ::lh_rail_loop_wake_cb — pointer to an ::lh_rail_loop_wake_fn.
 */

#ifndef LH_RAIL_LOOP_WAKE_CB_H
#define LH_RAIL_LOOP_WAKE_CB_H

#include <lh/rail/loop/wake/fn.h>
#include <lh/core/util/ptr.h>

/** @brief Pointer to an ::lh_rail_loop_wake_fn. */
#define lh_rail_loop_wake_cb lh_ptr_of(lh_rail_loop_wake_fn)

#endif /* LH_RAIL_LOOP_WAKE_CB_H */
