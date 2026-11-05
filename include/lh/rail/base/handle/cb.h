/**
 * @file cb.h
 * @brief ::lh_rail_base_handle_cb — pointer to an ::lh_rail_base_handle_fn.
 */

#ifndef LH_RAIL_BASE_HANDLE_CB_H
#define LH_RAIL_BASE_HANDLE_CB_H

#include <lh/rail/base/handle/fn.h>
#include <lh/core/util/ptr.h>

/** @brief Pointer to an ::lh_rail_base_handle_fn. */
#define lh_rail_base_handle_cb lh_ptr_of(lh_rail_base_handle_fn)

#endif /* LH_RAIL_BASE_HANDLE_CB_H */
