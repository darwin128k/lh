/**
 * @file thread.h
 * @brief ::lh_exec_thread_t — an executor with one thread, tasks in order.
 *
 * The thread executor owns one OS thread. Tasks submitted through its executor
 * record (::lh_exec_thread_get_executor) from any thread run on that thread one
 * at a time, in the order they were accepted; after each `work` the task goes to
 * its mailbox, and the owner dispatches `complete` and `destroy` on its own
 * thread. A context (lh/exec/context.h) gives the thread state of its own:
 * opened before the first task, handed to every `work` as `local`, closed after
 * the last one, all on this thread.
 *
 * Lifecycle: ::lh_exec_thread_try_start fills a caller-owned record and starts
 * the thread; the record must stay where it is until ::lh_exec_thread_destroy.
 * ::lh_exec_thread_try_stop asks it to stop (lh/exec/stop/kind.h) and waits a
 * bounded time for it to end; ::lh_exec_thread_destroy waits for the end without
 * a bound, joins the thread and releases the OS objects. A thread stuck in a
 * call cannot be made to return: give `work` bounded calls, stop with a timeout,
 * and destroy once the stop reported the end.
 *
 * Submission takes the executor's mutex for a moment (to link the task and wake
 * the thread); it never waits for a task to run. This executor is built on the
 * OS thread, mutex and condition beside `lh/os`, as docs/design/execution.md
 * decides; the loop of a later step replaces the mutex wait.
 */

#ifndef LH_EXEC_THREAD_H
#define LH_EXEC_THREAD_H

#include <lh/exec/context.h>
#include <lh/exec/executor.h>
#include <lh/exec/stop/kind.h>
#include <lh/exec/thread/fields.h>
#include <lh/os/condition.h>
#include <lh/os/thread.h>

/**
 * @brief One OS thread running tasks in order, with its own state.
 */
struct lh_exec_thread
{
    /** Members: ::lh_exec_thread_fields. */
    lh_exec_thread_fields(lh_exec_executor_t, lh_exec_queue_t, lh_exec_context_t,
                          lh_exec_stop_kind_t, lh_bool_t, lh_os_mutex_t, lh_os_condition_t,
                          lh_os_thread_t);
};

/** @copybrief lh_exec_thread */
typedef struct lh_exec_thread lh_exec_thread_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The executor record of @p self, for submitting (level 1).
 * @param self Started thread executor; must not be null.
 * @return The record; valid until ::lh_exec_thread_destroy.
 */
const lh_exec_executor_t *
lh_exec_thread_get_executor(const lh_exec_thread_t *self);

/**
 * @brief The executor record of @p self, for filling (level 1).
 * @param self Thread executor; must not be null.
 * @return The record.
 */
lh_exec_executor_t *
lh_exec_thread_get_executor_mut(lh_exec_thread_t *self);

/**
 * @brief The accepted tasks of @p self not yet started (level 1).
 * @param self Thread executor; must not be null.
 * @return The queue.
 */
lh_exec_queue_t *
lh_exec_thread_get_pending_mut(lh_exec_thread_t *self);

/**
 * @brief What the thread of @p self opens and closes (level 1).
 * @param self Thread executor; must not be null.
 * @return The context copy, read-only.
 */
const lh_exec_context_t *
lh_exec_thread_get_context(const lh_exec_thread_t *self);

/**
 * @brief What the thread of @p self opens and closes, for filling (level 1).
 * @param self Thread executor; must not be null.
 * @return The context copy.
 */
lh_exec_context_t *
lh_exec_thread_get_context_mut(lh_exec_thread_t *self);

/**
 * @brief What the thread of @p self opened (level 1).
 * @param self Thread executor; must not be null. The executor's thread only.
 * @return The `local` every `work` receives.
 */
lh_ptr
lh_exec_thread_get_local(const lh_exec_thread_t *self);

/**
 * @brief Store what the thread of @p self opened (level 1).
 * @param self  Thread executor; must not be null. The executor's thread only.
 * @param local The opened state.
 */
lh_void
lh_exec_thread_set_local(lh_exec_thread_t *self, lh_ptr local);

/**
 * @brief Whether and how @p self stops (level 1).
 * @param self Thread executor; must not be null. Under its mutex.
 * @return ::lh_exec_stop_kind_none while it runs, else the stop asked for.
 */
lh_exec_stop_kind_t
lh_exec_thread_get_stop(const lh_exec_thread_t *self);

/**
 * @brief Store whether and how @p self stops (level 1).
 * @param self Thread executor; must not be null. Under its mutex.
 * @param stop The stop kind.
 */
lh_void
lh_exec_thread_set_stop(lh_exec_thread_t *self, lh_exec_stop_kind_t stop);

/**
 * @brief Whether the thread of @p self has ended its work (level 1).
 * @param self Thread executor; must not be null. Under its mutex.
 * @return ::lh_bool_true once the context is closed and nothing more runs.
 */
lh_bool_t
lh_exec_thread_is_ended(const lh_exec_thread_t *self);

/**
 * @brief Store whether the thread of @p self has ended its work (level 1).
 * @param self  Thread executor; must not be null. Under its mutex.
 * @param ended The new value.
 */
lh_void
lh_exec_thread_set_ended(lh_exec_thread_t *self, lh_bool_t ended);

/**
 * @brief The mutex of @p self (level 1).
 * @param self Thread executor; must not be null.
 * @return The mutex.
 */
lh_os_mutex_t *
lh_exec_thread_get_mutex_mut(lh_exec_thread_t *self);

