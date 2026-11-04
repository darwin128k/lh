/**
 * @file worker.h
 * @brief ::lh_os_sql_worker_t — asynchronous SQL: one thread, one connection, tasks in order.
 *
 * A composition of the execution part and the database: an ::lh_exec_thread_t
 * whose thread context holds one ::lh_sql_db_t. The connection is opened once,
 * on the worker's thread, when it starts (the caller's connect call chooses the
 * driver and the address), every task's `work` uses it, and it is closed on the
 * same thread after the last task — then the caller's release call frees what
 * the driver keeps per thread (`mysql_thread_end`). So a stream of queries pays
 * for one connection, not one each, and never blocks the thread that submits.
 *
 * Submit through ::lh_os_sql_worker_get_executor with ::lh_exec_executor_try_submit
 * and a mailbox: `work` runs on the worker with `local` = the worker
 * (::lh_os_sql_worker_from_local), `complete` and `destroy` on the owner's thread
 * at dispatch. A task that finds the connection lost reconnects with
 * ::lh_os_sql_worker_try_ensure_open.
 *
 * The record belongs to the caller and must not move between
 * ::lh_os_sql_worker_try_start and ::lh_os_sql_worker_destroy.
 */

#ifndef LH_OS_SQL_WORKER_H
#define LH_OS_SQL_WORKER_H

#include <lh/exec/thread.h>
#include <lh/os/sql/worker/connect/cb.h>
#include <lh/os/sql/worker/fields.h>
#include <lh/os/sql/worker/release/cb.h>

/**
 * @brief A thread executor that owns one database connection.
 */
struct lh_os_sql_worker
{
    /** Members: ::lh_os_sql_worker_fields. */
    lh_os_sql_worker_fields(lh_exec_thread_t, lh_sql_db_t, lh_os_sql_worker_connect_cb,
                            lh_os_sql_worker_release_cb, lh_ptr);
};

/** @copybrief lh_os_sql_worker */
typedef struct lh_os_sql_worker lh_os_sql_worker_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The thread executor, read-only (level 1).
 * @param self Must not be null.
 * @return Its address.
 */
const lh_exec_thread_t *
lh_os_sql_worker_get_thread(const lh_os_sql_worker_t *self);

/**
 * @brief The thread executor, for change (level 1).
 * @param self Must not be null.
 * @return Its address.
 */
lh_exec_thread_t *
lh_os_sql_worker_get_thread_mut(lh_os_sql_worker_t *self);

/**
 * @brief The connection; its thread's alone, read-only (level 1).
 * @param self Must not be null.
 * @return Its address.
 */
const lh_sql_db_t *
lh_os_sql_worker_get_db(const lh_os_sql_worker_t *self);

/**
 * @brief The connection; its thread's alone, for change (level 1).
 * @param self Must not be null.
 * @return Its address.
 */
lh_sql_db_t *
lh_os_sql_worker_get_db_mut(lh_os_sql_worker_t *self);

/**
 * @brief How the connection is opened (level 1).
 * @param self Must not be null.
 * @return The value.
 */
lh_os_sql_worker_connect_cb
lh_os_sql_worker_get_connect_cb(const lh_os_sql_worker_t *self);

/**
 * @brief Store how the connection is opened (level 1).
 * @param self  Must not be null.
 * @param value The new value.
 */
lh_void
lh_os_sql_worker_set_connect_cb(lh_os_sql_worker_t *self, lh_os_sql_worker_connect_cb value);

/**
 * @brief What the thread releases at its end (level 1).
 * @param self Must not be null.
 * @return The value.
 */
lh_os_sql_worker_release_cb
lh_os_sql_worker_get_release_cb(const lh_os_sql_worker_t *self);

/**
 * @brief Store what the thread releases at its end (level 1).
 * @param self  Must not be null.
 * @param value The new value.
 */
lh_void
lh_os_sql_worker_set_release_cb(lh_os_sql_worker_t *self, lh_os_sql_worker_release_cb value);

