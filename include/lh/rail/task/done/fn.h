/**
 * @file fn.h
 * @brief What runs after the work of an ::lh_rail_task_t: complete and destroy.
 */

#ifndef LH_RAIL_TASK_DONE_FN_H
#define LH_RAIL_TASK_DONE_FN_H

#include <lh/core/ptr.h>
#include <lh/core/compiler/extern/c.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @typedef lh_rail_task_done_fn
 * @brief Function type `void (lh_ptr user)` — a task's complete or destroy.
 *
 * `complete` runs on the owner's thread when the owner dispatches the finished
 * task; `destroy` runs once for every accepted task, after `complete` or
 * instead of it when the task is cancelled, and is where the caller frees what
 * the task holds — the task record itself included. Not a pointer type by
 * itself; use ::lh_rail_task_done_cb for a pointer.
 */
typedef lh_void(lh_rail_task_done_fn)(lh_ptr user);

LH_COMPILER_EXTERN_C_END

#endif /* LH_RAIL_TASK_DONE_FN_H */
