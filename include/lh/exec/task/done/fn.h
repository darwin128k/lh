/**
 * @file fn.h
 * @brief What runs after the work of an ::lh_exec_task_t: complete and destroy.
 */

#ifndef LH_EXEC_TASK_DONE_FN_H
#define LH_EXEC_TASK_DONE_FN_H

#include <lh/core/ptr.h>

/**
 * @typedef lh_exec_task_done_fn
 * @brief Function type `void (lh_ptr user)` — a task's complete or destroy.
 *
 * `complete` runs on the owner's thread when the owner dispatches the finished
 * task; `destroy` runs once for every accepted task, after `complete` or
 * instead of it when the task is cancelled, and is where the caller frees what
 * the task holds — the task record itself included. Not a pointer type by
 * itself; use ::lh_exec_task_done_cb for a pointer.
 */
typedef lh_void(lh_exec_task_done_fn)(lh_ptr user);

#endif /* LH_EXEC_TASK_DONE_FN_H */