/**
 * @brief The pointer both calls receive (level 1).
 * @param self Must not be null.
 * @return The value.
 */
lh_ptr
lh_os_sql_worker_get_user(const lh_os_sql_worker_t *self);

/**
 * @brief Store the pointer both calls receive (level 1).
 * @param self  Must not be null.
 * @param value The new value.
 */
lh_void
lh_os_sql_worker_set_user(lh_os_sql_worker_t *self, lh_ptr value);

/**
 * @brief The worker a task's `work` runs on, from its `local`.
 * @param local The `local` a task on this worker receives.
 * @return The worker.
 */
lh_os_sql_worker_t *
lh_os_sql_worker_from_local(lh_ptr local);

/**
 * @brief Open the connection with the connect call; the worker's thread only.
 * @param self Worker; must not be null.
 * @return ::lh_bool_true when the connection is open.
 */
lh_bool_t
lh_os_sql_worker_try_connect(lh_os_sql_worker_t *self);

/**
 * @brief The worker's thread context close (an ::lh_exec_context_close_fn).
 *
 * Closes and frees the connection, then runs the release call.
 *
 * @param user  The worker.
 * @param local What open returned.
 */
lh_void
lh_os_sql_worker_close(lh_ptr user, lh_ptr local);

/**
 * @brief Fill @p self and start its thread, which connects first.
 *
 * @param self       Caller-owned record; must not be null; it must not move afterwards.
 * @param connect_cb How the connection is opened; must not be null.
 * @param release_cb What the thread releases at its end (may be ::lh_null).
 * @param user       The pointer both calls receive, borrowed; it must outlive the thread.
 * @return ::lh_bool_true when the thread runs (the connection may still have
 *         failed: tasks see it closed); ::lh_bool_false when @p connect_cb is null
 *         or the thread could not start.
 */
lh_bool_t
lh_os_sql_worker_try_start(lh_os_sql_worker_t *self, lh_os_sql_worker_connect_cb connect_cb,
                           lh_os_sql_worker_release_cb release_cb, lh_ptr user);

/**
 * @brief The worker as an executor, for submitting tasks.
 * @param self Started worker; must not be null.
 * @return The executor record; valid until ::lh_os_sql_worker_destroy.
 */
const lh_exec_executor_t *
lh_os_sql_worker_get_executor(const lh_os_sql_worker_t *self);

/**
 * @brief Ask the worker to stop and wait at most @p timeout_ms (::lh_exec_thread_try_stop).
 * @param self       Started worker; must not be null.
 * @param kind       ::lh_exec_stop_kind_drain or ::lh_exec_stop_kind_cancel.
 * @param timeout_ms Longest wait.
 * @return ::lh_bool_true when the thread has ended and the connection is closed.
 */
lh_bool_t
lh_os_sql_worker_try_stop(lh_os_sql_worker_t *self, lh_exec_stop_kind_t kind, lh_u32_t timeout_ms);

/**
 * @brief Wait for the worker's end (draining without an earlier stop) and release it.
 * @param self Started worker; must not be null.
 */
lh_void
lh_os_sql_worker_destroy(lh_os_sql_worker_t *self);

/**
 * @brief Open the connection again if it is not open; the worker's thread only.
 *
 * For a task that found it closed (a failed start, a dropped server).
 *
 * @param self Worker; must not be null.
 * @return ::lh_bool_true when the connection is open (now or already).
 */
lh_bool_t
lh_os_sql_worker_try_ensure_open(lh_os_sql_worker_t *self);

/**
 * @brief The worker's thread context open (an ::lh_exec_context_open_fn).
 *
 * Initializes the connection and connects; a failed connect leaves it closed
 * for the tasks to see.
 *
 * @param user The worker.
 * @return The worker: the `local` every task receives.
 */
lh_ptr
lh_os_sql_worker_open(lh_ptr user);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_SQL_WORKER_H */
