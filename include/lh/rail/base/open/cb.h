/**
 * @file cb.h
 * @brief ::lh_rail_base_open_cb — pointer to an ::lh_rail_base_open_fn.
 */

#ifndef LH_RAIL_BASE_OPEN_CB_H
#define LH_RAIL_BASE_OPEN_CB_H

#include <lh/rail/base/open/fn.h>
#include <lh/core/util/ptr.h>

/** @brief Pointer to an ::lh_rail_base_open_fn. */
#define lh_rail_base_open_cb lh_ptr_of(lh_rail_base_open_fn)

#endif /* LH_RAIL_BASE_OPEN_CB_H */
