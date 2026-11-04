/**
 * @file fn.h
 * @brief How an SQL worker opens its connection.
 */

#ifndef LH_OS_SQL_WORKER_CONNECT_FN_H
#define LH_OS_SQL_WORKER_CONNECT_FN_H

#include <lh/core/sql/db.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @typedef lh_os_sql_worker_connect_fn
 * @brief Open @p db: the driver and the address are the caller's choice.
 *
 * Runs on the worker's thread: when it starts, and again when a task finds the
 * connection closed (::lh_os_sql_worker_try_ensure_open). @p db is initialized
 * and closed; open it with a backend (`lh_os_sql_sqlite_open`,
 * `lh_os_sql_mysql_open`, ...). @p user is the worker's own pointer.
 * Returns whether @p db is open; on failure the backend's error stays in @p db.
 */
typedef lh_bool_t(lh_os_sql_worker_connect_fn)(lh_ptr user, lh_sql_db_t *db);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_SQL_WORKER_CONNECT_FN_H */
