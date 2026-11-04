/**
 * @file task.h
 * @brief ::lh_exec_task_t — one piece of work handed to an executor.
 *
 * A task is what `lh/exec` moves between threads: what to do (`work`), what to
 * tell the owner when it is done (`complete`), how to clean up (`destroy`), and
 * the pointer all three receive (`user`). It is the one task type of LH's
 * execution part; every executor, queue and mailbox carries it.
 *
 * The record belongs to the caller: embedded in the caller's own object or
 * allocated by the caller — `lh/exec` allocates nothing per task. Once an
 * executor accepts it, the record must stay where it is and untouched until its
 * `destroy` runs; `destroy` is where the caller frees it. A record the executor
 * refused stays the caller's at once.
 *
 * The laws (docs/design/execution.md): `work` runs once, on the executor's
 * thread; `complete` runs on the owner's thread, only when the owner
 * dispatches; `destroy` runs exactly once for every accepted task — after
 * `complete`, or instead of it when the task is cancelled.
 *
 * The queue link (`next`) is `lh/exec`'s: queues write it from any thread
 * through the atomic brick, and nobody else touches it. The mailbox
 * (`mailbox`) is the owner's ::lh_exec_mailbox_t that delivers `complete`;
 * ::lh_exec_executor_try_submit sets it, and the executor posts the task there
 * after `work`. An executor that stops before the work ran marks the task
 * `cancelled` and posts it all the same, so its `destroy` too runs on the
 * owner's thread.
 */

#ifndef LH_EXEC_TASK_H
#define LH_EXEC_TASK_H

#include <lh/core/atomic/ptr.h>
#include <lh/core/bool.h>
#include <lh/exec/task/done/cb.h>
#include <lh/exec/task/fields.h>
#include <lh/exec/task/work/cb.h>

/* The mailbox is only pointed at here; lh/exec/mailbox.h defines it. */
struct lh_exec_mailbox;

/**
 * @brief One piece of work, its completion and its cleanup.
 */
struct lh_exec_task
{
    /** Members: ::lh_exec_task_fields. */
    lh_exec_task_fields(lh_exec_task_work_fn, lh_exec_task_done_fn, lh_ptr, lh_atomic_ptr_t,
                        struct lh_exec_mailbox, lh_bool_t);
};

/** @copybrief lh_exec_task */
typedef struct lh_exec_task lh_exec_task_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The work of @p self (level 1).
 * @param self Task; must not be null.
 * @return The work call.
 */
lh_exec_task_work_cb
lh_exec_task_get_work_cb(const lh_exec_task_t *self);

/**
 * @brief Store the work of @p self (level 1).
 * @param self    Task; must not be null.
 * @param work_cb The work call (must not be ::lh_null when the task is submitted).
 */
lh_void
lh_exec_task_set_work_cb(lh_exec_task_t *self, lh_exec_task_work_cb work_cb);

/**
 * @brief The complete call of @p self (level 1).
 * @param self Task; must not be null.
 * @return The complete call, or ::lh_null for none.
 */
lh_exec_task_done_cb
lh_exec_task_get_complete_cb(const lh_exec_task_t *self);

/**
 * @brief Store the complete call of @p self (level 1).
 * @param self        Task; must not be null.
 * @param complete_cb The complete call (may be ::lh_null).
 */
lh_void
lh_exec_task_set_complete_cb(lh_exec_task_t *self, lh_exec_task_done_cb complete_cb);

/**
 * @brief The destroy call of @p self (level 1).
 * @param self Task; must not be null.
 * @return The destroy call, or ::lh_null for none.
 */
lh_exec_task_done_cb
lh_exec_task_get_destroy_cb(const lh_exec_task_t *self);

/**
 * @brief Store the destroy call of @p self (level 1).
 * @param self       Task; must not be null.
 * @param destroy_cb The destroy call (may be ::lh_null).
 */
lh_void
lh_exec_task_set_destroy_cb(lh_exec_task_t *self, lh_exec_task_done_cb destroy_cb);

/**
 * @brief The pointer every call of @p self receives (level 1).
 * @param self Task; must not be null.
 * @return The user pointer (may be ::lh_null).
 */
lh_ptr
lh_exec_task_get_user(const lh_exec_task_t *self);

/**
 * @brief Store the pointer every call of @p self receives (level 1).
 * @param self Task; must not be null.
 * @param user The user pointer, borrowed (may be ::lh_null).
 */
