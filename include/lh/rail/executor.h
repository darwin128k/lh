/**
 * @file executor.h
 * @brief ::lh_rail_executor_t — where a task runs, behind one interface.
 *
 * An executor is a call that takes a task (`accept_cb`) and the state it works
 * on (`context`). Every kind fills the same record: the inline executor runs the
 * work at once on the caller's thread (lh/rail/inline.h); a thread executor
 * queues it for its own thread. Code that submits holds a
 * `const lh_rail_executor_t *` and does not know which kind it got — so the same
 * code runs synchronously in a test and on a thread in production.
 *
 * ::lh_rail_executor_try_submit is the one way in. It takes a place in the
 * owner's ::lh_rail_mailbox_t first, so a full owner refuses at once; then it
 * hands the task over. Either the task is accepted — the executor runs `work`,
 * the mailbox delivers `complete` and `destroy` when the owner dispatches — or
 * nothing happened and the task is still the caller's. A submission never
 * blocks the submitter.
 *
 * The record is a plain value: the kind that filled it owns `context` and says
 * how long it lives. Any thread may submit through the same record at once.
 */

#ifndef LH_RAIL_EXECUTOR_H
#define LH_RAIL_EXECUTOR_H

#include <lh/rail/executor/accept/cb.h>
#include <lh/rail/executor/fields.h>
#include <lh/rail/mailbox.h>

/**
 * @brief One way to run tasks: a call that takes them and its state.
 */
struct lh_rail_executor
{
    /** Members: ::lh_rail_executor_fields. */
    lh_rail_executor_fields(lh_rail_executor_accept_fn, lh_ptr);
};

/** @copybrief lh_rail_executor */
typedef struct lh_rail_executor lh_rail_executor_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The call that takes a task (level 1).
 * @param self Executor; must not be null.
 * @return The accept call.
 */
lh_rail_executor_accept_cb
lh_rail_executor_get_accept_cb(const lh_rail_executor_t *self);

/**
 * @brief Store the call that takes a task (level 1).
 * @param self      Executor; must not be null.
 * @param accept_cb The accept call (must not be ::lh_null when tasks are submitted).
 */
lh_void
lh_rail_executor_set_accept_cb(lh_rail_executor_t *self, lh_rail_executor_accept_cb accept_cb);

/**
 * @brief The state the accept call receives (level 1).
 * @param self Executor; must not be null.
 * @return The context (may be ::lh_null).
 */
lh_ptr
lh_rail_executor_get_context(const lh_rail_executor_t *self);

/**
 * @brief Store the state the accept call receives (level 1).
 * @param self    Executor; must not be null.
 * @param context The context, owned by the executor kind (may be ::lh_null).
 */
lh_void
lh_rail_executor_set_context(lh_rail_executor_t *self, lh_ptr context);

/**
 * @brief Fill @p self with its call and its state.
 * @param self      Executor; must not be null.
 * @param accept_cb The accept call (must not be ::lh_null when tasks are submitted).
 * @param context   The state the call receives (may be ::lh_null).
 */
lh_void
lh_rail_executor_init(lh_rail_executor_t *self, lh_rail_executor_accept_cb accept_cb,
                      lh_ptr context);

/**
 * @brief Hand @p task to the accept call of @p self, with its context.
 *
 * The bare hand-over: no mailbox place is taken here. ::lh_rail_executor_try_submit
 * is the way in for callers; this is the step it is built on.
 *
 * @param self Executor with an accept call; must not be null.
 * @param task Task already holding a place in its mailbox; must not be null.
 * @return ::lh_bool_true when the executor took the task;
 *         ::lh_bool_false when it refused it (the task is untouched).
 */
lh_bool_t
lh_rail_executor_try_accept(const lh_rail_executor_t *self, lh_rail_task_t *task);

/**
 * @brief Submit @p task to @p self, its completion to be delivered by @p mailbox.
 *
 * Takes a place in @p mailbox, records the mailbox in the task, and hands the
 * task over; when the executor refuses, gives the place back. Never blocks.
 *
 * @param self    Executor; must not be null.
 * @param task    Task filled by the caller (::lh_rail_task_init), in no queue;
 *                must not be null.
 * @param mailbox The owner's mailbox; must not be null. It must outlive the
 *                task's delivery.
 * @return ::lh_bool_true when the task was accepted: it belongs to the executor
 *         and then to @p mailbox until its `destroy` runs at dispatch;
 *         ::lh_bool_false when @p mailbox is full or the executor refused: the
 *         task is still the caller's and nothing ran.
 */
lh_bool_t
lh_rail_executor_try_submit(const lh_rail_executor_t *self, lh_rail_task_t *task,
                            lh_rail_mailbox_t *mailbox);

/**
 * @brief Submit @p task to @p self; a refusal is a failed runtime check.
 *
 * ::lh_rail_executor_try_submit followed by a check with
 * ::lh_runtime_error_code_not_accepted. For callers whose mailbox is sized so
 * that a refusal is a bug.
 *
 * @param self    Executor; must not be null.
 * @param task    Task filled by the caller, in no queue; must not be null.
 * @param mailbox The owner's mailbox; must not be null.
 */
lh_void
lh_rail_executor_submit(const lh_rail_executor_t *self, lh_rail_task_t *task,
                        lh_rail_mailbox_t *mailbox);

LH_COMPILER_EXTERN_C_END

#endif /* LH_RAIL_EXECUTOR_H */
