/**
 * @file cb.h
 * @brief Pointer type for a task's complete and destroy.
 */

#ifndef LH_RAIL_TASK_DONE_CB_H
#define LH_RAIL_TASK_DONE_CB_H

#include <lh/core/util/ptr.h>
#include <lh/rail/task/done/fn.h>

/**
 * @def lh_rail_task_done_cb
 * @brief Pointer to ::lh_rail_task_done_fn.
 */
#define lh_rail_task_done_cb lh_ptr_of(lh_rail_task_done_fn)

#endif /* LH_RAIL_TASK_DONE_CB_H */
