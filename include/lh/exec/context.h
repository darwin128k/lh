/**
 * @file context.h
 * @brief ::lh_exec_context_t — state that lives as long as an executor's thread.
 *
 * Some work needs something that belongs to the thread, not to the task: a
 * database connection that must be opened, used and closed on one thread, a
 * scratch buffer reused by every task. A context says how to make it: `open`
 * runs on the executor's thread when it starts and returns that state
 * (`local`), every `work` on the thread receives it, and `close` runs on the
 * same thread after the last task.
 *
 * The record is a plain value the executor copies when it starts; `user` is
 * borrowed and must outlive the executor's thread. Both calls are optional: the
 * empty context opens nothing.
 */

#ifndef LH_EXEC_CONTEXT_H
#define LH_EXEC_CONTEXT_H

#include <lh/exec/context/close/cb.h>
#include <lh/exec/context/fields.h>
#include <lh/exec/context/open/cb.h>

/**
 * @brief How a thread opens and closes the state its tasks share.
 */
struct lh_exec_context
{
    /** Members: ::lh_exec_context_fields. */
    lh_exec_context_fields(lh_exec_context_open_fn, lh_exec_context_close_fn, lh_ptr);
};

/** @copybrief lh_exec_context */
typedef struct lh_exec_context lh_exec_context_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The open call of @p self (level 1).
 * @param self Context; must not be null.
 * @return The open call, or ::lh_null for none.
 */
lh_exec_context_open_cb
lh_exec_context_get_open_cb(const lh_exec_context_t *self);

/**
 * @brief Store the open call of @p self (level 1).
 * @param self    Context; must not be null.
 * @param open_cb The open call (may be ::lh_null).
 */
lh_void
lh_exec_context_set_open_cb(lh_exec_context_t *self, lh_exec_context_open_cb open_cb);

/**
 * @brief The close call of @p self (level 1).
 * @param self Context; must not be null.
 * @return The close call, or ::lh_null for none.
 */
lh_exec_context_close_cb
lh_exec_context_get_close_cb(const lh_exec_context_t *self);

/**
 * @brief Store the close call of @p self (level 1).
 * @param self     Context; must not be null.
 * @param close_cb The close call (may be ::lh_null).
 */
lh_void
lh_exec_context_set_close_cb(lh_exec_context_t *self, lh_exec_context_close_cb close_cb);

/**
 * @brief The pointer both calls of @p self receive (level 1).
 * @param self Context; must not be null.
 * @return The user pointer (may be ::lh_null).
 */
lh_ptr
lh_exec_context_get_user(const lh_exec_context_t *self);

/**
 * @brief Store the pointer both calls of @p self receive (level 1).
 * @param self Context; must not be null.
 * @param user The user pointer, borrowed (may be ::lh_null).
 */
lh_void
lh_exec_context_set_user(lh_exec_context_t *self, lh_ptr user);

/**
 * @brief Fill @p self with its calls and their pointer.
 * @param self     Context; must not be null.
 * @param open_cb  The open call (may be ::lh_null).
 * @param close_cb The close call (may be ::lh_null).
 * @param user     The pointer both calls receive, borrowed (may be ::lh_null).
 */
lh_void
lh_exec_context_init(lh_exec_context_t *self, lh_exec_context_open_cb open_cb,
                     lh_exec_context_close_cb close_cb, lh_ptr user);

/**
 * @brief Run the open call of @p self, if it has one.
 * @param self Context; must not be null.
 * @return What open returned: the thread's `local`; ::lh_null without an open
 *         call.
 */
lh_ptr
lh_exec_context_run_open(const lh_exec_context_t *self);

/**
 * @brief Run the close call of @p self with @p local, if it has one.
 * @param self  Context; must not be null.
 * @param local What ::lh_exec_context_run_open returned on this thread.
 */
lh_void
lh_exec_context_run_close(const lh_exec_context_t *self, lh_ptr local);

/**
 * @brief Make @p self a copy of @p other.
 * @param self  Context to fill; must not be null.
 * @param other Context to copy; must not be null.
 */
lh_void
lh_exec_context_assign(lh_exec_context_t *self, const lh_exec_context_t *other);

LH_COMPILER_EXTERN_C_END

#endif /* LH_EXEC_CONTEXT_H */
