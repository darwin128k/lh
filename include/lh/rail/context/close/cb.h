/**
 * @file cb.h
 * @brief Pointer type for what an executor thread closes when it stops.
 */

#ifndef LH_RAIL_CONTEXT_CLOSE_CB_H
#define LH_RAIL_CONTEXT_CLOSE_CB_H

#include <lh/core/util/ptr.h>
#include <lh/rail/context/close/fn.h>

/**
 * @def lh_rail_context_close_cb
 * @brief Pointer to ::lh_rail_context_close_fn.
 */
#define lh_rail_context_close_cb lh_ptr_of(lh_rail_context_close_fn)

#endif /* LH_RAIL_CONTEXT_CLOSE_CB_H */
