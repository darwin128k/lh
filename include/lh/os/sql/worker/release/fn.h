/**
 * @file fn.h
 * @brief What an SQL worker's thread releases after its last task.
 */

#ifndef LH_OS_SQL_WORKER_RELEASE_FN_H
#define LH_OS_SQL_WORKER_RELEASE_FN_H

#include <lh/core/sql/db.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @typedef lh_os_sql_worker_release_fn
 * @brief Free what the driver keeps for the worker's thread.
 *
 * Runs once on the worker's thread after the connection is closed — the place
 * for `mysql_thread_end`, which must run on the thread that used the client.
 * @p user is the worker's own pointer.
 */
typedef lh_void(lh_os_sql_worker_release_fn)(lh_ptr user);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_SQL_WORKER_RELEASE_FN_H */
