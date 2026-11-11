/**
 * @file keep.h
 * @brief ::lh_os_sql_keep_t — one connection a thread keeps between jobs.
 *
 * A thread that runs database jobs one after another (a task thread, a worker
 * of a pool) pays for one connection, not one per job: the first job opens it,
 * the next ones find it open and use it again. A job for another address
 * (::lh_sql_address_is_equal), or one that finds the connection lost, closes
 * it and opens its own. At the thread's end ::lh_os_sql_keep_deinit closes it
 * and lets the MySQL client go of the thread (`mysql_thread_end`) — on the
 * thread that used it, which is why the record is that thread's alone
 * (typically thread-local).
 *
 * ::lh_os_sql_worker_t is the same idea with a thread of its own; this is the
 * piece for a thread someone else runs.
 *
 * Requires ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_OS_SQL_KEEP_H
#define LH_OS_SQL_KEEP_H

#include <lh/os/sql/address.h>
#include <lh/os/sql/keep/fields.h>

/**
 * @struct lh_os_sql_keep
 * @brief Connection, its address, release flag. Fields via ::lh_os_sql_keep_fields.
 */
struct lh_os_sql_keep
{
    /** Members: ::lh_os_sql_keep_fields. */
    lh_os_sql_keep_fields(lh_sql_db_t, lh_sql_address_t, lh_bool_t);
};
/** @copybrief lh_os_sql_keep */
typedef struct lh_os_sql_keep lh_os_sql_keep_t;

LH_COMPILER_EXTERN_C_BEGIN

/** The connection, read-only (level 1); its error tells why an open failed. */
const lh_sql_db_t *
lh_os_sql_keep_get_db(const lh_os_sql_keep_t *self);

/** The connection, for use (level 1). */
lh_sql_db_t *
lh_os_sql_keep_get_db_mut(lh_os_sql_keep_t *self);

/** The address the connection was opened on (level 1); not set while none is kept. */
const lh_sql_address_t *
lh_os_sql_keep_get_address(const lh_os_sql_keep_t *self);

/** The address the connection was opened on, for change (level 1). */
lh_sql_address_t *
lh_os_sql_keep_get_address_mut(lh_os_sql_keep_t *self);

/** Whether the MySQL client was used on this thread (level 1). */
lh_bool_t
lh_os_sql_keep_needs_release(const lh_os_sql_keep_t *self);

/** Store whether the MySQL client was used on this thread (level 1). */
lh_void
lh_os_sql_keep_set_needs_release(lh_os_sql_keep_t *self, lh_bool_t needs_release);

/**
 * @brief Nothing kept yet (level 2). Allocates nothing; a zeroed record is not
 *        enough, call this first.
 */
lh_void
lh_os_sql_keep_init(lh_os_sql_keep_t *self);

/**
 * @brief Close the connection and forget its address (the password is wiped)
 *        (level 2). The record stays usable; the MySQL client is not let go.
 */
lh_void
lh_os_sql_keep_close(lh_os_sql_keep_t *self);

/**
 * @brief The connection for @p address: the kept one when it is open on that
 *        address, else a new one opened now (level 3).
 *
 * A kept connection that is used again has its last error cleared. One for
 * another address, or no longer open, is closed first.
 *
 * @param self    The record.
 * @param address Where to connect; checked by its accessors.
 * @return The connection (owned by @p self, valid until the next call or
 *         ::lh_os_sql_keep_close), or ::lh_null when the open failed: the reason
 *         is then in ::lh_os_sql_keep_get_db's error, and nothing is kept.
 */
lh_sql_db_t *
lh_os_sql_keep_try_get_db(lh_os_sql_keep_t *self, const lh_sql_address_t *address);

/**
 * @brief ::lh_os_sql_keep_close, release every storage, and let the MySQL
 *        client go of this thread when it was used here (level 3).
 *
 * Call it on the thread that used the connection. @p self may be
 * ::lh_os_sql_keep_init'ed again afterwards.
 */
lh_void
lh_os_sql_keep_deinit(lh_os_sql_keep_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_SQL_KEEP_H */
