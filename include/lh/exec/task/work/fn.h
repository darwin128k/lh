/**
 * @file fn.h
 * @brief The work of an ::lh_exec_task_t.
 */

#ifndef LH_EXEC_TASK_WORK_FN_H
#define LH_EXEC_TASK_WORK_FN_H

#include <lh/core/ptr.h>
#include <lh/core/compiler/extern/c.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @typedef lh_exec_task_work_fn
 * @brief Function type `void (lh_ptr user, lh_ptr local)` — what a task does.
 *
 * Runs once, on the executor's thread (on the submitting thread for the inline
 * executor). @p user is the task's own pointer; @p local is the state of the
 * thread that runs it — what its thread context opened (a database connection,
 * say), or ::lh_null where there is none. Not a pointer type by itself; use
 * ::lh_exec_task_work_cb for a pointer.
 */
typedef lh_void(lh_exec_task_work_fn)(lh_ptr user, lh_ptr local);

LH_COMPILER_EXTERN_C_END

#endif /* LH_EXEC_TASK_WORK_FN_H */
