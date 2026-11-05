/**
 * @file cb.h
 * @brief ::lh_rail_base_now_cb — pointer to an ::lh_rail_base_now_fn.
 */

#ifndef LH_RAIL_BASE_NOW_CB_H
#define LH_RAIL_BASE_NOW_CB_H

#include <lh/rail/base/now/fn.h>
#include <lh/core/util/ptr.h>

/** @brief Pointer to an ::lh_rail_base_now_fn. */
#define lh_rail_base_now_cb lh_ptr_of(lh_rail_base_now_fn)

#endif /* LH_RAIL_BASE_NOW_CB_H */
