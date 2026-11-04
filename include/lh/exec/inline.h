/**
 * @file inline.h
 * @brief The inline executor: a task's work runs at once, on the submitter.
 *
 * Synchronous execution is not a second mechanism in `lh/exec`: it is this
 * executor. ::lh_exec_executor_try_submit through it runs `work` on the
 * submitting thread before it returns (with `local` = ::lh_null — there is no
 * thread context), then posts the task to its mailbox. `complete` and `destroy`
 * still run only when the owner dispatches, exactly as on a thread executor, so
 * the code that submits behaves the same on both. Tests use it to run task logic
 * without threads or timing; a build without threads uses it in production.
 *
 * It never refuses a task (only the mailbox can be full) and holds no state, so
 * one shared record serves every caller: ::lh_exec_inline_get_executor.
 */

#ifndef LH_EXEC_INLINE_H
#define LH_EXEC_INLINE_H

#include <lh/exec/executor.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The inline executor's accept call (an ::lh_exec_executor_accept_fn).
 *
 * Runs the work of @p task with `local` = ::lh_null, then posts @p task to its
 * mailbox.
 *
 * @param context Unused (the inline executor has no state).
 * @param task    Task holding a place in its mailbox
 *                (::lh_exec_task_get_mailbox is set); must not be null.
 * @return Always ::lh_bool_true.
 */
lh_bool_t
lh_exec_inline_try_accept(lh_ptr context, lh_exec_task_t *task);

/**
 * @brief The inline executor.
 * @return One shared, read-only record, valid for the whole program.
 */
const lh_exec_executor_t *
lh_exec_inline_get_executor(void);

LH_COMPILER_EXTERN_C_END

#endif /* LH_EXEC_INLINE_H */
