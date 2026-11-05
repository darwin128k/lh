/**
 * @file fn.h
 * @brief How an executor takes a task: the one call every kind implements.
 */

#ifndef LH_RAIL_EXECUTOR_ACCEPT_FN_H
#define LH_RAIL_EXECUTOR_ACCEPT_FN_H

#include <lh/rail/task.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @typedef lh_rail_executor_accept_fn
 * @brief Function type `lh_bool_t (lh_ptr context, lh_rail_task_t *task)` —
 *        an executor taking a task.
 *
 * @p context is the executor's own state (::lh_rail_executor_get_context).
 * @p task already holds a place in its mailbox
 * (::lh_rail_task_get_mailbox). On ::lh_bool_true the executor owns the task:
 * it runs `work` once and then posts the task to that mailbox. On
 * ::lh_bool_false (full, stopping) it has not touched the task, and
 * ::lh_rail_executor_try_submit gives the place back. It must not block the
 * caller. Not a pointer type by itself; use ::lh_rail_executor_accept_cb.
 */
typedef lh_bool_t(lh_rail_executor_accept_fn)(lh_ptr context, lh_rail_task_t *task);

LH_COMPILER_EXTERN_C_END

#endif /* LH_RAIL_EXECUTOR_ACCEPT_FN_H */
