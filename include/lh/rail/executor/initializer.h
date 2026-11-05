/**
 * @file initializer.h
 * @brief Brace-enclosed initializer for ::lh_rail_executor_t.
 */

#ifndef LH_RAIL_EXECUTOR_INITIALIZER_H
#define LH_RAIL_EXECUTOR_INITIALIZER_H

#include <lh/rail/executor/accept/cb.h>

/**
 * @def lh_rail_executor_initializer(accept_fn, context)
 * @brief Produces a brace-enclosed initializer for ::lh_rail_executor_t.
 *
 * @param accept_fn ::lh_rail_executor_accept_fn (must not be ::lh_null).
 * @param context   The state the call receives (may be ::lh_null).
 */
#define lh_rail_executor_initializer(accept_fn, context)                                           \
    lh_initializer(lh_cast_static(lh_rail_executor_accept_cb, accept_fn), context)

#endif /* LH_RAIL_EXECUTOR_INITIALIZER_H */
