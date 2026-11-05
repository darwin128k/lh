/**
 * @file cb.h
 * @brief Pointer type for how an executor takes a task.
 */

#ifndef LH_RAIL_EXECUTOR_ACCEPT_CB_H
#define LH_RAIL_EXECUTOR_ACCEPT_CB_H

#include <lh/rail/executor/accept/fn.h>

/**
 * @def lh_rail_executor_accept_cb
 * @brief Pointer to ::lh_rail_executor_accept_fn.
 */
#define lh_rail_executor_accept_cb lh_ptr_of(lh_rail_executor_accept_fn)

#endif /* LH_RAIL_EXECUTOR_ACCEPT_CB_H */
