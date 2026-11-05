/**
 * @file cb.h
 * @brief Pointer type for what an executor thread opens when it starts.
 */

#ifndef LH_RAIL_CONTEXT_OPEN_CB_H
#define LH_RAIL_CONTEXT_OPEN_CB_H

#include <lh/core/util/ptr.h>
#include <lh/rail/context/open/fn.h>

/**
 * @def lh_rail_context_open_cb
 * @brief Pointer to ::lh_rail_context_open_fn.
 */
#define lh_rail_context_open_cb lh_ptr_of(lh_rail_context_open_fn)

#endif /* LH_RAIL_CONTEXT_OPEN_CB_H */