/**
 * @brief The condition the thread of @p self waits on for work (level 1).
 * @param self Thread executor; must not be null.
 * @return The condition.
 */
lh_os_condition_t *
lh_exec_thread_get_wake_mut(lh_exec_thread_t *self);

/**
 * @brief The condition a stop of @p self waits on for the end (level 1).
 * @param self Thread executor; must not be null.
 * @return The condition.
 */
lh_os_condition_t *
lh_exec_thread_get_end_mut(lh_exec_thread_t *self);

/**
 * @brief The OS thread of @p self (level 1).
 * @param self Thread executor; must not be null.
 * @return The thread object.
 */
lh_os_thread_t *
lh_exec_thread_get_thread_mut(lh_exec_thread_t *self);

/**
 * @brief Wake whoever waits on @p condition; a failure terminates.
 * @param condition Open condition; must not be null.
 */
lh_void
lh_exec_thread_broadcast(lh_os_condition_t *condition);

/**
 * @brief Take the mutex of @p self; a failure terminates.
 * @param self Started thread executor; must not be null.
 */
lh_void
lh_exec_thread_lock(lh_exec_thread_t *self);

/**
 * @brief Give the mutex of @p self back; a failure terminates.
 * @param self Thread executor whose mutex the caller holds; must not be null.
 */
lh_void
lh_exec_thread_unlock(lh_exec_thread_t *self);

/**
 * @brief Close the mutex and both conditions of @p self; a failure terminates.
 *
 * An object that never opened is skipped, so it serves a start that failed
 * halfway as well as ::lh_exec_thread_destroy. The OS thread must not run.
 *
 * @param self Thread executor whose thread is joined or never started; must
 *             not be null.
 */
lh_void
lh_exec_thread_release(lh_exec_thread_t *self);

/**
 * @brief Run @p task on the thread of @p self and post it to its mailbox.
 *
 * The work gets the thread's `local`; a cancelled task does not run and is
 * posted all the same.
 *
 * @param self Started thread executor; must not be null. Its thread only.
 * @param task Task from ::lh_exec_thread_wait_task; must not be null.
 */
lh_void
lh_exec_thread_run_task(lh_exec_thread_t *self, lh_exec_task_t *task);

/**
 * @brief The thread executor's accept call (an ::lh_exec_executor_accept_fn).
 *
 * Links @p task behind the accepted ones and wakes the thread; refuses once a
 * stop was asked for. Any thread.
 *
 * @param context The ::lh_exec_thread_t; must not be null.
 * @param task    Task holding a place in its mailbox; must not be null.
 * @return ::lh_bool_true when the task was linked; ::lh_bool_false when the
 *         executor is stopping (the task is untouched).
 */
lh_bool_t
lh_exec_thread_try_accept(lh_ptr context, lh_exec_task_t *task);

/**
 * @brief Wait for the next task of @p self; the executor's thread only.
 *
 * Sleeps while nothing is accepted and no stop was asked for. A task taken
 * after a cancelling stop comes back marked cancelled.
 *
 * @param self Started thread executor; must not be null.
 * @return The next task, now the thread's; ::lh_null when the executor stops
 *         and nothing is left.
 */
lh_exec_task_t *
lh_exec_thread_wait_task(lh_exec_thread_t *self);

/**
 * @brief Fill @p self and start its thread.
 *
 * @param self    Caller-owned record; must not be null. It must not move until
 *                ::lh_exec_thread_destroy.
 * @param context What the thread opens and closes, copied (must not be null;
 *                pass the empty context for none).
 * @return ::lh_bool_true when the thread runs; ::lh_bool_false when an OS object
 *         could not be made (nothing is left to release).
 */
lh_bool_t
lh_exec_thread_try_start(lh_exec_thread_t *self, const lh_exec_context_t *context);

/**
 * @brief Ask @p self to stop and wait up to @p timeout_ms for its end.
 *
 * New tasks are refused from now on. A drain may be turned into a cancel by a
 * later call, never the other way round. Call it again to wait longer.
 *
 * @param self       Started thread executor; must not be null.
 * @param kind       ::lh_exec_stop_kind_drain or ::lh_exec_stop_kind_cancel.
 * @param timeout_ms Longest wait for the end, in milliseconds (0: do not wait).
 * @return ::lh_bool_true when the thread has ended: every accepted task is in
 *         its mailbox and the context is closed; ::lh_bool_false when the time
 *         ran out first (the thread still runs a task).
 */
lh_bool_t
lh_exec_thread_try_stop(lh_exec_thread_t *self, lh_exec_stop_kind_t kind, lh_u32_t timeout_ms);

/**
 * @brief Wait for the end of @p self, join its thread and release its OS
 *        objects.
 *
 * Without an earlier stop it drains. It waits without a bound: when a task may
 * hang, stop with a timeout first. Afterwards the record may be reused or freed.
 *
 * @param self Started thread executor; must not be null.
 */
lh_void
lh_exec_thread_destroy(lh_exec_thread_t *self);

/**
 * @brief The body of the executor's thread (an ::lh_os_thread_fn).
 *
 * Opens the context, runs tasks until the stop leaves none, closes the context,
 * then marks the executor ended and wakes the stop that waits for it.
 *
 * @param user The ::lh_exec_thread_t.
 */
lh_void
lh_exec_thread_run(lh_ptr user);

LH_COMPILER_EXTERN_C_END

#endif /* LH_EXEC_THREAD_H */
