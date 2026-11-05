/**
 * @file cb.h
 * @brief ::lh_rail_loop_release_cb — pointer to an ::lh_rail_loop_release_fn.
 */

#ifndef LH_RAIL_LOOP_RELEASE_CB_H
#define LH_RAIL_LOOP_RELEASE_CB_H

#include <lh/rail/loop/release/fn.h>
#include <lh/core/util/ptr.h>

/** @brief Pointer to an ::lh_rail_loop_release_fn. */
#define lh_rail_loop_release_cb lh_ptr_of(lh_rail_loop_release_fn)

#endif /* LH_RAIL_LOOP_RELEASE_CB_H */
