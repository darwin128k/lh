/**
 * @file cb.h
 * @brief Pointer type for the work of an ::lh_rail_task_t.
 */

#ifndef LH_RAIL_TASK_WORK_CB_H
#define LH_RAIL_TASK_WORK_CB_H

#include <lh/core/util/ptr.h>
#include <lh/rail/task/work/fn.h>

/**
 * @def lh_rail_task_work_cb
 * @brief Pointer to ::lh_rail_task_work_fn.
 */
#define lh_rail_task_work_cb lh_ptr_of(lh_rail_task_work_fn)

#endif /* LH_RAIL_TASK_WORK_CB_H */
