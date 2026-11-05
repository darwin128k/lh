/**
 * @file cb.h
 * @brief ::lh_rail_base_run_cb — pointer to an ::lh_rail_base_run_fn.
 */

#ifndef LH_RAIL_BASE_RUN_CB_H
#define LH_RAIL_BASE_RUN_CB_H

#include <lh/rail/base/run/fn.h>
#include <lh/core/util/ptr.h>

/** @brief Pointer to an ::lh_rail_base_run_fn. */
#define lh_rail_base_run_cb lh_ptr_of(lh_rail_base_run_fn)

#endif /* LH_RAIL_BASE_RUN_CB_H */