lh_void
lh_exec_task_set_user(lh_exec_task_t *self, lh_ptr user);

/**
 * @brief The queue link of @p self (level 1).
 * @param self Task; must not be null.
 * @return The link, read-only; read it with the atomic brick.
 */
const lh_atomic_ptr_t *
lh_exec_task_get_next(const lh_exec_task_t *self);

/**
 * @brief The queue link of @p self, for change (level 1).
 * @param self Task; must not be null.
 * @return The link; write it with the atomic brick. Queues only.
 */
lh_atomic_ptr_t *
lh_exec_task_get_next_mut(lh_exec_task_t *self);

/**
 * @brief The mailbox that delivers the completion of @p self (level 1).
 * @param self Task; must not be null.
 * @return The mailbox, read-only, or ::lh_null before the task is submitted.
 */
const struct lh_exec_mailbox *
lh_exec_task_get_mailbox(const lh_exec_task_t *self);

/**
 * @brief The mailbox that delivers the completion of @p self, for posting
 *        (level 1).
 * @param self Task; must not be null.
 * @return The mailbox, or ::lh_null before the task is submitted.
 */
struct lh_exec_mailbox *
lh_exec_task_get_mailbox_mut(lh_exec_task_t *self);

/**
 * @brief Store the mailbox that delivers the completion of @p self (level 1).
 *
 * ::lh_exec_executor_try_submit calls it; a caller does not.
 *
 * @param self    Task; must not be null.
 * @param mailbox The owner's mailbox, borrowed (may be ::lh_null).
 */
lh_void
lh_exec_task_set_mailbox(lh_exec_task_t *self, struct lh_exec_mailbox *mailbox);

/**
 * @brief Whether @p self was cancelled before its work ran (level 1).
 * @param self Task; must not be null.
 * @return ::lh_bool_true when only `destroy` is to run at dispatch.
 */
lh_bool_t
lh_exec_task_is_cancelled(const lh_exec_task_t *self);

/**
 * @brief Mark @p self cancelled, or not (level 1).
 *
 * An executor calls it on the thread that owns the task at the time, before it
 * posts the task; a caller does not.
 *
 * @param self      Task; must not be null.
 * @param cancelled ::lh_bool_true when the work will not run.
 */
lh_void
lh_exec_task_set_cancelled(lh_exec_task_t *self, lh_bool_t cancelled);

/**
 * @brief The task linked after @p self, read with acquire order.
 *
 * What the producer that linked it did before linking is visible afterwards.
 *
 * @param self Task in a queue; must not be null.
 * @return The next task, or ::lh_null when none is linked yet.
 */
lh_exec_task_t *
lh_exec_task_get_next_task(const lh_exec_task_t *self);

/**
 * @brief Fill @p self: its three calls, its pointer, an empty queue link, no
 *        mailbox, not cancelled.
 *
 * For a task no other thread can reach yet: the link is cleared with relaxed
 * order, and the submission that publishes the task orders everything.
 *
 * @param self        Task; must not be null.
 * @param work_cb     The work (must not be ::lh_null when the task is submitted).
 * @param complete_cb The complete call (may be ::lh_null).
 * @param destroy_cb  The destroy call (may be ::lh_null).
 * @param user        The pointer every call receives, borrowed (may be ::lh_null).
 */
lh_void
lh_exec_task_init(lh_exec_task_t *self, lh_exec_task_work_cb work_cb,
                  lh_exec_task_done_cb complete_cb, lh_exec_task_done_cb destroy_cb, lh_ptr user);

/**
 * @brief Run the work of @p self with its pointer and @p local.
 * @param self  Task with a work call; must not be null.
 * @param local The running thread's context state (may be ::lh_null).
 */
lh_void
lh_exec_task_run_work(const lh_exec_task_t *self, lh_ptr local);

/**
 * @brief Run the complete call of @p self, if it has one.
 * @param self Task; must not be null.
 */
lh_void
lh_exec_task_run_complete(const lh_exec_task_t *self);

/**
 * @brief Run the destroy call of @p self, if it has one.
 *
 * After it the record may be gone: do not touch @p self again.
 *
 * @param self Task; must not be null.
 */
lh_void
lh_exec_task_run_destroy(const lh_exec_task_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_EXEC_TASK_H */
