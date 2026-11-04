/**
 * @file mailbox.h
 * @brief ::lh_exec_mailbox_t — where finished tasks wait for their owner.
 *
 * An owner — a game frame, a UI tick, any thread that wants results on itself —
 * keeps one mailbox. Every task submitted for that owner names it
 * (::lh_exec_executor_try_submit). When the task's `work` is done, wherever it
 * ran, the executor posts it here; the owner calls ::lh_exec_mailbox_dispatch on
 * its own thread, and only then do `complete` and `destroy` run. Nothing is
 * delivered behind the owner's back.
 *
 * The mailbox is bounded. Its capacity counts the tasks accepted for the owner
 * and not yet dispatched — queued, running and finished alike. A submission
 * takes a place first (::lh_exec_mailbox_try_reserve) and is refused at once
 * when none is free; dispatch gives the place back. So a slow owner cannot be
 * buried: the submitters are told "no" instead.
 *
 * The record is the owner's and allocates nothing: the finished tasks are
 * linked through themselves. It holds an ::lh_exec_queue_t, so it must not be
 * moved or copied after ::lh_exec_mailbox_init. It needs no teardown, but the
 * owner must not let it go while places are taken: drain it first (stop the
 * executors, then dispatch until ::lh_exec_mailbox_get_used_count is zero).
 */

#ifndef LH_EXEC_MAILBOX_H
#define LH_EXEC_MAILBOX_H

#include <lh/core/atomic/uint.h>
#include <lh/exec/mailbox/fields.h>
#include <lh/exec/queue.h>

/**
 * @brief The owner's bounded box of finished tasks.
 */
struct lh_exec_mailbox
{
    /** Members: ::lh_exec_mailbox_fields. */
    lh_exec_mailbox_fields(lh_uint_t, lh_atomic_uint_t, lh_exec_queue_t);
};

/** @copybrief lh_exec_mailbox */
typedef struct lh_exec_mailbox lh_exec_mailbox_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The capacity of @p self (level 1).
 * @param self Mailbox; must not be null.
 * @return How many tasks may be accepted and not yet dispatched.
 */
lh_uint_t
lh_exec_mailbox_get_capacity(const lh_exec_mailbox_t *self);

/**
 * @brief Store the capacity of @p self (level 1).
 *
 * Only before any thread can reach the mailbox: ::lh_exec_mailbox_init does it.
 *
 * @param self     Mailbox; must not be null.
 * @param capacity How many tasks may be accepted and not yet dispatched.
 */
lh_void
lh_exec_mailbox_set_capacity(lh_exec_mailbox_t *self, lh_uint_t capacity);

/**
 * @brief The count of taken places of @p self (level 1).
 * @param self Mailbox; must not be null.
 * @return The counter, read-only; read it with the atomic brick.
 */
const lh_atomic_uint_t *
lh_exec_mailbox_get_used(const lh_exec_mailbox_t *self);

/**
 * @brief The count of taken places of @p self, for change (level 1).
 * @param self Mailbox; must not be null.
 * @return The counter; change it with the atomic brick.
 */
lh_atomic_uint_t *
lh_exec_mailbox_get_used_mut(lh_exec_mailbox_t *self);

/**
 * @brief The queue of finished tasks of @p self (level 1).
 * @param self Mailbox; must not be null.
 * @return The queue: executors push, the owner pops.
 */
lh_exec_queue_t *
lh_exec_mailbox_get_finished_mut(lh_exec_mailbox_t *self);

/**
 * @brief How many places of @p self are taken now, read with acquire order.
 *
 * Any thread may read it; on another thread than the owner's it is a snapshot
 * that may already be out of date.
 *
 * @param self Mailbox; must not be null.
 * @return Tasks accepted for the owner and not yet dispatched.
 */
lh_uint_t
lh_exec_mailbox_get_used_count(const lh_exec_mailbox_t *self);

/**
 * @brief Make @p self an empty mailbox with room for @p capacity tasks.
 *
 * Call it before any thread can reach the mailbox; afterwards it must stay
 * where it is. A capacity of zero refuses every submission.
 *
 * @param self     Mailbox; must not be null.
 * @param capacity How many tasks may be accepted and not yet dispatched.
 */
lh_void
lh_exec_mailbox_init(lh_exec_mailbox_t *self, lh_uint_t capacity);

/**
 * @brief Take one place in @p self; any thread.
 *
 * Exact under contention: the places taken never exceed the capacity.
 *
 * @param self Mailbox; must not be null.
 * @return ::lh_bool_true when a place was taken (give it back with
 *         ::lh_exec_mailbox_release or through dispatch);
 *         ::lh_bool_false when the mailbox is full (nothing changed).
 */
lh_bool_t
lh_exec_mailbox_try_reserve(lh_exec_mailbox_t *self);

/**
 * @brief Give one taken place of @p self back; any thread.
 *
 * For a place whose task will never be posted: the executor refused it, or a
 * stop cancelled it. Dispatch gives back the places of delivered tasks itself.
 *
 * @param self Mailbox with at least one taken place; must not be null.
 */
lh_void
lh_exec_mailbox_release(lh_exec_mailbox_t *self);

/**
 * @brief Hand the finished @p task to the owner of @p self; any thread.
 *
 * The executor calls it after `work`. Everything `work` did is visible to the
 * owner when it dispatches the task.
 *
 * @param self Mailbox in which @p task holds a place; must not be null.
 * @param task Task whose work is done; must not be null. It belongs to the
 *             mailbox until it is dispatched.
 */
lh_void
lh_exec_mailbox_post(lh_exec_mailbox_t *self, lh_exec_task_t *task);

/**
 * @brief Take the oldest posted task of @p self; the owner's thread only.
 *
 * Its place stays taken: hand it to ::lh_exec_mailbox_deliver.
 *
 * @param self Mailbox; must not be null.
 * @return The task, now the owner's; or ::lh_null when none is ready (see
 *         ::lh_exec_queue_try_pop for the moment a post is half done).
 */
lh_exec_task_t *
lh_exec_mailbox_try_take(lh_exec_mailbox_t *self);

/**
 * @brief Run `complete` then `destroy` of @p task, then give its place in
 *        @p self back; the owner's thread only.
 *
 * A cancelled task (::lh_exec_task_is_cancelled) gets `destroy` only.
 *
 * @param self Mailbox the task was taken from; must not be null.
 * @param task Task from ::lh_exec_mailbox_try_take; must not be null. It may be
 *             gone afterwards (its `destroy` ran).
 */
lh_void
lh_exec_mailbox_deliver(lh_exec_mailbox_t *self, lh_exec_task_t *task);

/**
 * @brief Deliver the finished tasks of @p self; the owner's thread only.
 *
 * Takes and delivers tasks in the order they were posted. It stops when none is
 * ready, or after as many as there were places taken when it began — so a
 * `complete` that submits again cannot keep one dispatch running forever; what
 * it submits waits for the next dispatch.
 *
 * @param self Mailbox; must not be null.
 * @return How many tasks were delivered.
 */
lh_uint_t
lh_exec_mailbox_dispatch(lh_exec_mailbox_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_EXEC_MAILBOX_H */
