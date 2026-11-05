/**
 * @file cb.h
 * @brief ::lh_rail_base_sleep_cb — pointer to an ::lh_rail_base_sleep_fn.
 */

#ifndef LH_RAIL_BASE_SLEEP_CB_H
#define LH_RAIL_BASE_SLEEP_CB_H

#include <lh/rail/base/sleep/fn.h>
#include <lh/core/util/ptr.h>

/** @brief Pointer to an ::lh_rail_base_sleep_fn. */
#define lh_rail_base_sleep_cb lh_ptr_of(lh_rail_base_sleep_fn)

#endif /* LH_RAIL_BASE_SLEEP_CB_H */
