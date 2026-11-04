/**
 * @file cb.h
 * @brief Pointer type for the work of an ::lh_exec_task_t.
 */

#ifndef LH_EXEC_TASK_WORK_CB_H
#define LH_EXEC_TASK_WORK_CB_H

#include <lh/core/util/ptr.h>
#include <lh/exec/task/work/fn.h>

/**
 * @def lh_exec_task_work_cb
 * @brief Pointer to ::lh_exec_task_work_fn.
 */
#define lh_exec_task_work_cb lh_ptr_of(lh_exec_task_work_fn)

#endif /* LH_EXEC_TASK_WORK_CB_H */
