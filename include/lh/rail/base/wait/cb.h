/**
 * @file cb.h
 * @brief ::lh_rail_base_wait_cb — pointer to an ::lh_rail_base_wait_fn.
 */

#ifndef LH_RAIL_BASE_WAIT_CB_H
#define LH_RAIL_BASE_WAIT_CB_H

#include <lh/rail/base/wait/fn.h>
#include <lh/core/util/ptr.h>

/** @brief Pointer to an ::lh_rail_base_wait_fn. */
#define lh_rail_base_wait_cb lh_ptr_of(lh_rail_base_wait_fn)

#endif /* LH_RAIL_BASE_WAIT_CB_H */
