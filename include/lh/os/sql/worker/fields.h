/**
 * @file fields.h
 * @brief Member fields of ::lh_os_sql_worker_t.
 */

#ifndef LH_OS_SQL_WORKER_FIELDS_H
#define LH_OS_SQL_WORKER_FIELDS_H

/**
 * @def lh_os_sql_worker_fields(thread_type, db_type, connect_cb_type, release_cb_type, user_type)
 * @brief The thread executor, its connection, and how the connection is made and let go.
 *
 * @param thread_type     ::lh_exec_thread_t: `thread`.
 * @param db_type         ::lh_sql_db_t: `db`, used on the worker's thread only.
 * @param connect_cb_type ::lh_os_sql_worker_connect_cb: `connect_cb`.
 * @param release_cb_type ::lh_os_sql_worker_release_cb: `release_cb` (may be null).
 * @param user_type       ::lh_ptr: `user`, given to both.
 */
#define lh_os_sql_worker_fields(thread_type, db_type, connect_cb_type, release_cb_type, user_type) \
    thread_type thread;                                                                            \
    db_type db;                                                                                    \
    connect_cb_type connect_cb;                                                                    \
    release_cb_type release_cb;                                                                    \
    user_type user

#endif /* LH_OS_SQL_WORKER_FIELDS_H */
